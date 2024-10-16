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
// cg_syscalls.c
// 

#include "cg_local.h"

cgameImport_t *cgSyscall;
cgameExport_t* cgame;

void CG_Init(int serverMessageNum, int serverCommandSequence, int clientNum);
void CG_Shutdown(void);

cgameExport_t *dllEntry(cgameImport_t *imports) {
    static cgameExport_t exports;

    exports.CG_Init = CG_Init;
    exports.CG_Shutdown = CG_Shutdown;
    exports.CG_ConsoleCommand = CG_ConsoleCommand;
    exports.CG_DrawActiveFrame = CG_DrawActiveFrame;
    exports.CG_CrosshairPlayer = CG_CrosshairPlayer;
    exports.CG_LastAttacker = CG_LastAttacker;
    exports.CG_KeyEvent = CG_KeyEvent;
    exports.CG_MouseEvent = CG_MouseEvent;
    exports.CG_EventHandling = CG_EventHandling;

    cgSyscall = imports;
    cgame = &exports;

    return &exports;
}

void trap_Print(const char* fmt) {
    cgSyscall->Print(fmt);
}

void trap_Error(const char* fmt) {
    cgSyscall->Error(ERR_DROP, fmt);
}

int trap_Milliseconds(void) {
    return cgSyscall->Milliseconds();
}

void trap_Cvar_Register(vmCvar_t* vmCvar, const char* varName, const char* defaultValue, int flags) {
    cgSyscall->Cvar_Register(vmCvar, varName, defaultValue, flags);
}

void trap_Cvar_Update(vmCvar_t* vmCvar) {
    cgSyscall->Cvar_Update(vmCvar);
}

void trap_Cvar_Set(const char* var_name, const char* value) {
    cgSyscall->Cvar_Set(var_name, value);
}

void trap_Cvar_VariableStringBuffer(const char* var_name, char* buffer, int bufsize) {
    cgSyscall->Cvar_VariableStringBuffer(var_name, buffer, bufsize);
}

int trap_Argc(void) {
    return cgSyscall->Argc();
}

void trap_Argv(int n, char* buffer, int bufferLength) {
    cgSyscall->Argv(n, buffer, bufferLength);
}

void trap_Args(char* buffer, int bufferLength) {
    cgSyscall->Args(buffer, bufferLength);
}

int trap_FS_FOpenFile(const char* qpath, fileHandle_t* f, fsMode_t mode) {
    return cgSyscall->FS_FOpenFile(qpath, f, mode);
}

void trap_FS_Read(void* buffer, int len, fileHandle_t f) {
    cgSyscall->FS_Read(buffer, len, f);
}

void trap_FS_Write(const void* buffer, int len, fileHandle_t f) {
    cgSyscall->FS_Write(buffer, len, f);
}

void trap_FS_FCloseFile(fileHandle_t f) {
    cgSyscall->FS_FCloseFile(f);
}

int trap_FS_Seek(fileHandle_t f, long offset, int origin) {
    return cgSyscall->FS_Seek(f, offset, origin);
}

void trap_SendConsoleCommand(const char* text) {
    cgSyscall->SendConsoleCommand(text);
}

void trap_AddCommand(const char* cmdName) {
    cgSyscall->AddCommand(cmdName);
}

void trap_RemoveCommand(const char* cmdName) {
    cgSyscall->RemoveCommand(cmdName);
}

void trap_SendClientCommand(const char* s) {
    cgSyscall->SendClientCommand(s);
}

void trap_UpdateScreen(void) {
    cgSyscall->UpdateScreen();
}

void trap_CM_LoadMap(const char* mapname) {
    cgSyscall->CM_LoadMap(mapname);
}

int trap_CM_NumInlineModels(void) {
    return cgSyscall->CM_NumInlineModels();
}

clipHandle_t trap_CM_InlineModel(int index) {
    return cgSyscall->CM_InlineModel(index);
}

clipHandle_t trap_CM_TempBoxModel(const vec3_t mins, const vec3_t maxs) {
    return cgSyscall->CM_TempBoxModel(mins, maxs, qfalse);
}

clipHandle_t trap_CM_TempCapsuleModel(const vec3_t mins, const vec3_t maxs) {
    return cgSyscall->CM_TempBoxModel(mins, maxs, qfalse);
}

int trap_CM_PointContents(const vec3_t p, clipHandle_t model) {
    return cgSyscall->CM_PointContents(p, model);
}

int trap_CM_TransformedPointContents(const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles) {
    return cgSyscall->CM_TransformedPointContents(p, model, origin, angles);
}

void trap_CM_BoxTrace(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask) {
    cgSyscall->CM_BoxTrace(results, start, end, mins, maxs, model, brushmask, qfalse);
}

void trap_CM_CapsuleTrace(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask) {
    cgSyscall->CM_BoxTrace(results, start, end, mins, maxs, model, brushmask, qtrue);
}

void trap_CM_TransformedBoxTrace(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles) {
    cgSyscall->CM_TransformedBoxTrace(results, start, end, mins, maxs, model, brushmask, origin, angles, qfalse);
}

void trap_CM_TransformedCapsuleTrace(trace_t* results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles) {
    cgSyscall->CM_TransformedBoxTrace(results, start, end, mins, maxs, model, brushmask, origin, angles, qtrue);
}

int trap_CM_MarkFragments(int numPoints, const vec3_t* points, const vec3_t projection, int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t* fragmentBuffer) {
    return cgSyscall->CM_MarkFragments(numPoints, points, projection, maxPoints, pointBuffer, maxFragments, fragmentBuffer);
}

void trap_S_StartSound(vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx) {
    cgSyscall->S_StartSound(origin, entityNum, entchannel, sfx);
}

void trap_S_StartLocalSound(sfxHandle_t sfx, int channelNum) {
    cgSyscall->S_StartLocalSound(sfx, channelNum);
}

void trap_S_ClearLoopingSounds(qboolean killall) {
    cgSyscall->S_ClearLoopingSounds(killall);
}

void trap_S_AddLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx) {
    cgSyscall->S_AddLoopingSound(entityNum, origin, velocity, sfx);
}

void trap_S_AddRealLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx) {
    cgSyscall->S_AddRealLoopingSound(entityNum, origin, velocity, sfx);
}

void trap_S_StopLoopingSound(int entityNum) {
    cgSyscall->S_StopLoopingSound(entityNum);
}

void trap_S_UpdateEntityPosition(int entityNum, const vec3_t origin) {
    cgSyscall->S_UpdateEntityPosition(entityNum, origin);
}

void trap_S_Respatialize(int entityNum, const vec3_t origin, vec3_t axis[3], int inwater) {
    cgSyscall->S_Respatialize(entityNum, origin, axis, inwater);
}

sfxHandle_t trap_S_RegisterSound(const char* sample, qboolean compressed) {
    return cgSyscall->S_RegisterSound(sample, compressed);
}

void trap_S_StartBackgroundTrack(const char* intro, const char* loop) {
    cgSyscall->S_StartBackgroundTrack(intro, loop);
}

void trap_R_LoadWorldMap(const char* mapname) {
    cgSyscall->R_LoadWorldMap(mapname);
}

qhandle_t trap_R_RegisterModel(const char* name) {
    return cgSyscall->R_RegisterModel(name);
}

qhandle_t trap_R_RegisterSkin(const char* name) {
    return cgSyscall->R_RegisterSkin(name);
}

qhandle_t trap_R_RegisterShader(const char* name) {
    return cgSyscall->R_RegisterShader(name);
}

qhandle_t trap_R_RegisterShaderNoMip(const char* name) {
    return cgSyscall->R_RegisterShaderNoMip(name);
}

void trap_R_RegisterFont(const char* fontName, int pointSize, fontInfo_t* font) {
    cgSyscall->R_RegisterFont(fontName, pointSize, font);
}

void trap_R_ClearScene(void) {
    cgSyscall->R_ClearScene();
}

void trap_R_AddRefEntityToScene(const refEntity_t* re) {
    cgSyscall->R_AddRefEntityToScene(re);
}

void trap_R_AddPolyToScene(qhandle_t hShader, int numVerts, const polyVert_t* verts) {
    cgSyscall->R_AddPolyToScene(hShader, numVerts, verts, 1);
}

void trap_R_AddPolysToScene(qhandle_t hShader, int numVerts, const polyVert_t* verts, int num) {
    cgSyscall->R_AddPolysToScene(hShader, numVerts, verts, num);
}

int trap_R_LightForPoint(vec3_t point, vec3_t ambientLight, vec3_t directedLight, vec3_t lightDir) {
    return cgSyscall->R_LightForPoint(point, ambientLight, directedLight, lightDir);
}

void trap_R_AddLightToScene(const vec3_t org, float intensity, float r, float g, float b) {
    cgSyscall->R_AddLightToScene(org, intensity, r, g, b);
}

void trap_R_AddAdditiveLightToScene(const vec3_t org, float intensity, float r, float g, float b) {
    cgSyscall->R_AddAdditiveLightToScene(org, intensity, r, g, b);
}

void trap_R_RenderScene(const refdef_t* fd) {
    cgSyscall->R_RenderScene(fd);
}

void trap_R_SetColor(const float* rgba) {
    cgSyscall->R_SetColor(rgba);
}

void trap_R_DrawStretchPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader) {
    cgSyscall->R_DrawStretchPic(x, y, w, h, s1, t1, s2, t2, hShader);
}

void trap_R_ModelBounds(clipHandle_t model, vec3_t mins, vec3_t maxs) {
    cgSyscall->R_ModelBounds(model, mins, maxs);
}

int trap_R_LerpTag(orientation_t* tag, clipHandle_t mod, int startFrame, int endFrame, float frac, const char* tagName) {
    return cgSyscall->R_LerpTag(tag, mod, startFrame, endFrame, frac, tagName);
}

void trap_R_RemapShader(const char* oldShader, const char* newShader, const char* timeOffset) {
    cgSyscall->R_RemapShader(oldShader, newShader, timeOffset);
}

void trap_GetGlconfig(glconfig_t* glconfig) {
    cgSyscall->GetGlconfig(glconfig);
}

void trap_GetGameState(gameState_t* gamestate) {
    cgSyscall->GetGameState(gamestate);
}

void trap_GetCurrentSnapshotNumber(int* snapshotNumber, int* serverTime) {
    cgSyscall->GetCurrentSnapshotNumber(snapshotNumber, serverTime);
}

qboolean trap_GetSnapshot(int snapshotNumber, snapshot_t* snapshot) {
    return cgSyscall->GetSnapshot(snapshotNumber, snapshot);
}

qboolean trap_GetServerCommand(int serverCommandNumber) {
    return cgSyscall->GetServerCommand(serverCommandNumber);
}

int trap_GetCurrentCmdNumber(void) {
    return cgSyscall->GetCurrentCmdNumber();
}

qboolean trap_GetUserCmd(int cmdNumber, usercmd_t* ucmd) {
    return cgSyscall->GetUserCmd(cmdNumber, ucmd);
}

void trap_SetUserCmdValue(int stateValue, float sensitivityScale) {
    cgSyscall->SetUserCmdValue(stateValue, sensitivityScale);
}

int trap_MemoryRemaining(void) {
    return cgSyscall->MemoryRemaining();
}

qboolean trap_Key_IsDown(int keynum) {
    return cgSyscall->Key_IsDown(keynum);
}

int trap_Key_GetCatcher(void) {
    return cgSyscall->Key_GetCatcher();
}

void trap_Key_SetCatcher(int catcher) {
    cgSyscall->Key_SetCatcher(catcher);
}

int trap_Key_GetKey(const char* binding) {
    return cgSyscall->Key_GetKey(binding);
}

int trap_PC_AddGlobalDefine(char* define) {
    return cgSyscall->PC_AddGlobalDefine(define);
}

int trap_PC_LoadSource(const char* filename) {
    return cgSyscall->PC_LoadSource(filename);
}

int trap_PC_FreeSource(int handle) {
    return cgSyscall->PC_FreeSource(handle);
}

int trap_PC_ReadToken(int handle, pc_token_t* pc_token) {
    return cgSyscall->PC_ReadToken(handle, pc_token);
}

int trap_PC_SourceFileAndLine(int handle, char* filename, int* line) {
    return cgSyscall->PC_SourceFileAndLine(handle, filename, line);
}

void trap_S_StopBackgroundTrack(void) {
    cgSyscall->S_StopBackgroundTrack();
}

int trap_RealTime(qtime_t* qtime) {
    return cgSyscall->RealTime(qtime);
}

int trap_CIN_PlayCinematic(const char* arg0, int xpos, int ypos, int width, int height, int bits) {
    return cgSyscall->CIN_PlayCinematic(arg0, xpos, ypos, width, height, bits);
}

e_status trap_CIN_StopCinematic(int handle) {
    return cgSyscall->CIN_StopCinematic(handle);
}

e_status trap_CIN_RunCinematic(int handle) {
    return cgSyscall->CIN_RunCinematic(handle);
}

void trap_CIN_DrawCinematic(int handle) {
    cgSyscall->CIN_DrawCinematic(handle);
}

void trap_CIN_SetExtents(int handle, int x, int y, int w, int h) {
    cgSyscall->CIN_SetExtents(handle, x, y, w, h);
}

qboolean trap_GetEntityToken(char* buffer, int bufferSize) {
    return cgSyscall->GetEntityToken(buffer, bufferSize);
}

qboolean trap_R_inPVS(const vec3_t p1, const vec3_t p2) {
    return cgSyscall->R_inPVS(p1, p2);
}

void trap_R_AddCoronaToScene(const vec3_t org, float r, float g, float b, float scale, int id, int flags) {
    cgSyscall->R_AddCoronaToScene(org, r, g, b, scale, id, flags);
}