//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// MultiplayerManager (mp_manager) class related Upgrades, Updates and Fixes
// This code should be independant from the Coop and be useable without the HZM Coop Mod
//-----------------------------------------------------------------------------------

class MultiplayerManager;

#include "_pch_cpp.h"
#include "player.h"
#include "mp_manager.hpp"

#include "upgPlayer.hpp"
#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"
#include "upgStrings.hpp"

//DEPENDENCIES TO REMOVE


//hzm coop mod daggolin - we need access to the frags (_numKills)
void MultiplayerManager::addKills(int entnum, int kills)
{
	if (!_inMultiplayerGame)
		return;

	_multiplayerGame->addKills(entnum, kills);
}

//hzm gameupdate chrissstrahl - add new func to reset vote count - used to reset after cinematics
void MultiplayerManager::resetVoteCount(Player* player)
{
	if (player && _inMultiplayerGame)//if we call this in singleplayer the game will freeze and eventually crash
	{
		_playerData[player->entnum]._votecount = 0;
	}
}

//hzm gamefix chrissstrahl - used to show new players the current vote
float MultiplayerManager::voteTime(void)
{
	return _voteTime;
}

//hzm gamefix chrissstrahl - used to show new players the current vote
str MultiplayerManager::getVoteString(void)
{
	return _voteString;
}

//hzm gamefix chrissstrahl - used to show new players the current vote
void MultiplayerManager::updateVotes(Player* player, bool bLeaving)
{
	//player entering game
	if (!bLeaving) {
		if (multiplayerManager.voteTime() > 0) {
			//set current number of voters
			_numVoters = coop_returnPlayerQuantity(3); //[b607] chrissstrahl - excluding bots

			//hzm coop mod chrissstrahl - translate the vote strings forbetter understanding
			str sVoteText = "";
			bool bALt = false;

			//hzm coop mod chrissstrahl - handle coop specific votes in a seperate function
			bALt = coop_vote_modifyVoteText(multiplayerManager.getVoteString(), sVoteText, player);

			//hzm coop mod chrissstrahl - print it in german if player is using german game version
			if (player->upgPlayerHasLanguageGerman()) {
				if (upgStrings.containsAt(multiplayerManager.getVoteString(), "skipcinematic") > -1) {
					bALt = true; sVoteText = "Filmsequenz abbrechen?"; sVoteText += upgStrings.getStartingFrom(multiplayerManager.getVoteString(), 13);
				}
			}
			else {
				if (upgStrings.containsAt(multiplayerManager.getVoteString(), "skipcinematic") > -1) {
					bALt = true; sVoteText = "Skip Cinematic ?"; sVoteText += upgStrings.getStartingFrom(multiplayerManager.getVoteString(), 13);
				}
			}

			//hzm coop mod chrissstrahl - do not set/send vote for player who started the vote
			if (!player->coopPlayer.startedVote) {

				//hzm decide which string to print
				if (!bALt)
					sVoteText = multiplayerManager.getVoteString();
				//[b607] chrissstrahl - yes and no are now in a seperate line - and non coop clients get the default text
				if (player->coop_getInstalled()) {
					player->setVoteText(va("$$NewVote$$: %s\n(F1 = $$Yes$$, F2 = $$No$$)", sVoteText.c_str()));
				}
				else {
					player->setVoteText(va("$$NewVote$$: %s", sVoteText.c_str()));
				}
			}
			//hzm eof
		}
		return;
	}

	//player leaving game
	//if only 2 players on the server and the player who started the vote leaves, end the vote
	if (player->coopPlayer.startedVote && coop_returnPlayerQuantity(3) < 3) { //[b607] chrissstrahl - excluding bots
		_voteYes = 0;
		_voteTime = 0.0f;

		for (int i = 0; i < maxclients->integer; i++) {
			Player* currentPlayer;
			currentPlayer = getPlayer(i);
			if (currentPlayer) {
				currentPlayer->clearVoteText();
				player->coopPlayer.startedVote = false;
				_playerData[player->entnum]._voted = false;
			}
		}
		return;
	}

	if (player->coopPlayer.startedVote || _playerData[player->entnum]._voted) {
		_voteYes--;
	}
	_numVoters = (coop_returnPlayerQuantity(3) - 1);  //[b607] chrissstrahl - 3 excluding bots and substracting leaving player
}
