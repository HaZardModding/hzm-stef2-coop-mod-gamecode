#ifndef __COOPGAMECMDS_CPP__
#define __COOPGAMECMDS_CPP__

#include "coopGamecmds.hpp"
#include "coopText.hpp"


//================================================================
// Name:        G_coopCom_class
// Class:       -
//              
// Description: handles player !class command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_eng(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

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

	}
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

	//coop only command
	if (!game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//deny request during cinematic and in spec
	if (sv_cinematic->integer || multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}
	
	//hzm coop mod chrissstrahl - allow to walk trugh a player that is currently blocking, this player needs to aim at the blocking player
	Entity* target;
	target = player->GetTargetedEntity();
	if ((target) && target->health > 0 && target->isSubclassOf(Player)) {
		Player* targetPlayer = (Player*)target;
		targetPlayer->setSolidType(SOLID_NOT);
		targetPlayer->_makeSolidASAPTime = (level.time + 7);
		targetPlayer->_makeSolidASAP = true;

		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("^5Coop^2: Spieler makiert!\n");
		}
		else {
			player->hudPrint("^5Coop^2: Player marked!\n");
		}
	}else{
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("^5Coop^2: Kein valides Ziel gefunden!\n");
		}
		else {
			player->hudPrint("^5Coop^2: No valid target found!\n");
		}
	}
	return true;
}

//================================================================
// Name:        G_coopCom_class
// Class:       -
//              
// Description: handles player !class command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_class(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;

	//coop only command
	if (!game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	const char* cmd;
	int   n;

	//NO ARGUMENT GIVEN
	n = gi.argc();
	if (n == 1){
		if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32){
			if (coop_checkPlayerLanguageGerman(player)) {
				player->hudPrint(va("^5Coop^2: Ihre aktuelle Klasse ist^5: %s\n", player->coopPlayer.className.c_str()));
			}
			else {
				player->hudPrint(va("^5Coop^2: Your current class is^5: %s\n", player->coopPlayer.className.c_str()));
			}
		}
		return true;
	}

	//hzm coop mod chrissstrahl - remember current health/armor/ammo status
	coop_serverSaveClientData(player);

	cmd = gi.argv(1);
	
	str classSelected;
	switch (cmd[0])
	{
	case 'h':
		classSelected = "HeavyWeapons";
		break;
	case 'm':
		classSelected = "Medic";
		break;
	case 't':
		classSelected = "Technician";
		break;
	default:
		if (gi.GetNumFreeReliableServerCommands(player->entnum) >= 32) {
			if (coop_checkPlayerLanguageGerman(player)) {
				player->hudPrint("^5Coop^2: ^3Invalideer Klassenname!^2 Valid: [^5t^2]Technician [^5m^2]Medic [^5h^2]HeavyWeapons\n");
			}
			else {
				player->hudPrint("^5Coop^2: ^3Invalid class name!^2 Valid: [^5t^2]Technician [^5m^2]Medic [^5h^2]HeavyWeapons\n");
			}
		}
		return true;
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

	//coop only command
	if (!game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//deny request during cinematic and in spec
	if (sv_cinematic->integer || multiplayerManager.isPlayerSpectator(player)) {
		return true;
	}

	//deny request during cinematic
	if (sv_cinematic->integer) {
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
			if (!Q_stricmpn("None", weaponName, 4) || !Q_stricmpn("EnterpriseCannon", weaponName, 4) || !Q_stricmpn("Batleth", weaponName, 4) || !Q_stricmpn("Phaser", weaponName, 6) || !Q_stricmpn("Tricorder", weaponName, 9))
			{
				if (coop_checkPlayerLanguageGerman(player)) {
					player->hudPrint("^5Coop^2: Diese Waffe kann nicht ablegt werden.\n");
				}
				else {
					player->hudPrint("^5Coop^2: Can't drop this particular Weapon.\n");
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

				if (coop_checkPlayerLanguageGerman(player)) {
					player->hudPrint("^5Coop^2: Ihre Waffe wurde ablegt.\n");
				}
				else {
					player->hudPrint("^5Coop^2: Your Weapon was dropped.\n");
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
	if (gi.GetNumFreeReliableServerCommands(player->entnum) < 32)
		return true;

	if (coop_checkPlayerLanguageGerman(player)) {
		//gi.SendConsoleCommand(va("echo %s\n", ));

		if (player->coopPlayer.admin) {
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
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!block^8 - Zielen Sie auf einen blockierenden Spieler\"\n");
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
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!leader^8 - Erzwingt Teamleader Abstimmung - Parameter: Client Nummer\"\n");

		player->hudPrint("^2Befehlsliste wurde in Konsole ausgegeben!\n");
		player->hudPrint("^2Tippen Sie ein einzelnes '!' Symbol ein und drücken Sie dann Tabulator\n");
	}
	else {
		if (player->coopPlayer.admin) {
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
		gi.SendServerCommand(player->entnum, "stufftext \"echo ^5!leader^8 - Forces a Teamleader Vote - Parameter: Client Number.\"\n");

		player->hudPrint("^2Command list printed to Console!\n");
		player->hudPrint("^2Enter a single '!' Mark and then press Tabulator to cycle commands.\n");
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
	multiplayerManager.callVote(player, "leader", va("%i",player->entnum));
	return true;
}

//================================================================
// Name:        G_coopCom_info
// Class:       -
//              
// Description: handles player !info command
//              
// Parameters:  const gentity_t* ent
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopCom_info(const gentity_t* ent)
{
	Player* player = (Player*)ent->entity;
	if (gi.GetNumFreeReliableServerCommands(player->entnum) < 32)
		return true;

	str s;
	player->hudPrint("===Your Informations ===\n");
	if (player->coopPlayer.installed == 1) {
		str temp = player->coopPlayer.installedVersion;
		str coopVer = temp[0];
		coopVer += '.';
		coopVer += temp[1];
		coopVer += temp[2];
		player->hudPrint(va("^5Coop Version^8: %s\n", coopVer.c_str()));
	}
	else {
		player->hudPrint("^5Coop Version^8: None detected\n");
	}
	player->hudPrint(va("^5Coop class^8: %s\n", player->coopPlayer.className.c_str()));
	player->hudPrint(va("^5Entred this game at^8: %f\n", player->client->pers.enterTime));
	player->hudPrint(va("^5Your game language is^8: %s\n", player->getLanguage()));
	player->hudPrint(va("^5Your Personal Id (only shown to you) is^8: %s\n", player->coopPlayer.coopId.c_str()));
	player->hudPrint(va("^5Your Client/Coop number is^8: %d\n", player->entnum));
	player->hudPrint("===SERVER Informations ===\n");
	player->hudPrint(va("^5Current map:^8 %s\n", level.mapname.c_str()));

	cvar_t* cvarTemp = gi.cvar_get("local_language");
	if (cvarTemp != NULL) {
		s = cvarTemp->string;
		player->hudPrint(va("^5Server Language:^8 %s\n", s.c_str()));
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
	player->hudPrint(va("^5Friendly Fire Multiplier:^8 %f\n", game.coop_friendlyFire));
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
	Player* player = (Player*)ent->entity;
	if (!coop_playerCheckAdmin(player)) {
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
	if (coop_playerCheckAdmin(player)) {
		player->hudPrint("^3You are already logged in - use ^2!logout^3 to log out.\n");
		return true;
	}

	player->hudPrint("^5login started\n");
	player->entityVars.SetVariable("uservar1", "mom_codepanel2");
	player->entityVars.SetVariable("uservar2", "coop_login");
	//player->setStringVar("uservar3","no coop mod thread (get latest coop mod to use this)");
	ExecuteThread("mom_basic", true, (Entity*)player);
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
	if (player->coopPlayer.admin) {
		player->coopPlayer.admin = false;
		player->hudPrint("^3You are now logged out.\n");
	}
	else {
		player->hudPrint("^3You are already logged out.\n");
	}
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
	str sPrint = va("^3Your origin is:^8 %i %i %i", (int)player->origin[1], (int)player->origin[1], (int)player->origin[2]);
	if (player->_targetedEntity != NULL) {
		sPrint += va(" - ^3Targeted origin is:^8 %i %i %i\n", (int)player->_targetedEntity->origin[0], (int)player->_targetedEntity->origin[1], (int)player->_targetedEntity->origin[2]);
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

	//coop only command
	if (!game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//deny request during cinematic and in spec
	if (sv_cinematic->integer) {
		return true;
	}

	//hzm coop mod chrissstrahl - place player at respawn/spawn location
	coop_playerTransportToSpawn(player);

	if (gi.GetNumFreeReliableServerCommands(player->entnum) > 32)
	{
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("^5Coop^2: Sie wurden zum Spawnpunkt Teleportiert.\n");
		}
		else {
			player->hudPrint("^5Coop^2: You have been teleported to your spawn location.\n");
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

	//coop only command
	if (!game.coop_isActive) {
		player->hudPrint(COOP_TEXT_COOP_COMMAND_ONLY);
		return true;
	}

	//deny request during cinematic and in spec [b607] chrissstrahl - moved health check here
	if (sv_cinematic->integer || multiplayerManager.isPlayerSpectator(player) || player->health <= 0) {
		return true;
	}
	//deny beaming when to fast
	if (player->coopPlayer.lastTimeTransported + 3 > level.time) {
		if (coop_checkPlayerLanguageGerman(player)) {
			player->hudPrint("^5Coop:^8 Ihr mobiler ^3Heisenberg Kompensator rekalibriert^8 (2 sek) gerade, bitte warten!\n");
		}
		else {
			player->hudPrint("^5Coop:^8 Your mobile ^3Heisenberg Compensator is recalibrating^8 (2 sec), please wait!\n");
		}
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
	if (iPlayer >= 0) {
		if (&g_entities[iPlayer] && g_entities[iPlayer].client && g_entities[iPlayer].inuse) {
			targetPlayer = (Player*)g_entities[iPlayer].entity;
		}
	}
	if (!targetPlayer) {
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
	//player->_makeSolidASAPSupposedToBeSolid = true;

	//remember tarnsport time
	player->coopPlayer.lastTimeTransported = level.time;

	//holster weapon, prevent beam killing
	weaponhand_t	hand = WEAPON_ANY;//get player weapon, we might want to utilize that further
	player->getActiveWeaponName(hand, player->coopPlayer.transportUnholsterWeaponName);
	player->coopPlayer.transportUnholster = true;
	player->SafeHolster(true);
	player->disableUseWeapon(true);

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
	//targetPlayer->_makeSolidASAPSupposedToBeSolid = true;
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
	if (!coop_playerCheckAdmin(player)) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return true;
	}
	
	game.coop_rebootForced = true;
	if (coop_serverManageReboot(level.mapname, player)) {
		player->hudPrint("^5Info^8: Server will reboot.\n");
	}
	else {
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
	if (!coop_playerCheckAdmin(player)) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return true;
	}

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

	if (!coop_playerCheckAdmin(player)) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return true;
	}

	if (player->coopPlayer.showTargetedEntity) {
		player->coopPlayer.showTargetedEntity = false;
		player->hudPrint("^5Info^8: Show Targetnames - ^1disabled.\n");
	}
	else {
		player->coopPlayer.showTargetedEntity = true;
		player->hudPrint("^5Info^8: Show Targetnames - ^2enabled.\n");
	}
	return true;
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
	if (!coop_playerCheckAdmin(player)) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return true;
	}

	ExecuteThread("globalCoop_level_showSpawn", true, (Entity*)player);
	return true;
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
	if (!coop_playerCheckAdmin(player)) {
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
	if (!coop_playerCheckAdmin(player)) {
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
	if (!coop_playerCheckAdmin(player)) {
		player->hudPrint(COOP_TEXT_LOGIN_NEEDLOGINASADMIN);
		return true;
	}
	player->hudPrint("^5noclip toggled\n");
	extern Event EV_Player_DevNoClipCheat;
	player->ProcessEvent(EV_Player_DevNoClipCheat);
	return true;
}

//===========================================================[b611]
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
		return false;

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

	//limit of data that can be actually used
	if (inputData.length() > 260) { //(264) make sure we have space for linebreak
		inputData = coop_substr(inputData.c_str(), 0, 259);
	}

	ent->entity->entityVars.SetVariable("coopInputData", inputData.c_str());
	player->RunThread("playerInput");
	return true;
}

//===========================================================[b611]
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
		return qfalse;

	str		threadName;

	// Get the thread name
	if (!gi.argc())
		return false;

	threadName = gi.argv(1);

	// Check to make sure player is allowed to run this thread
	// Need to do this part
	// Run the thread - if set 
	Player* player = (Player*)ent->entity;
	if (!threadName.length() || g_gametype->integer != GT_SINGLE_PLAYER && coop_returnIntFind(threadName, "coopThread_") != 0 && player->coopPlayer.admin != true) {
		return false;
	}
	player->RunThread(threadName.c_str());
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
	if (strlen(coopVer)) { //[b611] Chrissstrahl - fixed bad check
		player->coopPlayer.installedVersion = atoi(coopVer);
	}
	coop_playerSetupCoop(player);

	return qtrue;
}

//================================================================
// Name:        G_coopItem
// Class:       -
//              
// Description: Add a new command, used to detect coop mod client installation
//              
// Parameters:  int entNum
//              
// Returns:     qboolean
//              
//================================================================
qboolean G_coopItem(const gentity_t* ent)
//[b611] chrissstrahl - add command allowing players to make use of special coop inventory
//allowing to place objects like mines and turrets
{
	if (!ent || !ent->inuse || !ent->client)
		return qfalse;

	Player* player = (Player*)ent->entity;

	//do not allow spawning if certain criteria are not meet
	if ((player->getLastDamageTime() + 0.5f) > level.time &&
		level.cinematic != qfalse &&
		multiplayerManager.isPlayerSpectator(player) &&
		player->health <= 0)
	{
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
	Event* attach1 = NULL;
	Event* attach2 = NULL;
	Event* attach3 = NULL;

	//placable explosive/mine
	if (!Q_stricmpn("mine", coopItem, 4)) {
		bValid = true;
		args.setArg("classname", "script_model");
		args.setArg("model", "models/item/alien_actor_explosive.tik");
	}
	//placable explosive/mine
	else if (!Q_stricmpn("medic", coopItem, 5)) {
		bValid = true;
		args.setArg("classname", "script_model");
		args.setArg("model", "models/item/mp_weapon-spawn.tik");
		args.setArg("setmovetype", "stationary");
		args.setArg("setsize", "\"-24 -24 0\" \"24 24 32\"");
		args.setArg("targetname", "yyy");
		args.setArg("notsolid", "1");
		//args.setArg("statemap", "1212");

		attach1 = new Event(EV_AttachModel);
		attach1->AddString("item/ammo_large.tik");
		attach1->AddString("none");
		attach1->AddFloat(0.6f);
		attach1->AddString("none");
		attach1->AddInteger(0);
		attach1->AddInteger(-1);
		attach1->AddInteger(-0);
		attach1->AddInteger(-1);
		attach1->AddInteger(0);
		attach1->AddVector(Vector(10, 10, 6));

		attach2 = new Event(EV_AttachModel);
		attach2->AddString("item/ammo_idryll_small.tik");
		attach2->AddString("none");
		attach2->AddFloat(0.6f);
		attach2->AddString("none");
		attach2->AddInteger(0);
		attach2->AddInteger(-1);
		attach2->AddInteger(-0);
		attach2->AddInteger(-1);
		attach2->AddInteger(0);
		attach2->AddVector(Vector(-10, 10, 6));

		attach3 = new Event(EV_AttachModel);
		attach3->AddString("item/ammo_fed_small.tik");
		attach3->AddString("none");
		attach3->AddFloat(0.9f);
		attach3->AddString("none");
		attach3->AddInteger(0);
		attach3->AddInteger(-1);
		attach3->AddInteger(-0);
		attach3->AddInteger(-1);
		attach3->AddInteger(0);
		attach3->AddVector(Vector(0, -10, 6));
	}
	//do not place 
	if (!bValid) {
		delete attach1;
		delete attach2;
		delete attach3;
		if (obj != NULL) {
			obj->PostEvent(EV_Remove, 0.0f);
		}
		return qtrue;
	}

	//now spawn object
	obj = args.Spawn();

	//attach stuff if wanted
	if (attach1 != NULL) {
		obj->PostEvent(attach1, 0.1f);
	}
	if (attach2 != NULL) {
		obj->PostEvent(attach2, 0.1f);
	}
	if (attach3 != NULL) {
		obj->PostEvent(attach3, 0.1f);
	}

	//Place object
	player->coopPlayer.ePlacable = obj;
	player->coopPlayer.ePlacable->setOrigin(player->origin);
	return qtrue;
}


#endif /* !__COOPGAMECMDS_CPP__ */