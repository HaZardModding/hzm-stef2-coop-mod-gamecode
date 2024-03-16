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
#include "mp_manager.hpp"
#include <qcommon/gameplaymanager.h>

#include "upgGame.hpp"
#include "upgPlayer.hpp"
#include "upgMp_manager.hpp"
#include "upgStrings.hpp"
#include "upgCoopInterface.hpp"
#include "upgWorld.hpp"

//[b60021] chrissstrahl - get last activating entity for this puzzle
Event EV_ScriptSlave_GetLastActivatingEntity
(
	"getLastActivatingEntity",
	EV_DEFAULT,
	"@e",
	"entity",
	"Returns last entity activating this scriptSlave"
);


//[b60021] chrissstrahl - get use label - script_object has onUse set
//-----------------------------------------------------
str ScriptSlave::upgScriptSlaveGetUseLabel()
{
	return uselabel;
}

//[b60021] chrissstrahl - return last activator for script/event usage
//-----------------------------------------------------
void ScriptSlave::upgGetLastActivatingEntity(Event* ev)
{
	ev->ReturnEntity(activator);
}

//[b60021] chrissstrahl - return last activator
//-----------------------------------------------------
EntityPtr ScriptSlave::upgGetLastActivatingEntity()
{
	return activator;
}