#pragma once

//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// HZM CO-OP MOD - NPC/AI TEAMMATE HANDLING FOR COOP - ADDS/REMOVES TEAMMATES FROM GAME
//-----------------------------------------------------------------------------------

#include "actor.h"
#include "mp_manager.hpp"
#include "coopScripting.hpp"
extern CoopScripting coopScripting;

#include "coopNpcTeam.hpp"
CoopNpcTeam coopNpcTeam;

void	CoopNpcTeam::npcLoadData()
//reads setup data from ini file at the start of the game, populates the variables
{
	if (!game.coop_isActive) { return; }
	//read from levelname ini
	va("/ini/maps/%s.ini", level.mapname.c_str());
	//set teammates are avialable
	teamMateAvailable = true;
}

void	CoopNpcTeam::playerLeft(Player* player)
//A player left, replace player with ai
{
	if ( !player || !teamMateAvailable) { return; }
	npcAdd(player);
}

void	CoopNpcTeam::playerReadyCheck(Player* player)
//A player who joined, is now ready to replace NPC
{
	if (!player || !teamMateAvailable || player->coopPlayer.coopNpcTeamHandled || multiplayerManager.inMultiplayer() && !multiplayerManager.isPlayerSpectator(player)) {
		return;
	}

	//Last Man Standing should be handled by forced spectator and CoopNpcTeam::playerSpectator()

	//if mission resource menu (mrm) included, abbort if player is not yet ready
	if (coopScripting.getIncludedMrm()) {
		if (!coopScripting.mrnPlayerReady(player)) {
			return;
		}
	}
	npcRemove(player);
}

void	CoopNpcTeam::playerSpectator(Player* player)
//A player is now spectating
{
	if (!player || !teamMateAvailable) { return; }
	npcAdd(player);
}

void	CoopNpcTeam::npcAdd(Player* player)
//A player left, replace player with ai
{
	if (!player || !teamMateAvailable) { return; }
	short int iTeammmate;
	for (iTeammmate = 0; iTeammmate < COOP_NPC_TEAM_MATE_MAX; iTeammmate++) {
		if (!teamMateActive[iTeammmate]) {
			Actor* new_actor;
			new_actor = new Actor;
			new_actor->setModel(teamMateModel[iTeammmate]);
			new_actor->setOrigin(player->origin);
			new_actor->setAngles(player->GetVAngles());
			player->coopPlayer.coopNpcTeamHandled = false;
		}
	}
}

void	CoopNpcTeam::npcRemove(Player* player)
//A player left, replace player with ai
{
	if (!player || !teamMateAvailable) { return; }
	short int iTeammmate;
	for (iTeammmate = (COOP_NPC_TEAM_MATE_MAX - 1); iTeammmate >= 0; iTeammmate--) {
		Entity* eTeammate = teamMateEntity[iTeammmate];
		if (eTeammate) {
			player->coopPlayer.coopNpcTeamHandled = true;
			player->setOrigin( eTeammate->origin );
			player->SetViewAngles( eTeammate->angles );
			teamMatePlayer[iTeammmate] = player; //remember which player replaced this npc
			
			//remove actor
			eTeammate->ProcessEvent(EV_Remove);
		}
	}
}