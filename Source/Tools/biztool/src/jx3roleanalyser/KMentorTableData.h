#ifndef KMENTOR_TABLE_DATA_H
#define KMENTOR_TABLE_DATA_H

#include "KGPublic.h"

#pragma pack(1)
struct KMentorRecordDBHead_Analyser
{
    int nVersion;
};

struct KMentorRecordDB_Analyser : KMentorRecordDBHead_Analyser
{
    time_t  nCreateTime;        // 拜师时间
    int     nMentorValue;       // 师徒值
    time_t  nEndTime;           // 状态的截止时间
    BYTE    byState;            // 关系解除发起方
    BYTE    byOriginalALevel;   // 拜师时徒弟的等级
    BYTE    byReserved[18];
};

struct KMentorTableDB_Analyser
{
    int     nVersion;
    time_t  nCreateTime;        // 拜师时间
    int     nMentorValue;       // 师徒值
    time_t  nEndTime;           // 状态的截止时间
    BYTE    byState;            // 关系解除发起方
    BYTE    byOriginalALevel;   // 拜师时徒弟的等级
    BYTE    byReserved[18];
};

#pragma pack()

struct KMentorInfo_Analyser
{
    time_t  nCreateTime;        // 拜师时间
    int     nMentorValue;       // 师徒值
    time_t  nEndTime;           // 状态的截止时间
    BYTE    byState;            // 关系解除发起方
    BYTE    byOriginalALevel;   // 拜师时徒弟的等级    
    BOOL    bSaved;
};

class KMentorTableData
{
public:
    KMentorTableData(void);
    ~KMentorTableData(void);
public:
    int PaserMentorInfo(unsigned char* pbyData, size_t uDataLen);
public:
    KMentorTableDB_Analyser m_KMentorTableData;
};

#endif //KMENTOR_TABLE_DATA_H