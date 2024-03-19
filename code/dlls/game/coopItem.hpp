//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING ITEM RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"

//[b60011] chrissstrahl
class CoopItem
{
private:
public:
	void init();
	bool coopItemSingleplayerHandling();
	bool coopItemGoldenShipFound(Player* playerFinder);
};

extern CoopItem coopItem;