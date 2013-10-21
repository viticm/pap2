////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : PlugIn_GM.h
//  Version     : 1.0
//  Creator     : tongxuehu, zoukewei
//  Create Date : 01/16/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#include "KG_AgentProtocol.h"
#include "IKG_AgentGateway.h"

#ifdef PLUGIN_GM_EXPORTS
#define PLUGIN_GM_API extern "C" __declspec(dllexport)
#else
#define PLUGIN_GM_API extern "C" __declspec(dllimport)
#endif

PLUGIN_GM_API int ReceiveFromServer(
    KG_AGENT_PROTOCOL *pAgentProtocol, 
    int               *pnSendMessageFlag,
    IKG_AgentGateway  *piAgentGateway
);

PLUGIN_GM_API int ReceiveFromClient(
    KG_AGENT_PROTOCOL *pAgentProtocol, 
    int               *pnSendMessageFlag,
    IKG_AgentGateway  *piAgentGateway
);