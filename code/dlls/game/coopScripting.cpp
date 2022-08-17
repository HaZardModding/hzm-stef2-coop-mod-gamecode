//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING SCRIPTING RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "coopReturn.hpp"
#include "coopScripting.hpp"
CoopScripting coopScripting;
#include "coopObjectives.hpp"

#define COOP_SCRIPTING_FILENAME_MULTIOPTIONSMENU_4 "mom4_coopInput.scr"
#define COOP_SCRIPTING_FILENAME_MULTIOPTIONSMENU "multioptions.scr"
#define COOP_SCRIPTING_FILENAME_NOSCRIPT "noscript.scr"
#define COOP_SCRIPTING_FILENAME_MISSIONRESOURCEMENU "_mrm.scr"
#define COOP_SCRIPTING_ENTITYVAR_MISSIONRESOURCEMENU_PLAYER_DONE "mrmLoadOutComplete"


bool CoopScripting::getIncludedMrm() { return includedMrm; }
bool CoopScripting::getIncludedMom4() { return includedMom4; }
bool CoopScripting::getIncludedMom() { return includedMom; }
bool CoopScripting::getIncludedNoscript() { return includedNoscript; }

void CoopScripting::checkIncludedFiles(str sLex)
{
	//[b60011] chrissstrahl - check and remember if special scripts are used
	checkIncludedMom(sLex);
	checkIncludedMom4(sLex);
	checkIncludedMrm(sLex);
	checkIncludedNoscript(sLex);

	//[b607] chrissstrahl - dynamically replace global_scripts with coop versions (only in multiplayer)
	checkReplaceInclude(sLex);
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

str CoopScripting::checkReplaceInclude(str sLex)
{
	if (g_gametype->integer != GT_SINGLE_PLAYER) {
		//[b60011] chrissstrahl - added other global files
		//so that the scripts will be switched by the mod and scripters don't have to worry
		if (!Q_stricmp(sLex, "maps/global_scripts/global_acceleratedmovement.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_archetype.scr") ||
			!Q_stricmp(sLex, "maps/global_scripts/global_cinematicfx.scr") ||
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



//================================================================
// Name:        coop_scriptingFunctionStarted
// Class:       -
//              
// Description: gets called when a function is called eigther as thread, or in line
//              
// Parameters:  str sFunctioname
//              
// Returns:     VOID
//              
//================================================================
//void coop_scriptingFunctionStarted( str sFunctioname )
//{
//	gi.Printf( "coop func call - %s\n" , sFunctioname.c_str() );
	//if ( !Q_stricmp( sFunctioname , "" ) ){
	//	gi.Printf( va("FUNCTION: %s\n" , sFunctioname.c_str() ) );
	//}
//}

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



