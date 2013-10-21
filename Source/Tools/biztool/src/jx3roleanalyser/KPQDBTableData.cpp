///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KPQDBTableData.h
// Creator  :   liuzhibiao
// Date     :   2009-10-12
// Comment  :   v1.0        从数据库中PQ信息
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KPQDBTableData.h"

KPQDBTableData::KPQDBTableData(void)
{
}

KPQDBTableData::~KPQDBTableData(void)
{
}

int KPQDBTableData::PaserPQDBTableData(unsigned char* pbyData, size_t uDataLen)
{
    int                                bResult             = false;
    //int                                bRetCode            = false;
    KG_JX3_PQ_DB_DATA*                  pPQ_DB_Data  = NULL;
    pPQ_DB_Data = (KG_JX3_PQ_DB_DATA*)pbyData;

    KG_PROCESS_ERROR(pPQ_DB_Data);
    KG_PROCESS_ERROR(uDataLen == sizeof(KG_JX3_PQ_DB_DATA));

    memcpy(&m_PQ_DB_Data, pPQ_DB_Data, sizeof(KG_JX3_PQ_DB_DATA));

    bResult = true;
Exit0:
    return bResult;
}
