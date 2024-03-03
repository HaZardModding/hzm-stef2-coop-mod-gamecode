#ifndef __COOPGAMECMDS_HPP__
#define __COOPGAMECMDS_HPP__

#include "_pch_cpp.h"
#include "mp_manager.hpp"
#include "coopCheck.hpp"

//[b60021] chrissstrahl - takes a live of a player and gives it to a dead player
qboolean G_coopCom_notransferlive(const gentity_t* ent);
//hzm coop mod chrissstrahl - allwoing players to signal that they have the coop mod installed
qboolean G_coopInstalled(const gentity_t* ent);
//hzm coop mod chrissstrahl - allow client to send a idendification string
qboolean G_coopClientId(const gentity_t* ent);
//[b60011] chrissstrahl - allowing players to make use of special coop inventory
qboolean G_coopItem(const gentity_t* ent);
//[b60011] chrissstrahl - allowing players to start thread that can be tied back
qboolean G_coopThread(const gentity_t* ent);
//[b60011] chrissstrahl - allowing players to send text to server
qboolean G_coopInput(const gentity_t* ent);

//[b60011] chrissstrahl - add coop ! commads
qboolean G_coopCom_block(const gentity_t* ent);
qboolean G_coopCom_class(const gentity_t* ent);
qboolean G_coopCom_drop(const gentity_t* ent);
qboolean G_coopCom_help(const gentity_t* ent);
qboolean G_coopCom_info(const gentity_t* ent);
qboolean G_coopCom_kickbots(const gentity_t* ent);
qboolean G_coopCom_kill(const gentity_t* ent);
qboolean G_coopCom_login(const gentity_t* ent);
qboolean G_coopCom_logout(const gentity_t* ent);
qboolean G_coopCom_mapname(const gentity_t* ent);
qboolean G_coopCom_origin(const gentity_t* ent);
qboolean G_coopCom_skill(const gentity_t* ent);
qboolean G_coopCom_stuck(const gentity_t* ent);
qboolean G_coopCom_transport(const gentity_t* ent);
qboolean G_coopCom_leader(const gentity_t* ent);
//[b60014] chrissstrahl - add coop ! commads
qboolean G_coopCom_follow(const gentity_t* ent);
qboolean G_coopCom_notransport(const gentity_t* ent);
//[b60021] chrissstrahl - added
qboolean G_coopCom_transferlive(const gentity_t* ent);
qboolean G_coopCom_classability(const gentity_t* ent);
// COOP COMMANDS - DEVELOPER SPECIFIC
qboolean G_coopCom_reboot(const gentity_t* ent);
qboolean G_coopCom_levelend(const gentity_t* ent);
qboolean G_coopCom_targeted(const gentity_t* ent);
qboolean G_coopCom_showspawn(const gentity_t* ent);
qboolean G_coopCom_flushtikis(const gentity_t* ent);
qboolean G_coopCom_hidespawn(const gentity_t* ent);
qboolean G_coopCom_testspawn(const gentity_t* ent);
qboolean G_coopCom_noclip(const gentity_t* ent);

#endif /* !__COOPGAMECMDS_HPP__ */