#include "stdafx.h"
#include "./wndmessage.h"
#include "./wndwindow.h"
#include "./wndframe.h"
#include "./wndbutton.h"
#include "./wndedit.h"
#include "./autolocatewnd.h"
#include "../config/kdebugconfig.h"
#include "../common/ksmallobjectpool.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"

namespace
{
	KSmallObjectPool	g_Allocator("KWndFrame", sizeof(UI::KWndFrame));
}

namespace UI
{

#ifdef KGUI_LUA_LOADER
int KWndFrame::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KWndFrame *pWnd = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pWnd = new(std::nothrow) KWndFrame;
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


KWndFrame::KWndFrame()
: m_dwStatus(0)
, m_pWndFocus(NULL)
, m_nFadeCount(0)
, m_nFadeFrame(0)
, m_nDefaultTableRef(LUA_NOREF)
, m_fDragAreaWidth(0.0f)
, m_fDragAreaX(0.0f)
, m_fDragAreaY(0.0f)
, m_fDragAreaHeight(0.0f)
#ifdef KGUI_LUA_LOADER
, m_nLayer(0)
#endif
{
	m_szLuaClass[0] = '\0';
}

KWndFrame::~KWndFrame()
{
	g_pUI->m_Script.UnsubscribeEvent(this);
}

void* KWndFrame::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KWndFrame::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KWndFrame::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KWndFrame::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KWndFrame::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;
    
    int  nEnableDrag  = false;
    int  nFollowMove  = false;
    int  nEnableTab   = false;
    int  nFollowFocus = false;
    int  nDisBreath   = false;
    int  nShowUI      = false;
    int  nRenderEvent = false;
    int  nRenderSamp  = false;
    int  nItemCount   = 0;
    char szLayer[KD_SMALL_BUFFER_SIZE];

    char szItem[WND_SECTION_NAME_LEN];
    char szCurItemKey[WND_SECTION_NAME_LEN];

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("layer",               emVAR_TYPE_STRING,  "",      szLayer,                _countof(szLayer)),
        KLUA_TABLE_FIELD("Dragable",            emVAR_TYPE_INT,     true,    &nEnableDrag,           0),
        KLUA_TABLE_FIELD("DragLeft",            emVAR_TYPE_FLOAT,   0,       &m_fDragAreaX,       0),
        KLUA_TABLE_FIELD("DragTop",             emVAR_TYPE_FLOAT,   0,       &m_fDragAreaY,        0),
        KLUA_TABLE_FIELD("DragWidth",           emVAR_TYPE_FLOAT,   0,       &m_fDragAreaWidth,      0),
        KLUA_TABLE_FIELD("DragHeight",          emVAR_TYPE_FLOAT,   0,       &m_fDragAreaHeight,     0),
        KLUA_TABLE_FIELD("FollowMouse",         emVAR_TYPE_INT,     false,   &nFollowMove,           0),
        KLUA_TABLE_FIELD("FollowMouseOffsetX",  emVAR_TYPE_FLOAT,   0,       &m_fFollowMouseOffsetX, 0),
        KLUA_TABLE_FIELD("FollowMouseOffsetY",  emVAR_TYPE_FLOAT,   0,       &m_fFollowMouseOffsetY, 0),
        KLUA_TABLE_FIELD("TabChangeFocus",      emVAR_TYPE_INT,     false,   &nEnableTab,            0),
        KLUA_TABLE_FIELD("MouseFollowFocus",    emVAR_TYPE_INT,     false,   &nFollowFocus,          0),
        KLUA_TABLE_FIELD("DisableBreath",       emVAR_TYPE_INT,     false,   &nDisBreath,            0),
        KLUA_TABLE_FIELD("AlwaysShow",          emVAR_TYPE_INT,     false,   &nShowUI,               0),
        KLUA_TABLE_FIELD("RenderEvent",         emVAR_TYPE_INT,     false,   &nRenderEvent,          0),
        KLUA_TABLE_FIELD("RenderSampling",      emVAR_TYPE_INT,     false,   &nRenderSamp,           0),
        KLUA_TABLE_FIELD("ItemCount",           emVAR_TYPE_INT,     0,       &nItemCount,            0),
        KLUA_TABLE_FIELD("class",               emVAR_TYPE_STRING,  "",      &m_szLuaClass,          _countof(m_szLuaClass)),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KWndWindow::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_emObjType = em_OBJ_TYPE_FRAME;
    
    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    m_nLayer = KWndStation::GetLayerIndexByName(szLayer);

    g_SetBitFlag(nEnableDrag, UI_WND_FRAME_ENABLE_DRAG, m_dwStatus);
    g_SetBitFlag(nFollowMove, UI_WND_FRAME_FOLLOW_MOUSE_MOVE, m_dwStatus);
    g_SetBitFlag(nEnableTab, UI_WND_FRAME_TAB_CHANGE_FOCUS, m_dwStatus);
    g_SetBitFlag(nFollowFocus, UI_WND_FRAME_MOUSE_FOLLOW_FOCUS, m_dwStatus);
    g_SetBitFlag(nDisBreath, UI_WND_FRAME_DISABLE_BREATH, m_dwStatus);
    g_SetBitFlag(nShowUI, UI_WND_FRAME_SHOW_WHEN_HIDE_UI, m_dwStatus);
    g_SetBitFlag(nRenderEvent, UI_WND_FRAME_POST_RENDER_EVENT, m_dwStatus);
    g_SetBitFlag(nRenderSamp, UI_WND_FRAME_RENDER_SAMPLING, m_dwStatus);

    m_FocusWndNameList.clear();
    for (int nIndex = 0; nIndex < nItemCount; ++nIndex) 
    {
        nRetCode = _snprintf(szCurItemKey, _countof(szCurItemKey), "Item_%d", nIndex);
        KGLOG_PROCESS_ERROR(nRetCode);
        szCurItemKey[_countof(szCurItemKey) - 1] = '\0';

        KLuaTableField ItemInfo = KLUA_TABLE_FIELD(szCurItemKey, emVAR_TYPE_STRING, "", szItem, _countof(szItem));
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &ItemInfo);
        if (nRetCode && szItem[0])
        {
            m_FocusWndNameList.push_back(szItem);
        }
    }

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
#endif

LRESULT KWndFrame::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int nResult = 0;

    switch(uMsg) 
    {
    case WM_LBUTTONDOWN:
        TryBeginDragWnd(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONUP:
		OnDragWndEnd(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEMOVE:
        if ((wParam & MK_LBUTTON) && IsStatus(UI_WND_FRAME_ON_DRAGGING))
            OnDragWnd(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
	case WND_M_CAPTURECHANGED:
		SetStatus(UI_WND_FRAME_ON_DRAGGING, FALSE);
		break;
    case WM_KEYDOWN:
        if (OnKeyDown(wParam, lParam))
			return TRUE;
        break;
    case WND_M_MOUSE_ENTER:
    case WND_M_MOUSE_LEAVE:
	case WND_N_SCORLLBAR_POS_CHANGED:
		return KWndWindow::WndProc(uMsg, wParam, lParam);
        break;
    }

	if (IsStatus(UI_WND_FRAME_MSG_FROM_CHILD))
		return KWndWindow::WndProc(uMsg, wParam, lParam);
	
	SetStatus(UI_WND_FRAME_MSG_FROM_CHILD, TRUE);

	LRESULT lResult = ChildProcessInput(uMsg, wParam, lParam);

	SetStatus(UI_WND_FRAME_MSG_FROM_CHILD, FALSE);

	return lResult;
}

LRESULT KWndFrame::ChildProcessInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nSetFocus = true;
	switch (uMsg) 
	{
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		nSetFocus = false;
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
		{
			KWndWindow* pTopWnd = TopChildFromPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			if (nSetFocus)
				SetFocusWnd(pTopWnd);
			if (pTopWnd)
				return pTopWnd->WndProc(uMsg, wParam, lParam);
		}
		break;
	case WM_MOUSEWHEEL:
		{
			KWndWindow* pTopWnd = TopChildFromPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			if (pTopWnd)
			{
				while (pTopWnd != this)
				{
					if (pTopWnd->WndProc(uMsg, wParam, lParam))
						return TRUE;
					pTopWnd = pTopWnd->GetParent();
				}
				return KWndWindow::WndProc(uMsg, wParam, lParam);
			}
		}
	default:
		if (m_pWndFocus)
			return m_pWndFocus->WndProc(uMsg, wParam, lParam);
		break;
	}

	return KWndWindow::WndProc(uMsg, wParam, lParam);
}

void KWndFrame::OnPaint()
{
	if (!g_pUI->m_WndStation.IsVisible() && !IsStatus(UI_WND_FRAME_SHOW_WHEN_HIDE_UI))
		return;

    //绘制拖动边框
    if (KDebugConfig::ms_aBoolFlag[KDebugConfig::BOOL_SHOW_FRAME_DRAG_AREA_BODER]) 
    {
        KG3DUIRect2 rect;
        rect.v2Min.x = m_fAbsX + m_fDragAreaX;
        rect.v2Min.y = m_fAbsY + m_fDragAreaY;
        rect.v2Max.x = rect.v2Min.x + m_fDragAreaWidth;
        rect.v2Max.y = rect.v2Min.y + m_fDragAreaHeight;
        rect.crDiffuse = 0xFFFFFFFF;

		g_pUI->m_p3DUI->DrawMultiRect(&rect, 1);
    }

	if (IsStatus(UI_WND_FRAME_POST_RENDER_EVENT))
	{
		g_pUI->m_Script.FireWndEvent(this, "OnFrameRender");
	}
	KWndWindow::OnPaint();

}

int KWndFrame::CanCastTo(LPCSTR szType) const
{
	return szType == "WndFrame" || szType == "WndWindow";
}

int KWndFrame::EnableDrag(int nEnableFlag)
{
	SetStatus(UI_WND_FRAME_ENABLE_DRAG, nEnableFlag);
   return true;
}

int KWndFrame::PtInWindow(int x, int y)
{
	if (!g_pUI->m_WndStation.IsVisible() && !IsStatus(UI_WND_FRAME_SHOW_WHEN_HIDE_UI))
		return false;

	float fRelX = x - m_fAbsX;
	float fRelY = y - m_fAbsY;

	if (IsVisible() && IsStyle(WND_S_SIZE_WITH_ALL_CHILD))
	{
		if ((IsStyle(UI_WND_FRAME_ENABLE_DRAG) || 
			(IsStyle(WND_S_SELF_HOLD_MOUSE_HOVER) && g_pUI->m_WndStation.GetMessage() == WM_LBUTTONDOWN)) &&
			(fRelX >= m_fDragAreaX) &&
			(fRelX <= m_fDragAreaWidth + m_fDragAreaX) && 
			(fRelY >= m_fDragAreaY) && 
			(fRelY <= m_fDragAreaHeight + m_fDragAreaY) )
			return true;
	}

	return KWndWindow::PtInWindow(x, y);
}


void KWndFrame::OnScale(float fScaleX, float fScaleY)
{
	KWndWindow::OnScale(fScaleX, fScaleY);

	m_fDragAreaX   *= fScaleX; 
	m_fDragAreaWidth  *= fScaleX;
	m_fDragAreaY    *= fScaleY;
	m_fDragAreaHeight *= fScaleY;

	m_fFollowMouseOffsetX *= fScaleX;
	m_fFollowMouseOffsetY *= fScaleY;

	OnSize();
}


void KWndFrame::SetFocusWnd(KWndWindow *pWnd)
{
    KWndWindow* pLastFocusWnd = g_pUI->m_WndStation.GetFocusWindow();
    if (pLastFocusWnd == pWnd) 
        return;

	if (pWnd && g_pUI->m_WndStation.GetRootFrame(pWnd) != this)
		return;

	m_pWndFocus = pWnd;

    if (pLastFocusWnd)
    {
        pLastFocusWnd->WndProc(WND_M_KILL_FOCUS, (WPARAM)pWnd, 0);
		g_pUI->m_Script.FireWndEvent(pLastFocusWnd, "OnKillFocus");
    }

    if (pWnd) 
    {
		pWnd->WndProc(WND_M_SET_FOCUS, (WPARAM)pLastFocusWnd, 0);
		g_pUI->m_Script.FireWndEvent(pWnd, "OnSetFocus");
    }
}

KWndWindow* KWndFrame::GetFocusWnd()
{
    return m_pWndFocus;
}

int KWndFrame::IsDragable()
{
	return IsStatus(UI_WND_FRAME_ENABLE_DRAG);
}

int KWndFrame::TryBeginDragWnd(int x, int y)
{
    if (!IsStatus(UI_WND_FRAME_ENABLE_DRAG))
    {
        SetStatus(UI_WND_FRAME_ON_DRAGGING, FALSE);
        return false;
    }

    m_fLastMouseHoldX = (float)x;
    m_fLastMouseHoldY = (float)y;
	float fX = m_fLastMouseHoldX - m_fAbsX;
	float fY = m_fLastMouseHoldY - m_fAbsY;
    if (    
        (fX >= m_fDragAreaX) &&
        (fX <= m_fDragAreaX +m_fDragAreaWidth) && 
        (fY >= m_fDragAreaY) && 
        (fY <= m_fDragAreaY + m_fDragAreaHeight) 
    )
    {
		SetStatus(UI_WND_FRAME_HAS_DRAGGED, FALSE);
        SetStatus(UI_WND_FRAME_ON_DRAGGING, TRUE);
		g_pUI->m_WndStation.SetCapture(this);
        return true;
    }

	SetStatus(UI_WND_FRAME_ON_DRAGGING, FALSE);
	return false;
}


void KWndFrame::OnDragWnd(int x, int y)
{
	int nResult = false;
	float fW = g_pUI->m_WndStation.GetClientWidth();
	float fH = g_pUI->m_WndStation.GetClientHeight();
	if (x < 0 || x > fW || y < 0 || y > fH)	//不能拖到窗口外面
		return;

	SetStatus(UI_WND_FRAME_HAS_DRAGGED, TRUE);
	
	g_pUI->m_Script.FireWndEvent(this, "OnFrameDrag", &nResult);
	if (nResult)
		return;

	SetRelPos(m_fRelX + x - m_fLastMouseHoldX, m_fRelY + y - m_fLastMouseHoldY);

	m_fLastMouseHoldX = (float)x;
	m_fLastMouseHoldY = (float)y;

	g_pUI->m_Script.FireWndEvent(this, "OnFrameDragSetPosEnd");
}

void KWndFrame::OnDragWndEnd(int x, int y)
{
	if (IsStatus(UI_WND_FRAME_ON_DRAGGING))
	{
		g_pUI->m_WndStation.ReleaseCapture();

		SetStatus(UI_WND_FRAME_ON_DRAGGING, FALSE);

		if (IsStatus(UI_WND_FRAME_HAS_DRAGGED))
		{
			float fW = g_pUI->m_WndStation.GetClientWidth();
			float fH = g_pUI->m_WndStation.GetClientHeight();
			if (x < 0)
				x = 0;
			if (x > fW)
				x = (int)(fW);
			if (y <= 0)
				y = 0;
			if (y > fH)
				y = (int)(fH);

			SetStatus(UI_WND_FRAME_HAS_DRAGGED, FALSE);

			g_pUI->m_Script.FireWndEvent(this, "OnFrameDragEnd");
		}
	}
}


void KWndFrame::OnBreathe()
{
	if (IsStatus(UI_WND_FRAME_FADE_IN))
	{
		m_nFadeCount--;
		if (m_nFadeFrame)
		{
			SetAlpha((m_nFadeFrame - m_nFadeCount) * 255 / m_nFadeFrame);
		}
		else
		{
			SetAlpha(255);
		}

		if (m_nFadeCount <= 0)
		{
			SetStatus(UI_WND_FRAME_FADE_IN, FALSE);

			g_pUI->m_Script.FireWndEvent(this, "OnFrameFadeIn");
		}
	}

	if (IsStatus(UI_WND_FRAME_FADE_OUT))
	{
		m_nFadeCount--;
		if (m_nFadeFrame)
		{
			SetAlpha(m_nFadeCount * 255 / m_nFadeFrame);
		}
		else
		{
			SetAlpha(0);
		}
		if (m_nFadeCount <= 0)
		{
			SetStatus(UI_WND_FRAME_FADE_OUT, FALSE);
			g_pUI->m_Script.FireWndEvent(this, "OnFrameFadeOut");
		}
	}

	// TODO : 以后减少调用频率或者加上timeout时间
	if (!IsStatus(UI_WND_FRAME_DISABLE_BREATH))
	{
		g_pUI->m_Script.FireWndEvent(this, "OnFrameBreathe");
	}

	KWndWindow::OnBreathe();
}

void KWndFrame::OnDestroy()
{
	KWndFrame* pFrame = this;

    // save szName before OnDestroy()
	char szName[_countof(m_szName)];
	strncpy(szName, m_szName, _countof(szName));

	int bNeedNullTable = IsStatus(UI_WND_FRAME_DESTROY_LUA_TABLE);
	int nDefaultRef = m_nDefaultTableRef;

	KWndWindow::OnDestroy();

	g_pUI->m_Script.UnbindScript(pFrame, szName, nDefaultRef, bNeedNullTable);
}

void KWndFrame::FireDestroy()
{
	g_pUI->m_Script.FireWndEvent(this, "OnFrameDestroy");
}

void KWndFrame::FireCreate()
{
	g_pUI->m_Script.FireWndEvent(this, "OnFrameCreate");
}

void KWndFrame::SetLuaClass(LPCSTR szClass)
{
	ASSERT(szClass);

	strncpy_s(m_szLuaClass, szClass, _TRUNCATE);
}

LPCSTR KWndFrame::GetLuaClass() const
{
	return m_szLuaClass;
}

LRESULT KWndFrame::OnKeyDown(WPARAM wKey, LPARAM lModifier)
{
	if (wKey == VK_TAB)
	{
        if ((::GetKeyState(VK_SHIFT) & 0x8000) && FocusPrev())
            return TRUE;
        else if (FocusNext())
            return TRUE;
	}
	
    KWndEdit* pEdit = (KWndEdit*)CastWndWindow(GetFocusWnd(), "WndEdit");
    if (pEdit && pEdit->IsPassword())
        return false;

	int nResult = false;
	g_pUI->m_Script.FireWndEvent(this, "OnFrameKeyDown", &nResult);
	return nResult;
}

void KWndFrame::SetRelPos(float fX, float fY)
{
	if (IsStatus(UI_WND_FRAME_FOLLOW_MOUSE_MOVE))
	{
		fX += m_fFollowMouseOffsetX;
		fY += m_fFollowMouseOffsetY;
	}

	KWndWindow::SetRelPos(fX, fY);
}

int KWndFrame::EnableFollowMouseMove(int nEnableFlag)
{
	SetStatus(UI_WND_FRAME_FOLLOW_MOUSE_MOVE, nEnableFlag);
	return true;
}

int KWndFrame::IsFollowMouseMove() const
{
	return IsStatus(UI_WND_FRAME_FOLLOW_MOUSE_MOVE);
}

int KWndFrame::SetFocusByIndex(int nIndex)
{
	if (!IsStatus(UI_WND_FRAME_TAB_CHANGE_FOCUS))
		return false;

	int nMax = (int)m_FocusWndNameList.size();
	if (nMax == 0)
		return false;

	nIndex = nIndex < 0 ? (nMax - 1) : (nIndex < nMax ? nIndex : 0);

	KWndWindow* pFocusWnd = g_pUI->m_WndStation.SearchWindow(this, m_FocusWndNameList[nIndex].c_str());
	if (pFocusWnd)
	{
		SetFocusWnd(pFocusWnd);
		if (IsStatus(UI_WND_FRAME_MOUSE_FOLLOW_FOCUS))
			pFocusWnd->SetCursorAbove();
		return true;
	}

	return false;
}

int KWndFrame::ChangeFocus(FOCUS focus)
{
	int nIndex = -1;

	if (m_pWndFocus)
	{
		nIndex = FindTabListIndexByWndSectionName(m_pWndFocus->GetName());
		ASSERT(nIndex < (int)m_FocusWndNameList.size());
	}

    switch (focus)
    {
    case FOCUS_PREV:
        --nIndex;
        break;
    case FOCUS_NEXT:
        ++nIndex;
        break;
    default:
        ASSERT(0);
        break;
    }
    
	return SetFocusByIndex(nIndex);
}

int KWndFrame::FocusPrev()
{
	if (!ChangeFocus(FOCUS_PREV))
		return false;

    int nTotalCount = (int)m_FocusWndNameList.size();
	for (int i = 0; i < nTotalCount; ++i)
	{
		KWndWindow* pFocusWnd = g_pUI->m_WndStation.GetFocusWindow();
		if (pFocusWnd && pFocusWnd->IsVisible())
			return true;
		if (!ChangeFocus(FOCUS_PREV))
			return false;
	}

	return false;
}

int KWndFrame::FocusNext()
{
	if (!ChangeFocus(FOCUS_NEXT))
		return false;

    int nTotalCount = (int)m_FocusWndNameList.size();
	for (int i = 0; i < nTotalCount; i++)
	{
		KWndWindow *pFocusWnd = g_pUI->m_WndStation.GetFocusWindow();
		if (pFocusWnd && pFocusWnd->IsVisible())
			return true;
		if (!ChangeFocus(FOCUS_NEXT))
			return false;
	}

	return false;
}

int KWndFrame::FocusHome()
{
	return SetFocusByIndex(0);
}

int KWndFrame::FocusEnd()
{
	int nIndex = (int)m_FocusWndNameList.size() - 1;
	if (nIndex >= 0)
		return SetFocusByIndex(nIndex);
	return false;
}


void KWndFrame::AppendFocus(LPCSTR szWndName)
{
	ASSERT(szWndName);

	m_FocusWndNameList.push_back(szWndName);
}


int KWndFrame::FindTabListIndexByWndSectionName(LPCSTR pszString, int nBegineIndex)
{
	if (pszString)
	{
		if (nBegineIndex < 0)
			nBegineIndex = 0;
        std::string strFind(pszString);
		int nCount = (int)m_FocusWndNameList.size();
		for (int nIndex = nBegineIndex; nIndex < nCount; ++nIndex)
		{
			if (m_FocusWndNameList[nIndex] == strFind)
            {
				return nIndex;
            }
		}
	}
	return -1;
}


void KWndFrame::OnShow()
{
	KWndWindow::OnShow();

	g_pUI->m_Script.FireWndEvent(this, "OnFrameShow");
}


void KWndFrame::OnHide()
{
	KWndWindow::OnHide();

	g_pUI->m_Script.FireWndEvent(this, "OnFrameHide");
}

void KWndFrame::SetDragArea(float fX, float fY, float fWidth, float fHeight)
{
	m_fDragAreaX = fX;
	m_fDragAreaY = fY;
	m_fDragAreaWidth = fWidth;
	m_fDragAreaHeight = fHeight;
}

void KWndFrame::SetFollowMouseOffset(float fX, float fY)
{
	m_fFollowMouseOffsetX = fX;
	m_fFollowMouseOffsetY = fY;
}

void KWndFrame::FadeIn(int nLoopCount)
{
	SetStatus(UI_WND_FRAME_FADE_IN, TRUE);
	SetStatus(UI_WND_FRAME_FADE_OUT, FALSE);

	m_nFadeCount = nLoopCount;
	m_nFadeFrame = nLoopCount;

	SetAlpha(0);
}

void KWndFrame::FadeOut(int nLoopCount)
{
	SetStatus(UI_WND_FRAME_FADE_IN, FALSE);
	SetStatus(UI_WND_FRAME_FADE_OUT, TRUE);

	m_nFadeCount = nLoopCount * GetAlpha() / 255;
	m_nFadeFrame = m_nFadeCount;
}

void KWndFrame::ShowWhenUIHide()
{
	SetStatus(UI_WND_FRAME_SHOW_WHEN_HIDE_UI, TRUE);
}

void KWndFrame::HideWhenUIHide()
{
	SetStatus(UI_WND_FRAME_SHOW_WHEN_HIDE_UI, FALSE);
}

int KWndFrame::IsVisibleWhenUIHide()
{
	return IsStatus(UI_WND_FRAME_SHOW_WHEN_HIDE_UI);
}

void KWndFrame::SetStatus(DWORD dwStatus, BOOL bEnable)
{
	if (bEnable)
		m_dwStatus |= dwStatus;
	else
		m_dwStatus &= ~dwStatus;
}

LPCSTR KWndFrame::GetWndType() const
{
	return "WndFrame";
}

}