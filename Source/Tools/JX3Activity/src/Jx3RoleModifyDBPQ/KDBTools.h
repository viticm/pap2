//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KDBTools.h
//  Version     :   1.0
//  Creater     :   Liuzhibiao
//  Date        :   2009-7-13
//  Comment     :   访问数据库扩展
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KDBTOOLS_H_
#define _KDBTOOLS_H_

#include "KDBBase.h"

#define MAX_SQL_COMMAND_LEN 102400
#define PQ_PARAM_DB_COUNT 8

#pragma pack(1)

struct KG_JX3_PQ_DB_DATA 
{
    unsigned char   byVersion;
    unsigned long   dwPQID;
    unsigned long   dwPQTemplateID;
    time_t          nBeginTime;
    int             nPQValue[PQ_PARAM_DB_COUNT];
    unsigned char   bFinished;
    unsigned char   byOnwerType;
    unsigned long   dwOnwerID1;
    unsigned long   dwOnwerID2;
    unsigned long   dwOnwerID3;
};

#pragma pack()

class KDBTools: public KDBBase 
{
public:
    KDBTools(void);
    ~KDBTools(void);
    int  Init(char szConnectIP[], char DBAcc[], char szDBPsw[], char szDBName[]);
    void UnInit();

    int ModifyPQTableStartTime(int nYear, int nMonth, int nDay, KDBTools* pdbOutTool);
    int UpdatePQTableStartTime(int nID, unsigned char* ucData, unsigned long ulDataSize);
    int CheckPQBeginTime(int nYear, int nMonth, int nDay, int& nIsOK);

private:
    char m_szSQL[MAX_SQL_COMMAND_LEN];
};

#endif //_KDBTOOLS_H_