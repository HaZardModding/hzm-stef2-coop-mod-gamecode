#ifndef __COOPALIAS_HPP__
#define __COOPALIAS_HPP__

#include "_pch_cpp.h"
#include "coopCheck.hpp"
#include "coopReturn.hpp"

inline int coop_find(str& sSource, str& sKeyword)
{
	return coop_returnIntFind(sSource, sKeyword);
}

inline int coop_contains(str& sSource, str& sKeyword)
{
	return coop_returnIntFind(sSource, sKeyword);
}

inline void coop_trimM(str& sTrim, const str& sTrimMatch)
{
	coop_manipulateStringTrim(sTrim, sTrimMatch);
}
inline str coop_trim(str sTrim, const str& sTrimMatch)
{
	return coop_returnStringTrim(sTrim, sTrimMatch);
}

inline str coop_substrToEnd(const str& sString, const int& iStart)
{
	return coop_returnStringStartingFrom(sString, iStart);
}

inline void coop_substrM(str& sString, const int& iStart, int iEnd)
{
	coop_manipulateStringFromWithLength(sString, iStart, iEnd);
}

inline str coop_substr( str sString , const int &iStart , int iEnd )
{
	return coop_returnStringFromWithLength(sString, iStart, iEnd);
}

inline str coop_getFileName( const str &sPath )
{ 
	return coop_returnStringFilenameOnly(sPath);
}

inline str coop_getFileExtension( const str &sPath )
{
	return coop_returnStringFileExtensionOnly(sPath);
}

#endif