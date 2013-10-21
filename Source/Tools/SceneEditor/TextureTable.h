// TextureTable.h: interface for the KTextureTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURETABLE_H__B40CDE05_5B79_4C98_9DBF_6A667B8E39EB__INCLUDED_)
#define AFX_TEXTURETABLE_H__B40CDE05_5B79_4C98_9DBF_6A667B8E39EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Texture.h"

class KTextureTable  
{
private:
	DWORD      m_dMaxNumTexture;
	LPTEXTURE* m_lpTextures;
public:
	HRESULT    ReloadFromFile();
	HRESULT    SetTexture(DWORD Index,DWORD TextureID);
	virtual    HRESULT CleanUp();
	HRESULT	   GetTexture(LPTEXTURE* ppTexture,DWORD ID);
	HRESULT	   LoadCubeFromFile(DWORD *pTextureID, LPSTR pFileName);
	HRESULT    LoadVolumeFromFile(DWORD *pTextureID, LPSTR pFileName);
	HRESULT    LoadFromFile(DWORD* pTextureID,LPSTR pFileName);
	HRESULT    Get1NewTexture(LPTEXTURE* ppTexture,DWORD Type);
	HRESULT    Get1NewTexturePointer(LPTEXTURE** pppTexture);
	HRESULT    LoadMovieFromFile(DWORD* pTextureID, TCHAR* pFileName);
	DWORD      m_dNumTexture;

	KTextureTable();
	virtual ~KTextureTable();

	HRESULT InvalidateDeviceObjects(void);
	HRESULT RestoreDeviceObjects(void);
};

#endif // !defined(AFX_TEXTURETABLE_H__B40CDE05_5B79_4C98_9DBF_6A667B8E39EB__INCLUDED_)
