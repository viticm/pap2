///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KFellowShipData.cpp
// Creator  :   liuzhibiao
// Date     :   2009-7-10
// Comment  :   v1.0        从数据库中查询角色好友信息
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KFellowShipData.h"

KFellowShipData::KFellowShipData(void)
{

}

KFellowShipData::~KFellowShipData(void)
{
    for (size_t i = 0; i < m_FellowshipArray.size(); i++)
    {
        KG_DELETE(m_FellowshipArray[i]);
    }
    m_FellowshipArray.clear();

    for (size_t i = 0; i < m_FoeArray.size(); i++)
    {
        KG_DELETE(m_FoeArray[i]);
    }
    m_FoeArray.clear();

    for (size_t i = 0; i < m_BlackArray.size(); i++)
    {
        KG_DELETE(m_BlackArray[i]);
    }
    m_BlackArray.clear();
}

int KFellowShipData::PaserFellowShipData(unsigned char* pInbyData, size_t uDataSize)
{
    int                                bResult             = false;
    //int                                bRetCode            = false;
    KG_FELLOWSHIP_DB_V1*                pFellowshipDBv1     = NULL;
    size_t                              uExceptedSize       = 0;
    unsigned char*                               pbyData             = NULL;
    KG_FELLOWSHIP_ENTRY_V1*             pDstFellowshipEntry = NULL;
    KG_FELLOWSHIP_FOE_ENTRY_V1*         pDstFoeEntry        = NULL;
    KG_FELLOWSHIP_BLACKLIST_ENTRY_V1*   pDstBlackListEntry  = NULL;

    KG_PROCESS_ERROR(uDataSize >= sizeof(KG_FELLOWSHIP_DB_V1));

    pFellowshipDBv1 = (KG_FELLOWSHIP_DB_V1*)pInbyData;

    uExceptedSize  = sizeof(KG_FELLOWSHIP_DB_V1);
    uExceptedSize += pFellowshipDBv1->nFriendCount * sizeof(KG_FELLOWSHIP_ENTRY_V1);
    uExceptedSize += pFellowshipDBv1->nFoeCount * sizeof(KG_FELLOWSHIP_FOE_ENTRY_V1);
    uExceptedSize += pFellowshipDBv1->nBlackListCount * sizeof(KG_FELLOWSHIP_BLACKLIST_ENTRY_V1);

    KG_PROCESS_ERROR(uDataSize == uExceptedSize);
    KG_PROCESS_ERROR(pFellowshipDBv1->nFriendCount <= KG_FELLOWSHIP_MAX_RECORD);
    KG_PROCESS_ERROR(pFellowshipDBv1->nFoeCount <= KG_FELLOWSHIP_MAX_FOE_RECORD);
    KG_PROCESS_ERROR(pFellowshipDBv1->nBlackListCount <= KG_FELLOWSHIP_MAX_BLACKLIST_RECORD);

    pbyData = pFellowshipDBv1->byData;

    // Friend
    for (int i = 0; i < pFellowshipDBv1->nFriendCount; i++)
    {
        KG_FELLOWSHIP_ENTRY_V1* pFellowshipEntry = (KG_FELLOWSHIP_ENTRY_V1*)pbyData;

        pDstFellowshipEntry = new KG_FELLOWSHIP_ENTRY_V1();
        KG_PROCESS_ERROR(pDstFellowshipEntry);

        pDstFellowshipEntry->dwAlliedPlayerID = pFellowshipEntry->dwAlliedPlayerID;
        pDstFellowshipEntry->Flags            = pFellowshipEntry->Flags;
        pDstFellowshipEntry->nAttraction      = pFellowshipEntry->nAttraction;

        strncpy(pDstFellowshipEntry->szRemark, pFellowshipEntry->szRemark, sizeof(pDstFellowshipEntry->szRemark));
        pDstFellowshipEntry->szRemark[sizeof(pDstFellowshipEntry->szRemark) - 1] = '\0';
        m_FellowshipArray.push_back(pDstFellowshipEntry);

        pbyData += sizeof(KG_FELLOWSHIP_ENTRY_V1);
    }

    // Foe
    for (int i = 0; i < pFellowshipDBv1->nFoeCount; i++)
    {
        KG_FELLOWSHIP_FOE_ENTRY_V1* pFoeEntry = (KG_FELLOWSHIP_FOE_ENTRY_V1*)pbyData;

        pDstFoeEntry = new KG_FELLOWSHIP_FOE_ENTRY_V1();
        KG_PROCESS_ERROR(pDstFoeEntry);

        pDstFoeEntry->dwAlliedPlayerID = pFoeEntry->dwAlliedPlayerID;

        m_FoeArray.push_back(pDstFoeEntry);

        pbyData += sizeof(KG_FELLOWSHIP_FOE_ENTRY_V1);
    }

    // BlackList
    for (int i = 0; i < pFellowshipDBv1->nBlackListCount; i++)
    {
        KG_FELLOWSHIP_BLACKLIST_ENTRY_V1*   pBlackListEntry = (KG_FELLOWSHIP_BLACKLIST_ENTRY_V1*)pbyData;

        pDstBlackListEntry = new KG_FELLOWSHIP_BLACKLIST_ENTRY_V1();
        KG_PROCESS_ERROR(pDstBlackListEntry);

        pDstBlackListEntry->dwAlliedPlayerID = pBlackListEntry->dwAlliedPlayerID;

        m_BlackArray.push_back(pDstBlackListEntry);

        pbyData += sizeof(KG_FELLOWSHIP_BLACKLIST_ENTRY_V1);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {

        if (pDstBlackListEntry != NULL)
        {
            KG_DELETE(pDstBlackListEntry);
        }

        if (pDstFoeEntry != NULL)
        {
            KG_DELETE(pDstFoeEntry);
        }

        if(pDstFellowshipEntry != NULL)
        {
            KG_DELETE(pDstFellowshipEntry);
        }
        
    }
    return bResult;
}
