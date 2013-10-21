#include "stdafx.h"
#include "./wndmessage.h"
#include "./wndcheckbox.h"
#include "./Components/itemhandle.h"
#include "../common/ksmallobjectpool.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"

namespace
{
	KSmallObjectPool	g_Allocator("KWndCheckBox", sizeof(UI::KWndCheckBox));
}

namespace UI
{

#ifdef KGUI_LUA_LOADER
int KWndCheckBox::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KWndWindow *pWnd = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pWnd = new(std::nothrow) KWndCheckBox;
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

KWndCheckBox::KWndCheckBox()
: m_eCurrentGroupIndex(WNDCHECKBOX_STATUS_END)
, m_dwStatus(WNDCHECKBOX_ENABLE)
, m_nAnimateIndex(-1)
{
	for (int nIndex = WNDCHECKBOX_STATUS_BEGIN; nIndex != WNDCHECKBOX_STATUS_END; ++nIndex)
	{
		m_nAnimateGroup[nIndex] = 0;
		m_nOverTextFont[nIndex] = -1;
	}
}

KWndCheckBox::~KWndCheckBox()
{
	g_pUI->m_AnimateImageMgr.UnloadFile(m_nAnimateIndex);
}

void* KWndCheckBox::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KWndCheckBox::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KWndCheckBox::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KWndCheckBox::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KWndCheckBox::Init(lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    char szImagePath[MAX_PATH];
    int  nDisCheck = false;
    int  nChecked  = false;
    int  nRadioBtn = false;

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("image",                            emVAR_TYPE_STRING, "",      szImagePath,                                                   _countof(szImagePath)),
        KLUA_TABLE_FIELD("DisableCheck",                     emVAR_TYPE_INT,    false,   &nDisCheck,                                                    0),
        KLUA_TABLE_FIELD("CheckedWhenCreate",                emVAR_TYPE_INT,    false,   &nChecked,                                                     0),
        KLUA_TABLE_FIELD("UnCheckAndEnable",                 emVAR_TYPE_INT,    0,       &m_nAnimateGroup[WNDCHECKBOX_UNCHCKED_AND_ENABLE],             0),
        KLUA_TABLE_FIELD("CheckAndEnable",                   emVAR_TYPE_INT,    0,       &m_nAnimateGroup[WNDCHECKBOX_CHCKED_AND_ENABLE],               0),
        KLUA_TABLE_FIELD("UnCheckAndDisable",                emVAR_TYPE_INT,    0,       &m_nAnimateGroup[WNDCHECKBOX_UNCHCKED_AND_DISABLE],            0),
        KLUA_TABLE_FIELD("CheckAndDisable",                  emVAR_TYPE_INT,    0,       &m_nAnimateGroup[WNDCHECKBOX_CHCKED_AND_DISABLE],              0),
        KLUA_TABLE_FIELD("Checking",                         emVAR_TYPE_INT,    0,       &m_nAnimateGroup[WNDCHECKBOX_NOW_CHECKING],                    0),
        KLUA_TABLE_FIELD("UnChecking",                       emVAR_TYPE_INT,    0,       &m_nAnimateGroup[WNDCHECKBOX_NOW_UNCHECKING],                  0),
        KLUA_TABLE_FIELD("CheckedAndEnableWhenMouseOver",    emVAR_TYPE_INT,    0,       &m_nAnimateGroup[WNDCHECKBOX_CHECKED_AND_ENABLE_MOUSEOVER],    0),
        KLUA_TABLE_FIELD("UnCheckedAndEnableWhenMouseOver",  emVAR_TYPE_INT,    0,       &m_nAnimateGroup[WNDCHECKBOX_UNCHECKED_AND_ENABLE_MOUSEOVER],  0),
        KLUA_TABLE_FIELD("CheckedAndDisableWhenMouseOver",   emVAR_TYPE_INT,    0,       &m_nAnimateGroup[WNDCHECKBOX_CHECKED_AND_DISABLE_MOUSEOVER],   0),
        KLUA_TABLE_FIELD("UnCheckedAndDisableWhenMouseOver", emVAR_TYPE_INT,    0,       &m_nAnimateGroup[WNDCHECKBOX_UNCHECKED_AND_DISABLE_MOUSEOVER], 0),
        KLUA_TABLE_FIELD("RadioButton",                      emVAR_TYPE_INT,    false,   &nRadioBtn,                                                    0),
        KLUA_TABLE_FIELD("CheckFont",                        emVAR_TYPE_INT,    0,       &m_nOverTextFont[WNDCHECKBOX_CHCKED_AND_ENABLE],               0),
        KLUA_TABLE_FIELD("UncheckFont",                      emVAR_TYPE_INT,    0,       &m_nOverTextFont[WNDCHECKBOX_UNCHCKED_AND_ENABLE],             0),
        KLUA_TABLE_FIELD("MouseDownFont",                    emVAR_TYPE_INT,    0,       &m_nOverTextFont[WNDCHECKBOX_NOW_CHECKING],                    0),
        KLUA_TABLE_FIELD("MouseOverFont",                    emVAR_TYPE_INT,    0,       &m_nOverTextFont[WNDCHECKBOX_CHECKED_AND_ENABLE_MOUSEOVER],    0),
        KLUA_TABLE_FIELD("DisableFont",                      emVAR_TYPE_INT,    0,       &m_nOverTextFont[WNDCHECKBOX_CHCKED_AND_DISABLE],              0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KWndWindow::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    ASSERT(m_nAnimateIndex == -1 && "贴图未释放"); 
    KGLOG_PROCESS_ERROR(szImagePath[0]);    // 必须不为空
    FormatFilePath(szImagePath);
    m_nAnimateIndex = g_pUI->m_AnimateImageMgr.LoadFile(szImagePath);
    KGLOG_PROCESS_ERROR(m_nAnimateIndex >= 0);

    g_SetBitFlag(!nDisCheck, WNDCHECKBOX_ENABLE, m_dwStatus);
    g_SetBitFlag(nChecked, WNDCHECKBOX_CHECKED, m_dwStatus);
    g_SetBitFlag(nRadioBtn, WNDCHECKBOX_RADIO_BUTTON, m_dwStatus);

    m_nOverTextFont[WNDCHECKBOX_NOW_UNCHECKING] = m_nOverTextFont[WNDCHECKBOX_NOW_CHECKING];
    m_nOverTextFont[WNDCHECKBOX_UNCHECKED_AND_ENABLE_MOUSEOVER] = m_nOverTextFont[WNDCHECKBOX_CHECKED_AND_ENABLE_MOUSEOVER];
    m_nOverTextFont[WNDCHECKBOX_CHECKED_AND_DISABLE_MOUSEOVER] = m_nOverTextFont[WNDCHECKBOX_CHECKED_AND_ENABLE_MOUSEOVER];
    m_nOverTextFont[WNDCHECKBOX_UNCHECKED_AND_DISABLE_MOUSEOVER] = m_nOverTextFont[WNDCHECKBOX_CHECKED_AND_ENABLE_MOUSEOVER];
    m_nOverTextFont[WNDCHECKBOX_UNCHCKED_AND_DISABLE] = m_nOverTextFont[WNDCHECKBOX_CHCKED_AND_DISABLE];

    UpdataAnimateGroup();
// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif


int KWndCheckBox::PtInWindow(int x, int y)
{
    return KWndWindow::PtInWindow(x, y);
}

void KWndCheckBox::OnPaint()
{
    g_pUI->m_AnimateImageMgr.Draw(m_nAnimateIndex, m_fAbsX, m_fAbsY, m_fWidth, m_fHeight, m_dwAlpha);
    KWndWindow::OnPaint();
}

LRESULT KWndCheckBox::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    LRESULT lRet = FALSE;
	
	if (KWndWindow::WndProc(uMsg, wParam, lParam))
		return FALSE;

    switch (uMsg) 
    {
    case WM_LBUTTONDOWN:
		g_pUI->m_WndStation.SetCapture(this); 
		if (IsStatus(WNDCHECKBOX_DRAG_WIDTH_LBUTTON))
		{
			if (g_pUI->m_Script.FireWndEvent(this, "OnCheckBoxDragBegin"))
				lRet = TRUE;
		}
        if (OnLButtonDown())
			lRet = TRUE;
        break;
    case WM_LBUTTONUP:
		lRet = OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		if (g_pUI->m_WndStation.GetCapture() == this)
		{
			g_pUI->m_WndStation.ReleaseCapture();
			if (IsStatus(WNDCHECKBOX_DRAG_WIDTH_LBUTTON))
			{
				if (g_pUI->m_Script.FireWndEvent(this, "OnCheckBoxDragEnd"))
					lRet = TRUE;
			}
		}
        break;
	case WM_RBUTTONDOWN:
		if (IsStatus(WNDCHECKBOX_DRAG_WIDTH_RBUTTON))
		{
			SetStatus(WNDCHECKBOX_PRESSED, TRUE);
	        UpdataAnimateGroup();
			g_pUI->m_WndStation.SetCapture(this); 
			if (g_pUI->m_Script.FireWndEvent(this, "OnCheckBoxDragBegin"))
				lRet = TRUE;
		} 
		break;
	case WM_LBUTTONDBLCLK:
		WndProc(WM_LBUTTONDOWN, wParam, lParam);
		WndProc(WM_LBUTTONUP, wParam, lParam);
		lRet = TRUE;
		break;
	case WM_RBUTTONUP:
		if (g_pUI->m_WndStation.GetCapture() == this)
		{
			g_pUI->m_WndStation.ReleaseCapture();
			SetStatus(WNDCHECKBOX_PRESSED, FALSE);
			UpdataAnimateGroup();
			if (IsStatus(WNDCHECKBOX_DRAG_WIDTH_RBUTTON))
			{
				if (g_pUI->m_Script.FireWndEvent(this, "OnCheckBoxDragEnd"))
					lRet = TRUE;
			}
		}
		break;
	case WM_MOUSEMOVE:
		if ((wParam & MK_LBUTTON) && IsStatus(WNDCHECKBOX_DRAG_WIDTH_LBUTTON) ||
			(wParam & MK_RBUTTON) && IsStatus(WNDCHECKBOX_DRAG_WIDTH_RBUTTON))
		{
			if (g_pUI->m_Script.FireWndEvent(this, "OnCheckBoxDrag"))
				lRet = TRUE;
		}
		break;
    case WND_M_MOUSE_ENTER:
        SetStatus(WNDCHECKBOX_MOUSEOVER, TRUE);
        UpdataAnimateGroup();
        lRet = TRUE;
        break;
    case WND_M_MOUSE_LEAVE:
        SetStatus(WNDCHECKBOX_MOUSEOVER, FALSE);
        UpdataAnimateGroup();
        lRet = TRUE;
        break;
    case WND_M_KILL_FOCUS:
	case WND_M_CAPTURECHANGED:
        SetStatus(WNDCHECKBOX_PRESSED, FALSE);
		UpdataAnimateGroup();
        lRet = TRUE;
        break;
    }

    return lRet;
}

int KWndCheckBox::OnLButtonDown()
{
    int nResult = false;

    KG_PROCESS_ERROR(IsStatus(WNDCHECKBOX_ENABLE));
	
	SetStatus(WNDCHECKBOX_PRESSED, TRUE);
	UpdataAnimateGroup();

    nResult = true;
Exit0:
    return nResult;
}

int KWndCheckBox::OnLButtonUp(int x, int y)
{
    int nResult     = false;
    int nRetCode    = false;
    KWndWindow *pFocus = NULL;
    KWndWindow *pParent = NULL;

    KG_PROCESS_ERROR(IsStatus(WNDCHECKBOX_PRESSED));
    KG_PROCESS_ERROR(IsStatus(WNDCHECKBOX_ENABLE));

    pFocus = g_pUI->m_WndStation.GetFocusWindow();
    KG_PROCESS_ERROR(pFocus == this);

	SetStatus(WNDCHECKBOX_PRESSED, FALSE);

	if (PtInWindow(x, y))
	{
		if (IsStatus(WNDCHECKBOX_RADIO_BUTTON))
			nRetCode = Check(true);
		else
			nRetCode = Check(!IsStatus(WNDCHECKBOX_CHECKED));
		pParent = GetParent();
		if (pParent)
			pParent->WndProc(WND_N_BUTTON_CLICK, WPARAM(this), LPARAM(nRetCode));
	}

    UpdataAnimateGroup();

    nResult = true;
Exit0:
    return nResult;
}

int KWndCheckBox::IsCheckBoxChecked()
{
    return IsStatus(WNDCHECKBOX_CHECKED);
}

int KWndCheckBox::IsCheckBoxActive()
{
    return IsStatus(WNDCHECKBOX_ENABLE);
}

int KWndCheckBox::Check(int nCheckFlag)
{
	int nRetCode = false;

    if (nCheckFlag)
    {
        if (!IsStatus(WNDCHECKBOX_CHECKED))
        {
            SetStatus(WNDCHECKBOX_CHECKED, TRUE);
            g_pUI->m_Script.FireWndEvent(this, "OnCheckBoxCheck");
        }         
    }
    else
    {
        if (IsStatus(WNDCHECKBOX_CHECKED))
        {
            SetStatus(WNDCHECKBOX_CHECKED, FALSE);
            g_pUI->m_Script.FireWndEvent(this, "OnCheckBoxUncheck");
        }
    }

    UpdataAnimateGroup();

    nRetCode = true;
	return nRetCode;
}

int KWndCheckBox::EnableCheck(int nEnableFlag)
{
	SetStatus(WNDCHECKBOX_ENABLE, nEnableFlag);
    
    UpdataAnimateGroup();

    return IsCheckBoxActive();
}

int KWndCheckBox::UpdataAnimateGroup()
{
    int nReslut  = false;
    int nRetCode = false;
    WNDCHECKBOX_STATUS eGroupIndex = WNDCHECKBOX_STATUS_BEGIN;

	if (IsStatus(WNDCHECKBOX_PRESSED) && IsStatus(WNDCHECKBOX_ENABLE))
	{
		if (IsStatus(WNDCHECKBOX_CHECKED))
			eGroupIndex = WNDCHECKBOX_NOW_UNCHECKING;
		else
			eGroupIndex = WNDCHECKBOX_NOW_CHECKING;
	}
	else if (IsStatus(WNDCHECKBOX_MOUSEOVER))
    {
        if (IsStatus(WNDCHECKBOX_ENABLE))
        {
            if (IsStatus(WNDCHECKBOX_CHECKED))
                eGroupIndex = WNDCHECKBOX_CHECKED_AND_ENABLE_MOUSEOVER;
            else
                eGroupIndex = WNDCHECKBOX_UNCHECKED_AND_ENABLE_MOUSEOVER;
        }
        else
        {
            if (IsStatus(WNDCHECKBOX_CHECKED))
                eGroupIndex = WNDCHECKBOX_CHECKED_AND_DISABLE_MOUSEOVER;
            else
                eGroupIndex = WNDCHECKBOX_UNCHECKED_AND_DISABLE_MOUSEOVER;
        }
    }
    else
    {
        if (IsStatus(WNDCHECKBOX_ENABLE))
        {
            if (IsStatus(WNDCHECKBOX_CHECKED))
                eGroupIndex = WNDCHECKBOX_CHCKED_AND_ENABLE;
            else
                eGroupIndex = WNDCHECKBOX_UNCHCKED_AND_ENABLE;
        }
        else
        {
            if (IsStatus(WNDCHECKBOX_CHECKED))
                eGroupIndex = WNDCHECKBOX_CHCKED_AND_DISABLE;
            else
                eGroupIndex = WNDCHECKBOX_UNCHCKED_AND_DISABLE;
        }
    }

    if (m_eCurrentGroupIndex != eGroupIndex)
    {
        ASSERT(eGroupIndex < WNDCHECKBOX_STATUS_END);

        KG_PROCESS_ERROR(m_nAnimateIndex >= 0);
        KG_PROCESS_ERROR(eGroupIndex < WNDCHECKBOX_STATUS_END);

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

int KWndCheckBox::UpdataOverTextShow()
{
	int nResult = false;
	int nCount = 0;
	int nFont = -1;
	KItemHandle *pHandle = GetItemHandle();
	KG_PROCESS_ERROR(pHandle);
	KG_PROCESS_ERROR(m_eCurrentGroupIndex >= WNDCHECKBOX_STATUS_BEGIN && m_eCurrentGroupIndex < WNDCHECKBOX_STATUS_END);
	nFont = m_nOverTextFont[m_eCurrentGroupIndex];
	if (nFont < 0) 
		nFont = m_nOverTextFont[WNDCHECKBOX_UNCHCKED_AND_ENABLE];
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

int KWndCheckBox::CanCastTo(LPCSTR szType) const
{
	return szType == "WndCheckBox" || szType == "WndWindow";
}

void KWndCheckBox::RegisterLButtonDrag()
{
	SetStatus(WNDCHECKBOX_DRAG_WIDTH_LBUTTON, TRUE);
}

void KWndCheckBox::UnregisterLButtonDrag()
{
	SetStatus(WNDCHECKBOX_DRAG_WIDTH_LBUTTON, FALSE);
}

int KWndCheckBox::IsLButtonDragable()
{
	return IsStatus(WNDCHECKBOX_DRAG_WIDTH_LBUTTON); 
}

void KWndCheckBox::RegisterRButtonDrag()
{
	SetStatus(WNDCHECKBOX_DRAG_WIDTH_RBUTTON, TRUE);
}

void KWndCheckBox::UnregisterRButtonDrag()
{
	SetStatus(WNDCHECKBOX_DRAG_WIDTH_RBUTTON, FALSE);
}

int KWndCheckBox::IsRButtonDragable()
{
	return IsStatus(WNDCHECKBOX_DRAG_WIDTH_RBUTTON); 
}

void KWndCheckBox::SetAnimation(int nUnCheckAndEnable, int nCheckAndEnable, int nUnCheckAndDisable, int nCheckAndDisable, 
				  int nChecking, int nUnChecking, int nCheckedAndEnableWhenMouseOver, int nUncheckedAndEnableWhenMouseOver, 
				  int nCheckedAndDisableWhenMouseOver, int nUncheckedAndDisableWhenMouseOver, LPCSTR szImagePath)
{
	m_nAnimateGroup[WNDCHECKBOX_UNCHCKED_AND_ENABLE] = nUnCheckAndEnable;
	m_nAnimateGroup[WNDCHECKBOX_CHCKED_AND_ENABLE] = nCheckAndEnable;
	m_nAnimateGroup[WNDCHECKBOX_UNCHCKED_AND_DISABLE] = nUnCheckAndDisable;
	m_nAnimateGroup[WNDCHECKBOX_CHCKED_AND_DISABLE] = nCheckAndDisable;
	m_nAnimateGroup[WNDCHECKBOX_NOW_CHECKING] = nChecking;
	m_nAnimateGroup[WNDCHECKBOX_NOW_UNCHECKING] = nUnChecking;
	m_nAnimateGroup[WNDCHECKBOX_CHECKED_AND_ENABLE_MOUSEOVER] = nCheckedAndEnableWhenMouseOver;
	m_nAnimateGroup[WNDCHECKBOX_UNCHECKED_AND_ENABLE_MOUSEOVER] = nUncheckedAndEnableWhenMouseOver;
	m_nAnimateGroup[WNDCHECKBOX_CHECKED_AND_DISABLE_MOUSEOVER] = nCheckedAndDisableWhenMouseOver;
	m_nAnimateGroup[WNDCHECKBOX_UNCHECKED_AND_DISABLE_MOUSEOVER] = nUncheckedAndDisableWhenMouseOver;

	if (m_nAnimateIndex >= 0)
		g_pUI->m_AnimateImageMgr.UnloadFile(m_nAnimateIndex);

    m_nAnimateIndex = g_pUI->m_AnimateImageMgr.LoadFile(szImagePath);
    KGLOG_PROCESS_ERROR(m_nAnimateIndex >= 0);

	m_eCurrentGroupIndex = WNDCHECKBOX_STATUS_END;
	UpdataAnimateGroup();
	return;
Exit0:
	return;
}

void KWndCheckBox::SetFontScheme(int nCheckFont, int nUncheckFont, int nMouseDownFont, int nMouseOverFont, int nDisableFont)
{
	m_nOverTextFont[WNDCHECKBOX_CHCKED_AND_ENABLE] = nCheckFont;
	m_nOverTextFont[WNDCHECKBOX_UNCHCKED_AND_ENABLE] = nUncheckFont;
	m_nOverTextFont[WNDCHECKBOX_NOW_CHECKING] = nMouseDownFont;
	m_nOverTextFont[WNDCHECKBOX_NOW_UNCHECKING] = nMouseDownFont;
	m_nOverTextFont[WNDCHECKBOX_CHECKED_AND_ENABLE_MOUSEOVER] = nMouseOverFont;
	m_nOverTextFont[WNDCHECKBOX_UNCHECKED_AND_ENABLE_MOUSEOVER] = nMouseOverFont;
	m_nOverTextFont[WNDCHECKBOX_CHECKED_AND_DISABLE_MOUSEOVER] = nDisableFont;
	m_nOverTextFont[WNDCHECKBOX_UNCHECKED_AND_DISABLE_MOUSEOVER] = nDisableFont;
	m_nOverTextFont[WNDCHECKBOX_CHCKED_AND_DISABLE] = nDisableFont;
	m_nOverTextFont[WNDCHECKBOX_UNCHCKED_AND_DISABLE] = nDisableFont;
}

void KWndCheckBox::SetStatus(DWORD dwStatus, BOOL bEnable)
{
	if (bEnable)
		m_dwStatus |= dwStatus;
	else
		m_dwStatus &= ~dwStatus;
}

LPCSTR KWndCheckBox::GetWndType() const
{
	return "WndCheckBox";
}

}  //namespace UI



