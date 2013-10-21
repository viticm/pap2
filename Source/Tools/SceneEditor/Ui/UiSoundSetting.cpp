/*****************************************************************************************
//	界面声音
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-5-15
*****************************************************************************************/
//#include "KWin32.h"
//#include "KIniFile.h"
#include "UiSoundSetting.h"
//#include "../../core/src/coreshell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define	SETTING_FILE	"\\Settings\\SoundList.txt"

static char s_SoundFileName[UI_SOUND_COUNT][80] =
{
	"",
	"",
	"",
	"",
	"",
	"",
};

//载入设定
bool UiSoundLoadSetting()
{
	KIniFile	Setting;
	if (!Setting.Load(SETTING_FILE))
		return false;
	char	szSection[8];
	for (int i = 0; i < (int)UI_SOUND_COUNT; i++)
	{
		itoa(i, szSection, 10);
		Setting.GetString("Ui", szSection, "", s_SoundFileName[i], sizeof(s_SoundFileName[i]));
	}
	return true;
}

//播放指定的声音
void UiSoundPlay(UI_SOUND_INDEX eIndex)
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_PLAY_SOUND, (unsigned int)(&s_SoundFileName[eIndex]), 0);
}

//播放指定的声音
void UiSoundPlay(char* SoundFileName)
{
	if (g_pCoreShell)
		g_pCoreShell->OperationRequest(GOI_PLAY_SOUND, (unsigned int)(SoundFileName), 0);
}
