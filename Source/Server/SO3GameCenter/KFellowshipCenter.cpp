#include "stdafx.h"
#include "KFellowshipCenter.h"
#include "KSO3GameCenter.h"


// CLASS: KGFellowshipCenter //////////////////////////////////////////////////
BOOL KGFellowshipCenter::Init()
{
    m_CacheRecycleFunc.m_pFellowshipCenter = this;

    m_FellowshipCache.Init(m_CacheRecycleFunc);

    m_lCurrentTimeStamp = time(NULL);
    m_lNextSaveTime = m_lCurrentTimeStamp + g_pSO3GameCenter->m_Settings.m_nFellowshipSaveInterval;

    return true;
}

void KGFellowshipCenter::UnInit()
{
    m_FellowshipCache.UnInit();

    m_lCurrentTimeStamp = 0;
    m_lNextSaveTime     = 0;
}

struct GET_MAX_UPDATE_COUNT 
{
    GET_MAX_UPDATE_COUNT()
    {
        nMaxUpdateCount = 0;
        dwSavePlayerID  = ERROR_ID;
    }

    inline BOOL operator () (DWORD dwPlayerID, IKG_Buffer*& rpiBuffer)
    {
        KG_FELLOWSHIP_CACHE*    pFellowshipCache    = NULL;

        assert(rpiBuffer->GetSize() >= sizeof(KG_FELLOWSHIP_CACHE));
        pFellowshipCache = (KG_FELLOWSHIP_CACHE*)rpiBuffer->GetData();
        assert(pFellowshipCache);

        if (pFellowshipCache->nUpdateCount > nMaxUpdateCount)
        {
            nMaxUpdateCount = pFellowshipCache->nUpdateCount;
            dwSavePlayerID  = dwPlayerID;
        }

        return true;
    }

    int     nMaxUpdateCount;
    DWORD   dwSavePlayerID;
};

void KGFellowshipCenter::Active()
{
    HRESULT     hrResult    = KG_S_FALSE;
    IKG_Buffer* piBuffer    = NULL;

    m_lCurrentTimeStamp = time(NULL);

    if (m_lCurrentTimeStamp >= m_lNextSaveTime)     // 定期保存好友数据
    {
        GET_MAX_UPDATE_COUNT    GetMaxUpdateCount;

        m_FellowshipCache.Traverse(GetMaxUpdateCount);

        if (GetMaxUpdateCount.dwSavePlayerID != ERROR_ID)
        {
            SaveFellowshipData(GetMaxUpdateCount.dwSavePlayerID);
        }

        m_lNextSaveTime = m_lCurrentTimeStamp + g_pSO3GameCenter->m_Settings.m_nFellowshipSaveInterval;
    }

    return;
}

BOOL KGFellowshipCenter::UpdateFellowshipData(DWORD dwPlayerID, size_t uDataLength, BYTE byData[])
{
    BOOL                    bResult             = false;
    int                     nRetCode            = 0;
    int                     nOldUpdateCount     = 0;
    IKG_Buffer*             piBuffer            = NULL;
    IKG_Buffer**            ppiBuffer           = NULL;
    KG_FELLOWSHIP_CACHE*    pFellowshipCache    = NULL;

    assert(dwPlayerID != ERROR_ID);
    assert(uDataLength);
    assert(byData);

    ppiBuffer = m_FellowshipCache.Find(dwPlayerID);
    if (!ppiBuffer)
    {
        ppiBuffer = m_FellowshipCache.Insert(dwPlayerID);
        KGLOG_PROCESS_ERROR(ppiBuffer);
        *ppiBuffer = NULL;
    }

    if (*ppiBuffer)
    {
        size_t uOldSize = (*ppiBuffer)->GetSize();

        assert(uOldSize >= sizeof(KG_FELLOWSHIP_CACHE));
        uOldSize -= sizeof(KG_FELLOWSHIP_CACHE);

        pFellowshipCache = (KG_FELLOWSHIP_CACHE*)(*ppiBuffer)->GetData();
        KGLOG_PROCESS_ERROR(pFellowshipCache);

        if (uOldSize == uDataLength)
        {
            nRetCode = memcmp(pFellowshipCache->byData, byData, uDataLength);
            KG_PROCESS_SUCCESS(nRetCode == 0);
        }

        nOldUpdateCount = pFellowshipCache->nUpdateCount;
        pFellowshipCache = NULL;
    }

    KG_COM_RELEASE(*ppiBuffer);

    piBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KG_FELLOWSHIP_CACHE) + uDataLength));
    KGLOG_PROCESS_ERROR(piBuffer);

    pFellowshipCache = (KG_FELLOWSHIP_CACHE*)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pFellowshipCache);

    pFellowshipCache->nUpdateCount = nOldUpdateCount + 1;
    memcpy(pFellowshipCache->byData, byData, uDataLength);

    *ppiBuffer = piBuffer;
    piBuffer = NULL;

Exit1:
    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

BOOL KGFellowshipCenter::GetFellowshipData(DWORD dwPlayerID)
{
    BOOL                    bResult             = false;
    IKG_Buffer**            ppiBuffer           = NULL;
    KG_FELLOWSHIP_CACHE*    pFellowshipCache    = NULL;
    size_t                  uDataLength         = 0;

    assert(dwPlayerID != ERROR_ID);

    ppiBuffer = m_FellowshipCache.Find(dwPlayerID);
    if (!ppiBuffer)
    {
        g_pSO3GameCenter->m_MiscDB.DoLoadFellowshipData(dwPlayerID);
        goto Exit1;
    }

    uDataLength = (*ppiBuffer)->GetSize();
    assert(uDataLength >= sizeof(KG_FELLOWSHIP_CACHE));

    uDataLength -= sizeof(KG_FELLOWSHIP_CACHE);

    pFellowshipCache = (KG_FELLOWSHIP_CACHE*)(*ppiBuffer)->GetData();
    KGLOG_PROCESS_ERROR(pFellowshipCache);

    g_pSO3GameCenter->m_GameServer.DoApplyFellowshipDataRespond(
        dwPlayerID, uDataLength, pFellowshipCache->byData
    );

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

//////////////////////////////////////////////////////////////////////////

BOOL KGFellowshipCenter::SaveFellowshipData(DWORD dwPlayerID)
{
    BOOL                            bResult         = false;
    BOOL                            bRetCode        = false;
    size_t                          uDataLength     = 0;
    IKG_Buffer*                     piBuffer        = NULL;
    IKG_Buffer**                    ppiBuffer       = NULL;
    KG_FELLOWSHIP_CACHE*            pFellowCache    = NULL;
    unsigned long                   ulKey           = 0;

    ppiBuffer = m_FellowshipCache.Find(dwPlayerID);
    KGLOG_PROCESS_ERROR(ppiBuffer);

    assert(*ppiBuffer);

    uDataLength = (*ppiBuffer)->GetSize();

    assert(uDataLength >= sizeof(KG_FELLOWSHIP_CACHE));

    pFellowCache = (KG_FELLOWSHIP_CACHE*)(*ppiBuffer)->GetData();
    KGLOG_PROCESS_ERROR(pFellowCache);

    uDataLength -= sizeof(KG_FELLOWSHIP_CACHE);
    
    bRetCode = g_pSO3GameCenter->m_MiscDB.DoSaveFellowshipData(dwPlayerID, pFellowCache->byData, uDataLength);
    KGLOG_PROCESS_ERROR(bRetCode);

    pFellowCache->nUpdateCount = 0;

    bResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return bResult;
}

struct FellowshipSaveFunc
{
    FellowshipSaveFunc() : m_pFellowshipCenter(NULL) {};

    inline bool operator() (DWORD dwPlayerID, IKG_Buffer*& rpiBuffer)
    {
        KG_FELLOWSHIP_CACHE*    pFellowshipCache    = NULL;

        assert(m_pFellowshipCenter);

        assert(rpiBuffer->GetSize() >= sizeof(KG_FELLOWSHIP_CACHE));
        pFellowshipCache = (KG_FELLOWSHIP_CACHE*)rpiBuffer->GetData();
        assert(pFellowshipCache);

        if (pFellowshipCache->nUpdateCount)
        {
            m_pFellowshipCenter->SaveFellowshipData(dwPlayerID);
            pFellowshipCache->nUpdateCount = 0;
        }

        return true;
    }

    KGFellowshipCenter* m_pFellowshipCenter;
};

void KGFellowshipCenter::SaveAllFellowshipData()
{
    FellowshipSaveFunc SaveFunc;

    SaveFunc.m_pFellowshipCenter = this;

    m_FellowshipCache.Traverse(SaveFunc);
}

void KGFellowshipCenter::OnLoadFellowshipData(DWORD dwPlayerID, BYTE* pbyFellowshipData, size_t uDataSize)
{
    BOOL                            bResult             = false;
    BOOL                            bRetCode            = false;
    KG_FELLOWSHIP_CACHE*            pFellowshipCache    = NULL;
    IKG_Buffer*                     piFellowBuffer      = NULL;
    IKG_Buffer**                    ppiBuffer           = NULL;

    ppiBuffer = m_FellowshipCache.Insert(dwPlayerID);
    KGLOG_PROCESS_ERROR(ppiBuffer);

    piFellowBuffer = KG_MemoryCreateBuffer((unsigned)(sizeof(KG_FELLOWSHIP_CACHE) + uDataSize));
    KGLOG_PROCESS_ERROR(piFellowBuffer);

    pFellowshipCache = (KG_FELLOWSHIP_CACHE*)piFellowBuffer->GetData();
    KGLOG_PROCESS_ERROR(pFellowshipCache);

    pFellowshipCache->nUpdateCount = 0;

    memcpy(pFellowshipCache->byData, pbyFellowshipData, uDataSize);

    *ppiBuffer = piFellowBuffer;
    piFellowBuffer = NULL;

    // 将数据发送到Game server。 
    g_pSO3GameCenter->m_GameServer.DoApplyFellowshipDataRespond(dwPlayerID, uDataSize, pbyFellowshipData);

Exit0:
    KG_COM_RELEASE(piFellowBuffer);
    return;
}
