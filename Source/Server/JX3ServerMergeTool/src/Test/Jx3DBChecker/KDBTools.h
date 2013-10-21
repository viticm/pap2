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

////////////////////////////////////////////////////////////////////////////////
#define KGD_MYSQL_PROCESS_ERROR(Condition, pMySQL, pszMsg)  \
    do                                                          \
    {                                                           \
        if (!(Condition))                                       \
        {                                                       \
            KGLogPrintf(                                        \
                KGLOG_DEBUG,                                    \
            "KGD_MYSQL_PROCESS_ERROR at line %d in (0x%p)->%s(), \"%s\", \"%s\"", \
            __LINE__, this, KG_FUNCTION, mysql_error(pMySQL), (pszMsg)            \
            );                                                  \
            goto Exit0;                                         \
        }                                                       \
    } while (false)

////////////////////////////////////////////////////////////////////////////////
typedef std::vector<std::string> FIELD_NAME_VECTOR;
typedef std::vector<unsigned> FIELD_ID_VECTOR;

struct FIELD_INFO
{
    unsigned  uDataSize;
    void     *pvData;
};
typedef std::vector<FIELD_INFO> FIELD_INFO_VECTOR;

struct RECORD_INFO
{
    FIELD_INFO_VECTOR  FieldInfoVector;
};
typedef std::vector<RECORD_INFO> RECORD_LIST;

class KDBTools: public KDBBase 
{
public:
    KDBTools(void);
    ~KDBTools(void);
    int  Init(char szConnectIP[], char DBAcc[], char szDBPsw[], char szDBName[]);
    void UnInit();

    int HadRecordInTable(char szTableName[], char szGetFieldName[],
        char szSearchFieldName[], int nID,
        int& nHadFind
        );
    int  GetInfo(char szTableName[], char szGetFieldName[], 
        char szSearchFieldName[], char szSearchName[], 
        BYTE* pbyData, size_t uDataSize, size_t& uUseSize, int& nHadFind
        );

    int DelInfoIfFind(char szTableName[], char szGetFieldName[],
        char szSearchFieldName[], char szSearchName[],
        int& nHadFind
        );

    int InsertNewRecord(const char cszTableName[], const FIELD_NAME_VECTOR& crFieldNameVector, const FIELD_INFO_VECTOR& crFieldInfoVector, int& rnSucceed);
    //int SaveTheSeashRecord(char szTableName[], char szSearchFieldName[],
    //    char szSearchName[], RECORD_LIST& InsertList, 
    //    FIELD_NAME_VECTOR& refRetFieldNameVector, int& nHadFind
    //);

    int GetFieldsName(const char cszTableName[], FIELD_NAME_VECTOR& refRetFieldNameVector);
    MYSQL_RES* CreateRecordSet(const char cszTableName[], const FIELD_NAME_VECTOR &crFieldNameVector, const FIELD_ID_VECTOR &crOrderByFields,
        const bool bHasCondition,
        const char cszSearchFieldName[],
        const char cszSearchThing[]);

    //unsigned long GetNumOfTheRecordSet(MYSQL_RES* pmySql_res);

    int GetNextRecord(MYSQL_RES *pRecordSet, FIELD_INFO_VECTOR& refFieldInfoVector, bool& refbIsNoMoreRecodes);
    int FreeRecordSet(MYSQL_RES*& refpRecordSet);
    int LockTable(const char cszTableName[]);
    int UnLockTable();

    int UpDateField(char szTableName[], char szUpdateFieldName[], 
        unsigned char* ucUpdateData, unsigned int unUndateDataSize,
        char szSearchFieldName[], int nID,
        int& nHadFind
        );

    int UpDateField(char szTableName[], char szUpdateFieldName[], 
        unsigned char* ucUpdateData, unsigned int unUndateDataSize,
        char szSearchFieldName[], char szSearchName[],
        int& nHadFind
        );
private:
    char m_szSQL[MAX_SQL_COMMAND_LEN];
};

#endif //_KDBTOOLS_H_