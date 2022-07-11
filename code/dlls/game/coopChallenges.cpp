// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Code related to Coop Player Challenges
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


#ifndef __COOPCHALLENGE_CPP__
#define __COOPCHALLENGE_CPP__

#include "coopChallenges.hpp"
#include "mp_manager.hpp"
#include "_pch_cpp.h"

extern CoopChallenges coopChallenges;

void CoopChallenges::init(void)
{
}

void CoopChallenges::cleanup(bool restart)
{
}

void CoopChallenges::update(float frameTime)
{
	int iCallangeNumber = 2;
	switch (iCallangeNumber)
	{
	case 1:
		CoopChallenges::updateCollision(frameTime);
		break;
	case 2:
		updateStayClose(frameTime);
		break;
	case 3:
		updateHalo(frameTime);
		break;
	default:
		break;
	}
}

void CoopChallenges::updateCollision(float frameTime)
{
	Player* player;
	Entity* entityOther;

	player = coop_returnPlayerFavored();//

	//don't do anything if not in coop or if there is no active player
	if (!game.coop_isActive || !player) {
		return;
	}

	//check if this player is colliding with any other player
	int i;
	//for (i = 0; i < maxentities->integer; i++) { //check only players in the beginning - deactivate actor checks
	for (i = 0; i < maxclients->integer; i++) { //check only players in the beginning - deactivate actor checks
		entityOther = g_entities[i].entity;

		if (!entityOther ||
			entityOther == player ||
			!entityOther->isSubclassOf(Sentient) ||
			entityOther->health <= 0.0f ||
			entityOther->isSubclassOf(Player) && multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator((Player*)entityOther))
		{
			continue;
		}

		if (entityOther->isSubclassOf(Actor)) {
			Actor* actor = NULL;
			actor = (Actor*)entityOther;
			if (!actor->GetActorFlag(ACTOR_FLAG_AI_ON) && actor->hidden()) {
				continue;
			}
		}

		//if they are to close like 25-30 make em fall over
		if (Distance(entityOther->origin, player->origin) > 50) {
			continue;
		}

		//STUFF DOWN HERE SHOULD BE IN A SEPERATE FUNC WITH ALL THE CHECKS
		//
		//
		//
		//
		//do not colide if player did colide moments ago
		float fTimeToImmobilizePlayer = 5.0f;

		if (player->coopPlayer.callange_LastCollisionTime + 10 > level.time) {
			continue;
		}

		player->coopPlayer.callange_LastCollisionTime = level.time;

		Event* StopFireEvent;
		StopFireEvent = new Event(EV_Sentient_StopFire);
		StopFireEvent->AddString("dualhand");
		player->ProcessEvent(StopFireEvent);

		str sUnholserName = "None";
		weaponhand_t	hand = WEAPON_ANY;
		player->getActiveWeaponName(hand, sUnholserName);

		Event* WeaponUse;
		WeaponUse = new Event(EV_Player_UseItem);
		WeaponUse->AddString(va("%s",sUnholserName.c_str()));
		player->PostEvent(WeaponUse, fTimeToImmobilizePlayer);

		Event* WeaponPutaway;
		WeaponPutaway = new Event(EV_Player_PutawayWeapon);
		WeaponPutaway->AddString("dualhand");
		player->ProcessEvent(WeaponPutaway);

		Event* AnimationIdle;
		AnimationIdle = new Event(EV_UseAnim_SetAnim);
		AnimationIdle->AddString(va("%s", "idle"));
		player->PostEvent(AnimationIdle, fTimeToImmobilizePlayer);


		//immobilize player event
/*		Event* ImmobilizePlayer;
		ImmobilizePlayer = new Event(EV_Player_Immobilize);
		ImmobilizePlayer->AddInteger(1);
		player->ProcessEvent(ImmobilizePlayer);

		ImmobilizePlayer = new Event(EV_Player_Immobilize);
		ImmobilizePlayer->AddInteger(0);
		player->PostEvent(EV_Player_Immobilize,4);

		//void PmoveSingle (pmove_t *pmove)
		// Clear movement flags
		//client->ps.pm_flags &= ~( PMF_FLIGHT | PMF_FROZEN | PMF_NO_PREDICTION | PMF_NO_MOVE | PMF_HAVETARGET | PMF_NO_GRAVITY );
		void Player::SetClientViewAngles( const Vector &position , const float cameraoffset , const Vector &ang , const Vector &vel , const float camerafov ) const
*/
//stop all other animations
		player->animate->RandomAnimate("death", EV_StopAnimating);

		//do same shit as to the current player - so create a fallover or animate or collide func a player can be given to.
		//if (entityOther->isSubclassOf(Player)) {
		//}
	}
}

void CoopChallenges::updateStayClose(float frameTime)
{
	Player* player;
	Entity* entityOther;

	player = coop_returnPlayerFavored();//

	//don't do anything if not in coop or if there is no active player
	if (!game.coop_isActive || !player) {
		return;
	}

	//check if this player is colliding with any other player
	int i;
	//for (i = 0; i < maxentities->integer; i++) { //check only players in the beginning - deactivate actor checks
	for (i = 0; i < maxclients->integer; i++) { //check only players in the beginning - deactivate actor checks
		entityOther = g_entities[i].entity;

		if (!entityOther ||
			entityOther == player ||
			!entityOther->isSubclassOf(Sentient) ||
			entityOther->health <= 0.0f ||
			entityOther->isSubclassOf(Player) && multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator((Player*)entityOther))
		{
			continue;
		}

		if (entityOther->isSubclassOf(Actor)) {
			Actor* actor = NULL;
			actor = (Actor*)entityOther;
			if (!actor->GetActorFlag(ACTOR_FLAG_AI_ON) && actor->hidden()) {
				continue;
			}
		}

		//if they are to far apart like 300 make em get hurt
		if (Distance(entityOther->origin, player->origin) < 300) {
			continue;
		}
		int iDamage = 5;
		player->Damage(world, world, iDamage, player->centroid, player->centroid, player->centroid, (int)iDamage, 0, MOD_NONE);
	}
}

void CoopChallenges::updateHalo(float frameTime)
{
	//used to recharge player shields and set vars if they take no damage because the shield is up
}

#endif