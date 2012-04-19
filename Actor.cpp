#include "StdAfx.h"
#include "Actor.h"
#include "GameManager.h"

//sets up a general actor
Actor::Actor(dxManager* dx)
{
	dxMan = dx;
	mySprite = NULL;
	xPosition = 0;
	yPosition = 0;
}

//deconstructor
Actor::~Actor(void)
{
}

//Alters the sprite's position to the actor's position. In this
//function, we must remember that not all actors use a sprite (so
// we need to check if it's NULL)
void Actor::UpdateSprite()
{
	if (mySprite == NULL)
	{
		cout << "Actor being updated without any sprite assigned!";
	}
	else
	{
		mySprite->SetPosition(xPosition, yPosition, 0.0f);
	}
	//mySprite->RefreshMatrix();
}

//Sets the actor's current position.
void Actor::SetPosition(float x, float y)
{
	xPosition = x;
	yPosition = y;
	UpdateSprite();
}

//Sets the parameters to this actor's x and y position
void Actor::GetPosition(float* x, float* y)
{
	*x = xPosition;
	*y = yPosition;
}

void Actor::GetHalfWidth(float* x, float* y)
{
	*x = halfXWidth;
	*y = halfYWidth;
}

//Add the specified values to the actor's position
void Actor::Move(float x, float y)
{
	xPosition += x;
	yPosition += y;
	UpdateSprite();
}

//Assigns a sprite object to attach to this Actor.
void Actor::SetSprite(SimpleSprite* sprite)
{
	mySprite = sprite;
	SetWidthHeight();
	UpdateSprite();
}

//get the sprite object attached to this actor
SimpleSprite* Actor::GetSprite()
{
	return mySprite;
}

void Actor::SetVisible(bool vis)
{
	if (mySprite != NULL)
		mySprite->Visible = vis;
}

void Actor::SetWidthHeight()
{
	float conversionRatioY = 0.007f;
	float conversionRatioX = 0.003f;

	// temporary. Sets the width and height to the map grid width, height
	xWidth = abs(mySprite->spriteWidth * conversionRatioX);
	yWidth = abs(mySprite->spriteHeight * conversionRatioY);

	halfXWidth = xWidth / 2;
	halfYWidth = yWidth / 2;
	/*
	xWidth = (float)mySprite->spriteWidth;
	yWidth = (float)mySprite->spriteHeight;
	*/
}

//adjust the scale of the actor
void Actor::AdjustScale(float xScale, float yScale)
{
	mySprite->AdjustScaling(xScale, yScale);
}

//retrieve x of actor
float Actor::getX()
{
	return xPosition;
}

//retrieve y of actor
float Actor::getY()
{
	return yPosition;
}