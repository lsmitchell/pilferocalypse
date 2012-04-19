#include "StdAfx.h"
#include "XinputController.h"

#define XINPUT_DEFAULT_DEADZONE 0.23f
#define JOYSTICK_MAX 32767 // I HATE API'S THAT HAVE ARBITRARY NUMBERS LIKE THIS NOT DEFINED AS A CONSTANT ANYWHERE ARRGLBFJLHWACAAAAGH

XinputController::XinputController(int controllerNum)
{
	controlNumber = controllerNum;
	xMove = 0.0f;
	yMove = 0.0f;
	disconnected = true;
	disconnectReported = true;
	deadzone = XINPUT_DEFAULT_DEADZONE;
	DWORD dwResult = XInputGetState(controlNumber, &state);
	lastState = state;
}


XinputController::~XinputController(void)
{
}

void XinputController::Update(float elaps)
{
	lastState = state;
	DWORD dwResult = XInputGetState(controlNumber, &state);
	if (dwResult == ERROR_SUCCESS) // error success! This actually means the controller is connected.
	{
		disconnectReported = false;
		disconnected = false;
	}
	else
		disconnected = true;
}

float XinputController::innerGetX(XINPUT_STATE* refState)
{
	if (disconnected) return 0.0f;
	if (refState->Gamepad.sThumbLX > (deadzone * JOYSTICK_MAX) || refState->Gamepad.sThumbLX < (-deadzone * JOYSTICK_MAX))
		return (float)refState->Gamepad.sThumbLX / (float)JOYSTICK_MAX;
	else
		return 0.0f;
}

float XinputController::innerGetY(XINPUT_STATE* refState)
{
	if (disconnected) return 0.0f;
	if (refState->Gamepad.sThumbLY > (deadzone * JOYSTICK_MAX) || refState->Gamepad.sThumbLY < (-deadzone * JOYSTICK_MAX))
		return (float)refState->Gamepad.sThumbLY / (float)JOYSTICK_MAX;
	else
		return 0.0f;
}

float XinputController::getXMove()
{
	return innerGetX(&state);
}

float XinputController::getYMove()
{
	return innerGetY(&state);
}

bool XinputController::getRight()
{
	return (getXMove() > 0.0f);
}

bool XinputController::getRightTap()
{
	return (innerGetX(&state) > 0.0f && !(innerGetX(&lastState) > 0.0f));
}

bool XinputController::getLeft()
{
	return (getXMove() < 0.0f);
}

bool XinputController::getLeftTap()
{
	return (innerGetX(&state) < 0.0f && !(innerGetX(&lastState) < 0.0f));
}

bool XinputController::getDown()
{
	return (getYMove() < 0.0f);
}

bool XinputController::getUp()
{
	return (getYMove() > 0.0f);
}

bool XinputController::getDownTap()
{
	return (innerGetY(&state) < 0.0f && !(innerGetY(&lastState) < 0.0f));
}

bool XinputController::getUpTap()
{
	return (innerGetY(&state) > 0.0f && !(innerGetY(&lastState) > 0.0f));
}

bool XinputController::getJump()
{
	if (disconnected) return false;
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A )
		return true;
	return false;
}

//gets whether the pause button is pressed.
bool XinputController::getPause()
{
	if (disconnected && !disconnectReported)
	{
		// pause if a controller was just disconnected
		disconnectReported = true;
		return true;
	}
	else if (!disconnected && (state.Gamepad.wButtons & XINPUT_GAMEPAD_START ) && !(lastState.Gamepad.wButtons & XINPUT_GAMEPAD_START))
	{
		// not disconnected, Start is pressed and wasn't pressed last frame
		return true;
	}
	return false;
}

bool XinputController::getMenuConfirm()
{
	if (disconnected) return false;
	if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A ) && !(lastState.Gamepad.wButtons & XINPUT_GAMEPAD_A))
	{
		return true;
	}
	return false;
}

void XinputController::KeyDown(WPARAM wp) {}

void XinputController::KeyUp(WPARAM wp) {}