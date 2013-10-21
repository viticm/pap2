// DataObject.cpp: implementation of the KComponentObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DataObject.h"

#include "SceneEditor.h"
#include "MeshTerran.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KComponentObject::KComponentObject()
{
	m_dwScrabbleOption = 0;

	m_fBrushSize = TERRIANCELLWIDTH;
	m_PickLine = D3DXVECTOR3(0,0,0);

	m_nStart_X = 0;
	m_nStart_Z = 0;
	m_nWidth  = 0;
	m_nHeight = 0;

	m_dwType = 0;
	ID = 0;
	D3DXMatrixIdentity(&m_Matrix);

	m_Position = D3DXVECTOR3(0,0,0);
	m_Rotation = D3DXVECTOR3(0,0,0);
	m_Scaling  = D3DXVECTOR3(1,1,1);

	m_PositionBack = D3DXVECTOR3(0,0,0);
	m_RotationBack = D3DXVECTOR3(0,0,0);
	m_ScalingBack  = D3DXVECTOR3(1,1,1);

	m_lpCellInformation = NULL;
	//M_lpGridInformation = NULL;
	m_lpMeshGridInfo    = NULL;

	m_bHeightDownNeedUpdate = FALSE;
	m_lpHeightDown = NULL;

	m_dwGroupIndex = 0;
	m_dwIndexInGroup = 0;
}

KComponentObject::~KComponentObject()
{

}

PropertyBase::KPropertyBase* KComponentObject::Acquire()
{
	return NULL;
}

void KComponentObject::Initialize(PVOID pObjectTable)
{
	;
}

HRESULT KComponentObject::Copy(KComponentObject** pNewObject)
{
	KComponentObject* pObject = NULL;
	g_cObjectTable.Get1NewObject(&pObject,0);

	for(DWORD i=0;i<m_vecMeshKeeper.size();i++)
	{
		DWORD NewMeshID = 0;
		if(SUCCEEDED(g_cMeshTable.GetCopy(&NewMeshID,m_vecMeshKeeper[i].m_dMeshID)))
		{
			pObject->AddMesh(NewMeshID);
			pObject->m_vecMeshKeeper[i].m_Matrix = m_vecMeshKeeper[i].m_Matrix;
		}
	}

	pObject->m_Position = m_Position;
	pObject->m_Rotation = m_Rotation;
	pObject->m_Scaling = m_Scaling;

	pObject->ComputeBBox();

	pObject->m_FileName = m_FileName;

	(*pNewObject) = pObject;

	return S_OK;
}

HRESULT KComponentObject::BackupData()
{
	m_PositionBack = m_Position;
	m_RotationBack = m_Rotation;
	m_ScalingBack  = m_Scaling;

	return S_OK;
}
ModelUI::FRECT KComponentObject::GetXZRect()
{
	D3DXVECTOR3 BoxPos[8];
	m_BBox.TransformCoord(BoxPos,&m_Matrix);
	
	AABBOX BoxNew;
	BoxNew.AddPosition(BoxPos,8);
	return BoxNew.ToRect();
}

HRESULT KComponentObject::FrameMove()
{

	D3DXMATRIX MatScale,MatRot;
	D3DXMatrixScaling(&MatScale,m_Scaling.x,m_Scaling.y,m_Scaling.z);
	D3DXMatrixRotationYawPitchRoll(&MatRot,m_Rotation.x,m_Rotation.y,m_Rotation.z);
	D3DXMatrixMultiply(&m_Matrix,&MatScale,&MatRot);
	m_Matrix._41 = m_Position.x;
	m_Matrix._42 = m_Position.y;
	m_Matrix._43 = m_Position.z;

	for(DWORD i=0;i<m_vecMeshKeeper.size();i++)
	{
		DWORD XMeshID = m_vecMeshKeeper[i].m_dMeshID;
		LPMODEL pMesh = NULL;
		if (SUCCEEDED(g_cMeshTable.GetXMesh(&pMesh,XMeshID)))
		{
			pMesh->FrameMove();
		}
	}
	return S_OK;
}
HRESULT KComponentObject::Render()
{	

	for(DWORD i=0;i<m_vecMeshKeeper.size();i++)
	{
		DWORD XMeshID = m_vecMeshKeeper[i].m_dMeshID;
		LPMODEL pMesh = NULL;
		if (SUCCEEDED(g_cMeshTable.GetXMesh(&pMesh,XMeshID)))
		{
			D3DXMATRIX MatSave,MatCur;
			g_pd3dDevice->GetTransform(D3DTS_WORLD,&MatSave);
			D3DXMatrixMultiply(&MatCur,&m_vecMeshKeeper[i].m_Matrix,&m_Matrix);
			g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatCur);
			pMesh->Render();
			//pMesh->DrawSnap();
			//pMesh->SaveSnap("D:\\001.jpg");
			g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatSave);
		}
	}
	return S_OK;
}

HRESULT KComponentObject::RenderWithEffect(DWORD Option,PVOID pData)
{
	for(DWORD i=0;i<m_vecMeshKeeper.size();i++)
	{
		DWORD XMeshID = m_vecMeshKeeper[i].m_dMeshID;
		LPMODEL pMesh = NULL;
		
		if (SUCCEEDED(g_cMeshTable.GetXMesh(&pMesh,XMeshID)))
		{
			D3DXMATRIX MatSave,MatCur;
			g_pd3dDevice->GetTransform(D3DTS_WORLD,&MatSave);
			D3DXMatrixMultiply(&MatCur,&m_vecMeshKeeper[i].m_Matrix,&m_Matrix);
			g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatCur);

			pMesh->RenderWithEffect(Option,pData,&m_Matrix);

			g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatSave);
		}
	}

	return S_OK;
}

HRESULT KComponentObject::RenderShadow(DWORD ShadowType)
{	

	D3DXMATRIX MatScale,MatRot;
	D3DXMatrixScaling(&MatScale,m_Scaling.x,m_Scaling.y,m_Scaling.z);
	D3DXMatrixRotationYawPitchRoll(&MatRot,m_Rotation.x,m_Rotation.y,m_Rotation.z);
	D3DXMatrixMultiply(&m_Matrix,&MatScale,&MatRot);
	m_Matrix._41 = m_Position.x;
	m_Matrix._42 = m_Position.y;
	m_Matrix._43 = m_Position.z;

	for(DWORD i=0;i<m_vecMeshKeeper.size();i++)
	{
		DWORD XMeshID = m_vecMeshKeeper[i].m_dMeshID;
		LPMODEL pMesh = NULL;
		if (SUCCEEDED(g_cMeshTable.GetXMesh(&pMesh,XMeshID)))
		{
			D3DXMATRIX MatSave,MatCur;
			g_pd3dDevice->GetTransform(D3DTS_WORLD,&MatSave);
			D3DXMatrixMultiply(&MatCur,&m_vecMeshKeeper[i].m_Matrix,&m_Matrix);
			g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatCur);
			pMesh->m_dwShadow = ShadowType;
			pMesh->m_fPlanarShadowHeight = m_Matrix._42 + m_BBox.A.y;

			pMesh->RenderShadow();
			//pMesh->DrawSnap();
			//pMesh->SaveSnap("D:\\001.jpg");
			g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatSave);
		}
	}
	return S_OK;
}

HRESULT KComponentObject::LoadFromX(LPSTR pFileName)
{
	LPMODEL pMesh = NULL;
	if (FAILED(g_cMeshTable.Get1NewXMesh(&pMesh)))
		return E_FAIL;
	if (FAILED(pMesh->LoadFromX(pFileName)))
		return E_FAIL;
	AddMesh(pMesh->ID); 
	return S_OK;
}

HRESULT KComponentObject::Get1NewXMeshKeeper(LPMESHKEEPER * pMeshKeeper)
{
	_MeshKeeper NewKeeper;
	m_vecMeshKeeper.push_back(NewKeeper);

	DWORD Index = m_vecMeshKeeper.size() -1;

	(*pMeshKeeper) = &m_vecMeshKeeper[Index];
	return S_OK;
}

HRESULT KComponentObject::GetMeshKeeper(LPMESHKEEPER *pMeshKeeper,DWORD ID)
{
	(*pMeshKeeper) = &m_vecMeshKeeper[ ID -1 ];
	return S_OK;
}

HRESULT KComponentObject::AddMesh(DWORD MeshID)
{
	LPMESHKEEPER pKeeper = NULL;
	if (FAILED(Get1NewXMeshKeeper(&pKeeper)))
		return E_FAIL;
	pKeeper->m_dMeshID = MeshID;
	ComputeBBox();
	return S_OK;
}

HRESULT KComponentObject::DeleteMesh(DWORD Index)
{
	int S =0;
	vector<_MeshKeeper>::iterator i = m_vecMeshKeeper.begin();
	while(i!=m_vecMeshKeeper.end())
	{
		if(S==Index)
		{
			m_vecMeshKeeper.erase(i);
			break;
		}
		i++;
		S++;
	}

	ComputeBBox();
	return S_OK;
}

HRESULT KComponentObject::LoadByIni(IIniFile* pIniFile,LPSTR pFileName)
{
	return S_OK;
}

HRESULT KComponentObject::LoadMesh(LPSTR pFileName)
{
	LPMODEL pMesh = NULL;
	if (FAILED(g_cMeshTable.Get1NewXMesh(&pMesh)))
		return E_FAIL;
						
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];
	TCHAR Name[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );
	wsprintf(Name,"%s%s%s.Mtl",drive,dir,fname);
	
	if (FAILED(pMesh->LoadMesh(pFileName)))
		return E_FAIL;
	AddMesh(pMesh->ID); 
	pMesh->LoadMaterial(Name);
	return S_OK;
}

HRESULT KComponentObject::LoadMaterial(LPSTR pFileName)
{
	LPMODEL pMesh = NULL;
	if (FAILED(g_cMeshTable.GetXMesh(&pMesh,1)))
		return E_FAIL;
	if (FAILED(pMesh->LoadMaterial(pFileName)))
		return E_FAIL;
	return S_OK;
}

HRESULT KComponentObject::GetMesh(LPMODEL *ppXMesh, DWORD ID)
{
	LPMESHKEEPER pKeeper = NULL;
	if (FAILED(GetMeshKeeper(&pKeeper,ID)))
		return E_FAIL;
	return g_cMeshTable.GetXMesh(ppXMesh,pKeeper->m_dMeshID);
}

HRESULT KComponentObject::CleanUp()
{
	m_vecMeshKeeper.clear();
	
	m_nHeight = 0;
	m_nWidth  = 0;
	SAFE_DELETE_ARRAY(m_lpCellInformation);
	SAFE_RELEASE(m_lpMeshGridInfo);

	SAFE_DELETE(m_lpHeightDown);
	return S_OK;
}


HRESULT KComponentObject::ComputeBBox()
{
	m_BBox.Clear();

	for(DWORD i=0;i<m_vecMeshKeeper.size();i++)
	{
		DWORD XMeshID = m_vecMeshKeeper[i].m_dMeshID;
		LPMODEL pMesh = NULL;
		if (SUCCEEDED(g_cMeshTable.GetXMesh(&pMesh,XMeshID)))
		{
			m_BBox.AddPosition(pMesh->m_BBox_A,
				pMesh->m_BBox_B,
				&m_vecMeshKeeper[i].m_Matrix);
		}
	}
	return S_OK;
}

HRESULT KComponentObject::DrawBBox()
{
	D3DXMATRIX MatSave;
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&MatSave);
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&m_Matrix);
	g_cGraphicsTool.DrawAABBox(&m_BBox.A,&m_BBox.B,0xFFFF00FF);
	g_pd3dDevice->SetTransform(D3DTS_WORLD,&MatSave);
	return S_OK;
}

HRESULT KComponentObject::Edit_Move(D3DXVECTOR3* pPos)
{
	if(m_dwScrabbleOption&SCRABBLE_SANP2GRID)
	{
		m_Position.x = (int) ((*pPos).x / TERRIANCELLWIDTH) *  TERRIANCELLWIDTH;
		m_Position.z = (int) ((*pPos).z / TERRIANCELLWIDTH) *  TERRIANCELLWIDTH;
		m_Position.y = (*pPos).y;
	}
	else
	{
		m_Position = (*pPos);
	}
	
	return S_OK;
}

HRESULT KComponentObject::Edit_Rotate(D3DXVECTOR3 *pRot)
{
	m_Rotation = *pRot;
	return S_OK;
}

HRESULT KComponentObject::Edit_Scaling(D3DXVECTOR3 *pScal)
{
	m_Scaling = *pScal;
	return S_OK;
}

HRESULT KComponentObject::CreateCellInformation()
{
	m_nStart_X = (int)(m_BBox.A.x / TERRIANCELLWIDTH -0.5f);
	m_nStart_Z = (int)(m_BBox.A.z / TERRIANCELLWIDTH -0.5f);
	m_nWidth   = (int)(m_BBox.GetLength() / TERRIANCELLWIDTH + 0.5f);
	m_nHeight  = (int)(m_BBox.GetWidth() / TERRIANCELLWIDTH + 0.5f);

	SAFE_DELETE_ARRAY(m_lpCellInformation);
	//SAFE_DELETE_ARRAY(M_lpGridInformation);
	SAFE_RELEASE(m_lpMeshGridInfo);

	m_lpCellInformation = new _CellInfo[m_nWidth*m_nHeight];
	//M_lpGridInformation = new _GridInfo[(m_nWidth+1)*(m_nHeight+1)];'
	D3DXVECTOR2 LeftUp(m_BBox.A.x , m_BBox.A.z);

	if(!m_lpHeightDown)
	{
		m_lpHeightDown = new KHeightMapBase;
	}

	m_lpHeightDown->CleanUp();
	m_lpHeightDown->Create(LeftUp,m_nWidth,m_nHeight,TRUE,TERRIANCELLWIDTH,m_BBox.A.y);

	return S_OK;
}

HRESULT KComponentObject::RenderCellInfoHeightDown()
{

	if((m_nWidth==0)||(m_nHeight==0))
		return S_OK;

	if((!m_lpMeshGridInfo)||(m_bHeightDownNeedUpdate))
	{
		UpdataCellInfoHeightDown();
		m_bHeightDownNeedUpdate = FALSE;
	}

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetTexture(0,NULL);

	m_lpMeshGridInfo->DrawSubset(0);

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE,D3DFILL_SOLID);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	if(m_lpHeightDown)
	{
		D3DXVECTOR3 Up;
		D3DXVECTOR2 Pos(m_PickLine.x,m_PickLine.z);
		if(m_lpHeightDown->GetPosition(&Up,Pos))
		{
			TCHAR str[256];
			sprintf(str,"%d",(int)Up.y);
			g_cGraphicsTool.DrawText(100,400,1,1,0xFFFFFFFF,16,"宋体",str,0);
	
			g_cGraphicsTool.DrawLine(&m_PickLine,&Up,0xFFFF0000,0xFFFF0000);

		}
		else
		{
			g_cGraphicsTool.DrawLine(&m_PickLine,&(m_PickLine+D3DXVECTOR3(0,300,0)),0xFFFF0000,0xFFFF0000);
		
		}
	}
	else
	{
		g_cGraphicsTool.DrawLine(&m_PickLine,&(m_PickLine+D3DXVECTOR3(0,300,0)),0xFFFF0000,0xFFFF0000);
	}

	{
		D3DXVECTOR3 A,B,D;
		A = m_PickLine + D3DXVECTOR3(-m_fBrushSize,5,-m_fBrushSize);
		B = A + D3DXVECTOR3(2*m_fBrushSize,0,0);
		D = A + D3DXVECTOR3(0,0,2*m_fBrushSize);

		g_cGraphicsTool.DrawRect(&A,&B,&D,0xFF008000,0);
	}
	
	return S_OK;
}

HRESULT KComponentObject::UpdataCellInfoHeightDown()
{
	HRESULT hr = S_OK;

	if((m_nWidth==0)||(m_nHeight==0))
		return S_OK;

	DWORD m_dNumFaces = m_nWidth * m_nHeight * 2;
	DWORD m_dNumVertices = (m_nWidth+1) * (m_nHeight+1);

	if(!m_lpMeshGridInfo)
	{
		if (FAILED(hr = D3DXCreateMeshFVF(m_dNumFaces,
			m_dNumVertices,
			D3DXMESH_MANAGED,
			D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1,
			g_pd3dDevice,
			&m_lpMeshGridInfo)))
		{
			return hr;
		}

		WORD* pIndex = NULL;
		DWORD * pAttrib = NULL;

		if (FAILED(m_lpMeshGridInfo->LockIndexBuffer (0,(void**)&pIndex)))
			return E_FAIL;
		if (FAILED(m_lpMeshGridInfo->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
			return E_FAIL;

		int Weight = m_nHeight+1;
		for(int X=0;X<m_nWidth;X++)
		{
			for(int Y=0;Y<m_nHeight;Y++)
			{
				int PloyIndex = X * m_nHeight + Y;

				DWORD Vertex_A = X    *Weight+ Y;
				DWORD Vertex_B = (X+1)*Weight+ Y;
				DWORD Vertex_C = (X+1)*Weight+(Y+1);
				DWORD Vertex_D = X    *Weight+(Y+1);

				DWORD Faces_A1 = (PloyIndex*2)*3;
				DWORD Faces_B1 = Faces_A1 + 1;
				DWORD Faces_C1 = Faces_B1 + 1;

				pIndex[Faces_B1] = (WORD)Vertex_A;
				pIndex[Faces_A1] = (WORD)Vertex_B;
				pIndex[Faces_C1] = (WORD)Vertex_D;
				pAttrib[PloyIndex*2] = 0;

				DWORD Faces_A2 = (PloyIndex*2+1)*3;
				DWORD Faces_B2 = Faces_A2 + 1;
				DWORD Faces_C2 = Faces_B2 + 1;

				pIndex[Faces_B2] = (WORD)Vertex_D;
				pIndex[Faces_A2] = (WORD)Vertex_B;
				pIndex[Faces_C2] = (WORD)Vertex_C;
				pAttrib[PloyIndex*2+1] = 0;
			}
		}

		m_lpMeshGridInfo->UnlockIndexBuffer();
		m_lpMeshGridInfo->UnlockAttributeBuffer();
	}

	VFormat::FACES_DIFFUSE_TEXTURE1 * pVers = NULL;
	if (FAILED(m_lpMeshGridInfo->LockVertexBuffer(0,(void**)&pVers)))
		return E_FAIL;

	for(int X=0;X<=m_nWidth;X++)
	{
		for(int Z=0;Z<=m_nHeight;Z++)
		{
			int Index = X * (m_nHeight+1) + Z;
			pVers[Index].p.x = (m_nStart_X+X)*TERRIANCELLWIDTH;
			pVers[Index].p.z = (m_nStart_Z+Z)*TERRIANCELLWIDTH;

			if(m_lpHeightDown)
			{
				D3DXVECTOR3 Pos = m_lpHeightDown->GetVertexPosion(X,Z);
				pVers[Index].p.y = Pos.y;
			}
			else
			{
				pVers[Index].p.y = 0;
			}


			pVers[Index].diffuse = 0xFFFFFFFF;
			pVers[Index].tu1 = X*0.1f;
			pVers[Index].tv1 = Z*0.1f;
		}
	}

	m_lpMeshGridInfo->UnlockVertexBuffer();

	return S_OK;
}

HRESULT KComponentObject::BuildCellInfoHeightDown()
{
	if((m_nWidth==0)||(m_nHeight==0))
		return S_OK;
	if(!m_lpCellInformation)
		return S_OK;

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Direction(0,1,0);


	for(int X=0;X<=m_nWidth;X++)
	{
		for(int Z=0;Z<=m_nHeight;Z++)
		{
			int Index = X * (m_nHeight+1) + Z;
			Position.x = (m_nStart_X+X)*TERRIANCELLWIDTH;
			Position.z = (m_nStart_Z+Z)*TERRIANCELLWIDTH;
			Position.y = m_BBox.A.y - 100;
			
			float MaxDis = 0;
			BOOL  bHitOneMesh = FALSE;

			for(DWORD i=0;i<m_vecMeshKeeper.size();i++)
			{
				LPMODEL pModel = NULL;

				if(SUCCEEDED(this->GetMesh(&pModel,i+1)))
				{
					BOOL  Hit   = FALSE;
					float Dis   = 0;
					DWORD Count =0;

					LPD3DXBUFFER pAllHit = NULL;

					if(SUCCEEDED(D3DXIntersect(pModel->m_pMeshSys,
						&Position,&Direction,
						&Hit,NULL,NULL,NULL,
						&Dis,&pAllHit,&Count)))
					{
						if(Hit)
						{
							bHitOneMesh = Hit;

							LPD3DXINTERSECTINFO pHitInfo
								= (LPD3DXINTERSECTINFO) pAllHit->GetBufferPointer();
							for(DWORD K=0;K<Count;K++)
							{
								if(pHitInfo[K].Dist>MaxDis)
								{
									MaxDis = pHitInfo[K].Dist;
								}				
							}

						}
					}
					SAFE_RELEASE(pAllHit);
				}
			}

			if(bHitOneMesh)
			{
				D3DXVECTOR3 Pos(0,m_BBox.A.y - 100 + MaxDis,0);
				m_lpHeightDown->SetVertexPosition(Pos,X,Z);
			}
			else
			{
				D3DXVECTOR3 Pos(0,m_BBox.A.y,0);
				m_lpHeightDown->SetVertexPosition(Pos,X,Z);
			}

		}
	}

	UpdataCellInfoHeightDown();
	return S_OK;
}

HRESULT KComponentObject::RenderCellInfoHeightUp()
{
	if((m_nWidth==0)||(m_nHeight==0))
		return S_OK;
	if(!m_lpCellInformation)
		return S_OK;



	return S_OK;
}

HRESULT KComponentObject::RenderCellInfoBarrier(DWORD dwBarrier)
{
	if((m_nWidth==0)||(m_nHeight==0))
		return S_OK;
	if(!m_lpCellInformation)
		return S_OK;

	D3DXVECTOR3 A,B,D;

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	BOOL m_bWalk   = dwBarrier & BARRIER_CAN_WALK;
	BOOL m_bJump   = dwBarrier & BARRIER_CAN_JUMP;
	BOOL m_bBullet = dwBarrier & BARRIER_BULLET_PASS;
	BOOL m_bPlace  = dwBarrier & BARRIER_OBJECT_PLACE;

	{
		A.x = (m_nStart_X)*TERRIANCELLWIDTH;
		A.z = (m_nStart_Z)*TERRIANCELLWIDTH;
		A.y = m_BBox.A.y - 5;

		B = A + D3DXVECTOR3(TERRIANCELLWIDTH*m_nWidth,0,0);
		D = A + D3DXVECTOR3(0,0,TERRIANCELLWIDTH*m_nHeight);

		g_cGraphicsTool.DrawRect(&A,&B,&D,0x60808040,0);

	}

	for(int X=0;X<m_nWidth;X++)
	{
		for(int Z=0;Z<m_nHeight;Z++)
		{
			int Index = X * (m_nHeight) + Z;

			//////////////////////////////////////////////////////////////////////////
			if(m_bWalk)
			{
				if (!(m_lpCellInformation[Index].Barrier&BARRIER_CAN_WALK))
				{
					A.x = (m_nStart_X+X)*TERRIANCELLWIDTH;
					A.z = (m_nStart_Z+Z)*TERRIANCELLWIDTH;
					A.y = m_BBox.A.y;

					B = A + D3DXVECTOR3(TERRIANCELLWIDTH,0,0);
					D = A + D3DXVECTOR3(0,0,TERRIANCELLWIDTH);

					g_cGraphicsTool.DrawRect(&A,&B,&D,0x800000FF,0);
				}
			}

			if(m_bJump)
			{
				if (!(m_lpCellInformation[Index].Barrier&BARRIER_CAN_JUMP))
				{
					A.x = (m_nStart_X+X)*TERRIANCELLWIDTH;
					A.z = (m_nStart_Z+Z)*TERRIANCELLWIDTH;
					A.y = m_BBox.A.y+2;
					;

					B = A + D3DXVECTOR3(TERRIANCELLWIDTH,0,0);
					D = A + D3DXVECTOR3(0,0,TERRIANCELLWIDTH);

					g_cGraphicsTool.DrawRect(&A,&B,&D,0x8000FF00,0);
				}
			}
			
			if(m_bBullet)
			{
				if (!(m_lpCellInformation[Index].Barrier&BARRIER_BULLET_PASS))
				{
					A.x = (m_nStart_X+X)*TERRIANCELLWIDTH;
					A.z = (m_nStart_Z+Z)*TERRIANCELLWIDTH;
					A.y = m_BBox.A.y+4;
					;

					B = A + D3DXVECTOR3(TERRIANCELLWIDTH,0,0);
					D = A + D3DXVECTOR3(0,0,TERRIANCELLWIDTH);

					g_cGraphicsTool.DrawRect(&A,&B,&D,0x80FF0000,0);
				}
			}

			if(m_bPlace)
			{
				if (!(m_lpCellInformation[Index].Barrier&BARRIER_OBJECT_PLACE))
				{
					A.x = (m_nStart_X+X)*TERRIANCELLWIDTH;
					A.z = (m_nStart_Z+Z)*TERRIANCELLWIDTH;
					A.y = m_BBox.A.y+6;
					;

					B = A + D3DXVECTOR3(TERRIANCELLWIDTH,0,0);
					D = A + D3DXVECTOR3(0,0,TERRIANCELLWIDTH);

					g_cGraphicsTool.DrawRect(&A,&B,&D,0x80FFFF00,0);
				}
			}
			
			//////////////////////////////////////////////////////////////////////////
		}
	}

	{
		g_cGraphicsTool.DrawLine(&m_PickLine,&(m_PickLine+D3DXVECTOR3(0,300,0)),0xFFFF0000,0xFFFF0000);
		int XX = (int)(m_PickLine.x / TERRIANCELLWIDTH);
		int YY = (int)(m_PickLine.z / TERRIANCELLWIDTH);
		int X,Y;
		for(X=XX-3;X<XX+3;X++)
		{
			for(Y=YY-3;Y<YY+3;Y++)
			{
				if ((X*TERRIANCELLWIDTH<=m_PickLine.x)&&((X+1)*TERRIANCELLWIDTH>=m_PickLine.x)&&
					(Y*TERRIANCELLWIDTH<=m_PickLine.z)&&((Y+1)*TERRIANCELLWIDTH>=m_PickLine.z))
				{
					D3DXVECTOR3 A,B,D;

					A.x = X*TERRIANCELLWIDTH;
					A.z = Y*TERRIANCELLWIDTH;
					A.y = m_PickLine.y + 2;
					;

					B = A + D3DXVECTOR3(TERRIANCELLWIDTH,0,0);
					D = A + D3DXVECTOR3(0,0,TERRIANCELLWIDTH);

					g_cGraphicsTool.DrawRect(&A,&B,&D,0xFFFF00FF,0);
					break;
				}
			}
		}
	}

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE,TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

	return S_OK;
}

HRESULT KComponentObject::SetPickLine(D3DXVECTOR3* pOrg,D3DXVECTOR3* pDir)
{
	D3DXPLANE  WaterLevel;
	D3DXPlaneFromPointNormal(&WaterLevel,&m_BBox.A,&D3DXVECTOR3(0,1,0));
	D3DXVECTOR3 Inter;
	D3DXVECTOR3 B = *pOrg + *pDir;

	if(D3DXPlaneIntersectLine(&Inter,&WaterLevel,pOrg,&B))
	{
		m_PickLine = Inter;
	}
	
	return S_OK;
}

HRESULT KComponentObject::SetCellBarrier(DWORD Operator,DWORD dwBarrier)
{
	if((m_nWidth==0)||(m_nHeight==0))
		return S_OK;
	if(!m_lpCellInformation)
		return S_OK;

	int XX = (int)(m_PickLine.x / TERRIANCELLWIDTH);
	int YY = (int)(m_PickLine.z / TERRIANCELLWIDTH);
	int X,Y;

	for(X=XX-3;X<XX+3;X++)
	{
		for(Y=YY-3;Y<YY+3;Y++)
		{
			if ((X*TERRIANCELLWIDTH<=m_PickLine.x)&&((X+1)*TERRIANCELLWIDTH>=m_PickLine.x)&&
				(Y*TERRIANCELLWIDTH<=m_PickLine.z)&&((Y+1)*TERRIANCELLWIDTH>=m_PickLine.z))
			{	
				if((X>=m_nStart_X)&&(X<m_nStart_X+m_nWidth)&&
				   (Y>=m_nStart_Z)&&(Y<m_nStart_Z+m_nHeight))
				{
					int Index = (X-m_nStart_X) * (m_nHeight) + (Y-m_nStart_Z);

					switch(Operator)
					{
					case BARRIER_OPERATOR_ADD:
						{
							m_lpCellInformation[Index].Barrier &= ~dwBarrier;
							break;
						}
					case BARRIER_OPERATOR_REMOVE:
						{
							m_lpCellInformation[Index].Barrier |= dwBarrier;
							break;
						}
					case BARRIER_OPERATOR_SET:
						{
							m_lpCellInformation[Index].Barrier = ~dwBarrier;
							break;
						}
					}
				}
				break;
			}
		}
	}
	return S_OK;
}

HRESULT KComponentObject::SetGridHeightDown(DWORD Operator,float Value)
{
	if((m_nWidth==0)||(m_nHeight==0))
		return S_OK;
	if(!m_lpCellInformation)
		return S_OK;

	D3DXVECTOR2 Center(m_PickLine.x ,m_PickLine.z);
	ModelUI::FRECT Rect(Center,m_fBrushSize*0.9f);

	switch(Operator)
	{
	case HEIGHTDOWN_UP:
		{
			m_lpHeightDown->ModifyHeight(Rect,0,GROUND_UP,Value,0.1F);
			break;
		}
	case HEIGHTDOWN_DOWN:
		{
			m_lpHeightDown->ModifyHeight(Rect,0,GROUND_DOWN,Value,0.1F);
			break;
		}
	case HEIGHTDOWN_SET:
		{
			m_lpHeightDown->ModifyHeight(Rect,0,GROUND_SET,Value,0.1F);
			break;
		}
	}

	m_bHeightDownNeedUpdate = TRUE;


	return S_OK;
}

BOOL KComponentObject::SelectByPickLine(float* Distance,D3DXVECTOR3* pOrg,D3DXVECTOR3* pDir)
{
	D3DXVECTOR3 A = *pOrg;
	D3DXVECTOR3 B = A + (*pDir)*100000;
	D3DXVECTOR3 Inter;

	D3DXVECTOR3 BoxPos[8];
	m_BBox.TransformCoord(BoxPos,&m_Matrix);
	if(::IsLineIntersectBox(&Inter,&A,&B,BoxPos))
	{
		D3DXVECTOR3 Len = A - Inter;
		float Length = D3DXVec3Length(&Len);

		float MinDis = 100000;
		BOOL  bHitOneMesh = FALSE;

		if(Length<MinDis)
		{
			(*Distance) = Length;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
		/*for(DWORD i=0;i<m_vecMeshKeeper.size();i++)
		{
			LPMODEL pModel = NULL;

			//////////////////////////////////////////////////////////////////////////
			if(SUCCEEDED(this->GetMesh(&pModel,i+1)))
			{
				BOOL  Hit   = FALSE;
				float Dis   = 0;
				DWORD Count = 0;

				LPD3DXBUFFER pAllHit = NULL;

				//////////////////////////////////////////////////////////////////////////
				D3DXMATRIX  Mat = m_Matrix;
				//D3DXMatrixMultiply(&Mat,&m_Matrix,&m_vecMeshKeeper[i].m_Matrix);
				D3DXVECTOR3 Scale,Position;
				D3DXMATRIX  Rot,MScale,RotX;
				::MatrixExract(Scale,Position,Rot,&Mat);

				D3DXMatrixInverse(&Rot,NULL,&Rot);
				//D3DXMatrixScaling(&MScale,1/Scale.x,1/Scale.y,1/Scale.z);
				D3DXMatrixRotationYawPitchRoll(&RotX,-m_Rotation.x,-m_Rotation.y,-m_Rotation.z);

				D3DXMatrixMultiply(&Mat,&MScale,&RotX);
				Mat._41 = - Position.x;
				Mat._42 = - Position.y;
				Mat._43 = - Position.z;

				D3DXVECTOR3 A_New = A;
				D3DXVECTOR3 B_New = B;
				D3DXVec3TransformCoord(&A_New,&A_New,&Mat);
				D3DXVec3TransformCoord(&B_New,&B_New,&Mat);
				D3DXVECTOR3 Orig_New = B - A;

				if(SUCCEEDED(D3DXIntersect(pModel->m_pMeshSys,
					&A_New,&Orig_New,
					&Hit,NULL,NULL,NULL,
					&Dis,&pAllHit,&Count)))
				{
					if(Hit)
					{
						bHitOneMesh = Hit;

						LPD3DXINTERSECTINFO pHitInfo
							= (LPD3DXINTERSECTINFO) pAllHit->GetBufferPointer();
						for(DWORD K=0;K<Count;K++)
						{
							if(pHitInfo[K].Dist<MinDis)
							{
								MinDis = pHitInfo[K].Dist;
							}				
						}

					}
				}
				SAFE_RELEASE(pAllHit);
			}
		}

		//
		if(MinDis<100000)
		{
			(*Distance) = MinDis;
			return TRUE;
		}
		else
		{
			return FALSE;
		}*/

	}
	return FALSE;
}

HRESULT KComponentObject::Save2File(LPSTR pFileName)
{
	FILE* pFile = fopen(pFileName,"wb");
	if(!pFile)
	{
		return E_FAIL;
	}

	_BuildingObjectHeader Header;
	int K = 0;

	DWORD   BlockEnd  = 0xFFFFFFFF;  
	DWORD   Pos_Start = ftell(pFile);

	Header.nStart_X = m_nStart_X;
	Header.nStart_Z = m_nStart_Z;
	Header.nWidth   = m_nWidth;
	Header.nHeight  = m_nHeight;

	K = fwrite(&Header,sizeof(_BuildingObjectHeader),1,pFile);
	//////////////////////////////////////////////////////////////////////////
	if((m_nWidth>0)&&(m_nHeight>0))
	{
		if(m_lpHeightDown)
		{
			Header.BlockHeightMapDown = ftell(pFile) - Pos_Start;
			
			DWORD Version = 0;
			K = fwrite(&Version,sizeof(DWORD),1,pFile);

			m_lpHeightDown->Save2File(pFile);
			
			K = fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
		}
	}

	if((m_nWidth>0)&&(m_nHeight>0))
	{
		if(m_lpCellInformation)
		{
			Header.BlockBarrier = ftell(pFile) - Pos_Start;

			DWORD Version = 0;
			K = fwrite(&Version,sizeof(DWORD),1,pFile);
			
			K = fwrite(m_lpCellInformation,sizeof(_CellInfo),m_nWidth*m_nHeight,pFile);

			K = fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
		}
	}

	{
		Header.BlockScrabble = ftell(pFile) - Pos_Start;
		DWORD Version = 0;
		K = fwrite(&Version,sizeof(DWORD),1,pFile);

		fwrite(&m_dwScrabbleOption,sizeof(DWORD),1,pFile);

		K = fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}
	
	if(m_vecMeshKeeper.size()>0)
	{
		Header.BlockMeshes = ftell(pFile) - Pos_Start;
		DWORD Version = 0;
		K = fwrite(&Version,sizeof(DWORD),1,pFile);
		DWORD Count = m_vecMeshKeeper.size();
		
		K = fwrite(&Count,sizeof(DWORD),1,pFile);
		
		for(DWORD i=0;i<m_vecMeshKeeper.size();i++)
		{
			_MeshKeeper* pKeep = &m_vecMeshKeeper[i];
			DWORD MeshType = 0;
			TCHAR MeshFile[256];
			TCHAR MtlFile [256];
			TCHAR AniFile [256];		
			DWORD Option1 = 0;
			DWORD Option2 = 0;
			DWORD Option3 = 0;
			DWORD Option4 = 0;

			LPMODEL pModel = NULL;
			if(SUCCEEDED(GetMesh(&pModel,i+1)))
			{
				MeshType = pModel->m_dwType;
				wsprintf(MeshFile,"%s",pModel->m_szMeshFileName);
				wsprintf(MtlFile ,"%s",pModel->m_szMtlFileName);
				wsprintf(AniFile ,"%s",pModel->m_szAniFileName);
			}
			else
			{
				MeshType = BlockEnd;
				wsprintf(MeshFile,"");
				wsprintf(MtlFile ,"");
				wsprintf(AniFile ,"");
			}

			fwrite(&MeshType,sizeof(DWORD),1  ,pFile);
			fwrite( MeshFile,sizeof(TCHAR),256,pFile);
			fwrite( MtlFile ,sizeof(TCHAR),256,pFile);
			fwrite( AniFile ,sizeof(TCHAR),256,pFile);

			fwrite(&pKeep->m_Matrix,sizeof(D3DMATRIX),1,pFile);

			fwrite(&Option1,sizeof(DWORD),1  ,pFile);
			fwrite(&Option2,sizeof(DWORD),1  ,pFile);
			fwrite(&Option3,sizeof(DWORD),1  ,pFile);
			fwrite(&Option4,sizeof(DWORD),1  ,pFile);
		}

		fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	}
	
	fwrite(&BlockEnd,sizeof(DWORD),1,pFile);
	//////////////////////////////////////////////////////////////////////////
	Header.BlockLength = ftell(pFile) - Pos_Start;
	fseek(pFile,Pos_Start,SEEK_SET);
	K = fwrite(&Header,sizeof(_BuildingObjectHeader),1,pFile);


	fclose(pFile);
	return S_OK;
}

HRESULT KComponentObject::LoadFromFile(LPSTR pFileName)
{
	FILE* pFile = fopen(pFileName,"rb");
	if(!pFile)
	{
		return E_FAIL;
	}
	DWORD   BlockEnd  = 0xFFFFFFFF;  
	DWORD   Pos_Start = ftell(pFile);
	int     K         = 0;

	_BuildingObjectHeader Header;
	K = fread(&Header,sizeof(_BuildingObjectHeader),1,pFile);

	if(!K)
	{
		fclose(pFile);
		return E_FAIL;
	}

	if(Header.FileMark!=0x4F6C7A2B)
	{
		fclose(pFile);
		return E_FAIL;
	}


	m_nStart_X = Header.nStart_X;
	m_nStart_Z = Header.nStart_Z;
	m_nWidth   = Header.nWidth  ;
	m_nHeight  = Header.nHeight ;

	if(Header.BlockBarrier)
	{
		fseek(pFile,Pos_Start+Header.BlockBarrier,SEEK_SET);
		DWORD Version = 0;
		K = fread(&Version,sizeof(DWORD),1,pFile);

		if(m_lpCellInformation)
		{
			SAFE_DELETE_ARRAY(m_lpCellInformation);
		}
		m_lpCellInformation = new _CellInfo[m_nHeight*m_nWidth];

		K = fread(m_lpCellInformation,sizeof(_CellInfo),m_nWidth*m_nHeight,pFile);

		K = fread(&BlockEnd,sizeof(DWORD),1,pFile);

	}
	if(Header.BlockHeightMapDown)
	{
		fseek(pFile,Pos_Start+Header.BlockHeightMapDown,SEEK_SET);
		DWORD Version = 0;
		K = fread(&Version,sizeof(DWORD),1,pFile);

		if(!m_lpHeightDown)
		{
			m_lpHeightDown = new KHeightMapBase;
		}
		else
		{
			m_lpHeightDown->CleanUp();
		}

		m_lpHeightDown->LoadFromFile(pFile);

		K = fread(&BlockEnd,sizeof(DWORD),1,pFile);

	}

	if(Header.BlockScrabble)
	{
		fseek(pFile,Pos_Start+Header.BlockScrabble,SEEK_SET);

		Header.BlockScrabble = ftell(pFile) - Pos_Start;
		DWORD Version = 0;
		K = fread(&Version,sizeof(DWORD),1,pFile);

		fread(&m_dwScrabbleOption,sizeof(DWORD),1,pFile);

		K = fread(&BlockEnd,sizeof(DWORD),1,pFile);
	}

	if(Header.BlockMeshes)
	{
		fseek(pFile,Pos_Start+Header.BlockMeshes,SEEK_SET);

		DWORD Version = 0;
		DWORD Count   = 0;
		K = fread(&Version,sizeof(DWORD),1,pFile);
		K = fread(&Count  ,sizeof(DWORD),1,pFile);

		for(DWORD i=0;i<Count;i++)
		{
			DWORD MeshType = 0;
			TCHAR MeshFile[256];
			TCHAR MtlFile [256];
			TCHAR AniFile [256];		
			DWORD Option1 = 0;
			DWORD Option2 = 0;
			DWORD Option3 = 0;
			DWORD Option4 = 0;
			D3DMATRIX Mat;

			fread(&MeshType,sizeof(DWORD),1  ,pFile);
			fread( MeshFile,sizeof(TCHAR),256,pFile);
			fread( MtlFile ,sizeof(TCHAR),256,pFile);
			fread( AniFile ,sizeof(TCHAR),256,pFile);

			fread(&Mat,sizeof(D3DMATRIX),1,pFile);

			fread(&Option1,sizeof(DWORD),1  ,pFile);
			fread(&Option2,sizeof(DWORD),1  ,pFile);
			fread(&Option3,sizeof(DWORD),1  ,pFile);
			fread(&Option4,sizeof(DWORD),1  ,pFile);

			LPMODEL pModel = NULL;

			DWORD MeshID;

			TCHAR NameFind[256];
			if(SUCCEEDED(g_cFileFinder.FindFile(NameFind,MeshFile)))
			{
				if(SUCCEEDED(g_cMeshTable.LoadFromFile(&MeshID,NameFind)))
				{
					g_cMeshTable.GetXMesh(&pModel,MeshID);

					if(_strcmpi(MtlFile,"")!=0)
					{
						g_cFileFinder.FindFile(NameFind,MtlFile);
						pModel->LoadMaterial(NameFind);
					}

					if(_strcmpi(AniFile,"")!=0)
					{
						g_cFileFinder.FindFile(NameFind,AniFile);
						pModel->LoadAnimation(NameFind);
					}


					AddMesh(MeshID);

					_MeshKeeper* pKeeper = &m_vecMeshKeeper[m_vecMeshKeeper.size()-1];
					pKeeper->m_Matrix = Mat;
				}
			}
			else
			{
				MessageBox(NULL,MeshFile,"找不到该文件",0);
			}
			
		}
	}

	fclose(pFile);

	m_FileName = pFileName;
	ComputeBBox();

	return S_OK;
}

KHeightMapBase::KHeightMapBase()
{
	nDataSize = 0;
	nWidth  = 16;
	nHeight = 16;
	
	m_nDataType = HEIGHTMAPDATATYPE_FLOATHEIGHTONLY;

	pData   = NULL;
	pDataBack   = NULL;
	fEffect = 1.0f;
	CellWidth = 50.0f;
	m_LeftUp = D3DXVECTOR2(0,0);
}

KHeightMapBase::~KHeightMapBase()
{
	SAFE_DELETE_ARRAY(pData);
	SAFE_DELETE_ARRAY(pDataBack);
}

HRESULT KHeightMapBase::CleanUp()
{
	SAFE_DELETE_ARRAY(pData);
	SAFE_DELETE_ARRAY(pDataBack);
	nDataSize = 0;
	nWidth  = 16;
	nHeight = 16;
	
m_nDataType   = HEIGHTMAPDATATYPE_FLOATHEIGHTONLY;

	pData   = NULL;
	pDataBack   = NULL;
	fEffect = 1.0f;
	CellWidth = 50.0f;
	m_LeftUp = D3DXVECTOR2(0,0);

	return S_OK;
}

float KHeightMapBase::GetVertexHeight(int nX,int nZ)
{
	if((nX>nWidth)||(nZ>nWidth))
	{
		return 0;
	}

	float  X = m_LeftUp.x + nX * CellWidth;
	float  Z = m_LeftUp.y + nZ * CellWidth;

	switch(m_nDataType)
	{
	case HEIGHTMAPDATATYPE_FLOATHEIGHTONLY:
		{
			float* pVertex = (float*) pData;
			float  Height = pVertex[nZ*(nWidth+1)+nX];

			return Height;
			break;
		}
	case HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL:
		{
			D3DXVECTOR4* pVertex = (D3DXVECTOR4*) pData;
			D3DXVECTOR4  Height = pVertex[nZ*(nWidth+1)+nX];
			return Height.w;
			break;
		}
	}
	return 0;
}

float KHeightMapBase::GetVertexHeight(const int Index)
{
	assert(Index < 33 * 33);
	
	D3DXVECTOR4* pVertex = (D3DXVECTOR4*) pData;
	D3DXVECTOR4  Height = pVertex[Index];
	return Height.w;
}

D3DXVECTOR3 KHeightMapBase::GetVertexPosion(int nX,int nZ)
{
	if((nX>nWidth)||(nZ>nWidth))
	{
		float  X = m_LeftUp.x + nX * CellWidth;
		float  Z = m_LeftUp.y + nZ * CellWidth;
		return D3DXVECTOR3(X,0,Z);
	}

	float  X = m_LeftUp.x + nX * CellWidth;
	float  Z = m_LeftUp.y + nZ * CellWidth;

	switch(m_nDataType)
	{
	case HEIGHTMAPDATATYPE_FLOATHEIGHTONLY:
		{
			float* pVertex = (float*) pData;
			float  Height = pVertex[nZ*(nWidth+1)+nX];

			return D3DXVECTOR3(X,Height,Z);
			break;
		}
	case HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL:
		{
			D3DXVECTOR4* pVertex = (D3DXVECTOR4*) pData;
			D3DXVECTOR4  Height = pVertex[nZ*(nWidth+1)+nX];

			D3DXVECTOR3 Normal(Height.x,Height.y,Height.z);
			return Normal * Height.w + D3DXVECTOR3(X,0,Z);
			break;
		}
	}
	return D3DXVECTOR3(0,0,0);
}

D3DXVECTOR3 KHeightMapBase::GetVertexPosionBack(int nX,int nZ)
{
	if((nX>nWidth)||(nZ>nWidth))
	{
		float  X = m_LeftUp.x + nX * CellWidth;
		float  Z = m_LeftUp.y + nZ * CellWidth;
		return D3DXVECTOR3(X,0,Z);
	}

	float  X = m_LeftUp.x + nX * CellWidth;
	float  Z = m_LeftUp.y + nZ * CellWidth;

	switch(m_nDataType)
	{
	case HEIGHTMAPDATATYPE_FLOATHEIGHTONLY:
		{
			float* pVertex = (float*) pDataBack;
			float  Height = pVertex[nZ*(nWidth+1)+nX];

			return D3DXVECTOR3(X,Height,Z);
			break;
		}
	case HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL:
		{
			D3DXVECTOR4* pVertex = (D3DXVECTOR4*) pDataBack;
			D3DXVECTOR4  Height = pVertex[nZ*(nWidth+1)+nX];

			D3DXVECTOR3 Normal(Height.x,Height.y,Height.z);
			return Normal * Height.w + D3DXVECTOR3(X,0,Z);
			break;
			break;
		}
	}
	return D3DXVECTOR3(0,0,0);
}

void KHeightMapBase::SetVertexPosition(D3DXVECTOR3 Pos,int nX,int nZ)
{
	if((nX>nWidth)||(nZ>nWidth))
	{
		return;
	}

	switch(m_nDataType)
	{
	case HEIGHTMAPDATATYPE_FLOATHEIGHTONLY:
		{
			float* pVertex = (float*) pData;
			pVertex[nZ*(nWidth+1)+nX] = Pos.y;

			break;
		}
	case HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL:
		{
			D3DXVECTOR4* pVertex = (D3DXVECTOR4*) pData;
			pVertex[nZ*(nWidth+1)+nX].w = Pos.y;
			break;
		}
	}
}

void KHeightMapBase::SetVertexNormal(D3DXVECTOR3 Normal,int nX,int nZ)
{
	if((nX>nWidth)||(nZ>nWidth))
	{
		return;
	}

	switch(m_nDataType)
	{
	case HEIGHTMAPDATATYPE_FLOATHEIGHTONLY:
		{
			break;
		}
	case HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL:
		{
			D3DXVECTOR4* pVertex = (D3DXVECTOR4*) pData;
			pVertex[nZ*(nWidth+1)+nX].x = Normal.x;
			pVertex[nZ*(nWidth+1)+nX].y = Normal.y;
			pVertex[nZ*(nWidth+1)+nX].z = Normal.z;
			break;
		}
	}
}

D3DXVECTOR3 KHeightMapBase::GetVertexNormal(int nX,int nZ)
{
	D3DXVECTOR3 Normal(0,1,0);

	if((nX>nWidth)||(nZ>nWidth))
	{
		return Normal;
	}

	switch(m_nDataType)
	{
	case HEIGHTMAPDATATYPE_FLOATHEIGHTONLY:
		{
			return Normal;
			break;
		}
	case HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL:
		{
			D3DXVECTOR4* pVertex = (D3DXVECTOR4*) pData;
			Normal.x = pVertex[nZ*(nWidth+1)+nX].x;
			Normal.y = pVertex[nZ*(nWidth+1)+nX].y;
			Normal.z = pVertex[nZ*(nWidth+1)+nX].z;
			return Normal;
			break;
		}
	}

	return Normal;
}

HRESULT KHeightMapBase::SetLeftUp(D3DXVECTOR2 InXZ)
{
	m_LeftUp = InXZ;
	float FWidth  = m_BBox.GetLength();
	float FHeight = m_BBox.GetWidth();

	m_BBox.A.x = InXZ.x;
	m_BBox.A.z = InXZ.y;

	m_BBox.B.x = m_BBox.A.x+FWidth;
	m_BBox.B.z = m_BBox.A.z+FHeight;

	return S_OK;
}

BOOL KHeightMapBase::GetPosition(D3DXVECTOR3* pOutPos,D3DXVECTOR2 InXZ)
{
	if ((InXZ.x>=m_BBox.A.x)&&(InXZ.y>=m_BBox.A.z)&&
		(InXZ.x<=m_BBox.B.x)&&(InXZ.y<=m_BBox.B.z))
	{
		float X_Map = InXZ.x - m_BBox.A.x;
		float Z_Map = InXZ.y - m_BBox.A.z;

		int   n_X = (int)(X_Map / CellWidth);
		int   n_Z = (int)(Z_Map / CellWidth);

		float X_Cell = (X_Map - n_X*CellWidth)/CellWidth;
		float Z_Cell = (Z_Map - n_Z*CellWidth)/CellWidth;

		if(X_Cell>=Z_Cell)
		{
			D3DXVECTOR3 V1 = GetVertexPosion(n_X  ,n_Z  );
			D3DXVECTOR3 V2 = GetVertexPosion(n_X+1,n_Z  );
			D3DXVECTOR3 V3 = GetVertexPosion(n_X+1,n_Z+1);

			float H = V1.y*(1-X_Cell) + V2.y*X_Cell +
				(V3.y-V2.y)*X_Cell*Z_Cell;
			//(*pOutPos) = D3DXVECTOR3(InXZ.x,H,InXZ.y);

			(*pOutPos) = V1*(1-X_Cell) + V2*X_Cell + (V3-V2)*X_Cell*Z_Cell;

			//D3DXVec3BaryCentric(pOutPos,&V1,&V2,&V3,X_Cell,Z_Cell);
			return TRUE;
		}
		else
		{
			D3DXVECTOR3 V1 = GetVertexPosion(n_X  ,n_Z  );
			D3DXVECTOR3 V2 = GetVertexPosion(n_X  ,n_Z+1);
			D3DXVECTOR3 V3 = GetVertexPosion(n_X+1,n_Z+1);

			float H = V1.y*(1-Z_Cell) + V2.y*Z_Cell +
				(V3.y-V2.y)*Z_Cell*X_Cell;
			
			//(*pOutPos) = D3DXVECTOR3(InXZ.x,H,InXZ.y);
			
			(*pOutPos) = V1*(1-Z_Cell) + V2*Z_Cell + (V3-V2)*X_Cell*Z_Cell;

			//D3DXVec3BaryCentric(pOutPos,&V1,&V2,&V3,Z_Cell,X_Cell);
			return TRUE;
		}
	}
	else
	{
		float X_Map = InXZ.x - m_BBox.A.x;
		float Z_Map = InXZ.y - m_BBox.A.z;

		int   n_X = (int)(X_Map / CellWidth);
		int   n_Z = (int)(Z_Map / CellWidth);

		if(n_X<0) n_X = 0;
		if(n_Z<0) n_Z = 0;

		if(n_X>nWidth ) n_X = nWidth;
		if(n_Z>nHeight) n_Z = nHeight;

		D3DXVECTOR3 V = GetVertexPosion(n_X,n_Z);
		(*pOutPos) = V;
	}
	return FALSE;
}

BOOL KHeightMapBase::GetNormal(D3DXVECTOR3* pOutNormal,D3DXVECTOR2 InXZ)
{
	D3DXVECTOR3 Pos;
	if(!GetPosition(&Pos,InXZ))
	{
		(*pOutNormal) = D3DXVECTOR3(0,1,0);
		return FALSE;
	}

	D3DXVECTOR2 P_X_UP(InXZ.x+30,InXZ.y);
	D3DXVECTOR2 P_Z_UP(InXZ.x,InXZ.y+30);

	D3DXVECTOR3 H_X_UP;
	D3DXVECTOR3 H_Z_UP;

	GetPosition(&H_X_UP,P_X_UP);
	GetPosition(&H_Z_UP,P_Z_UP);

	D3DXVECTOR3 N_X_UP(H_X_UP.y-Pos.y,30,0);
	D3DXVECTOR3 N_Z_UP(0,30,H_Z_UP.y-Pos.y);
	D3DXVec3Normalize(&N_X_UP,&N_X_UP);
	D3DXVec3Normalize(&N_Z_UP,&N_Z_UP);

	D3DXVECTOR3 Normal = (N_X_UP + N_Z_UP)*0.5f;;
	D3DXVec3Normalize(&Normal,&Normal);	

	(*pOutNormal) = Normal;
	return TRUE;
}

void KHeightMapBase::CreateTestMap()
{
	nWidth  = 1;
	nHeight = 1;

	m_nDataType = HEIGHTMAPDATATYPE_FLOATHEIGHTONLY;

	fEffect = 1.0f;
	CellWidth = 500.0f;;
	float* pVertexHeight = new float[4*nWidth*nHeight];
	pVertexHeight[0] = 100;
	pVertexHeight[1] = 500;
	pVertexHeight[2] = 0;
	pVertexHeight[3] = 100;
	pData = pVertexHeight;

	m_BBox.A = GetVertexPosion(0,0);
	m_BBox.B = GetVertexPosion(1,1);
}

HRESULT KHeightMapBase::Create(D3DXVECTOR2 Left_Up,int Width,int Height,int DataType,
			   float Cell_Width,float BaseHeight)
{
	m_LeftUp = Left_Up;

	nWidth = Width;
	nHeight = Height;

	CellWidth = Cell_Width;

	SAFE_DELETE_ARRAY(pData);

	m_nDataType = DataType;
	int Size = (nWidth+1)*(nHeight+1);

	switch(m_nDataType)
	{
	case HEIGHTMAPDATATYPE_FLOATHEIGHTONLY:
		{
			float* pVertexHeight = new float[Size];
			ZeroMemory( pVertexHeight, sizeof(float)*Size );
			pData = pVertexHeight;

			float* pVertexHeightBack = new float[Size];
			ZeroMemory( pVertexHeightBack, sizeof(float)*Size );
			pDataBack = pVertexHeightBack;

			nDataSize =  sizeof(float)*Size;

			for(int y=0;y<=Height;y++)
			{
				for(int x=0;x<=Width;x++)
				{
				
					D3DXVECTOR3 Pos(0,BaseHeight,0);
					SetVertexPosition(Pos,x,y);
				}
			}
			break;
		}
	case HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL:
		{
			D3DXVECTOR4* pVertexHeight = new D3DXVECTOR4[Size];
			ZeroMemory( pVertexHeight, sizeof(D3DXVECTOR4)*Size);
			pData = pVertexHeight;

			D3DXVECTOR4* pVertexHeightBack = new D3DXVECTOR4[Size];
			ZeroMemory( pVertexHeightBack, sizeof(D3DXVECTOR4)*Size);
			pDataBack = pVertexHeightBack;

			nDataSize =  sizeof(D3DXVECTOR4)*Size;

			for(int y=0;y<=Height;y++)
			{
				for(int x=0;x<=Width;x++)
				{
				
					D3DXVECTOR3 Pos(0,BaseHeight,0);
					D3DXVECTOR3 Normal(0,1,0);
					SetVertexPosition(Pos,x,y);
					SetVertexNormal(Normal,x,y);
				}
			}
			break;
		}
	}

	m_BBox.Clear();
	m_BBox.AddPosition(GetVertexPosion(0,0));
	m_BBox.AddPosition(GetVertexPosion(nWidth,nHeight));

	return S_OK;
}

HRESULT KHeightMapBase::FromTexture(LPDIRECT3DTEXTURE9 pTexture,float HeightBase,float HeightScale)
{
	if (!pTexture)
		return E_FAIL;

	D3DSURFACE_DESC Desc;
	Desc.Width =0;
	Desc.Height =0;

	if (FAILED(pTexture->GetLevelDesc(0,&Desc)))
		return E_FAIL;
	if ((Desc.Format!=D3DFMT_A8R8G8B8)&&(Desc.Format!=D3DFMT_X8R8G8B8))
		return E_FAIL;

	D3DLOCKED_RECT lockedRect;
	lockedRect.pBits = NULL;

	if (FAILED(pTexture->LockRect(0, &lockedRect, NULL, D3DLOCK_READONLY )))
		return E_FAIL;

	BYTE* pTextData = (BYTE*) lockedRect.pBits;

	DWORD BitK = 1;

	float Rt_Width  = ((float)Desc.Width)/nWidth;
	float Rt_Height = ((float)Desc.Height)/nHeight;
	float* pVertex = (float*) pData;

	for(DWORD X =0;X<=(DWORD)nWidth;X++)
	{
		for(DWORD Y =0;Y<=(DWORD)nHeight;Y++)
		{
			DWORD Text_X = (DWORD)(X*Rt_Width);
			DWORD Text_Y = (DWORD)(Y*Rt_Height);

			float S_X = X*Rt_Width  - Text_X;
			float S_Y = Y*Rt_Height - Text_Y;

			DWORD Text_X_To = Text_X + 1;
			if (Text_X_To>=Desc.Width) Text_X_To = Desc.Width-1;
			DWORD Text_Y_To = Text_Y + 1;
			if (Text_Y_To>=Desc.Height) Text_Y_To = Desc.Height-1;

			float Pos_Y = pTextData[(Text_Y*Desc.Height+Text_X)*4+BitK]*HeightScale-HeightBase;

			pVertex[Y*(nWidth+1)+X] = Pos_Y;
		}
	}
	pTexture->UnlockRect(0);

	m_BBox.Clear();
	m_BBox.AddPosition(GetVertexPosion(0,0));
	m_BBox.AddPosition(GetVertexPosion(nWidth,nHeight));

	return S_OK;
}

void KHeightMapBase::BackUpData()
{
	if((pDataBack)&&(pData))
		memcpy(pDataBack,pData,nDataSize);
}

void KHeightMapBase::RestoreData()
{
	if((pDataBack)&&(pData))
		memcpy(pData,pDataBack,nDataSize);
}

HRESULT KHeightMapBase::Save2File(FILE* pFile)
{
	if(!pFile)
		return E_FAIL;

	fwrite(&m_nDataType,sizeof(int),1,pFile);
	fwrite(&nHeight,sizeof(int),1,pFile);
	fwrite(&nWidth ,sizeof(int),1,pFile);
	fwrite(&fEffect,sizeof(float),1,pFile);
	fwrite(&CellWidth,sizeof(float),1,pFile);
	fwrite(&m_LeftUp,sizeof(D3DXVECTOR2),1,pFile);
	fwrite(&nDataSize,sizeof(int),1,pFile);

	switch(m_nDataType)
	{
	case HEIGHTMAPDATATYPE_FLOATHEIGHTONLY:
		{
			DWORD Size =  (nWidth+1)*(nHeight+1);
			float* pHeightData = (float*)pData;
			for(DWORD i=0;i<Size;i++)
			{
				fwrite(&pHeightData[i],sizeof(float),1,pFile);
			}
			break;
		}
	case HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL:
		{
			DWORD Size =  (nWidth+1)*(nHeight+1);
			D3DXVECTOR4* pHeightData = (D3DXVECTOR4*)pData;
			for(DWORD i=0;i<Size;i++)
			{
				fwrite(&pHeightData[i],sizeof(D3DXVECTOR4),1,pFile);
			}
			break;
		}
	}
	return S_OK;
}

HRESULT KHeightMapBase::LoadFromFile(FILE* pFile)
{
	if(!pFile)
		return E_FAIL;

	fread(&m_nDataType,sizeof(int),1,pFile);
	fread(&nHeight,sizeof(int),1,pFile);
	fread(&nWidth ,sizeof(int),1,pFile);
	fread(&fEffect,sizeof(float),1,pFile);
	fread(&CellWidth,sizeof(float),1,pFile);
	fread(&m_LeftUp,sizeof(D3DXVECTOR2),1,pFile);	
	fread(&nDataSize,sizeof(int),1,pFile);

	SAFE_DELETE_ARRAY(pData);
	SAFE_DELETE_ARRAY(pDataBack);

	switch(m_nDataType)
	{
	case HEIGHTMAPDATATYPE_FLOATHEIGHTONLY:
		{
			DWORD Size =  (nWidth+1)*(nHeight+1);

			float* pVertexHeight     = new float[Size];
			float* pVertexHeightBack = new float[Size];
			ZeroMemory( pVertexHeight, sizeof(float)*Size );

			for(DWORD i=0;i<Size;i++)
			{
				if(0==fread(&pVertexHeight[i],sizeof(float),1,pFile))
				{
					ASSERT(0);
				}
			}

			pData = pVertexHeight;
			pDataBack = pVertexHeightBack;
			break;
		}
	case HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL:
		{
			DWORD Size =  (nWidth+1)*(nHeight+1);

			D3DXVECTOR4* pVertexHeight     = new D3DXVECTOR4[Size];
			D3DXVECTOR4* pVertexHeightBack = new D3DXVECTOR4[Size];
			ZeroMemory( pVertexHeight, sizeof(D3DXVECTOR4)*Size );

			for(DWORD i=0;i<Size;i++)
			{
				if(0==fread(&pVertexHeight[i],sizeof(D3DXVECTOR4),1,pFile))
				{
					ASSERT(0);
				}
			}

			pData = pVertexHeight;
			pDataBack = pVertexHeightBack;
			break;
		}
	}

	m_BBox.Clear();
	m_BBox.AddPosition(GetVertexPosion(0,0));
	m_BBox.AddPosition(GetVertexPosion(nWidth,nHeight));

	return S_OK;
}

HRESULT KHeightMapBase::CheckBorder_Left (KHeightMapBase* DestMap)
{
	for(int i=0;i<33;i++)
	{
		D3DXVECTOR3 Pos = GetVertexPosion(0,i);
		DestMap->SetVertexPosition(Pos,32,i);
	}
	return S_OK;
}

HRESULT KHeightMapBase::CheckBorder_Right(KHeightMapBase* DestMap)
{
	for(int i=0;i<33;i++)
	{
		D3DXVECTOR3 Pos = GetVertexPosion(32,i);
		DestMap->SetVertexPosition(Pos,0,i);
	}
	return S_OK;
}

HRESULT KHeightMapBase::CheckBorder_Up   (KHeightMapBase* DestMap)
{
	for(int i=0;i<33;i++)
	{
		D3DXVECTOR3 Pos = GetVertexPosion(i,0);
		DestMap->SetVertexPosition(Pos,i,32);
	}
	return S_OK;
}

HRESULT KHeightMapBase::CheckBorder_Down (KHeightMapBase* DestMap)
{
	for(int i=0;i<33;i++)
	{
		D3DXVECTOR3 Pos = GetVertexPosion(i,32);
		DestMap->SetVertexPosition(Pos,i,32);
	}
	return S_OK;
}