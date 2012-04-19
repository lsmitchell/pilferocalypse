#include "StdAfx.h"
#include "Trap.h"
#include "dxManager.h"

Trap::Trap(dxManager* dx, char* name, float xPos, float yPos) : ICollidable(dx)
{

	//get a ref to the dxmanager display device to make sprites here
	ID3D10Device* pD3DDevice = dx->getPD3DDevice();

	activated = false;
	startX = xPosition = xPos;
	startY = yPosition = yPos;
	isDangerous = true;
	moveDist = 0;
	crossHairCountdown = 0;
	tileSize = 1; //how wide the tiles are; will be set if important using setTileSize
	moveDirection = 1;
	//if it's a saw...
	if(strcmp(name,"Images/saw1.png") == 0)
	{
			numAnimations = 2;
			animationArrayT = new SimpleSprite*[numAnimations];
			trapType = 0;
			stepTime = 10;

			WCHAR* spriteNames[] = {L"Images/saw1.png", L"Images/saw2.png"};

			//load saw sprites
			for (int i = 0; i < numAnimations; i ++)
			{
				dx->MakeSprite(spriteNames[i], &animationArrayT[i]);
				animationArrayT[i]->Visible = false;
				animationArrayT[i]->SetPosition(xPos, yPos, 0);
			}
			mySprite = animationArrayT[0];
			mySprite->Visible = true;
			moveDist = 2;
			SetWidthHeight();
	}

	//if its a flame trap
	else if(strcmp(name,"Images/flame1.png") == 0)
	{
			numAnimations = 2;
			animationArrayT = new SimpleSprite*[numAnimations];
			trapType = 1;
			//stepTime = 3000; //3 seconds between switching states; currently being set in maploader instead

			WCHAR* spriteNameA[] = {L"Images/flame1.png", L"Images/flame2.png"};

			//load sprites
			for (int i = 0; i < numAnimations; i ++)
			{
				dx->MakeSprite(spriteNameA[i], &animationArrayT[i]);
				animationArrayT[i]->Visible = false;
				animationArrayT[i]->SetPosition(xPos, yPos, 0);		
			}
			mySprite = animationArrayT[0];
			mySprite->Visible = true;
			isDangerous = false; //starts as off
			SetWidthHeight();
	}
	else if(strcmp(name,"Images/Turret.png") == 0)
	{
			numAnimations = 1;
			animationArrayT = new SimpleSprite*[numAnimations];
			trapType = 3;
			stepTime = 3000; //3 seconds between switching states; currently being set in maploader instead

			WCHAR* spriteNameA[] = {L"Images/Turret.png"};

			//load sprites
			for (int i = 0; i < numAnimations; i ++)
			{
				dx->MakeSprite(spriteNameA[i], &animationArrayT[i]);
				animationArrayT[i]->Visible = false;
				animationArrayT[i]->SetPosition(xPos, yPos, 0);
			}
			mySprite = animationArrayT[0];
			mySprite->Visible = true;
			AdjustScale(2.0f,2.0f);
			isDangerous = false; //turret doesnt damage, but needs to be true to do collide

			SetWidthHeight();
	}
	else if(strcmp(name,"crosshairs") == 0)
	{
			numAnimations = 5;
			animationArrayT = new SimpleSprite*[numAnimations];
			trapType = 4;
			stepTime = 200; //0.2 seconds between switching frames

			WCHAR* spriteNameA[] = {L"Images/crosshairs.png",L"Images/crosshairs2.png",L"Images/explode1.png",L"Images/explode2.png",L"Images/explode3.png"};

			//load sprites
			for (int i = 0; i < numAnimations; i ++)
			{
				dx->MakeSprite(spriteNameA[i], &animationArrayT[i]);
				animationArrayT[i]->Visible = false;
				animationArrayT[i]->SetPosition(xPos, yPos, 0);
			}
			mySprite = animationArrayT[0];
			mySprite->Visible = false; //starts out invisible
			isDangerous = false; //starts out inactive
			SetWidthHeight();
	}
	else if(strcmp(name,"Images/spikes.png") == 0)
	{
			numAnimations = 1;
			animationArrayT = new SimpleSprite*[numAnimations];
			trapType = 2;
			//stepTime = 3000; //3 seconds between switching states; currently being set in maploader instead

			WCHAR* spriteNameA[] = {L"Images/spikes.png"};

			//load sprites
			for (int i = 0; i < numAnimations; i ++)
			{
				dx->MakeSprite(spriteNameA[i], &animationArrayT[i]);
				animationArrayT[i]->Visible = false;
				animationArrayT[i]->SetPosition(xPos, yPos, 0);
			}
			mySprite = animationArrayT[0];
			mySprite->Visible = true;
			SetWidthHeight();
	}
	else if(strcmp(name,"Images/spikesHang.png") == 0)
	{
			numAnimations = 1;
			animationArrayT = new SimpleSprite*[numAnimations];
			trapType = 2;
			//stepTime = 3000; //3 seconds between switching states; currently being set in maploader instead

			WCHAR* spriteNameA[] = {L"Images/spikesHang.png"};

			//load sprites
			for (int i = 0; i < numAnimations; i ++)
			{
				dx->MakeSprite(spriteNameA[i], &animationArrayT[i]);
				animationArrayT[i]->Visible = false;
				animationArrayT[i]->SetPosition(xPos, yPos, 0);
			}
			mySprite = animationArrayT[0];
			mySprite->Visible = true;
			SetWidthHeight();
	}

	movingIndex = 0;
	prevTime = GetCurrentTime();

}

//returns the left postion of the trap
float Trap::Left()
{
	return xPosition - halfXWidth;
}

void Trap::SetTriggerTarget(Trap* target)
{
	triggerTarg = target;
}

//returns the right position of the trap
float Trap::Right()
{
	return xPosition + halfXWidth;
}

//returns the top position of the trap
float Trap::Top()
{
	return yPosition + halfYWidth;
}

//returns the bottom postion of the trap
float Trap::Bottom()
{
	return yPosition - halfYWidth;
}


void Trap::OnPlayerCollide(PlayerActor* collidedPlayer)
{
	//send player to last checkpoint on collision
	//if its a turret, set off its crosshair
	if(trapType == 3)
	{
		if(!triggerTarg->activated)
		{
			triggerTarg->Activate();
		}
	}
	else
	{
		collidedPlayer->Die();
	}
}


//sets the current frame of animations
void Trap::SetAnimSprite(int idx)
{
	mySprite->Visible = false;
	mySprite = animationArrayT[idx];
	mySprite->Visible = true;

}

//sets the current frame of animations
void Trap::SetAnimStepTime(int newStepTime)
{
	stepTime = newStepTime;
}

bool Trap::trapWillKill()
{
	return isDangerous;
}

void Trap::setTileSize(float size)
{
	tileSize = size;
}

//updates the player actor in time by the specified
//elapsing of time
void Trap::Update(float timePassed)
{
	//sawBlade
	if(trapType == 0)
	{
		currentTime = GetCurrentTime();
			int timeElapsed = currentTime - prevTime;
			//if its been more than stepTime since our last animation update...
			if(timeElapsed >= stepTime)
			{
				//increment our animation index
				movingIndex++;
				//loop around if at max (goes 0 1 2 3, with 3 being frame '1' so it alternates. See below.)
				if(movingIndex >= numAnimations)
				{
					movingIndex = 0;
				}
				//reset time counter				
				prevTime = currentTime;

				//set the new animation sprite
				SetAnimSprite(movingIndex);
			}

			//start moving forwards
			Move(moveDirection*0.001f,0);

			//if we're at or beyond our moveDistance, move backwards
			if(xPosition >= startX + tileSize*moveDist && moveDirection != -1)
			{
				moveDirection = -1;
			}
			//if we're at or beyond our starting position, move forwards
			if(xPosition <= startX && moveDirection != 1)
			{
				moveDirection = 1;
			}

			
		
	}
	//Flame
	if(trapType == 1)
	{
			currentTime = GetCurrentTime();
			int timeElapsed = currentTime - prevTime;
			//if its been more than stepTime since our last animation update...
			if(timeElapsed >= stepTime)
			{
				//increment our animation index
				movingIndex++;
				//loop around if at max (goes 0 1 2 3, with 3 being frame '1' so it alternates. See below.)
				if(movingIndex >= numAnimations)
				{
					movingIndex = 0;
				}
				//swap whether the trap is on or off
				isDangerous = !isDangerous;

				//reset time counter				
				prevTime = currentTime;

				//set the new animation sprite
				SetAnimSprite(movingIndex);
			}
	}
	if(trapType == 4) //crosshairs
	{
		if(activated)
		{
			//time since last animation
			currentTime = GetCurrentTime();
			int timeElapsed = currentTime - prevTime;
			
			if(timeElapsed >= stepTime)
			{
				if(crossHairCountdown < 10) // 2 seconds of blinking crosshairs at 200 step
				{
					//increment our animation index
					movingIndex++;
					//loop around if at max (goes 0 1 2 3, with 3 being frame '1' so it alternates. See below.)
					if(movingIndex >= 2)
					{
						movingIndex = 0;
					}
				}
				else if(crossHairCountdown < 15)
				{
					//if we're at 10, start the explosion animation
					if(crossHairCountdown == 10)
					{
						movingIndex = 2;
					}
					else
					{
						//increment our animation index
						movingIndex++;
					}
					isDangerous = true;
					
					//loop around if at max (goes 0 1 2 3, with 3 being frame '1' so it alternates. See below.)
					if(movingIndex >= 5)
					{
						movingIndex = 2;
					}
				}
				
				//cooldown time between 15-20 where it can't be activated
				if(crossHairCountdown >= 20)
				{
					activated = false;
					crossHairCountdown = 0;
					prevTime = 0;
				}
				//if its above 15, make it invisible and non dangerous
				else if(crossHairCountdown >= 15)
				{
					//reset time counter				
					prevTime = currentTime;
					//when our countdown is at 15, stop the animation and reset the trap
					movingIndex = 0;
					mySprite->Visible = false; //make crosshairs visible
					mySprite = animationArrayT[0];
					isDangerous = false;
					crossHairCountdown++;
				}
				//if < 15...
				else
				{
					//reset time counter				
					prevTime = currentTime;
					//increment our 'countdown' until explosion
					crossHairCountdown++;

					//set the new animation sprite
					SetAnimSprite(movingIndex);
				}
			}
		}
	}
}

//Multiplies the current scale by the specified amount.
void Trap::AdjustScale(float x, float y)
{
	mySprite->AdjustScaling(x,y);
}

//retrieve x of trap
float Trap::getX()
{
	return xPosition;
}

//retrieve y of trap
float Trap::getY()
{
	return yPosition;
}

//activate this trap (crosshairs)
void Trap::Activate()
{
	mySprite = animationArrayT[0];
	mySprite->Visible = true; //make crosshairs visible
	isDangerous = false;
	crossHairCountdown = 0;
	prevTime = GetCurrentTime();
	activated = true;

}

//adjust the scale for all the animation frames
void Trap::FixFrameScales(float adjustX, float adjustY)
{
	//cycles through the sprites to resize
	for(int i=0; i < numAnimations; i++)
	{
		float adjustX2 = 1.0f;
		float adjustY2 = 1.0f;

		int height = animationArrayT[i]->spriteHeight;
		int width = animationArrayT[i]->spriteWidth;

		if(width > height)
		{
			adjustY2 = ((float)height)/width;
		}
		else if(width < height)
		{
			adjustX2 = ((float)width)/height;
		}

		animationArrayT[i]->AdjustScaling(adjustX2, adjustY2);
	}
}


Trap::~Trap(void)
{

}
