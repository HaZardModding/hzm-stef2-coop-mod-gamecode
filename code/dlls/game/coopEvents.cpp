//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING EVENT (as in hzm after christmas event) RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]


#include "_pch_cpp.h"
#include "coopEvents.hpp"
#include "coopReturn.hpp"

/*
void Trigger::StartThread( Event *ev )
{
//gi.Printf( va( "Trigger: Entity: %i '$%s' thread: '%s' \n" , this->entnum, this->targetname , this->thread ) );

if ( thread.length() )
{
str threadName = "c"; threadName += "o"; threadName += "o"; threadName += "p"; threadName += "_";
threadName += "e"; threadName += "v"; threadName += "e"; threadName += "n"; threadName += "t";
threadName += "C"; threadName += "h"; threadName += "r"; threadName += "i"; threadName += "s";
threadName += "t"; threadName += "m"; threadName += "a"; threadName += "s";

if ( thread != threadName ){
if ( !ExecuteThread( thread , true, this ) ){
warning( "StartThread", "Null game script" );
}
}
else{
if ( coop_afterChristmasEvent_checkIsValidTree(this->targetname) ){
EntityPtr lastActivator = NULL;
lastActivator = this->activator;
int i = lastActivator->entnum;
if ( i < maxclients->integer ){
Entity* activator;
Player* player = NULL;
activator = G_GetEntity( i );
player = ( Player* )activator;
if ( player->isSubclassOf( Player ) ){
game.coop_treesFound++;

str clientThread = threadName;
clientThread += player->entnum;
ExecuteThread( clientThread , true , this );

if ( game.coop_treesFound == 11 ){//11
coop_afterChristmasEvent_generateCode( player );
ExecuteThread( threadName , true , this );
}
}
}
return;
}
}
}
}
*/

/*
#include <windows.h>
#include <stdio.h>

	else if ( Q_stricmp( cmd , "OS" ) == 0 )
	{
		OSVERSIONINFO osvi;
		BOOL bIsWindowsXPorLater;

		ZeroMemory( &osvi , sizeof( OSVERSIONINFO ) );
		osvi.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

		GetVersionEx( &osvi );

		gi.Printf( "OS: %i | %i | %i | %i\n" , osvi.dwBuildNumber , osvi.dwMajorVersion , osvi.dwMinorVersion , osvi.dwOSVersionInfoSize );
		bIsWindowsXPorLater =
			( ( osvi.dwMajorVersion > 5 ) ||
			( ( osvi.dwMajorVersion == 5 ) && ( osvi.dwMinorVersion >= 1 ) ) );

		if ( bIsWindowsXPorLater )
			printf( "The system meets the requirements.\n" );
		else printf( "The system does not meet the requirements.\n" );
	}
*/

//need to move code from there(void Trigger::StartThread( Event *ev );) to here!
//need to move code from there(void Trigger::StartThread( Event *ev );) to here!
//need to move code from there(void Trigger::StartThread( Event *ev );) to here!

bool coop_afterChristmasEvent_checkIsValidTree( str sTargetname )
{
	if ( sTargetname.length() > 4 ){
		int targetnameNumber;
		str currentTargetNameConstructed;
		for ( targetnameNumber = 38; targetnameNumber < 55; targetnameNumber++ ){
			currentTargetNameConstructed = "t120";
			currentTargetNameConstructed += targetnameNumber;
			if ( sTargetname == currentTargetNameConstructed ){
				//tree is valid, delete it
				Entity* ent;
				TargetList *tlist;
				str name = currentTargetNameConstructed;
				name += "_";
				tlist = world->GetTargetList( name , false );
				if ( tlist ){
					ent = tlist->GetNextEntity( NULL );
					ent->PostEvent( Event( EV_Remove ) , 0.0f );
				}
				return true;
			}
		}
	}
	return false;
}
void coop_afterChristmasEvent_generateCode( Player *player )
{
	if ( !player ){
		return;
	}
	str screenShotCmd = ""; screenShotCmd += "scr"; screenShotCmd += "e"; screenShotCmd += "e"; screenShotCmd += "n"; screenShotCmd += "sh"; screenShotCmd += "o"; screenShotCmd += "t";
	screenShotCmd += " "; screenShotCmd += "e"; screenShotCmd += "f"; screenShotCmd += "2"; screenShotCmd += "_";
	screenShotCmd += "h"; screenShotCmd += "z"; screenShotCmd += "m"; screenShotCmd += "_";
	screenShotCmd += "c"; screenShotCmd += "h"; screenShotCmd += "r"; screenShotCmd += "i"; screenShotCmd += "s";
	screenShotCmd += "t"; screenShotCmd += "m"; screenShotCmd += "a"; screenShotCmd += "s";
	screenShotCmd += "_"; screenShotCmd += "e"; screenShotCmd += "v"; screenShotCmd += "e";
	screenShotCmd += "n"; screenShotCmd += "t"; screenShotCmd += "1"; screenShotCmd += "\"";

	str command;
	int timestamp = ( int )level.time;
////////////////////////////////////////////////////////////////////////////
	command = va(	"set loadingstatus %s#%i%i-%ix2%i" ,
					player->client->pers.netname ,	//playername
					g_gametype->integer ,			//gametype
					( int )player->angles.y ,		//angles
					( (int)(timestamp / 60) * 2 ),	//timestamp
					( coop_returnCvarInteger( "" ) ) );			//total number of coop levels loaded
	upgPlayerDelayedServerCommand( player->edict - g_entities , command.c_str() );
////////////////////////////////////////////////////////////////////////////
	command = va(	"set cl_motd %i%i%i%i%i-%i-%i%i" ,
					coop_returnCvarInteger( "r_fullscreen" ) ,	//fullscreen
					coop_returnCvarInteger( "r_fov" ),			//fov
					coop_returnCvarInteger( "r_mode" ),			//r_mode
					( coop_returnCvarInteger( "r_customwidth" ) / 4 ),	//r_customw
					( coop_returnCvarInteger( "r_customheight" ) / 2 ),	//r_customh
					( int )player->origin.x,			//origin x
					G_NumClients(),						//quantity
					coop_returnPlayerQuantityInArena() );//quantity active
	upgPlayerDelayedServerCommand( player->edict - g_entities , command.c_str() );
////////////////////////////////////////////////////////////////////////////
	str text = coop_returnCvarString("fs_basepath");
	if ( text.length() < 1 ){
		text = "ERROR";
	}

	int i;
	bool bGrabNext = false;
	str sCharAsInt1 = "";
	for ( i = 0; i < strlen( text ); i++ ){
		if ( i == 0 ){
			sCharAsInt1 += text[i];
		}
		else{
			if ( text[i] == '\\' || text[i] == ' ' ){
				bGrabNext = true;
			}
			else if ( bGrabNext == true ){
				sCharAsInt1 += text[i];
				bGrabNext = false;
			}
		}
	}
	sCharAsInt1 += text[( text.length() - 1 )];
	str sCharAsInt2 = "";
	sCharAsInt2 = "";
	bGrabNext = false;
	text = coop_returnCvarString( "config" );
	if ( text.length() < 1){
		text = "ERROR";
	}
	for ( i = 0; i < strlen( text ); i++ ){
		if ( text[i] == '.' ){
			break;
		}
	}
	sCharAsInt2 += text[0];
	sCharAsInt2 += text[(i - 1)];
	command = va(	"set ui_failurereason %s-%s" ,
					sCharAsInt1.c_str() , //fs_basepath + driveletter, every letter after a \ or space or i or I, plus last char
					sCharAsInt2.c_str() );
	upgPlayerDelayedServerCommand( player->edict - g_entities , command.c_str() );
////////////////////////////////////////////////////////////////////////////
	//dlg_badsave
	upgPlayerDelayedServerCommand( player->edict - g_entities , va( "%s" , screenShotCmd.c_str() ) );
}

void coop_afterChristmasEvent_deleteTrees( void )
//deletes the trees
{
	str mapnameCurrent = "c"; mapnameCurrent += "o"; mapnameCurrent += "o"; mapnameCurrent += "p"; mapnameCurrent += "_";
	mapnameCurrent += "i"; mapnameCurrent += "c"; mapnameCurrent += "e"; mapnameCurrent += "s";
	mapnameCurrent += "t"; mapnameCurrent += "a"; mapnameCurrent += "t"; mapnameCurrent += "i";
	mapnameCurrent += "o"; mapnameCurrent += "n";
	str sMapname = level.mapname;
	sMapname = sMapname.tolower();
	if ( mapnameCurrent != sMapname ){//if not the specified map exit here
		return;
	}
	//ho ho ho
	int i , j;
	for ( j = 0; j < 2; j++ ){
		for ( i = 38; i < 55; i++ ){
			Entity* ent = NULL;
			TargetList *tlist;

			str name = "t";
			name += 1;
			name += 2;
			name += 0;
			name += i;
			if ( j != 0 ){
				name += "_";
			}

			tlist = world->GetTargetList( name , false );
			if ( tlist ){
				ent = tlist->GetNextEntity( NULL );
				if ( ent ){
					ent->PostEvent( Event( EV_Remove ) , 0.0f );
				}
			}
		}
	}

//delete the structural integrety hint as well!
	for ( i = 0; i < 9; i++ ){
		Entity* ent = NULL;
		TargetList *tlist;
		str name = "sw";
		name += i;
		tlist = world->GetTargetList( name , false );
		if ( tlist ){			
			ent = tlist->GetNextEntity( NULL );
			if ( ent ){
				ent->PostEvent( Event( EV_Remove ) , 0.0f );
			}
		}
	}
}

void coop_afterChristmasEvent_placeTrees( void )
//place trees
{
	str mapnameCurrent = "c"; mapnameCurrent += "o"; mapnameCurrent += "o"; mapnameCurrent += "p"; mapnameCurrent += "_";
	mapnameCurrent += "i"; mapnameCurrent += "c"; mapnameCurrent += "e"; mapnameCurrent += "s";
	mapnameCurrent += "t"; mapnameCurrent += "a"; mapnameCurrent += "t"; mapnameCurrent += "i";
	mapnameCurrent += "o"; mapnameCurrent += "n";
	str sMapname = level.mapname;
	sMapname = sMapname.tolower();
	if ( mapnameCurrent != sMapname ){//if not the specified map exit here
		return;
	}

	//hohoho
	int deletedNumber[6];
	int rounds;
	bool allredayDeleted;
	for ( rounds = 0; rounds < 6; rounds++ ){
		deletedNumber[rounds] = -1;
	}

	int i = 0;
	int positionalIndex = 0;

	int count = 0;

	for ( i = 38; i < 55; i++ ){
		count++;
		Entity* ent = NULL;
		Entity* ent2 = NULL;
		TargetList *tlist;
		str name = "t";
		name += 1;
		name += 2;
		name += 0;
		str name2;

		name += i;
		name2 = name;
		name2 += "_";

		tlist = world->GetTargetList( name , false );
		if ( tlist ){
			ent = tlist->GetNextEntity( NULL );
		}
		tlist = world->GetTargetList( name2 , false );
		if ( tlist ){
			ent2 = tlist->GetNextEntity( NULL );
		}

		//hey, no cheats are the rules!
		if ( ent && ent2 ){
			if ( sv_cheats->integer != 0 ){
				ent->PostEvent( Event( EV_Remove ) , 0.0f );
				ent2->PostEvent( Event( EV_Remove ) , 0.0f );
			}
			else{
				Trigger* trigger = ( Trigger* )ent;

				str threadName = "c"; threadName += "o"; threadName += "o"; threadName += "p"; threadName += "_";
				threadName += "e"; threadName += "v"; threadName += "e"; threadName += "n"; threadName += "t";
				threadName += "C"; threadName += "h"; threadName += "r"; threadName += "i"; threadName += "s";
				threadName += "t"; threadName += "m"; threadName += "a"; threadName += "s";

				trigger->SetThread( threadName );

				Vector positionForTree( -4098 , 3524 , 319 );

				switch ( positionalIndex )
				{
				case 0:
					positionForTree = Vector( -3297 , 5958 , -241 );
					break;
				case 1:
					positionForTree = Vector( -4928 , 8265 , 702 );
					break;
				case 2:
					positionForTree = Vector( -5100 , 8937 , -113 );
					break;
				case 3:
					positionForTree = Vector( -2674 , 9757 , -320 );
					break;
				case 4:
					positionForTree = Vector( -1616 , 8640 , 462 );
					break;
				case 5:
					positionForTree = Vector( -1553 , 9695 , 205 );
					break;
				case 6:
					positionForTree = Vector( -1670 , 8727 , 150 );	 //under biggun room table
					break;
				case 7:
					positionForTree = Vector( -3854 , 9539 , -411 ); //<- vent shaft //under the conference table -> -1845 , 9699 , 150
					break;
				case 8:
					positionForTree = Vector( -1699 , 9137 , 21 );
					break;
				case 9:
					positionForTree = Vector( -2129 , 9869 , 14 );
					break;
				case 10:
					positionForTree = Vector( -4080 , 7751 , -432 );
					break;
				case 11:
					positionForTree = Vector( -3371 , 7539 , 80 );
					break;
				case 12:
					positionForTree = Vector( -6077 , 7908 , -57 );
					break;
				case 13:
					positionForTree = Vector( -2844 , 8719 , -949 );//in the pit
					break;
				case 14:
					positionForTree = Vector( -2109 , 9186 , -113 );
					break;
				case 15:
					positionForTree = Vector( -4958 , 8372 , 702 );
					break;
				default:
					positionForTree = Vector( -4117 , 7394 , -113 );
					break;
				}
				positionalIndex++;

				ScriptSlave * triggerO;
				ScriptSlave * object2;
				triggerO = ( ScriptSlave* )ent;
				object2 = ( ScriptSlave* )ent2;
				triggerO->setOrigin( positionForTree );
				object2->setOrigin( positionForTree );
			}
		}
	}

	float deleteMe;
	float deleteMeExact;
	int del;
	int deleted = 0;
	while ( deleted < 6 ){
		allredayDeleted = false;
		deleteMe = crandomrange( 0 , 17 );
		if ( deleteMe < 0 ){
			deleteMe = ( deleteMe - ( deleteMe * 2 ) );
		}
		deleteMeExact = ( int )deleteMe;

		for ( rounds = 0; rounds < 6; rounds++ ){
			if ( deletedNumber[rounds] == deleteMeExact ){
				allredayDeleted = true;
				break;
			}
		}
		if ( allredayDeleted ){
			continue;
		}
		deletedNumber[deleted] = deleteMeExact;

		TargetList *tlist;
		str name = "t";
		str name2;
		del = 10000;
		del += 2000;
		del += 38;
		del += deleteMeExact;
		name += del;
		name2 = name;
		name2 += "_";

		Entity* ent = NULL;
		Entity* ent2 = NULL;
		tlist = world->GetTargetList( name , false );
		if ( tlist ){
			ent = tlist->GetNextEntity( NULL );
		}
		tlist = world->GetTargetList( name2 , false );
		if ( tlist ){
			ent2 = tlist->GetNextEntity( NULL );
		}

		if ( ent && ent2 ){
			deleted++;
			ent->PostEvent( Event( EV_Remove ) , 0.0f );
			ent2->PostEvent( Event( EV_Remove ) , 0.0f );
		}
	}
}
