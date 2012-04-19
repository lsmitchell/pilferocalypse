#pragma once
#include "actor.h"
#include "Checkpoint.h"

#define LEVELPAN_INACTIVE -1
#define LEVELPAN_PREP_PHASE 0
#define LEVELPAN_PAN_PHASE 1
#define LEVELPAN_PAUSE_PHASE 2
#define LEVELPAN_OVERVIEW_PHASE 3
#define LEVELPAN_FINISHED_PHASE 4
#define TOTAL_PANNING_TIME 10.0
#define TOTAL_POSTPAN_PAUSE 3.0
#define TOTAL_ZOOMOUT_TIME 5.0

class GameManager;

class LevelPanGhost :
	public Actor
{
private:
	GameManager* gameMan;
	int phase;
	float timeProgression;
	Checkpoint* startPoint;
	Checkpoint* endPoint;
public:
	LevelPanGhost(GameManager* gm);
	~LevelPanGhost(void);
	bool Finished();
	void MakeActive();
	void Cancel();
	void Update(float timePassed);
};

