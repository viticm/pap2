////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemshadow.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-15 15:57:22
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "./itemshadow.h"
#include "./itemeventmgr.h"
#include "./decoder.h"
#include "../../common/ksmallobjectpool.h"
#include "../../script/kscriptmgr.h"
#include "../../KGUI.h"

////////////////////////////////////////////////////////////////////////////////
namespace
{
	KSmallObjectPool	g_Allocator("KItemShadow", sizeof(UI::KItemShadow));
}

namespace UI
{

DWORD KItemShadow::ms_dwItemType = ITEM_SHADOW;

KItemShadow::KItemShadow()
{
}

KItemShadow::~KItemShadow()
{
}

void* KItemShadow::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KItemShadow::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KItemShadow::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KItemShadow::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KItemShadow::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KItemNull *pItem = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pItem = new(std::nothrow) KItemShadow;
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

KItemShadow *KItemShadow::Create(KItemShadowData &ItemShadowData)
{
    KItemShadow *pItemShadow = NULL;
    int nRetCode = false;

    pItemShadow = new(std::nothrow) KItemShadow;
    KG_PROCESS_ERROR(pItemShadow);

    nRetCode = pItemShadow->Init(ItemShadowData);
	KG_PROCESS_ERROR(nRetCode);
	pItemShadow->OnInitEnd(ItemShadowData);
Exit0:
    if (!nRetCode)
        SAFE_RELEASE(pItemShadow);
    return pItemShadow;
}

void KItemShadow::Release()
{
	KItemNull::Release();
}

#ifdef KGUI_LUA_LOADER
int KItemShadow::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    char  szColor[KD_SMALL_BUFFER_SIZE];
    int   nAlpha  = 0;
    DWORD dwColor = 0;

    KColorSchemeMgr& ColorSchemeMgr = g_pUI->m_ColorSchemeMgr;

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("ShadowColor", emVAR_TYPE_STRING, "",  szColor, _countof(szColor)),
        KLUA_TABLE_FIELD("alpha",       emVAR_TYPE_INT,    128, &nAlpha, 0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KItemNull::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    if (szColor[0])
    {
        int nColorScheme = ColorSchemeMgr.FindScheme(ATL::CA2W(szColor, GetCodePage()));
        if (ColorSchemeMgr.IsExistColor(nColorScheme))
            dwColor = ColorSchemeMgr.GetColor(nColorScheme);
    }
    m_ShadowRect.crDiffuse = AddAlpha(dwColor, nAlpha);

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

int KItemShadow::Init(KItemShadowData &ItemShadowData)
{
    int nResult = false;
    int nRetCode = false;

    nRetCode = KItemNull::Init(ItemShadowData);
    KG_PROCESS_ERROR(nRetCode);
    
    m_ShadowRect.crDiffuse = ItemShadowData.dwShadowColor;

    nResult = true;
Exit0:
    return nResult;
}

void KItemShadow::Draw()
{
    if(!(m_dwStyle & ITEM_SHOW))
		return;
#ifndef DISABLE_UI_EDIT
	KItemNull::Draw();
#endif

	if (m_dwStyle & SHADOW_TRIANGLE_FAN && m_OrgVertex.size() > 0)
	{
		VertexArray aVertex;
		aVertex.reserve(m_OrgVertex.size());
		for (VertexArray::iterator it = m_OrgVertex.begin(); it != m_OrgVertex.end(); ++it)
		{
			KG3DUIVertex2 Vertex = *it;
			Vertex.v2Vertex.x += m_fAbsX;
			Vertex.v2Vertex.y += m_fAbsY;
			aVertex.push_back(Vertex);
		}
		g_pUI->m_p3DUI->FillTriangleFan(&(aVertex[0]), aVertex.size());
		return;
	}

    m_ShadowRect.v2Min.x = m_fAbsX;
    m_ShadowRect.v2Min.y = m_fAbsY;
    m_ShadowRect.v2Max.x = m_fAbsX + m_fWidth;
    m_ShadowRect.v2Max.y = m_fAbsY + m_fHeight;

    g_pUI->m_p3DUI->FillMultiRect(&m_ShadowRect, 1);
}

DWORD KItemShadow::GetItemType()
{
    return ms_dwItemType;
}

int KItemShadow::IsItemCanChangeTo(DWORD dwType)
{
    if (
        dwType == ITEM_SHADOW || 
        dwType == ITEM_NULL
    )
        return true;
    return false;
}

int KItemShadow::SetShadowColor(DWORD dwColor)
{
	m_ShadowRect.crDiffuse = dwColor;
	return true;
}

int KItemShadow::Scale(float fScaleX, float fScaleY)
{
	KItemNull::Scale(fScaleX, fScaleY);
	if (m_dwStyle & ITEM_NEVER_SCALED)
		return true;
	for (VertexArray::iterator it = m_OrgVertex.begin(); it != m_OrgVertex.end(); ++it)
	{
		it->v2Vertex.x *= fScaleX;
		it->v2Vertex.y *= fScaleY;
	}
	return true;
}

DWORD KItemShadow::GetshadowColor()
{
	return m_ShadowRect.crDiffuse;
}

int KItemShadow::SetAlpha(int nAlpha)
{
	KItemNull::SetAlpha(nAlpha);

	m_ShadowRect.crDiffuse = AddAlpha(m_ShadowRect.crDiffuse, m_dwAlpha);
	return true;
}

int KItemShadow::SetTriangleFan(int nTriangleFan)
{
	m_dwStyle &= ~SHADOW_TRIANGLE_FAN;
	if (nTriangleFan)
		m_dwStyle |= SHADOW_TRIANGLE_FAN;
	return true;
}

int KItemShadow::IsTriangleFan()
{
	if (m_dwStyle & SHADOW_TRIANGLE_FAN)
		return true;
	return false;
}

int KItemShadow::AppendTriangleFanPoint(float fX, float fY, DWORD dwClolor)
{
	KG3DUIVertex2 Vertex;
	Vertex.v2Vertex.x = fX;
	Vertex.v2Vertex.y = fY;
	Vertex.crDiffuse = dwClolor;
	m_OrgVertex.push_back(Vertex);
	return true;
}

int KItemShadow::ClearTriangleFanPoint()
{
	m_OrgVertex.clear();
	return true;
}



} //namespace UI
