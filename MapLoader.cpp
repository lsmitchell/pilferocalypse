#include "StdAfx.h"
#include "MapLoader.h"
#include "Globals.h"

MapLoader::MapLoader(dxManager* d, char* mapFileName,char* keyFileName, int rows, int cols, float blockSize)
{
	dx = d; // get the refrence to the dxManager
	mapLength = cols; // how far across the map is
	mapHeight = rows; // how tall the map is
	gridBlockSize = blockSize; // size of each block in our gird based map

	// Make our array size dynamically
	mapArray = new char*[rows];

	for( int i = 0 ; i < rows ; i++ )
		mapArray[i] = new char [cols];

	keyList.clear(); // make sure our map is empty
	
	finishPoint = new Checkpoint();
	checkpointCounter = 0; // will be incremented every time we add a checkpoint
	checkpointArray = new Checkpoint*[NUM_OF_CHECKPOINTS]; // Initalize our array of checkpoints

	int HR = readMapFile(mapFileName); // process the map text file

	int HR2 = readKeyFile(keyFileName); // process the key to the map file

	if(HR != 0 || HR2 != 0)
		cout << "Problem Reading Text File" << endl;

	createMap(-2.0f,1.1f);
}

// reads in the MapRepresentation file and stores all the chars in an array
int MapLoader::readMapFile(char* fileName)
{
	// read a text file
	ifstream input;

	// open for reading
	input.open(fileName);

	// Make sure we can open the file
	if(input.fail())
		return -1;

	// read in a chracter at a time
	char charData;
	int rowCounter = 0;
	int colCounter = 0;

	// loop through the text file
	while(!input.eof())
	{
		charData = input.get(); // grab each character individually
		
		// if we have reached the end of the text file, exit the loop
		if((int)charData == -1)
			break;

		// check if we reached the end of a line in the text file, dont want to store that
		if((int)charData != 10) 
		{
			mapArray[rowCounter][colCounter] = charData;
			colCounter++;
		}
		else
		{
			rowCounter++;
			colCounter = 0;
		}

	}

	input.close();

	return 0;
}

// reads in the MapKey file that determines which char values in the map represent which sprite
int MapLoader::readKeyFile(char* fileName)
{
	// read a text file
	ifstream input;

	// open for reading
	input.open(fileName);

	// Make sure we can open the file
	if(input.fail())
		return -1;

	// reads each line from the txt file
	char text[40];

	while(!input.eof())
	{
		input.getline(text,40);
		char key = text[0]; // the representation in the MapFile
		
		if(key == 'E')// E is put at the end of the text file to signify we are done
			break;

		char* value = new char[(int)input.gcount()-1];
		
		// copy the char values over from text to value, ignoring the beginning key part
		for(int i = 2; i < (int)input.gcount(); i++)
		{
			value[i-2] = text[i];
						
			if(text[i] == '\0')//reached the end of the line
				break;
		}

		keyList.insert(map<char,char*>::value_type(key,value)); // store the key and value in the keyList
	}

	input.close();

	return 0;
}

// space out our sprites on the map depending on where they were in the text file
// parameters define where the top left hand corner or the map is
void MapLoader::createMap(float topLeftX, float topLeftY)
{
	float xPos = topLeftX;
	float yPos = topLeftY;
	bool foundP2 = false;
	map<char,char*>::iterator ptr = keyList.begin();
	int counter = 0;

	for(int i = 0; i < mapHeight; i++) // rows
	{
		for(int j = 0; j < mapLength; j++) // cols
		{
			char key = mapArray[i][j]; // grab the char from each row,col

			ptr = keyList.find(key);
			
			if(ptr != keyList.end()) // make sure there is a match
			{
				char* imgFile = (ptr->second); // grab the name of the image file the key uses

				if (strcmp(imgFile, "Images/platformSingle.png") == 0 || strcmp(imgFile, "Images/platformVert.png") == 0 || strcmp(imgFile, "Images/platformLeft.png") == 0 || strcmp(imgFile, "Images/platformMiddle.png") == 0 || strcmp(imgFile, "Images/platformRight.png") == 0)
				{
					Wall* newWall = new Wall(dx, charToTCHAR(imgFile));
					newWall->SetPosition(xPos, yPos);
					dx->AddWall(newWall);
				}
				else if (strcmp(imgFile, "floorT") == 0)
				{
					Wall* newWall2 = new Wall(dx, charToTCHAR("Images/platformV.png"));
					newWall2->SetPosition(xPos+gridBlockSize/2-0.025f, yPos-0.025f);
					dx->AddWall(newWall2);

					Wall* newWall = new Wall(dx, charToTCHAR("Images/platformMiddle.png"));
					newWall->SetPosition(xPos, yPos);
					dx->AddWall(newWall);
				}		
				else if (strcmp(imgFile, "crossPlatform") == 0)
				{
					Wall* newWall2 = new Wall(dx, charToTCHAR("Images/crossPlatH.png"));
					newWall2->SetPosition(xPos, yPos);
					dx->AddWall(newWall2);

					Wall* newWall = new Wall(dx, charToTCHAR("Images/crossPlatV.png"));
					newWall->SetPosition(xPos, yPos);
					dx->AddWall(newWall);
					
				}
				else if (strcmp(imgFile, "Images/flame1.png") == 0)
				{
					Trap* newTrap = new Trap(dx,imgFile, xPos, yPos);
					newTrap->SetAnimStepTime(3000); //3 second flames
					dx->AddTrap(newTrap);
				}
				else if (strcmp(imgFile, "Images/saw1.png") == 0)
				{
					Trap* newTrap = new Trap(dx,imgFile, xPos, yPos-gridBlockSize);
					newTrap->setTileSize(gridBlockSize);
					dx->AddTrap(newTrap);
				}
				else if (strcmp(imgFile, "Images/Turret.png") == 0)
				{
					Trap* newTrap = new Trap(dx,imgFile, xPos, yPos-gridBlockSize);
					dx->AddTrap(newTrap);

					Trap* newTrap2 = new Trap(dx,"crosshairs", xPos+(gridBlockSize*1), yPos-(gridBlockSize*1)); //1 down, 1 over
					newTrap->SetTriggerTarget(newTrap2); //set this turrent to these crosshairs
					dx->AddTrap(newTrap2);

				}
				else if (strcmp(imgFile, "Images/spikes.png") == 0)
				{
					Trap* newTrap = new Trap(dx,imgFile, xPos, yPos-gridBlockSize+0.16f);
					dx->AddTrap(newTrap);
				}
				else if (strcmp(imgFile, "Images/spikesHang.png") == 0)
				{
					Trap* newTrap = new Trap(dx,imgFile, xPos, yPos+gridBlockSize-0.14f);
					dx->AddTrap(newTrap);
				}
				else if (strcmp(imgFile, "Images/checkpoint.png") == 0) // lets us add our checkpoints
				{
					SimpleSprite* currentSprite; // Add the img of our checkpoint
					dx->MakeSprite(charToWchar(imgFile), &currentSprite);

					currentSprite->SetPosition(xPos, yPos, 0.0f); // Set the sprites position and scale
					currentSprite->SetScaling(0.4f,0.5f);

					// store the checkpoints location in an array 
					checkpointArray[checkpointCounter] = new Checkpoint(xPos, yPos);
					checkpointCounter++;
				}
				else if(strcmp(imgFile, "playerstart") == 0) // found in the map where player 1 is starting
				{
					p1StartX = xPos;
					p1StartY = yPos;
				}
				else if(strcmp(imgFile, "player2start") == 0)  // found in the map where player 2 is starting
				{
					p2StartX = xPos;
					p2StartY = yPos;
					foundP2 = true;
				}
				else if(strcmp(imgFile, "FinishPoint") == 0)  // found the finish point for the race
				{
					finishPoint->x = xPos;
					finishPoint->y = yPos;
					SimpleSprite* currentSprite;
					dx->MakeSprite(charToWchar("Images/JewelGear.png"), &currentSprite);
					currentSprite->SetPosition(xPos, yPos, 0.0f);
				}
				else if(strcmp(imgFile, "empty") != 0) // we wont be loading a sprite if there is an empty space
				{
					SimpleSprite* currentSprite;
					dx->MakeSprite(charToWchar(imgFile), &currentSprite);
					currentSprite->SetPosition(xPos, yPos, 0.0f);
				}

				xPos += gridBlockSize;
				counter++;
			}
		}
		xPos = topLeftX;
		yPos -= gridBlockSize;
	}
	if (!foundP2)
	{
		p2StartX = p1StartX;
		p2StartY = p1StartY;
	}
}

// lets us convert from a char to a tchar
TCHAR* MapLoader::charToTCHAR(char* myString)
{
	char* pchString = myString;

	// Find out required string size
	int iRequiredSize = ::MultiByteToWideChar(CP_ACP, NULL, pchString, -1, NULL, 0);

	// Alloc buffer for converted string
	TCHAR* pwchString = new TCHAR[iRequiredSize];

	// Do the string conversion
	MultiByteToWideChar(CP_ACP, NULL, pchString , -1, pwchString, iRequiredSize);

	return pwchString;
}


// returns the length(how far horz) of the map, in pixels based on the gird block size
float MapLoader::getMapLengthP(void)
{
	return mapLength*gridBlockSize;
}

// returns the height(how far vert) of the map, in pixels based on the gird block size
float MapLoader::getMapHeightP(void)
{
	return mapHeight*gridBlockSize;
}

//gets the start position of player 1
void MapLoader::getP1Start(float* x, float* y)
{
	*x = p1StartX;
	*y = p1StartY;
}

//gets the start position of player 2
void MapLoader::getP2Start(float* x, float* y)
{
	*x = p2StartX;
	*y = p2StartY;
}

Checkpoint* MapLoader::getFinishPoint()
{
	return finishPoint;
}

int MapLoader::getNumOfCheckpoints(void)
{
	return checkpointCounter;
}

//deconstructor
MapLoader::~MapLoader(void)
{
}
