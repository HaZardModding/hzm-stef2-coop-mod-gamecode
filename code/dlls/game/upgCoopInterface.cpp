//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// USED AS A INTERFACE BETWEEN THE GAMEUPGRADE CODE AND THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"

#include "upgCoopInterface.hpp"

#include "coopCheck.hpp"
#include "coopReturn.hpp"
#include "coopServer.hpp"
#include "coopText.hpp"

UpgCoopInterface upgCoopInterface;

//================================================================
// Name:        upgCoopInterfaceDiagnoseRunning
// Class:       UpgCoopInterface
//              
// Description:	checks if the coop mod is currently running in diagone mode - basically loading every level to see if they error out
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool UpgCoopInterface::upgCoopInterfaceDiagnoseRunning()
{
	return coopServer.coopServerDiagoneRunning();
}

//================================================================
// Name:        isCoopActive
// Class:       UpgCoopInterface
//              
// Description:	checks if the coop mod is currently active
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool UpgCoopInterface::isCoopActive()
{
	return coopServer.coopIsActive();
}


//================================================================
// Name:        playerHasCoop
// Class:       UpgCoopInterface
//              
// Description:	checks if player has the coop mod
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool UpgCoopInterface::playerHasCoop(Player* player)
{
	if (player) {
		return player->coop_getInstalled();
	}
	return false;
}

//================================================================
// Name:        playerClosestTo
// Class:       UpgCoopInterface
//              
// Description:	checks if player has the coop mod
//              
// Parameters:  Entity*
//              
// Returns:     bool
//================================================================
Player* UpgCoopInterface::playerClosestTo(Entity* ent)
{
	if (!ent) {
		return nullptr;
	}
	return coop_returnPlayerClosestTo(ent);
}
