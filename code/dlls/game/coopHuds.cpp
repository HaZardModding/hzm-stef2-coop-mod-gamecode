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
			if (player->coopPlayer.installed) {
				DelayedServerCommand(player->entnum, va("globalwidgetcommand %s title %s", sWidget.c_str(), sValue.c_str()));
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
	DelayedServerCommand(player->entnum, "globalwidgetcommand dmTimer disable");
	DelayedServerCommand(player->entnum, "ui_addhud mp_console"); 

	//handle all the coop specific huds
	if (player->coopPlayer.installed) {
		DelayedServerCommand(player->entnum, "ui_addhud coop_radarhud");
		DelayedServerCommand(player->entnum, "globalwidgetcommand crs disable");
	}
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
	if (!player->coopPlayer.installed) {
		DelayedServerCommand(player->entnum, "ui_removehud targetedhud"); //sniper red dot when being targeted
	}
	//coop huds are handled in the cfg file
	else {
		DelayedServerCommand(player->entnum, "exec coop_mod/cfg/sod.cfg");
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
		DelayedServerCommand( player->entnum , va( "ui_removehud %s" , hudName.c_str() ) );
		DelayedServerCommand( player->entnum , va( "ui_addhud %s" , hudName.c_str() ) );
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
		if ( player->coopPlayer.installed == 1 ){
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
		DelayedServerCommand( player->entnum , va( "ui_removehud %s" , hudName.c_str() ) );
	}
}

//hzm coop mod chrissstrahl - add coop related huds, updatenotification
void coop_hudsUpdateNotification( Player *player )
{
	if ( player->coopPlayer.installed ) {
		if ( player->coopPlayer.installedVersion < COOP_BUILD ) {
			if ( !player->coopPlayer.updateHudDisplayed ) {
				if ( ( level.time + 20 ) > mp_warmUpTime->integer &&
					level.cinematic != true &&
					level.mission_failed != true &&
					level.playerfrozen != true &&
					player->coopPlayer.setupComplete != false )
				{
					player->coopPlayer.updateHudDisplayed = true;
					player->hudPrint( va( "^5Coop info^8: Please ^5update^8 the HZM Coop Mod!\nYour version:^3 %d ^8- server version:^5 %d\n" , player->coopPlayer.installedVersion , COOP_BUILD ) );
				}
			}
		}
	}
}

//add huds that have been specified for this mission or map only
//$world.getStringvar( "coop_registredHud" + fIndex );
//cvar_bool_coop_disClass = getCvarInt( "coop_disClass" );
//$world.setFloatVar("globalCoop_disableBuildTransporter",1);//disable technican class teleporter build ability

