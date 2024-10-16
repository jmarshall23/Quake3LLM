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
// sv_game.c -- interface to the game dll

#include "server.h"

#include "../game/botlib.h"
#include "../llm/llm.h"

gameExport_t* game;
static intptr_t dllHandle;
botlib_export_t	*botlib_export;

void SV_GameError( const char *string ) {
	Com_Error( ERR_DROP, "%s", string );
}

void SV_GamePrint( const char *string ) {
	Com_Printf( "%s", string );
}

// these functions must be used instead of pointer arithmetic, because
// the game allocates gentities with private information after the server shared part
int	SV_NumForGentity( sharedEntity_t *ent ) {
	int		num;

	num = ( (byte *)ent - (byte *)sv.gentities ) / sv.gentitySize;

	return num;
}

sharedEntity_t *SV_GentityNum( int num ) {
	sharedEntity_t *ent;

	ent = (sharedEntity_t *)((byte *)sv.gentities + sv.gentitySize*(num));

	return ent;
}

playerState_t *SV_GameClientNum( int num ) {
	playerState_t	*ps;

	ps = (playerState_t *)((byte *)sv.gameClients + sv.gameClientSize*(num));

	return ps;
}

svEntity_t	*SV_SvEntityForGentity( sharedEntity_t *gEnt ) {
	if ( !gEnt || gEnt->s.number < 0 || gEnt->s.number >= MAX_GENTITIES ) {
		Com_Error( ERR_DROP, "SV_SvEntityForGentity: bad gEnt" );
	}
	return &sv.svEntities[ gEnt->s.number ];
}

sharedEntity_t *SV_GEntityForSvEntity( svEntity_t *svEnt ) {
	int		num;

	num = svEnt - sv.svEntities;
	return SV_GentityNum( num );
}

/*
===============
SV_GameSendServerCommand

Sends a command string to a client
===============
*/
void SV_GameSendServerCommand( int clientNum, const char *text ) {
	if ( clientNum == -1 ) {
		SV_SendServerCommand( NULL, "%s", text );
	} else {
		if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
			return;
		}
		SV_SendServerCommand( svs.clients + clientNum, "%s", text );	
	}
}


/*
===============
SV_GameDropClient

Disconnects the client with a message
===============
*/
void SV_GameDropClient( int clientNum, const char *reason ) {
	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		return;
	}
	SV_DropClient( svs.clients + clientNum, reason );	
}


/*
=================
SV_SetBrushModel

sets mins and maxs for inline bmodels
=================
*/
void SV_SetBrushModel( sharedEntity_t *ent, const char *name ) {
	clipHandle_t	h;
	vec3_t			mins, maxs;

	if (!name) {
		Com_Error( ERR_DROP, "SV_SetBrushModel: NULL" );
	}

	if (name[0] != '*') {
		Com_Error( ERR_DROP, "SV_SetBrushModel: %s isn't a brush model", name );
	}


	ent->s.modelindex = atoi( name + 1 );

	h = CM_InlineModel( ent->s.modelindex );
	CM_ModelBounds( h, mins, maxs );
	VectorCopy (mins, ent->r.mins);
	VectorCopy (maxs, ent->r.maxs);
	ent->r.bmodel = qtrue;

	ent->r.contents = -1;		// we don't know exactly what is in the brushes

	SV_LinkEntity( ent );		// FIXME: remove
}



/*
=================
SV_inPVS

Also checks portalareas so that doors block sight
=================
*/
qboolean SV_inPVS (const vec3_t p1, const vec3_t p2)
{
	int		leafnum;
	int		cluster;
	int		area1, area2;
	byte	*mask;

	leafnum = CM_PointLeafnum (p1);
	cluster = CM_LeafCluster (leafnum);
	area1 = CM_LeafArea (leafnum);
	mask = CM_ClusterPVS (cluster);

	leafnum = CM_PointLeafnum (p2);
	cluster = CM_LeafCluster (leafnum);
	area2 = CM_LeafArea (leafnum);
	if ( mask && (!(mask[cluster>>3] & (1<<(cluster&7)) ) ) )
		return qfalse;
	if (!CM_AreasConnected (area1, area2))
		return qfalse;		// a door blocks sight
	return qtrue;
}


/*
=================
SV_inPVSIgnorePortals

Does NOT check portalareas
=================
*/
qboolean SV_inPVSIgnorePortals( const vec3_t p1, const vec3_t p2)
{
	int		leafnum;
	int		cluster;
	int		area1, area2;
	byte	*mask;

	leafnum = CM_PointLeafnum (p1);
	cluster = CM_LeafCluster (leafnum);
	area1 = CM_LeafArea (leafnum);
	mask = CM_ClusterPVS (cluster);

	leafnum = CM_PointLeafnum (p2);
	cluster = CM_LeafCluster (leafnum);
	area2 = CM_LeafArea (leafnum);

	if ( mask && (!(mask[cluster>>3] & (1<<(cluster&7)) ) ) )
		return qfalse;

	return qtrue;
}


/*
========================
SV_AdjustAreaPortalState
========================
*/
void SV_AdjustAreaPortalState( sharedEntity_t *ent, qboolean open ) {
	svEntity_t	*svEnt;

	svEnt = SV_SvEntityForGentity( ent );
	if ( svEnt->areanum2 == -1 ) {
		return;
	}
	CM_AdjustAreaPortalState( svEnt->areanum, svEnt->areanum2, open );
}


/*
==================
SV_GameAreaEntities
==================
*/
qboolean	SV_EntityContact( vec3_t mins, vec3_t maxs, const sharedEntity_t *gEnt, int capsule ) {
	const float	*origin, *angles;
	clipHandle_t	ch;
	trace_t			trace;

	// check for exact collision
	origin = gEnt->r.currentOrigin;
	angles = gEnt->r.currentAngles;

	ch = SV_ClipHandleForEntity( gEnt );
	CM_TransformedBoxTrace ( &trace, vec3_origin, vec3_origin, mins, maxs,
		ch, -1, origin, angles, capsule );

	return trace.startsolid;
}


/*
===============
SV_GetServerinfo

===============
*/
void SV_GetServerinfo( char *buffer, int bufferSize ) {
	if ( bufferSize < 1 ) {
		Com_Error( ERR_DROP, "SV_GetServerinfo: bufferSize == %i", bufferSize );
	}
	Q_strncpyz( buffer, Cvar_InfoString( CVAR_SERVERINFO ), bufferSize );
}

/*
===============
SV_LocateGameData

===============
*/
void SV_LocateGameData( sharedEntity_t *gEnts, int numGEntities, int sizeofGEntity_t,
					   playerState_t *clients, int sizeofGameClient ) {
	sv.gentities = gEnts;
	sv.gentitySize = sizeofGEntity_t;
	sv.num_entities = numGEntities;

	sv.gameClients = clients;
	sv.gameClientSize = sizeofGameClient;
}


/*
===============
SV_GetUsercmd

===============
*/
void SV_GetUsercmd( int clientNum, usercmd_t *cmd ) {
	if ( clientNum < 0 || clientNum >= sv_maxclients->integer ) {
		Com_Error( ERR_DROP, "SV_GetUsercmd: bad clientNum:%i", clientNum );
	}
	*cmd = svs.clients[clientNum].lastUsercmd;
}

//==============================================

/*
===============
SV_ShutdownGameProgs

Called every time a map changes
===============
*/
void SV_ShutdownGameProgs( void ) {
    if (!dllHandle) {
        return;
    }

	game->G_ShutdownGame(qfalse);
    Sys_UnloadDll(dllHandle);
    game = NULL;
    dllHandle = NULL;
}

/*
==================
SV_InitGameVM

Called for both a full init and a restart
==================
*/
static void SV_InitGameVM( qboolean restart ) {
	int		i;

	// start the entity parsing at the beginning
	sv.entityParsePoint = CM_EntityString();

	// clear all gentity pointers that might still be set from
	// a previous level
	// https://zerowing.idsoftware.com/bugzilla/show_bug.cgi?id=522
	//   now done before GAME_INIT call
	for ( i = 0 ; i < sv_maxclients->integer ; i++ ) {
		svs.clients[i].gentity = NULL;
	}
	
	// use the current msec count for a random seed
	// init for this gamestate
    game->G_InitGame(svs.time, Com_Milliseconds(), restart);
}



/*
===================
SV_RestartGameProgs

Called on a map_restart, but not on a normal map change
===================
*/
void SV_RestartGameProgs( void ) {
	if ( !dllHandle ) {
		return;
	}
    SV_ShutdownGameProgs();

    SV_InitGameProgs();
}
/*
===============
SV_GetEntityToken
===============
*/
int SV_GetEntityToken(char* buffer, int bufferSize) {
    const char* s;

    s = COM_Parse(&sv.entityParsePoint);
    Q_strncpyz(buffer, s, bufferSize);
    if (!sv.entityParsePoint && !s[0]) {
        return qfalse;
    }

    return qtrue;
}

/*
===============
SV_InitGameProgs

Called on a normal map change, not on a map_restart
===============
*/
void SV_InitGameProgs( void ) {
	cvar_t	*var;
	//FIXME these are temp while I make bots run in vm
	extern int	bot_enable;
	static gameImport_t gameExports;

	gameExports.Print = Com_Printf;
	gameExports.Error = Com_Error;
	gameExports.Milliseconds = Sys_Milliseconds;
	gameExports.Cvar_Register = Cvar_Register;
	gameExports.Cvar_Update = Cvar_Update;
	gameExports.Cvar_Set = Cvar_Set;
	gameExports.Cvar_VariableIntegerValue = Cvar_VariableIntegerValue;
	gameExports.Cvar_VariableStringBuffer = Cvar_VariableStringBuffer;
	gameExports.Argc = Cmd_Argc;
	gameExports.Argv = Cmd_ArgvBuffer;
	gameExports.SendConsoleCommand = Cbuf_ExecuteText;
	gameExports.FS_FOpenFile = FS_FOpenFileByMode;
	gameExports.FS_Read = FS_Read2;
	gameExports.FS_Write = FS_Write;
	gameExports.FS_FCloseFile = FS_FCloseFile;
	gameExports.FS_GetFileList = FS_GetFileList;
	gameExports.FS_Seek = FS_Seek;
	gameExports.LocateGameData = SV_LocateGameData;
	gameExports.DropClient = SV_GameDropClient;
	gameExports.SendServerCommand = SV_GameSendServerCommand;
	gameExports.SetConfigstring = SV_SetConfigstring;
	gameExports.GetConfigstring = SV_GetConfigstring;
	gameExports.SetUserinfo = SV_SetUserinfo;
	gameExports.GetUserinfo = SV_GetUserinfo;
	gameExports.GetServerinfo = SV_GetServerinfo;
	gameExports.SetBrushModel = SV_SetBrushModel;
	gameExports.Trace = SV_Trace;
	gameExports.PointContents = SV_PointContents;
	gameExports.inPVS = SV_inPVS;
	gameExports.inPVSIgnorePortals = SV_inPVSIgnorePortals;
	gameExports.AdjustAreaPortalState = SV_AdjustAreaPortalState;
	gameExports.AreasConnected = CM_AreasConnected;
	gameExports.LinkEntity = SV_LinkEntity;
	gameExports.UnlinkEntity = SV_UnlinkEntity;
	gameExports.EntitiesInBox = SV_AreaEntities;
	gameExports.EntityContact = SV_EntityContact;
	gameExports.BotAllocateClient = SV_BotAllocateClient;
	gameExports.BotFreeClient = SV_BotFreeClient;
	gameExports.GetUsercmd = SV_GetUsercmd;
	gameExports.GetEntityToken = SV_GetEntityToken;
	gameExports.DebugPolygonCreate = BotImport_DebugPolygonCreate;
	gameExports.DebugPolygonDelete = BotImport_DebugPolygonDelete;
	gameExports.RealTime = Com_RealTime;
	gameExports.BotLibSetup = SV_BotLibSetup;
	gameExports.BotLibShutdown = SV_BotLibShutdown;
	gameExports.BotLibVarSet = botlib_export->BotLibVarSet;
	gameExports.BotLibVarGet = botlib_export->BotLibVarGet;
	gameExports.BotLibDefine = botlib_export->PC_AddGlobalDefine;
	gameExports.BotLibStartFrame = botlib_export->BotLibStartFrame;
	gameExports.BotLibLoadMap = botlib_export->BotLibLoadMap;
	gameExports.BotLibUpdateEntity = botlib_export->BotLibUpdateEntity;
	gameExports.BotLibTest = botlib_export->Test;
	gameExports.BotGetSnapshotEntity = SV_BotGetSnapshotEntity;
	gameExports.BotGetServerCommand = SV_BotGetConsoleMessage;
	gameExports.BotUserCommand = SV_ClientNumThink;
	gameExports.AAS_EntityInfo = botlib_export->aas.AAS_EntityInfo;
	gameExports.AAS_Initialized = botlib_export->aas.AAS_Initialized;
	gameExports.AAS_PresenceTypeBoundingBox = botlib_export->aas.AAS_PresenceTypeBoundingBox;
	gameExports.AAS_Time = botlib_export->aas.AAS_Time;
	gameExports.AAS_PointAreaNum = botlib_export->aas.AAS_PointAreaNum;
	gameExports.AAS_PointReachabilityAreaIndex = botlib_export->aas.AAS_PointReachabilityAreaIndex;
	gameExports.AAS_TraceAreas = botlib_export->aas.AAS_TraceAreas;
	gameExports.AAS_BBoxAreas = botlib_export->aas.AAS_BBoxAreas;
	gameExports.AAS_AreaInfo = botlib_export->aas.AAS_AreaInfo;
	gameExports.AAS_PointContents = botlib_export->aas.AAS_PointContents;
	gameExports.AAS_NextBSPEntity = botlib_export->aas.AAS_NextBSPEntity;
	gameExports.AAS_ValueForBSPEpairKey = botlib_export->aas.AAS_ValueForBSPEpairKey;
	gameExports.AAS_VectorForBSPEpairKey = botlib_export->aas.AAS_VectorForBSPEpairKey;
	gameExports.AAS_FloatForBSPEpairKey = botlib_export->aas.AAS_FloatForBSPEpairKey;
	gameExports.AAS_IntForBSPEpairKey = botlib_export->aas.AAS_IntForBSPEpairKey;
	gameExports.AAS_AreaReachability = botlib_export->aas.AAS_AreaReachability;
	gameExports.AAS_AreaTravelTimeToGoalArea = botlib_export->aas.AAS_AreaTravelTimeToGoalArea;
	gameExports.AAS_EnableRoutingArea = botlib_export->aas.AAS_EnableRoutingArea;
	gameExports.AAS_PredictRoute = botlib_export->aas.AAS_PredictRoute;
	gameExports.AAS_AlternativeRouteGoals = botlib_export->aas.AAS_AlternativeRouteGoals;
	gameExports.AAS_Swimming = botlib_export->aas.AAS_Swimming;
	gameExports.AAS_PredictClientMovement = botlib_export->aas.AAS_PredictClientMovement;
	gameExports.EA_Say = botlib_export->ea.EA_Say;
	gameExports.EA_SayTeam = botlib_export->ea.EA_SayTeam;
	gameExports.EA_Command = botlib_export->ea.EA_Command;
	gameExports.EA_Action = botlib_export->ea.EA_Action;
	gameExports.EA_Gesture = botlib_export->ea.EA_Gesture;
	gameExports.EA_Talk = botlib_export->ea.EA_Talk;
	gameExports.EA_Attack = botlib_export->ea.EA_Attack;
	gameExports.EA_Use = botlib_export->ea.EA_Use;
	gameExports.EA_Respawn = botlib_export->ea.EA_Respawn;
	gameExports.EA_Crouch = botlib_export->ea.EA_Crouch;
	gameExports.EA_MoveUp = botlib_export->ea.EA_MoveUp;
	gameExports.EA_MoveDown = botlib_export->ea.EA_MoveDown;
	gameExports.EA_MoveForward = botlib_export->ea.EA_MoveForward;
	gameExports.EA_MoveBack = botlib_export->ea.EA_MoveBack;
	gameExports.EA_MoveLeft = botlib_export->ea.EA_MoveLeft;
	gameExports.EA_MoveRight = botlib_export->ea.EA_MoveRight;
	gameExports.EA_SelectWeapon = botlib_export->ea.EA_SelectWeapon;
	gameExports.EA_Jump = botlib_export->ea.EA_Jump;
	gameExports.EA_DelayedJump = botlib_export->ea.EA_DelayedJump;
	gameExports.EA_Move = botlib_export->ea.EA_Move;
	gameExports.EA_View = botlib_export->ea.EA_View;
	gameExports.EA_EndRegular = botlib_export->ea.EA_EndRegular;
	gameExports.EA_GetInput = botlib_export->ea.EA_GetInput;
	gameExports.EA_ResetInput = botlib_export->ea.EA_ResetInput;
	gameExports.BotLoadCharacter = botlib_export->ai.BotLoadCharacter;
	gameExports.BotFreeCharacter = botlib_export->ai.BotFreeCharacter;
	gameExports.Characteristic_Float = botlib_export->ai.Characteristic_Float;
	gameExports.Characteristic_BFloat = botlib_export->ai.Characteristic_BFloat;
	gameExports.Characteristic_Integer = botlib_export->ai.Characteristic_Integer;
	gameExports.Characteristic_BInteger = botlib_export->ai.Characteristic_BInteger;
	gameExports.Characteristic_String = botlib_export->ai.Characteristic_String;
	gameExports.BotAllocChatState = botlib_export->ai.BotAllocChatState;
	gameExports.BotFreeChatState = botlib_export->ai.BotFreeChatState;
	gameExports.BotQueueConsoleMessage = botlib_export->ai.BotQueueConsoleMessage;
	gameExports.BotRemoveConsoleMessage = botlib_export->ai.BotRemoveConsoleMessage;
	gameExports.BotNextConsoleMessage = botlib_export->ai.BotNextConsoleMessage;
	gameExports.BotNumConsoleMessages = botlib_export->ai.BotNumConsoleMessages;
	gameExports.BotInitialChat = botlib_export->ai.BotInitialChat;
	gameExports.BotNumInitialChats = botlib_export->ai.BotNumInitialChats;
	gameExports.BotReplyChat = botlib_export->ai.BotReplyChat;
	gameExports.BotChatLength = botlib_export->ai.BotChatLength;
	gameExports.BotEnterChat = botlib_export->ai.BotEnterChat;
	gameExports.BotGetChatMessage = botlib_export->ai.BotGetChatMessage;
	gameExports.StringContains = botlib_export->ai.StringContains;
	gameExports.BotFindMatch = botlib_export->ai.BotFindMatch;
	gameExports.BotMatchVariable = botlib_export->ai.BotMatchVariable;
	gameExports.UnifyWhiteSpaces = botlib_export->ai.UnifyWhiteSpaces;
	gameExports.BotReplaceSynonyms = botlib_export->ai.BotReplaceSynonyms;
	gameExports.BotLoadChatFile = botlib_export->ai.BotLoadChatFile;
	gameExports.BotSetChatGender = botlib_export->ai.BotSetChatGender;
	gameExports.BotSetChatName = botlib_export->ai.BotSetChatName;
	gameExports.BotResetGoalState = botlib_export->ai.BotResetGoalState;
	gameExports.BotResetAvoidGoals = botlib_export->ai.BotResetAvoidGoals;
	gameExports.BotRemoveFromAvoidGoals = botlib_export->ai.BotRemoveFromAvoidGoals;
	gameExports.BotPushGoal = botlib_export->ai.BotPushGoal;
	gameExports.BotPopGoal = botlib_export->ai.BotPopGoal;
	gameExports.BotEmptyGoalStack = botlib_export->ai.BotEmptyGoalStack;
	gameExports.BotDumpAvoidGoals = botlib_export->ai.BotDumpAvoidGoals;
	gameExports.BotDumpGoalStack = botlib_export->ai.BotDumpGoalStack;
	gameExports.BotGoalName = botlib_export->ai.BotGoalName;
	gameExports.BotGetTopGoal = botlib_export->ai.BotGetTopGoal;
	gameExports.BotGetSecondGoal = botlib_export->ai.BotGetSecondGoal;
	gameExports.BotChooseLTGItem = botlib_export->ai.BotChooseLTGItem;
	gameExports.BotChooseNBGItem = botlib_export->ai.BotChooseNBGItem;
	gameExports.BotTouchingGoal = botlib_export->ai.BotTouchingGoal;
	gameExports.BotItemGoalInVisButNotVisible = botlib_export->ai.BotItemGoalInVisButNotVisible;
	gameExports.BotGetLevelItemGoal = botlib_export->ai.BotGetLevelItemGoal;
	gameExports.BotGetNextCampSpotGoal = botlib_export->ai.BotGetNextCampSpotGoal;
	gameExports.BotGetMapLocationGoal = botlib_export->ai.BotGetMapLocationGoal;
	gameExports.BotAvoidGoalTime = botlib_export->ai.BotAvoidGoalTime;
	gameExports.BotSetAvoidGoalTime = botlib_export->ai.BotSetAvoidGoalTime;
	gameExports.BotInitLevelItems = botlib_export->ai.BotInitLevelItems;
	gameExports.BotUpdateEntityItems = botlib_export->ai.BotUpdateEntityItems;
	gameExports.BotLoadItemWeights = botlib_export->ai.BotLoadItemWeights;
	gameExports.BotFreeItemWeights = botlib_export->ai.BotFreeItemWeights;
	gameExports.BotInterbreedGoalFuzzyLogic = botlib_export->ai.BotInterbreedGoalFuzzyLogic;
	gameExports.BotSaveGoalFuzzyLogic = botlib_export->ai.BotSaveGoalFuzzyLogic;
	gameExports.BotMutateGoalFuzzyLogic = botlib_export->ai.BotMutateGoalFuzzyLogic;
	gameExports.BotAllocGoalState = botlib_export->ai.BotAllocGoalState;
	gameExports.BotFreeGoalState = botlib_export->ai.BotFreeGoalState;
	gameExports.BotResetMoveState = botlib_export->ai.BotResetMoveState;
	gameExports.BotAddAvoidSpot = botlib_export->ai.BotAddAvoidSpot;
	gameExports.BotMoveToGoal = botlib_export->ai.BotMoveToGoal;
	gameExports.BotMoveInDirection = botlib_export->ai.BotMoveInDirection;
	gameExports.BotResetAvoidReach = botlib_export->ai.BotResetAvoidReach;
	gameExports.BotResetLastAvoidReach = botlib_export->ai.BotResetLastAvoidReach;
	gameExports.BotReachabilityArea = botlib_export->ai.BotReachabilityArea;
	gameExports.BotMovementViewTarget = botlib_export->ai.BotMovementViewTarget;
	gameExports.BotPredictVisiblePosition = botlib_export->ai.BotPredictVisiblePosition;
	gameExports.BotAllocMoveState = botlib_export->ai.BotAllocMoveState;
	gameExports.BotFreeMoveState = botlib_export->ai.BotFreeMoveState;
	gameExports.BotInitMoveState = botlib_export->ai.BotInitMoveState;
	gameExports.BotChooseBestFightWeapon = botlib_export->ai.BotChooseBestFightWeapon;
	gameExports.BotGetWeaponInfo = botlib_export->ai.BotGetWeaponInfo;
	gameExports.BotLoadWeaponWeights = botlib_export->ai.BotLoadWeaponWeights;
	gameExports.BotAllocWeaponState = botlib_export->ai.BotAllocWeaponState;
	gameExports.BotFreeWeaponState = botlib_export->ai.BotFreeWeaponState;
	gameExports.BotResetWeaponState = botlib_export->ai.BotResetWeaponState;
	gameExports.GeneticParentsAndChildSelection = botlib_export->ai.GeneticParentsAndChildSelection;
	gameExports.PC_LoadSource = botlib_export->PC_LoadSourceHandle;
	gameExports.PC_FreeSource = botlib_export->PC_FreeSourceHandle;
	gameExports.PC_ReadToken = botlib_export->PC_ReadTokenHandle;
	gameExports.PC_SourceFileAndLine = botlib_export->PC_SourceFileAndLine;
	gameExports.LLM_PushPrompt = LLM_PushPrompt;

	var = Cvar_Get( "bot_enable", "1", CVAR_LATCH );
	if ( var ) {
		bot_enable = var->integer;
	}
	else {
		bot_enable = 0;
	}

    // load the dll
    dllHandle = Sys_DLL_Load("qagame");
    if (!dllHandle) {
        Com_Error(ERR_DROP, "VM_Create on game failed");
    }

    game = Sys_DLL_CallEntry(dllHandle, &gameExports);
    if (!game) {
        Com_Error(ERR_DROP, "VM_Create game api was invalid");
    }

	SV_InitGameVM( qfalse );
}


/*
====================
SV_GameCommand

See if the current console command is claimed by the game
====================
*/
qboolean SV_GameCommand( void ) {
	if ( sv.state != SS_GAME ) {
		return qfalse;
	}

    return game->ConsoleCommand();
}

