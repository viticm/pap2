#include "stdafx.h"
#include "./wndbutton.h"
#include "./wndmessage.h"
#include "./Components/itemhandle.h"
#include "../common/ksmallobjectpool.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"

namespace
{
	KSmallObjectPool	g_Allocator("KWndButton", sizeof(UI::KWndButton));
}

namespace UI
{

#ifdef KGUI_LUA_LOADER
int KWndButton::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KWndWindow *pWnd = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pWnd = new(std::nothrow) KWndButton;
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

KWndButton::KWndButton()
: m_eCurrentGroupIndex(BUTTON_STATUS_UNKNOWN)
, m_dwStatus(WNDBUTTON_ENABLE)  
, m_nAnimateIndex(-1)
{
	for (int nIndex = BUTTON_STATUS_BEGIN; nIndex != BUTTON_STATUS_END; ++nIndex)
	{
		m_nAnimateGroup[nIndex] = 0;
		m_nOverTextFrame[nIndex] = -1;
	}
}

KWndButton::~KWndButton()
{
	if (m_nAnimateIndex != -1)
	{
		g_pUI->m_AnimateImageMgr.UnloadFile(m_nAnimateIndex);
		m_nAnimateIndex = -1;
	}
}

void* KWndButton::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KWndButton::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KWndButton::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KWndButton::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KWndButton::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    int  nDisable = false;
    char szImagePath[MAX_PATH];

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("disable",        emVAR_TYPE_INT,    false, &nDisable,                                  0),
        KLUA_TABLE_FIELD("NormalGroup",    emVAR_TYPE_INT,    0,     &m_nAnimateGroup[BUTTON_STATUS_NORMAL],     0),
        KLUA_TABLE_FIELD("MouseOverGroup", emVAR_TYPE_INT,    0,     &m_nAnimateGroup[BUTTON_STATUS_MOUSEOVER],  0),
        KLUA_TABLE_FIELD("MouseDownGroup", emVAR_TYPE_INT,    0,     &m_nAnimateGroup[BUTTON_STATUS_MOUSEDOWN],  0),
        KLUA_TABLE_FIELD("DisableGroup",   emVAR_TYPE_INT,    0,     &m_nAnimateGroup[BUTTON_STATUS_DISABLE],     0),
        KLUA_TABLE_FIELD("NormalFont",     emVAR_TYPE_INT,    0,     &m_nOverTextFrame[BUTTON_STATUS_NORMAL],    0),
        KLUA_TABLE_FIELD("MouseOverFont",  emVAR_TYPE_INT,    0,     &m_nOverTextFrame[BUTTON_STATUS_MOUSEOVER], 0),
        KLUA_TABLE_FIELD("MouseDownFont",  emVAR_TYPE_INT,    0,     &m_nOverTextFrame[BUTTON_STATUS_MOUSEDOWN], 0),
        KLUA_TABLE_FIELD("DisableFont",    emVAR_TYPE_INT,    0,     &m_nOverTextFrame[BUTTON_STATUS_DISABLE],    0),
        KLUA_TABLE_FIELD("image",          emVAR_TYPE_STRING, "",    szImagePath,                                _countof(szImagePath)),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KWndWindow::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    g_SetBitFlag(!nDisable, WNDBUTTON_ENABLE, m_dwButtonStatus);

    ASSERT(m_nAnimateIndex == -1 && "ÌùÍ¼Î´ÊÍ·Å"); 
    if (szImagePath[0])
    {
        FormatFilePath(szImagePath);
        nRetCode = g_pUI->m_AnimateImageMgr.AddFile(szImagePath, m_nAnimateIndex);
        if (!nRetCode)
        {
            KGLogPrintf(KGLOG_ERR, "KGUI KWndButton::Init()\t add file [%s] failed\n", szImagePath);
        }
    }
    
    UpdataAnimateGroup();
// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

void KWndButton::SetFont(int nNormal, int nMouseOver, int nMouseDown, int nDisable)
{
	m_nOverTextFrame[BUTTON_STATUS_NORMAL] = nNormal;
	m_nOverTextFrame[BUTTON_STATUS_MOUSEOVER] = nMouseOver;
	m_nOverTextFrame[BUTTON_STATUS_MOUSEDOWN] = nMouseDown;
	m_nOverTextFrame[BUTTON_STATUS_DISABLE] = nDisable;
}

void KWndButton::SetAnimation(int nNormal, int nMouseOver, int nMouseDown, int nDisable, LPCSTR szImagePath)
{
	m_nAnimateGroup[BUTTON_STATUS_NORMAL] = nNormal;
	m_nAnimateGroup[BUTTON_STATUS_MOUSEOVER] = nMouseOver;
	m_nAnimateGroup[BUTTON_STATUS_MOUSEDOWN] = nMouseDown;
	m_nAnimateGroup[BUTTON_STATUS_DISABLE] = nDisable;

    if (szImagePath[0] != '\0')
    {
		if (m_nAnimateIndex >= 0)
			g_pUI->m_AnimateImageMgr.UnloadFile(m_nAnimateIndex);

        m_nAnimateIndex = g_pUI->m_AnimateImageMgr.LoadFile(szImagePath);
    }

	UpdataAnimateGroup();
}


int KWndButton::PtInWindow(int x, int y)
{
    return KWndWindow::PtInWindow(x, y);
}

void KWndButton::OnPaint()
{   
	if (m_nAnimateIndex != -1)
		g_pUI->m_AnimateImageMgr.Draw(m_nAnimateIndex, m_fAbsX, m_fAbsY, m_fWidth, m_fHeight, m_dwAlpha);

	KWndWindow::OnPaint();
}

LRESULT KWndButton::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRet = FALSE;

	if (KWndWindow::WndProc(uMsg, wParam, lParam))
		return TRUE;
    switch (uMsg) 
    {
    case WM_LBUTTONDOWN:
        OnLButtonDown(lParam);
		lRet = TRUE;
        break;
    case WM_LBUTTONUP:
        OnLButtonUp(lParam);
		lRet = TRUE;
        break;
    case WM_RBUTTONDOWN:
		OnRButtonDown(lParam);
		lRet = TRUE;
		break;
    case WM_RBUTTONUP:
		OnRButtonUp(lParam);
		lRet = TRUE;
		break;
    case WM_MOUSEMOVE:
        if (IsStatus(WNDBUTTON_DRAG_WIDTH_LBUTTON) && IsStatus(WNDBUTTON_ON_DRAGING_WIDTH_LBUTTON) && (wParam & MK_LBUTTON) ||
            IsStatus(WNDBUTTON_DRAG_WIDTH_RBUTTON) && IsStatus(WNDBUTTON_ON_DRAGING_WIDTH_RBUTTON) && IsStatus(MK_RBUTTON))
        {
			if (g_pUI->m_Script.FireWndEvent(this, "OnDragButton"))
				lRet = TRUE;
        }
        break;
    case WM_LBUTTONDBLCLK:
		if (IsStatus(WNDBUTTON_ENABLE))
        {
			OnLButtonDown(lParam);
			OnLButtonUp(lParam);
			if (g_pUI->m_Script.FireWndEvent(this, "OnLButtonDBClick"))
				lRet = TRUE;
        }
        break;
	case WM_RBUTTONDBLCLK:
		if (IsStatus(WNDBUTTON_ENABLE))
		{
			OnRButtonDown(lParam);
			OnRButtonUp(lParam);
			if (g_pUI->m_Script.FireWndEvent(this, "OnRButtonDBClick"))
				lRet = TRUE;
		}
		break;
    case WND_M_MOUSE_ENTER:
        lRet = OnMouseEnter();
        break;
    case WND_M_MOUSE_LEAVE:
        lRet = OnMouseLeave();
        break;
    case WND_M_KILL_FOCUS:
		SetStatus(WNDBUTTON_ON_DRAGING_WIDTH_LBUTTON, FALSE);
		SetStatus(WNDBUTTON_ON_DRAGING_WIDTH_RBUTTON, FALSE);
        SetStatus(WNDBUTTON_PRESSED, FALSE);
        SetStatus(WNDBUTTON_MOUSE_DOWN, FALSE);
        UpdataAnimateGroup();
        lRet = TRUE;
        break;
	case WM_MOUSEHOVER:
		if (IsStatus(WNDBUTTON_PRESSED))
		{
			KWndWindow *pParent = GetParent();
			if (pParent)
				pParent->WndProc(WND_N_BUTTON_HOLD, (WPARAM)this, 0);
			g_pUI->m_Script.FireWndEvent(this, "OnLButtonHold");
		}
		break;
	case WND_M_CAPTURECHANGED:
		SetStatus(WNDBUTTON_MOUSE_DOWN, FALSE);
		UpdataAnimateGroup();
		lRet = TRUE;
		break;
    }

    return lRet;
}

int KWndButton::OnMouseLeave()
{
    int nResult = false;
    KWndWindow *pParent = NULL;

    SetStatus(WNDBUTTON_MOUSE_OVER, FALSE);

	UpdataAnimateGroup();

    pParent = GetParent();
    if (pParent)
        pParent->WndProc(WND_M_MOUSE_LEAVE, WPARAM(this), LPARAM(0));

    nResult = true;
    return nResult;
}

int KWndButton::OnMouseEnter()
{
    SetStatus(WNDBUTTON_MOUSE_OVER, TRUE);
    UpdataAnimateGroup();

    return true;
}

int KWndButton::OnLButtonDown(LPARAM lParam)
{
    int nResult = false;
    KWndWindow *pParent = NULL;   

    KG_PROCESS_ERROR(IsStatus(WNDBUTTON_ENABLE));

	g_pUI->m_WndStation.SetCapture(this); 
	if (IsStatus(WNDBUTTON_DRAG_WIDTH_LBUTTON))
	{
		SetStatus(WNDBUTTON_ON_DRAGING_WIDTH_LBUTTON, TRUE);
		g_pUI->m_Script.FireWndEvent(this, "OnDragButtonBegin");
	}

    SetStatus(WNDBUTTON_MOUSE_DOWN, TRUE);
    SetStatus(WNDBUTTON_PRESSED, TRUE);
    UpdataAnimateGroup();

    pParent = GetParent();
    if (pParent)
        pParent->WndProc(WND_N_BUTTON_DOWN, (WPARAM)(KWndWindow*)this, lParam);

    nResult = true;
Exit0:
    return nResult;
}

int KWndButton::OnLButtonUp(LPARAM lParam)
{
    int nResult = false;
    KWndWindow *pFocus = NULL;
    KWndWindow* pParent = NULL;
	int nCaptrueMouse = false;
	int nPtInMe = false;

	if (g_pUI->m_WndStation.GetCapture() == this)
	{
		g_pUI->m_WndStation.ReleaseCapture();
		nCaptrueMouse = true;
	}

    KG_PROCESS_ERROR(IsStatus(WNDBUTTON_PRESSED));
    KG_PROCESS_ERROR(IsStatus(WNDBUTTON_ENABLE));

	pFocus = g_pUI->m_WndStation.GetFocusWindow();
    KG_PROCESS_ERROR(pFocus == this);

	nPtInMe = PtInWindow(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    SetStatus(WNDBUTTON_MOUSE_DOWN, FALSE);
	if (nPtInMe)
		SetStatus(WNDBUTTON_MOUSE_OVER, TRUE);
    SetStatus(WNDBUTTON_PRESSED, FALSE);

    UpdataAnimateGroup();

	pParent = GetParent();
    if (pParent)
        pParent->WndProc(WND_N_BUTTON_CLICK, (WPARAM)(KWndWindow*)this, (LPARAM)0);

	if (nPtInMe)
		g_pUI->m_Script.FireWndEvent(this, "OnLButtonClick");

	if (nCaptrueMouse)
	{
		if (IsStatus(WNDBUTTON_DRAG_WIDTH_LBUTTON))
		{
			SetStatus(WNDBUTTON_ON_DRAGING_WIDTH_LBUTTON, FALSE);
			g_pUI->m_Script.FireWndEvent(this, "OnDragButtonEnd");
		}
	}

    nResult = true;
Exit0:
	if (!nResult)
	{
		SetStatus(WNDBUTTON_MOUSE_DOWN, FALSE);
		SetStatus(WNDBUTTON_PRESSED, FALSE);
		UpdataAnimateGroup();
	}
    return nResult;
}

int KWndButton::OnRButtonDown(LPARAM lParam)
{
	int nResult = false;
	KWndWindow *pParent = NULL;   

	KG_PROCESS_ERROR(IsStatus(WNDBUTTON_ENABLE));

	g_pUI->m_WndStation.SetCapture(this); 
	if (IsStatus(WNDBUTTON_DRAG_WIDTH_RBUTTON))
	{
		SetStatus(WNDBUTTON_ON_DRAGING_WIDTH_RBUTTON, TRUE);
		g_pUI->m_Script.FireWndEvent(this, "OnDragButtonBegin");
	}

	SetStatus(WNDBUTTON_MOUSE_DOWN, TRUE);
	SetStatus(WNDBUTTON_PRESSED, TRUE);
	UpdataAnimateGroup();

	pParent = GetParent();
	if (pParent)
		pParent->WndProc(WND_N_BUTTON_DOWN, (WPARAM)(KWndWindow*)this, lParam);

	nResult = true;
Exit0:
	return nResult;
}

int KWndButton::OnRButtonUp(LPARAM lParam)
{
	int nResult = false;
	KWndWindow *pFocus = NULL;
	KWndWindow* pParent = NULL;
	int nCaptrueMouse = false;
	int nPtInMe = false;

	if (g_pUI->m_WndStation.GetCapture() == this)
	{
		g_pUI->m_WndStation.ReleaseCapture();
		nCaptrueMouse = true;
	}

	KG_PROCESS_ERROR(IsStatus(WNDBUTTON_PRESSED));
	KG_PROCESS_ERROR(IsStatus(WNDBUTTON_ENABLE));

	pFocus = g_pUI->m_WndStation.GetFocusWindow();
	KG_PROCESS_ERROR(pFocus == this);

	nPtInMe = PtInWindow(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	SetStatus(WNDBUTTON_MOUSE_DOWN, FALSE);
	if (nPtInMe)
		SetStatus(WNDBUTTON_MOUSE_OVER, TRUE);
	SetStatus(WNDBUTTON_PRESSED, FALSE);

	UpdataAnimateGroup();

	pParent = GetParent();
	if (pParent)
		pParent->WndProc(WND_N_BUTTON_CLICK, (WPARAM)(KWndWindow*)this, (LPARAM)0);

	if (nPtInMe)
		g_pUI->m_Script.FireWndEvent(this, "OnRButtonClick");

	if (nCaptrueMouse)
	{
		if (IsStatus(WNDBUTTON_DRAG_WIDTH_RBUTTON))
		{
			SetStatus(WNDBUTTON_ON_DRAGING_WIDTH_RBUTTON, FALSE);
			g_pUI->m_Script.FireWndEvent(this, "OnDragButtonEnd");
		}
	}

	nResult = true;
Exit0:
	if (!nResult)
	{
		SetStatus(WNDBUTTON_MOUSE_DOWN, FALSE);
		SetStatus(WNDBUTTON_PRESSED, FALSE);
		UpdataAnimateGroup();
	}
	return nResult;
}

int KWndButton::IsButtonEnabled() const
{
    return IsStatus(WNDBUTTON_ENABLE);
}

void KWndButton::SetButtonDownState()
{
    SetStatus(WNDBUTTON_MOUSE_DOWN, TRUE);
    UpdataAnimateGroup();
}

void KWndButton::ClearButtonDownstate()
{
    SetStatus(WNDBUTTON_MOUSE_DOWN, FALSE);
    UpdataAnimateGroup();
}

void KWndButton::SetAnimateGroupNormal(int nGroup)
{
    m_nAnimateGroup[BUTTON_STATUS_NORMAL] = nGroup;
    UpdataAnimateGroup(true);
}

void KWndButton::SetAnimateGroupMouseOver(int nGroup)
{
    m_nAnimateGroup[BUTTON_STATUS_MOUSEOVER] = nGroup;
    UpdataAnimateGroup(true);
}

void KWndButton::SetAnimateGroupMouseDown(int nGroup)
{
    m_nAnimateGroup[BUTTON_STATUS_MOUSEDOWN] = nGroup;
    UpdataAnimateGroup(true);
}

void KWndButton::SetAnimateGroupDisable(int nGroup)
{
    m_nAnimateGroup[BUTTON_STATUS_DISABLE] = nGroup;
    UpdataAnimateGroup(true);
}

void KWndButton::RegisterLButtonDrag()
{
    SetStatus(WNDBUTTON_DRAG_WIDTH_LBUTTON, TRUE);
}

void KWndButton::UnregisterLButtonDrag()
{
    SetStatus(WNDBUTTON_DRAG_WIDTH_LBUTTON, FALSE);
}

int KWndButton::IsLButtonDragable()
{
    return IsStatus(WNDBUTTON_DRAG_WIDTH_LBUTTON);
}

void KWndButton::RegisterRButtonDrag()
{
    SetStatus(WNDBUTTON_DRAG_WIDTH_RBUTTON, TRUE);
}

void KWndButton::UnregisterRButtonDrag()
{
    SetStatus(WNDBUTTON_DRAG_WIDTH_RBUTTON, FALSE);
}

int KWndButton::IsRButtonDragable()
{
    return IsStatus(WNDBUTTON_DRAG_WIDTH_RBUTTON);
}


int KWndButton::EnableButton(int nEnable)
{
	SetStatus(WNDBUTTON_ENABLE, nEnable);
    
    UpdataAnimateGroup();

    return IsButtonEnabled();
}


int KWndButton::UpdataAnimateGroup(int nForceToUpData /* = false*/)
{
    int nReslut  = false;
    int nRetCode = false;
    BUTTON_STATUS eGroupIndex = BUTTON_STATUS_BEGIN;

	KG_PROCESS_ERROR(m_nAnimateIndex != -1);

	if (IsStatus(WNDBUTTON_MOUSE_DOWN))
        eGroupIndex = BUTTON_STATUS_MOUSEDOWN;
    else if (!IsStatus(WNDBUTTON_ENABLE))
        eGroupIndex = BUTTON_STATUS_DISABLE;
    else if (IsStatus(WNDBUTTON_MOUSE_OVER))
        eGroupIndex = BUTTON_STATUS_MOUSEOVER;
    else
        eGroupIndex = BUTTON_STATUS_NORMAL;

    if (m_eCurrentGroupIndex != eGroupIndex || nForceToUpData)
    {
        ASSERT(eGroupIndex < BUTTON_STATUS_END);
        KG_PROCESS_ERROR(eGroupIndex < BUTTON_STATUS_END);

        g_pUI->m_AnimateImageMgr.SetGroup(m_nAnimateIndex, m_nAnimateGroup[eGroupIndex]);
        g_pUI->m_AnimateImageMgr.SetFrame(m_nAnimateIndex, 0);
        g_pUI->m_AnimateImageMgr.SetLoopCount(m_nAnimateIndex, -1);

        m_eCurrentGroupIndex = eGroupIndex;
    }

	UpdataOverTextShow();

    nReslut = true;
Exit0:
    return nReslut;
}

int KWndButton::UpdataOverTextShow()
{
	int nResult = false;
	int nCount = 0;
	int nFont = -1;
	KItemHandle *pHandle = GetItemHandle();
	KG_PROCESS_ERROR(pHandle);
	KG_PROCESS_ERROR(m_eCurrentGroupIndex >= BUTTON_STATUS_BEGIN && m_eCurrentGroupIndex < BUTTON_STATUS_END);
	nFont = m_nOverTextFrame[m_eCurrentGroupIndex];
	if (nFont < 0) 
		nFont = m_nOverTextFrame[BUTTON_STATUS_NORMAL];
	KG_PROCESS_ERROR(nFont >= 0);

	nCount = pHandle->GetItemCount();
	for (int i = 0; i < nCount; ++i)
	{
		KItemText *pText = g_ItemCast<KItemText>(pHandle->GetItemByIndex(i));
		if (pText)
			pText->SetFontScheme(nFont);
	}

	nResult = true;
Exit0:
	return nResult;
}

void KWndButton::SetStatus(DWORD dwStatus, BOOL bEnable)
{
	if (bEnable)
		m_dwStatus |= dwStatus;
	else
		m_dwStatus &= ~dwStatus;
}

int KWndButton::CanCastTo(LPCSTR szType) const
{
	return szType == "WndButton" || szType == "WndWindow";
}

LPCSTR KWndButton::GetWndType() const
{
	return "WndButton";
}

} // namespace