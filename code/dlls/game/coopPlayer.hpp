//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#pragma once
#include "equipment.h"

#define COOP_RADAR_MAX_BLIPS 9
#define COOP_RADAR_MAX_OBJCTIVE_BLIPS (COOP_RADAR_MAX_BLIPS - 1)
#define COOP_RADAR_CIRCLE_START 10
#define COOP_RADAR_HUD_REAL_WIDTH 768
#define COOP_RADAR_HUD_SCALE_WIDTH 128
#define COOP_RADAR_BLIP_SIZE 30
#define COOP_RADAR_MAX_RADIUS 55
#define COOP_RADAR_TIMECYCLE 0.05f //used to be 0.1
#define COOP_RADAR_SCALE_FACTOR 25000 //used to be 40000

#define COOP_MAX_ARMOR 200
#define COOP_MAX_HW_AMMO_FED 500
#define COOP_MAX_HW_AMMO_PLASMA 500
#define COOP_MAX_HW_AMMO_IDRYLL 500


//--------------------------------------------------------------
// [b60025] GAMEFIX - Added: Information we want to persist over level changes and restarts - chrissstrahl
//--------------------------------------------------------------
struct gamefix_client_persistant_s
{
	/*
	bool			isBot = false;
	str				language = "Eng";
	bool			admin = false;
	str				commandsStalled = "";
	int				commands = 0;
	float			commandsLast = 0.0f;
	int				chats = 0;
	float			chatsLast = 0.0f;
	str				currentModel = "models/char/munro.tik";
	str				currentTeam = "none";
	*/


	//--------------------------------------------------------------
	// GAMEFIX - Fixed: Phaser shots and hits being count on a per bullet rather as per beam basis - chrissstrahl
	//--------------------------------------------------------------
	EntityPtr		heuristicsWeap = nullptr;
	long int		heuristicsShots = 0;
	bool			heuristicsHit = false;
};
extern gamefix_client_persistant_s gamefix_client_persistant_t[MAX_CLIENTS];

typedef struct pendingServerCommand_s
{
	char					*command;
	pendingServerCommand_s	*next;
} pendingServerCommand;

class CoopPlayer
{
	//[b60014] chrissstrahl - needed to get cleaner code, seperating coop/stock game better
	friend class		Player;
public:
	//hzm coop mod chrissstrahl - used to determin what class the player does have
	str		className = "";

	//hzm coop mod chrissstrahl - used to determin if injured symbol is visbile on player
	bool	injuredSymbolVisible = false;
	//hzm coop mod chrissstrahl - used to determin if player spawned or respawned
	bool	respawned = false;
	//hzm coop mod chrissstrahl - used to determin if player should respawn at where he is or at a predefinied spawn location
	bool	respawnAtRespawnpoint = false;
	//hzm coop mod chrissstrahl - used to saveoff the viewangle when the player dies, to restore it when the player respawns at the same location he died
	int		deathViewangleY = 0;


//[b60014] chrissstrahl - moved here to make sure it is not illegally accessed
private:
	//[b60017] chrissstrahl - how many cycles the player will regenerate after giving stuff to other player
	int		regenerationCycles = 0;

	//used to determin if player has coop mod installed and which version
	int		installed = 0;
	int		installedVersion = 0;
	float	installedCheckTime = 0.0f;
	//used to count how many tries happened to check player for coop mod
	int		setupTries = 0;
	//used to check if player has a coopid, within a certain timeframe
	float	setupTriesCidCheckTime = 0.0f;
	//used to save if player setup is complete
	bool	setupComplete = false;
	//used for coopID detection setup timeout in coopPlayerThink()
	int		setupTriesCid = 0;
	//[b60014] chrissstrahl - used for admin login
	bool	admin = false;
	str		adminAuthString = "";
	bool	adminAuthStarted = false;
	int		adminAuthStringLengthLast = 0;
	short	adminAuthAttempts = 0;
	//used to store player Anonymous idendity! Don't want admins have to much power, seen it far to often to go wrong
	str		coopId = "";
	//used to signal the game that this player is currently neutralized
	bool	neutralized = false;
	//used to store last level.time this player died
	float	diedLast = -985.0f;
	//inventory and health status to compare if the ini file should be updated
	str		coopStatus = "";
	//contains the current cyle the 
	int		objectivesCycle = -994;
	//[b60011] chrissstrahl - when the Missionobjectives Title text was last printed
	float	lastTimePrintedObjectivesTitle = -1.0f;
public:
	//hzm coop mod chrissstrahl - used to store if a medic was notified to heal this now critically injured player
	str		lastTargetedClassSend = "";

	//keep track of the objective blips from the radar - so that it will only be resend if nessary
	bool	radarBlipActive[COOP_RADAR_MAX_BLIPS];
	bool	radarSelectedActive = false;
	Vector	radarBlipLastPosition[COOP_RADAR_MAX_BLIPS];
	//hzm coop mod chrissstrahl - used to saveoff the player his last radar update time
	float	lastTimeRadarUpdated = 0.0f;
	//hzm coop mod chrissstrahl - used to saveoff the player his last transmitted radar angle
	int		lastRadarAngle = 0;

	//hzm coop mod chrissstrahl - used to store last player mass value, needed to restore it after player has been revived
	int		lastMass = 0;
	//hzm coop mod chrissstrahl - used to count the revive process
	int		reviveCounter = 0;

	//hzm coop mod chrissstrahl - remember the EXACT location the player died
	//player might be moved by physics or WHAT EVA , which will result in the
	//player respawning inside a wall or in a bad spot
	Vector	lastAliveLocation = Vector(0,0,0);
	//hzm coop mod chrissstrahl - used to determin when player spawned last
	float	lastTimeSpawned = -998.0f;
	//hzm coop mod chrissstrahl - used to saveoff the player his last think time
	float	lastTimeThink = -997.0f;

	//hzm coop mod chrissstrahl - used to saveoff the player his last class change time
	float	lastTimeAppliedClass = -996.0f;
	//hzm coop mod chrissstrahl - used to saveoff the player his last class info update time
	float	lastTimeUpdatedClassStat = -995.0f;
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
	//hzm coop mod chrissstrahl - show targeted entity targetname and class dev command (!targeted)
	bool	showTargetedEntity = false;
	//hzm coop mod chrissstrahl - keeps track of last targeted entity its class
	str		lastTargetedEntityClass = "";
	//hzm coop mod chrissstrahl - remember when player caused the last time a message to the hud, prevent him from spamming, used for events like rejoining when dead in lms and so forth
	float	lastTimeHudMessage = -989.0f;
	//hzm coop mod chrissstrahl - marks if player weapons should be unholstered after !transport
	bool	transportUnholster = false;
	//hzm coop mod chrissstrahl - remember weapon that should be unholstered after !transport
	str		transportUnholsterWeaponName = "";
	//hzm coop mod chrissstrahl - needed for additional vote options
	bool startedVote = false;
	//hzm coop mod chrissstrahl - used to store tricorder scan last data send time
	float lastScanSend = -987.0f;
	//[b60011] chrissstrahl - used to store deaths this player died during lms
	int lmsDeaths = 0;
	//[b607] chrissstrahl - used to store last if a certain hud is active
	bool clickFireHudActive = false;
	//[b60011] chrissstrahl - used to allow placing of objects - this will be the temp model used to visualize where a object will be placed
	EntityPtr ePlacable = NULL;
	//[b60011] chrissstrahl - used to keep track of the class ability of a player to place a class specific station
	EntityPtr eClassPlacable = NULL;
	//[b60011] chrissstrahl - collision
	float callange_LastCollisionTime = 0.0f;
	//[b60011] chrissstrahl - disable ability to change class
	bool classChangingDisabled = false;
	//[b60011] chrissstrahl - player is already handled by CoopNpcTeam
	bool coopNpcTeamHandled = false;

	//[b60021] chrissstrahl
	void coopPlayerBegin(gentity_t* ent);
	void coopPlayerDisconnecting( Player *player );
};

extern CoopPlayer coopPlayer;
extern Event EV_Player_coopMessageUpdateYourMod;
extern Event EV_Player_SetClassLocked;
extern Event EV_Player_SetClassLocked;
extern Event EV_Player_GetCoopClass;
extern Event EV_Player_IsTechnichian;
extern Event EV_Player_IsMedic;
extern Event EV_Player_IsHeavyWeapons;
extern Event EV_Player_getCoopVersion;
extern pendingServerCommand *pendingServerCommandList[MAX_CLIENTS];

//[b607] chrissstrahl - add/remove/update playername for communicator menu
void coop_playerCommunicator(Player* player, int iAdd);
//[b607] chrissstrahl - added to handle stuff when a player is connecting
void coop_playerConnect(bool isBot);
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
