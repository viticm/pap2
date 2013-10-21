// MeshRotateGrid.cpp: implementation of the KModelGridRotate class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshRotateGrid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KModelGridRotate::KModelGridRotate()
{
	m_lpMeshYaw = NULL;
	m_lpMeshPitch = NULL;
	m_lpMeshRoll = NULL;
	m_bSelectedYaw = TRUE;
	m_bSelectedPitch = TRUE;
	m_bSelectedRoll =  TRUE;

	m_Scaling = 1.0f;
	m_Center = D3DXVECTOR3(0,0,0);
}

KModelGridRotate::~KModelGridRotate()
{

}

HRESULT KModelGridRotate::LoadMesh(LPSTR pFileName)
{
	g_cMeshTable.Get1NewXMesh(&m_lpMeshYaw);
	TCHAR Name[256];
	wsprintf(Name,"%s\\编辑器数据\\Rotation_X.Mesh",g_Def_AppDirectory);
	m_lpMeshYaw->LoadMesh    (Name);
	wsprintf(Name,"%s\\编辑器数据\\Rotation_X.Mtl",g_Def_AppDirectory);
	m_lpMeshYaw->LoadMaterial(Name);
	for(DWORD i=0;i<m_lpMeshYaw->m_dNumMaterial;i++)
	{
		m_lpMeshYaw->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;

		KModel::LPOPTION pNewOption = new KModel::OPTION;

		pNewOption->Type = OPTION_MATERIAL_ALPHABLEND;
		KModel::LPMATBLEND pMatBlend = new KModel::_MatBlend;

		pMatBlend->SrcBlend = D3DBLEND_SRCCOLOR;
		pMatBlend->DestBlend = D3DBLEND_ONE;

		pNewOption->pData = pMatBlend;	

		m_lpMeshYaw->m_lpMaterial[i].m_dwNumOptions = 1;
		m_lpMeshYaw->m_lpMaterial[i].m_lpMatOptions.push_back(pNewOption);
	}

	g_cMeshTable.Get1NewXMesh(&m_lpMeshPitch);
	wsprintf(Name,"%s\\编辑器数据\\Rotation_Y.Mesh",g_Def_AppDirectory);
	m_lpMeshPitch->LoadMesh    (Name);
	wsprintf(Name,"%s\\编辑器数据\\Rotation_Y.Mtl",g_Def_AppDirectory);
	m_lpMeshPitch->LoadMaterial(Name);
	for( i=0;i<m_lpMeshPitch->m_dNumMaterial;i++)
	{
		m_lpMeshPitch->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;

		KModel::LPOPTION pNewOption = new KModel::OPTION;

		pNewOption->Type = OPTION_MATERIAL_ALPHABLEND;
		KModel::LPMATBLEND pMatBlend = new KModel::_MatBlend;

		pMatBlend->SrcBlend = D3DBLEND_SRCCOLOR;
		pMatBlend->DestBlend = D3DBLEND_ONE;

		pNewOption->pData = pMatBlend;	

		m_lpMeshPitch->m_lpMaterial[i].m_dwNumOptions = 1;
		m_lpMeshPitch->m_lpMaterial[i].m_lpMatOptions.push_back(pNewOption);
	}

	g_cMeshTable.Get1NewXMesh(&m_lpMeshRoll);
	wsprintf(Name,"%s\\编辑器数据\\Rotation_Z.Mesh",g_Def_AppDirectory);
	m_lpMeshRoll->LoadMesh    (Name);
	wsprintf(Name,"%s\\编辑器数据\\Rotation_Z.Mtl",g_Def_AppDirectory);
	m_lpMeshRoll->LoadMaterial(Name);

	for( i=0;i<m_lpMeshRoll->m_dNumMaterial;i++)
	{
		m_lpMeshRoll->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;

		KModel::LPOPTION pNewOption = new KModel::OPTION;

		pNewOption->Type = OPTION_MATERIAL_ALPHABLEND;
		KModel::LPMATBLEND pMatBlend = new KModel::_MatBlend;

		pMatBlend->SrcBlend = D3DBLEND_SRCCOLOR;
		pMatBlend->DestBlend = D3DBLEND_ONE;

		pNewOption->pData = pMatBlend;	

		m_lpMeshRoll->m_lpMaterial[i].m_dwNumOptions = 1;
		m_lpMeshRoll->m_lpMaterial[i].m_lpMatOptions.push_back(pNewOption);
	}

	return S_OK;
}

HRESULT KModelGridRotate::Render()
{
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
	if (m_bSelectedYaw)
	{
		for(DWORD i=0;i<m_lpMeshYaw->m_dNumMaterial;i++)
		{
			m_lpMeshYaw->m_lpMaterial[i].m_sMaterial9.Ambient.r = 1.0f;
			m_lpMeshYaw->m_lpMaterial[i].m_sMaterial9.Ambient.g = 1.0f;
			m_lpMeshYaw->m_lpMaterial[i].m_sMaterial9.Ambient.b = 1.0f;
		}
	}
	else
	{
		for(DWORD i=0;i<m_lpMeshYaw->m_dNumMaterial;i++)
		{
			m_lpMeshYaw->m_lpMaterial[i].m_sMaterial9.Ambient.r = 
				m_lpMeshYaw->m_lpMaterial[i].m_sMaterial9.Diffuse.r*0.5f;
			m_lpMeshYaw->m_lpMaterial[i].m_sMaterial9.Ambient.g = 
				m_lpMeshYaw->m_lpMaterial[i].m_sMaterial9.Diffuse.g*0.5f;
			m_lpMeshYaw->m_lpMaterial[i].m_sMaterial9.Ambient.b = 
				m_lpMeshYaw->m_lpMaterial[i].m_sMaterial9.Diffuse.b*0.5f;
			m_lpMeshYaw->m_lpMaterial[i].m_sMaterial9.Power = 15;
		}
	}
	if (m_bSelectedPitch)
	{
		for(DWORD i=0;i<m_lpMeshPitch->m_dNumMaterial;i++)
		{
			m_lpMeshPitch->m_lpMaterial[i].m_sMaterial9.Ambient.r = 1.0f;
			m_lpMeshPitch->m_lpMaterial[i].m_sMaterial9.Ambient.g = 1.0f;
			m_lpMeshPitch->m_lpMaterial[i].m_sMaterial9.Ambient.b = 1.0f;
		}
	}
	else
	{
		for(DWORD i=0;i<m_lpMeshPitch->m_dNumMaterial;i++)
		{
			m_lpMeshPitch->m_lpMaterial[i].m_sMaterial9.Ambient.r = 
				m_lpMeshPitch->m_lpMaterial[i].m_sMaterial9.Diffuse.r*0.5f;
			m_lpMeshPitch->m_lpMaterial[i].m_sMaterial9.Ambient.g = 
				m_lpMeshPitch->m_lpMaterial[i].m_sMaterial9.Diffuse.g*0.5f;
			m_lpMeshPitch->m_lpMaterial[i].m_sMaterial9.Ambient.b = 
				m_lpMeshPitch->m_lpMaterial[i].m_sMaterial9.Diffuse.b*0.5f;
			m_lpMeshPitch->m_lpMaterial[i].m_sMaterial9.Power = 15;		}
	}
	if (m_bSelectedRoll)
	{
		for(DWORD i=0;i<m_lpMeshRoll->m_dNumMaterial;i++)
		{
			m_lpMeshRoll->m_lpMaterial[i].m_sMaterial9.Ambient.r = 1.0f;
			m_lpMeshRoll->m_lpMaterial[i].m_sMaterial9.Ambient.g = 1.0f;
			m_lpMeshRoll->m_lpMaterial[i].m_sMaterial9.Ambient.b = 1.0f;
		}
	}
	else
	{
		for(DWORD i=0;i<m_lpMeshRoll->m_dNumMaterial;i++)
		{
			m_lpMeshRoll->m_lpMaterial[i].m_sMaterial9.Ambient.r = 
				m_lpMeshRoll->m_lpMaterial[i].m_sMaterial9.Diffuse.r*0.5f;
			m_lpMeshRoll->m_lpMaterial[i].m_sMaterial9.Ambient.g = 
				m_lpMeshRoll->m_lpMaterial[i].m_sMaterial9.Diffuse.g*0.5f;
			m_lpMeshRoll->m_lpMaterial[i].m_sMaterial9.Ambient.b = 
				m_lpMeshRoll->m_lpMaterial[i].m_sMaterial9.Diffuse.b*0.5f;
			m_lpMeshRoll->m_lpMaterial[i].m_sMaterial9.Power = 15;		}
	}
	m_lpMeshYaw->Render();
	m_lpMeshPitch->Render();
	m_lpMeshRoll->Render();

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	return S_OK;
}

void KModelGridRotate::SetMatrix(float Scaling, D3DXVECTOR3 *pCenter)
{
	m_Scaling = Scaling;
	m_Center = *pCenter;

	D3DXMATRIX MatSc,Trans,Temp;

	D3DXMatrixScaling(&MatSc,m_Scaling,m_Scaling,m_Scaling);
	D3DXMatrixTranslation(&Trans,m_Center.x,m_Center.y,m_Center.z);
	
	D3DXMatrixMultiply(&Temp,&MatSc,&Trans);
	m_lpMeshYaw->m_Matrix = Temp;
	m_lpMeshPitch->m_Matrix = Temp;
	m_lpMeshRoll->m_Matrix = Temp;
}

HRESULT KModelGridRotate::SelectByPickRay(D3DXVECTOR3 *RayOrig, D3DXVECTOR3 *RayDir)
{
	D3DXVECTOR3 A,B,Intersect;
	A = *RayOrig;
	B = A + 1000 * *RayDir;

	float R1 = 95 * m_Scaling;
	float R2 = 85 * m_Scaling;
	float RR = 0;
	D3DXPLANE Plane;
	D3DXVECTOR3 Normal;
	
	Normal = D3DXVECTOR3(0,1,0);
	D3DXPlaneFromPointNormal(&Plane,&m_Center,&Normal);
	D3DXPlaneIntersectLine(&Intersect,&Plane,&A,&B);
	RR = D3DXVec3Length(&(Intersect-m_Center));
	if ((RR>=R2)&&(RR<=R1))
	{
		m_bSelectedYaw = TRUE;
	}
	else
	{
		m_bSelectedYaw = FALSE;
	}
	
	Normal = D3DXVECTOR3(1,0,0);
	D3DXPlaneFromPointNormal(&Plane,&m_Center,&Normal);
	D3DXPlaneIntersectLine(&Intersect,&Plane,&A,&B);
	RR = D3DXVec3Length(&(Intersect-m_Center));
	if ((RR>=R2)&&(RR<=R1))
	{
		m_bSelectedPitch = TRUE;
	}
	else
	{
		m_bSelectedPitch = FALSE;
	}

	Normal = D3DXVECTOR3(0,0,1);
	D3DXPlaneFromPointNormal(&Plane,&m_Center,&Normal);
	D3DXPlaneIntersectLine(&Intersect,&Plane,&A,&B);
	RR = D3DXVec3Length(&(Intersect-m_Center));
	if ((RR>=R2)&&(RR<=R1))
	{
		m_bSelectedRoll = TRUE;
	}
	else
	{
		m_bSelectedRoll = FALSE;
	}

	return S_OK;
}
