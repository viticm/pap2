//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KSoundShell.cpp
// Date:	2004.07.21
// Code:	ShiShuaiWei(Canon)
// Desc:	Sound Shell Class
//---------------------------------------------------------------------------
#include "StdAfx.h"
#include "KSoundShell.h"

//#define KG_SOUND_SHELL_RES_LEAK_CHECK
//#define USE_DUMMY_SOUND_SHELL
#define USE_MULTI_THREAD_SAFE_SOUND_SHELL

static IKG3DSoundShell *g_pSound = NULL;

static float s_fMinDistanceToListener = 0.01f;
static float s_fMinDistanceDiff = 0.01f;

#ifdef USE_DUMMY_SOUND_SHELL
struct KDummySoundShell : IKG3DSoundShell
{
public:
    KDummySoundShell() 
        : m_ulRefCount(0)
        , m_ulBgMusic(0)
        , m_ulMusic(0)
        , m_ulSound(0)
        , m_ul3DMusic(0)
        , m_ul3DSound(0)
        , m_ulRecordSound(0)
    {
    }

    virtual ULONG AddRef() { return ++m_ulRefCount; }
    virtual ULONG Release() 
    { 
        ULONG ulRefCount = --m_ulRefCount;
        if (ulRefCount == 0) 
        {
            ASSERT(m_ulBgMusic == 0);
            ASSERT(m_ulMusic == 0);
            ASSERT(m_ulSound == 0);
            ASSERT(m_ul3DMusic == 0);
            ASSERT(m_ul3DSound == 0);
            ASSERT(m_ulRecordSound == 0);

            delete this;
        }
        return ulRefCount; 
    }

    virtual HRESULT	Init(HWND hWnd) { return S_OK; }

    virtual HRESULT Active() { return S_OK; }

	virtual HRESULT	SetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity) { return S_OK; }
	virtual HRESULT	GetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity) { return S_OK; }
	virtual HRESULT	SetOrientation(D3DVECTOR* pvFront, D3DVECTOR* pvTop) { return S_OK; }
	virtual HRESULT	SetListenerParameters(FLOAT fDopplerFactor, FLOAT fRolloffFactor) { return S_OK; }

	virtual HRESULT	SetVolume(float fVolume) { return S_OK; }
	virtual float	GetVolume() { return 0.0f; }
	virtual HRESULT	SetVolume(int nType, float fVolume) { return S_OK; }
	virtual float	GetVolume(int nType) { return 0.0f; }

	virtual HRESULT	Lock() { return S_OK; }
	virtual HRESULT	Unlock() { return S_OK; }
	virtual BOOL	IsLocked() { return FALSE; }

	virtual HRESULT	Enable(BOOL bEnable) { return S_OK; }
	virtual BOOL	IsEnable() { return FALSE; }
	virtual HRESULT	Enable(int nType, BOOL bEnable) { return S_OK; }
	virtual BOOL	IsEnable(int nType) { return FALSE; }

	virtual HRESULT	Enable3DSound(BOOL bEnable) { return S_OK; }
	virtual BOOL	IsEnable3DSound() { return TRUE; }

	virtual HRESULT PlayBgMusic(int nType, LPCSTR FileName, INT nPan, float fVolume) { ++m_ulBgMusic; return S_OK; }
	virtual HRESULT StopBgMusic(BOOL bImmediately) { --m_ulBgMusic; return S_OK; }
	virtual HRESULT SetBgMusicVolume(float fVolume) { return S_OK; }
	virtual BOOL	IsPlayingBgMusic() { return FALSE; }
	virtual BOOL	IsPlayingBgMusic(LPCSTR FileName) { return FALSE; }
	virtual BOOL	IsBgMusicLoop() { return FALSE; }
	virtual HRESULT	SetBgMusicLoop(BOOL bLoop) { return S_OK; }

	virtual HRESULT PlayMusic(int nType, LPCSTR FileName, INT nPan, float fVolume, BOOL bLoop, DWORD *pdwID) { ++m_ulMusic; return S_OK; }
	virtual HRESULT StopMusic(DWORD dwID) { --m_ulMusic; return S_OK; }
	virtual HRESULT SetMusicVolume(DWORD dwID, float fVolume) { return S_OK; }
	virtual BOOL	IsPlayingMusic(DWORD dwID) { return FALSE; }
	virtual BOOL	IsPlayingMusic(LPCSTR FileName) { return FALSE; }
	virtual HRESULT StopAllMusic() { return S_OK; }

	virtual HRESULT PlaySound(int nType, LPCSTR FileName, INT nPan, float fVolume, BOOL bLoop, DWORD *pdwID) { ++m_ulSound; return S_OK; }
	virtual HRESULT StopSound(DWORD dwID) { --m_ulSound; return S_OK; }
	virtual HRESULT StopSoundAfterThisTime(DWORD dwID) { return S_OK; }
	virtual HRESULT SetSoundVolume(DWORD dwID, float fVolume) { return S_OK; }
	virtual BOOL	IsPlayingSound(DWORD dwID) { return FALSE; }
	virtual BOOL	IsPlayingSound(LPCSTR FileName) { return FALSE; }
	virtual HRESULT StopAllSound() { return S_OK; }

	virtual HRESULT Play3DMusic(int nType, LPCSTR FileName, float fVolume, BOOL bLoop, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity, FLOAT fMinDistance, FLOAT fMaxDistance, DWORD *pdwID) { ++m_ul3DMusic; return S_OK; }
	virtual HRESULT Stop3DMusic(DWORD dwID) { --m_ul3DMusic; return S_OK; }
	virtual HRESULT Set3DMusicVolume(DWORD dwID, float fVolume) { return S_OK; }
	virtual BOOL	IsPlaying3DMusic(DWORD dwID) { return FALSE; }
	virtual BOOL	IsPlaying3DMusic(LPCSTR FileName) { return FALSE; }
	virtual HRESULT	Set3DMusicPosition(DWORD dwID, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity) { return S_OK; }
	virtual HRESULT	Set3DMusicParameters(DWORD dwID, FLOAT fMinDistance, FLOAT fMaxDistance) { return S_OK; }
	virtual HRESULT StopAll3DMusic() { return S_OK; }

	virtual HRESULT Play3DSound(int nType, LPCSTR FileName, float fVolume, BOOL bLoop, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity, FLOAT fMinDistance, FLOAT fMaxDistance, DWORD *pdwID) { ++m_ul3DSound; return S_OK; }
	virtual HRESULT Stop3DSound(DWORD dwID) { --m_ul3DSound; return S_OK; }
	virtual HRESULT Stop3DSoundAfterThisTime(DWORD dwID) { --m_ul3DSound; return S_OK; }
	virtual HRESULT Set3DSoundVolume(DWORD dwID, float fVolume) { return S_OK; }
	virtual BOOL	IsPlaying3DSound(DWORD dwID)  { return FALSE; }
	virtual BOOL	IsPlaying3DSound(LPCSTR FileName) { return FALSE; }
	virtual HRESULT	Set3DSoundPosition(DWORD dwID, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity) { return S_OK; }
	virtual HRESULT	Set3DSoundParameters(DWORD dwID, FLOAT fMinDistance, FLOAT fMaxDistance) { return S_OK; }
	virtual HRESULT StopAll3DSound() { return S_OK; }

	virtual HRESULT StartRecordSound() { ++m_ulRecordSound; return S_OK; }
	virtual HRESULT StopRecordSound() { --m_ulRecordSound; return S_OK; }
	virtual HRESULT LockRecordSoundBuffer(void **ppBuffer, DWORD *pdwSize) { return S_OK; }
	virtual HRESULT ClearRecordSoundBuffer() { return S_OK; }
	virtual HRESULT UnlockRecordSoundBuffer() { return S_OK; }
	virtual void * GetRecordSoundWaveFormatEx() { return NULL; }

	virtual HRESULT SetEventRecordFuc(RecordSoundEventFuc pFunc) { return S_OK; }

private:
    ULONG m_ulRefCount;

	ULONG m_ulBgMusic;
	ULONG m_ulMusic;
	ULONG m_ulSound;
	ULONG m_ul3DMusic;
	ULONG m_ul3DSound;
	ULONG m_ulRecordSound;
};
#endif

#ifdef USE_MULTI_THREAD_SAFE_SOUND_SHELL
struct KMultiThreadSafeSoundShell : public KSoundShell
{
private:
    class KCSGuard
    {
    public:
        KCSGuard(PCRITICAL_SECTION pCS) : m_pCS(pCS)
        {
            ::EnterCriticalSection(m_pCS);
        }

        ~KCSGuard()
        {
        	::LeaveCriticalSection(m_pCS);
        }

    private:
        PCRITICAL_SECTION m_pCS;
    };

public:
    KMultiThreadSafeSoundShell() 
    {
    	::InitializeCriticalSection(&m_cs);
    }

    virtual ~KMultiThreadSafeSoundShell()
    {
    	::DeleteCriticalSection(&m_cs);
    }

    virtual ULONG AddRef() 
    { 
        KCSGuard csg(&m_cs);
        return KSoundShell::AddRef(); 
    }

    virtual ULONG Release() 
    {
        return KSoundShell::Release(); 
    }

	virtual HRESULT Active()
	{
		KCSGuard csg(&m_cs);
		return KSoundShell::Active();
	}

    virtual HRESULT	Init(HWND hWnd) 
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Init(hWnd); 
    }

	virtual HRESULT	SetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity) 
    { 
        KCSGuard csg(&m_cs);
        return KSoundShell::SetListenerPosition(pvPosition, pvVelocity); 
    }

	virtual HRESULT	GetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity)
    { 
        KCSGuard csg(&m_cs);
        return KSoundShell::GetListenerPosition(pvPosition, pvVelocity);
    }

	virtual HRESULT	SetOrientation(D3DVECTOR* pvFront, D3DVECTOR* pvTop)
    { 
        KCSGuard csg(&m_cs);
        return KSoundShell::SetOrientation(pvFront, pvTop);
    }

	virtual HRESULT	SetListenerParameters(FLOAT fDopplerFactor, FLOAT fRolloffFactor)
    { 
        KCSGuard csg(&m_cs);
        return KSoundShell::SetListenerParameters(fDopplerFactor, fRolloffFactor);
    }

	virtual HRESULT	SetVolume(float fVolume) 
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::SetVolume(fVolume);
    }

	virtual float GetVolume()
    { 
        KCSGuard csg(&m_cs);
        return KSoundShell::GetVolume();
    }

	virtual HRESULT	SetVolume(int nType, float fVolume)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::SetVolume(nType, fVolume);
    }

	virtual float GetVolume(int nType)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::GetVolume(nType);
    }

	virtual HRESULT	Lock()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Lock();
    }

	virtual HRESULT	Unlock()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Unlock();
    }

	virtual BOOL IsLocked() 
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsLocked();
    }

	virtual HRESULT	Enable(BOOL bEnable)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Enable(bEnable);
    }

	virtual BOOL IsEnable()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsEnable();
    }

	virtual HRESULT	Enable(int nType, BOOL bEnable)
    { 
        KCSGuard csg(&m_cs);
        return KSoundShell::Enable(nType, bEnable);
    }

	virtual BOOL IsEnable(int nType)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsEnable(nType);
    }

	virtual HRESULT	Enable3DSound(BOOL bEnable)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Enable3DSound(bEnable);
    }

	virtual BOOL IsEnable3DSound()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsEnable3DSound();
    }

	virtual HRESULT PlayBgMusic(int nType, LPCSTR FileName, INT nPan, float fVolume)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::PlayBgMusic(nType, FileName, nPan, fVolume);
    }

	virtual HRESULT StopBgMusic(BOOL bImmediately)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::StopBgMusic(bImmediately);
    }

	virtual HRESULT SetBgMusicVolume(float fVolume)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::SetBgMusicVolume(fVolume);
    }

	virtual BOOL IsPlayingBgMusic()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsPlayingBgMusic();
    }

	virtual BOOL IsPlayingBgMusic(LPCSTR szFileName)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsPlayingBgMusic(szFileName);
    }

	virtual BOOL IsBgMusicLoop()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsBgMusicLoop();
    }

	virtual HRESULT	SetBgMusicLoop(BOOL bLoop)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::SetBgMusicLoop(bLoop);
    }

	virtual HRESULT PlayMusic(int nType, LPCSTR FileName, INT nPan, float fVolume, BOOL bLoop, DWORD *pdwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::PlayMusic(nType, FileName, nPan, fVolume, bLoop, pdwID);
    }

	virtual HRESULT StopMusic(DWORD dwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::StopMusic(dwID);
    }

	virtual HRESULT SetMusicVolume(DWORD dwID, float fVolume)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::SetMusicVolume(dwID, fVolume);
    }

	virtual BOOL IsPlayingMusic(DWORD dwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsPlayingMusic(dwID);
    }

	virtual BOOL IsPlayingMusic(LPCSTR FileName)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsPlayingMusic(FileName);
    }

	virtual HRESULT StopAllMusic()
    { 
        KCSGuard csg(&m_cs);
        return KSoundShell::StopAllMusic();
    }

	virtual HRESULT PlaySound(int nType, LPCSTR FileName, INT nPan, float fVolume, BOOL bLoop, DWORD *pdwID)
    { 
        KCSGuard csg(&m_cs);
        return KSoundShell::PlaySound(nType, FileName, nPan, fVolume, bLoop, pdwID);
    }

	virtual HRESULT StopSound(DWORD dwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::StopSound(dwID);
    }

	virtual HRESULT StopSoundAfterThisTime(DWORD dwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::StopSoundAfterThisTime(dwID);
    }

	virtual HRESULT SetSoundVolume(DWORD dwID, float fVolume)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::SetSoundVolume(dwID, fVolume);
    }

	virtual BOOL IsPlayingSound(DWORD dwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsPlayingSound(dwID);
    }

	virtual BOOL IsPlayingSound(LPCSTR FileName)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsPlayingSound(FileName);
    }

	virtual HRESULT StopAllSound()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::StopAllSound();
    }

	virtual HRESULT Play3DMusic(int nType, LPCSTR FileName, float fVolume, BOOL bLoop, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity, FLOAT fMinDistance, FLOAT fMaxDistance, DWORD *pdwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Play3DMusic(nType, FileName, fVolume, bLoop, pvPosition, pvVelocity, fMinDistance, fMaxDistance, pdwID);
    }

	virtual HRESULT Stop3DMusic(DWORD dwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Stop3DMusic(dwID);
    }

	virtual HRESULT Set3DMusicVolume(DWORD dwID, float fVolume)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Set3DMusicVolume(dwID, fVolume);
    }

	virtual BOOL IsPlaying3DMusic(DWORD dwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsPlaying3DMusic(dwID);
    }

	virtual BOOL IsPlaying3DMusic(LPCSTR FileName)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsPlaying3DMusic(FileName);
    }

	virtual HRESULT	Set3DMusicPosition(DWORD dwID, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Set3DMusicPosition(dwID, pvPosition, pvVelocity);
    }

	virtual HRESULT	Set3DMusicParameters(DWORD dwID, FLOAT fMinDistance, FLOAT fMaxDistance)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Set3DMusicParameters(dwID, fMinDistance, fMaxDistance);
    }

	virtual HRESULT StopAll3DMusic()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::StopAll3DMusic();
    }

	virtual HRESULT Play3DSound(int nType, LPCSTR FileName, float fVolume, BOOL bLoop, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity, FLOAT fMinDistance, FLOAT fMaxDistance, DWORD *pdwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Play3DSound(nType, FileName, fVolume, bLoop, pvPosition, pvVelocity, fMinDistance, fMaxDistance, pdwID);
    }

	virtual HRESULT Stop3DSound(DWORD dwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Stop3DSound(dwID);
    }

	virtual HRESULT Stop3DSoundAfterThisTime(DWORD dwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Stop3DSoundAfterThisTime(dwID);
    }

	virtual HRESULT Set3DSoundVolume(DWORD dwID, float fVolume)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Set3DSoundVolume(dwID, fVolume);
    }

	virtual BOOL IsPlaying3DSound(DWORD dwID)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsPlaying3DSound(dwID);
    }

	virtual BOOL IsPlaying3DSound(LPCSTR FileName)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::IsPlaying3DSound(FileName);
    }

	virtual HRESULT	Set3DSoundPosition(DWORD dwID, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Set3DSoundPosition(dwID, pvPosition, pvVelocity);
    }

	virtual HRESULT	Set3DSoundParameters(DWORD dwID, FLOAT fMinDistance, FLOAT fMaxDistance)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::Set3DSoundParameters(dwID, fMinDistance, fMaxDistance);
    }

	virtual HRESULT StopAll3DSound()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::StopAll3DSound();
    }

	virtual HRESULT StartRecordSound()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::StartRecordSound();
    }

	virtual HRESULT StopRecordSound()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::StopRecordSound();
    }

	virtual HRESULT LockRecordSoundBuffer(void **ppBuffer, DWORD *pdwSize)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::LockRecordSoundBuffer(ppBuffer, pdwSize);
    }

	virtual HRESULT ClearRecordSoundBuffer()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::ClearRecordSoundBuffer();
    }

	virtual HRESULT UnlockRecordSoundBuffer()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::UnlockRecordSoundBuffer();
    }

	virtual void* GetRecordSoundWaveFormatEx()
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::GetRecordSoundWaveFormatEx();
    }

	virtual HRESULT SetEventRecordFuc(RecordSoundEventFuc pFunc)
    {
        KCSGuard csg(&m_cs);
        return KSoundShell::SetEventRecordFuc(pFunc);
    }

private:
	CRITICAL_SECTION m_cs;
};
#endif

//---------------------------------------------------------------------------
KSoundShell::KSoundShell()
{
	m_dwID = 0;

	m_ulRefCount = 0;
	m_bInit = FALSE;
	m_fVolume = 1.0f;
	m_bEnable = TRUE;
	m_bLoopBgMusic = FALSE;
	m_bEnbale3DSound = TRUE;
	m_bLock = TRUE;

	m_pMusic = NULL;
	m_pSound = NULL;
	m_p3DMusic = NULL;
	m_p3DSound = NULL;
	m_pRecordEventFuc = NULL;
	m_szNextMusic[0] = '\0';
	m_vec3ListenerPosition = D3DXVECTOR3(.0f, .0f, .0f);
}

KSoundShell::~KSoundShell()
{
	m_RecordSound.StopRecordSound();
	Clear();

	KMp3MusicInfo * pMusicInfo = m_pMusic;
	KWavSoundInfo * pSoundInfo = m_pSound;
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pMusicInfo)
	{
#ifdef KG_SOUND_SHELL_RES_LEAK_CHECK
        KGLogPrintf(KGLOG_INFO, "Release 2d mp3 [%s]\n", pMusicInfo->strFile.c_str());
#endif

        KMp3MusicInfo *pTemp = pMusicInfo;
		pMusicInfo = pMusicInfo->pNext;
		SAFE_DELETE(pTemp);
	}
	m_pMusic = NULL;

	while (pSoundInfo)
	{
#ifdef KG_SOUND_SHELL_RES_LEAK_CHECK
        KGLogPrintf(KGLOG_INFO, "Release 2d wav [%s]\n", pSoundInfo->strFile.c_str());
#endif

		KWavSoundInfo *pTemp = pSoundInfo;
		pSoundInfo = pSoundInfo->pNext;
		SAFE_DELETE(pTemp);
	}
	m_pSound = NULL;

	while (pMusic3DInfo)
	{
#ifdef KG_SOUND_SHELL_RES_LEAK_CHECK
        KGLogPrintf(KGLOG_INFO, "Release 3d mp3 [%s]\n", pMusic3DInfo->strFile.c_str());
#endif

		KMp3Music3DInfo *pTemp = pMusic3DInfo;
		pMusic3DInfo = pMusic3DInfo->pNext;
		SAFE_DELETE(pTemp);
	}
	m_p3DMusic = NULL;

	while (pSound3DInfo)
	{
#ifdef KG_SOUND_SHELL_RES_LEAK_CHECK
        KGLogPrintf(KGLOG_INFO, "Release 3d wav [%s]\n", pSound3DInfo->strFile.c_str());
#endif

		KWavSound3DInfo *pTemp = pSound3DInfo;
		pSound3DInfo = pSound3DInfo->pNext;
		SAFE_DELETE(pTemp);
	}
	m_p3DSound = NULL;

	m_DirectSound.Exit();
}

//初始化KObjCache对象,设置加载/卸载KWavSound对象的回调函数
HRESULT KSoundShell::Init(HWND hWnd)
{
	HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	KG_PROCESS_SUCCESS(m_bInit);
	
	hr = m_DirectSound.Init(hWnd);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = ClearBuffer();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = InternalInit();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = Clear();
	KGLOG_COM_PROCESS_ERROR(hr);

	m_bInit = TRUE;
Exit1:
	hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KSoundShell::SetEventRecordFuc(RecordSoundEventFuc pFunc)
{
	m_pRecordEventFuc = pFunc;
	return S_OK;
}
HRESULT KSoundShell::ClearBuffer()
{
	KMp3MusicInfo * pMusicInfo = m_pMusic;
	KWavSoundInfo * pSoundInfo = m_pSound;
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pMusicInfo)
	{
		KMp3MusicInfo *pTemp = pMusicInfo;
		pMusicInfo = pMusicInfo->pNext;
		SAFE_DELETE(pTemp);
	}
	m_pMusic = NULL;

	while (pSoundInfo)
	{
		KWavSoundInfo *pTemp = pSoundInfo;
		pSoundInfo = pSoundInfo->pNext;
		SAFE_DELETE(pTemp);
	}
	m_pSound = NULL;

	while (pMusic3DInfo)
	{
		KMp3Music3DInfo *pTemp = pMusic3DInfo;
		pMusic3DInfo = pMusic3DInfo->pNext;
		SAFE_DELETE(pTemp);
	}
	m_p3DMusic = NULL;

	while (pSound3DInfo)
	{
		KWavSound3DInfo *pTemp = pSound3DInfo;
		pSound3DInfo = pSound3DInfo->pNext;
		SAFE_DELETE(pTemp);
	}
	m_p3DSound = NULL;
	return S_OK;
}

HRESULT KSoundShell::InternalInit()
{
    HRESULT hrResult = E_FAIL;

	KMp3MusicInfo ** ppMusicInfo = &m_pMusic;
	KWavSoundInfo ** ppSoundInfo = &m_pSound;
	KMp3Music3DInfo ** ppMusic3DInfo = &m_p3DMusic;
	KWavSound3DInfo ** ppSound3DInfo = &m_p3DSound;
	for (int i = 0; i < KG_2D_MUSIC_COUNT; ++i)
	{
		*ppMusicInfo = new KMp3MusicInfo;
		KGLOG_PROCESS_ERROR(*ppMusicInfo);
		(*ppMusicInfo)->strFile = "";
		(*ppMusicInfo)->pNext = NULL;
		ppMusicInfo = &((*ppMusicInfo)->pNext);
	}
	
	for (int i = 0; i < KG_2D_SOUND_COUNT; ++i)
	{
		*ppSoundInfo = new KWavSoundInfo;
		KGLOG_PROCESS_ERROR(*ppSoundInfo);
		(*ppSoundInfo)->strFile = "";
		(*ppSoundInfo)->pNext = NULL;
		ppSoundInfo = &(*ppSoundInfo)->pNext;
	}

	for (int i = 0; i < KG_3D_MUSIC_COUNT; ++i)
	{
		*ppMusic3DInfo = new KMp3Music3DInfo;
		KGLOG_PROCESS_ERROR(*ppMusic3DInfo);
		(*ppMusic3DInfo)->strFile = "";
		(*ppMusic3DInfo)->pNext = NULL;
		ppMusic3DInfo = &(*ppMusic3DInfo)->pNext;
	}

	for (int i = 0; i < KG_3D_SOUND_COUNT; ++i)
	{
		*ppSound3DInfo = new KWavSound3DInfo;
		KGLOG_PROCESS_ERROR(*ppSound3DInfo);
		(*ppSound3DInfo)->strFile = "";
		(*ppSound3DInfo)->pNext = NULL;
		//(*ppSound3DInfo)->vec3CurrentPosition = D3DXVECTOR3(.0f, .0f, .0f);
		(*ppSound3DInfo)->vec3NextPosition = D3DXVECTOR3(.0f, .0f, .0f);
		ppSound3DInfo = &(*ppSound3DInfo)->pNext;
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}

ULONG KSoundShell::Release()
{
	ULONG ulRefCount = InterlockedDecrement((LONG*)&m_ulRefCount);
	if (ulRefCount == 0)
	{
		g_pSound = NULL;
		delete this;
	}
	return ulRefCount;
}
#include "atlbase.h"
#include "atltrace.h"

static DWORD dwMinTimeSpan = 100;
HRESULT KSoundShell::Active()
{
	if (m_szNextMusic[0] && !m_BgMusic.IsPlaying())
	{
		m_BgMusic.Open(m_szNextMusic);
		m_BgMusic.SetVolume(GetFinalVolume(m_BgMusic.GetType(), m_BgMusic.GetOriginalVolume()));
		if (!IsLocked() && IsEnable() && IsEnable(m_BgMusic.GetType()))
			m_BgMusic.Play(m_bLoopBgMusic, FALSE);
		m_szNextMusic[0] = '\0';
		m_BgMusic.FadeIn();
	}

	static DWORD dwLastTime = GetTickCount();
	DWORD dwCurrentTime = GetTickCount();
	D3DXVECTOR3 vec3Velocity(.0f, .0f, .0f);
	if (dwCurrentTime - dwLastTime > dwMinTimeSpan)
	{
		if (m_bListenerUpdated)
		{
			m_DirectSound.SetListenerPosition(&m_vec3ListenerPosition, &vec3Velocity);
		}
		KWavSound3DInfo * pSound3DInfo = m_p3DSound;
		while (pSound3DInfo)
		{
			if (pSound3DInfo->bUpdatePosition || 
				m_bListenerUpdated) 
			{
				float fPositionToListener = D3DXVec3Length(&(pSound3DInfo->vec3NextPosition - m_vec3ListenerPosition));
				if (fPositionToListener < s_fMinDistanceToListener)
				{
					//如果距离小于了距离listener的阈值那么就直接把音源设置到listener位置
					pSound3DInfo->vec3NextPosition = m_vec3ListenerPosition;
				}
				pSound3DInfo->wavSound3D.SetPosition(&pSound3DInfo->vec3NextPosition, &vec3Velocity, DS3D_DEFERRED);
				pSound3DInfo->bUpdatePosition = FALSE;
			}
			pSound3DInfo = pSound3DInfo->pNext;
		}
		m_DirectSound.CommitDeferredSettings();
		dwLastTime = dwCurrentTime;
		m_bListenerUpdated = FALSE;
	}

	return S_OK;
}
//HRESULT KSoundShell::Active()
//{
//	if (m_szNextMusic[0] && !m_BgMusic.IsPlaying())
//	{
//		m_BgMusic.Open(m_szNextMusic);
//		m_BgMusic.SetVolume(GetFinalVolume(m_BgMusic.GetType(), m_BgMusic.GetOriginalVolume()));
//		if (!IsLocked() && IsEnable() && IsEnable(m_BgMusic.GetType()))
//			m_BgMusic.Play(m_bLoopBgMusic, FALSE);
//		m_szNextMusic[0] = '\0';
//		m_BgMusic.FadeIn();
//	}
//
//	static DWORD dwLastTime = GetTickCount();
//	DWORD dwCurrentTime = GetTickCount();
//	D3DXVECTOR3 vec3Velocity(.0f, .0f, .0f);
//	if (dwCurrentTime - dwLastTime > dwMinTimeSpan)
//	{
//		if (m_bListenerUpdated)
//		{
//			m_DirectSound.SetListenerPosition(&m_vec3ListenerPosition, &D3DXVECTOR3(.0f, .0f, .0f));
//		}
//		KWavSound3DInfo * pSound3DInfo = m_p3DSound;
//		while (pSound3DInfo)
//		{
//			if (pSound3DInfo->bUpdatePosition || 
//				m_bListenerUpdated) 
//			{
//				float fPositionToListener = 0.0f;
//				float fPositionDiff = D3DXVec3Length(&(pSound3DInfo->vec3CurrentPosition - pSound3DInfo->vec3NextPosition));
//				if (fPositionDiff > s_fMinDistanceDiff)
//				{
//					//如果距离变化大于了距离变化的阈值才更新位置， 防止因为位置的高频低幅震荡
//					fPositionToListener = D3DXVec3Length(&(pSound3DInfo->vec3NextPosition - m_vec3ListenerPosition));
//				}
//				else
//				{
//					fPositionToListener = D3DXVec3Length(&(pSound3DInfo->vec3CurrentPosition - m_vec3ListenerPosition));
//				}
//
//				if (fPositionToListener < s_fMinDistanceToListener)
//				{
//					//如果距离小于了距离listener的阈值那么就直接把音源设置到listener位置
//					pSound3DInfo->vec3NextPosition = m_vec3ListenerPosition;
//				}
//				pSound3DInfo->wavSound3D.SetPosition(&pSound3DInfo->vec3NextPosition, &vec3Velocity, DS3D_DEFERRED);
//				pSound3DInfo->vec3CurrentPosition = pSound3DInfo->vec3NextPosition;
//				pSound3DInfo->bUpdatePosition = FALSE;
//			}
//			pSound3DInfo = pSound3DInfo->pNext;
//		}
//		m_DirectSound.CommitDeferredSettings();
//		dwLastTime = dwCurrentTime;
//		m_bListenerUpdated = FALSE;
//	}
//
//	return S_OK;
//}

ULONG KSoundShell::AddRef()
{
	return (ULONG)InterlockedIncrement((long *)&m_ulRefCount);
}

HRESULT KSoundShell::Clear()
{
	m_BgMusic.Close();

	KMp3MusicInfo * pMusicInfo = m_pMusic;
	KWavSoundInfo * pSoundInfo = m_pSound;
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pMusicInfo)
	{
		pMusicInfo->mp3Music.Close();
		pMusicInfo = pMusicInfo->pNext;
	}

	while (pSoundInfo)
	{
		pSoundInfo->wavSound.Stop();
		pSoundInfo = pSoundInfo->pNext;
	}

	while (pMusic3DInfo)
	{
		pMusic3DInfo->mp3Music3D.Close();
		pMusic3DInfo = pMusic3DInfo->pNext;
	}

	while (pSound3DInfo)
	{
		pSound3DInfo->wavSound3D.Stop();
		pSound3DInfo = pSound3DInfo->pNext;
	}

	return S_OK;
}

HRESULT KSoundShell::UpdatePlayState()
{
	m_BgMusic.SetVolume(GetFinalVolume(m_BgMusic.GetType(), m_BgMusic.GetOriginalVolume()));

	KMp3MusicInfo * pMusicInfo = m_pMusic;
	KWavSoundInfo * pSoundInfo = m_pSound;
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pMusicInfo)
	{
		pMusicInfo->mp3Music.SetVolume(GetFinalVolume(pMusicInfo->mp3Music.GetType(), pMusicInfo->mp3Music.GetOriginalVolume()));
		pMusicInfo = pMusicInfo->pNext;
	}

	while (pSoundInfo)
	{
		pSoundInfo->wavSound.SetVolume(GetFinalVolume(pSoundInfo->wavSound.GetType(), pSoundInfo->wavSound.GetOriginalVolume()));
		pSoundInfo = pSoundInfo->pNext;
	}

	while (pMusic3DInfo)
	{
		pMusic3DInfo->mp3Music3D.SetVolume(GetFinalVolume(pMusic3DInfo->mp3Music3D.GetType(), pMusic3DInfo->mp3Music3D.GetOriginalVolume()));
		pMusic3DInfo = pMusic3DInfo->pNext;
	}

	while (pSound3DInfo)
	{
		pSound3DInfo->wavSound3D.SetVolume(GetFinalVolume(pSound3DInfo->wavSound3D.GetType(), pSound3DInfo->wavSound3D.GetOriginalVolume()));
		pSound3DInfo = pSound3DInfo->pNext;
	}

	return S_OK;
}

float KSoundShell::GetFinalVolume(int nType, float fVolume)
{
	float fTypeVolume = 1.0f;
	KTypeToInfoMap::iterator it = m_Volume.find(nType);
	if (it != m_Volume.end())
		fTypeVolume = it->second.fVolume;

	return fTypeVolume * m_fVolume * fVolume;
}

HRESULT	KSoundShell::SetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity)
{
	if (D3DXVec3Length(&(m_vec3ListenerPosition - *pvPosition)) > s_fMinDistanceDiff)
	{
		m_vec3ListenerPosition = *pvPosition;
		m_bListenerUpdated = TRUE;
	}
	return S_OK;
}

HRESULT	KSoundShell::GetListenerPosition(D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity)
{
	return m_DirectSound.GetListenerPosition(pvPosition, pvVelocity);
}

HRESULT KSoundShell::SetOrientation(D3DVECTOR* pvFront, D3DVECTOR* pvTop)
{
	return m_DirectSound.SetOrientation(pvFront, pvTop);
}

HRESULT	KSoundShell::SetListenerParameters(FLOAT fDopplerFactor, FLOAT fRolloffFactor)
{
	return m_DirectSound.SetListenerParameters(fDopplerFactor, fRolloffFactor);
}

HRESULT	KSoundShell::SetVolume(float fVolume)
{
	m_fVolume = fVolume;

	UpdatePlayState();

	return S_OK;
}

float KSoundShell::GetVolume()
{
	return m_fVolume;
}

HRESULT	KSoundShell::SetVolume(int nType, float fVolume)
{
	KTypeToInfoMap::iterator it = m_Volume.find(nType);
	if (it == m_Volume.end())
	{
		KSoundTypeInfo Info;
		Info.fVolume = fVolume;
		m_Volume.insert(make_pair(nType, Info));
	}
	else
	{
		it->second.fVolume = fVolume;
	}

	UpdatePlayState();

	return S_OK;
}

float KSoundShell::GetVolume(int nType)
{
	float fTypeVolume = 1.0f;
	KTypeToInfoMap::iterator it = m_Volume.find(nType);
	if (it != m_Volume.end())
		return it->second.fVolume;
	return 1.0f;
}

HRESULT	KSoundShell::Lock()
{
	if (m_bLock)
		return E_FAIL;
	if (IsEnable())
		Stop();
	m_bLock = TRUE;
	return S_OK;
}

HRESULT	KSoundShell::Unlock()
{
	if (!m_bLock)
		return E_FAIL;

	if (IsEnable() && IsEnable(m_BgMusic.GetType()))
	{
		m_BgMusic.SetFullVolume(GetFinalVolume(m_BgMusic.GetType(), m_BgMusic.GetOriginalVolume()));
		m_BgMusic.Play(m_bLoopBgMusic, TRUE);
	}

	m_bLock = FALSE;
	return S_OK;
}

BOOL KSoundShell::IsLocked()
{
	return m_bLock;
}

HRESULT KSoundShell::Stop()
{
	m_BgMusic.SetVolume(0.0f);
	m_BgMusic.Stop();

	KMp3MusicInfo * pMusicInfo = m_pMusic;
	KWavSoundInfo * pSoundInfo = m_pSound;
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pMusicInfo)
	{
		pMusicInfo->mp3Music.Stop();
		pMusicInfo = pMusicInfo->pNext;
	}

	while (pSoundInfo)
	{
		pSoundInfo->wavSound.Stop();
		pSoundInfo = pSoundInfo->pNext;
	}

	while (pMusic3DInfo)
	{
		pMusic3DInfo->mp3Music3D.Stop();
		pMusic3DInfo = pMusic3DInfo->pNext;
	}

	while (pSound3DInfo)
	{
		pSound3DInfo->wavSound3D.Stop();
		pSound3DInfo = pSound3DInfo->pNext;
	}

	return S_OK;
}

HRESULT	KSoundShell::Enable(BOOL bEnable)
{
	m_bEnable = bEnable;

	if (!IsLocked() && IsEnable() && IsEnable(m_BgMusic.GetType()))
	{
		m_BgMusic.SetFullVolume(GetFinalVolume(m_BgMusic.GetType(), m_BgMusic.GetOriginalVolume()));
		m_BgMusic.Play(m_bLoopBgMusic, TRUE);
	}
	else
	{
		Stop();
	}

	return S_OK;
}

BOOL KSoundShell::IsEnable()
{
	return m_bEnable;
}

HRESULT	KSoundShell::Enable(int nType, BOOL bEnable)
{
	KTypeToInfoMap::iterator it = m_Volume.find(nType);
	if (it == m_Volume.end())
	{
		KSoundTypeInfo Info;
		Info.bEnable = bEnable;
		m_Volume.insert(make_pair(nType, Info));
	}
	else
	{
		it->second.bEnable = bEnable;
	}

	if(!bEnable)
	{
		KMp3MusicInfo * pMusicInfo = m_pMusic;
		KWavSoundInfo * pSoundInfo = m_pSound;
		KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
		KWavSound3DInfo * pSound3DInfo = m_p3DSound;
		while (pMusicInfo)
		{
			if (pMusicInfo->mp3Music.GetType() == nType)
				pMusicInfo->mp3Music.Stop();
			pMusicInfo = pMusicInfo->pNext;
		}

		while (pSoundInfo)
		{
			if (pSoundInfo->wavSound.GetType() == nType)
				pSoundInfo->wavSound.Stop();
			pSoundInfo = pSoundInfo->pNext;
		}

		while (pMusic3DInfo)
		{
			if (pMusic3DInfo->mp3Music3D.GetType() == nType)
				pMusic3DInfo->mp3Music3D.Stop();
			pMusic3DInfo = pMusic3DInfo->pNext;
		}

		while (pSound3DInfo)
		{
			if (pSound3DInfo->wavSound3D.GetType() == nType)
				pSound3DInfo->wavSound3D.Stop();
			pSound3DInfo = pSound3DInfo->pNext;
		}
	}

	if (nType == m_BgMusic.GetType())
	{
		if (!IsLocked() && IsEnable() && IsEnable(m_BgMusic.GetType()))
		{
			m_BgMusic.Play(m_bLoopBgMusic, TRUE);
		}
		else
		{
			m_BgMusic.Stop();
		}
	}

	UpdatePlayState();

	return S_OK;

}

BOOL KSoundShell::IsEnable(int nType)
{
	KTypeToInfoMap::iterator it = m_Volume.find(nType);
	if (it != m_Volume.end())
		return it->second.bEnable;
	return TRUE;
}

HRESULT	KSoundShell::Enable3DSound(BOOL bEnable)
{
	if ((bEnable && !m_bEnbale3DSound) || (!bEnable && m_bEnbale3DSound))
	{
		KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
		KWavSound3DInfo * pSound3DInfo = m_p3DSound;

		while (pMusic3DInfo)
		{
			pMusic3DInfo->mp3Music3D.Enable3DSound(bEnable);
			pMusic3DInfo = pMusic3DInfo->pNext;
		}

		while (pSound3DInfo)
		{
			pSound3DInfo->wavSound3D.Enable3DSound(bEnable);
			pSound3DInfo = pSound3DInfo->pNext;
		}
	}
	m_bEnbale3DSound = bEnable;

	return S_OK;
}

BOOL KSoundShell::IsEnable3DSound()
{
	return m_bEnbale3DSound;
}

HRESULT KSoundShell::PlayBgMusic(int nType, LPCSTR FileName, INT nPan, float fVolume)
{
	if (m_pRecordEventFuc)
	{
		SoundParameters MyParameters;
		MyParameters.nNum = 4;
		MyParameters.size_tDataSize = sizeof(int) + 
			(MAX_PATH) * sizeof(char) + sizeof(INT) + sizeof(float);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(int*)(&pData[MyParameters.nFrom[0]]) = nType;
		MyParameters.nFrom[1] = sizeof(int);
		strcpy(&(pData[MyParameters.nFrom[1]]),FileName);
		MyParameters.nFrom[2] = sizeof(int) + MAX_PATH * sizeof(char);
		*(INT*)(&pData[MyParameters.nFrom[2]]) = nPan;
		MyParameters.nFrom[3] = sizeof(int) + MAX_PATH * sizeof(char) + sizeof(INT);
		*(float*)(&pData[MyParameters.nFrom[3]]) = fVolume;
		(*m_pRecordEventFuc)(SOUNDFT_PLAYBGMUSIC,MyParameters);
		delete[](MyParameters.pData);
	}

	BOOL bRetCode = FALSE;
	BOOL bCanPlay = !IsLocked() && IsEnable() && IsEnable(nType);

	KG_PROCESS_ERROR(FileName && FileName[0]);

	if (strcmp(m_BgMusic.GetName(), FileName) == 0)
	{
		if (m_BgMusic.IsFadeOut() || m_BgMusic.IsFadeIn())
		{
			m_BgMusic.FadeIn();
		}
		else if (!m_BgMusic.IsPlaying())
		{
			if (bCanPlay)
				m_BgMusic.Play(m_bLoopBgMusic, FALSE);
		}
		m_szNextMusic[0] = '\0';
	}
	else
	{
		if (m_BgMusic.IsPlaying())
		{
			m_BgMusic.FadeOut();
			strncpy(m_szNextMusic, FileName, MAX_PATH);
			m_szNextMusic[MAX_PATH - 1] = '\0';
		}
		else
		{
			m_BgMusic.Open(LPSTR(FileName));
			m_BgMusic.SetOriginalVolume(fVolume);
			m_BgMusic.SetVolume(GetFinalVolume(nType, fVolume));
			m_BgMusic.SetPan(nPan);
			if (bCanPlay)
				m_BgMusic.Play(m_bLoopBgMusic, FALSE);
			m_szNextMusic[0] = '\0';
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;

}

HRESULT KSoundShell::StopBgMusic(BOOL bImmediately)
{
	if (m_pRecordEventFuc)
	{
		SoundParameters MyParameters;
		(*m_pRecordEventFuc)(SOUNDFT_STOPBGMUSIC,MyParameters);
	}
	if (m_BgMusic.IsPlaying())
	{
		if (bImmediately)
			m_BgMusic.Stop();
		else
			m_BgMusic.FadeOut();
	}
	return S_OK;
}

HRESULT KSoundShell::SetBgMusicVolume(float fVolume)
{
	if (m_pRecordEventFuc)
	{
		SoundParameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(float);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(float*)(&pData[MyParameters.nFrom[0]]) = fVolume;
		(*m_pRecordEventFuc)(SOUNDFT_SETBGMUSICVOLUME,MyParameters);
		delete[](MyParameters.pData);
	}

	m_BgMusic.SetOriginalVolume(fVolume);
	m_BgMusic.SetVolume(GetFinalVolume(m_BgMusic.GetType(), fVolume));
	return S_OK;
}

BOOL KSoundShell::IsPlayingBgMusic()
{
	return (m_BgMusic.IsPlaying() && !m_BgMusic.IsFadeOut());
}

BOOL KSoundShell::IsPlayingBgMusic(LPCSTR FileName)
{
	if (!FileName)
		return FALSE;
	if (m_szNextMusic[0] && strcmp(m_szNextMusic, FileName) == 0)
		return TRUE;
	if (m_BgMusic.IsFadeOut())
		return FALSE;
	if (strcmp(m_BgMusic.GetName(), FileName) == 0)
		return TRUE;
	return FALSE;
}

BOOL KSoundShell::IsBgMusicLoop()
{
	return m_bLoopBgMusic;
}

HRESULT	KSoundShell::SetBgMusicLoop(BOOL bLoop)
{
	if (m_pRecordEventFuc)
	{
		SoundParameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(BOOL);
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(BOOL*)(&pData[MyParameters.nFrom[0]]) = bLoop;
		(*m_pRecordEventFuc)(SOUNDFT_SETBGMUSICLOOP,MyParameters);
		delete[](MyParameters.pData);
	}
	m_bLoopBgMusic = bLoop;
	m_BgMusic.SetLoop(bLoop);
	return S_OK;
}

HRESULT KSoundShell::PlayMusic(int nType, LPCSTR FileName, INT nPan, float fVolume, BOOL bLoop, DWORD *pdwID)
{
    HRESULT hrResult = E_FAIL;
	KMp3MusicInfo* pMusicInfo = m_pMusic;

	KG_PROCESS_ERROR(FileName);
	KG_PROCESS_ERROR(!IsLocked() && IsEnable() && IsEnable(nType));

	while (pMusicInfo)
	{
		if (!pMusicInfo->mp3Music.IsPlaying())
		{
			pMusicInfo->mp3Music.Open((LPSTR)FileName);
			pMusicInfo->mp3Music.SetPan(nPan);
			pMusicInfo->mp3Music.SetType(nType);
			pMusicInfo->mp3Music.SetOriginalVolume(fVolume);
			pMusicInfo->mp3Music.SetVolume(GetFinalVolume(nType, fVolume));
			pMusicInfo->mp3Music.Play(bLoop, FALSE);
			pMusicInfo->mp3Music.SetID(++m_dwID);
			if (pdwID)
				*pdwID = m_dwID;
			KG_PROCESS_SUCCESS(1);
		}
		pMusicInfo = pMusicInfo->pNext;
	}
	KG_PROCESS_ERROR(0);

Exit1:
	hrResult = S_OK;
Exit0:
#if 0
    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_INFO, "KSoundShell::PlayMusic(%d, %s, %d, %f, %d, 0x%.8x)\n", nType, FileName, nPan, fVolume, bLoop, pdwID);
    }
#endif
	return hrResult;
}

HRESULT KSoundShell::StopMusic(DWORD dwID)
{
    BOOL bFound = FALSE;
	KMp3MusicInfo* pMusicInfo = m_pMusic;

	while (pMusicInfo)
	{
		if (pMusicInfo->mp3Music.GetID() == dwID)
		{
			pMusicInfo->mp3Music.Stop();

            bFound = TRUE;
			break;
		}
		pMusicInfo = pMusicInfo->pNext;
	}
#if 0
    if (!bFound)
    {
        KGLogPrintf(KGLOG_INFO, "KSoundShell::StopMusic(%u)\n", dwID);
    }
#endif
	return S_OK;
}

HRESULT KSoundShell::SetMusicVolume(DWORD dwID, float fVolume)
{
	KMp3MusicInfo * pMusicInfo = m_pMusic;
	while (pMusicInfo)
	{
		if (pMusicInfo->mp3Music.GetID() == dwID)
		{
			pMusicInfo->mp3Music.SetOriginalVolume(fVolume);
			pMusicInfo->mp3Music.SetVolume(GetFinalVolume(pMusicInfo->mp3Music.GetType(), fVolume));
			break;
		}
		pMusicInfo = pMusicInfo->pNext;
	}
	return S_OK;
}

BOOL KSoundShell::IsPlayingMusic(DWORD dwID)
{
	KMp3MusicInfo * pMusicInfo = m_pMusic;
	while (pMusicInfo)
	{
		if (pMusicInfo->mp3Music.GetID() == dwID)
			return pMusicInfo->mp3Music.IsPlaying();
		pMusicInfo = pMusicInfo->pNext;
	}
	return FALSE;
}

BOOL KSoundShell::IsPlayingMusic(LPCSTR FileName)
{
	if (!FileName)
		return FALSE;
	KMp3MusicInfo * pMusicInfo = m_pMusic;
	while (pMusicInfo)
	{
		if (pMusicInfo->strFile == FileName)
			return pMusicInfo->mp3Music.IsPlaying();
		pMusicInfo = pMusicInfo->pNext;
	}
	return FALSE;
}

HRESULT KSoundShell::StopAllMusic()
{
	KMp3MusicInfo * pMusicInfo = m_pMusic;
	while (pMusicInfo)
	{
		pMusicInfo->mp3Music.Stop();
		pMusicInfo = pMusicInfo->pNext;
	}
	return S_OK;
}

HRESULT KSoundShell::PlaySound(int nType, LPCSTR FileName, INT nPan, float fVolume, BOOL bLoop, DWORD *pdwID)
{
    HRESULT hrResult = E_FAIL;
	KWavSoundInfo* pSoundInfoToPlay = NULL;
	KWavSoundInfo* pSoundInfo = m_pSound;
	KWavSoundInfo* pLastSameSoundInfo = NULL;
	KWavSoundInfo* pLastSoundInfo = NULL;
	UINT uFileHash = g_FileNameHash(FileName);
	int nPlayCount = 0;

	KG_PROCESS_ERROR(FileName);
	KG_PROCESS_ERROR(!IsLocked() && IsEnable() && IsEnable(nType));

	//LOOP 自重
	while (pSoundInfo)
	{
		if (!pLastSoundInfo || (pSoundInfo->dwTime < pLastSoundInfo->dwTime && !pSoundInfo->wavSound.IsLoop()))
			pLastSoundInfo = pSoundInfo;
		BOOL bSame = pSoundInfo->uFileHash == uFileHash && pSoundInfo->strFile == FileName;
		if (bSame)
		{
			nPlayCount = nPlayCount + 1;
			if (!pLastSameSoundInfo || (pSoundInfo->dwTime < pLastSameSoundInfo->dwTime && !pSoundInfo->wavSound.IsLoop()))
				pLastSameSoundInfo = pSoundInfo;
		}
		if (!pSoundInfo->wavSound.IsPlaying())
		{
			if (bSame)
			{
				pSoundInfoToPlay = pSoundInfo;
				goto Exit1;
			}
			if (!pSoundInfoToPlay)
				pSoundInfoToPlay = pSoundInfo;
		}
		pSoundInfo = pSoundInfo->pNext;
	}

	if (nPlayCount >= KG_MAX_SINGLE_2D_SOUND_COUNT)
	{
		if (pLastSameSoundInfo)
		{
			pSoundInfoToPlay = pLastSameSoundInfo;
			goto Exit1;
		}
	}
	else
	{
		if(!pSoundInfoToPlay)
			pSoundInfoToPlay = pLastSoundInfo;
	}

	KG_PROCESS_ERROR(pSoundInfoToPlay);
	pSoundInfoToPlay->wavSound.Load((LPSTR)FileName);
	pSoundInfoToPlay->strFile = FileName;
	pSoundInfoToPlay->uFileHash = uFileHash;

Exit1:
	pSoundInfoToPlay->wavSound.SetType(nType);
	pSoundInfoToPlay->wavSound.SetOriginalVolume(fVolume);
	pSoundInfoToPlay->wavSound.Play(nPan, GetFinalVolume(nType, fVolume), bLoop);
	pSoundInfoToPlay->wavSound.SetID(++m_dwID);
	pSoundInfoToPlay->dwTime = KG_GetTickCount();
	if (pdwID)
		*pdwID = m_dwID;

	hrResult = S_OK;
Exit0:
#if 0
    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_INFO, "KSoundShell::PlaySound(%d, %s, %d, %f, %d, 0x%.8x)\n", nType, FileName, nPan, fVolume, bLoop, pdwID);
    }
#endif
	return hrResult;
}

HRESULT KSoundShell::StopSound(DWORD dwID)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bFound = FALSE;
	KWavSoundInfo* pSoundInfo = m_pSound;

	while (pSoundInfo)
	{
		if (pSoundInfo->wavSound.GetID() == dwID)
        {
            bFound = TRUE;

            hr = pSoundInfo->wavSound.Stop();
            KGLOG_COM_PROCESS_ERROR(hr);

            break;
        }
		pSoundInfo = pSoundInfo->pNext;
	}

    hrResult = S_OK;
Exit0:
#if 0
    if (!bFound)
    {
        KGLogPrintf(KGLOG_INFO, "KSoundShell::StopSound(%u)\n", dwID);
    }
#endif
	return hrResult;
}

HRESULT KSoundShell::StopSoundAfterThisTime(DWORD dwID)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bFound = FALSE;
	KWavSoundInfo* pSoundInfo = m_pSound;
	while (pSoundInfo)
	{
		if (pSoundInfo->wavSound.GetID() == dwID)
        {
            bFound = TRUE;

            hr = pSoundInfo->wavSound.StopAfterThisTime();
            KGLOG_COM_PROCESS_ERROR(hr);

            break;
        }
		pSoundInfo = pSoundInfo->pNext;
	}

    hrResult = S_OK;
Exit0:
#if 0
    if (!bFound)
    {
        KGLogPrintf(KGLOG_INFO, "KSoundShell::StopSoundAfterThisTime(%u)\n", dwID);
    }
#endif
	return hrResult;
}

HRESULT KSoundShell::SetSoundVolume(DWORD dwID, float fVolume)
{
	KWavSoundInfo* pSoundInfo = m_pSound;
	while (pSoundInfo)
	{
		if (pSoundInfo->wavSound.GetID() == dwID)
		{
			pSoundInfo->wavSound.SetOriginalVolume(fVolume);
			pSoundInfo->wavSound.SetVolume(GetFinalVolume(pSoundInfo->wavSound.GetType(), fVolume));
			break;
		}
		pSoundInfo = pSoundInfo->pNext;
	}
	return S_OK;
}

BOOL KSoundShell::IsPlayingSound(DWORD dwID)
{
	KWavSoundInfo* pSoundInfo = m_pSound;
	while (pSoundInfo)
	{
		if (pSoundInfo->wavSound.GetID() == dwID)
			return pSoundInfo->wavSound.IsPlaying();
		pSoundInfo = pSoundInfo->pNext;
	}
	return FALSE;
}

BOOL KSoundShell::IsPlayingSound(LPCSTR FileName)
{
	if (!FileName)
		return FALSE;
	UINT uFileHash = g_FileNameHash(FileName);
	KWavSoundInfo * pSoundInfo = m_pSound;
	while (pSoundInfo)
	{
		if (pSoundInfo->uFileHash == uFileHash && pSoundInfo->strFile == FileName)
			return pSoundInfo->wavSound.IsPlaying();
		pSoundInfo = pSoundInfo->pNext;
	}
	return FALSE;
}

HRESULT KSoundShell::StopAllSound()
{
	KWavSoundInfo * pSoundInfo = m_pSound;
	while (pSoundInfo)
	{
		pSoundInfo->wavSound.Stop();
		pSoundInfo = pSoundInfo->pNext;
	}
	return S_OK;
}

HRESULT KSoundShell::Play3DMusic(int nType, LPCSTR FileName, float fVolume, BOOL bLoop, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity, FLOAT fMinDistance, FLOAT fMaxDistance, DWORD *pdwID)
{
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;

	KG_PROCESS_ERROR(FileName);
	KG_PROCESS_ERROR(!IsLocked() && IsEnable() && IsEnable(nType));

	while (pMusic3DInfo)
	{
		if (!pMusic3DInfo->mp3Music3D.IsPlaying())
		{
			pMusic3DInfo->mp3Music3D.Open((LPSTR)FileName);
			pMusic3DInfo->mp3Music3D.Enable3DSound(m_bEnbale3DSound);
			pMusic3DInfo->mp3Music3D.SetType(nType);
			pMusic3DInfo->mp3Music3D.SetOriginalVolume(fVolume);
			pMusic3DInfo->mp3Music3D.SetVolume(GetFinalVolume(nType, fVolume));
			pMusic3DInfo->mp3Music3D.SetPosition(pvPosition, pvVelocity);
			pMusic3DInfo->mp3Music3D.SetParameters(fMinDistance, fMaxDistance);
			pMusic3DInfo->mp3Music3D.Play(bLoop, FALSE);
			pMusic3DInfo->mp3Music3D.SetID(++m_dwID);
			if (pdwID)
				*pdwID = m_dwID;
			KG_PROCESS_SUCCESS(1);
		}
		pMusic3DInfo = pMusic3DInfo->pNext;
	}
	KG_PROCESS_ERROR(0);
Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KSoundShell::Stop3DMusic(DWORD dwID)
{
    BOOL bFound = FALSE;
	KMp3Music3DInfo* pMusic3DInfo = m_p3DMusic;

	while (pMusic3DInfo)
	{
		if (pMusic3DInfo->mp3Music3D.GetID() == dwID)
		{
            bFound = TRUE;

			pMusic3DInfo->mp3Music3D.Stop();
			break;
		}
		pMusic3DInfo = pMusic3DInfo->pNext;
	}
#if 0
    if (!bFound)
    {
        KGLogPrintf(KGLOG_INFO, "KSoundShell::Stop3DMusic(%u)\n", dwID);
    }
#endif
	return S_OK;
}

HRESULT KSoundShell::Set3DMusicVolume(DWORD dwID, float fVolume)
{
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	while (pMusic3DInfo)
	{
		if (pMusic3DInfo->mp3Music3D.GetID() == dwID)
		{
			pMusic3DInfo->mp3Music3D.SetOriginalVolume(fVolume);
			pMusic3DInfo->mp3Music3D.SetVolume(GetFinalVolume(pMusic3DInfo->mp3Music3D.GetType(), fVolume));
			break;
		}
		pMusic3DInfo = pMusic3DInfo->pNext;
	}
	return S_OK;
}

BOOL KSoundShell::IsPlaying3DMusic(DWORD dwID)
{
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	while (pMusic3DInfo)
	{
		if (pMusic3DInfo->mp3Music3D.GetID() == dwID)
			return pMusic3DInfo->mp3Music3D.IsPlaying();
		pMusic3DInfo = pMusic3DInfo->pNext;
	}
	return FALSE;
}

BOOL KSoundShell::IsPlaying3DMusic(LPCSTR FileName)
{
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	while (pMusic3DInfo)
	{
		if (pMusic3DInfo->strFile == FileName)
			return pMusic3DInfo->mp3Music3D.IsPlaying();
		pMusic3DInfo = pMusic3DInfo->pNext;
	}
	return FALSE;
}

HRESULT	KSoundShell::Set3DMusicPosition(DWORD dwID, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity)
{
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	while (pMusic3DInfo)
	{
		if (pMusic3DInfo->mp3Music3D.GetID() == dwID)
		{
			pMusic3DInfo->mp3Music3D.SetPosition(pvPosition, pvVelocity);
			break;
		}
		pMusic3DInfo = pMusic3DInfo->pNext;
	}
	return S_OK;
}

HRESULT	KSoundShell::Set3DMusicParameters(DWORD dwID, FLOAT fMinDistance, FLOAT fMaxDistance)
{
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	while (pMusic3DInfo)
	{
		if (pMusic3DInfo->mp3Music3D.GetID() == dwID)
		{
			pMusic3DInfo->mp3Music3D.SetParameters(fMinDistance, fMaxDistance);
			break;
		}
		pMusic3DInfo = pMusic3DInfo->pNext;
	}
	return S_OK;
}

HRESULT KSoundShell::StopAll3DMusic()
{
	KMp3Music3DInfo * pMusic3DInfo = m_p3DMusic;
	while (pMusic3DInfo)
	{
		pMusic3DInfo->mp3Music3D.Stop();
		pMusic3DInfo = pMusic3DInfo->pNext;
	}
	return S_OK;
}

HRESULT KSoundShell::Play3DSound(int nType, LPCSTR FileName, float fVolume, BOOL bLoop, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity, FLOAT fMinDistance, FLOAT fMaxDistance, DWORD *pdwID)
{
	KWavSound3DInfo* pSound3DInfoToPlay = NULL;
	KWavSound3DInfo* pSound3DInfo = m_p3DSound;
	KWavSound3DInfo* pLastSameSound3DInfo = NULL;
	KWavSound3DInfo* pLastSound3DInfo = NULL;
	UINT uFileHash = g_FileNameHash(FileName);
	int nPlayCount = 0;
#if 0
	HRESULT hr = S_OK;
#endif
	KG_PROCESS_ERROR(FileName);
	KG_PROCESS_ERROR(!IsLocked() && IsEnable() && IsEnable(nType));
	
	while (pSound3DInfo)
	{
		if (!pLastSound3DInfo || (pSound3DInfo->dwTime < pLastSound3DInfo->dwTime && !pSound3DInfo->wavSound3D.IsLoop()))
			pLastSound3DInfo = pSound3DInfo;
		BOOL bSame = pSound3DInfo->uFileHash == uFileHash && pSound3DInfo->strFile == FileName;
		if (bSame)
		{
			nPlayCount = nPlayCount + 1;
			if (!pLastSameSound3DInfo || (pSound3DInfo->dwTime < pLastSameSound3DInfo->dwTime && !pSound3DInfo->wavSound3D.IsLoop()))
				pLastSameSound3DInfo = pSound3DInfo;
		}

		if (!pSound3DInfo->wavSound3D.IsPlaying())
		{
			if (bSame)
			{
				pSound3DInfoToPlay = pSound3DInfo;
				goto Exit1;
			}
			if (!pSound3DInfoToPlay)
            {
				pSound3DInfoToPlay = pSound3DInfo;
            }
		}
		pSound3DInfo = pSound3DInfo->pNext;
	}

	if (nPlayCount >= KG_MAX_SINGLE_3D_SOUND_COUNT)
	{
		if (pLastSameSound3DInfo)
		{
			pSound3DInfoToPlay = pLastSameSound3DInfo;
			goto Exit1;
		}
	}
	else
	{
		if(!pSound3DInfoToPlay)
			pSound3DInfoToPlay = pLastSound3DInfo;
	}

	KG_PROCESS_ERROR(pSound3DInfoToPlay);
	
#if 0
	hr = pSound3DInfoToPlay->wavSound3D.Load((LPSTR)FileName);
	if (FAILED(hr))
		KGLogPrintf(KGLOG_ERR, "play 3d sound load sound file failed(%s)!", FileName);
#else
	pSound3DInfoToPlay->wavSound3D.Load((LPSTR)FileName);
#endif
	pSound3DInfoToPlay->strFile = FileName;
	pSound3DInfoToPlay->uFileHash = uFileHash;

Exit1:
	pSound3DInfoToPlay->vec3NextPosition = *pvPosition;
	//pSound3DInfoToPlay->vec3CurrentPosition = *pvPosition;
	pSound3DInfoToPlay->bUpdatePosition = FALSE;
	pSound3DInfoToPlay->wavSound3D.Enable3DSound(m_bEnbale3DSound);
	pSound3DInfoToPlay->wavSound3D.SetPosition(pvPosition, pvVelocity, DS3D_IMMEDIATE);
	pSound3DInfoToPlay->wavSound3D.SetParameters(fMinDistance, fMaxDistance);
	pSound3DInfoToPlay->wavSound3D.SetType(nType);
	pSound3DInfoToPlay->wavSound3D.SetOriginalVolume(fVolume);
	pSound3DInfoToPlay->wavSound3D.Play(GetFinalVolume(nType, fVolume), bLoop);
	pSound3DInfoToPlay->wavSound3D.SetID(++m_dwID);
	pSound3DInfoToPlay->dwTime = KG_GetTickCount();
	if (pdwID)
		*pdwID = m_dwID;

	return S_OK;
Exit0:
#if 0
    D3DVECTOR vPosition = { 0.0f, 0.0f, 0.0f };
    D3DVECTOR vVelocity = { 0.0f, 0.0f, 0.0f };

    if (pvPosition)
        vPosition = *pvPosition;
    if (pvVelocity)
        vVelocity = *pvPosition;

    KGLogPrintf(KGLOG_ERR, "Play3DSound(%d, %s, %f, %d, [%f, %f, %f], [%f, %f, %f], %f, %f)\n", 
		nType, FileName ? FileName : "unknowfile", fVolume, bLoop, 
        vPosition.x, vPosition.y, vPosition.z,
        vVelocity.x, vVelocity.y, vVelocity.z,
        fMinDistance, fMaxDistance);
	if (IsLocked())
		KGLogPrintf(KGLOG_ERR, "sound shell is locked!\n");
	if (!IsEnable())
		KGLogPrintf(KGLOG_ERR, "sound shell is not enabled!\n");
	if (!IsEnable(nType))
		KGLogPrintf(KGLOG_ERR, "sound shell is not enabled(%d)!\n", nType);

#endif
	return E_FAIL;
}

HRESULT KSoundShell::Stop3DSound(DWORD dwID)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bFound = FALSE;
    KWavSound3DInfo* pSound3DInfo = m_p3DSound;

	while (pSound3DInfo)
	{
		if (pSound3DInfo->wavSound3D.GetID() == dwID)
        {
            bFound = TRUE;

			hr = pSound3DInfo->wavSound3D.Stop();
            KGLOG_COM_PROCESS_ERROR(hr);
        }
		pSound3DInfo = pSound3DInfo->pNext;
	}

    hrResult = S_OK;
Exit0:
#if 0
    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_INFO, "KSoundShell::Stop3DSound(%u) %s\n", dwID, bFound ? "found" : "not found");
    }
#endif
	return hrResult;
}

HRESULT KSoundShell::Stop3DSoundAfterThisTime(DWORD dwID)
{
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pSound3DInfo)
	{
		if (pSound3DInfo->wavSound3D.GetID() == dwID)
			return pSound3DInfo->wavSound3D.StopAfterThisTime();
		pSound3DInfo = pSound3DInfo->pNext;
	}
	return S_OK;
}

HRESULT KSoundShell::Set3DSoundVolume(DWORD dwID, float fVolume)
{
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pSound3DInfo)
	{
		if (pSound3DInfo->wavSound3D.GetID() == dwID)
		{
			pSound3DInfo->wavSound3D.SetOriginalVolume(fVolume);
			pSound3DInfo->wavSound3D.SetVolume(GetFinalVolume(pSound3DInfo->wavSound3D.GetType(), fVolume));
			break;
		}
		pSound3DInfo = pSound3DInfo->pNext;
	}
	return S_OK;
}

BOOL KSoundShell::IsPlaying3DSound(DWORD dwID)
{
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pSound3DInfo)
	{
		if (pSound3DInfo->wavSound3D.GetID() == dwID)
			return pSound3DInfo->wavSound3D.IsPlaying();
		pSound3DInfo = pSound3DInfo->pNext;
	}
	return FALSE;
}

BOOL KSoundShell::IsPlaying3DSound(LPCSTR FileName)
{
	UINT uFileHash = g_FileNameHash(FileName);
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pSound3DInfo)
	{
		if (pSound3DInfo->uFileHash == uFileHash && pSound3DInfo->strFile == FileName)
			return pSound3DInfo->wavSound3D.IsPlaying();
		pSound3DInfo = pSound3DInfo->pNext;
	}
	return FALSE;
}

HRESULT	KSoundShell::Set3DSoundPosition(DWORD dwID, D3DVECTOR* pvPosition, D3DVECTOR* pvVelocity)
{
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pSound3DInfo)
	{
		if (pSound3DInfo->wavSound3D.GetID() == dwID)
		{
			pSound3DInfo->vec3NextPosition = *pvPosition;
			pSound3DInfo->bUpdatePosition = TRUE;
			break;
		}
		pSound3DInfo = pSound3DInfo->pNext;
	}
	return S_OK;
}

HRESULT	KSoundShell::Set3DSoundParameters(DWORD dwID, FLOAT fMinDistance, FLOAT fMaxDistance)
{
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pSound3DInfo)
	{
		if (pSound3DInfo->wavSound3D.GetID() == dwID)
		{
			pSound3DInfo->wavSound3D.SetParameters(fMinDistance, fMaxDistance);
			break;
		}
		pSound3DInfo = pSound3DInfo->pNext;
	}
	return S_OK;
}

HRESULT KSoundShell::StopAll3DSound()
{
	KWavSound3DInfo * pSound3DInfo = m_p3DSound;
	while (pSound3DInfo)
	{
		pSound3DInfo->wavSound3D.Stop();
		pSound3DInfo = pSound3DInfo->pNext;
	}
	return S_OK;
}

HRESULT KSoundShell::StartRecordSound()
{
	return m_RecordSound.StartRecordSound();
}

HRESULT KSoundShell::StopRecordSound()
{
	return m_RecordSound.StopRecordSound();
}

HRESULT KSoundShell::LockRecordSoundBuffer(void **ppBuffer, DWORD *pSize)
{
	return m_RecordSound.LockRecordSoundBuffer(ppBuffer, pSize);
}

HRESULT KSoundShell::ClearRecordSoundBuffer()
{
	return m_RecordSound.ClearRecordSoundBuffer();
}

HRESULT KSoundShell::UnlockRecordSoundBuffer()
{
	return m_RecordSound.UnlockRecordSoundBuffer();
}

void * KSoundShell::GetRecordSoundWaveFormatEx()
{
	return m_RecordSound.GetWaveFormatEx();
}

extern "C" __declspec(dllexport)
IKG3DSoundShell*  Create3DSoundShell()
{
	if (!g_pSound)
#if defined (USE_DUMMY_SOUND_SHELL)
        g_pSound = new KDummySoundShell;
#elif defined (USE_MULTI_THREAD_SAFE_SOUND_SHELL)
       g_pSound = new KMultiThreadSafeSoundShell;
#else
		g_pSound = new KSoundShell;
#endif
	if (g_pSound)
		g_pSound->AddRef();
	return g_pSound;
}
