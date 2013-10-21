/*****************************************************************************************
//	界面--聊天快捷输入短语
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-3-23
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#define	MAX_PHRASE_COUNT	10
#define MAX_EMOTE_LENGTH    128
#define MAX_NAME_AND_CMD_LENGTH 16

#define	HAT_MSG_SPECIAL_CONTROL_PREFIX	VK_ESCAPE

//聊天语句前缀中
enum	CHAT_MSG_SPECIAL_CONTROL
{
	CMSC_EMOTE = 1,			//角色动作
	CMSC_AUTOREPLY_MSG,		//自动回复的密聊消息
	CMSC_OFFLINE_MSG,		//自己不在线时别人发来的消息
};

class KUiChatPhrase
{
public:
	KUiChatPhrase();
	~KUiChatPhrase();
	static int		Initialize();
	int		GetPhrase(int nIndex, char* pszPhrase);
	int		SetPhrase(int nIndex, char* pszPhrase, int nLen);
	int		GetPhraseCount();
	int		SavePrivateSetting(KIniFile* pFile, LPCSTR lpSection, int nStart);
	int     SavePrivateEmote(KIniFile* pFile, LPCSTR lpSection, int nStart);
	void	ClearAllPhrase();

	enum EMOTE_TYPE
	{
		EMOTE_T_PHRASE = 0,
		EMOTE_T_MENU,
	};
	//载入所有聊天动作文件
	int     LoadEntireEmote();
	//设置聊天动作，有给出index的就改index的，没有就根据Cmd寻找要替换的
	int     SetEmote(char *szCmd, char *szName, char *szStringTarget, char *szStringSelf, int nIndex = -1);
	//
	int     GetEmoteCount(EMOTE_TYPE eType);
	//根据命令获取一个动作描述(pBuff)，返回描述长度，这里是包括了MENU和PHRASE的
	int     GetEmote(const char *szCmd, char *pBuff, int nBuffLen, int nType = 0);
	//根据索引获取一个动作描述(pBuff)，返回描述长度，，这里是包括了MENU和PHRASE的
	int     GetEmote(int nIndex, char *pBuff, int nBuffLen, int nType = 0);
	//获取一个用于菜单上的动作描述(pBuff)，返回描述长度，仅仅MENU
	int     GetMenuEmote(const char *szCmd, char *pBuff, int nBuffLen, int nType = 0);
	int     GetMenuEmote(int nIndex, char *pBuff, int nBuffLen, int nType = 0);
	//取出指定索引的动作名字
	int     GetMenuEmoteName(int nIndex, char *pBuff, int nBuffLen);
	//制作字符串
	int     ConvertEmoteString(char *szString, int nStringLen, const char *szMyName, const char *szTarName);

	//把读取到的Emote信息构造进去Shortcut系统中的函数别名表
	int     ConstructFunctionAlias();
	int     ConstructAAlias(char* szCmd, int nIndex);

	//设置自动回复语句
	static bool	SetAutoReply(const char* pszWords, bool bTakeEffect);
	//设置自动回复语句
	static bool SetAutoReply(int nIndex);
	//获取自动回复语句
	static int	GetAutoReply(char* pszBuffer, int nSize, bool bIncludePrefix);

private:
	struct  EMOTE_PHRASE
	{
		char    szName[MAX_NAME_AND_CMD_LENGTH];
		int     nNameLen;
		char    szString[MAX_EMOTE_LENGTH];
		int     nStringLen;
		char    szStringMe[MAX_EMOTE_LENGTH];
		int     nStringMeLen;
		char    szCmd[MAX_NAME_AND_CMD_LENGTH];
	} *m_pEmotePhrase, *m_pMenuEmote;
	
	int     LoadEmotePhrase(KIniFile *pIni);
	int     LoadMenuEmote(KIniFile *pIni);
	int     HandleLoad(KIniFile *pIni, EMOTE_PHRASE *pData, int nCount, const char *szSection);
	int     FindEmote(EMOTE_TYPE eType, const char *szCmd);

private:
	struct	CHAT_PHRASE
	{
		char	szString[64];
		int		nStringLen;
	}		m_PhraseList[MAX_PHRASE_COUNT];

	int m_nEmotePhraseCount;
	int m_nMenuEmoteCount;

	unsigned int m_uMyNameColor;
	unsigned int m_uTargetColor;

private:
	//自动回复内容
#define			REPLY_COUNT		4
#define			MAX_REPLY_LEN	128
	//当前使用的自动回复的索引，-1表示当前未设定使用自动回复
	static int		ms_nCurrentUseReplyIndex;
	//自动回复的文字内容，第0条为玩家自设定的
	static char		ms_szReplyContent[REPLY_COUNT][MAX_REPLY_LEN];
	//自动回复内容的长度
	static int		ms_nReplyContentLen[REPLY_COUNT];
};

extern KUiChatPhrase	g_UiChatPhrase;