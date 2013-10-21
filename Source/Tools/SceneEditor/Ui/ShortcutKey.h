/*****************************************************************************************
//	快捷键系统
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-1-13
------------------------------------------------------------------------------------------
	用于设置，获取，检查各种控制快捷键；读取，保存快捷键设置。
	当前支持的快捷键为：字母键A至Z;数字键0至9;功能键F1至F11。
*****************************************************************************************/
#pragma once

#include "../Public/KLuaScript.h"

#define SCK_SHORTCUT_FRIEND "Open([[friend]])"
#define SCK_SHORTCUT_SYSTEM "Open([[system]])"
#define SCK_SHORTCUT_ITEMS	"Open([[items]])"
#define SCK_SHORTCUT_STATUS "Open([[status]])"
#define SCK_SHORTCUT_MAP	"Open([[map]])"
#define SCK_SHORTCUT_SKILLS	"Open([[skills]])"
#define SCK_SHORTCUT_TEAM	"Open([[team]])"
#define SCK_SHORTCUT_OPTIONS "Open([[options]])"

#define SCK_SHORTCUT_SIT	"Switch([[sit]])"
#define SCK_SHORTCUT_RUN	"Switch([[run]])"
#define SCK_SHORTCUT_PK		"Switch([[pk]],%d)"
#define SCK_SHORTCUT_HORSE	"Switch([[horse]])"
#define SCK_SHORTCUT_TRADE	"Switch([[trade]])"
#define SCK_SHORTCUT_SHOWPLAYERNAME	"Switch([[showplayername]])"
#define SCK_SHORTCUT_SHOWPLAYERNUMBER	"Switch([[showplayernumber]])"

#define SCK_SHORTCUTSKILL_FORMAT	"ShortcutSkill(%d)"
#define SCK_DIRECTSHORTCUTSKILL_FORMAT	"DirectShortcutSkill(%d)"

struct COMMAND_SETTING
{
	DWORD uKey;				//第一关键字
	char szCommand[32];		//第二关键字,当第一关键字为0时有效
	char szDo[128];
};

#pragma warning(disable:4786)
#include "string"
#include "map"
#include "list"
#include "tchar.h"
#include "commctrl.h"

struct string_less
{
	bool operator() (const std::string& src1, const std::string& src2) const
	{
		const size_t len1 = src1.size();
		const size_t len2 = src2.size();
		if (len1 < len2)
			return true;
		if (len1 > len2)
			return false;
		return _tcscmp(src1.c_str(), src2.c_str()) < 0;
	}
};
struct string_iless
{
	bool operator() (const std::string& src1, const std::string& src2) const
	{
		const size_t len1 = src1.size();
		const size_t len2 = src2.size();
		if (len1 < len2)
			return true;
		if (len1 > len2)
			return false;
		return _tcsicmp(src1.c_str(), src2.c_str()) < 0;
	}
};

typedef std::list<std::string> PARAMLIST;

struct ShortFuncInfo
{
	std::string strName;
	int nParamNum;
	PARAMLIST strDefaultParam;
};

typedef std::map<std::string, ShortFuncInfo, string_iless> SHORTFUNCMAP;

class KIniFile;

class KShortcutKeyCentre
{
public:
	static int			HandleKeyInput(unsigned int uKey, int nModifier);
	static int			HandleMouseInput(unsigned int uKey, int nModifier, int x, int y);

	static BOOL			InitScript();
	static BOOL			LoadScript(char* pFileName);
	static BOOL			ClearScript();
	static BOOL			UninitScript();
	static BOOL			LoadPrivateSetting(KIniFile* pFile);
	static int			SavePrivateSetting(KIniFile* pFile);
	static BOOL			ExcuteScript(const char * ScriptCommand);
	static int			AddCommand(COMMAND_SETTING* pAdd);	//复制Add数据并增加到Commands中，如果uKey!=0则覆盖原uKey，否则如szCommand[0]!=0则覆盖szCommand相同者
	static int			RemoveCommand(int nIndex);	//返回剩余Command的总数
	static int			FindCommand(DWORD uKey);
	static int			FindCommand(const char* szCommand);
	static int			FindCommandByScript(const char* szScript);
	static DWORD		GetCommandKey(int nIndex);
	static const char*	GetKeyName(DWORD Key);

	static	void		Enable(bool b);

	static BOOL			TranslateExcuteScript(const char * ScriptCommand);

	static BOOL			RegisterFunctionAlias(const char * strFunAlias, const char * strFun, int nParam, const PARAMLIST& List);
	
	static void			RemoveCommandAll();
	
	static BOOL			ExcuteHWNDScript(const char * ScriptCommand);

	static SHORTFUNCMAP ms_FunsMap;
	static int	ms_MouseX;
	static int	ms_MouseY;
	static bool	ms_bMouse;
private:
	static KLuaScript	ms_Script;
	static COMMAND_SETTING* ms_pCommands;
	static int ms_nCommands;
	static bool ms_Enable;
};

#define VK_LDBUTTON 0x0100
#define VK_RDBUTTON 0x0101
#define VK_MDBUTTON 0x0102
