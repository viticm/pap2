#include "stdafx.h"
#include "KScriptFuncList.h"
#include "KGatewayClient.h"

#ifdef _CLIENT

#include "KSO3World.h"
#include "SO3UI\SO3GameWorldUIHandler.h"

//#define g_BishopClient g_GatewayClient

namespace KScriptFuncList
{
	int LuaSetGatewayAddress(Lua_State* L)
	{
        const char* cszLoginIP   = NULL;
        WORD        wLoginPort  = 0;

        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);
        
        cszLoginIP = (const char*)Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(cszLoginIP);

		wLoginPort = (WORD)Lua_ValueToNumber(L, 2);

        g_GatewayClient.SetGatewayAddress(cszLoginIP, wLoginPort);
Exit0:
        return 0;
	}

	int LuaBeginLogin(Lua_State* L)
	{   
        const char* cszAccountName   = NULL;
        const char* cszPassword      = NULL;
        const char* cszActiveCode    = NULL;
        int         nTopIndex        = Lua_GetTopIndex(L);
		BOOL bRetCode				 = FALSE;
		char szDestMD5String[64];
		

        KGLOG_PROCESS_ERROR(nTopIndex == 2 || nTopIndex == 3);

		cszAccountName = (const char*)Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(cszAccountName);

		cszPassword = (const char*)Lua_ValueToString(L, 2);
		KGLOG_PROCESS_ERROR(cszPassword);

		KGLOG_PROCESS_ERROR(g_pGameWorldUIHandler);

		bRetCode = g_pGameWorldUIHandler->GetPassword(cszPassword, szDestMD5String);
		KGLOG_PROCESS_ERROR(bRetCode);

        if (nTopIndex == 3)
        {
            cszActiveCode = (const char*)Lua_ValueToString(L, 3);
            KGLOG_PROCESS_ERROR(cszActiveCode);
        }

        g_GatewayClient.BeginLogin(cszAccountName, szDestMD5String, cszActiveCode);

Exit0:
        return 0;
	}

	int LuaGetRoleCount(Lua_State* L)
	{
        int nResult = 0;
        int nCount  = 0;

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

		nCount = g_GatewayClient.GetRoleCount();

		Lua_PushNumber(L, nCount);

        nResult = 1;
Exit0:
		return nResult;
	}

    int LuaGetLeftTime(Lua_State* L)
    {
        int     nResult      = 0;
        DWORD   dwLeftTime   = 0;

        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

        dwLeftTime = g_GatewayClient.GetLeftTime();

        Lua_PushNumber(L, dwLeftTime);

        nResult = 1;
Exit0:
        return nResult;
    }

    int LuaGetLastLoginTime(Lua_State* L)
    {
        int         nResult     = 0;
        time_t      nTime       = 0;
        struct tm*  pFormatTime = NULL;

        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

        nTime = g_GatewayClient.GetLastLoginTime();

        pFormatTime = localtime(&nTime);
        KGLOG_PROCESS_ERROR(pFormatTime);

        Lua_PushNumber(L, pFormatTime->tm_year + 1900);
        Lua_PushNumber(L, pFormatTime->tm_mon + 1);
        Lua_PushNumber(L, pFormatTime->tm_mday);
        Lua_PushNumber(L, pFormatTime->tm_hour);
        Lua_PushNumber(L, pFormatTime->tm_min);
        Lua_PushNumber(L, pFormatTime->tm_sec);

        nResult = 6;
Exit0:
        return nResult;
    }

    int LuaGetLastLoginIP(Lua_State* L)
    {
        int             nResult = 0;
        DWORD           dwIP    = 0;
        char*           pszIP   = NULL;
        struct in_addr  InAddr;

        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);

        dwIP = g_GatewayClient.GetLastLoginIP();

        InAddr.s_addr = dwIP;
        
        pszIP = inet_ntoa(InAddr);
        KGLOG_PROCESS_ERROR(pszIP);

        Lua_PushString(L, pszIP);

        nResult = 1;
Exit0:
        return nResult;
    }

    int LuaGiveupQueue(Lua_State* L)
    {
        g_GatewayClient.DoGiveupQueueRequest();
        return 1;
    }

	int LuaGetRoleInfo(Lua_State* L)
	{
        int                 nResult         = 0;
		BOOL                bRetCode        = false;
		int                 nRoleIndex      = 0;
		UI_ROLE_LIST_ITEM   RoleListItem;

		bRetCode = lua_gettop(L);
		KGLOG_PROCESS_ERROR(bRetCode == 1);

		nRoleIndex = (int)lua_tonumber(L, 1);

		bRetCode = g_GatewayClient.GetRoleListItem(&RoleListItem, nRoleIndex);
		KGLOG_PROCESS_ERROR(bRetCode);
			
		lua_pushstring(L, RoleListItem.szAccountName);
		lua_pushstring(L, RoleListItem.szRoleName);
		lua_pushnumber(L, RoleListItem.nRoleType);
		lua_pushnumber(L, RoleListItem.nLevel);
		
		for (int nIndex = 0; nIndex < perRepresentCount; ++nIndex)
        {
			lua_pushnumber(L, RoleListItem.wRepresentID[nIndex]);
        }

		lua_pushnumber(L, RoleListItem.dwMapID);
        lua_pushnumber(L, 0);
		lua_pushnumber(L, RoleListItem.nLastSaveTime);
        lua_pushnumber(L, RoleListItem.nTotalGameTime);
        lua_pushstring(L, "");  // TODO: 角色当前门派名字

		nResult = 4 + perRepresentCount + 5;
Exit0:
		return nResult;
	}

	int LuaGetAccount(Lua_State* L)
	{
        int         nResult         = 0;
        const char* cszAccountName  = NULL; 

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
        
        cszAccountName = g_GatewayClient.GetAccountName();
        KGLOG_PROCESS_ERROR(cszAccountName);

		Lua_PushString(L, cszAccountName);

        nResult = 1;
Exit0:
		return nResult;
	}

	int LuaRoleLogin(Lua_State* L)
	{
        const char* cszRoleName = NULL;

        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 1);

		cszRoleName = (const char*)Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(cszRoleName);

		g_GatewayClient.RoleLogin(cszRoleName);
Exit0:
		return 0;
	}

	int LuaDeleteRole(Lua_State* L)
	{
        const char* cszRoleName          = NULL;
        const char* cszSecondlyPassword  = NULL;
  		char        szSecondlyPasswordMD5[KSG_PASSWORD_MAX_SIZE];

		KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 2);

		cszRoleName = (const char*)Lua_ValueToString(L, 1);
        KGLOG_PROCESS_ERROR(cszRoleName);

		cszSecondlyPassword = (const char*)Lua_ValueToString(L, 2);
        KGLOG_PROCESS_ERROR(cszSecondlyPassword);

		KG_EDStringToMD5String(szSecondlyPasswordMD5, cszSecondlyPassword);
        szSecondlyPasswordMD5[sizeof(szSecondlyPasswordMD5) - 1] = '\0';

		g_GatewayClient.DeleteRole(cszRoleName, szSecondlyPasswordMD5);
Exit0:
		return 0;
	}

	int LuaCreateRole(Lua_State* L)
	{
		BOOL                    bRetCode    = false;
		int                     nParamCount = 0;
		char const*             pszRoleName = NULL;
		UI_ROLE_CREATE_PARAM    RoleCreateParam;

		nParamCount = Lua_GetTopIndex(L);
		KGLOG_PROCESS_ERROR(nParamCount == 4 + perRepresentCount);

		pszRoleName = (char const*)Lua_ValueToString(L, 1);
		KGLOG_PROCESS_ERROR(pszRoleName && pszRoleName[0] != _T('\0'));
        
        ZeroMemory(&RoleCreateParam, sizeof(UI_ROLE_CREATE_PARAM));

		strncpy(RoleCreateParam.szRoleName, pszRoleName, sizeof(RoleCreateParam.szRoleName));
		RoleCreateParam.szRoleName[sizeof(RoleCreateParam.szRoleName) - 1] = '\0';       

		RoleCreateParam.nRoleType = (int)Lua_ValueToNumber(L, 2);
		KGLOG_PROCESS_ERROR(RoleCreateParam.nRoleType >= rtInvalid && RoleCreateParam.nRoleType < rtTotal);

		RoleCreateParam.dwMapID        = (DWORD)Lua_ValueToNumber(L, 3);
   		RoleCreateParam.nMapCopyIndex  = (DWORD)Lua_ValueToNumber(L, 4);

        RoleCreateParam.wRepresentId[perFaceStyle]         = (WORD)Lua_ValueToNumber(L, 5);    
        RoleCreateParam.wRepresentId[perHairStyle]         = (WORD)Lua_ValueToNumber(L, 6);
        RoleCreateParam.wRepresentId[perHelmStyle]         = (WORD)Lua_ValueToNumber(L, 7);
        RoleCreateParam.wRepresentId[perHelmColor]         = (WORD)Lua_ValueToNumber(L, 8);
        RoleCreateParam.wRepresentId[perHelmEnchant]       = (WORD)Lua_ValueToNumber(L, 9);   
        RoleCreateParam.wRepresentId[perChestStyle]        = (WORD)Lua_ValueToNumber(L, 10);
        RoleCreateParam.wRepresentId[perChestColor]        = (WORD)Lua_ValueToNumber(L, 11);
        RoleCreateParam.wRepresentId[perChestEnchant]      = (WORD)Lua_ValueToNumber(L, 12);
        RoleCreateParam.wRepresentId[perWaistStyle]        = (WORD)Lua_ValueToNumber(L, 13);
        RoleCreateParam.wRepresentId[perWaistColor]        = (WORD)Lua_ValueToNumber(L, 14);
        RoleCreateParam.wRepresentId[perWaistEnchant]      = (WORD)Lua_ValueToNumber(L, 15);
        RoleCreateParam.wRepresentId[perBangleStyle]       = (WORD)Lua_ValueToNumber(L, 16);
        RoleCreateParam.wRepresentId[perBangleColor]       = (WORD)Lua_ValueToNumber(L, 17);
        RoleCreateParam.wRepresentId[perBangleEnchant]     = (WORD)Lua_ValueToNumber(L, 18);
        RoleCreateParam.wRepresentId[perBootsStyle]        = (WORD)Lua_ValueToNumber(L, 19);
        RoleCreateParam.wRepresentId[perBootsColor]        = (WORD)Lua_ValueToNumber(L, 20);
        RoleCreateParam.wRepresentId[perWeaponStyle]       = (WORD)Lua_ValueToNumber(L, 21);
        RoleCreateParam.wRepresentId[perWeaponEnchant1]    = (WORD)Lua_ValueToNumber(L, 22);
        RoleCreateParam.wRepresentId[perWeaponEnchant2]    = (WORD)Lua_ValueToNumber(L, 23);
        RoleCreateParam.wRepresentId[perBackExtend]        = (WORD)Lua_ValueToNumber(L, 24);
        RoleCreateParam.wRepresentId[perWaistExtend]       = (WORD)Lua_ValueToNumber(L, 25);

		g_GatewayClient.CreateRole(RoleCreateParam);
Exit0:
		return 0;
	}

    int LuaCancelLogin(Lua_State* L)
    {
        KGLOG_PROCESS_ERROR(Lua_GetTopIndex(L) == 0);
        g_GatewayClient.Reset();
Exit0:
        return 0;
    }

    int LuaQueryHometownList(Lua_State* L)
    {
        g_GatewayClient.QueryHometownList();
        return 0;
    }

    int LuaGetHometownList(Lua_State* L)
    {
        const KUI_HOMETOWN_MAP_TABLE&   HometownList        = g_GatewayClient.GetHometownList();
        int                             nHometownMapCount   = (int)HometownList.size();

		Lua_NewTable(L);

		for (int nMap = 0; nMap < nHometownMapCount; ++nMap)
		{
            int nCopyCount = (int)HometownList[nMap].LoadFactorTable.size();
			
			Lua_PushNumber(L, nMap);
			Lua_NewTable(L);

			lua_pushliteral(L, "MapID");
			Lua_PushNumber(L, HometownList[nMap].dwMapID);
			Lua_SetTable(L, -3);

			lua_pushliteral(L, "Copy");
			Lua_NewTable(L);

            for (int nCopy = 0; nCopy < nCopyCount; ++nCopy)
            {
				Lua_PushNumber(L, nCopy + 1);
				Lua_NewTable(L);

				lua_pushliteral(L, "CopyIndex");
			    Lua_PushNumber(L, HometownList[nMap].LoadFactorTable[nCopy].dwCopyIndex);
			    Lua_SetTable(L, -3);

				lua_pushliteral(L, "LoadFactor");
				Lua_PushNumber(L, HometownList[nMap].LoadFactorTable[nCopy].nLoadFactor);
			    Lua_SetTable(L, -3);

				Lua_SetTable(L, -3); // nCopyCount
			}

			Lua_SetTable(L, -3); // Copy
			Lua_SetTable(L, -3); // nIndex
		}

        return 1;
    }

	int LuaReLogin(Lua_State *L)
	{
        g_GatewayClient.ReLogin();
		return 0;
	}

    static KLuaFunc g_LoginFuncList[] = 
    {
	    { "Login_SetGatewayAddress",					LuaSetGatewayAddress },
	    { "Login_BeginLogin",						    LuaBeginLogin },
	    { "Login_ReLogin",								LuaReLogin },
	    { "Login_GetRoleCount",							LuaGetRoleCount },
        { "Login_GetLeftTime",                          LuaGetLeftTime },
        { "Login_GetLastLoginTime",                     LuaGetLastLoginTime},
        { "Login_GetLastLoginIP",                       LuaGetLastLoginIP},
        { "Login_GiveupQueue",                          LuaGiveupQueue },
	    { "Login_GetRoleInfo",							LuaGetRoleInfo },
	    { "Login_GetAccount",							LuaGetAccount },
	    { "Login_RoleLogin",							LuaRoleLogin },
	    { "Login_DeleteRole",							LuaDeleteRole },
	    { "Login_CreateRole",							LuaCreateRole },
	    { "Login_CancelLogin",							LuaCancelLogin },
        { "Login_QueryHometownList",					LuaQueryHometownList },
	    { "Login_GetHometownList",						LuaGetHometownList },
        {0,0}
    };

    int GetLoginFuncList(KLuaFunc*& pFuncList)
    {
        pFuncList = g_LoginFuncList;
        return sizeof(g_LoginFuncList) / sizeof(g_LoginFuncList[0]);
    }
}
#endif
