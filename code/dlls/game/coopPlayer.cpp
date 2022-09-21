//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "coopPlayer.hpp"

//[b60011] chrissstrahl - gameupgrades
#include "upgCircleMenu.hpp"

//[b60011] chrissstrahl
#include "coopNpcTeam.hpp"
extern CoopNpcTeam coopNpcTeam;
extern CoopChallenges coopChallenges;
extern CoopServer coopServer;

#include "coopAlias.hpp"
#include "coopParser.hpp"
#include "coopModel.hpp"
#include "coopRadar.hpp"
#include "coopCheck.hpp"
#include "coopText.hpp"
#include "coopStory.hpp"
#include "coopServer.hpp"
#include "coopClass.hpp"
#include "coopArmory.hpp"
#include "coopObjectives.hpp"
#include "coopHuds.hpp"
#include "coopReturn.hpp"

#include "level.h"
#include "player.h"
#include "weapon.h"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include <qcommon/gameplaymanager.h>
//#include "powerups.h"

//[b60011] chrissstrahl - make avialable to use here
extern Event EV_SetOriginEveryFrame;
extern Event EV_World_AutoFailure;

pendingServerCommand *pendingServerCommandList[MAX_CLIENTS];

extern int iTIKIS;
extern int iSKAS;
extern int iSPRITES;

//[b607] chrissstrahl - updated and improved and also fixed player names with space not showing
//================================================================
// Name:        coop_playerCommunicator
// Class:       -
//              
// Description: [b607] adds player names to !transport section of the coop communicator
//              
// Parameters:  Player*, int iAdd <0,1,2> 0=getlist,1=add, 2=remove
//              
// Returns:     void
//              
//================================================================
void coop_playerCommunicator(Player* player, int iAdd)
{
	if (!game.coop_isActive) {
		return;
	}

	str sName;
	Player *currentPlayer = NULL;
	int iMaxCients = maxclients->integer;
	if (iMaxCients > COOP_MAX_PLAYERS) {
		iMaxCients = COOP_MAX_PLAYERS;
	}

	//grab player name or set empty for the slot in the menu
	if (iAdd<=0){
		sName = "$$Empty$$";
	}
	else {
		sName = player->client->pers.netname;
	}

	//don't send update info to leaving player 
	for (int i = 0; i < iMaxCients; i++) {
		currentPlayer = (Player *)g_entities[i].entity;

		//don't send update info to player without mod menu or if it is bot
		if (!currentPlayer || !currentPlayer->coopPlayer.installed || currentPlayer->edict->svflags & SVF_BOT) { continue; }

		//don't send update info to leaving player
		if (iAdd <= 0 && i == player->entnum) {
gi.Printf(va("COOPDEBUG [%s] Leaving Player skipped: %s\n", currentPlayer->client->pers.netname, player->client->pers.netname));
			continue;
		}

		//send list to current player communicator menu, menu has 8 slots
		for (int j = 0; j < 8; j++) {
			str sListName;
			Player *listPlayer = (Player *)g_entities[j].entity;

			if (!listPlayer || !listPlayer->isClient() || iAdd <= 0 ) {
				sListName = "$$Empty$$";
			}
			else {
				sListName = listPlayer->client->pers.netname;
				sListName = coop_textReplaceWhithespace(sListName);
			}

			DelayedServerCommand(currentPlayer->entnum, va("globalwidgetcommand coop_comTrans%i title %s",j, sListName.c_str()));
gi.Printf(va("COOPDEBUG [%s] coop_comTrans%i title %s\n", currentPlayer->client->pers.netname, j, sListName.c_str()));
		}
	}
}


//================================================================
// Name:        coop_playerCheckAdmin
// Class:       -
//              
// Description: [b607] added login authorisation stuff for coop admin login menu
//              
// Parameters:  Player*
//              
// Returns:     bool
//              
//================================================================
bool coop_playerCheckAdmin(Player *player)
{
	//already logged in
	if (player->coopPlayer.admin) {
		return true;
	}

#ifdef WIN32
	bool bWindows = true;
#else
	bool bWindows = false;
#endif

	//[b610] chrissstrahl - auto login if player is host
	if (dedicated->integer == 0 && player->entnum == 0 && bWindows) {
		player->coopPlayer.admin = true;
		player->hudPrint("^3You are now logged in (Host auto-!login).\n");
		return true;
	}

	str sPlayerAuth = "";
	str sServerAuth = "";

	//gets the cvar name set in the multioptions_login.scr file
	cvar_t *cvar = gi.cvar_get(coop_getStringScriptVariable("string_cvarNameForCoopAdminLoginCode"));
	if (cvar == NULL) {
		return false;
	}
	sServerAuth += cvar->string;

	ScriptVariable *entityData;
	entityData = NULL;
	entityData = player->entityVars.GetVariable("coop_login_authorisation");
	if (entityData == NULL) {
		return false;
	}
	sPlayerAuth = entityData->stringValue();

	if (sPlayerAuth == sServerAuth) {
		player->coopPlayer.admin = true;
		return true;
	}
	return false;
}

//================================================================
// Name:        coop_playerGetDataSegment
// Class:       -
//              
// Description: returns a specific data segment for a player id from ini file
//				like health, ammo and other statistics
//              
// Parameters:  Player*, sort int
//              
// Returns:     string
//              
//================================================================
str coop_playerGetDataSegment( Player *player , short int iNumber )
{
	str sData;
	str sSegment = "";
	sData = coop_parserIniGet( "ini/serverData.ini" , player->coopPlayer.coopId , "client" );
	coop_trimM( sData , " \t\r\n" );

	if ( !Q_stricmp( sData , "" ) )
		return "0";

	int short iChar = 0;
	int short iCurrentSegment = 1;

	for ( iChar = 0; iChar < sData.length(); iChar++ ) {
		if ( sData[iChar] == ' ' ) {
			iCurrentSegment++;
			continue;
		}
		if ( iCurrentSegment == iNumber ) {
			sSegment += sData[iChar];
		}
		else if ( iCurrentSegment > iNumber ) {
			break;
		}
	}
	return sSegment;
}

//================================================================
// Name:        coop_manageIntervalTransmit
// Class:       -
//              
// Description: transmit data in a interval fashion
//              
// Parameters:  gentity_t *ent
//              
// Returns:     VOID
//              
//================================================================
void coop_manageIntervalTransmit( Player* player , str sData , float fInterval , float &last )
{
	//if ( ( last + fInterval ) > level.time ) {
		//return;
	//}
	
	if ( sData == player->coopPlayer.lastScanSendData && ( last + fInterval ) > level.time ){
		//return;
	}
	last = level.time;
	player->coopPlayer.lastScanSendData = sData;
	DelayedServerCommand( player->entnum , sData.c_str() );
}


//================================================================
// Name:        coop_playerSpawnLms
// Class:       -
//              
// Description: tries to spawn the player if in lms or prevent the spawn
//              
// Parameters:  Player *player
//              
// Returns:     BOOL
//              
//================================================================
bool coop_playerSpawnLms( Player *player )
{
	//no coop or lms
	if ( !game.coop_isActive || !game.coop_lastmanstanding || game.levelType < MAPTYPE_MISSION )
		return true;

	//player died after this map was started
	if ( player->coopPlayer.deathTime > game.coop_levelStartTime &&  ( player->coopPlayer.timeEntered + 3 ) < level.time){
		multiplayerManager.makePlayerSpectator(player, SPECTATOR_TYPE_FOLLOW, false);

		if ( !level.mission_failed && ( player->coopPlayer.lastTimeHudMessage + 3 ) < level.time ){
			player->coopPlayer.lastTimeHudMessage = level.time;

			if (coop_checkPlayerLanguageGerman(player)) {
				multiplayerManager.HUDPrint( player->entnum , "^5Coop^8 ^5L^8ast ^5M^8an ^5S^8tanding ^2Aktiv^8 - ^1Sie sind momentan ausgeschaltet.\n" );
			}
			else {
				multiplayerManager.HUDPrint( player->entnum , "^5Coop^8 ^5L^8ast ^5M^8an ^5S^8tanding ^2Active^8 - ^1You are neutralised for the Moment.\n" );
			}
		}
		return false;
	}
	return true;
}

//================================================================
// Name:        coop_playerRestore
// Class:       -
//              
// Description: restore player his previouse health, armor and ammo
//              
// Parameters:  gentity_t *ent
//              
// Returns:     VOID
//              
//================================================================
void coop_playerRestore( Player *player )
{
	if ( !player || !game.coop_isActive || !Q_stricmp( player->coopPlayer.coopId , "" ) )
		return;

	if ( multiplayerManager.isPlayerSpectator( player ) )
	{
		int iTime = atoi( coop_playerGetDataSegment( player , 7 ) );
		player->coopPlayer.deathTime = iTime;
		if ( game.coop_lastmanstanding )
		{
			if ( !multiplayerManager.isPlayerSpectatorByChoice( player ) )
			{
				if ( player->coopPlayer.deathTime < game.coop_levelStartTime && ( mp_warmUpTime->integer + 20 ) < level.time )
				{
					multiplayerManager.playerEnterArena( player->entnum , 666 );
				}
			}
		}
		else if( multiplayerManager.getRespawnTime() > 0.0f )
		{
			time_t result = time( NULL );
			localtime( &result );

			if ( ( iTime + ( int )multiplayerManager.getRespawnTime() ) > ( int )result )
			{
				multiplayerManager._playerData[player->entnum]._waitingForRespawn = true;
				multiplayerManager._playerData[player->entnum]._respawnTime = multiplayerManager.getRespawnTime();
			}
		}
		return;
	}

	//if it is not a mission or custom coop map do not restore
	if ( game.levelType != MAPTYPE_MISSION && game.levelType != MAPTYPE_CUSTOM )
		return;
	
	str sData = coop_parserIniGet( "ini/serverData.ini" , player->coopPlayer.coopId , "client" );
	if ( !Q_stricmp( sData, "") )
	{
		return;
	}

	//health armor phaser plasma fed idryll timestamp
	//100 200 200 200 200 200 1465368163

	//trim/clean
	coop_trimM( sData," \t\r\n" );

	//get first set of data
	int iTempData=0;
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( iTempData > 0 )
	{ 
		//make sure player has at least 10 health!
		if ( iTempData  < 10 )
		{
			iTempData = 10;
		}
		player->health = iTempData;
	}

	//get secound set of data
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );

	//make sure player has at least 10 shield!
	if ( iTempData < 10 )
	{
		iTempData = 10;
	}

	//if ( ( Sentient * )player->GetArmorValue() ){( Sentient * )player->SetArmorValue( iTempData );}
	//else{...}
	Event *armorEvent;
	armorEvent = new Event( EV_Sentient_GiveArmor );
	armorEvent->AddString( "BasicArmor" );
	armorEvent->AddInteger( iTempData );
	player->ProcessEvent( armorEvent );

	//get third set of data
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( iTempData > 0 )
	{player->GiveAmmo("Phaser",(iTempData - player->AmmoCount( "Phaser" )),false,-1);}

	//get forth set of data
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( iTempData > 0 )
	{player->GiveAmmo("Plasma",(iTempData - player->AmmoCount( "Plasma" )),false,-1);}

	//get fifth set of data
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( iTempData > 0 )
	{player->GiveAmmo("Fed",(iTempData - player->AmmoCount( "Fed" )),false,-1);}

	//get sixth set of data
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( iTempData > 0 )
	{player->GiveAmmo("Idryll",(iTempData - player->AmmoCount( "Idryll" )),false,-1);}

	//get seventh set of data - deathtime unixtimestamp
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( sData.length() == 10 ) {
		iTempData = atoi( sData );
		player->coopPlayer.deathTime = iTempData;
		//try to respawnif data is restored
		if ( multiplayerManager.isPlayerSpectator( player ) && !multiplayerManager.isPlayerSpectatorByChoice( player ) ) {
			multiplayerManager.playerEnterArena( player->entnum , iTempData );
		}
	}
}

//================================================================
// Name:        coop_playerSetup
// Class:       -
//              
// Description: Setup the player for coop, execute clientside cfg, set entity vars on player for script use
//              
// Parameters:  gentity_t *ent
// Parameters:  Player *player
//              
// Returns:     bool
//              
//================================================================
bool coop_playerSetup( gentity_t *ent )
{
	if ( !ent )
		return false;
	Player *player = multiplayerManager.getPlayer( ent - g_entities );
	return coop_playerSetup( player );
}
bool coop_playerSetup(Player* player)
{
	if (!player)
		return false;

	//[b607] chrissstrahl - allow cancelation of pending Missionfailure event (only if autofail because of empty server)
	if (game.coop_autoFailPending) {
		world->CancelEventsOfType(EV_World_AutoFailure);
	}

	//[b607] chrissstrahl - check if this player is a bot
	gentity_t* ent = player->edict;
	if (!ent) {
		return false;
	}

	//[b607] daggolin - Restore bot state on player object
	if (level.spawn_bot) {
		ent->svflags |= SVF_BOT;
	}
	//[b610] chrissstrahl - add var in any case
	player->entityVars.SetVariable("_playerIsBot", (float)(int)level.spawn_bot);

	//[b60011] chrissstrahl - what happned to this ? I readded this as I can't find it
	player->coopPlayer.timeEntered = level.time;

	//[b607] chrissstrahl - make sure we do not handle bots
	if (ent->svflags & SVF_BOT) {
		cvar_t* cvar = gi.cvar_get("local_language");
		str sCvar = (cvar ? cvar->string : "Eng");
		player->setLanguage(sCvar);

//player->coopPlayer.setupComplete = true;
		coop_classSet(player, "HeavyWeapon");
		return true;
	}

#ifdef WIN32
	bool bWindows = true;
#else
	bool bWindows = false;
#endif

	if (dedicated->integer == 0 && player->entnum == 0 && bWindows) {
		coop_playerSetupHost(player);
	}
	else {
		coop_playerSetupClient(player);
	}

	//[b60011] chrissstrahl - exit here on singleplayer / solomatch
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return true;
	}

	//hzm coop mod chrissstrahl - disable radar hud selected symbol
	player->coopPlayer.radarSelectedActive = false;
	//[b607] chrissstrahl - this is used to reduce nettraffic on first spawn - needs to be false on start
	player->coopPlayer.radarFirstResetDone = false;

	//hzm coop mod chrissstrahl - mark as not respawned
	//hzm coop mod chrissstrahl - mark to respawn next time where player died
	player->coopPlayer.deathViewangleY = 0;
	//player->coopPlayer.respawnAtRespawnpoint = true;

	//hzm coop mod chrissstrahl - record time when player entred, store in player entity
	player->entityVars.SetVariable("globalCoop_timeEntered", level.time);

	if(game.coop_isActive){
		//hzm coop mod chrissstrahl - run level script threads, used for scriptmod and noscript script
		coop_serverRunScriptThread("globalCoop_teammate_follow");
		coop_serverRunScriptThread("coop_newPlayerEntered"); //noscript?

		//[b607] chrissstrahl - add this player to the communicator menu
		coop_playerCommunicator(player, 1);

		//[b60011] chrissstrahl - set for all players - SCOREBOARD Gametype Name
		DelayedServerCommand(player->entnum, va("set mp_gametypename ^8HZM Coop Mod %i^0 %i", COOP_BUILD, mp_gametype->integer));
	}

	//hzm coop mod chrissstrahl - place player at spawnpoint
	coop_playerPlaceAtSpawn( player );

	//hzm coop mod chrissstrahl - enable ai again, if it was disabled, as there is now a player on the server again
	coop_serverManageAi();

	return true;
}
//================================================================
// Name:        coop_playerSetupClient
// Class:       -
//              
// Description: Does Setup for client player
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSetupClient(Player* player)
{
	//[b60011] chrissstrahl - get player langauge/clientid/clientCoopVersion
	DelayedServerCommand(player->entnum, "vstr local_language");
	DelayedServerCommand(player->entnum, "vstr coop_pId");
	DelayedServerCommand(player->entnum, "vstr coop_verInf"); //[b60011] chrissstrahl - changed to avoid command being shown as text on older servers

	//Do this only during a active coop game
	if (game.coop_isActive) {
		//[b60011] chrissstrahl - get player class
		DelayedServerCommand(player->entnum, "vstr coop_class");

		//[b60011] chrissstrahl - headhudtext widget hide in multiplayer, because it does not work right (flickering)
		DelayedServerCommand(player->entnum, "globalwidgetcommand DialogConsole rect -10000 0 0 0");
	}
}


//================================================================
// Name:        coop_playerSetupHost
// Class:       -
//              
// Description: Does Setup for host player
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSetupHost(Player* player)
{
	cvar_t* cvar = gi.cvar_get("local_language");
	str sCvar = (cvar ? cvar->string : "Eng");
	player->setLanguage(sCvar);

	cvar = gi.cvar_get("coop_verInf");
	sCvar = (cvar ? cvar->string : va("%d", COOP_BUILD));
	coop_manipulateStringTrim(sCvar, "coopinstalled "); //remove command prefix
	player->coopPlayer.installedVersion = atoi(sCvar.c_str());
	player->coopPlayer.installed = true;

	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER ){
		player->coopPlayer.setupComplete = true;
		DelayedServerCommand(player->entnum, "globalwidgetcommand DialogConsole rect 8 7 304 89");
		return;
	}

	bool bGenerateNewId = false;
	cvar = gi.cvar_get("coop_pId");
	sCvar = (cvar ? cvar->string : "");
	coop_manipulateStringTrim(sCvar,"coopcid ");
	player->coopPlayer.coopId = coop_checkPlayerCoopIdExistInIni(player, sCvar);
	coop_playerRestore(player);

	coop_playerSetupCoop(player);

	if (game.coop_isActive) {
		cvar = gi.cvar_get("coop_class");
		sCvar = (cvar ? cvar->string : "");
		coop_manipulateStringTrim(sCvar, "!class "); //remove command prefix
		coop_classSet(player, sCvar);
	}
}

//================================================================
// Name:        coop_playerGenerateNewPlayerId
// Class:       -
//              
// Description: Generates a uniqe playerid for this client for idendification
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerGenerateNewPlayerId(Player* player)
{
	//that should create a pretty uniqe player id
	time_t curTime;
	time(&curTime);
	str sPlayerId = va("%d%d", (int)curTime, player->entnum);
	gi.Printf("\n======================\nSENDING NEW ID TO PLAYER\n======================\n");
	gi.Printf(va("%s\n", sPlayerId.c_str()));
	//add current client number to make sure we add a absolute uniqe player id
	//even if two players join at the same instance
	player->coopPlayer.coopId = sPlayerId.c_str();
	//make sure the cvar is created and saved with seta first before using set - set used because it accepts commands with space
	DelayedServerCommand(player->entnum, va("seta coop_pId 0;set coop_pId coopcid %s", sPlayerId.c_str()));
}

//================================================================
// Name:        coop_playerSaveNewPlayerId
// Class:       -
//              
// Description: Saves a new playerid for this client in the server ini file for idendification
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSaveNewPlayerId(Player *player)
{
gi.Printf("\n======================\nSAVING NEW PLAYER ID\n======================\n");

	//if player has no id send from his config, generate one
	if (!player->coopPlayer.coopId.length()) {
		coop_playerGenerateNewPlayerId(player);
	}

	//write id of player to server ini
	coop_parserIniSet("ini/serverData.ini", player->coopPlayer.coopId, "100 40 0 0 0 0", "client");
	
	//hzm coop mod chrissstrahl - allow new players to join directly in on LMS and respawntime
	if (multiplayerManager.inMultiplayer()) {
		player->coopPlayer.deathTime = 0;
		multiplayerManager._playerData[player->entnum]._waitingForRespawn = true;
		multiplayerManager._playerData[player->entnum]._respawnTime = 0.0f;
	}
}

//================================================================
// Name:        coop_playerSetupCoop
// Class:       -
//              
// Description: Player has coop, do setup, this is supposed to be called only once per level
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSetupCoop( Player *player )
{
	if ( !player ){
		return;
	}

	//hzm coop mod chrissstrahl - notify game about the client state
	//can also be used on regular deathmatch by script check
	player->coopPlayer.installed = 1;

	//make sure the setup # executed while coop is not active
	//because the command can and will be executed even if there is no coop
	if ( !game.coop_isActive ){
		return;
	}

	//hzm coop mod chrissstrahl - execute clientside setup for the specific map, can contain story or translated objectives
	if ( g_gametype->integer != GT_SINGLE_PLAYER ){
		//hzm coop mod chrissstrahl - execute clientside cfg file
		DelayedServerCommand( player->entnum , "exec coop_mod/cfg/init.cfg" );
	}

	//not used anmyore
	//hzm coop mod chrissstrahl - tell client the server version - used for menu and hud features
	//DelayedServerCommand( player->entnum , va( "set coop_svB %i" , COOP_BUILD ) );

	//hzm coop mod chrissstrahl - update mission objective hud and callvote, once
	DelayedServerCommand( player->entnum , va( "globalwidgetcommand coop_objectivesMap title %s" , level.mapname.tolower() ) );
	DelayedServerCommand( player->entnum , va( "globalwidgetcommand coop_objectivesSkillValue title %s" , coop_returnStringSkillname(skill->integer).c_str() ) );
	DelayedServerCommand( player->entnum , va( "globalwidgetcommand coopGpoSkill title %s" , coop_returnStringSkillname(skill->integer).c_str() ));
	DelayedServerCommand( player->entnum , va( "globalwidgetcommand coopGpoMvSpd title %d" , game.coop_maxspeed ) );
	DelayedServerCommand( player->entnum , va( "globalwidgetcommand coopGpoRspwt title %d" , game.coop_respawnTime ) );
	DelayedServerCommand( player->entnum , va( "globalwidgetcommand coopGpoLms title %d" , (int)game.coop_lastmanstanding ) );
	DelayedServerCommand( player->entnum , va( "globalwidgetcommand coopGpoAw title %d" , (int)game.coop_awardsActive ) );

	//[b607] chrissstrahl - deadbodies option
	//[b607] chrissstrahl - teamicon option
	DelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoDb title %d", game.coop_deadBodiesPerArea));
	DelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoCh title %d",(short)coopChallenges.iCurrentChallenge));
	DelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoSt title %d",(int)game.coop_stasisTime));

	//[b607] chrissstrahl - airaccelerate option
	int iAccel;
	if (world->getPhysicsVar(WORLD_PHYSICS_AIRACCELERATE) != -1.0f) {
		iAccel = (int)world->getPhysicsVar(WORLD_PHYSICS_AIRACCELERATE);
	}
	else {
		iAccel = (int)sv_airaccelerate->value;
	}
	DelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoAa title %d", iAccel));

	str sFF = va( "%f" , game.coop_friendlyFire );
	coop_manipulateStringFromWithLength( sFF , 0 , 4 );
	DelayedServerCommand( player->entnum , va( "globalwidgetcommand coopGpoFF title %s" , sFF.c_str() ) );
	
	//hzm coop mod chrissstrahl - disable all other inactive blips
	for ( int i = 0; i < COOP_RADAR_MAX_BLIPS; i++ ){
		player->coopPlayer.radarBlipActive[i] = false;
	}

	//hzm coop mod chrissstrahl - reset objectives, so they may also work in singleplayer
	coop_objectivesSetup( player );
	
	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		player->coopPlayer.setupComplete = true;
		return;
	}

	coop_classSet( player , "current" );

	player->coopPlayer.setupComplete = true;

	//[b60011] chrissstrahl 
	coopChallenges.playerEnteredWarning(player);
}


//================================================================
// Name:        coop_playerSetupNoncoop
// Class:       -
//              
// Description: Player does not have the coop mod installed, this is supposed to be called only once per level
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSetupNoncoop( Player *player)
{
	//hzm coop mod chrissstrahl - notify game about the client state
	player->coopPlayer.installed = 0;

	//hzm coop mod chrissstrahl - tell player that it would be so much better if he has the coop mod
	//this is notr a priority message, we don't bother with it if the client has heavy traffic
	if ( game.coop_isActive && !level.mission_failed ){
		//[b607] chrissstrahl - make sure we do not handle bots
		gentity_t *ent = player->edict;
		if (!(ent->svflags & SVF_BOT)) {
			if ( coop_checkPlayerLanguageGerman(player) ) {
				DelayedServerCommand( player->entnum , "hudprint ^2Holen Sie sich den ^5HZM Coop Mod^2 fuer ein volles Erlebniss! ^5!help^8 eingeben fuer Befehle.\n" );
			}
			else {
				DelayedServerCommand( player->entnum , "hudprint ^2For the full Experience please download the ^5HZM Coop Mod^2! ^8Enter ^5!help^8 for Commands.\n" );
			}			
		}
	}

	//hzm coop mod chrissstrahl - do the regular setup here after coop has or has not been detected
	coop_classSet( player , "current" );
	coop_objectivesSetup( player );
	player->coopPlayer.setupComplete = true;
}


//================================================================
// Name:        coop_playerTransportToSpawn
// Class:       -
//              
// Description: Places the player at the coop spawn location, and uses a beam effect - activated when player uses !stuck
//              
// Parameters:  Player player
//              
// Returns:     bool
//              
//================================================================
bool coop_playerTransportToSpawn( Player *player )
{
	//hzm coop mod chrissstrahl - place player at respawn/spawn location
	player->coopPlayer.respawnAtRespawnpoint = true;

	player->client->ps.pm_time = 100;
	player->client->ps.pm_flags |= PMF_TIME_TELEPORT;

	//[b607] chrissstrahl - do not show effect on dead or spec
	if (player->health > 0 && !multiplayerManager.isPlayerSpectator(player)) {
		Event *newEvent2 = new Event(EV_DisplayEffect);
		newEvent2->AddString("TransportIn");
		newEvent2->AddString("Multiplayer");
		player->PostEvent(newEvent2, 0.0f);
	}

	return coop_playerPlaceAtSpawn( player );
}


//================================================================
// Name:        coop_playerSpawnTryVariable
// Class:       -
//              
// Description: Tries to spawn the player at a spawnloacation provided by a script variable
//              
// Parameters:  Player player
//              
// Returns:     bool
//              
//================================================================
bool coop_playerSpawnTryVariable( Player *player , bool bRespawning )
{
	if ( !player ){
		return false;
	}
	float fSpawnAngle;
	Vector vSpawnOrigin , vAngles;
	vSpawnOrigin[0] = 0.0f;
	vSpawnOrigin[1] = 0.0f;
	vSpawnOrigin[2] = 0.0f;
	if ( bRespawning ){
		//gi.Printf( "coop_playerSpawnTryVariable: respawn\n" );
		//vSpawnOrigin = program.getVectorVariableValue( va( "coop_vector_respawnOrigin%i" , ( 1 + player->entnum ) ) );
		//spawning the player at where he died restore viewangles of the player as they where before he died
		vAngles[0] = 0.0f;
		vAngles[1] = player->coopPlayer.deathViewangleY;
		vAngles[2] = 0.0f;
		player->SetViewAngles( vAngles );
		return true;
	}
	else if ( level.game_script != "" ){//hzm coop mod chrissstrahl - do this only if we know we have a valid scriptfile for this map, cuz scriptmaster keeps values of last script file
		//gi.Printf( "coop_playerSpawnTryVariable: coop_vector_spawnOrigin\n" );
		fSpawnAngle = program.getFloatVariableValue( va("coop_float_spawnAngle%i" , (1 + player->entnum)) );

		if ( fSpawnAngle == 0.0f ){
			if ( program.getFloatVariableValue( "coop_float_spawnAngle0" ) != 0.0f ){
				fSpawnAngle = program.getFloatVariableValue( "coop_float_spawnAngle0" );
			}
		}

		vAngles[1] = fSpawnAngle;
		vSpawnOrigin = program.getVectorVariableValue( va( "coop_vector_spawnOrigin%i" , ( 1 + player->entnum ) ) );
		//gi.Printf( "spawn at var\n" );
	}

	if ( vSpawnOrigin[0] != 0.0f || vSpawnOrigin[1] != 0.0f || vSpawnOrigin[2] != 0.0f ){
		player->setAngles( vAngles );
		player->SetViewAngles( vAngles );
		player->setOrigin( vSpawnOrigin );
		return true;
	}
	return false;
}


//================================================================
// Name:        coop_playerSpawnTryIPD
// Class:       -
//              
// Description: Tries to spawn the player at a targetnames info_player_deathmatch spawnloacation
//              
// Parameters:  Player player
//              
// Returns:     bool
//              
//================================================================
bool coop_playerSpawnTryIPD( Player *player , bool bRespawning )
{
	if ( !player ){
		return false;
	}
	Vector vSpawnOrigin , vAngles;
	vSpawnOrigin[0] = 0.0f;
	vSpawnOrigin[1] = 0.0f;
	vSpawnOrigin[2] = 0.0f;
	vAngles[0] = 0.0f;
	vAngles[1] = 0.0f;
	vAngles[2] = 0.0f;
	if ( bRespawning ){
		//gi.Printf( "coop_playerSpawnTryIPD: respawn\n" );
		//spawning the player at where he died restore viewangles of the player as they where before he died
		vAngles[0] = 0.0f;
		vAngles[1] = player->coopPlayer.deathViewangleY;
		vAngles[2] = 0.0f;
		player->SetViewAngles( vAngles );
		return true;
	}
	else{
		//gi.Printf( "coop_playerSpawnTryIPD: ipd\n" );
		Entity* ent;
		TargetList *tlist;
		tlist = world->GetTargetList( va("ipd%i", ( 1 + player->entnum ) ) , false );
		if ( tlist ){
			ent = tlist->GetNextEntity( NULL );

			vAngles[1] = ent->angles.y;
			player->setAngles( vAngles );
			player->SetViewAngles( vAngles );
			player->setOrigin( ent->origin );
			return true;
		}
	}
	return false;
}


//================================================================
// Name:        coop_playerSpawnTrySpSpawn
// Class:       -
//              
// Description: Tries to spawn the player at a info_player_start spawnloacation
//              
// Parameters:  Player player
//              
// Returns:     bool
//              
//================================================================
bool coop_playerSpawnTrySpSpawn( Player *player , bool bRespawning )
{
	if ( !player ){
		return false;
	}

	Vector vSpawnOrigin , vAngles;
	vSpawnOrigin[0] = 0.0f;
	vSpawnOrigin[1] = 0.0f;
	vSpawnOrigin[2] = 0.0f;
	vAngles[0] = 0.0f;
	vAngles[1] = 0.0f;
	vAngles[2] = 0.0f;
	if ( bRespawning ){
		//gi.Printf( "coop_playerSpawnTrySpSpawn: respawn\n" );
		//spawning the player at where he died restore viewangles of the player as they where before he died
		vAngles[0] = 0.0f;
		vAngles[1] = player->coopPlayer.deathViewangleY;
		vAngles[2] = 0.0f;
		player->SetViewAngles( vAngles );
		return true;
	}
	else{
		//gi.Printf( "coop_playerSpawnTrySpSpawn: info_player_start\n" );
		Entity *spawnLocation;
		spawnLocation = G_FindClass( NULL , "info_player_start" );
		if ( spawnLocation ){
			player->WarpToPoint( spawnLocation );
			gi.Printf("ERROR: No info_player_deathmatch with targetname $ipd%i found\n", (1 + player->entnum)); //[b60011] chrissstrahl - added targetname info
			return true;
		}
		else{
			gi.Printf( "ERROR: No info_player_start or info_player_deathmatch found, spawning player at '0 0 0'\n" );
			return false;		
		}
	}
	player->_makeSolidASAP = true;
	player->_makeSolidASAPTime = 0.0f;
}

//================================================================
// Name:        coop_playerPlaceAtSpawn
// Class:       -
//              
// Description: Places the player at the coop spawn location
//              
// Parameters:  Player player
//              
// Returns:     bool
//              
//================================================================
bool coop_playerPlaceAtSpawn( Player *player )
{
	bool bSpawnedSucessfull = false;
	if ( game.coop_isActive && player )
	{
		player->_makeSolidASAP = true;
		player->_makeSolidASAPTime = 0.25f;

		//check if the player can respawn at the same position he died, next time he will die
		//this is used in m9l2, with lurok
		//if player is blasted into space he will be set to: coop_respawnAtRespawnpoint
		//that will fore him to respawn on a perdefined location in the map, rather than where he died
		//but since we don't want the player to respawn at that spawn location all the time,
		//we will set a var that allowes the force respawn status to be reset: coop_respawnAtReset
		//this way the player can respawn at the same position as long as he does not get blasted into space
		ScriptVariable *entityData = NULL;
		entityData = player->entityVars.GetVariable( "coop_respawnAtReset" );
		if ( entityData != NULL && entityData->floatValue() == 1.0f )
		{
			player->entityVars.SetVariable( "coop_respawnAtRespawnpoint" , 0.0f );
		}

		//gi.Printf("coop_playerPlaceAtSpawn CALLED\n");
		//hzm coop mod chrissstrahl - player died a questionable dead or in a questionable way
		if ( player->coopPlayer.respawnAtRespawnpoint == true || player->coopPlayer.respawned == false ){
			//try spawning at coop predefined script variable spawn
			bSpawnedSucessfull = coop_playerSpawnTryVariable( player , false );
			//not sucessful, try spawning at info_player_deathmatch, with targetname
			if ( !bSpawnedSucessfull ){
				bSpawnedSucessfull = coop_playerSpawnTryIPD( player , false );
				if ( !bSpawnedSucessfull ){
					bSpawnedSucessfull = coop_playerSpawnTrySpSpawn( player , false );
					//let the regular spawn routine handle the spawning...
					//player->WarpToPoint( spawnLocation );
					//gi.Printf( "spawn at and info_player_deathmatch\n" );
					//return false;					
				}
			}
		}
		else{
			//spawning the player at where he died restore viewangles of the player as they where before he died
			Vector vAngles;
			vAngles[0] = 0.0f;
			vAngles[1] = player->coopPlayer.deathViewangleY;
			vAngles[2] = 0.0f;
			player->SetViewAngles( vAngles );

			//hzm coop mod chrissstrahl - reset the player to where he was alive the last time
			//this makes sure the player does not get stuck in a wall or other objects
			//when he is pushed while dieing, which is more than just likley to happen
			//and if I might add, it does happen quite frequently
			player->origin = player->coopPlayer.lastAliveLocation;
			return true;
		}
	}
	return bSpawnedSucessfull;
}

//[b60011] chrissstrahl - major restructure, moved commands to - coopGamecmds.cpp
//================================================================
// Name:        coop_playerSay
// Class:       -
//              
// Description:  this addes the coop specific commands, that have to be entred into chat catch chat text that is suppose to be a coop command, starting with ! as very first letter
//              
// Parameters:  Player *player , str sayString
//              
// Returns:     bool
//              
//================================================================
bool coop_playerSay( Player *player , str sayString)
{
	if ( !player || g_gametype->integer == GT_SINGLE_PLAYER)
		return false;


	//[b60011] chrissstrahl - Strip out any bad characters
	int i;
	for (i = 0; i < sayString.length(); i++){
		if (sayString[i] == '%'){
			sayString[i] = '.';
		}
	}
	sayString = sayString.tolower();

	//[b60011] gameupdate add ! as command start inside say strings
	if (sayString[0] == '!') {
		//send back to client and client console will decide if it wants to send it again without say infront of it
		gi.SendServerCommand(player->entnum, va("stufftext \"%s \"\n", sayString.c_str()));
		return true;
	}

	//[b60011] chrissstrahl - clientid backwardscompatibility - create new id
	if (!Q_stricmpn(sayString.c_str(), "cid.", 4)) {
		coop_playerSaveNewPlayerId(player);
		return true;
	}

	//hzm coop mod chrissstrahl - detect player language
	if (!Q_stricmpn(sayString.c_str(), "deu", 3) || !Q_stricmpn(sayString.c_str(), "eng", 3)) {
		//make sure player has now setup his language correctly
		if (!Q_stricmpn(sayString.c_str(), "deu", 3)) {
			player->setLanguage("Deu");
		}
		else {
			player->setLanguage("Eng");
		}
		return true;
	}

	//SPAM - FILTER - this is our sv_floodprotect replacement, since flood protect also blocks multiplayer specific commands which we are in need of to work
	if (player->coopPlayer.chatTimeLimit < level.time) {
		player->coopPlayer.chatTimeLimit = level.time;
	}
	player->coopPlayer.chatTimeLimit++;

	if (player->coopPlayer.chatTimeLimit > (level.time + 3)) {
		//display info that the player was spamming
		if ((player->coopPlayer.lastTimeSpamInfo + 3.0f) < level.time) {
			player->coopPlayer.lastTimeSpamInfo = level.time;
			if (coop_checkPlayerLanguageGerman(player)) {
				player->hudPrint("Sie chatten zu schnell, Nachricht blockiert durch Spamschutz!\nNutzen Sie die Pfeil nach oben Taste in chat um Nachricht zu wiederholen.\n");
			}
			else {
				player->hudPrint("You chat to fast, message blocked by Spamprotection!\nUse Arrow UP while in text message mode to repeat last message\n");
			}
		}
		return true;
	}
	return false;
}


//================================================================
// Name:        coop_playerEnterArena
// Class:       -
//              
// Description: Handlescoop stuff for player when entering the arena, as active player
//              
// Parameters:  int entnum , float health
//              
// Returns:     bool
//              
//================================================================
void coop_playerEnterArena(int entnum, float health)
{
	if (entnum < 0 || entnum >(game.maxclients - 1))
		return;

	Player *player;
	player = multiplayerManager.getPlayer(entnum);

	if (!player)
		return;

	//hzm coop mod chrissstrahl - spawn and place player
	coop_playerPlaceAtSpawn(player);

	//hzm coop mod chrissstrahl set when the player spawned last time, used to display objectives in text hud
	player->coopPlayer.lastTimeSpawned = level.time;

	if (!game.coop_isActive) {
		return;
	}	

	//[b607] chrissstrahl - add all players to blue team in coop
	//[b608] chrissstrahl - adjusted that it makes player go to blue team if he is not in blue team, added NOTE
	//
	// NOTE: if a player first joins the game this function is not called!
	// NOTE: void MultiplayerManager::addPlayer is always called
	// NOTE: this function should probably called from void MultiplayerManager::addPlayer to be sure it always works
	//		 but this might take to much time for testing and adjusting
	//
	if (multiplayerManager.getPlayersTeam(player) != NULL && multiplayerManager.getPlayersTeam(player)->getName() != "Blue") {
		multiplayerManager.joinTeam(player, "Blue");
		if ((player->coopPlayer.timeEntered + 2) < level.time && !multiplayerManager.isPlayerSpectator(player)) {
			if (coop_checkPlayerLanguageGerman(player)) {
				player->hudPrint("^5INFO:^2 Coop erlaubt nur blaues Team.\n");
			}
			else {
				player->hudPrint("^5INFO:^2 Coop does only allow blue team.\n");
			}
		}
		return;
	}

	//reset the radar hud
	//[b607] chrissstrahl - make sure not to reset twiche
	coop_radarReset( player );

	//hzm coop mod chrissstrahl - used to store if a medic was notified to heal this now critically injured player
	player->coopPlayer.lastTargetedClassSend = "";

	//hzm coop mod chrissstrahl - give weapons, make use start weapon
	coop_armoryEquipPlayer( player );

	//hzm coop mod chrissstrahl - update statistics of how many active players of each class are
	coop_classUpdateClassStats();
	//coop_classUpdateHealthStatFor( player->entnum );

	//hzm coop mod chrissstrahl - manages ai on/off
	coop_serverManageAi();

	//hzm coop mod chrissstrahl - this clears the notargetflag and makes player valid target for ai
	if ((player->flags & FL_NOTARGET))
		player->flags &= ~FL_NOTARGET;

	//hzm coop mod chrissstrahl - display a icon and some text, this is a test...
	//int iconIndex;
	//iconIndex = gi.imageindex("sysimg/icons/items/armor");
	//player->setItemText(iconIndex, va("$$Specialties$$: $$Medic$$ %i", 10000 ));


	//Event *newEvent = new Event(EV_Sentient_SetViewMode);
	//newEvent->AddString("forcevisible");
	//newEvent->AddInteger(0);
	//player->ProcessEvent(newEvent);

	//hzm coop mod chrissstrahl - check if player has a certain item/weapon
	//if (player->FindItemByModelname("models/weapons/worldmodel-tricorder.tik"))

	//_playerSpecialtyData[player->entnum]._specialty == SPECIALTY_HEAVY_WEAPONS;
	//gi.SendServerCommand( player->entnum , "stufftext \"ui_addhud mp_specialties\"\n" );	

	//hzm coop mod chrissstrahl - notify level scripts that the player just spawned - this is used on custom map scripts
	//[b608] chrissstrahl - fixed using client id instead of player targetname number which is client-id + 1
	coop_serverRunScriptThread( va( "coop_justSpawnedplayer%i" , (player->entnum + 1)) );

	//hzm coop mod chrissstrahl - add all mission relevant huds to the players interface
	str sValue;
	ScriptVariable *entityData;
	int iHuds;
	for ( iHuds = 0; iHuds < 8; iHuds++ ){

		sValue = "";
		entityData = NULL;
		entityData = world->entityVars.GetVariable( va( "coop_registredHud%d" , iHuds ) );

		if ( entityData == NULL ){
			continue;
		}
		
		sValue = entityData->stringValue();
		if ( sValue  != ""){
			coop_hudsAdd( player , sValue );
		}

		//[b60011] chrissstrahl - execute command if one is attached - usually to exec a cfg or to handle globalwidgetcommands for ui elements
		sValue = "";
		entityData = NULL;
		entityData = world->entityVars.GetVariable(va("coop_registredHud%d_command", iHuds));
		if (entityData != NULL) {
			sValue = entityData->stringValue();
			if (sValue != "") {
				DelayedServerCommand(player->entnum, sValue.c_str());
			}
		}

	}

	//hzm coop mod chrissstrahl - sometimes the timer hud reactivates
	//if ( game.coop_isActive ) {
		//DelayedServerCommand( player->entnum , "globalwidgetcommand dmTimer disable" );
	//}

	//hzm coop mod chrissstrahl - retore health, armor and ammo to previouse state
	coop_playerRestore(player);

//[b60011] chrissstrahl - handle NPC / AI Teammates
//coopNpcTeam.playerReadyCheck(player);
//already in think

	//[b60011] chrissstrahl 
	coopChallenges.playerEntered(player);
}


//================================================================
// Name:        coop_playerKilled
// Class:       -
//              
// Description: this is our own death message handling function, return false if player is not meant to die (coop_gametype)
//              
// Parameters:  Player *killedPlayer , Entity *attacker , Entity *inflictor , int meansOfDeath
//              
// Returns:     void
//              
//================================================================
bool coop_playerKilled( const Player *killedPlayer , const Entity *attacker , const Entity *inflictor , const int meansOfDeath )
{
	if ( !killedPlayer || !attacker || !inflictor ){
		return false;
	}

//check means of death, if vaporised do not keep alive!
//check if player is alone, if the player is alone, make the player die within a moment, unless he shoots a enemy
	Player *playerPrey = ( Player * )g_entities[killedPlayer->entnum].entity;
	Entity *entityAttacker = ( Entity * )g_entities[attacker->entnum].entity;
	Entity *entityInflictor = ( Entity * )g_entities[inflictor->entnum].entity;

	//hzm coop mod chrissstrahl - used to store server time at wich the player died last
	time_t result = time( NULL );
	localtime( &result );
	playerPrey->coopPlayer.deathTime = ( int )result;
	//[b607] chrissstrahl - remember when this player died last in this level
	playerPrey->coopPlayer.diedLast = level.time;

	//hzm coop mod chrissstrahl - remember where the player was alive the last time
	playerPrey->coopPlayer.lastAliveLocation = killedPlayer->origin;

	//[b607] chrissstrahl - we now set the data above on the player even if we are not in coop, so we have accsess to them in multi
	if (!game.coop_isActive) {
		return true;
	}

	//hzm coop mod chrissstrahl - check if mission failed
	coop_serverLmsCheckFailure();

	//hzm coop mod chrissstrahl - force update to the restore data of the player
	coop_serverSaveClientData( playerPrey );

	if (	//probably not supported, but we add them anyway
			meansOfDeath != MOD_ELECTRICWATER &&
			meansOfDeath != MOD_GIB &&
			meansOfDeath != MOD_IMPALE &&
			meansOfDeath != MOD_EAT &&
			meansOfDeath != MOD_UPPERCUT &&
			//supported for sure
		//meansOfDeath != MOD_SUICIDE && //currently needed to work, so we can test it
			meansOfDeath != MOD_NONE &&
			meansOfDeath != MOD_VAPORIZE_COMP &&
			meansOfDeath != MOD_VAPORIZE_DISRUPTOR &&
			meansOfDeath != MOD_VAPORIZE_PHOTON &&
			meansOfDeath != MOD_SNIPER &&
			meansOfDeath != MOD_DROWN &&
			meansOfDeath != MOD_LAVA &&
			meansOfDeath != MOD_CRUSH &&
			meansOfDeath != MOD_CRUSH_EVERY_FRAME &&
			meansOfDeath != MOD_FALLING &&
			meansOfDeath != MOD_VAPORIZE )
	{
		if ( game.coop_gametype == 1 ){
			//keep alive
			playerPrey->health = 1;
			if ( playerPrey->coopPlayer.neutralized ){
				return false;
			}
			//this should make the ai ignore this player
			if ( !( killedPlayer->flags & FL_NOTARGET ) ){
				playerPrey->flags ^= FL_NOTARGET;
			}
			//change mass, so ai can't kick player arround
			playerPrey->coopPlayer.lastMass = killedPlayer->mass;
			playerPrey->mass = 9999;

			//used to signal the game that this player is currently neutralized
			playerPrey->coopPlayer.neutralized = true;
			// Give the player the phaser, just to be safe
			playerPrey->giveItem( "models/weapons/worldmodel-phaser-stx.tik" );
			//use phaser when neutralized
			playerPrey->useWeapon( "phaser-stx" , WEAPON_DUAL );
			//set info so the coop client think knows when to dissable the current weapon
			playerPrey->coopPlayer.lastTimeNeutralized = level.time;
			str printString = killedPlayer->client->pers.netname;
			printString += "^8 was Injured, and needs help to get up\n";
			coop_textHudprintAll( printString.c_str() );

			//exit from here on
			return false;
		}
		else if ( game.coop_gametype == 2 ){
		}
	}

	//[b607] chrissstrahl - copied here, so it will be executed also in coop
	if (playerPrey->kill_thread.length() > 0) {
		ExecuteThread(playerPrey->kill_thread.c_str(), true, playerPrey);
	}

	int i = 0;
	bool idendified = false;

	str sModel = coop_returnStringFilenameOnly( inflictor->model ).tolower();
//////gi.Printf( va( "%s #%s %s killed %s! $%s mod: %f \n" , inflictor.getClassname() , attacker.getClassname() , sModel.c_str() , killedPlayer->client->pers.netname , inflictor->targetname , meansOfDeath ) );
	//playerPrey->hudPrint( sModel.c_str() );
	
	//this is the printout string of the killed message, let us start with the player his name
	str printString = killedPlayer->client->pers.netname;

	//hzm coop mod chrissstrahl - set viewangle to restore on player if he is respawning at the same location he died
	//hzm coop mod chrissstrahl - remove any attached stuff to prevent the stupid console to print error spam info
	Vector vView;
	vView = playerPrey->getViewAngles();
	playerPrey->coopPlayer.deathViewangleY = floor( vView[1] );
	playerPrey->coopPlayer.respawned = true;
	playerPrey->coopPlayer.injuredSymbolVisible = false;
	playerPrey->coopPlayer.respawnAtRespawnpoint = false;

	//check if we are allowed to spawn players at the place they die or if we are forced to respawn them on a save respawnlocation
	ScriptVariable *entityData = NULL;
	ScriptVariable *entityData2 = NULL;
	entityData = world->entityVars.GetVariable( "coop_respawnAtRespawnpoint" );
	entityData2 = playerPrey->entityVars.GetVariable( "coop_respawnAtRespawnpoint" );
	
	if (	entityData != NULL && entityData->floatValue() == 1.0f ||
			entityData2 != NULL && entityData2->floatValue() == 1.0f
		)
	{
		playerPrey->coopPlayer.respawnAtRespawnpoint = true;
	}

	coop_classUpdateClassStats();
	//coop_classUpdateHealthStatFor( killedPlayer->entnum );

	playerPrey->removeAttachedModelByTargetname( "globalCoop_playerInjured" );
	//hzm coop mod chrissstrahl - we do no longer attach model, class is now shown with the player name when targeted
	//killedPlayer->removeAttachedModelByTargetname( "globalCoop_playerClassIndicator" );

	coop_hudsKilled((Player *)killedPlayer); //chrissstrahl - added for better managment [b607]
	//[b608] chrissstrahl - fixed using client id instead of player targetname number which is client-id + 1
	coop_serverRunScriptThread(va("coop_justDiedplayer%i", (killedPlayer->entnum + 1)));

//check uservars of inflictor, should we have a custom killmessage ?
//should we use a custom name for the inflictor ?	
	Entity *ePurp = entityAttacker;
	//if(inflictor->isSubclassOf( Projectile ))

	Entity *owner = NULL;
	owner = coop_returnOwnerEntity( entityInflictor );
	//gi.Printf( va( "inflictor:%i attacker:%i killed:%i owner:%i Targetname:%s\n" , inflictor->entnum , attacker->entnum , killedPlayer->entnum , owner->entnum , attacker->targetname.c_str() ) );

	if ( owner == NULL ){
		owner = entityAttacker;
	}

	if ( !attacker->isSubclassOf( Actor ) && !owner->isSubclassOf( Player ) && !owner->isSubclassOf( Actor ) )
	{
		gi.Printf( va( "inflictor: %i - ePurp: %i\n" , inflictor->entnum , ePurp->entnum) );
		//[609] chrissstrahl - reenabled this, because a trigger (mod_electric) killing a player
		//would make the code use $world from attacker, instead of using the inflictor which is
		//the actual cause for the damage - if world is really the attacker it is also the inflictor
		ePurp = (Entity *)inflictor;//inflictor
	}

	//if inflictor (mostly kill triggers) is marked as badspot, it means we don't want players to respawn there
	if ( coop_checkStringInUservarsOf( ePurp , "badspot" ) ||
		//probably not supported, but we add them anyway
		meansOfDeath == MOD_ELECTRICWATER ||
		meansOfDeath == MOD_GIB ||
		meansOfDeath == MOD_IMPALE ||
		meansOfDeath == MOD_EAT ||
		//meansOfDeath == MOD_SUICIDE || //- disabled for testing
		meansOfDeath == MOD_NONE ||
		meansOfDeath == MOD_ELECTRIC || //[609] chrissstrahl - added to prevent players from respawning there
		meansOfDeath == MOD_VAPORIZE_COMP ||
		meansOfDeath == MOD_VAPORIZE_DISRUPTOR ||
		meansOfDeath == MOD_VAPORIZE_PHOTON ||
		meansOfDeath == MOD_SNIPER ||
		meansOfDeath == MOD_DROWN ||
		meansOfDeath == MOD_LAVA ||
		meansOfDeath == MOD_CRUSH ||
		meansOfDeath == MOD_CRUSH_EVERY_FRAME ||
		meansOfDeath == MOD_FALLING ||
		meansOfDeath == MOD_VAPORIZE)
	{
		playerPrey->coopPlayer.respawnAtRespawnpoint = true;
	}

	str sEntityName , eEntityKillmessage;
	//[609] chrissstrahl - falldamage should always be falldamage
	if (meansOfDeath != MOD_FALLING) {
		for (i = 1; i < 5; i++) {
			int j = 0;
			entityData = NULL;
			sEntityName = "";
			eEntityKillmessage = NULL;
			entityData = ePurp->entityVars.GetVariable(va("uservar%d", i));
			if (entityData == NULL) {
				continue;
			}
			sEntityName = entityData->stringValue();

			if (!Q_stricmpn("killmessage", sEntityName, 11)) {
				if (sEntityName.length() > 12) {
					str tempName = sEntityName;
					sEntityName = NULL;
					for (j = 12; j < tempName.length(); j++) {
						sEntityName += tempName[j];
					}
					printString += sEntityName;
					idendified = true;
					break;
				}
			}
			else if (!Q_stricmpn("name", sEntityName, 4)) {
				if (sEntityName.length() > 5) {
					str tempName = sEntityName;
					sEntityName = "";
					printString += "^8 was neutralized by ";
					for (j = 5; j < tempName.length(); j++) {
						sEntityName += tempName[j];
					}
					printString += sEntityName;
					idendified = true;
				}
				break;
			}
		}
	}
//custom defined kill message, print it and exit here
	if ( idendified == true ){
		//gi.Printf( va( "Used custom name or killmessage in uservar%d of '$%s' (%s)\n" , i , ePurp->targetname , ePurp->getClassname() ) );
		printString += "\n";
		coop_textHudprintAll( printString.c_str() );
		return true;
	}
//check if the inflictor is a player
	if ( attacker->isSubclassOf( Player ) || !Q_stricmp(sModel,"fx-sml-exp.tik") || !Q_stricmpn( entityInflictor->getClassname(), "MultiExploder", 13) || !Q_stricmpn( entityInflictor->getClassname() , "ExplodeObject" , 13 ) ){
		if ( attacker != killedPlayer && attacker->isSubclassOf( Player ) ){
			printString += va(" was neutralized by their Teammate: %s",attacker->client->pers.netname );
		}else if (	!Q_stricmp( sModel, "fx-sml-exp.tik") ||
					!Q_stricmpn( entityInflictor->getClassname() ,  "MultiExploder", 13 ) ||
					!Q_stricmpn( entityInflictor->getClassname() , "ExplodeObject" , 13 ))
		{
			printString += "^8 was neutralized by a ^2Explosion";
		}
		else
		{
			printString += " ^8$$MOD_SUICIDE$$";
		}
	}
//check if inflictor is NOT a Actor, and if we are killed by a certain damage type
	else if ( !attacker->isSubclassOf( Actor ) &&
				meansOfDeath == MOD_FALLING ||
				meansOfDeath == MOD_DROWN ||
				meansOfDeath == MOD_SLIME ||
				meansOfDeath == MOD_ELECTRIC ||
				meansOfDeath == MOD_FIRE ||
				meansOfDeath == MOD_ON_FIRE ||
				meansOfDeath == MOD_LAVA )
	{
		switch ( meansOfDeath )
		{
		case MOD_DROWN:
			printString = "$$MOD_DROWN$$";
			break;
		case MOD_LAVA:
			printString = "$$MOD_LAVA$$";
			break;
		case MOD_SLIME:
			printString = "$$MOD_SLIME$$";
			break;
		case MOD_FALLING:
			printString = "$$MOD_FALLING$$";
			break;
		case MOD_ELECTRIC:
			printString = "was electrified";
			break;
		case MOD_FIRE:
		case MOD_ON_FIRE:
			printString = "was burned";
			break;
		default:
			printString = "$$MOD_SUICIDE$$";
			break;
		}
		playerPrey->coopPlayer.respawnAtRespawnpoint = true;
		printString = va( "%s %s" , killedPlayer->client->pers.netname , printString.c_str() );
	}
//inflictor is a actor
	else if ( attacker->isSubclassOf( Actor ) ){
		Actor *actor = (Actor*)attacker;
		str sActorResolved = coop_parserIniGet("deathlist.ini", actor->name.c_str(), "actorname");
		if ( !sActorResolved.length() ) {
			printString += va(" # Unhandled actorname: %s\n", actor->name.c_str());
		}
		else {
			printString += sActorResolved;
		}

	}
	else if ( !Q_stricmp( entityInflictor->getClassname(), "TriggerHurt") ){
		//hzm coop mod chrissstrahl - used to determin if player should respawn at where he is or at a predefinied spawn location
		playerPrey->coopPlayer.respawnAtRespawnpoint = true;
		printString += " ^8$$MOD_SUICIDE$$";
	}
//hzm coop mod chrissstrahl - owner is not a actor
/////////possible owners:
//SCRIPT OWNER
//-ScriptOrigin //script_origin entity
	else{
		printString += coop_returnStringDeathmessagenameForEntity( entityInflictor );
	}

	//hzm coop mod chrissstrahl - The Line Must Be Drawn Here! This far no further! <- omg a movie qoute, is this your first contact ? Maybe you should assimilate more code...
	printString += " \n";

	//hzm coop mod chrissstrahl - keep text minimal after failure
	if ( !level.mission_failed ) {//hzm gameupdate chrissstrahl - keep text minimal after failure
		coop_textHudprintAll( printString.c_str() );
	}

	return true;
}


//================================================================
// Name:        coop_playerModelChanged
// Class:       -
//              
// Description: this is our own death message handling function i'm sure we will add more stuff to it as we develop and test the mod
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerModelChanged( Player *player )
{
	if ( game.coop_isActive && player )
	{
		//hzm coop mod chrissstrahl - 
		player->coopPlayer.lastTimeChangedClass = level.time;

		//hzm coop mod chrissstrahl - remove the injured symbol
		player->coopPlayer.injuredSymbolVisible = false;
		player->removeAttachedModelByTargetname( "globalCoop_playerInjured" );

		//hzm coop mod chrissstrahl - notify level scripts of modelchange - this is used on custom coop maps
		//[b608] chrissstrahl - fixed using client id instead of player targetname number which is client-id + 1
		coop_serverRunScriptThread( va( "coop_changedModel%i", (player->entnum + 1) ) );
	}
}


//================================================================
// Name:        coop_playerUsed
// Class:       -
//              
// Description: called when the player gets used
//              
// Parameters:  Player *usedPlayer , Player *usingPlayer , Equipment *equipment 
//              
// Returns:     bool
//              
//================================================================
void coop_playerUsed( Player *usedPlayer , Player *usingPlayer , Equipment *equipment )
{
	coop_classPlayerUsed( usedPlayer , usingPlayer , equipment );
}


//================================================================
// Name:        coop_playerDamaged
// Class:       -
//              
// Description: this is called wehen the player gets hurt/damaged
//              
// Parameters:  Player *damagedPlayer , Player *attackingPlayer , float damage , int meansOfDeath 
//              
// Returns:     bool
//              
//================================================================
void coop_playerDamaged( Player *damagedPlayer , Player *attackingPlayer , float damage , int meansOfDeath )
{
	//if ( damagedPlayer ){
	//}
}


//================================================================
// Name:        coop_playerSpectator
// Class:       -
//              
// Description: handles coop related stuff when player becomes a spectator
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSpectator( Player *player )
{
	//[b607] chrissstrahl - once the player is a spectator we don't need to execute this again
	//reduce nettraffic and multiexecution of script threads and ini writings
	if (multiplayerManager._playerData[player->entnum]._spectator) {
		return;
	}

	//hzm coop mod chrissstrahl - remove huds - [b607] now using a centralized function
	coop_hudsKilled(player);

	//hzm coop mod chrissstrahl - save latest data from player
	if ( ( mp_warmUpTime->integer + 20 ) < level.time ){
		coop_serverSaveClientData( player );
	}

	player->coopPlayer.injuredSymbolVisible = false;
	player->_makeSolidASAPTime = -1.0f;
	player->_makeSolidASAP = false;

	if ( !game.coop_isActive ){
		return;
	}

	//[b60011] chrissstrahl - handle NPC / AI Teammates
	coopNpcTeam.playerSpectator(player);
	coopChallenges.playerSpectator(player);

	//hzm coop mod chrissstrahl - remove the injured symbol
	player->coopPlayer.injuredSymbolVisible = false;
	player->removeAttachedModelByTargetname( "globalCoop_playerInjured" );

	//hzm coop mod chrissstrahl - this should make the ai ignore this player
	if ( !( player->flags & FL_NOTARGET ) ){
		player->flags ^= FL_NOTARGET;
	}

	//hzm coop mod chrissstrahl - reset respawn flag
	if ( !multiplayerManager._playerData[player->entnum]._waitingForRespawn ) {
		player->coopPlayer.respawned = false;
		player->coopPlayer.respawnAtRespawnpoint = true;
	}

	//hzm coop mod chrissstrahl - notify level scripts of teamchange - this is used on custom map scripts
	//[b608] chrissstrahl - fixed using client id instead of player targetname number which is client-id + 1
	coop_serverRunScriptThread( va( "coop_justBecameSpectatorplayer%i" , (player->entnum + 1) ));

	//hzm coop mod chrissstrahl - update statistics of how many active players of each class are
	coop_classUpdateClassStats();

	//hzm coop mod chrissstrahl - chek if we should disable the ai
	coop_serverManageAi();
}

//================================================================
// Name:        coop_playerMakeSolidASAP
// Class:       -
//              
// Description: Makes player solid as soon as possible - not stuck in actor or other player
//              
// Parameters:  Player *player
//              
// Returns:     BOOL
//              
//================================================================
bool coop_playerMakeSolidASAPThink(Player* player)
{
	//check if inside a actor or a player
	if ( player->_makeSolidASAP ) {
		if ( coop_checkInsidePlayerOrActor( ( Entity* )player ) || level.cinematic ) {
			player->setSolidType( SOLID_NOT );
			player->_makeSolidASAP = true;
			return false;
		}
		if ( g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager._playerData[player->entnum]._spectator ) {
			player->setSolidType( SOLID_BBOX );
		}
		player->_makeSolidASAP = false;
	}
	return true;//[b60011] chrissstrahl - moved
}

//================================================================
// Name:        coop_playerPlaceableThink
// Class:       -
//              
// Description: Handles the placable object each - called from playerthink
//              
// Parameters:  Player *player
//              
// Returns:     VOID
//              
//================================================================
void coop_playerPlaceableThink(Player* player)
{
	if (!player->coopPlayer.ePlacable) {
		return;
	}

	//abbort in certain conditions are meet
	if (	(player->getLastDamageTime() + 0.5f) > level.time	||
			multiplayerManager.isPlayerSpectator(player)		||
			player->client->ps.jumped							||
			level.cinematic == qtrue							||
			player->health <= 0	) 
	{
		player->coopPlayer.ePlacable->PostEvent(EV_Remove, 0.0f);
		player->coopPlayer.ePlacable = NULL;
		return;
	}

	SpawnArgs		args;
	Entity*			obj;
	Vector			vPlayer, vPlayerAngle, end_pos;
	trace_t			trace;
	vec3_t			fwd;
	AngleVectors(player->client->ps.viewangles, fwd, NULL, NULL);
	Vector forward = fwd;
	//end_pos = (fwd * 100.0f) + start;

	//trace = G_Trace(start, vec_zero, vec_zero, end_pos, player, MASK_PROJECTILE, false, "ProjectileAttack");
	vPlayer = player->client->ps.origin;
	float fBboxHeight = player->maxs[2];
	fBboxHeight = (fBboxHeight - (fBboxHeight / 100 * 20));
	vPlayer[2] += fBboxHeight; //set z height to 80% of bbox size
	end_pos = (forward * 100.0f + vPlayer);
	trace = G_Trace(vPlayer, vec_zero, vec_zero, end_pos, player, MASK_PROJECTILE, false, "ProjectileAttack");
	//player->coopPlayer.ePlacable->setOrigin(trace.endpos);
	trace.endpos[2] = vPlayer[2];
	vPlayerAngle[1] = player->client->ps.viewangles[1];
	if (player->coopPlayer.ePlacable) {
		player->coopPlayer.ePlacable->setOrigin(trace.endpos);
		player->coopPlayer.ePlacable->setAngles(vPlayerAngle);

		//drop about 100 units - do stuff here if it could not be dropped
		if (!player->coopPlayer.ePlacable->droptofloor(100)) {
			//if object could not be placed, display it at the height of feet of player and change animation so it changes skin/texture
			trace.endpos[2] = player->client->ps.origin[2];
			player->coopPlayer.ePlacable->setOrigin(trace.endpos);
			player->coopPlayer.ePlacable->animate->RandomAnimate("location_invalid");
			//gi.Printf("ePlacable - can't be placed here\n");
		}
		else {
			extern Event EV_Object_SetAnim;
			Event* event;
			event = new Event(EV_Object_SetAnim);
			event->AddString("location_valid");
			player->coopPlayer.ePlacable->ProcessEvent(event);

			//wait after the placable is shown to player before the click counts at placing it
			if (player->upgCircleMenu.thinkTime + (0.45) < level.time && player->GetLastUcmd().buttons & (BUTTON_ATTACKLEFT | BUTTON_ATTACKRIGHT)) {
				//perevent weapon firing
				Event* StopFireEvent;
				StopFireEvent = new Event(EV_Sentient_StopFire);
				StopFireEvent->AddString("dualhand");
				player->ProcessEvent(StopFireEvent);

				//args.setArg("model", player->coopPlayer.ePlacable->model.c_str());

				bool classSpecificStation = false;
				if ("models/item/mp_weapon-spawn.tik" == player->coopPlayer.ePlacable->model) {
					classSpecificStation = true;
					if (player->coopPlayer.className == "HeavyWeapons" || g_gametype->integer == GT_SINGLE_PLAYER) { //Make it work in singleplayer for testing
						args.setArg("model", "models/item/coop_ammoStation.tik");
					}
					else if (player->coopPlayer.className == "Medic") {
						args.setArg("model", "models/item/coop_mediStation.tik");
					}
					else {
						args.setArg("model", "models/item/coop_techStation.tik");
					}
				}
				else {
					args.setArg("model", player->coopPlayer.ePlacable->model.c_str());
				}

				args.setArg("classname", player->coopPlayer.ePlacable->getClassname());
				args.setArg("setmovetype", "" + player->coopPlayer.ePlacable->getMoveType());
				args.setArg("targetname", player->coopPlayer.ePlacable->targetname.c_str());
				args.setArg("notsolid", "1");
				//args.setArg("anim", "idle");
				obj = args.Spawn();
				obj->setOrigin(player->coopPlayer.ePlacable->origin);
				obj->setAngles(player->coopPlayer.ePlacable->angles);
				obj->setSize(player->coopPlayer.ePlacable->mins, player->coopPlayer.ePlacable->maxs);
				//handle class specific stations
				if (classSpecificStation) {
					//remove any previouse class specific placed objects
					if (player->coopPlayer.eClassPlacable) {
						Event* RemoveMe;
						RemoveMe = new Event(EV_Remove);
						player->coopPlayer.eClassPlacable->ProcessEvent(RemoveMe);
					}
					//remember the class specific placed object
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
				}

				player->coopPlayer.ePlacable->PostEvent(EV_Remove, 0.0f);
				player->coopPlayer.ePlacable = NULL;

				player->_makeSolidASAP = true;
				player->_makeSolidASAPTime = 0.0f;
			}
		}
	}
}

//================================================================
// Name:        coop_playerClientThink
// Class:       -
//              
// Description: This will be called once for each client frame, which will
//              usually be a couple times for each server frame.
//              
// Parameters:  Player *player
//              
// Returns:     VOID
//              
//================================================================
void coop_playerThink( Player *player )
{
	if ( !player ){
		return;
	}
	coop_playerMakeSolidASAPThink( player );
	coop_playerPlaceableThink(player);

	//[608] chrissstrahl - moved up here
	//hzm coop mod chrissstrahl - update objectives every secound in sp
	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		if ( ( player->coopPlayer.lastTimeThink + 1.0f ) < level.time ){
			player->coopPlayer.lastTimeThink = level.time;
			coop_objectivesUpdatePlayer( player );
		}
		return;
	}

	//[b60011] chrissstrahl - put the code in dedicated functions
	coop_checkDoesPlayerHaveCoopMod(player);
	coop_checkDoesPlayerHaveCoopId(player);

	//[b607] chrissstrahl - moved here to prevent players staying solid in regular Multimatch
	//hzm coop mod chrissstrahl - exit here if this is not coop
	if (!game.coop_isActive) { //GT_SINGLE_PLAYER - see above
		return;
	}

	//hzm coop mod chrissstrahl
	//- update radar
	//- show injured sprite
	coop_radarUpdate( player );
	coop_modelDisplayInjuredSprite( player );

	weaponhand_t	hand = WEAPON_ANY;//get player weapon, we might want to utilize that further
	str				weaponName;
	player->getActiveWeaponName( hand , weaponName );

	//hzm coop mod chrissstrahl - cheap way to allow player move again - temporary changeme fixme unfinished
	//we don't want players to go to spectator and come back again, no no no, play fair
	//this code needs work!
	if ( !multiplayerManager.isPlayerSpectator( player ) ){

		//hzm coop mod chrissstrahl - make sure that the player does not respawn inside a door, this would be bad
		if ( ( player->coopPlayer.lastTimeSpawned + 1 ) > level.time ) {
			if ( coop_checkEntityInsideDoor( ( Entity * )player ) ) {
				player->coopPlayer.respawnAtRespawnpoint = true;
				coop_playerPlaceAtSpawn( player );
			}
		}

		if ( player->health > 1 ){
			player->coopPlayer.neutralized = false;
			player->disableUseWeapon( false );
		//do no longer ignore this player
			if ( ( player->flags & FL_NOTARGET ) ){
				player->flags ^= FL_NOTARGET;
			}
		}
		else{
			if ( (player->coopPlayer.lastTimeNeutralized + 0.25f) > level.time ){
				if ( stricmp( weaponName , "phaser-stx" ) == 0 ){
					//disable weapon use from now on - means player can no longer change his weapon
					player->disableUseWeapon( true );
				}
			}
		}
	}
	
//hzm coop mod chrissstrahl - check/do this each secound once
	if ( ( player->coopPlayer.lastTimeThink + 1.0f ) < level.time ){
		player->coopPlayer.lastTimeThink = level.time;

		//count text counter down
		coop_textChatTimeCountDown( player );

		//display update notification menu if needed
		coop_hudsUpdateNotification( player );

		//needs only to be send to players that played the coop mod in singleplayer on a custom map before, so only send to players with coop mod
		if ( player->coopPlayer.installed ) {
			DelayedServerCommand( player->entnum ,"bind TAB +objectives_score");
		}

		if ( player->health < 0.0f || sv_cinematic->integer != 0 || multiplayerManager.isPlayerSpectator( player ) ) {
			return;
		}
			
		//coop_serverSaveClientData( player );
		coop_objectivesUpdatePlayer( player );
		coop_classRegenerate( player );
		coop_classCheckApplay( player );
		coop_classCeckUpdateStat( player );

		//[b60011] chrissstrahl - handle NPC / AI Teammates
		coopNpcTeam.playerReadyCheck(player);
	}
}

//==========================================================[b607]
// Name:        coop_playerConnect
// Class:       -
//              
// Description: executed when a player connects
//              
// Parameters:  
//              
// Returns:     void
//              
//================================================================
void coop_playerConnect(Entity *ePlayer)
{
	if (!game.coop_isActive) {
		return;
	}

	//[b607] chrissstrahl - clear configstrings, to allow new player to connect
	if ( level.time > 30.0f ) {
		coop_serverConfigstringRemoveNonCoopStrings();
	}

	if (ePlayer) {
	}
}

//================================================================
// Name:        coop_playerLeft
// Class:       -
//              
// Description: executed when a player disconnects, quits or times out, reliable since it is called by the player destructor
//              
// Parameters:  
//              
// Returns:     void
//              
//================================================================
void coop_playerLeft( Player *player )
{
	if ( !game.coop_isActive || !player ){
		return;
	}

	//[b60011] chrissstrahl - moved code here
	//enable/disable ai
	//check if mission failed
	coop_serverManageAi();
	coop_serverLmsCheckFailure();	

	//hzm coop mod chrissstrahl - save current status when player leaves the game (unless he is spec)
	if ( !multiplayerManager.isPlayerSpectator( player ) ){
		coop_serverSaveClientDataWrite( player );
		
		//[b60011] chrissstrahl - handle NPC / AI Teammates
		coopNpcTeam.playerLeft(player);
	}

	//chrissstrahl - make sure server is restarted if it really needs to
	//no player on the server left, see if server should be rebooted
	if (coop_returnPlayerQuantityInArena() <= 1) { //[b607] chrissstrahl - fixed, because this counts leaving player as well
		if (coop_serverManageReboot(level.mapname.c_str())) {
			return; //[b607] chrissstrahl - if server is going to reboot we don't need to continue
		}

		//[b607] chrissstrahl - fail mission (delayed) so the server can reload the map (this prevents certain overload issues caused by ongoing mission scripts (spawning etc))
		int iFailtime = coop_returnCvarInteger("coop_autoFailtime");

		//yes -1 deactivates failtime
		if (iFailtime < 0) {
			return;
		}

		//let us be a little paranoid
		if (iFailtime == 0) {
			iFailtime = 1;
		}
		else if (iFailtime > 3000) {
			iFailtime = 3000;
		}

		//[b60011] chrissstrahl - reset air accelerate, to prevent new players to get confused or stuck
		world->setPhysicsVar("airAccelerate", COOP_DEFAULT_AIRACCELERATE);
		//[b60011] chrissstrahl - reset max speed, to prevent new players to get confused
		world->setPhysicsVar("maxSpeed", COOP_DEFAULT_MAXSPEED);

		//[b60011] chrissstrahl - flushtikis - fixing animation issues of actor and other models - just to be sure
		coopServer.flushTikis();

		game.coop_autoFailPending = true;
		Event *newEvent2 = new Event(EV_World_AutoFailure);
		world->PostEvent(newEvent2, (iFailtime * 60));
		return;
	}

	//hzm coop mod chrissstrahl - update statistic for all players
	coop_classUpdateClassStats();

	//[b607] chrissstrahl - clear player name from all other players their communicator transport buttons
	coop_playerCommunicator(player, 0);
}

int Player::coop_updateStatsCoopHealth(int statNum)
{
	int value = 0;
	if (game.coop_isActive && coopPlayer.installed) {
		Entity* target;
		if (statNum == STAT_MP_GENERIC4)
		{
			target = GetTargetedEntity();
			if ((target) && target->isSubclassOf(Player))
			{
				Player* targetPlayer = (Player*)target;
				//[b607] chrissstrahl - fix health showing when targeting a different entity that is not a player
				value = (int)(target->getHealth() + 0.99f);
			}
		}
	}
	return value;
}


//================================================================
// Name:        coop_updateStats
// Class:       -
//              
// Description: handles player stats in coop - executed from player.cpp at Player::UpdateStats( void )
//              
// Parameters:  void
//              
// Returns:     void
//              
// PLEASE SEE ALSO:
// int MultiplayerManager::getStat( Player *player, int statNum )
// void Player::UpdateStats( void )
//================================================================
bool Player::coop_updateStats(void)
{
	if (!game.coop_isActive) {
		return false;
	}

	// Deathmatch stats for arena mode
	if (multiplayerManager.inMultiplayer()) {
		client->ps.stats[STAT_SCORE] = multiplayerManager.getPoints(this);
		client->ps.stats[STAT_KILLS] = multiplayerManager.getKills(this);
		client->ps.stats[STAT_DEATHS] = multiplayerManager.getDeaths(this);

		client->ps.stats[STAT_MP_SPECTATING_ENTNUM] = multiplayerManager.getStat(this, STAT_MP_SPECTATING_ENTNUM);

		edict->s.infoIcon = multiplayerManager.getInfoIcon(this, last_ucmd.buttons);

		if (game.coop_awardsActive) {
			client->ps.stats[STAT_MP_AWARD_ICON] = multiplayerManager.getIcon(this, STAT_MP_AWARD_ICON);
			client->ps.stats[STAT_MP_AWARD_COUNT] = multiplayerManager.getStat(this, STAT_MP_AWARD_COUNT);
		}

		//used for player health on targeting
		client->ps.stats[STAT_MP_GENERIC4] = multiplayerManager.getStat(this, STAT_MP_GENERIC4); //SPECIALTY_MEDIC - health targeted
		/*
		//They have multiple use cases (changed with each modifier), I listed one for each so a general idea can be derived
		client->ps.stats[STAT_MP_GENERIC1] = multiplayerManager.getStat(this, STAT_MP_GENERIC1); //_redDestructionObject
		client->ps.stats[STAT_MP_GENERIC2] = multiplayerManager.getStat(this, STAT_MP_GENERIC2); //_blueDestructionObject
		client->ps.stats[STAT_MP_GENERIC3] = multiplayerManager.getStat(this, STAT_MP_GENERIC3); //_blueBombPlace
		client->ps.stats[STAT_MP_GENERIC5] = multiplayerManager.getStat(this, STAT_MP_GENERIC5); //ctfflag
		client->ps.stats[STAT_MP_GENERIC6] = multiplayerManager.getStat(this, STAT_MP_GENERIC6); //oneFlag
		client->ps.stats[STAT_MP_GENERIC7] = multiplayerManager.getStat(this, STAT_MP_GENERIC7); //bomber get name
		client->ps.stats[STAT_MP_GENERIC8] = multiplayerManager.getStat(this, STAT_MP_GENERIC8); //CONTROL_POINT_GAMMA
		*/

		client->ps.stats[STAT_MP_STATE] = multiplayerManager.getStat(this, STAT_MP_STATE);
	}

	return true;
}

//================================================================
// Name:        DelayedServerCommand
// Class:       -
//              
// Description: This adds a serverCommand to a player's list of delayed commands.
//              
// Parameters:  int entNum, const char *commandText
//              
// Returns:     -
//              
//================================================================
void DelayedServerCommand( int entNum, const char *commandText )
{
	//hzm chrissstrahl - I have a hunch this fixes the current issue that this fuc causes
	if ( entNum < 0 || entNum >( game.maxclients - 1 ) ){
		return;
	}

	pendingServerCommand *command;
	pendingServerCommand *temp;
	gentity_t			 *edict = &g_entities[entNum];
	Player				 *player;
	int					  commandLength;

	if ( !edict || !edict->inuse || !edict->client ){
		return;
	}
		
	player = (Player*)edict->entity;

	if ( !player ){
		return;
	}

	// Get the memory...
	command = (pendingServerCommand*)malloc(sizeof(pendingServerCommand));
	temp	= pendingServerCommandList[entNum];

	if ( command == NULL ){
		gi.Printf("DelayedServerCommand: Couldn't allocate memory for new pendingServerCommand -> Dropping command.\n");
		return;
	}

	//gi.Printf( va( "A command: %s\n" , commandText ) );//debug

	// Prepare the new pendingServerCommand
	commandLength = strlen(commandText);
	commandLength++;//hzm chrissstrahl - fix text being cut off - what the fuck ???
	command->command = (char*)malloc( commandLength * sizeof(char));

	if ( command->command == NULL ){
		gi.Printf("DelayedServerCommand: Couldn't allocate memory for new pendingServerCommandText -> Dropping command.\n");
		free( command );
		return;
	}

	Q_strncpyz( command->command, commandText, commandLength );
	command->next = NULL;

	//gi.Printf( va( "???command->command: %s\n" , command->command ) );//debug

	// Append the new command to the list (or start the list)
	if ( pendingServerCommandList[entNum] == NULL ){
		pendingServerCommandList[entNum] = command;
	}else{
		while ( temp->next )
		{
			temp = temp->next;
		}
		temp->next = command;
	}
}

//================================================================
// Name:        handleDelayedServerCommands
// Class:       -
//              
// Description: This handles the still delayed serverCommands of all players.
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
void handleDelayedServerCommands( void )
{
	int i;
	int j;
	for ( i = 0; i < maxclients->integer; i++ ){
		if ( &g_entities[i] && g_entities[i].client && g_entities[i].inuse ){
			Player *player = (Player *)g_entities[i].entity;

			if ( player ){
				pendingServerCommand *pendingCommand = pendingServerCommandList[i];

				while ( pendingCommand ){
					//[b607] chrissstrahl, testedt value 71, result was that the mission succsess and sublevelLoading hud did stay on at the next level
					//the command ui_removehuds in coop_huds_setupMultiplayerUI did no longer work
					//the client 0 player name was set empty and player was named redshirt 105 WORKED FOR A LONG TIME but we are testing 90 now
					//the lower the number the potentially faster the commands get send, which is good for tricorder puzzles
					if ( gi.GetNumFreeReliableServerCommands( i ) > 90 ){ //chrissstrahl - used to be 105 changed, due to cyceling out cmds, was 32, 32 is to low! 105 might be to high... ...but coop mod used 122 for ages, 128 are usually the max
						//gi.Printf( va( "command: %s\n" , pendingCommand->command ) );

						int foundSpace = 0;
						int startIndex	= 0;
						str sCmd;
						str sNewText	= "";
						str sText		= "";

						//[b607] chrissstrahl - optimize data string by not adding stufftext when not needed
						//[b608] chrissstrahl - popmenu with a menuname does not work purly clientside, it needs stufftext prefix which is why it has been removed from the checks
						foundSpace = coop_returnIntFind(pendingCommand->command, " ");
						if (Q_stricmpn("hudprint ", pendingCommand->command, foundSpace) == 0 ||
							Q_stricmpn("status ", pendingCommand->command, foundSpace) == 0 ||
							Q_stricmpn("score ", pendingCommand->command, foundSpace) == 0 
							)
						{
							sCmd = coop_returnStringUntilWhithspace(pendingCommand->command);
							startIndex = (sCmd.length() + 1);
						}
						else{
							sCmd = "stufftext";
							startIndex = 0;
						}
						sCmd	+= " \"";

						//[b607] chrissstrahl - printout all commands for debugging and optimisatzion purpose
						//gi.Printf(va("handleDelayedServerCommand: %s\n",pendingCommand->command));

						//hzm gameupdate chrissstrahl - cleanup string
						for ( j = startIndex; j < strlen( pendingCommand->command ); j++ ){
							sText += pendingCommand->command[j];
						}

						//hzm coop mod chrissstrahl - phrase coop localstrings, replace with normal chars if player does not have coop
						sNewText = coop_textPhraseLocalStrUmlaute( player, sText );
						//make sure the text is no longer than 287 units or it will crash the game
						if ( sNewText.length() > 287 ) {
							sNewText = coop_substr( sNewText , 0 , 286 );
							gi.Printf( "handleDelayedServerCommands: String to long, was cut down to 286\n" );
							gi.Printf( "%s" , sCmd.c_str() );
						}
						sCmd += sNewText;
						sCmd += "\"\n";

						gi.SendServerCommand( i , sCmd.c_str() );

						// Free the just used command
						pendingServerCommandList[i] = pendingCommand->next;
						free(pendingCommand->command);
						free(pendingCommand);
						pendingCommand = pendingServerCommandList[i];
					}
					else{
						//chrissstrahl - put the info into one print command [b607]
						//gi.Printf( va( "=====================\nhandleDelayedServerCommands freereliable: %i\ncommand delayed: %s\n==========================\n" , gi.GetNumFreeReliableServerCommands( i ) , pendingCommand->command) );
						break;
					}
				}
			}
		}
	}
}

//================================================================
// Name:        clearDelayedServerCommands
// Class:       -
//              
// Description: This clears the still delayed serverCommands of a player.
//              
// Parameters:  int entNum
//              
// Returns:     -
//              
//================================================================
void clearDelayedServerCommands( int entNum )
{
	pendingServerCommand *current;
	pendingServerCommand *temp;

	current = pendingServerCommandList[entNum];

	while ( current != NULL ){
		temp = current;
		current = current->next;
		free( temp->command );
		free( temp );
	}

	pendingServerCommandList[entNum] = NULL;
}