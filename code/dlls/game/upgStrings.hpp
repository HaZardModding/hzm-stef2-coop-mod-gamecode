//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// Contains text/strings related code for the gameupgrades
//-----------------------------------------------------------------------------------
#pragma once;

#include "_pch_cpp.h"

class UpgStrings
{
public:
	bool			replace(str& sHeystack, const str& sDildo, const str& sNedle);
	int				containsAt(str sSource, str sKeyword);
	str				getStartingFrom(const str& sString, const int& iStart);
	str				getStartingFromUntil(str sText, int iStart, int iMax);
	str				getUntilChar(str sChain, str sUntil);
	str				getLettersOnly(str sText);
	str				getLettersOnlyAsLowercase(str sText);
	str				getReplacedSpaceWithUnderscore(str sText);
	str				getReplacedSpaceWithUnderscoreBlack(str sText);
	str				getReplacedUmlaute(str sText);
	str				getReplacedUmlaute(Player* player, str sText);
	str				getReplacedForLabeltext(str sPure);
	void			manipulateFromWithLength(str& sString, int const& iStart, int iEnd);
	void			manipulateTrim(str& sTrim, const str& sTrimMatch);
	str				getTrimmed(str sTrim, const str sTrimMatch);
	str				getSubStr(str sString, const int& iStart, int iEnd);
	str				getFileName(const str& sPath);
	str				getFileExtension(const str& sPath);
};

extern UpgStrings upgStrings;