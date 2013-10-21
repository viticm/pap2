#ifndef _KPROBABILITY_MANAGER_H_
#define _KPROBABILITY_MANAGER_H_

#include <list>
#include "Global.h"
#include "Engine/KMemory.h"
#include "KLRUCacheMap.h"

#define MAX_RAND_DATA_COUNT 256

enum PROBABILITY_TYPE
{
    // 只能增加，不可更改
    eptInvalid = 0,

    eptQuestDrop,
    eptVenation,

    eptTotal
};

template <typename TKey, DWORD dwMaxProbability, size_t uMaxDataCount>
class KProbability
{
public:
    BOOL Init();
    void UnInit();

#pragma pack(1)
    struct KRandData
    {
        KRandData() : dwFrustration(0), nCount(0) {}
        BOOL Init() { return true; }
        BOOL UnInit() { dwFrustration = 0; nCount = 0; return true; }
        TKey        key;
        DWORD       dwFrustration;
        int         nCount;
    };
#pragma pack()

    struct SAVE_FUNC
    {
        BOOL operator () (TKey key, KRandData RandData)
        {
            memcpy(m_pbyOffset, &RandData, sizeof(KRandData));
            m_pbyOffset += sizeof(KRandData);

            return true;
        }
        BYTE* m_pbyOffset;
    };

    BYTE* Save(BYTE* pbyBuffer, size_t uBufferSize);
    BYTE* Load(BYTE* pbyData, size_t uDataLen);

    BOOL RandomResult(TKey key, DWORD dwProbability, DWORD dwRandomValue);

    typedef KLRUCahceMap<TKey, KRandData, KDefaultUnInitEraseFunc<TKey, KRandData> >    KRAND_MAP;
    KRAND_MAP   m_RandCache;

};

template <typename TKey, DWORD dwMaxProbability, size_t uMaxDataCount>
BOOL KProbability<TKey, dwMaxProbability, uMaxDataCount>::Init()
{
    BOOL    bResult  = false;
    BOOL    bRetCode = false;

    bRetCode = m_RandCache.Init(uMaxDataCount, 1);
    KGLOG_PROCESS_ERROR(bRetCode);
    
    KGLOG_PROCESS_ERROR(dwMaxProbability <= MILLION_NUM);

    bResult = true;
Exit0:
    return bResult;
}

template <typename TKey, DWORD dwMaxProbability, size_t uMaxDataCount>
void KProbability<TKey, dwMaxProbability, uMaxDataCount>::UnInit()
{
    m_RandCache.UnInit();
}

template <typename TKey, DWORD dwMaxProbability, size_t uMaxDataCount>
BOOL KProbability<TKey, dwMaxProbability, uMaxDataCount>::RandomResult(TKey key, DWORD dwProbability, DWORD dwRandomValue)
{
    BOOL                                    bResult   = false;
    KRandData*                              pRandData = NULL;
    DWORD                                   dwRandom  = 0;

    KG_PROCESS_ERROR(dwProbability > 0);
    KG_PROCESS_ERROR(dwRandomValue <=  dwMaxProbability);

    pRandData = m_RandCache.Find(key);
    if (!pRandData)
    {
        pRandData = m_RandCache.Insert(key);
        KGLOG_PROCESS_ERROR(pRandData);

        pRandData->key = key;
    }

    dwRandom = dwRandomValue;
    
    if (dwRandom >= dwProbability)
    {
        pRandData->nCount++;
        pRandData->dwFrustration += dwProbability * 4;

        KG_PROCESS_ERROR(pRandData->dwFrustration >= dwMaxProbability * 10);
    }
    
    pRandData->dwFrustration = 0;
    pRandData->nCount = 0;

    bResult = true;
Exit0:
    return bResult;
}

template <typename TKey, DWORD dwMaxProbability, size_t uMaxDataCount>
BYTE* KProbability<TKey, dwMaxProbability, uMaxDataCount>::Save(BYTE* pbyBuffer, size_t uBufferSize)
{
    BYTE*                   pbyResult       = NULL;
    BYTE*                   pbyOffset       = pbyBuffer;
    size_t                  uDataCount      = 0;
    size_t                  uTotalLen       = 0;
    SAVE_FUNC               SaveFunc;                

    uDataCount = m_RandCache.Size();

    uTotalLen = sizeof(uDataCount) + sizeof(KRandData) * uDataCount;
    KGLOG_PROCESS_ERROR(uBufferSize >= uTotalLen);

    *(size_t*)pbyOffset = uDataCount;
    pbyOffset += sizeof(uDataCount);

    SaveFunc.m_pbyOffset = pbyOffset;
    m_RandCache.Traverse(SaveFunc);

    pbyResult = SaveFunc.m_pbyOffset;
Exit0:
    return pbyResult;
}

template <typename TKey, DWORD dwMaxProbability, size_t uMaxDataCount>
BYTE* KProbability<TKey, dwMaxProbability, uMaxDataCount>::Load(BYTE* pbyData, size_t uDataLen)
{
    BYTE*                                   pbyResult       = NULL;
    BYTE*                                   pbyOffset       = pbyData;
    int                                     nDataCount      = 0;
    size_t                                  uTotalLen       = 0;

    memcpy(&nDataCount, pbyOffset, sizeof(nDataCount));
    pbyOffset += sizeof(nDataCount);

    uTotalLen = sizeof(nDataCount) + sizeof(KRandData) * nDataCount;
    KGLOG_PROCESS_ERROR(uTotalLen <= uDataLen);

    for (int i = 0; i < nDataCount; ++i)
    {
        KRandData*  pRandData = (KRandData*)pbyOffset;

        pRandData = m_RandCache.Insert(pRandData->key);
        KGLOG_PROCESS_ERROR(pRandData);

        memcpy(pRandData, pbyOffset, sizeof(KRandData));

        pbyOffset += sizeof(KRandData);
    }

    pbyResult = pbyOffset;
Exit0:
    return pbyResult;
}
#endif
