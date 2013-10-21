//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KThread.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:
//---------------------------------------------------------------------------
#include "stdafx.h"
#include "KGPublic.h"
#include "KThread.h"

//---------------------------------------------------------------------------
// 函数:	~KThread
// 功能:	构造函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KThread::KThread()
{
	#ifdef WIN32
		m_ThreadHandle      = NULL;
	#else
		m_ThreadHandle      = 0;
	#endif
	m_pfnThread         = NULL;
    m_pvThreadArg       = NULL;
}

//---------------------------------------------------------------------------
// 函数:	~KThread
// 功能:	析购函数
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
KThread::~KThread()
{
    Destroy();
}
//---------------------------------------------------------------------------
// 函数:	MyThreadProc
// 功能:	线程过程
// 参数:	pvArg       参数
// 返回:	void
//---------------------------------------------------------------------------
#ifdef WIN32
static unsigned __stdcall MyThreadProc(void *pvArg)
#else
static void *MyThreadProc(void *pvArg)
#endif
{
	KThread *pThread = (KThread *)pvArg;
	
	pThread->ThreadFunction();
	
    return 0;
}

//---------------------------------------------------------------------------
// 函数:	ThreadFunction
// 功能:
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
void KThread::ThreadFunction()
{
	m_pfnThread(m_pvThreadArg);
}

//---------------------------------------------------------------------------
// 函数:	Create
// 功能:	创建线程
// 参数:	pfnThread		线程函数
//			pvArg 		    线程参数
// 返回:	int
//---------------------------------------------------------------------------
int KThread::Create(KThreadFunction *pfnThread, void *pvArg)
{
	m_pfnThread     = pfnThread;
	m_pvThreadArg   = pvArg;
	
	if (m_ThreadHandle)
	    return false;

#ifdef WIN32
    unsigned uThreadID;
	m_ThreadHandle = (HANDLE)_beginthreadex(
		NULL,			        // SD
		0,				        // initial stack size
		MyThreadProc,		    // thread function
		(void *)this,	        // thread argument
		0,				        // creation option
		(unsigned *)&uThreadID   // thread identifier
    );	
	return (m_ThreadHandle != NULL);
#else
    int nRetCode = pthread_create(
        &m_ThreadHandle, 
        NULL, 
        MyThreadProc, 
        this
    );

    if (nRetCode != 0) // if fail
	{
#ifdef WIN32
		m_ThreadHandle      = NULL;
#else
		m_ThreadHandle      = 0;
#endif
	}
        
	return (nRetCode == 0);
#endif
}

//---------------------------------------------------------------------------
// 函数:	Destroy
// 功能:	退出线程
// 参数:	void
// 返回:	void
//---------------------------------------------------------------------------
int KThread::Destroy()
{
    int nResult = false;
    
    KG_PROCESS_ERROR(m_ThreadHandle);
    
    
#ifdef WIN32
    if (m_ThreadHandle)
    {
	    WaitForSingleObject(m_ThreadHandle, INFINITE);
    	
		CloseHandle(m_ThreadHandle);
#ifdef WIN32
		m_ThreadHandle      = NULL;
#else
		m_ThreadHandle      = 0;
#endif
	}
#else
    if (m_ThreadHandle)
    {
		pthread_join(m_ThreadHandle, NULL);
#ifdef WIN32
		m_ThreadHandle      = NULL;
#else
		m_ThreadHandle      = 0;
#endif
    }
#endif

    nResult = true;
Exit0:
    return nResult;
}

int KGThread_Sleep(unsigned uMilliseconds)
{
    #ifdef WIN32
    Sleep(uMilliseconds);
    return true;
    #else
    int nRetCode = false;
    nRetCode = usleep(uMilliseconds * 1000);
    return (nRetCode == 0);
    #endif
}
