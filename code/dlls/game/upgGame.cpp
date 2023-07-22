//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// Game related upgrades and fixes
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"

#include "upgGame.hpp"

UpgGame upgGame;

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