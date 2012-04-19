#include "StdAfx.h"
#include "LevelPanGhost.h"
#include "GameManager.h"

extern Camera* mainCamInst;

LevelPanGhost::LevelPanGhost(GameManager* gm) : Actor(gm->getDXRef())
{
	gameMan = gm;
	phase = LEVELPAN_INACTIVE;
	startPoint = gm->GetLeftmostCheckpoint();
	endPoint = gm->GetFinalCheckpoint();
	timeProgression = 0.0f;
}


LevelPanGhost::~LevelPanGhost(void)
{
}

bool LevelPanGhost::Finished()
{
	return (phase == LEVELPAN_FINISHED_PHASE);
}

void LevelPanGhost::MakeActive()
{
	::mainCamInst->ClearTargets();
	::mainCamInst->AddTrackingTarget(this);
	phase = LEVELPAN_PREP_PHASE;
}

void LevelPanGhost::Cancel()
{
	phase = LEVELPAN_FINISHED_PHASE;
	::mainCamInst->ClearTargets();
}

void LevelPanGhost::Update(float timePassed)
{
	timeProgression += timePassed;
	switch(phase)
	{
	case LEVELPAN_PREP_PHASE:
		timeProgression = 0.0f;
		phase = LEVELPAN_PAN_PHASE;
		break;
	case LEVELPAN_PAN_PHASE:
		if (timeProgression < TOTAL_PANNING_TIME)
		{
			float interpAmt = timeProgression / (float) TOTAL_PANNING_TIME;
			float newX = (endPoint->x - startPoint->x) * interpAmt + startPoint->x;
			float newY = (endPoint->y - startPoint->y) * interpAmt + startPoint->y;
			SetPosition(newX, newY);

		}
		else
		{
			timeProgression = 0.0f;
			phase = LEVELPAN_PAUSE_PHASE;
		}
		break;
	case LEVELPAN_PAUSE_PHASE:
		if (timeProgression >= TOTAL_POSTPAN_PAUSE)
		{
			// move on to overview phase
			::mainCamInst->ClearTargets();
			::mainCamInst->AddTrackingTarget(startPoint);
			::mainCamInst->AddTrackingTarget(endPoint);
			timeProgression = 0.0f;
			phase = LEVELPAN_OVERVIEW_PHASE;
		}
		break;
	case LEVELPAN_OVERVIEW_PHASE:
		if (timeProgression >= TOTAL_ZOOMOUT_TIME)
		{
			::mainCamInst->ClearTargets();
			timeProgression = 0.0f;
			phase = LEVELPAN_FINISHED_PHASE;
		}
		break;
	}
}