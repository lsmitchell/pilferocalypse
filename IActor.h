#pragma once
#include "SimpleSprite.h"

interface IActor
{
	virtual void Update(float timePassed) = 0;
	virtual void SetPosition(float, float) = 0;
	virtual void GetPosition(float*, float*) = 0;
	virtual void SetSprite(SimpleSprite*) = 0;
	virtual void AdjustScale(float, float) = 0;
	virtual void Move(float, float) = 0;
	virtual SimpleSprite* GetSprite(void) = 0;
};

