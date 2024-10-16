#pragma once

#include "../game/q_shared.h"
#include "../qcommon/qcommon.h"

void LLM_Init(void);
extern char* FS_BuildOSPath(const char* base, const char* game, const char* qpath);
void LLM_PushPrompt(int chatstate, const char* prompt);
void LLM_SendChatMessages(void);