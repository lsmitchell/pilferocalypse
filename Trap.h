#pragma once
#include "ICollidable.h"
#include "PlayerActor.h"

using namespace std;

class Trap : public ICollidable
{
private:
	//Holds all the animation frames for a player.
	SimpleSprite** animationArrayT;
	//number of animations the player has, used to create the array.
	int numAnimations;

	int movingIndex; //keep track of which index we're on for movement animation
	DWORD currentTime, prevTime;
	//milliseconds between animations
	int stepTime;
	//how many 'squares' forward it will oscillate back and forth from its placement position
	float moveDist;
	float tileSize;
	int moveDirection;
	//where the trap was placed initially; useful for resetting
	float startX;
	float startY;
	bool isDangerous; //whether the trap is currently in a state that will kill the player
	Trap* triggerTarg; //what trap this will set off
	int crossHairCountdown;
public:
	Trap(dxManager* dx, char* type, float xPos, float yPos);
	~Trap(void);
	virtual void Update(float timePassed);
	void SetAnimSprite(int idx);
	void SetAnimStepTime(int newStepTime);
	virtual void AdjustScale(float x, float y);
	void FixFrameScales(float adjustX, float adjustY);
	virtual float Left();
	virtual float Right();
	virtual float Top();
	virtual float Bottom();
	bool trapWillKill();
	void SetTriggerTarget(Trap* target);
	void setTileSize(float size);
	void OnPlayerCollide(PlayerActor* player);
	float getX();
	float getY();
	void Activate();
	bool activated; //if the parent turret has activated a crosshair
	//type of trap
	int trapType;
};

