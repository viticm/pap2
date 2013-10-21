////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IKG_AgentServer.h
//  Version     : 1.0
//  Creator     : tongxuehu, zoukewei
//  Create Date : 01/11/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_AGENT_SERVER_H_
#define _KG_AGENT_SERVER_H_ 1

#include "KG_AgentProtocol.h"
#include "IKG_AgentServer.h"
#include "KG_AgentGateway.h"

typedef struct _KG_AGENT_SERVER_PARAM
{
    char szListenIP[KG_AGENT_STRING_MAX_SIZE];
    int  nListenPort;

    int  nCheckIPMacFlag;
   
    ENCODE_DECODE_MODE     eEncodeDecodeMode;
    KG_AGENT_GATEWAY_PARAM GatewayParam;
} KG_AGENT_SERVER_PARAM;

typedef struct _KG_AGENT_GATEWAY_INFO
{
    KG_AgentGateway *pAgentGateway;
    int              nMaxSequenceID;
    int              nMinSequenceID;

    HMODULE      hModule;
    PLUG_IN_FUNC OnReceiveFromServerFunc;
    PLUG_IN_FUNC OnReceiveFromClientFunc;
} KG_AGENT_GATEWAY_INFO;

typedef struct
{
    bool operator()(const std::string &X, const std::string &Y) const
	{
	    #ifdef WIN32
		return (_mbsicmp((const unsigned char *)X.c_str(), (const unsigned char *)Y.c_str()) < 0);
		#else   // linux
		return (strcasecmp(X.c_str(), Y.c_str()) < 0);
		#endif
	}
} STRICMP_LESS;

typedef std::map<std::string, KG_AGENT_GATEWAY_INFO *, STRICMP_LESS> AGENT_GATEWAY_MAP;

class KG_AgentServer : public IKG_AgentServer
{
public:
    KG_AgentServer(void);
    virtual ~KG_AgentServer(void);

public:
    virtual int Init(void *pvContext);
    virtual int UnInit(void *pvContext);

public:
    virtual int SendRequest(  
        const char  cszAgentServerName[],  
        KG_AGENT_PROTOCOL *pAgentProtocol 
    );

    virtual int SendResponse(
        KG_AGENT_PROTOCOL *pAgentProtocol
    );

private:
    KG_AGENT_SERVER_PARAM m_AgentServerParam;
    AGENT_GATEWAY_MAP     m_AgentGatewayMap;

private:
    int CreateAgentGateway(
        KG_AGENT_PROTOCOL *pAgentProtocol,
        IKG_SocketStream  *piSocketStream
    );

    int DestoryAllAgentGateway();

    int OnAgentClientLogin(
        KG_AGENT_PROTOCOL *pRequestProtocol,
        IKG_SocketStream  *piSocketStream,
        int               *pRetLoginResult
    );

    int OnAccpetAgentClient(IKG_SocketStream *piSocketStream);

private:
    KThread     m_WorkThread;
    int         _ThreadFunction(void);
    static void _WorkThreadFunction(void *pvParam);

private:
    int IsActive()             { return m_nActiveFlag;             }
    int GetExitFlag()          { return m_nExitFlag;               }
    int SetExitFlag(int nFlag) { m_nExitFlag = nFlag; return true; }

private:
    int m_nActiveFlag;
    int m_nExitFlag;

private:
    KG_SocketAcceptor m_SocketAcceptor;
};

#endif // _KG_AGENT_SERVER_H_


