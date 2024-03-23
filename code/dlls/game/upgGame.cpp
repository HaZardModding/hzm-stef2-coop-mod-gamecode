//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// General Game related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#include "upgGame.hpp"
UpgGame upgGame;

#include "upgWorld.hpp"

#include "_pch_cpp.h"
#include "mp_manager.hpp"


//-----------------------------------------------------------------------------------
// Events, these have external Dependencies
//-----------------------------------------------------------------------------------
Event EV_ScriptThread_ConfigstringRemove
(
	"configstringRemove",
	EV_SCRIPTONLY,
	"s",
	"String",
	"Removes given string from the configstrings to fix cl_parsegamestate"
);


//========================================================[b60021]
// Name:        upgGameIinitGame 
// Class:       UpgGame
//              
// Description: Started as soon as the game is initialised
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
void UpgGame::upgGameInitGame()
{
	if (g_gametype->integer != GT_MULTIPLAYER) {
		return;
	}

	upgGameResetMapsLoaded();
}


//========================================================[b60021]
// Name:        upgGameResetMapsLoaded 
// Class:       UpgGame
//              
// Description: Resets number of loadad maps
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
void UpgGame::upgGameResetMapsLoaded()
{
	reconnectMapsLoaded = 0;
}

//========================================================[b60021]
// Name:        upgGameCountMapsLoaded 
// Class:       UpgGame
//              
// Description: Counts up loaded maps + 1
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
void UpgGame::upgGameCountMapsLoaded()
{
	if (g_gametype->integer != GT_MULTIPLAYER) {
		return;
	}

	reconnectMapsLoaded++;
}

//========================================================[b60021]
// Name:        upgGameGetMapsLoadedSinceReconnect 
// Class:       UpgGame
//              
// Description: Returns how many levels where loaded since the last reconnect event
//              
// Parameters:  void
//              
// Returns:     int
//================================================================
int UpgGame::upgGameGetMapsLoadedSinceReconnect()
{
	if (g_gametype->integer != GT_MULTIPLAYER) {
		return 0;
	}

	return reconnectMapsLoaded;
}

//========================================================[b60021]
// Name:        upgGameGetCvarReconnectTime 
// Class:       UpgGame
//              
// Description: Sets time given to player to reconnect before game continues
//              
// Parameters:  void
//              
// Returns:     int
//================================================================
int UpgGame::upgGameGetCvarReconnectTime()
{
	if (g_gametype->integer != GT_MULTIPLAYER) {
		return 0;
	}

	int cvarReconnectTime = 0;
	cvar_t* cvar1 = gi.cvar_get("upg_reconnectTime");
	if (cvar1 && cvar1->integer > 10) {
		cvarReconnectTime = cvar1->integer;
	}

	if (cvarReconnectTime <= 0) { cvarReconnectTime = 15; }
	if (cvarReconnectTime  < 10) { cvarReconnectTime = 10; }
	if (cvarReconnectTime > 60) { cvarReconnectTime = 60; }
	return cvarReconnectTime;
}

//========================================================[b60021]
// Name:        upgGameSetReconnectTime 
// Class:       UpgGame
//              
// Description: Sets time given to player to reconnect before game continues
//              
// Parameters:  int
//              
// Returns:     void
//================================================================
void UpgGame::upgGameSetReconnectTime(int time)
{
	reconnectTime = time;
}

//========================================================[b60021]
// Name:        upgGameGetReconnectTime 
// Class:       UpgGame
//              
// Description: Returns time given to player to reconnect before game continues
//              
// Parameters:  -
//              
// Returns:     int
//================================================================
int UpgGame::upgGameGetReconnectTime()
{
	if (g_gametype->integer != GT_MULTIPLAYER) {
		return 0;
	}

	return reconnectTime;
}

//========================================================[b60021]
// Name:        upgGameStartMatch 
// Class:       UpgGame
//              
// Description: Executed when the game starts the match - Multiplayer only
//              
// Parameters:  -
//              
// Returns:     void
// -> MultiplayerModeBase::startMatch
//================================================================
void UpgGame::upgGameStartMatch()
{
	//STOP waiting for players to reconnect after X sec - and start the cinematic or what ever
	Event* ev = new Event(EV_SetFloatVar);
	ev->AddString("upg_playersReconnectingWait");
	ev->AddFloat(0.0f);
	world->PostEvent(ev, ((float)upgGameGetReconnectTime() + (float)mp_warmUpTime->integer));
}

//========================================================[b60016]
// Name:        setEnvironment 
// Class:       UpgGame
//              
// Description: Sets the name of the current Enviroment (loadingscreen/sublevels/localstrings)
//              
// Parameters:  str
//              
// Returns:     void
//================================================================
void UpgGame::setEnvironment(str name)
{
	environmentName = name;
}

//========================================================[b60016]
// Name:        getEnvironment 
// Class:       UpgGame
//              
// Description: Sets the name of the current Enviroment (loadingscreen/sublevels/localstrings)
//              
// Parameters:  void
//              
// Returns:     str
//================================================================
str UpgGame::getEnvironment()
{
	return environmentName;
}

//========================================================[b60016]
// Name:        cleanUp 
// Class:       UpgGame
//              
// Description: Resets and cleans up UpgGame class related stuff
//				-> void Level::CleanUp( qboolean restart )
//              
// Parameters:  bool
//              
// Returns:     void
//================================================================
void UpgGame::cleanUp(bool restart)
{
	setCameraCurrent(NULL);

	//--------------------------------------------------------------
	//[b60021] chrissstrahl - clean up the models cache
	//--------------------------------------------------------------
	//[b60021] chrissstrahl - make sure this is executed only in multiplayer
	if (g_gametype->integer == GT_MULTIPLAYER) {
		upgGame.flushTikisServer();
		upgGame.flushTikisPlayers();		
	}
}
	
//===========================================================[b607]
// Name:        configstringRemove
// Class:       UpgGame
//              
// Description: Removes the given string from the configstrings
//              
// Parameters:  STRING to remove
//              
// Returns:     int
//              
//[b607] chrissstrahl - try to minimize the usage of configstrings due to cl_parsegamestate issue
//================================================================
int UpgGame::configstringRemove(str sRem)
{
	if (!sRem.length()) {
		return 0;
	}

	int iRem = 0;
	char* s;
	for (int i = 1; i < MAX_CONFIGSTRINGS; i++) {
		s = gi.getConfigstring(i);
		str ss = "";
		ss += s;

		if (ss.length() > 0) {
			//if this is a dialog try to handle german and english localized strings as well
			if (!strnicmp(ss.c_str(), "localization/", 13)) {
				//regular dialog
				if (strcmpi(ss.c_str(), sRem.c_str()) == 0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}

				//handle deu version of dialog
				char unlocal[96]; //MAX_QPATH + 5 <- did not work!
				memset(unlocal, 0, sizeof(unlocal));
				Q_strncpyz(unlocal, va("loc/deu/%s", sRem.c_str() + 13), sizeof(unlocal));
				if (strcmpi(ss.c_str(), unlocal) == 0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}

				//handle eng version of dialog
				memset(unlocal, 0, sizeof(unlocal));
				Q_strncpyz(unlocal, va("loc/eng/%s", sRem.c_str() + 13), sizeof(unlocal));
				if (strcmpi(ss.c_str(), unlocal) == 0) {
					//gi.Printf(va("#REMOVED CS: #%i: %s\n", i, ss.c_str()));
					gi.setConfigstring(i, "");
					iRem++;
				}
			}
			else {
				if (strcmpi(ss.c_str(), sRem.c_str()) == 0) {
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
// Name:        upgGameConfigstringRemove
// Class:       CThread
//              
// Description: Removes the given string from the configstrings
//				Called from level scripts
//              
// Parameters:  Event*
//              
// Returns:     void
//              
//[b607] chrissstrahl - try to minimize the usage of configstrings due to cl_parsegamestate issue
//================================================================

//[b607] chrissstrahl - remove given string from configstrings
void CThread::upgGameConfigstringRemove(Event* ev)
{
	if (ev->NumArgs() < 1) { return; }

	str sName = ev->GetString(1);
	int iNum = upgGame.configstringRemove(sName);
	gi.Printf("upgGame.configstringRemove(%s) removed %i items\n", sName.c_str(), iNum);
}

//===========================================================[b6xx]
// Name:        upgGame.checkMpGametype
// Class:       UpgGame
//              
// Description: fix mp_gametype, which will otherwise lead to join team buttons no longer working right
//              the coop mod does cause that if started from with game menu, on purpose
// 
// Parameters:  void
//              
// Returns:     void        
//================================================================
void UpgGame::checkMpGametype()
{
	if (mp_gametype->integer > 3 || mp_gametype->integer < 0) {
		gi.cvar_set("mp_gametype", "0");
	}
}

//=========================================================[b60012]
// Name:        upgGame.gameFlushTikisPlayers
// Class:       UpgGame
//              
// Description: Flushtikis for clients - try to fix tiki model anim cache overload issue
//              
// Parameters:  void
//              
// Returns:     void        
//================================================================
void UpgGame::flushTikisPlayers()
{
	//[b60021] chrissstrahl - make sure this is executed only in multiplayer
	if (g_gametype->integer != GT_MULTIPLAYER) {
		return;
	}

	//flush also clients
	for (int i = 0; i < maxclients->integer; i++) {
		if (&g_entities[i] && g_entities[i].client && g_entities[i].inuse) {
			Player* playerValid = (Player*)g_entities[i].entity;
			if (playerValid && !(playerValid->edict->svflags & SVF_BOT)) {
				//[b60014] chrissstrahl - changed so it will instantly transmit
				//DelayedServerCommand(i, "flushtikis");
				gi.SendServerCommand(i, "stufftext flushtikis\n");
			}
		}
	}
}

//=========================================================[b60012]
// Name:        flushTikisServer
// Class:       UpgGame
//              
// Description: Flushtikis server - try to fix tiki model anim cache overload issue
//              
// Parameters:  void
//              
// Returns:     void        
//================================================================
void UpgGame::flushTikisServer()
{
	//[b60021] chrissstrahl - make sure this is executed only in multiplayer
	if (g_gametype->integer != GT_MULTIPLAYER) {
		return;
	}

	//[b60011] chrissstrahl - refined the handling, handle dedicated server as before
	if (dedicated->integer > 0) {
		Engine_TIKI_FreeAll(1);//call to function pointer

		//[b60012] chrissstrahl - disabled, I don't think it does us any good
		//[b607] chrissstrahl - flushtikis - fixing animation issues of actor and other models
		//gi.SendServerCommand(NULL, "stufftext flushtikis\n");
	}
	else {
		//handle listen servers with a automatic flushtikis
		gi.SendServerCommand(0, "stufftext flushtikis\n");
	}
}

//================================================================
// Name:        setCameraCurrent
// Class:       UpgGame
//              
// Description: set current camera
//              
// Parameters:  void
//              
// Returns:     void        
//================================================================
void UpgGame::setCameraCurrent(Entity* eCam)
{
	cinematicCurrentCam = eCam;
}

//================================================================
// Name:        getCameraCurrent
// Class:       UpgGame
//              
// Description: get current camera
//              
// Parameters:  void
//              
// Returns:     Entity*        
//================================================================
Entity* UpgGame::getCameraCurrent()
{
	return cinematicCurrentCam;
}

//================================================================
// Name:        startCinematic
// Class:       UpgGame
//              
// Description: hides player during cinematic start on multiplayer
//              
// Parameters:  void
//              
// Returns:     void     
//================================================================
void UpgGame::startCinematic()
{
	//[b60021] chrissstrahl - make sure this is executed only in multiplayer
	if (g_gametype->integer != GT_MULTIPLAYER) {
		return;
	}

	gentity_t* other;
	Player* player;

	int j;
	for (j = 0; j < game.maxclients; j++){
		other = &g_entities[j];
		if (other->inuse && other->client && other->entity){
			player = (Player*)other->entity;
			//hzm gameupdate chrissstrahl - do this allways
			player->setSolidType(SOLID_NOT);
			player->takedamage = DAMAGE_NO;
			player->SetState("STAND", "STAND");
			player->cinematicStarted();

			//player->hideModel(); //hzm - does not work right
			//hzm gameupdate chrissstrahl - hide with delay, to fix issues
			Event* hidePlayer;
			hidePlayer = new Event(EV_Hide);
			player->PostEvent(hidePlayer, 0.1f);
		}
	}
}

//================================================================
// Name:        stopCinematic
// Class:       UpgGame
//              
// Description: handles cinematic stop on multiplayer
//              
// Parameters:  void
//              
// Returns:     void     
//================================================================
void UpgGame::stopCinematic()
{
	if (!multiplayerManager.inMultiplayer()) { return; }
	
	//clear current cinematic camera - used for multiplayer/coop camera tracking
	upgGame.setCameraCurrent(NULL);

	gentity_t* other;
	Player* player;

	int j;
	for (j = 0; j < game.maxclients; j++){
		other = &g_entities[j];
		if (other->inuse && other->client && other->entity){
			player = (Player*)other->entity;

			player->cinematicStopped();
			player->takedamage = DAMAGE_YES;
		}
	}
}