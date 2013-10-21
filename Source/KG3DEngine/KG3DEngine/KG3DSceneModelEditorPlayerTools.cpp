#include "StdAfx.h"
#include "KG3DSceneModelEditorPlayerTools.h"
#include "KG3DModelST.h"
#include "KG3DModelTable.h"
#include "KG3DGraphicsTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DSceneModelEditorPlayerTools::KG3DSceneModelEditorPlayerTools(void)
{
	m_lpModelStand = NULL;

	//m_lpVertexStand = NULL;
	//m_dwNumVertexStand = 0;

	m_lpModelST = NULL;
	m_lpModelHead = NULL;
	m_lpModelFace = NULL;
	m_lpModelBody = NULL;
	m_lpModelHand = NULL;
	m_lpModelLeg  = NULL;
	m_lpModelBelt = NULL;
	m_lpModelStand = NULL;

}

KG3DSceneModelEditorPlayerTools::~KG3DSceneModelEditorPlayerTools(void)
{
	SAFE_RELEASE(m_lpModelStand);

	SAFE_RELEASE(m_lpModelST);
	SAFE_RELEASE(m_lpModelHead);
	SAFE_RELEASE(m_lpModelFace);
	SAFE_RELEASE(m_lpModelBody);
	SAFE_RELEASE(m_lpModelHand);
	SAFE_RELEASE(m_lpModelLeg);
	SAFE_RELEASE(m_lpModelBelt);
	SAFE_RELEASE(m_lpModelStand);
}

HRESULT KG3DSceneModelEditorPlayerTools::LoadModifyModelST(LPSTR pFileName)
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(m_lpModelST);
	hr = g_cModelTable.LoadResourceFromFile(pFileName, 
		0,
		MLO_TEXTURE_MULTI,
		(IKG3DResourceBase**)&m_lpModelST);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneModelEditorPlayerTools::LoadStandModel(LPCSTR pFileName)
{
	HRESULT hr = S_OK;

	SAFE_RELEASE(m_lpModelStand);
	hr = g_cModelTable.LoadResourceFromFile(pFileName, 
		0,
		MLO_TEXTURE_MULTI,
		(IKG3DResourceBase**)&m_lpModelStand);
	KGLOG_COM_PROCESS_ERROR(hr);
	
	CreateSpheresFromModel(m_lpModelStand);
	
	//m_lpModelStand->SetTranslation(&D3DXVECTOR3(1000.0f,0,0));
	return S_OK;
Exit0:
	return E_FAIL;
}

//HRESULT KG3DSceneModelEditorPlayerTools::LockStandModel()
//{
//	HRESULT hr = S_OK;
//	DWORD dwFVF = 0;
//
//	KGLOG_PROCESS_ERROR(m_lpModelStand);
//
//	KGLOG_PROCESS_ERROR(m_lpModelStand->GetMesh());
//
//	KG3DMesh* pMesh = m_lpModelStand->GetMesh();
//	KGLOG_PROCESS_ERROR(pMesh);
//
//	KGLOG_PROCESS_ERROR(pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]);
//
//	dwFVF = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetFVF();
//	KGLOG_PROCESS_ERROR(dwFVF==(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));
//
//	m_dwNumVertexStand = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetNumVertices();
//
//	hr = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->LockVertexBuffer(0, (void**)&m_lpVertexStand);
//	KGLOG_COM_PROCESS_ERROR(hr);
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}
//
//HRESULT KG3DSceneModelEditorPlayerTools::UnLockStandModel()
//{
//	HRESULT hr = S_OK;
//
//	KGLOG_PROCESS_ERROR(m_lpModelStand);
//
//	KGLOG_PROCESS_ERROR(m_lpModelStand->GetMesh());
//
//	KG3DMesh* pMesh = m_lpModelStand->GetMesh();
//	KGLOG_PROCESS_ERROR(pMesh);
//
//	KGLOG_PROCESS_ERROR(pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]);
//
//	hr = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockVertexBuffer();
//	KGLOG_COM_PROCESS_ERROR(hr);
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

int KG3DSceneModelEditorPlayerTools::FindNearestVertex(D3DXVECTOR3& vPos)
{
	static float fMax = 1.0F;

	float fFind = 10000;
	int   nFind = -1;

	for (DWORD i=0;i<(DWORD)m_vecSplitSphere.size();i++)
	{
		float fDistance = D3DXVec3LengthSq(&(m_vecSplitSphere[i].vPos - vPos));
		if(fDistance<fMax)
		{
			if(fFind>fDistance)
			{
				nFind = i;
				fFind = fDistance;
			}
		}
	}

	if(fFind >= 1)
		nFind = -1;

	return nFind;
}

HRESULT KG3DSceneModelEditorPlayerTools::WedVertexForModel(KG3DModel* piModel,D3DXMATRIX* pmat,LPSTR szMeshName)
{
	HRESULT hr = S_OK;
	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	DWORD dwFVF = 0;
	DWORD dwNumVertex = 0;
	D3DXMATRIX matInverse;
	D3DXVECTOR3 Pos;

    KG3DModel *pModel = dynamic_cast<KG3DModel *>(piModel);
    KGLOG_PROCESS_ERROR(pModel);
	
    //hr = LockStandModel();
	//KGLOG_COM_PROCESS_ERROR(hr);
	m_vecFoundPos.clear();

	D3DXMatrixInverse(&matInverse,NULL,pmat);
	KGLOG_PROCESS_ERROR(pModel);

	KGLOG_PROCESS_ERROR(pModel->GetMesh());

	KG3DMesh* pMesh = pModel->GetMesh();
	KGLOG_PROCESS_ERROR(pMesh);

	KGLOG_PROCESS_ERROR(pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]);

	dwFVF = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetFVF();
	KGLOG_PROCESS_ERROR(dwFVF==(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));

	dwNumVertex = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetNumVertices();

	hr = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->LockVertexBuffer(0, (void**)&pVers);
	KGLOG_COM_PROCESS_ERROR(hr);
	//////////////////////////////////////////////////////////////////////////
	for(DWORD i=0;i<dwNumVertex;i++)
	{
		D3DXVec3TransformCoord(&Pos,&pVers[i].p,pmat);
		int nIndex = FindNearestVertex(Pos);

		if(nIndex!=-1)
		{
			pVers[i].Normal = D3DXVECTOR3(0,1,0);
			D3DXVec3TransformNormal(&pVers[i].Normal,&m_vecSplitSphere[nIndex].vNormal,&matInverse);
			m_vecFoundPos.push_back(m_vecSplitSphere[nIndex]);
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	hr = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockVertexBuffer();
	KGLOG_COM_PROCESS_ERROR(hr);

	//hr = UnLockStandModel();
	//KGLOG_COM_PROCESS_ERROR(hr);

	{
		//piModel->SaveMeshToFile( szMeshName , 0);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneModelEditorPlayerTools::Render()
{
	if(m_lpModelStand)
	{
		m_lpModelStand->Render(0,NULL);
	}

	if(m_lpModelST)
	{
		m_lpModelST->Render(0,NULL);
	}

	for (size_t i=0;i<m_vecFoundPos.size();i++)
	{
		D3DXVECTOR3 vPos = m_vecFoundPos[i].vPos;
		D3DXVECTOR3 vStart = vPos + m_vecFoundPos[i].vNormal * 10;
		g_cGraphicsTool.DrawLine(&vStart,&vPos,0xFFFF0000,0xFF00FF00);
	}

	if(0)
	{
		for (size_t i=0;i<m_vecSplitSphere.size();i++)
		{
			D3DXVECTOR3 vPos = m_vecSplitSphere[i].vPos;
			D3DXVECTOR3 vStart = vPos + m_vecSplitSphere[i].vNormal * 10;
			g_cGraphicsTool.DrawLine(&vPos,&vStart,0xFFFFFF00,0xFF00FF00);
		}
	}

	return S_OK;
}

HRESULT KG3DSceneModelEditorPlayerTools::LoadPartFromIni(LPSTR pFileName)
{
	IIniFile* pFile = NULL;
	int nIndex = 0;

	//KGLOG_PROCESS_ERROR(pFileName);

	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];
	TCHAR BaseDir[256];

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );
	wsprintf(BaseDir,"%s%s",drive,dir);
	m_szModelBaseDir = BaseDir;

	pFile = g_OpenIniFile(pFileName, false, true);
	if(!pFile)
		return E_FAIL;
	//KGLOG_PROCESS_ERROR(pFile);

	m_vecHead.clear();
	m_vecFace.clear();
	m_vecBody.clear();
	m_vecHand.clear();
	m_vecLeg.clear();
	m_vecBelt.clear();
	m_vecMDL.clear();

	for(nIndex=0;nIndex<=1000;nIndex++)
	{
		TCHAR SectionName[256];
		TCHAR IDName[256];
		TCHAR ModelName[256];
		TCHAR MeshName[256];

		wsprintf(SectionName,"Name%d",nIndex);
		if(pFile->IsSectionExist(SectionName))
		{
			pFile->GetString(SectionName,"ID","",IDName,256);

			if(_strcmpi(IDName,"")!=0)
			{
				/*wsprintf(ModelName,"%s",IDName);
				m_vecMDL.push_back(ModelName);*/

				wsprintf(MeshName,"%s%s_head.Mesh",BaseDir,IDName);
				wsprintf(ModelName,"%s_head",IDName);
				if(g_IsFileExist(MeshName))
					m_vecHead.push_back(ModelName);

				wsprintf(MeshName,"%s%s_body.Mesh",BaseDir,IDName);
				wsprintf(ModelName,"%s_body",IDName);
				if(g_IsFileExist(MeshName))
					m_vecBody.push_back(ModelName);

				wsprintf(MeshName,"%s%s_hand.Mesh",BaseDir,IDName);
				wsprintf(ModelName,"%s_hand",IDName);
				if(g_IsFileExist(MeshName))
					m_vecHand.push_back(ModelName);

				wsprintf(MeshName,"%s%s_belt.Mesh",BaseDir,IDName);
				wsprintf(ModelName,"%s_belt",IDName);
				if(g_IsFileExist(MeshName))
					m_vecBelt.push_back(ModelName);

				wsprintf(MeshName,"%s%s_leg.Mesh",BaseDir,IDName);
				wsprintf(ModelName,"%s_leg",IDName);
				if(g_IsFileExist(MeshName))
					m_vecLeg.push_back(ModelName);

				wsprintf(MeshName,"%s%s_face.Mesh",BaseDir,IDName);
				wsprintf(ModelName,"%s_face",IDName);
				if(g_IsFileExist(MeshName))
					m_vecFace.push_back(ModelName);
			}
		}
		else
			break;
	}

	pFile->Release();

	return S_OK;
}

HRESULT KG3DSceneModelEditorPlayerTools::GetModelPartVector(int nIndex,vector<string>* pvec)
{
	switch(nIndex)
	{
	case ST_PART_HEAD:
		(*pvec) = m_vecHead;
		break;
	case ST_PART_FACE:
		(*pvec) = m_vecFace;
		break;
	case ST_PART_BODY:
		(*pvec) = m_vecBody;
		break;
	case ST_PART_HAND:
		(*pvec) = m_vecHand;
		break;
	case ST_PART_LEG:
		(*pvec) = m_vecLeg;
		break;
	case ST_PART_BELT:
		(*pvec) = m_vecBelt;
		break;
	//case ST_PART_:
	//	(*pvec) = m_vecMDL;
	//	break;
	}

	return S_OK;
}

HRESULT KG3DSceneModelEditorPlayerTools::SetCurrentModelPart(int nPart,int nIndex)
{
	static D3DXMATRIX matId;
	D3DXMatrixIdentity(&matId);
	HRESULT hr = E_FAIL;
	TCHAR szName[MAX_PATH];
	KG3DModel* pModel = NULL;
	KGLOG_PROCESS_ERROR(m_lpModelST);

	hr = GetModelPartFileName(szName,nPart,nIndex);
	KGLOG_COM_PROCESS_ERROR(hr);


	hr = g_cModelTable.LoadResourceFromFile(szName,0,0,(IKG3DResourceBase**)&pModel);
	KGLOG_COM_PROCESS_ERROR(hr);

	if(nPart == ST_PART_FACE)
	{
		hr = pModel->BindToSocket(m_lpModelST, "S_Face");
		KGLOG_COM_PROCESS_ERROR(hr);

		pModel->AddRef();

		pModel->FrameMove();

		WedVertexForModel(pModel,&pModel->m_matWorld,szName);

	}
	else
	{
		hr = m_lpModelST->ChangeModel(nPart,pModel);
		KGLOG_COM_PROCESS_ERROR(hr);

		WedVertexForModel(pModel,&matId,szName);
	}
	

	hr = S_OK;
Exit0:
	SAFE_RELEASE(pModel);
	return hr;
}

HRESULT KG3DSceneModelEditorPlayerTools::GetModelPartFileName(LPSTR pFileName,int nPart,int nIndex)
{
	vector<string>* pvec = NULL;

	switch(nPart)
	{
	case ST_PART_HEAD:
		pvec = &m_vecHead;
		break;
	case ST_PART_FACE:
		pvec = &m_vecFace;
		break;
	case ST_PART_BODY:
		pvec = &m_vecBody;
		break;
	case ST_PART_HAND:
		pvec = &m_vecHand;
		break;
	case ST_PART_LEG :
		pvec = &m_vecLeg;
		break;
	case ST_PART_BELT:
		pvec = &m_vecBelt;
		break;
	//case 6:
	//	pvec = &m_vecMDL;
	//	break;
	}

	if (!pvec)
	{
		return E_FAIL;
	}

	if(nIndex<0)
		return E_FAIL;
	if(nIndex>=(int)pvec->size())
		return E_FAIL;

	wsprintf(pFileName,"%s%s.Mesh",m_szModelBaseDir.c_str(),(*pvec)[nIndex].c_str());

	return S_OK;
}

HRESULT KG3DSceneModelEditorPlayerTools::WedVertexForModel(KG3DModel* pModel,
														 VFormat::FACES_NORMAL_TEXTURE1* pVerStand,D3DXMATRIX* pmat)
{
	HRESULT hr = S_OK;
	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	DWORD dwFVF = 0;
	DWORD dwNumVertex = 0;
	D3DXMATRIX matInverse;
	D3DXVECTOR3 Pos;
	//IEKG3DMeshBase *piMeshBase = NULL;
	IEKG3DMesh* pMesh = NULL;
	BOOL bMeshSys = FALSE;
	DWORD dwSize = 0;

	D3DXMatrixInverse(&matInverse,NULL,pmat);
	KGLOG_PROCESS_ERROR(pModel);

	pModel->GetIEKG3DMesh(&pMesh);
	KGLOG_PROCESS_ERROR(pMesh);

	pMesh->IsMeshSysNotNull(&bMeshSys);
	KGLOG_PROCESS_ERROR(bMeshSys);

	pMesh->GetMeshSysFVF(&dwFVF);
	KGLOG_PROCESS_ERROR(dwFVF==(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));

	pMesh->GetMeshSysNumVertices(&dwNumVertex);

	hr = pMesh->LockVertexBufferOfMesfSys(0, (void**)&pVers);
	KGLOG_COM_PROCESS_ERROR(hr);
	//////////////////////////////////////////////////////////////////////////
	for(DWORD i = 0; i < dwNumVertex; i++)
	{
		dwSize = (DWORD)m_vecSplitSphere.size();
		for(DWORD k = 0; k < dwSize; k++)
		{
			Sphere& she = m_vecSplitSphere[k];

			D3DXVec3TransformCoord(&Pos,&pVers[i].p,pmat);

			if(she.IsPositionIn(Pos))
			{
				//ASSERT(vIndex.vecIndex.size()!=0);
				//int nIndex = vIndex.vecIndex[0];

				//D3DXVec3TransformNormal(&pVers[i].Normal,&pVerStand[nIndex].Normal,&matInverse);
				pVers[i].Normal = D3DXVECTOR3(1,0,0);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	hr = pMesh->UnLockVertexBufferOfMesfSys();
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneModelEditorPlayerTools::CreateSpheresFromModel(KG3DModel* pModel)
{
	HRESULT hr = S_OK;
	//IEKG3DMeshBase *piMeshBase = NULL;
	IEKG3DMesh *pMesh = NULL;
	BOOL bMeshSysState = FALSE;
	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	DWORD dwFVF = 0;
	DWORD dwNumVertex = 0;

	m_vecSplitSphere.clear();

	KGLOG_PROCESS_ERROR(pModel);

	hr = pModel->GetIEKG3DMesh(&pMesh);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = pMesh->IsMeshSysNotNull(&bMeshSysState);
	KGLOG_COM_PROCESS_ERROR(hr);

	KGLOG_PROCESS_ERROR(bMeshSysState);

	hr = pMesh->GetMeshSysFVF(&dwFVF);
	KGLOG_COM_PROCESS_ERROR(hr);

	KGLOG_PROCESS_ERROR(dwFVF==(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));

	hr = pMesh->GetMeshSysNumVertices(&dwNumVertex);
	KGLOG_COM_PROCESS_ERROR(hr);
	hr = pMesh->LockVertexBufferOfMesfSys(0, (void**)&pVers);
	KGLOG_COM_PROCESS_ERROR(hr);

	for(DWORD i =0;i < dwNumVertex;i++)
	{
		Sphere she;
		she.vPos    = pVers[i].p;
		she.vNormal = pVers[i].Normal;// = D3DXVECTOR3(0,1,0);
		she.fRadius = 0.01F;
		m_vecSplitSphere.push_back(she);
	}

	hr = pMesh->UnLockVertexBufferOfMesfSys();
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneModelEditorPlayerTools::SetStandModel(KG3DModel* pModel)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pModel);
	SAFE_RELEASE(m_lpModelStand);
	m_lpModelStand = pModel;
	pModel->AddRef();
	hr = CreateSpheresFromModel(m_lpModelStand);
Exit0:
	return hr;
}

KG3DModel* KG3DSceneModelEditorPlayerTools::GetStandModel()
{
	return m_lpModelStand;
}


HRESULT KG3DSceneModelEditorPlayerTools::WedModelVertex(KG3DModel* pModel,D3DXMATRIX* pmat)
{
	HRESULT hr = S_OK;
	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	DWORD dwFVF = 0;
	DWORD dwNumVertex = 0;
	D3DXMATRIX matInverse;
	D3DXVECTOR3 Pos;
	IEKG3DMesh* pMesh = NULL;
	BOOL bMeshSys = FALSE;
	DWORD dwSize = 0;

	int nCountWed = 0;
	
	if (pmat)
		D3DXMatrixInverse(&matInverse,NULL,pmat);
	else
		D3DXMatrixIdentity(&matInverse);
	KGLOG_PROCESS_ERROR(pModel);

	pModel->GetIEKG3DMesh(&pMesh);
	KGLOG_PROCESS_ERROR(pMesh);

	pMesh->IsMeshSysNotNull(&bMeshSys);
	KGLOG_PROCESS_ERROR(bMeshSys);

	pMesh->GetMeshSysFVF(&dwFVF);
	KGLOG_PROCESS_ERROR(dwFVF==(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1));

	pMesh->GetMeshSysNumVertices(&dwNumVertex);

	hr = pMesh->LockVertexBufferOfMesfSys(0, (void**)&pVers);
	KGLOG_COM_PROCESS_ERROR(hr);
	//////////////////////////////////////////////////////////////////////////
	for(DWORD i = 0; i < dwNumVertex; i++)
	{
		int nCountInOnePoint=0;
		dwSize = (DWORD)m_vecSplitSphere.size();
		for(DWORD k = 0; k < dwSize; k++)
		{
			Sphere& she = m_vecSplitSphere[k];
			if(pmat)
				D3DXVec3TransformCoord(&Pos,&pVers[i].p,pmat);
			else
				Pos = pVers[i].p;
			if(she.IsPositionIn(Pos))
			{
				nCountInOnePoint++;
				//ASSERT(vIndex.vecIndex.size()!=0);
				//int nIndex = vIndex.vecIndex[0];
				if(pmat)
				{
					//D3DXVec3TransformCoord(&pVers[i].p,&she.vPos,&matInverse);
					//pVers[i].Normal = D3DXVECTOR3(0,0,0);
					//pVers[i].Normal = she.vNormal;
					D3DXVec3TransformNormal(&pVers[i].Normal,&she.vNormal,&matInverse);
				}
				else
				{
					pVers[i].Normal =she.vNormal;
					//pVers[i].p = she.vPos;
				}
				//break ;
			}
		}
		if(nCountInOnePoint>0)
			nCountWed++;
		if(nCountInOnePoint > 1)
			ATLTRACE("第%i个顶点，受到了%i个点的影响\n",i,nCountInOnePoint);
		//_ASSERTE( nCountInOnePoint <= 1 && "nCountInOnPoint大于1");
	}
	//////////////////////////////////////////////////////////////////////////
	hr = pMesh->UnLockVertexBufferOfMesfSys();
	KGLOG_COM_PROCESS_ERROR(hr);
	{
		TCHAR strOutputInfo[MAX_PATH];
		sprintf_s(strOutputInfo, MAX_PATH, "有%d个顶点移动", nCountWed);
		MessageBox(NULL,strOutputInfo,"提示",0);
	}
	
	return S_OK;
Exit0:
	return E_FAIL;
}

BOOL KG3DSceneModelEditorPlayerTools::Sphere::IsPositionIn(D3DXVECTOR3 &vP)
{
	D3DXVECTOR3 C = vP - vPos;
	float fS = D3DXVec3Length(&C);
	BOOL bRet = (fS < FLT_EPSILON);//fRadius * fRadius;
	if(bRet)
		return TRUE;
	return bRet;
}