#pragma once
#include "TextureManager.h"

// Used to Identify what kind of sprite this is
enum SpriteType
{
	Player, ObstacleTrap, Object, MainMenu, TimeMenu, InstructMenu, LobbyMenu, NotSet, Selector
};

class SimpleSprite
{
private:
	Texture* myTexture;
	D3DX10_SPRITE spriteInfo;
	ID3D10Device* d3ddevice;
	float xPosition;
	float yPosition;
	float scaleX, scaleY;
	
	WCHAR* originalFilename;
	SpriteType spriteType; // Identifies what kind of sprite we are using

	void RefreshMatrix();
	
public:
	bool Visible;

	HRESULT creationError;
	SimpleSprite(void);
	SimpleSprite(ID3D10Device* dxDevice, WCHAR* filename);
	void ZeroValues();
	void InitSpriteInfo();
	void SetTexture(WCHAR* textureName);
	void SetTexture(Texture* texture);
	void SetPosition(float, float, float);
	void SetScaling(float);
	void SetScaling(float, float);
	void AdjustScaling(float, float);
	int spriteWidth, spriteHeight;
	void Draw();
	void SetSpriteType(SpriteType type);
	SpriteType GetSpriteType();
	~SimpleSprite(void);
};

