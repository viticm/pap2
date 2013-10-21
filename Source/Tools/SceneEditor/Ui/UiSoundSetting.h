/*****************************************************************************************
//	界面声音
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-5-15
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

enum UI_SOUND_INDEX
{
	UI_SI_WND_OPENCLOSE = 0,	//弹出或关闭界面时刷的一声
	UI_SI_POPUP_OUTGAME_WND,	//进入游戏外的界面的声音
	UI_SI_OVER_OUTGAME_WND_BTN,	//鼠标移动到游戏外的界面的按钮上
	UI_SI_PICKPUT_ITEM,			//拿起或放置道具到物品拦时的声音
	UI_SI_PLAYER_ATTRIB,		//选人及五行属性声音
	UI_SI_FRIEND_ONLINE,		//好友上线提示声

	UI_SOUND_COUNT
};

bool UiSoundLoadSetting();		//载入设定
void UiSoundPlay(UI_SOUND_INDEX eIndex);	//播放指定的声音
void UiSoundPlay(char* SoundFileName);	//播放指定的声音
