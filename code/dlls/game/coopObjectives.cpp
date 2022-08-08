//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING OBJECTIVES RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]


#include "_pch_cpp.h"

#include "coopCheck.hpp"
#include "coopReturn.hpp"
#include "coopHuds.hpp"
#include "coopPlayer.hpp"
#include "coopClass.hpp"
#include "coopArmory.hpp"
#include "coopObjectives.hpp"
#include "coopText.hpp"
#include "coopStory.hpp"

#include "level.h"
#include "player.h"
#include "weapon.h"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include "powerups.h"

//================================================================
// Name:        coop_objectivesMarkerUpdate
// Class:       -
//              
// Description: updates the objective marker entities to sync with their master
//              
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void coop_objectivesMarkerUpdate( void )
{
	int i;
	Entity *eEntity = NULL;
	Entity *eMarker = NULL;
	for ( i = 0; i < globals.max_entities; i++ ){
		eEntity = G_GetEntity( i );
		if ( eEntity && eEntity->myCoopMarker ){

			eMarker = eEntity->myCoopMarker;
			float fScale = 0.05;

			if ( !level.cinematic ){
				//hzm coop mod chrissstrahl - manaual bind to real objective, has many advantages (will not be hidden or scaled)
				if ( eMarker->origin != eEntity->origin ){
					eMarker->setOrigin( eEntity->origin );
				}

				//hzm coop mod chrissstrahl - scale our marker relative to the original marker
				int playerNum;
				
				Entity *player;
				Vector vCalc;
				float fLength = 999999;
				for ( playerNum = 0; playerNum < maxclients->integer; playerNum++ ){
					player = g_entities[playerNum].entity;
					if ( player && player->client && player->isSubclassOf( Player ) ){
						vCalc = ( ( player->origin ) - ( eMarker->origin ) );

						if ( vCalc.length() < fLength ){
							fLength = vCalc.length();
						}
					}
				}

				if ( fLength > 12000.0f ){
					fLength = 12000.0f;
				}

				fLength = ( fLength / 3000 );
				fScale += fLength;
				//eMarker->edict->s.scale
			}
			else{ //make the markers very very small, avoid them to be visible during cinematic sequences
				fScale = 0.005;
			}
			

			eMarker->setScale( fScale );
		}
	}
}

//================================================================
// Name:        coop_objectivesMarker
// Class:       -
//              
// Description: spawns marker for given objectives entity
//              
// Parameters:  Entity *eMaster
//              
// Returns:     void
//              
//================================================================
void coop_objectivesMarker( Entity *eMaster )
{

	if ( !game.coop_isActive|| g_gametype->integer == GT_SINGLE_PLAYER )return;

	Entity		   *entMiObj;
	Entity		   *entSpawn;

	entMiObj = G_GetEntity( eMaster->entnum );

	// create a new entity
	SpawnArgs args;
	args.setArg( "classname" , "ScriptModel" );
	//args.setArg( "model" , "sysimg/icons/mp/team_red.spr" );
	//args.setArg( "model" , "sysimg/icons/mp/actionhero_icon.spr" );
	args.setArg( "model" , "models/hud/radar_ground-plane.tik" );
	args.setArg( "origin" , va( "%f %f %f" , entMiObj->origin[0] , entMiObj->origin[1] , entMiObj->origin[2] ) );
	args.setArg( "scale" , "0.25" );
	args.setArg( "angle" , "-1" );
	args.setArg( "notsolid" , "1" );
	args.setArg( "rotateZ" , "200" );
	//args.setArg( "rotateX" , "999" );
	args.setArg( "rendereffects" , "+depthhack" );
	args.setArg( "targetname" , va("%s_objMarker" , entMiObj->targetname.c_str()) );//use original name plus additional
	entSpawn = args.Spawn();

	//hzm coop mod chrissstrahl - used to store the entity that marks this missionObjective
	eMaster->myCoopMarker = entSpawn;
}

//================================================================
// Name:        coop_objectivesMarkerRemove
// Class:       -
//              
// Description: removes marker for given objectives entity
//              
// Parameters:  Entity *eMaster
//              
// Returns:     void
//              
//================================================================
void coop_objectivesMarkerRemove( Entity *eMaster )
{
	if ( !eMaster )return;

	//hzm coop mod chrissstrahl - remove the mission objective visual marker as well
	if ( eMaster->myCoopMarker != NULL ){
		Entity *eMarker = eMaster->myCoopMarker;
		G_RemoveEntityFromExtraList( eMarker->entnum );
		eMarker->PostEvent( EV_Remove , 0.0f );
	}
}


//================================================================
// Name:        coop_objectivesSetup
// Class:       -
//              
// Description: setup the objectives for given player
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_objectivesSetup( Player *player)
{
	if ( !player || !player->coopPlayer.installed ){
		return;
	}

	//[b607] chrissstrahl - make sure we do not handle bots
	gentity_t *ent = player->edict;
	if (ent->svflags & SVF_BOT) {
		return;
	}

	int i;
	str sCvar;
//reset objectives and tactical info
	for ( i = 1; i < 12; i++ ){
		sCvar = coop_returnObjectivesCvarName( i );
		if ( i < 9){
			DelayedServerCommand( player->entnum , va( "set %s_s 0" , sCvar.c_str() ) );
		}else{
			DelayedServerCommand( player->entnum , va( "set %s ^0" , sCvar.c_str() ) );
		}
	}

	//[b610] chrissstrahl - moved it here
	//standard maps always have ritual entertainment as author
	if (game.isStandardLevel) {
		game.coop_author = "Ritual Entertainment";
	}
	else {
		if (game.coop_author.length() < 1) {
			game.coop_author = program.getStringVariableValue( "coop_string_levelAuthor" );
		}
		if ( game.coop_author.length() < 1 ){
			game.coop_author = "$$Empty$$";
		}
	}
	game.coop_author = coop_replaceForLabelText(game.coop_author);
	DelayedServerCommand( player->entnum , va( "globalwidgetcommand coop_objectivesMapAuthor labeltext %s" , game.coop_author.c_str() ) );
	
	//hzm coop mod chrissstrahl - set story right away, need to do this differently in mp see coop_playerSay
	coop_storySet( player );
}


//================================================================
// Name:        coop_objectivesUpdateUservar
// Class:       -
//              
// Description: called when a uservar is set and variables have to be updated 
//              
// Parameters:  int iUservar
//              
// Returns:     void
//              
//================================================================
void coop_objectivesUpdateUservar( int iUservar )
{
	if ( level.mission_failed )
		return;

	int i;
	gentity_t *gentity;
	Player *player = NULL;

	//hzm coop mod chrissstrahl - return in sp directly
	if ( g_gametype->integer == GT_SINGLE_PLAYER ){
		player = ( Player * )g_entities[0].entity;
		if ( player ){
			if ( iUservar == 1){
				DelayedServerCommand( player->entnum , va( "globalwidgetcommand coop_objectivesMapAuthor title %s" , game.coop_author.c_str() ) );
			}else if ( iUservar == 2){
				DelayedServerCommand( player->entnum , va( "set coop_story %s" , game.coop_story.c_str() ) );
			}else if ( iUservar == 3 ){
				if ( coop_checkPlayerLanguageGerman(player) ){
					DelayedServerCommand( player->entnum , va( "set coop_story %s" , game.coop_story_deu.c_str() ) );
				}
			}
		}
		return;
	}

	gi.Printf( va( "Uservar updated on world, uservar%d\n" , iUservar ) );

	for ( i = 0; i < maxclients->integer; i++ ){
		gentity = &g_entities[i];
		if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) ){
			player = ( Player * )gentity->entity;
			if ( player && player->coopPlayer.installed ){
				if ( iUservar == 1){
					DelayedServerCommand( player->entnum , va( "globalwidgetcommand coop_objectivesMapAuthor title %s" , game.coop_author.c_str() ) );
				}
				else if ( iUservar == 2 ){
					DelayedServerCommand( player->entnum , va( "set coop_story %s" , game.coop_story.c_str() ) );
				}
				else if ( iUservar == 3 ){
					if ( coop_checkPlayerLanguageGerman(player) ){
						DelayedServerCommand( player->entnum , va( "set coop_story %s" , game.coop_story_deu.c_str() ) );
					}
				}
			}
		}
	}
}

//================================================================
// Name:        coop_objectivesNotify
// Class:       -
//              
// Description: INDICATE HUD AND PLAY NOTIFICATION SOUND
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_objectivesNotify( Player* player )
{
	if ( !player || level.mission_failed )
		return;

	if ( game.coop_objectiveItemState[0] == -1 && game.coop_objectiveItemState[1] == -1 &&
		game.coop_objectiveItemState[2] == -1 && game.coop_objectiveItemState[3] == -1 &&
		game.coop_objectiveItemState[4] == -1 && game.coop_objectiveItemState[5] == -1 &&
		game.coop_objectiveItemState[6] == -1 && game.coop_objectiveItemState[7] == -1 )
	{
		return;
	}

	coop_hudsAdd( player , "objectivenotifytext" );
	player->Sound( "snd_objectivechanged" , CHAN_LOCAL );
	coop_hudsAdd( player , "coop_notify" );//SYMBOL->(|!|)
}

//================================================================
// Name:        coop_objectivesUpdatePlayer
// Class:       -
//              
// Description: Checks if a objective should be send to the player
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_objectivesUpdatePlayer( Player* player )
{
	if ( !player || level.mission_failed )
		return;

	if ( player->coopPlayer.setupComplete ){
		if ( player->coopPlayer.lastTimeUpdatedObjectives != game.coop_objectiveLastUpdate || ( player->coopPlayer.lastTimeSpawned + 1.0f ) > level.time ){
			player->coopPlayer.lastTimeUpdatedObjectives = game.coop_objectiveLastUpdate;

			coop_objectivesNotify( player );
			
			int i , iState;
			for ( i = 9; i < 12; i++ ){
				coop_objectivesShow( player , i , 0 , true );//was false
			}
			for ( i = 1; i < 9; i++ ){
				iState = game.coop_objectiveItemState[( i - 1 )];
				//gi.Printf( va( "status: [%i] -> %i\n", i, iState ) );
				if ( iState != -1 ){//state set, means this objective is being used
					coop_objectivesShow( player , i , iState , true );
				}
			}
		}
	}
}


//================================================================
// Name:        coop_objectivesUpdate
// Class:       -
//              
// Description: UPDATES A OBJECTIVE, IN RELATION TO COOP LEVEL SCRIPT
//              
// Parameters:  str sObjectiveState , float fObjectiveItem , float fObjectiveShow
//              
// Returns:     void
//              
//================================================================
void coop_objectivesUpdate( str sObjectiveState , float fObjectiveItem , float fObjectiveShow )
{
	gi.Printf( "OBJECTIVE: %s , %d , %d \n" , sObjectiveState.c_str() , fObjectiveItem , fObjectiveShow );
}


//================================================================
// Name:        coop_objectivesSingleplayer
// Class:       -
//              
// Description: MANAGES OBJECTIVES FOR SINGLEPLAYER MODE
//              
// Parameters:  str sObjectiveState , int iObjectiveNumber , bool bShowNow
//              
// Returns:     void
//              
//================================================================
void coop_objectivesSingleplayer( str sObjectiveState , int iObjectiveNumber , int iShowNow )
{
	gentity_t *gentity;
	Player *player = NULL;
	gentity = &g_entities[0];
	if ( gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf( Player ) ){
		player = ( Player * )gentity->entity;
	}
	else{
		return;
	}

	qboolean Show = iShowNow;
	str ObjName;
	ObjName = program.getStringVariableValue( va( "coop_string_objectiveItem%i" , iObjectiveNumber ) );
	if ( ObjName.length() < 1 ){
		gi.Printf( "coop_objectivesSingleplayer: objective name was empty\n" );
		return;
	}

	gi.Printf( va( "OBJECTIVE SP: %s\n" , ObjName.c_str() ) );

	int ObjIndex;
	bool playSound = false;
	ObjIndex = gi.MObjective_GetIndexFromName( ObjName.c_str() );
	if ( ObjIndex == 0 ){
		gi.Printf( "coop_objectivesSingleplayer: invalid objective name\n" );
		return;
	}
	
	if ( !Q_stricmp( sObjectiveState , "ObjectiveComplete") ){//CHEKME
		qboolean Complete = 1;

		switch ( ObjIndex )
		{
		case OBJECTIVE1:
			if ( Complete ){ player->_objectiveStates |= OBJECTIVE1_COMPLETE; }
			else{ player->_objectiveStates &= ~( OBJECTIVE1_COMPLETE ); }
			break;

		case OBJECTIVE2:
			if ( Complete ){ player->_objectiveStates |= OBJECTIVE2_COMPLETE; }
			else{ player->_objectiveStates &= ~( OBJECTIVE2_COMPLETE ); }
			break;

		case OBJECTIVE3:
			if ( Complete ){ player->_objectiveStates |= OBJECTIVE3_COMPLETE; }
			else{ player->_objectiveStates &= ~( OBJECTIVE3_COMPLETE ); }
			break;

		case OBJECTIVE4:
			if ( Complete ){ player->_objectiveStates |= OBJECTIVE4_COMPLETE; }
			else{ player->_objectiveStates &= ~( OBJECTIVE4_COMPLETE ); }
			break;

		case OBJECTIVE5:
			if ( Complete ){ player->_objectiveStates |= OBJECTIVE5_COMPLETE; }
			else{ player->_objectiveStates &= ~( OBJECTIVE5_COMPLETE ); }
			break;

		case OBJECTIVE6:
			if ( Complete ){ player->_objectiveStates |= OBJECTIVE6_COMPLETE; }
			else{ player->_objectiveStates &= ~( OBJECTIVE6_COMPLETE ); }
			break;

		case OBJECTIVE7:
			if ( Complete ){ player->_objectiveStates |= OBJECTIVE7_COMPLETE; }
			else{ player->_objectiveStates &= ~( OBJECTIVE7_COMPLETE ); }
			break;

		case OBJECTIVE8:
			if ( Complete ){ player->_objectiveStates |= OBJECTIVE8_COMPLETE; }
			else{ player->_objectiveStates &= ~( OBJECTIVE8_COMPLETE ); }
			break;

		default:
			break;
		}
	}
	else if ( !Q_stricmp( sObjectiveState, "ObjectiveFailed") ){
		qboolean Failed = 1;
		switch ( ObjIndex )
		{
		case OBJECTIVE1:
			if ( Failed )
				player->_objectiveStates |= OBJECTIVE1_FAILED;
			else
				player->_objectiveStates &= ~( OBJECTIVE1_FAILED );
			break;

		case OBJECTIVE2:
			if ( Failed ){ player->_objectiveStates |= OBJECTIVE2_FAILED; }
			else{ player->_objectiveStates &= ~( OBJECTIVE2_FAILED ); }
			break;

		case OBJECTIVE3:
			if ( Failed ){ player->_objectiveStates |= OBJECTIVE3_FAILED; }
			else{ player->_objectiveStates &= ~( OBJECTIVE3_FAILED ); }
			break;

		case OBJECTIVE4:
			if ( Failed ){ player->_objectiveStates |= OBJECTIVE4_FAILED; }
			else{ player->_objectiveStates &= ~( OBJECTIVE4_FAILED ); }
			break;

		case OBJECTIVE5:
			if ( Failed ){ player->_objectiveStates |= OBJECTIVE5_FAILED; }
			else{ player->_objectiveStates &= ~( OBJECTIVE5_FAILED ); }
			break;

		case OBJECTIVE6:
			if ( Failed ){ player->_objectiveStates |= OBJECTIVE6_FAILED; }
			else{ player->_objectiveStates &= ~( OBJECTIVE6_FAILED ); }
			break;

		case OBJECTIVE7:
			if ( Failed ){ player->_objectiveStates |= OBJECTIVE7_FAILED; }
			else{ player->_objectiveStates &= ~( OBJECTIVE7_FAILED ); }

		case OBJECTIVE8:
			if ( Failed ){ player->_objectiveStates |= OBJECTIVE8_FAILED; }
			else{ player->_objectiveStates &= ~( OBJECTIVE8_FAILED ); }
			break;

		default:
			break;
		}
	}
	else{
		//Update the objective show flag.
		gi.MObjective_SetShowObjective( ObjName.c_str() , Show );

		//Set the appropriate bit on the flag passed to the client.
		unsigned int bitToChange;

		switch ( ObjIndex )
		{
		case OBJECTIVE1:
			bitToChange = OBJECTIVE1_SHOW;
			break;
		case OBJECTIVE2:
			bitToChange = OBJECTIVE2_SHOW;
			break;
		case OBJECTIVE3:
			bitToChange = OBJECTIVE3_SHOW;
			break;
		case OBJECTIVE4:
			bitToChange = OBJECTIVE4_SHOW;
			break;
		case OBJECTIVE5:
			bitToChange = OBJECTIVE5_SHOW;
			break;
		case OBJECTIVE6:
			bitToChange = OBJECTIVE6_SHOW;
			break;
		case OBJECTIVE7:
			bitToChange = OBJECTIVE7_SHOW;
			break;
		case OBJECTIVE8:
			bitToChange = OBJECTIVE8_SHOW;
			break;
		default:
			bitToChange = 0;
			break;
		}

		if ( Show && !( player->_objectiveStates & bitToChange ) ){
			player->_objectiveStates |= bitToChange;
			playSound = true;
		}

		if ( playSound )
			player->Sound( "snd_objectivechanged" , CHAN_LOCAL );
	}
}




//================================================================
// Name:        coop_objectivesUpdate
// Class:       -
//              
// Description: UPDATES A OBJECTIVE, IN RELATION TO COOP LEVEL SCRIPT, needed sice we can't read floats from script in the interpreter
//              
// Parameters:  str sObjectiveState , str sObjectiveItem , str sObjectiveShow
//              
// Returns:     void
//              
//================================================================
void coop_objectivesUpdate( str sObjectiveState, str sObjectiveItem, str sObjectiveShow  )
{
	int iObjectivteStatus;
	str sObjective;
	int iObjectiveNumber;
	iObjectiveNumber = atoi( sObjectiveItem );
	str sVariableName = "coop_string_objectiveItem";
	game.coop_objectiveLastUpdate = level.time;

//get the mission objective string from script variable, make sure the item range is valid 1-8 (-11 since we gravtactical info here as well)
	if ( iObjectiveNumber < 2 ){
		iObjectiveNumber = 1;
	}
	else if ( iObjectiveNumber > 11 ){
		iObjectiveNumber = 11;
	}

//check for valid objectives state, set incomplete on default
	sObjectiveState = sObjectiveState.tolower();
	
	if ( !Q_stricmp( sObjectiveState, "complete") || !Q_stricmp( sObjectiveState, "objectivecomplete") ){
		iObjectivteStatus = 2;
		sObjectiveState = "ObjectiveComplete";
	}else if ( !Q_stricmp( sObjectiveState, "failed") || !Q_stricmp( sObjectiveState, "objectivefailed") ){
		iObjectivteStatus = 3;
		sObjectiveState = "ObjectiveFailed";
	}else{
		iObjectivteStatus = 1;
		sObjectiveState = "ObjectiveIncomplete";
	}

//update global objective arrays
	if ( iObjectiveNumber < 9){
//make sure that it stays completed once it has been completed!
		if ( game.coop_objectiveItemState[( iObjectiveNumber - 1 )] == 2 ){
			iObjectivteStatus = 2;
			sObjectiveState = "ObjectiveComplete";		
		}else{
			game.coop_objectiveItemState[( iObjectiveNumber - 1 )] = iObjectivteStatus;
		}

		if ( iObjectivteStatus == 2){
			game.coop_objectiveItemCompletedAt[( iObjectiveNumber - 1 )] = level.time;
		}
	}

	sVariableName += iObjectiveNumber;
	sObjective = program.getStringVariableValue( sVariableName.c_str());

//check is it a valid string
	if ( sObjective.length() < 2 ){
		gi.Printf( va( "CO-OP OBJECTIVE ERROR: script variable %s is empty or to short!" , sVariableName.c_str() ) );
		return;
	}

//print status into the game console
	str sLocalString = "$$";
	//do not automatically localize for custom maps!
	if ( !game.isStandardLevel ){
		sLocalString = "";
	}
	//do not localize if it already is
	else if ( sObjective[0] == '$' && sObjective[1] == '$' ){
		sLocalString = "";
	}

	//phrase text replace umlaute äöäü and ß, allow german text to work right
	sObjective = sObjective;

	gi.Printf( va( "OBJECTIVE $$%s$$ | %s%s%s\n" , sObjectiveState.c_str() , sLocalString.c_str() , sObjective.c_str() , sLocalString.c_str() ) );

//handle singleplayer objectives, this only works, if the objectives are default game objectives
/*	if ( player && g_gametype->integer == GT_SINGLE_PLAYER){
		if ( game.isStandardLevel ){
			coop_objectivesSingleplayer( sObjectiveState , iObjectiveNumber , (int)bShowNow );	
		}else{
			coop_objectivesNotify( player );
			coop_objectivesShow( player , iObjectiveNumber , iObjectivteStatus , bShowNow );
		}
	}
	*/
}

//================================================================
// Name:        coop_objectivesShow
// Class:       -
//              
// Description: SHOWS OBJECTIVE TO THE GIVEN PLAYER IF NEEDED
//              
// Parameters:  Player *player , int fObjectiveItem , int sObjectiveState , bool fObjectiveShow
//              
// Returns:     void
//              
//================================================================
void coop_objectivesShow( Player *player , int iObjectiveItem , int iObjectiveState , bool bfObjectiveShow )
{
	if ( !player || level.mission_failed ){
		return;
	}

	str sObjective;
	str sStatusName;
	str sVariableName;

//determin objective status
	switch ( iObjectiveState )
	{
		case 0:
			sStatusName = "BAD_OBJSTATE";
			break;
		case 1:
			sStatusName = "^5$$ObjectiveIncomplete$$";
			break;
		case 2:
			sStatusName = "^2$$ObjectiveComplete$$";
			break;
		default:
			sStatusName = "^1$$ObjectiveFailed$$";
			iObjectiveState = 3;
			break;
	}

//get cvarname
	str sCvar = coop_returnObjectivesCvarName( iObjectiveItem );
	str sLocalString = "$$";

//do not automatically localize for custom maps!
	if ( !game.isStandardLevel ){
		sLocalString = "";
	}

	if ( iObjectiveItem < 9){
		sVariableName = "coop_string_objectiveItem";
		sVariableName += iObjectiveItem;

		//if ( iObjectiveItem == 1 && iObjectiveState == 1 && player->coopPlayer.installed == 1 || g_gametype->integer == GT_SINGLE_PLAYER ){
			//str sStory = game.coop_story;
			//if ( coop_checkPlayerLanguageGerman(player) ){
				//if ( game.coop_story_deu.length() > 0){
					//sStory = game.coop_story_deu;
				//}else{
					//gi.Printf( "WARNING: No German Backgroundstory set in uservar3 of $world OR in script: coop_string_story\n" );
				//}
			//}
			//DelayedServerCommand( player->entnum , va( "set coop_story %s" , sStory.c_str() ) );
		//}

		if ( coop_checkPlayerLanguageGerman(player) && game.isStandardLevel == false ){
			str sTempVar , sTempVar2;
			sTempVar2 = sVariableName;
			sTempVar2 += "_deu";
			sTempVar = program.getStringVariableValue( sTempVar2.c_str() );
			if ( sTempVar.length() > 0 ){
				sObjective = sTempVar;
			}else{
				sObjective = program.getStringVariableValue( sVariableName.c_str() );
				gi.Printf( va( "WARNING: No German objective string set for Objective: %d\n" , iObjectiveItem ) );
			}
		}else{
			sObjective = program.getStringVariableValue( sVariableName.c_str() );
		}
		if ( sObjective.length() > 1 ){
		//make sure it is not localized twiche...
			if ( sObjective[0] == '$' && sObjective[1] == '$' ){
				sLocalString = "";
			}

			//if player has the mod installed display to menu
			if ( player->coopPlayer.installed ){
				//phrase text replace umlaute äöäü and ß, allow german text to work right - using loc strings $$u$$
				sObjective = sObjective;

				DelayedServerCommand( player->entnum , va( "set %s %s%s%s" , sCvar.c_str() , sLocalString.c_str() , sObjective.c_str() , sLocalString.c_str() ) );
				DelayedServerCommand( player->entnum , va( "set %s_s %i" , sCvar.c_str() , iObjectiveState ) );
			}
			//otherwhise display to chat hud
			else{
				//phrase text replace umlaute äöäü and ß, allow german text to work better - using alias: ue ae oe ss
				sObjective = sObjective;
				//don't display if, objective is complete and has been completed more than 5 secounds ago
				float fAge = game.coop_objectiveItemCompletedAt[( iObjectiveItem - 1 )];
				if ( ( fAge + 5 ) > level.time || fAge < 0 ){
					if ( gi.GetNumFreeReliableServerCommands( player->entnum ) > 32 ){
						if ( ( game.coop_objectiveLastUpdate + 3 ) < level.time || player->coopPlayer.lastTimeUpdatedObjectives < 0 ){
							//make sure the objectives of player is sync
							//player->hudPrint( va( "\n^0=^5-^0=^5-^0=^5-^0=^5-^5^5Co-Op Mod %i ^0-^5 $$MissionObjectives$$ ^0=^5-^0=^5-^0=^5-^0=\n" , COOP_BUILD ) );
							player->hudPrint( "\n^8======^5 $$MissionObjectives$$ ^8======\n" );
						}
						player->hudPrint( va( "%s:^8 %s%s%s\n" , sStatusName.c_str() , sLocalString.c_str() , sObjective.c_str() , sLocalString.c_str() ) );
					}
				}
			}
		}
	}
	else
	{
		str sHint = "";
		sVariableName = "coop_string_objectiveTacticalInfo";
		sVariableName += (iObjectiveItem - 8);
		str sHintString;
		if ( coop_checkPlayerLanguageGerman(player) ){
			sHintString = sVariableName;
			sHintString += "_deu";
			sHint = program.getStringVariableValue( sHintString.c_str() );
		}
		if ( sHint.length() < 1 ){
			sHint = program.getStringVariableValue( sVariableName.c_str() );
		}
		if ( sHint.length() > 0 && player->coopPlayer.installed == 1 ){
			DelayedServerCommand( player->entnum , va( "set %s %s%s%s" , sCvar.c_str() , sLocalString.c_str() , sHint.c_str() , sLocalString.c_str() ) );
		}
	}
}

//================================================================
// Name:        coop_objectives_tacticalShow
// Class:       -
//              
// Description: SHOWS TACTICAL TO THE GIVEN PLAYER
//              
// Parameters:  Player *player , int iTactical
//              
// Returns:     void
//              
//================================================================
void coop_objectives_tacticalShow( Player *player , int iTactical )
{
	if ( player && player->coopPlayer.installed == 1 )
	{
		if ( iTactical < 1 )
		{
			iTactical = 1;
		}
		else if ( iTactical > 3 )
		{
			iTactical = 3;
		}

		str sTacEng = program.getStringVariableValue( va("coop_string_objectiveTacticalInfo%d", iTactical ) );
		str sTacDeu = program.getStringVariableValue( va( "coop_string_objectiveTacticalInfo%d_deu" , iTactical ) );
		if ( coop_checkPlayerLanguageGerman( player ) && sTacDeu.length() ) {
			DelayedServerCommand( player->entnum , va( "set coop_t%d %s" , iTactical , sTacDeu.c_str() ) );
		}
		else if( sTacEng.length() ){
			DelayedServerCommand( player->entnum , va( "set coop_t%d %s" , iTactical , sTacEng.c_str() ) );
		}
	}
}