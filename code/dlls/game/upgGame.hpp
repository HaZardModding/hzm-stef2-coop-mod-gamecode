#ifndef __UPGGAME_HPP__
#define __UPGGAME_HPP__

class UpgGame
{
	Entity*			cinematicCurrentCam = NULL;
public:
	void			flushTikisPlayers();
	void			flushTikisServer();
	void			setCameraCurrent(Entity* eCam);
	Entity*			getCameraCurrent();
};

extern UpgGame upgGame;

#endif /* upgGame.hpp */