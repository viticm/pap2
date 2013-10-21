////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAIManager.h
//  Version     : 1.0
//  Creator     : Chen Jie, zhaochunfeng
//  Comment     : 管理各种AI类型数据
//
////////////////////////////////////////////////////////////////////////////////
#ifndef _KAIMANAGER_H_ 
#define _KAIMANAGER_H_

#include <map>
#include <string>

#include "Luna.h"
#include "KAIAction.h"
#include "KAIState.h"
#include "KAILogic.h"

class KAIManager
{
public:
    BOOL Init();
    void UnInit();

    void RegisterActionFunctions();

    BOOL LoadAITabFile();

    BOOL ReloadAILogic(int nAIType);

    KAILogic*       GetAILogic(int nAIType);

    KAI_ACTION_FUNC GetActionFunction(int nKey);

private:
    KAILogic* CreateAI(int nType, DWORD dwScriptID);

private:
    KAI_ACTION_FUNC m_ActionFunctionTable[eakTotal];

    struct KAIInfo
    {
        DWORD       dwScriptID;
        KAILogic*   pLogic;
    };

    typedef std::map<int, KAIInfo> KAI_TABLE;
    KAI_TABLE        m_AITable;
};


#endif
