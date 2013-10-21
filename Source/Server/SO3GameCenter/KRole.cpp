#include "stdafx.h"
#include "KRole.h"
#include "KSO3GameCenter.h"
#include "SO3Result.h"

#define ROLE_SCRIPT_NAME    CENTER_SCRIPTS_DIR"/role.lua"

KRole::KRole(void)
{
    m_pszAccount                = NULL;
    m_szName[0]                 = '\0';
    m_dwPlayerID                = 0;
    m_nConnIndex                = 0;
    m_dwMapID                   = 0;
    m_nMapCopyIndex             = 0;

    m_nDestConnIndex            = 0;
    m_dwDestMapID               = 0;
    m_nDestMapCopyIndex         = 0;
    
    m_dwSystemTeamID            = ERROR_ID;
    m_dwTeamID                  = ERROR_ID;
    m_eCamp                     = cNeutral;
    m_dwTongID                  = ERROR_ID;

    m_byLevel                   = 0;
    m_byType                    = 0;
    m_dwForceID                 = 0;

	m_eRoleState                = rsOffline;
}

KRole::~KRole(void)
{
}

KROLE_STATE KRole::GetRoleState(void)
{
	return m_eRoleState;
}

void KRole::SetRoleState(KROLE_STATE eDestState)
{
    assert(eDestState > rsInvalid && eDestState < rsTotal);

    if (eDestState == rsOffline)
    {
        g_pSO3GameCenter->m_Gateway.DoPlayerLeaveGame(m_pszAccount);

		m_nConnIndex        = 0;
		m_nDestConnIndex    = 0;
        m_dwMapID           = 0;
        m_nMapCopyIndex     = 0;
        m_dwDestMapID       = 0;
        m_nDestMapCopyIndex = 0;

        g_pSO3GameCenter->m_ApexProxy.DoRoleLogout(m_dwPlayerID);
        g_pSO3GameCenter->m_TongManager.OnPlayerLeaveGame(this);
        g_pSO3GameCenter->m_GameServer.DoSyncFellowshipMapID(m_dwPlayerID, m_dwMapID);
        g_pSO3GameCenter->m_GameServer.DoSyncFellowshipPlayerLevel(m_dwPlayerID, 0, true);
        g_pSO3GameCenter->m_MailManager.UnLoadMailCount(m_dwPlayerID);
        g_pSO3GameCenter->m_MentorCenter.ClearMaxApprenticeNum(m_dwPlayerID);

        CallLogoutScript();
    }

	m_eRoleState = eDestState;
}

BOOL KRole::IsOnline()
{
    BOOL bResult = false;

    switch (m_eRoleState)
    {
    case rsInvalid:
    case rsOffline:
    case rsLG_WaitForPermit:
    case rsLG_WaitForLogin:
        break;

    case rsOnline:
    case rsCG_WaitForSrcLeave:
    case rsCG_WaitForGuid:
    case rsCG_WaitForCreateMap:
    case rsCG_WaitForPlayerData:
    case rsCG_WaitForEnter_Leave:
    case rsCG_WaitForDestEnter:
    case rsCG_WaitForDestTimeout:
    case rsCG_WaitForSrcTimeout:

        bResult = true;
        break;

    default:
        assert(false);
        break;
    }  
    return bResult;
}

BOOL KRole::UpdateMapCopyOwnerForTeamLeader()
{
    BOOL            bResult         = false;
    BOOL            bRetCode        = false;
    KTeam*          pTeam           = NULL;
    const KMapCopy* pcMapCopy       = NULL;
    KMapInfo*       pMapInfo        = NULL;
    KRole*          pOldOwner       = NULL;           

    KG_PROCESS_ERROR(m_dwTeamID != ERROR_ID);

    pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(m_dwTeamID);
    KGLOG_PROCESS_ERROR(pTeam);

    KG_PROCESS_ERROR(pTeam->dwAuthority[tatLeader] == m_dwPlayerID);

    // 由于队长可能移交给一个不在线的玩家,所以找不到地图也是正常的
    pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(m_dwMapID);
    KG_PROCESS_ERROR(pMapInfo);

    KG_PROCESS_SUCCESS(pMapInfo->m_nType != emtDungeon);
    
    pcMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(m_dwMapID, m_nMapCopyIndex);
    KG_PROCESS_ERROR(pcMapCopy);
    
    if (pcMapCopy->m_dwOwnerID != ERROR_ID)
    {
        pOldOwner = g_pSO3GameCenter->m_RoleManager.GetRole(pcMapCopy->m_dwOwnerID);
    }
    
    if (pOldOwner != NULL && pOldOwner->m_dwTeamID == m_dwTeamID)
    {
        bRetCode  = g_pSO3GameCenter->m_MapManager.SetMapCopyOwner(m_dwMapID, m_nMapCopyIndex, m_dwPlayerID);    
        KG_PROCESS_ERROR(bRetCode);
    }

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KRole::OnLeaveGS(int nConnIndex)
{
    BOOL        bResult         = false;
    BOOL        bRetCode        = false;
    KMapCopy*   pcMapCopy       = NULL;
    KMapInfo*   pMapInfo        = NULL;
    KTeam*      pTeam           = NULL;

	if (nConnIndex == m_nConnIndex)
	{
		switch (m_eRoleState)
		{
        case rsOffline:
            assert(false);
            break;

		case rsOnline:            
            bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwMapID, m_nMapCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

			SetRoleState(rsOffline);
			break;

        case rsCG_WaitForCreateMap:
            // 跨服,等待地图加载的过程中,玩家退出,或者源服务器宕机(可能源和目标是同一个服务器)
            bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwMapID, m_nMapCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

            // 将改玩家从副本的CallbackList删除掉
            pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(m_dwDestMapID);
            KGLOG_PROCESS_ERROR(pMapInfo);

            pcMapCopy = pMapInfo->GetMapCopy(m_nDestMapCopyIndex);
            KGLOG_PROCESS_ERROR(pcMapCopy);

            pcMapCopy->UnregisterCGCallbackPlayer(m_dwPlayerID);

            SetRoleState(rsOffline);
            break;

		case rsCG_WaitForPlayerData:
            // 将源副本人数减1
            bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwMapID, m_nMapCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

            // 将目标副本人数减1
            pcMapCopy = g_pSO3GameCenter->m_MapManager.GetMapCopy(m_dwDestMapID, m_nDestMapCopyIndex);
            if (pcMapCopy && pcMapCopy->m_nConnIndex == m_nDestConnIndex) // 目标GS可能已经宕机了,所以找不到目标副本是可能的
            {
                bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwDestMapID, m_nDestMapCopyIndex);
                KGLOG_PROCESS_ERROR(bRetCode);
            }

            SetRoleState(rsOffline);
            break;

		case rsCG_WaitForGuid:
            // 将源副本人数减1
            bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwMapID, m_nMapCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

            m_nConnIndex = 0;

			SetRoleState(rsCG_WaitForDestTimeout);
			break;

		case rsCG_WaitForEnter_Leave:
            // 将源副本人数减1
            bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwMapID, m_nMapCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

            g_pSO3GameCenter->m_ApexProxy.DoLeaveServer(m_dwPlayerID, m_nConnIndex);

            m_nConnIndex = 0;

			SetRoleState(rsCG_WaitForDestEnter);
			break;

		case rsCG_WaitForSrcLeave:
            // 跨服已经进入目标GS,等待从源GS离开
            // 将源副本人数减1
            bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwMapID, m_nMapCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

            g_pSO3GameCenter->m_ApexProxy.DoLeaveServer(m_dwPlayerID, m_nConnIndex);

            m_dwMapID            = m_dwDestMapID;
            m_nMapCopyIndex      = m_nDestMapCopyIndex;
            m_nConnIndex         = m_nDestConnIndex;

            g_pSO3GameCenter->m_ApexProxy.DoEntryServer(m_dwPlayerID, m_nConnIndex);
            
            g_pSO3GameCenter->m_GameServer.DoSyncFellowshipMapID(m_dwPlayerID, m_dwMapID);

		    SetRoleState(rsOnline);

            g_pSO3GameCenter->m_GameServer.DoConfirmPlayerLoginRespond(m_nDestConnIndex, m_dwPlayerID, true);

            if (m_dwTeamID != ERROR_ID)
            {
                pTeam = g_pSO3GameCenter->m_TeamCenter.GetTeam(m_dwTeamID);
                KGLOG_PROCESS_ERROR(pTeam);

                pMapInfo = g_pSO3GameCenter->m_MapManager.GetMapInfo(m_dwMapID);
                KGLOG_PROCESS_ERROR(pMapInfo);

                if (pMapInfo->m_nType == emtDungeon && pTeam->dwAuthority[tatLeader] == m_dwPlayerID)
                {
                    bRetCode = g_pSO3GameCenter->m_MapManager.SetMapCopyOwner(m_dwMapID, m_nMapCopyIndex, m_dwPlayerID);
                    KGLOG_PROCESS_ERROR(bRetCode);
                }
            }
			break;

		case rsCG_WaitForSrcTimeout:
            // 将源副本人数减1
            bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwMapID, m_nMapCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

			SetRoleState(rsOffline);
			break;

        case rsCG_WaitForDestEnter:
        case rsCG_WaitForDestTimeout:
            assert(false);
            break;

		default:
			assert(false);
            break;
		}
	}
    else if (nConnIndex == m_nDestConnIndex)
	{
		switch (m_eRoleState)
		{
        case rsOffline:
        case rsOnline:
            break;

		case rsLG_WaitForPermit:
		case rsLG_WaitForLogin:
			bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwDestMapID, m_nDestMapCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

			SetRoleState(rsOffline);
			break;

        case rsCG_WaitForCreateMap:
            // 目标地图所在服务器宕机了,这时要通知源服务器"SearchMap"失败,并切换玩家状态
            // 通知玩家,前方路途不通,稍后再来
            g_pSO3GameCenter->m_GameServer.DoSearchMapRespond(
                m_nConnIndex, m_dwPlayerID, smrcFailed, false, 
                m_dwDestMapID, m_nDestMapCopyIndex,
                0, 0, 0
            );
            SetRoleState(rsOnline);
            break;

        case rsCG_WaitForPlayerData:
            // 跨服等待角色信息的时候,目标GS宕机,这时什么也不用做,等待角色信息来了再退回去
            break;

        case rsCG_WaitForGuid:
            // 在等待GUID的时候,目标GS宕机了,这时应该给源GS回包否决这次跨服
            // 当然,没必要去减目标副本的人数,因为目标Server都宕机了,呵呵
            g_pSO3GameCenter->m_GameServer.DoTransferPlayerRespond(m_nConnIndex, m_dwPlayerID, false);

            SetRoleState(rsOnline);
            break;

		case rsCG_WaitForEnter_Leave:
		case rsCG_WaitForSrcLeave:
            // 玩家在跨服后期从目标GS退出,这时要等待玩家从源GS退出,然后离线
            // 注意,这时跨服并没有真正完成,玩家的ConnIndex还是算在源GS上的
            // 将目标副本人数减1
            bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwDestMapID, m_nDestMapCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);
            
			SetRoleState(rsCG_WaitForSrcTimeout);

            m_dwDestMapID       = 0;
            m_nDestMapCopyIndex = 0;
            m_nDestConnIndex    = 0;
			break;

		case rsCG_WaitForDestEnter:
		case rsCG_WaitForDestTimeout:
            // 将目标副本人数减1
            bRetCode = g_pSO3GameCenter->m_MapManager.DecreaseMapCopyPlayerCount(m_dwDestMapID, m_nDestMapCopyIndex);
            KGLOG_PROCESS_ERROR(bRetCode);

			SetRoleState(rsOffline);
			break;

        case rsCG_WaitForSrcTimeout:
            break;

		default:
            assert(false);
            break;
		}
	}

    bResult = true;
Exit0:
	return bResult;
}

BOOL KRole::CallLoginScript()
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    int         nIndex              = 0;
    DWORD       dwScriptID          = 0;
    const char  cszFunction[]       = "OnRoleLogin";

    dwScriptID = g_FileNameHash(ROLE_SCRIPT_NAME);

    g_pSO3GameCenter->m_ScriptManager.SafeCallBegin(&nIndex);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsScriptExist(dwScriptID);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsFuncExist(dwScriptID, cszFunction);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_ScriptManager.Push(this);

    g_pSO3GameCenter->m_ScriptManager.Call(dwScriptID, cszFunction, 0);

    bResult = true;
Exit0:
    g_pSO3GameCenter->m_ScriptManager.SafeCallEnd(nIndex);
    return bResult;
}

BOOL KRole::CallLogoutScript()
{
    BOOL        bResult             = false;
    BOOL        bRetCode            = false;
    int         nIndex              = 0;
    DWORD       dwScriptID          = 0;
    const char  cszFunction[]       = "OnRoleLogout";

    dwScriptID = g_FileNameHash(ROLE_SCRIPT_NAME);

    g_pSO3GameCenter->m_ScriptManager.SafeCallBegin(&nIndex);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsScriptExist(dwScriptID);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_pSO3GameCenter->m_ScriptManager.IsFuncExist(dwScriptID, cszFunction);
    KG_PROCESS_ERROR(bRetCode);

    g_pSO3GameCenter->m_ScriptManager.Push(this);

    g_pSO3GameCenter->m_ScriptManager.Call(dwScriptID, cszFunction, 0);

    bResult = true;
Exit0:
    g_pSO3GameCenter->m_ScriptManager.SafeCallEnd(nIndex);
    return bResult;
}

int KRole::LuaIsOnline(Lua_State* L)
{
    BOOL bResult = 0;

    bResult = IsOnline();

    Lua_PushBoolean(L, bResult);
    return 1;
}

DEFINE_LUA_CLASS_BEGIN(KRole)

    REGISTER_LUA_STRING_READONLY(KRole, Account)
    REGISTER_LUA_STRING_READONLY(KRole, Name)
    REGISTER_LUA_DWORD_READONLY(KRole, PlayerID)
    REGISTER_LUA_INTEGER_READONLY(KRole, ConnIndex)
    REGISTER_LUA_DWORD_READONLY(KRole, MapID)
    REGISTER_LUA_INTEGER_READONLY(KRole, MapCopyIndex)
    REGISTER_LUA_DWORD_READONLY(KRole, TeamID)
    REGISTER_LUA_ENUM_READONLY(KRole, Camp)
    REGISTER_LUA_DWORD_READONLY(KRole, TongID)
    REGISTER_LUA_INTEGER_READONLY(KRole, Level)
    REGISTER_LUA_INTEGER_READONLY(KRole, Type)
    REGISTER_LUA_TIME_READONLY(KRole, LastSaveTime)
    REGISTER_LUA_DWORD_READONLY(KRole, ForceID)
    REGISTER_LUA_FUNC(KRole, IsOnline)

DEFINE_LUA_CLASS_END(KRole)

