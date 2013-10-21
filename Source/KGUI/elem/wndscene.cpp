/*****************************************************************************************
//	界面窗口体系结构--图形窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-25
*****************************************************************************************/
#include "stdafx.h"
#include "./wndscene.h"
#include "./wndmessage.h"
#include "../common/ksmallobjectpool.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"

namespace
{
	KSmallObjectPool	g_Allocator("KWndScene", sizeof(UI::KWndScene));
}

namespace UI
{

#ifdef KGUI_LUA_LOADER
int KWndScene::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KWndWindow *pWnd = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pWnd = new(std::nothrow) KWndScene;
    KGLOG_PROCESS_ERROR(pWnd);

    if (pLua)
    {
        nRetCode = pWnd->Init(pLua, nTableIndex);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    *ppObj = pWnd;
    // Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE(pWnd);
    }
    return nResult;
}
#endif

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndScene::KWndScene()
{
	memset(&m_Scene, 0, sizeof(m_Scene));
}

KWndScene::~KWndScene()
{
}

void* KWndScene::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KWndScene::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KWndScene::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KWndScene::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

void KWndScene::UpdateSceneViewport()
{
    int nResult = false;
    HRESULT hr = E_FAIL;

    m_Scene.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
    m_Scene.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);

    m_Scene.uViewportWidth = (UINT)m_fWidth;
    m_Scene.uViewportHeight = (UINT)m_fHeight;

    if (m_Scene.p3DScene)
    {
        hr = g_pUI->m_p3DUI->UpdateMultiSceneViewport(&m_Scene, 1);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    nResult = true;
Exit0:
    return;
}

#ifdef KGUI_LUA_LOADER
int KWndScene::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    int nDisTerrainFlag  = false;
    int nDisSkyBoxFlag   = false;
    int nEnableAlphaFlag = false;

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("DisableRenderTerrain", emVAR_TYPE_INT, false, &nDisTerrainFlag,  0),
        KLUA_TABLE_FIELD("DisableRenderSkyBox",  emVAR_TYPE_INT, false, &nDisSkyBoxFlag,   0),
        KLUA_TABLE_FIELD("EnableAlpha",          emVAR_TYPE_INT, false, &nEnableAlphaFlag, 0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KWndWindow::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    EnableRenderTerrain(nDisTerrainFlag == false); 
    EnableRenderSkyBox(nDisSkyBoxFlag == false);
    EnableAlpha(nEnableAlphaFlag != false);

    UpdateSceneViewport();

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif


LRESULT KWndScene::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (KWndWindow::WndProc(uMsg, wParam, lParam))
		return TRUE;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		g_pUI->m_Script.FireWndEvent(this, "OnSceneLButtonDown");
		return TRUE;
	case WM_LBUTTONUP:
		g_pUI->m_Script.FireWndEvent(this, "OnSceneLButtonUp");
		return TRUE;
	case WM_LBUTTONDBLCLK:
		g_pUI->m_Script.FireWndEvent(this, "OnSceneLButtonDown");
		g_pUI->m_Script.FireWndEvent(this, "OnSceneLButtonUp");
		return TRUE;
	case WM_RBUTTONDOWN:
		g_pUI->m_Script.FireWndEvent(this, "OnSceneRButtonDown");
		return TRUE;
	case WM_RBUTTONUP:
		g_pUI->m_Script.FireWndEvent(this, "OnSceneRButtonUp");
		return TRUE;
	case WM_RBUTTONDBLCLK:
		g_pUI->m_Script.FireWndEvent(this, "OnSceneRButtonDown");
		g_pUI->m_Script.FireWndEvent(this, "OnSceneRButtonUp");
		return TRUE;
	default:
		break;
	}

	return FALSE;
}

void KWndScene::OnPaint()
{
	if (m_Scene.p3DScene)
		g_pUI->m_p3DUI->DrawMultiScene(&m_Scene, 1);

	KWndWindow::OnPaint();
}

void KWndScene::OnBreathe()
{
	KWndWindow::OnBreathe();

	if (m_Scene.p3DScene == NULL)
        return;
    
    if ((m_Scene.dwFlags & KG3DUI_SCENE_FLAGS_ENABLE_FRAMEMOVE) == 0)
        return;

    m_Scene.p3DScene->FrameMove();
}
	
void KWndScene::OnSize()
{
	KWndWindow::OnSize();

	UpdateSceneViewport();
}


void KWndScene::OnScale(float fScaleX, float fScaleY)
{
	KWndWindow::OnScale(fScaleX, fScaleY);

	UpdateSceneViewport();
}


void KWndScene::OnMove()
{
	KWndWindow::OnMove();

	UpdateSceneViewport();
}


int KWndScene::CanCastTo(LPCSTR szType) const
{
	return szType == "WndScene" || szType == "WndWindow";
}


void KWndScene::SetScene(IKG3DScene* p3DScene)
{
	m_Scene.p3DScene = p3DScene;
	UpdateSceneViewport();
}


void KWndScene::EnableFrameMove(bool bEnable)
{
	if (bEnable)
	{
		m_Scene.dwFlags |= KG3DUI_SCENE_FLAGS_ENABLE_FRAMEMOVE;
	}
	else
	{
		m_Scene.dwFlags &= ~KG3DUI_SCENE_FLAGS_ENABLE_FRAMEMOVE;
	}
}


void KWndScene::EnableRenderTerrain(bool bEnable)
{
	if (bEnable)
	{
		m_Scene.dwFlags &= ~KG3DUI_SCENE_FLAGS_DISABLE_TERRAIN;
	}
	else
	{
		m_Scene.dwFlags |= KG3DUI_SCENE_FLAGS_DISABLE_TERRAIN;
	}
}


void KWndScene::EnableRenderSkyBox(bool bEnable)
{
	if (bEnable)
	{
		m_Scene.dwFlags &= ~KG3DUI_SCENE_FLAGS_DISABLE_SKYBOX;
	}
	else
	{
		m_Scene.dwFlags |= KG3DUI_SCENE_FLAGS_DISABLE_SKYBOX;
	}
}


void KWndScene::EnableAlpha(bool bEnable)
{
	if (bEnable)
	{
		m_Scene.dwFlags |= KG3DUI_SCENE_FLAGS_ENABLE_ALPHA;
	}
	else
	{
		m_Scene.dwFlags &= ~KG3DUI_SCENE_FLAGS_ENABLE_ALPHA;
	}
}

LPCSTR KWndScene::GetWndType() const
{ 
	return "WndScene";
}

} // namespace UI
