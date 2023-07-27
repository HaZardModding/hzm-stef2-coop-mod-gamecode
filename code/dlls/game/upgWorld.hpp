//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// WORLD Class related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#ifndef __UPGWORLD_HPP__
#define __UPGWORLD_HPP__

class UpgWorld
{
	bool			updateDynamicLights = false;
public:
	bool			upgWorldGetUpdateDynamicLights();
	void			upgWorldSetUpdateDynamicLights(bool bUpdate);
};

extern UpgWorld upgWorld;

#endif /* upgWorld.hpp */