////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemsence.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-22 17:07:58
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./itemscene.h"
#include "./itemeventmgr.h"
#include "./decoder.h"
#include "../../common/ksmallobjectpool.h"
#include "../../script/kscriptmgr.h"
#include "../../KGUI.h"

////////////////////////////////////////////////////////////////////////////////
namespace
{
	KSmallObjectPool	g_Allocator("KItemScene", sizeof(UI::KItemScene));
}

namespace UI
{

DWORD KItemScene::ms_dwItemType = ITEM_SCENE;


KItemScene::KItemScene()
{
	m_Scene.p3DScene = NULL;
	m_Scene.dwFlags = KG3DUI_SCENE_FLAGS_DISABLE_TERRAIN | 
		KG3DUI_SCENE_FLAGS_DISABLE_SKYBOX | KG3DUI_SCENE_FLAGS_ENABLE_ALPHA;
}

KItemScene::~KItemScene()
{

}

void* KItemScene::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KItemScene::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KItemScene::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KItemScene::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KItemScene::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KItemNull *pItem = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pItem = new(std::nothrow) KItemScene;
    KGLOG_PROCESS_ERROR(pItem);

    if (pLua)
    {
        nRetCode = pItem->Init(pLua, nTableIndex);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    *ppObj = pItem;
    // Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_RELEASE(pItem);
    }
    return nResult;
}
#endif

KItemScene *KItemScene::Create(KItemSceneData &ItemSceneData)
{
    KItemScene *pItemScene = NULL;
    int nRetCode = false;

    pItemScene = new(std::nothrow) KItemScene;
    KG_PROCESS_ERROR(pItemScene);

    nRetCode = pItemScene->Init(ItemSceneData);
	KG_PROCESS_ERROR(nRetCode);
	pItemScene->OnInitEnd(ItemSceneData);
Exit0:
    if (!nRetCode)
        SAFE_RELEASE(pItemScene);
    return pItemScene;
}

void KItemScene::Release()
{
	KItemNull::Release();
}

#ifdef KGUI_LUA_LOADER
int KItemScene::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    int nRenderTerrain = 0;
    int nRenderSky     = 0;
    int nEnableAlpha   = 0;

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("RenderTerrain", emVAR_TYPE_INT, false, &nRenderTerrain, 0),
        KLUA_TABLE_FIELD("RenderSky",     emVAR_TYPE_INT, false, &nRenderSky,     0),
        KLUA_TABLE_FIELD("EnableAlpha",   emVAR_TYPE_INT, false, &nEnableAlpha,   0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KItemNull::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    g_SetBitFlag(nRenderTerrain, KG3DUI_SCENE_FLAGS_DISABLE_TERRAIN, m_Scene.dwFlags);
    g_SetBitFlag(nRenderSky, KG3DUI_SCENE_FLAGS_DISABLE_SKYBOX, m_Scene.dwFlags);
    g_SetBitFlag(nEnableAlpha, KG3DUI_SCENE_FLAGS_ENABLE_ALPHA, m_Scene.dwFlags);

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

int KItemScene::Init(KItemSceneData &ItemSceneData)
{
    int nResult = false;
    int nRetCode = false;

    nRetCode = KItemNull::Init(ItemSceneData);
    KG_PROCESS_ERROR(nRetCode);

	EnableFrameMove(ItemSceneData.nEnableFrameMove);
	EnableRenderTerrain(ItemSceneData.nEnableRenderTerrain);
	EnableRenderSkyBox(ItemSceneData.nEnableRenderSkyBox);
	EnableAlpha(ItemSceneData.nEnableAlpha);
    
    nResult = true;
Exit0:
    return nResult;
}

void KItemScene::Draw()
{
    if(!(m_dwStyle & ITEM_SHOW))
		return;
#ifndef DISABLE_UI_EDIT
    KItemNull::Draw();
#endif //_DEBUG
	if(!(m_dwStyle & ITEM_DATA_FORMATED))
	{
		m_Scene.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		m_Scene.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		m_dwStyle |= ITEM_DATA_FORMATED;
	}

	if (m_Scene.p3DScene)
    {
		if (m_Scene.dwFlags & KG3DUI_SCENE_FLAGS_ENABLE_FRAMEMOVE)
            m_Scene.p3DScene->FrameMove();
		g_pUI->m_p3DUI->DrawMultiScene(&m_Scene, 1);
    }
}

DWORD KItemScene::GetItemType()
{
    return ms_dwItemType;
}

int KItemScene::IsItemCanChangeTo(DWORD dwType)
{
    if (
        dwType == ITEM_SCENE ||
        dwType == ITEM_NULL
    )
        return true;
    return false;
}

void KItemScene::SetScene(IKG3DScene* p3DScene)
{
	m_Scene.p3DScene = p3DScene;
}

void KItemScene::EnableFrameMove(int nEnable)
{
	if (nEnable)
		m_Scene.dwFlags |= KG3DUI_SCENE_FLAGS_ENABLE_FRAMEMOVE;
	else
		m_Scene.dwFlags &= ~KG3DUI_SCENE_FLAGS_ENABLE_FRAMEMOVE;
}

void KItemScene::EnableRenderTerrain(int nEnable)
{
	if (nEnable)
		m_Scene.dwFlags &= ~KG3DUI_SCENE_FLAGS_DISABLE_TERRAIN;
	else
		m_Scene.dwFlags |= KG3DUI_SCENE_FLAGS_DISABLE_TERRAIN;
}

void KItemScene::EnableRenderSkyBox(int nEnable)
{
	if (nEnable)
		m_Scene.dwFlags &= ~KG3DUI_SCENE_FLAGS_DISABLE_SKYBOX;
	else
		m_Scene.dwFlags |= KG3DUI_SCENE_FLAGS_DISABLE_SKYBOX;
}

void KItemScene::EnableAlpha(int nEnable)
{
	if (nEnable)
		m_Scene.dwFlags |= KG3DUI_SCENE_FLAGS_ENABLE_ALPHA;
	else
		m_Scene.dwFlags &= ~KG3DUI_SCENE_FLAGS_ENABLE_ALPHA;
}

}