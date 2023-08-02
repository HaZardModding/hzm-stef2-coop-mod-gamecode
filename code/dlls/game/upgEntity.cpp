//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// Entity class related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#include "upgEntity.hpp"


#include "mp_manager.hpp"


//-----------------------------------------------------------------------------------
// Events, these have external Dependencies
//-----------------------------------------------------------------------------------
//NONE


//=========================================================[b60014]
// Name:        upgEntityMakeSolidAsap
// Class:       Entity
//              
// Description: check if entity is in actor or in player that is not it self
//				if not it will turn the Entity solid
//				this is used make sure player and actors don't get stuck inside each other
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
void Entity::upgEntityMakeSolidAsap()
{
	if (_makeSolidASAP) {
		if (edict->solid == SOLID_NOT) {
			if (!isInsidePlayerOrActor()) {
				setSolidType(SOLID_BBOX);
				_makeSolidASAP = false;
			}
		}
		else {
			if (isInsidePlayerOrActor()) {
				setSolidType(SOLID_NOT);
			}
			else {
				_makeSolidASAP = false;
				setSolidType(SOLID_BBOX);
			}
		}
	}
}


//================================================================
// Name:        isInsidePlayerOrActor
// Class:       Entity
//              
// Description: checks if this entity is inside a player or actor
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Entity::isInsidePlayerOrActor()
{
	//exit if player or actor but dead
	if (isSubclassOf(Player) && health <= 0.0f || !isSubclassOf(Actor) && health <= 0.0f)
		return false;

	//exit if player but spectator
	if (isSubclassOf(Player) && multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator((Player*)this))
		return false;

	int i;
	Entity* entity2 = NULL;

	//check (all) entities against this entity
	for (i = 0; i < maxentities->integer; i++) {

		entity2 = g_entities[i].entity;

		//skip if missing,same,not player/actor or dead
		if (!entity2 || this == entity2)
			continue;

		if (!entity2->isSubclassOf(Player) && !entity2->isSubclassOf(Actor))
			continue;

		if (entity2->health <= 0)
			continue;

		if (entity2->isSubclassOf(Player) && multiplayerManager.isPlayerSpectator((Player*)entity2))
			continue;

		//do not check actor on actor, because this creates more trouble than it is useful
		if (isSubclassOf(Actor) && entity2->isSubclassOf(Actor))
			continue;

		//ai_off and hidden
		//make sure we skip notsolid and hidden actors as they are probably left over from cinematic
		Actor* a = NULL;
		if (isSubclassOf(Actor)) {
			a = (Actor*)this;
		}
		else if (entity2->isSubclassOf(Actor)) {
			a = (Actor*)entity2;
		}
		if (a && a->isSubclassOf(Actor) && (!a->GetActorFlag(ACTOR_FLAG_AI_ON) /* && a->hidden() && a->edict->svflags & SVF_NOCLIENT*/)) {
			continue;
		}

		//player inside actor or another plyer, make sure the other is also set not solid
		if (upgEntityInBoundingBox(entity2)) {
			entity2->setSolidType(SOLID_NOT);
			entity2->_makeSolidASAP = true;
			entity2->_makeSolidASAPTime = 0.0f;
			return true;
		}
	}
	return false;
}


//================================================================
// Name:        upgEntityInBoundingBox
// Class:       Entity
//              
// Description:  checks if given Entity is inside of the boundingbox
//              
// Parameters:  Entity *eIntruder 
//              
// Returns:     bool
//              
//================================================================
bool Entity::upgEntityInBoundingBox(Entity* eIntruder)
{
	if (!eIntruder || eIntruder == this ||
		(eIntruder->absmin[0] > this->absmax[0]) ||
		(eIntruder->absmin[1] > this->absmax[1]) ||
		(eIntruder->absmin[2] > this->absmax[2]) ||
		(eIntruder->absmax[0] < this->absmin[0]) ||
		(eIntruder->absmax[1] < this->absmin[1]) ||
		(eIntruder->absmax[2] < this->absmin[2]))
	{
		return false;
	}
	return true;
}