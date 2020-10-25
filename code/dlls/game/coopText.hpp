//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING TEXT AND LOCALIZING RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]



#ifndef __COOPTEXT_HPP__
#define __COOPTEXT_HPP__

#define COOP_MAX_TRANSMITT_STRING 272		//max string size, limit is menu compatibility whichends at 272
#define COOP_TRANSMITT_STARTCMD	10			//client start cmd (stufftext ), size of inizial client command
#define COOP_JOIN_SEPERATOR_SIZE 1			//size of join seperator (;) for console commands
#define COOP_JOIN_MAX_SINGLE_COMMAND (((COOP_MAX_TRANSMITT_STRING / 2 ) - (COOP_TRANSMITT_STARTCMD / 2)) - 1)

#include "_pch_cpp.h"

bool coop_textReplace( str &sHeystack , const str &sDildo , const str &sNedle );
bool coop_textModifyVoteText( const str _voteString , str &sVoteText , Player* player );
str coop_textJoinCommands( str sCmd1 , str sCmd2 );
str coop_textReturnFromTill( str sText , int iStart , int iMax );
void coop_textCinematicHudprint( Player *player , str sText );
str coop_textCleanAllButLetters( str sText );
str coop_textCleanAllButLettersAndLower( str sText );
str coop_textReplaceWhithespace( str sText );
str coop_textReplaceWhithespaceBlack( str sText );
void coop_textChatTimeCountDown( Player *player );
str coop_textPhraseLocalStrUmlaute( Player *player, str sText );
void coop_textHudprint( Player *player , str sText );
void coop_textHudprintAll( str messageToShow );

#endif /* coopText.hpp */
