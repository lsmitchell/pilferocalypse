#pragma once
#include "ICollidable.h"
class RaceGate :
	public ICollidable
{
private:
	bool active;
public:
	RaceGate(dxManager* dx);
	~RaceGate(void);
	void SetBlockExit();
	bool IsActive();
	void SetInactive();
	void Update(float);
	void OnPlayerCollide(PlayerActor* player);
};

