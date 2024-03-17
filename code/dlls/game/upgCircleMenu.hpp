//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl, 
// E-Mail:	chrissstrahl@yahoo.de
// 
// Circle Menu Related Code, used to add New Mewnu Type to the game, like known from modern games.
//-----------------------------------------------------------------------------------
#pragma once

//void PmoveSingle (pmove_t *pmove)
// Clear movement flags
//client->ps.pm_flags &= ~( PMF_FLIGHT | PMF_FROZEN | PMF_NO_PREDICTION | PMF_NO_MOVE | PMF_HAVETARGET | PMF_NO_GRAVITY );

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
	int		active = 0;
	int		numOfSegments = 4; //[b60021] chrissstrahl - changed so we can SETUP circlemenu without inizialising/opening it first
	//viewangle - stored upon opening the menu
	Vector	viewAngle = Vector(0, 0, 0);
	//last time the menu did think
	float	thinkTime = 0.0f;
	//time when the menu was activated - used to prevent premature opening/closeing
	float	activatingTime = 0.0f;
//time when the circlemenu was last active - used to prevent weapon fireing in next frame on menu closing
//float	lastTimeActive = 0.0f;
	//time a message was last send to player
	float	lastMessageTime = 0.0f;
	//remember stuff so we can compare
	Vector	lastViewangle = Vector(0, 0, 0);
	Vector	longtimeViewangle = Vector(0, 0, 0);
	str		lastWidget = "";
	int		lastSegment = -1;
	
	//[b60021] chrissstrahl
	str		lastWeapon = "None";
	bool	holdingRightButton = false;
	bool	holdingLeftButton = false;
	str		optionTextLastSend[CIRCLEMENU_MAX_OPTIONS];
	str		optionIconLastSend[CIRCLEMENU_MAX_OPTIONS];

	//circlemenu options
	str		optionThreadOrCommand	[CIRCLEMENU_MAX_OPTIONS];
	str		optionText				[CIRCLEMENU_MAX_OPTIONS];
	str		optionIcon				[CIRCLEMENU_MAX_OPTIONS];
	bool	optionIsScript			[CIRCLEMENU_MAX_OPTIONS];
	int		optionAmmount			[CIRCLEMENU_MAX_OPTIONS];
	int		optionCost				[CIRCLEMENU_MAX_OPTIONS];
	str		optionCostType			[CIRCLEMENU_MAX_OPTIONS];

	//circlemenu dialog options  - used for dialog selection
	str		optionDialogThread	[CIRCLEMENU_MAX_OPTIONSDIALOG];
	str		optionDialogText	[CIRCLEMENU_MAX_OPTIONSDIALOG];
	str		optionDialogIcon	[CIRCLEMENU_MAX_OPTIONSDIALOG];
};

//UpgCircleMenu::upgCircleMenuCall();


/* THIS WAS ADDED TO: player.cpp at CLASS_DECLARATION( Sentient , Player , "player" )
//hzm gameupdate chrissstrahl [b60011] circlemenu
{ &EV_Player_circleMenu,					&Player::upgCircleMenuEvent},
{ &EV_Player_circleMenuDialogSet,			&Player::upgCircleMenuDialogSetEvent },
{ &EV_Player_circleMenuDialogClear,			&Player::upgCircleMenuDialogClearEvent },
{ &EV_Player_circleMenuSet,					&Player::upgCircleMenuSetEvent },
{ &EV_Player_circleMenuClear,				&Player::upgCircleMenuClearEvent },
*/

/* THIS WAS ADDED TO: player.h at class Player : public Sentient
		//hzm gameupdate chrissstrahl [b60011] - circlemenu stuff
	public:
		UpgCircleMenu		upgCircleMenu;
		friend class		UpgCircleMenu;
	private:
		void				CircleMenuSetup();
		void				upgCircleMenuCall(int iType);
		bool				upgCircleMenuIsActive(void);
		float				upgCircleMenuLastTimeActive();
		str					upgCircleMenuGetWidgetName(int iDirection);
		int					getSegmentNumForAngle(float fAngle);
		void				upgCircleMenuThink(void);
		void				upgCircleMenuSelect(int iSelection);
		void				upgCircleMenuSet(int iOption, str sText, str sThread, str sImage, bool bThread, int iAmmount, int iCost, str sCostType);
		void				upgCircleMenuClear(int iOption);
		void				upgCircleMenuDialogSet(int iOption, str sText, str sThread, str sImage);
		void				upgCircleMenuDialogClear(int iOption);
		void				upgCircleMenuHud(bool show);
		void				upgCircleMenuSetEvent(Event* ev);
		void				upgCircleMenuClearEvent(Event* ev);
		void				upgCircleMenuDialogSetEvent(Event* ev);
		void				upgCircleMenuDialogClearEvent(Event* ev);
		void				upgCircleMenuEvent(Event* ev);
		//circlemenu - end
*/