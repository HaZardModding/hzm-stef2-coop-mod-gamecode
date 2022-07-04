//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING CHECK-FUNCTIONS, RETURNING EIGTHER FALSE OR TRUE

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]


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
		if ( a && a->isSubclassOf( Actor) && !a->GetActorFlag( ACTOR_FLAG_AI_ON ) && a->hidden() ) {
			continue;
		}

		//player inside actor or another plyer, make sure the other is also set not solid
		if (coop_checkIsEntityInBoundingBox( entity1 , entity2 ) ) {
			entity2->setSolidType( SOLID_NOT );
			entity2->_makeSolidASAP = true;
			entity2->_makeSolidASAPTime = 0.0f;
			//entity2->_makeSolidASAPSupposedToBeSolid = true;
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
// Name:        coop_checkDoesPlayerHaveCoopMod
// Class:       -
//              
// Description:  checks if the given player does have the coop mod installed or not
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_checkDoesPlayerHaveCoopMod( Player *player )
{
	//if player has coop or if there was a sufficent ammount of time passed
	if (player->coopPlayer.installed != 0 || player->coopPlayer.setupTries == 12) {
		return;
	}

	//if in singleplayer, player does ofcourse have the coop mod
	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		player->coopPlayer.installed = 1;
		coop_playerSetupCoop( player );
		return;
	}
	//in multiplayer do the checking procedure
	else {
		//have some time delay and also make sure the player is even able to process any commands
		if (player->coopPlayer.installedCheckTime < level.time) {
			player->coopPlayer.installedCheckTime = (level.time + 0.25f);
			if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32) {
				player->coopPlayer.setupTries++;
			}
		}
	}

	//player does not have coop mod - give up at this point
	if (player->coopPlayer.setupTries == 11) {
		coop_playerSetupNoncoop(player);
		player->coopPlayer.setupTries++;
		return;
	}
}

//================================================================
// Name:        coop_checkDoesPlayerHaveCoopId
// Class:       -
//              
// Description:  checks if the given player does have a coop mod id
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_checkDoesPlayerHaveCoopId(Player* player)
{
	//if player has coop or if there was a sufficent ammount of time passed
	if (player->coopPlayer.coopId.length() || player->coopPlayer.setupTriesCid == 12) {
		return;
	}

	//have some time delay and also make sure the player is even able to process any commands
	if (player->coopPlayer.setupTriesCidCheckTime < level.time) {
		player->coopPlayer.setupTriesCidCheckTime = (level.time + 0.25f);
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32) {
			player->coopPlayer.setupTriesCid++;
		}
	}
	//player does not have coop mod - give up at this point
	if (player->coopPlayer.setupTriesCid == 11) {
		coop_playerSaveNewPlayerId(player);
		player->coopPlayer.setupTriesCid++;
		return;
	}
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


