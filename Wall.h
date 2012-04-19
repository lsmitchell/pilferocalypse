#pragma once
#include "ICollidable.h"

using namespace std;

// represents a solid object in the world that doesn't move. Many of these are used to make up the level geometry
class Wall : public ICollidable
{
public:
	Wall(dxManager*, TCHAR*);
	void Update(float);
	void OnPlayerCollide(PlayerActor* player);
	~Wall(void);
};

