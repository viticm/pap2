// MeshMoveGrid.h: interface for the KModelGridMove class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHMOVEGRID_H__1DDAEF62_60AB_4AE3_B7E5_ED8B10CEBD73__INCLUDED_)
#define AFX_MESHMOVEGRID_H__1DDAEF62_60AB_4AE3_B7E5_ED8B10CEBD73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"

class KModelGridMove : public KModel  
{
public:
	LPMODEL m_lpMeshAxisX;
	LPMODEL m_lpMeshAxisY;
	LPMODEL m_lpMeshAxisZ;
	LPMODEL m_lpMeshAxisXYZ;

	LPMODEL m_lpMeshAxisYZ;
	LPMODEL m_lpMeshAxisYX;
	LPMODEL m_lpMeshAxisXZ;

	BOOL    m_bSelectedX;
	BOOL    m_bSelectedY;
	BOOL    m_bSelectedZ;

	BOOL    m_bSelectedXYZ;
	BOOL    m_bSelectedYZ;
	BOOL    m_bSelectedYX;
	BOOL    m_bSelectedXZ;

public:
	void SetMatrix(D3DXMATRIX* pMat);
	
	HRESULT SelectByPickRay(D3DXVECTOR3 *RayOrig, D3DXVECTOR3 *RayDir);
	HRESULT Render();
	HRESULT LoadMesh(LPSTR pFileName);
	KModelGridMove();
	virtual ~KModelGridMove();

};

#endif // !defined(AFX_MESHMOVEGRID_H__1DDAEF62_60AB_4AE3_B7E5_ED8B10CEBD73__INCLUDED_)
