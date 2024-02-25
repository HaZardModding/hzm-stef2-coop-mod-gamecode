//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// PuzzleObject related Upgrades, Updates and Fixes
// This code should be independant from the Coop and be useable without the HZM Coop Mod
//-----------------------------------------------------------------------------------
#include "_pch_cpp.h"

#include "upgPuzzleObject.hpp"

#include "player.h"
#include "puzzleobject.hpp"
#include "powerups.h"
#include "mp_manager.hpp"
#include <qcommon/gameplaymanager.h>

#include "upgGame.hpp"
#include "upgPlayer.hpp"
#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"
#include "upgStrings.hpp"
#include "upgCoopInterface.hpp"
#include "upgWorld.hpp"

#include "coopClass.hpp"

extern Event EV_PuzzleObject_Canceled;


//[b60011] chrissstrahl - thread called when puzzle is started to be used
Event EV_PuzzleObject_SetUsedStartThread
(
	"puzzleobject_usedStartThread",
	EV_DEFAULT,
	"s",
	"threadname",
	"The thread to call when the puzzle is used, works on any puzzle type"
);

//[b60012] chrissstrahl - get last activating entity for this puzzle
Event EV_PuzzleObject_GetLastActivatingEntity
(
	"getLastActivatingEntity",
	EV_DEFAULT,
	"@e",
	"entity",
	"Returns last entity activating this puzzle"
);

//[b60021] chrissstrahl - reset last activator data - moved here to seperate code bases
//-----------------------------------------------------
void PuzzleObject::upgNullPlayer()
{
	activator = NULL;
	entityVars.SetVariable("_activator", -1.0f);
}

//[b60021] chrissstrahl - remove hud from player currently modulating
//-----------------------------------------------------
void PuzzleObject::upgPlayerResetHud()
{
	//[b60014] chrissstrahl - added fix to remove hud from player currently modulating
	if (activator && activator->isSubclassOf(Player) && _hudOn) {
		gi.SendServerCommand(activator->entnum, va("stufftext \"popmenu %s 1\"\n", _hudName.c_str()));
	}
}

//[b60012] chrissstrahl - I was so FUCKING tempted to call this cancelculture
//-----------------------------------------------------
void PuzzleObject::upgCancelPlayer(Player* player)
{
	if (!player) {
		return;
	}
	if (_hudOn) {
		hideTimerHud(player);
		_usedTime = 0.0f;

		if (_canceledThread.length())
		{
			ProcessEvent(EV_PuzzleObject_Canceled);
		}

		//switch to any weapon, avoids the issue that the player might start to modulate to early again
		Weapon* weap = player->BestWeapon();
		if (weap) {
			//player->useWeapon(weap, WEAPON_ANY);
			player->useWeapon(weap, WEAPON_DUAL);
		}
	}
}

//[b60012] chrissstrahl - return last activator
//-----------------------------------------------------
void PuzzleObject::upgGetLastActivatingEntity(Event* ev)
{
	ev->ReturnEntity(activator);
}

//[b60012] chrissstrahl - return last activator
//-----------------------------------------------------
EntityPtr PuzzleObject::upgGetLastActivatingEntity()
{
	return activator;
}

//[b60011] chrissstrahl - thread called when puzzle is started to be used
//-----------------------------------------------------
void PuzzleObject::upgSetUsedStartThread(Event* event)
{
	_usedStartThread = event->GetString(1);
}