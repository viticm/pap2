#include "StdAfx.h"
#include "KG3DTextureAnimation.h"

/*#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
////////////////////////////////////////////////////////////////////////////////

KG3DTextureAnimation::KG3DTextureAnimation(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	m_pDevice->AddRef();

	m_pTexture = NULL;
	//m_lpBackTextures = NULL;
	m_dwNumFrame = 0;
	m_dwCurFrame = -1;
}

KG3DTextureAnimation::~KG3DTextureAnimation(void)
{
	UnInit();
}

HRESULT KG3DTextureAnimation::UnInit()
{
	Clean();
	//SAFE_RELEASE(m_pTexture);
	m_textures.clear();
	m_texturefilenames.clear();
	SAFE_RELEASE(m_pDevice);
	return S_OK;//KG3DTexture::UnInit();
}

HRESULT KG3DTextureAnimation::Clean()
{
	for(DWORD i=0;i<m_textures.size();i++)
	{
		SAFE_RELEASE(m_textures[i])
	}
	SAFE_RELEASE(m_pTexture);
	m_textures.clear();
	m_texturefilenames.clear();
	//SAFE_DELETE_ARRAY(m_lpBackTextures);
	m_dwNumFrame = 0;
	m_dwCurFrame = -1;
	return S_OK;
}

HRESULT KG3DTextureAnimation::LoadFromFile(LPCTSTR cszFileName)
{
	//Clean();
	HANDLE hFile = NULL; 
	hFile = CreateFile(cszFileName,    // file to open
		GENERIC_READ,          // open for reading
		FILE_SHARE_READ,       // share for reading
		NULL,                  // default security
		OPEN_EXISTING,         // existing file only
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL);                 // no attr. template
	if (hFile == INVALID_HANDLE_VALUE) 
		return E_FAIL;

	CloseHandle(hFile);

	//m_lpBackTextures = new LPDIRECT3DTEXTURE9[m_dwNumFrame];
	//ZeroMemory(m_lpBackTextures,sizeof(LPDIRECT3DTEXTURE9)*m_dwNumFrame);
	m_dwNumFrame ++;
	if (m_textures.size() < m_dwNumFrame)
	{
		LPDIRECT3DTEXTURE9 pTexture = NULL;
		m_textures.push_back(pTexture);
		m_texturefilenames.push_back(cszFileName);
	}
	//m_dwNumFrame = (DWORD)m_textures.size();

	return S_OK;//D3DXCreateTextureFromFile(pDevice, cszFileName, &m_textures[m_dwNumFrame - 1]);
}

HRESULT KG3DTextureAnimation::Update(int nFrameStep)
{
	if (m_dwNumFrame <= 0)
		return E_FAIL;

	m_dwCurFrame ++;
	if (m_dwCurFrame >= m_dwNumFrame)
		m_dwCurFrame = 0;

	SAFE_RELEASE(m_pTexture);
	if (m_textures[m_dwCurFrame] == NULL)
	{
		D3DXCreateTextureFromFile(m_pDevice, m_texturefilenames[m_dwCurFrame].c_str(), &m_textures[m_dwCurFrame]);
	}
	m_pTexture = m_textures[m_dwCurFrame];
	m_pTexture->AddRef();

	return S_OK;
}
