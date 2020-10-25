//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER WEAPON/ARMORY RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#include "_pch_cpp.h"

#include "coopCheck.hpp"
#include "coopReturn.hpp"

//////////////////////////////////////////////////////////////////
// CONTAINS ALLIASES TO MAKE FUNCTION NAMES EASIER, because I am
// so sick and tired of not finding what I am loking for
//////////////////////////////////////////////////////////////////

//strcpy( mapName, tempMapName );
//xx.CapLength(int); //cap string in length
//xx.append(str); //ensures the memory is alocated and if it is already it wil waste no more extra
//sLine[0] == ';' ? "^8":"^2"

int coop_find(str &sSource, str &sKeyword ){ return coop_returnIntFind( sSource , sKeyword ); }

void coop_trimM( str &sTrim , const str &sTrimMatch ) { coop_manipulateStringTrim( sTrim , sTrimMatch ); }
str coop_trim( str sTrim , const str &sTrimMatch ) { return coop_returnStringTrim( sTrim , sTrimMatch ); }

str coop_substrToEnd( const str &sString , const int &iStart ) { return coop_returnStringStartingFrom( sString ,iStart );}
void coop_substrM( str &sString , const int &iStart , int iEnd ) { coop_manipulateStringFromWithLength(sString , iStart , iEnd ); }
str coop_substr( str sString , const int &iStart , int iEnd ) { return coop_returnStringFromWithLength( sString , iStart , iEnd ); }

str coop_getFileName(const str &sPath) { return coop_returnStringFilenameOnly( sPath ); }
str coop_getFileExtension(const str &sPath) { return coop_returnStringFileExtensionOnly( sPath ); }