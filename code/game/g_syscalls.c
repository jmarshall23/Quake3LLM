/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
//
#include "g_local.h"

gameImport_t* gameSyscall;
gameExport_t* game;

void G_InitGame(int levelTime, int randomSeed, int restart);
void G_ShutdownGame(int restart);
void G_RunFrame(int levelTime);

gameExport_t *dllEntry(gameImport_t* gameAPI) {
    static gameExport_t exports;

    exports.G_InitGame = G_InitGame;
    exports.G_ShutdownGame = G_ShutdownGame;
    exports.ClientConnect = ClientConnect;
    exports.ClientThink = ClientThink;
    exports.ClientUserinfoChanged = ClientUserinfoChanged;
    exports.ClientDisconnect = ClientDisconnect;
    exports.ClientBegin = ClientBegin;
    exports.ClientCommand = ClientCommand;
    exports.G_RunFrame = G_RunFrame;
    exports.ConsoleCommand = ConsoleCommand;
    exports.BotAIStartFrame = BotAIStartFrame;

    gameSyscall = gameAPI;
    game = &exports;

    return &exports;
}
void trap_Printf(const char* fmt) {
    gameSyscall->Print(fmt);
}

void trap_Error(const char* fmt) {
    gameSyscall->Error(ERR_DROP, fmt);
}

int trap_Milliseconds(void) {
    return gameSyscall->Milliseconds();
}

void trap_Cvar_Register(vmCvar_t* cvar, const char* var_name, const char* value, int flags) {
    gameSyscall->Cvar_Register(cvar, var_name, value, flags);
}

void trap_Cvar_Update(vmCvar_t* cvar) {
    gameSyscall->Cvar_Update(cvar);
}

void trap_Cvar_Set(const char* var_name, const char* value) {
    gameSyscall->Cvar_Set(var_name, value);
}

int trap_Cvar_VariableIntegerValue(const char* var_name) {
    return gameSyscall->Cvar_VariableIntegerValue(var_name);
}

void trap_Cvar_VariableStringBuffer(const char* var_name, char* buffer, int bufsize) {
    gameSyscall->Cvar_VariableStringBuffer(var_name, buffer, bufsize);
}

int trap_Argc(void) {
    return gameSyscall->Argc();
}

void trap_Argv(int n, char* buffer, int bufferLength) {
    gameSyscall->Argv(n, buffer, bufferLength);
}

void trap_SendConsoleCommand(int exec_when, const char* text) {
    gameSyscall->SendConsoleCommand(exec_when, text);
}

int trap_FS_FOpenFile(const char* qpath, fileHandle_t* f, fsMode_t mode) {
    return gameSyscall->FS_FOpenFile(qpath, f, mode);
}

void trap_FS_Read(void* buffer, int len, fileHandle_t f) {
    gameSyscall->FS_Read(buffer, len, f);
}

void trap_FS_Write(const void* buffer, int len, fileHandle_t f) {
    gameSyscall->FS_Write(buffer, len, f);
}

void trap_FS_FCloseFile(fileHandle_t f) {
    gameSyscall->FS_FCloseFile(f);
}

int trap_FS_GetFileList(const char* path, const char* extension, char* listbuf, int bufsize) {
    return gameSyscall->FS_GetFileList(path, extension, listbuf, bufsize);
}

int trap_FS_Seek(fileHandle_t f, long offset, int origin) {
    return gameSyscall->FS_Seek(f, offset, origin);
}

void trap_LocateGameData(gentity_t* gEnts, int numGEntities, int sizeofGEntity_t, playerState_t* clients, int sizeofGClient) {
    gameSyscall->LocateGameData(gEnts, numGEntities, sizeofGEntity_t, clients, sizeofGClient);
}

void trap_DropClient(int clientNum, const char* reason) {
    gameSyscall->DropClient(clientNum, reason);
}

void trap_SendServerCommand(int clientNum, const char* text) {
    gameSyscall->SendServerCommand(clientNum, text);
}

void trap_SetConfigstring(int num, const char* string) {
    gameSyscall->SetConfigstring(num, string);
}

void trap_GetConfigstring(int num, char* buffer, int bufferSize) {
    gameSyscall->GetConfigstring(num, buffer, bufferSize);
}

void trap_GetUserinfo(int num, char* buffer, int bufferSize) {
    gameSyscall->GetUserinfo(num, buffer, bufferSize);
}

void trap_SetUserinfo(int num, const char* buffer) {
    gameSyscall->SetUserinfo(num, buffer);
}

void trap_GetServerinfo(char* buffer, int bufferSize) {
    gameSyscall->GetServerinfo(buffer, bufferSize);
}

void trap_SetBrushModel(gentity_t* ent, const char* name) {
    gameSyscall->SetBrushModel(ent, name);
}

void trap_Trace(trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask) {
    gameSyscall->Trace(results, start, mins, maxs, end, passEntityNum, contentmask, qfalse);
}

void trap_TraceCapsule(trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask) {
    gameSyscall->Trace(results, start, mins, maxs, end, passEntityNum, contentmask, qtrue);
}

int trap_PointContents(const vec3_t point, int passEntityNum) {
    return gameSyscall->PointContents(point, passEntityNum);
}

qboolean trap_InPVS(const vec3_t p1, const vec3_t p2) {
    return gameSyscall->inPVS(p1, p2);
}

qboolean trap_InPVSIgnorePortals(const vec3_t p1, const vec3_t p2) {
    return gameSyscall->inPVSIgnorePortals(p1, p2);
}

void trap_AdjustAreaPortalState(gentity_t* ent, qboolean open) {
    gameSyscall->AdjustAreaPortalState(ent, open);
}

qboolean trap_AreasConnected(int area1, int area2) {
    return gameSyscall->AreasConnected(area1, area2);
}

void trap_LinkEntity(gentity_t* ent) {
    gameSyscall->LinkEntity(ent);
}

void trap_UnlinkEntity(gentity_t* ent) {
    gameSyscall->UnlinkEntity(ent);
}

int trap_EntitiesInBox(const vec3_t mins, const vec3_t maxs, int* list, int maxcount) {
    return gameSyscall->EntitiesInBox(mins, maxs, list, maxcount);
}

qboolean trap_EntityContact(const vec3_t mins, const vec3_t maxs, const gentity_t* ent) {
    return gameSyscall->EntityContact(mins, maxs, ent, qfalse);
}

qboolean trap_EntityContactCapsule(const vec3_t mins, const vec3_t maxs, const gentity_t* ent) {
    return gameSyscall->EntityContact(mins, maxs, ent, qtrue);
}

int trap_BotAllocateClient(void) {
    return gameSyscall->BotAllocateClient();
}

void trap_BotFreeClient(int clientNum) {
    gameSyscall->BotFreeClient(clientNum);
}

void trap_GetUsercmd(int clientNum, usercmd_t* cmd) {
    gameSyscall->GetUsercmd(clientNum, cmd);
}

qboolean trap_GetEntityToken(char* buffer, int bufferSize) {
    return gameSyscall->GetEntityToken(buffer, bufferSize);
}

int trap_DebugPolygonCreate(int color, int numPoints, vec3_t* points) {
    return gameSyscall->DebugPolygonCreate(color, numPoints, points);
}

void trap_DebugPolygonDelete(int id) {
    gameSyscall->DebugPolygonDelete(id);
}

int trap_RealTime(qtime_t* qtime) {
    return gameSyscall->RealTime(qtime);
}

// BotLib traps start here
int trap_BotLibSetup(void) {
    return gameSyscall->BotLibSetup();
}

int trap_BotLibShutdown(void) {
    return gameSyscall->BotLibShutdown();
}

int trap_BotLibVarSet(char* var_name, char* value) {
    return gameSyscall->BotLibVarSet(var_name, value);
}

int trap_BotLibVarGet(char* var_name, char* value, int size) {
    return gameSyscall->BotLibVarGet(var_name, value, size);
}

int trap_BotLibDefine(char* string) {
    return gameSyscall->BotLibDefine(string);
}

int trap_BotLibStartFrame(float time) {
    return gameSyscall->BotLibStartFrame(time);
}

int trap_BotLibLoadMap(const char* mapname) {
    return gameSyscall->BotLibLoadMap(mapname);
}

int trap_BotLibUpdateEntity(int ent, void* bue) {
    return gameSyscall->BotLibUpdateEntity(ent, bue);
}

int trap_BotLibTest(int parm0, char* parm1, vec3_t parm2, vec3_t parm3) {
    return gameSyscall->BotLibTest(parm0, parm1, parm2, parm3);
}

int trap_BotGetSnapshotEntity(int clientNum, int sequence) {
    return gameSyscall->BotGetSnapshotEntity(clientNum, sequence);
}

int trap_BotGetServerCommand(int clientNum, char* message, int size) {
    return gameSyscall->BotGetServerCommand(clientNum, message, size);
}

void trap_BotUserCommand(int clientNum, usercmd_t* ucmd) {
    gameSyscall->BotUserCommand(clientNum, ucmd);
}

void trap_AAS_EntityInfo(int entnum, void* info) {
    gameSyscall->AAS_EntityInfo(entnum, info);
}

int trap_AAS_Initialized(void) {
    return gameSyscall->AAS_Initialized();
}

void trap_AAS_PresenceTypeBoundingBox(int presencetype, vec3_t mins, vec3_t maxs) {
    gameSyscall->AAS_PresenceTypeBoundingBox(presencetype, mins, maxs);
}

float trap_AAS_Time(void) {
    return gameSyscall->AAS_Time();
}

int trap_AAS_PointAreaNum(vec3_t point) {
    return gameSyscall->AAS_PointAreaNum(point);
}

int trap_AAS_PointReachabilityAreaIndex(vec3_t point) {
    return gameSyscall->AAS_PointReachabilityAreaIndex(point);
}

int trap_AAS_TraceAreas(vec3_t start, vec3_t end, int* areas, vec3_t* points, int maxareas) {
    return gameSyscall->AAS_TraceAreas(start, end, areas, points, maxareas);
}

int trap_AAS_BBoxAreas(vec3_t absmins, vec3_t absmaxs, int* areas, int maxareas) {
    return gameSyscall->AAS_BBoxAreas(absmins, absmaxs, areas, maxareas);
}

int trap_AAS_AreaInfo(int areanum, void* info) {
    return gameSyscall->AAS_AreaInfo(areanum, info);
}

int trap_AAS_PointContents(vec3_t point) {
    return gameSyscall->AAS_PointContents(point);
}

int trap_AAS_NextBSPEntity(int ent) {
    return gameSyscall->AAS_NextBSPEntity(ent);
}

int trap_AAS_ValueForBSPEpairKey(int ent, char* key, char* value, int size) {
    return gameSyscall->AAS_ValueForBSPEpairKey(ent, key, value, size);
}

int trap_AAS_VectorForBSPEpairKey(int ent, char* key, vec3_t v) {
    return gameSyscall->AAS_VectorForBSPEpairKey(ent, key, v);
}

int trap_AAS_FloatForBSPEpairKey(int ent, char* key, float* value) {
    return gameSyscall->AAS_FloatForBSPEpairKey(ent, key, value);
}

int trap_AAS_IntForBSPEpairKey(int ent, char* key, int* value) {
    return gameSyscall->AAS_IntForBSPEpairKey(ent, key, value);
}

int trap_AAS_AreaReachability(int areanum) {
    return gameSyscall->AAS_AreaReachability(areanum);
}

int trap_AAS_AreaTravelTimeToGoalArea(int areanum, vec3_t origin, int goalareanum, int travelflags) {
    return gameSyscall->AAS_AreaTravelTimeToGoalArea(areanum, origin, goalareanum, travelflags);
}

int trap_AAS_EnableRoutingArea(int areanum, int enable) {
    return gameSyscall->AAS_EnableRoutingArea(areanum, enable);
}

int trap_AAS_PredictRoute(void* route, int areanum, vec3_t origin, int goalareanum, int travelflags, int maxareas, int maxtime, int stopevent, int stopcontents, int stoptfl, int stopareanum) {
    return gameSyscall->AAS_PredictRoute(route, areanum, origin, goalareanum, travelflags, maxareas, maxtime, stopevent, stopcontents, stoptfl, stopareanum);
}

int trap_AAS_AlternativeRouteGoals(vec3_t start, int startareanum, vec3_t goal, int goalareanum, int travelflags, void* altroutegoals, int maxaltroutegoals, int type) {
    return gameSyscall->AAS_AlternativeRouteGoals(start, startareanum, goal, goalareanum, travelflags, altroutegoals, maxaltroutegoals, type);
}

int trap_AAS_Swimming(vec3_t origin) {
    return gameSyscall->AAS_Swimming(origin);
}

int trap_AAS_PredictClientMovement(void* move, int entnum, vec3_t origin, int presencetype, int onground, vec3_t velocity, vec3_t cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize) {
    return gameSyscall->AAS_PredictClientMovement(move, entnum, origin, presencetype, onground, velocity, cmdmove, cmdframes, maxframes, frametime, stopevent, stopareanum, visualize);
}

void trap_EA_Say(int client, char* str) {
    gameSyscall->EA_Say(client, str);
}

void trap_EA_SayTeam(int client, char* str) {
    gameSyscall->EA_SayTeam(client, str);
}

void trap_EA_Command(int client, char* command) {
    gameSyscall->EA_Command(client, command);
}

void trap_EA_Action(int client, int action) {
    gameSyscall->EA_Action(client, action);
}

void trap_EA_Gesture(int client) {
    gameSyscall->EA_Gesture(client);
}

void trap_EA_Talk(int client) {
    gameSyscall->EA_Talk(client);
}

void trap_EA_Attack(int client) {
    gameSyscall->EA_Attack(client);
}

void trap_EA_Use(int client) {
    gameSyscall->EA_Use(client);
}

void trap_EA_Respawn(int client) {
    gameSyscall->EA_Respawn(client);
}

void trap_EA_Crouch(int client) {
    gameSyscall->EA_Crouch(client);
}

void trap_EA_MoveUp(int client) {
    gameSyscall->EA_MoveUp(client);
}

void trap_EA_MoveDown(int client) {
    gameSyscall->EA_MoveDown(client);
}

void trap_EA_MoveForward(int client) {
    gameSyscall->EA_MoveForward(client);
}

void trap_EA_MoveBack(int client) {
    gameSyscall->EA_MoveBack(client);
}

void trap_EA_MoveLeft(int client) {
    gameSyscall->EA_MoveLeft(client);
}

void trap_EA_MoveRight(int client) {
    gameSyscall->EA_MoveRight(client);
}

void trap_EA_SelectWeapon(int client, int weapon) {
    gameSyscall->EA_SelectWeapon(client, weapon);
}

void trap_EA_Jump(int client) {
    gameSyscall->EA_Jump(client);
}

void trap_EA_DelayedJump(int client) {
    gameSyscall->EA_DelayedJump(client);
}

void trap_EA_Move(int client, vec3_t dir, float speed) {
    gameSyscall->EA_Move(client, dir, speed);
}

void trap_EA_View(int client, vec3_t viewangles) {
    gameSyscall->EA_View(client, viewangles);
}

void trap_EA_EndRegular(int client, float thinktime) {
    gameSyscall->EA_EndRegular(client, thinktime);
}

void trap_EA_GetInput(int client, float thinktime, void* input) {
    gameSyscall->EA_GetInput(client, thinktime, input);
}

void trap_EA_ResetInput(int client) {
    gameSyscall->EA_ResetInput(client);
}

int trap_BotLoadCharacter(char* charfile, float skill) {
    return gameSyscall->BotLoadCharacter(charfile, skill);
}

void trap_BotFreeCharacter(int character) {
    gameSyscall->BotFreeCharacter(character);
}

float trap_Characteristic_Float(int character, int index) {
    return gameSyscall->Characteristic_Float(character, index);
}

float trap_Characteristic_BFloat(int character, int index, float min, float max) {
    return gameSyscall->Characteristic_BFloat(character, index, min, max);
}

int trap_Characteristic_Integer(int character, int index) {
    return gameSyscall->Characteristic_Integer(character, index);
}

int trap_Characteristic_BInteger(int character, int index, int min, int max) {
    return gameSyscall->Characteristic_BInteger(character, index, min, max);
}

void trap_Characteristic_String(int character, int index, char* buf, int size) {
    gameSyscall->Characteristic_String(character, index, buf, size);
}

int trap_BotAllocChatState(void) {
    return gameSyscall->BotAllocChatState();
}

void trap_BotFreeChatState(int handle) {
    gameSyscall->BotFreeChatState(handle);
}

void trap_BotQueueConsoleMessage(int chatstate, int type, char* message) {
    gameSyscall->BotQueueConsoleMessage(chatstate, type, message);
}

void trap_BotRemoveConsoleMessage(int chatstate, int handle) {
    gameSyscall->BotRemoveConsoleMessage(chatstate, handle);
}

int trap_BotNextConsoleMessage(int chatstate, void* cm) {
    return gameSyscall->BotNextConsoleMessage(chatstate, cm);
}

int trap_BotNumConsoleMessages(int chatstate) {
    return gameSyscall->BotNumConsoleMessages(chatstate);
}

void trap_BotInitialChat(int chatstate, char* type, int mcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7) {
    gameSyscall->BotInitialChat(chatstate, type, mcontext, var0, var1, var2, var3, var4, var5, var6, var7);
}

int trap_BotNumInitialChats(int chatstate, char* type) {
    return gameSyscall->BotNumInitialChats(chatstate, type);
}

int trap_BotReplyChat(int chatstate, char* message, int mcontext, int vcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7) {
    return gameSyscall->BotReplyChat(chatstate, message, mcontext, vcontext, var0, var1, var2, var3, var4, var5, var6, var7);
}

int trap_BotChatLength(int chatstate) {
    return gameSyscall->BotChatLength(chatstate);
}

void trap_BotEnterChat(int chatstate, int client, int sendto) {
    gameSyscall->BotEnterChat(chatstate, client, sendto);
}

void trap_BotGetChatMessage(int chatstate, char* buf, int size) {
    gameSyscall->BotGetChatMessage(chatstate, buf, size);
}

int trap_StringContains(char* str1, char* str2, int casesensitive) {
    return gameSyscall->StringContains(str1, str2, casesensitive);
}

int trap_BotFindMatch(char* str, void* match, unsigned long int context) {
    return gameSyscall->BotFindMatch(str, match, context);
}

void trap_BotMatchVariable(void* match, int variable, char* buf, int size) {
    gameSyscall->BotMatchVariable(match, variable, buf, size);
}

void trap_UnifyWhiteSpaces(char* string) {
    gameSyscall->UnifyWhiteSpaces(string);
}

void trap_BotReplaceSynonyms(char* string, unsigned long int context) {
    gameSyscall->BotReplaceSynonyms(string, context);
}

int trap_BotLoadChatFile(int chatstate, char* chatfile, char* chatname) {
    return gameSyscall->BotLoadChatFile(chatstate, chatfile, chatname);
}

void trap_BotSetChatGender(int chatstate, int gender) {
    gameSyscall->BotSetChatGender(chatstate, gender);
}

void trap_BotSetChatName(int chatstate, char* name, int client) {
    gameSyscall->BotSetChatName(chatstate, name, client);
}

void trap_BotResetGoalState(int goalstate) {
    gameSyscall->BotResetGoalState(goalstate);
}

void trap_BotResetAvoidGoals(int goalstate) {
    gameSyscall->BotResetAvoidGoals(goalstate);
}

void trap_BotRemoveFromAvoidGoals(int goalstate, int number) {
    gameSyscall->BotRemoveFromAvoidGoals(goalstate, number);
}

void trap_BotPushGoal(int goalstate, void* goal) {
    gameSyscall->BotPushGoal(goalstate, goal);
}

void trap_BotPopGoal(int goalstate) {
    gameSyscall->BotPopGoal(goalstate);
}

void trap_BotEmptyGoalStack(int goalstate) {
    gameSyscall->BotEmptyGoalStack(goalstate);
}

void trap_BotDumpAvoidGoals(int goalstate) {
    gameSyscall->BotDumpAvoidGoals(goalstate);
}

void trap_BotDumpGoalStack(int goalstate) {
    gameSyscall->BotDumpGoalStack(goalstate);
}

void trap_BotGoalName(int number, char* name, int size) {
    gameSyscall->BotGoalName(number, name, size);
}

int trap_BotGetTopGoal(int goalstate, void* goal) {
    return gameSyscall->BotGetTopGoal(goalstate, goal);
}

int trap_BotGetSecondGoal(int goalstate, void* goal) {
    return gameSyscall->BotGetSecondGoal(goalstate, goal);
}

int trap_BotChooseLTGItem(int goalstate, vec3_t origin, int* inventory, int travelflags) {
    return gameSyscall->BotChooseLTGItem(goalstate, origin, inventory, travelflags);
}

int trap_BotChooseNBGItem(int goalstate, vec3_t origin, int* inventory, int travelflags, void* ltg, float maxtime) {
    return gameSyscall->BotChooseNBGItem(goalstate, origin, inventory, travelflags, ltg, maxtime);
}

int trap_BotTouchingGoal(vec3_t origin, void* goal) {
    return gameSyscall->BotTouchingGoal(origin, goal);
}

int trap_BotItemGoalInVisButNotVisible(int viewer, vec3_t eye, vec3_t viewangles, void* goal) {
    return gameSyscall->BotItemGoalInVisButNotVisible(viewer, eye, viewangles, goal);
}

int trap_BotGetLevelItemGoal(int index, char* classname, void* goal) {
    return gameSyscall->BotGetLevelItemGoal(index, classname, goal);
}

int trap_BotGetNextCampSpotGoal(int num, void* goal) {
    return gameSyscall->BotGetNextCampSpotGoal(num, goal);
}

int trap_BotGetMapLocationGoal(char* name, void* goal) {
    return gameSyscall->BotGetMapLocationGoal(name, goal);
}

float trap_BotAvoidGoalTime(int goalstate, int number) {
    return gameSyscall->BotAvoidGoalTime(goalstate, number);
}

void trap_BotSetAvoidGoalTime(int goalstate, int number, float avoidtime) {
    gameSyscall->BotSetAvoidGoalTime(goalstate, number, avoidtime);
}

void trap_BotInitLevelItems(void) {
    gameSyscall->BotInitLevelItems();
}

void trap_BotUpdateEntityItems(void) {
    gameSyscall->BotUpdateEntityItems();
}

int trap_BotLoadItemWeights(int goalstate, char* filename) {
    return gameSyscall->BotLoadItemWeights(goalstate, filename);
}

void trap_BotFreeItemWeights(int goalstate) {
    gameSyscall->BotFreeItemWeights(goalstate);
}

void trap_BotInterbreedGoalFuzzyLogic(int parent1, int parent2, int child) {
    gameSyscall->BotInterbreedGoalFuzzyLogic(parent1, parent2, child);
}

void trap_BotSaveGoalFuzzyLogic(int goalstate, char* filename) {
    gameSyscall->BotSaveGoalFuzzyLogic(goalstate, filename);
}

void trap_BotMutateGoalFuzzyLogic(int goalstate, float range) {
    gameSyscall->BotMutateGoalFuzzyLogic(goalstate, range);
}

int trap_BotAllocGoalState(int state) {
    return gameSyscall->BotAllocGoalState(state);
}

void trap_BotFreeGoalState(int handle) {
    gameSyscall->BotFreeGoalState(handle);
}

void trap_BotResetMoveState(int movestate) {
    gameSyscall->BotResetMoveState(movestate);
}

void trap_BotAddAvoidSpot(int movestate, vec3_t origin, float radius, int type) {
    gameSyscall->BotAddAvoidSpot(movestate, origin, radius, type);
}

void trap_BotMoveToGoal(void* result, int movestate, void* goal, int travelflags) {
    gameSyscall->BotMoveToGoal(result, movestate, goal, travelflags);
}

int trap_BotMoveInDirection(int movestate, vec3_t dir, float speed, int type) {
    return gameSyscall->BotMoveInDirection(movestate, dir, speed, type);
}

void trap_BotResetAvoidReach(int movestate) {
    gameSyscall->BotResetAvoidReach(movestate);
}

void trap_BotResetLastAvoidReach(int movestate) {
    gameSyscall->BotResetLastAvoidReach(movestate);
}

int trap_BotReachabilityArea(vec3_t origin, int testground) {
    return gameSyscall->BotReachabilityArea(origin, testground);
}

int trap_BotMovementViewTarget(int movestate, void* goal, int travelflags, float lookahead, vec3_t target) {
    return gameSyscall->BotMovementViewTarget(movestate, goal, travelflags, lookahead, target);
}

int trap_BotPredictVisiblePosition(vec3_t origin, int areanum, void* goal, int travelflags, vec3_t target) {
    return gameSyscall->BotPredictVisiblePosition(origin, areanum, goal, travelflags, target);
}

int trap_BotAllocMoveState(void) {
    return gameSyscall->BotAllocMoveState();
}

void trap_BotFreeMoveState(int handle) {
    gameSyscall->BotFreeMoveState(handle);
}

void trap_BotInitMoveState(int handle, void* initmove) {
    gameSyscall->BotInitMoveState(handle, initmove);
}

int trap_BotChooseBestFightWeapon(int weaponstate, int* inventory) {
    return gameSyscall->BotChooseBestFightWeapon(weaponstate, inventory);
}

void trap_BotGetWeaponInfo(int weaponstate, int weapon, void* weaponinfo) {
    gameSyscall->BotGetWeaponInfo(weaponstate, weapon, weaponinfo);
}

int trap_BotLoadWeaponWeights(int weaponstate, char* filename) {
    return gameSyscall->BotLoadWeaponWeights(weaponstate, filename);
}

int trap_BotAllocWeaponState(void) {
    return gameSyscall->BotAllocWeaponState();
}

void trap_BotFreeWeaponState(int weaponstate) {
    gameSyscall->BotFreeWeaponState(weaponstate);
}

void trap_BotResetWeaponState(int weaponstate) {
    gameSyscall->BotResetWeaponState(weaponstate);
}

int trap_GeneticParentsAndChildSelection(int numranks, float* ranks, int* parent1, int* parent2, int* child) {
    return gameSyscall->GeneticParentsAndChildSelection(numranks, ranks, parent1, parent2, child);
}

int trap_PC_LoadSource(const char* filename) {
    return gameSyscall->PC_LoadSource(filename);
}

int trap_PC_FreeSource(int handle) {
    return gameSyscall->PC_FreeSource(handle);
}

int trap_PC_ReadToken(int handle, pc_token_t* pc_token) {
    return gameSyscall->PC_ReadToken(handle, pc_token);
}

int trap_PC_SourceFileAndLine(int handle, char* filename, int* line) {
    return gameSyscall->PC_SourceFileAndLine(handle, filename, line);
}

void trap_LLM_PushPrompt(int chatstate, const char* prompt) {
    gameSyscall->LLM_PushPrompt(chatstate, prompt);
}