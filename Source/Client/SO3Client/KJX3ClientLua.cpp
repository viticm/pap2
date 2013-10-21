#include "stdafx.h"
#include "./KJX3ClientLua.h"
#include "./KJX3Sdoa.h"
#include "./SO3Client.h"
#include "./KJX3CodePage.h"
#include "SO3UI/ISO3UI.h"
#include "SO3UI/IKGUI.h"
#include "IKG3DSoundShell.h"

static int LuaSdoaLogin(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;

    KG_ASSERT_EXIT(L);

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = JX3SdoaLogin(&g_SO3Client.m_JX3Sdoa);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return 0;
}

static int LuaSdoaLogout(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;

    KG_ASSERT_EXIT(L);

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = JX3SdoaLogout(&g_SO3Client.m_JX3Sdoa);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return 0;
}

static int RoleTypeToSex(int nRoleType, int* pnSex)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pnSex);

    switch (nRoleType)
    {
    case rtInvalid:
        KG_ASSERT_EXIT(0);
        break;
    case rtStandardMale:
    case rtStrongMale:
    case rtLittleBoy:
        *pnSex = 1;
        break;
    case rtStandardFemale:
    case rtSexyFemale:
    case rtLittleGirl:
        *pnSex = 0;
        break;
    default:
        KG_ASSERT_EXIT(0);
        break;
    }

    nResult = true;
Exit0:
    return nResult;
}

static int LuaSdoaSetRoleInfo(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    RoleInfo roleinfo;
    int nRoleType = 0;
    int nSex = 0;
    unsigned int uCodePage = 0;
    LPCSTR szRoleName = NULL;

    KG_ASSERT_EXIT(L);

    uCodePage = GetCodePage();

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    szRoleName = lua_tostring(L, 1);
    KGLOG_PROCESS_ERROR(szRoleName);
    
    nRoleType = (int)lua_tonumber(L, 2);

    nRetCode = RoleTypeToSex(nRoleType, &nSex);
    KGLOG_PROCESS_ERROR(nRetCode);

    roleinfo.cbSize = sizeof(RoleInfo);
    roleinfo.pwcsRoleName = ATL::CA2W(szRoleName, uCodePage);
    roleinfo.nSex = nSex;

    nRetCode = SetJX3SodaRoleInfo(&g_SO3Client.m_JX3Sdoa, &roleinfo);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return 0;
}

static int LuaSdoaGetWinExtent(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nResultCount = 0;
    LPCSTR pcszWinName = NULL;
    unsigned int uCodePage = 0;
    SDOAWinProperty Property = { 0 };

    KG_ASSERT_EXIT(L);

    uCodePage = GetCodePage();

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pcszWinName = lua_tostring(L, 1);
    KGLOG_PROCESS_ERROR(pcszWinName);

    nRetCode = GetJX3SdoaWinProperty(&g_SO3Client.m_JX3Sdoa, ATL::CA2W(pcszWinName, uCodePage), &Property);
    KGLOG_PROCESS_ERROR(nRetCode);

    lua_newtable(L);
    ++nResultCount;

    lua_pushnumber(L, Property.nLeft);
    lua_setfield(L, -2, "left");

    lua_pushnumber(L, Property.nTop);
    lua_setfield(L, -2, "top");

    lua_pushnumber(L, Property.nWidth);
    lua_setfield(L, -2, "width");

    lua_pushnumber(L, Property.nHeight);
    lua_setfield(L, -2, "height");

    nResult = true;
Exit0:
    return nResultCount;
}

static int LuaSdoaSetWinExtent(lua_State* L)
{
    int nRetCode = false;
    LPCSTR pcszWinName = NULL;
    unsigned int uCodePage = 0;
    SDOAWinProperty Property = { 0 };

    KG_ASSERT_EXIT(L);

    KGLOG_PROCESS_ERROR(g_SO3Client.m_pUI);

    uCodePage = GetCodePage();

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    pcszWinName = lua_tostring(L, 1);
    KGLOG_PROCESS_ERROR(pcszWinName);

    nRetCode = GetJX3SdoaWinProperty(&g_SO3Client.m_JX3Sdoa, ATL::CA2W(pcszWinName, uCodePage), &Property);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = lua_istable(L, 2);
    KGLOG_PROCESS_ERROR(nRetCode);

    lua_getfield(L, -1, "left");
    if (lua_isnumber(L, -1))
        Property.nLeft = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "top");
    if (lua_isnumber(L, -1))
        Property.nTop = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "width");
    if (lua_isnumber(L, -1))
        Property.nWidth = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "height");
    if (lua_isnumber(L, -1))
        Property.nHeight = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    nRetCode = SetJX3SdoaWinProperty(&g_SO3Client.m_JX3Sdoa, L"igwUserLoginDialog", &Property);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

static int LuaSdoaWinExists(lua_State* L)
{
    int nRetCode = false;
    BOOL bExists = FALSE;
    LPCSTR pcszWinName = NULL;
    unsigned int uCodePage = 0;

    KG_ASSERT_EXIT(L);

    uCodePage = GetCodePage();

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    pcszWinName = lua_tostring(L, 1);
    KGLOG_PROCESS_ERROR(pcszWinName);

    nRetCode = JX3SdoaWinExists(&g_SO3Client.m_JX3Sdoa, ATL::CA2W(pcszWinName, uCodePage), &bExists);
    KGLOG_PROCESS_ERROR(nRetCode);

    lua_pushboolean(L, bExists);
Exit0:
    return 1;
}

static int GetSdoaScreenStatus(LPCSTR szStatus, int* pnStatus)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(szStatus);
    KGLOG_PROCESS_ERROR(pnStatus);

    if (strcmp(szStatus, "none") == 0)
    {
        *pnStatus = SDOA_SCREEN_NONE;
    }
    else if (strcmp(szStatus, "normal") == 0)
    {
        *pnStatus = SDOA_SCREEN_NORMAL;
    }
    else if (strcmp(szStatus, "mini") == 0)
    {
        *pnStatus = SDOA_SCREEN_MINI;
    }
    else
    {
        ASSERT(0);
        *pnStatus = SDOA_SCREEN_NONE;
    }

    nResult = true;
Exit0:
    return nResult;
}

static int LuaSdoaSetScreenStatus(lua_State* L)
{
    int nRetCode = false;
    BOOL bExists = FALSE;
    LPCSTR szStatus = NULL;
    int nStatus = 0;

    KG_ASSERT_EXIT(L);

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    szStatus = lua_tostring(L, 1);
    KGLOG_PROCESS_ERROR(szStatus);

    nRetCode = GetSdoaScreenStatus(szStatus, &nStatus);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = SetJX3SdoaScreenStatus(&g_SO3Client.m_JX3Sdoa, nStatus);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return 0;
}

static int LuaGetVersion(Lua_State* L)
{
    lua_pushstring(L, g_SO3Client.m_Version.szVersionLineFullName);
    lua_pushstring(L, g_SO3Client.m_Version.szVersion);
    lua_pushstring(L, g_SO3Client.m_Version.szVersionLineName);
    lua_pushstring(L, g_SO3Client.m_Version.szVersionEx);
    return 4;
}

static int LuaGetAddonVersion(Lua_State* L)
{
    lua_pushstring(L, g_SO3Client.m_Version.szAddonVersion);
    return 1;
}

static int LuaGetServerListUrl(Lua_State* L)
{
    lua_pushstring(L, g_SO3Client.m_Version.szServerListUrl);
    return 1;
}

static int LuaSetBgMusicLoop(Lua_State* L)
{
    int nRetCode = false;
    BOOL bLoop = TRUE;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    nRetCode = lua_gettop(L) > 0;
    KGLOG_PROCESS_ERROR(nRetCode);

    if (lua_isboolean(L, 1))
        bLoop = (BOOL)lua_toboolean(L, 1);
    else
        bLoop = (BOOL)lua_tonumber(L, 1);

    pSoundShell->SetBgMusicLoop(bLoop);

Exit0:
    return 0;
}

static int LuaIsBgMusicLoop(Lua_State* L)
{
    BOOL bLoop = TRUE;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    bLoop = pSoundShell->IsBgMusicLoop();

Exit0:
    lua_pushboolean(L, bLoop);
    return 1;
}

static int LuaSetMainVolume(Lua_State* L)
{
    int nRetCode = false;
    float fVolume = 1.0f;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    fVolume = (float)lua_tonumber(L, 1);

    pSoundShell->SetVolume(fVolume);

    if (g_SO3Client.m_JX3Sdoa.pSDOAAppUtils)
        g_SO3Client.m_JX3Sdoa.pSDOAAppUtils->NodifyAudioChanged();

Exit0:
    return 0;
}

static int LuaGetMainVolume(Lua_State* L)
{
    float fVolume = 1.0f;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    fVolume = pSoundShell->GetVolume();
Exit0:
    lua_pushnumber(L, fVolume);
    return 1;
}

static int LuaSetVolume(Lua_State* L)
{
    int nRetCode = false;
    float fVolume = 1.0f;
    int nType = 0;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    nType = (int)lua_tonumber(L, 1);
    fVolume = (float)lua_tonumber(L, 2);

    pSoundShell->SetVolume(nType, fVolume);

    if (g_SO3Client.m_JX3Sdoa.pSDOAAppUtils)
        g_SO3Client.m_JX3Sdoa.pSDOAAppUtils->NodifyAudioChanged();

Exit0:
    return 0;
}

static int LuaGetVolume(Lua_State* L)
{
    int nRetCode = false;
    int nType = 0;
    float fVolume = 1.0f;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nType = (int)lua_tonumber(L, 1);

    fVolume = pSoundShell->GetVolume(nType);

Exit0:
    lua_pushnumber(L, fVolume);
    return 1;
}

static int LuaEnableAllSound(Lua_State* L)
{
    int nRetCode = false;
    BOOL bEnable = TRUE;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    if (lua_isboolean(L, 1))
        bEnable = (BOOL)lua_toboolean(L, 1);
    else
        bEnable = (BOOL)lua_tonumber(L, 1);

    if (!bEnable)
    {
        g_SO3Client.UnInitSoundShell();
    }
    else
    {
        pSoundShell = g_SO3Client.m_pSoundShell;
        if (pSoundShell == NULL)
        {
            g_SO3Client.InitSoundShell();

            pSoundShell = g_SO3Client.m_pSoundShell;
        }

        KG_PROCESS_ERROR(pSoundShell);

        pSoundShell->Enable(bEnable);
    }

Exit0:
    return 0;
}

static int LuaIsEnableAllSound(Lua_State* L)
{
    int nRetCode = false;
    BOOL bEnable = TRUE;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    bEnable = pSoundShell->IsEnable();

Exit0:
    lua_pushboolean(L, bEnable);
    return 1;
}

static int LuaEnableSound(Lua_State* L)
{
    int nRetCode = false;
    int nType = 0;
    BOOL bEnable = TRUE;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 2);

    nType = (int)lua_tonumber(L, 1);
    if (lua_isboolean(L, 2))
        bEnable = (BOOL)lua_toboolean(L, 2);
    else
        bEnable = (BOOL)lua_tonumber(L, 2);

    pSoundShell->Enable(nType, bEnable);

Exit0:
    return 0;
}

static int LuaIsEnableSound(Lua_State* L)
{
    int nRetCode = false;
    int nType = 0;
    BOOL bEnable = TRUE;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    nType = (int)lua_tonumber(L, 1);

    bEnable = pSoundShell->IsEnable(nType);

Exit0:
    lua_pushboolean(L, bEnable);
    return 1;
}

static int LuaEnable3DSound(Lua_State* L)
{
    int nRetCode = false;
    BOOL bEnable = TRUE;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    nRetCode = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nRetCode == 1);

    if (lua_isboolean(L, 1))
        bEnable = (BOOL)lua_toboolean(L, 1);
    else
        bEnable = (BOOL)lua_tonumber(L, 1);

    pSoundShell->Enable3DSound(bEnable);

Exit0:
    return 0;
}

static int LuaIsEnable3DSound(Lua_State* L)
{
    BOOL bEnable = TRUE;
    IKG3DSoundShell* pSoundShell = NULL;

    ASSERT(L);

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    bEnable = pSoundShell->IsEnable3DSound();

Exit0:
    lua_pushboolean(L, bEnable);
    return 1;
}


luaL_reg const* GetJX3SdoaFuncList()
{
    static luaL_reg s_Funcs[] =
    {
        { "SdoaLogin", LuaSdoaLogin },
        { "SdoaLogout", LuaSdoaLogout },
        { "SdoaSetRoleInfo", LuaSdoaSetRoleInfo },
        { "SdoaGetWinExtent", LuaSdoaGetWinExtent },
        { "SdoaSetWinExtent", LuaSdoaSetWinExtent },
        { "SdoaWinExists", LuaSdoaWinExists },
        { "SdoaSetScreenStatus", LuaSdoaSetScreenStatus },

        { "GetVersion", LuaGetVersion },
        { "GetAddOnVersion", LuaGetAddonVersion },
        { "GetAddonVersion", LuaGetAddonVersion },
        { "GetServerListUrl", LuaGetServerListUrl },

        { "SetBgMusicLoop", LuaSetBgMusicLoop },
        { "IsBgMusicLoop", LuaIsBgMusicLoop },
        { "SetTotalVolume", LuaSetMainVolume },
        { "GetTotalVolume",	LuaGetMainVolume },
        { "SetMainVolume", LuaSetMainVolume },
        { "GetMainVolume",	LuaGetMainVolume },
        { "SetVolume", LuaSetVolume },
        { "GetVolume", LuaGetVolume },
        { "EnableAllSound", LuaEnableAllSound },
        { "IsEnableAllSound", LuaIsEnableAllSound },
        { "EnableSound", LuaEnableSound },
        { "IsEnableSound", LuaIsEnableSound },
        { "Enable3DSound", LuaEnable3DSound },
        { "IsEnable3DSound", LuaIsEnable3DSound },

        { NULL, NULL },
    };

    return s_Funcs;
}
