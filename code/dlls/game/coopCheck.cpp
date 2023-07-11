//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING CHECK-FUNCTIONS, RETURNING EIGTHER FALSE OR TRUE
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"

#include "coopCheck.hpp"
#include "coopReturn.hpp"

#include "entity.h"
#include "player.h"
#include "weapon.h"
#include "mp_manager.hpp"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include "powerups.h"
#include "interpreter.h"
#include "program.h"

//================================================================
// Name:        coop_checkPlayerLanguageGerman
// Class:       -
//              
// Description:  check if player has german game version
//              
// Parameters:  Player *player
//              
// Returns:     bool
//              
//================================================================
bool coop_checkPlayerLanguageGerman(Player *player)
{
	if (player && player->getLanguage() == "Deu")
	{
		return true;
	}
	return false;
}

//================================================================
// Name:        coop_checkCharsInsideString
// Class:       -
//              
// Description:  checks if any given char is inside the first string
//              
// Parameters:  Player *player
//              
// Returns:     bool
//              
//================================================================
bool coop_checkCharsInsideString(str sSource, str sObjectsOfIntrest)
{
	int iLen = strlen( sObjectsOfIntrest );
	int i = 0;
	do
	{
		char c = sObjectsOfIntrest[i];
		if (c != NULL && strstr( sSource, (str)c ) != NULL )
		{
			return true;
		}
		i++;
	} while ( i < iLen );
	return false;
}
//================================================================
// Name:        coop_checkInsidePlayerOrActor
// Class:       -
//              
// Description:  checks if the given player should be made solid or if he is still inside another player or any actor
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
bool coop_checkInsidePlayerOrActor( Entity *entity1 )
{
	//return if not existing
	if ( !entity1 )
		return false;

	//exit if player or actor but dead
	if ( entity1->isSubclassOf( Player ) && entity1->health <= 0.0f || !entity1->isSubclassOf( Actor ) && entity1->health <= 0.0f )
		return false;

	//exit if player but spectator
	if ( entity1->isSubclassOf( Player ) && multiplayerManager.isPlayerSpectator( ( Player * )entity1 ) )
		return false;

	int i;
	Entity *entity2=NULL;

	//check (all) entities against this entity
	for ( i = 0; i < maxentities->integer; i++ ) {

		entity2 = g_entities[i].entity;

		//skip if missing,same,not player/actor or dead
		if ( !entity2 || entity1 == entity2 )
			continue;

		if ( !entity2->isSubclassOf( Player ) && !entity2->isSubclassOf( Actor ) )
			continue;

		if ( entity2->health <= 0 )
			continue;

		if ( entity2->isSubclassOf( Player ) && multiplayerManager.isPlayerSpectator( ( Player * )entity2 ) )
			continue;

		//do not check actor on actor, because this creates more trouble than it is useful
		if ( entity1->isSubclassOf( Actor ) && entity2->isSubclassOf( Actor ) )
			continue;

		//ai_off and hidden
		//make sure we skip notsolid and hidden actors as they are probably left over from cinematic
		Actor *a = NULL;
		if ( entity1->isSubclassOf( Actor ) ) {
			a = ( Actor * )entity1;
		}else if( entity2->isSubclassOf( Actor ) ){
			a = ( Actor * )entity2;
		}
		if ( a && a->isSubclassOf( Actor) && (!a->GetActorFlag( ACTOR_FLAG_AI_ON ) /* && a->hidden() && a->edict->svflags & SVF_NOCLIENT*/)) {
			continue;
		}

		//player inside actor or another plyer, make sure the other is also set not solid
		if (coop_checkIsEntityInBoundingBox( entity1 , entity2 ) ) {
			entity2->setSolidType( SOLID_NOT );
			entity2->_makeSolidASAP = true;
			entity2->_makeSolidASAPTime = 0.0f;
			return true;
		}
	}
	return false;
}


//================================================================
// Name:        coop_checkEntityInsideDoor
// Class:       -
//              
// Description:  checks if the given entity is inside a door
//              
// Parameters:  Entity *entity1
//              
// Returns:     bool
//              
//================================================================
bool coop_checkEntityInsideDoor( Entity *entity1 )
{
	if ( !entity1 ) {
		return false;
	}

	//ACTOR OR PLAYER DEAD
	if ( entity1->isSubclassOf( Actor ) || entity1->isSubclassOf( Player ) ) {
		if ( entity1->health <= 0 ) {
			return false;
		}

		//HANDLE PLAYER SPECTATOR
		if ( entity1->isSubclassOf( Player ) && multiplayerManager.isPlayerSpectator( ( Player * )entity1 ) ) {
			return false;
		}
	}

	int i;
	Entity *entity2 = NULL;

	//check (all) entities against this entity
	for ( i = maxclients->integer; i < maxentities->integer; i++ ) {
		entity2 = g_entities[i].entity;
		//CHECK IF ENTITY EXISTS
		if ( entity2 ) {

			//DO NOT CHECK AGAINST IT SELF
			if ( entity1 == entity2 || !entity2->isSubclassOf( Door ) ) {
				continue;
			}

			if ( entity2->edict->solid == SOLID_BSP && coop_checkIsEntityInBoundingBox( entity1 , entity2 ) )
			{
				return true;
			}
		}
	}
	return false;
}

//================================================================
// Name:        coop_checkPlayerHasCoop
// Class:       -
//              
// Description:  checks if the given player does have the coop mod installed or not
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_checkPlayerHasCoop( Player *player )
{
	constexpr auto COOP_MAX_MOD_CHECK_TRIES = 15;

	//if player has coop or if there was a sufficent ammount of time passed
	if (player->coop_getInstalled() || player->coopPlayer.setupTries >= COOP_MAX_MOD_CHECK_TRIES) {
		return;
	}

	//[b60014] chrissstrahl - don't handle bots
	if (player->coop_isBot()) {
		player->coop_setInstalled(false);
		player->coopPlayer.setupComplete = true;
		player->coopPlayer.setupTries = (COOP_MAX_MOD_CHECK_TRIES + 1);
		return;
	}

	//in multiplayer do the checking procedure
	//have some time delay and also make sure the player is even able to process any commands
	if (player->coop_getInstalledCheckTime() < level.time) {
		player->coop_setInstalledCheckTime(level.time + 0.25f);
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 96) { //[b60014] chrissstrahl - changed from 32 to 96
			player->coopPlayer.setupTries++;
		}
	}

	//player does not have coop mod - give up at this point
	if (player->coopPlayer.setupTries == COOP_MAX_MOD_CHECK_TRIES ) {
		coop_playerSetupNoncoop(player);
		player->coopPlayer.setupTries++;
		return;
	}
}

//================================================================
// Name:        coop_checkPlayerHasCoopId
// Class:       -
//              
// Description:  checks if the given player does have a coop mod id
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_checkPlayerHasCoopId(Player* player)
{
	constexpr auto COOP_MAX_ID_CHECK_TRIES = 15;

	//if player has coop or if there was a sufficent ammount of time passed
	if (g_gametype->integer == GT_SINGLE_PLAYER || player->coopPlayer.setupTriesCid >= COOP_MAX_ID_CHECK_TRIES || player->coopPlayer.coopId.length()) {
		return;
	}

	//[b60014] chrissstrahl - don't handle bots
	if (player->coop_isBot()) {
		player->coopPlayer.coopId = 0;
		player->coopPlayer.setupTriesCid = (COOP_MAX_ID_CHECK_TRIES + 1);
		return;
	}

	//have some time delay and also make sure the player is even able to process any commands
	if (player->coopPlayer.setupTriesCidCheckTime < level.time) {
		player->coopPlayer.setupTriesCidCheckTime = (level.time + 0.15f);
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 96) { //[b60014] chrissstrahl - chnaged from 32 to 96
			player->coopPlayer.setupTriesCid++;
		}
	}
	//player does not have coop mod - give up at this point
	if (player->coopPlayer.setupTriesCid == COOP_MAX_ID_CHECK_TRIES) {
		player->coopPlayer.setupTriesCid++;
		coop_playerSaveNewPlayerId(player);
		return;
	}
}

//================================================================
// Name:        coop_checkPlayerCoopIdExistInIni
// Class:       -
//              
// Description:  checks if the given clientid does exist in server ini, if not it will create one
//				will return clientid or empty string
//              
// Parameters:  Player *player
//              
// Returns:     string
//              
//================================================================
str coop_checkPlayerCoopIdExistInIni(Player* player, str sClientId)
{
	//client has none given - create new one
	if (!sClientId.length()) {
		coop_playerSaveNewPlayerId(player);
		return sClientId;
	}
	
	//if it could be a valid id - acceept it on this server
	if (sClientId.length() <= 11 && sClientId.length() > 5 && IsNumeric(sClientId)) {
		player->coopPlayer.coopId = sClientId;
	}
	//make sure it is empty
	else {
		player->coopPlayer.coopId = "";
	}

	//check if it is in ini
	//client id was provided
	str sData = coop_parserIniGet("serverData.ini", sClientId.c_str(), "client");
	//client id is not known on the server
	if (!sData.length()) {
		coop_playerSaveNewPlayerId(player);		
	}
	//client is in ini, restore
	else {
		player->coopPlayer.coopId = sClientId.c_str();
		coop_playerRestore(player);
	}
	return player->coopPlayer.coopId;
}

//================================================================
// Name:        coop_checkStringInUservarsOf
// Class:       -
//              
// Description:  checks if the given string is inside any uservar of the given entity
//              
// Parameters:  Entity * ent , str sStringPart
//              
// Returns:     bool
//              
//================================================================
bool coop_checkStringInUservarsOf( Entity * ent , str sStringPart )
{
	if ( !ent || sStringPart.length() < 1 ){
		return false;
	}

	int i;
	str sValue;
	ScriptVariable *entityData;
	for ( i = 1; i < 5; i++ ){
		entityData = NULL;
		entityData = ent->entityVars.GetVariable( va( "uservar%d" , i ) );
		if ( entityData == NULL ){
			continue;
		}
		sValue = "";
		sValue = entityData->stringValue();
		if ( !Q_stricmp( sValue.c_str() , sStringPart.c_str() ) ){
			return true;
		}
	}
	return false;
}

//================================================================
// Name:        coop_checkIsEntityInBoundingBox
// Class:       -
//              
// Description:  checks if given Entity is inside of the boundingbox of the secound given entity
//              
// Parameters:  Entity *eIntruder , Entity *eTheBox 
//              
// Returns:     bool
//              
//================================================================
bool coop_checkIsEntityInBoundingBox( Entity *eIntruder , Entity *eTheBox )
{

	if ( !eIntruder || !eTheBox || eIntruder == eTheBox ||
		( eIntruder->absmin[0] > eTheBox->absmax[0] ) ||
		( eIntruder->absmin[1] > eTheBox->absmax[1] ) ||
		( eIntruder->absmin[2] > eTheBox->absmax[2] ) ||
		( eIntruder->absmax[0] < eTheBox->absmin[0] ) ||
		( eIntruder->absmax[1] < eTheBox->absmin[1] ) ||
		( eIntruder->absmax[2] < eTheBox->absmin[2] ) )
	{
		return false;
	}
	return true;
}


//================================================================
// Name:        coop_checkIsPlayerActiveAliveInBoundingBox
// Class:       -
//              
// Description:  checks if given Player, alive and not in spectator is inside of the boundingbox of the given entity
//              
// Parameters:  Player *player , Entity *eTheBox 
//              
// Returns:     bool
//              
//================================================================
bool coop_checkIsPlayerActiveAliveInBoundingBox( Player *player , Entity *eTheBox )
{
	if ( !player || !eTheBox || player->health <= 0.0f || (Entity *)player == eTheBox ||
		multiplayerManager.isPlayerSpectator( player ) ||
		( player->absmin[0] > eTheBox->absmax[0] ) ||
		( player->absmin[1] > eTheBox->absmax[1] ) ||
		( player->absmin[2] > eTheBox->absmax[2] ) ||
		( player->absmax[0] < eTheBox->absmin[0] ) ||
		( player->absmax[1] < eTheBox->absmin[1] ) ||
		( player->absmax[2] < eTheBox->absmin[2] ) )
	{
		return false;
	}

	return true;
}

//========================================================[b60011]
// Name:        coop_checkCanPickUpItem
// Class:       -
//              
// Description: prevent Sentient from picking up all the items in coop if he already has em
//              
// Parameters:  Entity *entity , str sModel
//              
// Returns:     bool
//              
//================================================================
bool coop_checkCanPickUpItem(Entity* entity, str sModel)
{
	if (game.coop_isActive && sModel.length() && entity->isSubclassOf(Sentient)) {
		Sentient* sentient = (Sentient*)entity;
		if (sentient->HasItem(sModel.c_str())) {
			//gi.Printf(va("Sentient already has: %s\n", this->model.c_str()));
			return false;
		}
	}
	return true;
}


