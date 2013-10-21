////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KFellowshipCenter.h
//  Version     : 1.0
//  Creator     : Lin Jiaqi
//  Create Date : 2007-07-19 15:04:58
//  Comment     : 
//                  Reconstructed by Jeffrey Chen, 2007-11-29. 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _FELLOWSHIP_CENTER_H_
#define _FELLOWSHIP_CENTER_H_

#include <map>
#include <set>
#include <deque>
#include <vector>
#include <algorithm>
#include "KGameServer.h"
#include "KLRUCacheMap.h"

// KGFellowshipCenter is just a fellowship data cache between game sever and database. 
// It does not understand the data structure of fellowship and does not responsible for the consistency. 

#define KG_FELLOWSHIP_DB_DATA_VERSION   1


struct KG_FELLOWSHIP_CACHE 
{
    int     nUpdateCount;
    BYTE    byData[0];
};
    
class KGFellowshipCenter
{
public:
    KGFellowshipCenter()
    {
        m_lCurrentTimeStamp = 0;
        m_lNextSaveTime     = 0;
    };

    ~KGFellowshipCenter() {};

    BOOL    Init();
    void    UnInit();
    void    Active();

    BOOL    UpdateFellowshipData(DWORD dwPlayerID, size_t uDataLength, BYTE byData[]);
    BOOL    GetFellowshipData(DWORD dwPlayerID);
    BOOL    SaveFellowshipData(DWORD dwPlayerID);

    void    SaveAllFellowshipData();

    void    OnLoadFellowshipData(DWORD dwPlayerID, BYTE* pbyFellowshipData, size_t uDataSize);

private:
    struct OnFellowshipRecycleFunc
    {
        OnFellowshipRecycleFunc() : m_pFellowshipCenter(NULL) {};

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

            KG_COM_RELEASE(rpiBuffer);

            return true;
        }

        KGFellowshipCenter* m_pFellowshipCenter;
    } m_CacheRecycleFunc;

    KLRUCahceMap<DWORD, IKG_Buffer*, OnFellowshipRecycleFunc> m_FellowshipCache;

    time_t  m_lCurrentTimeStamp;
    time_t  m_lNextSaveTime;
};

#endif	//_FELLOWSHIP_CENTER_H_
//////////////////////////////////////////////////////////////////////////
