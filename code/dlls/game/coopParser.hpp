#ifndef __COOPPHRASER_HPP__
#define __COOPPHRASER_HPP__

#include "_pch_cpp.h"
#include "coopAlias.hpp"

bool coop_parserReadFile( const str sFile , str &buffer );
void coop_parserGetItemsFromCategory( str sFile , ListenKnoten *&start , ListenKnoten *&end , const str section );
bool coop_parserIsItemInCategory( str sFile , const str item , const str section );
int coop_parserGetNumberOfItemsFromCategory( str sFile , const str section );
str coop_parserIniGet( str sFile , const str key , const str section );
bool coop_parserIniSet( str sFile , const str &key , const str &value , const str section );

#endif