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

#include "_pch_cpp.h"
//#include "equipment.h"

void coop_classCheckApplay( Player *player );
void coop_classCeckUpdateStat( Player *player );
void coop_classRegenerate(Player *player);
//void coop_classNotifyOfInjured( Player *player );
//void coop_classUpdateHealthStatFor( int iPlayerNum );
void coop_classSet( Player *player , str classToSet );
void coop_classApplayAttributes( Player *player , bool changeOnly );
void coop_classPlayerUsed( Player *usedPlayer , Player *usingPlayer , Equipment *equipment );
int coop_classPlayersOfClass( str className );
void coop_classUpdateClassStats( void );

#endif /* coopClass */



