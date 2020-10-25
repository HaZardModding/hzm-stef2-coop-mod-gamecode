//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER HANDLING RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]


#ifndef __COOPPLAYER_HPP__
#define __COOPPLAYER_HPP__
#include "_pch_cpp.h"
#include "equipment.h"
#define COOP_RADAR_MAX_BLIPS 8
//[b607] chrissstrahl - changed to autoadjust
#define COOP_RADAR_CIRCLE_START (COOP_RADAR_MAX_BLIPS + 1)
#define COOP_RADAR_BLIP_SIZE 28
#define COOP_RADAR_MAX_RADIUS 55
//used to be 40000
#define COOP_RADAR_SCALE_FACTOR 25000 

#define COOP_CLASS_HURT_WAITTIME 6
#define COOP_CLASS_REGENERATE_HEALTH 3
#define COOP_CLASS_REGENERATE_ARMOR 5
#define COOP_CLASS_REGENERATE_AMMO 3

#define COOP_MAX_ARMOR 200
#define COOP_MAX_HW_AMMO_FED 500
#define COOP_MAX_HW_AMMO_PLASMA 500
#define COOP_MAX_HW_AMMO_IDRYLL 500

#define COOP_CLASS_MEDIC_MAX_HEALTH 110.0f
#define COOP_CLASS_TECHNICIAN_MAX_HEALTH 100.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_HEALTH 120.0f

#define COOP_CLASS_MEDIC_START_ARMOR 25.0f
#define COOP_CLASS_TECHNICIAN_START_ARMOR 100.0f
#define COOP_CLASS_HEAVYWEAPONS_START_ARMOR 50.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_PHASER 400.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_PHASER 400.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_PHASER 1000.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_FED 250.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_FED 300.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_FED 500.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_PLASMA 250.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_PLASMA 250.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_PLASMA 50.0f

#define COOP_CLASS_MEDIC_MAX_AMMO_IDRYLLUM 250.0f
#define COOP_CLASS_TECHNICIAN_MAX_AMMO_IDRYLLUM 300.0f
#define COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_IDRYLLUM 500.0f

#define COOP_CLASS_MEDIC_GRAVITY 0.85f
#define COOP_CLASS_TECHNICIAN_GRAVITY 0.9f
#define COOP_CLASS_HEAVYWEAPONS_GRAVITY 1.0f

#define COOP_CLASS_MEDIC_MASS 270
#define COOP_CLASS_TECHNICIAN_MASS 270
#define COOP_CLASS_HEAVYWEAPONS_MASS 350

#define COOP_REVIVE_TIME 100 //was 400
#define COOP_MAX_ARMOR_TO_GIVE 200
#define COOP_MAX_AMMO_TO_GIVE_PLASMA 800
#define COOP_MAX_AMMO_TO_GIVE_IDRYLL 800
#define COOP_MAX_AMMO_TO_GIVE_FED 800

typedef struct pendingServerCommand_s
{
	char					*command;
	pendingServerCommand_s	*next;
} pendingServerCommand;


class CoopPlayer
{
public:
	//hzm coop mod chrissstrahl - used to store chat message limit data
	float	chatTimeLimit;
	//hzm coop mod chrissstrahl - used to count how many tries the had to check player for coop mod
	int		setupTries;
	//hzm coop mod chrissstrahl - used to send the exec command to player within a certain timeframe
	float	installedCheckTime;
	//hzm coop mod chrissstrahl - used to determin which version the player has of the coop mod
	int		installedVersion;
	//hzm coop mod chrissstrahl - used to determin if player has coop mod installed or not
	int		installed;
	//hzm coop mod chrissstrahl - used to determin what class the player does have
	str		className;
	//hzm coop mod chrissstrahl - used to store player ANONYMOUSE idendity! Don't want admins have to much power, seen it far to often to go wrong
	str		coopId;
	//hzm coop mod chrissstrahl - inventory and health status to compare if the ini file should be updated
	str		coopStatus;
	//hzm coop mod chrissstrahl - used to determin if injured symbol is visbile on player
	bool	injuredSymbolVisible;
	//hzm coop mod chrissstrahl - used to determin if player spawned or respawned
	bool	respawned;
	//hzm coop mod chrissstrahl - used to determin if player should respawn at where he is or at a predefinied spawn location
	bool	respawnAtRespawnpoint;
	//hzm coop mod chrissstrahl - used to saveoff the viewangle when the player dies, to restore it when the player respawns at the same location he died
	int		deathViewangleY;
	//hzm coop mod chrissstrahl - used to saveoff the player his game language version
	str		language;
	//hzm coop mod chrissstrahl - used to save if player setup is complete
	bool	setupComplete;
	//hzm coop mod chrissstrahl - used to store if a medic was notified to heal this now critically injured player
	str		lastTargetedClassSend;

	//hzm coop mod chrissstrahl - used to store previouse health status//float	previouseHealth;

	//hzm coop mod chrissstrahl - used to signal the game that this player is currently neutralized
	bool	neutralized;

	//hzm coop mod chrissstrahl - keep track of the objective blips from the radar - so that it will only be resend if nessary
	bool	radarBlipActive[COOP_RADAR_MAX_BLIPS];
	bool	radarSelectedActive;
	bool	radarFirstResetDone; //[b607] chrissstrahl - used to reduce nettraffic on first spawn
	//bool	radarBlipSetInactive[COOP_RADAR_MAX_BLIPS];
	Vector	radarBlipLastPosition[COOP_RADAR_MAX_BLIPS];

	//hzm coop mod chrissstrahl - used to store last player mass value, needed to restore it after player has been revived
	int		lastMass;
	//hzm coop mod chrissstrahl - used to count the revive process
	int		reviveCounter;

	//hzm coop mod chrissstrahl - remember the EXACT location the player died
	//player might be moved by physics or WHAT EVA , which will result in the
	//player respawning inside a wall or in a bad spot
	Vector	lastAliveLocation;
	//hzm coop mod chrissstrahl - used to saveoff info when the player was last notified about spamming
	float	lastTimeSpamInfo;
	//hzm coop mod chrissstrahl - used to determin when player spawned last
	float	lastTimeSpawned;
	//hzm coop mod chrissstrahl - used to saveoff the player his last think time
	float	lastTimeThink;
	//hzm coop mod chrissstrahl - used to saveoff the player his last radar update time
	float	lastTimeRadarUpdated;
	//hzm coop mod chrissstrahl - used to saveoff the player his last transmitted radar angle
	int		lastRadarAngle;
	//hzm coop mod chrissstrahl - used to saveoff the player his last class change time
	float	lastTimeAppliedClass;
	//hzm coop mod chrissstrahl - used to saveoff the player his last class info update time
	float	lastTimeUpdatedClassStat;
	//hzm coop mod chrissstrahl - used to save when the player has recived the last time the mission objectives
	float	lastTimeUpdatedObjectives;
	//hzm coop mod chrissstrahl - used to save when the player was used the last time by another player
	float	lastTimeUsedClassMsg;
	//hzm coop mod chrissstrahl - used to determin when player was injured last time
	float	lastTimeInjured;
	//hzm coop mod chrissstrahl - used to store the time of last class change
	float	lastTimeChangedClass;
	//hzm coop mod chrissstrahl - used to store gametime this player was neutralized at
	float	lastTimeNeutralized;
	//hzm coop mod chrissstrahl - used to store gametime this player was last time revived
	float	lastTimeRevived;
	//hzm coop mod chrissstrahl - used to store gametime this player was last time using other payer via use or tricorder
	float	lastTimeUsing;
	//hzm coop mod chrissstrahl - used to store gametime this player was last time modulating a puzzleobject
	float	lastTimeModulatingPuzzle;
	//hzm coop mod chrissstrahl - keeps track of last targeted entity num
	//int		lastTargetedEntity; //[b607] removed, there is last_entityTargeted in player class
	//hzm coop mod chrissstrahl - show targeted entity targetname and class dev command (!targeted)
	bool	showTargetedEntity;
	//hzm coop mod chrissstrahl - keeps track of last targeted entity its class
	str		lastTargetedEntityClass;
	//hzm coop mod chrissstrahl - keeps track of when the player last used !transport
	float	lastTimeTransported;
	//hzm coop mod chrissstrahl - remember when player pressed escape during this cinematic
	float	lastTimeSkipCinematic;
	//hzm coop mod chrissstrahl - remember when player caused the last time a message to the hud, prevent him from spamming, used for events like rejoining when dead in lms and so forth
	float lastTimeHudMessage;
	//hzm coop mod chrissstrahl - keeps track of when the player entered the game
	float	timeEntered;
	//hzm coop mod chrissstrahl - marks if player weapons should be unholstered after !transport
	bool	transportUnholster;
	//hzm coop mod chrissstrahl - remember weapon that should be unholstered after !transport
	str		transportUnholsterWeaponName;
	//hzm coop mod chrissstrahl - remember if update menu has been shown or not
	bool	updateHudDisplayed;
	//hzm coop mod chrissstrahl - needed for additional vote options
	bool startedVote;
	//hzm coop mod chrissstrahl - needed for tricorder scanning/archetypes in mp
	bool scanning;
	//hzm coop mod chrissstrahl - used to keep track of tricorder scan info hud status
	bool scanHudActive;
	//hzm coop mod chrissstrahl - used to store tricorder scan last data send time
	float lastScanSend;
	//hzm coop mod chrissstrahl - used to store real time when player last died
	int deathTime;
	//hzm coop mod chrissstrahl - used to store data send 
	str lastScanSendData;
	//[b607] chrissstrahl - used to store last level.time this player died
	float diedLast;
	//[b607] chrissstrahl - used to store last if a certain hud is active
	bool clickFireHudActive;
	//[b607] chrissstrahl - used to manage transmitting of scanning data to coop tricorder hud
	str scanData1;
	str scanData2;
	//[b607] chrissstrahl - addad coop admin check var
	bool admin;
	//[b607] chrissstrahl - addad delayed equip routine to prevent issues we are having especially in singleplayer and on fast computers
	bool armoryNeedstoBeEquiped = false;
};


extern pendingServerCommand *pendingServerCommandList[MAX_CLIENTS];
//[b607] chrissstrahl - add/remove/update playername for communicator menu
void coop_playerCommunicator(Player* player, int iAdd);
//[b607] chrissstrahl - added for coop admin login
bool coop_playerCheckAdmin(Player *player);
//[b607] chrissstrahl - added to handle stuff when a player is connecting
void coop_playerConnect(Entity *ePlayer); 
bool coop_playerSpawnLms( Player *player );
str coop_playerGetDataSegment( Player *player , short int iNumber );
void coop_manageIntervalTransmit( Player* player , str sData , float fInterval , float &last );
void coop_playerRestore( Player *player );
bool coop_playerSetup( gentity_t *ent );
bool coop_playerSetup( Player *player );
void coop_playerSetupNoncoop( Player *player );
void coop_playerSetupCoop( Player *player );
bool coop_playerTransportToSpawn( Player *player );
bool coop_playerSpawnTryVariable( Player *player , bool bRespawning );
bool coop_playerSpawnTryIPD( Player *player , bool bRespawning );
bool coop_playerSpawnTrySpSpawn( Player *player , bool bRespawning );
bool coop_playerPlaceAtSpawn(Player *player);
bool coop_playerSay( Player *player , str sayString );
void coop_playerEnterArena( int entnum , float health );
bool coop_playerKilled( const Player *killedPlayer , const Entity * attacker , const Entity *inflictor , const int meansOfDeath );
void coop_playerModelChanged( Player *player );
void coop_playerUsed( Player *usedPlayer , Player *usingPlayer , Equipment *equipment );
void coop_playerDamaged( Player *damagedPlayer , Player *attackingPlayer , float damage , int meansOfDeath );
void coop_playerSpectator( Player *player );
void coop_playerThink( Player *player );
void coop_playerLeft( Player *player );


void DelayedServerCommand( int entNum, const char *commandText );
void handleDelayedServerCommands( void );
void clearDelayedServerCommands( int entNum );


#endif /* coopPlayer.hpp */


