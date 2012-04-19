#pragma once
class ITrackable abstract
{
public:
	virtual void GetPosition(float*, float*) = 0;
};