#ifndef __COOPALIAS_HPP__
#define __COOPALIAS_HPP__

#include "_pch_cpp.h"

inline int coop_find( str &sSource , str &sKeyword );

inline void coop_trimM( str &sTrim , const str &sTrimMatch );
inline str coop_trim( str sTrim , const str &sTrimMatch );

inline str coop_substrToEnd( const str &sString , const int &iStart );
inline void coop_substrM( str &sString , const int &iStart , int iEnd );
inline str coop_substr( str sString , const int &iStart , int iEnd );

inline str coop_getFileName( const str &sPath );
inline str coop_getFileExtension( const str &sPath );

#endif