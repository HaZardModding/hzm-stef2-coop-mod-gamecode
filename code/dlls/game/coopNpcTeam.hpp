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
#define COOP_NPC_TEAM_MATE_VALID_MODEL_1 "xxx.tik"
#define COOP_NPC_TEAM_MATE_VALID_MODEL_2 "xxx.tik"
#define COOP_NPC_TEAM_MATE_VALID_MODEL_3 "xxx.tik"
#define COOP_NPC_TEAM_MATE_VALID_MODEL_4 "xxx.tik"
#define COOP_NPC_TEAM_MATE_VALID_MODEL_22 "readValidModelsFromIni and put into container"

class CoopNpcTeam
{
private:
	bool teamMateAvailable = false;
	str teamMateModel[COOP_NPC_TEAM_MATE_MAX]{ "","","",""};
	str teamMateName[COOP_NPC_TEAM_MATE_MAX]{ "","","","" };
	str teamMateWeapon[COOP_NPC_TEAM_MATE_MAX]{ "","","","" };
	bool teamMateActive[COOP_NPC_TEAM_MATE_MAX]{ false,false,false,false };
	Entity *teamMateEntity[COOP_NPC_TEAM_MATE_MAX]{ NULL,NULL,NULL,NULL };
	Player *teamMatePlayer[COOP_NPC_TEAM_MATE_MAX]{ NULL,NULL,NULL,NULL };

public:
	void	npcLoadData();
	void	playerLeft(Player* player);
	void	playerReadyCheck(Player* player);
	void	playerSpectator(Player* player);
	void	npcAdd(Player* player);
	void	npcRemove(Player* player);
};

