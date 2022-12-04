//-----------------------------------------------------------------------------
//
//  $Logfile:: /EF2/Code/DLLs/game/globalcmd.cpp                              $
// $Revision:: 76                                                             $
//     $Date:: 9/26/03 2:36p                                                  $
//
// Copyright (C) 1999 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
// 
//
// DESCRIPTION:
// Global commands for scripts
//

#include "_pch_cpp.h"
#include "globalcmd.h"
#include "scriptmaster.h"
#include "player.h"
#include "actor.h"
#include <qcommon/gameplaymanager.h>
#include "CinematicArmature.h"
#include "mp_manager.hpp"
#include "mp_modeBase.hpp"

#include "vector.h"
#include "interpreter.h"
#include "coopParser.hpp"
#include "coopReturn.hpp"
#include "coopCheck.hpp"
#include "coopScripting.hpp"

Event EV_ProcessCommands
( 
	"processCommands",
	EV_CODEONLY,
	NULL,
	NULL,
	"Not used."
);
Event EV_Script_NewOrders
(	
	"newOrders",
	EV_CODEONLY,
	NULL,
	NULL,
	"Inform script that it is about to get new orders."
);
Event EV_ScriptThread_Execute
( 
	"execute",
	EV_CODEONLY,
	NULL,
	NULL,
	"Execute the thread."
);
Event EV_ScriptThread_Callback
( 
	"script_callback",
	EV_CODEONLY,
	"ese",
	"slave name other",
	"Script callback."
);
Event EV_ScriptThread_ThreadCallback
( 
	"thread_callback",
	EV_CODEONLY,
	NULL,
	NULL,
	"Thread callback."
);
Event EV_ScriptThread_DeathCallback
( 
	"death_callback",
	EV_CODEONLY,
	NULL,
	NULL,
	"Death callback."
);
Event EV_ScriptThread_CreateThread
( 
	"thread",
	EV_SCRIPTONLY,
	"@is",
	"threadnumber label",
	"Creates a thread starting at label."
);
Event EV_ScriptThread_CreateThread2
( 
	"runThreadNamed",
	EV_SCRIPTONLY,
	"@is",
	"threadnumber label",
	"Creates a thread starting at label."
);
Event EV_ScriptThread_TerminateThread
( 
	"terminate",
	EV_SCRIPTONLY,
	"i",
	"thread_number",
	"Terminates the specified thread or the current one if none specified."
);
Event EV_ScriptThread_ControlObject
( 
	"control",
	EV_CODEONLY,
	"e",
	"object",
	"Not used."
);
Event EV_ScriptThread_Goto
( 
	"goto",
	EV_SCRIPTONLY,
	"s",
	"label",
	"Goes to the specified label."
);
Event EV_ScriptThread_Pause
( 
	"pause",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Pauses the thread."
);
Event EV_ScriptThread_Wait
( 
	"wait",
	EV_SCRIPTONLY,
	"f",
	"wait_time",
	"Wait for the specified amount of time."
);
Event EV_ScriptThread_WaitFor
( 
	"waitfor",
	EV_SCRIPTONLY,
	"e",
	"ent",
	"Wait for the specified entity."
);
Event EV_ScriptThread_WaitForThread
( 
	"waitForThread",
	EV_SCRIPTONLY,
	"i",
	"thread_number",
	"Wait for the specified thread."
);
Event EV_ScriptThread_WaitForDeath
( 
	"waitForDeath",
	EV_SCRIPTONLY,
	"s",
	"name",
	"Wait for death."
);
Event EV_ScriptThread_WaitForSound
( 
	"waitForSound",
	EV_SCRIPTONLY,
	"sf",
	"sound_name delay",
	"Wait for end of the named sound plus an optional delay."
);
Event EV_ScriptThread_WaitForDialog
( 
	"waitForDialog",
	EV_SCRIPTONLY,
	"eF",
	"actor additional_delay",
	"Wait for end of the dialog for the specified actor.\n"
	"If additional_delay is specified, than the actor will\n"
	"wait the dialog length plus the delay."
);
Event EV_ScriptThread_WaitDialogLength
( 
	"waitDialogLength",
	EV_SCRIPTONLY,
	"s",
	"dialogName",
	"Wait for the length of time of the dialog.\n"
);
Event EV_ScriptThread_WaitForAnimation
( 
	"waitForAnimation",
	EV_SCRIPTONLY,
	"esF",
	"entity animName additionalDelay",
	"Waits for time of the animation length plus the additional delayt specified.\n"
);
Event EV_ScriptThread_WaitForPlayer
( 
	"waitForPlayer",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Wait for player to be ready."
);
Event EV_ScriptThread_CPrint
(
	"cprint",
	EV_SCRIPTONLY,
	"s",
	"string",
	"Prints a string."
);
Event EV_ScriptThread_Print
(
	"print",
	EV_SCRIPTONLY,
	"sSSSSSSSSSSSSSSS",
	"string string string string string string string string string string string string string string string string",
	"Prints a string."
);
Event EV_ScriptThread_PrintInt
( 
	"printint",
	EV_SCRIPTONLY,
	"i",
	"integer",
	"Print integer."
);
Event EV_ScriptThread_PrintFloat
( 
	"printfloat",
	EV_SCRIPTONLY,
	"f",
	"float",
	"Prints a float."
);
Event EV_ScriptThread_PrintVector
( 
	"printvector",
	EV_SCRIPTONLY,
	"v",
	"vector",
	"Prints a vector."
);
Event EV_ScriptThread_NewLine
( 
	"newline",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Prints a newline."
);
Event EV_ScriptThread_Assert
( 
	"assert",
	EV_SCRIPTONLY,
	"f",
	"value",
	"Assert if value is 0."
);
Event EV_ScriptThread_Break
( 
	"break",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Asserts so that we can break back into the debugger from script."
);
Event EV_ScriptThread_Clear
( 
	"clear",
	EV_SCRIPTONLY,
	"s",
	"var_group",
	"Clears the specified var group."
);
Event EV_ScriptThread_Trigger
( 
	"trigger",
	EV_SCRIPTONLY,
	"s",
	"entname",
	"Trigger the specified target."
);
Event EV_ScriptThread_TriggerEntity
( 
	"triggerentity",
	EV_SCRIPTONLY,
	"e",
	"entityToTrigger",
	"Trigger the specified entity."
);
Event EV_ScriptThread_Spawn
( 
	"spawn", 
	EV_CHEAT | EV_SCRIPTONLY,
	"@esSSSSSSSS",
	"entity entityname keyname1 value1 keyname2 value2 keyname3 value3 keyname4 value4",
	"Spawn the specified entity."
);
Event EV_ScriptThread_Map
( 
	"map",
	EV_SCRIPTONLY,
	"s",
	"map_name",
	"Starts the specified map."
);
Event EV_ScriptThread_NoIntermission
( 
	"noIntermission",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Makes it so no intermission happens at the end of this level."
);
Event EV_ScriptThread_DontSaveOrientation
( 
	"dontSaveOrientation",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Makes it so we don't save the player's orientation across this sublevel."
);
Event EV_ScriptThread_PlayerDeathThread
( 
	"playerDeathThread",
	EV_SCRIPTONLY,
	"s",
	"threadName",
	"Sets a thread to run when the player dies."
);
Event EV_ScriptThread_EndPlayerDeathThread
( 
	"endPlayerDeathThread",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Ends the player death thread (goes on to mission failed)."
);
Event EV_ScriptThread_GetCvar
(
	"getcvar",
	EV_SCRIPTONLY,
	"@ss",
	"string_value cvar_name",
	"Returns the string value of the specified cvar."
);
Event EV_ScriptThread_GetCvarFloat
(
	"getcvarfloat",
	EV_SCRIPTONLY,
	"@fs",
	"float_value cvar_name",
	"Returns the float value of the specified cvar."
);
Event EV_ScriptThread_GetCvarInt
(
	"getcvarint",
	EV_SCRIPTONLY,
	"@fs",
	"int_value cvar_name",
	"Returns the integer value of the specified cvar."
);
Event EV_ScriptThread_SetCvar
( 
	"setcvar",
	EV_SCRIPTONLY,
	"ss",
	"cvar_name value",
	"Sets the value of the specified cvar."
);
Event EV_ScriptThread_CueCamera
( 
	"cuecamera",
	EV_SCRIPTONLY,
	"eF",
	"entity switchTime",
	"Cue the camera. If switchTime is specified, then the camera\n"
	"will switch over that length of time."
);
Event EV_ScriptThread_CuePlayer
( 
	"cueplayer",
	EV_SCRIPTONLY,
	"F",
	"switchTime",
	"Go back to the normal camera. If switchTime is specified,\n"
	"then the camera will switch over that length of time."
);
Event EV_ScriptThread_FreezePlayer
( 
	"freezeplayer",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Freeze the player."
);
Event EV_ScriptThread_ReleasePlayer
( 
	"releaseplayer",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Release the player."
);
Event EV_ScriptThread_FakePlayer
( 
	"fakeplayer",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Will create a fake version of the player, hide the real one and\n"
	"call the fake one 'fakeplayer'."
);
Event EV_ScriptThread_RemoveFakePlayer
( 
	"removefakeplayer",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Will delete the fake version of the player, un-hide the real one and\n"
	"return to the normal game"
);
Event EV_ScriptThread_KillEnt
( 
	"killent", 
	EV_CHEAT | EV_SCRIPTONLY,
	"i",
	"ent_num",
	"Kill the specified entity."
);
Event EV_ScriptThread_KillClass
( 
	"killclass", 
	EV_CHEAT | EV_SCRIPTONLY,
	"sI",
	"class_name except",
	"Kills everything in the specified class except for the specified entity (optional)."
);
Event EV_ScriptThread_RemoveEnt
( 
	"removeent", 
	EV_CHEAT | EV_SCRIPTONLY,
	"i",
	"ent_num",
	"Removes the specified entity."
);
Event EV_ScriptThread_RemoveClass
( 
	"removeclass", 
	EV_CHEAT | EV_SCRIPTONLY,
	"sI",
	"class_name except",
	"Removes everything in the specified class except for the specified entity (optional)."
);
Event EV_ScriptThread_RemoveActorsNamed
(
	"removeActorsNamed",
	EV_CHEAT | EV_SCRIPTONLY,
	"s",
	"class_name",
	"remove all actors in the game that have a matching name field (NOT targetname)"
);

// client/server flow control
Event EV_ScriptThread_StuffCommand
( 
	"stuffcmd",
	EV_SCRIPTONLY,
	"SSSSSS",
	"arg1 arg2 arg3 arg4 arg5 arg6",
	"Server only command."
);

//
// world stuff
//
Event EV_ScriptThread_RegisterAlias
( 
	"alias",
	EV_DEFAULT,
	"ssSSSS",
	"alias_name real_name arg1 arg2 arg3 arg4",
	"Sets up an alias."
);
Event EV_ScriptThread_RegisterAliasAndCache
(
	"aliascache",
	EV_DEFAULT,
	"ssSSSS",
	"alias_name real_name arg1 arg2 arg3 arg4",
	"Sets up an alias and caches the resourse."
);
Event EV_ScriptThread_SetCinematic
( 
	"cinematic",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Turns on cinematic."
);
Event EV_ScriptThread_SetNonCinematic
( 
	"noncinematic",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Turns off cinematic."
);
Event EV_ScriptThread_SetLevelAI
( 
	"level_ai",
	EV_SCRIPTONLY,
	"b",
	"on_or_off",
	"Turns on or off Level-wide AI for monsters."
);
Event EV_ScriptThread_SetSkipThread
( 
	"skipthread",
	EV_SCRIPTONLY,
	"s",
	"thread_name",
	"Set the thread to skip."
);

// Precache specific
Event EV_ScriptThread_Precache_Cache
( 
	"cache",
	EV_DEFAULT,
	"s",
	"resource_name",
	"Cache the specified resource."
);

// drugged up warping FOV controls
Event EV_ScriptThread_DrugView
( 
	"drugview",
	EV_SCRIPTONLY,
	"b",
	"on_or_off",
	"1 to turn FOV warping on, 0 to turn off."
);

// fades for movies
Event EV_ScriptThread_FadeIn
( 
	"cam_fadein",
	EV_SCRIPTONLY,
	"fffffI",
	"time color_red color_green color_blue alpha mode",
	"Sets up fadein in values."
);
Event EV_ScriptThread_FadeOut
( 
	"cam_fadeout",
	EV_SCRIPTONLY,
	"fffffI",
	"time color_red color_green color_blue alpha mode",
	"Sets up fadeout values."
);
Event EV_ScriptThread_FadeIsActive
(
	"fadeisactive",
	EV_SCRIPTONLY,
	"@i",
	"Result",
	"Returns 1 if a fade is in progress."
);
Event EV_ScriptThread_DoesEntityExist
(
	"doesEntityExist",
	EV_SCRIPTONLY,
	"@fe",
	"return_0_or_1 entity",
	"Returns whether or not an entity exists."
);
Event EV_ScriptThread_GetEntity
( 
	"getentity",
	EV_SCRIPTONLY,
	"@es",
	"entity entityname",
	"Returns the entity with the specified targetname."
);
Event EV_ScriptThread_GetNextEntity
( 
	"getnextentity",
	EV_SCRIPTONLY,
	"@ee",
	"entity currententity",
	"Returns the next entity in the list with the same name as the current entity."
);

// music command
Event EV_ScriptThread_MusicEvent
( 
	"music",
	EV_SCRIPTONLY,
	"sS",
	"current fallback",
	"Sets the current and fallback (optional) music moods."
);
Event EV_ScriptThread_ForceMusicEvent
( 
	"forcemusic",
	EV_SCRIPTONLY,
	"sS",
	"current fallback",
	"Forces the current and fallback (optional) music moods."
);
Event EV_ScriptThread_MusicVolumeEvent
( 
	"musicvolume",
	EV_SCRIPTONLY,
	"ff",
	"volume fade_time",
	"Sets the volume and fade time of the music."
);
Event EV_ScriptThread_RestoreMusicVolumeEvent
( 
	"restoremusicvolume",
	EV_SCRIPTONLY,
	"f",
	"fade_time",
	"Restores the music volume to its previous value."
);
Event EV_ScriptThread_AllowMusicDucking
( 
	"allowMusicDucking",
	EV_SCRIPTONLY,
	"B",
	"boolean",
	"Sets whether or not music ducking is allowed."
);
Event EV_ScriptThread_AllowActionMusic
( 
	"allowActionMusic",
	EV_SCRIPTONLY,
	"B",
	"boolean",
	"Sets whether or not action music is allowed or not."
);
Event EV_ScriptThread_SoundtrackEvent
( 
	"soundtrack",
	EV_SCRIPTONLY,
	"s",
	"soundtrack_name",
	"Changes the soundtrack."
);
Event EV_ScriptThread_RestoreSoundtrackEvent
( 
	"restoresoundtrack",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Restores the soundtrack to the previous one."
);
Event EV_ScriptThread_ClearFade
(
	"clearfade",
	EV_SCRIPTONLY,
	NULL,
	NULL,
	"Clear the fade from the screen"
);
Event EV_ScriptThread_Letterbox
( 
	"letterbox",
	EV_SCRIPTONLY,
	"f",
	"time",
	"Puts the game in letterbox mode."
);
Event EV_ScriptThread_ClearLetterbox
( 
	"clearletterbox",
	EV_SCRIPTONLY,
	"f",
	"time",
	"Clears letterbox mode."
);
Event EV_ScriptThread_SetDialogScript
( 
	"setdialogscript" ,
	EV_SCRIPTONLY,
	"s",
	"dialog_script",
	"Set the script to be used when dialog:: is used"
);
Event EV_ScriptThread_CosDegrees
( 
	"cosdegrees",
	EV_SCRIPTONLY,
	"@ff",
	"cosine degrees",
	"Returns the cosine of the angle specified (in degrees)."
);
Event EV_ScriptThread_SinDegrees
( 
	"sindegrees",
	EV_SCRIPTONLY,
	"@ff",
	"sine degrees",
	"Returns the sine of the angle specified (in degrees)."
);
Event EV_ScriptThread_CosRadians
( 
	"cosradians",
	EV_SCRIPTONLY,
	"@ff",
	"cosine radians",
	"Returns the cosine of the angle specified (in radians)."
);
Event EV_ScriptThread_SinRadians
( 
	"sinradians",
	EV_SCRIPTONLY,
	"@ff",
	"sine radians",
	"Returns the sine of the angle specified (in radians)."
);
Event EV_ScriptThread_ArcTanDegrees
(
	"arctandegrees",
	EV_SCRIPTONLY,
	"@fff",
	"arctangent y x",
	"Returns the angle (in degrees) described by a 2d vector pointing to (x,y)."
);
Event EV_ScriptThread_Sqrt
(
	"sqrt",
	EV_SCRIPTONLY,
	"@ff",
	"square_root x",
	"Returns the sign-preserving square root of x."
);
Event EV_ScriptThread_Log
(
	"log",
	EV_SCRIPTONLY,
	"@ff",
	"log x",
	"Returns the log (base 2)."
);
Event EV_ScriptThread_Exp
(
	"exp",
	EV_SCRIPTONLY,
	"@ff",
	"exp x",
	"Returns the exponential."
);
Event EV_ScriptThread_RandomFloat
( 
	"randomfloat",
	EV_SCRIPTONLY,
	"@ff",
	"returnval max_float",
	"Sets the float to a random number."
);
Event EV_ScriptThread_RandomInteger
( 
	"randomint",
	EV_SCRIPTONLY,
	"@ii",
	"returnval max_int",
	"Sets the integer to a random number."
);
Event EV_ScriptThread_CameraCommand
( 
	"cam",
	EV_SCRIPTONLY,
	"sSSSSSS",
	"command arg1 arg2 arg3 arg4 arg5 arg6",
	"Processes a camera command."
);
Event EV_ScriptThread_SetLightStyle
( 
	"setlightstyle" ,
	EV_SCRIPTONLY,
	"is",
	"lightstyleindex lightstyledata",
	"Set up the lightstyle with lightstyleindex to the specified data"
);
Event EV_ScriptThread_KillThreadEvent
( 
	"killthread",
	EV_SCRIPTONLY,
	"s",
	"threadName",
	"kills all threads starting with the specified name"
);
Event EV_ScriptThread_SetThreadNameEvent
( 
	"threadname",
	EV_SCRIPTONLY,
	"s",
	"threadName",
	"sets the name of the thread"
);
Event EV_ScriptThread_CenterPrint
(
	"centerprint",
	EV_SCRIPTONLY,
	"s",
	"stuffToPrint",
	"prints the included message in the middle of all player's screens"
);
Event EV_ScriptThread_IsActorDead
(
	"isactordead",
	EV_SCRIPTONLY,
	"s",
	"actor_name",
	"Checks if an Actor is dead or not"
);
Event EV_ScriptThread_IsActorAlive
(
	"isactoralive",
	EV_SCRIPTONLY,
	"@fs",
	"flag actor_name",
	"Checks if an Actor is alive or not"
);
Event EV_ScriptThread_SendClientCommand
(
	"SendClientCommand",
	EV_SCRIPTONLY,
	"es",
	"player command",
	"Sends a command across to the client"
);
Event EV_ScriptThread_GetNumFreeReliableServerCommands
(
	"GetNumFreeReliableServerCommands",
	EV_SCRIPTONLY,
	"@fe",
	"count player",
	"Returns the number of reliable server command slots that are still available for use"
);
Event EV_ScriptThread_SendClientVar
(
	"SendClientVar",
	EV_SCRIPTONLY,
	"es",
	"player varName",
	"Sends a variable across to the client"
);
Event EV_ScriptThread_GetCurrentEntity
(
	"getcurrententity",
	EV_SCRIPTONLY,
	"@e",
	"entity",
	"Retrieves the current entity"
);
Event EV_ScriptThread_Mod
(
	"mod",
	EV_SCRIPTONLY,
	"@fii",
	"integer_remainder nominator denominator",
	"Returns the integer remainder from nominator / denominator"
);
Event EV_ScriptThread_Div
(
	"div",
	EV_SCRIPTONLY,
	"@fii",
	"integer_result nominator denominator",
	"Returns the integer value from nominator / denominator"
);
Event EV_ScriptThread_VectorScale
(
	"vectorscale",
	EV_SCRIPTONLY,
	"@vvf",
	"scaled_vector vector scale_value",
	"Returns the vector scaled by scale"
);
Event EV_ScriptThread_VectorDot
(
	"vectordot",
	EV_SCRIPTONLY,
	"@fvv",
	"dot_product vector1 vector2",
	"Returns the dot product of vector1 and vector2."
);
Event EV_ScriptThread_VectorCross
(
	"vectorcross",
	EV_SCRIPTONLY,
	"@vvv",
	"cross_vector vector1 vector2",
	"Returns the cross product of vector1 and vector2."
);
Event EV_ScriptThread_VectorNormalize
(
	"vectornormalize",
	EV_SCRIPTONLY,
	"@vv",
	"normalized_vector vector",
	"Returns the normalized vector."
);
Event EV_ScriptThread_VectorLength
(
	"vectorlength",
	EV_SCRIPTONLY,
	"@fv",
	"length vector",
	"Returns the length of the vector."
);
Event EV_ScriptThread_VectorForward
(
	"vectorforward",
	EV_SCRIPTONLY,
	"@vv",
	"forward_vector vector",
	"Returns the forward vector of the vector."
);
Event EV_ScriptThread_VectorLeft
(
	"vectorleft",
	EV_SCRIPTONLY,
	"@vv",
	"left_vector vector",
	"Returns the left vector of the vector."
);
Event EV_ScriptThread_VectorUp
(
	"vectorup",
	EV_SCRIPTONLY,
	"@vv",
	"up_vector vector",
	"Returns the up vector of the vector."
);
Event EV_ScriptThread_VectorGetX
(
	"vectorgetx",
	EV_SCRIPTONLY,
	"@fv",
	"x_value vector",
	"Returns the x of the vector."
);
Event EV_ScriptThread_VectorGetY
(
	"vectorgety",
	EV_SCRIPTONLY,
	"@fv",
	"y_value vector",
	"Returns the y of the vector."
);
Event EV_ScriptThread_VectorGetZ
(
	"vectorgetz",
	EV_SCRIPTONLY,
	"@fv",
	"z_value vector",
	"Returns the z of the vector."
);
Event EV_ScriptThread_VectorSetX
(
	"vectorsetx",
	EV_SCRIPTONLY,
	"@vvf",
	"vector_result vector x_value",
	"Sets the x value of a vector"
);
Event EV_ScriptThread_VectorSetY
(
	"vectorsety",
	EV_SCRIPTONLY,
	"@vvf",
	"vector_result vector y_value",
	"Sets the y value of a vector"
);
Event EV_ScriptThread_VectorSetZ
(
	"vectorsetz",
	EV_SCRIPTONLY,
	"@vvf",
	"vector_result vector z_value",
	"Sets the z value of a vector"
);
Event EV_ScriptThread_VectorToString
(
	"vectorToString",
	EV_SCRIPTONLY,
	"@sv",
	"string_result vector",
	"Converts a vector to a string"
);
Event EV_ScriptThread_TargetOf
(
	"targetof",
	EV_SCRIPTONLY,
	"@se",
	"name entity",
	"Gets the name of the entity's target"
);
Event EV_ScriptThread_Floor
(
	"floor",
	EV_SCRIPTONLY,
	"@ff",
	"return_value float_value",
	"Returns the closest integer not larger than float."
);
Event EV_ScriptThread_Ceil
(
	"ceil",
	EV_SCRIPTONLY,
	"@ff",
	"return_value float_value",
	"Returns the closest integer not smaller than float."
);
Event EV_ScriptThread_Round
(
	"round",
	EV_SCRIPTONLY,
	"@ff",
	"return_value float_value",
	"Returns the rounded value of a float."
);
Event EV_ScriptThread_SetGameplayFloat
(
	"setgameplayfloat",
	EV_SCRIPTONLY,
	"ssf",
	"objname propname value",
	"Sets a property float value on an object"
);
Event EV_ScriptThread_GetGameplayString
(
	"getgameplaystring",
	EV_DEFAULT,
	"@sss",
	"stringvalue objname propname",
	"Returns the string value of the object property."
);
Event EV_ScriptThread_GetGameplayFloat
(
	"getgameplayfloat",
	EV_DEFAULT,
	"@fss",
	"floatvalue objname propname",
	"Returns the float value of the object property."
);
Event EV_ScriptThread_SetGameplayString
(
	"setgameplaystring",
	EV_SCRIPTONLY,
	"sss",
	"objname propname valuestr",
	"Sets a property string value on an object"
);
Event EV_ScriptThread_GetIntegerFromString
(
	"getintfromstring",
	EV_SCRIPTONLY,
	"@fs",
	"returned_int string",
	"Returns the integer found in a string.  Can be called with strings "
	"that contain text, such as enemy12."
);
Event EV_ScriptThread_CreateCinematic
(
	"createCinematic",
	EV_SCRIPTONLY,
	"@es",
	"returnedCinematic cinematicName",
	"Creates a cinematic from disk. Returns the created cinematic."
	"Each call to this function creates a new cinematic, even if a"
	"cinematic with this name already exists."
);
Event EV_ScriptThread_GetTime
(
	"getLevelTime",
	EV_SCRIPTONLY,
	"@f",
	"returnedFloat",
	"Returns how many seconds.milliseconds the current level has been running."
);
Event EV_ScriptThread_ConnectPathnodes
(
	"connectPathnodes",
	EV_SCRIPTONLY,
	"ss",
	"node1 node2",
	"Connects the 2 specified nodes."
);
Event EV_ScriptThread_DisconnectPathnodes
(
	"disconnectPathnodes",
	EV_SCRIPTONLY,
	"ss",
	"node1 node2",
	"Disconnects the 2 specified nodes."
);

//[b607] chrissstrahl configstrings stuff issue
Event EV_ScriptThread_ConfigstringRemove
(
	"configstringRemove",
	EV_SCRIPTONLY,
	"s",
	"sString",
	"Removes given string from the configstrings"
);

//[b607] chrissstrahl configstrings stuff issue
Event EV_ScriptThread_ConfigstringRemoveCombatSounds
(
	"configstringRemoveCombatSounds",
	EV_SCRIPTONLY,
	"s",
	"sActorname",
	"Removes associated predefined & precached combat dialog/sounds for the named actor"
);

//hzm coop mod chrissstrahl - add new script functionality
Event EV_ScriptThread_getStringFromStringWithLengt
(
	"getStringFromStringWithLengt" ,
	EV_SCRIPTONLY ,
	"@ssff" ,
	"retunedString string float float" ,
	"Returns a string Starting from given starts at 0 with given length"
);
Event EV_ScriptThread_getStringToLower
(
	"getStringToLower" ,
	EV_SCRIPTONLY ,
	"@ss" ,
	"retunedString string" ,
	"Returns given string in lowercase."
);
Event EV_ScriptThread_getStringToUpper
(
	"getStringToUpper" ,
	EV_SCRIPTONLY ,
	"@ss" ,
	"retunedString string" ,
	"Returns given string in uppercase."
);
Event EV_ScriptThread_getIntStringLength
(
	"getIntStringLength" ,
	EV_SCRIPTONLY ,
	"@fs" ,
	"retunedFloat string" ,
	"Returns length of given string."
);
Event EV_ScriptThread_getIntStringFind
(
	"getIntStringFind" ,
	EV_SCRIPTONLY ,
	"@fss" ,
	"retunedFloat sourceString findString" ,
	"Returns position at wich the given string was found otherwise, if not found return -1"
);
Event EV_ScriptThread_getVectorScriptVariable
(
	"getVectorScriptVariable" ,
	EV_SCRIPTONLY ,
	"@vs" ,
	"retunedVector string" ,
	"Returns level script code (vector) variable by given name"
);
Event EV_ScriptThread_getFloatScriptVariable
(
	"getFloatScriptVariable" ,
	EV_SCRIPTONLY ,
	"@fs" ,
	"retunedFloat string" ,
	"Returns level script code (float) variable by given name"
);
Event EV_ScriptThread_getStringScriptVariable
(
	"getStringScriptVariable" ,
	EV_SCRIPTONLY ,
	"@ss" ,
	"retunedString string" ,
	"Returns level script code (string) variable by given name"
);
Event EV_ScriptThread_setVectorScriptVariable
(
	"setVectorScriptVariable" ,
	EV_SCRIPTONLY ,
	"sv" ,
	"variablename vector" ,
	"Sets vector on global script vector variable located by given name"
);
Event EV_ScriptThread_setFloatScriptVariable
(
	"setFloatScriptVariable" ,
	EV_SCRIPTONLY ,
	"sf" ,
	"variablename float" ,
	"Sets float on global script float variable located by given name"
);
Event EV_ScriptThread_setStringScriptVariable
(
	"setStringScriptVariable" , //hzm gamefix chrissstrahl - fixed spelling issue of script command
	EV_SCRIPTONLY ,
	"ss" ,
	"variablename string" ,
	"Sets string on global script string variable located by given name"
);
Event EV_ScriptThread_getLevelParamaterValue
(
	"getLevelParamaterValue" ,
	EV_SCRIPTONLY ,
	"@ss" ,
	"returnString string" ,
	"Returns the value of the paramater, if it is attached to the mapname via $"
);
Event EV_ScriptThread_getFloatFromString
(
	"getFloatFromString" ,
	EV_SCRIPTONLY ,
	"@fs" ,
	"returnFloat string" ,
	"Returns the given value as float, if possible"
);
Event EV_ScriptThread_isDigit
(
	"isDigit" ,
	EV_SCRIPTONLY ,
	"@fs" ,
	"returnFloat string" ,
	"Returns if given char is a digit"
);
Event EV_ScriptThread_MissionFailed
(
	"missionfailed" ,
	EV_SCRIPTONLY ,
	"S" ,
	"reason" ,
	"Displays the mission failed screen on the client side"
);
//[b60011] chrissstrahl
Event EV_ScriptThread_getMapByServerIp
(
	"getMapByServerIp",
	EV_SCRIPTONLY,
	"",
	"",
	"Sends Server IP data to HaZardModding Website to detect a missing map - works only on windows client host"
);
//[b60011] chrissstrahl
Event EV_ScriptThread_hasDpiException
(
	"hasDpiException",
	EV_SCRIPTONLY,
	"@i",
	"returnInt",
	"Checks if a dpi scale exception for currently running game exe is set in registry for user - works only on windows client host"
);
//[b60011] chrissstrahl
Event EV_ScriptThread_setDpiException
(
	"setDpiException",
	EV_SCRIPTONLY,
	"@i",
	"returnInt",
	"Sets a dpi scale exception for currently running game exe in registry for user - works only on windows client host"
);
//[b60011] chrissstrahl
Event EV_ScriptThread_isDpiScaled
(
	"getDpiScale",
	EV_SCRIPTONLY,
	"@i",
	"returnInt",
	"Returns dpi scale value of the system from registry of user - works only on windows client host"
);
//[b60011] chrissstrahl
Event EV_ScriptThread_getScreenWidth
(
	"getScreenWidth",
	EV_SCRIPTONLY,
	"@i",
	"returnWidth",
	"Returns current screen res WIDTH - works only on windows client host"
);

//[b60011] chrissstrahl
Event EV_ScriptThread_getScreenHeight
(
	"getScreenHeight",
	EV_SCRIPTONLY,
	"@i",
	"returnHeight",
	"Returns current screen res HEIGHT - works only on windows client host"
);

Event EV_ScriptThread_setIniDataPlayer
(
	"setIniDataPlayer",
	EV_SCRIPTONLY,
	"@iesss",
	"returnsuccsess player category keyname value",
	"sets data for player from current map ini or given inifile"
);
Event EV_ScriptThread_getIniDataPlayer
(
	"getIniDataPlayer",
	EV_SCRIPTONLY,
	"@sesSS",
	"returndatastring player category keyname filename",
	"gets data for player from current map ini or given inifile"
);
Event EV_ScriptThread_getIniData
(
	"getIniData",
	EV_SCRIPTONLY,
	"@sssS",
	"returndatastring category keyname filename",
	"gets data from current map ini or given inifile"
);
Event EV_ScriptThread_setIniData
(
	"setIniData",
	EV_SCRIPTONLY,
	"@fsss",
	"returndbool category keyname value",
	"sets data to map-specific ini file"
);
//[b60011] chrissstrahl
Event EV_ScriptThread_getVectorFromString
(
	"getVectorFromString",
	EV_SCRIPTONLY,
	"@vs",
	"returnVector string",
	"Returns the given value as vector, if possible"
);
Event EV_ScriptThread_challengesDisabled
(
	"challengesDisabled",
	EV_SCRIPTONLY,
	"@fF",
	"returnIsDisabled SetDisabledOrNot",
	"Returns current status, if parameter1 given 0=enables/1=disables challenge on current level"
);
Event EV_ScriptThread_challengeDisabledNamed
(
	"challengeDisabledNamed",
	EV_SCRIPTONLY,
	"@fsF",
	"returnIsDisabled name SetDisabledOrNot",
	"Returns current status of named challenge, if parameter2 given 0=enables/1=disables challenge on current level"
);
//end of hzm

CLASS_DECLARATION( Interpreter, CThread, NULL )
{
	{ &EV_ScriptThread_Execute,						&CThread::ExecuteFunc },
	{ &EV_MoveDone,									&CThread::ObjectMoveDone },
	{ &EV_ScriptThread_Callback,					&CThread::ScriptCallback },
	{ &EV_ScriptThread_ThreadCallback,				&CThread::ThreadCallback },
	{ &EV_ScriptThread_DeathCallback,				&CThread::DeathCallback },
	{ &EV_ScriptThread_CreateThread,				&CThread::CreateThread },
	{ &EV_ScriptThread_CreateThread2,				&CThread::CreateThread },
	{ &EV_ScriptThread_TerminateThread,				&CThread::TerminateThread },
	{ &EV_ScriptThread_Pause,						&CThread::EventPause },
	{ &EV_ScriptThread_Wait,						&CThread::EventWait },
	{ &EV_ScriptThread_WaitFor,						&CThread::EventWaitFor },
	{ &EV_ScriptThread_WaitForThread,				&CThread::EventWaitForThread },
	{ &EV_ScriptThread_WaitForDeath,				&CThread::EventWaitForDeath },
	{ &EV_ScriptThread_WaitForSound,				&CThread::EventWaitForSound },
	{ &EV_ScriptThread_WaitForDialog,				&CThread::EventWaitForDialog },
	{ &EV_ScriptThread_WaitDialogLength,			&CThread::EventWaitDialogLength },
	{ &EV_ScriptThread_WaitForAnimation,			&CThread::EventWaitForAnimation },
	{ &EV_ScriptThread_WaitForPlayer,				&CThread::EventWaitForPlayer },
	{ &EV_ScriptThread_CPrint,						&CThread::CPrint },
	{ &EV_ScriptThread_Print,						&CThread::Print },
	{ &EV_ScriptThread_PrintInt,					&CThread::PrintInt },
	{ &EV_ScriptThread_PrintFloat,					&CThread::PrintFloat },
	{ &EV_ScriptThread_PrintVector,					&CThread::PrintVector },
	{ &EV_ScriptThread_NewLine,						&CThread::NewLine },
	{ &EV_ScriptThread_Assert,						&CThread::Assert },
	{ &EV_ScriptThread_Break,						&CThread::Break },
	{ &EV_ScriptThread_Trigger,						&CThread::TriggerEvent },
	{ &EV_ScriptThread_TriggerEntity,				&CThread::TriggerEntityEvent },
	{ &EV_ScriptThread_StuffCommand,				&CThread::StuffCommand },
	{ &EV_ScriptThread_Spawn,						&CThread::Spawn },
	{ &EV_ScriptThread_Precache_Cache,				&CThread::CacheResourceEvent },
	{ &EV_ScriptThread_RegisterAlias,				&CThread::RegisterAlias },
	{ &EV_ScriptThread_RegisterAliasAndCache,		&CThread::RegisterAliasAndCache },
	{ &EV_ScriptThread_Map,							&CThread::MapEvent },
	{ &EV_ScriptThread_NoIntermission,				&CThread::noIntermission },
	{ &EV_ScriptThread_DontSaveOrientation,			&CThread::dontSaveOrientation },

	{ &EV_ScriptThread_PlayerDeathThread,			&CThread::setPlayerDeathThread },
	{ &EV_ScriptThread_EndPlayerDeathThread,		&CThread::endPlayerDeathThread },

	{ &EV_ScriptThread_GetCvar,						&CThread::GetCvarEvent },
	{ &EV_ScriptThread_GetCvarFloat,				&CThread::GetCvarFloatEvent },
	{ &EV_ScriptThread_GetCvarInt,					&CThread::GetCvarIntEvent },
	{ &EV_ScriptThread_SetCvar,						&CThread::SetCvarEvent },
	{ &EV_ScriptThread_CueCamera,					&CThread::CueCamera },
	{ &EV_ScriptThread_CuePlayer,					&CThread::CuePlayer },
	{ &EV_ScriptThread_FreezePlayer,				&CThread::FreezePlayer },
	{ &EV_ScriptThread_ReleasePlayer,				&CThread::ReleasePlayer },
	{ &EV_ScriptThread_FadeIn,						&CThread::FadeIn },
	{ &EV_ScriptThread_FadeOut,						&CThread::FadeOut },
	{ &EV_ScriptThread_FadeIsActive,				&CThread::FadeIsActive },
	{ &EV_ScriptThread_ClearFade,					&CThread::ClearFade },
	{ &EV_ScriptThread_Letterbox,					&CThread::Letterbox },
	{ &EV_ScriptThread_ClearLetterbox,				&CThread::ClearLetterbox },
	{ &EV_ScriptThread_MusicEvent,					&CThread::MusicEvent },
	{ &EV_ScriptThread_ForceMusicEvent,				&CThread::ForceMusicEvent },
	{ &EV_ScriptThread_MusicVolumeEvent,			&CThread::MusicVolumeEvent },
	{ &EV_ScriptThread_RestoreMusicVolumeEvent,		&CThread::RestoreMusicVolumeEvent },
	{ &EV_ScriptThread_AllowMusicDucking,			&CThread::allowMusicDucking },
	{ &EV_ScriptThread_AllowActionMusic,			&CThread::allowActionMusic },
	{ &EV_ScriptThread_SoundtrackEvent,				&CThread::SoundtrackEvent },
	{ &EV_ScriptThread_RestoreSoundtrackEvent,		&CThread::RestoreSoundtrackEvent },
	{ &EV_ScriptThread_SetCinematic,				&CThread::SetCinematic },
	{ &EV_ScriptThread_SetNonCinematic,				&CThread::SetNonCinematic },
	{ &EV_ScriptThread_SetLevelAI,					&CThread::SetLevelAI },
	{ &EV_ScriptThread_SetSkipThread,				&CThread::SetSkipThread },
	{ &EV_ScriptThread_KillEnt,						&CThread::KillEnt },
	{ &EV_ScriptThread_RemoveEnt,					&CThread::RemoveEnt },
	{ &EV_ScriptThread_KillClass,					&CThread::KillClass },
	{ &EV_ScriptThread_RemoveClass,					&CThread::RemoveClass },
	{ &EV_ScriptThread_RemoveActorsNamed,			&CThread::RemoveActorsNamed },
	{ &EV_ScriptThread_FakePlayer,					&CThread::FakePlayer },
	{ &EV_ScriptThread_RemoveFakePlayer,			&CThread::RemoveFakePlayer },
	{ &EV_AI_RecalcPaths,							&CThread::PassToPathmanager },
	{ &EV_AI_CalcPath,								&CThread::PassToPathmanager },
	{ &EV_ScriptThread_DoesEntityExist,				&CThread::doesEntityExist },
	{ &EV_ScriptThread_GetEntity,					&CThread::GetEntityEvent },
	{ &EV_ScriptThread_GetNextEntity,				&CThread::GetNextEntityEvent },
	{ &EV_ScriptThread_CosDegrees,					&CThread::CosDegrees },
	{ &EV_ScriptThread_SinDegrees,					&CThread::SinDegrees },
	{ &EV_ScriptThread_CosRadians,					&CThread::CosRadians },
	{ &EV_ScriptThread_SinRadians,					&CThread::SinRadians },
	{ &EV_ScriptThread_ArcTanDegrees,				&CThread::ArcTanDegrees },
	{ &EV_ScriptThread_Sqrt,						&CThread::ScriptSqrt },
	{ &EV_ScriptThread_Log,							&CThread::ScriptLog },
	{ &EV_ScriptThread_Exp,							&CThread::ScriptExp },
	{ &EV_ScriptThread_RandomFloat,					&CThread::RandomFloat },
	{ &EV_ScriptThread_RandomInteger,				&CThread::RandomInteger },
	{ &EV_ScriptThread_CameraCommand,				&CThread::CameraCommand },
	{ &EV_ScriptThread_SetLightStyle,				&CThread::SetLightStyle },
	{ &EV_ScriptThread_KillThreadEvent,				&CThread::KillThreadEvent },
	{ &EV_ScriptThread_SetThreadNameEvent,			&CThread::SetThreadName },

	{ &EV_SetFloatVar,								&CThread::SetFloatVar },
	{ &EV_SetVectorVar,								&CThread::SetVectorVar },
	{ &EV_SetStringVar,								&CThread::SetStringVar },

	{ &EV_DoesVarExist,								&CThread::doesVarExist },
	{ &EV_RemoveVariable,							&CThread::RemoveVariable },

	{ &EV_GetFloatVar,								&CThread::GetFloatVar },
	{ &EV_GetVectorVar,								&CThread::GetVectorVar },
	{ &EV_GetStringVar,								&CThread::GetStringVar },

	{ &EV_ScriptThread_CenterPrint,					&CThread::CenterPrint },
	{ &EV_ScriptThread_IsActorDead,					&CThread::isActorDead		},
	{ &EV_ScriptThread_IsActorAlive,				&CThread::isActorAlive		},
	{ &EV_ScriptThread_SendClientCommand,			&CThread::SendClientCommand		},
	{ &EV_ScriptThread_GetNumFreeReliableServerCommands, &CThread::GetNumFreeReliableServerCommands },
	{ &EV_ScriptThread_SendClientVar,				&CThread::SendClientVar		},
	{ &EV_ScriptThread_GetCurrentEntity,			&CThread::GetCurrentEntity     },
	{ &EV_ScriptThread_Mod,							&CThread::ModEvent },
	{ &EV_ScriptThread_Div,							&CThread::DivEvent },
	{ &EV_ScriptThread_VectorScale,					&CThread::VectorScaleEvent },
	{ &EV_ScriptThread_VectorDot,					&CThread::VectorDotEvent },
	{ &EV_ScriptThread_VectorCross,					&CThread::VectorCrossEvent },
	{ &EV_ScriptThread_VectorNormalize,				&CThread::VectorNormalizeEvent },
	{ &EV_ScriptThread_VectorLength,				&CThread::VectorLengthEvent },
	{ &EV_ScriptThread_VectorSetX,					&CThread::VectorSetXEvent },
	{ &EV_ScriptThread_VectorSetY,					&CThread::VectorSetYEvent },
	{ &EV_ScriptThread_VectorSetZ,					&CThread::VectorSetZEvent },
	{ &EV_ScriptThread_VectorGetX,					&CThread::VectorGetXEvent },
	{ &EV_ScriptThread_VectorGetY,					&CThread::VectorGetYEvent },
	{ &EV_ScriptThread_VectorGetZ,					&CThread::VectorGetZEvent },
	{ &EV_ScriptThread_VectorForward,				&CThread::VectorForwardEvent },
	{ &EV_ScriptThread_VectorLeft,					&CThread::VectorLeftEvent },
	{ &EV_ScriptThread_VectorUp,					&CThread::VectorUpEvent },
	{ &EV_ScriptThread_VectorToString,				&CThread::vectorToString },
	{ &EV_ScriptThread_TargetOf,					&CThread::TargetOfEvent },
	{ &EV_ScriptThread_Floor,						&CThread::FloorEvent },
	{ &EV_ScriptThread_Ceil,						&CThread::CeilEvent },
	{ &EV_ScriptThread_Round,						&CThread::RoundEvent },
	{ &EV_ScriptThread_GetGameplayFloat,			&CThread::GetGameplayFloat },
	{ &EV_ScriptThread_GetGameplayString,			&CThread::GetGameplayString },
	{ &EV_ScriptThread_SetGameplayFloat,			&CThread::SetGameplayFloat },
	{ &EV_ScriptThread_SetGameplayString,			&CThread::SetGameplayString },
	{ &EV_ScriptThread_GetIntegerFromString,		&CThread::GetIntegerFromString },
	{ &EV_ScriptThread_CreateCinematic,				&CThread::CreateCinematic },
	{ &EV_ScriptThread_GetTime,						&CThread::GetLevelTime },

	{ &EV_ScriptThread_ConnectPathnodes,			&CThread::connectPathnodes },
	{ &EV_ScriptThread_DisconnectPathnodes,			&CThread::disconnectPathnodes },

	//[b60011] chrissstral - allow to retrive real screen resolution
	{ &EV_ScriptThread_getMapByServerIp,			&CThread::getMapByServerIp },
	{ &EV_ScriptThread_hasDpiException,				&CThread::hasDpiException },
	{ &EV_ScriptThread_setDpiException,				&CThread::setDpiException },
	{ &EV_ScriptThread_isDpiScaled,					&CThread::getDpiScale },
	{ &EV_ScriptThread_getScreenWidth,				&CThread::getScreenWidth },
	{ &EV_ScriptThread_getScreenHeight,				&CThread::getScreenHeight },
	//[b60011] chrissstral - allow read write to map specific ini files
	{ &EV_ScriptThread_setIniDataPlayer,			&CThread::setIniDataPlayer },
	{ &EV_ScriptThread_getIniDataPlayer,			&CThread::getIniDataPlayer },
	{ &EV_ScriptThread_getIniData,					&CThread::getIniData },
	{ &EV_ScriptThread_setIniData,					&CThread::setIniData },
	//[b60011] chrissstrahl
	{ &EV_ScriptThread_getVectorFromString,			&CThread::getVectorFromString } ,
	{ &EV_ScriptThread_challengesDisabled,			&CThread::challengesDisabled } ,
	{ &EV_ScriptThread_challengeDisabledNamed,		&CThread::challengeDisabledNamed } ,
	//[b607] chrissstrahl - remove combatsounds for named actor, to save configstrings in multiplayer
	{ &EV_ScriptThread_ConfigstringRemove, &CThread::configstringRemove },
	{ &EV_ScriptThread_ConfigstringRemoveCombatSounds, &CThread::configstringRemoveCombatSounds },
	//hzm coop mod chrissstrahl try to add a new script function
	{ &EV_ScriptThread_getStringFromStringWithLengt, &CThread::getStringFromStringWithLengt } ,
	{ &EV_ScriptThread_getStringToLower, &CThread::getStringToLower } ,
	{ &EV_ScriptThread_getStringToUpper, &CThread::getStringToUpper } ,
	{ &EV_ScriptThread_getIntStringLength, &CThread::getIntStringLength } ,
	{ &EV_ScriptThread_getIntStringFind, &CThread::getIntStringFind } ,
	{ &EV_ScriptThread_getVectorScriptVariable, &CThread::getVectorScriptVariable } ,
	{ &EV_ScriptThread_getFloatScriptVariable, &CThread::getFloatScriptVariable } ,
	{ &EV_ScriptThread_getStringScriptVariable, &CThread::getStringScriptVariable } ,		
	{ &EV_ScriptThread_setVectorScriptVariable, &CThread::setVectorScriptVariable } ,
	{ &EV_ScriptThread_setFloatScriptVariable, &CThread::setFloatScriptVariable } ,
	{ &EV_ScriptThread_setStringScriptVariable, &CThread::setStringScriptVariable } ,
	{ &EV_ScriptThread_getLevelParamaterValue, &CThread::getLevelParamaterValue } ,
	{ &EV_ScriptThread_getFloatFromString, &CThread::getFloatFromString } ,
	{ &EV_ScriptThread_isDigit, &CThread::isDigit } ,
	{ &EV_ScriptThread_MissionFailed, &CThread::missionFailed } ,
//end of hzm
	{ NULL, NULL }
};

/*
//[b60011] chrissstrahl - sets status of named achivment for a player
//returns: void
//takes: player, achivmentname, value
void CThread::setAchivment(Event* ev)
{

}
//player unlocked a achivment
void CThread::achivment(Event* ev)
{

}

//check achivment for player
void CThread::checkAchivment(Event* ev)
{

}
*/

#ifdef WIN32
	#include "wtypes.h"
	#include <cstdio>
	#include <tlhelp32.h>
	#include <tchar.h>

	void GetDesktopSize(int& vertical, int& horizontal);
	bool isProcessRunning(const char* processName);
	void winShellExecuteOpen(str sResource, str sType);
	str winGetRegSzValue(HKEY hKey, str sPath, str sKey);
	bool winSetRegSzValue(HKEY hKey, str sPath, str sKey, str sValue);
	str winGetRunningExeName();

	void GetDesktopSize(int &vertical, int &horizontal)
	{
		/*
		horizontal = GetSystemMetrics(SM_CXSCREEN);
		vertical = GetSystemMetrics(SM_CYSCREEN);
		*/

		RECT desktop;
		// Get a handle to the desktop window
		const HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);
		// The top left corner will have coordinates (0,0)
		// and the bottom right corner will have coordinates
		// (horizontal, vertical)
		horizontal	= desktop.right;
		vertical	= desktop.bottom;
		gi.Printf(va("GetDesktopSize detected: %d x %d\n", horizontal, vertical));
	}
	
	bool isProcessRunning(const char* processName)
	//https://stackoverflow.com/questions/1591342/c-how-to-determine-if-a-windows-process-is-running
	{
		bool exists = false;
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry)) {
			while (Process32Next(snapshot, &entry)) {
				//if (!_wcsicmp((wchar_t*), (wchar_t*))) {
				//gi.Printf(va("comparing: '%s' to '%s'\n", entry.szExeFile, processName));
				if (!_stricmp(entry.szExeFile, processName)) {
					//gi.Printf(va("isProcessRunning detected: %s == %s\n",entry.szExeFile, processName));
					exists = true;
				}
			}
		}

		CloseHandle(snapshot);
		return exists;
	}

	void winShellExecuteOpen(str sResource)
	{
		//we might want some kind of security check here, allowing only hzm and other sites and only base folder
		str sHzm = "hazardmodding.com";
		str sRes = coop_returnCvarString("fs_basepath").c_str();
		str sForbiddenFileTypes[]{ "com","exe","bat","sh","ink","link","pif","reg","vbs" };

		//if not a web address



		if (coop_find(sResource,"http://") == -1 && coop_find(sResource, "https://") == -1) {
			//disallow certain filetypes
			int iArrayLength = sizeof(sForbiddenFileTypes) / sizeof(str);
			
			int i;
			for (i = 0; i < iArrayLength;i++) {
				if(strcmpi(coop_returnStringFileExtensionOnly(sResource).c_str(),va(".%s",sForbiddenFileTypes[i].c_str())) == 0) {
					return;
				}
			}

			if (coop_contains(sResource,".\\") != -1 || coop_contains(sResource, "./") != -1 || coop_contains(sResource, "%") != -1) {
				return;
			}

			sResource = va("%s\\%s",sRes.c_str(), sResource.c_str());
			gi.Printf(va("File: %s\n", sResource.c_str()));
		}
		else {
			gi.Printf(va("Domain Name: %s\n", coop_returnStringDomainname(sResource.c_str()).c_str()));
		}
		ShellExecuteA(NULL, "open", sResource.c_str(), NULL, NULL, SW_SHOWDEFAULT);
		//CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)
	}

	/*
	void winSetDpiException()
	//this creates a registry file 
	{
		static fileHandle_t regFile = NULL;

		//make sure no file is open from last time
		if (regFile) {
			throw("Regfile a file was already open\n");
			return;
		}

		str sFull;
		str sPath = coop_returnCvarString("fs_basepath") + "\\";
		str sFile = "EF2_ADD_DPI_Exceptions.reg";
		regFile = gi.FS_FOpenFileWrite(sFile.c_str());

		//make sure we can write file
		if (!regFile) {
			//[b60011] chrissstrahl - throw error every time, to prevent the server not working right unnoticed
			throw(va("Regfile coud not open to write: \\base\\%s - Write-protection? Bad-Accsess-rights?\n", sFile.c_str()));
			return;
		}

		str sRegPath = "";
		int i;
		for (i = 0; i < sPath.length(); i++) {
			if (sPath[i] == '\\') {
				sRegPath += '\\';
				sRegPath += '\\';
			}
			else{
				sRegPath += sPath[i];
			}
		}

		str buffer = "Windows Registry Editor Version 5.00\r\n\r\n";
		buffer += "[HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers]\r\n";
		buffer += va("\"%sef2.exe\"=\"~ HIGHDPIAWARE\"\r\n", sRegPath.c_str());
		buffer += va("\"%squake3.exe\"=\"~ HIGHDPIAWARE\"\r\n\r\n", sRegPath.c_str());
		
		//if (buffer[buffer.length() - 1] != '\n')
			//buffer += '\n';

		if (gi.FS_Write(buffer, buffer.length(), regFile) == 0) {
			//[b60011] chrissstrahl - throw error every time, to prevent the server not working right unnoticed
			throw(va("Regfile coud not write data to file: \\base\\%s - Write-protection? Bad-Accsess-rights?\n", sPath.c_str()));
		}

		//close file
		gi.FS_Flush(regFile);
		gi.FS_FCloseFile(regFile);
		regFile = NULL;

		//execute registry file
		sFull = va("%s\\base\\%s",sPath.c_str(), sFile.c_str());
		ShellExecuteA(NULL, "open", sFull.c_str(), NULL, NULL, SW_SHOWDEFAULT);
	}
	*/

	str winGetRegSzValue(HKEY hKey, str sPath, str sKey)
	{
		char value[1024] = { 0 };
		DWORD lengh = sizeof(value);
		if (RegOpenKeyEx(hKey, TEXT(sPath + '\0'), 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
		{
			RegQueryValueExA(hKey, sKey + '\0', NULL, NULL, LPBYTE(value), &lengh);
			RegCloseKey(hKey);

			gi.Printf("\n-------\nRegistry Key read from:\n\\\%s\n%s\nwith result: '%s'\n-------\n\n", sPath.c_str(), sKey.c_str(), value);
		}
		return va("%s", value);
	}

	bool winSetRegSzValue(HKEY hKey, str sPath, str sKey,str sValue)
	{
		if (RegOpenKeyEx(hKey, TEXT(sPath), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			LPCTSTR data = TEXT(sValue+'\0');
			if (RegSetValueEx(hKey, (LPCTSTR)sKey + '\0', NULL, REG_SZ, (LPBYTE)data, _tcslen(data) * sizeof(TCHAR)) == ERROR_SUCCESS) {
				RegCloseKey(hKey);
				gi.Printf("\n-------\nRegistry Key written to:\n\\\%s\n%s\nwith data: '%s'\n-------\n\n", sPath.c_str(), sKey.c_str(), sValue.c_str());
				return true;
			}
		}
		return false;
	}

	str winGetRunningExeName()
	//obviously this will work right if you have the game running with different named exe files in paralell
	{
		str sExeName = "";
		int			numdirs;
		char		filename[128];
		char		dirlist[1024];
		char* dirptr;
		int			i;
		int			dirlen;
		numdirs = gi.FS_GetFileList("../", ".exe", dirlist, 1024);
		dirptr = dirlist;
		for (i = 0; i < numdirs; i++, dirptr += dirlen + 1) {
			dirlen = strlen(dirptr);
			strcpy(filename, "");
			strcat(filename, dirptr);

			//files to skip, uninstaller,seperate game-server
			if (strcmp("unins000.exe", filename) == 0 ||
				strcmp("server.exe", filename) == 0)
			{
				continue;
			}
			if (isProcessRunning(filename)) {
				//gi.Printf(va("Detected game exe: %s\n", filename));
				return filename;
			}
		}
		return sExeName;
	}
#endif

//[b60011] chrissstrahl - set dpi scaling exception for runn ing exe
void CThread::getMapByServerIp(Event* ev)
{
#ifdef WIN32
	str sServerIp = coop_returnCvarString("cl_currentServerAddress");
	if (sServerIp == "") {
		gi.Printf("getMapByServerIp - connect first to a server until you get disconnected with a error message\n");
		return;
	}

	str sServerIpEncoded = "";
	int i;
	for (i = 0; i < sServerIp.length();i++) {
		/*if (sServerIp[i] == '.') {
			sServerIpEncoded += "D";
		}*/
			
		if (sServerIp[i] == ':') {
			sServerIpEncoded += "P";
		}
		else {
			sServerIpEncoded += sServerIp[i];
		}
	}
	if (coop_returnCvarString("username") == "Chrissstrahl") {
		winShellExecuteOpen(va("http://localhost/gameq/?data=%s&t=%i", sServerIpEncoded.c_str(), level.time));
	}
	else {
		winShellExecuteOpen(va("http://findmap.hazardmodding.com?data=%s&t=%i", sServerIpEncoded.c_str(), level.time));
	}
	winShellExecuteOpen("base");
#else
	gi.Printf("getMapByServerIp - works only on windows when a mp map is loaded\n");
#endif
}

//[b60011] chrissstrahl - check dpi scaling exception for running exe
void CThread::hasDpiException(Event* ev)
{
#ifdef WIN32
	str sExeName = winGetRunningExeName();
	if (sExeName.length()) {
		str sPath = coop_returnCvarString("fs_basepath");
		sPath += "\\";
		sPath += sExeName;
		str sValue = winGetRegSzValue(HKEY_CURRENT_USER,"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", sPath);
	
		//has a exception
		if (sValue[0] == '~' && coop_contains(sValue, "HIGHDPIAWARE") != -1) {
			ev->ReturnFloat(1.0f);
			return;
		}
	}
#else
	gi.Printf("setDpiException - works only on windows when a mp map is loaded\n");
#endif
	ev->ReturnFloat(0.0f);
}

//[b60011] chrissstrahl - set dpi scaling exception for running exe
void CThread::setDpiException(Event* ev)
{
#ifdef WIN32
	str sExeName = winGetRunningExeName();
	if (sExeName.length()) {
		str sKey = coop_returnCvarString("fs_basepath");
		sKey += "\\";
		sKey += sExeName;
		str sPath = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers";
		str sValue = "~ HIGHDPIAWARE";
		winSetRegSzValue(HKEY_CURRENT_USER, sPath, sKey, sValue);
		ev->ReturnFloat(1.0f);
		return;
	}
#else
	gi.Printf("setDpiException - works only on windows when a mp map is loaded\n");
#endif
	ev->ReturnFloat(0.0f);
}

//[b60011] chrissstrahl - get dpi scaling
void CThread::getDpiScale(Event* ev)
{
	int iScale = -1;
#ifdef WIN32
	if (dedicated->integer == 0 && g_gametype->integer == GT_MULTIPLAYER) {
		DWORD val;
		DWORD dataSize = sizeof(val);
		str sPath = "Control Panel\\Desktop\\WindowMetrics";
		str sKey = "AppliedDPI";
		if (ERROR_SUCCESS == RegGetValueA(HKEY_CURRENT_USER, sPath+'\0', sKey+'\0', RRF_RT_DWORD, nullptr /*type not required*/, &val, &dataSize)) {
			switch (val)
			{
			case 96:
				iScale = 100;
				break;
			case 120:
				iScale = 125;
				break;
			case 144:
				iScale = 150;
				break;
			case 192:
				iScale = 200;
				break;
			default:
				iScale = -2;
			}
			gi.Printf("\n-------\nRegistry Key read from:\n\\\%s\n%s\nwith result: '%i'\n-------\n\n", sPath.c_str(),sKey.c_str(), iScale);
		}
		// no CloseKey needed because it is a predefined registry key
	}
#else
	gi.Printf("isDpiScaled - works only on windows when a mp map is loaded\n");
#endif
	ev->ReturnFloat(iScale);
}

//[b60011] chrissstrahl - get screen width
void CThread::getScreenWidth(Event* ev)
{
#ifdef WIN32
	int iVal = -1;
	if (dedicated->integer == 0 && g_gametype->integer == GT_MULTIPLAYER) {
		//make sure we get the correct size, not the virual scaled size
		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		int iVert;
		int iHorz;
		GetDesktopSize(iVert, iHorz);
		iVal = iHorz;
	}
	ev->ReturnFloat(iVal);
#else
	gi.Printf("getScreenWidth - works only on windows when a mp map is loaded\n");
	ev->ReturnInteger(-1);
#endif
}

//[b60011] chrissstrahl - get screen height
void CThread::getScreenHeight(Event* ev)
{
#ifdef WIN32
	int iVal = -1;
	if (dedicated->integer == 0 && g_gametype->integer == GT_MULTIPLAYER) {
		//make sure we get the correct size, not the virual scaled size
		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		int iVert;
		int iHorz;
		GetDesktopSize(iVert, iHorz);
		iVal = iVert;
	}
	ev->ReturnFloat(iVal);
#else
	gi.Printf("getScreenHeight - works only on windows when a mp map is loaded\n");
	ev->ReturnInteger(-1);
#endif
}

//[b60011] chrissstrahl - add command allow reading player specific data from ini
str CThread::getIniData(str sFilename,str sKeyname,str sCategoryname)
{
	if (!sFilename.length()) {
		sFilename = va("ini/%s.ini", level.mapname.tolower());
	}

	//prevent certain ini files to be accsessed
	//do not allow reading/writing specific files
	const char* forBiddenFiles[1] = {
		"serverdata.ini",
		//"deathlist.ini",
		//"maplist.ini",
		//"vote_maplist.ini"
	};
	int arrSize = sizeof(forBiddenFiles) / sizeof(forBiddenFiles[0]);
	for (int i = 0; i < arrSize; i++) {
		if (coop_returnIntFind(forBiddenFiles[i], sFilename.c_str()) != -1) {
			G_ExitWithError(va("getIniData/getIniDataPlayer - Accsess Violation on reading file: %s\n", sFilename.c_str()));
		}
	}
	str sValue;
	sValue = coop_parserIniGet(sFilename, sKeyname, sCategoryname);
	return sValue;
}

//[b60011] chrissstrahl - add command allow reading player specific data from ini
//returns: string ini data
//takes: categoryname, player, key, filename
void CThread::getIniDataPlayer(Event* ev)
{
	Entity* entity;
	str sFilename;
	str sKeyname;
	str sCategoryname;
	str sValue;
	str sPlayerId;
	sCategoryname = ev->GetString(2);

	entity = ev->GetEntity(1);

	if (!entity) {
		str sEmpty = "";
		gi.Printf("getIniDataPlayer - Given entity does not exist\n");
		ev->ReturnString(sEmpty);
		return;
	}

	if (!entity->isSubclassOf(Player)) {
		str sEmpty = "";
		gi.Printf("getIniDataPlayer - Given entity was not a player\n");
		ev->ReturnString(sEmpty);
		return;
	}

	Player* player;
	player = (Player*)entity;
	sPlayerId = player->coopPlayer.coopId;

	if (ev->NumArgs() > 2) {
		str sKey;
		sKey = ev->GetString(3);
		sKeyname = (va("%s%s", sPlayerId.c_str(), sKey.c_str()));
	}

	if(!sKeyname.length()) {
		sKeyname = sPlayerId;
	}

	if (ev->NumArgs() > 3) {
		sFilename = ev->GetString(4);
	}
	else {
		sFilename = va("ini/%s.ini", level.mapname.tolower());
	}

	sValue = getIniData(sFilename, sKeyname, sCategoryname);
	ev->ReturnString(sValue.c_str());
}

//[b60011] chrissstrahl - add command allow reading player specific data from ini
//returns: string ini data
//takes: categoryname, player, key, filename
void CThread::setIniDataPlayer(Event* ev)
{
	bool bScuccsess = false;
	Entity* entity;
	str sFilename = va("ini/%s.ini", level.mapname.tolower());
	str sKeyname;
	str sCategoryname;
	str sValue;
	str sPlayerId;
	sCategoryname = ev->GetString(2);

	entity = ev->GetEntity(1);

	if (!entity) {
		str sEmpty = "";
		gi.Printf("setIniDataPlayer - Given entity does not exist\n");
		ev->ReturnInteger(0);
		return;
	}

	if (!entity->isSubclassOf(Player)) {
		str sEmpty = "";
		gi.Printf("setIniDataPlayer - Given entity was not a player\n");
		ev->ReturnInteger(0);
		return;
	}

	Player* player;
	player = (Player*)entity;
	sPlayerId = player->coopPlayer.coopId;

	if (ev->NumArgs() > 2) {
		str sKey;
		sKey = ev->GetString(3);
		sKeyname = (va("%s%s", sPlayerId.c_str(), sKey.c_str()));
	}

	if (!sKeyname.length()) {
		sKeyname = sPlayerId;
	}

	if (ev->NumArgs() < 3) {
		ev->ReturnString("!Parameter-Error!");
		return;
	}

	sValue = ev->GetString(4);

	bScuccsess = coop_parserIniSet(sFilename, sKeyname, sValue, sCategoryname);
	ev->ReturnFloat(float(bScuccsess));
	//gi.Printf(va("setIniDataPlayer::%s at %s - %s\n", sFilename.c_str(), sCategoryname.c_str(), sKeyname.c_str()));
}

	//[b60011] chrissstrahl - add command allow reading and writing to ini file
//returns: string ini data
//takes: categoryname, keyname, filename
void CThread::getIniData(Event *ev)
{
	if (ev->NumArgs() < 2) {
		ev->ReturnString("!Parameter-Error!");
		return;
	}
	str sFilename;
	str sKeyname;
	str sCategoryname;
	str sValue;
	sCategoryname		= ev->GetString(1);
	sKeyname			= ev->GetString(2);

	if (ev->NumArgs() > 2) {
		sFilename = ev->GetString(3);
	}
	else {
		sFilename = va("ini/%s.ini",level.mapname.tolower());
	}

	sValue = getIniData(sFilename,sKeyname,sCategoryname);
	ev->ReturnString(sValue.c_str());
}

//takes: categoryname, keyname, value
void CThread::setIniData(Event *ev)
{
	if (ev->NumArgs() < 3) {
		ev->ReturnString("!Parameter-Error!");
		return;
	}
	bool bScuccsess;
	str sFilename	= va("ini/%s.ini",level.mapname.tolower());
	str sKeyname;
	str sCategoryname;
	str sValue;
	sCategoryname	= ev->GetString(1);
	sKeyname		= ev->GetString(2);
	sValue			= ev->GetString(3);

	bScuccsess = coop_parserIniSet(sFilename, sKeyname, sValue, sCategoryname);
	ev->ReturnFloat(float(bScuccsess));
	gi.Printf(va("setIniData::%s at %s - %s\n", sFilename.c_str(),sCategoryname.c_str(), sKeyname.c_str()));
}

//[b607] chrissstrahl - remove given string from configstrings
void CThread::configstringRemove(Event *ev)
{
	if (ev->NumArgs() < 1) {
		return;
	}

	str sName = ev->GetString(1);
	int iNum = coop_serverConfigstringRemove(sName);
	gi.Printf("configstringRemove(%s) removed %i items\n", sName.c_str(), iNum);
}
//[b607] chrissstrahl - remove combatsounds for named actor, to save configstrings in multiplayer
void CThread::configstringRemoveCombatSounds(Event *ev)
{
	if (ev->NumArgs() < 1) {
		return;
	}

	str sName = ev->GetString(1);
	int iNum = coop_serverConfigstringRemoveCombatSounds(sName);
	gi.Printf("configstringRemoveCombatSounds(%s) removed %i items\n",sName.c_str(),iNum);
}
//hzm coop mod chrissstrahl - add new script commands
void CThread::getStringScriptVariable( Event *ev )
{
	str s = coop_getStringScriptVariable( ev->GetString(1) );
	ev->ReturnString( s.c_str() );
}
void CThread::getVectorScriptVariable( Event *ev )
{
	Vector v;
	v = coop_getVectorScriptVariable( ev->GetString( 1 ) );
	ev->ReturnVector( v );
}
void CThread::getFloatScriptVariable( Event *ev )
{
	float f;
	f = coop_getFloatScriptVariable( ev->GetString( 1 ) );
	ev->ReturnFloat( f );
}
void CThread::getStringFromStringWithLengt( Event *ev )
{
	str s =  ev->GetString( 1 );
	int iStart = ev->GetInteger( 2 );
	if ( iStart > s.length() ) {
		ev->ReturnString("");
		return;
	}
	int iLength = ev->GetInteger( 3 );
	coop_manipulateStringFromWithLength( s , iStart , iLength);
	ev->ReturnString( s.length() ? va("%s", s.c_str() ) : "" );
}
void CThread::getStringToLower(Event *ev)
{
	str s = ev->GetString( 1 );
	s = s.tolower();
	ev->ReturnString( s.length() ? va( "%s" , s.c_str() ) : "" );
}
void CThread::getStringToUpper( Event *ev )
{
	str s = ev->GetString( 1 );
	s = s.toupper();
	ev->ReturnString( s.length() ? va( "%s" , s.c_str() ) : "" );
}
void CThread::getIntStringLength( Event *ev )
{
	str s = ev->GetString( 1 );
	ev->ReturnFloat( s.length() );
}
void CThread::getIntStringFind( Event *ev )
{
	str sSource = ev->GetString( 1 );
	str sFind = ev->GetString( 2 );
	ev->ReturnFloat( coop_returnIntFind( sSource , sFind ) );
}
void CThread::setVectorScriptVariable( Event *ev )
{
	const char* varname = ev->GetString( 1 );
	Vector vSet = ev->GetVector( 2 );
	coop_setVectorScriptVariableValue( varname , vSet );
}
void CThread::setFloatScriptVariable( Event *ev )
{
	const char* varname = ev->GetString( 1 );
	float fSet = ev->GetFloat( 2 );
	coop_setFloatScriptVariableValue( varname , fSet );
}
void CThread::setStringScriptVariable( Event *ev )
{
	const char* varname = ev->GetString( 1 );
	const char* sSet = ev->GetString( 2 );
	coop_setStringScriptVariableValue( varname , sSet );
}
void CThread::getLevelParamaterValue( Event *ev )
{
	str varname = ev->GetString( 1 );
	cvar_t *cvar = gi.cvar_get( "mapname" );
	if ( !cvar || !varname.length() ){
		ev->ReturnString( "" );
		return;
	}

	//make sure this is handled as part of the variable name
	varname += "=";

	int iVarPos;
	str s = cvar->string;
	iVarPos = coop_returnIntFind( s , varname.tolower() );

	//not found
	if ( iVarPos < 0 ) {
		ev->ReturnString( "" );
		return;
	}

	//
	str sValue	= coop_returnStringStartingFrom( s , (iVarPos + varname.length() ));
	iVarPos		= coop_returnIntFind( sValue , "?" );
	if ( iVarPos  > 0) {
		sValue	= coop_returnStringFromWithLength( sValue , 0 , iVarPos );
	}
	ev->ReturnString( sValue.c_str() );
}

void CThread::getFloatFromString( Event *ev )
{
	ev->ReturnFloat( coop_returnFloatFromString( ev->GetString( 1 ) ) );
}
void CThread::isDigit( Event *ev )
{
	str s = ev->GetString( 1 );
	char c = s[0];
	int i = isdigit( c );
	if ( i != 0 ) {
		i = 1;
	}else {
		i = 0;
	}
	ev->ReturnFloat( i );
}
void CThread::missionFailed( Event *ev )
{
	str reason = "DefaultFailure";
	if ( ev->NumArgs() > 0 )
		reason = ev->GetString( 1 );

	G_MissionFailed( reason );
}
//end of hzm

//[b60011] chrissstrahl
void CThread::getVectorFromString(Event* ev)
{
	ev->ReturnVector(ev->GetVector(1));
}
//end












CThread::CThread()
{
	ClearWaitFor();
	currentEntity = NULL;
}

void CThread::ExecuteFunc( Event *ev )
{
	ClearWaitFor();
	
	Interpreter::Execute( ev );
}

void CThread::ClearWaitFor( void )
{
	waitUntil = 0;
	waitingFor = NULL;
	waitingNumObjects = 0;
	waitingForThread = NULL;
	waitingForDeath = "";
	waitingForPlayer = false;
}

qboolean CThread::WaitingFor( Entity *obj )
{
	assert( obj );
	
	return ( waitingFor && waitingFor->list.ObjectInList( obj ) );
}

void CThread::ObjectMoveDone( Event *ev )
{
	Entity *obj;
	
	obj = ev->GetEntity( 1 );
	assert( obj );
	
	if ( WaitingFor( obj ) )
	{
		waitingNumObjects--;
		if ( waitingNumObjects <= 0 )
		{
			ClearWaitFor();
			DelayedStart( 0.0f );
		}
	}
}

void CThread::CreateThread( Event *ev )
{
	CThread *pThread;
	
	pThread = Director.CreateThread( ev->GetToken( 1 ) );
	if ( pThread )
	{
		// start right away
		pThread->Start();
		
		ev->ReturnInteger( pThread->ThreadNum() );
	}
}

void CThread::TerminateThread( Event *ev )
{
	int threadnum;
	
	threadnum = ev->GetInteger( 1 );
	Director.GetThread( threadnum );
	Director.KillThread( threadnum );
}

void CThread::EventPause( Event *ev )
{
	DoMove();
	
	ClearWaitFor();
	
	doneProcessing = true;
}

void CThread::EventWait( Event *ev )
{
	DoMove();
	
	ClearWaitFor();
	
	waitUntil = ev->GetFloat( 1 ) + level.time;
	
	DelayedStart( ev->GetFloat( 1 ) );
	doneProcessing = true;
}

void CThread::EventWaitFor( Event *ev )
{
	Entity      *ent;
	const char  *tname;
	TargetList  *tlist;
	int         i;
	
	ClearWaitFor();
	doneProcessing = true;
	
	ent = ev->GetEntity( 1 );
	if ( ent )
	{
		tname = ent->TargetName();
		
		//
		// set the number of objects that belong to this targetname
		//
		tlist = world->GetTargetList( str( tname ) );
		waitingFor = tlist;
		waitingNumObjects = tlist->list.NumObjects();
		
		//
		// make sure all these objects are in the update list
		//
		for( i = 1; i <= waitingNumObjects; i++ )
		{
			ent = tlist->list.ObjectAt( i );
			
			// add the object to the update list to make sure we tell it to do a move
			if ( !updateList.ObjectInList( ent->entnum ) )
            {
				updateList.AddObject( ent->entnum );
            }
		}
		
		if ( waitingNumObjects <= 0 )
		{
			waitingNumObjects = 1;
			ev->Error( "no objects of targetname %s found.\n", tname );
		}
	}
	
	DoMove();
}

void CThread::EventWaitForThread( Event *ev )
{
	doneProcessing = true;
	
	ClearWaitFor();
	waitingForThread = Director.GetThread( ev->GetInteger( 1 ) );
	if ( !waitingForThread )
	{
		ev->Error( "Thread %d not running", ev->GetInteger( 1 ) );
		return;
	}
	
	DoMove();
}

void CThread::EventWaitForDeath( Event *ev )
{
	doneProcessing = true;
	
	ClearWaitFor();
	waitingForDeath = ev->GetString(1);
	if ( !waitingForDeath.length() )
	{
		ev->Error( "Null name" );
		return;
	}
	
	DoMove();
}

void CThread::EventWaitForSound( Event *ev )
{
	str sound;
	float delay;
	
	ClearWaitFor();
	
	DoMove();
	
	delay = 0.0f;
	sound = ev->GetString( 1 );
	
	delay = gi.SoundLength( sound.c_str() );
	
	if ( delay < 0.0f )
	{
		gi.WDPrintf( "Lip file not found for dialog %s\n", sound.c_str() );
	}
	
	if ( ev->NumArgs() > 1 )
	{
		delay += ev->GetFloat( 2 );
	}
	
	DelayedStart( delay );
	doneProcessing = true;
}

void CThread::EventWaitForDialog( Event *ev )
{
	float delay;
	Actor *act;
	Entity *ent;
	
	ent = ev->GetEntity( 1 );
	
	if ( !ent )
	{
		gi.WDPrintf( "WaitForDialog: Can't find entity\n" );
		return;
	}
	
	if ( ent->isSubclassOf( Actor ) )
	{
		act = ( Actor * )ent;
		
		delay = act->GetDialogRemainingTime( );
		if ( ev->NumArgs() > 1 )
		{
			delay += ev->GetFloat( 2 );
		}
		DelayedStart( delay );
		doneProcessing = true;
	}
}

void CThread::EventWaitDialogLength( Event *ev )
{
	str dialogName;
	float delay;
	char localizedDialogName[ MAX_QPATH ];
	
	// Get the localized dialog name

	dialogName = ev->GetString( 1 );

	gi.LocalizeFilePath( dialogName, localizedDialogName );

	// Figure out the delay time (length of dialog + extra specified delay)

	delay = gi.SoundLength( localizedDialogName );

	if ( ev->NumArgs() > 1 )
	{
		delay += ev->GetFloat( 2 );
	}

	// Delay the thread
	
	DelayedStart( delay );
	doneProcessing = true;
}

void CThread::EventWaitForAnimation( Event *ev )
{
	Entity *ent;
	str animName;
	int animNum;
	float totalTime;
	float extraTime;
	
	// Get the entity we are refering to
	
	ent = ev->GetEntity( 1 );
	
	if ( !ent )
		return;
	
	// Get the animation we are refering to
	
	animName = ev->GetString( 2 );
	
	// Get any extra time if any
	
	if ( ev->NumArgs() > 2 )
		extraTime = ev->GetFloat( 3 );
	else
		extraTime = 0.0f;
	
	// Get the anim number
	
	animNum = gi.Anim_NumForName( ent->edict->s.modelindex, animName.c_str() );
	
	if ( animNum < 0 )
	{
		gi.WDPrintf( "Waiting for animation %s, but %s doesn't have this anim!\n", animName.c_str(), ent->model.c_str() );
		return;
	}
	
	// Get the total time of this animation
	
	totalTime = gi.Anim_Time( ent->edict->s.modelindex, animNum );
	
	totalTime += extraTime;
	
	// Setup all of the wait for stuff
	
	ClearWaitFor();
	DoMove();
	DelayedStart( totalTime );
	
	doneProcessing = true;
}

void CThread::EventWaitForPlayer( Event *ev )
{
	if ( !Director.PlayerReady() )
	{
		doneProcessing = true;
		
		ClearWaitFor();
		waitingForPlayer = true;
		
		DoMove();
	}
}

void CThread::CPrint( Event *ev )
{
	int         j;
	gentity_t   *other;

	for ( j = 0; j < game.maxclients; j++ )
	{
		other = &g_entities[j];
		if ( other->inuse && other->client )
		{
			gi.centerprintf( other , CENTERPRINT_IMPORTANCE_CRITICAL , ev->GetString( 1 ) );
		}
	}
}

void CThread::Print( Event *ev )
{
	int i;
	int n;
	
	n = ev->NumArgs();
	for( i = 1; i <= n; i++ )
	{
		gi.Printf( "%s", ev->GetString( i ) );
	}
}

void CThread::PrintInt( Event *ev )
{
	gi.Printf( "%d", ev->GetInteger( 1 ) );
}

void CThread::PrintFloat( Event *ev )
{
	gi.Printf( "%.2f", ev->GetFloat( 1 ) );
}

void CThread::PrintVector( Event *ev )
{
	Vector vec;
	
	vec = ev->GetVector( 1 );
	gi.Printf( "(%.2f %.2f %.2f)", vec.x, vec.y, vec.z );
}

void CThread::NewLine( Event *ev )
{
	gi.Printf( "\n" );
}

void CThread::Assert( Event *ev )
{
	assert( ev->GetFloat( 1 ) );
}

void CThread::Break( Event *ev )
{
	// Break into the debugger
#ifdef _WIN32
	__asm int 3
#else
		assert( 0 );
#endif
}

void CThread::ScriptCallback( Event *ev )
{
	const char  *name;
	Entity	   *other;
	Entity	   *slave;
	
	if ( threadDying )
	{
		return;
	}
	
	
	slave = ev->GetEntity( 1 );
	name = ev->GetString( 2 );
	other = ev->GetEntity( 3 );
	currentEntity = slave;
	
	if ( !Goto( name ) )
	{
		ev->Error( "Label '%s' not found", name );
	}
	else
	{
		// kill any execute events (in case our last command was "wait")
		ClearWaitFor();
		
		// start right away
		Start();
	}
}

void CThread::ThreadCallback( Event *ev )
{
	CThread *thread;
	
	if ( threadDying )
	{
		return;
	}
	
	thread = ev->GetThread();
	if ( thread && ( thread == waitingForThread ) )
	{
		ClearWaitFor();
		Start();
	}
}

void CThread::DeathCallback( Event *ev )
{
	if ( threadDying )
	{
		return;
	}
	
	ClearWaitFor();
	DelayedStart( 0.0f );
}

void CThread::DoMove( void )
{
	int      entnum;
	Entity   *ent;
	Event    *event;
	int      count;
	int      i;
	
	count = updateList.NumObjects();
	
	for( i = 1; i <= count; i++ )
	{
		entnum = ( int )updateList.ObjectAt( i );
		ent = G_GetEntity( entnum );
		if ( ent && ( ent->ValidEvent( EV_ProcessCommands ) ) )
		{
			event = new Event( EV_ProcessCommands );
			event->SetThread( this );
			ent->PostEvent( event , 0.00f );
		}
		else
		{
			// try to remove this from the update list
			if ( waitingNumObjects > 0 )
            {
				waitingNumObjects--;
            }
		}
	}
	
	updateList.ClearObjectList();
}

void CThread::TriggerEvent( Event *ev )
{
	const char	*name;
	Event		   *event;
	Entity	   *ent;
	TargetList  *tlist;
	int         i;
	int         num;
	
	name = ev->GetString( 1 );
	
	if ( !name )
		return;
	
	// Check for object commands
	if ( name[ 0 ] == '$' )
	{
		tlist = world->GetTargetList( str( name + 1 ) );
		num = tlist->list.NumObjects();
		for ( i = 1; i <= num; i++ )
		{
			ent = tlist->list.ObjectAt( i );
			
			assert( ent );
			
			event = new Event( EV_Activate );
			event->SetSource( EV_FROM_SCRIPT );
			event->SetThread( this );
			event->SetLineNumber( ev->GetLineNumber() );
			event->AddEntity( world );
			ent->ProcessEvent( event );
		}
	}
	else if ( name[ 0 ] == '*' )   // Check for entnum commands
	{
		if ( !IsNumeric( &name[ 1 ] ) )
		{
			RunError( "trigger: Expecting numeric value for * command, but found '%s'\n", &name[ 1 ] );
		}
		else
		{
			ent = G_GetEntity( atoi( &name[ 1 ] ) );
			if ( ent )
            {
				event = new Event( EV_Activate );
				event->SetSource( EV_FROM_SCRIPT );
				event->SetThread( this );
				event->SetLineNumber( ev->GetLineNumber() );
				event->AddEntity( world );
				ent->ProcessEvent( event );
            }
			else
            {
				RunError( "trigger: Entity not found for * command\n" );
            }
		}
		return;
	}
	else
	{
		RunError( "trigger: Invalid entity reference '%s'.\n", name );
	}
}

//----------------------------------------------------------------
// Name:			TriggerEntityEvent
// Class:			CThread
//
// Description:		Triggers the specified entity
//
// Parameters:		Event *ev			- contains entity to trigger
//
// Returns:			none
//----------------------------------------------------------------

void CThread::TriggerEntityEvent( Event *ev )
{
	Event	   *event;
	Entity	   *ent = NULL;//hzm gameupdate chrissstrahl - added NULL, in case a entity does not exist, this used to crash the game
	
	// Get the entity we wish to trigger
	
	ent = ev->GetEntity( 1 );
	
	if ( !ent ){
		gi.Printf( "CThread::TriggerEntityEvent - entity missing\n" );
		return;
	}
	
	// Trigger the entity
	
	event = new Event( EV_Activate );
	
	event->SetSource( EV_FROM_SCRIPT );
	event->SetThread( this );
	event->SetLineNumber( ev->GetLineNumber() );
	event->AddEntity( world );
	
	ent->ProcessEvent( event );
}

void CThread::CacheResourceEvent( Event * ev )
{
	if ( !precache->integer )
	{
		return;
	}
	
	CacheResource( ev->GetString( 1 ), world );
}

void CThread::RegisterAlias( Event *ev )
{
	str parameters;
	int i;
	
	// Get the parameters for this alias command
	for( i = 3; i <= ev->NumArgs(); i++ )
	   {
		parameters += ev->GetString( i );
		parameters += " ";
	   }
	
	gi.GlobalAlias_Add( ev->GetString( 1 ), ev->GetString( 2 ),  parameters.c_str() );
}

void CThread::RegisterAliasAndCache( Event *ev )
{
	RegisterAlias( ev );
	
	if ( !precache->integer )
	{
		return;
	}
	
	CacheResource( ev->GetString( 2 ) );
}

void CThread::MapEvent( Event *ev )
{
	G_BeginIntermission( ev->GetString( 1 ) );
	doneProcessing = true;
}

void CThread::noIntermission( Event *ev )
{
	level._showIntermission = false;
}

void CThread::dontSaveOrientation( Event *ev )
{
	level._saveOrientation = false;
}

void CThread::setPlayerDeathThread( Event *ev )
{
	level.setPlayerDeathThread( ev->GetString( 1 ) );
}

void CThread::endPlayerDeathThread( Event *ev )
{
	G_FinishMissionFailed();
}

void CThread::GetCvarEvent( Event *ev )
{
	assert( ev );
	cvar_t *cvar = gi.cvar_get( ev->GetString( 1 ) );
	ev->ReturnString( cvar ? cvar->string : "" );
}

void CThread::GetCvarFloatEvent( Event *ev )
{
	assert( ev );
	cvar_t *cvar = gi.cvar_get( ev->GetString( 1 ) );
	ev->ReturnFloat( cvar ? cvar->value : 0.0f );
}

void CThread::GetCvarIntEvent( Event *ev )
{
	assert( ev );
	cvar_t *cvar = gi.cvar_get( ev->GetString( 1 ) );
	ev->ReturnFloat( cvar ? cvar->integer : 0.0f );
}

void CThread::SetCvarEvent( Event *ev )
{
	str name;
	
	name = ev->GetString( 1 );
	if ( name != "" )
	{
		gi.Printf( va( "CVAR :: name: %s %s\n" , name.c_str() , ev->GetString( 2 ) ) );
		gi.cvar_set( name.c_str(), ev->GetString( 2 ) );
	}
}

void CThread::CueCamera( Event *ev )
{
	float    switchTime;
	Entity   *ent;
	
	if ( ev->NumArgs() > 1 )
	{
		switchTime = ev->GetFloat( 2 );
	}
	else
	{
		switchTime = 0;
	}
	
	ent = ev->GetEntity( 1 );
	if ( ent )
	{
		SetCamera( ent, switchTime );
	}
	else
	{
		ev->Error( "Camera named %s not found", ev->GetString( 1 ) );
	}
}

void CThread::CuePlayer( Event *ev )
{
	float switchTime;
	
	if ( ev->NumArgs() > 0 )
	{
		switchTime = ev->GetFloat( 1 );
	}
	else
	{
		switchTime = 0;
	}

	//hzm gameupdate chrissstrahl - clear current cinematic camera
	game.cinematicCurrentCam = NULL;
	
	SetCamera( NULL, switchTime );
}

void CThread::FreezePlayer( Event *ev )
{
	level.playerfrozen = true;
}

void CThread::ReleasePlayer( Event *ev )
{
	level.playerfrozen = false;
}

void CThread::FakePlayer( Event *ev )
{
	//hzm gamefix chrissstrahl - dissable in multi - cheap fix - fixme unfinished
	if ( g_gametype->integer > GT_SINGLE_PLAYER )
	{
		return;
	}

	Player *player;
	
	player = ( Player * )g_entities[ 0 ].entity;
	if ( player && player->edict->inuse && player->edict->client )
	{
		player->FakePlayer(false);
	}
}

void CThread::RemoveFakePlayer( Event *ev )
{
	//hzm gamefix chrissstrahl - dissable in multi - cheap fix - fixme unfinished
	if ( g_gametype->integer > GT_SINGLE_PLAYER )
	{
		return;
	}

	Player *player;
	
	player = ( Player * )g_entities[ 0 ].entity;
	if ( player && player->edict->inuse && player->edict->client )
	{
		player->RemoveFakePlayer();
	}
}

void CThread::Spawn( Event *ev )
{
	Entity		   *ent;
	Entity         *tent;
	const char	   *name;
	ClassDef		   *cls;
	int			   n;
	int			   i;
	const char     *targetname;
	const char	   *value;
	
	if ( ev->NumArgs() < 1 )
	{
		ev->Error( "Usage: spawn entityname [keyname] [value]..." );
		return;
	}
	
	// create a new entity
	SpawnArgs args;
	
	name = ev->GetString( 1 );
	
	if ( name )
	{
		cls = getClassForID( name );
		if ( !cls )
		{
			cls = getClass( name );
		}
		
		if ( !cls )
		{
			str sN;
			
			sN = name;
			if ( !strstr( sN.c_str(), ".tik" ) )
            {
				sN += ".tik";
            }
			args.setArg( "model", sN.c_str() );
		}
		else
		{
			args.setArg( "classname", name );
		}
	}
	
	if ( ev->NumArgs() > 2 )
	{
		n = ev->NumArgs();
		for( i = 2; i <= n; i += 2 )
		{
			args.setArg( ev->GetString( i ), ev->GetString( i + 1 ) );
		}
	}
	
	cls = args.getClassDef();
	if ( !cls )
	{
		ev->Error( "'%s' is not a valid entity name", name );
		return;
	}
	
	// If there is a spawntarget set, then use that entity's origin and angles
	targetname = args.getArg( "spawntarget" );
	
	if ( targetname )
	{
		tent = G_FindTarget( NULL, targetname );
		if ( tent )
		{
			args.setArg( "origin", va( "%f %f %f", tent->origin[ 0 ], tent->origin[ 1 ], tent->origin[ 2 ] ) );
			args.setArg( "angle", va( "%f", tent->angles[1] ) );
		}
		else
		{
			ev->Error( "Can't find targetname %s", targetname );
		}
	}
	
	//
	// make sure to setup spawnflags properly
	//
	level.spawnflags = 0;
	value = args.getArg( "spawnflags" );
	if ( value )
	{
		level.spawnflags = atoi( value );
	}
	
	ent = args.Spawn();

	ev->ReturnEntity( ent );
}

void CThread::Letterbox( Event *ev )
{
	float time;
	
	if ( ev->NumArgs() < 1 )
	{
		warning ("CThread::Letterbox", "time parameter required!\n");
		return;
	}
	
	level.m_letterbox_lastfraction = 1.0f/8.0f;
	
	time = ev->GetFloat( 1 );
	
	if ( ev->NumArgs() > 1 )
	{
		level.m_letterbox_lastfraction = ev->GetFloat( 2 );
	}
	
	// this is now handled server side
	//gi.SendServerCommand( NULL, va( "letterbox %0.2f 1 %0.2f",time,last_fraction ) );
	level.m_letterbox_time_start = time;
	level.m_letterbox_dir = letterbox_in;
	
	level.m_letterbox_time = time;
	level.m_letterbox_fraction = level.m_letterbox_lastfraction;
}

void CThread::ClearLetterbox( Event *ev )
{
	float time;
	
	time = ev->GetFloat( 1 );
	
	// this is now handled server side
	//gi.SendServerCommand( NULL, va( "letterbox %0.2f -1 %0.2f",time,last_fraction ) );
	level.m_letterbox_time_start = time;
	level.m_letterbox_dir = letterbox_out;
	
	level.m_letterbox_time = time;
	level.m_letterbox_fraction = level.m_letterbox_lastfraction;
}

void CThread::FadeIn( Event *ev )
{
	level.m_fade_time_start = ev->GetFloat( 1 );
	level.m_fade_time       = ev->GetFloat( 1 );
	level.m_fade_color[0]   = ev->GetFloat( 2 );
	level.m_fade_color[1]   = ev->GetFloat( 3 );
	level.m_fade_color[2]   = ev->GetFloat( 4 );
	level.m_fade_alpha      = ev->GetFloat( 5 );
	level.m_fade_type       = fadein;
	level.m_fade_style      = alphablend;
	
	if ( ev->NumArgs() > 5 )
	{
		level.m_fade_style   = (fadestyle_t)ev->GetInteger( 6 );
	}
}

void CThread::ClearFade( Event *ev )
{
	// this is now handled server side
	//gi.SendServerCommand( NULL, "clearfade" );
	level.m_fade_time = -1;
	level.m_fade_type = fadein;
}

void CThread::FadeOut( Event *ev )
{
	// Make sure we are not already faded or fading out

	if ( ( level.m_fade_type == fadeout ) && ( level.m_fade_time_start > 0.0f ) )
	{
		float alpha;

		alpha = 1.0f - ( level.m_fade_time / level.m_fade_time_start );

		if ( alpha > 0.0f )
			return;
	}

	level.m_fade_time_start = ev->GetFloat( 1 );
	level.m_fade_time       = ev->GetFloat( 1 );
	level.m_fade_color[0]   = ev->GetFloat( 2 );
	level.m_fade_color[1]   = ev->GetFloat( 3 );
	level.m_fade_color[2]   = ev->GetFloat( 4 );
	level.m_fade_alpha      = ev->GetFloat( 5 );
	level.m_fade_type       = fadeout;
	level.m_fade_style      = alphablend;
	
	if ( ev->NumArgs() > 5 )
	{
		level.m_fade_style   = (fadestyle_t)ev->GetInteger( 6 );
	}
}

void CThread::FadeIsActive( Event *ev )
{
	if ( level.m_fade_time > 0.0f )
		ev->ReturnInteger( true );
	else
		ev->ReturnInteger( false );
}

void CThread::MusicEvent( Event *ev )
{
	const char *current;
	const char *fallback;
	
	current = ev->GetString( 1 );
	
	fallback = NULL;
	if ( ev->NumArgs() > 1 )
	{
		fallback = ev->GetString( 2 );
	}
	
	ChangeMusic( current, fallback, false );
}

void CThread::MusicVolumeEvent( Event *ev )
{
	float volume;
	float fade_time;
	
	volume    = ev->GetFloat( 1 );
	fade_time = ev->GetFloat( 2 );
	
	ChangeMusicVolume( volume, fade_time );
}

void CThread::RestoreMusicVolumeEvent( Event *ev )
{
	float fade_time;
	
	fade_time = ev->GetFloat( 1 );
	
	RestoreMusicVolume( fade_time );
}

//----------------------------------------------------------------
// Name:			allowMusicDucking
// Class:			CThread
//
// Description:		Specifies whether or not music ducking is allowed
//
// Parameters:		Event *ev				- contains bool, specifies whether or not music ducking is allowed
//
// Returns:			none
//----------------------------------------------------------------

void CThread::allowMusicDucking( Event *ev )
{
	if ( ev->NumArgs() > 0 )
		G_AllowMusicDucking( ev->GetBoolean( 1 ) );
	else
		G_AllowMusicDucking( true );
}

//----------------------------------------------------------------
// Name:			allowActionMusic
// Class:			CThread
//
// Description:		Specifies whether or not action music is allowed
//
// Parameters:		Event *ev				- contains bool, specifies whether or not action music is allowed
//
// Returns:			none
//----------------------------------------------------------------

void CThread::allowActionMusic( Event *ev )
{
	if ( ev->NumArgs() > 0 )
		G_AllowActionMusic( ev->GetBoolean( 1 ) );
	else
		G_AllowActionMusic( true );
}

void CThread::ForceMusicEvent( Event *ev )
{
	const char *current;
	const char *fallback;
	
	current = ev->GetString( 1 );
	fallback = NULL;
	if ( ev->NumArgs() > 1 )
	{
		fallback = ev->GetString( 2 );
	}
	
	ChangeMusic( current, fallback, true );
}

void CThread::SoundtrackEvent( Event *ev )
{
	ChangeSoundtrack( ev->GetString( 1 ) );
}

void CThread::RestoreSoundtrackEvent( Event *ev )
{
	RestoreSoundtrack();
}

void CThread::SetCinematic( Event *ev )
{
	G_StartCinematic();
}

void CThread::SetNonCinematic( Event *ev )
{
	G_StopCinematic();
	//level.cinematic = false;
}

void CThread::SetLevelAI( Event *ev )
{
	qboolean ai = false;
	
	if ( ev->NumArgs() > 0 )
		ai = ev->GetBoolean( 1 );
	
	if ( ai )
		level.ai_on = true;
	else
		level.ai_on = false;
}

void CThread::SetSkipThread( Event *ev )
{
	world->skipthread = ev->GetString( 1 );
}

void CThread::PassToPathmanager( Event *ev )
{
	thePathManager.ProcessEvent( ev );
}

void CThread::StuffCommand( Event *ev )
{
	int i;
	str command;
	
	for( i = 1; i <= ev->NumArgs(); i++ )
	{
		command += ev->GetString( i );
		command += " ";
	}
	
	if ( command.length() )
	{
		gi.SendConsoleCommand( command.c_str() );
	}
}

void CThread::KillEnt( Event * ev )
{
	int num;
	Entity *ent;
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "No args passed in" );
		return;
	}
	
	num = ev->GetInteger( 1 );
	if ( ( num < 0 ) || ( num >= globals.max_entities ) )
	{
		ev->Error( "Value out of range.  Possible values range from 0 to %d.\n", globals.max_entities );
		return;
	}
	
	ent = G_GetEntity( num );
	ent->Damage( world, world, ent->max_health + 25.0f, vec_zero, vec_zero, vec_zero, 0, 0, 0 );
}

void CThread::RemoveEnt( Event * ev )
{
	int num;
	Entity *ent;
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "No args passed in" );
		return;
	}
	
	num = ev->GetInteger( 1 );
	if ( ( num < 0 ) || ( num >= globals.max_entities ) )
	{
		ev->Error( "Value out of range.  Possible values range from 0 to %d.\n", globals.max_entities );
		return;
	}
	
	ent = G_GetEntity( num );
	ent->PostEvent( Event( EV_Remove ), 0.0f );
}

void CThread::KillClass( Event * ev )
{
	int except;
	str classname;
	gentity_t * from;
	Entity *ent;
	
	if ( ev->NumArgs() < 1 )
	{
		ev->Error( "No args passed in" );
		return;
	}
	
	classname = ev->GetString( 1 );
	
	except = 0;
	if ( ev->NumArgs() == 2 )
	{
		except = ev->GetInteger( 1 );
	}
	
	for ( from = &g_entities[ game.maxclients ]; from < &g_entities[ globals.num_entities ]; from++ )
	{
		if ( !from->inuse )
		{
			continue;
		}
		
		assert( from->entity );
		
		ent = from->entity;
		
		if ( ent->entnum == except )
		{
			continue;
		}
		
		if ( ent->inheritsFrom( classname.c_str() ) )
		{
			ent->Damage( world, world, ent->max_health + 25.0f, vec_zero, vec_zero, vec_zero, 0, 0, 0 );
		}
	}
}

// RemoveActorsNamed - remove all actors in the game that have a matching name field
void CThread::RemoveActorsNamed( Event* ev )
{
	if ( ev->NumArgs() < 1 )
	{
		ev->Error( "No args passed in" );
		return;
	}
	
	str removename = ev->GetString( 1 );

	for( gentity_t* gent = &g_entities[ game.maxclients ]; 
		gent < &g_entities[ globals.num_entities ]; 
		gent++ )
	{
		if( !gent->inuse ) continue;
		assert( gent->entity );
		if( !gent->entity->inheritsFrom( "Actor" ) ) continue;

		Actor* act = static_cast< Actor* >( gent->entity );
		if( act->getName() == removename )
		{
			gent->entity->PostEvent( Event( EV_Remove ), 0.0f );
		}
	}
}

void CThread::RemoveClass( Event * ev )
{
	int except;
	str classname;
	gentity_t * from;
	Entity *ent;
	
	if ( ev->NumArgs() < 1 )
	{
		ev->Error( "No args passed in" );
		return;
	}
	
	classname = ev->GetString( 1 );
	
	except = 0;
	if ( ev->NumArgs() == 2 )
	{
		except = ev->GetInteger( 1 );
	}
	
	for ( from = &g_entities[ game.maxclients ]; from < &g_entities[ globals.num_entities ]; from++ )
	{
		if ( !from->inuse )
		{
			continue;
		}
		
		assert( from->entity );
		
		ent = from->entity;
		
		if ( ent->entnum == except )
			continue;
		
		if ( ent->inheritsFrom( classname.c_str() ) )
		{
			ent->PostEvent( Event( EV_Remove ), 0.0f );
		}
	}
}

//===============================================================
// Name:		doesEntityExist
// Class:		CThread
//
// Description: Determines if an entity with the specified name
//				exists.  The return value for the event is either
//				a 1 or a 0 ( true or false ).
// 
// Parameters:	Event* -- the event that triggered this call.
//
// Returns:		None
// 
//===============================================================

void CThread::doesEntityExist( Event *ev )
{
	if ( ev->IsEntityAt( 1 ) )
		ev->ReturnFloat( 1.0f );
	else
		ev->ReturnFloat( 0.0f );
}


void CThread::GetEntityEvent( Event *ev )
{
	TargetList *tlist;
	
	str name( ev->GetString( 1 ) );
	if ( ( name.length() > 0 ) && name[ 0 ] == '*' )
	{
		ev->ReturnEntity( G_GetEntity( atoi( &name[ 1 ] ) ) );
		return;
	}
	tlist = world->GetTargetList( name, false );
	
	//FIXME
	//ev->ReturnInteger( ( int )tlist );
	if ( tlist )
	{
		ev->ReturnEntity( tlist->GetNextEntity( NULL ) );
	}
	else
	{
		ev->ReturnEntity( NULL );
	}
}

void CThread::GetNextEntityEvent( Event *ev )
{
	TargetList *tlist;
	
	Entity *ent = 0;
	ent = ev->GetEntity( 1 );
	
	if ( !ent )
	{
		ev->ReturnEntity( NULL );
		return;
	}
	
	tlist = world->GetTargetList( ent->targetname, false );
	
	if ( tlist )
		ev->ReturnEntity( tlist->GetNextEntity( ent ) );
	else
		ev->ReturnEntity( NULL );
}

void CThread::CosDegrees( Event *ev )
{
	float degrees = ev->GetFloat( 1 );
	float radians = DEG2RAD( degrees );
	float result = cos( radians );
	
	ev->ReturnFloat( result );
}

void CThread::SinDegrees( Event *ev )
{
	float degrees = ev->GetFloat( 1 );
	float radians = DEG2RAD( degrees );
	float result = sin( radians );
	
	ev->ReturnFloat( result );
}

void CThread::CosRadians( Event *ev )
{
	float radians = ev->GetFloat( 1 );
	float result = cos( radians );
	
	ev->ReturnFloat( result );
}

void CThread::SinRadians( Event *ev )
{
	float radians = ev->GetFloat( 1 );
	float result = sin( radians );
	
	ev->ReturnFloat( result );
}

void CThread::ArcTanDegrees( Event *ev )
{
	float y = ev->GetFloat( 1 );
	float x = ev->GetFloat( 2 );
	float radians = atan2( y, x );
	float degrees = RAD2DEG( radians );
	
	ev->ReturnFloat( degrees );
}

void CThread::ScriptSqrt( Event *ev )
{
	/// Perform a sign-preserving square root ( i.e. sqrt( -16 ) = -4 )
	float x = ev->GetFloat( 1 );
	if( x < 0.0f )
	{
		x = static_cast<float>( -sqrt( -x ) );
	}
	else
	{
		x = static_cast<float>( sqrt( x ) );
	}
	ev->ReturnFloat( x );
}

void CThread::ScriptLog( Event *ev )
{
	float x = ev->GetFloat( 1 );

	ev->ReturnFloat( log( x ) );
}

void CThread::ScriptExp( Event *ev )
{
	float x = ev->GetFloat( 1 );

	ev->ReturnFloat( exp( x ) );
}

void CThread::RandomFloat( Event *ev )
{
	ev->ReturnFloat( G_Random( ev->GetFloat( 1 ) ) );
}

void CThread::RandomInteger( Event *ev )
{
	ev->ReturnFloat( (float)(int)G_Random( (float)ev->GetInteger( 1 ) ) );
}

void CThread::CameraCommand( Event * ev )
{
	Event *e;
	const char *cmd;
	int   i;
	int   n;
	
	if ( !ev->NumArgs() )
	{
		ev->Error( "Usage: cam [command] [arg 1]...[arg n]" );
		return;
	}
	
	cmd = ev->GetString( 1 );
	if ( Event::Exists( cmd ) )
	{
		e = new Event( cmd );
		e->SetSource( EV_FROM_SCRIPT );
		e->SetThread( this );
		e->SetLineNumber( ev->GetLineNumber() );
		
		n = ev->NumArgs();
		for( i = 2; i <= n; i++ )
		{
			e->AddToken( ev->GetToken( i ) );
		}
		
		CameraMan.ProcessEvent( e );
	}
	else
	{
		ev->Error( "Unknown camera command '%s'.\n", cmd );
	}
}

void CThread::SetLightStyle( Event *ev )
{
	gi.SetLightStyle( ev->GetInteger( 1 ), ev->GetString( 2 ) );
}

void CThread::KillThreadEvent( Event *ev )
{
	str threadToKill;

	threadToKill = ev->GetString( 1 );

	if ( threadToKill == threadName )
	{
		gi.WDPrintf( "Trying to kill the current thread %s, this is very bad!\n", threadName.c_str() );
		return;
	}

	if ( threadToKill.length() == 0 )
	{
		gi.WDPrintf( "Trying to kill a thread with no name\n" );
		return;
	}

	Director.KillThread( threadToKill );
}

void CThread::SetThreadName( Event *ev )
{
	threadName = ev->GetString( 1 );
}

void CThread::GetCurrentEntity( Event *ev )
{
	if ( currentEntity )
		ev->ReturnEntity( currentEntity );
	else
		ev->ReturnEntity ( NULL );
}

void CThread::SetCurrentEntity( Entity *ent )
{
	if ( ent )
		currentEntity = ent;
}

void CThread::Parse( const char *filename )
{
	str      token;
	Script   script;
	int      eventnum;
	Event    *ev;
	
	script.LoadFile( filename );
	while( script.TokenAvailable( true ) )
	{
		token = script.GetToken( true );
		if ( token == "end" )
		{
			break;
		}
		
		eventnum = Event::FindEvent( token );
		if ( !eventnum )
		{
			gi.WDPrintf( "Unknown command '%s' in %s\n", token.c_str(), filename );
			script.SkipToEOL();
		}
		else
		{
			ev = new Event( eventnum );
			while( script.TokenAvailable( false ) )
            {
				ev->AddToken( script.GetToken( false ) );
            }
			
			ProcessEvent( ev );
		}
	}
}

void CThread::SendCommandToSlaves( const char *name, Event *ev )
{
	Event		   *sendevent;
	Entity	   *ent;
	TargetList  *tlist;
	int         i;
	int         num;
	
	if ( name && name[ 0 ] )
	{
		updateList.ClearObjectList();
		
		tlist = world->GetTargetList( str( name + 1 ) );
		num = tlist->list.NumObjects();
		for ( i = 1; i <= num; i++ )
		{
			ent = tlist->list.ObjectAt( i );
			
			assert( ent );
			
			sendevent = new Event( *ev );
			
			if ( !updateList.ObjectInList( ent->entnum ) )
			{
				updateList.AddObject( ent->entnum );
				
				// Tell the object that we're about to send it some orders
				ent->ProcessEvent( EV_Script_NewOrders );
			}
			
			// Send the command
			ent->ProcessEvent( sendevent );
		}
		
		if ( !num )
		{
			warning( "SendCommandToSlaves", "Could not find target %s in world.\n", name );
		}
	}
	
	//
	// free up the event
	//
	delete ev;
}

void CThread::ProcessCommand( int argc, const char **argv )
{
	str		command;
	str		name;
	Event		*event;
	Entity	*ent;
	
	if ( argc < 1 )
	{
		return;
	}
	
	name = argv[ 0 ];
	if ( argc > 1 )
	{
		command = argv[ 1 ];
	}
	
	// Check for object commands
	if ( name[ 0 ] == '$' )
	{
		if ( Event::FindEvent( command.c_str() ) )
		{
			event = new Event( command );
			event->SetSource( EV_FROM_SCRIPT );
			event->SetThread( this );
			event->SetLineNumber( 0 );
			event->AddTokens( argc - 2, &argv[ 2 ] );
			SendCommandToSlaves( name.c_str(), event );
		}
		return;
	}
	
	// Check for entnum commands
	if ( name[ 0 ] == '*' )
	{
		if ( !IsNumeric( &name[ 1 ] ) )
		{
			gi.WPrintf( "Expecting numeric value for * command, but found '%s'\n", &name[ 1 ] );
		}
		else if ( Event::FindEvent( command.c_str() ) )
		{
			ent = G_GetEntity( atoi( &name[ 1 ] ) );
			if ( ent )
            {
				event = new Event( command );
				event->SetSource( EV_FROM_SCRIPT );
				event->SetThread( this );
				event->SetLineNumber( 0 );
				event->AddTokens( argc - 2, &argv[ 2 ] );
				ent->ProcessEvent( event );
            }
			else
            {
				gi.WPrintf( "Entity not found for * command\n" );
            }
		}
		return;
	}
	
	// Handle global commands
	if ( Event::FindEvent( name.c_str() ) )
	{
		event = new Event( name );
		event->SetSource( EV_FROM_SCRIPT );
		event->SetThread( this );
		event->SetLineNumber( 0 );
		event->AddTokens( argc - 1, &argv[ 1 ] );
		if ( !ProcessEvent( event ) )
		{
			gi.WPrintf( "Invalid global command '%s'\n", name.c_str() );
		}
	}
}

inline void CThread::Archive( Archiver &arc )
{
	int exists;
	
	Interpreter::Archive( arc );
	
	arc.ArchiveFloat( &waitUntil );
	
	// save waiting for thread
	
	if ( arc.Saving() )
	{
		if ( waitingForThread )
			exists = 1;
		else
			exists = 0;
	}
	
	arc.ArchiveInteger( &exists );
	
	if ( exists )
		arc.ArchiveObjectPointer( ( Class ** )&waitingForThread );
	else if ( arc.Loading() )
		waitingForThread = NULL;
	
	arc.ArchiveString( &waitingForDeath );
	arc.ArchiveBoolean( &waitingForPlayer );
	
	if ( arc.Saving() )
	{
		if ( waitingFor )
			exists = 1;
		else
			exists = 0;
	}
	
	arc.ArchiveInteger( &exists );
	
	if ( exists )
		arc.ArchiveObjectPointer( ( Class ** )&waitingFor );
	else if ( arc.Loading() )
		waitingFor = NULL;
	
	arc.ArchiveInteger( &waitingNumObjects );
	
	arc.ArchiveSafePointer( &currentEntity );
}

void CThread::SetFloatVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	float value;
	
	var_name = ev->GetString( 1 );
	value    = ev->GetFloat( 2 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		levelVars.SetVariable( real_var_name, value );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//hzm gamefix chrissstrahl - fix game. variables useing now +5 instead of +6 (the first letter gets cut off)
		real_var_name = var_name.c_str() + 5;
		gameVars.SetVariable( real_var_name, value );

		//hzm coop mod chrissstrahl - make sure we catch the values to save them in case of restart
		//save it in ini cuz cvars don't work if clients hosts a dedicated server and starts another instance from the same directory
		coop_serverSaveGameVars( real_var_name , ( int )value );
	}
}

void CThread::SetVectorVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	Vector value;
	
	var_name = ev->GetString( 1 );
	value    = ev->GetVector( 2 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		levelVars.SetVariable( real_var_name, value );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//hzm gamefix chrissstrahl - fix game. variables useing now +5 instead of +6 (the first letter gets cut off)
		real_var_name = var_name.c_str() + 5;
		gameVars.SetVariable( real_var_name, value );

		//hzm coop mod chrissstrahl - make sure we chatch the values to save them in case of restart
		//save it in ini cuz cvars don't work if clients hosts a dedicated server and starts another instance from the same directory
		coop_serverSaveGameVars( real_var_name , ("" + value) );
	}
}

void CThread::SetStringVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	str value;
	
	var_name = ev->GetString( 1 );
	value    = ev->GetString( 2 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		levelVars.SetVariable( real_var_name, value );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//hzm gamefix chrissstrahl - fix game. variables useing now +5 instead of +6 (the first letter gets cut off)
		real_var_name = var_name.c_str() + 5;
		gameVars.SetVariable( real_var_name, value );

		//hzm coop mod chrissstrahl - make sure we chatch the values to save them in case of restart
		//save it in ini cuz cvars don't work if clients hosts a dedicated server and starts another instance from the same directory
		coop_serverSaveGameVars( real_var_name , value );
	}
}

void CThread::RemoveVariable( Event* ev )
{
	str var_name;
	const char* real_var_name;

	var_name = ev->GetString( 1 );

	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		levelVars.RemoveVariable( real_var_name );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//hzm gamefix chrissstrahl - fix game. variables useing now +5 instead of +6 (the first letter gets cut off)
		real_var_name = var_name.c_str() + 5;
		levelVars.RemoveVariable( real_var_name );
	}
}

void CThread::GetFloatVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	ScriptVariable *var = NULL;
	
	var_name = ev->GetString( 1 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		var = levelVars.GetVariable( real_var_name );
	}
	
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//hzm gamefix chrissstrahl - fix game. variables useing now +5 instead of +6 (the first letter gets cut off)
		real_var_name = var_name.c_str() + 5;
		var = gameVars.GetVariable( real_var_name );
	}
	
	if ( var )
		ev->ReturnFloat( var->floatValue() );
	else
	{
		gi.WDPrintf( "%s variable not found\n", var_name.c_str() );
		ev->ReturnFloat( 0.0f );
	}
}

void CThread::GetVectorVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	ScriptVariable *var = NULL;
	
	var_name = ev->GetString( 1 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		var = levelVars.GetVariable( real_var_name );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//hzm gamefix chrissstrahl - fix game. variables useing now +5 instead of +6 (the first letter gets cut off)
		real_var_name = var_name.c_str() + 5;
		var = gameVars.GetVariable( real_var_name );
	}
	
	if ( var )
		ev->ReturnVector( var->vectorValue() );
	else
	{
		gi.WDPrintf( "%s variable not found\n", var_name.c_str() );
		ev->ReturnVector( vec_zero );
	}
}

void CThread::GetStringVar( Event *ev )
{
	str var_name;
	const char *real_var_name;
	ScriptVariable *var = NULL;
	
	var_name = ev->GetString( 1 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		var = levelVars.GetVariable( real_var_name );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//hzm gamefix chrissstrahl - fix game. variables useing now +5 instead of +6 (the first letter gets cut off)
		real_var_name = var_name.c_str() + 5;
		var = gameVars.GetVariable( real_var_name );
	}
	
	if ( var )
		ev->ReturnString( var->stringValue() );
	else
	{
		gi.WDPrintf( "%s variable not found\n", var_name.c_str() );
		ev->ReturnString( "\n" );
	}
}

void CThread::doesVarExist( Event *ev )
{
	str var_name;
	const char *real_var_name;
	ScriptVariable *var = NULL;
	
	var_name = ev->GetString( 1 );
	
	if ( strncmp( var_name.c_str(), "level.", 6 ) == 0 )
	{
		real_var_name = var_name.c_str() + 6;
		var = levelVars.GetVariable( real_var_name );
	}
	else if ( strncmp( var_name.c_str(), "game.", 5 ) == 0 )
	{
		//hzm gamefix chrissstrahl - fix game. variables useing now +5 instead of +6 (the first letter gets cut off)
		real_var_name = var_name.c_str() + 5;
		var = gameVars.GetVariable( real_var_name );
	}
	
	if ( var )
		ev->ReturnFloat( 1.0f );
	else
		ev->ReturnFloat( 0.0f );
}

void CThread::CenterPrint( Event *ev )
{
	int         j;
	gentity_t   *other;
	
	for( j = 0; j < game.maxclients; j++ )
	{
		other = &g_entities[ j ];
		if ( other->inuse && other->client )
		{
			gi.centerprintf( other, CENTERPRINT_IMPORTANCE_CRITICAL, ev->GetString( 1 ) );
		}
	}
}

void CThread::isActorDead( Event *ev )
{
	ev->ReturnInteger( isActorDead( ev->GetString( 1 ) ) );
}

qboolean CThread::isActorDead( const str &actor_name )
{
	Actor *act;
	
	act = GetActor( actor_name );
	
	if ( !act || act->deadflag || ( act->health <= 0 ) )
		return true;
	
	return false;
}

void CThread::isActorAlive( Event *ev )
{
	ev->ReturnFloat( isActorAlive( ev->GetString( 1 ) ) );
}

qboolean CThread::isActorAlive( const str &actor_name )
{
	Actor *act;
	
	act = GetActor( actor_name );
	
	if ( !act || act->deadflag || ( act->health <= 0 ) )
		return false;
	
	return true;
}

void CThread::SendClientCommand( Event *ev )
{
	Entity	*entity;
	Player	*player;
	str		builtCommand;
	int		i;
	
	
	// Get the player
	
	entity = ev->GetEntity( 1 );
	
	//[b60011] chrissstrahl - gamefix - added check if entity even exists
	//this was crashing server if given player did not exist
	if (!entity) {
		gi.Printf("SendClientCommand: Given Player does not exist!\n");
		return;
	}

	if (!entity->isSubclassOf( Player ) )
		return;
	
	player = (Player *)entity;
	
	// Build the command
	
	builtCommand += "stufftext";
	
	builtCommand += " \"";
	
	for ( i = 2 ; i <= ev->NumArgs() ; i++ )
	{
		builtCommand += " ";
		
		builtCommand += ev->GetString( i );
	}
	
	builtCommand += "\"\n";

	gi.SendServerCommand( player->edict - g_entities, builtCommand.c_str() );
}

void CThread::GetNumFreeReliableServerCommands( Event* ev )
{
	Entity* entity;
	Player* player;
	int i;

	entity = ev->GetEntity( 1 );

	//[b60011] chrissstrahl - gamefix - added check if entity even exists
	//this was crashing server if given player did not exist
	if (!entity) {
		gi.Printf("GetNumFreeReliableServerCommands: Given Player does not exist!\n");
		ev->ReturnFloat(0.0);
		return;
	}

	if(!entity->isSubclassOf( Player ) )
	{
		ev->ReturnFloat( 0.0 );
		return;
	}

	player = (Player*) entity;

	i = gi.GetNumFreeReliableServerCommands( player->edict - g_entities );
	ev->ReturnFloat( i );
}

void CThread::SendClientVar( Event *ev )
{
	Entity	*entity;
	Player	*player;
	str		builtCommand;
	str		varName;
	str		realVarName;
	ScriptVariable *var = NULL;
	str		levelVarPrefix = "level.";
	
	
	// Get the player
	
	entity = ev->GetEntity( 1 );
	
	if ( !entity->isSubclassOf( Player ) )
		return;
	
	player = (Player *)entity;
	
	// Build the command
	
	builtCommand += "stufftext";
	
	builtCommand += " \"SetClientVar ";
	
	// Add the variable name
	
	varName = ev->GetString( 2 );
	
	if ( strnicmp( varName.c_str(), levelVarPrefix.c_str(), levelVarPrefix.length() ) != 0 )
	{
		gi.WDPrintf( "SendClientVar can only send level vars, %s not allowed\n", varName.c_str() );
		return;
	}
	
	realVarName = varName.c_str() + levelVarPrefix.length();
	
	builtCommand += realVarName;
	
	// Add the variable value
	
	builtCommand += " ";
	
	var = levelVars.GetVariable( realVarName );
	
	if ( var )
		builtCommand += var->stringValue();
	else
		return;
	
	builtCommand += "\"\n";
	
	// Send the command
	
	gi.SendServerCommand( player->edict - g_entities, builtCommand.c_str() );
}

//===============================================================
// Name:		ModEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_Mod event.
//				Gets the integer remainder of a division.
// 
// Parameters:	Event* -- Argument 1 is the nominator.
//						  Argument 2 is the denominator.
//						  Return cache is the integer remainder.
//
// Returns:		None
// 
//===============================================================
void CThread::ModEvent( Event *ev )
{
	assert( ev );
	if ( ev->NumArgs() != 2 )
	{
		ev->Error("Usage: variable = %s( nominator, denominator)\n", ev->getName());
		return ;
	}
	
	int	nominator	= ev->GetInteger( 1 );
	int	denominator = ev->GetInteger( 2 );
	int	remainder	= nominator % denominator ;
	ev->ReturnFloat( remainder );
}

//===============================================================
// Name:		DivEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_Div event.
//				Gets the integer result of a division.
// 
// Parameters:	Event* -- Argument 1 is the nominator
//						  Argument 2 is the denominator
//						  Return cache is the integer result
//
// Returns:		None
// 
//===============================================================
void CThread::DivEvent( Event *ev )
{
	assert( ev );
	if ( ev->NumArgs() != 2 )
	{
		ev->Error("Usage: variable = %s( nominator, denominator )\n", ev->getName());
		return ;
	}
	
	int	nominator	= ev->GetInteger( 1 );
	int	denominator	= ev->GetInteger( 2 );
	int	result		= nominator / denominator ;
	
	ev->ReturnFloat( result );
}

//===============================================================
// Name:		VectorScaleEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorScaleEvent.
//				Scales a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to scale (vector)
//						  Argument 2 is the amount to scale (float)
//						  Return cache is the scaled vector.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorScaleEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 2 )
	{
		ev->Error("Usage: vectorvariable = %s( vector, scale_value )\n", ev->getName());
		return ;
	}
	
	Vector	v		= ev->GetVector( 1 );
	float	scaler	= ev->GetFloat( 2 );
	Vector returnVector = v * scaler ;
	
	ev->ReturnVector( returnVector );
}

//===============================================================
// Name:		VectorDotEvent
// Class:		CThread
//
// Description: Handles EV_ScriptThread_VectorDotEvent.
//				Gets the dot product of two vectors.
// 
// Parameters:	Event* -- Argument 1 is the first vector.
//						  Argument 2 is the second vector.
//						  Return cache is the dot product value.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorDotEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 2 )
	{
		ev->Error("Usage: floatvariable = %s( vector, vector )\n", ev->getName());
		return ;
	}
	
	Vector	v1		= ev->GetVector( 1 );
	Vector	v2		= ev->GetVector( 2 );
	float	result	= Vector::Dot( v1, v2 );
	
	ev->ReturnFloat( result );
}

//===============================================================
// Name:		VectorCrossEvent
// Class:		CThread
//
// Description: Handles EV_ScriptThread_VectorCrossEvent.
//				Gets the cross product of two vectors.
// 
// Parameters:	Event* -- Argument 1 is the first vector.
//						  Argument 2 is the second vector.
//						  Return cache is the cross product value.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorCrossEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 2 )
	{
		ev->Error("Usage: vectorvariable = %s( vector, vector )\n", ev->getName());
		return ;
	}
	
	Vector	v1		= ev->GetVector( 1 );
	Vector	v2		= ev->GetVector( 2 );
	Vector	result ;
	result.CrossProduct( v1, v2 );
	
	ev->ReturnVector( result );
}

//===============================================================
// Name:		VectorLengthEvent
// Class:		CThread
//
// Description: Handles EV_ScriptThread_VectorLengthEvent.
//				Gets the length of a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get the length of.
//						  Return cache is the vector length.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorLengthEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	ev->ReturnFloat( v1.length() );
}

//===============================================================
// Name:		VectorNormalizeEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorNormalizeEvent.  
//				Normalizes a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to normalize.
//						  Return cache is the normalized vector.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorNormalizeEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: vectorvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	v1.normalize();
	ev->ReturnVector( v1 );
}

//===============================================================
// Name:		VectorGetXEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorGetXEvent.
//				Gets the x value from a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get x from.
//						  Return cache is the x value.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorGetXEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	ev->ReturnFloat( v1.x ) ;
}

//===============================================================
// Name:		VectorGetYEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorGetXEvent.
//				Gets the y value from a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get y from.
//						  Return cache is the y value.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorGetYEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	ev->ReturnFloat( v1.y ) ;
}

//===============================================================
// Name:		VectorGetZEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorGetXEvent.
//				Gets the z value from a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get z from.
//						  Return cache is the z value.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorGetZEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	ev->ReturnFloat( v1.z ) ;
}

//===============================================================
// Name:		VectorSetXEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorSetXEvent.
//				Sets the x value for a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to set the x of.
//						  Argument 2 is the x value for the vector
//						  Return cache is the resulting vector.
//							
//
// Returns:		None
// 
//===============================================================
void CThread::VectorSetXEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 2 )
	{
		ev->Error( "Usage: vectorvariable = %s( vector, x_value )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	v1.x = ev->GetFloat( 2 );
	ev->ReturnVector( v1 );
}

//===============================================================
// Name:		VectorSetYEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorSetYEvent.
//				Sets the y value for a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to set the y of.
//						  Argument 2 is the y value for the vector
//						  Return cache is the resulting vector.
//							
//
// Returns:		None
// 
//===============================================================
void CThread::VectorSetYEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 2 )
	{
		ev->Error( "Usage: vectorvariable = %s( vector, y_value )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	v1.y = ev->GetFloat( 2 );
	ev->ReturnVector( v1 );
}

//===============================================================
// Name:		VectorSetZEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorSetZEvent.
//				Sets the z value for a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to set the z of
//						  Argument 2 is the z value for the vector
//						  Return cache is the resulting vector.
//							
// Returns:		None
// 
//===============================================================
void CThread::VectorSetZEvent( Event *ev )
{
	assert( ev );
	
	if (ev->NumArgs() != 2 )
	{
		ev->Error( "Usage: vectorvariable = %s( vector, z_value )\n", ev->getName());
		return ;
	}
	
	Vector v1 = ev->GetVector( 1 );
	v1.z = ev->GetFloat( 2 );
	ev->ReturnVector( v1 );
}

//===============================================================
// Name:		VectorForwardEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorForwardEvent.
//				Gets the forward vector of a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get the forward of.
//						  Return cache is the forward vector.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorForwardEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector forward ;
	Vector v1 = ev->GetVector( 1 );
	v1.AngleVectors( &forward );
	ev->ReturnVector( forward );
}

//===============================================================
// Name:		VectorLeftEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorLeftEvent.
//				Gets the left vector of a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get the left of.
//						  Return cache is the left vector.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorLeftEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector left ;
	Vector v1 = ev->GetVector( 1 );
	v1.AngleVectors( 0, &left );
	ev->ReturnVector( left );
}

//===============================================================
// Name:		VectorUpEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_VectorUpEvent.
//				Gets the up vector of a vector.
// 
// Parameters:	Event* -- Argument 1 is the vector to get the up of.
//						  Return cache is the up vector.
//
// Returns:		None
// 
//===============================================================
void CThread::VectorUpEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: floatvariable = %s( vector )\n", ev->getName());
		return ;
	}
	
	Vector up ;
	Vector v1 = ev->GetVector( 1 );
	v1.AngleVectors( 0, 0, &up );
	ev->ReturnVector( up );
}

//===============================================================
// Name:		vectorToString
// Class:		CThread
//
// Description: Converts a vector to a string
// 
// Parameters:	Event *ev				- contains a vector
//
// Returns:		None
// 
//===============================================================

void CThread::vectorToString( Event *ev )
{
	str stringResult;
	Vector vectorToConvert;
	
	vectorToConvert = ev->GetVector( 1 );
	
	stringResult = "\"";
	stringResult += vectorToConvert.x;
	stringResult += " ";
	stringResult += vectorToConvert.y;
	stringResult += " ";
	stringResult += vectorToConvert.z;
	stringResult += "\"";
	
	ev->ReturnString( stringResult );
}

//===============================================================
// Name:		TargetOfEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_TargetOfEvent.
//				Gets the target of an entity.
// 
// Parameters:	Event* -- Argument 1 is the entity whose target you want.
//						  Return cache is the name of the target,
//						  or the empty string if no target.
//
// Returns:		None
// 
//===============================================================
void CThread::TargetOfEvent( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 )
	{
		ev->Error( "Usage: stringvariable = %s( entity )\n", ev->getName());
		return ;
	}
	
	Entity *entity = ev->GetEntity( 1 );
	if ( !entity )
	{
		ev->Error( "First argument of %s is not an entity!\n", ev->getName());
		return ;
	}
	
	const char *target = entity->Target();
	ev->ReturnString( target ? target : "" );
}



//===============================================================
// Name:		FloorEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_FloorEvent,
//				Floors the specified floor.
// 
// Parameters:	Event* -- Argument 1 is the float you want the
//						  floor of.
//
// Returns:		None
// 
//===============================================================
void CThread::FloorEvent( Event *ev )
{
	assert( ev );
	
	float value = ev->GetFloat( 1 );
	ev->ReturnFloat( floor( value ) );
}

//===============================================================
// Name:		CeilEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_CeilEvent,
//				Ceils the specified float.
// 
// Parameters:	Event* -- Argument 1 is the float you want the 
//						  ceil of.
//
// Returns:		None
// 
//===============================================================
void CThread::CeilEvent( Event *ev )
{
	assert( ev );
	float value = ev->GetFloat( 1 );
	ev->ReturnFloat( ceil( value ) );
}

//===============================================================
// Name:		RoundEvent
// Class:		CThread
//
// Description: Handles the EV_ScriptThread_RoundEvent.
//				Rounds the specified float.
// 
// Parameters:	Event* -- Argument 1 is the float you want to round.
//
// Returns:		None
// 
//===============================================================
void CThread::RoundEvent( Event *ev )
{
	assert( ev );
	
	float value = ev->GetFloat( 1 );
	float remainder = value - floor(value) ;
	ev->ReturnFloat( (remainder >= 0.5) ? ceil( value ) : floor( value ) );
}

//--------------------------------------------------------------
//
// Name:			SetGameplayFloat
// Class:			CThread
//
// Description:		Sets properties on the gameplay database.
//
// Parameters:		Event *ev
//
// Returns:			None
//
//--------------------------------------------------------------
void CThread::SetGameplayFloat( Event *ev )
{
	const char *objname = 0;
	const char *propname = 0;
	float value = 1.0f;
	
	// Check for not enough args
	if ( ev->NumArgs() < 3 )
		return;
	
	objname = ev->GetString( 1 );
	propname = ev->GetString( 2 );
	value = ev->GetFloat( 3 );
	
	GameplayManager::getTheGameplayManager()->setFloatValue(objname, propname, value);
}

//--------------------------------------------------------------
//
// Name:			GetGameplayString
// Class:			CThread
//
// Description:		Gets a gameplay database property string value.
//
// Parameters:		Event *ev
//
// Returns:			None
//
//--------------------------------------------------------------
void CThread::GetGameplayString( Event *ev )
{
	const char *objname = 0;
	const char *propname = 0;
	
	// Check for not enough args
	if ( ev->NumArgs() < 2 )
	{
		ev->Error( "getgameplaystring: Wrong number of arguments. getgameplaystring objname propertyname" );
		return;
	}
	
	objname = ev->GetString( 1 );
	propname = ev->GetString( 2 );
	
	str stringvalue = GameplayManager::getTheGameplayManager()->getStringValue( objname, propname );
	ev->ReturnString( stringvalue.c_str() );
}

//--------------------------------------------------------------
//
// Name:			GetGameplayFloat
// Class:			CThread
//
// Description:		Gets a gameplay database property float value.
//
// Parameters:		Event *ev
//
// Returns:			None
//
//--------------------------------------------------------------
void CThread::GetGameplayFloat( Event *ev )
{
	const char *objname = 0;
	const char *propname = 0;
	
	// Check for not enough args
	if ( ev->NumArgs() < 2 )
	{
		ev->Error( "getgameplayfloat: Wrong number of arguments. getgameplayfloat objname propertyname" );
		return;
	}
	
	objname = ev->GetString( 1 );
	propname = ev->GetString( 2 );
	
	float floatvalue = GameplayManager::getTheGameplayManager()->getFloatValue( objname, propname );
	ev->ReturnFloat( floatvalue );
}

//--------------------------------------------------------------
//
// Name:			SetGameplayString
// Class:			CThread
//
// Description:		Sets properties on the gameplay database.
//
// Parameters:		Event *ev
//
// Returns:			None
//
//--------------------------------------------------------------
void CThread::SetGameplayString( Event *ev )
{
	const char *objname = 0;
	const char *propname = 0;
	const char *valuestr = 0;
	
	// Check for not enough args
	if ( ev->NumArgs() < 3 )
		return;
	
	objname = ev->GetString( 1 );
	propname = ev->GetString( 2 );
	valuestr = ev->GetString( 3 );
	
	GameplayManager::getTheGameplayManager()->setStringValue(objname, propname, valuestr);
}

//===============================================================
// Name:		GetIntegerFromString
// Class:		CThread
//
// Description: Retreives the first integer value found in the
//				event's first argument (which must be a string).
//
//				The value is returned in the event's return field.
// 
// Parameters:	Event -- the event that triggered this call
//
// Returns:		None
// 
//===============================================================
void CThread::GetIntegerFromString( Event *ev )
{
	assert( ev );
	
	const char  *valueText	= 0 ;
	char		*text		= 0 ;
	char		 buffer[256] ;
	
	strcpy( buffer, ev->GetString( 1 ) );
	text = buffer ;
	
	while ( text && *text )
	{
		if ( ( *text < '0' ) ||  ( *text > '9') )
		{
			text++ ;
			continue ;
		}
		valueText = text ;
		while ( text && *text )
		{
			if ( ( *text < '0') || ( *text > '9' ) )
			{
				*text = 0 ;
				break ;
			}
			text++ ;
		}
		break ;
	}
	
	int value = atoi( valueText );
	ev->ReturnInteger( value );
}

//===============================================================
// Name:		CreateCinematic
// Class:		CThread
//
// Description: Creates a new cinematic with the specified name.
//				Loads the corresponding cinematic from disk.  Once
//				loaded, events can be called on the cinematic directly
//				to set it up and play it.
// 
// Parameters:	Event* -- EV_ScriptThread_CreateCinematic.
//					- First argument must be string name without .cin extension.
//
// Returns:		None
// 
//===============================================================
void CThread::CreateCinematic( Event *ev )
{
	assert( ev );
	
	if ( ev->NumArgs() != 1 ) 
	{
		ev->Error( "script usage: entity cin = createCinematic <cinematicWithoutCinExtension\n" );
		return ;
	}
	
	str cinematicName = ev->GetString( 1 );
	Cinematic *cinematic = theCinematicArmature.createCinematic( cinematicName );
	ev->ReturnEntity( cinematic );
}

//===============================================================
// Name:		GetLevelTime
// Class:		CThread
//
// Description:	Returns the level.time to the script.
// Parameters:	Event* -- EV_ScriptThread_GetTime
// Returns:		none
//===============================================================
void CThread::GetLevelTime( Event* ev )
{
	assert( ev );
	ev->ReturnFloat( level.time );
}

void CThread::disconnectPathnodes( Event *ev )
{
	thePathManager.disconnectNodes( ev->GetString( 1 ), ev->GetString( 2 ) );
}

void CThread::connectPathnodes( Event *ev )
{
	thePathManager.connectNodes( ev->GetString( 1 ), ev->GetString( 2 ) );
}
