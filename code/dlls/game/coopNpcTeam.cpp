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

void	CoopNpcTeam::init()
{
	if (!game.coop_isActive) { return; }
	//npcLoadData();
}


void	CoopNpcTeam::npcLoadData()
//reads setup data from ini file at the start of the game, populates the variables
{
	//read from levelname ini
	va("/ini/maps/%s.ini", level.mapname.c_str());

	//models/char/
	//hazardteam_jurot.tik
	//hazardteam_kourban-leadplayer.tik
	//hazardteam_kourban.tik
	//hazardteam_picard.tik
	//hazardteam_chang-voyager.tik

//teamMateModel[0] = "models/char/hazardteam_picard.tik";
//teamMateModel[1] = "models/char/hazardteam_jurot.tik";
//teamMateModel[2] = "hazardteam_chang-voyager.tik";
//teamMateModel[3] = "models/char/hazardteam_chang-voyager.tik";

	if (teamMateModel[0].length() || teamMateModel[1].length() || teamMateModel[2].length() || teamMateModel[3].length() ) {
		//set teammates are avialable
		teamMateAvailable = true;

		return;

		//grab info_player_deathmatch spawnlocations and place ai there.
		//start in reverse 1 goest to spawn 8, 2 goes on spawn 7 and so on
		//making sure the models that stay will be removed at the very last
		Entity * ent;
		ent = npcGetSpawnPos();
		Vector vOrigin(0,0,0);
		Vector vAngles(0,0,0);
		if (ent) {
			vOrigin	= ent->origin;
			vAngles = ent->angles;
		}
		CoopNpcTeam::npcAdd(vOrigin,vAngles);
	}
}

Entity*	CoopNpcTeam::npcGetSpawnPos()
//grabs player spawnloacation for npc to spawn
{
	int iMaxPlayers = 8;
	int iSpawnPosition = (iMaxPlayers - teamMatesOnLevel);

	Entity* ent;
	TargetList* tlist;
	tlist = world->GetTargetList(va("ipd%i", iSpawnPosition), false);
	if (tlist) {
		ent = tlist->GetNextEntity(NULL);
		return ent;
	}
	return NULL;
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
{
	if (!player || !teamMateAvailable) { return; }
	player->coopPlayer.coopNpcTeamHandled = false;
	CoopNpcTeam::npcAdd(player->origin,player->GetVAngles());
}

void	CoopNpcTeam::npcAdd(Vector vOrigin,Vector vAngles)
//A player left, replace player with ai
{
	if(teamMatesOnLevel >= COOP_NPC_TEAM_MATE_MAX){
		gi.Printf(va("CoopNpcTeam::npcAdd - Maximum number of Teammates reached\n"));
		return;
	}

	short int iTeammmate;
	for (iTeammmate = 0; iTeammmate < COOP_NPC_TEAM_MATE_MAX; iTeammmate++) {
		if (!teamMateActive[iTeammmate]) {
			//keep count
			teamMatesOnLevel++;

			Actor* new_actor;
			new_actor = new Actor;
			new_actor->setModel(teamMateModel[iTeammmate]);
			new_actor->setOrigin(vOrigin);
			new_actor->setAngles(vAngles);
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

			//keep count
			teamMatesOnLevel--;
		}
	}
}

void	CoopNpcTeam::cleanUp(bool restart)
//A player left, replace player with ai
{

}