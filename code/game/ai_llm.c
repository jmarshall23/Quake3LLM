#include "g_local.h"
#include "botlib.h"
#include "be_aas.h"
#include "be_ea.h"
#include "be_ai_char.h"
#include "be_ai_chat.h"
#include "be_ai_gen.h"
#include "be_ai_goal.h"
#include "be_ai_move.h"
#include "be_ai_weap.h"
//
#include "ai_main.h"
#include "ai_dmq3.h"
#include "ai_chat.h"
#include "ai_cmd.h"
#include "ai_dmnet.h"
#include "ai_team.h"
//
#include "chars.h"				//characteristics
#include "inv.h"				//indexes into the inventory
#include "syn.h"				//synonyms
#include "match.h"				//string matching types and vars

// for the voice chats
#include "../ui/menudef.h" // sos001205 - for q3_ui also

/*
==================
BotAI_BotInitialChat
==================
*/
void QDECL BotAI_BotInitialChat(bot_state_t* bs, char* type, ...) {
    int i;
    va_list ap;
    char* p;
    char* vars[MAX_MATCHVARIABLES];

    // Clear the variables
    memset(vars, 0, sizeof(vars));

    // Start variadic argument handling
    va_start(ap, type);
    p = va_arg(ap, char*);
    for (i = 0; i < MAX_MATCHVARIABLES; i++) {
        if (!p || strcmp(p, "[invalid var]") == 0) {
            vars[i] = NULL;
        }
        else {
            vars[i] = p;
            p = va_arg(ap, char*);
        }
    }
    va_end(ap);

    // Create the prompt based on the chat type and use vargs for dynamic content
    if (strcmp(type, "game_enter") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just entered the first person shooter game. Trash-talk your opponents about how you are going to fuck them up, and how big and bad you are. Make it a trolling response.\n",
            vars[0] ? vars[0] : "an opponent", vars[1] ? vars[1] : "something random"
        ));
    }
    else if (strcmp(type, "game_exit") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You're about to leave the first person shooter game. Your last words to %s are: %s. What do you say?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "an opponent", vars[1] ? vars[1] : "a final taunt"
        ));
    }
    else if (strcmp(type, "level_start") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "The level just started, and you're in the first person shooter game. You see %s and you taunt them with: %s.  Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent", vars[1] ? vars[1] : "something random"
        ));
    }
    else if (strcmp(type, "level_end_victory") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just won the level in a first person shooter. %s and %s lost to you. Mock them with: %s.  Make it a trolling response.\n",
            vars[0] ? vars[0] : "The first player in rankings",
            vars[1] ? vars[1] : "the last player in rankings",
            vars[2] ? vars[2] : "a victory taunt"
        ));
    }
    else if (strcmp(type, "level_end_lose") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just lost the level, in a first person shooter, you taunt %s and %s. What do you say?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "the winner", vars[1] ? vars[1] : "another player"
        ));
    }
    else if (strcmp(type, "death_drown") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just drowned in a first person shooter, you blame %s for your drowning. What do you say?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "someone"
        ));
    }
    else if (strcmp(type, "death_slime") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just died by falling into slime in a first person shooter, you blame %s for your death. What do you say?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "someone"
        ));
    }
    else if (strcmp(type, "death_lava") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just died by falling into lava in a first person shooter, you blame %s for your death. What do you say?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "someone"
        ));
    }
    else if (strcmp(type, "death_cratered") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just cratered (died by falling from a height) in a first person shooter, you sarcastically comment to %s. What do you say?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "someone"
        ));
    }
    else if (strcmp(type, "death_suicide") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just committed suicide in the game in a first person shooter, you sarcastically comment to %s. What do you say?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "someone"
        ));
    }
    else if (strcmp(type, "death_telefrag") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just got telefragged in a first person shooter, you blame %s. What do you say?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "someone"
        ));
    }
    else if (strcmp(type, "death_gauntlet") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just got killed by a gauntlet  in a first person shooter You blame %s for using %s. What do you say?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent", vars[1] ? vars[1] : "the gauntlet"
        ));
    }
    else if (strcmp(type, "death_rail") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just got killed by a railgun  in a first person shooter You blame %s for sniping you with %s. What do you say? Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent", vars[1] ? vars[1] : "the railgun"
        ));
    }
    else if (strcmp(type, "death_bfg") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just got killed by the BFG in a first person shooter You mock %s for using %s. What do you say?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent", vars[1] ? vars[1] : "the BFG"
        ));
    }
    else if (strcmp(type, "kill_gauntlet") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            " You just killed %s with a gauntlet in a first person shooter How do you mock them?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent"
        ));
    }
    else if (strcmp(type, "kill_rail") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just sniped %s with a railgunin a first person shooter How do you insult them?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent"
        ));
    }
    else if (strcmp(type, "kill_telefrag") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just telefragged %s in a first person shooter How do you mock them?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent"
        ));
    }
    else if (strcmp(type, "kill_insult") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just killed %s in a first person shooter What's your insulting comment?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent"
        ));
    }
    else if (strcmp(type, "enemy_suicide") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "Your enemy %s just committed suicide in a first person shooter How do you mock them? Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent"
        ));
    }
    else if (strcmp(type, "random_misc") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You're trash-talking %s on %s with %s in a first person shooter What's your random insult? Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent", vars[4] ? vars[4] : "this map", vars[5] ? vars[5] : "a random weapon"
        ));
    }
    else if (strcmp(type, "hit_talking") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just got hit by %s with %s while they were talking in a first person shooter How do you respond? Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent", vars[1] ? vars[1] : "their weapon"
        ));
    }
    else if (strcmp(type, "hit_nodeath") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just got hit by %s with %s but didn't die in a first person shooter How do you mock them?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent", vars[1] ? vars[1] : "their weapon"
        ));
    }
    else if (strcmp(type, "hit_nokill") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "You hit %s but failed to kill them with %s in a first person shooter How do you respond?  Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent", vars[1] ? vars[1] : "your weapon"
        ));
    }
    else if (strcmp(type, "random_insult") == 0) {
        trap_LLM_PushPrompt(bs->client, va(
            "Trash-talk your opponents about how great you are and how much you are going to enjoy killing them.  Make it a trolling response.\n",
            vars[0] ? vars[0] : "your opponent", vars[1] ? vars[1] : "your weapon", vars[4] ? vars[4] : "this map"
        ));
    }
    else if (strstr(type, "death_")) {
        trap_LLM_PushPrompt(bs->client, va(
            "You just got killed by %s using %s. What do you say? Make it a trolling response.\n",
            vars[0] ? vars[0] : "someone", vars[1] ? vars[1] : "a gun"
        ));
        }
    // Handle any other case if needed.
    else {
        trap_LLM_PushPrompt(bs->client,
            "You're playing a game as a raging 18-year-old in a first person shooter What's your trash-talk or trolling comment in this situation?  Make it a trolling response.\n");
            }
}
