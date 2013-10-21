///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KStatDataAnalyser.h
// Creator  :   liuzhibiao
// Date     :   2009-8-21
// Comment  :   v1.0        从数据库中查询角色好友信息
///////////////////////////////////////////////////////////////

#ifndef KSTATDATAANALYSER_H
#define KSTATDATAANALYSER_H

#define MAX_STAT_DATA_COUNT 1024 * 512

#include "KGPublic.h"

#pragma pack(1)
struct JX3KStatData_DB
{
    int     nDataCount;
    int64_t nData[0];
};
#pragma pack()

class KStatDataAnalyser
{
public:
    KStatDataAnalyser(void);
    ~KStatDataAnalyser(void);
public:
    int PaserStatData(unsigned char* pbyData, size_t uDataLen);

public:
    int64_t m_StatValues[MAX_STAT_DATA_COUNT];
    int     m_nMaxUsedID;
};
#endif //KSTATDATAANALYSER_H