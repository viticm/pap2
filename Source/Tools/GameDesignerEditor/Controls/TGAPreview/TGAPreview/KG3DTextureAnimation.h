#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <string>

using namespace std;
class KG3DTextureAnimation
{
private:
	//LPDIRECT3DTEXTURE9*  m_lpBackTextures;
	vector<LPDIRECT3DTEXTURE9> m_textures;
	vector<string> m_texturefilenames;
	DWORD  m_dwNumFrame;
	DWORD  m_dwCurFrame;
	LPDIRECT3DTEXTURE9   m_pTexture;
	IDirect3DDevice9*	m_pDevice;
public:
	HRESULT UnInit();
	
	HRESULT Clean();

	virtual HRESULT Update(int nFrame);//¸üÐÂÌùÍ¼

	HRESULT LoadFromFile(LPCTSTR cszFileName);

	LPDIRECT3DTEXTURE9 GetCurrentTexture() {return m_pTexture;}
	KG3DTextureAnimation(IDirect3DDevice9* pDevice);
	virtual ~KG3DTextureAnimation(void);
};
