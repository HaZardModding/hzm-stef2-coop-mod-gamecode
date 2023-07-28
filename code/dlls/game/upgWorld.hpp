//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// WORLD Class related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#pragma once

class UpgWorld
{
	bool			updateDynamicLights = false;
public:
	bool			upgWorldGetUpdateDynamicLights();
	void			upgWorldSetUpdateDynamicLights(bool bUpdate);
};

extern UpgWorld upgWorld;