//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// WORLD Class related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"

class UpgWorld
{
	bool			updateDynamicLights = false;
public:
	bool			upgWorldGetUpdateDynamicLights();
	void			upgWorldSetUpdateDynamicLights(bool bUpdate);
};

extern UpgWorld upgWorld;
extern Event EV_World_GetPhysicsVar;