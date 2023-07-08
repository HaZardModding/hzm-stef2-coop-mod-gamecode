//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING FORCEFIELD RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-------------------------------------------------------------------------//[b60013]

#include "_pch_cpp.h"
#include "mp_manager.hpp"
#include "weaputils.h"

#include "coopReturn.hpp"
#include "coopArmory.hpp"
#include "coopObjectives.hpp"
#include "coopParser.hpp"
#include "coopScripting.hpp"

#include "coopForcefield.hpp"
CoopForcefield coopForcefield;



//[b60013] chrissstrahl - set/reset vars upon each map load
void CoopForcefield::init()
{
}

//Allow specific weaponfire to pass
bool CoopForcefield::passthroughBullettAtack(Entity* owner, trace_t &trace, const Vector &start, Vector &end,int &meansofdeath)
{
	Entity* eTrace = NULL;
	if (trace.ent) {
		eTrace = trace.ent->entity;
	}

	if (!eTrace || !eTrace->isSubclassOf(ScriptSlave) || !owner->isSubclassOf(Player) || meansofdeath != MOD_PHASER /* && meansofdeath != MOD_VAPORIZE*/) {
		//gi.Printf(va("NO-MATCH : BulletAttack Forcefield NOT ALL REQ MEET\n"));
		return false;
	}

	Entity *ent;
	trace_t trace2 = trace;
	ScriptSlave* forceField = (ScriptSlave*)eTrace;

	ScriptVariable* var = NULL;
	var = owner->entityVars.GetVariable("globalCoop_scannedCoopForcefieldFreq");
	if (!var || !strlen(var->stringValue()) || atof(var->stringValue()) <= 0.00) {
		return false;
	}

	str sFfFrequency = va("%.2f", forceField->_forcefieldNumber);
	//int surfaceFlags = trace.surfaceFlags;	// surface hit
	//int contents = trace.contents;	   // contents on other side of surface hit

	//frequency does not match
	if (sFfFrequency != var->stringValue()) {
		//gi.Printf(va("NO-MATCH : BulletAttack Forcefield Frequency -> %s , %s\n", sFfFrequency.c_str(), var->stringValue()));
		return false;
	}

	//Event* even1 = new Event(EV_BecomeNonSolid);
	//eTrace->PostEvent(even1, 0.0f);

	eTrace->CancelEventsOfType(EV_BecomeSolid);
	eTrace->ProcessPendingEvents();
	eTrace->setSolidType(SOLID_NOT);
	//trace.contents = CONTENTS_WATER;
	//trace.surfaceFlags = SURF_NONSOLID;

	if (!multiplayerManager.inMultiplayer() || multiplayerManager.fullCollision()) {
		trace = G_FullTrace(start, vec_zero, vec_zero, end, owner, MASK_SHOT, false, "BulletAttack");
	}else {
		trace = G_Trace(start, vec_zero, vec_zero, end, owner, MASK_SHOT, false, "BulletAttack");
	}

Event* even2 = new Event(EV_BecomeSolid);
eTrace->PostEvent(even2, 0.01f);
	//eTrace->setSolidType(SOLID_BSP);
	//trace.contents = contents;
	//trace.surfaceFlags = surfaceFlags;

	//DEBUG PRINTOUT
	if (trace.ent) {
		ent = trace.ent->entity;
		//gi.Printf(va("BulletAttack Forcefield passed -> '$%s'\n", ent->targetname.c_str()));		
	}
	//else {
		//gi.Printf("BulletAttack Forcefield passed -> NULL\n");
	//}

	return false;
}

//When a forcefield gets scanned
void CoopForcefield::scan(Entity* owner, Equipment* scanner)
{
	Vector start;
	Vector end;
	trace_t trace;

	Player* player = (Player*)owner;
	player->GetViewTrace(trace, MASK_SHOT, 5000.0f);
	start = trace.endpos;
	end = start + Vector(0.0f, 0.0f, 5000.0f);
	
	if (!trace.ent) {
		return;
	}

	Entity* eFF;
	eFF = trace.ent->entity;
	if (eFF->isSubclassOf(ScriptSlave)) {
		ScriptSlave* forceField = (ScriptSlave*)eFF;
		if (forceField->_forcefieldNumber > 0.0f) {
			//player->hudPrint(va("FF FREQ: %.2f\n", forceField->_forcefieldNumber));
			player->addHud("coop_tricorderFreq");
			player->entityVars.SetVariable("globalCoop_scannedCoopForcefieldFreq",va("%.2f",forceField->_forcefieldNumber));
			gi.SendServerCommand(player->entnum, va("stufftext \"globalwidgetcommand coop_tricorderFreq1 title %.2f\"\n",forceField->_forcefieldNumber));
		}
	}
}

//When a forcefield gets scanned no more
void CoopForcefield::scanEnd(Entity* owner, Equipment* scanner)
{
	Player* player = (Player*)owner;
	player->removeHud("coop_tricorderFreq");
	//player->hudPrint("stop\n");
}