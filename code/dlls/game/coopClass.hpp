//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER CLASS(Medic,Technician,HeavyWeapons) HANDLING RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#ifndef __COOPCLASS_HPP__
#define __COOPCLASS_HPP__

//[b60011] chrissstrahl - have the classnames fixed
#define COOP_CLASS_NAME_TECHNICIAN "Technician"
#define COOP_CLASS_NAME_MEDIC "Medic"
#define COOP_CLASS_NAME_HEAVYWEAPONS "HeavyWeapons"

#define COOP_CLASS_HURT_WAITTIME 6
#define COOP_CLASS_REGENERATE_HEALTH 3
#define COOP_CLASS_REGENERATE_ARMOR 5
#define COOP_CLASS_REGENERATE_AMMO 3

#define COOP_CLASS_MEDIC_MAX_HEALTH 110.0f
#define COOP_CLASS_TECHNICIAN_MAX_HEALTH 100.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_HEALTH 120.0f

#define COOP_CLASS_MEDIC_START_ARMOR 25.0f
#define COOP_CLASS_TECHNICIAN_START_ARMOR 100.0f
#define COOP_CLASS_HEAVYWEAPONS_START_ARMOR 50.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_PHASER 400.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_PHASER 400.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_PHASER 1000.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_FED 250.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_FED 300.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_FED 500.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_PLASMA 250.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_PLASMA 250.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_PLASMA 50.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_IDRYLLUM 250.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_IDRYLLUM 300.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_IDRYLLUM 500.0f

#define COOP_CLASS_MEDIC_GRAVITY 0.85f
#define COOP_CLASS_TECHNICIAN_GRAVITY 0.9f
#define COOP_CLASS_HEAVYWEAPONS_GRAVITY 1.0f

#define COOP_CLASS_MEDIC_MASS 270
#define COOP_CLASS_TECHNICIAN_MASS 270
#define COOP_CLASS_HEAVYWEAPONS_MASS 350

#define COOP_REVIVE_TIME 100 //was 400
#define COOP_MAX_ARMOR_TO_GIVE 200
#define COOP_MAX_AMMO_TO_GIVE_PLASMA 800
#define COOP_MAX_AMMO_TO_GIVE_IDRYLL 800
#define COOP_MAX_AMMO_TO_GIVE_FED 800

#include "_pch_cpp.h"
//#include "equipment.h"

void coop_classCheckApplay( Player *player );
void coop_classCheckUpdateStat( Player *player );
void coop_classRegenerate(Player *player);
//void coop_classNotifyOfInjured( Player *player );
//void coop_classUpdateHealthStatFor( int iPlayerNum );
void coop_classSet( Player *player , str classToSet );
void coop_classApplayAttributes( Player *player , bool changeOnly );
void coop_classPlayerUsed( Player *usedPlayer , Player *usingPlayer , Equipment *equipment );
int coop_classPlayersOfClass( str className );
void coop_classUpdateClassStats( void );

#endif /* coopClass */



