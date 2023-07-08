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

#include "_pch_cpp.h"
#include "coopText.hpp"
#include "coopCheck.hpp"
#include "coopReturn.hpp"
#include "coopAlias.hpp"

//================================================================
// Name:        coop_textReplace
// Class:       -
//              
// Description:	replace ge given string inside the 
//              
// Parameters:  str sText, str sFind , sReplace
//              
// Returns:     bool
//              
//================================================================
bool coop_textReplace( str &sHeystack, const str &sDildo, const str &sNedle )
{
	//if dildo bad , plug it
	//if dildo smaller than heystack, burn heystack
	if ( strlen( sDildo ) < 1 || strlen( sHeystack ) < strlen( sDildo ) )
		return false;

	int iWater = coop_returnIntFind( sHeystack , sDildo );

	//found no dildo, imidatly plug it
	if ( iWater < -1 )
		return false;

	str sLighter = sHeystack;
	coop_manipulateStringFromWithLength( sLighter , 0 , (iWater - 1 ) );
	sLighter += sNedle;
	sLighter += coop_returnStringStartingFrom( sLighter , ((iWater - 1) + strlen( sDildo ) )  );

	//be manipulative
	sHeystack = sLighter;
	return true;
}

//================================================================
// Name:        coop_textModifyVoteText
// Class:       -
//              
// Description: Return localized and optimized vote string
//              
// Parameters:  str sVoteText
//              
// Returns:     bool
//              
//================================================================
bool coop_textModifyVoteText( const str _voteString, str &sVoteText, Player* player )
{
	if ( !game.coop_isActive )
		return false;

	bool bAltText = true;

	//hzm coop mod chrissstrahl - print it in german if player is using german game version
	if ( coop_checkPlayerLanguageGerman( player ) )
	{
		if ( coop_returnIntFind( _voteString , "coop_maxspeed" ) > -1 )
			{ sVoteText = "Setze Laufgeschwindigkeit auf:"; sVoteText += coop_returnStringStartingFrom( _voteString , 13 ); }
		else if ( coop_returnIntFind( _voteString , "coop_ff" ) > -1 )
			{sVoteText = "Setze Friendly Fire auf:";sVoteText += coop_returnStringStartingFrom( _voteString , 7 );}
		else if ( coop_returnIntFind( _voteString , "coop_next" ) > -1 )
			{ sVoteText = "Lade naechste Coop Karte - JETZT!"; sVoteText += coop_returnStringStartingFrom( _voteString , 9 ); }
		else if ( coop_returnIntFind( _voteString , "coop_prev" ) > -1 )
			{ sVoteText = "Lade vorherige Coop Karte - JETZT!"; sVoteText += coop_returnStringStartingFrom( _voteString , 9 ); }
		else if ( coop_returnIntFind( _voteString , "coop_skill" ) > -1 )
			{ sVoteText = "Setze Schwiergkeitsgrad auf:"; sVoteText += coop_returnStringStartingFrom( _voteString , 5 ); }
		//chrissstrahl - print readable string instead of command [b607]
		else if (coop_returnIntFind(_voteString, "coop_challenge") > -1)
			{ sVoteText = "Setze Herausforderung auf:"; sVoteText += coop_returnStringStartingFrom(_voteString, 13);}
		else if (coop_returnIntFind(_voteString, "coop_lms") > -1)
			{ sVoteText = "Setze Last Man Standing auf:"; sVoteText += coop_returnStringStartingFrom(_voteString, 8);}
		else if (coop_returnIntFind(_voteString, "coop_awards") > -1)
			{ sVoteText = "Setze Auszeichnungen auf:"; sVoteText += coop_returnStringStartingFrom(_voteString, 11);}
		//end of [b607] changes
		else
			{ bAltText = false; }
	}
	else {
		if ( coop_returnIntFind( _voteString , "coop_maxspeed" ) > -1 )
			{ sVoteText = "Set Movement Speed to:"; sVoteText += coop_returnStringStartingFrom( _voteString , 13 ); }
		else if ( coop_returnIntFind( _voteString , "coop_ff" ) > -1 )
			{sVoteText = "Set Friendly Fire to:";sVoteText += coop_returnStringStartingFrom( _voteString , 7 );}
		else if ( coop_returnIntFind( _voteString , "coop_next" ) > -1 )
			{ sVoteText = "Load Next Coop Map - NOW!"; sVoteText += coop_returnStringStartingFrom( _voteString , 9 ); }
		else if ( coop_returnIntFind( _voteString , "coop_prev" ) > -1 )
			{ sVoteText = "Load Previous Coop Map - NOW!"; sVoteText += coop_returnStringStartingFrom( _voteString , 9 ); }
		else if ( coop_returnIntFind( _voteString , "coop_skill" ) > -1 )
			{ sVoteText = "Set Dificulty to:"; sVoteText += coop_returnStringStartingFrom( _voteString , 5 ); }
		//chrissstrahl - print readable string instead of command [b607]
		else if (coop_returnIntFind(_voteString, "coop_teamicon") > -1)
			{ sVoteText = "Set Challenge to:"; sVoteText += coop_returnStringStartingFrom(_voteString, 13);}
		else if (coop_returnIntFind(_voteString, "coop_lms") > -1)
			{ sVoteText = "Set Last Man Standing to:"; sVoteText += coop_returnStringStartingFrom(_voteString, 8);}
		else if (coop_returnIntFind(_voteString, "coop_awards") > -1)
			{ sVoteText = "Set Awards to:"; sVoteText += coop_returnStringStartingFrom(_voteString, 11);}
		//end of [b607] changes
		else
			{ bAltText = false; }
	}
	return bAltText;
}

//================================================================
// Name:        coop_textJoinCommands
// Class:       -
//              
// Description:	Joins two console commands to one
//              
// Parameters:  str sCmd1 , str SCmd2
//              
// Returns:     str
//              
//================================================================
str coop_textJoinCommands( str sCmd1 , str sCmd2 )
{
	bool bEmpty=false;
	str s1="" , s2="";

	if ( !sCmd1 || strlen( sCmd1 ) < 1 )bEmpty = true;
	else s1 = sCmd1;

	if ( !sCmd2 || strlen( sCmd2 ) < 1 )bEmpty = true;
	else s2 = sCmd2;
	
	sCmd1 = coop_textReturnFromTill( sCmd1 , 0 , COOP_JOIN_MAX_SINGLE_COMMAND );
	sCmd2 = coop_textReturnFromTill( sCmd2 , 0 , COOP_JOIN_MAX_SINGLE_COMMAND );

	if ( bEmpty ){
		gi.Printf("coop_textJoinCommands: WARNING: empty command joined\n");
	}else{
		s1 += ";";
		s1 += s2;	
	}

	return s1;
}

//================================================================
// Name:        coop_textReturnFromTill
// Class:       -
//              
// Description:	Returns a string starting from, until, do not confuse this till with length!
//              
// Parameters:  str sText , int iStart , int iMax
//              
// Returns:     str
//              
//================================================================
str coop_textReturnFromTill( str sText, int iStart, int iMax )
{
	if ( !sText || sText.length() < 1 ) return "";
	if ( iMax > sText.length() ) iMax = sText.length();

	if ( iStart > iMax ){
		gi.Printf( va( "coop_textReturnFromTill: ERROR in values: start[%i] max[%i]\n" , iStart , iMax ) );
		return "";
	}

	int					i;
	str					sNew = "";
	for ( i = iStart; i < iMax; i++ ){
		sNew += sText[i];
	}
	return sNew;
}

//================================================================
// Name:        coop_textCleanAllButLetters
// Class:       -
//              
// Description:		removes the everything but the actual letters from string
//              
// Parameters:  str sText
//              
// Returns:     str
//              
//================================================================
str coop_textCleanAllButLetters( str sText )
{
	//filter all but letters
	static const char	validChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	int					i , j;
	bool				isValidChar;
	str					sNew = "";

	for ( i = 0; i < sText.length(); i++ ){
		isValidChar = false;
		for ( j = 0; validChars[j]; j++ ){
			if ( sText[i] == validChars[j] ){ isValidChar = true; }
		}

		if ( !isValidChar ){
			sNew += "";
		}else{
			sNew += sText[i];
		}
	}
	return sNew;
}


//================================================================
// Name:        coop_textCleanAllButLettersAndLower
// Class:       -
//              
// Description:		removes the everything but the actual letters from string, transforms to lowercase
//              
// Parameters:  str sText
//              
// Returns:     str
//              
//================================================================
str coop_textCleanAllButLettersAndLower( str sText )
{
	//filter all but letters
	static const char	validChars[] = "abcdefghijklmnopqrstuvwxyz";
	int					i , j;
	bool				isValidChar;
	str					sNew = "";

	sText = sText.tolower();

	for ( i = 0; i < sText.length(); i++ ){
		isValidChar = false;
		for ( j = 0; validChars[j]; j++ ){
			if ( sText[i] == validChars[j] ){ isValidChar = true; }
		}

		if ( !isValidChar ){
			sNew += "";
		}
		else{
			sNew += sText[i];
		}
	}
	return sNew;
}

//================================================================
// Name:        coop_textReplaceWhithespace
// Class:       -
//              
// Description:		replaces the current string after all whithespaces have been replaced
//					used for locationprint strings and menu related stuff (Coop communicator Transporetr Menu)
//              
// Parameters:  str sText
//              
// Returns:     str
//              
//================================================================
str coop_textReplaceWhithespace( str sText )
{
	int		i;
	str		sNewText = "";

	for ( i = 0; i < sText.length(); i++ ){
		if ( sText[i] == ' ' ){
			sNewText += "_";
		}
		else{
			sNewText += sText[i];
		}
	}

	return sNewText;
}

//================================================================
// Name:        coop_textReplaceWhithespaceBlack
// Class:       -
//              
// Description:		replaces the current string after all whithespaces have been replaced
//					used for locationprint strings
//              
// Parameters:  str sText
//              
// Returns:     str
//              
//================================================================
str coop_textReplaceWhithespaceBlack( str sText )
{
	int		i;
	str		sNewText = "";

	for ( i = 0; i < sText.length(); i++ ){
		if ( sText[i] == ' ' ){
			sNewText += "^0_^8";
		}
		else{
			sNewText += sText[i];
		}
	}

	return sNewText;
}

//================================================================
// Name:        coop_textChatTimeCountDown
// Class:       -
//              
// Description: counts down after player has spammed until player is allowed to chat again
//
// Parameters:  str sText 
//              
// Returns:     void
//              
//================================================================
void coop_textChatTimeCountDown( Player *player )
{
	if ( player->coopPlayer.chatTimeLimit < level.time )return;

	if ( ( player->coopPlayer.chatTimeLimit + -1 ) < level.time ){
		player->coopPlayer.chatTimeLimit = level.time;
	}else{
		player->coopPlayer.chatTimeLimit--;
	}
}


//================================================================
// Name:        coop_textPhraseLocalStrUmlaute
// Class:       -
//              
// Description: replaces umlaute with ASCII compatible letters,
//				umlaute are filtred in net code and would not make it to the client otherwise
//
// Parameters:  str sText 
//              
// Returns:     void
//              
//================================================================
str coop_textPhraseLocalStrUmlaute( Player *player, str sText )
{
	//WARNING, this has a bug, if a umlaut is followed by another, they will not work and $$$ is shown
	int i;
	str sNewText = "";
	str sCoop = "";
	str sNoCoop = "";
	int iIndex = 0;
	
	for ( i = 0; i < sText.length(); i++ ) {
		//if we found something like $$o$$, we will have to skip $o$$
		if ( iIndex > i ) {
			continue;
		}

		bool bAddE=true;
		iIndex = i;

		//search for something like $$o$$
		if ( sText[i] == '$' && ( i + 4 ) < sText.length() ){
			bAddE = false;
			if ( sText[i + 1] == '$' && sText[i + 3] == '$' && sText[i + 4] == '$' ) {
				if ( sText[i + 2] == 'a' ||
					sText[i + 2] == 'A' ||
					sText[i + 2] == 'o' ||
					sText[i + 2] == 'O' ||
					sText[i + 2] == 'u' ||
					sText[i + 2] == 'U' ||
					sText[i + 2] == 'S' )
				{
					bAddE = true;
					str conv = sText[i + 2];
					sCoop	= va( "$$%s$$", conv.c_str() );
					sNoCoop	= va( "%se", conv.c_str() );
					iIndex = (i + 5);
				}
			}
		}
		//search for umlauts
		else if ( sText[i] == 'ä' ) {
			sCoop = "$$a$$"; sNoCoop = "ae";
		}
		else if ( sText[i] == 'Ä' ) {
			sCoop = "$$A$$"; sNoCoop = "AE";
		}
		else if ( sText[i] == 'ö' ) {
			sCoop = "$$o$$"; sNoCoop = "oe";
		}
		else if ( sText[i] == 'Ö' ) {
			sCoop = "$$O$$"; sNoCoop = "OE";
		}
		else if ( sText[i] == 'ü' ) {
			sCoop = "$$u$$"; sNoCoop = "ue";
		}
		else if ( sText[i] == 'Ü' ) {
			sCoop = "$$U$$"; sNoCoop = "UE";
		}
		else if ( sText[i] == 'ß' ) {
			sCoop = "$$S$$"; sNoCoop = "Sz";
		}
		//no match, just regular text
		else {
			bAddE = false;
		}

		if ( bAddE ) {
			//no coop mod
			if ( player->coop_getInstalled() == 0 ) {
				sNewText += sNoCoop;
			}
			//coop mod installed
			else {
				sNewText += sCoop;
			}
		}
		else {
			sNewText += sText[i];
		}
	}
	return sNewText;
}


//================================================================
// Name:        coop_textCinematicHudprint
// Class:       -
//              
// Description: Prints text to player hud, unless there is a cinematic
//				prints hud to predefined loctaion during cinematic
//
// Parameters:  Player *player , str sText 
//              
// Returns:     void
//              
//================================================================
void coop_textCinematicHudprint( Player *player , str sText )
{
	if ( !level.cinematic ){
		player->hudPrint( sText.c_str() );
	}
	else{
		sText = coop_textReplaceWhithespace( sText.c_str() );
		DelayedServerCommand( player->entnum , va( "locationprint 220 455 %s 0.8" , sText.c_str() ) );
	}
}

//================================================================
// Name:        coop_textHudprint
// Class:       -
//              
// Description: (SP/MP) this is our reliable Hudprint function
//				The text will always be shown to the player             
//
// Parameters:  Player *player , str sText 
//              
// Returns:     void
//              
//================================================================
void coop_textHudprint( Player *player , str sText )
{
	if ( !sText.length() ){
		return;
	}

	str command;
	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		gentity_t *gentity;
		gentity = &g_entities[0];
		if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) ){
			command = sText;
			command += "\n";
			gi.centerprintf( gentity , CENTERPRINT_IMPORTANCE_CRITICAL , command.c_str() );
		}
	}
	else if( player ){
		player->hudPrint( sText.c_str() );
	}
}

//================================================================
// Name:        coop_textHudprintAll
// Class:       -
//              
// Description: (SP/MP) this is our Hudprint function, works in SP and MP
//              
// Parameters:  str messageToShow
//              
// Returns:     void
//              
//================================================================
void coop_textHudprintAll( str messageToShow )
{
	if ( !messageToShow.length() ){
		return;
	}

	int i;
	gentity_t *gentity;
	Player *player = NULL;
	str command;

	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		gentity = &g_entities[0];
		if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) ){


			command = messageToShow;
			command += "\n";
			gi.centerprintf( gentity , CENTERPRINT_IMPORTANCE_CRITICAL , command.c_str() );
		}
		return;
	}

	for ( i = 0; i < maxclients->integer; i++ ){
		gentity = &g_entities[i];
		if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) ){
			player = ( Player * )gentity->entity;
			player->hudPrint( messageToShow.c_str() );
		}
	}
}

//[b60011]=Chrissstrahl=============================================
// Replaces withespace and newline with labeltext compatible chars, which are handled client side
//================================================================
str coop_replaceForLabelText(str sPure)
{
	int i = 0;
	//[b60014] chrissstrahl - fixed "labeltext^" issue
	if (strlen(sPure) > 10) {
		str sTemp = sPure;
		sTemp = sTemp.tolower();
		if (coop_returnIntFind(sTemp,"labeltext ") != -1) {
			i = 11;
		}
	}
	
	for (i = i; i < strlen(sPure); i++) {
		if (sPure[i] == '\n' || sPure[i] == '#')
			sPure[i] = '~';
		if (sPure[i] == ' ')
			sPure[i] = '^';
	}
	return sPure;
}