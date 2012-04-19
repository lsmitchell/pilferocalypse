// Entry point for the application.
//

#include "stdafx.h"
#include "DXAssignment3.h"
#include "dxManager.h"
#include "GameManager.h"
#include <time.h>

#define MAX_LOADSTRING 100

#define DX_FULLSCREEN_WINDOWSTYLE (WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX)

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING] = L"Pilferocalypse";// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

dxManager dx; // Manages all of our DirectX stuff

HWND hWnd;

bool exitProgram = false; // boolean that lets the program know to kill everything

int frameCount = 0; //integer to keep track of frame rate

int scrollX = 0;

GameManager* gameManager;
Camera* mainCamInst;

//DirectX-related variables
/*
UINT width;
UINT height;
DXGI_SWAP_CHAIN_DESC swapChainDesc;
ID3D10Buffer* vertexBuffer;
IDXGISwapChain* swapChain;
ID3D10Device* dxdevice;
D3D10_VIEWPORT viewport;
ID3D10Texture2D* backBuffer;
ID3D10RenderTargetView* renderTargetView;*/


// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				moveAll(float, float);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DXASSIGNMENT3, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	//change the scale to make the sprites square
	float adjustX = 1.0f;
	float adjustY = 1.0f;

	//get window dimensions
	RECT rc;
    GetClientRect( hWnd, &rc );
    UINT windowWidth = rc.right - rc.left;
    UINT windowHeight = rc.bottom - rc.top;

	if(windowWidth > windowHeight)
	{
		adjustX = ((float)windowHeight)/windowWidth;
	}
	else
	{
		adjustY = ((float)windowWidth)/windowHeight;
	}

	// initialize DirectX here
	dx.initialize(&hWnd, adjustX, adjustY);
	mainCamInst = new Camera(&dx);

	gameManager = new GameManager(&dx); // create our game manager

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DXASSIGNMENT3));

	// Main message loop:
	SetTimer(hWnd, 1, 1000, NULL);

	DWORD currentTime, prevTime;
	float diffTime;
	prevTime = GetCurrentTime();
	float secsPerClock = 1.0f / (float)CLOCKS_PER_SEC;

	while(!exitProgram)
	{
		if (PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		currentTime = GetCurrentTime();
		diffTime = ((float)(currentTime - prevTime))*secsPerClock;

		//debug hack: We'd like to not do several seconds at once, or collision
		//and various other things will suck. So, we max out diffTime at a certain
		//value.
		if (diffTime > 0.2f) diffTime = 0.2f;

		//idle code here
		gameManager->GameLoop(diffTime);
		
		prevTime = currentTime;

		dx.renderScene();

		//we want to only increment this once per draw cycle
		frameCount += 1;
	}

	KillTimer(hWnd, 1);

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXASSIGNMENT3));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DXASSIGNMENT3);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}
//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   

   hInst = hInstance; // Store instance handle in our global variable

   //hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   hWnd = CreateWindow(szWindowClass, szTitle, DX_FULLSCREEN_WINDOWSTYLE,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//toggles fullscreen mode
void SwitchFullscreen()
{
	dx.toggleFullscreen();
	/*
	bool isFullscreen = dx->getFullscreen();
	delete(dx);
	dx = new dxManager();
	dx->setFullscreen(!isFullscreen);
	dx->initialize(&hWnd);*/
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_TIMER:
		switch(wParam){
		case 1:
			dx.setFrame(frameCount);
			frameCount = 0;
		}
		break;
	case WM_KEYDOWN:
		//might check for certain program-related inputs here like fullscreen, quit. Otherwise, pass to keyboard controller stored in game manager.
		
		// used for when we press escape while at the start menu, the program will close
		if(wParam == VK_ESCAPE && gameManager->GetGameState() == StartMenu)
		{
			PostQuitMessage(0);
			exitProgram = true; // if we postquit we want to exit the program
			gameManager->CleanUp();
		}
		else
			gameManager->KeyDown(wParam);

		break;
	case WM_KEYUP:
		gameManager->KeyUp(wParam);
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			dx.cleanup();
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		exitProgram = true; // if we postquit we want to exit the program
		gameManager->CleanUp();
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//get the texture by name
Texture* getTexture(WCHAR* name)
{
	return dx.getTexture(name);
}

// lets us convert a char array to a wchar
WCHAR* charToWchar(char* myString)
{
	char* pchString = myString;

	// Find out required string size
	int iRequiredSize = ::MultiByteToWideChar(CP_ACP, NULL, pchString, -1, NULL, 0);

	// Allocate buffer for converted string
	WCHAR* pwchString = new WCHAR[iRequiredSize];

	// Do the string conversion
	MultiByteToWideChar(CP_ACP, NULL, pchString , -1, pwchString, iRequiredSize);

	return pwchString;
}