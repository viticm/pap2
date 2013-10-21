// MeshGroup.h: interface for the KModelGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHGROUP_H__88B5D13E_12A7_4EA9_8FD3_BB1EFE67A656__INCLUDED_)
#define AFX_MESHGROUP_H__88B5D13E_12A7_4EA9_8FD3_BB1EFE67A656__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"
#include "MeshBillBoard.h"
namespace ModelSFX
{
#define D3DFVF_OBJECTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

class KModelGroup : public KModelSFXBillBoardBase  
{
public:
	HRESULT Render2Texture();
	HRESULT FrameMove();
	HRESULT Render();
	HRESULT LoadMesh(LPSTR pFileName);
	
	/*struct OBJECTVERTEX
	{
		D3DXVECTOR3 position; // The position
		D3DCOLOR    Color;
		FLOAT       tu, tv;   // The texture coordinates
	};
	typedef struct _Object
	{
		D3DXVECTOR3   Position;
		float         Angle;
		float         Distance;
		float         Size;
	}*LPOBJECT,OBJECT;

	DWORD      m_dwNumObject;
	LPOBJECT*  m_lpObjects;*/
	D3DCOLOR   m_BaseColor;

	LPDIRECT3DTEXTURE9      m_pTexture; // Textures for Render targer mesh
	LPDIRECT3DVERTEXBUFFER9 m_pVerBuffer;
	LPDIRECT3DSURFACE9      m_lpSurfaceDep ;
	
	DWORD                   m_TextureWidth;
	float                   CameraDistance;
	float                   Length;

	KModelGroup();
	virtual ~KModelGroup();

};

};

#endif // !defined(AFX_MESHGROUP_H__88B5D13E_12A7_4EA9_8FD3_BB1EFE67A656__INCLUDED_)
