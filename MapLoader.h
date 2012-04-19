#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include "SimpleSprite.h"
#include "Wall.h"
#include "dxManager.h"
#include "Globals.h"
#include "Checkpoint.h"
#include "Trap.h"

using namespace std;

class MapLoader
{
public:
	MapLoader(dxManager* dx, char* mapFileName,char* keyFileName, int rows, int cols, float blockSize);
	~MapLoader(void);
	float getMapLengthP();
	float getMapHeightP();
	void getP1Start(float*, float*);
	void getP2Start(float*, float*);
	Checkpoint* getFinishPoint();
	int getNumOfCheckpoints(void);
	Checkpoint** checkpointArray; // this will be passed into CheckPointManager

private:
	dxManager* dx;
	char** mapArray;
	map<char, char*> keyList;
	static const int NUM_OF_CHECKPOINTS = 10; // used to initalize our checkpoint array
	int checkpointCounter;
	int mapLength;
	int mapHeight;
	float gridBlockSize;
	float p1StartX, p1StartY, p2StartX, p2StartY;
	Checkpoint* finishPoint;
	int readMapFile(char* fileName);
	int readKeyFile(char* fileName);
	void createMap(float topLeftX, float topLeftY);
	TCHAR* charToTCHAR(char* myString);
};

