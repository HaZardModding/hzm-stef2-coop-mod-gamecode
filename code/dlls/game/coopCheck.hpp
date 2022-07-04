//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING CHECK-FUNCTIONS, RETURNING EIGTHER FALSE OR TRUE

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#ifndef __COOPCHECK_HPP__
#define __COOPCHECK_HPP__
#include "_pch_cpp.h"

//check if player has german game version
bool coop_checkPlayerLanguageGerman(Player *player);
bool coop_checkCharsInsideString( str sSource , str sObjectsOfIntrest );
bool coop_checkInsidePlayerOrActor( Entity *entity1 );
void coop_checkDoesPlayerHaveCoopMod( Player *player );
void coop_checkDoesPlayerHaveCoopId( Player *player );
str coop_checkPlayerCoopIdExistInIni(Player* player, str sClientId);
bool coop_checkStringInUservarsOf( Entity * ent , str sStringPart );
bool coop_checkIsEntityInBoundingBox( Entity *eIntruder , Entity *eTheBox );
bool coop_checkEntityInsideDoor( Entity *entity1 );
bool coop_checkIsPlayerActiveAliveInBoundingBox( Player *player , Entity *eTheBox );
#endif


