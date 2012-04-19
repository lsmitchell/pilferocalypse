#pragma once

// IController represents any control scheme available
// to the player, and contains a series of methods
// made available to game menus and playeractors.
class IController abstract
{
public:
	IController(void);
	~IController(void);
	virtual void KeyUp(WPARAM) = 0;
	virtual void KeyDown(WPARAM) = 0;
	virtual float getXMove() = 0;
	virtual bool getUp() = 0;
	virtual bool getUpTap() = 0;
	virtual bool getDown() = 0;
	virtual bool getDownTap() = 0;
	virtual bool getJump() = 0;
	virtual bool getPause() = 0;
	virtual bool getMenuConfirm() = 0;
	virtual bool getRight() = 0;
	virtual bool getRightTap() = 0;
	virtual bool getLeft() = 0;
	virtual bool getLeftTap() = 0;
	virtual void Update(float) = 0;
};

