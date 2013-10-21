///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KTongData.h
// Creator  :   liuzhibiao
// Date     :   2009-7-27
// Comment  :   v1.0        从数据库中查询帮会信息
///////////////////////////////////////////////////////////////

#ifndef KTONGDATA_H
#define KTONGDATA_H

// linux编译时需要
#define KGLOG_ERR 3
#define KGLOG_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))   \
            goto Exit0;     \
    } while (false)
// linux编译时需要end

#include "ProtocolBasic.h"
#include "SO3GlobalDef.h"
#include "KGPublic.h"
#include "KTongDef.h"
#include "KTongDB.h"
#include "Engine/EngineBase.h"
#include "Engine/FileType.h"
#include "KGPublic.h"
#include "KGD_Interface.h"
#include "KRoleDBDataDef.h"
#include "Luna.h"
#include "KItem.h"
#include "KTongRepertory.h"
#include <vector>

using namespace std;

#define MAX_TONG_TECHTREE_NODE_NUM_AnanlyserCompatible             64                  // 科技树结点数上限
#define TONG_MEMBER_REMARK_LEN_AnanlyserCompatible                 32

#pragma pack(1)

struct KTONG_DB_DEVELOPMENT_DATA_NOW_USE
{
    int             nFund;              // 资金(银币)
    int             nMaxWageRate;
    int             nDevelopmentPoint;  // 发展点
    int             nUsedDevelopmentPoint;
    unsigned char   ucTechTree[MAX_TONG_TECHTREE_NODE_NUM_AnanlyserCompatible];
};

struct KTONG_DB_MEMBER_DATA_NOW_USE
{
    DWORD   dwID;
    int     nGroupID;                                               // 所属成员组
    time_t  nJoinTime;                                              // 加入时间
    char    szRemark[TONG_MEMBER_REMARK_LEN_AnanlyserCompatible];   // 备注
    time_t  nLastOfflineTime;                                       // 最后在线时间
    int     nSalary;                                                // 工资储蓄(银币)
};

struct KTONG_DB_MEMBER_DATA_VERSION1_AnanlyserCompatible
{
    DWORD   dwID;
    int     nGroupID;                                               // 所属成员组
    time_t  nJoinTime;                                              // 加入时间
    char    szRemark[TONG_MEMBER_REMARK_LEN_AnanlyserCompatible];   // 备注
    time_t  nLastOfflineTime;                                       // 最后在线时间
};

struct KTONG_DB_BASEINFO_VERSION1_ANALYSER
{
    char            szName[_NAME_LEN];
    unsigned long   dwMaster;           // 帮主
    int             nFund;              // 资金
    int             nLevel;
    int             nDevelopmentPoint;  // 发展点
    unsigned short  wMaxMemberCount;    // 成员上限
    unsigned char   byTongState;
    time_t          nStateTimer;
    unsigned char   byCamp;             // 阵营
};

struct KTONG_DB_BASEINFO_ANALYSER
{
    char    szName[_NAME_LEN];
    DWORD   dwMaster;           // 帮主
    int     nLevel;
    WORD    wMaxMemberCount;    // 成员上限
    BYTE    byTongState;
    time_t  nStateTimer;
    BYTE    byCamp;             // 阵营

};

struct KJX3TONG_DB_SECTION
{
    int             nType;
    size_t          uDataLen;
    unsigned char   ucData[0];
};

struct KJX3TONG_DB_SCHEMA
{
    KTongSchemaData Schema;
};

struct KJX3TONG_DB_ROSTER
{
    int              nMemberCount;
    unsigned char    byData[0];
};

struct TONG_REPERTORY_ITEM_ANALYSER
{
    bool          bIsExist;
    unsigned char ucPos;
    unsigned char ucSize;
    int           nIsEqui;
    union
    {
        KCOMMON_ITEM_DATA commonEquipData;
        KCUSTOM_EQUI_DATA customEquipData;
    };
};

struct TONG_REPERTORY_PAGE_ANALYSER
{
    bool          bIsEnable;
    unsigned char ucPageIndex;
    unsigned char ucCount;
    TONG_REPERTORY_ITEM_ANALYSER trItem[TONG_REPERTORY_PAGE_CAPACITY];
};
#pragma pack()

#define TONG_MEMBER_REMARK_LEN_NOW_USE 32

struct KTongMember_NowUse
{
    DWORD   dwID;
    int     nGroupID;                                   // 所属成员组
    time_t  nJoinTime;                                  // 加入时间
    char    szRemark[TONG_MEMBER_REMARK_LEN_NOW_USE];   // 备注
    time_t  nLastOfflineTime;                           // 最后在线时间
    int     nSalary;                                    // 工资储蓄(银币)
    //---------- 以下变量周期性查询更新 ----------
    int     nLevel;
    DWORD   dwForceID;
    DWORD   dwMapID;
    BOOL    bIsOnline;
    //--------------------------------------------
    int     nLastUpdateFrame;
};

class KTongData
{
public:
    KTongData(void);
    ~KTongData(void);
public:
    int PaserTongDataInfo(unsigned char* pbyData, size_t uDataLen);

private:
    int LoadBaseInfoVersion1(unsigned char* pbyData, size_t uDataLen);
    int LoadBaseInfo(unsigned char* pbyData, size_t uDataLen);
    int LoadSchema(unsigned char* pbyData, size_t uDataLen);
    int LoadRoster(unsigned char* pbyData, size_t uDataLen);
    int LoadRosterVersion1(unsigned char* pbyData, size_t uDataLen);
    int LoadMemorabilia(unsigned char* pbyData, size_t uDataLen);
    int LoadRepertory(unsigned char* pbyData, size_t uDataLen);
    int LoadDevelopmentInfo(unsigned char* pbyData, size_t uDataLen);

public:
    KTONG_DB_BASEINFO_VERSION1_ANALYSER     m_TongBaseInfo;
    KTongSchemaData                         m_SchemaData;
    char                                    m_szAnnouncement[128];              // 公告
    char                                    m_szOnlineMessage[128];             // 上线提示
    char                                    m_szIntroduction[128];              // 介绍
    char                                    m_szRules[1024];                    // 帮规
    vector<KTongMember_NowUse*>             m_Memberlist;                       // 成员列表
    int                                     m_nMaxWageRate;                     // 工资发放比例上限
    int                                     m_nUsedDevelopmentPoint;            // 投入发展点
    unsigned char                           m_byTechTree[MAX_TONG_TECHTREE_NODE_NUM];

    typedef std::vector<KTongIncidentRecord> KINCIDENT_TABLE;

    KINCIDENT_TABLE                         m_Memorabilia;                      // 帮会大事记

    TONG_REPERTORY_PAGE_ANALYSER    m_TongRepertoryPage[TONG_REPERTORY_PAGE_NUM];   // analyser帮会仓库
};
#endif //KTONGDATA_H