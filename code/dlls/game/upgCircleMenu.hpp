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

//void PmoveSingle (pmove_t *pmove)
// Clear movement flags
//client->ps.pm_flags &= ~( PMF_FLIGHT | PMF_FROZEN | PMF_NO_PREDICTION | PMF_NO_MOVE | PMF_HAVETARGET | PMF_NO_GRAVITY );

#ifndef __UPGCIRCLEMENU_HPP__
#define __UPGCIRCLEMENU_HPP__

#include "_pch_cpp.h"
#include "player.h"

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
//int		currentSegment = -1;

	//circlemenu options
	str		optionThreadOrCommand	[CIRCLEMENU_MAX_OPTIONS];
	str		optionText				[CIRCLEMENU_MAX_OPTIONS];
	str		optionIcon				[CIRCLEMENU_MAX_OPTIONS];
	bool	optionIsScript			[CIRCLEMENU_MAX_OPTIONS];
	bool	optionAmmount			[CIRCLEMENU_MAX_OPTIONS];
	bool	optionCost				[CIRCLEMENU_MAX_OPTIONS];
	bool	optionCostType			[CIRCLEMENU_MAX_OPTIONS];

	//circlemenu dialog options  - used for dialog selection
	str		optionDialogThread	[CIRCLEMENU_MAX_OPTIONSDIALOG];
	str		optionDialogText	[CIRCLEMENU_MAX_OPTIONSDIALOG];
	str		optionDialogIcon	[CIRCLEMENU_MAX_OPTIONSDIALOG];
};


//UpgCircleMenu::circleMenu();

#endif /* !__UPGCIRCLEMENU_HPP__ */