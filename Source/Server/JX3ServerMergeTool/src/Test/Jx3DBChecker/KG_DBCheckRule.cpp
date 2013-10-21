//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KG_DBCheckRule.cpp
//  Version     :   1.0
//  Creater     :   Liuzhibiao
//  Date        :   2009-12-8
//  Comment     :   
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_DBCheckRule.h"
#include "KG_Jx3DBChecker.h"
#include "KDBTools.h"
#include "KCommon.h"

int KG_DBCheckRule::m_snFuncIndex = 0;

#define REGISTER_RULE_FUNC(TableName, Func)                 \
    do                                                      \
    {                                                       \
        m_RuleTable[TableName] =  m_snFuncIndex;            \
        m_RuleFunc[m_snFuncIndex] = &KG_DBCheckRule::Func;  \
        m_snFuncIndex++;                                    \
    } while (false)

#define CLEAR_INFOVECTOR(InfoVector)                    \
    do                                                  \
    {                                                   \
        for (size_t i = 0; i < InfoVector.size(); i++)  \
        {                                               \
            KG_DELETE_ARRAY(InfoVector[i].pvData);      \
        }                                               \
        InfoVector.clear();                             \
    } while(false)

int KG_DBCheckRule::InitDBCheckRule()
{
    int nResult = false;
//    int nRetCode = false;

    m_lRoleID_A = 0;
    m_lSequenceRoleID_B = 0;
    m_lRoleID_C = 0;

    m_lTongID_A = 0;
    m_lSequenceTongID_B = 0;
    m_lTongID_C = 0;

    m_GlobalMailIndexB = 0;

    memset(m_RuleFunc, 0, sizeof(m_RuleFunc));

    REGISTER_RULE_FUNC("sequence", _CheckSequenceTable);
    REGISTER_RULE_FUNC("activity", _CheckActivityTable);
    REGISTER_RULE_FUNC("antifarmer", _CheckAntifarmerTable);
    REGISTER_RULE_FUNC("auction", _CheckAuctionTable);
    REGISTER_RULE_FUNC("fellowship", _CheckFellowshipTable);
    REGISTER_RULE_FUNC("gamecard", _CheckGameCardTable);
    REGISTER_RULE_FUNC("globalcustomdata", _CheckGlobalcustomdataTable);
    REGISTER_RULE_FUNC("globalsystemvalue", _CheckGlobalsystemvalue);
    REGISTER_RULE_FUNC("globalmailex", _CheckGlobalmailex);
    REGISTER_RULE_FUNC("mailbox", _CheckMailBoxTable);
    REGISTER_RULE_FUNC("mapcopy", _CheckMapcopyTable);
    REGISTER_RULE_FUNC("pq", _CheckPQTable);
    REGISTER_RULE_FUNC("role", _CheckRoleTable);
    REGISTER_RULE_FUNC("restorerole", _CheckRestoreroleTable);
    REGISTER_RULE_FUNC("roleblacklist", _CheckRoleblacklistTable);
    REGISTER_RULE_FUNC("roledeletelist", _CheckDeletelistTable);
    REGISTER_RULE_FUNC("statdatanameindex", _CheckStatdatanameindex);
    REGISTER_RULE_FUNC("statdata", _CheckSatadata);
    REGISTER_RULE_FUNC("tong", _CheckTongTable);
    REGISTER_RULE_FUNC("renamerole", _CheckRenameRoleTable);
    nResult = true;

    return nResult;
}

void KG_DBCheckRule::UnInitDBCheckRule()
{

}

int KG_DBCheckRule::RuleCheck(string strTableName, KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    RULE_FUNC RuleFunc = NULL;
    
    RuleFunc = m_RuleFunc[m_RuleTable[strTableName]];
    if (RuleFunc != NULL)
    {
        nRetCode = (this->*RuleFunc)(pTaskInfo);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        KGLogPrintf(KGLOG_INFO, "Do not find The CheckRule of The %s Table", strTableName);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KG_DBCheckRule::_CheckActivityTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;
    
    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;

    bool              bIsNoMoreRecordA = false;
    bool              bIsNoMoreRecordB = false;
    bool              bIsNoMoreRecordC = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    unsigned long ulLastID_B = 0;
    unsigned long ulLastID_C = 0;

    TempFileNameVector.push_back("ID");
    TempFileNameVector.push_back("Type");
    TempFileNameVector.push_back("Time");
    TempFileNameVector.push_back("Value0");
    TempFileNameVector.push_back("Value1");
    TempFileNameVector.push_back("Value2");
    TempFileNameVector.push_back("Value3");
    TempFileNameVector.push_back("Value4");
    TempFileNameVector.push_back("Value5");

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    // 检查 B 和 C 匹配
    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);

        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecordB);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (bIsNoMoreRecordB)
        {
            break;
        }

        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);

        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecordC);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(!bIsNoMoreRecordC);
        ulHadNowCheckCCount++;
        // 判断所有字段信息是否完全一致
        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
        // 保存最后一个ID号
        ulLastID_B = strtoul((char*)TempInfoVectorB[0].pvData, NULL, 10);

    }

    // 检查 A 和 C 匹配, ID 累加
    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorA);

        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecordA);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (bIsNoMoreRecordA)
        {
            break;
        }
        ulHadNowCheckACount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecordC);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecordC);
        ulHadNowCheckCCount++;

        // 判断所有字段信息除ID项累加外,是否完全一致
        KGLOG_PROCESS_ERROR(TempInfoVectorA.size() == TempInfoVectorC.size());
        
        ulLastID_B++;
        ulLastID_C = strtoul((char*)TempInfoVectorB[0].pvData, NULL, 10);
        
        KGLOG_PROCESS_ERROR(ulLastID_C == ulLastID_B);

        for (size_t i = 1; i < TempInfoVectorA.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorA[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorA[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    nResult = true;
Exit0:
    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckSequenceTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    long lRoleID_A = 0;
    long lRoleID_B = 0;
    long lRoleID_C = 0;

    long lTongID_A = 0;
    long lTongID_B = 0;
    long lTongID_C = 0;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    TempFileNameVector.push_back("seqname");
    TempFileNameVector.push_back("currentid");

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "seqname", "RoleID");
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "seqname", "RoleID");
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "seqname", "RoleID");
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    // 检查RoleID   a + b = c

    CLEAR_INFOVECTOR(TempInfoVectorA);

    nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);

    lRoleID_A = strtoul((char*)TempInfoVectorA[1].pvData, NULL, 10);
    m_lRoleID_A = lRoleID_A;

    CLEAR_INFOVECTOR(TempInfoVectorB);

    nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);

    lRoleID_B = strtoul((char*)TempInfoVectorB[1].pvData, NULL, 10);
    m_lSequenceRoleID_B = lRoleID_B;

    CLEAR_INFOVECTOR(TempInfoVectorC);

    nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);

    lRoleID_C = strtoul((char*)TempInfoVectorC[1].pvData, NULL, 10);
    m_lRoleID_C = lRoleID_C;

    KGLOG_PROCESS_ERROR(lRoleID_C == lRoleID_A + lRoleID_B);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }

    // 检查TongID   a + b = c
    TempFileNameVector.clear();

    TempFileNameVector.push_back("seqname");
    TempFileNameVector.push_back("currentid");

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "seqname", "TongID");
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "seqname", "TongID");
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "seqname", "TongID");
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    CLEAR_INFOVECTOR(TempInfoVectorA);

    nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);

    lTongID_A = strtoul((char*)TempInfoVectorA[1].pvData, NULL, 10);
    m_lTongID_A = lTongID_A;

    CLEAR_INFOVECTOR(TempInfoVectorB);

    nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);

    lTongID_B = strtoul((char*)TempInfoVectorB[1].pvData, NULL, 10);
    m_lSequenceTongID_B = lTongID_B;

    CLEAR_INFOVECTOR(TempInfoVectorC);

    nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
    KGLOG_PROCESS_ERROR(nRetCode);
    KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);

    lTongID_C = strtoul((char*)TempInfoVectorC[1].pvData, NULL, 10);
    m_lTongID_C = lTongID_C;

    KGLOG_PROCESS_ERROR(lTongID_C == lTongID_A + lTongID_B);

    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }

    // 获取B的总GlobalMailIndex

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "seqname", "GlobalMailIndex");
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    CLEAR_INFOVECTOR(TempInfoVectorB);

    nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    if (bIsNoMoreRecord)
    {
        m_GlobalMailIndexB = 0;
    }
    else
    {
        m_GlobalMailIndexB = strtoul((char*)TempInfoVectorB[1].pvData, NULL, 10);
    }

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);
    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckAntifarmerTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    char szAccountA[MYSQL_STRING_MAX_SIZE];
    char szAccountB[MYSQL_STRING_MAX_SIZE];

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("Account");
    TempFileNameVector.push_back("PunishEndTime");

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    //（3）antifarmer表：
    //    a中的账号在b中不存在，插入，并检查插入信息和原a中信息完全一致
    //    a宏的账号在b中存在，替换，并检查插入信息和原a中信息完全一致
    // 检查a中的账号在b中不存在，在c中存在，检查c信息和原a中信息完全一致
    // a中的账号在b中存在，替换，并检查c信息和原a中信息完全一致
    // 检查b中的账号,c中都存在

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckACount++;

        memset(szAccountA, '\0', sizeof(szAccountA));
        //KGLOG_PROCESS_ERROR(TempInfoVectorA[0].uDataSize <= sizeof(szAccountA));
        strncpy(szAccountA, (char*)TempInfoVectorA[0].pvData, sizeof(szAccountA));
        szAccountA[TempInfoVectorA[0].uDataSize] = '\0';
        szAccountA[sizeof(szAccountA) - 1] = '\0';

        pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "Account", szAccountA);
        KGLOG_PROCESS_ERROR(pMySQL_ResC);
        nInitMySQL_ResC = true;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        for (size_t i = 0; i < TempInfoVectorA.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorA[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorA[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }

        if (nInitMySQL_ResC)
        {
            pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
            nInitMySQL_ResC = false;
        }
    }

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;
    ulHadNowCheckBCount = 0;

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }

        memset(szAccountB, '\0', sizeof(szAccountB));
        //KGLOG_PROCESS_ERROR(TempInfoVectorB[0].uDataSize <= sizeof(szAccountB));
        strncpy(szAccountB, (char*)TempInfoVectorB[0].pvData, sizeof(szAccountB));
        szAccountB[TempInfoVectorB[0].uDataSize] = '\0';
        szAccountB[sizeof(szAccountB) - 1] = '\0';

        pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "Account", szAccountB);
        KGLOG_PROCESS_ERROR(pMySQL_ResA);
        nInitMySQL_ResA = true;

        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "Account", szAccountB);
            KGLOG_PROCESS_ERROR(pMySQL_ResC);
            nInitMySQL_ResC = true;

            CLEAR_INFOVECTOR(TempInfoVectorC);
            nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
            KGLOG_PROCESS_ERROR(nRetCode);
            KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);

            for (size_t i = 0; i < TempInfoVectorB.size(); i++)
            {
                nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
                KGLOG_PROCESS_ERROR(nRetCode == 0);
            }

            if (nInitMySQL_ResC)
            {
                pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
                nInitMySQL_ResC = false;
            }
        }
        if (nInitMySQL_ResA)
        {
            pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
            nInitMySQL_ResA = false;
        }
    }

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckAuctionTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    long lSellerID_A = 0;
    long lSellerID_C = 0;

    long lBidderID_A = 0;
    long lBidderID_C = 0;

    long lAuctionLastID_A = 0;
    long lAuctionLastID_B = 0;
    long lAuctionLastID_C = 0;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("ID");
    TempFileNameVector.push_back("SellerID");
    TempFileNameVector.push_back("BidderID");
    TempFileNameVector.push_back("Name");
    TempFileNameVector.push_back("AucGenre");
    TempFileNameVector.push_back("AucSub");
    TempFileNameVector.push_back("RequireLevel");
    TempFileNameVector.push_back("Quality");
    TempFileNameVector.push_back("SellerName");
    TempFileNameVector.push_back("Price");
    TempFileNameVector.push_back("BuyItnowPrice");
    TempFileNameVector.push_back("CustodyCharges");
    TempFileNameVector.push_back("DurationTime");
    TempFileNameVector.push_back("ItemData");

    //（4）auction表：
    //    a中内容在c中全部存在，sellerID = a中原来SellerID + b原来的总RoleID
    //    bidderID = a中原来bidderID + b原来的总RoleID
    //    其余字段完全一致
    //  方法 , 遍历b,获取b中的所有记录,都在c中找到
    //         遍历a,在b的id值基础上,每条记录在c中ID值递增,对比其他字段
    //          sellerID = a中原来SellerID + b原来的总RoleID
    //          bidderID = a中原来bidderID + b原来的总RoleID

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        lAuctionLastID_B = strtoul((char*)TempInfoVectorB[0].pvData, NULL, 10);

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    lAuctionLastID_A = lAuctionLastID_B;
    while(true)  // 遍历a,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckACount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorA.size() == TempInfoVectorC.size());

        lAuctionLastID_C = strtoul((char*)TempInfoVectorC[0].pvData, NULL, 10);
        lAuctionLastID_A++;
        KGLOG_PROCESS_ERROR(lAuctionLastID_C == lAuctionLastID_A);

        lSellerID_A = strtoul((char*)TempInfoVectorA[1].pvData, NULL, 10);
        lSellerID_C = strtoul((char*)TempInfoVectorC[1].pvData, NULL, 10);
        KGLOG_PROCESS_ERROR(lSellerID_A + m_lSequenceRoleID_B == lSellerID_C);
        
        lBidderID_A = strtoul((char*)TempInfoVectorA[2].pvData, NULL, 10);
        lBidderID_C = strtoul((char*)TempInfoVectorC[2].pvData, NULL, 10);
        if (lBidderID_A != 0)
        {
            KGLOG_PROCESS_ERROR(lBidderID_A + m_lSequenceRoleID_B == lBidderID_C);
        }
        else
        {
            KGLOG_PROCESS_ERROR(lBidderID_A == lBidderID_C);
        }

        for (size_t i = 3; i < TempInfoVectorA.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorA[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorA[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }
    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckACount + ulHadNowCheckBCount);

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckFellowshipTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    long lPlayerID_A = 0;
    long lPlyaerID_C = 0;

    int nFellowShipCountA   = 0;
    int nFellowShipCountC   = 0;

    int nFoeCountA   = 0;
    int nFoeCountC   = 0;

    int nBlackListCountA   = 0;
    int nBlackListCountC   = 0;

    DWORD dwAlliedPlayerIDA;
    int nAttractionA;
    int nGroupIDA; 
    int nMarriedA; 
    int nBrotherA;
    int nWhisperDailyCountA;
    int nDuelDailyCountA; 
    char szRemarkA[128]; 
    int nRemarkSizeA = sizeof(szRemarkA);

    DWORD dwAlliedPlayerIDC;
    int nAttractionC;
    int nGroupIDC; 
    int nMarriedC; 
    int nBrotherC;
    int nWhisperDailyCountC;
    int nDuelDailyCountC; 
    char szRemarkC[128]; 
    int nRemarkSizeC = sizeof(szRemarkC);

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    PARSEHANDLE pParseHandleA = NULL;
    PARSEHANDLE pParseHandleC = NULL;

    int nInitParseHandleA = false;
    int nInitParseHandleC = false;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("PlayerID");
    TempFileNameVector.push_back("FellowshipData");

    //（5）fellowship表：
    //    a中内容在c中全部存在，playerID = a中原来playerID + b原来的总RoleID
    //    FellowshipData中，所有的好友、仇人、黑名单 = 原ID + b原来的总RoleID
    //  方法 , 遍历b,获取b中的所有记录,都在c中找到
    //         遍历a,在b的id值基础上,每条记录在c中ID值递增,对比其他字段
    //          sellerID = a中原来SellerID + b原来的总RoleID
    //          bidderID = a中原来bidderID + b原来的总RoleID

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)  // 遍历a,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckACount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        //KGLOG_PROCESS_ERROR(TempInfoVectorA.size() == TempInfoVectorC.size());

        lPlayerID_A = strtoul((char*)TempInfoVectorA[0].pvData, NULL, 10);
        lPlyaerID_C = strtoul((char*)TempInfoVectorC[0].pvData, NULL, 10);

        KGLOG_PROCESS_ERROR(lPlayerID_A + m_lSequenceRoleID_B == lPlyaerID_C);

        pParseHandleA = ParseFellowShipData((unsigned char*)TempInfoVectorA[1].pvData, TempInfoVectorA[1].uDataSize);
        KGLOG_PROCESS_ERROR(pParseHandleA);
        nInitParseHandleA = true;

        pParseHandleC = ParseFellowShipData((unsigned char*)TempInfoVectorC[1].pvData, TempInfoVectorC[1].uDataSize);
        KGLOG_PROCESS_ERROR(pParseHandleC);
        nInitParseHandleC = true;

        nRetCode = GetFellowShipCount(pParseHandleA, nFellowShipCountA);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetFellowShipCount(pParseHandleC, nFellowShipCountC);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(nFellowShipCountA == nFellowShipCountC);

        for (int i = 0; i < nFellowShipCountA; i++)
        {
            nRetCode = GetFellowshipInfo(pParseHandleA, i, dwAlliedPlayerIDA, nAttractionA, 
                nGroupIDA, nMarriedA, nBrotherA,  nWhisperDailyCountA,
                nDuelDailyCountA, szRemarkA, nRemarkSizeA
            );
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = GetFellowshipInfo(pParseHandleC, i, dwAlliedPlayerIDC, nAttractionC, 
                nGroupIDC, nMarriedC, nBrotherC,  nWhisperDailyCountC,
                nDuelDailyCountC, szRemarkC, nRemarkSizeC  
            );
            KGLOG_PROCESS_ERROR(nRetCode);
            KGLOG_PROCESS_ERROR(dwAlliedPlayerIDC == dwAlliedPlayerIDA + m_lSequenceRoleID_B);
        }

        nRetCode = GetFoeCount(pParseHandleA, nFoeCountA);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetFoeCount(pParseHandleC, nFoeCountC);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(nFoeCountA == nFoeCountC);

        for (int i = 0; i < nFoeCountA; i++)
        {
            nRetCode = GetFoeID(pParseHandleA, i, dwAlliedPlayerIDA);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = GetFoeID(pParseHandleC, i, dwAlliedPlayerIDC);
            KGLOG_PROCESS_ERROR(nRetCode);

            KGLOG_PROCESS_ERROR(dwAlliedPlayerIDC == dwAlliedPlayerIDA + m_lSequenceRoleID_B);
        }

        nRetCode = GetBlackListCount(pParseHandleA, nBlackListCountA);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetBlackListCount(pParseHandleC, nBlackListCountC);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(nBlackListCountA == nBlackListCountC);

        for (int i = 0; i < nBlackListCountA; i++)
        {
            nRetCode = GetBlackListID(pParseHandleA, i, dwAlliedPlayerIDA);
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = GetBlackListID(pParseHandleC, i, dwAlliedPlayerIDC);
            KGLOG_PROCESS_ERROR(nRetCode);

            KGLOG_PROCESS_ERROR(dwAlliedPlayerIDC == dwAlliedPlayerIDA + m_lSequenceRoleID_B);
        }

         EndFellowshipParse(pParseHandleA);
         EndFellowshipParse(pParseHandleC);
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }

    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckACount + ulHadNowCheckBCount);

    nResult = true;
Exit0:
    if (nInitParseHandleA)
    {
        EndFellowshipParse(pParseHandleA);
    }
    if (nInitParseHandleC)
    {
        EndFellowshipParse(pParseHandleC);
    }
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckGameCardTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    long lSellerID_A = 0;
    long lSellerID_C = 0;

    long lGameCardLastID_A = 0;
    long lGameCardLastID_B = 0;
    long lGameCardLastID_C = 0;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("ID");
    TempFileNameVector.push_back("SellerID");
    TempFileNameVector.push_back("Coin");
    TempFileNameVector.push_back("GameTime");
    TempFileNameVector.push_back("Type");
    TempFileNameVector.push_back("Price");
    TempFileNameVector.push_back("EndTime");

    //（6）gamecard表：
    //    a中内容在c中全部存在，SellerID = a中原来SellerID + b原来的总RoleID
    //    其余字段完全一致

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        lGameCardLastID_B = strtoul((char*)TempInfoVectorB[0].pvData, NULL, 10);

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    lGameCardLastID_A = lGameCardLastID_B;
    while(true)  // 遍历a,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckACount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorA.size() == TempInfoVectorC.size());

        lGameCardLastID_C = strtoul((char*)TempInfoVectorC[0].pvData, NULL, 10);
        lGameCardLastID_A++;
        KGLOG_PROCESS_ERROR(lGameCardLastID_C == lGameCardLastID_A);

        lSellerID_A = strtoul((char*)TempInfoVectorA[1].pvData, NULL, 10);
        lSellerID_C = strtoul((char*)TempInfoVectorC[1].pvData, NULL, 10);
        KGLOG_PROCESS_ERROR(lSellerID_A + m_lSequenceRoleID_B == lSellerID_C);

        for (size_t i = 2; i < TempInfoVectorA.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorA[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorA[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }

    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckACount + ulHadNowCheckBCount);

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckGlobalcustomdataTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("Name");
    TempFileNameVector.push_back("Value");

    //（7）globalcustomdata表：
    //    a中内容全部被丢弃

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }

    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckBCount);

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckGlobalsystemvalue(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("Name");
    TempFileNameVector.push_back("Value");

    //（8）globalsystemvalue表：
    //    a中内容全部被丢弃

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }
    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckBCount);

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckGlobalmailex(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("Version");
    TempFileNameVector.push_back("MailIndex");
    TempFileNameVector.push_back("EndTime");
    TempFileNameVector.push_back("MailInfo");

    //（9）globalmailex表：
    //    a中内容全部被丢弃  

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }
    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckBCount);

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckMailBoxTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    long lMailboxLastID_A = 0;
    long lMailboxLastID_B = 0;
    long lMailboxLastID_C = 0;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    PARSEHANDLE pParseHandleA = NULL;
    PARSEHANDLE pParseHandleC = NULL;

    unsigned long ulLastGlobalMailIndexC;
    unsigned long ulNextMailIDC;
    unsigned short usVersionC;

    int nMailCountA;
    int nMailCountC;

    unsigned long   ulSenderIDA;
    DWORD           dwMailIDA; 
    char            szSenderNameA[128];
    unsigned char   ucTypeA;
    bool            bReadA;
    int             nSenderNameSizeA = sizeof(szSenderNameA); 
    char            szTitleA[128];
    int             nTitleSizeA = sizeof(szTitleA);
    time_t          nSendTimeA;
    time_t          nReceiveTimeA;
    time_t          nLockTimeA; 
    int             nMoneyA;
    int             nItemCountA;
    char            szTextA[1024];
    int             nTextSizeA = sizeof(szTextA);

    unsigned long   ulSenderIDC;
    DWORD           dwMailIDC; 
    char            szSenderNameC[128];
    unsigned char   ucTypeC;
    bool            bReadC;
    int             nSenderNameSizeC = sizeof(szSenderNameC); 
    char            szTitleC[128];
    int             nTitleSizeC = sizeof(szTitleC);
    time_t          nSendTimeC;
    time_t          nReceiveTimeC;
    time_t          nLockTimeC; 
    int             nMoneyC;
    int             nItemCountC;
    char            szTextC[1024];
    int             nTextSizeC = sizeof(szTextC);

    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("ID");
    TempFileNameVector.push_back("BaseTime");
    TempFileNameVector.push_back("MailBoxInfo");

    //（10）mailbox表：
    //    a中内容在c中全部存在，ID = a中原来ID +  b原来的总RoleID
    //    MailBoxInfo中，dwLastGlobalMailIndex = b原来的总GlobalMainIndex
    //    SenderID = a中原来的SenderID + b原来的总RoleID
    //    其余字段完全一致

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        lMailboxLastID_B = strtoul((char*)TempInfoVectorB[0].pvData, NULL, 10);

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)  // 遍历a,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckACount++;

        lMailboxLastID_A = strtoul((char*)TempInfoVectorA[0].pvData, NULL, 10);

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorA.size() == TempInfoVectorC.size());

        lMailboxLastID_C = strtoul((char*)TempInfoVectorC[0].pvData, NULL, 10);

        KGLOG_PROCESS_ERROR(lMailboxLastID_C == m_lSequenceRoleID_B + lMailboxLastID_A);

        pParseHandleA = ParseMailBoxInfoData((unsigned char*)TempInfoVectorA[2].pvData, TempInfoVectorA[2].uDataSize);
        KGLOG_PROCESS_ERROR(pParseHandleA);
        pParseHandleC = ParseMailBoxInfoData((unsigned char*)TempInfoVectorC[2].pvData, TempInfoVectorC[2].uDataSize);
        KGLOG_PROCESS_ERROR(pParseHandleC);

        nRetCode = GetMailBaseInfo(pParseHandleC, ulNextMailIDC, ulLastGlobalMailIndexC, usVersionC);
        KGLOG_PROCESS_ERROR(nRetCode);
        
        KGLOG_PROCESS_ERROR(ulLastGlobalMailIndexC == m_GlobalMailIndexB);

        nRetCode = GetMailCount(pParseHandleA, nMailCountA);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetMailCount(pParseHandleC, nMailCountC);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(nMailCountA == nMailCountC);

        for(int i = 0; i < nMailCountA; i++)
        {
            nRetCode = GetMailInfo(pParseHandleA, i, dwMailIDA, ucTypeA, bReadA, 
                szSenderNameA, nSenderNameSizeA, ulSenderIDA,
                szTitleA, nTitleSizeA,
                szTextA, nTextSizeA, nSendTimeA, nReceiveTimeA, nLockTimeA, nMoneyA, nItemCountA
            );
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = GetMailInfo(pParseHandleC, i, dwMailIDC, ucTypeC, bReadC, 
                szSenderNameC, nSenderNameSizeC, ulSenderIDC,
                szTitleC, nTitleSizeC,
                szTextC, nTextSizeC, nSendTimeC, nReceiveTimeC, nLockTimeC, nMoneyC, nItemCountC 
            );
            KGLOG_PROCESS_ERROR(nRetCode);

            KGLOG_PROCESS_ERROR(dwMailIDA == dwMailIDC);
            KGLOG_PROCESS_ERROR(ucTypeA == ucTypeC);
            KGLOG_PROCESS_ERROR(bReadA == bReadC);

            nRetCode = strcmp(szSenderNameA, szSenderNameC);
            KGLOG_PROCESS_ERROR(nRetCode == 0);

            if (ucTypeC == 4)// 是玩家邮件才累加
            {
                KGLOG_PROCESS_ERROR(ulSenderIDA + m_lSequenceRoleID_B == ulSenderIDC);
            }

            nRetCode = strcmp(szTitleA, szTitleC);
            KGLOG_PROCESS_ERROR(nRetCode == 0);

            nRetCode = strcmp(szTextA, szTextC);
            KGLOG_PROCESS_ERROR(nRetCode == 0);

            KGLOG_PROCESS_ERROR(nSendTimeA == nSendTimeC);
            KGLOG_PROCESS_ERROR(nReceiveTimeA == nReceiveTimeC);
            KGLOG_PROCESS_ERROR(nLockTimeA == nLockTimeC);
            KGLOG_PROCESS_ERROR(nMoneyA == nMoneyC);
            KGLOG_PROCESS_ERROR(nItemCountA == nItemCountC);
        }
        EndMailBoxParse(pParseHandleA);
        EndMailBoxParse(pParseHandleC);

    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }

    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckACount + ulHadNowCheckBCount);

    nResult = true;
Exit0:
    EndMailBoxParse(pParseHandleA);
    EndMailBoxParse(pParseHandleC);
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckMapcopyTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("MapID");
    TempFileNameVector.push_back("MapCopyIndex");
    TempFileNameVector.push_back("SceneData");
    TempFileNameVector.push_back("CreateTime");
    TempFileNameVector.push_back("LastModifyTime");

    //（11）mapcopy表：
    //    a中内容全部被丢弃

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }

    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckBCount);

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckPQTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("ID");
    TempFileNameVector.push_back("Data");

    //（12）pq表：
    //    a中内容全部被丢弃

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }
    
    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }

    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckBCount);

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckRoleTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    long lRoleLastID_A = 0;
    long lRoleLastID_C = 0;

    long long llReNameRoleID = 0;

    char szRoleNameA[MYSQL_STRING_MAX_SIZE];
    char szRoleNameB[MYSQL_STRING_MAX_SIZE];
    char szRoleNameC[MYSQL_STRING_MAX_SIZE];

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    PARSEHANDLE pParseHandleA = NULL;
    PARSEHANDLE pParseHandleB = NULL;
    PARSEHANDLE pParseHandleC = NULL;
    PARSEHANDLE pParseHandleA_Base = NULL;

    time_t nLastSaveTimeA = 0;
    time_t nLastSaveTimeB = 0;

    map<string, int>::iterator itA;
    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;
    bool          bIsDataBlockExist   =false;

    TempFileNameVector.push_back("ID");
    TempFileNameVector.push_back("RoleName");
    TempFileNameVector.push_back("ExtInfo");
    TempFileNameVector.push_back("BaseInfo");
    TempFileNameVector.push_back("Account");
    TempFileNameVector.push_back("LastModify");

    //（13）role表：
    //    a中内容在c中全部存在，ID = a中原来ID +  b原来的总RoleID
    //    RoleName：a中的RoleName在b中存在，判断角色的最后保存时间,时间大的,角色名保留,时间小的,角色名更改；不存在，不变
    //    ExtInfo：清除rbtPQList字段相关数据
    //    其余字段完全一致

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c,且在a中找b当前的角色名
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        KGLOG_PROCESS_ERROR(sizeof(szRoleNameB) > TempInfoVectorB[1].uDataSize);
        memset(szRoleNameB, '\0', sizeof(szRoleNameB));
        strncpy(szRoleNameB, (const char*)TempInfoVectorB[1].pvData, TempInfoVectorB[1].uDataSize);

        // 在a中找b的当前角色名
        pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, true, "RoleName", szRoleNameB);
        KGLOG_PROCESS_ERROR(pMySQL_ResA);
        nInitMySQL_ResA = true;

        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)//找不到
        {
            for (size_t i = 0; i < TempInfoVectorB.size(); i++)
            {
                nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
                KGLOG_PROCESS_ERROR(nRetCode == 0);
            }
        }
        else//找到了
        {
            pParseHandleB = ::ParseRoleBaseData((unsigned char*)TempInfoVectorB[3].pvData, TempInfoVectorB[3].uDataSize);
            KGLOG_PROCESS_ERROR(pParseHandleB);

            nRetCode = ::GetLastSaveTime(pParseHandleB, nLastSaveTimeB);
            KGLOG_PROCESS_ERROR(nRetCode);

            ::EndRoleParse(pParseHandleB);

            m_RoleNamesMap[szRoleNameB] = nLastSaveTimeB;

            pParseHandleA_Base = ::ParseRoleBaseData((unsigned char*)TempInfoVectorA[3].pvData, TempInfoVectorA[3].uDataSize);
            KGLOG_PROCESS_ERROR(pParseHandleA_Base);

            nRetCode = ::GetLastSaveTime(pParseHandleA_Base, nLastSaveTimeA);

            ::EndRoleParse(pParseHandleA_Base);

            if (nLastSaveTimeA > nLastSaveTimeB) // a的角色保存时间 比 b大,b要改角色名
            {
                nRetCode = memcmp(TempInfoVectorB[0].pvData, TempInfoVectorC[0].pvData, TempInfoVectorB[0].uDataSize);
                KGLOG_PROCESS_ERROR(nRetCode == 0);//ID段

                llReNameRoleID = strtoul((char*)TempInfoVectorB[0].pvData, NULL, 10);

                KGLOG_PROCESS_ERROR(sizeof(szRoleNameC) > TempInfoVectorC[1].uDataSize);
                memset(szRoleNameC, '\0', sizeof(szRoleNameC));
                strncpy(szRoleNameC, (const char*)TempInfoVectorC[1].pvData, TempInfoVectorC[1].uDataSize);

                string strB = szRoleNameB;
                string strC = szRoleNameC;

                nRetCode = strC.compare(strB);
                KGLOG_PROCESS_ERROR(nRetCode > 0);

                for (size_t i = 2; i < TempInfoVectorB.size(); i++)
                {
                    nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
                    KGLOG_PROCESS_ERROR(nRetCode == 0);
                }
                
                m_RenameRoleMap[llReNameRoleID] = false;
            }
            else
            {
                for (size_t i = 0; i < TempInfoVectorB.size(); i++)
                {
                    nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
                    KGLOG_PROCESS_ERROR(nRetCode == 0);
                }
            }
        }
        if (nInitMySQL_ResA)
        {
            pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
            nInitMySQL_ResA = false;
        }
    }

    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    while(true)  // 遍历a,对比c
    {
        itA = m_RoleNamesMap.end();

        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckACount++;

        lRoleLastID_A = strtoul((char*)TempInfoVectorA[0].pvData, NULL, 10);

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorA.size() == TempInfoVectorC.size());

        lRoleLastID_C = strtoul((char*)TempInfoVectorC[0].pvData, NULL, 10);

        KGLOG_PROCESS_ERROR(lRoleLastID_C == m_lSequenceRoleID_B + lRoleLastID_A);

        if (TempInfoVectorA[2].uDataSize != 0)
        {
            pParseHandleA = ::ParseRoleExtData((unsigned char*)TempInfoVectorA[2].pvData, TempInfoVectorA[2].uDataSize);
            KGLOG_PROCESS_ERROR(pParseHandleA);
            pParseHandleC = ::ParseRoleExtData((unsigned char*)TempInfoVectorC[2].pvData, TempInfoVectorC[2].uDataSize);
            KGLOG_PROCESS_ERROR(pParseHandleC);

            nRetCode = ::IsDataBlockExist(pParseHandleC, 16, bIsDataBlockExist);
            KGLOG_PROCESS_ERROR(nRetCode);

            KGLOG_PROCESS_ERROR(!bIsDataBlockExist);
        }

        KGLOG_PROCESS_ERROR(sizeof(szRoleNameA) > TempInfoVectorA[1].uDataSize);
        memset(szRoleNameA, '\0', sizeof(szRoleNameA));
        strncpy(szRoleNameA, (const char*)TempInfoVectorA[1].pvData, TempInfoVectorA[1].uDataSize);

        itA = m_RoleNamesMap.find(szRoleNameA);
        if (itA != m_RoleNamesMap.end()) //处理重名
        {
            pParseHandleA_Base = ::ParseRoleBaseData((unsigned char*)TempInfoVectorA[3].pvData, TempInfoVectorA[3].uDataSize);
            KGLOG_PROCESS_ERROR(pParseHandleA_Base);

            nRetCode = ::GetLastSaveTime(pParseHandleA_Base, nLastSaveTimeA);
            KGLOG_PROCESS_ERROR(nRetCode);

            ::EndRoleParse(pParseHandleA_Base);

            if (nLastSaveTimeA <= m_RoleNamesMap[szRoleNameA]) //a比b晚,a改名
            {
                llReNameRoleID = strtoul((char*)TempInfoVectorA[0].pvData, NULL, 10);

                m_RenameRoleMap[llReNameRoleID + m_lSequenceRoleID_B] = false;

                KGLOG_PROCESS_ERROR(sizeof(szRoleNameC) > TempInfoVectorC[1].uDataSize);
                memset(szRoleNameC, '\0', sizeof(szRoleNameC));
                strncpy(szRoleNameC, (const char*)TempInfoVectorC[1].pvData, TempInfoVectorC[1].uDataSize);

                string strA = szRoleNameA;
                string strC = szRoleNameC;

                nRetCode = strC.compare(strA);
                KGLOG_PROCESS_ERROR(nRetCode > 0);
            }
            else
            {
                KGLOG_PROCESS_ERROR(sizeof(szRoleNameC) > TempInfoVectorC[1].uDataSize);
                memset(szRoleNameC, '\0', sizeof(szRoleNameC));
                strncpy(szRoleNameC, (const char*)TempInfoVectorC[1].pvData, TempInfoVectorC[1].uDataSize);

                string strA = szRoleNameA;
                string strC = szRoleNameC;

                nRetCode = strC.compare(strA);
                KGLOG_PROCESS_ERROR(nRetCode == 0);
            }
        }

        ::EndRoleParse(pParseHandleA);
        ::EndRoleParse(pParseHandleC);

        for (size_t i = 3; i < TempInfoVectorA.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorA[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorA[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }

    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckACount + ulHadNowCheckBCount);

    nResult = true;
Exit0:
    ::EndRoleParse(pParseHandleA_Base);
    ::EndRoleParse(pParseHandleA);
    ::EndRoleParse(pParseHandleB);
    ::EndRoleParse(pParseHandleC);
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;

}

int KG_DBCheckRule::_CheckRestoreroleTable( KG_TASK_INFO* pTaskInfo )
{
    int nResult = false;
    int nRetCode = false;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;

    bool              bIsNoMoreRecordB = false;
    bool              bIsNoMoreRecordC = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("ID");
    TempFileNameVector.push_back("ExtInfo");
    TempFileNameVector.push_back("Rolename");
    TempFileNameVector.push_back("Account");
    TempFileNameVector.push_back("BaseInfo");
    TempFileNameVector.push_back("DeleteTime");

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    //（14）Restorerole表：
    //    a中内容全部被丢弃

    // 检查 B 和 C 匹配
    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecordB);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (bIsNoMoreRecordB)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecordC);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecordC);
        ulHadNowCheckCCount++;

        // 判断所有字段信息是否完全一致
        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecordC);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecordC)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }

    KGLOG_PROCESS_ERROR(ulHadNowCheckBCount == ulHadNowCheckCCount);
    nResult = true;
Exit0:
    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckRoleblacklistTable( KG_TASK_INFO* pTaskInfo )
{
    int nResult = false;
    int nRetCode = false;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;

    bool              bIsNoMoreRecordA = false;
    bool              bIsNoMoreRecordB = false;
    bool              bIsNoMoreRecordC = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    unsigned long ulLastID_A = 0;
    unsigned long ulLastID_B = 0;
    unsigned long ulLastID_C = 0;

    TempFileNameVector.push_back("ID");
    TempFileNameVector.push_back("FreezeTime");

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    // 检查 B 和 C 匹配
    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecordB);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (bIsNoMoreRecordB)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecordC);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(!bIsNoMoreRecordC);
        ulHadNowCheckCCount++;
        // 判断所有字段信息是否完全一致
        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
        // 保存最后一个ID号
        ulLastID_B = strtoul((char*)TempInfoVectorB[0].pvData, NULL, 10);
        //nRetCode = 
    }

    // 检查 A 和 C 匹配, ID 累加
    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecordA);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (bIsNoMoreRecordA)
        {
            break;
        }
        ulHadNowCheckACount++;

        ulLastID_A = strtoul((char*)TempInfoVectorA[0].pvData, NULL, 10);

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecordC);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecordC);
        ulHadNowCheckCCount++;

        // 判断所有字段信息除ID项累加外,是否完全一致
        KGLOG_PROCESS_ERROR(TempInfoVectorA.size() == TempInfoVectorC.size());

        ulLastID_C = strtoul((char*)TempInfoVectorC[0].pvData, NULL, 10);

        KGLOG_PROCESS_ERROR(ulLastID_C == ulLastID_A + m_lSequenceRoleID_B);

        for (size_t i = 1; i < TempInfoVectorA.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorA[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorA[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecordC);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecordC)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }

    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckACount + ulHadNowCheckBCount);

    nResult = true;
Exit0:
    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;

}

int KG_DBCheckRule::_CheckDeletelistTable( KG_TASK_INFO* pTaskInfo )
{
    int nResult = false;
    int nRetCode = false;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;

    bool              bIsNoMoreRecordA = false;
    bool              bIsNoMoreRecordB = false;
    bool              bIsNoMoreRecordC = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    unsigned long ulLastID_A = 0;
    unsigned long ulLastID_C = 0;

    TempFileNameVector.push_back("ID");
    TempFileNameVector.push_back("EndTime");

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    // 检查 B 和 C 匹配
    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecordB);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecordB)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecordC);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(!bIsNoMoreRecordC);
        ulHadNowCheckCCount++;
        // 判断所有字段信息是否完全一致
        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }

    }

    // 检查 A 和 C 匹配, ID 累加
    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecordA);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecordA)
        {
            break;
        }
        ulHadNowCheckACount++;

        ulLastID_A = strtoul((char*)TempInfoVectorA[0].pvData, NULL, 10);

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecordC);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecordC);
        ulHadNowCheckCCount++;

        // 判断所有字段信息除ID项累加外,是否完全一致
        KGLOG_PROCESS_ERROR(TempInfoVectorA.size() == TempInfoVectorC.size());

        ulLastID_C = strtoul((char*)TempInfoVectorC[0].pvData, NULL, 10);

        KGLOG_PROCESS_ERROR(ulLastID_C == ulLastID_A + m_lSequenceRoleID_B);

        for (size_t i = 1; i < TempInfoVectorA.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorA[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorA[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecordC);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecordC)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }
    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckACount + ulHadNowCheckBCount);

    nResult = true;
Exit0:
    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckStatdatanameindex(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long lRecordCountB = 0;
    unsigned long lRecordCountC = 0;

    TempFileNameVector.push_back("Name");
    TempFileNameVector.push_back("ID");

    //（17）statdatanameindex表：
    //    a中内容全部被丢弃

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        lRecordCountB++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        lRecordCountC++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecord)
        {
            break;
        }
        lRecordCountC++;
    }

    KGLOG_PROCESS_ERROR(lRecordCountC == lRecordCountB);

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckSatadata(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    unsigned long lRecordCountB = 0;
    unsigned long lRecordCountC = 0;

    TempFileNameVector.push_back("StatTime");
    TempFileNameVector.push_back("StatData");

    //（18）statdata表：
    //    a中内容全部被丢弃

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        lRecordCountB++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        lRecordCountC++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());
        
        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        lRecordCountC++;
    }

    KGLOG_PROCESS_ERROR(lRecordCountB == lRecordCountC);

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckTongTable(KG_TASK_INFO* pTaskInfo)
{
    int nResult = false;
    int nRetCode = false;

    long lTongID_A = 0;
    long lTongID_C = 0;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    PARSEHANDLE pParseHandleA = NULL;
    PARSEHANDLE pParseHandleB = NULL;
    PARSEHANDLE pParseHandleC = NULL;

    char    szNameA[32];
    DWORD   dwMasterA;           // 帮主
    int     nFundA;              // 资金
    int     nLevelA;
    int     nDevelopmentPointA;  // 发展点
    WORD    wMaxMemberCountA;    // 成员上限
    BYTE    byTongStateA;
    time_t  nStateTimerA;
    BYTE    byCampA;             // 阵营

    int     nTongMemberCountA;
    DWORD   dwIDA;
    int     nGroupIDA;
    time_t  nJoinTimeA;
    time_t  nLastOfflineTimeA;
    char    szRemarkA[32];
    int     nSalaryA;                            // 工资储蓄(银币)

    char    szNameB[32];
    DWORD   dwMasterB;           // 帮主
    int     nFundB;              // 资金
    int     nLevelB;
    int     nDevelopmentPointB;  // 发展点
    WORD    wMaxMemberCountB;    // 成员上限
    BYTE    byTongStateB;
    time_t  nStateTimerB;
    BYTE    byCampB;             // 阵营


    char    szNameC[32];
    DWORD   dwMasterC;           // 帮主
    int     nFundC;              // 资金
    int     nLevelC;
    int     nDevelopmentPointC;  // 发展点
    WORD    wMaxMemberCountC;    // 成员上限
    BYTE    byTongStateC;
    time_t  nStateTimerC;
    BYTE    byCampC;             // 阵营

    int     nTongMemberCountC;
    DWORD   dwIDC;
    int     nGroupIDC;
    time_t  nJoinTimeC;
    time_t  nLastOfflineTimeC;
    char    szRemarkC[32];
    int     nSalaryC;                            // 工资储蓄(银币)

    map<string, int>::iterator it;

    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("ID");
    TempFileNameVector.push_back("Data");

    //（19）tong表：
    //    a中内容在c中全部存在，ID = a中原来ID + b原来的总tongID
    //    Data：szName：a中的szName在b中存在，加数字后缀；不存在，不变
    //    dwMaster = a中原来dwMaster + b原来的总RoleID
    //    dwID = a中原来dwID + b原来的总RoleID

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c  // 并保存帮会名称
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
        
        pParseHandleB = PaserTongData((unsigned char*)TempInfoVectorB[1].pvData, TempInfoVectorB[1].uDataSize);
        KGLOG_PROCESS_ERROR(pParseHandleB);

        nRetCode = GetTongBaseInfo(pParseHandleB, szNameB, sizeof(szNameB),
            dwMasterB, nFundB, nLevelB, nDevelopmentPointB, wMaxMemberCountB, byTongStateB, nStateTimerB, byCampB
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        m_TongNameMap[szNameB] = true;
    }

    while(true)  // 遍历a,对比c
    {
        it = m_TongNameMap.end();
        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckACount++;

        lTongID_A = strtoul((char*)TempInfoVectorA[0].pvData, NULL, 10);

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorA.size() == TempInfoVectorC.size());

        lTongID_C = strtoul((char*)TempInfoVectorC[0].pvData, NULL, 10);

        KGLOG_PROCESS_ERROR(lTongID_C == m_lSequenceTongID_B + lTongID_A);

        pParseHandleA = PaserTongData((unsigned char*)TempInfoVectorA[1].pvData, TempInfoVectorA[1].uDataSize);
        KGLOG_PROCESS_ERROR(pParseHandleA);
        pParseHandleC = PaserTongData((unsigned char*)TempInfoVectorC[1].pvData, TempInfoVectorC[1].uDataSize);
        KGLOG_PROCESS_ERROR(pParseHandleC);

        nRetCode = GetTongBaseInfo(pParseHandleA, szNameA, sizeof(szNameA),
            dwMasterA, nFundA, nLevelA, nDevelopmentPointA, wMaxMemberCountA, byTongStateA, nStateTimerA, byCampA
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetTongBaseInfo(pParseHandleC, szNameC, sizeof(szNameC),
            dwMasterC, nFundC, nLevelC, nDevelopmentPointC, wMaxMemberCountC, byTongStateC, nStateTimerC, byCampC
        );
        KGLOG_PROCESS_ERROR(nRetCode);

        it = m_TongNameMap.find(szNameA);
        if (it != m_TongNameMap.end())
        {
            string strA = szNameA;
            string strC = szNameC;
            //strA = strA + '1';
            KGLOG_PROCESS_ERROR(strC.find("@") != string::npos);
            KGLOG_PROCESS_ERROR(strC.find(strA.substr(0,2)) != string::npos);
        }
        KGLOG_PROCESS_ERROR(dwMasterC == dwMasterA + m_lSequenceRoleID_B);

        nRetCode = GetTongMemberCount(pParseHandleA, nTongMemberCountA);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = GetTongMemberCount(pParseHandleC, nTongMemberCountC);
        KGLOG_PROCESS_ERROR(nRetCode);

        KGLOG_PROCESS_ERROR(nTongMemberCountA == nTongMemberCountC);

        for(int i = 0; i < nTongMemberCountA; i++)
        {
            nRetCode = GetTongMemberInfo(pParseHandleA, 
                i, dwIDA, nGroupIDA, nJoinTimeA, 
                nLastOfflineTimeA, szRemarkA, 32, nSalaryA
            );
            KGLOG_PROCESS_ERROR(nRetCode);

            nRetCode = GetTongMemberInfo(pParseHandleC, 
                i, dwIDC, nGroupIDC, nJoinTimeC, 
                nLastOfflineTimeC, szRemarkC, 32, nSalaryC
            );
            KGLOG_PROCESS_ERROR(nRetCode);

            KGLOG_PROCESS_ERROR(dwIDC == dwIDA + m_lSequenceRoleID_B);
            KGLOG_PROCESS_ERROR(nGroupIDA == nGroupIDC);
            KGLOG_PROCESS_ERROR(nJoinTimeA == nJoinTimeC);
            KGLOG_PROCESS_ERROR(nLastOfflineTimeA == nLastOfflineTimeC);

            nRetCode = strcmp(szRemarkA, szRemarkC);
            KGLOG_PROCESS_ERROR(nRetCode == 0);

            KGLOG_PROCESS_ERROR(nSalaryA == nSalaryC);
        }
        EndTongParse(pParseHandleA);
        EndTongParse(pParseHandleC);
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckCCount++;
    }

    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckACount + ulHadNowCheckBCount);

    nResult = true;
Exit0:
    EndTongParse(pParseHandleA);
    EndTongParse(pParseHandleB);
    EndTongParse(pParseHandleC);
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;
}

int KG_DBCheckRule::_CheckRenameRoleTable( KG_TASK_INFO* pTaskInfo )
{
    int nResult = false;
    int nRetCode = false;

    long lRoleID_A = 0;
    long lRoleID_C = 0;

    FIELD_NAME_VECTOR TempFileNameVector;
    FIELD_INFO_VECTOR TempInfoVectorA;
    FIELD_INFO_VECTOR TempInfoVectorB;
    FIELD_INFO_VECTOR TempInfoVectorC;
    FIELD_ID_VECTOR   TempIDVector;
    bool              bIsNoMoreRecord = false;

    MYSQL_RES* pMySQL_ResA = NULL;
    MYSQL_RES* pMySQL_ResB = NULL;
    MYSQL_RES* pMySQL_ResC = NULL;

    int nInitMySQL_ResA = false;
    int nInitMySQL_ResB = false;
    int nInitMySQL_ResC = false;

    map<string, int>::iterator it;
    map<long long , bool>::iterator itRename;
    unsigned long ulHadNowCheckACount = 0;
    unsigned long ulHadNowCheckBCount = 0;
    unsigned long ulHadNowCheckCCount = 0;

    TempFileNameVector.push_back("RoleID");

    //（20）renamerole表：

    pMySQL_ResA = pTaskInfo->DBTools[0].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResA);
    nInitMySQL_ResA = true;

    pMySQL_ResB = pTaskInfo->DBTools[1].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResB);
    nInitMySQL_ResB = true;

    pMySQL_ResC = pTaskInfo->DBTools[2].CreateRecordSet(pTaskInfo->strTableName.c_str(), TempFileNameVector, TempIDVector, false, NULL, NULL);
    KGLOG_PROCESS_ERROR(pMySQL_ResC);
    nInitMySQL_ResC = true;

    while(true)  // 遍历b,对比c
    {
        CLEAR_INFOVECTOR(TempInfoVectorB);
        nRetCode = pTaskInfo->DBTools[1].GetNextRecord(pMySQL_ResB, TempInfoVectorB, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckBCount++;

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorB.size() == TempInfoVectorC.size());

        for (size_t i = 0; i < TempInfoVectorB.size(); i++)
        {
            nRetCode = memcmp(TempInfoVectorB[i].pvData, TempInfoVectorC[i].pvData, TempInfoVectorB[i].uDataSize);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }
    }

    while(true)  // 遍历a,对比c
    {
        it = m_TongNameMap.end();
        CLEAR_INFOVECTOR(TempInfoVectorA);
        nRetCode = pTaskInfo->DBTools[0].GetNextRecord(pMySQL_ResA, TempInfoVectorA, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if (bIsNoMoreRecord)
        {
            break;
        }
        ulHadNowCheckACount++;

        lRoleID_A = strtoul((char*)TempInfoVectorA[0].pvData, NULL, 10);

        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(!bIsNoMoreRecord);
        ulHadNowCheckCCount++;

        KGLOG_PROCESS_ERROR(TempInfoVectorA.size() == TempInfoVectorC.size());

        lRoleID_C = strtoul((char*)TempInfoVectorC[0].pvData, NULL, 10);

        KGLOG_PROCESS_ERROR(lRoleID_C == m_lSequenceRoleID_B + lRoleID_A);
    }

    while(true)
    {
        CLEAR_INFOVECTOR(TempInfoVectorC);
        nRetCode = pTaskInfo->DBTools[2].GetNextRecord(pMySQL_ResC, TempInfoVectorC, bIsNoMoreRecord);
        KGLOG_PROCESS_ERROR(nRetCode);
        if(bIsNoMoreRecord)
        {
            break;
        }

        lRoleID_C = strtoul((char*)TempInfoVectorC[0].pvData, NULL, 10);
        
        itRename = m_RenameRoleMap.find(lRoleID_C);
        KGLOG_PROCESS_ERROR(itRename != m_RenameRoleMap.end());
        m_RenameRoleMap[lRoleID_C] = true;

        ulHadNowCheckCCount++;
    }

    for (itRename = m_RenameRoleMap.begin(); itRename != m_RenameRoleMap.end(); ++itRename)
    {
        KGLOG_PROCESS_ERROR(itRename->second == true);
    }
    KGLOG_PROCESS_ERROR(ulHadNowCheckCCount == ulHadNowCheckACount + ulHadNowCheckBCount + m_RenameRoleMap.size());

    nResult = true;
Exit0:
    CLEAR_INFOVECTOR(TempInfoVectorA);
    CLEAR_INFOVECTOR(TempInfoVectorB);
    CLEAR_INFOVECTOR(TempInfoVectorC);

    if (nInitMySQL_ResC)
    {
        pTaskInfo->DBTools[2].FreeRecordSet(pMySQL_ResC);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResB)
    {
        pTaskInfo->DBTools[1].FreeRecordSet(pMySQL_ResB);
        nInitMySQL_ResC = false;
    }
    if (nInitMySQL_ResA)
    {
        pTaskInfo->DBTools[0].FreeRecordSet(pMySQL_ResA);
        nInitMySQL_ResA = false;
    }
    return nResult;

}
