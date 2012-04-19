#include "StdAfx.h"
#include "WASDKeyboardInput.h"
//#include "dxManager.h"

// create a new WASDKeyboardInput object.
WASDKeyboardInput::WASDKeyboardInput(void)
{
	xMove = 0.0f;
	up = false;
	down = false;
	upLast = false;
	downLast = false;
	leftLast = false;
	rightLast = false;
	pause = false;
	pauseLast = false;
	menuConfirm = false;
	menuConfirmLast = false;
}


WASDKeyboardInput::~WASDKeyboardInput(void)
{
}

//KeyDown is called when the Win32 message loop detects
//a key-press WPARAM. This function reads and stores
//the information.
void WASDKeyboardInput::KeyDown(WPARAM param)
{
	switch (param)
		{
		case 'p':
		case 'P':
			pause = true; // pause when escape key is hit
			break;
		case VK_RETURN:
			menuConfirm = true;
			break;
		case 'F':
			break;
		case 'D':
		case 'd':
			xMove = 1.0f;
			break;
		case 'A':
		case 'a':
			xMove = -1.0f;
			break;
		case 'W':
		case 'w':
			up = true;
			break;
		case 'S':
		case 's':
			down = true;
		}
}

//KeyUp is called when the Win32 message loop detects
//a key-press WPARAM. This function reads and stores
//the information.
void WASDKeyboardInput::KeyUp(WPARAM param)
{
	switch (param)
		{
		case 'p':
		case 'P':
			//dx.exitProgram();
			pause = false; // pause when escape key is hit
			pauseLast = false;
			break;
		case VK_RETURN:
			menuConfirm = false;
			menuConfirmLast = false;
		case 'D':
		case 'd':
			if (xMove == 1.0f) xMove = 0.0f;
			rightLast = false;
			break;
		case 'A':
		case 'a':
			if (xMove == -1.0f) xMove = 0.0f;
			leftLast = false;
			break;
		case 'W':
		case 'w':
			up = false;
			upLast = false;
			break;
		case 'S':
		case 's':
			down = false;
			downLast = false;
			break;
		}
}

//returns the float representing which direction left-to-right the
//player is pressing.
float WASDKeyboardInput::getXMove()
{
	return xMove;
}

bool WASDKeyboardInput::getRight()
{
	return (xMove > 0.0f);
}

bool WASDKeyboardInput::getRightTap()
{
	if (getRight() && !rightLast)
	{
		rightLast = true;
		return true;
	}
	return false;
}

bool WASDKeyboardInput::getLeft()
{
	return (xMove < 0.0f);
}

bool WASDKeyboardInput::getLeftTap()
{
	if (getLeft() && !leftLast)
	{
		leftLast = true;
		return true;
	}
	return false;
}

bool WASDKeyboardInput::getDown()
{
	return down;
}

bool WASDKeyboardInput::getUp()
{
	return up;
}

bool WASDKeyboardInput::getDownTap()
{
	if (down && !downLast)
	{
		downLast = true;
		return true;
	}
	return false;
}

bool WASDKeyboardInput::getUpTap()
{
	if (up && !upLast)
	{
		upLast = true;
		return true;
	}
	return false;
}

bool WASDKeyboardInput::getJump()
{
	return up;
}

//gets whether the pause button is pressed.
bool WASDKeyboardInput::getPause()
{
	if (pause && !pauseLast)
	{
		pauseLast = true;
		return true;
	}
	return false;
}

bool WASDKeyboardInput::getMenuConfirm()
{
	if (menuConfirm && !menuConfirmLast)
	{
		menuConfirmLast = true;
		return true;
	}
	return false;
}

void WASDKeyboardInput::Update(float elaps) {}