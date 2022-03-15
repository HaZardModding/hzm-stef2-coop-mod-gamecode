//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER WEAPON/ARMORY RELATED FUNCTIONS FOR THE HZM CO-OP MOD

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
#include "coopReturn.hpp"
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
// Name:        coop_armoryRememberLastUsed
// Class:       -
//              
// Description: remembers the weapon the player used last, restore on mapchange
//              
// Parameters:  Player player
//              
// Returns:     bool
//              
//================================================================
void coop_armoryRememberLastUsed( Player *player, str sWeapon )
{
	if ( !game.coop_isActive )return;
	DelayedServerCommand( player->entnum , va( "set coop_weap use %s" , sWeapon.c_str() ) );
}

//================================================================
// Name:        coop_armoryEquipPlayer
// Class:       -
//              
// Description: Equips player with starting weapons for coop, sepcified by script
//              
// Parameters:  Player player
//              
// Returns:     bool
//              
//================================================================
void coop_armoryEquipPlayer(Player *player)
{
	if ( !player )return;

//	bool bGiveSecretWeapons=true;
	int iWeapons=0;
	int iTemporary;
	str sTricorderName , sWeaponName , sPhaserName, sVariableName , sTikiName, sUseThisWeapon;

	sUseThisWeapon = "";

	//hzm coop mod chrissstrahl - equip the player, give weapons specified by script
	for ( iTemporary = 8; iTemporary > 0; iTemporary-- ){
		sVariableName = "coop_string_weapon";
		sVariableName += iTemporary;
		sTikiName = program.getVariableValueAsString( sVariableName.c_str() );

		if ( sTikiName.length() > 4 ){
			iWeapons++;

			//make this work in coop and singleplayer
			if(g_gametype->integer != GT_SINGLE_PLAYER ){
				multiplayerManager.givePlayerItem( player->entnum , sTikiName.c_str() );
//gi.Printf(va("COOPDEBUG: Equiping %s with %s\n", player->client->pers.netname, sTikiName.c_str()));
			}else{
				Event *event;
				event = new Event( EV_Player_GiveCheat );
				event->AddString( sTikiName );
				player->ProcessEvent( event );
			}

			str sTempName = coop_armoryReturnWeaponName( sTikiName );
			sTikiName = sTikiName.tolower();
			sTempName = sTempName.tolower();

			if (	!strcmpi( sTikiName , "models/weapons/worldmodel-tricorder-stx.tik" ) ||
					!strcmpi( sTikiName , "models/weapons/worldmodel-rom-datapad.tik" ) ||
					!strcmpi( sTikiName , "models/weapons/worldmodel-tricorder.tik" ) )
			{
				sTricorderName = sTikiName;
			}
			else
			{
				/* chrissstrahl - don't need it this is managed by global_playerLoadout.scr
				if ( bGiveSecretWeapons && game.isStandardLevel )
				{
					//gi.Printf("HZM Coop Mod - Checking and restoring secret weapons\n");
					if ( coop_returnBool( coop_phraserIniGet( "ini/server.ini" , "secretWeapon_CompressionRifle" , "server" ) ) )
						multiplayerManager.givePlayerItem( player->entnum , "models/weapons/worldmodel-CompressionRifle.tik" );
					if ( coop_returnBool( coop_phraserIniGet( "ini/server.ini" , "secretWeapon_IMOD" , "server" ) ) )
						multiplayerManager.givePlayerItem( player->entnum , "models/weapons/worldmodel-imod.tik" );
					if ( coop_returnBool( coop_phraserIniGet( "ini/server.ini" , "secretWeapon_IdryllStaff" , "server" ) ) )
						multiplayerManager.givePlayerItem( player->entnum , "models/weapons/worldmodel-drull-staff.tik" );
					
					//if it is the romulan secret mission give romulan hand textured rad, otherwise, give normal
					if ( !Q_stricmpn( level.mapname,"m10l",4)  )
					{
						if ( coop_returnBool( coop_phraserIniGet( "ini/server.ini" , "secretWeapon_RomulanExperimental" , "server" ) ) )
							multiplayerManager.givePlayerItem( player->entnum , "models/weapons/worldmodel-rom-radgun-romhands.tik" );
					}
					else
					{
						if ( coop_returnBool( coop_phraserIniGet( "ini/server.ini" , "secretWeapon_RomulanExperimental" , "server" ) ) )
							multiplayerManager.givePlayerItem( player->entnum , "models/weapons/worldmodel-rom-radgun.tik" );
					}
				}
				*/

				if ( !strcmpi( sTikiName , "models/weapons/worldmodel-phaser.tik" ) ||
					!strcmpi( sTikiName , "models/weapons/worldmodel-phaser-stx.tik" ) )
				{
					sPhaserName = sTikiName;
				}
				else {
					sWeaponName = sTikiName;
				}
			}
		}
	}

	//[b610] chrissstrahl - print out how many weapons player got at start
	gi.Printf(va("INFO COOP: %i script-specified Weapons given to %s\n", iWeapons, player->client->pers.netname));

	//hzm coop mod chrissstrahl - exit here if there are no weapons to handle
	if ( iWeapons == 0 ){
		return;
	}

	//hzm coop mod chrissstrahl - check if there is no active weapon, fall back to tricoder if it exists
	//this should fix the problem that in many missions players spawn with the tricorder, instead
	//with a real weapon in their hands, making some respawn situations a angering issue
	//Priority:
	//- any other real weapon
	//- phaser
	//- tricorder
	if ( !Q_stricmp( sUseThisWeapon,"") ){
		if ( sWeaponName.length() < 4 && sPhaserName.length() > 3 ){
			sTikiName = sPhaserName;
		}
		else if ( sWeaponName.length() < 4 && sTricorderName.length() > 3 ){
			sTikiName = sTricorderName;
		}
		else{
			sTikiName = sWeaponName;
		}
		sUseThisWeapon = coop_armoryReturnWeaponName( sTikiName );
	}

	if ( sUseThisWeapon.length() > 0 ){
		Event *ev = new Event( "use" );
		ev->AddString( sUseThisWeapon.c_str() );
		player->ProcessEvent( ev );			
	}
	DelayedServerCommand( player->entnum , "vstr coop_weap" );
}


//================================================================
// Name:        coop_armoryReturnTiki
// Class:       -
//              
// Description: Returns Tiki file location name for given weapon
//              
// Parameters:  str sWeaponName
//              
// Returns:     str
//              
//================================================================
str coop_armoryReturnWeaponTiki( str sWeaponName )
{
	str sTikiName = "";
	if ( sWeaponName.length() )
	{
		sWeaponName = sWeaponName.tolower();
		if ( !Q_stricmpn( "federationsniperrifle" , sWeaponName , 21 ) )
		{
			sTikiName = "models/weapons/worldmodel-Sniperrifle.tik";
		}
		else if ( !Q_stricmpn( "fieldassaultrifle" , sWeaponName , 17 ) )
		{
			sTikiName = "models/weapons/worldmodel-fieldassaultrifle.tik";
		}
		else if ( !Q_stricmpn( "compressionrifle" , sWeaponName , 16 ) )
		{
			sTikiName = "models/weapons/worldmodel-compressionrifle.tik";
		}
		else if ( !Q_stricmpn( "burstrifle" , sWeaponName , 10 ) )
		{
			sTikiName = "models/weapons/worldmodel-burstrifle.tik";
		}
		else if ( !Q_stricmpn( "batleth" , sWeaponName , 7 ) )
		{
			sTikiName = "models/weapons/worldmodel-batleth.tik";
		}
		else if ( !Q_stricmpn( "i-mod" , sWeaponName , 5 ) )
		{
			sTikiName = "models/weapons/worldmodel-IMod.tik";
		}
		else if ( !Q_stricmpn( "phaser" , sWeaponName , 6 ) )
		{
			sTikiName = "models/weapons/worldmodel-phaser.tik";
		}
		else if ( !Q_stricmpn( "phaser-stx" , sWeaponName , 10 ) )
		{
			sTikiName = "models/weapons/worldmodel-phaser-STX.tik";
		}
		else if ( !Q_stricmpn( "tricorder-stx" , sWeaponName , 13 ) )
		{
			sTikiName = "models/weapons/worldmodel-tricorder-STX.tik";
		}
		else if ( !Q_stricmpn( "tricorder" , sWeaponName , 9 ) )
		{
			sTikiName = "models/weapons/worldmodel-tricorder.tik";
		}
		else if ( !Q_stricmpn( "tricorder-rom" , sWeaponName , 13 ) )
		{
			sTikiName = "models/weapons/worldmodel-rom-datapad.tik";
		}
		else if ( !Q_stricmpn( "romulandisruptor" , sWeaponName , 16 ) )
		{
			sTikiName = "models/weapons/worldmodel-rom-disruptor.tik";
		}
		else if ( !Q_stricmpn( "tetryongatlinggun" , sWeaponName , 17 ) )
		{
			sTikiName = "models/weapons/worldmodel-tetryon.tik";
		}
		else if ( !Q_stricmpn( "attrexianrifle" , sWeaponName , 14 ) )
		{
			sTikiName = "models/weapons/worldmodel-attrex-rifle.tik";
		}
		else if ( !Q_stricmpn( "drullstaff" , sWeaponName , 10 ) )
		{
			sTikiName = "models/weapons/worldmodel-drull-staff.tik";
		}
		else if ( !Q_stricmpn( "grenadelauncher" , sWeaponName , 15 ) )
		{
			sTikiName = "models/weapons/worldmodel-grenadelauncher.tik";
		}
		else if ( !Q_stricmpn( "photonburst" , sWeaponName , 11 ) )
		{
			sTikiName = "models/weapons/worldmodel-photon.tik";
		}
		else if ( !Q_stricmpn( "romulanradgun" , sWeaponName , 13 ) )
		{
			sTikiName = "models/weapons/worldmodel-rom-radgun.tik";
		}
		else if ( !Q_stricmpn( "knife" , sWeaponName , 5 ) )
		{
			sTikiName = "models/weapons/worldmodel-knife.tik";
		}
		else if ( !Q_stricmpn( "enterprisecannon" , sWeaponName , 16 ) )
		{
			sTikiName = "models/weapons/worldmodel-enterprise.tik";
		}
	}
	return sWeaponName;
}


//================================================================
// Name:        coop_armoryReturnWeaponName
// Class:       -
//              
// Description: Returns name from  given weapon Tiki file location
//              
// Parameters:  str sTikiName
//              
// Returns:     str
//              
//================================================================
str coop_armoryReturnWeaponName( str sTikiName )
{
	str sWeaponName = "None";
	if(sTikiName.length())
	{
		sTikiName = sTikiName.tolower();

		//not astarting off with model/, add it
		if ( Q_stricmpn(sTikiName,"models/",7) )
		{
			sTikiName += "models/" + sTikiName;
		}//coop_armoryReturnWeaponName( str sTikiName )

		if ( !Q_stricmpn( "models/weapons/worldmodel-sniperrifle.tik" , sTikiName , 41 ) )
		{
			sWeaponName = "FederationSniperRifle";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-fieldassaultrifle.tik" , sTikiName , 47 ) )
		{
			sWeaponName = "FieldAssaultRifle";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-compressionrifle.tik" , sTikiName , 46 ) )
		{
			sWeaponName = "CompressionRifle";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-burstrifle.tik" , sTikiName , 40 ) )
		{
			sWeaponName = "BurstRifle";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-batleth.tik" , sTikiName , 37 ) )
		{
			sWeaponName = "Batleth";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-imod.tik" , sTikiName , 34 ) )
		{
			sWeaponName = "I-Mod";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-phaser.tik" , sTikiName , 36 ) )
		{
			sWeaponName = "Phaser";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-phaser-stx.tik" , sTikiName , 40 ) )
		{
			sWeaponName = "Phaser-stx";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-tricorder-stx.tik" , sTikiName , 43 ) )
		{
			sWeaponName = "Tricorder-stx";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-tricorder.tik" , sTikiName , 39 ) )
		{
			sWeaponName = "Tricorder";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-rom-datapad.tik" , sTikiName , 41 ) )
		{
			sWeaponName = "Tricorder-rom";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-rom-disruptor.tik" , sTikiName , 43 ) )
		{
			sWeaponName = "RomulanDisruptor";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-rom-disruptor-romhands.tik" , sTikiName , 52 ) )
		{
			sWeaponName = "RomulanDisruptor";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-tetryon.tik" , sTikiName , 37 ) )
		{
			sWeaponName = "TetryonGatlingGun";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-attrex-rifle.tik" , sTikiName , 42 ) )
		{
			sWeaponName = "AttrexianRifle";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-drull-staff.tik" , sTikiName , 41 ) )
		{
			sWeaponName = "DrullStaff";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-grenadelauncher.tik" , sTikiName , 45 ) )
		{
			sWeaponName = "GrenadeLauncher";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-photon.tik" , sTikiName , 36 ) )
		{
			sWeaponName = "PhotonBurst";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-rom-radgun.tik" , sTikiName , 40 ) )
		{
			sWeaponName = "RomulanRadGun";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-rom-radgun-romhands.tik" , sTikiName , 49 ) )
		{
			sWeaponName = "RomulanRadGun";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-knife.tik" , sTikiName , 35 ) )
		{
			sWeaponName = "Knife";
		}
		else if ( !Q_stricmpn( "models/weapons/worldmodel-enterprise.tik" , sTikiName , 40 ) )
		{
			sWeaponName = "EnterpriseCannon";
		}
	}
	return sWeaponName;
}










