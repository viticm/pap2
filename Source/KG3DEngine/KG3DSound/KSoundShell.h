//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KSoundShell.h
// Date:	2004.07.21
// Code:	ShiShuaiWei(Canon)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef _KSOUNDSHELL_H
#define _KSOUNDSHELL_H
//---------------------------------------------------------------------------
#pragma once
#include "IKG3DSoundShell.h"
#include "KMp3Music.h"
#include "KWavSound.h"
#include "KDSound.h"
#include "KMp3Music3D.h"
#include "KWavSound3D.h"
#include "KRecordSound.h"
#include "d3dx9math.h"
//---------------------------------------------------------------------------
//## 声音模块的对外接口的实现。

#define KG_2D_MUSIC_COUNT 0
#define KG_2D_SOUND_COUNT 8
#define KG_3D_MUSIC_COUNT 0
#define KG_3D_SOUND_COUNT 64
#define KG_MAX_SINGLE_2D_SOUND_COUNT 3
#define KG_MAX_SINGLE_3D_SOUND_COUNT 6

class KSoundShell :	public IKG3DSoundShell
{
public:
	virtual ULONG AddRef();
	//销毁接口对象
	virtual ULONG Release();
	//初始化音效播放
	virtual HRESULT	Init(HWND hWnd);

	virtual HRESULT Active();

	//收听者接口
	virtual HRESULT	SetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity);
	virtual HRESULT	GetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity);
	virtual HRESULT	SetOrientation(D3DVECTOR* pvFront, D3DVECTOR* pvTop);
	virtual HRESULT	SetListenerParameters(FLOAT fDopplerFactor, FLOAT fRolloffFactor);

	virtual HRESULT	SetVolume(float fVolume); //设置整体音量 0 - 1
	virtual float	GetVolume(); //取得整体音量
	virtual HRESULT	SetVolume(int nType, float fVolume); //设置这一类型的音乐的音量 0 - 1
	virtual float	GetVolume(int nType); //取得这一类型的音量

	//----------------用来解决窗口丢失焦点之后再找回的时候音量不正确的问题。
	virtual HRESULT	Lock(); //锁定，这个时候停止播放声音
	virtual HRESULT	Unlock(); //解除锁定，这个时候看Enable的情况是否播放
	virtual BOOL	IsLocked(); //是否被锁定

	virtual HRESULT	Enable(BOOL bEnable); //允许播放音乐
	virtual BOOL	IsEnable();
	virtual HRESULT	Enable(int nType, BOOL bEnable); //允许播放这一类型音乐
	virtual BOOL	IsEnable(int nType);

	virtual HRESULT	Enable3DSound(BOOL bEnable); //允许3D方式播放音效
	virtual BOOL	IsEnable3DSound();

	//----------2D音乐接口-----------------
	//播放背景音乐，同时只能播放一个背景音乐，mp3，双通道
	virtual HRESULT PlayBgMusic(int nType, LPCSTR FileName, INT nPan, float fVolume);
	virtual HRESULT StopBgMusic(BOOL bimmediately);
	virtual HRESULT SetBgMusicVolume(float fVolume);
	virtual BOOL	IsPlayingBgMusic();
	virtual BOOL	IsPlayingBgMusic(LPCSTR FileName);
	virtual BOOL	IsBgMusicLoop();
	virtual HRESULT	SetBgMusicLoop(BOOL bLoop);

	//播放其他音乐，比如人物内心对白等，可以同时存在多个。mp3，双通道
	virtual HRESULT PlayMusic(int nType, LPCSTR FileName, INT nPan, float fVolume, BOOL bLoop, DWORD *pdwID);
	virtual HRESULT StopMusic(DWORD dwID);
	virtual HRESULT SetMusicVolume(DWORD dwID, float fVolume);
	virtual BOOL	IsPlayingMusic(DWORD dwID);
	virtual BOOL	IsPlayingMusic(LPCSTR FileName);
	virtual HRESULT StopAllMusic();

	//播放音效，比如界面提示音。可以同时存在多个。wav,双通道
	virtual HRESULT PlaySound(int nType, LPCSTR FileName, INT nPan, float fVolume, BOOL bLoop, DWORD *pdwID);
	virtual HRESULT StopSound(DWORD dwID);
	virtual HRESULT StopSoundAfterThisTime(DWORD dwID);
	virtual HRESULT SetSoundVolume(DWORD dwID, float fVolume);
	virtual BOOL	IsPlayingSound(DWORD dwID);
	virtual BOOL	IsPlayingSound(LPCSTR FileName);
	virtual HRESULT StopAllSound();


	//----------3D音乐接口-----------------
	//播放其他音乐，比如人物对话，可以同时存在多个。mp3，单通道
	virtual HRESULT Play3DMusic(int nType, LPCSTR FileName, float fVolume, BOOL bLoop, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity, FLOAT fMinDistance, FLOAT fMaxDistance, DWORD *pdwID);
	virtual HRESULT Stop3DMusic(DWORD dwID);
	virtual HRESULT Set3DMusicVolume(DWORD dwID, float fVolume);
	virtual BOOL	IsPlaying3DMusic(DWORD dwID);
	virtual BOOL	IsPlaying3DMusic(LPCSTR FileName);
	virtual HRESULT	Set3DMusicPosition(DWORD dwID, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity);
	virtual HRESULT	Set3DMusicParameters(DWORD dwID, FLOAT fMinDistance, FLOAT fMaxDistance);
	virtual HRESULT StopAll3DMusic();

	//播放音效，比如打击声音，或者特效声音。可以同时存在多个。wav，单通道
	virtual HRESULT Play3DSound(int nType, LPCSTR FileName, float fVolume, BOOL bLoop, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity, FLOAT fMinDistance, FLOAT fMaxDistance, DWORD *pdwID);
	virtual HRESULT Stop3DSound(DWORD dwID);
	virtual HRESULT Stop3DSoundAfterThisTime(DWORD dwID);
	virtual HRESULT Set3DSoundVolume(DWORD dwID, float fVolume);
	virtual BOOL	IsPlaying3DSound(DWORD dwID);
	virtual BOOL	IsPlaying3DSound(LPCSTR FileName);
	virtual HRESULT	Set3DSoundPosition(DWORD dwID, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity);
	virtual HRESULT	Set3DSoundParameters(DWORD dwID, FLOAT fMinDistance, FLOAT fMaxDistance);
	virtual HRESULT StopAll3DSound();

	//录音接口
	virtual HRESULT StartRecordSound();
	virtual HRESULT StopRecordSound();
	virtual HRESULT LockRecordSoundBuffer(void **ppBuffer, DWORD *pSize);
	virtual HRESULT ClearRecordSoundBuffer();
	virtual HRESULT UnlockRecordSoundBuffer();
	virtual void * GetRecordSoundWaveFormatEx();

	//录制接口相关
	virtual HRESULT SetEventRecordFuc(RecordSoundEventFuc pFunc);

	KSoundShell();

protected:
	virtual ~KSoundShell();

private:
	HRESULT ClearBuffer();
	HRESULT InternalInit();
	HRESULT Clear();
	float GetFinalVolume(int nType, float fVolume);
	HRESULT Stop();
	HRESULT UpdatePlayState();

private:
	struct KMp3MusicInfo
	{
		string strFile;
		KMp3Music mp3Music;
		KMp3MusicInfo *pNext;
	};
	KMp3MusicInfo * m_pMusic;

	struct KWavSoundInfo
	{
		UINT uFileHash;
		string strFile;
		KWavSound wavSound;
		DWORD dwTime;
		KWavSoundInfo *pNext;
	};
	KWavSoundInfo * m_pSound;

	struct KMp3Music3DInfo
	{
		string strFile;
		KMp3Music3D mp3Music3D;
		KMp3Music3DInfo *pNext;
	};
	KMp3Music3DInfo * m_p3DMusic;

	struct KWavSound3DInfo
	{
		UINT uFileHash;
		string strFile;
		BOOL bUpdatePosition;
		//D3DXVECTOR3 vec3CurrentPosition;
		D3DXVECTOR3 vec3NextPosition;
		KWavSound3D wavSound3D;
		DWORD dwTime;
		KWavSound3DInfo *pNext;
	};
	KWavSound3DInfo * m_p3DSound;

	struct KSoundTypeInfo
	{
		float fVolume;
		BOOL bEnable;
		KSoundTypeInfo()
		{
			fVolume = 1.0f;
			bEnable = TRUE;
		}
	};
	typedef std::map<int, KSoundTypeInfo> KTypeToInfoMap;

	KMp3Music			m_BgMusic;			//用于播放背景音乐
	char				m_szNextMusic[MAX_PATH];

	KDirectSound	m_DirectSound;  //用于初始化DirectSound

	KRecordSound	m_RecordSound;

	DWORD m_dwID; //当前ID

	ULONG m_ulRefCount; //实例的引用计数
	BOOL m_bInit; //有没有被初始化过

	float m_fVolume;
	KTypeToInfoMap m_Volume;

	BOOL m_bEnable;
	BOOL m_bLoopBgMusic;

	BOOL m_bEnbale3DSound;

	BOOL m_bLock;
	D3DXVECTOR3 m_vec3ListenerPosition;
	BOOL m_bListenerUpdated;
	RecordSoundEventFuc m_pRecordEventFuc;
};
//---------------------------------------------------------------------------
#endif