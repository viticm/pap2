//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KThread.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis), Freeway Chen
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KThread_H
#define KThread_H
//---------------------------------------------------------------------------

typedef void KThreadFunction(void *pvArg);

#ifdef WIN32
#include "process.h"
#else
#include <unistd.h>
#include <pthread.h>
#endif


//---------------------------------------------------------------------------
class KThread
{
public:
    int             Create(KThreadFunction *pfnThread, void *pvArg);
    int             Destroy();

public:
	KThread();
	~KThread();

    void            ThreadFunction();

private:
    #ifdef WIN32
    HANDLE          m_ThreadHandle;
    #else
    pthread_t       m_ThreadHandle;  
    #endif

	KThreadFunction *m_pfnThread;
    void            *m_pvThreadArg;
};

int KGThread_Sleep(unsigned uMilliseconds);

#ifdef WIN32

inline void *KGThread_GetSelfId()
{
    return (void *)((char *)NULL + GetCurrentThreadId());   // for no 64 bit check warning
}

#else     //linux

inline void *KGThread_GetSelfId()
{
    return (void *)pthread_self();
}

#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
