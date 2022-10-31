//[b607] new - handle vote specific coop stuff 2019.12.15

//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING SERVER RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2019 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]


#include "coopVote.hpp"
#include "coopReturn.hpp"
#include "coopHuds.hpp"
#include "coopParser.hpp"
#include "coopActor.hpp"

extern CoopChallenges coopChallenges;
#include "coopChallenges.hpp"

#include "actor.h"
#include "player.h"
#include "mp_manager.hpp"


//check also func:
//bool coop_textModifyVoteText( const str _voteString, str &sVoteText, Player* player )



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//														::callVote
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//========================================================= [b607]
// Name:        coop_vote_checkvalid
// Class:       -
//              
// Description: checks if a valid coop vote command was given
//              
// Parameters:	const str &command
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_checkvalid(const str &command)
{
	if (stricmp(command.c_str(), "coop_skill") == 0 ||
		stricmp(command.c_str(), "coop_maxspeed") == 0 ||
		stricmp(command.c_str(), "coop_respawntime") == 0 ||
		stricmp(command.c_str(), "coop_lms") == 0 ||
		stricmp(command.c_str(), "coop_airaccelerate") == 0 || //added in [b607]
		stricmp(command.c_str(), "coop_challenge") == 0 || //added in [b607]
		stricmp(command.c_str(), "coop_deadbodies") == 0 || //added in [b607]
		stricmp(command.c_str(), "coop_stasistime") == 0 || //added in [b607]
		stricmp(command.c_str(), "coop_next") == 0 ||
		stricmp(command.c_str(), "coop_prev") == 0 ||
		stricmp(command.c_str(), "coop_awards") == 0 ||
		stricmp(command.c_str(), "coop_ff") == 0 ||
		stricmp(command.c_str(), "coop_quit") == 0 //added [b60011]
		)
	{
		return true;
	}
	return false;
}

//========================================================= [b607]
// Name:        coop_vote_printcommands
// Class:       -
//              
// Description: Prints a list of valid votecommands to player hud
//              
// Parameters:	Player* player
//              
// Returns:     VOID
//              
//================================================================
void coop_vote_printcommands(Player *player)
{
	if ( !game.coop_isActive || !player) {
		return;
	}
	multiplayerManager.HUDPrint(player->entnum, "coop_skill <0-3>, coop_ff <0.0-2.0>, coop_maxspeed <200-1000>,coop_awards <0-1>\n");
	multiplayerManager.HUDPrint(player->entnum, "coop_respawntime <0-60>, coop_lms <0-10>,coop_challenge <0-3>, coop_next, coop_prev\n"); //[b607] added coop_teamIcon
	multiplayerManager.HUDPrint(player->entnum, "coop_deadbodies <0-25>,coop_airaccelerate <0-4>,coop_stasisTime <5-60>\n"); //[b607] added
}

//========================================================= [b607]
// Name:        coop_vote_skipcinematicValidate
// Class:       -
//              
// Description: Validates skipcinematic vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_skipcinematicValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "skipcinematic") != 0) {
		return 0;
	}

	if (level.cinematic != 1) {
		if (coop_checkPlayerLanguageGerman(player)) {
			multiplayerManager.HUDPrint(player->entnum, "Keine Sequenz aktiv.\n");
		}
		else {
			multiplayerManager.HUDPrint(player->entnum, "No Cinematic active.\n");
		}
		return 1;
	}
	if (world->skipthread.length() < 1) {
		//chrissstrahl - if a cinematic is not yet skipable but a player tries
		return 1;
	}
	//hzm gameupdate chrissstrahl - set skip status
	game.cinematicSkipping = true;

	return 3; //votestring was not altred
}

//========================================================= [b60011]
// Name:        coop_vote_quitserverValidate
// Class:       -
//              
// Description: Validates server quit vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_quitserverValidate(Player* player, const str& command, const str& arg, str& _voteString)
{
	if (Q_stricmp(command.c_str(), "coop_quit") != 0) {
		return 0;
	}

	//[b60011] chrissstrahl - save changes directly to ini
	int iDisallowQuit = atoi(coop_parserIniGet("serverData.ini", "votedisallowserverquit", "server"));
	iDisallowQuit = coop_returnIntWithinOrDefault(iDisallowQuit, 0, 1, 0);

	if (iDisallowQuit) {
		multiplayerManager.HUDPrint(player->entnum,"Coop Server Quit by vote is disabled on this server\n");
		return 1;		
	}

	if (dedicated->integer == 0) {
		multiplayerManager.HUDPrint(player->entnum, "Coop Server Quit only works on a dedicated server\n");
		return 1;
	}

	_voteString = "coop_quit";
	return 2; //votestring could have been changed (args)
}

//========================================================= [b607]
// Name:        coop_vote_lastmanstandingValidate
// Class:       -
//              
// Description: Validates last man standing vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_lastmanstandingValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "coop_lms") != 0) {
		return 0;
	}
	game.coop_lastmanstanding = coop_returnIntWithinOrDefault(coop_parserIniGet("serverData.ini", "lastmanstanding", "server"), 0, 10, (int)COOP_DEFAULT_LASTMANSTANDING);
	if (!stricmp(arg.c_str(), "")) {
		multiplayerManager.HUDPrint(player->entnum, va("Current LMS Status: %d\nRange: 0 - 10\n", game.coop_lastmanstanding));
		return 1;
	}
	int iLms = atoi(arg.c_str());
	if (iLms > 60) {
		iLms = 60;
	}
	else if (iLms < 0) {
		iLms = 0;
	}
	_voteString = va("coop_lms %i",iLms);
	return 2; //votestring could have been changed (args)
}

//========================================================= [b607]
// Name:        coop_vote_respawntimeValidate
// Class:       -
//              
// Description: Validates respawn time vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_respawntimeValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "coop_respawntime") != 0) {
		return 0;
	}
	int iTime; //[b607] chrissstrahl - changed because time is alreday reserved
	iTime = (int)multiplayerManager.getRespawnTime();

	if (!stricmp(arg.c_str(), "")) {
		multiplayerManager.HUDPrint(player->entnum, va("current respawntime: %d\nRange: 0 - 60 sec\n", iTime));
		return 1;
	}
	iTime = atoi(arg.c_str());
	if (iTime > 60) {
		iTime = 60;
	}
	else if (time < 0) {
		iTime = 0;
	}
	_voteString = va("coop_respawntime %i", iTime);
	return 2;//votestring could have been changed (args)
}
	
//========================================================= [b607]
// Name:        coop_vote_awardsValidate
// Class:       -
//              
// Description: Validates respawn time vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_awardsValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "coop_awards") != 0) {
		return 0;
	}

	int iVal = atoi(arg.c_str());
	if (!stricmp(arg.c_str(), "")){
		multiplayerManager.HUDPrint(player->entnum, "^2$$Usage$$:^8 coop_awards 0 - 1\n");
		return 1;
	}

	if (iVal > 1) {
		iVal = 1;
	}
	else if (iVal < 0) {
		iVal = 0;
	}

	_voteString = va("coop_awards %i", iVal);
	return 2;//votestring could have been changed (args)
}

//========================================================= [b607]
// Name:        coop_vote_friendlyfireValidate
// Class:       -
//              
// Description: Validates friendly fire vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_friendlyfireValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "coop_ff") != 0) {
		return 0;
	}

	float fVal = atof(arg.c_str());
	if (!stricmp(arg.c_str(), "")){
		multiplayerManager.HUDPrint(player->entnum, "^2$$Usage$$:^8 coop_ff 0 - 2.0\n");
		return 1;
	}

	if (fVal > 2) {
		fVal = 2.0f;
	}
	else if (fVal < 0) {
		fVal = 0.0f;
	}

	_voteString = va("coop_ff %f", fVal);
	return 2;//votestring could have been changed (args)
}

//========================================================= [b607]
// Name:        coop_vote_maxspeedValidate
// Class:       -
//              
// Description: Validates movement speed vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_maxspeedValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "coop_maxspeed") != 0) {
		return 0;
	}
	if (!stricmp(arg.c_str(), "")){
		multiplayerManager.HUDPrint(player->entnum, "^2$$Usage$$:^8 coop_maxspeed 200 - 1000\n");
		return 1;
	}

	int iVal = atoi(arg.c_str());
	if (iVal > 1000) {
		iVal = 1000;
	}
	else if (iVal < 200) {
		iVal = 200;
	}
	
	_voteString = va("coop_maxspeed %i",iVal);
	return 2;//votestring could have been changed (args)
}

//========================================================= [b607]
// Name:        coop_vote_mapValidate
// Class:       -
//              
// Description: Validates map vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER - 0 = skip, 1 = not valid, 2 = possbily altred, 3 = not altred
//              
//================================================================
int coop_vote_mapValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "map") != 0 && Q_stricmp(command.c_str(), "nextmap") != 0) {
		return 0;
	}
	// If a map command, make sure the map actually exists
	//hzm gamefix chrissstrahl - make also sure the next map exist, or the game crashes!
	if (stricmp(command.c_str(), "map") == 0 || stricmp(command.c_str(), "nextmap") == 0)
	{
		str fullMapName;

		//hzm gameupdate chrissstrahl - just to be sure, and to support $ als map var indicator
		int iVarPos;
		str sMapRealName = arg;
		iVarPos = coop_returnIntFind(sMapRealName, "$");
		if (iVarPos > 0) {
			sMapRealName = coop_returnStringFromWithLength(sMapRealName, 0, iVarPos);
		}

		fullMapName = "maps/";
		fullMapName += sMapRealName;
		fullMapName += ".bsp";

		if (!gi.FS_Exists(fullMapName.c_str())){
			multiplayerManager.HUDPrint(player->entnum, va("%s $$NotFoundOnServer$$ !!!\n",fullMapName.c_str()));
			return 1;
		}

		//[b608] chrissstrahl - if a non coop map is voted during coop and it is disabled do not allow it
		if (coop_returnCvarInteger("coop_votedisable") == 1) {
			if (strnicmp(sMapRealName.c_str(), "coop_",5) != 0 && //maps starting with coop_
				strnicmp(sMapRealName.c_str(), "prf_",4) != 0 && //maps starting with prf_
				coop_parserIsItemInCategory("maplist.ini", sMapRealName.c_str(),"singlePlayerMission") != true &&
				coop_parserIsItemInCategory("maplist.ini", sMapRealName.c_str(),"singlePlayerIgm") != true &&
				coop_parserIsItemInCategory("maplist.ini", sMapRealName.c_str(),"singlePlayerSecret") != true &&
				coop_parserIsItemInCategory("maplist.ini", sMapRealName.c_str(),"coopIncluded") != true
			) {
				if (coop_checkPlayerLanguageGerman(player)) {
					multiplayerManager.HUDPrint(player->entnum, va("%s Ist kein Coop Level! Server erlaubt nur Coop Levels!\n", sMapRealName.c_str()));
				}
				else {
					multiplayerManager.HUDPrint(player->entnum, va("%s is not a Coop Level! Server allowes only Coop Levels!\n", sMapRealName.c_str()));
				}
				return 1;
			}
		}
	}

	// If a map command was issued, preserve the nextmap cvar so we don't lose it
	if (strlen(sv_nextmap->string)){
		_voteString = va("%s %s; set nextmap \"%s\"", command.c_str(), arg.c_str(), sv_nextmap->string);
		return 2;//votestring could have been changed (args)
	}

	return 3;//votestring was not altred
}

//========================================================= [b607]
// Name:        coop_vote_mpmodifierValidate
// Class:       -
//              
// Description: Validates mp_modifier vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
//int coop_vote_mpmodifierValidate(Player* player, const str &command, const str &arg, str &_voteString)
//{
	//handled in MultiplayerManager::callVote
//}

//========================================================= [b607]
// Name:        coop_vote_airaccelerateValidate
// Class:       -
//              
// Description: Validates air accelleration (air move control) vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_airaccelerateValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "coop_airaccelerate") != 0) {
		return 0;
	}
	int iAccel;
	if (world->getPhysicsVar(WORLD_PHYSICS_AIRACCELERATE) != -1.0f) {
		iAccel = (int)world->getPhysicsVar(WORLD_PHYSICS_AIRACCELERATE);
	}
	else {
		iAccel = (int)sv_airaccelerate->value;
	}

	if (!stricmp(arg.c_str(), "")) {
		multiplayerManager.HUDPrint(player->entnum, va("current airaccelerate: %i\nRange: 0 - 4\n", iAccel));
		return 1;
	}

	iAccel = atoi(arg.c_str());
	if (iAccel > 4) {
		iAccel = 4;
	}
	else if (iAccel < 0) {
		iAccel = 0;
	}

	_voteString = va("coop_airaccelerate %i", iAccel);
	return 2;//votestring could have been changed (args)
}

//========================================================= [b607]
// Name:        coop_vote_stasistimeValidate
// Class:       -
//              
// Description: Validates stasistime vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_stasistimeValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "coop_stasistime") != 0) {
		return 0;
	}
	int iAccel = -666; //this is unfinsied have a bad value here so it can be quickly spotted

	if (!stricmp(arg.c_str(), "")) {
		multiplayerManager.HUDPrint(player->entnum, va("current stasistime: %d\nRange: 5 - 60\n", iAccel));
		return 1;
	}

	iAccel = atoi(arg.c_str());
	if (iAccel > 60) {
		iAccel = 60;
	}
	else if (iAccel < 5) {
		iAccel = 5;
	}
	_voteString = va("coop_stasistime %i", iAccel);
	return 2;
}

//========================================================= [b607]
// Name:        coop_vote_challengeValidate
// Class:       -
//              
// Description: Validates Challenge vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_challengeValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "coop_challenge") != 0) {
		return 0;
	}

	if (!stricmp(arg.c_str(), "")) {
		multiplayerManager.HUDPrint(player->entnum,va("^2$$Usage$$:^8 coop_challenge 0 - 3, current: %i\n", (short)coopChallenges.iCurrentChallenge));
		return 1;
	}

	if (game.levelType >= MAPTYPE_MISSION) {
		multiplayerManager.HUDPrint(player->entnum, va("^2$$c#127$$\n"));
		return 0;
	}

	int iChallenge = atoi(arg.c_str());
	if (iChallenge > 1) {
		iChallenge = 1;
	}
	else if (iChallenge < 0) {
		iChallenge = 0;
	}
	_voteString = va("coop_challenge %i", iChallenge);
	return 2;
}


//========================================================= [b607]
// Name:        coop_vote_deadbodiesValidate
// Class:       -
//              
// Description: Validates dead bodies vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_deadbodiesValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	#define COOP_MAX_DEADBODIES_PERAREA 25
	if (Q_stricmp(command.c_str(), "coop_deadbodies") != 0) {
		return 0;
	}

	if (!stricmp(arg.c_str(), "")){
		multiplayerManager.HUDPrint(player->entnum,va("^2$$Usage$$:^8 coop_deadbodies 0 - 25, current: %i\n", game.coop_deadBodiesPerArea));
		return 1;
	}

	int iDead = atoi(arg.c_str());
	if (iDead > COOP_MAX_DEADBODIES_PERAREA) {
		iDead = COOP_MAX_DEADBODIES_PERAREA;
	}
	else if (iDead < 0) {
		iDead = 0;
	}
	_voteString = va("coop_deadbodies %i", iDead);
	return 2;
}

//========================================================= [b607]
// Name:        coop_vote_kickValidate
// Class:       -
//              
// Description: Validates kick vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_kickValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "kick") != 0) {
		return 0;
	}
	if (!stricmp(arg.c_str(), "")){
		player->hudPrint("$$Usage$$: kick <number> or kick <playername>\n");

		gentity_t   *other;
		Player      *aPlayer;
		int	j;
		for (j = 0; j < game.maxclients; j++){
			other = &g_entities[j];
			if (other->inuse && other->client){
				aPlayer = (Player *)other->entity;
				if (aPlayer){
					player->hudPrint(va("Number:^5 %i ^8- Name: %s\n", aPlayer->entnum, aPlayer->client->pers.netname));
				}
			}
		}
		return 1;
	}
	//[b607] chrissstrahl - make sure we can kick players with spaces in their name
	if (coop_returnIntFind(arg, " ") != -1) {
		int iClient = -1;
		Player* plToKick = NULL;
		for (int i = 0; i < maxclients->integer; i++) {
			plToKick = (Player *)g_entities[i].entity;
			if (plToKick && plToKick->client && plToKick->isSubclassOf(Player)) {
				if (plToKick->client->pers.netname == arg) {
					iClient = i;
				}
			}
		}
		if (iClient >= 0) {
			_voteString = va("kick %i", iClient);
			return 2;
		}
	}
	return 3;//votestring was not altred
}

//========================================================= [b607]
// Name:        coop_vote_execValidate
// Class:       -
//              
// Description: Validates exec vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_execValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "exec") != 0) {
		return 0;
	}

	str fullCFGName;

	if (!stricmp(arg.c_str(), "")){
		//[b607] chrissstrahl - added german text handling
		if (coop_checkPlayerLanguageGerman(player)) {
			multiplayerManager.HUDPrint(player->entnum, "^5Beispiel: callvote exec holomatch -> startet Datei: cfg/server/callvote/holomatch.cfg\n");
		}
		else {
			multiplayerManager.HUDPrint(player->entnum, "^5Example: callvote exec holomatch -> executes file: cfg/server/callvote/holomatch.cfg\n");
		}
		return 1;
	}

	//[b607] chrissstrahl - changed file directory + added localstring
	fullCFGName = "cfg/server/callvote/";
	fullCFGName += coop_returnStringFilenameOnly(arg);
	fullCFGName += ".cfg";

	if (!gi.FS_Exists(fullCFGName.c_str())){
		multiplayerManager.HUDPrint(player->entnum, va("base/%s - $$NotFoundOnServer$$\n",fullCFGName.c_str()));
		if (coop_checkPlayerLanguageGerman(player)) {
			multiplayerManager.HUDPrint(player->entnum, "^2Zur Hilfe tippen Sie: ^5callvote exec\n");
		}
		else {
			multiplayerManager.HUDPrint(player->entnum, "^2For help type: ^5callvote exec\n");
		}
		return 1;
	}

	_voteString = va("exec ", fullCFGName.c_str());
	return 2;
}

//========================================================= [b607]
// Name:        coop_vote_skillValidate
// Class:       -
//              
// Description: Validates skill vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_skillValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "coop_skill") != 0) {
		return 0;
	}
	if (!stricmp(arg.c_str(), "") || atoi(arg) < 0 || atoi(arg) > 3){
		str currentSkill;
		if (coop_checkPlayerLanguageGerman(player)) {
			currentSkill = "^2$$Usage$$:^8 skill 0 = Easy, 1 = Normal, 2 = Hard, 3 = VeryHard. ^3Current skill: ";
		}
		else {
			currentSkill = "^2$$Usage$$:^8 skill 0 = Leicht, 1 = Normal, 2 = Schwer, 3 = Sehr Schwer. ^3Aktuell: ";
		}
		currentSkill += skill->integer;
		currentSkill += "\n";
		multiplayerManager.HUDPrint(player->entnum, currentSkill.c_str());
		return 1;
	}
	_voteString = va("skill %i", atoi(arg.c_str()));
	return 2;
}

//========================================================= [b607]
// Name:        coop_vote_addbotValidate
// Class:       -
//              
// Description: Validates skill vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_addbotValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "addbot") != 0) {
		return 0;
	}

	if (!gi.Cvar_VariableIntegerValue("bot_enable")){
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("Server: Diese Abstimmung erfordert dass bot_enable auf 1 gesetzt wird!\n");
		}
		else {
			player->hudPrint("Server: This Vote requires bot_enable to be set to 1!\n");
		}
		return 1;
	}
	if (!gi.AAS_Initialized()){
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("Server: Diese Karte hat keinen Bot-Support!\n");
		}
		else {
			player->hudPrint("Server: this map does not have bot-support!\n");
		}
		return 1;
	}
	if (!stricmp(arg.c_str(), "")){
		player->hudPrint("$$Usage$$: Addbot <botname> [skill 1-5] [team] [infiltrator,medic,technician,demolitionist,heavyweapons,sniper] [altname]\n");
		return 1;
	}

	//[b607] chrissstrahl - limit for number of bots that can be added via callvote
	int iNumberOfBots = 0;
	gclient_t	*cl;
	for (int i = 0; i < maxclients->integer; i++) {
		cl = game.clients + i;
		if (!(g_entities[cl->ps.clientNum].svflags & SVF_BOT)) {
			continue;
		}
		iNumberOfBots++;
	}

	if (coop_returnCvarInteger("sv_maxbots") >= iNumberOfBots) {
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint(va("Server: Maximale Bots erreicht, sv_maxbots ist gesetzt auf %i\n", coop_returnCvarInteger("sv_maxbots")));
		}
		else {
			player->hudPrint(va("Server: Can't add more bots, sv_maxbots is set to %i\n", coop_returnCvarInteger("sv_maxbots")));
		}
		return 1;
	}

	//gi.SendConsoleCommand(va("addbot %s", arg.c_str()));
	return 3;
}

//========================================================= [b607]
// Name:        coop_vote_mapNxtPrevValidate
// Class:       -
//              
// Description: Validates coop_nextmap/coop_prevmap vote string before it becomes a vote
//              
// Parameters:	Player* player, const str &command, const str &arg
//              
// Returns:     INTEGER
//              
//================================================================
int coop_vote_mapNxtPrevValidate(Player* player, const str &command, const str &arg, str &_voteString)
{
	if (Q_stricmp(command.c_str(), "coop_next") != 0 && Q_stricmp(command.c_str(), "coop_prev") != 0) {
		str sVt = va("%s %s", command.c_str(), arg.c_str());
		return 0;
	}
	//hzm coop mod chrissstrahl - now this is getting a bit complicated, but it is worth it
	//because we can now use a ini file to store the data that we need to offer vote next or previouse coop map
	//so the user or server admin can decide for him self what maps he wants to add to that list
	//this example could be used in many other aspects
	//
	//

	//start pos
	ListenKnoten *startMaplist = NULL;
	//end pos
	ListenKnoten *endMaplist = NULL;
	//current pos
	ListenKnoten *currentMaplist = NULL;
	//delete pos
	ListenKnoten *lastMaplist = NULL;
	//delete pos
	ListenKnoten *delMaplist = NULL;

	//start pos
	ListenKnoten *startCategories = NULL;
	//end pos
	ListenKnoten *endCategories = NULL;
	//current pos
	ListenKnoten *currentCategories = NULL;
	//delete pos
	ListenKnoten *delCategories = NULL;
	coop_parserGetItemsFromCategory("vote_maplist.ini", startCategories, endCategories, "categories");

	if (!startCategories) {
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("FEHLER: vote_maplist.ini sektion: categories, besch. oder fehlt.\n");
		}
		else {
			player->hudPrint("ERROR: vote_maplist.ini section: categories, is damaged or missing.\n");
		}
		gi.Printf(va("MultiplayerManager::callVote (%s) ERROR: vote_maplist.ini section: categories, is damaged or missing.\n", command.c_str()));

		//delete nodes again
		currentCategories = startCategories;
		while (currentCategories)
		{
			//gi.Printf( va( "delte c: %s\n" , currentCategories->value.c_str() ) );
			//get current address
			delCategories = currentCategories;
			//get next address
			currentCategories = currentCategories->next;
			//delete old element
			delete delCategories;
		}
		return 1;
	}

	//trough all categories and get all the items
	currentCategories = startCategories;
	while (currentCategories) {
		//start pos
		ListenKnoten *start = NULL;
		//end pos
		ListenKnoten *end = NULL;
		//current pos
		ListenKnoten *current = NULL;
		//delete pos
		ListenKnoten *del = NULL;
		coop_parserGetItemsFromCategory("vote_maplist.ini", start, end, currentCategories->value.c_str());


		//abbort on error///////////////////////////////////////////////////////
		if (!start) {
			if (coop_checkPlayerLanguageGerman(player)) {
				player->hudPrint(va("FEHLER: vote_maplist.ini sektion: %s, kaputt oder fehlt.\n", currentCategories->value.c_str()));
			}
			else {
				player->hudPrint(va("ERROR: vote_maplist.ini section: %s, is damaged or missing.\n", currentCategories->value.c_str()));
			}
			gi.Printf(va("MultiplayerManager::callVote (%s) ERROR: vote_maplist.ini section: %s, is damaged or missing.\n", command.c_str(), currentCategories->value.c_str()));

			//delete nodes again
			current = start;
			while (current)
			{
				//gi.Printf( va( "delte i: %s\n" , current->value.c_str() ) );
				//get current address
				del = current;
				//get next address
				current = current->next;
				//delete old element
				delete del;
			}

			//delete nodes again
			currentCategories = startCategories;
			while (currentCategories)
			{
				//gi.Printf( va( "delte c: %s\n" , currentCategories->value.c_str() ) );
				//get current address
				delCategories = currentCategories;
				//get next address
				currentCategories = currentCategories->next;
				//delete old element
				delete delCategories;
			}
			return 1;
		}
		//////////////////////////////////////////////////////////

		//make start node the current node
		current = start;

		///////////////////////////////////////////////////////////////////
		//////////////////////////////////ADD THESE MAPS TO THE VOTING LIST
					//handle current nodes
		while (current){
			//gi.Printf( va( "added m: %s\n" , current->value.c_str() ) );

			//create new element
			ListenKnoten *maplistNode = new ListenKnoten;
			maplistNode->value = current->value;
			maplistNode->value2 = currentCategories->value;
			maplistNode->next = NULL;
			if (lastMaplist) {
				lastMaplist->next = maplistNode;//old node knows next this one
				maplistNode->prev = lastMaplist;//new node knows prev last one
			}
			else {
				startMaplist = maplistNode;//remember first node
				maplistNode->next = NULL;//new node knows nothing yet
				maplistNode->prev = NULL;//new node knows nothing yet
			}
			lastMaplist = maplistNode;

			//gi.Printf( va( "added m2: %s\n" , maplistNode->value.c_str() ) );

			//get next address
			current = current->next;
		}
		endMaplist = lastMaplist;
		///////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////
					//delete nodes again
		current = start;
		while (current)
		{
			//gi.Printf( va( "delte i: %s\n" , current->value.c_str() ) );
			//get current address
			del = current;
			//get next address
			current = current->next;

			//because the list start and end are linked, we need to abbort at the end
			if (del == end) {
				current = NULL;
			}

			//delete old element
			delete del;
		}
		currentCategories = currentCategories->next;
	}

	//delete nodes again
	currentCategories = startCategories;
	while (currentCategories)
	{
		//gi.Printf( va( "delte c: %s\n" , currentCategories->value.c_str() ) );
		//get current address
		delCategories = currentCategories;
		//get next address
		currentCategories = currentCategories->next;
		//delete old element
		delete delCategories;
	}
	/////////////////////////////////////////////////////////////////////
	//hzm coop mod chrissstrahl - search current map
	//hzm gameupdate chrissstrahl - just to be sure, and to cupport $ als map var indicator
	int iVarPos;

	str sMapRealName = "";
	sMapRealName += level.mapname;
	iVarPos = coop_returnIntFind(sMapRealName, "$");
	if (iVarPos > 0) {
		sMapRealName = coop_returnStringFromWithLength(sMapRealName, 0, iVarPos);
		sMapRealName = sMapRealName.tolower();
	}

	//find current map in the list
	gi.Printf(va("searching m: %s\n", sMapRealName.c_str()));

	str sMapToLoad = "";
	currentMaplist = startMaplist;
	while (currentMaplist) {
		if (stricmp(sMapRealName.c_str(), currentMaplist->value.c_str()) == 0) {
			//gi.Printf( va( "found m: %s\n" , currentMaplist->value.c_str() ) );

			//get next or previouse map
			if (stricmp(command.c_str(), "coop_next") == 0) {
				if (currentMaplist->next) {
					currentMaplist = currentMaplist->next;
					//gi.Printf( "next\n" );
				}
				else if (startMaplist) {
					currentMaplist = startMaplist;
					//gi.Printf( "start\n" );
				}
			}
			else {
				if (currentMaplist->prev) {
					currentMaplist = currentMaplist->prev;
					//gi.Printf( "prev\n" );
				}
				else if (endMaplist) {
					currentMaplist = endMaplist;
					//gi.Printf( "end\n" );
				}
			}

			sMapToLoad = currentMaplist->value.tolower();
			break;
		}
		currentMaplist = currentMaplist->next;
	}
	///////////////////////////////////////////////////////////////////
	//delete nodes again
	currentMaplist = startMaplist;
	while (currentMaplist)
	{
		//gi.Printf( va( "delte m: %s\n" , currentMaplist->value.c_str() ) );
		//get current address
		delMaplist = currentMaplist;
		//get next address
		currentMaplist = currentMaplist->next;
		//delete old element
		delete delMaplist;
	}
	///////////////////////////////////////////////////////////////////

	//hzm coop mod chrissstrahl - current map not listed here - abort
	if (stricmp(sMapToLoad.c_str(), "") == 0) {
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint(va("%s ist nicht aufgelistet in: vote_maplist.ini.\n", sMapRealName.c_str()));
		}
		else {
			player->hudPrint(va("%s is not listed in: vote_maplist.ini.\n", sMapRealName.c_str()));
		}
		gi.Printf(va("%s is not listed in: vote_maplist.ini.\n", sMapRealName.c_str()));
		return 1;
	}

	_voteString = va("map %s", sMapToLoad.c_str());

	//hzm coop mod chrissstrahl - start a vote (will also check if the map is on the server)
	//multiplayerManager.callVote(player, "map", sMapToLoad.c_str());

	return 2; //votestring has changed
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//														::setVote
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//========================================================= [b607]
// Name:        coop_vote_deadbodiesSet
// Class:       -
//              
// Description: Handles deadbodies
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_addbotSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "addbot", 7) != 0) {
		return false;
	}
	str sBotname;
	int i;

	int iStart = coop_returnIntFind(_voteString, " ");
	if (_voteString.length() > (iStart + 1)) { iStart++; }
	else { return true; }

	for (i = iStart; i < _voteString.length(); i++) {
		sBotname += _voteString[i];
	}

	//check if bots can be spawned
	//[b607] chrissstrahl - limit for number of bots that can be added via callvote
	int iNumberOfBots = 0;
	gclient_t	*cl;
	for (int j = 0; j < maxclients->integer; j++) {
		cl = game.clients + j;
		if (!(g_entities[cl->ps.clientNum].svflags & SVF_BOT)) {
			continue;
		}
		iNumberOfBots++;
	}

	if (coop_returnCvarInteger("sv_maxbots") >= iNumberOfBots) {
		multiplayerManager.HUDPrintAllClients(va("Server: Can't add more bots, sv_maxbots is set to %i\n", coop_returnCvarInteger("sv_maxbots")));
		return true;
	}

	gi.SendConsoleCommand(va("addbot %s\n", sBotname.c_str()));
	return true;
}


//========================================================= [b607]
// Name:        coop_vote_skipcinematicSet
// Class:       -
//              
// Description: Handles skipcinematic
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_skipcinematicSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "skipcinematic", 13) != 0) {
		return false;
	}

	if (level.cinematic == 1 && (world->skipthread.length() > 0)){
		str skipthread;
		skipthread = world->skipthread;
		G_ClearFade();

		if (skipthread[0] == '$'){
			//call the given trigger instead
			if (world->skipthreadEntity){
				Event *event = new Event(EV_Activate);
				event->SetSource(EV_FROM_SCRIPT);
				event->SetThread(0);
				event->SetLineNumber(0);
				event->AddEntity(world);
				world->skipthreadEntity->ProcessEvent(event);
			}
		}
		else{
			ExecuteThread(skipthread);
		}
		// now that we have executed it, lets clear it so we don't call it again
		world->skipthread = "";
		//clear entity
		world->skipthreadEntity = NULL;

		//hzm gameupdate chrissstrahl - reset skip status
		game.cinematicSkipping = false;
	}
	return true;
}

//========================================================= [b607]
// Name:        coop_vote_mpmodifierSet
// Class:       -
//              
// Description: Handles skipcinematic
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_mpmodifierSet(const str  _voteString)
{
	if (Q_stricmpn(_voteString, "mp_modifier", 11) != 0) {
		return false;
	}

	str sValue = coop_returnStringStartingFrom(_voteString, coop_returnIntFind(_voteString, " "));
	str sModifier = coop_returnStringUntilWhithspace(_voteString);
	sModifier = coop_returnStringStartingFrom(sModifier, 12);

	str sFile = "";
	if (coop_returnBool(sValue)) {
		sFile = va("cfg/server/callvote/%s.cfg", sModifier.c_str());
	}
	else {
		sFile = va("cfg/server/callvote/%s_r.cfg", sModifier.c_str());
	}
	if (!gi.FS_Exists(sFile.c_str())) {
		multiplayerManager.HUDPrintAllClients(va("^3$$WARNING$$: %s $$NotFoundOnServer$$!", sFile.c_str()));
		return true;
	}
	gi.SendConsoleCommand(va("exec %s\n", sFile.c_str()));
	//inform players that they need to change the map
	multiplayerManager.HUDPrintAllClients("$$ServerOptionChanged$$ - $$ChangeMap$$ \n");

	return true;
}


//========================================================= [b611]
// Name:        coop_vote_quitserverSet
// Class:       -
//              
// Description: Handles server quit vote
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_quitserverSet(const str _voteString)
{
	int iLength = 9;
	if (Q_stricmpn(_voteString, "coop_quit", iLength) != 0) {
		return false;
	}

	//[b60011] chrissstrahl - save changes directly to ini
	int iDisallowQuit = atoi(coop_parserIniGet("serverData.ini", "votedisallowserverquit", "server"));
	iDisallowQuit = coop_returnIntWithinOrDefault(iDisallowQuit, 0, 1, 0);
	
	if (iDisallowQuit == 0) {
		game.coop_rebootForced = true;
		coop_serverManageReboot(level.mapname.tolower(), NULL);
		return true;
	}
	return false;
}

//========================================================= [b611]
// Name:        coop_vote_lastmanstandingSet
// Class:       -
//              
// Description: Handles last man standing
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_lastmanstandingSet(const str _voteString)
{
	int iLength = 9;
	if (Q_stricmpn(_voteString, "coop_lms ", iLength) != 0) {
		return false;
	}

	//Make sure there is actually a value
	if (_voteString.length() <= iLength) {
		return true;
	}

	//get value from votestring
	str sValueNew = "";
	for (int i = iLength; i < _voteString.length(); i++){
		sValueNew += _voteString[i];
	}

	int iValueOld = game.coop_lastmanstanding;
	int iValueNew = atoi(sValueNew);
	iValueNew =	coop_returnIntWithinOrDefault(iValueNew,0,10,0);

	//[b60011] chrissstrahl - save changes directly to ini
	coop_parserIniSet("serverData.ini", "lastmanstanding", (str)iValueNew, "server");
	game.coop_lastmanstanding = iValueNew;

	Player *player = NULL;

	//[b60011] chrissstrahl - send updated data to player UI
	for (int i = 0; i < maxclients->integer; i++) {
		player = (Player*)g_entities[i].entity;
		if (player && player->client && player->isSubclassOf(Player) && player->coopPlayer.installed) {
			player->widgetCommand("coopGpoLms", va("title %d", iValueNew));
		}
	}

	//[b60011] chrissstrahl - check if player is allowed back in
	if (game.coop_lastmanstanding == 0 || iValueOld == 0) {
		for (int i = 0; i < maxclients->integer; i++) {
			player = (Player*)g_entities[i].entity;
			if (player && player->client && player->isSubclassOf(Player)) {

				//[b60011] chrissstrahl - inform players of change
				if (game.levelType < MAPTYPE_MISSION) {
					if (coop_checkPlayerLanguageGerman(player)) {
						multiplayerManager.HUDPrint(player->entnum, "^5INFO^8: Last Man Standing nur auf Missionskarten aktiv!\n");
					}
					else {
						multiplayerManager.HUDPrint(player->entnum, "^5INFO^8: Last Man Standing is only active on Missionmaps!\n");
					}
				}else{
					if (coop_checkPlayerLanguageGerman(player)) {
						multiplayerManager.HUDPrint(player->entnum, va("^5INFO^8: Last Man Standing gesetzet auf:^5 %d\n", iValueNew));
					}
					else {
						multiplayerManager.HUDPrint(player->entnum, va("^5INFO^8: Last Man Standing set to:^5 %d\n", iValueNew));
					}
				}

				if (player->coopPlayer.deathTime > game.coop_levelStartTime) {
					player->coopPlayer.deathTime = 0;
					if (multiplayerManager.isPlayerSpectator(player) &&
						!multiplayerManager.isPlayerSpectatorByChoice(player) &&
						game.levelType >= MAPTYPE_MISSION)
					{
						multiplayerManager.respawnPlayer(player, true);
					}
				}
			}
		}
	}
	return true;
}

//========================================================= [b607]
// Name:        coop_vote_respawntimeSet
// Class:       -
//              
// Description: Handles respawn time vote
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_respawntimeSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "coop_respawntime", 16) != 0) {
		return false;
	}
	str sMultiplier;
	int i;

	int iStart = coop_returnIntFind(_voteString, " ");
	if (_voteString.length() > (iStart + 1)) { iStart++; }
	else { return true; }

	for (i = iStart; i < _voteString.length(); i++){
		sMultiplier += _voteString[i];
	}

	//hzm coop mod chrissstrahl - save changes
	multiplayerManager.setRespawnTime(atof(sMultiplier.c_str()));

	//hzm coop mod chrissstrahl - set global var
	game.coop_respawnTime = atoi(sMultiplier.c_str());

	//hzm coop mod chrissstrahl - save changes directly to ini
	coop_parserIniSet("serverData.ini", "respawntime", (int)multiplayerManager.getRespawnTime(), "server");

	coop_huds_callvoteOptionChangedUI("Respawn Time", sMultiplier.c_str(), "coopGpoRspwt");
	return true;
}

//========================================================= [b607]
// Name:        coop_vote_airaccelerateSet
// Class:       -
//              
// Description: Handles air accellerate
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_airaccelerateSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "coop_airaccelerate", 18) != 0) {
		return false;
	}

	int iStart = coop_returnIntFind(_voteString, " ");
	if (_voteString.length() > (iStart + 1)) { iStart++; }
	else { return true; }

	str sMultiplier;
	for (int i = iStart; i < _voteString.length(); i++){
		sMultiplier += _voteString[i];
	}
	int iVal = atoi(sMultiplier);

	//set air accelerate to physics var this way we do not have to bother with multiplayer settings
	world->setPhysicsVar("airAccelerate", (float)iVal);

	//save changes directly to ini
	coop_parserIniSet("serverData.ini", "airaccelerate", iVal , "server");

	//update callvote ui
	coop_huds_callvoteOptionChangedUI("airaccelerate", sMultiplier.c_str(), "coopGpoAa");
	return true;
}

//========================================================= [b607]
// Name:        coop_vote_stasistimeSet
// Class:       -
//              
// Description: Handles stasistime
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_stasistimeSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "coop_stasistime", 15) != 0) {
		return false;
	}

	str sMultiplier;

	int iStart = coop_returnIntFind(_voteString, " ");
	if (_voteString.length() > (iStart + 1)) { iStart++; }
	else { return true; }

	for (int i = iStart; i < _voteString.length(); i++) {
		sMultiplier += _voteString[i];
	}

	game.coop_stasisTime = atoi(sMultiplier);
	if (game.coop_stasisTime < 7) {
		game.coop_stasisTime = 7;
	}
	else if(game.coop_stasisTime > 30){
		game.coop_stasisTime = 30;
	}
	//hzm coop mod chrissstrahl - save changes directly to ini
	coop_parserIniSet("serverData.ini", "stasistime", game.coop_stasisTime, "server");

	//update callvote ui
	coop_huds_callvoteOptionChangedUI("Stasis Time", sMultiplier.c_str(), "coopGpoSt");
	return true;
}

//========================================================= [b607]
// Name:        coop_vote_awardsSet
// Class:       -
//              
// Description: Handles awards
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_awardsSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "coop_awards", 11) != 0) {
		return false;
	}
	str sValue;
	int i;

	int iStart = coop_returnIntFind(_voteString, " ");
	if (_voteString.length() > (iStart + 1)) { iStart++; }
	else { return true; }

	for (i = iStart; i < _voteString.length(); i++){
		sValue += _voteString[i];
	}

	//hzm coop mod chrissstrahl - save changes
	int iState = atof(sValue.c_str());
	if (iState != 1)iState = 0;
	sValue = iState;
	game.coop_awardsActive = iState;
	gameVars.SetVariable("awards", game.coop_awardsActive); //[b607] chrissstrahl - fixed wrong var being used

	//hzm coop mod chrissstrahl - save changes directly to ini
	coop_parserIniSet("serverData.ini", "awards", sValue, "server");

	//[b607] chrissstrahl - update callvote ui
	coop_huds_callvoteOptionChangedUI("Awards", sValue.c_str(), "coopGpoAw");

	return true;
}

//========================================================= [b607]
// Name:        coop_vote_friendlyfireSet
// Class:       -
//              
// Description: Handles friendly fire
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_friendlyfireSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "coop_ff", 7) != 0) {
		return false;
	}
	str sMultiplier;
	int i;

	int iStart = coop_returnIntFind(_voteString, " ");
	if (_voteString.length() > (iStart + 1)) { iStart++; }
	else { return true; }

	for (i = iStart; i < _voteString.length(); i++){
		sMultiplier += _voteString[i];
	}

	//hzm coop mod chrissstrahl - save changes
	game.coop_friendlyFire = atof(sMultiplier.c_str());
	gameVars.SetVariable("friendlyFire", game.coop_friendlyFire);

	//hzm coop mod chrissstrahl - save changes directly to ini
	coop_parserIniSet("serverData.ini", "friendlyFire", sMultiplier, "server");

	//[b607] chrissstrahl - update callvote ui
	coop_huds_callvoteOptionChangedUI("Friendly Fire", sMultiplier.c_str(), "coopGpoFF");

	return true;
}

//========================================================= [b607]
// Name:        coop_vote_maxspeedSet
// Class:       -
//              
// Description: Handles movespeed
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_maxspeedSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "coop_maxspeed", 13) != 0) {
		return false;
	}

	str speed;
	int i;

	int iStart = coop_returnIntFind(_voteString, " ");
	if (_voteString.length() > (iStart + 1)) { iStart++; }
	else { return true; }

	for (i = iStart; i < _voteString.length(); i++){
		speed += _voteString[i];
	}
	//hzm coop mod chrissstrahl - set global var
	game.coop_maxspeed = atoi(speed.c_str());

	world->setPhysicsVar("maxSpeed", game.coop_maxspeed);

	//[b607] chrissstrahl - set maxSpeed info for script (game.maxspeed)
	gameVars.SetVariable("maxSpeed", game.coop_maxspeed);

	//hzm coop mod chrissstrahl - save changes directly to ini
	coop_parserIniSet("serverData.ini", "maxSpeed", game.coop_maxspeed, "server");

	//[b607] chrissstrahl - update callvote ui
	coop_huds_callvoteOptionChangedUI("Max Speed", speed.c_str(), "coopGpoMvSpd");

	return true;
}

//========================================================= [b607]
// Name:        coop_vote_teamiconSet
// Class:       -
//              
// Description: Handles team icon/arrow
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_challengeSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "coop_challenge", 14) != 0) {
		return false;
	}
	str sChallenge;
	int i;

	int iStart = coop_returnIntFind(_voteString, " ");
	if (_voteString.length() > (iStart + 1)) { iStart++; }
	else { return true; }

	for (i = iStart; i < _voteString.length(); i++) {
		sChallenge += _voteString[i];
	}

	//hzm coop mod chrissstrahl - save changes directly to ini
	sChallenge = (short)atoi(sChallenge);
	coop_parserIniSet("serverData.ini", "challenge", sChallenge.c_str(), "server");

	//hzm coop mod chrissstrahl - set global var
	coopChallenges.iCurrentChallenge = (bool)atoi(sChallenge);

	//[b607] chrissstrahl - update callvote ui
	coop_huds_callvoteOptionChangedUI("$$c#032$$", coopChallenges.iCurrentChallenge, "coopGpoCh");

	return true;
}

//========================================================= [b607]
// Name:        coop_vote_deadbodiesSet
// Class:       -
//              
// Description: Handles deadbodies
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_deadbodiesSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "coop_deadbodies", 15) != 0) {
		return false;
	}
	str icon;
	int i;

	int iStart = coop_returnIntFind(_voteString, " ");
	if (_voteString.length() > (iStart + 1)) { iStart++; }
	else { return true; }

	for (i = iStart; i < _voteString.length(); i++) {
		icon += _voteString[i];
	}

	//save to ini
	icon = atoi(icon);
	coop_parserIniSet("serverData.ini", "deadbodies", icon.c_str(), "server");

	//set to glkobal var
	game.coop_deadBodiesPerArea = atoi(icon);

	//[b607] chrissstrahl - update callvote ui
	coop_huds_callvoteOptionChangedUI("Dead Bodies per Area", game.coop_deadBodiesPerArea, "coopGpoDb");

	//check if we should remove dead bodies
	if (game.coop_deadBodiesPerArea <= 0) {
		coop_actorDeadBodiesRemove();
	}

	return true;
}

//========================================================= [b607]
// Name:        coop_vote_mapSet
// Class:       -
//              
// Description: Handles mapvote
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_mapSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "map ", 4) != 0) {
		return false;
	}
//hzm coop mod chrissstrahl - if a coop map is voted and we are in coop level, use gamemap command
//i don't recall why we are doing this, but i think there was a benifical factor involved

	//hzm coop mod chrissstrahl - replace command 'map' with 'gamemap'
	str temp = "gamemap ";
	str temp2 = "";

	int i;
	for (i = 4; i < _voteString.length(); i++){
		temp += _voteString[i];
		temp2 += _voteString[i];
	}

	//[b607] only reboot here if we are running the coop mod
	if (game.coop_isActive) {
		coop_serverManageReboot(temp2);
	}

	//[b607] end cinematic if it is running
	if (level.cinematic) {
		//hzm coop mod chrissstrahl - end cin - clear letterbox or players might get stuck in game without being able to access menus if server chrashes
		level.m_letterbox_time_start = 0.0f;
		level.m_letterbox_dir = letterbox_out;
		level.m_letterbox_time = 0.0f;
		level.m_letterbox_fraction = 1.0f / 8.0f;
		G_StopCinematic();
	}
	//[b607] chrisssttrahl - let players know that the server is loading the map
	multiplayerManager.HUDPrintAllClients(va("$$LoadingMap$$: %s\n", temp2.c_str()));

	//hzm coop mod chrissstrahl - load map now
	gi.SendConsoleCommand(va("%s\n", temp.c_str()));

	return true;
}

//========================================================= [b607]
// Name:        coop_vote_kickbotsSet
// Class:       -
//              
// Description: Handles kickbots
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_kickbotsSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "kickbots ", 9) != 0 && Q_stricmp(_voteString, "kick kickbots") != 0) {
		return false;
	}
//[b607] chrissstrahl - added kickbots - second check needed if kick used from the ui (new Kick all bots)
	gclient_t	*cl;
	for (int i = 0; i < maxclients->integer; i++) {
		cl = game.clients + i;
		if (!(g_entities[cl->ps.clientNum].svflags & SVF_BOT)) {
			continue;
		}
		gi.SendConsoleCommand(va("kick %i\n", i));
	}
	return true;
}

//========================================================= [b607]
// Name:        coop_vote_execSet
// Class:       -
//              
// Description: Handles kickbots
//              
// Parameters:	str _voteString
//              
// Returns:     BOOL
//              
//================================================================
bool coop_vote_execSet(const str _voteString)
{
	if (Q_stricmpn(_voteString, "exec ", 5) != 0) {
		return false;
	}
//[b607] chrissstrahl - added exec for executing server side configs (in a extra folder)

	str sValue = "";
	int iStart = coop_returnIntFind(_voteString, " ");
	if (_voteString.length() > (iStart + 1)) {
		iStart++;
		for (int i = iStart; i < _voteString.length(); i++) {
			sValue += _voteString[i];
		}
		sValue = coop_returnStringFilenameOnly(sValue);
		gi.SendConsoleCommand(va("exec cfg/server/callvote/%s.cfg\n", sValue.c_str()));
	}
	return true;
}