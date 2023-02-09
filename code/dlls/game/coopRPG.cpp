//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING RPG LEVEL RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "coopReturn.hpp"
#include "coopArmory.hpp"
#include "coopObjectives.hpp"
#include "coopParser.hpp"
#include "coopScripting.hpp"

#include "coopRPG.hpp"
CoopRPG coopRpg;

//[b60013] chrissstrahl - set/reset vars upon each map load
void CoopRPG::init()
{
	//disable all challenges
	coopChallenges.disabled(true);
}