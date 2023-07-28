//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
//  GAMEUPGRADE code for dialog stuff in multiplayer
//-----------------------------------------------------------------------------------
#include "upgBranchdialog.hpp"



UpgBranchDialog upgBranchDialog;


void UpgBranchDialog::failsave()
{
	//hzm gameupdate chrissstrahl - CHECK IF branchdialog failsafe SHOULD BE TRIGGERED
	if (g_gametype->integer != GT_SINGLE_PLAYER && game.branchdialog_selectionActive){
		//hzm gameupdate chrissstrahl - failsafe triggered on timeout and other events
		if ((game.branchdialog_startingTime + game.branchdialog_timeout) < level.time) {
			if (game.branchdialog_name.length()) {
				ExecuteThread(va("%s_failsafe", game.branchdialog_name.c_str()));

				//[b608] Chrissstrahl - handle activator stuff
				if (game.branchdialog_chosenPlayer && game.branchdialog_chosenPlayer->isSubclassOf(Player)) {
					Player* player = (Player*)(Entity*)game.branchdialog_chosenPlayer;
					gi.SendServerCommand(player->entnum, "stufftext popmenu branchdialog 1\n");
					player->branchdialog_active = false;
				}

				game.branchdialog_chosenPlayer = NULL; //[b608] chrissstrahl - used to store player that is valid to select the dialog
				game.branchdialog_selectionActive = false;
				game.branchdialog_name = "";
			}
		}
	}
}