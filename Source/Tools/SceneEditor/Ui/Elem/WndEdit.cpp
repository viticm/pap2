/*****************************************************************************************
//	界面窗口体系结构--输入窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-22
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KWin32Wnd.h"
#include "Win32Edit.h"
#include "../../Public/KIniFile.h"
#include "../Elem/WndMessage.h"
#include "Wnds.h"
#include "WndEdit.h"
#include "../../Public/KIme.h"
#include "../../Public/Text.h"
#include "../../Public/KRepresentUnit.h"
#include "../../Public/iRepresentShell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;

static KIme	s_Ime;
static BOOL	s_LastTimeIsIme;


#define	MULTI_LINE	(m_Flag & WNDEDIT_ES_MULTI_LINE)

//寻找字符串换行的位置
int SplitStringLine(char* pString, int nLine, int nLineByte);

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndEdit::KWndEdit()
{
	m_Flag = WNDEDIT_ES_ENGLISH_AVAILABLE;
	m_nBufferSize	= 0;
	m_nLimitText	= 0;
	m_nTextLen		= 0;

	m_nSkipAhead    = 0;
	m_nSkipBehind   = 0;
	m_nCaretLineIndex = 0;
	m_nCaretPos     = 0;
	m_nCaretTime    = 0;
	m_nFontSize		= 12;

	m_TextBorderColor = 0;
	m_FocusBkColor = 0;

	m_hWin32Wnd = NULL;
}

KWndEdit::~KWndEdit()
{
    if (m_hWin32Wnd)
	{
		Win32Edit_Destroy(m_hWin32Wnd);
        m_hWin32Wnd = NULL;
    }
}

void KWndEdit::Clone(KWndEdit* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_Flag = m_Flag & WNDEDIT_ES_FILTER & ~WNDEDIT_ES_WIN32_WND;
		pCopy->m_nLimitText	= m_nLimitText;
		pCopy->m_TextColor	= m_TextColor;
		pCopy->m_nFontSize	= m_nFontSize;
		pCopy->m_TextBorderColor = m_TextBorderColor;
		pCopy->m_FocusBkColor = m_FocusBkColor;
		pCopy->FmtForShow();
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KWndEdit::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		int	nValue;
		pIniFile->GetInteger(pSection, "MultiLine", 0, &nValue);
		if (nValue)
		{
			m_Flag |= WNDEDIT_ES_MULTI_LINE;
			m_nSkipAhead = 0;
		}
		else
		{
			m_Flag &= ~WNDEDIT_ES_MULTI_LINE;
			m_nCaretLineIndex = 0;
		}

		pIniFile->GetInteger(pSection, "Password", 0, &nValue);
		if (nValue)
		{
			m_Flag |= WNDEDIT_ES_MASK_CHARACTER;
			m_Flag &= ~WNDEDIT_ES_MULTI_LINE;
		}
		else
		{
			m_Flag &= ~WNDEDIT_ES_MASK_CHARACTER;
		}

		m_Flag &= ~WNDEDIT_ES_CHAR_TYPE_FILTER;
		pIniFile->GetInteger(pSection, "Type", 0, &nValue);
		if (nValue == 1)
			m_Flag |= WNDEDIT_ES_ENGLISH_AVAILABLE;
		else if (nValue == 2)
			m_Flag |= WNDEDIT_ES_IME_AVAILABLE;

		//====字串限长====
		pIniFile->GetInteger(pSection, "MaxLen", -1, &m_nLimitText);
		if (m_nLimitText > m_nBufferSize - 1 || m_nLimitText == -1)
			m_nLimitText = m_nBufferSize -1;
		if (m_nLimitText < 0)
			m_nLimitText = 0;
		//if limitext value is been changed, ignore changing.

		//===文字颜色====
		char	Buff[16];
		pIniFile->GetString(pSection, "Color", "", Buff, 16);
		m_TextColor = GetColor(Buff);
		pIniFile->GetString(pSection, "BorderColor", "", Buff, 16);
		m_TextBorderColor = GetColor(Buff);
		//====文字大小====
		pIniFile->GetInteger(pSection, "Font", 12, &m_nFontSize);
		if (m_nFontSize < 8)
			m_nFontSize = 12;
		if (m_Width < m_nFontSize)
		{
			if (m_Flag & WNDEDIT_ES_IME_AVAILABLE)
				m_Width = m_nFontSize;
			else
				m_Width = m_nFontSize / 2;
		}
		if (m_Height < m_nFontSize + 1)
			m_Height = m_nFontSize + 1;

		pIniFile->GetString(pSection, "FocusBKColor", "0,0,0", Buff, sizeof(Buff));
		m_FocusBkColor = (GetColor(Buff) & 0xffffff);
		if (pIniFile->GetInteger(pSection, "FocusBKColorAlpha", 128, &nValue))
		{
			if (nValue > 255 || nValue < 0)
				m_FocusBkColor |= 0x16000000;
			else
				m_FocusBkColor |= (((255 - nValue) << 21) & 0xff000000);
		}


		pIniFile->GetInteger(pSection, "Win32Edit", 0, &nValue);
        //nValue = true;
		if (nValue)
		{
			if (m_hWin32Wnd == NULL)
				m_hWin32Wnd = Win32Edit_Create(g_GetMainHWnd(), m_Flag, m_nLimitText);
			if (m_hWin32Wnd)
			{
				Win32Edit_UpdateSizePos(m_hWin32Wnd, m_nAbsoluteLeft, m_nAbsoluteTop, m_Width, m_Height);
				Win32Edit_SetTextColor(m_hWin32Wnd, m_TextColor);
				m_Flag |= WNDEDIT_ES_WIN32_WND;
			}
			else
			{
				m_Flag &= ~WNDEDIT_ES_WIN32_WND;
			}
		}
		if (m_hWin32Wnd && (m_Flag & WNDEDIT_ES_WIN32_WND) == 0)
		{
			Win32Edit_Destroy(m_hWin32Wnd);
			m_hWin32Wnd = NULL;
		}

		FmtForShow();
		return true;
	}
	return false;
}

BOOL KWndEdit::SetType(int nType)
{
	if (nType >= 0 && nType <= 2)
	{
		m_Flag &= ~WNDEDIT_ES_CHAR_TYPE_FILTER;
		switch (nType)
		{
		case 0:
			break;
		case 1:
			m_Flag |= WNDEDIT_ES_ENGLISH_AVAILABLE;
			break;
		case 2:
			m_Flag |= WNDEDIT_ES_IME_AVAILABLE;
			break;
		}
		return TRUE;
	}
	return FALSE;
}

void KWndEdit::Show()   //显示窗口
{
    KWndWindow::Show();

	if (m_Flag & WNDEDIT_ES_WIN32_WND)
    {
        if (m_hWin32Wnd)
            Win32Edit_ShowWindow(m_hWin32Wnd, TRUE);
    }

}

void KWndEdit::Hide()   //隐藏窗口
{
    KWndWindow::Hide();

	if (m_Flag & WNDEDIT_ES_WIN32_WND)
    {
        if (m_hWin32Wnd)
            Win32Edit_ShowWindow(m_hWin32Wnd, FALSE);
    }

}

//--------------------------------------------------------------------------
//	功能：禁止或者允许窗口被操作
//--------------------------------------------------------------------------
void KWndEdit::Enable(int bEnable)
{
	if (bEnable)
	{
		m_Style &= ~WND_S_DISABLE;
	}
	else
	{
		if (m_Flag & WNDEDIT_F_HAVE_FOCUS)
			Wnd_SetFocusWnd(NULL);
		m_Style |= WND_S_DISABLE;
	}
	if (m_Flag & WNDEDIT_ES_WIN32_WND)
	{
		::EnableWindow(m_hWin32Wnd, bEnable);
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数（处理消息）
//--------------------------------------------------------------------------
int KWndEdit::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	if (m_Flag & WNDEDIT_ES_WIN32_WND)
	{
		if (uMsg == WM_KEYDOWN)
			nRet = OnKeyDownWin32(uParam, nParam);
		if (uMsg == WND_M_SET_FOCUS)
			Win32Edit_SetFocus(m_hWin32Wnd, true);
		else if (uMsg == WND_M_KILL_FOCUS)
			Win32Edit_SetFocus(m_hWin32Wnd, false);
		return 0;
	}

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		if (IsVisible() && Wnd_GetFocusWnd() != (KWndWindow*)this)
		{
			Wnd_SetFocusWnd((KWndWindow*)this);
		}
		else
			nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	case WM_IME_CHAR:
	case WM_CHAR:
		if ((m_Flag & WNDEDIT_F_HAVE_FOCUS) && ((m_Style & WND_S_DISABLE) == 0))
		{
			if (InsertChar(uParam))
				UpdateData();
			nRet = 1;
		}
		break;
	case WM_KEYDOWN:
		if (!IsDisable())
			nRet = OnKeyDown(uParam, nParam);
		break;
	case WND_M_SET_FOCUS:
		if (m_pText)
		{
			if (m_Flag & WNDEDIT_ES_IME_AVAILABLE)
			{
				s_Ime.EnableLanguageChange();
				if (s_LastTimeIsIme)
				    s_Ime.OpenIME();
			}

//			m_nCaretPos = m_nTextLen;	//把光标位置设在文本末尾
			m_Flag |= WNDEDIT_F_HAVE_FOCUS;
			m_nCaretTime = 0;

//			CreateCaret(::g_GetMainHWnd(), (HBITMAP)NULL, 2, m_nFontSize);
            m_nCaretX = m_nAbsoluteLeft;
            m_nCaretY = m_nAbsoluteTop;
            SetCaretPos(m_nCaretX, m_nCaretY);
            s_Ime.SetCaretPos(m_nCaretX, m_nCaretY);

//          ShowCaret(::g_GetMainHWnd());
		}
		break;
	case WND_M_KILL_FOCUS:
//        DestroyCaret();

		if (m_Flag & WNDEDIT_ES_IME_AVAILABLE)
			s_LastTimeIsIme = s_Ime.IsIme();
		s_Ime.DisableLanguageChange();
		s_Ime.CloseIME();
		m_Flag &= ~WNDEDIT_F_HAVE_FOCUS;
		break;
	case WM_SYSKEYDOWN:
		if (m_pParentWnd)
			nRet = m_pParentWnd->WndProc(uMsg, uParam, nParam);
		break;
	case WM_MOUSEWHEEL:
		if (!IsDisable())
		{
			int zDelta = GET_Y_LPARAM(uParam);
			if (zDelta != 0)
				nRet = OnKeyDown(zDelta < 0 ? VK_DOWN : VK_UP, 0);
		}
		break;

    case WM_MOVING:
    case WM_MOVE:
    {
        if (m_Flag & WNDEDIT_ES_WIN32_WND)
        {
    	    Win32Edit_UpdateSizePos(m_hWin32Wnd, m_nAbsoluteLeft, m_nAbsoluteTop, m_Width, m_Height);
            break;
        }
    }
	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KWndEdit::SetColor(unsigned int uColor)
{
	m_TextColor = uColor;
	if (m_Flag & WNDEDIT_ES_WIN32_WND)
		Win32Edit_SetTextColor(m_hWin32Wnd, uColor);
}

//--------------------------------------------------------------------------
//	功能：插入字符串
//--------------------------------------------------------------------------
int KWndEdit::InsertString(const char* pString, int nLen)
{
	int bInserted = false;
	if (m_Flag & WNDEDIT_ES_WIN32_WND)
	{
		bInserted = Win32Edit_InsertString(m_hWin32Wnd, pString, nLen);
	}
	else if (pString)
	{
		int bEnableChinese = (m_Flag & WNDEDIT_ES_IME_AVAILABLE);
		if (nLen < 0)
			nLen = strlen(pString);
		for (int nPos = 0; nPos < nLen; nPos++)
		{
			if ((unsigned char)(pString[nPos]) <= 0x80)
			{
				if (InsertChar(pString[nPos]))
					bInserted = true;
			}
			else if (nPos + 1 < nLen)
			{
				if (bEnableChinese && InsertChar(pString[nPos + 1], pString[nPos]))
					bInserted = true;
				nPos ++;
			}
		}
		if (bInserted)
			FmtForShow();
	}
	return bInserted;
}

int KWndEdit::Paste()
{
	int bPasted = false;
	if (OpenClipboard(NULL))
	{
		HGLOBAL hData = GetClipboardData(CF_TEXT);
		if (hData)
		{
			char* pszString = (char*)GlobalLock(hData);
            if (pszString)
            {
				bPasted = InsertString(pszString, -1);
				GlobalUnlock(hData);
				if (m_Flag & WNDEDIT_ES_MASK_CHARACTER)
					EmptyClipboard();
            }
		}
		CloseClipboard();
	}
	return bPasted;
}

int KWndEdit::Copy()
{
	int bCopyed = false;
	if ((m_Flag & WNDEDIT_ES_MASK_CHARACTER) == 0 &&
		m_nTextLen > 0 && m_pText)
	{
		if (OpenClipboard(g_GetMainHWnd()))
		{
			EmptyClipboard();
			HGLOBAL hData = (char*)GlobalAlloc(GMEM_MOVEABLE, m_nTextLen + 1);
			if (hData)
			{
				char* pszData = (char*)GlobalLock(hData);
				if (pszData)
				{
					memcpy(pszData, m_pText, m_nTextLen);
					pszData[m_nTextLen] = 0;
					GlobalUnlock(hData);
					SetClipboardData(CF_TEXT, hData);
					bCopyed = true;
				}
			}
			CloseClipboard();
		}
	}
	return bCopyed;
}

//--------------------------------------------------------------------------
//	功能：响应键盘按键操作
//--------------------------------------------------------------------------
int KWndEdit::OnKeyDownWin32(int nKeyCode, int nModifiers)
{
	int nRet = 1;
	if (m_pParentWnd)
	{
		if (nKeyCode == VK_UP ||
			nKeyCode == VK_DOWN ||
			nKeyCode == VK_TAB ||
			nKeyCode == VK_PRIOR ||
			nKeyCode == VK_NEXT ||
			nKeyCode == VK_ESCAPE ||
			nKeyCode == VK_RETURN)
		{
			nRet = m_pParentWnd->WndProc(WND_N_EDIT_SPECIAL_KEY_DOWN, (unsigned int)(KWndWindow*)this, nKeyCode);
		}
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应键盘按键操作
//--------------------------------------------------------------------------
int KWndEdit::OnKeyDown(int nKeyCode, int nModifiers)
{
	int nRet = 1;
	switch(nKeyCode)
	{
	case VK_DELETE:
		if (m_nCaretPos < m_nTextLen)
		{
			int nNextPos = TSplitString(m_pText + m_nCaretPos, 1, false);
			m_nTextLen -= nNextPos;
			memmove(m_pText + m_nCaretPos, m_pText + m_nCaretPos + nNextPos, m_nTextLen - m_nCaretPos + 1);
			UpdateData();
		}
		break;
	case VK_LEFT:
		if (m_nCaretPos)
		{
			m_nCaretPos = TSplitString(m_pText, m_nCaretPos - 1, true);
			FmtForShow();
		}
		break;
	case VK_RIGHT:
		if (m_nCaretPos < m_nTextLen)
		{
			m_nCaretPos += TSplitString(m_pText + m_nCaretPos, 1, false);
			FmtForShow();
		}
		break;
	case VK_HOME:
		m_nCaretPos = 0;
		FmtForShow();
		break;
	case VK_END:
		m_nCaretPos = m_nTextLen;
		FmtForShow();
		break;
	case VK_UP:
		if (MULTI_LINE && m_nCaretLineIndex)
		{
			int nCharPerLine = (m_Width * 2) / m_nFontSize;
			int nLineHead = 0, nLineEnd = 0;
			for (nRet = 0; nRet < m_nCaretLineIndex; nRet++)
			{
				nLineHead = nLineEnd;
				nLineEnd = nLineHead + SplitStringLine(&m_pText[nLineHead], 1, nCharPerLine);
			}

			m_nCaretPos = nLineHead + TSplitString(&m_pText[nLineHead],
				(m_nCaretPos - nLineEnd), false);
			if (m_nCaretPos >= nLineEnd && nLineEnd)
			{
				m_nCaretPos = nLineEnd;
				if(m_pText[nLineEnd - 1] == KTC_ENTER)
					m_nCaretPos--;
			}
			FmtForShow();
		}
		else if (m_pParentWnd)
			nRet = m_pParentWnd->WndProc(WND_N_EDIT_SPECIAL_KEY_DOWN, (unsigned int)(KWndWindow*)this, VK_UP);
		break;
	case VK_DOWN:
		if (MULTI_LINE)
		{
			int nCharPerLine = (m_Width * 2) / m_nFontSize;
			int nLineHead = 0, nLineEnd = 0;
			for (nRet = 0; nRet <= m_nCaretLineIndex; nRet++)
			{
				nLineHead = nLineEnd;
				nLineEnd = nLineHead + SplitStringLine(&m_pText[nLineHead], 1, nCharPerLine);
			}

			m_nCaretPos = nLineEnd + TSplitString(&m_pText[nLineEnd],
				(m_nCaretPos - nLineHead), false);
			nLineHead = nLineEnd;
			nLineEnd = nLineHead + SplitStringLine(&m_pText[nLineHead], 1, nCharPerLine);
			if (m_nCaretPos >= nLineEnd && nLineEnd)
			{
				m_nCaretPos = nLineEnd;
				if(m_pText[nLineEnd - 1] == KTC_ENTER)
					m_nCaretPos--;
			}
			FmtForShow();
		}
		else if (m_pParentWnd)
			nRet = m_pParentWnd->WndProc(WND_N_EDIT_SPECIAL_KEY_DOWN, (unsigned int)(KWndWindow*)this, VK_DOWN);
		break;
	case VK_TAB:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_ESCAPE:
		if (m_pParentWnd)
			nRet = m_pParentWnd->WndProc(WND_N_EDIT_SPECIAL_KEY_DOWN, (unsigned int)(KWndWindow*)this, nKeyCode);
		break;
	case VK_RETURN:
		if (m_pParentWnd)
		{
			if ((m_Flag & WNDEDIT_ES_MULTI_LINE) == 0)
				nRet = m_pParentWnd->WndProc(WND_N_EDIT_SPECIAL_KEY_DOWN, (unsigned int)(KWndWindow*)this, VK_RETURN);
			else if ((GetKeyState(VK_CONTROL) & 0x8000) == 0 && (GetKeyState(VK_SHIFT) & 0x8000) == 0)
				nRet = m_pParentWnd->WndProc(WND_N_EDIT_SPECIAL_KEY_DOWN, (unsigned int)(KWndWindow*)this, VK_RETURN);
			else if (InsertChar(0x0a, 0))
			{
				UpdateData();
				nRet = 1;
			}
		}
		break;
	case 'V':	//粘帖
		if ((GetKeyState(VK_CONTROL) & 0x8000) && Paste())
			nRet = 1;
		break;
	case 'C':	//复制
		if ((GetKeyState(VK_CONTROL) & 0x8000) && Copy())
			nRet = 1;
		break;
	default:
		if ((nKeyCode < '0' || nKeyCode > '9') &&
			(nKeyCode < 'A' || nKeyCode > 'Z') &&
			(nKeyCode < VK_NUMPAD0 || nKeyCode > VK_DIVIDE))
		{
			nRet = 0;
		}
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：插入字符操作
//	返回：是否有字符串是否被摆变
//--------------------------------------------------------------------------
int KWndEdit::InsertChar(int nChar)
{
	if (nChar > 0x80)	//此判断与语言编码规则相关
	{
		if (m_Flag & WNDEDIT_ES_IME_AVAILABLE)
			return (InsertChar((char)(nChar & 0xff), (char)(nChar >> 8)));
		else
			return false;
	}
	if (nChar == VK_BACK)		//退格
	{
		if (m_nCaretPos)
		{
			int nPrioPos = TSplitString(m_pText, m_nCaretPos - 1, true);
			memmove(m_pText + nPrioPos, m_pText + m_nCaretPos, m_nTextLen - m_nCaretPos + 1);
			m_nTextLen -= m_nCaretPos - nPrioPos;
			m_nCaretPos = nPrioPos;
			return true;
		}
	}
	else if (nChar >= ' ' && nChar <= '~')
	{
		if ((m_Flag & WNDEDIT_ES_CHAR_TYPE_FILTER) || (nChar >= '0' && nChar <= '9'))
			return (InsertChar((char)(nChar & 0xff), 0));
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：在字符串里执行插入字符操作
//	参数：cLow  -> 为要插入的为单字节字符；如果要插入为双字节字符，则为后一个字节。
//	      cHigh -> 要插入的为单字节字符时cHigh为0，如果要插入为双字节字符，则为前一个字节。
//	返回：字符串是否被改变
//--------------------------------------------------------------------------
int	KWndEdit::InsertChar(char cLow, char cHigh)
{
	int	nLen = cHigh ? 2 : 1;
	if (m_nCaretPos + nLen <= m_nLimitText)
	{
		if (m_nTextLen + nLen > m_nLimitText)
		{
			m_nTextLen = m_nCaretPos + TSplitString(m_pText + m_nCaretPos, m_nLimitText - nLen - m_nCaretPos, true);
			m_pText[m_nTextLen] = 0;
		}
		memmove(m_pText + m_nCaretPos + nLen, m_pText + m_nCaretPos, m_nTextLen - m_nCaretPos + 1);
		if (cHigh)
			m_pText[m_nCaretPos++] = cHigh;
		m_pText[m_nCaretPos++] = cLow;
		m_nTextLen += nLen;
		return 1;
	}
	return 0;
}

//--------------------------------------------------------------------------
//	功能：字符串被更新
//--------------------------------------------------------------------------
void KWndEdit::UpdateData()
{
	FmtForShow();
	if (m_pParentWnd)
	{	//发送更新消息
		m_pParentWnd->WndProc(WND_N_EDIT_CHANGE, (unsigned int)(KWndWindow*)this, 0);
	}
}

//--------------------------------------------------------------------------
//	功能：计算更新影响字符串显示的一些控制参数
//--------------------------------------------------------------------------
void KWndEdit::FmtForShow()
{
	if (m_pText == NULL || (m_Flag & WNDEDIT_ES_WIN32_WND))
		return;
	int	nLineLen = (m_Width * 2) / m_nFontSize;
	if ((m_Flag & WNDEDIT_ES_MULTI_LINE) == 0)
	//============================
	//		单行输入窗口处理
	//============================
	{
		int nJumpLen = 4;
		if (nLineLen < nJumpLen + 2)
			nJumpLen = 1;
		if (m_nCaretPos < m_nSkipAhead)
			m_nSkipAhead = TSplitString(m_pText, m_nCaretPos - nJumpLen, true);
		else if (m_nCaretPos > m_nSkipAhead + nLineLen - 1)
			m_nSkipAhead = TSplitString(m_pText, m_nCaretPos + nJumpLen - nLineLen, false);
		if (m_nSkipAhead + nLineLen >= m_nTextLen)
			m_nSkipBehind = -1;
		else
			m_nSkipBehind = m_nSkipAhead + TSplitString(m_pText + m_nSkipAhead, nLineLen, true);
	}
	//============================
	//		多行输入窗口处理
	//============================
	else
	{
		int	nLine = m_Height / (m_nFontSize + 1);
		if (m_nCaretPos < m_nSkipAhead)
		{
			m_nSkipBehind = 0;
			do
			{
				m_nSkipAhead = m_nSkipBehind;
				m_nSkipBehind = m_nSkipAhead + SplitStringLine(m_pText + m_nSkipAhead, 1, nLineLen);
			}while(m_nSkipBehind < m_nCaretPos);
		}
		m_nSkipBehind = m_nSkipAhead + SplitStringLine(m_pText + m_nSkipAhead, nLine, nLineLen);
		while (m_nCaretPos > m_nSkipBehind)
		{
			m_nSkipAhead = m_nSkipAhead + SplitStringLine(m_pText + m_nSkipAhead, 1, nLineLen);
			m_nSkipBehind = m_nSkipAhead + SplitStringLine(m_pText + m_nSkipAhead, nLine, nLineLen);
		}
		m_nCaretLineIndex = 0;
		int nLineHead = 0, nLineEnd = 0;
		while(true)
		{
			nLineHead = nLineEnd;
			nLineEnd = nLineHead + SplitStringLine(&m_pText[nLineHead], 1, nLineLen);
			if (nLineEnd >= m_nCaretPos)
				break;
			m_nCaretLineIndex ++;
		};
		if (m_nCaretPos > 0 && m_pText[m_nCaretPos -1] == KTC_ENTER)
		{
			m_nCaretLineIndex ++;
			int nTemp = m_nSkipAhead + SplitStringLine(m_pText + m_nSkipAhead, nLine - 1, nLineLen);
			if (nTemp < m_nSkipBehind)
			{
				m_nSkipAhead = m_nSkipAhead + SplitStringLine(m_pText + m_nSkipAhead, 1, nLineLen);
				m_nSkipBehind = m_nSkipAhead + SplitStringLine(m_pText + m_nSkipAhead, nLine, nLineLen);
			}
		}
	}
}

//--------------------------------------------------------------------------
//	功能：寻找字符串换行的位置
//--------------------------------------------------------------------------
int SplitStringLine(char* pString, int nLine, int nLineByte)
{
	int nPos = 0;
	if (pString)
	{
		int nLineOffset = 0;
		while(pString[nPos])
		{
			if ((BYTE)pString[nPos] > 0x80)
			{
				if (pString[nPos + 1])//防止出现单BYTE的高于0x80的字符
				{	//中文字符
					nLineOffset += 2;
					if (nLineOffset > nLineByte)
					{
						if ((--nLine) == 0)
							break;
						nLineOffset = 2;
					}
					nPos += 2;
					continue;
				}
			}
			nPos ++;
			if (pString[nPos - 1] == KTC_ENTER || (++nLineOffset) >= nLineByte)
			{
				if ((--nLine) == 0)
					break;
				nLineOffset = 0;
			}
		}
	}
	return nPos;
}

//--------------------------------------------------------------------------
//	功能：窗体绘制
//--------------------------------------------------------------------------
void KWndEdit::PaintWindow()
{
    //HideCaret(::g_GetMainHWnd());

	KWndWindow::PaintWindow();

	if (m_pText == NULL ||
		(m_Flag & WNDEDIT_ES_WIN32_WND))
    {
		return;
        //ShowCaret(::g_GetMainHWnd());
    }

	bool bShowCaret = false;
	if (m_Flag & WNDEDIT_F_HAVE_FOCUS)
	{
		if ( ++m_nCaretTime <= 10)
			bShowCaret = true;
		else if (m_nCaretTime >= 17)
			m_nCaretTime = 0;

		if (m_FocusBkColor && g_pRepresentShell)
		{
			KRUShadow	Shadow;
			Shadow.oPosition.nX = m_nAbsoluteLeft;
			Shadow.oPosition.nY = m_nAbsoluteTop;
			Shadow.oEndPos.nX = m_nAbsoluteLeft + m_Width;
			Shadow.oEndPos.nY = m_nAbsoluteTop + m_Height;
			Shadow.Color.Color_dw = m_FocusBkColor;
			g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
		}
	}

	if (MULTI_LINE == 0)
	{
		int nLen;
		if (m_nSkipBehind < 0)
			nLen = m_nTextLen - m_nSkipAhead;
		else
			nLen = m_nSkipBehind - m_nSkipAhead;

		if ((m_Flag & WNDEDIT_ES_MASK_CHARACTER) == 0)
		{
			g_pRepresentShell->OutputText(m_nFontSize, &m_pText[m_nSkipAhead],  nLen,
					m_nAbsoluteLeft, m_nAbsoluteTop, m_TextColor, 0,
					TEXT_IN_SINGLE_PLANE_COORD, m_TextBorderColor);
		}
		else
		{
			int	x = m_nAbsoluteLeft;
			for (; nLen > 1; nLen -= 2, x += m_nFontSize)
				g_pRepresentShell->OutputText(m_nFontSize, "**", 2, x, m_nAbsoluteTop, m_TextColor,
					0, TEXT_IN_SINGLE_PLANE_COORD, m_TextBorderColor);
			if (nLen)
				g_pRepresentShell->OutputText(m_nFontSize, "*", 1, x, m_nAbsoluteTop, m_TextColor,
					0, TEXT_IN_SINGLE_PLANE_COORD, m_TextBorderColor);
		}
		if (bShowCaret)
		{
			int nCaretX = m_nAbsoluteLeft + (m_nCaretPos - m_nSkipAhead) * m_nFontSize / 2 + 2;
			int nCaretY = m_nAbsoluteTop;

            if (
                (m_nCaretX != nCaretX) ||
                (m_nCaretY != nCaretY)
            )
            {
                m_nCaretX = nCaretX;
                m_nCaretY = nCaretY;
                SetCaretPos(m_nCaretX, m_nCaretY);
                s_Ime.SetCaretPos(m_nCaretX, m_nCaretY);
            }

            g_pRepresentShell->OutputText(m_nFontSize, "│", 2,
				nCaretX - m_nFontSize / 2, nCaretY, m_TextColor,
				0, TEXT_IN_SINGLE_PLANE_COORD, m_TextBorderColor);
		}
	}
	else
	{
		int	nLineBytes = (m_Width * 2) / m_nFontSize;
		int nLineAhead = m_nSkipAhead, nLineEnd;
		int y = m_nAbsoluteTop;
		do
		{
			nLineEnd  = nLineAhead + SplitStringLine(m_pText + nLineAhead, 1, nLineBytes);

			g_pRepresentShell->OutputText(m_nFontSize, m_pText + nLineAhead, nLineEnd - nLineAhead, m_nAbsoluteLeft, y,
				m_TextColor, m_Width, TEXT_IN_SINGLE_PLANE_COORD, m_TextBorderColor);

			if (bShowCaret && m_nCaretPos >= nLineAhead && m_nCaretPos <= nLineEnd)
			{
				int nCaretX, nCaretY;

				if (m_nCaretPos == nLineEnd && m_nCaretPos > 0 && m_pText[m_nCaretPos - 1] == KTC_ENTER)
				{
					nCaretX = m_nAbsoluteLeft - m_nFontSize / 2 + 2;
					nCaretY = y + m_nFontSize + 1;
				}
				else
				{
					nCaretX = m_nAbsoluteLeft + (m_nCaretPos - nLineAhead - 1) * m_nFontSize / 2 + 2;
					nCaretY = y;
				}

                if (
                    (m_nCaretX != nCaretX) ||
                    (m_nCaretY != nCaretY)
                )
                {
                    m_nCaretX = nCaretX;
                    m_nCaretY = nCaretY;
                    SetCaretPos(m_nCaretX, m_nCaretY);
                    s_Ime.SetCaretPos(m_nCaretX, m_nCaretY);
                }

				g_pRepresentShell->OutputText(m_nFontSize, "│", 2, nCaretX, nCaretY, m_TextColor,
					0, TEXT_IN_SINGLE_PLANE_COORD, m_TextBorderColor);
				bShowCaret = false;
			}
			y += m_nFontSize + 1;
			nLineAhead = nLineEnd;
		}while(nLineEnd < m_nSkipBehind);
	}

    //ShowCaret(::g_GetMainHWnd());
}

//--------------------------------------------------------------------------
//	功能：设置文本缓冲区指针
//--------------------------------------------------------------------------
void KWndEdit::SetTextPtr(char* pText, int nMaxLen)
{
	m_Flag  = 0;
	m_pText = pText;
	if ((m_nBufferSize = nMaxLen) <= 0)
		m_nBufferSize = 0;
	else if (m_nLimitText > m_nBufferSize - 1)
		m_nLimitText = m_nBufferSize - 1;
}

//--------------------------------------------------------------------------
//	功能：设置文本文字
//--------------------------------------------------------------------------
void KWndEdit::SetText(const char* pText)
{
	if (pText)
	{
		if (m_Flag & WNDEDIT_ES_WIN32_WND)
		{
			Win32Edit_SetText(m_hWin32Wnd, pText);
		}
		else if (m_pText)
		{
			m_nTextLen = strlen(pText);
			if (m_nTextLen > m_nLimitText)
				m_nTextLen = TSplitString(pText, m_nLimitText, true);
			memcpy(m_pText, pText, m_nTextLen);
			m_pText[m_nTextLen] = 0;
			m_nCaretPos = m_nTextLen;
			UpdateData();
		}
	}
}

void KWndEdit::ClearText(bool bSafe)
{
	if (m_Flag & WNDEDIT_ES_WIN32_WND)
	{
		Win32Edit_SetText(m_hWin32Wnd, "");
	}
	else if (m_pText)
	{
		m_nTextLen = 0;
		m_nCaretPos = 0;
		if (bSafe == false)
			m_pText[0] = 0;
		else
			memset(m_pText, 0, m_nBufferSize);
		UpdateData();
	}
}

//--------------------------------------------------------------------------
//	功能：设置数值文字
//--------------------------------------------------------------------------
void KWndEdit::SetIntText(int nNumber)
{
	char	Buff[16];
	sprintf(Buff, "%d", nNumber);
	SetText(Buff);
}

int	KWndEdit::GetIntNumber()
{
	int nResult = 0;
	if (m_Flag & WNDEDIT_ES_WIN32_WND)
	{
		char	Buff[16];
		if (GetText(Buff, sizeof(Buff), true) > 0)
		{
			Buff[sizeof(Buff) - 1] = '\0';
			nResult = atoi(Buff);
		}
	}
	else if (m_pText)
	{
		nResult = atoi(m_pText);
	}
	return nResult;
}

//--------------------------------------------------------------------------
//	功能：获取字符串内容
//--------------------------------------------------------------------------
int KWndEdit::GetText(char* pBuffer, int nSize, bool bExcludeSpace)
{
	if (pBuffer == NULL)
		return 0;
	int nRet = 0;
	if (m_Flag & WNDEDIT_ES_WIN32_WND)
	{
		return Win32Edit_GetText(m_hWin32Wnd, pBuffer, nSize, bExcludeSpace);
	}
	if (m_pText)
	{
		int nSkipAhead = 0;
		nRet = m_nTextLen;
		if (bExcludeSpace)
		{
			while(nRet && m_pText[nRet - 1] == ' ')
				nRet --;
			while(nSkipAhead < nRet && m_pText[nSkipAhead] == ' ')
				nSkipAhead ++;
		}

		if (nRet - nSkipAhead + 1 <= nSize)
		{
			memcpy(pBuffer, &m_pText[nSkipAhead], nRet - nSkipAhead);
			nRet -= nSkipAhead;
			pBuffer[nRet] = 0;
		}
		else
			nRet = -1;
	}
	return nRet;
}

void KWndEdit::GetCaretPosition(int* nx, int* ny)
{
	if (nx)
		*nx = (m_nCaretPos - m_nSkipAhead) * m_nFontSize / 2;
	if (ny)
		*ny = m_nCaretLineIndex  * m_nFontSize;
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndEdit32::KWndEdit32()
{
	m_Text[0] = m_Text[31] = 0;
	SetTextPtr(m_Text, 32);
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndEdit512::KWndEdit512()
{
	m_Text[0] = m_Text[511] = 0;
	SetTextPtr(m_Text, 512);
}