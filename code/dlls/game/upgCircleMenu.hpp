//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
// 
// Circle Menu Related Code, used to add New Mewnu Type to the game, like known from modern games.
//-----------------------------------------------------------------------------------

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

extern Event EV_Player_circleMenu;
extern Event EV_Player_circleMenuDialogSet;
extern Event EV_Player_circleMenuDialogClear;
extern Event EV_Player_circleMenuSet;
extern Event EV_Player_circleMenuClear;

class UpgCircleMenu
{
public:
	int		active;
	int		numOfSegments;
	//viewangle - stored upon opening the menu
	Vector	viewAngle;
	//last time the menu did think
	float	thinkTime;
	//time when the menu was activated - used to prevent premature opening/closeing
	float	activatingTime;
	//remember stuff so we can compare
	Vector	lastViewangle;
	Vector	longtimeViewangle;
	str		lastWidget;
	int		lastSegment;
	void	init();
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


/* THIS WAS ADDED TO: player.cpp at CLASS_DECLARATION( Sentient , Player , "player" )
//hzm gameupdate chrissstrahl [b60011] circlemenu
{ &EV_Player_circleMenu,					&Player::circleMenuEvent},
{ &EV_Player_circleMenuDialogSet,			&Player::circleMenuDialogSetEvent },
{ &EV_Player_circleMenuDialogClear,			&Player::circleMenuDialogClearEvent },
{ &EV_Player_circleMenuSet,					&Player::circleMenuSetEvent },
{ &EV_Player_circleMenuClear,				&Player::circleMenuClearEvent },
*/

/* THIS WAS ADDED TO: player.h at class Player : public Sentient
		//hzm gameupdate chrissstrahl [b60011] - circlemenu stuff
	public:
		UpgCircleMenu		upgCircleMenu;
		friend class		UpgCircleMenu;
	private:
		void				circleMenu(int iType);
		bool				circleMenuIsActive(void);
		str					circleMenuGetWidgetName(int iDirection);
		int					getSegmentNumForAngle(float fAngle);
		void				circleMenuThink(void);
		void				circleMenuSelect(int iSelection);
		void				circleMenuSet(int iOption, str sText, str sThread, str sImage, bool bThread, int iAmmount, int iCost, str sCostType);
		void				circleMenuClear(int iOption);
		void				circleMenuDialogSet(int iOption, str sText, str sThread, str sImage);
		void				circleMenuDialogClear(int iOption);
		void				circleMenuHud(bool show);
		void				circleMenuSetEvent(Event* ev);
		void				circleMenuClearEvent(Event* ev);
		void				circleMenuDialogSetEvent(Event* ev);
		void				circleMenuDialogClearEvent(Event* ev);
		void				circleMenuEvent(Event* ev);
		//circlemenu - end
*/