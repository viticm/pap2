/*****************************************************************************************
//	剑侠界面系统基础功能模块
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-17
*****************************************************************************************/
//#include "KWin32.h"
#include "../public/KWin32Wnd.h"
#include "../public/KIniFile.h"
#include "../public/KFile.h"
#include "UiBase.h"
#include "KFilePath.h"
#include "KIme.h"
#include "../../core/src/coreshell.h"
#include "../../core/src/GameDataDef.h"
#include "Elem/MouseHover.h"
#include "Elem/WndShowAnimate.h"
#include "Elem/WndObjContainer.h"

#include "ShortcutKey.h"
#include "Elem/Wnds.h"
#include "Elem/PopupMenu.h"
#include "../../Represent/iRepresent/iRepresentShell.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*      g_pCoreShell;


#define	UI_INFO_FILE_PATH			"\\Ui"				//界面信息文件存放的根目录
#define	UI_COMMON_CONFIG_FILE		"\\Ui\\Setting.ini"
#define	UI_DEF_CONFIG_FILE			"\\Ui\\DefConfig.Ini"//界面配置纪录文件名
#define	UI_PUBLIC_SETTING			"公共.ini"			//某个界面方案的设定文件

#define	UI_USER_DATA_FOLDER			"\\UserData"		//玩家数据的存盘目录位置
#define	UI_USER_DATA_TEMP_FOLDER	"\\UserData\\Temp"	//玩家数据的临时存盘目录位置
#define	UI_COMMON_SETTING_FILE		"\\UserData\\UiCommon.ini"//界面公共设置的文件的名称
#define UI_PRIVATE_SETTING_FILE		"UiConfig.ini"		//界面个人数据的存储文件

#define	THEME_SECTION			"Theme"
#define	FONT_SECTION			"FontList"

const char*	$Main = "Main";

KUiBase	g_UiBase;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiBase::KUiBase()
{
	m_CurScheme[0] = 0;
	m_CurSchemePath[0] = 0;
	m_UserAccountId[0] = 0;
	m_nStatusHandleCount = 1;
	m_pUiCommSettingFile = NULL;	//界面公共设置的文件
	m_pAccountSettingFile = NULL;	//账号设置文件
	m_pUiPrivateSettingFile = NULL;	//当前账号的设置文件
	memset(m_aryStatus, 0, sizeof(m_aryStatus));
}

//--------------------------------------------------------------------------
//	功能：析构函数
//--------------------------------------------------------------------------
KUiBase::~KUiBase()
{
}

//--------------------------------------------------------------------------
//	功能：退出函数
//--------------------------------------------------------------------------
void KUiBase::Exit()
{
	m_UserAccountId[0] = 0;
	CloseCommSettingFile(false);
	ClosePrivateSettingFile(false);
	CloseCommConfigFile();
}

#ifdef _DEBUG
	extern int WND_SHOW_MOUSE_OVER_WND;
#endif

//获得玩家数据的临时存盘目录位置
const char*	KUiBase::GetUserTempDataFolder()
{
	return UI_USER_DATA_TEMP_FOLDER;
}

//清除玩家数据的临时存盘目录内的全部文件
void KUiBase::CleanTempDataFolder()
{
	ClearFolder(UI_USER_DATA_TEMP_FOLDER, false);
}

//清除当前玩家私有数据的存储目录
void KUiBase::ClearFolder(const char* pszFolder, bool bDeleteFolder)
{
	char szPathFile[MAX_PATH];
	WIN32_FIND_DATA FileData;
	HANDLE hFind;

	if (pszFolder && pszFolder[0])
	{
		g_GetFullPath(szPathFile, (char*)pszFolder);
		int		nLen = strlen(szPathFile) + 1;
		strcat(szPathFile, "\\*.*");
		hFind = FindFirstFile(szPathFile, &FileData);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				strcpy(szPathFile + nLen, FileData.cFileName);
				SetFileAttributes(szPathFile, FILE_ATTRIBUTE_NORMAL);
				DeleteFile(szPathFile);
			}while(FindNextFile(hFind, &FileData));
			FindClose(hFind);
		}
		if (bDeleteFolder)
		{
			szPathFile[nLen - 1] = 0;
			RemoveDirectory(szPathFile);
		}
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KUiBase::Init()
{
	if (g_pIme)
	{
		g_pIme->DisableLanguageChange();
		g_pIme->CloseIME();
		g_pIme->TurnOn();
	}

	KIniFile* pSetting = GetCommSettingFile();
	char	SchemeName[32] = "";
	int		nInterval = 20;
	if (pSetting)
	{
		pSetting->GetString("Main", "LastUsedScheme", "", SchemeName, 32);
		pSetting->GetInteger("Main", "WndMoveInterval", 20, &nInterval);
		#ifdef _DEBUG
			int	nValue;
			pSetting->GetInteger("Main", "ShowDebugFrameText", 0, &nValue);
			WND_SHOW_DEBUG_FRAME_TEXT = nValue;
			pSetting->GetInteger("Main", "ShowMouseOverWnd", 0, &nValue);
			WND_SHOW_MOUSE_OVER_WND = nValue;
		#endif
			CloseCommSettingFile(false);
	}
	g_CreatePath(UI_USER_DATA_FOLDER);
	g_CreatePath(UI_USER_DATA_TEMP_FOLDER);
	KWndShowAnimate::SetInterval(nInterval);
	LoadScheme(SchemeName);
	return true;
}

//--------------------------------------------------------------------------
//	功能：设置用户账号
//--------------------------------------------------------------------------
void KUiBase::SetUserAccount(char* pszId, char* pszRole)
{
	if (pszId == NULL || pszId[0] == 0 || pszRole == NULL || pszRole[0] == 0)
		return;

	//根据(玩家)名称id获得对应处理过字符串id
	{
		m_UserAccountId[0] = 0;
		unsigned int	uID1 =  g_FileName2Id(pszId);
		unsigned int	uID2 = ~g_FileName2Id(pszRole);
		int				nLen1 = strlen(pszId);
		int				nLen2 = strlen(pszRole);
		sprintf(m_UserAccountId, "%x%xl%x%x", uID1, nLen1, nLen2, uID2);
	}
}

//清除当前玩家私有数据的存储目录
void KUiBase::CleanPrivateDataFolder()
{
	char	szBuffer[128];
	sprintf(szBuffer, "%s\\%s", UI_USER_DATA_FOLDER, m_UserAccountId);
	ClearFolder(szBuffer, true);
}

//清除当前玩家私有数据的某个文件
void KUiBase::DeletePrivateDataFile(const char* pszFileName)
{
	char	szBuffer[128], szPathFile[MAX_PATH];

	if (pszFileName && pszFileName[0])
	{
		sprintf(szBuffer, "%s\\%s\\%s", UI_USER_DATA_FOLDER, m_UserAccountId, pszFileName);
		g_GetFullPath(szPathFile, (char*)szBuffer);
		SetFileAttributes(szPathFile, FILE_ATTRIBUTE_NORMAL);
		DeleteFile(szPathFile);
	}
}

void SaveStallPrivateConfig(KIniFile* pIniFile);
void LoadStallPrivateConfig(KIniFile* pIniFile);

//--------------------------------------------------------------------------
//	功能：载入配置纪录，如定制的窗口位置
//--------------------------------------------------------------------------
void CreateBlackListUnit();
void KUiBase::LoadPrivateConfig()
{
	CreateBlackListUnit();

	KIniFile*	pConfigFile = GetPrivateSettingFile();

	if (pConfigFile)
	{
		//----获取窗口方案设定----
		char	SchemeName[32];
		if (pConfigFile->GetString("Main", "Scheme", "", SchemeName, 32))
		{
			if (SchemeName[0] && LoadScheme(SchemeName))
			{
			//----逐个窗口载入配置设定----
				KUiPlayerBar::LoadPrivateSetting(pConfigFile);
				KUiSkillTree::LoadConfig(pConfigFile);
				KShortcutKeyCentre::LoadPrivateSetting(pConfigFile);
				KUiChatCentre::LoadPrivateSetting(pConfigFile);	//在KShortcutKeyCentre之后,因为会有脚本生成Unit
			//----逐个窗口载入配置设定结束----
			}
		}
		::LoadStallPrivateConfig(pConfigFile);			//摆摊标价存盘
		ClosePrivateSettingFile(false);
	}
}

//--------------------------------------------------------------------------
//	功能：存储配置纪录
//--------------------------------------------------------------------------
int KUiBase::SavePrivateConfig()
{
	if (m_UserAccountId[0] == 0)
		return true;

	KIniFile* pConfigFile = GetPrivateSettingFile();
	if (pConfigFile)
	{
		pConfigFile->Clear();

		pConfigFile->WriteString("Main", "Scheme", m_CurScheme);
		//----逐个窗口保存配置设定----
		KUiSkillTree::SaveConfig(pConfigFile);
		KUiPlayerBar::SavePrivateSetting(pConfigFile);
		KUiChatCentre::SavePrivateSetting(pConfigFile);
		KShortcutKeyCentre::SavePrivateSetting(pConfigFile);
		::SaveStallPrivateConfig(pConfigFile);		//摆摊标价存盘
		//----逐个窗口保存配置设定结束----
		ClosePrivateSettingFile(true);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：获得当前玩家账号在界面系统中的id
//--------------------------------------------------------------------------
int KUiBase::GetUserPrivateDataFolder(char* pBuffer, int nSize)
{
	char	szBuffer[128];
	sprintf(szBuffer, "%s\\%s\\", UI_USER_DATA_FOLDER, m_UserAccountId);
	g_CreatePath(szBuffer);
	if (pBuffer && nSize > (int)(strlen(szBuffer)))
	{
		strcpy(pBuffer, szBuffer);
		return true;
	}
	return false;
}


int g_nWindowNTFlag = false;

static int _KSG_GetWindowVersion()
{
    g_nWindowNTFlag = (GetVersion() < 0x80000000);

    return true;
}

//--------------------------------------------------------------------------
//	功能：按照某种方案载入界面(Skin)
//--------------------------------------------------------------------------
int KUiBase::LoadScheme(const char* pScheme)
{
    _KSG_GetWindowVersion();

	if (pScheme == NULL)
		return false;

	if (m_CurScheme[0] != 0 && strcmp(pScheme, m_CurScheme) == 0)
		return true;

	char	Buffer[128];

	sprintf(Buffer, "%s\\"UI_PUBLIC_SETTING, m_CurSchemePath);
	if (GetSchemePath(pScheme))
	{
		int			nCount, nId, i;
		KIniFile	Ini;

		char		Section[8];
		//----卸载字体----
		if (g_pRepresentShell && Ini.Load(Buffer) &&
			Ini.GetInteger(FONT_SECTION, "Count", 0, &nCount))
		{
			for (i = 0; i < nCount; i++)
			{
				itoa(i, Section, 10);
				if (Ini.GetInteger(FONT_SECTION, Section, 0, &nId))
					g_pRepresentShell->ReleaseAFont(nId);
			}
		}

		sprintf(Buffer, "%s\\"UI_PUBLIC_SETTING, m_CurSchemePath);
		Ini.Load(Buffer);

		//----载入字体----
		if (g_pRepresentShell && Ini.GetInteger(FONT_SECTION, "Count", 0, &nCount))
		{
			for (i = 0; i < nCount; i++)
			{
				itoa(i, Section, 10);
				if (Ini.GetInteger(FONT_SECTION, Section, 0, &nId))
				{
					strcat(Section, "_File");
					if (Ini.GetString(FONT_SECTION, Section, "", Buffer, sizeof(Buffer)) &&
							Buffer[0])
					{
						g_pRepresentShell->CreateAFont(Buffer, CHARACTER_CODE_SET_GBK, nId);
					}
				}
			}
		}
		//----载入字体结束----
		WndObjContainerInit(&Ini);

		for (i = CURSOR_NORMAL; i < CURSOR_INDEX_COUNT; i++)
		{
			itoa(i, Section, 10);
            if (g_nWindowNTFlag)
			    Ini.GetString("CursorList", Section, "", Buffer, sizeof(Buffer));
            else    // Win9x
			    Ini.GetString("CursorList98", Section, "", Buffer, sizeof(Buffer));

			Wnd_LoadCursor(i, Buffer);//载入鼠标指针资源
		}

		LoadSchemeForEachWnd();

		return true;
	}
	return false;
}

void KUiBase::LoadSchemeForEachWnd()
{
	//----逐个窗口界面方案----
	KUiPlayerBar::LoadScheme(m_CurSchemePath);
	KUiStatus::LoadScheme(m_CurSchemePath);
	KUiMsgCentrePad::LoadScheme(m_CurSchemePath);
//	KUiManage::LoadScheme(m_CurSchemePath);
	KUiOptions::LoadScheme(m_CurSchemePath);
	KUiItem::LoadScheme(m_CurSchemePath);
	KUiSkills::LoadScheme(m_CurSchemePath);
	KUiStoreBox::LoadScheme(m_CurSchemePath);
	KUiMsgSel::LoadScheme(m_CurSchemePath);
	KUiSelPlayerNearby::LoadScheme(m_CurSchemePath);
	KUiChatCentre::LoadScheme(m_CurSchemePath);
	KUiChatStatus::LoadScheme(m_CurSchemePath);
	KUiTeamManage::LoadScheme(m_CurSchemePath);
	KUiHeaderControlBar::LoadScheme(m_CurSchemePath);
	KUiShop::LoadScheme(m_CurSchemePath);
	KUiSysMsgCentre::LoadScheme(m_CurSchemePath);
	KUiMiniMap::LoadScheme(m_CurSchemePath);
	g_MouseOver.LoadScheme(m_CurSchemePath);
	g_UiInformation.LoadScheme(m_CurSchemePath);
	g_UiInformation2.LoadScheme(m_CurSchemePath);
	g_UiFollowInfo.LoadScheme(m_CurSchemePath);
	KPopupMenu::LoadTheme(m_CurSchemePath);
	KUiSelColor::LoadScheme(m_CurSchemePath);
	//----逐个窗口界面方案结束----
}

//--------------------------------------------------------------------------
//	功能：获得当前指定界面方案的目录名，如果未能找到指定的方案，则取第一个
//		　方案（默认方案）作为当前方案
//	参数：pScheme --> 指定界面方案名称
//--------------------------------------------------------------------------
int KUiBase::GetSchemePath(const char* pScheme)
{
	KIniFile* pIni = NULL;
	int			nCount;
	char		Buff[32], Key[32];

	if (pScheme == NULL ||
		(pIni = GetCommConfigFile()) == NULL)
		return false;

	pIni->GetInteger(THEME_SECTION, "Count", 0, &nCount);
	for (int i = 0; i < nCount; i++)
	{
		itoa(i, Key, 10);
		if (pIni->GetString(THEME_SECTION, Key, "", Buff, 32))
		{
			if (strcmp(Buff, pScheme) == 0)
			{
				strcpy(m_CurScheme, pScheme);
				strcat(Key, "_Path");
				if (pIni->GetString(THEME_SECTION, Key, "", Buff, 32) && Buff[0])
				{
					sprintf(m_CurSchemePath, "%s\\%s", UI_INFO_FILE_PATH, Buff);
					CloseCommConfigFile();
					return true;
				}
			}
		}
	}
	if (pIni->GetString(THEME_SECTION, "0", "", m_CurScheme, 32) &&
		pIni->GetString(THEME_SECTION, "0_Path", "", Buff, 32) && Buff[0])
	{
		sprintf(m_CurSchemePath, "%s\\%s", UI_INFO_FILE_PATH, Buff);
		CloseCommConfigFile();
		return true;
	}
	CloseCommConfigFile();
	return false;
}

//--------------------------------------------------------------------------
//	功能：获得界面方案的数目
//--------------------------------------------------------------------------
int KUiBase::SchemeCount()
{
	int			nCount = 0;
	KIniFile*	pIni = GetCommConfigFile();
	if (pIni)
	{
		pIni->GetInteger(THEME_SECTION, "Count", 0, &nCount);
		if (nCount < 0)
			nCount = 0;
		CloseCommConfigFile();
	}
	return nCount;
}

//--------------------------------------------------------------------------
//	功能：获得某个界面方案的名称与路径
//	参数：pName --> 用于获取方案的名称，为指向一个长度不小于的32的缓冲区的指针，或者空指针。
//		  pPath --> 用于获取方案的路径，为指向一个长度不小于的32的缓冲区的指针，或者空指针。
//--------------------------------------------------------------------------
int KUiBase::GetScheme(int nIndex, char* pName, char* pPath)
{
	KIniFile	Ini;
	int			nCount = 0;
	KIniFile*	pIni = GetCommConfigFile();
	if (pIni)
	{
		Ini.GetInteger(THEME_SECTION, "Count", 0, &nCount);
		if (nIndex >= 0 && nIndex < nCount)
		{
			char	Num[8] = "";
			itoa(nIndex, Num, 10);
			if (pName)
				Ini.GetString(THEME_SECTION, Num, "", pName, 32);
			if (pPath)
				Ini.GetString(THEME_SECTION, Num, "", pPath, 32);
			return true;
		}
		CloseCommConfigFile();
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：获得当前界面方案的路径
//--------------------------------------------------------------------------
int KUiBase::GetCurSchemePath(char* pBuffer, int nSize)
{
	if (pBuffer && nSize >= (int)strlen(m_CurSchemePath) + 1)
	{
		strcpy(pBuffer, m_CurSchemePath);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：打开保存界面公共设置的文件
//--------------------------------------------------------------------------
KIniFile*	KUiBase::GetCommSettingFile()
{
	if (m_pUiCommSettingFile == NULL)
	{
		m_pUiCommSettingFile = new KIniFile;
		if (m_pUiCommSettingFile)
			m_pUiCommSettingFile->Load((char*)UI_COMMON_SETTING_FILE);
	}
	return m_pUiCommSettingFile;
}

//--------------------------------------------------------------------------
//	功能：关闭保存界面公共设置的文件
//--------------------------------------------------------------------------
void KUiBase::CloseCommSettingFile(bool bSave)
{
	if (m_pUiCommSettingFile)
	{
		if (bSave)
			m_pUiCommSettingFile->Save((char*)UI_COMMON_SETTING_FILE);
		delete(m_pUiCommSettingFile);
		m_pUiCommSettingFile = NULL;
	}
}

KIniFile* KUiBase::GetCommConfigFile()
{
	if (m_pUiCommConfigFile == NULL)
	{
		m_pUiCommConfigFile = new KIniFile;
		if (m_pUiCommConfigFile &&
			!m_pUiCommConfigFile->Load((char*)UI_COMMON_CONFIG_FILE))
		{
			delete m_pUiCommConfigFile;
			m_pUiCommConfigFile = NULL;
		}
	}
	return m_pUiCommConfigFile;
}

void KUiBase::CloseCommConfigFile()
{
	if (m_pUiCommConfigFile)
	{
		delete(m_pUiCommConfigFile);
		m_pUiCommConfigFile = NULL;
	}
}

//--------------------------------------------------------------------------
//	功能：打开打开当前账号的设置文件
//--------------------------------------------------------------------------
KIniFile*	KUiBase::GetPrivateSettingFile()
{
	if (m_pUiPrivateSettingFile == NULL && m_UserAccountId[0])
	{
		m_pUiPrivateSettingFile = new KIniFile;
		if (m_pUiPrivateSettingFile)
		{
			char	FileName[128];
			sprintf(FileName, "%s\\%s\\%s", UI_USER_DATA_FOLDER, m_UserAccountId, UI_PRIVATE_SETTING_FILE);
			m_pUiPrivateSettingFile->Load(FileName);
		}
	}
	return m_pUiPrivateSettingFile;
}

//--------------------------------------------------------------------------
//	功能：关闭打开当前账号的设置文件
//--------------------------------------------------------------------------
void KUiBase::ClosePrivateSettingFile(bool bSave)
{
	if (m_pUiPrivateSettingFile)
	{
		if (bSave && m_UserAccountId[0])
		{
			char	FileName[128];
			sprintf(FileName, "%s\\%s", UI_USER_DATA_FOLDER, m_UserAccountId);
			g_CreatePath(FileName);
			strcat(FileName, "\\");
			strcat(FileName, UI_PRIVATE_SETTING_FILE);
			m_pUiPrivateSettingFile->Save(FileName);
		}
		delete(m_pUiPrivateSettingFile);
		m_pUiPrivateSettingFile = NULL;
	}
}


/**
 * @brief 设置状态
 * @param eStatus 要设置的状态，查看enum UISYS_STATUS
 * @return 此次设置状态的句柄，在释放的时候要使用到
 */
void KUiBase::SetStatus(UISYS_STATUS eStatus, int &nHandle)
{
	if(eStatus != UIS_S_IDLE)
	{
		STATUS_HANDLE_LIST::iterator itNode;
		if((itNode = m_sStatusHandleList.find(nHandle)) != m_sStatusHandleList.end())
		{
			m_aryStatus[&(itNode->second)]--;	//对于eStatus，&操作符重载为另外一个含义
			itNode->second = eStatus;
		}
		else
		{
			m_nStatusHandleCount++;
			if(m_nStatusHandleCount < 1)
			{
				m_nStatusHandleCount = 1;
			}
			m_sStatusHandleList[m_nStatusHandleCount] = eStatus;
			nHandle = m_nStatusHandleCount;
		}
		m_aryStatus[&eStatus] ++;	//对于eStatus，&操作符重载为另外一个含义
		Wnd_GameSpaceHandleInput(!GetStatus());
		SetCurrentCursor();
	}
}


/**
 * @brief 获取有哪些状态被设置了
 */
DWORD KUiBase::GetStatus()
{
	DWORD dwRet = 0;
	UISYS_STATUS eUIS = UIS_S_IDLE;

	for(int i = 0;i < UIS_S_NUM;i++)
	{
		if(m_aryStatus[i])
		{
			dwRet |= eUIS;
		}
		eUIS++;
	}
	return dwRet;
}


/**
 * @brief 获取现象状态（现象指表现出来的现象，其实就是哪个状态当前设置最多）
 */
DWORD KUiBase::GetAppearStatus()
{
	int nRet, nCount;
	UISYS_STATUS eUIS;

	nRet = 0;
	eUIS = UIS_S_IDLE;
	nCount = m_aryStatus[0];
	for(int i = 0;i < UIS_S_NUM;i++)
	{
		if(m_aryStatus[i] > nCount)
		{
			nCount = m_aryStatus[i];
			nRet = (int)eUIS;
		}
		eUIS++;
	}
	return nRet;
}


/**
 * @brief 释放已经设置的状态
 */
void KUiBase::ReleaseStatus(int &nHandle)
{
	//句柄非0的话，要查找这个句柄是否有分配，有的话就就删去，并且执行释放
	//没有的话就直接返回不执行释放
	STATUS_HANDLE_LIST::iterator itNode = m_sStatusHandleList.find(nHandle);
	if(itNode != m_sStatusHandleList.end())
	{
		//释放
		int nIndex = &(itNode->second);	//对于eStatus，&操作符重载为另外一个含义
		if(m_aryStatus[nIndex])
		{
			m_aryStatus[nIndex]--;
		}
		m_sStatusHandleList.erase(itNode);
		nHandle = 0;
		Wnd_GameSpaceHandleInput(!GetStatus());
	}
}


/**
 * @biref 清空状态设置数据
 */
void KUiBase::ClearStatus()
{
	m_sStatusHandleList.clear();
	memset(m_aryStatus, 0, sizeof(m_aryStatus));
}


//根据当前状态设置鼠标指针图形
void KUiBase::SetCurrentCursor()
{
	int	nCursorIndex;
	switch(GetAppearStatus())
	{
	case UIS_S_TRADE_PLAYER:	//（与Player）交易
	case UIS_S_TRADE_NPC:		//（与npc）交易
		nCursorIndex = CURSOR_NORMAL;
		break;
	case UIS_S_TRADE_SALE:		//（与npc）交易 卖
		nCursorIndex = CURSOR_SELL;
		break;
	case UIS_S_TRADE_BUY:		//（与npc）交易 买
		nCursorIndex = CURSOR_BUY;
		break;
	case UIS_S_TRADE_REPAIR:	//（与npc）交易 修理
		nCursorIndex = CURSOR_REPAIR;
		break;
	case UIS_S_STALL_SETPRICE:
		nCursorIndex = CURSOR_STALL_MARK_PIRCE;	//暂时用这个鼠标指针来标记[wxb 2003-11-18]
		break;
	case UIS_S_STALL_BUY:
		nCursorIndex = CURSOR_BUY;	//摆摊:(买方)处于买东西状态
		break;
	default:
		nCursorIndex = CURSOR_NORMAL;
		break;
	}
	Wnd_SwitchCursor(nCursorIndex);
}

int	KUiBase::IsOperationEnable(UISYS_OPERATION uOper)
{
	int bEnable = false;
	DWORD dwStatus = GetStatus();
	switch(uOper)
	{
	case UIS_O_MOVE_ITEM:
		{
			DWORD dwBit = UIS_S_BESET | UIS_S_STORE_BOX;
			bEnable = ((!dwStatus) || ((!(dwStatus & ~dwBit)) && (dwStatus & dwBit)));
		}
		break;
	case UIS_O_USE_ITEM:
		bEnable = !(dwStatus);
		break;
	case UIS_O_TRADE_ITEM:
		{
			DWORD dwBit = UIS_S_TRADE_PLAYER | UIS_S_TRADE_NPC | UIS_S_TRADE_SALE | UIS_S_TRADE_BUY;
			bEnable = (dwStatus & dwBit) && (!(dwStatus & ~dwBit));
		}
		break;
	case UIS_O_REPARE_ITEM:
		bEnable = (dwStatus & UIS_S_TRADE_REPAIR);
	}
	return bEnable;
}

void KUiBase::RegisterEvent(const char* szName, HANDLE hWnd)
{
	if (hWnd == 0)
		return;

	EVENTWNDLIST::iterator i = m_EventWndList.find(szName);
	if (i == m_EventWndList.end())
	{
		m_EventWndList[szName] = hWnd;
	}
}

void KUiBase::UnregisterEvent(const char* szName, HANDLE hWnd)
{
	if (hWnd == 0)
		return;

	EVENTWNDLIST::iterator i = m_EventWndList.find(szName);
	if (i != m_EventWndList.end() && i->second == hWnd)
	{
		m_EventWndList.erase(i);
	}
}

int KUiBase::NotifyOneEvent(const char* szName, const char* szEvent)		//0为已经处理, 不要再处理了, 非0为未处理或错误
{
	if (szEvent == NULL || szEvent[0] == 0)
		return -1;

	EVENTWNDLIST::iterator i = m_EventWndList.find(szName);
	if (i == m_EventWndList.end())
		return -1;

	int nLen = strlen(szEvent) + 1;

	int nRet = -1;
	if (i->second)
	{
		COPYDATASTRUCT cp;
		cp.dwData = 0;
		cp.cbData = nLen;
		cp.lpData = (void*)szEvent;
		nRet = ::SendMessage((HWND)i->second, WM_COPYDATA, (WPARAM)g_GetMainHWnd(), (LPARAM)&cp);
	}

	return nRet;
}

int KUiBase::NotifyEvent(const char* szEvent)		//0为已经处理, 不要再处理了, 非0为未处理或错误
{
	if (szEvent == NULL || szEvent[0] == 0)
		return -1;

	int nLen = strlen(szEvent) + 1;

	int nRet = -1;
	EVENTWNDLIST::iterator i = m_EventWndList.begin();
	while(i != m_EventWndList.end())
	{
		if (i->second)
		{
			COPYDATASTRUCT cp;
			cp.dwData = 0;
			cp.cbData = nLen;
			cp.lpData = (void*)szEvent;
			nRet = ::SendMessage((HWND)i->second, WM_COPYDATA, (WPARAM)g_GetMainHWnd(), (LPARAM)&cp);
			if (!nRet)
				return nRet;
		}
		i++;
	}

	return nRet;
}

KFile* KUiBase::OpenSaveSentenceFile(const char* szRoleName)
{
	KFile* pFile = NULL;

	if (pFile)
	{
		pFile->Seek(0, FILE_END);
	}

	return pFile;
}

void KUiBase::SaveSentence(KFile* pFile, char *lpFromLogonName, char *lpFromName, char *lpszTargetLogonName, char *lpszTargetName, char *lpszSentence, int nSencLen)
{
	if (!pFile || !lpFromName || !lpszTargetName || !lpszSentence)
		return;
	if (!lpFromName[0] || !lpszTargetName[0] || !lpszSentence[0])
		return;

	char Buffer[256];
	SYSTEMTIME systm;
	::GetLocalTime(&systm);
	int nLen = 0;
	nLen = sprintf(Buffer, "<Message Date=\"%d-%d-%d\" Time=\"%d:%d:%d\">",
		systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond);
	pFile->Write(Buffer, nLen);
	nLen = sprintf(Buffer, "<From>");
	pFile->Write(Buffer, nLen);
	nLen = sprintf(Buffer, "<User LogonName=\"%s\" FriendlyName=\"%s\"/>", lpFromLogonName, lpFromName);
	pFile->Write(Buffer, nLen);
	nLen = sprintf(Buffer, "</From>");
	pFile->Write(Buffer, nLen);
	nLen = sprintf(Buffer, "<To>");
	pFile->Write(Buffer, nLen);
	nLen = sprintf(Buffer, "<User LogonName=\"%s\" FriendlyName=\"%s\"/>", lpszTargetLogonName, lpszTargetName);
	pFile->Write(Buffer, nLen);
	nLen = sprintf(Buffer, "</To>");
	pFile->Write(Buffer, nLen);
	nLen = sprintf(Buffer, "<Text>");
	pFile->Write(Buffer, nLen);

	pFile->Write(lpszSentence, nSencLen);

	nLen = sprintf(Buffer, "</Text>");
	pFile->Write(Buffer, nLen);
	nLen = sprintf(Buffer, "</Message>");
	pFile->Write(Buffer, nLen);
}

void KUiBase::CloseSaveSentenceFile(KFile* pFile)
{
	if (pFile)
	{
		pFile->Close();
	}
}

///////////////////////////////////////////////////////////////////////////////

int AddinNotify::CreateUnit(const char* Unit)
{
	if (Unit && Unit[0])
	{
		char szEvent[256];
		sprintf(szEvent, APP_CREATEUNIT, Unit);
		return g_UiBase.NotifyEvent(szEvent);
	}

	return 1;
}

int AddinNotify::DeleteUnit(const char* Unit)
{
	if (Unit && Unit[0])
	{
		char szEvent[256];
		sprintf(szEvent, APP_DELETEUNIT, Unit);
		return g_UiBase.NotifyEvent(szEvent);
	}

	return 1;
}

int AddinNotify::CreateUnitGroup(const char* Unit, const char* Name)
{
	if (Unit && Unit[0] && Name)
	{
		char szEvent[256];
		sprintf(szEvent, APP_CREATEUNITGROUP, Unit, Name);
		return g_UiBase.NotifyEvent(szEvent);
	}

	return 1;
}

int AddinNotify::RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends)
{
	if (Unit && Unit[0] && Name && NewName)
	{
		char szEvent[512];
		int nLen = sprintf(szEvent, APP_RENAMEUNITROUP, Unit, Name, NewName);
		return g_UiBase.NotifyEvent(szEvent);
	}

	return 1;
}

int AddinNotify::DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends)
{
	if (Unit && Unit[0] && Name)
	{
		char szEvent[512];
		int nLen = sprintf(szEvent, APP_DELETEUNITROUP, Unit, Name);
		return g_UiBase.NotifyEvent(szEvent);
	}

	return 1;
}

int AddinNotify::MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends)
{
	if (Unit && Unit[0] && Name && Name2)
	{
		char szEvent[512];
		int nLen = sprintf(szEvent, APP_MOVEUNITROUP, Unit, Name, Name2);
		return g_UiBase.NotifyEvent(szEvent);
	}

	return 1;
}

int	AddinNotify::CreateUnitMember(const char* Unit, const char* Name, const char* Group)
{
	if (Unit && Unit[0] && Name && Group)
	{
		char szEvent[256];
		sprintf(szEvent, APP_CREATEUNITMEMBER, Unit, Name, Group);
		return g_UiBase.NotifyEvent(szEvent);
	}

	return 1;
}

int AddinNotify::DeleteUnitMember(const char* Unit, const char* Name)
{
	if (Unit && Unit[0] && Name)
	{
		char szEvent[256];
		sprintf(szEvent, APP_DELETEUNITMEMBER, Unit, Name);
		return g_UiBase.NotifyEvent(szEvent);
	}

	return 1;
}

int	AddinNotify::MoveUnitMember(const char* Unit, const char* Name, const char* Group)
{
	if (Unit && Unit[0] && Name && Group)
	{
		char szEvent[256];
		sprintf(szEvent, APP_MOVEUNITMEMBER, Unit, Name, Group);
		return g_UiBase.NotifyEvent(szEvent);
	}

	return 1;
}

int	AddinNotify::SetUnitMemberStatus(const char* Unit, const char* Name, const char* status)
{
	if (Unit && Unit[0] && Name && status)
	{
		char szEvent[256];
		sprintf(szEvent, APP_SETUNITMEMBERSTATUS, Unit, Name, status);
		return g_UiBase.NotifyEvent(szEvent);
	}

	return 1;
}

AddinNotify s_ANotify;

void CreateAddinUnit(const char* UnitName)
{
	KUiChatCentre::AddAddinUnit(UnitName, &s_ANotify);
}

void DeleteAddinUnit(const char* UnitName)
{
	KUiChatCentre::RemoveAddinUnit(UnitName);
}


/**
 * @brief 获取账号设置文件
 */
KIniFile* KUiBase::GetAccountSettingFile(char* szAccount)
{
	if(szAccount && szAccount[0])
	{
		unsigned int uID1 =  g_FileName2Id(szAccount);
		unsigned int uID2 =  g_FileName2Id(UI_USER_DATA_FOLDER);
		sprintf(m_szAccountSettingFilePath, "%s\\%u\\", UI_USER_DATA_FOLDER, uID2);
		g_CreatePath(m_szAccountSettingFilePath);
		sprintf(m_szAccountSettingFilePath, "%s\\%u.ini", m_szAccountSettingFilePath, uID1);
		if(m_pAccountSettingFile)
		{
			m_pAccountSettingFile->Clear();
		}
		else
		{
			m_pAccountSettingFile = new KIniFile;
		}
		if(m_pAccountSettingFile)
		{
			m_pAccountSettingFile->Load(m_szAccountSettingFilePath);
		}
	}
	return m_pAccountSettingFile;
}


//获取账号设置文件的路径
void KUiBase::GetAccountSettingFilePath(char *pszBuffer)
{
	if(m_szAccountSettingFilePath && pszBuffer)
	{
		strcpy(pszBuffer, m_szAccountSettingFilePath);
	}
}


/**
 * @brief 为UISYS_STATUS取下标重载&运算符
 */
int operator&(UISYS_STATUS &eEnum)
{
	int nIndex;
	UISYS_STATUS eStatus = UIS_S_IDLE;
	for(nIndex = 0;nIndex < UIS_S_NUM;nIndex++)
	{
		if(eEnum == eStatus)
		{
			break;
		}
		eStatus++;
	}
	if(nIndex == UIS_S_NUM)
	{
		nIndex = 0;
	}
	return nIndex;
}

/**
 * @brief 为UISYS_STATUS取下一个值重载++运算符
 */
UISYS_STATUS operator++(UISYS_STATUS &eEnum, int)
{
	int nEnum;
	if(eEnum == UIS_S_IDLE)
	{
		eEnum = (UISYS_STATUS)1;
	}
	else
	{
		nEnum = (int)eEnum << 1;
		eEnum = (UISYS_STATUS)nEnum;
	}
	return eEnum;
}
