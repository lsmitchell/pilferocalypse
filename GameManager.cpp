#include "StdAfx.h"
#include "GameManager.h"
#include "Camera.h"
#include "WASDKeyboardInput.h"
#include "ArrowKeyboardInput.h"
#include "XinputController.h"
#include "RaceGate.h"

extern Camera* mainCamInst;

GameManager::GameManager(dxManager* dMan)
{
	dx = dMan;
	gameState = StartMenu; // we start the game off at the menu
	currentMenuSelection = PlayGame; // The Menu Starts off with PlayGame selected
	controllersSelected = false;
	numOfPlayers = 2;
	winningTime = 0.0f;
	WinningPlayerNum = 0; 
	winnerName = "---";
	letterCounter = 0;
	numControllersSelected = 0;
	dx->numControllersSelected = controllersSelected;

	InitalizeGame();
	InitalizeMenu();

	FixScaling(dx->adjustX, dx->adjustY);
	player1->FixFrameScales(dx->adjustX, dx->adjustY); //Fix the player's animation scales
	dx->gameState = &gameState; // tell dxManager to refer to the GameManager to get the Game's State
	dx->numOfPlayers = &numOfPlayers;
	dx->winningPlayer = &WinningPlayerNum;
	dx->winnerName = &winnerName;

	// refrences to actors and traps
	numActors = dx->NumActors();
	numTraps = dx->NumTraps();

	//Sound stuff
	soundManager.LoadSoundFile("Sounds\\Walk Stab Walk.wav");
	soundManager.LoadSoundFile("Sounds\\Time on My Side.wav");

	// After Everything is loaded we initalize our timer
	raceTimer = new RaceTimer();

	// Tips that are displayed on the instruction menu
	InitalizeInstructMenu();

	// Set Up the Best Times Menu Score Listing
	ReadBestTimes();
}

//sets up the game to start playing
void GameManager::InitalizeGame()
{
	dx->loadPicsFromTextFile("TextFiles/myPics.txt");
	dx->MakeSprite(L"Images/hall1.bmp");
	dx->MakeSprite(L"Images/hall2.bmp");
	dx->MakeSprite(L"Images/hall3.bmp");
	dx->MakeSprite(L"Images/hall2.bmp");
	dx->MakeSprite(L"Images/hall4.bmp");
	dx->MakeSprite(L"Images/hall2.bmp");
	dx->MakeSprite(L"Images/hall2.bmp");
	dx->MakeSprite(L"Images/hall2.bmp");
	dx->MakeSprite(L"Images/hall6.bmp");

	//setup the background
	for(int j =0; j < 9; j++)
	{
		SimpleSprite* currentSprite;
		currentSprite = dx->getSprite(j);
		currentSprite->SetPosition(j*3.225f-0.8f, 0.0f, 0.0f);
		currentSprite->AdjustScaling(6.0f,6.0f);
	}
	
	// Load the map from a text file
	loader = new MapLoader(dx, "TextFiles/MapRepresentation.txt", "TextFiles/MapKey.txt", 6,59, MAP_BLOCK_SIZE);
	
	// Create our checkpoint Manager by using values stored in the map loader
	checkpointManager = new CheckPointManager(loader->checkpointArray, loader->getNumOfCheckpoints());

	//set the checkpoint manager for dx to handle player deaths
	dx->cpm = checkpointManager;

	// set up our player
	float player1x, player1y;
	loader->getP1Start(&player1x, &player1y);

	// Get our finishing point for the race
	finishPoint = loader->getFinishPoint();

	// set up the level pan object for the start
	panGhost = new LevelPanGhost(this);

	gate = new RaceGate(dx);
	dx->addActor(panGhost);
	dx->AddWall(gate);

	// set up controllers
	controllers = new IController*[CONTROLLERS];
#ifndef XBOX_CONTROLLERS
	controllers[0] = new WASDKeyboardInput();
	controllers[1] = new ArrowKeyboardInput();
#else
	controllers[0] = new XinputController(0);
	controllers[1] = new XinputController(1);
#endif
	controllers[2] = new XinputController(0);
	controllers[3] = new XinputController(1);

	//load the player
	player1 = new PlayerActor(dx, false, 1); //is blue for now, so pass in false
	player1->SetPosition(player1x, player1y);

	::mainCamInst->CenterOn(player1);
	dx->addActor(player1);
	dx->addActor(::mainCamInst);

	float player2x, player2y;
	loader->getP2Start(&player2x, &player2y);
	player2 = new PlayerActor(dx, true, 2);
	player2->FixFrameScales(dx->adjustX, dx->adjustY);
	player2->SetPosition(player2x, player2y);
	dx->addActor(player2);
}

// Initalizes the Sprites used for out menus and sets their positions
void GameManager::InitalizeMenu()
{
	dx->MakeMenuSprite(L"Images/Menu/MenuScreen.png", &menuScreen, MainMenu);
	dx->MakeMenuSprite(L"Images/Menu/Instructions.jpg", &instructScreen, InstructMenu);
	dx->MakeMenuSprite(L"Images/Menu/BestTimes.jpg", &timesScreen, TimeMenu);
	dx->MakeMenuSprite(L"Images/Menu/PlayerSelect.jpg", &lobbyScreen, LobbyMenu);
	dx->MakeMenuSprite(L"Images/Menu/selector.png", &selector, Selector);

	// place our red box selector and menu, and set what kind of sprites they are
	selector->SetPosition(0.01f, -0.09f, 0.0f);

	menuScreen->SetPosition(0.0f, 0.0f, 0.0f);
	instructScreen->SetPosition(0.0f, 0.0f, 0.0f);
	timesScreen->SetPosition(0.0f, 0.0f, 0.0f);
	lobbyScreen->SetPosition(0.0f, 0.0f, 0.0f);

	// scale them up, for some reason they are really small at first
	instructScreen->AdjustScaling(3.5f,3.5f);
	timesScreen->AdjustScaling(3.5f,3.5f);
}

// fixes the scaling of the sprites due to screen resolution problem
void GameManager::FixScaling(float adjustX, float adjustY)
{
	int numberSprites = dx->NumSprites();

	// fix the scaling of all our normal sprites
	for (int i = 0; i < numberSprites; i++)
	{
		SimpleSprite* sprite = dx->getSprite(i);
		//sprite->AdjustScaling(adjustX, adjustY);

		float adjustX2 = 1.0f;
		float adjustY2 = 1.0f;
		if(sprite->spriteWidth > sprite->spriteHeight)
		{
			adjustY2 = ((float)sprite->spriteHeight)/sprite->spriteWidth;
		}
		else if(sprite->spriteWidth < sprite->spriteHeight)
		{
			adjustX2 = ((float)sprite->spriteWidth)/sprite->spriteHeight;
		}

		sprite->AdjustScaling(adjustX2, adjustY2);
	}

	int numberMenuSprites = dx->NumMenuSprites();

	
	// fix the scaling of all our menu sprites
	for (int j = 0; j < numberMenuSprites; j++)
	{
		SimpleSprite* sprite = dx->getMenuSprite(j);
		sprite->AdjustScaling(adjustX, adjustY);

		
		float adjustX2 = 1.0f;
		float adjustY2 = 1.0f;
		if(sprite->spriteWidth > sprite->spriteHeight)
		{
			adjustY2 = ((float)sprite->spriteHeight)/sprite->spriteWidth;
		}
		else if(sprite->spriteWidth < sprite->spriteHeight)
		{
			adjustX2 = ((float)sprite->spriteWidth)/sprite->spriteHeight;
		}

		sprite->AdjustScaling(adjustX2, adjustY2);
	}

	// menu isnt resized correctly for some reason, this fixes it
	menuScreen->AdjustScaling(4.5f, 4.5f); // scale it up
	lobbyScreen->AdjustScaling(4.5f, 4.5f);
}

dxManager* GameManager::getDXRef()
{
	return dx;
}

Checkpoint* GameManager::GetLeftmostCheckpoint()
{
	return checkpointManager->LeftmostCheckpoint();
}

Checkpoint* GameManager::GetFinalCheckpoint()
{
	return finishPoint;
}

//figures out the state of the game
GameState GameManager::GetGameState()
{
	return gameState;
}

// pass in +1 or -1 to go up or down on the menu
void GameManager::ChangeMenuSelection(int val)
{
	if(gameState == StartMenu) // we only want to try this if we are in the start menu state
	{
		int num = currentMenuSelection; // grab the value of the enum we are currently using
		num += val;

		if(num >= 3) // we only have 3 choices for this enum, 0 - 2
			num = 0;
		else if(num < 0)
			num = 2;

		currentMenuSelection = (MenuSelector) num; // assign our new menu selection

		// Placement of the red selection box
		if(currentMenuSelection == PlayGame)
		{
			selector->SetPosition(0.01f, -0.09f, 0.0f);
		}
		else if(currentMenuSelection == BestTimes)
		{
			selector->SetPosition(0.01f, -0.40f, 0.0f);
		}
		else if(currentMenuSelection == Instructions)
		{
			selector->SetPosition(0.01f, -0.7f, 0.0f);
		}
	}
	else if(gameState == Lobby)
	{
		int num = currentMenuSelection; // grab the value of the enum we are currently using
		num += val;

		if(num > 5) // we only have 3 choices for this enum, 0 - 2
			num = 4;
		else if(num < 4)
			num = 5;

		currentMenuSelection = (MenuSelector) num; // assign our new menu selection
		
		if(currentMenuSelection == OnePlayer)
		{
			selector->SetPosition(-0.21f, -0.35f, 0.0f);
		}
		else if(currentMenuSelection == TwoPlayer)
		{
			selector->SetPosition(0.22f, -0.35f, 0.0f);
		}
	}
	

}

//gets the object of the player
PlayerActor* GameManager::GetPlayer(int num)
{
	if (num == 1)
		return player1;
	else if (num == 2 && numOfPlayers == 2)
		return player2;
	else
		// tried to get player that did not exist!
		return NULL;
}

//gets the menu selector object
MenuSelector GameManager::GetMenuSelect()
{
	return currentMenuSelection;
}

void GameManager::KeyDown(WPARAM wparam)
{
	
	switch(wparam)
	{
		case(VK_ESCAPE):BackToMenu();
			return;
			break;
		case(VK_SUBTRACT): soundManager.ChangeVolume(-0.1f);
			break;
		case(VK_ADD): soundManager.ChangeVolume(0.1f);
			break;
	}
	for (int i = 0; i < CONTROLLERS; i ++)
	{
		controllers[i]->KeyDown(wparam);
	}

	// This is Where we enter our name for the high score
	if(gameState == GameOver)
	{
		EnterNameText(wparam);
	}
}

void GameManager::KeyUp(WPARAM wparam)
{
	for (int i = 0; i < CONTROLLERS; i ++)
	{
		controllers[i]->KeyUp(wparam);
	}
}

// where all the magic happens. Update.
void GameManager::GameLoop(float time)
{
	// call update on all controllers
	for (int i = 0; i < CONTROLLERS; i ++)
	{
		controllers[i]->Update(time);
	}

	// Only update our sprites if we are playing the game
	if (gameState == Overview)
	{
		if(soundManager.GetIsPaused()) // Will unpause the looping sound
			soundManager.PauseCurrentSound();

		soundManager.LoopSound(1);

		panGhost->Update(time);
		::mainCamInst->Update(time);
		if (panGhost->Finished())
		{
			BeginGame();
		}
	}
	else if(gameState == PlayingGame)
	{
		diff_type timerTime = raceTimer->getCurrentSeconds();
		dx->gameTime = raceTimer->getCurrentSeconds(); // send dxManager our time
		
		if (gate->IsActive() && timerTime >= 0.0f)
		{
			// remove the gate
			gate->SetInactive();
		}

		// Update our sprites and traps
		for (int i = 0; i < numActors; i ++)
		{
			dx->updateActor(i, time);
		}
		for (int i = 0; i < numTraps; i ++)
		{
			dx->updateTrap(i, time);
		}

		// Dont let the players go past a starting area until timer is up to 0
		if(dx->gameTime < 0)
		{
			float changeX = player1->getX() - -1.3f;
			if(changeX > 0)
				player1->SetPosition(player1->getX()-0.008f,player1->getY());

			float changeX2 = player2->getX() - -1.3f;
			if(changeX2 > 0)
				player2->SetPosition(player2->getX()-0.008f,player2->getY());

		}

		// Updates our Checkpoint System, based on number of players
		if(numOfPlayers == 1)
		{
			
			float changeX = player1->getX() - finishPoint->x; // x dist between us and the finish point

			if(changeX >= 0) // if we are at or infront of it
			{
				winningTime = raceTimer->stop(); // stop our time and set winning player
				WinningPlayerNum = player1->playerNumber;
				gameState = GameOver;
			}
		}
		else if(numOfPlayers == 2)
		{

			float dxP1 = player1->getX() - finishPoint->x; // x dist between us and the finish point
			float dxP2 = player2->getX() - finishPoint->x; // x dist between us and the finish point

			if(dxP1 >= 0) // if we are at or infront of it
			{
				winningTime = raceTimer->stop(); // stop our time and set winning player
				WinningPlayerNum = player1->playerNumber;
				gameState = GameOver;
			}
			else if(dxP2 >= 0)
			{
				winningTime = raceTimer->stop(); // stop our time and set winning player
				WinningPlayerNum = player2->playerNumber;
				gameState = GameOver;
			}
		}

	}

	// Checks for KeyPresses
	CheckMenuKeyPresses();
	
}

void GameManager::CheckMenuKeyPresses()
{
	// check for keyboard presses
	bool pause = false;
	bool confirm = false;
	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;

	if(gameState == ControllerSelect)
	{
		// Lets the players choose which controllers they want to use in the
		// controller selection lobby, by pressing up
		for(int i = 0; i < CONTROLLERS; i++)
		{
			bool isUp = controllers[i]->getUpTap();
			
			if(isUp && currentMenuSelection == OnePlayer) // Only one person is playing
			{
				player1->SetController(controllers[i]);
				controllersSelected = true;
				MenuItemSelected();
				break;
			}
			else if(isUp && currentMenuSelection == TwoPlayer) // Two people are playing
			{
				if(controllersSelected == 0) // Player 1 turn
				{
					player1->SetController(controllers[i]);
					controllersSelected ++;
					dx->numControllersSelected = controllersSelected;
				}
				else if(controllersSelected == 1) // Player 2 turn
				{
					if(player1->GetController() != controllers[i]) // Make sure p1 & p2 dont have same controls
					{
						player2->SetController(controllers[i]);
						controllersSelected ++;
						dx->numControllersSelected = controllersSelected;
						MenuItemSelected();
						break;
					}
				}
			}
		}
	}

	for (int i = 0; i < CONTROLLERS; i++)
	{
		pause = (pause || controllers[i]->getPause());
		confirm = (confirm || controllers[i]->getMenuConfirm());
		up = (up || controllers[i]->getUpTap());
		down = (down || controllers[i]->getDownTap());
		right = (right || controllers[i]->getRightTap());
		left = (left || controllers[i]->getLeftTap());
	}
	if (pause) TogglePauseGame();
	if (confirm)
	{
		if (gameState == GameOver && letterCounter == 3)
		{
			RecordBestTimes();
			ReadBestTimes();
			BackToMenu();
		}
		else if(gameState == ControllerSelect)
		{
			if(controllersSelected == true)
				MenuItemSelected();
		}
		else
		{
			MenuItemSelected();
		}
	}
	if (up) ChangeMenuSelection(-1);
	if (down) ChangeMenuSelection(1);
	if (left && gameState == Lobby) ChangeMenuSelection(1); // For the Lobby screen when choosing 1 or 2 players
	if (right && gameState == Lobby) ChangeMenuSelection(-1);

	// For changing menu instruction pages
	if(left && gameState == IMenu)
	{
		int cPage = dx->currentInstructPage;
		cPage--;

		if(cPage < 1)
			cPage = 1;

		dx->currentInstructPage = cPage;
	}
	if(right && gameState == IMenu)
	{
		int cPage = dx->currentInstructPage;
		cPage++;

		if(cPage > numOfInstructPages)
			cPage = numOfInstructPages;

		dx->currentInstructPage = cPage;
	}

}

void GameManager::StartOverview()
{
	panGhost->MakeActive();
	gameState = Overview;
}

void GameManager::BeginGame()
{
		if(currentMenuSelection == OnePlayer)
			numOfPlayers = 1;
		else if(currentMenuSelection == TwoPlayer)
			numOfPlayers = 2;
		::mainCamInst->ClearTargets();
		::mainCamInst->AddTrackingTarget(player1);
		if(numOfPlayers == 1)
		{
			player2->SetVisible(false); // make p2 invisible
			 // make sure the camera only focuses on p1, so don't add P2
		}
		if (numOfPlayers == 2) // If we have 2 players, create player 2
		{
			//player2->SetController(controllers[1]);
			player2->SetVisible(true);
			::mainCamInst->AddTrackingTarget(player2);
		}

		::mainCamInst->CenterOn(player1); // make sure camera is in the right spot when the game starts

		numActors = numOfPlayers + 2; // p1, camera, p2, and LevelPanGhost are actors
		gameState = PlayingGame;
		raceTimer->delayedStart(5.0f); // start our timer when the game starts
}

// Triggered when Enter Key is pressed while at the menu
void GameManager::MenuItemSelected()
{
	// We have selected the PlayGame option on the menu screen
	if(gameState == StartMenu && currentMenuSelection == PlayGame)
	{
		gameState = Lobby;
		currentMenuSelection = OnePlayer;
		selector->SetPosition(-0.21f, -0.35f, 0.0f);
	}
	else if(gameState == Lobby) // The game lobby where we choose how many players we want
	{
		gameState = ControllerSelect;
		//StartOverview();
	}
	else if(gameState == ControllerSelect)
	{
		StartOverview();
	}
	else if (gameState == Overview)
	{
		// just pressed Enter during overview
		panGhost->Cancel();
	}
	else if(gameState == StartMenu && currentMenuSelection == BestTimes)
	{
		gameState = TMenu;
		currentMenuSelection = Menu;
		selector->SetPosition(-0.71f, -1.0f, 0.0f);
	}
	else if(gameState == StartMenu && currentMenuSelection == Instructions)
	{
		gameState = IMenu;
		currentMenuSelection = Menu;
		selector->SetPosition(-0.72f, -0.92f, 0.0f);
	}
	else if(currentMenuSelection == Menu)
	{
		gameState = StartMenu;
		currentMenuSelection = PlayGame;
		ChangeMenuSelection(3); // resets the selector
		numControllersSelected = 0;
		dx->numControllersSelected = controllersSelected;
		controllersSelected = false;
	}
}

// lets us toggle pause while in the game
void GameManager::TogglePauseGame()
{
	if(gameState == PlayingGame)
		gameState = Paused;
	else if(gameState == Paused)
		gameState = PlayingGame;

	raceTimer->pause();
}

// brings us back to the main menu after we hit esc while in the game
void GameManager::BackToMenu()
{
	if(gameState == PlayingGame || gameState == Overview || gameState == GameOver)
		soundManager.PauseCurrentSound();

	gameState = StartMenu;
	currentMenuSelection = PlayGame;
	ChangeMenuSelection(3); // Will reset the Selector to Inital Position
	controllersSelected = false;

	// resets p1 and p2 positions
	float player1x, player1y;
	loader->getP1Start(&player1x, &player1y);
	float player2x, player2y;
	loader->getP2Start(&player2x, &player2y);

	player1->SetPosition(player1x, player1y);
	player2->SetPosition(player2x, player2y);

	// Reset P1 and P2 Checkpoints
	checkpointManager->ResetCheckpoints();

	// Reset Our Timer
	raceTimer = new RaceTimer();

	// Reset Name Values
	WinningPlayerNum = 0;
	winnerName = "---";
	letterCounter = 0;
	numControllersSelected = 0;
	dx->numControllersSelected = controllersSelected;
}

void GameManager::EnterNameText(WPARAM wparam)
{
    char letter = (char)wparam; // grab character from the keyboard
	int lNum = (int)letter; // check its number

	// Make sure we are only grabbing char input from keyboard A-Z and a-z
	if(lNum > 64 && lNum < 91)
	{
		if(letterCounter < 3) // You can only enter up to 3 letters
		{
			winnerName[letterCounter] = letter;
			letterCounter++;
		}
	}
	else if(lNum == 8) //Backspace
	{
		letterCounter--;

		if(letterCounter < 0)
			letterCounter = 0;

		winnerName[letterCounter] = '-';
	}
}

void GameManager::RecordBestTimes(void)
{
	// create a text file 
	ofstream output;

	// open the file
	output.open("TextFiles//BestTimes.txt", ios::app);

	// check for an error
	if(output.fail()) // failed open
	{
		cout << "Unable to open file" << endl;
		system("Pause");
		return;
	}

	output << winnerName.c_str()<<endl; // the name of the winner

	char tBuffer [sizeof(int)*12+1];

	sprintf_s(tBuffer,"%3.1f", winningTime); // the time of the person
	output << tBuffer << endl;

	// close the file
	output.close();
}

void GameManager::ReadBestTimes(void)
{

	// read a text file
	ifstream input;

	// open for reading
	input.open("TextFiles//BestTimes.txt");

	// Make sure we can open the file
	if(input.fail())
	{
		dx->bestTimesNames = NULL;
		dx->bestTimesNum = NULL;
		return;
	}

	// how may lines the text file has
	int lineCounter = 0;
	int nameCounter = 0;
	int timeCounter = 0;

	// reads each line from the txt file
	char text[40];
	string* names = new string[100];
	float* times = new float[100];

	while(!input.eof())
	{
		input.getline(text,40); // grab the text and put it in the text array
		
		// at the persons name
		if(lineCounter %2 == 0)
		{
			names[nameCounter] = text;
			nameCounter++;
		}
		else // at the person's time score
		{
			times[timeCounter] = (float)atof(text);
			timeCounter++;
		}
		lineCounter++;
	}

	input.close();

	// sort the list of times from smallest to largest
	for(int i = 0; i < timeCounter; i++)
	{
		for(int j = 0; j < timeCounter; j++)
		{
			float timeA = times[i];
			float timeB = times[j];

			if(timeA < timeB)
			{
				// sawp times
				float temp = times[i];
				times[i] = times[j];
				times[j] = temp;

				// swap names
				string tempN = names[i];
				names[i] = names[j];
				names[j] = tempN;
			}
		}
	}

	// Clear out the old arrays since the new ones are bigger in size
	dx->bestTimesNames = new string[timeCounter];
	dx->bestTimesNum = new float[timeCounter];

	// Send all our info to the directxManager for Displaying
	dx->bestTimesNames = names;
	dx->bestTimesNum = times;
	dx->bestTimesCount = timeCounter;
}

// Set up the tips that displayed in the instruction menu
void GameManager::InitalizeInstructMenu()
{
	numOfInstructPages = 3;
	int numTips = numOfInstructPages * 3;
	instructTips = new char*[numTips];
	instructTips[0] = "NONE";

	// Page 1
	instructTips[1] = "You cannot move past";
	instructTips[2] = "the starting area until the";
	instructTips[3] = "the Timer is a positive number.";

	// Page 2
	instructTips[4] = "You should avoid colliding";
	instructTips[5] = "with traps, as they will send";
	instructTips[6] = "you back to the last checkpoint.";

	// Page 3
	//instructTips[2] = "Your goal is to reach the end of the level in the least amount of time possible!";
	instructTips[7] = "Your goal is to reach the";
	instructTips[8] = "end of the level in the least";
	instructTips[9] = "amount of time possible!";
	
	// Add the instruct pics to use in order
	SimpleSprite* p1;
	SimpleSprite* p2;
	SimpleSprite* p3;
	SimpleSprite* s1;

	dx->MakeInstructSprite(L"Images/InstructPic1.jpg",&p1, InstructMenu);
	p1->SetPosition(0.6f,-0.15f,0.0f);
	p1->SetScaling(1.7f,1.7f);

	dx->MakeInstructSprite(L"Images/InstructPic2.jpg",&p2, InstructMenu);
	p2->SetPosition(0.6f,-0.15f,0.0f);
	p2->SetScaling(1.7f,1.7f);

	dx->MakeInstructSprite(L"Images/InstructPic3.jpg",&p3, InstructMenu);
	p3->SetPosition(0.6f,-0.15f,0.0f);
	p3->SetScaling(1.7f,1.7f);

	// Theses are the left right arrows that appear on the Instruct Menu
	dx->MakeMenuSprite(L"Images/Menu/left_right.jpg", &s1, InstructMenu);
	s1->SetPosition(1.3f, -1.2f, 0.0f);
	s1->SetScaling(0.2f,0.2f);

	// Let dx know so it can draw the text
	dx->instructTips = new char*[numTips];
	dx->instructTips = instructTips;
	dx->currentInstructPage = 1;
	dx->numOfInstructPages = numOfInstructPages;
}

//destroys the sound manager
void GameManager::CleanUp()
{
	soundManager.~SoundManager();
}

//deconstructor
GameManager::~GameManager(void)
{
}
