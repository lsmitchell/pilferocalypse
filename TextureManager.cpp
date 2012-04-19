#include "StdAfx.h"
#include "TextureManager.h"

//creates a new list of textures
TextureManager::TextureManager()
{
	textureList = new pair<WCHAR*, Texture*>[MAX_TEXTURES];
	loadedTextures = 0;
}

//creates a new list of textures using a specified transform matrix
TextureManager::TextureManager(ID3D10Device* dev, D3DXMATRIX* transMatrixIn)
{
	d3ddevice = dev;
	textureList = new pair<WCHAR*, Texture*>[MAX_TEXTURES];
	loadedTextures = 0;
	transMatrix = transMatrixIn;
}

//deconstructor
TextureManager::~TextureManager(void)
{
}

//returns the texture specified by name
Texture* TextureManager::get(WCHAR* name)
{
	/*
	hash_map<WCHAR*,Texture*>::const_iterator finder;
	finder = textureHash.find(name);
	if (finder == textureHash.end())
	{
		// could not find item
		cout << "Could not find texture " << name;
		return NULL;
	}
	else
	{
		return finder->second;
	}*/
	for (int i = 0; i < loadedTextures; i ++)
	{
		pair<WCHAR*, Texture*> newpair = textureList[i];
		if (wcscmp(newpair.first, name) == 0)
		{
			return newpair.second;
		}
	}
	return NULL;
}

//loads in the texture based on filename
HRESULT TextureManager::LoadTexture(WCHAR* filename)
{
	//end of texture was succeeded
	if (loadedTextures >= MAX_TEXTURES)
	{
		cout << "Could not load texture; reached max textures";
		return E_FAIL;
	}

	//set up for new texture
	Texture* newTexture = new Texture();
	newTexture->filename = filename;
	D3DX10_IMAGE_INFO imageInfo;
	D3DX10_IMAGE_LOAD_INFO loadInfo;

	DWORD nDirectoryLength = 400;
	TCHAR lpCurrentDir[400];
	GetCurrentDirectory(nDirectoryLength,lpCurrentDir);
	lstrcat(lpCurrentDir, L"\\");
	lstrcat(lpCurrentDir,filename);

	//loads in info of image
	HRESULT hr = D3DX10GetImageInfoFromFile(lpCurrentDir, NULL, &imageInfo,NULL);

	//check to see if image info failed
	if (FAILED(hr))
	{
		return hr;
	}

	
	//set information on the texture
	newTexture->textureWidth = loadInfo.Width = imageInfo.Width;
	newTexture->textureHeight = loadInfo.Height = imageInfo.Height;
	loadInfo.Depth = imageInfo.Depth;	
	loadInfo.FirstMipLevel = 1;
	loadInfo.MipLevels = imageInfo.MipLevels;
	loadInfo.Usage = D3D10_USAGE_DEFAULT;
	loadInfo.BindFlags = D3D10_BIND_SHADER_RESOURCE ;
	loadInfo.CpuAccessFlags = 0;
	loadInfo.MiscFlags = 0;
	loadInfo.Format = imageInfo.Format;
	loadInfo.Filter = D3DX10_FILTER_NONE;
	loadInfo.MipFilter = D3DX10_FILTER_NONE;
	loadInfo.pSrcInfo = &imageInfo;

	//create the shader
	hr = D3DX10CreateShaderResourceViewFromFile(d3ddevice, lpCurrentDir, &loadInfo, NULL, &newTexture->texture,NULL);

	//check to see if the shader had issues
	if( FAILED(hr) )
	{
		return hr;
	}

	//create the sprite
	hr = D3DX10CreateSprite(d3ddevice, 1, &newTexture->sprite);

	//check to see if the sprite had issues
	if( FAILED(hr) )
	{
		return hr;
	}


	newTexture->sprite->SetProjectionTransform(transMatrix);//set the transform matrix
	//textureHash[filename] = newTexture;
	textureList[loadedTextures] = pair<WCHAR*, Texture*>(filename, newTexture);//set the texture in the list
	loadedTextures ++;//go to next texture
	return S_OK;
}