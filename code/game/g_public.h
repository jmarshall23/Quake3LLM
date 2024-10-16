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

// g_public.h -- game module information visible to server

#define	GAME_API_VERSION	8

// entity->svFlags
// the server does not know how to interpret most of the values
// in entityStates (level eType), so the game must explicitly flag
// special server behaviors
#define	SVF_NOCLIENT			0x00000001	// don't send entity to clients, even if it has effects

// TTimo
// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=551
#define SVF_CLIENTMASK 0x00000002

#define SVF_BOT					0x00000008	// set if the entity is a bot
#define	SVF_BROADCAST			0x00000020	// send to all connected clients
#define	SVF_PORTAL				0x00000040	// merge a second pvs at origin2 into snapshots
#define	SVF_USE_CURRENT_ORIGIN	0x00000080	// entity->r.currentOrigin instead of entity->s.origin
											// for link position (missiles and movers)
#define SVF_SINGLECLIENT		0x00000100	// only send to a single client (entityShared_t->singleClient)
#define SVF_NOSERVERINFO		0x00000200	// don't send CS_SERVERINFO updates to this client
											// so that it can be updated for ping tools without
											// lagging clients
#define SVF_CAPSULE				0x00000400	// use capsule for collision detection instead of bbox
#define SVF_NOTSINGLECLIENT		0x00000800	// send entity to everyone but one client
											// (entityShared_t->singleClient)



//===============================================================


typedef struct {
	entityState_t	s;				// communicated by server to clients

	qboolean	linked;				// qfalse if not in any good cluster
	int			linkcount;

	int			svFlags;			// SVF_NOCLIENT, SVF_BROADCAST, etc

	// only send to this client when SVF_SINGLECLIENT is set	
	// if SVF_CLIENTMASK is set, use bitmask for clients to send to (maxclients must be <= 32, up to the mod to enforce this)
	int			singleClient;		

	qboolean	bmodel;				// if false, assume an explicit mins / maxs bounding box
									// only set by trap_SetBrushModel
	vec3_t		mins, maxs;
	int			contents;			// CONTENTS_TRIGGER, CONTENTS_SOLID, CONTENTS_BODY, etc
									// a non-solid entity should set to 0

	vec3_t		absmin, absmax;		// derived from mins/maxs and origin + rotation

	// currentOrigin will be used for all collision detection and world linking.
	// it will not necessarily be the same as the trajectory evaluation for the current
	// time, because each entity must be moved one at a time after time is advanced
	// to avoid simultanious collision issues
	vec3_t		currentOrigin;
	vec3_t		currentAngles;

	// when a trace call is made and passEntityNum != ENTITYNUM_NONE,
	// an ent will be excluded from testing if:
	// ent->s.number == passEntityNum	(don't interact with self)
	// ent->s.ownerNum = passEntityNum	(don't interact with your own missiles)
	// entity[ent->s.ownerNum].ownerNum = passEntityNum	(don't interact with other missiles from owner)
	int			ownerNum;
} entityShared_t;



// the server looks at a sharedEntity, which is the start of the game's gentity_t structure
typedef struct {
	entityState_t	s;				// communicated by server to clients
	entityShared_t	r;				// shared by both the server system and game
} sharedEntity_t;



//===============================================================

//
// system traps provided by the main engine
//
typedef struct {
    void (*Print)(const char* fmt);
	void (*Error)(int errorLevel, const char* fmt);
	int (*Milliseconds)(void);
	void (*Cvar_Register)(void* vmCvar, const char* varName, const char* defaultValue, int flags);
	void (*Cvar_Update)(void* vmCvar);
	void (*Cvar_Set)(const char* var_name, const char* value);
	int (*Cvar_VariableIntegerValue)(const char* var_name);
	void (*Cvar_VariableStringBuffer)(const char* var_name, char* buffer, int bufsize);
	int (*Argc)(void);
	void (*Argv)(int n, char* buffer, int bufferLength);
	void (*SendConsoleCommand)(int exec_when, const char* text);
	int (*FS_FOpenFile)(const char* qpath, fileHandle_t* f, fsMode_t mode);
	void (*FS_Read)(void* buffer, int len, int f);
	void (*FS_Write)(const void* buffer, int len, int f);
	void (*FS_FCloseFile)(int f);
	int (*FS_GetFileList)(const char* path, const char* extension, char* listbuf, int bufsize);
	int (*FS_Seek)(int f, long offset, int origin);
	void (*LocateGameData)(void* gEnts, int numGEntities, int sizeofGEntity_t, void* clients, int sizeofGClient);
	void (*DropClient)(int clientNum, const char* reason);
	void (*SendServerCommand)(int clientNum, const char* text);
	void (*SetConfigstring)(int num, const char* string);
	void (*GetConfigstring)(int num, char* buffer, int bufferSize);
	void (*SetUserinfo)(int num, const char* buffer);
	void (*GetUserinfo)(int num, char* buffer, int bufferSize);
	void (*GetServerinfo)(char* buffer, int bufferSize);
	void (*SetBrushModel)(void* ent, const char* name);
	void (*Trace)(trace_t* results, const vec3_t start, vec3_t mins, vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask, int capsule);
	int (*PointContents)(const void* point, int passEntityNum);
	qboolean (*inPVS)(const void* p1, const void* p2);
	qboolean (*inPVSIgnorePortals)(const void* p1, const void* p2);
	void (*AdjustAreaPortalState)(sharedEntity_t* ent, qboolean open);
	qboolean (*AreasConnected)(int area1, int area2);
	void (*LinkEntity)(void* ent);
	void (*UnlinkEntity)(void* ent);
	int (*EntitiesInBox)(const void* mins, const void* maxs, int* list, int maxcount);
	qboolean (*EntityContact)(vec3_t mins, vec3_t maxs, const sharedEntity_t* gEnt, int capsule);
	int (*BotAllocateClient)(void);
	void (*BotFreeClient)(int clientNum);
	void (*GetUsercmd)(int clientNum, void* cmd);
	int (*GetEntityToken)(char* buffer, int bufferSize);
	int (*DebugPolygonCreate)(int color, int numPoints, void* points);
	void (*DebugPolygonDelete)(int id);
	int (*RealTime)(void* qtime);
	// BotLib functions
	int (*BotLibSetup)(void);
	int (*BotLibShutdown)(void);
	int (*BotLibVarSet)(char* var_name, char* value);
	int (*BotLibVarGet)(char* var_name, char* value, int size);
	int (*BotLibDefine)(char* string);
	int (*BotLibStartFrame)(float time);
	int (*BotLibLoadMap)(const char* mapname);
	int (*BotLibUpdateEntity)(int ent, void* bue);
	int (*BotLibTest)(int parm0, char* parm1, void* parm2, void* parm3);
	int (*BotGetSnapshotEntity)(int clientNum, int sequence);
	int (*BotGetServerCommand)(int clientNum, char* message, int size);
	void (*BotUserCommand)(int clientNum, void* ucmd);
	void (*AAS_EntityInfo)(int entnum, void* info);
	int (*AAS_Initialized)(void);
	void (*AAS_PresenceTypeBoundingBox)(int presencetype, void* mins, void* maxs);
	float (*AAS_Time)(void);
	int (*AAS_PointAreaNum)(void* point);
	int (*AAS_PointReachabilityAreaIndex)(void* point);
	int (*AAS_TraceAreas)(void* start, void* end, int* areas, void* points, int maxareas);
	int (*AAS_BBoxAreas)(void* absmins, void* absmaxs, int* areas, int maxareas);
	int (*AAS_AreaInfo)(int areanum, void* info);
	int (*AAS_PointContents)(void* point);
	int (*AAS_NextBSPEntity)(int ent);
	int (*AAS_ValueForBSPEpairKey)(int ent, char* key, char* value, int size);
	int (*AAS_VectorForBSPEpairKey)(int ent, char* key, void* v);
	int (*AAS_FloatForBSPEpairKey)(int ent, char* key, float* value);
	int (*AAS_IntForBSPEpairKey)(int ent, char* key, int* value);
	int (*AAS_AreaReachability)(int areanum);
	int (*AAS_AreaTravelTimeToGoalArea)(int areanum, void* origin, int goalareanum, int travelflags);
	int (*AAS_EnableRoutingArea)(int areanum, int enable);
	int (*AAS_PredictRoute)(void* route, int areanum, void* origin, int goalareanum, int travelflags, int maxareas, int maxtime, int stopevent, int stopcontents, int stoptfl, int stopareanum);
	int (*AAS_AlternativeRouteGoals)(void* start, int startareanum, void* goal, int goalareanum, int travelflags, void* altroutegoals, int maxaltroutegoals, int type);
	int (*AAS_Swimming)(void* origin);
	int (*AAS_PredictClientMovement)(void* move, int entnum, void* origin, int presencetype, int onground, void* velocity, void* cmdmove, int cmdframes, int maxframes, float frametime, int stopevent, int stopareanum, int visualize);
	void (*EA_Say)(int client, char* str);
	void (*EA_SayTeam)(int client, char* str);
	void (*EA_Command)(int client, char* command);
	void (*EA_Action)(int client, int action);
	void (*EA_Gesture)(int client);
	void (*EA_Talk)(int client);
	void (*EA_Attack)(int client);
	void (*EA_Use)(int client);
	void (*EA_Respawn)(int client);
	void (*EA_Crouch)(int client);
	void (*EA_MoveUp)(int client);
	void (*EA_MoveDown)(int client);
	void (*EA_MoveForward)(int client);
	void (*EA_MoveBack)(int client);
	void (*EA_MoveLeft)(int client);
	void (*EA_MoveRight)(int client);
	void (*EA_SelectWeapon)(int client, int weapon);
	void (*EA_Jump)(int client);
	void (*EA_DelayedJump)(int client);
	void (*EA_Move)(int client, void* dir, float speed);
	void (*EA_View)(int client, void* viewangles);
	void (*EA_EndRegular)(int client, float thinktime);
	void (*EA_GetInput)(int client, float thinktime, void* input);
	void (*EA_ResetInput)(int client);
	int (*BotLoadCharacter)(char* charfile, float skill);
	void (*BotFreeCharacter)(int character);
	float (*Characteristic_Float)(int character, int index);
	float (*Characteristic_BFloat)(int character, int index, float min, float max);
	int (*Characteristic_Integer)(int character, int index);
	int (*Characteristic_BInteger)(int character, int index, int min, int max);
	void (*Characteristic_String)(int character, int index, char* buf, int size);
	int (*BotAllocChatState)(void);
	void (*BotFreeChatState)(int handle);
	void (*BotQueueConsoleMessage)(int chatstate, int type, char* message);
	void (*BotRemoveConsoleMessage)(int chatstate, int handle);
	int (*BotNextConsoleMessage)(int chatstate, void* cm);
	int (*BotNumConsoleMessages)(int chatstate);
	void (*BotInitialChat)(int chatstate, char* type, int mcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7);
	int (*BotNumInitialChats)(int chatstate, char* type);
	int (*BotReplyChat)(int chatstate, char* message, int mcontext, int vcontext, char* var0, char* var1, char* var2, char* var3, char* var4, char* var5, char* var6, char* var7);
	int (*BotChatLength)(int chatstate);
	void (*BotEnterChat)(int chatstate, int client, int sendto);
	void (*BotGetChatMessage)(int chatstate, char* buf, int size);
	int (*StringContains)(char* str1, char* str2, int casesensitive);
	int (*BotFindMatch)(char* str, void* match, unsigned long int context);
	void (*BotMatchVariable)(void* match, int variable, char* buf, int size);
	void (*UnifyWhiteSpaces)(char* string);
	void (*BotReplaceSynonyms)(char* string, unsigned long int context);
	int (*BotLoadChatFile)(int chatstate, char* chatfile, char* chatname);
	void (*BotSetChatGender)(int chatstate, int gender);
	void (*BotSetChatName)(int chatstate, char* name, int client);
	void (*BotResetGoalState)(int goalstate);
	void (*BotResetAvoidGoals)(int goalstate);
	void (*BotRemoveFromAvoidGoals)(int goalstate, int number);
	void (*BotPushGoal)(int goalstate, void* goal);
	void (*BotPopGoal)(int goalstate);
	void (*BotEmptyGoalStack)(int goalstate);
	void (*BotDumpAvoidGoals)(int goalstate);
	void (*BotDumpGoalStack)(int goalstate);
	void (*BotGoalName)(int number, char* name, int size);
	int (*BotGetTopGoal)(int goalstate, void* goal);
	int (*BotGetSecondGoal)(int goalstate, void* goal);
	int (*BotChooseLTGItem)(int goalstate, void* origin, int* inventory, int travelflags);
	int (*BotChooseNBGItem)(int goalstate, void* origin, int* inventory, int travelflags, void* ltg, float maxtime);
	int (*BotTouchingGoal)(void* origin, void* goal);
	int (*BotItemGoalInVisButNotVisible)(int viewer, void* eye, void* viewangles, void* goal);
	int (*BotGetLevelItemGoal)(int index, char* classname, void* goal);
	int (*BotGetNextCampSpotGoal)(int num, void* goal);
	int (*BotGetMapLocationGoal)(char* name, void* goal);
	float (*BotAvoidGoalTime)(int goalstate, int number);
	void (*BotSetAvoidGoalTime)(int goalstate, int number, float avoidtime);
	void (*BotInitLevelItems)(void);
	void (*BotUpdateEntityItems)(void);
	int (*BotLoadItemWeights)(int goalstate, char* filename);
	void (*BotFreeItemWeights)(int goalstate);
	void (*BotInterbreedGoalFuzzyLogic)(int parent1, int parent2, int child);
	void (*BotSaveGoalFuzzyLogic)(int goalstate, char* filename);
	void (*BotMutateGoalFuzzyLogic)(int goalstate, float range);
	int (*BotAllocGoalState)(int state);
	void (*BotFreeGoalState)(int handle);
	void (*BotResetMoveState)(int movestate);
	void (*BotAddAvoidSpot)(int movestate, void* origin, float radius, int type);
	void (*BotMoveToGoal)(void* result, int movestate, void* goal, int travelflags);
	int (*BotMoveInDirection)(int movestate, void* dir, float speed, int type);
	void (*BotResetAvoidReach)(int movestate);
	void (*BotResetLastAvoidReach)(int movestate);
	int (*BotReachabilityArea)(void* origin, int testground);
	int (*BotMovementViewTarget)(int movestate, void* goal, int travelflags, float lookahead, void* target);
	int (*BotPredictVisiblePosition)(void* origin, int areanum, void* goal, int travelflags, void* target);
	int (*BotAllocMoveState)(void);
	void (*BotFreeMoveState)(int handle);
	void (*BotInitMoveState)(int handle, void* initmove);
	int (*BotChooseBestFightWeapon)(int weaponstate, int* inventory);
	void (*BotGetWeaponInfo)(int weaponstate, int weapon, void* weaponinfo);
	int (*BotLoadWeaponWeights)(int weaponstate, char* filename);
	int (*BotAllocWeaponState)(void);
	void (*BotFreeWeaponState)(int weaponstate);
	void (*BotResetWeaponState)(int weaponstate);
	int (*GeneticParentsAndChildSelection)(int numranks, float* ranks, int* parent1, int* parent2, int* child);
	int (*PC_LoadSource)(const char* filename);
	int (*PC_FreeSource)(int handle);
	int (*PC_ReadToken)(int handle, void* pc_token);
	int (*PC_SourceFileAndLine)(int handle, char* filename, int* line);

	void (*LLM_PushPrompt)(int chatstate, const char* prompt);
} gameImport_t;


//
// functions exported by the game subsystem
//
typedef struct {
	void (*G_InitGame)(int, int, int);
	void (*G_ShutdownGame)(int);
	char *(*ClientConnect)(int, qboolean, qboolean);
	void (*ClientThink)(int);
	void (*ClientUserinfoChanged)(int);
	void (*ClientDisconnect)(int);
	void (*ClientBegin)(int);
	void (*ClientCommand)(int);
	void (*G_RunFrame)(int);
	qboolean (*ConsoleCommand)(void);
	int (*BotAIStartFrame)(int);
} gameExport_t;

extern gameExport_t* game;

