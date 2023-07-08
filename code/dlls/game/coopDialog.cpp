//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING DIALOG HANDLING RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#include "_pch_cpp.h"

#include "coopDialog.hpp"
#include "coopReturn.hpp"
#include "coopCheck.hpp"

#include "entity.h"
#include "player.h"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include "interpreter.h"
#include "program.h"
#include "trigger.h"


//================================================================
// Name:        coop_dialogSetupPlayerAll
// Class:       -
//              
// Description: setup dialog on player, set headhud entity and text for the dialog
//              
// Parameters:  Actor *speaker , char localizedDialogName[MAX_QPATH] , bool headDisplay
//              
// Returns:     void
//              
//================================================================
void coop_dialogSetupPlayerAll( Actor *speaker , char localizedDialogName[MAX_QPATH] , bool headDisplay )
{
	int i;
	Entity *player;
	Player *playerPlayer;

	for ( i = 0; i < maxclients->integer; i++ ){
		player = g_entities[i].entity;
		if ( player && player->client && player->isSubclassOf( Player ) ){
			playerPlayer = ( Player * )player;
			if ( headDisplay && speaker /* && playerplayer->coop_getInstalled() == 1 */ ){
				//gi.Printf("coop_dialogSetupPlayerAll speaker exists (head)\n");
				playerPlayer->SetupDialog( (Entity *)speaker , localizedDialogName );
			}else{
				//gi.Printf( "coop_dialogSetupPlayerAll speaker not found (no head)\n" );
				playerPlayer->SetupDialog( NULL , localizedDialogName );
			}
		}
	}
}


void coop_dialogSetupPlayerAll( Entity *speaker , char localizedDialogName[MAX_QPATH] , bool headDisplay )
{
	int i;
	Entity *player;
	Player *playerPlayer;

	for ( i = 0; i < maxclients->integer; i++ ){
		player = g_entities[i].entity;
		if ( player && player->client && player->isSubclassOf( Player ) ){
			playerPlayer = ( Player * )player;
			if ( headDisplay && speaker ){
				playerPlayer->SetupDialog( speaker , localizedDialogName );
			}else{
				playerPlayer->SetupDialog( NULL , localizedDialogName );
			}
		}
	}
}


//================================================================
// Name:        coop_dialogResetPlayerAll
// Class:       -
//              
// Description: stops/resets dialog on player
//              
// Parameters:  Actor *speaker , char localizedDialogName[MAX_QPATH] , bool headDisplay
//              
// Returns:     void
//              
//================================================================
void coop_dialogResetPlayerAll( void )
{
	int i;
	Entity *player;
	Player *playerPlayer;

	for ( i = 0; i < maxclients->integer; i++ )
	{
		player = g_entities[i].entity;
		if ( player && player->client && player->isSubclassOf( Player ) )
		{
			playerPlayer = ( Player * )player;
			playerPlayer->ClearDialog();
		}
	}
}





