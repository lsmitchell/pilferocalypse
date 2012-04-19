#pragma once
#include "Actor.h"
#include "IController.h"
#include "Wall.h"
#include <iostream>

using namespace std;

#define MAX_SPEED 1.1f
#define X_ACCEL_RATE 1.5f
#define X_FRICTION 3.2f
#define MAX_JUMP_POWER_TIME 0.5f
#define MAX_JUMP_SPEED 1.6f
#define MAX_FALL_SPEED -1.9f
#define FALL_GRAVITY 6.0f

class Wall;
class Trap;

//The PlayerActor represents the player's avatar in the world.
//Remember that this is not a unique object, since this is a
//two-player game.
class PlayerActor : public Actor
{
private:

	IController* controlsSource;
	// indications of which controls are being pressed at the moment.
	bool jumpLastFrame;
	//bool jumpHeld;
	float jumpPower; // value used to enable Mario-style "hold to go higher" jumps
	float moveValue;
	bool isOnGround;

	//Animation Variables
	bool facingRight; //facing right, or facing left
	int movingIndex; //keep track of which index we're on for movement animation
	DWORD currentTime, prevTime;
	//milliseconds between step animations
	static const int stepTime = 200;
	bool moving;
	bool dead; //if the player is currently dead
	int respawnTime; //how long it takes to respawn the player from death in milliseconds
	bool canMove; //whether player movement is disabled or not

	//Holds all the animation frames for a player.
	SimpleSprite** animationArray;
	//number of animations the player has, used to create the array.
	static const int numAnimations = 8;

	float xVelocity, yVelocity;
	float totalTimePassed;
	ICollidable* standingWall;

	void Jump();
	void SetAnimSprite(int idx);

	dxManager* dxm;
public:
	PlayerActor(dxManager*, bool isRed, int playerNumber);
	~PlayerActor(void);
	void doUpdatePhysics(float);
	void doCollision(float*, float*);
	void doTrapCollision(float*, float*);
	bool TestWallCollision(ICollidable* wall, float* nextX, float* nextY);
	bool TestTrapCollision(Trap* wall, float* nextX, float* nextY);
	virtual void Update(float timePassed);
	virtual void AdjustScale(float x, float y);
	void FixFrameScales(float adjustX, float adjustY);
	void Die();
	void SetController(IController* controller);
	IController* GetController();
	int playerNumber;
};

