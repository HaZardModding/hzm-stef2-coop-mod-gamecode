//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// Contains text/strings related code for the gameupgrades
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"

#include "upgStrings.hpp"
#include "upgCoopInterface.hpp"

UpgStrings upgStrings;

//================================================================
// Name:        replace
// Class:       UpgStrings
//              
// Description:	replace the given string inside the string
//              
// Parameters:  str sText, str sFind , sReplace
//              
// Returns:     bool          
//================================================================
bool UpgStrings::replace(str& sHeystack, const str& sFind, const str& sNedle)
{
	//if sFind empty or heystack smaller than sFind, burn heystack
	if (strlen(sFind) < 1 || strlen(sHeystack) < strlen(sFind))
		return false;

	int iWater = containsAt(sHeystack, sFind);

	//found no sFind, imidatly plug it
	if (iWater < -1)
		return false;

	str sLighter = sHeystack;
	manipulateFromWithLength(sLighter, 0, (iWater - 1));
	sLighter += sNedle;
	sLighter += getStartingFrom(sLighter, ((iWater - 1) + strlen(sFind)));

	//be manipulative
	sHeystack = sLighter;
	return true;
}

//================================================================
// Name:        containsAt
// Class:       UpgStrings
//              
// Description:	return position at which given string was found otherwise return -1
//              
// Parameters:  str sSource, str sKeyword
//              
// Returns:     int       
//================================================================
int UpgStrings::containsAt(str sSource, str sKeyword)
{
	//[b60012] chrissstrahl - fix missing .c_str()
	if (Q_stricmp(sKeyword.c_str(), "") == 0 || Q_stricmp(sSource.c_str(), "") == 0){ return -1; }
	str ss, sk;
	ss = sSource;
	sk = sKeyword;
	ss = ss.tolower();
	sk = sk.tolower();

	int iCurrtrentMatchIndex = 0;
	int iFoundAt = -1;

	for (int i = 0; i < strlen(ss); i++){
		//check every char from the MATCH-LIST
		if (ss[i] == sk[iCurrtrentMatchIndex]) {
			if (iCurrtrentMatchIndex == 0){
				iFoundAt = i;
			}

			iCurrtrentMatchIndex++;

			if (iCurrtrentMatchIndex == strlen(sk)) {
				break;
			}
		}
		else {
			iCurrtrentMatchIndex = 0;
			iFoundAt = -1;
		}
	}

	//make sure we don't get any odd behaviour
	if (iCurrtrentMatchIndex < sk.length()) {
		iCurrtrentMatchIndex = 0;
		iFoundAt = -1;
	}

	//gi.Printf( va( "FOUND::::%d\n" , iFoundAt ) );
	return iFoundAt;
}


//================================================================
// Name:        manipulateFromWithLength
// Class:       UpgStrings
//              
// Description:  returns a string starting from given index with given length
//				 Replacement for substr
//              
// Parameters:  str sString , int iStart, int iEnd
//              
// Returns:     void           
//================================================================
void UpgStrings::manipulateFromWithLength(str& sString, const int& iStart, int iEnd)
{
	const int iLength = sString.length();
	str sPartial = sString;
	sString = "";
	if (iLength > iStart){
		if (iLength < (iStart + iEnd)){
			iEnd = (iLength - iStart);
		}

		for (int i = iStart; i < (iStart + iEnd); i++){
			sString += sPartial[i];
		}
	}
	else{
		throw("Contact HZM with this info data: COOP PROGRAMMING ERROR IN: _returnStringFromWithLength ERROR INFORMATION READS: start pos > then strlen");
	}
}

//================================================================
// Name:        getStartingFrom
// Class:       UpgStrings
//              
// Description:  returns a string starting from given index
//              
// Parameters:  str sString , int iStart
//              
// Returns:     str           
//================================================================
str UpgStrings::getStartingFrom(const str& sString, const int& iStart)
{
	str sPartial = "";
	if (sString && strlen(sString) > iStart) {
		for (int i = iStart; i < strlen(sString); i++) {
			sPartial += sString[i];
		}
	}
	return sPartial;
}

//================================================================
// Name:        getStartingFromUntil
// Class:       UpgStrings
//              
// Description:	Returns a string starting from, until - do not confuse this till with length!
//              
// Parameters:  str sText , int iStart , int iMax
//              
// Returns:     str     
//================================================================
str UpgStrings::getStartingFromUntil(str sText, int iStart, int iMax)
{
	if (!sText || sText.length() < 1) return "";
	if (iMax > sText.length()) iMax = sText.length();

	if (iStart > iMax) {
		gi.Printf(va("upgStrings.getStartingFromUntil: ERROR in values: start[%i] max[%i]\n", iStart, iMax));
		return "";
	}

	int					i;
	str					sNew = "";
	for (i = iStart; i < iMax; i++) {
		sNew += sText[i];
	}
	return sNew;
}

//================================================================
// Name:        getUntilChar
// Class:       UpgStrings
//              
// Description:	returns the given string until a given-char occours
//              
// Parameters:  str sChain
//              
// Returns:     str  
//================================================================
str UpgStrings::getUntilChar(str sChain,str sUntil)
{
	if (!sChain) return "";

	int i;
	str sPart = "";
	for (i = 0; i < sChain.length(); i++) {
		if (sChain[i] == sUntil[0]) {
			return sPart;
		}
		else {
			sPart += sChain[i];
		}
	}
	return sPart;
}

//================================================================
// Name:        coop_textCleanAllButLetters
// Class:       -
//              
// Description:		removes the everything but the actual letters from string
//              
// Parameters:  str sText
//              
// Returns:     str
//================================================================
str UpgStrings::getLettersOnly(str sText)
{
	//filter all but letters
	static const char	validChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	int					i, j;
	bool				isValidChar;
	str					sNew = "";

	for (i = 0; i < sText.length(); i++) {
		isValidChar = false;
		for (j = 0; validChars[j]; j++) {
			if (sText[i] == validChars[j]) { isValidChar = true; }
		}

		if (!isValidChar) {
			sNew += "";
		}
		else {
			sNew += sText[i];
		}
	}
	return sNew;
}

//================================================================
// Name:        getLettersOnlyAsLowercase
// Class:       UpgStrings
//              
// Description:	removes everything but the actual letters from string, transforms to lowercase
//              
// Parameters:  str
//              
// Returns:     str
//================================================================
str UpgStrings::getLettersOnlyAsLowercase(str sText)
{
	//filter all but letters
	static const char	validChars[] = "abcdefghijklmnopqrstuvwxyz";
	int					i, j;
	bool				isValidChar;
	str					sNew = "";

	sText = sText.tolower();

	for (i = 0; i < sText.length(); i++) {
		isValidChar = false;
		for (j = 0; validChars[j]; j++) {
			if (sText[i] == validChars[j]) { isValidChar = true; }
		}

		if (!isValidChar) {
			sNew += "";
		}
		else {
			sNew += sText[i];
		}
	}
	return sNew;
}


//================================================================
// Name:        getReplacedSpaceWithUnderscore
// Class:       UpgStrings
//              
// Description:		replaces the current string after all whithespaces have been replaced
//					used for locationprint strings and menu related stuff (Coop communicator Transporetr Menu)
//              
// Parameters:  str
//              
// Returns:     str        
//================================================================
str UpgStrings::getReplacedSpaceWithUnderscore(str sText)
{
	int		i;
	str		sNewText = "";

	for (i = 0; i < sText.length(); i++) {
		if (sText[i] == ' ') {
			sNewText += "_";
		}
		else {
			sNewText += sText[i];
		}
	}

	return sNewText;
}

//================================================================
// Name:        getReplacedSpaceWithUnderscoreBlack
// Class:       UpgStrings
//              
// Description:	replaces the current string after all whithespaces have been replaced
//				used for locationprint strings
//              
// Parameters:  str
//              
// Returns:     str         
//================================================================
str UpgStrings::getReplacedSpaceWithUnderscoreBlack(str sText)
{
	int		i;
	str		sNewText = "";

	for (i = 0; i < sText.length(); i++) {
		if (sText[i] == ' ') {
			sNewText += "^0_^8";
		}
		else {
			sNewText += sText[i];
		}
	}

	return sNewText;
}

//================================================================
// Name:        upgStrings.getReplacedUmlaute
// Class:       -
//              
// Description: replaces umlaute with ASCII compatible letters,
//				umlaute are filtred in net code and would not make it to the client otherwise
//
// Parameters:  Player* player, str sText 
//              
// Returns:     void
//              
//================================================================
str UpgStrings::getReplacedUmlaute(str sText) { return getReplacedUmlaute(nullptr, sText); }
str UpgStrings::getReplacedUmlaute(Player* player, str sText)
{
	//WARNING, this has a bug, if a umlaut is followed by another, they will not work and $$$ is shown
	int i;
	str sNewText = "";
	str sCoop = "";
	str sNoCoop = "";
	int iIndex = 0;

	for (i = 0; i < sText.length(); i++) {
		//if we found something like $$o$$, we will have to skip $o$$
		if (iIndex > i) {
			continue;
		}

		bool bAddE = true;
		iIndex = i;

		//search for something like $$o$$
		if (sText[i] == '$' && (i + 4) < sText.length()) {
			bAddE = false;
			if (sText[i + 1] == '$' && sText[i + 3] == '$' && sText[i + 4] == '$') {
				if (sText[i + 2] == 'a' ||
					sText[i + 2] == 'A' ||
					sText[i + 2] == 'o' ||
					sText[i + 2] == 'O' ||
					sText[i + 2] == 'u' ||
					sText[i + 2] == 'U' ||
					sText[i + 2] == 'S')
				{
					bAddE = true;
					str conv = sText[i + 2];
					sCoop = va("$$%s$$", conv.c_str());
					sNoCoop = va("%se", conv.c_str());
					iIndex = (i + 5);
				}
			}
		}
		//search for umlauts
		else if (sText[i] == 'ä') {
			sCoop = "$$a$$"; sNoCoop = "ae";
		}
		else if (sText[i] == 'Ä') {
			sCoop = "$$A$$"; sNoCoop = "AE";
		}
		else if (sText[i] == 'ö') {
			sCoop = "$$o$$"; sNoCoop = "oe";
		}
		else if (sText[i] == 'Ö') {
			sCoop = "$$O$$"; sNoCoop = "OE";
		}
		else if (sText[i] == 'ü') {
			sCoop = "$$u$$"; sNoCoop = "ue";
		}
		else if (sText[i] == 'Ü') {
			sCoop = "$$U$$"; sNoCoop = "UE";
		}
		else if (sText[i] == 'ß') {
			sCoop = "$$S$$"; sNoCoop = "Sz";
		}
		//no match, just regular text
		else {
			bAddE = false;
		}

		if (bAddE) {
			//coop mod installed
			if (player && upgCoopInterface.playerHasCoop(player)) {
				sNewText += sCoop;
			}//no coop mod
			else {
				sNewText += sNoCoop;
			}
		}
		else {
			sNewText += sText[i];
		}
	}
	return sNewText;
}


//========================================================[b60011]
// Name:        returnForLabeltext
// Class:       UpgStrings
//              
// Description:  Replaces withespace and newline with labeltext compatible chars, which are handled client side
//              
// Parameters:  str sString , int iStart
//              
// Returns:     str           
//================================================================
str UpgStrings::getReplacedForLabeltext(str sPure)
{
	int i = 0;
	//[b60014] chrissstrahl - fixed "labeltext^" issue
	if (strlen(sPure) > 10) {
		str sTemp = sPure;
		sTemp = sTemp.tolower();
		if (upgStrings.containsAt(sTemp, "labeltext ") != -1) {
			i = 11;
		}
	}

	for (i = i; i < strlen(sPure); i++) {
		if (sPure[i] == '\n' || sPure[i] == '#')
			sPure[i] = '~';
		if (sPure[i] == ' ')
			sPure[i] = '^';
	}
	return sPure;
}

//================================================================
// Name:        manipulateTrim
// Class:       UpgStrings
//              
// Description:  trim given chars (remove them if they are at the end or the beginning of that string)
//              
// Parameters:  str sBase, str sTrimChars
//              
// Returns:     void           
//================================================================
void UpgStrings::manipulateTrim(str& sTrim, const str& sTrimMatch)
{
	str sTemp = "";
	bool b;
	bool bStarted = false;

	//check every char from the MATCH-LIST
	for (int i = 0; i < sTrim.length(); i++) {
		b = true;
		for (int j = 0; j < strlen(sTrimMatch); j++) {
			if (sTrim[i] == sTrimMatch[j])
			{
				b = false;
				break;
			}
		}

		//the actual word we want to keep started
		if (!bStarted && b)
		{
			bStarted = true;
		}

		//combine word
		if (b || bStarted)
		{
			sTemp += sTrim[i];
		}
	}

	//clear var
	sTrim = "";
	bStarted = false;

	//check every char from the MATCH-LIST
	for (int i = (sTemp.length() - 1); i > -1; i--) {
		b = true;
		for (int j = 0; j < strlen(sTrimMatch); j++) {
			if (sTemp[i] == sTrimMatch[j])
			{
				b = false;
				break;
			}
		}

		//the actual word we want to keep started
		if (!bStarted && b)
		{
			bStarted = true;
		}

		//combine word
		if (b || bStarted)
		{
			sTrim += sTemp[i];
		}
	}

	//reset var
	sTemp = "";

	//reverse the shitt again
	for (int i = (sTrim.length() - 1); i > -1; i--) {
		sTemp += sTrim[i];
	}

	sTrim = sTemp;
}

//================================================================
// Name:        returnTrimmed
// Class:       UpgStrings
//              
// Description:  trim given chars (remove them if they are at the end or the beginning of that string)
//              
// Parameters:  str sBase, str sTrimChars
//              
// Returns:     str sTrim           
//================================================================
str UpgStrings::getTrimmed(str sTrim, const str sTrimMatch)
{
	upgStrings.manipulateTrim(sTrim, sTrimMatch);
	return sTrim;
}

//================================================================
// Name:        substr
// Class:       UpgStrings
//              
// Description:  returns the specified part of a string
//              
// Parameters:  str sString, const int& iStart, int iEnd
//              
// Returns:     str           
//================================================================
str UpgStrings::getSubStr(str sString, const int& iStart, int iEnd)
{
	manipulateFromWithLength(sString,iStart,iEnd);
	return sString;
}

//================================================================
// Name:        getFileName
// Class:       UpgStrings
//              
// Description:  returns the filename of a given path with file string
//              
// Parameters:  str sPath
//              
// Returns:     str             
//================================================================
str UpgStrings::getFileName(const str& sPath)
{
	int i;
	str fileName = "";
	for (i = 0; i < sPath.length(); i++) {
		if (sPath[i] == '/' || sPath[i] == '\\') {
			fileName = "";
		}
		else {
			fileName += sPath[i];
		}
	}
	return fileName;
}

//================================================================
// Name:        coop_returnStringFileExtensionOnly
// Class:       UpgStrings
//              
// Description:  returns the fileextension of a given path string
//              
// Parameters:  str sPath
//              
// Returns:     str
//              
//================================================================
str UpgStrings::getFileExtension(const str& sPath)
{
	str fileName = "";
	str sPath2 = sPath;
	sPath2 = sPath2.tolower();
	for (int i = 0; i < strlen(sPath2); i++)
	{
		if (sPath2[i] == '/' || sPath2[i] == '\\' || sPath2[i] == ':' || sPath2[i] == ' ') {
			fileName = "";
		}
		else {
			fileName += sPath2[i];
		}
	}

	str fileName2 = "";
	for (int i = 0; i < strlen(fileName); i++)
	{
		if (fileName[i] == '.') {
			fileName2 = ".";
		}
		else {
			fileName2 += fileName[i];
		}
	}

	if (strlen(fileName2))
	{
		return fileName2;
	}
	return fileName;
}
