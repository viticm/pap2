////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemanimate.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-9 17:12:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./itemanimate.h"
#include "./itemeventmgr.h"
#include "./decoder.h"
#include "../../common/ksmallobjectpool.h"
#include "../../script/kscriptmgr.h"
#include "../../KGUI.h"


////////////////////////////////////////////////////////////////////////////////
namespace
{
	KSmallObjectPool	g_Allocator("KItemAnimate", sizeof(UI::KItemAnimate));
}


namespace UI
{

DWORD KItemAnimate::ms_dwItemType = ITEM_ANIAMTE;

KItemAnimate::KItemAnimate()
{
    m_nAnimateIndex = -1;
}

KItemAnimate::~KItemAnimate()
{
}

void* KItemAnimate::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KItemAnimate::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KItemAnimate::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KItemAnimate::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KItemAnimate::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KItemNull *pItem = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pItem = new(std::nothrow) KItemAnimate;
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

KItemAnimate *KItemAnimate::Create(KItemAnimateData &ItemAnimateData)
{
    KItemAnimate *pItemAnimate = NULL;
    int nRetCode = false;

    pItemAnimate = new(std::nothrow) KItemAnimate;
    KG_PROCESS_ERROR(pItemAnimate);

    nRetCode = pItemAnimate->Init(ItemAnimateData);
	KG_PROCESS_ERROR(nRetCode);

	pItemAnimate->OnInitEnd(ItemAnimateData);

Exit0:
    if (!nRetCode)
        SAFE_RELEASE(pItemAnimate);
    return pItemAnimate;    
}

void KItemAnimate::Release()
{
	if (m_nAnimateIndex >= 0)
		g_pUI->m_AnimateImageMgr.UnloadFile(m_nAnimateIndex);
	KItemNull::Release();
}

#ifdef KGUI_LUA_LOADER
int KItemAnimate::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    char szImagePath[MAX_PATH];
    int  nGroup     = 0;
    int  nLoopCount = 0;

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("image",     emVAR_TYPE_STRING, "", szImagePath, _countof(szImagePath)),
        KLUA_TABLE_FIELD("group",     emVAR_TYPE_INT,    0,  &nGroup,     0),
        KLUA_TABLE_FIELD("LoopCount", emVAR_TYPE_INT,    0,  &nLoopCount, 0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KItemNull::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (szImagePath[0])
    {
        FormatFilePath(szImagePath);
        nRetCode = SetAnimate(szImagePath, nGroup, nLoopCount);
        KG_PROCESS_ERROR(nRetCode);
    }

    if (m_fWidth == 0 && m_fHeight == 0)
    {
        AutoSize();
    }
    
// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

int KItemAnimate::Init(KItemAnimateData &ItemAnimateData)
{
    int nResult = false;
    int nRetCode = false;

    nRetCode = KItemNull::Init(ItemAnimateData);
    KG_PROCESS_ERROR(nRetCode);

    SetAnimate(ItemAnimateData.szImagePath, ItemAnimateData.nGroup, ItemAnimateData.nLoopCount);

    if (ItemAnimateData.nAutoSize)
        AutoSize();

    nResult = true;
Exit0:
    return nResult;
}


int KItemAnimate::SetAnimate(LPCSTR szImageName, int nAnimateGroup, int nLoopCount)
{
    int nResult = false;

    KG_PROCESS_ERROR(szImageName && szImageName[0] != '\0');

    if (m_nAnimateIndex >= 0)
        g_pUI->m_AnimateImageMgr.UnloadFile(m_nAnimateIndex);

	m_nAnimateIndex = g_pUI->m_AnimateImageMgr.LoadFile(szImageName);
	KG_PROCESS_ERROR(m_nAnimateIndex >= 0);

    g_pUI->m_AnimateImageMgr.SetGroup(m_nAnimateIndex, nAnimateGroup);
    g_pUI->m_AnimateImageMgr.SetFrame(m_nAnimateIndex, 0);
    g_pUI->m_AnimateImageMgr.SetLoopCount(m_nAnimateIndex, nLoopCount);

    nResult = true;
Exit0:
    return nResult;
}

int KItemAnimate::AutoSize()
{   
    int nResult = false;                                                                                  
    int nRetCode = false;
    KImageInfo *pImageInfo = NULL;
    SIZE size = { 0, 0 };
    int nImageIndex = 0;
    int nTextureFrame = 0;

	KG_PROCESS_ERROR(m_nAnimateIndex >= 0);

    nImageIndex = g_pUI->m_AnimateImageMgr.GetImageInfoIndex(m_nAnimateIndex);
    KG_PROCESS_ERROR(nImageIndex >= 0);

    nTextureFrame = g_pUI->m_AnimateImageMgr.GetFrameIndex(m_nAnimateIndex);
    KG_PROCESS_ERROR(nTextureFrame >= 0);

	pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(nImageIndex);
    KG_PROCESS_ERROR(pImageInfo);

	size = pImageInfo->GetFrameSize(nTextureFrame);
    SetSize((float)size.cx, (float)size.cy);

	if (!(m_dwStyle & ITEM_NEVER_SCALED))
	{
		float fScale = g_pUI->m_WndStation.GetUIScale();
		m_fWidth *= fScale;
		m_fHeight *= fScale;
	}

    nResult = true;
Exit0:
    return nResult;
}

int KItemAnimate::SetImagePath(LPCSTR szImagePath)
{
    int nResult = false;

    KG_PROCESS_ERROR(szImagePath && szImagePath[0] != '\0');

    if (m_nAnimateIndex >= 0)
        g_pUI->m_AnimateImageMgr.UnloadFile(m_nAnimateIndex);

    m_nAnimateIndex = g_pUI->m_AnimateImageMgr.LoadFile(szImagePath);
	KG_PROCESS_ERROR(m_nAnimateIndex >= 0);

    nResult = true;
Exit0:
    return nResult;
}

int KItemAnimate::SetGroup(int nGroup)
{
    g_pUI->m_AnimateImageMgr.SetGroup(m_nAnimateIndex, nGroup);

    return true;
}

int KItemAnimate::SetLoopCount(int nLoopCount)
{
    g_pUI->m_AnimateImageMgr.SetLoopCount(m_nAnimateIndex, nLoopCount);
    return true;
}

void KItemAnimate::Replay()
{
    if (m_nAnimateIndex >= 0)
        g_pUI->m_AnimateImageMgr.SetFrame(m_nAnimateIndex, 0);
}

void KItemAnimate::Draw()
{
    if (!(m_dwStyle & ITEM_SHOW))
		return;

#ifndef DISABLE_UI_EDIT
    KItemNull::Draw();
#endif //_DEBUG

    g_pUI->m_AnimateImageMgr.Draw(m_nAnimateIndex, m_fAbsX, m_fAbsY, m_fWidth, m_fHeight, m_dwAlpha);
}

DWORD KItemAnimate::GetItemType()
{
    return ms_dwItemType;
}

int KItemAnimate::IsItemCanChangeTo(DWORD dwType)
{
    if (
        dwType == ITEM_ANIAMTE || 
        dwType == ITEM_NULL
    )
        return true;
    return false;
}

} //namespace UI