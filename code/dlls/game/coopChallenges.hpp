//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#pragma once

#include "_pch_cpp.h"
#include "player.h"

extern Event EV_Player_Immobilize;
extern Event EV_Player_PutawayWeapon;
extern Event EV_Player_UseItem;
extern Event EV_UseAnim_SetAnim;

class CoopChallenges : public Class
{
public:
	void		init(void);
	void		cleanUp(bool restart);
	void		playerEntered(Player *player);
	void		playerLeft(Player* player);
	void		update(float frameTime);
	void		updateCollision(float frameTime);
	void		updateStayClose(float frameTime);
	void		updateHalo(float frameTime);
	bool		haloShieldRelayDamage(Sentient* sentient, float fDamage);
	bool		haloCanPickup(Sentient* sentient, str sItem);
	void		disabled(bool bEnable);
	bool		isDisabled();
	
	float		fLastDamageTime			= 0.0f;
	bool		bIsDisabled				= false;
	short		iCurrentChallenge		= 0;
};

//[b60011] chrissstrahl - Variouse External Dependencies
/*
void CThread::challengeDisabled(Event* ev){}							-> coopChallenges.cpp
void CThread::challengeDisabled(Event* ev);								-> globalcmd.h

*/