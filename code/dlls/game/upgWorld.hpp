//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// WORLD Class related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"

constexpr auto UPGWORLD_FLUSHTIKI_MAPLISTSIZE = 6;

class UpgWorld
{
private:
					
	bool			updateDynamicLights = false;
	str				flushTikiMaps[UPGWORLD_FLUSHTIKI_MAPLISTSIZE];
public:
					UpgWorld();
	void			upgWorldSetPlayersSetReconnecting(bool reconnecting);
	bool			upgWorldSetPlayersGetReconnecting();
	bool			upgWorldGetUpdateDynamicLights();
	void			upgWorldSetUpdateDynamicLights(bool bUpdate);
	void			upgWorldFlushTikisLevelStart();
};

extern UpgWorld upgWorld;
extern Event EV_World_GetPhysicsVar;