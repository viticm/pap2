#include "stdafx.h"
#include "KProcessLock.h"
#ifdef WIN32    
#else
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#endif

#ifdef WIN32
KProcessLock::KProcessLock()
{
    m_hLockFile = INVALID_HANDLE_VALUE;
}

BOOL KProcessLock::CreateLock(const char cszLockName[])
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    m_hLockFile = CreateFile(cszLockName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0);
    KGLOG_PROCESS_ERROR(m_hLockFile != INVALID_HANDLE_VALUE);

    bRetCode = LockFile(m_hLockFile, 0, 0, 1, 0);

    if (!bRetCode)
    {
        KGLogPrintf(KGLOG_ERR, "Already start another process!");
        goto Exit0;
    }

    bResult = true;
Exit0:
    if (!bResult && m_hLockFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hLockFile);
        m_hLockFile =INVALID_HANDLE_VALUE;
    }
    return bResult;
}

void KProcessLock::FreeLock()
{
    BOOL bRetCode = false;
    
    bRetCode = UnlockFile(m_hLockFile, 0, 0, 1, 0);
    KGLOG_PROCESS_ERROR(bRetCode);

    if (m_hLockFile)
    {
        bRetCode = CloseHandle(m_hLockFile);
        KGLOG_PROCESS_ERROR(bRetCode);
        m_hLockFile = INVALID_HANDLE_VALUE;
    }
Exit0:
    return;
}

#else
KProcessLock::KProcessLock()
{
    m_nLockFile = -1;
}

BOOL KProcessLock::CreateLock(const char cszLockName[])
{
    BOOL            bResult     = false;
    int             nRetCode    = 0;
    struct flock    LockInfo;

    KGLOG_PROCESS_ERROR(m_nLockFile == -1);

    m_nLockFile = creat(cszLockName, S_IRWXU);
    KGLOG_PROCESS_ERROR(m_nLockFile != -1);

    LockInfo.l_type   = F_WRLCK;
    LockInfo.l_whence = SEEK_SET;
    LockInfo.l_start  = 0;
    LockInfo.l_len    = 0;

    nRetCode = fcntl(m_nLockFile, F_SETLK, &LockInfo);

    if (nRetCode == -1)
    {
        KGLOG_PROCESS_ERROR (errno == EACCES || errno == EAGAIN);
        KGLogPrintf(KGLOG_ERR, "Already start another process!");
        goto Exit0;
    }

    bResult = true;
Exit0:
    if (!bResult && m_nLockFile != -1)
    {
        close(m_nLockFile);
        m_nLockFile = -1;
    }
    return bResult;
}

void KProcessLock::FreeLock()
{
    int             nRetCode = 0;
    struct flock    LockInfo;

    LockInfo.l_type     = F_UNLCK;
    LockInfo.l_whence   = SEEK_SET;
    LockInfo.l_start    = 0;
    LockInfo.l_len      = 0;

    LockInfo.l_type     = F_UNLCK;
    LockInfo.l_whence   = SEEK_SET;
    LockInfo.l_start    = 0;
    LockInfo.l_len      = 0;
    
    nRetCode = fcntl(m_nLockFile, F_SETLK, &LockInfo);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    nRetCode = close(m_nLockFile);
    KGLOG_PROCESS_ERROR(nRetCode != -1);
    m_nLockFile = -1;
Exit0:
    return;
}
#endif
