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

class UpgCircleMenu
{
public:
	int		active = 0;
	//last time the menu did think
	float	thinkTime = 0.0f;
	//time when the menu was activated - used to prevent premature opening/closeing
	float	activatingTime = 0.0f;
	//viewangle - stored upon opening the menu
	Vector	viewAngle = Vector(0, 0, 0);
	//remember last angles so we can compare
	Vector	lastAngle = Vector(0, 0, 0);
	//remember values of last move directions - used to determin which direction the player intended to move to
	int		moveUp = 0;
	int		moveDown = 0;
	int		moveLeft = 0;
	int		moveRight = 0;
	str		lastWidget = "";

	//circlemenu options
	str		option1 = "";
	str		option1Icon = "";
	str		option1Text = "";
	bool	option1IsScript = false;

	str		option2 = "";
	str		option2Icon = "";
	str		option2Text = "";
	bool	option2IsScript = false;

	str		option3 = "";
	str		option3Icon = "";
	str		option3Text = "";
	bool	option3IsScript = false;

	str		option4 = "";
	str		option4Icon = "";
	str		option4Text = "";
	bool	option4IsScript = false;

	//circlemenu dialog options  - used for dialog selection
	str		dialogOption1 = "";
	str		dialogOption1Icon = "";
	str		dialogOption1Text = "";

	str		dialogOoption2 = "";
	str		dialogOption2Icon = "";
	str		dialogOption2Text = "";

	str		dialogOption3 = "";
	str		dialogOption3Icon = "";
	str		dialogOption3Text = "";

	str		dialogOption4 = "";
	str		dialogOption4Icon = "";
	str		dialogOption4Text = "";
};

#endif /* !__UPGCIRCLEMENU_HPP__ */