#pragma once
#include "IController.h"

//KeyboardInput represents a generic object for
//PlayerActor or any other controlling object (say,
//menu options) to poll for control-presses. This will
//be extended to alternate keyboard arrangement and
//controllers.
class ArrowKeyboardInput : public IController
{
private:
	float xMove;
	bool up;
	bool upLast;
	bool down;
	bool downLast;
	bool leftLast;
	bool rightLast;
	bool pause;
	bool menuConfirm;
public:
	ArrowKeyboardInput(void);
	~ArrowKeyboardInput(void);
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

