///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KPQDBTableData.h
// Creator  :   liuzhibiao
// Date     :   2009-10-12
// Comment  :   v1.0        从数据库中PQ信息
///////////////////////////////////////////////////////////////

#ifndef _KPQDBTABLEDATA_H_
#define _KPQDBTABLEDATA_H_

#define PQ_PARAM_DB_COUNT           8

#include "KGPublic.h"

#pragma pack(1)
struct KG_JX3_PQ_DB_DATA 
{
    unsigned char    byVersion;
    unsigned long   dwPQID;
    unsigned long   dwPQTemplateID;
    time_t  nBeginTime;
    int     nPQValue[PQ_PARAM_DB_COUNT];
    unsigned char    bFinished;
    unsigned char    byOnwerType;
    unsigned long   dwOnwerID1;
    unsigned long   dwOnwerID2;
    unsigned long   dwOnwerID3;
};
#pragma pack()

class KPQDBTableData
{
public:
    KPQDBTableData(void);
    ~KPQDBTableData(void);
public:
    int PaserPQDBTableData(unsigned char* pbyData, size_t uDataLen);
    
    KG_JX3_PQ_DB_DATA m_PQ_DB_Data;
};

#endif //_KPQDBTABLEDATA_H_