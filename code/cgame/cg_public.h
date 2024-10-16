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


#define	CMD_BACKUP			64	
#define	CMD_MASK			(CMD_BACKUP - 1)
// allow a lot of command backups for very fast systems
// multiple commands may be combined into a single packet, so this
// needs to be larger than PACKET_BACKUP


#define	MAX_ENTITIES_IN_SNAPSHOT	256

// snapshots are a view of the server at a given time

// Snapshots are generated at regular time intervals by the server,
// but they may not be sent if a client's rate level is exceeded, or
// they may be dropped by the network.
typedef struct {
	int				snapFlags;			// SNAPFLAG_RATE_DELAYED, etc
	int				ping;

	int				serverTime;		// server time the message is valid for (in msec)

	byte			areamask[MAX_MAP_AREA_BYTES];		// portalarea visibility bits

	playerState_t	ps;						// complete information about the current player at this time

	int				numEntities;			// all of the entities that need to be presented
	entityState_t	entities[MAX_ENTITIES_IN_SNAPSHOT];	// at the time of this snapshot

	int				numServerCommands;		// text based server commands to execute when this
	int				serverCommandSequence;	// snapshot becomes current
} snapshot_t;

enum {
  CGAME_EVENT_NONE,
  CGAME_EVENT_TEAMMENU,
  CGAME_EVENT_SCOREBOARD,
  CGAME_EVENT_EDITHUD
};


/*
==================================================================

functions imported from the main executable

==================================================================
*/

#define	CGAME_IMPORT_API_VERSION	1001

typedef struct {
    void (*Print)(const char* fmt);
    void (*Error)(int level, const char* fmt);
    int (*Milliseconds)(void);
    void (*Cvar_Register)(vmCvar_t* vmCvar, const char* varName, const char* defaultValue, int flags);
    void (*Cvar_Update)(vmCvar_t* vmCvar);
    void (*Cvar_Set)(const char* var_name, const char* value);
    void (*Cvar_VariableStringBuffer)(const char* var_name, char* buffer, int bufsize);
    int (*Argc)(void);
    void (*Argv)(int n, char* buffer, int bufferLength);
    void (*Args)(char* buffer, int bufferLength);
    int (*FS_FOpenFile)(const char* qpath, fileHandle_t* f, fsMode_t mode);
    void (*FS_Read)(void* buffer, int len, fileHandle_t f);
    void (*FS_Write)(const void* buffer, int len, fileHandle_t f);
    void (*FS_FCloseFile)(fileHandle_t f);
    int (*FS_Seek)(fileHandle_t f, long offset, int origin);
    void (*SendConsoleCommand)(const char* text);
    void (*AddCommand)(const char* cmdName);
    void (*RemoveCommand)(const char* cmdName);
    void (*SendClientCommand)(const char* s);
    void (*UpdateScreen)(void);
    void (*CM_LoadMap)(const char* mapname);
    int (*CM_NumInlineModels)(void);
    clipHandle_t(*CM_InlineModel)(int index);
    clipHandle_t(*CM_TempBoxModel)(const vec3_t mins, const vec3_t maxs, int capsule);
    int (*CM_PointContents)(const vec3_t p, clipHandle_t model);
    int (*CM_TransformedPointContents)(const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles);
    void (*CM_BoxTrace)(trace_t* results, const vec3_t start, const vec3_t end,vec3_t mins, vec3_t maxs, clipHandle_t model, int brushmask, int capsule);
    void (*CM_TransformedBoxTrace)(trace_t* results, const vec3_t start, const vec3_t end, vec3_t mins, vec3_t maxs,clipHandle_t model, int brushmask,const vec3_t origin, const vec3_t angles, int capsule);
    int (*CM_MarkFragments)(int numPoints, const vec3_t* points, const vec3_t projection, int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t* fragmentBuffer);
    void (*S_StartSound)(vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx);
    void (*S_StartLocalSound)(sfxHandle_t sfx, int channelNum);
    void (*S_ClearLoopingSounds)(qboolean killall);
    void (*S_AddLoopingSound)(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx);
    void (*S_AddRealLoopingSound)(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx);
    void (*S_StopLoopingSound)(int entityNum);
    void (*S_UpdateEntityPosition)(int entityNum, const vec3_t origin);
    void (*S_Respatialize)(int entityNum, const vec3_t origin, vec3_t axis[3], int inwater);
    sfxHandle_t(*S_RegisterSound)(const char* sample, qboolean compressed);
    void (*S_StartBackgroundTrack)(const char* intro, const char* loop);
    void (*R_LoadWorldMap)(const char* mapname);
    qhandle_t(*R_RegisterModel)(const char* name);
    qhandle_t(*R_RegisterSkin)(const char* name);
    qhandle_t(*R_RegisterShader)(const char* name);
    qhandle_t(*R_RegisterShaderNoMip)(const char* name);
    void (*R_RegisterFont)(const char* fontName, int pointSize, fontInfo_t* font);
    void (*R_ClearScene)(void);
    void (*R_AddRefEntityToScene)(const refEntity_t* re);
    void (*R_AddPolyToScene)(qhandle_t hShader, int numVerts, const polyVert_t* verts, int num);
    void (*R_AddPolysToScene)(qhandle_t hShader, int numVerts, const polyVert_t* verts, int num);
    int (*R_LightForPoint)(vec3_t point, vec3_t ambientLight, vec3_t directedLight, vec3_t lightDir);
    void (*R_AddLightToScene)(const vec3_t org, float intensity, float r, float g, float b);
    void (*R_AddAdditiveLightToScene)(const vec3_t org, float intensity, float r, float g, float b);
    void (*R_RenderScene)(const refdef_t* fd);
    void (*R_SetColor)(const float* rgba);
    void (*R_DrawStretchPic)(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader);
    void (*R_ModelBounds)(clipHandle_t model, vec3_t mins, vec3_t maxs);
    int (*R_LerpTag)(orientation_t* tag, clipHandle_t mod, int startFrame, int endFrame, float frac, const char* tagName);
    void (*R_RemapShader)(const char* oldShader, const char* newShader, const char* timeOffset);
    void (*GetGlconfig)(glconfig_t* glconfig);
    void (*GetGameState)(gameState_t* gamestate);
    void (*GetCurrentSnapshotNumber)(int* snapshotNumber, int* serverTime);
    qboolean(*GetSnapshot)(int snapshotNumber, snapshot_t* snapshot);
    qboolean(*GetServerCommand)(int serverCommandNumber);
    int (*GetCurrentCmdNumber)(void);
    qboolean(*GetUserCmd)(int cmdNumber, usercmd_t* ucmd);
    void (*SetUserCmdValue)(int stateValue, float sensitivityScale);
    int (*MemoryRemaining)(void);
    qboolean(*Key_IsDown)(int keynum);
    int (*Key_GetCatcher)(void);
    void (*Key_SetCatcher)(int catcher);
    int (*Key_GetKey)(const char* binding);
    int (*PC_AddGlobalDefine)(char* define);
    int (*PC_LoadSource)(const char* filename);
    int (*PC_FreeSource)(int handle);
    int (*PC_ReadToken)(int handle, pc_token_t* pc_token);
    int (*PC_SourceFileAndLine)(int handle, char* filename, int* line);
    void (*S_StopBackgroundTrack)(void);
    int (*RealTime)(qtime_t* qtime);
    int (*CIN_PlayCinematic)(const char* arg0, int xpos, int ypos, int width, int height, int bits);
    e_status(*CIN_StopCinematic)(int handle);
    e_status(*CIN_RunCinematic)(int handle);
    void (*CIN_DrawCinematic)(int handle);
    void (*CIN_SetExtents)(int handle, int x, int y, int w, int h);
    qboolean(*GetEntityToken)(char* buffer, int bufferSize);
    qboolean(*R_inPVS)(const vec3_t p1, const vec3_t p2);
    void	(*R_AddCoronaToScene)(const vec3_t org, float r, float g, float b, float scale, int id, int flags);
} cgameImport_t;

/*
==================================================================

functions exported to the main executable

==================================================================
*/

typedef struct {
    void (*CG_Init)(int, int, int);
    void (*CG_Shutdown)(void);
    qboolean (*CG_ConsoleCommand)(void);
    void (*CG_DrawActiveFrame)(int, stereoFrame_t, qboolean);
    int (*CG_CrosshairPlayer)(void);
    int (*CG_LastAttacker)(void);
    void (*CG_KeyEvent)(int, qboolean);
    void (*CG_MouseEvent)(int, int);
    void (*CG_EventHandling)(int);
} cgameExport_t;

extern cgameExport_t* cgame;

//----------------------------------------------
