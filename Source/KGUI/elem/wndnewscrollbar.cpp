////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : wndnewscrollbar.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-2 10:25:55
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "./wndnewscrollbar.h"
#include "./wndmessage.h"
#include "./../common/ksmallobjectpool.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"

namespace
{
	KSmallObjectPool	g_Allocator("KWndNewScrollBar", sizeof(UI::KWndNewScrollBar));
}

namespace UI
{

#ifdef KGUI_LUA_LOADER
int KWndNewScrollBar::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KWndWindow *pWnd = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pWnd = new(std::nothrow) KWndNewScrollBar;
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

KWndNewScrollBar::KWndNewScrollBar()
: m_dwStatus(0)
, m_pSlideButton(NULL)
, m_fDragPosOffset(0.0f)
, m_nPageStepCount(0)
, m_nTotalStepCount(0)
, m_nScrollPos(0)
{
}

void* KWndNewScrollBar::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KWndNewScrollBar::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KWndNewScrollBar::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KWndNewScrollBar::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KWndNewScrollBar::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    int nScrollType = 0;
    int nAutoHide   = false;
    int nAutoResize = false;
    int nSetpCount  = 0;
    int nPageCount  = 0;
    int nTopIndex   = 0;

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("type",          emVAR_TYPE_INT,    0,     &nScrollType, 0),
        KLUA_TABLE_FIELD("AutoHide",      emVAR_TYPE_INT,    false, &nAutoHide,   0),
        KLUA_TABLE_FIELD("AutoResize",    emVAR_TYPE_INT,    false, &nAutoResize, 0),
        KLUA_TABLE_FIELD("StepCount",     emVAR_TYPE_INT,    0,     &nSetpCount,  0),
        KLUA_TABLE_FIELD("PageStepCount", emVAR_TYPE_INT,    0,     &nPageCount,  0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nTopIndex = lua_gettop(pLua);

    nRetCode = KWndWindow::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    g_SetBitFlag(nScrollType, WNDNEWSCROLL_VERTICAL, m_dwStatus);
    g_SetBitFlag(nAutoHide, WNDNEWSCROLL_AUTO_HIDE_SLIDE, m_dwStatus);
    g_SetBitFlag(nAutoResize, WNDNEWSCROLL_AUTO_RESIZE_SLIDE, m_dwStatus);

    lua_getfield(pLua, nTableIndex, "SlideBtn");
    nRetCode = lua_isnil(pLua, -1);
    if (!nRetCode)
    {
        nRetCode = KWndButton::Create((IKUIObject **)&m_pSlideButton, pLua, -1);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(m_pSlideButton);
        AddChild(m_pSlideButton);
        SetStepCount(nSetpCount);
        SetPageStepCount(nPageCount);
    }

// Exit1:
	nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_RELEASE(m_pSlideButton);
    }
    lua_settop(pLua, nTopIndex);
	return nResult;
}
#endif

LRESULT KWndNewScrollBar::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = FALSE;

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		lResult = OnLButtonDown((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEMOVE:
		if ((wParam & MK_LBUTTON) && IsStatus(WNDNEWSCROLL_DRAGGING_SLIDE))
			lResult = OnDraggingSlide((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		break;
	case WM_LBUTTONUP:
		if (IsStatus(WNDNEWSCROLL_DRAGGING_SLIDE))
		{
			SetStatus(WNDNEWSCROLL_DRAGGING_SLIDE, FALSE);
			
			g_pUI->m_WndStation.ReleaseCapture();
            if (m_pSlideButton)
                m_pSlideButton->ClearButtonDownstate();
		}
        lResult = TRUE;
		break;
	case WND_N_BUTTON_DOWN:
		if (wParam == (WPARAM)(KWndWindow*)m_pSlideButton)
            OnSlideBtnPressed(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
        lResult = TRUE;
		break;
    case WND_M_MOUSE_LEAVE:
		if ((wParam & MK_LBUTTON) &&
			IsStatus(WNDNEWSCROLL_DRAGGING_SLIDE) &&
            (KWndWindow*)wParam == m_pSlideButton)
		{
            m_pSlideButton->SetButtonDownState();
		}
        lResult = TRUE;
        break;
	case WM_MOUSEWHEEL:
		ScrollNext(-GET_Y_LPARAM(wParam) / WHEEL_DELTA);
		lResult = TRUE;
	default:
        break;
	}

#ifndef DISABLE_UI_EDIT
    KWndWindow::WndProc(uMsg, wParam, lParam);
#endif

    return lResult;
}

void KWndNewScrollBar::OnPaint()
{
    KWndWindow::OnPaint();
}

int KWndNewScrollBar::OnLButtonDown(float fX, float fY)
{
    float fSlidBtnWidth = 0.0f;
    float fSlidBtnHeight = 0.0f;
    if (m_pSlideButton)
        m_pSlideButton->GetSize(&fSlidBtnWidth, &fSlidBtnHeight);
    if (IsStatus(WNDNEWSCROLL_VERTICAL))
    {
        //垂直型滚动条
        float fCurrentPos = 0.0f;
        if (m_nTotalStepCount > 0)
            fCurrentPos = m_fAbsY + (m_fHeight - fSlidBtnHeight) * m_nScrollPos / m_nTotalStepCount;
        if (fY < fCurrentPos)
            ScrollPagePrev();
        else
            ScrollPageNext();
    }
    else
    {
        float fCurrentPos = 0.0f;
        if (m_nTotalStepCount > 0)
            fCurrentPos = m_fAbsX + (m_fWidth - fSlidBtnWidth) * m_nScrollPos / m_nTotalStepCount;
        if (fX < fCurrentPos)
            ScrollPagePrev();
        else
            ScrollPageNext();
    }

    return true;
}

int KWndNewScrollBar::OnSlideBtnPressed(float x, float y)
{
    int nResult = false;

    KG_PROCESS_ERROR(m_pSlideButton);

	g_pUI->m_WndStation.SetCapture(this);
	SetStatus(WNDNEWSCROLL_DRAGGING_SLIDE, TRUE);

	m_pSlideButton->SetButtonDownState();

    if (IsStatus(WNDNEWSCROLL_VERTICAL))
        m_fDragPosOffset = y - m_pSlideButton->GetAbsY();
    else
        m_fDragPosOffset = x - m_pSlideButton->GetAbsX();
        

    nResult = true;
Exit0:
    return nResult;
}

int KWndNewScrollBar::OnDraggingSlide(float fX, float fY)
{
    int nResult = false;
    float fScrollPos = 0.0f;
    float fSlideBtnWidth = 0.0f;
    float fSlideBtnHeight = 0.0f;
	float fBtnX = 0.0f;
	float fBtnY = 0.0f;

    KG_PROCESS_ERROR(m_pSlideButton);

    m_pSlideButton->GetSize(&fSlideBtnWidth, &fSlideBtnHeight);
	
	fBtnX = m_pSlideButton->GetRelX();
	fBtnY = m_pSlideButton->GetRelY();

	if (
        IsStatus(WNDNEWSCROLL_VERTICAL) && 
        m_fHeight - fSlideBtnHeight > FLT_EPSILON
    )
    {
		fBtnY = fY  - m_fAbsY - m_fDragPosOffset;
        fScrollPos = fBtnY / (m_fHeight - fSlideBtnHeight);
		fBtnY = min(fBtnY, m_fHeight - fSlideBtnHeight);
        goto Exit1;
    }

    if (m_fWidth - fSlideBtnWidth > FLT_EPSILON)
	{
		fBtnX = fX - m_fAbsX - m_fDragPosOffset;
        fScrollPos = fBtnX / (m_fWidth - fSlideBtnWidth);
		fBtnX = min(fBtnX, m_fWidth - fSlideBtnWidth);
	}

Exit1:
    SetScrollPos((int)(fScrollPos * m_nTotalStepCount));
	fBtnX = max(fBtnX, 0.0f);
	fBtnY = max(fBtnY, 0.0f);
	m_pSlideButton->SetRelPos(fBtnX, fBtnY);

    nResult = true;
Exit0:
    return nResult;
}

int KWndNewScrollBar::CanCastTo(LPCSTR szType) const
{
	return szType == "WndNewScrollBar" || szType == "WndWindow";
}


int KWndNewScrollBar::SetScrollPos(int nScrollPos)
{
    m_nScrollPos = nScrollPos;
    if (m_nScrollPos < 0)
        m_nScrollPos = 0;
    if (m_nScrollPos > m_nTotalStepCount)
        m_nScrollPos = m_nTotalStepCount;

    UpdateScrollPos();

	KWndWindow *pParent = GetParent();
	if (pParent)
    {
		pParent->WndProc(WND_N_SCORLLBAR_POS_CHANGED, (WPARAM)(KWndWindow*)this, (LPARAM)m_nScrollPos);
	    g_pUI->m_Script.FireWndEvent(this, "OnScrollBarPosChanged");
    }

    return true;
}

int KWndNewScrollBar::UpdateScrollPos()
{
    int nResult = false;
    float fSlideBtnPosX = 0.0f;
    float fSlideBtnPosY = 0.0f;
    float fSlideBtnWidth = 0.0f;
    float fSlideBtnHeight = 0.0f;

    KG_PROCESS_ERROR(m_pSlideButton);

    fSlideBtnPosX = m_pSlideButton->GetRelX();
	fSlideBtnPosY = m_pSlideButton->GetRelY();

    m_pSlideButton->GetSize(&fSlideBtnWidth, &fSlideBtnHeight);
    if (IsStatus(WNDNEWSCROLL_VERTICAL))
    {
        if (m_nTotalStepCount > 0)
            fSlideBtnPosY = (m_fHeight - fSlideBtnHeight) * m_nScrollPos / m_nTotalStepCount;
        else if (m_nTotalStepCount == 0)
            fSlideBtnPosY = 0.0f;
        KG_PROCESS_SUCCESS(true);
    }

    if (m_nTotalStepCount > 0)
        fSlideBtnPosX = (m_fWidth - fSlideBtnWidth) * m_nScrollPos / m_nTotalStepCount;
    else if (m_nTotalStepCount == 0)
        fSlideBtnPosX = 0.0f;
    

Exit1:
    m_pSlideButton->SetRelPos(fSlideBtnPosX, fSlideBtnPosY);
    nResult = true;
Exit0:
    return nResult;
}

int KWndNewScrollBar::UpdateScrollSize()
{
    int nResult = false;
    float fPos = 1.0f;
    float fSlideBtnWidth = 0.0f;
    float fSlideBtnHeight = 0.0f;

    KG_PROCESS_ERROR(m_pSlideButton);

	if (
        IsStatus(WNDNEWSCROLL_AUTO_HIDE_SLIDE) && 
        (m_nTotalStepCount <= 0)
    )
    {
        m_pSlideButton->Hide();
        KG_PROCESS_SUCCESS(true);
    }

    m_pSlideButton->Show();

    KG_PROCESS_SUCCESS(!IsStatus(WNDNEWSCROLL_AUTO_RESIZE_SLIDE));

    m_pSlideButton->GetSize(&fSlideBtnWidth, &fSlideBtnHeight);

    if (m_nPageStepCount > 0)
        fPos = (float)m_nPageStepCount / (float)(m_nTotalStepCount + m_nPageStepCount);
    else if (m_nTotalStepCount > 0)
        fPos = 1.0f / (float)m_nTotalStepCount;

    if (IsStatus(WNDNEWSCROLL_VERTICAL))
        m_pSlideButton->SetSize(fSlideBtnWidth, fPos * m_fHeight);
    else
        m_pSlideButton->SetSize(fPos * m_fWidth, fSlideBtnHeight);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KWndNewScrollBar::GetScrollPos()
{
    return m_nScrollPos;
}

int KWndNewScrollBar::SetStepCount(int nStepCount)
{
    KG_PROCESS_SUCCESS(nStepCount == m_nTotalStepCount);

	if (nStepCount < 0)
		nStepCount = 0;
    m_nTotalStepCount = nStepCount;
    if (m_nScrollPos > m_nTotalStepCount)
        m_nScrollPos = m_nTotalStepCount;

    UpdateScrollSize();
    SetScrollPos(m_nScrollPos);

Exit1:
    return true;
}

int KWndNewScrollBar::GetStepCount()
{
    return m_nTotalStepCount;
}

int KWndNewScrollBar::SetPageStepCount(int nStepCount)
{
    m_nPageStepCount = nStepCount;

    UpdateScrollSize();
    UpdateScrollPos();
    return true;
}

int KWndNewScrollBar::GetPageStepCount()
{
    return m_nPageStepCount;
}

void KWndNewScrollBar::BindSlideButton(KWndButton* pButton)
{
	m_pSlideButton = pButton;

    UpdateScrollSize();
    UpdateScrollPos();
}

int KWndNewScrollBar::ScrollPrev(int nStep /* = 1 */)
{
    SetScrollPos(m_nScrollPos - nStep);
    return true;
}

int KWndNewScrollBar::ScrollNext(int nStep /* = 1 */)
{
    SetScrollPos(m_nScrollPos + nStep);
    return true;
}

int KWndNewScrollBar::ScrollPagePrev()
{
    SetScrollPos(m_nScrollPos - m_nPageStepCount);
    return true;
}

int KWndNewScrollBar::ScrollPageNext()
{
    SetScrollPos(m_nScrollPos + m_nPageStepCount);
    return true;
}

int KWndNewScrollBar::ScrollHome()
{
    SetScrollPos(0);
    return true;
}

int KWndNewScrollBar::ScrollEnd()
{
    SetScrollPos(m_nTotalStepCount);
    return true;
}

void KWndNewScrollBar::SetStatus(DWORD dwStatus, BOOL bEnable)
{
	if (bEnable)
		m_dwStatus |= dwStatus;
	else
		m_dwStatus &= ~dwStatus;
}

LPCSTR KWndNewScrollBar::GetWndType() const
{
	return "WndNewScrollBar";
}

}  //namespace UI
