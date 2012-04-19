#pragma once
#include "icontroller.h"
#include <XInput.h> // XInput API

// An extension of the IController interface made for game controllers, most commonly
// the Xbox 360 controller.
class XinputController :
	public IController
{
private:
	float xMove;
	float yMove;
	float deadzone;
	int controlNumber;
	XINPUT_STATE state;
	XINPUT_STATE lastState;

	bool disconnected;
	bool disconnectReported;

	float getYMove();
	float innerGetX(XINPUT_STATE*);
	float innerGetY(XINPUT_STATE*);
public:
	XinputController(int);
	~XinputController(void);
	void KeyDown(WPARAM);
	void KeyUp(WPARAM);
	float getXMove();
	bool getUp();
	bool getUpTap();
	bool getDown();
	bool getDownTap();
	bool getJump();
	bool getPause();
	bool getMenuConfirm();
	bool getRight();
	bool getRightTap();
	bool getLeft();
	bool getLeftTap();
	void Update(float);
};

