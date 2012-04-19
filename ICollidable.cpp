#include "StdAfx.h"
#include "ICollidable.h"


ICollidable::ICollidable(dxManager* dx):Actor(dx)
{
}

/*
//returns the left postion of the wall
float ICollidable::Left()
{
	return xPosition - halfXWidth;
}

//returns the right position of the wall
float ICollidable::Right()
{
	return xPosition + halfXWidth;
}

//returns the top position of the wall
float ICollidable::Top()
{
	return yPosition + halfYWidth;
}

//returns the bottom postion of the wall
float ICollidable::Bottom()
{
	return yPosition - halfYWidth;
}
*/