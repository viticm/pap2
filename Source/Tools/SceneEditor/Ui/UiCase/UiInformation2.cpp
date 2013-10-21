/*****************************************************************************************
//	界面--消息窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-14
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiInformation2.h"
#include "../../../core/src/coreshell.h"
#include "../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI	"提示2.ini"

KUiInformation2	g_UiInformation2;

void UIMessageBox2(const char* pMsg, int nMsgLen, const char* pBtnLabel,
				   KWndWindow* pCaller, unsigned int uParam)
{
	if (pCaller && (unsigned int)pCaller != WND_GAMESPACE)
		pCaller->Enable(false);
	g_UiInformation2.Show(pMsg, nMsgLen, pBtnLabel, pCaller, uParam);
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiInformation2::KUiInformation2()
{
	m_pCallerWnd = NULL;
	m_pWordDataList = NULL;
	m_nNumWordData = 0;
	m_nCurrentWord = 0;
}

//显示对话内容
void KUiInformation2::SpeakWords(KUiInformationParam* pWordDataList, int nCount)
{
	if (pWordDataList == NULL || nCount <= 0)
		return;
	KUiInformationParam* pNewList = (KUiInformationParam*)malloc(sizeof(KUiInformationParam) *(nCount + m_nNumWordData - m_nCurrentWord));
	if (pNewList == NULL)
		return;
	if (m_nNumWordData > m_nCurrentWord)
	{
		memcpy(pNewList, m_pWordDataList, sizeof(KUiInformationParam) * (m_nNumWordData - m_nCurrentWord));
		m_nCurrentWord = m_nNumWordData - m_nCurrentWord;
	}
	else
		m_nCurrentWord = 0;
	memcpy(&pNewList[m_nCurrentWord], pWordDataList, sizeof(KUiInformationParam) * nCount);
	m_nNumWordData = m_nCurrentWord + nCount;

	if (m_pWordDataList)
		free(m_pWordDataList);
	m_pWordDataList = pNewList;

	if (IsVisible() == false)
	{
		KWndWindow*	pCaller = m_pWordDataList->bNeedConfirmNotify ? ((KWndWindow*)WND_GAMESPACE) : 0;
		Show(m_pWordDataList->sInformation, m_pWordDataList->nInforLen,
				m_pWordDataList->sConfirmText, pCaller, 0);
		m_nCurrentWord ++;
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiInformation2::Initialize()
{
	m_pCallerWnd = NULL;
	AddChild(&m_Information);
	AddChild(&m_OKBtn);
	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_TOPMOST);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiInformation2::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_Information .Init(&Ini, "Info");
		m_OKBtn.Init(&Ini, "OK");
	}
}

//--------------------------------------------------------------------------
//	功能：显示窗口
//--------------------------------------------------------------------------
void KUiInformation2::Show(const char* pInformation, int nInforLen, const char* pBtnLabel,
				KWndWindow* pCallerWnd, unsigned int uParam)
{
	if (pInformation)
	{
		m_pCallerWnd = pCallerWnd;
		m_uCallerParam = uParam;
		m_Information.SetText(pInformation, nInforLen);
/*		if (pBtnLabel == NULL)
			m_OKBtn.Hide();
		else
		{
			m_OKBtn.SetText(pBtnLabel, -1);
			m_OKBtn.Show();
		}*/
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		BringToTop();
		KWndShowAnimate::Show();
		Wnd_SetExclusive((KWndWindow*)this);
	}
}

//--------------------------------------------------------------------------
//	功能：隐藏窗口
//--------------------------------------------------------------------------
void KUiInformation2::Hide()
{
	if (m_pCallerWnd)
	{
		if ((unsigned int)m_pCallerWnd == WND_GAMESPACE)
			g_pCoreShell->OperationRequest(GOI_INFORMATION_CONFIRM_NOTIFY, 0, 0);
		else
		{
			m_pCallerWnd->Enable(true);
			m_pCallerWnd->WndProc(WND_M_OTHER_WORK_RESULT, m_uCallerParam, 0);
		}
		m_pCallerWnd = NULL;
	}
	Wnd_ReleaseExclusive((KWndWindow*)this);

	if (m_nCurrentWord < m_nNumWordData)
	{
		KWndWindow*	pCaller = m_pWordDataList[m_nCurrentWord].bNeedConfirmNotify ?
								((KWndWindow*)WND_GAMESPACE) : 0;
		Show(m_pWordDataList[m_nCurrentWord].sInformation,
			m_pWordDataList[m_nCurrentWord].nInforLen,
			m_pWordDataList[m_nCurrentWord].sConfirmText, pCaller, 0);
		m_nCurrentWord ++;
	}
	else
		KWndShowAnimate::Hide();
	if (m_pWordDataList && m_nCurrentWord >= m_nNumWordData)
	{
		m_nNumWordData = 0;
		m_nCurrentWord = 0;
		free(m_pWordDataList);
		m_pWordDataList = NULL;
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiInformation2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
	case WM_LBUTTONUP:
//		if (uParam == (unsigned int)(KWndWindow*)&m_OKBtn)
		Hide();
		break;
	case WM_KEYDOWN:
		if (uParam  == VK_RETURN || uParam == VK_SPACE)
		{
			Hide();
			nRet = true;
		}
		else if (uParam == VK_ESCAPE)
		{
			while (m_pCallerWnd == NULL && IsVisible())
				Hide();
			Hide();
			nRet = true;
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

//关闭窗口，不通知调用窗口
void KUiInformation2::Close()
{
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndShowAnimate::Hide();
	if (m_pWordDataList)
	{
		free(m_pWordDataList);
		m_pWordDataList = NULL;
	}
	m_nNumWordData = 0;
	m_nCurrentWord = 0;
}

KUiInformation2::~KUiInformation2()
{
	Close();
}