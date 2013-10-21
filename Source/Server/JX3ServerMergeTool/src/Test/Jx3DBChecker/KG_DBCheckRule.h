//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KG_DBCheckRule.h
//  Version     :   1.0
//  Creater     :   Liuzhibiao
//  Date        :   2009-12-8
//  Comment     :   
//////////////////////////////////////////////////////////////////////////////////////

#ifndef KG_DBCHECKRULE_H
#define KG_DBCHECKRULE_H

const int CN_RULEFUNC_MAX_COUNT = 20;
const int MYSQL_STRING_MAX_SIZE = 64;

#include<map>

struct KG_TASK_INFO;

class KG_DBCheckRule
{
public:
    int InitDBCheckRule();
    int RuleCheck(string strTableName, KG_TASK_INFO* pTaskInfo);
    static void UnInitDBCheckRule();

    static int m_snFuncIndex;
private:
    typedef int (KG_DBCheckRule::*RULE_FUNC)(KG_TASK_INFO* pTaskInfo);

    RULE_FUNC   m_RuleFunc[CN_RULEFUNC_MAX_COUNT];

private:

    int _CheckSequenceTable(KG_TASK_INFO* pTaskInfo);
    int _CheckActivityTable(KG_TASK_INFO* pTaskInfo);
    int _CheckAntifarmerTable(KG_TASK_INFO* pTaskInfo);
    int _CheckAuctionTable(KG_TASK_INFO* pTaskInfo);
    int _CheckFellowshipTable(KG_TASK_INFO* pTaskInfo);
    int _CheckGameCardTable(KG_TASK_INFO* pTaskInfo);
    int _CheckGlobalcustomdataTable(KG_TASK_INFO* pTaskInfo);
    int _CheckGlobalsystemvalue(KG_TASK_INFO* pTaskInfo);
    int _CheckGlobalmailex(KG_TASK_INFO* pTaskInfo);
    int _CheckMailBoxTable(KG_TASK_INFO* pTaskInfo);
    int _CheckMapcopyTable(KG_TASK_INFO* pTaskInfo);
    int _CheckPQTable(KG_TASK_INFO* pTaskInfo);
    int _CheckRoleTable(KG_TASK_INFO* pTaskInfo);
    int _CheckRestoreroleTable(KG_TASK_INFO* pTaskInfo);
    int _CheckRoleblacklistTable(KG_TASK_INFO* pTaskInfo);
    int _CheckDeletelistTable(KG_TASK_INFO* pTaskInfo);
    int _CheckStatdatanameindex(KG_TASK_INFO* pTaskInfo);
    int _CheckSatadata(KG_TASK_INFO* pTaskInfo);
    int _CheckTongTable(KG_TASK_INFO* pTaskInfo);
    int _CheckRenameRoleTable(KG_TASK_INFO* pTaskInfo);

private:
    typedef std::map<string, int> KRuleTable;
    KRuleTable     m_RuleTable;

    long m_lRoleID_A;
    long m_lSequenceRoleID_B;
    long m_lRoleID_C;

    long m_lTongID_A;
    long m_lSequenceTongID_B;
    long m_lTongID_C;

    map<string, int> m_RoleNamesMap;
    map<string, int> m_TongNameMap;
    map<long long, bool>  m_RenameRoleMap;
    unsigned long m_GlobalMailIndexB;
};

#endif //KG_DBCHECKRULE_H