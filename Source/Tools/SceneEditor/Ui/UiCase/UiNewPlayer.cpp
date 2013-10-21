// -------------------------------------------------------------------------
//	文件名		：	UiNewPlayer.cpp
//	创建者		：	彭建波
//	创建时间	：	2002-9-10 14:25:24
//	功能描述	：	角色新建界面－1
//
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "UiNewPlayer.h"
#include "UiInformation.h"
#include "UiLoginBg.h"
#include "UiConnectInfo.h"
#include "UiSelNativePlace.h"
#include "UiSelPlayer.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../Engine/Src/Text.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI_NEWPLAYER 	"新建角色.ini"
#define	SERIES_INI				"\\Ui\\五行.ini"

static const char* s_szSeriesSectionList[series_num] =
{
	"Gold",
	"Wood",
	"Water",
	"Fire",
	"Earth",
};

const char* 	PROP_GetTitleString(OBJ_ATTRIBYTE_TYPE eType)
{
	_ASSERT(eType >= series_metal && eType < series_num);
	return (s_szSeriesSectionList[eType]);
}

KUiNewPlayer* KUiNewPlayer::m_pSelf=NULL;

KUiNewPlayer::KUiNewPlayer()
{
	memset(&m_Info, 0, sizeof(m_Info));
	m_Info.Attribute = series_metal;
	m_Info.Gender = OBJ_G_MALE;
	m_szLoginBg[0] = 0;
	memset(&m_propTypeInfoTable, 0, sizeof(m_propTypeInfoTable));
	m_propTypeInfoTable[0].pBtn = (KWndWindow*)&m_Gold;
	m_propTypeInfoTable[1].pBtn = (KWndWindow*)&m_Wood;
	m_propTypeInfoTable[2].pBtn = (KWndWindow*)&m_Water;
	m_propTypeInfoTable[3].pBtn = (KWndWindow*)&m_Fire;
	m_propTypeInfoTable[4].pBtn = (KWndWindow*)&m_Earth;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiNewPlayer* KUiNewPlayer::OpenWindow(int nNativePlaceId)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiNewPlayer;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->m_Info.NativePlaceId = nNativePlaceId;
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		m_pSelf->UpdateProperty();
		KUiLoginBackGround::SetConfig(m_pSelf->m_szLoginBg);
		m_pSelf->Show();
	}
	return m_pSelf;
}

KUiNewPlayer::~KUiNewPlayer()
{
	for (int i = 0; i < series_num; i++)
		m_propTypeInfoTable[i].pBtn = 0;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiNewPlayer::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->m_Name.ClearText(false);
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiNewPlayer::Initialize()
{
	AddChild(&m_Male);
	AddChild(&m_Female);
	AddChild(&m_Name);
	AddChild(&m_OK);
	AddChild(&m_Cancel);

	AddChild(&m_PropertyShow);
	AddChild(&m_Gold);
	AddChild(&m_Wood);
	AddChild(&m_Water);
	AddChild(&m_Fire);
	AddChild(&m_Earth);

	char Buff[256];
	KIniFile	Ini;
	if (Ini.Load(SERIES_INI))
	{
		for (int i = 0; i < series_num; i++)
		{
			Ini.GetString(s_szSeriesSectionList[i], "PropText", "", Buff, sizeof(Buff));
			m_propTypeInfoTable[i].nShowTextLen = strlen(Buff);
			m_propTypeInfoTable[i].nShowTextLen = TEncodeText(Buff, m_propTypeInfoTable[i].nShowTextLen);
			memcpy(m_propTypeInfoTable[i].propertyShow, Buff, m_propTypeInfoTable[i].nShowTextLen);

			Ini.GetString(s_szSeriesSectionList[i], "SelSound_m", "",
				m_propTypeInfoTable[i].szMaleSound, sizeof(m_propTypeInfoTable[i].szMaleSound));
			Ini.GetString(s_szSeriesSectionList[i], "SelSound_f", "",
				m_propTypeInfoTable[i].szFemaleSound, sizeof(m_propTypeInfoTable[i].szFemaleSound));
		}
	}

	g_UiBase.GetCurSchemePath(Buff, 256);
	m_pSelf->LoadScheme(Buff);

	SelGender();

	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiNewPlayer::SelGender()
{
	char	szFile[128];
	bool bMale = (m_Info.Gender == OBJ_G_MALE);
	KUiSelPlayer::GetRoleImageName(szFile, m_szPlayerImgPrefix, false, m_Info.Attribute, bMale ? 1 : 2);
	m_Male.CheckButton(bMale);
	m_Male.SetImage(ISI_T_SPR, szFile);
	m_Male.SetFrame(0);

	KUiSelPlayer::GetRoleImageName(szFile, m_szPlayerImgPrefix, true, m_Info.Attribute, bMale ? 2 : 1);
	m_Female.CheckButton(!bMale);
	m_Female.SetImage(ISI_T_SPR, szFile);
	m_Female.SetFrame(0);

	Wnd_SetFocusWnd(&m_Name);
	m_bJustClicked = true;
	UiSoundPlay(m_Info.Gender == OBJ_G_MALE ?
		m_propTypeInfoTable[m_Info.Attribute].szMaleSound :
		m_propTypeInfoTable[m_Info.Attribute].szFemaleSound);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiNewPlayer::LoadScheme(const char* pScheme)
{
	char		Buff[300];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_NEWPLAYER);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "NewPlayer");
		Ini.GetString("NewPlayer", "LoginBg", "", m_szLoginBg, sizeof(m_szLoginBg));
		Ini.GetString("NewPlayer", "PlayerImgPrefix", "", m_szPlayerImgPrefix, sizeof(m_szPlayerImgPrefix));

		m_Male  .Init(&Ini, "Male");
		m_Female.Init(&Ini, "Female");
		m_Male.SetImage(ISI_T_SPR, "");
		m_Female.SetImage(ISI_T_SPR, "");
		m_Name  .Init(&Ini, "Name");
		m_OK    .Init(&Ini, "OK");
		m_Cancel.Init(&Ini, "Cancel");

		m_PropertyShow.	Init(&Ini, "PropertyShow");

		for (int i = 0; i < series_num; i++)
		{
			m_propTypeInfoTable[i].pBtn->Init(&Ini, s_szSeriesSectionList[i]);
			Ini.GetString(s_szSeriesSectionList[i], "MaleImg", "",
				m_propTypeInfoTable[i].szMaleImg, sizeof(m_propTypeInfoTable[i].szMaleImg));
			Ini.GetString(s_szSeriesSectionList[i], "FemaleImg", "",
				m_propTypeInfoTable[i].szFemaleImg, sizeof(m_propTypeInfoTable[i].szFemaleImg));
		}
	}
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiNewPlayer::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndWindow*)uParam);
		break;
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
		{
			OnCancel();
			nRet = 1;
		}
		else if (uParam == VK_RETURN)
		{
			OnOk();
			nRet = 1;
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应点击按钮
//--------------------------------------------------------------------------
void KUiNewPlayer::OnClickButton(KWndWindow* pWnd)
{
	if (pWnd == (KWndWindow*)&m_Male)
	{
		UiSoundPlay(UI_SI_PLAYER_ATTRIB);
		m_Info.Gender = OBJ_G_MALE;
		SelGender();
	}
	else if (pWnd == (KWndWindow*)&m_Female)
	{
		UiSoundPlay(UI_SI_PLAYER_ATTRIB);
		m_Info.Gender = OBJ_G_FEMALE;
		SelGender();
	}
	else if (pWnd == (KWndWindow*)&m_OK)
		OnOk();
	else if (pWnd == (KWndWindow*)&m_Cancel)
		OnCancel();
	else
	{
		for (int i = 0; i < series_num; i++)
		{
			if (pWnd == m_propTypeInfoTable[i].pBtn)
			{
				m_Info.Attribute = i;
				break;
			}
		}
		if (i < series_num)
		{
			UiSoundPlay(UI_SI_PLAYER_ATTRIB);
		  	UpdateProperty();
		}
	}
}

//--------------------------------------------------------------------------
//	功能：更新属性说明
//--------------------------------------------------------------------------
void KUiNewPlayer::UpdateProperty()
{
	for (int i = 0; i < series_num; i++)
		((KWndButton*)m_propTypeInfoTable[i].pBtn)->CheckButton(i == m_Info.Attribute);

	if (m_Info.Attribute == series_metal)
	{
		m_Male.Enable(true);
		m_Info.Gender = OBJ_G_MALE;
		m_Female.Enable(false);
		SelGender();
	}
	else if (m_Info.Attribute == series_water)
	{
		m_Female.Enable(true);
		m_Info.Gender = OBJ_G_FEMALE;
		m_Male.Enable(false);
		SelGender();
	}
	else
	{
		m_Male.Enable(true);
		m_Female.Enable(true);
		SelGender();
	}

	m_PropertyShow.SetText(m_propTypeInfoTable[m_Info.Attribute].propertyShow,
		m_propTypeInfoTable[m_Info.Attribute].nShowTextLen);

	Wnd_SetFocusWnd(&m_Name);
}

//--------------------------------------------------------------------------
//	功能：完成角色选择界面
//--------------------------------------------------------------------------
void KUiNewPlayer::OnOk()
{
	if (GetInputInfo())
	{
		g_LoginLogic.CreateRole(&m_Info);
		KUiConnectInfo::OpenWindow(CI_MI_CREATING_ROLE, LL_S_IN_GAME, m_Info.NativePlaceId);
		CloseWindow(false);
	}
}

//--------------------------------------------------------------------------
//	功能：返回“角色选择界面”
//--------------------------------------------------------------------------
void KUiNewPlayer::OnCancel()
{
	CloseWindow(false);
	KUiSelNativePlace::OpenWindow(m_Info.NativePlaceId);
}

bool KUiNewPlayer::IsValidRoleName(const char* pszName, int nLen)
{
	if (pszName && nLen >= LOGIN_ROLE_NAME_MIN_LEN && nLen <= LOGIN_ROLE_NAME_MAX_LEN)
	{
		for (int i = 0; i < nLen;)
		{
			unsigned char	cCode = (unsigned char)pszName[i];
			if (cCode > 0x80)
			{
				if (i + 2 > nLen ||
					cCode == 0xa1 && (unsigned char)pszName[i + 1] == 0xa1)
					break;
				i += 2;
			}
			else if (cCode <= 0x20 || cCode > 0x7e)
				break;
			else
				i ++;
		}
		if (i == nLen)
			return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：获取输入的姓名别名信息
//--------------------------------------------------------------------------
int KUiNewPlayer::GetInputInfo()
{
	int nLen = m_Name.GetText(m_Info.Name, sizeof(m_Info.Name), false);
	if (IsValidRoleName(m_Info.Name, nLen))
		return true;
	CloseWindow(false);
	if (nLen < LOGIN_ROLE_NAME_MIN_LEN || nLen > LOGIN_ROLE_NAME_MAX_LEN)
	{
		//"姓名长度必须在3到8个汉字以内，允许输入英文字符，请重新输入姓名！"
		KUiConnectInfo::OpenWindow(CI_MI_INVALID_LOGIN_INPUT2, CI_NS_NEW_ROLE_WND, m_Info.NativePlaceId);
	}
	else
	{
		//"姓名中不可以包含空格、制表格等字符！"
		KUiConnectInfo::OpenWindow(CI_MI_INVALID_LOGIN_INPUT1, CI_NS_NEW_ROLE_WND);
	}
	return false;
}

void KUiNewPlayer::Breathe()
{
	int nLoopBackMale = m_Male.NextFrame();
	int nLoopBackFemale	= m_Female.NextFrame();

	if (m_bJustClicked)
	{
		char	szFileName[128];
		KWndImage* pBtn = NULL;
		if ((m_Info.Gender == OBJ_G_MALE) && nLoopBackMale)
			pBtn = &m_Male;
		else if (m_Info.Gender == OBJ_G_FEMALE && nLoopBackFemale)
			pBtn = &m_Female;
		if (pBtn)
		{
			KUiSelPlayer::GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_Info.Gender, m_Info.Attribute, 0);
			pBtn->SetImage(ISI_T_SPR, szFileName);
			pBtn->SetFrame(0);
			m_bJustClicked = false;
		}
	}
}