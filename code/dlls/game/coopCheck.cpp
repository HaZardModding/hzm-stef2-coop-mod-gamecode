//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING CHECK-FUNCTIONS, RETURNING EIGTHER FALSE OR TRUE
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"

#include "coopCheck.hpp"
#include "coopReturn.hpp"
#include "coopServer.hpp"

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

			if ( entity2->edict->solid == SOLID_BSP && entity1->upgEntityInBoundingBox( entity2 ) )
			{
				return true;
			}
		}
	}
	return false;
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
		player->coop_setId(sClientId);
	}
	//make sure it is empty
	else {
		player->coop_setId("");
	}

	//check if it is in ini
	//client id was provided
	str sData = coop_parserIniGet(coopServer.getServerDataIniFilename(), sClientId, "client");
	//client id is not known on the server
	if (!sData.length()) {
		coop_playerSaveNewPlayerId(player);		
	}
	//client is in ini, restore
	else {
		player->coop_setId(sClientId);
		coop_playerRestore(player);
	}
	return player->coop_getId();
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


