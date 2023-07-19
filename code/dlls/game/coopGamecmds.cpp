//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// Gamecommands / Console Commands for Coop
//-----------------------------------------------------------------------------------

#ifndef __COOPGAMECMDS_CPP__
#define __COOPGAMECMDS_CPP__

#include "coopGamecmds.hpp"
#include "coopText.hpp"

#include "coopSpawnlocation.hpp"
extern CoopSpawnlocation coopSpawnlocation;


//================================================================
// Name:        G_coopClientId
// Class:       -
//              
// Description: Allow client to idendify them self
//              
// Parameters:  int entNum
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopClientId(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client || g_gametype->integer == GT_SINGLE_PLAYER)
		return qtrue;

	Player* player = (Player*)ent->entity;

	const char* cCClientId = gi.argv(1);
	str sId = va("%s", cCClientId);

	str sClientId;
	if (!sId.length()) {
		//[b60012] chrissstrahl - have a printout
		gi.Printf(va("COOPDEBUG: coop_cId - Bad or Empty: Rejected! For: %s\n",player->client->pers.netname));
		return qtrue;
	}

	if ((player->coopPlayer.timeEntered + 10) > level.time) {
		sClientId = coop_returnStringTrim(sId, " \t\r\n;[]=");
		str sClientIdNew = coop_checkPlayerCoopIdExistInIni(player, sClientId);
	}
	else {
		player->hudPrint("COOPDEBUG: coop_cId - Timed Out: Rejected!\n");
		gi.Printf(va("COOPDEBUG: coop_cId - Timed Out: Rejected! For: %s\n", player->client->pers.netname));
	}
	return qtrue;
}

//================================================================
// Name:        G_coopCom_block
// Class:       -
//              
// Description: handles player !block command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_block(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	//[b60014] chrissstrahl - print info
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//[b60014] chrissstrahl - deny request during cinematic and in spec
	if (sv_cinematic->integer || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}

	//[b60014] chrissstrahl - prevent spamming
	constexpr auto COOP_COOLDOWN_CMD_BLOCK = 9;
	//deny usage of command if player executed command to quickly
	float fCoolDownTime = coop_returnEntityFloatVar((Entity*)player, "!block");
	if ((fCoolDownTime + COOP_COOLDOWN_CMD_BLOCK) > level.time) {
		player->hudPrint(va("^5!hudprint^8, has a Cooldown please wait %d sec\n",((fCoolDownTime + COOP_COOLDOWN_CMD_BLOCK) - level.time )));
		return true;
	}
	player->entityVars.SetVariable("!block", level.time);
	
	//hzm coop mod chrissstrahl - allow to walk trugh a player that is currently blocking, this player needs to aim at the blocking player
	Entity* target;
	target = player->GetTargetedEntity();
	if ((target) && target->health > 0 && target->isSubclassOf(Player)) {
		Player* targetPlayer = (Player*)target;
		targetPlayer->setSolidType(SOLID_NOT);
		targetPlayer->_makeSolidASAPTime = (level.time + 7);
		targetPlayer->_makeSolidASAP = true;

		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint(COOP_TEXT_BLOCK_MARKED_PLAYER_DEU);
		}
		else {
			player->hudPrint(COOP_TEXT_BLOCK_MARKED_PLAYER_ENG);
		}
	}else{
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint(COOP_TEXT_BLOCK_MARKED_PLAYER_NOTFOUND_DEU);
		}
		else {
			player->hudPrint(COOP_TEXT_BLOCK_MARKED_PLAYER_NOTFOUND_ENG);
		}
	}
	return true;
}

//================================================================
// Name:        G_coopCom_class
// Class:       -
//              
// Description: handles player !class command, coop only command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_class(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	//[b60014] chrissstrahl - print info
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//[b60011] chrissstrahl - if changing classes is disabled for this player, abbort
	if (player->coopPlayer.classChangingDisabled) {
		//[b60012][cleanup] chrissstrahl - this could be put into a func
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint(COOP_TEXT_CLASS_CANT_CHANGE_ANYMORE_DEU);
		}else{
			player->hudPrint(COOP_TEXT_CLASS_CANT_CHANGE_ANYMORE_ENG);
		}

		gi.Printf(va("COOPDEBUG G_coopCom_class can't change class anymore for %s\n", player->client->pers.netname));
		return true;
	}

	int   n;

	//NO ARGUMENT GIVEN
	n = gi.argc();
	if (n == 1){
		//[b60012][cleanup] chrissstrahl - this could be put into a func
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32){
			if (coop_checkPlayerLanguageGerman(player)) {
				player->hudPrint(va("%s %s\n", COOP_TEXT_CLASS_YOUR_CLASS_IS_DEU, player->coopPlayer.className.c_str()));
			}
			else {
				player->hudPrint(va("%s %s\n", COOP_TEXT_CLASS_YOUR_CLASS_IS_ENG, player->coopPlayer.className.c_str()));
			}
		}
		return true;
	}

	//hzm coop mod chrissstrahl - remember current health/armor/ammo status
	coop_serverSaveClientData(player);

	//grab intended class
	str classSelected = gi.argv(1);
	classSelected.tolower();

	//[b60012] chrissstrahl - improved comparison	
	switch (classSelected[0]) {
	case 'h':
		classSelected = COOP_CLASS_NAME_HEAVYWEAPONS;
		break;
	case 'm':
		classSelected = COOP_CLASS_NAME_MEDIC;
		break;
	case 't':
		classSelected = COOP_CLASS_NAME_TECHNICIAN;
		break;
	default:
		classSelected = COOP_CLASS_NAME_TECHNICIAN;
		//[b60012][cleanup] chrissstrahl - this could be put into a func
		if (gi.GetNumFreeReliableServerCommands(player->entnum) >= 32) {
			if (coop_checkPlayerLanguageGerman(player)) {
				player->hudPrint(COOP_TEXT_CLASS_INVALID_CLASSNAME_DEU);
			}
			else {
				player->hudPrint(COOP_TEXT_CLASS_INVALID_CLASSNAME_ENG);
			}
		}
	}

	//hzm coop mod chrissstrahl - set new class on player
	coop_classSet(player, classSelected);
	coop_classApplayAttributes(player, true);
	
	return true;
}

//================================================================
// Name:        G_coopCom_drop
// Class:       -
//              
// Description: handles player !drop command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_drop(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - prevent spamming
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!drop") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!drop", level.time);

	//[b60014] chrissstrahl - print info
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//[b60014] chrissstrahl - deny request during cinematic and in spec
	if (sv_cinematic->integer || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}

	int i;
	for (i = 0; i < MAX_ACTIVE_WEAPONS; i++)
	{
		//hzm coop mod chrissstrahl - can't figure out hoe to convert.... :(
		//hzm coop mod chrissstrahl - daggolin figured it out, here have a cookie :P
		Weapon* weap = player->getActiveWeaponList()[i];

		if (weap)
		{
			str weaponName = "None";
			player->getActiveWeaponName(WEAPON_ANY, weaponName);

			//hzm coop mod chrissstrahl - check if the weapon can be dropped
			//[b60012] chrissstrahl - fix missing .c_str()
			if (	Q_stricmpn("None", weaponName.c_str(), 4) == 0 ||
					Q_stricmpn("EnterpriseCannon", weaponName.c_str(), 4) == 0 ||
					Q_stricmpn("Batleth", weaponName.c_str(), 4) == 0 ||
					Q_stricmpn("Phaser", weaponName.c_str(), 6) == 0 ||
					Q_stricmpn("Tricorder", weaponName.c_str(), 9) == 0)
			{
				//[b60012][cleanup] chrissstrahl - this could be put into a func
				if (coop_checkPlayerLanguageGerman(player)) {
					player->hudPrint(COOP_TEXT_WEAPON_CANT_DROP_DEU);
				}
				else {
					player->hudPrint(COOP_TEXT_WEAPON_CANT_DROP_ENG);
				}
			}
			else
			{
				weap->Drop();
				//hzm coop mod chrissstrahl - if dropped weapon still exist scale down to fit size as it was when the player held it
				if (weap) {
					weap->setScale(0.45);

					Entity* target;
					target = player->GetTargetedEntity();
					if ((target) && target->health > 0 && target->isSubclassOf(Player)) {
						Player* targetPlayer = (Player*)target;
						weap->origin = targetPlayer->origin;
					}
					else {
						trace_t trace;
						player->GetViewTrace(trace, MASK_PROJECTILE, 100.0f);
						weap->origin = trace.endpos;
					}
				}
				player->getActiveWeaponList()[i] = NULL;
				//hzm coop mod chrissstrahl - resets the empty hands of the player
				player->animate->ClearTorsoAnim();

				//[b60012][cleanup] chrissstrahl - this could be put into a func
				if (coop_checkPlayerLanguageGerman(player)) {
					player->hudPrint(COOP_TEXT_WEAPON_WAS_DROPPED_DEU);
				}
				else {
					player->hudPrint(COOP_TEXT_WEAPON_WAS_DROPPED_ENG);
				}
			}
		}
	}
	return true;
}

//================================================================
// Name:        G_coopCom_help
// Class:       -
//              
// Description: handles player !help command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_help(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	//[b60014] chrissstrahl - prevent spamming
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!help") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!help", level.time);

	//[b60014] chrissstrahl - print info
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//[b60014] chrissstrahl - deny request during cinematic and in spec
	if (sv_cinematic->integer) {
		player->hudPrint(COOP_TEXT_NOT_POSSIBLE_DURING_CINEMATIC);
		return true;
	}

	if (gi.GetNumFreeReliableServerCommands(player->entnum) < 32)
		return true;

	//[b60014] chrissstrahl - show communicator menu to player
	if (player->coop_getInstalledVersion() >= 60014) {
		gi.SendServerCommand(player->entnum, "stufftext \"set coop_comT 1;set coop_comL 10;pushmenu coop_com\"\n");
		return true;
	}

	//print to console if player does have outdated or no coop at all
	if (coop_checkPlayerLanguageGerman(player)) {
		//gi.SendConsoleCommand(va("echo %s\n", ));

		if (player->coop_playerAdmin()) {
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!noclip^8 - Schaltet keine Kollision an/aus ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!reboot^8 - Rebootet den Server sofort ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!levelend^8 - Startet die level end Funktion ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!targetnames^8 - Zeigt die Entity Targetnamen bei wechsel ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!showspawn^8 - Zeigt playerspawnspots ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!hidespawn^8 - Versteckt playerspawnspots ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!testspawn^8 - Zum Testen der playerspawnspots ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!thread^8 - Startet ein Script Thread und sets Sie als activator ^3Admin\"\n");
		}
		else {
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^3Bitte ^5!login^8 ^3nutzen um Admin Befehele zu sehen\"\n");
		}

		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!help^8 - Zeigt alle Befehle in der Konsole an\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!block^8 - Eingeben und zielen Sie auf einen blockierenden Spieler\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!class^8 - Zeigt Ihre Klasse an. Tippen Sie '^5!class Medic^8' zum wechseln\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!drop^8 - Legt Ihre aktuelle Waffe ab. Zielen Sie auf einen Spieler zum vergeben\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!kill^8 -  Tippen Sie dies zum Selbstmord\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!stuck^8 - Tippen Sie dies wenn Sie feststecken, verschiebt Sie zum Spawnpunkt\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!skill^8 - Zeigt Schwierigkeit an, tippen Sie '^5!skill 3^8' [$$VeryHard$$]. Optionen: 0 bis 3\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!transport^8 - Transportiert Sie zum angegeben Spieler. Optionen: 0 bis 8\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!mapname^8 - Zeigt Dateiname der aktuellen Karte an (*.bsp)\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!info^8 - Zeigt Ihre Informationen an.\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!build^8 - Zeigt aktuelle Build.\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!origin^8 - Zeigt Ihren und Ziel origin an.\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!login^8 - Zeigt die Coop Admin Login Schnittstelle an.\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!logout^8 - Entzieht Ihnen Coop Admin Rechte.\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!kickbots^8 - Kickt alle Bots\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!follow^8 - Zeigt nutzenden Spieler auf dem Radar\"\n");
		//gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!leader^8 - Erzwingt Teamleader Abstimmung - Parameter: Client Nummer\"\n");

		player->hudPrint(COOP_TEXT_HELP_COMMAND_LIST_PRINTED_DEU);
		player->hudPrint(COOP_TEXT_HELP_COMMAND_LIST_ENTER_TAB_DEU);
	}
	else {
		if (player->coop_playerAdmin()) {
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!noclip^8 - Turns no clipping on/off ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!reboot^8 - Reboots the Server right now ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!levelend^8 - Runs the level end function now ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!targetnames^8 - Shows entity Targetname when changing ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!showspawn^8 - Shows playerspawnspots ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!hidespawn^8 - Hides playerspawnspots ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!testspawn^8 - Used to test playerspawnspots ^3Admin\"\n");
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!thread^8 - Start script thread and sets you as activator ^3Admin\"\n");
		}
		else {
			gi.SendServerCommand(player->entnum, "stufftext \"echo ^3Use ^5!login^8 ^3to see Admin commands\"\n");
		}

		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!help^8 - Prints all avialable commands to console\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!block^8 - Type this and aim on a Player that is blocking the way\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!class^8 - Prints current class. Type '^5!class Medic^8' to change class\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!drop^8 - Drops your current Weapon, used to give Weapons to others\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!kill^8 - Type this to kill your self\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!stuck^8 - Type this if stuck or lost, will take you back to spawn location\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!skill^8 - Prints current skill, type '^5!skill 3^8' for Very Hard. Range: 0-3\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!transport^8 - Transports you to given player. Range 0-8\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!mapname^8 - Prints filename of the current map (*.bsp)\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!info^8 - Prints (your) Informations.\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!build^8 - Displays current Build.\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!origin^8 - Displays your and targeted origin.\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!login^8 - Promts the Coop Admin Login Menu.\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!logout^8 - Revokes your Coop Admin Status.\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!kickbots^8 - Kicks all Bots\"\n");
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!follow^8 - Shows useing Player on the Radar for everyone\"\n");
		//gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!leader^8 - Forces a Teamleader Vote - Parameter: Client Number.\"\n");

		player->hudPrint(COOP_TEXT_HELP_COMMAND_LIST_PRINTED_ENG);
		player->hudPrint(COOP_TEXT_HELP_COMMAND_LIST_ENTER_TAB_DEU);
	}
	return true;
}

//=========================================================[b60014]
// Name:        G_coopCom_follow
// Class:       -
//              
// Description: handles player !follow command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_follow(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!follow") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!follow", level.time);

	//[b60014] chrissstrahl - print info
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//[b60014] chrissstrahl - deny request during cinematic and in spec
	if (sv_cinematic->integer || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}

	//remember if player using command is currently shown on radar (as missionobjective blip)
	bool bDisable = (bool)ent->entity->edict->s.missionObjective;

	//reset missionobjective blip on all players
	gentity_t* gentity;
	for (int i = 0; i < maxclients->integer; i++) {
		gentity = &g_entities[i];
		if (gentity->inuse && gentity->entity && gentity->client && gentity->entity->isSubclassOf(Player)){
			gentity->entity->edict->s.missionObjective = 0;
		}
	}

	//If blip is enabled for player disable (toggle)
	if (bDisable) {
		str text = "^5Locaction ^2green^5 location marker ^5on Radar ^1disabled";
		if (coop_checkPlayerLanguageGerman(player)) {
			text = "^2Gruene ^5Positions ^5Markerkierung ^5auf Radar ^1deaktiviert";
		}
		multiplayerManager.HUDPrint(player->entnum, va("%s\n", text.c_str()));
		ent->entity->edict->s.missionObjective;
		return true;
	}

	//Otherwise enable missionobjective blip for player
	ent->entity->edict->s.missionObjective = 1;

	//print message to all player huds of player being marked
	for (int i = 0; i < maxclients->integer; i++) {
		gentity_t* gentity2 = &g_entities[i];
		if (gentity2->inuse && gentity2->entity && gentity2->client && gentity2->entity->isSubclassOf(Player)) {
			Player* currentPlayer = (Player*)gentity2->entity;
			if (currentPlayer) {
				str text = "^5Locaction marked ^2green ^5on Radar of^8";
				if (coop_checkPlayerLanguageGerman(currentPlayer)) {
					text = "^5Position ^2gruen ^5markiert auf Radar von^8";
				}
				multiplayerManager.HUDPrint(currentPlayer->entnum,va("%s: %s\n", text.c_str(), player->client->pers.netname));
			}
		}
	}
	return true;
}

//================================================================
// Name:        G_coopCom_leader
// Class:       -
//              
// Description: handles player !leader command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_leader(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!leader") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!leader", level.time);

	//[b60014] chrissstrahl - print info
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//[b60014] chrissstrahl - deny request during cinematic and in spec
	if (sv_cinematic->integer || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}

	multiplayerManager.callVote(player, "leader", va("%i",player->entnum));
	return true;
}

//================================================================
// Name:        G_coopCom_info
// Class:       -
//              
// Description: handles player !info command, coop only command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_info(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!info") + 10) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!info", level.time);

	//[b60014] chrissstrahl - print info
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER || !game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}
	
	//[b60014] chrissstrahl - deny request
	if (gi.GetNumFreeReliableServerCommands(player->entnum) < 48 ||
		multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player)){
		return true;
	}

	//[b60014] chrissstrahl - deny request during cinematic and in spec
	if (sv_cinematic->integer) {
		player->hudPrint(COOP_TEXT_NOT_POSSIBLE_DURING_CINEMATIC);
		return true;
	}

	str s,s2;

	//[b60014] chrissstrahl printout the info to menu
	if (player->coop_getInstalledVersion() >= 60014) {
		str sInfoPrint = "YOU:\n";
		sInfoPrint += va("Coop Ver.: %i, C-Id: %d\n",player->coop_getInstalledVersion(),player->entnum);
		sInfoPrint += va("Coop Class: %s\n", player->coopPlayer.className.c_str());
		sInfoPrint += va("Lang.: %s, Entered: %.2f\n", player->getLanguage().c_str(),player->client->pers.enterTime);
		sInfoPrint += va("Pers.Id: %s\n", player->coop_getId().c_str());
	
		sInfoPrint += "\nSERVER:\n";
		cvar_t* cvarTemp = gi.cvar_get("local_language");
		if (cvarTemp != NULL) {
			s = cvarTemp->string;
		}
		if (skill->integer == 0)
			s2 = "Easy";
		else if (skill->integer == 1)
			s2 = "Normal";
		else if (skill->integer == 2)
			s2 = "Hard";
		else
			s2 = "VeryHard";

		sInfoPrint += va("Lang: %s, Skill: %s, FF.:  %.2f\n", s.c_str(),s2.c_str(),game.coop_friendlyFire);

#ifdef WIN32
		str sys2 = "Win";
#else
		str sys2 = "Lin";
#endif
		sInfoPrint += va("%i %s [%s %s]\n", COOP_BUILD, sys2.c_str(), __DATE__, __TIME__);
		sInfoPrint += va("Map: %s\n", level.mapname.c_str());
		sInfoPrint = coop_replaceForLabelText(sInfoPrint);
		DelayedServerCommand(player->entnum, va("globalwidgetcommand coop_comCmdI0 labeltext %s", sInfoPrint.c_str()));
		return true;
	}
	player->hudPrint(COOP_TEXT_HELP_YOUR_INFO_ENG);
	if (player->coop_getInstalled() == 1) {
		player->hudPrint(va("^5Coop Version^8: %i\n",player->coop_getInstalledVersion()));
	}
	else {
		player->hudPrint("^5Coop Version^8: None detected\n");
	}
	player->hudPrint(va("^5Coop Class^8: %s, ", player->coopPlayer.className.c_str()));
	player->hudPrint(va("^5Language^8: %s, ", player->getLanguage().c_str()));
	player->hudPrint(va("^5Client-Id^8: %d\n", player->entnum));

	player->hudPrint(va("^5Entred game at^8: %.2f, ", player->client->pers.enterTime));
	player->hudPrint(va("^5Personal Id^8: %s\n", player->coop_getId().c_str()));
	
	player->hudPrint("===SERVER Informations ===\n");
	player->hudPrint(va("^5Map:^8 %s\n", level.mapname.c_str()));

	cvar_t* cvarTemp = gi.cvar_get("local_language");
	if (cvarTemp != NULL) {
		s = cvarTemp->string;
		player->hudPrint(va("^5Language:^8 %s, ", s.c_str()));
	}
	if (skill->integer == 0)
		s = " [$$Easy$$]";
	else if (skill->integer == 1)
		s = " [$$Normal$$]";
	else if (skill->integer == 2)
		s = " [$$Hard$$]";
	else
		s = " [$$VeryHard$$]";
	player->hudPrint(va("^5Dificulty:^8 %d %s\n", skill->integer, s.c_str()));


	player->hudPrint(va("^5Friendly Fire Multiplier:^8 %.2f\n", game.coop_friendlyFire));
#ifdef WIN32
	str sys = "Windows";
#else
	str sys = "Linux";
#endif
	player->hudPrint(va("^5HZM Coop Mod [ %i ]^8 [ %s ] - ^3Compiled:^8 %s %s\n", COOP_BUILD, sys.c_str(), __DATE__, __TIME__));


	//player->hudPrint( "^3For more, Mission Info type:^5 !status\n" );
	//player->hudPrint( va( "^5Monsters killed^8: %i\n" , player->client->pers.enterTime ) );
	//add more from heuristics! - chrissstrahl - //hzm unfinished, //hzm upgrademe
	player->hudPrint("==================\n");
	return true;
}

//================================================================
// Name:        G_coopCom_kickbots
// Class:       -
//              
// Description: handles player !kickbots command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_kickbots(const gentity_t* ent)
{
	if (g_gametype->integer == GT_SINGLE_PLAYER || g_gametype->integer == GT_BOT_SINGLE_PLAYER) {
		return true;
	}

	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!kickbots") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!kickbots", level.time);


	if (!player->coop_playerCheckAdmin()) {
		multiplayerManager.callVote(player, "kick", "kickbots");
		return true;
	}
	gclient_t* cl;
	int iKicked = 0;
	for (int i = 0; i < maxclients->integer; i++) {
		cl = game.clients + i;
		if (!(g_entities[cl->ps.clientNum].svflags & SVF_BOT)) {
			continue;
		}
		gi.SendConsoleCommand(va("kick %i\n", i));
		iKicked++;
	}
	player->hudPrint(va("^5!kickbots^2 - kicked^6 %i ^2bots from server!\n", iKicked));
	return true;
}

//================================================================
// Name:        G_coopCom_kill
// Class:       -
//              
// Description: handles player !kill command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_kill(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32)
	{
		gi.SendServerCommand(player->entnum, "stufftext \"kill\"\n");
	}
	return true;
}

//================================================================
// Name:        G_coopCom_login
// Class:       -
//              
// Description: handles player !login command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_login(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!login") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!login", level.time);

	if (player->coop_playerCheckAdmin()) {
		player->hudPrint("^3You are already logged in - use ^2!logout^3 to log out.\n");
		return true;
	}

	player->hudPrint("^5login started\n");
	player->coop_playerAdminAuthStarted(true);
	DelayedServerCommand(player->entnum, "pushmenu coop_com");
	DelayedServerCommand(player->entnum, va("globalwidgetcommand coop_comCmdLoginMsg labeltext %s\n", coop_replaceForLabelText("Login Started - Please enter the code.").c_str()));
	player->coop_playerAdminAuthStringLastLengthUpdate();

	return true;
}

//================================================================
// Name:        G_coopCom_logout
// Class:       -
//              
// Description: handles player !logout command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_logout(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	str sMessage = "^3You are already logged out.\n";
	//[b60014] chrissstrahl - fixed login check
	if (player->coop_playerCheckAdmin()) {
		player->coop_playerAdmin(false);
		//[b60014] chrissstrahl - fixed auth string being retained keeping player loged in
		player->entityVars.SetVariable("coop_login_authorisation","*");
		sMessage = "^3You are now logged out.\n";
	}

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!logout") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!logout", level.time);

	player->hudPrint(sMessage.c_str());
	return true;
}

//================================================================
// Name:        G_coopCom_mapname
// Class:       -
//              
// Description: handles player !mapname command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_mapname(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!mapname") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!mapname", level.time);


	if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32)
	{
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint(va("Aktuelles Level ist:^5 %s.bsp\n", level.mapname.c_str()));
		}
		else {
			player->hudPrint(va("Current Level is:^5 %s.bsp\n", level.mapname.c_str()));
		}
	}
	return true;
}

//================================================================
// Name:        G_coopCom_origin
// Class:       -
//              
// Description: handles player !origin command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_origin(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;


	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!origin") + 1) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!origin", level.time);


	str sPrint = va("^3Your origin is:^8 %i %i %i", (int)player->origin[1], (int)player->origin[1], (int)player->origin[2]);
	if (player->_targetedEntity != NULL) {
		sPrint += va(" - ^3Targeted[$%s] origin is:^8 %i %i %i\n", player->_targetedEntity->targetname.c_str(), (int)player->_targetedEntity->origin[0], (int)player->_targetedEntity->origin[1], (int)player->_targetedEntity->origin[2]);
	}
	else {
		sPrint += "\n";
	}
	player->hudPrint(sPrint.c_str());
	return true;
}

//================================================================
// Name:        G_coopCom_skill
// Class:       -
//              
// Description: handles player !skill command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_skill(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!skill") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!skill", level.time);


	//coop only command
	if (!game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	const char* cmd;
	int   n;

	//NO ARGUMENT GIVEN
	n = gi.argc();
	if (n == 1) {
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32) {
			int currentSkill = (int)skill->value;
			str printMe = "^5Coop^2: ";
			if (coop_checkPlayerLanguageGerman(player)) {
				printMe += "Schwierigkeit bei: ";
			}
			else {
				printMe += "current SKILL is: ";
			}
			printMe += currentSkill;

			if (currentSkill == 0)
				printMe += " [$$Easy$$]";
			else if (currentSkill == 1)
				printMe += " [$$Normal$$]";
			else if (currentSkill == 2)
				printMe += " [$$Hard$$]";
			else
				printMe += " [$$VeryHard$$]";

			printMe += "\n";
			player->hudPrint(printMe);
		}
		return true;
	}

	//deny request during cinematic
	if (sv_cinematic->integer) {
		return true;
	}

	//get skill level input
	cmd = gi.argv(1);
	str sVal = cmd[0];
	int iRange = atoi(sVal.c_str());
	if (iRange < 0 || iRange > 3) {
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32) {
			if (coop_checkPlayerLanguageGerman(player)) {
				player->hudPrint("^5Coop^2: Illegale Angabe! Optionen: 0^8[$$Easy$$]^2, bis 3^8[$$Very Hard$$]\n");
			}
			else {
				player->hudPrint("^5Coop^2: Invalide range! Range is: 0^8[$$Easy$$]^2, to 3^8[$$Very Hard$$]\n");
			}
		}
		return true;
	}

	//hzm coop mod chrissstrahl - callvote if valid skill has been requested
	str command = "stufftext \"callvote skill ";
	command += cmd[0];
	if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32)
	{
		command += "\n";
		gi.SendServerCommand(player->entnum, command);
	}

	return true;
}

//================================================================
// Name:        G_coopCom_stuck
// Class:       -
//              
// Description: handles player !stuck command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_stuck(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	
	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!stuck") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!stuck", level.time);

	//coop only command
	if (!game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//deny request during cinematic and in spec
	if (sv_cinematic->integer) {
		return true;
	}

	//[b60014] chrissstrahl - place player at respawn/spawn location - changed from transporting to just moving
	Vector vOriginB4 = player->origin;
	coopSpawnlocation.placeAtSpawnPoint(player);
	Vector vOriginDATA = player->origin;

	//[b60014] chrissstrahl - compare player locations to determin if player is at a bad spawnspot
	//level z-axis out so we ignore falling players
	vOriginDATA[2] = 0;
	vOriginB4[2] = 0;
	if (VectorLength(vOriginB4 - vOriginDATA) < 100) {
		player->entityVars.SetVariable("!transport", 0.0f);
		G_coopCom_transport(ent);
		return true;
	}	

	if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32)
	{
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("^5Coop^2: Sie wurden zu Ihrem Spawnpunkt geschoben.\n");
		}
		else {
			player->hudPrint("^5Coop^2: You have been moved to your spawn location.\n");
		}
	}
	return true;
}

//================================================================
// Name:        G_coopCom_transport
// Class:       -
//              
// Description: handles player !transport command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_transport(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!transport") + 5) > level.time) {
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("^5Coop:^8 Ihr mobiler ^3Heisenberg Kompensator rekalibriert^8 gerade, bitte warten!\n");
		}
		else {
			player->hudPrint("^5Coop:^8 Your mobile ^3Heisenberg Compensator is recalibrating^8, please wait!\n");
		}
		return true;
	}

	//coop only command
	if (!game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//deny request during cinematic and in spec [b607] chrissstrahl - moved health check here
	if (sv_cinematic->integer || multiplayerManager.isPlayerSpectator(player) || player->health <= 0) {
		return true;
	}

	bool bTransportFailed = false;
	const char* cmd;
	int   n;
	n = gi.argc();
	cmd = gi.argv(1);


	int iPlayer = -1;
	if (n > 1) {
		str sId = va("%s",cmd);

		int i;
		for (i = 0; i < sId.length(); i++) {
			if (isdigit(sId[i])) {
				sId = sId[i];
				iPlayer = atoi(sId.c_str());
			}
		}
	}

	Player* targetPlayer = NULL;
	//beam directly to a given player number
	if (iPlayer >= 0) {
		if (&g_entities[iPlayer] && g_entities[iPlayer].client && g_entities[iPlayer].inuse) {
			targetPlayer = (Player*)g_entities[iPlayer].entity;
		}
	}
	//given player does not exist or no number was given
	if (!targetPlayer) {
		//more than 2 players, transport to closest player
		if (coop_returnPlayerQuantity(2) > 1) {
			targetPlayer = coop_returnPlayerClosestTo(player);
		}
		else
		{
			if (coop_checkPlayerLanguageGerman(player)) {
				player->hudPrint("^5Coop^2: Kein Teammitglied als Transport Ziel gefunden.\n");
			}
			else {
				player->hudPrint("^5Coop^2: No Teammember found, to serve as Transport target.\n");
			}

			bTransportFailed = true;
		}
	}

	//hzm coop mod chrissstrahl - fix transporting to spectator
	//[b607] chrissstrahl - fix nullpointer if target player does not exist
	//[b609] chrissstrahl - prevent beaming to dead player
	if (!targetPlayer || multiplayerManager.isPlayerSpectator(targetPlayer) || targetPlayer->health <= 0) {
		bTransportFailed = true;
	}
	//[b60014] chrissstrahl - prevent transport to target player if player has !notransport activated
	else {
		if (coop_returnEntityIntegerVar((Entity*)targetPlayer,"!notransport_active") == 1) {
			bTransportFailed = true;

			if (coop_checkPlayerLanguageGerman(player)) {
				player->hudPrint("^5Coop^2: Spieler erlaubt keine direkt Transporte.\n");
			}
			else {
				player->hudPrint("^5Coop^2: Player does not allow, direct Transports.\n");
			}
		}
		//[b60014] chrissstrahl - prevent transport if player has !notransport activated - do not allow terrorising other players
		else {
			if (coop_returnEntityIntegerVar((Entity*)player, "!notransport_active") == 1) {
				bTransportFailed = true;

				if (coop_checkPlayerLanguageGerman(player)) {
					player->hudPrint("^5Coop^2: Ihr Transport Inhibitor ist aktiviert!!!\n");
				}
				else {
					player->hudPrint("^5Coop^2: Your Transport inhibitor is active!!!\n");
				}
			}
		}
	}

	if (targetPlayer == player) {
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("^5Coop^2: Transport zu sich selbst nicht erlaubt.\n");
		}
		else {
			player->hudPrint("^5Coop^2: Can't transport to your self.\n");
		}
		bTransportFailed = true;
	}
	
	if (bTransportFailed) {
		return true;
	}

	//hzm coop mod chrissstrahl - make sure players do not get stuck inside each other
	player->_makeSolidASAP = true;

	player->entityVars.SetVariable("!transport", level.time);

	//holster weapon, prevent beam killing
	weaponhand_t	hand = WEAPON_ANY;//get player weapon, we might want to utilize that further
	player->getActiveWeaponName(hand, player->coopPlayer.transportUnholsterWeaponName);
	player->coopPlayer.transportUnholster = true;
	player->SafeHolster(true);
	player->upgPlayerDisableUseWeapon(true);

	player->client->ps.pm_time = 100;
	player->client->ps.pm_flags |= PMF_TIME_TELEPORT;

	player->client->ps.pm_time = 100;
	player->client->ps.pm_flags |= PMF_TIME_TELEPORT;

	Event* newEvent2 = new Event(EV_DisplayEffect);
	newEvent2->AddString("TransportIn");
	newEvent2->AddString("Multiplayer");
	player->PostEvent(newEvent2, 0.0f);

	player->origin = targetPlayer->origin;
	player->SetViewAngles(targetPlayer->angles);

	targetPlayer->_makeSolidASAP = true;

	//[b60014] chrissstrahl - execute thread if player gets transported
	ExecuteThread("coop_justTransported", true, (Entity*)player);

	return true;
}

//================================================================
// Name:        G_coopCom_notransport
// Class:       -
//              
// Description: toggles transport inhibitor
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_notransport(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!notransport") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!notransport", level.time);


	//coop only command
	if (!game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	if(coop_returnEntityFloatVar((Entity*)player, "!notransport_active") == 0.0f){
		player->entityVars.SetVariable("!notransport_active", 1.0f);

		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("^5Coop^2: Transport Inhibitor aktiviert.\n");
		}
		else {
			player->hudPrint("^5Coop^2: Transport inhibitor activated.\n");
		}
	}
	else {
		player->entityVars.SetVariable("!notransport_active", 0.0f);

		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("^5Coop^2: Transport Inhibitor ausgeschaltet.\n");
		}
		else {
			player->hudPrint("^5Coop^2: Transport inhibitor turned off.\n");
		}
	}

	return true;
}

// COOP COMMANDS - DEVELOPER SPECIFIC
// COOP COMMANDS - DEVELOPER SPECIFIC
// COOP COMMANDS - DEVELOPER SPECIFIC
//================================================================
// Name:        G_coopCom_reboot
// Class:       -
//              
// Description: handles player !reboot command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_reboot(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!reboot") + 10) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!reboot", level.time);

	if (!player->coop_playerCheckAdmin()) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return true;
	}
	
	game.coop_rebootForced = true;
	if (coop_serverManageReboot(level.mapname, player)) {
		player->hudPrint("^5Info^8: Server will reboot.\n");
	}
	else {
		game.coop_rebootForced = false;
		player->hudPrint("^5Info^8: Server rejected reboot command.\n");
	}
	return true;
}

//================================================================
// Name:        G_coopCom_levelend
// Class:       -
//              
// Description: handles player !levelend command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_levelend(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	if (!player->coop_playerCheckAdmin()) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return true;
	}

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!levelend") + 10) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!levelend", level.time);


	CThread *thread;
	thread = ExecuteThread("coop_endLevel", true, (Entity*)player);
	if (thread == NULL) {
		player->hudPrint("^5Info^8: Function 'coop_endLevel' not found in script.\n");
	}
	else {
		player->hudPrint("^5Info^8: Level End thread executed.\n");
	}
	return true;
}

//================================================================
// Name:        G_coopCom_targeted
// Class:       -
//              
// Description: handles player !targeted command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_targeted(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!targeted") + 3) > level.time) {
		return qtrue;
	}
	player->entityVars.SetVariable("!targeted", level.time);

	if (!player->coop_playerCheckAdmin()) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return qtrue;
	}

	if (player->coopPlayer.showTargetedEntity) {
		player->coopPlayer.showTargetedEntity = false;
		player->hudPrint("^5Info^8: Show Targetnames - ^1disabled.\n");
	}
	else {
		player->coopPlayer.showTargetedEntity = true;
		player->hudPrint("^5Info^8: Show Targetnames - ^2enabled.\n");
	}
	return qtrue;
}

//================================================================
// Name:        G_coopCom_showspawn
// Class:       -
//              
// Description: handles player !showspawn command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_showspawn(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!showspawn") + 3) > level.time) {
		return qtrue;
	}
	player->entityVars.SetVariable("!showspawn", level.time);

	if (!player->coop_playerCheckAdmin()) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return qtrue;
	}

	ExecuteThread("globalCoop_level_showSpawn", true, (Entity*)player);
	return qtrue;
}

//================================================================
// Name:        G_coopCom_hidespawn
// Class:       -
//              
// Description: handles player !hidespawn command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_hidespawn(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	if (!player->coop_playerCheckAdmin()) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return true;
	}

	ExecuteThread("globalCoop_level_hideSpawn", true, (Entity*)player);
	return true;
}

//================================================================
// Name:        G_coopCom_testspawn
// Class:       -
//              
// Description: handles player !testspawn command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_testspawn(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - add spam protection
	//deny usage of command if player executed command to quickly
	if ((coop_returnEntityFloatVar((Entity*)player, "!testspawn") + 3) > level.time) {
		return true;
	}
	player->entityVars.SetVariable("!testspawn", level.time);

	if (!player->coop_playerCheckAdmin()) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return true;
	}

	ExecuteThread("globalCoop_level_testSpawn", true, (Entity*)player);
	return true;
}

//================================================================
// Name:        G_coopCom_noclip
// Class:       -
//              
// Description: handles player !noclip command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_noclip(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	if (!player->coop_playerCheckAdmin()) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return true;
	}
	player->hudPrint("^5noclip toggled\n");
	extern Event EV_Player_DevNoClipCheat;
	player->ProcessEvent(EV_Player_DevNoClipCheat);
	return true;
}

//===========================================================[b60011]
// Name:        G_coopInput
// Class:       -
//              
// Description: allow player to start send text input to server
//              
// Parameters:  string-input-data
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopInput(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client)
		return qfalse;

	str		inputData;

	// Get the thread name
	if (!gi.argc())
		return true;

	inputData = gi.argv(1);

	//Grab more data
	int i;
	for (i = 2; i < 32; i++) {
		str sGrabMe = gi.argv(i);
		if (sGrabMe.length()) {
			inputData = va("%s %s", inputData.c_str(), sGrabMe.c_str());
		}
	}

	// Check to make sure player is allowed to run this thread
	// Need to do this part
	// Run the thread
	if (!inputData.length())
		return false;

	Player* player = (Player*)ent->entity;

	//[b60014] chrissstrahl - if !login is active add input to coopPlayer.adminAuthString instead
	//also update the cvar that is shown in teh login menu of the communicator
	if (multiplayerManager.inMultiplayer() && player->coop_playerAdminAuthStarted()) {
		if (inputData == "clear") {
			player->coop_playerAdminAuthString("");
		}
		else {
			player->coop_playerAdminAuthString(va("%s%s", player->coop_playerAdminAuthString().c_str(), inputData.c_str()));
		}
		
		DelayedServerCommand(player->entnum,va("globalwidgetcommand coop_comCmdLoginCode title '%s'\n",player->coop_playerAdminAuthString().c_str()));
		return true;
	}

	//limit of data that can be actually used
	if (inputData.length() > 260) { //(264) make sure we have space for linebreak
		inputData = coop_returnStringFromWithLength(inputData.c_str(), 0, 259);
	}

	ent->entity->entityVars.SetVariable("coopInputData", inputData.c_str());
	player->RunThread("playerInput");
	return true;
}

//===========================================================[b60011]
// Name:        G_coopThread
// Class:       -
//              
// Description: allow player to start a thread that ties back to him
// always during singleplayer
// always with prefix coopThread_ in threadname
// only as admin in mp
//              
// Parameters:  string-threadname
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopThread(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client)
		return true;

	str		threadName;

	// Get the thread name
	if (!gi.argc())
		return true;

	threadName = gi.argv(1);

	// Check to make sure player is allowed to run this thread
	Player* player = (Player*)ent->entity;
	if (	g_gametype->integer == GT_SINGLE_PLAYER || !multiplayerManager.inMultiplayer() ||
			!threadName.length() || coop_returnIntFind(threadName, "coopThread_") != 0 ||
			!player->coop_playerCheckAdmin())
	{
		return true;
	}
	
	CThread* pThread;
	pThread = player->RunThread(threadName.c_str());

	//if a admin executes a thread, give feedback
	if (coop_returnIntFind(threadName, "coopThread_") != 0) {
		str sPrint = "^5Succsessfully ran func";
		if (pThread == NULL) {
			sPrint = "^2FAILED to run func";
		}
		player->hudPrint(va("%s:^3 %s\n", sPrint.c_str(), threadName.c_str()));
	}
	return true;
}

//================================================================
// Name:        G_coopInstalled
// Class:       -
//              
// Description: Add a new command, used to detect coop mod client installation
//              
// Parameters:  int entNum
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopInstalled(const gentity_t* ent)
{
	if (!ent || !ent->inuse || !ent->client)
		return qfalse;

	Player* player = (Player*)ent->entity;
	const char* coopVer = gi.argv(1);

	if (strlen(coopVer)) { //[b60011] Chrissstrahl - fixed bad check
		player->coop_setInstalledVersion(atoi(coopVer));
	}
	coop_playerSetupCoop(player);

	return qtrue;
}

//================================================================
// Name:        G_coopItem
// Class:       -
//              
// Description: Allowing players to make use of special coop inventory
//              
// Parameters:  int entNum
//              
// Returns:     qboolean
// 
// Think:		coop_playerPlaceableThink
//              
//================================================================
qboolean G_coopItem(const gentity_t* ent)
//[b60011] chrissstrahl - allowing to place objects like mines and turrets
{
	return qtrue;
	if (!ent || !ent->inuse || !ent->client)
		return qtrue;

	Player* player = (Player*)ent->entity;

	//do not allow spawning if certain criteria are not meet
	if ((player->getLastDamageTime() + 0.5f) > level.time ||
		level.cinematic != qfalse ||
		multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(player) ||
		player->health <= 0)
	{
		player->hudPrint("coopitem - critaria for spawning are not meet\n");
		gi.Printf("coopitem - critaria for spawning are not meet\n");
		return qtrue;
	}

	str sModel;
	str sClass;
	bool bValid = false;
	const char* coopItem = gi.argv(1);

	//remove old placable right now
	if (player->coopPlayer.ePlacable) {
		player->coopPlayer.ePlacable->PostEvent(EV_Remove, 0.0f);
		player->coopPlayer.ePlacable = NULL;
	}

	SpawnArgs      args;
	Entity* obj = NULL;

	//placable explosive/mine
	if ( Q_stricmpn("mine", coopItem, 4) == 0) {
		bValid = true;
		args.setArg("classname", "script_model");
		args.setArg("model", "models/item/alien_actor_explosive.tik");
	}
	//placable explosive/mine
	else if (Q_stricmpn("medic", coopItem, 5) == 0) {
		bValid = true;
		args.setArg("classname", "script_model");
		args.setArg("model", "models/item/mp_weapon-spawn.tik");
		args.setArg("setmovetype", "stationary");
		args.setArg("notsolid","1");

//add ghost, model which attaches, and see if players without mod see it:
		//"models/item/mp_weapon-spawn.tik"
		//"item/ammo_large.tik"
		//"item/ammo_idryll_small.tik"
	}
	else {
		player->hudPrint("coopitem - No Item-Type specified.\n");
		gi.Printf("coopitem - No Item-Type specified.\n");
		return qtrue;
	}

	//do not place 
	if (!bValid) {
		if (obj != NULL) {
			obj->PostEvent(EV_Remove, 0.0f);
		}
		player->hudPrint("coopitem - object not placed\n");
		gi.Printf("coopitem - object not placed\n");
		return qtrue;
	}

	//now spawn object
	obj = args.Spawn();

	//Place object
	player->coopPlayer.ePlacable = obj;
	player->coopPlayer.ePlacable->setOrigin(player->origin);
	player->coopPlayer.ePlacable->setSize(Vector(-24, -24, 0), Vector(24, 24, 48));

	if (obj == NULL) {
		player->hudPrint("coopitem - error\n");
		gi.Printf("coopitem - error\n");
	}
	return qtrue;
}


#endif /* !__COOPGAMECMDS_CPP__ */