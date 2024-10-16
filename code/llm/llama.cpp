#include <atomic>
#include <deque>
#include <thread>
#include <functional>
#include <mutex>
#include <vector>
#include <string>
#include <iostream>

#include "llama.h"
#include "sampling.h"

extern "C" {
#include "llm.h"

void EA_Command(int client, char* command);
};

using namespace std;

llama_model* model = nullptr;
llama_context* ctx = nullptr;

// Atomic flag for stopping the thread
std::atomic<bool> keepRunning(true);

// Mutex and condition variable for synchronizing access to the prompt queue
std::mutex queueMutex;
std::mutex uploadMutex;
struct PromptQueueItem_t {
    int chatstate;
    std::string prompt;
};
std::deque<PromptQueueItem_t> promptQueue;
std::deque<PromptQueueItem_t> readyQueue;
std::condition_variable promptNotifier;

void LLM_ResponseThread();

static bool eval_tokens(struct llama_context* ctx_llama, std::vector<llama_token> tokens, int n_batch, int* n_past) {
    int N = (int)tokens.size();
    for (int i = 0; i < N; i += n_batch) {
        int n_eval = (int)tokens.size() - i;
        if (n_eval > n_batch) {
            n_eval = n_batch;
        }
        int ret = llama_decode(ctx_llama, llama_batch_get_one(&tokens[i], n_eval, *n_past, 0));
        if (ret) {
            Com_Printf("%s : failed to eval. token %d/%d (batch size %d, n_past %d)\n", __func__, i, N, n_batch, *n_past);
            return false;
        }
        *n_past += n_eval;
    }
    return true;
}

/*
=====================
Sys_DLL_Load
=====================
*/
llama_model* LLM_Load(const char* dllName) {

    char* basepath;
    char* cdpath;
    char* gamedir;
    char* fn;

    basepath = Cvar_VariableString("fs_basepath");
    cdpath = Cvar_VariableString("fs_cdpath");
    gamedir = Cvar_VariableString("fs_game");

    fn = FS_BuildOSPath(basepath, gamedir, dllName);
    struct llama_model_params lparams = {
        /*.n_gpu_layers                =*/ 0,
        /*.split_mode                  =*/ LLAMA_SPLIT_MODE_LAYER,
        /*.main_gpu                    =*/ 0,
        /*.tensor_split                =*/ nullptr,
        /*.rpc_servers                 =*/ nullptr,
        /*.progress_callback           =*/ nullptr,
        /*.progress_callback_user_data =*/ nullptr,
        /*.kv_overrides                =*/ nullptr,
        /*.vocab_only                  =*/ false,
        /*.use_mmap                    =*/ true,
        /*.use_mlock                   =*/ false,
        /*.check_tensors               =*/ false,
    };

    // -eps 1e-5 -t 8 -ngl 50
    lparams.n_gpu_layers = 100;
    //lparams.n_ctx = 4096;
    lparams.main_gpu = 1;

    Com_Printf("LLM_Load: Init backend and numa...\n");
    llama_backend_init();
    ggml_backend_init_best();
    llama_numa_init(GGML_NUMA_STRATEGY_DISTRIBUTE);

    Com_Printf("LLM_Load: Loading... %s\n", fn);
    model = llama_load_model_from_file(fn, lparams);  // replace with actual parameters

    if (model)
        Com_Printf("LLM_Load: ok\n");
    else
        Com_Printf("LLM_Load: failed\n");

    return model;
}

void LLM_Init(void) {
    Com_Printf("------ LLM_Init ------\n");
    model = LLM_Load("aimodels/Meta-Llama-3.1-70B-Instruct-IQ2_XS.gguf");

    Com_Printf("Creating llama context...\n");
    llama_context_params ctx_params = llama_context_default_params();
    ctx = llama_new_context_with_model(model, ctx_params);
    if (ctx == NULL) {
        Com_Error(ERR_FATAL, "Failed to create llama context...\n");
        return;
    }

    // Start the response thread
    static std::thread responseThread(LLM_ResponseThread);
    LLM_PushPrompt(-1, "You are a angry 18 year old playing a first person shooter game.\n");
}

std::vector<llama_token> llama_tokenize2(
    const struct llama_model* model,
    const std::string& text,
    bool   add_special,
    bool   parse_special) {
    // upper limit for the number of tokens
    int n_tokens = text.length() + 2 * add_special;
    std::vector<llama_token> result(n_tokens);
    n_tokens = llama_tokenize(model, text.data(), text.length(), result.data(), result.size(), add_special, parse_special);
    if (n_tokens < 0) {
        result.resize(-n_tokens);
        int check = llama_tokenize(model, text.data(), text.length(), result.data(), result.size(), add_special, parse_special);
        GGML_ASSERT(check == -n_tokens);
    }
    else {
        result.resize(n_tokens);
    }
    return result;
}

static bool eval_string(struct llama_context* ctx_llama, const char* str, int n_batch, int* n_past, bool add_bos) {
    std::string              str2 = str;
    std::vector<llama_token> embd_inp = llama_tokenize2(model, str2, add_bos, true);
    eval_tokens(ctx_llama, embd_inp, n_batch, n_past);
    return true;
}

static bool eval_id(struct llama_context* ctx_llama, int id, int* n_past) {
    std::vector<llama_token> tokens;
    tokens.push_back(id);
    return eval_tokens(ctx_llama, tokens, 1, n_past);
}

static const char* sample(struct gpt_sampler* smpl,
    struct llama_context* ctx_llama,
    int* n_past) {
    const llama_token id = gpt_sampler_sample(smpl, ctx_llama, -1);
    gpt_sampler_accept(smpl, id, true);
    static std::string ret;
    if (llama_token_is_eog(llama_get_model(ctx_llama), id)) {
        ret = "</s>";
    }
    else {
        ret = llama_token_to_piece(ctx_llama, id);
    }
    eval_id(ctx_llama, id, n_past);
    return ret.c_str();
}

/*
=====================
LLM_ResponseThread
Runs in a separate thread, waits for prompts, and generates Llama responses.
=====================
*/
void LLM_ResponseThread() {
    int n_past = 0;
    int n_first = -1;

    while (keepRunning) {
        std::unique_lock<std::mutex> lock(queueMutex);
        promptNotifier.wait(lock, [] { return !promptQueue.empty() || !keepRunning; });

        if (!keepRunning) break;

        // Get the next prompt
        PromptQueueItem_t queue = promptQueue.front();
        promptQueue.pop_front();
        lock.unlock();

       // if(n_first != -1)
       //     n_past = n_first;

        // Generate response using llama (simplified llama call for illustration)
        if (ctx != nullptr) {
           // Com_Printf("Generating response for: %s\n", prompt.c_str());

            if (!eval_string(ctx, queue.prompt.c_str(), queue.prompt.size(), &n_past, false))
                continue;

            // generate the response
            gpt_sampler_params sparams;
            sparams.seed = rand() % LLAMA_DEFAULT_SEED;
            struct gpt_sampler* smpl = gpt_sampler_init(model, sparams);
            if (!smpl) {
                Com_Error(ERR_FATAL, "%s: failed to initialize sampling subsystem\n", __func__);
            }
            
            const int max_tgt_len = 50;

            std::string response = "";
            for (int i = 0; i < max_tgt_len; i++) {
                const char* tmp = sample(smpl, ctx, &n_past);   
                n_past++; // Increment n_past for the next token

                if (strcmp(tmp, "</s>") == 0) break;
                if (strstr(tmp, "###")) break; // Yi-VL behavior
                if (strstr(tmp, "<|im_end|>")) break; // Yi-34B llava-1.6 - for some reason those decode not as the correct token (tokenizer works)
                if (strstr(tmp, "<|im_start|>")) break; // Yi-34B llava-1.6
                if (strstr(tmp, "USER:")) break; // mistral llava-1.6
                if (strstr(tmp, "(")) break; // mistral llava-1.6
                if (strstr(tmp, "-")) break; // mistral llava-1.6
 
                response += tmp;

                // Check if the response ends with sentence-ending punctuation
                if (response.back() == '.' || response.back() == '!' || response.back() == '?' || response.back() == '\n') {
                    break;
                }                
            }

            gpt_sampler_free(smpl);

            llama_kv_cache_clear(ctx);

          //  if (n_first == -1)
          //      n_first = n_past;

            std::lock_guard<std::mutex> lock(uploadMutex);
            {
                PromptQueueItem_t chatqueue;
                chatqueue.chatstate = queue.chatstate;
                chatqueue.prompt = response;
                readyQueue.push_back(chatqueue);
            }
        }
    }
}

/*
=====================
LLM_SendChatMessages
=====================
*/
void LLM_SendChatMessages(void) {    
    if (readyQueue.size() <= 0)
    {
        return;
    }

    std::unique_lock<std::mutex> lock(uploadMutex);
    PromptQueueItem_t queue = readyQueue.front();
    readyQueue.pop_front();
    lock.unlock();

    if(queue.chatstate != -1)
        EA_Command(queue.chatstate, va("say %s", queue.prompt.c_str()));
}

/*
=====================
LLM_PushPrompt
Push a new prompt to the prompt queue, to be handled by the LLM_ResponseThread.
=====================
*/
void LLM_PushPrompt(int chatstate, const char *prompt) {
    std::lock_guard<std::mutex> lock(queueMutex);
    PromptQueueItem_t queue;
    queue.chatstate = chatstate;
    queue.prompt = prompt;
    promptQueue.push_back(queue);
    promptNotifier.notify_one();  // Notify the LLM_ResponseThread
}

/*
=====================
LLM_Shutdown
Shuts down the LLM thread and cleans up resources.
=====================
*/
void LLM_Shutdown() {
    keepRunning = false;
    promptNotifier.notify_all();  // Wake up the thread to exit

    // Clean up the Llama context and model
    if (ctx != nullptr) {
        //llama_free_context(ctx);
        ctx = nullptr;
    }

    if (model != nullptr) {
        llama_free_model(model);
        model = nullptr;
    }
}