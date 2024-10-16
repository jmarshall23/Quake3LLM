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
#ifndef __UI_PUBLIC_H__
#define __UI_PUBLIC_H__

#define UI_API_VERSION	6

typedef struct {
	connstate_t		connState;
	int				connectPacketCount;
	int				clientNum;
	char			servername[MAX_STRING_CHARS];
	char			updateInfoString[MAX_STRING_CHARS];
	char			messageString[MAX_STRING_CHARS];
} uiClientState_t;

typedef struct {
    void (*Com_Error)(int code, const char* fmt, ...);
	void (*Com_Printf)(const char* fmt, ...);
	int (*Sys_Milliseconds)(void);
	void (*Cvar_Register)(vmCvar_t* cvar, const char* varName, const char* defaultValue, int flags);
	void (*Cvar_Update)(vmCvar_t* cvar);
	void (*Cvar_Set)(const char* var_name, const char* value);
	float (*Cvar_VariableValue)(const char* var_name);
	void (*Cvar_VariableStringBuffer)(const char* var_name, char* buffer, int bufsize);
	void (*Cvar_SetValue)(const char* var_name, float value);
	void (*Cvar_Reset)(const char* name);
	cvar_t *(*Cvar_Get)(const char* var_name, const char* var_value, int flags);
	void (*Cvar_InfoStringBuffer)(int bit, char* buffer, int bufsize);
	int (*Cmd_Argc)(void);
	void (*Cmd_ArgvBuffer)(int n, char* buffer, int bufferLength);
	void (*Cbuf_ExecuteText)(int exec_when, const char* text);
	int (*FS_FOpenFileByMode)(const char* qpath, fileHandle_t* f, fsMode_t mode);
	void (*FS_Read2)(void* buffer, int len, fileHandle_t f);
	void (*FS_Write)(const void* buffer, int len, fileHandle_t f);
	void (*FS_FCloseFile)(fileHandle_t f);
	int (*FS_GetFileList)(const char* path, const char* extension, char* listbuf, int bufsize);
	int (*FS_Seek)(fileHandle_t f, long offset, int origin);
	qhandle_t(*re_RegisterModel)(const char* name);
	qhandle_t(*re_RegisterSkin)(const char* name);
	qhandle_t(*re_RegisterShaderNoMip)(const char* name);
	void (*re_ClearScene)(void);
	void (*re_AddRefEntityToScene)(const refEntity_t* re);
	void (*re_AddPolyToScene)(qhandle_t hShader, int numVerts, const polyVert_t* verts, int numverts);
	void (*re_AddLightToScene)(const vec3_t org, float intensity, float r, float g, float b);
	void (*re_RenderScene)(const refdef_t* fd);
	void (*re_SetColor)(const float* rgba);
	void (*re_DrawStretchPic)(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader);
	void (*re_ModelBounds)(clipHandle_t model, vec3_t mins, vec3_t maxs);
	void (*SCR_UpdateScreen)(void);
	int (*re_LerpTag)(orientation_t* tag, clipHandle_t mod, int startFrame, int endFrame, float frac, const char* tagName);
	sfxHandle_t(*S_RegisterSound)(const char* sample, qboolean compressed);
	void (*S_StartLocalSound)(sfxHandle_t sfx, int channelNum);
	void (*Key_KeynumToStringBuf)(int keynum, char* buf, int buflen);
	void (*Key_GetBindingBuf)(int keynum, char* buf, int buflen);
	void (*Key_SetBinding)(int keynum, const char* binding);
	qboolean(*Key_IsDown)(int keynum);
	qboolean(*Key_GetOverstrikeMode)(void);
	void (*Key_SetOverstrikeMode)(qboolean state);
	void (*Key_ClearStates)(void);
	int (*Key_GetCatcher)(void);
	void (*Key_SetCatcher)(int catcher);
	void (*GetClipboardData)(char* buf, int bufsize);
	void (*GetClientState)(uiClientState_t* state);
	void (*CL_GetGlconfig)(glconfig_t* glconfig);
	int (*GetConfigString)(int index, char* buff, int buffsize);
	int (*LAN_GetServerCount)(int source);
	void (*LAN_GetServerAddressString)(int source, int n, char* buf, int buflen);
	void (*LAN_GetServerInfo)(int source, int n, char* buf, int buflen);
	int (*LAN_GetServerPing)(int source, int n);
	int (*LAN_GetPingQueueCount)(void);
	int (*LAN_ServerStatus)(char* serverAddress, char* serverStatus, int maxLen);
	void (*LAN_SaveCachedServers)();
	void (*LAN_LoadCachedServers)();
	void (*LAN_ResetPings)(int n);
	void (*LAN_ClearPing)(int n);
	void (*LAN_GetPing)(int n, char* buf, int buflen, int* pingtime);
	void (*LAN_GetPingInfo)(int n, char* buf, int buflen);
	void (*LAN_MarkServerVisible)(int source, int n, qboolean visible);
	int (*LAN_ServerIsVisible)(int source, int n);
	qboolean(*LAN_UpdateVisiblePings)(int source);
	int (*LAN_AddServer)(int source, const char* name, const char* addr);
	void (*LAN_RemoveServer)(int source, const char* addr);
	int (*LAN_CompareServers)(int source, int sortKey, int sortDir, int s1, int s2);
	int (*Hunk_MemoryRemaining)(void);
	void (*CLUI_GetCDKey)(char* buf, int buflen);
	void (*CLUI_SetCDKey)(char* buf);
	void (*S_StopBackgroundTrack)(void);
	void (*S_StartBackgroundTrack)(const char* intro, const char* loop);
	int (*Com_RealTime)(qtime_t* qtime);
	int (*CIN_PlayCinematic)(const char* arg0, int xpos, int ypos, int width, int height, int bits);
	e_status(*CIN_StopCinematic)(int handle);
	e_status(*CIN_RunCinematic)(int handle);
	void (*CIN_DrawCinematic)(int handle);
	void (*CIN_SetExtents)(int handle, int x, int y, int w, int h);
	void (*re_RemapShader)(const char* oldShader, const char* newShader, const char* timeOffset);
	qboolean(*CL_CDKeyValidate)(const char* key, const char* chksum);
	int (*botlib_export_PC_AddGlobalDefine)(char* define);
	int (*botlib_export_PC_LoadSourceHandle)(const char* filename);
	int (*botlib_export_PC_FreeSourceHandle)(int handle);
	int (*botlib_export_PC_ReadTokenHandle)(int handle, pc_token_t* pc_token);
	int (*botlib_export_PC_SourceFileAndLine)(int handle, char* filename, int* line);
} uiImport_t;

typedef enum {
	UIMENU_NONE,
	UIMENU_MAIN,
	UIMENU_INGAME,
	UIMENU_NEED_CD,
	UIMENU_BAD_CD_KEY,
	UIMENU_TEAM,
	UIMENU_POSTGAME
} uiMenuCommand_t;

#define SORT_HOST			0
#define SORT_MAP			1
#define SORT_CLIENTS		2
#define SORT_GAME			3
#define SORT_PING			4
#define SORT_PUNKBUSTER		5

typedef struct {
	void (*UI_Init)(void);
	void (*UI_Shutdown)(void);
	void (*UI_KeyEvent)(int, int);
	void (*UI_MouseEvent)(int, int);
	void (*UI_Refresh)(int);
	qboolean (*UI_IsFullscreen)(void);
	void (*UI_SetActiveMenu)(uiMenuCommand_t);
	qboolean (*UI_ConsoleCommand)(int);
	void (*UI_DrawConnectScreen)(qboolean);
} uiExport_t;

extern uiExport_t* ui;

#endif
