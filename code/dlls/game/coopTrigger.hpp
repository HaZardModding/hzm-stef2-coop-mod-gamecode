#ifndef __COOPTRIGGER_HPP__
#define __COOPTRIGGER_HPP__

#include "_pch_cpp.h"

class TriggerCoopActorHeadwatch : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopActorHeadwatch );
	void Activate( Event *ev );
	TriggerCoopActorHeadwatch();
};
class TriggerCoopActorTurntowards : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopActorTurntowards );
	void Activate( Event *ev );
	TriggerCoopActorTurntowards();
};
class TriggerCoopEntityArchetype : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityArchetype );
	void Activate( Event *ev );
	TriggerCoopEntityArchetype();
};
class TriggerCoopEntityBind : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityBind );
	void Activate( Event *ev );
	TriggerCoopEntityBind();
};
class TriggerCoopWorldWater : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopWorldWater );
	void Activate( Event *ev );
	TriggerCoopWorldWater();
};
class TriggerCoopActorFieldofview : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopActorFieldofview );
	void Activate( Event *ev );
	TriggerCoopActorFieldofview();
};
class TriggerCoopWorldBroken : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopWorldBroken );
	void Activate( Event *ev );
	TriggerCoopWorldBroken();
};
class TriggerCoopEntityViewmode : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityViewmode );
	void Activate( Event *ev );
	TriggerCoopEntityViewmode();
};
class TriggerCoopWorldViewmode : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopWorldViewmode );
	void Activate( Event *ev );
	TriggerCoopWorldViewmode();
};
class TriggerCoopPlayerHud : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopPlayerHud );
	void Activate( Event *ev );
	TriggerCoopPlayerHud();
};
class TriggerCoopMissionMarker : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopMissionMarker );
	void Activate( Event *ev );
	TriggerCoopMissionMarker();
};
class TriggerCoopGameSave : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopGameSave );
	void Activate( Event *ev );
	TriggerCoopGameSave();
};
class TriggerCoopWorldFog : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopWorldFog );
	void Activate( Event *ev );
	TriggerCoopWorldFog();
};
class TriggerCoopWorldWeather : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopWorldWeather );
	void Activate( Event *ev );
	TriggerCoopWorldWeather();
};
class TriggerCoopWorldPhysics : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopWorldPhysics );
	void Activate( Event *ev );
	TriggerCoopWorldPhysics();
};
class TriggerCoopEntityRelativemove : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityRelativemove );
	void Activate( Event *ev );
	TriggerCoopEntityRelativemove();
};
class TriggerCoopEntityContents : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityContents );
	void Activate( Event *ev );
	TriggerCoopEntityContents();
};
class TriggerCoopPlayerForeach : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopPlayerForeach );
	void Activate( Event *ev );
	TriggerCoopPlayerForeach();
};
class TriggerCoopActorDeath : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopActorDeath );
	void Activate( Event *ev );
	TriggerCoopActorDeath();
};
class TriggerCoopMissionStory : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopMissionStory );
	void Activate( Event *ev );
	TriggerCoopMissionStory();
};
class TriggerCoopMissionTactical : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopMissionTactical );
	void Activate( Event *ev );
	TriggerCoopMissionTactical();
};
class TriggerCoopMissionObjective : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopMissionObjective );
	void Activate( Event *ev );
	TriggerCoopMissionObjective();
};
class TriggerCoopEntityTransport : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityTransport );
	void Activate( Event *ev );
	TriggerCoopEntityTransport();
};
class TriggerCoopEntityWarp : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityWarp );
	void Activate( Event *ev );
	TriggerCoopEntityWarp();
};
class TriggerCoopEntityScale : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityScale );
	void Activate( Event *ev );
	TriggerCoopEntityScale();
};
class TriggerCoopTriggerEnable : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopTriggerEnable );
	void Activate( Event *ev );
	TriggerCoopTriggerEnable();
};
class TriggerCoopEntityHealth : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityHealth );
	void Activate( Event *ev );
	TriggerCoopEntityHealth();
};
class TriggerCoopMissionComplete : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopMissionComplete );
	void Activate( Event *ev );
	TriggerCoopMissionComplete();
};
class TriggerCoopPlayerAmmo : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopPlayerAmmo );
	void Activate( Event *ev );
	TriggerCoopPlayerAmmo();
};
class TriggerCoopPlayerItem : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopPlayerItem );
	void Activate( Event *ev );
	TriggerCoopPlayerItem();
};
class TriggerCoopPlayerMessage : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopPlayerMessage );
	void Activate( Event *ev );
	TriggerCoopPlayerMessage();
};
class TriggerCoopConsoleMessage : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopConsoleMessage );
	void Activate( Event *ev );
	TriggerCoopConsoleMessage();
};
class TriggerCoopDoorManage : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopDoorManage );
	void Activate( Event *ev );
	TriggerCoopDoorManage();
};
class TriggerCoopEntityAnimate : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityAnimate );
	void Activate( Event *ev );
	TriggerCoopEntityAnimate();
};
class TriggerCoopEntityVisibility : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopEntityVisibility );
	void Activate( Event *ev );
	TriggerCoopEntityVisibility();
};
class TriggerCoopActorWalkto : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopActorWalkto );
	void Activate( Event *ev );
	TriggerCoopActorWalkto();
};
class TriggerCoopActorAi : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopActorAi );
	void Activate( Event *ev );
	TriggerCoopActorAi();
};
class TriggerCoopActorPlaydialog : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopActorPlaydialog );
	void Activate( Event *ev );
	TriggerCoopActorPlaydialog();
};
class TriggerCoopCinematicStart : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopCinematicStart );
	void Activate( Event *ev );
	TriggerCoopCinematicStart();
};
class TriggerCoopCinematicSkip : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopCinematicSkip );
	void Activate( Event *ev );
	TriggerCoopCinematicSkip();
};
class TriggerCoopCinematicEnd : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopCinematicEnd );
	void Activate( Event *ev );
	TriggerCoopCinematicEnd();
};
class TriggerCoopCinematicCuecamera : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopCinematicCuecamera );
	void Activate( Event *ev );
	TriggerCoopCinematicCuecamera();
};
class TriggerCoopCinematicFollowpath : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopCinematicFollowpath );
	void Activate( Event *ev );
	TriggerCoopCinematicFollowpath();
};
class TriggerCoopCinematicFov : public Trigger
{
public:
	CLASS_PROTOTYPE( TriggerCoopCinematicFov );
	void Activate( Event *ev );
	TriggerCoopCinematicFov();
};


#endif