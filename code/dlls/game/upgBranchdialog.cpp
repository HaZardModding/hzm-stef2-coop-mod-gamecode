//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
//  GAMEUPGRADE code for branch dialog (player selecting from presented options) stuff in multiplayer
//-----------------------------------------------------------------------------------
#include "upgBranchdialog.hpp"
UpgBranchDialog upgBranchDialog;

#include "mp_manager.hpp"

#include "upgCoopInterface.hpp"


extern Event EV_Actor_upgBranchDialogFailsafe;
extern qboolean G_DialogRunThread(const gentity_t* ent);
extern qboolean G_ClientRunThreadCmd(const gentity_t* ent);

/* //Related functions:
G_DialogRunThread
Actor::setBranchDialog
Actor::clearBranchDialog
Actor::BranchDialog
Player::setBranchDialogActor
Player::clearBranchDialogActor */

//=========================================================[b60014]
// Name:        runThread
// Class:       UpgBranchDialog
//              
// Description: Executed from G_DialogRunThread
//              
// Parameters:  Actor* actor, str sDialogName
//              
// Returns:     void
//================================================================
void UpgBranchDialog::runThread(Entity* ent)
{
	if (ent && ent->isSubclassOf(Player)) {
		Player* player = (Player*)ent;
		Actor* actor = (Actor*)player->upgBranchDialogGetActor();
		if (actor) {
			str sGivenThread = "";
			str sDialogName = actor->upgBranchDialogGetName();
			if (gi.argc()) {
				sGivenThread = gi.argv(1);
			}
			if (sGivenThread.length() && sDialogName.length()) {
				if (upgStrings.containsAt(sGivenThread, "Choice") != -1 ||
					upgStrings.containsAt(sGivenThread, "_DialogChoice") != -1 ||
					upgStrings.containsAt(sGivenThread, "Option") != -1 ||
					upgStrings.containsAt(sGivenThread, "cinematicArm") != -1 ||
					upgStrings.containsAt(sGivenThread, "failedBranch") != -1 ||
					upgStrings.containsAt(sGivenThread, "successBranch") != -1
					)
				{
					player->clearBranchDialogActor();
					G_ClientRunThreadCmd(&g_entities[player->entnum]);
				}
			}
		}
	}
}

//=========================================================[b60014]
// Name:        setBranchDialog
// Class:       UpgBranchDialog
//              
// Description: Executed when a BranchDialog is started
//              
// Parameters:  Actor* actor, str sDialogName
//              
// Returns:     void
//================================================================
void UpgBranchDialog::setBranchDialog(Actor* actor, str sDialogName)
{
	if (!actor || !sDialogName.length()) { return; }
	
	//cancel any old failsafes
	actor->CancelEventsOfType(EV_Actor_upgBranchDialogFailsafe);

	//grab activating player, if that fails grab closest viable player
	Player* player = nullptr;
	Entity* ent = (Entity*)actor->activator;
	if (ent && ent->isSubclassOf(Player)) {
		player = (Player*)ent;
	}else {
		player = upgCoopInterface.playerClosestTo((Entity*)actor);
		actor->activator = (Entity*)player;
	}
	if (!player) {
		gi.Printf("UpgBranchDialog::setBranchDialog - No player found, executing failsafe thread\n");
		ExecuteThread(va("%s_failsafe", sDialogName.c_str()));
		return;
	}

	//probably set for head hud display
	player->setBranchDialogActor(actor);
	
	//show menu to player
	upgPlayerDelayedServerCommand(player->entnum, va("displaybranchdialog %s", sDialogName.c_str()));
	upgPlayerDelayedServerCommand(player->entnum, "pushmenu branchdialog");

	//Actor post failsave event
	Event* e = new Event(EV_Actor_upgBranchDialogFailsafe);
	e->AddEntity((Entity*)player); //player that did talk
	e->AddString(va("%s_failsafe", sDialogName.c_str())); //thread to execute on failure
	actor->PostEvent(e,20.0f);
}

//=========================================================[b60014]
// Name:        upgBranchDialogFailsafe
// Class:       Actor
//              
// Description: Failsave event started with a delay when a dialog is started
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
void Actor::upgBranchDialogFailsafe(Event* ev)
{
	if (_branchDialogName.length()) {
		ExecuteThread(va("%s_failsafe", _branchDialogName.c_str()));
	}

	Entity* ent = (Entity*)activator;
	if (ent && ent->isSubclassOf(Player)) {
		Player* player = (Player*)ent;
		player->clearBranchDialogActor();
		gi.SendServerCommand(ent->entnum, "stufftext popmenu branchdialog 1\n");
	}

	//cancel any old failsafes
	CancelEventsOfType(EV_Actor_upgBranchDialogFailsafe);
	clearBranchDialog();	
}

//=========================================================[b60014]
// Name:        upgBranchDialogCancelEvent
// Class:       Player
//              
// Description: Cancels Failsave Event
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
void Player::upgBranchDialogCancelEvent()
{
	//cancel any old failsafes
	if (multiplayerManager.inMultiplayer() && _branchDialogActor) {
		Actor* actor = (Actor*)_branchDialogActor;
		actor->CancelEventsOfType(EV_Actor_upgBranchDialogFailsafe);
	}
}


//=========================================================[b60014]
// Name:        upgBranchDialogGetName
// Class:       Actor
//              
// Description: Returns Branch Dialog Name from Actor
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
str Actor::upgBranchDialogGetName()
{
	return _branchDialogName;
}

//=========================================================[b60014]
// Name:        upgBranchDialogGetActor
// Class:       Player
//              
// Description: Returns Branch Dialog Actor from player
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
Actor* Player::upgBranchDialogGetActor()
{
	return (Actor*)_branchDialogActor;
}