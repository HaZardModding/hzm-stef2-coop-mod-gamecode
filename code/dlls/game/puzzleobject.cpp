//-----------------------------------------------------------------------------
//
//  $Logfile:: /EF2/Code/DLLs/game/puzzleobject.cpp                           $
// $Revision:: 28                                                             $
//     $Date:: 9/26/03 2:36p                                                  $
//
// Copyright (C) 1999 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//

#include "_pch_cpp.h"

#include "coopReturn.hpp"

#include "puzzleobject.hpp"
#include "sentient.h"
#include "player.h"
#include "equipment.h"
#include "gamecmds.h"


Event EV_PuzzleObject_SetOpenDistance
(
	"puzzleobject_opendistance",
	EV_DEFAULT,
	"f",
	"openDistance",
	"Sets the open distance from player that the puzzle object will open or close"
);
Event EV_PuzzleObject_AnimationDone
(
	"puzzleobject_animdone",
	EV_CODEONLY,
	NULL,
	NULL,
	"Called when the puzzle object's animation is done."
);
Event EV_PuzzleObject_SetItemToUse
(
	"puzzleobject_itemtouse",
	EV_DEFAULT,
	"s",
	"item",
	"The item to use on the puzzle"
);
Event EV_PuzzleObject_SetItemUsedThread
(
	"puzzleobject_itemusedthread",
	EV_DEFAULT,
	"s",
	"threadname",
	"The thread to call when the item is used"
);
Event EV_PuzzleObject_SetFailedThread
(
	"puzzleobject_failedthread",
	EV_DEFAULT,
	"s",
	"threadname",
	"The thread to call when the puzzle fails"
);
Event EV_PuzzleObject_SetSolvedThread
(
	"puzzleobject_solvedthread",
	EV_DEFAULT,
	"s",
	"threadname",
	"The thread to call when the puzzle is solved"
);
Event EV_PuzzleObject_SetCanceledThread
(
	"puzzleobject_canceledthread",
	EV_DEFAULT,
	"s",
	"threadname",
	"The thread to call when the puzzle is canceled"
);
Event EV_PuzzleObject_Failed
(
	"puzzleobject_failed",
	EV_DEFAULT,
	NULL,
	NULL,
	"Received when the puzzle fails"
);
Event EV_PuzzleObject_Canceled
(
	"puzzleobject_canceled",
	EV_DEFAULT,
	NULL,
	NULL,
	"Received when the puzzle is canceled"
);
Event EV_PuzzleObject_Solved
(
	"puzzleobject_solved",
	EV_DEFAULT,
	NULL,
	NULL,
	"Received when the puzzle is solved"
);
Event EV_PuzzleObject_Reset
(
	"puzzleobject_reset",
	EV_DEFAULT,
	NULL,
	NULL,
	"Resets a previously solved puzzle so that it can be triggered again"
);
Event EV_PuzzleObject_TimeToUse
(
	"puzzleobject_timeToUse",
	EV_DEFAULT,
	"f",
	"timeToUse",
	"Makes the puzzle object solved after the user has used it for long enough."
);
Event EV_PuzzleObject_Activate
(
	"puzzleobject_activate",
	EV_DEFAULT,
	NULL,
	NULL,
	"Lets the object think and respond to the player again.  DOES NOT affect any change in animation."
);
Event EV_PuzzleObject_DeActivate
(
	"puzzleobject_deactivate",
	EV_DEFAULT,
	NULL,
	NULL,
	"Makes the object unresponsive to input and the player.  DOES NOT affect any change in animation."
);
Event EV_PuzzleObject_TimerHudName
(
	"puzzleobject_timerHudName",
	EV_DEFAULT,
	"s",
	"hudName",
	"Sets the hud name to use for the timer hud."
);
Event EV_PuzzleObject_BecomeModBarInSkill
(
	"becomeModBarInSkill",
	EV_DEFAULT,
	"f",
	"skill",
	"Tells a puzzleobject to just display a timed modulation bar in any skill less than or equal to the specified one."
);

//[b60011] chrissstrahl - thread called when puzzle is started to be used
Event EV_PuzzleObject_SetUsedStartThread
(
	"puzzleobject_usedStartThread",
	EV_DEFAULT,
	"s",
	"threadname",
	"The thread to call when the puzzle is used, works on any puzzle type"
);

//[b60012] chrissstrahl - get last activating entity for this puzzle
Event EV_PuzzleObject_GetLastActivatingEntity
(
	"getLastActivatingEntity",
	EV_DEFAULT,
	"@e",
	"entity",
	"Returns last entity activating this puzzle"
);

//---------------------------------------------------------
//             PUZZLE OBJECT
//---------------------------------------------------------
/*****************************************************************************/
/*QUAKED puzzle_object (0 0.5 1) (-16 -16 0) (16 16 32) 

Puzzle object is the basic framework for all the puzzles. It communicates 
with the script through events and threads to process the puzzle state.
All logic for the puzzle object exists in the script. 

The puzzle object contains four threads, which are ItemUsed, Solved, Canceled and Failed. 
These are explained in the Key Value Pairs section. The Puzzle object also contains an 
item used string. This allows the level designer to specify the item the player must 
use to activate the puzzle. 

Key Value Pairs:
model						 <model name>	- the tiki model to use for the puzzle object
puzzleobject_opendistance	 <distance>		- the amount of distance the player should be before the puzzle opens.
puzzleobject_itemtouse		 <item name>	- the name of the item the player must use to execute the puzzle
puzzleobject_itemusedthread	 <thread name>	- the name of the thread called when the item is used.
puzzleobject_usedStartThread <thread name>	- [coop] thread called when puzzle is started to be used
puzzleobject_failedthread	 <thread name>	- the name of the thread called when the puzzle fails
puzzleobject_canceledthread	 <thread name>  - the name of the thread called when the puzzle is canceled
puzzleobject_solvedthread	 <thread name>	- the name of the thread called when the puzzle is solved.

******************************************************************************/
CLASS_DECLARATION( Entity, PuzzleObject, "puzzle_object" )
{
	{ &EV_PuzzleObject_SetOpenDistance,		&PuzzleObject::setOpenDistance		},
	{ &EV_PuzzleObject_AnimationDone,		&PuzzleObject::animationDone		},
	{ &EV_PuzzleObject_SetItemToUse,		&PuzzleObject::setItemToUse		},
	{ &EV_PuzzleObject_SetItemUsedThread,	&PuzzleObject::setItemUsedThread	}, //Chrissstrahl - This only works if there is no time set on the puzzle, since we do not want to change the behaviour we added new functionality below
	{ &EV_PuzzleObject_SetUsedStartThread,	&PuzzleObject::setUsedStartThread	}, //[b60011] chrissstrahl - thread called when puzzle is started to be used
	{ &EV_PuzzleObject_GetLastActivatingEntity,		&PuzzleObject::GetLastActivatingEntity }, //[b60012] chrissstrahl - return last activator
	{ &EV_PuzzleObject_SetFailedThread,		&PuzzleObject::setFailedThread		},
	{ &EV_PuzzleObject_SetSolvedThread,		&PuzzleObject::setSolvedThread		},
	{ &EV_PuzzleObject_SetCanceledThread,	&PuzzleObject::setCanceledThread	},
	{ &EV_PuzzleObject_Failed,				&PuzzleObject::failed				},
	{ &EV_PuzzleObject_Canceled,			&PuzzleObject::canceled			},
	{ &EV_PuzzleObject_Solved,				&PuzzleObject::solved				},
	{ &EV_PuzzleObject_Reset,				&PuzzleObject::reset				},
	{ &EV_PuzzleObject_TimeToUse,			&PuzzleObject::setTimeToUse		},
	{ &EV_Use,								&PuzzleObject::useEvent			},
	{ &EV_PuzzleObject_Activate,			&PuzzleObject::activate			},
	{ &EV_PuzzleObject_DeActivate,			&PuzzleObject::deActivate			},
	{ &EV_PuzzleObject_TimerHudName,		&PuzzleObject::setTimerHudName		},
	{ &EV_PuzzleObject_BecomeModBarInSkill, &PuzzleObject::becomeModBarInSkill },
	{NULL, NULL}
};

//[b60012] chrissstrahl - I was so FUCKING tempted to call this cancelculture
//-----------------------------------------------------
void PuzzleObject::cancelPlayer(Player* player)
{
	if (!player) {
		return;
	}
	if (_hudOn) {
		hideTimerHud(player);
		_usedTime = 0.0f;

		//switch to any weapon, avoids the issue that the player might start to modulate to early again
		Weapon* weap = player->BestWeapon();
		if (weap) {
			//player->useWeapon(weap, WEAPON_ANY);
			player->useWeapon(weap, WEAPON_DUAL);
		}
	}
}

//[b60012] chrissstrahl - return last activator
//-----------------------------------------------------
void PuzzleObject::GetLastActivatingEntity(Event *ev)
{
	ev->ReturnEntity(activator);
}

//[b60012] chrissstrahl - return last activator
//-----------------------------------------------------
EntityPtr PuzzleObject::GetLastActivatingEntity()
{
	return activator;
}

//[b60011] chrissstrahl - thread called when puzzle is started to be used
//-----------------------------------------------------
void PuzzleObject::setUsedStartThread(Event* event)
{
	_usedStartThread = event->GetString(1);
}



//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
PuzzleObject::PuzzleObject() 
{
	animate = new Animate( this );

	_saveState = PUZZLE_STATE_IDLE;
	_puzzleState = PUZZLE_STATE_IDLE;
	animate->RandomAnimate( "puzzle_idle" );

	edict->s.eType = ET_MODELANIM;
	_openDistance = 200.0f;
	
	// setup default bounding box if there is no model
	if( model.length() == 0 )
	{
		Vector mins( -16, -16, 0 );
		Vector maxs( 16, 16, 32 );
		this->setSize( mins, maxs );
	}

	_timed = false;
	_timeToUse = 0.0f;
	_hudOn = false;
	_lastTimeUsed = 0.0f;
	_usedTime = 0.0f;

	_nextNeedToUseTime = 0.0f;

	turnThinkOn();

	_hudName = "timerhud";
	_minSkill = -1;

	//hzm coop mod chrissstrahl - added for multiplayer compatibility
	activator = NULL;
	entityVars.SetVariable( "_activator" , -1.0f );
}



//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
PuzzleObject::~PuzzleObject()
{

}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::Think( void )
{
	bool playerNearby = false;
	Player *player = NULL;
	Vector dir;
	float distance;

	// when idle, handle player walking in and out of range
	if( (_puzzleState == PUZZLE_STATE_IDLE) || (_puzzleState == PUZZLE_STATE_IDLE_OPEN) )
	{

		//hzm gamefix chrissstrahl - respond to all players
		// get the player's distance from the puzzle	
		player = coop_returnPlayerClosestTo( this );//player = (Player *)g_entities[ 0 ].entity;		

		if ( player )
		{
			dir = player->origin - origin;
			distance = dir.length();

			if ( distance < _openDistance )
			{
				playerNearby = true;
			}
		}

		// if the player is near, open up
		if( (_puzzleState == PUZZLE_STATE_IDLE) && (playerNearby == true) )
		{
			_puzzleState = PUZZLE_STATE_OPENING;
			animate->RandomAnimate( "puzzle_opening", EV_PuzzleObject_AnimationDone );
		}

		// if the player has moved away, close up
		if( (_puzzleState == PUZZLE_STATE_IDLE_OPEN) && (playerNearby == false) )
		{
			_puzzleState = PUZZLE_STATE_CLOSING;
			animate->RandomAnimate("puzzle_closing", EV_PuzzleObject_AnimationDone );
		}
	}

	if ( ( _timed || (level.getSkill() <= _minSkill) ) 
		&& _hudOn && ( _lastTimeUsed + 0.25 < level.time ) )
	{
		_lastTimeUsed = 0.0f;
		_usedTime = 0.0f;

		//hzm gamefix chrissstrahl - reset variable even when player has left the game, avoid puzzles becomming unresponsive/locked
		_hudOn = false;

		timedPuzzleCanceled( );
	}
}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::setOpenDistance(Event* event)
{
	_openDistance = event->GetFloat(1);
}

void PuzzleObject::setItemToUse(Event* event)
{
	_itemToUse = event->GetString(1);
}
//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::setItemUsedThread(Event* event)
{
	_itemUsedThread = event->GetString(1);
}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::setFailedThread(Event* event)
{
	_failedThread = event->GetString(1);
}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::setSolvedThread(Event* event)
{
	_solvedThread = event->GetString(1);
}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::setCanceledThread(Event* event)
{
	_canceledThread = event->GetString(1);
}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::failed(Event* event)
{
	// close and lapse into locked mode
	_puzzleState = PUZZLE_STATE_CLOSING_LOCKED;
	animate->RandomAnimate( "puzzle_closing", EV_PuzzleObject_AnimationDone );
	
	if(_failedThread.length() != 0)
	{
		ExecuteThread(_failedThread, true, this);
	}
	else
	{
		PostEvent( EV_PuzzleObject_Reset, 1 );
	}
	entityVars.SetVariable( "_activator" , -1.0f );
}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::canceled(Event* event)
{
	// close and lapse into normal mode
	_puzzleState = PUZZLE_STATE_CLOSING;
	animate->RandomAnimate( "puzzle_closing", EV_PuzzleObject_AnimationDone );

	if(_canceledThread.length() != 0)
	{
		ExecuteThread( _canceledThread, true, this);
	}
	else
	{
		PostEvent( EV_PuzzleObject_Reset, 1 );
	}
	entityVars.SetVariable( "_activator" , -1.0f );
}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::solved(Event* event)
{
	// close and lapse into solved mode
	_puzzleState = PUZZLE_STATE_CLOSING_SOLVED;
	animate->RandomAnimate( "puzzle_closing", EV_PuzzleObject_AnimationDone );

	if(_solvedThread.length() != 0)
	{
		ExecuteThread(_solvedThread, true, this);
	}
	entityVars.SetVariable( "_activator" , -1.0f );
}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::animationDone(Event* event)
{
	if( _puzzleState == PUZZLE_STATE_OPENING )
	{
		_puzzleState = PUZZLE_STATE_IDLE_OPEN;
		animate->RandomAnimate( "puzzle_waitingopen" );
	}
	else if( _puzzleState == PUZZLE_STATE_CLOSING )
	{
		_puzzleState = PUZZLE_STATE_IDLE;
		animate->RandomAnimate( "puzzle_idle" );
	}
	else if( _puzzleState == PUZZLE_STATE_CLOSING_SOLVED )
	{
		_puzzleState = PUZZLE_STATE_IDLE_SOLVED;
		animate->RandomAnimate( "puzzle_idle" );
	}
	else if( _puzzleState == PUZZLE_STATE_CLOSING_LOCKED )
	{
		_puzzleState = PUZZLE_STATE_IDLE_LOCKED;
		animate->RandomAnimate( "puzzle_idle" );
	}
}


//-----------------------------------------------------
//
// Name:		
// Class:		
//
// Description:	
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::useEvent(Event* event)
//hzm gamefix chrissstrahl - updated, so it works with any player not just client 0
{
	Entity* entity = NULL;
	Player *player = NULL;

	if ( event->NumArgs() > 0 )
		entity = event->GetEntity( 1 );

	// Don't get retriggered if the puzzle is busy
	// Make sure we are being used by the proper thing

	if(	(_puzzleState != PUZZLE_STATE_IDLE_OPEN) || !entity )
		return;

	//hzm gameupdate chrissstrahl - set activator on puzzle
	player = coop_returnOwnerPlayer( entity );
	activator = ( Entity * )player;
	entityVars.SetVariable( "_activator" , (float)player->entnum );

	//hzm coop mod chrissstrahl - set timestamp on player when he last modulated
	player->coopPlayer.lastTimeUsing = level.time;

	str sCoopClass;
	ScriptVariable *uservar;
	int i;
	//[b60011] chrissstrahl - coop mod supports more than 4 uservars, now 10
	for ( i = 1; i < 11; i++ ){
		uservar = NULL;
		uservar = entityVars.GetVariable( va("uservar%i", i) );
		if ( uservar ){
			if ( !Q_stricmpn( "class " , uservar->stringValue() , 6 ) ){
				sCoopClass = uservar->stringValue();
			}
		}
	}

	//hzm coop mod chrissstrahl - makes puzzles respond only to a specific class
	if ( game.coop_isActive){
		if (sCoopClass.length() > 0) {
			sCoopClass = sCoopClass.tolower();
			float fLastMsgTime;
			uservar = NULL;
			uservar = player->entityVars.GetVariable("_puzzleLastMessage");
			if (!uservar) {
				fLastMsgTime = -1.0f;
			}
			else {
				fLastMsgTime = uservar->floatValue();
			}
			if (!Q_stricmpn("class m", sCoopClass, 7)) {
				if (player->coopPlayer.className != COOP_CLASS_NAME_MEDIC) {
					if ((fLastMsgTime + 3) < level.time) {
						player->entityVars.SetVariable("_puzzleLastMessage", level.time);
						if (coop_checkPlayerLanguageGerman(player)) {
							player->hudPrint(COOP_TEXT_CLASS_MEDIC_ONLY_DEU);
						}
						else {
							player->hudPrint(COOP_TEXT_CLASS_MEDIC_ONLY_ENG);
						}

					}
					return;
				}
			}
			else if (!Q_stricmpn("class t", sCoopClass, 7)) {
				if (player->coopPlayer.className != COOP_CLASS_NAME_TECHNICIAN) {
					if ((fLastMsgTime + 3) < level.time) {
						player->entityVars.SetVariable("_puzzleLastMessage", level.time);
						if (coop_checkPlayerLanguageGerman(player)) {
							player->hudPrint(COOP_TEXT_CLASS_TECHNICIAN_ONLY_DEU);
						}
						else {
							player->hudPrint(COOP_TEXT_CLASS_TECHNICIAN_ONLY_ENG);
						}
					}
					return;
				}
			}
			else if (!Q_stricmpn("class h", sCoopClass, 7)) {
				if (player->coopPlayer.className != COOP_CLASS_NAME_HEAVYWEAPONS) {
					if ((fLastMsgTime + 3) < level.time) {
						player->entityVars.SetVariable("_puzzleLastMessage", level.time);
						if (coop_checkPlayerLanguageGerman(player)) {
							player->hudPrint(COOP_TEXT_CLASS_HEAVYWEAPONS_ONLY_DEU);
						}
						else {
							player->hudPrint(COOP_TEXT_CLASS_HEAVYWEAPONS_ONLY_ENG);
						}
					}
					return;
				}
			}
		}
	}

	if ( _itemToUse.length() > 0 )
	{
		Equipment *equipment;

		// An item is supposed to be used on us
		if ( !entity->isSubclassOf( Equipment ) )
		{
			if ( _nextNeedToUseTime < level.time )
			{
				//hzm gameupdate chrissstrahl - make this multiplayer compatible
				if (entity->isSubclassOf(Player)){
					if ( g_gametype->integer != GT_SINGLE_PLAYER ) {
						static float fLastTime = 0.0f;
						if ( ( fLastTime + 1.5 ) < level.time ) {
							fLastTime = level.time;
							player->hudPrint( va( "$$NeedToUse$$ ^5%s^8\n" , _itemToUse.c_str() ) );
						}
					}
					//end of hzm
					else {
						gi.centerprintf( entity->edict , CENTERPRINT_IMPORTANCE_NORMAL , "$$NeedToUse$$ %s" , _itemToUse.c_str() );
					}
					player->loadUseItem( _itemToUse );
				}

				_nextNeedToUseTime = level.time + 1.0f;
			}
			return;
		}

		equipment = (Equipment*)entity;

		// Make sure this is the correct item
		if ( stricmp( equipment->getTypeName().c_str(), _itemToUse.c_str() ) != 0 )
			return;
	}
	else
	{
		// The player is supposed to use us directly

		if ( !entity->isSubclassOf( Player ) )
		{
			// We're being used by something other than the player

			return;
		}
		player = ( Player * )entity;
	}
	if ( _timed || ( level.getSkill() <= _minSkill ) )
		timedUse( event );
	else
		normalUse( event );
}

void PuzzleObject::normalUse( Event* event )
//used to be void, added event parameter
{
	//[b60011] chrissstrahl - thread called when puzzle is started to be used [_usedStartThread]
	if ( _itemUsedThread.length() <= 0 && _usedStartThread.length() <= 0)
		return;

	Entity* entity = NULL;
	Player* player = NULL;

	if ( event->NumArgs() > 0 )
	{
		entity = event->GetEntity( 1 );

		//hzm gameupdate chrissstrahl - set activator on puzzle
		player = coop_returnOwnerPlayer( entity );
		activator = ( Entity * )player;

		entityVars.SetVariable( "_activator" , (float)player->entnum );
	}
	else
	{
		entityVars.SetVariable( "_activator" , -1.0f );
	}

	// let script take it from here
	if ( _itemUsedThread.length() > 0) { //[b60011] chrissstrahl - thread called when puzzle is started to be used [added check as string could be empty now]
		ExecuteThread(_itemUsedThread, true, this);
	}

	//[b60011] chrissstrahl - thread called when puzzle is started to be used
	if (_usedStartThread.length() > 0) {
		ExecuteThread(_usedStartThread, true, this);
	}

	_puzzleState = PUZZLE_STATE_ACTIVE_OPEN;
	animate->RandomAnimate( "puzzle_openon" );
}

void PuzzleObject::timedUse( Event* event )
//hzm gamefix chrissstrahl - updated, so it works with any player not just client 0
{
	// Make sure we haven't already been used this frame

	if ( _lastTimeUsed >= level.time )
		return;

	//hzm gamefix chrissstrahl - grab the player that is using this puzzle, by grabbing owner of current used entity
	if ( event->NumArgs() == 0 )
		return;

	Player *player = NULL;
	Entity* entity = NULL;
	entity = event->GetEntity( 1 );

	if ( !entity )
		return;

	player = coop_returnOwnerPlayer( entity );

	if ( !player )
		return;
	
	activator = (Entity *)player;
	entityVars.SetVariable( "_activator" , (float)player->entnum );

	float percent;
	Event *eventStat;

	// Turn on the timed hud if we haven't yet

	if ( !_hudOn )
	{
		//[b60011] chrissstrahl - thread called when puzzle is started to be used
		if (_usedStartThread.length() != 0) {
			ExecuteThread(_usedStartThread, true, this);
		}

		showTimerHud( player );
	}

	_lastTimeUsed = level.time;
	_usedTime += level.frametime;

	percent = _usedTime / _timeToUse * 100.0f;

	eventStat = new Event( EV_Player_SetStat );
	eventStat->AddString( "generic" );
	eventStat->AddInteger( ( int )percent );
	player->ProcessEvent( eventStat );

	if ( percent >= 100.0f )
	{
		timedPuzzleSolved( player );
	}
}

void PuzzleObject::timedPuzzleSolved( Player *player )
{
	if ( !player )
		return;

	hideTimerHud( player );

//hzm gameupdate chrissstrahl - puzzleobjects will now activate entities that they are targetting
//the basically work now like triggers when the puzzle has been solved
	Entity	*ent;
	Event	*event;
	const char *name;
	name = Target();
	if ( name && strcmp( name , "" ) )
	{
		ent = NULL;
		do
		{
			ent = G_FindTarget( ent , name );
			if ( !ent )
			{
				break;
			}
//this does not work bellow, so I gave up on it, so much to do, not wasting time with little issues like this
//hzm gameupdate chrissstrahl - we need to activate doors from script
			//if ( ent->isSubclassOf(Door) ){
				//event = new Event( EV_Activate );
				//event->SetSource( EV_FROM_CODE );
				//event->AddEntity( world );
				//ent->ProcessEvent( event );
			//}
//hzm gameupdate chrissstrahl - try activating everything else by the player
			//else{
			event = new Event( EV_Activate );
			event->AddEntity( ( Entity * )player );
			ent->ProcessEvent( event );
			//}
		} while ( 1 );
	}
//EOF GAMEUPDATE

	ProcessEvent( EV_PuzzleObject_Solved );
}

void PuzzleObject::timedPuzzleCanceled( void )
//hzm gamefix chrissstrahl - changed it so it would work with any player not just client 0
{
	if ( activator )
	{
		Entity* ePlayer = (Entity *)activator;
		hideTimerHud( (Player *)ePlayer );
		activator = NULL;
	}

	if ( _canceledThread.length() )
	{
		ProcessEvent( EV_PuzzleObject_Canceled );
	}
}

void PuzzleObject::showTimerHud( Player *player)
//hzm gamefix chrissstrahl - updated, so it works with any player not just client 0
{
	_hudOn = true;

	if ( !player )
		return;

	activator = (Entity *)player;

	str commandString;

	commandString = "pushmenu ";
	commandString += _hudName;
	commandString += "\n";

	G_SendCommandToPlayer( player->edict, commandString.c_str() );	
}

void PuzzleObject::hideTimerHud( Player *player )
//hzm gamefix chrissstrahl - updated, so it works with any player not just client 0
{
	_hudOn = false;

	if ( !player )
		return;

	str commandString;

	commandString = "popmenu ";
	commandString += _hudName;
	commandString += " 1\n";

	G_SendCommandToPlayer( player->edict, commandString.c_str() );
}

//-----------------------------------------------------
//
// Name:		reset
// Class:		PuzzleObject
//
// Description:	Resets the puzzle's state to idle so it will 
//				animate correctly if played again and can be used again
//
// Parameters:	
//
// Returns:		
//-----------------------------------------------------
void PuzzleObject::reset( Event* event )
{
	_puzzleState = PUZZLE_STATE_IDLE;
	animate->RandomAnimate( "puzzle_idle" );

	_lastTimeUsed = 0.0f;
	_usedTime = 0.0f;
	activator = NULL;
	entityVars.SetVariable( "_activator" , -1.0f );
}

void PuzzleObject::activate( Event* event )
{
	_puzzleState = _saveState;
}

void PuzzleObject::deActivate( Event* event )
{
	_saveState = _puzzleState;
	_puzzleState = PUZZLE_STATE_DEACTIVATED;
}

void PuzzleObject::setTimeToUse( Event * ev )
{
	_timeToUse = ev->GetFloat( 1 );

	if ( _timeToUse > 0.0f )
		_timed = true;
}

void PuzzleObject::setTimerHudName( Event * ev )
{
	_hudName = ev->GetString( 1 );
}

void PuzzleObject::becomeModBarInSkill( Event* ev )
{
	_minSkill = (int) ev->GetFloat( 1 );
	if( _timeToUse == 0.0f )
	{
		_timeToUse = 2.0f;
	}
}

