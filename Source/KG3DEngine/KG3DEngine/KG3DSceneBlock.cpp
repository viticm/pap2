#include "StdAfx.h"
#include "KG3DSceneBlock.h"
#include "KG3DFrustum.h"
#include "KG3DTemplates.h"
#include "KG3DTerrainBlock.h"
#include "KG3DRepresentObject.h"
#include "KG3DSceneDataInterface.h"
#include "KG3DRepresentObjectTable.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DRepresentObjectPVS.h"
#include "KG3DScene.h"
#include "KG3DModelPointLight.h"
#include "KG3DResourceDelayReleaseMgr.h"

#include "KG3DTerrainRender.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

TKG3DResourcePool<KG3DSceneZone>g_cSceneZonePool;
TKG3DResourcePool<KG3DSceneSection>g_cSceneSectionPool;
TKG3DResourcePool<KG3DSceneRegion>g_cSceneRegionPool;
TKG3DResourcePool<KG3D_PointLight_Info>g_cPointLightInfoPool;
extern TKG3DResourcePool<KG3DTerrainBlock>g_cTerrainBlockPool;
extern DWORD g_dwRenderCount;
extern DWORD g_dwCullCounter;

TKG3DResourcePool<KG3DEntityObjectShell>g_cObjectShellPool;


extern INT GetSceneBlockIndex( FLOAT In, FLOAT BlockWidth )
{
	if(In<0)
	{
		FLOAT InRevert = -In;
		InRevert /= BlockWidth;
		FLOAT fFrac = 0;
		FLOAT fIn;
		fFrac = modf(InRevert, &fIn);
		INT nRet = MT_FLOAT_TRIM_TAIL(fIn);
		if (fFrac > 0)
		{
			return -nRet - 1;
		}
		return -nRet;
	}
	FLOAT fRet = In/BlockWidth;
	return MT_FLOAT_TRIM_TAIL(fRet);
	
}


//////////////////////////////////////////////////////////////////////////
HRESULT KG3DEntityObjectShell::Init()
{
	return S_OK;
}

HRESULT KG3DEntityObjectShell::UnInit()
{
    /*
    if (m_lpSpaceMgr)
        m_lpSpaceMgr->CheckReleaseFromPvs(m_lpObject);
        */
	RemoveFromAllRefrenceContainer();

	SAFE_RELEASE(m_lpObject);

	m_uHandle = 0;
	m_dwVisibleCount = 0;
	m_vPosition = D3DXVECTOR3(0,0,0);
	m_vBBox_B = D3DXVECTOR3(0,0,0);
	m_vBBox_A = D3DXVECTOR3(0,0,0);
	m_fDiameter = 0;

	m_lpSpaceMgr = NULL;
	m_bLoaded = FALSE;

	return S_OK;
}

/*
HRESULT KG3DEntityObjectShell::RemoveFromPvs()
{
    if (m_lpObject)
    {
        if (m_lpObject->GetType() == REPRESENTOBJECT_PVS)
        {
            KG3DRepresentObjectPVS* pvs = (KG3DRepresentObjectPVS*)m_lpObject;

            if (pvs->m_lpSpaceNodeParent)
                KG3DSceneSpaceNode::DisConnecteTwoNode(pvs->m_lpSpaceNodeParent, pvs);

            if (m_lpSpaceMgr && m_lpSpaceMgr->GetScene())
                m_lpSpaceMgr->GetScene()->UnRegisterSpaceNode(pvs);
        }

        m_lpObject->UpdateByMatrix(m_lpObject->GetMatrixWorld());
        m_lpObject->SetParentObject(NULL);
        m_lpObject->SetParentMatrix(NULL);
    }

    if (m_lpSpaceMgr)
        m_lpSpaceMgr->SetEntityParent(m_uHandle, INVALID_OBJSHELL_HANDLE);

    

    return S_OK;
}*/

HRESULT KG3DEntityObjectShell::RemoveFromAllRefrenceContainer()
{
	set<IKG3DSceneEntityContainer*>::iterator i = m_setRefrenceContainer.begin();
	set<IKG3DSceneEntityContainer*>::iterator iend = m_setRefrenceContainer.end();
	while (i!=iend)
	{
		IKG3DSceneEntityContainer* pCon = *i;
		pCon->RemoveEntityObjectShell(this);
		++i;
	}
	m_setRefrenceContainer.clear();

	return S_OK;
}

KG3DEntityObjectShell::KG3DEntityObjectShell()
{
	m_lpObject = NULL;

	m_vPosition = D3DXVECTOR3(0,0,0);
	m_vBBox_B = D3DXVECTOR3(0,0,0);
	m_vBBox_A = D3DXVECTOR3(0,0,0);
	m_fDiameter = 0;

	m_lpSpaceMgr = NULL;

	m_uHandle = 0;
	m_dwVisibleCount = 0;
	m_bLoaded = FALSE;
}


KG3DEntityObjectShell::~KG3DEntityObjectShell()
{
	UnInit();
}

HRESULT KG3DEntityObjectShell::TryDynamicLoad(DWORD dwOption)
{
	SetVisibleCount(g_dwRenderCount);
	if (!m_bLoaded)
	{
		if(m_lpSpaceMgr)
		{
			m_lpSpaceMgr->LoadOneObjectShell(this, dwOption);
            m_lpSpaceMgr->CheckConnectNode(this);
		}
		if(m_lpObject)
		{
			m_lpObject->m_BBox.A = m_vBBox_A;
			m_lpObject->m_BBox.B = m_vBBox_B;
		}
		m_bLoaded = TRUE;
	}
	return S_OK;
}

HRESULT KG3DEntityObjectShell::TryDynamicUnLoad()
{
	SAFE_RELEASE(m_lpObject);
	m_bLoaded = FALSE;
	return S_OK;
}

KG3DRepresentObjectPVS* KG3DEntityObjectShell::GetPvs()
{
	if(m_bLoaded && m_lpObject)
	{
		DWORD dwType = m_lpObject->GetType();
		if(dwType == REPRESENTOBJECT_PVS)
		{
			return dynamic_cast<KG3DRepresentObjectPVS*>(m_lpObject);
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DSceneEntityContainer::RemoveRepresentObject(KG3DRepresentObject* pObject)
{
	setObject.erase(pObject);
	return S_OK;
}


HRESULT KG3DSceneEntityContainerData::RemoveEntityObjectShell(KG3DEntityObjectShell* pShell)
{

	m_Lock.Lock();

	std::set<KG3DEntityObjectShell*>::iterator iFind = setObjectShell.find(pShell);
	if (iFind!=setObjectShell.end())
	{
		setObjectShell.erase(iFind);
	}

	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneEntityContainer::RemoveEntityObjectShell(KG3DEntityObjectShell* pShell)
{
	return m_Data.RemoveEntityObjectShell(pShell);
}

HRESULT KG3DSceneEntityContainerData::UnInit()
{
	m_Lock.Lock();

	setObjectShell.clear();

	m_setUnLoadObjectHandle.clear();
	{
		set<KG3DModel*>::iterator i = setModel.begin();
		while (i!=setModel.end())
		{
			KG3DModel* pModel = *i;
			pModel->m_lpSceneRegionOn = NULL;
			i++;
		}
		setModel.clear();
	}
	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneEntityContainer::UnInit()
{
	{
		std::set<KG3DRepresentObject*>::iterator i = setObject.begin();
		while (i != setObject.end())
		{
			(*i)->SetPhysicsScene(NULL);
			++i;
		}
	}
	setObject.clear();

	m_Data.UnInit();

	m_dwTerrainLoadState = 0;
	m_dwObjectLoadState = 0;
    m_dwLastCullCounter = 0;
	//m_dwLastUseTime = 0;
	return S_OK;
}


HRESULT KG3DSceneEntityContainerData::GetObjectShellIntersectPoint2D(list<KG3DEntityObjectShell*>*plistShell,D3DXVECTOR2& vPos)
{
	if(!setObjectShell.size())
		return S_OK;

	m_Lock.Lock();

	std::set<KG3DEntityObjectShell*>::iterator i = setObjectShell.begin();
	std::set<KG3DEntityObjectShell*>::iterator iend = setObjectShell.end();
	while (i!=iend)
	{
		KG3DEntityObjectShell* pShell = *i;
		
		if ((pShell->m_vBBox_A.x <= vPos.x)&&(pShell->m_vBBox_A.z <= vPos.y)&&
			(pShell->m_vBBox_B.x >= vPos.x)&&(pShell->m_vBBox_B.z >= vPos.y))
		{
			plistShell->push_back(pShell);
		}
		++i;
	}

	m_Lock.Unlock();

	return S_OK;
}

/************************************************************************/
/*
用函数内的类继承某种Visitor来访问内部的元素，只需要Accept方法，如果Accept返回
错误，会终止Traverse。

例子：
HRESULT GetXX(std::set<KG3DRepresentObject*>& set)
{
	struct Visitor : public KG3DRepresentObjectVisitor
	{
		DWORD m_dwType;
		std::set<KG3DRepresentObject*>*	m_pSet;
		vitual HRESULT Accept(KG3DRepresentObject& Obj)
		{
			if(Obj.GetType() == m_dwType)
			{
				m_pSet->insert(&Obj);
			}
		}
	}
	Visitor visitor;
	visitor.m_dwType = REPRESENTOBJECT_DEFAULT;
	visitor.m_pSet = &set;

	return XXX.Traverse(visitor);
}
*/
/************************************************************************/
HRESULT KG3DSceneEntityContainerData::Traverse( KG3DRepresentObjectVisitor& Visitor )
{
	HRESULT hr = E_FAIL;
	m_Lock.Lock();

	std::set<KG3DEntityObjectShell*>::iterator t = setObjectShell.begin();
	std::set<KG3DEntityObjectShell*>::iterator tEnd = setObjectShell.end();

	for(; t != tEnd; ++t)
	{
		KG3DEntityObjectShell* p = *t;
		if (p->m_lpObject)
		{
			hr = Visitor.Accept(*(p->m_lpObject));
			KG_COM_PROCESS_ERROR(hr);
		}

	}

	hr = S_OK;
Exit0:
	m_Lock.Unlock();
	return hr;
	
}

HRESULT KG3DSceneEntityContainer::Traverse( KG3DRepresentObjectVisitor& Visitor )
{
	HRESULT hr = E_FAIL;

	hr = m_Data.Traverse(Visitor);
	KG_COM_PROCESS_ERROR(hr);

	{
		std::set<KG3DRepresentObject*>::iterator t = setObject.begin();
		std::set<KG3DRepresentObject*>::iterator tEnd = setObject.end();

		for(; t != tEnd; ++t)
		{
			KG3DRepresentObject* p = *t;
			hr = Visitor.Accept(*p);
			KG_COM_PROCESS_ERROR(hr);
		}
	}

Exit0:	
	return S_OK;
}

HRESULT KG3DSceneEntityContainer::Traverse( KG3DEntityObjectShellVisitor& visitorIn )
{
	return m_Data.Traverse(visitorIn);
}

HRESULT KG3DSceneEntityContainerData::Traverse( KG3DEntityObjectShellVisitor& visitorIn )
{
	HRESULT hr = E_FAIL;
	m_Lock.Lock();

	{
		std::set<KG3DEntityObjectShell*>::iterator t = setObjectShell.begin();
		std::set<KG3DEntityObjectShell*>::iterator tEnd = setObjectShell.end();

		for(; t != tEnd; ++t)
		{
			KG3DEntityObjectShell* p = *t;
			hr = visitorIn.Accept(*p);
			KG_COM_PROCESS_ERROR(hr);
		}
	}
Exit0:	
	m_Lock.Unlock();
	return S_OK;
}
HRESULT KG3DSceneEntityContainer::TraverseOldFormat( KG3DRepresentObjectVisitor& Visitor )
{
	HRESULT hr = E_FAIL;
	{
		std::set<KG3DRepresentObject*>::iterator t = setObject.begin();
		std::set<KG3DRepresentObject*>::iterator tEnd = setObject.end();

		for(; t != tEnd; ++t)
		{
			KG3DRepresentObject* p = *t;
			hr = Visitor.Accept(*p);
			KG_COM_PROCESS_ERROR(hr);
		}
	}

Exit0:
	return S_OK;
}

VOID KG3DSceneEntityContainerData::Clear(KG3DSceneEntityContainer* pCon)
{
	m_Lock.Lock();
	//Model里面只记录了一个SceneRegion，所以可以直接检查，不过因为Model里面记录的那个是void*，要进行强转可行性检查
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DSceneEntityContainer, KG3DSceneRegion);
	for (std::set<KG3DModel*>::iterator it = setModel.begin()
		; it != setModel.end()
		; ++it)
	{
		KG3DModel* pModel = *it;
		if (pModel->m_lpSceneRegionOn == (void*)this)
		{
			pModel->LeaveSceneRegion(FALSE);
		}
	}
	setModel.clear();


	for (std::set<KG3DEntityObjectShell*>::iterator it = setObjectShell.begin()
		; it != setObjectShell.end()
		; ++it)
	{
		KG3DEntityObjectShell* p = *it;
		p->m_setRefrenceContainer.erase(pCon);
	}
	setObjectShell.clear();
	m_setUnLoadObjectHandle.clear();

	m_Lock.Unlock();
}

VOID KG3DSceneEntityContainer::Clear()
{
	for (std::set<KG3DRepresentObject*>::iterator it = setObject.begin()
		; it != setObject.end()
		; ++it)
	{
		KG3DRepresentObject* p = *it;
		p->OnRemoveFromContainer(this);
	}
	setObject.clear();
	
	m_Data.Clear(this);
}

HRESULT KG3DSceneEntityContainer::Add( KG3DRepresentObject* pObject )
{
	return KG3DSceneEntityContainer::InputRepresentObject(pObject);	//注意不是虚函数且强制其非虚调用
}
HRESULT KG3DSceneEntityContainer::InputRepresentObject( KG3DRepresentObject* pObject )
{
	KG_PROCESS_ERROR(NULL != pObject);

	{
		std::set<KG3DRepresentObject*>::_Pairib Pib = setObject.insert(pObject);
		KG_PROCESS_ERROR(Pib.second);//已经存在了不能添加
		
		HRESULT hr = pObject->OneInputContainer(this);
		if (FAILED(hr))
		{
			setObject.erase(Pib.first);
			goto Exit0;
		}
		m_BBox.AddY(pObject->m_BBox);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneEntityContainerData::Add( KG3DModel* pModel,KG3DSceneEntityContainer* pCon )
{
	m_Lock.Lock();
	{
		std::set<KG3DModel*>::_Pairib Pib = setModel.insert(pModel);
		KG_PROCESS_ERROR(Pib.second);//已经存在了不能添加

		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DSceneEntityContainer, KG3DSceneRegion);//验证强转的可行性

		HRESULT hr = pModel->OnEnterSceneRegion(*static_cast<KG3DSceneRegion*>(pCon));
		if (FAILED(hr))
		{
			setModel.erase(Pib.first);
			goto Exit0;
		}

	}
Exit0:
	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneEntityContainer::Add( KG3DModel* pModel )
{
	KG_PROCESS_ERROR(this->GetType() == KG3DTYPE_SCENE_REGION);//只有Region能AddModel，暂时是这么设计的
	KG_PROCESS_ERROR(NULL != pModel);

	m_Data.Add(pModel,this);

	if (pModel->m_Translation.y >= m_BBox.B.y)
		m_BBox.B.y = pModel->m_Translation.y + 1000;//原来就是这样写的

	return S_OK;
Exit0:
	return E_FAIL;
}

//HRESULT KG3DSceneEntityContainerData::AddPVS(KG3DRepresentObjectPVS* pPVS)
//{
//	m_Lock.Lock();
//
//	m_setPvs.insert(pPVS);
//
//	m_Lock.Unlock();
//	return S_OK;
//}
//
//HRESULT KG3DSceneEntityContainerData::RemovePVS(KG3DRepresentObjectPVS* pPVS)
//{
//	m_Lock.Lock();
//
//	m_setPvs.erase(pPVS);
//
//	m_Lock.Unlock();
//	return S_OK;
//}

HRESULT KG3DSceneEntityContainer::AddPVS(KG3DRepresentObjectPVS* pPVS)
{
	return S_OK;//m_Data.AddPVS(pPVS);
}

HRESULT KG3DSceneEntityContainer::RemovePVS(KG3DRepresentObjectPVS* pPVS)
{
	return S_OK;//m_Data.RemovePVS(pPVS);
}

KG3DSceneEntityContainerData::KG3DSceneEntityContainerData()
{
	m_eFrustumIntersectType = KG3DFrustum::OUTSIDE;
}

KG3DSceneEntityContainerData::~KG3DSceneEntityContainerData()
{
}

KG3DSceneEntityContainer::KG3DSceneEntityContainer()
{
    m_lpTerrainBlock = NULL;
    m_dwVisibleCount = 0;
	m_BBox.Clear();
    m_dwTerrainLoadState = BLOCK_LOAD_NONE;
	m_dwObjectLoadState  = BLOCK_LOAD_NONE;
	m_fSortValue = 0;
    m_dwLastCullCounter = 0;
}

void KG3DSceneEntityContainerData::ReInputObject(BOOL bClient)
{
	m_Lock.Lock();

	set<KG3DEntityObjectShell*> setObjectSellTemp = setObjectShell;

	for (set<KG3DEntityObjectShell*>::iterator i = setObjectSellTemp.begin();
		i != setObjectSellTemp.end(); ++i)
	{
		KG3DEntityObjectShell* shell = (*i);

		if (!shell)
			continue;

		KG3DRepresentObject* obj = shell->m_lpObject;

		if (!obj)
			continue;

		KG3DSceneOutDoorSpaceMgr* pMgr = shell->m_lpSpaceMgr;

		if (!pMgr)
			continue;

		DWORD dwType = obj->GetType();

		if (dwType == REPRESENTOBJECT_PVS)
			continue;

		if (bClient)
		{
			if (dwType != REPRESENTOBJECT_NPC && dwType != REPRESENTOBJECT_DOODAD)
				continue;
		}

		obj->AddRef();
		pMgr->RemoveRepresentObject(obj);
		pMgr->InputRepresentObject(obj, FALSE, FALSE, NULL);
	}   

	m_Lock.Unlock();
}

void KG3DSceneEntityContainer::ReInputObject(BOOL bClient)
{
	m_Data.ReInputObject(bClient);
}

BOOL KG3DSceneEntityContainerData::InputObjectShellToPvs(KG3DEntityObjectShell* objShell)
{
	bool bReturn = false;
	m_Lock.Lock();

	for (std::set<KG3DEntityObjectShell*>::iterator i = setObjectShell.begin();
		i != setObjectShell.end(); ++i)
	{
		KG3DEntityObjectShell* pShell = *i;
	
		KG3DRepresentObjectPVS* pPVS = pShell->GetPvs();

		if(pPVS)
		{
			D3DXVECTOR3 center = 0.5F * (objShell->m_vBBox_A + objShell->m_vBBox_B);

			if (pPVS->IsPositionInSpace(center))
			{
				pPVS->InputObjectShell(objShell,TRUE);

				bReturn = true;
				goto Exit0;
			}
		}
	}
Exit0:
	m_Lock.Unlock();
	return bReturn;
}

bool KG3DSceneEntityContainerData::InputObjectToPvs(KG3DEntityObjectShell* objShell)
{
	bool bReturn = false;
	m_Lock.Lock();

	for (std::set<KG3DEntityObjectShell*>::iterator i = setObjectShell.begin();
		i != setObjectShell.end(); ++i)
	{
		KG3DRepresentObject* obj = (*i)->m_lpObject;

		if (!obj || obj->GetType() != REPRESENTOBJECT_PVS)
			continue;

		KG3DRepresentObjectPVS* pvs = (KG3DRepresentObjectPVS*)obj;

		D3DXVECTOR3 center = objShell->m_lpObject->m_BBox.GetCenter();

		if (pvs->IsPositionInSpace(center))
		{
			pvs->InputObject(objShell->m_lpObject, &objShell->m_lpSpaceMgr->GetScene());

			objShell->m_lpObject->m_bSleep = false;
			objShell->m_lpObject->FrameMove();

			if (objShell->m_lpSpaceMgr)
			{
				/*
				UINT uParent = objShell->m_lpObject->GetParentObject()->GetHandleHeldBySpaceMgr();
				objShell->m_lpSpaceMgr->SetEntityParent(objShell->m_uHandle, uParent);
				*/

				if (objShell->m_lpSpaceMgr->GetSceneDataInterface())
					objShell->m_lpSpaceMgr->GetSceneDataInterface()->UpdateObjTransToData(objShell->m_lpObject);
			}

			bReturn = true;
			goto Exit0;
		}

	}
Exit0:
	m_Lock.Unlock();
	return bReturn;
}

bool KG3DSceneEntityContainer::InputObjectToPvs(KG3DEntityObjectShell* objShell)
{
    if (!objShell)
        return false;

	return m_Data.InputObjectShellToPvs(objShell);
}

//bool KG3DSceneEntityContainer::CheckConnectNode(KG3DEntityObjectShell* objShell)
//{
//    if (!objShell || !objShell->m_lpObject)
//        return false;
//
//    objShell->m_lpSpaceMgr->SetEntityParent(objShell->m_uHandle, INVALID_OBJSHELL_HANDLE);
//
//    if (objShell->m_lpObject->GetType() != REPRESENTOBJECT_PVS)
//        return false;
//
//    KG3DSceneOutDoorSpaceMgr* pSceneMgr = objShell->m_lpSpaceMgr;
//
//    if (!pSceneMgr)
//        return false;
//
//    KG3DScene* pScene = &pSceneMgr->GetScene();
//
//    if (!pScene)
//        return false;
//
//    KG3DRepresentObjectPVS* pvs = (KG3DRepresentObjectPVS*)objShell->m_lpObject;
//    pvs->m_uHandle = pScene->GetOneNewSpaceNodeHandle();
//    KG3DSceneSpaceNode::ConnecteTwoNode(pSceneMgr, pvs);
//    pScene->RegisterSpaceNode(pvs);
//
//    return true;
//}

KG3DTerrainBlock* KG3DSceneEntityContainer::GetTerrainBlock()
{
	if(GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH)
		return m_lpTerrainBlock;
	return NULL;
}

KG3DSceneEntityContainer::TypeAccessProxy KG3DSceneEntityContainer::GetSetObjects()
{
	KGCH::TFrontAccessProxy<KG3DRepresentObject*>* p = new KGCH::TFrontAccessProxySolid<std::set<KG3DRepresentObject*> >(setObject);
	return auto_ptr<KGCH::TFrontAccessProxy<KG3DRepresentObject*> >(p);
}


//////////////////////////////////////////////////////////////////////////
KG3DSceneRegion::KG3DSceneRegion()
{
	m_lpTerrainBlock = NULL;

	_ASSERTE(this->GetType() == KG3DTYPE_SCENE_REGION);
}

KG3DSceneRegion::~KG3DSceneRegion()
{
    UnInit();
}

const static DWORD CELLWIDTH = 100;

DWORD KG3DSceneRegion::GetTerrainType(const D3DXVECTOR2& vec2Pos)
{
	DWORD dwReturn = 0xffffffff;
	BYTE byteDetailTexturIndex = 0xff; 
	
	if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
	{
		int nIndexX = static_cast<int>(vec2Pos.x - m_nIndexX * SCENEREGIONWIDTH);
		int nIndexZ = static_cast<int>(vec2Pos.y - m_nIndexZ * SCENEREGIONWIDTH);
		int nX = static_cast<int>(nIndexX / CELLWIDTH);
		int nZ = static_cast<int>(nIndexZ / CELLWIDTH);
		int nCellIndex = nZ * (BLOCK_CELL_COUNT + 1) + nX;

		KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
		if(pTerrain)
		{
			pTerrain->AddRef();
	
			byteDetailTexturIndex = static_cast<BYTE>(m_lpTerrainBlock->m_lpBlendFactor->vBlend[nCellIndex]);//m_lpTerrainBlock->m_byteTextureIndex[nCellIndex];

			pTerrain->Release();
		}
	}
	KG_PROCESS_ERROR(byteDetailTexturIndex < TERRAIN_BLOCK_MAX_DETAILMAP);

	dwReturn = static_cast<DWORD>(m_lpTerrainBlock->m_byteDetailTextureIndex[byteDetailTexturIndex]);
Exit0:
	return dwReturn;
}


HRESULT KG3DSceneRegion::Init(int nX,int nZ)
{
	m_nIndexX = nX;
	m_nIndexZ = nZ;

	D3DXVECTOR3 A((float)m_nIndexX * SCENEREGIONWIDTH,0.0F,(float)m_nIndexZ*SCENEREGIONWIDTH);
	D3DXVECTOR3 C = A + D3DXVECTOR3(SCENEREGIONWIDTH,0.0F,SCENEREGIONWIDTH);

	m_BBox.Clear();
	m_BBox.AddPosition(A);
	m_BBox.AddPosition(C);

	/*setObject.clear();
	setModel.clear();*/
	//_ASSERTE(GetSetObjects()->IsEmpty() && setModel.empty() && _T("如果它们非空，则是重复的初始化"));

	SetObjectLoadState(BLOCK_LOAD_NODATA);
	SetTerrainLoadState(BLOCK_LOAD_NODATA);
	dwViewClass = 0;
	return S_OK;
}

HRESULT KG3DSceneRegion::UnInit()
{
	//SAFE_RELEASE(m_lpTerrainBlock);
	if (m_lpTerrainBlock)
	{
	    KG3DResourceDelayReleaseMgr::Push(m_lpTerrainBlock);
    	m_lpTerrainBlock = NULL;
	}
	KG3DSceneEntityContainer::UnInit();

	dwViewClass = 0;
	return S_OK;
}

HRESULT KG3DSceneRegion::CreateTerrainBlock()
{
	HRESULT hr = S_OK;

	KG_PROCESS_SUCCESS(m_lpTerrainBlock);

	m_lpTerrainBlock = g_cTerrainBlockPool.RequestResource();
	KGLOG_PROCESS_ERROR(m_lpTerrainBlock);
	m_lpTerrainBlock->AddRef();

	hr = m_lpTerrainBlock->Init(BLOCK_TYPE_REGION,m_nIndexX,m_nIndexZ);
	KGLOG_COM_PROCESS_ERROR(hr);

    m_lpTerrainBlock->m_Container = this;
Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneRegion::DeleteTerrainBlock()
{
	//SAFE_RELEASE(m_lpTerrainBlock);
	if (m_lpTerrainBlock)
	{
	    KG3DResourceDelayReleaseMgr::Push(m_lpTerrainBlock);
    	m_lpTerrainBlock = NULL;
	}
	return S_OK;
}

HRESULT KG3DSceneEntityContainerData::ComputeBBox(float x,float z,AABBOX& bbox,BOOL bClear)
{
	m_Lock.Lock();

	set<KG3DEntityObjectShell*>::iterator i = setObjectShell.begin();
	set<KG3DEntityObjectShell*>::iterator iend = setObjectShell.end();
	while (i!=iend)
	{
		KG3DEntityObjectShell* pShell = *i;

		D3DXVECTOR3 A(x, pShell->m_vBBox_A.y, z);
		D3DXVECTOR3 B(x, pShell->m_vBBox_B.y, z);

		bbox.AddPosition(A);
		bbox.AddPosition(B);

		++i;
	}
	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneRegion::ComputeBBox(BOOL bClear)
{
	if(bClear)
	{
		m_BBox.Clear();
		float x = (float)m_nIndexX * SCENEREGIONWIDTH;
		float z = (float)m_nIndexZ * SCENEREGIONWIDTH;

		if (m_lpTerrainBlock)
		{

			D3DXVECTOR3 A(x                  , m_lpTerrainBlock->m_fHeightMin, z);
			D3DXVECTOR3 B(x+SCENEREGIONWIDTH, m_lpTerrainBlock->m_fHeightMax, z+SCENEREGIONWIDTH);
			m_BBox.AddPosition(A);
			m_BBox.AddPosition(B);
		}
		else
		{
			D3DXVECTOR3 A(x                  , 0 , z);
			D3DXVECTOR3 B(x+SCENEREGIONWIDTH , 10, z+SCENEREGIONWIDTH);
			m_BBox.AddPosition(A);
			m_BBox.AddPosition(B);
		}

		m_Data.ComputeBBox(x,z,m_BBox,bClear);
		
	}
	//m_BBox.Extract(m_BoxPos);

	return S_OK;
}

float KG3DSceneRegion::GetGroundHeight(float fX,float fZ)
{
	float Height = 0;
	if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
	{
		KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
		if(pTerrain)
		{
			pTerrain->AddRef();
			Height = pTerrain->GetGroundHeight(fX,fZ);
			pTerrain->Release();
		}
	}
	return Height;
}

BYTE KG3DSceneRegion::GetDetailTextureIndex(float fX,float fZ)
{
	BYTE Index = 0;

	if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
	{
		KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
		if(pTerrain)
		{
			pTerrain->AddRef();
			Index = pTerrain->GetDetailTextureIndex(fX,fZ);
			pTerrain->Release();
		}
	}
	
	return Index;
}

HRESULT KG3DSceneRegion::GetWaterHeight(float& fHeight,float fX,float fZ)
{
	HRESULT hr = E_FAIL;
	if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
	{
		KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
		if(pTerrain)
		{
			pTerrain->AddRef();
			hr = pTerrain->GetWaterHeight(fHeight,fX,fZ);
			pTerrain->Release();
		}
	}
	
	return hr;
}

HRESULT KG3DSceneRegion::WedTerrainNormal(KG3DSceneRegion* pLeft,KG3DSceneRegion* pRight,KG3DSceneRegion* pUp,KG3DSceneRegion* pDown)
{
	KG3DTerrainBlock* pBlockLeft  = NULL;
	KG3DTerrainBlock* pBlockRight = NULL;
	KG3DTerrainBlock* pBlockUp    = NULL;
	KG3DTerrainBlock* pBlockDown  = NULL;

	if (pLeft)
	{
		pBlockLeft = pLeft->m_lpTerrainBlock;
	}
	if(pRight)
	{
		pBlockRight = pRight->m_lpTerrainBlock;
	}
	if (pUp)
	{
		pBlockUp = pUp->m_lpTerrainBlock;
	}
	if(pDown)
	{
		pBlockDown = pDown->m_lpTerrainBlock;
	}

	if(m_lpTerrainBlock)
	{
		return m_lpTerrainBlock->WedNormal(pBlockLeft,pBlockRight,pBlockUp,pBlockDown);
	}
	return S_OK;
}


void KG3DSceneRegion::OnInputObject(KG3DEntityObjectShell* objShell)
{
    extern BOOL g_bClient;

    if (!g_bClient)
    {
        if (!objShell || !objShell->m_lpObject || objShell->m_lpObject->GetType() != REPRESENTOBJECT_PVS || !objShell->m_lpSpaceMgr)
            return;

        ReInputObject(TRUE);  
    }
}

HRESULT KG3DSceneEntityContainerData::InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs,KG3DSceneEntityContainer* pCon )
{
	m_Lock.Lock();

	setObjectShell.insert(pShell);
	pShell->m_setRefrenceContainer.insert(pCon);
	/*KG3DRepresentObjectPVS* pPVS = pShell->GetPvs();
	if(pPVS)
	{
		ASSERT(0);
	}*/

	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneRegion::InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs)
{
    /* try first input to pvs */ 
    if (bCheck2Pvs && InputObjectToPvs(pShell))
        return S_OK;

    if (!pShell)
        return E_FAIL;

	m_Data.InputObjectShell(pShell,bCheck2Pvs,this);

	float x = m_BBox.A.x;
	float z = m_BBox.A.z;
	m_BBox.AddPosition(D3DXVECTOR3(x, pShell->m_vBBox_A.y, z));
	m_BBox.AddPosition(D3DXVECTOR3(x, pShell->m_vBBox_B.y, z));
    ComputeBBox(FALSE);

    /* if the objshell is a pvs object
    then link to the outdoor mgr */
    if (pShell->m_lpSpaceMgr)
        pShell->m_lpSpaceMgr->CheckConnectNode(pShell);

    OnInputObject(pShell);

	return S_OK;
}

HRESULT KG3DSceneRegion::InputRepresentObject(KG3DRepresentObject* pObject)
{
	HRESULT hr = KG3DSceneEntityContainer::InputRepresentObject(pObject);
	if (SUCCEEDED(hr))
	{
		ComputeBBox(FALSE);
	}
	return hr;
}

ULONG KG3DSceneRegion::GetType()const
{
	return KG3DTYPE_SCENE_REGION;
}

HRESULT KG3DSceneRegion::CheckLodLevel()
{
	if(!m_lpTerrainBlock)
		return E_FAIL;
	if(m_lpTerrainBlock->m_fViewDistance < g_cEngineOption.nTerrainLODGates[0]/*12800*/)
		m_lpTerrainBlock->m_nLodLevel = 0;
	else if(m_lpTerrainBlock->m_fViewDistance < g_cEngineOption.nTerrainLODGates[1]/*24000*/)
		m_lpTerrainBlock->m_nLodLevel = 1;
	else if(m_lpTerrainBlock->m_fViewDistance < g_cEngineOption.nTerrainLODGates[2]/*24800*/)
		m_lpTerrainBlock->m_nLodLevel = 2;
	else
		m_lpTerrainBlock->m_nLodLevel = 3;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////

HRESULT KG3DSceneSection::CheckLodLevel()
{
	if(!m_lpTerrainBlock)
		return E_FAIL;
	if(m_lpTerrainBlock->m_fViewDistance < 44800)
		m_lpTerrainBlock->m_nLodLevel = 0;
	else if(m_lpTerrainBlock->m_fViewDistance < 57600)
		m_lpTerrainBlock->m_nLodLevel = 1;
	else if(m_lpTerrainBlock->m_fViewDistance < 70400)
		m_lpTerrainBlock->m_nLodLevel = 2;
	else
		m_lpTerrainBlock->m_nLodLevel = 3;
	return S_OK;
}
HRESULT KG3DSceneSection::CheckRegionDetial()
{
	BOOL bRegion = FALSE;
	for (int nZ=0;nZ<SCENESECTIONNUMREGION;nZ++)
	{
		for (int nX=0;nX<SCENESECTIONNUMREGION;nX++)
		{
			int nIndex = nZ * SCENESECTIONNUMREGION + nX;
			if(m_lpRegions[nIndex])
			{
				bRegion = TRUE;
				KG_PROCESS_SUCCESS(bRegion);
			}
		}
	}
Exit1:
	m_bRegionDetail = bRegion;
	return S_OK;
}

HRESULT KG3DSceneSection::CheckMemoryUseMultiThread(DWORD dwTime,BOOL* pbStop)
{
	for (int nZ=0;nZ<SCENESECTIONNUMREGION;nZ++)
	{
		for (int nX=0;nX<SCENESECTIONNUMREGION;nX++)
		{
			int nIndex = nZ * SCENESECTIONNUMREGION + nX;
			KG_PROCESS_SUCCESS(*pbStop);

			KG3DSceneRegion* pRegion = m_lpRegions[nIndex];
			if(pRegion && pRegion->GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH)
			{
				if(pRegion->m_lpTerrainBlock && pRegion->m_lpTerrainBlock->IsLoaded() && (g_dwRenderCount - pRegion->GetVisibleCount() > dwTime))
				{
					pRegion->SetTerrainLoadState(BLOCK_LOAD_NODATA);
					//SAFE_RELEASE(pRegion->m_lpTerrainBlock);
					if (pRegion->m_lpTerrainBlock)
					{
					    KG3DResourceDelayReleaseMgr::Push(pRegion->m_lpTerrainBlock);
				    	pRegion->m_lpTerrainBlock = NULL;
					}
				}
			}
		}
	}

Exit1:
	CheckRegionDetial();
	CheckRegionLoad();
	return S_OK;
}


HRESULT KG3DSceneSection::CheckRegionLoad()
{
	BOOL bRegion = TRUE;
	if(!m_bRegionDetail)	
		bRegion = FALSE;
	KG_PROCESS_ERROR(bRegion);
		
	for (int nZ=0;nZ<SCENESECTIONNUMREGION;nZ++)
	{
		for (int nX=0;nX<SCENESECTIONNUMREGION;nX++)
		{
			int nIndex = nZ * SCENESECTIONNUMREGION + nX;
			KG3DSceneRegion* pRegion = m_lpRegions[nIndex];

			if(pRegion)
			{
				if(pRegion->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
				{
					bRegion = FALSE;
					KG_PROCESS_ERROR(bRegion);
				}
				
			}
		}
	}

Exit0:
	m_bAllRegionReady = bRegion;
	return S_OK;
}

static float s_fRegionLength = 1600.0f;
HRESULT KG3DSceneSection::GetVisiableSceneRegions(
    vector<KG3DSceneEntityContainer *> *pvecContainer,
    KG3DTerrainRender *pTerrainRender,
    const KG3DFrustum *pFrustum,
    const D3DXVECTOR3 &vPosCamera,
    int nLevelDeep
)
{
	if(!m_bRegionDetail)
		return S_OK;

	BOOL PutRegionTerrain = TRUE;
    int nCameraRegionIndexX = (int)(vPosCamera.x / s_fRegionLength);
    int nCameraRegionIndexZ = (int)(vPosCamera.z / s_fRegionLength);

    if ((!m_bAllRegionReady || nLevelDeep <= 2) && m_lpTerrainBlock)
    {
        pTerrainRender->TryInputTerrainBlock(m_lpTerrainBlock);
    }

    PutRegionTerrain = (nLevelDeep > 2);

	for (int nZ=0;nZ<SCENESECTIONNUMREGION;nZ++)
	{
		for (int nX=0;nX<SCENESECTIONNUMREGION;nX++)
		{
			int nIndex = nZ * SCENESECTIONNUMREGION + nX;
			KG3DSceneRegion* pRegion = m_lpRegions[nIndex];

            if (!pRegion)
                continue;

            if (pRegion->m_dwLastCullCounter == g_dwCullCounter)
                continue;

            pRegion->m_Data.m_eFrustumIntersectType = (KG3DFrustum::INTERSECT_TYPE)pFrustum->Intersect(pRegion->GetAABBox());

            if (pRegion->m_Data.m_eFrustumIntersectType != KG3DFrustum::OUTSIDE)
            {
                if (pRegion->m_dwLastCullCounter != g_dwCullCounter)
                {
                    pvecContainer->push_back(pRegion);
                    pRegion->m_dwLastCullCounter = g_dwCullCounter;
                }

				KG3DTerrainBlock* pBlock = pRegion->m_lpTerrainBlock;
				if(PutRegionTerrain && pRegion->GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH && pBlock && (nLevelDeep>2) )
				{
					pBlock->AddRef();

					D3DXVECTOR2 vec2Camera(vPosCamera.x, vPosCamera.z);
					D3DXVECTOR2 vec2Center((pRegion->m_nIndexX + 0.5f)* s_fRegionLength, 
						(pRegion->m_nIndexZ + 0.5f) * s_fRegionLength);


					D3DXVECTOR2 vec2Distance = vec2Camera - vec2Center;
					pBlock->m_fViewDistance = D3DXVec2Length(&vec2Distance);
					pRegion->CheckLodLevel();
                    if (
                        pBlock->m_bWaterVisible &&
                        (   
                            (vPosCamera.y > pRegion->m_lpTerrainBlock->m_fWaterHeightMin) ||               			 // water visible
                            (nCameraRegionIndexX == pRegion->m_nIndexX && nCameraRegionIndexZ == pRegion->m_nIndexZ) // maybe under water
                        )
                    )
                    {
                        pTerrainRender->SetWaterVisible(TRUE);
                    }

					pTerrainRender->TryInputTerrainBlock(pBlock);

					pBlock->Release();
                }
            }
		}
	}

	return S_OK;
}

KG3DSceneSection::KG3DSceneSection()
{
	m_bRegionDetail = FALSE;
	m_bAllRegionReady = FALSE;

	ZeroMemory(m_lpRegions,sizeof(KG3DSceneRegion*)*SCENESECTIONNUMREGION*SCENESECTIONNUMREGION);

	m_lpTerrainBlock = NULL;

	_ASSERTE(this->GetType() == KG3DTYPE_SCENE_SECTION);
}

KG3DSceneSection::~KG3DSceneSection()
{
	UnInit();	
}

HRESULT KG3DSceneSection::Init(int nX,int nZ)
{
	m_nIndexX = nX;
	m_nIndexZ = nZ;
	m_bRegionDetail = FALSE;

	D3DXVECTOR3 A((float)m_nIndexX * SCENESECTIONWIDTH,-1800.0F,(float)m_nIndexZ*SCENESECTIONWIDTH);
	D3DXVECTOR3 C = A + D3DXVECTOR3(SCENESECTIONWIDTH,3600.0F,SCENESECTIONWIDTH);

	m_BBox.Clear();
	m_BBox.AddPosition(A);
	m_BBox.AddPosition(C);

	//m_BBox.Extract(m_BoxPos);

	SetTerrainLoadState(BLOCK_LOAD_NODATA);
	SetObjectLoadState(BLOCK_LOAD_NODATA);
	return S_OK;
}

HRESULT KG3DSceneSection::UnInit()
{
	
	KG3DSceneEntityContainer::UnInit();

	FreeAllRegions();

	//SAFE_RELEASE(m_lpTerrainBlock);
	if (m_lpTerrainBlock)
	{
	    KG3DResourceDelayReleaseMgr::Push(m_lpTerrainBlock);
    	m_lpTerrainBlock = NULL;
	}

	m_bRegionDetail = FALSE;
	m_bAllRegionReady = FALSE;
	return S_OK;
}

HRESULT KG3DSceneSection::CreateAllRegions()
{
	HRESULT hr = S_OK;

	for (int nZ=0;nZ<SCENESECTIONNUMREGION;nZ++)
	{
		for (int nX=0;nX<SCENESECTIONNUMREGION;nX++)
		{
			int nX_Region = nX + m_nIndexX  * SCENESECTIONNUMREGION;
			int nZ_Region = nZ + m_nIndexZ  * SCENESECTIONNUMREGION;

			KG3DSceneRegion* pNewRegion = NULL;
			hr = NewOneRegion(&pNewRegion,nX_Region,nZ_Region,nX,nZ);

			KGLOG_COM_PROCESS_ERROR(hr);

			pNewRegion->CreateTerrainBlock();

		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSection::FreeAllRegions()
{
	for (int i=0;i<64;i++)
	{
		if(m_lpRegions[i])
		{
			g_cSceneRegionPool.FreeResource(m_lpRegions[i]);
			m_lpRegions[i] = NULL;
		}
	}
	m_bRegionDetail = FALSE;

	return S_OK;
}

KG3DSceneRegion* KG3DSceneSection::GetRegionByIndex(int nSectionX,int nSectionZ,int nX,int nZ)
{
	int relativeX = nX - nSectionX * SCENESECTIONNUMREGION;
	int relativeZ = nZ - nSectionZ * SCENESECTIONNUMREGION;


	int nIndex = relativeZ * SCENESECTIONNUMREGION + relativeX;

	if((relativeX<0)||(relativeX>=SCENESECTIONNUMREGION))
		return NULL;
	if((relativeZ<0)||(relativeZ>=SCENESECTIONNUMREGION))
		return NULL;

	return m_lpRegions[nIndex];
}

//HRESULT KG3DSceneSection::RemoveObject(KG3DRepresentObject* pObject)
//{
//	if(!m_bRegionDetail)
//		return E_FAIL;
//
//	if(!m_lpRegions)
//		return E_FAIL;
//
//	for (int S=0;S<SCENESECTIONNUMREGION * SCENESECTIONNUMREGION;S++)
//	{
//		KG3DSceneRegion* pRegion = m_lpRegions[S];
//		if(pRegion)
//			pRegion->setObject.erase(pObject);
//	}
//	return S_OK;
//}

HRESULT KG3DSceneSection::DeleteRegion(KG3DSceneRegion* pRegion,int nRegionX,int nRegionZ,int nX_Local,int nZ_Local)
{
	KG_PROCESS_ERROR( nX_Local>=0 );
	KG_PROCESS_ERROR( nZ_Local>=0 );
	KG_PROCESS_ERROR( nX_Local<SCENESECTIONNUMREGION );
	KG_PROCESS_ERROR( nZ_Local<SCENESECTIONNUMREGION );

	int nIndex = nZ_Local * SCENESECTIONNUMREGION + nX_Local;
	KGLOG_PROCESS_ERROR(m_lpRegions[nIndex]);

	g_cSceneRegionPool.FreeResource(m_lpRegions[nIndex]);
	m_lpRegions[nIndex] = NULL;

	CheckRegionDetial();
	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DSceneSection::NewOneRegion(KG3DSceneRegion** ppNewRegion,int nRegionX,int nRegionZ,int nX_Local,int nZ_Local)
{
	HRESULT hr = S_OK;
	KG3DSceneRegion* pNewRegion = NULL;

	KG_PROCESS_ERROR( nX_Local>=0 );
	KG_PROCESS_ERROR( nZ_Local>=0 );
	KG_PROCESS_ERROR( nX_Local<SCENESECTIONNUMREGION );
	KG_PROCESS_ERROR( nZ_Local<SCENESECTIONNUMREGION );

	int nIndex = nZ_Local * SCENESECTIONNUMREGION + nX_Local;
	KGLOG_PROCESS_ERROR(!m_lpRegions[nIndex]);

	{
		pNewRegion = g_cSceneRegionPool.RequestResource();
		KGLOG_PROCESS_ERROR(pNewRegion);

		hr = pNewRegion->Init(nRegionX,nRegionZ);
		KGLOG_COM_PROCESS_ERROR(hr);

		m_lpRegions[nIndex] = pNewRegion;
		(*ppNewRegion) = pNewRegion;

	}

	CheckRegionDetial();
	return S_OK;
Exit0:
	if(pNewRegion)
	{
		g_cSceneRegionPool.FreeResource(pNewRegion);
		pNewRegion = NULL;
	}
	return E_FAIL;
}

HRESULT KG3DSceneSection::CreateTerrainBlock()
{
	HRESULT hr = S_OK;

	KG_PROCESS_SUCCESS(m_lpTerrainBlock);

	m_lpTerrainBlock = g_cTerrainBlockPool.RequestResource();
	KGLOG_PROCESS_ERROR(m_lpTerrainBlock);
	m_lpTerrainBlock->AddRef();

	hr = m_lpTerrainBlock->Init(BLOCK_TYPE_SECTION,m_nIndexX,m_nIndexZ);
	KGLOG_COM_PROCESS_ERROR(hr);

    m_lpTerrainBlock->m_Container = this;
Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSection::DeleteTerrainBlock()
{
	//SAFE_RELEASE(m_lpTerrainBlock);
	
	if (m_lpTerrainBlock)
	{
	    KG3DResourceDelayReleaseMgr::Push(m_lpTerrainBlock);
    	m_lpTerrainBlock = NULL;
	}
	return S_OK;
}

HRESULT KG3DSceneSection::ComputeBBox(BOOL bClear)
{
	if(bClear)
	{
		m_BBox.Clear();
		float x = (float)m_nIndexX * SCENESECTIONWIDTH;
		float z = (float)m_nIndexZ * SCENESECTIONWIDTH;

		if (m_lpTerrainBlock)
		{

			D3DXVECTOR3 A(x                  , m_lpTerrainBlock->m_fHeightMin, z);
			D3DXVECTOR3 B(x+SCENESECTIONWIDTH, m_lpTerrainBlock->m_fHeightMax, z+SCENESECTIONWIDTH);
			m_BBox.AddPosition(A);
			m_BBox.AddPosition(B);
		}
		else
		{
			D3DXVECTOR3 A(x                  , 0, z);
			D3DXVECTOR3 B(x+SCENESECTIONWIDTH, 10, z+SCENESECTIONWIDTH);
			m_BBox.AddPosition(A);
			m_BBox.AddPosition(B);
		}

		m_Data.ComputeBBox(x,z,m_BBox,bClear);

	}
	//m_BBox.Extract(m_BoxPos);

	return S_OK;

}

float KG3DSceneSection::GetGroundHeight(float fX,float fZ)
{
	float Height = 0;
	if(m_bRegionDetail)
	{
		int nIndexX = GetSceneRegionIndex(fX);
		int nIndexZ = GetSceneRegionIndex(fZ);

		int nIndex_X_Local = nIndexX - m_nIndexX * SCENESECTIONNUMREGION;
		int nIndex_Z_Local = nIndexZ - m_nIndexZ * SCENESECTIONNUMREGION;

		KGLOG_PROCESS_ERROR(nIndex_X_Local >=0);
		KGLOG_PROCESS_ERROR(nIndex_Z_Local >=0);
		KGLOG_PROCESS_ERROR(nIndex_X_Local < SCENESECTIONNUMREGION );
		KGLOG_PROCESS_ERROR(nIndex_Z_Local < SCENESECTIONNUMREGION );

		int nIndex = nIndex_Z_Local * SCENESECTIONNUMREGION + nIndex_X_Local;
		KG3DSceneRegion* pRegion = m_lpRegions[nIndex];
		if(pRegion && pRegion->GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH )
		{
			Height = pRegion->GetGroundHeight(fX,fZ);
		}
		else if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
		{
			KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
			if(pTerrain)
			{
				pTerrain->AddRef();
				Height = pTerrain->GetGroundHeight(fX,fZ);
				pTerrain->Release();
			}
		}
	}
	else if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
	{
		KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
		if(pTerrain)
		{		
			pTerrain->AddRef();
			Height = pTerrain->GetGroundHeight(fX,fZ);
			pTerrain->Release();
		}
	}
Exit0:
	return Height;
}

BYTE KG3DSceneSection::GetDetailTextureIndex(float fX,float fZ)
{
	BYTE Index = 0;

	if(m_bRegionDetail)
	{
		int nIndexX = GetSceneRegionIndex(fX);
		int nIndexZ = GetSceneRegionIndex(fZ);

		int nIndex_X_Local = nIndexX - m_nIndexX * SCENESECTIONNUMREGION;
		int nIndex_Z_Local = nIndexZ - m_nIndexZ * SCENESECTIONNUMREGION;

		KGLOG_PROCESS_ERROR(nIndex_X_Local >=0);
		KGLOG_PROCESS_ERROR(nIndex_Z_Local >=0);
		KGLOG_PROCESS_ERROR(nIndex_X_Local < SCENESECTIONNUMREGION );
		KGLOG_PROCESS_ERROR(nIndex_Z_Local < SCENESECTIONNUMREGION );

		int nIndex = nIndex_Z_Local * SCENESECTIONNUMREGION + nIndex_X_Local;
		KG3DSceneRegion* pRegion = m_lpRegions[nIndex];
		if(pRegion && pRegion->GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH )
		{
			Index = pRegion->GetDetailTextureIndex(fX,fZ);
		}
		else if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
		{
			KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
			if(pTerrain)
			{
				pTerrain->AddRef();
				Index = pTerrain->GetDetailTextureIndex(fX,fZ);
				pTerrain->Release();
			}
		}
	}
	else if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
	{
		KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
		if(pTerrain)
		{
			pTerrain->AddRef();
			Index = pTerrain->GetDetailTextureIndex(fX,fZ);
			pTerrain->Release();
		}
	}
Exit0:
	return Index;
}

HRESULT KG3DSceneSection::GetWaterHeight(float& fHeight,float fX,float fZ)
{
	HRESULT hr = E_FAIL;

	if(m_bRegionDetail)
	{
		int nIndexX = GetSceneRegionIndex(fX);
		int nIndexZ = GetSceneRegionIndex(fZ);

		int nIndex_X_Local = nIndexX - m_nIndexX * SCENESECTIONNUMREGION;
		int nIndex_Z_Local = nIndexZ - m_nIndexZ * SCENESECTIONNUMREGION;

		KGLOG_PROCESS_ERROR(nIndex_X_Local >=0);
		KGLOG_PROCESS_ERROR(nIndex_Z_Local >=0);
		KGLOG_PROCESS_ERROR(nIndex_X_Local < SCENESECTIONNUMREGION );
		KGLOG_PROCESS_ERROR(nIndex_Z_Local < SCENESECTIONNUMREGION );

		int nIndex = nIndex_Z_Local * SCENESECTIONNUMREGION + nIndex_X_Local;
		KG3DSceneRegion* pRegion = m_lpRegions[nIndex];
		if(pRegion && pRegion->GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH )
		{
			hr =  pRegion->GetWaterHeight(fHeight,fX,fZ);
		}
		else if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
		{
			KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
			if(pTerrain)
			{
				pTerrain->AddRef();
				hr = pTerrain->GetWaterHeight(fHeight,fX,fZ);
				pTerrain->Release();
			}
		}
	}
	else if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
	{
		KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
		if(pTerrain)
		{
			pTerrain->AddRef();
			hr = pTerrain->GetWaterHeight(fHeight,fX,fZ);
			pTerrain->Release();
		}
	}
Exit0:
	return hr;
}

HRESULT KG3DSceneSection::WedTerrainNormal(KG3DSceneSection* pLeft,KG3DSceneSection* pRight,KG3DSceneSection* pUp,KG3DSceneSection* pDown)
{
	KG3DTerrainBlock* pBlockLeft  = NULL;
	KG3DTerrainBlock* pBlockRight = NULL;
	KG3DTerrainBlock* pBlockUp    = NULL;
	KG3DTerrainBlock* pBlockDown  = NULL;

	if (pLeft)
	{
		pBlockLeft = pLeft->m_lpTerrainBlock;
	}
	if(pRight)
	{
		pBlockRight = pRight->m_lpTerrainBlock;
	}
	if (pUp)
	{
		pBlockUp = pUp->m_lpTerrainBlock;
	}
	if(pDown)
	{
		pBlockDown = pDown->m_lpTerrainBlock;
	}

	if(m_lpTerrainBlock)
	{
		return m_lpTerrainBlock->WedNormal(pBlockLeft,pBlockRight,pBlockUp,pBlockDown);
	}
	return S_OK;
}

void KG3DSceneSection::OnInputObject(KG3DEntityObjectShell* objShell)
{
    extern BOOL g_bClient;
    if (!g_bClient)
    {
        if (!objShell || !objShell->m_lpObject || objShell->m_lpObject->GetType() != REPRESENTOBJECT_PVS || !objShell->m_lpSpaceMgr)
            return;

        ReInputObject(TRUE);  


        D3DXVECTOR2 A = D3DXVECTOR2(objShell->m_vBBox_A.x,objShell->m_vBBox_A.z);
        D3DXVECTOR2 C = D3DXVECTOR2(objShell->m_vBBox_B.x,objShell->m_vBBox_B.z);

        vector<KG3DSceneRegion*> vecRegion;

        if( m_bRegionDetail)
        {
            SelectRegion(&vecRegion,A,C);

            for (size_t i=0;i<vecRegion.size();i++)
            {
                KG3DSceneRegion* pRegion = vecRegion[i];

                if (!pRegion)
                    continue;

                pRegion->ReInputObject(TRUE);
            }
        }
    }
}

HRESULT KG3DSceneSection::InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs)
{
	BOOL bKeep = FALSE;
	D3DXVECTOR2 A,C;
	vector<KG3DSceneRegion*>vecRegion;

	KGLOG_PROCESS_ERROR(pShell);


    /* try first input to pvs */ 
    if (bCheck2Pvs && InputObjectToPvs(pShell))
        return S_OK;


	A = D3DXVECTOR2(pShell->m_vBBox_A.x,pShell->m_vBBox_A.z);
	C = D3DXVECTOR2(pShell->m_vBBox_B.x,pShell->m_vBBox_B.z);

	float width = C.x - A.x;
	float Height = C.y - A.y;

	if(!m_bRegionDetail)
	{
		bKeep = TRUE;
	}
	if ((width > SCENEREGIONWIDTH) || (Height > SCENEREGIONWIDTH))
	{
		bKeep = TRUE;
	}

	if(!bKeep)
	{
		SelectRegion(&vecRegion,A,C);
		if(vecRegion.size()==0)
			bKeep = TRUE;
	}

    


	if(bKeep)
	{
		m_Data.InputObjectShell(pShell,bCheck2Pvs,this);

        /* if the objshell is a pvs object
        then link to the outdoor mgr */
        if (pShell->m_lpSpaceMgr)
            pShell->m_lpSpaceMgr->CheckConnectNode(pShell);

        OnInputObject(pShell);
	}
	else
	{
		for (size_t i=0;i<vecRegion.size();i++)
		{
			KG3DSceneRegion* pRegion = vecRegion[i];
			pRegion->InputObjectShell(pShell, bCheck2Pvs);
		}
	}

    float x = m_BBox.A.x;
    float z = m_BBox.A.z;
    m_BBox.AddPosition(D3DXVECTOR3(x, pShell->m_vBBox_A.y, z));
    m_BBox.AddPosition(D3DXVECTOR3(x, pShell->m_vBBox_B.y, z));
    ComputeBBox(FALSE);


	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSection::InputRepresentObject(KG3DRepresentObject* pObject)
{
	BOOL bKeep = FALSE;
	D3DXVECTOR2 A,C;
	vector<KG3DSceneRegion*>vecRegion;

	KGLOG_PROCESS_ERROR(pObject);

	float width = pObject->m_BBox.GetLength();
	float Height = pObject->m_BBox.GetWidth();
	pObject->m_BBox.GetXZRect(A,C);

	if(!m_bRegionDetail)
	{
		bKeep = TRUE;
	}
	if ((width > SCENEREGIONWIDTH) || (Height > SCENEREGIONWIDTH))
	{
		bKeep = TRUE;
	}

	if(!bKeep)
	{
		SelectRegion(&vecRegion,A,C);
		if(vecRegion.size()==0)
			bKeep = TRUE;
	}

	float x = m_BBox.A.x;
	float z = m_BBox.A.z;
	m_BBox.AddPosition(D3DXVECTOR3(x, pObject->m_BBox.A.y, z));
	m_BBox.AddPosition(D3DXVECTOR3(x, pObject->m_BBox.B.y, z));
    ComputeBBox(FALSE);


	if(bKeep)
	{
		HRESULT hr = KG3DSceneEntityContainer::InputRepresentObject(pObject);
		KG_COM_PROCESS_ERROR(hr);
	}
	else
	{
		for (size_t i=0;i<vecRegion.size();i++)
		{
			KG3DSceneRegion* pRegion = vecRegion[i];
			pRegion->InputRepresentObject(pObject);
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneSection::SelectRegion(vector<KG3DSceneRegion*>*pvecRegion,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	int XStartIndex,ZStartIndex,XEndIndex,ZEndIndex;

	XStartIndex = GetSceneRegionIndex(A.x);
	ZStartIndex = GetSceneRegionIndex(A.y);
	XEndIndex   = GetSceneRegionIndex(C.x);
	ZEndIndex   = GetSceneRegionIndex(C.y);

	int nIndexX_Start = m_nIndexX * SCENESECTIONNUMREGION;
	int nIndexZ_Start = m_nIndexZ * SCENESECTIONNUMREGION;

	XStartIndex = max(nIndexX_Start,XStartIndex);
	ZStartIndex = max(nIndexZ_Start,ZStartIndex);
	XEndIndex   = min(nIndexX_Start + SCENESECTIONNUMREGION - 1,XEndIndex);
	ZEndIndex   = min(nIndexZ_Start + SCENESECTIONNUMREGION - 1,ZEndIndex);

	for (int Z = ZStartIndex; Z <= ZEndIndex; Z++)
	{
		for (int X = XStartIndex; X <= XEndIndex; X++)
		{
			int X_Local = X - nIndexX_Start;
			int Z_Local = Z - nIndexZ_Start;

			KG3DSceneRegion* pRegion = m_lpRegions[Z_Local * SCENESECTIONNUMREGION + X_Local];
			if (pRegion)
			{
				pvecRegion->push_back(pRegion);		
			}
		}
	}

	return S_OK;
}

ULONG KG3DSceneSection::GetType()const
{
	return KG3DTYPE_SCENE_SECTION;
}

HRESULT KG3DSceneSection::FindSoundBall(KG3DRepresentObjectVisitor& visitor,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	Traverse(visitor);

	vector<KG3DSceneRegion*>vecRegion;
	SelectRegion(&vecRegion,A,C);

	for (size_t i = 0;i<vecRegion.size();i++)
	{
		vecRegion[i]->Traverse(visitor);
	}
	return S_OK;
}


KG3DSceneEntityContainer* KG3DSceneSection::GetSubContainer( INT nLocalX, INT nLocalZ )
{
	INT nIndex = nLocalZ * SCENESECTIONNUMREGION + nLocalX;
	if (nIndex >= 0 && nIndex < _countof(m_lpRegions))
	{
		return m_lpRegions[nIndex];
	}
	return NULL;
}

HRESULT KG3DSceneSection::TraverseSubContainer(KG3DEntityCtVisitor& visitor)
{
	HRESULT hr = S_OK;
	for (size_t i = 0; i < _countof(m_lpRegions); ++i)
	{
		KG3DSceneRegion* p = m_lpRegions[i];
		if(NULL == p)
			continue;
		hr = visitor.Accept(*p);
		KG_COM_PROCESS_ERROR(hr);
		if(hr == EM_VISITOR_BREAK)
			break;
	}
Exit0:
	return hr;
}
//////////////////////////////////////////////////////////////////////////
KG3DSceneZone::KG3DSceneZone()
{
	m_nIndexX = 0;
	m_nIndexZ = 0;
	ZeroMemory(m_lpSections,sizeof(KG3DSceneSection*)*64);
	m_bSectionDetail = FALSE;

	m_lpTerrainBlock = NULL;

	_ASSERTE(this->GetType() == KG3DTYPE_SCENE_ZONE);

}

KG3DSceneZone::~KG3DSceneZone()
{
	UnInit();
}

HRESULT KG3DSceneZone::Init(int nX,int nZ)
{
	m_nIndexX = nX;
	m_nIndexZ = nZ;
	m_bSectionDetail = FALSE;

	D3DXVECTOR3 A((float)m_nIndexX * SCENEZONEWIDTH,-2000.0F,(float)m_nIndexZ*SCENEZONEWIDTH);
	D3DXVECTOR3 C = A + D3DXVECTOR3(SCENEZONEWIDTH,4000.0F,SCENEZONEWIDTH);

	m_BBox.Clear();
	m_BBox.AddPosition(A);
	m_BBox.AddPosition(C);

	//m_BBox.Extract(m_BoxPos);

	SetTerrainLoadState(BLOCK_LOAD_NODATA);
	SetObjectLoadState (BLOCK_LOAD_NODATA);

	return S_OK;
}

HRESULT KG3DSceneZone::UnInit()
{
	m_nIndexX = 0;
	m_nIndexZ = 0;
	m_bSectionDetail = FALSE;

	for (int i=0;i<64;i++)
	{
		if(m_lpSections[i])
		{
			g_cSceneSectionPool.FreeResource(m_lpSections[i]);
			m_lpSections[i] = NULL;
		}
	}

	//SAFE_RELEASE(m_lpTerrainBlock);
	if (m_lpTerrainBlock)
	{
	    KG3DResourceDelayReleaseMgr::Push(m_lpTerrainBlock);
    	m_lpTerrainBlock = NULL;
	}

	KG3DSceneEntityContainer::UnInit();
	return S_OK;
}

HRESULT KG3DSceneZone::CheckSectionDetial()
{
	BOOL bSection = FALSE;
	for (int nZ=0;nZ<SCENESECTIONNUMREGION;nZ++)
	{
		for (int nX=0;nX<SCENESECTIONNUMREGION;nX++)
		{
			int nIndex = nZ * SCENESECTIONNUMREGION + nX;
			if(m_lpSections[nIndex])
			{
				bSection = TRUE;
				KG_PROCESS_SUCCESS(bSection);
			}
		}
	}
Exit1:
	m_bSectionDetail = bSection;
	return S_OK;
}

HRESULT KG3DSceneZone::CheckMemoryUseMultiThread(DWORD dwTime,BOOL* pbStop)
{
	for (int nZ=0;nZ<SCENESECTIONNUMREGION;nZ++)
	{
		for (int nX=0;nX<SCENESECTIONNUMREGION;nX++)
		{
			KG_PROCESS_SUCCESS(*pbStop);

			int nIndex = nZ * SCENESECTIONNUMREGION + nX;
			KG3DSceneSection* pSection = m_lpSections[nIndex];

			if(pSection)
			{
				if( g_dwRenderCount - pSection->GetVisibleCount() > dwTime)
				{
					pSection->CheckMemoryUseMultiThread(dwTime,pbStop);
					/*if(pSection->m_lpTerrainBlock && pSection->m_lpTerrainBlock->IsLoaded() && pSection->GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH)
					{
						pSection->SetTerrainLoadState(BLOCK_LOAD_NODATA);
						SAFE_RELEASE(pSection->m_lpTerrainBlock);
					}*/
				}
				
			}
			
			
		}
	}

Exit1:
	return S_OK;
}


HRESULT KG3DSceneZone::CreateSections(BOOL bCreateAllRegion)
{
	HRESULT hr = S_OK;

	for (int nZ=0;nZ<SCENESECTIONNUMREGION;nZ++)
	{
		for (int nX=0;nX<SCENESECTIONNUMREGION;nX++)
		{
			int nX_Section = nX + m_nIndexX  * SCENESECTIONNUMREGION;
			int nZ_Section = nZ + m_nIndexZ  * SCENESECTIONNUMREGION;

			KG3DSceneSection* pNewSection = NULL;
			hr = NewOneSection(&pNewSection,nX_Section,nZ_Section,nX,nZ);

			KGLOG_COM_PROCESS_ERROR(hr);

			if(bCreateAllRegion)
			{
				pNewSection->CreateAllRegions();
			}
			else
			{
				if(abs(nX_Section-nZ_Section)<4)//调试代码
				{
					pNewSection->CreateAllRegions();
				}
			}


			pNewSection->CreateTerrainBlock();

		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DSceneZone::NewOneSection(KG3DSceneSection** ppNewSection,int nSectionX,int nSectionZ,int nX_Local,int nZ_Local)
{
	KG3DSceneSection* pNewSection = NULL;
	HRESULT hr = S_OK;

	KG_PROCESS_ERROR( nX_Local>=0 );
	KG_PROCESS_ERROR( nZ_Local>=0 );
	KG_PROCESS_ERROR( nX_Local<SCENESECTIONNUMREGION );
	KG_PROCESS_ERROR( nZ_Local<SCENESECTIONNUMREGION );

	int nIndex = nZ_Local * SCENESECTIONNUMREGION + nX_Local;
	KGLOG_PROCESS_ERROR(!m_lpSections[nIndex]);


	{
		pNewSection = g_cSceneSectionPool.RequestResource();
		KGLOG_PROCESS_ERROR(pNewSection);


		hr = pNewSection->Init(nSectionX,nSectionZ);
		KGLOG_COM_PROCESS_ERROR(hr);

		m_lpSections[nIndex] = pNewSection;
		(*ppNewSection) = pNewSection;

	}

	CheckSectionDetial();
	return S_OK;
Exit0:
	if(pNewSection)
	{
		g_cSceneSectionPool.FreeResource(pNewSection);
		pNewSection = NULL;
	}
	return E_FAIL;
}

HRESULT KG3DSceneZone::DeleteSection(KG3DSceneSection* pSection,int nSectionX,int nSectionZ,int nX_Local,int nZ_Local)
{
	KG_PROCESS_ERROR( nX_Local>=0 );
	KG_PROCESS_ERROR( nZ_Local>=0 );
	KG_PROCESS_ERROR( nX_Local<SCENESECTIONNUMREGION );
	KG_PROCESS_ERROR( nZ_Local<SCENESECTIONNUMREGION );

	int nIndex = nZ_Local * SCENESECTIONNUMREGION + nX_Local;
	KGLOG_PROCESS_ERROR(m_lpSections[nIndex]);

	g_cSceneSectionPool.FreeResource(m_lpSections[nIndex]);
	m_lpSections[nIndex] =NULL;

	CheckSectionDetial();
	return S_OK;
Exit0:
	return E_FAIL;
}


//extern DWORD g_dw;

static float s_fSectionLength= 12800.0f;
HRESULT KG3DSceneZone::GetVisiableSceneSections(
    vector<KG3DSceneEntityContainer *> *pvecContainer,
    KG3DTerrainRender *pTerrainRender,
    map<int,KG3DTerrainRoad *> *pmapRoad,
    set<KG3DTerrainRiver *> *psetRiver,
    const KG3DFrustum *pFrustum, const D3DXVECTOR3 &vPosCamera,
    int nLevelDeep
)
{
	if(!m_bSectionDetail)
		return S_OK;

	int nXCameraSection,nZCameraSection;
	nXCameraSection = GetSceneSectionIndex(vPosCamera.x);
	nZCameraSection = GetSceneSectionIndex(vPosCamera.z);

	for (int nZ=0;nZ<SCENESECTIONNUMREGION;nZ++)
	{
		for (int nX=0;nX<SCENESECTIONNUMREGION;nX++)
		{
			int nIndex = nZ * SCENESECTIONNUMREGION + nX;
			KG3DSceneSection* pSection = m_lpSections[nIndex];
            if (!pSection)
                continue;

            if (pSection->GetTerrainLoadState() < BLOCK_LOAD_TERRAIN_FINISH)
                continue;

			//pSection->SetVisibleCount(g_dwRenderCount);
			KG3DTerrainBlock* pSectionTerrain = pSection->m_lpTerrainBlock;
			if(pSectionTerrain)
			{
				//pSectionTerrain->SetLastVisibleFrameCount(g_dwRenderCount);
				pSectionTerrain->AddRef();
			}

			//////////////////////////////////////////////////////////////////////////
			pSection->m_Data.m_eFrustumIntersectType = (KG3DFrustum::INTERSECT_TYPE)pFrustum->Intersect(pSection->GetAABBox());
			
            if (pSection->m_Data.m_eFrustumIntersectType != KG3DFrustum::OUTSIDE)
            {
                if (pSection->m_dwLastCullCounter != g_dwCullCounter)
                {
                    pvecContainer->push_back(pSection);
                    pSection->m_dwLastCullCounter = g_dwCullCounter;
                }

				int xx = pSection->m_nIndexX - nXCameraSection;
				int zz = pSection->m_nIndexZ - nZCameraSection;
				int nDistance = xx*xx + zz*zz;
				//////////////////////////////////////////////////////////////////////////////////////////
				D3DXVECTOR2 vec2Camera(vPosCamera.x, vPosCamera.z);
				D3DXVECTOR2 vec2Center((pSection->m_nIndexX + 0.5f)* s_fSectionLength,(pSection->m_nIndexZ + 0.5f) * s_fSectionLength);
				D3DXVECTOR2 vec2Distance = vec2Camera - vec2Center;

				if(pSectionTerrain)
				{
					pSectionTerrain->m_fViewDistance = D3DXVec2Length(&vec2Distance);
				}
				pSection->CheckLodLevel();
				//////////////////////////////////////////////////////////////////////////////////////////
				BOOL bNeedCheckWater = TRUE;
				BOOL bNeedTryInputBlock = FALSE;

				if(pSection->m_bRegionDetail)
				{
					if ((nDistance <= 8 )&&(nLevelDeep>1))
					{
						bNeedTryInputBlock = FALSE;
						bNeedCheckWater    = FALSE;

						pSection->GetVisiableSceneRegions(
                            pvecContainer,
                            pTerrainRender,
                            pFrustum,
                            vPosCamera,
                            nLevelDeep
                        );

                        if (nLevelDeep <= 2 && pSectionTerrain)
                        {
							bNeedCheckWater    = TRUE;
                        }
					}
					else
						bNeedTryInputBlock = TRUE;
				}
				else
					bNeedTryInputBlock = TRUE;

                if(pSectionTerrain)
                {
					if(bNeedCheckWater)
					{
						if (pSectionTerrain->m_bWaterVisible &&
							(vPosCamera.y > pSectionTerrain->m_fWaterHeightMin))
						{
							pTerrainRender->SetWaterVisible(TRUE);
						}
					}

					if(bNeedTryInputBlock)
					{
						pTerrainRender->TryInputTerrainBlock(pSectionTerrain);
					}

                    pSectionTerrain->GetVisibleRoad(pmapRoad,pFrustum);
                    pSectionTerrain->GetVisibleRiver(psetRiver,pFrustum);
                }
			}
			//////////////////////////////////////////////////////////////////////////
			if(pSectionTerrain)
			{
				pSectionTerrain->Release();
			}
		}
	}

	return S_OK;
}

HRESULT KG3DSceneZone::CreateTerrainBlock()
{
	HRESULT hr = S_OK;

	KG_PROCESS_SUCCESS(m_lpTerrainBlock);

	m_lpTerrainBlock = g_cTerrainBlockPool.RequestResource();
	KGLOG_PROCESS_ERROR(m_lpTerrainBlock);
	m_lpTerrainBlock->AddRef();

	hr = m_lpTerrainBlock->Init(BLOCK_TYPE_ZONE,m_nIndexX,m_nIndexZ);
	KGLOG_COM_PROCESS_ERROR(hr);

    m_lpTerrainBlock->m_Container = this;
Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneZone::DeleteTerrainBlock()
{
	//SAFE_RELEASE(m_lpTerrainBlock);
	if (m_lpTerrainBlock)
	{
	    KG3DResourceDelayReleaseMgr::Push(m_lpTerrainBlock);
    	m_lpTerrainBlock = NULL;
	}
	return S_OK;
}

HRESULT KG3DSceneZone::ComputeBBox(BOOL bClear)
{
	if(bClear)
	{
		m_BBox.Clear();
		float x = (float)m_nIndexX * SCENEZONEWIDTH;
		float z = (float)m_nIndexZ * SCENEZONEWIDTH;

		if (m_lpTerrainBlock)
		{

			D3DXVECTOR3 A(x                  , m_lpTerrainBlock->m_fHeightMin, z);
			D3DXVECTOR3 B(x+SCENEZONEWIDTH   , m_lpTerrainBlock->m_fHeightMax, z+SCENEZONEWIDTH);
			m_BBox.AddPosition(A);
			m_BBox.AddPosition(B);
		}
		else
		{
			D3DXVECTOR3 A(x                  , 0 , z);
			D3DXVECTOR3 B(x+SCENEZONEWIDTH   , 10, z+SCENEZONEWIDTH);
			m_BBox.AddPosition(A);
			m_BBox.AddPosition(B);
		}

		m_Data.ComputeBBox(x,z,m_BBox,bClear);
	}
	//m_BBox.Extract(m_BoxPos);

	return S_OK;
}

float KG3DSceneZone::GetGroundHeight(float fX,float fZ)
{
	float Height = 0;
	if(m_bSectionDetail)
	{
		int nIndexX = GetSceneSectionIndex(fX);
		int nIndexZ = GetSceneSectionIndex(fZ);

		int nIndex_X_Local = nIndexX - m_nIndexX * SCENESECTIONNUMREGION;
		int nIndex_Z_Local = nIndexZ - m_nIndexZ * SCENESECTIONNUMREGION;
		KG_PROCESS_ERROR( nIndex_X_Local>=0 );
		KG_PROCESS_ERROR( nIndex_Z_Local>=0 );
		KG_PROCESS_ERROR( nIndex_X_Local<SCENESECTIONNUMREGION );
		KG_PROCESS_ERROR( nIndex_Z_Local<SCENESECTIONNUMREGION );
		
		int nIndex = nIndex_Z_Local * SCENESECTIONNUMREGION + nIndex_X_Local;
		KG3DSceneSection* pSection = m_lpSections[nIndex];
		if(pSection && pSection->GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH)
		{
			Height = pSection->GetGroundHeight(fX,fZ);
		}
		else if (GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
		{
			KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
			if(pTerrain)
			{
				pTerrain->AddRef();
				Height = pTerrain->GetGroundHeight(fX,fZ);
				pTerrain->Release();
			}
		}
	}
	else if (GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
	{
		KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
		if(pTerrain)
		{
			pTerrain->AddRef();
			Height = pTerrain->GetGroundHeight(fX,fZ);
			pTerrain->Release();
		}
	}
Exit0:
	return Height;
}

BYTE KG3DSceneZone::GetDetailTextureIndex(float fX,float fZ)
{
	BYTE Index = 0;

	if(m_bSectionDetail)
	{
		int nIndexX = GetSceneSectionIndex(fX);
		int nIndexZ = GetSceneSectionIndex(fZ);

		int nIndex_X_Local = nIndexX - m_nIndexX * SCENESECTIONNUMREGION;
		int nIndex_Z_Local = nIndexZ - m_nIndexZ * SCENESECTIONNUMREGION;
		KG_PROCESS_ERROR( nIndex_X_Local>=0 );
		KG_PROCESS_ERROR( nIndex_Z_Local>=0 );
		KG_PROCESS_ERROR( nIndex_X_Local<SCENESECTIONNUMREGION );
		KG_PROCESS_ERROR( nIndex_Z_Local<SCENESECTIONNUMREGION );

		int nIndex = nIndex_Z_Local * SCENESECTIONNUMREGION + nIndex_X_Local;
		KG3DSceneSection* pSection = m_lpSections[nIndex];
		if(pSection && pSection->GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH)
		{
			Index = pSection->GetDetailTextureIndex(fX,fZ);
		}
		else if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
		{
			KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
			if(pTerrain)
			{
				pTerrain->AddRef();
				Index = pTerrain->GetDetailTextureIndex(fX,fZ);
				pTerrain->Release();
			}
		}
	}
	else if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
	{
		KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
		if(pTerrain)
		{
			pTerrain->AddRef();
			Index = pTerrain->GetDetailTextureIndex(fX,fZ);
			pTerrain->Release();
		}
	}
Exit0:
	return Index;
}

HRESULT KG3DSceneZone::GetWaterHeight(float& fHeight,float fX,float fZ)
{
	HRESULT hr = E_FAIL;

	if(m_bSectionDetail)
	{
		int nIndexX = GetSceneSectionIndex(fX);
		int nIndexZ = GetSceneSectionIndex(fZ);

		int nIndex_X_Local = nIndexX - m_nIndexX * SCENESECTIONNUMREGION;
		int nIndex_Z_Local = nIndexZ - m_nIndexZ * SCENESECTIONNUMREGION;
		KG_PROCESS_ERROR( nIndex_X_Local>=0 );
		KG_PROCESS_ERROR( nIndex_Z_Local>=0 );
		KG_PROCESS_ERROR( nIndex_X_Local<SCENESECTIONNUMREGION );
		KG_PROCESS_ERROR( nIndex_Z_Local<SCENESECTIONNUMREGION );

		int nIndex = nIndex_Z_Local * SCENESECTIONNUMREGION + nIndex_X_Local;
		KG3DSceneSection* pSection = m_lpSections[nIndex];
		if(pSection && pSection->GetTerrainLoadState() >= BLOCK_LOAD_TERRAIN_FINISH)
		{
			hr = pSection->GetWaterHeight(fHeight,fX,fZ);
		}
		else if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
		{
			KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
			if(pTerrain)
			{
				pTerrain->AddRef();
				hr = pTerrain->GetWaterHeight(fHeight,fX,fZ);
				pTerrain->Release();
			}
		}
	}
	else if (GetTerrainLoadState()>= BLOCK_LOAD_TERRAIN_FINISH && m_lpTerrainBlock)
	{
		KG3DTerrainBlock* pTerrain = m_lpTerrainBlock;
		if(pTerrain)
		{
			pTerrain->AddRef();
			hr = pTerrain->GetWaterHeight(fHeight,fX,fZ);
			pTerrain->Release();
		}
	}
Exit0:
	return hr;
}

HRESULT KG3DSceneZone::WedTerrainNormal(KG3DSceneZone* pLeft,KG3DSceneZone* pRight,KG3DSceneZone* pUp,KG3DSceneZone* pDown)
{
	KG3DTerrainBlock* pBlockLeft  = NULL;
	KG3DTerrainBlock* pBlockRight = NULL;
	KG3DTerrainBlock* pBlockUp    = NULL;
	KG3DTerrainBlock* pBlockDown  = NULL;

	if (pLeft)
	{
		pBlockLeft = pLeft->m_lpTerrainBlock;
	}
	if(pRight)
	{
		pBlockRight = pRight->m_lpTerrainBlock;
	}
	if (pUp)
	{
		pBlockUp = pUp->m_lpTerrainBlock;
	}
	if(pDown)
	{
		pBlockDown = pDown->m_lpTerrainBlock;
	}

	if(m_lpTerrainBlock)
	{
		return m_lpTerrainBlock->WedNormal(pBlockLeft,pBlockRight,pBlockUp,pBlockDown);
	}
	return S_OK;
}

void KG3DSceneZone::OnInputObject(KG3DEntityObjectShell* objShell)
{
    extern BOOL g_bClient;
    if (!g_bClient)
    {
        if (!objShell || !objShell->m_lpObject || objShell->m_lpObject->GetType() != REPRESENTOBJECT_PVS || !objShell->m_lpSpaceMgr)
            return;

        ReInputObject(TRUE);  

        D3DXVECTOR2 A = D3DXVECTOR2(objShell->m_vBBox_A.x,objShell->m_vBBox_A.z);
        D3DXVECTOR2 C = D3DXVECTOR2(objShell->m_vBBox_B.x,objShell->m_vBBox_B.z);

        vector<KG3DSceneSection*>vecSection;
       
        if( m_bSectionDetail)
        {
            SelectSection(&vecSection,A,C);

            for (size_t i=0;i<vecSection.size();i++)
            {
                KG3DSceneSection* pSection = vecSection[i];

                if (!pSection)
                    continue;

                pSection->ReInputObject(TRUE);

                vector<KG3DSceneRegion*> vecRegion;

                if (pSection->m_bRegionDetail)
                {
                    pSection->SelectRegion(&vecRegion, A, C);

                    for (size_t j = 0; j < vecRegion.size(); ++j)
                    {
                        KG3DSceneRegion* pRegion = vecRegion[j];

                        if (!pRegion)
                            continue;

                        pRegion->ReInputObject(TRUE);
                    }
                }
            }
        }
    }
}

HRESULT KG3DSceneZone::InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs)
{
	HRESULT hr = E_FAIL;
	BOOL bKeep = FALSE;
	D3DXVECTOR2 A,C;
	vector<KG3DSceneSection*>vecSection;

	KGLOG_PROCESS_ERROR(pShell);


    /* try first input to pvs */ 
    if (bCheck2Pvs && InputObjectToPvs(pShell))
        return S_OK;


	A = D3DXVECTOR2(pShell->m_vBBox_A.x,pShell->m_vBBox_A.z);
	C = D3DXVECTOR2(pShell->m_vBBox_B.x,pShell->m_vBBox_B.z);

	float width = C.x - A.x;
	float Height = C.y - A.y;

	if(!m_bSectionDetail)
	{
		bKeep = TRUE;
	}
	if ((width > SCENESECTIONWIDTH) || (Height > SCENESECTIONWIDTH))
	{
		bKeep = TRUE;
	}

	if(!bKeep)
	{
		SelectSection(&vecSection,A,C);
		if(vecSection.size()==0)
			bKeep = TRUE;
	}

	if(bKeep)
	{
		m_Data.InputObjectShell(pShell,bCheck2Pvs,this);

        /* if the objshell is a pvs object
        then link to the outdoor mgr */
        if (pShell->m_lpSpaceMgr)
            pShell->m_lpSpaceMgr->CheckConnectNode(pShell);

        OnInputObject(pShell);
	}
	else
	{
		for (size_t i=0;i<vecSection.size();i++)
		{
			KG3DSceneSection* pSection = vecSection[i];
			hr = pSection->InputObjectShell(pShell, bCheck2Pvs);
			KGLOG_COM_PROCESS_ERROR(hr);
		}
	}


    /* ajust the bbox whether the objects input to it */
    float x = m_BBox.A.x;
    float z = m_BBox.A.z;
    m_BBox.AddPosition(D3DXVECTOR3(x, pShell->m_vBBox_A.y, z));
    m_BBox.AddPosition(D3DXVECTOR3(x, pShell->m_vBBox_B.y, z));
    ComputeBBox(FALSE);


	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneZone::InputRepresentObject(KG3DRepresentObject* pObject)
{
	BOOL bKeep = FALSE;
	D3DXVECTOR2 A,C;
	vector<KG3DSceneSection*>vecSection;

	KGLOG_PROCESS_ERROR(pObject);

	float width = pObject->m_BBox.GetLength();
	float Height = pObject->m_BBox.GetWidth();
	pObject->m_BBox.GetXZRect(A,C);

	if(!m_bSectionDetail)
	{
		bKeep = TRUE;
	}
	if ((width > SCENESECTIONWIDTH) || (Height > SCENESECTIONWIDTH))
	{
		bKeep = TRUE;
	}

	if(!bKeep)
	{
		SelectSection(&vecSection,A,C);
		if(vecSection.size()==0)
			bKeep = TRUE;
	}
 
	float x = m_BBox.A.x;
	float z = m_BBox.A.z;
	m_BBox.AddPosition(D3DXVECTOR3(x, pObject->m_BBox.A.y, z));
	m_BBox.AddPosition(D3DXVECTOR3(x, pObject->m_BBox.B.y, z));
    ComputeBBox(FALSE);

	if(bKeep)
	{
		KG3DSceneEntityContainer::InputRepresentObject(pObject);
	}
	else
	{
		for (size_t i=0;i<vecSection.size();i++)
		{
			KG3DSceneSection* pSection = vecSection[i];
			pSection->InputRepresentObject(pObject);
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneZone::SelectSection(vector<KG3DSceneSection*>*pvecSection,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	int XStartIndex,ZStartIndex,XEndIndex,ZEndIndex;

	XStartIndex = GetSceneSectionIndex(A.x);
	ZStartIndex = GetSceneSectionIndex(A.y);
	XEndIndex   = GetSceneSectionIndex(C.x);
	ZEndIndex   = GetSceneSectionIndex(C.y);

	int nIndexX_Start = m_nIndexX * SCENESECTIONNUMREGION;
	int nIndexZ_Start = m_nIndexZ * SCENESECTIONNUMREGION;

	XStartIndex = max(nIndexX_Start,XStartIndex);
	ZStartIndex = max(nIndexZ_Start,ZStartIndex);
	XEndIndex   = min(nIndexX_Start + SCENESECTIONNUMREGION - 1,XEndIndex);
	ZEndIndex   = min(nIndexZ_Start + SCENESECTIONNUMREGION - 1,ZEndIndex);

	for (int Z = ZStartIndex; Z <= ZEndIndex; Z++)
	{
		for (int X = XStartIndex; X <= XEndIndex; X++)
		{
			int XX = X - nIndexX_Start;
			int ZZ = Z - nIndexZ_Start;

			KG3DSceneSection* pSection = m_lpSections[ZZ * SCENESECTIONNUMREGION + XX];
			if (pSection)
			{
				pvecSection->push_back(pSection);		
			}
		}
	}

	return S_OK;
}

ULONG KG3DSceneZone::GetType()const
{
	return KG3DTYPE_SCENE_ZONE;
}

HRESULT KG3DSceneZone::FindSoundBall(KG3DRepresentObjectVisitor& visitor,D3DXVECTOR2 A,D3DXVECTOR2 C)
{
	Traverse(visitor);

	vector<KG3DSceneSection*>vecSection;
	SelectSection(&vecSection,A,C);

	for (size_t i = 0;i<vecSection.size();i++)
	{
		vecSection[i]->FindSoundBall(visitor,A,C);
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////

KG3DSceneEntityContainer* KG3DSceneZone::GetSubContainer( INT nLocalX, INT nLocalZ )
{
	INT nIndex = nLocalZ * SCENESECTIONNUMREGION + nLocalX;
	if (nIndex >= 0 && nIndex < _countof(m_lpSections))
	{
		return m_lpSections[nIndex];
	}
	return NULL;
}

HRESULT KG3DSceneZone::TraverseSubContainer( KG3DEntityCtVisitor& visitor )
{
	HRESULT hr = S_OK;
	for (size_t i = 0; i < _countof(m_lpSections); ++i)
	{
		KG3DSceneSection* p = m_lpSections[i];
		if(NULL == p)
			continue;

		hr = p->TraverseSubContainer(visitor);
		KG_COM_PROCESS_ERROR(hr);

		hr = visitor.Accept(*p);
		KG_COM_PROCESS_ERROR(hr);
		if(hr == EM_VISITOR_BREAK)
			break;
	}
Exit0:
	return hr;
}
//////////////////////////////////////////////////////////////////////////
KG3DSceneBlock::KG3DSceneBlock(void)
{
}

KG3DSceneBlock::~KG3DSceneBlock(void)
{
}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DSceneBlock)
	{
		//检查GetSceneZoneWidth等几个函数的正常性
		_ASSERTE(GetSceneBlockIndex(0, SCENEZONEWIDTH) == 0);
		_ASSERTE(GetSceneZoneIndex(SCENEZONEWIDTH) == 1);
		_ASSERTE(GetSceneZoneIndex(0) == 0);
		_ASSERTE(GetSceneZoneIndex(SCENEZONEWIDTH*1.2) == 1);
		_ASSERTE(GetSceneZoneIndex(-SCENEZONEWIDTH) == -1);
		_ASSERTE(GetSceneZoneIndex(-SCENEZONEWIDTH*1.01) == -2);
		_ASSERTE(GetSceneZoneIndex(-0.5 * SCENEZONEWIDTH) == -1);

		_ASSERTE(GetSceneSectionIndex(1 * SCENESECTIONWIDTH) == 1);
		_ASSERTE(GetSceneRegionIndex(1 * SCENEREGIONWIDTH) == 1);
	}
KG_TEST_END(KG3DSceneBlock)
#endif

HRESULT KG3D_PointLight_Info::InputLight(KG3DModelPointLight* pLight)
{
	if(m_nNumPointLights >= MAX_POINTLIGHT)
	{
		return E_FAIL;
	}

    /*
	D3DXVECTOR3 pos;
	D3DXVec3TransformCoord(&pos,&pLight->m_Translation,&pLight->m_matWorld);
	m_vPointLightPosition[m_nNumPointLights].x = pos.x;
	m_vPointLightPosition[m_nNumPointLights].y = pos.y;
	m_vPointLightPosition[m_nNumPointLights].z = pos.z;
	m_vPointLightPosition[m_nNumPointLights].w = pLight->m_Scaling.y * pLight->m_fRadius;
    */
    
    m_vPointLightPosition[m_nNumPointLights].x = pLight->m_matWorld._41;
    m_vPointLightPosition[m_nNumPointLights].y = pLight->m_matWorld._42;
    m_vPointLightPosition[m_nNumPointLights].z = pLight->m_matWorld._43;
    m_vPointLightPosition[m_nNumPointLights].w = 1.0F / max(1.0f,pLight->m_BBox.GetDiameter() * 0.5f); 
	m_vPointLightColor[m_nNumPointLights]   = *(D3DXVECTOR4*)&pLight->m_vColor;

	m_nNumPointLights++;
	return S_OK;
}

HRESULT KG3DEntityCtVisitorForObj::Accept( KG3DSceneEntityContainer& Container )
{
	//这里等于转调了一下
	struct ObjectVisitor : public KG3DRepresentObjectVisitor
	{
		KG3DEntityCtVisitorForObj* pCtVisitor;
		virtual HRESULT Accept(KG3DRepresentObject& Obj)
		{
			return pCtVisitor->Accept(Obj);
		}
	};

	static ObjectVisitor visitor;
	visitor.pCtVisitor = this;

	return Container.Traverse(visitor);
}

HRESULT KG3DEntityCtVisitorForObjVisitor::Accept( KG3DSceneEntityContainer& Container )
{
	return Container.Traverse(m_objVisitor);
}	

HRESULT KG3DEntityCtVisitorForShell::Accept( KG3DSceneEntityContainer& Container )
{
	//这里等于转调了一下
	struct ObjectShellVisitor : public KG3DEntityObjectShellVisitor
	{
		KG3DEntityCtVisitorForShell* pCtVisitor;
		virtual HRESULT Accept(KG3DEntityObjectShell& Obj)
		{
			return pCtVisitor->Accept(Obj);
		}
	};

	static ObjectShellVisitor visitor;
	visitor.pCtVisitor = this;

	return Container.Traverse(visitor);
}

HRESULT KG3DSceneEntityContainerData::ClearUnLoadObjectShellHandles()
{
	m_Lock.Lock();

	m_setUnLoadObjectHandle.clear();

	m_Lock.Unlock();

	return S_OK;
}

HRESULT KG3DSceneEntityContainerData::EraseUnLoadObjectShellHandles(DWORD dwCount,UINT* pHandle)
{
	m_Lock.Lock();

	for (DWORD i=0;i<dwCount;i++)
	{
		UINT uHandle = pHandle[i];
		set<UINT>::iterator iFind = m_setUnLoadObjectHandle.find(uHandle);
		if(iFind!=m_setUnLoadObjectHandle.end())
		{
			m_setUnLoadObjectHandle.erase(iFind);
		}
	}

	m_Lock.Unlock();
	return S_OK;
}

DWORD KG3DSceneEntityContainerData::GetUnLoadObjectShellHandlesSize()
{
	return (DWORD)m_setUnLoadObjectHandle.size();
}

HRESULT KG3DSceneEntityContainerData::GetUnLoadObjectShellHandles(DWORD& dwCount,UINT* pHandle,DWORD dwMaxCount)
{
	dwCount = 0;
	m_Lock.Lock();

	set<UINT>::iterator i = m_setUnLoadObjectHandle.begin();
	set<UINT>::iterator iend = m_setUnLoadObjectHandle.end();
	while (i!=iend)
	{

		if(dwCount >= dwMaxCount)
		{
			KGLogPrintf(KGLOG_ERR, "KG3DSceneEntityContainerData::GetUnLoadObjectShellHandles Count > 1024 ");
			break;
		}

		UINT uHandle = *i;
		
		pHandle[dwCount] = uHandle;

		i++;
		dwCount++;
	}

	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneEntityContainerData::GetAllObjectShell(set<KG3DEntityObjectShell*>* pSetShell)
{
	m_Lock.Lock();

	set<KG3DEntityObjectShell*>::iterator i = setObjectShell.begin();
	set<KG3DEntityObjectShell*>::iterator iend = setObjectShell.end();
	while (i!=iend)
	{

		KG3DEntityObjectShell* pShell = *i;

		pSetShell->insert(pShell);

		i++;
	}

	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneEntityContainerData::CheckInput2Pvs(D3DXVECTOR3& vPos,KG3DModel* pModel)
{
	HRESULT hr  = E_FAIL;
	m_Lock.Lock();

	for (std::set<KG3DEntityObjectShell*>::iterator i = setObjectShell.begin(); i != setObjectShell.end(); ++i)
	{
		KG3DRepresentObject* obj = (*i)->m_lpObject;

		if (!obj)
			continue;

		if (obj->GetType() == REPRESENTOBJECT_PVS)
		{
			KG3DRepresentObjectPVS* pvs = (KG3DRepresentObjectPVS*)obj;

			if (pvs->IsPositionInSpace(vPos))
			{
				pvs->InputModel(pModel);
				hr = S_OK;
				goto Exit0;
			}
		}
	}

Exit0:
	m_Lock.Unlock();
	return hr;
}

HRESULT KG3DSceneEntityContainerData::FindUnLoadedObjectShellInRect(set<KG3DEntityObjectShell*>*psetShells)
{
	m_Lock.Lock();

	set<KG3DEntityObjectShell*>::iterator s = setObjectShell.begin();
	set<KG3DEntityObjectShell*>::iterator send = setObjectShell.end();

	while (s!=send)
	{
		KG3DEntityObjectShell* pShell = *s;

		pShell->SetVisibleCount(g_dwRenderCount);
		if((pShell->m_uHandle!=-1) && (!pShell->m_bLoaded))
		{
			psetShells->insert(pShell);
		}
		s++;
	}	

	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneEntityContainerData::GetEntityIntersectRect(KG3DSceneEntityList*plistEntity,D3DXVECTOR2& A,D3DXVECTOR2& C,BOOL bShellMustLoaded)
{
	m_Lock.Lock();

	set<KG3DEntityObjectShell*>::iterator s = setObjectShell.begin();
	set<KG3DEntityObjectShell*>::iterator send = setObjectShell.end();
	KG3DSceneEntityList::iterator iFind;
	while (s!=send)
	{
		KG3DEntityObjectShell* pShell = *s;
		
		if(pShell->m_lpObject)
		{
			if(pShell->m_vBBox_A.x <= C.x && pShell->m_vBBox_A.z <= C.y &&
				pShell->m_vBBox_B.x >= A.x && pShell->m_vBBox_B.z >= A.y   )
			{
				KG3DSceneEntity Entity(SCENEENTITY_OBJECT,pShell->m_lpObject);
				iFind = std::find(plistEntity->begin(), plistEntity->end(), Entity);
				if(iFind == plistEntity->end())
					plistEntity->push_back(Entity);
			}
		}
		else if(bShellMustLoaded)
		{
			CHAR strInfo[MAX_PATH];
			wsprintf(strInfo,"[Debug] Shell(%d) is not Ready \n 不生成逻辑障碍信息。", pShell->m_uHandle);
			KGLogPrintf(KGLOG_DEBUG, strInfo);
			MessageBox(NULL,strInfo,"提示",0);
		}
		++s;
	}

	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneEntityContainerData::SetUnLoadObjectShellHandles(set<UINT>*pHandle)
{
	m_Lock.Lock();

	m_setUnLoadObjectHandle.clear();

	m_setUnLoadObjectHandle = *pHandle;

	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneEntityContainerData::OnModelsVisible(set<KG3DModel*>*psetModel,int nLevel)
{
	m_Lock.Lock();

	std::set<KG3DModel*>::iterator t = setModel.begin();
	std::set<KG3DModel*>::iterator iend = setModel.end();
	while (t!=iend)
	{
		KG3DModel* pModel = *t;
		psetModel->insert(pModel);
		++t;
	}

	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneEntityContainerData::_GetModelFromContainers(set<KG3DModel*>*psetModel,const vector<KG3DFrustum>& vFrustum)
{
	m_Lock.Lock();

	std::set<KG3DModel*>::iterator t = setModel.begin();
	std::set<KG3DModel*>::iterator iend = setModel.end();
	while (t!=iend)
	{
		KG3DModel* pModel = *t;

		if (m_eFrustumIntersectType == KG3DFrustum::INSIDE)
		{
			psetModel->insert(pModel);
		}
		else
		{
			int nModelType = pModel->GetType();
			KG3DFrustum::INTERSECT_TYPE eFrustumIntersectType = KG3DFrustum::OUTSIDE;

			pModel->FrameMoveOnce();

			if (nModelType == MESHTYPE_SFX)
			{
				pModel->ComputeBBox();
			}

			BBox ModelBBox = pModel->GetBBox();

			for (size_t i = 0; i < vFrustum.size(); ++i)
			{
				eFrustumIntersectType = (KG3DFrustum::INTERSECT_TYPE)vFrustum[i].Intersect(ModelBBox);
				if (eFrustumIntersectType != KG3DFrustum::OUTSIDE)
					break;
			}

			if (eFrustumIntersectType != KG3DFrustum::OUTSIDE)
				psetModel->insert(pModel);
		}

		t++;
	}

	m_Lock.Unlock();
	return S_OK;
}

HRESULT KG3DSceneEntityContainerData::GetVisibleEntityShell(DWORD& dwCount,KG3DRepresentObject** pObject,DWORD dwMaxCount,const vector<KG3DFrustum> vFrustum)
{
	m_Lock.Lock();

	std::set<KG3DEntityObjectShell*>::iterator t = setObjectShell.begin();
	std::set<KG3DEntityObjectShell*>::iterator iend = setObjectShell.end();
	while (t!=iend)
	{
		KG3DEntityObjectShell* pShell = *t;

		if(pShell->m_lpObject)
		{
			if (pShell->m_lpObject->IsVisible(vFrustum))
			{
				if(dwCount >= dwMaxCount)
					break;
				
				pObject[dwCount] = pShell->m_lpObject;

				dwCount++;
			}
		}
		else
		{
			//pShell->TryDynamicLoad();
		}
		++t;
	}

	m_Lock.Unlock();
	return S_OK;
}


HRESULT KG3DSceneEntityContainerData::GetAllObject(DWORD& dwCount,KG3DRepresentObject** pObject,DWORD dwMaxCount)
{
	m_Lock.Lock();

	std::set<KG3DEntityObjectShell*>::iterator t = setObjectShell.begin();
	std::set<KG3DEntityObjectShell*>::iterator iend = setObjectShell.end();
	while (t!=iend)
	{
		KG3DEntityObjectShell* pShell = *t;

		if(pShell->m_lpObject)
		{
			if(dwCount >= dwMaxCount)
				break;

			pObject[dwCount] = pShell->m_lpObject;

			dwCount++;

		}
		++t;
	}

	m_Lock.Unlock();
	return S_OK;
}

extern BOOL g_bClient;

HRESULT KG3DSceneEntityContainerData::_GetEntityShellFromContainers(
    BOOL bCheckVisibleCount,
    float fVisibleAngleGate,
    float fNearbyObjectVisibleAngleGate,
    DWORD& dwCount,
    KG3DRepresentObject** pazObject,
    DWORD dwMaxCount,
    const D3DXVECTOR3& vCameraPos,
    KG3DScene *pScene,
    const vector<KG3DFrustum>& vFrustum
)
{
    m_Lock.Lock();
    
	std::set<KG3DEntityObjectShell*>::iterator t;
	std::set<KG3DEntityObjectShell*>::iterator iend = setObjectShell.end();

	for (t = setObjectShell.begin(); t !=iend; ++t)
	{
		KG3DEntityObjectShell* pShell = *t;
        KG3DRepresentObject* pObject = pShell->m_lpObject;

        pShell->SetVisibleCount(g_dwRenderCount);

		if(dwCount >= dwMaxCount)
			break;
			
        if (!pObject || (pObject->m_dwLastCullCounter == g_dwCullCounter))
		    continue;
			
        if (pObject->m_bBBoxChanged)
        {
            // 如果Box变了，需要重新放置，以免裁剪错误(以下代码会有很多Lock操作，以免Lock过多造成性能损失，先注释掉)
            //if ((pShell->m_vBBox_A != pObject->m_BBox.A) &&
            //    pShell->m_vBBox_B != pObject->m_BBox.B)
            //{
            //    pShell->m_vBBox_A = pObject->m_BBox.A;
            //    pShell->m_vBBox_B = pObject->m_BBox.B;
            //    pShell->m_fDiameter = pObject->m_BBox.GetDiameter();
            //    pScene->m_lpSceneOutDoorSpaceMgr->InputObjectShell(pShell, FALSE);
            //}

            pObject->m_bBBoxChanged = FALSE;
        }

		if(bCheckVisibleCount)
		{
			BOOL bAngleTest = TRUE;
			static BBox bbox;
			bbox.A = pObject->m_BBox.A;
			bbox.B = pObject->m_BBox.B;

			if (g_bClient) // 客户端才做视角剔除
			{
				float fDistance = D3DXVec3Length(&(pShell->m_vPosition - vCameraPos));
				if (fDistance >= 5000)
				{
					float fVisibleGate = fVisibleAngleGate;
					if (fDistance <= 12000)
					{
						fVisibleGate = fNearbyObjectVisibleAngleGate;
					}

					float fVisibleAngle = atan(pShell->m_fDiameter / fDistance);

					if (fVisibleAngle <= fVisibleGate)
					{
						bAngleTest = FALSE;
					}
				}
			}

			if(bAngleTest)
			{
				if (m_eFrustumIntersectType == KG3DFrustum::INSIDE)
				{
					pazObject[dwCount++] = pObject;
				}
				else
				{
					KG3DFrustum::INTERSECT_TYPE eFrustumIntersectType = KG3DFrustum::OUTSIDE;        

					for (size_t i = 0; i < vFrustum.size(); ++i)
					{
						eFrustumIntersectType = (KG3DFrustum::INTERSECT_TYPE)vFrustum[i].Intersect(bbox);
						if (eFrustumIntersectType != KG3DFrustum::OUTSIDE)
							break;
					}

					if (eFrustumIntersectType != KG3DFrustum::OUTSIDE)
					{
						pazObject[dwCount++] = pObject;
					}
				}
			}
		}
		else
		{
			pazObject[dwCount++] = pObject;
		}
	}

	m_Lock.Unlock();

	return S_OK;
}

HRESULT KG3DSceneEntityContainerData::EraseModel(KG3DModel* pModel)
{
	m_Lock.Lock();
	
	setModel.erase(pModel);

	m_Lock.Unlock();
	return S_OK;
}
