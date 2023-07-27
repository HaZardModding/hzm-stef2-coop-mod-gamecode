//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING WORLD RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "coopWorld.hpp"
CoopWorld coopWorld;

#include "worldspawn.h"

#include "upgWorld.hpp"


//================================================================
// Name:        coop_worldAutoFailure
// Class:       CoopWorld
//              
// Description: [b6xx] used to compare realtime of level start to realtime when player last died
//				as some sort of reconnect protection if some one tries to cheat Coop LMS or Respawntime
//              
// Parameters:  void
//              
// Returns:     void     
//================================================================
void CoopWorld::coop_worldSetLevelStartTime()
{
	time_t result = time(NULL);
	localtime(&result);
	game.coop_levelStartTime = (int)result;
}

//================================================================
// Name:        coop_worldAutoFailure
// Class:       World
//              
// Description: [b607] allow call as event, allowes delays
//              
// Parameters:  Event*
//              
// Returns:     void     
//================================================================
void World::coop_worldAutoFailure(Event* ev)
{
	game.coop_autoFailPending = false;
	G_MissionFailed("PlayerKilled");
}

//================================================================
// Name:        coop_worldLoadMap
// Class:       World
//              
// Description: [b607] allow call as event, allowes delays
//              
// Parameters:  Event*
//              
// Returns:     void     
//================================================================
void World::coop_worldLoadMap(Event* ev)
{
	str command = ev->GetString(1);
	if (command.length()) {
		if (sv_cheats->integer == 1) {
#ifdef __linux__
			gi.SendConsoleCommand(va("map %s \n", command.c_str()));//load map now
#else
			gi.SendConsoleCommand(va("devmap %s \n", command.c_str()));//load map now
#endif	
		}
		else {
			gi.SendConsoleCommand(va("map %s \n", command.c_str()));//load map now
		}
	}
}