//-----------------------------------------------------------------------------
//
//  $Logfile:: /EF2/Code/DLLs/game/closeInOnPlayer.cpp              $
// $Revision:: 4                                                              $
//   $Author:: Singlis                                                        $
//     $Date:: 9/26/03 2:36p                                                  $
//
// Copyright (C) 2002 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
//	CloseInOnPlayer Implementation
// 
// PARAMETERS:
//		
//
// ANIMATIONS:
//		
//--------------------------------------------------------------------------------
#include "coopReturn.hpp"

#include "actor.h"
#include "closeInOnPlayer.hpp"
#include "mp_manager.hpp"

//--------------------------------------------------------------
//
// Class Declaration and Event Registration
//
//--------------------------------------------------------------
CLASS_DECLARATION( Behavior, CloseInOnPlayer, NULL )
	{
		{ &EV_Behavior_Args,			&CloseInOnPlayer::SetArgs },      
		{ NULL, NULL }
	};


//--------------------------------------------------------------
// Name:		CloseInOnPlayer()
// Class:		CloseInOnPlayer
//
// Description:	Constructor
//
// Parameters:	None
//
// Returns:		None
//--------------------------------------------------------------
CloseInOnPlayer::CloseInOnPlayer()
{
	_anim = "";
	_torsoAnim = "";
	_dist = 64.0f;
}

//--------------------------------------------------------------
// Name:		~CloseInOnPlayer()
// Class:		CloseInOnPlayer
//
// Description:	Destructor
//
// Parameters:	None
//
// Returns:		None
//--------------------------------------------------------------
CloseInOnPlayer::~CloseInOnPlayer()
{
}


//--------------------------------------------------------------
//
// Name:        SetArgs()
// Class:       CloseInOnPlayer
//
// Description: 
//
// Parameters:  Event *ev -- Event containing the string
//
// Returns:     None
//
//--------------------------------------------------------------
void CloseInOnPlayer::SetArgs( Event *ev )
{
	_anim = ev->GetString( 1 );

	if ( ev->NumArgs() > 1 )
		_dist = ev->GetFloat( 2 );
 
	if ( ev->NumArgs() > 2 )
		_torsoAnim = ev->GetString( 3 );
}



//--------------------------------------------------------------
//
// Name:        Begin()
// Class:       CloseInOnPlayer
//
// Description: Initializes the behavior
//
// Parameters:  Actor &self -- The actor executing this behavior
//
// Returns:     None
//
//--------------------------------------------------------------
void CloseInOnPlayer::Begin( Actor &self )
{          
	init( self );
}



//--------------------------------------------------------------
//
// Name:        Evaluate()
// Class:       CloseInOnPlayer
//
// Description: Update for this behavior -- called every server frame
//
// Parameters:  Actor &self -- Actor executing this behavior
//
// Returns:     BehaviorReturnCode_t
//
//--------------------------------------------------------------
BehaviorReturnCode_t	CloseInOnPlayer::Evaluate( Actor &self )
{

	BehaviorReturnCode_t stateResult;


	think();

	switch ( _state )
		{
		//---------------------------------------------------------------------
		case CLOSE_IN_ON_PLAYER_APPROACH:
		//---------------------------------------------------------------------
			stateResult = evaluateStateApproach();
			if ( stateResult == BEHAVIOR_FAILED )
				transitionToState( CLOSE_IN_ON_PLAYER_FAILED );

			if ( stateResult == BEHAVIOR_FAILED_STEERING_NO_PATH )
				transitionToState( CLOSE_IN_ON_PLAYER_FAILED );

			if ( stateResult == BEHAVIOR_SUCCESS )
				transitionToState( CLOSE_IN_ON_PLAYER_SUCCESS );
		break;

		//---------------------------------------------------------------------
		case CLOSE_IN_ON_PLAYER_SUCCESS:
		//---------------------------------------------------------------------
			return BEHAVIOR_SUCCESS;
		break;

		//---------------------------------------------------------------------
		case CLOSE_IN_ON_PLAYER_FAILED:
		//---------------------------------------------------------------------
			return BEHAVIOR_FAILED;
		break;
		}
	
	return BEHAVIOR_EVALUATING;
}



//--------------------------------------------------------------
//
// Name:        End()
// Class:       CloseInOnPlayer
//
// Description: Ends this behavior -- cleans things up
//
// Parameters:  Actor &self -- Actor executing this behavior
//
// Returns:     None
//
//--------------------------------------------------------------
void CloseInOnPlayer::End(Actor &self)
{   
	_chase.End( self );
}


//--------------------------------------------------------------
// Name:		transitionToState()
// Class:		CloseInOnPlayer
//
// Description:	Transitions the behaviors state
//
// Parameters:	coverCombatStates_t state -- The state to transition to
//
// Returns:		None
//--------------------------------------------------------------
void CloseInOnPlayer::transitionToState( closeInOnPlayerStates_t state )
{
	switch( state )
		{
		case CLOSE_IN_ON_PLAYER_APPROACH:
			setupStateApproach();
			setInternalState( state , "CLOSE_IN_ON_PLAYER_APPROACH" );
		break;

		case CLOSE_IN_ON_PLAYER_SUCCESS:
			setInternalState( state , "CLOSE_IN_ON_PLAYER_SUCCESS" );
		break;

		case CLOSE_IN_ON_PLAYER_FAILED:
			setInternalState( state , "CLOSE_IN_ON_PLAYER_FAILED" );
		break;
		}
}


//--------------------------------------------------------------
// Name:		setInternalState()
// Class:		CloseInOnPlayer
//
// Description:	Sets the internal state of the behavior
//
// Parameters:	unsigned int state
//				const str &stateName
//
// Returns:		None
//--------------------------------------------------------------
void CloseInOnPlayer::setInternalState( closeInOnPlayerStates_t state , const str &stateName )
{
	_state = state;
	SetInternalStateName( stateName );	
}

//--------------------------------------------------------------
// Name:		init()
// Class:		CloseInOnPlayer
//
// Description:	Initializes the behavior
//
// Parameters:	Actor &self
//
// Returns:		None
//--------------------------------------------------------------
void CloseInOnPlayer::init( Actor &self )
{
	_self = &self;
	findPlayer();	
	transitionToState(CLOSE_IN_ON_PLAYER_APPROACH);
}

//--------------------------------------------------------------
// Name:		think()
// Class:		CloseInOnPlayer
//
// Description:	Does any processing required before evaluating states
//
// Parameters:	None
//
// Returns:		None
//--------------------------------------------------------------
void CloseInOnPlayer::think()
{	
}


//--------------------------------------------------------------
// Name:		findPlayer()
// Class:		CloseInOnPlayer
//
// Description:	Sets our _currentEnemy
//
// Parameters:	None
//
// Returns:		None
//--------------------------------------------------------------
void CloseInOnPlayer::findPlayer()
{
	Entity *player = NULL;

	//hzm coop mod chrissstrahl
	if ( g_gametype->integer != GT_SINGLE_PLAYER ){

		//[b607] chrissstrahl - use current enemy or follow target if it is valid player
		//[b607] This might need some finetunning), especially the player validation process
		//[b607] We are accepting dead players
		Entity *eCurr = (Entity *)_self->enemyManager->GetCurrentEnemy();
		if (eCurr && eCurr->isSubclassOf(Player) && multiplayerManager.isPlayerSpectator((Player*)eCurr)) {

			if (game.coop_isActive) {
				Player *pl = (Player*)eCurr;
				//if player did not re/spawn after he died and some time has passed this player is no longer valid
				if( pl->coopPlayer.lastTimeSpawned < pl->coop_playerDiedLast() && (pl->coop_playerDiedLast() + 30.0f) > level.time) {
					player = (Player*)eCurr;
				}
			}
		}
		
		//[b607] chrissstrahl - if not a valid player, use follow target if valid player
		if (!player) {
			eCurr = (Entity *)_self->followTarget.specifiedFollowTarget;
			if (eCurr && eCurr->isSubclassOf(Player) && !multiplayerManager.isPlayerSpectator((Player*)eCurr)) {
				player = (Player*)eCurr;
			}
		}

		//[b607] chrissstrahl - if not a valid player, use closest valid player
		if (!player) {
			player = coop_returnPlayerClosestTo( ( Entity * )_self );
		}
	}
	else
	{
		player = ( Entity* )GetPlayer( 0 );
	}

	if ( !player )
		{
		SetFailureReason( "CloseInOnPlayer::findPlayer -- No Player" );
		transitionToState( CLOSE_IN_ON_PLAYER_FAILED );
		return;
		}

	//hzm gameupate chrissstrahl - player is in spec
	if ( g_gametype->integer != GT_SINGLE_PLAYER && multiplayerManager.isPlayerSpectator( (Player *)player ) )
	{
		SetFailureReason( "CloseInOnPlayer::findPlayer -- Player is in spectator" );
		transitionToState( CLOSE_IN_ON_PLAYER_FAILED );
		return;
	}

	if ( player->flags & FL_NOTARGET )
	{
		SetFailureReason( "CloseInOnPlayer::findPlayer -- Player has notarget on" );
		transitionToState( CLOSE_IN_ON_PLAYER_FAILED );
		return;
	}

	_player = player;
}

//--------------------------------------------------------------
// Name:		setTorsoAnim()
// Class:		CloseInOnPlayer
//
// Description:	Sets our Torso Animation
//
// Parameters:	None
//
// Returns:		None
//--------------------------------------------------------------
void CloseInOnPlayer::setTorsoAnim()
{
	_self->SetAnim( _torsoAnim , NULL , torso );
}

//--------------------------------------------------------------
// Name:		setupStateApproach()
// Class:		CloseInOnPlayer
//
// Description:	Sets up State
//
// Parameters:	None
//
// Returns:		None
//--------------------------------------------------------------
void CloseInOnPlayer::setupStateApproach()
{
   _chase.SetAnim( _anim );
   _chase.SetDistance( _dist );   
   _chase.SetEntity( *_self, _player );

   if ( _torsoAnim.length() > 0 )
	   setTorsoAnim();

   _chase.Begin( *_self );
}

//--------------------------------------------------------------
// Name:		evaluateStateApproach()
// Class:		CloseInOnPlayer
//
// Description:	Evaluates State
//
// Parameters:	None
//
// Returns:		BehaviorReturnCode_t
//--------------------------------------------------------------
BehaviorReturnCode_t CloseInOnPlayer::evaluateStateApproach()
{
	return _chase.Evaluate( *_self );
}

//--------------------------------------------------------------
// Name:		failureStateApproach()
// Class:		CloseInOnPlayer
//
// Description:	Failure Handler for State
//
// Parameters:	const str &failureReason
//
// Returns:		None
//--------------------------------------------------------------
void CloseInOnPlayer::failureStateApproach( const str& failureReason )
{
}
