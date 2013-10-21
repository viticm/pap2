////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IKG_AgentServer.cpp
//  Version     : 1.0
//  Creator     : tongxuehu, zoukewei
//  Create Date : 01/11/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG_AgentUtil.h"
#include "KG_AgentServer.h"
#include "KG_AgentServerExports.h"

const char KG_AGENT_SERVER_INI_FILENAME[]            = "KG_AgentServer.ini";
const char KG_AGENT_SERVER_OPTIONAL_INI_FILENAME[]   = "KG_AgentServer_Optional.ini";

KG_AgentServer::KG_AgentServer(void)
{
}

KG_AgentServer::~KG_AgentServer(void)
{
}

int KG_AgentServer::Init(void *pvContext)
{
    int nResult  = false;
    int nRetCode = false;

	IIniFile *piIniFile               = NULL;
	IIniFile *piOptionalIniFile       = NULL;
    int       nVersion                = 0;
    int       nIniValue               = 0;
    int       i                       = 0;
    int       nSequenceIDScope        = 0;
    int       nSocketAcceptorOpenFlag = false;
    int       nWorkThreadCreateFlag   = false;

    char      szAgentClientSessionName[KG_AGENT_STRING_MAX_SIZE];
    char      szAgentClientName[KG_AGENT_STRING_MAX_SIZE];
    char      szPluginDllName[KG_AGENT_STRING_MAX_SIZE];
    char      szPluginDllFuncName[KG_AGENT_STRING_MAX_SIZE];

    KG_AGENT_GATEWAY_INFO *pAgentGatewayInfo         = NULL;
    int                    nAgentClientInfoPushFlag  = false;
    
    KG_USE_ARGUMENT(pvContext);
    
    piIniFile = g_OpenIniFile(KG_AGENT_SERVER_INI_FILENAME);
    KGLOG_PROCESS_ERROR(piIniFile);

    memset(&m_AgentServerParam, 0, sizeof(m_AgentServerParam));
    m_AgentGatewayMap.clear();

    // read version config
    nRetCode = piIniFile->GetInteger(
        "Version", 
        "Version", 
        0, 
        &nVersion
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Unable read \"[Version]:Version\"");
    KGLOG_PROCESS_ERROR(nVersion == 1);
    
    // read network config
    nRetCode = piIniFile->GetString(
        "ServerListener", 
        "ListenIP", 
        "0.0.0.0", 
        m_AgentServerParam.szListenIP,
        sizeof(m_AgentServerParam.szListenIP)
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Unable read \"[ServerListener]:ListenIP\"");
    m_AgentServerParam.szListenIP[sizeof(m_AgentServerParam.szListenIP) - 1] = '\0';

    nRetCode = piIniFile->GetInteger(
        "ServerListener", 
        "ListenPort", 
        0, 
        &m_AgentServerParam.nListenPort
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Unable read \"[ServerListener]:ListenPort\"");

    nSequenceIDScope                                        = 10000;
    m_AgentServerParam.eEncodeDecodeMode                    = EASYCRYPT_ENCODE_DECODE;
    m_AgentServerParam.GatewayParam.nLoopSleepTime          = 10;
    m_AgentServerParam.GatewayParam.nAutoDisconnectTime     = 120000;
    m_AgentServerParam.GatewayParam.nSendRecvTimeout        = 60000;

    piOptionalIniFile = g_OpenIniFile(KG_AGENT_SERVER_OPTIONAL_INI_FILENAME);
    // It doesn't matter that no optional ini file, we set default values
    KGLOG_PROCESS_SUCCESS(piOptionalIniFile == false); 

    // read security config
    nRetCode = piOptionalIniFile->GetInteger(
        "Network", 
        "EncodeDecodeType", 
        (int)EASYCRYPT_ENCODE_DECODE, 
        &nIniValue
    );
    //this ini config is optional, so not need to exit when conn't read it
    //KGLOG_PROCESS_ERROR(nRetCode && "Unable read \"[Network]:EncodeDecodeType\"");
    switch (nIniValue)
    {
    case (int)KSG_ENCODE_DECODE:
    case (int)EASYCRYPT_ENCODE_DECODE:
    case (int)KSG_ENCODE_DECODE_SIMPLE:
        m_AgentServerParam.eEncodeDecodeMode = (ENCODE_DECODE_MODE)nIniValue;
        break;

    default:
        KGLogPrintf(
            KGLOG_ERR,
            "[KG_AgentServerManager]:EncoderDecoder Type = %d, Error!",
            nIniValue
        );
        KGLOG_PROCESS_ERROR(false);
        break;
    }

    nRetCode = piOptionalIniFile->GetInteger(
        "Network", 
        "LoopSleepTime", 
        10, 
        &m_AgentServerParam.GatewayParam.nLoopSleepTime
    );
    //KGLOG_PROCESS_ERROR(nRetCode && "Unable read \"[Network]:LoopSleepTime\"");

    nRetCode = piOptionalIniFile->GetInteger(
        "Network", 
        "AutoDisconnectTime", 
        120000, 
        &m_AgentServerParam.GatewayParam.nAutoDisconnectTime
    );
    //KGLOG_PROCESS_ERROR(nRetCode && "Unable read \"[Network]:AutoDisconnectTime\"");

    nRetCode = piOptionalIniFile->GetInteger(
        "Network", 
        "SendRecvTimeout", 
        60000, 
        &m_AgentServerParam.GatewayParam.nSendRecvTimeout
    );
    //KGLOG_PROCESS_ERROR(nRetCode && "Unable read \"[Network]:SendRecvTimeout\"");

    nRetCode = piOptionalIniFile->GetInteger(
        "Network", 
        "CheckIPMacFlag", 
        true, 
        &m_AgentServerParam.nCheckIPMacFlag
    );
    //KGLOG_PROCESS_ERROR(nRetCode && "Unable read \"[Network]:CheckIPMacFlag\"");

    nRetCode = piOptionalIniFile->GetInteger(
        "Network", 
        "SequenceIDScope", 
        10000, 
        &nSequenceIDScope
    );
    //KGLOG_PROCESS_ERROR(nRetCode && "Unable read \"[Network]:CheckIPMacFlag\"");

Exit1:
    for (i = 1; i < 100; i++)
    {
        nRetCode = _snprintf(
            szAgentClientSessionName, 
            sizeof(szAgentClientSessionName),
            "%s%d",
            "AgentClient",
            i
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szAgentClientSessionName[nRetCode] = '\0';
        
        nRetCode = piIniFile->GetString(
            szAgentClientSessionName,
            "Name",
            "",
            szAgentClientName,
            sizeof(szAgentClientName)
        );
        if (!nRetCode)
            continue;
        szAgentClientName[sizeof(szAgentClientName) - 1] = '\0';

        pAgentGatewayInfo = m_AgentGatewayMap[szAgentClientName];
        KGLOG_PROCESS_ERROR(!pAgentGatewayInfo && "AgentClientName cann't refrain!");

        pAgentGatewayInfo = new KG_AGENT_GATEWAY_INFO;
        KGLOG_PROCESS_ERROR(pAgentGatewayInfo);
        nAgentClientInfoPushFlag = false;

        pAgentGatewayInfo->pAgentGateway          = NULL;
        pAgentGatewayInfo->nMinSequenceID          = i * nSequenceIDScope;
        pAgentGatewayInfo->nMaxSequenceID          = (i + 1) * nSequenceIDScope - 1;
        pAgentGatewayInfo->hModule                 = NULL;
        pAgentGatewayInfo->OnReceiveFromServerFunc = NULL;
        pAgentGatewayInfo->OnReceiveFromClientFunc = NULL;
        m_AgentGatewayMap[szAgentClientName] = pAgentGatewayInfo;
        nAgentClientInfoPushFlag = true;

        nRetCode = piIniFile->GetString(
            szAgentClientSessionName,
            "PlugIn",
            "",
            szPluginDllName,
            sizeof(szPluginDllName)
        );
        if (nRetCode == 0)
            continue;
        szPluginDllName[sizeof(szPluginDllName) - 1] = '\0';

        // Load Plug-in Module
        pAgentGatewayInfo->hModule = ::LoadLibrary(szPluginDllName);
        KGLOG_PROCESS_ERROR(pAgentGatewayInfo->hModule && "Load plug-in dll failed!");

        nRetCode = piIniFile->GetString(
            szAgentClientSessionName,
            "OnReceiveFromServerFunc",
            "",
            szPluginDllFuncName,
            sizeof(szPluginDllFuncName)
        );
        szPluginDllFuncName[sizeof(szPluginDllFuncName) - 1] = '\0';
        if (nRetCode)
        {
            pAgentGatewayInfo->OnReceiveFromServerFunc = (PLUG_IN_FUNC)GetProcAddress(
                pAgentGatewayInfo->hModule, 
                szPluginDllFuncName
            );
	        KGLOG_PROCESS_ERROR(pAgentGatewayInfo->OnReceiveFromServerFunc);
        }

        nRetCode = piIniFile->GetString(
            szAgentClientSessionName,
            "OnReceiveFromClientFunc",
            "",
            szPluginDllFuncName,
            sizeof(szPluginDllFuncName)
        );
        szPluginDllFuncName[sizeof(szPluginDllFuncName) - 1] = '\0';
        if (nRetCode)
        {
            pAgentGatewayInfo->OnReceiveFromClientFunc = (PLUG_IN_FUNC)GetProcAddress(
                pAgentGatewayInfo->hModule, 
                szPluginDllFuncName
            );
	        KGLOG_PROCESS_ERROR(pAgentGatewayInfo->OnReceiveFromClientFunc);
        }
    }

    SetExitFlag(false);
    
    nRetCode = m_SocketAcceptor.Open(
        m_AgentServerParam.szListenIP, 
        m_AgentServerParam.nListenPort
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    nSocketAcceptorOpenFlag = true;

    nRetCode = m_WorkThread.Create(_WorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(nRetCode);
    nWorkThreadCreateFlag = true;

    KGLogPrintf(
        KGLOG_INFO,
        "[KG_AgentServer] Listening at %s:%d", 
        m_AgentServerParam.szListenIP, 
        m_AgentServerParam.nListenPort
    );

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (nWorkThreadCreateFlag)
        {
            SetExitFlag(true);
            m_WorkThread.Destroy();
            nWorkThreadCreateFlag = false;
        }

        if (nSocketAcceptorOpenFlag)
        {
            m_SocketAcceptor.Close();
            nSocketAcceptorOpenFlag = false;
        }

        if (!nAgentClientInfoPushFlag)
        {
            KG_DELETE(pAgentGatewayInfo);
            nAgentClientInfoPushFlag = true;
        }

        AGENT_GATEWAY_MAP::iterator itAgentGateway;
        for (
            itAgentGateway = m_AgentGatewayMap.begin();
            itAgentGateway != m_AgentGatewayMap.end();
            itAgentGateway++
        )
        {
            if (itAgentGateway->second->hModule)
                FreeLibrary(itAgentGateway->second->hModule);

            KG_DELETE(itAgentGateway->second);
        }
        m_AgentGatewayMap.clear();

        {
            static const char s_cszMsg[] = "[KG_AgentServer] Start failed";
            puts(s_cszMsg);
            KGLogPrintf(KGLOG_INFO, "%s", s_cszMsg);
        }
    }
    KG_COM_RELEASE(piOptionalIniFile);
    KG_COM_RELEASE(piIniFile);
    return nResult;
}

int KG_AgentServer::UnInit(void *pvContext)
{
	int nResult  = false;
	int nRetCode = false;

    KG_USE_ARGUMENT(pvContext);

    SetExitFlag(true);
    m_WorkThread.Destroy();

    m_SocketAcceptor.Close();

    AGENT_GATEWAY_MAP::iterator itAgentGateway;
    for (
        itAgentGateway = m_AgentGatewayMap.begin();
        itAgentGateway != m_AgentGatewayMap.end();
        itAgentGateway++
    )
    {
        if (itAgentGateway->second->hModule)
            FreeLibrary(itAgentGateway->second->hModule);

        KG_DELETE(itAgentGateway->second);
    }
    m_AgentGatewayMap.clear();
// Exit1:
	nResult = true;
// Exit0:
	return nResult;
}

void KG_AgentServer::_WorkThreadFunction(void *pvParam)
{
    KG_AgentServer *pThis = (KG_AgentServer *)pvParam;

    ASSERT(pThis);

    pThis->_ThreadFunction();
}

int KG_AgentServer::_ThreadFunction()
{
    int nResult  = false;
    int nRetCode = false;

    timeval           TimeVal      = {1, 0};
    struct in_addr    RemoteAddr   = { 0 };
    unsigned short    usRemotePort = 0;
    IKG_SocketStream *piSockStream = NULL;

    m_SocketAcceptor.SetTimeout(&TimeVal);

    m_nActiveFlag = true;

    while (!GetExitFlag())
    {
        piSockStream = m_SocketAcceptor.AcceptSecurity(m_AgentServerParam.eEncodeDecodeMode);
        if (!piSockStream)
            continue;  // timeout
        
        nRetCode = piSockStream->GetRemoteAddress(&RemoteAddr, &usRemotePort);
        if (!nRetCode)
        {
            KG_COM_RELEASE(piSockStream);
            continue;
        }

        KGLogPrintf(
            KGLOG_INFO, 
            "[AgentServerManager] Accept from (%s: %d) SocketStream = %p", 
            inet_ntoa(RemoteAddr),
            ntohs(usRemotePort),
            piSockStream
        );

        nRetCode = OnAccpetAgentClient(piSockStream);
        // KGLOG_PROCESS_ERROR(nRetCode);

        KG_COM_RELEASE(piSockStream);
    }

    nResult = true;
//Exit0:
    m_nActiveFlag = false;
    DestoryAllAgentGateway();
    return nResult;
}

int KG_AgentServer::OnAccpetAgentClient(IKG_SocketStream *piSocketStream)
{
    int nResult  = false;
    int nRetCode = false;

    IKG_Buffer *piBuffer      = NULL;
    char       *pszBuffer     = NULL;
    unsigned    uBufferSize   = 0;
    int         nReceiveCount = 0;
    int         nLoginResult  = false;

    KG_AGENT_PROTOCOL AgentProtocol;

    while (true)
    {
        nRetCode = piSocketStream->Recv(&piBuffer);
        if (nRetCode > 0)
            break;        
        KGLOG_PROCESS_ERROR(nRetCode == 0);
        
        nReceiveCount++;
        KGLOG_PROCESS_ERROR(nReceiveCount < KG_AGENT_MAX_RECEIVE_LOOP_COUNT);
        KGThread_Sleep(m_AgentServerParam.GatewayParam.nLoopSleepTime);
    }
        
    ASSERT(piBuffer);

    uBufferSize = piBuffer->GetSize();
    KGLOG_PROCESS_ERROR(uBufferSize > 0);

    pszBuffer = (char *)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pszBuffer);

    pszBuffer[uBufferSize - 1] = '\0';

    nRetCode = ParseProtocol(pszBuffer, &AgentProtocol);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = OnAgentClientLogin(&AgentProtocol, piSocketStream, &nLoginResult);
    KGLOG_PROCESS_ERROR(nRetCode);
    // Login in Failed, go to Exit1
    KG_PROCESS_SUCCESS(nLoginResult != KG_AGENT_ERRORNO_SUCCESS);

    nRetCode = CreateAgentGateway(&AgentProtocol, piSocketStream);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

int KG_AgentServer::OnAgentClientLogin(
    KG_AGENT_PROTOCOL *pRequestProtocol,
    IKG_SocketStream  *piSocketStream,
    int               *pnRetLoginResult
)
{
	int nResult  = false;
	int nRetCode = false;

    char                  *pszClientName      = NULL;
    char                  *pszAgentServerType = NULL;
    char                  *pszBuffer          = NULL;
    IKG_Buffer            *piResponseBuffer   = NULL;
    KG_AGENT_GATEWAY_INFO *pAgentClientInfo   = NULL;
    int                    nLoginErrorNo      = KG_AGENT_ERRORNO_FAIL;

    KG_AGENT_PROTOCOL     ResponseProtocol;
    char                  szMaxSequenceIDBuffer[KG_AGENT_STRING_MAX_SIZE];
    char                  szMinSequenceIDBuffer[KG_AGENT_STRING_MAX_SIZE];
    
    KGLOG_PROCESS_ERROR(pRequestProtocol);
    KGLOG_PROCESS_ERROR(pRequestProtocol->pszCommand);
    KGLOG_PROCESS_ERROR(piSocketStream);
    KGLOG_PROCESS_ERROR(pnRetLoginResult);

    nRetCode = InitProtocolStruct(&ResponseProtocol);
    ASSERT(nRetCode);
    ResponseProtocol.pszCommand    = (char *)KG_AGENT_STRING_COMMAND_LOGIN_RESULT;
    ResponseProtocol.nSequenceID   = pRequestProtocol->nSequenceID;
    ResponseProtocol.nProtocolType = KG_AGENT_PROTOCOL_TYPE_RESPONSE;

    nRetCode = strcmp(pRequestProtocol->pszCommand, KG_AGENT_STRING_COMMAND_LOGIN);
    KG_PROCESS_SUCCESS(nRetCode != 0);

    nRetCode = (int)pRequestProtocol->CommandParameterMap.size();
    KG_PROCESS_SUCCESS(nRetCode == 0);
    // Check login name
    pszClientName = pRequestProtocol->CommandParameterMap[(char *)KG_AGENT_STRING_PARAMETER_CLIENT_NAME];
    KGLOG_PROCESS_ERROR(pszClientName);

    pAgentClientInfo = m_AgentGatewayMap[pszClientName];
    if (pAgentClientInfo == NULL)
    {
        ResponseProtocol.nResult = KG_AGENT_ERRORNO_CLIENT_NAME_NOT_EXIST;
        KG_PROCESS_SUCCESS(true);
    }
    if (pAgentClientInfo->pAgentGateway != NULL)
    {
        nRetCode = pAgentClientInfo->pAgentGateway->IsActive();
        if (nRetCode)
        {
            ResponseProtocol.nResult = KG_AGENT_ERRORNO_CLIENT_ALREADY_LOGIN;
            KG_PROCESS_SUCCESS(true);
        }
        nRetCode = pAgentClientInfo->pAgentGateway->UnInit(NULL);
        KGLOG_PROCESS_ERROR(nRetCode);
        KG_DELETE(pAgentClientInfo->pAgentGateway);
    }

    ResponseProtocol.nResult = KG_AGENT_ERRORNO_SUCCESS;

    pszBuffer = itoa(
        pAgentClientInfo->nMinSequenceID, 
        szMinSequenceIDBuffer, 
        10
    );
    KGLOG_PROCESS_ERROR(pszBuffer);
    szMinSequenceIDBuffer[sizeof(szMinSequenceIDBuffer) - 1] = '\0';

    pszBuffer = itoa(
        pAgentClientInfo->nMaxSequenceID / 2, 
        szMaxSequenceIDBuffer, 
        10
    );
    KGLOG_PROCESS_ERROR(pszBuffer);
    szMaxSequenceIDBuffer[sizeof(szMaxSequenceIDBuffer) - 1] = '\0';

    ResponseProtocol.CommandParameterMap[(char *)KG_AGENT_STRING_PARAMETER_MAX_SEQUENCE_ID] = szMinSequenceIDBuffer;
    ResponseProtocol.CommandParameterMap[(char *)KG_AGENT_STRING_PARAMETER_MIN_SEQUENCE_ID] = szMaxSequenceIDBuffer;
    
Exit1:
    nRetCode = BuildProtocol(&piResponseBuffer, &ResponseProtocol);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = piSocketStream->Send(piResponseBuffer);
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    *pnRetLoginResult = ResponseProtocol.nResult;

   nResult = true;
Exit0:
    KGLogPrintf(KGLOG_INFO, "[KG_AgentServer %s] Login Reuslt=%d", pszClientName, ResponseProtocol.nResult);
    KG_COM_RELEASE(piResponseBuffer);
    return nResult;
}

int KG_AgentServer::CreateAgentGateway(
    KG_AGENT_PROTOCOL *pAgentProtocol,
    IKG_SocketStream  *piSocketStream
)
{
	int nResult  = false;
	int nRetCode = false;

    KG_AGENT_GATEWAY_INFO *pAgentClientInfo = NULL;
    KG_AgentGateway       *pAgentGateway    = NULL;
    char                  *pszClientName    = NULL;
    int                    nGatewayInitFlag = false;
    KGLOG_PROCESS_ERROR(pAgentProtocol);
    KGLOG_PROCESS_ERROR(piSocketStream);

    pszClientName = pAgentProtocol->CommandParameterMap[(char *)KG_AGENT_STRING_PARAMETER_CLIENT_NAME];
    KGLOG_PROCESS_ERROR(pszClientName);
    pAgentClientInfo = m_AgentGatewayMap[pszClientName];
    KGLOG_PROCESS_ERROR(pAgentClientInfo);

    KGLOG_PROCESS_ERROR(!pAgentClientInfo->pAgentGateway);
    pAgentGateway = new KG_AgentGateway(
        pAgentClientInfo->nMinSequenceID / 2 + 1, 
        pAgentClientInfo->nMaxSequenceID,
        pAgentClientInfo->OnReceiveFromServerFunc,
        pAgentClientInfo->OnReceiveFromClientFunc
    );
    KGLOG_PROCESS_ERROR(pAgentGateway);

    nRetCode = pAgentGateway->Init(
        pAgentProtocol, 
        this, 
        piSocketStream, 
        m_AgentServerParam.GatewayParam, 
        NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode);
    nGatewayInitFlag = true;

    pAgentClientInfo->pAgentGateway = pAgentGateway;
// Exit1:
	nResult = true;
Exit0:
    if (!nResult)
    {
        if (nGatewayInitFlag)
        {
            pAgentGateway->UnInit(NULL);
            nGatewayInitFlag = false;
        }
        KG_DELETE(pAgentGateway);
    }
	return nResult;
}

int KG_AgentServer::DestoryAllAgentGateway()
{
    int nResult  = false;
    int nRetCode = false;

    KG_AGENT_GATEWAY_INFO *pAgentGatewayInfo = NULL;
    KG_AgentGateway       *pAgentGateway     = NULL;
    AGENT_GATEWAY_MAP::iterator it;
    for (
        it = m_AgentGatewayMap.begin();
        it != m_AgentGatewayMap.end();
        ++it
    )
    {
        pAgentGatewayInfo = it->second;
        ASSERT(pAgentGatewayInfo);

        pAgentGateway = pAgentGatewayInfo->pAgentGateway;
        if (!pAgentGateway)
            continue;

        pAgentGateway->UnInit(NULL);
        KG_DELETE(pAgentGateway);
    }
  
    nResult = true;
//Exit0:
    return nResult;
}

int KG_AgentServer::SendRequest(
    const char cszAgentServerName[], 
    KG_AGENT_PROTOCOL *pAgentProtocol
)
{
	int nResult  = false;
	int nRetCode = false;

    KG_AGENT_GATEWAY_INFO *pAgentClientInfo = NULL;
    
    KGLOG_PROCESS_ERROR(cszAgentServerName);
    KGLOG_PROCESS_ERROR(pAgentProtocol);

    pAgentClientInfo = m_AgentGatewayMap[cszAgentServerName];
    KGLOG_PROCESS_ERROR(pAgentClientInfo);
    KGLOG_PROCESS_ERROR(pAgentClientInfo->pAgentGateway);

    nRetCode = pAgentClientInfo->pAgentGateway->OnReceiveFromServer(pAgentProtocol);
    KGLOG_PROCESS_ERROR(nRetCode);
// Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KG_AgentServer::SendResponse(
    KG_AGENT_PROTOCOL *pAgentProtocol
)
{
	int nResult  = false;
	int nRetCode = false;

    AGENT_GATEWAY_MAP::iterator itAgentClientMap;
    
    KGLOG_PROCESS_ERROR(pAgentProtocol);
    KGLOG_PROCESS_ERROR(pAgentProtocol->nSequenceID != KG_AGENT_SEQUENCE_ID_INVALID);

    for (
        itAgentClientMap = m_AgentGatewayMap.begin();
        itAgentClientMap != m_AgentGatewayMap.end();
        ++itAgentClientMap
    )
    {
        KG_AGENT_GATEWAY_INFO *pAgentClientInfo = itAgentClientMap->second;
        if (pAgentProtocol->nSequenceID < pAgentClientInfo->nMinSequenceID)
            continue;

        if (pAgentProtocol->nSequenceID > pAgentClientInfo->nMaxSequenceID)
            continue;

        nRetCode = pAgentClientInfo->pAgentGateway->OnReceiveFromServer(pAgentProtocol);
        KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(true);
    }

    KGLOG_PROCESS_ERROR(false);
Exit1:
	nResult = true;
Exit0:
	return nResult;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( 
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

int CreateAgentServerInstance(
    void **ppvAgentServerHandle
)
{
	int nResult  = false;
	int nRetCode = false;

    KG_AgentServer *pAgentServer         = NULL;
    int             nAgentServerInitFlag = false;

    KGLOG_PROCESS_ERROR(ppvAgentServerHandle);

    pAgentServer = new KG_AgentServer();
    KGLOG_PROCESS_ERROR(pAgentServer);

    nRetCode = pAgentServer->Init(NULL);
    KGLOG_PROCESS_ERROR(nRetCode);
    nAgentServerInitFlag = true;

    *ppvAgentServerHandle = pAgentServer;

// Exit1:
	nResult = true;
Exit0:
    if (!nResult)
    {
        if (nAgentServerInitFlag)
        {
            pAgentServer->UnInit(NULL);
            nAgentServerInitFlag = false;
        }
        KG_DELETE(pAgentServer);
    }
	return nResult;
}


int DestroyAgentServerInstance(
    void **ppvAgentServerHandle
)
{
	int nResult  = false;
	int nRetCode = false;

    KG_AgentServer *pAgentServer = NULL;

    KGLOG_PROCESS_ERROR(ppvAgentServerHandle);
    KGLOG_PROCESS_ERROR(*ppvAgentServerHandle);
            
    pAgentServer = (KG_AgentServer *)*ppvAgentServerHandle;
    pAgentServer->UnInit(NULL);
    
    KG_DELETE(pAgentServer);
    *ppvAgentServerHandle = NULL;
// Exit1:
	nResult = true;
Exit0:
	return nResult;
}
