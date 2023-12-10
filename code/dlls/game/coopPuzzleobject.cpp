//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PUZZLEOBJECT RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#include "coopPuzzleobject.hpp"

#include "_pch_cpp.h"
#include "puzzleobject.hpp"
#include "mp_manager.hpp"

#include "coopClass.hpp"

//CoopPuzzleobject coopPuzzleobject;

//allowes tomake a regular puzzle become a modulation-timer puzzle if player is technician or a certain skill level is meet
bool PuzzleObject::coopPuzzleobjectUsePuzzleCheck(Player* player)
{
	if (player && multiplayerManager.inMultiplayer() && game.coop_isActive) {
		if(_itemUsedThread.length() || _solvedThread.length()){
			//[b60018] chrissstrahl - fix puzzle time being shortened for all players instead only technicians
			if (player->coopPlayer.className == COOP_CLASS_NAME_TECHNICIAN || (level.getSkill() <= _minSkill)) {
				return true;
			}
		}
	}
	return false;
}

//return the modulation time for the puzzle - set short time for technicians
//[b60018] chrissstrahl - fixed function return value
float PuzzleObject::coopPuzzleobjectUsePuzzleGetTime(Player* player)
{
	//Technician modulate for max 4 sec - also set 4 sec if it becomes a bar but no time was set
	if (player->coopPlayer.className == COOP_CLASS_NAME_TECHNICIAN || (level.getSkill() <= _minSkill) && _timeToUse == 0.0f) {
		if (_timeToUse > 4 || _timeToUse == 0.0f) {
			return 4.0f;
		}
	}
	return _timeToUse;
}