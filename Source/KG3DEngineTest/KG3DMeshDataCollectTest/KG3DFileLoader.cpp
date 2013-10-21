#include "StdAfx.h"
#include "KG3DFileLoader.h"

////////////////////////////////////////////////////////////////////////////////
KG3DFileLoader g_FileLoader;

////////////////////////////////////////////////////////////////////////////////
struct _TASK
{
    FILE_LOAD_STATE eState ;
    FILE_TYPE eType;
    // by using reference count, we do not need care about whether the task is
    // still in request queue or is loading or is finished.
    unsigned long  ulRefCount; 
    // -------------------- in -------------------- 
    char szFileName[MAX_PATH];
    // -------------------- out -------------------- 
    IKG_Buffer *piFileData;
    IIniFile *piIniFile;
};
////////////////////////////////////////////////////////////////////////////////
static inline _TASK *_TaskCreate(const char cszFileName[], FILE_TYPE eType)
{
    int nResult  = false;
    _TASK *pTask = NULL;
    size_t uFileNameLength = 0;

    KG_ASSERT_EXIT(cszFileName);

    uFileNameLength = strlen(cszFileName);
    KG_ASSERT_EXIT(uFileNameLength < sizeof(pTask->szFileName));

    //pTask = (_TASK *)RawAlloc(sizeof(_TASK));
    pTask = new _TASK();
    KG_ASSERT_EXIT(pTask);

    // init
    pTask->eState = FILE_LOAD_STATE_WAIT;
    pTask->eType  = eType;
    pTask->ulRefCount = 1;
    memcpy(pTask->szFileName, cszFileName, uFileNameLength + 1);
    pTask->piFileData = NULL;
    pTask->piIniFile  = NULL;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if(pTask)
        {
            //RawFree(pTask, sizeof(*pTask));
            delete pTask;
            pTask = NULL;
        }
    }
    return pTask;
}
////////////////////////////////////////////////////////////////////////////////
static inline unsigned long _TaskAddRef(_TASK *pTask)
{
    ASSERT(pTask);
    return (unsigned long)KG_InterlockedIncrement((long *)&(pTask->ulRefCount));
}
////////////////////////////////////////////////////////////////////////////////
static inline unsigned long _TaskRelease(_TASK *pTask)
{
    ASSERT(pTask);
    unsigned long ulNewRefCount = (unsigned long)KG_InterlockedDecrement((long *)&(pTask->ulRefCount));
    if (ulNewRefCount == 0)
    {
        if(pTask)
        {
            KG_COM_RELEASE(pTask->piFileData);
            KG_COM_RELEASE(pTask->piIniFile);
            //RawFree(pTask, sizeof(*pTask));
            delete pTask;
            pTask = NULL;
        }
    }
    return ulNewRefCount;
}
////////////////////////////////////////////////////////////////////////////////
KG3DFileLoader::KG3DFileLoader(void):
m_nExitFlag(false)
{
}

KG3DFileLoader::~KG3DFileLoader(void)
{
}
////////////////////////////////////////////////////////////////////////////////
int KG3DFileLoader::Init()
{
    int nResult  = false;
    int nRetCode = false;
    int nWorkThreadCreateFlag = false;

    nRetCode = m_WorkThread.Create(_WorkThread, this);
    KGLOG_PROCESS_ERROR(nRetCode);
    nWorkThreadCreateFlag = true;

    nResult = true;
Exit0:
    if (!nResult)
    {
        m_nExitFlag = true;
        m_WorkThread.Destroy();
        nWorkThreadCreateFlag = false;
    }
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DFileLoader::UnInit()
{
    m_nExitFlag = true;
    m_WorkThread.Destroy();

#ifdef _DEBUG
    ASSERT(m_TaskSet.empty() && "Some where forget call DeleteTask()");    
#endif

    while (!m_RequestQueue.empty())
    {
        _TASK *pTask = m_RequestQueue.front();
        m_RequestQueue.pop();

        if (pTask)
        {
            _TaskRelease(pTask);
            pTask = NULL;
        }
    }    
    return true;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DFileLoader::LoadImmediately(const char cszFileName[], IKG_Buffer **ppiRetFileContent)
{
    int nResult  = false;
    int nRetCode = false;
    IFile *piFile = NULL;
    IKG_Buffer *piFileBuffer = NULL;
    unsigned uFileSize = 0;
    void *pvFileBuffer = NULL;

    KG_ASSERT_EXIT(cszFileName);
    KG_ASSERT_EXIT(ppiRetFileContent);

    piFile = g_OpenFile(cszFileName);
    //KGLOG_PROCESS_ERROR(piFile);
    KG_PROCESS_ERROR(piFile); // no log, because maybe use to try load, (example: try load a INI file if exist)

    uFileSize = (unsigned)piFile->Size();
    KGLOG_PROCESS_ERROR(uFileSize > 0);

    piFileBuffer = KG_MemoryCreateBuffer(uFileSize);
    KGLOG_PROCESS_ERROR(piFileBuffer);

    pvFileBuffer = piFileBuffer->GetData();
    ASSERT(pvFileBuffer);

    nRetCode = (int)piFile->Read(pvFileBuffer, uFileSize);
    KGLOG_PROCESS_ERROR(nRetCode == (int)uFileSize);

    (*ppiRetFileContent) = piFileBuffer;
    (*ppiRetFileContent)->AddRef();
    nResult = true;
Exit0:
    KG_COM_RELEASE(piFileBuffer);
    KG_COM_RELEASE(piFile);
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
HANDLE KG3DFileLoader::CreateTask(const char cszFileName[], FILE_TYPE eType)
{
    HANDLE hResult = NULL;
    _TASK *pTask = NULL;

    KG_ASSERT_EXIT(cszFileName);

    pTask = _TaskCreate(cszFileName, eType);
    KG_ASSERT_EXIT(pTask);

    _TaskAddRef(pTask);
    m_RequestQueueLock.Lock();
    m_RequestQueue.push(pTask);
    m_RequestQueueLock.Unlock();

#ifdef _DEBUG
    m_TaskSetLock.Lock();
    {
        std::set<_TASK *>::_Pairib ib = m_TaskSet.insert(pTask);
        ASSERT(ib.second);
    }
    m_TaskSetLock.Unlock();
#endif

    hResult = pTask;
    pTask = NULL;
Exit0:
    if (pTask)
    {
        _TaskRelease(pTask);
        pTask = NULL;
    }
    return hResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DFileLoader::DeleteTask(HANDLE hHandle)
{
    int nResult = false;
    _TASK *pTask = NULL;

    KG_ASSERT_EXIT(hHandle);
    pTask = (_TASK *)hHandle;

#ifdef _DEBUG
    m_TaskSetLock.Lock();
    {
        std::set<_TASK *>::iterator it = m_TaskSet.find(pTask);
        ASSERT(it != m_TaskSet.end());
        m_TaskSet.erase(it);
    }
    m_TaskSetLock.Unlock();
#endif

    if (pTask)
    {
        _TaskRelease(pTask);
        pTask = NULL;
    }

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DFileLoader::CheckResult(HANDLE hHandle, FILE_LOAD_STATE *peRetState)
{
    int nResult = false;
    _TASK *pTask = NULL;

    KG_ASSERT_EXIT(hHandle);
    KG_ASSERT_EXIT(peRetState);

    pTask = (_TASK *)hHandle;
    *peRetState = pTask->eState;

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DFileLoader::FetchResultNormal(
                                      HANDLE hHandle, FILE_LOAD_STATE *peRetState, IKG_Buffer **ppiRetFileContent
                                      )
{
    int nResult = false;
    _TASK *pTask = NULL;

    KG_ASSERT_EXIT(hHandle);
    KG_ASSERT_EXIT(peRetState);
    KG_ASSERT_EXIT(ppiRetFileContent);

    ASSERT(!*ppiRetFileContent);

    pTask = (_TASK *)hHandle;

    *peRetState          = pTask->eState;
    (*ppiRetFileContent) = pTask->piFileData;
    if ((*ppiRetFileContent))
    {
        (*ppiRetFileContent)->AddRef();
    }
    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DFileLoader::FetchResultIni(
                                   HANDLE hHandle, FILE_LOAD_STATE *peRetState, IIniFile **ppiRetFile
                                   )
{
    int nResult = false;
    _TASK *pTask = NULL;

    KG_ASSERT_EXIT(hHandle);
    KG_ASSERT_EXIT(peRetState);
    KG_ASSERT_EXIT(ppiRetFile);

    ASSERT(!*ppiRetFile);

    pTask = (_TASK *)hHandle;

    *peRetState = pTask->eState;
    *ppiRetFile = pTask->piIniFile;
    pTask->piIniFile = NULL; // Not COM interface

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
void KG3DFileLoader::_WorkThread(void *pvThis)
{
    ASSERT(pvThis);
    ((KG3DFileLoader *)pvThis)->_ThreadFunction();
}
////////////////////////////////////////////////////////////////////////////////
int KG3DFileLoader::_ThreadFunction()
{
    int nResult  = false;
    int nRetCode = false;
    _TASK *pTask = NULL;

    while (!m_nExitFlag)
    {
        ::Sleep(1); // the max load speed can't over 1000/second because we sleep after per file be load

        // -------------------- get task --------------------
        m_RequestQueueLock.Lock();
        if (!m_RequestQueue.empty())
        {
            pTask = m_RequestQueue.front();
            m_RequestQueue.pop();
        }    
        m_RequestQueueLock.Unlock();

        if (!pTask)
            continue;

        // load if not call Delete() before pop
        if (pTask->ulRefCount > 1)
        {
            if (pTask->eType == FILE_TYPE_INI)
            {
                pTask->piIniFile = g_OpenIniFile(pTask->szFileName);
                if (pTask->piIniFile)
                    pTask->eState = FILE_LOAD_STATE_SUCCESS;
                else
                    pTask->eState = FILE_LOAD_STATE_FAILED;
            }
            else
            {
                nRetCode = LoadImmediately(pTask->szFileName, &(pTask->piFileData));
                if (nRetCode)
                    pTask->eState = FILE_LOAD_STATE_SUCCESS;
                else
                    pTask->eState = FILE_LOAD_STATE_FAILED;
            }
        }

        if (pTask)
        {
            _TaskRelease(pTask);
            pTask = NULL;
        }
    }

    nResult = true;
    //Exit0:
    if (pTask)
    {
        _TaskRelease(pTask);
        pTask = NULL;
    }
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Helper class for read data from buffer
////////////////////////////////////////////////////////////////////////////////
KG3DBufferReader::KG3DBufferReader()
{
    m_pbyNextReadPos = NULL;
    m_uUnreadSize    = 0;
    m_pbyBuffer		 = NULL;
    m_uBufferSize	 = 0;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DBufferReader::SetBuffer(IKG_Buffer *piBuffer)
{
    int nResult = false;

    KG_ASSERT_EXIT(piBuffer);
    m_pbyBuffer   = (unsigned char *)piBuffer->GetData();
    m_uBufferSize = piBuffer->GetSize();

    m_pbyNextReadPos = m_pbyBuffer;
    m_uUnreadSize    = m_uBufferSize;

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DBufferReader::Read(void *pvRetDateBuf, size_t uSize)
{
    int nResult = false;

    KG_ASSERT_EXIT(pvRetDateBuf);
    KG_PROCESS_ERROR(uSize <= m_uUnreadSize);

    memcpy(pvRetDateBuf, m_pbyNextReadPos, uSize);
    m_pbyNextReadPos += uSize;
    m_uUnreadSize    -= (unsigned)uSize;

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DBufferReader::ReferenceRead(void **ppvRetDatePos, size_t uSize)
{
    int nResult = false;

    KG_ASSERT_EXIT(ppvRetDatePos);
    KG_PROCESS_ERROR(uSize <= m_uUnreadSize);

    *ppvRetDatePos = m_pbyNextReadPos;
    m_pbyNextReadPos += uSize;
    m_uUnreadSize    -= (unsigned)uSize;

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG3DBufferReader::Seek(DWORD dwOffset, int nSeekType)
{
    int nResult = false;

    switch (nSeekType)
    {
    case SEEK_SET:
        KG_PROCESS_ERROR(dwOffset <= m_uBufferSize);
        m_pbyNextReadPos = m_pbyBuffer   + dwOffset;
        m_uUnreadSize    = m_uBufferSize - dwOffset;
        break;

    case SEEK_CUR:
        KG_PROCESS_ERROR(dwOffset <= m_uUnreadSize);
        m_pbyNextReadPos += dwOffset;
        m_uUnreadSize    -= dwOffset;
        break;

    default:
        ASSERT(false);
    }

    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT ChangePathExtName(
                          const char cszSourcePathName[], 
                          const char cszNewExtName[],
                          unsigned uDestPathNameBufferCharCount, 
                          char *pszRetDestPathName
                          )
{
    HRESULT hrResult = E_FAIL;
    const char *pszDot = NULL;
    size_t uNoExtNameLen = 0;
    size_t uNewExtNameLen = 0;

    KG_ASSERT_EXIT(cszSourcePathName);
    KG_ASSERT_EXIT(cszNewExtName);
    KG_ASSERT_EXIT(pszRetDestPathName);

    pszDot = strrchr(cszSourcePathName, '.');
    if (pszDot)
        uNoExtNameLen = pszDot - cszSourcePathName;
    else
        uNoExtNameLen = strlen(cszSourcePathName);

    uNewExtNameLen = strlen(cszNewExtName);

    KGLOG_PROCESS_ERROR(uDestPathNameBufferCharCount > (uNoExtNameLen +  sizeof('.') + uNewExtNameLen));
    memcpy(pszRetDestPathName, cszSourcePathName, uNoExtNameLen);
    pszRetDestPathName[uNoExtNameLen] = '.';
    memcpy(pszRetDestPathName + uNoExtNameLen + 1, cszNewExtName, uNoExtNameLen + sizeof('\0'));

    hrResult = S_OK;
Exit0:
    return hrResult;
}
