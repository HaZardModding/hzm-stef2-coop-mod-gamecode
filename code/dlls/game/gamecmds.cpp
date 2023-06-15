//-----------------------------------------------------------------------------
//
//  $Logfile:: /EF2/Code/DLLs/game/gamecmds.cpp                              $
// $Revision:: 54                                                             $
//     $Date:: 9/26/03 2:36p                                                  $
//
// Copyright (C) 1999 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
//

#include "_pch_cpp.h"

#include "coopPlayer.hpp"

#include "gamecmds.h"
#include "camera.h"
#include "viewthing.h"
#include "soundman.h"
#include "navigate.h"
#include "mp_manager.hpp"
#include "CinematicArmature.h"
#include <qcommon/gameplaymanager.h>
#include "botmenudef.h"

//[b60011] chrissstrahl - make avialable to use here
extern Event EV_SetOriginEveryFrame;

typedef struct
{
	const char  *command;
	qboolean    ( *func )( const gentity_t *ent );
	qboolean    allclients;
} consolecmd_t;

consolecmd_t G_ConsoleCmds[] =
{
	//   command name       function             available in multiplayer?

//[b60011] chrissstrahl - coop commands 
//[b60011] chrissstrahl - coop commands 
//[b60011] chrissstrahl - coop commands 
	{ "coopthread",G_coopThread,			true }, //[b60011] chrissstrahl - allowing players to start a script thread
	{ "coopitem",G_coopItem,				true }, //[b60011] chrissstrahl - allowing players to make use of special coop inventory
	{ "coopinstalled",G_coopInstalled,		true },	//[b600] chrissstrahl - allow deetection if mod is installed
	{ "coopinput",G_coopInput	,			true }, //[b60011] chrissstrahl - allowing players to send text to server
	{ "coopcid",G_coopClientId	,			true }, //[b60011] chrissstrahl - allowing players to send idendification string
//[b60011] chrissstrahl - ! text commands moved here from coop_playerSay in coopPlayer.cpp
	{ "!thread",G_coopThread	,			true }, 
	{ "!block",G_coopCom_block	,			true }, 
	{ "!class",G_coopCom_class	,			true }, 
	{ "!drop",G_coopCom_drop	,			true }, 
	{ "!help",G_coopCom_help	,			true }, 
	{ "!info",G_coopCom_info	,			true }, 
	{ "!kickbots",G_coopCom_kickbots,		true }, 
	{ "!kill",G_coopCom_kill	,			true }, 
	{ "!login",G_coopCom_login	,			true }, 
	{ "!logout",G_coopCom_logout	,		true }, 
	{ "!mapname",G_coopCom_mapname	,		true }, 
	{ "!origin",G_coopCom_origin	,		true }, 
	{ "!skill",G_coopCom_skill	,			true }, 
	{ "!stuck",G_coopCom_stuck	,			true }, 
	{ "!transport",G_coopCom_transport,		true },
	{ "!leader",G_coopCom_leader,			true },
	{ "!reboot",G_coopCom_reboot	,		true }, 
	{ "!levelend",G_coopCom_levelend	,	true }, 
	{ "!targeted",G_coopCom_targeted	,	true }, 
	{ "!targetnames",G_coopCom_targeted	,	true }, 
	{ "!showspawn",G_coopCom_showspawn	,	true }, 
	{ "!hidespawn",G_coopCom_hidespawn	,	true }, 
	{ "!testspawn",G_coopCom_testspawn	,	true }, 
	{ "!noclip",G_coopCom_noclip,			true }, 
	//[b60014] chrissstrahl - added
	{ "!follow",G_coopCom_follow,			true },

//Coop Commands End
//Coop Commands End
//Coop Commands End

	//[b60013] chrissstrahl - add features to game without coop dependency
	{ "reload",				G_Reload	,			true },	//[b60013] chrissstrahl - allow reloading of map (complete reload, not just a restart)
	{ "Eng",				G_LanguageEng,			true },	//[b60011] chrissstrahl - select English Player Laguage
	{ "Deu",				G_LanguageDeu,			true },	//[b60011] chrissstrahl - select English German Laguage
	{ "circle",				G_circleMenu,			true }, //[b60011] chrissstrahl - show circle menu to player
	{ "circledialog",		G_circleDialog,			true }, //[b60011] chrissstrahl - show circle menu to player
	{ "widgettext",			G_widgettext,			true }, //[b60011] chrissstrahl - allowing players to make use of widgettext command
	{ "dialogrunthread",	G_DialogRunThread,		true }, //[b608] chrissstrahl - allow this in multiplayer/coop - used to be false
	//hzm gameupdate end

	{ "vtaunt",				G_VTaunt,				true },
//	{ "vsay_team",			G_VTaunt,				true },
//	{ "vosay_team",			G_VTaunt,				true },
//	{ "vtell",				G_VTaunt,				true },
	{ "vsay",				G_SayCmd,				true },
	{ "vosay",				G_SayCmd,				true },
	{ "tell",				G_TellCmd,				true },
	{ "vtell",				G_TellCmd,				true },
	{ "say",				G_SayCmd,				true },
	{ "taunt",				G_TauntCmd,				true },
	{ "vosay_team",			G_TeamSayCmd,			true },
	{ "vsay_team",			G_TeamSayCmd,			true },
	{ "tsay",				G_TeamSayCmd,			true },
	{ "say_team",			G_TeamSayCmd,			true }, // added for BOTLIB
	{ "eventlist",			G_EventListCmd,			false },
	{ "pendingevents",		G_PendingEventsCmd,		false },
	{ "eventhelp",			G_EventHelpCmd,			false },
	{ "dumpevents",			G_DumpEventsCmd,		false },
	{ "classevents",		G_ClassEventsCmd,		false },
	{ "dumpclassevents",	G_DumpClassEventsCmd,	false },
	{ "dumpallclasses",		G_DumpAllClassesCmd,	false },
	{ "classlist",			G_ClassListCmd,			false },
	{ "classtree",			G_ClassTreeCmd,			false },
	{ "cam",				G_CameraCmd,			false },
	{ "snd",				G_SoundCmd,				false },
	{ "cin",				G_CinematicCmd,			false },
//	{ "showvar",			G_ShowVarCmd,			false },
	{ "script",				G_ScriptCmd,			false },
	{ "clientrunthread",	G_ClientRunThreadCmd,	false },
	{ "clientsetvar",		G_ClientSetVarCmd,		false },
	{ "levelvars",			G_LevelVarsCmd,			false },
	{ "gamevars",			G_GameVarsCmd,			false },
	{ "loc",				G_LocCmd,				false },
	{ "warp",				G_WarpCmd,				false },
	{ "mask",				G_MaskCmd,				false },
	{ "setgameplayfloat",	G_SetGameplayFloatCmd,	false },
	{ "setgameplaystring",	G_SetGameplayStringCmd, false },
	{ "purchaseSkill",		G_PurchaseSkillCmd,		false },
	{ "swapItem",			G_SwapItemCmd,			false },
	{ "dropItem",			G_DropItemCmd,			false },
	{ NULL,					NULL,					NULL }
};

void G_InitConsoleCommands( void )
{
	consolecmd_t *cmds;
	
	//
	// the game server will interpret these commands, which will be automatically
	// forwarded to the server after they are not recognized locally
	//
	gi.AddCommand( "give" );
	gi.AddCommand( "god" );
	gi.AddCommand( "notarget" );
	gi.AddCommand( "noclip" );
	gi.AddCommand( "kill" );
	gi.AddCommand( "script" );
	
	for( cmds = G_ConsoleCmds; cmds->command != NULL; cmds++ )
	{
		gi.AddCommand( cmds->command );
	}
}

qboolean	G_ConsoleCommand( void )
{
	qboolean result;
	
	result = false;
	try
	{
		if ( dedicated->integer )
		{
			const char *cmd;
			const char *cmd2;
			const char *cmd3;
			cmd = gi.argv( 0 );
			cmd2 = gi.argv( 1 );
			cmd3 = gi.argv( 2 );

			if ( stricmp( cmd, "say" ) == 0 )
			{
				G_Say( NULL, false, false );
				result = true;
			}
		}

		//[b607] chrissstrahl - moved here because it would print out commands send to server at G_ProcessClientCommand
		//hzm gamefix daggolin - console commands
		if (!result)
		{
			result = G_ServerCommand();
		}

		if ( !result )
		{
			gentity_t *ent;
			ent = &g_entities[ 0 ];
			result = G_ProcessClientCommand( ent );
		}
	}
	
	catch( const char *error )
	{
		G_ExitWithError( error );
	}
	
	return result;
}

void G_ClientCommand( gentity_t *ent )
{
	try
	{
		if ( ent && !G_ProcessClientCommand( ent ) )
		{
			const char* cmd;
			cmd = gi.argv(0);
			//[b609] hzm gameupdate chrissstrahl - remove leading / or \ or ^
			//the slashes for ef1 users convinience and ^ for general convinience
			//then sends this data back to client and client console decides what to do and sends it possibly back to server
			bool bFiltered = false;
			while (*cmd == '^' || *cmd == '/' || *cmd == '\\') {
				cmd++;
				bFiltered = true;
			}

			if (bFiltered && gi.GetNumFreeReliableServerCommands(ent->entity->entnum) > 32)
			{
				int cmdNum = 1;
				str cmdGlueUp ="";
				char *checkCmdPresent;

				cmdGlueUp = va("%s", cmd);

				checkCmdPresent = gi.argv(cmdNum);
				while (checkCmdPresent && strlen(checkCmdPresent) > 0) {
					cmdGlueUp += va(" %s", checkCmdPresent);
					cmdNum++;
					checkCmdPresent = gi.argv(cmdNum);
				}
				//send back to client and client console will decide it it wants to send it again
				gi.SendServerCommand(ent->entity->entnum, va("stufftext \"%s\"\n", cmdGlueUp.c_str()));
				return;
			}
			//end

			// anything that doesn't match a command will be a chat
			G_Say( ent, false, true );
		}
	}
	
	catch( const char *error )
	{
		G_ExitWithError( error );
	}
}

qboolean G_ProcessClientCommand( gentity_t *ent )
{
	const char	   *cmd;
	consolecmd_t   *cmds;
	int			   i;
	int            n;
	Event			   *ev;
	
	cmd = gi.argv( 0 );

	if ( !ent || !ent->client || !ent->entity || !ent->inuse )
	{
		// not fully in game yet
		return false;
	}
	
	for( cmds = G_ConsoleCmds; cmds->command != NULL; cmds++ )
	{
		//hzm gamefix chrissstrahl - if sv_maxclient is not set to 1 it happened that the sp game did not interpret console script commands and other commands correctly
		// if we have multiple clients and this command isn't allowed by multiple clients, skip it
		if ( ( game.maxclients > 1  && g_gametype->integer != 0 ) && ( !cmds->allclients ) && !sv_cheats->integer )
		{
			continue;
		}
		
		if ( !Q_stricmp( cmd, cmds->command ) )
		{
			return cmds->func( ent );
		}
	}
	
	if ( Event::Exists( cmd ) )
	{
		ev = new Event( cmd );
		ev->SetSource( EV_FROM_CONSOLE );
		ev->SetConsoleEdict( ent );
		
		n = gi.argc();
		for( i = 1; i < n; i++ )
		{
			ev->AddToken( gi.argv( i ) );
		}
		
		if ( Q_stricmpn( cmd, "ai_", 2 ) == 0 )
		{
			return thePathManager.ProcessEvent( ev );
		}
		else if ( Q_stricmpn( cmd, "view", 4 ) == 0 )
		{
			return Viewmodel.ProcessEvent( ev );
		}
		else
		{
			if( ent && ent->entity )
				return ent->entity->ProcessEvent( ev );
		}
	}
	
	return false;
}

/*
==================
Cmd_Say_f
==================
*/
void G_Say( const gentity_t *ent, bool team, qboolean arg0 )
{
	str text;
	const char	*p;
	
	if ( gi.argc() < 2 && !arg0 )
	{
		return;
	}
	
	if ( arg0 )
	{
		text = gi.argv( 0 );
		text += " ";
		text += gi.args();
	}
	else
	{
		p = gi.args();
		
		if ( *p == '"' )
		{
			p++;
			text = p;
			text[ text.length() - 1 ] = 0;
		}
		else
		{
			text = p;
		}
	}
	
	if ( ent && ent->entity && ent->entity->isSubclassOf( Player ) )
	{
		multiplayerManager.say( (Player *)ent->entity, text, team );
	}
	else
	{
		multiplayerManager.say( NULL, text, false );
	}
}

//===========================================================[b60011]
// Name:        G_circleMenu
// Class:       -
//              
// Description: show circle menu to player
//              
// Parameters:  
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_circleMenu(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client)
		return qtrue;

	Player* player = (Player*)ent->entity;

	//make sure player that do not have the coop mod and try this command know
	if (!player->coopPlayer.installed) {
		player->hudPrint("You need the Coop Mod for this to work\n");
		return qtrue;
	}

	player->circleMenu(1);
	return qtrue;
}

//===========================================================[b60011]
// Name:        G_circleDialog
// Class:       -
//              
// Description: show circle dialog menu to player
//              
// Parameters:  
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_circleDialog(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client)
		return qtrue;

	Player* player = (Player*)ent->entity;

	//make sure player that do not have the coop mod and try this command know
	if (!player->coopPlayer.installed) {
		player->hudPrint("You need the Coop Mod for this to work\n");
		return qtrue;
	}
	
	player->circleMenu(2);
	return qtrue;
}

//===========================================================[b60011]
// Name:        G_widgettext
// Class:       -
//              
// Description: allow player using this in multiplayer/server console
//              
// Parameters:  strings
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_widgettext(const gentity_t *ent)
{
	if (!ent || !ent->inuse || !ent->client)
		return qtrue;

	const char *widgetName = gi.argv(1);
	if (strlen(widgetName) == 0) {
		return qtrue;
	}
	int iParameterIndex = 2;
	str sParameters = "";
	while(1){
		const char *parameter = gi.argv(iParameterIndex);
		if (strlen(parameter)) {
			if (iParameterIndex > 2){
				sParameters += " ";
			}
			iParameterIndex++;
			sParameters += parameter;
		}
		else {
			if (iParameterIndex == 2) {
				return qtrue;
			}
			break;
		}
	}

	const char *widgetText = sParameters.c_str();
	return G_SetWidgetTextOfPlayer(ent, widgetName, widgetText);
}

qboolean G_CameraCmd( const gentity_t *ent )
{
	Event *ev;
	const char *cmd;
	int   i;
	int   n;
	
	n = gi.argc();
	if ( !n )
	{
		gi.WPrintf( "Usage: cam [command] [arg 1]...[arg n]\n" );
		return qtrue;
	}
	
	cmd = gi.argv( 1 );
	if ( Event::Exists( cmd ) )
	{
		ev = new Event( cmd );
		ev->SetSource( EV_FROM_CONSOLE );
		ev->SetConsoleEdict( NULL );
		
		for( i = 2; i < n; i++ )
		{
			ev->AddToken( gi.argv( i ) );
		}
		
		CameraMan.ProcessEvent( ev );
	}
	else
	{
		gi.WPrintf( "Unknown camera command '%s'.\n", cmd );
	}
	
	return true;
}

//===============================================================
// Name:		G_CinematicCmd
// Class:		
//
// Description: Takes a cinematic command from the console, rips
//				off the first argument and looks up the second
//				as a cinematic armature event.  If found, sends
//				it off to the armature along with the rest of the
//				event tokens.
// 
// Parameters:	gentity_t* -- entity sending the event
//
// Returns:		qboolean -- true if processed.
// 
//===============================================================
qboolean G_CinematicCmd( const gentity_t *ent )
{
	Event		*ev ;
	const char	*cmd ;
	int			 n		= gi.argc();
	
	if ( !n )
	{
		gi.WPrintf( "Usage: cin <command> <arg1> ... <argn>\n");
		return qtrue ;
	}
	
	cmd = gi.argv( 1 );
	if ( Event::Exists( cmd ) )
	{
		ev = new Event( cmd );
		ev->SetSource( EV_FROM_CONSOLE );
		ev->SetConsoleEdict( NULL );
		
		for (int i = 2; i < n; i++ )
		{
			ev->AddToken( gi.argv( i ) );
		}
		
		theCinematicArmature.ProcessEvent( ev );
	}
	else
	{
		gi.WPrintf( "Unknown cinematic armature command '%s'.\n", cmd );
	}
	
	return qtrue ;
}

qboolean G_SoundCmd( const gentity_t *ent )
{
	Event *ev;
	const char *cmd;
	int   i;
	int   n;
	
	n = gi.argc();
	if ( !n )
	{
		gi.WPrintf( "Usage: snd [command] [arg 1]...[arg n]\n" );
		return true;
	}
	
	cmd = gi.argv( 1 );
	if ( Event::Exists( cmd ) )
	{
		ev = new Event( cmd );
		ev->SetSource( EV_FROM_CONSOLE );
		ev->SetConsoleEdict( NULL );
		
		for( i = 2; i < n; i++ )
		{
			ev->AddToken( gi.argv( i ) );
		}
		
		SoundMan.ProcessEvent( ev );
	}
	else
	{
		gi.WPrintf( "Unknown sound command '%s'.\n", cmd );
	}
	
	return true;
}

char *ClientName(int client, char *name, int size);
qboolean G_VTaunt( const gentity_t *ent )
{
// NOTE: vtant, vsay, vosay are q3's specific hotkey chat mechanisms.  we don't have these so this is a stub
// function right now.
/*
	gentity_t *who;
	int i;

	if (!ent->client) {
		return qfalse;
	}


	Player *player = (Player *)ent->entity;
	Player *enemy = multiplayerManager.getLastKilledByPlayer(player);

	// insult someone who just killed you
	if (ent->enemy && ent->enemy->client && ent->enemy->client->lastkilled_client == ent->s.number) {
		// i am a dead corpse
		if (!(ent->enemy->r.svFlags & SVF_BOT)) {
			G_Voice( ent, ent->enemy, SAY_TELL, VOICECHAT_DEATHINSULT, qfalse );
		}
		if (!(ent->r.svFlags & SVF_BOT)) {
			G_Voice( ent, ent,        SAY_TELL, VOICECHAT_DEATHINSULT, qfalse );
		}
		ent->enemy = NULL;
		return;
	}
	// insult someone you just killed
	if (ent->client->lastkilled_client >= 0 && ent->client->lastkilled_client != ent->s.number) {
		who = g_entities + ent->client->lastkilled_client;
		if (who->client) {
			// who is the person I just killed
			if (who->client->lasthurt_mod == MOD_GAUNTLET) {
				if (!(who->r.svFlags & SVF_BOT)) {
					G_Voice( ent, who, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse );	// and I killed them with a gauntlet
				}
				if (!(ent->r.svFlags & SVF_BOT)) {
					G_Voice( ent, ent, SAY_TELL, VOICECHAT_KILLGAUNTLET, qfalse );
				}
			} else {
				if (!(who->r.svFlags & SVF_BOT)) {
					G_Voice( ent, who, SAY_TELL, VOICECHAT_KILLINSULT, qfalse );	// and I killed them with something else
				}
				if (!(ent->r.svFlags & SVF_BOT)) {
					G_Voice( ent, ent, SAY_TELL, VOICECHAT_KILLINSULT, qfalse );
				}
			}
			ent->client->lastkilled_client = -1;
			return;
		}
	}

	if (gametype >= GT_TEAM) {
		// praise a team mate who just got a reward
		for(i = 0; i < MAX_CLIENTS; i++) {
			who = g_entities + i;
			if (who->client && who != ent && who->client->sess.sessionTeam == ent->client->sess.sessionTeam) {
				if (who->client->rewardTime > level.time) {
					if (!(who->r.svFlags & SVF_BOT)) {
						G_Voice( ent, who, SAY_TELL, VOICECHAT_PRAISE, qfalse );
					}
					if (!(ent->r.svFlags & SVF_BOT)) {
						G_Voice( ent, ent, SAY_TELL, VOICECHAT_PRAISE, qfalse );
					}
					return;
				}
			}
		}
	}
*/
	// just say something
/*
	cmd = "vchat";
	}

	trap_SendServerCommand( other-g_entities, va("%s %d %d %d %s", cmd, voiceonly, ent->s.number, color, id));
*/

//	trap_SendServerCommand( other-g_entities, va("%s %d %d %d %s", cmd, voiceonly, ent->s.number, color, id));
/*
	GameplayManager
	gi.SendServerCommand(NULL,va("%s %d %d %d %s", "taunt", voiceonly, ent->s.number, color, id));
	//	G_Say( ent,false, va("%s","taunt") ); // VOICECHAT_TAUNT)); //  NULL, SAY_ALL, VOICECHAT_TAUNT, qfalse );
	char bogoname[1024];

	gi.SendServerCommand(ent->s.clientNum,va("hudsay \"%s: %s \"",ClientName(ent->s.clientNum,bogoname,1024),"taunt")); // ClientName(ent->s.clientNum,bogoname,1024),
*/

	return true;
}

qboolean G_TellCmd( const gentity_t *ent )
{
	str text;
	const char	*p;
	int i;
	int entnum;
	
	if ( gi.argc() < 3 )
	{
		return true;
	}

	entnum = atoi( gi.argv( 1 ) );
	
	for ( i = 2 ; i < gi.argc() ; i++ )
	{
		p = gi.argv( i );
	
		if ( *p == '"' )
		{
			p++;
			text += p;
			text[ text.length() - 1 ] = 0;
		}
		else
		{
			text += p;
		}

	}
	
	if ( ent->entity && ent->entity->isSubclassOf( Player ) )
	{
		multiplayerManager.tell( (Player *)ent->entity, text, entnum );
	}

	return true;
}

qboolean G_SayCmd( const gentity_t *ent )
{
	G_Say( ent, false, false );

	return true;
}

qboolean G_TeamSayCmd( const gentity_t *ent )
{
	G_Say( ent, true, false );

	return true;
}

qboolean G_TauntCmd( const gentity_t *ent )
{
	str tauntName;
	
	if ( gi.argc() < 2 )
	{
		return true;
	}
	
	tauntName = "taunt";
	tauntName += gi.argv( 1 );
	
	if ( ent->entity && ent->entity->isSubclassOf( Player ) )
	{
		Player *player = (Player *)ent->entity;

		//[b60011] chrissstrahl - set a cooldown time for taunt - also don't play during cinematic
		if (level.cinematic || player->coopPlayer.tauntCooldownTime > level.time) {
			return true;
		}

		if ( multiplayerManager.inMultiplayer() && !multiplayerManager.isPlayerSpectator( player ) )
		{
			//[b60011] chrissstrahl - update a cooldown time for taunt
			player->coopPlayer.tauntCooldownTime = (level.time + 5.0f);
			//player->hudPrint(va("G_TauntCmd: %f -> %f\n", player->coopPlayer.tauntCooldownTime,level.time));

			//ent->entity->Sound( tauntName, CHAN_TAUNT, DEFAULT_VOL, LEVEL_WIDE_MIN_DIST );
			ent->entity->Sound( tauntName, CHAN_TAUNT, DEFAULT_VOL, 250.0f );
		}
	}

	return true;
}

qboolean G_LocCmd( const gentity_t *ent )
{
	if ( ent )
	{
		gi.Printf( "Origin = ( %f, %f, %f )   Angles = ( %f, %f, %f )\n", 
			ent->currentOrigin[0], ent->currentOrigin[1], ent->currentOrigin[2],
			ent->currentAngles[0], ent->currentAngles[1], ent->currentAngles[0] );
	}
	
	return true;
}

qboolean G_WarpCmd( const gentity_t *ent )
{
	Vector pos;

	if ( sv_cheats->integer == 0 )
	{
		return true;
	}
	
	if ( ent )
	{
		Entity *entity;
		
		if ( ent->entity )
		{
			// Get the new position
			
			if ( gi.argc() == 2 )
			{
				pos = Vector(gi.argv( 1 ));
			}
			else if ( gi.argc() == 4 )
			{
				pos[ 0 ] = atof( gi.argv( 1 ) );
				pos[ 1 ] = atof( gi.argv( 2 ) );
				pos[ 2 ] = atof( gi.argv( 3 ) );
			}
			else
			{
				gi.Printf( "Incorrect parms\n" );
				return false;
			}
			
			// Move the entity to the new position
			
			entity = ent->entity;
			entity->setOrigin( pos );
		}
	}
	
	return true;
}

qboolean G_MaskCmd( const gentity_t *ent )
{
	Vector pos;
	
	// Check parms
	
	if ( gi.argc() != 2 )
	{
		gi.Printf( "Incorrect parms\n" );
		return true;
	}
	
	if ( ent )
	{
		Entity *entity;
		
		if ( ent->entity )
		{
			entity = ent->entity;
			
			// Get and set new mask
			
			if ( stricmp( gi.argv( 1 ), "monster" ) == 0 )
			{
				entity->edict->clipmask	 = MASK_MONSTERSOLID;
			}
			else if ( stricmp( gi.argv( 1 ), "player" ) == 0 )
			{
				entity->edict->clipmask	 = MASK_PLAYERSOLID;
			}
			else
			{
				gi.Printf( "Unknown mask name - %s\n", gi.argv( 1 ) );
			}
		}
	}
	
	return true;
}

qboolean G_EventListCmd( const gentity_t *ent )
{
	const char *mask;
	
	mask = NULL;
	if ( gi.argc() > 1 )
	{
		mask = gi.argv( 1 );
	}
	
	Event::ListCommands( mask );
	
	return true;
}

qboolean G_PendingEventsCmd( const gentity_t *ent )
{
	const char *mask;
	
	mask = NULL;
	if ( gi.argc() > 1 )
	{
		mask = gi.argv( 1 );
	}
	
	Event::PendingEvents( mask );
	
	return true;
}

qboolean G_EventHelpCmd( const gentity_t *ent )
{
	const char *mask;
	
	mask = NULL;
	if ( gi.argc() > 1 )
	{
		mask = gi.argv( 1 );
	}
	
	Event::ListDocumentation( mask, false );
	
	return true;
}

qboolean G_DumpEventsCmd( const gentity_t *ent )
{
	const char *mask;
	
	mask = NULL;
	if ( gi.argc() > 1 )
	{
		mask = gi.argv( 1 );
	}
	
	Event::ListDocumentation( mask, true );
	
	return true;
}

qboolean G_ClassEventsCmd( const gentity_t *ent )
{
	const char *className;
	
	className = NULL;
	if ( gi.argc() < 2 )
	{
		gi.WPrintf( "Usage: classevents [className]\n" );
		className = gi.argv( 1 );
	}
	else
	{
		className = gi.argv( 1 );
		ClassEvents( className );
	}
	return true;
}

qboolean G_DumpClassEventsCmd( const gentity_t *ent )
{
	const char *className;
	
	className = NULL;
	if ( gi.argc() < 2 )
	{
		gi.WPrintf( "Usage: dumpclassevents [className]\n" );
		className = gi.argv( 1 );
	}
	else
	{
		className = gi.argv( 1 );
		ClassEvents( className, true );
	}
	return true;
}

qboolean G_DumpAllClassesCmd( const gentity_t *ent )
{
	const char *tmpstr = NULL;
	const char *filename = NULL;
	int typeFlag = EVENT_ALL;
	int outputFlag = OUTPUT_ALL;
	
	if ( gi.argc() > 1 )
	{   
		tmpstr = gi.argv( 1 );
		if ( !strcmp("tiki", tmpstr ) )
			typeFlag = EVENT_TIKI_ONLY;
		if ( !strcmp("script", tmpstr ) )
			typeFlag = EVENT_SCRIPT_ONLY;
	}
	if ( gi.argc() > 2 )
	{
		tmpstr = gi.argv( 2 );
		if ( !strcmp("html", tmpstr ) )
			outputFlag = OUTPUT_HTML;
		if ( !strcmp("cmdmap", tmpstr ) )
			outputFlag = OUTPUT_CMD;
	}
	if ( gi.argc() > 3 )
	{
		filename = gi.argv( 3 );
	}
	
	DumpAllClasses(typeFlag, outputFlag, filename);
	
	return true;
}

qboolean G_ClassListCmd( const gentity_t *ent )
{
	listAllClasses();
	
	return true;
}

qboolean G_ClassTreeCmd( const gentity_t *ent )
{
	if ( gi.argc() > 1 )
	{
		listInheritanceOrder( gi.argv( 1 ) );
	}
	else
	{
		gi.SendServerCommand( ent - g_entities, "print \"Syntax: classtree [classname].\n\"" );
	}
	
	return true;
}

/*
qboolean G_ShowVarCmd( gentity_t *ent )
{
	ScriptVariable *var;

	var = Director.GetExistingVariable( gi.argv( 1 ) );
	if ( var )
	{
		gi.SendServerCommand( ent - g_entities, "print \"%s = '%s'\n\"", gi.argv( 1 ), var->stringValue() );
	}
	else
	{
		gi.SendServerCommand( ent - g_entities, "print \"Variable '%s' does not exist.\"", gi.argv( 1 ) );
	}
  
	return true;
} */

qboolean G_ScriptCmd( const gentity_t *ent )
{
	int i, argc;
	const char *argv[ 32 ];
	char args[ 32 ][ 64 ];
	
	argc = 0;
	for( i = 1; i < gi.argc(); i++ )
	{
		if ( argc < 32 )
		{
			strncpy( args[ argc ], gi.argv( i ), 64 );
			argv[ argc ] = args[ argc ];
			argc++;
		}
	}
	if ( argc > 0 )
	{
		level.consoleThread->ProcessCommand( argc, argv );
	}
	
	return true;
}

qboolean G_ClientRunThreadCmd( const gentity_t *ent )
{
	str		threadName;
	CThread	*thread;
	
	// Get the thread name
	
	if ( !gi.argc() )
		return true;
	
	threadName = gi.argv( 1 );
	
	
	// Check to make sure player is allowed to run this thread
	
	// Need to do this part
	
	// Run the thread
	
	if ( !threadName.length() )
		return true;
	
	thread = Director.CreateThread( threadName );
	
	if ( thread )
		thread->DelayedStart( 0.0f );
	
	return true;
}

qboolean G_ClientSetVarCmd( const gentity_t *ent )
{
	str		varName;
	str		value;
	
	
	if ( gi.argc() != 3 )
		return true;
	
	// Get the variable name
	
	varName = gi.argv( 1 );
	
	// Get the variable value
	
	value = gi.argv( 2 );
	
	// Check to make sure player is allowed to set this variable
	
	// Need to do this part
	
	// Set the variable
	
	if ( varName.length() && value.length() )
	{
		levelVars.SetVariable( varName, value );
	}
	
	return true;
}

//===============================================================
// Name:		G_SendCommandToAllPlayers
// Class:		None
//
// Description: Sends the specified command to all connected
//				clients (players).
// 
// Parameters:	const char* -- the command to send
//
// Returns:		qboolean -- qtrue if successfully sent.
// 
//===============================================================
qboolean G_SendCommandToAllPlayers( const char *command )
{
	bool retVal = true ;

	for( unsigned int clientIdx = 0; clientIdx < maxclients->integer; ++clientIdx )
	{
		gentity_t *ent = g_entities + clientIdx ;
		if ( !ent->inuse || !ent->client || !ent->entity ) continue;

		if ( !G_SendCommandToPlayer( ent, command ) ) retVal = false ;
	}

	return retVal ;
}


//===============================================================
// Name:		G_SendCommandToPlayer
// Class:		
//
// Description: Sends the specified command to the specified player.
// 
// Parameters:	Player* -- the player to send it to
//				str		-- the command
//
// Returns:		None
// 
//===============================================================
qboolean G_SendCommandToPlayer( const gentity_t *ent, const char *command )
{
	assert( ent );
	Entity *entity = ent->entity ;
	
	assert( entity );
	assert( entity->isSubclassOf( Player ) );
	Player *player = ( Player* )entity ;
	
	str		builtCommand("stufftext \"");
	builtCommand += command ;
	builtCommand += "\"\n";
	gi.SendServerCommand( player->edict - g_entities, builtCommand.c_str() );
	
	return true ;
}


//===============================================================
// Name:		G_EnableWidgetOfPlayer
// Class:		
//
// Description: Enables (or disables) the specified widget of 
//				the specified player.
// 
// Parameters:	const gentity_t*	-- the player gentity_t to send this command to.
//				const char*			-- the name of the widget to enable/disable
//				bool				-- true means enable
//
// Returns:		None
// 
//===============================================================
qboolean G_EnableWidgetOfPlayer( const gentity_t *ent, const char *widgetName, qboolean enableFlag )
{
	assert( widgetName );

	str command("globalwidgetcommand ");
	command += widgetName ;
	command += ( enableFlag ) ? " enable" : " disable" ;

	return G_SendCommandToPlayer( ent, command.c_str());
}


//===============================================================
// Name:		G_SetWidgetTextOfPlayer
// Class:		
//
// Description: Sets the widget text of the widget for the speicifed player
// 
// Parameters:	const gentity_t*	-- the player gentity_t to send this command to.
//				const char*			-- the name of the widget to set the text on
//				const char*			-- text the put on the widget
//
// Returns:		None
// 
//===============================================================
qboolean G_SetWidgetTextOfPlayer( const gentity_t *ent, const char *widgetName, const char *widgetText )
{
	assert( widgetName );
	char tmpstr[4096];
	if ( strlen(widgetText) > 4095 )
		assert(0);
	
	strcpy(tmpstr, widgetText);
	
	int i;
	for ( i=0; i<strlen(widgetText); i++ )
	{
		if ( tmpstr[i] == '\n' )
			tmpstr[i] = '~';
		if ( tmpstr[i] == ' ' )
			tmpstr[i] = '^';
	}
	
	str command("globalwidgetcommand ");
	command += widgetName ;
	command += " labeltext " ;
	command += tmpstr ;
	
	return G_SendCommandToPlayer( ent, command.c_str());
}

void PrintVariableList( ScriptVariableList * list )
{
	ScriptVariable *var;
	int i;
	
	for( i = 1; i <= list->NumVariables(); i++ )
	{
		var = list->GetVariable( i );
		gi.Printf( "%s = %s\n", var->getName(), var->stringValue() );
	}
	gi.Printf( "%d variables\n", list->NumVariables() );
}

qboolean G_LevelVarsCmd( const gentity_t *ent )
{
	gi.Printf( "Level Variables\n" );
	PrintVariableList( &levelVars );
	
	return true;
}

qboolean G_GameVarsCmd( const gentity_t *ent )
{
	gi.Printf( "Game Variables\n" );
	PrintVariableList( &gameVars );
	
	return true;
}

//--------------------------------------------------------------
//
// Name:			G_SetGameplayFloatCmd
// Class:			None
//
// Description:		Sets properties on the gameplay database.
//
// Parameters:		const gentity_t *ent -- Entity, not used
//
// Returns:			qboolean
//
//--------------------------------------------------------------
qboolean G_SetGameplayFloatCmd( const gentity_t *ent )
{
	str objname;
	str propname;
	str create = "0";
	float value = 1.0f;

	// Check for not enough args
	if ( gi.argc() < 4 )
		return qfalse;

	objname = gi.argv( 1 );
	propname = gi.argv( 2 );
	value = (double)atof(gi.argv( 3 ));
	if ( gi.argc() > 4 )
		create = gi.argv( 4 );

	if ( create == "0" )
		GameplayManager::getTheGameplayManager()->setFloatValue(objname, propname, value);
	else
		GameplayManager::getTheGameplayManager()->setFloatValue(objname, propname, value, true);

	gi.Printf("Gameplay Modified -- %s's %s changed to %g\n", objname.c_str(), propname.c_str(), value);

	return qtrue;
}


//--------------------------------------------------------------
//
// Name:			G_SetGameplayStringCmd
// Class:			None
//
// Description:		Sets properties on the gameplay database.
//
// Parameters:		const gentity_t *ent -- Entity, not used
//
// Returns:			qboolean
//
//--------------------------------------------------------------
qboolean G_SetGameplayStringCmd( const gentity_t *ent )
{
	str objname;
	str propname;
	str valuestr;
	str create = "0";

	// Check for not enough args
	if ( gi.argc() < 4 )
		return qfalse;

	objname = gi.argv( 1 );
	propname = gi.argv( 2 );
	valuestr = gi.argv( 3 );
	if ( gi.argc() > 4 )
		create = gi.argv( 4 );

	if ( create == "0" )
		GameplayManager::getTheGameplayManager()->setStringValue(objname, propname, valuestr);
	else
		GameplayManager::getTheGameplayManager()->setStringValue(objname, propname, valuestr, true);

	gi.Printf("Gameplay Modified -- %s's %s changed to %s\n", objname.c_str(), propname.c_str(), valuestr.c_str());

	return qtrue;
}


//===============================================================
// Name:		G_PurchaseSkillCmd
// Class:		None
//
// Description: Purchases a skill for the current player.  This
//				goes into the gameplay database and looks for a
//				CurrentPlayer object who has a skillPoints property.
//
//				If its found, we attempt to increment the value
//				of the skill specified in the command's first argument.
//				We also decrement by one the number of available skillPoints.
// 
// Parameters:	gentity_t -- the entity issuing the command. Not used.
//
// Returns:		qboolean -- true if the command was executed.
// 
//===============================================================
qboolean G_PurchaseSkillCmd( const gentity_t *ent )
{
	str propname ;

	if ( gi.argc() < 1 ) return qfalse ;

	propname = gi.argv( 1 );

	GameplayManager *gpm = GameplayManager::getTheGameplayManager();
	if ( !gpm ) return false ;

	float skillPoints = gpm->getFloatValue( "CurrentPlayer", "SkillPoints" );
	if ( skillPoints > 0.0f )
	{
		str objName("CurrentPlayer.");
		objName += propname ;
		float skillValue = gpm->getFloatValue( objName, "value" );
		if ( skillValue < gpm->getFloatValue( objName, "max" ) )
		{
			gpm->setFloatValue( objName, "value", skillValue + 1.0f );
			gpm->setFloatValue( "CurrentPlayer", "SkillPoints", skillPoints - 1.0f );

			if ( ent->entity->isSubclassOf( Player ) )
			{
				Player *player = (Player*)ent->entity;
				player->skillChanged( objName );
			}
		}
	}

	return qtrue ;
}


//===============================================================
// Name:		G_SwapItemCmd
// Class:		None
//
// Description: Swaps an inventory item with the currently held item
//				of that type.  This is fairly specific at this point
//				to use the Gameplay Database in a particular way.
//
//				The command takes a single string, which defines an
//				inventory slot object.  We retrieve the name of the
//				item in that inventory slot.  If it isn't found, there
//				isn't an item there and nothing happens.
//
//				If it is found, we determine if the player already has
//				an item of that type in their local inventory (hands).
//				This is done by again checking the database.  If they
//				do have a weapon of that type, we swap this one with
//				that.  Otherwise we just give him this weapon.
// 
// Parameters:	gentity_t -- the entity issuing the command.  This
//							 is the player who issued the command.
//
// Returns:		qboolean -- true if the command was executed.
// 
//===============================================================
qboolean G_SwapItemCmd( const gentity_t *ent )
{
	if ( gi.argc() < 1 ) return qfalse ;
	if ( !ent->entity->isSubclassOf( Player ) ) return qfalse ;
	
	Player		*player			= (Player*)ent->entity ;
	str			 objectName		= gi.argv( 1 );
	str			 propertyName	= gi.argv( 2 );
	str			 heldItem ;
	weaponhand_t hand			= WEAPON_RIGHT;
	
	GameplayManager *gpm = GameplayManager::getTheGameplayManager();
	if ( !gpm ) return false ;
	
	str itemName	= gpm->getStringValue( objectName, propertyName );
	str tikiName	= gpm->getStringValue( itemName, "model" );
	str itemType	= gpm->getStringValue( itemName, "class" );
	str	playerItem("CurrentPlayer." + itemType);
	
	if ( !gpm->hasObject(playerItem) )
	{
		gi.WPrintf( "Warning: Unknown item type %s for item %s", itemType.c_str(), itemName.c_str() );
		return qfalse ;
	}
	
	// Get the currently held item, and replace it with the item we clicked on
	heldItem = gpm->getStringValue( playerItem, "name" );
	gpm->setStringValue( playerItem, "name", itemName );
	
	// Ranged weapons are left hand
	if ( itemType == "Ranged" )
		hand = WEAPON_LEFT ;
	
	// Remove the currently held item from the player's inventory
	player->takeItem( heldItem.c_str() );
	
	// Give the player the new item we clicked on
	player->giveItem( tikiName );
	
	// Use the weapon we clicked on if we picked a weapon of the same type
	// that we have currently equipped.  Otherwise, we just swap out
	// the slots
	Weapon *weap = 0;
	weap = player->GetActiveWeapon(WEAPON_RIGHT);
	if ( !weap )
		weap = player->GetActiveWeapon(WEAPON_LEFT);
	if ( weap )
	{
		str tmpstr = gpm->getStringValue(weap->getArchetype(), "class");
		if ( tmpstr == itemType )
			player->useWeapon( itemType, hand );
	}
	
	// Put the held item in inventory slot the clicked item was in.
	gpm->setStringValue( objectName, propertyName, heldItem );
	
	return qtrue ;
}

//--------------------------------------------------------------
//
// Name:			G_DropItemCmd
// Class:			None
//
// Description:		Drops an item that is clicked on from the inventory
//
// Parameters:		gentity_t -- the entity issuing the command.  This
//							 is the player who issued the command.
//
// Returns:			qboolean -- true if the command was executed.	
//
//--------------------------------------------------------------
qboolean G_DropItemCmd( const gentity_t *ent )
{
	if ( gi.argc() < 1 ) return false ;
	if ( !ent->entity->isSubclassOf( Player ) ) return false ;
	
	Player		*player			= (Player*)ent->entity ;
	str			 objectName		= gi.argv( 1 );
	str			 propertyName	= gi.argv( 2 );
	
	GameplayManager *gpm = GameplayManager::getTheGameplayManager();
	if ( !gpm ) return false ;
	
	str itemName	= gpm->getStringValue( objectName, propertyName );
	if ( itemName == "Empty" )
		return false;
	
	str tikiName	= gpm->getStringValue( itemName, "model" );
	str itemType	= gpm->getStringValue( itemName, "class" );
	str	playerItem("CurrentPlayer." + itemType);
	
	if ( !gpm->hasObject(playerItem) )
	{
		gi.WPrintf( "Warning: Unknown item type %s for item %s", itemType.c_str(), itemName.c_str() );
		return false ;
	}
	
	// Empty the slot
	gpm->setStringValue( objectName, propertyName, "Empty" );
	
	// Give the player the item to drop
	//Item *givenItem = player->giveItem(tikiName);
	
	Weapon *dropweap;
	ClassDef *cls;
	cls = getClass( tikiName.c_str() );
	if ( !cls )
	{
		SpawnArgs args;
		args.setArg( "model", tikiName.c_str() );
		cls = args.getClassDef();
		if ( !cls )
			return false;
	}
	dropweap = ( Weapon * )cls->newInstance();
	dropweap->setModel( tikiName.c_str() );
	dropweap->ProcessPendingEvents();
	dropweap->SetOwner(player);
	dropweap->hideModel();
	dropweap->setAttached(true);
	dropweap->Drop();
	
	return true ;
}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
qboolean G_DialogRunThread( const gentity_t *ent )
{
	//[b608] chrissstrahl - make sure dialog stuff works right - prevent double execution
	if (g_gametype->integer != GT_SINGLE_PLAYER && !game.branchdialog_selectionActive ) {
		//return true so it isn't printed as chat
		return qtrue;
	}

	//[b608] chrissstrahl - prevent other players from abousing this opertunity
	if (ent->entity->isSubclassOf(Player)) {
		Player* player = (Player*)ent->entity;
		if ((Entity *)player != game.branchdialog_chosenPlayer) {
			//return true so it isn't printed as chat
			return qtrue;
		}
	}

	// clear out the current dialog actor
	if(ent->entity->isSubclassOf(Player))
	{
		Player* player = (Player*)ent->entity;
		player->clearBranchDialogActor();
	}

	game.branchdialog_selectionActive = false;
	game.branchdialog_chosenPlayer = NULL; //[b608] chrissstrahl - used to store player that is valid to select the dialog

	return G_ClientRunThreadCmd( ent );
}

//[b60011] chrissstrahl - add language detection
//--------------------------------------------------------------
//
// Name:			G_LanguageEng
// Class:			None
//
// Description:		Sets player language to ENGLISH
//
// Parameters:		gentity_t -- the entity issuing the command.  This
//							 is the player who issued the command.
//
// Returns:			qboolean -- true if the command was executed.	
//
//--------------------------------------------------------------
qboolean G_LanguageEng(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	player->setLanguage("Eng");
	if ((player->client->pers.enterTime + 5) < level.time) {
		player->hudPrint("Your Language was set to English\n");
	}
	return true;
}


//[b60011] chrissstrahl - add language detection
//--------------------------------------------------------------
//
// Name:			G_LanguageDeu
// Class:			None
//
// Description:		Sets player language to GERMAN
//
// Parameters:		gentity_t -- the entity issuing the command.  This
//							 is the player who issued the command.
//
// Returns:			qboolean -- true if the command was executed.	
//
//--------------------------------------------------------------
qboolean G_LanguageDeu(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	player->setLanguage("Deu");
	if ((player->client->pers.enterTime + 5) < level.time) {
		player->hudPrint("Ihre Sprache wurde auf deutsch gesetzt\n");
	}
	return true;
}

//[b60013] chrissstrahl - add command to reload map - so it can be bind and works on every map without typing mapname
//--------------------------------------------------------------
//
// Name:			G_Reload
// Class:			None
//
// Description:		Detects the player class
//
// Parameters:		gentity_t -- the entity issuing the command.  This
//							 is the player who issued the command.
//
// Returns:			qboolean -- true if the command was executed.	
//
//--------------------------------------------------------------
extern void G_ExitLevel(void);
qboolean G_Reload(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
#ifdef WIN32
	bool bWindows = true;
#else
	bool bWindows = false;
#endif

	if (dedicated->integer == 0 && player->entnum == 0 && bWindows) {
		G_ExitLevel();
	}
	else {
		str sVote = va("callvote map %s", level.mapname.c_str());
		gi.SendServerCommand(ent->entity->entnum, va("stufftext \"%s\"\n", sVote.c_str()));
	}
	return true;
}