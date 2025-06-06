//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// Player class related Upgrades, Updates and Fixes
// This code should be independant from the Coop and be useable without the HZM Coop Mod
//-----------------------------------------------------------------------------------
#include "_pch_cpp.h"
#include "player.h"
#include "puzzleobject.hpp"
#include "powerups.h"
#include "mp_manager.hpp"
#include <qcommon/gameplaymanager.h>

#include "upgPlayer.hpp"
#include "upgGame.hpp"
#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"
#include "upgStrings.hpp"
#include "upgCoopInterface.hpp"
#include "upgWorld.hpp"

#include "coopClass.hpp"

UpgPlayer upgPlayer;

extern Event EV_Player_ActivateNewWeapon;
extern Event EV_Player_DeactivateWeapon;

pendingServerCommand* pendingServerCommandList[MAX_CLIENTS];


//-----------------------------------------------------------------------------------
// Events, these have external Dependencies
//-----------------------------------------------------------------------------------
Event EV_Player_reconnect
(
	"upgPlayerReconnect",
	EV_DEFAULT,
	"",
	"",
	"send reconnect command to player"
);
//[GAMEUPGRADE][b60014] chrissstrahl - print message of the day (delayed)
Event EV_Player_upgPlayerMessageOfTheDay
(
	"upgPlayerMessageOfTheDay",
	EV_DEFAULT,
	"",
	"",
	"Prints the message of the day from cvar mp_motd"
);

//hzm gameupdate daggolin - new commands
Event EV_Player_GetScriptVariablesCommand
(
	"getScriptVariable",
	EV_CHEAT,
	NULL,
	"type",
	"Lists all script-variables of the specified type"
);
//hzm gameupdate chrissstrahl - add new commands for script use
Event EV_Player_setKillThread
(
	"killthread",
	EV_SCRIPTONLY,
	"s",
	"kill_thread",
	"Sets the thread to run if player gets killed."
);
//hzm gameupdate chrissstrahl [b60011] - add new commands for script use
Event EV_Player_getLanguage
(
	"getLanguage",
	EV_SCRIPTONLY,
	"@s",
	"language-string",
	"Gets language string from player."
);
//hzm gameupdate chrissstrahl [b60011] - add new commands for script use
Event EV_Player_setCamera
(
	"setCamera",
	EV_SCRIPTONLY,
	"eF",
	"camera switchtime",
	"Sets view of this player to this camera"
);
//hzm gameupdate chrissstrahl [b60011] - widgetcommand
Event EV_Player_WidgetCommand
(
	"widgetCmd",
	EV_SCRIPTONLY,
	"ssS",
	"string-widgetname string-command string-parameter",
	"Allowes to set widget text using SPACE and NEWLINE in text"
);
//hzm gameupdate chrissstrahl - get player name
Event EV_Player_getName
(
	"getName",
	EV_SCRIPTONLY,
	"@sF",
	"retunedString float-filter-option",
	"Returns multiplayer name of player, filters 1=replace space,2=colorcodes,3=both"
);
//hzm gameupdate chrissstrahl - gets score
Event EV_Player_getScore
(
	"getScore",
	EV_SCRIPTONLY,
	"@f",
	"return-Integer",
	"Returns points/score of the player entity"
);
//hzm gameupdate chrissstrahl - gets deaths
Event EV_Player_getDeaths
(
	"getDeaths",
	EV_SCRIPTONLY,
	"@f",
	"return-Integer",
	"Returns number of deaths of the player entity"
);
//hzm gameupdate chrissstrahl - get player kills
Event EV_Player_getKills
(
	"getKills",
	EV_SCRIPTONLY,
	"@f",
	"return-Integer",
	"Returns number of kills by the player entity"
);
//hzm gameupdate chrissstrahl - adds to player score
Event EV_Player_addScore
(
	"addScore",
	EV_SCRIPTONLY,
	"f",
	"integer",
	"Adds given ammount of score to player"
);
//hzm gameupdate chrissstrahl - get time player was last injured
Event EV_Player_getLastDamaged
(
	"getLastDamaged",
	EV_SCRIPTONLY,
	"@f",
	"return-Float",
	"Returns time when player was last time hit"
);
//hzm gameupdate chrissstrahl - get name/color of players team
Event EV_Player_getTeamName
(
	"getTeamName",
	EV_SCRIPTONLY,
	"@s",
	"return-String",
	"Returns team color/name of player"
);
//hzm gameupdate chrissstrahl - get score of players team
Event EV_Player_getTeamScore
(
	"getTeamScore",
	EV_SCRIPTONLY,
	"@f",
	"return-Integer",
	"Returns team score of player"
);
//[b60011] chrissstrahl - Returns Int/Bool if player was pressing use or not
Event EV_Player_checkUse
(
	"checkUse",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player was pressing use or not"
);

//[b60011] chrissstrahl - Returns Int/Bool if player is in third person view or not
Event EV_Player_checkThirdperson
(
	"checkThirdperson",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is in third person view"
);

//[b60011] chrissstrahl - Returns Int/Bool if player is pressing fire button
Event EV_Player_checkFire
(
	"checkFire",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing fire button"
);

//[b60011] chrissstrahl - Returns Int/Bool if player is pressing alternative fire button
Event EV_Player_checkFirealt
(
	"checkFirealt",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing alternatvive fire button"
);

//[b60011] chrissstrahl - allowes a player entity to start a thread
Event EV_Player_RunThread
(
	"runthread",
	EV_DEFAULT,
	"s",
	"threadname",
	"Runs the specified thread. and sets this player as currententity"
);

//[b60011] chrissstrahl - get player viewangle
Event EV_Player_GetViewangles
(
	"getViewAngles",
	EV_DEFAULT,
	"@v",
	"vector-viewangle",
	"Returns the player current viewangles"
);
//[b607] chrissstrahl - get targeted Entity (entity player is aiming at)
Event EV_Player_GetTargetedEntity
(
	"getTargetedEntity",
	EV_DEFAULT,
	"@e",
	"targetedentity",
	"Returns the entity the player is currently targeting"
);
//[b60011] chrissstrahl - checks player has ger/eng langauge
Event EV_Player_HasLanguageGerman
(
	"hasLanguageGerman",
	EV_SCRIPTONLY,
	"@f",
	"bool-yes-or-no",
	"Check if Player has English Language of the game"
);
//[b60011] chrissstrahl - checks player has ger/eng langauge
Event EV_Player_HasLanguageEnglish
(
	"hasLanguageEnglish",
	EV_SCRIPTONLY,
	"@f",
	"bool-yes-or-no",
	"Check if Player has English Language of the game"
);
//[b60013] chrissstrahl - Returns Int/Bool if player is pressing jump button
Event EV_Player_checkCrouch
(
	"checkCrouch",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing Crouch button"
);

//[b60013] chrissstrahl - Returns Int/Bool if player is pressing jump button
Event EV_Player_checkJump
(
	"checkJump",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing jump button"
);

//[b60018] chrissstrahl - Returns Int/Bool if player is pressing any button
Event EV_Player_checkAnyButton
(
	"checkAnyButton",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing any button"
);
//[b60018] chrissstrahl - Returns Int/Bool if player is in chat, menu or console
Event EV_Player_checkMenu
(
	"checkMenu",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is in chat, menu or console"
);

//[b60013] chrissstrahl - Returns Int/Bool if player is pressing forward button
Event EV_Player_checkForward
(
	"checkForward",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing forward button"
);

//[b60013] chrissstrahl - Returns Int/Bool if player is pressing backward button
Event EV_Player_checkBackward
(
	"checkBackward",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing backward button"
);

//[b60013] chrissstrahl - Returns Int/Bool if player is pressing left button
Event EV_Player_checkLeft
(
	"checkLeft",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing left button"
);

//[b60013] chrissstrahl - Returns Int/Bool if player is pressing right button
Event EV_Player_checkRight
(
	"checkRight",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing right button"
);

//[b60013] chrissstrahl - Returns Int/Bool if player is pressing lean right button
Event EV_Player_checkLeanRight
(
	"checkLeanRight",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing lean right button"
);

//[b60013] chrissstrahl - Returns Int/Bool if player is pressing lean right button
Event EV_Player_checkLeanLeft
(
	"checkLeanLeft",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing lean left button"
);

//[b60013] chrissstrahl - Returns Int/Bool if player is pressing drop rune button
Event EV_Player_checkDropRune
(
	"checkDropRune",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing drop rune button"
);

//[b60013] chrissstrahl - Returns Int/Bool if player is pressing run button
Event EV_Player_checkRun
(
	"checkRun",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing run button"
);

//[b60013] chrissstrahl - Returns Int/Bool if player is pressing reload button
Event EV_Player_checkReload
(
	"checkReload",
	EV_DEFAULT,
	"@i",
	"return-int",
	"Returns Int/Bool if player is pressing reload button"
);
//[b60013] chrissstrahl - get offsets for player skins/models used in specialities and ctf, this might come in handy in coop
Event EV_Player_getBackpackAttachOffset
(
	"getBackpackAttachOffset",
	EV_DEFAULT,
	"@v",
	"return-vector",
	"Returns vector of player skin defined offset data for backpacks"
);
Event EV_Player_getBackpackAttachAngles
(
	"getBackpackAttachAngles",
	EV_DEFAULT,
	"@v",
	"return-vector",
	"Returns vector of player skin defined Angle data for backpacks"
);
Event EV_Player_getFlagAttachOffset
(
	"getFlagAttachOffset",
	EV_DEFAULT,
	"@v",
	"return-vector",
	"Returns vector of player skin defined offset data for CTF Flags"
);
Event EV_Player_getFlagAttachAngles
(
	"getFlagAttachAngles",
	EV_DEFAULT,
	"@v",
	"return-vector",
	"Returns vector of player skin defined Angle data for CTF Flags"
);
//[b60014] chrissstrahl - grab userfov
Event EV_Player_getUserFov
(
	"getUserFov",
	EV_DEFAULT,
	"@f",
	"return-float",
	"Returns player their userfov setting"
);
//[b60014] chrissstrahl - grab vector the player targeting end location
Event EV_Player_getViewtraceEndpos
(
	"getViewtraceEndpos",
	EV_DEFAULT,
	"@v",
	"return-vector",
	"Returns vector at wich players aim ends at"
);


//=========================================================[b60021]      
// called when a client has finished connecting, and is ready
// to be placed into the game.This will happen every level load.
// THIS HAPPENS AFTER -> Player::Player()
//
// -> extern "C" void G_ClientBegin( gentity_t *ent, const usercmd_t *cmd )
//================================================================
void UpgPlayer::upgPlayerBegin(gentity_t* ent)
{
	if (g_gametype->integer > GT_SINGLE_PLAYER)
	{
		//hzm gameupdate chrissstrahl - request current status of server/players, for debugging and overview reasons
		//[b607] chrissstrahl - only print this in coop - it spams only in regular multi
		if (game.coop_isActive) {
			gi.SendConsoleCommand("status\n");
		}
		//hzm gameupdate chrissstrahl - targetname all players by client id
		str setTargetname = va("player%d", ent - g_entities);
		ent->entity->SetTargetName(setTargetname.c_str());
		gi.Printf("%s entered the game and was targetnamed '$%s'\n", ent->client->pers.netname, setTargetname.c_str());
	}
}

//=========================================================[b60021]
// Name:        upgPlayerDoUseIgnore
// Class:       Player
//              
// Description: Check if this entity should be ignored on a use event
//				This is used to fix a issue when a script_object object
//				has a large bounding box, like a patch but it is not
//				set to SOLID_NOT, players don't colide with that boundingbox
//				they colide with the surfaces, how ever for Player::DoUse(EVENT)
//				Bounding Boxes are used, we want script_objects to only be
//				valid for that event, if they have a actual onUse thread set.
//				-
//				Otherwhise you could not use Holdable Items on coop_bugs
//				while being on the ship. There could ve other levels with
//				the same issue, how ever, this fixes it now
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool Player::upgPlayerDoUseIgnore(Entity *e)
{
	//[b60021] chrissstrahl - make sure that holdable items still work even if we are around script objects
	//unless that script object can actually really be used
	str sClassName = e->getClassname();
	if (!Q_stricmpn(sClassName.c_str(), "ScriptSlave", 11)) {
		ScriptSlave* ScriptObject = (ScriptSlave*)e;
		if (!ScriptObject->upgScriptSlaveGetUseLabel().length()) {
			return true;
		}
	}
	
	//I need to look into this in the future perhaps - I am not willing now to spend 2 more hours on it figuring all the spawnflags out and getting accsess to all the related classes
	// 
	//unsure if we should ignore touchfields, because of doors check it
	//if (!Q_stricmpn(sClassName.c_str(), "TouchField", 10)) {
		//return false;
	//}

	return false;
}

//=========================================================[b60017]
// Name:        upgPlayerCanTaunt
// Class:       Player
//              
// Description: Check if a player can actually taunt (again)
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool Player::upgPlayerCanTaunt()
{
	//[b60017] chrissstrahl - set a cooldown time for taunt - also don't play during cinematic
	ScriptVariable* entityData = entityVars.GetVariable("_mpTauntCool");
	if (level.cinematic || entityData != NULL && entityData->floatValue() + 3 > level.time) {
		return false;
	}
	//[b60017] chrissstrahl - update a cooldown time for taunt
	entityVars.SetVariable("_mpTauntCool", level.time);
	return true;
}

//=========================================================[b60021]
// Name:        upgPlayerReconnectEvent
// Class:       Player
//              
// Description: Used to make player reconnect with a delay
//              
// Parameters:  Event*
//              
// Returns:     void
//================================================================
void Player::upgPlayerReconnectEvent(Event *ev)
{
	gi.SendServerCommand(entnum, "stufftext reconnect");
	gi.Printf(va("Reconnect issued for: %s\n",client->pers.netname));
}

// //=========================================================[b60021]
// Name:        upgPlayerSetReconnect
// Class:       UpgPlayer
//              
// Description: set that player should be reconnecting
//              
// Parameters:  clientnum, doreconnect
//              
// Returns:     void
//================================================================
void UpgPlayer::upgPlayerSetReconnect(int clientNum,bool reconnect)
{
	upgPlayer.reconnectRequired[clientNum] = reconnect;
}

//=========================================================[b60021]
// Name:        upgPlayerGetReconnect
// Class:       UpgPlayer
//              
// Description: get if player should be reconnecting
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool UpgPlayer::upgPlayerGetReconnect(int clientNum)
{
	//Client is host, don't even atempt to reconnect
	if (clientNum == 0){
		if (dedicated->integer == 0 || g_gametype->integer != GT_MULTIPLAYER) {
			return false;
		}
	}

	return upgPlayer.reconnectRequired[clientNum];
}

//=========================================================[b60021]
// Name:        upgPlayerConnecting
// Class:       UpgPlayer
//              
// Description: Executed when the player is about to connect/connecting
//				Player does not exist yet, we can NOT accsess the player from here
//              
// Parameters:  firstconnect, isbot, name, clientnum
//              
// Returns:     void
//================================================================
void UpgPlayer::upgPlayerConnecting(bool firstTime,bool isBot, str netname,int clientnum)
{
	upgPlayer.upgPlayerSetReconnect(clientnum,false);

	if (firstTime) {
		if (!isBot) {
			//[b60014] chrissstrahl - we don't want to know when a bot connects - Inform all of the players that a player is about to connect
			multiplayerManager.HUDPrintAllClients(va("%s ^8is connecting!\n", netname.c_str()));
		}
	}
	else {
		if (!isBot && upgWorld.upgWorldGetPlayersReconnecting()) {
			upgPlayer.upgPlayerSetReconnect(clientnum, true);
		}
	}

	//--------------------------------------------------------------
	// GAMEUPGRADE [b6000x] & gamefix daggolin - pending server commands
	//--------------------------------------------------------------
	upgPlayerclearDelayedServerCommands(clientnum);
}

//=========================================================[b60014]         
// Description: Executed when the player is about to disconnect/leaving/left
// 
// -> extern "C" void G_ClientDisconnect( gentity_t *ent )
//================================================================
void UpgPlayer::upgPlayerDisconnecting(Player* player)
{

}

//=========================================================[b60014]
// Name:        upgPlayerLoadingSavegame
// Class:       Player
//              
// Description: Executed from Player::Archive( Archiver &arc ) when loading a savegame
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
void Player::upgPlayerLoadingSavegame()
{
	upgPlayer.scanHudActive = true;
	upgPlayer.scanDataSendLast = "?NoNe?";

	upgPlayerSetScanData(0, "");
	upgPlayerSetScanData(1, "");
	upgPlayerSetScanData(2, "");

	upgPlayer.targetedEntityLast = NULL;
	upgPlayer.timeEntered = 0.1f;
	cvar_t* cvar = gi.cvar_get("cl_maxpackets");
	upgPlayer.clMaxPackets = atoi((cvar ? cvar->string : "15"));
}

//===========================================================[b6xx]
// Name:        upgPlayerSkipCinematic
// Class:       Player
//              
// Description:  allow skipping of cinematics in mp and also allow opening the menu during cinematic
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool Player::upgPlayerSkipCinematic()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		return true;
	}

	//player presses long or repeatedly ESC
	if ((upgPlayerGetSkipCinematicTimeLast() + 0.25) > level.time && !game.cinematicSkipping) {
		upgPlayerSetSkipCinematicTimeLast();
		multiplayerManager.callVote(this, "skipcinematic", "");
		return false;
	}
	//player presses short ESC
	//player presses long ESC and a cinematic skip vote is active
	else {
		//check if player has voted, if not make him vote yes for skip, if a vote is active, then exit
		if ((upgPlayerGetSkipCinematicTimeLast() + 0.25) > level.time && game.cinematicSkipping)
		{
			if (!multiplayerManager._playerData[this->entnum]._voted)
			{
				multiplayerManager.vote(this, "y");
			}
			upgPlayerSetSkipCinematicTimeLast();
			return false;
		}

		//if ESC is pressed only shortly or if no skip is active, show/hide menu
		upgPlayerSetSkipCinematicTimeLast();
		upgPlayerDelayedServerCommand(this->entnum, "pushmenu ingame_multiplayer");
		return false;
	}
}


//=========================================================[b60014]
// Name:        mp_motd
// Class:       Player
//              
// Description: Reads mp_motd cvar and prints it to player hud
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
void Player::upgPlayerMessageOfTheDay(Event* ev)
{
	if (!multiplayerManager.inMultiplayer()) { return; }
	
	//on missionfailure, stop
	if (level.mission_failed == (qboolean)qtrue) {
		return;
	}

	//while in cinematic postpone event, post it again
	if (sv_cinematic->integer == 1) {
		Event* newEvent = new Event(EV_Player_upgPlayerMessageOfTheDay);
		PostEvent(newEvent,3.0f);
		return;
	}

	//get cvar, abbort if empty
	str sMessage;
	cvar_t* cvar = gi.cvar_get("mp_motd");
	if (!cvar) { return; }
	
	sMessage = cvar->string;
	if (!sMessage.length()) { return; }

	hudPrint(va("^5Message of the day:^8 %s\n", sMessage.c_str()));
}

//=========================================================[b60014]
// Name:        upgPlayerGetSkipCinematicTimeLast
// Class:       Player
//              
// Description: get skip cinematic time last 
//              
// Parameters:  void
//              
// Returns:     float
//================================================================
float Player::upgPlayerGetSkipCinematicTimeLast()
{
	return upgPlayer.lastTimeSkipCinematic;
}

//=========================================================[b60014]
// Name:        upgPlayerSetSkipCinematicTimeLast
// Class:       Player
//              
// Description: get skip cinematic time last 
//              
// Parameters:  void
//              
// Returns:     float
//================================================================
void Player::upgPlayerSetSkipCinematicTimeLast()
{
	upgPlayer.lastTimeSkipCinematic = level.time;
}

//=========================================================[b60014]
// Name:        upgPlayerGetScanData
// Class:       Player
//              
// Description: get scanData strings
//              
// Parameters:  short iNumber
//              
// Returns:     str
//================================================================
str Player::upgPlayerGetScanData(short iNumber)
{
	str sData = "";
	if (iNumber == 0)		{ sData = upgPlayer.scanData0; }
	else if (iNumber == 1)	{ sData = upgPlayer.scanData1; }
	else if (iNumber == 2)	{ sData = upgPlayer.scanData2; }
	else {
		gi.Error(ERR_DROP, "upgPlayerSetScanData Parameter 1 out of range (0-2)\n");
	}
	return sData;
}

//=========================================================[b60014]
// Name:        upgPlayerSetScanData
// Class:       Player
//              
// Description: set scanData strings
//              
// Parameters:  short iNumber, str sData
//              
// Returns:     void             
//================================================================
void Player::upgPlayerSetScanData(short iNumber,str sData)
{
	if (upgCoopInterface.playerHasCoop(this)) {
		if(iNumber == 0)		{ upgPlayer.scanData0 = sData; }
		else if (iNumber == 1)	{ upgPlayer.scanData1 = sData; }
		else if (iNumber == 2)	{ upgPlayer.scanData2 = sData; }
		else {
			gi.Error(ERR_DROP, "upgPlayerSetScanData Parameter 1 out of range (0-2)\n");
		}
	}
}


//=========================================================[b60014]
// Name:        upgPlayerHudAddScanning
// Class:       Player
//              
// Description: removes the scanning hud that appears when useing the tricorder
//              
// Parameters:  void
//              
// Returns:     void             
//================================================================
void Player::upgPlayerHudAddScanning()
{
	if (upgCoopInterface.playerHasCoop(this) && !upgPlayerGetHudScanningActive()) {
		upgPlayerSetHudScanningActive(true);
		gi.SendServerCommand(entnum, "stufftext \"ui_addhud coop_scan\"\n");
	}
}

//=========================================================[b60014]
// Name:        upgPlayerHudRemoveScanning
// Class:       Player
//              
// Description: removes the scanning hud that appears when useing the tricorder
//              
// Parameters:  void
//              
// Returns:     void             
//================================================================
void Player::upgPlayerHudRemoveScanning()
{
	if (upgCoopInterface.playerHasCoop(this) && upgPlayerGetHudScanningActive()) {
		upgPlayerSetHudScanningActive(false);
		gi.SendServerCommand(entnum, "stufftext \"ui_removehud coop_scan\"\n");
	}
}

//=========================================================[b60014]
// Name:        upgPlayerGetHudScanningActive
// Class:       Player
//              
// Description: returns if scanning hud for tricorder is active
//              
// Parameters:  void
//              
// Returns:     bool             
//================================================================
bool Player::upgPlayerGetHudScanningActive()
{
	return upgPlayer.scanHudActive;
}

//=========================================================[b60014]
// Name:        upgPlayerSetHudScanningActive
// Class:       Player
//              
// Description: sets if scanning hud for tricorder is active
//              
// Parameters:  void
//              
// Returns:     void             
//================================================================
void Player::upgPlayerSetHudScanningActive(bool bActive)
{
	upgPlayer.scanHudActive = bActive;
}

//=========================================================[b60014]
// Name:        upgPlayerSetLastScanData
// Class:       Player
//              
// Description: sets data string that was last aquired by scanning
//              
// Parameters:  void
//              
// Returns:     void             
//================================================================
void Player::upgPlayerSetLastScanData(str sData)
{
	upgPlayer.scanDataSendLast = sData;
}

//=========================================================[b60014]
// Name:        upgPlayerSetLastScanData
// Class:       Player
//              
// Description: sets data string that was last aquired by scanning
//              
// Parameters:  void
//              
// Returns:     void             
str Player::upgPlayerGetLastScanData()
{
	return upgPlayer.scanDataSendLast;
}

//=========================================================[b60014]
// Name:        upgPlayerIsScanning
// Class:       Player
//              
// Description: checks if player is scanning something or not
//              
// Parameters:  void
//              
// Returns:     bool             
//================================================================
bool Player::upgPlayerIsScanning()
{
	Weapon* currentWeapon = GetActiveWeapon(WEAPON_ANY);//WEAPON_ANY WEAPON_DUAL
	if (currentWeapon) {
		Equipment* eQuipMenT = (Equipment*)currentWeapon;
		return eQuipMenT->isScanning();
	}
	return false;
}

//=========================================================[b60014]
// Name:        upgPlayerSetTargetedEntity
// Class:       Player
//              
// Description: called from: void Player::SetTargetedEntity( EntityPtr entity )
//              
// Parameters:  void
//              
// Returns:     bool             
//================================================================
bool Player::upgPlayerSetTargetedEntity()
{
	if (multiplayerManager.inMultiplayer()) {
		Player* playerOther;
		for (int i = 0; i < maxclients->integer; i++) {
			playerOther = multiplayerManager.getPlayer(i);
			if (!playerOther || multiplayerManager.isPlayerSpectator(playerOther) || playerOther->upgPlayerIsBot()) { continue; }

			//exit, do not allow new player to end a scann, because this player is scanning
			if (playerOther != this && playerOther->upgPlayerIsScanning() && playerOther->_targetedEntity) {
				return true;
			}
		}
	}

	if (upgCoopInterface.playerHasCoop(this) && upgPlayer.scanHudActive) {
		Weapon* currentWeapon = GetActiveWeapon(WEAPON_ANY);//WEAPON_ANY WEAPON_DUAL
		if (currentWeapon) {
			Equipment* e;
			e = (Equipment*)currentWeapon;

			if (e->isScanning())
				return true;
		}
		upgPlayerHudRemoveScanning();
	}
	return false;
}

//=========================================================[b60014]
// Name:        upgPlayerGetLevelTimeEntered
// Class:       Player
//              
// Description: get level.time at which this player did enter the game
//              
// Parameters:  void
//              
// Returns:     float             
//================================================================
float Player::upgPlayerGetLevelTimeEntered()
{
	if (!multiplayerManager.inMultiplayer()) { return 0.0f; }
	return upgPlayer.timeEntered;
}

//=========================================================[b60014]
// Name:        upgPlayerSetLevelTimeEntered
// Class:       Player
//              
// Description: sets level.time at which this player did enter the game
//              
// Parameters:  void
//              
// Returns:     void             
//================================================================
void Player::upgPlayerSetLevelTimeEntered()
{
	upgPlayer.timeEntered = level.time;
}

//=========================================================[b60014]
// Name:        upgPlayerSetTargetedEntityLast
// Class:       Player
//              
// Description: get lastentity player did target
//              
// Parameters:  void
//              
// Returns:     Entity*             
//================================================================
Entity* Player::upgPlayerSetTargetedEntityLast()
{
	return upgPlayer.targetedEntityLast;
}

//=========================================================[b60014]
// Name:        upgPlayerSetTargetedEntityLast
// Class:       Player
//              
// Description: sets level.time at which this player did enter the game
//              
// Parameters:  Entity*
//              
// Returns:     void             
//================================================================
void Player::upgPlayerSetTargetedEntityLast(Entity * eLast)
{
	upgPlayer.timeEntered = level.time;
}

//=========================================================[b60014]
// Name:        upgPlayerIsHost
// Class:       Player
//              
// Description: Checks if player is host
//              
// Parameters:  void
//              
// Returns:     bool           
//================================================================
bool Player::upgPlayerIsHost() //[b60022] chrissstrahl - Cleaned up Player::upgPlayerIsHost() a little
{
#ifdef __linux__
	return false;
#endif

	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return true;
	}

	cvar_t* cl_running = gi.cvar_get("cl_running");
	if (dedicated->integer == 0 && entnum == 0 && (cl_running ? cl_running->integer : 0)) {
		return true;
	}
	return false;
}

//================================================================
// Name:        upgPlayerHasLanguageGerman
// Class:       Player
//              
// Description: check if player has german game version
//              
// Parameters:  Player *player
//              
// Returns:     bool             
//================================================================
bool Player::upgPlayerHasLanguageGerman()
{
	if (upgPlayerGetLanguage() == "Deu")
	{
		return true;
	}
	return false;
}

//========================================================[b60011]
// Name:        upgPlayerHasLanguageGerman
// Class:       Player
//              
// Description: returns if player has german language of game
//              
// Parameters:  Event* ev
//              
// Returns:     bool             
//================================================================
void Player::upgPlayerHasLanguageGerman(Event* ev)
{
	bool bLangMatch = false;
	if (upgPlayerHasLanguageGerman()) {
		bLangMatch = true;
	}
	ev->ReturnFloat((float)bLangMatch);
}

//========================================================[b60011]
// Name:        upgPlayerHasLanguageEnglish
// Class:       Player
//              
// Description: returns if player has english language of game
//              
// Parameters:  Event* ev
//              
// Returns:     void             
//================================================================
void Player::upgPlayerHasLanguageEnglish(Event* ev)
{
	bool bLangMatch = false;
	if (upgPlayerGetLanguage() == "Eng") {
		bLangMatch = true;
	}
	ev->ReturnFloat((float)bLangMatch);
}

//========================================================[b60011]
// Name:        upgPlayerHasLanguageGerman
// Class:       Player
//              
// Description: returns player language string [Deu/Eng/?]
//              
// Parameters:  Event* ev
//              
// Returns:     void             
//================================================================
void Player::upgPlayerGetLanguageEvent(Event* ev)
{
	ev->ReturnString(upgPlayerGetLanguage());
}

//========================================================[b60011]
// Name:        upgPlayerHasLanguageGerman
// Class:       Player
//              
// Description: returns player language string [Deu/Eng/?]
//              
// Parameters:  void
//              
// Returns:     str             
//================================================================
str Player::upgPlayerGetLanguage()
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	if (g_gametype->integer == GT_SINGLE_PLAYER ) { //[b60022] chrissstrahl - Fixed bad check
		//[b60022] chrissstrahl - updated to use the cvar
		return local_language->string;
	}
	return upgPlayer.language;
}

//========================================================[b60011]
// Name:        upgPlayerSetLanguage
// Class:       Player
//              
// Description: sets player language string
//              
// Parameters:  str String
//              
// Returns:     void           
//================================================================
void Player::upgPlayerSetLanguage(str sLang)
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	if (multiplayerManager.inMultiplayer()) {
		if (sLang.length() < 1 || sLang.length() > 3) {
			upgPlayer.language = "Eng";
			gi.Printf(va("setLanguage(%s) - Bad string size for client %s\n", sLang.c_str(), entnum));
			return;
		}
		if (sLang != "Eng" && sLang != "Deu") {
			upgPlayer.language = "Eng";
			gi.Printf(va("setLanguage(%s) - Unknown Language for client %s\n", sLang.c_str(), entnum));
			return;
		}
		if (sLang != "Deu") {
			upgPlayer.language = "Eng";
			return;
		}
		upgPlayer.language = sLang;
	}
}

//=========================================================[b60014]
// Name:        upgPlayerDeathTimeUpdate
// Class:       Player
//              
// Description: updates deathtime, used to store server time at wich the player died last
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
void Player::upgPlayerDeathTimeUpdate()
{
	//used to store server time at wich the player died last
	time_t result = time(NULL);
	localtime(&result);
	upgPlayer.deathTime = (int)result;
}

//=========================================================[b60014]
// Name:        upgPlayerDeathTimeSet
// Class:       Player
//              
// Description: resest deathtime, used to store server time at wich the player died last
//              
// Parameters:  int
//              
// Returns:     void
//================================================================
void Player::upgPlayerDeathTimeSet(int iTime)
{
	upgPlayer.deathTime = iTime;
}

//=========================================================[b60014]
// Name:        upgPlayerDeathTime
// Class:       Player
//              
// Description: returns at wich server time the player died last
//              
// Parameters:  void
//              
// Returns:     int
//================================================================
int Player::upgPlayerDeathTime()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer()) {
		return 1;
	}
	return upgPlayer.deathTime;
}

//=========================================================[b60014]
// Name:        upgPlayerSetup
// Class:       Player
//              
// Description: Handles Player Setup stuff, like language and netsetting
//				Always executed from Player::Player()
//              
// Parameters:  void
//              
// Returns:     void
//================================================================
void UpgPlayer::upgPlayerSetup(Player* player)
{
	//[b607] daggolin - Restore bot state on player object
	gentity_t* ent = player->edict; if (!ent) { return; }
	if (level.spawn_bot) { player->edict->svflags |= SVF_BOT; }

	player->upgPlayerSetLevelTimeEntered();
	player->upgCircleMenuReset();

	//tell player to give us his cl_maxpackets and language
	if (player->upgPlayerIsHost()) {
		player->upgPlayerSetLanguage(local_language->string); //[b60022] chrissstrahl - updated to use the cvar

		cvar_t* cvar2 = gi.cvar_get("cl_maxpackets");
		str sCvar2 = (cvar2 ? cvar2->string : "0");
		if (atoi(sCvar2) < 60) {
			sCvar2 = "60";
			gi.cvar_set("cl_maxpackets",va("%s", sCvar2.c_str()));
		}
		player->upgPlayer.clMaxPackets = atoi(sCvar2);player->upgCircleMenuReset();
	}
	else {
		//[b60011] chrissstrahl - make sure we do not handle bots
		if (multiplayerManager.inMultiplayer() && player->upgPlayerIsBot()) {
			player->upgPlayerSetLanguage(local_language->string); //[b60022] chrissstrahl - updated to use the cvar

			cvar_t* cvar2 = gi.cvar_get("cl_maxpackets");
			int iCvar2 = (cvar2 ? cvar2->integer : 0);
			if (iCvar2 < 60) {
				gi.cvar_set("cl_maxpackets", "60");
			}

			cvar_t* cvar3 = gi.cvar_get("cl_packetdup");
			int iCvar3 = (cvar3 ? cvar3->integer : 0);
			if (iCvar3 > 0) {
				gi.cvar_set("cl_packetdup", "0");
			}
			coop_classSet(player, "HeavyWeapon");
			return;
		}

		upgPlayerDelayedServerCommand(player->entnum, "vstr cl_maxpackets;vstr local_language");
	}

	if (g_gametype->integer == GT_MULTIPLAYER)
	{
		/*
		* [b60025] chrissstrahl - we rather like to reconnect players upon mapchange
		* that way we can killserver and make it load the level and resources fresh
		* while the clients sit in the reconnect menu and wait for the game to reconnect
		* after a few sec. This should also fix issues with host of a listen server
		* 
		//[b60021] chrissstrahl - reconnect player if required
		if (upgPlayer.upgPlayerGetReconnect(player->entnum)){
			if(player->upgPlayerHasLanguageGerman()){
				multiplayerManager.centerPrint(player->entnum, UPG_RECONNECTING_YOU_MODEL_FIX_DEU, CENTERPRINT_IMPORTANCE_CRITICAL);
			}
			else {
				multiplayerManager.centerPrint(player->entnum, UPG_RECONNECTING_YOU_MODEL_FIX_ENG, CENTERPRINT_IMPORTANCE_CRITICAL);
			}
		
			Event* reconnectEV = new Event(EV_Player_reconnect);
			player->PostEvent(reconnectEV, 3.0f);
			return;
		}
		*/

		//print message of the day
		Event* newEvent = new Event(EV_Player_upgPlayerMessageOfTheDay);
		player->PostEvent(newEvent, 12.0f);
	}

}

//=========================================================[b60014]
// Name:        upgPlayerKilled
// Class:       Player
//              
// Description: executed when player dies
//              
// Parameters:  const Entity* attacker, const Entity* inflictor, const int meansOfDeath
//              
// Returns:     bool
//================================================================
bool Player::upgPlayerKilled(const Entity* attacker, const Entity* inflictor, const int meansOfDeath)
{
	//set last killed time to current time
	upgPlayerDeathTimeUpdate();

	//there is nothing we do in here that would need to stop other functions, such as coop
	//if(killMessage || killHandling ){ return true; }

	return false;
}

//=========================================================[b60014]
// Name:        upgPlayerSay
// Class:       Player
//              
// Description: Handles Player Say stuff, filters and script execution, ect...
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool Player::upgPlayerSay(str sayString)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER) { return false; }

	//filter bad chars
	for (int i = 0; i < sayString.length(); i++) {
		if (sayString[i] == '%') { sayString[i] = '.'; }
	}

	//detect player cl_maxpackets and filter this text
	if (upgPlayer.clMaxPackets < 15) {
		int iClMaxPack = atoi(sayString.c_str());
		if (iClMaxPack >= 15) {
			if (iClMaxPack < 60) {
				iClMaxPack = 60;
				upgPlayerDelayedServerCommand(entnum, "set cl_maxpackets 60");
			}
			upgPlayer.clMaxPackets = iClMaxPack;
			return true;
		}
	}

	//detect player language
	if		(Q_stricmpn(sayString.c_str(), "eng", 3) == 0) { upgPlayerSetLanguage("Eng");return true; }
	else if	(Q_stricmpn(sayString.c_str(), "deu", 3) == 0) { upgPlayerSetLanguage("Deu");return true; }

	//SPAM FILTER - this is our sv_floodprotect replacement, since flood protect also blocks multiplayer specific commands which we are in need of to work
	if (upgPlayer.chatTimeLimit < level.time) { upgPlayer.chatTimeLimit = level.time; }
	upgPlayer.chatTimeLimit++;
	if (upgPlayer.chatTimeLimit > (level.time + 3)) {
		//display info that the player was spamming
		if ((upgPlayer.chatTimeLastSpamInfo + 3.0f) < level.time) {
			upgPlayer.chatTimeLastSpamInfo = level.time;
			if (upgPlayerHasLanguageGerman()) { hudPrint("^3Sie chatten zu schnell, Nachricht blockiert durch ^5Spamschutz!\n"); }
			else { hudPrint("^3You chat to fast, message blocked by ^5Spamprotection!\n"); }
		}
		return true;
	}


	return false;
}

//================================================================
// Name:        upgPlayerSaySpamfilterCountdown
// Class:       Player
//              
// Description: counts down after player has spammed until player is allowed to chat again
//
// Parameters:  void 
//              
// Returns:     void             
//================================================================
void Player::upgPlayerSaySpamfilterCountdown()
{
	if (upgPlayer.chatTimeLimit < level.time) { return; }

	if ((upgPlayer.chatTimeLimit + -1) < level.time) { upgPlayer.chatTimeLimit = level.time; }
	else { upgPlayer.chatTimeLimit--; }
}

//========================================================[b60014]
// Name:        upgPlayerClientThink
// Class:       Player
//              
// Description: ClientThink, used to update/check stuff each frame
//
// Parameters:  void 
//              
// Returns:     void             
//================================================================
void Player::upgPlayerClientThink()
{
	//this runs once per secound
	if ((upgPlayer.clientThinkLastTime + 1) < level.time) {
		upgPlayerSaySpamfilterCountdown();
		upgPlayer.clientThinkLastTime = level.time;
	}
}

//========================================================[b60014]
// Name:        upgPlayerIsBot
// Class:       Player
//              
// Description: Checks if player is a bot
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool Player::upgPlayerIsBot()
{
	if (edict->svflags & SVF_BOT) {
		return true;
	}
	return false;
}

//========================================================[b60014]
// Name:        upgPlayerGetTargetedEntity
// Class:       Player
//              
// Description: this has been implemented for script use
//              
// Parameters:  event* ev
//              
// Returns:     void
//================================================================
void Player::upgPlayerGetTargetedEntity(Event* ev)
{
	Entity* target;
	target = GetTargetedEntity();
	if (!target) {
		target = world;
	}
	ev->ReturnEntity(target);
}

//==========================================================[b607]
// Name:        ugpPlayerInThirdPerson
// Class:       -
//              
// Description: returns if player is in third person or not
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
bool Player::ugpPlayerInThirdPerson()
{
	return _isThirdPerson;
}

//================================================================
// Name:		Player::upgPlayerDisableUseWeapon
// Class:		-
//
// Description:	Can disable the use of a new weapon
//
// Parameters:	bool that sets the disableUseWeapon
//
// Returns:		void
//================================================================
void Player::upgPlayerDisableUseWeapon(bool bDisable)
{
	if (bDisable){ _disableUseWeapon = true; }
	else{ _disableUseWeapon = false; }
}

//[b60014] chrissstrahl - grab player userfov, basically the fov the player has set in menu
void Player::upgPlayerGetViewtraceEndpos(Event* ev)
{
	ev->ReturnVector(upgPlayerGetViewTraceEndVector());
}

//================================================================
// Name:        upgPlayerGetViewTraceEndVector
// Class:       -
//              
// Description: Gets end vector location of player view/target
//              
// Parameters:  void
//              
// Returns:     Vector   
//================================================================
Vector Player::upgPlayerGetViewTraceEndVector(void)
{
	trace_t viewTrace;
	memset(&viewTrace, 0, sizeof(trace_t));
	GetViewTrace(viewTrace, MASK_SHOT | CONTENTS_TARGETABLE);
	if (!viewTrace.ent) { return origin; }
	return viewTrace.endpos;
}

//========================================================[b60014]
// Name:        upgPlayerGetUserFov
// Class:       -
//              
// Description: grab player userfov, basically the fov the player has set in menu
//              
// Parameters:  void
//              
// Returns:     void   
//================================================================
void Player::upgPlayerGetUserFov(Event* ev)
{
	float fov = (float)atof(Info_ValueForKey(client->pers.userinfo, "userFov"));
	if (fov < 1.0f) {
		str sCvarValue = "";
		cvar_t* cvar = gi.cvar_get("sv_defaultFov");
		if (cvar) { sCvarValue = cvar->string; }
		fov = atof(sCvarValue.c_str());
	}
	else if (fov > 160.0f) {
		fov = 160.0f;
	}
	ev->ReturnFloat(fov);
}

//[b60013] chrissstrahl - get offsets for player skins/models used in specialities and ctf, this might come in handy in coop
void Player::upgPlayerGetBackpackAttachOffset(Event* ev) { ev->ReturnVector(_backpackAttachOffset); }
void Player::upgPlayerGetBackpackAttachAngles(Event* ev) { ev->ReturnVector(_backpackAttachAngles); }
void Player::upgPlayerGetFlagAttachOffset(Event* ev) { ev->ReturnVector(_flagAttachOffset); }
void Player::upgPlayerGetFlagAttachAngles(Event* ev) { ev->ReturnVector(_flagAttachAngles); }

//[b60013] chrissstrahl - checks if player is a specific button
void Player::upgPlayerCheckCrouch(Event* ev) { ev->ReturnFloat((int)GetCrouch()); }
void Player::upgPlayerCheckJump(Event* ev){ qboolean bJump = last_ucmd.upmove > 0; ev->ReturnFloat((int)bJump); }
void Player::upgPlayerCheckForward(Event* ev){ qboolean bJump = last_ucmd.forwardmove > 0; ev->ReturnFloat((int)bJump); }
void Player::upgPlayerCheckBackward(Event* ev){ qboolean bJump = last_ucmd.forwardmove < 0; ev->ReturnFloat((int)bJump); }
void Player::upgPlayerCheckLeft(Event* ev){ qboolean bJump = last_ucmd.rightmove < 0; ev->ReturnFloat((int)bJump); }
void Player::upgPlayerCheckRight(Event* ev){ qboolean bJump = last_ucmd.rightmove > 0; ev->ReturnFloat((int)bJump); }
void Player::upgPlayerCheckLeanLeft(Event* ev){ qboolean bJump = last_ucmd.lean > 0; ev->ReturnFloat((int)bJump); }
void Player::upgPlayerCheckLeanRight(Event* ev){ qboolean bJump = last_ucmd.lean < 0; ev->ReturnFloat((int)bJump); }
void Player::upgPlayerCheckDropRune(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_DROP_RUNE) != 0); }
void Player::upgPlayerCheckRun(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_RUN) != 0); }
void Player::upgPlayerCheckReload(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_RELOAD) != 0); }

//[b60018] chrissstrahl - checks if player is pressing a specific button
void Player::upgPlayerCheckAnyButton(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_ANY) != 0); }
void Player::upgPlayerCheckMenu(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_TALK) != 0); }

//[b60011] chrissstrahl - checks if player is pressing a specific button
void Player::upgPlayerCheckFire(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_ATTACKLEFT) != 0); }
void Player::upgPlayerCheckFirealt(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_ATTACKRIGHT) != 0); }
void Player::upgPlayerCheckThirdperson(Event* ev) { ev->ReturnFloat((int)_isThirdPerson); }
void Player::upgPlayerCheckUsePressing(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_USE) != 0); }

//[b60012] chrissstrahl - cancel player modulating event - if player gets hurt or so
void Player::upgPlayerCancelPuzzle()
{
	Entity* puzzle;
	puzzle = G_FindClass(NULL, "puzzle_object");
	while (puzzle) {
		PuzzleObject* puzObj = (PuzzleObject*)puzzle;
		if (puzObj->upgGetLastActivatingEntity() == this) {
			puzObj->upgCancelPlayer(this);
			return;
		}
		puzzle = G_FindClass(puzzle, "puzzle_object");
	}
}

//[b60011] chrissstrahl
void Player::upgPlayerRunThread(Event* ev)
{
	str thread_name = "";
	thread_name = ev->GetString(1);
	upgPlayerRunThread(thread_name);
}

//[b60011] chrissstrahl
CThread* Player::upgPlayerRunThread(const str& thread_name)
{
	if (thread_name.length() <= 0)
		return NULL;

	return ExecuteThread(thread_name, true, this);
}

//[b60011] chrissstrahl - allow to get Holdable Item Name from outside class
str Player::upgPlayerGetHoldableName(void)
{
	if (_holdableItem) {
		return _holdableItem->getName();
	}
	return "";
}

//[b60011] chrissstrahl - allow to get Holdable Item Model from outside class
str Player::upgPlayerGetHoldableModel(void)
{
	if (_holdableItem) {
		return _holdableItem->model;
	}
	return "";
}

//[b60011] chrissstrahl - allow to get Rune Item from outside class
Rune* Player::upgPlayerGetRuneItem(void)
{
	if (_rune) {
		return _rune;
	}
	return NULL;
}

//[b60011] chrissstrahl - allow to get Rune Model from outside class
str Player::upgPlayerGetRuneModel(void)
{
	if (_rune) {
		return _rune->model;
	}
	return "";
}

//[b60011] chrissstrahl - allow to get Rune Name from outside class
str Player::upgPlayerGetRuneName(void)
{
	if (_rune) {
		return _rune->getName();
	}
	return "";
}

//[b60011] chrissstrahl - allow to get Rune Item from outside class
Powerup* Player::upgPlayerGetPowerupItem(void)
{
	if (_powerup) {
		return _powerup;
	}
	return NULL;
}

//[b60011] chrissstrahl - allow to get Rune Model from outside class
str Player::upgPlayerGetPowerupModel(void)
{
	if (_powerup) {
		return _powerup->model;
	}
	return "";
}

//[b60011] chrissstrahl - allow to get Rune Name from outside class
str Player::upgPlayerGetPowerupName(void)
{
	if (_powerup) {
		return _powerup->getName();
	}
	return "";
}

//hzm gameupdate chrissstrahl - add new commands for script use
void Player::upgPlayerGetScore(Event* ev)
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	if (multiplayerManager.inMultiplayer()) {
		ev->ReturnFloat(multiplayerManager.getPoints(this));
	}
	else {
		ev->ReturnFloat(0.0f);
	}
}

//hzm gameupdate chrissstrahl - add new commands for script use
void Player::upgPlayerAddScore(Event* ev)
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	if (multiplayerManager.inMultiplayer()) {
		int iAdd = ev->GetInteger(1);

		multiplayerManager.addPoints(entnum, iAdd);
		str s;
		s = multiplayerManager._playerData[entnum]._name;
		gi.Printf("Info: Level-Script adding Points(%d) to Player[%d]: %s\n", iAdd, entnum, s.c_str());
	}
}

void Player::upgPlayerGetDeaths(Event* ev)
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	if (multiplayerManager.inMultiplayer()) {
		ev->ReturnFloat(multiplayerManager.upgPlayerGetDeaths(this));
	}
	else {
		ev->ReturnFloat(0.0f);
	}
}

//[b60011] chrissstrahl - get player viewangle
void Player::upgPlayerGetViewanglesEvent(Event* ev)
{
	Vector vAngle = Vector(0, 0, 0);
	GetPlayerView(NULL, &vAngle);
	ev->ReturnVector(vAngle);
}

//Allow individual killthread to be set by script for each player
void Player::upgPlayerSetKillThread(Event* ev)
{
	entityVars.SetVariable("_killThread",ev->GetString(1));
}

//[GAMEUPGRADE][b60014] chrissstrahl - Execute individual killthread for this player
void Player::upgPlayerRunKillThread()
{
	ScriptVariable* entityData = NULL;
	entityData = entityVars.GetVariable("_killThread");
	str sThread = (entityData) ? entityData->stringValue() : "";
	if (sThread.length()){ ExecuteThread(sThread.c_str(), true, this); }
}

//hzm gameupdate chrissstrahl - return time at which player was last hurt
float Player::upgPlayerGetLastDamageTime(void)
{
	float fValue1;
	float fValue2;
	if (_lastDamagedTimeFront > _lastDamagedTimeBack)
	{
		fValue1 = _lastDamagedTimeFront;
	}
	else {
		fValue1 = _lastDamagedTimeBack;
	}

	if (_lastDamagedTimeLeft > _lastDamagedTimeRight)
	{
		fValue2 = _lastDamagedTimeLeft;
	}
	else {
		fValue2 = _lastDamagedTimeRight;
	}

	if (fValue1 > fValue2)
	{
		return fValue1;
	}
	else {
		return fValue2;
	}
}

//hzm gameupdate daggolin - new commands
extern	type_t	type_void;
extern	type_t	type_string;
extern	type_t	type_float;
extern	type_t	type_vector;
extern	type_t	type_entity;
extern	type_t	type_function;

void Player::upgPlayerGetScriptVariablesCommand(Event* ev)
{
	str		 type = ev->GetString(1);
	def_t* def;
	type_t* searched;

	if (!type.icmp("void")) searched = &type_void;
	else if (!type.icmp("string")) searched = &type_string;
	else if (!type.icmp("float")) searched = &type_float;
	else if (!type.icmp("vector")) searched = &type_vector;
	else if (!type.icmp("entity")) searched = &type_entity;
	else if (!type.icmp("function")) searched = &type_function;
	else
	{
		multiplayerManager.HUDPrint(entnum, va("%s: ", type.c_str()));
		multiplayerManager.HUDPrint(entnum, program.getVariableValueAsString(type.c_str()));
		multiplayerManager.HUDPrint(entnum, "\n");
		return;
	}

	for (def = program.def_head.next; def; def = def->next)
	{
		if (def->type == searched && def->name.icmp("IMMEDIATE") && def->name.icmp("<RESULT>"))
		{
			if (def->type == &type_string) multiplayerManager.HUDPrint(entnum, va("%s: %s\n", def->name.c_str(), program.getString(def->ofs)));
			else if (def->type == &type_float) multiplayerManager.HUDPrint(entnum, va("%s: %f\n", def->name.c_str(), program.getFloat(def->ofs)));
			else if (def->type == &type_vector) multiplayerManager.HUDPrint(entnum, va("%s: %f, %f, %f\n", def->name.c_str(), program.getVector(def->ofs)[0], program.getVector(def->ofs)[1], program.getVector(def->ofs)[2]));
			else								  multiplayerManager.HUDPrint(entnum, va("%s: *** OUTPUT NOT SUPPORTED ***\n", def->name.c_str()));
		}
	}
}

//hzm gamefix - new function to handle enviromental influences (like drowning)
void Player::upgPlayerWorldEffects(void)
{
	if (deadflag == DEAD_DEAD || multiplayerManager.isPlayerSpectator(this))
	{
		// if we are dead, no world effects
		return;
	}

	// check for drowning
	if (waterlevel == 3 && watertype & CONTENTS_WATER)
	{
		// if out of air, start drowning
		if ((air_finished < level.time) && !(flags & FL_GODMODE))
		{
			// drown!
			if ((next_drown_time < level.time) && (health > 0))
			{
				int i, bubbleAmount;

				next_drown_time = level.time + 1.0f;

				// take more damage the longer underwater
				drown_damage += 2.0f;
				if (drown_damage > 15.0f)
				{
					drown_damage = 15.0f;
				}

				// Damage-Multiplier
				float damageMultiplier = 1;
				if (!multiplayerManager.inMultiplayer() || upgCoopInterface.isCoopActive())
				{
					GameplayManager* gpm;
					int skillLevel;

					if (g_gametype->integer == GT_SINGLE_PLAYER)
					{
						skillLevel = getSkill();
					}
					else
					{
						skillLevel = skill->integer;
					}

					gpm = GameplayManager::getTheGameplayManager();

					if (gpm->hasObject("SkillLevel-PlayerDamage"))
					{
						if (skillLevel == 0)
							damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "Easy");
						else if (skillLevel == 1)
							damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "Normal");
						else if (skillLevel == 2)
							damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "Hard");
						else
							damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "VeryHard");
					}
				}

				damageByWater += (drown_damage * damageMultiplier);
				Damage(world, world, drown_damage, origin, vec_zero, vec_zero, 0, DAMAGE_NO_ARMOR, MOD_DROWN);

				//hzm gameupdate - player leaking air...
				/*
				if ( drown_damage < 6 ) bubbleAmount = (random()*2)+1;
				else					bubbleAmount = (random()*1)+2;
				*/

				bubbleAmount = 1;
				if (health <= 75)	bubbleAmount = 2;
				if (health <= 50)	bubbleAmount = 3;
				if (health <= 25)	bubbleAmount = 4;
				if (health <= 0)	bubbleAmount = 5;

				SpawnSound("sound/player/player_gulp.wav", origin, 1.4f, 1.0f);

				for (i = 0; i < bubbleAmount; i++)
				{ // Spawn some bubbles...
					Entity* ent;
					vec3_t		dropOrigin;

					VectorCopy(client->ps.origin, dropOrigin);

					//					dropOrigin[2] += client->ps.viewheight;
					if (this->maxs[2] == 49) dropOrigin[2] += 67;
					else					   dropOrigin[2] += 100;

					ent = SpawnEffect("waterdrop1.spr", dropOrigin, angles, 0.0f);
					ent->SetTargetName("breathBubble");
					ent->bubbleMaxLife = level.time + 10.0f; // Remove after 10s, no matter where it is...
					ent->bubbleNextSpeedIncrease = level.time + .5f;
					ent->bubbleTotalSpeedIncrease = 0;

					ent->setScale((random() * 0.1) + 0.1);

					ent->velocity[0] = (((int)(random() * 2) == 1) ? 1 : -1) * random() * 0.25;
					ent->velocity[1] = (((int)(random() * 2) == 1) ? 1 : -1) * random() * 0.25;
					ent->velocity[2] = (random() * 0.5) + 1;

					VectorScale(ent->velocity, 25, ent->velocity);

					ent->movetype = MOVETYPE_FLY;
				}
			}
		}
	}
	else
	{
		//hzm gameupdate daggolin - air in dm/coop/sp
		if (upgCoopInterface.isCoopActive()) air_finished = level.time + 15.0f;
		else					  air_finished = level.time + 10.0f;
		drown_damage = 2.0f;

		//hzm gameupdate chrissstrahl - take the Damage-Multiplier in account, when restoring health
		float damageMultiplier = 1;
		if (!multiplayerManager.inMultiplayer() || upgCoopInterface.isCoopActive())
		{
			GameplayManager* gpm;
			int skillLevel;

			if (g_gametype->integer == GT_SINGLE_PLAYER)
			{
				skillLevel = getSkill();
			}
			else
			{
				skillLevel = skill->integer;
			}

			gpm = GameplayManager::getTheGameplayManager();

			if (gpm->hasObject("SkillLevel-PlayerDamage"))
			{
				if (skillLevel == 0)
					damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "Easy");
				else if (skillLevel == 1)
					damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "Normal");
				else if (skillLevel == 2)
					damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "Hard");
				else
					damageMultiplier = gpm->getFloatValue("SkillLevel-PlayerDamage", "VeryHard");
			}
		}

		// Restore the damage caused by water...
		if (damageByWater > 0 && getHealth() > 0)
		{
			if (getHealth() < max_health)
			{
				Event* e = new Event(EV_AddHealthOverTime);
				e->AddFloat(damageByWater * damageMultiplier);
				e->AddFloat(3);
				PostEvent(e, level.frametime);
			}
			//hzm gamefix chrissstrahl - this would not work on a german server, so leave it be entirly, until we find a way to work arround
			//SpawnSound("loc/Eng/sound/dialog/combat/munro_jump3.mp3", origin, 1.4f, 1.0f);//localisazion thingy here
		}
		damageByWater = 0;
	}
}

void Player::upgPlayerGetKills(Event* ev)
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	if (multiplayerManager.inMultiplayer()) {
		ev->ReturnFloat(multiplayerManager.upgPlayerGetKills(this));
	}
	else {
		ev->ReturnFloat(0.0f);
	}
}

void Player::upgPlayerGetLastDamaged(Event* ev)
{
	ev->ReturnFloat(upgPlayerGetLastDamageTime());
}

void Player::upgPlayerGetTeamName(Event* ev)
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	if (multiplayerManager.inMultiplayer()) {
		Team* team;
		team = multiplayerManager.getPlayersTeam(this);
		if (team == NULL) {
			ev->ReturnString("None");
			return;
		}
		ev->ReturnString(team->getName());
	}
	else {
		ev->ReturnString("None");
	}
}

void Player::upgPlayerGetTeamScore(Event* ev)
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	if (multiplayerManager.inMultiplayer()) {
		Team* team;
		team = multiplayerManager.getPlayersTeam(this);
		if (team == NULL) {
			//[b60012] chrissstrahl - return player score if not in a team
			ev->ReturnFloat(multiplayerManager.getPoints(this));
		}
		else {
			ev->ReturnFloat(multiplayerManager.getTeamPoints(this));
		}
	}
	else {
		ev->ReturnFloat(0.0f);
	}
}

//hzm gameupdate chrissstrahl - get player name
void Player::upgPlayerGetNameEvent(Event* ev)
{
	//[b60019] chrissstrahl upgraded function
	str playerName = "";

	//[b60014] chrissstrahl - return also player name when not in multiplayer instead of crashing
	if (!multiplayerManager.inMultiplayer()) {
		cvar_t* cvar = gi.cvar_get("name");
		if (cvar) {
			playerName = cvar->string;
		}
	}
	//multiplayer
	else {
		playerName = multiplayerManager._playerData[entnum]._name;
	}

	//filter 
	str new_playerName = "";
	str sReplace = "_";
	if (ev->NumArgs() > 0 && ev->GetInteger(1) > 0) {
		int filterOption = ev->GetInteger(1);

		//filter space
		if (filterOption == 1 || filterOption == 3) {
			for (int i = 0; i < playerName.length(); i++) {
				if (playerName[i] == ' ') {
					new_playerName += sReplace;
				}
				else {
					new_playerName += playerName[i];
				}
			}
			playerName = new_playerName;
			new_playerName = "";
		}

		//filter color codes
		if (filterOption == 2 || filterOption == 3) {
			for (int i = 0; i < playerName.length(); i++) {
				if (playerName[i] == '^' && playerName.length() > (i + 1) && IsNumeric(str(playerName[i + 1]))) {
					i++;
					continue;
				}
				else {
					new_playerName += playerName[i];
				}
			}
			playerName = new_playerName;
		}
	}
	else {
		new_playerName = playerName;
	}
	ev->ReturnString(new_playerName.c_str());
}

//[b60011] chrissstrahl - add ability to set a proper widgetCommand that contains spaces
//widgetname,commandparameter,parameter,parameter,parameter,parameter,parameter,parameter,parameter,parameter,parameter
void Player::upgPlayerWidgetCommandEvent(Event* ev)
{
	//make sure we have at least widgetname and a commandparameter
	if (ev->NumArgs() < 2) {
		return;
	}

	str sData;
	str sParameters = ev->GetString(2);	//widgetcommandparameter

	if (ev->NumArgs() > 2) {
		sParameters += " "; //spacer
		str sTemp = ev->GetString(3);
		sParameters += sTemp;
	}
	upgPlayerWidgetCommand(ev->GetString(1), sParameters);
}

//[b60011] chrissstrahl - add ability to set a proper widgetCommand that contains spaces
void Player::upgPlayerWidgetCommand(str sWidget, str sParameters)
{
	//SPECIALS: ~=NEWLINE ^=SPACER #=NEWLINE
	//str sTemp;
	if (upgStrings.contains(sParameters.c_str(), "labeltext")) {
		sParameters = upgStrings.getReplacedForLabeltext(sParameters);
	}
	str sData = "stufftext \"globalwidgetcommand ";
	sData += sWidget;
	sData += " ";
	sData += sParameters; //[b60014] chrissstrahl - fixed widgetcommand not converting text
	sData += "\"\n";
	gi.SendServerCommand(edict - g_entities, sData.c_str());
	//gi.Printf(va("%s \n",sData.c_str()));
}

//hzm gameupdate chrissstrahl [b60011]  - sets specific camera on player
void Player::upgPlayerSetCameraEvent(Event* ev)
{
	Entity* camera;
	float switchTime = 0;

	camera = ev->GetEntity(1, true);
	if (ev->NumArgs() > 1) {
		switchTime = ev->GetFloat(2);
	}

	//reset camera
	if (!camera) {
		SetCamera(NULL, switchTime);
		//clear current camera so it can be restored on savegame
		if (g_gametype->integer == GT_SINGLE_PLAYER) {
			upgGame.setCameraCurrent(NULL);
		}
		return;
	}

	if (!camera->isSubclassOf(Camera)) {
		gi.Printf(va("setCamera::Entity $%s is of class %s needs to be Camera\n", camera->targetname.c_str(), camera->getClassname()));
		return;
	}

	//set current camera so it can be restored on savegame
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		upgGame.setCameraCurrent(camera);
	}
	SetCamera((Camera*)camera, switchTime);
}

//================================================================
// Name:        upgPlayerDelayedServerCommand
// Class:       -
//              
// Description: This adds a serverCommand to a player's list of delayed commands.
//              
// Parameters:  int entNum, const char *commandText
//              
// Returns:     -
//              
//================================================================
void upgPlayerDelayedServerCommand(int entNum,const char* commandText)
{
	//hzm chrissstrahl - I have a hunch this fixes the current issue that this fuc causes
	if (entNum < 0 || entNum >(game.maxclients - 1)) {
		return;
	}

	pendingServerCommand* command;
	pendingServerCommand* temp;
	gentity_t* edict = &g_entities[entNum];
	Player* player;
	int					  commandLength;

	if (!edict || !edict->inuse || !edict->client) {
		return;
	}

	player = (Player*)edict->entity;

	if (!player) {
		return;
	}

	// Get the memory...
	command = (pendingServerCommand*)malloc(sizeof(pendingServerCommand));
	temp = pendingServerCommandList[entNum];

	if (command == NULL) {
		gi.Printf("DelayedServerCommand: Couldn't allocate memory for new pendingServerCommand -> Dropping command.\n");
		return;
	}

	//gi.Printf( va( "A command: %s\n" , commandText ) );//debug

	// Prepare the new pendingServerCommand
	commandLength = strlen(commandText);
	commandLength++;//hzm chrissstrahl - fix text being cut off - what the fuck ???
	command->command = (char*)malloc(commandLength * sizeof(char));

	if (command->command == NULL) {
		gi.Printf("DelayedServerCommand: Couldn't allocate memory for new pendingServerCommandText -> Dropping command.\n");
		free(command);
		return;
	}

	Q_strncpyz(command->command, commandText, commandLength);
	command->next = NULL;

	//gi.Printf( va( "???command->command: %s\n" , command->command ) );//debug

	// Append the new command to the list (or start the list)
	if (pendingServerCommandList[entNum] == NULL) {
		pendingServerCommandList[entNum] = command;
	}
	else {
		while (temp->next)
		{
			temp = temp->next;
		}
//gi.Printf("upgPlayerDelayedServerCommand (%s): %s\n", player->client->pers.netname, command->command);
		temp->next = command;
	}
}

//================================================================
// Name:        handleDelayedServerCommands
// Class:       -
//              
// Description: This handles the still delayed serverCommands of all players.
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
void upgPlayerHandleDelayedServerCommands(void)
{
	int j;
	for (int i = 0; i < maxclients->integer; i++) {

		if (!&g_entities[i].inuse || !g_entities[i].entity || !g_entities[i].client) {
			continue;
		}
		Player* player = (Player*)g_entities[i].entity;
		
		pendingServerCommand* pendingCommand = pendingServerCommandList[i];

		while (pendingCommand) {
			//[b607] chrissstrahl, testedt value 71, result was that the mission succsess and sublevelLoading hud did stay on at the next level
			//the command ui_removehuds in coop_huds_setupMultiplayerUI did no longer work
			//the client 0 player name was set empty and player was named redshirt 105 WORKED FOR A LONG TIME but we are testing 90 now
			//the lower the number the potentially faster the commands get send, which is good for tricorder puzzles
			if (gi.GetNumFreeReliableServerCommands(player->entnum) > 90) { //chrissstrahl - used to be 105 changed, due to cyceling out cmds, was 32, 32 is to low! 105 might be to high... ...but coop mod used 122 for ages, 128 are usually the max
				//gi.Printf( va( "command: %s\n" , pendingCommand->command ) );
				
				int startIndex = 0;
				str sCmd;
				str sNewText = "";
				str sText = "";

				//[b607] chrissstrahl - optimize data string by not adding stufftext when not needed
				//[b608] chrissstrahl - popmenu with a menuname does not work purly clientside, it needs stufftext prefix which is why it has been removed from the checks
				if (Q_stricmpn("hudprint ", pendingCommand->command,9) == 0 ||
					Q_stricmp("status", pendingCommand->command) == 0 ||
					Q_stricmp("score", pendingCommand->command) == 0
					)
				{
					sCmd = upgStrings.getUntilChar(pendingCommand->command," ");
					startIndex = (sCmd.length() + 1);
				}
				else {
					sCmd = "stufftext";
				}
				sCmd += " \"";

				//[b607] chrissstrahl - printout all commands for debugging and optimisatzion purpose
				//gi.Printf(va("handleDelayedServerCommand: %s\n",pendingCommand->command));

				//hzm gameupdate chrissstrahl - cleanup string
				for (j = startIndex; j < strlen(pendingCommand->command); j++) {
					sText += pendingCommand->command[j];
				}

				//hzm coop mod chrissstrahl - phrase coop localstrings, replace with normal chars if player does not have coop
				sNewText = upgStrings.getReplacedUmlaute(player, sText);
				//make sure the text is no longer than 287 units or it will crash the game
				if (sNewText.length() > 287) {
					sNewText = upgStrings.getSubStr(sNewText, 0, 286);
					gi.Printf("handleDelayedServerCommands: String to long, was cut down to 286\n");
					gi.Printf("%s", sNewText.c_str());
				}
				sCmd += sNewText;
				sCmd += "\"\n";

				gi.SendServerCommand(i, sCmd.c_str());

				// Free the just used command
				pendingServerCommandList[i] = pendingCommand->next;
				free(pendingCommand->command);
				free(pendingCommand);
				pendingCommand = pendingServerCommandList[i];
			}
			else {
				//chrissstrahl - put the info into one print command [b607]
				//gi.Printf( va( "=====================\nhandleDelayedServerCommands freereliable: %i\ncommand delayed: %s\n==========================\n" , gi.GetNumFreeReliableServerCommands( i ) , pendingCommand->command) );
				break;
			}
		}
	}
}

//================================================================
// Name:        clearDelayedServerCommands
// Class:       -
//              
// Description: This clears the still delayed serverCommands of a player.
//              
// Parameters:  int entNum
//              
// Returns:     -
//              
//================================================================
void upgPlayerclearDelayedServerCommands(int entNum)
{
	pendingServerCommand* current;
	pendingServerCommand* temp;

	current = pendingServerCommandList[entNum];

	while (current != NULL) {
		temp = current;
		current = current->next;
		free(temp->command);
		free(temp);
	}

	pendingServerCommandList[entNum] = NULL;
}