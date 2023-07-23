#include "worldspawn.h"

#include "upgWorld.hpp"
UpgWorld upgWorld;


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
	if (upgWorld.getUpdateDynamicLights()) {
		upgWorld.setUpdateDynamicLights(false);
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

bool UpgWorld::getUpdateDynamicLights()
{
	return updateDynamicLights;
}
void UpgWorld::setUpdateDynamicLights(bool bUpdate)
{
	updateDynamicLights = bUpdate;
}