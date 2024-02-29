//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// CONTAINING Last Man Standing RELATED FUNCTIONS FOR THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------
#pragma once

void coop_lmsSetLives(int iLives);
int coop_lmsGetLives(void);
bool coop_lmsActive(void);
void coop_lmsCheckFailure(void);

//-> needs work, should be cleaned up -> void coop_playerRestore( Player *player )

// void Player::coop_lmsCheckReconnectHack()
// bool Player::coop_lmsSpawn()
// void Player::coop_lmsPlayerKilled()
// bool MultiplayerManager::coop_lmsMpManagerUpdate(Player * player)