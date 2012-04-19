#include "StdAfx.h"
#include "CheckPointManager.h"


CheckPointManager::CheckPointManager(Checkpoint** checkArray, int numOfChckpts)
{
	checkpointArray = checkArray; // grab our array of checkpoints
	numOfCheckpoints = numOfChckpts; // find out how many checkpoints there are

	// initalize currentCheckpoints just in case
	p1CurrentCheckpoint = new Checkpoint(-1000.0f, 0);
	p2CurrentCheckpoint = new Checkpoint(-1000.0f, 0);
}

Checkpoint* CheckPointManager::LeftmostCheckpoint()
{
	Checkpoint* leftmost = checkpointArray[0];
	for (int i = 1; i < numOfCheckpoints; i ++)
	{
		if (checkpointArray[i]->x < leftmost->x)
			leftmost = checkpointArray[i];
	}
	return leftmost;
}

// Pass in a refrence to the player and whether it is player 1 or player 2
// Will reset the Player's poistion based on which checkpoint they were closest to last
void CheckPointManager::SendToLastChckPt(PlayerActor* player, int playerNumber)
{
	if(playerNumber == 1)
	{
		
		Checkpoint* checkPtP1 = findNextCheckpoint(player); // find the next checkpoint based on where we are
	
		// make sure we havent already passed this checkpoint by comparing x coords
		float dxP1 = p1CurrentCheckpoint->x - checkPtP1->x;

		// if the old checkpoint was before the newly found one, set checkPt as the new one
		if(dxP1 < 0)
		{
			p1CurrentCheckpoint = checkPtP1;
		}

		player->SetPosition(p1CurrentCheckpoint->x, p1CurrentCheckpoint->y);
	}
	if(playerNumber == 2)
	{
		
		Checkpoint* checkPtP2 = findNextCheckpoint(player); // find the next checkpoint based on where we are
	
		// make sure we havent already passed this checkpoint by comparing x coords
		float dxP2 = p2CurrentCheckpoint->x - checkPtP2->x;

		// if the old checkpoint was before the newly found one, set checkPt as the new one
		if(dxP2 < 0)
		{
			p2CurrentCheckpoint = checkPtP2;
		}

		player->SetPosition(p2CurrentCheckpoint->x, p2CurrentCheckpoint->y);
	}
}

// Returns the checkpoint that the player has just passed
Checkpoint* CheckPointManager::findNextCheckpoint(PlayerActor* player)
{
	// Grab the players position
	Checkpoint playerPos(player->getX(), player->getY());

	float closestDist = 1000000;
	Checkpoint* closestPoint = checkpointArray[0]; // in case we go behind the first checkpoint

	// loop through to find out which checkpoint he is closest to
	for(int i = 0; i < numOfCheckpoints; i++)
	{
		Checkpoint* checkPoint = checkpointArray[i];

		// just need to find the closest so not doing Math.sqrt
		float dx = (playerPos.x - checkPoint->x);

		float dist = dx*dx;

		// we found a checkpoint that we are closest to and infront of
		if(dx >= 0 && dist < closestDist)
		{
			closestDist = dist;
			closestPoint = checkPoint;
		}
	}

	return closestPoint;
}

// Resets P1 and P2 Checkpoints
void CheckPointManager::ResetCheckpoints(void)
{
	p1CurrentCheckpoint = new Checkpoint(-1000.0f, 0);
	p2CurrentCheckpoint = new Checkpoint(-1000.0f, 0);
}

CheckPointManager::~CheckPointManager(void)
{
}
