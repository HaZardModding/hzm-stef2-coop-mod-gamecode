//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING WORLD RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#pragma once

#include "worldspawn.h"

class CoopWorld
{
public:
	void			coop_worldSetLevelStartTime();
};

extern CoopWorld coopWorld;
extern Event EV_World_LoadMap;
extern Event EV_World_AutoFailure;