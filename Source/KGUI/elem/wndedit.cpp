////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : wndedit.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-7-18 10:43:13
//  Comment     : 
//                Edited by Chen Jie 2006-12-30
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "engine/text.h"
#include "./wndmessage.h"
#include "./wndedit.h"
#include "./Components/itemhandle.h"
#include "./Components/itemtext.h"
#include "../driver/kcolor.h"
#include "../text/ktext.h"
#include "../common/ksmallobjectpool.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"


//#define KWNDEDIT_DEBUG_TRACE

namespace
{
	KSmallObjectPool	g_Allocator("KWndEdit", sizeof(UI::KWndEdit));
}

namespace UI
{

#define	MULTI_LINE	(m_Flag & WNDEDIT_ES_MULTI_LINE)

#ifdef KGUI_LUA_LOADER
int KWndEdit::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KWndWindow *pWnd = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pWnd = new(std::nothrow) KWndEdit;
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
KWndEdit::KWndEdit()
: m_pTextHandle(NULL)
, m_pCursor(NULL)
, m_dwFontColor(0)
, m_dwStatus(WNDEDIT_F_LIMIT_MULTIBYTE)
, m_dwSelStartPos(0)
, m_dwSelEndPos(0)
, m_dwCaretPos(0)
, m_dwCaretTickCount(0)
, m_nCaretAfterFlag(false)
, m_nTextLimit(-1)
, m_dwFocusBgColor(0)
, m_dwSelectBgColor(0)
, m_nFontScheme(0)
, m_nSelectFontScheme(0)
, m_nCaretFontScheme(0)
, m_fFontSpacing(0.0f)
, m_fRowSpacing(0.0f)
{
}

KWndEdit::~KWndEdit()
{
	ClearText();
	SAFE_RELEASE(m_pTextHandle);
    SAFE_RELEASE(m_pCursor);
}

void* KWndEdit::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KWndEdit::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KWndEdit::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KWndEdit::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KWndEdit::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;
    
    int  nMultiFlag          = 0;
    int  nPswFlag            = 0;
    int  nType               = 0;
    int  nFocusBgColorAlpha  = 0;
    int  nSelectBgColorAlpha = 0;
    char szFocusBgColor[KD_SMALL_BUFFER_SIZE];
    char szSelectBgColor[KD_SMALL_BUFFER_SIZE];

    KItemNull *pItem     = NULL;
    KItemNull *pItemNull = NULL;
    float      fWidth    = 0.0f;
    float      fHeight   = 0.0f;

    KColorSchemeMgr& ColorSchemeMgr = g_pUI->m_ColorSchemeMgr;
    
    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("MultiLine",          emVAR_TYPE_INT,     false, &nMultiFlag,          0),
        KLUA_TABLE_FIELD("Password",           emVAR_TYPE_INT,     false, &nPswFlag,            0),
        KLUA_TABLE_FIELD("type",               emVAR_TYPE_INT,     0,     &nType,               0),
        KLUA_TABLE_FIELD("MaxLen",             emVAR_TYPE_INT,     0,     &m_nTextLimit,       0),
        KLUA_TABLE_FIELD("font",               emVAR_TYPE_INT,     0,     &m_nFontScheme,       0),
        KLUA_TABLE_FIELD("SelFont",            emVAR_TYPE_INT,     0,     &m_nSelectFontScheme,    0),
        KLUA_TABLE_FIELD("CaretFont",          emVAR_TYPE_INT,     0,     &m_nCaretFontScheme,  0),
        KLUA_TABLE_FIELD("FocusBgColor",       emVAR_TYPE_STRING,  "",    szFocusBgColor,       _countof(szFocusBgColor)),
        KLUA_TABLE_FIELD("FocusBgColorAlpha",  emVAR_TYPE_INT,     255,   &nFocusBgColorAlpha,  0),
        KLUA_TABLE_FIELD("SelectBgColor",      emVAR_TYPE_STRING,  "",    szSelectBgColor,      _countof(szSelectBgColor)),
        KLUA_TABLE_FIELD("SelectBgColorAlpha", emVAR_TYPE_INT,     255,   &nSelectBgColorAlpha, 0),
        KLUA_TABLE_FIELD("FontSpacing",        emVAR_TYPE_FLOAT,   0,     &m_fFontSpacing,      0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nRetCode = KWndWindow::Init(pLua, nTableIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    g_SetBitFlag(nMultiFlag & !nPswFlag, WNDEDIT_ES_MULTI_LINE, m_dwStatus);  // 密码模式忽略多行选项
    g_SetBitFlag(nPswFlag, WNDEDIT_ES_MASK_CHARACTER, m_dwStatus);
    SetType(nType);

    if (szFocusBgColor[0])
    {
        int nColorScheme = ColorSchemeMgr.FindScheme(ATL::CA2W(szFocusBgColor, GetCodePage()));
        if (ColorSchemeMgr.IsExistColor(nColorScheme))
        {
            m_dwFocusBgColor = ColorSchemeMgr.GetColor(nColorScheme);
        }
    }
    m_dwFocusBgColor = AddAlpha(m_dwFocusBgColor, nFocusBgColorAlpha);

    if (szSelectBgColor[0])
    {
        int nColorScheme = ColorSchemeMgr.FindScheme(ATL::CA2W(szSelectBgColor, GetCodePage()));
        if (ColorSchemeMgr.IsExistColor(nColorScheme))
        {
            m_dwSelectBgColor = ColorSchemeMgr.GetColor(nColorScheme);
        }
    }
    m_dwSelectBgColor = AddAlpha(m_dwSelectBgColor, nSelectBgColorAlpha);



    // TODO:tongxuehu 原Bind + CreateHandle中内容,为了能先跑起来，先调旧的方法
    // Bind
    ASSERT(m_pTextHandle == NULL && "历史实例未释放");
    nRetCode = KItemText::Create((IKUIObject **)&m_pCursor, NULL, 0);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_pCursor);
    m_pCursor->SetName(K_INPUT_CURSOR_NAME);
    m_pCursor->SetText(K_INPUT_CURSOR);
    m_pCursor->SetRichText(false);
    m_pCursor->SetHAlign(1);
    m_pCursor->SetVAlign(2);
    m_pCursor->SetFontSpacing(0.0f);
    m_pCursor->SetAbsPos(m_fAbsX, m_fAbsY);
    m_pCursor->SetFontScheme(m_nCaretFontScheme);
    fHeight = m_pCursor->GetSingleTextHeight();
    m_pCursor->SetSize(fWidth, fHeight);

    // CreateHandle
    nRetCode = KItemHandle::Create((IKUIObject **)&m_pTextHandle, NULL, 0);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(m_pTextHandle);

    m_pTextHandle->SetSize(m_fWidth, m_fHeight);
    m_pTextHandle->SetHandleStyle(ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM);
    
    nRetCode = KItemNull::Create((IKUIObject **)&pItemNull, NULL, 0);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(pItemNull);

    nRetCode = pItemNull->SetName(K_INPUT_TERM_NAME);
    KGLOG_PROCESS_ERROR(nRetCode);

    fHeight  = GetFontHeight(m_nFontScheme);
    nRetCode = pItemNull->SetSize(fWidth, fHeight);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pTextHandle->AppendItem(pItemNull);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_pTextHandle->SetAbsPos(m_fAbsX, m_fAbsY);

// Exit1:
	nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_RELEASE(m_pTextHandle);
        SAFE_RELEASE(m_pCursor);
        SAFE_RELEASE(pItemNull);
    }

	return nResult;
}
#endif

int KWndEdit::CreateCursorHandle()
{
	int nResult = false;
	int nRetCode = false;
    KItemTextData TextData(NULL);
    KItemNull *pItem = NULL;

    ASSERT(m_pCursor == NULL);

	m_pCursor = KItemText::Create(TextData);
	KG_PROCESS_ERROR(m_pCursor);

    m_pCursor->SetName(K_INPUT_CURSOR_NAME);
    m_pCursor->SetText(K_INPUT_CURSOR);
    m_pCursor->SetRichText(false);
    m_pCursor->SetHAlign(1);
    m_pCursor->SetVAlign(2);
    m_pCursor->SetFontSpacing(0.0f);
    m_pCursor->SetAbsPos(m_fAbsX, m_fAbsY);
    m_pCursor->SetFontScheme(m_nCaretFontScheme);
    m_pCursor->SetSize(0.0f, m_pCursor->GetSingleTextHeight());

	SetStatus(WNDEDIT_F_CURSOR_MOVED, TRUE);

    nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_RELEASE(m_pCursor);
    }

	return nResult;
}

void KWndEdit::SetSize(float fWidth, float fHeight)
{
	KWndWindow::SetSize(fWidth, fHeight);

	if (m_pTextHandle)
	{
		m_pTextHandle->SetSize(fWidth, fHeight);
		SetStatus(WNDEDIT_F_CURSOR_MOVED, TRUE);
		SetStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED, TRUE);
	}
}

int KWndEdit::CreateTextHandle()
{
	int nResult = false;
    int nRetCode = false;
	int nValue = 0;
    float fWidth = 0.0f;
    float fHeight = 0.0f;
    KItemHandleData HandleData(this);
    KItemNull* pItemNull = NULL;
    KItemNullData ItemNullData(NULL);

	HandleData.fWidth = m_fWidth;
	HandleData.fHeight = m_fHeight;
	HandleData.nHandleType = ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM;
	HandleData.nAutoSize = false;
	HandleData.nPixelScroll = !IsStatus(WNDEDIT_ES_MULTI_LINE);
	HandleData.fRowSpacing = m_fRowSpacing;
	HandleData.nControlShow = TRUE;

	m_pTextHandle = KItemHandle::Create(HandleData);
	KG_PROCESS_ERROR(m_pTextHandle);
	m_pTextHandle->SetDisableExtent(true);

    pItemNull = KItemNull::Create(ItemNullData);
    KG_PROCESS_ERROR(pItemNull);

    nRetCode = pItemNull->SetName(K_INPUT_TERM_NAME);
    KG_PROCESS_ERROR(nRetCode);

    fHeight  = GetFontHeight(m_nFontScheme);
    nRetCode = pItemNull->SetSize(fWidth, fHeight);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pTextHandle->AppendItem(pItemNull);
    KG_PROCESS_ERROR(nRetCode);
    pItemNull = NULL;

	m_pTextHandle->SetAbsPos(m_fAbsX, m_fAbsY);

	nResult = true;
Exit0:
	if (!nResult)
		SAFE_RELEASE(m_pTextHandle);

    SAFE_RELEASE(pItemNull);
	return nResult;
}

BOOL KWndEdit::SetCurSel(int nStart, int nEnd, BOOL bMultiByte)
{
	int nResult = FALSE;
	int nLen = GetTextLength(bMultiByte);
	if (nStart < 0)
		nStart = nLen;
	if (nEnd < 0)
		nEnd = nLen;
	nStart = min(max(nStart, 0), nLen);
	nEnd = min(max(nEnd, 0), nLen);
	if (nStart > nEnd)
	{
		int nTemp = nEnd;
		nEnd = nStart;
		nStart = nTemp;
	}
	if (bMultiByte)
	{
		char chEnd;
		char * pszTemp = new char[nLen + 1];
		KGLOG_PROCESS_ERROR(pszTemp);
		strncpy(pszTemp, ATL::CW2A(m_strText.c_str(), GetCodePage()), nLen + 1);
		pszTemp[nLen] = '\0';
		chEnd = *(pszTemp + nStart);
		*(pszTemp + nStart) = '\0';
		nStart = (int)wcslen(ATL::CA2W(pszTemp));
		*(pszTemp + nStart) = chEnd;
		*(pszTemp + nEnd) = '\0';
		nEnd = (int)wcslen(ATL::CA2W(pszTemp));
		SAFE_DELETE_ARRAY(pszTemp);
	}

	SetCaretPos(nEnd, TRUE);
	SetSelStartPos(nStart, TRUE);
	SetSelEndPos(nEnd, TRUE);

	nResult = TRUE;
Exit0:
	return nResult;
}

int KWndEdit::SetType(int nType)
{
	int nResult = false;

	KG_PROCESS_ERROR(nType >= 0 && nType <= 2);

	SetStatus(WNDEDIT_ES_CHAR_TYPE_FILTER, FALSE);
	switch (nType)
	{
	case 0:
		break;
	case 1:
		SetStatus(WNDEDIT_ES_ENGLISH_AVAILABLE, TRUE);
		break;
	case 2:
		SetStatus(WNDEDIT_ES_IME_AVAILABLE, TRUE);
		break;
	}

	nResult = true;
Exit0:
	return nResult;
}

int	KWndEdit::SetLimit(int nLimit)
{
	m_nTextLimit = nLimit;
	return true;
}

int	KWndEdit::GetLimit()
{
	return m_nTextLimit;
}

void KWndEdit::SetLimitMultiByte(int nMultiByte)
{
	SetStatus(WNDEDIT_F_LIMIT_MULTIBYTE, nMultiByte);
}

int	KWndEdit::IsLimitMultiByte()
{
	return IsStatus(WNDEDIT_F_LIMIT_MULTIBYTE);
}

int	KWndEdit::IsPassword()
{
	return IsStatus(WNDEDIT_ES_MASK_CHARACTER);
}

//--------------------------------------------------------------------------
//	功能：禁止或者允许窗口被操作
//--------------------------------------------------------------------------
void KWndEdit::Enable(int bEnable)
{
	if (!bEnable && IsStatus(WNDEDIT_F_HAVE_FOCUS))
		g_pUI->m_WndStation.SetFocusWindow(NULL);

	KWndWindow::Enable(bEnable);

	SetStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED, TRUE);
}

//--------------------------------------------------------------------------
//	功能：窗口函数（处理消息）
//--------------------------------------------------------------------------
LRESULT KWndEdit::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT	lRet = FALSE;

	if (KWndWindow::WndProc(uMsg, wParam, lParam))
		return true;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		lRet = OnLButtonDown((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		break;
	case WM_RBUTTONDOWN:
		lRet = OnRButtonDown((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		break;
    case WM_LBUTTONUP:
		lRet = OnLButtonUp((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
		break;
    case WM_RBUTTONUP:
		lRet = OnRButtonUp((float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEMOVE:
		lRet = OnMouseMove(wParam, (float)GET_X_LPARAM(lParam), (float)GET_Y_LPARAM(lParam));
        break;
	case WM_IME_CHAR:
		lRet = OnImeChar(wParam);
		break;
	case WM_CHAR:
		lRet = OnChar(wParam);
		break;
	case WM_KEYDOWN:
		lRet = OnKeyDown(wParam, lParam);
		break;
	case WM_SYSKEYDOWN:
		lRet = OnSysKeyDown(wParam, lParam);
		break;
	case WND_M_SET_FOCUS:
		lRet = OnSetFocus();
		break;
	case WND_M_KILL_FOCUS:
		lRet = OnKillFocus();
		break;
	case WM_MOUSEWHEEL:
		lRet = OnMouseWheel(wParam);
		break;
    case WM_LBUTTONDBLCLK:
		lRet = OnLButtonDblClk();
		break;
	case WND_M_CAPTURECHANGED:
		lRet = TRUE;
		break;
	default:
		break;
	}

	return lRet;
}

int KWndEdit::GetTextPos(float fX, float fY, DWORD *pdwRetPos, int *pnRetAfterFlag)
{
    int         nResult         = false;
	DWORD       dwRetPos        = 0;
    int         nRetAfterFlag   = false;
	int         nCount          = 0;
	int         nInThisRow      = false;
    float       fRowEnd         = min(fY, m_fAbsY) - 100;
    KItemNull  *pItem           = NULL;

    KG_PROCESS_ERROR(pdwRetPos);
    KG_PROCESS_ERROR(pnRetAfterFlag);
	KG_PROCESS_ERROR(m_pTextHandle);

    if (!IsStatus(WNDEDIT_ES_MULTI_LINE))
    {
        //单行，里面没有换行符和制表符，插入的时候注意过滤掉换行符和制表
        nInThisRow = true;
    }

	nCount = m_pTextHandle->GetItemCount();
	for (int i = 0; i < nCount; ++i)
	{
		pItem = m_pTextHandle->GetItemByIndex(i);
		ASSERT(pItem);
		float fAbsX, fAbsY;
		float fWidth, fHeight;
		pItem->GetAbsPos(fAbsX, fAbsY);
		pItem->GetSize(fWidth, fHeight);

		if (IsStatus(WNDEDIT_ES_MULTI_LINE))
		{
			//多行，里面没有制表符，插入的时候注意过滤掉制表符
			if (UI_FLOAT_LARGE(fAbsY + fHeight, fRowEnd))
			{
				if (nInThisRow)
				{
					//这一行里面所有的东西都找完了还没有找到，就应该位于上一行的结尾。
                    KItemNull *pItemPrev = NULL;
                    KItemText *pItemPrevText = NULL;

                    dwRetPos = i - 1;
                    nRetAfterFlag = true;

                    // If the last char is '\n', put the cursor before it.
                    pItemPrev = m_pTextHandle->GetItemByIndex(dwRetPos);
                    ASSERT(pItemPrev);
                    pItemPrevText = g_ItemCast<KItemText>(pItemPrev);
                    if (pItemPrevText)
                    {
                        LPCWSTR wszText = pItemPrevText->GetText();
                        if (0 == wcscmp(wszText, L"\n"))
                        {
                            nRetAfterFlag = false;
                        }
                    }

					break;
				}
				
				//新的RowEnd
				fRowEnd = fAbsY + fHeight;
				if (fY <= fRowEnd)
				{
					//应该是这一行的
					nInThisRow = true;
				}
			}
		}

		if (nInThisRow)
		{
			if (fX < fAbsX + fWidth / 2.0f)
			{
				//找到，之前
                dwRetPos = i;
                nRetAfterFlag = false;
				break;
			}

			if (fX < fAbsX + fWidth)
			{
				//找到，之后
				dwRetPos = i + 1;

                KItemNull *pItemNext = m_pTextHandle->GetItemByIndex(i + 1);
                if (pItemNext)
                {
                    float fNextX;
                    float fNextY;

                    pItemNext->GetAbsPos(fNextX, fNextY);

                    if (fNextX <= fAbsX)
                    {
                        --dwRetPos;
                        nRetAfterFlag = true;
                    }
                }

				break;
			}
		}

        dwRetPos = i;
        nRetAfterFlag = false;
	}

    *pdwRetPos = dwRetPos;
    *pnRetAfterFlag = nRetAfterFlag;
    nResult = true;

Exit0:
	return nResult; 
}

int	KWndEdit::GetTextRowHeight(int nIndex, float &fRowHeight)
{
	int nResult = false;
	KItemNull *pItemMe = NULL;
	float fAbsX, fAbsY;
	float fAbsXMe, fAbsYMe;
	float fW, fH;
	KG_PROCESS_ERROR(m_pTextHandle);
	pItemMe = m_pTextHandle->GetItemByIndex(nIndex);
	KG_PROCESS_ERROR(pItemMe);
	pItemMe->GetAbsPos(fAbsXMe, fAbsYMe);
	pItemMe->GetSize(fW, fH);
	fAbsYMe += fH;
	for(int i = nIndex - 1; i >= 0; --i)
	{
		KItemNull *pItem = m_pTextHandle->GetItemByIndex(i);
		pItem->GetAbsPos(fAbsX, fAbsY);
		pItem->GetSize(fW, fH);
		fAbsY += fH;

		if (UI_FLOAT_SMALL(fAbsY, fAbsYMe))
		{
			fRowHeight = fAbsYMe - fAbsY - m_pTextHandle->GetRowSpacing();
			goto Exit1;
		}
	}

	m_pTextHandle->GetItemStartRelPos(fAbsX, fAbsY);
	fRowHeight = fAbsYMe - m_fAbsY - fAbsY;

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int	KWndEdit::IsTextRowVisible(int nIndex)
{
	int nResult = false;
	KItemNull *pItemMe = NULL;
	float fAbsX, fAbsY;
	float fAbsXMe, fAbsYMe;
	float fW, fH;
	int nCount = 0;
	KG_PROCESS_ERROR(m_pTextHandle);

	nCount = m_pTextHandle->GetItemCount();
	KG_PROCESS_ERROR(nIndex >= 0 && nIndex < nCount);
	pItemMe = m_pTextHandle->GetItemByIndex(nIndex);
	pItemMe->GetAbsPos(fAbsXMe, fAbsYMe);
	pItemMe->GetSize(fW, fH);
	fAbsYMe += fH;
	for(int i = nIndex; i >= 0; --i)
	{
		KItemNull *pItem = m_pTextHandle->GetItemByIndex(i);
		pItem->GetAbsPos(fAbsX, fAbsY);
		pItem->GetSize(fW, fH);
		fAbsY += fH;
		if (!(UI_FLOAT_EQUAL(fAbsY, fAbsYMe)))
			break;
		KG_PROCESS_ERROR(pItem->IsVisible());
	}
	for(int i = nIndex; i < nCount; ++i)
	{
		KItemNull *pItem = m_pTextHandle->GetItemByIndex(i);
		pItem->GetAbsPos(fAbsX, fAbsY);
		pItem->GetSize(fW, fH);
		fAbsY += fH;
		if (!(UI_FLOAT_EQUAL(fAbsY, fAbsYMe)))
			break;
		KG_PROCESS_ERROR(pItem->IsVisible());
	}

	nResult = true;
Exit0:
	return nResult;
}

LRESULT	KWndEdit::OnLButtonDown(float fX, float fY)
{
    LRESULT lResult    = FALSE;
    int     nRetCode   = false;
    DWORD   dwPos      = 0;
    int     nAfterFlag = false;

    KG_PROCESS_ERROR(m_pTextHandle);

	nRetCode = GetTextPos(fX, fY, &dwPos, &nAfterFlag);
    KG_PROCESS_ERROR(nRetCode);

    SetCaretPos(dwPos, nAfterFlag);
	SetSelStartPos(dwPos, nAfterFlag);
	SetSelEndPos(dwPos, nAfterFlag);

	SetStatus(WNDEDIT_F_SEL_STARTED, TRUE);

	if (IsVisible() && g_pUI->m_WndStation.GetFocusWindow() != this)
		g_pUI->m_WndStation.SetFocusWindow(this);

	g_pUI->m_WndStation.SetCapture(this);

    lResult = TRUE;
Exit0:
	return lResult;
}


LRESULT	KWndEdit::OnRButtonDown(float fX, float fY)
{
	//TODO:通知脚本右键按下
	return TRUE;
}


LRESULT	KWndEdit::OnLButtonUp(float fX, float fY)
{
	//设置光标位置
	if (g_pUI->m_WndStation.GetCapture() == this)
		g_pUI->m_WndStation.ReleaseCapture();

	return TRUE;
}


LRESULT	KWndEdit::OnRButtonUp(float fX, float fY)
{
	//TODO:通知脚本右键松开,可以考虑做一个鼠标单击事件
	return TRUE;
}


LRESULT	KWndEdit::OnMouseMove(WPARAM wParam, float fX, float fY)
{
	if (IsStatus(WNDEDIT_F_SEL_STARTED) && (wParam & MK_LBUTTON))
	{
		//设置光标位置
		DWORD dwPos      = 0;
        int   nAfterFlag = false;

        GetTextPos(fX, fY, &dwPos, &nAfterFlag);;
		SetCaretPos(dwPos, nAfterFlag);
		SetSelEndPos(dwPos, nAfterFlag);
	}

	return TRUE;
}


LRESULT	KWndEdit::OnChar(WPARAM wParam)
{
    static char s_szCharBuffer[4];      // 用于解决如极品五笔等不规范的中文输入法

    LRESULT lRet = FALSE;

    if (IsStatus(WNDEDIT_F_HAVE_FOCUS) && (!IsDisable()))
	{
#ifndef _UNICODE
        if (wParam > 0x80 || s_szCharBuffer[0] != '\0')
        {
            ASSERT(wParam < 0xFF);

            if (s_szCharBuffer[0] == '\0')
            {
                s_szCharBuffer[0] = (char)wParam;
            }
            else
            {
                s_szCharBuffer[1] = (char)wParam;

                ATL::CA2W wszChar(s_szCharBuffer, GetCodePage());

                memset(s_szCharBuffer, 0, sizeof(s_szCharBuffer));

                AppendChar(wszChar[0]);
            }
        }
        else
        {
            char szTemp[4] = { 0 };

            szTemp[0] = (char)(wParam & 0xFF);

            ATL::CA2W wszChar(szTemp, GetCodePage());

            AppendChar(wszChar[0]);
        }
#endif

		lRet = TRUE;
	}
	return lRet;
}


LRESULT	KWndEdit::OnImeChar(WPARAM wParam)
{
	LRESULT lRet = FALSE;

	if (IsStatus(WNDEDIT_F_HAVE_FOCUS) && (!IsDisable()))
	{
#ifndef _UNICODE
        char szTemp[4];
        if (wParam > 0x80)
        {
            szTemp[0] = (char)(wParam >> 8);
            szTemp[1] = (char)(wParam & 0xFF);
        }
        else
        {
            ASSERT(wParam < 0xFF);

            szTemp[0] = (char)(wParam);
            szTemp[1] = '\0';
        }

        szTemp[2] = '\0';
        szTemp[3] = '\0';

        ATL::CA2W wszChar(szTemp, GetCodePage());
        wParam = wszChar[0];
#endif

		AppendChar((WCHAR)wParam);
		lRet = TRUE;
	}
	return lRet;
}

int KWndEdit::Backspace()
{
    int nResult = false;

    if (m_dwSelStartPos == m_dwSelEndPos && m_dwCaretPos > 0)
    {
        SetSelStartPos(GetCaretPos() - 1, false);
        SetSelEndPos(GetCaretPos(), false);
    }
    if (m_dwSelStartPos != m_dwSelEndPos)
    {
        nResult = DeleteString();
    }
    return nResult;
}

int KWndEdit::AppendChar(WCHAR wcChar)
{
	if (wcChar > 0xFF)
	{
		//汉字???
		if (IsStatus(WNDEDIT_ES_IME_AVAILABLE))
		{
			if (m_dwSelStartPos != m_dwSelEndPos)
				DeleteString();
			return AppendFilteredChar(wcChar);
		}
        else
        {
		    return false;
        }
	}

	if (wcChar == VK_BACK)		//退格
	{
        Backspace();
	}
	else if (wcChar >= L' ' && wcChar <= L'~')
	{
		if (IsStatus(WNDEDIT_ES_CHAR_TYPE_FILTER) || (wcChar >= L'0' && wcChar <= L'9'))
		{
			if (m_dwSelStartPos != m_dwSelEndPos)
				DeleteString();
			return AppendFilteredChar(wcChar);
		}
	}
	else if (wcChar >= 0x80 && wcChar < 0xFF && 
		wcChar != 0x81 && wcChar != 0x8D && wcChar != 0x8F && wcChar != 0x90 && wcChar != 0x9D)
	{
		//这些特殊的符号应该算作汉字吧。
		if (IsStatus(WNDEDIT_ES_IME_AVAILABLE))
		{
			if (m_dwSelStartPos != m_dwSelEndPos)
				DeleteString();
			return AppendFilteredChar(wcChar);
		}
	}
	else if (wcChar == L'\n' && IsStatus(WNDEDIT_ES_MULTI_LINE))
    {
        return AppendFilteredChar(wcChar);
	}
    else if (wcChar == L'\r'  && IsStatus(WNDEDIT_ES_MULTI_LINE))
    {
        // skip the '\r'
        return true;
    }

	return false;
}

//--------------------------------------------------------------------------
//  功能：在字符串里执行插入字符操作
//  参数：wcChar 已经被过滤后的字符
//  返回：字符串是否被改变
//--------------------------------------------------------------------------
int	KWndEdit::AppendFilteredChar(WCHAR wcChar)
{
	int     nResult  = false;
    int     nRetCode = false;
    WCHAR   wszTemp[2];

    KG_ASSERT_EXIT(sizeof(DWORD) == 4);
    KG_ASSERT_EXIT(sizeof(WCHAR) == 2);
    KG_PROCESS_ERROR(m_pTextHandle);

	KG_PROCESS_ERROR(wcChar != L'\0');

	if (!IsStatus(WNDEDIT_ES_MULTI_LINE))
	{
		KG_PROCESS_ERROR(wcChar != L'\n');
	}

	wszTemp[0] = wcChar;
	wszTemp[1] = L'\0';

	if (IsStatus(WNDEDIT_F_LIMIT_MULTIBYTE))
	{
		nRetCode = GetTextLength(TRUE);
		int nAppend = (int)wcslen(wszTemp);
		KG_PROCESS_ERROR(m_nTextLimit < 0 || nRetCode + nAppend <= m_nTextLimit);
	}
	else
	{
		nRetCode = GetTextLength(FALSE);
		KG_PROCESS_ERROR(m_nTextLimit < 0 || nRetCode < m_nTextLimit);
	}

    if (IsStatus(WNDEDIT_ES_MASK_CHARACTER))
    {
        nRetCode = InsertObject(K_INPUT_MASK_NAME, K_INPUT_MASK, (int)(wcChar));
        KG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = InsertObject(K_INPUT_TEXT_NAME, wszTemp, 0);
        KG_PROCESS_ERROR(nRetCode);
    }

	nResult = true;
Exit0:
	return nResult;
}

int KWndEdit::InsertObject(LPCSTR szTypeName, LPCWSTR wszShowText, int nUserData)
{
    int nResult = false;
    int nRetCode = false;
    int nTypeNameLen = 0;
    int nShowTextLen = 0;
    DWORD dwCaretPos = GetCaretPos();
    KItemText* pItemText = NULL;
    KItemTextData ItemTextData(NULL);

    KGLOG_PROCESS_ERROR(m_pTextHandle);

    KGLOG_PROCESS_ERROR(szTypeName);

    nTypeNameLen = (int)strlen(szTypeName);
    KGLOG_PROCESS_ERROR((nTypeNameLen + sizeof('\0')) <= ITEM_NAME_MAX_SIZE);

    KGLOG_PROCESS_ERROR(wszShowText);
	if (IsStatus(WNDEDIT_F_LIMIT_MULTIBYTE))
	{
		nShowTextLen = (int)wcslen(wszShowText);
		nRetCode = GetTextLength(TRUE);
		KG_PROCESS_ERROR(m_nTextLimit < 0 || nRetCode + nShowTextLen <= m_nTextLimit);
	}
	else
	{
		nShowTextLen = (int)strlen(ATL::CW2A(wszShowText, GetCodePage()));
		nRetCode = GetTextLength(FALSE);
		KG_PROCESS_ERROR(m_nTextLimit < 0 || nRetCode + nShowTextLen <= m_nTextLimit);
	}

    ItemTextData.fFontSpacing = m_fFontSpacing;
    ItemTextData.nFontScheme = m_nFontScheme;
	ItemTextData.dwColor = m_dwFontColor;

    pItemText = KItemText::Create(ItemTextData);
    KGLOG_PROCESS_ERROR(pItemText);

    nRetCode = pItemText->SetName(szTypeName);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pItemText->SetText(wszShowText);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pItemText->SetUserDataInt(nUserData, 0);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pItemText->SetSelected(false);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = pItemText->SetSelBgColor(m_dwSelectBgColor);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_pTextHandle->InsertItem(dwCaretPos, pItemText);
    KG_PROCESS_ERROR(nRetCode);

    pItemText = NULL;

    SetCaretPos(m_dwCaretPos + 1, false);

    SetStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED, TRUE);
    SetStatus(WNDEDIT_F_STR_TRANSFORMED, FALSE);

	g_pUI->m_Script.FireWndEvent(this, "OnEditChanged");

    nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_RELEASE(pItemText);
    }
    return nResult;
}

int KWndEdit::GetTextLuaStruct(Lua_State* L)
{
    int nResult = 0;
    int nRetCode = false;
    int nOrigTop = 0;
    int nIndex = 0;
    int nTable = 0;
    int nSubTable = 0;
    int nItemCount = 0;
    KItemNull* pItem = NULL;
    KItemText* pItemText = NULL;
    LPCSTR szName  = NULL;
    std::wstring strTemp;

    KGLOG_PROCESS_ERROR(m_pTextHandle);

    nOrigTop = Lua_GetTopIndex(L);

    Lua_NewTable(L);

    nTable = Lua_GetTopIndex(L);
    KGLOG_PROCESS_ERROR(nTable > 0);

    nItemCount = m_pTextHandle->GetItemCount();

    for (int i = 0; i < (nItemCount - 1); ++i)
    {
        pItem = m_pTextHandle->GetItemByIndex(i);
        KGLOG_PROCESS_ERROR(pItem);

        pItemText = g_ItemCast<KItemText>(pItem);
        KGLOG_PROCESS_ERROR(pItemText);

        szName = pItemText->GetName();
        KGLOG_PROCESS_ERROR(szName);

        if (strcmp(szName, K_INPUT_TEXT_NAME) == 0)      // Combine text.
        {
            strTemp += pItemText->GetText();
            continue;
        }

        if (strcmp(szName, K_INPUT_CURSOR_NAME) == 0)    // Skip cursor.
            continue;

        // If we have text already, then output them before any object.
        if (!strTemp.empty())
        {
            int nTextTable = 0;
            ATL::CW2A szText(strTemp.c_str(), GetCodePage());

            Lua_PushNumber(L, ++nIndex);
            Lua_NewTable(L);

            nTextTable = Lua_GetTopIndex(L);
            KGLOG_PROCESS_ERROR(nTextTable > 0);

            Lua_PushString(L, "type");
            Lua_PushString(L, "text");
            Lua_SetTable(L, nTextTable);

            Lua_PushString(L, "text");
            Lua_PushString(L, szText);
            Lua_SetTable(L, nTextTable);

            Lua_SetTable(L, nTable);

            strTemp.clear();
        }

        // Output object
		if (strcmp(szName, K_INPUT_OBJ_NAME) == 0)
        {
            int     nObjectTable = 0;
            int     nUserData   = LUA_NOREF;

            nRetCode = pItemText->GetUserDataInt(&nUserData, NULL);
            KGLOG_PROCESS_ERROR(nRetCode);
			KGLOG_PROCESS_ERROR(nUserData != LUA_NOREF);

            Lua_PushNumber(L, ++nIndex);
			lua_rawgeti(L, LUA_REGISTRYINDEX, nUserData);

            Lua_SetTable(L, nTable);
        }
    }

    //Does we left any text?
    if (!strTemp.empty())
    {
        int nTextTable = 0;

        Lua_PushNumber(L, ++nIndex);
        Lua_NewTable(L);

        nTextTable = Lua_GetTopIndex(L);
        KGLOG_PROCESS_ERROR(nTextTable > 0);

        Lua_PushString(L, "type");
        Lua_PushString(L, "text");
        Lua_SetTable(L, nTextTable);

        Lua_PushString(L, "text");
        Lua_PushString(L, ATL::CW2A(strTemp.c_str(), GetCodePage()));
        Lua_SetTable(L, nTextTable);

        Lua_SetTable(L, nTable);

        strTemp.clear();
    }

    nResult = 1;

Exit0:
    if (!nResult)
    {
        Lua_SetTopIndex(L, nOrigTop);
    }

    return nResult;
}


LRESULT	KWndEdit::OnSetFocus()
{
    KWndStation& WndStation = g_pUI->m_WndStation;	
    
    if (IsDisable())
	{
		WndStation.SetFocusWindow(GetParent());
		return TRUE;
	}

    if (IsStatus(WNDEDIT_ES_IME_AVAILABLE))
	{
        ImmAssociateContext(g_pUI->m_hWnd, WndStation.GetHIMC());
	}

	SetStatus(WNDEDIT_F_HAVE_FOCUS, TRUE);
	SetStatus(WNDEDIT_F_CURSOR_MOVED, TRUE);

	if (m_pCursor)
		m_pCursor->SetAlpha(255);
	m_dwCaretTickCount = KG_GetTickCount();

	CorrectImePos();

	return TRUE;
}


LRESULT	KWndEdit::OnKillFocus()
{
    if (IsStatus(WNDEDIT_ES_IME_AVAILABLE))
    {
        KWndStation& WndStation = g_pUI->m_WndStation;	

        ImmAssociateContext(g_pUI->m_hWnd, NULL);
    }

	SetStatus(WNDEDIT_F_HAVE_FOCUS, FALSE);
	CancelSelect();

	return TRUE;
}


void KWndEdit::CorrectImePos()
{
	if (!m_pTextHandle)
        return;

    if (!m_pCursor)
        return;

	float fX, fY;
    m_pCursor->GetAbsPos(fX, fY);
	::SetCaretPos((int)fX, (int)fY);

    HIMC hImc = ImmGetContext(g_pUI->m_hWnd);
	if (hImc)
	{
        COMPOSITIONFORM cf;

        cf.dwStyle = CFS_POINT;
        cf.ptCurrentPos.x = (LONG)fX;
        cf.ptCurrentPos.y = (LONG)fY;
        ImmSetCompositionWindow(hImc, &cf);

        ImmReleaseContext(g_pUI->m_hWnd, hImc);
    }
}


LRESULT	KWndEdit::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}


LRESULT	KWndEdit::OnMouseWheel(WPARAM wParam)
{
	LRESULT lRet = FALSE;

	if (IsStatus(WNDEDIT_F_HAVE_FOCUS) && !IsDisable())
	{
		int const zDelta = GET_Y_LPARAM(wParam);
		if (zDelta)
			lRet = OnKeyDown(zDelta < 0 ? VK_DOWN : VK_UP, 0);
	}

	return lRet;
}


LRESULT	KWndEdit::OnLButtonDblClk()
{
	SelectAll();
	return TRUE;
}

int	KWndEdit::SelectAll()
{
	int nResult = false;
    int nSelEndPos = 0;

    KG_PROCESS_ERROR(m_pTextHandle);

    nSelEndPos = m_pTextHandle->GetItemCount();
    KG_PROCESS_ERROR(nSelEndPos >= 1);
    nSelEndPos -= 1;

    SetCaretPos(nSelEndPos, false);
    SetSelStartPos(0, false);
    SetSelEndPos(nSelEndPos, false);

	SetStatus(WNDEDIT_F_SEL_STARTED, FALSE);

	nResult = true;
Exit0:
	return nResult;
}

int	KWndEdit::CancelSelect()
{
	SetSelStartPos(m_dwCaretPos, m_nCaretAfterFlag);
	SetSelEndPos(m_dwCaretPos, m_nCaretAfterFlag);
	SetStatus(WNDEDIT_F_SEL_STARTED, FALSE);
	return true;
}


int KWndEdit::AppendString(LPCWSTR wszString)
{
	if (wszString == NULL)
        return FALSE;

	while (wszString && *wszString != L'\0')
	{
		if (*wszString == L'\t')    // '\t' turn to 4 spaces 
		{
			AppendString(L"    ");
		}
		else if (!AppendChar(wszString[0]))
		{
			return FALSE;
		}

		wszString = ::CharNextW(wszString);
	}
	
	return TRUE;
}

int KWndEdit::PasteString()
{
	int nResult = false;
	HGLOBAL hData = NULL;
	LPCWSTR wszString = NULL;
	int nOpen = false;
	int nLock = false;

	KG_PROCESS_ERROR(::OpenClipboard(NULL));
	nOpen = true;

	hData = ::GetClipboardData(CF_UNICODETEXT);
	KG_PROCESS_ERROR(hData);

	wszString = (LPCWSTR)::GlobalLock(hData);
	nLock = true;
	KG_PROCESS_ERROR(wszString);
	KG_PROCESS_ERROR(wszString[0] != L'\0');

	if (m_dwSelStartPos != m_dwSelEndPos)
		DeleteString();

	AppendString(wszString);

	::GlobalUnlock(hData);
	nLock = false;

	if (IsStatus(WNDEDIT_ES_MASK_CHARACTER))
		::EmptyClipboard();
	
	nResult = true;
Exit0:
	if (nLock)
		::GlobalUnlock(hData);
	if (nOpen)
		::CloseClipboard();
	return nResult;
}

int KWndEdit::CopyString()
{
	int nResult = false;
	DWORD dwCopyStart = m_dwSelStartPos;
	DWORD dwCopyEnd = m_dwSelEndPos;
	int nOpen = false;
	int nLock = false;
	HGLOBAL hData = NULL;
	LPWSTR wszDest = NULL;
    std::wstring strTemp;

	KG_PROCESS_ERROR(m_pTextHandle);
	KG_PROCESS_ERROR(!IsStatus(WNDEDIT_ES_MASK_CHARACTER));
	KG_PROCESS_ERROR(m_dwSelEndPos != m_dwSelStartPos);
	
	if (m_dwSelEndPos < m_dwSelStartPos)
	{
		dwCopyStart = m_dwSelEndPos;
		dwCopyEnd = m_dwSelStartPos;
	}

	for (DWORD i = dwCopyStart; i < dwCopyEnd; ++i)
	{
		KItemText *pText = g_ItemCast<KItemText>(m_pTextHandle->GetItemByIndex(i));
		ASSERT(pText);

        LPCWSTR szText = pText->GetText();
        if (wcscmp(szText, L"\n"))
			strTemp += szText;
		else
			strTemp += L"\r\n";
	}
	KG_PROCESS_ERROR(strTemp.size() > 0 && strTemp[0] != L'\0');

	KG_PROCESS_ERROR(::OpenClipboard(g_pUI->m_hWnd));
	nOpen = true;

	::EmptyClipboard();
	hData = ::GlobalAlloc(GMEM_MOVEABLE, (strTemp.length() + 1) * sizeof(WCHAR));	//应该是不用释放的!?
	KG_PROCESS_ERROR(hData);

	wszDest = (LPWSTR)::GlobalLock(hData);
	nLock = true;
	KG_PROCESS_ERROR(wszDest);

	wcscpy(wszDest, strTemp.c_str());

	::GlobalUnlock(hData);
	nLock = false;

	::SetClipboardData(CF_UNICODETEXT, hData);
	
	nResult = true;
Exit0:
	if (nLock)
		::GlobalUnlock(hData);
	if (nOpen)
		::CloseClipboard();
	return nResult;
}

int KWndEdit::DeleteString()
{
	int     nResult = false;
    int     nCount  = 0;
	DWORD   dwStart = m_dwSelStartPos;
	DWORD   dwEnd   = m_dwSelEndPos;
    DWORD   dwCaret = m_dwSelStartPos;

    KG_PROCESS_ERROR(m_pTextHandle);

	if (m_dwSelEndPos < m_dwSelStartPos)
	{
		dwStart = m_dwSelEndPos;
		dwEnd   = m_dwSelStartPos;
	}

	if (dwStart == dwEnd)   // If nothing have been selected, then delete the next one.
	{
        dwStart = GetCaretPos();
        dwEnd   = GetCaretPos() + 1;
	}
	KG_PROCESS_ERROR(dwEnd > dwStart);

    SetStatus(WNDEDIT_F_SEL_STARTED, FALSE);
	SetStatus(WNDEDIT_F_STR_TRANSFORMED, FALSE);
    SetStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED, TRUE);

    nCount = m_pTextHandle->GetItemCount();
    if ((int)dwEnd >= nCount)
        dwEnd = nCount - 1; // DO NOT REMOVE the last one(terminator).

    for (int i = (int)dwEnd - 1; i >= (int)dwStart; --i)
    {
        int nRetCode = false;
        KItemNull *pItem = NULL;

        pItem = m_pTextHandle->GetItemByIndex(i);
        KG_ASSERT_EXIT(pItem);

		Remove(pItem);
    }

	if (m_pTextHandle->GetItemCount() < 3)
	{
		float fX, fY;
		m_pTextHandle->GetItemStartRelPos(fX, fY);
		m_pTextHandle->SetItemStartRelPos(0.0f, fY);
	}

	g_pUI->m_Script.FireWndEvent(this, "OnEditChanged");

	SetCaretPos(dwStart, false);
	SetSelStartPos(dwStart, false);
	SetSelEndPos(dwStart, false);

	nResult = true;
Exit0:
	return nResult;
}

LRESULT KWndEdit::OnKeyDownSpecial(WPARAM wKey)
{
	LRESULT lRet = FALSE;

	int nCtrl  = ::GetKeyState(VK_CONTROL) & 0x8000;
	int nShift = ::GetKeyState(VK_SHIFT)   & 0x8000;

    KG_PROCESS_ERROR(m_pTextHandle);

    if (nShift)
	{
		if (!IsStatus(WNDEDIT_F_SEL_STARTED))
		{
			SetStatus(WNDEDIT_F_SEL_STARTED, TRUE);
			SetSelStartPos(m_dwCaretPos, m_nCaretAfterFlag);
			SetSelEndPos(m_dwCaretPos, m_nCaretAfterFlag);
		}
	}
	else
	{
		SetStatus(WNDEDIT_F_SEL_STARTED, FALSE);
		SetSelStartPos(m_dwCaretPos, m_nCaretAfterFlag);
		SetSelEndPos(m_dwCaretPos, m_nCaretAfterFlag);
	}

	switch (wKey)
	{
	case VK_LEFT:	//向前
		if (nCtrl)
			MoveCaretWordPrev();
		else
            MoveCaretCharPrev();
		break;
	case VK_RIGHT:	//向后
		if (nCtrl)
			MoveCaretWordNext();
		else
            MoveCaretCharNext();
		break;
	case VK_UP:		//向上
		MoveCaretRowPrev();
		{
			KWndWindow *pParent = GetParent();
			if (pParent)
				pParent->WndProc(WND_N_EDIT_SPECIAL_KEY_DOWN, (WPARAM)(KWndWindow*)this, wKey);
			g_pUI->m_Script.FireWndEvent(this, "OnEditSpecialKeyDown");
		}
		break;
	case VK_DOWN:	//向下
		MoveCaretRowNext();
		{
			KWndWindow *pParent = GetParent();
			if (pParent)
				pParent->WndProc(WND_N_EDIT_SPECIAL_KEY_DOWN, (WPARAM)(KWndWindow*)this, wKey);
			g_pUI->m_Script.FireWndEvent(this, "OnEditSpecialKeyDown");
		}
		break;
	case VK_HOME:	//开始
		if (nCtrl || !IsStatus(WNDEDIT_ES_MULTI_LINE))
			SetCaretPos(0, false);
		else
			MoveCaretRowHome();
		break;
	case VK_END:	//结束
		if (nCtrl || !IsStatus(WNDEDIT_ES_MULTI_LINE))
        {
            int nItemCount = m_pTextHandle->GetItemCount();
            ASSERT(nItemCount >= 1);
			SetCaretPos((DWORD)(nItemCount - 1), false); // Before the terminator.
        }
		else
        {
			MoveCaretRowEnd();
        }
		break;
	case VK_PRIOR:	//上一页
		MoveCaretPagePrev();
		break;
	case VK_NEXT:	//下一页
		MoveCaretPageNext();
		break;
	}

	if (nShift && IsStatus(WNDEDIT_F_SEL_STARTED))
		SetSelEndPos(m_dwCaretPos, m_nCaretAfterFlag);

	lRet = TRUE;
Exit0:
	return lRet;
}


inline void KWndEdit::MoveCaretCharPrev()
{
    DWORD dwCaretPos = GetCaretPos();

    if (dwCaretPos > 0)
        --dwCaretPos;
        
    SetCaretPos(dwCaretPos, false);
}

inline void KWndEdit::MoveCaretCharNext()
{
    DWORD dwCaretPos = GetCaretPos();

    if (!m_pTextHandle) return;

    if (dwCaretPos < (DWORD)(m_pTextHandle->GetItemCount() - 1))
        ++dwCaretPos;

    SetCaretPos(dwCaretPos, false);
}


void KWndEdit::MoveCaretWordPrev()
{
	int nRetCode = false;
	LPCWSTR wszClip = NULL;
    KItemNull *pItem = NULL;
    KItemText *pItemText = NULL;
    DWORD dwCaretPos = GetCaretPos();

    KG_PROCESS_ERROR(m_pTextHandle);
    KG_PROCESS_SUCCESS(dwCaretPos == 0);

    pItem = m_pTextHandle->GetItemByIndex(--dwCaretPos);
    KG_PROCESS_ERROR(pItem);

    pItemText = g_ItemCast<KItemText>(pItem);
    KG_PROCESS_ERROR(pItemText);

    wszClip = pItemText->GetText();
    KG_PROCESS_ERROR(wszClip);

    // If we start with a separator, jump over all separators.
	if (IsCharacterSeparator(wszClip[0]))
	{
		while (true)
		{
            if (dwCaretPos == 0)
                break;

            pItem = m_pTextHandle->GetItemByIndex(--dwCaretPos);
            KG_PROCESS_ERROR(pItem);

            pItemText = g_ItemCast<KItemText>(pItem);
            KG_PROCESS_ERROR(pItemText);

            wszClip = pItemText->GetText();
            KG_PROCESS_ERROR(wszClip);

			if (!IsCharacterSeparator(wszClip[0]))
			{
                ++dwCaretPos;
				break;
			}
		}

        KG_PROCESS_SUCCESS(true);
	}

    while (dwCaretPos > 0)
	{
        pItem = m_pTextHandle->GetItemByIndex(--dwCaretPos);
        KG_PROCESS_ERROR(pItem);

        pItemText = g_ItemCast<KItemText>(pItem);
        KG_PROCESS_ERROR(pItemText);

        wszClip = pItemText->GetText();

        if (IsCharacterSeparator(wszClip[0]))
		{
            ++dwCaretPos;
			break;
        }
    }

Exit1:
    SetCaretPos(dwCaretPos, false);
Exit0:
    return;
}

void KWndEdit::MoveCaretWordNext()
{
	int nRetCode = false;
    int nItemCount = 0;
	LPCWSTR wszClip = NULL;
    KItemNull *pItem = NULL;
    KItemText *pItemText = NULL;
    DWORD dwCaretPos = GetCaretPos();

    KG_PROCESS_ERROR(m_pTextHandle);
    nItemCount = m_pTextHandle->GetItemCount();

    pItem = m_pTextHandle->GetItemByIndex(dwCaretPos);
    KG_PROCESS_ERROR(pItem);

    pItemText = g_ItemCast<KItemText>(pItem);
    KG_PROCESS_ERROR(pItemText);

    wszClip = pItemText->GetText();
    KG_PROCESS_ERROR(wszClip);

    // If we start with a separator, jump over all separators.
	if (IsCharacterSeparator(wszClip[0]))
	{
		while (true)
		{
            if ((int)dwCaretPos == nItemCount - 2)
            {
                //--dwCaretPos;
                break;
            }

            pItem = m_pTextHandle->GetItemByIndex(++dwCaretPos);
            KG_PROCESS_ERROR(pItem);

            pItemText = g_ItemCast<KItemText>(pItem);
            KG_PROCESS_ERROR(pItemText);

            wszClip = pItemText->GetText();
            KG_PROCESS_ERROR(wszClip);

			if (!IsCharacterSeparator(wszClip[0]))
            {
                --dwCaretPos;
				break;
            }
		}

        KG_PROCESS_SUCCESS(true);
	} 

    while ((int)dwCaretPos < nItemCount - 2)
	{
        pItem = m_pTextHandle->GetItemByIndex(++dwCaretPos);
        KG_PROCESS_ERROR(pItem);

        pItemText = g_ItemCast<KItemText>(pItem);
        KG_PROCESS_ERROR(pItemText);

        wszClip = pItemText->GetText();
        KG_PROCESS_ERROR(wszClip);

        if (IsCharacterSeparator(wszClip[0]))
        {
            --dwCaretPos;
			break;
        }
    }

Exit1:
	SetCaretPos(dwCaretPos + 1, false);
Exit0:
	return;
}

void KWndEdit::MoveCaretRowPrev()
{
    int         nRetCode    = false;
	KItemNull  *pItem       = NULL;
	float       fX          = 0.0f;
	float       fY          = 0.0f;
	float       fWidth      = 0.0f;
	float       fHeight     = 0.0f;
	DWORD       dwPos       = 0;
    int         nAfterFlag  = false;
	int         nIndex      = (int)m_dwCaretPos;

	KG_PROCESS_ERROR(m_pTextHandle);

	pItem = m_pTextHandle->GetItemByIndex(nIndex);
	KG_PROCESS_ERROR(pItem);

	pItem->GetAbsPos(fX, fY);
	pItem->GetSize(fWidth, fHeight);
	fY += fHeight;
	nRetCode = GetTextRowHeight(nIndex, fHeight);
	KG_PROCESS_ERROR(nRetCode);
	fY -= fHeight;

    if (m_nCaretAfterFlag)
    {
        fX += fWidth;
    }
    fY = fY - 1.0f - m_pTextHandle->GetRowSpacing();

	nRetCode = GetTextPos(fX, fY, &dwPos, &nAfterFlag);
    KG_PROCESS_ERROR(nRetCode);

	SetCaretPos(dwPos, nAfterFlag);
Exit0:
	return;
}

void KWndEdit::MoveCaretRowNext()
{
    int         nRetCode    = false;
	KItemNull  *pItem       = NULL;
	float       fX          = 0.0f;
	float       fY          = 0.0f;
	float       fWidth      = 0.0f;
	float       fHeight     = 0.0f;
	DWORD       dwPos       = 0;
    int         nAfterFlag  = false;
	int         nIndex      = (int)m_dwCaretPos;

    KG_PROCESS_ERROR(m_pTextHandle);

    pItem = m_pTextHandle->GetItemByIndex(nIndex);
	KG_PROCESS_ERROR(pItem);

	pItem->GetAbsPos(fX, fY);
	pItem->GetSize(fWidth, fHeight);

    if (m_nCaretAfterFlag)
    {
        fX += fWidth;
    }
    fY = fY + fHeight + m_pTextHandle->GetRowSpacing() + 1.0f;

    nRetCode = GetTextPos(fX, fY, &dwPos, &nAfterFlag);
    KG_PROCESS_ERROR(nRetCode);

	SetCaretPos(dwPos, nAfterFlag);
Exit0:
	return;
}

void KWndEdit::MoveCaretPagePrev()
{
    int         nRetCode    = false;
	KItemNull  *pItem       = NULL;
	float       fX          = 0.0f;
	float       fY          = 0.0f;
	float       fWidth      = 0.0f;
	float       fHeight     = 0.0f;
	DWORD       dwPos       = 0;
    int         nAfterFlag  = false;
	int         nIndex      = (int)m_dwCaretPos;

    KG_PROCESS_ERROR(m_pTextHandle);

    pItem = m_pTextHandle->GetItemByIndex(nIndex);
	KG_PROCESS_ERROR(pItem);

	pItem->GetAbsPos(fX, fY);
	pItem->GetSize(fWidth, fHeight);

    if (m_nCaretAfterFlag)
    {
        fX += fWidth;
    }
    fY = fY + fHeight - m_fHeight;

    nRetCode = GetTextPos(fX, fY, &dwPos, &nAfterFlag);
    KG_PROCESS_ERROR(nRetCode);

	SetCaretPos(dwPos, nAfterFlag);
Exit0:
	return;
}

void KWndEdit::MoveCaretPageNext()
{
    int         nRetCode    = false;
	KItemNull  *pItem       = NULL;
	float       fX          = 0.0f;
	float       fY          = 0.0f;
    float       fWidth      = 0.0f;
    float       fHeight     = 0.0f;
	DWORD       dwPos       = m_dwCaretPos;
    int         nAfterFlag  = false;
	int         nIndex      = (int)m_dwCaretPos;

    KG_PROCESS_ERROR(m_pTextHandle);

    pItem = m_pTextHandle->GetItemByIndex(nIndex);
	KG_PROCESS_ERROR(pItem);

	pItem->GetAbsPos(fX, fY);
    pItem->GetSize(fWidth, fHeight);

    if (m_nCaretAfterFlag)
    {
        fX += fWidth;
    }
    fY = fY + m_fHeight;

    nRetCode = GetTextPos(fX, fY, &dwPos, &nAfterFlag);
    KG_PROCESS_ERROR(nRetCode);

	SetCaretPos(dwPos, nAfterFlag);
Exit0:
	return;
}

void KWndEdit::MoveCaretRowHome()
{
	int         nRetCode    = false;
    KItemNull  *pItem       = NULL;
	float       fX          = 0.0f;
	float       fY          = 0.0f;
	float       fWidth      = 0.0f;
	float       fHeight     = 0.0f;
	DWORD       dwPos       = 0;
    int         nAfterFlag  = false;
	int         nIndex      = (int)m_dwCaretPos;

    KG_PROCESS_ERROR(m_pTextHandle);

    pItem = m_pTextHandle->GetItemByIndex(nIndex);
	KG_PROCESS_ERROR(pItem);

	pItem->GetAbsPos(fX, fY);
	pItem->GetSize(fWidth, fHeight);
	
    fX = m_fAbsX;
    fY = fY + fHeight / 2;

    nRetCode = GetTextPos(fX, fY, &dwPos, &nAfterFlag);
    KG_PROCESS_ERROR(nRetCode);

	SetCaretPos(dwPos, nAfterFlag);
Exit0:
	return;
}

void KWndEdit::MoveCaretRowEnd()
{
	int         nRetCode    = false;
    KItemNull  *pItem       = NULL;
	float       fX          = 0.0f;
	float       fY          = 0.0f;
	float       fWidth      = 0.0f;
	float       fHeight     = 0.0f;
	DWORD       dwPos       = 0;
    int         nAfterFlag  = false;
	int         nIndex      = (int)m_dwCaretPos;

    KG_PROCESS_ERROR(m_pTextHandle);

    pItem = m_pTextHandle->GetItemByIndex(nIndex);
	KG_PROCESS_ERROR(pItem);

	pItem->GetAbsPos(fX, fY);
	pItem->GetSize(fWidth, fHeight);

    fX = m_fAbsX + m_fWidth;
    fY = fY + fHeight / 2;

	nRetCode = GetTextPos(fX, fY, &dwPos, &nAfterFlag);
    KG_PROCESS_ERROR(nRetCode);

	SetCaretPos(dwPos, nAfterFlag);
Exit0:
	return;
}

//--------------------------------------------------------------------------
//	功能：响应键盘按键操作
//--------------------------------------------------------------------------
LRESULT KWndEdit::OnKeyDown(WPARAM wKey, LPARAM lModifier)
{
	LRESULT lRet = FALSE;

	if (IsDisable() || !IsStatus(WNDEDIT_F_HAVE_FOCUS))
		return lRet;

	if (!(::GetKeyState(VK_SHIFT) & 0x8000) && !(::GetKeyState(VK_LBUTTON) & 0x8000))
        SetStatus(WNDEDIT_F_SEL_STARTED, FALSE);

	if (!IsStatus(WNDEDIT_ES_MASK_CHARACTER) && IgnoreKey(wKey))
		return lRet;

	switch (wKey)
	{
	case VK_DELETE:
		DeleteString();
		lRet = TRUE;
		break;
	case VK_LEFT:
	case VK_RIGHT:
	case VK_UP:
	case VK_DOWN:
	case VK_HOME:
	case VK_END:
	case VK_PRIOR:
	case VK_NEXT:
		OnKeyDownSpecial(wKey);
		lRet = TRUE;
		break;
	case 'V':	//粘帖
	case VK_INSERT:
		if (::GetKeyState(VK_CONTROL) & 0x8000)
			lRet = PasteString();
		break;
	case 'C':	//复制
		if (::GetKeyState(VK_CONTROL) & 0x8000)
			lRet = CopyString();
		break;
	case 'X':
		if (::GetKeyState(VK_CONTROL) & 0x8000 && CopyString())
		{
			if (m_dwSelStartPos != m_dwSelEndPos)
				DeleteString();
			lRet = TRUE;
		}
		break;
	case 'A':
		if (::GetKeyState(VK_CONTROL) & 0x8000)
			lRet = SelectAll();
	case VK_TAB:
	case VK_ESCAPE:
		{
            //这里把tab键esc键，和enter键作为特殊的按键。

            SetStatus(WNDEDIT_F_SEL_STARTED, FALSE);

			int nRet;
			g_pUI->m_Script.FireWndEvent(this, "OnEditSpecialKeyDown", &nRet);
			if (nRet)
				lRet = TRUE;
		}
		break;
	case VK_RETURN:
		{
			int nRet = false;
			g_pUI->m_Script.FireWndEvent(this, "OnEditSpecialKeyDown", &nRet);
			if (nRet)
			{
				SetSelStartPos(m_dwCaretPos, m_nCaretAfterFlag);
				SetSelEndPos(m_dwCaretPos, m_nCaretAfterFlag);
				SetStatus(WNDEDIT_F_SEL_STARTED, FALSE);
				lRet = TRUE;
			}
			else if (IsStatus(WNDEDIT_ES_MULTI_LINE))
			{
				if (m_dwSelStartPos != m_dwSelEndPos)
					DeleteString();
				lRet = AppendFilteredChar(L'\n');
			}
		}
		break;
	}

	if (!lRet)
		lRet = HoldUpKey(wKey);

	return lRet;
}

int KWndEdit::IgnoreKey(WPARAM wKey)
{
	int nResult = false;
	g_pUI->m_Script.FireWndEvent(this, "OnIgnoreKeyDown", &nResult);
	return nResult;
}

int KWndEdit::HoldUpKey(WPARAM wKey)
{
	if (::GetKeyState(VK_CONTROL) & 0x8000)
		return false;
	if (::GetKeyState(VK_MENU) & 0x8000)
		return false;
	if (::GetKeyState(VK_PAUSE) & 0x8000)
		return false;
	if (::GetKeyState(VK_SHIFT) & 0x8000)
		return true;
	if (wKey == VK_PROCESSKEY)
		return true;
	if (wKey == VK_NUMLOCK)
		return true;

	if (wKey >= '0' && wKey <= '9')
		return true;
	if (wKey >= 'A' && wKey <= 'Z')
		return true;
	if (wKey >= VK_NUMPAD0 && wKey <= VK_DIVIDE)
		return true;
	if (wKey >= VK_OEM_1 && wKey <= VK_OEM_8)
		return true;
	if (wKey == VK_SPACE)
		return true;

	return false;
}

void KWndEdit::OnBreathe()
{
    int nResult = false;

	KWndWindow::OnBreathe();
	//切换输入光标的显示状态

	KG_PROCESS_ERROR(IsStatus(WNDEDIT_F_HAVE_FOCUS));
	KG_PROCESS_ERROR(!IsDisable());

    KG_PROCESS_ERROR(m_pCursor);

    DWORD const dwTickCount = KG_GetTickCount();
	DWORD const dwTickCountElapsed = dwTickCount - m_dwCaretTickCount;
	if (dwTickCountElapsed > 500)
	{
        if (m_pCursor->GetAlpha() > 0)
            m_pCursor->SetAlpha(0);
        else
            m_pCursor->SetAlpha(255);
        
		m_dwCaretTickCount = dwTickCount;
	}

    nResult = true;

Exit0:
	return;
}


void KWndEdit::OnScale(float fScaleX, float fScaleY)
{

    KWndWindow::OnScale(fScaleX, fScaleY);

    float fRelX = 0.0f;
	float fRelY = 0.0f;

	if (!m_pTextHandle)
        return;

    m_pTextHandle->Scale(fScaleX, fScaleY);
	m_pTextHandle->GetRelPos(fRelX, fRelY);
	m_pTextHandle->SetAbsPos(m_fAbsX + fRelX, m_fAbsY + fRelY);

	KItemNull *pT = m_pTextHandle->GetItemByIndex(m_pTextHandle->GetItemCount() - 1);
	if (pT)
		pT->SetSize(0, GetFontHeight(m_nFontScheme));

    m_pCursor->Scale(fScaleX, fScaleY);
	m_pCursor->AutoSize();
    SetStatus(WNDEDIT_F_CURSOR_MOVED, TRUE);


    //TODO: Check text position and size!
	SetStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED, TRUE);
}


void KWndEdit::OnPaint()
{
    int   nRetCode   = false;
    float fX         = 0.0f;
    float fY         = 0.0f;
    float fCursorX   = 0.0f;
    float fCursorY   = 0.0f;
    float fWidth     = 0.0f;
    float fHeight    = 0.0f;
    KItemNull *pItem = NULL;
	KItemArray::iterator itEnd;

    KG_PROCESS_ERROR(m_pTextHandle);
    KG_PROCESS_ERROR(m_pCursor);

    nRetCode = IsVisible();
    KG_PROCESS_ERROR(nRetCode);

    if (IsStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED))
        FormatShowData();

    if (IsStatus(WNDEDIT_F_CURSOR_MOVED))
    {
        LetCaretShow();

        pItem = m_pTextHandle->GetItemByIndex(m_dwCaretPos);
        if (pItem)
        {
            pItem->GetAbsPos(fX, fY);
            pItem->GetSize(fWidth, fHeight);
            fY += fHeight;
            if (m_nCaretAfterFlag)
                fX += fWidth;
        }
        else
        {
            ASSERT(!"ERROR!");
            SetCaretPos(0, false);
            m_pTextHandle->GetAbsPos(fX, fY);
            m_pTextHandle->GetAllItemSize(fWidth, fHeight);
            fY += fHeight;
        }

        m_pCursor->GetSize(fWidth, fHeight);
        fY -= fHeight;
        m_pCursor->SetAbsPos(fX - fWidth / 2, fY);

        CorrectImePos();
        SetStatus(WNDEDIT_F_CURSOR_MOVED, FALSE);
    }

    if (IsStatus(WNDEDIT_F_HAVE_FOCUS))
	{
		//绘制底图阴影
		KG3DUIRect2 rc;
		rc.crDiffuse = m_dwFocusBgColor;
		rc.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		rc.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		g_pUI->m_p3DUI->FillMultiRect(&rc, 1);
	}

	if (IsStatus(WNDEDIT_F_DRAW_DATA_CHANGED))
	{
		m_aDraw.clear();
		m_pTextHandle->PushAllDrawItem(m_aDraw);
		SetStatus(WNDEDIT_F_DRAW_DATA_CHANGED, FALSE);
	}

	{
		KItemArray::iterator it = m_aDraw.begin();
		KItemArray::iterator ie = m_aDraw.end();
		if (!IsStatus(WNDEDIT_ES_MULTI_LINE))
		{
			float fW, fH;
			m_pTextHandle->GetAllItemSize(fW, fH);
			m_pTextHandle->SetSize(m_fWidth, fH);
		}
		for (; it != ie; ++it)
		{
			KItemNull* pItem = *it;
			pItem->Draw();
		}
		if (!IsStatus(WNDEDIT_ES_MULTI_LINE))
		{
			m_pTextHandle->SetSize(m_fWidth, m_fHeight);
		}
		if (IsStatus(WNDEDIT_F_HAVE_FOCUS))
		{
			m_pCursor->Draw();
		}
	}

	KWndWindow::OnPaint();

Exit0:
    return;
}

void KWndEdit::AbsMove(float fXOffset, float fYOffset)
{
	KWndWindow::AbsMove(fXOffset, fYOffset);
	if (m_pTextHandle)
		m_pTextHandle->AbsMove(fXOffset, fYOffset);
}

int KWndEdit::SetText(LPCWSTR wszText)
{
	int nResult  = false;
    int nRetCode = false;

	KG_PROCESS_ERROR(wszText);

	nRetCode = ClearText();
    KG_PROCESS_ERROR(nRetCode);

	nRetCode = AppendString(wszText);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_ERROR(m_pTextHandle);
    nRetCode = m_pTextHandle->GetItemCount();
    ASSERT(nRetCode >= 1);
    SetCaretPos((DWORD)(nRetCode - 1), false);

    nResult = true;
Exit0:
    return nResult;
}

LPCWSTR KWndEdit::GetText()
{
    LPCWSTR pResult = NULL;
    LPCSTR szName = NULL;

    KG_PROCESS_ERROR(m_pTextHandle);
    KG_PROCESS_SUCCESS(IsStatus(WNDEDIT_F_STR_TRANSFORMED));

    m_strText.clear();

    for (int i = 0; i < (m_pTextHandle->GetItemCount() - 1); ++i)
    {
        KItemNull *pItemNull = NULL;
		KItemText *pItemText = NULL;
        
        pItemNull = m_pTextHandle->GetItemByIndex(i);
        KG_PROCESS_ERROR(pItemNull);

#ifdef _DEBUG
        szName = pItemNull->GetName();
        KG_PROCESS_ERROR(szName);

        if (strcmp(szName, K_INPUT_TERM_NAME) == 0)
        {
            ASSERT(!"Why we reach a terminator before loop exit?");
            continue;
        }

        if (strcmp(szName, K_INPUT_CURSOR_NAME) == 0)
        {
            ASSERT(!"Why we got a cursor before loop exit?");
            continue;
        }
#endif

        pItemText = g_ItemCast<KItemText>(pItemNull);
		KG_PROCESS_ERROR(pItemText);

        m_strText += pItemText->GetText();
    }

    SetStatus(WNDEDIT_F_STR_TRANSFORMED, TRUE);

Exit1:
	pResult = m_strText.c_str();

#ifdef KWNDEDIT_DEBUG_TRACE
    ATLTRACE("KWndEdit::GetText()\t%s\n", pResult);
#endif

Exit0:
    return pResult;
}

int KWndEdit::GetPassword(char szDestMD5String[64])
{
    int nResult  = false;
    int nRetCode = false;
    std::string strTmp;

    KG_PROCESS_ERROR(m_pTextHandle);
	KG_PROCESS_ERROR(IsStatus(WNDEDIT_ES_MASK_CHARACTER));

    for (int i = 0; i < m_pTextHandle->GetItemCount(); ++i)
    {
        int		  nUserData = 0;
        KItemNull *pItem = NULL;
		KItemText *pItemText = NULL;
		WCHAR szTemp[2];
        
        pItem = m_pTextHandle->GetItemByIndex(i);
		KG_PROCESS_ERROR(pItem);

        if (strcmp(pItem->GetName(), K_INPUT_MASK_NAME))
            continue;        

        pItemText = (KItemText*)pItem;
		KG_PROCESS_ERROR(pItemText);

        nRetCode = pItemText->GetUserDataInt(&nUserData, NULL);
        KG_PROCESS_ERROR(nRetCode);

		szTemp[0] = (WCHAR)nUserData;
		szTemp[1] = L'\0';

        strTmp += ATL::CW2A(szTemp, GetCodePage());
    }

    KG_PROCESS_ERROR(strTmp.c_str());

    KG_EDStringToMD5String(szDestMD5String, strTmp.c_str());

    nResult = true;
Exit0:
    return nResult;
}

int KWndEdit::GetTextLength(BOOL bMultiByte)
{
    int   nResult   = 0;
    void *pvRetCode = NULL;

    KG_PROCESS_SUCCESS(IsStatus(WNDEDIT_F_STR_TRANSFORMED));

    pvRetCode = (void *)GetText();
    KG_PROCESS_ERROR(pvRetCode);

Exit1:
	if (bMultiByte)
	{
		nResult = (int)strlen(ATL::CW2A(m_strText.c_str(), GetCodePage()));
	}
	else
	{
		nResult = (int)wcslen(m_strText.c_str());
	}
Exit0:
    return nResult;
}

int KWndEdit::ClearText()
{
	int nChange = false;
    if (!m_pTextHandle) 
        return false;

    m_strText.clear();
	SetCaretPos(0, false);
	SetSelStartPos(0, false);
	SetSelEndPos(0, false);

    for (int i = m_pTextHandle->GetItemCount() - 2; i >= 0; --i)
    {
        KItemNull *pItem = NULL;

        pItem = m_pTextHandle->GetItemByIndex(i);
        if (!pItem)
            continue;

#ifdef _DEBUG
        if (strcmp(pItem->GetName(), K_INPUT_CURSOR_NAME) == 0)
        {
            ASSERT(!"How should we found a cursor?");
            continue;
        }

        if (strcmp(pItem->GetName(), K_INPUT_TERM_NAME) == 0)
        {
            ASSERT(!"How should we found a terminator?");
            continue;
        }
#endif

		Remove(pItem);
		nChange = true;
    }

	m_pTextHandle->SetItemStartRelPos(0.0f, 0.0f);

	if (nChange)
		g_pUI->m_Script.FireWndEvent(this, "OnEditChanged");

    SetStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED, TRUE);

    return true;
}

void KWndEdit::Remove(KItemNull *pItem)
{
	if (m_pTextHandle && pItem)
	{
		LPCSTR pszName = pItem->GetName();
		if (pszName && strcmp(pszName, K_INPUT_OBJ_NAME) == 0)
		{
			int nUserData = LUA_NOREF;
			pItem->GetUserDataInt(&nUserData, NULL);
			if (nUserData != LUA_NOREF)
				luaL_unref(g_pUI->m_Script.GetLuaState(), LUA_REGISTRYINDEX, nUserData);
		}
		m_pTextHandle->Remove(pItem);
	}
}

void KWndEdit::SetFontScheme(int nFontScheme)
{
    KItemNull* pItem = NULL;
    int nIndex = 0;

	if (g_pUI->m_FontSchemeMgr.GetFontScheme(nFontScheme) == NULL)
	    return;

	m_nFontScheme = nFontScheme;

    if (!m_pTextHandle)
        return;

    nIndex = m_pTextHandle->GetItemCount() - 1;
    for (int i = 0; i < nIndex; ++i)
    {
        KItemNull *pItem     = NULL;
        KItemText *pItemText = NULL;

        pItem = m_pTextHandle->GetItemByIndex(i);
        if (!pItem)
            continue;

        pItemText = g_ItemCast<KItemText>(pItem);
        if (!pItemText)
            continue;

        pItemText->SetFontScheme(m_nFontScheme);
    }

    pItem = m_pTextHandle->GetItemByIndex(nIndex);
    if (pItem)
    {
        float fWidth  = 0.0f;
        float fHeight = 0.0f;

        fHeight = GetFontHeight(m_nFontScheme);
        pItem->SetSize(fWidth, fHeight);
    }

	SetStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED, TRUE);
}

void KWndEdit::SetFontColor(DWORD dwColor)
{
	m_dwFontColor = dwColor | 0xFF000000;
	int nCount = 0;
	if (!m_pTextHandle)
		return;

	nCount = m_pTextHandle->GetItemCount() - 1;
	for (int i = 0; i < nCount; ++i)
	{
		KItemNull *pItem     = NULL;
		KItemText *pItemText = NULL;

		pItem = m_pTextHandle->GetItemByIndex(i);
		if (!pItem)
			continue;

		pItemText = g_ItemCast<KItemText>(pItem);
		if (!pItemText)
			continue;

		pItemText->SetFontColor(m_dwFontColor);
	}
}

int KWndEdit::GetFontScheme()
{
	return m_nFontScheme;
}


int KWndEdit::CanCastTo(LPCSTR szType) const
{
	return szType == "WndEdit" || szType == "WndWindow";
}

int KWndEdit::FormatShowData()
{
    int   nResult  = false;
    float fX       = 0.0f;
    float fY       = 0.0f;
    float fWidth   = 0.0f;
    float fHeight  = 0.0f;

    KG_PROCESS_ERROR(m_pTextHandle);

    KG_PROCESS_SUCCESS(!IsStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED));

    if (!IsStatus(WNDEDIT_ES_MULTI_LINE))
    {
	    m_pTextHandle->SetAjustShowRichTextType(true);
        m_pTextHandle->GetSize(fWidth, fHeight);
	    m_pTextHandle->SetSize(1000000.0f, fHeight);
    }

    m_pTextHandle->FormatAllItemPos();

    if (!IsStatus(WNDEDIT_ES_MULTI_LINE))
    {
	    m_pTextHandle->SetSize(fWidth, fHeight);
	    m_pTextHandle->AdjustItemShowInfo();
    }

	SetStatus(WNDEDIT_F_DRAW_DATA_CHANGED, TRUE);

Exit1:
    SetStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED, FALSE);
    nResult = true;

Exit0:
    return nResult;
}

int KWndEdit::LetCaretShow()
{
	int nResult = false;
	float fX = 0.0f;
	float fY = 0.0f;
	float fWidth = 0.0f;
	float fHeight = 0.0f;

	float fXAll = 0.0f;
	float fYAll = 0.0f;
	float fWidthAll = 0.0f;
	float fHeightAll = 0.0f;

	float fRelPosX = 0.0f;
	float fRelPosY = 0.0f;

    KItemNull *pItem = NULL;

	KG_PROCESS_ERROR(m_pTextHandle);

    pItem = m_pTextHandle->GetItemByIndex(m_dwCaretPos);
	KG_PROCESS_ERROR(pItem);

    if (IsStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED))
        m_pTextHandle->FormatAllItemPos();

	m_pTextHandle->GetAbsPos(fXAll, fYAll);
	m_pTextHandle->GetSize(fWidthAll, fHeightAll);
	m_pTextHandle->GetItemStartRelPos(fRelPosX, fRelPosY);

    pItem->GetAbsPos(fX, fY);
    pItem->GetSize(fWidth, fHeight);

	//让光标始终处于可以看见的位置。
	if (IsStatus(WNDEDIT_ES_MULTI_LINE))
	{
		//多行
		KG_PROCESS_SUCCESS(IsTextRowVisible((int)m_dwCaretPos));
		fY += fHeight;
		GetTextRowHeight((int)m_dwCaretPos, fHeight);
		fY -= fHeight;
		if (fY < fYAll)
		{
			fRelPosY = fRelPosY + fYAll - fY;
		}

		if (fY + fHeight > fYAll + fHeightAll)
		{
			fRelPosY = fRelPosY - ((fY + fHeight) - (fYAll + fHeightAll));
		}

		m_pTextHandle->SetItemStartRelPos(fRelPosX, fRelPosY);
	}
	else
	{
		//单行
		if (!pItem->IsVisible() || fX + fWidth > fXAll + fWidthAll)
		{
			if (fX < fXAll)
				fRelPosX += fXAll - fX;

			if (fX + fWidth > fXAll + fWidthAll)
				fRelPosX -= (fX + fWidth) - (fXAll + fWidthAll);

			m_pTextHandle->SetItemStartRelPos(fRelPosX, fRelPosY);
		}

		if (pItem->IsVisible())
		{
			if (m_dwCaretPos > 0)
			{
				KItemNull *pItemPrev = m_pTextHandle->GetItemByIndex(m_dwCaretPos - 1);
				pItemPrev->GetAbsPos(fX, fY);
				if (fX < fXAll)
				{
					m_pTextHandle->SetItemStartRelPos(fRelPosX + fXAll - fX, fRelPosY);
					if (!pItem->IsVisible())
						m_pTextHandle->SetItemStartRelPos(fRelPosX, fRelPosY);
				}
			}
			else
			{
				m_pTextHandle->SetItemStartRelPos(0.0f, fRelPosY);
			}
		}

	}

Exit1:
	SetStatus(WNDEDIT_F_DRAW_DATA_CHANGED, TRUE);

//	pItem->Show();
	nResult = true;
Exit0:
	return nResult;
}

inline int KWndEdit::IsInSelectRegion(DWORD dwPos)
{
    if (
        m_dwSelEndPos > m_dwSelStartPos &&
        dwPos >= m_dwSelStartPos &&
        dwPos <  m_dwSelEndPos
    )
        return true;

    if (
        m_dwSelEndPos < m_dwSelStartPos &&
        dwPos <  m_dwSelStartPos &&
        dwPos >= m_dwSelEndPos
    )
        return true;

    return false;
}

void KWndEdit::UpdateSelRegion()
{
    int nSelected   = false;
    int nFontScheme = 0;

    KItemNull *pItem     = NULL;
    KItemText *pItemText = NULL;

    if (!m_pTextHandle)
        return;

    for (int i = 0; i != (m_pTextHandle->GetItemCount() - 1); ++i)
    {
        pItem = m_pTextHandle->GetItemByIndex(i);
        if (!pItem)
            continue;

#ifdef _DEBUG
        if (strcmp(pItem->GetName(), K_INPUT_CURSOR_NAME) == 0 ||
            strcmp(pItem->GetName(), K_INPUT_TERM_NAME) == 0)
        {
            ASSERT(!"How can we found a cursor?");
            continue;
        }
#endif

        pItemText = g_ItemCast<KItemText>(pItem);
        if (!pItemText)
            continue;

        nSelected   = false;
        nFontScheme = m_nFontScheme;
        if (IsInSelectRegion((DWORD)i))
        {
    		nSelected   = true;
            nFontScheme = m_nSelectFontScheme;
        }

        pItemText->SetFontScheme(nFontScheme);
		if (m_dwFontColor && !nSelected)
			pItemText->SetFontColor(m_dwFontColor);

        pItemText->SetSelected(nSelected);
        pItemText->SetSelBgColor(m_dwSelectBgColor);
    }

    SetStatus(WNDEDIT_F_SHOW_DATA_UNFORMATED, TRUE);
}

void KWndEdit::SetSelStartPos(DWORD dwPos, int nAfterFlag)
{
    if (nAfterFlag)
        ++dwPos;

	if (dwPos == m_dwSelStartPos)
        return;

    m_dwSelStartPos = dwPos;
    UpdateSelRegion();
}

void KWndEdit::SetSelEndPos(DWORD dwPos, int nAfterFlag)
{
    if (nAfterFlag)
        ++dwPos;

    if (dwPos == m_dwSelEndPos)
        return;

    m_dwSelEndPos = dwPos;
    UpdateSelRegion();
}

inline void KWndEdit::SetCaretPos(DWORD dwPos, int nAfterFlag)
{
    int nCount = 0;

    KG_PROCESS_ERROR(m_pTextHandle);

    nCount = m_pTextHandle->GetItemCount();
    ASSERT(nCount >= 1);

    if ((int)dwPos >= (nCount - 1))
    {
        dwPos = nCount - 1;
    }

    if (nAfterFlag && dwPos >= (DWORD)(nCount - 2))
    {
        dwPos = nCount - 2;
    }

    m_dwCaretPos = dwPos;
    m_nCaretAfterFlag = nAfterFlag;
    SetStatus(WNDEDIT_F_CURSOR_MOVED, TRUE);

    if (!IsDisable() && m_pCursor)
    {
        m_dwCaretTickCount = KG_GetTickCount();
        m_pCursor->SetAlpha(255);
    }

Exit0:
    return;
}

inline DWORD KWndEdit::GetCaretPos()
{
    DWORD dwResult = m_dwCaretPos;
    
    if (m_nCaretAfterFlag)
        ++dwResult;

    return dwResult;
}

float KWndEdit::GetFontHeight(int nFontScheme)
{
    float fHeight = 12.0f;
	KFontScheme const* pFontScheme = NULL;

	pFontScheme = g_pUI->m_FontSchemeMgr.GetFontScheme(nFontScheme);
	KGLOG_PROCESS_ERROR(pFontScheme);

    if (g_pUI->m_FontSchemeMgr.IsFontVertical(nFontScheme))
		g_pUI->m_p3DUI->GetCharWidth(pFontScheme->nFontID, 1.0f, fHeight);
    else
		g_pUI->m_p3DUI->GetCharHeight(pFontScheme->nFontID, 1.0f, fHeight);

Exit0:
    return fHeight;
}


void KWndEdit::SetStatus(DWORD dwStatus, BOOL bEnable)
{
	if (bEnable)
		m_dwStatus |= dwStatus;
	else
		m_dwStatus &= ~dwStatus;
}

void KWndEdit::SetFontSpacing(float fSpacing)
{
	m_fFontSpacing = fSpacing;
}

void KWndEdit::SetRowSpacing(float fSpacing)
{
	m_fRowSpacing = fSpacing;
}

void KWndEdit::SetFocusBgColor(DWORD dwColor)
{
	m_dwFocusBgColor = dwColor;
}

void KWndEdit::SetSelectBgColor(DWORD dwColor)
{
    m_dwSelectBgColor = dwColor;
}

void KWndEdit::SetSelectFontScheme(int nFontScheme)
{
	m_nSelectFontScheme = nFontScheme;
}

void KWndEdit::SetCaretFontScheme(int nFontScheme)
{
	m_nCaretFontScheme = nFontScheme;
}

LPCSTR KWndEdit::GetWndType() const
{ 
	return "WndEdit";
}

}