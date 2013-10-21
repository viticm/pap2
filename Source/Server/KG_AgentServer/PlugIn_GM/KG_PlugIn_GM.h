////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_PlugIn_GM.h
//  Version     : 1.0
//  Creator     : tongxuehu, zoukewei
//  Create Date : 01/17/2007
//  Comment     : 
//  
//  Copyright (C) 2007 - All Rights Reserved
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_PLUGIN_GM_
#define _KG_PLUGIN_GM_ 1

#include "KG_AgentProtocol.h"
#include "IKG_AgentGateway.h"

typedef enum _KG_AGENT_PROGRAM_STAT
{
    KG_AGENT_PROGRAM_STATE_NOT_START,
    KG_AGENT_PROGRAM_STATE_STARTTING,
    KG_AGENT_PROGRAM_STATE_RUNNING,
    KG_AGENT_PROGRAM_STATE_STOPING
} KG_AGENT_PROGRAM_STAT;

typedef struct _PROGRAM_CONTROL_ITEM
{
    char szKey[KG_AGENT_STRING_MAX_SIZE];
    char szAgentName[KG_AGENT_STRING_MAX_SIZE];
    int  nIsStartWaitResult;
    int  nIsStopWaitResult;
    int  nStartPriority;
    int  nStopPriority;
    char szStartProgram[256];
    char szStopProgram[256];

    // runtime attribute
    int nSequenceID;
    int nProcessID;
    KG_AGENT_PROGRAM_STAT eProgramState;
} PROGRAM_CONTROL_ITEM;

typedef std::vector<PROGRAM_CONTROL_ITEM *> PROGRAM_CONTROL_ITEM_LIST;

typedef enum _KG_AGENT_GROUP_STAT
{
    KG_AGENT_GROUP_STATE_NOT_START,
    KG_AGENT_GROUP_STATE_STARTTING,
    KG_AGENT_GROUP_STATE_RUNNING,
    KG_AGENT_GROUP_STATE_STOPING,
    KG_AGENT_GROUP_STATE_STOP_FAIL
} KG_AGENT_GROUP_STAT;

const char KG_AGENT_SERVER_GM_INI_FILENAME[]            = "KG_AgentGMPlugIn.ini";
const int  KG_AGENT_SERVER_GM_MAX_ITEM_COUNT            = 100;
const char KG_AGENT_SERVER_GM_STRING_WAIT_RESULT[]      = "WaitResult";
const char KG_AGENT_SERVER_GM_STRING_NOT_WAIT_RESULT[]  = "NotWaitResult";
const char KG_AGENT_SERVER_GM_STRING_ALL[]              = "All";
const char KG_AGENT_SERVER_GM_STRING_START_GROUP[]      = "StartGroup";
const char KG_AGENT_SERVER_GM_STRING_STOP_GROUP[]       = "StopGroup";

class KG_PlugIn_GM
{
public:
    KG_PlugIn_GM();
    virtual ~KG_PlugIn_GM();

public:
    int Init(void *pvContext);
    int UnInit(void *pvContext);

public:
    int ReceiveFromServer(
        KG_AGENT_PROTOCOL *pAgentProtocol, 
        int               *pnSendMessageFlag,
        IKG_AgentGateway  *piAgentGateway
    );

    int ReceiveFromClient(
        KG_AGENT_PROTOCOL *pAgentProtocol, 
        int               *pnSendMessageFlag,
        IKG_AgentGateway  *piAgentGateway
    );

private:
    int OnMessage_ExecuteCommandLineResult(
        KG_AGENT_PROTOCOL *pAgentProtocol
    );

    int OnCommand_StartServerGroup(KG_AGENT_PROTOCOL *pAgentProtocol);
    int OnCommand_StopServerGroup(KG_AGENT_PROTOCOL *pAgentProtocol);

private:
    int _StartProgramsByPriority(int nPriority);
    int _StopProgramsByPriority(int nPriority);

private:
    int _GetProgramItemByKey(
        PROGRAM_CONTROL_ITEM_LIST  *pProgramItemList,
        const char                  cszKey[],
        PROGRAM_CONTROL_ITEM      **ppProgramItem
    );

private:
    IKG_AgentGateway *m_piAgentGateway;
};

#endif // _KG_PLUGIN_GM_
