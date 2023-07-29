//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER HUD RELATED FUNCTIONS, MANAGE HUDS AND HUD/TEXT DISPLAYING ON PLAYER
//-----------------------------------------------------------------------------------
#include "_pch_cpp.h"

#include "coopHuds.hpp"
#include "coopReturn.hpp"
#include "coopPlayer.hpp"

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

//========================================================= [b607]
// Name:        coop_huds_callvoteOptionChangedUI
// Class:       -
//              
// Description: Manage UI for the player during coop (a coop option/setting has changed)
//              
// Parameters:  str sText, str sValue, str sWidget
//              
// Returns:     VOID
//              
//================================================================
void coop_huds_callvoteOptionChangedUI(str sText, str sValue, str sWidget)
{
	if (!game.coop_isActive) {
		return;
	}

	//hzm coop mod chrissstrahl - send updated data to player UI
	Player *player = NULL;
	for (int i = 0; i < maxclients->integer; i++){
		player = (Player*)g_entities[i].entity;
		if (player && player->client && player->isSubclassOf(Player)) {
			multiplayerManager.HUDPrint(player->entnum, va("^5INFO^8: %s set to^5 %s\n", sText.c_str(), sValue.c_str()));
			if (player->coop_getInstalled()) {
				upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand %s title %s", sWidget.c_str(), sValue.c_str()));
			}
		}
	}
}

//========================================================= [b607]
// Name:        coop_huds_setupMultiplayerUI
// Class:       -
//              
// Description: Manage UI for the player during coop
//              
// Parameters:  Player *player
//              
// Returns:     VOID
//              
//================================================================
void coop_huds_setupMultiplayerUI(Player *player) //chrissstrahl - added [b607]
{
	//[b607] chrissstrahl - make sure we do not handle bots
	gentity_t *ent = player->edict;
	if (ent->svflags & SVF_BOT) {
		return;
	}

	gi.SendServerCommand(player->entnum, "stufftext \"ui_removehuds\"\n");

	//use delayed server command so it will not be added to soon and then removed from the command above
	//issues often occur on a listen server
	upgPlayerDelayedServerCommand(player->entnum, "globalwidgetcommand dmTimer disable");
	upgPlayerDelayedServerCommand(player->entnum, "ui_addhud mp_console");
}

//========================================================= [b607]
// Name:        coop_hudsKilled
// Class:       -
//              
// Description: Manage UI for the player during coop when killed
//              
// Parameters:  Player *player
//              
// Returns:     VOID
//              
//================================================================
void coop_hudsKilled(Player *player) //chrissstrahl - added [b607]
{
	//if player does not have the coop mod installed handle [b607]
	if (!player->coop_getInstalled()) {
		upgPlayerDelayedServerCommand(player->entnum, "ui_removehud targetedhud"); //sniper red dot when being targeted
	}
	//coop huds are handled in the cfg file
	else {
		upgPlayerDelayedServerCommand(player->entnum, "exec coop_mod/cfg/sod.cfg");
	}
}

//================================================================
// Name:        coop_hudsAdd
// Class:       -
//              
// Description: Adds hud to player
//              
// Parameters:  Player *player , str hudName
//              
// Returns:     VOID
//              
//================================================================
void coop_hudsAdd( Player *player , str hudName )
{
	if ( player && strlen( hudName ) > 0 )
	{
		upgPlayerDelayedServerCommand( player->entnum , va( "ui_removehud %s" , hudName.c_str() ) );
		upgPlayerDelayedServerCommand( player->entnum , va( "ui_addhud %s" , hudName.c_str() ) );
	}
}

//================================================================
// Name:        coop_hudsAddCoop
// Class:       -
//              
// Description: Adds hud to player if the player has the coop mod installed
//              
// Parameters:  Player *player , str hudName
//              
// Returns:     VOID
//              
//================================================================
void coop_hudsAddCoop( Player *player , str hudName )
{
	if ( player && strlen(hudName) > 0 ){
		if ( player->coop_getInstalled() == 1 ){
			coop_hudsAdd( player , hudName );
		}
	}
}

//================================================================
// Name:        coop_hudsRemove
// Class:       -
//              
// Description: Removes hud from player
//              
// Parameters:  Player *player , str hudName
//              
// Returns:     VOID
//              
//================================================================
void coop_hudsRemove( Player *player , str hudName )
{
	if ( player && strlen( hudName ) > 0 )
	{
		upgPlayerDelayedServerCommand( player->entnum , va( "ui_removehud %s" , hudName.c_str() ) );
	}
}

//add huds that have been specified for this mission or map only
//$world.getStringvar( "coop_registredHud" + fIndex );
//cvar_bool_coop_disClass = getCvarInt( "coop_disClass" );
//$world.setFloatVar("globalCoop_disableBuildTransporter",1);//disable technican class teleporter build ability

