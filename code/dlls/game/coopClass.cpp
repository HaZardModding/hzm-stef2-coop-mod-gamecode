//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER CLASS(Medic,Technician,HeavyWeapons) HANDLING RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]

#include "_pch_cpp.h"

#include "coopClass.hpp"
#include "coopHuds.hpp"
#include "coopPlayer.hpp"
#include "coopCheck.hpp"
#include "coopReturn.hpp"
#include "sentient.h"
#include "entity.h"
#include "player.h"
#include "weapon.h"
#include "mp_manager.hpp"
#include "mp_manager.hpp"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"


//================================================================
// Name:        coop_classCheckApplay
// Class:       -
//              
// Description: Checks and applies the selected class upon player if it should
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_classCheckApplay( Player *player )
{
	//[b60011] chrissstrahl - changing class disabled feature, is located in: G_coopCom_class
	if ( player->coopPlayer.lastTimeChangedClass != player->coopPlayer.lastTimeAppliedClass ){
		coop_classApplayAttributes( player , false );
		player->coopPlayer.lastTimeAppliedClass = player->coopPlayer.lastTimeChangedClass;
	}
}


//================================================================
// Name:        coop_classCeckUpdateStat
// Class:       -
//              
// Description: Updates class statistics of how many players are with the given class
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_classCeckUpdateStat( Player *player )
{
	if ( game.coop_classInfoSendAt > player->coopPlayer.lastTimeUpdatedClassStat ){
		if ( player->coopPlayer.installed ) {
			player->coopPlayer.lastTimeUpdatedClassStat = game.coop_classInfoSendAt;
			DelayedServerCommand( player->entnum , va( "set coop_ch %i" , coop_classPlayersOfClass( "HeavyWeapon" ) ) );
			DelayedServerCommand( player->entnum , va( "set coop_ct %i" , coop_classPlayersOfClass( "Technician" ) ) );
			DelayedServerCommand( player->entnum , va( "set coop_cm %i" , coop_classPlayersOfClass( "Medic" ) ) );
		}
	}
}

//================================================================
// Name:        coop_classRegenerate
// Class:       -
//              
// Description: Regenerates according to class. Medic=Health,Tech=Shield,HW=Ammo
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================
void coop_classRegenerate( Player *player )
{
	//do not regenerate if dead, neutralized or under fire
	//[b60014] chrissstrahl - go further only in multiplayer or coop
	if (	/* haloThingActive || */
			player->health <= 0 ||
			!multiplayerManager.inMultiplayer() ||
			game.coop_isActive ||
			player->coopPlayer.neutralized ||
			(player->getLastDamageTime() + COOP_CLASS_HURT_WAITTIME) > level.time )
	{
		return;
	}

	//[b60014] chrissstrahl - deactivating regeneration to motivate players to work closer together
	return;

	//[b60012] chrissstrahl - fix missing .c_str()
	if ( !Q_stricmp( player->coopPlayer.className.c_str(), COOP_CLASS_NAME_MEDIC) ){
		player->AddHealth( COOP_CLASS_REGENERATE_HEALTH );
	}
	else if ( !Q_stricmp( player->coopPlayer.className.c_str(), COOP_CLASS_NAME_TECHNICIAN) ){
		float fArmorCurrent = player->GetArmorValue();
		if ( ( fArmorCurrent + COOP_CLASS_REGENERATE_ARMOR ) <= COOP_MAX_ARMOR ){
			fArmorCurrent++;
			Event *armorEvent;
			armorEvent = new Event( EV_Sentient_GiveArmor );
			armorEvent->AddString( "BasicArmor" );
			armorEvent->AddInteger( COOP_CLASS_REGENERATE_ARMOR );
			player->ProcessEvent( armorEvent );
		}
	}
	else if ( !Q_stricmp( player->coopPlayer.className.c_str(), COOP_CLASS_NAME_HEAVYWEAPONS) ){
		player->GiveAmmo( "Fed" , COOP_CLASS_REGENERATE_AMMO , false , COOP_MAX_HW_AMMO_FED );
		player->GiveAmmo( "Plasma" , COOP_CLASS_REGENERATE_AMMO , false , COOP_MAX_HW_AMMO_PLASMA );
		player->GiveAmmo( "Idryll" , COOP_CLASS_REGENERATE_AMMO , false , COOP_MAX_HW_AMMO_IDRYLL );
	}
}


//================================================================
// Name:        coop_classNotifyOfInjured
// Class:       -
//              
// Description: Notifies all medics if a player is injured and needs a medic
//              
// Parameters:  Player *player
//              
// Returns:     void
//              
//================================================================

/*
void coop_classNotifyOfInjured( Player *player )
{
	if ( !player || player->health <= 0.0f ){
		return;
	}

	str sCoopInjuredMessage;
	sCoopInjuredMessage = va( "%s ^8needs " , player->client->pers.netname );
	if ( player->health < COOP_HEALTH_CRITICAL ){
		sCoopInjuredMessage += "now ^2urgent ";
	}
	sCoopInjuredMessage += "^2Medical^8 attention.\n";

	int i;
	Player *medic;
	if ( player->health <= COOP_HEALTH_CRITICAL || player->GetArmorValue() <= COOP_ARMOR_NOTIFY && player->health <= COOP_HEALTH_NOTIFY ){
		if ( ( player->coopPlayer.lastTimeHealMeInfo + COOP_NOTIFY_DELAY ) < level.time || player->coopPlayer.lastHealMeInfoCritical == false && player->health <= COOP_HEALTH_CRITICAL ){
			int iMeds = 0;
			for ( i = 0; i < maxclients->integer; i++ ){
				medic = ( Player * )g_entities[i].entity;
				if ( medic && medic->client && medic->isSubclassOf( Player ) && medic->coopPlayer.className == "Medic" ){
					iMeds++;
					if ( player->health <= COOP_HEALTH_CRITICAL ){
						player->coopPlayer.lastHealMeInfoCritical = true;
					}else{
						player->coopPlayer.lastHealMeInfoCritical = false;
					}
					medic->hudPrint( sCoopInjuredMessage );
				}
			}
		//remember/pause only if any medic has been notified
			if ( iMeds != 0){
				player->coopPlayer.lastTimeHealMeInfo = level.time;
			}
		}
	}
}
*/

//================================================================
// Name:        coop_classUpdateHealthStatFor
// Class:       -
//              
// Description: Sends current health for the medic health info hud to the players
//              
// Parameters:  int iPlayerNum
//              
// Returns:     void
//              
//================================================================
/*void coop_classUpdateHealthStatFor( int iPlayerNum )
{
	//we don't use this anymore - now we aim at player to see health


	if ( !game.coop_isActive ){
		return;
	}
	//notify every medic about health change of the given player
	int i;
	int iHealth = 0;
	Player *player = NULL;
	player = multiplayerManager.getPlayer( iPlayerNum );
	if ( player ){
		iHealth = ( int )player->health;
		if ( multiplayerManager.isPlayerSpectator( player ) || iHealth < 0 ){
			iHealth = 0;
		}
	}

	//update stats also if player has left the game, set health to 0
	Player *medic;
	for ( i = 0; i < maxclients->integer; i++ ){
		medic = ( Player * )g_entities[i].entity;
		if ( medic && medic->client && medic->isSubclassOf( Player ) && medic->coopPlayer.className == "Medic" ){
			DelayedServerCommand( medic->entnum , va( "set coop_%d %d" , ( iPlayerNum + 1 ) , iHealth ) );
		}
	}
}
*/

//================================================================
// Name:        coop_classSet
// Class:       -
//              
// Description: Sets the player class
//              
// Parameters:  Player *player , str classToSet, bool setup
//              
// Returns:     void
//              
//================================================================
void coop_classSet( Player *player , str classToSet )
{
	//[b60014] chrissstrahl - accsess coopPlayer.className only in multiplayer
	if ( player && multiplayerManager.inMultiplayer() && game.coop_isActive )
	{	
		if ( !Q_stricmp( "current", classToSet.c_str()) || !Q_stricmp( "" , classToSet.c_str()) ){
			classToSet = player->coopPlayer.className;
		}else{
			player->coopPlayer.lastTimeChangedClass = level.time;
		}

		//[b60011] chrissstrahl - make sure the right class is set
		if (Q_stricmpn("h", classToSet.c_str(), 1) == 0) {
			classToSet = COOP_CLASS_NAME_HEAVYWEAPONS;
		}
		else if (Q_stricmpn("m", classToSet.c_str(), 1) == 0) {
			classToSet = COOP_CLASS_NAME_MEDIC;
		}
		else {
			classToSet = COOP_CLASS_NAME_TECHNICIAN;
		}

		player->coopPlayer.className = classToSet;
	}
}



//================================================================
// Name:        classApplay
// Class:       -
//              
// Description: handles class related player appliances
//              
// Parameters:  Player *player, bool setup
//              
// Returns:     void
//              
//================================================================
void coop_classApplayAttributes( Player *player , bool changeOnly )
{
	//[b60011] chrissstrahl - added prevention of class being executed before player setup is complete
	//[b60014] chrissstrahl - handle only in multiplayer
	if (	!player ||
			!multiplayerManager.inMultiplayer() ||
			multiplayerManager.isPlayerSpectator( player ) ||
			level.time < mp_warmUpTime->integer ||
			!player->coopPlayer.setupComplete)
	{
		return;
	}
	coop_classSet( player , "current" );
	str currentClass = player->coopPlayer.className;

	//hzm coop mod chrissstrahl - we do no longer attach model, class is now shown with the player name when targeted
	//hzm coop mod chrissstrahl - remove class indicator model
	//player->removeAttachedModelByTargetname( "globalCoop_playerClassIndicator" );

	//hzm coop mod chrissstrahl - definy class setup variables
	float classMaxHealth = 100.0f;
	float classStartArmor = 20.0f;
	int classMaxAmmoPhaser = 1000;
	int classMaxAmmoFed = 400;
	int classMaxAmmoPlasma = 400;
	int classMaxAmmoIdryll = 400;
	int classGiveAmmoPhaser = 0;
	int classGiveAmmoFed = 0;
	int classGiveAmmoPlasma = 0;
	int classGiveAmmoIdryll = 0;

	//[b60012] chrissstrahl - fix missing .c_str()
	if ( !Q_stricmp( currentClass.c_str(), COOP_CLASS_NAME_MEDIC) ){
		classMaxHealth		= COOP_CLASS_MEDIC_MAX_HEALTH;
		classStartArmor		= COOP_CLASS_MEDIC_START_ARMOR;
		classMaxAmmoPhaser	= COOP_CLASS_MEDIC_MAX_AMMO_PHASER;
		classMaxAmmoFed		= COOP_CLASS_MEDIC_MAX_AMMO_FED;
		classMaxAmmoPlasma	= COOP_CLASS_MEDIC_MAX_AMMO_PLASMA;
		classMaxAmmoIdryll	= COOP_CLASS_MEDIC_MAX_AMMO_IDRYLLUM;
		player->gravity		= COOP_CLASS_MEDIC_GRAVITY;
		player->mass		= COOP_CLASS_MEDIC_MASS;
	}
	else if ( !Q_stricmp( currentClass.c_str(), COOP_CLASS_NAME_HEAVYWEAPONS) ){
		classMaxHealth		= COOP_CLASS_HEAVYWEAPONS_MAX_HEALTH;
		classStartArmor		= COOP_CLASS_HEAVYWEAPONS_START_ARMOR;
		classMaxAmmoPhaser	= COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_PHASER;
		classMaxAmmoFed		= COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_FED;
		classMaxAmmoPlasma	= COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_PLASMA;
		classMaxAmmoIdryll	= COOP_CLASS_HEAVYWEAPONS_MAX_AMMO_IDRYLLUM;
		player->gravity		= COOP_CLASS_HEAVYWEAPONS_GRAVITY;
		player->mass		= COOP_CLASS_HEAVYWEAPONS_MASS;
	}
	else{ //technician
		currentClass = COOP_CLASS_NAME_TECHNICIAN;
		classMaxHealth		= COOP_CLASS_TECHNICIAN_MAX_HEALTH;
		classStartArmor		= COOP_CLASS_TECHNICIAN_START_ARMOR;
		classMaxAmmoPhaser	= COOP_CLASS_TECHNICIAN_MAX_AMMO_PHASER;
		classMaxAmmoFed		= COOP_CLASS_TECHNICIAN_MAX_AMMO_FED;
		classMaxAmmoPlasma	= COOP_CLASS_TECHNICIAN_MAX_AMMO_PLASMA;
		classMaxAmmoIdryll	= COOP_CLASS_TECHNICIAN_MAX_AMMO_IDRYLLUM;
		player->gravity		= COOP_CLASS_TECHNICIAN_GRAVITY;
		player->mass		= COOP_CLASS_TECHNICIAN_MASS;
	}

	//hzm coop mod chrissstrahl - used to store last player mass value, needed to restore it after player has been revived
	player->coopPlayer.lastMass = player->mass;

	//hzm coop mod chrissstrahl - set player class, sets default if none stored at the client side cvar
	//hzm coop mod chrissstrahl - make sure the value is applied (again)
	player->entityVars.SetVariable( "globalclassName" , currentClass.c_str() );

	//hzm coop mod chrissstrahl - set health
	player->setMaxHealth( classMaxHealth );

	//hzm coop mod chrissstrahl - set player class, sets default if none stored at the client side cvar
	player->entityVars.SetVariable( "globalclassName" , currentClass.c_str() );

	//hzm coop mod chrissstrahl - add a background shader to the hud, this shows the player his current class
	if ( player->coopPlayer.installed ) {
		//DelayedServerCommand( player->entnum , va( "globalwidgetcommand classBg shader coop_%s" , currentClass.c_str() ) );
		DelayedServerCommand( player->entnum , va( "exec coop_mod/cfg/%s.cfg" , currentClass.c_str() ) );
	}

	//hzm coop mod chrissstrahl - make sure we don't give more than we can
	if ( classMaxAmmoPhaser < classGiveAmmoPhaser )	{ classGiveAmmoPhaser	= classMaxAmmoPhaser; }
	if ( classMaxAmmoFed < classGiveAmmoFed )		{ classGiveAmmoFed		= classMaxAmmoFed; }
	if ( classMaxAmmoPlasma < classGiveAmmoPlasma )	{ classGiveAmmoPlasma	= classMaxAmmoPlasma; }
	if ( classMaxAmmoIdryll < classGiveAmmoIdryll )	{ classGiveAmmoIdryll	= classMaxAmmoIdryll; }

	//hzm coop mod chrissstrahl - give some starting ammo
	player->GiveAmmo( "Phaser" , 0 , true , classMaxAmmoPhaser );
	player->GiveAmmo( "Fed" , 0 , true , classMaxAmmoFed );
	player->GiveAmmo( "Plasma" , 0 , true , classMaxAmmoPlasma );
	player->GiveAmmo( "Idryll" , 0 , true , classMaxAmmoIdryll );

	int i = program.getFloatVariableValue( "coop_integer_startingArmor" );
	if ( i > 0 ){
		classStartArmor = i;
	}
		
	//hzm coop mod chrissstrahl - if player has no or low armor, give armor
	//float fArmorCurrent = player->GetArmorValue();
	//if ( fArmorCurrent < classStartArmor ){
		//Event *armorEvent;
		//armorEvent = new Event( EV_Sentient_GiveArmor );
		//armorEvent->AddString( "BasicArmor" );
		//armorEvent->AddInteger( ( classStartArmor - fArmorCurrent ) );
		//player->ProcessEvent( armorEvent );
	//}

	//hzm coop mod chrissstrahl - display info to player
	if ( !player->coopPlayer.installed ){
		player->hudPrint( va( "^5Coop:^2 You are now: ^5$$%s$$\n" , currentClass.c_str() ) );
	}

	//update statistic for all players
	coop_classUpdateClassStats();

	//prevent player from freezing each other with the idryll staff
	Event *event;
	event = new Event( EV_Sentient_AddResistance );
	event->AddString( "stasis" );
	event->AddInteger( 100 );
	player->ProcessEvent( event );

	//[b60013] chrissstrahl - added to have more control over attributes changing
	ExecuteThread("coop_justChangedClass", true, (Entity*)player);
}


//================================================================
// Name:        coop_classPlayerUsed
// Class:       -
//              
// Description: Called when a player gets used ( usebutton or tricorder)
//              
// Parameters:  Player *usedPlayer , Player *usingPlayer , Equipment *equipment
//              
// Returns:     bool
//              
//================================================================
void coop_classPlayerUsed( Player *usedPlayer , Player *usingPlayer , Equipment *equipment )
{
	//[b60014] chrissstrahl - handle only in multiplayer
	if (	usedPlayer && usingPlayer &&
			multiplayerManager.inMultiplayer() &&
			!multiplayerManager.isPlayerSpectator( usedPlayer ) &&
			usedPlayer->health > 0.0f )
	{
		float fMessageTime = 2.0f;

		//get current weapon name
		str sEquipment = "USE";
		if ( equipment ){
			sEquipment = equipment->getTypeName();
		}

		//check if player was constantly revived or if it stopped
		if ( usedPlayer->coopPlayer.neutralized == true && !equipment ){ //[b60011] chrissstrahl - fixed forbidden check practise
			if ( ( usedPlayer->coopPlayer.lastTimeRevived + 0.1f ) > level.time ){
				usedPlayer->coopPlayer.lastTimeRevived = level.time;
				usedPlayer->coopPlayer.reviveCounter = 0;
				return;
			}
			fMessageTime = 1.0f;
			usedPlayer->coopPlayer.reviveCounter++;
		}

		//check if player is modulating, do not allow the class modulation
		if ( (usingPlayer->coopPlayer.lastTimeUsing + 0.1 ) > level.time ){
			return;
		}

		//print this on the player that has been used
		if ( ( usedPlayer->coopPlayer.lastTimeUsedClassMsg + fMessageTime ) < level.time ){
			usedPlayer->coopPlayer.lastTimeUsedClassMsg = level.time;

			if ( usedPlayer->coopPlayer.neutralized == true && !equipment ){ //[b60011] chrissstrahl - fixed forbidden check practise
				if ( coop_checkPlayerLanguageGerman(usedPlayer) ){
					usedPlayer->hudPrint( va( "^5COOP^8 - You are beeing revived by: %s \n" , usingPlayer->client->pers.netname ) );
				}else{
					usedPlayer->hudPrint( va( "^5COOP^8 - Sie werden wiederbelebt von: %s \n" , usingPlayer->client->pers.netname ) );
				}
			}
			else{
				if ( equipment ){
					if ( !Q_stricmp( usingPlayer->coopPlayer.className, COOP_CLASS_NAME_MEDIC) ){
						if ( usedPlayer->health >= usedPlayer->max_health ){
							return;
						}

						if ( coop_checkPlayerLanguageGerman(usedPlayer) ){
							usedPlayer->hudPrint( va( "^5COOP^8 - Sie wurden geheilt von: %s\n" , usingPlayer->client->pers.netname ) );
						}else{
							usedPlayer->hudPrint( va( "^5COOP^8 - You have been healed by: %s\n" , usingPlayer->client->pers.netname ) );
						}

						//give full health
						usedPlayer->health = usedPlayer->max_health;
					}
					else if ( !Q_stricmp( usingPlayer->coopPlayer.className, COOP_CLASS_NAME_TECHNICIAN) ){
						float fArmorToGive = COOP_MAX_ARMOR_TO_GIVE;
						float fArmorToGive_max = COOP_MAX_ARMOR_TO_GIVE;
						float fArmorCurrent = usedPlayer->GetArmorValue();
						if ( fArmorCurrent >= COOP_MAX_ARMOR ){
							return;
						}
						
						//do not give more than 200 armor
						if ( ( fArmorCurrent + fArmorToGive_max ) > COOP_MAX_ARMOR ){
							fArmorToGive = fArmorToGive_max;
							fArmorToGive -= ( ( fArmorCurrent + fArmorToGive_max ) - COOP_MAX_ARMOR );
						}

						if ( coop_checkPlayerLanguageGerman(usedPlayer) ){//CHEKME
							usedPlayer->hudPrint( va( "^5COOP^8 - Ihr Schild wurde aufgeladen von: %s\n" , usingPlayer->client->pers.netname ) );
						}else{
							usedPlayer->hudPrint( va( "^5COOP^8 - You shield was charged by: %s\n" , usingPlayer->client->pers.netname ) );
						}
						Event *armorEvent;
						armorEvent = new Event( EV_Sentient_GiveArmor );
						armorEvent->AddString( "BasicArmor" );
						armorEvent->AddInteger( fArmorToGive );
						usedPlayer->ProcessEvent( armorEvent );
					}
					else{
						if ( coop_checkPlayerLanguageGerman(usedPlayer) ){
							usedPlayer->hudPrint( va( "^5COOP^8 - Ihre Waffen wurden geladen von: %s\n" , usingPlayer->client->pers.netname ) );
						}else{
							usedPlayer->hudPrint( va( "^5COOP^8 - Your Weapons have been charged by: %s\n" , usingPlayer->client->pers.netname ) );
						}
						Event *ammoEvent;
						ammoEvent = new Event( EV_Sentient_GiveAmmoOverTime );
						ammoEvent->AddString( "Fed" );
						ammoEvent->AddInteger( COOP_MAX_AMMO_TO_GIVE_FED );
						ammoEvent->AddFloat( 1.5 );
						usedPlayer->ProcessEvent( ammoEvent );
						ammoEvent = new Event( EV_Sentient_GiveAmmoOverTime );
						ammoEvent->AddString( "Plasma" );
						ammoEvent->AddInteger( COOP_MAX_AMMO_TO_GIVE_PLASMA );
						ammoEvent->AddFloat( 1.5 );
						usedPlayer->ProcessEvent( ammoEvent );
						ammoEvent = new Event( EV_Sentient_GiveAmmoOverTime );
						ammoEvent->AddString( "Idryll" );
						ammoEvent->AddInteger( COOP_MAX_AMMO_TO_GIVE_IDRYLL );
						ammoEvent->AddFloat( 1.5 );
						usedPlayer->ProcessEvent( ammoEvent );
						//PostEvent( ammoEvent , level.frametime );
					}
				}
				/*
				else{
					if ( coop_checkPlayerLanguageGerman(usedPlayer) ){
						usedPlayer->hudPrint( va( "^5COOP^8 - Sie wurden benutzt von: %s [%s]\n" , usingPlayer->client->pers.netname , sEquipment.c_str() ) );
					}
					else{
						usedPlayer->hudPrint( va( "^5COOP^8 - You have been used by: %s [%s]\n" , usingPlayer->client->pers.netname , sEquipment.c_str() ) );
					}
				}
				*/
			}

		}
		//print this on the hud of the player that was using
		if ( ( usingPlayer->coopPlayer.lastTimeUsedClassMsg + fMessageTime ) < level.time ){
			usingPlayer->coopPlayer.lastTimeUsedClassMsg = level.time;

			if ( usingPlayer->coopPlayer.neutralized == true && !equipment ){//[b60011] chrissstrahl - fixed forbidden check practise
				if ( coop_checkPlayerLanguageGerman((usedPlayer)) ){
					usingPlayer->hudPrint( va( "^5COOP^8 - Wiederbeleben von: %s, bitte weitermachen!\n" , usedPlayer->client->pers.netname ) );
				}else{
					usingPlayer->hudPrint( va( "^5COOP^8 - You are reviving: %s, please continue!\n" , usedPlayer->client->pers.netname ) );
				}	
			}
			else{
				if ( equipment ){
					//[b60012] chrissstrahl - fix missing .c_str()
					if ( !Q_stricmp( usingPlayer->coopPlayer.className.c_str(), COOP_CLASS_NAME_MEDIC) ){
						if ( usedPlayer->health >= usedPlayer->max_health ){

							//[b60012][cleanup] chrissstrahl - this could be put into a func
							if ( coop_checkPlayerLanguageGerman((usingPlayer)) ){//[b607] chrissstrahl - using now correct entity
								usingPlayer->hudPrint( "^5COOP^8 - Spieler bereits bei voller Gesundheit!\n" );
							}
							else{
								usingPlayer->hudPrint( "^5COOP^8 - Player already at full health!\n" );
							}
							return;
						}

						//[b60012][cleanup] chrissstrahl - this could be put into a func
						if ( coop_checkPlayerLanguageGerman(( usingPlayer )) ){
							usingPlayer->hudPrint( va( "^5COOP^8 - Sie heilten: %s\n" , usedPlayer->client->pers.netname ) );
						}else{
							usingPlayer->hudPrint( va( "^5COOP^8 - You healed: %s\n" , usedPlayer->client->pers.netname ) );
						}

						//[b60014] chrissstrahl - give player health in return for his cooperation
						if(usingPlayer->health < COOP_CLASS_MEDIC_MAX_HEALTH){
							//give player 15% health back for his cooperation
							float playerHealthNew = (usingPlayer->health + ((COOP_CLASS_MEDIC_MAX_HEALTH / 100) * 15));
							if (playerHealthNew > COOP_CLASS_MEDIC_MAX_HEALTH) {
								playerHealthNew = COOP_CLASS_MEDIC_MAX_HEALTH;
							}
							usingPlayer->health = playerHealthNew;
							
							//if player has health below 50 up it to 50
							if (usingPlayer->health < ((COOP_CLASS_MEDIC_MAX_HEALTH / 100) * 50)) {
								usingPlayer->health = ((COOP_CLASS_MEDIC_MAX_HEALTH / 100) * 50);
							}
						}
					}
					else if ( !Q_stricmp( usingPlayer->coopPlayer.className.c_str(), COOP_CLASS_NAME_TECHNICIAN) ){
						if ( usedPlayer->GetArmorValue() >= COOP_MAX_ARMOR ){

							//[b60012][cleanup] chrissstrahl - this could be put into a func
							if ( coop_checkPlayerLanguageGerman((usingPlayer)) ){//[b607] chrissstrahl - using now correct entity
								usingPlayer->hudPrint( va( "^5COOP^8 - %ss Schild ist bereits bei maximler Kapazitaet\n" , usedPlayer->client->pers.netname ) );
							}else{
								usingPlayer->hudPrint( va( "^5COOP^8 - %ss Shield is already at maximum capacity\n" , usedPlayer->client->pers.netname ) );
							}
							return;
						}

						//[b60012][cleanup] chrissstrahl - this could be put into a func
						if ( coop_checkPlayerLanguageGerman(( usingPlayer )) ){
							usingPlayer->hudPrint( va( "^5COOP^8 - Sie luden %ss Schild auf\n" , usedPlayer->client->pers.netname ) );
						}else{
							usingPlayer->hudPrint( va( "^5COOP^8 - You charged %ss shield\n" , usedPlayer->client->pers.netname ) );
						}
					}
					else{

						//[b60012][cleanup] chrissstrahl - this could be put into a func
						if ( coop_checkPlayerLanguageGerman(( usingPlayer )) ){
							usingPlayer->hudPrint( va( "^5COOP^8 - Sie luden %ss Waffenenergie auf\n" , usedPlayer->client->pers.netname ) );
						}else{
							usingPlayer->hudPrint( va( "^5COOP^8 - You charged %ss ammo\n" , usedPlayer->client->pers.netname ) );
						}
					}
				}
				/*
				else{
					if ( coop_checkPlayerLanguageGerman(usingPlayer) ){
						usingPlayer->hudPrint( va( "^5COOP^8 - Gebraucht: %s [%s]\n" , usedPlayer->client->pers.netname , sEquipment.c_str() ) );
					}else{
						usingPlayer->hudPrint( va( "^5COOP^8 - USED: %s [%s]\n" , usedPlayer->client->pers.netname , sEquipment.c_str() ) );
					}
				}
				*/
			}
		}

		//player has been sucessfully revived
		if ( usedPlayer->coopPlayer.reviveCounter >= COOP_REVIVE_TIME ){
			usedPlayer->coopPlayer.reviveCounter = 0;
			usedPlayer->health = usedPlayer->max_health;
		}
	}
}

//================================================================
// Name:        coop_classPlayersOfClass
// Class:       -
//              
// Description: used to get the number of active players for a certain class
//              
// Parameters:  classname
//              
// Returns:     int
//              
//================================================================
int coop_classPlayersOfClass(str className)
{
	if ( className.length() < 1 )
		return -1;

	//[b60014] chrissstrahl - return default value if not in multiplayer or coop
	if (!multiplayerManager.inMultiplayer() || game.coop_isActive) {
		return 0;
	}

	str temp;
	temp = className.tolower();

	if ( temp[0] == 'm' )
		temp = COOP_CLASS_NAME_MEDIC;
	else if ( temp[0] == 'h' )
		temp = COOP_CLASS_NAME_HEAVYWEAPONS;
	else
		temp = COOP_CLASS_NAME_TECHNICIAN;
	
	gentity_t   *other;
	Player      *player;
	int i;
	int iActive=0;

	for ( i = 0; i < game.maxclients;i++ ){
		other = &g_entities[i];
		if ( other->inuse && other->client ){
			player = ( Player * )other->entity;
			if (	player &&
					player->health >= 1 &&
					player->coopPlayer.className == temp &&
					!multiplayerManager.isPlayerSpectator( player ) )
			{
				iActive++;
			}
		}
	}
	return iActive;
}



//================================================================
// Name:        coop_classUpdateClassStats
// Class:       -
//              
// Description: used to update statisics of player classes for all players
//              
// Parameters:  classname
//              
// Returns:     int
//              
//================================================================
void coop_classUpdateClassStats( void )
{
	//hzm coop mod chrissstrahl - set current time, so the client think function can send class statistics to each client
	game.coop_classInfoSendAt = level.time;
}



