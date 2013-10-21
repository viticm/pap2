#include "StdAfx.h"
#include "kg3dscene.h"
#include "KG3DSceneWndManager.h"
#include "KG3DSceneTable.h"
#include "KG3DGraphicsTool.h"
#include "KG3DModelTable.h"
#include "kg3dmeshtable.h"
#include "KG3DSFX.h"
#include "KG3DSceneModelEditor.h"//by dengzhihui Refactor 2006年11月23日
#include "KG3DSceneSceneEditor.h"//by dengzhihui Refactor 2006年11月27日
#include "KG3DSceneSFXEditor.h"  //by dengzhihui Refactor 2006年11月29日
#include "KG3DEnvironment.h"
//#include "SO3GlobalDef.h"
#include "KG3DRegionInfoManager.h"
#include "KG3DTypes.h"
#include "PhysiscSystem/KG3DPhysiscScene.h"
#include "PhysiscSystem/KG3DPhysiscManager.h"
#include "KG3DSceneCameraMovement.h"
#include "KG3DEnvEffFog.h"
#include "KG3DEnvEffLightSet.h"
#include "KG3DEnvEffLight.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DStateInitializer.h"
#include "KG3DCommonObjectBuilder.h"
#include "KG3DCommonContainer.h"
#include "KG3DMessageBroadcasterGroupImp.h"
#include "KG3DSceneObjectTransformTool.h"
#include "KG3DIntersection.h"
#include "KG3DTextureTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern HRESULT RegisterOutDoorSpaceMgr(KG3DSceneOutDoorSpaceMgr* pMgr);
extern HRESULT UnRegisterOutDoorSpaceMgr(KG3DSceneOutDoorSpaceMgr* pMgr);
////////////////////////////////////////////////////////////////////////////////



KG3DScene::KG3DScene()
	:m_CommonObjNode(KG3DScene::GetCommonObjectBuilder())
	,m_pCameraMovement(*this)
	,m_pBroadcaster(NULL)
{
	m_lpCurrentSpaceNode = NULL;
	m_uCurrentSpaceNodeHandle = 0;


	m_dwType = 0;
	m_ulRefCount = 0;
	m_dwRecordID = 0;

	m_lpPhysicsScene = NULL;

	m_lpSceneOutDoorSpaceMgr = new KG3DSceneOutDoorSpaceMgr(*this);
	m_lpSceneOutDoorSpaceMgr->AddRef();

	m_lpSceneOutDoorSpaceMgr->SetHandle(GetOneNewSpaceNodeHandle());
	RegisterSpaceNode(m_lpSceneOutDoorSpaceMgr);
	RegisterOutDoorSpaceMgr(m_lpSceneOutDoorSpaceMgr);

	m_SceneWndManager = new KG3DSceneWndManager(this);

    m_pEditSpaceNode = m_lpSceneOutDoorSpaceMgr;

	///<CTH>
	m_lpRegionDWInfoManager = NULL;

	//m_lpEvnMap = NULL;//<EnvMap Mov>
    m_pCurRenderWnd = NULL;
	///</CTH>

	m_uOption = 0;

	m_nLogicalSceneX_Start = 0;
	m_nLogicalSceneZ_Start = 0;
	m_nLogicalSceneX_Width = 0;
	m_nLogicalSceneZ_Width = 0;
	
	m_nLogicalRegionX_Start = 0;
	m_nLogicalRegionZ_Start = 0;;
	m_nCountLogicalRegionX = 0;;
	m_nCountLogicalRegionZ = 0;;

	

	try
	{
		m_pBroadcaster = new KG3DMessageBroadcasterGroupImp(KM_LISTENER_TYPE_COUNT, KM_LISTENER_TYPE_BEGIN);
	}
	catch (...)
	{
		
	}
	
}

KG3DScene::~KG3DScene(void)
{
	BOOL bIsPassRecord = FALSE;
	if (g_cRecorder.IsRecord() && m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DSCENEEDITOR_DESTRUCT,GetCurrentTime(),MyParameters);
		bIsPassRecord = TRUE;
		g_cRecorder.Lock();
	}

	UnInit();	

	if (bIsPassRecord)
		g_cRecorder.UnLock();

	SAFE_DELETE(m_pBroadcaster);
}


HRESULT KG3DScene::RefreshPhysisc()
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    KGLOG_PROCESS_ERROR(m_lpPhysicsScene);

    hrRetCode = m_lpPhysicsScene->Refresh();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

ULONG STDMETHODCALLTYPE KG3DScene::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KG3DScene::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        OnReferenceZero(); 
    return uNewRefCount;
}

HRESULT STDMETHODCALLTYPE KG3DScene::QueryInterface( 
	/* [in] */ REFIID riid,
	/* [iid_is][out] */ void **ppvObject)
{
	return E_FAIL;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DScene::GetSceneSFXEditor(IEKG3DSceneSFXEditor **ppiRetSFXEditor)
{
    HRESULT hResult  = E_FAIL;
    KG3DSceneSFXEditor *pSFXEditor = NULL;

    KGLOG_PROCESS_ERROR(ppiRetSFXEditor);

    pSFXEditor = dynamic_cast<KG3DSceneSFXEditor *>(this);
    KGLOG_PROCESS_ERROR(pSFXEditor);

//Exit1:
    *ppiRetSFXEditor = pSFXEditor;
    hResult = S_OK;
Exit0:
    return hResult;
}


///////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DScene::GetFirstRenderEntity(IEKG3DSceneEntity **ppiRetEntity)
{
    HRESULT hResult  = E_FAIL;
    IEKG3DSceneEntity *piEntity = NULL;

    KGLOG_PROCESS_ERROR(ppiRetEntity);

    m_itRenderEntity = m_listRenderEntity.begin();
    if (m_itRenderEntity != m_listRenderEntity.end())
    {
        piEntity = &(*m_itRenderEntity);
        ++m_itRenderEntity;
    }

//Exit1:
    *ppiRetEntity = piEntity;
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DScene::GetNextRenderEntity(IEKG3DSceneEntity **ppiRetEntity)
{
    HRESULT hResult  = E_FAIL;
    IEKG3DSceneEntity *piEntity = NULL;

    KGLOG_PROCESS_ERROR(ppiRetEntity);

    if (m_itRenderEntity != m_listRenderEntity.end())
    {
        piEntity = &(*m_itRenderEntity);
        ++m_itRenderEntity;
    }    

//Exit1:
    *ppiRetEntity = piEntity;
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DScene::FindRenderEntity(
    SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, IEKG3DSceneEntity **ppiRetEntity
)
{
    HRESULT hResult  = E_FAIL;
    IEKG3DSceneEntity *piEntity = NULL;
    KG3DSceneEntity Entity(eEntityType, pvEntityPointer);
    KG3DSceneEntityList::iterator it;

    KGLOG_PROCESS_ERROR(ppiRetEntity);

    it = find(m_listRenderEntity.begin(), m_listRenderEntity.end(), Entity);
    if (it != m_listRenderEntity.end())
    {
        piEntity = &(*it);
    }

//Exit1:
    *ppiRetEntity = piEntity;
    hResult = S_OK;
Exit0:
    return hResult;
}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DScene::GetAmbient(DWORD *pdwRetAmbient)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetAmbient);

    *pdwRetAmbient = GetEnvironment().GetLightSet().GetSceneAmbient();

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

D3DCOLOR KG3DScene::GetAmbient()
{
	return GetEnvironment().GetLightSet().GetSceneAmbient();
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DScene::SetAmbient(DWORD dwAmbient)
{
    HRESULT hResult  = E_FAIL;

    GetEnvironment().GetLightSet().SetSceneAmbient(dwAmbient);

//Exit1:
    hResult = S_OK;
//Exit0:
    return hResult;
}

//////////////////////////////////////////////////////////////////////////
//初始化,清除数据
HRESULT KG3DScene::Init()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    int nInitSetCurScene = false;
    int nInitEntityMgr = false;
    //int nSetEvnMap = false;//<EnvMap Mov>
    int nInitEnvironment = false;

	m_scName = "";

	hr = g_cGraphicsTool.SetCurScene(this);
    KGLOG_COM_PROCESS_ERROR(hr);

    nInitSetCurScene = true;

	KG3DStateInitializer::InitScene(*this);	//让配置器配置自己

	hr = m_EntityMgr.Init(TRUE);
    KGLOG_COM_PROCESS_ERROR(hr);

    nInitEntityMgr = true;

   /* hr = SetEnvMap(TEXT("\\data\\public\\evn.dds"));
    KGLOG_COM_PROCESS_ERROR(hr);

	nSetEvnMap = true;*///<EnvMap Mov>

    m_dwID = 0xffffffff;

	m_lpPhysicsScene = NULL;

    hr = GetEnvironment().Init();
    KGLOG_COM_PROCESS_ERROR(hr);

    nInitEnvironment = true;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nInitEnvironment)
        {
            hr = GetEnvironment().UnInit();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        /*if (nSetEvnMap)//<EnvMap Mov>
        {
            SAFE_RELEASE(m_lpEvnMap);
        }*/
        if (nInitEntityMgr)
        {
            hr = m_EntityMgr.UnInit();
            KGLOG_COM_CHECK_ERROR(hr);
        }
        if (nInitSetCurScene)
        {
            hr = g_cGraphicsTool.SetCurScene(NULL);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
	return hrResult;
}

HRESULT KG3DScene::UnInit()
{
    HRESULT hr = E_FAIL;

	if (NULL != m_SceneWndManager)
	{
		//UnBind里面的Scene，这个操作要在Terrain释放之前做，因为Camera里面可能会调用到Terrain 的GetGroundHeight
		for (list<KG3DSceneOutputWnd*>::iterator it = m_SceneWndManager->m_listOutputWindows.begin()
			; it != m_SceneWndManager->m_listOutputWindows.end(); ++it)
		{
			KG3DSceneOutputWnd* pOutWnd = *it;
			_ASSERTE(NULL != pOutWnd);
			if(NULL == pOutWnd)
				continue;

			pOutWnd->GetCamera().UnBindScene();
		}
	}
	SAFE_DELETE(m_SceneWndManager);

	m_lpPhysicsScene = NULL;
	m_lpCurrentSpaceNode = NULL;

	g_cGraphicsTool.SetCurScene(NULL);
	g_cGraphicsTool.SetCamera(NULL);

	//SAFE_DELETE(m_lpEnvironment);

	SAFE_RELEASE(m_lpRegionDWInfoManager);
	
	//DeleteAllLight();// Last modify by Chen Tianhong: 2007-12-7 14:44:26//Environment的LightSet会释放Light

	if (!m_listRenderEntity.empty())
	{
        KGLogPrintf(KGLOG_ERR, "[Scene] some Render entity not released when UnInit()");
        m_listRenderEntity.clear();
	}

    hr = GetEnvironment().UnInit();
    KGLOG_COM_CHECK_ERROR(hr);

	m_EntityMgr.UnInit();

	if(m_lpSceneOutDoorSpaceMgr)
	{
		UnRegisterSpaceNode(m_lpSceneOutDoorSpaceMgr);
		m_lpSceneOutDoorSpaceMgr->PauseMultiThreadLoad(TRUE);
		UnRegisterOutDoorSpaceMgr(m_lpSceneOutDoorSpaceMgr);
		m_lpSceneOutDoorSpaceMgr->StopMultiThreadLoad();
	}

	SAFE_RELEASE(m_lpSceneOutDoorSpaceMgr);

	// SAFE_RELEASE(m_lpEvnMap);//<EnvMap Mov>

	{
		//防止有东西还在使用被析构的Scene Last modify by Chen Tianhong: 2008-1-18 11:11:44
		KG3DScene* p = g_cGraphicsTool.GetCurScene();
		if (p == this)
		{
			g_cGraphicsTool.SetCurScene(NULL);
		}
	}
	
	return S_OK;//g_cSceneTable.ReleaseResource(this);
}
//////////////////////////////////////////////////////////////////////////
//加载,保存
HRESULT KG3DScene::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
	return S_OK;
}

HRESULT KG3DScene::SaveToFile(const char cszFileName[],   unsigned uOption)
{
	return S_OK;
}

//HRESULT KG3DScene::GetSelectModelList(const D3DXVECTOR2 &fScaleLeftUp,const D3DXVECTOR2 &fScale1RightBottom,
//									  std::vector<IKG3DModel*> &pList)
//{
//	return S_OK;
//}
//
//HRESULT KG3DScene::GetSelectModelList(const D3DXVECTOR2 &fScale,std::vector<IKG3DModel*> &pList)
//{
//	return S_OK;
//}
//
//HRESULT KG3DScene::GetSelectModelListForClient(const D3DXVECTOR2 &fScale,std::vector<IKG3DModel*> &pList)
//{
//	return S_OK;
//}
//
//HRESULT KG3DScene::GetSelectModelList(const D3DXVECTOR2& vPosScreen, std::vector<IKG3DModel*>& pList, BOOL bGetNearestOrAll)
//{
//	return S_OK;
//}

HRESULT KG3DScene::GetSelectPosition(D3DXVECTOR2 const& vScale, D3DXVECTOR3& vPosition)
{
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
//ID,type
DWORD KG3DScene::GetID()
{
	return m_dwID;
}
unsigned KG3DScene::GetType()
{
	return m_dwType;
}

//////////////////////////////////////////////////////////////////////////
//内存管理
HRESULT KG3DScene::PrepareRender(unsigned uOption)//压缩数据
{
	return S_OK;
}
HRESULT KG3DScene::PostRender()//恢复数据，对象可以立即被使用
{
	return S_OK;
}
HRESULT KG3DScene::GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem)//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
{
	return S_OK;;
}
unsigned   KG3DScene::GetLastUseTime()//查询上次使用的时间
{
	return S_OK;;
}
unsigned   KG3DScene::GetUseFrequency()//查询使用频率，一段时间里的使用次数
{
	return S_OK;;
}

HRESULT KG3DScene::OnReferenceZero()
{
	g_cSceneTable.ReleaseResource(this,RELEASE_ATONCE);
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////

HRESULT KG3DScene::FrameMove()
{
    KG3DSceneEntityList listRenderEntity = m_listRenderEntity;
	m_SceneWndManager->FrameMove();
    
    //
	//BOOL bSaveToFile = FALSE;
	//if(m_SceneWndManager->GetCurOutputWnd())
	//{
	//	bSaveToFile = m_SceneWndManager->GetCurOutputWnd()->IsSaveToFile();
	//}
	//m_TimeControl.FrameMove(bSaveToFile);
	
    for_each(listRenderEntity.begin(), listRenderEntity.end(), mem_fun_ref(&KG3DSceneEntity::FrameMove));

	GetCommonObjectNode().FrameMove();
	return S_OK;
}

HRESULT KG3DScene::RenderWindowBegin(IEKG3DSceneOutputWnd* pWnd,BOOL bSetViewPort)
{
	return static_cast<KG3DSceneOutputWnd *>(pWnd)->RenderWindowBegin(bSetViewPort);
}

HRESULT KG3DScene::RenderWindowEnd(IEKG3DSceneOutputWnd* pWnd)
{
	static_cast<KG3DSceneOutputWnd *>(pWnd)->RenderWindowEnd();
	return S_OK;
}
 
HRESULT KG3DScene::RenderForEachWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
	RenderWindowBegin(pWnd,TRUE);
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	g_pd3dDevice->SetTransform( D3DTS_WORLD ,&mat);

	/*float k = timeGetTime()*0.001F;
	D3DXVECTOR3 Pos(800*sinf(k),600,800*cosf(k));
	GetCamera().SetPosition(Pos);
	GetCamera().SetLookAtPosition(D3DXVECTOR3(0,0,0));
	GetCamera().SetPerspective(D3DX_PI/6,1.0F,1,10000);*/

	g_cGraphicsTool.DrawGroundGrid(20, 0xff464646);
	
	g_cGraphicsTool.ApplyLight(*this);//设定默认灯光
	//ApplyLight();

	for_each(m_listRenderEntity.begin(),m_listRenderEntity.end(),mem_fun_ref(&KG3DSceneEntity::Render));

	RenderWindowEnd(pWnd);

	return S_OK;
}

HRESULT KG3DScene::Render(unsigned uOption)		//渲染
{ 
	HRESULT hr = S_OK;

	//	//set state
	//	for (KG3DSceneEntityList::iterator it = m_listRenderEntity.begin(); it != m_listRenderEntity.end(); ++it)
	//	{
	//		KG3DSceneEntity& SceneEntity = *it;
	//		SceneEntity.SetOCState(false);
	//	}

	g_cGraphicsTool.SetCurScene(this);
	for (list<KG3DSceneOutputWnd*>::iterator it = m_SceneWndManager->m_listOutputWindows.begin(); it != m_SceneWndManager->m_listOutputWindows.end(); ++it)
	{
		KG3DSceneOutputWnd* pWnd = *it;

        m_pCurRenderWnd = pWnd;
		_ASSERTE(NULL != pWnd);
		hr = RenderForEachWindow(*pWnd);
		KGLOG_COM_PROCESS_ERROR(hr);

		pWnd->Present();
	}

	hr = S_OK;

Exit0:
	return hr;
}

HRESULT KG3DScene::RenderToScreen(D3DXVECTOR2 const* pA, D3DXVECTOR2 const* pC, D3DXVECTOR2 const* pUVA, D3DXVECTOR2 const* pUVC, float fZ, DWORD dwFlags)
{
	return S_OK;
}

HRESULT KG3DScene::AddPostRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable)
{
	return E_NOTIMPL;
}


HRESULT KG3DScene::AddRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable,BOOL bLight)
{
	if (NULL == pEntity)//不能允许里面没有东西的Entity，否则每个函数都要判断一下，很多地方会当
		return E_FAIL;

    if (!uClass)
    {
	    KG3DSceneEntity Entity(SCENEENTITY_MODEL,pEntity);
       this->AddRenderEntity(Entity, bSelectable);
    }
    else
    {
        KG3DSceneEntity Entity(SCENEENTITY_OBJECT, pEntity);
		this->AddRenderEntity(Entity, bSelectable);
    }
	return S_OK;
}

HRESULT KG3DScene::AddRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer)
{
   /*//SFX的小羊皮类型已经没有使用了，否则这样子调用的话，很难同一AddRenderEntity这个函数
     if (eEntityType == SCENEENTITY_SFX_SUEDE)
        {
            KG3DSFXSuede* pSuede = (KG3DSFXSuede*)pvEntityPointer;
            if (pSuede)
            {
                DWORD dwCount = pSuede->GetModelCount();
                for (DWORD dw = 0; dw < dwCount; dw++)
                {
                    KG3DModel* pModel = (KG3DModel*)pSuede->GetModel(dw);
                    AddRenderEntity(pModel, SCENEENTITY_MODEL, TRUE);
                }
            }
        }
        else*/
    if(pvEntityPointer == NULL)//不能允许里面没有东西的Entity，否则每个函数都要判断一下，很多地方会当
		return E_FAIL;
    {
        KG3DSceneEntity Entity(eEntityType, pvEntityPointer);
		this->AddRenderEntity(Entity, TRUE);
    }
	return S_OK;
}

HRESULT KG3DScene::AddRenderEntity( KG3DSceneEntity Entity, BOOL bIsSelectable/*TRUE*/, BOOL bIsSelected /*FALSE*/)
{
	//<Selectable Refactor>
	/*if (Entity.GetType() == SCENEENTITY_MODEL)
	{
		KG3DModel* pModel = reinterpret_cast<KG3DModel*>(Entity.m_pPointer);
        pModel->SetPhysicsScene(m_lpPhysicsScene);
	}
	m_listRenderEntity.push_back(Entity);
	if (bIsSelectable)
	{
		KG3DSceneSelectionTool& selTool = GetSelectionTool();
		selTool.SetSelectable(Entity, bIsSelectable);
		if (bIsSelected)
		{
			selTool.SetSelected(Entity, bIsSelected);
		}
	}

	return S_OK;*/
	_ASSERTE(NULL && _T("应该是KG3DSceneEditBase的同名函数在起作用，不应该调用到这里"));
	return E_NOTIMPL;
}
HRESULT KG3DScene::RemoveRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer)
{
	if(NULL == pvEntityPointer)//不能允许里面没有东西的Entity，否则每个函数都要判断一下，很多地方会当
		return E_FAIL;
	return RemoveRenderEntity(KG3DSceneEntity(eEntityType, pvEntityPointer), TRUE);//外部调用，最后一个参数一定为True
}


HRESULT KG3DScene::RemoveRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable)
{
	//<Selectable Refactor>
	//if(NULL == pEntity)//不能允许里面没有东西的Entity，否则每个函数都要判断一下，很多地方会当
	//	return E_FAIL;
	//KG3DSceneEntity Entity(uClass ? SCENEENTITY_OBJECT : SCENEENTITY_MODEL, pEntity);
 //   if (Entity.GetType() == SCENEENTITY_MODEL)
 //   {
 //       KG3DModel* pModel = reinterpret_cast<KG3DModel*>(Entity.m_pPointer);
 //       pModel->SetPhysicsScene(NULL);
 //   }
 //   m_listRenderEntity.remove(Entity);

	//KG3DSceneSelectionTool& selTool = GetSelectionTool();
	//selTool.SetSelectable(Entity, FALSE);
	//selTool.SetSelected(Entity, FALSE);
 //   return S_OK;
	_ASSERTE(NULL && _T("不该调用到这里，移动到KG3DSceneEditBase"));
	return E_NOTIMPL;
}

HRESULT KG3DScene::RemoveRenderEntity( KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList /*= TRUE*/ )
{	
	//<Selectable Refactor>
	/*{
        if (Entity.GetType() == SCENEENTITY_MODEL)
        {
            KG3DModel* pModel = reinterpret_cast<KG3DModel*>(Entity.m_pPointer);
            pModel->SetPhysicsScene(NULL);
        }
		m_listRenderEntity.remove(Entity);
		
		KG3DSceneSelectionTool& selTool = GetSelectionTool();
		selTool.SetSelectable(Entity, FALSE);
		if(bEitherRemoveInSelectedList)
			selTool.SetSelected(Entity, FALSE);
	}
	return S_OK;*/
	_ASSERTE(NULL && _T("不该调用到这里，移动到KG3DSceneEditBase"));
	return E_NOTIMPL;
}



HRESULT KG3DScene::DelRenderEntity( KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList /*= TRUE*/ )
{
	return RemoveRenderEntity(Entity, bEitherRemoveInSelectedList);
}

HRESULT KG3DScene::ProcessMessage( UINT message, WPARAM wParam, LPARAM lParam)//<PROCESS_MESSAGE>
{
	return S_OK;
}

IKG3DCamera* KG3DScene::GetCurrentCamera()
{
	return m_SceneWndManager->GetCurOutputWnd() ? &m_SceneWndManager->GetCurOutputWnd()->GetCamera(): NULL;
}

HRESULT  KG3DScene::SetCurrentCamera(IKG3DCamera* pCamera)
{
	return S_OK;
}

HRESULT KG3DScene::LoadScene(LPSTR pFileName,DWORD Option)
{
	return S_OK;
}

HRESULT KG3DScene::GetFloor(const D3DXVECTOR3* pvInput, FLOAT* pfRet)
{
    return E_NOTIMPL;
}

HRESULT KG3DScene::GetWaterHeight(D3DXVECTOR3* pOut, D3DXVECTOR3* pIn)
{
	return E_NOTIMPL;
}

HRESULT KG3DScene::GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR3* pIn)
{
	return S_OK;
}

HRESULT KG3DScene::GetForce (D3DXVECTOR4* pOut,D3DXVECTOR3* pIn)
{
	return S_OK;
}

HRESULT KG3DScene::LogicalToScene(D3DXVECTOR3* pPos, float fX, float fY, float fZ, BOOL bOnFloor)
{
	return S_OK;
}

HRESULT KG3DScene::SceneToLogical(int* pX, int* pY, int* pZ, D3DXVECTOR3 Pos)
{
	return S_OK;
}

HRESULT KG3DScene::ScenePosToScreenPos(D3DXVECTOR3 const& vScenePos, float& fScreenX, float& fScreenY)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    D3DXMATRIX mWorld;
    D3DXMATRIX mView;
    D3DXMATRIX mProjection;
    D3DXVECTOR3 vScreenPos(0.0f, 0.0f, 0.0f);
    KG3DSceneOutputWnd* p3DSceneOutputWnd = NULL;

    KG_ASSERT_EXIT(g_pd3dDevice);

    hr = g_pd3dDevice->GetTransform(D3DTS_WORLD , &mWorld);
    KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

    p3DSceneOutputWnd = m_SceneWndManager->GetCurOutputWnd();
    KG_ASSERT_EXIT(p3DSceneOutputWnd);

    mView = p3DSceneOutputWnd->GetCamera().GetViewMatrix();
    mProjection = p3DSceneOutputWnd->GetCamera().GetProjectionMatrix();

    D3DXVec3Project(&vScreenPos, &vScenePos, &p3DSceneOutputWnd->m_Viewport, &mProjection, &mView, &mWorld);

	if (vScreenPos.z >= 0.0f && vScreenPos.z <= 1.0f)
	{
		fScreenX = vScreenPos.x;
		fScreenY = vScreenPos.y;

        hrResult = S_OK;
    }
	else
	{
		fScreenX = FLT_MIN;
		fScreenY = FLT_MIN;

        hrResult = S_FALSE;
	}

Exit0:
    return hrResult;
}


HRESULT KG3DScene::AddOutputWindow(LPCTSTR szName, HWND hWnd, DWORD dwType, DWORD_PTR dwOption, int* pOutputWindowID)
{
	if (pOutputWindowID == NULL)
		return E_FAIL;

	*pOutputWindowID = m_SceneWndManager->AddOutputWindow(const_cast<LPTSTR>(szName), hWnd, dwType, dwOption);
	
 
    //
    // Band The Scene To Camere
    //
    
    list<KG3DSceneOutputWnd*>::iterator iter = m_SceneWndManager->m_listOutputWindows.begin();
    while (iter != m_SceneWndManager->m_listOutputWindows.end())
    {
        KG3DSceneOutputWnd* pWnd = *iter;
        if (pWnd->m_nID == *pOutputWindowID)
        {
            pWnd->GetCamera().BindScene(this);
            break;
        }
        iter++;
    }

    
    return S_OK;
}

HRESULT KG3DScene::RemoveOutputWindow(int nOutputWindowID)
{
    if (m_pCurRenderWnd && m_pCurRenderWnd->m_nID == nOutputWindowID)
        m_pCurRenderWnd = NULL;
	return m_SceneWndManager->RemoveOutputWindow(nOutputWindowID);
}

HRESULT KG3DScene::GetCurRenderWnd(IEKG3DSceneOutputWnd **ppiRetCurOutputWnd)
{
    if (m_pCurRenderWnd)
    {
        (*ppiRetCurOutputWnd) = m_pCurRenderWnd;
        return S_OK;
    }
    else
    {
        (*ppiRetCurOutputWnd) = NULL;
        return E_FAIL;
    }
}

HRESULT KG3DScene::GetOutputWindow(int nID, IEKG3DSceneOutputWnd **ppiRetSceneOutputWnd)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(ppiRetSceneOutputWnd);

    *ppiRetSceneOutputWnd = m_SceneWndManager->GetOutputWindow(nID);

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DScene::EnablePostRenderEffect(int nOutputWindowID, BOOL bEnable)
{
	HRESULT hr = E_FAIL;
	KG3DSceneOutputWnd* pOutputWnd = NULL;

	pOutputWnd = static_cast<KG3DSceneOutputWnd *>(m_SceneWndManager->GetOutputWindow(nOutputWindowID));
	KGLOG_PROCESS_ERROR(pOutputWnd);

	hr = pOutputWnd->ResetDevice(bEnable, pOutputWnd->m_Viewport.Width, pOutputWnd->m_Viewport.Height);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return hr;
}

HRESULT KG3DScene::EnablePostRenderEffect(int nOutputWindowID, PostRenderEffectType nType, BOOL bEnable)
{
	HRESULT hr = E_FAIL;
	IEKG3DSceneOutputWnd* pOutputWnd = NULL;

	pOutputWnd = m_SceneWndManager->GetOutputWindow(nOutputWindowID);
	KGLOG_PROCESS_ERROR(pOutputWnd);

	hr = pOutputWnd->EnablePostEffect(nType, bEnable);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return hr;
}


HRESULT KG3DScene::OnResizeWindow(PRECT const pRect)
{
	return m_SceneWndManager->OnResizeWindow(pRect);
}

//HRESULT KG3DScene::GetStandHeight(DWORD& dwCount,D3DXVECTOR3* pInter,D3DXVECTOR3& vIn,DWORD dwMaxCount)
//{
//	dwCount = 0;
//	return S_OK;
//}



HRESULT KG3DScene::GetMinimapLayer(IN const D3DVECTOR &vPos, OUT int &nLayer)
{
	nLayer = 0;
	return S_OK;
}

//(by dengzhihui Refactor 2006年11月23日
HRESULT KG3DScene::GetSceneEditorBase(IEKG3DSceneEditorBase **pBase)
{
    HRESULT hResult = NULL;
	KG3DSceneEditorBase *pSceneEditorBase = NULL;
    
    _ASSERTE(pBase);
   
	if (pBase)
	{
        pSceneEditorBase = dynamic_cast<KG3DSceneEditorBase *>(this);
        KGLOG_PROCESS_ERROR(pSceneEditorBase);

		*pBase = dynamic_cast<IEKG3DSceneEditorBase *>(pSceneEditorBase);
        KGLOG_PROCESS_ERROR(pBase);

	}

    hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DScene::GetSceneModelEditor(IEKG3DSceneModelEditor** ppModelEditor)
{
	_ASSERTE(ppModelEditor);
	if (ppModelEditor)
	{
		*ppModelEditor = dynamic_cast<IEKG3DSceneModelEditor*>(this);//有问题

		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DScene::GetSceneSceneEditor(IEKG3DSceneSceneEditor** ppSceneEditor)
{
	_ASSERTE(ppSceneEditor);
	KG_PROCESS_ERROR(NULL != ppSceneEditor);
	{
		KG3DSceneSceneEditor* p = this->GetSceneSceneEditor();	//dynamic_cast是个超慢的操作，需要上百汇编语句，用这种形式比较好
		KG_PROCESS_ERROR(NULL != p);
		*ppSceneEditor = p;//有问题
		return S_OK;
	}
Exit0:
	return E_FAIL;
}
KG3DSceneSceneEditor* KG3DScene::GetSceneSceneEditor()
{
	return NULL;
}

HRESULT KG3DScene::GetSceneObjectEditor(IEKG3DSceneObjectEditor** ppObjectEditor)
{
	_ASSERTE(ppObjectEditor);
	if (ppObjectEditor)
	{
		*ppObjectEditor = dynamic_cast<IEKG3DSceneObjectEditor*>(this);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DScene::GetSceneDataFlowEditor(IEKG3DSceneDataFlowEditor** ppDataFlowEditor)
{
	_ASSERTE(ppDataFlowEditor);
	if (ppDataFlowEditor)
	{
		*ppDataFlowEditor = dynamic_cast<IEKG3DSceneDataFlowEditor*>(this);
		return S_OK;
	}
	return E_FAIL;
}

///<CTH>
HRESULT KG3DScene::GetCameraMovement( IEKG3DSceneCameraMovement** ppiCameraMovement )
{
	_ASSERTE(ppiCameraMovement);
	if (ppiCameraMovement)
	{
		KG3DSceneCameraMovement* pCamMov = KGCommonBufferGetPtr(m_pCameraMovement);
		if(pCamMov)
		{
			*ppiCameraMovement = pCamMov;
			return S_OK;
		}		
	}
	return E_FAIL;
}

HRESULT KG3DScene::GetCameraMovement( IKG3DSceneCameraMovement** ppiCameraMovement )
{
	_ASSERTE(ppiCameraMovement);
	if (ppiCameraMovement)
	{
		KG3DSceneCameraMovement* pCamMov = KGCommonBufferGetPtr(m_pCameraMovement);
		if(pCamMov)
		{
			*ppiCameraMovement = pCamMov;
			return S_OK;
		}	
	}
	return E_FAIL;
}

HRESULT KG3DScene::GetEnvironment( IEKG3DEnvironment** ppiEnvironment )
{
	_ASSERTE(ppiEnvironment);
	if (ppiEnvironment)
	{
		*ppiEnvironment = &GetEnvironment();
		return S_OK;
	}
	return E_FAIL;
}

KG3DEnvironment& KG3DScene::GetEnvironment()
{
	return *(KGCommonBufferGetPtr(m_lpEnvironment));
}

HRESULT KG3DScene::LoadFromFileEx( const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR pKG3DSCENE_INITSTRUCT, DWORD_PTR LOption )
{
	KG3DSCENE_INITSTRUCT* pInitStruct = reinterpret_cast<KG3DSCENE_INITSTRUCT*>(pKG3DSCENE_INITSTRUCT);
	if(NULL != pInitStruct)
		m_GameGPS.Init(pInitStruct->m_vecGameGPSInitPos);
	return LoadFromFile(cszFileName, uFileNameHash, uOption);
}
HRESULT KG3DScene::GetEngineIEInterface( DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, VOID** ppiInterface )
{
	///在传出指针之前，记得做一个静态的向上转型，转成IE接口，
	///这是因为没有保证IE接口一定是类的第一个public，所以可能强转出来的并不正确。
	VOID* pInterface = NULL;
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(ppiInterface);

	switch(dwType)
	{
	case KG3DTYPE_ENVIRONMENT:
		{
			IEKG3DEnvironment* pEnv = NULL;
			hr = this->GetEnvironment(&pEnv);
			KG_PROCESS_ERROR(SUCCEEDED(hr) && pEnv);
			pInterface = static_cast<VOID*>(pEnv);
		}
		break;
	case KG3DTYPE_REGION_INFO_MANAGER:
		{
			IEKG3DRegionInfoManager* pTempInformer = this->GetRegionDWInfoManager();
			KG_PROCESS_ERROR(pTempInformer);
			pInterface = pTempInformer;
		}
		break;
	case KG3DTYPE_CAM_MOVEMENT:
		{
			IEKG3DSceneCameraMovement* pCamMov = KGCommonBufferGetPtr(m_pCameraMovement);
			KG_PROCESS_ERROR(pCamMov);
			pInterface = pCamMov;
		}
	    break;
	case KG3DTYPE_CAM_ANIMATION:
		{
			IEKG3DSceneCameraMovement* pCamMov = KGCommonBufferGetPtr(m_pCameraMovement);
			KG_PROCESS_ERROR(pCamMov);
			IEKG3DSceneCameraAnimation* pCamAni = NULL;
			KG_COM_PROCESS_ERROR(pCamMov->GetCurCamAnimation(&pCamAni));
			_ASSERTE(pCamAni);
			pInterface = pCamAni;
		}
	    break;
	default:
		{
			if (dwType > KG3DTYPE_ENVEFF_BEGIN && dwType < KG3DTYPE_ENVEFF_END)
			{
				IEKG3DEnvEffect* pEffect = NULL;
				hr = this->GetEngineIEAbstructInterface(KG3DTYPE_ENV_EFFECT, dwType, wParam, lParam, reinterpret_cast<VOID**>(&pEffect));
				KG_COM_PROCESS_ERROR(hr);
				_ASSERTE(pEffect);
#if defined(DEBUG) | defined(_DEBUG)
				{
					ULONG dwTempType = pEffect->GetType();
					_ASSERTE(dwType == dwTempType);
				}
#endif
				hr = pEffect->ToSolidInterface(&pInterface);
				KG_PROCESS_ERROR( SUCCEEDED(hr) && pInterface);
			}
			else
			{
				goto Exit0;
			}
		}
	    break;
	}
	
	_ASSERTE(ppiInterface && pInterface);
	*ppiInterface = pInterface;
	return S_OK;
Exit0:
	if (ppiInterface)
	{
		*ppiInterface = NULL;
	}
	return E_FAIL;
}

HRESULT KG3DScene::GetEngineIEAbstructInterface( DWORD dwAbstructType, DWORD dwType, DWORD_PTR wParam, DWORD_PTR lParam, VOID** ppiInterface )
{
	///在传出指针之前，记得做一个静态的向上转型，转成IE接口，
	///这是因为没有保证IE接口一定是类的第一个public，所以可能强转出来的并不正确。
	VOID* pInterface = NULL;
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(ppiInterface);

	switch(dwAbstructType)
	{
	case KG3DTYPE_ENV_EFFECT:
		{
			KG_PROCESS_ERROR(dwType > KG3DTYPE_ENVEFF_BEGIN && dwType < KG3DTYPE_ENVEFF_END);
			IEKG3DEnvironment* pEnv = NULL;
			IEKG3DEnvEffect* pEffect = NULL;

			hr = this->GetEnvironment(&pEnv);
			KG_PROCESS_ERROR(SUCCEEDED(hr) && pEnv);

			hr = pEnv->GetEnvEffect(dwType, _kg_to_smaller_size_cast<INT>(wParam), &pEffect);
			KG_PROCESS_ERROR(SUCCEEDED(hr) && pEffect);
#if defined(DEBUG) | defined(_DEBUG)
			DWORD dwTempType = pEffect->GetType();
			_ASSERTE(dwTempType == dwType);			
#endif
			pInterface = static_cast<VOID*>(pEffect);

		}
		break;
	case KG3DTYPE_ENVEFF_KEY_TABLE:
		{
			KG_PROCESS_ERROR(dwType > KG3DTYPE_ENVEFF_BEGIN && dwType < KG3DTYPE_ENVEFF_END);
			IEKG3DEnvironment* pEnv = NULL;

			hr = this->GetEnvironment(&pEnv);
			KG_PROCESS_ERROR(SUCCEEDED(hr) && pEnv);

			IEKG3DEnvEffDWGC_KeyTable* pKeyTable = NULL;
			IEKG3DEnvEffect* pDWEffect = NULL;
			IEKG3DEnvEffDynamicWeather* pDW = NULL;
			hr = pEnv->GetEnvEffect(KG3DTYPE_ENVEFF_DYNAMIC_WEATHER, 0, &pDWEffect);
			KG_PROCESS_ERROR(SUCCEEDED(hr) && pDWEffect);
			hr = pDWEffect->ToSolidInterface((LPVOID*)&pDW);
			KG_PROCESS_ERROR(SUCCEEDED(hr) && pDW);
			hr = pDW->TL_GetKeyTable(KG3DENVEFF_HANDLE(dwType, _kg_to_smaller_size_cast<INT>(wParam)), &pKeyTable);
			KG_PROCESS_ERROR(SUCCEEDED(hr) && pKeyTable);
#if defined(DEBUG) | defined(_DEBUG)
			DWORD dwTempType = 0;
			pKeyTable->GetType(&dwTempType);
			_ASSERTE(dwTempType == dwType);			
#endif
			pInterface = static_cast<VOID*>(pKeyTable);

		}
		break;
	case KG3DTYPE_COMMON_OBJECT_CONTAINER:
		{
			/*
			KG3DCommonObject* pObj = m_CommonContainerPool.Get(dwType);
						KG_PROCESS_ERROR(NULL != pObj);
						IEKG3DCommonContainer* p = pObj->GetContainerInterface();
						KG_PROCESS_ERROR(p);
						pInterface = static_cast<VOID*>(p);*/
			KG3DCommonObject* pObj = GetCommonObjectNode().Get(KG3DCOBJ_HANDLE(dwType, 0));
			KG_PROCESS_ERROR(NULL != pObj && NULL != pObj->GetContainerInterface());
			IEKG3DCommonContainer* p = pObj->GetContainerInterface();
			KG_PROCESS_ERROR(NULL != p);
			pInterface = static_cast<VOID*>(p);
		}
		break;
	default:
		goto Exit0;
		break;
	}
	_ASSERTE(ppiInterface && pInterface);
	*ppiInterface = pInterface;
	return S_OK;
Exit0:
	if (ppiInterface)
	{
		*ppiInterface = NULL;
	}
	return E_FAIL;
	
}


///</CTH>
extern const KG3DLogicalConstances& g_GetLogicalConstants();
HRESULT KG3DScene::TraverseEntityMeshForGenerateLogicObstacle(
    float fWorldStartX, float fWorldStartY, int nRegionPosX, int nRegionPosY,
    KTraverseEntityMeshForGenerateLogicObstacleCallback* pCallback
)
{
    HRESULT                 hResult              = E_FAIL;
    int                     nRetCode             = 0;
    float                   fRegionStartX        = 0.0f;
    float                   fRegionStartY        = 0.0f;
    D3DXVECTOR2             vRegionLeftBottom;
    D3DXVECTOR2             vRegionRightTop;
    KG3DSceneEntityList   IntersectEntityList;
    BBox                    RegionBox;
	//static BOOL g_bNoCell[1024];
	//ZeroMemory(g_bNoCell,sizeof(BOOL)*1024);

	const KG3DLogicalConstances& lgC = g_GetLogicalConstants();	//利用引用机制创建一个短一点容易读的别名

	/*fRegionStartX = fWorldStartX + LOGICAL_CELL_CM_LENGTH * REGION_GRID_WIDTH * nRegionPosX;
	fRegionStartY = fWorldStartY + LOGICAL_CELL_CM_LENGTH * REGION_GRID_HEIGHT * nRegionPosY;*/
	fRegionStartX = fWorldStartX + lgC.logicalCellCmLength * lgC.regionGridWidth * nRegionPosX;
	fRegionStartY = fWorldStartY + lgC.logicalCellCmLength * lgC.regionGridHeight * nRegionPosY;

    vRegionLeftBottom.x = fRegionStartX;
    vRegionLeftBottom.y = fRegionStartY;

    /*vRegionRightTop.x   = fRegionStartX + LOGICAL_CELL_CM_LENGTH * REGION_GRID_WIDTH;
    vRegionRightTop.y   = fRegionStartY + LOGICAL_CELL_CM_LENGTH * REGION_GRID_HEIGHT;*/
	vRegionRightTop.x   = fRegionStartX + lgC.logicalCellCmLength * lgC.regionGridWidth;
	vRegionRightTop.y   = fRegionStartY + lgC.logicalCellCmLength * lgC.regionGridHeight;

    // TODO: 将这个Callback弄到m_QTree里面去,可以省得构造这个list

    // 取得Region相交的Entity list
	nRetCode = m_lpSceneOutDoorSpaceMgr->GetEntityIntersectRect(&IntersectEntityList, vRegionLeftBottom, vRegionRightTop,TRUE);
	KGLOG_COM_PROCESS_ERROR(nRetCode);

    RegionBox.A.x = fRegionStartX;
    RegionBox.A.y = /*TERRAIN_MIN_HEIGHT*/lgC.terrainMinHeight;
    RegionBox.A.z = fRegionStartY;

    /*RegionBox.B.x = fRegionStartX + LOGICAL_CELL_CM_LENGTH * REGION_GRID_WIDTH;
    RegionBox.B.y = TERRAIN_MAX_HEIGHT;
    RegionBox.B.z = fRegionStartY + LOGICAL_CELL_CM_LENGTH * REGION_GRID_HEIGHT;*/
	RegionBox.B.x = fRegionStartX + lgC.logicalCellCmLength * lgC.regionGridWidth;
	RegionBox.B.y = lgC.terrainMaxHeight;
	RegionBox.B.z = fRegionStartY + lgC.logicalCellCmLength * lgC.regionGridHeight;

	KGLogPrintf(KGLOG_DEBUG,"Region %d %d has Entity %d",nRegionPosX,nRegionPosY,IntersectEntityList.size());

	for (KG3DSceneEntityList::iterator it = IntersectEntityList.begin(); it != IntersectEntityList.end(); ++it)
    {
        KG3DSceneEntity*        pEntity                     = &*it;
        KG3DRepresentObject*    pRepresentObj               = NULL;
        DWORD                   dwObjType                   = 0;
        int                     nDynamicObstacleGroupIndex  = lgC.invalidObstableGroup;
        vector<KG3DModel*>      vModels;

		if (pEntity->GetType() != SCENEENTITY_OBJECT)
        {
            continue;
        }
		
		pRepresentObj = (KG3DRepresentObject*)pEntity->m_pPointer;

		pRepresentObj->FrameMove();//<SLEEP_REFRACTOR>
		pRepresentObj->UpdateTransformation();

        nDynamicObstacleGroupIndex = pRepresentObj->GetDynamicObstacleGroupIndex();

		dwObjType = pRepresentObj->GetType();

		if (
            dwObjType == REPRESENTOBJECT_WAYPOINT || dwObjType == REPRESENTOBJECT_NPC ||
			dwObjType == REPRESENTOBJECT_REFRESHPOINT || (dwObjType == REPRESENTOBJECT_DOODAD && nDynamicObstacleGroupIndex == lgC.invalidObstableGroup) ||
			dwObjType == REPRESENTOBJECT_POLY || dwObjType == REPRESENTOBJECT_TRAFFICPOINT || dwObjType == REPRESENTOBJECT_SOUNDBALL
        )
		{
			continue;
		}

        vModels.clear();
        pRepresentObj->GetSubModels(vModels);
		
		if(vModels.size()==0)
		{
			CHAR strInfo[MAX_PATH];
			wsprintf(strInfo,"[Debug] Region(%.2d, %.2d): Entity(%s) is not Ready \n 不生成逻辑障碍信息。", nRegionPosX, nRegionPosY, pRepresentObj->m_scName.c_str());
			KGLogPrintf(KGLOG_DEBUG, strInfo);
			MessageBox(NULL,strInfo,"提示",0);
		}
		TraverseModelsForGenerateLogicObstacle(vModels,nDynamicObstacleGroupIndex,nRegionPosX,nRegionPosY,vRegionLeftBottom,vRegionRightTop,RegionBox,pCallback);
       
		if(dwObjType == REPRESENTOBJECT_PVS)
		{
			D3DXVECTOR2 vA,vC;
			vector<KG3DRepresentObject*> vObject;
			KG3DRepresentObjectPVS* pPvs = static_cast<KG3DRepresentObjectPVS*>(pRepresentObj);
			RegionBox.GetXZRect(vA,vC);
			pPvs->GetObjectIntersectRect(vObject,vA,vC);
			for (int l = 0;l < (int)vObject.size();l++ )
			{
				KG3DRepresentObject* pObj = vObject[l];
				if(pObj)
				{
					nDynamicObstacleGroupIndex = pObj->GetDynamicObstacleGroupIndex();
					if(pObj->GetType() == REPRESENTOBJECT_DOODAD && nDynamicObstacleGroupIndex != lgC.invalidObstableGroup)
					{
						vModels.clear();
						pObj->GetSubModels(vModels);
						TraverseModelsForGenerateLogicObstacle(vModels,nDynamicObstacleGroupIndex,nRegionPosX,nRegionPosY,vRegionLeftBottom,vRegionRightTop,RegionBox,pCallback);
					}
				}
			}
		}
    }

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DScene::ClearAllHelpLine()
{
	ASSERT(0);
	return E_NOTIMPL; 
}

HRESULT KG3DScene::SetHelpLineSize(size_t nLine, size_t nSize)
{
	ASSERT(0);
	return E_NOTIMPL;
}


HRESULT KG3DScene::SetHelpLineStart(size_t nLine, size_t nStart)
{
	ASSERT(0);
	return E_NOTIMPL;
}

HRESULT KG3DScene::SetHelpLine(size_t nLine, int nSetID, size_t nBegin, size_t nCount, D3DCOLOR color, D3DXVECTOR3* pPos)
{
	ASSERT(0);
	return E_NOTIMPL;
}

HRESULT KG3DScene::GetHelpLine(size_t nLine, size_t* pPoint, D3DXVECTOR3** ppPos)
{
	ASSERT(0);
	return E_NOTIMPL;
}

HRESULT KG3DScene::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	if (m_lpSceneOutDoorSpaceMgr)
    {
        hr = m_lpSceneOutDoorSpaceMgr->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    {
        list<KG3DSceneOutputWnd*>::iterator it = m_SceneWndManager->m_listOutputWindows.begin();
        list<KG3DSceneOutputWnd*>::iterator ie = m_SceneWndManager->m_listOutputWindows.end();
        for (; it != ie; ++it)
        {
            KG3DSceneOutputWnd* p =  *it;

            hr = p->OnLostDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    hr = m_EntityMgr.OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = GetEnvironment().OnLostDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DScene::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	hr = GetEnvironment().OnResetDevice();
    KGLOG_COM_PROCESS_ERROR(hr);

    {
        list<KG3DSceneOutputWnd*>::iterator it = m_SceneWndManager->m_listOutputWindows.begin();
        list<KG3DSceneOutputWnd*>::iterator ie = m_SceneWndManager->m_listOutputWindows.end();
        for (; it != ie; ++it)
        {
            KG3DSceneOutputWnd* p =  *it;

            hr = p->OnResetDevice();
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    m_EntityMgr.OnResetDevice();

	if (m_lpSceneOutDoorSpaceMgr)
    {
        hr = m_lpSceneOutDoorSpaceMgr->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = GetEnvironment().GetFog().Apply();//Fog在设备恢复的时候需要Set一下，不知道为什么
    KGLOG_COM_PROCESS_ERROR(hr);
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

DWORD KG3DScene::GetDataInterfaceVersion()
{
    if (!m_lpSceneOutDoorSpaceMgr)
        return 0;

    return m_lpSceneOutDoorSpaceMgr->GetDataInterfaceVersion();
}

HRESULT KG3DScene::OnSetFocus()
{
	return g_cGraphicsTool.SetCurScene(this);
}

HRESULT KG3DScene::SetSceneSettingData( DWORD dwEnum, DWORD_PTR dwData )
{
	return E_FAIL;//留待以后使用
}

HRESULT KG3DScene::GetSceneSettingData( DWORD dwEnum, DWORD_PTR* pdwData)
{
	return E_FAIL;//留待以后使用
}

KG3DSceneOutputWnd* KG3DScene::GetPrimaryWindow()
{
    if (!m_SceneWndManager)
        return NULL;
	return  m_SceneWndManager->GetSceneEditorMainOutputWnd();
}



HRESULT KG3DScene::ProcessSpaceNodeByPosition(UINT& uCurrentNode,UINT nStartNode,D3DXVECTOR3& vPosition)
{
	HRESULT hr = E_FAIL;

	KG3DSceneSpaceNode* pRootNode = m_lpSceneOutDoorSpaceMgr;
	KG3DSceneSpaceNode* pStartNode = NULL;
	KG3DSceneSpaceNode* pCurrentNode = NULL;

    KG_PROCESS_ERROR(pRootNode);

	if(nStartNode)
	{
		KG3DSceneSpaceNode* pNode = GetSpaceNodeByHandle(nStartNode);
		if(pNode)//如果当前节点有效，从当前节点处开始判断摄像机所处空间节点
		{
			pStartNode = pNode;
		}
		else//摄像机上的节点已经无效，从头开始判断
		{
			pStartNode = pRootNode;
		}
	}
	else//没有进行过节点判断，从根节点开始判断
	{
		pStartNode = pRootNode;
	}
	//////////////////////////////////////////////////////////////////////////
	if (pStartNode->ProcessPosition(&pCurrentNode,vPosition,TRUE))
	{
		uCurrentNode = pCurrentNode->GetHandle();
		hr = S_OK;
	}
	else
	{
		hr = E_FAIL;
	}
Exit0:
	return hr;
}

HRESULT KG3DScene::ProcessSpaceNodeByCamera(KG3DCamera* pCamera)
{
	HRESULT hr = E_FAIL;
	D3DXVECTOR3 vPos = pCamera->GetPosition();
	hr = ProcessSpaceNodeByPosition(pCamera->m_uSceneSpaceNode,pCamera->m_uSceneSpaceNode,vPos);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DScene::CalcFrustum(KG3DCamera* pCamera)
{
    ASSERT(pCamera);

    /*
    clear scene node frustum first 
    */

    m_EntityMgr.ClearFrustum();


    /*
    calc current frustum now
    */

    KG3DSceneSpaceNode* pCurrNode = GetSpaceNodeByHandle(pCamera->m_uSceneSpaceNode);
    ASSERT(pCurrNode);

    KG3DFrustum Frustum;
    Frustum.CreateByCamera(*pCamera);

    KG3DSceneSpaceNode::SetTopVisableNode(NULL);
    pCurrNode->CalcFrustum(Frustum, LOOK_IN_TO_OUT);
    ASSERT(KG3DSceneSpaceNode::GetTopVisableNode());

    return S_OK;
}


KG3DSceneSpaceNode* KG3DScene::GetSpaceNodeByHandle(UINT hHandle)
{
	map<UINT,KG3DSceneSpaceNode*>::iterator iFind = m_mapSceneSpaceNodeHandle.find(hHandle);
	if(iFind==m_mapSceneSpaceNodeHandle.end())
	{
		return NULL;
	}
	else
	{
		return iFind->second;
	}
}

HRESULT KG3DScene::RegisterSpaceNode(KG3DSceneSpaceNode* pNode)
{
	m_mapSceneSpaceNodeHandle[pNode->GetHandle()] = pNode;
	return S_OK;
}

HRESULT KG3DScene::UnRegisterSpaceNode(KG3DSceneSpaceNode* pNode)
{
	map<UINT,KG3DSceneSpaceNode*>::iterator iFind = m_mapSceneSpaceNodeHandle.find(pNode->GetHandle());
	if(iFind==m_mapSceneSpaceNodeHandle.end())
	{
		return E_FAIL;
	}
	else
	{
		m_mapSceneSpaceNodeHandle.erase(iFind);
		return S_OK;
	}
}

UINT KG3DScene::GetOneNewSpaceNodeHandle()
{
	m_uCurrentSpaceNodeHandle++;
	return m_uCurrentSpaceNodeHandle;
}

HRESULT KG3DScene::RefreshOption(unsigned uOption)
{
	return S_OK;
}

KG3DCommonObjectBuilder& KG3DScene::GetCommonObjectBuilder()
{
	static KG3DCommonObjectBuilder stBuilder;
	return stBuilder;
}

KG3DCommonObjectNode& KG3DScene::GetCommonObjectNode()
{
	return m_CommonObjNode;
}

KG3DSceneObjectPlacementTool* KG3DScene::GetObjectPlacementTool()
{
	KG3DSceneObjectPlacementTool* p = reinterpret_cast<KG3DSceneObjectPlacementTool*>(m_CommonObjNode.GetFast(1));
	if (NULL == p)
	{
		p = reinterpret_cast<KG3DSceneObjectPlacementTool*>(m_CommonObjNode.Request(KG3DCOBJ_HANDLE(KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL, 0)
			, NULL, reinterpret_cast<DWORD_PTR>(this), 0, NULL));
		_ASSERTE(NULL != p);
	}
	return p;
}
KG3DSceneSelectionTool& KG3DScene::GetSelectionTool()
{
	KG3DCommonObject* p = this->GetCommonObjectNode().GetFast(2);
	if(NULL != p)
	{
		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IEKG3DSceneSelectionTool, KG3DSceneSelectionTool);
		return *reinterpret_cast<KG3DSceneSelectionTool*>(p->ToAnotherInterface(0));//出来的实际上是IE接口，直接转成引擎接口		
	}
	static KG3DSceneSelectionToolDummy dummyTool;
	return dummyTool;
}

KG3DTransformTool& KG3DScene::GetTransformTool( DWORD dwType )
{
	KG_PROCESS_ERROR(dwType >= KG3DTYPE_TRANSFORM_TOOL_BEGIN && dwType <= KG3DTYPE_TRANSFORM_TOOL_END);

	{
		DWORD_PTR dwWParam = reinterpret_cast<DWORD_PTR>(static_cast<KG3DScene*>(this));
		KG3DCommonObject* p = this->GetCommonObjectNode().Request(KG3DCOBJ_HANDLE(dwType, 0), NULL, dwWParam,0,NULL);
		KG_PROCESS_ERROR(NULL != p);

		_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DCommonObject, KG3DTransformTool);
		return *static_cast<KG3DTransformTool*>(p);
	}
Exit0:
	return ::GetStaticDummyTransformTool();
}

KG3DScene::KG3DCommonObjectNodeScene::KG3DCommonObjectNodeScene(KG3DCommonObjectBuilder& Builder)
:m_Builder(Builder)
{
	KG3DCOBJ_HANDLE AcceleratorTable[] = {
		KG3DCOBJ_HANDLE(KG3DTYPE_SCENE_SHADOW_TOOLS, 0),
		KG3DCOBJ_HANDLE(KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL, 0),
		KG3DCOBJ_HANDLE(KG3DTYPE_SCENE_SELECTION_TOOL, 0),
	};

	this->GetCommonObjectPool().InitAccelaratorTable(AcceleratorTable, _countof(AcceleratorTable));
}

KG3DCommonObjectBuilder* STDMETHODCALLTYPE KG3DScene::KG3DCommonObjectNodeScene::GetCommonObjectBuilder()
{
	return &m_Builder;
}

HRESULT KG3DScene::GetLogicalSceneSize(int* pXStart,int* pZStart,int* pWidthX,int* pWidthZ)
{
	if(pXStart)
		(*pXStart) = m_nLogicalSceneX_Start;
	if(pZStart)
		(*pZStart) = m_nLogicalSceneZ_Start;
	if(pWidthX)
		(*pWidthX ) = m_nLogicalSceneX_Width;
	if(pWidthZ)
		(*pWidthZ) = m_nLogicalSceneZ_Width;

	return S_OK;
}

HRESULT KG3DScene::SetLogicalSceneSize(int  nXStart,int  nZStart,int  nWidthX,int  nWidthZ)
{
	m_nLogicalSceneX_Start = nXStart;
	m_nLogicalSceneZ_Start = nZStart;
	m_nLogicalSceneX_Width = nWidthX;
	m_nLogicalSceneZ_Width = nWidthZ;

	return S_OK;
}
HRESULT KG3DScene::GetAppointLogicalRegion(int& nRegionXStart,int& nRegionZStart,int& nCountRegionX,int& nCountRegionZ)
{
	nRegionXStart = m_nLogicalRegionX_Start;
	nRegionZStart = m_nLogicalRegionZ_Start;
	nCountRegionX = m_nCountLogicalRegionX;
	nCountRegionZ = m_nCountLogicalRegionZ;

	return S_OK;
}

HRESULT KG3DScene::SetAppointLogicalRegion(int nRegionXStart,int nRegionZStart,int nCountRegionX,int nCountRegionZ)
{

	m_nLogicalRegionX_Start = nRegionXStart;
	m_nLogicalRegionZ_Start = nRegionZStart;
	m_nCountLogicalRegionX = nCountRegionX;
	m_nCountLogicalRegionZ = nCountRegionZ;

	return S_OK;
}

KG3DMessageBroadcasterGroup& KG3DScene::GetBroadcasterGroup()
{
	if(NULL != m_pBroadcaster)
		return *m_pBroadcaster;
	static KG3DMessageBroadcasterGroupDummy stInstance;
	return stInstance;
}
KG3DMessageSender& KG3DScene::GetMessageSender(KM_MESSEGE emType)
{
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(IKG3DMessageSenderBase, KG3DMessageSender);
	IKG3DMessageSenderBase& baseSender = this->GetBroadcasterGroup().GetBroadcaster(emType);
	return static_cast<KG3DMessageSender&>(baseSender);
}

HRESULT KG3DScene::RegisterListener( DWORD ListenerType, IKG3DMessageReceiverBase* pReceiver )
{
	KG_PROCESS_ERROR(NULL != pReceiver);
	KG_PROCESS_ERROR(ListenerType >= KM_LISTENER_TYPE_BEGIN && ListenerType < KM_LISTENER_TYPE_END);
	{
		bool bRet = GetBroadcasterGroup().GetBroadcaster(ListenerType).RegisterReceiver(pReceiver);
		KG_PROCESS_ERROR(bRet);
		return S_OK;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DScene::UnRegisterListener( DWORD ListenerType, IKG3DMessageReceiverBase* pReceiver )
{
	KG_PROCESS_ERROR(NULL != pReceiver);
	KG_PROCESS_ERROR(ListenerType >= KM_LISTENER_TYPE_BEGIN && ListenerType < KM_LISTENER_TYPE_END);
	{
		bool bRet = GetBroadcasterGroup().GetBroadcaster(ListenerType).UnRegisterReceiver(pReceiver);
		KG_PROCESS_ERROR(bRet);
		return S_OK;
	}
Exit0:
	return E_FAIL;
}

ULONG KG3DScene::GetEditState()
{
	return SCENESTATE_SELECT;
}

int KG3DScene::GetModelOverlapBox(KG3DModel* pModels[], int maxNum, D3DXVECTOR3 box[8])
{
    return m_EntityMgr.GetModelOverlapBox(pModels, maxNum, box);
}


//HRESULT KG3DScene::PushModel(KG3DModel* pModel)
//{
//	KG_PROCESS_ERROR(pModel);
//	pModel->AddRef();
//	m_listKeepModel.push_back(pModel);
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}
//
//HRESULT KG3DScene::EraseModel(KG3DModel* pModel)
//{
//	list<KG3DModel*>::iterator it = m_listKeepModel.begin();
//	while (it != m_listKeepModel.end())
//	{
//		if( *it == pModel)
//		{
//			pModel->Release();
//			m_listKeepModel.erase(it);
//			break;
//		}
//		it++;
//	}
//	return S_OK;
//}
//
//HRESULT KG3DScene::ClearListKeepModel()
//{
//	list<KG3DModel*>::iterator it = m_listKeepModel.begin();
//	while (it != m_listKeepModel.end())
//	{
//		SAFE_RELEASE(*it);
//		it++;
//	}
//	m_listKeepModel.clear();
//	return S_OK;
//}

HRESULT KG3DScene::GetName( LPTSTR pNameBuffer, size_t uBufferSize )
{
	KG_PROCESS_ERROR(NULL != pNameBuffer);
	{
		errno_t errCode = _tcscpy_s(pNameBuffer, uBufferSize, m_scName.c_str());
		KG_PROCESS_ERROR(errCode == 0);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DScene::RemoveSelectableModel(IKG3DResourceBase *pEntity)
{
	ASSERT(0);
	return S_OK;
}

HRESULT KG3DScene::AutoReleadModifiedResource()
{
	m_EntityMgr.AutoReloadModifiedResource();
	return S_OK;
}

void KG3DScene::BoardCastAction( INT nActionID, INT nExecState, WPARAM wParam, LPARAM lParam )
{
	if(nActionID != EXEACTION_MOUSE_MOVE)
	{
		KG3DMessageActionExecutionStruct TempStruct(nActionID, nExecState, wParam, lParam);

		IEKG3DSceneOutputWnd* pOutputWnd = NULL;
		this->GetCurOutputWnd(&pOutputWnd);

		GetMessageSender(KM_EXECUTE_ACTION).PostMessage(
			KG3DMessage(KM_EXECUTE_ACTION, reinterpret_cast<WPARAM>(&TempStruct), reinterpret_cast<LPARAM>(pOutputWnd)));
	}
	else	//MouseMove消息太频繁了，要加上分时，而且其Listener也是另外注册的
	{
		enum{em_mousemove_check_gap = 30};
		static DWORD s_dwLastTime = 0;
		DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
		if (dwNowTime - s_dwLastTime < em_mousemove_check_gap)
			return;

		s_dwLastTime = dwNowTime;

		KG3DMessageActionExecutionStruct TempStruct(nActionID, nExecState, wParam, lParam);

		IEKG3DSceneOutputWnd* pOutputWnd = NULL;
		this->GetCurOutputWnd(&pOutputWnd);

		GetMessageSender(KM_MOUSE_MOUSE).PostMessage(KG3DMessage(KM_EXECUTE_ACTION
			, reinterpret_cast<WPARAM>(&TempStruct), reinterpret_cast<LPARAM>(pOutputWnd)));
		
	}
}

HRESULT KG3DScene::GetSelectionToolInterface( IEKG3DSceneSelectionTool** ppTool )
{
	if (NULL != ppTool)
	{
		*ppTool = &this->GetSelectionTool();
		return S_OK;
	}
	return E_FAIL;
}

ULONG KG3DScene::GetObjectCount( DWORD dwTypeFilter )
{
	return 0;
}

HRESULT KG3DScene::Traverse( KG3DRepresentObjectVisitor& visitor )
{
	return E_NOTIMPL;
}
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DScene)
{
	_ASSERTE( KM_LISTENER_TYPE_END - KM_LISTENER_TYPE_BEGIN == KM_LISTENER_TYPE_COUNT);
}
KG_TEST_END(KG3DScene)
#endif

//返回当前鼠标点击的地表坐标
HRESULT KG3DScene::GetPickRayPos(D3DXVECTOR3* pOutPos)
{
	HRESULT hr = E_FAIL;

	D3DXVECTOR3 Inter(0, 0, 0);
	D3DXVECTOR3 vSrc;
	D3DXVECTOR3 vNormalizedDir, vPositionSceneSelectionA;
	IEKG3DSceneOutputWnd *piRetCurOutputWnd = NULL;

	GetCurOutputWnd(&piRetCurOutputWnd);
	KG_PROCESS_ERROR(piRetCurOutputWnd);

	piRetCurOutputWnd->GetPickRay(&vSrc, &vNormalizedDir, NULL);
	FLOAT fTempRet = FLT_MAX;
	if (::RayIntersection(*this, vSrc, vNormalizedDir, &fTempRet, NULL, NULL))
	{
		vPositionSceneSelectionA = vSrc + vNormalizedDir * fTempRet;;
		vPositionSceneSelectionA.y += 0.0;
		*pOutPos = vPositionSceneSelectionA;
		hr = S_OK;
	}
	else
		hr = E_FAIL;
Exit0:
	return hr;
}

LPDIRECT3DCUBETEXTURE9 KG3DScene::GetEnvMap( LPTSTR pNameBuffer, size_t uBufferCount )
{
	if (NULL != pNameBuffer)
	{
		_tcscpy_s(pNameBuffer, uBufferCount, KG3DTextureTable::GetDefaultEnvMapName());
	}
	return KG3DTextureTable::GetDefaultEnvMap();
}

DWORD KG3DScene::GetGroundType(const D3DXVECTOR3 &vec3Pos)
{
	return INVALID_GROUNDTYPE;
}


HRESULT KG3DScene::SetFocus(D3DXVECTOR3 &vec3Pos)
{
	m_vec3FocusPos = vec3Pos;
	return S_OK;
}

HRESULT KG3DScene::GetFocus(D3DXVECTOR3* pvecPos)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(pvecPos);
	*pvecPos = m_vec3FocusPos;
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DScene::TraverseModelsForGenerateLogicObstacle(vector<KG3DModel*>& vModels,int nDynamicObstacleGroupIndex,
														  int nRegionPosX, int nRegionPosY, 
														  D3DXVECTOR2& vRegionLeftBottom,D3DXVECTOR2& vRegionRightTop,
														  BBox& RegionBox,KTraverseEntityMeshForGenerateLogicObstacleCallback* pCallback)
{
	int nRetCode = 0;
	static BOOL g_bNoCell[1024];
	ZeroMemory(g_bNoCell,sizeof(BOOL)*1024);
	HRESULT hResult =E_FAIL;
	for (size_t i = 0; i < vModels.size(); ++i)
	{
		KG3DModel*  pModel       = NULL;
		DWORD       dwModelType  = 0;
		KG3DMesh*   pMesh        = NULL;
		D3DXMATRIX  MatFinal;

		pModel = vModels[i];
		ASSERT(pModel);
		//_ASSERTE(pModel->IsResourceReady() && _T("模型还没加载完！"));
		dwModelType   = pModel->GetType();   

		if(!(dwModelType == MESHTYPE_DEFAULT || dwModelType==MESHTYPE_SPEEDTREE))
		{
			continue;
		}

		if(!pModel->IsResourceReady(FALSE))
		{
			CHAR strInfo[MAX_PATH];
			wsprintf(strInfo,"[Debug] Region(%.2d, %.2d): Entity(%s) is not Ready \n 不生成逻辑障碍信息。", nRegionPosX, nRegionPosY, pModel->m_scName.c_str());
			//wsprintf(strInfo,"[Debug] Region(%.2d, %.2d): Entity(%s) is not Ready \n", nRegionPosX, nRegionPosY, pModel->m_scName.c_str());
			KGLogPrintf(KGLOG_DEBUG, strInfo);
			MessageBox(NULL,strInfo,"提示",0);
			if(pModel->m_bInMultiThreadLoadList)
			{
				do 
				{
					Sleep(10);
				} while (pModel->m_bInMultiThreadLoadList);		
			}
			else
			{
				hResult = pModel->LoadFromFile(pModel->m_scName.c_str(), 0, 0);
				if (SUCCEEDED(hResult))
					pModel->FrameMove();
			}
		}

		//nRetCode = RegionBox.IsTwoBoxIntersect(pModel->m_BBox);
		pModel->Helper_UpdateTransformation(TRUE);
		D3DXVECTOR2 rA,rC;
		pModel->m_BBox.GetXZRect(rA,rC);

		nRetCode = RegionBox.IsIntersectRect(rA,rC);
		if (!nRetCode)
			continue;

		if (dwModelType == MESHTYPE_DEFAULT)
		{
			pMesh = pModel->GetMesh();

			if (!pMesh || !pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL])
			{
				CHAR strInfo[MAX_PATH];
				wsprintf(strInfo,"[Debug] Region(%.2d, %.2d): Mesh of Mesh(%s) is not Ready \n 不生成逻辑障碍信息。", nRegionPosX, nRegionPosY, pModel->m_scName.c_str());
				KGLogPrintf(KGLOG_DEBUG, strInfo);
				MessageBox(NULL,strInfo,"提示",0);
				continue;
			}

			KG3DOBJProperty *pObjProperty = NULL;
			pMesh->GetObjProperty((BYTE**)&pObjProperty);
			if (!pObjProperty || !pObjProperty->bAutoProduceObstacle)
				continue;

			for (int k=0;k<(int)pMesh->GetNumSubset();k++)
			{
				DWORD dwOption = 0;
				pModel->GetMaterialSubsetOption(k,&dwOption);
				g_bNoCell[k] = dwOption & MATERIAL_OPTION_NOLOGICALCELL;
			}

		}
		else if (dwModelType == MESHTYPE_SPEEDTREE)
		{
			KG3DModelSpeedTree* pSpdTree = static_cast<KG3DModelSpeedTree*>(pModel);
			BYTE* pbyteProperty = NULL;
			HRESULT hRetCode = E_FAIL;
			hRetCode = pSpdTree->GetObjProperty(&pbyteProperty);
			if (FAILED(hRetCode))
				continue;
			KG3DOBJProperty *objProperty = reinterpret_cast<KG3DOBJProperty*>(pbyteProperty);
			if (!pSpdTree || !objProperty->bAutoProduceObstacle)
				continue;
			KG3DModel *pTreeModel = pSpdTree->m_lpModelTree;
			if (!pTreeModel)
				continue;
			pMesh = pTreeModel->GetMesh();


			if (!pMesh || !pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL])
				continue;

			for (int k=0;k<(int)pMesh->GetNumSubset();k++)
			{
				DWORD dwOption = 0;
				pTreeModel->GetMaterialSubsetOption(k,&dwOption);
				g_bNoCell[k] = dwOption & MATERIAL_OPTION_NOLOGICALCELL;
			}
		}
		else
		{
			continue;
		}

		MatFinal = pModel->m_matWorld;

		BOOL bIntersect = pModel->m_BBox.IsIntersectRect(vRegionLeftBottom,vRegionRightTop);
		if(!bIntersect)
		{
			CHAR strInfo[MAX_PATH];
			wsprintf(strInfo,"[Debug] Region(%.2d, %.2d): Model(%s) BBox is Wrong! \n 不生成逻辑障碍信息。", nRegionPosX, nRegionPosY, pModel->m_scName.c_str());
			KGLogPrintf(KGLOG_DEBUG, strInfo);
			MessageBox(NULL,strInfo,"提示",0);
		}
		/*MatFinal._41 -= fWorldStartX;
		MatFinal._43 -= fWorldStartY;*/

		/* KGLogPrintf(
		KGLOG_DEBUG, "[Debug] Region(%.2d, %.2d): Entity(%s)\n", 
		nRegionPosX, nRegionPosY, pMesh->m_scName.c_str()
		);*/

		nRetCode = (*pCallback)(
			RegionBox.A.x, RegionBox.A.y, RegionBox.A.z,
			pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL],pMesh->GetNumSubset(),g_bNoCell, MatFinal,
			nDynamicObstacleGroupIndex,
			(LPSTR)pMesh->m_scName.c_str()

			);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DScene::LoadCameraAnimation(LPSTR pszCameraAniFile)
{
    return S_OK;
}

HRESULT KG3DScene::StartPlayCamAni()
{
    return S_OK;
}

HRESULT KG3DScene::StopPlayCamAni()
{
    return S_OK;
}

HRESULT KG3DScene::PausePlayCamAni()
{
    return S_OK;
}