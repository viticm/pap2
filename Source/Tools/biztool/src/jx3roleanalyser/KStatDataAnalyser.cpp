///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KStatDataAnalyser.cpp
// Creator  :   liuzhibiao
// Date     :   2009-8-21
// Comment  :   v1.0        从数据库中查询角色好友信息
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KStatDataAnalyser.h"

KStatDataAnalyser::KStatDataAnalyser(void)
{
    memset(&m_StatValues[0], 0, sizeof(m_StatValues));
    m_nMaxUsedID = 0;
}

KStatDataAnalyser::~KStatDataAnalyser(void)
{
}

int KStatDataAnalyser::PaserStatData( unsigned char* pbyData, size_t uDataLen )
{
    int               bResult = false;
    JX3KStatData_DB*   pDataDB = (JX3KStatData_DB*)pbyData;

    assert(pDataDB);

    KG_PROCESS_ERROR(uDataLen == sizeof(JX3KStatData_DB) + pDataDB->nDataCount * sizeof(int64_t));

    memcpy(&m_StatValues[0], pDataDB->nData, uDataLen - sizeof(pDataDB->nDataCount));

    m_nMaxUsedID = pDataDB->nDataCount;

    bResult = true;
Exit0:
    return bResult;
}
