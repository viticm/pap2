// MeshLineGrass.h: interface for the KModelSFXLineGrass class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHLINEGRASS_H__42C46A88_08AC_48CF_9B59_310745619DD3__INCLUDED_)
#define AFX_MESHLINEGRASS_H__42C46A88_08AC_48CF_9B59_310745619DD3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MeshBillBoard.h"

namespace ModelSFX
{
class KModelSFXLineGrass : public KModelSFXBillBoardBase  
{
public:
	struct LINE{
		D3DXVECTOR3 Pos;
		float       PSize;
		D3DCOLOR    Color;
	};
#define LINE_FVF (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_PSIZE)
	int   m_nNumLinePerGrass;

protected:
	HRESULT CreateVerticesBuffer();
	
public:
	HRESULT FrameMove();
	HRESULT UpdateVertices();
	HRESULT Render();
	HRESULT LoadMesh(LPSTR pFileName);
	KModelSFXLineGrass();
	virtual ~KModelSFXLineGrass();

};
};

#endif // !defined(AFX_MESHLINEGRASS_H__42C46A88_08AC_48CF_9B59_310745619DD3__INCLUDED_)
