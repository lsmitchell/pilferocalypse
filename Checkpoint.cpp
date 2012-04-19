#include "StdAfx.h"
#include "Checkpoint.h"


Checkpoint::Checkpoint()
{
	this->x = 0;
	this->y = 0;
}

Checkpoint::Checkpoint(float x, float y)
{
	this->x = x;
	this->y = y;
}

Checkpoint::~Checkpoint(void)
{
}

void Checkpoint::GetPosition(float* px, float* py)
{
	*px = x;
	*py = y;
}