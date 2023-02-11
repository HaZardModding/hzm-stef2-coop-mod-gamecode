//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING SPAWNLOCATION RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-------------------------------------------------------------------------//[b60013]
#ifndef __COOPSPAWNLOCATION_HPP__
#define __COOPSPAWNLOCATION_HPP__

#include "_pch_cpp.h"

class CoopSpawnlocation
{
public:
	void		init();
	Entity*		getSpawnPoint(Player* player, Entity* spawnPoint);
	bool		placeAtSpawnPoint(Player* player);
	bool		transportToSpawnPoint(Player* player);
private:
};

#endif