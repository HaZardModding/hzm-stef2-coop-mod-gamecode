//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING SPAWNLOCATION RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-------------------------------------------------------------------------//[b60013]

#include "_pch_cpp.h"
#include "coopReturn.hpp"
#include "coopArmory.hpp"
#include "coopObjectives.hpp"
#include "coopParser.hpp"
#include "coopScripting.hpp"

#include "coopSpawnlocation.hpp"
CoopSpawnlocation coopSpawnlocation;

void CoopSpawnlocation::init()
{
}

//returns spawnspot entity, eigther a temp ewntity with script coords or pre-existing one
Entity* CoopSpawnlocation::getSpawnPoint(Player* player, Entity* spawnPoint)
{
	bool respawning			= player->coopPlayer.respawned;
	Entity* spawnCoop		= NULL;
	Vector vAngles			= Vector(0.0f,0.0f,0.0f);
	Vector vSpawnOrigin		= Vector(0.0f, 0.0f, 0.0f);

	if (game.coop_isActive || !spawnPoint) {
		//if rpg and valid spawnlocation found - exit
		if (spawnPoint && game.levelType == MAPTYPE_RPG) {
			return spawnPoint;
		}

		//make player - not solid to prevent em getting stuck
		player->setSolidType(SOLID_NOT);
		player->_makeSolidASAP = true;
		player->_makeSolidASAPTime = 0.25f;

		//FORCE SPAWN AT SPAWNLOCATION
		//- lookup coop_playerPlaceAtSpawn if we need more variables
		if (player->entityVars.GetVariable("coop_respawnAtRespawnpoint")) {
			player->entityVars.SetVariable("coop_respawnAtRespawnpoint", 0.0f);
			respawning = false;
		}

		//RESPAWN IN PLACE - restore player last active location and angle
		if (respawning) {
			//grab player angles from before
			vAngles  = Vector(0.0f, 0.0f, 0.0f);
			vAngles[1] = player->coopPlayer.deathViewangleY;
			player->setAngles(vAngles);
			player->origin = player->coopPlayer.lastAliveLocation;
			spawnCoop = player;
			return spawnCoop;
		}

		//CHECK IF LEVEL HAS SCRIPT SPAWNS - then abuse player as his own spawnlocation
		if (level.game_script != "") {
			vSpawnOrigin = program.getVectorVariableValue(va("coop_vector_spawnOrigin%i", (1 + player->entnum)));
			if (vSpawnOrigin[0] != 0.0f || vSpawnOrigin[1] != 0.0f || vSpawnOrigin[2] != 0.0f) {
				float fSpawnAngle = program.getFloatVariableValue(va("coop_float_spawnAngle%i", (1 + player->entnum)));
				if (fSpawnAngle == 0.0f) {
					if (program.getFloatVariableValue("coop_float_spawnAngle0") != 0.0f) {
						fSpawnAngle = program.getFloatVariableValue("coop_float_spawnAngle0");
					}
				}
				vAngles = Vector(0.0f, 0.0f, 0.0f);
				vAngles[1] = fSpawnAngle;
				player->origin = vSpawnOrigin;
				player->setAngles(vAngles);
				spawnCoop = player;
			}
		}

		//CHECK IF THERE IS A TARGETNAMED SPAWN
		if (!spawnCoop) {
			Entity* ent;
			TargetList* tlist;
			tlist = world->GetTargetList(va("ipd%i", (1 + player->entnum)), false);
			if (tlist) {
				ent = tlist->GetNextEntity(NULL);
				spawnCoop = ent;
			}
		}
		
		//has coop specific spawn spot
		if (spawnCoop) {
			spawnPoint = spawnCoop;
		}
		//does not have coop specific spawn spot
		else {
			//see if we can find singleplayer spawn
			if (!spawnPoint) {
				spawnPoint = G_FindClass(NULL, "info_player_start");
				if(!spawnPoint) {
					gi.Printf("ERROR: No info_player_start found on level - Player starting at '0 0 0'\n");
				}
			}
		}
	}

	return spawnPoint;
}

bool CoopSpawnlocation::placeAtSpawnPoint(Player* player)
{
	Entity* spawnPoint;
	spawnPoint = getSpawnPoint(player,NULL);
	
	if (spawnPoint){
		player->WarpToPoint(spawnPoint);
		return true;
	}
	return false;
}