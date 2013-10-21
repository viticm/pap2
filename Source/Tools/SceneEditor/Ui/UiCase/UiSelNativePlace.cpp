/*****************************************************************************************
//	界面--选择出生地窗口
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-6-7
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "../UiSoundSetting.h"
#include "UiSelNativePlace.h"
#include "UiLoginBg.h"
#include "UiSelPlayer.h"
#include "UiNewPlayer.h"
#include "../../Login/Login.h"
#include "../../../Engine/Src/Text.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI_SELSERV			"选新手村.ini"
#define	PLACE_LIST_FILE				"\\Settings\\NativePlaceList.ini"

KUiSelNativePlace* KUiSelNativePlace::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiSelNativePlace::KUiSelNativePlace()
{
	m_pPlaceList = NULL;
	m_szLoginBg[0] = 0;
	m_nLastSelPlace = 0;
}

KUiSelNativePlace::~KUiSelNativePlace()
{
	m_List.SetContent(NULL, 0, 0, 0);
	if (m_pPlaceList)
	{
		free(m_pPlaceList);
		m_pPlaceList = NULL;
	}
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiSelNativePlace* KUiSelNativePlace::OpenWindow(int nPlaceId)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSelNativePlace;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->LoadList(nPlaceId);
		m_pSelf->UpdateData();
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		KUiLoginBackGround::SetConfig(m_pSelf->m_szLoginBg);
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口
//--------------------------------------------------------------------------
void KUiSelNativePlace::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->Hide();
		}
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiSelNativePlace::Initialize()
{
	AddChild(&m_List);
	AddChild(&m_PlaceImg);
	AddChild(&m_Ok);
	AddChild(&m_Cancel);
	AddChild(&m_PlaceDesc);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiSelNativePlace::LoadList(int nPlaceId)
{
	if (m_pPlaceList)
	{
		free(m_pPlaceList);
		m_pPlaceList = NULL;
	}
	m_List.SetContent(NULL, 0, 0, 0);

	KIniFile	File;
	if (!File.Load(PLACE_LIST_FILE))
		return;
	int nCount;
	File.GetInteger("List", "Count", 0, &nCount);
	if (nCount <= 0)
		return;
	m_pPlaceList = (KNativePlace*)malloc(sizeof(KNativePlace) * nCount);
	if (m_pPlaceList == NULL)
		return;

	char	szSection[4], sBuffer[320];
	int		nNumValid = 0;
	m_nLastSelPlace = 0;
	for (int i = 0; i < nCount; i++)
	{
		itoa(i, szSection, 10);
		int	nValue;
		if (!File.GetInteger(szSection, "Id", 0, &nValue))
			continue;
		m_pPlaceList[nNumValid].nId = nValue;
		if (!File.GetString(szSection, "Name", "", m_pPlaceList[nNumValid].szName, sizeof(m_pPlaceList->szName)))
			continue;
		if (!File.GetString(szSection, "Img", "", m_pPlaceList[nNumValid].szImage,
				sizeof(m_pPlaceList[nNumValid].szImage)))
			continue;
		m_pPlaceList[nNumValid].nDescLen = 0;
		m_pPlaceList[nNumValid].sDesc[0] = 0;
		if (File.GetString(szSection, "Desc", "", sBuffer, sizeof(sBuffer)))
		{
			m_pPlaceList[nNumValid].nDescLen = strlen(sBuffer);
			m_pPlaceList[nNumValid].nDescLen = TEncodeText(sBuffer, m_pPlaceList[nNumValid].nDescLen);
			if (m_pPlaceList[nNumValid].nDescLen < sizeof(m_pPlaceList[nNumValid].sDesc))
				memcpy(m_pPlaceList[nNumValid].sDesc, sBuffer, m_pPlaceList[nNumValid].nDescLen);
			else
				m_pPlaceList[nNumValid].nDescLen = 0;
		}
		if (m_pPlaceList[nNumValid].nId == nPlaceId)
			m_nLastSelPlace = nNumValid;
		nNumValid ++;
	}
	if (nNumValid)
	{
		m_List.SetContent((BYTE*)m_pPlaceList, nNumValid, sizeof(KNativePlace), (int)(&((KNativePlace*)0)->szName));
		m_List.SetCurSel(m_nLastSelPlace);
	}
	else
	{
		free(m_pPlaceList);
		m_pPlaceList = NULL;
	}
	m_nLastSelPlace = 0;
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiSelNativePlace::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SELSERV);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_List     .Init(&Ini, "List");
		m_PlaceImg	.Init(&Ini, "PlaceImg");
		m_Ok	   .Init(&Ini, "OK");
		m_Cancel   .Init(&Ini, "Cancel");
		m_PlaceDesc.Init(&Ini, "PlaceDescText");

		Ini.GetString("Main", "LoginBg", "", m_szLoginBg, sizeof(m_szLoginBg));
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiSelNativePlace::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Ok)
			OnOk();
		else if (uParam == (unsigned int)(KWndWindow*)&m_Cancel)
			OnCancel();
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
			OnOk();
		else if (uParam == VK_ESCAPE)
			OnCancel();
		else if (uParam == VK_UP)
		{
			nRet = m_List.GetCurSel();
			if (nRet  > 0)
				m_List.SetCurSel(nRet - 1);
			else if (nRet < 0)
				m_List.SetCurSel(0);
		}
		else if (uParam == VK_DOWN)
		{
			nRet = m_List.GetCurSel();
			if (nRet < m_List.GetCount() - 1)
				m_List.SetCurSel(nRet + 1);
			else if (nRet < 0)
				m_List.SetCurSel(0);
		}
		nRet = 1;
		break;
	case WND_N_LIST_ITEM_SEL:
		if (nParam != m_nLastSelPlace &&
			uParam == (unsigned int)(KWndWindow*)&m_List)
		{
			if (nParam >= 0)
			{
				m_nLastSelPlace = nParam;
				UpdateData();
			}
			else
			{
				m_List.SetCurSel(m_nLastSelPlace);
			}
		}
		break;
	case WND_N_LIST_ITEM_D_CLICK:
		if (nParam >= 0 && uParam == (unsigned int)(KWndWindow*)&m_List)
			OnOk();
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：选择确定
//--------------------------------------------------------------------------
void KUiSelNativePlace::OnOk()
{
	int nSelPlace = m_List.GetCurSel();
	if (m_pPlaceList && nSelPlace >= 0)
	{
		KUiNewPlayer::OpenWindow(m_pPlaceList[nSelPlace].nId);
		CloseWindow(false);
	}
}

//--------------------------------------------------------------------------
//	功能：取消
//--------------------------------------------------------------------------
void KUiSelNativePlace::OnCancel()
{
	CloseWindow(true);
	KUiSelPlayer::OpenWindow();
}

void KUiSelNativePlace::UpdateData()
{
	if (m_pPlaceList == NULL)
		return;
	m_PlaceImg.SetImage(ISI_T_BITMAP16, m_pPlaceList[m_nLastSelPlace].szImage);
	m_PlaceDesc.SetText(m_pPlaceList[m_nLastSelPlace].sDesc, m_pPlaceList[m_nLastSelPlace].nDescLen);
}