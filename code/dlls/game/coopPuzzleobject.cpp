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

bool PuzzleObject::coopPuzzleobjectUsePuzzleCheck(Player* player)
{
	if (player && multiplayerManager.inMultiplayer() && game.coop_isActive) {
		if(_itemUsedThread.length() || _solvedThread.length()){
			if (player->coopPlayer.className == COOP_CLASS_NAME_TECHNICIAN || _timed || (level.getSkill() <= _minSkill)) {
				return true;
			}
		}
	}
	return false;
}

bool PuzzleObject::coopPuzzleobjectUsePuzzleGetTime(Player* player)
{
	if (coopPuzzleobjectUsePuzzleCheck(player)) {
		if (_timeToUse < 2) {
			return 3;
		}
	}
	return _timeToUse;
}