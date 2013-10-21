// MeshMoveGrid.cpp: implementation of the KModelGridMove class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "MeshMoveGrid.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KModelGridMove::KModelGridMove()
{
	m_lpMeshAxisX = NULL;
	m_lpMeshAxisY = NULL;
	m_lpMeshAxisZ = NULL;
	m_bSelectedX = FALSE;
	m_bSelectedY = FALSE;
	m_bSelectedZ = FALSE;

	m_lpMeshAxisXYZ = NULL;
	m_lpMeshAxisYZ = NULL;
	m_lpMeshAxisYX = NULL;
	m_lpMeshAxisXZ = NULL;

	m_bSelectedXYZ = FALSE;
	m_bSelectedYZ = FALSE;
	m_bSelectedYX = FALSE;
	m_bSelectedXZ = FALSE;
}

KModelGridMove::~KModelGridMove()
{

}

HRESULT KModelGridMove::LoadMesh(LPSTR pFileName)
{
	//TCHAR Name[256];
	//wsprintf(Name,"%s%s",g_Def_ModelDirectory,)

	g_cMeshTable.Get1NewXMesh(&m_lpMeshAxisX);
	
	TCHAR Name[256];
	wsprintf(Name,"%s\\编辑器数据\\Axis_X.Mesh",g_Def_AppDirectory);
	m_lpMeshAxisX->LoadMesh    (Name);

	wsprintf(Name,"%s\\编辑器数据\\Axis_X.Mtl",g_Def_AppDirectory);
	m_lpMeshAxisX->LoadMaterial(Name);

	for(DWORD i=0;i<m_lpMeshAxisX->m_dNumMaterial;i++)
	{
		m_lpMeshAxisX->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;

		KModel::LPOPTION pNewOption = new KModel::OPTION;
		
		pNewOption->Type = OPTION_MATERIAL_ALPHABLEND;
		KModel::LPMATBLEND pMatBlend = new KModel::_MatBlend;

		pMatBlend->SrcBlend = D3DBLEND_SRCCOLOR;
		pMatBlend->DestBlend = D3DBLEND_ONE;

		pNewOption->pData = pMatBlend;	
	
		m_lpMeshAxisX->m_lpMaterial[i].m_dwNumOptions = 1;
		m_lpMeshAxisX->m_lpMaterial[i].m_lpMatOptions.push_back(pNewOption);
	}

	g_cMeshTable.Get1NewXMesh(&m_lpMeshAxisY);
	wsprintf(Name,"%s\\编辑器数据\\Axis_Y.Mesh",g_Def_AppDirectory);
	m_lpMeshAxisY->LoadMesh    (Name);
	wsprintf(Name,"%s\\编辑器数据\\Axis_Y.Mtl",g_Def_AppDirectory);
	m_lpMeshAxisY->LoadMaterial(Name);
	for( i=0;i<m_lpMeshAxisX->m_dNumMaterial;i++)
	{
		m_lpMeshAxisY->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
		KModel::LPOPTION pNewOption = new KModel::OPTION;
		pNewOption->Type = OPTION_MATERIAL_ALPHABLEND;
		KModel::LPMATBLEND pMatBlend = new KModel::_MatBlend;

		pMatBlend->SrcBlend = D3DBLEND_SRCCOLOR;
		pMatBlend->DestBlend = D3DBLEND_ONE;

		pNewOption->pData = pMatBlend;	

		m_lpMeshAxisY->m_lpMaterial[i].m_dwNumOptions = 1;
		m_lpMeshAxisY->m_lpMaterial[i].m_lpMatOptions.push_back(pNewOption);
	}

	g_cMeshTable.Get1NewXMesh(&m_lpMeshAxisZ);
	wsprintf(Name,"%s\\编辑器数据\\Axis_Z.Mesh",g_Def_AppDirectory);
	m_lpMeshAxisZ->LoadMesh    (Name);
	wsprintf(Name,"%s\\编辑器数据\\Axis_Z.Mtl",g_Def_AppDirectory);
	m_lpMeshAxisZ->LoadMaterial(Name);
	for( i=0;i<m_lpMeshAxisX->m_dNumMaterial;i++)
	{
		m_lpMeshAxisZ->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
		KModel::LPOPTION pNewOption = new KModel::OPTION;
		pNewOption->Type = OPTION_MATERIAL_ALPHABLEND;
		KModel::LPMATBLEND pMatBlend = new KModel::_MatBlend;

		pMatBlend->SrcBlend = D3DBLEND_SRCCOLOR;
		pMatBlend->DestBlend = D3DBLEND_ONE;

		pNewOption->pData = pMatBlend;	

		m_lpMeshAxisZ->m_lpMaterial[i].m_dwNumOptions = 1;
		m_lpMeshAxisZ->m_lpMaterial[i].m_lpMatOptions.push_back(pNewOption);
	}

	//////////////////////////////////////////////////////////////////////////
	//XYZ
	g_cMeshTable.Get1NewXMesh(&m_lpMeshAxisXYZ);
	wsprintf(Name,"%s\\编辑器数据\\Axis_XYZ.Mesh",g_Def_AppDirectory);
	m_lpMeshAxisXYZ->LoadMesh    (Name);
	wsprintf(Name,"%s\\编辑器数据\\Axis_XYZ.Mtl",g_Def_AppDirectory);
	m_lpMeshAxisXYZ->LoadMaterial(Name);
	for( i=0;i<m_lpMeshAxisXYZ->m_dNumMaterial;i++)
	{
		m_lpMeshAxisXYZ->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
		KModel::LPOPTION pNewOption = new KModel::OPTION;
		pNewOption->Type = OPTION_MATERIAL_ALPHABLEND;
		KModel::LPMATBLEND pMatBlend = new KModel::_MatBlend;

		pMatBlend->SrcBlend = D3DBLEND_SRCCOLOR;
		pMatBlend->DestBlend = D3DBLEND_ONE;

		pNewOption->pData = pMatBlend;	

		m_lpMeshAxisXYZ->m_lpMaterial[i].m_dwNumOptions = 1;
		m_lpMeshAxisXYZ->m_lpMaterial[i].m_lpMatOptions.push_back(pNewOption);
	}	

	//////////////////////////////////////////////////////////////////////////
	//YZ
	g_cMeshTable.Get1NewXMesh(&m_lpMeshAxisYZ);
	wsprintf(Name,"%s\\编辑器数据\\Axis_Y_Z.Mesh",g_Def_AppDirectory);
	m_lpMeshAxisYZ->LoadMesh    (Name);
	wsprintf(Name,"%s\\编辑器数据\\Axis_Y_Z.Mtl",g_Def_AppDirectory);
	m_lpMeshAxisYZ->LoadMaterial(Name);
	for( i=0;i<m_lpMeshAxisYZ->m_dNumMaterial;i++)
	{
		m_lpMeshAxisYZ->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
		KModel::LPOPTION pNewOption = new KModel::OPTION;
		pNewOption->Type = OPTION_MATERIAL_ALPHABLEND;
		KModel::LPMATBLEND pMatBlend = new KModel::_MatBlend;

		pMatBlend->SrcBlend = D3DBLEND_SRCCOLOR;
		pMatBlend->DestBlend = D3DBLEND_ONE;

		pNewOption->pData = pMatBlend;	

		m_lpMeshAxisYZ->m_lpMaterial[i].m_dwNumOptions = 1;
		m_lpMeshAxisYZ->m_lpMaterial[i].m_lpMatOptions.push_back(pNewOption);
	}	
	//////////////////////////////////////////////////////////////////////////
	//YX
	g_cMeshTable.Get1NewXMesh(&m_lpMeshAxisYX);
	wsprintf(Name,"%s\\编辑器数据\\Axis_Y_X.Mesh",g_Def_AppDirectory);
	m_lpMeshAxisYX->LoadMesh    (Name);
	wsprintf(Name,"%s\\编辑器数据\\Axis_Y_X.Mtl",g_Def_AppDirectory);
	m_lpMeshAxisYX->LoadMaterial(Name);
	for( i=0;i<m_lpMeshAxisYX->m_dNumMaterial;i++)
	{
		m_lpMeshAxisYX->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
		KModel::LPOPTION pNewOption = new KModel::OPTION;
		pNewOption->Type = OPTION_MATERIAL_ALPHABLEND;
		KModel::LPMATBLEND pMatBlend = new KModel::_MatBlend;

		pMatBlend->SrcBlend = D3DBLEND_SRCCOLOR;
		pMatBlend->DestBlend = D3DBLEND_ONE;

		pNewOption->pData = pMatBlend;	

		m_lpMeshAxisYX->m_lpMaterial[i].m_dwNumOptions = 1;
		m_lpMeshAxisYX->m_lpMaterial[i].m_lpMatOptions.push_back(pNewOption);
	}	
	//////////////////////////////////////////////////////////////////////////
	//XZ
	g_cMeshTable.Get1NewXMesh(&m_lpMeshAxisXZ);
	wsprintf(Name,"%s\\编辑器数据\\Axis_XZ.Mesh",g_Def_AppDirectory);
	m_lpMeshAxisXZ->LoadMesh    (Name);
	wsprintf(Name,"%s\\编辑器数据\\Axis_XZ.Mtl",g_Def_AppDirectory);
	m_lpMeshAxisXZ->LoadMaterial(Name);
	for( i=0;i<m_lpMeshAxisXZ->m_dNumMaterial;i++)
	{
		m_lpMeshAxisXZ->m_lpMaterial[i].m_dOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
		KModel::LPOPTION pNewOption = new KModel::OPTION;
		pNewOption->Type = OPTION_MATERIAL_ALPHABLEND;
		KModel::LPMATBLEND pMatBlend = new KModel::_MatBlend;

		pMatBlend->SrcBlend = D3DBLEND_SRCCOLOR;
		pMatBlend->DestBlend = D3DBLEND_ONE;

		pNewOption->pData = pMatBlend;	

		m_lpMeshAxisXZ->m_lpMaterial[i].m_dwNumOptions = 1;
		m_lpMeshAxisXZ->m_lpMaterial[i].m_lpMatOptions.push_back(pNewOption);
	}	
	return S_OK;
}

HRESULT KModelGridMove::Render()
{
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
	if (m_bSelectedX)
	{
		for(DWORD i=0;i<m_lpMeshAxisX->m_dNumMaterial;i++)
		{
			m_lpMeshAxisX->m_lpMaterial[i].m_sMaterial9.Ambient.r = 1.0f;
			m_lpMeshAxisX->m_lpMaterial[i].m_sMaterial9.Ambient.g = 1.0f;
			m_lpMeshAxisX->m_lpMaterial[i].m_sMaterial9.Ambient.b = 1.0f;
		}
	}
	else
	{
		for(DWORD i=0;i<m_lpMeshAxisX->m_dNumMaterial;i++)
		{

			m_lpMeshAxisX->m_lpMaterial[i].m_sMaterial9.Ambient.r = 
				m_lpMeshAxisX->m_lpMaterial[i].m_sMaterial9.Diffuse.r *0.5f;
			m_lpMeshAxisX->m_lpMaterial[i].m_sMaterial9.Ambient.g = 
				m_lpMeshAxisX->m_lpMaterial[i].m_sMaterial9.Diffuse.g *0.5f;
			m_lpMeshAxisX->m_lpMaterial[i].m_sMaterial9.Ambient.b = 
				m_lpMeshAxisX->m_lpMaterial[i].m_sMaterial9.Diffuse.b *0.5f;

			m_lpMeshAxisX->m_lpMaterial[i].m_sMaterial9.Power = 15;
		}
	}
	m_lpMeshAxisX->Render();
	
	if (m_bSelectedY)
	{
		for(DWORD i=0;i<m_lpMeshAxisY->m_dNumMaterial;i++)
		{
			m_lpMeshAxisY->m_lpMaterial[i].m_sMaterial9.Ambient.r = 1.0f;
			m_lpMeshAxisY->m_lpMaterial[i].m_sMaterial9.Ambient.g = 1.0f;
			m_lpMeshAxisY->m_lpMaterial[i].m_sMaterial9.Ambient.b = 1.0f;
		}
	}
	else
	{
		for(DWORD i=0;i<m_lpMeshAxisY->m_dNumMaterial;i++)
		{
			m_lpMeshAxisY->m_lpMaterial[i].m_sMaterial9.Ambient.r = 
				m_lpMeshAxisY->m_lpMaterial[i].m_sMaterial9.Diffuse.r *0.5f;
			m_lpMeshAxisY->m_lpMaterial[i].m_sMaterial9.Ambient.g = 
				m_lpMeshAxisY->m_lpMaterial[i].m_sMaterial9.Diffuse.g *0.5f;
			m_lpMeshAxisY->m_lpMaterial[i].m_sMaterial9.Ambient.b = 
				m_lpMeshAxisY->m_lpMaterial[i].m_sMaterial9.Diffuse.b *0.5f;

			m_lpMeshAxisY->m_lpMaterial[i].m_sMaterial9.Power = 15;
		}
	}
	m_lpMeshAxisY->Render();


	if (m_bSelectedZ)
	{
		for(DWORD i=0;i<m_lpMeshAxisZ->m_dNumMaterial;i++)
		{
			m_lpMeshAxisZ->m_lpMaterial[i].m_sMaterial9.Ambient.r = 1.0f;
			m_lpMeshAxisZ->m_lpMaterial[i].m_sMaterial9.Ambient.g = 1.0f;
			m_lpMeshAxisZ->m_lpMaterial[i].m_sMaterial9.Ambient.b = 1.0f;
		}
	}
	else
	{
		for(DWORD i=0;i<m_lpMeshAxisZ->m_dNumMaterial;i++)
		{
			m_lpMeshAxisZ->m_lpMaterial[i].m_sMaterial9.Ambient.r = 
				m_lpMeshAxisZ->m_lpMaterial[i].m_sMaterial9.Diffuse.r *0.5f;
			m_lpMeshAxisZ->m_lpMaterial[i].m_sMaterial9.Ambient.g = 
				m_lpMeshAxisZ->m_lpMaterial[i].m_sMaterial9.Diffuse.g *0.5f;
			m_lpMeshAxisZ->m_lpMaterial[i].m_sMaterial9.Ambient.b = 
				m_lpMeshAxisZ->m_lpMaterial[i].m_sMaterial9.Diffuse.b *0.5f;
			m_lpMeshAxisZ->m_lpMaterial[i].m_sMaterial9.Power = 15;
		}
	}
	m_lpMeshAxisZ->Render();

	//////////////////////////////////////////////////////////////////////////
	//xyz
	if (m_bSelectedXYZ)
	{
		for(DWORD i=0;i<m_lpMeshAxisXYZ->m_dNumMaterial;i++)
		{
			m_lpMeshAxisXYZ->m_lpMaterial[i].m_sMaterial9.Ambient.r = 1.0f;
			m_lpMeshAxisXYZ->m_lpMaterial[i].m_sMaterial9.Ambient.g = 1.0f;
			m_lpMeshAxisXYZ->m_lpMaterial[i].m_sMaterial9.Ambient.b = 1.0f;
		}
	}
	else
	{
		for(DWORD i=0;i<m_lpMeshAxisXYZ->m_dNumMaterial;i++)
		{
			m_lpMeshAxisXYZ->m_lpMaterial[i].m_sMaterial9.Ambient.r = 
				m_lpMeshAxisXYZ->m_lpMaterial[i].m_sMaterial9.Diffuse.r *0.5f;
			m_lpMeshAxisXYZ->m_lpMaterial[i].m_sMaterial9.Ambient.g = 
				m_lpMeshAxisXYZ->m_lpMaterial[i].m_sMaterial9.Diffuse.g *0.5f;
			m_lpMeshAxisXYZ->m_lpMaterial[i].m_sMaterial9.Ambient.b = 
				m_lpMeshAxisXYZ->m_lpMaterial[i].m_sMaterial9.Diffuse.b *0.5f;
			m_lpMeshAxisXYZ->m_lpMaterial[i].m_sMaterial9.Power = 15;
		}
	}
	SAFE_RENDER(m_lpMeshAxisXYZ);
	//////////////////////////////////////////////////////////////////////////
	//yz
	if (m_bSelectedYZ)
	{
		for(DWORD i=0;i<m_lpMeshAxisYZ->m_dNumMaterial;i++)
		{
			m_lpMeshAxisYZ->m_lpMaterial[i].m_sMaterial9.Ambient.r = 1.0f;
			m_lpMeshAxisYZ->m_lpMaterial[i].m_sMaterial9.Ambient.g = 1.0f;
			m_lpMeshAxisYZ->m_lpMaterial[i].m_sMaterial9.Ambient.b = 1.0f;
		}
	}
	else
	{
		for(DWORD i=0;i<m_lpMeshAxisYZ->m_dNumMaterial;i++)
		{
			m_lpMeshAxisYZ->m_lpMaterial[i].m_sMaterial9.Ambient.r = 
				m_lpMeshAxisYZ->m_lpMaterial[i].m_sMaterial9.Diffuse.r *0.5f;
			m_lpMeshAxisYZ->m_lpMaterial[i].m_sMaterial9.Ambient.g = 
				m_lpMeshAxisYZ->m_lpMaterial[i].m_sMaterial9.Diffuse.g *0.5f;
			m_lpMeshAxisYZ->m_lpMaterial[i].m_sMaterial9.Ambient.b = 
				m_lpMeshAxisYZ->m_lpMaterial[i].m_sMaterial9.Diffuse.b *0.5f;
			m_lpMeshAxisYZ->m_lpMaterial[i].m_sMaterial9.Power = 15;
		}
	}
	SAFE_RENDER(m_lpMeshAxisYZ);
	//////////////////////////////////////////////////////////////////////////
	//yx
	if (m_bSelectedYX)
	{
		for(DWORD i=0;i<m_lpMeshAxisYX->m_dNumMaterial;i++)
		{
			m_lpMeshAxisYX->m_lpMaterial[i].m_sMaterial9.Ambient.r = 1.0f;
			m_lpMeshAxisYX->m_lpMaterial[i].m_sMaterial9.Ambient.g = 1.0f;
			m_lpMeshAxisYX->m_lpMaterial[i].m_sMaterial9.Ambient.b = 1.0f;
		}
	}
	else
	{
		for(DWORD i=0;i<m_lpMeshAxisYX->m_dNumMaterial;i++)
		{
			m_lpMeshAxisYX->m_lpMaterial[i].m_sMaterial9.Ambient.r = 
				m_lpMeshAxisYX->m_lpMaterial[i].m_sMaterial9.Diffuse.r *0.5f;
			m_lpMeshAxisYX->m_lpMaterial[i].m_sMaterial9.Ambient.g = 
				m_lpMeshAxisYX->m_lpMaterial[i].m_sMaterial9.Diffuse.g *0.5f;
			m_lpMeshAxisYX->m_lpMaterial[i].m_sMaterial9.Ambient.b = 
				m_lpMeshAxisYX->m_lpMaterial[i].m_sMaterial9.Diffuse.b *0.5f;
			m_lpMeshAxisYX->m_lpMaterial[i].m_sMaterial9.Power = 15;
		}
	}
	SAFE_RENDER(m_lpMeshAxisYX);
	//////////////////////////////////////////////////////////////////////////
	//xz
	if (m_bSelectedXZ)
	{
		for(DWORD i=0;i<m_lpMeshAxisXZ->m_dNumMaterial;i++)
		{
			m_lpMeshAxisXZ->m_lpMaterial[i].m_sMaterial9.Ambient.r = 1.0f;
			m_lpMeshAxisXZ->m_lpMaterial[i].m_sMaterial9.Ambient.g = 1.0f;
			m_lpMeshAxisXZ->m_lpMaterial[i].m_sMaterial9.Ambient.b = 1.0f;
		}
	}
	else
	{
		for(DWORD i=0;i<m_lpMeshAxisXZ->m_dNumMaterial;i++)
		{
			m_lpMeshAxisXZ->m_lpMaterial[i].m_sMaterial9.Ambient.r = 
				m_lpMeshAxisXZ->m_lpMaterial[i].m_sMaterial9.Diffuse.r *0.5f;
			m_lpMeshAxisXZ->m_lpMaterial[i].m_sMaterial9.Ambient.g = 
				m_lpMeshAxisXZ->m_lpMaterial[i].m_sMaterial9.Diffuse.g *0.5f;
			m_lpMeshAxisXZ->m_lpMaterial[i].m_sMaterial9.Ambient.b = 
				m_lpMeshAxisXZ->m_lpMaterial[i].m_sMaterial9.Diffuse.b *0.5f;
			m_lpMeshAxisXZ->m_lpMaterial[i].m_sMaterial9.Power = 15;
		}
	}
	SAFE_RENDER(m_lpMeshAxisXZ);
	
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	return S_OK;
}

void KModelGridMove::SetMatrix(D3DXMATRIX *pMat)
{
	m_lpMeshAxisX->m_Matrix = *pMat;
	m_lpMeshAxisY->m_Matrix = *pMat;
	m_lpMeshAxisZ->m_Matrix = *pMat;

	m_lpMeshAxisXYZ->m_Matrix = *pMat;
	m_lpMeshAxisYZ->m_Matrix = *pMat;
	m_lpMeshAxisYX->m_Matrix = *pMat;
	m_lpMeshAxisXZ->m_Matrix = *pMat;

}

HRESULT KModelGridMove::SelectByPickRay(D3DXVECTOR3 *RayOrig, D3DXVECTOR3 *RayDir)
{
	D3DXVECTOR3 A,B,Intersect;
	A = *RayOrig;
	B = A + 10000 * *RayDir;

	D3DXVECTOR3 BoxPos[8];
	GraphicsStruct::_BoundingBox BBox;
	
	BBox.PositionA = m_lpMeshAxisX->m_BBox_A;
	BBox.PositionB = m_lpMeshAxisX->m_BBox_B;
	BoundingBox_TransformCoord(BoxPos,&BBox,&m_lpMeshAxisX->m_Matrix);
	if (IsLineIntersectBox(&Intersect,&A,&B,BoxPos))
	{
		m_bSelectedX = TRUE;
	}
	else
	{
		m_bSelectedX = FALSE;
	}

	BBox.PositionA = m_lpMeshAxisY->m_BBox_A;
	BBox.PositionB = m_lpMeshAxisY->m_BBox_B;
	BoundingBox_TransformCoord(BoxPos,&BBox,&m_lpMeshAxisY->m_Matrix);
	if (IsLineIntersectBox(&Intersect,&A,&B,BoxPos))
	{
		m_bSelectedY = TRUE;
	}
	else
	{
		m_bSelectedY = FALSE;
	}

	BBox.PositionA = m_lpMeshAxisZ->m_BBox_A;
	BBox.PositionB = m_lpMeshAxisZ->m_BBox_B;
	BoundingBox_TransformCoord(BoxPos,&BBox,&m_lpMeshAxisZ->m_Matrix);
	if (IsLineIntersectBox(&Intersect,&A,&B,BoxPos))
	{
		m_bSelectedZ = TRUE;
	}
	else
	{
		m_bSelectedZ = FALSE;
	}

	//////////////////////////////////////////////////////////////////////////
	//xyz
	BBox.PositionA = m_lpMeshAxisXYZ->m_BBox_A;
	BBox.PositionB = m_lpMeshAxisXYZ->m_BBox_B;
	BoundingBox_TransformCoord(BoxPos,&BBox,&m_lpMeshAxisXYZ->m_Matrix);
	if (IsLineIntersectBox(&Intersect,&A,&B,BoxPos))
	{
		m_bSelectedXYZ = TRUE;
	}
	else
	{
		m_bSelectedXYZ = FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	//yz
	BBox.PositionA = m_lpMeshAxisYZ->m_BBox_A;
	BBox.PositionB = m_lpMeshAxisYZ->m_BBox_B;
	BoundingBox_TransformCoord(BoxPos,&BBox,&m_lpMeshAxisYZ->m_Matrix);
	if (IsLineIntersectBox(&Intersect,&A,&B,BoxPos))
	{
		m_bSelectedYZ = TRUE;
	}
	else
	{
		m_bSelectedYZ = FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	//yx
	BBox.PositionA = m_lpMeshAxisYX->m_BBox_A;
	BBox.PositionB = m_lpMeshAxisYX->m_BBox_B;
	BoundingBox_TransformCoord(BoxPos,&BBox,&m_lpMeshAxisYX->m_Matrix);
	if (IsLineIntersectBox(&Intersect,&A,&B,BoxPos))
	{
		m_bSelectedYX = TRUE;
	}
	else
	{
		m_bSelectedYX = FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	//xz
	BBox.PositionA = m_lpMeshAxisXZ->m_BBox_A;
	BBox.PositionB = m_lpMeshAxisXZ->m_BBox_B;
	BoundingBox_TransformCoord(BoxPos,&BBox,&m_lpMeshAxisXZ->m_Matrix);
	if (IsLineIntersectBox(&Intersect,&A,&B,BoxPos))
	{
		m_bSelectedXZ = TRUE;
	}
	else
	{
		m_bSelectedXZ = FALSE;
	}
	return S_OK;
}
