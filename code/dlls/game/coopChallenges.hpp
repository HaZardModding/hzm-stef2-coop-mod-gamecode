//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Code related to Coop Player Challanges
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING TEXT AND LOCALIZING RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2022 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]
//[b611] Chrissstrahl

#ifndef __COOPCHALLANGE_HPP__
#define __COOPCHALLANGE_HPP__

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
	float		fLastDamageTime = 0.0f;
};

#endif