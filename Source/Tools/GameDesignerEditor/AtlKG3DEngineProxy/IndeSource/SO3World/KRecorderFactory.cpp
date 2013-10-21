#include "stdafx.h"
#include "KRecorderFactory.h"
#include "KProtocolRecorder.h"
#include "KRecorderToolsNormal.h"
#include "KRecorderToolsRecord.h"
#include "KRecorderToolsReplay.h"
#include "KRecorderSceneLoaderNormal.h"
#include "KRecorderSceneLoaderRecord.h"
#include "KRecorderSceneLoaderReplay.h"
#include "KRecorderSocketClientNormal.h"
#include "KRecorderSocketClientRecord.h"
#include "KRecorderSocketClientReplay.h"
#include "KRecorderSocketServerNormal.h"
#include "KRecorderSocketServerRecord.h"
#include "KRecorderSocketServerReplay.h"
#include "GS_Client_Protocol.h"

#define RECORDER_CURRENT_VERSION 1

#pragma pack(1)
struct KRECORD_FILE_HEADER
{
    BYTE byRecordVersion;
    int  nGameWorldCurrentVersion;
    int  nGameWorldLowestVersion;
    char szCompileTime[_NAME_LEN];
};
#pragma pack()

//////////////////////////////////////////////////////////////////////////
class KRecorderFactoryNormal : public IRecorderFactory
{
public:
    KRecorderFactoryNormal() : m_ulRefCount(1){}
    ~KRecorderFactoryNormal(){}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    IRecorderTools*         CreateToolsInterface();
    IRecorderSceneLoader*   CreateSceneLoader();
    ISocketClient*          CreateSocketClient();
    ISocketServer*          CreateSocketServer();

private:
    ULONG m_ulRefCount;
};

HRESULT STDMETHODCALLTYPE KRecorderFactoryNormal::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderFactoryNormal::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderFactoryNormal::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        delete this;
    }
    return ulCount;
}

IRecorderTools* KRecorderFactoryNormal::CreateToolsInterface()
{
    IRecorderTools* pTools = new KRecorderToolsNormal();
    return pTools;
}

IRecorderSceneLoader* KRecorderFactoryNormal::CreateSceneLoader()
{
    IRecorderSceneLoader*       piRetSceneLoader    = NULL;
    KRecorderSceneLoaderNormal* pSceneLoaderNormal  = NULL;
    BOOL                        bRetCode            = false;
    
    pSceneLoaderNormal  = new KRecorderSceneLoaderNormal();
    KGLOG_PROCESS_ERROR(pSceneLoaderNormal);

    bRetCode = pSceneLoaderNormal->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    piRetSceneLoader = pSceneLoaderNormal;
Exit0:
    return piRetSceneLoader;
}

ISocketClient* KRecorderFactoryNormal::CreateSocketClient()
{
    ISocketClient* piSocketClient = new KRecorderSocketClientNormal();
    return piSocketClient;
}

ISocketServer* KRecorderFactoryNormal::CreateSocketServer()
{
    ISocketServer* piSocketServer = new KRecorderSocketServerNormal();
    return piSocketServer;
}

//////////////////////////////////////////////////////////////////////////
class KRecorderFactoryRecord : public IRecorderFactory
{
public:
    KRecorderFactoryRecord(IProtocolRecorder* piRecorder) : m_ulRefCount(1)
    {
        assert(piRecorder);
        m_piRecorder = piRecorder;
        m_piRecorder->AddRef();
    }
    ~KRecorderFactoryRecord()
    {
        KG_COM_RELEASE(m_piRecorder);
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    IRecorderTools*         CreateToolsInterface();
    IRecorderSceneLoader*   CreateSceneLoader();
    ISocketClient*          CreateSocketClient();
    ISocketServer*          CreateSocketServer();

private:
    ULONG               m_ulRefCount;
    IProtocolRecorder*  m_piRecorder;
};

HRESULT STDMETHODCALLTYPE KRecorderFactoryRecord::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderFactoryRecord::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderFactoryRecord::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        KG_COM_RELEASE(m_piRecorder);
        delete this;
    }
    return ulCount;
}

IRecorderTools* KRecorderFactoryRecord::CreateToolsInterface()
{
    IRecorderTools* pTools = new KRecorderToolsRecord(m_piRecorder);
    return pTools;
}

IRecorderSceneLoader* KRecorderFactoryRecord::CreateSceneLoader()
{
    IRecorderSceneLoader*       piRetSceneLoader    = NULL;
    KRecorderSceneLoaderRecord* pSceneLoaderRecord  = NULL;
    BOOL                        bRetCode            = false;
    
    pSceneLoaderRecord  = new KRecorderSceneLoaderRecord(m_piRecorder);
    KGLOG_PROCESS_ERROR(pSceneLoaderRecord);

    bRetCode = pSceneLoaderRecord->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    piRetSceneLoader = pSceneLoaderRecord;
Exit0:
    return piRetSceneLoader;
}

ISocketClient* KRecorderFactoryRecord::CreateSocketClient()
{
    ISocketClient* piSocketClient = new KRecorderSocketClientRecord(m_piRecorder);
    return piSocketClient;
}

ISocketServer* KRecorderFactoryRecord::CreateSocketServer()
{
    ISocketServer* piSocketServer = new KRecorderSocketServerRecord(m_piRecorder);
    return piSocketServer;
}

//////////////////////////////////////////////////////////////////////////
class KRecorderFactoryReplay : public IRecorderFactory
{
public:
    KRecorderFactoryReplay(IProtocolReplayer* piReplayer) : m_ulRefCount(1)
    {
        assert(piReplayer);
        m_piReplayer = piReplayer;
        m_piReplayer->AddRef();
    }
    ~KRecorderFactoryReplay()
    {
        KG_COM_RELEASE(m_piReplayer);
    }

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

    IRecorderTools*         CreateToolsInterface();
    IRecorderSceneLoader*   CreateSceneLoader();
    ISocketClient*          CreateSocketClient();
    ISocketServer*          CreateSocketServer();

private:
    ULONG               m_ulRefCount;
    IProtocolReplayer*  m_piReplayer;
};

HRESULT STDMETHODCALLTYPE KRecorderFactoryReplay::QueryInterface(REFIID riid, void** ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

ULONG STDMETHODCALLTYPE KRecorderFactoryReplay::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long*)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KRecorderFactoryReplay::Release(void)
{
    ULONG ulCount = KG_InterlockedDecrement((long*)&m_ulRefCount);
    if (ulCount == 0)
    {
        KG_COM_RELEASE(m_piReplayer);
        delete this;
    }
    return ulCount;
}

IRecorderTools* KRecorderFactoryReplay::CreateToolsInterface()
{
    IRecorderTools* pTools = new KRecorderToolsReplay(m_piReplayer);
    return pTools;
}

IRecorderSceneLoader* KRecorderFactoryReplay::CreateSceneLoader()
{
    IRecorderSceneLoader*   piRetSceneLoader = new KRecorderSceneLoaderReplay(m_piReplayer);
    return piRetSceneLoader;
}

ISocketClient* KRecorderFactoryReplay::CreateSocketClient()
{
    ISocketClient* piSocketClient = new KRecorderSocketClientReplay(m_piReplayer);
    return piSocketClient;
}

ISocketServer* KRecorderFactoryReplay::CreateSocketServer()
{
    ISocketServer* piSocketServer = new KRecorderSocketServerReplay(m_piReplayer);
    return piSocketServer;
}

//////////////////////////////////////////////////////////////////////////
IRecorderFactory* CreateRecorderFactoryInterface(KRecoderMode eMode)
{
    IRecorderFactory*   piResult    = NULL;
    int                 nRetCode    = false;
    KProtocolRecorder*  pRecorder   = NULL;
    KProtocolReplayer*  pReplayer   = NULL;
    char                szDirName[64];

    if (eMode != ermRecord && eMode != ermReplay)
    {
        eMode = ermNormal;
    }

    switch (eMode)
    {
    case ermNormal:
        piResult = new KRecorderFactoryNormal();
        break;

    case ermRecord:
        {
            time_t              nTimeNow    = time(NULL);
            struct tm           tmNow; 
            KRECORD_FILE_HEADER FileHeader;

            localtime_r(&nTimeNow, &tmNow);

            nRetCode = (int)snprintf(
                szDirName, sizeof(szDirName),
                "recorder/gs%d%2.2d%2.2d_%2.2d_%2.2d_%2.2d",
                tmNow.tm_year + 1900,
                tmNow.tm_mon + 1,
                tmNow.tm_mday,
                tmNow.tm_hour,
                tmNow.tm_min,
                tmNow.tm_sec
            );
            KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szDirName));

            nRetCode = KG_mkdir("recorder");
            KGLOG_PROCESS_ERROR((nRetCode != -1) || (errno == EEXIST));

            nRetCode = KG_mkdir(szDirName);
            KGLOG_PROCESS_ERROR(nRetCode != -1);

            pRecorder = new KProtocolRecorder(szDirName);
            KGLOG_PROCESS_ERROR(pRecorder);
            
            FileHeader.byRecordVersion          = RECORDER_CURRENT_VERSION;
            FileHeader.nGameWorldCurrentVersion = GAME_WORLD_CURRENT_VERSION;
            FileHeader.nGameWorldLowestVersion  = GAME_WORLD_LOWEST_VERSION;
            
            strncpy(&FileHeader.szCompileTime[0], __TIMESTAMP__, sizeof(FileHeader.szCompileTime));
            FileHeader.szCompileTime[sizeof(FileHeader.szCompileTime) - 1] = '\0';

            nRetCode = pRecorder->Push(&FileHeader, sizeof(FileHeader));
            KGLOG_PROCESS_ERROR(nRetCode);

            piResult = new KRecorderFactoryRecord(pRecorder);
            KGLOG_PROCESS_ERROR(piResult);
            break;
        }

    case ermReplay:
        {
            KRECORD_FILE_HEADER FileHeader;

            strcpy(szDirName, "gs");

            pReplayer = new KProtocolReplayer(szDirName);
            KGLOG_PROCESS_ERROR(pReplayer);
            
            nRetCode = pReplayer->Pop(&FileHeader, sizeof(FileHeader));
            KGLOG_PROCESS_ERROR(nRetCode);
            
            KGLOG_PROCESS_ERROR(FileHeader.byRecordVersion == RECORDER_CURRENT_VERSION);
            KGLOG_PROCESS_ERROR(FileHeader.nGameWorldCurrentVersion == GAME_WORLD_CURRENT_VERSION);
            KGLOG_PROCESS_ERROR(FileHeader.nGameWorldLowestVersion == GAME_WORLD_LOWEST_VERSION);
            
            FileHeader.szCompileTime[sizeof(FileHeader.szCompileTime) - 1] = '\0';
            printf("************** The record game server compiled at date %s ***********\n", FileHeader.szCompileTime);

            piResult = new KRecorderFactoryReplay(pReplayer);
            KGLOG_PROCESS_ERROR(piResult);
            break;
        }

    default:
        assert(false);
    }

Exit0:
    KG_COM_RELEASE(pRecorder);
    KG_COM_RELEASE(pReplayer);
    return piResult;
}
