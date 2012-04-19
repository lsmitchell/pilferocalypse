#include "StdAfx.h"
#include "SimpleSprite.h"
#include "Camera.h"

extern Texture* getTexture(WCHAR*);
extern Camera* mainCamInst;

//constructor that sets all sprites to zero out
SimpleSprite::SimpleSprite()
{
	ZeroValues();
	spriteType = NotSet;
}

//constructor that creates a sprite based on a file
SimpleSprite::SimpleSprite(ID3D10Device* dxDevice, WCHAR* filename)
{
	ZeroValues();
	d3ddevice = dxDevice;
	SetTexture(filename);
	InitSpriteInfo();
	spriteType = NotSet;
}

//Sets all the values of the sprite to zero, to avoid
//uninitialized memory errors.
void SimpleSprite::ZeroValues()
{
	d3ddevice = 0;
	ZeroMemory(&spriteInfo, sizeof(D3DX10_SPRITE));
	ZeroMemory(&myTexture, sizeof(Texture));
	xPosition = -1;
	yPosition = -1;
	scaleX = 1.0f;
	scaleY = 1.0f;
	Visible = true;
}

//sets the 'spriteInfo' object's info appropriately,
//and sets the texture.
void SimpleSprite::InitSpriteInfo()
{
	spriteInfo.TexCoord.x = 0.0f;
	spriteInfo.TexCoord.y = 0.0f;
	spriteInfo.TexSize.x = 1.0f;
	spriteInfo.TexSize.y = 1.0f;
	spriteInfo.ColorModulate = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	spriteInfo.pTexture = myTexture->texture;
	spriteHeight = myTexture->textureHeight;
	spriteWidth = myTexture->textureWidth;
	spriteInfo.TextureIndex = 0;
	scaleX = 1.0f;
	scaleY = 1.0f;
}

//retrieves the texture associated with the given name,
//and sets it to the sprite.
void SimpleSprite::SetTexture(WCHAR* textureName)
{
	myTexture = getTexture(textureName);
}

//sets the currently assigned texture, by object
void SimpleSprite::SetTexture(Texture* texture)
{
	myTexture = texture;
}

//sets the current position of the sprite
void SimpleSprite::SetPosition(float x, float y, float z)
{
	//ignore Z since we're not using it
	//D3DXMatrixTranslation(&spriteInfo.matWorld, x, y, 0.0f);
	xPosition = x;
	yPosition = y;
	RefreshMatrix();
}

//sets the scaling amount on the current sprite; the same value for both x and y
void SimpleSprite::SetScaling(float scale)
{
	scaleX = scale;
	scaleY = scale;
	RefreshMatrix();
}

//sets the scaling amount on the current sprite
void SimpleSprite::SetScaling(float x, float y)
{
	scaleX = x;
	scaleY = y;
	RefreshMatrix();
}

//multiplies the current scale by the specified amounts.
void SimpleSprite::AdjustScaling(float x, float y)
{
	scaleX *= x;
	scaleY *= y;
	RefreshMatrix();
}

//sets the sprite's current matrix to translation and scaling
//based on the positional values, so it can be drawn.
void SimpleSprite::RefreshMatrix()
{

	D3DXMATRIX transform;
	
	if (!(spriteType == MainMenu || spriteType == LobbyMenu || spriteType == TimeMenu || spriteType == InstructMenu || spriteType == Selector))
	{
		float camX, camY, camXYScale;
		D3DXMATRIX camTranslate, camScale, translate, scaling;
		::mainCamInst->GetPosition(&camX, &camY);
		camXYScale = 1.0f / ::mainCamInst->getScale();
		D3DXMatrixTranslation(&translate, (-camX + xPosition) * camXYScale, (-camY + yPosition) * camXYScale, 0.0f);
		D3DXMatrixScaling(&camScale, 1.0f / camXYScale, 1.0f / camXYScale, 0.0f);
		//D3DXMatrixTranslation(&translate, xPosition, yPosition, 0.0f);
		D3DXMatrixScaling(&scaling, scaleX * camXYScale, scaleY * camXYScale, 1.0f);

		D3DXMatrixIdentity(&transform);
		//D3DXMatrixMultiply(&translate, &camTranslate, &translate);

		D3DXMatrixMultiply(&transform, &translate, &transform);
		D3DXMatrixMultiply(&transform, &scaling, &transform);
		
		//D3DXMatrixMultiply(&transform, &camTranslate, &transform);
		
		
	}
	else
	{
		D3DXMATRIX scaling;
		// just apply scaling
		D3DXMatrixTranslation(&transform, xPosition, yPosition, 0.0f);
		D3DXMatrixScaling(&scaling, scaleX, scaleY, 1.0f);
		D3DXMatrixMultiply(&transform, &scaling, &transform);
	}
	spriteInfo.matWorld = transform;
	/*
	D3DXMATRIX translation, scaling;
	D3DXMatrixTranslation(&translation, xPosition, yPosition, 0.0f);
	if (!(spriteType == MainMenu || spriteType == TimeMenu || spriteType == InstructMenu || spriteType == Selector))
	{
		float camX, camY;
		D3DXMATRIX camTranslate;
		::mainCamInst->GetPosition(&camX, &camY);
		D3DXMatrixTranslation(&camTranslate, -camX, -camY, 0.0f);
		D3DXMatrixMultiply(&translation, &camTranslate, &translation);
	}
	D3DXMatrixScaling(&scaling, scaleX, scaleY, 1.0f);
	D3DXMatrixMultiply(&spriteInfo.matWorld, &scaling, &translation);
	*/
	
}

//draws the sprite
void SimpleSprite::Draw()
{
	if (myTexture != NULL && Visible)
	{
		RefreshMatrix();
		ID3DX10Sprite* sprite = myTexture->sprite;
		sprite->Begin(D3DX10_SPRITE_SORT_TEXTURE);
		sprite->DrawSpritesBuffered(&spriteInfo, 1);
		sprite->Flush();
		sprite->End();
	}
}

// Lets us set which type of sprite this is
void SimpleSprite::SetSpriteType(SpriteType type)
{
	spriteType = type;
}

// Getter that lets other know what type of sprite this is
SpriteType SimpleSprite::GetSpriteType()
{
	return spriteType;
}

//deconstructor
SimpleSprite::~SimpleSprite(void)
{
}
