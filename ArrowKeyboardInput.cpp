#include "StdAfx.h"
#include "ArrowKeyboardInput.h"
//#include "dxManager.h"

// create a new ArrowKeyboardInput object.
ArrowKeyboardInput::ArrowKeyboardInput(void)
{
	xMove = 0.0f;
	up = false;
	down = false;
	upLast = false;
	downLast = false;
	leftLast = false;
	rightLast = false;
	pause = false;
	menuConfirm = false;
}


ArrowKeyboardInput::~ArrowKeyboardInput(void)
{
}

//KeyDown is called when the Win32 message loop detects
//a key-press WPARAM. This function reads and stores
//the information.
void ArrowKeyboardInput::KeyDown(WPARAM param)
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
		case VK_RIGHT:
			xMove = 1.0f;
			break;
		case VK_LEFT:
			xMove = -1.0f;
			break;
		case VK_UP:
			up = true;
			upLast = false;
			break;
		case VK_DOWN:
			down = true;
			downLast = false;
		}
}

//KeyUp is called when the Win32 message loop detects
//a key-press WPARAM. This function reads and stores
//the information.
void ArrowKeyboardInput::KeyUp(WPARAM param)
{
	switch (param)
		{
		case 'p':
		case 'P':
			pause = false; // pause when escape key is hit
			break;
		case VK_RETURN:
			menuConfirm = true;
		case VK_RIGHT:
			if (xMove == 1.0f) xMove = 0.0f;
			rightLast = false;
			break;
		case VK_LEFT:
			if (xMove == -1.0f) xMove = 0.0f;
			leftLast = false;
			break;
		case VK_UP:
			up = false;
			upLast = false;
			break;
		case VK_DOWN:
			down = false;
			downLast = false;
			break;
		}
}

//returns the float representing which direction left-to-right the
//player is pressing.
float ArrowKeyboardInput::getXMove()
{
	return xMove;
}

bool ArrowKeyboardInput::getRight()
{
	return (xMove > 0.0f);
}

bool ArrowKeyboardInput::getRightTap()
{
	if (getRight() && !rightLast)
	{
		rightLast = true;
		return true;
	}
	return false;
}

bool ArrowKeyboardInput::getLeft()
{
	return (xMove < 0.0f);
}

bool ArrowKeyboardInput::getLeftTap()
{
	if (getLeft() && !leftLast)
	{
		leftLast = true;
		return true;
	}
	return false;
}

bool ArrowKeyboardInput::getDown()
{
	return down;
}

bool ArrowKeyboardInput::getUp()
{
	return up;
}

bool ArrowKeyboardInput::getDownTap()
{
	if (down && !downLast)
	{
		downLast = true;
		return true;
	}
	return false;
}

bool ArrowKeyboardInput::getUpTap()
{
	if (up && !upLast)
	{
		upLast = true;
		return true;
	}
	return false;
}

bool ArrowKeyboardInput::getJump()
{
	return up;
}

//gets whether the pause button is pressed.
// the WASDkeyboard class handles this, so there is no need to check it here.
bool ArrowKeyboardInput::getPause()
{
	/*
	if (pause)
	{
		pause = false;
		return true;
	}*/
	return false;
}

// the WASDkeyboard class handles this, so there is no need to check it here.
bool ArrowKeyboardInput::getMenuConfirm()
{
	/*
	if (menuConfirm)
	{
		menuConfirm = false;
		return true;
	}*/
	return false;
}

void ArrowKeyboardInput::Update(float elaps) {}