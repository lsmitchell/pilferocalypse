#include "stdafx.h"
#include "dxManager.h"

#define DRAW_TIMES 1000
#define MAX_NUM_SPRITES 300
#define MAX_ACTORS 100

/*******************************************************************
* Constructor
*******************************************************************/
dxManager::dxManager() :	pD3DDevice(NULL),
							pSwapChain(NULL),
							pRenderTargetView(NULL),
							pVertexBuffer(0),
							pVertexLayout(0),
							pBasicEffect(0),
							pRS(0)
{
	//set up important variables and arrays
	fontPosition.left = 10;
	fontPosition.top = 10;
	fontPosition.right = 150;
	fontPosition.bottom = 60;
	sSprites = new SimpleSprite*[MAX_NUM_SPRITES];
	menuSprites = new SimpleSprite*[10]; // Initalize the array that stores our menu stuff
	instructPicSprites = new SimpleSprite*[10]; // Stores sprites for use inside Instructions
	initializeActors();
	numSprites = 0;
	numMenuSprites = 0;
	numInstructSprites = 0;
	numActors = 0;
	numTraps = 0;
	playerIndex = 0;
}
/*******************************************************************
* Destructor
*******************************************************************/
dxManager::~dxManager()
{
	if ( pRenderTargetView ) pRenderTargetView->Release();
	if ( pSwapChain ) pSwapChain->Release();
	if ( pD3DDevice ) pD3DDevice->Release();
	if ( pVertexBuffer ) pVertexBuffer->Release();
	if ( pVertexLayout ) pVertexLayout->Release();
	if ( pRS ) pRS->Release();
	if ( pBasicEffect ) pBasicEffect->Release();
}
/*******************************************************************
* Initializes Direct3D Device
*******************************************************************/
bool dxManager::initialize( HWND* hW, float adjX, float adjY )
{
	//window handle
	hWnd = hW;

	//set the sprite adjustment numbers for use wherever we need to resize for dimensions
	adjustX = adjX;
	adjustY = adjY;
	
	//get window dimensions
	RECT rc;
    GetClientRect( *hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

	//Set up DX swap chain
	//--------------------------------------------------------------
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	
	//set buffer dimensions and format
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;;
	
	//set refresh rate
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	
	//sampling settings
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;

	//output window handle
	swapChainDesc.OutputWindow = *hWnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//Create the D3D device
	//--------------------------------------------------------------
	if ( FAILED( D3D10CreateDeviceAndSwapChain(		NULL, 
													D3D10_DRIVER_TYPE_HARDWARE, 
													NULL, 
													0, 
													D3D10_SDK_VERSION, 
													&swapChainDesc, 
													&pSwapChain, 
													&pD3DDevice ) ) ) return fatalError(L"D3D device creation failed");

	//SHADER STAGE
	//*****************************************************************************
	
	//Load Basic Effect and Technique 
	//--------------------------------------------------------------
	if ( FAILED( D3DX10CreateEffectFromFile(	L"basicEffect.fx", 
												NULL, NULL, 
												"fx_4_0", 
												D3D10_SHADER_ENABLE_STRICTNESS, 
												0, 
												pD3DDevice, 
												NULL, 
												NULL, 
												&pBasicEffect, 
												NULL, 
												NULL	) ) ) return fatalError(L"Could not load effect file!");	

	pBasicTechnique = pBasicEffect->GetTechniqueByName("Render");
	
	//create matrix effect pointers
	pViewMatrixEffectVariable = pBasicEffect->GetVariableByName( "View" )->AsMatrix();
	pProjectionMatrixEffectVariable = pBasicEffect->GetVariableByName( "Projection" )->AsMatrix();
	pWorldMatrixEffectVariable = pBasicEffect->GetVariableByName( "World" )->AsMatrix();
	
	//INPUT ASSEMBLY STAGE
	//*****************************************************************************
	
	//Create Input Layout
	//--------------------------------------------------------------	
	
	D3D10_INPUT_ELEMENT_DESC layout[] = 
	{	
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = 2;

	D3D10_PASS_DESC PassDesc;
	pBasicTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	if ( FAILED( pD3DDevice->CreateInputLayout( layout, 
												numElements, 
												PassDesc.pIAInputSignature,
												PassDesc.IAInputSignatureSize, 
												&pVertexLayout ) ) ) return fatalError(L"Could not create Input Layout!");

	// Set the input layout
//	pD3DDevice->IASetInputLayout( pVertexLayout );

	//create vertex buffer (space for 100 vertices)
	//---------------------------------------------------------------------------------
	UINT numVertices = 100;

	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof( vertex ) * numVertices;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	if ( FAILED( pD3DDevice->CreateBuffer( &bd, NULL, &pVertexBuffer ) ) ) return fatalError(L"Could not create vertex buffer!");;

	// Set vertex buffer
	UINT stride = sizeof( vertex );
	UINT offset = 0;
	pD3DDevice->IASetVertexBuffers( 0, 1, &pVertexBuffer, &stride, &offset );	

	//RASTERIZER STAGE SETUP
	//*****************************************************************************
	
	//Create viewport
	//--------------------------------------------------------------
	
	//create viewport structure	
	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	pD3DDevice->RSSetViewports(1, &viewPort);

	// OUTPUT-MERGER STAGE
	//*****************************************************************************

	//Create render target view
	//--------------------------------------------------------------
	
	//try to get the back buffer
	ID3D10Texture2D* pBackBuffer;	
	if ( FAILED( pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*) &pBackBuffer) ) ) return fatalError(L"Could not get back buffer");

	//try to create render target view
	if ( FAILED( pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView) ) ) return fatalError(L"Could not create render target view");
	
	pBackBuffer->Release();
	pD3DDevice->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	// Set up the view and projection matrices
	//*****************************************************************************
	D3DXVECTOR3 camera[3] = {	D3DXVECTOR3(0.0f, 0.0f, -5.0f),
								D3DXVECTOR3(0.0f, 0.0f, -1.0f),
								D3DXVECTOR3(0.0f, 1.0f, 0.0f) };
	D3DXMatrixLookAtLH(&viewMatrix, &camera[0], &camera[1], &camera[2]);
		
    //D3DXMatrixPerspectiveFovLH(&projectionMatrix, (float)D3DX_PI * 0.5f, (float)width/(float)height, 0.1f, 100.0f);
	// this is the magic 
	D3DXMatrixOrthoLH(&orthoMatrix, 3.0f, 3.0f, 0, 1);

	
	

	// create font
	D3DX10_FONT_DESC fontDesc = {
    28, 0, 400, 1,
    false,
    DEFAULT_CHARSET,
    OUT_TT_PRECIS,
    CLIP_DEFAULT_PRECIS,
    DEFAULT_PITCH,
    L"Arial"
	};
	HRESULT hr = D3DX10CreateFontIndirect(pD3DDevice, &fontDesc, &d3dxFont);



	//everything completed successfully
	return true;
}

//lets the user toggle between fullscreen and windowed mode
void dxManager::toggleFullscreen()
{
	BOOL isFullscreen;
	pSwapChain->GetFullscreenState(&isFullscreen, NULL);
	pSwapChain->SetFullscreenState(!isFullscreen, NULL);
}

// Doing PostQuit While the screen was fullsize caused problems, so make it windowed THEN PostQuit
void dxManager::exitProgram(void)
{
	pSwapChain->SetFullscreenState(FALSE, NULL);
	PostQuitMessage(0);
}

//draws text given a string (LPCSTR), x position, and y position
void dxManager::drawText(LPCSTR stringToDraw, int xLoc, int yLoc)
{
	const D3DXCOLOR BLACK(0.9f, 0.1f, 0.1f, 1.0f);
	RECT drawRect = { xLoc,yLoc, 0, 0 };
	d3dxFont->DrawTextA(0, stringToDraw, -1, &drawRect, DT_NOCLIP, BLACK);
}

//creates a 2d vertex on the screen
vertex vertex2d(float x, float y)
{
	return vertex(D3DXVECTOR3(x, y, 0.0f), D3DXVECTOR4(1,0,0,1));

}

//sets the frame rate inside dxManager
void dxManager::setFrame(int fCnt){
	frameCount = fCnt;
}

/*******************************************************************
* Scene Renderer
*******************************************************************/
void dxManager::renderScene()
{	
	//clear scene
	//MessageBox(*hWnd, LPCWSTR(windowText), L"report", MB_ICONEXCLAMATION);
	pD3DDevice->ClearRenderTargetView( pRenderTargetView, D3DXCOLOR(0,0,0,0) );
	//MessageBox(*hWnd, LPCWSTR(windowText), L"report", MB_ICONEXCLAMATION);
	pD3DDevice->IASetInputLayout( pVertexLayout );
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	pD3DDevice->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	//create world matrix
	static float r;
	D3DXMATRIX w;
	D3DXMatrixIdentity(&w);

	//set effect matrices
	pWorldMatrixEffectVariable->SetMatrix(w);
	pViewMatrixEffectVariable->SetMatrix(viewMatrix);
	//pProjectionMatrixEffectVariable->SetMatrix(projectionMatrix);
	pProjectionMatrixEffectVariable->SetMatrix(orthoMatrix);

	// only draw our ingame sprites if we are playing the game or paused
	if(*gameState == PlayingGame || *gameState == Paused || *gameState == GameOver || *gameState == Overview) // We are not in the StartMenu
	{
		// draw all sprites
		for (int i = 0; i < numSprites; i ++)
		{
			sSprites[i]->Draw();
		}
	}
	else if(*gameState == StartMenu) // only draw our menu if that is our state
	{
		// Draw our Menu
		for(int j = 0; j < numMenuSprites; j++)
		{
			if(menuSprites[j]->GetSpriteType() == MainMenu ||
				menuSprites[j]->GetSpriteType() == Selector) // Only draw the menu sprites relating to the main menu
				menuSprites[j]->Draw();
		}
	}
	else if(*gameState == TMenu) // only draw our BestTimes menu if that is our state
	{
		// Draw our Menu
		for(int j = 0; j < numMenuSprites; j++)
		{
			if(menuSprites[j]->GetSpriteType() == TimeMenu ||
				menuSprites[j]->GetSpriteType() == Selector) // Only draw the menu sprites relating to the time menu
				menuSprites[j]->Draw();
		}

		drawBestTimesText(); // draws the text listing best times
	}
	else if(*gameState == IMenu) // only draw our Instructions menu if that is our state
	{
		// Draw our Menu
		for(int j = 0; j < numMenuSprites; j++)
		{
			if(menuSprites[j]->GetSpriteType() == InstructMenu ||
				menuSprites[j]->GetSpriteType() == Selector) // Only draw the menu sprites relating to the instructions menu
				menuSprites[j]->Draw();
		}

		// Draw Tip Text
		// Each Page has 3 tips, Page1 = Tips 1-3, Page 2 = Tips 4-6, Page 3 = Tips 7-9
		int yPos = 150;
		for(int i = currentInstructPage*3-2; i <= currentInstructPage*3; i++)
		{
			drawText(instructTips[i], 60, yPos);
			yPos += 30;
		}

		// Draw Tip Picture
		instructPicSprites[currentInstructPage-1]->Draw();

		// Show us which page we are on for instructions
		char tBuffer [sizeof(int)*12+1];
		_itoa_s(currentInstructPage,tBuffer,10);

		drawText("Page: ", 450, 510);
		drawText(tBuffer, 520, 510);
		drawText("/3", 535, 510);
		
	}
	else if(*gameState == Lobby)
	{
		// Draw our Menu
		for(int j = 0; j < numMenuSprites; j++)
		{
			if(menuSprites[j]->GetSpriteType() == LobbyMenu ||
				menuSprites[j]->GetSpriteType() == Selector) // Only draw the menu sprites relating to the lobby menu
				menuSprites[j]->Draw();
		}
	}
	else if(*gameState == ControllerSelect)
	{
		for(int j = 0; j < numMenuSprites; j++)
		{
			if(menuSprites[j]->GetSpriteType() == LobbyMenu) // Only draw the menu sprites relating to the lobby menu
				menuSprites[j]->Draw();
		}
		
		if(numControllersSelected == 0)
			drawText("Player1 Press Up On Your Controller", 320, 400);
		else if(numControllersSelected == 1)
			drawText("Player2 Press Up On Your Controller", 320, 400);
	}

	//set up text strings
	LPCSTR preTextOne = "Time: ";
	LPCSTR textOne = "";
	//LPCSTR preTextFive = "FRate: ";
	//LPCSTR textFive = "";

	// character buffers for text
	//char fBuffer [sizeof(int)*12+1];
	char tBuffer [sizeof(int)*12+1];

	// draws frame rate to the screen
	//_itoa_s((int)frameCount,fBuffer,10);
	//textFive = fBuffer;

	// draws game time to the screen
	sprintf_s(tBuffer,"%5.1f", gameTime);
	textOne = tBuffer;

	if(*gameState == PlayingGame) // Only Print out all this info In-Game
	{
		//draw text to screen here
		drawText(preTextOne,15,15);
		drawText(textOne,90,15);

		//drawText(preTextFive,15,40);
		//drawText(textFive,110,40);
		
		// Notifications to the player about the race starting
		if(gameTime < -2)
			drawText("On Your Mark!", 550, 100);
		else if (gameTime < 0)
			drawText("Get Set!", 550, 100);
		else if (gameTime < 1)
			drawText("PILFER!", 550, 100);
	}
	else if(*gameState == Paused)
	{
		drawText("PAUSED",15,15);
	}
	else if(*gameState == GameOver) // Game is over, state who won
	{
		LPCSTR preTextWin = "Winner is Player: ";
		LPCSTR textWin = "";

		char wBuffer [sizeof(int)*12+1];

		_itoa_s(*winningPlayer,wBuffer,10);
		textWin = wBuffer;

		// The time
		drawText(preTextOne,15,15);
		drawText(textOne,90,15);

		// The winning player
		drawText(preTextWin,450,200);
		drawText(textWin,650,200);

		// Winning player can enter their name
		LPCSTR preName = "Enter your Name: ";
		LPCSTR textName = winnerName->c_str();

		drawText(preName, 450, 250);
		drawText(textName, 650, 250);
	}
	else // There is a bug, unless we do this the selector becomes a red block
	{
		drawText("",15,15);
	}

	//flip buffers
	pSwapChain->Present(0,0);
}


//Resets and releases everything in DirectX for cleanup
void dxManager::cleanup()
{
	pSwapChain->SetFullscreenState(FALSE, 0);
	pSwapChain->Release();
	pRenderTargetView->Release();
	pD3DDevice->Release();
}

//returns the number of sprites/actors that were loaded
int dxManager::NumSprites()
{
	return numSprites;
}

//sends back the number of actors created
int dxManager::NumActors()
{
	return numActors;
}

//sends back the number of traps created
int dxManager::NumTraps()
{
	return numTraps;
}

int dxManager::NumMenuSprites()
{
	return numMenuSprites;
}

//returns which sprite is controllable 
int dxManager::PlayerIndex()
{
	return playerIndex;
}

//returns a sprite at the given index; really should use getActor instead
SimpleSprite* dxManager::getSprite(int index)
{
	if (index > numSprites)
	{
		return 0;
	}
	else
	{
		return sSprites[index];
	}
}

SimpleSprite* dxManager::getMenuSprite(int index)
{
	if(index > numMenuSprites)
	{
		return 0;
	}
	else
	{
		return menuSprites[index];
	}
}

//sets up the actor array
void dxManager::initializeActors()
{
	actors = new IActor*[MAX_ACTORS];
}

//adds an actor at the specified index
void dxManager::addActor(IActor* actor)
{
	actors[numActors] = actor;
	numActors ++;
}

//adds an actor at the specified index
void dxManager::AddTrap(Trap* trap)
{
	traps.resize(numTraps+1);
	traps[numTraps] = trap;
	numTraps ++;
}

//calls update for an actor at the given index. Also pass in the movement speed.
void dxManager::updateActor(int index, float timePassed)
{
	actors[index]->Update(timePassed);
}

//calls update for a trap at the given index. Also pass in the movement speed.
void dxManager::updateTrap(int index, float timePassed)
{
	traps[index]->Update(timePassed);
}

//returns an actor at a specified index.
IActor* dxManager::getActor(int index)
{
	if (index > numSprites)
	{
		return 0;
	}
	else
	{
		return actors[index];
	}
}

//Acquires a list of walls
list<ICollidable*>* dxManager::GetWalls()
{
	return &walls;
}

//Acquires a list of walls
vector<Trap*>* dxManager::GetTraps()
{
	return &traps;
}

//Adds a new wall to the list of walls
void dxManager::AddWall(ICollidable* wall)
{
	walls.push_back(wall);
}

//gets rid of everything inside all of the actors used
void dxManager::clearActors()
{
	for (int i = 0; i < numActors; i ++)
	{
		if (actors[i] != 0)
		delete actors[i];
	}
	numActors = 0;
	walls.clear();
}

//gets rid of all the sprites loaded in
void dxManager::clearSprites()
{
	for (int i = 0; i < numSprites; i ++)
	{
		if (sSprites[i] != 0)
		delete sSprites[i];
	}
	numSprites = 0;
}

// Pass in the name of the txt file as a param, like, myTextFile.txt
void dxManager::loadPicsFromTextFile(char* fileName)
{
	// read a text file
	ifstream input;

	// open for reading
	input.open(fileName);

	// Make sure we can open the file
	if(input.fail())
	{
		cout << "Unable to open file for reading" << endl;
		system("Pause");
		return;
	}

	// reads each line from the txt file, which is an img name
	char text[40];
	textures = new TextureManager(pD3DDevice, &orthoMatrix);
	while(!input.eof())
	{
		input.getline(text,40);
		HRESULT hr = textures->LoadTexture(charToWchar(text));
		if (FAILED(hr))
		{
			cout << "Unable to create texture from file " << text;
			return;
		}
	}

	input.close();
}

//Loads in sprites
HRESULT dxManager::MakeSprite(WCHAR* textureName, SimpleSprite** sprite)
{
	//if sprite is less than the max number of sprites allowed
	if (numSprites < MAX_NUM_SPRITES)
	{
		//add a new sprite to the array
		sSprites[numSprites] = new SimpleSprite(pD3DDevice, textureName);
		//fix scaling for screen dimensions here instead of later
		sSprites[numSprites]->AdjustScaling(adjustX, adjustY);
		*sprite = sSprites[numSprites];
		numSprites ++;
		return sSprites[numSprites - 1]->creationError;
	}
	//otherwise we have a problem
	else
	{
		return E_FAIL;
	}
}

//In case a name is only passed, just give it a location in memory.
HRESULT dxManager::MakeSprite(WCHAR* textureName)
{
	SimpleSprite* sprite;
	return MakeSprite(textureName, &sprite);
}

// Lets us make sprites to use for the menu
HRESULT dxManager::MakeMenuSprite(WCHAR* textureName, SimpleSprite** sprite,SpriteType type)
{
	//add a new sprite to the array
	menuSprites[numMenuSprites] = new SimpleSprite(pD3DDevice, textureName);
	menuSprites[numMenuSprites]->SetSpriteType(type);

	*sprite = menuSprites[numMenuSprites];
	numMenuSprites ++;

	return menuSprites[numMenuSprites - 1]->creationError;
}

HRESULT dxManager::MakeInstructSprite(WCHAR* textureName, SimpleSprite** sprite, SpriteType type)
{
	instructPicSprites[numInstructSprites] = new SimpleSprite(pD3DDevice, textureName);
	instructPicSprites[numInstructSprites]->SetSpriteType(type);

	*sprite = instructPicSprites[numInstructSprites];
	numInstructSprites ++;

	return instructPicSprites[numInstructSprites - 1]->creationError;
}

//acquires the texture by name
Texture* dxManager::getTexture(WCHAR* name)
{
	return textures->get(name);
}

void dxManager::KillPlayer(PlayerActor* player, int playerNumber)
{
	cpm->SendToLastChckPt(player, playerNumber);
}

//gets an instance of the Direct 3D Device
ID3D10Device* dxManager::getPD3DDevice()
{
	return pD3DDevice;
}

void dxManager::drawBestTimesText(void)
{
	int xPos = 420;
	int yPos = 170;

	// The Top Header
	drawText("Place",xPos, yPos);
	drawText("Names",xPos+100, yPos);
	drawText("Times",xPos+200, yPos);
	yPos += 30;

	for(int i = 0; i < bestTimesCount; i++)
	{
		// We only want to print out the top 10 scores
		if(i >= 10)
			break;
		
		char wBuffer [sizeof(int)*12+1];
		_itoa_s(i+1,wBuffer,10);

		// The place, 1st, 2d,etc... the person is in
		drawText(wBuffer,xPos, yPos);

		if(bestTimesNames != NULL && bestTimesNum != NULL)
		{
			// The name of the person
			drawText(bestTimesNames[i].c_str(),xPos+100, yPos);
			
			// The time of that person
			char tBuffer [sizeof(int)*12+1];
			sprintf_s(tBuffer,"%5.1f", bestTimesNum[i]);
			
			char* t = " sec";
			strcat_s(tBuffer,t);

			drawText(tBuffer,xPos+200, yPos);

			yPos += 30; // vertical spacing
		}
		
	}
}

/*******************************************************************
* Fatal Error Handler
*******************************************************************/
//uh-oh!
bool dxManager::fatalError(LPCWSTR msg)
{
	MessageBox(*hWnd, msg, L"Fatal Error!", MB_ICONERROR);
	return false;
}