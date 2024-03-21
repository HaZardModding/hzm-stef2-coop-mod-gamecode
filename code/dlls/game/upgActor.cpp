//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// Contains Actor related code for the gameupgrades
//-----------------------------------------------------------------------------------
#include "upgActor.hpp"
UpgActor upgActor;

#include "upgStrings.hpp"

#include "coopReturn.hpp"

#include "mp_manager.hpp"
extern Event EV_Actor_SetFollowTarget;


//================================================================
// Name:        upgActorUsedByEquipment
// Class:       Actor
//              
// Description: Handles actor being used (by player equipment)
//              
// Parameters:  Entity
//              
// Returns:     void           
//================================================================
void Actor::upgActorUsedByEquipment(Entity* entityEquipment)
{
}

//================================================================
// Name:        upgActorFollowPlayer
// Class:       Actor
//              
// Description: Handles actor following a player in multiplayer
//              
// Parameters:  str
//              
// Returns:     void           
//================================================================
void Actor::upgActorFollowPlayer()
{
	if (!multiplayerManager.inMultiplayer() || !activator || !activator->isSubclassOf(Player) || !GetActorFlag(ACTOR_FLAG_AI_ON) || personality->GetTendency("follow") < 1.0f) {
		return;
	}
	multiplayerManager.HUDPrint(activator->entnum, "^5INFO^8: Teammate is now following you!\n");
	Event* followThisPlayer;
	followThisPlayer = new Event(EV_Actor_SetFollowTarget);
	followThisPlayer->AddEntity(activator);
	ProcessEvent(followThisPlayer);	
}

//================================================================
// Name:        getUsedByPlayer
// Class:       UpgActor
//              
// Description: gets if actor was used by a player imidiatly before playing a dialog
//              
// Parameters:  str
//              
// Returns:     void           
//================================================================
bool UpgActor::getUsedByPlayer(Actor* actor)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		return false;
	}
	return upgActor.usedByPlayer;
}

//================================================================
// Name:        setUsedByPlayer
// Class:       UpgActor
//              
// Description: sets if actor was used by a player imidiatly before playing a dialog
//              
// Parameters:  str
//              
// Returns:     void           
//================================================================
void UpgActor::setUsedByPlayer(Actor* actor, bool bUsed)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		return;
	}
	upgActor.usedByPlayer = bUsed;
}

//================================================================
// Name:        setHeadWatchTarget
// Class:       UpgActor
//              
// Description: returns player Actor should be watching when near and idle
//              
// Parameters:  Entity*, str
//              
// Returns:     Player*           
//================================================================
int  UpgActor::setHeadWatchTarget(Entity* entity,str watchTarget)
{
	int iStart = upgStrings.containsAt(watchTarget.tolower(), "player");
	str sPlayerNumber = upgStrings.getSubStr(watchTarget.tolower(), iStart, watchTarget.length());
	Player* player = GetPlayer(atoi(sPlayerNumber));
	//get the closest player if the given player did not exist
	if (!player) {
		player = coop_returnPlayerClosestTo(entity);
	}
	if (player) {
		return player->entnum;
	}
	return 0;
}

//================================================================
// Name:        upgActorGrabValidFollowTarget
// Class:       Actor
//              
// Description: makes sure actor is following a valid target
//              
// Parameters:  void
//              
// Returns:     void         
//================================================================
void Actor::upgActorGrabValidFollowTarget()
{
	if (!multiplayerManager.inMultiplayer()) {
		return;
	}

	if (!followTarget.specifiedFollowTarget ||
		followTarget.specifiedFollowTarget && followTarget.specifiedFollowTarget->isSubclassOf(Player) && multiplayerManager.isPlayerSpectator((Player*)(Entity*)followTarget.specifiedFollowTarget))
	{
		followTarget.specifiedFollowTarget = coop_returnPlayerClosestTo((Entity*)this);
	}
}