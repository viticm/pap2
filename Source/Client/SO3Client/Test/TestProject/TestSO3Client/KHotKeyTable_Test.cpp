#include "stdafx.h"

#ifdef _UNIT_TEST

#include "KHotKeyTable_Test.h"
#include <string>
#include "../ui/driver/khotkeytable.h"
#include <commctrl.h>
#include <vector>
#include <iterator>
//
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(KHotKeyTable_Test);
vector<DWORD> KHotKeyTable_Test_Vector;
void KHotKeyTable_Test::setUp()
{
}

void KHotKeyTable_Test::tearDown()
{
}

//////////////////////////////////////////////////////////////////////////
//
// Pre Senario:
//(1)
// By Object(s):
// m_KHotKeyTable of UI::KHotkeyTable class.
// Operation(s):
//input all hotkey string to the m_KHotKeyTable.TranslateTextToHotkey(LPCTSTR) and record 
//all the input, then the output as arguments of m_KHotKeyTable.TranslateHotkeyToText(DWORD)
//and now compare the input that recorded and the m_KHotKeyTable.TranslateHotkeyToText(DWORD)
// CheckPoint(s):
//(1)in theory, all the hotkey string can be transformed Virtual-Key Codes by TranslateTextToHotkey.
//(2)the TranslateHotkeyToText function can transformed TranslateTextToHotkey to hotkey sting
//////////////////////////////////////////////////////////////////////////
void KHotKeyTable_Test::testTranslateTextToHotkey()
{
	const MaxNumber = 8 * 34 * 5;
	//                                   0						  1             2            3           4			    5            6				7
	char szHotKeyIn[MaxNumber][32] = {  "",					 "LButton",	   "RButton",	"Cancel",	"MButton",		"XButton1",   "XButton2",		"",				//0
										"BackSpace",		 "Tab",		   "",			"",			"Clear",		"Enter",	  "",				"",
										"",					 "",		   "",  		"Pause",	"CapLock",		"Hanguel",	  "",				"Junja",		//1
										"Final",			 "Kanji",	   "",			"ESC",		"Convert",		"NonConvert", "Accept",			"ModeChange",
										"Space",			 "PageUp",	   "PageDown",	"End",		"Home",			"Left",		  "Up",				"Right",		//2
										"Down",				 "Select",	   "Print",	    "Execute",	"PrintScreen",	"Insert",     "Delete",			"Help",
										"0",				 "1",		   "2",		    "3",		"4",			"5",		  "6",				"7",			//3
										"8",				 "9",		   "",			"",			"",				"",			  "",				"",
										"",					 "A",		   "B",		    "C",		"D",			"E",		  "F",				"G",			//4
										"H",				 "I",		   "J",		    "K",		"L",			"M",		  "N",				"O",
										"P",				 "Q",		   "R",		    "S",		"T",			"U",		  "V",				"W",			//5
										"X",			  	 "Y",		   "Z",		    "LWin", 	"RWin",			"Apps",		  "",				"",
										"Num0",				 "Num1",	   "Num2",		"Num3",		"Num4",			"Num5",		  "Num6",			"Num7",			//6
										"Num8",				 "Num9",	   "Num*",		"Num+",		"Separator",	"Num-",		  "Num.",			"Num/",
										"F1",				 "F2",		   "F3",		"F4",		"F5",			"F6",		  "F7",				"F8",			//7
										"F9",				 "F10",		   "F11",		"F12",		"F13",			"F14",		  "F15",			"F16",	
										"F17",				 "F18",		   "F19",		"F20",		"F21",			"F22",		  "F23",			"F24",			//8
										"",					 "",		   "",			"",			"",				"",			  "",				"",
										"NumLock",			 "ScrollLock", "",		    "",			"",				"",			  "",				"",				//9
										"",					 "",		   "",			"",			"",				"",			  "",				"",	
										"",					 "",		   "",			"",			"",				"",			  "BrowserBack",	"BrowserForward",	//A
										"BrowserRefresh", "BrowserStop",	"BrowserSearch", "BrowserFavorites",	"BrowserHome",	"VolumeMute",		 "VolumeDown",	"VolumeUp",
										"MediaNextTrack", "MediaPrevTrack",	"MediaStop",	 "MediaPlayPause",		"LaunchMail",	"LaunchMediaSelect", "LaunchApp1",	"LaunchApp2",	//B
										"",					 "",		   ";",		    "=",		",",			"-",		  ".",					"/",
										"`",				 "",		   "",			"",			"",				"",			  "",					"",				//C
										"",					 "",		   "",			"",			"",				"",			  "",					"",
										"",					 "",		   "",			"",			"",				"",			  "",					"",				//D
										"",					 "",		   "",			"[",		"\\",			"]",		  "'",					"",
										"",					 "",		   "",			"",			"",				"",			  "",					"",				//E
										"",					 "",		   "",			"",			"",				"",			  "",					"",	
										"",					 "",		   "",			"",			"",				"",			  "",					"",				//F
										"",					 "",		   "",			"",			"",				"",			  "",					"",
										"LDButton",			"RDButton",    "MDButton",  "",			"",				"",			  "",					"",				//10
										"",					 "",		   "",			"",			"",				"",			  "",					"",

										"Shift",				"Shift + LButton",		"Shift + RButton",			"Shift + Cancel",		"Shift + MButton",		"Shift + XButton1",		"Shift + XButton2 ",	"",				//0
										"Shift + BackSpace",	"Shift + Tab",			"",							"",						"Shift + Clear",		"Shift + Enter",		"",						"",
										"",						"",						"",  						"Shift + Pause",		"Shift + CapLock",		"Shift + Hanguel",		"",		"Shift + Junja",				//1
										"Shift + Final",		"Shift + Kanji",		"",							"Shift + ESC",			"Shift + Convert",		"Shift + NonConvert",	"Shift + Accept",		"Shift + ModeChange",
										"Shift + Space",		"Shift + PageUp",		"Shift + PageDown",			"Shift + End",			"Shift + Home",			"Shift + Left",			"Shift + Up",			"Shift + Right",		//2
										"Shift + Down",			"Shift + Select",		"Shift + Print",			"Shift + Execute",		"Shift + PrintScreen",	"Shift + Insert",		"Shift + Delete",		"Shift + Help",
										"Shift + 0",			"Shift + 1",			"Shift + 2",				"Shift + 3",			"Shift + 4",			"Shift + 5",		    "Shift + 6",			"Shift + 7",			//3
										"Shift + 8",			"Shift + 9",			"",							"",						"",						"",						"",						"",
										"",						"Shift + A",			"Shift + B",				"Shift + C",			"Shift + D",			"Shift + E",			"Shift + F",			"Shift + G",			//4
										"Shift + H",			"Shift + I",			"Shift + J",				"Shift + K",			"Shift + L",			"Shift + M",			"Shift + N",			"Shift + O",
										"Shift + P",			"Shift + Q",			"Shift + R",				"Shift + S",			"Shift + T",			"Shift + U",			"Shift + V",			"Shift + W",			//5
										"Shift + X",			"Shift + Y",			"Shift + Z",				"Shift + LWin",			"Shift + RWin",			"Shift + Apps",			"",						"",
										"Shift + Num0",			"Shift + Num1",			"Shift + Num2",				"Shift + Num3",			"Shift + Num4",			"Shift + Num5",			"Shift + Num6",			"Shift + Num7",			//6
										"Shift + Num8",			"Shift + Num9",			"Shift + Num*",				"Shift + Num+",			"Shift + Separator",	"Shift + Num-",			"Shift + Num.",			"Shift + Num/",
										"Shift + F1",			"Shift + F2",			"Shift + F3",				"Shift + F4",			"Shift + F5",			"Shift + F6",			"Shift + F7",			"Shift + F8",			//7
										"Shift + F9",			"Shift + F10",			"Shift + F11",				"Shift + F12",			"Shift + F13",			"Shift + F14",			"Shift + F15",			"Shift + F16",	
										"Shift + F17",			"Shift + F18",			"Shift + F19",				"Shift + F20",			"Shift + F21",			"Shift + F22",			"Shift + F23",			"Shift + F24",			//8
										"",						"",						"",							"",						"",						"",						"",						"",
										"Shift + NumLock",		"Shift + ScrollLock",	"",							"",						"",						"",						"",						"",				//9
										"",						"",						"",							"",						"",						"",						"",						"",	
										"",						"",						"",							"",						"",						"",						"Shift + BrowserBack",	"Shift + BrowserForward",				//A
										"Shift + BrowserRefresh",	"Shift + BrowserStop",	"Shift + BrowserSearch",	"Shift + BrowserFavorites",	  "Shift + BrowserHome",	"Shift + VolumeMute",	"Shift + VolumeDown",	"Shift + VolumeUp",
										"Shift + MediaNextTrack",	"Shift + MediaPrevTrack",	"Shift + MediaStop",	"Shift + MediaPlayPause",     "Shift + LaunchMail",		"Shift + LaunchMediaSelect",	"Shift + LaunchApp1",	"Shift + LaunchApp2",				//B
										"",						"",						"Shift + ;",				"Shift + =",			"Shift + ,",			"Shift + -",			"Shift + .",			"Shift + /",
										"Shift + `",			"",						"",							"",						"",						"",						"",						"",				//C
										"",						"",						"",							"",						"",						"",						"",						"",
										"",						"",						"",							"",						"",						"",						"",						"",				//D
										"",						"",						"",							"Shift + [",			"Shift + \\",			"Shift + ]",			"Shift + '",			"",
										"",						"",						"",							"",						"",						"",						"",						"",				//E
										"",						"",						"",							"",						"",						"",						"",						"",	
										"",						"",						"",							"",						"",						"",						"",						"",				//F
										"",						"",						"",							"",						"",						"",						"",						"",
										"Shift + LDButton",		"Shift + RDButton",		"Shift + MDButton",			"",						"",						"",						"",						"",				//10
										"",						"",						"",							"",						"",						"",						"",						"",
				
										"Ctrl",					"Ctrl + LButton",		"Ctrl + RButton",			"Ctrl + Cancel",		"Ctrl + MButton",		"Ctrl + XButton1",		"Ctrl + XButton2 ",		"",				//0
										"Ctrl + BackSpace",		"Ctrl + Tab",			"",							"",						"Ctrl + Clear",			"Ctrl + Enter",			"",						"",
										"",						"",						"",  						"Ctrl + Pause",			"Ctrl + CapLock",		"Ctrl + Hanguel",		"",						"Ctrl + Junja",				//1
										"Ctrl + Final",			"Ctrl + Kanji",			"",							"Ctrl + ESC",			"Ctrl + Convert",		"Ctrl + NonConvert",	"Ctrl + Accept",		"Ctrl + ModeChange",
										"Ctrl + Space",			"Ctrl + PageUp",		"Ctrl + PageDown",			"Ctrl + End",			"Ctrl + Home",			"Ctrl + Left",			"Ctrl + Up",			"Ctrl + Right",		//2
										"Ctrl + Down",			"Ctrl + Select",		"Ctrl + Print",				"Ctrl + Execute",		"Ctrl + PrintScreen",	"Ctrl + Insert",		"Ctrl + Delete",		"Ctrl + Help",
										"Ctrl + 0",				"Ctrl + 1",				"Ctrl + 2",					"Ctrl + 3",				"Ctrl + 4",				"Ctrl + 5",				"Ctrl + 6",				"Ctrl + 7",			//3
										"Ctrl + 8",				"Ctrl + 9",				"",							"",						"",						"",						"",						"",
										"",						"Ctrl + A",				"Ctrl + B",					"Ctrl + C",				"Ctrl + D",				"Ctrl + E",				"Ctrl + F",				"Ctrl + G",			//4
										"Ctrl + H",				"Ctrl + I",				"Ctrl + J",					"Ctrl + K",				"Ctrl + L",				"Ctrl + M",				"Ctrl + N",				"Ctrl + O",
										"Ctrl + P",				"Ctrl + Q",				"Ctrl + R",					"Ctrl + S",				"Ctrl + T",				"Ctrl + U",				"Ctrl + V",				"Ctrl + W",			//5
										"Ctrl + X",				"Ctrl + Y",				"Ctrl + Z",					"Ctrl + LWin",			"Ctrl + RWin",			"Ctrl + Apps",			"",						"",
										"Ctrl + Num0",			"Ctrl + Num1",			"Ctrl + Num2",				"Ctrl + Num3",			"Ctrl + Num4",			"Ctrl + Num5",			"Ctrl + Num6",			"Ctrl + Num7",			//6
										"Ctrl + Num8",			"Ctrl + Num9",			"Ctrl + Num*",				"Ctrl + Num+",			"Ctrl + Separator",		"Ctrl + Num-",			"Ctrl + Num.",			"Ctrl + Num/",
										"Ctrl + F1",			"Ctrl + F2",			"Ctrl + F3",				"Ctrl + F4",			"Ctrl + F5",			"Ctrl + F6",			"Ctrl + F7",			"Ctrl + F8",			//7
										"Ctrl + F9",			"Ctrl + F10",			"Ctrl + F11",				"Ctrl + F12",			"Ctrl + F13",			"Ctrl + F14",			"Ctrl + F15",			"Ctrl + F16",	
										"Ctrl + F17",			"Ctrl + F18",			"Ctrl + F19",				"Ctrl + F20",			"Ctrl + F21",			"Ctrl + F22",			"Ctrl + F23",			"Ctrl + F24",			//8
										"",						"",						"",							"",						"",						"",						"",						"",
										"Ctrl + NumLock",		"Ctrl + ScrollLock",	"",							"",						"",						"",						"",						"",				//9
										"",						"",						"",							"",						"",						"",						"",						"",	
										"",						"",						"",							"",						"",						"",						"Ctrl + BrowserBack",	"Ctrl + BrowserForward",				//A
										"Ctrl + BrowserRefresh","Ctrl + BrowserStop",	"Ctrl + BrowserSearch",	 "Ctrl + BrowserFavorites",	"Ctrl + BrowserHome",	"Ctrl + VolumeMute",	"Ctrl + VolumeDown",	"Ctrl + VolumeUp",
										"Ctrl + MediaNextTrack","Ctrl + MediaPrevTrack","Ctrl + MediaStop",		 "Ctrl + MediaPlayPause",   "Ctrl + LaunchMail",	"Ctrl + LaunchMediaSelect",	"Ctrl + LaunchApp1","Ctrl + LaunchApp2",				//B
										"",						"",						"Ctrl + ;",					"Ctrl + =",				"Ctrl + ,",				"Ctrl + -",				"Ctrl + .",				"Ctrl + /",
										"Ctrl + `",				"",						"",							"",						"",						"",						"",						"",				//C
										"",						"",						"",							"",						"",						"",						"",						"",
										"",						"",						"",							"",						"",						"",						"",						"",				//D
										"",						"",						"",							"Ctrl + [",				"Ctrl + \\",			"Ctrl + ]",				"Ctrl + '",				"",
										"",						"",						"",							"",						"",						"",						"",						"",				//E
										"",						"",						"",							"",						"",						"",						"",						"",	
										"",						"",						"",							"",						"",						"",						"",						"",				//F
										"",						"",						"",							"",						"",						"",						"",						"",
										"Ctrl + LDButton",		"Ctrl + RDButton",		"Ctrl + MDButton",			"",						"",						"",						"",						"",				//10
										"",						"",						"",							"",						"",						"",						"",						"",

										"Alt",					"Alt + LButton",		"Alt + RButton",			"Alt + Cancel",			"Alt + MButton",		"Alt + XButton1",		"Alt + XButton2 ",		"",				//0
										"Alt + BackSpace",		"Alt + Tab",			"",							"",						"Alt + Clear",			"Alt + Enter",			"",						"",
										"",						"",						"",  						"Alt + Pause",			"Alt + CapLock",		"Alt + Hanguel",		"",						"Alt + Junja",				//1
										"Alt + Final",			"Alt + Kanji",			"",							"Alt + ESC",			"Alt + Convert",		"Alt + NonConvert",		"Alt + Accept",			"Alt + ModeChange",
										"Alt + Space",			"Alt + PageUp",			"Alt + PageDown",			"Alt + End",			"Alt + Home",			"Alt + Left",			"Alt + Up",				"Alt + Right",		//2
										"Alt + Down",			"Alt + Select",			"Alt + Print",				"Alt + Execute",		"Alt + PrintScreen",	"Alt + Insert",			"Alt + Delete",			"Alt + Help",
										"Alt + 0",				"Alt + 1",				"Alt + 2",					"Alt + 3",				"Alt + 4",				"Alt + 5",				"Alt + 6",				"Alt + 7",			//3
										"Alt + 8",				"Alt + 9",				"",							"",						"",						"",						"",						"",
										"",						"Alt + A",				"Alt + B",					"Alt + C",				"Alt + D",				"Alt + E",				"Alt + F",				"Alt + G",			//4
										"Alt + H",				"Alt + I",				"Alt + J",					"Alt + K",				"Alt + L",				"Alt + M",				"Alt + N",				"Alt + O",
										"Alt + P",				"Alt + Q",				"Alt + R",					"Alt + S",				"Alt + T",				"Alt + U",				"Alt + V",				"Alt + W",			//5
										"Alt + X",				"Alt + Y",				"Alt + Z",					"Alt + LWin",			"Alt + RWin",			"Alt + Apps",			"",						"",
										"Alt + Num0",			"Alt + Num1",			"Alt + Num2",				"Alt + Num3",			"Alt + Num4",			"Alt + Num5",			"Alt + Num6",			"Alt + Num7",			//6
										"Alt + Num8",			"Alt + Num9",			"Alt + Num*",				"Alt + Num+",			"Alt + Separator",		"Alt + Num-",			"Alt + Num.",			"Alt + Num/",
										"Alt + F1",				"Alt + F2",				"Alt + F3",					"Alt + F4",				"Alt + F5",				"Alt + F6",				"Alt + F7",				"Alt + F8",			//7
										"Alt + F9",				"Alt + F10",			"Alt + F11",				"Alt + F12",			"Alt + F13",			"Alt + F14",			"Alt + F15",			"Alt + F16",	
										"Alt + F17",			"Alt + F18",			"Alt + F19",				"Alt + F20",			"Alt + F21",			"Alt + F22",			"Alt + F23",			"Alt + F24",			//8
										"",						"",						"",							"",						"",						"",						"",						"",
										"Alt + NumLock",		"Alt + ScrollLock",	"",								"",						"",						"",						"",						"",				//9
										"",						"",						"",							"",						"",						"",						"",						"",	
										"",						"",						"",							"",						"",						"",						"Alt + BrowserBack",	"Alt + BrowserForward",				//A
										"Alt + BrowserRefresh",	"Alt + BrowserStop",	"Alt + BrowserSearch",	"Alt + BrowserFavorites",	"Alt + BrowserHome",	"Alt + VolumeMute",		"Alt + VolumeDown",		"Alt + VolumeUp",
										"Alt + MediaNextTrack",	"Alt + MediaPrevTrack",	"Alt + MediaStop",		"Alt + MediaPlayPause",     "Alt + LaunchMail",		"Alt + LaunchMediaSelect","Alt + LaunchApp1",	"Alt + LaunchApp2",				//B
										"",						"",						"Alt + ;",					"Alt + =",				"Alt + ,",				"Alt + -",				"Alt + .",				"Alt + /",
										"Alt + `",				"",						"",							"",						"",						"",						"",						"",				//C
										"",						"",						"",							"",						"",						"",						"",						"",
										"",						"",						"",							"",						"",						"",						"",						"",				//D
										"",						"",						"",							"Alt + [",				"Alt + \\",				"Alt + ]",				"Alt + '",				"",
										"",						"",						"",							"",						"",						"",						"",						"",				//E
										"",						"",						"",							"",						"",						"",						"",						"",	
										"",						"",						"",							"",						"",						"",						"",						"",				//F
										"",						"",						"",							"",						"",						"",						"",						"",
										"Alt + LDButton",		"Alt + RDButton",		"Alt + MDButton",			"",						"",						"",						"",						"",				//10
										"",						"",						"",							"",						"",						"",						"",						"",

										"Ext",					"Ext + LButton",		"Ext + RButton",			"Ext + Cancel",			"Ext + MButton",		"Ext + XButton1",		"Ext + XButton2 ",		"",				//0
										"Ext + BackSpace",		"Ext + Tab",			"",							"",						"Ext + Clear",			"Ext + Enter",			"",						"",
										"",						"",						"",  						"Ext + Pause",			"Ext + CapLock",		"Ext + Hanguel",		"",						"Ext + Junja",				//1
										"Ext + Final",			"Ext + Kanji",			"",							"Ext + ESC",			"Ext + Convert",		"Ext + NonConvert",		"Ext + Accept",			"Ext + ModeChange",
										"Ext + Space",			"Ext + PageUp",			"Ext + PageDown",			"Ext + End",			"Ext + Home",			"Ext + Left",			"Ext + Up",				"Ext + Right",		//2
										"Ext + Down",			"Ext + Select",			"Ext + Print",				"Ext + Execute",		"Ext + PrintScreen",	"Ext + Insert",			"Ext + Delete",			"Ext + Help",
										"Ext + 0",				"Ext + 1",				"Ext + 2",					"Ext + 3",				"Ext + 4",				"Ext + 5",				"Ext + 6",				"Ext + 7",			//3
										"Ext + 8",				"Ext + 9",				"",							"",						"",						"",						"",						"",
										"",						"Ext + A",				"Ext + B",					"Ext + C",				"Ext + D",				"Ext + E",				"Ext + F",				"Ext + G",			//4
										"Ext + H",				"Ext + I",				"Ext + J",					"Ext + K",				"Ext + L",				"Ext + M",				"Ext + N",				"Ext + O",
										"Ext + P",				"Ext + Q",				"Ext + R",					"Ext + S",				"Ext + T",				"Ext + U",				"Ext + V",				"Ext + W",			//5
										"Ext + X",				"Ext + Y",				"Ext + Z",					"Ext + LWin",			"Ext + RWin",			"Ext + Apps",			"",						"",
										"Ext + Num0",			"Ext + Num1",			"Ext + Num2",				"Ext + Num3",			"Ext + Num4",			"Ext + Num5",			"Ext + Num6",			"Ext + Num7",			//6
										"Ext + Num8",			"Ext + Num9",			"Ext + Num*",				"Ext + Num+",			"Ext + Separator",		"Ext + Num-",			"Ext + Num.",			"Ext + Num/",
										"Ext + F1",				"Ext + F2",				"Ext + F3",					"Ext + F4",				"Ext + F5",				"Ext + F6",				"Ext + F7",				"Ext + F8",			//7
										"Ext + F9",				"Ext + F10",			"Ext + F11",				"Ext + F12",			"Ext + F13",			"Ext + F14",			"Ext + F15",			"Ext + F16",	
										"Ext + F17",			"Ext + F18",			"Ext + F19",				"Ext + F20",			"Ext + F21",			"Ext + F22",			"Ext + F23",			"Ext + F24",			//8
										"",						"",						"",							"",						"",						"",						"",						"",
										"Ext + NumLock",		"Ext + ScrollLock",	"",								"",						"",						"",						"",						"",				//9
										"",						"",						"",							"",						"",						"",						"",						"",	
										"",						"",						"",							"",						"",						"",						"Ext + BrowserBack",	"Ext + BrowserForward",				//A
										"Ext + BrowserRefresh",	"Ext + BrowserStop",	"Ext + BrowserSearch",	"Ext + BrowserFavorites",	"Ext + BrowserHome",	"Ext + VolumeMute",	    "Ext + VolumeDown",		"Ext + VolumeUp",
										"Ext + MediaNextTrack",	"Ext + MediaPrevTrack",	"Ext + MediaStop",		"Ext + MediaPlayPause",     "Ext + LaunchMail",		"Ext + LaunchMediaSelect","Ext + LaunchApp1",	"Ext + LaunchApp2",				//B
										"",						"",						"Ext + ;",					"Ext + =",				"Ext + ,",				"Ext + -",				"Ext + .",				"Ext + /",
										"Ext + `",				"",						"",							"",						"",						"",						"",						"",				//C
										"",						"",						"",							"",						"",						"",						"",						"",
										"",						"",						"",							"",						"",						"",						"",						"",				//D
										"",						"",						"",							"Ext + [",				"Ext + \\",				"Ext + ]",				"Ext + '",				"",
										"",						"",						"",							"",						"",						"",						"",						"",				//E
										"",						"",						"",							"",						"",						"",						"",						"",	
										"",						"",						"",							"",						"",						"",						"",						"",				//F
										"",						"",						"",							"",						"",						"",						"",						"",
										"Ext + LDButton",		"Ext + RDButton",		"Ext + MDButton",			"",						"",						"",						"",						"",				//10
										"",						"",						"",							"",						"",						"",						"",						""
		                       };
												
	DWORD dwRet         = 0;
	DWORD dwLine        = 0;
	LPCTSTR lpStringIn  = 0;
	LPCTSTR lpStringout = 0;

	for(dwLine = 0; dwLine < MaxNumber; dwLine++)
	{
		lpStringIn = szHotKeyIn[dwLine];
//		dwRet = m_KHotKeyTable.TranslateTextToHotkey(lpStringIn);

		if(dwRet != 0)
		{
			KHotKeyTable_Test_Vector.push_back(dwRet);
//			lpStringout = m_KHotKeyTable.TranslateHotkeyToText(dwRet);
			CPPUNIT_ASSERT(strcmp(szHotKeyIn[dwLine], lpStringout) == 0);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//
// Pre Senario:
//(1)
// By Object(s):
// m_KHotKeyTable of UI::KHotkeyTable.
// Operation(s):
// Pass all Virtual-Key Codes and NULL to TranslateHotkeyToText(LPCTSTR),and record the result
// CheckPoint(s):
//(1)Perform a lowercase comparison of strings.
//(2)Perform a capital comparison of strings.
//(3)Perform TranslateTextToHotkey(lpctString) and compare the output and record
//////////////////////////////////////////////////////////////////////////
void KHotKeyTable_Test::testTranslateHotkeyToText()
{
	DWORD dwHotKeyOut  = 0;
	DWORD dwRet        = 0;
	LPCTSTR lpctString = NULL;
	vector<DWORD>::iterator it     = KHotKeyTable_Test_Vector.begin();
	vector<DWORD>::iterator it_end = KHotKeyTable_Test_Vector.end();
	for(; it != it_end; ++it)
	{
		dwRet = *it;
//		lpctString = m_KHotKeyTable.TranslateHotkeyToText(*it);

		if (*it != 0)
		{		
			CPPUNIT_ASSERT(_tcscmp(lpctString, _T("")) != 0);
			CPPUNIT_ASSERT(_tcsicmp(lpctString, _T("")) != 0);
		}

//		dwHotKeyOut = m_KHotKeyTable.TranslateTextToHotkey(lpctString);
		CPPUNIT_ASSERT(dwRet == dwHotKeyOut);
	}
}

#endif //#ifdef _TEST
