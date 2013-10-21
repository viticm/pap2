
#include "StdAfx.h"
#include "KG3DRepresentObjectPVS.h"
#include "KG3DModelTable.h"
#include "KG3DCamera.h"
#include "KG3DGraphicsTool.h"
#include "KG3DSceneSceneEditor.h"
#include <new>
#include "KG3DSceneEntityMgr.h"
#include "KG3DRepresentObjectTable.h"
#include "KG3DIntersectionBase.h"
#include "KG3DStructs.h"
#include "KG3DScenePointLightRender.h"
#include "KG3DModelLightMap.h"
#include "KG3DModelPointLight.h"
#include "KG3DEngineManager.h"

KG3DPvsPortal::KG3DPvsPortal(KG3DRepresentObjectPVS* pPvs) 
    : m_pDependPvs(pPvs), m_nSelected(FALSE)
{
    D3DXMatrixIdentity(&m_matWorld);
    memset(m_vLocalVector, 0, sizeof(m_vLocalVector));
    memset(m_vWorldVector, 0, sizeof(m_vWorldVector));

    m_strName = "Portal";
}

HRESULT KG3DPvsPortal::RenderSelectMark()
{
    m_nSelected = TRUE;
    return S_OK;
}

HRESULT KG3DPvsPortal::GetBBox(AABBOX* pAABBox)
{
    ASSERT(pAABBox);

    pAABBox->Clear();
    pAABBox->AddPosition(m_vWorldVector, 4);

    return S_OK;
}

HRESULT KG3DPvsPortal::FrameMove()
{
    D3DXMATRIX matWorld = m_Matrix * m_pDependPvs->GetMatrixWorld();

    if (matWorld != m_matWorld)
    {
        m_matWorld = matWorld;

        D3DXVECTOR3 v[4] = {
            D3DXVECTOR3(-0.5f,  0.5f, 0.f),
            D3DXVECTOR3( 0.5f,  0.5f, 0.f),
            D3DXVECTOR3( 0.5f, -0.5f, 0.f),
            D3DXVECTOR3(-0.5f, -0.5f, 0.f)
        };

        D3DXVec3TransformCoordArray(m_vLocalVector, sizeof(D3DXVECTOR3), v, sizeof(D3DXVECTOR3), &m_Matrix,   4);
        D3DXVec3TransformCoordArray(m_vWorldVector, sizeof(D3DXVECTOR3), v, sizeof(D3DXVECTOR3), &m_matWorld, 4);
    }

    return S_OK;
}

HRESULT KG3DPvsPortal::Render(DWORD dwColor /* = 0xff000000 */)
{
    KG3DRenderState R;
    D3DXMATRIX matWorldSave;
    D3DXMATRIX matCoord;

    D3DXVECTOR3 vCenter = D3DXVECTOR3(m_Matrix._41, m_Matrix._42, m_Matrix._43);
    D3DXVECTOR3 vLeft   = D3DXVECTOR3(m_Matrix._11, m_Matrix._12, m_Matrix._11);
    D3DXVECTOR3 vUp     = D3DXVECTOR3(m_Matrix._21, m_Matrix._22, m_Matrix._23);
    D3DXVECTOR3 vFront  = D3DXVECTOR3(m_Matrix._31, m_Matrix._32, m_Matrix._33);

    if (m_nSelected)
        dwColor = 0x55ffff00;
    m_nSelected = FALSE;

    D3DXVec3Normalize(&vLeft, &vLeft);
    D3DXVec3Normalize(&vUp, &vUp);
    D3DXVec3Normalize(&vFront, &vFront);

    GraphicsStruct::RenderState ZFun(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
    R.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    R.SetRenderState(D3DRS_LIGHTING, FALSE);
    R.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    R.SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    R.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    R.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &m_pDependPvs->GetMatrixWorld());
    g_pd3dDevice->SetTexture(0, NULL);
    g_pd3dDevice->SetTexture(1, NULL);

    g_cGraphicsTool.DrawRect(&m_vLocalVector[PORTAL_LEFT_TOP], &m_vLocalVector[PORTAL_RIGHT_TOP],
        &m_vLocalVector[PORTAL_LEFT_B0TTOM], dwColor, NULL);
    g_cGraphicsTool.DrawLine(&m_vLocalVector[PORTAL_LEFT_B0TTOM], &m_vLocalVector[PORTAL_RIGHT_B0TTOM], 0xff0000ff, 0xff0000ff);
    g_cGraphicsTool.DrawLine(&m_vLocalVector[PORTAL_RIGHT_B0TTOM],&m_vLocalVector[PORTAL_RIGHT_TOP],    0xff0000ff, 0xff0000ff);
    g_cGraphicsTool.DrawLine(&m_vLocalVector[PORTAL_RIGHT_TOP],   &m_vLocalVector[PORTAL_LEFT_TOP],     0xff0000ff, 0xff0000ff);
    g_cGraphicsTool.DrawLine(&m_vLocalVector[PORTAL_LEFT_TOP],    &m_vLocalVector[PORTAL_LEFT_B0TTOM],  0xff0000ff, 0xff0000ff);
    g_cGraphicsTool.DrawLine(&vCenter, &(vCenter + vFront * 500.f), 0xff0000ff, 0xff0000ff);
    g_cGraphicsTool.DrawLine(&(vCenter + vFront * 400.f + vLeft * 100.f),
        &(vCenter + vFront * 500.f), 0xff0000ff, 0xff0000ff);
    g_cGraphicsTool.DrawLine(&(vCenter + vFront * 400.f - vLeft * 100.f),
        &(vCenter + vFront * 500.f), 0xff0000ff, 0xff0000ff);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);

    return S_OK;
}

D3DXMATRIX KG3DPvsPortal::GetMatrix()
{
    return m_Matrix;
}

D3DXMATRIX KG3DPvsPortal::GetWorldMatrix()
{
    return m_matWorld;
}

BOOL KG3DPvsPortal::IsVisiable(KG3DCamera* pCamera)
{
    D3DXVECTOR3 vFrontCamera = pCamera->GetCameraFront();
    D3DXVECTOR3 vFrontPortal = D3DXVECTOR3(m_matWorld._31, m_matWorld._32, m_matWorld._33);

    if (D3DXVec3Dot(&vFrontCamera, &vFrontPortal) > 0.f)
        return TRUE;
    return FALSE;
}

HRESULT KG3DPvsPortal::UpdateFrustum(KG3DCamera* pCamera)
{
	if(NULL == pCamera)
		return E_FAIL;
    return m_Frustum.CreateByObjectPVS(*pCamera, m_vWorldVector);
}

LPCTSTR KG3DPvsPortal::GetName()
{
    return m_strName.c_str();
}

KG3DPvsPortal* KG3DPvsPortal::Clone(KG3DRepresentObjectPVS* pvs)
{
    KG3DPvsPortal* ptl = new KG3DPvsPortal(m_pDependPvs);

    memcpy(ptl->m_vLocalVector, m_vLocalVector, sizeof(m_vLocalVector));
    memcpy(ptl->m_vWorldVector, m_vWorldVector, sizeof(m_vWorldVector));
    memcpy(ptl->m_matWorld, m_matWorld, sizeof(m_matWorld));
    memcpy(ptl->m_Matrix, m_Matrix, sizeof(m_Matrix));
    ASSERT_FLOAT_IS_VALIED(ptl->m_Matrix._11);

    ptl->m_strName = m_strName;
    if (pvs)
        ptl->m_pDependPvs = pvs;
    else
        ptl->m_pDependPvs = m_pDependPvs;
    ptl->m_Frustum = m_Frustum;

    return ptl;
}

KG3DRepresentObjectPVS::KG3DRepresentObjectPVS()
{
	m_bRenderBorderModel = TRUE;
	m_bRenderPortal = TRUE;
    m_dwVersion = PVS_FILE_VERSION_2;
    
    for (int i = 0; i < SHELL_COUNT; ++i)
        m_bShelllShowFlag[i] = TRUE;
	m_dwLightmapInUse = 0;
}

KG3DRepresentObjectPVS::~KG3DRepresentObjectPVS()
{
    UnInit();
}

HRESULT KG3DRepresentObjectPVS::CalcFrustum(const TypeFrustums& vInputFrustum, DWORD dwLookMode)
{
    if (dwLookMode == LOOK_IN_TO_OUT)
    {
		TypeFrustums frustumsPreviousCull;	//?m_vParentFrustum??????
        //m_vParentFrustum.clear();

        //m_vFrustum = vInputFrustum;
		SetFrustums(vInputFrustum);

		const TypeFrustums& myFrumstums = GetFrustums();
        if (myFrumstums.size() > 0)
            KG3DSceneSpaceNode::SetTopVisableNode(this);

        for (size_t v = 0; v < vInputFrustum.size(); ++v)
        {
            for (list<KG3DPvsPortal*>::iterator i = m_listPortal.begin(); 
                 i != m_listPortal.end(); ++i)
            {
                KG3DPvsPortal* pPortal = (*i);
				_ASSERTE(NULL != pPortal);
				
                KG3DFrustum tempFrustum = vInputFrustum[v]; 
              
                if (tempFrustum.CullByPortal(*pPortal, LOOK_IN_TO_OUT))
					frustumsPreviousCull.push_back(tempFrustum);
            }
        }

        if (this->GetParent())
            this->GetParent()->CalcFrustum(/*m_vParentFrustum*/frustumsPreviousCull, dwLookMode);
    }
    else
    {
        if (vInputFrustum.size() <= 0)
            return E_FAIL;

		ClearFrustums();

        for (size_t v = 0; v < vInputFrustum.size(); ++v)
        {
            for (list<KG3DPvsPortal*>::iterator i = m_listPortal.begin(); 
                i != m_listPortal.end(); ++i)
            {
                KG3DPvsPortal* pPortal = (*i);
				_ASSERTE(NULL != pPortal);
                KG3DFrustum Furstum = vInputFrustum[v]; 

                if (Furstum.CullByPortal(*pPortal, LOOK_OUT_TO_IN))
                    InsertFrustum(Furstum);
            }
        }

        /*for (set<KG3DSceneSpaceNode*>::iterator i = m_setSpaceNodeChild.begin();
             i != m_setSpaceNodeChild.end(); ++i)
        {
            (*i)->m_vFrustum.clear();
        }*/
		KG3DSceneSpaceNode::TypeAccessProxy& accessProxy = this->GetChildrenAccessProxy();
		for(accessProxy.Begin(); ! accessProxy.IsEnd(); accessProxy.StepForward())
		{
			KG3DSceneSpaceNode* p = accessProxy.GetCur();
			p->ClearFrustums();
		}
    }

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::Copy(KG3DRepresentObject* pSrcObject)
{
    if (FAILED(KG3DRepresentObject::Copy(pSrcObject)))
        return E_FAIL;

    if (pSrcObject->GetType() != REPRESENTOBJECT_PVS)
        return E_FAIL;

    KG3DRepresentObjectPVS* pvs = static_cast<KG3DRepresentObjectPVS*>(pSrcObject);

    for (set<KG3DRepresentObject*>::iterator i = pvs->m_setObjectInside.begin(); i != pvs->m_setObjectInside.end(); ++i)
    {
        KG3DRepresentObject* obj = NULL;
        
        if (SUCCEEDED(g_GetObjectTable().GetCopy(&obj, *i)))
            AddObject(obj, g_cGraphicsTool.GetCurScene());
    }

    for (list<KG3DPvsPortal*>::iterator i = pvs->m_listPortal.begin(); i != pvs->m_listPortal.end(); ++i)
    {
        m_listPortal.push_back((*i)->Clone(this));
    }

    m_bRenderBorderModel = pvs->m_bRenderBorderModel;
    m_bRenderPortal = pvs->m_bRenderPortal;

    m_setObjectVisiable.clear();

    return S_OK;

}

HRESULT KG3DRepresentObjectPVS::SetOutsideBorderModel(LPCTSTR fileName)
{
    if (!fileName)
        return E_FAIL;

    return SetModel(0, fileName, TEXT(""), TEXT(""), TRUE);

}

HRESULT KG3DRepresentObjectPVS::SetInsideBorderModel(LPCTSTR fileName)
{
    if (!fileName)
        return E_FAIL;

    return SetModel(1, fileName, TEXT(""), TEXT(""), TRUE);

}

namespace {

    const char* SafeGetRetPath(const char filePath[]) 
    {
        char rootPath[MAX_PATH];
        g_GetRootPath(rootPath);

        char* pStr = strstr(const_cast<char*>(filePath), rootPath);
        size_t offset = 0;

        if (pStr == filePath) 
        {
            offset = strlen(rootPath);            
            if (filePath[offset] == '\\')
                ++offset;
        }

        return filePath + offset;
    }
}

HRESULT KG3DRepresentObjectPVS::ProcessModel(KG3DModel* pModel,ModelExtData& vData)
{
	if(vData.nLightmapID==-1)
	{
		;
	}
	else
	{
		TCHAR strFile[MAX_PATH]; 
		TCHAR strDrive[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strExt[MAX_PATH];
		_splitpath_s(m_scName.c_str(), 
			strDrive, 
			MAX_PATH,
			strPath, 
			MAX_PATH, 
			strFile, 
			MAX_PATH, 
			strExt,
			MAX_PATH);
		TCHAR strLightMapName[MAX_PATH];
		sprintf_s(strLightMapName, 
			MAX_PATH,
			"%s%s%sLightmap\\%d.dds", 
			strDrive,
			strPath,strFile,
			vData.nLightmapID);

		pModel->EnableLightmap(TRUE,0,FALSE,strLightMapName);
		pModel->m_eRuntimeShaderType = (KG3DDefaultShaderType)vData.nShaderType;
	}
	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::_ReadObjectsInside(IFile* pFile,DWORD dwOption)
{
	/* load inside objects, the object may be is pvs object too */
	char szPath[MAX_PATH];

	size_t objNum = 0;
	pFile->Read(&objNum, sizeof(objNum));

	for (size_t i = 0; i < objNum; ++i)
	{
		KG3DRepresentObject* obj = NULL;

		D3DXVECTOR3     ScalingCenter;
		D3DXQUATERNION  ScalingRotation;
		D3DXVECTOR3     Scaling;
		D3DXVECTOR3     RotationCenter;
		D3DXQUATERNION  Rotation;
		D3DXVECTOR3     Translation;
		AABBOX          aabox;
		DWORD           dwObjType;

		pFile->Read(&dwObjType, sizeof(dwObjType));

		switch (dwObjType)
		{
		case REPRESENTOBJECT_PVS :
			{
				pFile->Read(szPath, sizeof(szPath));
				g_cEngineManager.BuildKG3DRepresentObject(szPath, dwObjType, dwOption, 0, (IEKG3DRepresentObject**)&obj);
			}
			break;
		case REPRESENTOBJECT_SET :
			{
				pFile->Read(szPath, sizeof(szPath));
				g_cEngineManager.BuildKG3DRepresentObject(szPath, dwObjType, dwOption, 0, (IEKG3DRepresentObject**)&obj);
				KG3DModel* pModel = NULL;
				obj->GetModel(pModel,0,FALSE);

				ModelExtData vData;
				_LoadModelExtData(vData,pFile);
				ProcessModel(pModel,vData);
			}
			break;
		case REPRESENTOBJECT_DEFAULT :
			{
				/* is normal object */

				typedef char _Path[MAX_PATH];

				/*
				char  szNames[PVS_SUOBJ_MAX_MDL_NUM][MAX_PATH];
				char  szAnins[PVS_SUOBJ_MAX_MDL_NUM][MAX_PATH];
				*/

				DWORD dwColor[PVS_SUOBJ_MAX_MDL_NUM];
				DWORD dwType[PVS_SUOBJ_MAX_MDL_NUM];
				DWORD dwCount = 0;

				pFile->Read(&dwCount, sizeof(dwCount));

				_Path* pszNames = (_Path*)malloc(sizeof(_Path) * dwCount);
				_Path* pszAnins = (_Path*)malloc(sizeof(_Path) * dwCount);

				ASSERT(pszNames);
				ASSERT(pszAnins);

				vector<ModelExtData>vecModelExtData;
				vector<KG3DModelLightExtData>vecLightExtData;
				vecModelExtData.reserve(dwCount);
				vecLightExtData.reserve(dwCount);

				for (DWORD i = 0; i < dwCount; ++i)
				{
					pFile->Read(pszNames[i], sizeof(pszNames[i]));

					pszAnins[i][0] = 0;
					if (m_dwVersion > PVS_FILE_VERSION_1)
						pFile->Read(pszAnins[i], sizeof(pszAnins[i]));

					TypeInfo* pInfo = NULL;
					g_cModelTable.GetTypeInfoByFileName(&pInfo, pszNames[i]);

					if (pInfo)
						dwType[i] = pInfo->dwType;
					else
						dwType[i] = MESHTYPE_DEFAULT;

					//////////////////////////////////////////////////////////////////////////
					ModelExtData vModelData;
					KG3DModelLightExtData vLightData;

					_LoadModelExtData(vModelData,pFile);

					switch (dwType[i])
					{
					case MESHTYPE_POINTLIGHT :
						{
							if(m_dwVersion<PVS_FILE_VERSION_3)
								pFile->Read(&dwColor[i], sizeof(dwColor[i]));
							else
							{
								pFile->Read(&vLightData,sizeof(KG3DModelLightExtData));
							}
						}
						break;
					default :
						break;
					}

					vecModelExtData.push_back(vModelData);
					vecLightExtData.push_back(vLightData);
				}

				if (FAILED(g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_DEFAULT, (IKG3DResourceBase**)&obj)))
				{
					if (pszNames)
						free(pszNames);

					if (pszAnins)
						free(pszAnins);

					break;
				}

				for (DWORD i = 0; i < dwCount; ++i)
				{
					KGLogPrintf(KGLOG_ERR,"%s",pszNames[i]);

					obj->SetModel(i, pszNames[i], TEXT(""), pszAnins[i], (dwOption == OBJLOAD_NONE ? TRUE : FALSE));

					KG3DModel* pModel = obj->GetModel(i);

					switch (dwType[i])
					{
					case MESHTYPE_POINTLIGHT :
						{


							if(m_dwVersion<PVS_FILE_VERSION_3)
							{
								D3DCOLORVALUE ColorValue;

								ColorValue.a = (float)((dwColor[i] >> 24) & 0xff) / 255.f;
								ColorValue.r = (float)((dwColor[i] >> 16) & 0xff) / 255.f;
								ColorValue.g = (float)((dwColor[i] >> 8) & 0xff) / 255.f;
								ColorValue.b = (float)((dwColor[i] >> 0) & 0xff) / 255.f;

								if (pModel)
									pModel->SetModelColor(0, ColorValue);
							}
							else
							{
								KG3DModelPointLight* pLight = (KG3DModelPointLight*)(pModel);
								pLight->SetExtData(vecLightExtData[i]);
							}
						}
						break;
					default :
						ProcessModel(pModel,vecModelExtData[i]);
						break;
					}
				}

				if (pszNames)
					free(pszNames);

				if (pszAnins)
					free(pszAnins);
			}
			break;

		default :
			ASSERT(false);
			break;
		}

		pFile->Read(&ScalingCenter, sizeof(ScalingCenter));
		pFile->Read(&ScalingRotation, sizeof(ScalingRotation));
		pFile->Read(&Scaling, sizeof(Scaling));
		pFile->Read(&RotationCenter, sizeof(RotationCenter));
		pFile->Read(&Rotation, sizeof(Rotation));
		pFile->Read(&Translation, sizeof(Translation));
		pFile->Read(&aabox, sizeof(aabox));

		if (obj)
		{
			AddObject(obj, g_cGraphicsTool.GetCurScene());

			obj->SetScalingCenter(&ScalingCenter);
			obj->SetScalingRotation(&ScalingRotation);
			obj->SetScaling(&Scaling);
			obj->SetRotationCenter(&RotationCenter);
			obj->SetRotation(&Rotation);
			obj->SetTranslation(&Translation);

			g_LoadPostProcessor.InputRepresentObject(obj);
		}
	}


	/* load pvs protal */

	size_t ptlNum = 0;
	pFile->Read(&ptlNum, sizeof(ptlNum));

	for (size_t i = 0; i < ptlNum; ++i)
	{
		char szName[PVS_PORTAL_NAME_MAX_LEN];
		pFile->Read(szName, sizeof(szName));

		D3DXMATRIX matPose;
		pFile->Read(matPose.m, sizeof(matPose.m));

		KG3DPvsPortal* ptl = NULL;
		AddPortal(ptl, matPose);

		if (ptl)
			ptl->FrameMove();
	}

	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::_LoadPvsFileVersion3(IFile* pFile,DWORD dwOption)
{
	/* load border, the frist model of the pvs object is it's ouside border
	and the second model is it's inside border */
	char szPath[MAX_PATH];
	ModelExtData vDataIn,vDataOut;

	pFile->Read(szPath, sizeof(szPath));
	_LoadModelExtData(vDataOut,pFile);
	SetModel(0, szPath, TEXT(""), TEXT(""), (dwOption == OBJLOAD_NONE ? TRUE : FALSE));


	pFile->Read(szPath, sizeof(szPath));
	_LoadModelExtData(vDataIn,pFile);
	SetModel(1, szPath, TEXT(""), TEXT(""), (dwOption == OBJLOAD_NONE ? TRUE : FALSE));

	//////////////////////////////////////////////////////////////////////////
	pFile->Read(&m_dwLightmapInUse,sizeof(DWORD));//加入光照图的个数

	KG3DModel* pOutsideBorderModel = static_cast<KG3DModel*>(GetOutsideBorderModel());
	KG3DModel* pInsideBroderModel  = static_cast<KG3DModel*>(GetInsideBorderModel());
	
	if(pOutsideBorderModel)
		ProcessModel(pOutsideBorderModel,vDataOut);
	if(pInsideBroderModel)
		ProcessModel(pInsideBroderModel ,vDataIn );

	_ReadObjectsInside(pFile,dwOption);

	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::_LoadPvsFileVersion2(IFile* pFile,DWORD dwOption)
{
	/* load border, the frist model of the pvs object is it's ouside border
	and the second model is it's inside border */
	char szPath[MAX_PATH];

	pFile->Read(szPath, sizeof(szPath));
	SetModel(0, szPath, TEXT(""), TEXT(""), (dwOption == OBJLOAD_NONE ? TRUE : FALSE));

	pFile->Read(szPath, sizeof(szPath));
	SetModel(1, szPath, TEXT(""), TEXT(""), (dwOption == OBJLOAD_NONE ? TRUE : FALSE));

	_ReadObjectsInside(pFile,dwOption);

	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::LoadPvsFile(LPCTSTR szFileName, DWORD dwOption)
{
    ASSERT(m_vecModel.size() == 0);

    IFile* pFile = g_OpenFile(szFileName, false, false);
    if (!pFile)
        return E_FAIL;

	m_scName = szFileName;

    /* read version */

    pFile->Read(&m_dwVersion, sizeof(m_dwVersion));

	if(m_dwVersion >= PVS_FILE_VERSION_3)
	{
		_LoadPvsFileVersion3(pFile,dwOption);
	}
	else
	{
		_LoadPvsFileVersion2(pFile,dwOption);
	}


    pFile->Close();
    SAFE_RELEASE(pFile);

    m_bSleep = FALSE;
    FrameMove();

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::_SaveModelExtData(KG3DModel* pModel,IFile* pFile)
{
	if(m_dwVersion>=PVS_FILE_VERSION_3)
	{
		ModelExtData vData;

		if(pModel && pModel->m_lpLightMap)
		{	
			vData.nLightmapID = pModel->m_lpLightMap->m_nKey;
			vData.nShaderType = pModel->m_eRuntimeShaderType;
		}
		pFile->Write(&vData,sizeof(ModelExtData));
	}
	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::_LoadModelExtData(ModelExtData& vData,IFile* pFile)
{
	if(m_dwVersion>=PVS_FILE_VERSION_3)
	{
		pFile->Read(&vData,sizeof(ModelExtData));
	}
	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::SavePvsFile(LPCTSTR szFileName)
{
    IFile* pFile = g_CreateFile(szFileName);//g_OpenFile(szFileName, false, true);

    if (!pFile)
        return E_FAIL;

	vector<KG3DModel*>vecModelWithLightmap;
	ProcessLightmapKeyForModels(&vecModelWithLightmap);

    D3DXMATRIX matInv;
    D3DXMatrixInverse(&matInv, NULL, &m_matWorld);


    /* save version */

    m_dwVersion = PVS_FILE_VERSION_3;//强制使用新格式
    pFile->Write(&m_dwVersion, sizeof(m_dwVersion));


    /* save border information, the frist model of the pvs object is it's ouside border
       and the second model is it's inside border */

    KG3DModel* pOutsideBorderModel = static_cast<KG3DModel*>(GetOutsideBorderModel());
    KG3DModel* pInsideBroderModel  = static_cast<KG3DModel*>(GetInsideBorderModel());

    char szPath[MAX_PATH];

    szPath[0] = 0;
    if (pOutsideBorderModel)
        strncpy(szPath, pOutsideBorderModel->m_scName.c_str(), pOutsideBorderModel->m_scName.length() + 1);    
    pFile->Write(SafeGetRetPath(szPath), sizeof(szPath));
	_SaveModelExtData(pOutsideBorderModel,pFile);

    szPath[0] = 0;
    if (pInsideBroderModel)
        strncpy(szPath, pInsideBroderModel->m_scName.c_str(), pInsideBroderModel->m_scName.length() + 1);    
    pFile->Write(SafeGetRetPath(szPath), sizeof(szPath));
	_SaveModelExtData(pInsideBroderModel,pFile);


	pFile->Write(&m_dwLightmapInUse,sizeof(DWORD));
	//////////////////////////////////////////////////////////////////////////
    /* save inside objects, the object may be is pvs object too */

    size_t objNum = m_setObjectInside.size();
    pFile->Write(&objNum, sizeof(objNum));

    for (set<KG3DRepresentObject*>::iterator i = m_setObjectInside.begin();
        i != m_setObjectInside.end(); ++i)
    {
        KG3DRepresentObject* obj = (*i);
        
        ASSERT(obj);
        
        DWORD  dwObjType = obj->GetType();
        AABBOX aabox;    

        pFile->Write(&dwObjType, sizeof(dwObjType));

        switch (dwObjType)
        {
        case REPRESENTOBJECT_PVS :
            {
                if (!stricmp(obj->m_scName.c_str(), m_scName.c_str()))
                {
                    string subName = m_scName.substr(0, m_scName.length() - strlen(".pvs"));
                    char tempName[MAX_PATH];
                    sprintf(tempName, "%s-%u.pvs", subName.c_str(), GetTickCount());
                    obj->m_scName = tempName;
                    static_cast<KG3DRepresentObjectPVS*>(obj)->SavePvsFile(obj->m_scName.c_str());
                }
            }
			break;
        case REPRESENTOBJECT_SET :
            {
                strncpy(szPath, obj->m_scName.c_str(), obj->m_scName.length() + 1);
                pFile->Write(SafeGetRetPath(szPath), sizeof(szPath));
				KG3DModel* pModel = NULL;
				obj->GetModel(pModel,0,FALSE);
				_SaveModelExtData(pModel,pFile);
            }
            break;
        case REPRESENTOBJECT_DEFAULT :
            {
                DWORD dwCount = obj->GetModelCount();
                
                if (dwCount > PVS_SUOBJ_MAX_MDL_NUM)
                    dwCount = PVS_SUOBJ_MAX_MDL_NUM;

                pFile->Write(&dwCount, sizeof(dwCount));

                for (DWORD i = 0; i < dwCount; ++i)
                {
                    KG3DModel* pModel = obj->GetModel(0);
                    szPath[0] = 0;
                    if (pModel)
                        strncpy(szPath, pModel->m_scName.c_str(), pModel->m_scName.length() + 1);  
                    pFile->Write(SafeGetRetPath(szPath), sizeof(szPath));
                    szPath[0] = 0;
                    if (pModel)
                        pModel->GetAnimationFileName(szPath, sizeof(szPath));
                    pFile->Write(SafeGetRetPath(szPath), sizeof(szPath));
					//////////////////////////////////////////////////////////////////////////
					_SaveModelExtData(pModel,pFile);

					if (pModel)
					{
						switch (pModel->GetType())
						{
						case MESHTYPE_POINTLIGHT :
							{
								KG3DModelLightExtData vData;

								KG3DModelPointLight* pLight = (KG3DModelPointLight*)pModel;
								pLight->GetExtData(vData);

								pFile->Write(&vData, sizeof(KG3DModelLightExtData));
							}
							break;
						default :
							break;
						}
					}
                }
            }
            break;
        default :
            ASSERT(false);
            break;
        }

        aabox = obj->m_BBox;
        aabox.Transform(matInv);

        pFile->Write(&obj->m_ScalingCenter, sizeof(obj->m_ScalingCenter));
        pFile->Write(&obj->m_ScalingRotation, sizeof(obj->m_ScalingRotation));
        pFile->Write(&obj->m_Scaling, sizeof(obj->m_Scaling));
        pFile->Write(&obj->m_RotationCenter, sizeof(obj->m_RotationCenter));
        pFile->Write(&obj->m_Rotation, sizeof(obj->m_Rotation));
        pFile->Write(&obj->m_Translation, sizeof(obj->m_Translation));
        pFile->Write(&aabox, sizeof(aabox));
    }


    /* save protals */

    size_t ptlNum = m_listPortal.size();
    pFile->Write(&ptlNum, sizeof(ptlNum));

    for (list<KG3DPvsPortal*>::iterator i = m_listPortal.begin();
        i != m_listPortal.end(); ++i)
    {
        KG3DPvsPortal* ptl = *i;

        ASSERT(ptl);

        char szName[PVS_PORTAL_NAME_MAX_LEN];
        szName[0] = 0;
        strncpy(szName, ptl->GetName(), PVS_PORTAL_NAME_MAX_LEN - 1);

        pFile->Write(szName, sizeof(szName));
        pFile->Write(&ptl->m_Matrix.m, sizeof(ptl->m_Matrix.m));

    }

    pFile->Close();
    SAFE_RELEASE(pFile);

    m_scName = szFileName;

    //m_PvsOCTree.Create(this);
	//////////////////////////////////////////////////////////////////////////
	{
		TCHAR strFile[MAX_PATH]; 
		TCHAR strDrive[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strExt[MAX_PATH];
		_splitpath_s(szFileName, 
			strDrive, 
			MAX_PATH,
			strPath, 
			MAX_PATH, 
			strFile, 
			MAX_PATH, 
			strExt,
			MAX_PATH);
		TCHAR strLightMapName[MAX_PATH];
		sprintf_s(strLightMapName, 
			MAX_PATH,
			"%s%s%s%sLightmap", 
			g_Def_WorkDirectory,
			strDrive,
			strPath,
			strFile);
		mkdir(strLightMapName);

		for (size_t i=0;i<vecModelWithLightmap.size();i++)
		{
			KG3DModel* pModel = vecModelWithLightmap[i];
			if(pModel && pModel->m_lpLightMap)
			{
				sprintf_s(strLightMapName, 
					MAX_PATH,
					"%s%s%s%sLightmap\\%d.dds", 
					g_Def_WorkDirectory,
					strDrive,
					strPath,strFile,
					pModel->m_lpLightMap->m_nKey);

				D3DXSaveTextureToFile(strLightMapName,D3DXIFF_DDS,pModel->m_lpLightMap->GetLightMap(),NULL);
			}
		}
	}
    return S_OK;
}


HRESULT KG3DRepresentObjectPVS::SaveToInformationBuffer(/*DWORD& dwType,DWORD& dwLength,*/KG3DMemoryFile* pFile)
{
    if (m_scName == "")
    {
        m_scName = "temp.pvs";
        SavePvsFile(m_scName.c_str());
    }

    char szName[MAX_PATH];
    strncpy(szName, m_scName.c_str(), sizeof(szName));
    pFile->write(szName, sizeof(szName));

    return S_OK;

}

HRESULT KG3DRepresentObjectPVS::LoadFromInformationBuffer(/*DWORD dwType,DWORD dwLength,*/const KG3DMemoryFile* pFile,DWORD dwOption)
{
    char szName[MAX_PATH];

    pFile->read(&szName, sizeof(szName));
    m_scName = szName;

    LoadPvsFile(szName, dwOption);
    ComputeBBox();

    return S_OK;
}


BOOL KG3DRepresentObjectPVS::IsPositionInSetDirection(const D3DXVECTOR3& Position, const D3DXVECTOR3& Direction)
{
	D3DXVECTOR3 Inter[256];

    IEKG3DModel* pCheckModel = GetInsideBorderModel();

    if (!pCheckModel)
        pCheckModel = GetOutsideBorderModel();

	DWORD dwCount = 0;

    for (list<KG3DPvsPortal*>::iterator i = m_listPortal.begin(); i != m_listPortal.end(); ++i)
    {
        KG3DPvsPortal* pPortal = (*i);
        D3DXVECTOR3 vDirPortal = D3DXVECTOR3(pPortal->m_Matrix._31, pPortal->m_Matrix._32, pPortal->m_Matrix._33);
        D3DXVec3TransformNormal(&vDirPortal, &vDirPortal, &m_matWorld);

        if (D3DXVec3Dot(&Direction, &vDirPortal) > 0.f)
            continue;

        if (D3DXIntersectTri(&pPortal->m_vWorldVector[0], &pPortal->m_vWorldVector[1], &pPortal->m_vWorldVector[2],
            &Position, &Direction, NULL, NULL, NULL))
            return TRUE;

        if (D3DXIntersectTri(&pPortal->m_vWorldVector[0], &pPortal->m_vWorldVector[2], &pPortal->m_vWorldVector[3],
            &Position, &Direction, NULL, NULL, NULL))
            return TRUE;
    }

    if (pCheckModel)
    {
        KG3DModel* pCheck = static_cast<KG3DModel*>(pCheckModel);

        if (!pCheck)
            return FALSE;

        if (!(pCheck->GetRayIntersect(dwCount, Inter,256, Position, Direction)))
            return FALSE;

        dwCount = (dwCount <= 256 ? dwCount : 256);

        for (DWORD i = 0; i < dwCount; i++)
        {
            D3DXVECTOR3 vDir = Inter[i] - Position;
            float fdot = D3DXVec3Dot(&vDir, &Direction);

            if (fdot >= 0)
                return TRUE;
        }
    }

    /*pCheckModel = GetInsideBorderModel();

    if (!pCheckModel)
        pCheckModel = GetOutsideBorderModel();

    if (!pCheckModel)
        return FALSE;

    if (!static_cast<KG3DModel*>(pCheckModel)->GetRayIntersect(dwCount, Inter,256, Position, Direction))
        return FALSE;

    dwCount = (dwCount <= 256 ? dwCount : 256);

	for (DWORD i = 0; i < dwCount; i++)
	{
		D3DXVECTOR3 vDir = Inter[i] - Position;
		float fdot = D3DXVec3Dot(&vDir, &Direction);

		if (fdot >= 0)
            return TRUE;
        
	}*/

	return FALSE;
}

BOOL KG3DRepresentObjectPVS::IsPositionInSet(D3DXVECTOR3& vPos)
{
    D3DXVECTOR3 vDir;

    /* check up */
    vDir = D3DXVECTOR3(0.f, 1.f, 0.f);
    if (!IsPositionInSetDirection(vPos, vDir))
        return FALSE;

    /* check floor */
    vDir = D3DXVECTOR3(0.f, -1.f, 0.f);
    if (!IsPositionInSetDirection(vPos, vDir))
        return FALSE;

    /* check right */
    vDir = D3DXVECTOR3(1.f, 0.f, 0.f);
    if (!IsPositionInSetDirection(vPos, vDir))
        return FALSE;

    /* check left */
    vDir = D3DXVECTOR3(-1.f, 0.f, 0.f);
    if (!IsPositionInSetDirection(vPos, vDir))
        return FALSE;

    /* check front */
    vDir = D3DXVECTOR3(0.f, 0.f, 1.f);
    if (!IsPositionInSetDirection(vPos, vDir))
        return FALSE;

    /* check back */
    vDir = D3DXVECTOR3(0.f, 0.f, -1.f);
    if (!IsPositionInSetDirection(vPos, vDir))
        return FALSE;

    return TRUE;
}

HRESULT KG3DRepresentObjectPVS::Init()
{
	return KG3DRepresentObject::Init();
}

HRESULT KG3DRepresentObjectPVS::UnInit()
{

    KG3DRepresentObject::UnInit();

    DeleteAllPortal();

    for (set<KG3DRepresentObject*>::iterator i = m_setObjectInside.begin(); i != m_setObjectInside.end(); ++i)
    {
        KG3DRepresentObject *pObject = *i;
        if (pObject)
            pObject->SetParentMatrix(NULL);
        SAFE_RELEASE(*i);
    }
    m_setObjectVisiable.clear();           
    m_setObjectInside.clear();

	{
		for (set<KG3DEntityObjectShell*>::iterator i = m_setEntityShellInside.begin(); i != m_setEntityShellInside.end(); ++i)
		{
			KG3DEntityObjectShell* pShell = *i;
			set<IKG3DSceneEntityContainer*>::iterator iFind = pShell->m_setRefrenceContainer.find(this);
			if(iFind != pShell->m_setRefrenceContainer.end())
			{
				pShell->m_setRefrenceContainer.erase(iFind);
			}
		}
		m_setEntityShellInside.clear();
	}

	set<KG3DModel*>setModelInsideCopy = m_setModelInside;
    for (set<KG3DModel*>::iterator i = setModelInsideCopy.begin(); i != setModelInsideCopy.end(); ++i)
	{
		KG3DModel* pModel = *i;
		pModel->LeavePvs(FALSE);
        SAFE_RELEASE(*i);
	}
    m_setModelInside.clear();

    m_bRenderBorderModel = TRUE;
    m_bRenderPortal = TRUE;

	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::Clear(BOOL bClearEntity)
{    
    for (size_t i = 0; i < m_vecModel.size(); ++i)
    {
        if (m_vecModel[i])
            m_vecModel[i]->SetParentObjMatrix(NULL);
        SAFE_RELEASE(m_vecModel[i]);
    }
    m_vecModel.clear();

    DeleteAllPortal();

    for (set<KG3DRepresentObject*>::iterator i = m_setObjectInside.begin(); i != m_setObjectInside.end(); ++i)
    {
        KG3DRepresentObject *pObject = *i;
        if (pObject)
            pObject->SetParentMatrix(NULL);
            
        SAFE_RELEASE(*i);
    }

    m_setObjectVisiable.clear();           
    m_setObjectInside.clear();

	if(bClearEntity)
	{
		for (set<KG3DEntityObjectShell*>::iterator i = m_setEntityShellInside.begin(); i != m_setEntityShellInside.end(); ++i)
		{
			KG3DEntityObjectShell* pShell = *i;
			set<IKG3DSceneEntityContainer*>::iterator iFind = pShell->m_setRefrenceContainer.find(this);
			if(iFind != pShell->m_setRefrenceContainer.end())
			{
				pShell->m_setRefrenceContainer.erase(iFind);
			}
		}
		m_setEntityShellInside.clear();
	}

	set<KG3DModel*>setModelInsideCopy = m_setModelInside;
    for (set<KG3DModel*>::iterator i = setModelInsideCopy.begin(); i != setModelInsideCopy.end(); ++i)
	{
        SAFE_RELEASE(*i);
		//(*i)->LeavePvs();
	}
    m_setModelInside.clear();

    return S_OK;
}

namespace {

    struct _Compare 
    {
        _Compare(const D3DXVECTOR3& vCam)
            : vCamPos(vCam)
        {
        }
        
        bool operator()(KG3DPvsPortal* left, KG3DPvsPortal* rigt)
        {
            D3DXVECTOR3 left_pos = *(D3DXVECTOR3*)&(left->GetWorldMatrix()._41);
            D3DXVECTOR3 rigt_pos = *(D3DXVECTOR3*)&(rigt->GetWorldMatrix()._41);
            return D3DXVec3LengthSq(&(left_pos - vCamPos)) > D3DXVec3LengthSq(&(rigt_pos - vCamPos));
        }

        D3DXVECTOR3 vCamPos;
    };

}

HRESULT KG3DRepresentObjectPVS::Render(DWORD dwOption)
{
    /*
     * render portal
     */

    if (!g_bClient && m_bRenderPortal)
    {
        m_listPortal.sort(_Compare(g_cGraphicsTool.GetCamera()->GetPosition()));

        list<KG3DPvsPortal*>::iterator icur = m_listPortal.begin();
        list<KG3DPvsPortal*>::iterator iend = m_listPortal.end();
  
        while (icur != iend)
        {
            (*icur)->Render(0x550000ff);
            ++icur;
        }

        IEKG3DSceneOutputWnd* pRenderWnd = NULL;
        g_cGraphicsTool.GetScene()->GetCurRenderWnd(&pRenderWnd);
        ASSERT(pRenderWnd);

        if (!pRenderWnd->GetIECamera()->IsPerspective())
        {
            KG3DRenderState R;
            DWORD dwState;
            
            R.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
            g_pd3dDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwState);
            g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			const TypeFrustums& myFrustums = GetFrustums();
            for (size_t i = 0; i < myFrustums.size(); ++i)     
                g_cGraphicsTool.DrawFrustum(&myFrustums[i], 0x880000ff, 0xffffffff & ~1);

            g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
            R.SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
            
            for (size_t i = 0; i < myFrustums.size(); ++i)    
                g_cGraphicsTool.DrawFrustum(&myFrustums[i], 0x880000ff, 0xffffffff & ~1);
            
            g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwState);
        }
    }

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::RenderHierarchy(BOOL bCull /* = TRUE */, KG3DScenePointLightRender* pRender)
{
    set<KG3DRepresentObject*>* pRenderSet = &m_setObjectInside;

    if (bCull)
        pRenderSet = &m_setObjectVisiable;

    ASSERT(pRenderSet);

    for (int i = 0; i < (int)m_vecModel.size(); ++i)
    {
        if (m_vecModel[i] && m_bShelllShowFlag[i])
        {
            if (pRender)
                pRender->FindRefrenceLightForModel(m_vecModel[i]);
            m_vecModel[i]->Render(0, NULL);
        }
    }

    set<KG3DRepresentObject*>::iterator icurs = pRenderSet->begin();
    set<KG3DRepresentObject*>::iterator iends = pRenderSet->end();

    while (icurs != iends)
    {
        if (!(*icurs)->HasSfx())
            (*icurs)->RenderHierarchy(bCull, pRender);
        ++icurs;
    }

    icurs = pRenderSet->begin();
    iends = pRenderSet->end();

    while (icurs != iends)
    {
        if ((*icurs)->HasSfx())
            (*icurs)->RenderHierarchy(bCull, pRender);
        ++icurs;
    }

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::FrameMove()
{
    BOOL bSleep = m_bSleep;

    KG3DRepresentObject::FrameMove();

    set<KG3DRepresentObject*>::iterator icur = m_setObjectInside.begin();
    set<KG3DRepresentObject*>::iterator iend = m_setObjectInside.end();

    while (icur != iend)
    {
        if (!bSleep)
            (*icur)->m_bSleep = FALSE;
        (*icur)->FrameMove();
        ++icur;
    }
    

    list<KG3DPvsPortal*>::iterator itorl = m_listPortal.begin();
    list<KG3DPvsPortal*>::iterator iendl = m_listPortal.end();
    
    while (itorl != iendl)
    {
        (*itorl)->FrameMove();
        ++itorl;
    }

	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::AddPortal(KG3DPvsPortal*& pPort, const D3DXMATRIX& matPose)
{
    static int index = 0;
    pPort = new(nothrow) KG3DPvsPortal(this);

    if (!pPort)
        return E_FAIL;

    TCHAR szPortalName[128];
    wsprintf(szPortalName, TEXT("Portal-%02u"),index++);
    pPort->m_strName = szPortalName;
    m_listPortal.push_back(pPort);

    pPort->UpdateByMatrix(matPose);

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::AddPortal(KG3DPvsPortal*& pPort, 
                                          D3DXVECTOR3     vPos,
                                          D3DXVECTOR3     vDir, 
                                          float           fHeight, 
                                          float           fWidth
                                          )
{
	pPort = new(nothrow) KG3DPvsPortal(this);

    if (!pPort)
        return E_FAIL;

    TCHAR szPortalName[128];
    wsprintf(szPortalName, TEXT("Portal-%02u"), m_listPortal.size());
    pPort->m_strName = szPortalName;
	m_listPortal.push_back(pPort);

    D3DXMATRIX matPose;
    D3DXMATRIX matScal;
    D3DXMATRIX matMove;

    D3DXMatrixLookAtLH(&matPose, &D3DXVECTOR3(0.f, 0.f, 0.f), &vDir, &D3DXVECTOR3(0.f, 1.f, 0.f));
    D3DXMatrixInverse(&matPose, NULL, &matPose);
    D3DXMatrixScaling(&matScal, fWidth, fHeight, 1.f);
    D3DXMatrixTranslation(&matMove, vPos.x, vPos.y, vPos.z);

    matPose = matScal * matPose * matMove;

    pPort->UpdateByMatrix(matPose);

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::DeletePortal(KG3DPvsPortal* pPort)
{
	m_listPortal.remove(pPort);
	SAFE_DELETE(pPort);
	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::DeleteAllPortal()
{
	list<KG3DPvsPortal*>::iterator itor = m_listPortal.begin();
	list<KG3DPvsPortal*>::iterator iend = m_listPortal.end();

	while (itor != iend)
    {
        KG3DPvsPortal* ptl = *itor;
        SAFE_DELETE(ptl);
	    ++itor;	
    }

	m_listPortal.clear();

	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::UpdateVisiable()
{
    m_setObjectVisiable.clear();
	const TypeFrustums& myFrustums = this->GetFrustums();

	{
		set<KG3DRepresentObject*>::iterator itor = m_setObjectInside.begin();
		set<KG3DRepresentObject*>::iterator iend = m_setObjectInside.end();

		while (itor != iend)
		{
			KG3DRepresentObject* pObject = *itor;

			for (size_t v = 0; v < myFrustums.size(); ++v)
			{
				if (myFrustums[v].IsAABBoxVisible(pObject->m_BBox))
				{
					m_setObjectVisiable.insert(pObject);
					break;
				}
			}
			++itor;
		}
	}
	{
		set<KG3DEntityObjectShell*>::iterator i = m_setEntityShellInside.begin();
		set<KG3DEntityObjectShell*>::iterator iend = m_setEntityShellInside.end();
		while (i!=iend)
		{
			KG3DEntityObjectShell* pShell = *i;
			if(pShell->m_bLoaded && pShell->m_lpObject)
			{
				static AABBOX bbox;
				bbox.A = pShell->m_vBBox_A;
				bbox.B = pShell->m_vBBox_B;

				for (size_t v = 0; v < myFrustums.size(); ++v)
				{
					if (myFrustums[v].IsAABBoxVisible(bbox))
					{
						m_setObjectVisiable.insert(pShell->m_lpObject);
						break;
					}
				}
			}
			i++;
		}
	}

    return S_OK;
}

BOOL KG3DRepresentObjectPVS::m_sCulling = TRUE;

void KG3DRepresentObjectPVS::UpdateVisiableSubset(KG3DCullData* pCull)
{
    // use for editor

    if (!m_sCulling)
    {
        set<KG3DRepresentObject*>::iterator itor = m_setObjectInside.begin();
        set<KG3DRepresentObject*>::iterator iend = m_setObjectInside.end();

        while (itor != iend)
        {
            KG3DRepresentObject* pObject = *itor;

            if (pObject)
            {
                if (pCull)
                    pCull->OnObjectVisible(pObject);
                else
                    pObject->UpdateVisiableSubset(pCull);
            }

            ++itor;
        }

        return;
    }

    /* update frustum first */

    if (GetFrustums().size() <= 0 && NULL != this->GetParent())
        CalcFrustum(this->GetParent()->GetFrustums(), LOOK_OUT_TO_IN);


    /* get the visiable sub obj */

    m_setObjectVisiable.clear();

    set<KG3DRepresentObject*>::iterator itor = m_setObjectInside.begin();
    set<KG3DRepresentObject*>::iterator iend = m_setObjectInside.end();

	const TypeFrustums& myFrustums = this->GetFrustums();
    while (itor != iend)
    {
        KG3DRepresentObject* pObject = *itor;

        for (size_t v = 0; v < myFrustums.size(); ++v)
        {
            if (myFrustums[v].IsAABBoxVisible(pObject->m_BBox))
            {
                m_setObjectVisiable.insert(pObject);
                
                if (pCull)
                    pCull->OnObjectVisible(pObject);
                else
                    pObject->UpdateVisiableSubset(pCull);
                
                break;
            }
        }
        ++itor;
    }

    set<KG3DModel*>::iterator imdl = m_setModelInside.begin();
    set<KG3DModel*>::iterator emdl = m_setModelInside.end();

    while (imdl != emdl)
    {
        KG3DModel* pModel = *imdl;

        for (size_t v = 0; v < myFrustums.size(); ++v)
        {
            if (myFrustums[v].IsAABBoxVisible(pModel->m_BBox))
            {
                if (pCull)
                    pCull->m_setVisiableModels.insert(pModel);
                break;
            }
        }
        ++imdl;
    }

	{
		set<KG3DEntityObjectShell*>::iterator i = m_setEntityShellInside.begin();
		set<KG3DEntityObjectShell*>::iterator iend = m_setEntityShellInside.end();
		while (i!=iend)
		{
			KG3DEntityObjectShell* pShell = *i;
			if(pShell->m_bLoaded && pShell->m_lpObject)
			{
				KG3DRepresentObject* pObject = pShell->m_lpObject;

				static AABBOX bbox;
				bbox.A = pShell->m_vBBox_A;
				bbox.B = pShell->m_vBBox_B;

				for (size_t v = 0; v < myFrustums.size(); ++v)
				{
					if (myFrustums[v].IsAABBoxVisible(bbox))
					{
						if (pCull)
							pCull->OnObjectVisible(pObject);
						else
							pObject->UpdateVisiableSubset(pCull);

						break;
					}
				}
			}
			i++;
		}
	}

}

HRESULT KG3DRepresentObjectPVS::UpdateVisiableByPortal(KG3DPvsPortal* pProtal)
{
	set<KG3DRepresentObject*>::iterator itor = m_setObjectInside.begin();
	set<KG3DRepresentObject*>::iterator iend = m_setObjectInside.end();

	while (itor != iend)
	{
		KG3DRepresentObject* pObject = *itor;
		if (pProtal->m_Frustum.IsAABBoxVisible(pObject->m_BBox))
			m_setObjectVisiable.insert(pObject);
		++itor;
	}

	return S_OK;
}

void KG3DRepresentObjectPVS::OnRemoveFromEntityMgr(KG3DCullData* pCull)
{
    if (pCull)
    {
        set<KG3DRepresentObject*>::iterator icur = m_setObjectInside.begin();
        set<KG3DRepresentObject*>::iterator iend = m_setObjectInside.end();

        while (icur != iend)
        {
            KG3DRepresentObject* obj = (KG3DRepresentObject*)(*icur);

            obj->OnRemoveFromEntityMgr(pCull);
            vector<KG3DRepresentObject *>::iterator itFinder = 
                find(pCull->m_vecVisiableObjectsRef.begin(), pCull->m_vecVisiableObjectsRef.end(), obj);

            if (itFinder != pCull->m_vecVisiableObjectsRef.end())
            {
                *itFinder = pCull->m_vecVisiableObjectsRef[pCull->m_vecVisiableObjectsRef.size() - 1];
                pCull->m_vecVisiableObjectsRef.resize(pCull->m_vecVisiableObjectsRef.size() - 1);
                obj->Release();
            }

            ++icur;
        }
    }
}

void KG3DRepresentObjectPVS::NextSubOctree()
{
   // m_PvsOCTree.SubsectionStep(this);
}

int KG3DRepresentObjectPVS::GetPortalCount()
{
	return (int)(m_listPortal.size());
}

IKG3DPvsPortal* KG3DRepresentObjectPVS::GetPortal(DWORD dwIndex)
{
    if (dwIndex < 0 || dwIndex >= m_listPortal.size())
        return NULL;

    list<KG3DPvsPortal*>::iterator ifind = m_listPortal .begin();
    advance(ifind, dwIndex);

    return (*ifind);
}

HRESULT KG3DRepresentObjectPVS::DeletePortal(DWORD dwIndex)
{
    if (dwIndex < 0 || dwIndex >= m_listPortal.size())
        return E_FAIL;

    return DeletePortal(dynamic_cast<KG3DPvsPortal*>(GetPortal(dwIndex)));
}

int KG3DRepresentObjectPVS::GetInsideObjectCount()
{
	return (int)(m_setObjectInside.size());
}

IEKG3DRepresentObject* KG3DRepresentObjectPVS::GetInsideObject(DWORD dwIndex)
{
    if (dwIndex < 0 || dwIndex >= m_setObjectInside.size())
        return NULL;

    set<KG3DRepresentObject*>::iterator ifind = m_setObjectInside .begin();
    advance(ifind, dwIndex);

    return (*ifind);
}


HRESULT KG3DRepresentObjectAntiPortal::UpdateToWorld(KG3DCamera* pCamera)
{
	D3DXVec3TransformCoord(&m_vWorldPostion,&D3DXVECTOR3(0,0,0),&m_Matrix);
	D3DXVec3TransformNormal(&m_vWorldDirection,&D3DXVECTOR3(0,0,1),&m_Matrix);

	//D3DXMATRIX mat = m_Matrix;
	D3DXVECTOR3 left(-1,0,0);
	D3DXVECTOR3 up(0,1,0);
	//D3DXVec3TransformNormal(&left,&left,&m_Matrix);
	//D3DXVec3TransformNormal(&up,&up,&m_Matrix);

	switch(m_dwAntiPortalType)
	{
	case ANTIPORTAL_DOUBLEPLANE:
		{
			D3DXVECTOR3 vFront = pCamera->GetCameraFront();
			float K = D3DXVec3Dot(&vFront,&m_vWorldDirection);
			if(K<0)
				left*=-1.0F;
		}
	case ANTIPORTAL_SINGLEPLANE:
		{
			m_vPortalPos[PORTAL_LEFT_B0TTOM] = left * m_fWidthBottom * 0.5F;
			m_vPortalPos[PORTAL_RIGHT_B0TTOM] = -left * m_fWidthBottom * 0.5F;
			m_vPortalPos[PORTAL_LEFT_TOP] =  left * m_fWidthTop * 0.5F + up * m_fHeight;
			m_vPortalPos[PORTAL_RIGHT_TOP] = -left * m_fWidthTop * 0.5F + up * m_fHeight;
		}
		break;
	case ANTIPORTAL_CYLINKDER:
		{
			//D3DXVECTOR3 vFront = pCamera->GetCameraFront();
			D3DXVECTOR3 right = pCamera->GetCameraRight();
			right.y = 0;
			D3DXVec3Normalize(&right,&right);
			
			m_vPortalPos[PORTAL_LEFT_B0TTOM] = -right * m_fWidthBottom * 0.5F;
			m_vPortalPos[PORTAL_RIGHT_B0TTOM] = right * m_fWidthBottom * 0.5F;
			m_vPortalPos[PORTAL_LEFT_TOP] =  -right * m_fWidthTop * 0.5F + up * m_fHeight;
			m_vPortalPos[PORTAL_RIGHT_TOP] = right * m_fWidthTop * 0.5F + up * m_fHeight;
		}
	    break;
	default:
	    break;
	}


	D3DXVec3TransformCoordArray(m_vPortalPosWorld,sizeof(D3DXVECTOR3),m_vPortalPos,sizeof(D3DXVECTOR3),&m_Matrix,4);

	return S_OK;
}

KG3DRepresentObjectAntiPortal::KG3DRepresentObjectAntiPortal()
{
	ZeroMemory(m_vPortalPos, sizeof(m_vPortalPos));
	m_dwAntiPortalType = ANTIPORTAL_SINGLEPLANE;
}

KG3DRepresentObjectAntiPortal::~KG3DRepresentObjectAntiPortal()
{

}

BOOL    KG3DRepresentObjectAntiPortal::IsVisiable(KG3DCamera* pCamera)
{
	D3DXVECTOR3 vFront = pCamera->GetCameraFront();
	float K = D3DXVec3Dot(&vFront,&m_vWorldDirection);
	if(K<0)
		return TRUE;
	else
		return FALSE;
}

HRESULT KG3DRepresentObjectAntiPortal::UpdateFrustum(KG3DCamera* pCamera)
{
	if(NULL == pCamera)
		return E_FAIL;
	return m_Frustum.CreateByObjectPVS(*pCamera,m_vPortalPosWorld);
}

HRESULT KG3DRepresentObjectAntiPortal::Render(DWORD dwOption)
{
	switch(m_dwAntiPortalType)
	{
	case ANTIPORTAL_SINGLEPLANE:
	case ANTIPORTAL_DOUBLEPLANE:
	case ANTIPORTAL_CYLINKDER:
		{
			DWORD color = 0xFFFF00FF;

			g_cGraphicsTool.DrawLine(&m_vPortalPosWorld[PORTAL_LEFT_B0TTOM],&m_vPortalPosWorld[PORTAL_RIGHT_B0TTOM],color,color);
			g_cGraphicsTool.DrawLine(&m_vPortalPosWorld[PORTAL_RIGHT_B0TTOM],&m_vPortalPosWorld[PORTAL_RIGHT_TOP],color,color);
			g_cGraphicsTool.DrawLine(&m_vPortalPosWorld[PORTAL_RIGHT_TOP],&m_vPortalPosWorld[PORTAL_LEFT_TOP],color,color);
			g_cGraphicsTool.DrawLine(&m_vPortalPosWorld[PORTAL_LEFT_TOP],&m_vPortalPosWorld[PORTAL_LEFT_B0TTOM],color,color);

			g_cGraphicsTool.DrawLine(&m_vWorldPostion,&(m_vWorldPostion + m_vWorldDirection * 500),color,color);
		}
		break;
	default:
		break;
	}

	return KG3DRepresentObject::Render(dwOption);
}

DWORD KG3DRepresentObjectAntiPortal::GetPortalType()
{
	return m_dwAntiPortalType;
}

HRESULT KG3DRepresentObjectAntiPortal::SetPortalType(DWORD dwType)
{
	m_dwAntiPortalType = dwType;
	return S_OK;
}

HRESULT KG3DRepresentObjectAntiPortal::GetData(float* pWidthBottom,float* pWidthTop,float* pHeight)
{
	(*pWidthBottom) = m_fWidthBottom;
	(*pWidthTop) = m_fWidthTop;
	(*pHeight) = m_fHeight;
	return S_OK;
}

HRESULT KG3DRepresentObjectAntiPortal::SetData(float  fWdithBottom,float fWdithTop,float fHeight)
{
	m_fWidthBottom = fWdithBottom;
	m_fWidthTop = fWdithTop;
	m_fHeight = fHeight;
	return S_OK;
}

BOOL KG3DRepresentObjectPVS::IsPositionsInSpace(D3DXVECTOR3* pVers,int nCount)
{
	if (GetOutsideBorderModel())
	{
		for (int i=0;i<nCount;i++)
		{
			if(!IsPositionInSet(pVers[i]))
				return FALSE;
		}
		return TRUE;
	}

    return FALSE;
}

BOOL KG3DRepresentObjectPVS::IsPositionInSpace(D3DXVECTOR3& vPos)
{
    if (m_BBox.IsPointInside(vPos))
        return IsPositionInSet(vPos);
    return FALSE;
}


HRESULT KG3DRepresentObjectPVS::ComputeBBox()
{
    KG3DRepresentObject::ComputeBBox();

    set<KG3DRepresentObject*>::iterator itor = m_setObjectInside.begin();
    set<KG3DRepresentObject*>::iterator iend = m_setObjectInside.end();

    while (itor != iend)
    {
        (*itor)->ComputeBBox();
        ++itor;
    }

    return S_OK;
}

KG3DRepresentObject* KG3DRepresentObjectPVS::CheckLoadIndoorWall()
{
    ASSERT(false);

    KG3DModel* pModel = static_cast<KG3DModel*>(GetOutsideBorderModel());

    if (!pModel)
        return NULL;

    TCHAR szInteriorName[128];
    _tcsncpy(szInteriorName, pModel->m_scName.c_str(), sizeof(szInteriorName));
    TCHAR* t = _tcsrchr(szInteriorName, TEXT('.'));
    *t = 0;
    _tcscat(szInteriorName, TEXT("_inside.mesh"));

    pModel = static_cast<KG3DModel*>(GetInsideBorderModel());
    SAFE_RELEASE(pModel);
    SetModel(1, szInteriorName, TEXT(""), TEXT(""), TRUE);

    //KG3DRepresentObject* obj = NULL;
    //g_GetObjectTable().NewOneResourse(REPRESENTOBJECT_DEFAULT,(IKG3DResourceBase**)&obj);

    //if (obj)
    //{
    //    if (FAILED(obj->SetModel(0, szInteriorName, TEXT(""), TEXT(""), TRUE)))
    //    {
    //        SAFE_RELEASE(obj);
    //        return NULL;
    //    }
    //}

   return this;
}

size_t KG3DRepresentObjectPVS::GetSubModels(vector<KG3DModel*>& vModels)
{
	const KG3DLogicalConstances& lgC = g_GetLogicalConstants();	//利用引用机制创建一个短一点容易读的别名
	int   nDynamicObstacleGroupIndex  = lgC.invalidObstableGroup;

    size_t s = vModels.size();

    for (size_t i = 0; i < m_vecModel.size(); ++i)
        if (m_vecModel[i])
            vModels.push_back(m_vecModel[i]);

    for (set<KG3DRepresentObject*>::iterator i = m_setObjectInside.begin(); i != m_setObjectInside.end(); ++i)
	{
		KG3DRepresentObject* pObject = *i;
		DWORD dwObjType = pObject->GetType();
		nDynamicObstacleGroupIndex = pObject->GetDynamicObstacleGroupIndex();

		if (
			dwObjType == REPRESENTOBJECT_WAYPOINT || dwObjType == REPRESENTOBJECT_NPC ||
			dwObjType == REPRESENTOBJECT_REFRESHPOINT || dwObjType == REPRESENTOBJECT_DOODAD ||
			dwObjType == REPRESENTOBJECT_POLY || dwObjType == REPRESENTOBJECT_TRAFFICPOINT || dwObjType == REPRESENTOBJECT_SOUNDBALL
			)
		{
			continue;
		}

		pObject->GetSubModels(vModels);
	}

	for (set<KG3DEntityObjectShell*>::iterator i = m_setEntityShellInside.begin(); i != m_setEntityShellInside.end(); ++i)
	{
		KG3DEntityObjectShell* pShell = *i;
		if(pShell->m_bLoaded && pShell->m_lpObject)
		{
			KG3DRepresentObject* pObject = pShell->m_lpObject;
			DWORD dwObjType = pObject->GetType();
			nDynamicObstacleGroupIndex = pObject->GetDynamicObstacleGroupIndex();

			if (
				dwObjType == REPRESENTOBJECT_WAYPOINT || dwObjType == REPRESENTOBJECT_NPC ||
				dwObjType == REPRESENTOBJECT_REFRESHPOINT || dwObjType == REPRESENTOBJECT_DOODAD  ||
				dwObjType == REPRESENTOBJECT_POLY || dwObjType == REPRESENTOBJECT_TRAFFICPOINT || dwObjType == REPRESENTOBJECT_SOUNDBALL
				)
			{
				continue;
			}

			pObject->GetSubModels(vModels);
		}
	}

    return vModels.size() - s;
}

BOOL KG3DRepresentObjectPVS::GetCameraNearRayIntersect(IN CONST D3DXVECTOR3 vSrc[],
                                                       IN CONST D3DXVECTOR3 vDst[], 
                                                       IN CONST INT         nNum,
                                                       OUT      FLOAT*      pDis 
                                                        )
{
    if (!pDis)
        return FALSE;

    BOOL bHit = FALSE;

    if (KG3DRepresentObject::GetCameraNearRayIntersect(vSrc, vDst, nNum, pDis))
        bHit = TRUE;

    for (set<KG3DRepresentObject*>::iterator i = m_setObjectInside.begin(); i != m_setObjectInside.end(); ++i)
    {
        KG3DRepresentObject* obj = (*i);

        if (!obj)
            continue;

        if (obj->GetCameraNearRayIntersect(vSrc, vDst, nNum, pDis))
            bHit = TRUE;

    }
    
    return bHit;
}

BOOL KG3DRepresentObjectPVS::GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction, BOOL bFiltrate)
{
    BOOL bhit = FALSE;

    if(m_vecModel.size()==0)
        return FALSE;

    D3DXVECTOR3* pVec = pInterPoint;

    for(size_t i=0;i<m_vecModel.size();i++)
    {
        KG3DModel* pModel = m_vecModel[i];
        if(NULL != pModel)
        {
            KG3DOBJProperty *pObjProperty = NULL;

            KG3DMesh* pMesh = pModel->GetMesh();
            if (NULL == pMesh)
                continue;

            pMesh->GetObjProperty((BYTE**)&pObjProperty);
            if (bFiltrate && !pObjProperty->bObscatleCamera)
                continue;

            if(dwCount < dwMaxNum)
            {
                DWORD dwNum = 0;
                DWORD dwSize = dwMaxNum - dwCount;
                BOOL bHitC = pModel->GetRayIntersect(dwNum,pVec,dwSize,Origin,Direction);
                bhit |= bHitC;
                if(bHitC)
                {
                    dwCount += dwNum;
                    pVec += dwNum;
                }
            }
            else
                return bhit;
        }
    }

    for (set<KG3DRepresentObject*>::iterator i = m_setObjectInside.begin(); i != m_setObjectInside.end(); ++i)
    {
        KG3DRepresentObject* obj = (*i);

        if (!obj)
            continue;

        if (dwCount > dwMaxNum)
            break;

        DWORD dwNum = 0;
        DWORD dwSize = dwMaxNum - dwCount;
        
        if (obj->GetRayIntersect(dwNum, pVec, dwSize, Origin, Direction, bFiltrate))
        {
            dwCount += dwNum;
            pVec += dwNum;
            bhit = true;
        }
    }

    return bhit;
}

BOOL KG3DRepresentObjectPVS::RayIntersection(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, FLOAT* pfRet, BOOL bIntersectBoxOnly, KG3DIntersectionFilter* pFilter /*= NULL*/ )
{
    BOOL bHit = FALSE;
    FLOAT fDistance = FLT_MAX;

    if (NULL != pFilter)
    {
        HRESULT hr = pFilter->Accept(*this);
        if (FAILED(hr))
            return FALSE;
    }

	if (bIntersectBoxOnly)
	{
		bHit = m_BBox.RayIntersection(vSrc, vNormalizedDir, &fDistance, NULL);
		if (bHit && NULL != pfRet)
		{
			*pfRet = fDistance;
		}
		return bHit;
	}

    if(! m_BBox.RayIntersection(vSrc, vNormalizedDir, NULL, NULL))
        return FALSE;

    for(size_t i=0; i < m_vecModel.size(); i++)
    {
        KG3DModel* pModel = m_vecModel[i];
        
        if (pModel)
        {
            FLOAT fTemp = NULL;
            BOOL bHitTemp = pModel->RayIntersection(vSrc, vNormalizedDir, &fTemp,NULL, bIntersectBoxOnly, pFilter);

            if(bHitTemp)
            {
                if (fTemp < fDistance && fTemp > 0)
                {
                    bHit |= bHitTemp;
                    fDistance = fTemp;
                }
            }
        }
    }

    /* check sub object */

    for (set<KG3DRepresentObject*>::iterator i = m_setObjectInside.begin(); i != m_setObjectInside.end(); ++i)
    {
        KG3DRepresentObject* obj = (*i);

        if (!obj)
            continue;

        float fDist = 0.f;

        if (!obj->RayIntersection(vSrc, vNormalizedDir, &fDist, FALSE, pFilter ))
            continue;

        if (fDist < fDistance && fDist > 0.f)
        {
            bHit = TRUE;
            fDistance = fDist;
        }
    }

    if (bHit)
    {
        _ASSERTE(fDistance < FLT_MAX);

        if (NULL != pfRet)
        {
            *pfRet = fDistance;
        }
    }

    return bHit;
}

HRESULT KG3DRepresentObjectPVS::AddObject(KG3DRepresentObject* pObject, KG3DScene* pScene)
{
    if (!pObject || !pScene)
        return E_FAIL;

	set<KG3DRepresentObject*>::iterator iFind = m_setObjectInside.find(pObject);
	if(iFind==m_setObjectInside.end())
	{
		m_setObjectInside.insert(pObject);

		if (pObject->GetParentObject() != this)
		{
			D3DXMATRIX matInv;
			D3DXMatrixInverse(&matInv, NULL, &GetMatrixWorld());
			pObject->UpdateByMatrix((pObject->GetMatrixWorld() * matInv));
		}

		pObject->SetParentMatrix(&GetMatrixWorld());
		pObject->SetParentObject(this);

		if (pObject->GetType() == REPRESENTOBJECT_PVS)
		{
			KG3DRepresentObjectPVS* pvs = static_cast<KG3DRepresentObjectPVS*>(pObject);
			pvs->SetHandle(pScene->GetOneNewSpaceNodeHandle());
			KG3DSceneSpaceNode::ConnectTwoNodes(this, pvs);
			pScene->RegisterSpaceNode(pvs);
		}	
	}

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::DelObject(KG3DRepresentObject* pObject, KG3DScene* pScene)
{
    if (!pObject || !pScene)
        return E_FAIL;

	set<KG3DRepresentObject*>::iterator iFind = m_setObjectInside.find(pObject);
	if(iFind!=m_setObjectInside.end())
	{
		m_setObjectInside.erase(pObject);
		m_setObjectVisiable.erase(pObject);

		pObject->SetParentMatrix(NULL);
		pObject->SetParentObject(NULL);
		pObject->UpdateByMatrix(pObject->GetMatrixWorld());

		if (pObject->GetType() == REPRESENTOBJECT_PVS)
		{
			KG3DRepresentObjectPVS* pvs = static_cast<KG3DRepresentObjectPVS*>(pObject);
			KG3DSceneSpaceNode::DisConnectTwoNode(this,pvs);
			pScene->UnRegisterSpaceNode(pvs);
		}	

		SAFE_RELEASE(pObject);
	}

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::InputObject(KG3DRepresentObject* pObject, KG3DScene* pScene)
{
    if (!pObject)
        return E_FAIL;

    D3DXVECTOR3 vPos = pObject->GetBBox().GetCenter();

    KG3DSceneSpaceNode::TypeAccessProxy& accessProxy = this->GetChildrenAccessProxy();
    for (accessProxy.Begin(); ! accessProxy.IsEnd(); accessProxy.StepForward())
    {
        KG3DRepresentObjectPVS* pvs = static_cast<KG3DRepresentObjectPVS*>(accessProxy.GetCur());

        if (pvs->IsPositionInSpace(vPos))
        {
            pvs->InputObject(pObject, pScene);
            break;
        }
    }

    if(!accessProxy.IsEnd())
        return S_OK;

	set<KG3DRepresentObject*>::iterator iFind = m_setObjectInside.find(pObject);
	if(iFind==m_setObjectInside.end())
	{
		pObject->AddRef();
		m_setObjectInside.insert(pObject);
		pObject->OnEnterPvs(this);

		if (pObject->GetType() == REPRESENTOBJECT_PVS && pScene)
		{
			KG3DRepresentObjectPVS* pvs = static_cast<KG3DRepresentObjectPVS*>(pObject);
			pvs->SetHandle(pScene->GetOneNewSpaceNodeHandle());
			KG3DSceneSpaceNode::ConnectTwoNodes(this, pvs);
			pScene->RegisterSpaceNode(pvs);
		}
	}
	else
	{
		ASSERT(0);
	}

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::RemoveObject(KG3DRepresentObject* pObject, KG3DScene* pScene)
{
    if (!pObject || !pScene)
        return E_FAIL;

	set<KG3DRepresentObject*>::iterator iFind = m_setObjectInside.find(pObject);
	if(iFind!=m_setObjectInside.end())
	{
		m_setObjectInside.erase(pObject);
		m_setObjectVisiable.erase(pObject);

		if (pObject->GetType() == REPRESENTOBJECT_PVS)
		{
			KG3DRepresentObjectPVS* pvs = static_cast<KG3DRepresentObjectPVS*>(pObject);
			KG3DSceneSpaceNode::DisConnectTwoNode(this,pvs);
			pScene->UnRegisterSpaceNode(pvs);
		}	

		pObject->m_pPvsOn = NULL;
		SAFE_RELEASE(pObject);
	}
	else
	{
		ASSERT(0);
	}

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::InputModel(KG3DModel* pModel)
{
    if (!pModel)
        return E_FAIL;

    D3DXVECTOR3 vPos = pModel->GetBBox().GetCenter();//D3DXVECTOR3(mdl->m_matWorld._41, mdl->m_matWorld._42, mdl->m_matWorld._43);

	KG3DSceneSpaceNode::TypeAccessProxy& accessProxy = this->GetChildrenAccessProxy();
	for (accessProxy.Begin(); ! accessProxy.IsEnd(); accessProxy.StepForward())
	{
		KG3DRepresentObjectPVS* pvs = static_cast<KG3DRepresentObjectPVS*>(accessProxy.GetCur());
		if (pvs->IsPositionInSpace(vPos))
		{
			pvs->InputModel(pModel);
			break;
		}
	}

	if(!accessProxy.IsEnd())
		return S_OK;

	set<KG3DModel*>::iterator iFind = m_setModelInside.find(pModel);
	if(iFind==m_setModelInside.end())
	{
		pModel->AddRef();
		m_setModelInside.insert(pModel);
		pModel->OnEnterPvs(this);
	}
	else
	{
		ASSERT(0);
	}

    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::RemoveModel(KG3DModel* pObject)
{
    if (!pObject)
        return E_FAIL;

    if (m_setModelInside.find(pObject) != m_setModelInside.end())
    {
        m_setModelInside.erase(pObject);
        pObject->m_pPvsOn = NULL;
		pObject->Release();
    }
    else
    {
		KG3DSceneSpaceNode::TypeAccessProxy& accessProxy = this->GetChildrenAccessProxy();
		for (accessProxy.Begin(); ! accessProxy.IsEnd(); accessProxy.StepForward())
		{
			KG3DRepresentObjectPVS* pvs = static_cast<KG3DRepresentObjectPVS*>(accessProxy.GetCur());

			if (SUCCEEDED(pvs->RemoveModel(pObject)))
				break;
		}
    }
    
    return S_OK;
}

HRESULT KG3DRepresentObjectPVS::RemoveEntityObjectShell(KG3DEntityObjectShell* pShell)
{
	KGLOG_PROCESS_ERROR(pShell);

	m_setEntityShellInside.erase(pShell);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectPVS::InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs)
{
	KGLOG_PROCESS_ERROR(pShell);
	pShell->m_setRefrenceContainer.insert(this);
	m_setEntityShellInside.insert(pShell);

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DRepresentObjectPVS::GetPolyIntersectRect(vector<KG3DRepresentPoly*>& vPoly,D3DXVECTOR2& A,D3DXVECTOR2& C)
{

	for (set<KG3DEntityObjectShell*>::iterator i = m_setEntityShellInside.begin(); i != m_setEntityShellInside.end(); ++i)
	{
		KG3DEntityObjectShell* pShell = *i;
		if(pShell->m_bLoaded && pShell->m_lpObject)
		{
			if(pShell->m_vBBox_A.x <= C.x && pShell->m_vBBox_A.z <= C.y &&
				pShell->m_vBBox_B.x >= A.x && pShell->m_vBBox_B.z >= A.y   )
			{
				KG3DRepresentObject* pObject = pShell->m_lpObject;
				DWORD dwObjType = pObject->GetType();

				if (dwObjType == REPRESENTOBJECT_POLY )
				{
					KG3DRepresentPoly* pPoly = static_cast<KG3DRepresentPoly*>(pObject);
					vPoly.push_back(pPoly);
				}
				else if (dwObjType == REPRESENTOBJECT_PVS)
				{
					KG3DRepresentObjectPVS* pPvs = static_cast<KG3DRepresentObjectPVS*>(pObject);
					pPvs->GetPolyIntersectRect(vPoly,A,C);
				}
			}
			
		}
	}

	return S_OK;
}


HRESULT KG3DRepresentObjectPVS::GetObjectIntersectRect(vector<KG3DRepresentObject*>& vObject,D3DXVECTOR2& A,D3DXVECTOR2& C)
{
	for (set<KG3DEntityObjectShell*>::iterator i = m_setEntityShellInside.begin(); i != m_setEntityShellInside.end(); ++i)
	{
		KG3DEntityObjectShell* pShell = *i;
		if(pShell->m_bLoaded && pShell->m_lpObject)
		{
			if(pShell->m_vBBox_A.x <= C.x && pShell->m_vBBox_A.z <= C.y &&
				pShell->m_vBBox_B.x >= A.x && pShell->m_vBBox_B.z >= A.y   )
			{
				KG3DRepresentObject* pObject = pShell->m_lpObject;
				DWORD dwObjType = pObject->GetType();
				if (dwObjType == REPRESENTOBJECT_PVS)
				{
					KG3DRepresentObjectPVS* pPvs = static_cast<KG3DRepresentObjectPVS*>(pObject);
					pPvs->GetObjectIntersectRect(vObject,A,C);
				}
				else
				{
					vObject.push_back(pObject);
				}
			}

		}
	}

	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::GetModels(vector<KG3DModel*>* pvecModels,vector<KG3DModelPointLight*>* pvecPointLight,BOOL bIncludeOutsiderBorder)
{
	//////////////////////////////////////////////////////////////////////////
	{
		KG3DModel* pInside = (KG3DModel*)GetInsideBorderModel();
		if(pInside)
			pvecModels->push_back(pInside);

		if(bIncludeOutsiderBorder)
		{
			KG3DModel* pOutside = (KG3DModel*)GetOutsideBorderModel();
			if(pOutside)
				pvecModels->push_back(pOutside);
		}
	}

	DWORD dwCount = GetInsideObjectCount();

	for (DWORD i = 0; i < dwCount; ++i)
	{
		IEKG3DRepresentObject* pObj = GetInsideObject(i);

		if (pObj)
		{
			KG3DRepresentObject* pObjectReal = dynamic_cast<KG3DRepresentObject*>(pObj);

			KG3DModel* pModel = NULL;

			pObjectReal->GetModel(pModel,0,FALSE);

			if(pModel)
			{
				switch (pModel->GetType())
				{
				case MESHTYPE_DEFAULT:
					{
						pvecModels->push_back(pModel);
						break;
					}
				case MESHTYPE_POINTLIGHT :
					{
						KG3DModelPointLight* pLight = dynamic_cast<KG3DModelPointLight*>(pModel);
						pvecPointLight->push_back(pLight);
						break;
					}
				default :
					break;
				}
			}
		}
	}

	return S_OK;
}

HRESULT KG3DRepresentObjectPVS::ProcessLightmapKeyForModels(vector<KG3DModel*>* pvecModelWithLightmap)
{
	vector<KG3DModelPointLight*>m_vecPointLight;
	vector<KG3DModel*>          m_vecModel;
	//////////////////////////////////////////////////////////////////////////
	GetModels(&m_vecModel,&m_vecPointLight,TRUE);

	m_dwLightmapInUse = 0;
	for (size_t i=0;i<m_vecModel.size();i++)
	{
		KG3DModel* pModel = m_vecModel[i];
		if(pModel && pModel->m_lpLightMap)
		{
			pvecModelWithLightmap->push_back(pModel);

			pModel->m_lpLightMap->m_nKey = m_dwLightmapInUse;

			m_dwLightmapInUse++;
		}
	}

	return S_OK;
}