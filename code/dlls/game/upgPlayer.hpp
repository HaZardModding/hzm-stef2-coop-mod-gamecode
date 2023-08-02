#pragma once
//[GAMEUPGRADE][b60014] chrissstrahl - used to seperate code bases better
class UpgPlayer
{
	friend class Player;
private:
	int			clMaxPackets = -1;
	float		chatTimeLimit = 0.0f;
	float		chatTimeLastSpamInfo = -999.0f;
	float		clientThinkLastTime = -1;
	int			deathTime = -986;
	str			language = "Eng";
	Entity*		targetedEntityLast = NULL;
	float		timeEntered = -988.0f;
	//used to manage transmitting of scanning data to coop tricorder hud
	str			scanDataSendLast = "";
	bool		scanHudActive = false;	
	str			scanData0 = "";
	str			scanData1 = "";
	str			scanData2 = "";
	float		lastTimeSkipCinematic = -970.0f;
};

void	upgPlayerDelayedServerCommand(int entNum, const char* commandText);
void	upgPlayerHandleDelayedServerCommands(void);
void	upgPlayerclearDelayedServerCommands(int entNum);

extern UpgPlayer upgPlayer;
extern Event EV_Player_upgPlayerMessageOfTheDay;
extern Event EV_Player_HasLanguageGerman;
extern Event EV_Player_HasLanguageEnglish;