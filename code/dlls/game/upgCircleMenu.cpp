//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// Circle Menu Related Code, used to add New Mewnu Type to the game, like known from modern games.
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "player.h"
#include "gamecmds.h"


#include "upgCircleMenu.hpp"
#include "upgMp_manager.hpp"
#include "upgStrings.hpp"
#include "upgPlayer.hpp"

UpgCircleMenu upgCircleMenu;

extern Event EV_Player_ActivateNewWeapon;
extern Event EV_Player_DeactivateWeapon;



//-----------------------------------------------------------------------------------
// Events, these have external Dependencies
//-----------------------------------------------------------------------------------
//hzm gameupdate chrissstrahl [b60011] - add new commands for script use
Event EV_Player_circleMenu
(
	"circleMenu",
	EV_SCRIPTONLY,
	"f",
	"integer-menutype",
	"Shows Circle Menu to player, 1=Normal, 2=Dialog."
);
//hzm gameupdate chrissstrahl [b60011] - add new commands for script use
Event EV_Player_circleMenuDialogSet
(
	"circleMenuDialogSet",
	EV_SCRIPTONLY,
	"issS",
	"optionnumber optiontext threadname imageORshader",
	"Adds a dialog option for player to circle menu"
);
//hzm gameupdate chrissstrahl [b60011] - add new commands for script use
Event EV_Player_circleMenuDialogClear
(
	"circleMenuDialogClear",
	EV_SCRIPTONLY,
	"F",
	"int-dialog-number",
	"Clears all circle menu dialog options from menu"
);
//hzm gameupdate chrissstrahl [b60011] - add new commands for script use
Event EV_Player_circleMenuSet
(
	"circleMenuSet",
	EV_SCRIPTONLY,
	"issSIIIS",
	"optionnumber optiontext threadOrCommandName imageORshader isThread iAmmount iCost sCostType",
	"Adds a option for player from circle menu"
);
//hzm gameupdate chrissstrahl [b60011] - add new commands for script use
Event EV_Player_circleMenuClear
(
	"circleMenuClear",
	EV_SCRIPTONLY,
	"F",
	"int-item-number",
	"Clears a option if a number is given. otherwise it clears all options for player circle menu"
);

//[b60021] chrissstrahl
//================================================================
// Name:        upgCircleMenuReset
// Class:       Player
//              
// Description: Sets up variables for circelmenu to work
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
void Player::upgCircleMenuReset()
{
	CancelEventsOfType(EV_Player_circleMenuSet);

	upgCircleMenu.active = 0;
	upgCircleMenu.numOfSegments = 4;
	upgCircleMenu.viewAngle = Vector(0, 0, 0);
	upgCircleMenu.thinkTime = 0.0f;
	upgCircleMenu.activatingTime = 0.0f;
	upgCircleMenu.lastMessageTime = 0.0f;
	upgCircleMenu.lastViewangle = Vector(0, 0, 0);
	upgCircleMenu.longtimeViewangle = Vector(0, 0, 0);
	upgCircleMenu.lastWidget = "";
	upgCircleMenu.lastSegment = -1;
	upgCircleMenu.lastWeapon = "None";
	upgCircleMenu.holdingRightButton = false;
	upgCircleMenu.holdingLeftButton = false;

	for (short i = 0; i < CIRCLEMENU_MAX_OPTIONS; i++) {
		upgCircleMenu.optionIconLastSend[i] = "";
		upgCircleMenu.optionTextLastSend[i] = "";
		upgCircleMenu.optionThreadOrCommand[i] = "";
		upgCircleMenu.optionText[i] = "";
		upgCircleMenu.optionIcon[i] = "";
		upgCircleMenu.optionIsScript[i] = false;
		upgCircleMenu.optionAmmount[i] = 9999;
		upgCircleMenu.optionCost[i] = 0;
		upgCircleMenu.optionCostType[i] = 0;
	}
	
	for (short i = 0; i < CIRCLEMENU_MAX_OPTIONSDIALOG; i++) {
		upgCircleMenu.optionDialogThread[i] = "";
		upgCircleMenu.optionDialogText[i] = "";
		upgCircleMenu.optionDialogIcon[i] = "";
	}
}

//[b60021] chrissstrahl
//================================================================
// Name:        upgCircleMenuSetup
// Class:       Player
//              
// Description: Sets up variables for circelmenu to work
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
void Player::upgCircleMenuSetup()
{
	for (short i = 0; i < CIRCLEMENU_MAX_OPTIONS;i++) {
		upgCircleMenu.optionIconLastSend[i] = "";
		upgCircleMenu.optionTextLastSend[i] = "";
	}
}

//[b60011] chrissstrahl
//================================================================
// Name:        switchWidgets
// Class:       Player
//              
// Description: This sends a command to enable/disable a widget in a single burst
//              
// Parameters:  str widget1, str widget2, str widget1Cmd, str widget2Cmd
//              
// Returns:     -
//              
//================================================================
void Player::upgCircleMenuSwitchWidgets(str widget1, str widget2, str widget1Cmd, str widget2Cmd)
{
	str command = "globalwidgetcommand ";
	command += widget1;
	command += " ";
	command += widget1Cmd;
	command += ";globalwidgetcommand ";
	command += widget2;
	command += " ";
	command += widget2Cmd;
	command += "\n";
	G_SendCommandToPlayer(this->edict, command.c_str());
}

//[b60011] chrissstrahl
//================================================================
// Name:        upgCircleMenuIsActive
// Class:       -
//              
// Description: Checks if upgCircleMenu is active for player
//              
// Parameters:  -
//              
// Returns:     bool
//              
//================================================================
bool Player::upgCircleMenuIsActive()
{
	if (upgCircleMenu.active > 0) {
		return true;
	}

	return false;
}

//[b60011] chrissstrahl
//================================================================
// Name:        upgCircleMenuLastTimeActive
// Class:       -
//              
// Description: Returns level time at which the menus was last active
//              
// Parameters:  -
//              
// Returns:     float
//              
//================================================================
float Player::upgCircleMenuLastTimeActive()
{
	if (upgCircleMenu.activatingTime > upgCircleMenu.thinkTime) {
		return upgCircleMenu.activatingTime;
	}
	return upgCircleMenu.thinkTime;
}

//[b60011] chrissstrahl
//================================================================
// Name:        upgCircleMenu
// Class:       -
//              
// Description: Activated/Deactivates the circle menu
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
void Player::upgCircleMenuCall(int iType)
{
	//prevent premature re-opening
	if (upgCircleMenu.active <= 0 && (upgCircleMenu.activatingTime + 0.5) >= level.time || level.cinematic || getHealth() <= 0 || multiplayerManager.isPlayerSpectator(this)) {
		//hudPrint("c open/close tosoon\n");
		return;
	}

	//make sure bad value is catched
	if (iType <= 0 || iType > 2) {
		gi.Printf(va("%s upgCircleMenu( %i ) <- unsupported Menu Type number in parameter 1\n", client->pers.netname ,iType));
		hudPrint(va("%s upgCircleMenu( %i ) <- unsupported Menu Type number in parameter 1\n", client->pers.netname, iType));
		return;
	}

	//[b60012] chrissstrahl
	//upgPlayerDelayedServerCommand(entnum, "-attackLeft");
	//upgPlayerDelayedServerCommand(entnum, "-attackRight");

	/*
	Event* StopFireEvent;
	StopFireEvent = new Event(EV_Sentient_StopFire);
	StopFireEvent->AddString("dualhand");
	this->ProcessEvent(StopFireEvent);
	*/

	upgCircleMenu.numOfSegments = 4;

	upgCircleMenu.activatingTime = level.time;

	if (upgCircleMenu.active <= 0) {
//hudPrint("upgCircleMenu - start\n");//hzm coopdebug circlemenu

		//reset holding buttons - precent acidental activation
		upgCircleMenu.holdingLeftButton = true;
		upgCircleMenu.holdingRightButton = true;

		upgCircleMenu.lastSegment = 0; //reset selected segment

		upgCircleMenu.viewAngle = getViewAngles();
		upgCircleMenu.lastViewangle = upgCircleMenu.viewAngle;

		upgCircleMenu.active = iType;
		upgCircleMenuHud(true);

		//[b60021] chrissstrahl - stop weapon from fireing
		upgCircleMenu.lastWeapon = "None";
		getActiveWeaponName(WEAPON_ANY, upgCircleMenu.lastWeapon);

		Event* StopFireEvent;
		StopFireEvent = new Event(EV_Sentient_StopFire);
		StopFireEvent->AddString("dualhand");
		this->ProcessEvent(StopFireEvent);

		Event* deactivateWeaponEv;
		deactivateWeaponEv = new Event(EV_Player_DeactivateWeapon);
		deactivateWeaponEv->AddString("dualhand");
		this->PostEvent(deactivateWeaponEv,0.05);

		disableInventory();

		Event* StopFireEvent2;
		StopFireEvent2 = new Event(EV_Sentient_StopFire);
		StopFireEvent2->AddString("dualhand");
		this->PostEvent(StopFireEvent2,0.1);

		//upgPlayerDisableUseWeapon(true);
	}
	else {
//hudPrint("upgCircleMenu - end\n");//hzm coopdebug circlemenu
		upgPlayerDelayedServerCommand(entnum, "-attackLeft");
		
		upgCircleMenuHud(false);
		upgCircleMenu.active = 0;

		enableInventory();

		//[b60021] chrissstrahl - restore prev weapon
		Event* useWeaponEv;
		useWeaponEv = new Event(EV_Player_UseItem);
		useWeaponEv->AddString(upgCircleMenu.lastWeapon);
		useWeaponEv->AddString("dualhand");
		this->ProcessEvent(useWeaponEv);

		Event* StopFireEvent;
		StopFireEvent = new Event(EV_Sentient_StopFire);
		StopFireEvent->AddString("dualhand");
		this->PostEvent(StopFireEvent,0.1f);

		Event* activateWeaponEv;
		activateWeaponEv = new Event(EV_Player_ActivateNewWeapon);
		this->PostEvent(activateWeaponEv,0.1f);
		//upgPlayerDisableUseWeapon(false);
	}
}

//[b60011] chrissstrahl
//================================================================
// Name:        upgCircleMenuGetWidgetName();
// Class:       -
//              
// Description:	Calculates the desired move direction based on the last direction moves
//              
// Parameters:  int iSegment
//              
// Returns:     Circle Menu Widgetname for given direction
//              
//================================================================
str Player::upgCircleMenuGetWidgetName(int iSegment)
{
	str currentWidget = "coop_circle";
	int iSegments = upgCircleMenu.numOfSegments;

	//if circlemenu is of type dialog
	if (upgCircleMenu.active == 2) {
		currentWidget = "coop_circleD";
	}

	switch (iSegments)
	{
	case 4: //0=up,1=right,2=down,3=left
		switch (iSegment) {
		case 0:
			currentWidget += "T";
			break;
		case 1:
			currentWidget += "R";
			break;
		case 2:
			currentWidget += "B";
			break;
		case 3:
			currentWidget += "L";
			break;
		default:
			gi.Printf(va("Player::circleMenuGetWidgetName(%i) %i - No widget name avialable for parameter 1\n", iSegment, iSegments));
		}
		break;
	case 8: //0=Top,1=TopRight 2=right,3=BottomRight, 4=Bottom,5=BottomLeft, 6=Left,7=TopLeft
		currentWidget = "coop_circle8";
		switch (iSegment) {
		case 0:
			currentWidget += "T";
			break;
		case 1:
			currentWidget += "TR";
			break;
		case 2:
			currentWidget += "R";
			break;
		case 3:
			currentWidget += "BR";
			break;
		case 4:
			currentWidget += "B";
			break;
		case 5:
			currentWidget += "BL";
			break;
		case 6:
			currentWidget += "L";
			break;
		case 7:
			currentWidget += "TL";
			break;
		default:
			gi.Printf(va("Player::circleMenuGetWidgetName(%i) %i - No widget name avialable for parameter 1\n", iSegment, iSegments));
		}
	default:
		gi.Printf(va("Player::circleMenuGetWidgetName(%i) %i - No widget name avialable for parameter 1\n", iSegment, iSegments));
	}
	return currentWidget;
}

//[b60011] chrissstrahl
//================================================================
// Name:        getSegmentNumForAngle();
// Class:       -
//              
// Description:	Get the Segment Number for given Angle
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
int Player::upgCircleMenuGetSegmentNumForAngle(float fAngle)
{
	float fNumSegments = 4;
	float fSegmentDegreeSize = (360 / fNumSegments);
	float fSegmentMaxEnd = (359 - (fSegmentDegreeSize / 2));

	//go step whise in reverse
	float iStep;
	float iHighestSegmentNum;
	float fCurrentMax;
	fCurrentMax = fSegmentMaxEnd;
	iHighestSegmentNum = (fNumSegments - 1);

	for (iStep = iHighestSegmentNum; iStep >= 0; iStep--) {
		//print("getSegmentNumForAngle: '"+fAngle+"'\n");
		if (fCurrentMax > fAngle && fAngle > (fCurrentMax - (fSegmentDegreeSize))) {
			return iStep;
		}
		fCurrentMax = AngleNormalize360(fCurrentMax - fSegmentDegreeSize);
	}
	if (fAngle > fCurrentMax && fAngle <= 359) {
		return 0;
	}
	return -1;
}

//[b60011] chrissstrahl
//================================================================
// Name:        upgCircleMenuThink
// Class:       -
//              
// Description:	Checks if circle menu is active and manages interactions
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
void Player::upgCircleMenuThink()
{
	//0 + = down
	//0 - = up
	//1 - = left
	//1 + right
	//1 179+ to 0 to -179

	//detect which movedirection the player did move towards
	//on menu show exec reset
	if (upgCircleMenu.active <= 0 /* || upgCircleMenu.thinkTime > level.time */) {
		return;
	}
	
//gi.Printf(va("Player::upgCircleMenuThink()->upgCircleMenuGetWidgetName %i - %f\n", upgCircleMenu.active, upgCircleMenu.thinkTime));

	//make sure it can not be abused by spec
	if (this->getHealth() <= 0 || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(this)) {
		if (upgCircleMenu.active) {
			upgCircleMenuHud(false);
			upgCircleMenu.active = 0;
		}
//hudPrint("upgCircleMenuThink - dead or spec\n");//hzm coopdebug circlemenu

		return;
	}

	//player is clicking fire
	if (last_ucmd.buttons & BUTTON_ATTACKLEFT) {
		if (!upgCircleMenu.holdingLeftButton) {
			upgCircleMenu.holdingLeftButton = true;
//hudPrint("c select\n");
			upgCircleMenu.thinkTime = level.time;
			upgCircleMenuSelect(upgCircleMenu.lastSegment);

//hudPrint(va("upgCircleMenuThink - select %d\n", upgCircleMenu.lastSegment));//hzm coopdebug circlemenu
			return;
		}
	}
	else {
		upgCircleMenu.holdingLeftButton = false;
	}

	//detect and record the mouse move directions
	Vector vDifference = Vector(0, 0, 0);
	Vector vViewangle = Vector(0, 0, 0);
	GetPlayerView(NULL, &vViewangle);

	//if all the same, we can abbort ?
	/* //[b60021] chrissstrahl - deactivated to allow attackright button test
	if (vViewangle == upgCircleMenu.lastViewangle) {
		upgCircleMenu.thinkTime = level.time;
		return;
	}*/


	if ((upgCircleMenu.thinkTime + 0.05) > level.time) { return; }
	upgCircleMenu.thinkTime = level.time;

	str sWidgetName;
	int fSegmentNum = upgCircleMenu.lastSegment;
	//[b60021] chrissstrahl - select widget by right click
	if (last_ucmd.buttons & BUTTON_ATTACKRIGHT) {
		if (upgCircleMenu.holdingRightButton) {
//hudPrint("upgCircleMenuThink - holding right\n");//hzm coopdebug circlemenu
			return;
		}

		upgCircleMenu.holdingRightButton = true;
		fSegmentNum = (upgCircleMenu.lastSegment + 1);
		if (fSegmentNum >= 4) {
			fSegmentNum = 0;
		}
//hudPrint(va("upgCircleMenuThink - next %d\n",fSegmentNum));//hzm coopdebug circlemenu
	}
	else {
		upgCircleMenu.holdingRightButton = false;
	}

	sWidgetName = upgCircleMenuGetWidgetName(fSegmentNum);
	//gi.Printf("Player::upgCircleMenuThink()->upgCircleMenuGetWidgetName\n");

	if (sWidgetName != "" && sWidgetName != upgCircleMenu.lastWidget) {
		str sCmd;
		G_SendCommandToPlayer(this->edict, va("globalwidgetcommand %s shadercolor 0 0 0 1", sWidgetName.c_str()));
		G_SendCommandToPlayer(this->edict, va("globalwidgetcommand %s shadercolor 0.5 0.5 0.5 1", upgCircleMenu.lastWidget.c_str()));
	}

	//gi.Printf(va("Reset: %s\n", upgCircleMenu.lastWidget));
	//str sPrint = va("prev: %s curr: %s\n", upgCircleMenu.lastWidget.c_str(), sWidgetName.c_str());
	upgCircleMenu.lastWidget = sWidgetName;
	upgCircleMenu.lastSegment = fSegmentNum;

	/* OLD CODE WITH MOUSE MOVE DETECTION - janky!!!
	//get difference - remember last viewangle
	vDifference = (upgCircleMenu.lastViewangle - vViewangle);
	upgCircleMenu.lastViewangle = vViewangle;

	upgCircleMenu.longtimeViewangle[0] += vDifference[0];
	upgCircleMenu.longtimeViewangle[1] += vDifference[1];

	//angle on 2d screen - circle menu
	float fAngle;
	if ((upgCircleMenu.thinkTime + 0.05) > level.time) { return; }
	upgCircleMenu.thinkTime = level.time;

	//here is where the magic happens
	float radians = atan2(upgCircleMenu.longtimeViewangle[1], upgCircleMenu.longtimeViewangle[0]);
	float degrees = RAD2DEG(radians);
	fAngle = AngleNormalize360( degrees );
	fSegmentNum = upgCircleMenuGetSegmentNumForAngle(fAngle);


//gi.Printf("Player::upgCircleMenuThink()->upgCircleMenuGetWidgetName\n");
	sWidgetName = upgCircleMenuGetWidgetName(fSegmentNum);

	//reset
	upgCircleMenu.longtimeViewangle = Vector(0,0,0);

	//make sure we only react if it seams like a legit move - this will need more love
	if (vDifference.length() < 0.1 || fSegmentNum < 0) { return; }

	if (sWidgetName != "" && sWidgetName != upgCircleMenu.lastWidget) {
		str sCmd;
		G_SendCommandToPlayer(this->edict,va("globalwidgetcommand %s shadercolor 0 0 0 1", sWidgetName.c_str()));
		G_SendCommandToPlayer(this->edict,va("globalwidgetcommand %s shadercolor 1 1 1 1", upgCircleMenu.lastWidget.c_str()));
	}

	//gi.Printf(va("Reset: %s\n", upgCircleMenu.lastWidget));
	//str sPrint = va("prev: %s curr: %s\n", upgCircleMenu.lastWidget.c_str(), sWidgetName.c_str());
	upgCircleMenu.lastWidget = sWidgetName;

	/*if (fAngle != 0) {
		sPrint = va("%s - '%d'\n",fSegmentNum);
		gi.Printf(va(": %s\n", sPrint));
	}
	gi.Printf(va("Angle: %d\n", fAngle));
	gi.Printf(va("length: %f\n", vDifference.length()));
	
	upgCircleMenu.lastSegment = fSegmentNum;*/
}

//[b60011] chrissstrahl
//================================================================
// Name:        upgCircleMenuSelect
// Class:       -
//              
// Description:	Manages if the player has selected a item on the circle menu
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
void Player::upgCircleMenuSelect(int iOption)
{
	if (iOption < 0 || iOption >= CIRCLEMENU_MAX_OPTIONS) {
		gi.Printf(va("upgCircleMenuSelect: Given Option %d is out of Range for Client[%d] %s\n", iOption,entnum,client->pers.netname));
		hudPrint("c select outofrange\n");
		return;
	}

	bool	bIsScript = true;
	str		sThread;

	if (upgCircleMenu.active != 2) {
		 bIsScript	= upgCircleMenu.optionIsScript[iOption];
		 sThread	= upgCircleMenu.optionThreadOrCommand[iOption];
		 if (upgCircleMenu.optionAmmount[iOption] < 1) {
			float fMessageSilenceTime = 0.5f;
			if ((upgCircleMenu.lastMessageTime + fMessageSilenceTime) < level.time){
				gi.Printf(va("upgCircleMenuSelect: Given Option %d ammount < 1 for Client[%d] %s\n", iOption, entnum,client->pers.netname));
			}

			//close menu if no options are set
			int iValidOptions = 0;
			for (int iOptionCheck = (CIRCLEMENU_MAX_OPTIONS - 1); iOptionCheck >= 0; iOptionCheck--) {
				if (upgCircleMenu.optionAmmount[iOptionCheck] > 0) {
					iValidOptions++;
				}		
			}
			if (iValidOptions == 0 && (upgCircleMenu.lastMessageTime + fMessageSilenceTime) < level.time) {
				//Close Menu
				upgCircleMenuCall(upgCircleMenu.active);
				hudPrint("Circle Menu - No options set by script, abborting.\n");
				upgCircleMenu.lastMessageTime = level.time;
			}
			return;
		}
	}
	else {
		sThread = upgCircleMenu.optionDialogThread[iOption];
	}

//gi.Printf(va("upgCircleMenuSelect: %i selected\n", (iOption + 1)));
	
	if (bIsScript) {
		upgPlayerRunThread(sThread);
	}
	else {
		upgPlayerDelayedServerCommand(entnum,va("%s", sThread.c_str()));
	}
	//Close Menu
	upgCircleMenuCall(upgCircleMenu.active);
}

//[b60011] chrissstrahl
//================================================================
// Name:        upgCircleMenuHud
// Class:       -
//              
// Description:	Hides/Shows related circle menu
//              
// Parameters:  Bool Show
//              
// Returns:     -
//              
//================================================================
void Player::upgCircleMenuHud(bool show)
{
	str sMenu;
	str sWidgetName;
	int iSegments;
	str sCommand = "ui_removehud";

	upgCircleMenu.lastWidget = "";

	if (show == true) { sCommand = "ui_addhud"; }

	switch (upgCircleMenu.active)
	{
	case 3:
		sMenu = "coop_circle8";
		iSegments = 8;
		break;
	case 2:
		sMenu = "coop_circleD";
		iSegments = 4;
		break;
	default:
		sMenu = "coop_circle";
		iSegments = 4;
		break;
	}

	for (int i = 0; i < iSegments; i++) {
		G_SendCommandToPlayer(this->edict, va("globalwidgetcommand %s shadercolor 0.5 0.5 0.5 0.8", upgCircleMenuGetWidgetName(i).c_str()));
	}

	gi.SendServerCommand(entnum, va("stufftext \"%s %s\"\n", sCommand.c_str(), sMenu.c_str()));
}

//hzm gameupdate chrissstrahl [b60011]  - starts circle menu
void Player::upgCircleMenuEvent(Event* ev)
{
	//gi.Printf(va("%s -> upgCircleMenuEvent(ev)\n", client->pers.netname));

	if (health <= 0 || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(this, SPECTATOR_TYPE_ANY) || level.cinematic) { return; }

	int iMenuType = ev->GetInteger(1);
	upgCircleMenuCall(iMenuType);
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu for player
void Player::upgCircleMenuSetEvent(Event* ev)
{
	int iOption = ev->GetInteger(1);
	str sText = ev->GetString(2);
	str sThread = ev->GetString(3);
	str sImage = "";
	bool bIsThread = false;
	int iAmmount = 999999;
	int iCost = 0;
	str sCostType = "";
	if (ev->NumArgs() > 3) {
		sImage = ev->GetString(4);
	}
	if (ev->NumArgs() > 4) {
		bIsThread = (bool)ev->GetInteger(5);
	}
	if (ev->NumArgs() > 5) {
		iAmmount = ev->GetInteger(6);
	}
	if (ev->NumArgs() > 7) {
		iCost = ev->GetInteger(7);
		sCostType = ev->GetString(8);
	}

	gi.Printf(va("upgCircleMenuSetEvent[%d]: %s, %s, %s, %d, %d, %d, %s, %s\n", iOption, sText.c_str(), sThread.c_str(),sImage.c_str(),(int)bIsThread,iAmmount,iCost,sCostType.c_str(),client->pers.netname));
	
	upgCircleMenuSet(iOption, sText, sThread, sImage, bIsThread, iAmmount, iCost, sCostType);
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu
void Player::upgCircleMenuSet(int iOption, str sText, str sThread, str sImage, bool bThread, int iAmmount, int iCost, str sCostType)
{
	//range 1 to CIRCLEMENU_MAX_OPTIONS
	if (iOption < 1 || iOption > CIRCLEMENU_MAX_OPTIONS) {
		gi.Printf(va("upgCircleMenuSet: Given Option %i is out of Range\n", iOption));
		return;
	}

	//[b60021] chrissstrahl - disabled - we want to use circlemenu in conjunction with class !ability
	//if (upgCircleMenu.active <= 0) {
		//gi.Printf(va("%s.upgCircleMenuSet() - Can only be used while menu active.\n", targetname.c_str()));
	//}

	if (!sImage.length()) { sImage = "weapons/empty"; }
	if (!sText.length()) { sText = "^"; }
	if (iAmmount == -1) { iAmmount = 999999; }

	int iOptionToArrayNum = (iOption - 1); //make it so that players can start with 1 instead of 0, substract 1

	upgCircleMenu.optionThreadOrCommand[iOptionToArrayNum] = sThread;
	upgCircleMenu.optionText[iOptionToArrayNum] = sText;
	upgCircleMenu.optionIcon[iOptionToArrayNum] = sImage;
	upgCircleMenu.optionIsScript[iOptionToArrayNum] = bThread;
	upgCircleMenu.optionAmmount[iOptionToArrayNum] = iAmmount;
	upgCircleMenu.optionCost[iOptionToArrayNum] = iCost;
	upgCircleMenu.optionCostType[iOptionToArrayNum] = sCostType;

	//gi.Printf("Player::upgCircleMenuSet()->upgCircleMenuGetWidgetName\n");
	str sWidgetName = upgCircleMenuGetWidgetName(iOptionToArrayNum);

	//send commands to menu
	if (upgCircleMenu.optionIconLastSend[iOptionToArrayNum] != sImage) { //[b60021] chrissstrahl - make sure not to resend unnessary data
		upgCircleMenu.optionIconLastSend[iOptionToArrayNum] = sImage;
		upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand %sIcon shader %s", sWidgetName.c_str(), sImage.c_str()));
	}
	else {
		gi.Printf("upgCircleMenuSet already set: %d\n", iOptionToArrayNum);
	}

	//replace withespace and newline to make it work with labeltext

	if (upgCircleMenu.optionTextLastSend[iOptionToArrayNum] != sText) { //[b60021] chrissstrahl - make sure not to resend unnessary data
		upgCircleMenu.optionTextLastSend[iOptionToArrayNum] = sText;
		sText = upgStrings.getReplacedForLabeltext(sText);
		upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand %sText labeltext %s", sWidgetName.c_str(), sText.c_str()));
	}
	else {
		gi.Printf("upgCircleMenuSet already set: %d\n", iOptionToArrayNum);
	}

	//gi.Printf(va("COOPDEBUG - EV_Player_circleMenuSet[%d] %s\n", iOptionToArrayNum, upgCircleMenu.optionThreadOrCommand[iOptionToArrayNum].c_str()));
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu
void Player::upgCircleMenuClearEvent(Event* ev)
{
	int iOption = ev->GetInteger(1);
	upgCircleMenuClear(iOption);
}

//hzm gameupdate chrissstrahl [b60011]  - clears dialog option from circle menu
void Player::upgCircleMenuClear(int iOption)
{
	if (upgCircleMenu.active <= 0) {
		gi.Printf(va("%s.circleMenuClear() - Can only be used while menu active.\n", targetname.c_str()));
		return;
	}

	//ramnge 0 to CIRCLEMENU_MAX_OPTIONS
	if (iOption < 0 || iOption > CIRCLEMENU_MAX_OPTIONS) {
		gi.Printf(va("%s.circleMenuClear() - Out of range: %d.\n", targetname.c_str(), iOption));
		return;
	}

	if (iOption != 0) {
		//reset specific
		upgCircleMenuSet(iOption, "", "", "", false, 999999, 0, "");
	}
	else {
		//reset all
		for (int i = 1; i <= CIRCLEMENU_MAX_OPTIONS; i++) {
			upgCircleMenuSet(i, "", "", "", false, 999999, 0, "");
		}
	}
}




//--------------------------------------------------------------
// CIRCLE MENU - DIALOG - FOR TALKING TO ACTOR
//--------------------------------------------------------------


// hzm gameupdate chrissstrahl[b60011] - adds dialog option to circle menu
void Player::upgCircleMenuDialogSetEvent(Event * ev)
{
	int iOption = ev->GetInteger(1);
	str sText = ev->GetString(2);
	str sThread = ev->GetString(3);
	str sImage = "";
	if (ev->NumArgs() > 3) {
		sImage = ev->GetString(4);
	}
	upgCircleMenuDialogSet(iOption, sText, sThread, sImage);
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu
void Player::upgCircleMenuDialogSet(int iOption, str sText, str sThread, str sImage)
{
	//range 1 to CIRCLEMENU_MAX_OPTIONSDIALOG
	if (iOption < 1 || iOption > CIRCLEMENU_MAX_OPTIONSDIALOG) {
		gi.Printf(va("circleMenuDialogSet: Given Option %i is out of Range\n", iOption));
		return;
	}

	//if (upgCircleMenu.active <= 0) {
		//gi.Printf(va("%s.upgCircleMenuDialogSet() - Can only be used while menu active.\n", targetname.c_str()));
	//}

	int iOptionToArrayNum = (iOption - 1); //make it so that players can start with 1 instead of 0, substract 1
	upgCircleMenu.optionDialogThread[iOptionToArrayNum] = sThread;
	upgCircleMenu.optionDialogText[iOptionToArrayNum] = sText;
	upgCircleMenu.optionDialogIcon[iOptionToArrayNum] = sImage;

	if (!sImage.length()) { sImage = "weapons/empty"; }
	if (!sText.length()) { sText = "^"; }

	//gi.Printf("Player::upgCircleMenuDialogSet()->upgCircleMenuGetWidgetName\n");
	str sWidgetName = upgCircleMenuGetWidgetName(iOptionToArrayNum);

	//send commands to menu
	upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand %sIcon shader %s", sWidgetName.c_str(), sImage.c_str()));
	sText = upgStrings.getReplacedForLabeltext(sText);
	upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand %sText labeltext %s", sWidgetName.c_str(), sText.c_str()));
}

//hzm gameupdate chrissstrahl [b60011]  - clears dialog options from circle menu
void Player::upgCircleMenuDialogClearEvent(Event* ev)
{
	int iOption = ev->GetInteger(1);
	upgCircleMenuDialogClear(iOption);
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu for player
void Player::upgCircleMenuDialogClear(int iOption)
{
	if (upgCircleMenu.active <= 0) {
		gi.Printf(va("%s.circleMenuDialogClear() - Can only be used while menu active.\n", targetname.c_str()));
		return;
	}

	//range 1 to CIRCLEMENU_MAX_OPTIONSDIALOG
	if (iOption < 0 || iOption > CIRCLEMENU_MAX_OPTIONSDIALOG) {
		gi.Printf(va("%s.circleMenuDialogClear() - Out of range: %d.\n", targetname.c_str(), iOption));
		return;
	}

	if (iOption != 0) {
		//reset specific
		upgCircleMenuDialogSet(iOption, "", "", "");
	}
	else {
		//reset all
		for (int i = 1; i <= CIRCLEMENU_MAX_OPTIONSDIALOG; i++) {
			upgCircleMenuDialogSet(i, "", "", "");
		}
	}
}