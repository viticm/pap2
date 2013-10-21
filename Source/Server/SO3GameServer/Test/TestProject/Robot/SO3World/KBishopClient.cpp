#include "StdAfx.h"
#include <string.h>
#include <string>
#include "KBishopClient.h"
#include "Protocol/BishopClientProtocol.h"
#include "KRoleDBDataDef.h"
#include "Engine/KG_Time.h"
#include "KPlayerClient.h"
#include "KMonster.h"

using namespace std;

char KBishopClient::ms_szResultDescription[Login_Total][256] =
{
    "Bishop",
    "OK",
    "连接Bishop失败",
    "帐号验证失败",
    "获取角色列表失败",
    "创建角色失败",
    "删除角色失败",
    "请求登录游戏失败"
};

KBishopClient::KBishopClient(void)
{
    memset(m_szLoginIP, 0, sizeof(m_szLoginIP));
    memset(m_szAccountName, 0, sizeof(m_szAccountName));
    memset(&m_Password, 0, sizeof(m_Password));
    memset(m_szActivateCode, 0, sizeof(m_szActivateCode));
    memset(m_szLoginRole, 0, sizeof(m_szLoginRole));
    memset(m_szRoleToDelete, 0, sizeof(m_szRoleToDelete));
    memset(&m_RoleCreateParam, 0, sizeof(m_RoleCreateParam));
    m_piSocketStream                     = NULL;
    m_usLoginPort                        = 0;
    m_CurrentState                       = LOGIN_STATE_NONE;
    m_nAutoLoginFlag                     = false;

    m_pOwner = NULL;

    m_nRetryCount = 0;

    m_nLoginResult = Login_Invalid;
}

KBishopClient::~KBishopClient(void)
{
    KG_COM_RELEASE(m_piSocketStream);
}

int KBishopClient::Init(KMonster *pMonster)
{
    int nResult = FALSE;
    KGLOG_PROCESS_ERROR(pMonster);
    m_pOwner = pMonster;

    m_nRetryCount = 0;

    m_nLoginResult = Login_Invalid;

    nResult = TRUE;
Exit0:
    return nResult;
}

int KBishopClient::UnInit()
{
    m_pOwner = NULL;    
    m_nRetryCount = 0;
    return true;
}

LOGIN_RESULT KBishopClient::GetLoginResult()
{
    return m_nLoginResult;
}

const char *KBishopClient::GetResultDescription()
{
    return ms_szResultDescription[m_nLoginResult];
}

int KBishopClient::BeginLogin(const char cszLoginIP[], unsigned short usLoginPort, const char cszAccountName[], const char cszPasswordMD5[])
{
    ASSERT(cszAccountName);
    ASSERT(cszPasswordMD5);

    ASSERT(m_CurrentState == LOGIN_STATE_NONE);

    ASSERT(cszLoginIP);
    strncpy(m_szLoginIP, cszLoginIP, sizeof(m_szLoginIP));
    m_szLoginIP[sizeof(m_szLoginIP) - 1] = '\0';
    m_usLoginPort = usLoginPort;

    strncpy(m_szAccountName, cszAccountName, sizeof(m_szAccountName));
    m_szAccountName[sizeof(m_szAccountName) - 1] = '\0';

    strncpy(m_Password.szPassword, cszPasswordMD5, sizeof(m_Password.szPassword));
    m_Password.szPassword[sizeof(m_Password.szPassword) - 1] = '\0';

    memset(m_szActivateCode, 0, sizeof(m_szActivateCode));
    
    Login();

    return true;
}


int KBishopClient::AutoLogin()
{
    ASSERT(m_CurrentState == LOGIN_STATE_NONE);
    m_nAutoLoginFlag = true;
    Login();
    return true;
}

int KBishopClient::Login()
{
    m_nRetryCount = 0;

    while (!m_pOwner->GetExitFlag() && m_nRetryCount++ < MAX_RETRY_COUNT)
    {       
        m_CurrentState = LOGIN_STATE_START;

        ProcessLogin();

        if (m_nLoginResult == Login_Sucess)
        {   
            break;            
        }
        KGThread_Sleep(1000);
        //printf("   Retry:%d\n", m_nRetryCount);
    }
    return TRUE;
}

int KBishopClient::ProcessLogin()
{
    m_nLoginResult = Login_Invalid;

    while (!m_pOwner->GetExitFlag())
    {
        Activate();

        if (m_nLoginResult != Login_Invalid)
            break;

        KGThread_Sleep(10);
    }
    return TRUE;
}

int KBishopClient::Activate()
{
    switch (m_CurrentState) 
    {        
    case LOGIN_STATE_NONE:
        break;
    case LOGIN_STATE_START:
        login_state_start();
        break;
    case LOGIN_STATE_REQUEST_LOGIN_VERIFY:
        login_state_request_login_verify();
        break;
    case LOGIN_STATE_WAIT_LOGIN_VERIFY_RESPOND:
        login_state_wait_login_verify_respond();
        break;
    case LOGIN_STATE_WAIT_PLAYER_INPUT_PSW_AGAIN:
        // do nothing !
        break;
    case LOGIN_STATE_WAIT_PLAYER_INPUT_ACTIVATE_CODE:
        // do nothing !
        break;
    case LOGIN_STATE_WAIT_ROLE_LIST:
        login_state_wait_role_list();
        break;
    case LOGIN_STATE_WAIT_PLAYER_OPERATE:
        login_state_wait_player_operate();
        break;
    case LOGIN_STATE_REQUEST_CREATE_ROLE:
        login_state_request_create_role();
        break;
    case LOGIN_STATE_WAIT_CREATE_RESPOND:
        login_state_wait_create_respond();
        break;
    case LOGIN_STATE_REQUEST_DELETE_ROLE:
        login_state_request_delete_role();
        break;
    case LOGIN_STATE_WAIT_DELETE_RESPOND:
        login_state_wait_delete_respond();
        break;
    case LOGIN_STATE_REQUEST_LOGIN_GAME:
        login_state_request_login_game();
        break;
    case LOGIN_STATE_WAIT_GAME_LOGIN_INFO:
        login_state_wait_game_login_info();
        break;
    default:
        ASSERT(!"Unexpected state !");
        break;
    }

    while (m_CurrentState == LOGIN_STATE_WAIT_GAME_LOGIN_INFO)
    {
        login_state_wait_game_login_info();
        KGThread_Sleep(10);
    }

    return true;
}

void KBishopClient::Cancel()
{
    KG_COM_RELEASE(m_piSocketStream);
    memset(m_szActivateCode, 0, sizeof(m_szActivateCode));
    m_CurrentState = LOGIN_STATE_NONE;
}

int KBishopClient::GetRoleCount()
{
    return (int)m_RoleList.size();
}

int KBishopClient::GetRoleListItem(UI_ROLE_LIST_ITEM *pRoleListItem, int nRoleIndex)
{
    int                 nResult    = false;
    int                 nRoleCount = 0;

    ASSERT(pRoleListItem);
    ASSERT(nRoleIndex >= 0);
    nRoleCount = (int)m_RoleList.size();
    ASSERT(nRoleIndex < nRoleCount);

    *pRoleListItem = m_RoleList[nRoleIndex];
    
    nResult = true;
//Exit0:
    return nResult;
}

int KBishopClient::RoleLogin(const char cszRoleName[])
{
    int nResult  = false;
    int nRetCode = false;
    ROLE_LIST::iterator it;

    ASSERT(cszRoleName);
    ASSERT(m_CurrentState == LOGIN_STATE_WAIT_PLAYER_OPERATE);

    for (it = m_RoleList.begin(); it != m_RoleList.end(); ++it) 
    {
        nRetCode = strcmp(it->szRoleName, cszRoleName);
        if (!nRetCode)
            break;
    }

    if (it == m_RoleList.end()) 
    {
        ASSERT(!"Selected role not in list");
        goto Exit0;
    }

    strncpy(m_szLoginRole, cszRoleName, sizeof(m_szLoginRole));
    m_szLoginRole[sizeof(m_szLoginRole) - 1] = '\0';

    m_CurrentState = LOGIN_STATE_REQUEST_LOGIN_GAME;

    nResult = true;
Exit0:
    return nResult;
}

int KBishopClient::DeleteRole(const char cszRoleName[], const char cszSecPasswordMD5[])
{
    ASSERT(cszRoleName);
    ASSERT(m_CurrentState == LOGIN_STATE_WAIT_PLAYER_OPERATE);
    strncpy(m_szRoleToDelete, cszRoleName, sizeof(m_szRoleToDelete));
    m_szRoleToDelete[sizeof(m_szRoleToDelete) - 1] = '\0';
    m_CurrentState = LOGIN_STATE_REQUEST_DELETE_ROLE;
    return true;
}

int KBishopClient::CreateRole(const UI_ROLE_CREATE_PARAM &crRoleCreateParam)
{
    int nResult  = false;
    int nRetCode = false;

    ASSERT(m_CurrentState == LOGIN_STATE_WAIT_PLAYER_OPERATE);
    m_RoleCreateParam = crRoleCreateParam;
    m_RoleCreateParam.szRoleName[sizeof(m_RoleCreateParam.szRoleName) - 1] = '\0';
    m_CurrentState = LOGIN_STATE_REQUEST_CREATE_ROLE;

    nResult = true;
//Exit0:
    return nResult;
}

int KBishopClient::login_state_start()
{
    int                 nResult         = false;
    int                 nRetCode        = false;
    timeval             TimeoutValue    = {60 * 3, 0};
    KG_SocketConnector  Connector;

    ASSERT(!m_piSocketStream);
    m_piSocketStream = Connector.ConnectSecurity(m_szLoginIP, m_usLoginPort, KSG_ENCODE_DECODE);

    KGLogPrintf(
        KGLOG_INFO, 
        "[KBishopClient] Connect to %s : %u ... ... [%s]", 
        m_szLoginIP, m_usLoginPort, m_piSocketStream ? "OK" : "Failed"
    );

    if (!m_piSocketStream)
    {
         goto Exit0;
    }

    nRetCode = m_piSocketStream->SetTimeout(&TimeoutValue);
    ASSERT(nRetCode);
    
    m_CurrentState = LOGIN_STATE_REQUEST_LOGIN_VERIFY;    

    nResult = true;
Exit0:
    if (!nResult) 
    {
        m_nLoginResult = Login_ConnectBishop;
        m_CurrentState = LOGIN_STATE_NONE;
        KG_COM_RELEASE(m_piSocketStream);
    }
    return nResult;
}

int KBishopClient::login_state_request_login_verify()
{
    int                           nResult          = false;
    int                           nRetCode         = false;
    IKG_Buffer                   *piSendBuffer     = NULL;
    PLAYER_LOGIN_GATEWAY_REQUEST *pLoginVerifyInfo = NULL;

    ASSERT(m_piSocketStream);
    piSendBuffer = KG_MemoryCreateBuffer(sizeof(PLAYER_LOGIN_GATEWAY_REQUEST));
    KGLOG_PROCESS_ERROR(piSendBuffer);
    pLoginVerifyInfo = (PLAYER_LOGIN_GATEWAY_REQUEST *)piSendBuffer->GetData();
    ASSERT(pLoginVerifyInfo);

    strncpy(pLoginVerifyInfo->szAccountName, m_szAccountName, sizeof(pLoginVerifyInfo->szAccountName));
    pLoginVerifyInfo->szAccountName[sizeof(pLoginVerifyInfo->szAccountName) - 1] = '\0';

    strncpy(pLoginVerifyInfo->szActivateCode, m_szActivateCode, sizeof(pLoginVerifyInfo->szActivateCode));
    pLoginVerifyInfo->szActivateCode[sizeof(pLoginVerifyInfo->szActivateCode) - 1] = '\0';
    memset(m_szActivateCode, 0, sizeof(m_szActivateCode));

    pLoginVerifyInfo->cProtocol                 = C2B_PLAYER_LOGIN_INFO;
    pLoginVerifyInfo->Password                  = m_Password;
    pLoginVerifyInfo->nGameWorldProtocolVersion = SWORD3_PROTOCOL_VERSION;
    pLoginVerifyInfo->nGatewayProtocolVersion   = BISHOP_PROTOCOL_VERSION;

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    if (nRetCode != 1)
    {
        goto Exit0;
    }
    
    m_CurrentState = LOGIN_STATE_WAIT_LOGIN_VERIFY_RESPOND;
    
    nResult = true;
Exit0:
    if (!nResult) 
    {  
        m_nLoginResult = Login_VerifyAccount;
        m_CurrentState = LOGIN_STATE_NONE;
        KG_COM_RELEASE(m_piSocketStream);
    }
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KBishopClient::login_state_wait_login_verify_respond()
{
    int                             nResult         = false;
    int                             nRetCode        = false;
    timeval                         TimeoutValue    = {0, 100 * 1000};
    unsigned                        uBufferSize     = 0;
    IKG_Buffer                     *piRespondBuffer = NULL;
    PLAYER_LOGIN_GATEWAY_RESPOND   *pVerifyRespond  = NULL;

    ASSERT(m_piSocketStream);
    nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
    KGLOG_PROCESS_ERROR(nRetCode != -1);
    KG_PROCESS_SUCCESS(!nRetCode);

    nRetCode = m_piSocketStream->Recv(&piRespondBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);
    ASSERT(piRespondBuffer);

    uBufferSize = piRespondBuffer->GetSize();
    KGLOG_PROCESS_ERROR(uBufferSize == sizeof(PLAYER_LOGIN_GATEWAY_RESPOND));
    
    pVerifyRespond = (PLAYER_LOGIN_GATEWAY_RESPOND *)piRespondBuffer->GetData();
    ASSERT(pVerifyRespond);
    KGLOG_PROCESS_ERROR(pVerifyRespond->cProtocol == B2C_ACCOUNT_VERIFY_RESULT);

    switch (pVerifyRespond->nRespondCode) 
    {
    case LOGIN_R_SUCCESS:        
        if (m_nAutoLoginFlag)
        {
            // 自动选择角色登陆(断线重连)
            m_CurrentState   = LOGIN_STATE_REQUEST_LOGIN_GAME;
            m_nAutoLoginFlag = false;
        }
        else
        {
            // 手动选择角色登陆时，需要等待角色列表
            m_CurrentState = LOGIN_STATE_WAIT_ROLE_LIST;
        }
    	break;
    case LOGIN_R_ACCOUNT_OR_PASSWORD_ERROR:
    case LOGIN_R_ACTIVE:
    case LOGIN_R_CDKEY:
    case LOGIN_R_SUCCESS_AND_IN_GAME:
    case LOGIN_R_TIMEOUT:
    case LOGIN_R_SYSTEM_MAINTENANCE:
    case LOGIN_R_INVALID_PROTOCOLVERSION:
    case LOGIN_R_INVALID_PROTO:
    case LOGIN_R_ACCOUNT_EXIST:
    case LOGIN_R_FREEZE:
    case LOGIN_R_BLACK_LIST:
    case LOGIN_R_ACC_IN_GATEWAY:        
    default:
        goto Exit0;
        break;  
    }

Exit1:
    nResult = true;
Exit0:
    if (!nResult) 
    {         
        m_nLoginResult = Login_VerifyAccount;
        m_CurrentState = LOGIN_STATE_NONE;
        KG_COM_RELEASE(m_piSocketStream);
    }
    KG_COM_RELEASE(piRespondBuffer);
    return nResult;
}

int KBishopClient::login_state_wait_role_list()
{
    int             nResult         = false;
    int             nRetCode        = false;
    TProcessData   *pTProcessData   = NULL;
    IKG_Buffer     *piBuffer        = NULL;
    unsigned        uBufferSize     = 0;
    timeval         TimeoutValue    = {0, 100 * 1000};
    int             nRoleCount      = 0;
    KBaseData      *pRoleBaseInfo   = NULL;
    int             nRoleIndex      = 0;

    ASSERT(m_piSocketStream);
    nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
    KGLOG_PROCESS_ERROR(nRetCode != -1);
    KG_PROCESS_SUCCESS(!nRetCode);

    nRetCode = m_piSocketStream->Recv(&piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    uBufferSize = piBuffer->GetSize();
    KGLOG_PROCESS_ERROR(uBufferSize >= sizeof(TProcessData));
    pTProcessData = (TProcessData *)piBuffer->GetData();
    ASSERT(pTProcessData);
    KGLOG_PROCESS_ERROR(pTProcessData->nProtoId == B2C_RETURN_ROLE_LIST);
    nRoleCount = pTProcessData->pDataBuffer[0];
    KGLOG_PROCESS_ERROR(uBufferSize == sizeof(TProcessData) + nRoleCount * sizeof(KBaseData));
    pRoleBaseInfo = (KBaseData *)(pTProcessData->pDataBuffer + 1);

    m_RoleList.clear();

    for (nRoleIndex = 0; nRoleIndex < nRoleCount; ++nRoleIndex) 
    {
        UI_ROLE_LIST_ITEM RoleListItem;

        pRoleBaseInfo[nRoleIndex].szAccount[sizeof(pRoleBaseInfo[nRoleIndex].szAccount) - 1] = 0;
        pRoleBaseInfo[nRoleIndex].szRoleName[sizeof(pRoleBaseInfo[nRoleIndex].szRoleName) - 1] = 0;
        
        strncpy(RoleListItem.szAccountName, pRoleBaseInfo[nRoleIndex].szAccount, sizeof(RoleListItem.szAccountName));
        RoleListItem.szAccountName[sizeof(RoleListItem.szAccountName) - 1] = '\0';

        strncpy(RoleListItem.szRoleName, pRoleBaseInfo[nRoleIndex].szRoleName, sizeof(RoleListItem.szRoleName));
        RoleListItem.szRoleName[sizeof(RoleListItem.szRoleName) - 1] = '\0';

        RoleListItem.nRoleType = pRoleBaseInfo[nRoleIndex].cRoleType;
        RoleListItem.nLevel    = (int)(pRoleBaseInfo[nRoleIndex].byLevel);

        memcpy(RoleListItem.wRepresentID, pRoleBaseInfo->wRepresentID, sizeof(RoleListItem.wRepresentID));

        m_RoleList.push_back(RoleListItem);
    }
    
    m_CurrentState = LOGIN_STATE_WAIT_PLAYER_OPERATE;
    
Exit1:    
    nResult = true;
Exit0:
    if (!nResult) 
    {  
        m_nLoginResult = Login_GetRolelist;
        m_CurrentState = LOGIN_STATE_NONE;
        KG_COM_RELEASE(m_piSocketStream);
    }
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

int KBishopClient::login_state_wait_player_operate()
{
    int nRetCode = FALSE;
    char szRoleName[_NAME_LEN];

    snprintf(szRoleName, _NAME_LEN - 1, "%d_%s", 
        m_pOwner->m_MonsterParam.nCreateRoleSN,
        m_szAccountName
    );
    szRoleName[_NAME_LEN - 1] = '\0';

    /////////////选择角色/////////////////////
    for (ROLE_LIST::iterator it = m_RoleList.begin(); it != m_RoleList.end(); ++it)
    {
        nRetCode = strcmp(it->szRoleName, szRoleName);
        if (nRetCode == 0)
        {
            RoleLogin(szRoleName);
            goto Exit0;
        }
    }
    ///////////////////////////////////////////


    /////////////删除角色/////////////////////
    nRetCode = GetRoleCount();
    if (nRetCode >= 3)
    {
        DeleteRole(m_RoleList[0].szRoleName, m_Password.szPassword);
        goto Exit0;
    }
    ///////////////////////////////////////////


    /////////////创建角色/////////////////////
    UI_ROLE_CREATE_PARAM crRoleCreateParam;
    memset(&crRoleCreateParam, 0, sizeof(crRoleCreateParam));
    crRoleCreateParam.nRoleType = 1;
    crRoleCreateParam.dwMapID = 1;
    crRoleCreateParam.dwMapCopyIndex = 1;            
    strcpy(crRoleCreateParam.szRoleName, szRoleName);

    CreateRole(crRoleCreateParam);
    //////////////////////////////////////////

Exit0:
    return true;
}

int KBishopClient::login_state_request_create_role()
{
    int                     nResult          = false;
    int                     nRetCode         = false;
    TProcessData           *pTProcessData    = NULL;
    IKG_Buffer             *piSendBuffer     = NULL;
    KROLE_GENERATOR_PARAM  *pRoleCreateParam = NULL;


    piSendBuffer = KG_MemoryCreateBuffer(sizeof(TProcessData)  + sizeof(KROLE_GENERATOR_PARAM));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pTProcessData = (TProcessData *)piSendBuffer->GetData();
    ASSERT(pTProcessData);

    pTProcessData->nProtoId         = C2B_CREATE_ROLE;
    pTProcessData->bLeave           = false;    // no use
    pTProcessData->ulIdentity       = 0;        // no use
    pTProcessData->nDataLen         = 1 + sizeof(KROLE_GENERATOR_PARAM);
    pTProcessData->pDataBuffer[0]   = 0;        // no use

    pRoleCreateParam = (KROLE_GENERATOR_PARAM *)(pTProcessData->pDataBuffer + 1);


    strncpy(pRoleCreateParam->szRoleName, m_RoleCreateParam.szRoleName, sizeof(pRoleCreateParam->szRoleName));
    pRoleCreateParam->szRoleName[sizeof(pRoleCreateParam->szRoleName) - 1] = '\0';
    
    strncpy(pRoleCreateParam->szAccountName, m_szAccountName, sizeof(pRoleCreateParam->szAccountName));
    pRoleCreateParam->szAccountName[sizeof(pRoleCreateParam->szAccountName) - 1] = '\0';

    pRoleCreateParam->cRoleType         = (char)m_RoleCreateParam.nRoleType;
    pRoleCreateParam->dwMapID           = m_RoleCreateParam.dwMapID;
    pRoleCreateParam->dwMapCopyIndex    = m_RoleCreateParam.dwMapCopyIndex;

    memcpy(pRoleCreateParam->wRepresentID, m_RoleCreateParam.wRepresentID, sizeof(pRoleCreateParam->wRepresentID));

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    m_CurrentState = LOGIN_STATE_WAIT_CREATE_RESPOND;
    
    nResult = true;
Exit0:
    if (!nResult) 
    {  
        m_nLoginResult = Login_CreateRole;
        m_CurrentState = LOGIN_STATE_NONE;
        KG_COM_RELEASE(m_piSocketStream);        
    }
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KBishopClient::login_state_wait_create_respond()
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    tagNewDelRoleResponse  *pCreateRoleRespond  = NULL;
    IKG_Buffer             *piBuffer            = NULL;
    unsigned                uBufferSize         = 0;
    timeval                 TimeoutValue        = {0, 100 * 1000};

    ASSERT(m_piSocketStream);
    nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
    KGLOG_PROCESS_ERROR(nRetCode != -1);
    KG_PROCESS_SUCCESS(!nRetCode);

    nRetCode = m_piSocketStream->Recv(&piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    uBufferSize = piBuffer->GetSize();
    KGLOG_PROCESS_ERROR(uBufferSize == sizeof(tagNewDelRoleResponse));
    pCreateRoleRespond = (tagNewDelRoleResponse *)piBuffer->GetData();
    ASSERT(pCreateRoleRespond);
    KGLOG_PROCESS_ERROR(pCreateRoleRespond->cProtocol == B2C_CREATE_OR_DELETE_ROLE_RESULT);
    
    KGLOG_CHECK_ERROR(pCreateRoleRespond->bSucceeded);
    if (pCreateRoleRespond->bSucceeded)
    {
        strncpy(m_szLoginRole, m_RoleCreateParam.szRoleName, sizeof(m_szLoginRole));
        m_szLoginRole[sizeof(m_szLoginRole) - 1] = '\0';

		m_CurrentState = LOGIN_STATE_REQUEST_LOGIN_GAME;
    }
    else
    {        
		//m_CurrentState = LOGIN_STATE_WAIT_PLAYER_OPERATE;
        goto Exit0;
    }

Exit1:    
    nResult = true;
Exit0:
    if (!nResult) 
    {  
        m_nLoginResult = Login_CreateRole;
        m_CurrentState = LOGIN_STATE_NONE;
        KG_COM_RELEASE(m_piSocketStream);
    }
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

int KBishopClient::login_state_request_delete_role()
{
    int                     nResult          = false;
    int                     nRetCode         = false;
    tagDBDelPlayer         *pDeleteRole      = NULL;
    IKG_Buffer             *piSendBuffer     = NULL;


    piSendBuffer = KG_MemoryCreateBuffer(sizeof(tagDBDelPlayer));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pDeleteRole = (tagDBDelPlayer *)piSendBuffer->GetData();
    ASSERT(pDeleteRole);
    
    pDeleteRole->cProtocol = C2B_DELETE_ROLE;

    strncpy(pDeleteRole->szAccountName, m_szAccountName, sizeof(pDeleteRole->szAccountName));
    pDeleteRole->szAccountName[sizeof(pDeleteRole->szAccountName) - 1] = '\0';

    strncpy(pDeleteRole->szRoleName, m_szRoleToDelete, sizeof(pDeleteRole->szRoleName));
    pDeleteRole->szRoleName[sizeof(pDeleteRole->szRoleName) - 1] = '\0';

    pDeleteRole->Password = m_Password;

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    m_CurrentState = LOGIN_STATE_WAIT_DELETE_RESPOND;
    nResult = true;
Exit0:
    if (!nResult) 
    {  
        m_nLoginResult = Login_DeleteRole;
        m_CurrentState = LOGIN_STATE_NONE;
        KG_COM_RELEASE(m_piSocketStream);       
    }
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KBishopClient::login_state_wait_delete_respond()
{
    int                     nResult             = false;
    int                     nRetCode            = false;
    tagNewDelRoleResponse  *pDeleteRoleRespond  = NULL;
    IKG_Buffer             *piBuffer            = NULL;
    unsigned                uBufferSize         = 0;
    timeval                 TimeoutValue        = {0, 100 * 1000};

    ASSERT(m_piSocketStream);
    nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
    KGLOG_PROCESS_ERROR(nRetCode != -1);
    KG_PROCESS_SUCCESS(!nRetCode);

    nRetCode = m_piSocketStream->Recv(&piBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    uBufferSize = piBuffer->GetSize();
    KGLOG_PROCESS_ERROR(uBufferSize == sizeof(tagNewDelRoleResponse));
    pDeleteRoleRespond = (tagNewDelRoleResponse *)piBuffer->GetData();
    ASSERT(pDeleteRoleRespond);
    KGLOG_PROCESS_ERROR(pDeleteRoleRespond->cProtocol == B2C_CREATE_OR_DELETE_ROLE_RESULT);
    pDeleteRoleRespond->szRoleName[sizeof(pDeleteRoleRespond->szRoleName) - 1] = '\0';
    
    if (pDeleteRoleRespond->bSucceeded)
    {
        ROLE_LIST::iterator it;

        for (it = m_RoleList.begin(); it != m_RoleList.end(); ++it) 
        {
            nRetCode = strcmp(it->szRoleName, pDeleteRoleRespond->szRoleName);
            if (!nRetCode)
            {
                m_RoleList.erase(it);
                break;
            }
        }
    }
    else
    {        
        goto Exit0;
    }

    m_CurrentState = LOGIN_STATE_WAIT_PLAYER_OPERATE;
    
Exit1:    
    nResult = true;
Exit0:
    if (!nResult) 
    {  
        m_nLoginResult = Login_DeleteRole;
        m_CurrentState = LOGIN_STATE_NONE;
        KG_COM_RELEASE(m_piSocketStream);
    }
    KG_COM_RELEASE(piBuffer);
    return nResult;
}

int KBishopClient::login_state_request_login_game()
{
    int                 nResult      = false;
    int                 nRetCode     = false;
    tagDBSelPlayer     *pRoleLogin   = NULL;
    IKG_Buffer         *piSendBuffer = NULL;

    piSendBuffer = KG_MemoryCreateBuffer(sizeof(tagDBSelPlayer));
    KGLOG_PROCESS_ERROR(piSendBuffer);

    pRoleLogin = (tagDBSelPlayer *)piSendBuffer->GetData();
    ASSERT(pRoleLogin);

    // 注意: 一个刚创建的角色是不在m_RoleList里面的,所以不要在这里检验"在不在角色列表"

    pRoleLogin->cProtocol  = C2B_GAME_LOGIN_REQUEST;
    pRoleLogin->ulIdentity = 0; // no use
    strncpy(pRoleLogin->szRoleName, m_szLoginRole, sizeof(pRoleLogin->szRoleName));
    pRoleLogin->szRoleName[sizeof(pRoleLogin->szRoleName) - 1] = '\0';

    nRetCode = m_piSocketStream->Send(piSendBuffer);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    m_CurrentState = LOGIN_STATE_WAIT_GAME_LOGIN_INFO;
    nResult = true;
Exit0:
    if (!nResult) 
	{  
        m_nLoginResult = Login_RequestLoginGameserver;
		m_CurrentState = LOGIN_STATE_NONE;
        KG_COM_RELEASE(m_piSocketStream);
    }
    KG_COM_RELEASE(piSendBuffer);
    return nResult;
}

int KBishopClient::login_state_wait_game_login_info()
{
    int                    nResult             = false;
    int                    nRetCode            = false;
    tagNotifyPlayerLogin  *pGameLoginInfo      = NULL;
    IKG_Buffer            *piBuffer            = NULL;
    unsigned               uBufferSize         = 0;
    timeval                TimeoutValue        = {0, 100 * 1000};
    struct in_addr         GameServerAddress;
    const char            *pcszGameServerIP    = NULL;

    ASSERT(m_piSocketStream);
    nRetCode = m_piSocketStream->CheckCanRecv(&TimeoutValue);
    KGLOG_PROCESS_ERROR(nRetCode != -1);
    KG_PROCESS_SUCCESS(!nRetCode);

    nRetCode = m_piSocketStream->Recv(&piBuffer);
    m_CurrentState = LOGIN_STATE_NONE;
    KG_COM_RELEASE(m_piSocketStream);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    uBufferSize = piBuffer->GetSize();
    KGLOG_PROCESS_ERROR(uBufferSize == sizeof(tagNotifyPlayerLogin));
    pGameLoginInfo = (tagNotifyPlayerLogin *)piBuffer->GetData();
    ASSERT(pGameLoginInfo);
    KGLOG_PROCESS_ERROR(pGameLoginInfo->cProtocol == B2C_NOTIFY_PLAYER_LOGIN_GAMESVR);

    KGLOG_CHECK_ERROR(pGameLoginInfo->nResult == ROLE_LOGIN_RESULT_SUCCESS);
    switch (pGameLoginInfo->nResult) 
    {
    case ROLE_LOGIN_RESULT_SUCCESS:
        // 启动游戏世界状态机 ...
        m_nLoginResult = Login_Sucess;

        GameServerAddress.s_addr = pGameLoginInfo->nIPAddr;
        pcszGameServerIP = inet_ntoa(GameServerAddress);
        ASSERT(pcszGameServerIP);
        KGLOG_PROCESS_ERROR(m_pOwner);
        nRetCode = m_pOwner->m_PlayerClient.Connect(pcszGameServerIP, pGameLoginInfo->wPort);
        if (nRetCode)
        {
            nRetCode = m_pOwner->m_PlayerClient.DoApplyPlayerData(pGameLoginInfo->szRoleName, pGameLoginInfo->guid);            
        }
        
    	break;
    case ROLE_LOGIN_RESULT_MAINTENANCE:
    case ROLE_LOGIN_RESULT_IS_FULL:
    case ROLE_LOGIN_RESULT_UNKNOWN:
    default:
        goto Exit0;
        break;
    }
    
Exit1:    
    nResult = true;
Exit0:
    if (!nResult) 
    {
        m_nLoginResult = Login_RequestLoginGameserver;
        m_CurrentState = LOGIN_STATE_NONE;
        KG_COM_RELEASE(m_piSocketStream);
    }

    KG_COM_RELEASE(piBuffer);    
    return nResult;
}
