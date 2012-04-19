#include "StdAfx.h"
#include "Wall.h"
#include "dxManager.h"
#include "GameManager.h"

// Constructor for a new wall sprite
Wall::Wall(dxManager* dx, TCHAR* imageName): ICollidable(dx)
{
	dx->MakeSprite(imageName, &mySprite);
	SetWidthHeight();
	//xWidth = MAP_BLOCK_SIZE;
	//yWidth = MAP_BLOCK_SIZE;
}

//updates the time passed
void Wall::Update(float timePassed)
{
	UpdateSprite();
}

void Wall::OnPlayerCollide(PlayerActor* player)
{
	// do nothing. Just a wall.
}

//deconstructor
Wall::~Wall(void)
{
}
