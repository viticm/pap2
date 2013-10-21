////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_AgentUtil.cpp
//  Version     : 1.0
//  Creator     : tongxuehu, zoukewei
//  Create Date : 01/11/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_AgentUtil.h"

int ParseProtocol(
    char               szProtocol[], 
    KG_AGENT_PROTOCOL *pAgentProtocol
)
{
    int nResult  = false;
    int nRetCode = false;

    char *pszParameterName  = NULL;
    char *pszParameterValue = NULL;
    
    KG_PROCESS_ERROR(szProtocol);
    KG_PROCESS_ERROR(pAgentProtocol);

    nRetCode = InitProtocolStruct(pAgentProtocol);
    ASSERT(nRetCode);

    // Get Protocol Context
    pAgentProtocol->pszContext = strchr(szProtocol, ':');
    if (pAgentProtocol->pszContext)
    {
        *pAgentProtocol->pszContext = '\0';
        ++pAgentProtocol->pszContext;
    }

    // Get Protocol Command
    pAgentProtocol->pszCommand = strtok(szProtocol, " ");
    KG_PROCESS_ERROR(pAgentProtocol->pszCommand);

    // Get Protocol SequenceID
    pszParameterValue = strtok(NULL, " ");
    KGLOG_PROCESS_ERROR(pszParameterValue);
    pAgentProtocol->nSequenceID = atoi(pszParameterValue);
    KGLOG_PROCESS_ERROR(pAgentProtocol->nSequenceID != KG_AGENT_SEQUENCE_ID_INVALID);

    // Get Protocol Type
    pszParameterValue = strtok(NULL, " ");
    KGLOG_PROCESS_ERROR(pszParameterValue);
    pAgentProtocol->nProtocolType = atoi(pszParameterValue);
    KGLOG_PROCESS_ERROR(pAgentProtocol->nProtocolType != KG_AGENT_PROTOCOL_TYPE_INVALID);

    // Get Protocol Parameter Loop
    while (true)
    {
        pszParameterName = strtok(NULL, "=");
        if (!pszParameterName)
            break;

        pszParameterValue = strtok(NULL, " ");
        KGLOG_PROCESS_ERROR(pszParameterValue);

        // Is Result?
        nRetCode = strcmp(pszParameterName, KG_AGENT_STRING_PARAMETER_RESULT);
        if (nRetCode == 0)
        {
            pAgentProtocol->nResult = atoi(pszParameterValue);
            KGLOG_PROCESS_ERROR(pAgentProtocol->nResult != KG_AGENT_ERRORNO_INVALID);
            continue;
        }

        pAgentProtocol->CommandParameterMap[pszParameterName] = pszParameterValue;
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        InitProtocolStruct(pAgentProtocol);
    }
    return nResult;
}

int BuildProtocol(
    IKG_Buffer        **ppiBuffer, 
    KG_AGENT_PROTOCOL  *pAgentProtocol
)
{
    int    nResult  = false;
    int    nRetCode = false;

    char        szProtocolBuffer[KG_AGENT_PROTOCOL_MAX_SIZE];
    IKG_Buffer *piBuffer              = NULL;
    int         nBufferOffset         = 0;

    char       *pszParameterName      = NULL;
    char       *pszParameterValue     = NULL;
    int         nParameterNameLength  = 0;
    int         nParameterValueLength = 0;

    COMMAND_PARAMETER_MAP::iterator itParameterMap;

    KGLOG_PROCESS_ERROR(ppiBuffer);
    KGLOG_PROCESS_ERROR(pAgentProtocol);
    KGLOG_PROCESS_ERROR(pAgentProtocol->pszCommand);
    // KGLOG_PROCESS_ERROR(pAgentProtocol->nSequenceID != KG_AGENT_SEQUENCE_ID_INVALID);
    KGLOG_PROCESS_ERROR(pAgentProtocol->nProtocolType != KG_AGENT_PROTOCOL_TYPE_INVALID);

    nRetCode = _snprintf(
        szProtocolBuffer + nBufferOffset, 
        sizeof(szProtocolBuffer) - nBufferOffset,
        "%s %d %d", 
        pAgentProtocol->pszCommand,
        pAgentProtocol->nSequenceID,
        pAgentProtocol->nProtocolType
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    szProtocolBuffer[sizeof(szProtocolBuffer) - 1] = '\0';
    nBufferOffset += nRetCode;

    if (pAgentProtocol->nResult)
    {
        nRetCode = _snprintf(
            szProtocolBuffer + nBufferOffset, 
            sizeof(szProtocolBuffer) - nBufferOffset,
            " %s=%d", 
            KG_AGENT_STRING_PARAMETER_RESULT,
            pAgentProtocol->nResult
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szProtocolBuffer[sizeof(szProtocolBuffer) - 1] = '\0';
        nBufferOffset += nRetCode;
    }

    for (
        itParameterMap = pAgentProtocol->CommandParameterMap.begin();
        itParameterMap != pAgentProtocol->CommandParameterMap.end();
        itParameterMap++
    )
    {
        pszParameterName  = (char *)(itParameterMap->first);
        ASSERT(pszParameterName);
        pszParameterValue = (char *)(itParameterMap->second);
        ASSERT(pszParameterValue);

        KGLOG_PROCESS_ERROR(nRetCode + nBufferOffset < sizeof(szProtocolBuffer));
        nRetCode = _snprintf(
            szProtocolBuffer + nBufferOffset, 
            sizeof(szProtocolBuffer) - nBufferOffset,
            " %s=%s", 
            pszParameterName,
            pszParameterValue
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szProtocolBuffer[sizeof(szProtocolBuffer) - 1] = '\0';
        nBufferOffset += nRetCode;
    }

    KG_PROCESS_SUCCESS(!pAgentProtocol->pszContext);

    nRetCode = (int)strlen(pAgentProtocol->pszContext);
    KGLOG_PROCESS_ERROR(nRetCode + nBufferOffset < sizeof(szProtocolBuffer));
    nRetCode = _snprintf(
        szProtocolBuffer + nBufferOffset, 
        sizeof(szProtocolBuffer) - nBufferOffset,
        " : %s", 
        pAgentProtocol->pszContext
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    szProtocolBuffer[sizeof(szProtocolBuffer) - 1] = '\0';
    nBufferOffset += nRetCode;

Exit1:
    piBuffer = KG_MemoryCreateBuffer((unsigned)nBufferOffset + 1);
    KGLOG_PROCESS_ERROR(piBuffer);

    char *pszBuffer = (char *)piBuffer->GetData();
    ASSERT(pszBuffer);
    memcpy(pszBuffer, szProtocolBuffer, nBufferOffset);
    pszBuffer[nBufferOffset] = '\0';

    *ppiBuffer = piBuffer;
    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_COM_RELEASE(piBuffer);
    }
    return nResult;
}

int InitProtocolStruct(KG_AGENT_PROTOCOL *AgentProtocol)
{
    int nResult = false;

    KG_PROCESS_ERROR(AgentProtocol);

    AgentProtocol->pszCommand    = NULL;
    AgentProtocol->nProtocolType = KG_AGENT_PROTOCOL_TYPE_INVALID;
    AgentProtocol->nSequenceID   = KG_AGENT_SEQUENCE_ID_INVALID;
    AgentProtocol->nResult       = 0;
    AgentProtocol->pszContext    = NULL;
    AgentProtocol->CommandParameterMap.clear();

    nResult = true;
Exit0:
    return nResult;
}
