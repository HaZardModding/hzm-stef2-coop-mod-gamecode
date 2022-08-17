//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/ipfilter.cpp                                  $
// $Revision:: 5                                                              $
//     $Date:: 12/19/02 7:06p                                                 $
//
// Copyright (C) 1999 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// PACKET FILTERING
//
// You can add or remove addresses from the filter list with:
//
// addip <ip>
// removeip <ip>
//
// The ip address is specified in dot format, and any unspecified digits will match
// any value, so you can specify an entire class C network with "addip 192.246.40".
//
// Removeip will only remove an address specified exactly the same way.  You cannot
// addip a subnet, then removeip a single host.
//
// listip
// Prints the current list of filters.
//
// writeip
// Dumps "addip <ip>" commands to listip.cfg so it can be execed at a later date.
// The filter lists are not saved and restored by default, because I beleive it would
// cause too much confusion.
//
// filterban <0 or 1>
//
// If 1 (the default), then ip addresses matching the current list will be prohibited
// from entering the game.  This is the default setting.
//
// If 0, then only addresses matching the list will be allowed.  This lets you easily
// set up a private game, or a game that only allows players from your local network.
//

#include "_pch_cpp.h"
#include "ipfilter.h"

typedef struct
{
	unsigned	mask;
	unsigned	compare;
} ipfilter_t;

#define	MAX_IPFILTERS	1024

ipfilter_t	ipfilters[ MAX_IPFILTERS ];
int			numipfilters;

/*
=================
StringToFilter
=================
*/
static qboolean StringToFilter( const char *s, ipfilter_t *f )
{
	char	num[ 128 ];
	int	i;
	int   j;
	byte	b[ 4 ];
	byte	m[ 4 ];
	
	for( i = 0; i < 4; i++ )
	{
		b[ i ] = 0;
		m[ i ] = 0;
	}
	
	for( i = 0; i < 4; i++ )
	{
		if ( ( *s < '0' ) || ( *s > '9' ) )
		{
			//hzm gamefix daggolin - show the message to the server
			gi.Printf( "Bad filter address: %s\n", s );
			return false;
		}
		
		j = 0;
		while( ( *s >= '0' ) && ( *s <= '9' ) )
		{
			num[ j++ ] = *s++;
		}
		
		num[ j ] = 0;
		b[ i ] = atoi( num );
		if ( b[ i ] != 0 )
		{
			m[ i ] = 255;
		}
		
		if ( !*s )
		{
			break;
		}
		
		s++;
	}
	
	f->mask = *( unsigned * )m;
	f->compare = *( unsigned * )b;
	
	return true;
}

/*
=================
SV_FilterPacket
=================
*/
qboolean SV_FilterPacket( const char *from )
{
	int i;
	unsigned	in;
	byte m[4] = { 0,0,0,0 }; //[b60011] chrissstrahl - fix accsessing this var uninizialised
	const char *p;
	
	if ( !from )
		return false;
	
	i = 0;
	p = from;
	while( *p && ( i < 4 ) )
	{
		m[ i ] = 0;
		while( ( *p >= '0' ) && ( *p <= '9' ) )
		{
			m[ i ] = m[ i ] * 10 + ( *p - '0' );
			p++;
		}
		
		if ( !*p || ( *p == ':' ) )
		{
			break;
		}
		
		i++;
		p++;
	   }
	
	in = *( unsigned * )m;
	for( i = 0; i < numipfilters; i++ )
	{
		if ( ( in & ipfilters[ i ].mask ) == ipfilters[ i ].compare )
		{
			return ( int )filterban->integer;
		}
	}
	
	return !( int )filterban->integer;
}


/*
=================
SV_AddIP_f
=================
*/
void SVCmd_AddIP_f( void )
{
	int i;
	
	if ( gi.argc() < 2 )
	{
		//hzm gamefix daggolin - show the message to the server
		gi.Printf( "Usage: addip <ip-mask>\n" );
		return;
	}
	
	for( i = 0; i < numipfilters; i++ )
	{
		if ( ipfilters[ i ].compare == 0xffffffff )
		{
			// free spot
			break;
		}
	}
	
	if ( i == numipfilters )
	{
		if ( numipfilters == MAX_IPFILTERS )
		{
			//hzm gamefix daggolin - show the message to the server
			gi.Printf( "IP filter list is full\n" );
			return;
		}
		numipfilters++;
	}
	
	if ( !StringToFilter( gi.argv( 1 ), &ipfilters[ i ] ) )
	{
		ipfilters[ i ].compare = 0xffffffff;
	}
}

/*
=================
SV_RemoveIP_f
=================
*/
void SVCmd_RemoveIP_f( void )
{
	ipfilter_t	f;
	int			i;
	int         j;
	
	if ( gi.argc() < 2 )
	{
		//hzm gamefix daggolin - show the message to the server
		gi.Printf( "Usage: removeip <ip-mask>\n" );
		return;
	   }
	
	if ( !StringToFilter( gi.argv( 1 ), &f ) )
	{
		return;
	}
	
	for( i = 0; i < numipfilters; i++ )
	{
		if ( ( ipfilters[ i ].mask == f.mask ) && ( ipfilters[ i ].compare == f.compare ) )
		{
			for ( j = i + 1; j < numipfilters; j++ )
            {
				ipfilters[ j - 1 ] = ipfilters[ j ];
            }
			
			numipfilters--;
			//hzm gamefix daggolin - show the message to the server
			gi.Printf( "Removed.\n" );
			return;
		}
	}
	
	//hzm gamefix daggolin - show the message to the server
	gi.Printf( "Didn't find %s.\n", gi.argv( 2 ) );
}

/*
=================
SV_ListIP_f
=================
*/
void SVCmd_ListIP_f( void )
{
	int   i;
	byte	b[ 4 ];
	
	//hzm gamefix daggolin - show the message to the server
	gi.Printf( "Filter list:\n", gi.argv( 1 ) );
	for( i = 0; i < numipfilters; i++ )
   	{
		*( unsigned * )b = ipfilters[ i ].compare;
		//hzm gamefix daggolin - show the message to the server
		gi.Printf( "%3i.%3i.%3i.%3i\n", b[ 0 ], b[ 1 ], b[ 2 ], b[ 3 ] );
	}
}

/*
=================
SV_WriteIP_f
=================
*/
void SVCmd_WriteIP_f( void )
{
	FILE	 *f;
	char	 name[ MAX_OSPATH ];
	byte	 b[ 4 ];
	int	 i;
	
	sprintf( name, "%s/listip.cfg", GAME_NAME );
	//hzm gamefix daggolin - show the message to the server
	gi.Printf( "Writing %s.\n", name );
	
	f = fopen( name, "wb" );
	if ( !f )
	{
		//hzm gamefix daggolin - show the message to the server
		gi.Printf( "Couldn't open %s.\n", name );
		return;
	}
	
	fprintf( f, "set filterban %d\n", ( int )filterban->integer );
	
	for( i = 0; i < numipfilters; i++ )
	{
		*( unsigned * )b = ipfilters[ i ].compare;
		fprintf( f, "addip %i.%i.%i.%i\n", b[ 0 ], b[ 1 ], b[ 2 ], b[ 3 ] );
	}
	
	fclose( f );
}

/*
=================
G_ServerCommand

G_ServerCommand will be called when an "sv" command is issued.
The game can issue gi.argc() / gi.argv() commands to get the rest
of the parameters
=================
*/
bool G_ServerCommand( void )
{
	const char *cmd;
	cmd = gi.argv(0);

	//hzm gamefix daggolin - console commands - Changed from "1" to "0"
	if ( Q_stricmp( cmd, "addip" ) == 0 )
	{
		SVCmd_AddIP_f();
	}
	else if ( Q_stricmp( cmd, "removeip" ) == 0 )
	{
		SVCmd_RemoveIP_f();
	}
	else if ( Q_stricmp( cmd, "listip" ) == 0 )
	{
		SVCmd_ListIP_f();
	}
	else if ( Q_stricmp( cmd, "writeip" ) == 0 )
	{
		SVCmd_WriteIP_f();
	}
	//[b607] chrissstrahl new
	else if (Q_stricmp(cmd, "login") == 0)
	{
		gi.Printf("This is a test - You logged in.\n");
	}
	//[b607] chrissstrahl - allow to kick all bots
	else if (Q_stricmp(cmd, "kickbots") == 0)
	{
		gi.Printf("INFO: All bots will be kicked now.\n");

		gclient_t	*cl;
		for (int i = 0; i < maxclients->integer; i++) {
			cl = game.clients + i;
			if (!(g_entities[cl->ps.clientNum].svflags & SVF_BOT)) {
				continue;
			}
			gi.SendConsoleCommand(va("kick %i\n", i));
		}
	}
	//hzm gamefix daggolin - show the message to the server
	else {
		if (dedicated->integer) gi.Printf("Unknown server command %s.\n", cmd);
		return false;
	}
	return true;
}
