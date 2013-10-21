////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KRecorderFactory.h
//  Version     : 1.0
//  Creator     : ZhaoChunfeng,XiaYong
//  Create Date : 2008-7-25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KRECORDERFACTORY_H_ 
#define _KRECORDERFACTORY_H_

#include "Common/KG_Socket.h"

struct IRecorderTools : IUnknown
{
    virtual DWORD   GetTickCount() = 0;
    virtual time_t  GetTime() = 0;
    virtual BOOL    CreateGUID(GUID* pGuid) = 0;
};

struct IRecorderSceneLoader: IUnknown
{
    virtual BOOL    PostLoadingScene(DWORD dwSceneID) = 0;
    virtual DWORD   GetCompletedScene() = 0;
};

struct ISocketClient : IUnknown
{
    virtual IKG_SocketStream* Connect(const char cszIP[], int nPort) = 0;
};

struct ISocketServer : IUnknown 
{
    virtual int Init(
        const char cszIPAddress[], int nPort,
        int nMaxAcceptEachWait,
        int nMaxRecvBufSizePerSocket,
        int nMaxSendBufSizePerSocket,
        ENCODE_DECODE_MODE EncodeDecodeMode, 
        void* pvContext
    ) = 0;

    virtual void UnInit(void* pvContext) = 0;

    virtual int Wait(int nEventCount, KG_SOCKET_EVENT* pEvent, int* pnRetEventCount) = 0;
};


enum KRecoderMode
{
    ermNormal,
    ermRecord,
    ermReplay
};

struct IRecorderFactory : IUnknown
{
    virtual IRecorderTools*         CreateToolsInterface() = 0;
    virtual IRecorderSceneLoader*   CreateSceneLoader() = 0;
    virtual ISocketClient*          CreateSocketClient() = 0;
    virtual ISocketServer*          CreateSocketServer() = 0;
};

IRecorderFactory* CreateRecorderFactoryInterface(KRecoderMode eMode);
#endif
