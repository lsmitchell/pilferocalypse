#include "StdAfx.h"
#include "RaceGate.h"


RaceGate::RaceGate(dxManager* dx):ICollidable(dx)
{
	active = false;
	SetInactive();
}


RaceGate::~RaceGate(void)
{
}

bool RaceGate::IsActive()
{
	return active;
}

void RaceGate::SetBlockExit()
{
	active = true;
	halfXWidth = 2.0f;
	halfYWidth = 10.0f;
}

void RaceGate::SetInactive()
{
	active = false;
	halfXWidth = 0.0f;
	halfYWidth = 0.0f;
	SetPosition(-1000, -1000);
}

void RaceGate::Update(float timePassed)
{
}

void RaceGate::OnPlayerCollide(PlayerActor* player)
{
}