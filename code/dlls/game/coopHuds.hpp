//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER HUD RELATED FUNCTIONS, MANAGE HUDS AND HUD/TEXT DISPLAYING ON PLAYER

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]


#ifndef __COOPHUDS_HPP__
#define __COOPHUDS_HPP__
#include "_pch_cpp.h"

void coop_huds_callvoteOptionChangedUI(str sText, str sValue, str sWidget); //[b607] coop callvote options menu
void coop_hudsAdd( Player *player , str hudName );
void coop_hudsAddCoop( Player *player , str hudName );
void coop_hudsRemove( Player *player , str hudName );
void coop_hudsUpdateNotification( Player *player );
void coop_huds_setupMultiplayerUI(Player *player);
void coop_hudsKilled(Player *player);

#endif /* coopHuds.hpp */