//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING CHECK-FUNCTIONS, RETURNING EIGTHER FALSE OR TRUE
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"

//check if player has german game version
bool coop_checkDeadBodiesStayDisabled();
bool coop_checkCharsInsideString( str sSource , str sObjectsOfIntrest );
str coop_checkPlayerCoopIdExistInIni(Player* player, str sClientId);
bool coop_checkStringInUservarsOf( Entity * ent , str sStringPart );
bool coop_checkEntityInsideDoor( Entity *entity1 );
bool coop_checkIsPlayerActiveAliveInBoundingBox( Player *player , Entity *eTheBox );
bool coop_checkCanPickUpItem(Entity *entity, str sModel );


