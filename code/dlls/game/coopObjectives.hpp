//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING OBJECTIVES RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"

void coop_objectivesStorySet(Player* player);
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