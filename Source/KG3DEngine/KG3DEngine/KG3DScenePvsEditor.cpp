
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DScenePvsEditor.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2008-10-25 17:27:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DScenePvsEditor.h"
#include "KG3DRepresentObjectPVS.h"
#include "KG3DEngineManager.h"
#include "KG3DRepresentObjectTable.h"
#include "KG3DLocalMoveCoord.h"
#include "KG3DLocalRotationCoord.h"
#include "KG3DLocalScalCoord.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DEnvironment.h"
#include "KG3DScenePointLightRender.h"
#include "KG3DSceneFunctions.h"
#include "KG3DLightMapBaker.h"

KG3DScenePvsEditor::KG3DScenePvsEditor()
    : m_pGroundGridVertexs(NULL),
      m_pPvs(NULL),
      m_pHandObj(NULL),
      m_pHandPtl(NULL),
      m_dwPlacePortalMode(PLACE_PTL_BILLBOARD),
      m_bSwitchObj(false),
      m_pRunder(NULL),
      m_bRunMode(false),
      m_bCull(false)
{
    m_itorListObj = m_listPlaceObj.end();

    m_CameraSpeed = 300.f;
    m_CamareState = 0;
    m_vMouseTargetPosition = D3DXVECTOR3(0,0,0); 
    m_bMouseTargetMoving = FALSE;
    m_vCamera = D3DXVECTOR3(1000,0,D3DX_PI/8);
    m_lpPointLightRender = NULL;

	m_lpLightMapBaker = NULL;
}

KG3DScenePvsEditor::~KG3DScenePvsEditor()
{
    UnInit();
    ASSERT(!m_lpPointLightRender);
}

HRESULT KG3DScenePvsEditor::Init()
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	int nPointLightRenderInitFlag = false;

    if (SUCCEEDED(g_GetObjectTable().Get1NewResource((KG3DRepresentObject**)&m_pRunder, REPRESENTOBJECT_NPC, 0)))
    {
        m_pRunder->SetModel(0, "data\\public\\scenerunner\\scenerunner.Mesh", NULL, "data\\public\\scenerunner\\scenerunner.ani", FALSE);
        m_pRunder->SetNpcFrontDir(D3DXVECTOR3(0.0f,0.0f, -1.0f));
        m_pRunder->SetNpcType(NPC_TYPE_PROTAGONIST);
        //m_pRunder->BandScene(this);
    }

    if (m_lpSceneOutDoorSpaceMgr)
    {
        extern TKG3DResourcePool<KG3DSceneZone> g_cSceneZonePool;
        KG3DSceneZone* pNewZone = g_cSceneZonePool.RequestResource();
        pNewZone->Init(0, 0);
        m_lpSceneOutDoorSpaceMgr->InputNewZone(pNewZone);
    }
	
	hrRetCode = KG3DSceneEditorBase::Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    
    {
        IIniFile* pIniFile = g_OpenIniFile("data\\public\\PvsEditorSetting.ini");
        
        if (!pIniFile)
            GetEnvironment().LoadMapSetting(NULL, NULL);
        else
            GetEnvironment().LoadMapSetting(pIniFile, NULL);

        SAFE_RELEASE(pIniFile);

    }
	
    m_lpPointLightRender = new KG3DScenePointLightRender;
    KGLOG_PROCESS_ERROR(m_lpPointLightRender);

    hrRetCode = m_lpPointLightRender->Init();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    nPointLightRenderInitFlag = true;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
	    if (nPointLightRenderInitFlag)
	    {
            m_lpPointLightRender->UnInit();
            nPointLightRenderInitFlag = false;
	    }
        SAFE_DELETE(m_lpPointLightRender);
    }
	return hrResult;
}

HRESULT KG3DScenePvsEditor::UnInit()
{
    IIniFile* pIniFile = g_OpenIniFile("data\\public\\PvsEditorSetting.ini", false, true);

    if (pIniFile)
    {
        GetEnvironment().SaveMapSetting(pIniFile);
        pIniFile->Save("data\\public\\PvsEditorSetting.ini");
    }

    if (m_lpPointLightRender)
        m_lpPointLightRender->UnInit();
    SAFE_DELETE(m_lpPointLightRender);

    SAFE_RELEASE(pIniFile);


    KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
    selTool.ClearSelection();

	SAFE_DELETE(m_lpLightMapBaker);

    SAFE_RELEASE(m_pGroundGridVertexs);
    SAFE_RELEASE(m_pPvs);
    SAFE_RELEASE(m_pHandObj);
    SAFE_DELETE(m_pHandPtl);
    SAFE_DELETE(m_pRunder);


	KG3DSceneEditorBase::UnInit();

    return S_OK;
}

HRESULT KG3DScenePvsEditor::GetFloor(const D3DXVECTOR3* pvInput, FLOAT* pfRet)
{
    if (!pvInput || !pfRet)
        return E_FAIL;
    
    (*pfRet) = 0.f;

    if (m_pPvs)
    {
        DWORD dwCount = 0;
        D3DXVECTOR3 vInsert[32];

        if (m_pPvs->GetRayIntersect(dwCount, vInsert, 32, (*pvInput), D3DXVECTOR3(0.f, -1.f, 0.f), false))
        {
            for (DWORD i = 0; i < dwCount; ++i)
            {
                if (vInsert[i].y > (*pfRet) && vInsert[i].y <= (*pvInput).y)
                    (*pfRet) = vInsert[i].y;
            }
        }
    }
   
    return S_OK;
}

HRESULT KG3DScenePvsEditor::OpenPvs(LPCTSTR fileName)
{
    TCHAR szRetPath[MAX_PATH];

    if (g_GetFilePathFromFullPath(szRetPath, fileName))
        fileName = szRetPath;

	_ASSERTE(NULL == m_pPvs && _T("否则下面的创建就会覆盖掉这个指针"));
    if (FAILED(g_cEngineManager.BuildKG3DRepresentObject(fileName, REPRESENTOBJECT_PVS, 0, 0, (IEKG3DRepresentObject**)&m_pPvs)))
        return E_FAIL;

	//KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
    for (list<KG3DPvsPortal*>::iterator i = m_pPvs->m_listPortal.begin(); i != m_pPvs->m_listPortal.end(); ++i)
	{
		/*KG3DSceneEntity Entity(SCENEENTITY_PVS_PORTAL, *i);
		selTool.SetSelectable(Entity, TRUE);*/

		this->IEInsertEntitySelectable(SCENEENTITY_PVS_PORTAL, *i, TRUE);//<Selectable Refactor>
	}

	for (set<KG3DRepresentObject*>::iterator i = m_pPvs->m_setObjectInside.begin(); i != m_pPvs->m_setObjectInside.end(); ++i)
	{
		/*KG3DSceneEntity Entity(SCENEENTITY_OBJECT, *i);
		selTool.SetSelectable(Entity, TRUE);*/
		this->IEInsertEntitySelectable(SCENEENTITY_OBJECT, *i, TRUE);//<Selectable Refactor>
	}

    m_pPvs->SetHandle(GetOneNewSpaceNodeHandle());
    KG3DSceneSpaceNode::ConnectTwoNodes(m_lpSceneOutDoorSpaceMgr, m_pPvs);
    RegisterSpaceNode(m_pPvs);

  /*  if (m_lpSceneOutDoorSpaceMgr)
        m_lpSceneOutDoorSpaceMgr->InputRepresentObject(m_pPvs, TRUE, FALSE);*/

    return S_OK;
}

HRESULT KG3DScenePvsEditor::NewPvs()
{
    if (FAILED(g_cEngineManager.NewKG3DRepresentObject(REPRESENTOBJECT_PVS, 0, 0, (IEKG3DRepresentObject**)&m_pPvs)))
        return E_FAIL;

    m_pPvs->SetHandle(GetOneNewSpaceNodeHandle());
    KG3DSceneSpaceNode::ConnectTwoNodes(m_lpSceneOutDoorSpaceMgr, m_pPvs);
    RegisterSpaceNode(m_pPvs);

    //if (m_lpSceneOutDoorSpaceMgr)
    //    m_lpSceneOutDoorSpaceMgr->InputRepresentObject(m_pPvs, TRUE, FALSE);
    
    return S_OK;
}

HRESULT KG3DScenePvsEditor::SavePvs(LPCTSTR fileName)
{
    if (!m_pPvs)
        return E_FAIL;

    return m_pPvs->SavePvsFile(fileName);

}

DWORD KG3DScenePvsEditor::ObjTypeFromName(LPCTSTR fileName)
{
    TCHAR* t = _tcsrchr((LPTSTR)fileName, TEXT('.'));

    if (!t)
        return (DWORD)REPRESENTOBJECT_FORCEDWORD;

    ++t;

    if (!_tcsicmp(t, TEXT("pvs")))
        return REPRESENTOBJECT_PVS;

    if (!_tcsicmp(t, TEXT("mesh")) || !_tcsicmp(t, TEXT("mdl")) || !_tcsicmp(t, TEXT("sfx")) || !_tcsicmp(t, TEXT("stree")) || !_tcsicmp(t, TEXT("plight")))
        return REPRESENTOBJECT_DEFAULT;

    if (!_tcsicmp(t, TEXT("modelset")))
        return REPRESENTOBJECT_SET;

    return (DWORD)REPRESENTOBJECT_FORCEDWORD;

}

HRESULT KG3DScenePvsEditor::SetHandObject(LPCTSTR fileName)
{
    if (!m_pPvs)
        return E_FAIL;

    SAFE_RELEASE(m_pHandObj);
    SAFE_DELETE(m_pHandPtl);

    if (!fileName)
        return S_OK;

    if (_tcsicmp(fileName, TEXT("portal")))
    {
        DWORD  dwObjType = ObjTypeFromName(fileName);

        if (dwObjType == (DWORD)REPRESENTOBJECT_FORCEDWORD)
            return E_FAIL;

        if (FAILED(g_cEngineManager.BuildKG3DRepresentObject(fileName, dwObjType, 0, 0, (IEKG3DRepresentObject**)&m_pHandObj)))
            return E_FAIL;
    }
    else
    {
        if (!m_pHandPtl)
            m_pHandPtl = new KG3DPvsPortal(m_pPvs);
        m_pHandPtl->SetScaling(&D3DXVECTOR3(500.f, 500.f, 500.f));
    }

    SetEditState(SCENESTATE_PLACEOBJ); 

    return S_OK;
}

HRESULT KG3DScenePvsEditor::SetEditState(DWORD dwState)
{
    KG3DSceneEditorBase::SetEditState(dwState);
    
    if (dwState != SCENESTATE_PLACEOBJ)
        SetHandObject(NULL);
    
    return S_OK;
}

void KG3DScenePvsEditor::SetCoordMode(KG3DCOORD coord)
{    
    this->GetCoordMove().SetMode(coord);
    this->GetCoordScale().SetMode(coord);
    this->GetCoordRotate().SetMode(coord);
}

void KG3DScenePvsEditor::RegistEventCallBack(IPvsEditorEventCallback* pEvent)
{
    vector<IPvsEditorEventCallback*>::iterator ifind = find(m_vpEventCallBack.begin(), m_vpEventCallBack.end(), pEvent);

    if (ifind == m_vpEventCallBack.end())
        m_vpEventCallBack.push_back(pEvent);
}

void KG3DScenePvsEditor::UnRegistEventCallBack(IPvsEditorEventCallback* pEvent)
{
    vector<IPvsEditorEventCallback*>::iterator ifind = find(m_vpEventCallBack.begin(), m_vpEventCallBack.end(), pEvent);

    if (ifind != m_vpEventCallBack.end())
        m_vpEventCallBack.erase(ifind);
}

void KG3DScenePvsEditor::SelectObject(IEKG3DRepresentObject* obj)
{
    //ClearSelectedEntity();
	this->GetSelectionTool().ClearSelection();
    
    KG3DSceneEntity SelEntity(SCENEENTITY_OBJECT, static_cast<KG3DRepresentObject*>(obj));

    /*list<KG3DSceneEntity>::iterator ifind = find(m_listSelectableEntity.begin(), m_listSelectableEntity.end(), SelEntity);
    
    if (ifind != m_listSelectableEntity.end())
        AddSelectedEntity(&SelEntity);*/

	if(this->GetSelectionTool().IsSelectable(SelEntity))
	{
		//AddSelectedEntity(&SelEntity);
		this->GetSelectionTool().SetSelected(SelEntity, TRUE);
	}
}

void KG3DScenePvsEditor::DeleteObject(IEKG3DRepresentObject* obj)
{

}

IEKG3DRepresentObject* KG3DScenePvsEditor::GetFirstSelectedObj()
{
	if (GetSelectionTool().GetSelectionCount() > 0)
	{
		TypeEntityAccessProxy& proxy = GetSelectionTool().GetSelectionAccessProxy();
		proxy.Begin();

		KG3DSceneEntity& e = proxy.GetCur();

		if (e.GetType() != SCENEENTITY_OBJECT)
			return NULL;

		return (KG3DRepresentObject*)e.m_pPointer;
	}

	return NULL;
}

void KG3DScenePvsEditor::EnableCull(BOOL bCull)
{
    if (bCull == m_bCull)
        return;

    m_bCull = bCull;
}

HRESULT KG3DScenePvsEditor::PasteSelection()
{
	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
    TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();

    size_t uSelCount = (size_t)(selTool.GetSelectionCount());
    std::vector<KG3DSceneEntity> vecEntities;
    vecEntities.reserve(uSelCount);
    for (proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
    {
        KG3DSceneEntity& entityCur = proxy.GetCur();
        vecEntities.push_back(entityCur);
    }

    for (size_t i = 0; i < vecEntities.size(); ++i)
	{
		KG3DSceneEntity Entity = vecEntities[i];

        switch (Entity.GetType())
        {
        case SCENEENTITY_OBJECT :
            {
                KG3DRepresentObject* obj = static_cast<KG3DRepresentObject*>(Entity.m_pPointer);
                KG3DRepresentObject* objClone = NULL;

                if (SUCCEEDED(g_GetObjectTable().GetCopy(&objClone, obj)))
                {
                    objClone->FrameMove();
                    m_pPvs->AddObject(objClone, this);
                    
					/*KG3DSceneEntity entity(SCENEENTITY_OBJECT, objClone);
					selTool.SetSelectable(entity, TRUE);*/
					this->IEInsertEntitySelectable(SCENEENTITY_OBJECT, objClone, TRUE);//<Selectable Refactor>


                    for (size_t i = 0; i < m_vpEventCallBack.size(); ++i)
                        m_vpEventCallBack[i]->OnAddObject(objClone);

					selTool.SetSelected(Entity, TRUE);
                }
            }
            break;
        default :
            break;
        }
    }

    return S_OK;
}
int KG3DScenePvsEditor::DeleteSelectedObject()
{
    if (!m_pPvs)
        return 0;

    int nRemove = 0;

   
	
	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
	TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();


	//Selection里面的东东要进行内部清除（ClearSelection）和外部清除，外部清除应该在内部清除完成后再做
	//，不然内部清除的时候，里面的物件已经被外部释放了
	
	size_t uSelCount = (size_t)(selTool.GetSelectionCount());
	std::vector<KG3DSceneEntity> vecEntities;
	vecEntities.reserve(uSelCount);
	for (proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
    {
		KG3DSceneEntity& entityCur = proxy.GetCur();
		vecEntities.push_back(entityCur);
    }

	selTool.ClearSelection();

	std::vector<KG3DRepresentObject*> vObjs;
	vObjs.reserve(uSelCount);

	for (size_t i = 0, uSize = vecEntities.size(); i < uSize; ++i)
	{
		KG3DSceneEntity& entityCur = vecEntities[i];
		switch (entityCur.GetType())
		{
		case SCENEENTITY_OBJECT :
			{
				//selTool.SetSelectable(/**i*/entityCur, FALSE);
				this->InsertEntitySelectable(entityCur, FALSE);//<Selectable Refactor>

				if (SUCCEEDED(m_pPvs->DelObject(static_cast<KG3DRepresentObject*>(entityCur.m_pPointer), this)))
					++nRemove;

				vObjs.push_back(static_cast<KG3DRepresentObject*>(entityCur.m_pPointer));

			}
			break;
		case SCENEENTITY_PVS_PORTAL :
			{
				//selTool.SetSelectable(/**i*/entityCur, FALSE);
				this->InsertEntitySelectable(entityCur, FALSE);//<Selectable Refactor>

				m_pPvs->DeletePortal(static_cast<KG3DPvsPortal*>(entityCur.m_pPointer));
			}
			break;
		default :
			break;
		} 
	}

    for (size_t i = 0; i < vObjs.size(); ++i)
    {
        for (size_t e = 0; e < m_vpEventCallBack.size(); ++e)
            m_vpEventCallBack[e]->OnDelObject(vObjs[i]);
    }
    
    return nRemove;
}

bool KG3DScenePvsEditor::GetPlaceObjectPos(D3DXVECTOR3& vPos, KG3DSceneOutputWnd* pWnd)
{
    ASSERT(pWnd);

    if (!pWnd || !m_pPvs)
        return false;

    RECT rectWnd;
    POINT ptCursor;

    GetWindowRect(pWnd->m_hWnd, &rectWnd);
    GetCursorPos(&ptCursor);

    if (!PtInRect(&rectWnd, ptCursor))
        return false;

    D3DXVECTOR3 v1;
    D3DXVECTOR3 v2;
    D3DXVECTOR3 dir;
    const D3DXPLANE plane = D3DXPLANE(0.0f, 1.0f, 0.0f, 0.f);

    pWnd->GetPickRay(&v1, &dir, NULL); 
    v2 = v1 + (dir * 100000.0f);
    D3DXPlaneIntersectLine(&vPos, &plane, &v1, &v2);

    D3DXVECTOR3 vSrc[1] = { v1 };
    D3DXVECTOR3 vDst[1] = { v2 };
    float fDis = 0.f;

    if (m_pPvs->GetCameraNearRayIntersect(vDst, vSrc, 1, &fDis))
        vPos = vDst[0] - dir * fDis;

    return true;
}

bool KG3DScenePvsEditor::GetPlacePortalPos(D3DXVECTOR3& vPos, KG3DSceneOutputWnd* pWnd, KG3DPvsPortal* ptl)
{
    ASSERT(pWnd);

    if (!pWnd || !m_pPvs || !ptl)
        return false;

    RECT rectWnd;
    POINT ptCursor;

    GetWindowRect(pWnd->m_hWnd, &rectWnd);
    GetCursorPos(&ptCursor);

    if (!PtInRect(&rectWnd, ptCursor))
        return false;

    D3DXVECTOR3 v1;
    D3DXVECTOR3 v2;
    D3DXVECTOR3 dir;
    const D3DXPLANE plane = D3DXPLANE(0.0f, 1.0f, 0.0f, 0.f);

    pWnd->GetPickRay(&v1, &dir, NULL); 
    v2 = v1 + (dir * 100000.0f);
    D3DXPlaneIntersectLine(&vPos, &plane, &v1, &v2);

    D3DXVECTOR3 vScal;
    ptl->GetScaling(&vScal);
    vScal *= 0.5f;

    D3DXVECTOR3 vSrc[5] = 
    {
        v1,
        v1 - pWnd->GetCamera().GetCameraRight() * vScal.x + pWnd->GetCamera().GetCameraUp() * vScal.y,
        v1 + pWnd->GetCamera().GetCameraRight() * vScal.x + pWnd->GetCamera().GetCameraUp() * vScal.y,
        v1 + pWnd->GetCamera().GetCameraRight() * vScal.x - pWnd->GetCamera().GetCameraUp() * vScal.y,
        v1 - pWnd->GetCamera().GetCameraRight() * vScal.x - pWnd->GetCamera().GetCameraUp() * vScal.y
    };

    D3DXVECTOR3 vDst[5] = 
    {
        v2,
        v2 - pWnd->GetCamera().GetCameraRight() * vScal.x + pWnd->GetCamera().GetCameraUp() * vScal.y,
        v2 + pWnd->GetCamera().GetCameraRight() * vScal.x + pWnd->GetCamera().GetCameraUp() * vScal.y,
        v2 + pWnd->GetCamera().GetCameraRight() * vScal.x - pWnd->GetCamera().GetCameraUp() * vScal.y,
        v2 - pWnd->GetCamera().GetCameraRight() * vScal.x - pWnd->GetCamera().GetCameraUp() * vScal.y
    };

    float fDis = 0.f;

    if (m_pPvs->GetCameraNearRayIntersect(vDst, vSrc, 5, &fDis))
        vPos = v2 - dir * fDis;

    return true;
}

void KG3DScenePvsEditor::Place2Pvs()
{
    if (!m_pPvs)
        return;

    if (m_pHandObj)
    {
        KG3DRepresentObject* obj = NULL;
        if (SUCCEEDED(g_GetObjectTable().GetCopy(&obj, m_pHandObj)))
        {
            obj->FrameMove();
            m_pPvs->AddObject(obj, this);

			//KG3DSceneEntity entity(SCENEENTITY_OBJECT, obj);
			//this->GetSelectionTool().SetSelectable(entity, TRUE);
			this->IEInsertEntitySelectable(SCENEENTITY_OBJECT, obj, TRUE);//<Selectable Refactor>

            for (size_t i = 0; i < m_vpEventCallBack.size(); ++i)
                m_vpEventCallBack[i]->OnAddObject(obj);
        }

        if (m_listPlaceObj.size() == 0 || m_pHandObj->m_scName != m_listPlaceObj.back())
            m_listPlaceObj.push_back(m_pHandObj->m_scName);
    }

    if (m_pHandPtl)
    {
        KG3DPvsPortal* ptl = NULL;
        m_pPvs->AddPortal(ptl, m_pHandPtl->GetMatrix());

		/*KG3DSceneEntity entity(SCENEENTITY_PVS_PORTAL, ptl);
		this->GetSelectionTool().SetSelectable(entity, TRUE);*///<Selectable Refactor>
		this->IEInsertEntitySelectable(SCENEENTITY_PVS_PORTAL, ptl, TRUE);

    }
}

void KG3DScenePvsEditor::ScalHandObj(float fScal)
{
    if (m_pHandObj)
    {
        D3DXVECTOR3 vScal;
        m_pHandObj->GetScaling(&vScal);
        fScal *= 0.1f;
        vScal += D3DXVECTOR3(fScal, fScal, fScal);
        m_pHandObj->SetScaling(&vScal);
    }

    if (m_pHandPtl)
    {
        D3DXVECTOR3 vScal;
        m_pHandPtl->GetScaling(&vScal);
        fScal *= 5.f;
        vScal += D3DXVECTOR3(fScal, fScal, 0.f);
        m_pHandPtl->SetScaling(&vScal);
    }
}

namespace {

    struct _Compare
    {
        bool operator() (const KG3DSceneEntity& left, const KG3DSceneEntity& rigt) 
        {
            D3DXMATRIX  mat_left = ((KG3DSceneEntity&)left).GetWorldMatrix();    
            D3DXMATRIX  mat_rigt = ((KG3DSceneEntity&)rigt).GetWorldMatrix();
    
            D3DXVECTOR3 pos_left = *(D3DXVECTOR3*)&mat_left._41;
            D3DXVECTOR3 pos_rigt = *(D3DXVECTOR3*)&mat_rigt._41;       
      
            return D3DXVec3LengthSq(&(pos_left - m_vCenter)) < D3DXVec3LengthSq(&(pos_rigt - m_vCenter));  
        }

        _Compare(const KG3DSceneEntity& center)
        {
            D3DXMATRIX matWorld = ((KG3DSceneEntity&)center).GetWorldMatrix();
            m_vCenter = *(D3DXVECTOR3*)&matWorld._41;
        }

        D3DXVECTOR3 m_vCenter;
    };

}

HRESULT KG3DScenePvsEditor::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)//<PROCESS_MESSAGE>
{
    switch (message)
    {
    case WM_LBUTTONUP :
        Place2Pvs();
        break;
    case WM_KEYDOWN :
        {
            switch (wParam)
            {
            case VK_SHIFT :
                {
                    if (!m_bSwitchObj && m_listPlaceObj.size() > 0)
                    {
                        m_itorListObj = m_listPlaceObj.end();
                        --m_itorListObj;
                    }
                    m_bSwitchObj = true;
                }
                break;
            case VK_TAB :
                {
					KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
                    //if (m_listSelectedEntity.size() <= 0)
                    //    break;

					if(selTool.GetSelectionCount() <= 0)
						break;


                    if (m_listSelTabEntity.size() == 0)
                    {
                        //KG3DSceneEntity frontEntity = m_listSelectedEntity.front();
						selTool.GetSelectionAccessProxy().Begin();
						KG3DSceneEntity frontEntity = selTool.GetSelectionAccessProxy().GetCur();	//注意前面做了选择非空的判断，不然这里是有问题的

                        //m_listSelTabEntity = m_listSelectableEntity;
						/*2009-1-8 新的选择工具不支持直接复制内部的选择体，但是支持创建选择组。但是这里问题比较严重，因为需要sort，而选择
						工具内部维护的选择表有自己的结构，不一定能支持sort，所以这里把选择表复制出来*/
						selTool.GetSelectableList(m_listSelTabEntity);

                        m_listSelTabEntity.sort(_Compare(frontEntity));
                        m_itorTabSel = m_listSelTabEntity.begin();
                    }

                    if (++m_itorTabSel == m_listSelTabEntity.end())
                        m_itorTabSel = m_listSelTabEntity.begin();

                    //ClearSelectedEntity();
					this->GetSelectionTool().ClearSelection();
                    
					//AddSelectedEntity(*m_itorTabSel);
					this->GetSelectionTool().SetSelected(*m_itorTabSel, TRUE);

      //              switch (this->GetEditState())
      //              {
      //              //case SCENESTATE_MOVE :
						////OnToggleStateMove();//<OnToggleStateMove Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
      //                 // break;
      //              //case SCENESTATE_ROTATE :
      //                  //OnToggleStateRotation();//<OnToggleStateRotation Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
      //                 // break;
      //             /* case SCENESTATE_SCALE :
				  // OnToggleStateScaling();//<OnToggleStateScaling Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
      //                  break;*/
      //              default :
      //                  break;
      //              }
                    
                }
                break;
            default :
                break;
            }
        }
        break;
    case WM_KEYUP :
        {
            switch (wParam)
            {
            case VK_SHIFT :
                {
                    if (m_listPlaceObj.size() > MAX_PLACE_OBJ_HISTORY_SIZE)
                    {
                        list<string>::iterator ito = m_listPlaceObj.begin();
                        advance(ito, MAX_PLACE_OBJ_HISTORY_SIZE / 10);
                        m_listPlaceObj.erase(m_listPlaceObj.begin(), ito);
                    }
                    m_bSwitchObj = false;
                }
                break;
            default :
                break;
            }
        }
        break;
    case WM_MOUSEWHEEL :
        {
            short wheel = static_cast<short>(HIWORD(wParam));

            if (m_bSwitchObj && m_listPlaceObj.size() > 0)
            {
                list<string>::iterator last = m_listPlaceObj.end();
                --last;

                if (wheel < 0 && m_itorListObj != last)
                    ++m_itorListObj;

                if (wheel > 0 && m_itorListObj != m_listPlaceObj.begin())
                    --m_itorListObj;

                SetHandObject((*m_itorListObj).c_str());
            }

            if (m_bSwitchObj)
                return S_OK;
        }
        break;
    case WM_RBUTTONDOWN :
        {
            if (m_bRunMode && m_pRunder)
                m_pRunder->ProcessRButtonDn(wParam, lParam);
        }
        break;
    case WM_RBUTTONUP :
        {
            if (m_bRunMode && m_pRunder)
                m_pRunder->ProcessRButtonUp(wParam, lParam);
        }
        break;
    default :
        break;
    }

    KG3DSceneEditorBase::ProcessMessage(message, wParam, lParam);

    return S_OK;
}

void KG3DScenePvsEditor::MsgOnMouseWheel(WPARAM wParam, LPARAM lParam)//<PROCESS_MESSAGE>
{
    if (m_bRunMode)
    {
        int k = 0;
        k |=wParam;
        float S = 1.0F;
        if (k>0)
            S = 0.9F;
        else 
            S = 1.1F;
        m_vCamera.x *= S;

        KG3DTrackCamera* pTrackCamera = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetTrackCameraInterface();
        if(NULL != pTrackCamera)
            pTrackCamera->ProcessMouseWeel(wParam, lParam);
    }
    else
    {
        KG3DSceneEditorBase::MsgOnMouseWheel(wParam, lParam);
    }
}

void KG3DScenePvsEditor::SetCamareState(WORD wState, BOOL bSet)
{
    if (bSet)
        m_CamareState |= WORD(wState);
    else
        m_CamareState &= ~WORD(wState);
}

void KG3DScenePvsEditor::TrackCamareFrameMove()
{
    D3DXVECTOR3 Pos,LookAt, vDelta, vFront, vRight, vUp;
    m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetPosition(&Pos);
    m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetLookAtPosition(&LookAt);
    vUp = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraUp();
    vFront = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraFront();
    vRight = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraRight();

    D3DXVec3Normalize(&vFront, &vFront);
    D3DXVec3Normalize(&vRight, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);

    if (m_CamareState & KG3DSceneSceneEditor::cmsCamareUp)
    {
        vUp *= float(g_eEngineOption.nFlySpeedY);
        Pos += vUp;
        LookAt += vUp;
    }
    else if (m_CamareState & KG3DSceneSceneEditor::cmsCamareDown)
    {
        vUp *= float(g_eEngineOption.nFlySpeedY);
        Pos -= vUp;
        LookAt -= vUp;
    }
    if (m_CamareState & KG3DSceneSceneEditor::cmsFoward)
    {
        vFront *= float(g_eEngineOption.nFlySpeedY);
        Pos += vFront;
        LookAt += vFront;
    }
    else if (m_CamareState & KG3DSceneSceneEditor::cmsBack)
    {
        vFront *= -float(g_eEngineOption.nFlySpeedY);
        Pos += vFront;
        LookAt += vFront;
    }
    if (m_CamareState & KG3DSceneSceneEditor::cmsMoveLeft)
    {
        vRight *= -float(g_eEngineOption.nFlySpeedX);
        Pos += vRight;
        LookAt += vRight;
    }
    else if (m_CamareState & KG3DSceneSceneEditor::cmsMoveRight)
    {
        vRight *= float(g_eEngineOption.nFlySpeedX);
        Pos += vRight;
        LookAt += vRight;
    }

    m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetPositionLookAtUp(&Pos, &LookAt, NULL);
}

INT KG3DScenePvsEditor::ExecAction(INT nActionID, INT nExecState, WPARAM wParam, LPARAM lParam)
{
    INT nRet = 1;

    int xPos = LOWORD(lParam); 
    int yPos = HIWORD(lParam);		

    xPos = xPos > 0x7fff ? (xPos - 0xffff) : xPos;
    yPos = yPos > 0x7fff ? (yPos - 0xffff) : yPos;

    KG3DCamera& camera = m_SceneWndManager->GetCurOutputWnd()->GetCamera();
    D3DXVECTOR3 Pos,LookAt;
    D3DXVECTOR3 Left = camera.GetCameraRight();
    D3DXVECTOR3 Front = camera.GetCameraFront();
    D3DXVECTOR3 Up = camera.GetCameraUp();

    camera.GetPosition(&Pos);
    camera.GetLookAtPosition(&LookAt);

    switch (nActionID)
    {	
    case EXEACTION_ROTATE_VIEW: //shift+中键，以选定的点为中心旋转摄像机视角
        {
            if (nExecState == 0)
            {
                m_bMouseTargetMoving = FALSE;
                break;
            }

            if(!m_bMouseTargetMoving)
            {
                m_bMouseTargetMoving = TRUE;
            }
            else
            {
                if(m_SceneWndManager->GetCurOutputWnd())
                {
                    D3DXVECTOR3 Pos,LookAt;
                    m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetPosition(&Pos);
                    m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetLookAtPosition(&LookAt);

                    //D3DXVECTOR3 Right = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetCameraRight();

                    D3DXVECTOR3 VE = Pos - m_vMouseTargetPosition;
                    D3DXVECTOR3 VL = LookAt - m_vMouseTargetPosition;

                    //D3DXVECTOR3 N;
                    //D3DXVec3Cross(&N,&VE,&VL);

                    extern BOOL g_bDesign;
                    float cmspeed = g_bDesign ? m_CameraSpeed * 0.00001f : 0.003F;
                    float fx = (xPos - g_nMouseMoveX) * cmspeed;
                    float fy = (yPos - g_nMouseMoveY) * cmspeed;

                    D3DXMATRIX mat;
                    D3DXMatrixRotationYawPitchRoll(&mat,fx,fy,0);

                    D3DXVECTOR3 VEnew,VLnew;
                    D3DXVec3TransformCoord(&VEnew,&VE,&mat);
                    D3DXVec3TransformCoord(&VLnew,&VL,&mat);

                    D3DXVECTOR3 Posnew,LookAtnew;
                    Posnew = m_vMouseTargetPosition + VEnew;
                    LookAtnew = m_vMouseTargetPosition + VLnew;

                    /*m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetPosition(Posnew);
                    m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetLookAtPosition(LookAtnew);*/

                    m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetPositionLookAtUp(&Posnew, &LookAtnew, NULL);
                }
            }
        }
        break;
    case EXEACTION_GO_FORWARD://前进
        if (!!m_bRunMode){
            m_pRunder->SetNpcMoveBits(KG3DRepresentNPC::nmsFoward, nExecState);
        }
        else
            SetCamareState(KG3DSceneSceneEditor::cmsFoward, nExecState);
        break;
    case EXEACTION_GO_BACKWARD://后退
        if (!!m_bRunMode){
            m_pRunder->SetNpcMoveBits(KG3DRepresentNPC::nmsBack, nExecState);
        }
        else
            SetCamareState(KG3DSceneSceneEditor::cmsBack, nExecState);
        break;
    case EXEACTION_TURN_LEFT://左转
        if (!!m_bRunMode){
            m_pRunder->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnLeft, nExecState);
        }
        else
            nRet = 0;//需要放行给WINDOWS处理
        break;
    case EXEACTION_TURN_RIGHT://右转
        if (!!m_bRunMode){
            m_pRunder->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnRight, nExecState);
        }
        else
            nRet = 0;//需要放行给WINDOWS处理
        break;
    case EXEACTION_MOV_LEFT://左移
        if (!!m_bRunMode){
            m_pRunder->SetNpcMoveBits(KG3DRepresentNPC::nmsMoveLeft, nExecState);
        }
        else
            SetCamareState(KG3DSceneSceneEditor::cmsMoveLeft, nExecState);
        break;
    case EXEACTION_MOV_RIGHT://右移
        if (!!m_bRunMode){
            m_pRunder->SetNpcMoveBits(KG3DRepresentNPC::nmsMoveRight, nExecState);
        }
        else
            SetCamareState(KG3DSceneSceneEditor::cmsMoveRight, nExecState);

        break;
    case EXEACTION_MOV_UP://上升
        if (!m_bRunMode)
            SetCamareState(KG3DSceneSceneEditor::cmsCamareUp, nExecState);
        break;
    case EXEACTION_MOV_DOWN://下降
        if (!m_bRunMode)
            SetCamareState(KG3DSceneSceneEditor::cmsCamareDown, nExecState);
        break;
    case EXEACTION_CAM_CIRCLE://转动摄像机
        if (!!m_bRunMode){
            m_pRunder->SetNpcMoveBits(KG3DRepresentNPC::nmsCamareTurn, nExecState);
        }
        else
            nRet = 0;//需要放行给WINDOWS处理
        break;
    case EXEACTION_JUMP://跳
        if (!!m_bRunMode){
            m_pRunder->SetNpcMoveBits(KG3DRepresentNPC::nmsJump, nExecState);
        }
        else
            nRet = 0;//需要放行给WINDOWS处理		
        break;
    case EXEACTION_TURN_LEFT_VIEW_FOLLOWED://转身同时转动摄像机
        if (!!m_bRunMode){
            m_pRunder->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnBodyLeft, nExecState);
        }
        else
            nRet = 0;//需要放行给WINDOWS处理
        break;
    case EXEACTION_TURN_RIGHT_VIEW_FOLLOWED://转身同时转动摄像机
        if (!!m_bRunMode){
            m_pRunder->SetNpcMoveBits(KG3DRepresentNPC::nmsTurnBodyRight, nExecState);
        }
        else
            nRet = 0;//需要放行给WINDOWS处理
        break;
    case EXEACTION_SPEED_UP://加速
        if (nExecState)
        {
            if (m_bEditing)
            {
                g_eEngineOption.nFlySpeedX += 5;
                g_eEngineOption.nFlySpeedY += 5;
            }
            else{
                m_pRunder->m_fMoveSpeed += 50.0f;
                m_pRunder->m_fMoveSpeed = min(10000.0f, m_pRunder->m_fMoveSpeed);
            }
        }
        break;
    case EXEACTION_SPEED_DOWN://减速
        if (nExecState)
        {
            if (!m_bRunMode)
            {
                g_eEngineOption.nFlySpeedX -= 5;
                g_eEngineOption.nFlySpeedY -= 5;
				g_eEngineOption.nFlySpeedX = max(g_eEngineOption.nFlySpeedX, 5);
				g_eEngineOption.nFlySpeedY = max(g_eEngineOption.nFlySpeedY, 5);
            }
            else
            {
                m_pRunder->m_fMoveSpeed -= 50.0f;
                m_pRunder->m_fMoveSpeed = max(100.0f, m_pRunder->m_fMoveSpeed);
            }
        }
        break;
	case EXEACTION_LEFE_KEY_DOWN://点选键按下
	case EXEACTION_SELECTION_ADD://在当前选择范围中，再增加选择内容
	case EXEACTION_SELECTION_SUTRACT://在当前选择范围中，再减少选择内容
		for (size_t i = 0; i < m_vpEventCallBack.size(); ++i)
			m_vpEventCallBack[i]->OnSelectedChanged();
    default:
        nRet = KG3DSceneEditorBase::ExecAction(nActionID, nExecState, wParam, lParam);	
    }

    return nRet;
}


void KG3DScenePvsEditor::OnCommand(enumCommandType eCommandType)
{
    switch(eCommandType)
    {		
    case ctInc: //放大缩小当前的笔刷一格			
        ExecAction(EXEACTION_INNER_CIRCLE, 1, 0, MAKELPARAM(g_nMouseMoveX+1, g_nMouseMoveY+1));
        break;
    case ctDec:
        ExecAction(EXEACTION_INNER_CIRCLE, 1, 0, MAKELPARAM(g_nMouseMoveX-1, g_nMouseMoveY-1));
        break;
    case ctHideObject: //隐藏物件
        g_cEngineManager.GetRenderOption( RENDER_ENTITY ) ? g_cEngineManager.SetRenderOption( RENDER_ENTITY ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_ENTITY ,TRUE);
        break;
    case ctHideTerrain://隐藏地表
        g_cEngineManager.GetRenderOption( RENDER_TERRAIN ) ? g_cEngineManager.SetRenderOption( RENDER_TERRAIN ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_TERRAIN ,TRUE);
        break;
    case ctHideGrass:  //隐藏草
        g_cEngineManager.GetRenderOption( RENDER_GRASS ) ? g_cEngineManager.SetRenderOption( RENDER_GRASS ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_GRASS ,TRUE);
		break;
    case ctHideTree:  //隐藏树叶
        g_cEngineManager.GetRenderOption( RENDER_TREE ) ? g_cEngineManager.SetRenderOption( RENDER_TREE ,FALSE) : g_cEngineManager.SetRenderOption( RENDER_TREE ,TRUE);
        break;
    case ctHideNPCAttackRange://隐藏NPC攻击范围
        {
			TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();


            //KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
            //while(i!=m_listSelectedEntity.end())

			for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
            {
                //KG3DSceneEntity Entity = *i;
				KG3DSceneEntity Entity = proxy.GetCur();

                if(Entity.GetType() == SCENEENTITY_OBJECT)
                {
                    KG3DRepresentObject* pObject = (KG3DRepresentObject* ) Entity.m_pPointer;
                    if(pObject->GetType() == REPRESENTOBJECT_NPC)
                    {
                        FireEvent2(EVENTEDITORNPCHIDEATTACKRANGE,0,0,pObject);
                    }
                }
                //i++;
            }
        }
        break;
    case ctEnd:    //P模式时，复位摄像机到后面
        break;
    default:
        KG3DSceneEditorBase::OnCommand(eCommandType);
        break;
    }
}

//HRESULT KG3DScenePvsEditor::GetStandHeight(DWORD& dwCount,D3DXVECTOR3 *pInter,D3DXVECTOR3& vIn,DWORD dwMaxCount)
//{
//    dwCount = 0;
//    D3DXVECTOR3 Out(vIn.x,0,vIn.z);
//    Out.y = 0.f;
//
//    dwCount++;
//    pInter[0] = Out;
//
//    if (!m_pPvs)
//        return S_OK;
//
//  
//    D3DXVECTOR3 vecInter[128];
//    DWORD dwEntityCount = 0;
//    D3DXVECTOR3 vEntityInter[128];
//
//    if (m_pPvs->GetRayIntersect(dwEntityCount,vEntityInter,128,vIn,D3DXVECTOR3(0,1,0), FALSE))
//    {
//        for(DWORD s=0;s<dwEntityCount;s++)
//        {
//            if(s+dwCount<dwMaxCount)
//            {
//                pInter[s+dwCount] = vEntityInter[s];
//            }
//            else
//                break;
//        }
//
//        dwCount += dwEntityCount;
//        if(dwCount >= dwMaxCount)
//        {
//            dwCount = dwMaxCount;
//        }
//    }
//
//    return S_OK;
//}

HRESULT KG3DScenePvsEditor::FrameMove()
{
    KG3DSceneEditorBase::FrameMove();
    GetEnvironment().FrameMove();
	if(m_lpLightMapBaker)
		m_lpLightMapBaker->FrameMove();

    return S_OK;
}

HRESULT KG3DScenePvsEditor::RenderForEachWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
    switch (pWnd->GetType())
    {
    case OUTPUTWND_PVSEDITOR_MAIN :
        RenderForMainWindow(wndCur);
        break;
    case OUTPUTWND_SCENEEDITOR_OBJECTPREVEIW :
        RenderObjViewWindow(pWnd);
        break;
    default:
        break;
    }

    return S_OK;
}

HRESULT KG3DScenePvsEditor::RenderForMainWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
    KG3DRenderState RenderState;

    D3DXVECTOR3 vPlacePos;
    D3DXMATRIX  matProj;
    D3DXMATRIX  matView;

    KG3DRepresentObjectPVS* pPvs = m_pPvs;

    ProcessSpaceNodeByCamera(&pWnd->GetCamera());
    CalcFrustum(&pWnd->GetCamera());

    if (KG3DSceneSpaceNode::GetTopVisableNode() != g_cGraphicsTool.GetCurScene()->m_lpSceneOutDoorSpaceMgr)
        pPvs = static_cast<KG3DRepresentObjectPVS*>(KG3DSceneSpaceNode::GetTopVisableNode());
    
    if (pPvs)
        pPvs->UpdateVisiableSubset(NULL);

    if (m_bRunMode && m_pRunder)
    {
        KG3DTrackCamera* pTrackCamera = pWnd->GetCamera().GetTrackCameraInterface();

        D3DXVECTOR3 vecNpcPos;
        D3DXVECTOR3 vecNpcDir;

        m_pRunder->GetCurPosition(&vecNpcPos);
        m_pRunder->GetDirection(&vecNpcDir);
        vecNpcPos = vecNpcPos + D3DXVECTOR3(0.0f, 180.0f, 0.0f);

        pTrackCamera->BindScene(this);
        pTrackCamera->UpdateAsClient(vecNpcPos, vecNpcDir, m_pRunder->m_fMoveSpeed, m_pRunder->m_dwMovementType);
        pTrackCamera->FrameMove();
    }
    else
        TrackCamareFrameMove();

    if (m_lpPointLightRender)
        m_lpPointLightRender->FrameMove();

    RenderWindowBegin(pWnd,TRUE);

	if(m_lpLightMapBaker && m_lpLightMapBaker->m_bShowDebug)
	{
		m_lpLightMapBaker->ShowDebug();
	}
	else
	{
		RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
		RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
		RenderState.SetSamplerState(0, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
		RenderState.SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
		RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RenderState.SetRenderState(D3DRS_ZENABLE, TRUE);

		g_cGraphicsTool.ApplyLight(*this);

		g_SetShaderSharedParams(*this, wndCur, NULL, NULL);//这个没有ConverMap
		if (m_pPvs)
		{
			m_pPvs->FrameMove();
			m_pPvs->RenderHierarchy(m_bCull, m_lpPointLightRender);
		}

		if (m_pHandObj)
		{
			if (GetPlaceObjectPos(vPlacePos, pWnd))
				m_pHandObj->SetTranslation(&vPlacePos);
			m_pHandObj->FrameMove();
			m_pHandObj->RenderHierarchy();
		}

		if (m_pHandPtl)
		{
			D3DXVECTOR3 vScal = D3DXVECTOR3(1.f, 1.f, 1.f);
			D3DXMATRIX  matLocal;
			D3DXMATRIX  matScal;

			D3DXMatrixIdentity(&matLocal);

			switch (m_dwPlacePortalMode)
			{
			case PLACE_PTL_BILLBOARD :    
				*((D3DXVECTOR3*)&matLocal.m[0]) = pWnd->GetCamera().GetCameraRight();
				*((D3DXVECTOR3*)&matLocal.m[1]) = pWnd->GetCamera().GetCameraUp();
				*((D3DXVECTOR3*)&matLocal.m[2]) = pWnd->GetCamera().GetCameraFront();
				break;
			case PLACE_PTL_LOCK_Y :
				*((D3DXVECTOR3*)&matLocal.m[0]) = pWnd->GetCamera().GetCameraRight();
				*((D3DXVECTOR3*)&matLocal.m[1]) = D3DXVECTOR3(0.f, 1.f, 0.f);
				D3DXVec3Cross((D3DXVECTOR3*)&matLocal.m[2], (D3DXVECTOR3*)&matLocal.m[0], (D3DXVECTOR3*)&matLocal.m[1]);
				break;
			case PLACE_PTL_HOR :
				*((D3DXVECTOR3*)&matLocal.m[0]) = D3DXVECTOR3(1.f, 0.f, 0.f);
				*((D3DXVECTOR3*)&matLocal.m[1]) = D3DXVECTOR3(0.f, 0.f, 1.f);
				*((D3DXVECTOR3*)&matLocal.m[2]) = D3DXVECTOR3(0.f, -1.f, 0.f);
				break;
			default :
				break;
			}

			if (GetPlacePortalPos(vPlacePos, pWnd, m_pHandPtl))
				*((D3DXVECTOR3*)&matLocal.m[3]) = vPlacePos;

			m_pHandPtl->GetScaling(&vScal);
			D3DXMatrixScaling(&matScal, vScal.x, vScal.y, vScal.z);

			matLocal = matScal * matLocal;

			m_pHandPtl->UpdateByMatrix(matLocal);

			m_pHandPtl->FrameMove();
			m_pHandPtl->Render(0x550000ff);
		}

		if (m_bRunMode && m_pRunder)
		{
			//m_pRunder->NPCFrameMove();

			_ASSERTE(NULL != m_pRunder);
			KSF::FrameMoveSceneRunner(*m_pRunder, *this);

			m_pRunder->RenderHierarchy();
		}

		if (m_pPvs)
			m_pPvs->Render(0x550000ff);

		for_each(m_listRenderEntity.begin(), m_listRenderEntity.end(), mem_fun_ref(&KG3DSceneEntity::Render));
		//////////////////////////////////////////////////////////////////////////
		if(m_lpLightMapBaker)
			m_lpLightMapBaker->Render();

		matProj = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetProjectionMatrix();
		matView = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetViewMatrix();

		if (!m_bRunMode)
			RenderSelectedEntityMark(this->GetEditState(), m_SceneWndManager->GetCurOutputWnd()->m_Viewport, matProj, matView, TRUE);

		DrawGroundGrid();
		DrawInfo();

		if (g_cEngineManager.GetFocusScene() == this)
		{
			g_cGraphicsTool.DrawFocusRect(
				m_SceneWndManager->GetCurOutputWnd()->m_Viewport, 
				4,
				0xFFFFFF00
				);
		}


		GetEnvironment().Render();
	}

    RenderWindowEnd(pWnd);

    if (m_pPvs)
        m_pPvs->ClearFrustums();

    return S_OK;
}


HRESULT KG3DScenePvsEditor::RenderObjViewWindow(KG3DSceneOutputWnd* pWnd)
{
    RenderWindowBegin(pWnd,TRUE);

    if (m_pHandObj)
    {
        float ObjRadius  = m_pHandObj->m_BBox.GetDiameter() / 2.f;
        float RotationAngle = g_cGraphicsTool.GetNowTime() * 0.0001F;
        D3DXVECTOR3 Direction = D3DXVECTOR3(sinf(RotationAngle), 0.5f, cosf(RotationAngle));
        
        D3DXVec3Normalize(&Direction, &Direction);

        KG3DCamera& Cam = pWnd->GetCamera();
        FLOAT fHalfFovY = Cam.GetPerspectiveData().fFovy / 2.f;

        FLOAT fMinDistance = fHalfFovY < FLT_EPSILON 
            ? 2 * ObjRadius 
            : ObjRadius / tan(fHalfFovY) + Cam.GetPerspectiveData().fzNear + 10;	

        D3DXVECTOR3 ObjectPos = m_pHandObj->GetTranslation();
        D3DXVECTOR3 vCameraLookAt = ObjectPos;
        vCameraLookAt.y += m_pHandObj->m_BBox.GetHeight() / 2;
        D3DXVECTOR3 vCameraPos = Direction * fMinDistance + vCameraLookAt;

        Cam.SetPositionLookAtUp(&vCameraPos, &vCameraLookAt, NULL);

        m_pHandObj->RenderHierarchy();


    }

    RenderWindowEnd(pWnd);	

    return S_OK;
}


void KG3DScenePvsEditor::DrawInfo()
{   
    TCHAR szInfo[128];

    /* calc fps */

    m_dwCurrTime = timeGetTime();
    m_dwTimer   += m_dwCurrTime - m_dwPrevTime;
    m_fCounter  += 1.f;

    if (m_dwTimer > 1000)
    {
        m_fFPS     = m_fCounter * 1000.f / static_cast<FLOAT>(m_dwTimer);
        m_fCounter = 0.f;
        m_dwTimer  = 0;
    }

    sprintf(szInfo, TEXT("FPS : %.2f"), m_fFPS);


    /* show fps */
    g_cGraphicsTool.DrawDebugText(szInfo);

    m_dwPrevTime = m_dwCurrTime;

    /* show pvs object info */
    if (m_pPvs)
    {
        sprintf(szInfo, TEXT("\n\nInside Object Num : %u\nPortal Num : %u\nVisable Object Num : %u"), 
            m_pPvs->m_setObjectInside.size(), 
            m_pPvs->m_listPortal.size(),
            m_pPvs->m_setObjectVisiable.size()
            );
        g_cGraphicsTool.DrawDebugText( szInfo);
    }

	g_cGraphicsTool.DrawAllDebugTextsAtLast();
}

void KG3DScenePvsEditor::DrawGroundGrid()
{
    struct _Vertex  { D3DXVECTOR3 v; DWORD c; };
    const int nGridNum = 40;

    UINT uVertexNum = (nGridNum + 1) * 4;

    if (!m_pGroundGridVertexs)
    {
        HRESULT hr = g_pd3dDevice->CreateVertexBuffer(
            uVertexNum * sizeof(_Vertex),
            0,
            D3DFVF_XYZ | D3DFVF_DIFFUSE,
            D3DPOOL_MANAGED,
            &m_pGroundGridVertexs,
            NULL
            );
        if (SUCCEEDED(hr))
        {
            _Vertex* pVertexs = NULL;

            if (SUCCEEDED(m_pGroundGridVertexs->Lock(0, 0, (void**)&pVertexs, 0)))
            {
                int strick = (nGridNum + 1);
                for (int i = 0; i < (nGridNum + 1); i ++)
                {
                    pVertexs[i * 2].v     = D3DXVECTOR3(-2000.f, 0.f, 2000.f - i * 100.f);
                    pVertexs[i * 2 + 1].v = D3DXVECTOR3( 2000.f, 0.f, 2000.f - i * 100.f);
                    pVertexs[i * 2].c     = 0xFF606060;
                    pVertexs[i * 2 + 1].c = 0xFF606060;
                }
                for (int i = (nGridNum + 1); i < (nGridNum + 1) * 2; i++)
                {
                    pVertexs[i * 2].v     = D3DXVECTOR3(2000.f - (i - strick) * 100.f, 0.f,  2000.f); 
                    pVertexs[i * 2 + 1].v = D3DXVECTOR3(2000.f - (i - strick) * 100.f, 0.f, -2000.f);
                    pVertexs[i * 2].c     = 0xFF606060;
                    pVertexs[i * 2 + 1].c = 0xFF606060;
                }

                pVertexs[nGridNum].c = 0xFF000000;
                pVertexs[nGridNum + 1].c = 0xFF000000;
                pVertexs[(nGridNum + 1) * 3 - 1].c = 0xFF000000;
                pVertexs[(nGridNum + 1) * 3].c = 0xFF000000;

                m_pGroundGridVertexs->Unlock();
            }  
        }
        else
        {
            SAFE_RELEASE(m_pGroundGridVertexs);
        }

    }

    if (m_pGroundGridVertexs)
    {
        KG3DRenderState RenderState;
        
        RenderState.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
        RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);

        g_pd3dDevice->SetTexture(0, NULL);
        g_pd3dDevice->SetStreamSource(0, m_pGroundGridVertexs,0, sizeof(_Vertex));
        g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
        g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, (nGridNum + 1) * 2);
    }

}

void __stdcall KG3DScenePvsEditor::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	if (Message.m_uMessage == KM_SELECT)
	{
		if (Message.m_dwLParam == EM_EVENT_STATE_END)
		{
			m_listSelTabEntity.clear();
		}
	}
}

HRESULT KG3DScenePvsEditor::BakeLightMap(BOOL bAllModel,BOOL bComputeDirectLight,BOOL bComputeIndirectLight)
{
	if(!m_lpLightMapBaker)
		return E_FAIL;

	if(bAllModel)
	{
		m_lpLightMapBaker->PrepareModelForBake(NULL,bComputeDirectLight,bComputeIndirectLight);
	}
	else
	{
		IEKG3DRepresentObject* pObj = GetFirstSelectedObj();

		if (!pObj)
			return E_FAIL;

		IKG3DModel* pModel = NULL;

		pObj->GetModel(pModel, 0);

		if (!pModel)
			return E_FAIL;

		switch (pModel->GetType())
		{
		case MESHTYPE_DEFAULT:
			{
				KG3DModel* pModelR = dynamic_cast<KG3DModel*>(pModel);
				m_lpLightMapBaker->PrepareModelForBake(pModelR,bComputeDirectLight,bComputeIndirectLight);
				break;
			}
		case MESHTYPE_POINTLIGHT :
			{
				break;
			}
		default :
			break;
		}
	}

	return S_OK;
}

HRESULT KG3DScenePvsEditor::ComputePhotons()
{
	HRESULT hr = E_FAIL;

	if(!m_lpLightMapBaker)
	{
		m_lpLightMapBaker = new KG3DLightMapBaker;
	}
	KGLOG_PROCESS_ERROR(m_lpLightMapBaker);

	hr = m_lpLightMapBaker->InitFromPVS(this);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	return hr;
}

extern BOOL g_bShowPointLightSign;
BOOL KG3DScenePvsEditor::GetEnableShowLightSign()
{
	return g_bShowPointLightSign;
}
void KG3DScenePvsEditor::SetEnableShowLightSign(BOOL bEnable)
{
	g_bShowPointLightSign = bEnable;
}

extern BOOL g_bShowLightMapAsChessboard;
BOOL KG3DScenePvsEditor::GetEnableShowChessBoradLightmap()
{
	return g_bShowLightMapAsChessboard;
}

void KG3DScenePvsEditor::SetEnableShowChessBoradLightmap(BOOL bEnable)
{
	g_bShowLightMapAsChessboard = bEnable;
}