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


//[b60011] chrissstrahl
//================================================================
// Name:        switchWidgets
// Class:       -
//              
// Description: This sends a command to enable/disable a widget in a single burst
//              
// Parameters:  str widget1, str widget2, str widget1Cmd, str widget2Cmd
//              
// Returns:     -
//              
//================================================================
void Player::circleMenuSwitchWidgets(str widget1, str widget2, str widget1Cmd, str widget2Cmd)
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
// Name:        circleMenuIsActive
// Class:       -
//              
// Description: Checks if circleMenu is active for player
//              
// Parameters:  -
//              
// Returns:     bool
//              
//================================================================
bool Player::circleMenuIsActive()
{
	if (upgCircleMenu.active > 0) {
		return true;
	}

	return false;
}

//[b60011] chrissstrahl
//================================================================
// Name:        circleMenuLastTimeActive
// Class:       -
//              
// Description: Returns level time at which the menus was last active
//              
// Parameters:  -
//              
// Returns:     float
//              
//================================================================
float Player::circleMenuLastTimeActive()
{
	if (upgCircleMenu.activatingTime > upgCircleMenu.thinkTime) {
		return upgCircleMenu.activatingTime;
	}
	return upgCircleMenu.thinkTime;
}

//[b60011] chrissstrahl
//================================================================
// Name:        circleMenu
// Class:       -
//              
// Description: Activated/Deactivates the circle menu
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
void Player::circleMenu(int iType)
{
	//prevent premature re-opening
	if (upgCircleMenu.active <= 0 && (upgCircleMenu.activatingTime + 0.5) >= level.time || level.cinematic || getHealth() <= 0 || multiplayerManager.isPlayerSpectator(this)) {
		//hudPrint("c open/close tosoon\n");
		return;
	}

	//make sure bad value is catched
	if (iType <= 0 || iType > 2) {
		gi.Printf("player::circleMenu( %i ) <- unsupported Menu Type number in parameter 1\n", iType);
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
//hudPrint("c start\n");

		//reset holding buttons - precent acidental activation
		upgCircleMenu.holdingLeftButton = true;
		upgCircleMenu.holdingRightButton = true;

		upgCircleMenu.lastSegment = 0; //reset selected segment

		upgCircleMenu.viewAngle = getViewAngles();
		upgCircleMenu.lastViewangle = upgCircleMenu.viewAngle;

		upgCircleMenu.active = iType;
		circleMenuHud(true);

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
//hudPrint("c end\n");
		upgPlayerDelayedServerCommand(entnum, "-attackLeft");
		
		circleMenuHud(false);
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
// Name:        circleMenuGetWidgetName();
// Class:       -
//              
// Description:	Calculates the desired move direction based on the last direction moves
//              
// Parameters:  int iSegment
//              
// Returns:     Circle Menu Widgetname for given direction
//              
//================================================================
str Player::circleMenuGetWidgetName(int iSegment)
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
int Player::circleMenuGetSegmentNumForAngle(float fAngle)
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
// Name:        circleMenuThink
// Class:       -
//              
// Description:	Checks if circle menu is active and manages interactions
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
void Player::circleMenuThink()
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
	
//gi.Printf(va("Player::circleMenuThink()->circleMenuGetWidgetName %i - %f\n", upgCircleMenu.active, upgCircleMenu.thinkTime));

	//make sure it can not be abused by spec
	if (this->getHealth() <= 0 || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(this)) {
		if (upgCircleMenu.active) {
			circleMenuHud(false);
			upgCircleMenu.active = 0;
		}
		return;
	}

	//player is clicking fire
	if (last_ucmd.buttons & BUTTON_ATTACKLEFT) {
		if (!upgCircleMenu.holdingLeftButton) {
			upgCircleMenu.holdingLeftButton = true;
//hudPrint("c select\n");
			upgCircleMenu.thinkTime = level.time;
			circleMenuSelect(upgCircleMenu.lastSegment);
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
			return;
		}

		upgCircleMenu.holdingRightButton = true;
		fSegmentNum = (upgCircleMenu.lastSegment + 1);
		if (fSegmentNum >= 4) {
			fSegmentNum = 0;
		}
		//hudPrint(va("c next %d\n", fSegmentNum));
	}
	else {
		upgCircleMenu.holdingRightButton = false;
	}

	sWidgetName = circleMenuGetWidgetName(fSegmentNum);
	//gi.Printf("Player::circleMenuThink()->circleMenuGetWidgetName\n");

	if (sWidgetName != "" && sWidgetName != upgCircleMenu.lastWidget) {
		str sCmd;
		G_SendCommandToPlayer(this->edict, va("globalwidgetcommand %s shadercolor 0 0 0 1", sWidgetName.c_str()));
		G_SendCommandToPlayer(this->edict, va("globalwidgetcommand %s shadercolor 1 1 1 1", upgCircleMenu.lastWidget.c_str()));
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
	fSegmentNum = circleMenuGetSegmentNumForAngle(fAngle);


//gi.Printf("Player::circleMenuThink()->circleMenuGetWidgetName\n");
	sWidgetName = circleMenuGetWidgetName(fSegmentNum);

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
// Name:        circleMenuSelect
// Class:       -
//              
// Description:	Manages if the player has selected a item on the circle menu
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
void Player::circleMenuSelect(int iOption)
{
	if (iOption < 0 || iOption >= CIRCLEMENU_MAX_OPTIONS) {
		gi.Printf(va("circleMenuSelect: Given Option %d is out of Range for Client[%d]\n", iOption,entnum));
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
				gi.Printf(va("circleMenuSelect: Given Option %d ammount < 1 for Client[%d]\n", iOption, entnum));
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
				circleMenu(upgCircleMenu.active);
				hudPrint("Circle Menu - No options set by script, abborting.\n");
				upgCircleMenu.lastMessageTime = level.time;
			}
			return;
		}
	}
	else {
		sThread = upgCircleMenu.optionDialogThread[iOption];
	}

//gi.Printf(va("circleMenuSelect: %i selected\n", (iOption + 1)));
	
	if (bIsScript) {
		upgPlayerRunThread(sThread);
	}
	else {
		upgPlayerDelayedServerCommand(entnum,va("%s", sThread.c_str()));
	}
	//Close Menu
	circleMenu(upgCircleMenu.active);
}

//[b60011] chrissstrahl
//================================================================
// Name:        circleMenuHud
// Class:       -
//              
// Description:	Hides/Shows related circle menu
//              
// Parameters:  Bool Show
//              
// Returns:     -
//              
//================================================================
void Player::circleMenuHud(bool show)
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
		G_SendCommandToPlayer(this->edict, va("globalwidgetcommand %s shadercolor 0.5 0.5 0.5 0.8", circleMenuGetWidgetName(i)));
	}

	gi.SendServerCommand(entnum, va("stufftext \"%s %s\"\n", sCommand.c_str(), sMenu.c_str()));
}

//hzm gameupdate chrissstrahl [b60011]  - starts circle menu
void Player::circleMenuEvent(Event* ev)
{
	if (health <= 0 || multiplayerManager.inMultiplayer() && multiplayerManager.isPlayerSpectator(this, SPECTATOR_TYPE_ANY)) { return; }

	int iMenuType = ev->GetInteger(1);
	circleMenu(iMenuType);
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu
void Player::circleMenuDialogSetEvent(Event* ev)
{
	int iOption = ev->GetInteger(1);
	str sText = ev->GetString(2);
	str sThread = ev->GetString(3);
	str sImage = "";
	if (ev->NumArgs() > 3) {
		sImage = ev->GetString(4);
	}
	circleMenuDialogSet(iOption, sText, sThread, sImage);
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu
void Player::circleMenuDialogSet(int iOption, str sText, str sThread, str sImage)
{
	//range 1 to CIRCLEMENU_MAX_OPTIONSDIALOG
	if (iOption < 1 || iOption > CIRCLEMENU_MAX_OPTIONSDIALOG) {
		gi.Printf(va("circleMenuDialogSet: Given Option %i is out of Range\n", iOption));
		return;
	}
	
	//if (upgCircleMenu.active <= 0) {
		//gi.Printf(va("%s.circleMenuDialogSet() - Can only be used while menu active.\n", targetname.c_str()));
	//}

	int iOptionToArrayNum = (iOption - 1); //make it so that players can start with 1 instead of 0, substract 1
	upgCircleMenu.optionDialogThread[iOptionToArrayNum] = sThread;
	upgCircleMenu.optionDialogText[iOptionToArrayNum] = sText;
	upgCircleMenu.optionDialogIcon[iOptionToArrayNum] = sImage;

	if (!sImage.length()) { sImage = "weapons/empty"; }
	if (!sText.length()) { sText = "^"; }

	//gi.Printf("Player::circleMenuDialogSet()->circleMenuGetWidgetName\n");
	str sWidgetName = circleMenuGetWidgetName(iOptionToArrayNum);

	//send commands to menu
	upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand %sIcon shader %s", sWidgetName.c_str(), sImage.c_str()));
	sText = upgStrings.getReplacedForLabeltext(sText);
	upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand %sText labeltext %s", sWidgetName.c_str(), sText.c_str()));
}

//hzm gameupdate chrissstrahl [b60011]  - clears dialog options from circle menu
void Player::circleMenuDialogClearEvent(Event* ev)
{
	int iOption = ev->GetInteger(1);
	circleMenuDialogClear(iOption);
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu for player
void Player::circleMenuDialogClear(int iOption)
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
		circleMenuDialogSet(iOption, "", "", "");
	}
	else {
		//reset all
		for (int i = 1; i <= CIRCLEMENU_MAX_OPTIONSDIALOG; i++) {
			circleMenuDialogSet(i, "", "", "");
		}
	}
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu for player
void Player::circleMenuSetEvent(Event* ev)
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
		sCostType = ev->GetInteger(8);
	}
	circleMenuSet(iOption, sText, sThread, sImage, bIsThread, iAmmount, iCost, sCostType);
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu
void Player::circleMenuSet(int iOption, str sText, str sThread, str sImage, bool bThread, int iAmmount, int iCost, str sCostType)
{
	//range 1 to CIRCLEMENU_MAX_OPTIONS
	if (iOption < 1 || iOption > CIRCLEMENU_MAX_OPTIONS) {
		gi.Printf(va("circleMenuSet: Given Option %i is out of Range\n", iOption));
		return;
	}

	//[b60021] chrissstrahl - disabled - we want to use circlemenu in conjunction with class !ability
	//if (upgCircleMenu.active <= 0) {
		//gi.Printf(va("%s.circleMenuSet() - Can only be used while menu active.\n", targetname.c_str()));
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

	//gi.Printf("Player::circleMenuSet()->circleMenuGetWidgetName\n");
	str sWidgetName = circleMenuGetWidgetName(iOptionToArrayNum);

	//send commands to menu
	upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand %sIcon shader %s", sWidgetName.c_str(), sImage.c_str()));
	//replace withespace and newline to make it work with labeltext
	sText = upgStrings.getReplacedForLabeltext(sText);
	upgPlayerDelayedServerCommand(entnum, va("globalwidgetcommand %sText labeltext %s", sWidgetName.c_str(), sText.c_str()));
}

//hzm gameupdate chrissstrahl [b60011]  - adds dialog option to circle menu
void Player::circleMenuClearEvent(Event* ev)
{
	int iOption = ev->GetInteger(1);
	circleMenuClear(iOption);
}

//hzm gameupdate chrissstrahl [b60011]  - clears dialog option from circle menu
void Player::circleMenuClear(int iOption)
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
		circleMenuSet(iOption, "", "", "", false, 999999, 0, "");
	}
	else {
		//reset all
		for (int i = 1; i <= CIRCLEMENU_MAX_OPTIONS; i++) {
			circleMenuSet(i, "", "", "", false, 999999, 0, "");
		}
	}
}