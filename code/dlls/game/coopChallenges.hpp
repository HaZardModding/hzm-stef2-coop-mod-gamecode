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

extern Container<str> CoopChallengeTypes;
extern Container<str> CoopChallengeTypesDisabled;

class CoopChallenges : public Class
{
public:
	void		init(void);
	void		cleanUp(bool restart);
	void		playerEnteredWarning(Player* player);
	void		playerLeftWarning(Player* player);
	void		playerSpectatorWarning(Player* player);
	void		playerEntered(Player *player);
	void		playerLeft(Player* player);
	void		playerSpectator(Player* player);
	void		update(float frameTime);
	void		updateCollision(float frameTime);
	void		updateStayClose(float frameTime);
	void		updateHalo(float frameTime);
	bool		haloShieldRelayDamage(Sentient* sentient, float fDamage);
	bool		haloCanPickup(Sentient* sentient, str sItem);
	void		disabled(bool bEnable);
	bool		isDisabled();
	
	float		fLastDamageTime			= 0.0f;
	bool		challenesAreDisabled	= false;
	short		iCurrentChallenge		= 0;
};

//[b60011] chrissstrahl - Variouse External Dependencies
/*
void CThread::challengeDisabled(Event* ev){}											-> coopChallenges.cpp
void challengesDisabled(Event* ev);														-> globalcmd.h
Event EV_ScriptThread_challengeDisabled													-> globalcmd.cpp
{ &EV_ScriptThread_challengeDisabled,			&CThread::challengeDisabled } ,			-> globalcmd.cpp


void CThread::challengeDisabledNamed(Event* ev){}										-> coopChallenges.cpp
void challengeDisabledNamed(Event* ev);													-> globalcmd.h
Event EV_ScriptThread_challengeDisabledNamed											-> globalcmd.cpp
{ &EV_ScriptThread_challengeDisabledNamed,		&CThread::challengeDisabledNamed } ,	-> globalcmd.cpp
*/