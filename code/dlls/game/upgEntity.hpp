//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// Entity class related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"


//[GAMEUPGRADE][b60014] chrissstrahl - used to seperate code bases better
class UpgEntity
{
public:
	float		upgEntityGetFloatVar(Entity* ent, str varname);
	int			upgEntityGetIntegerVar(Entity* ent, str varname);
	Vector		upgEntityGetVectorVar(Entity* ent, str varname);
	str			upgEntityGetStringVar(Entity* ent, str varname);
};


extern UpgEntity upgEntity;