//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// CONTAINING SERVER RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

//>>starts everytime a map is changed or a game is closed
//void MultiplayerManager::cleanup


#include "_pch_cpp.h"
#include "player.h"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include "powerups.h"
#include "interpreter.h"
#include "program.h"
#include "trigger.h"

#include "upgPlayer.hpp"
#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"
#include "upgStrings.hpp"
#include "upgCoopInterface.hpp"
extern UpgCircleMenu upgCircleMenu;

#include "coopEvents.hpp"
#include "coopServer.hpp"
CoopServer coopServer;
#include "coopScripting.hpp"
extern CoopScripting coopScripting;
#include "coopReturn.hpp"
#include "coopPlayer.hpp"
#include "coopObjectives.hpp"
#include "coopText.hpp"
#include "coopParser.hpp"
#include "coopNpcTeam.hpp"//[b60011] chrissstrahl
extern CoopNpcTeam coopNpcTeam;
#include "coopChallenges.hpp"
extern CoopChallenges coopChallenges;

extern Event EV_ScriptThread_StuffCommand;

//[b60011] chrissstrahl
#define COOP_SERVER_PHYSICS_BUG_MAX_FPS 80
#define COOP_SERVER_PHYSICS_BUG_MAX_FPS_MESSAGE "Server: Your com_maxFps has been set to 80 to fix a game bug on this level.\n"
#define COOP_SERVER_MASTERSERVER "master.hazardmodding.com"

//========================================================[b60014]
// Name:        coopIsActive
// Class:       CoopServer
//              
// Description: used to check if game is currently in coop
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool CoopServer::coopIsActive()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return false;
	}
	return game.coop_isActive;
}

//========================================================[b60014]
// Name:        getServerDataIniFilename
// Class:       -
//              
// Description: Used to return the name for this servers serverData.ini file
//              
// Parameters:  void
//              
// Returns:     str
//              
// Called from:	Multiple functions, Files and locations.
//================================================================
str CoopServer::getServerDataIniFilename()
{
	//return "serverData_test_test.ini";
	static str iniFilename = "";

	if (!iniFilename.length()) {
		//changed, because we had issues with linux servers
		cvar_t* cvar1 = gi.cvar_get("username");
		cvar_t* cvar2 = gi.cvar_get("config");
		str sUsername = (cvar1 ? cvar1->string : "");
		str sConfig = (cvar2 ? coop_returnStringPathFileNoExtension(cvar2->string) : "");
		iniFilename = va("serverData_%s_%s.ini", sUsername.c_str(), sConfig.c_str());
	}
	return iniFilename;
}

//========================================================[b60014]
// Name:        svFloodProtectDisable
// Class:       -
//              
// Description: Disables sv_floodprotect to receive language/id/version commands
//              
// Parameters:  void
//              
// Returns:     void
//              
// Called from:	coop_playerSetup
//================================================================
void CoopServer::svFloodProtectDisable()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		return;
	}

	//[b60014] chrissstrahl - allow sv_floodprotect toggeling to receive language/id/version
	game.coop_floodprotectToggle = true;
	
	cvar_t* cvar = gi.cvar_get("sv_floodprotect");
	if ((cvar ? cvar->integer : 0.0f) == 1) {
		gi.SendConsoleCommand("set sv_floodprotect 0\n");
		coop_parserIniSet(coopServer.getServerDataIniFilename(), "sv_floodprotect", "1", "server");
	}
}

//========================================================[b60014]
// Name:        svFloodProtectEnable
// Class:       -
//              
// Description: Enables sv_floodprotect again when setup for all players is done
//              
// Parameters:  void
//              
// Returns:     void
//              
// Called from:	coop_serverThink
//================================================================
void CoopServer::svFloodProtectEnable()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !game.coop_floodprotectToggle) {
		return;
	}

	if (!game.coop_isActive) {
		//[b60014] chrissstrahl - regular multiplayer - we have to toggle floodprotect to receive some commands on player join
		if (multiplayerManager.inMultiplayer() && game.coop_floodprotectToggle) {
			Player* player = NULL;
			int iPlayerTotal = 0;
			int iPlayerDone = 0;
			for (int i = 0; i < maxclients->integer; i++) {
				player = (Player*)g_entities[i].entity;
				if (player) {
					iPlayerTotal++;
					if (player->coop_playerSetupComplete()) {
						iPlayerDone++;
					}
				}
			}

			//all players are done reenable sv_floodprotect
			if (iPlayerDone == iPlayerTotal) {
				game.coop_floodprotectToggle = false;
				if (coop_parserIniGet(coopServer.getServerDataIniFilename(), "sv_floodprotect", "server") == "1") {
					gi.SendConsoleCommand("set sv_floodprotect 1\n");
				}
			}
		}
	}
}




//================================================================
// Name:        maploadEnforce
// Class:       -
//              
// Description: checks if the seerver should bee loading a different map than it currently is, forces mapload
//              
// Parameters:  str sMapToLoad
//              
// Returns:     void
//              
// Called from:	extern "C" void G_InitGame( int startTime, int randomSeed )
//================================================================
void CoopServer::mapLoadEnforce()
{
	//[b60012][cleanup] chrissstrahl - pretty sure this could be done better
	// 
	//[b607] chrissstrahl - moved code down here used to be after CVAR_Init(); 
	//but it will not load the map in linux since we use the killserver method
	//instead of the actual quit, but creating a error, so maybe this fixes it
	//ERROR READS:
	// Error in `./linuxef2ded': malloc(): smallbin double linked list corrupted: 0x0956cde0 
	// Aborted
	//
	//hzm coop mod chrissstrahl - restore map to continue coop on
	cvar_t* temp_dedicated;
	cvar_t* temp_gametype;
	temp_dedicated = gi.cvar_get("dedicated");
	temp_gametype = gi.cvar_get("g_gametype");


	//hzm coop mod chrissstrahl - load error restore map
	str sStartMap = "blackbox";
	if (coop_returnBool(coop_parserIniGet(coopServer.getServerDataIniFilename(), "errorboot", "server")))
	{
		str sErrorMap = coop_parserIniGet(coopServer.getServerDataIniFilename(), "errormap", "server");

		//if not fatal, we can load the map again that did have a error before
		//because it is fairly save to assume the error does not put the server in a reboot loop
		if (!coop_returnBool(coop_parserIniGet(coopServer.getServerDataIniFilename(), "errorfatal", "server"))) {
			sStartMap = sErrorMap;
		}

		str sErrorMsg = coop_parserIniGet(coopServer.getServerDataIniFilename(), "errortext", "server");

//coop_parserIniSet(coopServer.getServerDataIniFilename(), "rebooting", "false", "server"); //[b60012][cleanup]

		//if map to load is not the map that it wants to load, force coop map
		if (Q_stricmpn(sStartMap.c_str(), level.mapname.c_str(), MAX_QPATH) != 0) { //[b60012] chrissstrahl - fix missing .c_str()
			gi.Printf(va("COOP MOD - MAP WITH ERROR FOUND:\n%s\n", sErrorMap.c_str()));
			gi.Printf(va("ERROR MESSAGE READS:\n%s\n", sErrorMsg.c_str()));
			//load the error map or if fatal, load blackbox map, which has no scripts and no ai
			gi.SendConsoleCommand(va("map %s\n", sStartMap.c_str()));
		}
	}
	else {
		//hzm coop mod chrissstrahl - load last coop map if there is any
		bool bServerHasRebooted = coop_returnBool(coop_parserIniGet(coopServer.getServerDataIniFilename(), "rebooting", "server"));

		if (temp_gametype->integer > 0 &&
			temp_dedicated->integer > 0 &&
			temp_dedicated->integer < 3 &&
			bServerHasRebooted == true)
		{
			sStartMap = coop_parserIniGet(coopServer.getServerDataIniFilename(), "startmap", "server");

//coop_parserIniSet(coopServer.getServerDataIniFilename(), "rebooting", "false", "server"); //[b60012][cleanup]

			//if map to load is not the map that it wants to load, force coop map
			if (Q_stricmpn(sStartMap.c_str(), level.mapname.c_str(), MAX_QPATH) != 0) { //[b60012] chrissstrahl - fix missing .c_str()
				gi.Printf(va("COOP MOD - RESTORING MAP:\n%s\n", sStartMap.c_str()));
				gi.SendConsoleCommand(va("map %s\n", sStartMap.c_str()));
			}
		}
	}

	//[b60012] chrissstrahl - moved here
	coop_parserIniSet(coopServer.getServerDataIniFilename(), "errorboot", "false", "server");
}

void CoopServer::enforceLevelSpecificSettings()
//[b60011] chrissstrahl - enforce variouse settings in levels to ensure the game working correctly
{
	//make sure the physics is correct - this can happen due to a coop mod voting option
	//[b60012] chrissstrahl - fix missing .c_str()
	if (	Q_stricmpn(level.mapname.c_str(),"m6-exterior", 11) == 0 ||
			Q_stricmpn(level.mapname.c_str(),"m3l1a-forever", 13) == 0 ||
			Q_stricmpn(level.mapname.c_str(),"ent-training1", 13) == 0||			//[b60012] chrissstrahl - added exception
			Q_stricmpn(level.mapname.c_str(),"m5l1b-drull_ruins1", 18) == 0||
			Q_stricmpn(level.mapname.c_str(),"m5l2b-drull_ruins1", 18) == 0)		//[b60012] chrissstrahl - added exception
	{
		//this can have a delay - com_maxfps force can not, to work right
		if (level.time > 12) {
			if (world->getPhysicsVar(WORLD_PHYSICS_AIRACCELERATE) != 2) {
				world->setPhysicsVar("airAccelerate", 2.0f);

				if (multiplayerManager.inMultiplayer()) {
					multiplayerManager.HUDPrintAllClients("Server: Air Accelerate reset to 2 for this level to work\n");
				}
			}
		}

		//this is a listen server - the host is also playing on the same instance - there is no seperate server running
		//this is a default game bug, where fps higher 80 can cause player to get stuck floating above in low grav
		//this is apperently only happening to the host (SP/MP)
		if (Q_stricmpn(level.mapname.c_str(), "m6-exterior", 11) == 0) {
			if (dedicated->integer == 0 && multiplayerManager.inMultiplayer() || g_gametype->integer == GT_SINGLE_PLAYER) {
				int iFps = coop_returnCvarInteger("com_maxFps");
				if (iFps > COOP_SERVER_PHYSICS_BUG_MAX_FPS) {
					Player* player;
					player = (Player*)g_entities[0].entity;
					if (player != NULL) {
						gi.cvar_set("com_maxFps", va("%d", COOP_SERVER_PHYSICS_BUG_MAX_FPS));
						//inform player why the fps keeps resetting
						if (level.time > 12) {
							if (g_gametype->integer == GT_SINGLE_PLAYER) {
								gi.centerprintf(&g_entities[0], CENTERPRINT_IMPORTANCE_CRITICAL, COOP_SERVER_PHYSICS_BUG_MAX_FPS_MESSAGE);
							}
							else {
								if (level.time > (mp_warmUpTime->integer + 10)) {
									player->hudPrint(COOP_SERVER_PHYSICS_BUG_MAX_FPS_MESSAGE);
								}
							}
						}
					}
				}
			}
		}
	}
}

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
	//[b60014] chrissstrahl - allow sv_floodprotect toggeling to receive language/id/version
	game.coop_floodprotectToggle = false;
	game.coop_author = "";
	game.coop_story = "";
	game.coop_story_deu = "";
	game.coop_objectiveLastUpdate = -1.0f;
	game.coop_objectiveCycle = -1;
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
	game.bForceDynLightUpdate = false;
	game.cinematicSkipping = false;
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
	//[b60011] chrissstrahl - fixed crash on accsess to uninizialized cvar
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
			if (upgStrings.containsAt(ss,va("/sound/dialog/combat/%s_",sActorname.c_str())) != -1) {
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
				Q_strncpyz(unlocal, va("loc/deu/%s", sRem.c_str() + 13), sizeof(unlocal));
				if (strcmpi(ss.c_str(), unlocal)==0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}

				//handle eng version of dialog
				memset(unlocal, 0, sizeof(unlocal));
				Q_strncpyz(unlocal, va("loc/eng/%s", sRem.c_str() + 13), sizeof(unlocal));
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
	if (multiplayerManager.getTime() > 172800.0f && coop_returnPlayerQuantityInArena() <= 0) {//259200 3days  //172800 2days //1500000.0f  //1999000.0f
		multiplayerManager.centerPrintAllClients("^5Coop:^8 The Server needs to reboot now!\n", CENTERPRINT_IMPORTANCE_CRITICAL);

		gi.Printf(va("\ncoop_serverCheckEndMatch: multiplayerManager.getTime() > 172800.0f\n"));

		//[b60011] chrissstrahl - using now dedicated function
		coop_serverWarningBadSetting(va("HZM Coop Mod: The Server needs to reboot now! (%f)\n", multiplayerManager.getTime()));

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
		game.coop_lastmanstanding <= 0 ||
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
			if (player->coopPlayer.lmsDeaths < game.coop_lastmanstanding) {
				iActive++;
			}
		}
	}

	//gi.Printf("COOPDEBUG coop_serverLmsCheckFailure: all[%i] vs active[%i]\n", iAll, iActive);

	//fail mission if all are dead
	if ( iAll > 0 && iActive == 0 )
	{
		//[b607] needs to be exactly this to trigger playerDeathThread
		G_MissionFailed(va("PlayerKilled%i", ((int)G_Random(9.0f) + 1)));
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
	//while rebooting do not return false, as this might make the server change the map rather than quitting it
	if ( game.coop_reboot ){
		return true;
	}

	//don't reboot a listen server, don't reboot on level startup
	if (dedicated->integer == 0 || level.time < 10) {
		//added info message printed to executing player [b607]
		if (player != NULL && dedicated->integer == 0) {
			player->hudPrint("^5Info^8: Only dedicated Servers are meant to be rebooted.\n");
		}
		return false;
	}

	//reset var if the map we are about to load is not starting with ent-deck or ent-training_weap (excluding ent-deck.bsp)
	//if map is ent-deck
	//if map is standardlevel and starts with m
	bool bReset = false;

	if ( strlen( sMapToLoad ) > 4 ) {
		str s = sMapToLoad;
		s.tolower();
		//mapname starts with m like m11
		//has m and number followed by a l like m1l
		//has m and number followed by a l like m11l
		 //[b60012] chrissstrahl - fix missing .c_str()
		if (	!Q_stricmp( "ent-deck" , sMapToLoad.c_str() )  ||
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

	//okays, make sure the map we want to load does really exist if it doesn't reload current map.
	//you know paranoya isn't real if it is real ;)
	gi.Printf("REBOOT: SET MAPNAME %s\n", sMapToLoad.c_str());
	if ( gi.FS_Exists( va( "maps/%s.bsp" , sMapToLoad.c_str() ) ) != -1 ){
		coop_parserIniSet( coopServer.getServerDataIniFilename() , "startmap" , sMapToLoad.c_str() , "server" );
	}else{
		coop_parserIniSet( coopServer.getServerDataIniFilename() , "startmap" , level.mapname.c_str() , "server" );
	}
	//set marker that server rebooted
	coop_parserIniSet( coopServer.getServerDataIniFilename() , "rebooting" , "true" , "server" );

	//save client data into serverData.ini
	coop_serverSaveAllClientData();

	gi.Printf("REBOOT: ISSUING RECONNECT TO CLIENTS\n");
	game.coop_restartServerEarliestAt = level.time + 0.1f;//35

	//set signal to reboot
	game.coop_reboot = true;

	for (int i = 0; i < maxclients->integer; i++ ){
		if ( &g_entities[i].inuse && g_entities[i].entity && g_entities[i].client ){
			Player *playerValid = ( Player * )g_entities[i].entity;
			if (playerValid){
				//keep it in two lines
				gi.SendServerCommand( i , "stufftext \"disconnect;freeze 4;reconnect\"\n" );
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
	if (!sData || sData.length() < 5 || sData[0] == '*' || g_gametype->integer == GT_SINGLE_PLAYER /*|| dedicated->integer == 0*/) {
		return;
	}

	sData = upgStrings.getFileExtension( sData );
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

		//[b60012] chrissstrahl - fix missing .c_str()
		if ( !sArray || sArray && !Q_stricmp( sArray.c_str(), "")) {
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

	if ( game.isStandardLevel && game.levelType == MAPTYPE_MISSION && gi.areSublevels( level.mapname.c_str(), sMap.c_str()) == 1) { //[b60012] chrissstrahl - fix missing .c_str()
		bCoopKeppPlayerStatus = true;
	}
	//check if coop map is a sequel (determined by name simularities coop_[paradiseIsland]2 - coop_[gbs]7 )
	//[b60012] chrissstrahl - fix missing .c_str()
	else if ( Q_stricmp( upgStrings.getLettersOnlyAsLowercase( level.mapname ).c_str(), upgStrings.getLettersOnlyAsLowercase(sMap).c_str()) == 0) {
		bCoopKeppPlayerStatus = true;
	}

	if ( bCoopKeppPlayerStatus ){
		gi.Printf("coop_serverManageClientData - Client Data saved for sublevel.\n");
		coop_serverSaveAllClientData();
	}else{
		gi.Printf("coop_serverManageClientData - Client Data reset, not a sublevel.\n");
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

	//[b60012] chrissstrahl - add mapname to client data
	int iCnt = 0;
	str sMapname;
	while ( iCnt < level.mapname.length() ){
		if (level.mapname[iCnt] == '$' ){
			iCnt = 999;
		}
		else{
			sMapname += level.mapname[iCnt];
			iCnt++;
		}
	}


	str sData = va( "%d %d %d %d %d %d %d %s" ,
		( int )player->getHealth() ,
		( int )player->GetArmorValue() ,
		( int )player->AmmoCount( "Phaser" ) ,
		( int )player->AmmoCount( "Plasma" ) ,
		( int )player->AmmoCount( "Fed" ) ,
		( int )player->AmmoCount( "Idryll" ),
		( int )player->upgPlayerDeathTime(),
		sMapname.c_str()
		);

	//[b60012] chrissstrahl - have a printout
	if (player->coop_getId() == "") {
		if ((player->coopPlayer.timeEntered + 10) < level.time) {
			str sDataz = va("Your Data was REJECTED! INVALID_ID: (%s) '%s'\nFor: %s\n", player->coop_getId().c_str(), sData.c_str(), player->client->pers.netname);
			player->hudPrint(sDataz.c_str());
			gi.Printf(sDataz.c_str());
		}
		return;
	}

	//data already saved, or coopId is empty, we have no use for data that belongs to no id
	if (player->coop_playerCoopStatus() == sData) {
		return;
	}

	player->coop_playerCoopStatus(sData);

	coop_parserIniSet( coopServer.getServerDataIniFilename() , player->coop_getId(), sData, "client");
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
	//[b60011]chrissstrahl - challenge voteable option
	else if (!Q_stricmp(name, "challenge")) {
		iCorrected = coop_returnIntOrDefaultFromString(value, 1);
		coopChallenges.iCurrentChallenge = (short)iCorrected;
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
		coop_parserIniSet( coopServer.getServerDataIniFilename() , name , value , "server" );
	}else{
		coop_parserIniSet( coopServer.getServerDataIniFilename() , name , iCorrected , "server" );
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
		gameVars.SetVariable( "globalMissionEnterprise" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "globalMissionEnterprise" , "server" ) , 1 ) );//game.
		gameVars.SetVariable( "globalTurboliftRide" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "globalTurboliftRide" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "globalKleeyaChoice" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "globalKleeyaChoice" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "globalTelsiaChoice" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "globalTelsiaChoice" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "globalNoneChoice" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "globalNoneChoice" , "server" ) , 0 ) );//game.

		gameVars.SetVariable( "attrexianWeaponFound" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "attrexianWeaponFound" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "secretWeapon_CompressionRifle" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "secretWeapon_CompressionRifle" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "secretWeapon_IMOD" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "secretWeapon_IMOD" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "secretWeapon_IdryllStaff" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "secretWeapon_IdryllStaff" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "secretWeapon_RomulanExperimental" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "secretWeapon_RomulanExperimental" , "server" ) , 0 ) );//game.

		gameVars.SetVariable( "igmRoomsVisited" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "igmRoomsVisited" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "igmHolodeckSpawn" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "igmHolodeckSpawn" , "server" ) , 0 ) );//game.
		gameVars.SetVariable( "igmTurboliftSpawn" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "igmTurboliftSpawn" , "server" ) , 0 ) );//game.

		gameVars.SetVariable( "statusM5L2CUnlocked" , ( float )coop_returnIntOrDefaultFromString( coop_parserIniGet( coopServer.getServerDataIniFilename() , "statusM5L2CUnlocked" , "server" ) , 0 ) );//game.
	}

	//[b60011] chrissstrah - fixed a few checks that where done to sloppy and did not work right
	str sValue;
	int iVal;

	//SKILL
	iVal = coop_returnIntWithinOrDefault(coop_parserIniGet(coopServer.getServerDataIniFilename(), "skill", "server"), 0, 3, COOP_DEFAULT_SKILL);
	if (skill->integer != iVal) { gi.cvar_set("skill", va("%d", iVal)); }

	//FRIENDLY FIRE
	game.coop_friendlyFire = coop_returnFloatWithinOrDefault(coop_parserIniGet(coopServer.getServerDataIniFilename(), "friendlyfire", "server"),0 ,2, COOP_DEFAULT_FF);
	gameVars.SetVariable("friendlyFire", game.coop_friendlyFire);

	//MAXSPEED
	game.coop_maxspeed = coop_returnIntWithinOrDefault(coop_parserIniGet(coopServer.getServerDataIniFilename(), "maxspeed", "server"), 200,1000 , COOP_DEFAULT_MAXSPEED);
	world->setPhysicsVar("maxspeed", game.coop_maxspeed);

	//RESPAWN TIME
	game.coop_respawnTime = coop_returnIntWithinOrDefault(coop_parserIniGet(coopServer.getServerDataIniFilename(), "respawntime", "server"),0 ,60,COOP_DEFAULT_RESPAWNTIME );
	multiplayerManager.setRespawnTime( (float)game.coop_respawnTime );

	//AIR ACCELERATE
	iVal = coop_returnIntWithinOrDefault(coop_parserIniGet(coopServer.getServerDataIniFilename(), "airaccelerate", "server"), 0, 4, COOP_DEFAULT_AIRACCELERATE);
	world->setPhysicsVar("airAccelerate", iVal);

	//AWARDS
	game.coop_awardsActive = coop_returnIntWithinOrDefault(coop_parserIniGet(coopServer.getServerDataIniFilename(), "awards", "server"), 0, 1, COOP_DEFAULT_AWARDS);
	
	//TEAMICON
	coopChallenges.iCurrentChallenge = (short)coop_returnIntWithinOrDefault(coop_parserIniGet(coopServer.getServerDataIniFilename(), "challenge", "server"), 0,1 ,(short)COOP_DEFAULT_CHALLENGE);
	
	//LMS - LAST MAN STANDING
	game.coop_lastmanstanding = coop_returnIntWithinOrDefault(coop_parserIniGet(coopServer.getServerDataIniFilename(), "lastmanstanding", "server"), 0,10 ,(int)COOP_DEFAULT_LASTMANSTANDING );
	
	//DEAD BODIES STAYING
	game.coop_deadBodiesPerArea = coop_returnIntWithinOrDefault(coop_parserIniGet(coopServer.getServerDataIniFilename(), "deadbodies", "server"),0,25 , COOP_DEFAULT_DEADBODIES);
	
	//STASIS TIME
	game.coop_stasisTime = coop_returnIntWithinOrDefault(coop_parserIniGet(coopServer.getServerDataIniFilename(), "stasistime", "server"),7,30 , COOP_DEFAULT_STASISTIME);
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
	if ( player->coop_playerCoopStatus() == sData )
		return;

	player->coop_playerCoopStatus(sData);

	coop_parserIniSet( coopServer.getServerDataIniFilename() , player->coop_getId(), sData, "client");
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
	//[b60013] chrissstrahl - set/reset vars upon each map load
	coopScripting.init();

	//hzm coop mod chrissstrahl - determin maptype
	//game.isStandardLevel
	//game.isMissionLevel
	//game.isSecretLevel
	//game.isIGMLevel
	//game.isCoopLevel

	//SETS SOME OF THE ABOVE LISTED COOP GAME VARS
	//if the map is included in the game or the coop mod it will return true
	//there is no >>extra<< var that will be set if it is a coop custom map
	game.isCoopIncludedLevel = coop_returnLevelType(level.mapname, game.isStandardLevel, game.levelType);

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

	//hzm coop mod chrissstrahl - print out some info to the console
	#ifdef __linux__
		str sLibrarayName = "ef2gamei386.so";
	#else
		str sLibrarayName = "gamex86.dll";
	#endif

	gi.Printf( va( "=============================================\n=== HaZardModding Coop Mod %i\n=== %s build %s @ %s\n=============================================\n", COOP_BUILD, sLibrarayName.c_str(), __DATE__, __TIME__) );
	if ( game.isCoopIncludedLevel  ) {
		gi.Printf( va( "=== %s is included in this build!\n=============================================\n" , level.mapname.c_str() ) );
	}

	//hzm coop mod chrissstrahl - NO MORE - Gallifrey Falls No More
	if ( g_gametype->integer != GT_MULTIPLAYER ) {
		return;
	}

	//[b60011] chrissstrahl - dectect if coop mod should be active
	if (game.levelType > MAPTYPE_MULTIPLAYER) {
		game.coop_isActive = true;

		//let the level scripts know that this is coop
		levelVars.SetVariable("isCoopActive", 1.0f);
	}

	//[b60011] chrissstrahl - initzialise
	coopChallenges.init();

	//hzm coop mod chrissstrahl - check if sv_floodprotect should be enabled
	//if coop is not active
	if ( !game.coop_isActive ) {
		if ( coop_returnBool( coop_parserIniGet( coopServer.getServerDataIniFilename() , "sv_floodprotect" , "server" )) ) {
			gi.SendConsoleCommand( "set sv_floodprotect 1\n" );
		}

		//hzm coop mod chrissstrahl - disable 3rd person aim
		gi.cvar_set( "g_aimviewangles" , "0" );

		levelVars.SetVariable( "isCoopLevel" , 0.0f );
		return;
	}
	
	game.coop_saveClientData = true;

	//hzm coop mod chrissstrahl - dissable native spam protection, but remember its state
	coop_parserIniSet( coopServer.getServerDataIniFilename() , "sv_floodprotect" , va("%d",sv_floodprotect->integer) , "server" );
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
	//gi.Printf( va("ARE SUBLEVELS:  %i\n" , gi.areSublevels( level.mapname.c_str() , level.nextmap.c_str() ) ) );

	//hzm coop mod chrissstrahl - register this server to the alternative master server
	str cvarNum;
	int i;
	int iEmptySlot = 0;
	bool bMasterSet = false;
	str cvarValueString;
	for ( i = 1; i < 7; i++ ){
		cvarValueString = coop_returnCvarString( va( "sv_master%i" , i ) );
		if ( strlen( va( "sv_master%d" , i ) ) > 0 ){
			if ( !stricmp( cvarValueString , COOP_SERVER_MASTERSERVER) ){ //[b60011] chrissstrahl - changed to new Master address
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
		gi.SendConsoleCommand( va( "set sv_master%d %s\n" , iEmptySlot, COOP_SERVER_MASTERSERVER) );
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
			//[b60011] chrissstrahl - made warnings more compact
			coop_serverWarningBadSetting("SETUP ERROR");
			coop_serverWarningBadSetting("coop_mod/cfg/server/setup.cfg $$NotFoundOnServer$$ !!!\nCould not configure server for Coop Mod!");
			gi.SendConsoleCommand( "killserver\n" );
			return;
		}

		SetupExecuted++;
		if ( SetupExecuted > COOP_MAX_SETUP_TRIES ){
			//[b60011] chrissstrahl - made warnings more compact
			coop_serverWarningBadSetting("SETUP ERROR");
			coop_serverWarningBadSetting("Server was stuck in a loop executing setup.cfg\nCould not configure server for Coop Mod!");
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
		coop_serverWarningBadSetting( "COOP BAD Setting: sv_pure 1 changing to 0" );
		correctingTheseSettingsRequiresMapload = true;
	}
	
	//[b60011] chrissstrahl - make sure at least 4 players are able to play from the get go (this is in Coop)
	if ( coop_returnCvarInteger( "sv_maxclients" ) < 2 )
	{
		gi.SendConsoleCommand("set sv_maxclients 4\n"); //can't set this cvar directly or game crashes
		coop_serverWarningBadSetting("BAD SETTING");
		coop_serverWarningBadSetting("sv_maxclients is < 2\nPLEASE START SERVER WITH sv_maxclients SET BEWTEEN 2 AND 8");
		correctingTheseSettingsRequiresMapload = true;
	}

	if ( correctingTheseSettingsRequiresMapload == true )
	{
		coop_serverWarningBadSetting("coop_serverSetup() requested a map reload");
		str sMapToLoad = level.mapname;

		//hzm coop mod chrissstrahl - load last coop map if there is any
		bool bServerHasRebooted = false;
		bServerHasRebooted = coop_returnBool(coop_parserIniGet( coopServer.getServerDataIniFilename() , "rebooting" , "server" ));

		//hzm coop mod chrissstrahl - load the map that we are actually suppose to load
		if ( bServerHasRebooted ){
			coop_parserIniSet( coopServer.getServerDataIniFilename() , "rebooting" , "false" , "server" );
			str sStartMap = coop_parserIniGet( coopServer.getServerDataIniFilename() , "startmap" , "server" );

			if ( gi.FS_Exists( va( "maps/%s.bsp" , sStartMap.c_str() ) ) ){
				//[b60012] chrissstrahl - fix missing .c_str()
				if ( Q_stricmp( sMapToLoad.c_str(), sStartMap.c_str()) == 0) {
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
void coop_serverManageAi(bool aiOn)
{
	level.ai_on = aiOn;
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
	bool isScriptFile = false;

	//[b60011] Chrissstrahl - clean this up a bit - and while I am at it, fix issues
	str sFileExt = upgStrings.getFileExtension(standardPath);

	//we handle alternative script file extension, so we need to know
	//[b60012] chrissstrahl - fix missing .c_str()
	if (Q_stricmpn(sFileExt.c_str(), ".scr", 4) == 0) {
		isScriptFile = true;
	}

	str fileInMapfolder = standardPath;
	str fileInCoopfolder = va("coop_mod/%s",standardPath.c_str());

	bool fileInMapsExists = false;
	bool fileInCoopExists = false;
	bool fileInMapsExistsScript = false;
	bool fileInCoopExistsScript = false;

	//if it is a script file check if there is a .script file
	if (isScriptFile) {
		if (gi.FS_ReadFile(va("%sipt", fileInMapfolder.c_str()), NULL, true) != -1) {
			fileInMapfolder = va("%sipt", fileInMapfolder.c_str());
			fileInMapsExistsScript = true;
		}
		if (gi.FS_ReadFile(va("%sipt", fileInCoopfolder.c_str()), NULL, true) != -1) {
			fileInCoopfolder = va("%sipt", fileInCoopfolder.c_str());
			fileInCoopExistsScript = true;
		}
	}

	if (gi.FS_ReadFile(fileInMapfolder.c_str(), NULL, true) != -1) {
		fileInMapsExists = true;
	}
	//if we are in multiplayer or the script is not in "/maps/" check "coop_mod/maps/"
	if (gi.FS_ReadFile(fileInCoopfolder.c_str(), NULL, true) != -1) {
		fileInCoopExists = true;
	}

	//make sure we load scripts for Singleplayer from "/maps/" (if they exist) - in singleplayer
	//make sure we load scripts for Coop from "coop_mod/maps/" (if they exist) - first in multiplayer
	if (g_gametype->integer == GT_SINGLE_PLAYER){
		if (fileInMapsExists || fileInMapsExistsScript){
			return fileInMapfolder;
		}
		if (fileInCoopExists || fileInCoopExistsScript) {
			return	fileInCoopfolder;
		}
	}
	else {
		if (fileInCoopExists || fileInCoopExistsScript) {
			return	fileInCoopfolder;
		}
		if (fileInMapsExists || fileInMapsExistsScript) {
			return fileInMapfolder;
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
	//[b60012] chrissstrahl - terminate only on Linux server
	//if ( g_gametype->integer != 1 || dedicated->integer < 1 )
	#ifndef __linux__
		return false;
	#endif

	//Make note into ini, disable reboot
	coop_parserIniSet( coopServer.getServerDataIniFilename() , "rebooting" , "false" , "server" );
	coop_parserIniSet( coopServer.getServerDataIniFilename() , "errorboot" , "true" , "server" );
	coop_parserIniSet( coopServer.getServerDataIniFilename() , "errorfatal" , coop_returnBoolWord( bFatal ) , "server" );
	coop_parserIniSet( coopServer.getServerDataIniFilename() , "errormap" , level.mapname , "server" );
	coop_parserIniSet( coopServer.getServerDataIniFilename() , "errortext" , sError , "server" );

	//[b60011] chrissstrahl - this prevents the server from exiting/rebooting if noerrorhalt=true in serverData.ini
	//Changed from, errorhalt to noerrorhalt, so that a server will not reboot on default
	//You are REQUIRED now to set noerrorhalt=true in /serverData.ini to allow server to exit on error
	if ( coop_returnBool( coop_parserIniGet( coopServer.getServerDataIniFilename() , "noerrorhalt" , "server" ) ) )
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
	//[b60011] chrissstrahl - make sure certain settings are forced for levels that won't work right without it
	coopServer.enforceLevelSpecificSettings();
	
	//[b60014] chrissstrahl - reenable sv_floodprotect after it was turned off before
	coopServer.svFloodProtectEnable();

	if (!game.coop_isActive) {
		return;
	}

	//hzm coop mod chrissstrahl - quit server if requested
	if ( game.coop_reboot && game.coop_restartServerEarliestAt < level.time ){ 
		//[b607] chrissstrahl - allow to adjust the coop reboot feature
		str sCoopRebootState = coop_parserIniGet(coopServer.getServerDataIniFilename(), "rebootType", "server");
		if (Q_stricmpn(sCoopRebootState.c_str(), "killserver",10) == 0) {
			gi.Printf("HZM COOP MOD: game.coop_reboot == true, rebootType = killserver, server killed\n");
			gi.SendConsoleCommand("exec coop_mod/cfg/server/killserver.cfg\n");
		}
		else{
			gi.Printf("HZM COOP MOD: game.coop_reboot == true, server quitting\n");
			gi.SendConsoleCommand("quit\n");
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
			gi.Printf("HZM COOP MOD: Disabled g_showevents again.\n");
		}
	}

	//hzm coop mod chrissstrahl - this will manage the objective marker
	coop_objectivesMarkerUpdate();
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
// Returns:     void
//              
//================================================================
void coop_serverCleanup(bool restart)
{
	//[b60011] chrissstrahl - moved code here from: void MultiplayerManager::cleanup( qboolean restart )

	//[b60011] chrissstrahl - this lets us detect if the map was restarted or loadad
	game.levelRestarted = restart;

	game.coop_author = "";
	game.coop_story = "";
	game.coop_story_deu = "";
	game.coop_awardsActive = false;
	game.coop_isActive = false;

	//[b60011] chrissstrahl - only clean up while in multiplayer bejond this point
	if (!multiplayerManager.inMultiplayer()) {
		return;
	}

	//hzm coop mod chrissstrahl - save client data
	if (game.coop_isActive) {
		coop_serverSaveAllClientData();
	}

	//[b60011] chrissstrahl
	coopChallenges.cleanUp(restart);
	coopNpcTeam.cleanUp(restart);
}

//[b60011] chrissstrahl - bad setting Warning
//================================================================
// Name:        coop_serverWarningBadSetting
// Class:       -
//              
// Description: Cleans up vars and other stuff once the level is cleaned up
//              
// Parameters:  string Message
//              
// Returns:     void
//              
//================================================================
void coop_serverWarningBadSetting(str sWarning)
{
	gi.Printf("\n====================================\n");
	gi.Printf(va("%s\n", sWarning.c_str()));
	gi.Printf("====================================\n\n");
}