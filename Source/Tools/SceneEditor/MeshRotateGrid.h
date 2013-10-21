// MeshRotateGrid.h: interface for the KModelGridRotate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHROTATEGRID_H__ED174E73_31CA_458D_BDD5_7EAD2C4F26A8__INCLUDED_)
#define AFX_MESHROTATEGRID_H__ED174E73_31CA_458D_BDD5_7EAD2C4F26A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"

class KModelGridRotate : public KModel  
{
public:
	LPMODEL m_lpMeshYaw;
	LPMODEL m_lpMeshPitch;
	LPMODEL m_lpMeshRoll;
	BOOL    m_bSelectedYaw;
	BOOL    m_bSelectedPitch;
	BOOL    m_bSelectedRoll;

	float       m_Scaling;
	D3DXVECTOR3 m_Center;
public:
	HRESULT SelectByPickRay(D3DXVECTOR3 *RayOrig, D3DXVECTOR3 *RayDir);
	void SetMatrix(float Scaling,D3DXVECTOR3* pCenter);
	HRESULT Render();
	HRESULT LoadMesh(LPSTR pFileName);
	KModelGridRotate();
	virtual ~KModelGridRotate();

};

#endif // !defined(AFX_MESHROTATEGRID_H__ED174E73_31CA_458D_BDD5_7EAD2C4F26A8__INCLUDED_)
