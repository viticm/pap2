// -------------------------------------------------------------------------
//	文件名		：	UiSelPlayer.cpp
//	创建者		：	吕桂华
//	创建时间	：	2002-9-10 14:24:12
//	功能描述	：
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiSelPlayer.h"
#include "UiSelNativePlace.h"
#include "UiSelServer.h"
#include "UiLoginbg.h"
#include "UiConnectInfo.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "../../Login/Login.h"
#include "../../Core/Src/GameDataDef.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include <time.h>
#include <assert.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;

#define	SCHEME_INI_SELPLAYER 	"选游戏存档人物.ini"
#define	SERIES_INI				"\\Ui\\五行.ini"
#define	LIFE_TIME_LINE_ID		"32"
#define	LIFE_TIME_AND_STRING_ID	"33"
#define	LIFE_TIME_STRING_ID		"25"
#define	LIFE_TIME_DAY_ID		"26"
#define	LIFE_TIME_HOUR_ID		"27"
#define	LIFE_TIME_MINUTE_ID		"28"
#define	LIFE_TIME_SECOND_ID		"29"

const char*		PROP_GetTitleString(OBJ_ATTRIBYTE_TYPE eType);

KUiSelPlayer* KUiSelPlayer::m_pSelf=NULL;

//--------------------------------------------------------------------------
//	功能：析构函数
//--------------------------------------------------------------------------
KUiSelPlayer::KUiSelPlayer()
{
	m_nSelPlayer = -1;
	m_nNumPlayer = 0;
	m_szLoginBg[0] = 0;
	m_szPlayerImgPrefix[0] = 0;
	m_bJustClicked = false;
	m_szSelPlayerSound[0][0] = m_szSelPlayerSound[1][0] =
		m_szSelPlayerSound[2][0] = m_szSelPlayerSound[3][0] =
		m_szSelPlayerSound[4][0] = 0;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiSelPlayer* KUiSelPlayer::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSelPlayer;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		m_pSelf->UpdateData();
		KUiLoginBackGround::SetConfig(m_pSelf->m_szLoginBg);
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiSelPlayer::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->m_bJustClicked = false;
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

void KUiSelPlayer::Hide()
{
	m_pSelf->m_bJustClicked = false;
	KWndShowAnimate::Hide();
}

// -------------------------------------------------------------------------
// 函数		: KUiSelPlayer::Initialize
// 功能		: 初始化
// -------------------------------------------------------------------------
void KUiSelPlayer::Initialize()
{
	AddChild(&m_btnOk);
	AddChild(&m_btnCancel);
	AddChild(&m_btnDel);
	AddChild(&m_btnNew);
	AddChild(&m_ButterflyWnd);
	AddChild(&m_LifeTimeText);

	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 128);
	m_pSelf->LoadScheme(Scheme);

	// 注册本窗口
	Wnd_AddWindow(this, WL_TOPMOST);
}

// -------------------------------------------------------------------------
// 函数		: KUiSelPlayer::LoadScheme
// 功能		: 载入界面方案
// -------------------------------------------------------------------------
void KUiSelPlayer::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SELPLAYER);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "SelRole");	// 窗口背景数据
		Ini.GetString("SelRole", "LoginBg", "", m_szLoginBg, sizeof(m_szLoginBg));
		Ini.GetString("SelRole", "PlayerImgPrefix", "", m_szPlayerImgPrefix, sizeof(m_szPlayerImgPrefix));

		Buff[0] = '\0';	// 清空缓存
		m_btnOk.Init(&Ini, "Ok");
		m_btnCancel.Init(&Ini, "Cancel");
		m_btnDel.Init(&Ini, "Del");
		m_btnNew.Init(&Ini, "New");
		m_ButterflyWnd.Init(&Ini, "Butterfly");

		m_player[0].Init(&Ini, "Player");
		m_Name[0].Init(&Ini, "Name");
		m_Level[0].Init(&Ini, "Level");
		m_PlayerInfoBg[0].Init(&Ini, "PlayerInfoBg");
		for (int i = 1; i < MAX_PLAYER_PER_ACCOUNT; i ++)
		{
			m_player[0].Clone(&m_player[i]);
			m_Name[0].Clone(&m_Name[i]);
			m_Level[0].Clone(&m_Level[i]);
			m_PlayerInfoBg[0].Clone(&m_PlayerInfoBg[i]);
		}

		Ini.GetInteger("SelRole", "Player2Pos_0",  0, &m_ChildWndXOffset[0]);
		Ini.GetInteger("SelRole", "Player2Pos_1",  0, &m_ChildWndXOffset[1]);
		Ini.GetInteger("SelRole", "Player3Pos_0",  0, &m_ChildWndXOffset[2]);
		Ini.GetInteger("SelRole", "Player3Pos_1",  0, &m_ChildWndYOffset);
		Ini.GetInteger("SelRole", "Player3Pos_2",  0, &m_ChildWndXOffset[3]);

		m_player[0].GetPosition(&m_ChildPos[0], NULL);
		m_Name[0].GetPosition(&m_ChildPos[1], NULL);
		m_Level[0].GetPosition(&m_ChildPos[2], NULL);
		m_PlayerInfoBg[0].GetPosition(&m_ChildPos[3], NULL);

		m_LifeTimeText.Init(&Ini, "LifeTime");
	}
}

// -------------------------------------------------------------------------
// 功能	: 绘制窗口
// -------------------------------------------------------------------------
void KUiSelPlayer::PaintWindow()
{
	KWndShowAnimate::PaintWindow();
	for (int i = 0; i < m_nNumPlayer; i++)
	{
		int nLoopBack = m_player[i].NextFrame();
		if (m_bJustClicked && nLoopBack && i == m_nSelPlayer)
		{
			char	szFileName[128];
			GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_Gender[m_nSelPlayer], m_Attribute[m_nSelPlayer], 0);
			m_player[m_nSelPlayer].SetImage(ISI_T_SPR, szFileName);
			m_player[m_nSelPlayer].SetFrame(0);
			m_bJustClicked = false;
		}
	}
}

// -------------------------------------------------------------------------
// 功能	: 更新
// -------------------------------------------------------------------------
void KUiSelPlayer::UpdateData()
{
	int nSel;
	m_nNumPlayer = g_LoginLogic.GetRoleCount(nSel);
	if (m_nNumPlayer > 0 && nSel < 0)
		m_nSelPlayer = 0;
	else
		m_nSelPlayer = nSel;

	int	x, y;
	//---调整子窗口位置----
	switch(m_nNumPlayer)
	{
	case 1:
		m_player[0].GetPosition(&x, &y);
		x = m_ChildPos[0];
		m_player[0].SetPosition(x, y);
		m_Name[0].GetPosition(&x, &y);
		x = m_ChildPos[1];
		m_Name[0].SetPosition(x, y);
        m_Level[0].GetPosition(&x, &y);
		x = m_ChildPos[2];
		m_Level[0].SetPosition(x, y);
        m_PlayerInfoBg[0].GetPosition(&x, &y);
		x = m_ChildPos[3];
		m_PlayerInfoBg[0].SetPosition(x, y);
		break;
	case 2:
		m_player[0].GetPosition(&x, &y);
		x = m_ChildPos[0] + m_ChildWndXOffset[0];
		m_player[0].SetPosition(x, y);
		x = m_ChildPos[0] + m_ChildWndXOffset[1];
		m_player[1].SetPosition(x, y);

		m_Name[0].GetPosition(&x, &y);
		x = m_ChildPos[1] + m_ChildWndXOffset[0];
		m_Name[0].SetPosition(x, y);
		x = m_ChildPos[1] + m_ChildWndXOffset[1];
		m_Name[1].SetPosition(x, y);

        m_Level[0].GetPosition(&x, &y);
		x = m_ChildPos[2] + m_ChildWndXOffset[0];
		m_Level[0].SetPosition(x, y);
		x = m_ChildPos[2] + m_ChildWndXOffset[1];
		m_Level[1].SetPosition(x, y);

        m_PlayerInfoBg[0].GetPosition(&x, &y);
		x = m_ChildPos[3] + m_ChildWndXOffset[0];
		m_PlayerInfoBg[0].SetPosition(x, y);
		x = m_ChildPos[3] + m_ChildWndXOffset[1];
		m_PlayerInfoBg[1].SetPosition(x, y);
		break;
	case 3:
		m_player[0].GetPosition(&x, &y);
		x = m_ChildPos[0];
		m_player[1].SetPosition(x, y + m_ChildWndYOffset);
		x += m_ChildWndXOffset[2];
		m_player[0].SetPosition(x, y);
		x = m_ChildPos[0] + m_ChildWndXOffset[3];
		m_player[2].SetPosition(x, y);

		m_Name[0].GetPosition(&x, &y);
		x = m_ChildPos[1];
		m_Name[1].SetPosition(x, y + m_ChildWndYOffset);
		x += m_ChildWndXOffset[2];
		m_Name[0].SetPosition(x, y);
		x = m_ChildPos[1] + m_ChildWndXOffset[3];
		m_Name[2].SetPosition(x, y);

        m_Level[0].GetPosition(&x, &y);
		x = m_ChildPos[2];
		m_Level[1].SetPosition(x, y + m_ChildWndYOffset);
		x += m_ChildWndXOffset[2];
		m_Level[0].SetPosition(x, y);
		x = m_ChildPos[2] + m_ChildWndXOffset[3];
		m_Level[2].SetPosition(x, y);

        m_PlayerInfoBg[0].GetPosition(&x, &y);
		x = m_ChildPos[3];
		m_PlayerInfoBg[1].SetPosition(x, y + m_ChildWndYOffset);
		x += m_ChildWndXOffset[2];
		m_PlayerInfoBg[0].SetPosition(x, y);
		x = m_ChildPos[3] + m_ChildWndXOffset[3];
		m_PlayerInfoBg[2].SetPosition(x, y);
		break;
	}

	int		i;
	char	szFileName[128];
	if (m_nNumPlayer > 0)
	{
		KIniFile	Ini;
		Ini.Load(SERIES_INI);

		for (i = 0; i < m_nNumPlayer; i++)
		{
			KRoleChiefInfo	Info;
			g_LoginLogic.GetRoleInfo(i, &Info);
			m_Name[i].SetText(Info.Name);
			m_Level[i].SetIntText(Info.nLevel);
			AddChild(&m_PlayerInfoBg[i]);
			AddChild(&m_player[i]);
			AddChild(&m_Name[i]);
			AddChild(&m_Level[i]);
			m_Gender[i] = Info.Gender;
			m_Attribute[i] = Info.Attribute;

			m_szSelPlayerSound[i][0] = 0;
			Ini.GetString(PROP_GetTitleString((OBJ_ATTRIBYTE_TYPE)Info.Attribute),
				Info.Gender ? "SelSound_f" : "SelSound_m", "",
				m_szSelPlayerSound[i], sizeof(m_szSelPlayerSound[i]));
			GetRoleImageName(szFileName, m_szPlayerImgPrefix, Info.Gender, Info.Attribute, 2);
			m_player[i].SetImage(ISI_T_SPR, szFileName);
			m_player[i].SetFrame(0);
		}

		if (m_nSelPlayer < 0)
			m_nSelPlayer = 0;

		OnSelectPlayer(m_nSelPlayer);
	}

	for (i = m_nNumPlayer; i < MAX_PLAYER_PER_ACCOUNT; i++)
	{
		m_player[i].LeaveAlone();
		m_Name[i].LeaveAlone();
		m_Level[i].LeaveAlone();
		m_PlayerInfoBg[i].LeaveAlone();
	}

	m_btnOk.BringToTop();
	m_btnCancel.BringToTop();
	m_btnDel.BringToTop();
	m_btnNew.BringToTop();

	m_btnNew.Enable(m_nNumPlayer < MAX_PLAYER_PER_ACCOUNT);
	m_btnDel.Enable(m_nNumPlayer > 0 && m_nSelPlayer >= 0);
	m_btnOk.Enable(m_nNumPlayer > 0 && m_nSelPlayer >= 0);

	unsigned long uTotalTime, uTimeOfPoint;
	g_LoginLogic.GetAccountLifeTime(uTotalTime, uTimeOfPoint);
	unsigned long uTimeLine = (uTotalTime - uTimeOfPoint);

	char	szInfo[256];
	int		nLen;
	KIniFile* pSetting = g_UiBase.GetCommConfigFile();

	szInfo[0] = '\0';
	nLen = 0;
	//----包月或包周----
	if (uTimeLine)
	{
		if (pSetting)
		{
			pSetting->GetString("InfoString", LIFE_TIME_LINE_ID,
				"", szInfo, sizeof(szInfo));
			nLen = strlen(szInfo);
		}
		time_t t;
		time(&t);
		t += uTimeLine;
		struct tm * lt = localtime(&t);
		assert(lt);
		//校正处理
		if (lt->tm_hour > 12)
		{
			t += 43200;//半天
			lt = localtime(&t);
			assert(lt);
		}
		nLen += sprintf(szInfo + nLen, " %d-%d-%d ",
			lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday);
	}

	if (uTimeOfPoint || uTimeLine == 0)
	{
		if (uTimeLine)
			nLen = strlen(strcat(szInfo, "   "));

		int		nHours, nMinutes;
		nHours = uTimeOfPoint / 3600;
		uTimeOfPoint -= nHours * 3600;
		nMinutes = uTimeOfPoint / 60;

		char	szHour[16] = "hour(s)";
		char	szMinute[16] = "minute(s)";

		if (pSetting)
		{
			pSetting->GetString("InfoString",
				uTimeLine ? LIFE_TIME_AND_STRING_ID : LIFE_TIME_STRING_ID,
				"", szInfo + nLen, sizeof(szInfo) - nLen);
			nLen = strlen(szInfo);
			pSetting->GetString("InfoString", LIFE_TIME_HOUR_ID,
				"hour(s)", szHour, sizeof(szHour));
			pSetting->GetString("InfoString", LIFE_TIME_MINUTE_ID,
				"minute(s)", szMinute, sizeof(szMinute));
		}

		if (nHours)
		{
			nLen += sprintf(szInfo + nLen, "%d%s", nHours, szHour);
		}
		if (nMinutes || nHours == 0)
		{
			nLen += sprintf(szInfo + nLen, "%d%s", nMinutes, szMinute);
		}
		szInfo[nLen] = ' ';
		nLen ++;
		szInfo[nLen] = '\0';
	}

	m_LifeTimeText.SetText(szInfo, nLen);
	if (pSetting)
		g_UiBase.CloseCommConfigFile();
}

void KUiSelPlayer::GetRoleImageName(char* pszName, const char* pszPrefix, unsigned char bGender, unsigned char bAttribute, int nIndex)
{
	_ASSERT(pszName && bAttribute >= series_metal && bAttribute < series_num);
	static const char* pszAttribute[series_num] = { "金", "木", "水", "火", "土" };
	static const char* pszGender[2] = { "男", "女" };
	sprintf(pszName, "%s_%s_%s_%d.spr", pszPrefix, pszAttribute[bAttribute],
		(bGender ? pszGender[1] : pszGender[0]), nIndex);
}

// -------------------------------------------------------------------------
// 函数	: 消息处理
// -------------------------------------------------------------------------
int KUiSelPlayer::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam, false);
        break;

	case WND_N_BUTTON_DB_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam, true);
        break;
	case WM_KEYDOWN:
		nRet = OnKeyDown(uParam);
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

int KUiSelPlayer::OnKeyDown(unsigned int uKey)
{
	int			nRet = 1;
	KWndWindow* pActive = NULL;
	KWndButton* pToActive = NULL;
	switch (uKey)
	{
	case VK_RETURN:
		pActive = GetActiveBtn();
		if (pActive == NULL && m_btnOk.IsDisable() == FALSE)
			pActive = &m_btnOk;
		OnClickButton((KWndButton*)pActive, false);
		break;
	case VK_ESCAPE:
		OnCancel();
		nRet = 1;
		break;
	case VK_UP:
		pActive = GetActiveBtn();
		if (pActive == &m_btnCancel)
			pToActive = &m_btnDel;
		else if (pActive == &m_btnDel)
			pToActive = &m_btnNew;
		else if (pActive == &m_btnOk)
			pToActive = &m_btnCancel;
		else
			pToActive = &m_btnOk;
		nRet = 1;
		break;
	case VK_DOWN:
		pActive = GetActiveBtn();
		if (pActive == &m_btnOk)
			pToActive = &m_btnNew;
		else if (pActive == &m_btnNew)
			pToActive = &m_btnDel;
		else if (pActive == &m_btnCancel)
			pToActive = &m_btnOk;
		else
			pToActive = &m_btnCancel;
		nRet = 1;
		break;
	case VK_LEFT:
		if (m_nSelPlayer > 0)
			OnSelectPlayer(m_nSelPlayer - 1);
		else if (m_nSelPlayer < 0 && m_nNumPlayer > 0)
			OnSelectPlayer(0);
		m_btnDel.Enable(m_nSelPlayer >= 0);
		nRet = 1;
		break;
	case VK_RIGHT:
	case VK_SPACE:
		if (m_nSelPlayer < m_nNumPlayer - 1)
			OnSelectPlayer(m_nSelPlayer + 1);
		else if (m_nNumPlayer < 0 && m_nNumPlayer > 0)
			OnSelectPlayer(0);
		m_btnDel.Enable(m_nSelPlayer >= 0);
		nRet = 1;
		break;
	default:
		nRet = 0;
	}
	if (pToActive)
		pToActive->SetCursorAbove();
	return nRet;
}

void KUiSelPlayer::OnSelectPlayer(int nSel, bool bPlaySound)
{
	char	szFileName[128];
	if (m_nSelPlayer != nSel)
	{
		if (m_nSelPlayer >= 0)
		{
			GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_Gender[m_nSelPlayer], m_Attribute[m_nSelPlayer], 2);
			m_player[m_nSelPlayer].SetImage(ISI_T_SPR, szFileName);
			m_player[m_nSelPlayer].SetFrame(0);
		}
		m_btnDel.Enable(true);
		m_nSelPlayer = nSel;
	}
	GetRoleImageName(szFileName, m_szPlayerImgPrefix, m_Gender[m_nSelPlayer], m_Attribute[m_nSelPlayer], 1);
	m_player[m_nSelPlayer].SetImage(ISI_T_SPR, szFileName);
	m_player[m_nSelPlayer].SetFrame(0);
	m_bJustClicked = true;
	if (bPlaySound)
		UiSoundPlay(m_szSelPlayerSound[m_nSelPlayer]);
}

// -------------------------------------------------------------------------
// 函数		: KUiSelPlayer::OnClickButton
// 功能		: 响应点击按钮
// 参数		: KWndWindow* pWnd ---> 被点击的控件指针
// -------------------------------------------------------------------------
void KUiSelPlayer::OnClickButton(KWndButton* pWnd, bool bDoubleClick)
{
	// ---> 判断点中的是那个控件
	for (int i = 0; i < m_nNumPlayer; i ++)
	{
		if (pWnd == (KWndWindow*)&m_player[i])	// 点中了某个玩家信息框
		{
			OnSelectPlayer(i, !bDoubleClick);
            // 如果是双击，就仿真选择用户
            if (bDoubleClick)
            {
        		OnEnterGame();
            }
			return;
		}
	}

	if (pWnd == (KWndWindow*)&m_btnNew)
		OnNew();
	else if (pWnd == (KWndWindow*)&m_btnCancel)
		OnCancel();
	else if (pWnd == (KWndWindow*)&m_btnOk)
		OnEnterGame();
	else if (pWnd == (KWndWindow*)&m_btnDel)
		OnDel();
}

void KUiSelPlayer::OnEnterGame()
{
	if (m_nSelPlayer >= 0)
	{
		if (g_LoginLogic.SelectRole(m_nSelPlayer))
		{
			KUiConnectInfo::OpenWindow(CI_MI_ENTERING_GAME, LL_S_IN_GAME);
			Hide();
		}
	}
}

// -------------------------------------------------------------------------
// 函数		: KUiSelPlayer::OnNew
// 功能		: 新建按钮点击事件处理
// -------------------------------------------------------------------------
void KUiSelPlayer::OnNew()
{
	int nSel;
	if (g_LoginLogic.GetRoleCount(nSel) < MAX_PLAYER_PER_ACCOUNT)
	{
		Hide();
		KUiSelNativePlace::OpenWindow();	// 打开新建角色对话框
	}
}

void KUiSelPlayer::OnDel()
{
	if (m_nSelPlayer >= 0)
	{
		char szBuff[32];
		m_Name[m_nSelPlayer].GetText(szBuff, sizeof(szBuff));
		KUiConnectInfo::OpenWindow(CI_MI_TO_DEL_ROLE, CI_NS_SEL_ROLE_WND, m_nSelPlayer, szBuff);
		Hide();
	}
}

// -------------------------------------------------------------------------
// 函数		: KUiSelPlayer::OnCancel
// 功能		: 取消按钮点击事件处理
// -------------------------------------------------------------------------
void KUiSelPlayer::OnCancel()
{
	Hide();
	g_LoginLogic.ReturnToIdle();
	KUiSelServer::OpenWindow();
}

KWndButton*	KUiSelPlayer::GetActiveBtn()
{
	KWndButton* pBtn = NULL;
	if (m_btnOk.IsButtonActive())
		pBtn = &m_btnOk;
	else if (m_btnCancel.IsButtonActive())
		pBtn = &m_btnCancel;
	else if (m_btnDel.IsButtonActive())
		pBtn = &m_btnDel;
	else if (m_btnNew.IsButtonActive())
		pBtn = &m_btnNew;

	return pBtn;
}
