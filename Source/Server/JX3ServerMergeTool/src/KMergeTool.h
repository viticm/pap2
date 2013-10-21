#ifndef _KMERGE_TOOLS_H_
#define _KMERGE_TOOLS_H_

#include <map>
#include <set>
#include "mysql.h"
#include "KMergeSettings.h"

#define MAX_DB_SQL_SIZE (1024 * 1024 * 8)

#define SEQ_TABLE_NAME  "sequence"
#define SEQ_NAME        "seqname"
#define SEQ_CURRENT_ID  "currentid"

#define SEQUENCE_ROLE_ID "RoleID"
#define SEQUENCE_TONG_ID "TongID"
#define SEQUENCE_GLOBAL_MAIL_INDEX "GlobalMailIndex"

class KMergeTool
{
public:
    KMergeTool();

    BOOL Init();
    void UnInit();

    BOOL Run();

private:
    BOOL DoSrcQuery(const char cszSQL[]);
    BOOL DoDstQuery(const char cszSQL[]);
    
    BOOL LoadDstSequenceValue(const char cszSeqName[], DWORD* pdwRetValue);
    BOOL LoadSrcSequenceValue(const char cszSeqName[], DWORD* pdwRetValue);
    BOOL LoadDstMaxID();
    BOOL LoadDstRoleName();
    const char* GetTongName(BYTE* pbyData, size_t uDataLen);
    BOOL LoadDstTongName();
    BOOL ConvertTong(BYTE* pbyData, size_t uDataLen);
    
    BOOL SetDstSequenceValue(const char szSequenceName[], DWORD dwSeqCount);
    BOOL MergeSequence();
    BOOL MergeActivity();
    BOOL MergeAntiFarmer();
    BOOL MergeAuction();
    BOOL MergeFellowship();
    BOOL MergeGameCard();
    BOOL MergeMailBox();
    BOOL MergeRenameRole();
    BOOL MergeRole();
    BOOL MergeRestoreRole();
    BOOL MergeRoleBlackList();
    BOOL MergeRoleDeleteList();
    BOOL MergeTong();

private:
    MYSQL* m_pSrcDBHandle;
    MYSQL* m_pDstDBHandle;
    char m_szSQL[MAX_DB_SQL_SIZE];

    KMergeSettings  m_Settings;

    DWORD           m_dwDstMaxRoleID;
    DWORD           m_dwDstMaxTongID;
    DWORD           m_dwDstMaxGlobalMailIndex;

    struct KDstDBRoleInfo
    {
        DWORD   dwID;
        time_t  nLastSaveTime;
    };
    typedef std::map<kstring, KDstDBRoleInfo, less<kstring>, KMemory::KAllocator<kstring> > KROLE_STAT_TABLE;
    typedef std::set<kstring, less<kstring>, KMemory::KAllocator<kstring> >KTONG_STAT_TABLE;

    KROLE_STAT_TABLE m_RoleNameTable;
    KTONG_STAT_TABLE m_TongNameTable;
};

extern KMergeTool* g_pMergeTool;

#define PROGRESS_MONITER_INIT()                                         \
    int nProgressCount = 0;                                             \
    int nProgressSize  = 0;

#define PROGRESS_MONITER_START(szTableName)                             \
{                                                                       \
    nRetCode = snprintf(m_szSQL, sizeof(m_szSQL),                       \
        "select count(*) from %s;", szTableName                         \
    );                                                                  \
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < sizeof(m_szSQL));    \
    bRetCode = DoSrcQuery(m_szSQL);                                     \
    KGLOG_PROCESS_ERROR(bRetCode);                                      \
    pQueryRes = mysql_store_result(m_pSrcDBHandle);                     \
    KGLOG_PROCESS_ERROR(pQueryRes);                                     \
    QueryRow = mysql_fetch_row(pQueryRes);                              \
    KGLOG_PROCESS_ERROR(QueryRow);                                      \
    nProgressSize = (int)strtoul(QueryRow[0], NULL, 10);                \
    MYSQL_FREE_RESULT(pQueryRes);                                       \
    printf("%3d%%", 0);                                                 \
}

#define PROGRESS_MONITER_INCREASE()                                                             \
    assert(nProgressSize != 0);                                                                 \
    if ((nProgressCount * 100 / nProgressSize) - (++nProgressCount  * 100 / nProgressSize) < 1) \
    {                                                                                           \
        printf("\r%3d%%", nProgressCount  * 100 / nProgressSize);                               \
    }

#define PROGRESS_MONITER_FINISH() printf("\r100%%\n");

#endif // _KMERGE_TOOLS_H_
