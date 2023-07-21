//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/game.h                                        $
// $Revision:: 3                                                              $
//     $Date:: 10/13/03 8:54a                                                 $
//
// Copyright (C) 1999 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//
//
// DESCRIPTION:
//

#ifndef __GAME_H__
#define __GAME_H__

#include "g_local.h"
#include "class.h"

//hzm coop mod chrissstrahl - used for oversseing the tiki model cache, to decive when to quit/reboot server
#define COOP_MAX_TIKI_LISTING 2200	//stock game has 2093 TIKIS
#define COOP_MAX_SKA_LISTING 3600	//stock game has 3473 animations
#define COOP_MAX_SPRITES_LISTING 250//guessed value
#define COOP_BUILD 60014 //used to be 3 digits like 610, changed to allow for more frequent relases
#define MAPTYPE_MULTIPLAYER 0
#define MAPTYPE_SECRET 1
#define MAPTYPE_IGM 2
#define MAPTYPE_MISSION 3
#define MAPTYPE_CUSTOM 4
#define MAPTYPE_RPG 5 //[b60013] chrissstrahl - allow rpg_ maps to be handled differently
//end of chrissstrahl - added this commentary [b607]

//
// this structure is left intact through an entire game
// it should be initialized at dll load time, and read/written to
// the server.ssv file for savegames
//

class Game : public Class
	{
	public:
		CLASS_PROTOTYPE( Game );

      gclient_t	      *clients;				// [maxclients]
	   qboolean	         autosaved;

// store latched cvars here that we want to get at often
	   int			 maxclients;
	   int			 maxentities;

//hzm coop mod chrissstrahl - used to monitor tiki cache (quit/reboot server)
	   str coop_serverLoadedTikiListing[COOP_MAX_TIKI_LISTING];
	   str coop_serverLoadedSkaListing[COOP_MAX_SKA_LISTING];
	   str coop_serverLoadedSpritesListing[COOP_MAX_SPRITES_LISTING];
//hzm gameupdate chrissstrahl - used to check if a branchdialog
	   EntityPtr	branchdialog_chosenPlayer; //[b608] chrissstrahl - used to store player that is valid to select the dialog
	   bool			branchdialog_selectionActive;
	   float		branchdialog_startingTime;
	   str			branchdialog_name;
	   int			branchdialog_timeout;

//hzm gameupdate chrissstrahl - add bool for standard level detection
//hzm coop mod chrissstrahl - add bool for coop mod included level detection
	   bool			isStandardLevel;
	   int			levelType;
	   bool			isCoopIncludedLevel;
//hzm gameupdate chrissstrahl - 
	   bool			cinematicSkipping;
//hzm gameupdate chrissstrahl - force update the dynamic lights
	   bool			bForceDynLightUpdate;
//hzm coop mod chrissstrahl - tells if current map is played in coop
	   bool			coop_isActive;
//hzm coop mod chrissstrahl - add bool for coop awards detection
	   bool			coop_awardsActive;
//hzm coop mod chrissstrahl - add bool for detection if we should reboot
	   bool			coop_reboot;
//hzm coop mod chrissstrahl - added to allow forced reboot [b607]
	   bool			coop_rebootForced;
//hzm coop mod chrissstrahl - used to tell the coop mod when it is okay to save client data
	   bool			coop_saveClientData;
//hzm coop mod chrissstrahl - store which objective items are set, and should be send to player
	   float		coop_objectiveLastUpdate;
	   int			coop_objectiveCycle;
//hzm coop mod chrissstrahl - store objective items status
	   int			coop_objectiveItemState[8];
//hzm coop mod chrissstrahl - store which objective items are completed when
	   float		coop_objectiveItemCompletedAt[8];
//hzm coop mod chrissstrahl - this stores the story for the current level, set/reset each level load
	   str			coop_story;
	   str			coop_story_deu;
//hzm coop mod chrissstrahl - this stores the story for the current level, set/reset each level load
	   str			coop_author;
//hzm coop mod chrissstrahl - this stores when the class info was updated last for all players
	   float		coop_classInfoSendAt;
//hzm coop mod chrissstrahl - stores if sv_floodprotect should be reactivated when entering regular multiplayer
	   bool			sv_floodprotectActivate;
	   bool			sv_floodprotectChecked;
//hzm coop mod chrissstrahl - stores a timestamp from when the server may reconnect
	   float		coop_restartServerEarliestAt;
//hzm coop mod chrissstrahl - stores the findly fire multiplier
	   float		coop_friendlyFire;
//hzm coop mod chrissstrahl - stores the gametype
	   int			coop_gametype;
//hzm coop mod chrissstrahl - stores status of Last Man Standing Modifier 
	   int			coop_lastmanstanding;
//hzm coop mod chrissstrahl - stores the maxspeed
	   int			coop_maxspeed;
//hzm coop mod chrissstrahl - stores the respawntime
	   int			coop_respawnTime;
//hzm coop mod chrissstrahl - stores the realtime when the map has started
	   int			coop_levelStartTime;
//[b607] hzm coop mod chrissstrahl - used to manage g_showevents 
	   int			coop_showEventsTime;
//[b607] chrissstrahl - sets the number of dead actor bodies per area, 0 means disabled
	   int			coop_deadBodiesPerArea;
//[b607] chrissstrahl - sets the number of all dead actor bodies 
	   int			coop_deadBodiesAllNum;
//[b607] chrissstrahl - allow us to detect if the game was restarted  via restart or loaded
	   bool			levelRestarted;
//[b607] chrissstrahl - allow us to set stasistime for idryll staff weapon against ai
	   int			coop_stasisTime;
//[b607] chrissstrahl - if this is true pending missionfailure event will be stoped (auto fail if sv empty)
	   bool			coop_autoFailPending;
//[b60014] chrissstrahl - allow sv_floodprotect toggeling to receive language/id/version
	   bool			coop_floodprotectToggle;
	Game() { Init(); }

	void              Init( void );
	virtual void      Archive( Archiver &arc );
};

extern Game game;

#endif /* !__GAME_H__ */
