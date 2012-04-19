/************************************************************************************
 *	DirectX 10 Manager Class
 *	----------------------------------------
 *	code by : bobby anguelov - banguelov@cs.up.ac.za
 *	downloaded from : takinginitiative.wordpress.org
 *
 *	code is free for use in whatever way you want, however if you work for a game
 *	development firm you are obliged to offer me a job :P (haha i wish)
 ************************************************************************************/

#ifndef DXMANAGER
#define DXMANAGER

#include <iostream>
#include <fstream>
#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>
#include <map>
#include "Globals.h"
#include "Camera.h"
#include "Trap.h"
#include <vector>
#include "CheckPointManager.h"

using namespace std;

class Wall;


//create a basic vertex container
struct vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR4 color;

	vertex( D3DXVECTOR3 p, D3DXVECTOR4 c )
	{
		pos = p;
		color = c;
	}
};

// Lets us determine the current state of the game
enum GameState
{
	StartMenu, TMenu, IMenu, Overview, PlayingGame, Lobby, ControllerSelect, Paused, GameOver, Exit
};

// Lets us determine which menu Item is selected
enum MenuSelector
{
	PlayGame, BestTimes, Instructions, Menu, OnePlayer, TwoPlayer
};

class dxManager
{
	/*******************************************************************
	* Members
	********************************************************************/	
private:

	//window handle
	HWND*						hWnd;
	
	//device vars
	ID3D10Device*				pD3DDevice;
	IDXGISwapChain*				pSwapChain;
	ID3D10RenderTargetView*		pRenderTargetView;
	D3D10_VIEWPORT				viewPort;
	ID3D10RasterizerState*		pRS;
	

	//input layout and vertex buffer
	ID3D10Buffer*				pVertexBuffer;
	ID3D10InputLayout*			pVertexLayout;

	//effects and techniques
	ID3D10Effect*				pBasicEffect;
	ID3D10EffectTechnique*		pBasicTechnique;

	//effect variable pointers
	ID3D10EffectMatrixVariable* pViewMatrixEffectVariable;
	ID3D10EffectMatrixVariable* pProjectionMatrixEffectVariable;
	ID3D10EffectMatrixVariable* pWorldMatrixEffectVariable;
	
	//projection and view matrices
	D3DXMATRIX                  viewMatrix;
	//D3DXMATRIX                  projectionMatrix;
	D3DXMATRIX                  orthoMatrix;

	//font variable
	ID3DX10Font					*d3dxFont;
	RECT						fontPosition;
	//WCHAR*						windowText;

	LPCSTR						preTextSix;
	LPCSTR						textSix;

	// sprite information
	//ID3D10ShaderResourceView**	textures;
	//ID3DX10Sprite**				sprites;
	SimpleSprite**				sSprites; //array of simple sprites
	int							numSprites; //number of sprites loaded in
	int							numActors; //number of actors created
	int							numTraps; //number of traps created
	int                         numMenuSprites; // number of menu items created
	int                         numInstructSprites; // number of Instruction menu pics
	int							playerIndex; //where in the sprite list the player is
	int							frameCount; // frame rate of the program

	IActor**				actors; //array of actors
	list<ICollidable*>		walls; //array of walls
	vector<Trap*>			traps; //array of traps
	SimpleSprite**          menuSprites; // array of sprites in the menu
	SimpleSprite**          instructPicSprites; // array of sprites the instructions menu uses

	/*******************************************************************
	* Methods
	********************************************************************/	
	void drawBestTimesText(void);
public:

	//constructor and destructor
	dxManager();
	~dxManager();

	//initialize directx device
	bool initialize(HWND*, float, float);	

	void toggleFullscreen();

	// lets us exit the program while in fullscreen
	void exitProgram();

	void cleanup();

	int NumSprites();

	int NumActors();

	int NumTraps();

	int NumMenuSprites();

	int PlayerIndex();

	SimpleSprite* getSprite(int index);

	SimpleSprite* getMenuSprite(int index);

	HRESULT MakeSprite(WCHAR* textureName, SimpleSprite** sprite);
	HRESULT MakeSprite(WCHAR* textureName);

	HRESULT MakeMenuSprite(WCHAR* textureName, SimpleSprite** sprite, SpriteType type);

	HRESULT MakeInstructSprite(WCHAR* textureName, SimpleSprite** sprite, SpriteType type);

	// uses a txt file which is passed in as a param and loads the pics listed
	void loadPicsFromTextFile(char* fileName);

	void setFrame(int fCnt);

	//renderScene
	void renderScene();	

	//actor code which was moved into dxManager instead of the main class
	void initializeActors();
	void addActor(IActor* actor);
	void updateActor(int index, float timePassed);
	void updateTrap(int index, float timePassed);
	IActor* getActor(int index);
	list<ICollidable*>* GetWalls();
	vector<Trap*>* GetTraps();
	void AddWall(ICollidable* wall);
	void AddTrap(Trap* wall);
	void clearActors();
	void clearSprites();

	Texture* getTexture(WCHAR* name);

	ID3D10Device* getPD3DDevice();

	GameState* gameState;
	int* winningPlayer;
	int* numOfPlayers;
	string* winnerName;

	// info used in displaying best times
	string* bestTimesNames;
	float* bestTimesNum;
	int bestTimesCount;


	float gameTime; // how long the game has been running
	char** instructTips;
	int currentInstructPage;
	int numOfInstructPages;
	int numControllersSelected;

	float adjustX;
	float adjustY;

	CheckPointManager*		cpm; //deal with the checkpoint system using this
	void KillPlayer(PlayerActor* p, int pnumber);

private:

	// this is also stored in the main class, but DX also needs a reference
	TextureManager* textures;

	void drawText(LPCSTR, int, int);

	//fatal error handler
	bool fatalError(LPCWSTR msg); 
};

#endif