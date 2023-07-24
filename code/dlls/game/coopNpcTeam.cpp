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
	if (!game.coop_isActive || teamMateInitDone) {
		return;
	}
//return;
	npcLoadData();
	teamMateInitDone = true;
}

void CoopNpcTeam::npcCheckAutoTeam()
{
	init();

	if (!teamMateAvailable || teamMatesOnLevel >= COOP_NPC_TEAM_MATE_MAX /* || teamMatesFillUntil >= ?ActivePlayerOnLevel */ ) {
		return;
	}

	//grab info_player_deathmatch spawnlocations and place ai there.
	//start in reverse 1 goest to spawn 8, 2 goes on spawn 7 and so on
	//making sure the models that stay will be removed at the very last
	Entity* ent;
	short int iTeammmate;
	for (iTeammmate = 0; iTeammmate < COOP_NPC_TEAM_MATE_MAX; iTeammmate++) {
		if (!teamMateActive[iTeammmate]) {
			//keep count
			teamMatesOnLevel++;
			ent = npcGetSpawnEnt();
			Vector vOrigin(0, 0, 0);
			Vector vAngles(0, 0, 0);
			if (ent) {
				vOrigin = ent->origin;
				vAngles = ent->angles;
			}
			else {
				if (level.game_script != "") {
					short iMaxPlayers = 8;
					short iSpawnPosition = (iMaxPlayers - teamMatesOnLevel);

					vOrigin = program.getVectorVariableValue(va("coop_vector_spawnOrigin%i", iSpawnPosition));
					if (vOrigin[0] != 0.0f || vOrigin[1] != 0.0f || vOrigin[2] != 0.0f) {
						float fSpawnAngle = program.getFloatVariableValue(va("coop_float_spawnAngle%i", iSpawnPosition));
						if (fSpawnAngle == 0.0f) {
							if (program.getFloatVariableValue("coop_float_spawnAngle0") != 0.0f) {
								fSpawnAngle = program.getFloatVariableValue("coop_float_spawnAngle0");
							}
						}
						vAngles = Vector(0.0f, 0.0f, 0.0f);
						vAngles[1] = fSpawnAngle;
					}
				}
			}
			CoopNpcTeam::npcAdd(vOrigin, vAngles);
		}
	}
}

void CoopNpcTeam::npcLoadData()
//reads setup data from ini file at the start of the game, populates the variables
{
	//read from levelname ini, when this gets operational, read stuff from the ini
	//va("/ini/maps/%s.ini", level.mapname.c_str());

	//this is still in testing, so activate it only on a specific map
	if (Q_stricmp(level.mapname.c_str(),str("coop_npcTeam").c_str())!=0){
		return;
	}
	teamMateModel[0] = "models/char/hazardteam_jurot.tik";
	teamMateModel[1] = "models/char/starfleet_picard_bosshealth.tik";
	teamMateModel[2] = "models/char/hazardteam_gonzales-rom-uniform.tik";
	teamMateModel[3] = "models/char/hazardteam_chang-voyager.tik";

	if (teamMateModel[0].length() || teamMateModel[1].length() || teamMateModel[2].length() || teamMateModel[3].length() ) {
		//set teammates are avialable
		teamMateAvailable = true;
	}
}

Entity*	CoopNpcTeam::npcGetSpawnEnt()
//grabs player spawnloacation for npc to spawn
{
	short iMaxPlayers = 8;
	short iSpawnPosition = (iMaxPlayers - teamMatesOnLevel);

	Entity* spot = NULL;
	Entity* bestspot = NULL;
	for (spot = G_FindClass(spot, "info_player_deathmatch"); spot; spot = G_FindClass(spot, "info_player_deathmatch")){
		if (spot){
			bestspot = spot;
			if (spot->targetname == str(va("ipd%i", iSpawnPosition))) {
				return spot;
			}
		}
	}
	return bestspot;
}

void	CoopNpcTeam::playerLeft(Player* player)
//A player left, replace player with ai
{
	if ( !player || !teamMateAvailable) {
		return;
	}
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
	if (!player || !teamMateAvailable) {
		return;
	}
	npcAdd(player);
}

void	CoopNpcTeam::npcAdd(Player* player)
{
	if (!player || !teamMateAvailable) {
		return;
	}
	player->coopPlayer.coopNpcTeamHandled = false;
	npcAdd(player->origin,player->GetVAngles());
	player->coopPlayer.coopNpcTeamHandled = true;
}

void	CoopNpcTeam::npcAdd(Vector vOrigin,Vector vAngles)
//A player left, replace player with ai
{
//return;
	if(teamMatesOnLevel >= COOP_NPC_TEAM_MATE_MAX){
		gi.Printf(va("CoopNpcTeam::npcAdd - Maximum number of Teammates reached\n"));
		return;
	}
	
	if((coop_returnPlayerQuantityInArena() + teamMatesOnLevel) >= teamMatesFillUntil){
		gi.Printf(va("CoopNpcTeam::npcAdd - Teammate Player Fill Ratio Limit reached\n"));
		return;
	}

	Actor* new_actor;
	new_actor = new Actor;
	new_actor->setModel(teamMateModel[teamMatesOnLevel]);
	new_actor->setOrigin(vOrigin);
	new_actor->setAngles(vAngles);
	new_actor->targetname = va("coopAutoTeamNpc%d",teamMatesOnLevel);
	teamMateEntity[teamMatesOnLevel] = (Entity*)new_actor;

	gi.Printf(va("^3CoopNpcTeam::npcAdd - %s spawned at: %d %d %d\n", teamMateModel[teamMatesOnLevel].c_str(), (int)vOrigin[0], (int)vOrigin[1], (int)vOrigin[2]));
}

void	CoopNpcTeam::npcRemove(Player* player)
//A player left, replace player with ai
{
return;
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
	teamMateInitDone = false; //[b60014] chrissstrahl - fixed forgotton reset
	teamMateAvailable = false;
	teamMatesOnLevel = 0;

	int iIndex;
	iIndex = 0;
	while (COOP_NPC_TEAM_MATE_MAX > iIndex) {
		teamMateModel[iIndex] = "";
		teamMateName[iIndex] = "";
		teamMateActive[iIndex] = false;
		teamMateClientEquivilent[iIndex] = -1;
		teamMateEntity[iIndex] = NULL;
		teamMatePlayer[iIndex] = NULL;
		iIndex++;
	}
}