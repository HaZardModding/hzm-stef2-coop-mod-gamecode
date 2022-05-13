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

	upgCircleMenu.activatingTime = level.time;

	if (upgCircleMenu.active <= 0) {
		upgCircleMenu.viewAngle = getViewAngles();
		upgCircleMenu.lastAngle = upgCircleMenu.viewAngle;

		upgCircleMenu.active = iType;
		if (upgCircleMenu.active == 1) {
			gi.SendServerCommand(entnum, "stufftext \"ui_addhud coop_circle\"\n");
		}
		else if (upgCircleMenu.active == 2) {
			gi.SendServerCommand(entnum, "stufftext \"ui_addhud coop_circle\"\n");
		}
	}
	else {
		gi.SendServerCommand(entnum, "stufftext \"ui_removehud coop_circle\"\n");
		upgCircleMenu.active = 0;
	}
}

//[b611] chrissstrahl
//================================================================
// Name:        circleMenuCalculateDirection();
// Class:       -
//              
// Description:	
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
int Player::circleMenuDetermineDirection()
{
	//0 + = down
	//0 - = up
	//1 - = left
	//1 + right
	//1 179+ to 0 to -179
	Vector plAngle = Vector(0, 0, 0);
	plAngle[0] = SHORT2ANGLE(current_ucmd->angles[0]);
	plAngle[1] = SHORT2ANGLE(current_ucmd->angles[1]);

	Vector vLast = upgCircleMenu.lastAngle;
	upgCircleMenu.lastAngle = plAngle;

	//gi.Printf(va("result %f %f %f\n", plAngle[0], plAngle[1], plAngle[2]));

	//0=up,1=right,2=down,3=left
	int iDirection = -1;
	//up or down or equal
	if (plAngle[0] != vLast[0]) {
		//up
		if (plAngle[0] < vLast[0]) {
			//gi.Printf("up\n");
			iDirection = 0;
		}
		//down
		else {
			//gi.Printf("down\n");
			iDirection = 2;
		}
	}

	//left or right or equal
	if (plAngle[1] != vLast[1]) {
		//right
		if (plAngle[1] < vLast[1]) {
			//gi.Printf("right\n");
			iDirection = 1;
		}
		//left
		else {
			//gi.Printf("left\n");
			iDirection = 3;
		}
	}

	return iDirection;
}

//[b611] chrissstrahl
//================================================================
// Name:        circleMenuCalculateDirection();
// Class:       -
//              
// Description:	
//              
// Parameters:  -
//              
// Returns:     -
//              
//================================================================
int Player::circleMenuCalculateDirection(int& up, int& down, int& left, int& right)
{
	bool bUp = false, bLeft = false;
	//check if player moved more up or down
	if (up > down) {
		bUp = true;
	}
	if (left > right) {
		bLeft = true;
	}

	//0=up,1=right,2=down,3=left
	int iDirection = -1;
	if (bUp && bLeft) {
		if (up > left) {
			iDirection = 0; //up
		}
		else if (up < left) {
			iDirection = 3; //left
		}
		else {
			//equal
		}
	}
	else if (!bUp && !bLeft) {
		if (down > right) {
			iDirection = 2; //up
		}
		else if (right < down) {
			iDirection = 1; //right
		}
		else {
			//equal
		}
	}
	return iDirection;
}

//[b611] chrissstrahl
//================================================================
// Name:        circleMenuGetOption();
// Class:       -
//              
// Description:	Returns the option number for each direction (up,left,down,right)
//              
// Parameters:  -
//              
// Returns:     Circle Menu Widgetname for given direction
//              
//================================================================
int Player::circleMenuGetOption(int iDirection)
{
	//DIRECTION:	//0=up,1=right,2=down,3=left
	//OPTION:		//1=up,2=left,3=down,4=right
	int iOption = 0;
	if (iDirection == 0) { iOption = 1; }
	else if (iDirection == 1) { iOption = 2; }
	else if (iDirection == 2) { iOption = 3; }
	else if (iDirection == 3) { iOption = 4; }
	return iDirection;
}

//[b611] chrissstrahl
//================================================================
// Name:        circleMenuGetWidgetName();
// Class:       -
//              
// Description:	Calculates the desired move direction based on the last direction moves
//              
// Parameters:  -
//              
// Returns:     Circle Menu Widgetname for given direction
//              
//================================================================
str Player::circleMenuGetWidgetName(int iDirection)
{
	//0=up,1=right,2=down,3=left
	str currentWidget = "";
	if (iDirection == 0) { currentWidget = "coop_circleT"; }
	else if (iDirection == 1) { currentWidget = "coop_circleR"; }
	else if (iDirection == 2) { currentWidget = "coop_circleB"; }
	else if (iDirection == 3) { currentWidget = "coop_circleL"; }
	else {
		gi.Printf(va("Player::circleMenuGetWidgetName(%i) - No widget name avialable for parameter 1\n", iDirection));
		return "";
	}
	return currentWidget;
}

//[b611] chrissstrahl
//================================================================
// Name:        circleMenuCalculateDirectionFinal();
// Class:       -
//              
// Description:	Calculates the desired move direction based on the last direction moves
//              
// Parameters:  -
//              
// Returns:     Indiana-Jones
//              
//================================================================
int Player::circleMenuCalculateDirectionFinal()
{
	bool bUp = false;
	bool bLeft = false;
	int iDirection = -1;

	int iUpDown = upgCircleMenu.moveDown;
	int iLeftRight = upgCircleMenu.moveRight;

	//grab most move dir up/down left/right
	if (upgCircleMenu.moveUp > upgCircleMenu.moveDown) {
		iUpDown = upgCircleMenu.moveUp;
		bUp = true;
	}
	if (upgCircleMenu.moveLeft > upgCircleMenu.moveRight) {
		iLeftRight = upgCircleMenu.moveLeft;
		bLeft = true;
	}

	str currentWidget = "";

	//movement did go up or down
	//0=up,1=right,2=down,3=left
	if (iUpDown > iLeftRight) {
		if (bUp) {
			iDirection = 0;
			currentWidget = "coop_circleT";
		}
		else {
			iDirection = 2;
			currentWidget = "coop_circleB";
		}
	}
	//movement did go left or right
	else {
		if (bLeft) {
			iDirection = 3;
			currentWidget = "coop_circleL";
		}
		else {
			iDirection = 1;
			currentWidget = "coop_circleR";
		}
	}

	upgCircleMenu.moveUp = 0;
	upgCircleMenu.moveDown = 0;
	upgCircleMenu.moveLeft = 0;
	upgCircleMenu.moveRight = 0;
	return iDirection;
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
	if (upgCircleMenu.active <= 0 || upgCircleMenu.thinkTime > level.time) {
		return;
	}

	str currentWidget = "";
	Vector plAngle = Vector(0, 0, 0);
	plAngle[0] = SHORT2ANGLE(current_ucmd->angles[0]);
	plAngle[1] = SHORT2ANGLE(current_ucmd->angles[1]);

	int iDirection = circleMenuDetermineDirection();

	//no change
	if (iDirection == -1) {
		return;
	}

	//0=up,1=right,2=down,3=left
	if (iDirection == 0) {
		//gi.Printf("up 0\n");
		upgCircleMenu.moveUp++;
	}
	else if (iDirection == 2) {
		//gi.Printf("down 2\n");
		upgCircleMenu.moveDown++;
	}
	else if (iDirection == 3) {
		//gi.Printf("left 3\n");
		upgCircleMenu.moveLeft++;
	}
	else if (iDirection == 1) {
		//gi.Printf("right 1\n");
		upgCircleMenu.moveRight++;
	}

	//printout what option was selected - close menu
	if ((last_ucmd.buttons & BUTTON_ATTACKLEFT) != 0) {
		int iOption;
		iDirection = circleMenuCalculateDirectionFinal();
		iOption = circleMenuGetOption(iDirection);
		gi.Printf("Circle Menu OPTION SELECTED: %i\n", iOption);
		gi.SendServerCommand(entnum, "stufftext \"ui_removehud coop_circle\"\n");
		upgCircleMenu.active = 0;
		return;
	}

	//calculate direction on a time based cycle - proceed if the fire button is pressed
	if ((upgCircleMenu.thinkTime + 0.1) > level.time) {
		return;
	}
	upgCircleMenu.thinkTime = level.time;

	iDirection = circleMenuCalculateDirectionFinal();
	currentWidget = circleMenuGetWidgetName(iDirection);

	//reset viewangle
//SetViewAngles(upgCircleMenu.viewAngle);

	gi.Printf(va("viewangle %f %f %f\n", upgCircleMenu.viewAngle[0], upgCircleMenu.viewAngle[1], upgCircleMenu.viewAngle[2]));

	if (currentWidget.length() && currentWidget != upgCircleMenu.lastWidget) {
		switchWidgets(currentWidget, upgCircleMenu.lastWidget, "bgcolor 0.5 0.5 0.5 1", "bgcolor 0.5 0.5 0.5 0");
		upgCircleMenu.lastWidget = currentWidget;
	}

	//--prevent player view moving with it
}


