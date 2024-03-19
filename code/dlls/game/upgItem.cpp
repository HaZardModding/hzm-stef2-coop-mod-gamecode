//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// ITEM Class related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------
#include "upgItem.hpp"
UpgItem upgItem;

#include "item.h"

void UpgItem::upgItemNoShadow(Item* item)
{
	if (item) {
		item->edict->s.renderfx &= ~RF_SHADOW;
		item->edict->s.renderfx &= ~RF_EXTRALIGHT;
		item->edict->s.renderfx &= ~RF_SHADOW_PRECISE;
	}
}