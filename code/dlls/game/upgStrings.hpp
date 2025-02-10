//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// Contains text/strings related code for the gameupgrades
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"

class UpgStrings
{
public:
	int				contains(str sSource, str sObjectsOfIntrest);
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
	str				getMapName(const str& sMapname); //[b60025] chrissstrahl
	str				getFileNameKeepParameter(const str& sMapname);  //[b60025] chrissstrahl
	str				getFileExtension(const str& sPath);
};

constexpr auto UPG_RECONNECTING_YOU_MODEL_FIX_ENG = "^5Reconnecting you now^8, to fix issues with Models.\n";
constexpr auto UPG_RECONNECTING_YOU_MODEL_FIX_DEU = "^5Sie werden erneut verbunden^8, behebt Model Probleme.\n";

extern UpgStrings upgStrings;