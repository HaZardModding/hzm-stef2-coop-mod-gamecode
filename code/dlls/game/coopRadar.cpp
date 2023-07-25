//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER RADAR RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"

#include "coopRadar.hpp"
#include "coopPlayer.hpp"
#include "player.h"
#include "mp_manager.hpp"

void coop_radarReset( Player* player )
//[b60014] chrissstrahl - reset variables - the ui is reset via ea.cfg in coop_playerEnterArena
{
	if ( !player )return;

	for ( int i = 0; i < COOP_RADAR_MAX_BLIPS; i++ ) {
		player->coopPlayer.radarBlipLastPosition[i] = Vector( 0.0f , -999.0f , -999.0f );
		player->coopPlayer.radarBlipActive[i] = false;
	}
	player->coopPlayer.radarSelectedActive = false;
}

float coop_radarAngleTo( const Vector& source , const Vector& dest )
{
	if ( source == dest ) {
		return 0;
	}
	float dot;
	dot = Vector::Dot( source , dest );
	return acosf( dot );
}

float coop_radarSignedAngleTo( const Vector& source , const Vector& dest , const Vector& planeNormal )
{
	float angle = coop_radarAngleTo( source , dest );
	Vector cross;
	cross = Vector::Cross( source , dest );
	float dot;
	dot = Vector::Dot( cross , planeNormal );

	return dot < 0 ? -angle : angle;
}

void coop_radarUpdateBlip(Player* player,
	Entity* eMiObjEntity,
	Entity* target,
	Vector& vRadarCenterPos,
	int& iMiObjEntityItemNumber,
	bool& targetedStillValid)
{
	float	fRealDistance;
	float	fRadarDistance;
	float	fRadarAngle;
	float	fBlipAxisX, fBlipAxisY;
	Vector	vRadarBlipLastPosition(0.0f, 0.0f, 0.0f);
	Vector	vRealDistance;
	Vector	vRadarDistance;
	Vector	vNorth(-1.0f, 0.0f, 0.0f);
	vRealDistance = (eMiObjEntity->origin - player->origin);
	vRealDistance.z = 0.0f;

	fRealDistance = vRealDistance.lengthSquared();
	fRadarDistance = fRealDistance / COOP_RADAR_SCALE_FACTOR;

	//make sure the blip does not go outside the actual radar
	if (fRadarDistance > COOP_RADAR_MAX_RADIUS) fRadarDistance = COOP_RADAR_MAX_RADIUS;

	//set vector to length 1, but keep it pointed into the actual direction (Einheitsvector (Länge = 1))
	vRadarDistance = vRealDistance;
	vRadarDistance.normalize();


	//make real pos of the object to a relative pos of the radar
	//Die reale position des objected wird zu realtiven einer position des radars
	vRadarDistance *= fRadarDistance;

	trace_t trace;
	player->GetViewTrace(trace, MASK_PROJECTILE, 1.0f);

	//[b607] chrissstrahl - fix radar not working when player is in cg_3rd_person
	Vector viewDir = Vector(0, 0, 0);
	Vector vorg = Vector(0, 0, 0);
	if (player->client->ps.pm_flags & PMF_CAMERA_VIEW) { // 3rd person automatic camera
		vorg = player->client->ps.camera_origin;
	}
	else if (!player->ugpPlayerInThirdPerson()) { // First person
		vorg = player->origin;
		vorg.z += player->client->ps.viewheight;
	}
	else { // Third person
		vorg = player->client->ps.camera_origin;
	}

	viewDir = trace.endpos - vorg; //[b607] chrissstrahl - use the correct player/camera origin now
	viewDir.z = 0.0f;

	vRealDistance.normalize();
	viewDir.normalize();
	Vector up(0.0f, 0.0f, 1.0f);

	fRadarAngle = coop_radarSignedAngleTo(viewDir, vRealDistance, up);

	//debug
	//gi.Printf( va( "PlayerPos(%f,%f)\n" , player->origin.x , player->origin.y ) );
	//gi.Printf( va( "fRadarAngle : %f\n" , fRadarAngle ) );

	vRadarDistance = Vector(0.0f, -fRadarDistance, 0.0f);

	fRadarAngle -= M_PI / 2;

	fBlipAxisX = vRadarCenterPos.x + ((sinf(fRadarAngle) * vRadarDistance.x) + (cosf(fRadarAngle) * vRadarDistance.y));
	fBlipAxisY = vRadarCenterPos.y + ((cosf(fRadarAngle) * vRadarDistance.x) - (sinf(fRadarAngle) * vRadarDistance.y));

	vRadarBlipLastPosition[0] = fBlipAxisX;
	vRadarBlipLastPosition[1] = fBlipAxisY;

	//hzm coop mod chrissstrahl - construct client command
	if (player->coopPlayer.radarBlipLastPosition[iMiObjEntityItemNumber] != vRadarBlipLastPosition) {
		player->coopPlayer.radarBlipLastPosition[iMiObjEntityItemNumber] = vRadarBlipLastPosition;

		//hzm coop mod chrissstrahl - convert floats to int, to reduce traffic
		//[b607] chrissstrahl - if there is a lot send to player, do not update blips
		//The blips are updated every time the player moves or turns, so chances are high
		//no one will ever notice this, and if they do then there are nettraffic issues anyway
		//used to be send via DelayedServerCommand 
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 120) { //64
			gi.SendServerCommand(player->entnum, va("stufftext \"globalwidgetcommand cr%i rect %i %i %i %i\"\n", iMiObjEntityItemNumber, (int)fBlipAxisX, (int)fBlipAxisY, COOP_RADAR_BLIP_SIZE, COOP_RADAR_BLIP_SIZE));
		}

		if (target) {
			if (target == eMiObjEntity) {
				targetedStillValid = true;
				//[b607] chrissstrahl - if there is a lot send to player, do not update blips
				//The blips are updated every time the player moves or turns, so chances are high
				//no one will ever notice this, and if they do then there are nettraffic issues anyway
				//used to be send via DelayedServerCommand 
				if (gi.GetNumFreeReliableServerCommands(player->entnum) > 120) { //64
					gi.SendServerCommand(player->entnum, va("stufftext \"globalwidgetcommand crs rect %i %i %i %i\"\n", (int)fBlipAxisX, (int)fBlipAxisY, COOP_RADAR_BLIP_SIZE, COOP_RADAR_BLIP_SIZE));
				}
			}
		}
	}

	//hzm coop mod chrissstrahl - enable active blip if it is not already
	if (player->coopPlayer.radarBlipActive[iMiObjEntityItemNumber] == false) {
		player->coopPlayer.radarBlipActive[iMiObjEntityItemNumber] = true;
		//we need it to be reliable
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand cr%i enable", iMiObjEntityItemNumber));
	}
}

void coop_radarUpdate( Player *player )
//[b60014] chrissstrahl - check for mission objective entities update radar
//- update the radar every server frame (1 / sv_fps 20)
//- CALCULATION CODE BY ALBERT DORN (dorn.albert)
{
	//Exit on NULL
	if (!player) { return; }

	//Exit on NULL, bot and mod not installed
	gentity_t *ent = player->edict;
	if (!player || ent->svflags & SVF_BOT || player->coop_getInstalled() != 1) { return; }

	//player dead or in spectator, disable selected blip
	Entity* target = player->GetTargetedEntity();
	if (player->health <= 0.0f || multiplayerManager.isPlayerSpectator(player) || !target)
	{
		if (player->coopPlayer.radarSelectedActive) {
			player->coopPlayer.radarSelectedActive = false;
			upgPlayerDelayedServerCommand(player->entnum, "globalwidgetcommand crs disable");
		}
	}

	//don't update radar:
	//- on mission failure
	//- player just entered server
	//- player just entered arena/team
	//- to soon after last update
	//- for spectators
	//- for dead
	//- when doing tricorder puzzle
	ScriptVariable* solvingPuzzle = NULL;
	solvingPuzzle = player->entityVars.GetVariable("_playerIsSolvingPuzzle");
	if( level.mission_failed ||
		player->health <= 0.0f ||
		multiplayerManager.isPlayerSpectator(player) ||
		(player->upgPlayerGetLevelTimeEntered() + 3) > level.time ||
		(player->coop_getSpawnedLastTime() + 1.5) > level.time ||
		player->coopPlayer.lastTimeRadarUpdated + COOP_RADAR_TIMECYCLE > level.time ||
		solvingPuzzle && solvingPuzzle->floatValue() == 1.0f )
	{
		return;
	}

	//keep track of last update time
	player->coopPlayer.lastTimeRadarUpdated = level.time;

	//radar COOP_RADAR_CIRCLE_START at: 09 09 of hud
	//radar COOP_RADAR_BLIP_SIZE /2 = precise blip pos
	//radar center pos = 55 55
	//centerpos (55 55) plus radar circle start (55 + 9 - 55 + 9 = 64 64)
	//blip correction = (64 64 - -12 -12 = 52 52 )
	//radar centerpos = 52 52
	//calculate offset dynamicly
	///////////////////////////////////////////////////////////////////
	Vector	vRadarCenterPos( 55.0f , 55.0f , 0.0f );
	vRadarCenterPos[0] = ( ( vRadarCenterPos[0] + COOP_RADAR_CIRCLE_START ) - ( COOP_RADAR_BLIP_SIZE / 2 ) );
	vRadarCenterPos[1] = ( ( vRadarCenterPos[1] + COOP_RADAR_CIRCLE_START ) - ( COOP_RADAR_BLIP_SIZE / 2 ) );

	Entity	*eMiObjEntity = NULL;
	int		iMiObjEntityItemNumber = 0;
	bool	targetedStillValid = false;

	//check routine is needed to detect what data needs to be send and not be send
	for (int i = 0; i < maxentities->integer; i++ ){
		eMiObjEntity = g_entities[i].entity;

		//not meant to show on radar
		if ( !eMiObjEntity || eMiObjEntity->edict->s.missionObjective != 1 || eMiObjEntity->isSubclassOf(Player)){
			continue;
		}

		//overreach
		if (iMiObjEntityItemNumber >= COOP_RADAR_MAX_OBJCTIVE_BLIPS) {
			break;
		}

		coop_radarUpdateBlip(player, eMiObjEntity, target, vRadarCenterPos, iMiObjEntityItemNumber, targetedStillValid); 

		//hzm coop mod chrissstrahl - keep track of the current Mission Objective Blip Marker
		iMiObjEntityItemNumber++;
	}

	//hzm coop mod chrissstrahl - update radar background compas disc, update it first that way the radar seams more responsive
	int iRadarAngle = ( int )player->client->ps.viewangles[1];
	if ( iRadarAngle < 0 ){ iRadarAngle = ( 359 + iRadarAngle ); }

	if ( iRadarAngle != player->coopPlayer.lastRadarAngle ){
		player->coopPlayer.lastRadarAngle = iRadarAngle;
		//[b607] chrissstrahl - if there is a lot send to player, do not update disc
		//The disc is updated every time the player turns, so chances are high
		//no one will ever notice this, and if they do then there are nettraffic issues anyway
		//used to be send via DelayedServerCommand 
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 64 ){
			//[b60014] chrissstrahl - this needs more network traffic, but reduces cfg saves for client massively, this can help some clients with performance
			//gi.SendServerCommand(player->entnum,va("stufftext \"set coop_r %i\"\n",player->coopPlayer.lastRadarAngle));
			int angle = (270 - player->coopPlayer.lastRadarAngle);
			gi.SendServerCommand(player->entnum,va("stufftext \"globalwidgetcommand radarBg rendermodelpitch %i\"\n", angle));
		}
	}

	//[b60014] chrissstrahl - disable all other inactive blips
	for (int i = iMiObjEntityItemNumber; i < COOP_RADAR_MAX_OBJCTIVE_BLIPS; i++ ){
		if ( player->coopPlayer.radarBlipActive[i] == true ){
			player->coopPlayer.radarBlipActive[i] = false;
			//we need it to be reliable
			upgPlayerDelayedServerCommand( player->entnum , va( "globalwidgetcommand cr%i disable" , i ) );
		}
	}

	//[b60014] chrissstrahl - show green player blip
	bool bGreenBlipActive = false;
	iMiObjEntityItemNumber = COOP_RADAR_MAX_OBJCTIVE_BLIPS;
	for (int i = 0; i < maxclients->integer; i++) {
		gentity_t* gentity = &g_entities[i];
		
		if (gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf(Player)) {
			Player* currentPlayer = (Player*)gentity->entity;
			if (currentPlayer && currentPlayer->edict->s.missionObjective == 1) {
				bGreenBlipActive = true;
				coop_radarUpdateBlip(player, currentPlayer, target, vRadarCenterPos, iMiObjEntityItemNumber, targetedStillValid);
				break;
			}
		}
	}
	//[b60014] chrissstrahl - deactivate green blip
	if (!bGreenBlipActive) {
		for (int i = 0; i < maxclients->integer; i++) {
			gentity_t* gentity = &g_entities[i];
			if (gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf(Player)) {
				Player* currentPlayer = (Player*)gentity->entity;
				if (currentPlayer->coopPlayer.radarBlipActive[(COOP_RADAR_MAX_BLIPS - 1)]) {
					currentPlayer->coopPlayer.radarBlipActive[(COOP_RADAR_MAX_BLIPS - 1)] = false;
					upgPlayerDelayedServerCommand(gentity->entity->entnum, va("globalwidgetcommand cr%i disable", (COOP_RADAR_MAX_BLIPS - 1)));
				}
			}
		}
	}


	//update (enable/disable) selected marker
	//bugfix - chrissstrahl - make sure it is deactivated if not needed
	if (targetedStillValid) {
		if (!player->coopPlayer.radarSelectedActive) {
			player->coopPlayer.radarSelectedActive = true;
			//[b607] chrissstrahl - made seperate, because we need it to be reliable
			upgPlayerDelayedServerCommand(player->entnum, "globalwidgetcommand crs enable");
		}
	}
}
