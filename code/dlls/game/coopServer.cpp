//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING SERVER RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2019 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

//>>starts everytime a map is chnaged or a game is closed
//void MultiplayerManager::cleanup


#include "_pch_cpp.h"

#include "coopEvents.hpp"
#include "coopServer.hpp"
#include "coopReturn.hpp"
#include "coopPlayer.hpp"
#include "coopObjectives.hpp"
#include "coopText.hpp"
#include "coopParser.hpp"

#include "player.h"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include "powerups.h"
#include "interpreter.h"
#include "program.h"
#include "trigger.h"
extern Event EV_ScriptThread_StuffCommand;

//================================================================
// Name:        coop_serverInizializeGameVars
// Class:       -
//              
// Description: Inizialized all coop gamevars ONLY ONCE the Game Server is created to prevent accessing empty vars
//				This is NOT executed each time a level is loaded.
//              
// Parameters:  NONE
//              
// Returns:     VOID
//              
//================================================================
void coop_serverInizializeGameVars(void)
{
	game.coop_author = "";
	game.coop_story = "";
	game.coop_story_deu = "";
	game.coop_objectiveLastUpdate = -1.0f;
	game.coop_classInfoSendAt = -1.0f;
	game.coop_isActive = false;
	game.coop_awardsActive = false;
	game.coop_reboot = false; //chrissstrahl - added for forced reboot [b607]
	game.coop_rebootForced = false;
	game.sv_floodprotectActivate = false;
	game.coop_restartServerEarliestAt = 99999999.0f;
	game.coop_friendlyFire = COOP_DEFAULT_FF;
	game.coop_gametype = MAPTYPE_MULTIPLAYER;
	game.coop_maxspeed = COOP_DEFAULT_MAXSPEED;
	game.coop_respawnTime = COOP_DEFAULT_RESPAWNTIME;
	game.coop_lastmanstanding = COOP_DEFAULT_LASTMANSTANDING;
	game.isCoopIncludedLevel = false;
	game.isStandardLevel = false;
	game.levelType = 0;
	game.branchdialog_chosenPlayer = NULL; //[b608] chrissstrahl - used to store player that is valid to select the dialog
	game.branchdialog_selectionActive = false;
	game.branchdialog_startingTime = 0.0f;
	game.branchdialog_name = "";
	game.branchdialog_timeout = 20;
	game.cinematicCurrentCam = NULL;
	game.bForceDynLightUpdate = false;
	game.cinematicSkipping = false;
	game.coop_teamIcon = COOP_DEFAULT_TEAMICON; //[b607] team arrow show 
	game.coop_showEventsTime = 0;//[b607] used to manage g_showevents 
	game.coop_deadBodiesPerArea = 0;//[b607] used to manage actors deadfade
	game.coop_deadBodiesAllNum = 0;//[b607] used to manage actors deadfade
	game.levelRestarted = false;//[b607] chrissstrahl - allow us to detect if the game was restarted  via restart or loaded
	game.coop_stasisTime = 0;//[b607] chrissstrahl - allow us to set stasistime for idryll staff weapon against ai
	game.coop_autoFailPending=false; //[b607] chrissstrahl - if this is true pending missionfailure event will be stoped (auto fail if sv empty)

	//[b610] Chrissstrahl - make sure windows dedicated server uses not the same cfg as the user
	//set CONFIG CVAR
	#ifdef WIN32
	bool bConfigChange = true;
	#else
	bool bConfigChange = false;
	#endif

	//gametype 0=sp 1=mp 2=solo, dedicated 0=listen 1=landedicated 2=internetdedicated
	//[b611] chrissstrahl - fixed crash on accsess to uninizialized cvar
	if (bConfigChange && g_gametype && g_gametype->integer > 0 && dedicated->integer > 0) {
		cvar_t *cvarUser = gi.cvar_get("username");
		str sUser = (cvarUser ? cvarUser->string : "");
		cvar_t *cvarConfig = gi.cvar_get("config");
		str sConfig = (cvarConfig ? cvarConfig->string : "");

		if (sConfig == sUser && sUser != "server"){
			gi.cvar_set("config", "server");
		}
	}
}

//===========================================================[b607]
// Name:        coop_serverConfigstringRemoveNonCoopStrings
// Class:       -
//              
// Description: Removes the configstrings that we don't need, want or shouldn't have in coop
//              
// Parameters:  VOID
//              
// Returns:     VOID
//              
//[b607] chrissstrahl - try to minimize the usage of configstrings due to cl_parsegamestate issue
//================================================================
int coop_serverConfigstringRemoveCombatSounds(str sActorname)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !sActorname.length()) {
		return 0;
	}

	sActorname = sActorname.tolower();
	char	   *s;
	int iNum = 0;
	for (int i = 1; i < MAX_CONFIGSTRINGS; i++) {
		s = gi.getConfigstring(i);
		str ss = "";
		ss += s;

		if (ss.length()) {
			if (coop_returnIntFind(ss,va("/sound/dialog/combat/%s_",sActorname.c_str())) != -1) {
				iNum++;
				//gi.Printf(va("#REMOVED COMBAT SOUND: #%i: %s\n", i, ss.c_str()));
				gi.setConfigstring(i, "");
			}
		}
	}
	return iNum;
}

//===========================================================[b607]
// Name:        coop_serverConfigstringRemoveNonCoopStrings
// Class:       -
//              
// Description: Removes the configstrings that we don't need, want or shouldn't have in coop
//              
// Parameters:  VOID
//              
// Returns:     VOID
//              
//[b607] chrissstrahl - try to minimize the usage of configstrings due to cl_parsegamestate issue
//================================================================
void coop_serverConfigstringRemoveNonCoopStrings()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !game.coop_isActive) {
		return;
	}
	//mp taunts we do not use in coop
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_andor2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_attrexf1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_attrexf2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_attrexm1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_attrexm2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_borgf1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_borgf2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_borgm1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_borgm2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_kleeya1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_kleeya2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_krindo1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_krindo2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_omag1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_omag2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_lurok1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_lurok2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_klingf1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_klingf2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_naus1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_naus2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_picard1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_picard2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_inform1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_inform2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_rene1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_rene2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_rom1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_rom2.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_tuvok1.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/mp_tuvok2.mp3");

	//mp player models we do not allow in coop
	coop_serverConfigstringRemove("models/char/dm_attrexian_command-female.tik");
	coop_serverConfigstringRemove("models/char/dm_attrexian_security-male.tik");
	coop_serverConfigstringRemove("models/char/dm_borg_female.tik");
	coop_serverConfigstringRemove("models/char/dm_borg_male.tik");
	coop_serverConfigstringRemove("models/char/dm_drull_kleeya.tik");
	coop_serverConfigstringRemove("models/char/dm_drull_krindo.tik");
	coop_serverConfigstringRemove("models/char/dm_ferengi_oolpax.tik");
	coop_serverConfigstringRemove("models/char/dm_klingon_merc-boss.tik");
	coop_serverConfigstringRemove("models/char/dm_klingon_merc-female.tik");
	coop_serverConfigstringRemove("models/char/dm_nausicaan_male-merc.tik");
	coop_serverConfigstringRemove("models/char/dm_romulan_informant-boss.tik");
	coop_serverConfigstringRemove("models/char/dm_romulan_rebel-commander.tik");
	coop_serverConfigstringRemove("models/char/dm_romulan_rebel-guard-snow.tik");
	coop_serverConfigstringRemove("models/char/dm_romulan_stx-female.tik");
	coop_serverConfigstringRemove("models/char/dm_stalker.tik");
	coop_serverConfigstringRemove("models/char/dm_tuvok.tik");

	//awards we do not use in coop
	coop_serverConfigstringRemove("sysimg/icons/mp/award_sharpshooter");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_untouchable");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_logistics");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_tactician");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_demolitionist");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_mvp");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_defender");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_warrior");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_carrier");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_interceptor");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_bravery");
	coop_serverConfigstringRemove("sysimg/icons/mp/award_firstStrike");

	//mp computer voice we do not use in coop
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_5mins.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_2mins.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_1min.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_500pointsleft.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_100pointsleft.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_25pointsleft.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_10pointsleft.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_5pointsleft.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_4pointsleft.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_3pointsleft.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_2pointsleft.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_1pointsleft.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_mats.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_tiedfirst.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_winn.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_second.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_third.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_didnotrank.mp3");
	coop_serverConfigstringRemove("localization/sound/dialog/dm/comp_matover.mp3");
}

//===========================================================[b607]
// Name:        coop_serverConfigstringRemove
// Class:       -
//              
// Description: Removes the given string from the configstrings
//              
// Parameters:  STRING to remove
//              
// Returns:     VOID
//              
//[b607] chrissstrahl - try to minimize the usage of configstrings due to cl_parsegamestate issue
//================================================================
int coop_serverConfigstringRemove(str sRem)
{
	if (!sRem.length()) {
		return 0;
	}

	int iRem = 0;
	char	   *s;
	for (int i = 1; i < MAX_CONFIGSTRINGS; i++) {
		s = gi.getConfigstring(i);
		str ss = "";
		ss += s;

		if (ss.length() > 0) {
			//if this is a dialog try to handle german and english localized strings as well
			if (!strnicmp(ss.c_str(),"localization/",13)) {
				//regular dialog
				if (strcmpi(ss.c_str(), sRem.c_str())==0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}

				//handle deu version of dialog
				char unlocal[96]; //MAX_QPATH + 5 <- did not work!
				memset(unlocal, 0, sizeof(unlocal));
				Q_strncpyz(unlocal, va("loc/deu/%s", sRem + 13), sizeof(unlocal));
				if (strcmpi(ss.c_str(), unlocal)==0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}

				//handle eng version of dialog
				memset(unlocal, 0, sizeof(unlocal));
				Q_strncpyz(unlocal, va("loc/eng/%s", sRem + 13), sizeof(unlocal));
				if (strcmpi(ss.c_str(), unlocal)==0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}
			}
			else {
				if (strcmpi(ss.c_str(),sRem.c_str())==0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}
			}
		}
	}
	return iRem;
}

//===========================================================[b607]
// Name:        coop_serverCheckEndMatch
// Class:       -
//              
// Description: Returns 1 or 0 if the match should end or not
//              
// Parameters:  NONE
//              
// Returns:     BOOL
//              
//================================================================
bool coop_serverCheckEndMatch(void) //added [b607]
{
	//hzm gamefix chrissstrahl - prevent crash on or after 23 days on the same map
	//load new map after before we reach 2'000'000 on the same map, have some buffer time to make sure no other setting will delay that over the magical 2
	if (multiplayerManager.getTime() > 172800.0f) {//259200 3days  //172800 2days //1500000.0f  //1999000.0f
		multiplayerManager.centerPrintAllClients("^5Coop:^8 The Server needs to reboot now!\n", CENTERPRINT_IMPORTANCE_CRITICAL);
		gi.Printf("HZM Coop Mod: The Server needs to reboot now! (%f)\n", multiplayerManager.getTime());

		//[b608] chrissstrahl - fix loading a map from mp_maplist - same map is loaded now
		gi.cvar_set("nextmap", level.mapname.c_str());

		//chrissstrahl - make sure we have a reboot and load the same map
		game.coop_rebootForced = true;
		coop_serverManageReboot(level.mapname.c_str());
		return true;
	}
	return false;
}


//================================================================
// Name:        coop_serverLmsCheckFailure
// Class:       -
//              
// Description: Checking if the mission should fail during LMS
//              
// Parameters:  NONE
//              
// Returns:     BOOL
//              
//================================================================
bool coop_serverLmsCheckFailure( void )
{
	if ( !game.coop_isActive ||
		!game.coop_lastmanstanding ||
		level.mission_failed ||
		game.levelType < MAPTYPE_MISSION )
	{
		return false;
	}
	
	Player *player;

	int iAll = 0;
	int iActive = 0;
	int i;

	for ( i = 0; i < maxclients->integer; i++ )
	{
		player = ( Player * )g_entities[i].entity;
		if ( player && player->client && player->isSubclassOf( Player ) )
		{
			iAll++;
			if ( player->coopPlayer.deathTime < game.coop_levelStartTime ) {
				iActive++;
			}
gi.Printf(va("COOPDEBUG coop_serverLmsCheckFailure: %s death[%d] vs startlms[%d]\n", multiplayerManager._playerData[player->entnum]._name.c_str(), player->coopPlayer.deathTime, game.coop_levelStartTime));
		}
	}

gi.Printf("COOPDEBUG coop_serverLmsCheckFailure: all[%i] vs active[%i]\n", iAll, iActive);

	//fail mission if all are dead
	if ( iAll > 0 && iActive == 0 )
	{
		//[b607] needs to be exactly this to trigger playerDeathThread
		//[b610] chrissstrahl - changed string to include $$ to fix issue
		//select a random player suicide kill message range 1 to 10
		G_MissionFailed(va("$$PlayerKilled%i$$", ((int)G_Random(9.0f) + 1)));
		return true;
	}
	return false;
}

//================================================================
// Name:        coop_serverManageReboot
// Class:       -
//              
// Description: checks if the dedicated server requires a reboot or not
//              
// Parameters:  str sMapToLoad
//              
// Returns:     VOID
//              
//================================================================
bool coop_serverManageReboot(str sMapToLoad)
{
	return coop_serverManageReboot( sMapToLoad, NULL);
}

//================================================================
// Name:        coop_serverManageReboot
// Class:       -
//              
// Description: checks if the dedicated server requires a reboot or not
//              
// Parameters:  str sMapToLoad, Player* player
//              
// Returns:     VOID
//              
//================================================================
bool coop_serverManageReboot(str sMapToLoad, Player* player) //[b607] chrisstrahl added player for printing info
{
	//[b607] chrissstrahl - allow to adjust the coop reboot feature
	bool bCoopNoReboot = false;
	str sCoopRebootState = coop_parserIniGet("ini/server.ini", "rebootType", "server");
	if (Q_stricmpn(sCoopRebootState.c_str(),"killserver",10) && Q_stricmpn(sCoopRebootState.c_str(),"quit",4)) { 
		return false;
	}

	//while rebooting do not return false, as this might make the server change the map rather than quitting it
	if ( game.coop_reboot ){
		return true;
	}

	//reset var if the map we are about to load is not starting with ent-deck or ent-training_weap (excluding ent-deck.bsp)
	//if map is ent-deck
	//if map is standardlevel and starts with m
	bool bReset = false;

	if ( strlen( sMapToLoad ) > 4 ) {
		str s = sMapToLoad.tolower();
		//mapname starts with m like m11
		//has m and number followed by a l like m1l
		//has m and number followed by a l like m11l
		if (	!Q_stricmp( "ent-deck" , sMapToLoad )  ||
				s[0] == 'm' && IsNumeric( va("%c",s[1]) ) && s[2] == 'l' ||
				s[0] == 'm' && IsNumeric( va("%c",s[1]) ) && IsNumeric( va( "%c" , s[1] ) ) && s[2] == 'l'
			)
		{
			bReset = true;
		}
	}

	if ( bReset ){
		coop_serverSaveGameVars( "igmRoomsVisited" , "0" );
		coop_serverSaveGameVars( "igmHolodeckSpawn" , "0" );
		coop_serverSaveGameVars( "igmTurboliftSpawn" , "0" );	
	}

	//don't reboot a listen server, don't reboot on level startup
	if ( dedicated->integer == 0 || level.time < 10 ){
		//added info message printed to executing player [b607]
		if (player != NULL && dedicated->integer == 0) {
			player->hudPrint("^5Info^8: Only dedicated Servers are meant to be rebooted.\n");
		}
		return false;
	}

	//check for reboot conditions
	if (!game.coop_rebootForced) { //chrissstrahl - allow forced reboot [b607]
		//if current map is m11l1a or it is about to be loaded we need a reboot 
		if (Q_stricmp("m11l1a-drull_ruins3", sMapToLoad) && Q_stricmp("m11l1a-drull_ruins3", level.mapname)){
			if (iTIKIS < COOP_MAX_SAFE_TIKI_LOAD &&
				iSKAS < COOP_MAX_SAFE_SKA_LOAD &&
				(COOP_MAX_SAFE_SKA_LOAD + COOP_MAX_SAFE_SKA_LOAD) < COOP_MAX_SAFE_COMBINED_LOAD)
			{
				return false;
			}
		}
	}

	//okays, make sure the map we want to load does really exist if it doesn't reload current map.
	//you know paranoya isn't real if it is real ;)
	gi.Printf("REBOOT: SET MAPNAME %s\n", sMapToLoad.c_str());
	if ( gi.FS_Exists( va( "maps/%s.bsp" , sMapToLoad.c_str() ) ) != -1 ){
		coop_parserIniSet( "ini/server.ini" , "startmap" , sMapToLoad.c_str() , "server" );
	}else{
		coop_parserIniSet( "ini/server.ini" , "startmap" , level.mapname.c_str() , "server" );
	}
	//set marker that server rebooted
	coop_parserIniSet( "ini/server.ini" , "rebooting" , "true" , "server" );

	//save client data into coop_status.ini
	coop_serverSaveAllClientData();

	gi.Printf("REBOOT: ISSUING RECONNECT TO CLIENTS\n");
	game.coop_restartServerEarliestAt = level.time + 0.1f;//35

	//set signal to reboot
	game.coop_reboot = true;

	for (int i = 0; i < maxclients->integer; i++ ){
		if ( &g_entities[i] && g_entities[i].client && g_entities[i].inuse ){
			Player *player = ( Player * )g_entities[i].entity;
			if ( player ){
				//keep it in two lines
				gi.SendServerCommand( i , "stufftext \"freeze 2\";stufftext reconnect\n" );
				gi.SendServerCommand( i , "stufftext \"exec coop_mod/cfg/reconnect\"\n" );	
			}
		}
	}
	return true;
}

//================================================================
// Name:        coop_serverMonitorTikiCache
// Class:       -
//              
// Description: counts the models and animations loadad by the game
//				and stores it, for later analythics when the server
//				actually need to be restarted or not
//              
// Parameters:  str sData
//              
// Returns:     VOID
//              
//================================================================
void coop_serverMonitorTikiCache( str sData )
{
	if ( !sData || sData.length() < 5 || sData[0] == '*' || g_gametype->integer == GT_SINGLE_PLAYER /*|| dedicated->integer == 0*/ )return;

	sData = coop_returnStringFilenameOnly( sData );
	sData = sData.tolower();

	int iMax;
	int iType = 99;

	if ( strstr( sData.c_str() , ".tik" ) ){
		iType = 0;
		iMax = COOP_MAX_TIKI_LISTING;
	}else if ( strstr( sData.c_str() , ".ska" ) ){
		iType = 1;
		iMax = COOP_MAX_SKA_LISTING;
	}else if( strstr( sData.c_str() , ".spr" ) ){
		iType = 2;
		iMax = COOP_MAX_SPRITES_LISTING;
	}else{
		return;
	}

	int i;
	str sArray;		

	for ( i = 0; i < iMax; i++ ){
		if ( iType == 0 )		{ sArray = game.coop_serverLoadedTikiListing[i];	}
		else if ( iType == 1 )	{ sArray = game.coop_serverLoadedSkaListing[i];		}
		else if ( iType == 2 )	{ sArray = game.coop_serverLoadedSpritesListing[i];	}
			

		if ( sArray && sArray.length() && sArray == sData ){
			return;
		}

		if ( !sArray || sArray && !Q_stricmp( sArray , "") ){
			if ( iType == 0 )		{ iTIKIS++;		game.coop_serverLoadedTikiListing[i]	= sData; }
			else if ( iType == 1 )	{ iSKAS++;		game.coop_serverLoadedSkaListing[i]		= sData; }
			else if ( iType == 2 )	{ iSPRITES++;	game.coop_serverLoadedSpritesListing[i]	= sData; }

			//debug
			//gi.Printf( va( "======================\n[%i,%i,%i]NEW: %s\n======================\n" , iTIKIS , iSKAS , iSPRITES , sData ) );
			break;
		}
	}
}


//================================================================
// Name:        coop_serverManageClientData
// Class:       -
//              
// Description: decides if the clientdata should be saved based upon mapnames
//              
// Parameters:  str sMap
//              
// Returns:     VOID
//              
//================================================================
void coop_serverManageClientData( str sMap )
{
	if ( !game.coop_isActive )
		return;
	
	//hzm coop mod chrissstrahl - remember player status if this is a sublevel
	bool bCoopKeppPlayerStatus = false;

	if ( game.isStandardLevel && game.levelType == MAPTYPE_MISSION && gi.areSublevels( level.mapname , sMap.c_str() ) == 1 ){
		bCoopKeppPlayerStatus = true;
	}
	//check if coop map is a sequel (determined by name simularities coop_[paradiseIsland]2 - coop_[gbs]7 )
	else if ( !Q_stricmpn( coop_textCleanAllButLettersAndLower( level.mapname ) , coop_textCleanAllButLettersAndLower( sMap.c_str() ) , 64 ) ){
		bCoopKeppPlayerStatus = true;
	}

	if ( bCoopKeppPlayerStatus ){
		coop_serverSaveAllClientData();
	}else{
		coop_serverResetAllClientData();
	}
	game.coop_saveClientData = false;
}


//================================================================
// Name:        coop_serverSaveClientData
// Class:       -
//              
// Description: saves client data so they can be retrived upon level change
//              
// Parameters:  Player *player
//              
// Returns:     VOID
//              
//================================================================
void coop_serverSaveClientData( Player *player )
{
	if ( !game.coop_saveClientData || !game.coop_isActive )
		return;
	if ( !player )
		return;
	if ( !player->isSubclassOf( Player ) )
		return;
	if ( multiplayerManager.isPlayerSpectator( player ) )
		return;
	if ( player->coopPlayer.lastTimeSpawned == -1.0f )
		return;

	//time_t timeOnServer;
	//time( &timeOnServer );

	//write data directly
	coop_serverSaveClientDataWrite( player );
}

//================================================================
// Name:        coop_serverSaveClientDataWrite
// Class:       -
//              
// Description: saves client data so they can be retrived upon level change
//              
// Parameters:  Player *player
//              
// Returns:     VOID
//              
//================================================================
void coop_serverSaveClientDataWrite( Player *player )
{
	if ( !player || g_gametype->integer == GT_SINGLE_PLAYER || multiplayerManager.isPlayerSpectator( player, SPECTATOR_TYPE_ANY ) )
		return;

	str sData = va( "%d %d %d %d %d %d %d" ,
		( int )player->getHealth() ,
		( int )player->GetArmorValue() ,
		( int )player->AmmoCount( "Phaser" ) ,
		( int )player->AmmoCount( "Plasma" ) ,
		( int )player->AmmoCount( "Fed" ) ,
		( int )player->AmmoCount( "Idryll" ),
		( int )player->coopPlayer.deathTime
		);

	//data already saved, or coopId is empty, we have no use for data that belongs to no id
	if ( player->coopPlayer.coopStatus == sData || player->coopPlayer.coopId == "" )
		return;

	player->coopPlayer.coopStatus = sData;
	coop_parserIniSet( "ini/server.ini" , player->coopPlayer.coopId , sData , "client" );
	//gi.Printf( va( "=============================\nSAVED DATA FOR CLIENT: %s\n=============================\n" , sData.c_str() ) );
}

//================================================================
// Name:        coop_serverSaveGameVars
// Class:       -
//              
// Description: saves gamevars used for game progress
//              
// Parameters:  string name, string value, char type
//              
// Returns:     VOID
//              
//================================================================
void coop_serverSaveGameVars( const str &name , const str &value )
{
	if ( !game.coop_isActive )return;

	int iCorrected = -1;
	float fCorrected = -1.0f;


	//keep status of the igm missions up to date
	if ( !Q_stricmp( name , "skill" ) )
	{
		iCorrected = coop_returnIntOrDefaultFromString( value , 1 );
		gi.cvar_set( "skill" , va( "%d" , iCorrected ) );
	}
	else if ( !Q_stricmp( name , "maxSpeed" ) )
	{
		iCorrected = coop_returnIntOrDefaultFromString( value , 300 );
		gameVars.SetVariable( "maxSpeed" , iCorrected );
		world->setPhysicsVar( "maxSpeed" , iCorrected );
	}
	//hzm coop mod chrissstrahl - added teamicon support [b607]
	else if (!Q_stricmp(name, "teamIcon")) {
		iCorrected = coop_returnIntOrDefaultFromString(value, 0);
		game.coop_teamIcon = (bool)iCorrected;
	}


	if ( game.isStandardLevel )
	{
		if ( !Q_stricmp( name , "globalMissionEnterprise" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "globalMissionEnterprise" , coop_returnIntOrDefaultFromString( value , 0 ) );
		}
		else if ( !Q_stricmp( name , "globalTurboliftRide" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "globalTurboliftRide" , iCorrected );//game.
		}
		else if ( !Q_stricmp( name , "globalKleeyaChoice" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "globalKleeyaChoice" , iCorrected );//game.
		}
		else if ( !Q_stricmp( name , "globalTelsiaChoice" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "globalTelsiaChoice" , iCorrected );//game.
		}
		else if ( !Q_stricmp( name , "globalNoneChoice" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "globalNoneChoice" , iCorrected );//game.
		}
		//secret weapons
		else if ( !Q_stricmp( name , "attrexianWeaponFound" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "attrexianWeaponFound" , iCorrected );//game.
		}
		else if ( !Q_stricmp( name , "secretWeapon_CompressionRifle" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "secretWeapon_CompressionRifle" , iCorrected );//game.
		}
		else if ( !Q_stricmp( name , "secretWeapon_IMOD" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "secretWeapon_IMOD" , iCorrected );//game.
		}
		else if ( !Q_stricmp( name , "secretWeapon_IdryllStaff" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "secretWeapon_IdryllStaff" , iCorrected );//game.
		}
		else if ( !Q_stricmp( name , "secretWeapon_RomulanExperimental" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "secretWeapon_RomulanExperimental" , iCorrected );//game.

		}
		else if ( !Q_stricmp( name , "igmRoomsVisited" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "igmRoomsVisited" , iCorrected );//game.
		}
		else if ( !Q_stricmp( name , "igmHolodeckSpawn" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "igmHolodeckSpawn" , iCorrected );//game.
		}
		else if ( !Q_stricmp( name , "igmTurboliftSpawn" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "igmTurboliftSpawn" , iCorrected );//game.
		}
		else if ( !Q_stricmp( name , "statusM5L2CUnlocked" ) )
		{
			iCorrected = coop_returnIntOrDefaultFromString( value , 0 );
			gameVars.SetVariable( "statusM5L2CUnlocked" , iCorrected );//game.
		}
	}
	
	//make sure we use the corrected value if there is one
	if ( iCorrected == -1 ){
		coop_parserIniSet( "ini/server.ini" , name , value , "server" );
	}else{
		coop_parserIniSet( "ini/server.ini" , name , iCorrected , "server" );
	}
}
void coop_serverRestoreGameVars()
{
	if ( !game.coop_isActive ){
		//restore the default respawn time if map is changed to a non coop map
		multiplayerManager.resetRespawnTime();
		return;
	}


	if ( game.isStandardLevel ){
		gameVars.SetVariable( "globalMissionEnterprise" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "globalMissionEnterprise" , "server" ) , 1 ) );//game.
		gameVars.SetVariable( "globalTurboliftRide" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "globalTurboliftRide" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "globalKleeyaChoice" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "globalKleeyaChoice" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "globalTelsiaChoice" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "globalTelsiaChoice" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "globalNoneChoice" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "globalNoneChoice" , "server" ) , 0 ) );//game.

		gameVars.SetVariable( "attrexianWeaponFound" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "attrexianWeaponFound" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "secretWeapon_CompressionRifle" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "secretWeapon_CompressionRifle" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "secretWeapon_IMOD" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "secretWeapon_IMOD" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "secretWeapon_IdryllStaff" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "secretWeapon_IdryllStaff" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "secretWeapon_RomulanExperimental" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "secretWeapon_RomulanExperimental" , "server" ) , 0 ) );//game.

		gameVars.SetVariable( "igmRoomsVisited" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "igmRoomsVisited" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "igmHolodeckSpawn" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "igmHolodeckSpawn" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "igmTurboliftSpawn" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "igmTurboliftSpawn" , "server" ) , 0 ) );//game.

		gameVars.SetVariable( "statusM5L2CUnlocked" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( "ini/server.ini" , "statusM5L2CUnlocked" , "server" ) , 0 ) );//game.
	}

	//[b607] chrissstrahl - now this needed some work over
	int iVal;
	iVal = coop_returnIntWithinOrDefault(atoi(coop_parserIniGet("ini/server.ini", "skill", "server")), 0, 3, COOP_DEFAULT_SKILL);
	if ( skill->integer != iVal)gi.cvar_set( "skill" , va( "%d" , iVal) );

	game.coop_friendlyFire = coop_returnFloatWithinOrDefault(atof(coop_parserIniGet("ini/server.ini", "friendlyfire", "server")),0 ,2, COOP_DEFAULT_FF);
	gameVars.SetVariable("friendlyFire", game.coop_friendlyFire);

	game.coop_maxspeed = coop_returnIntWithinOrDefault(atoi(coop_parserIniGet("ini/server.ini", "maxspeed", "server")), 200,1000 , COOP_DEFAULT_MAXSPEED);
	world->setPhysicsVar("maxspeed", game.coop_maxspeed);

	game.coop_respawnTime = coop_returnIntWithinOrDefault(atoi(coop_parserIniGet("ini/server.ini", "respawntime", "server")),0 ,60,COOP_DEFAULT_RESPAWNTIME );
	multiplayerManager.setRespawnTime( (float)game.coop_respawnTime );

	iVal = coop_returnIntWithinOrDefault(atoi(coop_parserIniGet("ini/server.ini", "airaccelerate", "server")), 0, 4, COOP_DEFAULT_AIRACCELERATE);
	world->setPhysicsVar("airAccelerate", iVal);

	game.coop_awardsActive = coop_returnIntWithinOrDefault(atoi(coop_parserIniGet("ini/server.ini", "awards", "server")), 0, 1, COOP_DEFAULT_AWARDS);
	game.coop_teamIcon = (bool)coop_returnIntWithinOrDefault(atoi(coop_parserIniGet("ini/server.ini", "teamicon", "server")), 0,1 ,(int)COOP_DEFAULT_TEAMICON );
	game.coop_lastmanstanding = coop_returnIntWithinOrDefault(atoi(coop_parserIniGet("ini/server.ini", "lastmanstanding", "server")), 0,1 ,(int)COOP_DEFAULT_LASTMANSTANDING );
	game.coop_deadBodiesPerArea = coop_returnIntWithinOrDefault(atoi(coop_parserIniGet("ini/server.ini", "deadbodies", "server")),0,25 , COOP_DEFAULT_DEADBODIES);
	game.coop_stasisTime = coop_returnIntWithinOrDefault(atoi(coop_parserIniGet("ini/server.ini", "stasistime", "server")),7,30 , COOP_DEFAULT_STASISTIME);
}


//================================================================
// Name:        coop_serverSaveAllClientData
// Class:       -
//              
// Description: saves client data for all clients
//              
// Parameters:  VOID
//              
// Returns:     VOID
//              
//================================================================
void coop_serverSaveAllClientData( void )
{
	//hzm chrissstrahl - prevent crash, happening if host player quits listen server
	if ( g_entities == NULL ) {
		return;
	}
	//end of hzm

	int	i;
	Player	*player = NULL;
	for ( i = 0; i < maxclients->integer; i++ ){
		player = ( Player * )g_entities[i].entity;
		if ( player && player->isSubclassOf( Player ) ){
			coop_serverSaveClientData(player);
		}
	}
}

//================================================================
// Name:        coop_serverResetClientData
// Class:       -
//              
// Description: resets client data
//              
// Parameters:  Player *player
//              
// Returns:     VOID
//              
//================================================================
void coop_serverResetClientData( Player *player )
{
	if ( !player || !player->isSubclassOf( Player ) )
		return;

	//time_t timeOnServer;
	//time( &timeOnServer );
	str sData = va("0 0 0 0 0 0");

	//hzm coop mod chrissstrahl - don't save data if already saved
	if ( player->coopPlayer.coopStatus == sData )
		return;

	player->coopPlayer.coopStatus = sData;

	coop_parserIniSet( "ini/server.ini" , player->coopPlayer.coopId , sData , "client" );
}


//================================================================
// Name:        coop_serverResetAllClientData
// Class:       -
//              
// Description: saves client data for all clients
//              
// Parameters:  VOID
//              
// Returns:     VOID
//              
//================================================================
void coop_serverResetAllClientData( void )
{
	int		i;
	Player	*player = NULL;
	for ( i = 0; i < maxclients->integer; i++ ){
		player = ( Player * )g_entities[i].entity;
		if ( player && player->isSubclassOf( Player ) ){
			coop_serverResetClientData( player );
		}
	}
}

//================================================================
// Name:        coop_serverCoop
// Class:       -
//              
// Description: Checks if current map is a coop map, if so it will start the coop mod and call all the setup functions
//              
// Parameters:  VOID
//              
// Returns:     VOID
//              
//================================================================
void coop_serverCoop()
{
//[b610] chrissstrahl - moved here from main.scr
//this removes the level end triggers from mission maps, so they can't be accidentially triggered by a player in coop
	if (game.isStandardLevel) {
		str classname = "TriggerChangeLevel";
		gentity_t * from;
		Entity *ent;

		for (from = &g_entities[game.maxclients]; from < &g_entities[globals.num_entities]; from++){
			if (!from->inuse)
			{
				continue;
			}

			assert(from->entity);

			ent = from->entity;

			if (ent->inheritsFrom(classname.c_str()))
			{
				ent->PostEvent(Event(EV_Remove), 0.0f);
			}
		}
	}

//hzm coop mod chrissstrahl - decide based upon the mapname and gametype if we are going in to coop mod modus
	int i;
	str currentMapName = level.mapname.tolower();

	int *objectiveItemState = game.coop_objectiveItemState;
	for ( i = 0; i<8; i++ ){
		*objectiveItemState = -1;
		objectiveItemState++;
	}

	float *objectiveItemCompleted = game.coop_objectiveItemCompletedAt;
	for ( i = 0; i<8; i++ ){
		*objectiveItemCompleted = -1.0f;
		objectiveItemCompleted++;
	}

	//hzm coop mod chrissstrahl - let the level scripts know that this server has the coop mod
	levelVars.SetVariable("isCoopServer", 1.0f);

	//dectect if coop mod should be active
	//[b607] chrissstrahl - added check if coop mod shoudl be disabled
	if ( game.levelType > MAPTYPE_MULTIPLAYER ) {
		//[b607] chrissstrahl - THIS creates a error in str.h at inline const char *str::c_str( void ) const
		//I was unable to resolve this issue, so I use a workaround
		//bool bb = coop_returnBool(coop_phraserIniGet("ini/server.ini", "disableCoop", "server"));
		
		
		//str s = coop_phraserIniGet("ini/server.ini", "disableCoop", "server");
		//bool b = coop_returnBool(s);
		if (!false) { //b
			game.coop_isActive = true;
			//[b607] chrissstrahl - let the level scripts know that this is coop
			levelVars.SetVariable("isCoopActive", 1.0f);
		}
		else {
			//[b607] chrissstrahl - let the level scripts know that this is not coop
			levelVars.SetVariable("isCoopActive", 0.0f);
			gi.Printf(va("HZM Coop Mod is disabled. Set disableCoop=false in coop_status.ini.\n"));
		}
	}

	//hzm coop mod chrissstrahl - print out some info to the console
	gi.Printf( va( "==== HaZardModding Coop Mod gamex86.dll -%i-====\n" , COOP_BUILD ) );
	if ( game.isCoopIncludedLevel  ) {
		gi.Printf( va( "==== %s is included in this coop build! ====\n" , currentMapName.c_str() ) );
	}

	//hzm coop mod chrissstrahl - NO MORE - Gallifrey Falls No More
	if ( g_gametype->integer != GT_MULTIPLAYER ) {
		return;
	}

	//hzm coop mod chrissstrahl - check if ssv_floodprotect should be enabled
	if ( !game.coop_isActive ) {
		int iSvFlood = atoi( coop_parserIniGet( "ini/server.ini" , "sv_floodprotect" , "server" ) );
		if ( iSvFlood ) {
			gi.SendConsoleCommand( "set sv_floodprotect 1\n" );
		}

		//hzm coop mod chrissstrahl - disable 3rd person aim
		gi.cvar_set( "g_aimviewangles" , "0" );

		levelVars.SetVariable( "isCoopLevel" , 0.0f );
		return;
	}
	
	game.coop_saveClientData = true;

	//hzm coop mod chrissstrahl - dissable native spam protection, but remember its state
	coop_parserIniSet( "ini/server.ini" , "sv_floodprotect" , va("%d",sv_floodprotect->integer) , "server" );
	gi.SendConsoleCommand( "set sv_floodprotect 0\n" );

	//hzm coop mod chrissstrahl - enable 3rd person aim
	gi.cvar_set( "g_aimviewangles" , "1" );

	//hzm coop mod chrissstrahl - let the level scripts know that this map is handles as coop level
	levelVars.SetVariable( "isCoopLevel" , 1.0f );

	//hzm coop mod chrissstrahl - get and set status for the information gathering missions
	coop_serverRestoreGameVars();

	//hzm coop mod chrissstrahl - run server corrections and setup
	coop_serverSetup();
}

//================================================================
// Name:        coop_server
// Class:       -
//              
// Description: Configures the server to suit the coop mod needs
//              
// Parameters:  VOID
//              
// Returns:     VOID
//              
//================================================================
void coop_serverSetup( void )
{
	static int SetupExecuted = 0;
	//gi.Printf( va("ARRE SUBLEVELS:  %i\n" , gi.areSublevels( level.mapname , level.nextmap ) ) );

	//hzm coop mod chrissstrahl - register this server to the alternative master server
	str cvarNum;
	int i;
	int iEmptySlot = 0;
	bool bMasterSet = false;
	str cvarValueString;
	for ( i = 1; i < 7; i++ ){
		cvarValueString = coop_returnCvarString( va( "sv_master%i" , i ) );
		if ( strlen( va( "sv_master%d" , i ) ) > 0 ){
			if ( !stricmp( cvarValueString , "ef2master.sfu-server.de" ) ){
				bMasterSet = true;
				break;
			}
		}else if ( iEmptySlot == 0 ){
			iEmptySlot = i;
		}
	}
	if ( !bMasterSet ){
		if ( iEmptySlot == 0){
			iEmptySlot = 7;
		}
		gi.SendConsoleCommand( va( "set sv_master%d ef2master.sfu-server.de\n" , iEmptySlot ) );
	}

	//hzm coop mod chrissstrahl - notify masters that this server is up and running
	gi.SendConsoleCommand( "heartbeat\n" );//notify always

	//hzm coop mod chrissstrahl - coop specific setup
	if ( !game.coop_isActive )
		return;

	if ( coop_returnCvarInteger("sv_timeout") < 60 )//give clients at least 60 sec connection time
		gi.SendConsoleCommand( "set sv_timeout 60\n" );
	if ( coop_returnCvarInteger( "sv_jumpvelocity" ) < 335 )//some missions need this value to be corrected
		gi.SendConsoleCommand( "set sv_jumpvelocity 335\n" );

	//hzm coop mod chrissstrahl - check sttings that require the server to reload the map
	bool correctingTheseSettingsRequiresMapload = false;

	//if new version or first time running coop mod
	i = coop_returnCvarInteger( "coop_ver" );
	if ( i < COOP_BUILD ){
		gi.Printf( va( "COOP MOD - STARTED SETUP - CFG[%i] CODE[%i]\n" , i , COOP_BUILD ) );

		//hzm coop mod chrissstrahl - check if the file exists, server would go into a map loading loop otherwise
		if ( !gi.FS_Exists( "coop_mod/cfg/server/setup.cfg" ) ){
			gi.Printf( "^1===========================================================\n" );
			gi.Printf( "^1===========================================================\n" );
			gi.Printf( "--- SETUP ERROR ----\n" );
			gi.Printf( "coop_mod/cfg/server/setup.cfg $$NotFoundOnServer$$ !!!\n" );
			gi.Printf( "Could not configure server for Coop Mod!\n" );
			gi.Printf( "Please report this error to HaZardModding\n" );
			gi.Printf( "--------------------\n" );
			gi.Printf( "^1===========================================================\n" );
			gi.Printf( "^1===========================================================\n" );
			gi.SendConsoleCommand( "killserver\n" );
			return;
		}

		SetupExecuted++;
		if ( SetupExecuted > COOP_MAX_SETUP_TRIES ){
			gi.Printf( "^1===========================================================\n" );
			gi.Printf( "^1===========================================================\n" );
			gi.Printf( "--------------------\n" );
			gi.Printf( "--- SETUP ERROR ----\n" );
			gi.Printf( "Server was stuck in a loop executing setup.cfg\n" );
			gi.Printf( "Could not configure server for Coop Mod!\n" );
			gi.Printf( "Please report this error to HaZardModding\n" );
			gi.Printf( "--------------------\n" );
			gi.Printf( "--------------------\n" );
			gi.Printf( "^1===========================================================\n" );
			gi.Printf( "^1===========================================================\n" );
			gi.SendConsoleCommand( "killserver\n" );
			return;
		}

		//hzm coop mod chrissstrahl - everything okay, 
		gi.SendConsoleCommand( va("seta coop_ver %d\n",COOP_BUILD) );//more reliable than putting it into the cfg
		gi.SendConsoleCommand( "exec coop_mod/cfg/server/setup.cfg\n" );
		correctingTheseSettingsRequiresMapload = true;
	}

	if ( coop_returnCvarInteger( "sv_pure" ) == 1 )//allow all clients, even if the basefolder contents missmatch with server
	{
		gi.cvar_set("sv_pure", "0");
		gi.Printf( "COOP BAD Setting: sv_pure 1 changing to 0\n" );
		correctingTheseSettingsRequiresMapload = true;
	}

	if ( correctingTheseSettingsRequiresMapload == true )
	{
		gi.Printf( "=============================================\n" );
		gi.Printf( "coop_serverSetup() requested a map reload\n" );
		gi.Printf( "=============================================\n" );

		str sMapToLoad = level.mapname.tolower();

		//hzm coop mod chrissstrahl - load last coop map if there is any
		bool bServerHasRebooted = false;
		bServerHasRebooted = coop_parserIniGet( "ini/server.ini" , "rebooting" , "server" );

		//hzm coop mod chrissstrahl - load the map that we are actually suppose to load
		if ( bServerHasRebooted ){
			coop_parserIniSet( "ini/server.ini" , "rebooting" , "false" , "server" );
			str sStartMap = coop_parserIniGet( "ini/server.ini" , "startmap" , "server" );

			if ( gi.FS_Exists( va( "maps/%s.bsp" , sStartMap.c_str() ) ) ){
				if ( Q_stricmp( sMapToLoad , sStartMap) ){
					sMapToLoad = sStartMap;
				}
			}
		}

	//make sure we load a map with dev map that have been loaded with devmap
	//when we automatically correct the sv_maxclients or anything else
		if ( sv_cheats->integer == 1 ){
#ifdef __linux__
			gi.SendConsoleCommand( va( "map %s \n" , sMapToLoad.c_str() ) );//load map now
#else
			gi.SendConsoleCommand( va( "devmap %s \n" , sMapToLoad.c_str() ) );//load map now
#endif	
		}
		else{
			gi.SendConsoleCommand( va( "map %s \n" , sMapToLoad.c_str() ) );//load map now
		}
	}
	else{		
		//reset error counter
		SetupExecuted=0;
	}
}

//================================================================
// Name:        coop_serverRunScriptThread
// Class:       -
//              
// Description: runs a thread/function from the levelscripts if it exists
//              
// Parameters:  str scriptThread
//              
// Returns:     bool
//              
//================================================================
bool coop_serverRunScriptThread( str scriptThread )
{
	if ( strlen( scriptThread ) > 0)
	{
		CThread *gamescript;
		gamescript = Director.CreateThread( scriptThread );

		//hzm coop mod chrissstrahl - this migth not work with included script files
		//if ( gamescript->labelExists( "precache" ) )

		//hzm coop mod chrissstrahl - make sure the thread exist and could be started
		if ( gamescript ){
			gamescript->Start();
			return true;
		}
	}
	return false;
}

//================================================================
// Name:        coop_serverManageAi
// Class:       -
//              
// Description: checks if the level ai should be disabled or enabled, depending on clients state
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
void coop_serverManageAi( void )
{
	if ( game.coop_isActive ) {
		if ( mp_warmUpTime->integer > level.time )
			return;

		if ( coop_returnPlayerQuantityInArena() > 0 ) {
			level.ai_on = true;
		}
		else {
			level.ai_on = false;
		}
	}
}

//================================================================
// Name:        coop_serverModifiedFile
// Class:       -
//              
// Description: tries to load script and sound from base/coop_mod/maps/ folder
//				if there is no script or sound file it tries to load from
//				base/maps/
//              
// Parameters:  void
//              
// Returns:     str
//              
//================================================================
str coop_serverModifiedFile( str standardPath )
{
	//hzm coop mod chrissstrahl - we need to fid out if we are dealing with script or any other file
	int i;
	bool isScriptFile = false;
	bool extensionStarted = false;
	str bareFilename = "";
	str filename = "";

	//hzm coop mod chrissstrahl - try to read file extension
	str fileExtension;
	for ( i = 0; i < standardPath.length(); i++ )
	{
		if ( standardPath[i] == '/' ){
			//subfolder detected, reset filename
			filename = "";
			bareFilename = "";
		}
		else
		{
			//get filename
			filename += standardPath[i];

			//check for fileextension
			if ( standardPath[i] == '.' ){
				extensionStarted = true;
			}
			//get extension
			if ( extensionStarted ){
				fileExtension += standardPath[i];
			}
			//get naked filename, without extension
			else{
				bareFilename += standardPath[i];
			}
		}
	}

	//hzm coop mod chrissstrahl - if there is no fileextension give us a error
	if ( fileExtension.length() == standardPath.length() )
	{
		gi.Printf( "----------------------------\n" );
		gi.Printf( "---- Coop Mod: ERROR! ----\n" );
		gi.Printf( "File has no extension: %s" , standardPath.c_str() );
		gi.Printf( "----------------------------\n" );
		return standardPath;
	}

	//hzm coop mod chrissstrahl - we handle alternative script file extension, so we need to know
	if ( !Q_stricmpn( fileExtension , ".scr",4) )
	{
		isScriptFile = true;
	}

	//hzm coop mod chrissstrahl - if standard level and we are in singleplayer do load default script or sound files first
	if ( standardPath.length() < 4 || g_gametype->integer == GT_SINGLE_PLAYER && game.isStandardLevel == true ){

		//hzm coop mod chrissstrahl - check if there is a file in base/maps/ (.scr or .snd)
		if ( gi.FS_ReadFile( standardPath.c_str() , NULL , true ) != -1 ){
			return standardPath;
		}
		if ( isScriptFile ){
			//hzm coop mod chrissstrahl - check if there is a file in base/maps/ (.script)
			if ( gi.FS_ReadFile( va( "%sipt" ,standardPath.c_str()) , NULL , true ) != -1 ){
				return va( "%sipt" , standardPath.c_str() );
			}
		}

		//hzm coop mod chrissstrahl - coop version failed, return standard, but give a warning
		if ( isScriptFile ){
			gi.Printf( "---- Coop Mod: WARNING! ----\n" );
			gi.Printf( "Could not find: %s\n" , standardPath.c_str() );
			gi.Printf( "Trying to load coop version: %s\n" , va( "coop_mod/%s" , standardPath.c_str() ) );
			gi.Printf( "----  ----\n" );
			gi.Printf( "You should create a seperate file-version for Singleplayer in: %s\n" , standardPath.c_str() );
			gi.Printf( "----------------------------\n" );
		}

		//hzm coop mod chrissstrahl - no file fond, now try the coop directory
		str newPath = "coop_mod/";
		newPath += standardPath;
		if ( gi.FS_ReadFile( newPath.c_str() , NULL , true ) != -1 ){
			return newPath;
		}
		//hzm coop mod chrissstrahl - if script file check for alternative file extension
		if ( isScriptFile ){
			//hzm coop mod chrissstrahl - check if there is a file in base/maps/ (.script)
			if ( gi.FS_ReadFile( va( "%sipt" , newPath.c_str() ) , NULL , true ) != -1 ){
				return va( "%sipt" , newPath.c_str() );
			}
		}
		return standardPath;
	}

//MULTIPLAYER
//MULTIPLAYER
//MULTIPLAYER
	
	//hzm coop mod chrissstrahl - we are in multiplayer
	//hzm coop mod chrissstrahl - we want allwaways first to check the coop folders

	if ( Q_stricmpn( "coop_" , filename , 5 ) && game.isStandardLevel == false ){
		gi.Printf( "---- Coop Mod: WARNING! ----\n" );
		gi.Printf( "Coop maps must start with 'coop_' in their filename to be supported correctly\n" );
		gi.Printf( "For Help please contact Chrissstrahl\n" );
		gi.Printf( "----------------------------\n" );
	}

	//hzm coop mod chrissstrahl - try coop directory first
	str newPath = "coop_mod/";
	newPath += standardPath;
	if ( gi.FS_ReadFile( newPath.c_str() , NULL , true ) != -1 ){
		return newPath;
	}
	//hzm coop mod chrissstrahl - if no file found and it is a script file check for alternative file extension
	if ( isScriptFile ){
		//hzm coop mod chrissstrahl - check if there is a file in base/coop_mod/maps/ (.script)
		if ( gi.FS_ReadFile( va( "%sipt" , newPath.c_str() ) , NULL , true ) != -1 ){
			return va( "%sipt" , newPath.c_str() );
		}
	}

	if ( isScriptFile ){
		//hzm coop mod chrissstrahl - coop version failed, return standard, but give a warning
		gi.Printf( "---- Coop Mod: WARNING! ----\n" );
		gi.Printf( "Could not find coop version: %s\n" , va( "coop_mod/%s" , standardPath.c_str() ) );
		gi.Printf( "Trying to load: %s\n" , standardPath.c_str() );
		gi.Printf( "----------------------------\n" );
	}

	//hzm coop mod chrissstrahl - no file found, check if there is a file in base/maps/ (.scr or .snd)
	if ( gi.FS_ReadFile( standardPath.c_str() , NULL , true ) != -1 ){
		return standardPath;
	}
	if ( isScriptFile ){
		str sTemp;
		//hzm coop mod chrissstrahl - check if there is a file in base/maps/ (.script)
		sTemp = standardPath.c_str();
		sTemp += "ipt";
		 
		if ( gi.FS_ReadFile( sTemp.c_str() , NULL , true ) != -1 ){
			return sTemp;
		}
	}
	return standardPath;
}


//================================================================
// Name:        coop_serverError
// Class:       -
//              
// Description: handels serverside stuff
//				quit complete game, allow reboot if server is dedicated              
//
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool coop_serverError( str sError, bool bFatal )
{
	if ( g_gametype->integer != 1 || dedicated->integer < 1 )
		return false;

	str sLevel = "";

	//Make note into ini, disable reboot
	coop_parserIniSet( "ini/server.ini" , "rebooting" , "false" , "server" );
	coop_parserIniSet( "ini/server.ini" , "errorboot" , "true" , "server" );
	coop_parserIniSet( "ini/server.ini" , "errorfatal" , coop_returnBoolWord( bFatal ) , "server" );
	coop_parserIniSet( "ini/server.ini" , "errormap" , level.mapname , "server" );
	coop_parserIniSet( "ini/server.ini" , "errortext" , sError , "server" );

	//[b607] chrissstrahl - this prevents the server from rebooting if errorhalt=true in coop_status.ini
	//only the commentary was added, the code was already in place
	if ( coop_returnBool( coop_parserIniGet( "ini/server.ini" , "errorhalt" , "server" ) ) )
		return true;

	//print info to console/logfile
	gi.Printf( "====================================\n" );
	gi.Printf( "GAME WAS QUIT TO ALLOW SERVER REBOOT\n" );
	gi.Printf( "HZM COOP MOD FEATURE================\n" );
	gi.Printf( va("%s\n", sError.c_str()) );
	gi.Printf( "====================================\n" );
	gi.SendConsoleCommand( "quit\n" );
	return true;
}

//================================================================
// Name:        coop_serverThink
// Class:       -
//              
// Description: handels serverside stuff each frame (called by worldspawn)
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
void coop_serverThink( void )
{
	if ( game.coop_isActive ){
		//hzm coop mod chrissstrahl - quit server if requested
		if ( game.coop_reboot && game.coop_restartServerEarliestAt < level.time ){ 
			//[b607] chrissstrahl - allow to adjust the coop reboot feature
			bool bCoopNoReboot = false;
			str sCoopRebootState = coop_parserIniGet("ini/server.ini", "rebootType", "server");
			if (!Q_stricmpn(sCoopRebootState.c_str(), "killserver",10)) { //killserver
				gi.SendConsoleCommand("exec coop_mod/cfg/server/killserver.cfg\n");
			}
			else if(!Q_stricmpn(sCoopRebootState.c_str(), "quit",4)){
				gi.SendConsoleCommand("quit\n");
			}
			else{
				return;
			}
			//chrissstrahl - we need to clear the variable now
			//because with kill server the game vars stay intact
			game.coop_reboot = false;

			//static float fTimeDetected = level.time + 0.045f;
			//if ( fTimeDetected < level.time ){
			//gi.SendConsoleCommand( "quit\n" ); //disabled [b607]
			return;
			//}
		}

		//chrissstrahl - handle g_showevents, disable after some time [b607]
		if (game.coop_showEventsTime != 0) {
			game.coop_showEventsTime++;
			if (game.coop_showEventsTime == 3) {
				game.coop_showEventsTime=301; //do not allow activation again until server has been completley(quit) restarted
				gi.cvar_set("g_showevents","0");
				gi.Printf("HZM COOP MOD - Disabled g_showevents again.\n");
			}
		}

		//hzm coop mod chrissstrahl - this will manage the objective marker
		coop_objectivesMarkerUpdate();
	}
}

//[b610] chrissstrahl - executed from Level::CleanUp
//================================================================
// Name:        coop_serverCleanup
// Class:       -
//              
// Description: Cleans up vars and other stuff once the level is cleaned up
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
void coop_serverCleanup(void)
{
	game.coop_author = "";
	game.coop_story = "";
	game.coop_story_deu = "";
}