//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
//
// Circle Menu Related Code, used to add New Mewnu Type to the game, like known from modern games.
//-----------------------------------------------------------------------------------

#include "_pch_cpp.h"
#include "upgCircleMenu.hpp"
#include "player.h"
#include "gamecmds.h"

UpgCircleMenu upgCircleMenu;

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
void Player::switchWidgets(str widget1, str widget2, str widget1Cmd, str widget2Cmd)
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
// Description: Checks if circleMenu is actuivate on player
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
	//prevent premature closing/opening
	if ((upgCircleMenu.activatingTime + 0.3) >= level.time) {
		return;
	}

	//make sure bad value is catched
	if (iType <= 0 || iType > 2) {
		gi.Printf("player::circleMenu( %i ) <- unsupported Menu Type number in parameter 1\n", iType);
		return;
	}

	Event* StopFireEvent;
	StopFireEvent = new Event(EV_Sentient_StopFire);
	StopFireEvent->AddString("dualhand");
	this->ProcessEvent(StopFireEvent);

	upgCircleMenu.numOfSegments = 4;

	upgCircleMenu.activatingTime = level.time;

	if (upgCircleMenu.active <= 0) {
		upgCircleMenu.viewAngle = getViewAngles();
		upgCircleMenu.lastViewangle = upgCircleMenu.viewAngle;

		upgCircleMenu.active = iType;
		circleMenuHud(true);

		disableInventory();
		disableUseWeapon(true);
	}
	else {
		circleMenuHud(false);
		upgCircleMenu.active = 0;

		enableInventory();
		disableUseWeapon(false);
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
int Player::getSegmentNumForAngle(float fAngle)
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
	if (upgCircleMenu.active <= 0 || upgCircleMenu.thinkTime > level.time) {
		return;
	}
	
gi.Printf(va("Player::circleMenuThink()->circleMenuGetWidgetName %i - %f\n", upgCircleMenu.active, upgCircleMenu.thinkTime));

	//make sure it can not be abused by spec
	if (this->getHealth() <= 0 || multiplayerManager.inMultiplayer() && !multiplayerManager.isFightingAllowed() || multiplayerManager.isPlayerSpectator(this)) {
		if (upgCircleMenu.active) {
			circleMenuHud(false);
			upgCircleMenu.active = 0;
		}
		return;
	}

	//player is clicking fire
	if (last_ucmd.buttons & BUTTON_ATTACKLEFT) {
		upgCircleMenu.thinkTime = level.time;
		circleMenuSelect(upgCircleMenu.lastSegment);
		return;
	}

	//detect and record the mouse move directions
	Vector vDifference = Vector(0, 0, 0);
	Vector vViewangle = Vector(0, 0, 0);
	GetPlayerView(NULL, &vViewangle);

	//if all the same, we can abbort ?
	if (vViewangle == upgCircleMenu.lastViewangle) { return; }

	//get difference - remember last viewangle
	vDifference = (upgCircleMenu.lastViewangle - vViewangle);
	upgCircleMenu.lastViewangle = vViewangle;

	upgCircleMenu.longtimeViewangle[0] += vDifference[0];
	upgCircleMenu.longtimeViewangle[1] += vDifference[1];

	//angle on 2d screen - circle menu
	float fAngle;

	if ((upgCircleMenu.thinkTime + 0.1) > level.time) { return; }
	upgCircleMenu.thinkTime = level.time;

	//here is where the magic happens
	float radians = atan2(upgCircleMenu.longtimeViewangle[1], upgCircleMenu.longtimeViewangle[0]);
	float degrees = RAD2DEG(radians);
	fAngle = AngleNormalize360( degrees );

	float fSegmentNum;
	str sWidgetName;
	fSegmentNum = getSegmentNumForAngle(fAngle);
gi.Printf("Player::circleMenuThink()->circleMenuGetWidgetName\n");
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
	*/
	upgCircleMenu.lastSegment = fSegmentNum;
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
		return;
	}

	bool bIsScript = upgCircleMenu.optionIsScript[iOption];
	str sThread		= upgCircleMenu.optionThreadOrCommand[iOption];

	//make sure player has items
	if (upgCircleMenu.optionAmmount[iOption] < 1) {
		gi.Printf(va("circleMenuSelect: Given Option %d ammount < 1 for Client[%d]\n", iOption, entnum));
		return;
	}

//gi.Printf(va("circleMenuSelect: %i selected\n", (iOption + 1)));
	
	if (bIsScript) {
		RunThread(sThread);
	}
	else {
		DelayedServerCommand(entnum,va("%s", sThread.c_str()));
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
	str sCommand = "ui_removehud";

	upgCircleMenu.lastWidget = "";

	if (show == true) { sCommand = "ui_addhud"; }

	switch (upgCircleMenu.active)
	{
	case 3:
		sMenu = "coop_circle8";
		break;
	case 2:
		sMenu = "coop_circleD";
		break;
	default:
		sMenu = "coop_circle";
		break;
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
	if (iOption < 0 || iOption >= CIRCLEMENU_MAX_OPTIONSDIALOG) {
		gi.Printf(va("circleMenuDialogSet: Given Option %i is out of Range\n"));
		return;
	}
	if (upgCircleMenu.active <= 0) {
		gi.Printf(va("%s.circleMenuDialogSet() - Can only be used while menu active.\n", targetname.c_str()));
	}
	upgCircleMenu.optionDialogThread[iOption] = sThread;
	upgCircleMenu.optionDialogText[iOption] = sText;
	upgCircleMenu.optionDialogIcon[iOption] = sImage;

	if (!sImage.length()) { sImage = "weapons/empty"; }
	if (!sText.length()) { sText = "^"; }

gi.Printf("Player::circleMenuDialogSet()->circleMenuGetWidgetName\n");
	str sWidgetName = circleMenuGetWidgetName(iOption);

	//send commands to menu
	DelayedServerCommand(entnum, va("globalwidgetcommand %sIcon shader %s", sWidgetName.c_str(), sImage.c_str()));
	sText = coop_replaceForLabelText(sText);
	DelayedServerCommand(entnum, va("globalwidgetcommand %sText labeltext %s", sWidgetName.c_str(), sText.c_str()));
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

	if (iOption < 0 || iOption > CIRCLEMENU_MAX_OPTIONSDIALOG) {
		gi.Printf(va("%s.circleMenuDialogClear() - Out of range: %d.\n", targetname.c_str(), iOption));
		return;
	}

	if (iOption != 0) {
		circleMenuDialogSet(iOption, "", "", "");
	}
	else {
		for (int i = 0; i < CIRCLEMENU_MAX_OPTIONSDIALOG; i++) {
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
	//correct offset
	iOption = (iOption - 1);

	if (iOption < 0 || iOption >= CIRCLEMENU_MAX_OPTIONS) {
		gi.Printf(va("circleMenuSet: Given Option %i is out of Range\n", iOption));
		return;
	}
	if (upgCircleMenu.active <= 0) {
		gi.Printf(va("%s.circleMenuSet() - Can only be used while menu active.\n", targetname.c_str()));
	}

	if (!sImage.length()) { sImage = "weapons/empty"; }
	if (!sText.length()) { sText = "^"; }
	if (iAmmount == -1) { iAmmount = 999999; }

	upgCircleMenu.optionThreadOrCommand[iOption] = sThread;
	upgCircleMenu.optionText[iOption] = sText;
	upgCircleMenu.optionIcon[iOption] = sImage;
	upgCircleMenu.optionIsScript[iOption] = bThread;
	upgCircleMenu.optionAmmount[iOption] = iAmmount;
	upgCircleMenu.optionCost[iOption] = iCost;
	upgCircleMenu.optionCostType[iOption] = sCostType;

gi.Printf("Player::circleMenuSet()->circleMenuGetWidgetName\n");
	str sWidgetName = circleMenuGetWidgetName(iOption);

	//send commands to menu
	DelayedServerCommand(entnum, va("globalwidgetcommand %sIcon shader %s", sWidgetName.c_str(), sImage.c_str()));
	//replace withespace and newline to make it work with labeltext
	sText = coop_replaceForLabelText(sText);
	DelayedServerCommand(entnum, va("globalwidgetcommand %sText labeltext %s", sWidgetName.c_str(), sText.c_str()));
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

	if (iOption < 0 || iOption > CIRCLEMENU_MAX_OPTIONS) {
		gi.Printf(va("%s.circleMenuClear() - Out of range: %d.\n", targetname.c_str(), iOption));
		return;
	}

	if (iOption != 0) {
		circleMenuSet(iOption, "", "", "", false, 999999, 0, "");
	}
	else {
		for (int i = 0; i < CIRCLEMENU_MAX_OPTIONS; i++) {
			circleMenuSet(i, "", "", "", false, 999999, 0, "");
		}
	}
}