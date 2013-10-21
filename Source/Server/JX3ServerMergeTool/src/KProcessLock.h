#ifndef _KPROCESS_LOCK_H_
#define _KPROCESS_LOCK_H_

class KProcessLock
{
public:
    KProcessLock();
    BOOL CreateLock(const char cszLockName[]);
    void FreeLock();
private:
#ifdef WIN32
    HANDLE m_hLockFile;
#else
    int m_nLockFile;
#endif
};

#endif // _KPROCESS_LOCK_H_