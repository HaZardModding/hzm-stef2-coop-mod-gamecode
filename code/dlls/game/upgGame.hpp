//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// General Game related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"


class UpgGame
{
	Entity*			cinematicCurrentCam = NULL;
	str				environmentName = "";
public:
	void			setEnvironment(str name);
	str				getEnvironment();
	void			cleanUp(bool restart);
	int				configstringRemove(str sRem);
	void			checkMpGametype();
	void			flushTikisPlayers();
	void			flushTikisServer();
	void			setCameraCurrent(Entity* eCam);
	Entity*			getCameraCurrent();
	void			startCinematic();
	void			stopCinematic();
};

extern UpgGame upgGame;
extern Event EV_ScriptThread_ConfigstringRemove;