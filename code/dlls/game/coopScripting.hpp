//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING SCRIPTING RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#ifndef __COOPSCRIPTING_HPP__
#define __COOPSCRIPTING_HPP__

#include "_pch_cpp.h"
#include "coopChallenges.hpp"
extern CoopChallenges coopChallenges;


//[b60011] chrissstrahl
class CoopScripting
{
private:
	bool includedMrm = false;
	bool includedMom4 = false;
	bool includedMom = false;
	bool includedNoscript = false;
public:
	bool getIncludedMrm();
	bool getIncludedMom4();
	bool getIncludedMom();
	bool getIncludedNoscript();
	str checkIncludedFiles(str sLex);
	bool checkIncludedMrm(str sLex);
	bool checkIncludedMom4(str sLex);
	bool checkIncludedMom(str sLex);
	bool checkIncludedNoscript(str sLex);
	void checkFuncCall(str sFunctioname, str parameterString[16]);
	str checkReplaceInclude(str sLex);
	bool mrnPlayerReady(Player *player);
};

//void coop_scriptingFunctionStarted( str sFunctioname );
void coop_setVectorScriptVariableValue( const char* varname , Vector vSet );
void coop_setFloatScriptVariableValue( const char* varname , float fSet );
void coop_setStringScriptVariableValue( const char* varname , const char* sSet );
#endif
