#include "stdafx.h"
#include "KMergeTool.h"
#include "KProcessLock.h"

KMergeTool* g_pMergeTool = NULL;

#define LOCK_FILE_NAME "MergeProcessLock.lock"

int main(int argc, char* argv[])
{
    BOOL            bRetCode            = false;
    int             nRetCode            = 0;
    const char*     pcszRetCWD          = NULL;
    BOOL            bLockInitFlag       = false;
    BOOL            bMergeToolInitFlag  = false;
    BOOL            bMemoryPoolInit     = false;
    BOOL            bLogInitFlag        = false;
    char            szCWD[MAX_PATH];
    KProcessLock    Lock;
    KGLOG_PARAM     LogParam;
    
    g_SetRootPath();
    
    bMemoryPoolInit = KMemory::Initialize("MergeTool.memory");
    KG_PROCESS_ERROR(bMemoryPoolInit);

    pcszRetCWD = getcwd(szCWD, sizeof(szCWD));    
    KG_PROCESS_ERROR(pcszRetCWD);

    memset(&LogParam, 0, sizeof(LogParam));
    snprintf(LogParam.szPath, sizeof(LogParam.szPath), "%s/logs", szCWD);
    strcpy(LogParam.szIdent, "MergeTool");
    LogParam.nMaxLineEachFile = 65536 * 2;
    LogParam.Options = (KGLOG_OPTIONS)(KGLOG_OPTION_FILE | KGLOG_OPTION_CONSOLE);

    bLogInitFlag = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(bLogInitFlag);

    bLockInitFlag = Lock.CreateLock(LOCK_FILE_NAME);
    KG_PROCESS_ERROR(bLockInitFlag);

    g_pMergeTool = KMemory::New<KMergeTool>();
    KG_PROCESS_ERROR(g_pMergeTool);

    bMergeToolInitFlag = g_pMergeTool->Init();
    KGLOG_PROCESS_ERROR(bMergeToolInitFlag);

    bRetCode = g_pMergeTool->Run();
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    if (bMergeToolInitFlag)
    {
        g_pMergeTool->UnInit();
        bMergeToolInitFlag = false;
    }
    
    if (g_pMergeTool)
    {
        KMemory::Delete(g_pMergeTool);
        g_pMergeTool = NULL;
    }
    
    if (bLogInitFlag)
    {
        KGLogUnInit(NULL);
        bLogInitFlag = false;
    }

    if (bMemoryPoolInit)
    {
        KMemory::Finalize();
        bMemoryPoolInit = false;
    }

    if (bLockInitFlag)
    {
        Lock.FreeLock();
        bLockInitFlag = false;
    }

	return 0;
}

