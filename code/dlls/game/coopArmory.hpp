//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER WEAPON/ARMORY RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#ifndef __COOPARMORY_HPP__
#define __COOPARMORY_HPP__
#include "_pch_cpp.h"

void coop_armoryRememberLastUsed( Player *player , str sWeapon );
void coop_armoryEquipPlayer( Player *player );
str coop_armoryReturnWeaponName( str sTikiName );
str coop_armoryReturnWeaponTiki( str sWeaponName );

#endif


