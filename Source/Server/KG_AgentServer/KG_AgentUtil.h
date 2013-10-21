////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_AgentUtil.h
//  Version     : 1.0
//  Creator     : tongxuehu, zoukewei
//  Create Date : 01/11/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_AGENT_COMMON_H_
#define _KG_AGENT_COMMON_H_ 1

#include "KG_AgentProtocol.h"

int ParseProtocol(
    char               szProtocol[], 
    KG_AGENT_PROTOCOL *pAgentProtocol
);

int BuildProtocol(
    IKG_Buffer        **ppiBuffer, 
    KG_AGENT_PROTOCOL  *pAgentProtocol
);

int InitProtocolStruct(KG_AGENT_PROTOCOL *AgentProtocol);

#endif // _KG_AGENT_COMMON_H_
