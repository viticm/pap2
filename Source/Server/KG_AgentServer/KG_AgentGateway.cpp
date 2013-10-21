////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_AgentGateway.cpp
//  Version     : 1.0
//  Creator     : tongxuehu, zoukewei
//  Create Date : 01/11/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_AgentUtil.h"
#include "KG_AgentGateway.h"

////////////////////////////////////////////////////////////////////////////////
//                            IKG_AgentServer
////////////////////////////////////////////////////////////////////////////////

KG_AgentGateway::KG_AgentGateway(
    int          nMinSequenceID,
    int          nMaxSequenceID,
    PLUG_IN_FUNC OnReceiveFromServerPlugInFunc,
    PLUG_IN_FUNC OnReceiveFromClientPlugInFunc
) : m_nMinSequenceID(-1),
    m_nMaxSequenceID(-1),
    m_OnReceiveFromServerPlugInFunc(NULL),
    m_OnReceiveFromClientPlugInFunc(NULL)
{
    m_szOS_Name[0]        = '\0';
    m_szOS_Version[0]     = '\0';

    m_piAgentServer        = NULL;

    m_piClientSocketStream      = NULL;    
    m_nActiveFlag         = true;
    m_nExitFlag           = false;
}

KG_AgentGateway::~KG_AgentGateway()
{
}

int KG_AgentGateway::Init(
    KG_AGENT_PROTOCOL           *pAgentProtocol,
    const IKG_AgentServer       *pciAgentServer,
    IKG_SocketStream            *piSocketStream,
    const KG_AGENT_GATEWAY_PARAM &crAgentGatewayParam,
    void                        *pvContext
)
{
    int nResult  = false;
    int nRetCode = false;

    timeval        TimeVal;
    struct in_addr RemoteAddr            = { 0 };
    unsigned short usRemotePort          = 0;
    int            nWorkThreadCreateFlag = false;
    char          *pszBuffer             = NULL;
    
    KGLOG_PROCESS_ERROR(pAgentProtocol);
    KGLOG_PROCESS_ERROR(pciAgentServer);
    KGLOG_PROCESS_ERROR(piSocketStream);
    KG_USE_ARGUMENT(pvContext);

    pszBuffer = pAgentProtocol->CommandParameterMap[(char *)KG_AGENT_STRING_PARAMETER_CLIENT_NAME];
    KGLOG_PROCESS_ERROR(pszBuffer);  
    strncpy(m_szAgentClientName, pszBuffer, sizeof(m_szAgentClientName));
    m_szAgentClientName[sizeof(m_szAgentClientName) - 1] = '\0';

    pszBuffer = pAgentProtocol->CommandParameterMap[(char *)KG_AGENT_STRING_PARAMETER_OS_NAME];
    KGLOG_PROCESS_ERROR(pszBuffer);
    strncpy(m_szOS_Name, pszBuffer, sizeof(m_szOS_Name));
    m_szOS_Name[sizeof(m_szOS_Name) - 1] = '\0';

    pszBuffer = pAgentProtocol->CommandParameterMap[(char *)KG_AGENT_STRING_PARAMETER_OS_VERSION];
    KGLOG_PROCESS_ERROR(pszBuffer);
    strncpy(m_szOS_Version, pszBuffer, sizeof(m_szOS_Version));
    m_szOS_Version[sizeof(m_szOS_Version) - 1] = '\0';

    piSocketStream->AddRef();
    m_piClientSocketStream = piSocketStream;
    m_AgentGatewayParam = crAgentGatewayParam;

    TimeVal.tv_sec  = (long)(crAgentGatewayParam.nSendRecvTimeout / 1000);
    TimeVal.tv_usec = (long)((crAgentGatewayParam.nSendRecvTimeout % 1000) * 1000);
    nRetCode = m_piClientSocketStream->SetTimeout(&TimeVal);
    KGLOG_PROCESS_ERROR(nRetCode);

    m_LastReceiveTime = time(NULL);

    SetExitFlag(false);
    nRetCode = m_WorkThread.Create(_WorkThreadFunction, this);
    KGLOG_PROCESS_ERROR(nRetCode);
    nWorkThreadCreateFlag = true;
    
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
        KG_COM_RELEASE(m_piClientSocketStream);
    }
    
    return nResult;
}

int KG_AgentGateway::UnInit(void *pvContext)
{
    int nResult  = false;
    int nRetCode = false;

    SetExitFlag(true);
    m_WorkThread.Destroy();
 
    KG_COM_RELEASE(m_piClientSocketStream);

//Exit1:
    nResult = true;
//Exit0:
    return nResult;
}


void KG_AgentGateway::_WorkThreadFunction(void *pvParam)
{
    KG_AgentGateway *pThis = (KG_AgentGateway *)pvParam;

    ASSERT(pThis);

    pThis->_ThreadFunction();
}

int KG_AgentGateway::_ThreadFunction()
{
    int nResult  = false;
    int nRetCode = false;
    HRESULT hrRetCode = E_FAIL;
    struct in_addr RemoteAddr   = { 0 };
    unsigned short usRemotePort = 0;
    time_t CurrentTime  = 0;
    int nTimeDifference = 0;

    ASSERT(m_piClientSocketStream);

    nRetCode = m_piClientSocketStream->GetRemoteAddress(&RemoteAddr, &usRemotePort);
    ASSERT(nRetCode);

    KGLogPrintf(
        KGLOG_INFO, 
        "[KG_AgentGateway::_ThreadFunction] Started, service for %s:%d",
        inet_ntoa(RemoteAddr),
        ntohs(usRemotePort)
    );

    m_LastReceiveTime = time(NULL);

    m_nActiveFlag = true;
    while (!GetExitFlag())
    {
        nRetCode = ReceiveMessages();
        KGLOG_PROCESS_ERROR(nRetCode && "ReceiveMessages() failed, KG_AgentGateway will stop");

        if (m_nExitFlag)
            break;

        CurrentTime = time(NULL);
        nTimeDifference = (int)(CurrentTime - m_LastReceiveTime);
        if ((nTimeDifference * 1000) >= m_AgentGatewayParam.nAutoDisconnectTime)
        {
            KGLogPrintf(
                KGLOG_ERR, 
                "[KG_AgentGateway %s] There are no data arrive during %d second, auto disconnect!", 
                m_szAgentClientName,
                nTimeDifference
            );
            KG_PROCESS_ERROR(false);
        }

        KGThread_Sleep(m_AgentGatewayParam.nLoopSleepTime);
    }

    nResult = true;
Exit0:
    m_nActiveFlag = false;

    KG_COM_RELEASE(m_piClientSocketStream);

    return nResult;
}

int KG_AgentGateway::ReceiveMessages()
{
    int nResult  = false;
    int nRetCode = false;

    IKG_Buffer        *piBuffer = NULL;
    KG_AGENT_PROTOCOL  AgentProtocol;
    
    while (!GetExitFlag())
    {
        KG_COM_RELEASE(piBuffer);
        nRetCode = m_piClientSocketStream->Recv(&piBuffer);
        KG_PROCESS_SUCCESS(nRetCode == 0);
        KGLOG_PROCESS_ERROR(nRetCode > 0);

        m_LastReceiveTime = time(NULL); 

        ASSERT(piBuffer);
        nRetCode = ProcessMessages(piBuffer);
        KGLOG_PROCESS_ERROR(nRetCode);

        KG_COM_RELEASE(piBuffer);
    }

Exit1:
    nResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

int KG_AgentGateway::ProcessMessages(IKG_Buffer *piBuffer)
{
	int nResult  = false;
	int nRetCode = false;

    char         *pszBuffer     = NULL;
    unsigned int  uBufferSize   = 0;
    
    KG_AGENT_PROTOCOL AgentProtocol;

    KGLOG_PROCESS_ERROR(piBuffer);
    
    uBufferSize = piBuffer->GetSize();
    KGLOG_PROCESS_ERROR(uBufferSize > 0);

    pszBuffer = (char *)piBuffer->GetData();
    KGLOG_PROCESS_ERROR(pszBuffer);

    pszBuffer[uBufferSize - 1] = '\0';

    nRetCode = ParseProtocol(pszBuffer, &AgentProtocol);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = OnReceiveFromClient(&AgentProtocol);
    KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KG_AgentGateway::OnReceiveFromClient(KG_AGENT_PROTOCOL *pAgentProtocol)
{
	int nResult  = false;
	int nRetCode = false;

    int nNeedSendFlag = true;

    KGLOG_PROCESS_ERROR(pAgentProtocol);
    KGLOG_PROCESS_ERROR(pAgentProtocol->pszCommand);

    nRetCode = strcmp(
        pAgentProtocol->pszCommand, 
        KG_AGENT_STRING_COMMAND_LOGOUT
    );
    if (nRetCode == 0)
    {
        KGLogPrintf(KGLOG_INFO, "[KG_AgentGateway %s] Logout", m_szAgentClientName);
        SetExitFlag(true);
        KG_PROCESS_SUCCESS(true);
    }

    nRetCode = strcmp(
        pAgentProtocol->pszCommand, 
        KG_AGENT_STRING_COMMAND_PING
    );
    if (nRetCode == 0)
    {
        nRetCode = _ProcessPingMessage(pAgentProtocol);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLogPrintf(KGLOG_INFO, "[KG_AgentServer %s %d] Ping", m_szAgentClientName, pAgentProtocol->nSequenceID);
        KG_PROCESS_SUCCESS(true);
    }

    if (m_OnReceiveFromClientPlugInFunc)
    {
        nRetCode = m_OnReceiveFromClientPlugInFunc(
            pAgentProtocol, 
            &nNeedSendFlag,
            this
        );
        KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(!nNeedSendFlag);
    }

    nRetCode = SendToServer(pAgentProtocol);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}


int KG_AgentGateway::OnReceiveFromServer(KG_AGENT_PROTOCOL *pAgentProtocol)
{
	int nResult  = false;
	int nRetCode = false;

    int nNeedSendFlag = true;
    KGLOG_PROCESS_ERROR(pAgentProtocol);

    if (m_OnReceiveFromServerPlugInFunc)
    {
        nRetCode = m_OnReceiveFromServerPlugInFunc(
            pAgentProtocol,
            &nNeedSendFlag,
            this
        );
        KGLOG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_SUCCESS(!nNeedSendFlag);
    }

    nRetCode = SendToClient(pAgentProtocol);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KG_AgentGateway::_ProcessPingMessage(KG_AGENT_PROTOCOL *pAgentProtocol)
{
    int nResult  = false;
    int nRetCode = false;

    KG_AGENT_PROTOCOL PingResultProtocol;

    KGLOG_PROCESS_ERROR(pAgentProtocol);
    KGLOG_PROCESS_ERROR(pAgentProtocol->nSequenceID != KG_AGENT_SEQUENCE_ID_INVALID);
    nRetCode = InitProtocolStruct(&PingResultProtocol);
    ASSERT(nRetCode);
    
    PingResultProtocol.pszCommand    = (char *)KG_AGENT_STRING_COMMAND_PING_RESULT;
    PingResultProtocol.nSequenceID   = pAgentProtocol->nSequenceID;
    PingResultProtocol.nProtocolType = KG_AGENT_PROTOCOL_TYPE_RESPONSE;

    nRetCode = SendToClient(&PingResultProtocol);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KG_AgentGateway::SendToServer(KG_AGENT_PROTOCOL *pAgentProtocol)
{
	int nResult  = false;
	int nRetCode = false;
    
    char *pszBuffer = NULL;

    KGLOG_PROCESS_ERROR(pAgentProtocol);

    switch(pAgentProtocol->nProtocolType)
    {
    case KG_AGENT_PROTOCOL_TYPE_REQUEST:
        pszBuffer = pAgentProtocol->CommandParameterMap[(char *)KG_AGENT_STRING_PARAMETER_DSET_AGENT_CLIENT_NAME];
        KGLOG_PROCESS_ERROR(pszBuffer);
        
        nRetCode = _SendRequestToServer(pszBuffer, pAgentProtocol);
        KGLOG_PROCESS_ERROR(nRetCode);
    	break;
    case KG_AGENT_PROTOCOL_TYPE_RESPONSE:
        nRetCode = _SendResponseToServer(pAgentProtocol);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;
    default:
        KGLOG_PROCESS_ERROR(false);
        break;
    }
// Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KG_AgentGateway::_SendRequestToServer( 
    const char  cszAgentServerName[],  
    KG_AGENT_PROTOCOL *pAgentProtocol 
)
{
    int nResult  = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(cszAgentServerName);
    KGLOG_PROCESS_ERROR(pAgentProtocol);

    ASSERT(m_piAgentServer);
    
    nRetCode = m_piAgentServer->SendRequest(
        cszAgentServerName,
        pAgentProtocol
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KG_AgentGateway::_SendResponseToServer(KG_AGENT_PROTOCOL *pAgentProtocol)
{
    int nResult  = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(pAgentProtocol);

    KGLOG_PROCESS_ERROR(pAgentProtocol->nSequenceID != KG_AGENT_SEQUENCE_ID_INVALID);

    ASSERT(m_piAgentServer);
    nRetCode = m_piAgentServer->SendResponse(pAgentProtocol);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KG_AgentGateway::SendToClient(KG_AGENT_PROTOCOL *pAgentProtocol)
{
	int nResult  = false;
	int nRetCode = false;

    IKG_Buffer *piBuffer = NULL;
    KGLOG_PROCESS_ERROR(pAgentProtocol);

    nRetCode = BuildProtocol(&piBuffer, pAgentProtocol);
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLOG_PROCESS_ERROR(m_piClientSocketStream);

    nRetCode = m_piClientSocketStream->Send(piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode);
// Exit1:
	nResult = true;
Exit0:
    KG_COM_RELEASE(piBuffer);
	return nResult;
}

int KG_AgentGateway::GetNextSequenceID(int *pnSequenceID)
{
    int nResult  = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(pnSequenceID);

    m_nCurrentSequenceID++;

    if (
        (m_nCurrentSequenceID < m_nMinSequenceID) ||
        (m_nCurrentSequenceID > m_nMaxSequenceID)
    )
    {
        m_nCurrentSequenceID = m_nMinSequenceID;
    }

    *pnSequenceID = m_nCurrentSequenceID;

    nResult = true;
Exit0:
    return nResult;
}

