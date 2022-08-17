//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING RETURN HANDLINGS RELATED FUNCTIONS (value is given computed here and returned to parrent func) FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#ifndef __COOPRETURN_HPP__
#define __COOPRETURN_HPP__
#include "_pch_cpp.h"

Entity*	coop_returnEntity( const str &name );
str coop_returnBoolWord( int iValue );
str coop_returnBoolWord( bool bValue );
int coop_returnIntWithinOrDefault(const str& sVal, const int& iMin, const int& iMax, const int& iDefault); //[b60011] chrissstrahl - have it also accept strings
float coop_returnFloatWithinOrDefault(const str& sVal, const float& fMin, const float& fMax, const float& fDefault); //[b60011] chrissstrahl - have it also accept strings
float coop_returnFloatOrDefaultFromString( str sSource , float fDefault );
int coop_returnIntOrDefaultFromString( str sSource , int iDefault );
int coop_returnIntWithinOrDefault(const int &iCur, const int &iMin, const int &iMax, const int &iDefault);	//[b607] chrissstrahl -  looks like we need something like that
float coop_returnFloatWithinOrDefault(const float &fCur, const float &fMin, const float &fMax, const float &fDefault);//[b607] chrissstrahl
int coop_returnIntFromString( const str &sSource );
bool coop_returnBool( str sValue );
int coop_returnIntFromFormatedString( str &sSource , const char &cFind );
int coop_returnIntFind( str sSource , str sKeyword );
str coop_returnStringTrim( str sTrim , str sTrimMatch );
void coop_manipulateStringTrim( str &sTrim , const str &sTrimMatch );
str coop_returnStringExtractedCvarValue( str s );
str coop_returnStringCleanedCvarvalue( str s );
str coop_returnStringCleanedCvarvalue( char s );
str coop_returnStringCleanedCvarname( str s );
str coop_returnStringCleanedCvarname( char s );
str coop_returnStringCleanedCvar( char s , bool bIsValue );
str coop_returnStringStartingFrom( const str &sString , const int &iStart );
void coop_manipulateStringFromWithLength( str &sString , int const &iStart , int iEnd );
str coop_returnStringFromWithLength( str sString , int const &iStart , int const &iEnd );
str coop_returnStringSkillname( int iSkill );
float coop_returnFloatAltIfValueBelow( bool bHigher , float fValue , float fLimit , float fAlt );
str coop_returnStringUntilWhithspace( str sChain );
str coop_returnStringUntilNewline( str sChain ); //[b607] chrissstrahl
str coop_returnStringPathFileNoExtension(const str &sPath);//[b607] chrissstrahl
str coop_returnStringFilenameOnly( const str &sPath );
str coop_returnStringFileExtensionOnly( const str &sPath );
str coop_returnStringDeathmessagenameForEntity( Entity * entity );
str coop_returnStringDeathmessagenameForActor( Actor * actor );
Entity* coop_returnEntityPlayerFavored( void );
Player* coop_returnPlayerNotFavored( void );
Player* coop_returnPlayerFavored( void );
Player* coop_returnPlayerInCallvolumeClosestTo( Entity *eMe );
Player* coop_returnPlayerClosestTo(Entity *eMe);
int coop_returnPlayerQuantity( void );
int coop_returnPlayerQuantityInArena( void );
int coop_returnPlayerQuantity(short condition);
int coop_returnCvarInteger( str cvarName );
str coop_returnCvarString( str cvarName );
Vector coop_returnCvarVector( str cvarName );
Entity * coop_returnEntityAttachedByTargetname( Entity *eParent , str attachedTargetname );
Entity * coop_returnOwnerEntity( Entity* entity );
Player * coop_returnOwnerPlayer( Entity* entity );
str coop_returnObjectivesCvarName( int iObjectivesItem );
Vector coop_returnVectorFromString( str sChain );
Vector coop_getVectorScriptVariable( str s );
float coop_getFloatScriptVariable( str s );
str coop_getStringScriptVariable( str s );
float coop_returnFloatFromString( str extrapolate );
bool coop_returnLevelType( str sLevelname , bool &standard , int &type );
//[b60011] chrissstrahl - frequently needed, couldn't find them in code but was sure they did exist
float coop_returnFloatNotNegative(float fLt);
int coop_returnIntegerNotNegative(int iNt);
#endif /* coopReturn.hpp */

