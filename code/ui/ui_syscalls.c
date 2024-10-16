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
#include "ui_local.h"

uiImport_t	*uiSyscalls;
uiExport_t*  ui;

uiExport_t  *dllEntry( uiImport_t *imports ) {
    static uiExport_t exports;

    exports.UI_Init = UI_Init;
    exports.UI_Shutdown = UI_Shutdown;
    exports.UI_KeyEvent = UI_KeyEvent;
    exports.UI_MouseEvent = UI_MouseEvent;
    exports.UI_Refresh = UI_Refresh;
    exports.UI_IsFullscreen = UI_IsFullscreen;
    exports.UI_SetActiveMenu = UI_SetActiveMenu;
    exports.UI_ConsoleCommand = UI_ConsoleCommand;
    exports.UI_DrawConnectScreen = UI_DrawConnectScreen;

    uiSyscalls = imports;
    ui = &exports;

    return &exports;
}

void trap_Print(const char* string) {
    uiSyscalls->Com_Printf(string);
}

void trap_Error(const char* string) {
    uiSyscalls->Com_Error(ERR_DROP, "%s", string);
}

int trap_Milliseconds(void) {
    return uiSyscalls->Sys_Milliseconds();
}

void trap_Cvar_Register(vmCvar_t* cvar, const char* var_name, const char* value, int flags) {
    uiSyscalls->Cvar_Register(cvar, var_name, value, flags);
}

void trap_Cvar_Update(vmCvar_t* cvar) {
    uiSyscalls->Cvar_Update(cvar);
}

void trap_Cvar_Set(const char* var_name, const char* value) {
    uiSyscalls->Cvar_Set(var_name, value);
}

float trap_Cvar_VariableValue(const char* var_name) {
    return uiSyscalls->Cvar_VariableValue(var_name);
}

void trap_Cvar_VariableStringBuffer(const char* var_name, char* buffer, int bufsize) {
    uiSyscalls->Cvar_VariableStringBuffer(var_name, buffer, bufsize);
}

void trap_Cvar_SetValue(const char* var_name, float value) {
    uiSyscalls->Cvar_SetValue(var_name, value);
}

void trap_Cvar_Reset(const char* name) {
    uiSyscalls->Cvar_Reset(name);
}

void trap_Cvar_Create(const char* var_name, const char* var_value, int flags) {
    uiSyscalls->Cvar_Get(var_name, var_value, flags);
}

void trap_Cvar_InfoStringBuffer(int bit, char* buffer, int bufsize) {
    uiSyscalls->Cvar_InfoStringBuffer(bit, buffer, bufsize);
}

int trap_Argc(void) {
    return uiSyscalls->Cmd_Argc();
}

void trap_Argv(int n, char* buffer, int bufferLength) {
    uiSyscalls->Cmd_ArgvBuffer(n, buffer, bufferLength);
}

void trap_Cmd_ExecuteText(int exec_when, const char* text) {
    uiSyscalls->Cbuf_ExecuteText(exec_when, text);
}

int trap_FS_FOpenFile(const char* qpath, fileHandle_t* f, fsMode_t mode) {
    return uiSyscalls->FS_FOpenFileByMode(qpath, f, mode);
}

void trap_FS_Read(void* buffer, int len, fileHandle_t f) {
    uiSyscalls->FS_Read2(buffer, len, f);
}

void trap_FS_Write(const void* buffer, int len, fileHandle_t f) {
    uiSyscalls->FS_Write(buffer, len, f);
}

void trap_FS_FCloseFile(fileHandle_t f) {
    uiSyscalls->FS_FCloseFile(f);
}

int trap_FS_GetFileList(const char* path, const char* extension, char* listbuf, int bufsize) {
    return uiSyscalls->FS_GetFileList(path, extension, listbuf, bufsize);
}

int trap_FS_Seek(fileHandle_t f, long offset, int origin) {
    return uiSyscalls->FS_Seek(f, offset, origin);
}

qhandle_t trap_R_RegisterModel(const char* name) {
    return uiSyscalls->re_RegisterModel(name);
}

qhandle_t trap_R_RegisterSkin(const char* name) {
    return uiSyscalls->re_RegisterSkin(name);
}

qhandle_t trap_R_RegisterShaderNoMip(const char* name) {
    return uiSyscalls->re_RegisterShaderNoMip(name);
}

void trap_R_ClearScene(void) {
    uiSyscalls->re_ClearScene();
}

void trap_R_AddRefEntityToScene(const refEntity_t* re) {
    uiSyscalls->re_AddRefEntityToScene(re);
}

void trap_R_AddPolyToScene(qhandle_t hShader, int numVerts, const polyVert_t* verts) {
    uiSyscalls->re_AddPolyToScene(hShader, numVerts, verts, 1);
}

void trap_R_AddLightToScene(const vec3_t org, float intensity, float r, float g, float b) {
    uiSyscalls->re_AddLightToScene(org, intensity, r, g, b);
}

void trap_R_RenderScene(const refdef_t* fd) {
    uiSyscalls->re_RenderScene(fd);
}

void trap_R_SetColor(const float* rgba) {
    uiSyscalls->re_SetColor(rgba);
}

void trap_R_DrawStretchPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader) {
    uiSyscalls->re_DrawStretchPic(x, y, w, h, s1, t1, s2, t2, hShader);
}

void trap_R_ModelBounds(clipHandle_t model, vec3_t mins, vec3_t maxs) {
    uiSyscalls->re_ModelBounds(model, mins, maxs);
}

void trap_UpdateScreen(void) {
    uiSyscalls->SCR_UpdateScreen();
}

int trap_CM_LerpTag(orientation_t* tag, clipHandle_t mod, int startFrame, int endFrame, float frac, const char* tagName) {
    return uiSyscalls->re_LerpTag(tag, mod, startFrame, endFrame, frac, tagName);
}

sfxHandle_t trap_S_RegisterSound(const char* sample, qboolean compressed) {
    return uiSyscalls->S_RegisterSound(sample, compressed);
}

void trap_S_StartLocalSound(sfxHandle_t sfx, int channelNum) {
    uiSyscalls->S_StartLocalSound(sfx, channelNum);
}

void trap_Key_KeynumToStringBuf(int keynum, char* buf, int buflen) {
    uiSyscalls->Key_KeynumToStringBuf(keynum, buf, buflen);
}

void trap_Key_GetBindingBuf(int keynum, char* buf, int buflen) {
    uiSyscalls->Key_GetBindingBuf(keynum, buf, buflen);
}

void trap_Key_SetBinding(int keynum, const char* binding) {
    uiSyscalls->Key_SetBinding(keynum, binding);
}

qboolean trap_Key_IsDown(int keynum) {
    return uiSyscalls->Key_IsDown(keynum);
}

qboolean trap_Key_GetOverstrikeMode(void) {
    return uiSyscalls->Key_GetOverstrikeMode();
}

void trap_Key_SetOverstrikeMode(qboolean state) {
    uiSyscalls->Key_SetOverstrikeMode(state);
}

void trap_Key_ClearStates(void) {
    uiSyscalls->Key_ClearStates();
}

int trap_Key_GetCatcher(void) {
    return uiSyscalls->Key_GetCatcher();
}

void trap_Key_SetCatcher(int catcher) {
    uiSyscalls->Key_SetCatcher(catcher);
}

void trap_GetClipboardData(char* buf, int bufsize) {
    uiSyscalls->GetClipboardData(buf, bufsize);
}

void trap_GetClientState(uiClientState_t* state) {
    uiSyscalls->GetClientState(state);
}

void trap_GetGlconfig(glconfig_t* glconfig) {
    uiSyscalls->CL_GetGlconfig(glconfig);
}

int trap_GetConfigString(int index, char* buff, int buffsize) {
    return uiSyscalls->GetConfigString(index, buff, buffsize);
}

int trap_LAN_GetServerCount(int source) {
    return uiSyscalls->LAN_GetServerCount(source);
}

void trap_LAN_GetServerAddressString(int source, int n, char* buf, int buflen) {
    uiSyscalls->LAN_GetServerAddressString(source, n, buf, buflen);
}

void trap_LAN_GetServerInfo(int source, int n, char* buf, int buflen) {
    uiSyscalls->LAN_GetServerInfo(source, n, buf, buflen);
}

int trap_LAN_GetServerPing(int source, int n) {
    return uiSyscalls->LAN_GetServerPing(source, n);
}

int trap_LAN_GetPingQueueCount(void) {
    return uiSyscalls->LAN_GetPingQueueCount();
}

int trap_LAN_ServerStatus(const char* serverAddress, char* serverStatus, int maxLen) {
    return uiSyscalls->LAN_ServerStatus(serverAddress, serverStatus, maxLen);
}

void trap_LAN_SaveCachedServers(void) {
    uiSyscalls->LAN_SaveCachedServers();
}

void trap_LAN_LoadCachedServers(void) {
    uiSyscalls->LAN_LoadCachedServers();
}

void trap_LAN_ResetPings(int n) {
    uiSyscalls->LAN_ResetPings(n);
}

void trap_LAN_ClearPing(int n) {
    uiSyscalls->LAN_ClearPing(n);
}

void trap_LAN_GetPing(int n, char* buf, int buflen, int* pingtime) {
    uiSyscalls->LAN_GetPing(n, buf, buflen, pingtime);
}

void trap_LAN_GetPingInfo(int n, char* buf, int buflen) {
    uiSyscalls->LAN_GetPingInfo(n, buf, buflen);
}

void trap_LAN_MarkServerVisible(int source, int n, qboolean visible) {
    uiSyscalls->LAN_MarkServerVisible(source, n, visible);
}

int trap_LAN_ServerIsVisible(int source, int n) {
    return uiSyscalls->LAN_ServerIsVisible(source, n);
}

qboolean trap_LAN_UpdateVisiblePings(int source) {
    return uiSyscalls->LAN_UpdateVisiblePings(source);
}

int trap_LAN_AddServer(int source, const char* name, const char* addr) {
    return uiSyscalls->LAN_AddServer(source, name, addr);
}

void trap_LAN_RemoveServer(int source, const char* addr) {
    uiSyscalls->LAN_RemoveServer(source, addr);
}

int trap_LAN_CompareServers(int source, int sortKey, int sortDir, int s1, int s2) {
    return uiSyscalls->LAN_CompareServers(source, sortKey, sortDir, s1, s2);
}

int trap_MemoryRemaining(void) {
    return uiSyscalls->Hunk_MemoryRemaining();
}

void trap_GetCDKey(char* buf, int buflen) {
    uiSyscalls->CLUI_GetCDKey(buf, buflen);
}

void trap_SetCDKey(const char* buf) {
    uiSyscalls->CLUI_SetCDKey(buf);
}

int trap_PC_AddGlobalDefine(const char* define) {
    return uiSyscalls->botlib_export_PC_AddGlobalDefine(define);
}

int trap_PC_LoadSource(const char* filename) {
    return uiSyscalls->botlib_export_PC_LoadSourceHandle(filename);
}

int trap_PC_FreeSource(int handle) {
    return uiSyscalls->botlib_export_PC_FreeSourceHandle(handle);
}

int trap_PC_ReadToken(int handle, pc_token_t* pc_token) {
    return uiSyscalls->botlib_export_PC_ReadTokenHandle(handle, pc_token);
}

int trap_PC_SourceFileAndLine(int handle, char* filename, int* line) {
    return uiSyscalls->botlib_export_PC_SourceFileAndLine(handle, filename, line);
}

void trap_S_StopBackgroundTrack(void) {
    uiSyscalls->S_StopBackgroundTrack();
}

void trap_S_StartBackgroundTrack(const char* intro, const char* loop) {
    uiSyscalls->S_StartBackgroundTrack(intro, loop);
}

int trap_RealTime(qtime_t* qtime) {
    return uiSyscalls->Com_RealTime(qtime);
}

int trap_CIN_PlayCinematic(const char* arg0, int xpos, int ypos, int width, int height, int bits) {
    return uiSyscalls->CIN_PlayCinematic(arg0, xpos, ypos, width, height, bits);
}

e_status trap_CIN_StopCinematic(int handle) {
    return uiSyscalls->CIN_StopCinematic(handle);
}

e_status trap_CIN_RunCinematic(int handle) {
    return uiSyscalls->CIN_RunCinematic(handle);
}

void trap_CIN_DrawCinematic(int handle) {
    uiSyscalls->CIN_DrawCinematic(handle);
}

void trap_CIN_SetExtents(int handle, int x, int y, int w, int h) {
    uiSyscalls->CIN_SetExtents(handle, x, y, w, h);
}

void trap_R_RemapShader(const char* oldShader, const char* newShader, const char* timeOffset) {
    uiSyscalls->re_RemapShader(oldShader, newShader, timeOffset);
}

qboolean trap_VerifyCDKey(const char* key, const char* chksum) {
    return uiSyscalls->CL_CDKeyValidate(key, chksum);
}

void trap_SetPbClStatus(int status) {
    
}
