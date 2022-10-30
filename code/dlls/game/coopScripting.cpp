//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING SCRIPTING RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "coopReturn.hpp"
#include "coopArmory.hpp"
#include "coopObjectives.hpp"
#include "coopParser.hpp"
#include "coopScripting.hpp"
CoopScripting coopScripting;


#define COOP_SCRIPTING_FILENAME_COOPMODMAIN "coop_mod/matrix/main.scr"
#define COOP_SCRIPTING_FILENAME_MULTIOPTIONSMENU_4 "mom4_coopInput.scr"
#define COOP_SCRIPTING_FILENAME_MULTIOPTIONSMENU "multioptions.scr"
#define COOP_SCRIPTING_FILENAME_NOSCRIPT "noscript.scr"
#define COOP_SCRIPTING_FILENAME_MISSIONRESOURCEMENU "_mrm.scr"
#define COOP_SCRIPTING_ENTITYVAR_MISSIONRESOURCEMENU_PLAYER_DONE "mrmLoadOutComplete"


bool CoopScripting::getIncludedCoop() { return includedCoop; }
bool CoopScripting::getIncludedMrm() { return includedMrm; }
bool CoopScripting::getIncludedMom4() { return includedMom4; }
bool CoopScripting::getIncludedMom() { return includedMom; }
bool CoopScripting::getIncludedNoscript() { return includedNoscript; }

str CoopScripting::checkIncludedFiles(str sLex)
{
	//[b60011] chrissstrahl - check and remember if special scripts are used
	checkIncludedCoop(sLex);
	checkIncludedMom(sLex);
	checkIncludedMom4(sLex);
	checkIncludedMrm(sLex);
	checkIncludedNoscript(sLex);

	//[b607] chrissstrahl - dynamically replace global_scripts with coop versions (only in multiplayer)
	sLex = checkReplaceInclude(sLex);
	//[b60011] chrissstrahl - return replaced script files
	return sLex;
}

bool CoopScripting::checkIncludedCoop(str sLex)
{
	if (coop_returnIntFind(sLex, COOP_SCRIPTING_FILENAME_COOPMODMAIN) != -1) {
		includedCoop = true;
	}
	return includedCoop;
}

bool CoopScripting::checkIncludedMrm(str sLex)
{
	if (coop_returnIntFind(sLex, COOP_SCRIPTING_FILENAME_MISSIONRESOURCEMENU) != -1) {
		includedMrm = true;
	}
	return includedMrm;
}

bool CoopScripting::checkIncludedMom4(str sLex)
{
	if (coop_returnIntFind(sLex, COOP_SCRIPTING_FILENAME_MULTIOPTIONSMENU_4) != -1) {
		includedMom4 = true;
	}
	return includedMom4;
}

bool CoopScripting::checkIncludedMom(str sLex)
{
	if (coop_returnIntFind(sLex, COOP_SCRIPTING_FILENAME_MULTIOPTIONSMENU) != -1) {
		includedMom = true;
	}
	return includedMom;
}

bool CoopScripting::checkIncludedNoscript(str sLex)
{
	if (coop_returnIntFind(sLex, COOP_SCRIPTING_FILENAME_NOSCRIPT) != -1) {
		includedNoscript = true;
	}
	return includedNoscript;
}

void CoopScripting::checkFuncCall(str sFunctioname,str parameterString[16])
//check for various Coop specific function calls to execute certain code in dll
//code that was perviously in the scripts but now is in the dll will be started from here
// - so the scripts do not need to ne rewritten
// - and no new script commands need to be put into place
// - however, if this gets to bit we might want to have new script commands and call them from teh coop scripts
{
	sFunctioname = sFunctioname.tolower();
	if (sFunctioname == "globalcoop_objectives_update_dll") {
		coop_objectivesUpdate(parameterString[0], parameterString[1], parameterString[2]);
	}
	else if (sFunctioname == "globalcoop_server_itemunlockedset_dll") {
		str sKey = va("unlocked.%s", coop_armoryReturnWeaponName(parameterString[0]).c_str());
		coop_parserIniSet("serverData.ini", sKey, "true", "server");
	}
	else if (sFunctioname == "globalcoop_server_itemlockedset_dll") {
		str sKey = va("unlocked.%s", coop_armoryReturnWeaponName(parameterString[0]).c_str());
		coop_parserIniSet("serverData.ini", sKey, "false", "server");
	}
	else if (	sFunctioname == "coop_endlevel" ||
				sFunctioname == "globalcoop_mission_accomplished" ||
				sFunctioname == "globalcoop_mission_completed" ||
				sFunctioname == "globalCoop_mission_failWithReason" )
	{
		coopChallenges.disabled(true);
	}
}

str CoopScripting::checkReplaceInclude(str sLex)
//will load coop optimized global files
//global scripts will be switched by the mod and scripters don't have to worry
{
	if (g_gametype->integer != GT_SINGLE_PLAYER && getIncludedCoop()) { //[b60011] chrissstrahl - also check if we are really using the coop mod or not - fix this destaster once and for all
		if (!Q_stricmp(sLex, "maps/global_scripts/global_acceleratedmovement.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_archetype.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_common.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_flyin.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_playerloadout.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_shuttle.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_soundpan.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_spawnwave.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_tubedoor.scr.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/igm_dialog.scr") ||
			//[b60011] end
			
			!Q_stricmp(sLex, "maps/global_scripts/global_tricorderbase.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_tricorderroute.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_tricorderkeypad.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_tricordermod.scr")
			)
		{
			str s = "coop_mod/maps/global_scripts/";
			str sFn = coop_returnStringFilenameOnly(sLex);
			s += sFn;
			if (gi.FS_ReadFile(s, NULL, true) != -1) {
				//strcpy(lex.pr_immediate_string, s.c_str());
				sLex = s;
				gi.Printf(va("HZM Coop Mod is using %s from coop_mod folders\n", sFn.c_str()));
			}
		}
	}
	return sLex;
}

bool CoopScripting::mrnPlayerReady(Player* player)
//checks if player has completed selecting equipment from Mission Resource Managment Menu
{
	if (!player) { return false; }

	ScriptVariable* entityData;
	entityData = NULL;
	entityData = player->entityVars.GetVariable(COOP_SCRIPTING_ENTITYVAR_MISSIONRESOURCEMENU_PLAYER_DONE);
	if (entityData == NULL) {
		return false;
	}
	float fReady = entityData->floatValue();
	return (bool)fReady;
}

void coop_setVectorScriptVariableValue( const char* varname , Vector vSet )
{
	program.setVectorVariableValue( varname , vSet );
}
void coop_setFloatScriptVariableValue( const char* varname , float fSet )
{
	program.setFloatVariableValue( varname , fSet );
}
void coop_setStringScriptVariableValue( const char* varname , const char* sSet )
{
	program.setStringVariableValue( varname , sSet );
}



