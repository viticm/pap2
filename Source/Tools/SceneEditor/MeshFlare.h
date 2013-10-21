// MeshFlare.h: interface for the KModelSFXFlare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHFLARE_H__424B5747_A910_4E79_82A1_7F25035709EF__INCLUDED_)
#define AFX_MESHFLARE_H__424B5747_A910_4E79_82A1_7F25035709EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"
namespace ModelSFX
{
const DWORD VertexFVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 );

class KModelSFXFlare : public KModel  
{

public:
	struct TLVertex
	{
		D3DXVECTOR3 position;
		D3DCOLOR    diffuse;
		FLOAT       tu, tv;
	};


	DWORD         m_dwNumLayer ;
	D3DXVECTOR4*  m_lpLayerPosition;
	
public:
	HRESULT Render();
	BOOL m_bShow;
	HRESULT FrameMove();
	HRESULT LoadMesh(LPSTR pFileName);
	KModelSFXFlare();
	virtual ~KModelSFXFlare();

};

};

#endif // !defined(AFX_MESHFLARE_H__424B5747_A910_4E79_82A1_7F25035709EF__INCLUDED_)
