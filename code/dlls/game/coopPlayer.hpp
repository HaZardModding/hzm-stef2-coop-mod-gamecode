//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#ifndef __COOPPLAYER_HPP__
#define __COOPPLAYER_HPP__

#include "_pch_cpp.h"
#include "equipment.h"
#include "coopClass.hpp"

#define COOP_RADAR_MAX_BLIPS 9
#define COOP_RADAR_MAX_OBJCTIVE_BLIPS (COOP_RADAR_MAX_BLIPS - 1)
//[b607] chrissstrahl - changed to autoadjust
#define COOP_RADAR_CIRCLE_START (COOP_RADAR_MAX_BLIPS + 1)
#define COOP_RADAR_BLIP_SIZE 28
#define COOP_RADAR_MAX_RADIUS 55
#define COOP_RADAR_TIMECYCLE 0.05f //used to be 0.1
#define COOP_RADAR_SCALE_FACTOR 25000 //used to be 40000

#define COOP_MAX_ARMOR 200
#define COOP_MAX_HW_AMMO_FED 500
#define COOP_MAX_HW_AMMO_PLASMA 500
#define COOP_MAX_HW_AMMO_IDRYLL 500

typedef struct pendingServerCommand_s
{
	char					*command;
	pendingServerCommand_s	*next;
} pendingServerCommand;

class CoopPlayer
{
public:
	//hzm coop mod chrissstrahl - used to count how many tries happened to check player for coop mod
	int		setupTries = 0;
	//hzm coop mod chrissstrahl - used to store chat message limit data
	float	chatTimeLimit = 0.0f;
	//hzm coop mod chrissstrahl - used to send the exec command to player within a certain timeframe
	float	installedCheckTime = 0.0f;
	//hzm coop mod chrissstrahl - used to determin which version the player has of the coop mod
	int		installedVersion = 0;
	//hzm coop mod chrissstrahl - used to determin if player has coop mod installed or not
	int		installed = 0;
	//hzm coop mod chrissstrahl - used to determin what class the player does have
	str		className = "";
	//hzm coop mod chrissstrahl - used to store player Anonymous idendity! Don't want admins have to much power, seen it far to often to go wrong
	str		coopId = "";
	//hzm coop mod chrissstrahl - inventory and health status to compare if the ini file should be updated
	str		coopStatus = "";
	//hzm coop mod chrissstrahl - used to determin if injured symbol is visbile on player
	bool	injuredSymbolVisible = false;
	//hzm coop mod chrissstrahl - used to determin if player spawned or respawned
	bool	respawned = false;
	//hzm coop mod chrissstrahl - used to determin if player should respawn at where he is or at a predefinied spawn location
	bool	respawnAtRespawnpoint = false;
	//hzm coop mod chrissstrahl - used to saveoff the viewangle when the player dies, to restore it when the player respawns at the same location he died
	int		deathViewangleY = 0;
	//hzm coop mod chrissstrahl - used to save if player setup is complete
	bool	setupComplete = false;
	//hzm coop mod chrissstrahl - used to store if a medic was notified to heal this now critically injured player
	str		lastTargetedClassSend = "";

	//hzm coop mod chrissstrahl - used to store previouse health status//float	previouseHealth;

	//hzm coop mod chrissstrahl - used to signal the game that this player is currently neutralized
	bool	neutralized = false;

	//hzm coop mod chrissstrahl - keep track of the objective blips from the radar - so that it will only be resend if nessary
	bool	radarBlipActive[COOP_RADAR_MAX_BLIPS];
	bool	radarSelectedActive = false;
	Vector	radarBlipLastPosition[COOP_RADAR_MAX_BLIPS];
	//bool	radarFirstResetDone = false
	//bool	radarBlipSetInactive[COOP_RADAR_MAX_BLIPS];
	

	//hzm coop mod chrissstrahl - used to store last player mass value, needed to restore it after player has been revived
	int		lastMass = 0;
	//hzm coop mod chrissstrahl - used to count the revive process
	int		reviveCounter = 0;

	//hzm coop mod chrissstrahl - remember the EXACT location the player died
	//player might be moved by physics or WHAT EVA , which will result in the
	//player respawning inside a wall or in a bad spot
	Vector	lastAliveLocation = Vector(0,0,0);
	//hzm coop mod chrissstrahl - used to saveoff info when the player was last notified about spamming
	float	lastTimeSpamInfo = -999.0f;
	//hzm coop mod chrissstrahl - used to determin when player spawned last
	float	lastTimeSpawned = -998.0f;
	//hzm coop mod chrissstrahl - used to saveoff the player his last think time
	float	lastTimeThink = -997.0f;
	//hzm coop mod chrissstrahl - used to saveoff the player his last radar update time
	float	lastTimeRadarUpdated = 0.0f;
	//hzm coop mod chrissstrahl - used to saveoff the player his last transmitted radar angle
	int		lastRadarAngle = 0;
	//hzm coop mod chrissstrahl - used to saveoff the player his last class change time
	float	lastTimeAppliedClass = -996.0f;
	//hzm coop mod chrissstrahl - used to saveoff the player his last class info update time
	float	lastTimeUpdatedClassStat = -995.0f;
	//hzm coop mod chrissstrahl - used to save when the player has recived the last time the mission objectives
	float	lastTimeUpdatedObjectives = -1.0f;
	int		objectivesCycle = -994;
	//[b60011] chrissstrahl - when the Missionobjectives Title text was last printed
	float	lastTimePrintedObjectivesTitle = -1.0f;
	//hzm coop mod chrissstrahl - used to save when the player was used the last time by another player
	float	lastTimeUsedClassMsg = 0.0f;
	//hzm coop mod chrissstrahl - used to determin when player was injured last time
	float	lastTimeInjured = -993.0f;
	//hzm coop mod chrissstrahl - used to store the time of last class change
	float	lastTimeChangedClass = -992.0f;
	//hzm coop mod chrissstrahl - used to store gametime this player was neutralized at
	float	lastTimeNeutralized = 0.0f;
	//hzm coop mod chrissstrahl - used to store gametime this player was last time revived
	float	lastTimeRevived = 0.0f;
	//hzm coop mod chrissstrahl - used to store gametime this player was last time using other payer via use or tricorder
	float	lastTimeUsing = 0.0f;
	//hzm coop mod chrissstrahl - used to store gametime this player was last time modulating a puzzleobject
	float	lastTimeModulatingPuzzle = 0.0f;
	//hzm coop mod chrissstrahl - keeps track of last targeted entity num
	//int		lastTargetedEntity; //[b607] removed, there is last_entityTargeted in player class
	//hzm coop mod chrissstrahl - show targeted entity targetname and class dev command (!targeted)
	bool	showTargetedEntity = false;
	//hzm coop mod chrissstrahl - keeps track of last targeted entity its class
	str		lastTargetedEntityClass = "";
	//hzm coop mod chrissstrahl - keeps track of when the player last used !transport
	float	lastTimeTransported = -991.0f;
	//hzm coop mod chrissstrahl - remember when player pressed escape during this cinematic
	float	lastTimeSkipCinematic = -990.0f;
	//hzm coop mod chrissstrahl - remember when player caused the last time a message to the hud, prevent him from spamming, used for events like rejoining when dead in lms and so forth
	float	lastTimeHudMessage = -989.0f;
	//hzm coop mod chrissstrahl - keeps track of when the player entered the game
	float	timeEntered = -988.0f;
	//hzm coop mod chrissstrahl - marks if player weapons should be unholstered after !transport
	bool	transportUnholster = false;
	//hzm coop mod chrissstrahl - remember weapon that should be unholstered after !transport
	str		transportUnholsterWeaponName = "";
	//hzm coop mod chrissstrahl - remember if update menu has been shown or not
	bool	updateHudDisplayed = false;
	//hzm coop mod chrissstrahl - needed for additional vote options
	bool startedVote = false;
	//hzm coop mod chrissstrahl - needed for tricorder scanning/archetypes in mp
	bool scanning = false;
	//hzm coop mod chrissstrahl - used to keep track of tricorder scan info hud status
	bool scanHudActive = false;
	//hzm coop mod chrissstrahl - used to store tricorder scan last data send time
	float lastScanSend = -987.0f;
	//hzm coop mod chrissstrahl - used to store real time when player last died
	int deathTime = -986;
	//[b607] chrissstrahl - used to store last level.time this player died
	float diedLast = -985.0f;
	//[b60011] chrissstrahl - used to store deaths this player died during lms
	int lmsDeaths = 0;
	//hzm coop mod chrissstrahl - used to store data send 
	str lastScanSendData = "";
	//[b607] chrissstrahl - used to store last if a certain hud is active
	bool clickFireHudActive = false;
	//[b60011][b607] chrissstrahl - used to manage transmitting of scanning data to coop tricorder hud
	str scanData0 = ""; //[b60011]
	str scanData1 = "";
	str scanData2 = "";
	//[b607] chrissstrahl - addad coop admin check var
	bool admin = false;
	//[b60011] chrissstrahl - used to allow placing of objects - this will be the temp model used to visualize where a object will be placed
	EntityPtr ePlacable = NULL;
	//[b60011] chrissstrahl - used to keep track of the class ability of a player to place a class specific station
	EntityPtr eClassPlacable = NULL;
	//[b60011] chrissstrahl - used for coopID detection setup timeout in coopPlayerThink()
	int setupTriesCid = 0;
	//[b60011] chrissstrahl - used to check if player has a coopid, within a certain timeframe
	float setupTriesCidCheckTime = 0.0f;
	//[b60011] chrissstrahl - cooldown for taunts
	float tauntCooldownTime = 0.0f;
	//[b60011] chrissstrahl - cooldown for !block
	float cmdBlockCooldownTime = 0.0f;
	//[b60011] chrissstrahl - collision
	float callange_LastCollisionTime = 0.0f;
	//[b60011] chrissstrahl - disable ability to change class
	bool classChangingDisabled = false;
	//[b60011] chrissstrahl - player is already handled by CoopNpcTeam
	bool coopNpcTeamHandled = false;
};


extern pendingServerCommand *pendingServerCommandList[MAX_CLIENTS];
//[b60012] chrissstrahl - try to fix tiki model anim cache overload issue
void coop_playerFlushTikis();
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

void coop_playerGenerateNewPlayerId(Player *player);	//[b60011] chrissstrahl - creating a new player id
bool coop_playerMakeSolidASAPThink(Player* player);		//[b60011] chrissstrahl - making player solid as soon as possible 
void coop_playerPlaceableThink(Player* player);			//[b60011] chrissstrahl - does thinking for placeable item
void coop_playerSetupHost(Player* player);				//[b60011] chrissstrahl - does setup for host player (only on listen win server/singleplayer)
void coop_playerSetupClient(Player* player);			//[b60011] chrissstrahl - does setup for non-host player (client)

void coop_playerSaveNewPlayerId(Player *player);
void coop_playerSetupNoncoop( Player *player );
void coop_playerSetupCoop( Player *player );
bool coop_playerSay( Player *player , str sayString );
bool coop_playerKilled( const Player *killedPlayer , const Entity * attacker , const Entity *inflictor , const int meansOfDeath );
void coop_playerModelChanged( Player *player );
void coop_playerUsed( Player *usedPlayer , Player *usingPlayer , Equipment *equipment );
void coop_playerDamaged( Player *damagedPlayer , Player *attackingPlayer , float damage , int meansOfDeath );
void coop_playerEnterArena( int entnum , float health );
void coop_playerSpectator( Player *player );
void coop_playerThink( Player *player );
void coop_playerLeft( Player *player );

void DelayedServerCommand( int entNum, const char *commandText );
void handleDelayedServerCommands( void );
void clearDelayedServerCommands( int entNum );


#endif /* coopPlayer.hpp */


