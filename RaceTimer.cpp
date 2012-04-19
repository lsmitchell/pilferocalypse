#include "StdAfx.h"
#include "RaceTimer.h"


RaceTimer::RaceTimer(void)
{
	isPaused = false;
	isStopped = false;
	pauseVal = 0;
}

RaceTimer::~RaceTimer(void)
{
}

void RaceTimer::start(void)
{
	begin = clock() * CLOCKS_PER_SEC; // start our timer
}

void RaceTimer::delayedStart(diff_type delayNum)
{
	begin = (clock_t)delayNum * 1000000 + (clock() * CLOCKS_PER_SEC);
}

// problem with pause adding time
void RaceTimer::pause(void)
{
	if(isStopped) // we shouldnt be pausing if we have already stopped
		return;

	if(!isPaused)
	{
		end = clock() * CLOCKS_PER_SEC; // stop our timer
		elapsedTicks = end - begin;
	}

	begin = clock() * CLOCKS_PER_SEC - elapsedTicks; // change our begin time to compensate when the timer restarts

	elapsedMilli = (diff_type)elapsedTicks/1000;     //milliseconds from Begin to End
	elapsedSeconds = elapsedMilli/1000;   //seconds from Begin to End
	elapsedMinutes = elapsedSeconds/60;   //minutes from Begin to End

	isPaused = !isPaused;
}

diff_type RaceTimer::stop(void)
{
	end = clock() * CLOCKS_PER_SEC; // stop our timer
	elapsedTicks = end - begin;

	elapsedMilli = (diff_type)elapsedTicks/1000;     //milliseconds from Begin to End
	elapsedSeconds = elapsedMilli/1000;   //seconds from Begin to End
	elapsedMinutes = elapsedSeconds/60;   //minutes from Begin to End

	begin = clock() * CLOCKS_PER_SEC;
	pauseVal = 0;
	isStopped = !isStopped;

	return elapsedSeconds;
}


diff_type RaceTimer::getCurrentSeconds(void)
{
	if(!isPaused && !isStopped)
	{
		elapsedTicks = (int) (clock() * CLOCKS_PER_SEC - (diff_type)begin); // how much time has passed since we started

		elapsedMilli = (diff_type)elapsedTicks/1000;     //milliseconds from Begin to End
		elapsedSeconds = elapsedMilli/1000;   //seconds from Begin to End
		elapsedMinutes = elapsedSeconds/60;   //minutes from Begin to End
	}

	return elapsedSeconds;
}