//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/mp_manager.cpp                                $
// $Revision:: 143                                                            $
//   $Author:: Singlis                                                        $
//     $Date:: 9/26/03 3:53p                                                  $
//
// Copyright (C) 2002 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
// DESCRIPTION:
//

#include "_pch_cpp.h"
#include "player.h"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include "powerups.h"
#include "interpreter.h"
#include "program.h"

#include "coopVote.hpp" //[b607] chrissstrahl - moved coop vote stuff to seperate file
#include "coopChallanges.hpp" //[b611] chrissstrahl
CoopChallanges coopChallenges;


MultiplayerManager multiplayerManager;

str playersLastTeam[ MAX_CLIENTS ];

// Setup constants

const int	MultiplayerManager::_playerFreezeTime = 100;
//const int	MultiplayerManager::_maxVoteCount = 3;
const float	MultiplayerManager::_maxVoteTime = 30.0f;
const int	MultiplayerManager::_maxSayStringLength = 150;
const float	MultiplayerManager::_inBetweenMatchTime = 3.0f;

const int	MultiplayerPlayerData::_maxDialogs = 16;

MultiplayerPlayerData::MultiplayerPlayerData()
{
	reset();

	_dialogData = new MultiplayerDialogData[ _maxDialogs ];
}

MultiplayerPlayerData::~MultiplayerPlayerData()
{
	delete [] _dialogData;
}

void MultiplayerPlayerData::reset( void )
{
	_votecount = 0;
	_voted = false;
	_spectator = false;
	_spectatorByChoice = false;
	_valid = false;
	_waitingForRespawn = false;
	_teamHud = "";
	_named = false;

	_nextDialogSendSpot = 0;
	_nextDialogAddSpot = 0;
	_nextDialogSendTime = 0.0f;
	_nextTauntTime = 0.0f;
}

CLASS_DECLARATION( Class, MultiplayerManager, NULL )
{
	{ NULL, NULL }
};

MultiplayerManager::MultiplayerManager()
{
	_inMultiplayerGame = false;

	_multiplayerGame = NULL;

	_playerData = NULL;

	_awardSystem = NULL;

	_gameStarted = false;
	_gameOver    = false;

	_needToAddBots = true;

	_inMatch = false;

	_talkingIconIndex = 0;
	_waitingToRespawnIconIndex = 0;

	_restartMatchTime = 0.0f;

	_voteTime = 0.0f;
}

MultiplayerManager::~MultiplayerManager()
{
	cleanup( false );
}

//
// Interface for game dll
//

void MultiplayerManager::cleanup( qboolean restart )
{
//[b607] chrissstrahl - this lets us detect if the map was restarted or loadad
	game.levelRestarted = (bool)restart;

//hzm coop mod chrissstrahl - save client data
	if ( game.coop_isActive ){
		coop_serverSaveAllClientData();
	}

//hzm coop mod chrissstrahl - set coop variable for the gamecode!
//hzm coop mod chrissstrahl - restore cvar for regular multiplayer use
	game.coop_awardsActive = false;
	game.coop_isActive = false;
//hzm coop mod eof edit

	_inMultiplayerGame = false;

	_gameStarted = false;
	_gameOver    = false;

	if ( !restart )
	{
		_needToAddBots = true;
	}

	// Clean up the game

	delete _multiplayerGame;
	_multiplayerGame = NULL;

	// Clean up the player data

	delete [] _playerData;
	_playerData = NULL;

	// Clean up the award system

	delete _awardSystem;
	_awardSystem = NULL;

	// Clean up all the modifiers
	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		delete modifier;
	}

	_modifiers.FreeObjectList();

	_voteTime = 0.0f;
}

void MultiplayerManager::init( void )
{
	_inMultiplayerGame = false;

	resetRespawnTime();

	checkModifiedCvars( false );
}

void MultiplayerManager::start( void )
{
	if ( !_inMultiplayerGame )
		return;
	
	_gameStarted = true;
	_gameOver    = false;

	_talkingIconIndex = gi.imageindex( "sysimg/icons/mp/talking" );
	_waitingToRespawnIconIndex = gi.imageindex( "sysimg/icons/mp/elimination_eliminated" );

	//disabling the code block below reduces the number of errors
	// Start the modifiers
	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->start();
	}

	// Start the game

	_multiplayerGame->start();
}

void MultiplayerManager::update( float frameTime )
{
	int i;


	if ( !_inMultiplayerGame )
		return;

	// Restart the match if time

	if ( !_inMatch && !_gameOver && ( _restartMatchTime > 0.0f ) && ( _restartMatchTime < getTime() ) )
	{
		restartMatch();
	}

	// Update the respawn time

	if ( mp_respawnTime->modified )
	{
		if ( mp_respawnTime->value >= 0.0f )
		{
			setRespawnTime( mp_respawnTime->value );
		}

		mp_respawnTime->modified = false;
	}

//hzm coop mod chrissstrahl - update the skill cvar in multiplayer on the fly
	if (skill->modified)
	{
		int skill_level;
		str skillCurrent;

		skill_level = skill->integer;
		skill_level = bound(skill_level, 0, 3);
		skillCurrent = skill_level;

		gi.cvar_set("skill", skillCurrent.c_str() );
		gameVars.SetVariable("skill", skill_level);

		//hzm coop mod chrissstrahl - save the new skill into the ini
		coop_serverSaveGameVars( "skill" , skill_level );

		//hzm coop mod chrissstrahl - send skill info to all players
		int i;
		Player *player = NULL;
		for ( i = 0; i < maxclients->integer; i++ )
		{
			player = ( Player* )g_entities[i].entity;
			if ( player && player->client && player->isSubclassOf( Player ) ){
				if ( player->coopPlayer.installed ) {
					DelayedServerCommand( player->entnum , va( "globalwidgetcommand coopGpoSkill title %s" , coop_returnStringSkillname( skill->integer ).c_str() ) );
					DelayedServerCommand( player->entnum , va( "globalwidgetcommand coop_objectivesSkillValue title %s" , coop_returnStringSkillname( skill->integer ).c_str() ) );
				}
			}
		}
		skill->modified = false;
	}

	//[b611] chrissstrahl - update coop challanges
	coopChallenges.update(frameTime);

	// Start the game if not started already

	if ( !_gameStarted )
		start();

	if ( _needToAddBots )
		addBots();

	// Update all of the modifiers
	if ( _multiplayerGame->inMatch() )
	{
		for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
		{
			MultiplayerModifier *modifier;

			modifier = _modifiers.ObjectAt( i );

			if ( modifier )
				modifier->update( frameTime );
		}
	}

	// Update the game

	_multiplayerGame->update( frameTime );

	// Check for voting conditions

	checkVote();

	// Check for modified cvars

	checkModifiedCvars( true );

	// Update cvars

	if ( strlen( password->string ) )
	{
		if ( !g_needpass->integer )
		{
			gi.cvar_set( "g_needpass", "1" );
		}
	}
	else
	{
		if ( g_needpass->integer )
		{
			gi.cvar_set( "g_needpass", "0" );
		}
	}

	// Update dialogs

	for ( i = 0 ; i < maxclients->integer ; i++ )
	{
		Player *player;

		// Get the player

		player = getPlayer( i );

		if ( !player )
			continue;

		sendNextPlayerSound( player );
	}

	// Check for end of intermission

	if ( level.intermissiontime )
	{
		G_CheckIntermissionExit();

		if ( !_declaredWinner && ( _declareWinnerTime < getTime() ) )
		{
			_multiplayerGame->declareWinner();

			_declaredWinner = true;
		}
		return;
	}

	// Check for end of match

	if ( _multiplayerGame->isEndOfMatch() )
	{
		_gameOver = true;

	//hzm coop mod chrissstrahl - only play this when coop is not active
		if ( !game.coop_isActive )
		{
			centerPrintAllClients( "$$MatchOver$$\n" , CENTERPRINT_IMPORTANCE_NORMAL );
			multiplayerManager.broadcastSound( "localization/sound/dialog/dm/comp_matover.mp3" , CHAN_AUTO , DEFAULT_VOL , DEFAULT_MIN_DIST , NULL , 1.2f );
		}

		_multiplayerGame->EndMatch();
		G_BeginIntermission2();
		matchOver();

		_declareWinnerTime = getTime() + 2.0f;
		_declaredWinner = false;

		setNextMap();
	}

	// Update the players

	for ( i = 0 ; i < maxclients->integer ; i++ )
	{
		Player *player;
		Player *playerFollowing;

		// Get the player

		player = getPlayer( i );

		if ( !player )
			continue;

		if ( isPlayerSpectator( player, SPECTATOR_TYPE_FOLLOW ) )
		{
			playerFollowing = getPlayer( _playerData[ player->entnum ]._spectatorPlayerNum );

			if ( !playerFollowing || playerFollowing->deadflag == DEAD_DEAD )
			{
				makePlayerSpectateNextPlayer( player );

				if ( !isPlayerSpectator( player, SPECTATOR_TYPE_FOLLOW ) )
					continue;

				playerFollowing = getPlayer( _playerData[ player->entnum ]._spectatorPlayerNum );
			}

			if ( playerFollowing )
			{
				player->setOrigin( playerFollowing->origin );
				player->setAngles( playerFollowing->angles );
				player->SetViewAngles( playerFollowing->getViewAngles() );
			}
		}

		//hzm coop mod chrissstrahl - if lms (last man standing) is active
		if ( game.coop_lastmanstanding && game.levelType >= MAPTYPE_MISSION )
		{
			if ( player->coopPlayer.deathTime < game.coop_levelStartTime && multiplayerManager.isPlayerSpectator( player ) && !multiplayerManager.isPlayerSpectatorByChoice( player ) )
			{
				if ( _gameStarted )
				{
					multiplayerManager.playerEnterArena( player->entnum , 999 );
				}
			}

			if ( player->health <= 0 )
			{
				time_t result = time( NULL );
				localtime( &result );
				
				if ( (player->coopPlayer.deathTime + 6 ) <= ( int )result )
				{
					makePlayerSpectator( player , SPECTATOR_TYPE_FOLLOW , false );
				}
			}
			return;
		}


		//if player has to wait for some time, make him spectator and follow, instead of staring at the scoreboard
		if ( player->health <= 0.0f &&  getRespawnTime() > 0.0f )
		{
			if ( !_playerData[player->entnum]._waitingForRespawn )
			{
				_playerData[player->entnum]._waitingForRespawn = true;
				_playerData[player->entnum]._respawnTime = getTime() + getRespawnTime();
			}

			if ( (_playerData[player->entnum]._respawnTime - getTime()) < ( getRespawnTime() - 5.0f ) && (_playerData[player->entnum]._respawnTime - getTime()) > 5.0f )
			{
				makePlayerSpectator( player , SPECTATOR_TYPE_FOLLOW , false );
			}
		}
		//end of hzm


		//hzm gameupdate chrissstrahl - make sure player gets automatically enlisted to respawn, also prevents cheating by going into spec before trying to respawn
		if (  _playerData[ player->entnum ]._waitingForRespawn )
		{
			int lastTimeLeft;
			int timeLeft;
			str printString;

			timeLeft = (int)( _playerData[ player->entnum ]._respawnTime - getTime() + 1.0f + ( frameTime / 2.0f ) );
			lastTimeLeft = (int)( _playerData[ player->entnum ]._respawnTime - getTime() + frameTime + 1.0f + ( frameTime / 2.0f ) );

			//hzm gameupdate chrissstrahl - do not spam the chat with that
			if ( ( timeLeft % 5 ) == 0 && ( lastTimeLeft != timeLeft ) || ( timeLeft < 5 ) && ( lastTimeLeft != timeLeft ) && ( timeLeft > 0 ) )
			{
				printString = "$$RespawnIn$$ ";
				printString += timeLeft;

				centerPrint( player->entnum, printString, CENTERPRINT_IMPORTANCE_HIGH );
			}

			if ( _playerData[ player->entnum ]._respawnTime <= getTime() )
			{
				respawnPlayer( player, true );
			}
		}
	}
}

void MultiplayerManager::addBots( void )
{
	cvar_t *mp_botcommands;

	// If this is not a dedicated server make sure the player enters the game first
	if ( !dedicated->integer && ( !g_entities[ 0 ].inuse || !g_entities[ 0 ].client || !g_entities[ 0 ].entity ) )
		return;

	// Add the bot commands
	mp_botcommands = gi.cvar( "mp_botcommands", "", 0 );

	if ( strlen( mp_botcommands->string ) > 0 )
	{
		gi.SendConsoleCommand( mp_botcommands->string );
		//gi.cvar_set( "mp_botcommands", "" );
	}

	_needToAddBots = false;
}

extern int gametype; // for BOTLIB
void MultiplayerManager::initMultiplayerGame( void )
{
	cvar_t *mp_modifier_Destruction;
	int i;

	// Automatically turn off diffusion, it will be turned back on if needed

	gi.cvar_set( "mp_modifier_diffusion", "0" );

	// Create the correct game type

	switch ( mp_gametype->integer ) // Todo : switch off of a something better than a hardcoded index
	{
		case 0 : 
			_multiplayerGame = new ModeDeathmatch(); 
			gametype = GT_FFA;
			break;
		case 1 :  
			_multiplayerGame = new ModeTeamDeathmatch(); 
			gametype = GT_TEAM;
			break;
		case 2 :  
			_multiplayerGame = new ModeCaptureTheFlag(); 
			gametype = GT_CTF;
			break ;
		case 3 :  
			// This is bomb diffusion mode

			_multiplayerGame = new ModeTeamDeathmatch(); 

			gi.cvar_set( "mp_modifier_diffusion", "1" );
			/* gi.cvar_set( "mp_modifier_specialties", "1" );
			gi.cvar_set( "mp_modifier_elimination", "1" ); */

			//gi.cvar_set( "mp_gametype", "1" );

			gametype = GT_TEAM;
			break ;

		default: 
			_multiplayerGame = new ModeDeathmatch(); 
			gametype = GT_FFA;
			break ;
	}

	// Setup some stuff for bots

	mp_modifier_Destruction = gi.cvar( "mp_modifier_Destruction", "0", 0 );

	if ( mp_modifier_Destruction->integer )
	{
		gametype = GT_OBELISK;
	}

	if ( !_multiplayerGame )
		return;

	// Create the player data

	_playerData = new MultiplayerPlayerData[ maxclients->integer ];

	// Initialize the game

	_multiplayerGame->init( maxclients->integer );

	_multiplayerGame->setPointLimit( mp_pointlimit->integer );
	_multiplayerGame->setTimeLimit( mp_timelimit->integer * 60.0f );	
	

	_inMultiplayerGame = true;

	// Add all of the needed modifiers
	addModifiers();

	// Initialize all of the needed modifiers
	for ( i = 1; i <= _modifiers.NumObjects(); i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->init( maxclients->integer );
	}

	// Initialize the award system
	_awardSystem = new AwardSystem;
	_awardSystem->init( maxclients->integer );

	// Initialize anything needed that is outside of the multiplayer system
	// Make sure no tricorder modes are available except those explicitly set by the script
	Event *event = new Event( "addAvailableViewMode" );
	event->AddString( "BogusMode" );
	world->ProcessEvent( event );
}

void MultiplayerManager::initItems( void )
{
	int i;
	int j;
	bool shouldKeep;
	MultiplayerItem *item;
	Item *normalItem;

	// Tell the game to initialize items //hzm chrissstrahl - and set/reset time, really someone should have mentioned that!
	_multiplayerGame->initItems();

	//hzm coop mod chrissstrahl - keep singleplayer setup, do not applay multiplayer settings
	if ( !_inMultiplayerGame || game.coop_isActive )
		return;

	// Tell all of the modifiers to initialize items
	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;
		modifier = _modifiers.ObjectAt( i );
		if ( modifier )
			modifier->initItems();
	}

	// Tell the award system to initialize items

	_awardSystem->initItems();

	// Find all of the multiplayer items, tell everyone about each of them and see if we need to keep them

	// TODO: move this to a seperate proc

	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		if ( g_entities[ i ].inuse && g_entities[ i ].entity )
		{
			if ( g_entities[ i ].entity->isSubclassOf( MultiplayerItem ) )
			{
				item = (MultiplayerItem *)g_entities[ i ].entity;

				shouldKeep = false;

				// Tell modifiers about the item and see if they want to keep it

				for ( j = 1 ; j <= _modifiers.NumObjects() ; j++ )
				{
					MultiplayerModifier *modifier;

					modifier = _modifiers.ObjectAt( j );

					if ( modifier )
					{
						if ( modifier->shouldKeepItem( item ) )
							shouldKeep = true;
					}
				}

				// Tell the mode about the item and see if it wants to keep it

				if ( _multiplayerGame->shouldKeepItem( item ) )
					shouldKeep = true;

				// If no one said to keep the item get rid of it

				if ( shouldKeep )
				{
					// Tell all of the modifiers that the item was kept

					for ( j = 1 ; j <= _modifiers.NumObjects() ; j++ )
					{
						MultiplayerModifier *modifier;

						modifier = _modifiers.ObjectAt( j );

						if ( modifier )
						{
							modifier->itemKept( item );
						}
					}

					// Tell the mode that the item was kept

					_multiplayerGame->itemKept( item );
				}
				else
				{
					// No one told us to keep this multiplayer item, so get rid of it

					item->PostEvent( EV_Remove, FRAMETIME );
				}
			}	
			else if ( g_entities[ i ].entity->isSubclassOf( Item ) )
			{
				normalItem = (Item *)g_entities[ i ].entity;

				shouldKeep = true;

				// Tell modifiers about the item and see if they want to keep it

				for ( j = 1 ; j <= _modifiers.NumObjects() ; j++ )
				{
					MultiplayerModifier *modifier;

					modifier = _modifiers.ObjectAt( j );

					if ( modifier )
					{
						if ( !modifier->shouldKeepNormalItem( normalItem ) )
							shouldKeep = false;
					}
				}

				// Tell the mode about the item and see if it wants to keep it

				if ( !_multiplayerGame->shouldKeepNormalItem( normalItem ) )
					shouldKeep = false;

				// If no one said to keep the item get rid of it

				if ( !shouldKeep )
				{
					// Someone told us to get rid of this item

					normalItem->PostEvent( EV_Remove, FRAMETIME );
				}
			}
		}
	}
}

void MultiplayerManager::resetItems( void )
{
	int i;
	Item *item;

	// Setup all of the items again

	for ( i = 0; i < MAX_GENTITIES; i++ )
	{
		if ( g_entities[ i ].inuse && g_entities[ i ].entity && g_entities[ i ].entity->isSubclassOf( Item ) )
		{
			item = (Item *)g_entities[ i ].entity;

			// Ignore runes and multiplayer items

			if ( item->isSubclassOf( Rune ) || item->isSubclassOf( MultiplayerItem ) )
				continue;

			if ( item->Respawnable() )
			{
				// Item is a normal respawnable item, so respawn it again

				item->CancelEventsOfType( EV_Item_Respawn );
				item->PostEvent( EV_Item_Respawn, 0.0f );
			}
			else
			{
				// Item is a temporary item, so get rid of it

				item->PostEvent( EV_Remove, 0.0f );
			}
		}
	}
}

//hzm gamefix chrissstrahl - used to show new players the current vote
float MultiplayerManager::voteTime( void )
{
	return _voteTime;
}

//hzm gamefix chrissstrahl - used to show new players the current vote
str MultiplayerManager::getVoteString( void )
{
	return _voteString;
}

//hzm gamefix chrissstrahl - used to show new players the current vote
void MultiplayerManager::updateVotes( Player * player, bool bLeaving )
{
	//player entering game
	if ( !bLeaving ){
		if ( multiplayerManager.voteTime() > 0 ){
			//set current number of voters
			_numVoters = coop_returnPlayerQuantity(3); //[b607] chrissstrahl - excluding bots

			//hzm coop mod chrissstrahl - translate the vote strings forbetter understanding
			str sVoteText = "";
			bool bALt = false;

			//hzm coop mod chrissstrahl - handle coop specific votes in a seperate function
			bALt = coop_textModifyVoteText( multiplayerManager.getVoteString() , sVoteText , player );

			//hzm coop mod chrissstrahl - print it in german if player is using german game version
			if ( coop_checkPlayerLanguageGerman( player ) ){
				if ( coop_returnIntFind( multiplayerManager.getVoteString() , "skipcinematic" ) > -1 ){
					bALt = true; sVoteText = "Filmsequenz abbrechen?"; sVoteText += coop_returnStringStartingFrom( multiplayerManager.getVoteString() , 13 );
				}
			}
			else {
				if ( coop_returnIntFind( multiplayerManager.getVoteString() , "skipcinematic" ) > -1 ){
					bALt = true; sVoteText = "Skip Cinematic ?"; sVoteText += coop_returnStringStartingFrom( multiplayerManager.getVoteString() , 13 );
				}
			}

			//hzm coop mod chrissstrahl - do not set/send vote for player who started the vote
			if ( !player->coopPlayer.startedVote ) {

				//hzm decide which string to print
				if ( !bALt )
					sVoteText = multiplayerManager.getVoteString();
				//[b607] chrissstrahl - yes and no are now in a seperate line - and non coop clients get the default text
				if (player->coopPlayer.installed) {
					player->setVoteText( va( "$$NewVote$$: %s\n(F1 = $$Yes$$, F2 = $$No$$)" , sVoteText.c_str() ) );
				}
				else {
					player->setVoteText(va("$$NewVote$$: %s", sVoteText.c_str()));
				}
			}
			//hzm eof
		}
		return;
	}

	//player leaving game
	//if only 2 players on the server and the player who started the vote leaves, end the vote
	if ( player->coopPlayer.startedVote && coop_returnPlayerQuantity(3) < 3 ){ //[b607] chrissstrahl - excluding bots
		_voteYes = 0;
		_voteTime = 0.0f;

		int i;
		for ( i = 0; i < maxclients->integer; i++ ) {
			Player *currentPlayer;
			currentPlayer = getPlayer( i );
			if ( currentPlayer ) {
				currentPlayer->clearVoteText();
				player->coopPlayer.startedVote = false;
				_playerData[player->entnum]._voted = false;
			}
		}
		return;
	}

	if ( player->coopPlayer.startedVote || _playerData[player->entnum]._voted ){
		_voteYes--;
	}
	_numVoters = ( coop_returnPlayerQuantity(3) - 1 );  //[b607] chrissstrahl - 3 excluding bots and substracting leaving player
}

bool MultiplayerManager::inMultiplayer( void )
{
	return _inMultiplayerGame;
}

bool MultiplayerManager::checkFlag( unsigned int flag )
{
	//hzm coop mod chrissstrahl - beware possible error, this action might have unknown sideeffects
	//hzm coop mod chrissstrahl - do not apply mp_flags during coop
	if ( !_inMultiplayerGame || game.coop_isActive )
		return false;
	

	if ( mp_flags->integer & flag )
		return true;
	else
		return false;
}

bool MultiplayerManager::fullCollision( void )
{
	if ( !_inMultiplayerGame )
		return true;

	if ( checkFlag( MP_FLAG_FULL_COLLISION ) )
		return true;
	else
		return false;
}

bool MultiplayerManager::isFightingAllowed( void )
{
	if ( !_inMultiplayerGame )
		return true;

	if ( !_inMatch )
		return false;

	return _allowFighting;
}

//[b607] chrissstrahl - add dynamic light support on multiplayer dedicated servers
void MultiplayerManager::handleDynamicLights(Player *player)
{
	game.bForceDynLightUpdate = true;
}

void MultiplayerManager::addPlayer( Player *player )
{
	if ( !_inMultiplayerGame )
		return;

	assert( player );

	if ( !player )
		return;

	// See if the player has already been added

	if ( _playerData[ player->entnum ]._valid )
		return;

	// Initialize the player data

	_playerData[ player->entnum ].reset();
	_playerData[ player->entnum ]._valid = true;

	//hzm gamefix daggolin - Always assume the player is named...
	_playerData[ player->entnum ]._named = true;

	// Inform the game about the new player

	_multiplayerGame->AddPlayer( player );

	// Inform all of the modifiers about the new player
	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->addPlayer( player );
	}

	// Inform the award system about the new player

	_awardSystem->addPlayer ( player );

	// Inform all of the players that the player has joined the game
	//[b607] chrissstrahl - restored to default - last minute fix
	HUDPrintAllClients( va( "%s $$JoinedGame$$\n" , player->client->pers.netname ) );

	//[b607] chrissstrahl - fix dynamic lights not updating/being activated for players on a dedicated server
	handleDynamicLights(player);
}

void MultiplayerManager::removePlayer( Player *player )
{
	if ( !_inMultiplayerGame )
		return;

	assert( player );

	if ( !player )
		return;

	if ( !_playerData[ player->entnum ]._valid )
		return;

	// Inform the game about the player being removed

	_multiplayerGame->RemovePlayer( player );

	// Reset the player's data

	_playerData[ player->entnum ].reset();

	// Inform all of the modifiers about the player being removed
	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->removePlayer( player );
	}

	// Inform the award system about the player being removed

	_awardSystem->removePlayer ( player );

	// Inform all of the players that the player has left the game

	multiplayerManager.HUDPrintAllClients( va( "%s $$LeftGame$$\n", player->client->pers.netname ) );
}

void MultiplayerManager::changePlayerModel( Player *player, const char *modelName, bool force )
{
	str modelToUse;

//hzm gamefix daggolin - don't change the model if the player is dead
//hzm coop mod chrissstrahl - moved your check up here, no need for extra if statment :D, this is a old scripting habbit
	if ( !_inMultiplayerGame || !player  || player->health <= 0.0f )
		return;

	if ( ( player->model != modelName ) || ( force ) )
	{
		modelToUse = modelName;

		// Verify that this is an acceptable model to use

		if ( !isValidPlayerModel( player, modelToUse ) )
		{
//hzm gamefix chrissstrahl - do not show that the spectator model (no model) is not a acceptable model, THIS is NOT helping!
			if ( modelToUse.length() > 3 && level.cinematic == false )
			{
				//hzm gameupdate chrissstrahl - if the mission has failed, don't show this anymore, keep text minimal to focus players on the failure text
				if ( !level.mission_failed )
				{
					if ( !game.coop_isActive || ( player->coopPlayer.lastTimeSpawned + 2 ) < level.time ) {
						if (player->getLanguage() == "Deu") {
							centerPrint( player->entnum , va( "^3 %s ^8ist keine akzeptable Spielfigur..." , modelName ) , CENTERPRINT_IMPORTANCE_NORMAL );
						}
						else {
							centerPrint( player->entnum , va( "^3 %s ^8is not an acceptable Player-Skin to use..." , modelName ) , CENTERPRINT_IMPORTANCE_NORMAL );
						}
					}
				}
			}
			modelToUse = getDefaultPlayerModel( player );
		}

		if ( checkFlag( MP_FLAG_FORCE_DEFAULT_MODEL ) )
		{
			modelToUse = getDefaultPlayerModel( player );
		}

		// Setup the default backup model

		player->setBackupModel( getDefaultPlayerModel( player ) );

		// Setup the model

		player->InitModel( modelToUse );

		player->CancelEventsOfType( EV_ProcessInitCommands );
		player->ProcessInitCommands( player->edict->s.modelindex );

		resetPlayerStateMachine( player );

		if ( multiplayerManager.isPlayerSpectator( player ) )
		{
			player->hideModel();
			player->setSolidType( SOLID_NOT );
		}

		for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
		{
			MultiplayerModifier *modifier;

			modifier = _modifiers.ObjectAt( i );

			if ( modifier )
				modifier->playerChangedModel( player );
		}

		_multiplayerGame->playerChangedModel( player );
		
		player->modelChanged();

		//hzm coop mod chrissstrahl - handle coop mod related model stuff
		coop_playerModelChanged(player);
	}
}

void MultiplayerManager::resetPlayerStateMachine( Player *player )
{
	if ( !_inMultiplayerGame )
		return;

	if ( player )
	{
		player->SetAnim( "stand_idle", legs, true );
		player->SetAnim( "stand_idle", torso, true );
		player->LoadStateTable();
	}

}

void MultiplayerManager::changePlayerName( Player *player, const str &playerName )
{
	if ( !_inMultiplayerGame )
		return;

	if ( player ){
		if ( _playerData[ player->entnum ]._named && ( _playerData[ player->entnum ]._name != playerName ) ){
			// Inform all of the players that the player has changed his name
			multiplayerManager.HUDPrintAllClients( va( "%s $$ChangedName$$ %s\n", _playerData[ player->entnum ]._name.c_str(), playerName.c_str() ) );

			//[b607] chrissstrahl - update communicator ui transporter playername info
			coop_playerCommunicator(player,1);
		}
		_playerData[ player->entnum ]._named = true;
		_playerData[ player->entnum ]._name = playerName;
	}
}

void MultiplayerManager::playerKilled( Player *killedPlayer, Player *attackingPlayer, Entity *inflictor, int meansOfDeath )
{
	if ( !_inMultiplayerGame )
		return;

	// Inform all of the modifiers about the player being killed

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->playerKilled( killedPlayer, attackingPlayer, inflictor, meansOfDeath );
	}

	// Inform the game about the player being killed

	_multiplayerGame->playerKilled( killedPlayer, attackingPlayer, inflictor, meansOfDeath );

	// Inform the award system about the player being killed

	_awardSystem->playerKilled( killedPlayer, attackingPlayer, inflictor, meansOfDeath );

	// Tell the players what happened

//hzm coop mod chrissstrahl - if not in coop mod, display what eVa you like
//hzm coop mod chrissstrahl - we manage death message in mp_modeBase.cpp/coopPlayer.cpp
	if ( game.coop_isActive )
		return;

	if ( killedPlayer == attackingPlayer || !attackingPlayer )
	{
		centerPrint( killedPlayer->entnum, va( "^%c$$YouKilledYourself$$^%c", COLOR_RED, COLOR_NONE ), CENTERPRINT_IMPORTANCE_NORMAL );
	}
	else
	{
		str printString;

		centerPrint( killedPlayer->entnum, va( "^%c$$KilledByPlayer$$^%c %s", COLOR_RED, COLOR_NONE, attackingPlayer->client->pers.netname, COLOR_WHITE + 1 ), CENTERPRINT_IMPORTANCE_NORMAL );

		printString = va( "^%c$$KilledPlayer$$^%c %s", COLOR_GREEN, COLOR_NONE, killedPlayer->client->pers.netname );

		if ( checkRule( "usingIndividualScore", false, attackingPlayer ) )
		{
			str placeName = getPlaceName( attackingPlayer );
			int points = getPoints( attackingPlayer );

			if ( points == 1 )
				printString += va( "\n$$PlaceString1$$ ^%c %s^%c $$PlaceString2$$ ^%c %d^%c $$Point$$", COLOR_CYAN, placeName.c_str(), COLOR_NONE, COLOR_CYAN, points, COLOR_NONE );
			else
				printString += va( "\n$$PlaceString1$$ ^%c %s^%c $$PlaceString2$$ ^%c %d^%c $$Points$$", COLOR_CYAN, placeName.c_str(), COLOR_NONE, COLOR_CYAN, points, COLOR_NONE );
		}

		centerPrint( attackingPlayer->entnum, printString, CENTERPRINT_IMPORTANCE_NORMAL );

		multiplayerManager.instantPlayerSound( attackingPlayer->entnum, "snd_mp_killedsomeone", CHAN_COMBAT4 );
	}
}

void MultiplayerManager::matchOver( void )
{
	if ( !_inMultiplayerGame )
		return;

	int i;

	// Inform all of the modifiers about the match being over
	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->matchOver();
	}

	// Inform the game about the match being over

	_multiplayerGame->matchOver();

	// Inform the award system about the match being over

	_awardSystem->matchOver();

	_inMatch = false;

	// Do some solo match work (EF2 specific)

	cvar_t *mp_solomatch;
	cvar_t *mp_mapAward;
	cvar_t *mp_botskilllevel;
	str cvarName;

	// Get all the cvars we need
	cvarName = level.mapname + "_award";

	mp_solomatch     = gi.cvar( "mp_solomatch",     "0", 0 );
	mp_mapAward      = gi.cvar( cvarName.c_str(),   "0", CVAR_ARCHIVE );
	mp_botskilllevel = gi.cvar( "mp_botskilllevel", "0", CVAR_ARCHIVE );

	// Make sure we are in solomatch

	if ( mp_solomatch->integer )
	{
		int playersScore;
		int botsScore;
		bool wonMatch = true;
		Player *player;

		// See if the player won the match

		player = getPlayer( 0 );
		playersScore = getPoints( player );

		for ( i = 1 ; i < maxclients->integer ; i++ )
		{
			player = getPlayer( i );

			if ( player )
			{
				botsScore = getPoints( player );

				if ( botsScore >= playersScore )
				{
					wonMatch = false;
					break;
				}
			}
		}

		if ( wonMatch )
		{
			// See if this is the highest bot skill level the player has won on

			if ( mp_mapAward->integer < mp_botskilllevel->integer )
			{
				// Save cvar

				gi.cvar_set( cvarName.c_str(), mp_botskilllevel->string );
			}
		}
	}
}

int MultiplayerManager::getScoreIcon( Player *player, int index )
{
	int icon = 0;

	if ( _inMatch )
	{
		for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
		{
			MultiplayerModifier *modifier;

			modifier = _modifiers.ObjectAt( i );

			if ( modifier )
			{
				icon = modifier->getScoreIcon( player, index, icon );
			}
		}

		// Allow the game to apply speed modifiers

		icon = _multiplayerGame->getScoreIcon( player, index, icon );
	}
	else
	{
		icon =  _awardSystem->getAfterMatchAward( player, index );
	}

	if ( ( index == SCOREICON3 ) && ( icon == 0 ) && _playerData[ player->entnum ]._waitingForRespawn )
	{
		icon = _waitingToRespawnIconIndex;
	}

	return icon;
}

int MultiplayerManager::getAfterMatchAward( Player *player, int index )
{
	return _awardSystem->getAfterMatchAward( player, index );
}

void MultiplayerManager::playerDead( Player *player )
{
	assert(player);
	
	if ( !player )
		return;

	// Inform the game that the player is dead

	_multiplayerGame->playerDead( player );
}

void MultiplayerManager::applySpeedModifiers( Player *player,  int *moveSpeed )
{
	//hzm coop mod chrissstrahl - let us handle things like in singleplayer, it feels more realistic
	if ( !_inMultiplayerGame || game.coop_isActive)
		return;

	// Allow all of the modifiers to apply speed modifiers

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->applySpeedModifiers( player, moveSpeed );
	}

	// Allow the game to apply speed modifiers

	_multiplayerGame->applySpeedModifiers( player, moveSpeed );
}

void MultiplayerManager::applyJumpModifiers( Player *player,  int *jumpSpeed )
{
//hzm coop mod chrissstrahl - use singleplayer jumpspeed
	if ( !_inMultiplayerGame || game.coop_isActive )
		return;

	// Allow all of the modifiers to apply jump modifiers

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->applyJumpModifiers( player, jumpSpeed );
	}

	// Allow the game to apply jump modifiers

	_multiplayerGame->applyJumpModifiers( player, jumpSpeed );
}

void MultiplayerManager::applyAirAccelerationModifiers( Player *player,  int *airAcceleration )
{
//hzm coop mod chrissstrahl - have the singleplayer airacceleration for coop, do not use multiplayer settings
	if (!_inMultiplayerGame || game.coop_isActive )
		return;

	// Allow all of the modifiers to apply air acceleration modifiers

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->applyAirAccelerationModifiers( player, airAcceleration );
	}

	// Allow the game to apply air acceleration modifiers

	_multiplayerGame->applyAirAccelerationModifiers( player, airAcceleration );
}

bool MultiplayerManager::canPickup( Player *player, MultiplayerItemType itemType, const char *item_name )
{
//hzm coop mod chrissstrahl - make it singleplayer alike for coop, do not use multiplayer settings
	if (!_inMultiplayerGame || game.coop_isActive )
		return true;

	// See if all of the modifiers will allow the player to pickup this item

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			if ( !modifier->canPickup( player, itemType, item_name ) )
				return false;
		}
	}

	// See if the game will allow the player to pickup this item

	if ( !_multiplayerGame->canPickup( player, itemType, item_name ) )
		return false;

	// The player is allowed to pickup this item

	return true;
}

void MultiplayerManager::pickedupItem( Player *player, MultiplayerItemType itemType, const char *itemName )
{
//hzm coop mod chrissstrahl - make it singleplayer alike for coop, do not use multiplayer settings
	if ( !_inMultiplayerGame || game.coop_isActive )
		return;

	// Tell modifiers that an item was picked up

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			modifier->pickedupItem( player, itemType, itemName );
		}
	}

	// Tell game that an item was picked up

	_multiplayerGame->pickedupItem( player, itemType, itemName );

	// Tell the award system  that an item was picked up

	_awardSystem->pickedupItem( player, itemType, itemName );
}

int MultiplayerManager::getPointsForKill( Player *killedPlayer, Player *attackingPlayer, Entity *inflictor, int meansOfDeath, int points )
{
	int realPoints;

	realPoints = points;

	// Allow all of the modifiers to change the points given for this kill

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			realPoints = modifier->getPointsForKill( killedPlayer, attackingPlayer, inflictor, meansOfDeath, realPoints );
	}

	return realPoints;
}

void MultiplayerManager::respawnPlayer( Player *player, bool forced )
{
	if ( !_inMultiplayerGame )
		return;

	assert(player);
	
	if ( !player )
		return;

	if ( multiplayerManager.isPlayerSpectator( player ) && multiplayerManager.isPlayerSpectatorByChoice( player ) )
		return;

	//hzm coop mod chrissstrahl - coop mod prevent player from respawning if LMS is active
	if ( !coop_playerSpawnLms( player ) )
	{
		return;
	}
	//end of hzm

	if ( getRespawnTime() > 0.0f && !forced )
	{
		if ( !_playerData[ player->entnum ]._waitingForRespawn )
		{
			_playerData[ player->entnum ]._waitingForRespawn = true;
			_playerData[ player->entnum ]._respawnTime = getTime() + getRespawnTime();
		}
	}
	else
	{
		_playerData[ player->entnum ]._waitingForRespawn = false;
		_multiplayerGame->respawnPlayer( player );
	}
}

void MultiplayerManager::respawnAllPlayers( void )
{
	int i;
	Player *player;

	if ( !_inMultiplayerGame )
		return;

	for( i = 0 ; i < maxclients->integer ; i++ )
	{
		player = getPlayer( i );

		if ( !player )
			continue;

		if ( multiplayerManager.isPlayerSpectatorByChoice( player ) && !( player->edict->svflags & SVF_BOT ) )
			continue;

		respawnPlayer( player, true );
	}
}

Entity *MultiplayerManager::getSpawnPoint( Player *player )
{

	if ( !_inMultiplayerGame)
		return NULL;

	assert( player );
	
	if ( !player )
		return NULL;
	
	// Get a spawn point for this player from the game
	return _multiplayerGame->getSpawnPoint( player );
}

float MultiplayerManager::playerDamaged( Player *damagedPlayer, Player *attackingPlayer, float damage, int meansOfDeath )
{
	float realDamage;

	realDamage = damage;

	if ( !_inMultiplayerGame )
		return realDamage;

	//hzm coop mod chrissstrahl - handle friendly fire
	if ( game.coop_isActive )
	{
		if ( damagedPlayer != attackingPlayer ) {
			if ( game.coop_friendlyFire <= 0 ) {
				return 0;
			}

			realDamage *= game.coop_friendlyFire;
		}
		//make sure we don't get hurt to badly from our own spashdamage
		else{
			realDamage = (realDamage / 2);
		}
	}
	//end of hzm
	else {
		// Change damage based on cvar
		realDamage *= mp_damageMultiplier->value;

		// Change damage based on modifiers

		for ( int i = 1; i <= _modifiers.NumObjects(); i++ )
		{
			MultiplayerModifier *modifier;

			modifier = _modifiers.ObjectAt( i );

			if ( modifier )
				realDamage = modifier->playerDamaged( damagedPlayer , attackingPlayer , realDamage , meansOfDeath );
		}

		// Change damage based on the mode

		realDamage = _multiplayerGame->playerDamaged( damagedPlayer , attackingPlayer , realDamage , meansOfDeath );

		// Inform the award system that someone was damaged
	}

	_awardSystem->playerDamaged( damagedPlayer, attackingPlayer, realDamage, meansOfDeath );

	return realDamage;
}

void MultiplayerManager::playerTookDamage( Player *damagedPlayer, Player *attackingPlayer, float damage, int meansOfDeath )
{
	_multiplayerGame->playerTookDamage( damagedPlayer, attackingPlayer, damage, meansOfDeath );
}

void MultiplayerManager::playerFired( Player *attackingPlayer )
{
	if ( !_inMultiplayerGame )
		return;

	// Inform the modifiers that the player shot

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->playerFired( attackingPlayer );
	}

	// Inform the game that the player shot

	_multiplayerGame->playerFired( attackingPlayer );

	// Inform the award system that the player shot

	_awardSystem->playerFired( attackingPlayer );
}

float MultiplayerManager::getModifiedKnockback( Player *damagedPlayer, Player *attackingPlayer, float knockback )
{
//hzm coop mod chrissstrahl - make it singleplayer alike for coop, do not use multiplayer settings
	if ( !_inMultiplayerGame || game.coop_isActive )
		return knockback;

	// Change damage based on cvar

	return knockback * mp_knockbackMultiplier->value;
}

void MultiplayerManager::itemTouched( Player *player, MultiplayerItem *item )
{
	if ( !_inMultiplayerGame )
		return;

	// Make sure the player is alive and playing

	if ( isPlayerSpectator( player ) || ( player->deadflag != DEAD_NO ) || ( player->getHealth() <= 0.0f ) )
		return;

	// Tell all of the modifiers that this item was touched

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->itemTouched( player, item );
	}

	// Tell the mode that this item was touched

	_multiplayerGame->itemTouched( player, item );
}

float MultiplayerManager::itemDamaged( MultiplayerItem *item, Player *attackingPlayer, float damage, int meansOfDeath )
{
	float realDamage;

	if ( !_inMultiplayerGame )
		return damage;

	realDamage = damage;

	// Tell all of the modifiers that this item was damaged

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			realDamage = modifier->itemDamaged( item, attackingPlayer, realDamage, meansOfDeath );
		}
	}

	// Tell the mode that this item was destroyed

	return _multiplayerGame->itemDamaged( item, attackingPlayer, realDamage, meansOfDeath );
}

void MultiplayerManager::itemDestroyed( Player *player, MultiplayerItem *item )
{
	if ( !_inMultiplayerGame )
		return;

	// Tell all of the modifiers that this item was destroyed

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->itemDestroyed( player, item );
	}

	// Tell the mode that this item was destroyed

	_multiplayerGame->itemDestroyed( player, item );
}

void MultiplayerManager::itemUsed( Entity *entity, MultiplayerItem *item )
{
	if ( !_inMultiplayerGame )
		return;

	// Tell all of the modifiers that this item was destroyed

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->itemUsed( entity, item );
	}

	// Tell the mode that this item was destroyed

	_multiplayerGame->itemUsed( entity, item );
}

void MultiplayerManager::playerUsed( Player *usedPlayer, Player *usingPlayer, Equipment *equipment )
{
	if ( !_inMultiplayerGame )
		return;

//hzm coop mod chrissstrahl - handle here our useing player stuff
	if ( game.coop_isActive && usedPlayer != usingPlayer )
	{
		coop_playerUsed(usedPlayer , usingPlayer ,equipment );
		return;
	}

	// Tell all of the modifiers that this item was destroyed
	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->playerUsed( usedPlayer, usingPlayer, equipment );
	}

	// Tell the mode that this item was destroyed

	_multiplayerGame->playerUsed( usedPlayer, usingPlayer, equipment );
}

bool MultiplayerManager::checkRule( const char *rule, bool defaultValue, Player *player )
{
	bool value;

	//hzm coop mod chrissstrahl - we might want to intercept here, but we should first be certain that there will be no negative impact in the game
	if ( !_inMultiplayerGame )
		return defaultValue;

	// Default the value to the value passed in

	value = defaultValue;

	// Check this rule with all of the modifiers

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			value = modifier->checkRule( rule, value, player );
		}
	}

	// Check this rule with the mode

	value = _multiplayerGame->checkRule( rule, value, player );

	return value;
}

bool MultiplayerManager::checkGameType( const char *gameType )
{
	if ( !_inMultiplayerGame )
		return false;

	// Check for the gametype with the game first

	if ( _multiplayerGame->checkGameType( gameType ) )
		return true;

	// Check for the gametype with  all of the modifiers
	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			if ( modifier->checkGameType( gameType ) )
				return true;
		}
	}

	return false;
}

bool MultiplayerManager::doesPlayerHaveItem( Player *player, const char *itemName )
{
	if ( !_inMultiplayerGame )
		return false;

	if ( !player )
		return false;

	// Check to see if the player has the item with the game first

	if ( _multiplayerGame->doesPlayerHaveItem( player, itemName ) )
		return true;

	// Check to see if the player has the item with all of the modifiers

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			if ( modifier->doesPlayerHaveItem( player, itemName ) )
				return true;
		}
	}

	return false;
}

void MultiplayerManager::score( Player *player )
{
	assert(player);
	
	if ( !player )
		return;

	if ( !_inMultiplayerGame )
		return;

	// Tell the game to send the current score to the player

	_multiplayerGame->score( player );
}

int MultiplayerManager::getPoints( Player *player )
{
	assert(player);
	
	if ( !player )
		return 0;

	if ( !_inMultiplayerGame )
		return 0;

	// Get the player's current points from the game

	return _multiplayerGame->getPoints( player );
}

int MultiplayerManager::getKills( Player *player )
{
	assert(player);
	
	if ( !player )
		return 0;

	if ( !_inMultiplayerGame )
		return 0;

	// Get the player's current kills from the game

	return _multiplayerGame->getKills( player );
}

int MultiplayerManager::getDeaths( Player *player )
{
	assert(player);
	
	if ( !player )
		return 0;

	if ( !_inMultiplayerGame )
		return 0;

	// Get the player's current deaths from the game

	return _multiplayerGame->getDeaths( player );
}

int MultiplayerManager::getTeamPoints( Player *player )
{
	assert(player);
	
	if ( !player )
		return 0;

	if ( !_inMultiplayerGame )
		return 0;

	// Get the player's team's current points from the game

	return _multiplayerGame->getTeamPoints( player );
}

int MultiplayerManager::getTeamPoints( const str &teamName )
{
	if ( !_inMultiplayerGame )
		return 0;

	// Get the player's team's current points from the game

	return _multiplayerGame->getTeamPoints( teamName );
}

void MultiplayerManager::addTeamPoints( const str &teamName, int points )
{
	if ( !_inMultiplayerGame )
		return;

	// Add team's current points

	_multiplayerGame->addTeamPoints( teamName, points );
}

int MultiplayerManager::getStat( Player *player, int statNum )
{
	int value = 0;

	assert(player);
	
	if ( !player )
		return 0;

	if ( !_inMultiplayerGame )
		return 0;

	//hzm coop mod chrissstrahl - show health
	if ( game.coop_isActive && player->coopPlayer.installed ) {
		Entity *target;
		if ( statNum == STAT_MP_GENERIC4 )
		{
			target = player->GetTargetedEntity();
			if ((target) && target->isSubclassOf(Player))
			{
				Player * targetPlayer = (Player*)target;
				//[b607] chrissstrahl - fix health showing when targeting a different entity that is not a player
				value = (int)(target->getHealth());
				value = (int)(target->getHealth() + 0.99f);
			}
		}
	}
	//end of hzm


//hzm coop mod chrissstrahl
	/*Entity *target;
	Player *targetPlayer;
	int lastPlayer;
	float lastPlayerTime;


	if (statNum == STAT_MP_GENERIC4)
	{
		target = player->GetTargetedEntity();
		//if (target ) //&& ((multiplayerManager.getPlayersTeam(player) == multiplayerManager.getPlayersTeam((Player*)target))
		{
				gi.SendServerCommand(player->entnum, "stufftext \"ui_addhud mp_specialties\"\n");
				value = (int)(target->getHealth() + 0.99f);
		}

		//targetPlayer = multiplayerManager.getPlayer(lastPlayer);
		//(lastPlayer >= 0) && (lastPlayerTime + 1.0f > multiplayerManager.getTime())
		//_playerSpecialtyData[player->entnum]._lastPlayer = -1;
	}*/


	// See if this is one of the stats that the manager cares about

	if ( ( statNum == STAT_TIMELEFT_SECONDS ) && game.coop_isActive == false )
	{
		value = 0;

		if ( mp_timelimit->integer )
		{
			value = (int) (mp_timelimit->integer * 60.0f - level.time);
		}
	}

	if ( statNum == STAT_MP_SPECTATING_ENTNUM )
	{
		value = _playerData[ player->entnum ]._spectatorPlayerNum;
	}

	// Allow all of the modifiers to set the stat

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			value = modifier->getStat( player, statNum, value );
		}
	}

	// Allow the game to set the stat
	value = _multiplayerGame->getStat( player, statNum, value );
	value = _awardSystem->getStat( player, statNum, value );
	return value;
}

int MultiplayerManager::getIcon( Player *player, int statNum )
{
	int value = -1;

	assert(player);
	
	if ( !player )
		return -1;

	if ( !_inMultiplayerGame )
		return -1;

	// Allow all of the modifiers to set the icon

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			value = modifier->getIcon( player, statNum, value );
		}
	}

	// Allow the game to set the icon

	value = _multiplayerGame->getIcon( player, statNum, value );

	value = _awardSystem->getIcon( player, statNum, value );

	return value;
}

Team* MultiplayerManager::getPlayersTeam( const Player *player )
{
	assert(player);
	
	if ( !player )
		return NULL;

	if ( !_inMultiplayerGame )
		return NULL;

	return _multiplayerGame->getPlayersTeam( player );
}

//info - chrissstrahl - this just checks if the vote is within certain parameters, before it will become active
//there is a seperate function for when the vote has passed or failed: MultiplayerManager::checkVote
//there is a seperate function for when a player casts his vote: MultiplayerManager::vote
void MultiplayerManager::callVote( Player *player , const str &command , const str &arg )
{
	if ( !_inMultiplayerGame )
		return;

	// Player wants to call a vote

	if ( checkFlag( MP_FLAG_DONT_ALLOW_VOTE ) )
	{
		HUDPrint( player->entnum , "$$VotingNotAllowed$$.\n" );
		return;
	}

	// If voteTime is set, then a vote has already been called

	if ( _voteTime )
	{
		HUDPrint( player->entnum , "$$AlreadyVoting$$.\n" );
		return;
	}

	// Make sure the player hasn't called too many votes

	if ( _playerData[player->entnum]._votecount >= mp_maxVotes->integer )
	{
		HUDPrint( player->entnum , va( "$$MaxVotes$$ (%d).\n" , mp_maxVotes->integer ) );
		return;
	}

	// Make sure everything is ok

	if ( strchr( command.c_str() , ';' ) || strchr( arg.c_str() , ';' ) )
	{
		HUDPrint( player->entnum , "$$InvalidVote$$\n" );
		return;
	}

	bool validVoteCommand = false;

	//[b607] chrissstrahl - votecommands that are only valid during coop
	if ( game.coop_isActive ) {
		if(coop_vote_checkvalid(command)){
			validVoteCommand = true;
		}
	}
	//[b607] chrissstrahl - NO COOP - allow modifier voteing
	else {
		if (stricmp(command.c_str(), "mp_modifier_actionhero") == 0 ||
			stricmp(command.c_str(), "mp_modifier_autohandicap") == 0 ||
			stricmp(command.c_str(), "mp_modifier_controlpoints") == 0 ||
			stricmp(command.c_str(), "mp_modifier_destruction") == 0 ||
			stricmp(command.c_str(), "mp_modifier_elimination") == 0 ||
			stricmp(command.c_str(), "mp_modifier_instantkill") == 0 ||
			stricmp(command.c_str(), "mp_modifier_oneflag") == 0 ||
			stricmp(command.c_str(), "mp_modifier_pointsperweapon") == 0 ||
			stricmp(command.c_str(), "mp_modifier_specialties") == 0
			)
		{
			//empty value
			if (!stricmp(arg.c_str(), "")) {
				//printout info
				HUDPrint(player->entnum, va("%s 0-1 - $$ObjectiveIncomplete$$:\n", command.c_str()));
				int iModifiersActive = 0;
				if (coop_returnCvarInteger("mp_modifier_actionhero")) { HUDPrint(player->entnum, "actionhero,");  iModifiersActive++; }
				if (coop_returnCvarInteger("mp_modifier_autohandicap")) { HUDPrint(player->entnum, "autohandicap,");  iModifiersActive++; }
				if (coop_returnCvarInteger("mp_modifier_controlpoints")) { HUDPrint(player->entnum, "controlpoints,");  iModifiersActive++; }
				if (coop_returnCvarInteger("mp_modifier_destruction")) { HUDPrint(player->entnum, "destruction");  iModifiersActive++; }
				if (coop_returnCvarInteger("mp_modifier_elimination")) { HUDPrint(player->entnum, "elimination,");  iModifiersActive++; }
				if (coop_returnCvarInteger("mp_modifier_instantkill")) { HUDPrint(player->entnum, "instantkill,"); iModifiersActive++; }
				if (coop_returnCvarInteger("mp_modifier_oneflag")) { HUDPrint(player->entnum, "oneflag,");  iModifiersActive++; }
				if (coop_returnCvarInteger("mp_modifier_pointsperweapon")) { HUDPrint(player->entnum, "pointsperweapon,");  iModifiersActive++; }
				if (coop_returnCvarInteger("mp_modifier_specialties")) { HUDPrint(player->entnum, "specialties,");  iModifiersActive++; }
				if (iModifiersActive > 0) { HUDPrint(player->entnum, "\n"); }
				else{ HUDPrint(player->entnum, "$$None$$\n"); }
				return;
			}
			validVoteCommand = true;
		}
	}
	if (stricmp(command.c_str(), "restart") == 0 ||
		stricmp(command.c_str(), "nextmap") == 0 ||
		stricmp(command.c_str(), "map") == 0 ||
		stricmp(command.c_str(), "kick") == 0 ||
		//[b607] chrissstrahl - added kickbots as valid vote
		stricmp(command.c_str(), "kickbots") == 0 ||
		//hzm gamefix chrissstrahl - fixed bad vote type
		stricmp(command.c_str(), "mp_gametype") == 0 ||
		//hzm gameupdate chrissstrahl - added new vote types
		stricmp(command.c_str(), "exec") == 0 ||
		stricmp(command.c_str(), "skipcinematic") == 0 ||
		stricmp(command.c_str(), "addbot") == 0)
	{
		validVoteCommand = true;
	}

	//hzm gameupdate chrissstrahl check if vote command is valid
	if ( validVoteCommand == false ){
		//[b607] chrissstrahl - tell player how to use mp_modifier
		if (Q_stricmpn(command.c_str(), "mp_modifier",11) == 0) {
			HUDPrint(player->entnum, "$$InvalidVote$$ - mp_modifier_*:\n");
			HUDPrint(player->entnum, "mp_modifier_actionhero <0-1>, mp_modifier_autohandicap <0-1>\n");
			HUDPrint(player->entnum, "mp_modifier_controlpoints <0-1>, mp_modifier_destruction <0-1>\n");
			HUDPrint(player->entnum, "mp_modifier_elimination <0-1>, mp_modifier_instantkill <0-1>\n");
			HUDPrint(player->entnum, "mp_modifier_oneflag <0-1>, mp_modifier_pointsperweapon <0-1>\n");
			HUDPrint(player->entnum, "mp_modifier_specialties <0-1>\n");
			return;
		}

		HUDPrint( player->entnum , "$$InvalidVote$$ - $$VoteCommands$$\n" );

		//[b607] chrissstrahl - coop votestring info
		coop_vote_printcommands(player);
		
		HUDPrint( player->entnum , "restart, nextmap, map , mp_gametype , exec , kick , addbot, kickbots, skipcinematic\n" ); //[b607] added kickbots
		HUDPrint( player->entnum , "mp_modifier_* <0-1> - *instantkill,*actionhero,*pointsperweapon,*specialties\n" ); //[b607] added kickbots
		HUDPrint( player->entnum , "- *autohandicap,*elimination,*controlpoints,*oneflag,*destruction\n" ); //[b607] added kickbots
		return;
	}

	//allow vote for skipping cinematics
	//[b607] chrissstrahl - try to handle the coop vote stuff a little more ellegant
	str _voteStringCompare = _voteString;
	int iVoteValid = 0;
	iVoteValid = coop_vote_skipcinematicValidate(player, command, arg, _voteString);
	if (iVoteValid == 0) { iVoteValid = coop_vote_lastmanstandingValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_respawntimeValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_awardsValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_friendlyfireValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_maxspeedValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_mapValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_mapNxtPrevValidate(player, command, arg, _voteString); }
	//if (iVoteValid == 0) { iVoteValid = coop_vote_mpmodifierValidate(player, command, arg, _voteString); } //handled above
	if (iVoteValid == 0) { iVoteValid = coop_vote_airaccelerateValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_stasistimeValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_teamiconValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_deadbodiesValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_kickValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_execValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_skillValidate(player, command, arg, _voteString); }
	if (iVoteValid == 0) { iVoteValid = coop_vote_addbotValidate(player, command, arg, _voteString); }
	//use the string as command exactly like provided

	//[b607] chrissstrahl - no valid vote (a coop vote was called but with wrong or bad parameters)
	if (iVoteValid == 1) {
		return;
	}

	//[b607] chrissstrahl - the votestring was not altred
	if (_voteStringCompare == _voteString) {
		_voteString = va("%s %s", command.c_str(), arg.c_str());
	}

	//hzm gameupdate chrissstrahl - remember player, sucessfull vote will not add to player vote count
	if ( player ){
		multiplayerManager._voteOwner = player;
	}

	//hzm gameupdate - print the message to the top of the screen, so that EVERYONE can actually see it while in cinematic
	if ( level.cinematic == 1 ) {
		Player	*playerCurrent;
		for (int  i = 0; i < maxclients->integer; i++ ) {
			playerCurrent = getPlayer( i );
			if ( playerCurrent ) {
				//[b608] chrissstrahl - disabled the coop_voteText menu, as it had mouse curser issues that I could not resolve
				//hzm coop mod chrissstrahl - use a menu to show vote stringon center screen
				//if ( playerCurrent->coopPlayer.installed ) {
					//DelayedServerCommand( playerCurrent->entnum , "showmenu coop_voteText" );
				//}
				//else {
				str s , s2;
				s = coop_textReplaceWhithespace( multiplayerManager._voteOwner->client->pers.netname );
				s2 = coop_textReplaceWhithespaceBlack( _voteString.c_str() );
				DelayedServerCommand( playerCurrent->entnum , va( "locationprint 220 455 %s^0_^8$$CalledVote$$:^0_^8%s 0.8" , s.c_str() , s2.c_str() ) );
				//}
			}
		}
	}

	// Play a sound to announce a new vote

	//sound/ships/attrexian/att-beepreject.wav
	//sound/environment/computer/lcars_door.wav
	//sound/player/CommunicatorBeep.wav
	//sound/ships/enterprise/eng_warpcore_alarm.wav
	//sound\ships\forever\for_alarm_beeps.wav
	//for_alarm_breach.wav

	//hzm gameupdate - try different sounds to make players notice that there is a vote going on, also only play sound if ther is more than 1 player
	if ( getTotalPlayers( true ) > 1 ) {
		broadcastInstantSound( "sound/environment/computer/alarm1.wav" );
	}

	// Start the voting, the caller automatically votes yes
	_voteTime = level.time;
	_voteNo = 0;
	_voteYes = 1;
	_playerData[player->entnum]._votecount++;
	_playerData[player->entnum]._voted = true;

	//hzm coop mod chrissstrahl - we need to remember who started the vote, for additional features
	player->coopPlayer.startedVote = true;
	//hzm eof

	// Clear all the other player's voteflags

	for (int clientNumberA = 0; clientNumberA < maxclients->integer; clientNumberA++ )
	{
		Player *currentPlayer;
		gentity_t *ent = g_entities + clientNumberA;

		if (!ent->inuse || !ent->client || !ent->entity) {
			continue;
		}

		if (ent->svflags & SVF_BOT) {
			continue;
		}

		currentPlayer = getPlayer(clientNumberA);

		if (currentPlayer == player ){
			//hzm gamefix chrissstrahl - show info to player who started the vote
			//[b607] chrissstrahl - now using _voteString, supporting modified votestring for votes like coop_next...
			if ( coop_checkPlayerLanguageGerman( currentPlayer ) )
				currentPlayer->setVoteText( va("Ihre Abstimmung ist aktiv:\n%s", _voteString.c_str()));
			else
				currentPlayer->setVoteText( va("Your Vote is in Progress:\n%s", _voteString.c_str()));
			//hzm eof
			continue;
		}

		_playerData[clientNumberA]._voted = false;

		if ( currentPlayer )
		{
			//hzm coop mod chrissstrahl - translate the vote strings forbetter understanding
			str sVoteText = "";
			bool bALt = false;

			//hzm coop mod chrissstrahl - handle coop specific votes in a seperate function
			bALt = coop_textModifyVoteText( _voteString , sVoteText , currentPlayer );

			//hzm coop mod chrissstrahl - print it in german if player is using german game version
			if ( coop_checkPlayerLanguageGerman( currentPlayer ) ){
				if ( coop_returnIntFind( _voteString , "skipcinematic" ) > -1 ){
					bALt = true; sVoteText = "Sequenz abbrechen ?";
					sVoteText += coop_returnStringStartingFrom( _voteString , 13 );
				}
			}
			else {
				if ( coop_returnIntFind( _voteString , "skipcinematic" ) > -1 ){
					bALt = true; sVoteText = "Skip Cinematic ?";
					sVoteText += coop_returnStringStartingFrom( _voteString , 13 );
				}
			}
			//hzm decide which string to print
			if (!bALt) {
				sVoteText = _voteString;
			}
			//[b608] chrissstrahl - restored default vote text
			currentPlayer->setVoteText(va("$$NewVote$$: %s", sVoteText.c_str()));
		}
	}
}

void MultiplayerManager::vote( Player *player, const str &vote )
{
	// Player is voting on the current vote

	// Make sure the is a vote current going on

	if ( !_voteTime )
	{
		HUDPrint( player->entnum, "$$NoVote$$\n" );
		return;
	}

	// Make sure this player hasn't already voted on this vote

	if ( _playerData[ player->entnum ]._voted )
	{
		HUDPrint( player->entnum, "$$VoteAlreadyCast$$\n" );
		return;
	}

	// Make sure the vote is valid

	if ( vote.length() < 1 )
		return;

	// Vote

	_playerData[ player->entnum ]._voted = true;

	player->clearVoteText();

	if ( ( vote[0] == 'y' ) || ( vote[0] == 'Y' ) || ( vote[0] == '1' ) )
	{
		_voteYes++;

gi.Printf(va("COOPDEBUG $$VoteCast$$ - $$Yes$$: %s\n", player->client->pers.netname));

		//[b607] chrisssttrahl - let others know that this player has voted
		HUDPrintAllClients(va("$$VoteCast$$ - $$Yes$$: %s\n", player->client->pers.netname));
		//HUDPrint( player->entnum, "$$VoteCast$$ - $$Yes$$" );
	}
	else
	{
		_voteNo++;

gi.Printf(va("COOPDEBUG $$VoteCast$$ - $$No$$: %s\n", player->client->pers.netname));

		//[b607] chrisssttrahl - let others know that this player has voted
		HUDPrintAllClients(va("$$VoteCast$$ - $$No$$: %s\n", player->client->pers.netname));
		//HUDPrint( player->entnum, "$$VoteCast$$ - $$No$$\n" );;
	}

	// NOTE: a majority will be determined in checkVote
}

//[b607] chrissstrahl - multiple withespaces added to vote string and added coop_teamIcon vote
//info - chrissstrahl - this checks if the vote has passed or failed
void MultiplayerManager::checkVote( void )
{
	// Check if a vote is active
	if ( !_voteTime ){
		return;
	}

	//hzm gameupdate chrissstrahl - when there is a skipcinematic vote, but cinematic is over, end the vote automatically
	if ( level.cinematic == false && !Q_stricmpn( _voteString , "skipcinematic" , 13 ) )
	{
		_voteTime = 0.0f;
	}

	// Make sure time hasn't run out for this vote
	if ( level.time - _voteTime >= _maxVoteTime ){
		//hzm gameupdate - print the message to the top of the screen, so that EVERYONE can actually see it while in cinematic
		if ( level.cinematic == false ){
			multiplayerManager.HUDPrintAllClients( "$$VoteFailed$$\n" ); //[b607] chrissstrahl - removed extra line printout
		}else{
			int i;
			Player *currentPlayer;
			for ( i = 0; i < maxclients->integer; i++ ){
				currentPlayer = getPlayer( i );
				if ( currentPlayer ){
					DelayedServerCommand( currentPlayer->entnum , "locationprint 220 455 ^0_^8$$VoteFailed$$ 0.8" );
				}
			}
		}
		//hzm gameupdate chrissstrahl - reset skip status
		game.cinematicSkipping = false;
	}
	else
	{
		// See if we have a majority vote yet
		//[b610] chrissstrahl - get playercount excluding bots
		//this fixes a bug where this var was randomly 0 when a vote was started
		_numVoters = coop_returnPlayerQuantity(3);

		if ( _voteYes > ( _numVoters / 2.0f ) )
		{
			// Vote passed - execute the command, then remove the vote

			//hzm gameupdate - don't display the message if there is only 1 player on the server, and don't display message when a cinematic is skipped, it spoils the mood
			if ( level.cinematic == false){ //[b607] chrissstrahl - removed that he message is not shown if only one player is on the server
				multiplayerManager.HUDPrintAllClients( va("$$VotePassed$$: %s\n", _voteString.c_str()) );
gi.Printf(va("COOPDEBUG CALLVOTE checkVote $$VotePassed$$ [%i][%i]\n", _voteYes, _numVoters));
			}

			//allow vote for skipping cinematics
			//[b607] chrissstrahl - try to handle the coop vote stuff a little more ellegant
			bool bCoopVote = false;
			bCoopVote = coop_vote_skipcinematicSet(_voteString.c_str());
			if (!bCoopVote)		 { bCoopVote = coop_vote_lastmanstandingSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_respawntimeSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_awardsSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_friendlyfireSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_maxspeedSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_mapSet(_voteString.c_str()); }
			//[b607] chrissstrahl - handle mp modifier type votes
			if (!bCoopVote) { bCoopVote = coop_vote_mpmodifierSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_airaccelerateSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_stasistimeSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_teamiconSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_deadbodiesSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_kickbotsSet(_voteString.c_str()); }
			if (!bCoopVote) { bCoopVote = coop_vote_execSet(_voteString.c_str()); }

			//use the string as command exactly like provided
			if(!bCoopVote){
				gi.SendConsoleCommand(va("%s\n", _voteString.c_str()));
			}

			//hzm coop mod chrissstrahl - if a vote was sucessfull, do not count up the player count
			if ( multiplayerManager._voteOwner ){
				if ( multiplayerManager._voteOwner->isSubclassOf( Player ) ){
					_playerData[_voteOwner->entnum]._votecount--;
					//multiplayerManager._voteOwner = NULL;
				}
			}
		}
		else if ( _voteNo >= ( _numVoters / 2.0f ) )
		{
			// Vote failed - same behavior as a timeout
			multiplayerManager.HUDPrintAllClients( "$$VoteFailed$$\n" );
gi.Printf(va("COOPDEBUG CALLVOTE checkVote $$VoteFailed$$ [%i][%i]\n", _voteYes, _numVoters));
		}
		else
		{
			// still waiting for a majority
			return;
		}
	}

	_voteTime = 0.0f;

	// Clear all the player's vote text
	for( int i = 0; i < maxclients->integer; i++ ){
		Player *currentPlayer;
		currentPlayer = getPlayer( i );
		if ( currentPlayer ){
			//[b610] chrissstrahl - reset vote status
			_playerData[i]._voted = false;

			currentPlayer->clearVoteText();
		}
	}
}

void MultiplayerManager::joinTeam( Player *player, const str &teamName )
{
	str realTeamName;

	int i;

	// Make sure everything is ok

	if ( !player || !_inMultiplayerGame )
		return;

	// Fix up the teamName

	if ( stricmp( teamName.c_str(), "Blue" ) == 0 )
		realTeamName = "Blue";
	else if ( stricmp( teamName.c_str(), "Red" ) == 0 )
		realTeamName = "Red";
	else
		realTeamName = teamName;

	if ( ( realTeamName == "Red" ) || ( realTeamName == "Blue" ) || ( realTeamName == "normal" ) )
	{
		_playerData[ player->entnum ]._spectatorByChoice = false;
	}

	// See if we can change to the specified team

	if ( !_multiplayerGame->canJoinTeam( player, realTeamName ) )
		return;

	// Tell the modifiers that player left game

	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			modifier->removePlayer( player );
		}
	}

	// Tell mode that I'm joining the specified team

	_multiplayerGame->joinTeam( player, realTeamName );

	// Tell all of the modifiers that I'm joining the specified team

	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			modifier->addPlayer( player );

			modifier->joinedTeam( player, realTeamName );
		}
	}
}

void MultiplayerManager::say( Player *player, const str &text, bool team )
{
	str realText;
	str name;

	//hzm gameupdate chrissstrahl - grab commands here that should work for our new features
	//hzm gameupdate chrissstrahl - this handels the tricorder puzzles code/script and ingame menus and branchdialog
	int i;
	str tempText = text;

	int startAt = 0;
	bool threadIsValid = 0;

	//hzm gameupdate chrissstrahl - remove useless ^
	//check if player accidently typed ^ into console
	// hzm gameupdate chrissstrahl - check if player used /, thats a bad habbit from older games
	int startLoc = 0;
	for ( int i = 0; i < tempText.length();i++ )
	{		
		if ( tempText[i] == '^' || tempText[i] == '/' || tempText[i] == '\\')
		{
			startLoc++;
		}
		else
		{
			break;
		}
	}
	// hzm gameupdate chrissstrahl - remove useless ^ from text
	tempText = coop_returnStringStartingFrom( tempText , startLoc );

	if ( !stricmp( tempText.c_str() , "" ) ) { return; }

	//hzm gameupdate chrissstrahl - make tricorderpuzzels an other default gamplay components from singleplayer work
	if ( !Q_stricmpn( "clientrunthread " , tempText , 16 ) )
	{
		if ( !Q_stricmpn( "clientrunthread exitRoutine" , tempText , 27 )			||
				!Q_stricmpn( "clientrunthread tricorderMod_" , tempText , 29 )		||
				!Q_stricmpn( "clientrunthread tricorderKeypad_" , tempText , 32 )	||
				!Q_stricmpn( "clientrunthread useLibraryTerminal" , tempText , 34 )	||
				!Q_stricmpn( "clientrunthread trirteClick" , tempText , 27 )		||
				!Q_stricmpn( "clientrunthread tricorderBaseCancel" , tempText , 35 )
		){
			threadIsValid = 1;
			startAt = 16;
		}
	}
	else if ( !Q_stricmpn( "ServerThreadToRun " , tempText , 18 ) )
	{
		if (	!Q_stricmpn( "ServerThreadToRun trirteClick" , tempText , 29 )			|| 
				!Q_stricmpn( "ServerThreadToRun useLibraryTerminal" , tempText , 46 )
		){
			threadIsValid = 1;
			startAt = 18;
		}
	}
	else if ( !Q_stricmpn( "script thread " , tempText , 14 ) )
	{
		if ( !Q_stricmpn( "script thread globalTricorder" , tempText , 29 ) ||
			!Q_stricmpn( "script thread trirteTT" , tempText , 22 ) ||
			!Q_stricmpn( "script thread _tricorderRoute_" , tempText , 30 ) ||
			!Q_stricmpn( "script thread _tricorderBase_" , tempText , 30 ) ||
			!Q_stricmpn( "script thread libraryTerminal_tooltip" , tempText , 37 )
			){
			threadIsValid = 1;
			startAt = 14;
		}
	}

	if ( threadIsValid )
	{
		str puzzleRunThread;
		for ( i = startAt; i < ( tempText.length() - 1 ); i++ )
		{
			if ( tempText != '"' && tempText != '%' && tempText != ' ' )
			{
				puzzleRunThread += tempText[i];
			}
		}
		//use this to check if your input/output is right
		//player->hudPrint( va( "'%s'\n" , puzzleRunThread.c_str()) );

		ExecuteThread( puzzleRunThread.c_str() , true , ( Entity * )player );
		return;
	}


	if ( player )
	{
		name = player->client->pers.netname;
	}
	else
	{
//hzm gamefix chrissstrahl - do not print "Server: score" when a player connects while he has the score still displayed, this can happen when a map changes, and scores are shown
		if ( !Q_stricmp( "score" , tempText ) )
		{
			return;
		}
		name = "$$ServerName$$";
	}

	//hzm coop mod chrissstrahl - this handels the coop specific say commands
	if (coop_playerSay(player, tempText)) {
		return;
	}

	realText = name;
	realText += ":";

	// Color the say (team - yellow, normal - cyan)

	if ( player )
	{
		realText += "^";

		if ( team )
			realText += COLOR_YELLOW;
		else
			realText += COLOR_CYAN;

		realText += " ";
	}

	// Get rid of all color in the say so our defaults will stay

	for ( i = 0 ; i < tempText.length() ; i++ )
	{
		if ( tempText[ i ] == '^' )
		{
			tempText[ i ] = '*';
		}
	}

	// Add the say text

	realText += tempText;

	// Finish up with the text

	if ( player )
	{
		realText += "^";
		realText += COLOR_NONE;
	}

	realText += "\n";

	// Strip out any bad characters

	for ( i = 0 ; i < realText.length() ; i++ )
	{
		if ( realText[ i ] == '%' )
		{
			realText[ i ] = '.';
		}
	}

	// Don't let text be too long for malicious reasons
	if ( realText.length() > _maxSayStringLength ){
		HUDPrint(player->entnum, "$$SayTooLong$$\n");
		return;
	}

	// Send say to console if in a dedicated server
	if ( dedicated->integer )
	{
		gi.Printf( "%s: %s\n", name.c_str(), text.c_str() );
	}

	// Send the say to appropriate clients
	if ( player && isPlayerSpectator( player ) )
		HUDPrintSpectators( player, realText, team );
	else if ( player && team )
		HUDPrintTeamClients( player, realText );
	else
		HUDPrintAllClients( realText );
}

void MultiplayerManager::tell( Player *player, const str &text, int entnum )
{
	str realText;
	str tempText;
	int i;
	Player *otherPlayer;


	realText = player->client->pers.netname;
	realText += ":";

	// Color the say (team - yellow, normal - cyan)

	realText += "^";
	realText += COLOR_MAGENTA;
	realText += " ";

	// Get rid of all color in the say so our defaults will stay

	tempText = text;

	for ( i = 0 ; i < tempText.length() ; i++ )
	{
		if ( tempText[ i ] == '^' )
		{
			tempText[ i ] = '*';
		}
	}

	// Add the say text

	realText += tempText;

	// Finish up with the text

	realText += "^";
	realText += COLOR_NONE;
	realText += "\n";

	// Don't let text be too long for malicious reasons

	if ( realText.length() > _maxSayStringLength )
	{
		HUDPrint( player->entnum, "$$SayTooLong$$\n" );
		return;
	}

	// Get other player

	otherPlayer = getPlayer( entnum );

	if ( !otherPlayer )
		return;


	// Send the say to appropriate clients

	if ( isPlayerSpectator( player ) && !isPlayerSpectator( otherPlayer ) )
		return;

	HUDSay( otherPlayer->entnum, realText );
}

//
// Modifier stuff
//

void MultiplayerManager::addModifiers( void )
{
	tryAddModifier( "InstantKill" );
	tryAddModifier( "ActionHero" );
	tryAddModifier( "AutoHandicap" );
	tryAddModifier( "PointsPerWeapon" );
	tryAddModifier( "ControlPoints" );
	tryAddModifier( "Destruction" );
	tryAddModifier( "OneFlag" );
	tryAddModifier( "Elimination" );
	tryAddModifier( "Diffusion" );
	tryAddModifier( "Specialties" );
}

void MultiplayerManager::tryAddModifier( const str &modifierName )
{
	//[b607] chrissstrahl - no modifiers in coop
	if (game.coop_isActive) {
		return;
	}

	cvar_t *mp_modifier;
	str cvarName;

	cvarName = "mp_modifier_";
	cvarName += modifierName;

	mp_modifier = gi.cvar( cvarName.c_str(), "0", 0 );

	if ( mp_modifier->integer )
	{
		addModifier( modifierName );
	}
}

void MultiplayerManager::addModifier( const str &modifierName )
{
	MultiplayerModifier *newModifier = NULL;

	if ( modifierName == "InstantKill" )
	{
		newModifier = new ModifierInstantKill;
	}
	else if ( modifierName == "ActionHero" )
	{
		newModifier = new ModifierActionHero;
	}
	else if ( modifierName == "AutoHandicap" )
	{
		newModifier = new ModifierAutoHandicap;
	}
	else if ( modifierName == "PointsPerWeapon" )
	{
		newModifier = new ModifierPointsPerWeapon;
	}
	else if ( modifierName == "ControlPoints" )
	{
		newModifier = new ModifierControlPoints;
	}
	else if ( modifierName == "Destruction" )
	{
		newModifier = new ModifierDestruction;
	}
	else if ( modifierName == "OneFlag" )
	{
		newModifier = new ModifierOneFlag;
	}
	else if ( modifierName == "Elimination" )
	{
		newModifier = new ModifierElimination;
	}
	else if ( modifierName == "Diffusion" )
	{
		newModifier = new ModifierDiffusion;
	}
	else if ( modifierName == "Specialties" )
	{
		newModifier = new ModifierSpecialties;
	}

	if ( newModifier )
	{
		_modifiers.AddObject( newModifier );
	}
}

void MultiplayerManager::addPoints( int entnum, int points )
{
	if ( !_inMultiplayerGame )
		return;

	_multiplayerGame->addPoints( entnum, points );
}

//
// Interface for modes
//

int MultiplayerManager::getClientNum( int entnum )
{
	gclient_s *client;

	// Get the client

	client = getClient( entnum );

	if ( client )
		return client->ps.clientNum;
	else
		return -1;
}

int MultiplayerManager::getClientPing( int entnum )
{
	gclient_s *client;

	// Get the client

	client = getClient( entnum );

	if ( client )
		return client->ps.ping;
	else
		return -1;
}

float MultiplayerManager::getTime( void )
{
	return level.time;
}

void MultiplayerManager::centerPrint( int entnum, const str &string, CenterPrintImportance importance )
{
	gentity_t *gentity;

	if ( ( entnum >= 0 ) && ( entnum < maxclients->integer ) )
	{
		gentity = &g_entities[ entnum ];

		if ( gentity->inuse && gentity->entity && gentity->client )
		{
			gi.centerprintf( gentity, importance, string.c_str() );
		}
	}
}

Player *MultiplayerManager::getPlayer( int entnum )
{
	gentity_t *gentity;
	Player *player = NULL;

	// Make sure everything is ok

	if ( ( entnum >= 0 ) && ( entnum < maxclients->integer ) )
	{
		gentity = &g_entities[ entnum ];

		if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) )
		{
			player = (Player *)gentity->entity;
		}
	}

	return player;
}

int MultiplayerManager::getMaxPlayers( void )
{
	return maxclients->integer;
}

int MultiplayerManager::getTotalPlayers( bool countSpectators )
{
	int i;
	Player *player;
	int numPlayers;

	numPlayers = 0;

	for( i = 0 ; i < maxclients->integer ; i++ )
	{
		player = getPlayer( i );

		if ( player )
		{
			if ( !isPlayerSpectator( player ) || !countSpectators )
			{
				numPlayers++;
			}
		}

	}
	return numPlayers;
}

gclient_s *MultiplayerManager::getClient( int entnum )
{
	gentity_t *gentity;

	// Make sure everything is ok

	if ( ( entnum >= 0 ) && ( entnum < maxclients->integer ) )
	{
		gentity = &g_entities[ entnum ];

		if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) )
		{
			return gentity->client;
		}
	}

	return NULL;
}

void MultiplayerManager::HUDSay( int entnum, const str &string )
{
	str command;
	Player *player;

	// Get the player

	player = getPlayer( entnum );

	if ( player )
	{

		//[b607] chrissstrahl - make sure we do not handle bots
		gentity_t *ent = player->edict;
		if (ent->svflags & SVF_BOT) {
			return;
		}

		// Build the hud print command to send to the client

		command = "hudsay \"";
		command += string;
		command += "\"\n";

		if ( gi.GetNumFreeReliableServerCommands( player->edict - g_entities ) > 32 )
		{
			gi.SendServerCommand( player->edict - g_entities, command.c_str() );
		}
	}
}

void MultiplayerManager::HUDPrint( int entnum, const str &string )
{
	Player *player;

	player = getPlayer( entnum );
	if ( player )
	{
		player->hudPrint( string );
	}
}

void MultiplayerManager::statusPrint( int entnum, const str &string )
{
	str command;
	Player *player;

	// Get the player

	player = getPlayer( entnum );

	if ( player )
	{
		// Build the status print command to send to the client

		command = "status \"";
		command += string;
		command += "\"\n";

		gi.SendServerCommand( player->edict - g_entities, command.c_str() );
	}
}

void MultiplayerManager::playerSound( int entnum, const str &soundName, int channel, float volume, float minDist, float time )
{
	Player *player;

	// Get the player

	player = getPlayer( entnum );

	// Tell the player to play a sound

	if ( player )
	{
		addSoundToQueue( player, soundName, channel, volume, minDist, time );
		//player->Sound( soundName, channel, volume, minDist, NULL, 1.0f, true );
	}
}

void MultiplayerManager::instantPlayerSound( int entnum, const str &soundName, int channel, float volume, float minDist )
{
	Player *player;

	// Get the playaer

	player = getPlayer( entnum );

	// Tell the player to play a sound

	if ( player )
	{
		player->Sound( soundName, channel, volume, minDist, NULL, 1.0f, true );
	}
}

void MultiplayerManager::broadcastInstantSound( const str &soundName, int channel, float volume, float minDist, Player *except )
{
	int i;
	Player *player;

	// Go through all of the clients

	for( i = 0; i < maxclients->integer; i++ )
	{
		// Get the player

		player = getPlayer( i );

		// Tell the player to play a sound

		if ( player && ( player != except ) )
		{
			instantPlayerSound( player->entnum, soundName, channel, volume, minDist );
		}
	}
}

void MultiplayerManager::teamSound( Team *team, const str &soundName, int channel, float volume, float minDist, float time )
{
	int i;
	Player *player;

	// Go through all of the clients

	for( i = 0; i < maxclients->integer; i++ )
	{
		// Get the player

		player = getPlayer( i );

		// Tell the player to play a sound

		if ( player && ( getPlayersTeam( player ) == team ) )
		{
			playerSound( player->entnum, soundName, channel, volume, minDist, time );
		}
	}
}

void MultiplayerManager::broadcastSound( const str &soundName, int channel, float volume, float minDist, Player *except, float time )
{
	int i;
	Player *player;

	// Go through all of the clients

	for( i = 0; i < maxclients->integer; i++ )
	{
		// Get the player

		player = getPlayer( i );

		// Tell the player to play a sound

		if ( player && ( player != except ) )
		{
			playerSound( player->entnum, soundName, channel, volume, minDist, time );
		}
	}
}

bool MultiplayerManager::isPlayerSpectator( Player *player, SpectatorTypes spectatorType )
{
	if ( !_inMultiplayerGame )
		return false;

	if ( spectatorType == SPECTATOR_TYPE_ANY )
		return _playerData[ player->entnum ]._spectator;
	else if ( spectatorType == SPECTATOR_TYPE_NONE )
		return !_playerData[ player->entnum ]._spectator;
	else
		return ( _playerData[ player->entnum ]._spectator && _playerData[ player->entnum ]._spectatorType == spectatorType );
}

bool MultiplayerManager::isPlayerSpectatorByChoice( Player *player )
{
	if ( !_inMultiplayerGame )
		return false;

	if ( _playerData[ player->entnum ]._spectator &&  _playerData[ player->entnum ]._spectatorByChoice )
		return true;
	else
		return false;
}

Player *MultiplayerManager::getPlayerSpectating( Player *player )
{
	if ( isPlayerSpectator( player, SPECTATOR_TYPE_FOLLOW ) )
	{
		return getPlayer( _playerData[ player->entnum ]._spectatorPlayerNum );
	}

	return NULL;
}

void MultiplayerManager::makePlayerSpectator( Player *player, SpectatorTypes spectatorType, bool byChoice )
{
	Team *team;

	// Make the player into a spectator

	if ( player )
	{
		//hzm coop mod chrissstrahl - manage coop related spectator stuff
		//[b607] this must be executed first before the spectatordata is set on player
		coop_playerSpectator( player );
		
		initPlayer( player );

		_playerData[ player->entnum ]._spectator = true;
		_playerData[ player->entnum ]._spectatorType = spectatorType;
		_playerData[ player->entnum ]._spectatorTime = getTime();

		if ( byChoice )
		{
			_playerData[ player->entnum ]._spectatorByChoice = true;
		}

		_playerData[ player->entnum ]._spectatorPlayerNum = 0;

		// Print something useful to the player

		//hzm gameupdate chrissstrahl - keep text minimal after failure
		if(!level.mission_failed)
		{
			if ( spectatorType == SPECTATOR_TYPE_FOLLOW ){
				centerPrint( player->entnum , "$$SpectatorFollow$$" , CENTERPRINT_IMPORTANCE_NORMAL );
			}

			else if ( spectatorType == SPECTATOR_TYPE_FREEFORM ) {
				centerPrint( player->entnum , "$$SpectatorFreeForm$$" , CENTERPRINT_IMPORTANCE_NORMAL );
			}
			else{
				centerPrint( player->entnum , "$$SpectatorNormal$$" , CENTERPRINT_IMPORTANCE_NORMAL );
			}
		}

		if ( spectatorType == SPECTATOR_TYPE_FOLLOW )
		{
			Player *playerToSpectate = getLastKillerOfPlayer( player );

			if ( playerToSpectate )
				makePlayerSpectatePlayer( player, playerToSpectate );
			else
				makePlayerSpectateNextPlayer( player );
		}

		player->takedamage              = DAMAGE_NO;
		player->client->ps.feetfalling  = false;
		player->deadflag				= DEAD_NO;

		player->flags &= ~FL_IMMOBILE;
		player->flags &= ~FL_STUNNED;

		//player->movecontrol             = MOVECONTROL_USER;

		// Hide the player model

		player->hideModel();

		// Force them to the stand state

		player->SetState( "STAND", "STAND" );

		// Go not solid

		player->setSolidType( SOLID_NOT );

		// Move in normal walking mode

		//player->setMoveType( MOVETYPE_NOCLIP );
		player->setMoveType( MOVETYPE_WALK );

		// Get rid of the inventory
		
		player->FreeInventory();
		player->disableInventory();

		team = getPlayersTeam( player );

		if ( !team )
			setTeamHud( player, "mp_teamspec" );
		else if ( team->getName() == "Red" )
			setTeamHud( player, "mp_teamredspec" );
		else
			setTeamHud( player, "mp_teambluespec" );

		player->clearItemText();

//hzm gameupdate chrissstrahl - make player view from the current camera, don't bother to show player his own view
		//[b608] chrissstrahl - fixed missing reference to the player
		if ( level.cinematic == 1 && game.cinematicCurrentCam != NULL )
		{
			player->SetCamera( (Camera *)(Entity *)game.cinematicCurrentCam , 0 );
		}
	}
}

void MultiplayerManager::makePlayerSpectateNextPlayer( Player *player )
{
	int startIndex;
	int currentIndex;
	Player *testPlayer;

	if ( !isPlayerSpectator( player, SPECTATOR_TYPE_FOLLOW ) )
		return;
	
	startIndex = _playerData[ player->entnum ]._spectatorPlayerNum;

	currentIndex = startIndex + 1;

	if ( currentIndex >= maxclients->integer )
		currentIndex = 0;

	while ( currentIndex != startIndex )
	{
		testPlayer = getPlayer( currentIndex );

		if ( testPlayer && !isPlayerSpectator( testPlayer ) )
		{
			_playerData[ player->entnum ]._spectatorPlayerNum = currentIndex;
			return;
		}

		currentIndex++;

		if ( currentIndex >= maxclients->integer )
			currentIndex = 0;
	}

	testPlayer = getPlayer( currentIndex );

	if ( testPlayer && !isPlayerSpectator( testPlayer ) )
	{
		_playerData[ player->entnum ]._spectatorPlayerNum = currentIndex;
		return;
	}
	else
	{
		// If we get here there isn't anyone to spectate

		makePlayerSpectator( player, SPECTATOR_TYPE_NORMAL );
	}
}

void MultiplayerManager::makePlayerSpectatePrevPlayer( Player *player )
{
	int startIndex;
	int currentIndex;
	Player *testPlayer;

	if ( !isPlayerSpectator( player, SPECTATOR_TYPE_FOLLOW ) )
		return;
	
	startIndex = _playerData[ player->entnum ]._spectatorPlayerNum;

	currentIndex = startIndex - 1;

	if ( currentIndex < 0 )
		currentIndex = maxclients->integer - 1;

	while ( currentIndex != startIndex )
	{
		testPlayer = getPlayer( currentIndex );

		if ( testPlayer && !isPlayerSpectator( testPlayer ) )
		{
			_playerData[ player->entnum ]._spectatorPlayerNum = currentIndex;
			return;
		}

		currentIndex--;

		if ( currentIndex < 0 )
			currentIndex = maxclients->integer - 1;
	}

	// If we get here there isn't anyone to spectate

	makePlayerSpectator( player, SPECTATOR_TYPE_NORMAL );
}

void MultiplayerManager::makePlayerSpectatePlayer( Player *player, Player *playerToSpectate )
{
	if ( !isPlayerSpectator( player, SPECTATOR_TYPE_FOLLOW ) )
		return;
	
	if ( playerToSpectate && !isPlayerSpectator( playerToSpectate ) )
	{
		_playerData[ player->entnum ]._spectatorPlayerNum = playerToSpectate->entnum;
		return;
	}

	// PlayerToSpectate wasn't valid, try the next player instead

	makePlayerSpectateNextPlayer( player );
}

void MultiplayerManager::playerEnterArena( int entnum, float health )
{
	Player *player;
	Team *team;

	player = getPlayer( entnum );

	if ( !player )
		return;

	//[b607] chrissstrahl - remove this hud if player respawns
	if (player->coopPlayer.clickFireHudActive) {
		player->coopPlayer.clickFireHudActive = false;
		if (player->coopPlayer.installed) {
			player->removeHud("coop_fireToSpawn");
		}
	}

	//hzm gameupdate chrissstrahl - make player view from the current camera, don't bother to show player his own view
	if ( level.cinematic == 1 && game.cinematicCurrentCam != NULL )
	{
		player->SetCamera( (Camera *)(Entity *)game.cinematicCurrentCam, 0 );
	}

	//hzm coop mod chrissstrahl - coop mod prevent player from respawning if LMS is active and player died
	if ( !coop_playerSpawnLms(player) )
	{
		return;
	}

	//hzm gameupdate chrissstrahl - prevent player from going into spec when dead and then reentering the game without waiting
	if ( getRespawnTime() > 0.0f )
	{
		if ( !game.coop_lastmanstanding && _playerData[player->entnum]._waitingForRespawn )
		{
			makePlayerSpectator( player , SPECTATOR_TYPE_FOLLOW , false );
			return;
		}
	}

	_playerData[player->entnum]._waitingForRespawn = false;
	//end of hzm

	_playerData[ entnum ]._spectator = false;
	_playerData[ entnum ]._spectatorByChoice = false;

	// Make sure player is solid and can take damage

	// Get rid of our inventory

	player->FreeInventory();

	player->dropRune();
	player->dropPowerup();
	player->removeHoldableItem();

	player->ProcessEvent( EV_Sentient_StopOnFire );
	player->stopStasis();

	player->removeAttachedModelByTargetname( "attachedSpecialityBackpack" );
	player->removeAttachedModelByTargetname( "attachedDiffusionBomb" );
	player->removeAttachedModelByTargetname( "actionHero" );

	player->clearTempAttachments();

	player->enableInventory();

	changePlayerModel( player , player->model , true );

	//   Init();	

//hzm coop mod chrissstrahl - do not show teamhuds in coop
	if ( !game.coop_isActive )
	{
		// Set the player's health to the specified value
		player->health = health;

		//hzm gameupdate chrissstrahl - do not display beam effect while in cinematic
		if ( level.cinematic == false )
		{
			// hold in place briefly
			player->client->ps.pm_time = 100;
			player->client->ps.pm_flags |= PMF_TIME_TELEPORT;

			Event *newEvent = new Event( EV_DisplayEffect );
			newEvent->AddString( "TransportIn" );
			newEvent->AddString( "Multiplayer" );
			player->PostEvent( newEvent , 0.0f );
		}

		team = getPlayersTeam(player);

		if (!team)
			setTeamHud(player, "");
		else if (team->getName() == "Red")
			setTeamHud(player, "mp_teamred");
		else
			setTeamHud(player, "mp_teamblue");
	}

//hzm gamefix - drowning
	player->damageByWater = 0;

	//hzm gameupdate chrissstrahl - do not show player while in cinematic, >>baby don't hurt me, don't hurt me no more<<
	if ( level.cinematic == false )
	{
		//hzm gameupdate chrissstrahl - not suppose to be solid
		//player->_makeSolidASAPSupposedToBeSolid = false;
		player->_makeSolidASAPTime = 0.0f;
		player->_makeSolidASAP = true;
		//end of hzm

		player->showModel();
		//player->takedamage = DAMAGE_YES;
		//player->setSolidType( SOLID_BBOX );
	}
	else{
		//hzm gameupdate chrissstrahl - suppose to be solid
		//player->_makeSolidASAPSupposedToBeSolid = false;
		player->_makeSolidASAPTime = -1.0f;
		player->_makeSolidASAP = false;
		//end of hzm

		player->SetState( "STAND" , "STAND" );
		player->setSolidType( SOLID_NOT );
		player->takedamage = DAMAGE_NO;
		player->cinematicStarted();
		//player->hideModel();
		//hzm gameupdate chrissstrahl - hide with delay, to fix issues
		Event *hidePlayer;
		hidePlayer = new Event( EV_Hide );
		player->PostEvent( hidePlayer , 0.1f );
	}

//hzm coop mod chrissstrahl - handle player for the coop mod
	coop_playerEnterArena( entnum , health );
}

void MultiplayerManager::playerSpawned( Player *player )
{
	// Tell all of the modifiers that the player has respawned

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			modifier->playerSpawned( player );
		}
	}
}

void MultiplayerManager::allowFighting( bool allowFighting )
{
	_allowFighting = allowFighting;
}

void MultiplayerManager::centerPrintAllClients( const str &string, CenterPrintImportance importance )
{
	int i;
	gentity_t *ent;

	// Go through all of the clients

	for( i = 0; i < maxclients->integer; i++ )
	{
		ent = g_entities + i;

		// Make sure this is a valid client

		if ( !ent->inuse || !ent->client || !ent->entity )
			continue;

		// Send centerprint command to the client

		centerPrint( ent->entity->entnum, string, importance );
	}
}

void MultiplayerManager::HUDPrintAllClients( const str &string )
{
	int i;
	gentity_t *ent;

	// Go through all of the clients

	for( i = 0; i < maxclients->integer; i++ )
	{
		ent = g_entities + i;

		// Make sure this is a valid client
		//hzm gamefix chrissstrahl - make sure this does not make issues if called before inizialised check g_entities
		if (g_entities == NULL || !ent->inuse || !ent->client || !ent->entity || !ent->entity->isSubclassOf(Player) || ent->svflags & SVF_BOT) //[b607] chrissstrahl - don't print for bots
			continue;

		// Send hud print command to the client

		HUDSay( ent->entity->entnum, string );
	}
}

void MultiplayerManager::centerPrintTeamClients( Player *player, const str &string, CenterPrintImportance importance )
{
	int i;
	gentity_t *ent;
	Player *otherPlayer;

	// Go through all of the clients

	for( i = 0; i < maxclients->integer; i++ )
	{
		ent = g_entities + i;

		// Make sure this is a valid client

		if ( !ent->inuse || !ent->client || !ent->entity || !ent->entity->isSubclassOf( Player ) || ent->svflags & SVF_BOT) //[b607] chrissstrahl - don't print for bots
			continue;

		otherPlayer = (Player *)ent->entity;

		// Make sure this player is on the same team

		if ( getPlayersTeam( player ) == getPlayersTeam( otherPlayer ) )
		{
			// Send centerprint command to the client

			centerPrint( otherPlayer->entnum, string, importance );
		}
	}
}

void MultiplayerManager::HUDPrintTeamClients( Player *player, const str &string )
{
	int i;
	gentity_t *ent;
	Player *otherPlayer;

	// Go through all of the clients

	for( i = 0; i < maxclients->integer; i++ )
	{
		ent = g_entities + i;

		// Make sure this is a valid client


		if ( !ent->inuse || !ent->client || !ent->entity || !ent->entity->isSubclassOf( Player ) || ent->svflags & SVF_BOT) //[b607] chrissstrahl - don't print for bots
			continue;

		otherPlayer = (Player *)ent->entity;

		// Make sure this player is on the same team

		if ( getPlayersTeam( player ) == getPlayersTeam( otherPlayer ) )
		{
			// Send the hud print command to the client

			HUDSay( otherPlayer->entnum, string );
		}
	}
}

void MultiplayerManager::HUDPrintSpectators( Player *player, const str &string, bool team )
{
	int i;
	gentity_t *ent;
	Player *otherPlayer;

	// Go through all of the clients

	for( i = 0 ; i < maxclients->integer ; i++ )
	{
		ent = g_entities + i;

		// Make sure this is a valid client

		if ( !ent->inuse || !ent->client || !ent->entity || !ent->entity->isSubclassOf( Player ) || ent->svflags & SVF_BOT) //[b607] chrissstrahl - don't print for bots
			continue;

		otherPlayer = (Player *)ent->entity;

		// Make sure this player is a spectator

		if ( !isPlayerSpectator( otherPlayer ) )
			continue;

		// Make sure the players are on the same team (if requested)

		if ( team && ( getPlayersTeam( player ) != getPlayersTeam( otherPlayer ) ) )
			continue;

		// Send the hud print command to the client

		HUDSay( otherPlayer->entnum, string );
	}
}

void MultiplayerManager::addPlayerHealth( int entnum, float healthToAdd ) 
{
	Player *player;

	player = getPlayer( entnum );

	if ( player )
	{
		player->AddHealth( healthToAdd );
	}
}

void MultiplayerManager::givePlayerItem( int entnum, const str &itemName )
{
	Player *player;
	Event *event;
	bool canGive;


	if ( !_inMatch )
		return;

	player = getPlayer( entnum );

	if ( !player )
		return;

	canGive = canGivePlayerItem( entnum, itemName );

	if ( canGive )
	{
		event = new Event( EV_Player_GiveCheat );
		event->AddString( itemName );
		player->ProcessEvent( event );
	}
}

void MultiplayerManager::usePlayerItem( int entnum, const str &itemName )
{
	Player *player;

	player = getPlayer( entnum );

	if ( !player )
		return;

	// Use the specified item

	Event *ev = new Event( "use" );
	ev->AddString( itemName );
	player->ProcessEvent( ev );
}

bool MultiplayerManager::canGivePlayerItem( int entnum, const str &itemName )
{
	int i;
	Player *player;

	player = getPlayer( entnum );

	if ( !player )
		return false;

	if ( player->FindItem( itemName ) )
		return false;

	// Make sure all of the modifiers allow this item to be given to the player

	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			if ( !modifier->canGivePlayerItem( entnum, itemName ) )
				return false;
		}
	}

	// Make sure the game allows this item to be given to the player

	if ( !_multiplayerGame->canGivePlayerItem( entnum, itemName ) )
		return false;

	return true;
}

float MultiplayerManager::getItemRespawnMultiplayer( void )
{
	if ( !_inMultiplayerGame )
		return 1.0f;
	else
		return mp_itemRespawnMultiplier->value;
}

float MultiplayerManager::getWeaponRespawnMultiplayer( void )
{
	if ( !_inMultiplayerGame )
		return 1.0f;
	else
		return mp_weaponRespawnMultiplier->value;
}

float MultiplayerManager::getPowerupRespawnMultiplayer( void )
{
	if ( !_inMultiplayerGame )
		return 1.0f;
	else
		return mp_powerupRespawnMultiplier->value;
}

void MultiplayerManager::playerEventNotification( const char *eventName, const char *eventItemName, Player *eventPlayer )
{
	int i;
	gentity_t *ent;
	Player *player;

	if ( !_inMultiplayerGame )
		return;

	// Notify all of the modifiers about this event

	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
			modifier->playerEventNotification( eventName, eventItemName, eventPlayer );
	}

	// Notify the award system about this event

	_awardSystem->playerEventNotification( eventName, eventItemName, eventPlayer );

	// Notify all the player's about the event

	for( i = 0; i < maxclients->integer; i++ )
	{
		ent = g_entities + i;

		// Make sure this is a valid client

		if ( !ent->inuse || !ent->client || !ent->entity || !ent->entity->isSubclassOf( Player ) )
			continue;

		player = (Player *)ent->entity;

		player->notifyPlayerOfMultiplayerEvent( eventName, eventItemName, eventPlayer );
	}

	// Notify the game about the event

	_multiplayerGame->playerEventNotification( eventName, eventItemName, eventPlayer );
}

void MultiplayerManager::startMatch( void )
{
	int i;

	_inMatch = true;

	// Inform all of the modifiers that the match is starting
	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			modifier->matchStarting();
		}
	}
	// Inform the game that the match has started

	_multiplayerGame->startMatch();

	// Inform all of the modifiers that the match has started
	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			modifier->matchStarted();
		}
	}
}

void MultiplayerManager::restartMatch( void )
{
	// Inform the game that the match has ended

	_multiplayerGame->endMatch();

	// Inform all of the modifiers that the match has started
	for ( int i = 1; i <= _modifiers.NumObjects(); i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			modifier->matchEnded();
		}
	}

	resetItems();
	
	// Inform the game that the match has restarted

	_multiplayerGame->restartMatch();

	// Inform all of the modifiers that the match has started
	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			modifier->matchRestarted();
		}
	}

	_inMatch = true;
}

void MultiplayerManager::endMatch( void )
{
	_inMatch = false;

	_restartMatchTime = getTime() + _inBetweenMatchTime;
}

void MultiplayerManager::initPlayer( Player *player )
{
	Vector savedAngles;
	Vector savedViewAngles;
	qboolean saved_teleport_bit;

	// Cancel all events that have been posted for the future

	player->CancelPendingEvents();

	// Save off necessary stuff

	saved_teleport_bit = player->edict->s.eFlags & EF_TELEPORT_BIT;
	savedAngles = player->client->cmd_angles;
	savedViewAngles = player->getViewAngles();

	// Initialize the player

	player->Init();

	// Restore necessary stuff

	player->edict->s.eFlags |= saved_teleport_bit;
	savedAngles.copyTo( player->client->cmd_angles );
	player->SetViewAngles( savedViewAngles );
}

Player *MultiplayerManager::getLastKilledByPlayer( Player *player, int *meansOfdeath )
{
	return _multiplayerGame->getLastKilledByPlayer( player, meansOfdeath );
}

Player *MultiplayerManager::getLastKillerOfPlayer( Player *player, int *meansOfdeath )
{
	return _multiplayerGame->getLastKillerOfPlayer( player, meansOfdeath );
}

void MultiplayerManager::playerCommand( Player *player, const char *command, const char *parm )
{
	if ( !_inMultiplayerGame )
		return;

	// Pass the player command to all of the modifiers

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			modifier->playerCommand( player, command, parm );
		}
	}

	// Pass the player command to the game

	_multiplayerGame->playerCommand( player, command, parm );
}

void MultiplayerManager::playerInput( Player *player, int newButtons )
{
	if ( isPlayerSpectator( player ) )
	{
		if ( newButtons & BUTTON_USE )
		{
			// Change spectator type

			if ( isPlayerSpectator( player, SPECTATOR_TYPE_FOLLOW ) )
				makePlayerSpectator( player, SPECTATOR_TYPE_NORMAL );
			//hzm coop mod chrissstrahl - do not allow free form on story/coop maps
			else if ( isPlayerSpectator( player, SPECTATOR_TYPE_NORMAL ) && game.coop_isActive == false )
				makePlayerSpectator( player, SPECTATOR_TYPE_FREEFORM );
			else 
				makePlayerSpectator( player, SPECTATOR_TYPE_FOLLOW );
		}

		if ( isPlayerSpectator( player, SPECTATOR_TYPE_FOLLOW ) && ( _playerData[ player->entnum ]._spectatorTime != getTime() ) )
		{
			if ( newButtons & BUTTON_ATTACKRIGHT )
			{
				// Follow the next player 

				makePlayerSpectateNextPlayer( player );
			}
		
			if ( newButtons & BUTTON_ATTACKLEFT )
			{
				// Follow the prev player 

				makePlayerSpectatePrevPlayer( player );
			}
		}
	}
}

int MultiplayerManager::getInfoIcon( Player *player, int buttons )
{
	int icon;

	// See if the play is talking

	if ( buttons & BUTTON_TALK )
		return _talkingIconIndex;

	// See if the game has anything

	icon = _multiplayerGame->getInfoIcon( player );

	if ( icon > 0 )
		return icon;

	// See if the award system has anything

	icon = _awardSystem->getInfoIcon( player );

	if ( icon > 0 )
		return icon;

	// See if any modifiers have anything

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			icon = modifier->getInfoIcon( player );

			if ( icon > 0 )
				return icon;
		}
	}

	// No one has anything to display

	return 0;
}

void MultiplayerManager::teamPointsChanged( Team *team, int oldPoints, int newPoints )
{
	if ( !_inMultiplayerGame )
		return;

	// Tell the game that the team points have changed

	_multiplayerGame->teamPointsChanged( team, oldPoints, newPoints );
}

void MultiplayerManager::cacheMultiplayerFiles( const str &cacheFileName )
{
	str fileToCache;

	// Build the name of the file to cache

	fileToCache = "precache/server/";
	fileToCache += cacheFileName;
	fileToCache += ".txt";

	// Cache everything in the file (if it is there)

	if ( gi.FS_ReadFile( fileToCache, NULL, true ) != -1 )
	{
		level.consoleThread->Parse( fileToCache );
	}
}

str MultiplayerManager::getPlaceName( Player *player )
{
	int place;
	str placeName;

	place = _multiplayerGame->getPlace( player );

	placeName = "$$Place";
	placeName += place;
	placeName += "$$";

	return placeName;
}

void MultiplayerManager::setTeamHud( Player *player, const str &teamHudName )
{
	str command;

//hzm coop mod chrissstrahl - no teamhuds in coop
	if ( teamHudName == _playerData[ player->entnum ]._teamHud || game.coop_isActive )
		return;

	// Remove the old team hud

	if ( _playerData[ player->entnum ]._teamHud.length() > 0 )
	{
		command = va( "stufftext \"ui_removehud %s\"\n", _playerData[ player->entnum ]._teamHud.c_str() );
		gi.SendServerCommand( player->entnum, command.c_str() );
	}

	// Add the new team hud

	if ( teamHudName.length() > 0 )
	{
		command = va( "stufftext \"ui_addhud %s\"\n", teamHudName.c_str() );
		gi.SendServerCommand( player->entnum, command.c_str() );
	}

	_playerData[ player->entnum ]._teamHud = teamHudName;
}

str MultiplayerManager::getSpawnPointType( Player *player )
{
	str spawnPointName;
	MultiplayerModifier *modifier;
	int i;
	float priority;
	float highestPriority = 0.0f;

	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			priority = modifier->getSpawnPointPriority( player );

			if ( priority > highestPriority )
			{
				spawnPointName = modifier->getSpawnPointType( player );

				highestPriority = priority;
			}
		}
	}

	return spawnPointName;
}

bool MultiplayerManager::isValidPlayerModel( Player *player, str modelToUse )
{
	MultiplayerModifier *modifier;
	int i;
	bool validPlayerModel = false;
	int modelIndex;
	tiki_cmd_t tikicmds;


	// Check to see if the model itself is ok first

	modelIndex = gi.modelindex( modelToUse );

	if ( gi.InitCommands( modelIndex, &tikicmds ) )
	{
		for( i = 0; i < tikicmds.num_cmds; i++ )
		{
			if ( stricmp( tikicmds.cmds[ i ].args[ 0 ], "validPlayerModel" ) == 0 )
			{
				validPlayerModel = true;
				break;
			}
		}
	}

	// Check with the game to see if the model is ok

	validPlayerModel = _multiplayerGame->isValidPlayerModel( player, modelToUse, validPlayerModel );

	// Check with all of the modifiers to see if the model is ok

	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			validPlayerModel = modifier->isValidPlayerModel( player, modelToUse, validPlayerModel );
		}
	}

//hzm cooop mod chrissstrahl - allow only specific models in coop
	if ( game.coop_isActive )
	{
		validPlayerModel = coop_modelValidPlayer( modelToUse );
	}

	return validPlayerModel;
}

str MultiplayerManager::getDefaultPlayerModel( Player *player )
{
	MultiplayerModifier *modifier;
	int i;
	str modelName;


	modelName = _multiplayerGame->getDefaultPlayerModel( player, "models/char/munro.tik" );

	for ( i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		modifier = _modifiers.ObjectAt( i );

		if ( modifier )
		{
			modelName = modifier->getDefaultPlayerModel( player, modelName );
		}
	}

	return modelName;
}

void MultiplayerManager::setNextMap( void )
{
	str nextMapName;
	str fullMapName;

	if ( ( strlen( sv_nextmap->string ) == 0 ) && ( mp_useMapList->integer ) && ( strlen( mp_mapList->string ) > 0 ) )
	{
		nextMapName = getNextMap();

		fullMapName = "maps/";
		fullMapName += nextMapName;
		fullMapName += ".bsp";

		if ( gi.FS_Exists( fullMapName ) )
		{
			gi.cvar_set( "nextmap", nextMapName.c_str() );
		}
		else
		{
			gi.Printf( "%s map not found\n", fullMapName.c_str() );
		}

		gi.cvar_set( "mp_currentPosInMapList", va( "%d", mp_currentPosInMapList->integer + 1 ) );
	}
}

str MultiplayerManager::getNextMap( void )
{
	str nextMapName;
	str mapList;
	int numMaps;
	const char *currentPlaceInMapList;
	int realCurrentPos;
	int i;
	str tempString;
	const char *nextSpace;
	int diff;


	mapList = mp_mapList->string;

	// Get the number of maps in the list

	numMaps = 0;
	currentPlaceInMapList = mapList.c_str();

	while( 1 )
	{
		currentPlaceInMapList = strstr( currentPlaceInMapList, ";" );

		if ( currentPlaceInMapList )
			numMaps++;
		else
			break;

		currentPlaceInMapList++;
	}

	if ( mapList.length() && ( mapList[ mapList.length() - 1 ] != ';' ) )
		numMaps++;

	if ( numMaps == 0 )
		return "";

	// Get the position in the list

	realCurrentPos = mp_currentPosInMapList->integer % numMaps;

	// Get the next map string

	currentPlaceInMapList = mapList.c_str();

	for ( i = 0 ; i < realCurrentPos ; i++ )
	{
		currentPlaceInMapList = strstr( currentPlaceInMapList, ";" );
		currentPlaceInMapList++;
	}

	nextMapName = currentPlaceInMapList;

	currentPlaceInMapList = nextMapName.c_str();

	currentPlaceInMapList = strstr( currentPlaceInMapList, ";" );

	if ( currentPlaceInMapList )
	{
		nextMapName.CapLength( currentPlaceInMapList - nextMapName.c_str() );
	}

	// Remove spaces from the beginning of the map name

	while ( ( nextMapName.length() > 0 ) && ( nextMapName[ 0 ] == ' ' ) )
	{
		tempString = nextMapName.c_str() + 1;
		nextMapName = tempString;
	}

	// Remove spaces from the end of the map name

	nextSpace = strstr( nextMapName.c_str(), " " );

	if ( nextSpace )
	{
		diff = nextSpace - nextMapName.c_str();
		nextMapName.CapLength( diff );
	}

	return nextMapName;
}

float MultiplayerManager::getRespawnTime( void )
{
	return _respawnTime;
}

void MultiplayerManager::setRespawnTime( float time )
{
	_respawnTime = time;
}

void MultiplayerManager::resetRespawnTime( void )
{
	setRespawnTime( mp_respawnTime->value );
	mp_respawnTime->modified = false;
}

void MultiplayerManager::checkModifiedCvars( bool informPlayers )
{
	// Inform players about cvar changes (if any )

	if ( informPlayers ){
		if ( hasFlagChanged( MP_FLAG_WEAPONS_STAY ) )
			checkCvar( mp_flags, "$$WeaponsStay$$", MP_CVAR_TYPE_BOOL, MP_FLAG_WEAPONS_STAY );
		if ( hasFlagChanged( MP_FLAG_FRIENDLY_FIRE ) )
			checkCvar( mp_flags, "$$AllowFriendlyFire$$", MP_CVAR_TYPE_BOOL, MP_FLAG_FRIENDLY_FIRE );
		if ( hasFlagChanged( MP_FLAG_FORCE_RESPAWN ) )
			checkCvar( mp_flags, "$$ForceRespawn$$", MP_CVAR_TYPE_BOOL, MP_FLAG_FORCE_RESPAWN );
		if ( hasFlagChanged( MP_FLAG_INFINITE_AMMO ) )
			checkCvar( mp_flags, "$$InfiniteAmmo$$", MP_CVAR_TYPE_BOOL, MP_FLAG_INFINITE_AMMO );
		if ( hasFlagChanged( MP_FLAG_FIXED_FOV ) )
			checkCvar( mp_flags, "$$FixedFOV$$", MP_CVAR_TYPE_BOOL, MP_FLAG_FIXED_FOV );
		if ( hasFlagChanged( MP_FLAG_NO_FOOTSTEPS ) )
			checkCvar( mp_flags, "$$NoFootstepSounds$$", MP_CVAR_TYPE_BOOL, MP_FLAG_NO_FOOTSTEPS );
		if ( hasFlagChanged( MP_FLAG_DONT_ALLOW_VOTE ) )
			checkCvar( mp_flags, "$$DontAllowVoting$$", MP_CVAR_TYPE_BOOL, MP_FLAG_DONT_ALLOW_VOTE );
		if ( hasFlagChanged( MP_FLAG_FULL_COLLISION ) )
			checkCvar( mp_flags, "$$FullCollision$$", MP_CVAR_TYPE_BOOL, MP_FLAG_FULL_COLLISION );

		//[b607] chrissstrahl - fused codeblocks and added additional troublemakers
		//hzm coop mod  chrissstrahl - do not check for these cvars as they are suppose to be dissabled during coop
		if ( !game.coop_isActive ){
			if ( hasFlagChanged( MP_FLAG_NO_FALLING ) )
				checkCvar( mp_flags , "$$NoFalling$$" , MP_CVAR_TYPE_BOOL , MP_FLAG_NO_FALLING );
			if ( hasFlagChanged( MP_FLAG_NO_DROP_WEAPONS ) )
				checkCvar( mp_flags, "$$DontDropWeapons$$", MP_CVAR_TYPE_BOOL, MP_FLAG_NO_DROP_WEAPONS );

			checkCvar( mp_pointlimit , "$$PointLimit$$" , MP_CVAR_TYPE_INTEGER );
			checkCvar( mp_timelimit , "$$TimeLimit$$" , MP_CVAR_TYPE_INTEGER );
			checkCvar( mp_bigGunMode, "$$OptionBigGunMode$$", MP_CVAR_TYPE_INTEGER );
			checkCvar( mp_respawnTime, "$$OptionRespawnTime$$", MP_CVAR_TYPE_INTEGER );
			checkCvar( sv_maxspeed, "$$PlayerSpeed$$", MP_CVAR_TYPE_INTEGER );	
			checkCvar( mp_bombTime, "$$OptionBombTime$$", MP_CVAR_TYPE_INTEGER);
		}
		else{
		//hzm gameupdate chrissstrahl - check if skill has been changed
			checkCvar( skill, "skill", MP_CVAR_TYPE_INTEGER );
		}

		checkCvar( mp_itemRespawnMultiplier, "$$ItemRespawnMultiplier$$", MP_CVAR_TYPE_FLOAT );
		checkCvar( mp_weaponRespawnMultiplier, "$$WeaponRespawnMultiplier$$", MP_CVAR_TYPE_FLOAT );
		checkCvar( mp_powerupRespawnMultiplier, "$$PowerupRespawnMultiplier$$", MP_CVAR_TYPE_FLOAT );
		checkCvar( mp_knockbackMultiplier, "$$KnocbackMultiplier$$", MP_CVAR_TYPE_INTEGER );
		checkCvar( mp_damageMultiplier, "$$DamageMultiplier$$", MP_CVAR_TYPE_FLOAT );
		checkCvar( mp_respawnInvincibilityTime, "$$RespawnInvincibilityTime$$", MP_CVAR_TYPE_FLOAT );
		checkCvar( mp_warmUpTime, "$$WarmUpTime$$", MP_CVAR_TYPE_INTEGER );
		checkCvar( mp_maxVotes, "$$OptionMaxVotes$$", MP_CVAR_TYPE_INTEGER );
	}

	// Mark everything as not modified

//hzm gamefix chrissstrahl - make skill work on the fly
	skill->modified = false;

	mp_flags->modified = false;
	mp_pointlimit->modified = false;
	mp_timelimit->modified = false;
	mp_itemRespawnMultiplier->modified = false;
	mp_weaponRespawnMultiplier->modified = false;
	mp_powerupRespawnMultiplier->modified = false;
	mp_knockbackMultiplier->modified = false;
	mp_damageMultiplier->modified = false;
	mp_respawnInvincibilityTime->modified = false;
	mp_warmUpTime->modified = false;
	sv_maxspeed->modified = false;
	mp_bigGunMode->modified = false;
	mp_respawnTime->modified = false;
	mp_bombTime->modified = false;
	mp_maxVotes->modified = false;

	// Save off the flags

	_oldFlags = mp_flags->integer;
}


void MultiplayerManager::checkCvar( cvar_t *mp_cvarToCheck, str optionName, MPCvarType cvarType, int bitToCheck )
{
	str stringToPrint;

	if ( mp_cvarToCheck->modified )
	{
		stringToPrint = "$$ServerOptionChanged$$ : ";
		stringToPrint += optionName;
		stringToPrint += " ";

		if ( cvarType == MP_CVAR_TYPE_INTEGER ){
			//[b607] chrissstrahl - disable this code, I guess we don't need this anymore
			/*
			//hzm coop mod chrissstrahl - set new movement speed on physics
			if ( game.coop_isActive ){
				optionName = optionName.tolower();
				/* 
				//hzm coop mod chrissstrahl - mp_warmuptime must not be under 1 or it will brake the coop mod
				//I didn't have time to investigate why, so this is a cheap fix
				if ( !Q_stricmp( optionName , "mp_warmuptime") ){
					if ( mp_cvarToCheck->integer < 1 ){ mp_cvarToCheck->integer = 1; }
					stringToPrint += mp_cvarToCheck->integer;
				}
				//hzm coop mod chrissstrahl - make sure it still displays the values it was changed to
				else{
					stringToPrint += mp_cvarToCheck->integer;
				}
				stringToPrint += mp_cvarToCheck->integer;
			}
			else{
			}*/

			//[b607] chrissstrahl - fix the timelimit not changing when the cvar is changed
			optionName = optionName.tolower();
			if (!Q_stricmp(optionName.c_str(), "$$timelimit$$")) {
				_multiplayerGame->setTimeLimit(mp_timelimit->integer * 60.0f);
			}
			//[b607] chrissstrahl - fix the pointlimit not changing when the cvar is changed
			else if (!Q_stricmp(optionName.c_str(), "$$pointlimit$$")) {
				_multiplayerGame->setPointLimit(mp_pointlimit->integer);
			}
			stringToPrint += mp_cvarToCheck->integer;
		}
		else if ( cvarType == MP_CVAR_TYPE_FLOAT ){
			stringToPrint += mp_cvarToCheck->value;
		}
		else if ( cvarType == MP_CVAR_TYPE_BOOL ){
			int value;

			if ( bitToCheck >= 0 ){
				value = mp_cvarToCheck->integer & bitToCheck;
			}
			else{
				value = mp_cvarToCheck->integer;
			}

			if ( value == 0 )
				stringToPrint += "$$Off$$";
			else
				stringToPrint += "$$On$$";
		}
		stringToPrint += "\n";
		multiplayerManager.HUDPrintAllClients( stringToPrint );
	}
}

bool MultiplayerManager::hasFlagChanged( int bitToCheck )
{
	if ( ( _oldFlags & bitToCheck ) != ( mp_flags->integer & bitToCheck ) )
		return true;
	else
		return false;
}

void MultiplayerManager::addSoundToQueue( Player *player, str soundName, int channel, float volume, float minDist, float time )
{
	MultiplayerDialogData * dialogData;
	int nextDialogAddSpot;
	MultiplayerPlayerData * playerData;


	playerData = &_playerData[ player->entnum ];

	nextDialogAddSpot = playerData->getNextDialogAddSpot();

	dialogData = &playerData->_dialogData[ nextDialogAddSpot ];

	// Add the dialog to the queue

	dialogData->_soundName = soundName;
	dialogData->_channel   = channel;
	dialogData->_volume    = volume;
	dialogData->_minDist   = minDist;
	dialogData->_time      = time;

	// Move to the next spot

	playerData->_nextDialogAddSpot++;
}

void MultiplayerManager::sendNextPlayerSound( Player *player )
{
	MultiplayerDialogData * dialogData;
	MultiplayerPlayerData * playerData;
	int nextDialogSendSpot;

	playerData = &_playerData[ player->entnum ];

	// Make sure we haven't sent anything too recently

	if ( playerData->_nextDialogSendTime > multiplayerManager.getTime() )
		return;

	// Make sure we have something to send

	if ( playerData->_nextDialogSendSpot >= playerData->_nextDialogAddSpot )
		return;

	// Send the sound

	nextDialogSendSpot = playerData->getNextDialogSendSpot();

	dialogData = &playerData->_dialogData[ nextDialogSendSpot ];

	player->Sound( dialogData->_soundName, dialogData->_channel, dialogData->_volume, dialogData->_minDist, NULL, 1.0f, true );

	// Move to the next slot

	playerData->_nextDialogSendSpot++;

	// Don't send another sound for a little while

	playerData->_nextDialogSendTime = multiplayerManager.getTime() + dialogData->_time;
}

bool MultiplayerManager::skipWeaponReloads( void )
{
//hzm coop mod chrissstrahl - do not do this in coop, it spoils the feeling
	if ( !_inMultiplayerGame || game.coop_isActive )
		return false;

	if ( mp_skipWeaponReloads->integer )
		return true;

	if ( _multiplayerGame->skipWeaponReloads() )
		return true;

	for ( int i = 1 ; i <= _modifiers.NumObjects() ; i++ )
	{
		MultiplayerModifier *modifier;

		modifier = _modifiers.ObjectAt( i );


		if ( modifier )
		{
			if ( modifier->skipWeaponReloads() )
				return true;
		}
	}

	return false;
}

//hzm coop mod daggolin - we need access to the frags (_numKills)
void MultiplayerManager::addKills( int entnum, int kills )
{
	if ( !_inMultiplayerGame )
		return;

	_multiplayerGame->addKills( entnum, kills );
}


//hzm gameupdate chrissstrahl - add new func to reset vote count - used to reset after cinematics
void MultiplayerManager::resetVoteCount( Player *player )
{
	if ( player && _inMultiplayerGame )//if we call this in singleplayer the game will freeze and eventually crash
	{
		_playerData[player->entnum]._votecount = 0;
	}
}


