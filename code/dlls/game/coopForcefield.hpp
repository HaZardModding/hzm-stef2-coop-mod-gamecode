//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING FORCEFIELD RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-------------------------------------------------------------------------//[b60013]
#ifndef __COOPFORCEFIELD_HPP__
#define __COOPFORCEFIELD_HPP__

#include "_pch_cpp.h"

class CoopForcefield
{
public:
	void init();
	bool passthroughBullettAtack(Entity* owner, trace_t &trace, const Vector &start, Vector &end,int &meansofdeath);
	void scan(Entity* owner, Equipment* scanner);
	void scanEnd(Entity* owner, Equipment* scanner);
private:
};

#endif