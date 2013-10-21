// UiFollowInfo.cpp: implementation of the KUiFollowInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KIniFile.h"

#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"

#include "UiFollowInfo.h"
#include "../UiSoundSetting.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI	"提示3.ini"
//#define	SCHEME_INI	"提示.ini"

KUiFollowInfo	g_UiFollowInfo;

void UIMessageBox3(const char* pMsg,
				  KWndWindow* pCaller /*= 0*/,
				  const char* pszFirstBtnText /*= "确定"*/,
				  const char* pszSecondBtnText /*= 0*/,
				  const char* pszCancelBtnText /*= 0*/,
				  unsigned int uParam /*= 0*/)
{
	g_UiFollowInfo.Show(pMsg, pszFirstBtnText, pszSecondBtnText,
		pszCancelBtnText, pCaller, uParam);
}

void UiCloseMessageBox3()
{
	g_UiFollowInfo.Close();
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiFollowInfo::KUiFollowInfo()
{
	m_pCallerWnd = NULL;
	m_nOrigFirstBtnXPos = 0;
	m_nCentreBtnXPos = 0;
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiFollowInfo::Initialize()
{
	m_pCallerWnd = NULL;
	AddChild(&m_Information);
	AddChild(&m_FirstBtn);
	AddChild(&m_SecondBtn);
	AddChild(&m_BtnClose);
	m_Style &= ~WND_S_VISIBLE;
	m_Style &= WND_S_SIZE_WITH_ALL_CHILD;
	Wnd_AddWindow(this, WL_TOPMOST);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiFollowInfo::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		KWndShadow::Init(&Ini, "Main");
		m_Information .Init(&Ini, "Info");
		m_FirstBtn .Init(&Ini, "FirstBtn");
		m_SecondBtn.Init(&Ini, "SecondBtn");
		m_BtnClose.Init(&Ini, "BtnClose");
		m_FirstBtn.GetPosition(&m_nOrigFirstBtnXPos, 0);
		m_FirstBtn.GetSize(&m_nCentreBtnXPos, 0);
		m_nCentreBtnXPos = (m_Width - m_nCentreBtnXPos) / 2;
		SetExactWnd(TRUE);
	}
}

//--------------------------------------------------------------------------
//	功能：显示窗口
//--------------------------------------------------------------------------
void KUiFollowInfo::Show(const char* pInformation,
				const char* pszFirstBtnText,
				const char* pszSecondBtnText /*= 0*/,
				const char* pszCancelBtnText /*= 0*/,
				KWndWindow* pCallerWnd /*= 0*/,
				unsigned int uParam /*= 0*/,
                int nInformationLen /*= -1*/)
{
	if (pInformation == NULL)
		return;

	m_pCallerWnd = pCallerWnd;
	m_uCallerParam = uParam;
	m_Information.SetText(pInformation, nInformationLen);

	if (pszSecondBtnText && pszFirstBtnText == NULL)
	{
		pszFirstBtnText = pszSecondBtnText;
		pszSecondBtnText = NULL;
	}

	int nWidth, nHeight, nFontSize, nLen, nPosX, nPosY;

	m_Information.GetSize(&nWidth, &nHeight);
	m_Information.GetPosition(&nPosX, &nPosY);
	nFontSize = m_Information.GetFontSize();
	if(nInformationLen == -1)
	{
		nLen = strlen(pInformation);
	}
	else
	{
		nLen = nInformationLen;
	}
	nWidth = (nLen + 4) * nFontSize / 2;
	m_Information.SetSize(nWidth, nHeight);
	nPosX += nWidth + nFontSize;
	KWndWindow::m_Width = nPosX;

	if (pszFirstBtnText)
	{
		m_FirstBtn.SetText(pszFirstBtnText);
		m_FirstBtn.SetPosition(nPosX, nPosY);
		m_FirstBtn.GetSize(&nWidth, &nHeight);
		nFontSize = m_FirstBtn.GetFontSize();
		nWidth = strlen(pszFirstBtnText) * nFontSize / 2 + 1;
		m_FirstBtn.SetSize(nWidth, nHeight);
		if(pszSecondBtnText)
			nPosX += nWidth + nFontSize;
		else
			nPosX += nWidth;
		KWndWindow::m_Width = nPosX;
		m_FirstBtn.Show();
	}
	else
	{
		m_FirstBtn.Hide();
	}

	if (pszSecondBtnText)
	{
		m_SecondBtn.SetText(pszSecondBtnText);
		m_SecondBtn.SetPosition(nPosX, nPosY);
		m_SecondBtn.GetSize(&nWidth, &nHeight);
		nFontSize = m_SecondBtn.GetFontSize();
		nWidth = strlen(pszSecondBtnText) * nFontSize / 2 + 1;
		m_SecondBtn.SetSize(nWidth, nHeight);
		nPosX += nWidth + ((pszCancelBtnText) ? nFontSize : 0 );
		KWndWindow::m_Width = nPosX + nWidth;
		m_SecondBtn.Show();
	}
	else
	{
		m_SecondBtn.Hide();
	}

	if (pszCancelBtnText)
	{
		m_BtnClose.SetText(pszCancelBtnText);
		m_BtnClose.SetPosition(nPosX, nPosY);
		m_BtnClose.GetSize(&nWidth, &nHeight);
		nFontSize = m_BtnClose.GetFontSize();
		nWidth = strlen(pszCancelBtnText) * nFontSize / 2 + 1;
		m_BtnClose.SetSize(nWidth, nHeight);
		KWndWindow::m_Width = nPosX + nWidth;
		m_BtnClose.Show();
	}
	else
	{
		m_BtnClose.Hide();
	}

	Wnd_GetCursorPos(&nPosX, &nPosY);
	nPosX -= KWndWindow::m_Width;
	nPosY -= KWndWindow::m_Height;
	if(nPosX < 0) nPosX = 0;
	if(nPosY < 0) nPosY = 0;
	KWndWindow::SetPosition(nPosX, nPosY);
	UiSoundPlay(UI_SI_WND_OPENCLOSE);
	BringToTop();
	KWndWindow::Show();
}

//--------------------------------------------------------------------------
//	功能：隐藏窗口
//--------------------------------------------------------------------------
void KUiFollowInfo::Hide(int nBtnIndex)
{
	if (m_pCallerWnd)
	{
		m_pCallerWnd->WndProc(WND_M_OTHER_WORK_RESULT,
			m_uCallerParam, WND_OPER_RESULT(nBtnIndex));
		m_pCallerWnd = NULL;
	}
	KWndWindow::Hide();
}

//关闭窗口，不通知调用窗口
void KUiFollowInfo::Close()
{
	KWndWindow::Hide();
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiFollowInfo::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_FirstBtn)
			Hide(0);
		else if (uParam == (unsigned int)(KWndWindow*)&m_SecondBtn)
			Hide(1);
		else if(uParam == (unsigned int)(KWndWindow*)&m_BtnClose)
			Hide(2);
		break;
	case WM_KEYDOWN:
		if (uParam  == VK_RETURN || uParam == VK_SPACE || uParam == VK_ESCAPE)
		{
			if (!m_SecondBtn.IsVisible())
			{
				Hide(m_FirstBtn.IsVisible() ? 0 : -1);
				nRet = true;
			}
		}
		break;
	default:
		nRet = KWndShadow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}