//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING INI PHRASER TO SAVE SETTINGS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#include "_pch_cpp.h"

#include "coopPlayer.hpp"
#include "coopClass.hpp"
#include "coopArmory.hpp"
#include "coopText.hpp"
#include "coopReturn.hpp"
#include "coopAlias.hpp"
#include "coopParser.hpp"

#include "level.h"
#include "player.h"
#include "weapon.h"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include "powerups.h"

//================================================================
// Name:        coop_phraserReadFile
// Class:       -
//              
// Description: reads file from hard drive
//              
// Parameters:  filename
//              
// Returns:     bool
//              
//================================================================
bool coop_parserReadFile( const str sFile , str &buffer )
{
	//if someone tries to open files with wrong extension
	str sFileExt = coop_getFileExtension( sFile );
	if ( stricmp( sFileExt.c_str() , ".ini" ) != 0 &&
		stricmp( sFileExt.c_str() , ".log" ) != 0 &&
		stricmp( sFileExt.c_str() , ".txt" ) != 0 &&
		stricmp( sFileExt.c_str() , ".cfg" ) != 0 &&
		stricmp( sFileExt.c_str() , ".vlp" ) != 0 &&//[b607] chrissstrahl - need this to get dialog length for actor.playdialog
		stricmp( sFileExt.c_str() , ".inf" ) != 0 )
	{
		throw( va( "HZM-Phraser does not read '%s' files, only: ini, inf, txt, cfg, vlp, log" , sFileExt.c_str() ) );
		return false;
	}

	Script bufferInternal;
	const char *token;

	// Make sure the file exists

	if ( !gi.FS_Exists( sFile.c_str() ) ) {
		gi.Printf( "=============================================\n" );
		gi.Printf( va( "WARNING: coop_phraserReadFile FILE NOT FOUND (%s) \n" , sFile.c_str() ) );
		gi.Printf( "=============================================\n" );
		return false;
	}

	// Load the file

	bufferInternal.LoadFile( sFile.c_str() );

	// load file contents directly to a var
	long lSize;
	lSize = bufferInternal.length;
	if ( bufferInternal.buffer != NULL){

		buffer = bufferInternal.buffer;
		coop_manipulateStringFromWithLength( buffer , 0 , lSize );
	}
	bufferInternal.Close();
	return true;
}

//================================================================
// Name:        coop_phraserGetNumberOfItemsFromCategory
// Class:       -
//              
// Description: counts number of items in a section
//              
// Parameters:  filename, sectionname
//              
// Returns:     int
//              
//================================================================
int coop_parserGetNumberOfItemsFromCategory( str sFile , const str section )
{
	bool bDesiredSection = false;
	str buffer = "";

	//read file and load contents into var
	if ( !coop_parserReadFile( sFile , buffer ) ){
		return -1;
	}

	//section not in the file
	if ( coop_returnIntFind( buffer , va("[%s]", section.c_str() ) ) == -1 ){
		return -1;
	}

	int iItemsFound=0;

	//find section, then continue from there
	str value = "";

	//check each letter and phrase it line by line
	for ( int i = 0; i < buffer.length(); i++ )
	{
		//append to string, using append is more memory efficent, i think...
		value += buffer[i];

		if ( buffer[i] == '\n' || buffer[i] == EOF || i == ( buffer.length() - 1 ) )
		{
			//check for starting braket
			if ( value[0] == '[' )
			{
				if ( value[section.length() + 1] == ']' &&
					stricmp( coop_returnStringFromWithLength( value , 1 , section.length() ).c_str() , section.c_str() ) == 0 )
				{
					bDesiredSection = true;
				}
				else
				{
					//desired section has ended
					if ( bDesiredSection ) {
						return iItemsFound;
					}

					//wrong section, skip
					bDesiredSection = false;
					value = "";
					continue;
				}
			}
			else {
				//checp check if there could really be a item
				int iPosEqu = coop_returnIntFind( value , "=" );
				if ( iPosEqu != -1 ) {
					iItemsFound++;
				}
			}
			//gi.Printf( va( "coop_phraserGetNumberOfItemsFromCategory: %s %d\n" , value.c_str(), iItemsFound ) );

			value = "";
		}
	}
	return iItemsFound;
}


//================================================================
// Name:        coop_phraserGetItemsFromCategory
// Class:       -
//              
// Description: counts number of items in a section
//              
// Parameters:  filename, ListenKnoten, ListenKnoten, sectionname
//              
// Returns:     void / sets START and END ListenKnoten
//              
//================================================================
void coop_parserGetItemsFromCategory( str sFile , ListenKnoten * &start , ListenKnoten * &end , const str section )
{
	str buffer = "";

	//read file and load contents into var
	if ( !coop_parserReadFile( sFile , buffer ) ) {
		return;
	}

	//section not in the file
	if ( coop_returnIntFind( buffer , va( "[%s]" , section.c_str() ) ) == -1 ) {
		gi.Printf( "=============================================\n" );
		gi.Printf( va( "INFO: coop_phraserGetItemsFromCategory SECTION NOT FOUND (%s) \n" , section.c_str() ) );
		gi.Printf( "=============================================\n" );
		return;
	}

	//find section, then continue from there
	str value = "";
	bool bDesiredSection = false;
	int iCurrentItem=0;

	//last node
	ListenKnoten *lastNode = NULL;

	//current node
	ListenKnoten *current = NULL;

	//check each letter and phrase it line by line
	for ( int i = 0; i < buffer.length(); i++ )
	{
		//append to string, using append is more memory efficent, i think...
		value += buffer[i];

		if ( buffer[i] == '\n' || buffer[i] == EOF || i == ( buffer.length() - 1 ) )
		{
			//check for starting braket
			if ( value[0] == '[' )
			{
				if ( value[section.length() + 1] == ']' &&
					stricmp( coop_returnStringFromWithLength( value , 1 , section.length() ).c_str() , section.c_str() ) == 0 )
				{
					bDesiredSection = true;
				}
				else
				{
					//desired section has ended
					if ( bDesiredSection ) {
						//get endpos
						end = lastNode;
						//gi.Printf( va( "end: %d\n" , end ) );
						return;
					}

					//wrong section, skip
					bDesiredSection = false;
					value = "";
					continue;
				}
			}
			else if( bDesiredSection ) {
				//check if there could really be a item
				int iValStartPos = coop_returnIntFind( value , "=" );
				if ( iValStartPos != -1 ) {
					iCurrentItem++;

					//create new element
					ListenKnoten *node = new ListenKnoten;

					coop_trimM( value , " \t\n" );

					//set data
					node->value = coop_returnStringStartingFrom(value,iValStartPos + 1);

					//link to NULL until another node is added
					node->next = NULL;

					//gi.Printf( va( "added v: %s\n" , node->value.c_str() ) );

					if ( lastNode ) {
						//add last node to current
						node->prev = lastNode;
						//add next node to previouse
						lastNode->next = node;
					}
					else {
						//no previouse
						node->prev = NULL;
						//first node
						start = node;
						//gi.Printf( va( "start: %d\n" , start ) );
					}

					//get current node
					lastNode = node;
				}
			}
			//gi.Printf( va( "coop_phraserGetNumberOfItemsFromCategory: %s %d\n" , value.c_str(), iItemsFound ) );
			value = "";
		}
	}

	//get endpos
	end = lastNode;
	//gi.Printf( va( "end: %d\n" , end ) );

	return;
}

//================================================================
// Name:        coop_phraserIsItemInCategory
// Class:       -
//              
// Description: returns if a items is in the given section
//              
// Parameters:  filename, itemname, sectionname
//              
// Returns:     bool
//              
//================================================================
bool coop_parserIsItemInCategory( str sFile , const str item , const str section )
{
	ListenKnoten *itemList_start = NULL;
	ListenKnoten *itemList_end = NULL;
	ListenKnoten *itemList_current = NULL;
	ListenKnoten *itemList_delete = NULL;

	bool bItemOnList = false;

	//get all items into the list
	coop_parserGetItemsFromCategory( sFile , itemList_start , itemList_end , section );

	//check each item
	itemList_current = itemList_start;
	while ( itemList_current ) {

		if ( !bItemOnList ) {
			//check if the item is on the list
			if ( stricmp( itemList_current->value.c_str() , item.c_str() ) == 0 ) {
				bItemOnList = true;
			}
		}

		//get next list item
		itemList_current = itemList_current->next;

		//remove the previouse list item
		delete itemList_delete;
	}

	return bItemOnList;
}


//================================================================
// Name:        coop_phraserIniGet
// Class:       -
//              
// Description: get a specified value from a specified section
//              
// Parameters:  filename, key, section
//              
// Returns:     string
//              
//================================================================
str coop_parserIniGet( str sFile, const str key, const str section )
{
	bool bDesiredSection = false;
	str buffer = "";

	//read file and load contents into var
	if ( !coop_parserReadFile( sFile.c_str() , buffer ) ){
		return "";
	}

	//section not in the file
	if ( coop_returnIntFind( buffer , va( "[%s]" , section.c_str() ) ) == -1 ) {
		gi.Printf( va( "INFO: coop_phraserIniGet section not found: %s\n" , section.c_str() ) );
		return "";
	}

	str value = "";

	//check each letter and phrase it line by line
	for ( int i = 0; i < buffer.length(); i++ ){
		//append to string, using append is more memory efficent, i think...
		value += buffer[i];

		if ( buffer[i] == '\n' || buffer[i] == EOF || i == ( buffer.length() - 1 ) ){
			//check for starting braket
			if ( value[0] == '[' ){
				//gi.Printf( va( "coop_phraserIniGet: %s\n" , value.c_str() ) );

				//check if ending braket is on the right position
				//check if section name is valid
				//fixed: make sure we do not accept finds that are outside of out the desired section
				if (	value[ section.length() + 1 ] == ']' &&
						stricmp( coop_returnStringFromWithLength( value , 1 , section.length() ).c_str() , section.c_str() ) == 0 )
				{
					bDesiredSection = true;
				}
				else{
					//fixed: make sure we do not accept finds that are outside of out the desired section, after wards when something real was found
					if( bDesiredSection ){

						//make sure memory is freed //make sure file is closed
						//if ( !buffer == NULL ) { free( buffer ); }
						//fclose( pFile );

						return "";
					}

					//wrong section, skip
					bDesiredSection = false;
					value = "";
					continue;
				}
			}

			//update: check onjly if valid section
			if ( bDesiredSection ) {
				//find pos if key
				int iPosKey = coop_returnIntFind( value , key.c_str() );

				//fix: check if the string we found is really the key or if it is just a part of it
				if ( iPosKey != -1 ) {
					int startingPos = ( iPosKey + key.length() );
					int endingPos = ( value.length() - startingPos );
					int currentPos = startingPos;
					bool bFoundEqu = false;
					for ( currentPos = currentPos; currentPos <= endingPos; currentPos++ ) {
						if ( value[currentPos] != ' ' && value[currentPos] != '\t' && value[currentPos] != '=' ) {
							iPosKey = -1;
							break;
						}
						else if ( value[currentPos] == '=' ) {
							bFoundEqu = true;
							break;
						}
					}
				}

				if ( iPosKey != -1 ){
					int iPosEqu = coop_returnIntFind( value , "=" );
					int iPosSemi = coop_returnIntFind( value , ";" );
					int iPosBrack = coop_returnIntFind( value , "[" );
					//if = comes after key
					//if ; comes not at all OR after =
					//if [ comes not at all OR after =
					if ( iPosEqu > iPosKey &&
						iPosSemi == -1 || iPosSemi > iPosEqu &&
						iPosBrack == -1 || iPosBrack > iPosEqu
						)
					{
						iPosEqu++;//start after =
						//check if there is a value after =
						if ( ( iPosEqu + 1 ) < strlen( value ) ){
							value = coop_substrToEnd( value , iPosEqu );
							coop_trimM( value , "\t\n\r" ); //[b607] chrissstrahl - trim last \n as well

							//debug
							//gi.Printf( "coop_phraserIniGet VALUE_RETRIVED!!\n" );
							return value;
						}
					}
				}
			}
			value = "";
		}
	}
	//debug
	//gi.Printf( "coop_phraserIniGet COULD_NOT_FIND_KEY\n" );
	return "";
}

//================================================================
// Name:        coop_parserIniSet
// Class:       -
//              
// Description: sets a specified value to a specified key in a specified section
//              
// Parameters:  filename, key, value, section
//              
// Returns:     bool
//              
//================================================================
bool coop_parserIniSet( str sFile , const str &key , const str &value , const str section )
{
	
	//if someone tries to open files with wrong extension
	str sFileExt = coop_getFileExtension( sFile );
	if (	stricmp( sFileExt.c_str() , ".ini" ) != 0 &&
			stricmp( sFileExt.c_str() , ".inf" ) != 0 )
	{
		throw( va( "HZM-Phraser does not write '%s' files, only: ini, inf" , sFileExt.c_str() ) );
		return false;
	}

	//read existing file
	//str sGame = coop_returnCvarString( "fs_game" );
	//if ( sGame.length() < 1) sGame = "base";
	//str sFilePath = va( "%s\\%s\\%s" , coop_returnCvarString( "fs_basepath" ).c_str() , sGame.c_str() , sFile.c_str() );


	str buffer = "";

	//here it does not matter if it is inside a pk3 or not
	//here it does not matter if it is inside a pk3 or not
	//here it does not matter if it is inside a pk3 or not

	//read file and load contents into var
	if ( coop_parserReadFile( sFile , buffer ) )
	{
		//can only delete from outside a pk3
		gi.FS_DeleteFile( sFile );
	}
	
	//start writing the new file - will be written outside of a pk3
	//start writing the new file - will be written outside of a pk3
	//start writing the new file - will be written outside of a pk3

	//make static so we can check if it is already open
	static fileHandle_t iniFile = NULL;

	//make sure no file is open from last time
	if ( iniFile ) {
		throw( "HZM-Phraser a file was already open\n" );
		return false;
	}

	iniFile = gi.FS_FOpenFileWrite( sFile.c_str() );

	//make sure we can write file
	if ( !iniFile ) {
		if ( coop_returnCvarInteger( "developer" ) > 0 ) {
			throw( va( "HZM-Phraser coud not write data to file: %s - Write-protection? Bad-Accsess-rights?\n" , sFile.c_str() ) );
		}
		return false;
	}

	//fix: if file ended without a final \n there where issues
	if ( buffer[buffer.length() - 1] != '\n' )
		buffer += '\n';

	long lSize = buffer.length();
	bool bSectionSet = false;
	bool bKeySet = false;
	str sLine = "";
	str sExtraLine = "";
	str sCurrentSection = "";
	str sNewBuffer = "";

	for ( int i = 0; i < lSize; i++ )
	{
		//append to string
		sLine += buffer[i];

		//skip if line is incomplete or file does not end yet
		if ( buffer[i] != '\n' && i < lSize || buffer[i] == EOF )
			continue;

		//gi.Printf( va( "..%s" , sLine.c_str() ) );

		//trim string
		coop_trimM( sLine , " \t\r" );

		//commentary, empty line or key already set, keep line as it is and write it directly to the file
		if ( bKeySet || sLine[0] == ';' || sLine[0] == '\n' )
		{
			//used for testing only, remove afterwards
			//gi.Printf( va( "LINE: %s" , sLine ) );

			//fprintf( pFileWrite , va( "%s" , sLine.c_str() ) );
			sNewBuffer += sLine;

			//clear line var
			sLine = "";

			//skip only if the file has not ended yet
			if ( i < ( lSize - 1 ) ) {
				continue;
			}
		}

		//gi.Printf( va( "%d:%d\n" , i , ( lSize - 1 ) ) );

		//[b607] chrissstrahl - make sure that there are not multiple lines in a value
		const str newValue = coop_returnStringUntilNewline(value);

		//key found overwrite
		if ( !Q_stricmp( sCurrentSection , section ) && coop_returnIntFind( sLine , key ) > -1 && !bKeySet )
		{
			sNewBuffer += va( "%s%s=%s\n" , sExtraLine.c_str() , key.c_str() , newValue.c_str() );
			//fprintf( pFileWrite , va( "%s%s=%s\n" , sExtraLine.c_str() , key.c_str() , value.c_str() ) );
			bKeySet = true;
		}
		else if ( sLine[0] != '[' )
		{
			sNewBuffer += sLine;
			//fprintf( pFileWrite , va( "%s" , sLine.c_str() ) );
		}

		//check if it is a section
		if ( sLine[0] == '[' )
		{
			//make sure the key is written before we leave the correct section
			if ( !Q_stricmp( sCurrentSection , section ) && !bKeySet )
			{
				sNewBuffer += va( "%s%s=%s\n" , sExtraLine.c_str() , key.c_str() , newValue.c_str() );
				//fprintf( pFileWrite , va( "%s%s=%s\n" , sExtraLine.c_str() , key.c_str() , value.c_str() ) );
				bKeySet = true;
			}

			//strip line down to sectionname only
			sCurrentSection = coop_trim( sLine.tolower() , " []\t\r\n" ).tolower();

			//write section name
			if ( !Q_stricmp( sCurrentSection , section ) ) {
				bSectionSet = true;
			}
			sNewBuffer += va( "%s[%s]\n" , sExtraLine.c_str() , sCurrentSection.c_str() );
			//fprintf( pFileWrite , va( "%s[%s]\n" , sExtraLine.c_str() , sCurrentSection.c_str() ) );
		}

		//file ended, and section could not be found
		if ( i == ( lSize - 1 ) )
		{
			//if there is no valid section create it now
			if ( Q_stricmp( sCurrentSection , section ) && !bSectionSet )
			{
				bSectionSet = true;
				sNewBuffer += va( "%s[%s]\n" , sExtraLine.c_str() , section.c_str() );
				//fprintf( pFileWrite , va( "%s[%s]\n" , sExtraLine.c_str() , section.c_str() ) );
			}
			//write key and value right now
			if ( !bKeySet )
			{
				sNewBuffer += va( "%s%s=%s\n" , sExtraLine.c_str() , key.c_str() , newValue.c_str() );
				//fprintf( pFileWrite , va( "%s%s=%s\n" , sExtraLine.c_str() , key.c_str() , value.c_str() ) );
			}
		}
		//clear line var
		sLine = "";
	}
	gi.FS_Write( sNewBuffer , sNewBuffer.length() , iniFile );

	//close file
	gi.FS_Flush( iniFile );
	gi.FS_FCloseFile( iniFile );
	iniFile = NULL;
	return true;
}
