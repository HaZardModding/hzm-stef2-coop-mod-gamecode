//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// NEWLY CREATED CODE
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING RADAR-FUNCTIONS, MANAGING OBJECTIVE RADAR

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#ifndef __COOPRADAR_HPP__
#define __COOPRADAR_HPP__
#include "_pch_cpp.h"

void coop_radarReset(Player* player);
float coop_radarAngleTo( const Vector& source , const Vector& dest );
float coop_radarSignedAngleTo( const Vector& source , const Vector& dest , const Vector& planeNormal );
void coop_radarUpdate( Player *player );

#endif
