//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING TEXT AND LOCALIZING RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "mp_manager.hpp"

#include "upgStrings.hpp"

#include "coopText.hpp"
#include "coopCheck.hpp"
#include "coopReturn.hpp"

//================================================================
// Name:        coop_textJoinCommands
// Class:       -
//              
// Description:	Joins two console commands to one
//              
// Parameters:  str sCmd1 , str SCmd2
//              
// Returns:     str
//              
//================================================================
str coop_textJoinCommands( str sCmd1 , str sCmd2 )
{
	bool bEmpty=false;
	str s1="" , s2="";

	if ( !sCmd1 || strlen( sCmd1 ) < 1 )bEmpty = true;
	else s1 = sCmd1;

	if ( !sCmd2 || strlen( sCmd2 ) < 1 )bEmpty = true;
	else s2 = sCmd2;
	
	sCmd1 = upgStrings.getStartingFromUntil( sCmd1 , 0 , COOP_JOIN_MAX_SINGLE_COMMAND );
	sCmd2 = upgStrings.getStartingFromUntil( sCmd2 , 0 , COOP_JOIN_MAX_SINGLE_COMMAND );

	if ( bEmpty ){
		gi.Printf("coop_textJoinCommands: WARNING: empty command joined\n");
	}else{
		s1 += ";";
		s1 += s2;	
	}

	return s1;
}

//================================================================
// Name:        coop_textCinematicHudprint
// Class:       -
//              
// Description: Prints text to player hud, unless there is a cinematic
//				prints hud to predefined loctaion during cinematic
//
// Parameters:  Player *player , str sText 
//              
// Returns:     void
//              
//================================================================
void coop_textCinematicHudprint( Player *player , str sText )
{
	if ( !level.cinematic && multiplayerManager.inMultiplayer()){
		player->hudPrint( sText.c_str() );
	}
	else{
		sText = upgStrings.getReplacedSpaceWithUnderscore( sText.c_str() );
		upgPlayerDelayedServerCommand( player->entnum , va( "locationprint 220 455 %s 0.8" , sText.c_str() ) );
	}
}

//================================================================
// Name:        coop_textHudprint
// Class:       -
//              
// Description: (SP/MP) this is our reliable Hudprint function
//				The text will always be shown to the player             
//
// Parameters:  Player *player , str sText 
//              
// Returns:     void
//              
//================================================================
void coop_textHudprint( Player *player , str sText )
{
	if ( !sText.length() ){
		return;
	}

	str command;
	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		gentity_t *gentity;
		gentity = &g_entities[0];
		if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) ){
			command = sText;
			command += "\n";
			gi.centerprintf( gentity , CENTERPRINT_IMPORTANCE_CRITICAL , command.c_str() );
		}
	}
	else if( player && !multiplayerManager.inMultiplayer()){
		player->hudPrint( sText.c_str() );
	}
}

//================================================================
// Name:        coop_textHudprintAll
// Class:       -
//              
// Description: (SP/MP) this is our Hudprint function, works in SP and MP
//              
// Parameters:  str messageToShow
//              
// Returns:     void
//              
//================================================================
void coop_textHudprintAll( str messageToShow )
{
	if ( !messageToShow.length() ){
		return;
	}

	int i;
	gentity_t *gentity;
	Player *player = NULL;
	str command;

	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		gentity = &g_entities[0];
		if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) ){


			command = messageToShow;
			command += "\n";
			gi.centerprintf( gentity , CENTERPRINT_IMPORTANCE_CRITICAL , command.c_str() );
		}
		return;
	}

	for ( i = 0; i < maxclients->integer; i++ ){
		gentity = &g_entities[i];
		if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) ){
			player = ( Player * )gentity->entity;
			player->hudPrint( messageToShow.c_str() );
		}
	}
}