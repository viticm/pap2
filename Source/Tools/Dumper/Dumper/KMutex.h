//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMutex.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KMutex_H
#define KMutex_H
//---------------------------------------------------------------------------

#ifdef WIN32
#include <Windows.h>
#include <process.h>

class KMutex
{
public:
    KMutex()        { InitializeCriticalSection(&m_CriticalSection);            }
    ~KMutex()       { DeleteCriticalSection(&m_CriticalSection);                }
    int Lock()      { EnterCriticalSection(&m_CriticalSection); return true;    }
    int Unlock()    { LeaveCriticalSection(&m_CriticalSection); return true;    }

private:
    CRITICAL_SECTION m_CriticalSection;
};


#else   // POSIX

#include <pthread.h>

class KMutex
{
public:
    KMutex()        { pthread_mutex_init(&m_Mutex, NULL);                                             }
    ~KMutex()       { pthread_mutex_destroy(&m_Mutex);                                          }
    int Lock()      { int nRetCode = pthread_mutex_lock(&m_Mutex);      return (nRetCode == 0); }
    int Unlock()    { int nRetCode = pthread_mutex_unlock(&m_Mutex);    return (nRetCode == 0); }

private:
    pthread_mutex_t m_Mutex;
};

#endif

#endif
