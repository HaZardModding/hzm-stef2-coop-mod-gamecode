//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER CLASS(Medic,Technician,HeavyWeapons) HANDLING RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

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

Event EV_Player_RecoverAbilityHud
(
	"recoverAbilityHud",
	EV_DEFAULT,
	"s",
	"string-widgetname",
	"Recovers the class ability widget displayed at coop classhud"
);


//[b60021] chrissstrahl
//================================================================
// handle restoration of ability hud bars
//================================================================
void Player::coopClassRecoverAbilityHud(Event* ev)
{
	//make sure we have at least widgetname and a commandparameter
	if (ev->NumArgs() < 1) {
		return;
	}
	upgPlayerWidgetCommand(ev->GetString(1), "enable");
}

//========================================================[b60021]
// Name:        coop_classPlayerKilled
// Class:       Player
//              
// Description: Handles the ability stuff on player death
// 
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_classPlayerKilled()
{
	entityVars.SetVariable("!ability", (level.time - COOP_CLASS_REGENERATION_COOLDOWN) - 1);
	entityVars.SetVariable("!abilityPrintout", (level.time + 3));
	coopPlayer.regenerationCycles = 0;

	//cancel events of type
	CancelEventsOfType(EV_Player_RecoverAbilityHud);
}

//========================================================[b60021]
// Name:        coop_classAbilityRecoverHud
// Class:       Player
//              
// Description: Handles the ability revovery bar on the coop class hud
//				Enables each widget timmed
//				also restored via "ea.cfg"
// 
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_classAbilityRecoverHud()
{
	Event* recover25 = new Event(EV_Player_RecoverAbilityHud);
	recover25->AddString("coop_class25");
	//recover25->AddString("enable");
	PostEvent(recover25, (COOP_CLASS_REGENERATION_COOLDOWN / 4) * 1);

	Event* recover50 = new Event(EV_Player_RecoverAbilityHud);
	recover50->AddString("coop_class50");
	//recover50->AddString("enable");
	PostEvent(recover50, (COOP_CLASS_REGENERATION_COOLDOWN / 4) * 2);

	Event* recover75 = new Event(EV_Player_RecoverAbilityHud);
	recover75->AddString("coop_class75");
	//recover75->AddString("enable");
	PostEvent(recover75, (COOP_CLASS_REGENERATION_COOLDOWN / 4) * 3);

	Event* recover100 = new Event(EV_Player_RecoverAbilityHud);
	recover100->AddString("coop_class100");
	//recover100->AddString("enable");
	PostEvent(recover100, COOP_CLASS_REGENERATION_COOLDOWN - 0.05); // make sure the hud reappaers just a lil bit earlier so it won't overlap
}

//========================================================[b60021]
// Name:        coop_classAbilityUse
// Class:       Player
//              
// Description: Player wants to use his class ability
// 
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_classAbilityUse()
{
	if (!game.coop_isActive) {
		return;
	}

	//use the regeneration function to recover health to all players
	//coop_classRegenerate
	coop_classRegenerationCycleSet();

	//hide the ability status bars
	upgPlayerDelayedServerCommand(entnum, "exec coop_mod/cfg/coop_classResetA.cfg");
	//make the status bars reappear over time
	coop_classAbilityRecoverHud();
}

//========================================================[b60017]
// Name:        coop_classRegenerationCycleSet
// Class:       Player
//              
// Description: Updates the cycles for regenerations
// 
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_classRegenerationCycleSet()
{
	coopPlayer.regenerationCycles = COOP_CLASS_REGENERATION_CYCLES;
}

//========================================================[b60017]
// Name:        coop_classRegenerationCycleGet
// Class:       Player
//              
// Description: Get the cycles for regenerations
// 
// Parameters:  void
//              
// Returns:     int
//              
//================================================================
int Player::coop_classRegenerationCycleGet()
{
	return coopPlayer.regenerationCycles;
}

//=========================================================[b60017]
// Name:        coop_classRegenerationCycleSubstract
// Class:       Player
//              
// Description: Substract from the cycles for regenerations
// 
// Parameters:  void
//              
// Returns:     void
//              
//================================================================
void Player::coop_classRegenerationCycleSubstract()
{
	coopPlayer.regenerationCycles--;
}

//================================================================
// Name:        coop_classCheckApplay
// Class:       -
//              
// Description: Checks and applies the selected class upon player if it should
//              SHALL NOT BE USED IN [SINGLEPLAYER]
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
void coop_classCheckUpdateStat( Player *player )
{
	//[b60014] chrissstrahl //[b60015] chrissstrahl - fixed bad check condition
	if (multiplayerManager.inMultiplayer() && game.coop_classInfoSendAt > player->coopPlayer.lastTimeUpdatedClassStat ){
		if ( player->coop_getInstalled() && player->coop_playerSetupComplete()) {
			player->coopPlayer.lastTimeUpdatedClassStat = game.coop_classInfoSendAt;
			//gi.Printf(va("COOPDEBUG coop_classCheckUpdateStat sending to %s\n", player->client->pers.netname));
			
			//[b60014] chrissstrahl - fused multiple commands to one data burs
			upgPlayerDelayedServerCommand( player->entnum , va( "set coop_ch %i;set coop_ct %i;set coop_cm %i\n" ,coop_classPlayersOfClass( "HeavyWeapon" ),coop_classPlayersOfClass( "Technician" ),coop_classPlayersOfClass( "Medic" )));
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
			!game.coop_isActive ||
			player->coop_playerNeutralized() //||
			//(player->upgPlayerGetLastDamageTime() + COOP_CLASS_HURT_WAITTIME) > level.time
			)
	{
		return;
	}

	//[b60017] chrissstrahl - changed regeneration that a player gets some ammount of regeneration cycles for him self after player gave something to other player class
	if (player->coop_classRegenerationCycleGet() > 0) {
		player->coop_classRegenerationCycleSubstract();

		Entity* eOther = nullptr;

		for (int i = 0; i < maxclients->integer; i++) {
			eOther = (Player*)g_entities[i].entity;
			if (!eOther || !eOther->isSubclassOf(Player)) {
				continue;
			}

			Player* other = (Player*)eOther;

			//criteria that prevent player getting regenerated
			if (other->upgPlayerIsBot() || other->getHealth() < 1 || other->upgPlayerIsBot() || multiplayerManager.isPlayerSpectator((Player*)other)) {
				continue;
			}

			//medic
			if (!Q_stricmp(player->coopPlayer.className.c_str(), COOP_CLASS_NAME_MEDIC)) {
				other->AddHealth(COOP_CLASS_REGENERATE_HEALTH);
			}
			//technician
			else if (!Q_stricmp(player->coopPlayer.className.c_str(), COOP_CLASS_NAME_TECHNICIAN)) {
				float fArmorCurrent = other->GetArmorValue();
				if ((fArmorCurrent + COOP_CLASS_REGENERATE_ARMOR) <= COOP_MAX_ARMOR) {
					fArmorCurrent++;
					Event* armorEvent;
					armorEvent = new Event(EV_Sentient_GiveArmor);
					armorEvent->AddString("BasicArmor");
					armorEvent->AddInteger(COOP_CLASS_REGENERATE_ARMOR);
					other->ProcessEvent(armorEvent);
				}
			}
			//heavy weapons
			else if (!Q_stricmp(player->coopPlayer.className.c_str(), COOP_CLASS_NAME_HEAVYWEAPONS)) {
				other->GiveAmmo("Fed", COOP_CLASS_REGENERATE_AMMO, false, COOP_MAX_HW_AMMO_FED);
				other->GiveAmmo("Plasma", COOP_CLASS_REGENERATE_AMMO, false, COOP_MAX_HW_AMMO_PLASMA);
				other->GiveAmmo("Idryll", COOP_CLASS_REGENERATE_AMMO, false, COOP_MAX_HW_AMMO_IDRYLL);
			}
		}
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
//              SHALL NOT BE USED IN [SINGLEPLAYER]
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

		gi.Printf(va("%s: %s\n", classToSet.c_str(), player->client->pers.netname));
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
			!player->coop_playerSetupComplete())
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

	//[b60021] chrissstrahl - circle menu related
	str circleText1 = "";
	str circleImg1 = "";

	//[b60012] chrissstrahl - fix missing .c_str()
	if ( !Q_stricmp( currentClass.c_str(), COOP_CLASS_NAME_MEDIC) ){
		//[b60021] chrissstrahl - handle circle menu
		circleImg1 = COOP_CLASS_MEDIC_ICON;
		if (player->upgPlayerHasLanguageGerman()) {circleText1 = COOP_TEXT_CLASS_MEDIC_ABILITY_DEU;}
		else {circleText1 = COOP_TEXT_CLASS_MEDIC_ABILITY_ENG;}

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
		//[b60021] chrissstrahl - handle circle menu
		circleImg1 = COOP_CLASS_HEAVYWEAPONS_ICON;
		if (player->upgPlayerHasLanguageGerman()) { circleText1 = COOP_TEXT_CLASS_HEAVYWEAPONS_ABILITY_DEU; }
		else { circleText1 = COOP_TEXT_CLASS_HEAVYWEAPONS_ABILITY_ENG; }

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
		//[b60021] chrissstrahl - handle circle menu
		circleImg1 = COOP_CLASS_TECHNICIAN_ICON;
		if (player->upgPlayerHasLanguageGerman()) { circleText1 = COOP_TEXT_CLASS_TECHNICIAN_ABILITY_DEU; }
		else { circleText1 = COOP_TEXT_CLASS_TECHNICIAN_ABILITY_ENG; }

		currentClass		= COOP_CLASS_NAME_TECHNICIAN;
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

	//hzm coop mod chrissstrahl - set health
	player->setMaxHealth( classMaxHealth );

	//hzm coop mod chrissstrahl - add a background shader to the hud, this shows the player his current class
	if ( player->coop_getInstalled() ) {
		//[b60021] chrissstrahl - add circlemenu features
		Event* evCircleSet1;
		evCircleSet1 = new Event(EV_Player_circleMenuSet);
		evCircleSet1->AddInteger(1);
		evCircleSet1->AddString(circleText1.c_str());
		evCircleSet1->AddString("!ability");
		evCircleSet1->AddString(circleImg1.c_str());
		evCircleSet1->AddInteger(0);
		player->ProcessEvent(evCircleSet1);

		//DelayedServerCommand( player->entnum , va( "globalwidgetcommand classBg shader coop_%s" , currentClass.c_str() ) );
		upgPlayerDelayedServerCommand( player->entnum , va( "exec coop_mod/cfg/%s.cfg" , currentClass.c_str() ) );
	}
	else {
		player->hudPrint( va( "^5Coop:^2 You are now: ^5$$%s$$\n" , currentClass.c_str() ) );
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
		if ( usedPlayer->coop_playerNeutralized() && !equipment ){ //[b60011] chrissstrahl - fixed forbidden check practise
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

			if ( usedPlayer->coop_playerNeutralized() && !equipment ){ //[b60011] chrissstrahl - fixed forbidden check practise
				if ( usedPlayer->upgPlayerHasLanguageGerman() ){
					usedPlayer->hudPrint( va( "^5COOP^8 - You are beeing revived by: %s \n" , usingPlayer->client->pers.netname ) );
				}else{
					usedPlayer->hudPrint( va( "^5COOP^8 - Sie werden wiederbelebt von: %s \n" , usingPlayer->client->pers.netname ) );
				}
			}
			else{
				if ( equipment ){
					//[b60021] chrissstrahl - we are using the ability now, makes transition easier to the new way of doing it
					upgPlayerDelayedServerCommand(usingPlayer->entnum, "!ability");

					/*if ( !Q_stricmp( usingPlayer->coopPlayer.className, COOP_CLASS_NAME_MEDIC) ){
						if ( usedPlayer->health >= usedPlayer->max_health ){
							return;
						}

						if ( usedPlayer->upgPlayerHasLanguageGerman() ){
							usedPlayer->hudPrint( va( "^5COOP^8 - Sie wurden geheilt von: %s\n" , usingPlayer->client->pers.netname ) );
						}else{
							usedPlayer->hudPrint( va( "^5COOP^8 - You have been healed by: %s\n" , usingPlayer->client->pers.netname ) );
						}

						//give full health
						usedPlayer->health = usedPlayer->max_health;

						//[b60017] chrissstrahl - changed regeneration that a player gets some ammount of regeneration cycles for him self after player gave something to other player class
						usingPlayer->coop_classRegenerationCycleSet();
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

						if ( usedPlayer->upgPlayerHasLanguageGerman() ){//CHEKME
							usedPlayer->hudPrint( va( "^5COOP^8 - Ihr Schild wurde aufgeladen von: %s\n" , usingPlayer->client->pers.netname ) );
						}else{
							usedPlayer->hudPrint( va( "^5COOP^8 - You shield was charged by: %s\n" , usingPlayer->client->pers.netname ) );
						}
						Event *armorEvent;
						armorEvent = new Event( EV_Sentient_GiveArmor );
						armorEvent->AddString( "BasicArmor" );
						armorEvent->AddInteger( fArmorToGive );
						usedPlayer->ProcessEvent( armorEvent );

						//[b60017] chrissstrahl - changed regeneration that a player gets some ammount of regeneration cycles for him self after player gave something to other player class
						usingPlayer->coop_classRegenerationCycleSet();
						
					}
					else{
						if ( usedPlayer->upgPlayerHasLanguageGerman() ){
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

						//[b60017] chrissstrahl - changed regeneration that a player gets some ammount of regeneration cycles for him self after player gave something to other player class
						usingPlayer->coop_classRegenerationCycleSet();
					}*/
				}
				else{
					/*if ( coop_checkPlayerLanguageGerman(usedPlayer) ){
						usedPlayer->hudPrint( va( "^5COOP^8 - Sie wurden benutzt von: %s [%s]\n" , usingPlayer->client->pers.netname , sEquipment.c_str() ) );
					}
					else{
						usedPlayer->hudPrint( va( "^5COOP^8 - You have been used by: %s [%s]\n" , usingPlayer->client->pers.netname , sEquipment.c_str() ) );
					}*/
				}
				
			}

		}
		//print this on the hud of the player that was using
		if ( ( usingPlayer->coopPlayer.lastTimeUsedClassMsg + fMessageTime ) < level.time ){
			usingPlayer->coopPlayer.lastTimeUsedClassMsg = level.time;

			if ( usingPlayer->coop_playerNeutralized() && !equipment ){//[b60011] chrissstrahl - fixed forbidden check practise
				if ( usedPlayer->upgPlayerHasLanguageGerman() ){
					usingPlayer->hudPrint( va( "^5COOP^8 - Wiederbeleben von: %s, bitte weitermachen!\n" , usedPlayer->client->pers.netname ) );
				}else{
					usingPlayer->hudPrint( va( "^5COOP^8 - You are reviving: %s, please continue!\n" , usedPlayer->client->pers.netname ) );
				}	
			}
			else{
				if ( equipment ){
					//[b60021] chrissstrahl - we are using the ability now, makes transition easier to the new way of doing it
					upgPlayerDelayedServerCommand(usingPlayer->entnum, "!ability");

					/*if ( !Q_stricmp( usingPlayer->coopPlayer.className.c_str(), COOP_CLASS_NAME_MEDIC) ){
						if ( usedPlayer->health >= usedPlayer->max_health ){

							//[b60012][cleanup] chrissstrahl - this could be put into a func
							if ( usingPlayer->upgPlayerHasLanguageGerman() ){//[b607] chrissstrahl - using now correct entity
								usingPlayer->hudPrint( "^5COOP^8 - Spieler bereits bei voller Gesundheit!\n" );
							}
							else{
								usingPlayer->hudPrint( "^5COOP^8 - Player already at full health!\n" );
							}
							return;
						}

						//[b60012][cleanup] chrissstrahl - this could be put into a func
						if ( usingPlayer->upgPlayerHasLanguageGerman() ){
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
							if ( usingPlayer->upgPlayerHasLanguageGerman() ){//[b607] chrissstrahl - using now correct entity
								usingPlayer->hudPrint( va( "^5COOP^8 - %ss Schild ist bereits bei maximler Kapazitaet\n" , usedPlayer->client->pers.netname ) );
							}else{
								usingPlayer->hudPrint( va( "^5COOP^8 - %ss Shield is already at maximum capacity\n" , usedPlayer->client->pers.netname ) );
							}
							return;
						}

						//[b60012][cleanup] chrissstrahl - this could be put into a func
						if ( usingPlayer->upgPlayerHasLanguageGerman() ){
							usingPlayer->hudPrint( va( "^5COOP^8 - Sie luden %ss Schild auf\n" , usedPlayer->client->pers.netname ) );
						}else{
							usingPlayer->hudPrint( va( "^5COOP^8 - You charged %ss shield\n" , usedPlayer->client->pers.netname ) );
						}
					}
					else{

						//[b60012][cleanup] chrissstrahl - this could be put into a func
						if ( usingPlayer->upgPlayerHasLanguageGerman() ){
							usingPlayer->hudPrint( va( "^5COOP^8 - Sie luden %ss Waffenenergie auf\n" , usedPlayer->client->pers.netname ) );
						}else{
							usingPlayer->hudPrint( va( "^5COOP^8 - You charged %ss ammo\n" , usedPlayer->client->pers.netname ) );
						}
					}
				*/
				}
				else{
					/* not shown to all players - something fishy here
					if ( usingPlayer->upgPlayerHasLanguageGerman() ){
						usingPlayer->hudPrint( va( "^5COOP^8 - Gebraucht: %s [%s]\n" , usedPlayer->client->pers.netname , sEquipment.c_str() ) );
					}else{
						usingPlayer->hudPrint( va( "^5COOP^8 - USED: %s [%s]\n" , usedPlayer->client->pers.netname , sEquipment.c_str() ) );
					}*/
				}
				
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
	if (!multiplayerManager.inMultiplayer() || !game.coop_isActive) {
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



