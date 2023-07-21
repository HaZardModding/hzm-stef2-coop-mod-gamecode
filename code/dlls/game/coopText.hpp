//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING TEXT AND LOCALIZING RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#ifndef __COOPTEXT_HPP__
#define __COOPTEXT_HPP__

#define COOP_MAX_TRANSMITT_STRING 272		//max string size, limit is menu compatibility whichends at 272
#define COOP_TRANSMITT_STARTCMD	10			//client start cmd (stufftext ), size of inizial client command
#define COOP_JOIN_SEPERATOR_SIZE 1			//size of join seperator (;) for console commands
#define COOP_JOIN_MAX_SINGLE_COMMAND (((COOP_MAX_TRANSMITT_STRING / 2 ) - (COOP_TRANSMITT_STARTCMD / 2)) - 1)

//[b60011] chrissstrahl - text for variouse outputs
#define COOP_TEXT_CLASS_MEDIC_ONLY_DEU "Benutzen nur durch: ^5$$Medic$$^8!\n"
#define COOP_TEXT_CLASS_MEDIC_ONLY_ENG "To use, you need to be: ^5$$Medic$$^8!\n"
#define COOP_TEXT_CLASS_TECHNICIAN_ONLY_DEU "Benutzen nur durch: ^5$$Technician$$^8!\n"
#define COOP_TEXT_CLASS_TECHNICIAN_ONLY_ENG "To use, you need to be: ^5$$Technician$$^8!\n"
#define COOP_TEXT_CLASS_HEAVYWEAPONS_ONLY_DEU "Benutzen nur durch: ^5$$HeavyWeapons$$^8!\n"
#define COOP_TEXT_CLASS_HEAVYWEAPONS_ONLY_ENG "To use, you need to be: ^5$$HeavyWeapons$$^8!\n"
#define COOP_TEXT_CLASS_CANT_CHANGE_ANYMORE_DEU "^3Ihre Klasse kann nicht mehr wechseln.\n"
#define COOP_TEXT_CLASS_CANT_CHANGE_ANYMORE_ENG "^3You can't change your class anymore.\n"
#define COOP_TEXT_CLASS_YOUR_CLASS_IS_DEU "^5Ihre aktuelle Klasse ist^5: "
#define COOP_TEXT_CLASS_YOUR_CLASS_IS_ENG "^5Your current class is^5: "
#define COOP_TEXT_CLASS_INVALID_CLASSNAME_DEU "^3Invalider Klassenname!^2 Valide: [^5t^2] $$Technician$$ [^5m^2] $$Medic$$ [^5h^2] $$HeavyWeapons$$\n"
#define COOP_TEXT_CLASS_INVALID_CLASSNAME_ENG "^3Invalid Classenname!^2 Valid: [^5t^2] $$Technician$$ [^5m^2] $$Medic$$ [^5h^2] $$HeavyWeapons$$\n"

#define COOP_TEXT_PLAYER_MODEL_NOT_ALLOWED_DEU "^8ist keine akzeptable Spielfigur..."
#define COOP_TEXT_PLAYER_MODEL_NOT_ALLOWED_ENG "^8is not an acceptable Player-Skin to use..."

#define COOP_TEXT_WEAPON_CANT_DROP_DEU "^5Diese Waffe kann nicht ablegt werden.\n"
#define COOP_TEXT_WEAPON_CANT_DROP_ENG "^5Can't drop this particular Weapon.\n"
#define COOP_TEXT_WEAPON_WAS_DROPPED_DEU "^5Ihre Waffe wurde ablegt.\n"
#define COOP_TEXT_WEAPON_WAS_DROPPED_ENG "^5Your Weapon was dropped.\n"

#define COOP_TEXT_NOT_POSSIBLE_DURING_CINEMATIC "^3Not possible during cinematic sequence.\n"
#define COOP_TEXT_COOP_COMMAND_ONLY "^3This command is only usable during Coop.\n"
#define COOP_TEXT_LOGIN_NEEDLOGINASADMIN "^3You need to !login as Coop Admin to use this command.\n"

#define COOP_TEXT_HELP_COMMAND_LIST_PRINTED_DEU "^2Befhelsliste in die Konsole ausgegeben!\n"
#define COOP_TEXT_HELP_COMMAND_LIST_PRINTED_ENG "^2Command list printed to Console!\n"
#define COOP_TEXT_HELP_COMMAND_LIST_ENTER_TAB_DEU "^2Tippen Sie ein einzelnes '!' Symbol ein und drücken Sie dann Tabulator\n"
#define COOP_TEXT_HELP_COMMAND_LIST_ENTER_TAB_ENG "^2Enter a single '!' Mark and then press Tabulator to cycle commands.\n"
#define COOP_TEXT_HELP_YOUR_INFO_DEU "===Ihre Informationen ===\n"
#define COOP_TEXT_HELP_YOUR_INFO_ENG "===Your Informations ===\n"

#define COOP_TEXT_BLOCK_MARKED_PLAYER_DEU "^2Spieler makiert!\n"
#define COOP_TEXT_BLOCK_MARKED_PLAYER_ENG "^2Player marked!\n"
#define COOP_TEXT_BLOCK_MARKED_PLAYER_NOTFOUND_DEU "^2Kein valides Ziel gefunden!\n"
#define COOP_TEXT_BLOCK_MARKED_PLAYER_NOTFOUND_ENG "^2No valid target found!\n"

#include "_pch_cpp.h"

str coop_textJoinCommands( str sCmd1 , str sCmd2 );
void coop_textCinematicHudprint( Player *player , str sText );
void coop_textHudprint( Player *player , str sText );
void coop_textHudprintAll( str messageToShow );

#endif /* coopText.hpp */
