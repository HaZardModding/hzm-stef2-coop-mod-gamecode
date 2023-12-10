//[b607] new - handle actor specific coop stuff

#ifndef __COOPACTOR_HPP__
#define __COOPACTOR_HPP__

#include "_pch_cpp.h"

bool coop_actorIgnoreAnimationWarning(const str &modelName,const str &animName);
void coop_actorUsedByEquipment(Actor* actor, Entity* entityEquipment);
bool coop_actorDeadBodiesHandle(Entity* actor);
void coop_actorDeadBodiesRemove();

#endif