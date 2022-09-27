//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:	chrissstrahl@yahoo.de
//
// CONTAINING PLAYER RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------



//[b60011]

#include "coopChallenges.hpp"
#include "mp_manager.hpp"
#include "_pch_cpp.h"

CoopChallenges coopChallenges;
Container<str> CoopChallengeTypes;
Container<str> CoopChallengeTypesDisabled;

#define COOP_CHALLENGE_STICKTOGETHER_DAMAGE 5.0f
#define COOP_CHALLENGE_STICKTOGETHER_CYCLE 5.0f
#define COOP_CHALLENGE_STICKTOGETHER_REGROUPTIME 30.0f
#define COOP_CHALLENGE_STICKTOGETHER_DISTANCE 600.0f
#define COOP_CHALLENGE_STICKTOGETHER_NAME "Stick together"
#define COOP_CHALLENGE_HALO_NAME "Halo"
#define COOP_CHALLENGE_COLLISION_NAME "Collision"


void CoopChallenges::init(void)
//on level start, init all challenges
{
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		fLastDamageTime = 0;
		challenesAreDisabled = true;
		return;
	}

	fLastDamageTime					= (level.time + COOP_CHALLENGE_STICKTOGETHER_CYCLE);
	challenesAreDisabled			= false;

	//populate lists
	CoopChallengeTypes.AddObject("sticktogether");
	CoopChallengeTypes.AddObject("collision");
	CoopChallengeTypes.AddObject("halo");
}

void CoopChallenges::cleanUp(bool restart)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		fLastDamageTime = 0;
		challenesAreDisabled = true;
		return;
	}

	fLastDamageTime = -1.0f;
	challenesAreDisabled		= false;

	//clean up lists
	CoopChallengeTypes.FreeObjectList();
	CoopChallengeTypesDisabled.FreeObjectList();
	//need to reset the current challenge and load it from ini each mapload - maybe have some dedicated function to load all coop settings from ini at once
}

void CoopChallenges::playerEnteredWarning(Player* player)
{
	//STOP challenges are disabled or just 1 player
	if (challenesAreDisabled || coop_returnPlayerQuantity(2) < 2) {
		return;
	}

	int iTime = COOP_CHALLENGE_STICKTOGETHER_REGROUPTIME;

	switch (iCurrentChallenge)
	{
	case 1: //stayClose
		//give the players some time to regroup
		fLastDamageTime = (level.time + COOP_CHALLENGE_STICKTOGETHER_REGROUPTIME);
		//inform all players to regroup
		player->hudPrint(va("^5Challenge ^2%s ^5is active!^8 You have %d sec to get close to the Team!\n", COOP_CHALLENGE_STICKTOGETHER_NAME, iTime));
		break;
	case 2: //halo
		break;
	case 3: //collision
		break;
	default:
		break;
	}
}

void CoopChallenges::playerLeftWarning(Player* player)
{
	//STOP challenges are disabled or just 1 player
	if (challenesAreDisabled || coop_returnPlayerQuantity(2) < 2) { return; }

	int iTime = COOP_CHALLENGE_STICKTOGETHER_REGROUPTIME;

	switch (iCurrentChallenge)
	{
	case 1: //stayClose
		//give the players some time to regroup
		fLastDamageTime = (level.time + COOP_CHALLENGE_STICKTOGETHER_REGROUPTIME);
		//inform all players to regroup
		coop_textHudprintAll(va("^5Challenge^2 %s:^8 A Player left you have %d sec to regroup!\n", COOP_CHALLENGE_STICKTOGETHER_NAME, iTime));
		break;
	case 2: //halo
		break;
	case 3: //collision

		break;
	default:
		break;
	}
}

void CoopChallenges::playerSpectatorWarning(Player* player)
{
	//STOP challenges are disabled or just 1 player
	if (challenesAreDisabled || coop_returnPlayerQuantity(2) < 2) { return; }

	int iTime = COOP_CHALLENGE_STICKTOGETHER_REGROUPTIME;

	switch (iCurrentChallenge)
	{
	case 1: //stayClose
		//give the players some time to regroup
		fLastDamageTime = (level.time + COOP_CHALLENGE_STICKTOGETHER_REGROUPTIME);
		//inform all players to regroup
		coop_textHudprintAll(va("^5Challenge^2 %s:^8 A Player pauses you have %d sec to regroup!\n", COOP_CHALLENGE_STICKTOGETHER_NAME, iTime));
		break;
	case 2: //halo
		break;
	case 3: //collision
		break;
	default:
		break;
	}
}

void CoopChallenges::playerEntered(Player* player)
{
	//STOP challenges are disabled or just 1 player
	if (challenesAreDisabled || coop_returnPlayerQuantity(2) < 2) { return; }

	switch (iCurrentChallenge)
	{
	case 1: //sticktogether
		//give the players some time to regroup
		fLastDamageTime = (level.time + COOP_CHALLENGE_STICKTOGETHER_REGROUPTIME);
		
		//inform all players to regroup
		if (player->coopPlayer.setupComplete) {
			playerEnteredWarning(player);
		}
		break;
	case 2: //halo
		break;
	case 3: //collision
		break;
	default:
		break;
	}
}

void CoopChallenges::playerSpectator(Player* player)
{
	switch (iCurrentChallenge)
	{
	case 1: //stayClose
		//give the players some time to regroup
		fLastDamageTime = (level.time + COOP_CHALLENGE_STICKTOGETHER_REGROUPTIME);

		//inform all players to regroup
		playerSpectatorWarning(player);	
		break;
	case 2: //halo
		break;
	case 3: //collision
		break;
	default:
		break;
	}
}

void CoopChallenges::playerLeft(Player* player)
{
	switch (iCurrentChallenge)
	{
	case 1: //stayClose
		//give the players some time to regroup
		fLastDamageTime = (level.time + COOP_CHALLENGE_STICKTOGETHER_REGROUPTIME);

		//inform all players to regroup
		playerSpectatorWarning(player);
		break;
	case 2: //halo
		break;
	case 3: //collision
		break;
	default:
		break;
	}
}

void CoopChallenges::update(float frameTime)
{
	//do not update the challenges:
	// if disabled via script
	// during cinematic
	// if only one player is alive
	if (g_gametype->integer == GT_SINGLE_PLAYER || challenesAreDisabled || level.cinematic || coop_returnPlayerQuantity(2) < 2 ) {
		return;
	}

	switch (iCurrentChallenge)
	{
	case 1:
		updateStickTogether(frameTime);
		break;
	case 2:
		updateHalo(frameTime);
		break;
	case 3:
		updateCollision(frameTime);
		break;
	default:
		break;
	}
}

void CoopChallenges::updateCollision(float frameTime)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !game.coop_isActive ) {
		return;
	}

	Player* player;
	Entity* entityOther;

	player = coop_returnPlayerFavored();
	if (!player) {
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

void CoopChallenges::updateStickTogether(float frameTime)
{
	//needs only to be updated once every sec or every 2 secs
	if (!game.coop_isActive || (fLastDamageTime + COOP_CHALLENGE_STICKTOGETHER_CYCLE) > level.time) {
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
			if (Distance(entityOther->origin, playerAnker->origin) < COOP_CHALLENGE_STICKTOGETHER_DISTANCE) {
				iAnyClose = true;
				break;
			}
		}
		if (!iAnyClose) {
			Player* player = (Player*)playerAnker;

			//show distance hud
			if (player->coopPlayer.installed) {
				DelayedServerCommand(player->entnum, "ui_addhud coop_stickTogether");
			}
			else {
				player->hudPrint("Coop Challenge: To far away from Group!\n");
			}

			Event *event = new Event(EV_Pain);
			event->AddFloat(COOP_CHALLENGE_STICKTOGETHER_DAMAGE);
			event->AddEntity(player);
			event->AddInteger(0);
			player->ProcessEvent(event);

			//substract health - make it ignore armor
			float fPlayerHealth = player->getHealth();
			fPlayerHealth = (fPlayerHealth - COOP_CHALLENGE_STICKTOGETHER_DAMAGE);
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

//[b60011] chrissstrahl - check if player can pickup this item while challenge is active
bool CoopChallenges::haloCanPickup(Sentient* sentient, str sItem)
{
	if (/* haloThingActive */ 0 ) {
		if (sentient->isSubclassOf(Player)) {
			Player* player = (Player*)sentient;
			player->hudPrint("Halo challenge is active, you can't receive or pick this up\n");
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

void CoopChallenges::disabled(bool bEnable)
//allowes to disable/enable the challenge - meant to be used from the scripts
{
	challenesAreDisabled = bEnable;
	gi.Printf(va("Coop Challenge status (0=on/1=off) changed: ",(int)bEnable));
}

bool CoopChallenges::isDisabled()
//allowes to check if challenges are disabled/enabled currently (from script)
{
	return challenesAreDisabled;
}

void CThread::challengesDisabled(Event* ev)
//allowes to disable/enable the challenge - meant to be used from the scripts
//also returns the status of the challenge 0=enabled 1=disabled
{
	if (ev->NumArgs() > 0) {
		bool challenesAreDisabled = (bool)ev->GetInteger(1);
		coopChallenges.disabled(challenesAreDisabled);
		gi.Printf(va("challengeDisabled: Challages set to %d (0=on/1=off)\n", challenesAreDisabled));
	}
	ev->ReturnInteger(coopChallenges.isDisabled());
}

void CThread::challengeDisabledNamed(Event* ev)
//allowes to disable/enable the challenge - meant to be used from the scripts
//also returns the status of the challenge 0=enabled 1=disabled
{
	str sName = ev->GetString(1);
	bool namedChallengeIsDisabled = true;

	//check if the given challenge name is valid
	if (sName == "" || !CoopChallengeTypes.ObjectInList(sName.tolower())) {
		gi.Printf(va("challengeDisabledNamed: Challege '%s' does not exist!\n", sName.c_str()));
		gi.Printf("challenges that do exist are named:\n");
		short iChallenge;
		for (iChallenge = CoopChallengeTypes.NumObjects(); iChallenge > 0;iChallenge--) {
			gi.Printf(va("%s\n",CoopChallengeTypes.ObjectAt(iChallenge).c_str()));
		}
		ev->ReturnInteger(iChallenge); //return 0
		return;
	}

	if (ev->NumArgs() > 1) {
		bool challenesAreDisabled = (bool)ev->GetInteger(2);
		short iChallenge = CoopChallengeTypesDisabled.IndexOfObject(sName.c_str());
		
		//if should be disabled but is not, add it
		if (challenesAreDisabled && iChallenge == 0) {
			CoopChallengeTypesDisabled.AddObject(sName.c_str());
			namedChallengeIsDisabled = true;
		}
		//if should be NOT disabled but is, remove it
		else if (!challenesAreDisabled && iChallenge != 0) {
			CoopChallengeTypesDisabled.RemoveObjectAt(iChallenge);
			namedChallengeIsDisabled = false;
		}

		//debug
		//for (iChallenge = CoopChallengeTypesDisabled.NumObjects(); iChallenge > 0; iChallenge--) {
			//gi.Printf(va("Challenges Disabled: %s\n", CoopChallengeTypesDisabled.ObjectAt(iChallenge)));
		//}

		gi.Printf(va("challengeDisabledNamed: Challege %s set to %i (0=on/1=off)\n", sName.c_str(), (int)namedChallengeIsDisabled));
	}
	ev->ReturnInteger((int)namedChallengeIsDisabled);
}