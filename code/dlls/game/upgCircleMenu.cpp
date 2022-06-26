#include "_pch_cpp.h"
#include "upgCircleMenu.hpp"
#include "player.h"
#include "gamecmds.h"


//[b611] chrissstrahl
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

//[b611] chrissstrahl
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

//[b611] chrissstrahl
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

//[b611] chrissstrahl
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

//[b611] chrissstrahl
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

//[b611] chrissstrahl
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
	if (upgCircleMenu.active <= 0 || upgCircleMenu.thinkTime > level.time || health <= 0) {
		return;
	}
	//make sure it can not be abused by spec
	if (multiplayerManager.inMultiplayer() && (!multiplayerManager.isFightingAllowed() || multiplayerManager.isPlayerSpectator(this))){
		return;
	}

	//player is clicking fire
	if (last_ucmd.buttons & BUTTON_ATTACKLEFT) {
		circleMenuSelect(upgCircleMenu.lastSegment);
		upgCircleMenu.active = 0;
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
	str sPrint = va("prev: %s curr: %s\n", upgCircleMenu.lastWidget.c_str(), sWidgetName.c_str());
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

//[b611] chrissstrahl
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
		gi.Printf(va("circleMenuSelect: Given Option %i is out of Range\n", iOption));
		return;
	}

	bool bIsScript = upgCircleMenu.optionIsScript[iOption];
	str sThread		= upgCircleMenu.optionThreadOrCommand[iOption];

	gi.Printf(va("circleMenuSelect: %i selected\n", (iOption + 1)));
	
	if (bIsScript) {
		RunThread(sThread);
	}
	else {
		DelayedServerCommand(entnum,va("%s", sThread.c_str()));
	}
	circleMenuHud(false);
}

//[b611] chrissstrahl
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