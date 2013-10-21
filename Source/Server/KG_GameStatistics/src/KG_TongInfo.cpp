////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_TongInfo.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-26  15:31:56
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "KG_TongInfo.h"

KG_TongInfo::KG_TongInfo()
{
}

KG_TongInfo::~KG_TongInfo()
{
}

int KG_TongInfo::Init(const unsigned char cbyInfo[], unsigned uInfoSize, void *pvContext)
{
	int nResult  = false;
	int nRetCode = false;
    const KTONG_DB_HEADER  *pcTongDBHeader = NULL;
    const KTONG_DB_SECTION *pcTongDBSection = NULL;
    const unsigned char *pcbyReadPos = NULL;
    unsigned uLeftSize = 0;    

    KGLOG_PROCESS_ERROR(cbyInfo);
    KGLOG_PROCESS_ERROR(uInfoSize);

    KG_USE_ARGUMENT(pvContext);

    pcbyReadPos = cbyInfo;
    uLeftSize   = uInfoSize;

    KGLOG_PROCESS_ERROR(uInfoSize >= sizeof(KTONG_DB_HEADER));
    pcTongDBHeader = (const KTONG_DB_HEADER *)cbyInfo;

    pcbyReadPos += sizeof(KTONG_DB_HEADER);
    uLeftSize   -= sizeof(KTONG_DB_HEADER);

    KGLOG_PROCESS_ERROR(pcTongDBHeader->nVersion == TONG_DB_VERSION);
    while (true)
    {
        if (uLeftSize == 0)
            break;

        KGLOG_PROCESS_ERROR(uLeftSize >= sizeof(KTONG_DB_SECTION));
        pcTongDBSection = (const KTONG_DB_SECTION *)pcbyReadPos;

        pcbyReadPos += sizeof(KTONG_DB_SECTION);
        uLeftSize   -= sizeof(KTONG_DB_SECTION);

        KGLOG_PROCESS_ERROR(uLeftSize >= pcTongDBSection->uDataLen);
        switch (pcTongDBSection->nType)
        {
        case eTongBaseInfo:
            nRetCode = _LoadBaseInfo(pcbyReadPos, (unsigned)pcTongDBSection->uDataLen);
            KGLOG_PROCESS_ERROR(nRetCode && "_LoadBaseInfo()");
            break;
        case eTongRoster:
            nRetCode = _LoadRosterInfo(pcbyReadPos, (unsigned)pcTongDBSection->uDataLen);
            KGLOG_PROCESS_ERROR(nRetCode && "_LoadRosterInfo()");
            break;
        default:    // Ignore the block not need parsed.
            break;
        }

        pcbyReadPos += (unsigned)pcTongDBSection->uDataLen;
        uLeftSize   -= (unsigned)pcTongDBSection->uDataLen;
    }

	nResult = true;
Exit0:
	return nResult;
}

int KG_TongInfo::UnInit(void *pvContext)
{
    KG_USE_ARGUMENT(pvContext);
    return true;
}

int KG_TongInfo::GetTongName(unsigned uBufferSize, char *pszRetBuffer) const
{
	int nResult  = false;
    
    KGLOG_PROCESS_ERROR(uBufferSize);
    KGLOG_PROCESS_ERROR(pszRetBuffer);

    strncpy(pszRetBuffer, m_BaseInfo.szName, uBufferSize);
    KGLOG_PROCESS_ERROR(pszRetBuffer[uBufferSize - 1] == '\0');
    
	nResult = true;
Exit0:
	return nResult;
}

int KG_TongInfo::GetMasterID(int *pnRetMasterID) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(pnRetMasterID);

    *pnRetMasterID = m_BaseInfo.dwMaster;

	nResult = true;
Exit0:
	return nResult;
}

int KG_TongInfo::GetCampID(int *pnRetCampID) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(pnRetCampID);

    *pnRetCampID = m_BaseInfo.byCamp;

	nResult = true;
Exit0:
	return nResult;
}

int KG_TongInfo::GetMaxMemberCount(int *pnRetMaxMemberCount) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(pnRetMaxMemberCount);

    *pnRetMaxMemberCount = m_BaseInfo.wMaxMemberCount;

	nResult = true;
Exit0:
	return nResult;
}

int KG_TongInfo::GetTotalMemberCount(int *pnRetTotalMemberCount) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(pnRetTotalMemberCount);

    *pnRetTotalMemberCount = (int)m_MemberDataVector.size();    

	nResult = true;
Exit0:
	return nResult;
}

int KG_TongInfo::GetMemberID(int nMemberIndex, int *pnRetMemberID) const
{
	int nResult  = false;

    KGLOG_PROCESS_ERROR(nMemberIndex >= 0);
    KGLOG_PROCESS_ERROR(nMemberIndex < (int)m_MemberDataVector.size());
    KGLOG_PROCESS_ERROR(pnRetMemberID);

    *pnRetMemberID = m_MemberDataVector[nMemberIndex].dwID;

	nResult = true;
Exit0:
	return nResult;
}

int KG_TongInfo::_LoadBaseInfo(const unsigned char cbyInfo[], unsigned uInfoSize)
{
	int nResult  = false;

    ASSERT(cbyInfo);
    ASSERT(uInfoSize);

    KGLOG_PROCESS_ERROR(uInfoSize == sizeof(KTONG_DB_BASEINFO));
    m_BaseInfo = *(KTONG_DB_BASEINFO *)cbyInfo;

	nResult = true;
Exit0:
	return nResult;
}

int KG_TongInfo::_LoadRosterInfo(const unsigned char cbyInfo[], unsigned uInfoSize)
{
	int nResult  = false;
    const KTONG_DB_ROSTER *pRoster = NULL;
    const KTONG_DB_MEMBER_DATA *pcMemberData = NULL;
    unsigned uRosterDataSize = 0;

    ASSERT(cbyInfo);
    ASSERT(uInfoSize);

    KGLOG_PROCESS_ERROR(uInfoSize >= sizeof(KTONG_DB_ROSTER));
    pRoster = (const KTONG_DB_ROSTER *)cbyInfo;

    uRosterDataSize = sizeof(KTONG_DB_ROSTER) + sizeof(KTONG_DB_MEMBER_DATA) * pRoster->nMemberCount;
    KGLOG_PROCESS_ERROR(uRosterDataSize == uInfoSize);

    pcMemberData = (const KTONG_DB_MEMBER_DATA *)pRoster->byData;

    m_MemberDataVector.clear();
    m_MemberDataVector.reserve(pRoster->nMemberCount);
    for (int i = 0; i < pRoster->nMemberCount; i++)
    {
        m_MemberDataVector.push_back(pcMemberData[i]);
    }
    
	nResult = true;
Exit0:
	return nResult;
}
