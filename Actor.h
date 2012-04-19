#pragma once
#include "SimpleSprite.h"
//#include "dxManager.h"
#include "IActor.h"
#include "ITrackable.h"
#include <iostream>

using namespace std;

class dxManager;

//Actors are objects in the world that perform actions
//or interact in some way. They are not directly drawn,
//but usually contain a SimpleSprite object to draw,
//which they set to their position.
class Actor : public IActor, public ITrackable
{
protected:
	dxManager* dxMan;
	float xPosition, yPosition;
	float xWidth, yWidth, halfXWidth, halfYWidth;
	SimpleSprite* mySprite;
	void UpdateSprite();
	void SetWidthHeight();
public:
	Actor(dxManager*);
	~Actor(void);
	
	//Update is called each timestep for every actor, to
	//increment positions, apply physics, etc.
	virtual void Update(float timePassed) = 0;
	void SetPosition(float, float);
	void GetPosition(float*, float*);
	void GetHalfWidth(float*, float*);
	void Move(float, float);
	void SetSprite(SimpleSprite*);
	void SetVisible(bool);
	SimpleSprite* GetSprite();
	float getX();
	float getY();
	
	void AdjustScale(float, float);
};

