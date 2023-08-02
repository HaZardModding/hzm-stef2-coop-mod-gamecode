//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
//  GAMEUPGRADE code for Actor Playdialog (when a actor talks) in multiplayer
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"

class UpgPlayDialog
{
public:
	void				dialogResetPlayers(void);
	void				dialogSetupPlayers(Entity* speaker, char localizedDialogName[MAX_QPATH], bool headDisplay);
};

extern UpgPlayDialog upgPlayDialog;
extern Event EV_Actor_upgBranchDialogFailsafe;