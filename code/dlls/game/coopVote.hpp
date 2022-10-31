//[b607] new - handling vote related coop stuff

#ifndef __COOPVOTE_HPP__
#define __COOPVOTE_HPP__

#include "_pch_cpp.h"
//associated with ::MultiplayerManager::callvote
bool coop_vote_checkvalid(const str &command);
void coop_vote_printcommands(Player *player);

//check also func:
//bool coop_textModifyVoteText( const str _voteString, str &sVoteText, Player* player )

int coop_vote_skipcinematicValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_lastmanstandingValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_quitserverValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_respawntimeValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_awardsValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_friendlyfireValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_maxspeedValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_mapValidate(Player* player, const str &command, const str &arg, str &_voteString);
//int coop_vote_mpmodifierValidate(Player* player, const str &command, const str &arg, str &_voteString); //handled in MultiplayerManager::callVote
int coop_vote_airaccelerateValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_stasistimeValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_challengeValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_deadbodiesValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_kickValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_execValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_skillValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_addbotValidate(Player* player, const str &command, const str &arg, str &_voteString);
int coop_vote_mapNxtPrevValidate(Player* player, const str &command, const str &arg, str &_voteString);

//associated with ::MultiplayerManager::checkVote
bool coop_vote_skipcinematicSet(const str  _voteString);
bool coop_vote_mpmodifierSet(const str  _voteString);
bool coop_vote_quitserverSet(const str _voteString);
bool coop_vote_lastmanstandingSet(const str  _voteString);
bool coop_vote_respawntimeSet(const str  _voteString);
bool coop_vote_airaccelerateSet(const str _voteString);
bool coop_vote_stasistimeSet(const str _voteString);
bool coop_vote_awardsSet(const str _voteString);
bool coop_vote_friendlyfireSet(const str _voteString);
bool coop_vote_maxspeedSet(const str _voteString);
bool coop_vote_challengeSet(const str _voteString);
bool coop_vote_deadbodiesSet(const str _voteString);
bool coop_vote_mapSet(const str _voteString);
bool coop_vote_kickbotsSet(const str _voteString);
bool coop_vote_execSet(const str _voteString);
#endif