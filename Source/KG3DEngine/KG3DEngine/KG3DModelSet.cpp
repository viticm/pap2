#include "StdAfx.h"
//#include "KG3DModelSet.h"
//#include "kg3dbip.h"
//#include "Kg3dmodel.h"
//#include "Kg3dmodeltable.h"
//#include "KG3DTexture.h"
//#include "KG3DGraphicsTool.h"
//#include "KG3DTextureTable.h"
//#include "kg3dmodelst.h"
//#include "KG3DEngineManager.h"
//#include "KG3DScene.h"
//#include ".\PhysiscSystem\KG3DPhysiscScene.h"
//#ifdef _DEBUG
//#define new DEBUG_NEW_3DENGINE
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
//#endif
//
//KG3DModelSet::KG3DModelSet(void)
//{
//	m_pBip = NULL;
//	m_pContainer = NULL;
//}
//
//KG3DModelSet::~KG3DModelSet(void)
//{
//	CleanUp();
//}
//////////////////////////////////////////////////////////////////////////////////
//
//HRESULT KG3DModelSet::GetModelsCount(DWORD *pdwCount)
//{
//    HRESULT hResult = E_FAIL;
//    KGLOG_PROCESS_ERROR(pdwCount);
//
//    *pdwCount = (DWORD)(m_Models.size());
//
//    hResult = S_OK;
//Exit0:
//    return hResult;
//}
//////////////////////////////////////////////////////////////////////////////////
//HRESULT KG3DModelSet::GetModel(
//    DWORD dwPosition, 
//    IEKG3DModel **ppModel
//)
//{
//    HRESULT hResult = E_FAIL;                    
//    DWORD dwSize = 0;
//    
//    KGLOG_PROCESS_ERROR(ppModel);
//    
//    dwSize = (DWORD)(m_Models.size()); 
//    KGLOG_PROCESS_ERROR(dwPosition < dwSize);
//
//    *ppModel = m_Models[dwPosition];
//    hResult = S_OK;
//Exit0:
//    return hResult;
//}
//////////////////////////////////////////////////////////////////////////////////
//HRESULT KG3DModelSet::GetIEBip(IEKG3DBip **ppiBip)
//{
//    HRESULT hResult = E_FAIL;      
//    KGLOG_PROCESS_ERROR(ppiBip);
//    *ppiBip = m_pBip;
//    hResult = S_OK;
//Exit0:
//    return hResult;
//}
//
//////////////////////////////////////////////////////////////////////////////////
//
//void KG3DModelSet::CleanUp()
//{
//	
//}
//
//
//HRESULT KG3DModelSet::ReflushPhysicsSys()
//{
//    HRESULT hResult  = E_FAIL;
//    HRESULT hRetCode = E_FAIL;
//
//    KG3DScene* pScene = dynamic_cast<KG3DScene*>(g_cGraphicsTool.GetScene());
//
//    UpdateFlexibleBoneList();
//
//    if (pScene && pScene->m_lpPhysiscScene)
//        pScene->m_lpPhysiscScene->ReflushModel(m_pContainer);
//
//    hResult = S_OK;
//Exit0:
//    return hResult;
//}
//
//int KG3DModelSet::Attach(IKG3DModel* pModel)
//{
//	int nReturn = -1;
//	unsigned int uType = pModel->GetType();
//	KGLOG_PROCESS_ERROR(uType == MESHTYPE_DEFAULT);
//
//	KG3DModel* pDefaultModel = static_cast<KG3DModel*>(pModel);
//	pDefaultModel->AddRef();
//	pDefaultModel->SetParentObjMatrix(&m_pContainer->m_matWorld);
//	if (m_pBip)
//	{
//		pDefaultModel->AttachBip(m_pBip);
//	}
//	m_pContainer->m_BBoxLocal.AddPosition(pDefaultModel->m_BBox.A);
//	m_pContainer->m_BBoxLocal.AddPosition(pDefaultModel->m_BBox.B);
//	pDefaultModel->FrameMove();
//	pDefaultModel->ComputeBBox();
//	m_Models.push_back(pDefaultModel);
//	nReturn = static_cast<int>(m_Models.size() - 1);
//
//    ReflushPhysicsSys();
//
//Exit0:
//	return nReturn;
//}
//
//void KG3DModelSet::DetachModel(IKG3DModel* pModel)
//{
//    D3DXVECTOR3 vec3PosBuffer[8];
//
//	for(size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (static_cast<KG3DModel*>(pModel) == m_Models[i])
//		{
//			SAFE_RELEASE(m_Models[i]);
//			m_Models[i] = m_Models[m_Models.size() - 1];
//			m_Models.pop_back();
//			break;
//		}
//	}
//	m_pContainer->m_BBoxLocal.Clear();
//	m_pContainer->m_BBox.Clear();
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			m_pContainer->m_BBoxLocal.AddPosition(m_Models[i]->m_BBoxLocal.A);
//			m_pContainer->m_BBoxLocal.AddPosition(m_Models[i]->m_BBoxLocal.B);
//		}
//	}
//	m_pContainer->m_BBoxLocal.TransformCoord(vec3PosBuffer, &(m_pContainer->m_matWorld));
//	m_pContainer->m_BBox.AddPosition(vec3PosBuffer, 8);
//
//    ReflushPhysicsSys();
//}
//
//void KG3DModelSet::ChangeModel(IKG3DModel* pModelNew, IKG3DModel* pModelOld)
//{
//	KG3DModel* pTemp = NULL;
//	KG3DModel* pTemp1 = NULL;
//	KG_PROCESS_ERROR(pModelNew);
//	KG_PROCESS_ERROR(pModelOld);
//
//	pTemp = static_cast<KG3DModel*>(pModelNew);
//	pTemp1 = static_cast<KG3DModel*>(pModelOld);
//
//	for(size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (static_cast<KG3DModel*>(pModelOld) == m_Models[i])
//		{
//			
//			pTemp->CopyBindInfo(pTemp1);
//			SAFE_RELEASE(m_Models[i]);
//			m_Models[i] = pTemp;
//			pModelNew->AddRef();
//			m_Models[i]->AttachBip(m_pBip);
//			break;
//		}
//	}
//
//    ReflushPhysicsSys();
//
//Exit0:
//	return;
//}
//
//HRESULT KG3DModelSet::RenderToScreen(const D3DXVECTOR2* pA, 
//									 const D3DXVECTOR2* pC, 
//									 const D3DXVECTOR2* pUVA, 
//									 const D3DXVECTOR2* pUVC, 
//									 float fZ, 
//									 DWORD dwFlags,
//									 void* pExtInfo)
//{
//	HRESULT hr = E_FAIL;
//    HRESULT hRetCode = E_FAIL;
//	static DWORD dwRenderTextureID = 0;
//	KG3DTexture* pTexture = NULL;
//	IKG3DAnimationController* pController = NULL;
//	DWORD dwSampleStateMinFilterPrev = 0;
//	DWORD dwSampleStateMagFilterPrev = 0;
//	LPDIRECT3DBASETEXTURE9 pTexturePrev = NULL;
//	
//	g_pd3dDevice->GetTexture(0, &pTexturePrev);
//	g_pd3dDevice->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwSampleStateMagFilterPrev);
//	g_pd3dDevice->GetSamplerState(0, D3DSAMP_MINFILTER, &dwSampleStateMinFilterPrev);
//
//	float fWidth = abs(pC->x - pA->x);
//	float fHeight = abs(pC->y - pA->y);
//	hr = g_cGraphicsTool.GetRenderTargetTexture(
//        (unsigned)fWidth,
//		(unsigned)fHeight,
//		D3DFMT_A8R8G8B8,
//		&pTexture);
//
//	KG_COM_PROCESS_ERROR(hr);
//	pTexture->GetID(&dwRenderTextureID);
//	
//	KG_PROCESS_ERROR(m_pBip);
//
//	pController = GetAnimationController();
//	if (pController->HasAnimation())
//	{	
//		size_t j = 0;
//        if (m_Models.size())
//		{
//			while(!m_Models[j] && j < m_Models.size())
//				j++;
//			if (j < m_Models.size())
//			{
//				MeshRenderExtraInfo Info;
//				Info.Data = m_pBip->m_pObjMatrices;
//				Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;
//				m_Models[j]->RenderToTexture(dwRenderTextureID, 
//					pA,
//					pC,
//					pUVA,
//					pUVC,
//					fZ,
//					D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
//					RENDER_BIP,
//					&Info);
//			}
//		}
//		for(size_t i = j; i < m_Models.size(); i++)
//		{
//			if (m_Models[i])
//			{
//                MeshRenderExtraInfo Info;
//                Info.Data = m_pBip->m_pObjMatrices;
//                Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;
//                m_Models[i]->RenderToTexture(dwRenderTextureID,
//                    pA, 
//                    pC,
//                    pUVA,
//                    pUVC,
//                    fZ, 
//                    0, 
//                    RENDER_BIP,
//                    &Info);
//			}
//		}
//	}
//	else
//	{
//		size_t j = 0;
//		if (m_Models.size())
//		{
//			while(!m_Models[j] && j < m_Models.size())
//				j++;
//
//			if (j < m_Models.size())
//			{
//				m_Models[j]->RenderToTexture(dwRenderTextureID, 
//					pA,
//					pC,
//					pUVA,
//					pUVC,
//					fZ,
//					D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
//					0,
//					NULL);
//			}
//		}
//		for(size_t i = j; i < m_Models.size(); i++)
//		{
//			m_Models[i]->RenderToTexture(dwRenderTextureID,
//				pA, 
//				pC,
//				pUVA,
//				pUVC,
//				fZ, 
//				0, 
//				0,
//				NULL);
//		}
//	}
//
//	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
//	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
//
//	g_cGraphicsTool.DrawScreenRect(pA, pC, fZ, 1.0f, 1.0f, 0xffffffff, dwRenderTextureID);
//
//    hRetCode = S_OK;
//Exit0:
//	g_pd3dDevice->SetTexture(0, pTexturePrev);
//	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, dwSampleStateMinFilterPrev);
//	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, dwSampleStateMagFilterPrev);
//
//	return hRetCode;
//}
//
//
//HRESULT KG3DModelSet::RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX)
//{
//	HRESULT hr = E_FAIL;
//
//	UINT uOption = 0;
//	IKG3DAnimationController* pController = NULL;
//	KG_PROCESS_ERROR(m_pBip);
//
//	pController = GetAnimationController();
//	if (pController->HasAnimation())
//	{
//		for(size_t i = 0; i < m_Models.size(); i++)
//		{
//			if (m_Models[i])
//			{	
//                MeshRenderExtraInfo Info;
//                Info.Data = m_pBip->m_pObjMatrices;
//                Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;
//                m_Models[i]->RenderReflect(uOption | RENDER_BIP, &Info,fWaterHeight,WaterPlaneFX,WaterPlaneFX);
//            }
//		}
//	}
//	else
//	{
//		for(size_t i = 0; i < m_Models.size(); i++)
//		{
//			if (m_Models[i])
//			{
//				m_Models[i]->RenderReflect(uOption, NULL,fWaterHeight,WaterPlaneFX,WaterPlaneFX);
//			}
//		}
//	}
//
//	hr = S_OK;
//Exit0:
//	return hr;
//}
//
//HRESULT KG3DModelSet::Render(unsigned int uOption, void* pExtInfo)
//{
//	HRESULT hr = E_FAIL;
//	
//	IKG3DAnimationController* pController = NULL;
//	KG_PROCESS_ERROR(m_pBip);
//	
//	pController = GetAnimationController();
//	if (pController->HasAnimation())
//	{
//		for(size_t i = 0; i < m_Models.size(); i++)
//		{
//			if (m_Models[i])
//			{
//                MeshRenderExtraInfo Info;
//                Info.Data = m_pBip->m_pObjMatrices;
//                Info.dwType = ENUM_MESHRENDER_EXTR_INFO_MATRIX;
//                m_Models[i]->Render(uOption | RENDER_BIP, &Info);
//            }
//		}
//	}
//	else
//	{
//		for(size_t i = 0; i < m_Models.size(); i++)
//		{
//			if (m_Models[i])
//			{
//				m_Models[i]->Render(uOption, NULL);
//			}
//		}
//	}
//	
//	hr = S_OK;
//Exit0:
//	return hr;
//}
//
//
//void KG3DModelSet::UpdateFlexBoneMatrices()
//{
//   
//}
//
//void KG3DModelSet::FrameMove(const D3DXMATRIX& matBase)
//{
//	if (m_pBip)
//	{
//		m_pBip->FrameMove();
//		for (size_t i = 0; i < m_Models.size(); i++)
//		{
//			if (m_Models[i])
//			{
//				m_Models[i]->UpdateFlexibleBoneInput(m_FlexibleBoneContainer.m_pResultMatrix, m_pBip->m_pObjMatrices);
//				m_Models[i]->UpdateSockets(m_pBip->m_pObjMatrices);
//				m_Models[i]->FrameMove();
//				m_Models[i]->ComputeBBox();
//			}
//		}
//	}
//	
//}
//
//void KG3DModelSet::PostWarperFrameMove(const D3DXMATRIX& matBase, 
//									   const D3DXMATRIX* pMatSrc = NULL)
//{
//	KG_PROCESS_ERROR(m_pBip);
//
//	if (pMatSrc)
//	{
//		memcpy(m_pBip->m_pObjMatrices, pMatSrc, sizeof(D3DXMATRIX) * m_pBip->GetNumBones());
//	}
//	else
//	{
//		memcpy(m_pBip->m_pObjMatrices, m_pBip->m_pWorldMatrices, sizeof(D3DXMATRIX) * m_pBip->GetNumBones());
//	}
//
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//            m_Models[i]->UpdateFlexibleBoneInput(m_FlexibleBoneContainer.m_pResultMatrix,m_pBip->m_pObjMatrices);
//			m_Models[i]->UpdateSockets(m_pBip->m_pObjMatrices);
//			m_Models[i]->FrameMove();
//		}
//	}
//Exit0:
//	return;
//}
//
//void KG3DModelSet::Detach()
//{
//	SAFE_RELEASE(m_pBip);
//}
//
//HRESULT KG3DModelSet::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
//{
//	HRESULT hr = E_FAIL;
//	TCHAR strBuffer[MAX_PATH];
//	TCHAR strDriver[MAX_PATH];
//	TCHAR strPath[MAX_PATH];
//	TCHAR strFileName[MAX_PATH];
//	TCHAR strMtlName[MAX_PATH];
//	memset(strMtlName,0,MAX_PATH);
//	FILE* fpFile = fopen(cszFileName, "r");
//	KGLOG_PROCESS_ERROR(fpFile);
//
//	_splitpath(cszFileName, strDriver, strPath, NULL, NULL);
//
//	KGLOG_PROCESS_ERROR(fgets(strBuffer, MAX_PATH, fpFile));
//	strBuffer[strlen(strBuffer) - 1] = '\0';
//
//	SAFE_RELEASE(m_pBip);
//
//	if (strBuffer[0] == '\\')
//	{
//		//If path is started with '\', it's relative to sceneeditor.exe
//		sprintf(strFileName, "%s%s", g_Def_AppDirectory, strBuffer);
//		g_pFileFinder->NormalizePath(strFileName);
//	}
//	else
//	{
//		sprintf(strFileName, "%s%s%s", strDriver, strPath, strBuffer);
//	}
//
//	m_pBip = new KG3DBip;
//	KGLOG_PROCESS_ERROR(m_pBip);
//
//	hr = m_pBip->LoadFromTextFile(strFileName);
//	KG_COM_PROCESS_ERROR(hr);
//
//	uOption = MODEL_LOADOPT_WITHBIP; 
//
//	TCHAR strMesh[MAX_PATH];
//	strMesh[0]= '\0';
//	TCHAR strMtl[MAX_PATH];
//	strMtl[0]= '\0';
//	
//	
//	while(fgets(strBuffer, MAX_PATH * 2, fpFile))
//	{
//		size_t strLen = strlen(strBuffer);
//		strBuffer[strLen - 1] = '\0';
//	//	TrimString(strBuffer);
//		//读取mesh文件名和mtl文件名
//		TCHAR* strFind = strstr(strBuffer, "  ");
//		TCHAR* strEnd = strBuffer + strLen;
//		if(strFind)
//		{
//			strFind[0] = '\0';
//			strcpy(strMesh, strBuffer);
//			strFind[0] = ' ';
//
//			if(strFind != strEnd)
//			{
//				while(*strFind == ' ' && strFind != strEnd)
//					strFind++;
//				strcpy(strMtl, strFind);
//				uOption |= MODEL_LOADOPT_WITHOUTMAT ;
//			}
//			else
//			{
//				//No specify Mtl;
//				strMtl[0] = '\0';
//				uOption |= MODEL_LOADOPT_WITHBIP ;
//			}
//			
//		}
//		else
//		{
//			//No specify Mtl;
//			strcpy(strMesh, strBuffer);
//			strMtl[0]= '\0';
//			uOption = MODEL_LOADOPT_WITHBIP;
//		}
//
//		KG3DModel* pModel = NULL;
//		if (strMesh[0] == '\\')
//		{
//			//If path is started with '\', it's relative to sceneeditor.exe
//			sprintf(strFileName, "%s%s", g_Def_AppDirectory, strMesh);
//			g_pFileFinder->NormalizePath(strFileName);
//		}
//		else
//		{
//			sprintf(strFileName, "%s%s%s", strDriver, strPath, strMesh);
//		}
//		hr = g_cModelTable.LoadResourceFromFile(strFileName, 0, uOption, 
//			(IKG3DResourceBase **)&pModel);
//	
//		if (SUCCEEDED(hr))
//		{
//			if (strMtl[0] != '\0')
//			{
//				if (strMtl[0] == '\\')
//				{
//					//If path is started with '\', it's relative to sceneeditor.exe
//					sprintf(strMtlName, "%s%s", g_Def_AppDirectory, strMtl);
//					g_pFileFinder->NormalizePath(strMtlName);
//				}
//				else
//				{
//					sprintf(strMtlName, "%s%s%s", strDriver, strPath, strMtl);
//				}
//
//				hr = pModel->LoadMaterialFromFile(strMtlName,uOption);
//				if (FAILED(hr))
//					g_OutputDebugInfoFmt("Failed LoadMaterialFromFile: %s",strMtlName);
//				KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//			}
//
//			m_pContainer->m_BBoxLocal.AddPosition(pModel->m_BBox.A);
//			m_pContainer->m_BBoxLocal.AddPosition(pModel->m_BBox.B);
//			Attach(pModel);
//			pModel->Release();
//			//pModel->ConstructFlexibleBody();
//		}
//	}
//
//	hr = S_OK;
//	Exit0:
//	if (fpFile)
//	{
//		fclose(fpFile);
//	}
//	return hr;
//}
//
//void KG3DModelSet::Attach(KG3DBip* pBip)
//{
//	
//}
//
//HRESULT KG3DModelSet::BindToSocket(IKG3DModel* pModel, const TCHAR* strSocketName)
//{
//	
//Exit0:
//	return hr;
//}
//
//HRESULT KG3DModelSet::BindToBone(IKG3DModel* pModel, const char* strBoneName)
//{
//	HRESULT hr = E_FAIL;
//	
//	KGLOG_PROCESS_ERROR(pModel);
//	KGLOG_PROCESS_ERROR(strBoneName);
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			hr = m_Models[i]->BindToBone(pModel, strBoneName);
//			if (SUCCEEDED(hr))
//			{
//				break;
//			}
//		}
//	}
//Exit0:
//	return hr;
//}
//
//HRESULT KG3DModelSet::PlayAnimation(const TCHAR* strFileName, DWORD PlayType, float fOffset, float fSpeed, void* pExtInfo)
//{
//	HRESULT hr = E_FAIL;
//	KG_PROCESS_ERROR(m_pBip);
//	hr = m_pBip->PlayAnimation(strFileName, PlayType, fOffset, fSpeed, pExtInfo);
//	KG_COM_PROCESS_ERROR(hr);
//	hr = S_OK;
//Exit0:
//	return hr;
//}
//
//HRESULT KG3DModelSet::FindSocket(const char* strName, BindExtraInfo& Info)
//{
//	HRESULT hr = E_FAIL;
//	KG_PROCESS_ERROR(strName);
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			int n = m_Models[i]->FindSocket(strName, NULL);
//			if (n != -1)
//			{
//				Info.nBindIndex = n;
//				Info.pModel = m_Models[i];
//				hr = S_OK;
//				break;
//			}
//		}
//	}
//Exit0:
//	return hr;
//}
//
//HRESULT KG3DModelSet::FindBone(const char* strName, BindExtraInfo& Info)
//{
//	HRESULT hr = E_FAIL;
//	KG_PROCESS_ERROR(strName);
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			int n = m_Models[i]->FindBone(strName, NULL);
//			if (n != -1)
//			{
//				Info.nBindIndex = n;
//				Info.pModel = m_Models[i];
//				hr = S_OK;
//				break;
//			}
//		}
//	}
//Exit0:
//	return hr;
//}
//
//
//void KG3DModelSet::ComputerAABBBox(AABBOX& Box, D3DXMATRIX& mat)
//{
//	D3DXVECTOR3 vec3PosBuffer[8];
//	D3DXMATRIX mat1;
//	D3DXMatrixIdentity(&mat1);
//	Box.Clear();
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			Box.AddPosition(m_Models[i]->m_BBox.A);
//			Box.AddPosition(m_Models[i]->m_BBox.B);
//		}
//	}
//}
//
//IKG3DAnimationController* KG3DModelSet::GetAnimationController()
//{
//	return &m_pBip->m_AnimationController;
//}
//
//extern BOOL g_bClient;
//
//BOOL KG3DModelSet::IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3& Origin,D3DXVECTOR3& Direction)
//{
//    int nResult = FALSE;
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			if (m_Models[i]->IsRayIntersect(Inter, Origin, Direction))
//			{
//				nResult = TRUE;
//                break;
//			}  
//		}
//	}
//    //if (nResult && m_pContainer && !g_bClient)
//    if (m_pContainer)
//	{      
//        m_pContainer->m_BBoxLocal.Clear();
//        for (size_t i = 0; i < m_Models.size(); i++)
//        {
//            if (m_Models[i])
//            {
//                m_pContainer->m_BBoxLocal.AddPosition(m_Models[i]->m_BBoxLocal.A);
//                m_pContainer->m_BBoxLocal.AddPosition(m_Models[i]->m_BBoxLocal.B);
//            }
//        }
//    }
//	return nResult;
//}
//
//void KG3DModelSet::SetContainer(KG3DModelST* pModel)
//{
//	m_pContainer = NULL;
//	ASSERT(pModel);
//	m_pContainer = pModel;
//}
//
//void KG3DModelSet::SetHighLightLevel(float fIllumination, BOOL bInherit)
//{
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			m_Models[i]->SetHighLightLevel(fIllumination, bInherit);
//		}
//	}
//}
//
//HRESULT KG3DModelSet::SetAlpha(float fAlpha, BOOL bUseSpecial)
//{
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			m_Models[i]->SetAlpha(fAlpha, bUseSpecial);
//		}
//	}
//	return S_OK;
//}
//
//D3DXMATRIX* KG3DModelSet::GetWorldMatrices()
//{
//	D3DXMATRIX* pReturn = NULL;
//	KG_PROCESS_ERROR(m_pBip);
//
//	KG_PROCESS_ERROR(m_pBip->m_pWorldMatrices);
//
//	pReturn = m_pBip->m_pWorldMatrices;
//Exit0:
//	return pReturn;
//}
//
//const KG3DBip* KG3DModelSet::GetBip()
//{
//	return m_pBip;
//}
//
//void KG3DModelSet::UpdateBindBoneMatrices()
//{
//    std::list<BindBoneMatrix>::iterator i = m_lstBindBone.begin();
//    while (i != m_lstBindBone.end())
//    {
//        
//        i++;
//    }
//}
//HRESULT KG3DModelSet::GetModel(unsigned int uIndex, IKG3DModel** ppModel)
//{
//	HRESULT hr = E_FAIL;
//	KG_PROCESS_ERROR(uIndex < m_Models.size());
//
//	*ppModel = m_Models[uIndex];
//	hr = S_OK;
//Exit0:
//	return hr;
//}
//
//int KG3DModelSet::GetNumModel()
//{
//	return static_cast<int>(m_Models.size());
//}
//
//
//
//HRESULT KG3DModelSet::ChangeModel(unsigned int uIndex, IKG3DModel* pModel)
//{
//	HRESULT hr = E_FAIL;
//	KG3DModel* pTemp = static_cast<KG3DModel*>(pModel);
//	KG_PROCESS_ERROR(uIndex < m_Models.size());
//	KG_PROCESS_ERROR(pModel);
//
//	pTemp->CopyBindInfo(m_Models[uIndex]);
//	SAFE_RELEASE(m_Models[uIndex]);
//	pTemp->AddRef();
//	m_Models[uIndex] = pTemp;
//	m_Models[uIndex]->AttachBip(m_pBip);
//
//    ReflushPhysicsSys();
//
//	hr = S_OK;
//Exit0:
//	return hr;
//}
//int KG3DModelSet::GetNumBones()
//{
//	return m_pBip->GetNumBones();
//}
//
//int KG3DModelSet::GetNumSocket()
//{
//	int nReturn = 0;
//    INT nNumSocket = 0;
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			m_Models[i]->GetNumSocket(&nNumSocket);
//            nReturn += nNumSocket;
//		}
//	}
//	return nReturn;
//}
//
//LPCSTR KG3DModelSet::GetBoneName(int nIndex)
//{
//	return m_pBip->GetBoneName(nIndex);
//}
//
//LPCSTR KG3DModelSet::GetSocketName(int nIndex)
//{
//	int nSocket = 0;
//    INT nNumSocket = 0;
//	LPCSTR strReturn = NULL;
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			int nLastNumSocket = nSocket;
//            m_Models[i]->GetNumSocket(&nNumSocket);
//			nSocket += nNumSocket;
//			if (nSocket > nIndex)
//			{
//				m_Models[i]->GetSocketName(nIndex - nLastNumSocket, &strReturn);
//				break;
//			}
//		}
//	}
//	return strReturn;
//}
//
//
//int KG3DModelSet::GetBindToSocketModel(const char cszSocketName[],vector<IEKG3DModel*> &vecpModel)
//{
//	HRESULT hr = E_FAIL;
//	int nCount = 0;
//	BindExtraInfo Info;
//
//	KG_PROCESS_ERROR(cszSocketName);
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			int n = m_Models[i]->GetBindToSocketModel(cszSocketName , vecpModel);
//			nCount += n;
//		}
//	}
//Exit0:
//	return nCount;
//}
//
//int KG3DModelSet::GetBindToSocketModel(const char cszSocketName[],vector<IKG3DModel*> &vecpModel)
//{
//	HRESULT hr = E_FAIL;
//	int nCount = 0;
//	BindExtraInfo Info;
//
//	KG_PROCESS_ERROR(cszSocketName);
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			int n = m_Models[i]->GetBindToSocketModel(cszSocketName , vecpModel);
//			nCount += n;
//		}
//	}
//Exit0:
//	return nCount;
//}
//void KG3DModelSet::UnbindChild(const TCHAR* strBindObjName)
//{
//	KG_PROCESS_ERROR(strBindObjName);
//    IEKG3DModel *piModel = NULL;
//
//	for (size_t i = 0; i < m_Models.size(); i++)
//	{
//		if (m_Models[i])
//		{
//			m_Models[i]->UnbindChild(strBindObjName);
//		}
//	}
//Exit0:
//	;
//}
//
//
//
//
//
