//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING ITEM RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#include "coopItem.hpp"
CoopItem coopItem;

#include "game.h"
#include "_pch_cpp.h"
#include "mp_manager.hpp"

#include "upgCoopInterface.hpp"

#include "coopServer.hpp"
#include "coopReturn.hpp"
#include "coopParser.hpp"

void CoopItem::init()
{

}

bool CoopItem::coopItemSingleplayerHandling()
{
	return game.coop_isActive;
}

//[b60014] chrissstrahl - print message to all player during coop
bool CoopItem::coopItemGoldenShipFound(Player *playerFinder)
{
	if (!playerFinder) { return false; }

	if (game.coop_isActive && multiplayerManager.inMultiplayer()) {
		
		playerFinder->AddHealth( 200 );

		Event* armorEvent;
		armorEvent = new Event(EV_Sentient_GiveArmor);
		armorEvent->AddString("BasicArmor");
		armorEvent->AddInteger( 100 );
		playerFinder->ProcessEvent(armorEvent);

		for (int i = 0; i < maxclients->integer; i++) {
			if (&g_entities[i] && g_entities[i].client && g_entities[i].inuse) {
				Player* player = (Player*)g_entities[i].entity;

				if (player) {
					//print Message
					str sMessage = COOP_TEXT_SECRETSHIP_FOUND_BY_ENG;
					if (player->upgPlayerHasLanguageGerman()) {
						sMessage = COOP_TEXT_SECRETSHIP_FOUND_BY_DEU;
					}
					gi.centerprintf(player->edict, CENTERPRINT_IMPORTANCE_NORMAL, va("%s%s", sMessage.c_str(), playerFinder->client->pers.netname));
					
					//revive during LMS
					if (playerFinder != player) {
						if (coop_lmsRevivePlayer(player)) {
							//print info to player so they know how many lives they have
							player->coop_lmsInfo();
						}
					}
				}
			}
		}
		return false;
	}
	return true;
}