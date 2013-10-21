////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_PlugIn_GM.cpp
//  Version     : 1.0
//  Creator     : tongxuehu, zoukewei
//  Create Date : 01/17/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_PlugIn_GM.h"

static const int gs_cnMaxSequenceIDBufferSize = 13;

static PROGRAM_CONTROL_ITEM_LIST gs_GroupItemList; 
static KG_AGENT_GROUP_STAT       gs_GroupState;
static CRITICAL_SECTION          gs_ItemListCriticalSection;

static int                       gs_nMaxStartPriority     = 0;
static int                       gs_nMaxStopPriority      = 0;

KG_PlugIn_GM::KG_PlugIn_GM()
{

}

KG_PlugIn_GM::~KG_PlugIn_GM()
{

}

int KG_PlugIn_GM::Init(void *pvContext)
{
    int nResult  = false;
    int nRetCode = false;

	IIniFile             *piIniFile                = NULL;
    char                 *pszIniValueSplit         = NULL;
    int                   nIKG_AgentServerInitFlag = false;
    int                   i                        = 0;
    PROGRAM_CONTROL_ITEM *pIniItem                 = NULL;
    const int KG_AGENT_SERVER_GM_MAX_PRIORITY      = 100;

    char szIniKey[KG_AGENT_STRING_MAX_SIZE];
    char szIniValue[300];
    int  nStartPriorityFlag[KG_AGENT_SERVER_GM_MAX_PRIORITY];

    KG_USE_ARGUMENT(pvContext);
    InitializeCriticalSection(&gs_ItemListCriticalSection);

    EnterCriticalSection(&gs_ItemListCriticalSection);
    memset(nStartPriorityFlag, sizeof(nStartPriorityFlag), 0);

    piIniFile = g_OpenIniFile(KG_AGENT_SERVER_GM_INI_FILENAME);
    KGLOG_PROCESS_ERROR(piIniFile && "Cann't load ini file!");

    gs_nMaxStartPriority = 0;
    gs_GroupItemList.clear();
    for (i = 0; i < KG_AGENT_SERVER_GM_MAX_ITEM_COUNT; ++i)
    {
        nRetCode = snprintf(
            szIniKey,
            sizeof(szIniKey),
            "Item%d",
            i
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szIniKey[sizeof(szIniKey) - 1] = '\0';

        nRetCode = piIniFile->GetString(
            KG_AGENT_SERVER_GM_STRING_START_GROUP, 
            szIniKey, 
            "", 
            szIniValue,
            sizeof(szIniValue)
        );
        if (!nRetCode)
            continue;
        szIniValue[sizeof(szIniValue) - 1] = '\0';

        pIniItem = new PROGRAM_CONTROL_ITEM;
        KGLOG_PROCESS_ERROR(pIniItem);
        gs_GroupItemList.push_back(pIniItem);

        pIniItem->eProgramState = KG_AGENT_PROGRAM_STATE_NOT_START;
        pIniItem->nSequenceID   = KG_AGENT_SEQUENCE_ID_INVALID;

        strncpy(
            pIniItem->szKey,
            szIniKey,
            sizeof(pIniItem->szKey)
        );
        pIniItem->szKey[sizeof(pIniItem->szKey) - 1] = '\0';

        pszIniValueSplit = strtok(szIniValue, ",");
        KG_PROCESS_ERROR(pszIniValueSplit);
        nRetCode = (int)strlen(pszIniValueSplit);
        KGLOG_PROCESS_ERROR(nRetCode < KG_AGENT_STRING_MAX_SIZE);
        KGLOG_PROCESS_ERROR(nRetCode > 0);

// 未完成
//         ASSERT(m_pAgentServerManager);
//         nRetCode = m_pAgentServerManager->CheckAgentClientName(pszIniValueSplit);
//         KGLOG_PROCESS_ERROR(nRetCode && "Cann't find this AgentName in AgentServer table!");

        strncpy(
            pIniItem->szAgentName,
            pszIniValueSplit,
            sizeof(pIniItem->szAgentName)
        );
        pIniItem->szAgentName[sizeof(pIniItem->szAgentName) - 1] = '\0';

        pszIniValueSplit = strtok(NULL, ",");
        KG_PROCESS_ERROR(pszIniValueSplit);
        pIniItem->nStartPriority = atoi(pszIniValueSplit);
        KGLOG_PROCESS_ERROR(pIniItem->nStartPriority > 0);
        KGLOG_PROCESS_ERROR(pIniItem->nStartPriority < KG_AGENT_SERVER_GM_MAX_PRIORITY);
        nStartPriorityFlag[pIniItem->nStartPriority] = true;
        if (pIniItem->nStartPriority > gs_nMaxStartPriority)
            gs_nMaxStartPriority = pIniItem->nStartPriority;

        pszIniValueSplit = strtok(NULL, ",");
        KG_PROCESS_ERROR(pszIniValueSplit);
        pIniItem->nIsStartWaitResult = true;
        nRetCode = strcmp(pszIniValueSplit, KG_AGENT_SERVER_GM_STRING_NOT_WAIT_RESULT);
        if (nRetCode == 0)
            pIniItem->nIsStartWaitResult = false;
        else
        {
            nRetCode = strcmp(pszIniValueSplit, KG_AGENT_SERVER_GM_STRING_WAIT_RESULT);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }

        nRetCode = (int)strlen(pszIniValueSplit);
        pszIniValueSplit += nRetCode + 1;
        nRetCode = (int)strlen(pszIniValueSplit);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        strncpy(
            pIniItem->szStartProgram,
            pszIniValueSplit,
            sizeof(pIniItem->szStartProgram)
        );
        pIniItem->szStartProgram[sizeof(pIniItem->szStartProgram) - 1] = '\0';                        
    }

    // 加载关闭命令行，待整理
    // check the priority is continuous, from 0 to gs_nMaxStartPriority
    for (i = 0; i <= gs_nMaxStartPriority; ++i)
    {
        KGLOG_PROCESS_ERROR(nStartPriorityFlag[i]);
    }

    gs_nMaxStopPriority = -1;
    for (i = 0; i < KG_AGENT_SERVER_GM_MAX_ITEM_COUNT; ++i)
    {
        nRetCode = snprintf(
            szIniKey,
            sizeof(szIniKey),
            "Stop%d",
            i
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        szIniKey[sizeof(szIniKey) - 1] = '\0';

        nRetCode = piIniFile->GetString(
            KG_AGENT_SERVER_GM_STRING_STOP_GROUP, 
            szIniKey, 
            "", 
            szIniValue,
            sizeof(szIniValue)
        );
        if (!nRetCode)
            continue;
        szIniValue[sizeof(szIniValue) - 1] = '\0';
        ++gs_nMaxStopPriority;

        pszIniValueSplit = strtok(szIniValue, ",");
        KG_PROCESS_ERROR(pszIniValueSplit);

        int nIsStopWaitResult = true;
        nRetCode = strcmp(pszIniValueSplit, KG_AGENT_SERVER_GM_STRING_NOT_WAIT_RESULT);
        if (nRetCode == 0)
            nIsStopWaitResult = false;
        else
        {
            nRetCode = strcmp(pszIniValueSplit, KG_AGENT_SERVER_GM_STRING_WAIT_RESULT);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
        }

        pszIniValueSplit = strtok(NULL, ",");
        KG_PROCESS_ERROR(pszIniValueSplit);
        while (pszIniValueSplit)
        {
            PROGRAM_CONTROL_ITEM *pProgramControlItem = NULL;
            nRetCode = _GetProgramItemByKey(
                &gs_GroupItemList,
                pszIniValueSplit,
                &pProgramControlItem
            );
            KGLOG_PROCESS_ERROR(nRetCode);
            ASSERT(pProgramControlItem);

            pProgramControlItem->nStopPriority = gs_nMaxStopPriority;
            pProgramControlItem->nIsStopWaitResult = nIsStopWaitResult;
     
            pszIniValueSplit = strtok(NULL, ",");
        }
    }
    if (gs_nMaxStopPriority == -1)
        gs_nMaxStopPriority = 0;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        PROGRAM_CONTROL_ITEM_LIST::iterator it;
        for (
            it = gs_GroupItemList.begin();
            it != gs_GroupItemList.end();
            ++it
        )
        {
            PROGRAM_CONTROL_ITEM *pItem = *it;
            ASSERT(pItem);
            KG_DELETE(pItem);
        }
        gs_GroupItemList.clear();
    }
    KG_COM_RELEASE(piIniFile);
    LeaveCriticalSection(&gs_ItemListCriticalSection);
    return nResult;
}

int KG_PlugIn_GM::UnInit(void *pvContext)
{
	int nResult  = false;
	int nRetCode = false;

    PROGRAM_CONTROL_ITEM_LIST::iterator it;

    KG_USE_ARGUMENT(pvContext);

    for (
        it = gs_GroupItemList.begin();
        it != gs_GroupItemList.end();
        ++it
    )
    {
        KG_DELETE(*it);
    }
    gs_GroupItemList.clear();

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}


int KG_PlugIn_GM::_GetProgramItemByKey(
    PROGRAM_CONTROL_ITEM_LIST  *pProgramItemList,
    const char                  cszKey[],
    PROGRAM_CONTROL_ITEM      **ppProgramItem
)
{
    int nResult  = false;
    int nRetCode = false;

    PROGRAM_CONTROL_ITEM *pProgramItem = NULL;
    PROGRAM_CONTROL_ITEM_LIST::iterator itItemList;

    KGLOG_PROCESS_ERROR(pProgramItemList);
    KGLOG_PROCESS_ERROR(cszKey);
    KGLOG_PROCESS_ERROR(ppProgramItem);

    for (
        itItemList = pProgramItemList->begin();
        itItemList != pProgramItemList->end();
        ++itItemList
    )
    {
        pProgramItem = *itItemList;
        ASSERT(pProgramItem);

        nRetCode = strcmp(
            cszKey,
            pProgramItem->szKey
        );
        if (nRetCode != 0)
            continue;

        KG_PROCESS_SUCCESS(true);
    }
    KG_PROCESS_ERROR(false);
    
Exit1:
    nResult = true;
Exit0:
    *ppProgramItem = pProgramItem;
    return nResult;
}

int KG_PlugIn_GM::ReceiveFromServer(
    KG_AGENT_PROTOCOL *pAgentProtocol, 
    int               *pnSendMessageFlag,
    IKG_AgentGateway  *piAgentGateway
)
{
	int nResult  = false;
	int nRetCode = false;

    KGLOG_PROCESS_ERROR(pAgentProtocol);
    KGLOG_PROCESS_ERROR(pnSendMessageFlag);
    KGLOG_PROCESS_ERROR(piAgentGateway);

    m_piAgentGateway = piAgentGateway;
// Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KG_PlugIn_GM::ReceiveFromClient(
    KG_AGENT_PROTOCOL *pAgentProtocol, 
    int               *pnSendMessageFlag,
    IKG_AgentGateway  *piAgentGateway
)
{
	int nResult  = false;
	int nRetCode = false;

    KGLOG_PROCESS_ERROR(pAgentProtocol);
    KGLOG_PROCESS_ERROR(pnSendMessageFlag);
    KGLOG_PROCESS_ERROR(piAgentGateway);
    
    m_piAgentGateway = piAgentGateway;
    nRetCode = strcmp(
        pAgentProtocol->pszCommand, 
        KG_AGENT_STRING_COMMAND_START_SERVER_GROUP
    );
    if (nRetCode == 0)
    {
        nRetCode = OnCommand_StartServerGroup(
            pAgentProtocol
        );
        KGLOG_PROCESS_ERROR(nRetCode);
        *pnSendMessageFlag = false;
        KG_PROCESS_SUCCESS(true);
    }

    nRetCode = strcmp(
        pAgentProtocol->pszCommand, 
        KG_AGENT_STRING_COMMAND_STOP_SERVER_GROUP
    );
    if (nRetCode == 0)
    {
        nRetCode = OnCommand_StopServerGroup(
            pAgentProtocol
        );
        KGLOG_PROCESS_ERROR(nRetCode);
        *pnSendMessageFlag = false;
        KG_PROCESS_SUCCESS(true);
    }

    *pnSendMessageFlag = true;
Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KG_PlugIn_GM::OnCommand_StartServerGroup(
    KG_AGENT_PROTOCOL *pAgentProtocol
)
{
    int nResult  = false;
    int nRetCode = false;

    KGLOG_PROCESS_ERROR(pAgentProtocol);

    KGLOG_PROCESS_ERROR(gs_GroupState == KG_AGENT_GROUP_STATE_NOT_START);
    gs_GroupState = KG_AGENT_GROUP_STATE_STARTTING;

    nRetCode = _StartProgramsByPriority(0);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KG_PlugIn_GM::_StartProgramsByPriority(int nPriority)
{
    int nResult  = false;
    int nRetCode = false;

    int                   nSequenceID  = 0;
    PROGRAM_CONTROL_ITEM *pProgramItem = NULL;
    PROGRAM_CONTROL_ITEM_LIST::iterator itItemList;
    KG_AGENT_PROTOCOL AgentProtocol;

    KGLOG_PROCESS_ERROR(nPriority > 0);
    
    for (
        itItemList = gs_GroupItemList.begin();
        itItemList != gs_GroupItemList.end();
        ++itItemList
    )
    {
        pProgramItem = *itItemList;
        ASSERT(pProgramItem);

        if (nPriority != pProgramItem->nStartPriority)
            continue;

        ASSERT(m_piAgentGateway);
        nRetCode = m_piAgentGateway->GetNextSequenceID(&nSequenceID);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(nSequenceID != KG_AGENT_SEQUENCE_ID_INVALID);

        AgentProtocol.pszCommand    = (char *)KG_AGENT_STRING_COMMAND_EXECUTE_COMMAND_LINE;
        AgentProtocol.nSequenceID   = nSequenceID;
        AgentProtocol.nProtocolType = KG_AGENT_PROTOCOL_TYPE_REQUEST;
        AgentProtocol.CommandParameterMap[(char *)KG_AGENT_STRING_PARAMETER_DSET_AGENT_CLIENT_NAME] = pProgramItem->szAgentName;
/*执行命令的参数，待整理
        AgentProtocol.ProtocolParameter.push_back(
            (char *)KG_AGENT_STRING_PARAMETER_NEW_THREAD
        );
        AgentProtocol.ProtocolParameter.push_back(
            (char *)KG_AGENT_STRING_PARAMETER_WAIT_EXECUTE_FINISH
        );
        AgentProtocol.ProtocolParameter.push_back(
            (char *)KG_AGENT_STRING_PARAMETER_WAIT_PING_RESULT
        );
*/
        AgentProtocol.pszContext = pProgramItem->szStartProgram;
        
        ASSERT(m_piAgentGateway);
        nRetCode = m_piAgentGateway->SendToServer(&AgentProtocol);
        KGLOG_PROCESS_ERROR(nRetCode);

        pProgramItem->eProgramState = KG_AGENT_PROGRAM_STATE_STARTTING;
        pProgramItem->nSequenceID   = nSequenceID;
        pProgramItem->nProcessID    = -1;
    }

    nResult = true;
Exit0:
    return nResult;
}


int KG_PlugIn_GM::OnCommand_StopServerGroup(
    KG_AGENT_PROTOCOL *pAgentProtocol
)
{
    int nResult  = false;
    int nRetCode = false;

    int nPriority = 0;
    PROGRAM_CONTROL_ITEM *pProgramItem = NULL;
    PROGRAM_CONTROL_ITEM_LIST::iterator itItemList;

    KGLOG_PROCESS_ERROR(pAgentProtocol);

    gs_GroupState = KG_AGENT_GROUP_STATE_STOPING;

    for (nPriority = 0; nPriority <= gs_nMaxStopPriority; ++nPriority)
    {
        for (
            itItemList = gs_GroupItemList.begin();
            itItemList != gs_GroupItemList.end();
            ++itItemList
        )
        {
            pProgramItem = *itItemList;
            ASSERT(pProgramItem);

            if (nPriority != pProgramItem->nStopPriority)
                continue;

            if (pProgramItem->eProgramState == KG_AGENT_PROGRAM_STATE_NOT_START)
                continue;

            if (pProgramItem->eProgramState == KG_AGENT_PROGRAM_STATE_STOPING)
                continue;

            nRetCode = _StopProgramsByPriority(nPriority);
            KGLOG_PROCESS_ERROR(nRetCode);
            KG_PROCESS_SUCCESS(true);
        }
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KG_PlugIn_GM::_StopProgramsByPriority(int nPriority)
{
    int nResult  = false;
    int nRetCode = false;

    int                   nSequenceID  = 0;
    PROGRAM_CONTROL_ITEM *pProgramItem = NULL;
    PROGRAM_CONTROL_ITEM_LIST::iterator itItemList;
    KG_AGENT_PROTOCOL AgentProtocol;

    KGLOG_PROCESS_ERROR(nPriority > 0);
    KGLOG_PROCESS_ERROR(nPriority <= gs_nMaxStopPriority);

    for (
        itItemList = gs_GroupItemList.begin();
        itItemList != gs_GroupItemList.end();
        ++itItemList
    )
    {
        pProgramItem = *itItemList;
        ASSERT(pProgramItem);

        if (nPriority != pProgramItem->nStopPriority)
            continue;

        ASSERT(m_piAgentGateway);
        nRetCode = m_piAgentGateway->GetNextSequenceID(&nSequenceID);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(nSequenceID != KG_AGENT_SEQUENCE_ID_INVALID);

        AgentProtocol.pszCommand    = (char *)KG_AGENT_STRING_COMMAND_EXECUTE_COMMAND_LINE;
        AgentProtocol.nSequenceID   = nSequenceID;
        AgentProtocol.nProtocolType = KG_AGENT_PROTOCOL_TYPE_REQUEST;
        AgentProtocol.CommandParameterMap[(char *)KG_AGENT_STRING_PARAMETER_DSET_AGENT_CLIENT_NAME] = pProgramItem->szAgentName;
/*参数，待整理
        AgentProtocol.ProtocolParameter.push_back((char *)GetName());
        AgentProtocol.ProtocolParameter.push_back(
            (char *)KG_AGENT_STRING_PARAMETER_NEW_THREAD
        );
        AgentProtocol.ProtocolParameter.push_back(
            (char *)KG_AGENT_STRING_PARAMETER_WAIT_EXECUTE_FINISH
        );*/

        AgentProtocol.pszContext = pProgramItem->szStopProgram;

        ASSERT(m_piAgentGateway);
        nRetCode = m_piAgentGateway->SendToServer(&AgentProtocol);
        KGLOG_PROCESS_ERROR(nRetCode);

        pProgramItem->eProgramState = KG_AGENT_PROGRAM_STATE_STOPING;
        pProgramItem->nSequenceID   = nSequenceID;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KG_PlugIn_GM::OnMessage_ExecuteCommandLineResult(
    KG_AGENT_PROTOCOL *pAgentProtocol
)
{
	int nResult  = false;
	int nRetCode = false;

// Exit1:
	nResult = true;
Exit0:
	return nResult;
}