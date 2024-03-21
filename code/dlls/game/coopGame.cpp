//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// CONTAINING GAME RELATED COOP CODE
//-----------------------------------------------------------------------------------
#include "coopGame.hpp"

#include "mp_manager.hpp"

#include "upgStrings.hpp"

#include "coopWorld.hpp"
#include "coopRadar.hpp"
#include "coopReturn.hpp"

CoopGame coopGame;

//=========================================================[b60021]
// Name:        upgGameStartMatch
// Class:       CoopGame
//              
// Description: Executed when the game starts the match - Multiplayer only
//              
// Parameters:  -
//              
// Returns:     void
// -> MultiplayerModeBase::startMatch
//================================================================
void CoopGame::coopGameStartMatch()
{
	
}

//================================================================
// Name:        missionFailureString
// Class:       CoopGame
//              
// Description:  alters the failure reason config string if needed (which is shown on coop failure hud/menu)
//              
// Parameters:  const str &reason
//              
// Returns:     str           
//================================================================
str CoopGame::missionFailureString(const str &reason)
{
	str newReason = reason;
	//if # is used it means set string without $$
	if (reason[0] == '#') {
		newReason = upgStrings.getStartingFrom(reason, 1);
		// Set our failure reason in the config string
		newReason = va("\n\n  %s^0", newReason.c_str());
	}
	return newReason;
}

//================================================================
// Name:        missionFailureHud
// Class:       CoopGame
//              
// Description: prints objectives to players without coop, to chat
//              
// Parameters:  const str &reason
//              
// Returns:     void           
//================================================================
void CoopGame::missionFailureHud(const str &reason)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER) { return; }

	gentity_t* other;
	int j;
	for (j = 0; j < game.maxclients; j++){
		other = &g_entities[j];
		if (other->inuse && other->client && other->entity){
			Player* player = (Player*)other->entity;
			str sReason = reason;
			//Intentionally no $$ wanted
			if (reason.length() && reason[0] == '#') {
				sReason = upgStrings.getStartingFrom(sReason, 1);
			}
			//If it is just a single word and has no $$ it is extreemly likley a local string, so add $$
			else if (!upgStrings.contains(sReason.c_str(), "$$") && !upgStrings.contains(sReason.c_str(), " ")) {
				sReason = va("$$%s$$", sReason.c_str());
			}

			//show hud for coop clients and text for others
			if (player->coop_getInstalled()) {

				upgPlayerDelayedServerCommand(player->entnum, va("set ui_failureReason %s\n", sReason.c_str()));
				upgPlayerDelayedServerCommand(player->entnum, "pushmenu coop_failure");
			}
			else {
				upgPlayerDelayedServerCommand(player->entnum, "hudprint ^1=============^3$$MISSIONFAILED$$^1=============\n");
				upgPlayerDelayedServerCommand(player->entnum, va("hudprint ^3%s\n", sReason.c_str()));
			}
		}
	}
}

//================================================================
// Name:        missionFailureEmptyServer
// Class:       CoopGame
//              
// Description: restarts/reloads level if mission has failed and no player is on server
//              
// Parameters:  void
//              
// Returns:     void           
//================================================================
void CoopGame::missionFailureEmptyServer()
{
	//if no player found exit right here
	if (G_NumClients() == 0) {
		gi.SendConsoleCommand("restart\n");
		return;
	}
}

//================================================================
// Name:        missionFailureLoadMap
// Class:       CoopGame
//              
// Description: on mission failure, reload map, (with level parameters)
//              
// Parameters:  void
//              
// Returns:     void           
//================================================================
void CoopGame::missionFailureLoadMap()
{
	//hzm coop mod chrissstrahl - load map delayed, but not in singleplayer
	if (!multiplayerManager.inMultiplayer()) { return; }

	Event* ev_loadMap = new Event(EV_World_LoadMap);
	str sParameters = "";

	//hzm coop mod chrissstrahl - do this only if we know we have a valid scriptfile for this map, cuz scriptmaster keeps values of last script file
	if (level.game_script != "") {
		sParameters = program.getStringVariableValue("coop_string_checkpointParameters");
	}
	if (sParameters != "") {
		ev_loadMap->AddString(va("%s$%s", level.mapname.c_str(), sParameters.c_str()));
	}
	else {
		ev_loadMap->AddString(level.mapname.c_str());
	}
	world->PostEvent(ev_loadMap, 8.0f);
}

//================================================================
// Name:		stopCinematic
// Class:       CoopGame
//              
// Description: handle stuff when cienmatics is stopped
//              
// Parameters:  void
//              
// Returns:     void           
//================================================================
void CoopGame::stopCinematic()
{
	gentity_t* other;
	Player* player;

	int j;
	for (j = 0; j < game.maxclients; j++){
		other = &g_entities[j];

		if (other->inuse && other->client && other->entity){
			player = (Player*)other->entity;

			//hzm gameupdate chrissstrahl - make sure spectators remain hidden
			if (multiplayerManager.isPlayerSpectator(player) == false) {
				player->showModel();
				player->_makeSolidASAP = true;
			}

			player->cinematicStopped();
			player->takedamage = DAMAGE_YES;

			//hzm gameupdate chrissstrahl - this should only be done in multiplayer - or chrash chrash goodbye
			if (g_gametype->integer > GT_SINGLE_PLAYER) {
				//clear vote text cinematic overlay
				upgPlayerDelayedServerCommand(player->entnum, "locationprint -1111 -1111 ^0 0");

				//hzm coop mod chrissstrahl - sometimes the timer hud reactivates
				//and also when the level was started with a cinematic sequence, so make sure this doesn't happen
				//hzm coop mod chrissstrahl - make sure the radar is reset, and does not get stuck
				if (game.coop_isActive) {
					upgPlayerDelayedServerCommand(player->entnum, "globalwidgetcommand dmTimer disable");
					coop_radarReset(player);
				}

				if (game.cinematicSkipping) {
					player->clearVoteText();
				}

				//reset vote count after cinematic, so votes that have been started during cinematic do not count when the match starts
				multiplayerManager.resetVoteCount(player);
			}
		}
	}

	//[b60011] chrissstrahl - reset challenges
	coopChallenges.reset();

	//hzm gameupdate chrissstrahl - reset skip status
	game.cinematicSkipping = false;
}