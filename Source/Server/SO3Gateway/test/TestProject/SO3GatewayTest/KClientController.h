///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
// 
// FileName :   KClientController.h
// Creator  :   liuzhibiao
// Date     :   2008-11-24
// Comment  :   v1.0
//              v1.0    Create  
///////////////////////////////////////////////////////////////

#ifndef  KCLIENTCONTROLLER_H_
#define  KCLIENTCONTROLLER_H_

#include "KSimulateClient.h"

class KClientController
{
public:
    int    Init(const KG_SIMULATE_CLIENT_CONFIG& crSimulateClientConfig, int nTestPoint, int nClientCount);
    void   UnInit(); 
    int    Run();
    int    FillOperate(int nOperateCount, int nOperate...);
    int    GetReturnResult(int nClientNum, KG_RETURN_RESULT* pResult);
    int    CheckTheAccountLockedOrNot(int nClientNum, int* pnAccountLockedState);
    int    AddOperateToClient(int nClientNum, int nOperateCount, int nOperate...);
    int    ModifyClientUserName(int nClientNum, char szNewName[]);
    int    GetClientConfig(KG_SIMULATE_CLIENT_CONFIG* pClientConfig,int nClientNum);
    int    CheckAllClientTaskCompleteState(int* pnCompleteState);

private:
    int                      m_nClientCount;
    vector<KSimulateClient*> m_ClientsList;
};

#endif  //KCLIENTCONTROLLER_H_
