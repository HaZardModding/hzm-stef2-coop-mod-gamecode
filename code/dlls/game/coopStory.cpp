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

#include "_pch_cpp.h"

#include "coopCheck.hpp"
#include "coopStory.hpp"
#include "coopPlayer.hpp"
#include "coopText.hpp"
#include "coopReturn.hpp"

#include "upgPlayer.hpp"
#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"

#include "entity.h"
#include "player.h"
#include "weapon.h"
#include "mp_manager.hpp"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include "powerups.h"
#include "interpreter.h"
#include "program.h"


//================================================================
// Name:        coop_storySet
// Class:       -
//              
// Description: Adds hud to player
//              
// Parameters:  Player *player , str hudName
//              
// Returns:     VOID
//              
//================================================================
void coop_storySet( Player *player )
{
	if ( !player ){
		return;
	}

	//send story if it is set by script
	if ( !game.coop_story.length() ){
		game.coop_story = program.getStringVariableValue( "coop_string_story" );
	}
	if ( !game.coop_story_deu.length() ){
		game.coop_story_deu = program.getStringVariableValue( "coop_string_story_deu" );
	}
	//get localized story
	str sStory = "";
	if (coop_checkPlayerLanguageGerman(player) && game.coop_story_deu.length()){
		sStory = game.coop_story_deu;
	}
	//if that failed or player has not a german version, set english story
	if ( !sStory.length() && game.coop_story.length()){
		sStory = game.coop_story;
	}
	//if that failed set empty
	if ( !sStory.length() ){
		sStory = "$$Empty$$";
	}
	//send story
	DelayedServerCommand( player->entnum , va( "set coop_story %s" , sStory.c_str() ) );
}

