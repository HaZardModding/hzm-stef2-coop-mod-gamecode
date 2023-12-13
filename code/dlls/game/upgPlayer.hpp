//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// Player class related Upgrades, Updates and Fixes
//-----------------------------------------------------------------------------------

#pragma once
//[GAMEUPGRADE][b60014] chrissstrahl - used to seperate code bases better
class UpgPlayer
{
	friend class Player;
	int			clMaxPackets = -1;
	float		chatTimeLimit = 0.0f;
	float		chatTimeLastSpamInfo = -999.0f;
	float		clientThinkLastTime = -1;
	int			deathTime = -986;
	str			language = "Eng";
	Entity*		targetedEntityLast = NULL;
	float		timeEntered = -988.0f;
	//used to manage transmitting of scanning data to coop tricorder hud
	str			scanDataSendLast = "";
	bool		scanHudActive = false;	
	str			scanData0 = "";
	str			scanData1 = "";
	str			scanData2 = "";
	float		lastTimeSkipCinematic = -970.0f;
};

void	upgPlayerDelayedServerCommand(int entNum, const char* commandText);
void	upgPlayerHandleDelayedServerCommands(void);
void	upgPlayerclearDelayedServerCommands(int entNum);

extern UpgPlayer upgPlayer;

extern Event EV_Player_getViewtraceEndpos;
extern Event EV_Player_getUserFov;
extern Event EV_Player_GetScriptVariablesCommand;
extern Event EV_Player_setCamera;
extern Event EV_Player_setKillThread;
extern Event EV_Player_getLanguage;
extern Event EV_Player_getName;
extern Event EV_Player_WidgetCommand;
extern Event EV_Player_getScore;
extern Event EV_Player_addScore;
extern Event EV_Player_getDeaths;
extern Event EV_Player_getKills;
extern Event EV_Player_getLastDamaged;
extern Event EV_Player_getTeamName;
extern Event EV_Player_getTeamScore;
extern Event EV_Player_getBackpackAttachOffset;
extern Event EV_Player_getBackpackAttachAngles;
extern Event EV_Player_getFlagAttachOffset;
extern Event EV_Player_getFlagAttachAngles;
extern Event EV_Player_checkAnyButton;
extern Event EV_Player_checkMenu;
extern Event EV_Player_checkForward;
extern Event EV_Player_checkBackward;
extern Event EV_Player_checkLeft;
extern Event EV_Player_checkRight;
extern Event EV_Player_checkLeanRight;
extern Event EV_Player_checkLeanLeft;
extern Event EV_Player_checkDropRune;
extern Event EV_Player_checkRun;
extern Event EV_Player_checkReload;
extern Event EV_Player_checkJump;
extern Event EV_Player_checkCrouch;
extern Event EV_Player_checkUse;
extern Event EV_Player_checkThirdperson;
extern Event EV_Player_checkFire;
extern Event EV_Player_checkFirealt;
extern Event EV_Player_RunThread;
extern Event EV_Player_GetViewangles;
extern Event EV_Player_GetTargetedEntity;
extern Event EV_Player_upgPlayerMessageOfTheDay;
extern Event EV_Player_HasLanguageGerman;
extern Event EV_Player_HasLanguageEnglish;
extern Event EV_Actor_upgBranchDialogFailsafe;