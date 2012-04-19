#include "StdAfx.h"
#include "PlayerActor.h"
#include "dxManager.h"

#define COLLISION_MINISCULE_OFFSET 0.01f

void	moveAll(float, float);

//sets up the player and tells the person whether they're first or second
PlayerActor::PlayerActor(dxManager* dx, bool isRed, int playerNum) : Actor(dx)
{
	animationArray = new SimpleSprite*[numAnimations];
	//get a ref to the dxmanager display device to make sprites here
	ID3D10Device* pD3DDevice = dx->getPD3DDevice();

	playerNumber = playerNum;
	dxm = dx;
	
	int numPlayerSprites = 9;

	if(isRed)
	{
		WCHAR* spriteNames[] = {L"Images/RedWalk1.png", L"Images/RedWalk2.png", L"Images/RedWalk3.png"
		, L"Images/RedJump.png", L"Images/RedWalk1Left.png", L"Images/RedWalk2Left.png"
		, L"Images/RedWalk3Left.png", L"Images/RedJumpLeft.png", L"Images/RedPoof.png"};

		//load red sprites
		for (int i = 0; i < numPlayerSprites; i ++)
		{
			dx->MakeSprite(spriteNames[i], &animationArray[i]);
			animationArray[i]->Visible = false;
		}
	}
	else
	{
		WCHAR* spriteNames[] = {L"Images/BlueWalk1.png", L"Images/BlueWalk2.png", L"Images/BlueWalk3.png"
		, L"Images/BlueJump.png", L"Images/BlueWalk1Left.png", L"Images/BlueWalk2Left.png"
		, L"Images/BlueWalk3Left.png", L"Images/BlueJumpLeft.png", L"Images/BluePoof.png"};

		for (int i = 0; i < numPlayerSprites; i ++)
		{
			dx->MakeSprite(spriteNames[i], &animationArray[i]);
			animationArray[i]->Visible = false;
		}
	}

	//make the starting sprite the standing still, right sprite.
	mySprite = animationArray[1];
	SetWidthHeight();
	facingRight = true;
	moving = false;
	movingIndex = 0;

	//gravity variables
	xVelocity = 0;
	yVelocity = 0;
	isOnGround = true;
	canMove = true;
	dead = false;
	respawnTime = 2000;
	//jumpHeld = false;
	totalTimePassed = 0.0f;
}

//deconstructor
PlayerActor::~PlayerActor(void)
{
}

//makes the playyer jump
void PlayerActor::Jump()
{
	jumpPower = MAX_JUMP_POWER_TIME;
	yVelocity = MAX_JUMP_SPEED;
	isOnGround = false;
}

//sets the variables to load the animation sprites
void PlayerActor::SetAnimSprite(int idx)
{
	mySprite->Visible = false;
	mySprite = animationArray[idx];
	mySprite->Visible = true;

}

//kills the player
void PlayerActor::Die()
{
	prevTime = GetCurrentTime();
	dead = true;
	canMove = false;
}

// we had encountered issues with the sprite not being scaled correctly; this is a manual method of fixing that by altering
// the sprite width and height.
void PlayerActor::FixFrameScales(float adjustX, float adjustY)
{
	//cycles through the sprites to resize
	for(int i=0; i < numAnimations; i++)
	{
		animationArray[i]->AdjustScaling(0.4f, 0.4f);
		//animationArray[i]->AdjustScaling(adjustX, adjustY);

		float adjustX2 = 1.0f;
		float adjustY2 = 1.0f;

		int height = animationArray[i]->spriteHeight;
		int width = animationArray[i]->spriteWidth;

		if(width > height)
		{
			adjustY2 = ((float)height)/width;
		}
		else if(width < height)
		{
			adjustX2 = ((float)width)/height;
		}

		animationArray[i]->AdjustScaling(adjustX2, adjustY2);
	}
}

//checks for wall collisions
void PlayerActor::doCollision(float* nextX, float* nextY)
{

	// wall collision
	
	// we will loop through and find the "standing wall" again. If there suddenly isn't one,
	// the player will start falling. (this should be set in TestWallCollision)
	standingWall = 0;
	float prevNextY = *nextY;

	list<ICollidable*>* walls = dxMan->GetWalls();
	for (list<ICollidable*>::iterator iter = walls->begin(); iter != walls->end(); ++iter)
	{
		ICollidable* wallTest = *iter;
		
		bool hit = TestWallCollision(wallTest, nextX, nextY);
		if (hit)
		{
			wallTest->OnPlayerCollide(this);
		}
	}

	if (standingWall == 0)
	{
		isOnGround = false;
	}
}

//checks for trap collisions
void PlayerActor::doTrapCollision(float* nextX, float* nextY)
{
	//loop through the traps and check collision
	vector<Trap*>* traps = dxMan->GetTraps();
	for(vector<Trap*>::iterator iter = traps->begin(); iter != traps->end(); ++iter)
	{
		Trap* trapTest = *iter;
		bool hit = TestTrapCollision(trapTest, nextX, nextY);
		if (hit)
		{
			//check if trap is dangerous at this time
			if(trapTest->trapWillKill() || trapTest->trapType == 3)
			{
				trapTest->OnPlayerCollide(this);
			}
		}
	}
}

//this function takes two potential variables for the next position, and checks
//for any collision with the current wall. Since it takes pointers, it modifies
//the position to correct for collision.
bool PlayerActor::TestWallCollision(ICollidable* wall, float* nextX, float* nextY)
{
	// collision detection done using seperating axis theorem, summary found here:
	// http://www.metanetsoftware.com/technique/tutorialA.html
	float wallHalfX, wallHalfY;
	wall->GetHalfWidth(&wallHalfX, &wallHalfY);
	float wallX, wallY;
	wall->GetPosition(&wallX, &wallY);
	float differenceX, differenceY, signX, signY; // green lines, figure 3
	differenceX = *nextX - wallX; // distance FROM wall TO player
	signX = sign(differenceX);
	differenceX = abs(differenceX);

	// maximum amount the two objects can be seperated, otherwise there is a collision
	float maxSeperX = halfXWidth + wallHalfX;
	float maxSeperY = halfYWidth + wallHalfY;

	//if we're farther than the max X seperation distance, we're not colliding.
	if (differenceX > maxSeperX)
	{
		return false;
	}
	
	//if we're here, we know the X plane is colliding, so check Y.
	
	differenceY = *nextY - wallY;
	signY = sign(differenceY);
	differenceY = abs(differenceY);

	// check if this wall is under our feet, set it as the standing wall.
	if (isOnGround)
	{
		if (signY == 1.0 && differenceY < maxSeperY + COLLISION_MINISCULE_OFFSET)
		{
			standingWall = wall;
		}
	}
	if (differenceY > maxSeperY)
	{
		return false;
	}

	//now that we know we're colliding, do special adjustments

	float collisionAmtX = maxSeperX - differenceX;
	float collisionAmtY = maxSeperY - differenceY;

	//check if they're colliding with a wall on the side
	if (collisionAmtY > collisionAmtX)
	{
		// fix along X axis
		xVelocity = 0.0f;
		*nextX = *nextX + collisionAmtX * signX;
	}
	else if (!isOnGround) // only accept Y collisions if we aren't already on the ground
	{
		// fix along Y axis
		if (yVelocity < 0.0f && signY == 1.0f && !isOnGround)
		{
			// just landed
			isOnGround = true;
			standingWall = wall;
		}
		if (yVelocity > 0.0f && signY == -1.0f && !isOnGround)
		{
			jumpPower = 0.0f;
		}
		yVelocity = 0.0f;
		*nextY = *nextY + collisionAmtY * signY;
	}
	return true;
}

bool PlayerActor::TestTrapCollision(Trap* trap, float* nextX, float* nextY)
{

	// uses a variant of seperating axis that doesn't bother computing collision-handling
	// (we don't need to relocate a doomed player)
	float trapHalfX, trapHalfY;
	trap->GetHalfWidth(&trapHalfX, &trapHalfY);
	float trapX, trapY;
	trap->GetPosition(&trapX, &trapY);
	float differenceX, differenceY; // green lines, figure 3
	differenceX = *nextX - trapX; // distance FROM trap TO player
	differenceY = *nextY - trapY;
	differenceX = abs(differenceX);
	differenceY = abs(differenceY);

	// maximum amount the two objects can be seperated, otherwise there is a collision
	float maxSeperX = halfXWidth + trapHalfX;
	float maxSeperY = halfYWidth + trapHalfY;
	if (differenceX > maxSeperX)
	{
		return false;
	}
	// now we know X is colliding
	if (differenceY > maxSeperY)
	{
		return false;
	}
	
	//if the trap is currently dangerous
	if(trap->trapWillKill())
	{
		yVelocity = 0.0f;
		jumpPower = 0.0f;
	}
	return true;
}

//updates the player actor in time by the specified
//elapsing of time
void PlayerActor::Update(float timePassed)
{
	if (!timePassed) return;
	totalTimePassed += timePassed;
	cout << "T Passed: " << totalTimePassed;

	//check the collisions of walls and traps, as well as movement and gravity
	doUpdatePhysics(timePassed);
	
}

void PlayerActor::doUpdatePhysics(float timePassed)
{
	float xMove = controlsSource->getXMove();
	bool jump = controlsSource->getJump();
	//keep track of which direction we were facing last
	//check if a current direction is held down
	bool wasMoving = moving; //check if we were moving in the last frame; so we can set moving timer if not and current are moving

	
	//in the case we can't move or are dead, we'll just stay where we are
	float nextX = xPosition;
	float nextY = yPosition;

	//only do things if the character isn't dead
	if(!dead)
	{
	//check if we are holding left or right; whether we are moving
	if(controlsSource->getLeft())
	{
		facingRight = false;
		moving = true;
	}
	else if(controlsSource->getRight())
	{
		facingRight = true;
		moving = true;
	}
	else
	{
		moving = false;
	}

	//if we're moving and previously weren't, set the 'timer' for motion animation
	if(!wasMoving && moving)
	{
		prevTime = GetCurrentTime();
	}

	if (jump) // if jump held
	{
		//only jump if the player is allowed to currently move
		if(canMove)
		{
			//jumpHeld = true;
			if (isOnGround && !jumpLastFrame)
			{
				Jump();
			}
			if (jumpPower > 0.0f)
			{
				jumpPower -= timePassed;
				yVelocity = MAX_JUMP_SPEED;
			}
			jumpLastFrame = true;
		}
	}
	else
	{
		// jump button is not held
		if (!isOnGround)
		{
			jumpPower = 0.0f;
		}
		jumpLastFrame = false;
	}
	xVelocity += X_ACCEL_RATE * xMove * MAX_SPEED;
	xVelocity = min(max(xVelocity, -MAX_SPEED), MAX_SPEED);
	if (!xMove)
	{
		if (xVelocity > 0.0f)
		{
			xVelocity -= X_FRICTION * timePassed;
		}
		else if (xVelocity < 0.0f)
		{
			xVelocity += X_FRICTION * timePassed;
		}
		if (abs(xVelocity) < 0.01f)
		{
			xVelocity = 0.0f;
		}
	}

	if(!isOnGround && jumpPower <= 0.0f)
	{
		yVelocity -= FALL_GRAVITY * timePassed;
		yVelocity = max(MAX_FALL_SPEED, yVelocity);
	}


	//else we'll move according to velocity
	if(canMove && !dead)
	{
		nextX = xPosition + (timePassed * xVelocity);
		nextY = yPosition + (timePassed * yVelocity);

		doCollision(&nextX, &nextY);
	}

	
	

	//CHARACTER ANIMATION
	//check if in the air, either falling or jumping. Also check direction.

	
		if(!isOnGround)
		{
			if(facingRight)
				SetAnimSprite(3);
			else
				SetAnimSprite(7);
		}
		else //If we're not in the air, we're on the ground! Check for motion.
		{
			//if We're moving...
			if(moving)
			{
				//check how long we've been moving
				currentTime = GetCurrentTime();
				int timeElapsed = currentTime - prevTime;
				//if its been more than stepTime since our last animation update...
				if(timeElapsed >= stepTime)
				{
					//increment our animation index
					movingIndex++;
					//loop around if at max (goes 0 1 2 3, with 3 being frame '1' so it alternates. See below.)
					if(movingIndex > 3)
					{
						movingIndex = 0;
					}
					//reset time counter
					prevTime = currentTime;
				
				}

				//if we're on frame '4' of our animation, use the middle idle animation that goes between walk frames.
				if(movingIndex == 3)
				{
					if(facingRight)
						SetAnimSprite(1);
					else
						SetAnimSprite(5);
				}
				//otherwise just use the movingIndex location. If facing left offset by 4.
				else
				{
					if(facingRight)
						SetAnimSprite(movingIndex);
					else
						SetAnimSprite(4+movingIndex);
				}
			}
			else //no motion
			{
				//we're not jumping, and not moving, we're standing still. So change the sprite.
				if(facingRight)
					SetAnimSprite(1);
				else
					SetAnimSprite(5);
			}
		}

		doTrapCollision(&nextX, &nextY);

		SetPosition(nextX, nextY);

	}
	else //if we're dead, show the dead animation for half the respawn time
	{
		
		//show the death sprite
		SetAnimSprite(8);

		SetPosition(nextX, nextY);

		currentTime = GetCurrentTime();
		int timeElapsed = currentTime - prevTime;

		//once the respawn time is over, the player can move again and isn't dead.
		if(timeElapsed >= respawnTime)
		{
			canMove = true;
			dead = false;
		}
		//pause for half the respawn time to show the player where they died
		else if(timeElapsed > respawnTime/2)
		{
			//when we're at half respawn time passed, move the player to the last checkpoint
			dxm->KillPlayer(this, playerNumber);
		}

		
	}

	
}

//Sets which 'controller' object to check controls from.
//This determines where to get input from; WASD, arrows, controller
void PlayerActor::SetController(IController* controller)
{
	controlsSource = controller;
}

IController* PlayerActor::GetController(void)
{
	return controlsSource;
}

//Multiplies the current scale by the specified amount.
void PlayerActor::AdjustScale(float x, float y)
{
	mySprite->AdjustScaling(x,y);
}