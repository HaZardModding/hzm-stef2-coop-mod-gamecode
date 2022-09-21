// Copyright (C) 1999-2000 Id Software, Inc.
//

/*****************************************************************************
 * name:		ai_chat.c
 *
 * desc:		Quake3 bot AI
 *
 * $Archive: /Code/DLLs/game/ai_chat.cpp $
 * $Author: Steven $ 
 * $Revision: 16 $
 * $Modtime: 10/13/03 9:03a $
 * $Date: 10/13/03 9:11a $
 *
 *****************************************************************************/

#include "g_local.h"
#include "botlib.h"
#include "be_aas.h"
#include "be_ea.h"
#include "be_ai_char.h"
#include "be_ai_chat.h"
#include "be_ai_gen.h"
#include "be_ai_goal.h"
#include "be_ai_move.h"
#include "be_ai_weap.h"
//
#include "ai_main.h"
#include "ai_dmq3.h"
#include "ai_chat.h"
#include "ai_cmd.h"
#include "ai_dmnet.h"
//
#include "chars.h"				//characteristics
#include "inv.h"				//indexes into the inventory
#include "syn.h"				//synonyms
#include "match.h"				//string matching types and vars

// for the voice chats
#ifdef MISSIONPACK // bk001205
#include "botmenudef.h"
#endif

#define TIME_BETWEENCHATTING	25


/*
==================
BotNumActivePlayers
==================
*/
int BotNumActivePlayers(void) {
	int i, num;
	char buf[MAX_INFO_STRING];
//	static int maxclients->integer;

//	if (!maxclients->integer)
//		maxclients->integer = gi.Cvar_VariableIntegerValue("sv_maxclients->integer");

	num = 0;
	for (i = 0; i < maxclients->integer && i < MAX_CLIENTS; i++) {
		strncpy(buf,gi.getConfigstring(CS_PLAYERS+i), sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || 
			!strlen(Info_ValueForKey(buf, "name"))
			) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//
		num++;
	}
	return num;
}

/*
==================
BotIsFirstInRankings
==================
*/
int BotIsFirstInRankings(bot_state_t *bs) {
//	int i, score;
//	char buf[MAX_INFO_STRING];
//	static int maxclients->integer;
//	playerState_t ps;

	return qfalse;
	/* FIXME
	score = bs->cur_ps.persistant[PERS_SCORE];
	for (i = 0; i < maxclients->integer && i < MAX_CLIENTS; i++) {
		gi.getConfigstring(CS_PLAYERS+i, buf, sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "name"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//
		BotAI_GetClientState(i, &ps);
		if (score < ps.persistant[PERS_SCORE]) return qfalse;
	}
	return qtrue;
	*/
}

/*
==================
BotIsLastInRankings
==================
*/
int BotIsLastInRankings(bot_state_t *bs) {
//	int i, score;
//	char buf[MAX_INFO_STRING];
//	playerState_t ps;

	return qfalse;
	/* FIXME
	score = bs->cur_ps.persistant[PERS_SCORE];
	for (i = 0; i < maxclients->integer && i < MAX_CLIENTS; i++) {
		gi.getConfigstring(CS_PLAYERS+i, buf, sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "name"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//
		BotAI_GetClientState(i, &ps);
		if (score > ps.persistant[PERS_SCORE]) return qfalse;
	}
	return qtrue;

  */
}

/*
==================
BotFirstClientInRankings
==================
*/
char *BotFirstClientInRankings(void) {
	int i, bestscore, bestclient;
	char buf[MAX_INFO_STRING];
	static char name[32];
	playerState_t ps;

	bestscore = -999999;
	bestclient = 0;
	for (i = 0; i < maxclients->integer && i < MAX_CLIENTS; i++) {
		strncpy(buf,gi.getConfigstring(CS_PLAYERS+i), sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "name"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//
		BotAI_GetClientState(i, &ps);
/* FIXME
		if (ps.persistant[PERS_SCORE] > bestscore) {
			bestscore = ps.persistant[PERS_SCORE];
			bestclient = i;
		}
*/
	}
	EasyClientName(bestclient, name, 32);
	return name;
}

/*
==================
BotLastClientInRankings
==================
*/
char *BotLastClientInRankings(void) {
	int i, worstscore, bestclient;
	char buf[MAX_INFO_STRING];
	static char name[32];
	playerState_t ps;

	worstscore = 999999;
	bestclient = 0;
	for (i = 0; i < maxclients->integer && i < MAX_CLIENTS; i++) {
		strncpy(buf,gi.getConfigstring(CS_PLAYERS+i), sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "name"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//
		BotAI_GetClientState(i, &ps);
/* FIXME
		if (ps.persistant[PERS_SCORE] < worstscore) {
			worstscore = ps.persistant[PERS_SCORE];
			bestclient = i;
		}
*/
	}
	EasyClientName(bestclient, name, 32);
	return name;
}

/*
==================
BotRandomOpponentName
==================
*/
char *BotRandomOpponentName(bot_state_t *bs) {
	int i, count;
	char buf[MAX_INFO_STRING];
	int opponents[MAX_CLIENTS], numopponents;
	static char name[32];

	numopponents = 0;
	opponents[0] = 0;
	for (i = 0; i < maxclients->integer && i < MAX_CLIENTS; i++) {
		if (i == bs->client) continue;
		//
		strncpy(buf,gi.getConfigstring(CS_PLAYERS+i), sizeof(buf));
		//if no config string or no name
		if (!strlen(buf) || !strlen(Info_ValueForKey(buf, "name"))) continue;
		//skip spectators
		if (atoi(Info_ValueForKey(buf, "t")) == TEAM_SPECTATOR) continue;
		//skip team mates
		if (BotSameTeam(bs, i)) continue;
		//
		opponents[numopponents] = i;
		numopponents++;
	}
	count = (int)(random() * numopponents);
	for (i = 0; i < numopponents; i++) {
		count--;
		if (count <= 0) {
			EasyClientName(opponents[i], name, sizeof(name));
			return name;
		}
	}
	EasyClientName(opponents[0], name, sizeof(name));
	return name;
}

/*
==================
BotMapTitle
==================
*/

char *BotMapTitle(void) {
	char info[1024];
	static char mapname[128];

	gi.SV_GetServerinfo(info, sizeof(info));

	strncpy(mapname, Info_ValueForKey( info, "mapname" ), sizeof(mapname)-1);
	mapname[sizeof(mapname)-1] = '\0';

	return mapname;
}


/*
==================
BotWeaponNameForMeansOfDeath
==================
*/

char *BotWeaponNameForMeansOfDeath( int mod )
{
	static char meansOfDeathName[ 64 ];

	strcpy( meansOfDeathName, "$$MOD-" );
	strcat( meansOfDeathName, MOD_NumToName( mod ) );
	strcat( meansOfDeathName, "$$" );

	return meansOfDeathName;

	/* switch(mod) {
	//case MOD_IMOD: return "I-Mod";
	case MOD_EXPLOSION: return "Explosion";
	case MOD_VAPORIZE:
	case MOD_VAPORIZE_DISRUPTOR:	// fall-through
	case MOD_VAPORIZE_COMP:			// fall-through
		return "Beam blast";
	default:
		return "Energy weapon";
	}
//	return "Energy weapon";  */
}

/*
==================
BotRandomWeaponName
==================
*/
char *BotRandomWeaponName(void) {
	int rnd;

	rnd = rand() % 17;
	//[b60011] code cleanup
	const char* returnVal;

	switch( rnd ) {
	case 0:		returnVal = "$$Weapon-AttrexianRifle$$"; break;
	case 1:		returnVal = "$$Weapon - Batleth$$"; break;
	case 2:		returnVal = "$$Weapon-BurstRifle$$"; break;
	case 3:		returnVal = "$$Weapon-CompressionRifle$$"; break;
	case 4:  	returnVal = "$$Weapon-DrullStaff$$"; break;
	case 5:		returnVal = "$$Weapon-FieldAssaultRifle$$"; break;
	case 6:		returnVal = "$$Weapon-GrenadeLauncher$$"; break;
	case 7:		returnVal = "$$Weapon-I-Mod$$"; break;
	case 8:		returnVal = "$$Weapon-Phaser$$"; break;
	case 9:		returnVal = "$$Weapon-PhotonBurst$$"; break;
	case 10:	returnVal = "$$Weapon-RomulanDisruptor$$"; break;
	case 11:	returnVal = "$$Weapon-RomulanRadGun$$"; break;
	case 12: 	returnVal = "$$Weapon-FederationSniperRifle$$"; break;
	case 13: 	returnVal = "$$Weapon-TetryonGatlingGun$$"; break;
	case 14:	returnVal = "$$Weapon-AttrexianRifle$$"; break;
	case 15: 	returnVal = "$$Weapon-AttrexianRifle$$"; break;
	case 16: 	returnVal = "$$Weapon-AttrexianRifle$$"; break;
	default:	returnVal = "$$Weapon-Phaser$$";
	}
	return (char*)returnVal;
}

/*
==================
BotVisibleEnemies
==================
*/
int BotVisibleEnemies(bot_state_t *bs) {
	float vis;
	int i;
	aas_entityinfo_t entinfo;

	for (i = 0; i < maxclients->integer && i < MAX_CLIENTS; i++) {

		if (i == bs->client) continue;
		//
		BotEntityInfo(i, &entinfo);
		//
		if (!entinfo.valid) continue;
		//if the enemy isn't dead and the enemy isn't the bot self
		if (EntityIsDead(&entinfo) || entinfo.number == bs->entitynum) continue;
		//if the enemy is invisible and not shooting
		if (EntityIsInvisible(&entinfo) && !EntityIsShooting(&entinfo)) {
			continue;
		}
		//if on the same team
		if (BotSameTeam(bs, i)) continue;
		//check if the enemy is visible
		vis = BotEntityVisible(bs->entitynum, bs->eye, bs->viewangles, 360, i);
		if (vis > 0) return qtrue;
	}
	return qfalse;
}

/*
==================
BotValidChatPosition
==================
*/
int BotValidChatPosition(bot_state_t *bs) {
	vec3_t point, start, end, mins, maxs;
	bsp_trace_t trace;

	//if the bot is dead all positions are valid
	if (BotIsDead(bs)) return qtrue;
	//never start chatting with a powerup
	if (bs->inventory[INVENTORY_QUAD] ||
		bs->inventory[INVENTORY_HASTE] ||
		bs->inventory[INVENTORY_INVISIBILITY] ||
		bs->inventory[INVENTORY_REGEN] ||
		bs->inventory[INVENTORY_FLIGHT]) return qfalse;
	//must be on the ground
	//if (bs->cur_ps.groundEntityNum != ENTITYNUM_NONE) return qfalse;
	//do not chat if in lava or slime
	VectorCopy(bs->origin, point);
	point[2] -= 24;
	if (gi.pointcontents(point,bs->entitynum) & (CONTENTS_LAVA|CONTENTS_SLIME)) return qfalse;
	//do not chat if under water
	VectorCopy(bs->origin, point);
	point[2] += 32;
	if (gi.pointcontents(point,bs->entitynum) & MASK_WATER) return qfalse;
	//must be standing on the world entity
	VectorCopy(bs->origin, start);
	VectorCopy(bs->origin, end);
	start[2] += 1;
	end[2] -= 10;
	gi.AAS_PresenceTypeBoundingBox(PRESENCE_CROUCH, mins, maxs);
	BotAI_Trace(&trace, start, mins, maxs, end, bs->client, MASK_SOLID);
	if (trace.ent != ENTITYNUM_WORLD) return qfalse;
	//the bot is in a position where it can chat
	return qtrue;
}

/*
==================
BotChat_EnterGame
==================
*/
int BotChat_EnterGame(bot_state_t *bs) {
	char name[32];
	float rnd;

	if (bot_nochat.integer) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	//don't chat in teamplay
	if (TeamPlayIsOn()) return qfalse;
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_ENTEREXITGAME, 0, 1);
	if (!bot_fastchat.integer) {
		if (random() > rnd) return qfalse;
	}
	if (BotNumActivePlayers() <= 1) return qfalse;
	if (!BotValidChatPosition(bs)) return qfalse;
	BotAI_BotInitialChat(bs, "game_enter",
				EasyClientName(bs->client, name, 32),	// 0
				BotRandomOpponentName(bs),				// 1
				"[invalid var]",						// 2
				"[invalid var]",						// 3
				BotMapTitle(),							// 4
				NULL);
	bs->lastchat_time = FloatTime();
	bs->chatto = CHAT_ALL;
	return qtrue;
}

/*
==================
BotChat_ExitGame
==================
*/
int BotChat_ExitGame(bot_state_t *bs) {
	char name[32];
	float rnd;

	if (bot_nochat.integer) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	//don't chat in teamplay
	if (TeamPlayIsOn()) return qfalse;
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_ENTEREXITGAME, 0, 1);
	if (!bot_fastchat.integer) {
		if (random() > rnd) return qfalse;
	}
	if (BotNumActivePlayers() <= 1) return qfalse;
	//
	BotAI_BotInitialChat(bs, "game_exit",
				EasyClientName(bs->client, name, 32),	// 0
				BotRandomOpponentName(bs),				// 1
				"[invalid var]",						// 2
				"[invalid var]",						// 3
				BotMapTitle(),							// 4
				NULL);
	bs->lastchat_time = FloatTime();
	bs->chatto = CHAT_ALL;
	return qtrue;
}

/*
==================
BotChat_StartLevel
==================
*/
int BotChat_StartLevel(bot_state_t *bs) {
	char name[32];
	float rnd;

	if (bot_nochat.integer) return qfalse;
	if (BotIsObserver(bs)) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	//don't chat in teamplay
	if (TeamPlayIsOn()) {
	    gi.EA_Command(bs->client, "vtaunt");
	    return qfalse;
	}
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_STARTENDLEVEL, 0, 1);
	if (!bot_fastchat.integer) {
		if (random() > rnd) return qfalse;
	}
	if (BotNumActivePlayers() <= 1) return qfalse;
	BotAI_BotInitialChat(bs, "level_start",
				EasyClientName(bs->client, name, 32),	// 0
				NULL);
	bs->lastchat_time = FloatTime();
	bs->chatto = CHAT_ALL;
	return qtrue;
}

/*
==================
BotChat_EndLevel
==================
*/
int BotChat_EndLevel(bot_state_t *bs) {
	char name[32];
	float rnd;

	if (bot_nochat.integer) return qfalse;
	if (BotIsObserver(bs)) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	// teamplay
	if (TeamPlayIsOn()) 
	{
		if (BotIsFirstInRankings(bs)) {
			gi.EA_Command(bs->client, "vtaunt");
		}
		return qtrue;
	}
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_STARTENDLEVEL, 0, 1);
	if (!bot_fastchat.integer) {
		if (random() > rnd) return qfalse;
	}
	if (BotNumActivePlayers() <= 1) return qfalse;
	//
	if (BotIsFirstInRankings(bs)) {
		BotAI_BotInitialChat(bs, "level_end_victory",
				EasyClientName(bs->client, name, 32),	// 0
				BotRandomOpponentName(bs),				// 1
				"[invalid var]",						// 2
				BotLastClientInRankings(),				// 3
				BotMapTitle(),							// 4
				NULL);
	}
	else if (BotIsLastInRankings(bs)) {
		BotAI_BotInitialChat(bs, "level_end_lose",
				EasyClientName(bs->client, name, 32),	// 0
				BotRandomOpponentName(bs),				// 1
				BotFirstClientInRankings(),				// 2
				"[invalid var]",						// 3
				BotMapTitle(),							// 4
				NULL);
	}
	else {
		BotAI_BotInitialChat(bs, "level_end",
				EasyClientName(bs->client, name, 32),	// 0
				BotRandomOpponentName(bs),				// 1
				BotFirstClientInRankings(),				// 2
				BotLastClientInRankings(),				// 3
				BotMapTitle(),							// 4
				NULL);
	}
	bs->lastchat_time = FloatTime();
	bs->chatto = CHAT_ALL;
	return qtrue;
}

/*
==================
BotChat_Death
==================
*/
int BotChat_Death(bot_state_t *bs) {
	char name[32];
	float rnd;

	if (bot_nochat.integer) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_DEATH, 0, 1);
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	//if fast chatting is off
	if (!bot_fastchat.integer) {
		if (random() > rnd) return qfalse;
	}
	if (BotNumActivePlayers() <= 1) return qfalse;
	//
	if (bs->lastkilledby >= 0 && bs->lastkilledby < maxclients->integer)
		EasyClientName(bs->lastkilledby, name, 32);
	else
		strcpy(name, "[world]");
	//
	if (TeamPlayIsOn() && BotSameTeam(bs, bs->lastkilledby)) {
		if (bs->lastkilledby == bs->client) return qfalse;
		BotAI_BotInitialChat(bs, "death_teammate", name, NULL);
		bs->chatto = CHAT_TEAM;
	}
	else
	{
		const char *enemyName;
		char meansOfDeathName[ 64 ];

		// Build the chat type

		strcpy( meansOfDeathName, "death_" );
		strcat( meansOfDeathName, MOD_NumToName( bs->botdeathtype ) );

		// Determine which enemy to char to

		if ( ( bs->botdeathtype == MOD_DROWN ) ||
			 ( bs->botdeathtype == MOD_LAVA ) ||
			 ( bs->botdeathtype == MOD_SUICIDE ) ||
			 ( bs->botdeathtype == MOD_SLIME ) ||
			 ( bs->botdeathtype == MOD_FALLING ) ||
			 ( bs->botdeathtype == MOD_CRUSH ) ) {
			enemyName = BotRandomOpponentName( bs );
		}
		else {
			enemyName = name;
		}

		// Try to chat

		if ( G_Random() < 0.25f ) {
			gi.EA_Command( bs->client, "taunt 2" );
		}
		else if ( gi.BotNumInitialChats( bs->cs, meansOfDeathName ) ) {
			BotAI_BotInitialChat( bs, meansOfDeathName, enemyName, BotWeaponNameForMeansOfDeath( bs->botdeathtype ), NULL );
		}
		else if ( ( random() < gi.Characteristic_BFloat( bs->character, CHARACTERISTIC_CHAT_INSULT, 0, 1 ) ) &&
				  ( gi.BotNumInitialChats( bs->cs, "death_insult" ) ) ) {
			BotAI_BotInitialChat(bs, "death_insult", enemyName, BotWeaponNameForMeansOfDeath( bs->botdeathtype ), NULL);
		}
		else if ( gi.BotNumInitialChats( bs->cs, "death_praise" ) ) {
			BotAI_BotInitialChat(bs, "death_praise", enemyName, BotWeaponNameForMeansOfDeath( bs->botdeathtype ), NULL);
		}

		/* 
		//teamplay
//		if (TeamPlayIsOn()) {
//			gi.EA_Command(bs->client, "vtaunt");
//			return qtrue;
//		}
		//

//		if (bs->botdeathtype == MOD_WATER)
//			BotAI_BotInitialChat(bs, "death_drown", BotRandomOpponentName(bs), NULL);
//		else 
			if (bs->botdeathtype == MOD_SLIME)
			BotAI_BotInitialChat(bs, "death_slime", BotRandomOpponentName(bs), NULL);
		else if (bs->botdeathtype == MOD_LAVA)
			BotAI_BotInitialChat(bs, "death_lava", BotRandomOpponentName(bs), NULL);
		else if (bs->botdeathtype == MOD_FALLING)
			BotAI_BotInitialChat(bs, "death_cratered", BotRandomOpponentName(bs), NULL);

		else if (bs->botsuicide || //all other suicides by own weapon
				bs->botdeathtype == MOD_CRUSH ||
				bs->botdeathtype == MOD_SUICIDE) //  ||
//				bs->botdeathtype == MOD_TARGET_LASER ||
//				bs->botdeathtype == MOD_TRIGGER_HURT ||
//				bs->botdeathtype == MOD_UNKNOWN)
			BotAI_BotInitialChat(bs, "death_suicide", BotRandomOpponentName(bs), NULL);
		else if (bs->botdeathtype == MOD_TELEFRAG)
			BotAI_BotInitialChat(bs, "death_telefrag", name, NULL);
#ifdef MISSIONPACKBOTTODO
		else if (bs->botdeathtype == MOD_KAMIKAZE && gi.BotNumInitialChats(bs->cs, "death_kamikaze"))
			BotAI_BotInitialChat(bs, "death_kamikaze", name, NULL);
#endif

		else {
			if ((//bs->botdeathtype == MOD_IMOD ||
				bs->botdeathtype == MOD_VAPORIZE_DISRUPTOR ||
				bs->botdeathtype == MOD_VAPORIZE_COMP ||
				bs->botdeathtype == MOD_VAPORIZE ||
				bs->botdeathtype == MOD_EXPLOSION) && random() < 0.5) {

				if (bs->botdeathtype == MOD_EXPLOSION)
					BotAI_BotInitialChat(bs, "death_explosion",
							name,												// 0
							BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
							NULL);
				else
					BotAI_BotInitialChat(bs, "death_vaporize",
							name,												// 0
							BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
							NULL);
			}

			//choose between insult and praise
			else 
				if (random() < gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_INSULT, 0, 1)) {
				BotAI_BotInitialChat(bs, "death_insult",
							name,												// 0
							BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
							NULL);
			}
			else {
				BotAI_BotInitialChat(bs, "death_praise",
							name,												// 0
							BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
							NULL);
			}
		} */
		bs->chatto = CHAT_ALL;
	}
	bs->lastchat_time = FloatTime();
	return qtrue;
  }

/*
==================
BotChat_Kill
==================
*/
int BotChat_Kill(bot_state_t *bs) {
	char name[32];
	float rnd;

	if (bot_nochat.integer) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_KILL, 0, 1);
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	//if fast chat is off
	if (!bot_fastchat.integer) {
		if (random() > rnd) return qfalse;
	}
	if (bs->lastkilledplayer == bs->client) return qfalse;
	if (BotNumActivePlayers() <= 1) return qfalse;
	if (!BotValidChatPosition(bs)) return qfalse;
	//
	if (BotVisibleEnemies(bs)) return qfalse;
	//
	EasyClientName(bs->lastkilledplayer, name, 32);
	//
	bs->chatto = CHAT_ALL;

	if ( TeamPlayIsOn() && BotSameTeam( bs, bs->lastkilledplayer ) ) {
		BotAI_BotInitialChat(bs, "kill_teammate", name, NULL);
		bs->chatto = CHAT_TEAM;
	}
	else {
		char meansOfDeathName[ 64 ];

		// Build the chat type

		strcpy( meansOfDeathName, "kill_" );
		strcat( meansOfDeathName, MOD_NumToName( bs->botdeathtype ) );

		// Try to chat

		if ( G_Random() < 0.25f ) {
			gi.EA_Command( bs->client, "taunt 1" );
		}
		else if ( gi.BotNumInitialChats( bs->cs, meansOfDeathName ) ) {
			BotAI_BotInitialChat( bs, meansOfDeathName, name, NULL );
		}
		else if ( ( random() < gi.Characteristic_BFloat( bs->character, CHARACTERISTIC_CHAT_INSULT, 0, 1 ) ) &&
				  ( gi.BotNumInitialChats( bs->cs, "kill_insult" ) ) ) {
			BotAI_BotInitialChat(bs, "kill_insult", name, NULL);
		}
		else if ( gi.BotNumInitialChats( bs->cs, "kill_praise" ) ) {
			BotAI_BotInitialChat(bs, "kill_praise", name, NULL);
		}

		/* if (bs->enemydeathtype == MOD_IMOD) {
			BotAI_BotInitialChat(bs, "kill_imod", name, NULL);
		}
		else if ( (bs->enemydeathtype == MOD_VAPORIZE) ||
			(bs->enemydeathtype == MOD_VAPORIZE_COMP) ||
			(bs->enemydeathtype == MOD_VAPORIZE_DISRUPTOR) ) {
			BotAI_BotInitialChat(bs, "kill_vaporize", name, NULL);
		}
		else if (bs->enemydeathtype == MOD_EXPLOSION) {
			BotAI_BotInitialChat(bs, "kill_explosion", name, NULL);
		}
		if (bs->enemydeathtype == MOD_TELEFRAG) {
			BotAI_BotInitialChat(bs, "kill_telefrag", name, NULL);
		}
#ifdef MISSIONPACKBOTTODO
		else if (bs->botdeathtype == MOD_KAMIKAZE && gi.BotNumInitialChats(bs->cs, "kill_kamikaze"))
			BotAI_BotInitialChat(bs, "kill_kamikaze", name, NULL);
#endif
		//choose between insult and praise
		else if (random() < gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_INSULT, 0, 1)) {
			BotAI_BotInitialChat(bs, "kill_insult", name, NULL);
		}
		else {
			BotAI_BotInitialChat(bs, "kill_praise", name, NULL);
		} */
	}
	bs->lastchat_time = FloatTime();
	return qtrue;
}

/*
==================
BotChat_EnemySuicide
==================
*/
int BotChat_EnemySuicide(bot_state_t *bs) {
//	char name[32];
//	float rnd;

	return qfalse;
	/* FIXME
	if (bot_nochat.integer) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	if (BotNumActivePlayers() <= 1) return qfalse;
	//
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_KILL, 0, 1);
	//don't chat in teamplay
	if (TeamPlayIsOn()) return qfalse;
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	//if fast chat is off
	if (!bot_fastchat.integer) {
		if (random() > rnd) return qfalse;
	}
	if (!BotValidChatPosition(bs)) return qfalse;
	//
	if (BotVisibleEnemies(bs)) return qfalse;
	//
	if (bs->enemy >= 0) EasyClientName(bs->enemy, name, 32);
	else strcpy(name, "");
	BotAI_BotInitialChat(bs, "enemy_suicide", name, NULL);
	bs->lastchat_time = FloatTime();
	bs->chatto = CHAT_ALL;
	return qtrue;
*/
}

/*
==================
BotChat_HitTalking
==================
*/
int BotChat_HitTalking(bot_state_t *bs) {
//	char name[32], *weap;
//	int lasthurt_client;
//	float rnd;
/*

	if (bot_nochat.integer) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	if (BotNumActivePlayers() <= 1) return qfalse;
	lasthurt_client = g_entities[bs->client].client->lasthurt_client;
	if (!lasthurt_client) return qfalse;
	if (lasthurt_client == bs->client) return qfalse;
	//
	if (lasthurt_client < 0 || lasthurt_client >= MAX_CLIENTS) return qfalse;
	//
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_HITTALKING, 0, 1);
	//don't chat in teamplay
	if (TeamPlayIsOn()) return qfalse;
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	//if fast chat is off
	if (!bot_fastchat.integer) {
		if (random() > rnd * 0.5) return qfalse;
	}
	if (!BotValidChatPosition(bs)) return qfalse;
	//
	ClientName(g_entities[bs->client].client->lasthurt_client, name, sizeof(name));
	weap = BotWeaponNameForMeansOfDeath(g_entities[bs->client].client->lasthurt_client);
	//
	BotAI_BotInitialChat(bs, "hit_talking", name, weap, NULL);
	bs->lastchat_time = FloatTime();
	bs->chatto = CHAT_ALL;
*/
  return qtrue;
}

/*
==================
BotChat_HitNoDeath
==================
*/
int BotChat_HitNoDeath(bot_state_t *bs) {
//	char name[32], *weap;
//	float rnd;
//	int lasthurt_client;
//	aas_entityinfo_t entinfo;
/*
	lasthurt_client = g_entities[bs->client].client->lasthurt_client;
	if (!lasthurt_client) return qfalse;
	if (lasthurt_client == bs->client) return qfalse;
	//
	if (lasthurt_client < 0 || lasthurt_client >= MAX_CLIENTS) return qfalse;
	//
	if (bot_nochat.integer) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	if (BotNumActivePlayers() <= 1) return qfalse;
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_HITNODEATH, 0, 1);
	//don't chat in teamplay
	if (TeamPlayIsOn()) return qfalse;
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	//if fast chat is off
	if (!bot_fastchat.integer) {
		if (random() > rnd * 0.5) return qfalse;
	}
	if (!BotValidChatPosition(bs)) return qfalse;
	//
	if (BotVisibleEnemies(bs)) return qfalse;
	//
	BotEntityInfo(bs->enemy, &entinfo);
	if (EntityIsShooting(&entinfo)) return qfalse;
	//
	ClientName(lasthurt_client, name, sizeof(name));
	weap = BotWeaponNameForMeansOfDeath(g_entities[bs->client].client->lasthurt_mod);
	//
	BotAI_BotInitialChat(bs, "hit_nodeath", name, weap, NULL);
	bs->lastchat_time = FloatTime();
	bs->chatto = CHAT_ALL;
*/
  return qtrue;
  }

/*
==================
BotChat_HitNoKill
==================
*/
int BotChat_HitNoKill(bot_state_t *bs) {
//	char name[32], *weap;
//	float rnd;
//	aas_entityinfo_t entinfo;
	return qfalse; // FIXME
/*
	if (bot_nochat.integer) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	if (BotNumActivePlayers() <= 1) return qfalse;
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_HITNOKILL, 0, 1);
	//don't chat in teamplay
	if (TeamPlayIsOn()) return qfalse;
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	//if fast chat is off
	if (!bot_fastchat.integer) {
		if (random() > rnd * 0.5) return qfalse;
	}
	if (!BotValidChatPosition(bs)) return qfalse;
	//
	if (BotVisibleEnemies(bs)) return qfalse;
	//
	BotEntityInfo(bs->enemy, &entinfo);
	if (EntityIsShooting(&entinfo)) return qfalse;
	//
	ClientName(bs->enemy, name, sizeof(name));
	weap = BotWeaponNameForMeansOfDeath(g_entities[bs->enemy].client->lasthurt_mod);
	//
	BotAI_BotInitialChat(bs, "hit_nokill", name, weap, NULL);
	bs->lastchat_time = FloatTime();
	bs->chatto = CHAT_ALL;
	return qtrue;
*/
}

/*
==================
BotChat_Random
==================
*/
int BotChat_Random(bot_state_t *bs) {
	float rnd;
	char name[32];

	if (bot_nochat.integer) return qfalse;
	if (BotIsObserver(bs)) return qfalse;
	if (bs->lastchat_time > FloatTime() - TIME_BETWEENCHATTING) return qfalse;
	// don't chat in tournament mode
	if (gametype == GT_TOURNAMENT) return qfalse;
	//don't chat when doing something important :)
	if (bs->ltgtype == LTG_TEAMHELP ||
		bs->ltgtype == LTG_TEAMACCOMPANY ||
		bs->ltgtype == LTG_RUSHBASE) return qfalse;
	//
	rnd = gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_RANDOM, 0, 1);
	if (random() > bs->thinktime * 0.1) return qfalse;
	if (!bot_fastchat.integer) {
		if (random() > rnd) return qfalse;
		if (random() > 0.25) return qfalse;
	}
	if (BotNumActivePlayers() <= 1) return qfalse;
	//
	if (!BotValidChatPosition(bs)) return qfalse;
	//
	if (BotVisibleEnemies(bs)) return qfalse;
	//
	if (bs->lastkilledplayer == bs->client) {
		strcpy(name, BotRandomOpponentName(bs));
	}
	else {
		EasyClientName(bs->lastkilledplayer, name, sizeof(name));
	}
	if (TeamPlayIsOn()) {
		gi.EA_Command(bs->client, "vtaunt");
		return qfalse;			// don't wait
	}
	//
	if (random() < gi.Characteristic_BFloat(bs->character, CHARACTERISTIC_CHAT_MISC, 0, 1)) {
		BotAI_BotInitialChat(bs, "random_misc",
					BotRandomOpponentName(bs),	// 0
					name,						// 1
					"[invalid var]",			// 2
					"[invalid var]",			// 3
					BotMapTitle(),				// 4
					BotRandomWeaponName(),		// 5
					NULL);
	}
	else {
		BotAI_BotInitialChat(bs, "random_insult",
					BotRandomOpponentName(bs),	// 0
					name,						// 1
					"[invalid var]",			// 2
					"[invalid var]",			// 3
					BotMapTitle(),				// 4
					BotRandomWeaponName(),		// 5
					NULL);
	}
	bs->lastchat_time = FloatTime();
	bs->chatto = CHAT_ALL;
	return qtrue;
}

/*
==================
BotChatTime
==================
*/
float BotChatTime(bot_state_t *bs) {
	int cpm;

	cpm = gi.Characteristic_BInteger(bs->character, CHARACTERISTIC_CHAT_CPM, 1, 4000);

	return 0.0f;	//(float) gi.BotChatLength(bs->cs) * 30 / cpm;
}

/*
==================
BotChatTest
==================
*/
void BotChatTest(bot_state_t *bs) {

	char name[32];
//	char *weap;
	int num, i;

	num = gi.BotNumInitialChats(bs->cs, "game_enter");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "game_enter",
					EasyClientName(bs->client, name, 32),	// 0
					BotRandomOpponentName(bs),				// 1
					"[invalid var]",						// 2
					"[invalid var]",						// 3
					BotMapTitle(),							// 4
					NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "game_exit");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "game_exit",
					EasyClientName(bs->client, name, 32),	// 0
					BotRandomOpponentName(bs),				// 1
					"[invalid var]",						// 2
					"[invalid var]",						// 3
					BotMapTitle(),							// 4
					NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "level_start");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "level_start",
					EasyClientName(bs->client, name, 32),	// 0
					NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "level_end_victory");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "level_end_victory",
				EasyClientName(bs->client, name, 32),	// 0
				BotRandomOpponentName(bs),				// 1
				BotFirstClientInRankings(),				// 2
				BotLastClientInRankings(),				// 3
				BotMapTitle(),							// 4
				NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "level_end_lose");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "level_end_lose",
				EasyClientName(bs->client, name, 32),	// 0
				BotRandomOpponentName(bs),				// 1
				BotFirstClientInRankings(),				// 2
				BotLastClientInRankings(),				// 3
				BotMapTitle(),							// 4
				NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "level_end");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "level_end",
				EasyClientName(bs->client, name, 32),	// 0
				BotRandomOpponentName(bs),				// 1
				BotFirstClientInRankings(),				// 2
				BotLastClientInRankings(),				// 3
				BotMapTitle(),							// 4
				NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	EasyClientName(bs->lastkilledby, name, sizeof(name));
	num = gi.BotNumInitialChats(bs->cs, "death_drown");
	for (i = 0; i < num; i++)
	{
		//
		BotAI_BotInitialChat(bs, "death_drown", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "death_slime");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "death_slime", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "death_lava");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "death_lava", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "death_cratered");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "death_cratered", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "death_suicide");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "death_suicide", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "death_telefrag");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "death_telefrag", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "death_gauntlet");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "death_gauntlet",
				name,												// 0
				BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
				NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "death_rail");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "death_rail",
				name,												// 0
				BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
				NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "death_bfg");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "death_bfg",
				name,												// 0
				BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
				NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "death_insult");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "death_insult",
					name,												// 0
					BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
					NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "death_praise");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "death_praise",
					name,												// 0
					BotWeaponNameForMeansOfDeath(bs->botdeathtype),		// 1
					NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	//
	EasyClientName(bs->lastkilledplayer, name, 32);
	//
	num = gi.BotNumInitialChats(bs->cs, "kill_gauntlet");
	for (i = 0; i < num; i++)
	{
		//
		BotAI_BotInitialChat(bs, "kill_gauntlet", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "kill_rail");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "kill_rail", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "kill_telefrag");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "kill_telefrag", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "kill_insult");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "kill_insult", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "kill_praise");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "kill_praise", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "enemy_suicide");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "enemy_suicide", name, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
/*
	ClientName(g_entities[bs->client].client->lasthurt_client, name, sizeof(name));
	weap = BotWeaponNameForMeansOfDeath(g_entities[bs->client].client->lasthurt_client);
	num = gi.BotNumInitialChats(bs->cs, "hit_talking");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "hit_talking", name, weap, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "hit_nodeath");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "hit_nodeath", name, weap, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "hit_nokill");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "hit_nokill", name, weap, NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
*/
	//
	if (bs->lastkilledplayer == bs->client) {
		strcpy(name, BotRandomOpponentName(bs));
	}
	else {
		EasyClientName(bs->lastkilledplayer, name, sizeof(name));
	}
	//
	num = gi.BotNumInitialChats(bs->cs, "random_misc");
	for (i = 0; i < num; i++)
	{
		//
		BotAI_BotInitialChat(bs, "random_misc",
					BotRandomOpponentName(bs),	// 0
					name,						// 1
					"[invalid var]",			// 2
					"[invalid var]",			// 3
					BotMapTitle(),				// 4
					BotRandomWeaponName(),		// 5
					NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}
	num = gi.BotNumInitialChats(bs->cs, "random_insult");
	for (i = 0; i < num; i++)
	{
		BotAI_BotInitialChat(bs, "random_insult",
					BotRandomOpponentName(bs),	// 0
					name,						// 1
					"[invalid var]",			// 2
					"[invalid var]",			// 3
					BotMapTitle(),				// 4
					BotRandomWeaponName(),		// 5
					NULL);
		gi.BotEnterChat(bs->cs, 0, CHAT_ALL);
	}

}
