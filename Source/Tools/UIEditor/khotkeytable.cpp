#include "Stdafx.h"
#include "UiEditor.h"
#include "khotkeytable.h"

//---------------------------------------------------------------------------------------------------------------------
static LPCTSTR modidesc_table[] = 
{
	//	0		1		2		3		4		5		6		7
	_T("Shift"),	_T("Ctrl"),	_T("Alt"),	_T("Ext"),	_T(""),		_T(""),		_T(""),		_T("")
};

static LPCTSTR vkeydesc_table[] = 
{
	//	0					1						2					3						
	//  4					5						6					7
	//	8					9						A					B						
	//  C					D						E					F
	_T(""),					_T("LButton"),			_T("RButton"),		_T("Cancel"),			//  0 0
	_T("MButton"),			_T("XButton1"),			_T("XButton2"),		_T(""),					//  0 4
	_T("BackSpace"),		_T("Tab"),				_T(""),				_T(""),					//  0 8
	_T("Clear"),			_T("Enter"),			_T(""),				_T(""),             	//  0 C

	_T(""),					_T(""),					_T(""),				_T("Pause"),			//  1 0
	_T("CapLock"),			_T("Hanguel"),			_T(""),				_T("Junja "),			//  1 4
	_T("Final"),			_T("Kanji"),			_T(""),				_T("ESC"),				//  1 8
	_T("Convert"),			_T("NonConvert"),		_T("Accept"),		_T("ModeChange"),   	//  1 C

	_T("Space"),			_T("PageUp"),			_T("PageDown"),		_T("End"),				//  2 0
	_T("Home"),				_T("Left"),				_T("Up"),			_T("Right"),			//  2 4
	_T("Down"),				_T("Select"),			_T("Print"),		_T("Execute"),			//  2 8
	_T("PrintScreen"),		_T("Insert"),			_T("Delete"),		_T("Help"),         	//  2 C

	_T("0"),				_T("1"),				_T("2"),			_T("3"),				//  3 0
	_T("4"),				_T("5"),				_T("6"),			_T("7"),				//  3 4
	_T("8"),				_T("9"),				_T(""),				_T(""),					//  3 8
	_T(""),					_T(""),					_T(""),				_T(""),             	//  3 C

	_T(""),					_T("A"),				_T("B"),			_T("C"),				//  4 0
	_T("D"),				_T("E"),				_T("F"),			_T("G"),				//  4 4
	_T("H"),				_T("I"),				_T("J"),			_T("K"),				//  4 8
	_T("L"),				_T("M"),				_T("N"),			_T("O"),            	//  4 C

	_T("P"),				_T("Q"),				_T("R"),			_T("S"),				//  5 0 
	_T("T"),				_T("U"),				_T("V"),			_T("W"),				//  5 4 
	_T("X"),				_T("Y"),				_T("Z"),			_T("LWin"),				//  5 8 
	_T("RWin"),				_T("Apps"),				_T(""),				_T(""),             	//  5 C 

	_T("Num0"),				_T("Num1"),				_T("Num2"),			_T("Num3"),				//  6 0 
	_T("Num4"),				_T("Num5"),				_T("Num6"),			_T("Num7"),				//  6 4 
	_T("Num8"),				_T("Num9"),				_T("Num*"),			_T("Num+"),				//  6 8 
	_T("Separator"),		_T("Num-"),				_T("Num."),			_T("Num/"),         	//  6 C 

	_T("F1"),				_T("F2"),				_T("F3"),			_T("F4"),				//  7 0 
	_T("F5"),				_T("F6"),				_T("F7"),			_T("F8"),				//  7 4 
	_T("F9"),				_T("F10"),				_T("F11"),			_T("F12"),				//  7 8 
	_T("F13"),				_T("F14"),				_T("F15"),			_T("F16"),	        	//  7 C 

	_T("F17"),				_T("F18"),				_T("F19"),			_T("F20"),				//  8 0 
	_T("F21"),				_T("F22"),				_T("F23"),			_T("F24"),				//  8 4 
	_T(""),					_T(""),					_T(""),				_T(""),					//  8 8 
	_T(""),					_T(""),					_T(""),				_T(""),             	//  8 C 

	_T("NumLock"),			_T("ScrollLock"),		_T(""),				_T(""),					//  9 0 
	_T(""),					_T(""),					_T(""),				_T(""),					//  9 4 
	_T(""),					_T(""),					_T(""),				_T(""),					//  9 8 
	_T(""),					_T(""),					_T(""),				_T(""),					//  9 C 

	_T(""),					_T(""),					_T(""),				_T(""),					//  A 0 
	_T(""),					_T(""),					_T("BrowserBack"),	_T("BrowserForward"),	//  A 4 
	_T("BrowserRefresh"),	_T("BrowserStop"),		_T("BrowserSearch"),_T("BrowserFavorites"),	//  A 8 
	_T("BrowserHome"),		_T("VolumeMute"),		_T("VolumeDown"),	_T("VolumeUp"),			//  A C 

	_T("MediaNextTrack"),	_T("MediaPrevTrack"),	_T("MediaStop"),	_T("MediaPlayPause"),	//  B 0 
	_T("LaunchMail"),		_T("LaunchMediaSelect"),_T("LaunchApp1"),	_T("LaunchApp2"),		//  B 4 
	_T(""),					_T(""),					_T(";"),			_T("="),				//  B 8 
	_T(","),				_T("-"),				_T("."),			_T("/"),            	//  B C 

	_T("`"),				_T(""),					_T(""),				_T(""),					//  C 0 
	_T(""),					_T(""),					_T(""),				_T(""),					//  C 4 
	_T(""),					_T(""),					_T(""),				_T(""),					//  C 8 
	_T(""),					_T(""),					_T(""),				_T(""),             	//  C C 

	_T(""),					_T(""),					_T(""),				_T(""),					//  D 0 
	_T(""),					_T(""),					_T(""),				_T(""),					//  D 4 
	_T(""),					_T(""),					_T(""),				_T("["),				//  D 8 
	_T("\\"),				_T("]"),				_T("'"),			_T(""),             	//  D C 

	_T(""),					_T(""),					_T(""),				_T(""),					//  E 0 
	_T(""),					_T(""),					_T(""),				_T(""),					//  D 4 
	_T(""),					_T(""),					_T(""),				_T(""),					//  D 8 
	_T(""),					_T(""),					_T(""),				_T(""),	            	//  D C 

	_T(""),					_T(""),					_T(""),				_T(""),					//  F 0 
	_T(""),					_T(""),					_T(""),				_T(""),					//  F 4 
	_T(""),					_T(""),					_T(""),				_T(""),					//  F 8 
	_T(""),					_T(""),					_T(""),				_T(""),             	//  F C 

	_T("LDButton"),			_T("RDButton"),			_T("MDButton"), 	_T(""),					// 10 0 
	_T(""),					_T(""),					_T(""),				_T(""),					// 10 4 
	_T(""),					_T(""),					_T(""),				_T(""),					// 10 8 
	_T(""),					_T(""),					_T(""),				_T(""),					// 10 C 
};                                                                                                  
                                                                                                    
static size_t const count_moditbl = sizeof(modidesc_table) / sizeof(modidesc_table[0]);             
static size_t const count_vkeytbl = sizeof(vkeydesc_table) / sizeof(vkeydesc_table[0]);             
                                                                                                    
static TCHAR const STR_DELIMITER[] = _T(" + ");                                                     
static size_t const LEN_DELIMITER = sizeof(STR_DELIMITER) - sizeof(STR_DELIMITER[0]);               
                                                                                                    
                                                                                                    
//---------------------------------------------------------------------------------------------------------------------
static inline bool __x_isgraph(TCHAR c)                                                             
{                                                                                                   
	return c < 0 || _istgraph(c);
}



//-------------------------------------------------------------------------------------------------------------------------
namespace UI
{
	LPCTSTR KHotKeyTable::TranslateHotkeyToText(DWORD dwHotkey) const
	{
		m_szHotkeyDesc[0] = _T('\0');

		WORD const modi = HIWORD(dwHotkey);
		WORD const vkey = LOWORD(dwHotkey);
		if ((modi & 0xFF00) || (vkey >= count_vkeytbl))
			return m_szHotkeyDesc;

		LPCTSTR szVkDesc = vkeydesc_table[vkey];
		if (szVkDesc[0] == 0)
			return m_szHotkeyDesc;

		for (size_t pos = 0; pos <= count_moditbl; ++pos)
		{
			LPCTSTR szDesc = NULL;

			if (pos < count_moditbl)
			{
				if (!(modi & (0x01 << pos)))
					continue;
				szDesc = modidesc_table[pos];
				if (szDesc[0] == 0)
					return m_szHotkeyDesc;
			}
			else
			{
				szDesc = szVkDesc;
			}

			if (m_szHotkeyDesc[0])
				_tcscat(m_szHotkeyDesc, STR_DELIMITER);
			_tcscat(m_szHotkeyDesc, szDesc);
		}

		return m_szHotkeyDesc;
	}

	DWORD KHotKeyTable::TranslateTextToHotkey(LPCTSTR szHotkeyText) const
	{
		static const struct PATTERNMAP
		{
			enum {MASK_VKEY = 0x0000FFFF};

			typedef std::pair<DWORD, DWORD>	HOTKEYPART;
			typedef std::map<_tstring, HOTKEYPART, _tstring_iless>	DESC2HKPMAP;
			DESC2HKPMAP theMap;

			PATTERNMAP()
			{
				const struct _PATTERN
				{
					LPCTSTR desc;
					DWORD mask;
					DWORD value;
				} 
				pattern_table[] = 
				{
					//modifier
					{ _T("Shift"), 				HOTKEYF_SHIFT	<< 16, HOTKEYF_SHIFT 	<< 16 }, 
					{ _T("Control"),			HOTKEYF_CONTROL	<< 16, HOTKEYF_CONTROL	<< 16 },
					{ _T("Alt"),				HOTKEYF_ALT		<< 16, HOTKEYF_ALT 		<< 16 }, 
					{ _T("Ext"),				HOTKEYF_EXT		<< 16, HOTKEYF_EXT 		<< 16 },

					//vk
					{ _T("LButton"),			MASK_VKEY, VK_LBUTTON }, 
					{ _T("RButton"), 			MASK_VKEY, VK_RBUTTON },
					{ _T("Cancel"), 			MASK_VKEY, VK_CANCEL }, 
					{ _T("MButton"),			MASK_VKEY, VK_MBUTTON },
					{ _T("BackSpace"), 			MASK_VKEY, VK_BACK }, 
					{ _T("Tab"),				MASK_VKEY, VK_TAB }, 
					{ _T("Clear"), 				MASK_VKEY, VK_CLEAR },
					{ _T("Return"), 			MASK_VKEY, VK_RETURN }, 
					{ _T("Pause"), 				MASK_VKEY, VK_PAUSE },
					{ _T("Convert"),			MASK_VKEY, VK_CONVERT }, 
					{ _T("NonConvert"), 		MASK_VKEY, VK_NONCONVERT },
					{ _T("Accept"), 			MASK_VKEY, VK_ACCEPT }, 
					{ _T("ModeChange"), 		MASK_VKEY, VK_MODECHANGE },
					{ _T("Escape"), 			MASK_VKEY, VK_ESCAPE }, 
					{ _T("Space"), 				MASK_VKEY, VK_SPACE },
					{ _T("Prior"), 				MASK_VKEY, VK_PRIOR }, 
					{ _T("Next"), 				MASK_VKEY, VK_NEXT }, 
					{ _T("End"),				MASK_VKEY, VK_END }, 
					{ _T("Home"), 				MASK_VKEY, VK_HOME },
					{ _T("Left"), 				MASK_VKEY, VK_LEFT }, 
					{ _T("Up"), 				MASK_VKEY, VK_UP }, 
					{ _T("Right"), 				MASK_VKEY, VK_RIGHT }, 
					{ _T("Down"), 				MASK_VKEY, VK_DOWN },
					{ _T("Insert"), 			MASK_VKEY, VK_INSERT }, 
					{ _T("Delete"), 			MASK_VKEY, VK_DELETE },
					{ _T("Select"), 			MASK_VKEY, VK_SELECT }, 
					{ _T("Print"), 				MASK_VKEY, VK_PRINT }, 
					{ _T("Execute"), 			MASK_VKEY, VK_EXECUTE },
					{ _T("SnapShot"), 			MASK_VKEY, VK_SNAPSHOT }, 
					{ _T("Help"), 				MASK_VKEY, VK_HELP },
					{ _T("0"), 					MASK_VKEY, _T('0') }, 
					{ _T("1"), 					MASK_VKEY, _T('1') }, 
					{ _T("2"), 					MASK_VKEY, _T('2') }, 
					{ _T("3"), 					MASK_VKEY, _T('3') },
					{ _T("4"), 					MASK_VKEY, _T('4') }, 
					{ _T("5"), 					MASK_VKEY, _T('5') }, 
					{ _T("6"), 					MASK_VKEY, _T('6') }, 
					{ _T("7"), 					MASK_VKEY, _T('7') },
					{ _T("8"), 					MASK_VKEY, _T('8') },
					{ _T("9"), 					MASK_VKEY, _T('9') },
					{ _T("A"), 					MASK_VKEY, _T('A') }, 
					{ _T("B"), 					MASK_VKEY, _T('B') }, 
					{ _T("C"), 					MASK_VKEY, _T('C') }, 
					{ _T("D"), 					MASK_VKEY, _T('D') },
					{ _T("E"), 					MASK_VKEY, _T('E') }, 
					{ _T("F"), 					MASK_VKEY, _T('F') }, 
					{ _T("G"), 					MASK_VKEY, _T('G') }, 
					{ _T("H"), 					MASK_VKEY, _T('H') },
					{ _T("I"), 					MASK_VKEY, _T('I') }, 
					{ _T("J"), 					MASK_VKEY, _T('J') }, 
					{ _T("K"), 					MASK_VKEY, _T('K') }, 
					{ _T("L"), 					MASK_VKEY, _T('L') },
					{ _T("M"), 					MASK_VKEY, _T('M') }, 
					{ _T("N"), 					MASK_VKEY, _T('N') }, 
					{ _T("O"), 					MASK_VKEY, _T('O') }, 
					{ _T("P"), 					MASK_VKEY, _T('P') },
					{ _T("Q"), 					MASK_VKEY, _T('Q') }, 
					{ _T("R"), 					MASK_VKEY, _T('R') }, 
					{ _T("S"), 					MASK_VKEY, _T('S') }, 
					{ _T("T"), 					MASK_VKEY, _T('T') },
					{ _T("U"), 					MASK_VKEY, _T('U') }, 
					{ _T("V"), 					MASK_VKEY, _T('V') }, 
					{ _T("W"), 					MASK_VKEY, _T('W') }, 
					{ _T("X"), 					MASK_VKEY, _T('X') },
					{ _T("Y"), 					MASK_VKEY, _T('Y') }, 
					{ _T("Z"), 					MASK_VKEY, _T('Z') },
					{ _T("Num0"), 				MASK_VKEY, VK_NUMPAD0 }, 
					{ _T("Num1"), 				MASK_VKEY, VK_NUMPAD1 }, 
					{ _T("Num2"), 				MASK_VKEY, VK_NUMPAD2 }, 
					{ _T("Num3"), 				MASK_VKEY, VK_NUMPAD3 },
					{ _T("Num4"), 				MASK_VKEY, VK_NUMPAD4 }, 
					{ _T("Num5"), 				MASK_VKEY, VK_NUMPAD5 }, 
					{ _T("Num6"), 				MASK_VKEY, VK_NUMPAD6 }, 
					{ _T("Num7"), 				MASK_VKEY, VK_NUMPAD7 },
					{ _T("Num8"), 				MASK_VKEY, VK_NUMPAD8 }, 
					{ _T("Num9"), 				MASK_VKEY, VK_NUMPAD9 },
					{ _T("Num+"), 				MASK_VKEY, VK_ADD }, 
					{ _T("Num-"), 				MASK_VKEY, VK_SUBTRACT }, 
					{ _T("Num*"), 				MASK_VKEY, VK_MULTIPLY }, 
					{ _T("Num/"), 				MASK_VKEY, VK_DIVIDE },
					{ _T("Separator"), 			MASK_VKEY, VK_SEPARATOR }, 
					{ _T("Num."), 				MASK_VKEY, VK_DECIMAL },
					{ _T("F1"), 				MASK_VKEY, VK_F1 }, 
					{ _T("F2"), 				MASK_VKEY, VK_F2 }, 
					{ _T("F3"), 				MASK_VKEY, VK_F3 }, 
					{ _T("F4"), 				MASK_VKEY, VK_F4 },
					{ _T("F5"), 				MASK_VKEY, VK_F5 }, 
					{ _T("F6"), 				MASK_VKEY, VK_F6 }, 
					{ _T("F7"), 				MASK_VKEY, VK_F7 }, 
					{ _T("F8"), 				MASK_VKEY, VK_F8 },
					{ _T("F9"), 				MASK_VKEY, VK_F9 }, 
					{ _T("F10"), 				MASK_VKEY, VK_F10 }, 
					{ _T("F11"), 				MASK_VKEY, VK_F11 }, 
					{ _T("F12"), 				MASK_VKEY, VK_F12 },
					{ _T("F13"), 				MASK_VKEY, VK_F13 }, 
					{ _T("F14"), 				MASK_VKEY, VK_F14 }, 
					{ _T("F15"), 				MASK_VKEY, VK_F15 }, 
					{ _T("F16"), 				MASK_VKEY, VK_F16 },
					{ _T("F17"), 				MASK_VKEY, VK_F17 }, 
					{ _T("F18"), 				MASK_VKEY, VK_F18 }, 
					{ _T("F19"), 				MASK_VKEY, VK_F19 }, 
					{ _T("F20"), 				MASK_VKEY, VK_F20 },
					{ _T("F21"), 				MASK_VKEY, VK_F21 }, 
					{ _T("F22"), 				MASK_VKEY, VK_F22 }, 
					{ _T("F23"), 				MASK_VKEY, VK_F23 }, 
					{ _T("F24"), 				MASK_VKEY, VK_F24 },
					{ _T("CapLock"), 			MASK_VKEY, VK_CAPITAL }, 
					{ _T("NumLock"), 			MASK_VKEY, VK_NUMLOCK }, 
					{ _T("ScrollLock"), 		MASK_VKEY, VK_SCROLL },
					{ _T(";"), 					MASK_VKEY, 0x00BA },	
					{ _T("="), 					MASK_VKEY, 0x00BB },	
					{ _T(","), 					MASK_VKEY, 0x00BC },	
					{ _T("-"), 					MASK_VKEY, 0x00BD },
					{ _T("."), 					MASK_VKEY, 0x00BE },	
					{ _T("/"), 					MASK_VKEY, 0x00BF },	
					{ _T("`"), 					MASK_VKEY, 0x00C0 },
					{ _T("["), 					MASK_VKEY, 0x00DB },
					{ _T("\\"), 				MASK_VKEY, 0x00DC }, 
					{ _T("]"), 					MASK_VKEY, 0x00DD }, 
					{ _T("'"), 					MASK_VKEY, 0x00DE },
					{ _T("Kana"), 				MASK_VKEY, VK_KANA },

#if(_WIN32_WINNT >= 0x0500)
					{ _T("XButton1"),			MASK_VKEY, VK_XBUTTON1 },
					{ _T("XButton2"), 			MASK_VKEY, VK_XBUTTON2 },
					{ _T("Hanguel"), 			MASK_VKEY, VK_HANGUEL },
					{ _T("Hangul"), 			MASK_VKEY, VK_HANGUL },
					{ _T("Junja"), 				MASK_VKEY, VK_JUNJA },
					{ _T("Final"), 				MASK_VKEY, VK_FINAL },
					{ _T("Hanja"), 				MASK_VKEY, VK_HANJA },
					{ _T("Kanji"), 				MASK_VKEY, VK_KANJI },
					{ _T("LWin"), 				MASK_VKEY, VK_LWIN },
					{ _T("RWin"), 				MASK_VKEY, VK_RWIN },
					{ _T("Apps"), 				MASK_VKEY, VK_APPS },
					{ _T("BrowserBack"), 		MASK_VKEY, VK_BROWSER_BACK },
					{ _T("BrowserForward"), 	MASK_VKEY, VK_BROWSER_FORWARD },
					{ _T("BrowserRefresh"), 	MASK_VKEY, VK_BROWSER_REFRESH },
					{ _T("BrowserStop"), 		MASK_VKEY, VK_BROWSER_STOP },
					{ _T("BrowserSearch"), 		MASK_VKEY, VK_BROWSER_SEARCH },
					{ _T("BrowserFavorites"), 	MASK_VKEY, VK_BROWSER_FAVORITES },
					{ _T("BrowserHome"), 		MASK_VKEY, VK_BROWSER_HOME },
					{ _T("VolumeMute"), 		MASK_VKEY, VK_VOLUME_MUTE },
					{ _T("VolumeDown"), 		MASK_VKEY, VK_VOLUME_DOWN },
					{ _T("VolumeUp"), 			MASK_VKEY, VK_VOLUME_UP },
					{ _T("MediaNextTrack"), 	MASK_VKEY, VK_MEDIA_NEXT_TRACK },
					{ _T("MediaPrevTrack"), 	MASK_VKEY, VK_MEDIA_PREV_TRACK },
					{ _T("MediaStop"), 			MASK_VKEY, VK_MEDIA_STOP },
					{ _T("MediaPlayPause"), 	MASK_VKEY, VK_MEDIA_PLAY_PAUSE },
					{ _T("LaunchMail"), 		MASK_VKEY, VK_LAUNCH_MAIL },
					{ _T("LaunchMediaSelect"), 	MASK_VKEY, VK_LAUNCH_MEDIA_SELECT },
					{ _T("LaunchApp1"), 		MASK_VKEY, VK_LAUNCH_APP1 },
					{ _T("LaunchApp2"), 		MASK_VKEY, VK_LAUNCH_APP2 },
#endif // #if(_WIN32_WINNT >= 0x0500)

					//modifier alias
					{ _T("Ctrl"), 				HOTKEYF_CONTROL	<<16, 	HOTKEYF_CONTROL	<<16 }, 
					{ _T("Menu"), 				HOTKEYF_ALT		<<16, 	HOTKEYF_ALT		<<16 },

					//vk alias
					{ _T("Break"), 				MASK_VKEY, VK_PAUSE },
					{ _T("ESC"), 				MASK_VKEY, VK_ESCAPE }, 
					{ _T("Enter"), 				MASK_VKEY, VK_RETURN },
					{ _T("BACK"), 				MASK_VKEY, VK_BACK },
					{ _T("INS"), 				MASK_VKEY, VK_INSERT }, 
					{ _T("DEL"), 				MASK_VKEY, VK_DELETE },
					{ _T("PageUp"), 			MASK_VKEY, VK_PRIOR }, 
					{ _T("PageDown"), 			MASK_VKEY, VK_NEXT },
					{ _T("ScrlLock"), 			MASK_VKEY, VK_SCROLL },
					{ _T("NumAdd"), 			MASK_VKEY, VK_ADD }, 
					{ _T("NumSub"), 			MASK_VKEY, VK_SUBTRACT }, 
					{ _T("NumMul"), 			MASK_VKEY, VK_MULTIPLY }, 
					{ _T("NumDiv"), 			MASK_VKEY, VK_DIVIDE },
					{ _T("NumDecimal"), 		MASK_VKEY, VK_DECIMAL },
					{ _T("PrintScreen"), 		MASK_VKEY, VK_SNAPSHOT },

					{ _T("LDButton"), 			MASK_VKEY, VK_LDBUTTON }, 
					{ _T("RDButton"), 			MASK_VKEY, VK_RDBUTTON }, 
					{ _T("MDButton"), 			MASK_VKEY, VK_MDBUTTON }, 
				};

				for (size_t i = 0; i < sizeof(pattern_table)/sizeof(pattern_table[0]); ++i)
				{
					_PATTERN const& pat = pattern_table[i];
					theMap[_tstring(pat.desc)] = std::make_pair(pat.mask, pat.value);
				}
			}
		} s_mapPattern;

		static TCHAR const CH_DELIMITER = _T('+');

		if (szHotkeyText[0] == _T('\0'))
			return 0;

		DWORD hkcode = 0;

		LPCTSTR szToken = szHotkeyText, szLimit = NULL, szNext = NULL;
		for ( ; *szToken; szToken = szNext)
		{
			for (szNext = NULL, szLimit = szToken; *szLimit; szLimit++)
			{
				if (*szLimit == CH_DELIMITER)
				{
					for (szNext = szLimit + 1; ; szNext++)
					{
						if (!*szNext)
						{
							szLimit ++;
							break;
						}
						if (*szNext == CH_DELIMITER)
							szLimit ++;
						else if (__x_isgraph(*szNext))
							break;
					}
					if (szLimit <= szToken)
						return 0;
					break;
				}
			}


			while (!__x_isgraph(*szToken))
			{
				szToken ++;
				if (szToken >= szLimit)
					return 0;
			}
			LPCTSTR pe = szLimit - 1;
			while (!__x_isgraph(*pe))
				pe --;
			size_t toklen = pe - szToken + 1;

			PATTERNMAP::DESC2HKPMAP::const_iterator it = s_mapPattern.theMap.find(_tstring(szToken, toklen));
			if (it == s_mapPattern.theMap.end())
				return 0;
			const PATTERNMAP::HOTKEYPART& hkp = (*it).second;

			if (hkcode & hkp.first)
				return 0;
			hkcode |= hkp.second;

			if (szNext == NULL)
				break;
		}

		if (!(hkcode & PATTERNMAP::MASK_VKEY))
			return 0;

		return hkcode;
	}

}
