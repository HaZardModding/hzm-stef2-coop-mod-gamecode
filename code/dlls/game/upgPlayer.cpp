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
#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"


//=========================================================[b60014]
// Name:        player::upgPlayerIsHost
// Class:       -
//              
// Description: Checks if player is host
//              
// Parameters:  void
//              
// Returns:     bool
//              
//================================================================
bool Player::upgPlayerIsHost()
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return true;
	}

#ifdef WIN32
	bool bWindows = true;
#else
	bool bWindows = false;
#endif

	cvar_t* cl_running = gi.cvar_get("cl_running");
	if (dedicated->integer == 0 && entnum == 0 && bWindows && (cl_running ? cl_running->integer : 0)) {
		return true;
	}
	return false;
}

//=========================================================[b60014]
// Name:        player::upgPlayerSetup
// Class:       -
//              
// Description: Handles Player Say stuff, filters and script execution, ect...
//				player::upgPlayerSay will retrive the player answer
//              
// Parameters:  void
//              
// Returns:     bool
//================================================================
void Player::upgPlayerSetup()
{
	//tell player to give us his cl_maxpackets and language
	DelayedServerCommand(entnum, "vstr cl_maxpackets;vstr local_language");
}

//=========================================================[b60014]
// Name:        player::upgPlayerSay
// Class:       -
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
				DelayedServerCommand(entnum, "set cl_maxpackets 60");
			}
			upgPlayer.clMaxPackets = iClMaxPack;
			return true;
		}
	}

	//detect player language
	if		(Q_stricmpn(sayString.c_str(), "eng", 3) == 0) { setLanguage("Eng");return true; }
	else if	(Q_stricmpn(sayString.c_str(), "deu", 3) == 0) { setLanguage("Deu");return true; }

	//SPAM FILTER - this is our sv_floodprotect replacement, since flood protect also blocks multiplayer specific commands which we are in need of to work
	if (upgPlayer.chatTimeLimit < level.time) { upgPlayer.chatTimeLimit = level.time; }
	upgPlayer.chatTimeLimit++;
	upgPlayer.chatTimeLimit++;
	if (upgPlayer.chatTimeLimit > (level.time + 3)) {
		//display info that the player was spamming
		if ((upgPlayer.chatTimeLastSpamInfo + 3.0f) < level.time) {
			upgPlayer.chatTimeLastSpamInfo = level.time;
			if (coop_checkPlayerLanguageGerman(this)) { hudPrint("^3Sie chatten zu schnell, Nachricht blockiert durch ^5Spamschutz!\n"); }
			else { hudPrint("^3You chat to fast, message blocked by ^5Spamprotection!\n"); }
		}
		return true;
	}


	return false;
}

//================================================================
// Name:        upgPlayerSaySpamfilterCountdown
// Class:       -
//              
// Description: counts down after player has spammed until player is allowed to chat again
//
// Parameters:  void 
//              
// Returns:     void
//              
//================================================================
void Player::upgPlayerSaySpamfilterCountdown()
{
	if (upgPlayer.chatTimeLimit < level.time) { return; }

	if ((upgPlayer.chatTimeLimit + -1) < level.time) { upgPlayer.chatTimeLimit = level.time; }
	else { upgPlayer.chatTimeLimit--; }
}

//========================================================[b60014]
// Name:        upgPlayerClientThink
// Class:       -
//              
// Description: ClientThink, used to update/check stuff each frame
//
// Parameters:  void 
//              
// Returns:     void
//              
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
// Name:        player::upgPlayerIsBot
// Class:       -
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
// Name:        player::upgPlayerGetTargetedEntity
// Class:       -
//              
// Description: this has been implemented for script use
//              
// Parameters:  event
//              
// Returns:     void
//================================================================
void Player::upgPlayerGetTargetedEntity(Event* ev)
{
	Entity* target;
	target = this->GetTargetedEntity();
	if (!target) {
		target = world;
	}
	ev->ReturnEntity(target);
}

//==========================================================[b607]
// Name:        player::ugpPlayerInThirdPerson
// Class:       -
//              
// Description: returns if player is in third person or not
//              
// Parameters:  void
//              
// Returns:     qboolean
//================================================================
qboolean Player::ugpPlayerInThirdPerson()
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
void Player::getViewtraceEndpos(Event* ev)
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
		fov = atof(coop_returnCvarString("sv_defaultFov"));
	}
	else if (fov > 160.0f) {
		fov = 160.0f;
	}
	ev->ReturnFloat(fov);
}

//[b60013] chrissstrahl - get offsets for player skins/models used in specialities and ctf, this might come in handy in coop
void Player::upgPlayerGetBackpackAttachOffset(Event* ev) { ev->ReturnVector(_backpackAttachOffset); }
void Player::upgPlayerGetBackpackAttachAngles(Event* ev) { ev->ReturnVector(_backpackAttachAngles); }
void Player::upgGetFlagAttachOffset(Event* ev) { ev->ReturnVector(_flagAttachOffset); }
void Player::getFlagAttachAngles(Event* ev) { ev->ReturnVector(_flagAttachAngles); }

//[b60013] chrissstrahl - checks if player is a specific button
void Player::checkCrouch(Event* ev) { ev->ReturnFloat((int)GetCrouch()); }
void Player::checkJump(Event* ev){ qboolean bJump = last_ucmd.upmove > 0; ev->ReturnFloat((int)bJump); }
void Player::checkForward(Event* ev){ qboolean bJump = last_ucmd.forwardmove > 0; ev->ReturnFloat((int)bJump); }
void Player::checkBackward(Event* ev){ qboolean bJump = last_ucmd.forwardmove < 0; ev->ReturnFloat((int)bJump); }
void Player::checkLeft(Event* ev){ qboolean bJump = last_ucmd.rightmove < 0; ev->ReturnFloat((int)bJump); }
void Player::checkRight(Event* ev){ qboolean bJump = last_ucmd.rightmove > 0; ev->ReturnFloat((int)bJump); }
void Player::checkLeanLeft(Event* ev){ qboolean bJump = last_ucmd.lean > 0; ev->ReturnFloat((int)bJump); }
void Player::checkLeanRight(Event* ev){ qboolean bJump = last_ucmd.lean < 0; ev->ReturnFloat((int)bJump); }
void Player::checkDropRune(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_DROP_RUNE) != 0); }
void Player::checkRun(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_RUN) != 0); }
void Player::checkReload(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_RELOAD) != 0); }

//[b60011] chrissstrahl - checks if player is pressing a specific button
void Player::checkFire(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_ATTACKLEFT) != 0); }
void Player::checkFirealt(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_ATTACKRIGHT) != 0); }
void Player::checkThirdperson(Event* ev) { ev->ReturnFloat((int)_isThirdPerson); }
void Player::checkUsePressing(Event* ev) { ev->ReturnFloat((last_ucmd.buttons & BUTTON_USE) != 0); }

//[b60012] chrissstrahl - cancel player modulating event - if player gets hurt or so
void Player::cancelPuzzle()
{
	Entity* puzzle;
	puzzle = G_FindClass(NULL, "puzzle_object");
	while (puzzle) {
		PuzzleObject* puzObj = (PuzzleObject*)puzzle;
		if (puzObj->GetLastActivatingEntity() == this) {
			puzObj->cancelPlayer(this);
			return;
		}
		puzzle = G_FindClass(puzzle, "puzzle_object");
	}
}

//[b60011] chrissstrahl
void Player::RunThread(Event* ev)
{
	str thread_name = "";
	thread_name = ev->GetString(1);
	RunThread(thread_name);
}

//[b60011] chrissstrahl
CThread* Player::RunThread(const str& thread_name)
{
	if (thread_name.length() <= 0)
		return NULL;

	return ExecuteThread(thread_name, true, this);
}

//[b60011] chrissstrahl - allow to get Holdable Item Name from outside class
str Player::getHoldableName(void)
{
	if (_holdableItem) {
		return _holdableItem->getName();
	}
	return "";
}

//[b60011] chrissstrahl - allow to get Holdable Item Model from outside class
str Player::getHoldableModel(void)
{
	if (_holdableItem) {
		return _holdableItem->model;
	}
	return "";
}

//[b60011] chrissstrahl - allow to get Rune Item from outside class
Rune* Player::getRuneItem(void)
{
	if (_rune) {
		return _rune;
	}
	return NULL;
}

//[b60011] chrissstrahl - allow to get Rune Model from outside class
str Player::getRuneModel(void)
{
	if (_rune) {
		return _rune->model;
	}
	return "";
}

//[b60011] chrissstrahl - allow to get Rune Name from outside class
str Player::getRuneName(void)
{
	if (_rune) {
		return _rune->getName();
	}
	return "";
}

//[b60011] chrissstrahl - allow to get Rune Item from outside class
Powerup* Player::getPowerupItem(void)
{
	if (_powerup) {
		return _powerup;
	}
	return NULL;
}

//[b60011] chrissstrahl - allow to get Rune Model from outside class
str Player::getPowerupModel(void)
{
	if (_powerup) {
		return _powerup->model;
	}
	return "";
}

//[b60011] chrissstrahl - allow to get Rune Name from outside class
str Player::getPowerupName(void)
{
	if (_powerup) {
		return _powerup->getName();
	}
	return "";
}

//hzm gameupdate chrissstrahl - add new commands for script use
void Player::getScore(Event* ev)
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
void Player::addScore(Event* ev)
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

void Player::getDeaths(Event* ev)
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	if (multiplayerManager.inMultiplayer()) {
		ev->ReturnFloat(multiplayerManager.getDeaths(this));
	}
	else {
		ev->ReturnFloat(0.0f);
	}
}

//[b60011] chrissstrahl - get player viewangle
void Player::getViewanglesEvent(Event* ev)
{
	Vector vAngle = Vector(0, 0, 0);
	GetPlayerView(NULL, &vAngle);
	ev->ReturnVector(vAngle);
}

//Allow individual killthread to be set by script for each player
void Player::setKillThread(Event* ev)
{
	entityVars.SetVariable("_killThread",ev->GetString(1));
}

//[GAMEUPGRADE][b60014] chrissstrahl - Execute individual killthread for this player
void Player::killThread()
{
	ScriptVariable* entityData = NULL;
	entityData = entityVars.GetVariable("_killThread");
	str sThread = (entityData) ? entityData->stringValue() : "";
	if (sThread.length()){ ExecuteThread(sThread.c_str(), true, this); }
}

//hzm gameupdate chrissstrahl - return time at which player was last hurt
float Player::getLastDamageTime(void)
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

void Player::getScriptVariablesCommand(Event* ev)
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
void Player::gamefix_WorldEffects(void)
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
				if (!multiplayerManager.inMultiplayer() || game.coop_isActive)
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
		if (game.coop_isActive) air_finished = level.time + 15.0f;
		else					  air_finished = level.time + 10.0f;
		drown_damage = 2.0f;

		//hzm gameupdate chrissstrahl - take the Damage-Multiplier in account, when restoring health
		float damageMultiplier = 1;
		if (!multiplayerManager.inMultiplayer() || game.coop_isActive)
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

void Player::getKills(Event* ev)
{
	//[b60014] chrissstrahl - make sure using that command in singleplayer does not make it go boom
	if (multiplayerManager.inMultiplayer()) {
		ev->ReturnFloat(multiplayerManager.getKills(this));
	}
	else {
		ev->ReturnFloat(0.0f);
	}
}

void Player::getLastDamaged(Event* ev)
{
	ev->ReturnFloat(getLastDamageTime());
}

void Player::getTeamName(Event* ev)
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

void Player::getTeamScore(Event* ev)
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
void Player::getNameEvent(Event* ev)
{
	//[b60014] chrissstrahl - return also player name when not in multiplayer instead of crashing
	if (!multiplayerManager.inMultiplayer()) {
		cvar_t* cvar = gi.cvar_get("name");
		ev->ReturnString(cvar ? cvar->string : "");
		return;
	}

	str s;
	s = multiplayerManager._playerData[entnum]._name;

	//[b609] chrissstrahl - updated to allow replacing of SPACE
	// orientate on func SetCanBeFinishedBy for a neat solution with multiple arguments
	str sReplace = "_";
	if (ev->NumArgs() > 0) {
		sReplace = ev->GetString(1);
		str sPlayername = s;
		int		i;
		s = "";

		for (i = 0; i < sPlayername.length(); i++) {
			if (sPlayername[i] == ' ') {
				s += sReplace;
			}
			else {
				s += sPlayername[i];
			}
		}
	}
	ev->ReturnString(s.c_str());
}

//[b60011] chrissstrahl - add ability to set a proper widgetCommand that contains spaces
//widgetname,commandparameter,parameter,parameter,parameter,parameter,parameter,parameter,parameter,parameter,parameter
void Player::widgetCommandEvent(Event* ev)
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
	widgetCommand(ev->GetString(1), sParameters);
}

//[b60011] chrissstrahl - add ability to set a proper widgetCommand that contains spaces
void Player::widgetCommand(str sWidget, str sParameters)
{
	//SPECIALS: ~=NEWLINE ^=SPACER #=NEWLINE
	//str sTemp;
	if (coop_returnIntFind(sParameters.c_str(), "labeltext") != -1) {
		sParameters = coop_replaceForLabelText(sParameters);
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
void Player::setCameraEvent(Event* ev)
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
			game.cinematicCurrentCam = NULL;
		}
		return;
	}

	if (!camera->isSubclassOf(Camera)) {
		gi.Printf(va("setCamera::Entity $%s is of class %s needs to be Camera\n", camera->targetname.c_str(), camera->getClassname()));
		return;
	}

	//set current camera so it can be restored on savegame
	if (g_gametype->integer == GT_SINGLE_PLAYER) {
		game.cinematicCurrentCam = camera;
	}
	SetCamera((Camera*)camera, switchTime);
}
