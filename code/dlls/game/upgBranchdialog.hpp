//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
//  GAMEUPGRADE code for dialog stuff in multiplayer
//-----------------------------------------------------------------------------------
#pragma once

#include "_pch_cpp.h"

class UpgBranchDialog
{
	bool			active = false;
public:
	void			runThread(Entity* ent);
	void			setBranchDialog(Actor* actor, str sDialogName);
};

extern UpgBranchDialog upgBranchDialog;