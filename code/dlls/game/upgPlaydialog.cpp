//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
//  GAMEUPGRADE code for Actor Playdialog (when a actor talks) in multiplayer
//-----------------------------------------------------------------------------------
#include "upgPlaydialog.hpp"
UpgPlayDialog upgPlayDialog;

//Related to
//void Actor::PlayDialog(Sentient* user, float volume, float min_dist, const char* dialog_name, const char* state_name, qboolean headDisplay, bool useTalk, bool important)


//================================================================
// Name:        dialogSetupPlayers
// Class:       UpgPlayDialog
//              
// Description: setup dialog on player, set headhud entity and text for the dialog
//              
// Parameters:  Actor *speaker , char localizedDialogName[MAX_QPATH] , bool headDisplay
//              
// Returns:     void
//              
//================================================================
void UpgPlayDialog::dialogSetupPlayers(Entity* speaker, char localizedDialogName[MAX_QPATH], bool headDisplay)
{
	Entity* player;
	Player* playerPlayer;
	Entity* eActor = nullptr;

	if (headDisplay && speaker) {
		eActor = speaker;
	}

	for (int i = 0; i < maxclients->integer; i++) {
		player = g_entities[i].entity;
		if (player && player->client && player->isSubclassOf(Player)) {
			playerPlayer = (Player*)player;
			playerPlayer->SetupDialog(eActor, localizedDialogName);
		}
	}
}


//================================================================
// Name:        resetDialogPlayers
// Class:       UpgPlayDialog
//              
// Description: stops/resets dialog on player
//              
// Parameters:  Actor *speaker , char localizedDialogName[MAX_QPATH] , bool headDisplay
//              
// Returns:     void
//              
//================================================================
void UpgPlayDialog::dialogResetPlayers(void)
{
	Entity* player;
	Player* playerPlayer;

	for (int i = 0; i < maxclients->integer; i++){
		player = g_entities[i].entity;
		if (player && player->client && player->isSubclassOf(Player)){
			playerPlayer = (Player*)player;
			playerPlayer->ClearDialog();
		}
	}
}
