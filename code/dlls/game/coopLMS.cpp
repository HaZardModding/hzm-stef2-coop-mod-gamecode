//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// [b60021] CONTAINING Last Man Standing RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "actor.h"
#include "player.h"
#include "mp_manager.hpp"
#include <qcommon/gameplaymanager.h>

#include "upgPlayer.hpp"
#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"
#include "upgStrings.hpp"
#include "upgCoopInterface.hpp"

#include "coopActor.hpp"
#include "coopReturn.hpp"
#include "coopClass.hpp"

//================================================================
// Name:        coop_lmsRevivePlayer
// Class:       -
//              
// Description: revives given player and give them 1 life
//              
// Parameters:  Player*
//              
// Returns:     bool
//              
//================================================================
bool coop_lmsRevivePlayer(Player* player)
{
	if (game.coop_lastmanstanding) {
		if (player->coopPlayer.lmsDeaths >= coop_lmsGetLives()) {
			player->coopPlayer.lmsDeaths = coop_lmsGetLives();
			player->coopPlayer.lmsDeaths--;

			//try spawn player
			multiplayerManager.respawnPlayer(player, true);

			return true;
		}
	}
	return false;
}
	
//================================================================
// Name:        coop_lmsCheckReconnectHack
// Class:       Player
//              
// Description: check if player is trying to cheat by reconnecting
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_lmsCheckReconnectHack()
{
	if (game.coop_lastmanstanding < 1) {
		return;
	}

	//grab data when did this player last die, was it after current map has started ?
	int deathTimeStamp = atoi(coop_playerGetDataSegment(this, 7));
	if (deathTimeStamp > game.coop_levelStartTime) {
		upgPlayerDeathTimeSet(deathTimeStamp);

		//grab deaths and restore
		int iDeaths = atoi(coop_playerGetDataSegment(this, 9));
		coopPlayer.lmsDeaths = iDeaths;
		
		//player died more often than they should have
		if (iDeaths >= game.coop_lastmanstanding) {
			if (upgPlayerHasLanguageGerman()) {
				hudPrint(va("^5Coop:^2Last Man Standing: Sie haben bereits^1 %d von^5 %d ^2erlaubte Tode.\n", iDeaths, game.coop_lastmanstanding));
			}else {
				hudPrint(va("^5Coop:^ 2Last Man Standing: You allready had^1 %d out of^5 %d ^2allowed deaths.\n", iDeaths, game.coop_lastmanstanding));
			}
			multiplayerManager.makePlayerSpectator(this, SPECTATOR_TYPE_FOLLOW, false);
		}
	}
}

//================================================================
// Name:        coop_lmsInfo
// Class:       Player
//              
// Description: prints lifes info to hud
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_lmsInfo()
{
	if (!game.coop_isActive || coop_lmsGetLives() == 0 || game.levelType < MAPTYPE_MISSION)
		return;

	//print message to let player know whats going on
	if (upgPlayerHasLanguageGerman()) {
		multiplayerManager.HUDPrint(entnum, va("^5Coop ^2Last Man Standing noch^1 %d ^2von^5 %d ^2Leben.\n", (coop_lmsGetLives() - coopPlayer.lmsDeaths), coop_lmsGetLives()));
	}
	else {
		multiplayerManager.HUDPrint(entnum, va("^5Coop ^2Last Man Standing remaining^1 %d ^2of^5 %d ^2lives.\n", (coop_lmsGetLives() - coopPlayer.lmsDeaths), coop_lmsGetLives()));
	}
}


//================================================================
// Name:        coop_lmsRevitalise
// Class:       Player
//              
// Description: tries to move player from spectator back into the team, if player was moved to spec after running out of lives
//              
// Parameters:  oldLives, newLives
//              
// Returns:     void
//              
//================================================================
void Player::coop_lmsRevitalise(int iOldLives,int iNewLives)
{
	//more lives availabe, or lms off
	if (iOldLives < iNewLives || iNewLives == 0) {
		//reset last killed time
		if (upgPlayerDeathTime() > game.coop_levelStartTime) {
			upgPlayerDeathTimeSet(0);
		}

		//currently spectator, has been forced into spec, this is a mission level
		if (multiplayerManager.isPlayerSpectator(this) &&
			!multiplayerManager.isPlayerSpectatorByChoice(this) &&
			game.levelType >= MAPTYPE_MISSION)
		{
			multiplayerManager.respawnPlayer(this, true);
		}
	}


}

//================================================================
// Name:        coop_lmsSpawn
// Class:       Player
//              
// Description: tries to spawn the player if in lms or prevent the spawn
//              
// Parameters:  void
//              
// Returns:     BOOL
//              
//================================================================
bool Player::coop_lmsSpawn()
{
	//no coop or lms
	if (!game.coop_isActive || coop_lmsGetLives() == 0 || game.levelType < MAPTYPE_MISSION)
		return true;

	//player died after this map was started and
	//player is more than 3 sec on level and
	//player dies more often than allowed
	if (upgPlayerDeathTime() > game.coop_levelStartTime &&
		(upgPlayerGetLevelTimeEntered() + 3) < level.time &&
		coopPlayer.lmsDeaths >= coop_lmsGetLives()
		) {
		multiplayerManager.makePlayerSpectator(this, SPECTATOR_TYPE_FOLLOW, false);

		if (!level.mission_failed && (coopPlayer.lastTimeHudMessage + 3) < level.time) {
			coopPlayer.lastTimeHudMessage = level.time;

			if (upgPlayerHasLanguageGerman()) {
				multiplayerManager.HUDPrint(entnum, "^5Coop^8 ^5L^8ast ^5M^8an ^5S^8tanding ^2Aktiv^8 - ^1Sie sind momentan ausgeschaltet.\n");
			}
			else {
				multiplayerManager.HUDPrint(entnum, "^5Coop^8 ^5L^8ast ^5M^8an ^5S^8tanding ^2Active^8 - ^1You are neutralised for the Moment.\n");
			}
		}
		return false;
	}
	return true;
}

//========================================================[b60021]
// Name:        coop_lmsPlayerKilled
// Class:       Player
//              
// Description: Counts deaths up and limits maximim
//            
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_lmsPlayerKilled()
{
	//[b60011] chrissstrahl - count up deaths
	coopPlayer.lmsDeaths++;

	//check if mission failed - all player dead
	coop_lmsCheckFailure();

	//print lifes info
	coop_lmsInfo();

	//if (coopPlayer.lmsDeaths > game.coop_lastmanstanding) {
		//coopPlayer.lmsDeaths = game.coop_lastmanstanding;
	//}
	//multiplayerManager.HUDPrint(entnum, va("coop_playerKilledLMS - deaths %d of %d.\n", coopPlayer.lmsDeaths, game.coop_lastmanstanding));
}

//========================================================[b60021]
// Name:        coop_lmsMpManagerUpdate
// Class:       MultiplayerManager
//              
// Description: 
//            
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
bool MultiplayerManager::coop_lmsMpManagerUpdate(Player* player)
{
	if (game.coop_lastmanstanding > 0 && game.levelType >= MAPTYPE_MISSION) {
		return true;
		//move player into game
		if (player->upgPlayerDeathTime() < game.coop_levelStartTime && multiplayerManager.isPlayerSpectator(player) && !multiplayerManager.isPlayerSpectatorByChoice(player))
		{
			if (_gameStarted)
			{
				multiplayerManager.playerEnterArena(player->entnum, 999);
				multiplayerManager.HUDPrint(player->entnum, va("MultiplayerManager::update [enter] -> dTime(%d)+6<=lvlstarttime d[%d].\n", player->upgPlayerDeathTime(), game.coop_levelStartTime, player->coopPlayer.lmsDeaths));
			}
		}

		//[b60021] chrissstrahl - disabled, was causing issues, moving players prematurely into spec
		if (player->health <= 0)
		{
			time_t result = time(NULL);
			localtime(&result);

			if ((player->upgPlayerDeathTime() + 6) <= (int)result)
			{
				makePlayerSpectator(player, SPECTATOR_TYPE_FOLLOW, false);
				multiplayerManager.HUDPrint(player->entnum, va("MultiplayerManager::update [spec] -> dTime(%d)+6<=localtime d[%d].\n", player->upgPlayerDeathTime(), (int)result, player->coopPlayer.lmsDeaths));
			}
		}
		return true;
	}
	return false;
}

//================================================================
// Name:        coop_lmsSetLives(int iLives)
// Class:       -
//              
// Description: Used to set LMS lifes
//              
// Parameters:  NONE
//              
// Returns:     int
//              
//================================================================
void coop_lmsSetLives(int iLives)
{
	if (iLives < 0) { iLives = 0; }
	game.coop_lastmanstanding = iLives;
}

//================================================================
// Name:        coop_lmsGetLives()
// Class:       -
//              
// Description: Used to retrive LMS lifes
//              
// Parameters:  NONE
//              
// Returns:     int
//              
//================================================================
int coop_lmsGetLives(void)
{
	return game.coop_lastmanstanding;
}

//================================================================
// Name:        coop_lmsActive()
// Class:       -
//              
// Description: Used to check if LMS is active
//              
// Parameters:  NONE
//              
// Returns:     bool
//              
//================================================================
bool coop_lmsActive(void)
{
	if (game.coop_lastmanstanding >= 1) {
		return true;
	}
	return false;
}

//================================================================
// Name:        coop_lmsCheckFailure
// Class:       -
//              
// Description: Checking if the mission should fail during LMS
//              
// Parameters:  NONE
//              
// Returns:     void
//              
//================================================================
void coop_lmsCheckFailure(void)
{
	if (!game.coop_isActive ||
		game.coop_lastmanstanding <= 0 ||
		level.mission_failed ||
		game.levelType < MAPTYPE_MISSION)
	{
		return;// false;
	}

	Player* player;

	int iAll = 0;
	int iActive = 0;

	for (int i = 0; i < maxclients->integer; i++)
	{
		player = (Player*)g_entities[i].entity;
		if (player && player->client && player->isSubclassOf(Player))
		{
			iAll++;
			//[b60021] chrissstrahl - added cehck to make sure alive players are counted as such, even if they died more often than they are suppose to
			if (player->coopPlayer.lmsDeaths < game.coop_lastmanstanding || (player->health > 0 && !multiplayerManager.isPlayerSpectator(player))) {
				iActive++;
			}
		}
	}

	//multiplayerManager.HUDPrintAllClients(va("coop_serverLmsCheckFailure: all[%i] vs active[%i]\n", iAll, iActive));
	//gi.Printf("COOPDEBUG coop_serverLmsCheckFailure: all[%i] vs active[%i]\n", iAll, iActive);

	//fail mission if all are dead
	if (iAll > 0 && iActive == 0)
	{
		//[b607] needs to be exactly this to trigger playerDeathThread
		G_MissionFailed(va("PlayerKilled%i", ((int)G_Random(9.0f) + 1)));
		return;// true;
	}
	return;// false;
}