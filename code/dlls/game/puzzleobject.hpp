//-----------------------------------------------------------------------------
//
//  $Logfile:: /Code/DLLs/game/program.h                                      $
// $Revision:: 7                                                              $
//     $Date:: 5/07/02 12:02p                                                 $
//
// Copyright (C) 1999 by Ritual Entertainment, Inc.
// All rights reserved.
//
// This source is may not be distributed and/or modified without
// expressly written permission by Ritual Entertainment, Inc.
//

#ifndef PUZZLE_OBJECT_HPP
#define PUZZLE_OBJECT_HPP

#include "entity.h"


class PuzzleObject : public Entity
{
	public:
		CLASS_PROTOTYPE( PuzzleObject );

		typedef enum
		{
			// "idle" anim
			PUZZLE_STATE_IDLE,			// normal, just waiting for the player
			PUZZLE_STATE_IDLE_SOLVED,	// player has solved it, it's in afterglow
			PUZZLE_STATE_IDLE_LOCKED,	// player fucked it up, it's sulking

			// "opening" anim
			PUZZLE_STATE_OPENING,		// working on getting open
			
			// "open" anim
			PUZZLE_STATE_IDLE_OPEN,		// waiting to be used by you
			
			// "openon" anim
			PUZZLE_STATE_ACTIVE_OPEN,	// is being used by you

			// "closing" anim
			PUZZLE_STATE_CLOSING,		// closing on the way to normal idle
			PUZZLE_STATE_CLOSING_SOLVED,// closing after being solved
			PUZZLE_STATE_CLOSING_LOCKED,// closing after being failed

			PUZZLE_STATE_DEACTIVATED

		}PuzzleState;


		PuzzleObject();
		virtual ~PuzzleObject();

		PuzzleState			getPuzzleState(void)					{ return _puzzleState;			}
		void				setPuzzleState(PuzzleState puzzleState)	{ _puzzleState = puzzleState;	}

		void				setOpenDistance(Event* event);

		//Thread setting functions
		void				setUsedStartThread(Event* event); //[b60011] chrissstrahl - thread called when puzzle is started to be used

		void				setItemUsedThread(Event* event);
		void				setFailedThread(Event* event);
		void				setSolvedThread(Event* event);
		void				setCanceledThread(Event* event);

		void				failed(Event* event);
		void				canceled(Event* event);
		void				solved(Event* event);
		void				reset( Event* event );

		void				animationDone(Event* event);
		void				setItemToUse(Event* event);
		void				useEvent(Event* event);
		void				activate( Event* event );
		void				deActivate( Event* event );

		void				normalUse( Event* event );
		void				timedUse( Event* event );

		void				setTimeToUse( Event *ev );
		void				timedPuzzleSolved( Player *player );
		void				timedPuzzleCanceled( void );
		void				showTimerHud( Player *player );
		void				hideTimerHud( Player *player );

		void				setTimerHudName( Event * ev );
		
		void				becomeModBarInSkill( Event* ev );

		/*virtual*/ void	Archive(Archiver &arc);
		/*virutal*/ void	Think( void );
		void				cancelPlayer(Player* player); //[b60012] chrissstrahl - allow camncellation of current puzzle
		EntityPtr			GetLastActivatingEntity(); //[b60012] chrissstrahl - return last activator
	private:
		str					_itemToUse;
		//Animations to use based up the state.
		void				GetLastActivatingEntity(Event* ev); //[b60012] chrissstrahl - return last activator
		str					_usedStartThread;  //[b60011] chrissstrahl - thread called when puzzle is started to be used
		str					_itemUsedThread;
		str					_failedThread;
		str					_solvedThread;
		str					_canceledThread;

		PuzzleState			_puzzleState;
		PuzzleState			_saveState;
		float				_openDistance;

		bool				_timed;
		float				_timeToUse;
		float				_lastTimeUsed;
		float				_usedTime;
		bool				_hudOn;

		float				_nextNeedToUseTime;

		str					_hudName;
		int					_minSkill; // if skill level is <= this, the puzzle becomes a mod bar

		EntityPtr	      activator;

		//[b60016] chrissstrahl - coop
		bool coopPuzzleobjectUsePuzzleCheck(Player* player);
};


inline void PuzzleObject::Archive( Archiver &arc )
{
	Entity::Archive( arc );

	arc.ArchiveString( &_itemToUse				);
	arc.ArchiveString( &_itemUsedThread			);
	arc.ArchiveString( &_usedStartThread		); //[b60011] chrissstrahl - thread called when puzzle is started to be used
	arc.ArchiveString( &_failedThread			);
	arc.ArchiveString( &_solvedThread			);
	arc.ArchiveString( &_canceledThread			);
	ArchiveEnum( _puzzleState, PuzzleState	);
	ArchiveEnum( _saveState, PuzzleState );

	arc.ArchiveFloat( &_openDistance );

	arc.ArchiveBool( &_timed );
	arc.ArchiveFloat( &_timeToUse );
	arc.ArchiveFloat( &_lastTimeUsed );
	arc.ArchiveFloat( &_usedTime );
	arc.ArchiveBool( &_hudOn );

	arc.ArchiveFloat( &_nextNeedToUseTime );

	arc.ArchiveString( &_hudName );
	
	arc.ArchiveInteger( &_minSkill );

	arc.ArchiveSafePointer( &activator );
}

#endif
