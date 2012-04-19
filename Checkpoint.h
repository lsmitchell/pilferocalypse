#pragma once
#include "ITrackable.h"

class Checkpoint : public ITrackable
{
public:
	Checkpoint();
	Checkpoint(float x, float y);
	~Checkpoint(void);
	void GetPosition(float*, float*);
	float x,y;
};

