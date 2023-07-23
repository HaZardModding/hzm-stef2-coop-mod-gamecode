#ifndef __UPGWORLD_HPP__
#define __UPGWORLD_HPP__

class UpgWorld
{
	bool			updateDynamicLights = false;
public:
	bool			getUpdateDynamicLights();
	void			setUpdateDynamicLights(bool bUpdate);
};

extern UpgWorld upgWorld;

#endif /* upgWorld.hpp */