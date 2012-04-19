#include "StdAfx.h"
#include "Camera.h"


// for reference: A horizontal wall is about 0.45 units wide.
// default width and height
#define CAMERA_DEF_WIDTH 2.7f
#define CAMERA_DEF_HEIGHT 2.7f
// minimum the camera can scale to. Prevents unnecessary closeups.
#define CAMERA_MIN_SCALE 1.0f
// target safe area is the intended distance between the targets and the edge
// of the screen; this is meant to be enough space to see the next platform
// to jump to.
#define TARGET_SAFE_AREA 2.0f
// space to interpolate between where the camera is, and where it wants to be.
// this is scaled by time elapsed, so it doesn't mean you'll suddenly go twice
// as far as you wanted to.
#define CAMERA_FOLLOW_RATE 2.0f
// start using a faster movement system when the targets are farther than this
#define CAMERA_FAST_FOLLOW_DIST 3.4f
// faster movement rate
#define CAMERA_FAST_FOLLOW_RATE 5.0f
// defines the camera's rate of zooming in or out to encompass all targets
#define CAMERA_ZOOM_RATE 0.07f
// used to initialize array of targets
#define CAMERA_MAX_TARGETS 5

//creates a new camera
Camera::Camera(dxManager* dx) : Actor(dx)
{
	zoom = 0.0f;
	targets = new ITrackable*[CAMERA_MAX_TARGETS];
	numTargets = 0;
	cameraScale = 1.0f;
}

//deconstructor
Camera::~Camera(void)
{
}

//updates the matrix
void Camera::RefreshMatrix()
{
	D3DXMatrixTranslation(&currentMatrix, -xPosition, -yPosition, zoom);
}

//updates the camera's position
void Camera::Update(float timePassed)
{
	// for now, track one target
	if (numTargets > 0)
	{
		float xLeft = FLT_MAX, xRight = -FLT_MAX, xMid, yTop = -FLT_MAX, yBottom = FLT_MAX, yMid;

		// get the farthest edges our targets are on
		for (int i = 0; i < numTargets; i ++)
		{
			float tX, tY;
		
			targets[i]->GetPosition(&tX, &tY);
			xLeft = min(tX, xLeft);
			xRight = max(tX, xRight);
			yTop = max(tY, yTop);
			yBottom = min(tY, yBottom);
		}
		xMid = xLeft + (xRight - xLeft) * 0.5f;
		yMid = yBottom + (yTop - yBottom) * 0.5f;

		float dx, dy;
		dx = xMid - xPosition;
		dy = yMid - yPosition;
		float targDistSquared = dx * dx + dy * dy;
		float moveRate;
		// if we are more than a certain distance from our target, move quickly to catch up
		if (targDistSquared < CAMERA_FAST_FOLLOW_DIST * CAMERA_FAST_FOLLOW_DIST)
		{
			moveRate = CAMERA_FOLLOW_RATE * timePassed;
		}
		else
		{
			moveRate = CAMERA_FAST_FOLLOW_RATE * timePassed;
		}
		
		SetPosition(xPosition + moveRate * dx, yPosition + moveRate * dy);
		// determine the amount of x space we should allow to show everything on-camera
		float targetWidth = xRight - xLeft + TARGET_SAFE_AREA;
		float targetHeight = yTop - yBottom + TARGET_SAFE_AREA;
		float targetScale = max(CAMERA_MIN_SCALE, max(targetWidth / CAMERA_DEF_WIDTH, targetHeight / CAMERA_DEF_HEIGHT));
		if ( abs(cameraScale - targetScale) > 0.05f)
			cameraScale += (targetScale - cameraScale) * CAMERA_ZOOM_RATE;
		RefreshMatrix();
	}
}

float Camera::getScale()
{
	return cameraScale;
}

//get the space height of the camera
float Camera::GetLSpaceHeight()
{
	return cameraScale * CAMERA_DEF_HEIGHT;
}

//get the space width of the camera
float Camera::GetLSpaceWidth()
{
	return cameraScale * CAMERA_DEF_WIDTH;
}

//gets the current matrix for the camera
D3DXMATRIX Camera::getMatrix()
{
	return currentMatrix;
}

//tracks the camera's target point
void Camera::AddTrackingTarget(ITrackable* actor)
{
	targets[numTargets] = actor;
	numTargets ++;
}

void Camera::ClearTargets()
{
	numTargets = 0;
}

void Camera::RemoveTrackingTarget(ITrackable* actor)
{
	// Find the actor we no londer want and set it to null
	for(int i = 0; i < numTargets; i++)
	{
		if(targets[i] == actor)
		{
			targets[i] = NULL;
		}
	}

	// Copy our non-null values into a temp array
	int val = 0;
	ITrackable** tempArray = new ITrackable*[numTargets];
	for(int j = 0; j < numTargets; j++)
	{
		if(targets[j] != NULL)
		{
			tempArray[val] = targets[j];
			val++;
		}
	}
	// put them back in the old array so null value is gone
	numTargets = val;
	for(int i = 0; i < numTargets; i++)
	{
		targets[i] = tempArray[i];
	}

}

void Camera::CenterOn(ITrackable* actor)
{
	float x, y;
	actor->GetPosition(&x, &y);
	SetPosition(x, y);
}