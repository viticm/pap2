////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : _KPROTOCOLRECORDERBASE_H_ 
//  Version     : 1.0
//  Creator     : ZhaoChunfeng, XiaYong
//  Create Date : 2008-7-28
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KPROTOCOLRECORDERBASE_H_ 
#define _KPROTOCOLRECORDERBASE_H_

enum RecordType
{
    ertInvalid = 0,
    
    ertGetTime,
    ertGetTickCount,
    ertGUID,
    ertGetLoadedScene,
    
    ertSocketConnector,
    ertSocketSetTimeout,
    ertSocketSend,
    ertSocketCheckCanRecv,
    ertSocketRecv,
    ertSocketIsAlive,
    
    ertServerInit,
    ertServerWait,
    ertServerSend,
    ertServerRecv,
    ertServerGetAddress
};
// 目前RecordType的值限制在[0,15]的范围
#pragma	pack(1)
struct KGET_TIME
{
    uint8_t     uType;
    time_t      nTime;
};

struct KGET_TICKCOUNT
{
    uint8_t     uType;
    DWORD       dwTime;
};

struct KGET_GUID
{
    uint8_t     uType;
    GUID        guid;
};

struct KGET_LOADED_SCENE_BASE
{
    uint8_t     uType : 4;
    uint8_t     uSceneLoadedFlag : 4;
};

struct KGET_LOADED_SCENE : KGET_LOADED_SCENE_BASE
{
    DWORD       dwSceneID;
};

struct KSOCKET_CONNECTOR
{
    uint8_t     uType;
    void*       piSocket;
};

struct KSOCKET_SET_TIMEOUT 
{
    uint8_t     uType  : 4;
    int8_t      nOperationResult : 4;
};

struct KSOCKET_SEND 
{
    uint8_t     uType  : 4;
    int8_t      nOperationResult : 4;
};

struct KSOCKET_CHECK_CAN_RECV 
{
    uint8_t     uType  : 4;
    int8_t      nOperationResult : 4;
};

struct KSOCKET_RECV
{
    uint8_t     uType  : 4;
    int8_t      nOperationResult : 4;
    WORD        wRecvDataLen;
};

struct KSOCKET_IS_ALIVE 
{
    uint8_t     uType  : 4;
    int8_t      nOperationResult : 4;
};

struct KSERVER_INIT 
{
    uint8_t     uType  : 4;
    int8_t      nOperationResult : 4;
};

struct KSERVER_WAIT 
{
    uint8_t      uType  : 4;
    int8_t       nOperationResult : 4;
    WORD         wRetEventCount;
};

struct KSERVER_SEND
{
    uint8_t     uType  : 4;
    int8_t      nOperationResult : 4;
    WORD        wSendDataLen; // just for replay debug
};

struct KSERVER_RECV
{
    uint8_t  uType  : 4;
    int8_t   nOperationResult : 4;
    WORD     wRecvDataLen;
};

struct KSERVER_GET_ADDRESS
{
    uint8_t  uType  : 4;
    int8_t   nOperationResult : 4;
    in_addr  addr;
    u_short  uPort;
};
#pragma pack()

#endif


