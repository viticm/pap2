#include "StdAfx.h"
#include "KG3DClipTable.h"
#include "KG3DAnimationTagContainer.h"
#include "KG3DEngineManager.h"
#include "KG3DAnimationMotionTag.h"
#include "KG3DAnimationSFXTag.h"
#include "KG3DAnimationEventHandlerRepresentDummy.h"
#include "KG3DGraphicsTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DClipTable g_cClipTable;
KG3DClipPrecompute g_ClipPrecompute;

extern int g_nExpireTime;
extern int g_nAniLimitMemSize;

const char DEFAULT_ANI[] = "data/public/DefaultAniList.ini";

extern KG3DFaceMotionDefaultHandler* g_pFaceMotionDefaultHandler;
extern KG3DFaceMotionLifeRangeHandler* g_pLifeRangeHandler;
extern KG3DAnimationEventHandlerRepresentDummy* g_pRepresentEventHandlerDummy;
extern KG3DAnimationEventHandlerRepresentDummy* s_pDummy;

KG3DClipTable::KG3DClipTable(void)
{
    m_dwExpireTime = 0;
    m_uLimitMemSize = 0;
}

KG3DClipTable::~KG3DClipTable(void)
{
    ASSERT(m_ClipResource.empty());
    ASSERT(m_lpEventCenter == NULL);
    ASSERT(g_pFaceMotionDefaultHandler == NULL);
    ASSERT(g_pLifeRangeHandler == NULL);
    ASSERT(m_lpKG3DAnimationTagSFXMotionSwithEventHandeler == NULL);
    ASSERT(g_pRepresentEventHandlerDummy == NULL);
    ASSERT(s_pDummy == NULL);
    ASSERT(m_lpKG3DSFXTagSceneWorldPosEventHandeler == NULL);
}

HRESULT KG3DClipTable::Init()
{
    HRESULT hrResult = E_FAIL;
    //HRESULT hrRetCode = E_FAIL;

    m_dwExpireTime = (DWORD)(g_nExpireTime * 1000 * 60);
    m_uLimitMemSize = (unsigned)(g_nAniLimitMemSize * 1024 * 1024);

    m_lpEventCenter = new KG3DEventCenter;
    KGLOG_PROCESS_ERROR(m_lpEventCenter);

    g_pFaceMotionDefaultHandler = new KG3DFaceMotionDefaultHandler();//确保这两个回调在 m_lpEventCenter之后创建
    KGLOG_PROCESS_ERROR(g_pFaceMotionDefaultHandler);

    g_pLifeRangeHandler = new KG3DFaceMotionLifeRangeHandler();
    KGLOG_PROCESS_ERROR(g_pLifeRangeHandler);

    m_lpKG3DAnimationTagSFXMotionSwithEventHandeler = new KG3DAnimationTagSFXMotionSwithEventHandeler;
    KGLOG_PROCESS_ERROR(m_lpKG3DAnimationTagSFXMotionSwithEventHandeler);

    g_pRepresentEventHandlerDummy = new KG3DAnimationEventHandlerRepresentDummy;
    KGLOG_PROCESS_ERROR(g_pRepresentEventHandlerDummy);

    s_pDummy = new KG3DAnimationEventHandlerRepresentDummy;
    KGLOG_PROCESS_ERROR(s_pDummy);

    m_lpKG3DSFXTagSceneWorldPosEventHandeler = new KG3DAnimationSFXTagSceneLifeRangeEventHandeler;
    KGLOG_PROCESS_ERROR(m_lpKG3DSFXTagSceneWorldPosEventHandeler);

    //hrRetCode = LoadDefaultAnimation("Common");
    //KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE(m_lpKG3DSFXTagSceneWorldPosEventHandeler);
        KG_DELETE(s_pDummy);
        KG_DELETE(g_pRepresentEventHandlerDummy);
        KG_DELETE(m_lpKG3DAnimationTagSFXMotionSwithEventHandeler);
        KG_DELETE(g_pLifeRangeHandler);
        KG_DELETE(g_pFaceMotionDefaultHandler);
        KG_DELETE(m_lpEventCenter);
    }
    return hrResult;
}

HRESULT KG3DClipTable::UnInit()
{
    CLIP_MAP::iterator it = m_ClipResource.begin();
    while (it != m_ClipResource.end())
    {
        KG3DClip *pClip = it->second;
        ASSERT(pClip);

        VERIFY_RELEASE(pClip);

        ++it;
    }
    m_ClipResource.clear();

    KG_DELETE(m_lpKG3DSFXTagSceneWorldPosEventHandeler);
    KG_DELETE(s_pDummy);
    KG_DELETE(g_pRepresentEventHandlerDummy);
    KG_DELETE(m_lpKG3DAnimationTagSFXMotionSwithEventHandeler);
    KG_DELETE(g_pLifeRangeHandler);
    KG_DELETE(g_pFaceMotionDefaultHandler);
    KG_DELETE(m_lpEventCenter);

    return S_OK;
}

HRESULT KG3DClipTable::LoadResourceFromFileInEditor(const char cszFileName[], unsigned uFileNameHash, unsigned uLoadOption, IEKG3DClip **ppiRetResource)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    KG_ASSERT_EXIT(cszFileName);
    KG_ASSERT_EXIT(ppiRetResource);

    hrRetCode = LoadResourceFromFile(cszFileName, uFileNameHash, uLoadOption, (KG3DClip **)ppiRetResource);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DClipTable::LoadResourceFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uLoadOption, KG3DClip **ppClip)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwID = 0;
    KG3DClip *pClip = NULL;
    CLIP_MAP::iterator it;
    CLIP_MAP::_Pairib Pair;

    m_Lock.Lock();

    KG_ASSERT_EXIT(cszFileName);
    KG_ASSERT_EXIT(ppClip);

    *ppClip = NULL;

    dwID = g_FileNameHash(cszFileName);

    it = m_ClipResource.find(dwID);
    if (it != m_ClipResource.end())
    {
        pClip = it->second;
        ASSERT(pClip);
        pClip->AddRef();
    }
    else
    {
        pClip = new KG3DClip;
        KGLOG_PROCESS_ERROR(pClip);

        hrRetCode = pClip->LoadFromFile(cszFileName, uLoadOption);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        pClip->SetID(dwID);

        Pair =  m_ClipResource.insert(make_pair(dwID, pClip));
        KGLOG_PROCESS_ERROR(Pair.second);

        Pair.first->second->AddRef();
    }

    *ppClip = pClip;
    (*ppClip)->AddRef();

    hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(pClip);
   
    m_Lock.Unlock();
    return hrResult;
}

HRESULT KG3DClipTable::ReleaseResourceLRU()
{
    HRESULT hrResult = E_FAIL;
    DWORD dwLastUseTime = 0;
    ULONG ulRef = 0;
    unsigned uTotalMemSize = 0;
    DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
    CLIP_MAP::iterator it;

    m_Lock.Lock();

    it = m_ClipResource.begin();
    while (it != m_ClipResource.end())
    {
        KG3DClip *pClip = it->second;
        ASSERT(pClip);

        uTotalMemSize += pClip->GetMemSize();

        ++it;
    }

    it = m_ClipResource.begin();
    while (it != m_ClipResource.end())
    {
        KG3DClip *pClip = it->second;
        ASSERT(pClip);
        dwLastUseTime = pClip->GetLastUseTime();
        ulRef = QueryRef(pClip);
        if (
            (ulRef == 1) && 
            (
                (dwNowTime - dwLastUseTime > m_dwExpireTime) ||
                (uTotalMemSize > m_uLimitMemSize)
            )
        )
        {
            KG_COM_RELEASE(pClip);

            it = m_ClipResource.erase(it);
        }
        else
        {
            ++it;
        }
    }

    hrResult = S_OK;
//Exit0:

    m_Lock.Unlock();
    return hrResult;
}

HRESULT KG3DClipTable::ReportResourceUse()
{
    HRESULT hrResult = E_FAIL;

    m_Lock.Lock();
	
    KGLogPrintf(KGLOG_INFO, "=======================================================");
    
    CLIP_MAP::iterator it = m_ClipResource.begin();
    while (it != m_ClipResource.end())
    {
        KG3DClip *pClip = it->second;
        ASSERT(pClip);

        KGLogPrintf(KGLOG_INFO, "ID=%u ref=%u %s", pClip->GetID(), QueryRef(pClip), pClip->m_scName.c_str());

        ++it;
    }

    KGLogPrintf(KGLOG_INFO, "ClipTable Count: %u", m_ClipResource.size());

    hrResult = S_OK;
//Exit0:
    m_Lock.Unlock();
	return hrResult;
}

HRESULT KG3DClipTable::LoadDefaultAnimation(const char cszSectionName[])
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int nRetCode = false;    
    IIniFile *piIniFile = NULL;
    int nAniNum = 0;
    char szAniFileName[MAX_PATH];
    char szKeyName[16];
    KG3DClip *pClip = NULL;

    KG_ASSERT_EXIT(cszSectionName);

    piIniFile = g_OpenIniFile(DEFAULT_ANI);
    KGLOG_PROCESS_ERROR(piIniFile);

    nRetCode = piIniFile->GetInteger(cszSectionName, "Num", 0, &nAniNum);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < nAniNum; i++)
    {
        nRetCode = snprintf(szKeyName, sizeof(szKeyName), "%d", i + 1);
        szKeyName[sizeof(szKeyName) - 1] = '\0';

        nRetCode = piIniFile->GetString(cszSectionName, szKeyName, "", szAniFileName, sizeof(szAniFileName));
        KGLOG_PROCESS_ERROR(nRetCode);

        KG_COM_RELEASE(pClip);

        hrRetCode = LoadResourceFromFile(szAniFileName, 0, 0, &pClip);
        KG_COM_PROCESS_ERROR(hrRetCode);
    }

    hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(pClip);
    KG_COM_RELEASE(piIniFile);
    return hrResult;
}

//////////////////////////////////////////////////////////////////////////
KG3DClipPrecompute::KG3DClipPrecompute()
{
    m_nExitFlag = false;
}

KG3DClipPrecompute::~KG3DClipPrecompute()
{
}

HRESULT KG3DClipPrecompute::Init()
{
    HRESULT hrResult = E_FAIL;
    int nRetCode = false;
	int nWorkThreadCreateFlag = false;

    nRetCode = m_WorkThread.Create(_WorkThread, this);
    KGLOG_PROCESS_ERROR(nRetCode);
	nWorkThreadCreateFlag = true;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult) && nWorkThreadCreateFlag)
    {
        m_nExitFlag = true;
        m_WorkThread.Destroy();
		nWorkThreadCreateFlag = false;
    }
    return hrResult;
}

HRESULT KG3DClipPrecompute::UnInit()
{
    m_nExitFlag = true;
    m_WorkThread.Destroy();
    m_ClipQueue.clear();

    return S_OK;
}

void KG3DClipPrecompute::_WorkThread(void *pvThis)
{
    ASSERT(pvThis);
    ((KG3DClipPrecompute *)pvThis)->_ThreadFunction();
}

int KG3DClipPrecompute::_ThreadFunction()
{
    int nResult  = false;
    HRESULT hrRetCode = E_FAIL;
    KG3DClip *pClip = NULL;

    while (!m_nExitFlag)
    {
        ::Sleep(1);

        pClip = NULL;

        m_Lock.Lock();
        if (!m_ClipQueue.empty())
        {
            pClip = m_ClipQueue.front();
            m_ClipQueue.pop_front();
        }    
        m_Lock.Unlock();

        if (!pClip)
            continue;

        hrRetCode = pClip->LoadFromFileMultiThread();
        //KG_COM_PROCESS_ERROR(hrRetCode);

        pClip->m_bInMultiThreadLoadList = FALSE;
    }

    nResult = true;
//Exit0:
    return nResult;
}

HRESULT KG3DClipPrecompute::InsertClip(KG3DClip *pClip)
{
    HRESULT hrResult = false;

    m_Lock.Lock();

    KG_PROCESS_ERROR(pClip);

    m_ClipQueue.push_back(pClip);

    hrResult = S_OK;
Exit0:
    m_Lock.Unlock();
    return hrResult;
}
