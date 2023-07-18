#ifndef __UPGPLAYER_HPP__
#define __UPGPLAYER_HPP__

//[GAMEUPGRADE][b60014] chrissstrahl - used to seperate code bases better
class UpgPlayer
{
	friend class Player;
private:
	int		clMaxPackets = -1;
	float	chatTimeLimit = 0.0f;
	float	chatTimeLastSpamInfo = -999.0f;
};

#endif /* !__UPGPLAYER_HPP__ */