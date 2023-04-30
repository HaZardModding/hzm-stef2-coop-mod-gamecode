//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING RETURN HANDLINGS RELATED FUNCTIONS (value is given computed here and returned to parrent func) FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"

#include "coopReturn.hpp"
#include "coopCheck.hpp"
#include "coopAlias.hpp"
#include "coopParser.hpp"

//[b60013] chrissstrahl - added rpg specific support
#include "coopRPG.hpp"
extern CoopRPG coopRpg;

#include "entity.h"
#include "player.h"
#include "weapon.h"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include "powerups.h"
#include "interpreter.h"
#include "program.h"
#include "trigger.h"
#include "weaputils.h"

Entity*	coop_returnEntity( const str &name )
{
#if 0
	gentity_t *ed;
	int i;

	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		ed = &g_entities[i];

		if ( !ed->inuse || !ed->entity )
			continue;

		if ( !Q_stricmp( ed->entity->targetname.c_str() , name.c_str() ) )
			return ed->entity;
	}

	return NULL;
#else
	TargetList *tlist;

	if ( ( name.length() > 0 ) && name[0] == '*' )
		return G_GetEntity( atoi( name.c_str() + 1 ) );

	tlist = world->GetTargetList( name , false );

	if ( tlist )
		return tlist->GetNextEntity( NULL );
	else
		return NULL;
#endif
}


str coop_getStringScriptVariable( str s )
{
	return program.getStringVariableValue( s.c_str() );
}
float coop_getFloatScriptVariable( str s )
{
	return program.getFloatVariableValue( s.c_str() );
}
Vector coop_getVectorScriptVariable( str s )
{
	return program.getVectorVariableValue( s.c_str() );
}
str coop_returnBoolWord(int iValue)
{
	if ( iValue == 1) {
		return "true";
	}
	return "false";
}
str coop_returnBoolWord( bool bValue )
{
	if ( bValue == true ) {
		return "true";
	}
	return "false";
}

int coop_returnIntOrDefaultFromString( str sSource, int iDefault )
{
	try
	{
		coop_trimM( sSource , " \t\r\n;=,f" );
		if ( sSource.length() ) {
			return atoi( sSource ); //[b60011] chrissstrahl - changed from (int)atof()
		}
		else {
			return iDefault;
		}
	}
	catch ( ... )
	{ return iDefault; }
}

//[b60011] chrissstrahl - have it also accept strings
int coop_returnIntWithinOrDefault(const str& sVal, const int& iMin, const int& iMax, const int& iDefault)
{
	const int iCur = coop_returnIntOrDefaultFromString(sVal, iDefault);
	return coop_returnIntWithinOrDefault(iCur, iMin, iMax, iDefault);
}
//[b607] chrissstrahl
int coop_returnIntWithinOrDefault(const int &iCur, const int &iMin, const int &iMax, const int &iDefault)
{
	if (iCur < iMin || iCur > iMax) {
		return iDefault;
	}
	return iCur;
}

//[b60011] chrissstrahl - have it also accept strings
float coop_returnFloatWithinOrDefault(const str& sVal, const float& fMin, const float& fMax, const float& fDefault)
{
	const float fCur = coop_returnIntOrDefaultFromString(sVal, fDefault);
	return coop_returnFloatWithinOrDefault(fCur, fMin, fMax, fDefault);
}
float coop_returnFloatWithinOrDefault(const float &fCur, const float &fMin, const float &fMax, const float &fDefault)
{
	if (fCur < fMin || fCur > fMax) {
		return fDefault;
	}
	return fCur;
}
//end

float coop_returnFloatOrDefaultFromString( str sSource , float fDefault )
{
	try
	{
		coop_trimM( sSource , " \t\r\n;=,f" );
		if ( sSource.length() ) {
			return atof( sSource );
		}
		else {
			return fDefault;
		}
	}
	catch ( ... )
	{
		return fDefault;
	}
}

int coop_returnIntFromString( const str &sSource )
{
	try { return (int)atof( sSource ); }
	catch ( ... ){
		gi.Printf( "coop_returnIntFromString\n===============\nNo Integer found in:'%s'\n" , sSource.c_str() );
		return -1;
	}
}

int coop_returnIntFromFormatedString(str &sSource, const char &cFind)
{
	if ( sSource.length() < 1 ) { return -1; }
	
	try
	{
		int iPos;
		int iTemp = -1;
		str sExtracted = "";
		coop_trimM( sSource , " \t" );

		iPos = coop_returnIntFind( sSource , cFind );
		if ( iPos < 1 )
		{
			sExtracted = coop_substrToEnd( sSource , iPos );
		}
		else
		{
			sExtracted = coop_substr( sSource , 0 , iPos );
		}

		coop_trimM( sExtracted , " \t" );

		try { iTemp = atoi( sExtracted ); }
		catch ( ... )
		{
			return -1;
		}

		if ( sSource.length() < ( iPos + 2 ) )
		{
			return iTemp;
		}

		try { sSource = coop_substrToEnd( sSource , iPos + 1 ); }
		catch ( ... ) { return -1; }

		coop_trimM( sSource , " \t" );

		return iTemp;
	}
	catch ( ... )
	{
		gi.Printf( va( "coop_returnIntFromFormatedString\n===============\n*.ini ERROR: Bad format in at:\n%s\n", sSource.c_str() ) );
		return -1;
	}
}

//extract/evaluate a boolan from the given string
bool coop_returnBool(str sValue)
{
	coop_manipulateStringTrim(sValue, " \t\r\n");
	//[b607] chrissstrahl - made to return false on default
	if ( !sValue.length() ) { return false; }

	if ( Q_stricmpn(sValue.c_str(),"true",4) == 0) { return true; }
	
	if ( atoi( sValue.c_str()) > 0 ) { return true; }
	return false;
}

//return position at which given string was found otherwise return -1
//starts at 0
int coop_returnIntFind(str sSource , str sKeyword )
{
	//[b60012] chrissstrahl - fix missing .c_str()
	if ( Q_stricmp( sKeyword.c_str(), "") == 0 || Q_stricmp( sSource, "") == 0)
	{
		return -1;
	}
	str ss , sk;
	ss = sSource;
	sk = sKeyword;
	ss = ss.tolower(); 
	sk = sk.tolower();

	int iCurrtrentMatchIndex = 0;
	int iFoundAt = -1;

	for ( int i = 0; i < strlen(ss); i++ )
	{
		//check every char from the MATCH-LIST
		if ( ss[i] == sk[iCurrtrentMatchIndex] ) {
			if ( iCurrtrentMatchIndex == 0 )
			{
				iFoundAt = i;
			}

			iCurrtrentMatchIndex++;

			if ( iCurrtrentMatchIndex == strlen( sk ) ) {
				break;
			}
		}
		else {
			iCurrtrentMatchIndex = 0;
			iFoundAt = -1;
		}
	}

	//make sure we don't get any odd behaviour
	if( iCurrtrentMatchIndex < sk.length() ) {
		iCurrtrentMatchIndex = 0;
		iFoundAt = -1;
	}

	//gi.Printf( va( "FOUND::::%d\n" , iFoundAt ) );
	return iFoundAt;
}
//trim given chars (remove them if they are at the end or the beginning of that string)
void coop_manipulateStringTrim( str &sTrim , const str &sTrimMatch )
{
	str sTemp = "";
	bool b;
	bool bStarted = false;

	//check every char from the MATCH-LIST
	for ( int i = 0; i < sTrim.length(); i++ ) {
		b = true;
		for ( int j = 0; j < strlen( sTrimMatch ); j++ ) {
			if ( sTrim[i] == sTrimMatch[j] )
			{
				b = false;
				break;
			}
		}

		//the actual word we want to keep started
		if ( !bStarted && b )
		{
			bStarted = true;
		}

		//combine word
		if ( b || bStarted )
		{
			sTemp += sTrim[i];
		}
	}

	//clear var
	sTrim = "";
	bStarted = false;

	//check every char from the MATCH-LIST
	for ( int i = ( sTemp.length() - 1 ); i > -1; i-- ) {
		b = true;
		for ( int j = 0; j < strlen( sTrimMatch ); j++ ) {
			if ( sTemp[i] == sTrimMatch[j] )
			{
				b = false;
				break;
			}
		}

		//the actual word we want to keep started
		if ( !bStarted && b )
		{
			bStarted = true;
		}

		//combine word
		if ( b || bStarted )
		{
			sTrim += sTemp[i];
		}
	}

	//reset var
	sTemp = "";

	//reverse the shitt again
	for ( int i = ( sTrim.length() - 1 ); i > -1; i-- ) {
		sTemp += sTrim[i];
	}

	sTrim = sTemp;
}
str coop_returnStringTrim( str sTrim , const str sTrimMatch )
{
	coop_manipulateStringTrim( sTrim , sTrimMatch );
	return sTrim;
}

//================================================================
// Name:        coop_returnStringCleanedCvar
// Class:       -
//              
// Description:  returns a string that has been cleaned to work with cvars
//              
// Parameters:  str
//              
// Returns:     str
//              
//================================================================
str coop_returnStringCleanedCvarvalue( str s ) { return coop_returnStringCleanedCvar( s[0] , true ); }
str coop_returnStringCleanedCvarvalue( char s ) { return coop_returnStringCleanedCvar( s , true ); }
str coop_returnStringCleanedCvarname( str s ){ return coop_returnStringCleanedCvar( s[0], false );}
str coop_returnStringCleanedCvarname( char s ){ return coop_returnStringCleanedCvar( s, false ); }
str coop_returnStringCleanedCvar( char s , bool bIsValue )
{//holy fuck, they are all valid in cVars, after all these years I didn't know that!

	//if it is the value, a few more will be valid
	if ( bIsValue == true )
	{
		if ( s == ' ' || s == '\t' || s == '"' || s == ';' || s == '\\' || s == '\"')
		{
			return va( "%c" , s );//convert to string
		}
	}
	//these are always valid
	if (isalnum(s) ||
		s == '_' ||
		s == '+' ||
		s == '-' ||
		s == '#' ||
		s == '-' ||
		s == '\'' ||
		s == '*' ||
		s == '^' ||
		s == '!' ||
		s == '?' ||
		s == '&' ||
		s == '?' ||
		s == '=' ||
		s == '/' ||
		s == '(' ||
		s == ')' ||
		s == '[' ||
		s == ']' ||
		s == '{' ||
		s == '}' ||
		s == '<' ||
		s == '>' ||
		s == '|' ||
		s == '$' ||
		//s == '%' || //could lead to issues so it was discarded
		s == ':' ||
		s == '.' ||
		s == ',' )
	{
		return va( "%c" , s );//convert to string
	}
	return "";
}

//================================================================
// Name:        coop_returnStringCleanedCvar
// Class:       -
//              
// Description:  returns a string that has been cleaned to work with cvars
//              
// Parameters:  str
//              
// Returns:     str
//              
//================================================================
str coop_returnStringExtractedCvarValue( str s )
{
	char cLast = '\0';
	str sTemp = "";
	bool bQouteOpen = false;
	for ( int i = 0; i < strlen( s ); i++ )
	{
		//skip these if they are at the start of the string
		if ( strlen( sTemp ) == 0 &&
			s[i] == '"' ||
			s[i] == ';' ||
			s[i] == ' ' ||
			s[i] == '\t'
			)
		{
			//remember last char for comparison with next
			cLast = '\0';//reset
			continue;
		}
		//skip these if at the end of the string
		if ( (i + 1) == strlen( s ) &&
			s[i] == '"' ||
			s[i] == ';' ||
			s[i] == ' ' ||
			s[i] == '\t'
			)
		{
			//remember last char for comparison with next
			cLast = '\0';//reset
			continue;
		}
		//keep track of the qoutes but ignore escaped qoutes
		if ( !bQouteOpen && s[i] == '"' && cLast != '\\' )
		{
			bQouteOpen = true;
			continue;
		}//inside qoute(after the first) ", occours a ", without a \, before it ("" NOT "\")
		else if( bQouteOpen && s[i] == '"' && cLast != '\\' )
		{
			bQouteOpen = false;
			//exit here, the value has ended
			break;
		}

		sTemp += s[i];
		//remember last char for comparison with next
		cLast = s[i];
	}
	return sTemp;
}
//================================================================
// Name:        coop_returnStringSkillname
// Class:       -
//              
// Description:  returns the name of the skill/dificulty for the given number
//              
// Parameters:  int iSkill
//              
// Returns:     str
//              
//================================================================
str coop_returnStringSkillname( int iSkill )
{
	if ( iSkill == 1 ){
		return "$$Normal$$";
	}else if ( iSkill == 2 ){
		return "$$Hard$$";
	}
	else if ( iSkill == 3 ){
		return "$$VeryHard$$";
	}
	return "$$Easy$$";
}

//================================================================
// Name:        coop_returnFloatAltIfPrimaryIsLtZero
// Class:       -
//              
// Description:		returns alternative value if primary value hits the limit
//					limit can be higher or lower, will be determined by bool
//              
// Parameters:  bool bHigher, float fValue, float fLimit , float fAlt
//              
// Returns:     float
//              
//================================================================
float coop_returnFloatAltIfValueBelow( bool bHigher, float fValue, float fLimit , float fAlt )
{
	if ( bHigher && fValue >= fLimit )	{ return fAlt; }
	else if ( fValue <= fLimit )		{ return fAlt; }
	return fValue;
}

//================================================================
// Name:        coop_returnStringStartingFrom
// Class:       -
//              
// Description:  returns a string starting from given index
//              
// Parameters:  str sString , int iStart
//              
// Returns:     str
//              
//================================================================
str coop_returnStringStartingFrom( const str &sString , const int &iStart )
{
	str sPartial = "";
	if ( sString && strlen( sString ) > iStart ) {
		int i;
		for ( i = iStart; i < strlen( sString ); i++ ) {
			sPartial += sString[i];
		}
	}
	return sPartial;
}

//================================================================
// Name:        coop_returnStringFromWithLength
// Class:       -
//              
// Description:  returns a string starting from given index with given length
//				 Replacement for substr
//              
// Parameters:  str sString , int iStart, int iEnd
//              
// Returns:     str
//              
//================================================================
void coop_manipulateStringFromWithLength( str &sString , const int &iStart , int iEnd )
{
	const int iLength = sString.length();
	str sPartial = sString;
	sString = "";
	if ( iLength > iStart )
	{
		if ( iLength < ( iStart + iEnd ) )
		{
			iEnd = ( iLength - iStart );
		}

		for ( int i = iStart; i < ( iStart + iEnd ); i++ )
		{
			sString += sPartial[i];
		}
	}
	else
	{
		throw( "Contact HZM with this info data: COOP PROGRAMMING ERROR IN: _returnStringFromWithLength ERROR INFORMATION READS: start pos > then strlen" );
	}
}
str coop_returnStringFromWithLength( str sString , const int &iStart , int const &iEnd )
{
	coop_manipulateStringFromWithLength( sString , iStart , iEnd );
	return sString;
}

//========================================================[b60011]
// Name:        coop_returnStringDomainname
// Class:       -
//              
// Description:  returns the filename of a given path string
//              
// Parameters:  str sPath
//              
// Returns:     str
//              
//================================================================
str coop_returnStringDomainname( const str &sPath )
{
	int i = 0;
	str domainName	= "";
	str construct	= "";

	//if it starts with http or https - reject all else
	if (coop_contains(sPath, "http://") == 0) {
		i  = 7;
	}
	else if(coop_contains(sPath, "https://") == 0) {
		i  = 8;
	}
	else if (coop_contains(sPath, "://") != -1) {
		return "";
	}

	while (1) {
		if (i >= sPath.length() || sPath[i] == '/' || sPath[i] == '?' || sPath[i] == '\\' || sPath[i] == '#') {
			break;
		}
		construct += sPath[i];

		if (sPath[i] == '.') {
			domainName = construct;
			construct = "";
		}
		else if (sPath[i] == '@') {
			construct = "";
		}
		i++;
	}
	return (domainName += construct);
}

//================================================================
// Name:        coop_returnStringFilenameOnly
// Class:       -
//              
// Description:  returns the filename of a given path string
//              
// Parameters:  str sPath
//              
// Returns:     str
//              
//================================================================
str coop_returnStringFilenameOnly( const str &sPath )
{
	int i;
	str fileName = "";
	for ( i = 0; i < sPath.length(); i++ ){
		if ( sPath[i] == '/' || sPath[i] == '\\' ){
			fileName = "";
		}
		else{
			fileName += sPath[i];
		}
	}
	return fileName;
}

//[b607]
//================================================================
// Name:        coop_returnStringPathFileNoExtension
// Class:       -
//              
// Description:  returns the path and filename without file extension
//              
// Parameters:  str sPath
//              
// Returns:     str
//              
//================================================================
str coop_returnStringPathFileNoExtension(const str &sPath)
{
	int i;
	str fileName = "";
	for (i = 0; i < sPath.length(); i++) {
		if (sPath[i] == '.') {
			break;
		}
		fileName += sPath[i];
	}
	return fileName;
}

//================================================================
// Name:        coop_returnStringFileExtensionOnly
// Class:       -
//              
// Description:  returns the fileextension of a given path string
//              
// Parameters:  str sPath
//              
// Returns:     str
//              
//================================================================
str coop_returnStringFileExtensionOnly( const str &sPath )
{
	str fileName = "";
	str sPath2 = sPath;
	sPath2 = sPath2.tolower();
	for ( int i = 0; i < strlen(sPath2); i++ )
	{
		if (sPath2[i] == '/' || sPath2[i] == '\\' || sPath2[i] == ':' || sPath2[i] == ' ' ) {
			fileName = "";
		}
		else {
			fileName += sPath2[i];
		}		
	}

	str fileName2 = "";
	for ( int i = 0; i < strlen( fileName ); i++ )
	{
		if ( fileName[i] == '.' ) {
			fileName2 = ".";
		}
		else {
			fileName2 += fileName[i];
		}
	}

	if ( strlen(fileName2) )
	{
		return fileName2;
	}
	return fileName;
}

//================================================================
// Name:        coop_returnStringUntilWhithspace
// Class:       -
//              
// Description:  returns the given string until a whitespace occours
//              
// Parameters:  str sChain
//              
// Returns:     str
//              
//================================================================
str coop_returnStringUntilWhithspace( str sChain )
{
	if ( !sChain ) return "";

	int i;
	str sPart = "";
	for ( i = 0; i < sChain.length(); i++ ){
		if ( sChain[i] == ' ' ){
			return sPart;
		}else{
			sPart += sChain[i];
		}
	}
	return sPart;
}

//[b607] chrissstrahl
//================================================================
// Name:        coop_returnStringUntilNewline
// Class:       -
//              
// Description:  returns the given string until a newline occours
//              
// Parameters:  str sChain
//              
// Returns:     str
//              
//================================================================
str coop_returnStringUntilNewline(str sChain)
{
	if (!sChain) return "";

	int i;
	str sPart = "";
	for (i = 0; i < sChain.length(); i++) {
		if (sChain[i] == '\n') {
			return sPart;
		}
		else {
			sPart += sChain[i];
		}
	}
	return sPart;
}

//================================================================
// Name:        coop_returnVectorFromString
// Class:       -
//              
// Description:  returns the given string until a whitespace occours
//              
// Parameters:  str sChain
//              
// Returns:     str
//              
//================================================================
Vector coop_returnVectorFromString( str sChain )
{
	if ( !sChain ) return Vector( 0.0f , 0.0f , 0.0f );

	Vector v( 0.0f , 0.0f , 0.0f );
	bool bBeginning=true;
	bool bDot=false;
	int i,iPos=0;
	float fVx , fVy , fVz;
	str sPart = "";
	str sPart0 , sPart1 , sPart2;
	for ( i = 0; i < sChain.length(); i++ ){

		//to many seperate values
		if ( iPos > 3 ){
			gi.Printf( va( "coop_returnVectorFromString(%s)\nTo many sections, aborting early.\n" , sChain.c_str() ) );
			break;
		}

		//valid digit
		if ( isdigit( sChain[i] ) ){
			if ( bBeginning )bBeginning = false;
			sPart += sChain[i];
			continue;
		}

		//accept minus as valid
		if ( sChain[i] == '-' ){
			if ( bBeginning ){
				sPart += '-';
				bBeginning = false;
			}
			else{
				gi.Printf( va( "coop_returnVectorFromString(%s)\nBAD CHAR %s at [i], dropped char.\n" , sChain.c_str() , sChain[i] , i ) );
			}
			continue;
		}

		//accept dot as valid, but only once per position
		if ( sChain[i] == '.' && !bDot ){
			bDot = true;

			//add leading 0
			if ( bBeginning ){
				sPart += '0';
				sPart += '.';
				bBeginning = false;
				continue;
			}
			else{
				sPart += '.';
				continue;
			}
		}

		//seperator, set new axis, set to beginning
		if ( sChain[i] == ' ' ){
			//whitespace at the beginning
			if ( bBeginning ){
				continue;
			}
			//whithespace in in between
			if ( !bBeginning && sPart.length() > 0 ){
				//is a valid number
				if ( IsNumeric( sPart ) ){
					if ( iPos == 0 ) sPart0=sPart;
					else if ( iPos == 1 )sPart1 = sPart;
					else sPart2 = sPart;
				}
				//else{
					//gi.Printf( va( "coop_returnVectorFromString(%s)\nBAD VECTOR, returning '0 0 0'\n" , sChain.c_str() ) );
				//}

				bDot = false;
				bBeginning = true;
				sPart = "";
				iPos++;
			}
			continue;
		}
	}

//extract floats
	if ( sPart ){
		sPart2 = sPart;
	}
	fVx = atof( sPart0 );
	fVy = atof( sPart1 );
	fVz = atof( sPart2 );

//if we have at least 3 valid segments, set Vec
	if ( iPos >= 2 ){
		v = Vector( fVx , fVy , fVz );
		//gi.Printf( va( "coop_returnVectorFromString(%s)\nOutput: (%f,%f,%f)\n" , sChain.c_str() , fVx , fVy , fVz ) );
	}//else{
		//gi.Printf( va( "coop_returnVectorFromString(%s)\nOutput: (default/error)\n" , sChain.c_str() ) );
	//}
	return v;
}

//================================================================
// Name:        coop_returnStringDeathmessagenameForEntity
// Class:       -
//              
// Description:  returns deathmessage string for the given actor, used when player gets killed by the actor
//              
// Parameters:  Actor * actor
//              
// Returns:     str
//              
//================================================================
str coop_returnStringDeathmessagenameForEntity( Entity * entity )
{
	//[b60012][cleanup] - this should be put in a list or ini (the ini handling needs to be improved first)
	if ( !entity ){
		return "^8a GHOST...";
	}
	str printString = "";
	str entityClass = entity->getClassname();
	str entityTargetname = entity->targetname;
	str entityModel = entity->model;

	//please note that we want to keep these names as abstract as possible, so that
	//they could be used by mappers on their own maps, it is possible to use the
	//uservar1 to uservar4 variables like this:
	//	"uservar1" "killmessage  was killed by a CockieMonster"
	//	"uservar1" "name CookieMonster"
	//
	//Details in: coop_playerKilled, or contact HaZardModding

	//get all items into the list, if they match the primary critiria
	//[b60012] chrissstrahl - fix missing .c_str()
	if ( Q_stricmpn( entityClass.c_str(), "ScriptSlave" , 11 ) == 0) {
		printString = coop_parserIniGet( "deathlist.ini" , entity->targetname , "targetname" );
	}else if (	Q_stricmpn( entityClass.c_str(), "Explosion" , 9 ) == 0 ||
				Q_stricmpn( entityClass.c_str(), "Projectile" , 10 ) == 0)
	{
		printString = coop_parserIniGet( "deathlist.ini" , entity->model , "model" );
	}
	else if ( Q_stricmpn( entityClass.c_str(), "SlidingDoor" , 11 ) == 0) {
		printString = coop_parserIniGet( "deathlist.ini" , entity->getClassname() , "class" );
	}

	if ( printString == "" ) {
		printString += "???: ";
		printString += entityClass;
		printString += " # ";
		printString += entityModel;
		printString += " # ";
		printString += " $";
		printString += entityTargetname;
	}
	return printString;
}

//================================================================
// Name:        coop_returnEntityPlayerFavored
// Class:       -
//              
// Description:  returns any player, prefarably not a spectator in multiplayer, be aware that this function is also called during SP and needs to work in any gametype or mode!
//              
// Parameters:  VOID
//              
// Returns:     Entity
//              
//================================================================
Entity* coop_returnEntityPlayerFavored( void )
{
	int i;
	Entity *player = NULL;
	Entity *playerFirstValid = NULL;
	Entity *playerNoSpectator = NULL;

	//hzm coop mod chrissstrahl - return in sp directly
	if ( g_gametype->integer == GT_SINGLE_PLAYER )
	{
		player = (Player *)g_entities[0].entity;
		if ( player && player->client && player->isSubclassOf( Player ) )
		{
			return player;
		}
	}

	for ( i = 0; i < maxclients->integer; i++ )
	{
		player = g_entities[i].entity;

		if ( player && player->client && player->isSubclassOf( Player ) )
		{
		//grab any player that is valid and store just in case
			if ( playerFirstValid == NULL )
			{
				playerFirstValid = player;
			}
		//see if player is NOT a spectator
			if ( !multiplayerManager.isPlayerSpectator( ( Player * )player ) )
			{
				if ( playerNoSpectator == NULL )
				{
					playerNoSpectator = player;
				}
				//perfect, not spec and not dead, return directly
				if ( player->deadflag == DEAD_NO )
				{
					return player;
				}
			}
		}
	}
	if ( playerNoSpectator )
		return playerNoSpectator;
	if ( playerFirstValid )
		return playerFirstValid;

	return NULL;
}


//================================================================
// Name:        coop_returnPlayerNotFavored
// Class:       -
//              
// Description:  returns any player no preferances, this player could be dead, or a spectator
//              
// Parameters:  VOID
//              
// Returns:     Player
//              
//================================================================
Player* coop_returnPlayerNotFavored( void )
{
	Player *player = NULL;
	for(int i = 0; i < maxclients->integer; i++)
	{
		player = ( Player * )g_entities[i].entity;
		if ( player && player->client && player->isSubclassOf( Player ) )
		{
			return player;
		}
	}
return NULL;
}


//================================================================
// Name:        coop_returnPlayerFavored
// Class:       -
//              
// Description:  returns any player, prefarably not a spectator in multiplayer, be aware that this function is also called during SP and needs to work in any gametype or mode!
//              
// Parameters:  VOID
//              
// Returns:     Player
//              
//================================================================
Player* coop_returnPlayerFavored( void )
{
	int i;
	Entity *currentEntity = NULL;
	Player *player = NULL;
	Player *playerFirstValid = NULL;
	Player *playerNoSpectator = NULL;


	//hzm coop mod chrissstrahl - return in sp directly
	if ( g_gametype->integer == GT_SINGLE_PLAYER )
	{ 
		currentEntity = g_entities[0].entity;
		if ( currentEntity && currentEntity->isClient() && currentEntity->isSubclassOf( Player ))
		{
			player = ( Player * )currentEntity;
			return player;
		}
		return NULL;
	}

	for ( i = 0; i < maxclients->integer; i++ )
	{
		player = (Player *)g_entities[i].entity;
		if ( player && player->client )
		{
			//multiplayer, so be picky, we prefer active players
			if ( playerFirstValid == NULL )
			{
				playerFirstValid = player;
			}
			//see if player is NOT a spectator
			if ( !multiplayerManager.isPlayerSpectator( player ) )
			{
				if ( playerNoSpectator == NULL )
				{
					playerNoSpectator = player;
				}
				//perfect, not spec and not dead, return imidatly
				if ( player->deadflag == DEAD_NO )
				{
					return player;
				}
			}
		}
	}
	if ( playerNoSpectator )
		return playerNoSpectator;
	if ( playerFirstValid )
		return playerFirstValid;

	return NULL;
}


//================================================================
// Name:        coop_returnPlayerInCallvolumeClosestTo
// Class:       -
//              
// Description:  returns a player closest to given entity while the player is inside a callvolume trigger
//              
// Parameters:   Entity *eMe
//              
// Returns:     Player*
//              
//================================================================
Player* coop_returnPlayerInCallvolumeClosestTo( Entity *eMe )
{
	Player *player = NULL;
	Entity *currentEntity = NULL;

	//hzm coop mod chrissstrahl - return in sp directly

	//hzm coop mod chrissstrahl - return in sp directly
	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		currentEntity = g_entities[0].entity;

		if ( currentEntity && currentEntity->isClient() && currentEntity->isSubclassOf( Player ) ){
			player = ( Player * )currentEntity;
			return player;
		}
	}

	if ( !eMe ){
		gi.Printf( "coop_returnPlayerInCallvolumeClosestTo(*e):: *e does not exist\n");
		return NULL;
	}

	//hzm coop mod chrissstrahl - get the closest player that actually is inside a callvolume and alive, everything else is not acceptable
	int iPlayer;
	Player *currentPlayer = NULL;
	Player *comparePlayer = NULL;
	Player *closestPlayer = NULL;
	Player *closestPlayerPreviouseTrigger = NULL;
	Entity *trigger = G_FindClass( NULL , "trigger_volume_callvolume" );

	Vector eMe_origin = eMe->origin;
	Vector clostestPlayerDist;
	Vector closestPlayerPreviouseTriggerDist;

	while ( trigger != NULL )
	{
		for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
		{
			currentPlayer = multiplayerManager.getPlayer( iPlayer );
			//see if this player is a valid starting point
			if ( currentPlayer && coop_checkIsPlayerActiveAliveInBoundingBox( currentPlayer , trigger ) )
			{
				//remember this player
				closestPlayer = currentPlayer;

				//this player is valid, compare him to all other valid players
				int iCompare;
				clostestPlayerDist = currentPlayer->origin - eMe_origin;
				Vector distCompare;

				for ( iCompare = (iPlayer + 1); iCompare < maxclients->integer; iCompare++ )
				{
					comparePlayer = multiplayerManager.getPlayer( iPlayer );

					if ( comparePlayer && coop_checkIsPlayerActiveAliveInBoundingBox( comparePlayer , trigger ) )
					{
						distCompare = comparePlayer->origin - eMe_origin;

						if ( distCompare.length() < clostestPlayerDist.length() )
						{
							//make currently processed closest player 'closestPlayer'
							closestPlayer = comparePlayer;
							clostestPlayerDist = comparePlayer->origin - eMe_origin;
						}
					}
				}
				break;
			}
		}
		//compare if current closest player is closer than the closest player in the previouse callvolume trigger
		if ( closestPlayerPreviouseTrigger && closestPlayer && closestPlayer != closestPlayerPreviouseTrigger ){
			if ( clostestPlayerDist.length() < closestPlayerPreviouseTriggerDist.length() ){
				closestPlayerPreviouseTriggerDist = clostestPlayerDist;
				closestPlayerPreviouseTrigger = closestPlayer;
			}
		}
		else{
			closestPlayerPreviouseTrigger = closestPlayer;
			closestPlayerPreviouseTriggerDist = clostestPlayerDist;
		}
		trigger = G_FindClass( trigger , "trigger_volume_callvolume" );
	}

	//return closest player if successfull
	if ( closestPlayerPreviouseTrigger ){
		return closestPlayerPreviouseTrigger;
	}

	return NULL;
}


//================================================================
// Name:        coop_returnPlayerClosestTo
// Class:       -
//              
// Description:  returns the player closest to given entity, preferably not dead and active - this needs also to work during SINGLEPLAYER!
//              
// Parameters:  Entity *eMe
//              
// Returns:     Player*
//              
//================================================================
Player* coop_returnPlayerClosestTo( Entity *eMe )
{
	Player *player = NULL;
	Entity *currentEntity = NULL;

	//hzm coop mod chrissstrahl - return in sp directly
	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		currentEntity = g_entities[0].entity;

		if ( currentEntity && currentEntity->isClient() && currentEntity->isSubclassOf( Player ) ){
			player = ( Player * )currentEntity;
			return player;
		}
	}

	int i;

	int iClosestNoSpecAliveNotOrigin = 99999;
	int iClosestNoSpecNotOrigin = 99999;
	int iClosestNotOrigin = 99999;

	int iClosestNoSpecAlive = 99999;
	int iClosestNoSpec = 99999;
	int closestDistance = 99999;

	Player *closestNoSpecAliveNotOrigin = NULL;
	Player *closestNoSpecNotOrigin = NULL;
	Player *closestNotOrigin = NULL;

	Player *closestNoSpecAlive = NULL;
	Player *closestNoSpec = NULL;
	Player *closest	= NULL;
	Player *temp	= NULL;

	Vector dir;

	for ( i = 0; i < maxclients->integer; i++ )
	{
		if ( &g_entities[i] && g_entities[i].client && g_entities[i].inuse && g_entities[i].entity )
		{
			temp = (Player*)g_entities[i].entity;

			dir = temp->origin - eMe->origin;

			//not eMe
			//closer than last
			//no spectator
			//is alive
			if ( eMe->entnum != temp->entnum && dir.length() < iClosestNoSpecAliveNotOrigin && temp->health >= 1 && !multiplayerManager.isPlayerSpectator( temp ) )
			{
				closestNoSpecAliveNotOrigin = temp;
				iClosestNoSpecAliveNotOrigin = dir.length();
			}

			//closer than last
			//no spectator
			//is alive
			if ( dir.length() < iClosestNoSpecAlive && temp->health >= 1 && !multiplayerManager.isPlayerSpectator( temp ) )
			{
				closestNoSpecAlive = temp;
				iClosestNoSpecAlive = dir.length();
			}

			//not eMe
			//closer than last
			//no spectator
			if ( eMe->entnum != temp->entnum && dir.length() < iClosestNoSpec && !multiplayerManager.isPlayerSpectator( temp ) )
			{
				closestNoSpecNotOrigin = temp;
				iClosestNoSpecNotOrigin = dir.length();
			}

			//closer than last
			//no spectator
			if ( dir.length() < iClosestNoSpec && !multiplayerManager.isPlayerSpectator( temp ) )
			{
				closestNoSpec = temp;
				iClosestNoSpec = dir.length();
			}

			//not eMe
			//closer than last
			if ( eMe->entnum != temp->entnum && dir.length() < closestDistance )
			{
					closestNotOrigin = temp;
					iClosestNotOrigin = dir.length();
			}

			//closer than last
			if ( dir.length() < closestDistance )
			{
				closestDistance = dir.length();
				closest = temp;
			}
		}
	}

	//return a player based on pereference
	if ( closestNoSpecAliveNotOrigin )	return closestNoSpecAliveNotOrigin;
	if ( closestNoSpecNotOrigin )		return closestNoSpecNotOrigin;
	if ( closestNotOrigin )				return closestNotOrigin;

	if ( closestNoSpec )				return closestNoSpec;
	if ( closestNoSpecAlive )			return closestNoSpecAlive;
	if ( closest )						return closest;

	return coop_returnPlayerFavored();
}


//================================================================
// Name:        coop_returnPlayerQuantity
// Class:       -
//              
// Description:  returns how many players are currently on the server
//              
// Parameters:  void
//              
// Returns:     int
//              
//================================================================
int coop_returnPlayerQuantity(void)
{
	return coop_returnPlayerQuantity(0);
}
//================================================================
// Name:        coop_returnPlayerQuantityInArena
// Class:       -
//              
// Description:  returns how many players are currently actively playing, meaning not counting spectators
//              
// Parameters:  void
//              
// Returns:     int
//              
//================================================================
int coop_returnPlayerQuantityInArena( void )
{
	return coop_returnPlayerQuantity( 1 );
}
//================================================================
// Name:        coop_returnPlayerQuantity
// Class:       -
//              
// Description: returns how many players are currently on the server
//				COndition can be: 0=all,1=nospec,2=nospec+alive,3=nobots
//              
// Parameters:  short condition
//              
// Returns:     int
//              
//================================================================
int coop_returnPlayerQuantity( short condition )
{
	Player *player;

	//hzm coop mod chrissstrahl - return in sp directly
	if ( g_gametype->integer == GT_SINGLE_PLAYER ) {
		player = ( Player * )g_entities[0].entity;
		if ( player ) {
			return 1;
		}
		return 0;
	}

	int iQuantity = 0;
	for (int i = 0; i < maxclients->integer; i++ ) {
		player = ( Player * )g_entities[i].entity;
		if ( player && player->client && player->isSubclassOf( Player ) ) {
			if ( condition == 1 ) {
				if ( !multiplayerManager.isPlayerSpectator( player ) ) {
					iQuantity++;
				}
			}
			else if ( condition == 2 ) {
				if ( player->health > 0 && !multiplayerManager.isPlayerSpectator( player ) ) {
					iQuantity++;
				}
			}
			//[b607] chrissstrahl - filter bots option
			else if ( condition == 3 ) {
				gentity_t *ent = player->edict;
				if (!(ent->svflags & SVF_BOT) ) { //[b610] chrissstrahl - fixed logic error only counting bots
					iQuantity++;
				}
			}
			else {
				iQuantity++;
			}
		}
	}
	return iQuantity;
}

//================================================================
// Name:        coop_returnCvarVector
// Class:       -
//              
// Description:  returns vector from given cvar
//              
// Parameters:  str cvarName
//              
// Returns:     Vector
//              
//================================================================
Vector coop_returnCvarVector( str cvarName )
{
//exit if string is empty
	if ( strlen( cvarName ) < 1 )return Vector( 0.0f , 0.0f , 0.0f );
		

//exit if cvar does NOT exist or is empty
	cvar_t *cvar = gi.cvar_get( cvarName.c_str() );
	if ( !cvar || !strlen( cvar->string ) )return Vector( 0.0f , 0.0f , 0.0f );

	return coop_returnVectorFromString( va("%s" , cvar->string) );
}

//================================================================
// Name:        coop_returnCvarInteger
// Class:       -
//              
// Description:  returns integer from given cvar
//              
// Parameters:  str cvarName
//              
// Returns:     int
//              
//================================================================
int coop_returnCvarInteger( str cvarName )
{
	//[b60011] Chrissstrahl - fixed bad check of cvar
	if (strlen(cvarName) < 1) {
		gi.Printf("coopReturn.cpp - coop_returnCvarInteger() - Empty CVar Name given!\nCausing function to return -1\n");
		return -1;
	}
	
	cvar_t* cvar = gi.cvar_get(cvarName.c_str());
	if (!cvar || !strlen(cvar->string)) {
		gi.Printf(va("coopReturn.cpp - coop_returnCvarInteger(%s) - CVar did not Exist or was empty!\nCausing function to return -1\n", cvarName.c_str())); //[b60014] chrissstrahl - added cvar name
		return -1;
	}

	return gi.Cvar_VariableIntegerValue(cvarName.c_str());
}


//================================================================
// Name:        coop_returnCvarString
// Class:       -
//              
// Description:  returns integer from given cvar
//              
// Parameters:  str cvarName
//              
// Returns:     str
//              
//================================================================
str coop_returnCvarString( str cvarName )
{
	if ( strlen( cvarName ) < 1 )
		return "";

	cvar_t *value;
	str variableValue;
	value = gi.cvar( cvarName.c_str() , "" , 0 );

	variableValue = value->string;

	if ( strlen( variableValue.c_str() ) < 1 )
		return "";

	return variableValue.c_str();
}

//================================================================
// Name:        coop_returnEntityAttachedByTargetname
// Class:       -
//              
// Description:  returns attached entity by its targetname from given Entity 
//              
// Parameters:  Entity *eParent , str attachedTargetname
//              
// Returns:     Entity *
//              
//================================================================
Entity * coop_returnEntityAttachedByTargetname( Entity *eParent , str attachedTargetname )
{
	if ( !eParent || attachedTargetname.length() < 1 )
		return NULL;

	int num;
	int i;
	Entity *eChild;
	

	if ( eParent->bind_info )
	{
		num = eParent->bind_info->numchildren;

		for ( i = 0; ( i < MAX_MODEL_CHILDREN ) && num; i++ )
		{
			if ( eParent->bind_info->children[i] == ENTITYNUM_NONE )
				continue;

			eChild = ( Entity * )G_GetEntity( eParent->bind_info->children[i] );

			if ( eChild && stricmp( eChild->targetname , attachedTargetname.c_str() ) == 0 )
			{
				return eChild;
			}

			num--;
		}
	}
	return NULL;
}


//================================================================
// Name:        coop_returnOwnerEntity
// Class:       -
//              
// Description:  returns owner entity from given object (could be player or actor, script entiy or world)
//              
// Parameters:  Entity* entity
//              
// Returns:     Entity *
//              
//================================================================
Entity * coop_returnOwnerEntity( Entity* entity )
{
	if ( !entity )
		return NULL;

	Entity* eReturn = NULL;
	if ( entity->isSubclassOf(Projectile) )
	{
		Projectile *projectile = ( Projectile * )entity;
		eReturn = projectile->getOwner();

		if ( !eReturn )
			return NULL;
	}
	else if ( entity->isSubclassOf(Equipment) )
	{
		Equipment* equipment = ( Equipment * )entity;
		Sentient* sentientOwener = equipment->GetOwner();

		if ( !sentientOwener )
			return NULL;

		eReturn = (Entity* )sentientOwener;
	}
	else
	{
		eReturn = g_entities[entity->edict->ownerNum].entity;
	}


	if ( !eReturn )
		eReturn = ( Entity* )entity;
	
	return eReturn;
}


//================================================================
// Name:        coop_returnOwnerPlayer
// Class:       -
//              
// Description:  returns owner of entity only if it is a player
//              
// Parameters:  Entity* entity
//              
// Returns:     Player *
//              
//================================================================
Player * coop_returnOwnerPlayer( Entity* entity )
{
	Entity* eOwner;
	eOwner = coop_returnOwnerEntity( entity );
	
	if ( !eOwner || !eOwner->isSubclassOf( Player ) || !eOwner->isClient() )
		return NULL;

	return (Player *)eOwner;
}

//================================================================
// Name:        coop_returnObjectivesCvarName
// Class:       -
//              
// Description:  returns cvar for the given objectives item number, range: 1-8 (- 11)
//              
// Parameters:  int iObjectivesItem
//              
// Returns:     str
//              
//================================================================
str coop_returnObjectivesCvarName( int iObjectivesItem )
{
	if ( iObjectivesItem < 2 ){
		iObjectivesItem = 1;
	}else if ( iObjectivesItem > 11 ){
		iObjectivesItem = 11;
	}

	//construct cvar
	str sCvar;
	switch ( iObjectivesItem )
	{
	case 1:
		sCvar = "coop_one";
		break;
	case 2:
		sCvar = "coop_two";
		break;
	case 3:
		sCvar = "coop_three";
		break;
	case 4:
		sCvar = "coop_four";
		break;
	case 5:
		sCvar = "coop_five";
		break;
	case 6:
		sCvar = "coop_six";
		break;
	case 7:
		sCvar = "coop_seven";
		break;
	case 8:
		sCvar = "coop_eight";
		break;
	case 9:
		sCvar = "coop_t1";
		break;
	case 10:
		sCvar = "coop_t2";
		break;
	default:
		sCvar = "coop_t3";
		break;
	}
	return sCvar;
}


//================================================================
// Name:        coop_returnFloatFromString
// Class:       -
//              
// Description:  returns first float or integer value from given string
//              
// Parameters:  str source
//              
// Returns:     float
//              
//================================================================
float coop_returnFloatFromString( const str extrapolate )
{
	//accept only good food
	if ( !extrapolate.length() ) {
		return 0.0f;
	}

	char current;
	bool containsPeriod = false;
	bool containsDigit = false;
	str sConstruct = "";


	for ( int i = 0; i < extrapolate.length(); i++ ) {
		current = extrapolate[i];

		//accept only digets and period, to construct a float
		if ( !isdigit( current ) && current != '.' )
			continue;

		//don't want ..... to hapen, accept only 1 period
		if ( current == '.' ) {
			//we have something like 1.00. or .12434.
			if ( containsPeriod && containsDigit )
				break;

			//we have something like ..
			if ( containsPeriod )
				sConstruct = "";

			//keep track
			containsPeriod = true;
		}
		else {
			containsDigit = true;
		}

		//construct new value
		sConstruct += current;
	}
	//make sure we get a good float
	if ( sConstruct[sConstruct.length() - 1] == '.' )
		sConstruct += "0";

	return atof( sConstruct.c_str() );
}

//================================================================
// Name:        coop_returnLevelType
// Class:       -
//              
// Description:  checks if the given map is known/included and sets the type of the map
//              
// Parameters:  str sLevelname, bool &standard, int type
//              
// Returns:     bool
//              
//================================================================
bool coop_returnLevelType( str sLevelname, bool &standard, int &type )
{
	//[b60012][cleanup] - all the static ini entries could be loaded into container list and read from there
	//There should be a solid structure, maybe have a few experiments with that first where each section is a sublist or something?
	// 
	//multiplayer=0,mission=1,igm=2,secret=3,customcoop=4,notsupported=5
	type = MAPTYPE_MULTIPLAYER;
	standard = false;

	//check if it is a multiplayer map - it is faster if we can check the name prefix first - reduce file read
	if (Q_stricmpn(sLevelname.c_str(), "dm_", 3) == 0 ||
		Q_stricmpn(sLevelname.c_str(), "aah_", 4) == 0 ||
		Q_stricmpn(sLevelname.c_str(), "ctf_", 4) == 0 ||
		Q_stricmpn(sLevelname.c_str(), "hm_", 3) == 0) {
		//check if it is a standard multiplayer map
		if (coop_parserIsItemInCategory("maplist.ini", sLevelname, "multiplayer")) {
			type = MAPTYPE_MULTIPLAYER;
			return true;
		}
	}

	//check if it is a custom coop (or testmap) map, it is faster if we can check the name prefix first - reduce file read
	if (Q_stricmpn( sLevelname.c_str() , "coop_", 5 ) == 0 ||
		Q_stricmpn( sLevelname.c_str() , "sp_" , 4 ) == 0 || //new added support for singleplayer maps
		Q_stricmpn( sLevelname.c_str() , "prf_" , 4 ) == 0 ||
		Q_stricmpn( sLevelname.c_str() , "rpg_" , 4 ) == 0 )
	{

		//[b60013] chrissstrahl - start RPG_ Maps handle
		if (Q_stricmpn(sLevelname.c_str(), "rpg_", 4) == 0) {
			type = MAPTYPE_RPG;
			coopRpg.init();
		}
		else {
			type = MAPTYPE_CUSTOM;
		}

		if ( coop_parserIsItemInCategory( "maplist.ini" , sLevelname , "coopincluded" ) ) {
			return true;
		}

		//if map is not listed, it is not known, and mostlikley not provided with the coop mod
		return false;
	}

	//check if it is a igm map - it is faster if we can check the name prefix first - reduce file read
	if (Q_stricmpn( sLevelname.c_str() , "ent-deck" , 8 ) == 0 ) {
		//check if it is a coop map - igm
		if ( coop_parserIsItemInCategory( "maplist.ini" , sLevelname , "singleplayerigm" ) ) {
			type = MAPTYPE_IGM;
			standard = true;
			return true;
		}	
	}
	//check if it is a coop map - this is most likley
	//check if it is a igm map - it is faster if we can check the name prefix first - reduce file read
	if (Q_stricmpn( sLevelname.c_str() , "m" , 1 ) == 0 &&
		coop_returnIntFind( sLevelname , "-" ) != -1 ||
		Q_stricmpn( sLevelname.c_str() , "credits" , 7 ) == 0 ) {
		
		if ( coop_parserIsItemInCategory( "maplist.ini" , sLevelname , "singleplayermission" ) ) {
			type = MAPTYPE_MISSION;
			standard = true;
			return true;
		}
	}

	//check if it is a coop map - secret or training maps
	if ( coop_parserIsItemInCategory( "maplist.ini" , sLevelname , "singleplayersecret" ) ) {
		type = MAPTYPE_SECRET;
		standard = true;
		return true;
	}
	//check if it is a not supported sp map - check this at the very end, because it is the most unlikly
	if ( coop_parserIsItemInCategory( "maplist.ini" , sLevelname , "singleplayer" ) ) {
		type = MAPTYPE_MULTIPLAYER;
		standard = true;
		return true;
	}
	return false;
}

//[b60011] chrissstrahl - frequently needed, couldn't find them in code but was sure they did exist
//================================================================
// Name:        coop_returnFloatNotNegative
// Class:       -
//              
// Description:  returns given float and makes sure it is not negative
//              
// Parameters:  float
//              
// Returns:     float
//              
//================================================================
float coop_returnFloatNotNegative(float fLt)
{
	if (fLt < 0) {
		return (fLt * -2);
	}
	return fLt;
}

//[b60011] chrissstrahl
//================================================================
// Name:        coop_returnIntegerNotNegative
// Class:       -
//              
// Description:  returns given int and makes sure it is not negative
//              
// Parameters:  int
//              
// Returns:     int
//              
//================================================================
int coop_returnIntegerNotNegative(int iNt)
{
	if (iNt < 0) {
		return (iNt * -2);
	}
	return iNt;
}

