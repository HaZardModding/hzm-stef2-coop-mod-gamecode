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
//[b60011] Chrissstrahl


#ifndef __COOPCHALLENGE_CPP__
#define __COOPCHALLENGE_CPP__

#include "coopChallenges.hpp"
#include "mp_manager.hpp"
#include "_pch_cpp.h"

#define COOP_CHALLANGE_STICKTOGETHER_DAMAGE 5.0f
#define COOP_CHALLANGE_STICKTOGETHER_CYCLE 5.0f
#define COOP_CHALLANGE_STICKTOGETHER_REGROUPTIME 30.0f

CoopChallenges coopChallenges;


void CoopChallenges::init(void)
{
	fLastDamageTime = (level.time + COOP_CHALLANGE_STICKTOGETHER_CYCLE);
}

void CoopChallenges::cleanUp(bool restart)
{
	fLastDamageTime = -1.0f;
	//need to reset the current challange and load it from ini each mapload - maybe have some dedicated function to load all coop settings from ini at once
}

void CoopChallenges::playerEntered(Player* player)
{
	//give the players some time to regroup
	fLastDamageTime = (level.time + COOP_CHALLANGE_STICKTOGETHER_REGROUPTIME);
}

void CoopChallenges::playerLeft(Player* player)
{
	//give the players some time to regroup
	fLastDamageTime = (level.time + COOP_CHALLANGE_STICKTOGETHER_REGROUPTIME);
}

void CoopChallenges::update(float frameTime)
{
	//[b60011] chrissstrahl - development protection - do not execute for anyone else
	//this way untested and unfinished features do not accidently leak and cause issues
	cvar_t* cvar = gi.cvar_get("username");
	if (!cvar || coop_returnIntFind(cvar->string, "chrissstrahl") == -1) {
		return;
	}
	//end development protection


	//do not update the challanges:
	// during cinematic
	// if only one player is alive
	if (level.cinematic || coop_returnPlayerQuantity(2) < 2 ) {
		return;
	}

	int iCallangeNumber = 2;
	switch (iCallangeNumber)
	{
	case 1:
		updateCollision(frameTime);
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
	//needs only to be updated once every sec or every 2 secs
	if (!game.coop_isActive || (fLastDamageTime + COOP_CHALLANGE_STICKTOGETHER_CYCLE) > level.time) {
		return;
	}

	//remember last run time
	fLastDamageTime = level.time;

	//check each player distance and hurt the players that are to far away from the group
	Entity* playerAnker;
	Entity* entityOther;

	int i;
	//for (i = 0; i < maxentities->integer; i++) { //check only players in the beginning - deactivate actor checks
	for (i = 0; i < maxclients->integer; i++) { //check only players in the beginning - deactivate actor checks
		playerAnker = g_entities[i].entity;

		if (!playerAnker ||
			playerAnker->health <= 0.0f ||
			multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator((Player*)playerAnker))
		{
			continue;
		}

		int j;
		bool iAnyClose = false;
		entityOther = NULL;
		for (j = 0; j < maxclients->integer; j++) {
			entityOther = g_entities[j].entity;

			if (!entityOther ||
				j == i ||
				entityOther->health <= 0.0f ||
				multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator((Player*)entityOther))
			{
				continue;
			}
			//if they are to far apart like 300 make em get hurt
			if (Distance(entityOther->origin, playerAnker->origin) < 300) {
				iAnyClose = true;
				break;
			}
		}
		if (!iAnyClose) {
			Player* player = (Player*)playerAnker;
			player->hudPrint("Coop Challenge Stick together: To far away from Group!\n");

			Event *event = new Event(EV_Pain);
			event->AddFloat(COOP_CHALLANGE_STICKTOGETHER_DAMAGE);
			event->AddEntity(player);
			event->AddInteger(0);
			player->ProcessEvent(event);

			//substract health - make it ignore armor
			float fPlayerHealth = player->getHealth();
			fPlayerHealth = (fPlayerHealth - COOP_CHALLANGE_STICKTOGETHER_DAMAGE);
			if (fPlayerHealth > 0) {
				player->setHealth(fPlayerHealth);
			}
			else {
				player->setHealth(0.0f);
				player->Damage(player, player, 10.0f, player->origin, vec_zero, vec_zero, 0, 0, MOD_SUICIDE);
			}
		}
	}
}

//[b60011] chrissstrahl - check if player can pickup this item while challange is active
bool CoopChallenges::haloCanPickup(Sentient* sentient, str sItem)
{
	if (/* haloThingActive */ 0 ) {
		if (sentient->isSubclassOf(Player)) {
			Player* player = (Player*)sentient;
			player->hudPrint("Halo Challange is active, you can't receive or pick this up\n");
		}
		return false;
	}

	return true;
}

//[b60011] chrissstrahl - if player has shield take from shield until depleted, discard any left over damage
bool CoopChallenges::haloShieldRelayDamage(Sentient *sentient,float fDamage)
{	
	bool bRelayDamage = true;
	float fArmor = sentient->GetArmorValue();
	float fVal = fArmor;
	if (game.coop_isActive && 0 /* && haloThingActive */ && fArmor > 0) {
		if (fArmor > 0) {
			bRelayDamage = false;
		}
		fArmor -= fDamage;
		if (fArmor < 0) { fArmor = 0; }
		sentient->SetMyArmorAmount(fArmor);
	}
//gi.Printf(va("haloShieldRelayDamage: D:%f -> A:%f -> RLY:%d\n",fDamage, fVal, (int)bRelayDamage));
	//relay damage to health ?
	return bRelayDamage;
}

void CoopChallenges::updateHalo(float frameTime)
{
	//used to recharge player shields and set vars if they take no damage because the shield is up

	//disable -> coop_classRegenerate regen
	//add -> func to quickly recharge shields and call it maybe from -> coop_playerThink
}

#endif