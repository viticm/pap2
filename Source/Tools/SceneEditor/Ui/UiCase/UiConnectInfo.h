/*****************************************************************************************
//	界面--login过程信息窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-16
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "../elem/wndpuretextbtn.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndEdit.h"
#include "../../Login/Login.h"

enum LOGIN_BG_INFO_MSG_INDEX
{
	CI_MI_CONNECTING = 1,		//1=连接中
	CI_MI_CONNECT_FAILED,		//2=连接失败
	CI_MI_CONNECT_SERV_BUSY,	//3=服务器忙
	CI_MI_CONNECT_TIMEOUT,		//4=连接超时未获得响应
	CI_MI_ACCOUNT_PWD_ERROR,	//5=账号/密码错误
	CI_MI_ACCOUNT_LOCKED,		//6=账号被锁定
	CI_MI_ERROR_ROLE_NAME,		//7=新建角色的名字不合法
	CI_MI_CREATING_ROLE,		//8=正在新建角色
	CI_MI_DELETING_ROLE,		//9=正在删除角色
	CI_MI_GETTING_ROLE_DATA,	//10=取角色数据中
	CI_MI_ENTERING_GAME,		//11=进入游戏中
	CI_MI_SVRDOWN,				//12=游戏服务器已满或正在维护中

	CI_MI_TO_DEL_ROLE,			//13=要删除角色
    CI_MI_INVALID_PROTOCOLVERSION,  //14=版本号较旧，需要升级到新的客户端

	CI_MI_ERROR_LOGIN_INPUT,	//15=登陆界面输入的账号密码有误，请重新输入
	CI_MI_ERROR_CONFIRM_INPUT,	//16=删除角色时，输入的密码有误，请重新输入
	CI_MI_INVALID_LOGIN_INPUT1,	//17=姓名中不可以包含空格、制表格等字符！
	CI_MI_INVALID_LOGIN_INPUT2,	//18=姓名长度必须在2到8个汉字以内，允许输入英文字符，请重新输入姓名！
	CI_MI_NOT_ENOUGH_ACCOUNT_POINT,	//19=点卡不足
	CI_MI_INVALID_PASSWORD,		//20=输入的密码不正确。
	CI_MI_ACCOUNT_FREEZE = 30,	//30=账号冻结
	CI_MI_DEL_ROLE_NOT_ALLOW = 31,	//31=此角色不可被删除
	CI_MI_ACCOUNT_SMS_LOCK = 34,	//32=账号被短信锁定
	CI_MI_ACCOUNT_NOT_ACTIVE = 35,	//33=账号未激活

	CI_MI_TO_TRANSFER_ROLE = 36,	//36=%转服条件的明显提醒%
};

enum LOGIN_BG_NEXT_STEP
{
	CI_NS_INIT_WND	= 1000,		//转到开始界面
	CI_NS_HEALTH_GAME,			//健康游戏忠告
	CI_NS_SEL_SERVER,			//转到选服务器界面
	CI_NS_LOGIN_WND,			//转到登陆界面
	CI_NS_SEL_ROLE_WND,			//转到新建角色界面
	CI_NS_NEW_ROLE_WND,			//转到新建角色界面
	CI_NS_CONFIRM_DEL_ROLE,		//转到确认删除角色界面
	CI_NS_EXIT_PROGRAM,			//退出程序
};

class KUiConnectInfo : protected KWndImage
{
public:
	//eDesireLoginStatus为没举LOGIN_LOGIC_STATUS或LOGIN_BG_NEXT_STEP的值之一
	static KUiConnectInfo*	OpenWindow(LOGIN_BG_INFO_MSG_INDEX eIndex,
									int eDesireLoginStatus,
									int nParam = 0, char *szString = NULL);	//打开窗口，返回唯一的一个类对象实例
	static void				CloseWindow(bool bDestroy);				//关闭窗口

private:
	KUiConnectInfo();
	~KUiConnectInfo() {}
	void	Initialize();							//初始化
	void	LoadScheme(const char* pScheme);		//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	void	PaintWindow();
	void	SetInfoMsg(LOGIN_BG_INFO_MSG_INDEX eIndex);
	void	Breathe();								//活动函数
	bool	OnDelRole();
	void	OnCancelDelRole();
	void	OnClickConfirmBtn();
	void	GotoNextStep();
private:
	static 	KUiConnectInfo* m_pSelf;
private:
	KWndImage		m_DelRoleBgImg;
	KWndButton		m_ConfirmBtn;
	KWndButton		m_DelRoleBtn;
	KWndButton		m_ContinueBtn;
	KWndButton		m_CancelDelRoleBtn;
	KWndEdit32		m_InputPwdWnd;
//	KWndPureTextBtn m_BtnUnlockLink;
	KWndText32		m_RoleName;

	int				m_nDesireLoginStatus;

	char			m_szMessage[64];
	char            m_szUnlockLink[256];
	int				m_nMsgLen;
	int				m_nFont;
	int				m_nColor2CharacterStartIndex;
	int				m_nColor2CharacterEndIndex;
	int				m_nTextCentreX, m_nTextCentreY;

	unsigned int	m_uMsgColor;
	unsigned int	m_uMsgBorderColor;
	unsigned int	m_uMsgColor2;
	unsigned int	m_uMsgBorderColor2;
	unsigned int	m_uMsgColorChangeInterval;
	unsigned int	m_uLastMsgColorChanageTime;

	char			m_szLoginBg[32];
	int				m_nParam;

	char			m_szString[32];
};
