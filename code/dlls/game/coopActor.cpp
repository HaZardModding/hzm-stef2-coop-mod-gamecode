//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// [b607] CONTAINING SERVER RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "actor.h"
#include "player.h"
#include "mp_manager.hpp"
#include <qcommon/gameplaymanager.h>

#include "upgPlayer.hpp"
#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"
#include "upgStrings.hpp"
#include "upgCoopInterface.hpp"

#include "coopActor.hpp"
#include "coopReturn.hpp"
#include "coopClass.hpp"

extern Event EV_Actor_Fade;

//========================================================[b60018]
// Name:        coop_actorIgnoreAnimationWarning
// Class:       -
//              
// Description: Checks if the warningmessage for a missing animation should be ignored
//				This exists because we can't change the tikis without requireing everyone
//				to have the coop mod installed, but we want stock maps playable without
//				WARNING: Actor::SetAnim Actor  has no anim named
//            
// Parameters:  str model, str anim
//              
// Returns:     bool
//              
//================================================================
bool coop_actorIgnoreAnimationWarning(const str &modelName, const str &animName)
{
	if (modelName == "models/char/attrexian-m7-pistol-followplayer-colonist-m.tik") {
		if(animName == "attrexpistol_calm_idle" || animName == "attrexpistol_idle") return true;
	}
	return false;
}

//========================================================[b60018]
// Name:        coop_actorUsedByEquipment
// Class:       -
//              
// Description: 
//              
// Parameters:  Actor, Entity
//              
// Returns:     VOID
//              
//================================================================
void coop_actorUsedByEquipment(Actor* actor, Entity* entityEquipment)
{
	GameplayManager* gpm = GameplayManager::getTheGameplayManager();
	str type = actor->getArchetype();
	str interactiveType = gpm->getStringValue(type, "InteractiveType");

	//don't allow to handle these
	if (actor->actortype == IS_INANIMATE ||
		actor->actortype == IS_MONSTER ||
		actor->actortype == IS_ENEMY ||
		interactiveType == "Enemy")
	{
		return;
	}

	Equipment* equipment = (Equipment*)entityEquipment;
	Player* player = (Player*)equipment->GetOwner();
	if (!Q_stricmp(player->coopPlayer.className.c_str(), COOP_CLASS_NAME_MEDIC) && coop_returnEntityFloatVar((Entity*)actor, "_playerCanNotHeal") != 1.0f) {
		//heal up the actor
		float fCurHealth = actor->getHealth();
		float fMaxHealth = actor->getMaxHealth();
		float fMaxBossHealth = actor->max_boss_health;

		if (fMaxBossHealth < fMaxHealth) {
			fMaxBossHealth = fMaxHealth;
		}
		
		str descr1 = gpm->getStringValue(type, "Description1");
		//str nameActor = actor->getName();
				
		str sPrint = "";
		if (descr1.length()) {
			sPrint += "$$";
			sPrint += descr1;
			sPrint += "$$";
		}
		else {
			sPrint += interactiveType;
		}
		
		if (fCurHealth < fMaxBossHealth) {
			actor->setHealth(fMaxBossHealth);

			if (player->upgPlayerHasLanguageGerman()) {
				player->hudPrint(va("^2Sie haben ^5geheilt^2: ^5%s\n", sPrint.c_str())); 
			}
			else {
				player->hudPrint(va("^2You ^5healed^2: ^5%s\n", sPrint.c_str()));
			}

			player->entityVars.SetVariable("coopHealMessage", level.time);
		}
		else {
			if ((coop_returnEntityFloatVar((Entity*)player, "coopHealMessage") + 3) < level.time) {
				if (player->upgPlayerHasLanguageGerman()) {
					player->hudPrint(va("^2Gesundheit bereits ^5maximal^2 fuer^2:^5 %s\n", sPrint.c_str()));
				}
				else {
					player->hudPrint(va("^2Health already ^5maxed out^2 for^2:^5 %s\n", sPrint.c_str()));
				}
				player->entityVars.SetVariable("coopHealMessage", level.time);
			}
		}


		/* TEST CODE
		Event* event = new Event(EV_Killed);
		event->AddEntity(player);
		event->AddInteger(0);
		event->AddEntity(player);
		event->AddInteger(MOD_SUICIDE);
		player->ProcessEvent(event);*/
	}
}

//================================================================
// Name:        coop_actorDeadBodiesRemove
// Class:       -
//              
// Description: Removes all dead bodies on the map (that are related to this feature)
//              
// Parameters:  NONE
//              
// Returns:     VOID
//              
//================================================================
void coop_actorDeadBodiesRemove()
{
	float fDelay = 0;
	Entity *ent;
	
	for (int i = 1; i <= SentientList.NumObjects(); i++) {
		ent = SentientList.ObjectAt(i);
		if (ent->isSubclassOf(Actor) && ent->deadflag == DEAD_DEAD && ent->targetname == "coop_deadActorBodies") {
			ent->targetname = "";
			ent->PostEvent(EV_Actor_Fade, fDelay);
			fDelay += 0.25;
		}
	}
}

//================================================================
// Name:        coop_actorDeadBodiesHandle
// Class:       -
//              
// Description: Inizialized all coop gamevars, to prevent accessing empty vars
//              
// Parameters:  NONE
//              
// Returns:     VOID
//              
//================================================================
bool coop_actorDeadBodiesHandle(Entity* actor)
{
	//[b60011] chrissstrahl - fix this making issues in singleplayer
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		return false;
	}

	#define COOP_MAX_DEADBODIES 50
	#define COOP_MAX_DEADBODIES_ARRAY_SIZE 256
	#define COOP_MIN_DEADBODIES_AREA_DIST_2D 4048
	#define COOP_MIN_DEADBODIES_VERT_DIST 256
	#define COOP_MAX_DEADBODIES_ROUNDS 1048 //[b60013] chrissstrahl - Changed to 1048 - it did hit 1000 rounds mark

	//do not fade out if we want dead bodies to stay
	if (game.coop_deadBodiesPerArea > 0) {
		Event *rename_event;
		rename_event = (Event *)new Event(EV_SetTargetName);
		rename_event->AddString("coop_deadActorBodies");
		actor->PostEvent(rename_event, 3.0f);
	}

	//[b607] chrissstrahl - retargetname the actor - this allowes for the scripts to continue while the
	//the actor body can stay on the map - if they have no deatheffect like deathfade...
	// deactivated - because we get gamestate issues -  on m4l1a
	//client shows as connecting but is then dropped with the above error
	if (game.coop_deadBodiesPerArea > 0 || game.coop_deadBodiesAllNum > 0) {
		//do not send this entity to client anymore
		/* Made them dissaper for client all together but still in gamestate
		extern Event EV_EntitySVFlags;
		rename_event = (Event *)new Event(EV_EntitySVFlags);
		rename_event->AddString("-broadcast");
		rename_event->AddString("+noclient");
		PostEvent(rename_event, 3.1f);
		*/

		str classname;
		Entity *ent;

		extern Event EV_SelfDetonate;

		int removeArray[COOP_MAX_DEADBODIES_ARRAY_SIZE];
		int removeArrayIndex = 0;

		int iAllActorsNum = 1;
		game.coop_deadBodiesAllNum = 0;//keep track of the bodies, so we can remove them once coop_deadBodiesPerArea is set to 0

		//get actors and check if they match the remove criteria
		for (int i = 1; i <= SentientList.NumObjects(); i++) {
			ent = SentientList.ObjectAt(i);

			//we handle only actors in here
			if (!ent->isSubclassOf(Actor) || ent == actor) {
				continue;
			}
			assert(ent);
			iAllActorsNum++;

			if (ent->deadflag == DEAD_DEAD && ent->targetname == "coop_deadActorBodies") {
				game.coop_deadBodiesAllNum++; //keep track of the bodies, so we can remove them once coop_deadBodiesPerArea is set to 0
				//ent->getHealth()

				//check if actors are in the same area or if they are connected
				if (actor->edict->areanum == ent->edict->areanum /*|| gi.AreasConnected(edict->areanum, ent->edict->areanum)*/) {

					//check vertical distance
					Vector vZcheck1 = actor->origin;
					vZcheck1[0] = 0.0f;
					vZcheck1[1] = 0.0f;
					Vector vZcheck2 = ent->origin;
					vZcheck2[0] = 0.0f;
					vZcheck2[1] = 0.0f;
					Vector vHorizontal = (vZcheck1 - vZcheck2);

					//check horizontal distance
					Vector v3Dimensional = (actor->origin - ent->origin);

					//Actor * a = (Actor *)ent;

					if (vHorizontal.length() < COOP_MIN_DEADBODIES_VERT_DIST && v3Dimensional.length() < COOP_MIN_DEADBODIES_AREA_DIST_2D /*&& (a->last_time_active + 60) < level.time */) {
						if (removeArrayIndex < COOP_MAX_DEADBODIES_ARRAY_SIZE) {
							removeArray[removeArrayIndex] = ent->entnum;
							removeArrayIndex++;
							//gi.Printf("Added actor to array: %i\n", ent->entnum);
						}
					}
				}
			}
		}

		//if we need to (limit hit) remove actors starting with the oldest death
		int iRemoveAble = removeArrayIndex;
		int iRounds = 0;

		while (iRemoveAble > game.coop_deadBodiesPerArea || iAllActorsNum > COOP_MAX_DEADBODIES) { //if max deadbodies for area is hit or overall
			iRounds++;
			int iMostDecayedTime = (level.time + 1);
			Entity* eRem = NULL;
			Actor * eRemoveMe = NULL;

			//get the one that has been decaying for the longest ammount of time
			for (int iRemoveIdx = 0; iRemoveIdx < removeArrayIndex; iRemoveIdx++) {
				eRem = (Actor *)(Entity *)G_GetEntity(removeArray[iRemoveIdx]);
				if (eRem) {
					Actor * a = (Actor *)eRem;
					if (a) {
						if (a->last_time_active < iMostDecayedTime && a->targetname == "coop_deadActorBodies") {
							iMostDecayedTime = a->last_time_active;
							eRemoveMe = a;
						}
					}
				}
			}

			//if there is a actor to remove, just Nike it, ow, I meant "just do it"
			if (eRemoveMe) {
				//gi.Printf("Added to remove: %i\n", eRemoveMe->entnum);
				iRemoveAble--;
				eRemoveMe->targetname = ""; //"coop_deadActorBodiesRemoved";
				eRemoveMe->PostEvent(EV_Actor_Fade, 0.0f);
			}
			//make sure it won't get us stuck
			if (iRounds > COOP_MAX_DEADBODIES_ROUNDS) {
				gi.Printf("==== COOP CODE ERROR ====\nActor::Killed\nHandle dead bodies was stuck more than 1000 rounds before abbort!\n==== END ====\n");
				break;
			}
		}
	}

	//return info that this body will not stay
	if (game.coop_deadBodiesPerArea < 1) {
		return false;
	}
	//return info that this body will stay
	return true;
}


//================================================================
// Name:       Actor::Coop_Killed -> actor.h/actor.cpp -> void Actor::Killed( Event *ev )
// Class:       -
//              
// Description: Adds points to the player score and possibly revives a player
//              
// Parameters:  Player*
//              
// Returns:     VOID
//              
//========================================================[b60013]
void Actor::coop_Killed(Player *player)
{
	if (!game.coop_isActive) {
		return;
	}

	multiplayerManager._awardSystem->coop_awardEnemyKilled(player, this);
	multiplayerManager.addPoints(player->entnum, 1);
	multiplayerManager.addKills(player->entnum, 1);

	//hzm coop mod chrissstrahl, if player is down, this is his chance to get up again
	if (player->coop_playerNeutralized())
	{
		player->coop_playerNeutralized(false);
		player->health = (player->max_health / 2);
		player->upgPlayerDisableUseWeapon(false);
		player->coopPlayer.reviveCounter = 0;
		if (player->coopPlayer.lastMass > 0) {
			player->mass = player->coopPlayer.lastMass;
		}
		player->coopPlayer.reviveCounter = 0;
		player->coopPlayer.lastTimeRevived = 0.0f;
	}
}

//================================================================
// Name:       Actor::coop_ArmorDamage -> actor.h/actor.cpp -> void Actor::ArmorDamage( Event *ev )
// Class:       -
//              
// Description: Handles damage to actor from a player based on number of player in game
//              
// Parameters:  Entity*, float
//              
// Returns:     VOID             
//========================================================[b60013]
void Actor::coop_ArmorDamage(Entity* enemy,float &damage)
{
	if (!game.coop_isActive || !enemy->isSubclassOf(Player)) {
		return;
	}

	if (damage < max_health) {//only check if enemy can't be killed withg one hit
		int iPlayerNum = coop_returnPlayerQuantityInArena();

		float fMultiplicator;

		if (iPlayerNum <= 1) fMultiplicator = 1.0f;
		else if (iPlayerNum == 2)fMultiplicator = 0.90f;
		else if (iPlayerNum == 3)fMultiplicator = 0.85f;
		else if (iPlayerNum == 4)fMultiplicator = 0.80f;
		else if (iPlayerNum == 4)fMultiplicator = 0.75f;
		else fMultiplicator = 0.70f;

		damage = (damage * fMultiplicator);
	}
}