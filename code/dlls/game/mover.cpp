//-----------------------------------------------------------------------------
//
//  $Logfile:: /EF2/Code/DLLs/game/mover.cpp                                 $
// $Revision:: 11                                                             $
//   $Author:: Singlis                                                        $
//     $Date:: 9/26/03 2:36p                                                  $
//
// Copyright (C) 1997 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
// Base class for any object that needs to move to specific locations over a
// period of time.  This class is kept separate from most entities to keep
// class size down for objects that don't need such behavior.
//

#include "_pch_cpp.h"
#include "entity.h"
#include "trigger.h"
#include "mover.h"

CLASS_DECLARATION( Listener, Mover, "mover" )
{
	{ &EV_MoveDone,						&Mover::MoveDone },

   { NULL, NULL }
};


Mover::Mover()
{
	// Should always use other constructor
	
	assert( 0 );
}

Mover::Mover( Entity *ent )
{
	self = ent;
	
	self->edict->s.pos.trType = TR_LERP;
	endevent = NULL;
	finaldest = ent->origin;
}

Mover::~Mover()
{
}

void Mover::MoveDone( Event *ev )
{
	Event * event;
	Vector move;
	Vector amove;
	
	// zero out the movement
	if ( moveflags & MOVE_ANGLES )
	{
		self->avelocity = vec_zero;
		amove = angledest - self->localangles;
	}
	else
	{
		amove = vec_zero;
	}
	
	if ( moveflags & MOVE_ORIGIN )
	{
		self->velocity	= vec_zero;
		move = finaldest - self->GetLocalOrigin();
	}
	else
	{
		move = vec_zero;
	}
	
	if ( !G_PushMove( self, move, amove ) )
	{
		// Delay finish till we can move into the final position
		PostEvent( EV_MoveDone, FRAMETIME );
		return;
	}
	
	//
	// After moving, set origin to exact final destination
	//
	if ( moveflags & MOVE_ORIGIN )
	{
		self->setOrigin( finaldest );
	}
	
	if ( moveflags & MOVE_ANGLES )
	{
		self->localangles = angledest;
		
		if ( ( self->localangles.x >= 360.0f ) || ( self->localangles.x < 0.0f ) )
		{
			self->localangles.x -= (float)( ( (int)self->localangles.x / 360 ) * 360 );
		}
		if ( ( self->localangles.y >= 360.0f ) || ( self->localangles.y < 0.0f ) )
		{
			self->localangles.y -= (float)( ( (int)self->localangles.y / 360 ) * 360 );
		}
		if ( ( self->localangles.z >= 360.0f ) || ( self->localangles.z < 0.0f ) )
		{
			self->localangles.z -= (float)( ( (int)self->localangles.z / 360 ) * 360 );
		}
	}
	
	event = endevent;
	endevent = NULL;
	self->ProcessEvent( event );
}

/*
=============
MoveTo

calculate self.velocity and self.nextthink to reach dest from
self.origin traveling at speed
===============
*/
void Mover::MoveTo( const Vector &tdest, const Vector &angdest, float tspeed, Event &event )
{
	Vector vdestdelta;
	Vector angdestdelta;
	float  len;
	float  traveltime;
	
	//hzm gameupdate chrissstrahl - no need for a assert here, this error originates from func_ entites from within the level
	//assert( tspeed >= 0.0f );
	
	if ( !tspeed )
	{
		//hzm gameupdate chrissstrahl - added help for mappers
		gi.Printf( "====ERROR HELP====\n" );
		gi.Printf( "MoveTo Error: Caused by eigther a func_door, func_rotatingdoor or func_sinkobject\n" );
		gi.Printf( "You eigther have been closing in on the object or using it, at the moment this error occured\n" );
		gi.Printf( "This error usually occurs if:\n" );
		gi.Printf( "- a func_ entity has time set to 0 and no speed is defined\n" );
		gi.Printf( "- a func_ entity has movespeed set to 0\n" );
		gi.Printf( "====ERROR FIX====\n" );
		gi.Printf( "To fix this, ungroup the entity in question and applay the func_ class again, but it is recommended to delete the entity and start from scratch\n" );
		
		
		error( "MoveTo" , "No speed is defined!" );
	}

	if ( tspeed < 0.0f )
	{
		//hzm gameupdate chrissstrahl - added help for mappers
		gi.Printf( "====ERROR HELP====\n" );
		gi.Printf( "MoveTo Error: Caused by eigther a func_door, func_rotatingdoor or func_sinkobject\n" );
		gi.Printf( "You eigther have been closing in on the object or using it, at the moment this error occured\n" );
		gi.Printf( "This error usually occurs if:\n" );
		gi.Printf( "- a func_ entity is copied and missaligned/damaged in the process\n" );
		gi.Printf( "- a func_ entity is made of patches and resized or damaged\n" );
		gi.Printf( "- a func_ entity is eventually smaller than 9x9x9 units\n" );
		gi.Printf( "- a func_ entity is smaller than its lip value (default is 8 units)\n" );
		gi.Printf( "====ERROR FIX====\n" );
		gi.Printf( "To fix this, ungroup the entity in question and applay the func_ class again, but it is recommended to delete the entity and start from scratch\n" );
		
		
		error( "MoveTo" , "Speed is negative!" );
	}
	
	// Cancel previous moves
	CancelEventsOfType( EV_MoveDone );
	
	moveflags = 0;
	
	if ( endevent )
	{
		delete endevent;
	}
	endevent = new Event( event );
	
	finaldest = tdest;
	angledest = angdest;
	
	if ( finaldest != self->GetLocalOrigin() )
	{
		moveflags |= MOVE_ORIGIN;
	}
	if ( angledest != self->localangles )
	{
		moveflags |= MOVE_ANGLES;
	}
	
	if ( !moveflags )
	{
		// stop the object from moving
		self->velocity = vec_zero;
		self->avelocity = vec_zero;
		
		// post the event so we don't wait forever
		PostEvent( EV_MoveDone, FRAMETIME );
		return;
	}
	
	// set destdelta to the vector needed to move
	vdestdelta = tdest - self->GetLocalOrigin();
	angdestdelta = angdest - self->localangles;
	
	if ( tdest == self->GetLocalOrigin() )
	{
		//hzm gamefix chrissstrahl - fix rotating doors rotating a extra 360 degree
		while ( angdestdelta.y < -360 )
			angdestdelta.y = angdestdelta.y + 360;

		while ( angdestdelta.y > 360 )
			angdestdelta.y = angdestdelta.y - 360;
		//end of hzm

		// calculate length of vector based on angles
		len = angdestdelta.length();
	}
	else
	{
		// calculate length of vector based on distance
		len = vdestdelta.length();
	}
	
	// divide by speed to get time to reach dest
	traveltime = len / tspeed;
	
	// Quantize to FRAMETIME
	// E3 HACK
	//	traveltime *= ( 1 / FRAMETIME );
	//	traveltime = ( float )( (int)traveltime ) * FRAMETIME;
	if ( traveltime < FRAMETIME )
	{
		traveltime = FRAMETIME;
		vdestdelta = vec_zero;
		angdestdelta = vec_zero;
	}
	
	// scale the destdelta vector by the time spent traveling to get velocity
	if ( moveflags & MOVE_ORIGIN )
	{
		self->velocity = vdestdelta * ( 1.0f / traveltime );
	}
	
	if ( moveflags & MOVE_ANGLES )
	{
		self->avelocity = angdestdelta * ( 1.0f / traveltime );
	}
	
	PostEvent( EV_MoveDone, traveltime );
}

/*
=============
LinearInterpolate
===============
*/
void Mover::LinearInterpolate( const Vector &tdest, const Vector &angdest, float time, Event &event )
{
	Vector vdestdelta;
	Vector angdestdelta;
	float t;
	
	if ( endevent )
	{
		delete endevent;
	}
	endevent = new Event( event );
	finaldest = tdest;
	angledest = angdest;
	
	// Cancel previous moves
	CancelEventsOfType( EV_MoveDone );
	
	// Quantize to FRAMETIME
	//E3 HACK
	//	time *= ( 1 / FRAMETIME );
	//	time = ( float )( (int)time ) * FRAMETIME;
	if ( time < FRAMETIME )
	{
		time = FRAMETIME;
	}
	
	moveflags = 0;
	t = 1.0f / time;
	// scale the destdelta vector by the time spent traveling to get velocity
	if ( finaldest != self->GetLocalOrigin() )
	{
		vdestdelta = tdest - self->GetLocalOrigin();
		self->velocity = vdestdelta * t;
		moveflags |= MOVE_ORIGIN;
	}
	
	if ( angledest != self->localangles )
	{
		angdestdelta = angdest - self->localangles;
		self->avelocity = angdestdelta * t;
		moveflags |= MOVE_ANGLES;
	}
	
	PostEvent( EV_MoveDone, time );
}

void Mover::SetEndEvent( const int endEvent )
{
	if ( endevent )
	{
		delete endevent;
	}
	endevent = new Event( endEvent );
}
