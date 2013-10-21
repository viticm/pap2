#ifndef _INCLUDE_KG3DFILELOADER_H_
#define _INCLUDE_KG3DFILELOADER_H_

////////////////////////////////////////////////////////////////////////////////
struct _TASK;
enum FILE_LOAD_STATE
{
    FILE_LOAD_STATE_WAIT,
    FILE_LOAD_STATE_SUCCESS,
    FILE_LOAD_STATE_FAILED
};
enum FILE_TYPE
{
    FILE_TYPE_NORMAL,
    FILE_TYPE_INI
};
class KG3DFileLoader
{
public:
    KG3DFileLoader(void);
    ~KG3DFileLoader(void);

    int Init();
    int UnInit();

    // ---------------------- immediate load ---------------------
    int LoadImmediately(const char cszFileName[], IKG_Buffer **ppiRetFileContent);

    // -------------------- multi thread load --------------------
    HANDLE CreateTask(const char cszFileName[], FILE_TYPE eType); // return NULL if failed
    int DeleteTask(HANDLE hHandle);
    int CheckResult(HANDLE hHandle, FILE_LOAD_STATE *peRetState);
    int FetchResultNormal(HANDLE hHandle, FILE_LOAD_STATE *peRetState, IKG_Buffer **ppiRetFileContent); 
    int FetchResultIni(HANDLE hHandle, FILE_LOAD_STATE *peRetState, IIniFile **ppiRetFile); 

private:
    static void _WorkThread(void *pvThis);
    int _ThreadFunction();
private:
    KThread m_WorkThread;
    int m_nExitFlag;

    std::queue<_TASK *> m_RequestQueue;
    KMutex              m_RequestQueueLock;

#ifdef _DEBUG
    std::set<_TASK *> m_TaskSet;
    KMutex            m_TaskSetLock;
#endif
};
////////////////////////////////////////////////////////////////////////////////
// Helper class for read data from buffer
////////////////////////////////////////////////////////////////////////////////
class KG3DBufferReader
{
public:
    KG3DBufferReader();
    int SetBuffer(IKG_Buffer *piBuffer);

    int Read(void *pvRetDateBuf, size_t uSize);
    int ReferenceRead(void **ppvRetDatePos, size_t uSize);
    int Seek(DWORD dwOffset, int nSeekType);  //SEEK_SET, SEEK_CUR  
private:
    unsigned char *m_pbyNextReadPos;
    unsigned m_uUnreadSize;

    unsigned char *m_pbyBuffer;
    unsigned m_uBufferSize;
};

////////////////////////////////////////////////////////////////////////////////
HRESULT ChangePathExtName(
                          const char cszSourcePathName[],
                          const char cszNewExtName[], // like "ini", not ".ini"
                          unsigned uDestPathNameBufferCharCount, 
                          char *pszRetDestPathName
                          );

////////////////////////////////////////////////////////////////////////////////
extern KG3DFileLoader g_FileLoader;

#endif //_INCLUDE_KG3DFILELOADER_H_
