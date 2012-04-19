#pragma once
#include "Actor.h"

class PlayerActor;

// ICollidable represents any abstract object in the environment for which something happens to the player if
// they touch it. The most simple collidable is a Wall.
class ICollidable abstract : public Actor
{
public:
	ICollidable(dxManager* dx);

	// don't really need these
	/*
	virtual float Left();
	virtual float Right();
	virtual float Top();
	virtual float Bottom();*/
	virtual void OnPlayerCollide(PlayerActor* player) = 0;
};

