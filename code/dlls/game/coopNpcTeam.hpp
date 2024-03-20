#pragma once

//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// HZM CO-OP MOD - NPC/AI TEAMMATE HANDLING FOR COOP - ADDS/REMOVES TEAMMATES FROM GAME
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "player.h"

#define COOP_NPC_TEAM_MATE_MAX 4

class CoopNpcTeam
{
private:
	bool teamMateInitDone = false;
	bool teamMateAvailable = false;
	short teamMatesFillUntil = 5;
	short teamMateClientEquivilent[COOP_NPC_TEAM_MATE_MAX]{-1,-1,-1,-1};
	str teamMateModel[COOP_NPC_TEAM_MATE_MAX]{ "","","",""};
	str teamMateName[COOP_NPC_TEAM_MATE_MAX]{ "","","","" };
	bool teamMateActive[COOP_NPC_TEAM_MATE_MAX]{ false,false,false,false };
	Entity *teamMateEntity[COOP_NPC_TEAM_MATE_MAX]{ NULL,NULL,NULL,NULL };
	Player *teamMatePlayer[COOP_NPC_TEAM_MATE_MAX]{ NULL,NULL,NULL,NULL };
	short teamMatesOnLevel = 0;

public:
	void	init();
	void	npcCheckAutoTeam();
	void	npcLoadData();
	void	playerLeft(Player* player);
	void	playerReadyCheck(Player* player);
	void	playerSpectator(Player* player);
	void	npcAdd(Player* player);
	void	npcAdd(Vector vOrigin, Vector vAngles);
	void	npcRemove(Player* player);
	Entity* npcGetSpawnEnt();
	void	cleanUp(bool restart);
};

extern CoopNpcTeam coopNpcTeam;
extern Event EV_Player_coop_playerNpcCheckAutoTeam;

/*
ADDED playerLeft() TO coopPLayer.cpp at coopPlayerDisconnecting()
ADDED playerReadyCheck() TO coopPLayer.cpp at coop_playerEnterArena()
ADDED playerSpectator() TO coopPLayer.cpp at coop_playerSpectator()
*/
