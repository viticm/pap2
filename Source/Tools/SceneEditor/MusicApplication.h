#pragma once

#define IMUSIC_INTERFACE_VERSION "C.Q_Music_System.01" 

#define PlayError -2
#define NoChannelToPlay -1
#define MethodTakePlaceError -3
#define RunOk 1
class MusicApplication
{
public:
	MusicApplication(void) ;
	virtual ~MusicApplication(void);
	// 播放一个音乐，并返回句柄,如果管道不够，则返回NoChannelToPlay
	virtual int PlayAmusic(char * sFileName) = 0 ;
	// 停止演奏某个音乐iChannelNum是音乐句柄
	virtual int StopAmusic(int iHandle) = 0 ;
	// 停止播放所有的音乐
	virtual int StopAllMusic(void) = 0 ;
	// 初始化
	virtual void Init(void) = 0 ;

	// 终止系统
	virtual void Terminate(void) = 0 ;
};

