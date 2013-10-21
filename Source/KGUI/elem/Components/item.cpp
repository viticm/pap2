////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : item.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-9 16:48:47
//  Comment     : 
//                Edited by Chen Jie 2006-12-30
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./item.h"
#include "./itemhandle.h"
#include "./itemeventmgr.h"
#include "./decoder.h"
#include "../wndwindow.h"
#include "../tipcenter.h"
#include "../../config/kdebugconfig.h"
#include "../../common/ksmallobjectpool.h"
#include "../../driver/kareatest.h"
#include "../../script/kscriptmgr.h"
#include "../../KGUI.h"

#ifndef DISABLE_UI_EDIT
#include "./itemeditmodemgr.h"
#endif


////////////////////////////////////////////////////////////////////////////////

namespace
{
	KSmallObjectPool	g_Allocator("KItemNull", sizeof(UI::KItemNull));
}


namespace UI
{

static LPCSTR s_pcszWndItemTypeString[ITEM_COUNT] = 
{
    "Null",
    "Text",
    "Image",
    "Shadow",
    "Animate",
    "Scene",
	"Box",
    "Handle",
    "TreeLeaf",
};

int g_GetWndItemType(LPCSTR pcszTypeName)
{
	if (!pcszTypeName)
		return ITEM_UNKNOWN;
    for (int i = ITEM_BEGINE; i < ITEM_COUNT; ++i) 
    {
        if (stricmp(s_pcszWndItemTypeString[i], pcszTypeName) == 0)
        {
            return i;
        }
    }

	return ITEM_UNKNOWN;
}

LPCSTR g_GetWndItemName(int nItemType)
{
    LPCSTR pcszResult = NULL;
    KG_PROCESS_ERROR(nItemType >= ITEM_BEGINE);
    KG_PROCESS_ERROR(nItemType < ITEM_COUNT);
    
    pcszResult = s_pcszWndItemTypeString[nItemType];
Exit0:
    return pcszResult;
}

////////////////////////////////////////////////////////////////////////////////

DWORD KItemNull::ms_dwItemType = ITEM_NULL;

KItemNull::KItemNull()
{
    m_dwPosType     = POSITION_BY_CUSTOM;
    m_fRelX         = 0.0f;
    m_fRelY         = 0.0f;
    m_fAbsX         = 0.0f;
    m_fAbsY         = 0.0f;
    m_fWidth        = 0.0f;
    m_fHeight       = 0.0f;
    m_dwStyle       = ITEM_SHOW;
	m_fUserData		= 0;
	m_nTipIndex     = -1;
	m_nAreaIndex	= -1;
	m_pParent		= NULL;
	m_nLuaRef		= LUA_NOREF;
    m_nLevel        = 0;
	m_dwEvent		= 0;
	m_dwAlpha		= 255;

    m_szItemName[0] = '\0';
}

KItemNull::~KItemNull()
{
}

void* KItemNull::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KItemNull::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KItemNull::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KItemNull::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KItemNull::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KItemNull *pItem = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pItem = new(std::nothrow) KItemNull;
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
        KG_DELETE(pItem);
    }
    return nResult;
}
#endif

KItemNull *KItemNull::Create(KItemNullData &ItemNullData)
{
    KItemNull *pItemNull = NULL;
    int nRetCode = false;

    pItemNull = new(std::nothrow) KItemNull;
    KG_PROCESS_ERROR(pItemNull);

    nRetCode = pItemNull->Init(ItemNullData);
	KG_PROCESS_ERROR(nRetCode);
	pItemNull->OnInitEnd(ItemNullData);
Exit0:
    if (!nRetCode)
        SAFE_RELEASE(pItemNull);
    return pItemNull;
}

void KItemNull::Release()
{
	if (m_dwEvent)
	{
		KWndWindow *pOWner = GetWndOwner();
		if (pOWner)
		{
			pOWner->OnItemRelease(this);
			pOWner->UpdateEvent();
		}
	}
	if (IsVisible())
		NotifyDraw();

	g_pUI->m_WndStation.SetBlurItem(this);
	if (m_nTipIndex >=0)
		KTipCenter::GetSelf().Remove(m_nTipIndex);

	g_pUI->m_Script.UnRefUIObject(m_nLuaRef);
	m_nLuaRef = LUA_NOREF;

#ifndef DISABLE_UI_EDIT
	if (KItemEditModeMgr::GetSelf().GetCurrentEditItem() == this)
		KItemEditModeMgr::GetSelf().ClearCurrentEditItem();
#endif

    delete this;
}

#ifdef KGUI_LUA_LOADER
int KItemNull::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    char  szTip[KD_NORMAL_BUFFER_SIZE];
    char  szFilePath[MAX_PATH];
    int   nDisableScaled     = false;
    int   nShowTipType       = 0;
    int   nTipRichText       = 0;
    int   nLockShowAndHide   = false;
    DWORD dwEventID          = 0;
    int   nAutoSize          = false;

    static char szDecodedText[ITEM_MAX_DECODE_TEXT_LEN];

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("name",         emVAR_TYPE_STRING, "",    &m_szItemName,     _countof(m_szItemName)),
        KLUA_TABLE_FIELD("PosType",      emVAR_TYPE_INT,    0,     &m_dwPosType,      0),
        KLUA_TABLE_FIELD("left",         emVAR_TYPE_FLOAT,  0,     &m_fRelX,          0),
        KLUA_TABLE_FIELD("top",          emVAR_TYPE_FLOAT,  0,     &m_fRelY,          0),
        KLUA_TABLE_FIELD("width",        emVAR_TYPE_FLOAT,  0,     &m_fWidth,         0),
        KLUA_TABLE_FIELD("height",       emVAR_TYPE_FLOAT,  0,     &m_fHeight,        0),
        KLUA_TABLE_FIELD("AutoSize",     emVAR_TYPE_FLOAT,  false, &nAutoSize,        0),
        KLUA_TABLE_FIELD("alpha",        emVAR_TYPE_INT,    255,   &m_dwAlpha,        0),
        KLUA_TABLE_FIELD("tip",          emVAR_TYPE_STRING, "",    &szTip,            _countof(szTip)),
        KLUA_TABLE_FIELD("ShowTipType",  emVAR_TYPE_INT,    0,     &nShowTipType,     0),
        KLUA_TABLE_FIELD("TipRichText",  emVAR_TYPE_INT,    0,     &nTipRichText,     0),
        KLUA_TABLE_FIELD("DisableScale", emVAR_TYPE_INT,    false, &nDisableScaled,   0),
        KLUA_TABLE_FIELD("lock",         emVAR_TYPE_INT,    false, &nLockShowAndHide, 0),
        KLUA_TABLE_FIELD("script",       emVAR_TYPE_STRING, "",    szDecodedText,     _countof(szDecodedText)),
        KLUA_TABLE_FIELD("event",        emVAR_TYPE_INT,    0,     &m_dwEvent,        0),
        KLUA_TABLE_FIELD("AreaFile",     emVAR_TYPE_STRING, "",    szFilePath,        _countof(szFilePath)),
    };                                                                                                  
                                                                                                        
    KGLOG_PROCESS_ERROR(pLua);                                                                          
                                                                                                        
    for (int i = 0; i < _countof(aParamTable); i++)                                                     
    {                                                                                                   
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);                              
        KGLOG_PROCESS_ERROR(nRetCode);                                                                  
    }           

    m_emObjType = em_OBJ_TYPE_ITEM;      // 指定类型
                                                                                                        
    if (szTip[0])                                                                                       
    {                                                                                                   
        int nTipIndex = g_pUI->m_GlobalStrValuableMgr.GetIndexAt(szTip);                    
        m_nTipIndex = KTipCenter::GetSelf().Append(nTipIndex, nShowTipType, nTipRichText);
    }

    g_SetBitFlag(nDisableScaled, ITEM_NEVER_SCALED, m_dwStyle);
    g_SetBitFlag(nLockShowAndHide, LOCK_SHOW_AND_HIDE, m_dwStyle);
    if (nLockShowAndHide)
        Hide();

    // TODO:tongxuehu m_fUserData ?

//     if (szDecodedText[0])
//     {
//         nRetCode = KUiComponentsDecoder::GetSelf().SetDecodedScript(ATL::CA2W(szDecodedText, KGUI_CODE_PAGE));
//         KGLOG_PROCESS_ERROR(nRetCode);
//     }

// TODO:tongxuehu
//     if (ItemNullData.pEventOwner)    
//     {
//         ItemNullData.pEventOwner->UpdateDrawList();
//         if(ItemNullData.dwEventID)
//         {
//             m_dwEvent = ItemNullData.dwEventID;
//         }
//     }

    if (szFilePath[0])
    {
        FormatFilePath(szFilePath);
        m_nAreaIndex = g_AreaTester.GetID(szFilePath);
    }

    g_SetBitFlag(false, ITEM_DATA_FORMATED, m_dwStyle);

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

int KItemNull::Init(KItemNullData & ItemNullData)
{
    if (ItemNullData.wszName[0] != L'\0')
        SetName(ATL::CW2A(ItemNullData.wszName, GetCodePage()));
    
    m_nLink = ItemNullData.nLink;
    if (m_nLink)
        SetLinkInfo(ATL::CW2A(ItemNullData.wszLinkInfo, GetCodePage()));

    m_dwPosType = static_cast<DWORD>(ItemNullData.nPosType);
	SetRelPos(ItemNullData.fRelX, ItemNullData.fRelY);
	SetSize(ItemNullData.fWidth, ItemNullData.fHeight);
	SetAlpha(ItemNullData.nAlpha);
	m_nTipIndex = ItemNullData.nTipIndex;

	if (ItemNullData.nDisableScale)
	{
		EnableScale(false);
		m_fRelX *= ItemNullData.fScale;
		m_fRelY *= ItemNullData.fScale;
	}
	else
	{
		m_fRelX *= ItemNullData.fScale;
		m_fRelY *= ItemNullData.fScale;
		m_fWidth *= ItemNullData.fScale;
		m_fHeight *= ItemNullData.fScale;
	}
	if (ItemNullData.nLockShowAndHide)
	{
		Hide();
		LockShowAndHide(true);
	}

	m_fUserData = ItemNullData.fUserData;

    if (ItemNullData.pEventOwner)
	{
		ItemNullData.pEventOwner->UpdateDrawList();
		if(ItemNullData.dwEventID)
		{
			m_dwEvent = ItemNullData.dwEventID;
			ItemNullData.pEventOwner->UpdateEvent();
		}
	}

	m_nAreaIndex = ItemNullData.nAreaTestIndex;

	if (ItemNullData.nIntPos)
		SetIntPos(true);

    return true;
}

void KItemNull::OnInitEnd(KItemNullData &ItemNullData)
{
	if (ItemNullData.nScript)
	{
		g_pUI->m_Script.OnItemInit(this, ATL::CW2A(KUiComponentsDecoder::GetSelf().GetDecodedScript(), GetCodePage()));
	}
}

#ifndef DISABLE_UI_EDIT
LRESULT KItemNull::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    float const fX = (float)(GET_X_LPARAM(lParam));
    float const fY = (float)(GET_Y_LPARAM(lParam));
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
        if (PtInItem(fX, fY))
        {
            if (KDebugConfig::ms_aBoolFlag[KDebugConfig::BOOL_SELECT_WINDOW_IN_CLIENT])
				g_pUI->m_EditorMgr.UIEditorOperation(PLUGIN_UI_EDITOR_SEL_WND, (WPARAM)m_szItemName, 0);

            if (KDebugConfig::ms_aBoolFlag[KDebugConfig::BOOL_RESIZE_WND_AND_ITEM_MODE] &&
                (fX > m_fAbsX + m_fWidth - 2 * KDebugConfig::ms_nResizeWindowHandleNodeHalfSize) &&
                (fY > m_fAbsY + m_fHeight - 2 * KDebugConfig::ms_nResizeWindowHandleNodeHalfSize))
            {
                KItemEditModeMgr::GetSelf().SetEditMode(EDIT_RESIZE_MODE);
            }
            else
            {
                KItemEditModeMgr::GetSelf().SetEditMode(EDIT_DRAG_MODE);
            }
            KItemEditModeMgr::GetSelf().SetCurrentEditItem(this);
            KItemEditModeMgr::GetSelf().SetDragStartPos(fX, fY);
            return TRUE;
        }
        break;
    case WM_MOUSEMOVE:
        if (wParam & MK_LBUTTON)
        {
            float fLastX;
            float fLastY;
            KItemEditModeMgr::GetSelf().GetFragStartPos(fLastX, fLastY);
            if (KItemEditModeMgr::GetSelf().GetEditMode() == EDIT_DRAG_MODE)
                AbsMove(fX - fLastX, fY - fLastY);
            else if (KItemEditModeMgr::GetSelf().GetEditMode() == EDIT_RESIZE_MODE)
            {
                float fWidth = m_fWidth + (fX - fLastX);
                float fHeight = m_fHeight + (fY - fLastY);
                if (fWidth < 0.0f)
                    fWidth = 0.0f;
                if (fHeight < 0.0f)
                    fHeight = 0.0f;
                SetSize(fWidth, fHeight);
            }
            KItemEditModeMgr::GetSelf().SetDragStartPos(fX, fY);
            return TRUE;
        }
		else if (PtInItem(fX, fY))
		{
			static char szInfo[MAX_PATH] = { 0 };
			_snprintf(szInfo, _countof(szInfo), "名字: %s\n   x=%d, y=%d, w=%d, h=%d\n", m_szItemName, 
				(int)m_fRelX, (int)m_fRelY, (int)m_fWidth, (int)m_fHeight);
			g_pUI->m_EditorMgr.UIEditorOperation(PLUGIN_UI_EDITOR_SHOW_INFO, (WPARAM)szInfo, 0);
			return TRUE;
		}
        break;
    }
    
    return FALSE;
}
#endif

void KItemNull::Draw()
{
#ifndef DISABLE_UI_EDIT
	if (KDebugConfig::ms_aBoolFlag[UI::KDebugConfig::BOOL_SHOW_FRAME])
	{
		KG3DUIRect2 rect;
		rect.v2Min.x = m_fAbsX;
		rect.v2Min.y = m_fAbsY;
		rect.v2Max.x = m_fAbsX + m_fWidth;
		rect.v2Max.y = m_fAbsY + m_fHeight;
		rect.crDiffuse = 0xFFFFFFFF;
		if (KItemEditModeMgr::GetSelf().GetCurrentEditItem() == this)
			rect.crDiffuse = 0xFFFFFF00;    
		g_pUI->m_p3DUI->DrawMultiRect(&rect, 1);

		if (KDebugConfig::ms_aBoolFlag[UI::KDebugConfig::BOOL_RESIZE_WND_AND_ITEM_MODE])
		{
			rect.v2Min.x = m_fAbsX + m_fWidth - 2 * KDebugConfig::ms_nResizeWindowHandleNodeHalfSize;
			rect.v2Min.y = m_fAbsY + m_fHeight -2 * KDebugConfig::ms_nResizeWindowHandleNodeHalfSize;
			rect.v2Max.x = m_fAbsX + m_fWidth;
			rect.v2Max.y = m_fAbsY + m_fHeight;
			rect.crDiffuse = 0xFFFF0000;    
			g_pUI->m_p3DUI->DrawMultiRect(&rect, 1);
		}
		return;
	}
#endif
}

DWORD KItemNull::GetItemType()
{
    return ms_dwItemType;
}

int KItemNull::IsItemCanChangeTo(DWORD dwType)
{
    if (dwType == ITEM_NULL)
        return true;
    return false;
}

int KItemNull::PtInItem(float fAbsX, float fAbsY)
{
#ifndef DISABLE_UI_EDIT
    if (!KDebugConfig::ms_aBoolFlag[KDebugConfig::BOOL_EDIT_MODE])
        if (!IsVisibleInWnd())
            return false;
#else
    if (!IsVisibleInWnd())
        return false;
#endif

	if (m_nAreaIndex != -1)
	{
		if (UI_FLOAT_LARGE(m_fWidth, 0.0f) && UI_FLOAT_LARGE(m_fHeight, 0.0f))
			return g_AreaTester.PtInArea(m_nAreaIndex, (fAbsX - m_fAbsX) / m_fWidth, (fAbsY - m_fAbsY) / m_fHeight);
		return false;
	}

    if (fAbsX >= m_fAbsX && fAbsX <= m_fAbsX + m_fWidth && fAbsY >= m_fAbsY && fAbsY <= m_fAbsY +m_fHeight)
    {
		if (m_pParent)
			return m_pParent->IsInStencialArea(fAbsX, fAbsY);
		return TRUE;
    }
	return FALSE;
}

int KItemNull::SetRelPos(float fX, float fY)
{
    m_fRelX = fX;
    m_fRelY = fY;
	m_dwStyle &= ~ITEM_DATA_FORMATED;
	return true;
}

int KItemNull::GetRelPos(float &fX, float &fY)
{
    fX = m_fRelX;
    fY = m_fRelY;
    return true;
}

int KItemNull::SetAbsPos(float fX, float fY)
{
    m_fAbsX = fX;
    m_fAbsY = fY;

	if (IsIntPos())
	{
		m_fAbsX = floor(m_fAbsX);
		m_fAbsY = floor(m_fAbsY);
	}

	m_dwStyle &= ~ITEM_DATA_FORMATED;
    return true;
}

int KItemNull::GetAbsPos(float &fX, float &fY)
{
    fX = m_fAbsX;
    fY = m_fAbsY;
    return true;
}

int KItemNull::AbsMove(float fDeltaX, float fDeltaY)
{
    m_fAbsX += fDeltaX;
    m_fAbsY += fDeltaY;

	if (IsIntPos())
	{
		m_fAbsX = floor(m_fAbsX);
		m_fAbsY = floor(m_fAbsY);
	}

	m_dwStyle &= ~ITEM_DATA_FORMATED;

    return true;
}

int KItemNull::Scale(float fScaleX, float fScaleY)
{
	ASSERT(fScaleX > 0.0f && fScaleY > 0.0f);
	SetRelPos(fScaleX * m_fRelX, fScaleY * m_fRelY);
	if (m_dwStyle & ITEM_NEVER_SCALED)
		return true;
    SetSize(fScaleX * m_fWidth, fScaleY * m_fHeight);
    
    return true;
}

int KItemNull::SetSize(float fWidth, float fHeight)
{
	if (fWidth < 0.0f)
		fWidth = 0.0f;
	if (fHeight < 0.0f)
		fHeight = 0.0f;
    m_fWidth    = fWidth;
    m_fHeight   = fHeight;

	if (IsIntPos())
	{
		m_fWidth = floor(m_fWidth);
		m_fHeight = floor(m_fHeight);
	}

	m_dwStyle &= ~ITEM_DATA_FORMATED;

    return true;
}

int KItemNull::GetSize(float &fWidth, float &fHeight)
{
    fWidth  = m_fWidth;
    fHeight = m_fHeight;

    return true;
}

int KItemNull::SetPosType(DWORD dwPosType)
{
    int nResult = false;

    KG_PROCESS_ERROR(dwPosType >= POSITION_BEGINE);
    KG_PROCESS_ERROR(dwPosType < POSITION_END);

    m_dwPosType = dwPosType;

    nResult = true;
Exit0:
    return nResult;
}

int KItemNull::GetPosType(DWORD &dwPosType)
{
    dwPosType = m_dwPosType;

    return true;
}

int KItemNull::SetTipIndex(int nIndex)
{
	m_nTipIndex = nIndex;
	return true;
}

int KItemNull::GetTipIndex()
{
	return m_nTipIndex;
}

int KItemNull::ShowTip()
{
	if (m_nTipIndex >= 0)
	{
		KTipCenter::GetSelf().ShowTip(m_nTipIndex, m_fAbsX, m_fAbsY, m_fWidth, m_fHeight);
	}
	return true;
}

int KItemNull::SetName(LPCSTR szItemName)
{
    int nResult = false;

    KG_PROCESS_ERROR(szItemName);

    strncpy(m_szItemName, szItemName, _countof(m_szItemName));
    m_szItemName[_countof(m_szItemName) - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

LPCSTR KItemNull::GetName() const
{
    return m_szItemName;
}

int KItemNull::Hide()
{
	if (m_dwStyle & LOCK_SHOW_AND_HIDE)
		return true;

	if (m_dwStyle & ITEM_SHOW)
		NotifyDraw();

    m_dwStyle &= ~ITEM_SHOW;
    return true;
}       

int KItemNull::Show()
{
	if (m_dwStyle & LOCK_SHOW_AND_HIDE)
		return true;

	if (!(m_dwStyle & ITEM_SHOW))
		NotifyDraw();

    m_dwStyle |= ITEM_SHOW;
    return true;
}

int	KItemNull::EnableScale(int nEnable)
{
	m_dwStyle |= ITEM_NEVER_SCALED;
	if (nEnable)
		m_dwStyle &= ~ITEM_NEVER_SCALED;
	return true;
}

int KItemNull::LockShowAndHide(int nEnable)
{
	m_dwStyle &= ~LOCK_SHOW_AND_HIDE;
	if (nEnable)
		m_dwStyle |= LOCK_SHOW_AND_HIDE;
	return true;
}

int KItemNull::IsShowOrHideLocked()
{
	if (m_dwStyle & LOCK_SHOW_AND_HIDE)
		return true;
	return false;
}

int KItemNull::IsVisible()
{
    if (m_dwStyle & ITEM_SHOW)
        return true;
    return false;
}

int	KItemNull::IsVisibleInWnd()
{
	if (m_dwStyle & ITEM_SHOW)
	{
		KItemNull *pParent = GetParent();
		while(pParent)
		{
			if (!pParent->IsVisible())
				return false;
			pParent = pParent->GetParent();
		}
		return true;
	}
	return false;
}

int KItemNull::SetUserData(double fUserData)
{
	m_fUserData = fUserData;
	return true;
}

double KItemNull::GetUserData()
{
	return m_fUserData;
}

int KItemNull::SetUserDataLONGLONG(long long llUserData)
{
    m_llUserData = llUserData;
    return true;
}

int KItemNull::GetUserDataLONGLONG(LONGLONG *pllRetUserData)
{
    if (!pllRetUserData)
        return false;

    *pllRetUserData = m_llUserData;
    return true;
}

int KItemNull::SetUserDataDWORD(DWORD dwUserData1, DWORD dwUserData2)
{
    int nResult = false;

    KG_ASSERT_EXIT((_countof(m_dwUserData)) >= 2);

    m_dwUserData[0] = dwUserData1;
    m_dwUserData[1] = dwUserData2;

    nResult = true;
Exit0:
    return nResult;
}

int KItemNull::GetUserDataDWORD(DWORD *pdwRetUserData1, DWORD *pdwRetUserData2)
{
    int nResult = false;

    KG_ASSERT_EXIT((_countof(m_dwUserData)) >= 2);

    if (pdwRetUserData1)
        *pdwRetUserData1 = m_dwUserData[0];

    if (pdwRetUserData2)
        *pdwRetUserData2 = m_dwUserData[1];

    nResult = true;
Exit0:
    return nResult;
}

int KItemNull::SetUserDataInt(int nUserData1, int nUserData2)
{
	int nResult = false;

	KG_ASSERT_EXIT((_countof(m_nUserData)) >= 2);

	m_nUserData[0] = nUserData1;
	m_nUserData[1] = nUserData2;

	nResult = true;
Exit0:
	return nResult;
}

int KItemNull::GetUserDataInt(int *pnRetUserData1, int *pnRetUserData2)
{
	int nResult = false;

	KG_ASSERT_EXIT((_countof(m_nUserData)) >= 2);

	if (pnRetUserData1)
		*pnRetUserData1 = m_nUserData[0];

	if (pnRetUserData2)
		*pnRetUserData2 = m_nUserData[1];

	nResult = true;
Exit0:
	return nResult;
}


int	KItemNull::SetParent(KItemHandle *pHandle)
{
	m_pParent = pHandle;
	return true;
}


int	KItemNull::SetAlpha(int nAlpha)
{
	if (nAlpha > 255)
		nAlpha = 255;
	if (nAlpha < 0)
		nAlpha = 0;
	m_dwAlpha = (DWORD)nAlpha;

	m_dwStyle &= ~ITEM_DATA_FORMATED;

	return true;
}

KWndWindow *KItemNull::GetWndOwner()
{
	KItemNull *pItem = this;
	KItemNull *pParent = GetParent();
	while (pParent)
	{
		pItem = pParent;
		pParent = pItem->GetParent();
	}
	KItemHandle *pHandle = g_ItemCast<KItemHandle>(pItem);
	if (!pHandle)
		return NULL;
	return pHandle->GetOwner();
}

inline void KItemNull::NotifyDraw()
{
	KItemNull *pItem = this;
	KItemNull *pParent = GetParent();
	while (pParent)
	{
		if (!pParent->IsVisible())
			return;
		pItem = pParent;
		pParent = pParent->GetParent();
	}
	KItemHandle *pHandle = g_ItemCast<KItemHandle>(pItem);
	if (!pHandle)
		return;
	KWndWindow *pWnd = pHandle->GetOwner();
	if (pWnd)
		pWnd->UpdateDrawList();
}

int	KItemNull::GetAlpha()
{
	return (int)m_dwAlpha;
}

void KItemNull::RegisterEvent(DWORD dwEvent)
{
	if (dwEvent != m_dwEvent)
	{
		m_dwEvent |= dwEvent;
		KWndWindow *pOwner = GetWndOwner();
		if (pOwner)
			pOwner->UpdateEvent();
	}
}

void KItemNull::ClearEvent()
{
	if (m_dwEvent)
	{
		KWndWindow *pOwner = GetWndOwner();
		if (pOwner)
			pOwner->UpdateEvent();
		m_dwEvent = 0;
	}
}

int	KItemNull::SetAreaTestFile(LPCSTR pcszFile)
{
	int nResult = false;

	KG_PROCESS_ERROR(pcszFile);

	m_nAreaIndex = g_AreaTester.GetID(pcszFile);

	nResult = true;
Exit0:
	if (!nResult)
		m_nAreaIndex = -1;
	return nResult;
}

void KItemNull::SetIntPos(int nIntPos)
{
	m_dwStyle &= ~ITEM_INT_POS;
	if (nIntPos)
	{
		m_dwStyle |= ITEM_INT_POS;
		SetAbsPos(m_fAbsX, m_fAbsY);
	}
}

int	KItemNull::IsIntPos()
{
	if (m_dwStyle & ITEM_INT_POS)
		return true;
	return false;
}

int KItemNull::UpdateLevel(int nLevel)
{
    m_nLevel = nLevel;

    return true;
}

int KItemNull::IsLink()
{
    return m_nLink;
}

int KItemNull::SetLinkInfo(const char szLinkInfo[])
{
    int nResult = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(szLinkInfo);

    strncpy(m_szLinkInfo, szLinkInfo, _countof(m_szLinkInfo));
    m_szLinkInfo[_countof(m_szLinkInfo) - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

LPCSTR KItemNull::GetLinkInfo() const
{
    return m_szLinkInfo;
}

} //namespace UI