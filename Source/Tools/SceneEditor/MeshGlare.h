// MeshGlare.h: interface for the CMeshGlare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHGLARE_H__D062E232_4668_4CB6_97B4_8019C880F0B0__INCLUDED_)
#define AFX_MESHGLARE_H__D062E232_4668_4CB6_97B4_8019C880F0B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MeshBillBoard.h"

namespace ModelSFX
{
class KModelSFXHalo : public KModelSFXBillBoardBase
{
public:
	struct HALO
	{
		D3DXVECTOR3 Position;
		DWORD       TextureID;
		int         Animation;
		float       AniTime;
		float       Sequence;
		HALO()
		{
			Position = D3DXVECTOR3(0,0,0);
			TextureID = 0;
			Animation = -1;
			AniTime = 0;
		}
		BOOL operator < ( CONST HALO& HH) const
		{
			return Sequence<HH.Sequence;
		}
	};
	list<HALO*>m_HaloList;
	D3DXVECTOR3   m_CurPosition;
	
	D3DCOLOR   m_BaseColor;
	int        m_nMaxNumHalo;

	vector<LPDIRECT3DTEXTURE9>m_pTexture; // Textures for Render targer mesh
	LPDIRECT3DVERTEXBUFFER9 m_pVerBuffer;
	LPDIRECT3DSURFACE9      m_lpSurfaceDep ;
	
	DWORD  T;
	DWORD                   m_TextureWidth;
	float                   CameraDistance;
	float                   Length;
	D3DXVECTOR2             m_A;
	D3DXVECTOR2             m_C;
	float                   m_Z;
	int                     m_nTextureCount;
public:
	HRESULT SortHalo();
	HRESULT UpdateVertices();
	HRESULT Remove1Halo();
	HRESULT AddHalo(HALO* NewHalo);
	HRESULT FrameMove();
	HRESULT Render2Texture(int TextIndex);
	HRESULT LoadMesh(LPSTR pFileName);
	HRESULT Render();
	KModelSFXHalo();
	virtual ~KModelSFXHalo();

};
};

#endif // !defined(AFX_MESHGLARE_H__D062E232_4668_4CB6_97B4_8019C880F0B0__INCLUDED_)
