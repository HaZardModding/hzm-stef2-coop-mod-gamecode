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
	return false;
	if (player && multiplayerManager.inMultiplayer() && game.coop_isActive) {
		if (_timed || (level.getSkill() <= _minSkill)) {
			if (player->coopPlayer.className == COOP_CLASS_NAME_TECHNICIAN) {
				return true;
			}
		}
	}
	return false;
}