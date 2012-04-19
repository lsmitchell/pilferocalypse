#pragma once
#include "IController.h"

//KeyboardInput represents a generic object for
//PlayerActor or any other controlling object (say,
//menu options) to poll for control-presses. This will
//be extended to alternate keyboard arrangement and
//controllers.
class WASDKeyboardInput : public IController
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
	bool pauseLast;
	bool menuConfirm;
	bool menuConfirmLast;
public:
	WASDKeyboardInput(void);
	~WASDKeyboardInput(void);
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

