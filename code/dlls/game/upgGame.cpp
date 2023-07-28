//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// General Game related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "mp_manager.hpp"

#include "upgGame.hpp"

UpgGame upgGame;

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
	if (!multiplayerManager.inMultiplayer()) { return; }

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