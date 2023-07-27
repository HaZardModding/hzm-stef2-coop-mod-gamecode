//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// General Game related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#ifndef __UPGGAME_HPP__
#define __UPGGAME_HPP__

class UpgGame
{
	Entity*			cinematicCurrentCam = NULL;
public:
	void			checkMpGametype();
	void			flushTikisPlayers();
	void			flushTikisServer();
	void			setCameraCurrent(Entity* eCam);
	Entity*			getCameraCurrent();
};

extern UpgGame upgGame;

#endif /* upgGame.hpp */