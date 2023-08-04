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


//================================================================
// Name:        upgActorSetHeadWatchTarget
// Class:       Actor
//              
// Description: returns player Actor should be watching when near and idle
//              
// Parameters:  str
//              
// Returns:     Player*           
//================================================================
Player* Actor::upgActorSetHeadWatchTarget(str watchTarget)
{
	int iStart = upgStrings.containsAt(watchTarget.tolower(), "player");
	str sPlayerNumber = upgStrings.getSubStr(watchTarget.tolower(), iStart, watchTarget.length());
	Player* player = GetPlayer(atoi(sPlayerNumber));
	//get the closest player if the given player did not exist
	if (!player) {
		player = coop_returnPlayerClosestTo((Entity*)this);
	}
	return player;
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