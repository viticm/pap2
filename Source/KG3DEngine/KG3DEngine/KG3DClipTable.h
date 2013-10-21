#pragma once

#include "IEKG3DClipTable.h"
#include "KG3DClip.h"
#include "KMutex.h"

class KG3DAnimationTagContainerTable;
class KG3DEventCenter;
class KG3DAnimationTagSFXMotionSwithEventHandeler;
class KG3DAnimationSFXTagSceneLifeRangeEventHandeler;

class KG3DClipTable : public IEKG3DClipTable
{
public:
	KG3DClipTable(void);
	~KG3DClipTable(void);

	HRESULT Init();
    HRESULT UnInit();

    void SetExpireTime(DWORD dwExpireTime) { m_dwExpireTime = dwExpireTime; }
    void SetLimitMemSize(unsigned dwLimitMemSize) { m_uLimitMemSize = dwLimitMemSize; }

	virtual HRESULT LoadResourceFromFileInEditor(const char cszFileName[], unsigned uFileNameHash, unsigned uLoadOption, IEKG3DClip **ppiRetResource);

    DWORD GetResourceCount() { return (DWORD)(m_ClipResource.size()); }
    HRESULT ReportResourceUse();

    HRESULT LoadResourceFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uLoadOption, KG3DClip **ppClip);
    HRESULT ReleaseResourceLRU();

    HRESULT LoadDefaultAnimation(const char cszSectionName[]);

	KG3DAnimationTagSFXMotionSwithEventHandeler* m_lpKG3DAnimationTagSFXMotionSwithEventHandeler;
	KG3DAnimationSFXTagSceneLifeRangeEventHandeler *m_lpKG3DSFXTagSceneWorldPosEventHandeler;
	KG3DEventCenter* m_lpEventCenter;
	KG3DEventCenter* GetEventCenter() { return m_lpEventCenter; }

private:
    typedef map<DWORD, KG3DClip *> CLIP_MAP;
    CLIP_MAP m_ClipResource;
    KMutex m_Lock;
    DWORD m_dwExpireTime;
    unsigned m_uLimitMemSize;
};

extern KG3DClipTable g_cClipTable;

class KG3DClipPrecompute
{
public:
    KG3DClipPrecompute();
    ~KG3DClipPrecompute();

    HRESULT Init();
    HRESULT UnInit();
    HRESULT InsertClip(KG3DClip *pClip);

private:
    static void _WorkThread(void *pvThis);
    int _ThreadFunction();

private:
    typedef std::deque<KG3DClip *> CLIP_DEQUE;
    CLIP_DEQUE m_ClipQueue;
    KMutex m_Lock;
    KThread m_WorkThread;
    int m_nExitFlag;
};

extern KG3DClipPrecompute g_ClipPrecompute;