//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// Circle Menu Related Code, used to add New Mewnu Type to the game, like known from modern games.
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING TEXT AND LOCALIZING RELATED FUNCTIONS FOR THE HZM CO-OP MOD

//HAZARDMODDING CO-OP SCRIPT MODIFICATION ©2006-2022 SOME RIGHTS RESERVED AND
//PRIMARY (IP)INTELLECTUAL PROPERTY ON THE HZM COOP MOD HELD BY CHRISTIAN SEBASTIAN STRAHL, ALIAS CHRISSSTRAHL.

//YOU ARE EXPLICITE FORBIDDEN TO PUBLISH A MODIFIED VARIANT OF THIS CODE,
//ANY MATERIALS OR INTELLECTUAL PROPERTY OF THIS FILE WITHOUT THE EXPLICIT
//WRITTEN PERMISSION OF THE RESPECTIVE OWNERS!

//YOU MAY USE CODE PARTS AS LONG AS THEY DO NOT COMPROMISE THE GAME SAFTY
//LOCAL AND INTERNATIONAL LAWS, AS WELL AS VIOLATE UPON THE ENDCLIENT ITS PRIVACY

//CONTACT: chrissstrahl@yahoo.de [Christian Sebastian Strahl, Germany]
//[b611] Chrissstrahl

#ifndef __UPGCIRCLEMENU_HPP__
#define __UPGCIRCLEMENU_HPP__

#include "_pch_cpp.h"
#include "player.h"
#include "upgCircleMenu.hpp"

#define CIRCLEMENU_STARTNUM 0
#define CIRCLEMENU_MAX_OPTIONS 4
#define CIRCLEMENU_MAX_OPTIONSDIALOG 4

class UpgCircleMenu
{
public:
	int		active = 0;
	int		numOfSegments = 0;
	//viewangle - stored upon opening the menu
	Vector	viewAngle = Vector(0, 0, 0);
	//last time the menu did think
	float	thinkTime = 0.0f;
	//time when the menu was activated - used to prevent premature opening/closeing
	float	activatingTime = 0.0f;
	//remember stuff so we can compare
	Vector	lastViewangle = Vector(0, 0, 0);
	Vector	longtimeViewangle = Vector(0, 0, 0);
	str		lastWidget = "";
	int		lastSegment = -1;
	int		currentSegment = -1;

	//circlemenu options
	str		optionThreadOrCommand	[CIRCLEMENU_MAX_OPTIONS];
	str		optionText				[CIRCLEMENU_MAX_OPTIONS];
	str		optionIcon				[CIRCLEMENU_MAX_OPTIONS];
	bool	optionIsScript			[CIRCLEMENU_MAX_OPTIONS];

	//circlemenu dialog options  - used for dialog selection
	str		optionDialogThread	[CIRCLEMENU_MAX_OPTIONSDIALOG];
	str		optionDialogText	[CIRCLEMENU_MAX_OPTIONSDIALOG];
	str		optionDialogIcon	[CIRCLEMENU_MAX_OPTIONSDIALOG];
};


//UpgCircleMenu::circleMenu();

#endif /* !__UPGCIRCLEMENU_HPP__ */



/*

void main();
float noNegativity(float fNumber);
float noNegativValues(float fNumber);
void playerAnglez();

float fNumSegments;
float fSegmentDegreeSize;
float fSegmentMaxEnd;


//if(sPrevWidget != "" && sWidgetName != sPrevWidget){
//print("reset widget ("+sPrevWidget+")\n");
//sendclientcommand($player,"globalwidgetcommand "+sPrevWidget+" shadercolor 1 1 1 1");
//}

string getWidgetNameForSegment(float fSegmentNumber)
{
	string sWidgetName;
	if (fSegmentNumber == 0) { sWidgetName = "coop_circleT"; }
	else if (fSegmentNumber == 1) { sWidgetName = "coop_circleR"; }
	else if (fSegmentNumber == 2) { sWidgetName = "coop_circleB"; }
	else if (fSegmentNumber == 3) { sWidgetName = "coop_circleL"; }
	else { sWidgetName = ""; }
	return "" + sWidgetName;
}

float getSegmentNumForAngle(float fAngle)
{
	//go step whise in reverse
	float iStep;
	float iHighestSegmentNum;
	float fCurrentMax;
	fCurrentMax = fSegmentMaxEnd;
	iHighestSegmentNum = (fNumSegments - 1);

	for (iStep = iHighestSegmentNum; iStep >= 0; iStep--) {
		//print("getSegmentNumForAngle: '"+fAngle+"'\n");
		//if(fAngle != 0){
			//print("getSegmentNumForAngle: '"+fAngle+"'\n");
			//print("getSegmentNumForAngle: "+fCurrentMax+" vs "+(fCurrentMax - fSegmentDegreeSize)+"\n");
		//}
		if (fCurrentMax > fAngle && fAngle > (fCurrentMax - (fSegmentDegreeSize))) {
			return iStep;
		}
		fCurrentMax = noNegativity(fCurrentMax - fSegmentDegreeSize);
	}
	return -1;
}

void main()
{
	fNumSegments = 4;
	fSegmentDegreeSize = (360 / fNumSegments);
	fSegmentMaxEnd = (359 - (fSegmentDegreeSize / 2));

	//$player.circleMenu();
	wait(3);

	print("fSegmentDegreeSize " + fSegmentDegreeSize + "\n");
	print("fSegmentMaxEnd " + fSegmentMaxEnd + "\n");

	string sItem1;
	string sItem2;
	string sItem3;
	string sItem4;
	sItem1 = "models/weapons/worldmodel-phaser-STX.tik";
	sItem2 = "models/weapons/worldmodel-enterprise.tik";
	sItem3 = "models/weapons/worldmodel-attrex-rifle.tik";
	sItem4 = "models/weapons/worldmodel-sniperrifle.tik";

	$world.setStringVar("segment", "coop_circleL");

	$player.addhud("coop_circle");
	vector V;
	vector VPrew;
	vector VDiff;
	vector VDiffLongTime;
	string sPrint;
	sPrint = "";
	string sPrevWidget;
	float fLastTime;

	while (1)
	{

		wait(0.05);
		V = $player.getViewangles();
		//viewangle has changed
		if (VPrew != V) {
			VDiff = (VPrew - V);
			VPrew = V;

			VDiffLongTime_x += VDiff_x;
			VDiffLongTime_y += VDiff_y;

			//after a few
			if ((fLastTime + 0.1) < getLevelTime()) {

				fLastTime = getLevelTime();

				float fAngle;
				fAngle = arctandegrees(VDiffLongTime_y, VDiffLongTime_x);
				fAngle = noNegativity(fAngle);

				float fSegmentNum;
				string sWidgetName;
				fSegmentNum = getSegmentNumForAngle(fAngle);
				sWidgetName = getWidgetNameForSegment(fSegmentNum);

				if (vectorlength(VDiff) > 0.4 && fSegmentNum >= 0) {

					if (sWidgetName != "" && sWidgetName != sPrevWidget) {
						sendclientcommand($player, "globalwidgetcommand " + sWidgetName + " shadercolor 0 0 0 1");
						sendclientcommand($player, "globalwidgetcommand " + sPrevWidget + " shadercolor 1 1 1 1");
						print("Reset: " + sPrevWidget + "\n");
					}

					sPrint = sPrint + "prev:" + sPrevWidget + " curr:" + sWidgetName;
					sPrevWidget = sWidgetName;

					if (fAngle != 0) {
						sPrint = sPrint + "-'" + fSegmentNum + "'";
						//print(": "+sPrint+" A:"+fAngle+"length:"+vectorlength(VDiff)+"\n");
						print(": " + sPrint + "\n");
					}
				}
				VDiffLongTime_x = 0;
				VDiffLongTime_y = 0;
				VDiffLongTime_z = 0;
			}
		}
		else {
			VDiffLongTime_x = 0;
			VDiffLongTime_y = 0;
			VDiffLongTime_z = 0;
		}
		sPrint = "";
	}

	//$player.circleMenu(1);
	$player.circleMenuSet(1, "Option 1", "use " + sItem1, "sysimg/icons/items/ammo_fed.dds", 0);
	$player.circleMenuSet(2, "Option 2", "use " + sItem2, "sysimg/icons/items/ammo_plasma.dds", 0);
	$player.circleMenuSet(3, "Option 3", "use " + sItem3, "sysimg/icons/items/wep_attrex.dds", 0);
	$player.circleMenuSet(4, "Option 4", "use " + sItem4, "sysimg/icons/items/wep_sniper.dds", 0);

	//$player.hudprint(sItem1+"\n");

	$player.give(sItem1);
	$player.give(sItem2);
	$player.give(sItem3);
	$player.give(sItem4);

	//$player.circleMenuDialogSet(1,"Dialog 1","level1_dialog1Selected","targ.tga");
	return;

	//spawn camera and make player view from there
	spawn("camera", "targetname", "cam1", "origin", "100 0 200");
	spawn("camera", "targetname", "cam2", "origin", "-100 0 200");
	spawn("camera", "targetname", "cam3", "origin", "0 -300 200");
	wait(3);
	float f;
	f = setIniData("category", "key1", "value");
	$player.hudprint("xxx: " + f + "\n");

	wait(1);
	string sData;
	sData = getIniData("tindergarten", "key1");
	$player.hudprint("xxx: " + sData + "\n");

	wait(1);
	setIniData("category", "key2", "value");
	setIniData("category", "key3", "value");
	setIniData("wuteva", "key", "value");
	setIniData("server", "key", "value");
	setIniData("pornhub", "key", "value");
	setIniData("tindergarten", "key1", "tindertinder");

	//make sure the player does not move away from the spot
	$player.immobilize(1);
	$player.nodamage();

	//make camera of player move to camera over time
	float fSwitchTime = 1;
	$cam1.lookat($player);
	$player.setCamera($cam1, fSwitchTime);

	//wait until switch time is over and then allow player to move again
	//make sure the player views from 3rd person or he can't see him self
	//allow player to get hurt again
	wait(fSwitchTime);

	//$player.pointOfView(1); //allow to read and set 3rd person view on player
	//$player.pointOfView(1); //allow to read and set 3rd person view on player
	//$player.pointOfView(1); //allow to read and set 3rd person view on player

	sendclientcommand($player, "set cg_3rd_person 1\n");

	$player.immobilize(0);
	$player.takedamage();

	//switch cameras
	wait(3);
	$cam2.lookat($player);
	//$cam2.follow($cam2,$player);
	$cam2.watch("$player");
	$player.setCamera($cam2, 0.0);
	wait(3);
	$player.setCamera($cam3, 0.0);

	$cam3.lookat($player);
	$cam3.watch("$player");
	//$cam3.follow($cam3,$player);
	wait(3);
	$player.setCamera($NULL, 0.0);
}

float noNegativValues(float fNumber)
{
	if (fNumber < 0) {
		fNumber = (fNumber * -1);
	}
	return fNumber;
}


float noNegativity(float fNumber)
{
	if (fNumber < 0) {
		fNumber = (360 - (fNumber * -1));
	}
	return fNumber;
}

//void PmoveSingle (pmove_t *pmove)
// Clear movement flags
//client->ps.pm_flags &= ~( PMF_FLIGHT | PMF_FROZEN | PMF_NO_PREDICTION | PMF_NO_MOVE | PMF_HAVETARGET | PMF_NO_GRAVITY );
void playerAnglez()
{
	wait(3);
	float fXaxis;
	float fYaxis;
	float fXaxisLast;
	float fYaxisLast;
	float fWasNegative;
	vector vAnglesLast;

	while (1)
	{
		print("---\n");
		string sMovement;
		vector vMoveDir;
		string sXa;
		vector vAngles;
		vAngles = $player.getViewangles();

		vMoveDir = '0 0 0';

		fXaxis = floor(vAngles_x);
		fYaxis = floor(vAngles_y);

		if (fWasNegative < 0) {
			fWasNegative = 1;
		}
		else {
			fWasNegative = 0;
		}

		fYaxis = noNegativity(fYaxis);

		sXa = sXa + "" + fXaxis + " " + fYaxis + " " + vAngles_z;

		if (fXaxisLast == 0 && fYaxisLast == 0) {
			fXaxisLast = fXaxis;
			fYaxisLast = fYaxis;
		}

		if (vAnglesLast == '0 0 0') {
			vAnglesLast = vAngles;
		}

		//calculate - difference of movement 
		float fXDiff;
		float fYDiff;
		fYDiff = noNegativity(fYaxis - fYaxisLast);
		fXDiff = noNegativity(fXaxis - fXaxisLast);

		//move left/right
		if (fYaxis != fYaxisLast) {
			if (fYaxis > fYaxisLast) {//left
				if (fYaxis > (fYaxisLast + 200)) { //degree jump - so still right
					sMovement = ">";
				}
				else {
					sMovement = "<";
				}
			}
			else {//right or degree jump
				if (fYaxisLast > (fYaxis + 200)) { //degree jump - so still left
					sMovement = "<";
				}
				else {
					sMovement = ">";
				}
			}
		}
		else {
			sMovement = "=";
		}

		//UP/DOWN
		if (fXaxis != fXaxisLast) {
			if (fXaxis < fXaxisLast) {
				sMovement = sMovement + "+";
			}
			else {
				sMovement = sMovement + "-";
			}
		}
		else {
			sMovement = sMovement + "=";
		}

		if (fXaxis != fXaxisLast || fYaxis != fYaxisLast) {
			sXa = sXa + ""; //fXaxis
			sXa = "x:" + fXDiff + " y:" + fYDiff;
			print(sMovement + " " + sXa + "\n");
		}

		fXaxisLast = fXaxis;
		fYaxisLast = fYaxis;
		sXa = "";
		wait(0.05);
	}
}

*/