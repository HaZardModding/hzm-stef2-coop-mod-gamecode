//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// WORLD Class related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#include "upgWorld.hpp"
UpgWorld upgWorld;

#include "upgEntity.hpp"
#include "upgGame.hpp"
#include "upgStrings.hpp"
#include "upgCoopInterface.hpp"

#include "worldspawn.h"
#include "mp_manager.hpp"


//-----------------------------------------------------------------------------------
// Events, these have external Dependencies
//-----------------------------------------------------------------------------------
Event EV_World_GetPhysicsVar
(
	"getPhysicsVar",
	EV_SCRIPTONLY,
	"@fs",
	"return-float physicsvar-name",
	"returns gravity, airaccelerate and maxspeed Physics values"
);

//================================================================
// Name:        UpgWorld
// Class:       UpgWorld
//              
// Description:
//              
// Parameters:  void
//              
// Returns:     void     
//================================================================
UpgWorld::UpgWorld()
{
	flushTikiMaps[0] = "m2l0-sfa";
	flushTikiMaps[1] = "m4l1a-attrexian_station";
	flushTikiMaps[2] = "m4l2b-attrexian_station";
	flushTikiMaps[3] = "m7l1a-attrexian_colony";
	flushTikiMaps[4] = "m9l1a-klingon_base";
	flushTikiMaps[5] = "m11l1a-drull_ruins3";
}

//=========================================================[b60014]
// Name:        upgWorldAdjustForLevelScript
// Class:       World
//              
// Description:  Called every frame from World
//              
// Parameters:  void
//              
// Returns:     void     
//================================================================
void UpgWorld::upgWorldThink()
{
	//3 sec interval
	if ((thinkLastInterval + 3) < level.time) {
		if (level.time < (upgGame.upgGameGetReconnectTime() + mp_warmUpTime->integer)) {
			ScriptVariable* entityData = NULL;
			entityData = world->entityVars.GetVariable("upg_playersReconnecting");
			if (entityData && entityData->intValue() > 0) {
				multiplayerManager.centerPrintAllClients("=/\\= Please Standby =/\\=\nWaiting for reconnecting Players.", CENTERPRINT_IMPORTANCE_CRITICAL);
			}
		}
		thinkLastInterval = level.time;
	}
}
	
//================================================================
// Name:        upgWorldAdjustForLevelScript
// Class:       World
//              
// Description: [b60014] chrissstrahl - checks if the levelscript might end with .script instead of .scr
//				modifies string if that is so, basically changes extenstion to .script then
//              
// Parameters:  void
//              
// Returns:     void     
//================================================================
void World::upgWorldAdjustForLevelScript(str &mapname)
{
	if (gi.FS_ReadFile(mapname.c_str(), NULL, true) == -1) {
		if (gi.FS_ReadFile(va("%sipt", mapname.c_str()), NULL, true) != -1) {
			mapname = va("%sipt", mapname.c_str());
		}
	}
}

//================================================================
// Name:        upgWorldViewmodesClear
// Class:       World
//              
// Description: [b6xx] 
//              
// Parameters:  void
//              
// Returns:     void     
//================================================================
void World::upgWorldViewmodesClear()
{
	_availableViewModes.ClearObjectList();
	_availableViewModes.AddUniqueObject("BogusMode");
}


//================================================================
// Name:        upgWorldGetPhysicsVar
// Class:       World
//              
// Description: [b60013] allow to return physics var value via script
//              
// Parameters:  void
//              
// Returns:     void     
//================================================================
void World::upgWorldGetPhysicsVar(Event* ev) {
	float fValue = -1;
	str sName = ev->GetString(1);
	if (sName.length()) {
		fValue = getPhysicsVar(sName.c_str());
		if (fValue == -1) {
			sName.tolower();

			if (sName == "maxspeed") { sName = "sv_maxspeed"; }
			else if (sName == "airaccelerate") { sName = "sv_airaccelerate"; }
			else if (sName == "gravity") { sName = "sv_gravity"; }
			else { gi.Printf("getPhysicsVar - unknown Physics Var Name: %s\nKnown names are: maxspeed, airaccelerate and gravity\n", sName.c_str()); }
			cvar_t* cvar = gi.cvar_get(sName.c_str());

			if (cvar) {
				ev->ReturnFloat((float)cvar->integer);
				return;
			}
		}
	}
	ev->ReturnFloat(fValue);
}

//================================================================
// Name:        upgWorldUpdateDynamicLights
// Class:       World
//              
// Description: [b607] chrissstrahl - make the dynamic lights work for players that join midgame / dedicated server
//				Turns all dynamic lights that are on off, so they update correctly for the client
//				They will be updated again in the next sv frame for the player again
//				From: World::UpdateDynamicLights() 
//              
// Parameters:  void
//              
// Returns:     bool     
//================================================================
bool World::upgWorldUpdateDynamicLights()
{
	if (upgWorld.upgWorldGetUpdateDynamicLights()) {
		upgWorld.upgWorldSetUpdateDynamicLights(false);
		for (int i = 0; i < MAX_LIGHTING_GROUPS; i++) {
			gi.SetDynamicLight(i, 0.0f);
			gi.SetDynamicLightDefault(i, 0.0f);
			/*
			if (dynamic_lights[i].intensity > 0.0f) {
				gi.SetDynamicLight(i, 0.0f);
				gi.Printf("SetDynamicLight: %i\n",i);
			}
			if (dynamic_lights[i].defaultIntensity) {
				gi.SetDynamicLightDefault(i, 0.0f);
				gi.Printf("SetDynamicLightDefault: %i\n", i);
			}
			*/
		}
		return true;
	}
	return false;
}

//================================================================
// Name:        upgWorldGetUpdateDynamicLights
// Class:       UpgWorld
//              
// Description: returns if dynamic lights need updating
//              
// Parameters:  void
//              
// Returns:     bool     
//================================================================
bool UpgWorld::upgWorldGetUpdateDynamicLights()
{
	return updateDynamicLights;
}

//================================================================
// Name:        upgWorldSetUpdateDynamicLights
// Class:       UpgWorld
//              
// Description: sets if dynamic lights need updating
//              
// Parameters:  bool
//              
// Returns:     void     
//================================================================
void UpgWorld::upgWorldSetUpdateDynamicLights(bool bUpdate)
{
	updateDynamicLights = bUpdate;
}

//================================================================
// Name:        upgWorldSetPlayersReconnecting
// Class:       World
//              
// Description: sets if we should wait for players that are reconnecting
//              
// Parameters:  bool
//              
// Returns:     void     
//================================================================
void UpgWorld::upgWorldSetPlayersReconnecting(bool reconnecting)
{
	//do not allow to reconnect during diagnose mode
	if(reconnecting && upgCoopInterface.upgCoopInterfaceDiagnoseRunning()) {
		return;
	}

	world->entityVars.SetVariable("upg_playersReconnecting", (float)reconnecting);		//stays like that during the entire level
	world->entityVars.SetVariable("upg_playersReconnectingWait", (float)reconnecting);	//used to signal scripts - will be set to 0 after 10 sec or so on level
}

//================================================================
// Name:        upgWorldGetPlayersReconnecting
// Class:       World
//              
// Description: gets if we should wait for players that are reconnecting
//              
// Parameters:  void
//              
// Returns:     bool     
//================================================================
bool UpgWorld::upgWorldGetPlayersReconnecting()
{
	if (upgCoopInterface.upgCoopInterfaceDiagnoseRunning()) {
		return false;
	}
	return (bool)upgEntity.upgEntityGetIntegerVar(world, "upg_playersReconnecting");
}

//================================================================
// Name:        upgWorldFlushTikisLevelStart
// Class:       UpgWorld
//              
// Description: handle flushtikis on level start
//              
// Parameters:  void
//              
// Returns:     bool     
//================================================================
void UpgWorld::upgWorldFlushTikisLevelStart()
{
	thinkLastInterval = 0.0f;
	bool bFlush = false;

	for (int i = 0; i < UPGWORLD_FLUSHTIKI_MAPLISTSIZE;i++) {
		if (upgStrings.contains(level.mapname.tolower(), flushTikiMaps[i])) {
			bFlush = true;
			break;
		}
	}
	if (bFlush) {
		upgGame.flushTikisServer();

		//This is not done if the server has just started
		if (upgGame.upgGameGetMapsLoadedSinceReconnect() > 0) {
			upgWorldSetPlayersReconnecting(true);
			upgGame.upgGameSetReconnectTime(upgGame.upgGameGetCvarReconnectTime());
			upgGame.upgGameResetMapsLoaded();
			//Clear handling of "upg_playersReconnectingWait" is in: void UpgGame::upgGameStartMatch()
		}
	}
	else {
		upgWorldSetPlayersReconnecting(false);
	}
	upgGame.upgGameCountMapsLoaded();
}