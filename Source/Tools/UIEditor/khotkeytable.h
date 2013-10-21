#ifndef UI_DRIVER_HOTKEY_TABLE_H
#define UI_DRIVER_HOTKEY_TABLE_H

namespace UI
{
	class KHotKeyTable
	{
	public:
		enum { VK_LDBUTTON = 0x0100 };
		enum { VK_RDBUTTON = 0x0101 };
		enum { VK_MDBUTTON = 0x0102 };

		enum { HOTKEY_DESC_MAX_LEN = 32 };

	public:

		// @Param : dwHotkey 是Win32 Virtual Key Code 
		// @Return : 转换失败，返回_T("")
		LPCTSTR TranslateHotkeyToText(DWORD dwHotkey) const;

		// @Param : szHotkeyText 快捷键文字组合，长度小于HOTKEY_DESC_MAX_LEN，如_T("Shift+Space")
		// @Return : 转换失败，返回 0
		DWORD TranslateTextToHotkey(LPCTSTR szHotkeyText) const;

	private:
		mutable TCHAR m_szHotkeyDesc[HOTKEY_DESC_MAX_LEN];
	};

};

#endif // UI_DRIVER_HOTKEY_TABLE_H

