#pragma once
#include <d3d10.h>
#include <d3dx10.h>
#include <hash_map>
#include <iostream>
#define MAX_TEXTURES 100

using namespace std;

struct Texture
{
public:
	ID3D10ShaderResourceView* texture;
	ID3DX10Sprite* sprite;
	WCHAR* filename;
	int textureWidth;
	int textureHeight;
};

class TextureManager
{
private:
	pair<WCHAR*, Texture*>* textureList;
	int loadedTextures;
	//hash_map<WCHAR*, Texture*> textureHash;
	ID3D10Device* d3ddevice;
	D3DXMATRIX* transMatrix;
public:
	TextureManager();
	TextureManager(ID3D10Device*, D3DXMATRIX*);
	~TextureManager(void);
	Texture* get(WCHAR* name);
	HRESULT LoadTexture(WCHAR*);
};