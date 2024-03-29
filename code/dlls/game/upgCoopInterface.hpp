//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// USED AS A INTERFACE BETWEEN THE GAMEUPGRADE CODE AND THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#ifndef __UPGCOOPINTERFACE_HPP__
#define __UPGCOOPINTERFACE_HPP__

#include "upgPlayer.hpp"
#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"
#include "upgStrings.hpp"

class UpgCoopInterface
{
private:

public:
	bool				upgCoopInterfaceDiagnoseRunning();
	bool				isCoopActive();
	bool				playerHasCoop(Player* player);
	Player*				playerClosestTo(Entity* ent);
};

extern UpgCoopInterface upgCoopInterface;

#endif /* upgCoopInterface.hpp */