///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KTongData.h
// Creator  :   liuzhibiao
// Date     :   2009-7-27
// Comment  :   v1.0        从数据库中查询帮会信息
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KTongData.h"

KTongData::KTongData(void)
{
    memset(m_TongRepertoryPage, 0, sizeof(m_TongRepertoryPage));
    memset(m_szAnnouncement, '\0', sizeof(m_szAnnouncement));
    memset(m_szOnlineMessage, '\0', sizeof(m_szOnlineMessage));
    memset(m_szIntroduction, '\0', sizeof(m_szIntroduction));
    memset(m_szRules, '\0', sizeof(m_szRules));
}

KTongData::~KTongData(void)
{
    for (size_t i = 0; i < m_Memberlist.size(); i++)
    {
        KG_DELETE(m_Memberlist[i]);
    }
    m_Memberlist.clear();
}

int KTongData::PaserTongDataInfo(unsigned char* pbyData, size_t uDataLen)
{
    int                 bResult     = false;
    int                 bRetCode    = false;
    KTONG_DB_HEADER*    pHeader     = (KTONG_DB_HEADER*)pbyData;
    unsigned char*      pbyOffSet   = pbyData;
    size_t              uLeftSize   = uDataLen;

    assert(pbyData);

    KG_PROCESS_ERROR(uDataLen >= sizeof(KTONG_DB_HEADER));
    KG_PROCESS_ERROR(pHeader->nVersion <= TONG_DB_VERSION);

    pbyOffSet += sizeof(KTONG_DB_HEADER);
    uLeftSize -= sizeof(KTONG_DB_HEADER);

    while (uLeftSize)
    {
        KJX3TONG_DB_SECTION* pSection = (KJX3TONG_DB_SECTION*)pbyOffSet;
        pbyOffSet += sizeof(KJX3TONG_DB_SECTION);
        uLeftSize -= sizeof(KJX3TONG_DB_SECTION);

        KG_PROCESS_ERROR(uLeftSize >= pSection->uDataLen);

        switch (pSection->nType)
        {
        case eTongBaseInfo:
            if (pHeader->nVersion == 1)
            {
                bRetCode = LoadBaseInfoVersion1(pSection->ucData, pSection->uDataLen);
                KG_PROCESS_ERROR(bRetCode);
            }
            else
            {
                KG_PROCESS_ERROR(pHeader->nVersion == TONG_DB_VERSION);
                bRetCode = LoadBaseInfo(pSection->ucData, pSection->uDataLen);
                KG_PROCESS_ERROR(bRetCode);
            }
            break;

        case eTongSchema:
            bRetCode = LoadSchema(pSection->ucData, pSection->uDataLen);
            KG_PROCESS_ERROR(bRetCode);
            break;

        case eTongRoster:
            if (pHeader->nVersion == 1)
            {
                bRetCode = LoadRosterVersion1(pSection->ucData, pSection->uDataLen);
            }
            else
            {
                KG_PROCESS_ERROR(pHeader->nVersion == TONG_DB_VERSION);
                bRetCode = LoadRoster(pSection->ucData, pSection->uDataLen);
            }
            KG_PROCESS_ERROR(bRetCode);
            break;

        case eTongAnnouncement:
            strncpy(m_szAnnouncement, (const char*)pSection->ucData, sizeof(m_szAnnouncement));
            m_szAnnouncement[sizeof(m_szAnnouncement) - 1] = '\0';
            break;

        case eTongOnlineMessage:
            strncpy(m_szOnlineMessage, (const char*)pSection->ucData, sizeof(m_szOnlineMessage));
            m_szOnlineMessage[sizeof(m_szOnlineMessage) - 1] = '\0';
            break;

        case eTongIntroduction:
            strncpy(m_szIntroduction, (const char*)pSection->ucData, sizeof(m_szIntroduction));
            m_szIntroduction[sizeof(m_szIntroduction) - 1] = '\0';
            break;

        case eTongRules:
            strncpy(m_szRules, (const char*)pSection->ucData, sizeof(m_szRules));
            m_szRules[sizeof(m_szRules) - 1] = '\0';
            break;

        case eTongMemorabilia:
             bRetCode = LoadMemorabilia(pSection->ucData, pSection->uDataLen);
             KG_PROCESS_ERROR(bRetCode);
            break;

        case eTongRepertory:
            bRetCode = LoadRepertory(pSection->ucData, pSection->uDataLen);
            KG_PROCESS_ERROR(bRetCode);
            break;

        case eTongDevelopmentInfo:
            bRetCode = LoadDevelopmentInfo(pSection->ucData, pSection->uDataLen);
            KG_PROCESS_ERROR(bRetCode);
            break;
        
        case eTongHistory:
            
            break;
        default:
            //KGLogPrintf(KGLOG_ERR, "[KTong] DB error, can not recognize section type\n");
            //goto Exit0;
            ASSERT(true);
            break;
        }

        pbyOffSet += pSection->uDataLen;
        uLeftSize -= pSection->uDataLen;
    }

    bResult = true;
Exit0:
    return bResult;
}

int KTongData::LoadBaseInfoVersion1(unsigned char* pbyData, size_t uDataLen)
{
    int                                   nResult     = false;
    KTONG_DB_BASEINFO_VERSION1_ANALYSER*  pBaseInfo   = NULL;

    assert(pbyData);
    KG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DB_BASEINFO_VERSION1_ANALYSER));

    pBaseInfo   = (KTONG_DB_BASEINFO_VERSION1_ANALYSER*)pbyData;

    m_TongBaseInfo.dwMaster          = pBaseInfo->dwMaster;
    m_TongBaseInfo.nFund             = pBaseInfo->nFund;
    m_TongBaseInfo.nLevel            = pBaseInfo->nLevel;
    m_TongBaseInfo.nDevelopmentPoint = pBaseInfo->nDevelopmentPoint;
    m_TongBaseInfo.wMaxMemberCount   = pBaseInfo->wMaxMemberCount;
    m_TongBaseInfo.byTongState       = pBaseInfo->byTongState;
    m_TongBaseInfo.nStateTimer       = pBaseInfo->nStateTimer;
    m_TongBaseInfo.byCamp            = pBaseInfo->byCamp;

    strncpy(m_TongBaseInfo.szName, pBaseInfo->szName, sizeof(m_TongBaseInfo.szName));
    m_TongBaseInfo.szName[sizeof(m_TongBaseInfo.szName) - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

int KTongData::LoadBaseInfo(unsigned char* pbyData, size_t uDataLen)
{
    int                         nResult     = false;
    KTONG_DB_BASEINFO_ANALYSER* pBaseInfo   = NULL;

    assert(pbyData);
    KG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DB_BASEINFO_ANALYSER));

    pBaseInfo           = (KTONG_DB_BASEINFO_ANALYSER*)pbyData;

    m_TongBaseInfo.dwMaster          = pBaseInfo->dwMaster;
    m_TongBaseInfo.nLevel            = pBaseInfo->nLevel;
    m_TongBaseInfo.wMaxMemberCount   = pBaseInfo->wMaxMemberCount;
    m_TongBaseInfo.byTongState       = pBaseInfo->byTongState;
    m_TongBaseInfo.nStateTimer       = pBaseInfo->nStateTimer;
    m_TongBaseInfo.byCamp            = pBaseInfo->byCamp;

    strncpy(m_TongBaseInfo.szName, pBaseInfo->szName, sizeof(m_TongBaseInfo.szName));
    m_TongBaseInfo.szName[sizeof(m_TongBaseInfo.szName) - 1] = '\0';

    nResult = true;
Exit0:
    return nResult;
}

int KTongData::LoadSchema(unsigned char* pbyData, size_t uDataLen)
{
    int                 bResult         = false;
    KJX3TONG_DB_SCHEMA* pScheamSection  = NULL;

    assert(pbyData);
    KG_PROCESS_ERROR(uDataLen == sizeof(KJX3TONG_DB_SCHEMA));

    pScheamSection = (KJX3TONG_DB_SCHEMA*)pbyData;

    memcpy(&m_SchemaData, pScheamSection, sizeof(KTongSchemaData));

    for (int nGroupIndex = 0; nGroupIndex < MAX_TONG_GROUP; nGroupIndex++)
    {
        m_SchemaData.Data[nGroupIndex].szName[sizeof(m_SchemaData.Data[nGroupIndex].szName) - 1] = '\0';
    }

    bResult = true;
Exit0:
    return bResult;

}

int KTongData::LoadRosterVersion1(unsigned char* pbyData, size_t uDataLen)
{
    int                                                 bResult         = false;
    KTONG_DB_ROSTER*                                    pRosterSection  = NULL;
    int                                                 nMemberCount    = 0;
    KTONG_DB_MEMBER_DATA_VERSION1_AnanlyserCompatible*  pMemberData     = NULL;
    KTongMember_NowUse*                                 pMember         = NULL;

    assert(pbyData);
    KG_PROCESS_ERROR(uDataLen >= sizeof(KTONG_DB_ROSTER));

    pRosterSection = (KTONG_DB_ROSTER*)pbyData;
    nMemberCount = pRosterSection->nMemberCount;

    KG_PROCESS_ERROR(nMemberCount >= 0);
    KG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DB_ROSTER) + sizeof(KTONG_DB_MEMBER_DATA_VERSION1_AnanlyserCompatible) * nMemberCount);

    pMemberData = (KTONG_DB_MEMBER_DATA_VERSION1_AnanlyserCompatible*)pRosterSection->byData;

    for (int i = 0; i < nMemberCount; i++)
    {
        pMember = new KTongMember_NowUse;
        KG_PROCESS_ERROR(pMember);

        pMember->dwID             = pMemberData[i].dwID;
        pMember->nGroupID         = pMemberData[i].nGroupID;
        pMember->nJoinTime        = pMemberData[i].nJoinTime;
        pMember->nLastOfflineTime = pMemberData[i].nLastOfflineTime;
        pMember->nSalary          = 0;

        pMember->bIsOnline        = false;

        pMember->nLastUpdateFrame = 0;

        strncpy(pMember->szRemark, pMemberData[i].szRemark, sizeof(pMember->szRemark));
        pMember->szRemark[sizeof(pMember->szRemark) - 1] = '\0';

        m_Memberlist.push_back(pMember);
    }

    bResult = true;
Exit0:
    return bResult;
}

int KTongData::LoadRoster(unsigned char* pbyData, size_t uDataLen)
{
    int                             bResult         = false;
    KJX3TONG_DB_ROSTER*             pRosterSection  = NULL;
    int                             nMemberCount    = 0;
    KTONG_DB_MEMBER_DATA_NOW_USE*   pMemberData     = NULL;
    KTongMember_NowUse*             pMember         = NULL;

    assert(pbyData);
    KG_PROCESS_ERROR(uDataLen >= sizeof(KJX3TONG_DB_ROSTER));

    pRosterSection = (KJX3TONG_DB_ROSTER*)pbyData;
    nMemberCount = pRosterSection->nMemberCount;

    KG_PROCESS_ERROR(nMemberCount >= 0);
    KG_PROCESS_ERROR(uDataLen == sizeof(KJX3TONG_DB_ROSTER) + sizeof(KTONG_DB_MEMBER_DATA_NOW_USE) * nMemberCount);

    pMemberData = (KTONG_DB_MEMBER_DATA_NOW_USE*)pRosterSection->byData;

    for (int i = 0; i < nMemberCount; i++)
    {
        pMember = new KTongMember_NowUse;
        KG_PROCESS_ERROR(pMember);

        pMember->dwID             = pMemberData[i].dwID;
        pMember->nGroupID         = pMemberData[i].nGroupID;
        pMember->nJoinTime        = pMemberData[i].nJoinTime;
        pMember->nLastOfflineTime = pMemberData[i].nLastOfflineTime;
        pMember->nSalary          = pMemberData[i].nSalary;
        pMember->bIsOnline        = false;

        pMember->nLastUpdateFrame = 0;

        strncpy(pMember->szRemark, pMemberData[i].szRemark, sizeof(pMember->szRemark));
        pMember->szRemark[sizeof(pMember->szRemark) - 1] = '\0';
        
        m_Memberlist.push_back(pMember);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        KG_DELETE(pMember);
    }
    return bResult;
}

int KTongData::LoadMemorabilia(unsigned char* pbyData, size_t uDataLen)
{
    int                     bResult         = false;
    KTONG_DB_MEMORABILIA*   pMemorabilia    = NULL;
    unsigned char*          pbyOffset       = pbyData;
    size_t                  uLeftSize       = uDataLen;
    unsigned long           dwRecordCount   = 0;
    size_t                  uRecordSize     = 0;
    KTongIncidentRecord     Record;

    assert(pbyData);
    KG_PROCESS_ERROR(m_Memorabilia.size() == 0);

    while(uLeftSize > 0)
    {
        KG_PROCESS_ERROR(uLeftSize > sizeof(KTONG_DB_MEMORABILIA));
        pMemorabilia = (KTONG_DB_MEMORABILIA*)pbyOffset;

        uRecordSize = sizeof(KTONG_DB_MEMORABILIA) + pMemorabilia->byDataLen;
        KG_PROCESS_ERROR(uLeftSize >= uRecordSize);

        Record.dwID     = ++dwRecordCount;
        Record.nTime    = pMemorabilia->nTime;

        strncpy(Record.szDescription, (const char*)pMemorabilia->byData, sizeof(Record.szDescription));
        Record.szDescription[sizeof(Record.szDescription) -1] = '\0';

        m_Memorabilia.push_back(Record);

        uLeftSize -= uRecordSize;
        pbyOffset += uRecordSize;
    }

    bResult = true;
Exit0:
    return bResult;
}

int KTongData::LoadRepertory(unsigned char* pbyData, size_t uDataLen)
{
    int                         nResult     = false;
    unsigned char*              pucItemInfo = NULL;
    KTONG_REPERTORY_DB_PAGE*    pDBPage     = NULL;
    KTONG_REPERTORY_DB_ITEM*    pDBItem     = NULL;
    unsigned char*              pbyOffset   = pbyData;
    size_t                      uLeftSize   = uDataLen;
    size_t                      uItemDBSize = 0;
    int                         nCount      = 0;
    int                         nPageIndex  = 0;
    int                         nPos        = 0;

    assert(pbyData);

    while (uLeftSize > 0)
    {
        KG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_REPERTORY_DB_PAGE));
        pDBPage = (KTONG_REPERTORY_DB_PAGE*)pbyOffset;
        pbyOffset += sizeof(KTONG_REPERTORY_DB_PAGE);
        uLeftSize -= sizeof(KTONG_REPERTORY_DB_PAGE);

        nPageIndex = (int)pDBPage->byPageIndex;
        KG_PROCESS_ERROR(nPageIndex >= 0 && nPageIndex < TONG_REPERTORY_PAGE_NUM);

        nCount = pDBPage->byCount;

        m_TongRepertoryPage[nPageIndex].bIsEnable   = true;
        m_TongRepertoryPage[nPageIndex].ucPageIndex = nPageIndex;
        m_TongRepertoryPage[nPageIndex].ucCount = (unsigned char)nCount;

        while (nCount--)
        {
            KG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_REPERTORY_DB_ITEM));
            pDBItem = (KTONG_REPERTORY_DB_ITEM*)pbyOffset;

            uItemDBSize = sizeof(KTONG_REPERTORY_DB_ITEM) + pDBItem->bySize;
            KG_PROCESS_ERROR(uLeftSize >= uItemDBSize);

            pbyOffset += uItemDBSize;
            uLeftSize -= uItemDBSize;

            KG_PROCESS_ERROR(pDBItem->byPos < TONG_REPERTORY_PAGE_CAPACITY);

            nPos = (int)pDBItem->byPos;

            m_TongRepertoryPage[nPageIndex].trItem[nPos].bIsExist   = true;
            m_TongRepertoryPage[nPageIndex].trItem[nPos].ucPos      = nPos;
            m_TongRepertoryPage[nPageIndex].trItem[nPos].ucSize     = pDBItem->bySize;
 
            if (pDBItem->bySize == sizeof(KCOMMON_ITEM_DATA))
            {
                m_TongRepertoryPage[nPageIndex].trItem[nPos].nIsEqui = false;
                memcpy(&(m_TongRepertoryPage[nPageIndex].trItem[nPos].commonEquipData), 
                    pDBItem->byData, 
                    sizeof(m_TongRepertoryPage[nPageIndex].trItem[nPos].commonEquipData)
                );
            }
            else if (pDBItem->bySize == sizeof(KCUSTOM_EQUI_DATA))
            {
                m_TongRepertoryPage[nPageIndex].trItem[nPos].nIsEqui = true;
                memcpy(&(m_TongRepertoryPage[nPageIndex].trItem[nPos].customEquipData), 
                    pDBItem->byData, 
                    sizeof(m_TongRepertoryPage[nPageIndex].trItem[nPos].customEquipData)
                );
            }
            else
            {
                KG_PROCESS_ERROR(false);
            }
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

int KTongData::LoadDevelopmentInfo(unsigned char* pbyData, size_t uDataLen)
{
    int                                 nResult     = false;
    KTONG_DB_DEVELOPMENT_DATA*          pData       = NULL;

    assert(pbyData);
    KG_PROCESS_ERROR(sizeof(KTONG_DB_DEVELOPMENT_DATA_NOW_USE) == sizeof(KTONG_DB_DEVELOPMENT_DATA));
    KG_PROCESS_ERROR(uDataLen == sizeof(KTONG_DB_DEVELOPMENT_DATA_NOW_USE));

    pData = (KTONG_DB_DEVELOPMENT_DATA*)pbyData;

    m_TongBaseInfo.nFund                = pData->nFund;
    m_nMaxWageRate                      = pData->nMaxWageRate;
    m_TongBaseInfo.nDevelopmentPoint    = pData->nDevelopmentPoint;
    m_nUsedDevelopmentPoint             = pData->nUsedDevelopmentPoint;

    KG_PROCESS_ERROR(sizeof(m_byTechTree) == sizeof(pData->byTechTree));
    memcpy(m_byTechTree, pData->byTechTree, sizeof(m_byTechTree));

    nResult = true;
Exit0:
    return nResult;
}
