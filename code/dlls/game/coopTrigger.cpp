//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Based upon code from the HaZardModding Coop Mod Level Scripts created at 2006
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING PLAYER HANDLING RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2018 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]


#include "_pch_cpp.h"
#include "actor.h"
#include "coopTrigger.hpp"
#include "mp_manager.hpp"
#include "coopObjectives.hpp"
#include "coopStory.hpp"
#include "coopReturn.hpp"
#include "coopArmory.hpp"
/*
#include "coopAlias.hpp"
#include "coopParser.hpp"
#include "coopModel.hpp"
#include "coopRadar.hpp"
#include "coopCheck.hpp"
#include "coopText.hpp"
#include "coopServer.hpp"
#include "coopPlayer.hpp"
#include "coopClass.hpp"
#include "coopHuds.hpp"
#include "level.h"
#include "player.h"
#include "weapon.h"
#include "mp_modeDm.hpp"
#include "mp_modeTeamDm.hpp"
#include "mp_modeCtf.hpp"
#include <qcommon/gameplaymanager.h>
*/

extern Event EV_SetArchetype;
extern Event EV_Warp;
extern Event EV_Idle;
extern Event EV_Actor_Immortal;
extern Event EV_Actor_AIOn;
extern Event EV_Actor_Suicide;
extern Event EV_Actor_ForgetEnemies;
extern Event EV_Actor_PlayDialog;
extern Event EV_Actor_HeadWatch;
extern Event EV_Actor_ResetHead;
extern Event EV_Actor_Idle;
extern Event EV_Sentient_StopFire;
extern Event EV_Camera_Cut;
extern Event EV_Contents;
extern Event EV_Camera_Follow;
extern Event EV_EntityRenderEffects;
extern Event EV_SetMissionObjective;
extern Event EV_Door_Open;
extern Event EV_Door_Close;
extern Event EV_Door_Unlock;
extern Event EV_Door_Lock;
extern Event EV_ScriptThread_FadeOut;
extern Event EV_Trigger_SetTriggerable;
extern Event EV_Trigger_SetNotTriggerable;
extern Event EV_Player_SetViewAngles;
extern Event EV_Player_ResetState;
extern Event EV_World_RemoveAvailableViewMode;
extern Event EV_World_AddAvailableViewMode;

/*
dropactorweapon
move (script_object)
dynamiclight, lightstyle, light_intensity, light_fade, getlightintensity, 
kill, damage/means of death, inflictor,
GATHERTEAM	(uservar1 countdown)
*/
/* ############################## GAME SAVE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_game_save(1 1 0) (-8 -8 -8) (8 8 8)
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Creates a savegame
[ ! ] Only works in Singleplayer

"uservar1" sets the name of the savegame (minimum 3, maximum 32 Letters)
*/

CLASS_DECLARATION( Trigger , TriggerCoopGameSave , "trigger_coop_game_save" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopGameSave::Activate } ,
	{ NULL, NULL }
};
TriggerCoopGameSave::TriggerCoopGameSave()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopGameSave::Activate( Event *ev )
{
	if ( g_gametype->integer != GT_SINGLE_PLAYER )
		return;

	cvar_t *cvar = gi.cvar_get( "com_canAutoSaveNow" );
	
	if ( !cvar )
		return;

	if ( cvar->integer != 1 )
	{
		gi.Printf( "TriggerCoopGameSave::Activate - Can't save right now! (com_canAutoSaveNow != 1)\n" );
		return;
	}

	Player* player = multiplayerManager.getPlayer( 0 );
	if ( !player )
		return;

	if ( player->health <= 0 )
	{
		gi.Printf( "TriggerCoopGameSave::Activate - Can't save right now! (Player is dead)\n" );
		return;
	}

	str sSaveName = "";
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );

	if ( !entityVar1 )
	{
		gi.Printf( "TriggerCoopGameSave::Activate - Missing name for the save-game in uservar1!\n" );
		return;
	}

	sSaveName = entityVar1->stringValue();

	if ( sSaveName.length() < 3 )
	{
		gi.Printf( "TriggerCoopGameSave::Activate - Save-game in uservar1 to short (minimum 3 Letters)!\n" );
		return;
	}
	else if ( sSaveName.length() > 32 )
	{
		gi.Printf( "TriggerCoopGameSave::Activate - Save-game in uservar1 to long, WAS RECIFIED (maximum 32 Letters)!\n" );
		sSaveName = coop_returnStringFromWithLength( sSaveName, 0 , 32 );
	}

	gi.SendConsoleCommand( va("ui_savegamewithname \"%s\"\n", sSaveName.c_str() ) );

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## WORLD WATER ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_world_water(1 1 0) (-8 -8 -8) (8 8 8)
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] sets Water Color and Water alpha

"uservar1" sets water color [vector](0 0 0 - 1 1 1)
"uservar2" sets water alpha ( 0-1 )
*/

CLASS_DECLARATION( Trigger , TriggerCoopWorldWater , "trigger_coop_world_water" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopWorldWater::Activate } ,
	{ NULL, NULL }
};
TriggerCoopWorldWater::TriggerCoopWorldWater()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopWorldWater::Activate( Event *ev )
{
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );

	if ( !entityVar1 && entityVar2 )
	{
		gi.Printf( "TriggerCoopWorldWater::Activate - You need to have at least COLOR or ALPHA set!" );
		return;
	}
	
	if ( entityVar1 )
		level.water_color = coop_returnVectorFromString( entityVar1->stringValue() );

	if ( entityVar2 )
		level.water_alpha = entityVar2->floatValue();

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## WORLD BROKEN ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_world_broken(1 1 0) (-8 -8 -8) (8 8 8) REMOVE
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Adds a Broken Item/Weapon to the Level
[ ! ] May not work with all weapons and items!

"uservar1-10" sets name of broken item

:::WEAPONS::::::::
attrex-rifle
batleth
burstrifle
compressionrifle
drull-staff
enterprise
fieldassaultrifle
grenadelauncher
IMod
phaser
phaser-STX
photon
rom-datapad
rom-radgun
rom-disruptor
sniperrifle
tetryon
tricorder
tricorder-STX
:::TIKI PATH EXAMPLE:::
models/weapons/worldmodel-IMod.tik
*/

CLASS_DECLARATION( Trigger , TriggerCoopWorldBroken , "trigger_coop_world_broken" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopWorldBroken::Activate } ,
	{ NULL, NULL }
};
TriggerCoopWorldBroken::TriggerCoopWorldBroken()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopWorldBroken::Activate( Event *ev )
{
#define REMOVE (1<<0)

	int iItem;
	for ( iItem = 1; iItem <= 10; iItem++ ) {
		ScriptVariable *entityVar = entityVars.GetVariable( va( "uservar%d" , iItem ) );

		if ( !entityVar )
			continue;

		str sItem = "";
		sItem = entityVar->stringValue();
		if ( !sItem.length() )
		{
			gi.Printf( "TriggerCoopWorldViewmode::Activate - uservar%d set but empty!\n" , iItem );
			continue;
		}

		if ( upgStrings.containsAt( sItem , ".tik" ) == -1 )
		{
			sItem = va( "models/weapons/worldmodel-%s.tik" , sItem.c_str() );
		}
		sItem = coop_armoryReturnWeaponName( sItem );

		if ( spawnflags & REMOVE )
		{
			world->removeBrokenThing( sItem.c_str() );
		}
		else
		{
			world->addBrokenThing( sItem.c_str() );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## WORLD VIEWMODE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_world_viewmode(1 1 0) (-8 -8 -8) (8 8 8) REMOVE
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Adds Tricorder-Viewmode the the level

"uservar1-6" Viewmode-Name

:::AVIALABLE VIEWMODES:::::::::
tracegas
structuralintegrity
phaseshift
torpedostrike
enemydetect
tripwire
*/

CLASS_DECLARATION( Trigger , TriggerCoopWorldViewmode , "trigger_coop_world_viewmode" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopWorldViewmode::Activate } ,
	{ NULL, NULL }
};
TriggerCoopWorldViewmode::TriggerCoopWorldViewmode()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopWorldViewmode::Activate( Event *ev )
{
#define REMOVE (1<<0)

	int iItem;
	for ( iItem = 1; iItem <= 7; iItem++ ) {
		ScriptVariable *entityVar = entityVars.GetVariable( va("uservar%d", iItem ) );
		
		if ( !entityVar )
			continue;

		str sHud = "";
		sHud = entityVar->stringValue();
		if ( !sHud.length() )
		{
			gi.Printf( "TriggerCoopWorldViewmode::Activate - uservar%d set but empty!\n", iItem );
			continue;
		}

		if ( spawnflags & REMOVE )
		{
			Event *eve = new Event( EV_World_RemoveAvailableViewMode );
			eve->AddString( sHud.c_str() );
			world->ProcessEvent( eve );
		}
		else
		{
			Event *eve = new Event( EV_World_AddAvailableViewMode );
			eve->AddString( sHud.c_str() );
			world->ProcessEvent( eve );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## WORLD FOG ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_world_fog(1 1 0) (-8 -8 -8) (8 8 8)
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sets fog on the world

"uservar1" sets distance the player can see (min: 512, Max: 10000 )
"uservar2" sets distance entities are visible  (min: 2048, Max: 10000 )
"uservar3" set the fog color (from 0 0 0 to 1 1 1)
*/

CLASS_DECLARATION( Trigger , TriggerCoopWorldFog , "trigger_coop_world_fog" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopWorldFog::Activate } ,
	{ NULL, NULL }
};
TriggerCoopWorldFog::TriggerCoopWorldFog()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopWorldFog::Activate( Event *ev )
{
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	ScriptVariable *entityVar3 = entityVars.GetVariable( "uservar3" );

	if ( entityVar1 )
	{
		world->farplane_distance = entityVar1->floatValue();
		world->UpdateFog();
	}

	if ( entityVar2 )
	{
		world->entity_fade_dist = entityVar2->floatValue();
		world->UpdateEntityFadeDist();
	}

	if ( entityVar3 )
	{
		world->farplane_color = coop_returnVectorFromString( entityVar3->stringValue() );
		world->UpdateFog();
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## WORLD WEATHER ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_world_weather(1 1 0) (-8 -8 -8) (8 8 8) RAIN RAIN_PLAIN SNOW
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Stops or starts weather on the level, if nothing is selected it will stop

[ if ] RAIN is set it will rain
[ if ] RAIN_PLAINE is set it will rain (without splash effects)
[ if ] SNOW is set it wil snow

"uservar1" sets amount of weather (min: 0, Max: 1000 )
*/
CLASS_DECLARATION( Trigger , TriggerCoopWorldWeather , "trigger_coop_world_weather" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopWorldWeather::Activate } ,
	{ NULL, NULL }
};
TriggerCoopWorldWeather::TriggerCoopWorldWeather()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopWorldWeather::Activate( Event *ev )
{
#define RAIN (1<<0) 
#define RAIN_PLAIN (1<<1) 
#define SNOW (1<<2) 
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );

	if ( entityVar1 )
	{
		world->weather.intensity = entityVar1->intValue();
	}

	if ( spawnflags & RAIN )
	{
		world->weather.type = WEATHER_RAIN;
	}
	else if ( spawnflags & RAIN_PLAIN )
	{
		world->weather.type = WEATHER_RAIN_PLAIN;
	}
	else if ( spawnflags & SNOW )
	{
		world->weather.type = WEATHER_SNOW;
	}
	else
	{
		world->weather.intensity = 0;
	}

	world->UpdateWeather();

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## WORLD PHYSICS ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_world_physics(1 1 0) (-8 -8 -8) (8 8 8)
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sets the world its physics parameters

"uservar1" sets gravity (min: 50, Max: 1600 )
"uservar2" sets maxSpeed (min: 50, Max: 2000 )
"uservar3" sets airAccelerate (min: 1, Max: 3 )
*/
CLASS_DECLARATION( Trigger , TriggerCoopWorldPhysics , "trigger_coop_world_physics" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopWorldPhysics::Activate } ,
	{ NULL, NULL }
};
TriggerCoopWorldPhysics::TriggerCoopWorldPhysics()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopWorldPhysics::Activate( Event *ev )
{
#define MIN_GRAVITY 50.0f
#define MAX_GRAVITY 1600.0f
#define MIN_MOVE 50.0f
#define MAX_MOVE 2000.0f
#define MIN_ACCELERATE 1.0f
#define MAX_ACCELERATE 3.0f

	float uservar1 = -1.0f;
	float uservar2 = -1.0f;
	float uservar3 = -1.0f;
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	ScriptVariable *entityVar3 = entityVars.GetVariable( "uservar3" );
	
	if ( entityVar1 )
		uservar1 = entityVar1->floatValue();

	if ( entityVar2 )
		uservar2 = entityVar2->floatValue();

	if ( entityVar3 )
		uservar3 = entityVar3->floatValue();

	if ( uservar1 != -1 && uservar1 < MIN_GRAVITY )
		uservar1 = MIN_GRAVITY;
	else if ( uservar1 > MAX_GRAVITY )
		uservar1 = MAX_GRAVITY;
	
	if ( uservar2 != -1 && uservar2 < MIN_MOVE )
		uservar2 = MIN_MOVE;
	else if ( uservar2 > MAX_MOVE )
		uservar2 = MAX_MOVE;

	if ( uservar3 != -1 && uservar3 < MIN_ACCELERATE )
		uservar3 = MIN_ACCELERATE;
	else if ( uservar3 > MAX_ACCELERATE )
		uservar3 = MAX_ACCELERATE;

	world->setPhysicsVar( "gravity" , uservar1 );
	world->setPhysicsVar( "maxSpeed" , uservar2 );
	world->setPhysicsVar( "airAccelerate" , uservar3 );

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## TRIGGER ENABLE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_trigger_enable(1 1 0) (-8 -8 -8) (8 8 8) DISABLE
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Enables a trigger so it can be activated

[ if ] DISABLE is set the trigger will be disabled instead

"uservar1" sets the targetname of the trigger to enable/disable
*/
CLASS_DECLARATION( Trigger , TriggerCoopTriggerEnable , "trigger_coop_trigger_enable" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopTriggerEnable::Activate } ,
	{ NULL, NULL }
};
TriggerCoopTriggerEnable::TriggerCoopTriggerEnable()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopTriggerEnable::Activate( Event *ev )
{
#define DISABLE (1<<0)

	str uservar1 = "";
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	if ( entityVar1 )
		uservar1 = entityVar1->stringValue();

	//uservar1 set
	if ( uservar1.length() < 1 )
	{
		gi.Printf( "TriggerCoopTriggerEnable::Activate - No targetname set in uservar1!\n" );
		return;
	}

	Entity *ent = coop_returnEntity( uservar1 );

	//does exist
	if ( !ent )
	{
		gi.Printf( "TriggerCoopTriggerEnable::Activate - Entity ($%s) was not found!\n" , uservar1.c_str() );
		return;
	}

	//is of type trigger
	if ( !ent->isSubclassOf( Trigger ) )
	{
		gi.Printf( "TriggerCoopTriggerEnable::Activate - Entity ($%s) needs to be of a subclass of: Trigger!\n" , uservar1.c_str() );
		return;
	}

	Trigger *trigger = ( Trigger* )ent;

	if ( spawnflags & DISABLE )
	{
		Event *eve = new Event( EV_Trigger_SetNotTriggerable );
		trigger->ProcessEvent( eve );
	}
	else
	{
		Event *evAnim = new Event( EV_Trigger_SetTriggerable );
		trigger->ProcessEvent( evAnim );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ENTITY ARCHETYPE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_archetype(1 1 0) (-8 -8 -8) (8 8 8) REMOVE
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sets Archetype of Entity

[ if ] REMOVE is set any archetype will be removed instead

"uservar1" Specifies Archetype to set
"uservar2-10" Entity targetnames meant to set Archetype to
"target" Entity to set Archetype
*/

CLASS_DECLARATION( Trigger , TriggerCoopEntityArchetype , "trigger_coop_entity_archetype" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityArchetype::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityArchetype::TriggerCoopEntityArchetype()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityArchetype::Activate( Event *ev )
{
#define REMOVE (1<<0)
	str sArcheType = "";
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );

	if ( spawnflags & REMOVE )
	{
		//placeholder
	}
	else
	{
		if ( !entityVar1 )
		{
			gi.Printf( "TriggerCoopEntityArchetype::Activate - uservar1 is not set, should hold name of Archetype!\n" );
			return;
		}
		sArcheType = entityVar1->stringValue();
	}

	int iValid = 0;
	int iItem;
	for ( iItem = 2; iItem <= 10; iItem++ ) {
		Entity *ent = NULL;
		ScriptVariable *entityVar = entityVars.GetVariable( va( "uservar%d" , iItem ) );

		if ( !entityVar )
			continue;

		str sTargetname = entityVar->stringValue();
		if ( !sTargetname.length() )
		{
			gi.Printf( "TriggerCoopEntityArchetype::Activate - uservar%d set but empty!\n" , iItem );
			continue;
		}

		ent = coop_returnEntity( sTargetname.c_str() );

		if ( !ent )
		{
			gi.Printf( "TriggerCoopEntityArchetype::Activate - Entity ($%s) in uservar%d was not found!\n" , sTargetname.c_str() , iItem );
			continue;
		}

		Event *even = new Event( EV_SetArchetype );
		even->AddString( sArcheType );
		ent->ProcessEvent( even );

		iValid++;
	}

	//handle target
	if ( iValid == 0 )
	{
		Entity *ent = NULL;
		ent = coop_returnEntity( target.c_str() );

		if ( !ent )
		{
			gi.Printf( "TriggerCoopEntityArchetype::Activate - Target Entity ($%s) was not found!\n" , target.c_str() );
			return;
		}

		Event *even = new Event( EV_SetArchetype );
		even->AddString( sArcheType );
		ent->ProcessEvent( even );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ENTITY BIND ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_bind(1 1 0) (-8 -8 -8) (8 8 8) UNBIND
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Binds a entity to the specified (master) entity (this is used for script objects)

"uservar1" Specifies master entity 
"uservar2-10" Entity targetnames meant to be bound to master (will overwrite target)
"target" Entity to bind to master
*/

CLASS_DECLARATION( Trigger , TriggerCoopEntityBind , "trigger_coop_entity_bind" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityBind::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityBind::TriggerCoopEntityBind()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityBind::Activate( Event *ev )
{
#define UNBIND (1<<0)

	Entity *master = NULL;
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );

	if ( spawnflags & UNBIND )
	{
		//placeholder
	}
	else
	{
		if ( !entityVar1 )
		{
			gi.Printf( "TriggerCoopEntityBind::Activate - uservar1 is not set, should hold name of master entity !\n" );
			return;
		}

		master = coop_returnEntity( entityVar1->stringValue() );

		if ( !master )
		{
			gi.Printf( "TriggerCoopEntityBind::Activate - Master Entity (%s) in uservar1 could not be found!\n", entityVar1->stringValue() );
			return;
		}
	}

	int iValid = 0;
	int iItem;
	for ( iItem = 2; iItem <= 10; iItem++ ) {
		Entity *ent = NULL;
		ScriptVariable *entityVar = entityVars.GetVariable( va( "uservar%d" , iItem ) );

		if ( !entityVar )
			continue;

		str sTargetname = entityVar->stringValue();
		if ( !sTargetname.length() )
		{
			gi.Printf( "TriggerCoopEntityViewmode::Activate - uservar%d set but empty!\n" , iItem );
			continue;
		}

		ent = coop_returnEntity( sTargetname.c_str() );

		if ( !ent )
		{
			gi.Printf( "TriggerCoopEntityViewmode::Activate - Entity ($%s) in uservar%d was not found!\n" , sTargetname.c_str() , iItem );
			continue;
		}

		if ( spawnflags & UNBIND )
		{
			ent->unbind();
		}
		else
		{
			ent->bind( master );
		}

		iValid++;
	}

	//handle target
	if ( iValid == 0 )
	{
		Entity *ent = NULL;
		ent = coop_returnEntity( target.c_str() );

		if ( !ent )
		{
			gi.Printf( "TriggerCoopEntityViewmode::Activate - Target Entity ($%s) was not found!\n" , target.c_str() );
			return;
		}
		if ( spawnflags & UNBIND )
		{
			ent->unbind();
		}
		else
		{
			ent->bind( master );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ENTITY VIEWMODE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_viewmode(1 1 0) (-8 -8 -8) (8 8 8) REMOVE
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Makes a Entity visible for players only if in specified Tricorder-Viewmode

"uservar1" Viewmode-Name
"uservar2-10" Entity targetnames to apply the viewmode to (will overwrite target)
"target" Entity to apply the viewmode to

:::AVIALABLE VIEWMODES:::::::::
tracegas
structuralintegrity
phaseshift
nightvision
torpedostrike
enemydetect
tripwire
*/

CLASS_DECLARATION( Trigger , TriggerCoopEntityViewmode , "trigger_coop_entity_viewmode" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityViewmode::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityViewmode::TriggerCoopEntityViewmode()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityViewmode::Activate( Event *ev )
{
#define REMOVE (1<<0)

	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );

	if ( !entityVar1 )
	{
		gi.Printf( "TriggerCoopEntityViewmode::Activate - uservar1 is not set, should hold name of viewmode !\n" );
		return;
	}

	str sViewmode = entityVar1->stringValue();

	if ( !sViewmode.length() )
	{
		gi.Printf( "TriggerCoopEntityViewmode::Activate - uservar1 is set but empty!\n" );
		return;
	}

	int iValid = 0;
	int iItem;
	for ( iItem = 2; iItem <= 10; iItem++ ) {

		Entity *ent = NULL;
		ScriptVariable *entityVar = entityVars.GetVariable( va( "uservar%d" , iItem ) );

		if ( !entityVar )
			continue;

		str sTargetname = entityVar->stringValue();
		if ( !sTargetname.length() )
		{
			gi.Printf( "TriggerCoopEntityViewmode::Activate - uservar%d set but empty!\n" , iItem );
			continue;
		}

		ent = coop_returnEntity( sTargetname.c_str() );

		if ( !ent )
		{
			gi.Printf( "TriggerCoopEntityViewmode::Activate - Entity ($%s) in uservar%d was not found!\n" , sTargetname.c_str() , iItem );
			continue;
		}

		if ( spawnflags & REMOVE )
		{
			ent->removeAffectingViewModes( gi.GetViewModeMask( sViewmode.c_str() ) );
		}
		else
		{
			ent->addAffectingViewModes( gi.GetViewModeMask( sViewmode.c_str() ) );
		}

		iValid++;
	}

	//handle target
	if ( iValid == 0 )
	{
		Entity *ent = NULL;
		ent = coop_returnEntity( target.c_str() );

		if ( !ent )
		{
			gi.Printf( "TriggerCoopEntityViewmode::Activate - Target Entity ($%s) was not found!\n" , target.c_str() );
			return;
		}
		if ( spawnflags & REMOVE )
		{
			ent->removeAffectingViewModes( gi.GetViewModeMask( sViewmode.c_str() ) );
		}
		else
		{
			ent->addAffectingViewModes( gi.GetViewModeMask( sViewmode.c_str() ) );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ENTITY RELATIVEMOVE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_relativemove (1 1 0) (-8 -8 -8) (8 8 8) ALL_PLAYERS TO_ACTIVATOR ACTIVATOR_ANKER
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Moves multiple entities with relative distances in tact.

[ if ] ALL_PLAYERS is set, this will move all players to the new location
[ if ] TO_ACTIVATOR	is set the entities will be moved relative to activator
this needs uservar4 and uservar5 to be set!
[ if ] uservar1 is set, it will be counted up, starting from 1 until the
first missing entity or 20, as follows:
$targetname1, $targetname2, ..., $targetname20
[ if ] uservar3 is set the entity specified by the given number will be used as a anker.
It will be moved directly to the spot 

"uservar1" sets the targetname of the enties to move (without ending number)
"uservar2" sets the coordinates the entities should be moved to in relative
"uservar3" sets number of anker entity, will move this one exactly to the coordinates, all others relative to it
"uservar4" sets the min distance the objects should have
"uservar5" sets the max distance the objects should have
*/
CLASS_DECLARATION( Trigger , TriggerCoopEntityRelativemove , "trigger_coop_entity_relativemove" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityRelativemove::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityRelativemove::TriggerCoopEntityRelativemove()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityRelativemove::Activate( Event *ev )
{
#define ALL_PLAYERS (1<<0)
#define TO_ACTIVATOR (1<<1)
#define ACTIVATOR_ANKER (1<<2)

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	int iIndex = 0;
	str sTargetname = "";
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	if ( !entityVar1 )
	{
		gi.Printf( "TriggerCoopEntityRelativemove::Activate - uservar1 is not set, should contain base-targetname!\n" );
		return;
	}

	if ( !entityVar2 )
	{
		gi.Printf( "TriggerCoopEntityRelativemove::Activate - uservar2 is not set, should contain new location vector!\n" );
		return;
	}
	
	sTargetname = entityVar1->stringValue();
	if ( sTargetname.length() )
	{
		gi.Printf( "TriggerCoopEntityRelativemove::Activate - uservar1 is not set, should contain base-targetname!\n" );
		return;
	}

	//count how many there are
	Entity *ent = NULL;
	while ( iIndex < 20 )
	{
		iIndex++;
		ent = coop_returnEntity( va( "%s%d" , sTargetname.c_str() , iIndex ) );
		if ( !ent )
		{
			break;
		}
	}

	if ( iIndex < 1 )
	{
		gi.Printf( "TriggerCoopEntityRelativemove::Activate - No entity with targetname (%s1) found!\n", sTargetname.c_str() );
		return;
	}

	Vector vOffset = Vector( 0 , 0 , 0 );
	Vector vNewLoc = Vector( 0 , 0 , 0 );
	Vector vCenterOrigin = Vector( 0 , 0 , 0 );

	/*

	//get anker pos/ent
	if ( spawnflags & ACTIVATOR_ANKER && activator && activator != world )
	{
		vCenterOrigin = activator->origin;
	}
	else
	{
		if ( entityVar3 )
		{
			iAnkerNumber = entityVar3->intValue;
		}

		//if no valid input, select the middle one
		if ( iAnkerNumber <= 0 )
		{
			iAnkerNumber = ceil( iIndex / 2 );
		}

		ent = coop_returnEntity( va( "%s%d" , sTargetname.c_str() , iAnkerNumber ) );

		//if it can't be found
		if ( ent ) {
			vCenterOrigin = ent->origin;
		}
		else
		{
			gi.Printf( "TriggerCoopEntityRelativemove::Activate - Could not find (%s%d)!\n" , sTargetname.c_str() , iAnkerNumber );
			return;
		}
	}

	int iCurrent;
	for ( iCurrent = iIndex; iCurrent > 0; iCurrent--) {
		ent = coop_returnEntity( va( "%s%d" , sTargetname.c_str() , iCurrent ) );
		if ( ent )
		{
			Vector vCalc1 = Vector( 0 , 0 , 0 );
			Vector vCalc2 = Vector( 0 , 0 , 0 );
			Vector vLength = Vector( 0 , 0 , 0 );

			vCalc1.x = ent->origin.x;
			vCalc2.x = vCenterOrigin.x;

			if ( vCenterOrigin.x < ent->origin.x )
			{
				vLength = ( vCalc1 )

			}

				 = ( newOrigin )+ ( + vCalc1.length() );

			if ( ent->origin == vCenterOrigin )
			{
			
			}
		}
		else
		{
			gi.Printf( "TriggerCoopEntityRelativemove::Activate - Could not find entity (%s%d)!\n" , sTargetname.c_str() , iCurrent );
		}
	}
	*/

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ENTITY WARP ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_warp(1 1 0) (-8 -8 -8) (8 8 8) ANTI_STUCK NOT_ACTIVATOR FORCE_ANGLE NO_VELOCITY ALL_PLAYERS
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Warps a entity to the new location

[ if ] ANTI_STUCK is set the entity will be warped and made notsolid untill it has been cleared of all players and actors
[ ! ] ANTI_STUCK will make entity solid
[ if ] NOT_ACTIVATOR is set the activating entity will not be moved
[ if ] FORCE_ANGLE is set the angle of the targer or set in uservar2 will be enforced
[ if ] NO_VELOCITY is set the velocity of the entity will be set to 0 before moving it
[ if ] ALL_PLAYERS is set it will only move the players

"target" sets the targeted entity its position as destination (do not set uservar1)
"uservar1" sets the new coordinates	(if set it will overwrite target)
"uservar2" sets the new angles (facing direction overwrites target angle)
"uservar3" sets a offset to the given location (X Y Z)
"uservar4-10" sets targetname of entity to be moved
*/
CLASS_DECLARATION( Trigger , TriggerCoopEntityWarp , "trigger_coop_entity_warp" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityWarp::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityWarp::TriggerCoopEntityWarp()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityWarp::Activate( Event *ev )
{
#define ANTI_STUCK (1<<0)
#define NOT_ACTIVATOR (1<<1)
#define	FORCE_ANGLE (1<<2)
#define NO_VELOCITY (1<<3)
#define ALL_PLAYERS (1<<4)
#define USERVARS_START 4
#define USERVARS_END 10

	Vector vNew;
	Vector vOffset = Vector(0,0,0);
	float fAngle = -666;
	str sTargetname="";

	ScriptVariable *entityVarOffset = entityVars.GetVariable( "uservar3" );
	if ( entityVarOffset )
	{
		str sValue = entityVarOffset->stringValue();
		vOffset = coop_returnVectorFromString(sValue);
	}

	ScriptVariable *entityVarAngle = entityVars.GetVariable( "uservar2" );
	if ( entityVarAngle )
	{
		fAngle = entityVarAngle->floatValue();
	}


	ScriptVariable *entityVar = entityVars.GetVariable( "uservar1" );
	if ( !entityVar )
	{
		Entity *ent = coop_returnEntity( target );

		if ( !ent )
		{
			if ( target.length() )
			{
				gi.Printf( "TriggerCoopEntityWarp::Activate - Target (%s) not found!\n" , target.c_str() );
			}
			else
			{
				gi.Printf( "TriggerCoopEntityWarp::Activate - uservar1 is empty, should contain vector!\n" );
			}
			return;
		}

		vNew = ent->origin;
		if ( fAngle == -666 )
		{
			fAngle = ent->angles.y;
		}
		//set offset
		vNew.x += vOffset.x;
		vNew.y += vOffset.y;
		vNew.z += vOffset.z;
	}
	else
	{
		str sValue = entityVar->stringValue();
		vNew = coop_returnVectorFromString( sValue );
		//set offset
		vNew.x += vOffset.x;
		vNew.y += vOffset.y;
		vNew.z += vOffset.z;
	}

	if ( spawnflags & ALL_PLAYERS )
	{
		int iPlayer;
		Player* currentPlayer;

		for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
		{
			currentPlayer = multiplayerManager.getPlayer( iPlayer );

			if ( !currentPlayer || multiplayerManager.isPlayerSpectator( currentPlayer ) || currentPlayer->health <= 0 )
				continue;

			Event *even = new Event( EV_Warp );
			even->AddVector( vNew );
			currentPlayer->ProcessEvent( even );

			//set angle
			if ( spawnflags & FORCE_ANGLE && fAngle != -666 )
			{
				currentPlayer->SetViewAngles( Vector( 0 , fAngle , 0 ) );
			}

			if ( spawnflags & NO_VELOCITY )
			{
				currentPlayer->velocity = Vector( 0 , 0 , 0 );
			}

			//prevent actors and players getting stuck
			if ( spawnflags & ANTI_STUCK )
			{
				Event *even2 = new Event( EV_BecomeNonSolid );
				currentPlayer->ProcessEvent( even2 );

				Event *even3 = new Event( EV_BecomeSolid );
				currentPlayer->PostEvent( even3 , 0.25f );
			}
		}
		return;
	}

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	//handle activator
	if ( spawnflags & NOT_ACTIVATOR )
	{
	}
	else if ( activator != world && !activator->isSubclassOf( Trigger ) )
	{
		if ( activator->isSubclassOf( Player ) || activator->isSubclassOf( Actor ) )
		{
			Event *even = new Event( EV_Warp );
			even->AddVector( vNew );
			activator->ProcessEvent( even );
		}
		else
		{
			activator->setOrigin( vNew );
		}

		if ( spawnflags & NO_VELOCITY )
		{
			activator->velocity = Vector( 0 , 0 , 0 );
		}

		//set angle
		if ( spawnflags & FORCE_ANGLE && fAngle != -666 )
		{
			if ( activator->isSubclassOf( Player ) )
			{
				Player *player = ( Player* )( Entity* )activator;
				player->SetViewAngles( Vector( 0 , fAngle , 0 ) );
			}
			else
			{
				Event *even1 = new Event( EV_SetAngle );
				activator->ProcessEvent( even1 );
			}
		}

		//prevent actors and players getting stuck
		if ( spawnflags & ANTI_STUCK )
		{
			Event *even2 = new Event( EV_BecomeNonSolid );
			activator->ProcessEvent( even2 );

			Event *even3 = new Event( EV_BecomeSolid );
			activator->PostEvent( even3 , 0.25f );
		}
	}

	//handle referenced
	Entity *eWarp;
	int short validNum=0;
	int short itemNum;
	for ( itemNum = USERVARS_START; itemNum <= USERVARS_END; itemNum++ )
	{
		entityVar = entityVars.GetVariable( va( "uservar%d" , itemNum ) );
		if ( entityVar )
		{
			sTargetname = entityVar->stringValue();
			if ( sTargetname.length() )
			{
				validNum++;
				eWarp = coop_returnEntity( sTargetname );
				if ( !eWarp )
				{
					gi.Printf( "TriggerCoopEntityWarp::Activate - Entity ($%s) not found!\n", sTargetname.c_str() );
					continue;
				}

				if ( eWarp == world )
				{
					continue;
				}

				if ( eWarp->isSubclassOf( Player ) )
					if ( multiplayerManager.isPlayerSpectator( ( Player* )eWarp ) || eWarp->health <= 0 )
						continue;

				if ( eWarp->isSubclassOf( Player ) || eWarp->isSubclassOf( Actor ) )
				{
					Event *even = new Event( EV_Warp );
					even->AddVector( vNew );
					eWarp->ProcessEvent( even );
				}
				else
				{
					eWarp->setOrigin( vNew );
				}

				if ( spawnflags & NO_VELOCITY )
				{
					eWarp->velocity = Vector( 0 , 0 , 0 );
				}

				//set angle
				if ( spawnflags & FORCE_ANGLE && fAngle != -666 )
				{
					if ( eWarp->isSubclassOf( Player ) )
					{
						Player *player = ( Player* )( Entity* )eWarp;
						player->SetViewAngles( Vector( 0 , fAngle , 0 ) );
					}
					else
					{
						Event *even1 = new Event( EV_SetAngle );
						even1->AddFloat( fAngle );
						eWarp->ProcessEvent( even1 );
					}
				}

				//prevent actors and players getting stuck
				if ( spawnflags & ANTI_STUCK )
				{
					Event *even2 = new Event( EV_BecomeNonSolid );
					eWarp->ProcessEvent( even2 );

					Event *even3 = new Event( EV_BecomeSolid );
					eWarp->PostEvent( even3 , 0.25f );
				}
			}
		}
	}
	if ( validNum == 0 )
	{
		if ( spawnflags & NOT_ACTIVATOR || activator == world )
		{
			gi.Printf( "TriggerCoopEntityWarp::Activate - No entitiy set in uservar%d-%d!\n" , USERVARS_START , USERVARS_END , sTargetname.c_str() );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ENTITY TRANSPORT ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_transport(1 1 0) (-8 -8 -8) (8 8 8) ANTI_STUCK NOT_ACTIVATOR FORCE_ANGLE ALL_PLAYERS ROMULAN IDRYLL BORG
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Transports a entity to the new location

[ if ] ANTI_STUCK is set the entity will be warped and made notsolid untill it has been cleared of all players and actors
[ ! ] ANTI_STUCK will make entity solid
[ if ] NOT_ACTIVATOR is set the activating entity will not be handled
[ if ] FORCE_ANGLE is set the angle of the targer or set in uservar2 will be enforced
[ if ] ALL_PLAYERS is set it will only move the players
[ if ] ROMULAN is set the beam animation will be romulan
[ if ] IDRYLL is set the beam animation will be idryll
[ if ] BORG is set the beam animation will be borg

"target" sets the targeted entity its position as destination (do not set uservar1)
"uservar1" sets the new coordinates	(if set it will overwrite target)
"uservar2" sets the new angles (facing direction overwrites target angle)
"uservar3" sets a offset to the given location (X Y Z)
"uservar4-10" sets targetname of entity to be moved
*/
CLASS_DECLARATION( Trigger , TriggerCoopEntityTransport , "trigger_coop_entity_transport" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityTransport::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityTransport::TriggerCoopEntityTransport()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityTransport::Activate( Event *ev )
{
#define ANTI_STUCK (1<<0)
#define NOT_ACTIVATOR (1<<1)
#define	FORCE_ANGLE (1<<2)
#define ALL_PLAYERS (1<<3)
#define	ROMULAN (1<<4)
#define IDRYLL (1<<5)
#define BORG (1<<6)
#define USERVARS_START 4
#define USERVARS_END 10

	bool bUseOnlyOffset = false;

	str sBeamAnimation;
	float fBeamTime = 0.75f;

	if ( spawnflags & ROMULAN ){
		sBeamAnimation = "Romulan";
	}
	else if ( spawnflags & IDRYLL ) {
		sBeamAnimation = "Idryll";
	}
	else if ( spawnflags & BORG ) {
		sBeamAnimation = "Borg";
	}
	else {
		sBeamAnimation = "Multiplayer";
		fBeamTime = 1.25;
	}

	Vector vNew;
	Vector vOffset = Vector( 0 , 0 , 0 );
	float fAngle = -666;
	str sTargetname = "";

	ScriptVariable *entityVarOffset = entityVars.GetVariable( "uservar3" );
	if ( entityVarOffset )
	{
		str sValue = entityVarOffset->stringValue();
		vOffset = coop_returnVectorFromString( sValue );
	}

	ScriptVariable *entityVarAngle = entityVars.GetVariable( "uservar2" );
	if ( entityVarAngle )
	{
		fAngle = entityVarAngle->floatValue();
	}

	ScriptVariable *entityVar = entityVars.GetVariable( "uservar1" );
	if ( !entityVar )
	{
		Entity *ent = coop_returnEntity( target );

		if ( !ent )
		{
			if ( target.length() )
			{
				gi.Printf( "TriggerCoopEntityTransport::Activate - Target (%s) not found!\n" , target.c_str() );
				return;
			}
			else if ( entityVarOffset )
			{
				bUseOnlyOffset = true;
			}
			else //if no offset set, print this warning
			{
				gi.Printf( "TriggerCoopEntityTransport::Activate - uservar1 is empty, should contain vector!\n" );
				return;
			}
		}

		if ( !bUseOnlyOffset )
		{
			vNew = ent->origin;
			if ( fAngle == -666 )
			{
				fAngle = ent->angles.y;
			}
			//set offset
			vNew.x += vOffset.x;
			vNew.y += vOffset.y;
			vNew.z += vOffset.z;
		}
	}
	else
	{
		str sValue = entityVar->stringValue();
		vNew = coop_returnVectorFromString( sValue );
		//set offset
		vNew.x += vOffset.x;
		vNew.y += vOffset.y;
		vNew.z += vOffset.z;
	}

	if ( spawnflags & ALL_PLAYERS )
	{
		int iPlayer;
		Player* currentPlayer;

		for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
		{
			currentPlayer = multiplayerManager.getPlayer( iPlayer );

			if ( !currentPlayer || multiplayerManager.isPlayerSpectator( currentPlayer ) || currentPlayer->health <= 0 )
				continue;

			if ( bUseOnlyOffset )
			{
				vNew = currentPlayer->origin;
				vNew.x += vOffset.x;
				vNew.y += vOffset.y;
				vNew.z += vOffset.z;
			}

			Event *noDamage = new Event( EV_NoDamage );
			currentPlayer->PostEvent( noDamage , 0.0f );

			Event *takeDamage = new Event( EV_TakeDamage );
			currentPlayer->PostEvent( takeDamage , ( fBeamTime * 2 ) );

			Event *newEvent1 = new Event( EV_DisplayEffect );
			newEvent1->AddString( "TransportOut" );
			newEvent1->AddString( sBeamAnimation.c_str() );
			currentPlayer->PostEvent( newEvent1 , 0.0f );

			Event *newEvent2 = new Event( EV_DisplayEffect );
			newEvent2->AddString( "TransportIn" );
			newEvent2->AddString( sBeamAnimation.c_str() );
			currentPlayer->PostEvent( newEvent2 , fBeamTime + 0.1f );

			Event *newEvent3 = new Event( EV_Hide );
			currentPlayer->PostEvent( newEvent3 , fBeamTime - 0.1f );

			Event *newEvent4 = new Event( EV_Show );
			currentPlayer->PostEvent( newEvent4 , fBeamTime + 0.1f );

			Event *even = new Event( EV_Warp );
			even->AddVector( vNew );
			currentPlayer->PostEvent( even , fBeamTime );

			Event *newEventReset1 = new Event( EV_Player_ResetState );
			currentPlayer->PostEvent( newEventReset1 , fBeamTime + 0.2f );

			Event *newEventReset2 = new Event( EV_Player_ResetState );
			currentPlayer->PostEvent( newEventReset2 , fBeamTime + fBeamTime + 0.3 );

			currentPlayer->velocity = Vector( 0 , 0 , 0 );

			//set angle
			if ( spawnflags & FORCE_ANGLE && fAngle != -666 )
			{
				Event *evenView = new Event( EV_Player_SetViewAngles );
				evenView->AddVector( Vector( 0 , fAngle , 0 ) );
				currentPlayer->PostEvent( evenView , fBeamTime );
			}

			//prevent actors and players getting stuck
			Event *even2 = new Event( EV_BecomeNonSolid );
			currentPlayer->PostEvent( even2 , fBeamTime );

			Event *even3 = new Event( EV_BecomeSolid );
			currentPlayer->PostEvent( even3 , fBeamTime + 0.25f );
		}
		return;
	}

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	//handle activator
	if ( spawnflags & NOT_ACTIVATOR )
	{
	}
	else if ( activator != world && !activator->isSubclassOf( Trigger ) )
	{
		if ( spawnflags & NO_VELOCITY )
		{
			activator->velocity = Vector( 0 , 0 , 0 );
		}

		if ( bUseOnlyOffset )
		{
			vNew = activator->origin;
			vNew.x += vOffset.x;
			vNew.y += vOffset.y;
			vNew.z += vOffset.z;
		}

		Event *noDamage = new Event( EV_NoDamage );
		activator->PostEvent( noDamage , 0.0f );

		Event *takeDamage = new Event( EV_TakeDamage );
		activator->PostEvent( takeDamage , (fBeamTime * 2) );

		Event *newEvent1 = new Event( EV_DisplayEffect );
		newEvent1->AddString( "TransportOut" );
		newEvent1->AddString( sBeamAnimation.c_str() );
		activator->PostEvent( newEvent1 , 0.0f );

		Event *newEvent2 = new Event( EV_DisplayEffect );
		newEvent2->AddString( "TransportIn" );
		newEvent2->AddString( sBeamAnimation.c_str() );
		activator->PostEvent( newEvent2 , fBeamTime + 0.1f );

		Event *newEvent3 = new Event( EV_Hide );
		activator->PostEvent( newEvent3 , fBeamTime - 0.1f );

		Event *newEvent4 = new Event( EV_Show );
		activator->PostEvent( newEvent4 , fBeamTime + 0.1f );

		Event * eventWarp = new Event( EV_Warp );
		eventWarp->AddVector( vNew );
		activator->PostEvent(eventWarp, fBeamTime );

		if ( activator->isSubclassOf( Player ) || activator->isSubclassOf( Actor ) )
		{
			if ( activator->isSubclassOf( Player ) )
			{
				Event *newEventReset1 = new Event( EV_Player_ResetState );
				activator->PostEvent( newEventReset1 , fBeamTime + 0.2f );

				Event *newEventReset2 = new Event( EV_Player_ResetState );
				activator->PostEvent( newEventReset2 , fBeamTime + fBeamTime + 0.9f );
			}

			Event *eventWarp2 = new Event( EV_Warp );
			eventWarp2->AddVector( vNew );
			activator->PostEvent(eventWarp2, fBeamTime );
		}
		else
		{
			Event *eventSetOrigin = new Event( EV_SetOrigin );
			eventSetOrigin->AddVector( vNew );
			activator->PostEvent(eventSetOrigin, fBeamTime );
		}

		//set angle
		if ( spawnflags & FORCE_ANGLE && fAngle != -666 )
		{
			if ( activator->isSubclassOf( Player ) )
			{
				Player *player = ( Player* )( Entity* )activator;
				Event *evenView = new Event( EV_Player_SetViewAngles );
				evenView->AddVector( Vector( 0 , fAngle , 0 ) );
				player->PostEvent( evenView , fBeamTime );
			}
			else
			{
				Event *even1 = new Event( EV_SetAngle );
				even1->AddFloat( fAngle );
				activator->ProcessEvent( even1 );
			}
		}

		//prevent actors and players getting stuck
		if ( spawnflags & ANTI_STUCK )
		{
			Event *even2 = new Event( EV_BecomeNonSolid );
			activator->PostEvent( even2 , fBeamTime );

			Event *even3 = new Event( EV_BecomeSolid );
			activator->PostEvent( even3 , fBeamTime + 0.25f );
		}
	}

	//handle referenced
	Entity *eWarp;
	int short validNum = 0;
	int short itemNum;
	for ( itemNum = USERVARS_START; itemNum <= USERVARS_END; itemNum++ )
	{
		entityVar = entityVars.GetVariable( va( "uservar%d" , itemNum ) );
		if ( entityVar )
		{
			sTargetname = entityVar->stringValue();
			if ( sTargetname.length() )
			{
				validNum++;
				eWarp = coop_returnEntity( sTargetname );
				if ( !eWarp )
				{
					gi.Printf( "TriggerCoopEntityTransport::Activate - Entity ($%s) not found!\n" , sTargetname.c_str() );
					continue;
				}

				if ( eWarp == world )
				{
					continue;
				}

				if ( eWarp->isSubclassOf( Player ) )
					if ( multiplayerManager.isPlayerSpectator( ( Player* )eWarp ) || eWarp->health <= 0 )
						continue;

				if ( bUseOnlyOffset )
				{
					vNew = eWarp->origin;
					vNew.x += vOffset.x;
					vNew.y += vOffset.y;
					vNew.z += vOffset.z;
				}

				Event *noDamage = new Event( EV_NoDamage );
				eWarp->PostEvent( noDamage , 0.0f );

				Event *takeDamage = new Event( EV_TakeDamage );
				eWarp->PostEvent( takeDamage , ( fBeamTime * 2 ) );

				Event *newEvent1 = new Event( EV_DisplayEffect );
				newEvent1->AddString( "TransportOut" );
				newEvent1->AddString( sBeamAnimation.c_str() );
				eWarp->PostEvent( newEvent1 , 0.0f );

				Event *newEvent2 = new Event( EV_DisplayEffect );
				newEvent2->AddString( "TransportIn" );
				newEvent2->AddString( sBeamAnimation.c_str() );
				eWarp->PostEvent( newEvent2 , fBeamTime + 0.1f );

				Event *newEvent3 = new Event( EV_Hide );
				eWarp->PostEvent( newEvent3 , fBeamTime - 0.1f );

				Event *newEvent4 = new Event( EV_Show );
				eWarp->PostEvent( newEvent4 , fBeamTime + 0.1f );

				if ( eWarp->isSubclassOf( Player ) || eWarp->isSubclassOf( Actor ) )
				{
					if ( eWarp->isSubclassOf( Player ) )
					{
						Event *newEventReset1 = new Event( EV_Player_ResetState );
						eWarp->PostEvent( newEventReset1 , fBeamTime + 0.2f );

						Event *newEventReset2 = new Event( EV_Player_ResetState );
						eWarp->PostEvent( newEventReset2 , fBeamTime + fBeamTime + 0.9f );
					}

					Event *even = new Event( EV_Warp );
					even->AddVector( vNew );
					eWarp->PostEvent( even , fBeamTime );
				}
				else
				{
					Event *even = new Event( EV_SetOrigin );
					even->AddVector( vNew );
					eWarp->PostEvent( even , fBeamTime );
				}

				//set angle
				if ( spawnflags & FORCE_ANGLE && fAngle != -666 )
				{
					if ( eWarp->isSubclassOf( Player ) )
					{
						Player *player = ( Player* )( Entity* )eWarp;
						Event *evenView = new Event( EV_Player_SetViewAngles );
						evenView->AddVector( Vector( 0 , fAngle , 0 ) );
						player->PostEvent( evenView , fBeamTime );
					}
					else
					{
						Event *even1 = new Event( EV_SetAngle );
						even1->AddFloat( fAngle );
						eWarp->ProcessEvent( even1 );
					}
				}

				//prevent actors and players getting stuck
				if ( spawnflags & ANTI_STUCK )
				{
					Event *even2 = new Event( EV_BecomeNonSolid );
					eWarp->PostEvent( even2 , fBeamTime );

					Event *even3 = new Event( EV_BecomeSolid );
					eWarp->PostEvent( even3 , fBeamTime + 0.25f );
				}
			}
		}
	}
	if ( validNum == 0 )
	{
		if ( spawnflags & NOT_ACTIVATOR || activator == world )
		{
			gi.Printf( "TriggerCoopEntityTransport::Activate - No entitiy set in uservar%d-%d!\n" , USERVARS_START , USERVARS_END , sTargetname.c_str() );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ENTITY SCALE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_scale(1 1 0) (-8 -8 -8) (8 8 8)

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Scales the attached Model of a entity, but not the entity it self.

"uservar1" sets scale to set
*/
CLASS_DECLARATION( Trigger , TriggerCoopEntityScale , "trigger_coop_entity_scale" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityScale::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityScale::TriggerCoopEntityScale()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityScale::Activate( Event *ev )
{
	if ( !target.length() )
	{
		gi.Printf( "TriggerCoopEntityScale::Activate - No Target Entity set!\n" );
		return;
	}
	Entity *ent = coop_returnEntity( target );
	if ( !ent )
	{
		gi.Printf( "TriggerCoopEntityScale::Activate - Target Entity ($%s) was not found!\n" , target.c_str() );
		return;
	}

	float uservar1 = 0.01f;
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	if ( entityVar1 )
		uservar1 = entityVar1->floatValue();
	
	ent->setScale( uservar1 );

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}


/* ############################## ENTITY HEALTH ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_health(1 1 0) (-8 -8 -8) (8 8 8) ADD TAKE IMMORTAL NOT_IMMORTAL ALL_PLAYERS

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sets health of a entity to value of uservar1, (including Actors and triggers)
[ info ] if no target set, this will apply to the activator

[ if ] ADD is set the health ammount in uservar1 will be added
[ if ] TAKE is set the health ammount in uservar1 will be taken
=== WORKS ONLY ON ACTORS ===
[ if ] IMMORTAL is set the entity will go down to 1 health but not below and it will not die
[ if ] NOT_IMMORTAL is set the effect of IMMOTRTAL is canceled (some Actors start as immortal on default)
===
[ if ] ALL_PLAYERS is set the this will affect all players

"uservar1" sets the ammount of health to set, add or take
"uservar2" sets the maximum/minimum health to set when ADD/TAKE is set
"target" sets the entity that is meant to be handled
*/
CLASS_DECLARATION( Trigger , TriggerCoopEntityHealth , "trigger_coop_entity_health" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityHealth::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityHealth::TriggerCoopEntityHealth()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityHealth::Activate( Event *ev )
{
#define ADD (1<<0)
#define TAKE (1<<1)
#define IMMORTAL (1<<2)
#define NOT_IMMORTAL (1<<3)
#define ALL_PLAYERS (1<<4)

	Entity *ent = NULL;
	Actor *actor = NULL;
	if ( target.length() > 0 )
	{
		ent = coop_returnEntity( target );
		if ( !ent )
		{
			gi.Printf( "TriggerCoopEntityHealth::Activate - Target Entity ($%s) was not found!\n" , target.c_str() );
			return;
		}
	}
	else if( spawnflags & ~ALL_PLAYERS )
	{
		//set activator
		Entity *other;
		other = ev->GetEntity( 1 );

		if ( !other )
			activator = world;
		else
			activator = other;


		if ( activator )
		{
			if ( activator == world )
			{
				gi.Printf( "TriggerCoopEntityHealth::Activate - Activator is WORLD, trigger might have been called from script or console!\n" );
				return;
			}
			else
			{
				ent = activator;
			}
		}
		else
		{
			gi.Printf( "TriggerCoopEntityHealth::Activate - No Activator found!\n" );
			return;
		}
	}

	float uservar1 = 0.0f;
	float uservar2 = 1.0f;
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	if ( entityVar1 )
		uservar1 = entityVar1->floatValue();
	if ( entityVar2 )
		uservar2 = entityVar2->floatValue();

	//eigther take or add
	if ( spawnflags & ADD && spawnflags & TAKE )
	{
		gi.Printf( "TriggerCoopEntityHealth::Activate - ERROR you can either ADD or TAKE health, aborted!\n" );
		return;
	}

	//check minimum value
	if ( uservar2 < 1.0f )
	{
		gi.Printf( "TriggerCoopEntityHealth::Activate - uservar2 was %d, set to minimum ( 1 ) !\n" , uservar2 );
		uservar2 = 1.0f;
	}

	//check set value
	if ( uservar1 < 1.0f )
	{
		gi.Printf( "TriggerCoopEntityHealth::Activate - Health to SET was %d, set to minimum ( 1 ) !\n" , uservar1 );
		uservar1 = 1.0f;
	}

	//immortal, mortal
	if ( spawnflags & IMMORTAL || spawnflags & NOT_IMMORTAL )
	{
		if ( ent && ent->isSubclassOf(Actor) )
		{
			qboolean immortal = true;
			if ( spawnflags & NOT_IMMORTAL )
			{
				immortal = false;
			}
			actor = ( Actor* )ent;
			actor->SetActorFlag( ACTOR_FLAG_IMMORTAL , immortal );
		}
		else
		{
			gi.Printf( "TriggerCoopEntityHealth::Activate - IMMORTAL works only on actors!\n" );
		}
	}

	//handle all players
	if ( spawnflags & ALL_PLAYERS )
	{
		int iPlayer;
		Player* currentPlayer;

		for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
		{
			currentPlayer = multiplayerManager.getPlayer( iPlayer );

			if ( !currentPlayer || multiplayerManager.isPlayerSpectator( currentPlayer ) || currentPlayer->health <= 0 )
				continue;

			//set/add/take health
			if ( spawnflags & ADD )
			{
				if ( currentPlayer->health < uservar2 )
				{
					if ( ( currentPlayer->health + uservar1 ) > uservar2 )
					{
						currentPlayer->setHealth( uservar2 );
					}
					else
					{
						currentPlayer->setHealth( ( currentPlayer->health + uservar1 ) );
					}
				}
			}
			//take health
			else if ( spawnflags & TAKE )
			{
				if ( ( currentPlayer->health - uservar1) < uservar2 )
				{
					currentPlayer->setHealth( uservar2 );
				}
				else
				{
					currentPlayer->setHealth( ( currentPlayer->health - uservar1 ) );
				}
			}
			//set health
			else
			{
				currentPlayer->setHealth( uservar1 );
			}
		}
	}
	else
	{
		//set/add/take health
		if ( spawnflags & ADD )
		{
			if ( ent->health < uservar2 )
			{
				if ( ( ent->health + uservar1 ) > uservar2 )
				{
					ent->setHealth( uservar2 );
				}
				else
				{
					ent->setHealth( ( ent->health + uservar1 ) );
				}				
			}
		}
		//take health
		else if ( spawnflags & TAKE )
		{
			if ( ( ent->health - uservar1 ) < uservar2 )
			{
				ent->setHealth( uservar2 );
			}
			else
			{
				ent->setHealth( ( ent->health - uservar1 ) );
			}
		}
		//set health
		else
		{
			ent->setHealth( uservar1 );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ENTITY CONTENTS ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_contents(1 1 0) (-8 -8 -8) (8 8 8) NOT_SOLID
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sets the contents of a entity to solid.

[ if ] NOT_SOLID is set the entity will become notsolid

"target" sets target entity
*/
CLASS_DECLARATION( Trigger , TriggerCoopEntityContents , "trigger_coop_entity_contents" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityContents::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityContents::TriggerCoopEntityContents()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityContents::Activate( Event *ev )
{
#define  NOT_SOLID (1<<0)
#define  WATER (1<<1)

	if ( !target.length() )
	{
		gi.Printf( "TriggerCoopEntityContents::Activate - (%s) No target entity set!\n" , target.c_str() );
		return;
	}

	Entity *ent = coop_returnEntity( target );


	if ( !ent )
	{
		gi.Printf( "TriggerCoopEntityContents::Activate - Entity ($%s) was not found!\n" , target.c_str() );
		return;
	}

	if ( spawnflags & NOT_SOLID )
	{
		//prevent actors and players getting stuck
		Event *even1 = new Event( EV_BecomeNonSolid );
		ent->ProcessEvent( even1 );
	}
	else
	{
		//prevent actors and players getting stuck
		Event *even1 = new Event( EV_BecomeSolid );
		ent->ProcessEvent( even1 );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}
/* ############################## ENTITY ANIMATE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_animate(1 1 0) (-8 -8 -8) (8 8 8) HOLD IDLE AI_ON DIE

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Animates any Entity.
[ info ] To work on a actor, AI_OFF must be set on the actor
[ info ] Does not work on MAKE_STATIC Models
[ info ] Some Animations might not work on all Models

== Works only with Actors ==
[ if ] HOLD is set the animation will be played once and held at the last frame
[ if ] IDLE is set the animation will be played once, followed by the idle animation
[ if ] AI_ON is set the ai will activate after the animation (needs ONCE enabled)
[ if ] DIE is set the ai will die after the animation (needs ONCE enabled)
===

"uservar1" sets the animation to play
"target" sets the entity that is meant to play the animation
*/

CLASS_DECLARATION( Trigger , TriggerCoopEntityAnimate , "trigger_coop_entity_animate" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityAnimate::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityAnimate::TriggerCoopEntityAnimate()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityAnimate::Activate( Event *ev )
{
#define HOLD (1<<0)
#define IDLE (1<<1)
#define AI_ON (1<<2)
#define DIE (1<<3)

	if ( !target.length() )
	{
		gi.Printf( "TriggerCoopEntityAnimate::Activate - (%s) No target entity set!\n", target.c_str() );
		return;
	}

	Entity *ent = coop_returnEntity( target );


	if ( !ent )
	{
		gi.Printf( "TriggerCoopEntityAnimate::Activate - Entity ($%s) was not found!\n" , target.c_str() );
		return;
	}

	str sAnimationName = "";
	ScriptVariable *entityVar = entityVars.GetVariable( "uservar1" );
	if( entityVar )
		sAnimationName = entityVar->stringValue();

	if ( sAnimationName.length() < 1)
	{
		gi.Printf( "TriggerCoopEntityAnimate::Activate - ($%s) uservar1 was empty, needs to be animation name\n", targetname.c_str() );
		return;
	}

	if ( ent->isSubclassOf(Actor) ) 
	{
		//need to turn ai off for animation to work
		Actor *act = ( Actor * )ent;
		act->TurnAIOff();
	}

	if ( ent->isSubclassOf( Actor ) )
	{
		Actor *act = ( Actor * )ent;

		if ( spawnflags & DIE )
		{
			act->animate->RandomAnimate( sAnimationName.c_str() , EV_Actor_Suicide );
		}
		else if ( spawnflags & AI_ON )
		{
			act->animate->RandomAnimate( sAnimationName.c_str() , EV_Actor_AIOn );
		}
		else if ( spawnflags & IDLE )
		{
			act->animate->RandomAnimate( sAnimationName.c_str() , EV_Idle );
			if ( spawnflags & HOLD )
			{
				gi.Printf( "TriggerCoopEntityAnimate::Activate - ($%s) HOLD and IDLE are both set, favouring IDLE\n", targetname.c_str() );
			}
		}
		else if ( spawnflags & HOLD )
		{
			act->animate->RandomAnimate( sAnimationName.c_str() , EV_StopAnimating );
		}
		else
		{
			act->animate->RandomAnimate( sAnimationName.c_str() );
		}
	}
	else
	{
		Event *evAnim = new Event( EV_Anim );
		evAnim->AddString( sAnimationName.c_str() );
		ent->PostEvent( evAnim , 0.0f );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ENTITY VISIBILITY ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_entity_visibility(1 1 0) (-8 -8 -8) (8 8 8) HIDE SHOW DEPTH NO_DEPTH

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Handles the visibility of a entity
[ WARNING ] Even if hidden the entity its contents will still be solid!

[ if ] SHOW is set the entity will become visible
[ if ] HIDE is set the entity will become invisible
[ if ] DEPTH is set the entity will become visible through objects
[ if ] NO_DEPTH is set the it will cancel the effects of DEPTH

"target" sets the entity that is meant to be affected
*/

CLASS_DECLARATION( Trigger , TriggerCoopEntityVisibility , "trigger_coop_entity_visibility" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopEntityVisibility::Activate } ,
	{ NULL, NULL }
};
TriggerCoopEntityVisibility::TriggerCoopEntityVisibility()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopEntityVisibility::Activate( Event *ev )
{
#define HIDE (1<<0)
#define SHOW (1<<1)
#define DEPTH (1<<2)
#define NO_DEPTH (1<<3)

	if ( target.length() < 1 )
	{
		gi.Printf( "TriggerCoopEntityVisibility::Activate - No target entity set!\n" );
		return;
	}

	Entity *ent = coop_returnEntity( target );


	if ( !ent )
	{
		gi.Printf( "TriggerCoopEntityVisibility::Activate - Entity ($%s) was not found!\n" , target.c_str() );
		return;
	}

	if ( spawnflags & HIDE )
	{
		Event *evShow = new Event( EV_Hide );
		ent->PostEvent( evShow , 0.0f );
	}
	else if ( spawnflags & SHOW )
	{
		Event *evShow = new Event( EV_Show );
		ent->PostEvent( evShow , 0.0f );
	}

	if ( spawnflags & DEPTH )
	{
		Event *evRender = new Event( EV_EntityRenderEffects );
		evRender->AddString( "+depthhack" );
		ent->ProcessEvent( evRender );
	}
	else if ( spawnflags & NO_DEPTH )
	{
		Event *evRender = new Event( EV_EntityRenderEffects );
		evRender->AddString( "-depthhack" );
		ent->ProcessEvent( evRender );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## DOOR MANAGE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_door_manage(1 1 0) (-8 -8 -8) (8 8 8) OPEN CLOSE UNLOCK LOCK

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Manages func_door entities

[ if ] OPEN is set the door will open (if not locked)
[ if ] CLOSE is set the door will close (if not closed)
[ if ] UNLOCK is set the door will unlock
[ if ] LOCK is set the door lock

"target" sets the entity that is meant to play the animation
*/

CLASS_DECLARATION( Trigger , TriggerCoopDoorManage , "trigger_coop_door_manage" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopDoorManage::Activate } ,
	{ NULL, NULL }
};
TriggerCoopDoorManage::TriggerCoopDoorManage()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopDoorManage::Activate( Event *ev )
{
#define OPEN (1<<0)
#define CLOSE (1<<1)
#define UNLOCK (1<<2)
#define LOCK (1<<3)

	if ( target.length() < 1 )
	{
		gi.Printf( "TriggerCoopDoorManage::Activate - No target entity set!\n" );
		return;
	}

	Entity *ent = coop_returnEntity( target );


	if ( !ent )
	{
		gi.Printf( "TriggerCoopDoorManage::Activate - Entity ($%s) was not found!\n" , target.c_str() );
		return;
	}

	if ( spawnflags & OPEN )
	{
		Event *evOpen = new Event( EV_Door_Open );
		evOpen->AddEntity( activator );
		ent->PostEvent( evOpen , 0.0f );
	}
	else if ( spawnflags & CLOSE )
	{
		Event *evClose = new Event( EV_Door_Close );
		ent->PostEvent( evClose , 0.0f );
	}
	if ( spawnflags & UNLOCK )
	{
		Event *evUnlock = new Event( EV_Door_Unlock );
		ent->PostEvent( evUnlock , 0.0f );
	}
	else if ( spawnflags & LOCK )
	{
		Event *evLock = new Event( EV_Door_Lock );
		evLock->AddEntity( activator );
		ent->PostEvent( evLock , 0.0f );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ACTOR TURNTOWARDS ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_actor_turntowards(1 1 0) (-8 -8 -8) (8 8 8) NOT_ACTIVATOR CLOSEST_PLAYER TOWARDS_ACTIVATOR
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Turns Actor towards given entity

[ if ] NOT_ACTIVATOR is set, the Activating actor will not be handled
[ if ] CLOSEST_PLAYER is set, the actor will turn towards their closest player
[ if ] TOWARDS_ACTIVATOR is set, the actor will turn towards the activator

"uservar1" Entity to turn towards to
"uservar2" Extra yaw to turn
"uservar3-10" Targetname of actors to handle
"target" Sets Actor to handle (uservar3-10 overwrite)
*/
CLASS_DECLARATION( Trigger , TriggerCoopActorTurntowards , "trigger_coop_actor_turntowards" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopActorTurntowards::Activate } ,
	{ NULL, NULL }
};
TriggerCoopActorTurntowards::TriggerCoopActorTurntowards()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopActorTurntowards::Activate( Event *ev )
{
#define NOT_ACTIVATOR (1<<0)
#define CLOSEST_PLAYER (1<<1)
#define TOWARDS_ACTIVATOR (1<<2)
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	Entity *turnto = NULL;

	if ( spawnflags & CLOSEST_PLAYER || spawnflags & TOWARDS_ACTIVATOR )
	{
		 //placeholder
	}
	else
	{
		if ( !entityVar1 )
		{
			gi.Printf( "TriggerCoopActorTurntowards::Activate - uservar1 is not set, should hold targetname of entity to turnto!\n" );
			return;
		}

		str sTargetname = entityVar1->stringValue();
		if ( !sTargetname.length() )
		{
			gi.Printf( "TriggerCoopActorTurntowards::Activate - uservar1 is empty, should hold targetname of entity to turnto!\n" );
			return;
		}

		turnto = coop_returnEntity( sTargetname.c_str() );

		if ( !turnto )
		{
			gi.Printf( "TriggerCoopActorTurntowards::Activate - Entity in uservar1 could not be found!\n" );
			return;
		}
	}

	float fExtraYaw = 0;

	if ( entityVar2 )
	{
		fExtraYaw = entityVar2->floatValue();
	}

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	int iItem;
	int iValid = 0;
	ScriptVariable *entityVar;
	for ( iItem = 3; iItem <= 10; iItem++ )
	{
		entityVar = entityVars.GetVariable( va( "uservar%d" , iItem ) );

		if ( !entityVar )
			continue;

		str sTargetname = entityVar->stringValue();

		if ( !sTargetname.length() )
		{
			gi.Printf( "TriggerCoopActorTurntowards::Activate - uservar%d set but value is empty!\n" , iItem );
			continue;
		}

		Entity *ent = coop_returnEntity( sTargetname.c_str() );
		if ( !ent )
		{
			gi.Printf( "TriggerCoopActorTurntowards::Activate - Entity (%s) in uservar%d could not be found!\n" , sTargetname.c_str() , iItem );
			continue;
		}

		if ( !ent->isSubclassOf( Actor ) )
		{
			gi.Printf( "TriggerCoopActorTurntowards::Activate - Entity (%s) in uservar%d is not of type Actor!\n" , sTargetname.c_str() , iItem );
			continue;
		}

		Actor *actor = ( Actor* )ent;

		//if turn towards closest player
		if ( spawnflags & TOWARDS_ACTIVATOR )
		{
			turnto = activator;
		}
		//turn towards activator
		else if ( spawnflags & CLOSEST_PLAYER )
		{
			turnto = coop_returnPlayerClosestTo( ent );
		}

		//make sure we have any entity
		if ( !turnto )
		{
			turnto = world;
		}

		actor->turnTowardsEntity( turnto , fExtraYaw );

		iValid++;
	}

	//make sure this is only applied to a actor
	if ( activator != world && activator->isSubclassOf( Actor ) )
	{
		if ( spawnflags & NOT_ACTIVATOR )
		{
			//placeholder
		}
		else
		{
			Actor *actor = ( Actor* )( Entity* )activator;

			//if turn towards closest player
			if ( spawnflags & TOWARDS_ACTIVATOR )
			{
				turnto = activator;
			}
			//turn towards activator
			else if ( spawnflags & CLOSEST_PLAYER )
			{
				turnto = coop_returnPlayerClosestTo( activator );
			}

			//make sure we have any entity
			if ( !turnto )
			{
				turnto = world;
			}

			actor->turnTowardsEntity( turnto , fExtraYaw );
		}
	}

	//handle target
	if ( iValid == 0 && target.length() > 0 )
	{
		Entity *ent = coop_returnEntity( target.c_str() );
		if ( !ent )
		{
			gi.Printf( "TriggerCoopActorTurntowards::Activate - Actor ($%s) was not found!\n" , target.c_str() );
			return;
		}
		if ( !ent->isSubclassOf( Actor ) )
		{
			gi.Printf( "TriggerCoopActorTurntowards::Activate - Target Entity (%s) is not of type Actor!\n" , target.c_str() );
			return;
		}
		Actor *actor = ( Actor * )ent;

		//if turn towards closest player
		if ( spawnflags & TOWARDS_ACTIVATOR )
		{
			turnto = activator;
		}
		//turn towards activator
		else if ( spawnflags & CLOSEST_PLAYER )
		{
			turnto = coop_returnPlayerClosestTo( ent );
		}

		//make sure we have any entity
		if ( !turnto )
		{
			turnto = world;
		}

		actor->turnTowardsEntity( turnto , fExtraYaw );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ACTOR HEADWATCH ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_actor_headwatch(1 1 0) (-8 -8 -8) (8 8 8) NOT_ACTIVATOR CLOSEST_PLAYER WATCH_ACTIVATOR STOP
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Makes Actor watch given entity

[ if ] STOP is set, the Actor will stop watching
[ if ] NOT_ACTIVATOR is set, the Activating actor will not be handled
[ if ] CLOSEST_PLAYER is set, the actor will turn towards their closest player
[ if ] TOWARDS_ACTIVATOR is set, the actor will turn towards the activator

"uservar1" Entity to watch
"uservar2" Head turn speed
"uservar3-10" Targetname of actors to handle
"target" Sets Actor to handle (uservar3-10 overwrite)
*/
CLASS_DECLARATION( Trigger , TriggerCoopActorHeadwatch , "trigger_coop_actor_headwatch" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopActorHeadwatch::Activate } ,
	{ NULL, NULL }
};
TriggerCoopActorHeadwatch::TriggerCoopActorHeadwatch()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopActorHeadwatch::Activate( Event *ev )
{
#define NOT_ACTIVATOR (1<<0)
#define CLOSEST_PLAYER (1<<1)
#define WATCH_ACTIVATOR (1<<2)
#define STOP (1<<3)
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	Entity *headwatch = NULL;

	if ( spawnflags & CLOSEST_PLAYER || spawnflags & WATCH_ACTIVATOR || spawnflags & STOP )
	{
		//placeholder
	}
	else
	{
		if ( !entityVar1 )
		{
			gi.Printf( "TriggerCoopActorHeadwatch::Activate - uservar1 is not set, should hold targetname of entity to watch!\n" );
			return;
		}

		str sTargetname = entityVar1->stringValue();
		if ( !sTargetname.length() )
		{
			gi.Printf( "TriggerCoopActorHeadwatch::Activate - uservar1 is empty, should hold targetname of entity to watch!\n" );
			return;
		}

		headwatch = coop_returnEntity( sTargetname.c_str() );

		if ( !headwatch )
		{
			gi.Printf( "TriggerCoopActorHeadwatch::Activate - Entity in uservar1 could not be found!\n" );
			return;
		}
	}

	float fHeadSpeed = 30.0f;

	if ( entityVar2 )
	{
		fHeadSpeed = entityVar2->floatValue();
	}

	if ( fHeadSpeed < 1.0f )
	{
		fHeadSpeed = 1.0f;
	}

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	int iItem;
	int iValid = 0;
	ScriptVariable *entityVar;
	for ( iItem = 3; iItem <= 10; iItem++ )
	{
		entityVar = entityVars.GetVariable( va( "uservar%d" , iItem ) );

		if ( !entityVar )
			continue;

		str sTargetname = entityVar->stringValue();

		if ( !sTargetname.length() )
		{
			gi.Printf( "TriggerCoopActorHeadwatch::Activate - uservar%d set but value is empty!\n" , iItem );
			continue;
		}

		Entity *ent = coop_returnEntity( sTargetname.c_str() );
		if ( !ent )
		{
			gi.Printf( "TriggerCoopActorHeadwatch::Activate - Entity (%s) in uservar%d could not be found!\n" , sTargetname.c_str() , iItem );
			continue;
		}

		if ( !ent->isSubclassOf( Actor ) )
		{
			gi.Printf( "TriggerCoopActorHeadwatch::Activate - Entity (%s) in uservar%d is not of type Actor!\n" , sTargetname.c_str() , iItem );
			continue;
		}

		Actor *actor = ( Actor* )ent;

		//if turn towards closest player
		if ( spawnflags & WATCH_ACTIVATOR )
		{
			headwatch = activator;
		}
		//turn towards activator
		else if ( spawnflags & CLOSEST_PLAYER )
		{
			headwatch = coop_returnPlayerClosestTo( ent );
		}

		//make sure we have any entity
		if ( !headwatch )
		{
			headwatch = world;
		}

		//reset head
		if ( spawnflags & STOP )
		{
			Event *even = new Event( EV_Actor_ResetHead );
			actor->ProcessEvent( even );
		}
		//watch
		else
		{
			Event *even = new Event( EV_Actor_HeadWatch );
			even->AddEntity( headwatch );
			even->AddFloat( fHeadSpeed );
			actor->ProcessEvent( even );
		}

		iValid++;
	}

	//make sure this is only applied to a actor
	if ( activator != world && activator->isSubclassOf( Actor ) )
	{
		if ( spawnflags & NOT_ACTIVATOR )
		{
			//placeholder
		}
		else
		{
			Actor *actor = ( Actor* )( Entity* )activator;

			//if turn towards closest player
			if ( spawnflags & WATCH_ACTIVATOR )
			{
				headwatch = activator;
			}
			//turn towards activator
			else if ( spawnflags & CLOSEST_PLAYER )
			{
				headwatch = coop_returnPlayerClosestTo( activator );
			}

			//make sure we have any entity
			if ( !headwatch )
			{
				headwatch = world;
			}

			//reset head
			if ( spawnflags & STOP )
			{
				Event *even = new Event( EV_Actor_ResetHead );
				actor->ProcessEvent( even );
			}
			//watch
			else
			{
				Event *even = new Event( EV_Actor_HeadWatch );
				even->AddEntity( headwatch );
				even->AddFloat( fHeadSpeed );
				actor->ProcessEvent( even );
			}
		}
	}

	//handle target
	if ( iValid == 0 && target.length() > 0 )
	{
		Entity *ent = coop_returnEntity( target.c_str() );
		if ( !ent )
		{
			gi.Printf( "TriggerCoopActorHeadwatch::Activate - Actor ($%s) was not found!\n" , target.c_str() );
			return;
		}
		if ( !ent->isSubclassOf( Actor ) )
		{
			gi.Printf( "TriggerCoopActorHeadwatch::Activate - Target Entity (%s) is not of type Actor!\n" , target.c_str() );
			return;
		}
		Actor *actor = ( Actor * )ent;

		//if turn towards closest player
		if ( spawnflags & TOWARDS_ACTIVATOR )
		{
			headwatch = activator;
		}
		//turn towards activator
		else if ( spawnflags & CLOSEST_PLAYER )
		{
			headwatch = coop_returnPlayerClosestTo( ent );
		}

		//make sure we have any entity
		if ( !headwatch )
		{
			headwatch = world;
		}

		//reset head
		if ( spawnflags & STOP )
		{
			Event *even = new Event( EV_Actor_ResetHead );
			actor->ProcessEvent( even );
		}
		//watch
		else
		{
			Event *even = new Event( EV_Actor_HeadWatch );
			even->AddEntity( headwatch );
			even->AddFloat( fHeadSpeed );
			actor->ProcessEvent( even );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ACTOR FIELDOFVIEW ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_actor_fieldofview(1 1 0) (-8 -8 -8) (8 8 8) NOT_ACTIVATOR
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sets the field of view of a Actor, allows for easier stealth missions

[ if ] NOT_ACTIVATOR is set the activating Actor will not be handled

"uservar1" Field of view angle
"uservar2" Vision distance
"uservar3-10" Targetname of actors to handle
"target" Sets Actor to handle (uservar3-10 overwrite)
*/
CLASS_DECLARATION( Trigger , TriggerCoopActorFieldofview , "trigger_coop_actor_fieldofview" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopActorFieldofview::Activate } ,
	{ NULL, NULL }
};
TriggerCoopActorFieldofview::TriggerCoopActorFieldofview()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopActorFieldofview::Activate( Event *ev )
{
#define NOT_ACTIVATOR (1<<0)
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );

	if ( !entityVar1 && !entityVar2 )
	{
		gi.Printf( "TriggerCoopActorFieldofview::Activate - You need to set at least FOV or VISION-DISTANCE!\n" );
		return;
	}

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	int iItem;
	int iValid = 0;
	ScriptVariable *entityVar;
	for ( iItem = 3; iItem <= 10; iItem++ )
	{
		entityVar = entityVars.GetVariable( va("uservar%d", iItem ) );
		
		if ( !entityVar )
			continue;
	
		str sTargetname = entityVar->stringValue();

		if ( !sTargetname.length() )
		{
			gi.Printf( "TriggerCoopActorFieldofview::Activate - uservar%d set but value is empty!\n", iItem );
			continue;
		}

		Entity *ent = coop_returnEntity( sTargetname );
		if ( !ent )
		{
			gi.Printf( "TriggerCoopActorFieldofview::Activate - Entity (%s) in uservar%d could not be found!\n" , sTargetname.c_str() , iItem );
			continue;
		}

		if ( !ent->isSubclassOf( Actor ) )
		{
			gi.Printf( "TriggerCoopActorFieldofview::Activate - Entity (%s) in uservar%d is not of type Actor!\n" , sTargetname.c_str() , iItem );
			continue;
		}

		Actor *actor = ( Actor* )ent;

		if ( entityVar1 )
			actor->sensoryPerception->SetFOV( entityVar1->floatValue() );

		if ( entityVar2 )
			actor->sensoryPerception->SetVisionDistance( entityVar2->floatValue() );

		iValid++;
	}

	//make sure this is only applied to a actor
	if ( activator != world && activator->isSubclassOf( Actor ) )
	{
		if ( spawnflags && NOT_ACTIVATOR )
		{
			//placeholder
		}
		else
		{
			Actor *actor = ( Actor* )( Entity* )activator;

			if ( entityVar1 )
				actor->sensoryPerception->SetFOV( entityVar1->floatValue() );

			if ( entityVar2 )
				actor->sensoryPerception->SetVisionDistance( entityVar2->floatValue() );
		}
	}

	//handle target
	if ( iValid == 0 && target.length() > 0 )
	{
		Entity *ent = coop_returnEntity( target );
		if ( !ent )
		{
			gi.Printf( "TriggerCoopActorFieldofview::Activate - Actor ($%s) was not found!\n" , target.c_str() );
			return;
		}
		if ( !ent->isSubclassOf( Actor ) )
		{
			gi.Printf( "TriggerCoopActorFieldofview::Activate - Target Entity (%s) is not of type Actor!\n" , target.c_str() );
			return;
		}
		Actor *actor = ( Actor * )ent;

		if ( entityVar1 )
			actor->sensoryPerception->SetFOV( entityVar1->floatValue() );

		if ( entityVar2 )
			actor->sensoryPerception->SetVisionDistance( entityVar2->floatValue() );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}


/*****************************************************************************/
/*QUAKED trigger_coop_actor_playdialog(1 1 0) (-8 -8 -8) (8 8 8) HEAD_HUD WAIT_FOR_DIALOG AI_ON	STOP
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Will make Actor play a dialog, will turn off ai (with 1 exception)
[ info ] Set path like this: dm/mp_picard1.mp3, Dialogs are played from:
:::::::::::::::::::::::::::::::::::::
loc/Eng/sound/dialog/
loc/Deu/sound/dialog/
:::::::::::::::::::::::::::::::::::::

[ if ] HEAD_HUD is set a HUD with the Head of the Actor will be shown
[ if ] WAITFORDIALOG is set the trigger/door in uservar4 will be activated on dialog end
[ if ] AI_ON is set the Actor ai will be activated after the dialog (1 sec delayed)
[ if ] STOP is set the Actor will stop their dialog	(this will not turn off ai)

"uservar1" part of the path from the dialog to play
"uservar2" volume of the dialog, default 1
"uservar3" minimum hearing distance for the dialog, default 128
"uservar4" trigger/door to activate when the dialog is finished (needs WAIT_FOR_DIALOG set)
"target" specifies the actor to play this dialog
"delay" will delay the firing of this trigger
*/
CLASS_DECLARATION( Trigger , TriggerCoopActorPlaydialog , "trigger_coop_actor_playdialog" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopActorPlaydialog::Activate } ,
	{ NULL, NULL }
};
TriggerCoopActorPlaydialog::TriggerCoopActorPlaydialog()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopActorPlaydialog::Activate( Event *ev )
{
#define HEAD_HUD (1<<0)
#define WAIT_FOR_DIALOG (1<<1)
#define AI_ON (1<<2)
#define STOP (1<<3)

	if ( target.length() < 1 )
	{
		gi.Printf( "TriggerCoopActorPlaydialog::Activate - No target Actor set!\n" );
		return;
	}

	Entity *ent = coop_returnEntity( target );

	if ( !ent )
	{
		gi.Printf( "TriggerCoopActorPlaydialog::Activate - Actor ($%s) was not found!\n" , target.c_str() );
		return;
	}

	//make sure this is only applied to a actor
	if ( !ent->isSubclassOf( Actor ) )
	{
		gi.Printf( "TriggerCoopActorPlaydialog::Activate - Target Entity (%s) is not of type Actor!\n" , target.c_str() );
		return;
	}

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	//need to turn ai off for animation to work
	Actor *actor = ( Actor * )ent;

	//STOP DIALOG
	if ( spawnflags & STOP )
	{
		actor->StopDialog();

		if ( spawnflags & AI_ON )
		{
			//post delayed ai_on, have a 1 sec delay for english/german compatibility
			Event *evAI = new Event( EV_Actor_AIOn );
			actor->ProcessEvent( evAI );
		}

		str sUservar4 = "";
		ScriptVariable *entityVar4 = entityVars.GetVariable( "uservar4" );

		if ( !entityVar4 )
			return;

		sUservar4 = entityVar4->stringValue();

		if ( !sUservar4.length() )
			return;

		Entity *ent2 = coop_returnEntity( sUservar4 );

		if ( !ent2 )
		{
			gi.Printf( "TriggerCoopActorPlaydialog::Activate - Entity (%s) set in uservar4 not found!\n" , sUservar4.c_str() );
			return;
		}
		//post delayed ai_on, have a 1 sec delay for english/german compatibility
		Event *evAfter;
		evAfter = new Event( EV_Activate );
		activator != NULL ? evAfter->AddEntity( activator ) : evAfter->AddEntity( world );
		ent2->PostEvent( evAfter , delay );
	}

	actor->TurnAIOff();

	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	ScriptVariable *entityVar3 = entityVars.GetVariable( "uservar3" );
	ScriptVariable *entityVar4 = entityVars.GetVariable( "uservar4" );


	str sUservar1 = "";
	float fUservar2 = 0.0f;
	float fUservar3 = 0.0f;
	str sUservar4 = "";

	if ( entityVar1 )
		sUservar1 = entityVar1->stringValue();

	if ( sUservar1.length() < 1 )
	{
		gi.Printf( "TriggerCoopActorPlaydialog::Activate - uservar1 was empty, needs dialog path\n" );
		return;
	}

	if ( entityVar2 )
		fUservar2 = entityVar2->floatValue();

	if ( entityVar3 )
		fUservar3 = entityVar3->floatValue();

	if ( entityVar4 )
		sUservar4 = entityVar4->stringValue();

	//set default if not set
	if ( fUservar2 <= 0.0f ) {
		fUservar2 = 1.0f;
	}

	//set default if not set
	if ( fUservar3 <= 63.0f ) {
		fUservar3 = 64.0f;
	}

	float fHead = 0;
	if ( spawnflags & HEAD_HUD )
	{
		fHead = 1.0f;
	}

	//start playiung dialog now
	Event *evPlaydialog = new Event( EV_Actor_PlayDialog );
	evPlaydialog->AddString( va( "localization/sound/dialog/%s" , sUservar1.c_str() ) );
	evPlaydialog->AddFloat( fUservar2 );
	evPlaydialog->AddFloat( fUservar3 );
	evPlaydialog->AddFloat( fHead );
	actor->ProcessEvent( evPlaydialog );

	if ( spawnflags & AI_ON )
	{
		//post delayed ai_on, have a 1 sec delay for english/german compatibility
		Event *evAI = new Event( EV_Actor_AIOn );
		actor->PostEvent( evAI , ( ( actor->GetDialogRemainingTime() + 1.0f ) + delay ) );
	}

	if ( spawnflags & WAIT_FOR_DIALOG )
	{
		if ( !sUservar4.length() )
		{
			gi.Printf( "TriggerCoopActorPlaydialog::Activate - WAIT_FOR_DIALOG SET but uservar4 is empty!\n" );
		}
		else
		{
			Entity *ent2 = coop_returnEntity( sUservar4 );

			if ( !ent2 )
			{
				gi.Printf( "TriggerCoopActorPlaydialog::Activate - Entity (%s) set in uservar4 not found!\n" , sUservar4.c_str() );
				return;
			}

			//post delayed ai_on, have a 1 sec delay for english/german compatibility
			Event *evAfter;
			evAfter = new Event( EV_Activate );
			activator != NULL ? evAfter->AddEntity( activator ) : evAfter->AddEntity( world );
			ent2->PostEvent( evAfter , ( ( actor->GetDialogRemainingTime() + 1.0f ) + delay ) );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ACTOR WALKTO ############################ */

/*****************************************************************************/
/*QUAKED trigger_coop_actor_walkto(1 1 0) (-8 -8 -8) (8 8 8) RUN AI_ON
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.
[ ! ] For this to work you need to place info_pathnodes to layout the path a actor can take.

[ info ] Makes targeted actor walk to a specific info_pathnodes

[ default ] actor will be using the animation "walk"
[ if ] RUN is set the actor will run (if supported)
[ if ] AI_ON is set the ai of the actor will turn be enabled after reaching its destination

"uservar1" specifies the targetname of the info_pathnode where the actor should go to
"uservar2" specifies the animation to use instead of "walk" or "run"
"uservar3" specifies the entity that shall be triggered after the actor has reached its destination
"target" specifies the actor to control
*/

CLASS_DECLARATION( Trigger , TriggerCoopActorWalkto , "trigger_coop_actor_walkto" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopActorWalkto::Activate },
	{ NULL, NULL }
};
TriggerCoopActorWalkto::TriggerCoopActorWalkto()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopActorWalkto::Activate( Event *ev )
{
	#define RUN ( 1 << 0 )
	#define AI_ON ( 1 << 1 )

	if ( target.length() < 1 )
	{
		gi.Printf( "TriggerCoopActorWalkto::Activate - No target Actor set!\n" );
		return;
	}

	Entity *ent = coop_returnEntity( target );


	if ( !ent )
	{
		gi.Printf( "TriggerCoopActorWalkto::Activate - Actor ($%s) was not found!\n" , target.c_str() );
		return;
	}

	//make sure this is only applied to a actor
	if ( !ent->isSubclassOf( Actor ) )
	{
		gi.Printf( "TriggerCoopActorWalkto::Activate - Target Entity (%s) is not of type Actor!\n" , target.c_str() );
		return;
	}

	Actor *actor = ( Actor* )ent;

	str sUservar1 = "";
	str sUservar2 = "";
	str sUservar3 = "";

	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	ScriptVariable *entityVar3 = entityVars.GetVariable( "uservar3" );

	if ( entityVar1 )
		sUservar1 = entityVar1->stringValue();

	if ( !sUservar1.length() )
	{
		gi.Printf( "TriggerCoopActorWalkto::Activate - uservar1 is empty, needs pathnode targetname!\n" );
		return;
	}

	if ( entityVar2 )
		sUservar2 = entityVar2->stringValue();

	if ( entityVar3 )
		sUservar3 = entityVar3->stringValue();

	str sAnimation = "walk";

	if ( sUservar2.length() > 0 )
	{
		sAnimation = sUservar2;
	}
	else if ( spawnflags & RUN )
	{
		sAnimation = "run";
	}

	//need to turn ai off for walkto to work
	actor->TurnAIOff();

	//post event
	Event *evWalkto = new Event( EV_Actor_WalkTo );
	evWalkto->AddString( "$" + sUservar1 );
	evWalkto->AddString( sAnimation.c_str() );
	actor->PostEvent( evWalkto , 0.1f );

	
	actor->activator = activator;

	if ( spawnflags & AI_ON )
	{
		actor->coop_behaviourAiOn = true;
	}

	if ( sUservar3.length() )
	{
		Entity *entActivate = coop_returnEntity( sUservar3 );
		if ( entActivate )
		{
			actor->coop_behaviourActivate = entActivate;
		}
		else
		{
			gi.Printf( "TriggerCoopActorWalkto::Activate Entity to activate (%s) was not found!\n" , sUservar3.c_str() );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ##############################  ACTOR DEATH ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_actor_death(1 1 0) (-8 -8 -8) (8 8 8)
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Activates given trigger once the actor has been killed

"target" specifies the actor
"uservar1" specifies the trigger to activate if this Actor dies
"uservar2" sets English reason of failure
"uservar3" sets German reason of failure
*/
CLASS_DECLARATION( Trigger , TriggerCoopActorDeath , "trigger_coop_actor_death" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopActorDeath::Activate } ,
	{ NULL, NULL }
};
TriggerCoopActorDeath::TriggerCoopActorDeath()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopActorDeath::Activate( Event *ev )
{
	if ( target.length() < 1 )
	{
		gi.Printf( "TriggerCoopActorDeath::Activate - There is no target set!\n" );
		return;
	}

	Entity *ent = NULL;
	ent = coop_returnEntity( target );

	if ( !ent )
	{
		gi.Printf( "TriggerCoopActorDeath::Activate - Actor ($%s) was not found!\n" , target.c_str() );
		return;
	}
	
	if ( !ent->isSubclassOf( Actor ) )
	{
		gi.Printf( "TriggerCoopActorDeath::Activate - Entity ($%s) is not a Actor!\n" , target.c_str() );
		return;
	}

	str sUservar1;
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );

	if ( !entityVar1 )
	{
		gi.Printf( "TriggerCoopActorDeath::Activate - No trigger/entity targetname specified in uservar1!\n" );
		return;
	}
	sUservar1 = entityVar1->stringValue();
	if ( !sUservar1.length() )
	{
		gi.Printf( "TriggerCoopActorDeath::Activate - No trigger/entity targetname specified in uservar1!\n" );
		return;
	}

	Actor *act = ( Actor* )ent;
	act->kill_thread = sUservar1.c_str();

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## ACTOR AI ############################ */

/*****************************************************************************/
/*QUAKED trigger_coop_actor_ai(1 1 0) (-8 -8 -8) (8 8 8) AI_ON AI_OFF AI_TOGGLE AGGRESSIVE NOT_AGGRESSIVE SLEEP WAKEUP
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Turns ON/OFF actor ai among other things.

[ if ] AI_ON is set the trigger will turn the actor ai ON
[ if ] AI_OFF is set the trigger will turn the actor ai OFF
[ if ] AI_TOGGLE is set the AI state will be toggled on/off
[ if ] AGGRESSIVE is set the actor will attack its enemies (needs ai_on)
[ if ] NOT_AGGRESSIVE is set the actor will be dorment if it is meant to
[ if ] SLEEP is set the actor will sleep until a enemy is spotted  (needs ai_on)
[ if ] WAKEUP is set the actor will wake up and become active again (needs ai_on)

"target" specifies the actor to control
*/

CLASS_DECLARATION( Trigger , TriggerCoopActorAi , "trigger_coop_actor_ai" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopActorAi::Activate } ,
	{ NULL, NULL }
};
TriggerCoopActorAi::TriggerCoopActorAi()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopActorAi::Activate( Event *ev )
{
#define AI_ON ( 1 << 0 )
#define AI_OFF ( 1 << 1 )
#define AGGRESSIVE ( 1 << 2 )
#define NOT_AGGRESSIVE ( 1 << 3 )
#define SLEEP ( 1 << 4 )
#define WAKEUP ( 1 << 5 )
#define DISABLE ( 1 << 6 )
#define NOT_DISABLE ( 1 << 7 )

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	if ( target.length() < 1 )
	{
		gi.Printf( "TriggerCoopActorAi::Activate - There is no target set!\n" );
		return;
	}

	Entity *ent = NULL;
	ent = coop_returnEntity( target );

 	if ( !ent )
	{
		gi.Printf( "TriggerCoopActorAi::Activate - Actor ($%s) was not found!\n" , target.c_str() );
		return;
	}

	if ( !ent->isSubclassOf( Actor ) )
	{
		gi.Printf( "TriggerCoopActorAi::Activate - Entity ($%s) is not a Actor!\n" , target.c_str() );
		return;
	}

	Actor *actor = ( Actor* )ent;
	actor->activator = activator;

	if ( spawnflags & AI_ON )
	{
		actor->TurnAIOn();
	}
	if ( spawnflags & AI_OFF )
	{
		actor->TurnAIOff();
	}
	if ( spawnflags & AGGRESSIVE )
	{
		actor->personality->SetAggressiveness(1.0f);
	}
	if ( spawnflags & NOT_AGGRESSIVE )
	{
		Event *evForgett = new Event( EV_Actor_ForgetEnemies );
		actor->PostEvent( evForgett , 0.0f );

		actor->personality->SetAggressiveness( 0.0f );
	}
	if ( spawnflags & SLEEP )
	{
		Event *evForgett = new Event( EV_Actor_ForgetEnemies );
		actor->PostEvent( evForgett , 0.0f );

		actor->Sleep();

		actor->animate->RandomAnimate( "idle", EV_StopAnimating ); //
		//RandomAnimate( ev->GetString( 1 ), NULL, legs );
	}
	if ( spawnflags & WAKEUP )
	{
		Event *evForgett = new Event( EV_Actor_ForgetEnemies );
		actor->PostEvent( evForgett , 0.0f );

		actor->Wakeup();
	}
	if ( spawnflags & DISABLE )
	{
		actor->SetActorFlag( ACTOR_FLAG_DISABLED , true );
	}
	if ( spawnflags & NOT_DISABLE )
	{
		//check what is happening when disabled, somehow the drone stays notsolid
		actor->SetActorFlag( ACTOR_FLAG_DISABLED , false );
		actor->setSolidType( SOLID_NOT );

		Event *evContents = new Event( EV_Contents );
		evContents->AddString("-shootable");
		actor->PostEvent( evContents , 0.0f );

		Event *evContents2 = new Event( EV_Contents );
		evContents2->AddString( "-targetable" );
		actor->PostEvent( evContents2 , 0.0f );

		//actor->contents targetable
		//actor->contents + shootable
		actor->_makeSolidASAP = true;
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}


/* ############################## CINEMATIC START ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_cinematic_start(1 1 0) (-8 -8 -8) (8 8 8) LETTER_BOX MOVE_ACTIVATOR ANTI_STUCK
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Starts a cinematic sequence

[ if ] LETTER_BOX is set, the game will add cinematic letterboxes (also hiding the hud)
[ if ] MOVE_ACTIVATOR is set, the activating player will be moved to the entity or position specified in uservar1
[ if ] ANTI_STUCK and MOVE_ACTIVATOR is set, the activator will handled in a fashion so it will not become stuck in a actor or player

"uservar1" specifies a entity or coordinates to move the activator to
"uservar2" specifies the angle the activator should look towards to after being moved (only if uservar1 holds cooridinates)
*/
CLASS_DECLARATION( Trigger , TriggerCoopCinematicStart , "trigger_coop_cinematic_start" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopCinematicStart::Activate } ,
	{ NULL, NULL }
};
TriggerCoopCinematicStart::TriggerCoopCinematicStart()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopCinematicStart::Activate( Event *ev )
{
#define LETTER_BOX ( 1 << 0)
#define MOVE_ACTIVATOR (1<<1)
#define ANTI_STUCK (1<<2)

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( other && other != world )
		activator = other;
	else
		activator = NULL;

	//start cinematic
	G_StartCinematic(); //G_StopCinematic();

	//freeze player
	level.playerfrozen = true;

	//letterbox
	if ( spawnflags & LETTER_BOX )
	{
		float time = 0.25f;
		level.m_letterbox_lastfraction = 1.0f / 8.0f;

		level.m_letterbox_time_start = time;
		level.m_letterbox_dir = letterbox_in;

		level.m_letterbox_time = time;
		level.m_letterbox_fraction = level.m_letterbox_lastfraction;
	}

	if ( activator && spawnflags & MOVE_ACTIVATOR && activator->isSubclassOf( Sentient ))
	{
		float fAngle = -666;
		Vector vNew = Vector( 0 , 0 , 0 );
		ScriptVariable *entityVar = entityVars.GetVariable( "uservar1" );
		ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );

		if ( entityVar2 )
			fAngle = entityVar2->floatValue();
		
		if ( entityVar )
		{
			str sDest = "";
			sDest = entityVar->stringValue();
			if ( sDest.length() )
			{
				Entity *eTarget = NULL;
				eTarget = coop_returnEntity( sDest.c_str() );

				if ( !eTarget )
				{
					vNew = coop_returnVectorFromString( sDest );
				}
				else
				{
					vNew = eTarget->origin;
					fAngle = eTarget->angles.y;
				}

				if ( vNew.length() )
				{
					Event *even = new Event( EV_Warp );
					even->AddVector( vNew );
					activator->ProcessEvent( even );

					
					if ( activator->isSubclassOf( Player ) )
					{
						Player *player = ( Player* )(Entity*)activator;
						player->SetViewAngles( Vector( 0 , fAngle , 0 ) );
					}
					else
					{
						Event *even1 = new Event( EV_SetAngle );
						even1->AddFloat( fAngle );
						activator->ProcessEvent( even1 );
					}

					if ( spawnflags & ANTI_STUCK )
					{
						Event *even2 = new Event( EV_BecomeNonSolid );
						activator->ProcessEvent( even2 );

						Event *even3 = new Event( EV_BecomeSolid );
						activator->PostEvent( even3 , 0.25f );
					}
				}
				else
				{
					gi.Printf( "TriggerCoopCinematicStart::Activate - MOVE_ACTIVATOR enabled, but uservar1 holds a invalid target/destination!\n" );
				}
			}
			else
			{
				gi.Printf( "TriggerCoopCinematicStart::Activate - MOVE_ACTIVATOR enabled, but uservar1 is empty!\n" );
			}
		}
		else
		{
			gi.Printf( "TriggerCoopCinematicStart::Activate - MOVE_ACTIVATOR enabled, but uservar1 is not set!\n" );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## CINEMATIC SKIP ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_cinematic_skip(1 1 0) (-8 -8 -8) (8 8 8)

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Allows to skip the current ciematic sequence by players
[ info ] This will not end cinematic, you need to use a trigger_coop_cinematic_end

"target" sets the trigger to activate when cinematic is skipped
*/
CLASS_DECLARATION( Trigger , TriggerCoopCinematicSkip , "trigger_coop_cinematic_skip" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopCinematicSkip::Activate } ,
	{ NULL, NULL }
};
TriggerCoopCinematicSkip::TriggerCoopCinematicSkip()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopCinematicSkip::Activate( Event *ev )
{
	if ( target.length() < 1 )
	{
		gi.Printf( "TriggerCoopCinematicSkip::Activate - No target entity set!\n" );
		return;
	}

	Entity *ent = coop_returnEntity( target );


	if ( !ent )
	{
		gi.Printf( "TriggerCoopCinematicSkip::Activate - Entity ($%s) was not found!\n" , target.c_str() );
		return;
	}

	world->skipthread = "$";
	world->skipthreadEntity = ent;

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## CINEMATIC END ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_cinematic_end(1 1 0) (-8 -8 -8) (8 8 8)

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Ends a cinematic sequence
*/
CLASS_DECLARATION( Trigger , TriggerCoopCinematicEnd , "trigger_coop_cinematic_end" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopCinematicEnd::Activate } ,
	{ NULL, NULL }
};
TriggerCoopCinematicEnd::TriggerCoopCinematicEnd()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopCinematicEnd::Activate( Event *ev )
{
	if ( !level.cinematic )
	{
		gi.Printf( "TriggerCoopCinematicEnd::Activate - Trigger activated, but no cinematic is active, abborting!\n" );
		return;
	}

	G_StopCinematic();

	float fTime = 0.5f;
	level.m_letterbox_time_start = fTime;
	level.m_letterbox_dir = letterbox_out;

	level.m_letterbox_time = fTime;
	level.m_letterbox_fraction = level.m_letterbox_lastfraction;

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## CINEMATIC CUECAMERA ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_cinematic_cuecamera(1 1 0) (-8 -8 -8) (8 8 8) CUT

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sets view of players to camera position

[ if ] CUT is set, the transition to the camera will be instant, without transition from the current camera

"uservar1" sets the time the transition between the player view and camera will take
"target" specified the targetname of the camera to view from
*/
CLASS_DECLARATION( Trigger , TriggerCoopCinematicCuecamera , "trigger_coop_cinematic_cuecamera" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopCinematicCuecamera::Activate } ,
	{ NULL, NULL }
};
TriggerCoopCinematicCuecamera::TriggerCoopCinematicCuecamera()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopCinematicCuecamera::Activate( Event *ev )
{
#define CUT (1 << 0)
	Entity *ent = NULL;

	if( target.length() )
	{
		ent = coop_returnEntity( target );
	}
	else
	{
		gi.Printf( "TriggerCoopCinematicCuecamera::Activate - No camera targetname given!\n" );
		return;
	}

	//cuecamera (sCamera)
	if ( !ent )
	{
		gi.Printf( "TriggerCoopCinematicCuecamera::Activate - Camera (%s) not found in the level!\n" , target.c_str() );
		return;
	}

	//use globalnamespace (::) here
	if ( spawnflags & CUT )
	{
		::SetCamera( ent , -1.0f );//cam, switchtime
	}
	else
	{
		float transitiontime = 0.0f;
		ScriptVariable *entityVar = entityVars.GetVariable( "uservar1" );
		if( entityVar )
			transitiontime = entityVar->floatValue();

		if ( transitiontime == 0.0f )
		{
			transitiontime = 0.2f; //set to default transition time
		}
		::SetCamera( ent , transitiontime );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## CINEMATIC FOLLOWPATH ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_cinematic_followpath(1 1 0) (-8 -8 -8) (8 8 8)

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sets path the camera is suppose to follow

"uservar1" specifies the targetname of the camera
"uservar2" specifies the targetname of the camera
*/
CLASS_DECLARATION( Trigger , TriggerCoopCinematicFollowpath , "trigger_coop_cinematic_followpath" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopCinematicFollowpath::Activate } ,
	{ NULL, NULL }
};
TriggerCoopCinematicFollowpath::TriggerCoopCinematicFollowpath()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopCinematicFollowpath::Activate( Event *ev )
{
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	str sUservar1 = "";
	str sUservar2 = "";

	if( entityVar1 )
		sUservar1 = entityVar1->stringValue();

	if( entityVar2 )
		sUservar2 = entityVar2->stringValue();


	if ( !sUservar1.length() ) {
		gi.Printf( "TriggerCoopCinematicFollowpath::Activate - No camera targetname in uservar1 specified!\n" );
		return;
	}
	if ( !sUservar2.length() ) {
		gi.Printf( "TriggerCoopCinematicFollowpath::Activate - No path targetname in uservar2 specified!\n" );
		return;
	}
	
	//get camera
	Entity *cam = coop_returnEntity( sUservar1 );
	if ( !cam )
	{
		gi.Printf( "TriggerCoopCinematicFollowpath::Activate - camera (%s) not found!\n" , sUservar1.c_str() );
		return;
	}

	//get path
	Entity *path = coop_returnEntity( sUservar2 );
	if ( !path )
	{
		gi.Printf( "TriggerCoopCinematicFollowpath::Activate - path (%s) not found!\n" , sUservar2.c_str() );
		return;
	}

	//make cam follow path
	Event *evFollow = new Event( EV_Camera_Follow );
	evFollow->AddString( "$" + sUservar2 );
	cam->PostEvent( evFollow , 0.0f );

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## CINEMATIC FOV ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_cinematic_fov(1 1 0) (-8 -8 -8) (8 8 8)

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sets view of field angle on a camera. Will force a cut to this camera

"uservar1" specifies the field of view angle
"target" specified the targetname of the camera to view from
*/
CLASS_DECLARATION( Trigger , TriggerCoopCinematicFov , "trigger_coop_cinematic_fov" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopCinematicFov::Activate } ,
	{ NULL, NULL }
};
TriggerCoopCinematicFov::TriggerCoopCinematicFov()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopCinematicFov::Activate( Event *ev )
{
	if ( !target.length() )
	{
		gi.Printf( "TriggerCoopCinematicFov::Activate - No camera was targeted/specified!\n" );
		return;
	}

	Entity *camera = coop_returnEntity( target );
	if ( !camera )
	{
		gi.Printf( "TriggerCoopCinematicFov::Activate - Camera (%s) was not found!\n", target.c_str() );
		return;
	}

	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	float sUservar1 = 0.0f;
	
	if( entityVar1 )
		sUservar1 = entityVar1->floatValue();

	if ( sUservar1 < 1.0f ) {
		sUservar1 = 90.0f;//default fov
	}

	Camera *cam = (Camera *)camera;
	cam->SetFOV( sUservar1 , 0.2f ); //useing default fadetime
	
	//we need to cut to this cam or fov will not work
	Event *evCut = new Event( EV_Camera_Cut );
	cam->PostEvent( evCut , 0.0f );

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## MISSION MARKER ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_mission_marker(1 1 0) (-8 -8 -8) (8 8 8) UNMARK
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Marks a Mission relevant entity on the Radar
[ info ] This also works with Coop Levels

[ if ] UNMARK is set the objects will be unmarked (removed from the radar)

"uservar1" sets targetname of object to mark
*/
CLASS_DECLARATION( Trigger , TriggerCoopMissionMarker , "trigger_coop_mission_marker" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopMissionMarker::Activate } ,
	{ NULL, NULL }
};
TriggerCoopMissionMarker::TriggerCoopMissionMarker()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopMissionMarker::Activate( Event *ev )
{
#define UNMARK (1<<0)

	int iItem;
	for ( iItem = 1; iItem <= 10; iItem++ )
	{
		str sTargetname = "";
		ScriptVariable *entityVar;
		entityVar = entityVars.GetVariable( va( "uservar%d" , iItem ) );

		if ( !entityVar )
			continue;

		sTargetname = entityVar->stringValue();

		if ( !sTargetname.length() )
			continue;
		
		Entity *ent = coop_returnEntity( sTargetname.c_str() );
		if ( !ent )
		{
			gi.Printf( "TriggerCoopMissionMarker::Activate - Entity ($%s) specified in uservar%d, could not be found!" , sTargetname.c_str(), iItem );
			continue;
		}

		Event *event = new Event( EV_SetMissionObjective );
		
		if ( spawnflags & UNMARK )
			event->AddInteger( 0 );
		else
			event->AddInteger( 1 );

		ent->ProcessEvent( event );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## MISSION TACTICAL ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_mission_tactical(1 1 0) (-8 -8 -8) (8 8 8)

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] sets tactical info for player(s)
[ info ] tactical info is shown in the Coop Mission Objective Menu 

"uservar1" sets tactical info 1 text in english
"uservar2" sets tactical info 2 text in english
"uservar3" sets tactical info 3 text in english
"uservar4" sets tactical info 1 text in german
"uservar5" sets tactical info 2 text in german
"uservar6" sets tactical info 3 text in german
*/
CLASS_DECLARATION( Trigger , TriggerCoopMissionTactical , "trigger_coop_mission_tactical" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopMissionTactical::Activate } ,
	{ NULL, NULL }
};
TriggerCoopMissionTactical::TriggerCoopMissionTactical()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopMissionTactical::Activate( Event *ev )
{
	str sTacDeu;
	str sTacEng;
	int iValid = 0;
	short int iItem;
	for ( iItem = 1; iItem < 4; iItem++ )
	{
		ScriptVariable *entityVar, *entityVar2;
		entityVar = entityVars.GetVariable( va( "uservar%d" , iItem ) );
		entityVar2 = entityVars.GetVariable( va( "uservar%d" , ( iItem + 3) ) );

		//str sTacEng = "";
		//str sTacDeu = "";

		//english
		if ( entityVar )
		{
			sTacEng = entityVar->stringValue();
			if ( sTacEng.length() )
			{
				program.setStringVariableValue( va( "coop_string_objectiveTacticalInfo%d" , iItem ) , entityVar->stringValue() );
				iValid++;
			}
		}

		sTacDeu = sTacEng;

		//german
		if ( entityVar2 )
		{
			if ( !sTacDeu.length() && sTacEng.length() ) {
				gi.Printf( "TriggerCoopMissionTactical::Activate - uservar%d, was empty - unsing text from uservar%d", (iItem + 3), iItem );
				sTacDeu = sTacEng;
			}
			else {
				sTacDeu = entityVar2->stringValue();
			}
			program.setStringVariableValue( va( "coop_string_objectiveTacticalInfo%d_deu" , iItem ) , sTacDeu.c_str() );
		}
	}

	if ( iValid == 0 )
	{
		gi.Printf( "TriggerCoopMissionTactical::Activate - uservar1-3 are empty, should contain tactical info text in english!" );
		gi.Printf( "TriggerCoopMissionTactical::Activate - uservar4-6, should contain tactical info text in german!" );
		return;
	}

	int iPlayer;
	Player *currentPlayer;
	for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
	{
		currentPlayer = multiplayerManager.getPlayer( iPlayer );

		if ( !currentPlayer )
			continue;

		int short iItemTac;
		for (iItemTac = 1; iItemTac < 4; iItemTac++ ){
			coop_objectives_tacticalShow( currentPlayer , iItemTac);
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## MISSION STORY ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_mission_story(1 1 0) (-8 -8 -8) (8 8 8)

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] sets a mission story for the player(s)
[ info ] Story is shown in the Coop Mission Objective Menu

"uservar1" sets English Mission Backgroundstory
"uservar2" sets German Mission Backgroundstory
*/
CLASS_DECLARATION( Trigger , TriggerCoopMissionStory , "trigger_coop_mission_story" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopMissionStory::Activate } ,
	{ NULL, NULL }
};
TriggerCoopMissionStory::TriggerCoopMissionStory()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopMissionStory::Activate( Event *ev )
{
	ScriptVariable *entityVar;
	entityVar = entityVars.GetVariable( "uservar1" );

	if ( !entityVar )
	{
		gi.Printf( "TriggerCoopMissionStory::Activate - uservar1 is not set, should contain English Backgroundsdtory!" );
		return;
	}

	str sStoryEng = entityVar->stringValue();
	entityVar = NULL;

	if ( !sStoryEng.length() )
	{
		gi.Printf( "TriggerCoopMissionStory::Activate - uservar1 is not set, should contain English Backgroundsdtory!" );
		return;
	}

	str sStoryDeu = "";
	entityVar = entityVars.GetVariable( "uservar2" );

	if ( !entityVar )
	{
		gi.Printf( "TriggerCoopMissionStory::Activate - uservar2 is not set, German Backgroundsdtory! Using English" );
		sStoryDeu = sStoryEng;
		return;
	}
	else
	{
		sStoryDeu = entityVar->stringValue();
		if ( !sStoryDeu.length() ) {
			gi.Printf( "TriggerCoopMissionStory::Activate - uservar2 is not set, German Backgroundsdtory! Using English" );
			sStoryDeu = sStoryEng;
		}
	}
	game.coop_story_deu = sStoryDeu;
	game.coop_story = sStoryEng;

	int iPlayer;
	Player *currentPlayer;
	for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
	{
		currentPlayer = multiplayerManager.getPlayer( iPlayer );

		if ( !currentPlayer )
			continue;

		coop_storySet( currentPlayer );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## MISSION OBJECTIVE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_mission_ojective(1 1 0) (-8 -8 -8) (8 8 8) COMPLETE FAIL

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] sets a mission objective for the player(s)
[ info ] objectives are shown in the Coop Mission Objective Menu

[ if ] COMPLETE is set the objective will be completed
[ if ] FAIL is set the objective will be failed

"uservar1" sets objective item number (1-8)
"uservar2" sets objective english text
"uservar3" sets objective german text
*/
CLASS_DECLARATION( Trigger , TriggerCoopMissionObjective , "trigger_coop_mission_ojective" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopMissionObjective::Activate } ,
	{ NULL, NULL }
};
TriggerCoopMissionObjective::TriggerCoopMissionObjective()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopMissionObjective::Activate( Event *ev )
{
#define COMPLETE (1<<0)
#define FAIL (1<<1)

	str sObjectiveState = "ObjectiveIncomplete";
	ScriptVariable *entityVar;
	entityVar = entityVars.GetVariable( "uservar1" );

	if ( !entityVar )
	{
		gi.Printf( "TriggerCoopMissionObjective::Activate - uservar1 is not set, should be item number!" );
		return;
	}

	int iItem = entityVar->intValue();
	entityVar = NULL;

	//check objective item range
	if ( iItem < 1 )
	{
		iItem = 1;
		gi.Printf( "TriggerCoopMissionObjective::Activate - objective item number out of range in uservar1. Range 1-8" );
	}
	else if ( iItem > 8.0f )
	{
		iItem = 8;
		gi.Printf( "TriggerCoopMissionObjective::Activate - objective item number out of range in uservar1. Range 1-8" );
	}

	//set complete state
	if ( spawnflags & COMPLETE ) {
		sObjectiveState = "ObjectiveComplete";
	}
	//set failed state
	else if ( spawnflags & FAIL )
	{
		sObjectiveState = "ObjectiveFailed";
	}
	//set incomplete state
	else {
		sObjectiveState = "ObjectiveIncomplete";
	}

	str sTemp;
	str sObjectiveEnglish = program.getStringVariableValue( va( "coop_string_objectiveItem%d" , iItem ) );
	str sObjectiveGerman = program.getStringVariableValue( va( "coop_string_objectiveItem%d_deu" , iItem ) );

	if ( !sObjectiveEnglish.length() )
	{
		entityVar = entityVars.GetVariable( "uservar2" );
		if ( !entityVar )
		{
			gi.Printf( "TriggerCoopMissionObjective::Activate - uservar2 is not set, should be english text!" );
			return;
		}

		sObjectiveEnglish = entityVar->stringValue();
		entityVar = NULL;

		if ( !sObjectiveEnglish.length() )
		{
			gi.Printf( "TriggerCoopMissionObjective::Activate - uservar2 is empty should be english text!" );
			return;
		}

		sTemp = va( "coop_string_objectiveItem%d" , iItem );
		program.setStringVariableValue( sTemp.c_str() , sObjectiveEnglish.c_str() );

		entityVar = entityVars.GetVariable( "uservar3" );

		if ( !entityVar )
		{
			gi.Printf( "TriggerCoopMissionObjective::Activate - uservar3 is not set, should be german text! Using now english text!" );
			sTemp = va( "coop_string_objectiveItem%d_deu" , iItem );
			program.setStringVariableValue( sTemp.c_str() , sObjectiveEnglish.c_str() );
		}
		else {
			sObjectiveGerman = entityVar->stringValue();
			if ( sObjectiveGerman.length() ) {
				sTemp = va( "coop_string_objectiveItem%d_deu" , iItem );
				program.setStringVariableValue( sTemp.c_str() , sObjectiveGerman.c_str() );
			}
			else {
				gi.Printf( "TriggerCoopMissionObjective::Activate - uservar3 is not set, should be german text! Using now english text!" );
				sTemp = va( "coop_string_objectiveItem%d_deu" , iItem );
				program.setStringVariableValue( sTemp.c_str() , sObjectiveEnglish.c_str() );
			}
		}
	}
	coop_objectivesUpdate( sObjectiveState , va( "%d" , iItem ) , "1" );

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## MISSION COMPLETE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_mission_complete(1 1 0) (-8 -8 -8) (8 8 8) FAIL SHOW_SUCCESS

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sets mission status

[ if ] FAIL is set the mission will fail instead
[ if ] SHOW_SUCCESS is set the Mission Success Screen will be shown

"uservar1" sets next map to load
"uservar2" sets reason of failure
*/
CLASS_DECLARATION( Trigger , TriggerCoopMissionComplete , "trigger_coop_mission_complete" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopMissionComplete::Activate } ,
	{ NULL, NULL }
};
TriggerCoopMissionComplete::TriggerCoopMissionComplete()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopMissionComplete::Activate( Event *ev )
{
#define FAIL ( 1<<0 )
#define SHOW_SUCCESS (1<<1)

	str sValue = "";
	ScriptVariable *entityVar;

	//failure
	if ( spawnflags & FAIL )
	{
		entityVar = entityVars.GetVariable( "uservar2" );

		if ( entityVar )
		{
			sValue = entityVar->stringValue();

			if ( sValue.length() )
			{
				world->entityVars.SetVariable( "globalCoop_missionFailedUserString" , 1.0f );
				levelVars.SetVariable( "coop_failureReason" , sValue.c_str() );
			}			
		}

		if ( !ExecuteThread( "coop_missionFailure" , true , this ) ) {
			gi.Printf( "TriggerCoopMissionComplete::Activate - Null game script (coop_missionFailure)" );
		}
		return;
	}

	//succsess show
	if ( spawnflags & SHOW_SUCCESS )
	{
		program.setFloatVariableValue( "coop_bool_showMissionSucessHud" , 1 );
	}

	entityVar = entityVars.GetVariable( "uservar1" );

	if ( !entityVar )
	{
		gi.Printf( "TriggerCoopMissionComplete::Activate - uservar1 is not set, should hold name of next level!\n" );
		return;
	}

	sValue = entityVar->stringValue();

	if ( sValue.length() < 1 )
	{
		gi.Printf( "TriggerCoopMissionComplete::Activate - uservar1 is empty, should hold name of next level!\n" );
	}

	levelVars.SetVariable( "coop_NextMapToLoad" , sValue.c_str() );

	if ( !ExecuteThread( "coop_endLevel" , true , this ) ) {
		gi.Printf( "TriggerCoopMissionComplete::Activate - Null game script (coop_endLevel)" );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## PLAYER HUD ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_player_hud(1 1 0) (-8 -8 -8) (8 8 8) REMOVE ALL_PLAYERS
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Adds a Hud to player(s)

[ if ] REMOVE is set, the hud will be removed instead
[ if ] ALL_PLAYERS is set, this applies to all players

"uservar1" sets the name of the hud to add
*/
CLASS_DECLARATION( Trigger , TriggerCoopPlayerHud , "trigger_coop_player_hud" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopPlayerHud::Activate } ,
	{ NULL, NULL }
};
TriggerCoopPlayerHud::TriggerCoopPlayerHud()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopPlayerHud::Activate( Event *ev )
{
#define REMOVE ( 1<<0 )
#define ALL_PLAYERS ( 1<<1 )

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	int iPlayer;
	Player* currentPlayer;
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );

	if ( !entityVar1 )
	{
		gi.Printf( "TriggerCoopPlayerHud::Activate - uservar1 is not set, should hold name of the hud!\n" );
		return;
	}

 	str sHud = entityVar1->stringValue();
	if ( !sHud.length() )
	{
		gi.Printf( "TriggerCoopPlayerHud::Activate - uservar1 empty or to few charaters!\n" );
		return;
	}

	//give to all
	if ( spawnflags & ALL_PLAYERS )
	{
		for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
		{
			currentPlayer = multiplayerManager.getPlayer( iPlayer );

			if ( !currentPlayer || multiplayerManager.isPlayerSpectator( currentPlayer ) || currentPlayer->health <= 0 )
				continue;
			
			//remove first to avoid certain bugs after dieing and such
			upgPlayerDelayedServerCommand( currentPlayer->entnum , va("ui_removehud %s", sHud.c_str())); //[b607] added c_str()

			if ( spawnflags & REMOVE )
			{
				//placeholder
			}
			else
			{
				upgPlayerDelayedServerCommand( currentPlayer->entnum , va( "ui_addhud %s" , sHud.c_str())); //[b607] added c_str()
			}
		}
	}
	//give to activator
	else
	{
		if ( activator && ( Entity * )activator->isSubclassOf( Player ) )
		{
			currentPlayer = ( Player* )( Entity * )activator;
			if ( currentPlayer && !multiplayerManager.isPlayerSpectator( currentPlayer ) && currentPlayer->health > 0 )
			{
				//remove first to avoid certain bugs after dieing and such
				upgPlayerDelayedServerCommand( currentPlayer->entnum , va( "ui_removehud %s" , sHud.c_str()) ); //[b607] added c_str()

				if ( spawnflags & REMOVE )
				{
					//placeholder
				}
				else
				{
					upgPlayerDelayedServerCommand( currentPlayer->entnum , va( "ui_addhud %s" , sHud.c_str() ) ); //[b607] added c_str()
				}
			}
		}
		else
		{
			if ( activator == world )
			{
				gi.Printf( "TriggerCoopPlayerHud::Activate - Activator is WORLD, trigger might have been called from script or console!\n" );
			}
			else
			{
				gi.Printf( "TriggerCoopPlayerHud::Activate - Activator is not of class Player!\n" );
			}
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ##############################  PLAYER FOREACH ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_player_foreach(1 1 0) (-8 -8 -8) (8 8 8) SPECTATORS NOT_DEAD
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Activates entity (or entities) once for each player that is on the server

[ if ] SPECTATORS is set spectators will be counted in
[ if ] NOT_DEAD is set dead/respawning players will not be counted in
[ if ] uservar1 is set, it will be counted up for each player by 1,
starting from 1 to 8, as follows: $targetname1, $targetname2, $targetname3...

"target" sets a trigger/door/entity to activate (will be overwritten if uservar1 is set)
"uservar1" sets the targetname of the trigger to activate (without ending number)
"uservar2" sets the minimum of times this trigger should be fired
"uservar3" sets the maximum of times this trigger should be fired
"uservar4" additional times this trigger should be fired
"wait" sets time to wait between each activation
*/
CLASS_DECLARATION( Trigger , TriggerCoopPlayerForeach , "trigger_coop_player_foreach" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopPlayerForeach::Activate } ,
	{ NULL, NULL }
};
TriggerCoopPlayerForeach::TriggerCoopPlayerForeach()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopPlayerForeach::Activate( Event *ev )
{
#define SPECTATORS ( 1<<0 )
#define NOT_DEAD ( 1<<1 )

	float fWaitTime = 0.0f;
	int iMinimum = 0;
	int iMaximum = 0;
	int iValidPlayers = 0;
	int iValid = 0;
	int iCurrentIndex = 0;
	int iRounds = 0;
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	ScriptVariable *entityVar3 = entityVars.GetVariable( "uservar3" );
	ScriptVariable *entityVar4 = entityVars.GetVariable( "uservar4" );

	if ( entityVar2 )
		iMinimum = entityVar2->intValue();

	if ( entityVar3 )
		iMaximum = entityVar3->intValue();

	if ( entityVar4 )
		iValidPlayers = entityVar4->intValue();

	Player* currentPlayer = NULL;
	int iPlayer = 0;
	for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
	{
		currentPlayer = multiplayerManager.getPlayer( iPlayer );

		if ( !currentPlayer )
			continue;

		if ( spawnflags & NOT_DEAD && currentPlayer->health <= 0 )
			continue;

		if ( spawnflags & SPECTATORS ) {
		}
		else {
			if ( multiplayerManager.isPlayerSpectator( currentPlayer ) )
				continue;
		}

		iValidPlayers++;
	}

	if ( iValidPlayers == 0)
	{
		gi.Printf( "TriggerCoopPlayerForeach::Activate - No valid players found!\n" );
		return;
	}


	//get the numbers right
	iRounds = iValidPlayers;

	if ( iRounds < iMinimum )
		iRounds = iMinimum;

	if ( iMaximum > 0 && iRounds > iMaximum )
		iRounds = iMaximum;

	if( iMaximum > 0 && iMinimum > iMaximum )
		gi.Printf( "TriggerCoopPlayerForeach::Activate - Minimum in uservar2 (%d) is bigger than the maximum in uservar3(%d)!\n", iMinimum , iMaximum );
	
	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );

	if ( !entityVar1 )
	{
		if ( !target.length() )
		{
			gi.Printf( "TriggerCoopPlayerForeach::Activate - target is not set, should hold trargetname of entity to activate!\n" );
			return;
		}
		else
		{
			for ( iValid = iRounds; iValid > 0; iValid-- ) {
				Entity* eActivate = coop_returnEntity( target.c_str() );

				if ( eActivate )
				{
					iCurrentIndex++;

					Event *evAfter;
					evAfter = new Event( EV_Activate );
					activator != NULL ? evAfter->AddEntity( activator ) : evAfter->AddEntity( world );
					eActivate->PostEvent( evAfter , fWaitTime );
				}
				else {
					gi.Printf( "TriggerCoopPlayerForeach::Activate - Couldn't find (%s) to activate!\n" , target.c_str() );
				}
				fWaitTime += wait;
				eActivate = NULL;
			}
			return;
		}
	}

	if ( entityVar1 )
	{
		str sTargetname = entityVar1->stringValue();
		if ( !sTargetname.length() ){
			gi.Printf( "TriggerCoopPlayerForeach::Activate - uservar1 is not set, should hold (base)trargetname of entity to activate!\n" );
			return;
		}
		
		for ( iValid = iRounds; iValid > 0; iValid-- ) {
			iCurrentIndex++;
			str sTempTargetname = va( "%s%d" , sTargetname.c_str() , iCurrentIndex );
			Entity* eActivate =  coop_returnEntity( sTempTargetname.c_str() );

			if ( eActivate )
			{
				Event *evAfter;
				evAfter = new Event( EV_Activate );
				activator != NULL ? evAfter->AddEntity( activator ) : evAfter->AddEntity( world );
				eActivate->PostEvent( evAfter , fWaitTime );
			}
			else {
				gi.Printf( "TriggerCoopPlayerForeach::Activate - Couldn't find (%s) to activate!\n" , sTempTargetname.c_str() );
			}
			fWaitTime += wait;
			eActivate = NULL;
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}


/* ############################## PLAYER AMMO ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_player_ammo(1 1 0) (-8 -8 -8) (8 8 8) ALL_PLAYERS

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Gives ammo to player(s)
[ info ] Ammo types are: Plasma, Phaser, Idryll, Fed
[ info ] Accepts also negative value in uservar2

[ if ] ALL_PLAYERS is set, all players will get ammo

"uservar1" ammo type name
"uservar2" amount of ammo to give
*/
CLASS_DECLARATION( Trigger , TriggerCoopPlayerAmmo , "trigger_coop_player_ammo" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopPlayerAmmo::Activate } ,
	{ NULL, NULL }
};
TriggerCoopPlayerAmmo::TriggerCoopPlayerAmmo()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopPlayerAmmo::Activate( Event *ev )
{
#define ALL_PLAYERS ( 1<<0 )

#define COOP_MAX_WEAPON_REGISTER 15
#define MAX_USERVARS 2

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	int iPlayer;
	Player* currentPlayer;
	Vector vector;

	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );
	str sItem = "";
	float fAmount = 1.0f;

	if ( !entityVar1 )
	{
		gi.Printf( "TriggerCoopPlayerAmmo::Activate - uservar1 is not set, should hold name of ammo/item to give!\n" );
		return;
	}

	sItem = entityVar1->stringValue();
	if ( sItem.length() < 1 )
	{
		gi.Printf( "TriggerCoopPlayerAmmo::Activate - uservar1 empty or to few charaters!\n" );
		return;
	}

	//only set uservar2 if not empty, otherwise keep 1.0f
	if ( entityVar2 )
	{
		fAmount = entityVar2->floatValue();
	}

	//give to all
	if ( spawnflags & ALL_PLAYERS )
	{
		for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
		{
			currentPlayer = multiplayerManager.getPlayer( iPlayer );

			if ( !currentPlayer || multiplayerManager.isPlayerSpectator( currentPlayer ) || currentPlayer->health <= 0 )
				continue;

			currentPlayer->GiveAmmo( sItem.c_str() , fAmount , false , -1 );
		}
	}
	//give to activator
	else
	{
		if ( activator && activator->isSubclassOf( Player ) )
		{
			currentPlayer = ( Player* )( Entity * )activator;
			if ( currentPlayer && !multiplayerManager.isPlayerSpectator( currentPlayer ) && currentPlayer->health > 0 )
			{
				currentPlayer->GiveAmmo( sItem.c_str() , fAmount , false , -1 );
			}
		}
		else
		{
			if ( activator == world )
			{
				gi.Printf( "TriggerCoopPlayerAmmo::Activate - Activator is WORLD, trigger might have been called from script or console!\n" );
			}
			else
			{
				gi.Printf( "TriggerCoopPlayerAmmo::Activate - Activator is not of class Player!\n" );
			}
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## PLAYER ITEM ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_player_item(1 1 0) (-8 -8 -8) (8 8 8) TAKE ALL_PLAYERS REGISTER USE
[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Gives a item (weapon/powerup...) to player(s)

[ default ] gives weapon to the activating player
[ if ] TAKE is set, the item will be taken instead
[ if ] ALL_PLAYERS is set, all players will get the item
[ if ] REGISTER is set the item will be given upon spawning (max 15 items)
[ if ] TAKE and REGSITER are both set the item will be unregistred
[ if ] USE is set the Player will instantly use the item (switch to weapon)

"uservar1-10" item model or item name

:::WEAPONS::::::::
attrex-rifle
batleth
burstrifle
compressionrifle
drull-staff
enterprise
fieldassaultrifle
grenadelauncher
IMod
phaser
phaser-STX
photon
rom-datapad
rom-radgun
sniperrifle
tetryon
tricorder
tricorder-STX
*/

CLASS_DECLARATION( Trigger , TriggerCoopPlayerItem , "trigger_coop_player_item" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopPlayerItem::Activate } ,
	{ NULL, NULL }
};
TriggerCoopPlayerItem::TriggerCoopPlayerItem()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopPlayerItem::Activate( Event *ev )
{
#define TAKE ( 1<<0 )
#define ALL_PLAYERS ( 1<<1 )
#define REGISTER ( 1<<2 )
#define USE ( 1<<3 )

#define COOP_MAX_WEAPON_REGISTER 15
#define MAX_USERVARS 10

	int short validItems = 0;

	int iPlayer;
	Player* currentPlayer;
	Vector vector;

	str sLastValid = "";

	ScriptVariable *entityVar = NULL;
	str sItem = "";

	Entity *ent = (Entity*)this;

	//set activator
	Entity *other;
	other = ev->GetEntity( 1 );

	if ( !other )
		activator = world;
	else
		activator = other;

	int short itemNum;
	for ( itemNum = 1; itemNum <= MAX_USERVARS; itemNum++ )
	{
		bool bContinue = false;

		sItem = "";
		entityVar = ent->entityVars.GetVariable( va("uservar%d",itemNum) );
				
		if ( entityVar )
		{
			sItem = entityVar->stringValue();
					
			if ( sItem.length() )
			{
				if ( upgStrings.containsAt( sItem , ".tik" ) == -1 )
				{
					sItem = va("models/weapons/worldmodel-%s.tik", sItem.c_str() );
				}

				validItems++;

				//give to all
				if ( spawnflags & ALL_PLAYERS )
				{
					for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
					{
						currentPlayer = multiplayerManager.getPlayer( iPlayer );

						if ( !currentPlayer || multiplayerManager.isPlayerSpectator( currentPlayer ) || currentPlayer->health <= 0 )
							continue;

						if ( spawnflags & TAKE )
						{
							currentPlayer->takeItem( sItem.c_str() );
						}
						else
						{
							currentPlayer->giveItem( sItem.c_str() , 1 );
							sLastValid = sItem;
						}
					}
				}
				//give to activator
				else
				{
					if ( activator && activator->isSubclassOf( Player ) )
					{
						currentPlayer = ( Player* )( Entity * )activator;
						if ( currentPlayer && !multiplayerManager.isPlayerSpectator( currentPlayer ) && currentPlayer->health > 0 )
						{
							if ( spawnflags & TAKE )
							{
								currentPlayer->takeItem( sItem.c_str() );
							}
							else
							{
								currentPlayer->giveItem( sItem.c_str() , 1 );
								sLastValid = sItem;
							}
						}
					}
					else
					{
						if ( activator == world )
						{
							gi.Printf( "TriggerCoopPlayerItem::Activate - Activator is WORLD, trigger might have been called from script or console!\n" );
						}
						else
						{
							gi.Printf( "TriggerCoopPlayerItem::Activate - Activator is not of class Player!\n" );
						}
					}
				}

				//REGISTER
				if ( spawnflags & REGISTER )
				{
					str sCurrentRegistred = "";
					int short iRegistered;

					//check for item in current register
					for ( iRegistered = 1; iRegistered <= COOP_MAX_WEAPON_REGISTER; iRegistered++ )
					{
						sCurrentRegistred = program.getStringVariableValue( va( "coop_string_weapon%i" , iRegistered ) );

						if ( stricmp( sCurrentRegistred.c_str() , sItem.c_str() ) == 0 )
						{
							if ( spawnflags & TAKE )
							{
								//remove item from register
								program.setStringVariableValue( va( "coop_string_weapon%i" , iRegistered ), "" );
							}
							else
							{
								gi.Printf( "TriggerCoopPlayerItem::Activate - Item already registred (%s)\n" , sCurrentRegistred.c_str() );
							}

							bContinue = true;
							break;
						}
					}

					//if we are to skip this item (already registred or unregistred)
					if ( bContinue )
						continue;

					//register item on a free spot
					bool bRegisterSuccsess = false;
					for ( iRegistered = 1; iRegistered <= COOP_MAX_WEAPON_REGISTER; iRegistered++ )
					{
						sCurrentRegistred = program.getStringVariableValue( va( "coop_string_weapon%i" , iRegistered ) );

						if ( sCurrentRegistred.length() < 1 )
						{
							bRegisterSuccsess = true;
							program.setStringVariableValue( va( "coop_string_weapon%i" , iRegistered ), sItem.c_str() );
							break;
						}
					}

					//let the mapper know that regsitering failed
					if ( !bRegisterSuccsess )
					{
						gi.Printf( "TriggerCoopPlayerItem::Activate - Register is full, could not register (%s)!\n", sItem.c_str() );
						return;
					}
				}
			}
		}
	}

	//let the mapper know that all items are empty
	if ( validItems == 0 )
	{
		gi.Printf( "TriggerCoopPlayerItem::Activate - No item set in any uservar!\n" );
		return;
	}
	else if ( spawnflags & USE )
	{
		//use all
		if ( spawnflags & ALL_PLAYERS )
		{
			if ( spawnflags & TAKE )
			{
				return;
			}

			for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
			{
				currentPlayer = multiplayerManager.getPlayer( iPlayer );

				if ( !currentPlayer || multiplayerManager.isPlayerSpectator( currentPlayer ) || currentPlayer->health <= 0 )
					continue;

				Event *newEvent2 = new Event( EV_Player_UseItem );
				newEvent2->AddString( sLastValid.c_str() );
				currentPlayer->ProcessEvent( newEvent2 );
			}
		}
		//use activator
		else if ( activator != world && activator->isSubclassOf(Player))
		{
			Event *newEvent2 = new Event( EV_Player_UseItem );
			newEvent2->AddString( sLastValid.c_str() );
			activator->ProcessEvent( newEvent2 );
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## PLAYER MESSAGE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_player_message(1 1 0) (-8 -8 -8) (8 8 8) ALL_PLAYERS CENTER_PRINT

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Sends a message to player(s)

[ default ] sends the message to the activating player
[ if ] ALL_PLAYERS is set, the message will be shown to all valid players
[ if ] CENTER_PRINT is set, the message will also be printed to the center of the screen

"uservar1" specifies the maximum distance the players can have to the trigger to see the message
"uservar2" specifies the minimum distance the players need to have to the trigger to see the message
"message" the text to show to player(s)
*/

CLASS_DECLARATION( Trigger , TriggerCoopPlayerMessage , "trigger_coop_player_message" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopPlayerMessage::Activate } ,
	{ NULL, NULL }
};
TriggerCoopPlayerMessage::TriggerCoopPlayerMessage()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopPlayerMessage::Activate( Event *ev )
{
#define ALL_PLAYERS ( 1<<0 )
#define CENTER_PRINT ( 1<<1 )

	if ( !message.length() )
	{
		gi.Printf( "TriggerCoopPlayerMessage::Activate - No Message set on trigger (%s) !\n" , targetname.c_str() );
		return;
	}

	float fMinDist = 0.0f;
	float fMaxDist = 9999999.0f;

	ScriptVariable *entityVar1 = entityVars.GetVariable( "uservar1" );
	ScriptVariable *entityVar2 = entityVars.GetVariable( "uservar2" );

	if ( entityVar2 )
		fMinDist = entityVar2->floatValue();

	if ( entityVar1 )
		fMaxDist = entityVar1->floatValue();

	if ( fMinDist > fMinDist )
	{
		gi.Printf( "TriggerCoopPlayerMessage::Activate - Min-distance in uservar2 was bigger than max-distance in uservar1 !\n" );
		fMinDist = 0.0f;
	}

	if ( spawnflags & ALL_PLAYERS )
	{
		int iPlayer;
		Player* currentPlayer;
		Vector vector;
	
		for ( iPlayer = 0; iPlayer < maxclients->integer; iPlayer++ )
		{
			currentPlayer = multiplayerManager.getPlayer( iPlayer );

			if ( !currentPlayer )
				continue;

			float fVectorLength = Distance(currentPlayer->origin, origin); //[b60011] chrissstrahl - changed how the distance is calculated

			if ( fVectorLength <= fMaxDist && fVectorLength >= fMinDist )
			{
				currentPlayer->hudPrint( va( "%s\n" , message.c_str() ) );
				if ( CENTER_PRINT )
				{
					multiplayerManager.centerPrint( currentPlayer->entnum , va( "%s\n" , message.c_str() ) , CENTERPRINT_IMPORTANCE_CRITICAL );
				}
			}
		}
	}
	else
	{
		Player* player;
		player = ( Player* )( Entity * )activator;
		
		float fVectorLength = Distance(player->origin, origin); //[b60011] chrissstrahl - changed how the dicstance is calculated

		if ( fVectorLength <= fMaxDist && fVectorLength >= fMinDist )
		{
			player->hudPrint( va( "%s\n" , message.c_str() ) );
			if ( CENTER_PRINT )
			{
				multiplayerManager.centerPrint( player->entnum , va( "%s\n" , message.c_str() ) , CENTERPRINT_IMPORTANCE_CRITICAL );
			}
		}
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}

/* ############################## MESSAGE CONSOLE ############################ */
/*****************************************************************************/
/*QUAKED trigger_coop_console_message(1 1 0) (-8 -8 -8) (8 8 8) WARNING

[ ! ] This fixed size trigger cannot be touched, it can only be fired by other events or triggers.

[ info ] Prints a message to the game console used for server log

[ if ] WARNING is set the message will only be printed if com_printwarnings is active

"message" the text to print to console
*/

CLASS_DECLARATION( Trigger , TriggerCoopConsoleMessage , "trigger_coop_console_message" )
{
	{ &EV_Touch , NULL } ,
	{ &EV_Activate, &TriggerCoopConsoleMessage::Activate } ,
	{ NULL, NULL }
};
TriggerCoopConsoleMessage::TriggerCoopConsoleMessage()
{
	setSolidType( SOLID_NOT );
}
void TriggerCoopConsoleMessage::Activate( Event *ev )
{
#define WARNING (1<<0)

	if ( !message.length() )
	{
		gi.Printf( "TriggerCoopConsoleMessage::Activate - No Message set on trigger (%s) !\n" , targetname.c_str() );
		return;
	}

	if ( spawnflags & WARNING )
	{
		gi.WDPrintf( "%s\n", message.c_str() );
	}
	else
	{
		gi.Printf( "%s\n" , message.c_str() );
	}

	//remove this trigger if it has cnt set	and hits the limit
	RemoveSelfOnCountHit();
}
