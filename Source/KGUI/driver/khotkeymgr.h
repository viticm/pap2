////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : khotkeymgr.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-8-2 14:19:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KHOTKEYMGR_H_
#define _INCLUDE_KHOTKEYMGR_H_

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

#define VK_MOUSEWHEELUP		0x0100
#define VK_MOUSEWHEELDOWN	0x0101
#define VK_MOUSEHOVER		0x0102

#define UI_HOTKEY_MODIFY_CTRL	0x0001
#define UI_HOTKEY_MODIFY_SHIFT	0x0002
#define UI_HOTKEY_MODIFY_ALT	0x0004

#define KHOTKEY_TO_DWORD(HotKey) ((DWORD)HotKey.uKey | ((DWORD)HotKey.uModify << 16))
#define DWORD_TO_KHOTKEY(nKey) (KHotkeyMgr::KHOTKEY(nKey & 0x0000FFFF, (nKey & 0xFFFF0000) >> 16))

class KHotkeyMgr
{
public:
	struct KHOTKEY
	{
		unsigned short uModify;	//需要的辅助键 
		unsigned short uKey; //虚拟键值
		KHOTKEY()
		{
			uKey = 0;
			uModify = 0;
		}
		KHOTKEY(unsigned short uInKey, unsigned short uInModify)
		{
			uKey = uInKey;
			uModify = uInModify;
		}
		bool operator<(const KHOTKEY& rhs) const
		{
			if (uKey < rhs.uKey)
				return true;
			if (uKey == rhs.uKey)
				return uModify < rhs.uModify;
			return false;
		}
		bool operator==(const KHOTKEY& rhs) const
		{
			return rhs.uKey == uKey && rhs.uModify == uModify;
		}

	};

	struct KHOTKEYBINDING
	{
		char szName[32];
		unsigned int uNameHash;
		char szDesc[64];
		char szHeader[64];
		char szTip[128];
		int nUnchangeable;
		int nScriptDown;
		int nScriptUp;
		KHOTKEY Hotkey[2];

		KHOTKEYBINDING()
		{
			szName[0] = '\0';
			uNameHash = g_StringHash(szName);
			szDesc[0] = '\0';
			szHeader[0] = '\0';
			szTip[0] = '\0';
			nUnchangeable = false;
			nScriptDown = LUA_NOREF;
			nScriptUp = LUA_NOREF;
		}

		bool operator==(const KHOTKEYBINDING& rhs) const
		{
			if (strcmp(szName, rhs.szName) == 0)
				return true;
			return false;
		}

		bool operator==(const LPCSTR& pcszCommand) const
		{
			if (strcmp(szName, pcszCommand) == 0)
				return true;
			return false;
		}

		bool operator==(const unsigned int & ruNameHash) const
		{
			return uNameHash == ruNameHash;
		}
	};
	typedef std::vector<KHOTKEYBINDING> KHotKeyBindingArray;


	struct KHOTKEYINFO
	{
		int nCommand;	//在KHotKeyBindingArray中的索引
		int nDown;		//已经被按下
		KHOTKEYINFO()
		{
			nCommand = -1;
			nDown = false;
		}
	};
	typedef std::map<KHOTKEY, KHOTKEYINFO> KHotkeyMap;

public:
	KHotkeyMgr();
	~KHotkeyMgr();

    bool Init();
	void Exit();
	bool ReInit();

	int HandleHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam);
	int FlipAllDownHotkey();

	//下面是提供给设置快捷键和载入玩家自定义的快捷键的接口。
	//捕获所有快捷键,用于快捷键设置
	int SetCaptureAllHotkey(int nCapture);
	int CaptureAllHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam);

	//载入默认快捷键设置
	int LoadDefault();
	int Load(LPCSTR pcszFileName);
	int Save(LPCSTR pcszFileName);

	int LoadAsAdd(LPCSTR pcszFileName);
	int SaveAsAdd(LPCSTR pcszFileName);

	int LoadAsAdd(Lua_State* L);
	int SaveAsAdd(Lua_State* L);

	int Clear();
	int Set(KHOTKEY Hotkey, LPCSTR pcszCommand, int nIndex);
	int Set(KHOTKEY Hotkey, unsigned int uCommand, int nIndex);
	KHOTKEY Get(LPCSTR pcszCommand, int nIndex);

	int IsUsed(KHOTKEY Hotkey);

	int Remove(KHOTKEY Hotkey);

	int IsKeyDown(KHOTKEY Hotkey);

	int GetBindingCount();
	const KHOTKEYBINDING *GetBinding(int nIndex);
	int AddBinding(const KHOTKEYBINDING &Binding);

	int Enable(int nEnable);
	int IsEnable();

	int GetCaptureKey(int &nKey, int &nShift, int &nCtrl, int &nAlt);
private:
	int LoadBinding();
	int UnLoadBinding();
private:
	KHotKeyBindingArray m_aHotKeyBinding;
	KHotkeyMap m_Hotkey;

	int m_nHandleAllHotKey;
	int m_nEnable;

	int m_nCaptureKey;
	int m_nCaptureKeyShift;
	int m_nCaptureKeyCtrl;
	int m_nCaptureKeyAlt;
};

}

#endif //_INCLUDE_KHOTKEYMGR_H_
