#ifndef __UPGPLAYER_HPP__
#define __UPGPLAYER_HPP__

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
	str			lastScanSendData = "";
	bool		scanHudActive = false;
};

void	upgPlayerDelayedServerCommand(int entNum, const char* commandText);
void	upgPlayerHandleDelayedServerCommands(void);
void	upgPlayerclearDelayedServerCommands(int entNum);

extern UpgPlayer upgPlayer;

#endif /* !__UPGPLAYER_HPP__ */