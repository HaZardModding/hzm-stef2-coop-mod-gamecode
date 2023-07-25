//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "level.h"
#include "player.h"
#include "weapon.h"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include <qcommon/gameplaymanager.h>

#include "upgPlayer.hpp"
#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"
#include "upgStrings.hpp"
#include "upgCoopInterface.hpp"

//[b60011] chrissstrahl
#include "coopNpcTeam.hpp"
extern CoopNpcTeam coopNpcTeam;
extern CoopChallenges coopChallenges;
extern CoopServer coopServer;
extern Event EV_Player_coop_playerNpcCheckAutoTeam;

#include "coopSpawnlocation.hpp"
extern CoopSpawnlocation coopSpawnlocation;

#include "coopPlayer.hpp"
#include "coopParser.hpp"
#include "coopModel.hpp"
#include "coopRadar.hpp"
#include "coopCheck.hpp"
#include "coopText.hpp"
#include "coopStory.hpp"
#include "coopServer.hpp"
#include "coopClass.hpp"
#include "coopArmory.hpp"
#include "coopObjectives.hpp"
#include "coopHuds.hpp"
#include "coopReturn.hpp"

//[b60011] chrissstrahl - make avialable to use here
extern Event EV_SetOriginEveryFrame;
extern Event EV_World_AutoFailure;

extern int iTIKIS;
extern int iSKAS;
extern int iSPRITES;

//[b60014] chrissstrahl - used to autospawn npc teammates
//================================================================
// HAS EVENT DEFINED IN: CLASS_DECLARATION( Sentient , Player , "player" )
//================================================================
void Player::coop_playerNpcCheckAutoTeam(Event* ev)
{
	if (!multiplayerManager.inMultiplayer()) {
		return;
	}

	if (level.cinematic) {
		for (int i = 0; i < maxclients->integer; i++) {
			Player* currentPlayer;
			currentPlayer = multiplayerManager.getPlayer(i);
			if (currentPlayer) {
				currentPlayer->CancelEventsOfType(EV_Player_coop_playerNpcCheckAutoTeam);
			}
		}

		//Manage Auto NPC Team
		Event* newEvent = new Event(EV_Player_coop_playerNpcCheckAutoTeam);
		PostEvent(newEvent, 3.0f);
		return;
	}

	//check if we should add npc team mates
	coopNpcTeam.npcCheckAutoTeam();
}

//[b60014] chrissstrahl - extend the amount of time a player can be under water without drowning
//================================================================
// CALLED FROM: void Player::InitWorldEffects( void )
//================================================================
void Player::coop_playerInitWorldEffects()
{
	if (multiplayerManager.inMultiplayer() && game.coop_isActive) {
		air_finished = level.time + 20.0f;
	}
}

//[b60011] chrissstrahl - get coop class name - used for scripting
//================================================================
// HAS EVENT DEFINED IN: CLASS_DECLARATION( Sentient , Player , "player" )
//================================================================
void Player::coop_playerGetCoopClass(Event* ev)
{
	//[b60014] chrissstrahl - accsess coopPlayer.className only in multiplayer
	if (multiplayerManager.inMultiplayer() && game.coop_isActive) {
		ev->ReturnString(this->coopPlayer.className);
	}
	else {
		ev->ReturnString("");
	}
}

//[b60011] chrissstrahl - check if coop class is technician
//================================================================
// HAS EVENT DEFINED IN: CLASS_DECLARATION( Sentient , Player , "player" )
//================================================================
void Player::coop_playerIsCoopClassTechnician(Event* ev)
{
	//[b60014] chrissstrahl - accsess coopPlayer.className only in multiplayer)
	if (multiplayerManager.inMultiplayer() && game.coop_isActive && this->coopPlayer.className == COOP_CLASS_NAME_TECHNICIAN) {
		ev->ReturnFloat(1.0f);
	}
	else {
		ev->ReturnFloat(0.0f);
	}
}

//[b60011] chrissstrahl - check if coop class is Medic
//================================================================
// HAS EVENT DEFINED IN: CLASS_DECLARATION( Sentient , Player , "player" )
//================================================================
void Player::coop_playerIsCoopClassMedic(Event* ev)
{
	//[b60014] chrissstrahl - accsess coopPlayer.className only in multiplayer)
	if (multiplayerManager.inMultiplayer() && game.coop_isActive && this->coopPlayer.className == COOP_CLASS_NAME_MEDIC) {
		ev->ReturnFloat(1.0f);
	}
	else {
		ev->ReturnFloat(0.0f);
	}
}

//[b60011] chrissstrahl - check if coop class is HeavyWeapons
//================================================================
// HAS EVENT DEFINED IN: CLASS_DECLARATION( Sentient , Player , "player" )
//================================================================
void Player::coop_playerIsCoopClassHeavyWeapons(Event* ev)
{
	//[b60014] chrissstrahl - accsess coopPlayer.className only in multiplayer)
	if (multiplayerManager.inMultiplayer() && game.coop_isActive && this->coopPlayer.className == COOP_CLASS_NAME_HEAVYWEAPONS) {
		ev->ReturnFloat(1.0f);
		return;
	}
	else {
		ev->ReturnFloat(0.0f);
	}
}

//[b60011] chrissstrahl - set coop class restriction - allowing/preventing player from switching class
//================================================================
// HAS EVENT DEFINED IN: CLASS_DECLARATION( Sentient , Player , "player" )
//================================================================
void Player::coop_playerSetClassLocked(Event* ev)
{
	//[b60014] chrissstrahl - accsess coopPlayer.classChangingDisabled only in multiplayer)
	if (multiplayerManager.inMultiplayer() && game.coop_isActive) {
		coopPlayer.classChangingDisabled = (bool)ev->GetInteger(1);
	}
}

//================================================================
// HAS EVENT DEFINED IN: CLASS_DECLARATION( Sentient , Player , "player" )
//================================================================
void Player::coop_playerGetCoopVersion(Event* ev)
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	ev->ReturnFloat(coop_getInstalledVersion());
}

//================================================================
// Name:        coop_playerCheckAdmin
// Class:       Player
//              
// Description: [b607] added login authorisation stuff for coop admin login menu
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Player::coop_playerCheckAdmin()
{
	//already logged in
	if (coop_playerAdmin()) {
		return true;
	}

	//[b610] chrissstrahl - auto login if player is host
	if (upgPlayerIsHost()) {
		coop_playerAdmin(true);
		hudPrint("^3You are now logged in (Host auto-!login).\n");
		return true;
	}

	//[b60014] chrissstrahl
	return false;
}

//========================================================[b60014]
// Name:        coop_playerObjectivesCycleEqual
// Class:       Player
//              
// Description:  check if player is up to date on the latest objectives
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Player::coop_playerObjectivesCycleEqual()
{
	return (coopPlayer.objectivesCycle == game.coop_objectiveCycle) ? true : false;
}

//========================================================[b60014]
// Name:        coop_playerObjectivesCycleUpdate
// Class:       Player
//              
// Description:  sets player objectives as up to date
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerObjectivesCycleUpdate()
{
	coopPlayer.objectivesCycle = game.coop_objectiveCycle;
}

//========================================================[b60014]
// Name:        coop_playerStatus
// Class:       Player
//              
// Description:  returns player status used for restoring upon reconnect
//              
// Parameters:  void
//              
// Returns:     str
//              
//================================================================
str Player::coop_playerCoopStatus()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return "";
	}
	return coopPlayer.coopStatus;
}

//========================================================[b60014]
// Name:        coop_playerStatus
// Class:       Player
//              
// Description:  sets player status used for restoring upon reconnect
//              
// Parameters:  str
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerCoopStatus(str sStatus)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.coopStatus Access VIOLATION!\n");
		return;
	}
	coopPlayer.coopStatus = sStatus;
}

//========================================================[b60014]
// Name:        coop_playerDiedLast
// Class:       Player
//              
// Description:  returns level.time when player died last
//              
// Parameters:  void
//              
// Returns:     float
//              
//================================================================
float Player::coop_playerDiedLast()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return -1.0f;
	}
	return coopPlayer.diedLast;
}

//========================================================[b60014]
// Name:        coop_playerDiedLastUpdate
// Class:       Player
//              
// Description:  sets level.time player died last
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerDiedLastUpdate()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.diedLast Access VIOLATION!\n");
		return;
	}
	coopPlayer.diedLast = level.time;
}

//========================================================[b60014]
// Name:        coop_playerNeutralized
// Class:       Player
//              
// Description:  returns if player is currently neutralized or not
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Player::coop_playerNeutralized()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return false;
	}
	return coopPlayer.neutralized;
}

//========================================================[b60014]
// Name:        coop_playerNeutralized
// Class:       Player
//              
// Description:  sets if player is currently neutralized or not
//              
// Parameters:  bool
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerNeutralized(bool bNeutralized)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.neutralized Access VIOLATION!\n");
		return;
	}
	coopPlayer.neutralized = bNeutralized;
}

//========================================================[b60014]
// Name:        coop_playerAdminAuthAttempts
// Class:       Player
//              
// Description:  returns failed attempts of player using !login
//              
// Parameters:  void
//              
// Returns:     int
//              
//================================================================
int Player::coop_playerAdminAuthAttempts()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return 9999;
	}
	return coopPlayer.adminAuthAttempts;
}

//========================================================[b60014]
// Name:        coop_playerAdminAuthAttemptsUpdate
// Class:       Player
//              
// Description:  counts up failed attempts of player trying to use !login
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerAdminAuthAttemptsUpdate()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.adminAuthAttempts Access VIOLATION!\n");
		return;
	}
	coopPlayer.adminAuthAttempts++;
}

//========================================================[b60014]
// Name:        coop_playerAdminAuthAttemptsUpdateReset
// Class:       Player
//              
// Description:  reset failed attempts of player trying to use !login
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerAdminAuthAttemptsReset()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.adminAuthAttempts Access VIOLATION!\n");
		return;
	}
	coopPlayer.adminAuthAttempts = 0;
}

//========================================================[b60014]
// Name:        coop_playerAdminAuthStringLastLength
// Class:       Player
//              
// Description:  return player if current and last auth strings are in the same length
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Player::coop_playerAdminAuthStringChanged()
{
	if (multiplayerManager.inMultiplayer()) {
		if (coopPlayer.adminAuthStringLengthLast != coopPlayer.adminAuthString.length()) {
			return true;
		}
	}
	return false;
}

//========================================================[b60014]
// Name:        coop_playerAdminAuthStringLastLengthUpdate
// Class:       Player
//              
// Description:  sets player admin auth string last to current auth string
//              
// Parameters:  str
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerAdminAuthStringLastLengthUpdate()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.adminAuthStringLengthLast Access VIOLATION!\n");
		return;
	}
	coopPlayer.adminAuthStringLengthLast = coopPlayer.adminAuthString.length();
}

//========================================================[b60014]
// Name:        coop_playerAdminAuthStarted
// Class:       Player
//              
// Description:  return player admin auth string
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Player::coop_playerAdminAuthStarted()
{
	if (multiplayerManager.inMultiplayer()) {
		return coopPlayer.adminAuthStarted;
	}
	return "";
}

//========================================================[b60014]
// Name:        coop_playerAdminAuthStarted
// Class:       -
//              
// Description:  sets player admin auth string
//              
// Parameters:  str
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerAdminAuthStarted(str sAuth)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.adminAuthStarted Access VIOLATION!\n");
		return;
	}
	coopPlayer.adminAuthStarted = sAuth;
}

//========================================================[b60014]
// Name:        coop_playerAdminAuthString
// Class:       Player
//              
// Description:  return player admin auth string
//              
// Parameters:  void
//              
// Returns:     str
//              
//================================================================
str Player::coop_playerAdminAuthString()
{
	if (multiplayerManager.inMultiplayer()) {
		return coopPlayer.adminAuthString;
	}
	return "";
}

//========================================================[b60014]
// Name:        coop_playerAdminAuthString
// Class:       Player
//              
// Description:  sets player admin auth string
//              
// Parameters:  str
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerAdminAuthString(str sAuth)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.adminAuthString Access VIOLATION!\n");
		return;
	}
	coopPlayer.adminAuthString = sAuth;
}

//========================================================[b60014]
// Name:        coop_playerAdmin
// Class:       Player
//              
// Description:  return player admin var
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Player::coop_playerAdmin()
{
	if (multiplayerManager.inMultiplayer()) {
		return coopPlayer.admin;
	}
	return false;
}

//========================================================[b60014]
// Name:        coop_playerAdmin
// Class:       Player
//              
// Description:  sets player admin var
//              
// Parameters:  bool
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerAdmin(bool bAdmin)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.admin Access VIOLATION!\n");
		return;
	}
	coopPlayer.admin = bAdmin;
}

//========================================================[b60014]
// Name:        coop_playerSetupComplete
// Class:       Player
//              
// Description:  Returns number of tried for coop player setup
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
int Player::coop_playerSetupTriesCidTime()
{
	if (multiplayerManager.inMultiplayer()) {
		return coopPlayer.setupTriesCidCheckTime;
	}
	return 0;
}

//========================================================[b60014]
// Name:        coop_playerSetupTriesCidIncremment
// Class:       Player
//              
// Description:  Updates time to when next check for cid
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerSetupTriesCidTimeUpdate()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.setupTriesCidCheckTime Access VIOLATION!\n");
		return;
	}
	coopPlayer.setupTriesCidCheckTime = (level.time + 0.15f);
}

//========================================================[b60014]
// Name:        coop_playerSetupTriesCid
// Class:       Player
//              
// Description:  Returns number of tried for coop player setup
//              
// Parameters:  void
//              
// Returns:     int
//              
//================================================================
int Player::coop_playerSetupTriesCid()
{
	if (multiplayerManager.inMultiplayer()) {
		return coopPlayer.setupTriesCid;
	}
	return 9999;
}

//========================================================[b60014]
// Name:        coop_playerSetupTriesCidIncremment
// Class:       Player
//              
// Description:  Sets Coop setup done for player
//              
// Parameters:  bool
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerSetupTriesCidIncremment()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.setupTriesCid Access VIOLATION!\n");
		return;
	}
	coopPlayer.setupTriesCid++;
}

//========================================================[b60014]
// Name:        coop_playerSetupTries
// Class:       Player
//              
// Description:  Returns number of tries for coop player setup
//              
// Parameters:  void
//              
// Returns:     int
//              
//================================================================
int Player::coop_playerSetupTries()
{
	if (multiplayerManager.inMultiplayer()) {
		return coopPlayer.setupTries;
	}
	return 9999;
}

//========================================================[b60014]
// Name:        coop_playerSetupTriesIncremment
// Class:       Player
//              
// Description:  Increments number of tries for setup - used for failsafe
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerSetupTriesIncremment()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.setupTries Access VIOLATION!\n");
		return;
	}
	coopPlayer.setupTries++;
}

//========================================================[b60014]
// Name:        coop_playerSetupComplete
// Class:       Player
//              
// Description:  Sets Coop setup complete status for player
//              
// Parameters:  bool
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerSetupComplete(bool bComplete)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.setupComplete Access VIOLATION!\n");
		return;
	}
	coopPlayer.setupComplete = bComplete;
}

//========================================================[b60014]
// Name:        coop_playerSetupComplete
// Class:       Player
//              
// Description:  checks if setup for player is done
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Player::coop_playerSetupComplete()
{
	if (multiplayerManager.inMultiplayer()) {
		return coopPlayer.setupComplete;
	}
	return true;
}

//========================================================[b60014]
// Name:        coop_playerScore
// Class:       Player
//              
// Description:  handles player score command in coop, removes score hud after few sec and shows press fire to spawn hud
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Player::coop_playerScore()
{
	if (!multiplayerManager.inMultiplayer() || !game.coop_isActive || health > 0 || (coop_playerDiedLast() + 10) > level.time) { return false; }
	
	if (!this->coopPlayer.clickFireHudActive) {
		this->coopPlayer.clickFireHudActive = true;
		gi.SendServerCommand(this->entnum, "stufftext \"-objectives_score\"\n");
		
		//Show Press Fire to Spawn Hud - but only if he really could (LMS!)
		if (this->coop_getInstalled() && coop_playerSpawnLms(this)) {
			this->addHud("coop_fireToSpawn");
		}
	}
	return true;
}

//================================================================
// Name:        coop_playerThinkDetectCoopId
// Class:       Player
//              
// Description:  checks if the given player does have a coop mod id
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerThinkDetectCoopId()
{
	constexpr auto COOP_MAX_ID_CHECK_TRIES = 15;

	//if player has coop or if there was a sufficent ammount of time passed
	if (!multiplayerManager.inMultiplayer() || coop_playerSetupTriesCid() >= COOP_MAX_ID_CHECK_TRIES || coop_getId().length()) {
		return;
	}

	//[b60014] chrissstrahl - don't handle bots
	if (upgPlayerIsBot()) {
		coop_setId("0");

		for (int i = coop_playerSetupTries(); i < (COOP_MAX_ID_CHECK_TRIES + 1); i++) {
			coop_playerSetupTriesCidIncremment();
		}
		return;
	}

	//have some time delay and also make sure the player is even able to process any commands
	if (coop_playerSetupTriesCidTime() < level.time) {
		coop_playerSetupTriesCidTimeUpdate();
		if (gi.GetNumFreeReliableServerCommands(entnum) > 96) { //[b60014] chrissstrahl - chnaged from 32 to 96
			coop_playerSetupTriesCidIncremment();
		}
	}
	//player does not have coop mod - give up at this point
	if (coop_playerSetupTriesCid() == COOP_MAX_ID_CHECK_TRIES) {
		coop_playerSetupTriesCidIncremment();
		coop_playerSaveNewPlayerId(this);
		return;
	}
}

//================================================================
// Name:        coop_playerThinkDetectCoop
// Class:       Player
//              
// Description:  checks if the given player does have the coop mod installed or not
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerThinkDetectCoop()
{
	constexpr auto COOP_MAX_MOD_CHECK_TRIES = 15;

	//if player has coop or if there was a sufficent ammount of time passed
	if (coop_getInstalled() || coop_playerSetupTries() >= COOP_MAX_MOD_CHECK_TRIES) {
		return;
	}

	//[b60014] chrissstrahl - don't handle bots
	if (upgPlayerIsBot()) {
		coop_setInstalled(false);
		coopPlayer.setupComplete = true;
		
		for (int i = coop_playerSetupTries(); i < (COOP_MAX_MOD_CHECK_TRIES + 1);i++) {
			coop_playerSetupTriesIncremment();
		}
		return;
	}

	//in multiplayer do the checking procedure
	//have some time delay and also make sure the player is even able to process any commands
	if (coop_getInstalledCheckTime() < level.time) {
		coop_setInstalledCheckTime(level.time + 0.25f);
		if (gi.GetNumFreeReliableServerCommands(entnum) > 96) { //[b60014] chrissstrahl - changed from 32 to 96
			coop_playerSetupTriesIncremment();
		}
	}

	//player does not have coop mod - give up at this point
	if (coop_playerSetupTries() == COOP_MAX_MOD_CHECK_TRIES) {
		coop_playerSetupNoncoop(this);
		coop_playerSetupTriesIncremment();
		return;
	}
}

//=========================================================[b60014]
// Name:        coop_playerThinkLogin
// Class:       Player
//              
// Description: Handles weapons load from script in singleplayer
//              we need to have this delayed because in sp the player is spawned way before the level or the script is ready
//
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_playerThinkLogin()
{
	if (coop_playerAdminAuthStarted() && !coop_playerAdmin()) {
		//exit here if there is no new input
		//using coopinput / G_coopInput to grab and construct input
		if (!coop_playerAdminAuthStringChanged()) {
			return;
		}
		coop_playerAdminAuthStringLastLengthUpdate();

		//the coop_admin cvar is empty, can't log in then
		str sCvar = "";
		cvar_t* cvar = gi.cvar_get("coop_admin");
		if (cvar) { sCvar = cvar->string; }
		if (sCvar.length() < 3) {
			upgPlayerDelayedServerCommand(entnum,va("globalwidgetcommand coop_comCmdLoginMsg labeltext %s\n", upgStrings.getReplacedForLabeltext("Error: 'coop_admin' is empty or shorter than 3 digits - Aborting").c_str()));
			return;
		}
		if (sCvar.length() > 10) {
			upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand coop_comCmdLoginMsg labeltext %s\n", upgStrings.getReplacedForLabeltext("Error: 'coop_admin' is longer than 10 digits - Aborting").c_str()));
			return;
		}

		//login succsessful
		if (coop_playerAdminAuthString() == sCvar) {
			coop_playerAdmin(true);
			coopPlayer.adminAuthAttempts = 0;
			coop_playerAdminAuthStarted(false);
			upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand coop_comCmdLoginMsg labeltext %s\n", upgStrings.getReplacedForLabeltext("Login succsessful - Accsess granted!\n").c_str()));
			//ePlayer.playsound( "sound/environment/computer/lcars_yes.wav" ,1);
			return;
		}

		//login failed
		if (coop_playerAdminAuthString().length() > 9) {
			coopPlayer.adminAuthAttempts++;
			coop_playerAdminAuthString("");
			upgPlayerDelayedServerCommand(entnum,va("globalwidgetcommand coop_comCmdLoginMsg labeltext %s\n", upgStrings.getReplacedForLabeltext("Login failed - Accsess denied!\n").c_str()));
			//gi.SendServerCommand(entnum,"stufftext \"playsound sound/environment/computer/access_denied.wav\"\n");

			if (coopPlayer.adminAuthAttempts > 5) {
				gi.SendConsoleCommand(va("kick %d\n",entnum));
			}
			return;
		}
	}
}

//=========================================================[b60014]
// Name:        coop_spEquip
// Class:       Player
//              
// Description: Handles weapons load from script in singleplayer
//              we need to have this delayed because in sp the player is spawned way before the level or the script is ready
//
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
//this should be split up and the gameupgrade for singleplayer and the coop stuff should go into sperate funcs
void Player::upgPlayerSpEquip()
{
	if (g_gametype->integer != GT_SINGLE_PLAYER || !Director.PlayerReady() || coop_returnEntityIntegerVar(this,"_spArmoryEquiped") == 1) {
		return;
	}

	entityVars.SetVariable("_spArmoryEquiped","1");
	coop_armoryEquipPlayer(this);

	//make sure the coop objectives hud is displayed when we play a custom (coop) map
	if (game.isStandardLevel) {
		gi.SendServerCommand(entnum, "stufftext \"set coop_oExc score\"\n");
	}
	else {
		gi.SendServerCommand(entnum, "stufftext \"set coop_oExc pushmenu coop_objectives\"\n");
		upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand coop_objectivesMapAuthor title %s", game.coop_author.c_str()));
		upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand coop_objectivesMap title %s", level.mapname.c_str()));
		upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand coop_objectivesSkillValue title %s", coop_returnStringSkillname(skill->integer).c_str()));
	}
}

//=========================================================[b60014]
// Name:        coop_getId
// Class:       Player
//              
// Description: Gets Player coop Id
//              
// Parameters:  void
//              
// Returns:     str
//              
//================================================================
str Player::coop_getId()
{
	if (multiplayerManager.inMultiplayer() && !upgPlayerIsHost()) {
		return coopPlayer.coopId;
	}
	else {
		cvar_t* cvar = gi.cvar_get("coop_cId");
		str sCvar = (cvar ? cvar->string : "");
		upgStrings.manipulateTrim(sCvar, "coopcid ");
		return sCvar;
	}
}

//=========================================================[b60014]
// Name:        coop_setId
// Class:       Player
//              
// Description: Sets Player coop Id
//              
// Parameters:  str
//              
// Returns:     void
//              
//================================================================
void Player::coop_setId(str sId)
{
	if (multiplayerManager.inMultiplayer()) {
		coopPlayer.coopId = sId;
	}
}

//=========================================================[b60014]
// Name:        coop_getInstalled
// Class:       Player
//              
// Description: Checks if player has the coop mod installed or not
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Player::coop_getInstalled()
{
	if (multiplayerManager.inMultiplayer()) {
		if (coopPlayer.installed > 0) {
			return true;
		}
	}
	return true;
}

//=========================================================[b60014]
// Name:        coop_setInstalled
// Class:       Player
//              
// Description: Sets flag that player has coop mod installed
//              
// Parameters:  bool
//              
// Returns:     void
//              
//================================================================
void Player::coop_setInstalled(bool bIns)
{
	if (multiplayerManager.inMultiplayer()) {
		coopPlayer.installed = bIns;
	}
}

//=========================================================[b60014]
// Name:        coop_getInstalledVersion
// Class:       Player
//              
// Description: Returns Coop Mod installed version
//              
// Parameters:  void
//              
// Returns:     int
//              
//================================================================
int Player::coop_getInstalledVersion()
{
	if (multiplayerManager.inMultiplayer()) {
		return coopPlayer.installedVersion;
	}
	else {
		cvar_t* cvar = gi.cvar_get("coop_ver");
		str sCvar = (cvar ? cvar->string : va("%d", COOP_BUILD));
		return atoi(sCvar);
	}
}

//=========================================================[b60014]
// Name:        coop_setInstalledVersion
// Class:       Player
//              
// Description: Sets Coop Mod installed version
//              
// Parameters:  int
//              
// Returns:     void
//              
//================================================================
void Player::coop_setInstalledVersion(int iVer)
{
	if (multiplayerManager.inMultiplayer()) {
		coopPlayer.installedVersion = iVer;
	}
}

//=========================================================[b60014]
// Name:        coop_getInstalledCheckTime
// Class:       Player
//              
// Description:	Returns Coop Mod installed time check var content
//				Used to detect when coop setup should timeout for player
//              
// Parameters:  void
//              
// Returns:     int
//              
//================================================================
int Player::coop_getInstalledCheckTime()
{
	if (multiplayerManager.inMultiplayer()) {
		return coopPlayer.installedVersion;
	}
	else {
		cvar_t* cvar = gi.cvar_get("coop_ver");
		str sCvar = (cvar ? cvar->string : va("%d", COOP_BUILD));
		return atoi(sCvar);
	}
}

//=========================================================[b60014]
// Name:        coop_setInstalledVersion
// Class:       Player
//              
// Description:	Sets Coop Mod installed time check var
//				Used to detect when coop setup should timeout for player
//              
// Parameters:  int
//              
// Returns:     void
//              
//================================================================
void Player::coop_setInstalledCheckTime(int iTime)
{
	if (multiplayerManager.inMultiplayer()) {
		coopPlayer.installedVersion = iTime;
	}
}
	
//[b607] chrissstrahl - updated and improved and also fixed player names with space not showing
//================================================================
// Name:        coop_playerCommunicator
// Class:       -
//              
// Description: adds player names to !transport section of the coop communicator
//              
// Parameters:  Player*, int iAdd <0,1,2> 0=getlist,1=add, 2=remove
//              
// Returns:     void
//              
//================================================================
void coop_playerCommunicator(Player* player, int iAdd)
{
	if (!game.coop_isActive) {
		return;
	}

	str sName;
	Player *currentPlayer = NULL;
	int iMaxCients = maxclients->integer;
	if (iMaxCients > COOP_MAX_PLAYERS) {
		iMaxCients = COOP_MAX_PLAYERS;
	}

	//grab player name or set empty for the slot in the menu
	if (iAdd<=0){
		sName = "$$Empty$$";
	}
	else {
		sName = player->client->pers.netname;
	}

	//don't send update info to leaving player 
	for (int i = 0; i < iMaxCients; i++) {
		currentPlayer = (Player *)g_entities[i].entity;

		//don't send update info to player without mod menu or if it is bot
		if (!currentPlayer || currentPlayer->edict->svflags & SVF_BOT) { //[b60011]Chrissstrahl - disabled coop check as a quick fix cuz entering player has not yet ccoop detected - !currentplayer->coop_getInstalled() 
			continue;
		}

		//don't send update info to leaving player
		if (iAdd <= 0 && i == player->entnum) {
			//gi.Printf(va("COOPDEBUG [%s] Leaving Player skipped: %s\n", currentPlayer->client->pers.netname, player->client->pers.netname));
			continue;
		}

		//send list to current player communicator menu, menu has 8 slots
		for (int j = 0; j < 8; j++) {
			str sListName;
			Player *listPlayer = (Player *)g_entities[j].entity;

			if (!listPlayer || !listPlayer->isClient() || iAdd <= 0 ) {
				sListName = "$$Empty$$";
			}
			else {
				sListName = listPlayer->client->pers.netname;
				sListName = upgStrings.getReplacedSpaceWithUnderscore(sListName);
			}

			upgPlayerDelayedServerCommand(currentPlayer->entnum, va("globalwidgetcommand coop_comTran%i title %s\n",j, sListName.c_str()));
			//multiplayerManager.HUDPrint(player->entnum, va("\nCOOPDEBUG coop_comTran%i title %s\n",j, sListName.c_str()));
			//gi.Printf(va("COOPDEBUG [%s] coop_comTran%i title %s\n", currentPlayer->client->pers.netname, j, sListName.c_str()));
		}
	}
}

//================================================================
// Name:        coop_playerGetDataSegment
// Class:       -
//              
// Description: returns a specific data segment for a player id from ini file
//				like health, ammo and other statistics
//              
// Parameters:  Player*, sort int
//              
// Returns:     string
//              
//================================================================
str coop_playerGetDataSegment( Player *player , short int iNumber )
{
	str sData;
	str sSegment = "";
	sData = coop_parserIniGet( coopServer.getServerDataIniFilename() , player->coop_getId(), "client");
	upgStrings.manipulateTrim( sData , " \t\r\n" );

	//[b60012] chrissstrahl - fix missing .c_str()
	if ( !Q_stricmp( sData.c_str(), "" ) )
		return "0";

	int short iChar = 0;
	int short iCurrentSegment = 1;

	for ( iChar = 0; iChar < sData.length(); iChar++ ) {
		if ( sData[iChar] == ' ' ) {
			iCurrentSegment++;
			continue;
		}
		if ( iCurrentSegment == iNumber ) {
			sSegment += sData[iChar];
		}
		else if ( iCurrentSegment > iNumber ) {
			break;
		}
	}
	return sSegment;
}

//================================================================
// Name:        coop_manageIntervalTransmit
// Class:       -
//              
// Description: transmit data in a interval fashion
//              
// Parameters:  gentity_t *ent
//              
// Returns:     VOID
//              
//================================================================
void coop_manageIntervalTransmit( Player* player , str sData , float fInterval , float &last )
{
	//if ( ( last + fInterval ) > level.time ) {
		//return;
	//}
	
	if ( sData == player->upgPlayerGetLastScanData() && (last + fInterval) > level.time) {
		//return;
	}
	last = level.time;
	player->upgPlayerSetLastScanData(sData);
	upgPlayerDelayedServerCommand( player->entnum , sData.c_str() );
}


//================================================================
// Name:        coop_playerSpawnLms
// Class:       -
//              
// Description: tries to spawn the player if in lms or prevent the spawn
//              
// Parameters:  Player *player
//              
// Returns:     BOOL
//              
//================================================================
bool coop_playerSpawnLms( Player *player )
{
	//no coop or lms
	if ( !game.coop_isActive || game.coop_lastmanstanding == 0 || game.levelType < MAPTYPE_MISSION )
		return true;

	//player died after this map was started and
	//player is more than 3 sec on level and
	//player dies more often than allowed
	if (	player->upgPlayerDeathTime() > game.coop_levelStartTime &&
			(player->upgPlayerGetLevelTimeEntered() + 3 ) < level.time &&
			player->coopPlayer.lmsDeaths >= game.coop_lastmanstanding
	){
		multiplayerManager.makePlayerSpectator(player, SPECTATOR_TYPE_FOLLOW, false);

		if ( !level.mission_failed && ( player->coopPlayer.lastTimeHudMessage + 3 ) < level.time ){
			player->coopPlayer.lastTimeHudMessage = level.time;

			if (player->upgPlayerHasLanguageGerman()) {
				multiplayerManager.HUDPrint( player->entnum , "^5Coop^8 ^5L^8ast ^5M^8an ^5S^8tanding ^2Aktiv^8 - ^1Sie sind momentan ausgeschaltet.\n" );
			}
			else {
				multiplayerManager.HUDPrint( player->entnum , "^5Coop^8 ^5L^8ast ^5M^8an ^5S^8tanding ^2Active^8 - ^1You are neutralised for the Moment.\n" );
			}
		}
		return false;
	}
	return true;
}

//================================================================
// Name:        coop_playerRestore
// Class:       -
//              
// Description: restore player his previouse health, armor and ammo
//              
// Parameters:  gentity_t *ent
//              
// Returns:     VOID
//              
//================================================================
void coop_playerRestore( Player *player )
{
	//[b60012] chrissstrahl - fix missing .c_str()
	if ( !player || !game.coop_isActive || !Q_stricmp( player->coop_getId().c_str(), ""))
		return;

	if ( multiplayerManager.isPlayerSpectator( player ) )
	{
		int iTime = atoi( coop_playerGetDataSegment( player , 7 ).c_str() );
		player->upgPlayerDeathTimeSet(iTime);
		if ( game.coop_lastmanstanding )
		{
			if ( !multiplayerManager.isPlayerSpectatorByChoice( player ) )
			{
				if ( player->upgPlayerDeathTime() < game.coop_levelStartTime && ( mp_warmUpTime->integer + 20 ) < level.time )
				{
					multiplayerManager.playerEnterArena( player->entnum , 666 );
				}
			}
		}
		else if( multiplayerManager.getRespawnTime() > 0.0f )
		{
			time_t result = time( NULL );
			localtime( &result );

			if ( ( iTime + ( int )multiplayerManager.getRespawnTime() ) > ( int )result )
			{
				multiplayerManager._playerData[player->entnum]._waitingForRespawn = true;
				multiplayerManager._playerData[player->entnum]._respawnTime = multiplayerManager.getRespawnTime();
			}
		}
		return;
	}

	//if it is not a mission or custom coop map do not restore
	if (game.levelType != MAPTYPE_MISSION && game.levelType != MAPTYPE_CUSTOM) {
		return;
	}

	str sData = coop_parserIniGet(coopServer.getServerDataIniFilename(), player->coop_getId(), "client");
	//[b60012] chrissstrahl - fix missing .c_str()
	if (!Q_stricmp(sData.c_str(), "")){
		return;
	}
	
	//[b60012] chrissstrahl - if it is not a sublevel, do not restore
	if (!gi.areSublevels(level.mapname.c_str(), coop_playerGetDataSegment(player, 8).c_str()))
		return;

	//health armor phaser plasma fed idryll timestamp mapname/enviroment
	//100 200 200 200 200 200 1465368163 mapname

	//trim/clean
	upgStrings.manipulateTrim( sData," \t\r\n" );

	//get first set of data
	int iTempData=0;
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( iTempData > 0 )
	{ 
		//make sure player has at least 10 health!
		if ( iTempData  < 10 )
		{
			iTempData = 10;
		}
		player->health = iTempData;
	}

	//get secound set of data
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );

	//make sure player has at least 10 shield!
	if ( iTempData < 10 )
	{
		iTempData = 10;
	}

	//if ( ( Sentient * )player->GetArmorValue() ){( Sentient * )player->SetArmorValue( iTempData );}
	//else{...}
	Event *armorEvent;
	armorEvent = new Event( EV_Sentient_GiveArmor );
	armorEvent->AddString( "BasicArmor" );
	armorEvent->AddInteger( iTempData );
	player->ProcessEvent( armorEvent );

	//get third set of data
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( iTempData > 0 )
	{player->GiveAmmo("Phaser",(iTempData - player->AmmoCount( "Phaser" )),false,-1);}

	//get forth set of data
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( iTempData > 0 )
	{player->GiveAmmo("Plasma",(iTempData - player->AmmoCount( "Plasma" )),false,-1);}

	//get fifth set of data
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( iTempData > 0 )
	{player->GiveAmmo("Fed",(iTempData - player->AmmoCount( "Fed" )),false,-1);}

	//get sixth set of data
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( iTempData > 0 )
	{player->GiveAmmo("Idryll",(iTempData - player->AmmoCount( "Idryll" )),false,-1);}

	//get seventh set of data - deathtime unixtimestamp
	iTempData = coop_returnIntFromFormatedString( sData , ' ' );
	if ( sData.length() == 10 ) {
		iTempData = atoi( sData );
		player->upgPlayerDeathTimeSet(iTempData);
		//try to respawnif data is restored
		if ( multiplayerManager.isPlayerSpectator( player ) && !multiplayerManager.isPlayerSpectatorByChoice( player ) ) {
			multiplayerManager.playerEnterArena( player->entnum , iTempData );
		}
	}
}

//================================================================
// Name:        coop_playerSetup
// Class:       -
//              
// Description: Setup the player for coop, execute clientside cfg, set entity vars on player for script use
//              
// Parameters:  gentity_t *ent
// Parameters:  Player *player
//              
// Returns:     bool
//              
//================================================================
bool coop_playerSetup( gentity_t *ent )
{
	if ( !ent )
		return false;
	Player *player = multiplayerManager.getPlayer( ent - g_entities );
	return coop_playerSetup( player );
}
bool coop_playerSetup(Player* player)
{
	//[b607] chrissstrahl - allow cancelation of pending Missionfailure event (only if autofail because of empty server)
	if (game.coop_autoFailPending) {
		world->CancelEventsOfType(EV_World_AutoFailure);
	}

	//[b60014] chrissstrahl - we don't want to handle bots
	if (multiplayerManager.inMultiplayer() && player->upgPlayerIsBot()) { return true; }

	//[b60014] chrissstrahl - temorarly disable sv_floodprotect to allow setup commands
	//from client which are send fast in groups, floodprotect actually discards them (as it should)
	coopServer.svFloodProtectDisable();

	//needs only to be send to players that played the coop mod in singleplayer on a custom map before, so only send to players with coop mod
	upgPlayerDelayedServerCommand(player->entnum, "bind TAB +objectives_score");
	
	//[b60014] chrissstrahl - added check for cl_running
	//because starting a local dedicated server and joing it from the same installation
	//is detected as a player who is joining as host, while technically right this is
	//not how we want it to go
	if (player->upgPlayerIsHost()) {
		coop_playerSetupHost(player);
	}
	else {
		coop_playerSetupClient(player);
	}

	//[b60011] chrissstrahl - exit here on singleplayer / solomatch
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return true;
	}

	//[b60013] chrissstrahl - moved here - execute clientside inizialisation for coop - for all clients
	upgPlayerDelayedServerCommand(player->entnum, "exec coop_mod/cfg/init.cfg");

	//hzm coop mod chrissstrahl - mark as not respawned
	//hzm coop mod chrissstrahl - mark to respawn next time where player died
	player->coopPlayer.deathViewangleY = 0;
	//player->coopPlayer.respawnAtRespawnpoint = true;

	if(game.coop_isActive){
		//Manage Auto NPC Team
		Event* newEvent2 = new Event(EV_Player_coop_playerNpcCheckAutoTeam);
		player->PostEvent(newEvent2, 3.0f);

		//[b60011] chrissstrahl - changed: player is now starting the thread, renamed thread
		//notify level scripts that the player just spawned - this is used on custom map scripts
		ExecuteThread("coop_justEntered", true, (Entity*)player);

		//[b607] chrissstrahl - add this player to the communicator menu
		coop_playerCommunicator(player, 1);

		//[b60011] chrissstrahl - set for all players - SCOREBOARD Gametype Name
		upgPlayerDelayedServerCommand(player->entnum, va("set mp_gametypename ^8HZM Coop Mod %i^0 %i", COOP_BUILD, mp_gametype->integer));
	}

//[b60013] chrissstrahl - 
//hzm coop mod chrissstrahl - place player at spawnpoint
//coop_playerPlaceAtSpawn( player );

	//hzm coop mod chrissstrahl - enable ai again, if it was disabled, as there is now a player on the server again
	coop_serverManageAi(true);

	return true;
}
//================================================================
// Name:        coop_playerSetupClient
// Class:       -
//              
// Description: Does Setup for client player
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSetupClient(Player* player)
{
	//[b60014] chrissstrahl
	if (player->upgPlayerIsBot()) {
		gi.Printf("COOPDEBUG coop_playerSetupClient %s [Is a BOT - abborted]\n", player->client->pers.netname);
		return;
	}
	//if (multiplayerManager.inMultiplayer()) {
		//multiplayerManager.HUDPrint(player->entnum, "COOPDEBUG coop_playerSetupClient\n");
	//}

	//[b60011] chrissstrahl - get player langauge/clientid/clientCoopVersion
	//[b60012] chrissstrahl - fixed missing letter c
	//[b60014] chrissstrahl - put both commands together
	gi.SendServerCommand(player->entnum, "stufftext \"vstr coop_cId;vstr coop_verInf\"\n");
	
	//Do this only during a active coop game
	if (game.coop_isActive) {
		//[b60011] chrissstrahl - get player class
		upgPlayerDelayedServerCommand(player->entnum, "vstr coop_class");

		//[b60011] chrissstrahl - headhudtext widget hide in multiplayer, because it does not work right (flickering)
		upgPlayerDelayedServerCommand(player->entnum, "globalwidgetcommand DialogConsole rect -10000 0 0 0");
	}
}


//================================================================
// Name:        coop_playerSetupHost
// Class:       -
//              
// Description: Does Setup for host player
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSetupHost(Player* player)
{
	//[b60014] chrissstrahl - changed to use functions that handle sp/mp/coop
	player->coop_setInstalledVersion(player->coop_getInstalledVersion());
	player->coop_setInstalled(true);

	//[b60014] chrissstrahl - move widget back into the correct place
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER){
		upgPlayerDelayedServerCommand(player->entnum, "globalwidgetcommand DialogConsole rect 8 7 304 89");
	}
	
	//[b60014] chrissstrahl - [SINGLEPLAYER] EXIT
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		return;
	}

	player->coop_setId(coop_checkPlayerCoopIdExistInIni(player, player->coop_getId()));
	coop_playerSetupCoop(player);

	//[b60014] chrissstrahl - [BOTMATCH] EXIT
	if (g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return;
	}	

	coop_playerRestore(player);
}

//================================================================
// Name:        coop_playerGenerateNewPlayerId
// Class:       -
//              
// Description: Generates a uniqe playerid for this client for idendification
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerGenerateNewPlayerId(Player* player)
{
	//that should create a pretty uniqe player id
	time_t curTime;
	time(&curTime);
	str sPlayerId = va("%d%d", (int)curTime, player->entnum);

	//add current client number to make sure we add a absolute uniqe player id
	//even if two players join at the same instance
	player->coop_setId(sPlayerId);

	gi.SendServerCommand(player->entnum, va("stufftext \"seta coop_cId 0;set coop_cId coopcid %s\"\n", sPlayerId.c_str()));
}

//================================================================
// Name:        coop_playerSaveNewPlayerId
// Class:       -
//              
// Description: Saves a new playerid for this client in the server ini file for idendification
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSaveNewPlayerId(Player *player)
{
	//[b60014] chrissstrahl - don't handle Bots
	if (player->upgPlayerIsBot()) {
		return;
	}

	//if player has no id send from his config, generate one
	if (!player->coop_getId().length()) {
		coop_playerGenerateNewPlayerId(player);
	}

	//write id of player to server ini
	coop_parserIniSet(coopServer.getServerDataIniFilename(), player->coop_getId(), "100 40 0 0 0 0", "client");
	
	//hzm coop mod chrissstrahl - allow new players to join directly in on LMS and respawntime
	if (multiplayerManager.inMultiplayer()) {
		player->upgPlayerDeathTimeSet(0);
		multiplayerManager._playerData[player->entnum]._waitingForRespawn = true;
		multiplayerManager._playerData[player->entnum]._respawnTime = 0.0f;
	}
}

//================================================================
// Name:        coop_playerSetupCoop
// Class:       -
//              
// Description: Player has coop, do setup, this is supposed to be called only once per level
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSetupCoop( Player *player )
{
	//[b60014] chrissstrahl
	if (!multiplayerManager.inMultiplayer() || g_gametype->integer == GT_SINGLE_PLAYER) {
		return;
	}

	//hzm coop mod chrissstrahl - notify game about the client state
	//can also be used on regular deathmatch by script check
	player->coop_setInstalled(true);

	//make sure the setup # executed while coop is not active
	//because the command can and will be executed even if there is no coop
	if (game.coop_isActive) {
		//hzm coop mod chrissstrahl - update mission objective hud and callvote, once	
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_objectivesMap title %s", level.mapname.c_str())); //[b60012] chrissstrahl - fix missing .c_str()
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coop_objectivesSkillValue title %s", coop_returnStringSkillname(skill->integer).c_str()));
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoSkill title %s", coop_returnStringSkillname(skill->integer).c_str()));
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoMvSpd title %d", game.coop_maxspeed));
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoRspwt title %d", game.coop_respawnTime));
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoLms title %d", game.coop_lastmanstanding));
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoAw title %d", (int)game.coop_awardsActive));

		//[b607] chrissstrahl - deadbodies option
		//[b607] chrissstrahl - teamicon option
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoDb title %d", game.coop_deadBodiesPerArea));
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoCh title %d", (short)coopChallenges.iCurrentChallenge));
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoSt title %d", (int)game.coop_stasisTime));

		//[b607] chrissstrahl - airaccelerate option
		int iAccel;
		if (world->getPhysicsVar(WORLD_PHYSICS_AIRACCELERATE) != -1.0f) {
			iAccel = (int)world->getPhysicsVar(WORLD_PHYSICS_AIRACCELERATE);
		}
		else {
			iAccel = (int)sv_airaccelerate->value;
		}
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoAa title %d", iAccel));

		str sFF = va("%f", game.coop_friendlyFire);
		upgStrings.manipulateFromWithLength(sFF, 0, 4);
		upgPlayerDelayedServerCommand(player->entnum, va("globalwidgetcommand coopGpoFF title %s", sFF.c_str()));

		//hzm coop mod chrissstrahl - disable all other inactive blips
		for (int i = 0; i < COOP_RADAR_MAX_BLIPS; i++) {
			player->coopPlayer.radarBlipActive[i] = false;
		}

		//hzm coop mod chrissstrahl - reset objectives, so they may also work in singleplayer
		coop_objectivesSetup(player);

		//[b60014] chrissstrahl - fix class not applaying on local server because lastTimeChangedClass and lastTimeAppliedClass are both 999 at start
		player->coopPlayer.lastTimeChangedClass = (player->upgPlayerGetLevelTimeEntered() - 42);

		//[b60011] chrissstrahl 
		coopChallenges.playerEnteredWarning(player);
	}

	player->coop_playerSetupComplete(true);

	if (game.coop_isActive) {
		//[b60011] chrissstrahl - setupComplete needs to be true for this to work
		coop_classSet(player, "current");
	}
}


//================================================================
// Name:        coop_playerSetupNoncoop
// Class:       -
//              
// Description: Player does not have the coop mod installed, this is supposed to be called only once per level
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSetupNoncoop( Player *player)
{
	//hzm coop mod chrissstrahl - notify game about the client state
	player->coop_setInstalled(false);

	//[b60014] chrissstrahl - don't handle bots
	if (player->upgPlayerIsBot()) {
		player->coop_playerSetupComplete(true);
		return;
	}

	//hzm coop mod chrissstrahl - tell player that it would be so much better if he has the coop mod
	//this is not a priority message, we don't bother with it if the client has heavy traffic
	if ( game.coop_isActive && !level.mission_failed ){
		if ( player->upgPlayerHasLanguageGerman() ) {
			upgPlayerDelayedServerCommand( player->entnum , "hudprint ^2Holen Sie sich den ^5HZM Coop Mod^2 fuer ein volles Erlebniss! ^5!help^8 eingeben fuer Befehle.\n" );
		}
		else {
			upgPlayerDelayedServerCommand( player->entnum , "hudprint ^2For the full Experience please download the ^5HZM Coop Mod^2! ^8Enter ^5!help^8 for Commands.\n" );
		}
	}

	//hzm coop mod chrissstrahl - do the regular setup here after coop has or has not been detected
	coop_objectivesSetup( player );

	player->coop_playerSetupComplete(true);

	//[b60011] chrissstrahl - setupComplete needs to be true for this to work
	coop_classSet(player, "current");
}


//[b60011] chrissstrahl - major restructure, moved commands to - coopGamecmds.cpp
//================================================================
// Name:        coop_playerSay
// Class:       -
//              
// Description:  this addes the coop specific commands, that have to be entred into chat catch chat text that is suppose to be a coop command, starting with ! as very first letter
//              
// Parameters:  Player *player , str sayString
//              
// Returns:     bool
//              
//================================================================
bool coop_playerSay( Player *player , str sayString)
{
	if ( !player || g_gametype->integer == GT_SINGLE_PLAYER)
		return false;


	//[b60011] chrissstrahl - Strip out any bad characters
	int i;
	for (i = 0; i < sayString.length(); i++){
		if (sayString[i] == '%'){
			sayString[i] = '.';
		}
	}
	sayString = sayString.tolower();

	//[b60011] chrissstrahl - clientid backwardscompatibility - supress text
	if (Q_stricmpn(sayString.c_str(), "cid.", 4) == 0) { return true; }


	return false;
}


//================================================================
// Name:        coop_playerEnterArena
// Class:       -
//              
// Description: Handlescoop stuff for player when entering the arena, as active player
//              
// Parameters:  int entnum , float health
//              
// Returns:     bool
//              
//================================================================
void coop_playerEnterArena(int entnum, float health)
{
	if (entnum < 0 || entnum >(game.maxclients - 1))
		return;

	Player *player;
	player = multiplayerManager.getPlayer(entnum);

	if (!player)
		return;

	//hzm coop mod chrissstrahl set when the player spawned last time, used to display objectives in text hud
	player->coop_setSpawnedLastTime();

	if (!game.coop_isActive) {
		return;
	}

	//[b607] chrissstrahl - add all players to blue team in coop
	//[b608] chrissstrahl - adjusted that it makes player go to blue team if he is not in blue team, added NOTE
	//
	// NOTE: if a player first joins the game this function is not called!
	// NOTE: void MultiplayerManager::addPlayer is always called
	// NOTE: this function should probably called from void MultiplayerManager::addPlayer to be sure it always works
	//		 but this might take to much time for testing and adjusting
	//
	if (multiplayerManager.getPlayersTeam(player) != NULL && multiplayerManager.getPlayersTeam(player)->getName() != "Blue") {
		multiplayerManager.joinTeam(player, "Blue");
		if ((player->upgPlayerGetLevelTimeEntered() + 2) < level.time && !multiplayerManager.isPlayerSpectator(player)) {
			//[b60012][cleanup] chrissstrahl - this could be put into a func
			if (player->upgPlayerHasLanguageGerman()) {
				player->hudPrint("^5INFO:^2 Coop erlaubt nur blaues Team.\n");
			}
			else {
				player->hudPrint("^5INFO:^2 Coop does only allow blue team.\n");
			}
		}
		return;
	}

	//[b60014] chrissstrahl - reset/hide radar blips
	//coop_radarResetCFG( player );
	//now managed in ea.cfg, which is executed at the end of this function

	//hzm coop mod chrissstrahl - used to store if a medic was notified to heal this now critically injured player
	player->coopPlayer.lastTargetedClassSend = "";

	//hzm coop mod chrissstrahl - give weapons, make use start weapon
	coop_armoryEquipPlayer( player );

	//hzm coop mod chrissstrahl - update statistics of how many active players of each class are
	coop_classUpdateClassStats();
	//coop_classUpdateHealthStatFor( player->entnum );

	//hzm coop mod chrissstrahl - this clears the notargetflag and makes player valid target for ai
	if ((player->flags & FL_NOTARGET))
		player->flags &= ~FL_NOTARGET;

	//hzm coop mod chrissstrahl - display a icon and some text, this is a test...
	//int iconIndex;
	//iconIndex = gi.imageindex("sysimg/icons/items/armor");
	//player->setItemText(iconIndex, va("$$Specialties$$: $$Medic$$ %i", 10000 ));


	//Event *newEvent = new Event(EV_Sentient_SetViewMode);
	//newEvent->AddString("forcevisible");
	//newEvent->AddInteger(0);
	//player->ProcessEvent(newEvent);

	//hzm coop mod chrissstrahl - check if player has a certain item/weapon
	//if (player->FindItemByModelname("models/weapons/worldmodel-tricorder.tik"))

	//_playerSpecialtyData[player->entnum]._specialty == SPECIALTY_HEAVY_WEAPONS;
	//gi.SendServerCommand( player->entnum , "stufftext \"ui_addhud mp_specialties\"\n" );	

	//[b60011] chrissstrahl - changed: player is now starting the thread, player number removed
	//notify level scripts that the player just spawned - this is used on custom map scripts
	ExecuteThread("coop_justSpawnedplayer", true, (Entity *)player);

	//hzm coop mod chrissstrahl - add all mission relevant huds to the players interface
	str sValue;
	ScriptVariable *entityData;
	int iHuds;
	for ( iHuds = 0; iHuds < 8; iHuds++ ){

		sValue = "";
		entityData = NULL;
		entityData = world->entityVars.GetVariable( va( "coop_registredHud%d" , iHuds ) );

		if ( entityData == NULL ){
			continue;
		}
		
		sValue = entityData->stringValue();
		if ( sValue  != ""){
			coop_hudsAdd( player , sValue );
		}

		//[b60011] chrissstrahl - execute command if one is attached - usually to exec a cfg or to handle globalwidgetcommands for ui elements
		sValue = "";
		entityData = NULL;
		entityData = world->entityVars.GetVariable(va("coop_registredHud%d_command", iHuds));
		if (entityData != NULL) {
			sValue = entityData->stringValue();
			if (sValue != "") {
				upgPlayerDelayedServerCommand(player->entnum, sValue.c_str());
			}
		}

	}

	//hzm coop mod chrissstrahl - sometimes the timer hud reactivates
	//if ( game.coop_isActive ) {
		//DelayedServerCommand( player->entnum , "globalwidgetcommand dmTimer disable" );
	//}

	//hzm coop mod chrissstrahl - retore health, armor and ammo to previouse state
	coop_playerRestore(player);

//[b60011] chrissstrahl - handle NPC / AI Teammates
//coopNpcTeam.playerReadyCheck(player);
//already in think

	//[b60011] chrissstrahl 
	coopChallenges.playerEntered(player);

	//[b60014] chrissstrahl - moved here because radar was not always added properly
	coop_radarReset(player);
	upgPlayerDelayedServerCommand(player->entnum, "exec coop_mod/cfg/ea.cfg");
}


//================================================================
// Name:        coop_playerKilled
// Class:       -
//              
// Description: this is our own death message handling function, return false if player is not meant to die (coop_gametype)
//              
// Parameters:  Player *killedPlayer , Entity *attacker , Entity *inflictor , int meansOfDeath
//              
// Returns:     void
//              
//================================================================
bool coop_playerKilled( const Player *killedPlayer , const Entity *attacker , const Entity *inflictor , const int meansOfDeath )
{
	if ( !killedPlayer || !attacker || !inflictor ){
		return false;
	}

//check means of death, if vaporised do not keep alive!
//check if player is alone, if the player is alone, make the player die within a moment, unless he shoots a enemy
	Player *playerPrey = ( Player * )g_entities[killedPlayer->entnum].entity;
	Entity *entityAttacker = ( Entity * )g_entities[attacker->entnum].entity;
	Entity *entityInflictor = ( Entity * )g_entities[inflictor->entnum].entity;

	//[b607] chrissstrahl - remember when this player died last in this level
	playerPrey->coop_playerDiedLastUpdate();

	//[b60011] chrissstrahl - count up deaths
	playerPrey->coopPlayer.lmsDeaths++;
	if (playerPrey->coopPlayer.lmsDeaths > game.coop_lastmanstanding) {
		playerPrey->coopPlayer.lmsDeaths = game.coop_lastmanstanding;
	}

	//hzm coop mod chrissstrahl - remember where the player was alive the last time
	playerPrey->coopPlayer.lastAliveLocation = killedPlayer->origin;

	//[b607] chrissstrahl - we now set the data above on the player even if we are not in coop, so we have accsess to them in multi
	if (!game.coop_isActive) {
		return true;
	}

	playerPrey->coopPlayer.respawnAtRespawnpoint = false;

	//[b60013]chrissstrahl - used to determin if player should respawn at where he is or at a predefinied spawn location
	//have this seperate to ensure it always triggers if killed by trigger
	if (!Q_stricmp(entityInflictor->getClassname(), "TriggerHurt")) {
		playerPrey->coopPlayer.respawnAtRespawnpoint = true;
	}

	//hzm coop mod chrissstrahl - check if mission failed
	coop_serverLmsCheckFailure();

	//hzm coop mod chrissstrahl - force update to the restore data of the player
	coop_serverSaveClientData( playerPrey );

	if (	//probably not supported, but we add them anyway
			meansOfDeath != MOD_ELECTRICWATER &&
			meansOfDeath != MOD_GIB &&
			meansOfDeath != MOD_IMPALE &&
			meansOfDeath != MOD_EAT &&
			meansOfDeath != MOD_UPPERCUT &&
			//supported for sure
			meansOfDeath != MOD_SUICIDE &&
			meansOfDeath != MOD_NONE &&
			meansOfDeath != MOD_VAPORIZE_COMP &&
			meansOfDeath != MOD_VAPORIZE_DISRUPTOR &&
			meansOfDeath != MOD_VAPORIZE_PHOTON &&
			meansOfDeath != MOD_SNIPER &&
			meansOfDeath != MOD_DROWN &&
			meansOfDeath != MOD_LAVA &&
			meansOfDeath != MOD_CRUSH &&
			meansOfDeath != MOD_CRUSH_EVERY_FRAME &&
			meansOfDeath != MOD_FALLING &&
			meansOfDeath != MOD_VAPORIZE )
	{
		if ( game.coop_gametype == 1 ){
			//keep alive
			playerPrey->health = 1;
			if ( playerPrey->coop_playerNeutralized()){
				return false;
			}
			//this should make the ai ignore this player
			if ( !( killedPlayer->flags & FL_NOTARGET ) ){
				playerPrey->flags ^= FL_NOTARGET;
			}
			//change mass, so ai can't kick player arround
			playerPrey->coopPlayer.lastMass = killedPlayer->mass;
			playerPrey->mass = 9999;

			//used to signal the game that this player is currently neutralized
			playerPrey->coop_playerNeutralized(true);
			// Give the player the phaser, just to be safe
			playerPrey->giveItem( "models/weapons/worldmodel-phaser-stx.tik" );
			//use phaser when neutralized
			playerPrey->useWeapon( "phaser-stx" , WEAPON_DUAL );
			//set info so the coop client think knows when to dissable the current weapon
			playerPrey->coopPlayer.lastTimeNeutralized = level.time;
			str printString = killedPlayer->client->pers.netname;
			printString += "^8 was Injured, and needs help to get up\n";
			coop_textHudprintAll( printString.c_str() );

			//exit from here on
			return false;
		}
		else if ( game.coop_gametype == 2 ){
		}
	}

	int i = 0;
	bool idendified = false;

	str sModel = upgStrings.getFileName( inflictor->model ).tolower();
//////gi.Printf( va( "%s #%s %s killed %s! $%s mod: %f \n" , inflictor.getClassname() , attacker.getClassname() , sModel.c_str() , killedPlayer->client->pers.netname , inflictor->targetname , meansOfDeath ) );
	//playerPrey->hudPrint( sModel.c_str() );
	
	//this is the printout string of the killed message, let us start with the player his name
	str printString = killedPlayer->client->pers.netname;

	//hzm coop mod chrissstrahl - set viewangle to restore on player if he is respawning at the same location he died
	//hzm coop mod chrissstrahl - remove any attached stuff to prevent the stupid console to print error spam info
	Vector vView;
	vView = playerPrey->getViewAngles();
	playerPrey->coopPlayer.deathViewangleY = floor( vView[1] );
	playerPrey->coopPlayer.respawned = true;
	playerPrey->coopPlayer.injuredSymbolVisible = false;

	//check if we are allowed to spawn players at the place they die or if we are forced to respawn them on a save respawnlocation
	ScriptVariable *entityData = NULL;
	ScriptVariable *entityData2 = NULL;
	entityData = world->entityVars.GetVariable("coop_respawnAtRespawnpoint");
	entityData2 = playerPrey->entityVars.GetVariable( "coop_respawnAtRespawnpoint" );
	
	if (	entityData != NULL && entityData->floatValue() == 1.0f ||
			entityData2 != NULL && entityData2->floatValue() == 1.0f
		)
	{
		playerPrey->coopPlayer.respawnAtRespawnpoint = true;
	}

	coop_classUpdateClassStats();
	//coop_classUpdateHealthStatFor( killedPlayer->entnum );

	playerPrey->removeAttachedModelByTargetname( "globalCoop_playerInjured" );
	//hzm coop mod chrissstrahl - we do no longer attach model, class is now shown with the player name when targeted
	//killedPlayer->removeAttachedModelByTargetname( "globalCoop_playerClassIndicator" );

	coop_hudsKilled((Player *)killedPlayer); //chrissstrahl - added for better managment [b607]

	//[b60011] chrissstrahl - changed: player is now starting the thread, player number removed
	//notify level scripts that the player just spawned - this is used on custom map scripts
	ExecuteThread("coop_justDiedplayer", true, (Entity*)killedPlayer);

//check uservars of inflictor, should we have a custom killmessage ?
//should we use a custom name for the inflictor ?	
	Entity *ePurp = entityAttacker;
	//if(inflictor->isSubclassOf( Projectile ))

	Entity *owner = NULL;
	owner = coop_returnOwnerEntity( entityInflictor );
	//gi.Printf( va( "inflictor:%i attacker:%i killed:%i owner:%i Targetname:%s\n" , inflictor->entnum , attacker->entnum , killedPlayer->entnum , owner->entnum , attacker->targetname.c_str() ) );

	if ( owner == NULL ){
		owner = entityAttacker;
	}

	if ( !attacker->isSubclassOf( Actor ) && !owner->isSubclassOf( Player ) && !owner->isSubclassOf( Actor ) )
	{
		//gi.Printf( va( "inflictor: %i - ePurp: %i\n" , inflictor->entnum , ePurp->entnum) );
		//[609] chrissstrahl - reenabled this, because a trigger (mod_electric) killing a player
		//would make the code use $world from attacker, instead of using the inflictor which is
		//the actual cause for the damage - if world is really the attacker it is also the inflictor
		ePurp = (Entity *)inflictor;//inflictor
	}

	//if inflictor (mostly kill triggers) is marked as badspot, it means we don't want players to respawn there
	if ( coop_checkStringInUservarsOf( ePurp , "badspot" ) ||
		//probably not supported, but we add them anyway
		meansOfDeath == MOD_ELECTRICWATER ||
		meansOfDeath == MOD_GIB ||
		meansOfDeath == MOD_IMPALE ||
		meansOfDeath == MOD_EAT ||
		//meansOfDeath == MOD_SUICIDE || //- disabled for testing
		meansOfDeath == MOD_NONE ||
		meansOfDeath == MOD_ELECTRIC || //[609] chrissstrahl - added to prevent players from respawning there
		meansOfDeath == MOD_VAPORIZE_COMP ||
		meansOfDeath == MOD_VAPORIZE_DISRUPTOR ||
		meansOfDeath == MOD_VAPORIZE_PHOTON ||
		meansOfDeath == MOD_SNIPER ||
		meansOfDeath == MOD_DROWN ||
		meansOfDeath == MOD_LAVA ||
		meansOfDeath == MOD_CRUSH ||
		meansOfDeath == MOD_CRUSH_EVERY_FRAME ||
		meansOfDeath == MOD_FALLING ||
		meansOfDeath == MOD_VAPORIZE)
	{
		playerPrey->coopPlayer.respawnAtRespawnpoint = true;
	}

	str sEntityName , eEntityKillmessage;
	//[609] chrissstrahl - falldamage should always be falldamage
	if (meansOfDeath != MOD_FALLING) {
		for (i = 1; i < 5; i++) {
			int j = 0;
			entityData = NULL;
			sEntityName = "";
			eEntityKillmessage = NULL;
			entityData = ePurp->entityVars.GetVariable(va("uservar%d", i));
			if (entityData == NULL) {
				continue;
			}
			sEntityName = entityData->stringValue();

			//[b60012] chrissstrahl - fix missing .c_str()
			if (Q_stricmpn("killmessage", sEntityName.c_str(), 11) == 0) {
				if (sEntityName.length() > 12) {
					str tempName = sEntityName;
					sEntityName = NULL;
					for (j = 12; j < tempName.length(); j++) {
						sEntityName += tempName[j];
					}
					printString += sEntityName;
					idendified = true;
					break;
				}
			}
			//[b60012] chrissstrahl - fix missing .c_str()
			else if (Q_stricmpn("name", sEntityName.c_str(), 4) == 0) {
				if (sEntityName.length() > 5) {
					str tempName = sEntityName;
					sEntityName = "";
					printString += "^8 was neutralized by ";
					for (j = 5; j < tempName.length(); j++) {
						sEntityName += tempName[j];
					}
					printString += sEntityName;
					idendified = true;
				}
				break;
			}
		}
	}
//custom defined kill message, print it and exit here
	if ( idendified == true ){
		//gi.Printf( va( "Used custom name or killmessage in uservar%d of '$%s' (%s)\n" , i , ePurp->targetname , ePurp->getClassname() ) );
		printString += "\n";
		coop_textHudprintAll( printString.c_str() );
		return true;
	}
//check if the inflictor is a player
	//[b60012] chrissstrahl - fix missing .c_str()
	if (	attacker->isSubclassOf( Player ) ||
			Q_stricmp(sModel.c_str(),"fx-sml-exp.tik") == 0 ||
			Q_stricmpn(entityInflictor->getClassname(), "MultiExploder", 13) == 0 ||
			Q_stricmpn(entityInflictor->getClassname(), "ExplodeObject" , 13 ) == 0)
	{
		if ( attacker != killedPlayer && attacker->isSubclassOf( Player ) ){
			printString += va(" was neutralized by their Teammate: %s",attacker->client->pers.netname );
		}else if (	Q_stricmp( sModel.c_str(), "fx-sml-exp.tik") == 0 ||
					Q_stricmpn( entityInflictor->getClassname(),  "MultiExploder", 13 ) == 0 ||
					Q_stricmpn( entityInflictor->getClassname(), "ExplodeObject" , 13 ) == 0)
		{
			printString += "^8 was neutralized by a ^2Explosion";
		}
		else
		{
			printString += " ^8$$MOD_SUICIDE$$";
		}
	}
//check if inflictor is NOT a Actor, and if we are killed by a certain damage type
	else if ( !attacker->isSubclassOf( Actor ) &&
				meansOfDeath == MOD_FALLING ||
				meansOfDeath == MOD_DROWN ||
				meansOfDeath == MOD_SLIME ||
				meansOfDeath == MOD_ELECTRIC ||
				meansOfDeath == MOD_FIRE ||
				meansOfDeath == MOD_ON_FIRE ||
				meansOfDeath == MOD_LAVA )
	{
		switch ( meansOfDeath )
		{
		case MOD_DROWN:
			printString = "$$MOD_DROWN$$";
			break;
		case MOD_LAVA:
			printString = "$$MOD_LAVA$$";
			break;
		case MOD_SLIME:
			printString = "$$MOD_SLIME$$";
			break;
		case MOD_FALLING:
			printString = "$$MOD_FALLING$$";
			break;
		case MOD_ELECTRIC:
			printString = "was electrified";
			break;
		case MOD_FIRE:
		case MOD_ON_FIRE:
			printString = "was burned";
			break;
		default:
			printString = "$$MOD_SUICIDE$$";
			break;
		}
		playerPrey->coopPlayer.respawnAtRespawnpoint = true;
		printString = va( "%s %s" , killedPlayer->client->pers.netname , printString.c_str() );
	}
//inflictor is a actor
	else if ( attacker->isSubclassOf( Actor ) ){
		Actor *actor = (Actor*)attacker;
		//[b60011] chrissstrahl - added filename to printout
		str sDeathListFile = "deathlist.ini";
		str sActorResolved = coop_parserIniGet(sDeathListFile, actor->name.c_str(), "actorname");
		if ( !sActorResolved.length() ) {
			printString += va(" # Unhandled (in %s) actorname: %s\n", sDeathListFile.c_str(), actor->name.c_str());
		}
		else {
			printString += sActorResolved;
		}

	}
	else if ( !Q_stricmp( entityInflictor->getClassname(), "TriggerHurt") ){
		printString += " ^8$$MOD_SUICIDE$$";
	}
//hzm coop mod chrissstrahl - owner is not a actor
/////////possible owners:
//SCRIPT OWNER
//-ScriptOrigin //script_origin entity
	else{
		printString += coop_returnStringDeathmessagenameForEntity( entityInflictor );
	}

	//hzm coop mod chrissstrahl - The Line Must Be Drawn Here! This far no further! <- omg a movie qoute, is this your first contact ? Maybe you should assimilate more code...
	printString += " \n";

	//hzm coop mod chrissstrahl - keep text minimal after failure
	if ( !level.mission_failed ) {//hzm gameupdate chrissstrahl - keep text minimal after failure
		coop_textHudprintAll( printString.c_str() );
	}

	return true;
}


//================================================================
// Name:        coop_playerModelChanged
// Class:       -
//              
// Description: this is our own death message handling function i'm sure we will add more stuff to it as we develop and test the mod
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerModelChanged( Player *player )
{
	if ( game.coop_isActive && player )
	{
		//hzm coop mod chrissstrahl - remove the injured symbol
		player->coopPlayer.injuredSymbolVisible = false;
		player->removeAttachedModelByTargetname( "globalCoop_playerInjured" );

		//[b60011] chrissstrahl - changed: player is now starting the thread, player number removed
		//notify level scripts that the player just spawned - this is used on custom map scripts
		ExecuteThread("coop_justChangedModel", true, (Entity*)player);
	}
}


//================================================================
// Name:        coop_playerUsed
// Class:       -
//              
// Description: called when the player gets used
//              
// Parameters:  Player *usedPlayer , Player *usingPlayer , Equipment *equipment 
//              
// Returns:     bool
//              
//================================================================
void coop_playerUsed( Player *usedPlayer , Player *usingPlayer , Equipment *equipment )
{
	coop_classPlayerUsed( usedPlayer , usingPlayer , equipment );
}


//================================================================
// Name:        coop_playerDamaged
// Class:       -
//              
// Description: this is called wehen the player gets hurt/damaged
//              
// Parameters:  Player *damagedPlayer , Player *attackingPlayer , float damage , int meansOfDeath 
//              
// Returns:     bool
//              
//================================================================
void coop_playerDamaged( Player *damagedPlayer , Player *attackingPlayer , float damage , int meansOfDeath )
{
	//if ( damagedPlayer ){
	//}
}


//================================================================
// Name:        coop_playerSpectator
// Class:       -
//              
// Description: handles coop related stuff when player becomes a spectator
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_playerSpectator( Player *player )
{
	//[b607] chrissstrahl - once the player is a spectator we don't need to execute this again
	//reduce nettraffic and multiexecution of script threads and ini writings
	if (multiplayerManager._playerData[player->entnum]._spectator) {
		return;
	}

	//hzm coop mod chrissstrahl - remove huds - [b607] now using a centralized function
	coop_hudsKilled(player);

	//hzm coop mod chrissstrahl - save latest data from player
	if ( ( mp_warmUpTime->integer + 20 ) < level.time ){
		coop_serverSaveClientData( player );
	}

	player->coopPlayer.injuredSymbolVisible = false;
	player->_makeSolidASAPTime = -1.0f;
	player->_makeSolidASAP = false;

	if ( !game.coop_isActive ){
		return;
	}

	//[b60011] chrissstrahl - handle NPC / AI Teammates
	coopNpcTeam.playerSpectator(player);
	coopChallenges.playerSpectator(player);

	//hzm coop mod chrissstrahl - remove the injured symbol
	player->coopPlayer.injuredSymbolVisible = false;
	player->removeAttachedModelByTargetname( "globalCoop_playerInjured" );

	//hzm coop mod chrissstrahl - this should make the ai ignore this player
	if ( !( player->flags & FL_NOTARGET ) ){
		player->flags ^= FL_NOTARGET;
	}

	//hzm coop mod chrissstrahl - reset respawn flag
	if ( !multiplayerManager._playerData[player->entnum]._waitingForRespawn ) {
		player->coopPlayer.respawned = false;
		player->coopPlayer.respawnAtRespawnpoint = true;
	}

	//[b60013] chrissstrahl  - notify level scripts of teamchange - this is used on custom map scripts
	ExecuteThread("coop_justBecameSpectator", true, (Entity*)player);

	//hzm coop mod chrissstrahl - update statistics of how many active players of each class are
	coop_classUpdateClassStats();
}

//================================================================
// Name:        coop_playerMakeSolidASAP
// Class:       -
//              
// Description: Makes player solid as soon as possible - not stuck in actor or other player
//              
// Parameters:  Player *player
//              
// Returns:     BOOL
//              
//================================================================
bool coop_playerMakeSolidASAPThink(Player* player)
{
	//check if inside a actor or a player
	if ( player->_makeSolidASAP ) {
		if ( coop_checkInsidePlayerOrActor( ( Entity* )player ) || level.cinematic ) {
			player->setSolidType( SOLID_NOT );
			player->_makeSolidASAP = true;
			return false;
		}
		if ( g_gametype->integer == GT_SINGLE_PLAYER || multiplayerManager.inMultiplayer() && !multiplayerManager._playerData[player->entnum]._spectator ) {
			player->setSolidType( SOLID_BBOX );
		}
		player->_makeSolidASAP = false;
	}
	return true;
}

//================================================================
// Name:        coop_playerPlaceableThink
// Class:       -
//              
// Description: Handles the placable object each - called from playerthink, coop only command
//              
// Parameters:  Player *player
//              
// Returns:     VOID
//              
//================================================================
void coop_playerPlaceableThink(Player* player)
{
	//[b60014] chrissstrahl - multiplayer only command
	if (!multiplayerManager.inMultiplayer()) {
		return;
	}

	if (!player->coopPlayer.ePlacable) {
		return;
	}

	//abbort in certain conditions are meet
	if (	(player->upgPlayerGetLastDamageTime() + 0.5f) > level.time	||
			multiplayerManager.isPlayerSpectator(player)		||
			player->client->ps.jumped							||
			level.cinematic == 1								||
			player->health <= 0	) 
	{
		player->coopPlayer.ePlacable->PostEvent(EV_Remove, 0.0f);
		player->coopPlayer.ePlacable = NULL;
		return;
	}

	SpawnArgs		args;
	Entity*			obj;
	Vector			vPlayer, vPlayerAngle, end_pos;
	trace_t			trace;
	vec3_t			fwd;
	AngleVectors(player->client->ps.viewangles, fwd, NULL, NULL);
	Vector forward = fwd;
	//end_pos = (fwd * 100.0f) + start;

	//trace = G_Trace(start, vec_zero, vec_zero, end_pos, player, MASK_PROJECTILE, false, "ProjectileAttack");
	vPlayer = player->client->ps.origin;
	float fBboxHeight = player->maxs[2];
	fBboxHeight = (fBboxHeight - (fBboxHeight / 100 * 20));
	vPlayer[2] += fBboxHeight; //set z height to 80% of bbox size
	end_pos = (forward * 100.0f + vPlayer);
	trace = G_Trace(vPlayer, vec_zero, vec_zero, end_pos, player, MASK_PROJECTILE, false, "ProjectileAttack");
	//player->coopPlayer.ePlacable->setOrigin(trace.endpos);

	//[b60014] chrissstrahl - make sure to exit on invalid
	if (!player->coopPlayer.ePlacable || !trace.endpos) {
		return;
	}

	trace.endpos[2] = vPlayer[2];
	vPlayerAngle[1] = player->client->ps.viewangles[1];

	player->coopPlayer.ePlacable->setOrigin(trace.endpos);
	player->coopPlayer.ePlacable->setAngles(vPlayerAngle);

	//drop about 100 units - do stuff here if it could not be dropped
	if (!player->coopPlayer.ePlacable->droptofloor(100)) {
		//if object could not be placed, display it at the height of feet of player and change animation so it changes skin/texture
		trace.endpos[2] = player->client->ps.origin[2];
		player->coopPlayer.ePlacable->setOrigin(trace.endpos);
		player->coopPlayer.ePlacable->animate->RandomAnimate("location_invalid");
		//gi.Printf("ePlacable - can't be placed here\n");
	}
	else {
		extern Event EV_Object_SetAnim;
		Event* event;
		event = new Event(EV_Object_SetAnim);
		event->AddString("location_valid");
		player->coopPlayer.ePlacable->ProcessEvent(event);

		//wait after the placable is shown to player before the click counts at placing it
		if (player->circleMenuLastTimeActive() + (0.45) < level.time && player->GetLastUcmd().buttons & (BUTTON_ATTACKLEFT | BUTTON_ATTACKRIGHT)) {
			//perevent weapon firing
			Event* StopFireEvent;
			StopFireEvent = new Event(EV_Sentient_StopFire);
			StopFireEvent->AddString("dualhand");
			player->ProcessEvent(StopFireEvent);

			//args.setArg("model", player->coopPlayer.ePlacable->model.c_str());

			bool classSpecificStation = false;
			if ("models/item/mp_weapon-spawn.tik" == player->coopPlayer.ePlacable->model) {
				classSpecificStation = true;
				if (player->coopPlayer.className == "HeavyWeapons" || g_gametype->integer == GT_SINGLE_PLAYER) { //Make it work in singleplayer for testing
					args.setArg("model", "models/item/coop_ammoStation.tik");
				}
				else if (player->coopPlayer.className == "Medic") {
					args.setArg("model", "models/item/coop_mediStation.tik");
				}
				else {
					args.setArg("model", "models/item/coop_techStation.tik");
				}
			}
			else {
				args.setArg("model", player->coopPlayer.ePlacable->model.c_str());
			}

			args.setArg("classname", player->coopPlayer.ePlacable->getClassname());
			args.setArg("setmovetype", "" + player->coopPlayer.ePlacable->getMoveType());
			args.setArg("targetname", player->coopPlayer.ePlacable->targetname.c_str());
			args.setArg("notsolid", "1");
			//args.setArg("anim", "idle");
			obj = args.Spawn();
			obj->setOrigin(player->coopPlayer.ePlacable->origin);
			obj->setAngles(player->coopPlayer.ePlacable->angles);
			obj->setSize(player->coopPlayer.ePlacable->mins, player->coopPlayer.ePlacable->maxs);
			//handle class specific stations
			if (classSpecificStation) {
				//remove any previouse class specific placed objects
				if (player->coopPlayer.eClassPlacable) {
					Event* RemoveMe;
					RemoveMe = new Event(EV_Remove);
					player->coopPlayer.eClassPlacable->ProcessEvent(RemoveMe);
				}
				//remember the class specific placed object
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
//player->coopPlayer.eClassPlacable = obj;
			}

			player->coopPlayer.ePlacable->PostEvent(EV_Remove, 0.0f);
			player->coopPlayer.ePlacable = NULL;

			player->_makeSolidASAP = true;
			player->_makeSolidASAPTime = 0.0f;
		}
	}
}

//================================================================
// Name:        coop_playerClientThink
// Class:       -
//              
// Description: This will be called once for each client frame, which will
//              usually be a couple times for each server frame.
//              
// Parameters:  Player *player
//              
// Returns:     VOID
//              
//================================================================
void coop_playerThink( Player *player )
{
	if ( !player ){
		return;
	}

	coop_playerMakeSolidASAPThink( player );
	//coop_playerPlaceableThink(player);	//[b60014] chrissstrahl - disabled as I don't have time to fix it right now

	//[b60014] chrissstrahl - [SINGLEPLAYER] update objectives every secound in sp
	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		if ((coop_returnEntityFloatVar((Entity*)player, "_spLastTimeThink") + 1) < level.time) {
			player->entityVars.SetVariable("_spLastTimeThink",level.time);
			coop_objectivesUpdatePlayer( player );
		}
		return;
	}

	//[b60011] chrissstrahl - put the code in dedicated functions
	player->coop_playerThinkDetectCoop();
	player->coop_playerThinkDetectCoopId();
	player->coop_playerThinkLogin();

	//[b607] chrissstrahl - moved here to prevent players staying solid in regular Multimatch
	if (!game.coop_isActive || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return;
	}

	//hzm coop mod chrissstrahl
	//- update radar
	//- show injured sprite
	coop_radarUpdate( player );
	coop_modelDisplayInjuredSprite( player );

	weaponhand_t	hand = WEAPON_ANY;//get player weapon, we might want to utilize that further
	str				weaponName;
	player->getActiveWeaponName( hand , weaponName );

	//hzm coop mod chrissstrahl - cheap way to allow player move again - temporary changeme fixme unfinished
	//we don't want players to go to spectator and come back again, no no no, play fair
	//this code needs work!
	if ( !multiplayerManager.isPlayerSpectator( player ) ){
		//make sure that the player does not respawn inside a door, this would be bad

		if ( ( player->coop_getSpawnedLastTime() + 1 ) > level.time ) {
			if ( coop_checkEntityInsideDoor( ( Entity * )player ) ) {
				player->coopPlayer.respawnAtRespawnpoint = true;
				//[b60013] chrissstrahl - move player to spawn if stuck in door
				coopSpawnlocation.placeAtSpawnPoint(player);
			}
		}

		if ( player->health > 1 ){
			player->coop_playerNeutralized(false);
			player->upgPlayerDisableUseWeapon( false );
		//do no longer ignore this player
			if ( ( player->flags & FL_NOTARGET ) ){
				player->flags ^= FL_NOTARGET;
			}
		}
		else{
			if ( (player->coopPlayer.lastTimeNeutralized + 0.25f) > level.time ){
				if ( stricmp( weaponName , "phaser-stx" ) == 0 ){
					//disable weapon use from now on - means player can no longer change his weapon
					player->upgPlayerDisableUseWeapon( true );
				}
			}
		}
	}
	
//hzm coop mod chrissstrahl - check/do this each secound once
	if ( ( player->coopPlayer.lastTimeThink + 1.0f ) < level.time ){
		player->coopPlayer.lastTimeThink = level.time;

		//display update notification menu if needed
		coop_hudsUpdateNotification( player );

		if ( player->health < 0.0f || sv_cinematic->integer != 0 || multiplayerManager.isPlayerSpectator( player ) ) {
			return;
		}
			
		//coop_serverSaveClientData( player );
		coop_objectivesUpdatePlayer( player );
		coop_classRegenerate( player );
		coop_classCheckApplay( player );
		coop_classCheckUpdateStat( player );

		//[b60011] chrissstrahl - handle NPC / AI Teammates
		coopNpcTeam.playerReadyCheck(player);
	}
}

//==========================================================[b607]
// Name:        coop_playerConnect
// Class:       -
//              
// Description: executed when a player connects
//              
// Parameters:  Entity*
//              
// Returns:     void
//              
//================================================================
void coop_playerConnect(Entity *ePlayer)
{
	//[b60014] chrissstrahl - also check if entity is of class Player
	if (!game.coop_isActive || !ePlayer || !ePlayer->isSubclassOf(Player)) {
		return;
	}

	//[b607] chrissstrahl - clear configstrings, to allow new player to connect
	if ( level.time > 30.0f ) {
		coop_serverConfigstringRemoveNonCoopStrings();
	}
}

//================================================================
// Name:        coop_playerLeft
// Class:       -
//              
// Description: executed when a player disconnects, quits or times out, reliable since it is called by the player destructor
//              
// Parameters:  
//              
// Returns:     void
//              
//================================================================
void coop_playerLeft( Player *player )
{
	if ( !game.coop_isActive || !player ){
		return;
	}

	//[b60011] chrissstrahl -notify level scripts that the player left - this is used on custom map scripts
	//[b60011] chrissstrahl - moved code here
	ExecuteThread("coop_justLeft", true, (Entity*)player);
	coop_serverLmsCheckFailure();	

	//hzm coop mod chrissstrahl - save current status when player leaves the game (unless he is spec)
	if ( !multiplayerManager.isPlayerSpectator( player ) ){
		coop_serverSaveClientDataWrite( player );

		//[b60011] chrissstrahl - handle NPC / AI Teammates
		coopNpcTeam.playerLeft(player);
	}

	//chrissstrahl - make sure server is restarted if it really needs to
	//no player on the server left, see if server should be rebooted
	if (coop_returnPlayerQuantity(3) <= 1) { //[b607] chrissstrahl - fixed, because this counts leaving player as well
		//if (coop_serverManageReboot(level.mapname.c_str())) {
		//return; //[b607] chrissstrahl - if server is going to reboot we don't need to continue
		//}

		//[b607] chrissstrahl - fail mission (delayed) so the server can reload the map (this prevents certain overload issues caused by ongoing mission scripts (spawning etc))
		int iFailtime = coop_returnCvarInteger("coop_autoFailtime");

		//yes -1 deactivates failtime
		if (iFailtime < 0) {
			return;
		}

		//let us be a little paranoid
		if (iFailtime == 0) {
			iFailtime = 1;
		}
		else if (iFailtime > 3000) {
			iFailtime = 3000;
		}

		//[b60011] chrissstrahl - reset air accelerate, to prevent new players to get confused or stuck
		world->setPhysicsVar("airAccelerate", COOP_DEFAULT_AIRACCELERATE);
		//[b60011] chrissstrahl - reset max speed, to prevent new players to get confused
		world->setPhysicsVar("maxSpeed", COOP_DEFAULT_MAXSPEED);

		//[b60011] chrissstrahl - flushtikis - fixing animation issues of actor and other models - just to be sure
		//coopServer.flushTikis();

		game.coop_autoFailPending = true;
		Event *newEvent2 = new Event(EV_World_AutoFailure);
		world->PostEvent(newEvent2, (iFailtime * 60));

		coop_serverManageAi(false);

		return;
	}

	//hzm coop mod chrissstrahl - update statistic for all players
	coop_classUpdateClassStats();

	//[b607] chrissstrahl - clear player name from all other players their communicator transport buttons
	coop_playerCommunicator(player, 0);
}

//================================================================
// Name:        coop_updateStatsCoopHealth
// Class:       Player
//              
// Description: handles player stats in coop - executed from player.cpp at Player::UpdateStats( void )
//              
// Parameters:  int
//              
// Returns:     int
//================================================================
int Player::coop_updateStatsCoopHealth(int statNum)
{
	int value = 0;
	if (game.coop_isActive && coop_getInstalled()) {
		Entity* target;
		if (statNum == STAT_MP_GENERIC4)
		{
			target = GetTargetedEntity();
			if ((target) && target->isSubclassOf(Player))
			{
				//[b607] chrissstrahl - fix health showing when targeting a different entity that is not a player
				value = (int)(target->getHealth() + 0.99f);
			}
		}
	}
	return value;
}

//================================================================
// Name:        coop_updateStats
// Class:       Player
//              
// Description: handles player stats in coop - executed from player.cpp at Player::UpdateStats( void )
//              
// Parameters:  void
//              
// Returns:     void
//              
// PLEASE SEE ALSO:
// int MultiplayerManager::getStat( Player *player, int statNum )
// void Player::UpdateStats( void )
//================================================================
bool Player::coop_updateStats(void)
{
	if (!game.coop_isActive) {
		return false;
	}

	// Deathmatch stats for arena mode
	if (multiplayerManager.inMultiplayer()) {
		client->ps.stats[STAT_SCORE] = multiplayerManager.getPoints(this);
		client->ps.stats[STAT_KILLS] = multiplayerManager.upgPlayerGetKills(this);
		client->ps.stats[STAT_DEATHS] = multiplayerManager.upgPlayerGetDeaths(this);

		client->ps.stats[STAT_MP_SPECTATING_ENTNUM] = multiplayerManager.getStat(this, STAT_MP_SPECTATING_ENTNUM);

		edict->s.infoIcon = multiplayerManager.getInfoIcon(this, last_ucmd.buttons);

		if (game.coop_awardsActive) {
			client->ps.stats[STAT_MP_AWARD_ICON] = multiplayerManager.getIcon(this, STAT_MP_AWARD_ICON);
			client->ps.stats[STAT_MP_AWARD_COUNT] = multiplayerManager.getStat(this, STAT_MP_AWARD_COUNT);
		}

		//used for player health on targeting
		client->ps.stats[STAT_MP_GENERIC4] = multiplayerManager.getStat(this, STAT_MP_GENERIC4); //SPECIALTY_MEDIC - health targeted
		/*
		//They have multiple use cases (changed with each modifier), I listed one for each so a general idea can be derived
		client->ps.stats[STAT_MP_GENERIC1] = multiplayerManager.getStat(this, STAT_MP_GENERIC1); //_redDestructionObject
		client->ps.stats[STAT_MP_GENERIC2] = multiplayerManager.getStat(this, STAT_MP_GENERIC2); //_blueDestructionObject
		client->ps.stats[STAT_MP_GENERIC3] = multiplayerManager.getStat(this, STAT_MP_GENERIC3); //_blueBombPlace
		client->ps.stats[STAT_MP_GENERIC5] = multiplayerManager.getStat(this, STAT_MP_GENERIC5); //ctfflag
		client->ps.stats[STAT_MP_GENERIC6] = multiplayerManager.getStat(this, STAT_MP_GENERIC6); //oneFlag
		client->ps.stats[STAT_MP_GENERIC7] = multiplayerManager.getStat(this, STAT_MP_GENERIC7); //bomber get name
		client->ps.stats[STAT_MP_GENERIC8] = multiplayerManager.getStat(this, STAT_MP_GENERIC8); //CONTROL_POINT_GAMMA
		*/

		client->ps.stats[STAT_MP_STATE] = multiplayerManager.getStat(this, STAT_MP_STATE);
	}

	return true;
}

//================================================================
// Name:        coop_setObjectivesPrintedTitleLast
// Class:       Player
//              
// Description: Sets coopPlayer.lastTimePrintedObjectivesTitle
//              
// Parameters:  void
//              
// Returns:     void           
//================================================================
void Player::coop_setObjectivesPrintedTitleLast()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.lastTimePrintedObjectivesTitle Access VIOLATION!\n");
	}
	coopPlayer.lastTimePrintedObjectivesTitle = level.time;
}

//================================================================
// Name:        coop_getObjectivesPrintedTitleLast
// Class:       Player
//              
// Description: Gets coopPlayer.lastTimePrintedObjectivesTitle
//              
// Parameters:  void
//              
// Returns:     float    
//================================================================
float Player::coop_getObjectivesPrintedTitleLast()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return 0.0f;
	}
	return coopPlayer.lastTimePrintedObjectivesTitle;
}

//================================================================
// Name:        coop_setSpawnedLastTime
// Class:       Player
//              
// Description: Sets coopPlayer.lastTimeSpawned
//              
// Parameters:  void
//              
// Returns:     void           
//================================================================
void Player::coop_setSpawnedLastTime()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.lastTimeSpawned Access VIOLATION!\n");
	}
	coopPlayer.lastTimeSpawned = level.time;
}

//================================================================
// Name:        coop_getSpawnedLastTime
// Class:       Player
//              
// Description: Gets coopPlayer.lastTimeSpawned
//              
// Parameters:  void
//              
// Returns:     float    
//================================================================
float Player::coop_getSpawnedLastTime()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return 0.0f;
	}
	return coopPlayer.lastTimeSpawned;
}

//================================================================
// Name:        coop_setShowTargetedEntity
// Class:       Player
//              
// Description: Sets coopPlayer.showTargetedEntity
//              
// Parameters:  bool
//              
// Returns:     void           
//================================================================
void Player::coop_setShowTargetedEntity(bool bShow)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		gi.Error(ERR_DROP, "FATAL: coopPlayer.showTargetedEntity Access VIOLATION!\n");
	}
	coopPlayer.showTargetedEntity = bShow;
}

//================================================================
// Name:        coop_getShowTargetedEntity
// Class:       Player
//              
// Description: Gets coopPlayer.showTargetedEntity
//              
// Parameters:  void
//              
// Returns:     bool    
//================================================================
bool Player::coop_getShowTargetedEntity()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return false;
	}
	return coopPlayer.showTargetedEntity;
}

//================================================================
// Name:        coop_playerLoadingSavegame
// Class:       Player
//              
// Description: Used to fix issues when loading savegame in singleplayer
//              
// Parameters:  void
//              
// Returns:     void    
//================================================================
void Player::coop_playerLoadingSavegame()
{
	coopPlayer.objectivesCycle = 0.1f;
	coopPlayer.lastTimePrintedObjectivesTitle = 9999.0f;
}

