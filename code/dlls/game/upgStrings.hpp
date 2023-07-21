//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// Contains text/strings related code for the gameupgrades
//-----------------------------------------------------------------------------------

#ifndef __UPGSTRINGS_HPP__
#define __UPGSTRINGS_HPP__

class UpgStrings
{
public:
	bool			replace(str& sHeystack, const str& sDildo, const str& sNedle);
	int				containsAt(str sSource, str sKeyword);
	void			manipulateFromWithLength(str& sString, int const& iStart, int iEnd);
	str				getStartingFrom(const str& sString, const int& iStart);
	str				getStartingFromUntil(str sText, int iStart, int iMax);
	str				getLettersOnly(str sText);
	str				getLettersOnlyAsLowercase(str sText);
	str				getReplacedSpaceWithUnderscore(str sText);
	str				getReplacedSpaceWithUnderscoreBlack(str sText);
	str				returnForLabeltext(str sPure);
	void			manipulateTrim(str& sTrim, const str& sTrimMatch);
	str				returnTrimmed(str sTrim, const str sTrimMatch);
	str				substr(str sString, const int& iStart, int iEnd);
	str				getFileName(const str& sPath);
	str				getFileExtension(const str& sPath);
};

extern UpgStrings upgStrings;

#endif /* upgStrings.cpp */