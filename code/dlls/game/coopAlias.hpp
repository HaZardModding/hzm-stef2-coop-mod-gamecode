#ifndef __COOPALIAS_HPP__
#define __COOPALIAS_HPP__

#include "_pch_cpp.h"
#include "coopCheck.hpp"
#include "coopReturn.hpp"

#include "upgStrings.hpp"

inline void coop_trimM(str& sTrim, const str& sTrimMatch)
{
	upgStrings.manipulateTrim(sTrim, sTrimMatch);
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