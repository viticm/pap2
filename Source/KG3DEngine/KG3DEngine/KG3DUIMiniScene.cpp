#include "stdafx.h"
#include "./KG3DUIMiniScene.h"
#include "./KG3DUI.h"
#include "./KG3DGraphicsTool.h"
#include "./KG3DModel.h"
#include "./KG3DModelST.h"
#include "./PhysiscSystem/KG3DPhysiscScene.h"
#include "./PhysiscSystem/KG3DPhysiscManager.h"
#include "KG3DEngineManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

unsigned KG3DUIMiniScene::GetLastUseTime() { ASSERT(0); return 0; }
unsigned KG3DUIMiniScene::GetUseFrequency() { ASSERT(0); return 0; }
unsigned KG3DUIMiniScene::GetType() { ASSERT(0); return 0; }
DWORD    KG3DUIMiniScene::GetID()  { ASSERT(0); return 0; }
ULONG STDMETHODCALLTYPE KG3DUIMiniScene::AddRef() {	ASSERT(0); return 0; }
ULONG STDMETHODCALLTYPE KG3DUIMiniScene::Release() { ASSERT(0); return 0; }
HRESULT STDMETHODCALLTYPE KG3DUIMiniScene::QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption) { ASSERT(0); return S_OK; }
HRESULT KG3DUIMiniScene::SaveToFile(const char cszFileName[], unsigned uOption) { ASSERT(0); return S_OK; }
HRESULT KG3DUIMiniScene::GetDateInformation(int *pnMemory, int *pnVideoMemory, int *pnCompressMem, int *pnCompressVideoMem) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::AddOutputWindow(LPCTSTR szName, HWND hWnd, DWORD dwType, DWORD_PTR dwOption, int* pOutputWindowID) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::RemoveOutputWindow(int nOutputWindowID) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::EnablePostRenderEffect(int nOutputWindowID, BOOL bEnable) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::EnablePostRenderEffect(int nOutputWindowID, PostRenderEffectType nType, BOOL bEnable) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::GetFloor(const D3DXVECTOR3* pvInput, FLOAT* pfRet) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::GetWaterHeight(D3DXVECTOR3* pOut, D3DXVECTOR3* pIn) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR3* pIn) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::LogicalToScene(D3DXVECTOR3* pPos, float fX, float fY, float fZ, BOOL bOnFloor) {	ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::SceneToLogical(int* pX, int* pY, int* pZ, D3DXVECTOR3 Pos) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::ScenePosToScreenPos(D3DXVECTOR3 const& vScenePos, float& fScreenX, float& fScreenY) { ASSERT(0); return E_NOTIMPL; }
//HRESULT KG3DUIMiniScene::GetSelectModelList(const D3DXVECTOR2 &vScale,std::vector<IKG3DModel*> &pList) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::GetSelectModelList(const D3DXVECTOR2& vPosScreen, std::vector<IKG3DModel*>& pList, BOOL bGetNearestOrAll){ASSERT(0); return E_NOTIMPL;}
//HRESULT KG3DUIMiniScene::GetSelectModelList(const D3DXVECTOR2 &vScaleLeftUp,const D3DXVECTOR2 &vScale1RightBottom, std::vector<IKG3DModel*> &pList) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::GetSelectPosition(D3DXVECTOR2 const& vScale, D3DXVECTOR3& vPosition) {	ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::GetMinimapLayer(IN const D3DVECTOR &vPos, OUT int &nLayer) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::GetCameraMovement(IKG3DSceneCameraMovement** ppiCameraMovement) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::InitGameTimeClock(KG3D_GAMETIME_INITSTRUCT* pInitStruct) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::Render(unsigned uOption) {	ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::PrepareRender(unsigned uOption) { ASSERT(0); return S_OK; }
HRESULT KG3DUIMiniScene::PostRender() { ASSERT(0); return S_OK; }
HRESULT KG3DUIMiniScene::SetCurrentCamera(IKG3DCamera* p3DCamera) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::OnResizeWindow(PRECT const pRect) { return S_OK; }
HRESULT KG3DUIMiniScene::ClearAllHelpLine() { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::SetHelpLineSize(size_t nLine, size_t nSize) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::SetHelpLineStart(size_t nLine, size_t nStart) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::SetHelpLine(size_t nLine, int nSetID, size_t nBegin, size_t nCount, D3DCOLOR color, D3DXVECTOR3* pPos) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::GetHelpLine(size_t nLine, size_t* pPoint, D3DXVECTOR3** ppPos) { ASSERT(0); return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::GetTerrainCross(D3DXVECTOR3* vInter, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDst) { return E_NOTIMPL; }
HRESULT KG3DUIMiniScene::SetFocus(D3DXVECTOR3& vecPos){return S_OK;}
float KG3DUIMiniScene::GetLoadingProcess() { return 1.0F; }


KG3DUIMiniScene::KG3DUIMiniScene() :
    m_pPhysicsScene(NULL),
    m_LastFrameMoveTime(0)
{
}

KG3DUIMiniScene::~KG3DUIMiniScene()
{
	UnInit();
    ASSERT(!m_pPhysicsScene);
}

HRESULT KG3DUIMiniScene::Init()
{
	//m_converMap.Default();
    return S_OK;
}

HRESULT KG3DUIMiniScene::UnInit()
{
	ASSERT(m_RenderEntities.empty());

    if (m_pPhysicsScene)
    {
        ASSERT(g_cEngineManager.m_pPhysicsManager);
        g_cEngineManager.m_pPhysicsManager->DeleteOnePhysiscScene(m_pPhysicsScene);
        m_pPhysicsScene = NULL;
    }
    return S_OK;
}

HRESULT KG3DUIMiniScene::RenderToScreen(D3DXVECTOR2 const* pA, D3DXVECTOR2 const* pC, D3DXVECTOR2 const* pUVA, 
										D3DXVECTOR2 const* pUVC, float fZ, DWORD dwFlags)
{
	HRESULT hr = E_FAIL;
	D3DVIEWPORT9 Viewport;
	KG3DCamera* p3DCamera = NULL;

	ASSERT(pA);
	ASSERT(pC);
	ASSERT(g_pd3dDevice);

	Viewport.X = (DWORD)pA->x;
	Viewport.Y = (DWORD)pA->y;
	Viewport.Width = (DWORD)(pC->x - pA->x);
	Viewport.Height = (DWORD)(pC->y - pA->y);
	Viewport.MinZ = 0.0F;
	Viewport.MaxZ = 1.0F;

	if ((dwFlags & KG3DUI_SCENE_FLAGS_DISABLE_VIEWPORT) == 0)
	{
		hr = g_pd3dDevice->SetViewport(&Viewport);
		KGLOG_COM_CHECK_ERROR(hr);

		D3DRECT rect = { Viewport.X, Viewport.Y, Viewport.X + Viewport.Width, Viewport.Y + Viewport.Height };

		hr = g_pd3dDevice->Clear(1, &rect, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00FFFFFF, 1.0f, 0);
		KGLOG_COM_CHECK_ERROR(hr);
	}

	m_3DCamera.RestoreGlobalProject();
    m_3DCamera.SetCamera();

	p3DCamera = g_cGraphicsTool.GetCamera();
	g_cGraphicsTool.SetCamera(&m_3DCamera);

	hr = Render(dwFlags);
	KGLOG_COM_CHECK_ERROR(hr);

	g_cGraphicsTool.SetCamera(p3DCamera);

	g_cGraphicsTool.SetCurrentConverMap(NULL);

	return S_OK;
}

extern VOID g_SetShaderSharedParamsDefault(const D3DXVECTOR3& camPos);
HRESULT KG3DUIMiniScene::Render(DWORD dwFlags)
{
	KG3DUIRenderStateGuard aRenderStateGuard[5];

	aRenderStateGuard[0].Save(D3DRS_DEPTHBIAS, 0);
	aRenderStateGuard[1].Save(D3DRS_AMBIENT, 0xFF808080);
	aRenderStateGuard[2].Save(D3DRS_LIGHTING, FALSE);
	aRenderStateGuard[3].Save(D3DRS_ZENABLE, TRUE);
	aRenderStateGuard[4].Save(D3DRS_ZWRITEENABLE, TRUE);
	g_cGraphicsTool.SetCurrentConverMap(&KG3DTerrainConverMap::GetUniqueDefault());

	g_SetShaderSharedParamsDefault(m_3DCamera.GetPosition());

	KRenderEntityList::iterator it = m_RenderEntities.begin();
	KRenderEntityList::iterator ie = m_RenderEntities.end();
	for (; it != ie; ++it)
	{
        KG3DModel* p3DModel = *it;
        ASSERT(p3DModel);
        p3DModel->Render(0, NULL);
	}
    g_RenderRenderTwiceInfo();    
	g_cGraphicsTool.SetCurrentConverMap(NULL);
	
	return S_OK;
}


HRESULT KG3DUIMiniScene::AddRenderEntity(IKG3DResourceBase* pEntity, unsigned uClass, BOOL bSelectable,BOOL bLight)
{
    KG_USE_ARGUMENT(uClass);
	KG_USE_ARGUMENT(bSelectable);

	KG3DModel* p3DModel = dynamic_cast<KG3DModel*>(pEntity);
    if (p3DModel)
    {
        if (g_cEngineManager.m_pPhysicsManager && !m_pPhysicsScene)
        {
            g_cEngineManager.m_pPhysicsManager->NewOnePhysiscScene(&m_pPhysicsScene);
        }
        if (m_pPhysicsScene)
        {
            p3DModel->SetPhysicsScene(m_pPhysicsScene);
        }
        p3DModel->SetClientAdd(TRUE);
        m_RenderEntities.push_back(p3DModel);
    }

	return S_OK;
}

HRESULT KG3DUIMiniScene::RemoveRenderEntity(IKG3DResourceBase* pEntity, unsigned uClass, BOOL bSelectable)
{
	KG_USE_ARGUMENT(uClass);
	KG_USE_ARGUMENT(bSelectable);

	KG3DModel* p3DModel = dynamic_cast<KG3DModel*>(pEntity);
    if (NULL == p3DModel)
		return E_FAIL;
    {
        KRenderEntityList::iterator itRender = m_RenderEntities.begin();
        KRenderEntityList::iterator ieRender = m_RenderEntities.end();
        for (; itRender != ieRender; ++itRender)
        {
            if (p3DModel == *itRender)
            {
                p3DModel->SetPhysicsScene(NULL);
                m_RenderEntities.erase(itRender);
                break;
            }
        }
    }

	return S_OK;
}


IKG3DCamera* KG3DUIMiniScene::GetCurrentCamera()
{
	return &m_3DCamera;
}

HRESULT KG3DUIMiniScene::LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WOption, DWORD_PTR LOption)
{
	return LoadFromFile(cszFileName, uFileNameHash, uOption);
}

HRESULT KG3DUIMiniScene::FrameMove()
{
    DWORD dwCurTime = timeGetTime();

	KRenderEntityList::iterator it = m_RenderEntities.begin();
	KRenderEntityList::iterator ie = m_RenderEntities.end();
	for (; it != ie; ++it)
	{
        KG3DModel* p3DModel = *it;
        ASSERT(p3DModel);

        if (!g_cEngineManager.GetFrameMoveOption(FRAMEMOVE_SKIN_MESH) && p3DModel->IsModelST())
            continue;

        p3DModel->m_bVisible = TRUE;
        p3DModel->FrameMove();
	}
    
    if (g_cEngineManager.GetFrameMoveOption(FRAMEMOVE_PHYSICS) && m_pPhysicsScene)
    {
        if (m_LastFrameMoveTime == 0)
        {
            m_LastFrameMoveTime = dwCurTime;
        }
        IKG3DCamera* pCamera = GetCurrentCamera();
        if (pCamera)
        {
            D3DXVECTOR3 vLookAtPos;
            D3DXVECTOR3 vCameraPos;
            pCamera->GetLookAtPosition(&vLookAtPos);
            pCamera->GetPosition(&vCameraPos);
            m_pPhysicsScene->FrameMove(dwCurTime - m_LastFrameMoveTime, vCameraPos, vLookAtPos);
            m_LastFrameMoveTime = dwCurTime;
        }
    }


    return S_OK;
}

HRESULT KG3DUIMiniScene::RefreshOption(unsigned uOption)
{
	return S_OK;
}

HRESULT KG3DUIMiniScene::OnLostDevice()
{
    return S_OK;
}

HRESULT KG3DUIMiniScene::OnResetDevice()
{
	return S_OK;
}
