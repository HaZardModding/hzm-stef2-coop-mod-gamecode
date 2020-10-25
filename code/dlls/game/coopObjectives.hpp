//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING OBJECTIVES RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#ifndef __COOPOBJECTIVES_HPP__
#define __COOPOBJECTIVES_HPP__

#include "_pch_cpp.h"

void coop_objectivesMarkerUpdate( void );
void coop_objectivesMarker( Entity *eMaster );
void coop_objectivesMarkerRemove( Entity *eMaster );
void coop_objectivesUpdateUservar( int iUservar );
void coop_objectivesNotify( Player* player );
void coop_objectivesUpdatePlayer( Player* player );
void coop_objectivesSetup(Player *player);
void coop_objectivesUpdate( str sObjectiveState , float fObjectiveItem , float fObjectiveShow );
void coop_objectivesUpdate( str sObjectiveState , str sObjectiveItem , str sObjectiveShow );
void coop_objectivesShow( Player *player , int iObjectiveItem , int iObjectiveState , bool bObjectiveShow );
void coop_objectivesSingleplayer( str sObjectiveState , int iObjectiveNumber , int iShowNow );
void coop_objectives_tacticalShow( Player *player , int iTactical );
#endif /* coopObjectives.hpp */