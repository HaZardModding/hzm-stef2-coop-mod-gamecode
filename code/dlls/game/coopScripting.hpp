//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING SCRIPTING RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"
#include "coopChallenges.hpp"

//[b60011] chrissstrahl
class CoopScripting
{
private:
	bool includedCoop = false;
	bool includedMrm = false;
	bool includedMom4 = false;
	bool includedMom = false;
	bool includedNoscript = false;
public:
	bool getIncludedCoop();
	bool getIncludedMrm();
	bool getIncludedMom4();
	bool getIncludedMom();
	bool getIncludedNoscript();
	void init();
	str checkIncludedFiles(str sLex);
	bool checkIncludedCoop(str sLex);
	bool checkIncludedMrm(str sLex);
	bool checkIncludedMom4(str sLex);
	bool checkIncludedMom(str sLex);
	bool checkIncludedNoscript(const str sLex);
	void checkFuncCall(str sFunctioname, str parameterString[16]);
	str checkReplaceInclude(str sLex);
	bool mrnPlayerReady(Player *player);
};

extern CoopScripting coopScripting;

//void coop_scriptingFunctionStarted( str sFunctioname );
void coop_setVectorScriptVariableValue( const char* varname , Vector vSet );
void coop_setFloatScriptVariableValue( const char* varname , float fSet );
void coop_setStringScriptVariableValue( const char* varname , const char* sSet );
