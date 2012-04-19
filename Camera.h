#pragma once
#include "Actor.h"

// the Camera is an instance of an Actor that is not drawn. It does update, however
class Camera : public Actor
{
private:
	D3DXMATRIX currentMatrix;
	float zoom;
	ITrackable** targets;
	int numTargets;
	float cameraScale;

	void RefreshMatrix();
public:
	Camera(dxManager*);
	~Camera(void);
	void AddTrackingTarget(ITrackable* actor);
	void ClearTargets();
	void RemoveTrackingTarget(ITrackable* actor);
	void CenterOn(ITrackable* actor);
	void Update(float timePassed);
	float getScale();
	float GetLSpaceWidth();
	float GetLSpaceHeight();
	D3DXMATRIX getMatrix();
};