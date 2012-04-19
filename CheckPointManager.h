#pragma once
#include "PlayerActor.h"
#include "Checkpoint.h"

class CheckPointManager
{
public:
	CheckPointManager(Checkpoint** checkArray, int numOfChckpts);
	~CheckPointManager(void);
	Checkpoint* LeftmostCheckpoint();
	void SendToLastChckPt(PlayerActor* player, int playerNumber);
	void ResetCheckpoints();
private:
	Checkpoint** checkpointArray;
	int numOfCheckpoints;
	Checkpoint* findNextCheckpoint(PlayerActor* player);
	Checkpoint* p1CurrentCheckpoint;
	Checkpoint* p2CurrentCheckpoint;
};

