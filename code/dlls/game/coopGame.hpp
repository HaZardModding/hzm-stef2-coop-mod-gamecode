//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// CONTAINING GAME RELATED COOP CODE
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"

//[b60011] chrissstrahl
class CoopGame
{
public:
	str				missionFailureString(const str& reason);
	void			missionFailureHud(const str& reason);
	void			missionFailureEmptyServer();
	void			missionFailureLoadMap();
	void			stopCinematic();
};

extern CoopGame coopGame;