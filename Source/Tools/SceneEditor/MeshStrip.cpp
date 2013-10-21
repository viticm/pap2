// MeshStrip.cpp: implementation of the KModelStrip class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "MeshStrip.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KModelStrip::KModelStrip()
{
	m_bBind = FALSE;
	
	m_NumNode = 51;
	m_lpStripNode = new NODE[m_NumNode];
	m_fNodeLength = 20;

	for(DWORD i=0;i<m_NumNode;i++)
	{
		m_lpStripNode[i].Position = D3DXVECTOR3(0,300,i*1.0f*m_fNodeLength);
		m_lpStripNode[i].Plane    = D3DXVECTOR3(1,0,0);
		m_lpStripNode[i].Speed    = D3DXVECTOR3(0,0,0);
		m_lpStripNode[i].YPR      = D3DXVECTOR3(0,0,0);
		m_lpStripNode[i].R_YPR    = D3DXVECTOR3(0,0,0);

	}
}

KModelStrip::~KModelStrip()
{

}

HRESULT KModelStrip::LoadMesh(LPSTR pFileName)
{
	HRESULT hr = S_OK;
	
	DWORD m_dNumPloy     = m_NumNode-1;
	DWORD m_dNumFaces    = m_dNumPloy * 2;
	DWORD m_dNumVertices = (m_dNumPloy+1) * 2; //(m_dwRegionX+1)*(m_dwNumRegionHeightBaseCell+1);
	DWORD m_dNumSubsets  = 1;
		
	SAFE_RELEASE(m_pMeshSys);
	if (FAILED(hr = D3DXCreateMeshFVF(m_dNumFaces,m_dNumVertices,D3DXMESH_MANAGED,
		D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1,g_pd3dDevice,&m_pMeshSys)))
	{
		return hr;
	}

	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	WORD* pIndex = NULL;
	DWORD * pAttrib = NULL;
	if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
		return E_FAIL;
	if (FAILED(m_pMeshSys->LockIndexBuffer (0,(void**)&pIndex)))
		return E_FAIL;
	if (FAILED(m_pMeshSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
		return E_FAIL;

	//vertices
	for(DWORD i=0;i<=m_dNumPloy;i++)
	{
		pVers[i*2  ].p = D3DXVECTOR3(-20,300,i*1.0f*m_fNodeLength);
		pVers[i*2  ].Normal = D3DXVECTOR3(0,1,0);
		pVers[i*2  ].tv1 = 0;
		pVers[i*2  ].tu1 = i*1.0f/m_dNumPloy;

		pVers[i*2+1].p = D3DXVECTOR3( 20,300,i*50.0f);
		pVers[i*2+1].Normal = D3DXVECTOR3(0,1,0);
		pVers[i*2+1].tv1 = 1;
		pVers[i*2+1].tu1 = i*1.0f/m_dNumPloy;
	}
	//faces index
	for(i=0;i<m_dNumPloy;i++)
	{
		pIndex[i*6  ] = (WORD)i*2  ;
		pIndex[i*6+1] = (WORD)i*2+1;
		pIndex[i*6+2] = (WORD)i*2+2;
		pIndex[i*6+3] = (WORD)i*2+2;
		pIndex[i*6+4] = (WORD)i*2+1;
		pIndex[i*6+5] = (WORD)i*2+3;

		pAttrib[i*2  ] = 0;
		pAttrib[i*2+1] = 0;
	}

	D3DXComputeBoundingBox((D3DXVECTOR3*) pVers,m_dNumVertices,sizeof(VFormat::FACES_NORMAL_TEXTURE1),
		&m_BBox_A,&m_BBox_B);

	if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
		return E_FAIL;
	if (FAILED(m_pMeshSys->UnlockIndexBuffer()))
		return E_FAIL;
	if (FAILED(m_pMeshSys->UnlockAttributeBuffer()))
		return E_FAIL;
	
	if (m_lpMaterial==NULL)
	{
		m_dNumMaterial = m_dNumSubsets;
		m_lpMaterial = new MATERIAL[m_dNumMaterial];
		ZeroMemory(m_lpMaterial,sizeof(MATERIAL)*m_dNumMaterial);
		for(i=0;i<m_dNumMaterial;i++)
		{
			m_lpMaterial[i].m_sMaterial9.Diffuse.r = 0.9f ;
			m_lpMaterial[i].m_sMaterial9.Diffuse.g = 0.9f ;
			m_lpMaterial[i].m_sMaterial9.Diffuse.b = 0.9f ;
			m_lpMaterial[i].m_sMaterial9.Diffuse.a = 1.0f ;
			m_lpMaterial[i].m_sMaterial9.Ambient  = m_lpMaterial[i].m_sMaterial9.Diffuse;
			m_lpMaterial[i].m_sMaterial9.Specular = m_lpMaterial[i].m_sMaterial9.Diffuse;
			m_lpMaterial[i].m_sMaterial9.Power = 50;
		}
		
		LoadMaterial("D:\\3dsmax5\\meshes\\Strip.Mtl");
	}
	
	D3DXComputeNormals(m_pMeshSys,NULL);

	/////////////////////////////////////////////////////////////////////////////////////
	//load shader for test
	//LoadShader();
	//KModel::OptimizeUnSkinedMesh();

	return S_OK;
}

HRESULT KModelStrip::UpdateStrip()
{
	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	if (FAILED(m_pMeshSys->LockVertexBuffer(0,(void**)&pVers)))
		return E_FAIL;
	
	float Time = timeGetTime()*0.001f;
	D3DXVECTOR3 WindForce = D3DXVECTOR3(cosf(Time),1,sinf(Time));
	D3DXVec3Normalize(&WindForce,&WindForce);

	
	float fWindEf = sinf(timeGetTime()*0.0001f)*sinf(timeGetTime()*0.0001f);

	for(DWORD i=0;i<m_NumNode;i++)
	{
		/*float WindSin = m_fNodeLength*sinf(fWindEf);
		float WindCos = m_fNodeLength*cosf(fWindEf);*/

		float K = sinf(timeGetTime()*-0.01f+i);
		/*m_lpStripNode[i].R_Position = D3DXVECTOR3(0,K*i*WindSin*0.05f,WindSin) +
			D3DXVECTOR3(0,-WindCos,0);*/
		m_lpStripNode[i].R_Position = m_fNodeLength * fWindEf* WindForce +
			m_fNodeLength*(1-fWindEf)*D3DXVECTOR3(0,-1,0);
		m_lpStripNode[i].R_YPR.z = K *0.31416f;
		if (i>0)
		{
			D3DXVECTOR3 NewPos = m_lpStripNode[i-1].Position + m_lpStripNode[i].R_Position;
			m_lpStripNode[i].Position += (NewPos-m_lpStripNode[i].Position)*0.1f;
	
			D3DXVECTOR3 NewYPR = m_lpStripNode[i-1].YPR + m_lpStripNode[i].R_YPR;
			m_lpStripNode[i].YPR += (NewYPR-m_lpStripNode[i].YPR)*0.1f;
			if (m_lpStripNode[i].Position.y<0.2f)
			{
				m_lpStripNode[i].Position.y = 0.2f;
				m_lpStripNode[i].YPR = D3DXVECTOR3(0,0,0);
			}
		}
		else
		{
			if (m_bBind)
			{
				int SocketIndex = m_pBindMesh->FindSocket(m_pBindSocketName);
				if (SocketIndex>=0)
				{
					LPSOCKET pSocket= &m_pBindMesh->m_lpSockets[SocketIndex];
					D3DXVECTOR3 Pos = D3DXVECTOR3(0,0,0);
					D3DXVec3TransformCoord(&Pos,&Pos,&pSocket->CurMatrix);
					m_lpStripNode[i].Position = Pos;
				}
			}
		}
		D3DXMATRIX MatYPR;
		D3DXMatrixRotationYawPitchRoll(&MatYPR,m_lpStripNode[i].YPR.x,m_lpStripNode[i].YPR.y,m_lpStripNode[i].YPR.z);
		D3DXVec3TransformCoord(&m_lpStripNode[i].Plane,&D3DXVECTOR3(1,0,0),&MatYPR);
		
		D3DXVECTOR3 A,B;
		
		A = m_lpStripNode[i].Position + m_lpStripNode[i].Plane * 10;
		B = m_lpStripNode[i].Position - m_lpStripNode[i].Plane * 10;

		pVers[i*2  ].p = A;
		pVers[i*2  ].Normal = m_lpStripNode[i].Plane;

		pVers[i*2+1].p = B;
		pVers[i*2+1].Normal = m_lpStripNode[i].Plane;
	}
	if (FAILED(m_pMeshSys->UnlockVertexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT KModelStrip::Render()
{
	UpdateStrip();
	KModel::Render();
	PlaneShadowRender();
	return S_OK;
}

HRESULT KModelStrip::PlaneShadowRender()
{
	D3DXVECTOR3 vecDir = D3DXVECTOR3(100*sinf(timeGetTime()*0.001f),
		                 100,
		                 100*cosf(timeGetTime()*0.001f) );

	D3DXVec3Normalize(&vecDir, &vecDir );

	D3DXVECTOR4 Light;
	Light.x = vecDir.x;
	Light.y = vecDir.y;
	Light.z = vecDir.z;
	Light.w = 0;
	D3DXVec4Normalize(&Light,&Light);
	
	D3DXPLANE Plane;
	D3DXPlaneFromPointNormal(&Plane,&D3DXVECTOR3(0,0.1f,0),&D3DXVECTOR3(0,1,0));
	
	D3DXMATRIX matShadow,matsave;
	D3DXMatrixShadow(&matShadow,&Light,&Plane);
	
	D3DMATERIAL9 Mtl,MtlSave;
	ZeroMemory(&Mtl,sizeof(D3DMATERIAL9));
	Mtl.Diffuse.a = 1.0f;

	g_pd3dDevice->GetMaterial(&MtlSave);
	g_pd3dDevice->GetTransform( D3DTS_WORLD , &matsave);
	
	g_pd3dDevice->SetTransform( D3DTS_WORLD , &matShadow);
	g_pd3dDevice->SetMaterial(&Mtl);
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_NONE);

	for(DWORD i=0;i<m_dNumMaterial;i++)
	{
		m_pMeshSys->DrawSubset(i);
	}
	
	g_pd3dDevice->SetTransform( D3DTS_WORLD , &matsave);
	g_pd3dDevice->SetMaterial(&MtlSave);

	return S_OK;
}

HRESULT KModelStrip::Bind(LPMODEL pXMesh, LPSTR SocketName)
{
	if (pXMesh==NULL)
		return E_FAIL;
	if (pXMesh==this)
		return E_FAIL;
	m_pBindMesh = pXMesh;
	if (m_pBindSocketName==NULL)
		m_pBindSocketName = new TCHAR[30];
	wsprintf(m_pBindSocketName,"%s",SocketName);
	m_bBind = TRUE;
	return S_OK;
}

HRESULT KModelStrip::UnBind()
{
	KModel::UnBind();
	m_bBind = FALSE;
	return S_OK;
}
