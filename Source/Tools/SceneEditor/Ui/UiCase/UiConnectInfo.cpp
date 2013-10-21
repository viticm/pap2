/*****************************************************************************************
//	界面--login过程背景窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-16
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiConnectInfo.h"
#include "UiLoginBg.h"
#include "UiInit.h"
#include "UiLogin.h"
#include "UiSelPlayer.h"
#include "UiMsgCentrePad.h"
#include "UiNewPlayer.h"
#include "UiInformation.h"
#include "UiSelServer.h"
#include "UiHealthGame.h"
#include "Text.h"
#include "../UiShell.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../ENGINE/Src/KSG_MD5_String.h"
extern iRepresentShell*	g_pRepresentShell;

#include "KWin32Wnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI				"登陆过程提示.ini"
#define INFO_LENGTH				40

KUiConnectInfo* KUiConnectInfo::m_pSelf = NULL;

KUiConnectInfo::KUiConnectInfo()
{
	m_nDesireLoginStatus = LL_S_IDLE;

	m_szMessage[0] = 0;
	m_nMsgLen = 0;
	m_szLoginBg[0] = 0;
	m_nColor2CharacterStartIndex = 0;
	m_nColor2CharacterEndIndex = 0;
	m_nFont = 12;
	m_nParam = 0;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiConnectInfo* KUiConnectInfo::OpenWindow(LOGIN_BG_INFO_MSG_INDEX eIndex,
										int eDesireLoginStatus,
										int nParam /*= 0*/, char *szString /*= NULL*/)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiConnectInfo;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		_ASSERT((m_pSelf->m_nDesireLoginStatus >= (int)LL_S_IDLE  &&
					m_pSelf->m_nDesireLoginStatus <= (int)LL_S_IN_GAME) ||
				(m_pSelf->m_nDesireLoginStatus >= (int)CI_NS_INIT_WND &&
					m_pSelf->m_nDesireLoginStatus <= (int)CI_NS_EXIT_PROGRAM));
		m_pSelf->m_nDesireLoginStatus = eDesireLoginStatus;
		m_pSelf->m_nParam = nParam;
		if(szString)
		{
			memcpy(m_pSelf->m_szString, szString, sizeof(m_pSelf->m_szString));
		}
		m_pSelf->SetInfoMsg(eIndex);
		KUiLoginBackGround::SetConfig(m_pSelf->m_szLoginBg);
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiConnectInfo::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
		{
			m_pSelf->Hide();
		}
		else
		{
			Wnd_ReleaseExclusive(m_pSelf);
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiConnectInfo::Initialize()
{
	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 128);
	m_pSelf->LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_NORMAL);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiConnectInfo::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "RuningImgBg");
		m_ConfirmBtn.Init(&Ini, "ConfirmBtn");
		m_InputPwdWnd.Init(&Ini, "Password");
		m_DelRoleBtn.Init(&Ini, "DelRole");
		m_DelRoleBgImg.Init(&Ini, "DelRoleBgImg");
		m_CancelDelRoleBtn.Init(&Ini, "CancelDelRole");
//		m_BtnUnlockLink.Init(&Ini, "BtnUnlock");
		m_RoleName.Init(&Ini, "RoleName");
		m_ContinueBtn.Init(&Ini, "ContinueBtn");
		Ini.GetString("BtnUnlock", "URL", "www.jxonline.net", m_szUnlockLink, sizeof(m_szUnlockLink));

		Ini.GetString("Message", "MsgColor", "0,0,0", Buff, sizeof(Buff));
		m_uMsgColor = GetColor(Buff);
		Ini.GetString("Message", "MsgBorderColor", "0,0,0", Buff, sizeof(Buff));
		m_uMsgBorderColor = GetColor(Buff);
		Ini.GetString("Message", "MsgColor2", "0,0,0", Buff, sizeof(Buff));
		m_uMsgColor2=  GetColor(Buff);
		Ini.GetString("Message", "MsgBorderColor2", "0,0,0", Buff, sizeof(Buff));
		m_uMsgBorderColor2=  GetColor(Buff);
		Ini.GetInteger("Message", "ColorChangeInterval", 0, (int*)&m_uMsgColorChangeInterval);
		Ini.GetInteger("Message", "Font", 12, &m_nFont);

		int	x, y, w, h;
		Ini.GetInteger2("Message", "Pos", &x, &y);
		Ini.GetInteger2("Message", "Size", &w, &h);
		m_nTextCentreX = x + w / 2;
		m_nTextCentreY = y;

		Ini.GetString("RuningImgBg", "LoginBg", "", m_szLoginBg, sizeof(m_szLoginBg));
	}
}

void KUiConnectInfo::OnClickConfirmBtn()
{
	switch(m_nDesireLoginStatus)
	{
	case CI_NS_INIT_WND:
		g_LoginLogic.ReturnToIdle();
		Hide();
		KUiInit::OpenWindow(false, false);
		break;
	case CI_NS_HEALTH_GAME:
		g_LoginLogic.ReturnToIdle();
		Hide();
		KUiHealthGame::OpenWindow();
		break;
	case CI_NS_SEL_SERVER:
		g_LoginLogic.ReturnToIdle();
		Hide();
		KUiSelServer::OpenWindow();
		break;
	case CI_NS_LOGIN_WND:
		Hide();
		KUiLogin::OpenWindow();
		break;
	case CI_NS_SEL_ROLE_WND:
		Hide();
		KUiSelPlayer::OpenWindow();
		break;
	case CI_NS_NEW_ROLE_WND:
		Hide();
		KUiNewPlayer::OpenWindow(m_nParam);
		break;
	case CI_NS_CONFIRM_DEL_ROLE:
		SetInfoMsg(CI_MI_TO_DEL_ROLE);
		break;
	case CI_NS_EXIT_PROGRAM:
		Hide();
        UiPostQuitMsg();
		break;
	default:
		//取消正在进行的操作
		Hide();
		g_LoginLogic.ReturnToIdle();
		KUiSelServer::OpenWindow();
		break;
	}
}

int KUiConnectInfo::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_ConfirmBtn ||
			uParam == (unsigned int)(KWndWindow*)&m_ContinueBtn)
			OnClickConfirmBtn();
		else if (uParam == (unsigned int)(KWndWindow*)&m_DelRoleBtn)
			OnDelRole();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelDelRoleBtn)
			OnCancelDelRole();
//		else if(uParam == (unsigned int)(KWndWindow*)&m_BtnUnlockLink)
//		{
//			ShellExecute(g_GetMainHWnd(), NULL, m_szUnlockLink, NULL, NULL, SW_SHOWNORMAL);
//			ShowWindow(g_GetMainHWnd(), SW_MINIMIZE);
//		}
		break;
	case WM_KEYDOWN:
		if (m_ConfirmBtn.GetParent() && (uParam == VK_RETURN || uParam == VK_ESCAPE))
			OnClickConfirmBtn();
		else if (m_DelRoleBtn.GetParent() && uParam == VK_RETURN)
			OnDelRole();
		else if (m_CancelDelRoleBtn.GetParent() && uParam == VK_ESCAPE)
			OnCancelDelRole();
		return 1;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return 0;
}


bool KUiConnectInfo::OnDelRole()
{
	KSG_PASSWORD SupperPassword;
    char	szSupperPassword[KSG_PASSWORD_MAX_SIZE];

	if (m_InputPwdWnd.GetText(szSupperPassword, sizeof(szSupperPassword), false))
	{

        #ifdef SWORDONLINE_USE_MD5_PASSWORD

        KSG_StringToMD5String(SupperPassword.szPassword, szSupperPassword);

        #else

        #pragma message (KSG_ATTENTION("Add Password to MD5 string"))
        strncpy(SupperPassword.szPassword, szSupperPassword, sizeof(SupperPassword.szPassword));
        SupperPassword.szPassword[sizeof(SupperPassword.szPassword) - 1] = '\0';

        #endif


		m_InputPwdWnd.ClearText(true);

        if (g_LoginLogic.DeleteRole(m_nParam, SupperPassword))
		{
			SetInfoMsg(CI_MI_DELETING_ROLE);
			m_nDesireLoginStatus = LL_S_ROLE_LIST_READY;
		}
		else
		{
			SetInfoMsg(CI_MI_CONNECT_FAILED);
			m_nDesireLoginStatus = CI_NS_INIT_WND;
		}
		memset(&szSupperPassword, 0, sizeof(szSupperPassword));
		memset(&SupperPassword, 0, sizeof(SupperPassword));
	}
	else
	{
		SetInfoMsg(CI_MI_ERROR_CONFIRM_INPUT);
		m_nDesireLoginStatus = CI_NS_CONFIRM_DEL_ROLE;
	}
	return false;
}

void KUiConnectInfo::OnCancelDelRole()
{
	m_InputPwdWnd.ClearText(true);
	Hide();
	KUiSelPlayer::OpenWindow();
}

void KUiConnectInfo::SetInfoMsg(LOGIN_BG_INFO_MSG_INDEX eIndex)
{
	m_DelRoleBgImg.LeaveAlone();
	m_DelRoleBtn.LeaveAlone();
	m_CancelDelRoleBtn.LeaveAlone();
	m_ConfirmBtn.LeaveAlone();
	m_ContinueBtn.LeaveAlone();
	m_InputPwdWnd.LeaveAlone();
//	m_BtnUnlockLink.LeaveAlone();
	m_RoleName.LeaveAlone();

	m_szMessage[0] = 0;
	m_nMsgLen = 0;
	m_nColor2CharacterStartIndex = 0;
	m_nColor2CharacterEndIndex = 0;

	KIniFile*	pIni = g_UiBase.GetCommConfigFile();
	if (pIni)
	{
		char	szKey[8];
		char	szBuff[64];
		itoa(eIndex, szKey, 10);
		pIni->GetString("InfoString", szKey, "", szBuff, sizeof(szBuff));
		m_nMsgLen = sprintf(m_szMessage, szBuff, m_szString);
		if (m_nMsgLen)
		{
			m_uLastMsgColorChanageTime = IR_GetCurrentTime();
			m_nColor2CharacterEndIndex =
				TSplitString(m_szMessage, m_nColor2CharacterStartIndex + 1, false);
		}
	}

	if (eIndex == CI_MI_TO_DEL_ROLE)	//13=要删除角色
	{
		AddChild(&m_DelRoleBgImg);
		AddChild(&m_InputPwdWnd);
		AddChild(&m_DelRoleBtn);
		AddChild(&m_RoleName);
		char szBuff[64];
		sprintf(szBuff, "角色名字：%s", m_szString);
		m_RoleName.SetText(szBuff);
		AddChild(&m_CancelDelRoleBtn);
		Wnd_SetFocusWnd(&m_InputPwdWnd);
	}
	else if (eIndex == CI_MI_TO_TRANSFER_ROLE)
	{
		AddChild(&m_ContinueBtn);
	}
	else if (eIndex != CI_MI_ENTERING_GAME)
	{
//		if(eIndex == CI_MI_ACCOUNT_LOCKED)
//		{
//			AddChild(&m_BtnUnlockLink);
//		}
		AddChild(&m_ConfirmBtn);
	}
}

void KUiConnectInfo::GotoNextStep()
{
	LOGIN_LOGIC_STATUS eStatus = g_LoginLogic.GetStatus();
	switch(eStatus)
	{
	case LL_S_IDLE:	//空闲
		Hide();
		KUiSelServer::OpenWindow();
		break;
	case LL_S_WAIT_INPUT_ACCOUNT:	//等待传账号密码
		Hide();
		KUiLogin::OpenWindow();
		break;
	case LL_S_ACCOUNT_CONFIRMING:	//等待账号密码验证
		SetInfoMsg(CI_MI_CONNECTING);
		break;
	case LL_S_WAIT_ROLE_LIST:		//等待接收角色列表数据
		SetInfoMsg(CI_MI_GETTING_ROLE_DATA);
		break;
	case LL_S_ROLE_LIST_READY:		//角色列表就绪
		Hide();
		KUiSelPlayer::OpenWindow();
		break;
	case LL_S_CREATING_ROLE:		//正在新建角色
		SetInfoMsg(CI_MI_CREATING_ROLE);
		break;
	case LL_S_DELETING_ROLE:		//正在删除角色
		SetInfoMsg(CI_MI_DELETING_ROLE);
		break;
	case LL_S_WAIT_TO_LOGIN_GAMESERVER:	//等待登陆游戏服务器
		SetInfoMsg(CI_MI_CONNECTING);
		break;
	case LL_S_ENTERING_GAME:		//正在进入游戏
		SetInfoMsg(CI_MI_ENTERING_GAME);
		break;
	case LL_S_IN_GAME:				//游戏运行时
		UiStartGame();
		break;
	}
}

//活动函数
void KUiConnectInfo::Breathe()
{
	if (m_nDesireLoginStatus >= CI_NS_INIT_WND)
		return;

	LOGIN_LOGIC_STATUS eStatus = g_LoginLogic.GetStatus();
	LOGIN_LOGIC_RESULT_INFO eResult = g_LoginLogic.GetResult();
	if (eResult == LL_R_NOTHING)
	{
		if (eStatus == m_nDesireLoginStatus)
			GotoNextStep();
		return;
	}

	switch(eResult)
	{
	case LL_R_CONNECT_FAILED:
		SetInfoMsg(CI_MI_CONNECT_FAILED);
		m_nDesireLoginStatus = CI_NS_SEL_SERVER;
		break;
	case LL_R_CONNECT_SERV_BUSY:
		SetInfoMsg(CI_MI_CONNECT_SERV_BUSY);
		m_nDesireLoginStatus = CI_NS_SEL_SERVER;
		break;
	case LL_R_CONNECT_TIMEOUT:
		SetInfoMsg(CI_MI_CONNECT_TIMEOUT);
		m_nDesireLoginStatus = CI_NS_SEL_SERVER;
		break;
	case LL_R_ACCOUNT_PWD_ERROR:
		SetInfoMsg(CI_MI_ACCOUNT_PWD_ERROR);
		m_nDesireLoginStatus = CI_NS_LOGIN_WND;
		break;
	case LL_R_ACCOUNT_LOCKED:
		SetInfoMsg(CI_MI_ACCOUNT_LOCKED);
		m_nDesireLoginStatus = CI_NS_LOGIN_WND;
		break;
	case LL_R_ACCOUNT_FREEZE:
		SetInfoMsg(CI_MI_ACCOUNT_FREEZE);
		m_nDesireLoginStatus = CI_NS_LOGIN_WND;
		break;
	case LL_R_INVALID_ROLENAME:		//(新建)角色的名字不合法
		SetInfoMsg(CI_MI_ERROR_ROLE_NAME);
		m_nDesireLoginStatus = CI_NS_NEW_ROLE_WND;
		break;
	case LL_R_SERVER_SHUTDOWN:		//游戏服务器已满或正在维护中
		SetInfoMsg(CI_MI_SVRDOWN);
		m_nDesireLoginStatus = CI_NS_SEL_SERVER;
		break;
	case LL_R_INVALID_PASSWORD:
		SetInfoMsg(CI_MI_INVALID_PASSWORD);
		m_nDesireLoginStatus = CI_NS_CONFIRM_DEL_ROLE;
		break;
	case LL_R_DEL_ROLE_NOT_ALLOW:
		SetInfoMsg(CI_MI_DEL_ROLE_NOT_ALLOW);
		m_nDesireLoginStatus = CI_NS_SEL_ROLE_WND;
		break;
	case LL_R_INVALID_PROTOCOLVERSION:
		SetInfoMsg(CI_MI_INVALID_PROTOCOLVERSION);
		m_nDesireLoginStatus = CI_NS_INIT_WND;
		break;
	case LL_R_ACCOUNT_CONFIRM_SUCCESS:
		SetInfoMsg(CI_MI_GETTING_ROLE_DATA);
		break;
	case LL_R_CREATE_ROLE_SUCCESS:
		SetInfoMsg(CI_MI_ENTERING_GAME);
		break;
	case LL_R_ACCOUNT_NOT_ENOUGH_POINT:
		SetInfoMsg(CI_MI_NOT_ENOUGH_ACCOUNT_POINT);
		m_nDesireLoginStatus = CI_NS_LOGIN_WND;
		break;
	case LL_R_ACCOUNT_SMS_LOCK:
		SetInfoMsg(CI_MI_ACCOUNT_SMS_LOCK);
		m_nDesireLoginStatus = CI_NS_LOGIN_WND;
		break;
	case LL_R_ACCOUNT_NOT_ACTIVE:
		SetInfoMsg(CI_MI_ACCOUNT_NOT_ACTIVE);
		m_nDesireLoginStatus = CI_NS_LOGIN_WND;
		break;
	case LL_R_LOGIN_TO_GAMESERVER:
		KUiMsgCentrePad::OpenWindow();
		KUiMsgCentrePad::CloseWindow(false);
		break;
	}
}

void KUiConnectInfo::PaintWindow()
{
	KWndImage::PaintWindow();
	if (m_nMsgLen && g_pRepresentShell)
	{
		if (IR_IsTimePassed(m_uMsgColorChangeInterval, m_uLastMsgColorChanageTime))
		{
			if (m_nColor2CharacterEndIndex != m_nMsgLen)
				m_nColor2CharacterStartIndex = m_nColor2CharacterEndIndex;
			else
				m_nColor2CharacterStartIndex = 0;
			m_nColor2CharacterEndIndex =
					TSplitString(m_szMessage, m_nColor2CharacterStartIndex + 1, false);
		}

		//判断是否超出一行
		int nMsgLen;
		if(m_nMsgLen > INFO_LENGTH)
		{
			nMsgLen = TSplitString(m_szMessage, INFO_LENGTH, FALSE);
		}
		else
		{
			nMsgLen = m_nMsgLen;
		}

		int nX = m_nAbsoluteLeft + m_nTextCentreX - nMsgLen * m_nFont / 4;
		int nY = m_nAbsoluteTop + m_nTextCentreY - ((m_nMsgLen > nMsgLen) ? m_nFont / 2: 0);
		if (m_nColor2CharacterStartIndex)
		{
			//如果要变色的字在第一行，就不需要很特别的处理
			if(m_nColor2CharacterStartIndex < nMsgLen)
			{
				g_pRepresentShell->OutputText(m_nFont, m_szMessage,
						m_nColor2CharacterStartIndex, nX, nY, m_uMsgColor,
						0, TEXT_IN_SINGLE_PLANE_COORD, m_uMsgBorderColor);
				nX += m_nColor2CharacterStartIndex * m_nFont / 2;
			}
			//要变色的字在下一行，那就
			else
			{
				//先直接画出第一行
				g_pRepresentShell->OutputText(m_nFont, m_szMessage,
						nMsgLen, nX, nY, m_uMsgColor,
						0, TEXT_IN_SINGLE_PLANE_COORD, m_uMsgBorderColor);

				//坐标换到下一行去
				nX = m_nAbsoluteLeft + m_nTextCentreX - (m_nMsgLen - nMsgLen) * m_nFont / 4;
				nY += m_nFont;
				//再画到要变色的那个字的前面
				g_pRepresentShell->OutputText(m_nFont, &m_szMessage[nMsgLen],
						m_nColor2CharacterStartIndex - nMsgLen, nX, nY, m_uMsgColor,
						0, TEXT_IN_SINGLE_PLANE_COORD, m_uMsgBorderColor);
				nX += (m_nColor2CharacterStartIndex - nMsgLen) * m_nFont / 2;
			}
		}
		//画出要变色的那个字
		g_pRepresentShell->OutputText(m_nFont,
			&m_szMessage[m_nColor2CharacterStartIndex],
			m_nColor2CharacterEndIndex - m_nColor2CharacterStartIndex,
			nX, nY, m_uMsgColor2,
			0, TEXT_IN_SINGLE_PLANE_COORD, m_uMsgBorderColor2);

		//如果刚好到行尾了，就要换行
		if(m_nColor2CharacterEndIndex == nMsgLen)
		{
			nX = m_nAbsoluteLeft + m_nTextCentreX - (m_nMsgLen - nMsgLen) * m_nFont / 4;
			nY += m_nFont;
		}
		//不然的话就不用换行了
		else
		{
			nX += (m_nColor2CharacterEndIndex - m_nColor2CharacterStartIndex) * m_nFont / 2;
		}

		//如果变色的那个字不是整句话的末尾
		if (m_nColor2CharacterEndIndex < m_nMsgLen)
		{
			//如果是两行的话，要进行特别判断，如果还没到第二行，就要分两行画
			//剩下的那些字
			if(nMsgLen < m_nMsgLen)
			{
				int nPos;
				if(m_nColor2CharacterEndIndex < nMsgLen)
				{
					g_pRepresentShell->OutputText(m_nFont,
							&m_szMessage[m_nColor2CharacterEndIndex],
							nMsgLen - m_nColor2CharacterEndIndex, nX, nY, m_uMsgColor,
							0, TEXT_IN_SINGLE_PLANE_COORD, m_uMsgBorderColor);
					nX = m_nAbsoluteLeft + m_nTextCentreX - (m_nMsgLen - nMsgLen) * m_nFont / 4;
					nY += m_nFont;
					nPos = nMsgLen;
				}
				else
				{
					nPos = m_nColor2CharacterEndIndex;
				}
				g_pRepresentShell->OutputText(m_nFont,
						&m_szMessage[nPos],
						m_nMsgLen - nPos, nX, nY, m_uMsgColor,
						0, TEXT_IN_SINGLE_PLANE_COORD, m_uMsgBorderColor);
			}
			//不是的话，就直接一行画出后面的就行了
			else
			{
				g_pRepresentShell->OutputText(m_nFont,
						&m_szMessage[m_nColor2CharacterEndIndex],
						nMsgLen - m_nColor2CharacterEndIndex, nX, nY, m_uMsgColor,
						0, TEXT_IN_SINGLE_PLANE_COORD, m_uMsgBorderColor);
			}
		}
	}
}