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
	str				returnForLabeltext(str sPure);
	void			manipulateTrim(str& sTrim, const str& sTrimMatch);
	str				returnTrimmed(str sTrim, const str sTrimMatch);
};

extern UpgStrings upgStrings;

#endif /* upgStrings.cpp */