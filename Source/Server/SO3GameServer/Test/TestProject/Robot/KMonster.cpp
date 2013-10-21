#include "StdAfx.h"
#include "engine/KSG_MD5_String.h"
#include "KMonster.h"
#include "KPlayer.h"
#include "KBishopClient.h"
#include "KPlayerClient.h"
#include "KPlayerEyeshot.h"
#include "KOperationSimulator.h"

char KMonster::ms_szStateDescription[osTotal][256] = 
{
    "    Login", 
    "  Offline",
    "Online",
    "  Offline"
};

KMonster::KMonster(void)
{
    m_pRole = NULL;
    m_nThreadFlag = FALSE;
    m_nExitFlag = FALSE;
    m_nGameLoop = 0;
    m_dwClientMapID = 0;
    m_dwClientMapCopyIndex = 0;
    m_piOperator = NULL;

    memset(&m_MonsterParam, 0, sizeof(m_MonsterParam));
}

KMonster::~KMonster(void)
{  
}

int KMonster::Init(MONSTER_PARAM &rParam)
{
    int nResult  = FALSE;
    int nRetCode = FALSE;

    m_MonsterParam = rParam;

    m_piOperator = CreateSimulator(ostGeneral, this);
    KG_PROCESS_ERROR(m_piOperator);

    nRetCode = m_Eyeshot.Init(this);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_PlayerClient.Init(this);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = m_BishopClient.Init(this);
    KG_PROCESS_ERROR(nRetCode);

    m_nExitFlag = FALSE;

    nRetCode = m_WorkThread.Create(WorkThreadFunction, (void *)this);
    KG_PROCESS_ERROR(nRetCode);
    m_nThreadFlag = TRUE;

    nResult = TRUE;
Exit0:
    return nResult;
}

int KMonster::UnInit()
{
    SetExitFlag();

    if (m_nThreadFlag)
    {
        m_WorkThread.Destroy();
        m_nThreadFlag = FALSE;
    }

    DestroySimulator(m_piOperator);
    m_piOperator = NULL;

    m_Eyeshot.UnInit();
    m_PlayerClient.UnInit();
    m_BishopClient.UnInit();

    m_pRole = NULL;

    return TRUE;
}

const char *KMonster::GetStateDescription()
{
    return ms_szStateDescription[m_nOnlineState];
}

const char *KMonster::GetFailDescription()
{
    switch (m_nOnlineState)
    {
    case osBishopOnline:    
    case osBishopOffline:
        return m_BishopClient.GetResultDescription();
        break;
    case osGameserverOnline:
    case osGameserverOffline:
        return m_PlayerClient.GetStateDecription();
        break;
    }
    return "";
}

int KMonster::Login(LOGIN_TYPE nLoginType)
{
    char szPwdMD5[64];
    int nResult = FALSE;

    m_nOnlineState = osBishopOnline;

    if (nLoginType == ltConnectTo)
    {
        KG_EDStringToMD5String(szPwdMD5, m_MonsterParam.szPassword);
        m_BishopClient.BeginLogin(m_MonsterParam.szBishopIP, m_MonsterParam.nBishopPort, 
            m_MonsterParam.szAccountName, szPwdMD5);
    }
    else
    {
        m_BishopClient.AutoLogin();
    }

    KG_PROCESS_ERROR(m_BishopClient.GetLoginResult() == Login_Sucess);

    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        m_nOnlineState = osBishopOffline;
    }
    return nResult;
}

int KMonster::GameLoop()
{
    int nResult = FALSE;
    int nServerLoop = 0;

    KG_PROCESS_ERROR(m_PlayerClient.GetOnlineState() == gmsOnline);
    
    m_nOnlineState = osGameserverOnline;

    while (!m_nExitFlag)
    {
        m_PlayerClient.Breathe();

        if (m_PlayerClient.GetOnlineState() != gmsOnline)
        {
            Reset();
            goto Exit0;
        }

        nServerLoop = m_PlayerClient.GetServerLoop();
        while (m_nGameLoop < nServerLoop - GAME_FPS / 2)
        {
            Activate();
        }

        if (m_nGameLoop < nServerLoop + GAME_FPS / 2)
            Activate();

        m_piOperator->Activate();

        KGThread_Sleep(10);
    }
    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        m_nOnlineState = osGameserverOffline;
    }
    return FALSE;
}

int KMonster::Activate()
{
    ++m_nGameLoop;

    m_Eyeshot.Activate();

    return TRUE;
}

int KMonster::Reset()
{
    m_piOperator->Stop();
    m_Eyeshot.DeleteAllObject();
    m_pRole = NULL;
    m_nGameLoop = 0;
    m_dwClientMapID = 0;
    m_dwClientMapCopyIndex = 0;
    return TRUE;
}

void KMonster::WorkThreadFunction(void *pvParam)
{
    KMonster *pThis = (KMonster *)pvParam;
    if (pThis)
    {
        pThis->ThreadFunction();
    }
}

int KMonster::ThreadFunction()
{
    int nRetCode = FALSE;
    LOGIN_TYPE nLoginType = ltConnectTo;
    int nConnectCount = 0;

    while (!m_nExitFlag)
    {
        ++nConnectCount;
        nRetCode = Login(nLoginType);
        if (nRetCode)
        {
            GameLoop();
            nLoginType = ltReconnectTo;
            nConnectCount = 0;
        }

        if (!m_MonsterParam.nReconnect || nConnectCount > 10)
        {            
            break;
        }
        
        //printf("ReConnect Count:%d\n", nConnectCount);
    }

    return TRUE;
}