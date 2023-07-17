//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER MODEL/SKIN HANDLING RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2014 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#include "_pch_cpp.h"
#include "coopModel.hpp"


//================================================================
// Name:        coop_modelValidPlayer
// Class:       -
//              
// Description: Returns if the given model is a valid player model 
//              
// Parameters:  Player *player
//              
// Returns:     BOOL
//              
//================================================================
bool coop_modelValidPlayer( str sModel )
{
	//[b60012] chrissstrahl - allow illia model on included coop custom coop maps
	sModel.tolower();
	if (/*game.isCoopIncludedLevel && */ !game.isStandardLevel && sModel == "models/char/gbs_dm_illia.tik") {
		return true;
	}

	if ( sModel != "models/char/munro.tik" &&
		sModel != "models/char/dm_munro.tik" &&
		sModel != "models/char/dm_chang.tik" &&
		sModel != "models/char/dm_chell.tik" &&
		sModel != "models/char/eb_gonzales.tik" &&
		sModel != "models/char/dm_kourban.tik" &&
		sModel != "models/char/dm_telsia.tik" &&
		sModel != "models/char/dm_jurot.tik" )
	{
		return false;
	}
	return true;
}

//================================================================
// Name:        coop_modelDisplayInjuredSprite
// Class:       -
//              
// Description: Displays a injured sprite at the players feet if player is hurt badly
//              this sprite can be seen by all players, this is suppose to make medics do their job and improve the teamplay
//              
// Parameters:  Player *player
//              
// Returns:     VOID
//              
//================================================================
void coop_modelDisplayInjuredSprite( Player *player )
{
	//hzm coop mod chrissstrahl - display injured symbol at player his feet
	///////////////////////////////////////////////////////////////////////
	float nextVisibleTime = ( player->coopPlayer.lastTimeInjured + float( 1 * 1000 ) );

	if ( level.inttime > nextVisibleTime ){
		player->coopPlayer.lastTimeInjured = level.inttime;

		if ( player->health > 0.0f && player->health < 50.0f ){
			Entity *eAttached;
			eAttached = coop_returnEntityAttachedByTargetname( ( Entity * )player , "globalCoop_playerInjured" );

			if ( eAttached && player->coop_playerNeutralized()){
				player->removeAttachedModelByTargetname( "globalCoop_playerInjured" );
				player->coopPlayer.injuredSymbolVisible = false;
			}

			if ( eAttached ){
				if ( !player->coopPlayer.injuredSymbolVisible ){
					eAttached->showModel();
					player->coopPlayer.injuredSymbolVisible = true;
				}
				else{
					eAttached->hideModel();
					player->coopPlayer.injuredSymbolVisible = false;
				}
			}
			else{
				Vector vAttachOffset = Vector(0,0,0);
				vAttachOffset[1] = 10.0f; //[b607] chrissstrahl - place skull a little bit higher so it can be seen right

				Event *attachEvent;
				attachEvent = new Event( EV_AttachModel );
				attachEvent->AddString( "sysimg/icons/items/rune_deathquad.spr" );

				//hover above head when down
				if ( player->coop_playerNeutralized() ){
					attachEvent->AddString( "tag_head" );
					vAttachOffset[2] = 30.0f;
				}
				//show at legs
				else{
					attachEvent->AddString( "origin" );
				}
				attachEvent->AddFloat( 0.2f );
				attachEvent->AddString( "globalCoop_playerInjured" );
				attachEvent->AddInteger( 0 );
				attachEvent->AddFloat( 0.0f );
				attachEvent->AddFloat( -1.0f );
				attachEvent->AddFloat( -1.0f );
				attachEvent->AddFloat( -1.0f );
				attachEvent->AddVector( vAttachOffset );
				player->ProcessEvent( attachEvent );

				player->coopPlayer.injuredSymbolVisible = true;
			}
		}
		else{
			player->removeAttachedModelByTargetname( "globalCoop_playerInjured" );
			player->coopPlayer.injuredSymbolVisible = false;
		}
	}
}

