//-----------------------------------------------------------------------------------
// Code by:	HaZardModding, Christian Sebastian Strahl
// E-Mail:		chrissstrahl@yahoo.de
//
// USED AS A INTERFACE BETWEEN THE GAMEUPGRADE CODE AND THE HZM CO-OP MOD
//-----------------------------------------------------------------------------------

#ifndef __UPGCOOPINTERFACE_HPP__
#define __UPGCOOPINTERFACE_HPP__


class UpgCoopInterface
{
private:

public:
	bool				isCoopActive();
};

extern UpgCoopInterface upgCoopInterface;

#endif /* upgCoopInterface.hpp */