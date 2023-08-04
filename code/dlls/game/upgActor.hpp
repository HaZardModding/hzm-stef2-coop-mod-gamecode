//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// Contains Actor related code for the gameupgrades
//-----------------------------------------------------------------------------------
#pragma once
class Entity;
class Actor;
class str;

class UpgActor
{
	friend Actor;
	bool usedByPlayer = false;
public:
	int					setHeadWatchTarget(Entity* entity, str watchTarget);
	void				setUsedByPlayer(Actor* actor, bool bUsed);
	bool				getUsedByPlayer(Actor* actor);
};

extern UpgActor upgActor;