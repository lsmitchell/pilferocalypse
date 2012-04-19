#pragma once
#include "MapLoader.h"
//#include "PlayerActor.h" // We know about this through CheckPointManager
#include "IController.h"
#include "SoundManager.h"
#include "CheckPointManager.h"
#include "LevelPanGhost.h"
#include "RaceTimer.h"
#include "RaceGate.h"
#include <string>

#define MAP_BLOCK_SIZE 0.535f
#define CONTROLLERS 4
//#define CONTROLLERS 4 //this will be 4 when we have arrow keys and controllers

class GameManager
{
private:
	dxManager* dx; // pointer to the dxManager
	PlayerActor* player1;
	PlayerActor* player2;
	SimpleSprite* selector;
	SimpleSprite* menuScreen;
	SimpleSprite* instructScreen;
	SimpleSprite* timesScreen;
	SimpleSprite* lobbyScreen;
	SoundManager soundManager;
	MapLoader* loader;
	CheckPointManager* checkpointManager;
	RaceTimer* raceTimer;
	GameState gameState; // uses enum defined in dxManager
	MenuSelector currentMenuSelection;
	int numActors;
	int numTraps;
	int numOfPlayers;
	IController** controllers;
	int WinningPlayerNum; // The Player who wins the game
	float winningTime;
	Checkpoint* finishPoint; // the point you need to reach to finish the game
	LevelPanGhost* panGhost;
	RaceGate* gate;
	string winnerName;
	int letterCounter; // used for when the winner enters their name
	char** instructTips;
	int numOfInstructPages;
	SimpleSprite** instructPics;
	bool controllersSelected;
	int numControllersSelected;

	void InitalizeGame();
	void InitalizeMenu();
	void StartOverview();
	void BeginGame();
	void FixScaling(float adjustX, float adjustY);
	void CheckMenuKeyPresses();
	void EnterNameText(WPARAM wparam);
	void RecordBestTimes();
	void ReadBestTimes();
	void InitalizeInstructMenu();
	
public:
	GameManager(dxManager* dMan);
	~GameManager(void);
	GameState GetGameState();
	dxManager* getDXRef();
	Checkpoint* GetLeftmostCheckpoint();
	Checkpoint* GetFinalCheckpoint();
	void ChangeMenuSelection(int val);
	PlayerActor* GetPlayer(int);
	MenuSelector GetMenuSelect();
	void KeyDown(WPARAM);
	void KeyUp(WPARAM);
	void GameLoop(float time);
	void MenuItemSelected();
	void TogglePauseGame();
	void BackToMenu();
	void CleanUp(void);
};

