#include "stdafx.h"
#include "./kscriptmgr.h"
#include "./kscriptloader.h"
#include "./kconstmgr.h"
#include "./kwndclassscripttable.h"
#include "./kitemhandleclassscripttable.h"
#include "./kwndclassscripttable.h"
#include "../elem/wndwindow.h"
#include "../elem/wndframe.h"
#include "../elem/Components/item.h"
#include "../KGUI.h"
#include "SO3World/KScriptFuncList.h"

#define ENABLE_CHECK_LUA_STACK 1

namespace 
{
	class KLuaThisGuard
	{
	public:
		KLuaThisGuard(lua_State *L)
		{
			ASSERT(L);
			m_L = L;
			lua_getglobal (L, "this");
			m_nOldRef = luaL_ref (L, LUA_REGISTRYINDEX);
		}

		KLuaThisGuard(lua_State *L, UI::KItemNull *pItem)
		{
			ASSERT(L);
			m_L = L;
			lua_getglobal (L, "this");
			m_nOldRef = luaL_ref (L, LUA_REGISTRYINDEX);

			lua_pushstring(L, "this");
			if (!UI::KItemHandleClassScriptTable::HostWndItem_Lookup(L, pItem))
				lua_pushnil(L);
			lua_settable(L, LUA_GLOBALSINDEX);
		}

		KLuaThisGuard(lua_State *L, UI::KWndWindow *pWnd)
		{
			ASSERT(L);
			m_L = L;
			lua_getglobal (L, "this");
			m_nOldRef = luaL_ref (L, LUA_REGISTRYINDEX);

			lua_pushstring(L, "this");
			if (!UI::KWndClassScriptTable::HostStation_Lookup(L, pWnd))
				lua_pushnil(L);
			lua_settable(L, LUA_GLOBALSINDEX);
		}

		virtual ~KLuaThisGuard()
		{
			ASSERT(m_L);
			lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_nOldRef);
			lua_setglobal (m_L, "this");
			luaL_unref (m_L, LUA_REGISTRYINDEX, m_nOldRef);
		}

	private:
		int m_nOldRef;
		lua_State *m_L;
	};


#if ENABLE_CHECK_LUA_STACK 
	class KCheckLuaStack
	{
	public:
		KCheckLuaStack(Lua_State* L, int nResult = 0)
		{
			m_L = L;
			m_nResult = nResult;
			m_nTop = lua_gettop(L);
		}


		~KCheckLuaStack()
		{
			int nTop = lua_gettop(m_L);
			ASSERT(nTop == m_nTop + m_nResult);
		}


	private:
		Lua_State*	m_L;
		int			m_nTop;
		int			m_nResult;
	};
#endif // ENABLE_CHECK_LUA_STACK
}
//--------------------------------------------------------------------------------------------------------------


namespace UI
{

    KScriptMgr::KScriptMgr()
        : m_LuaState(NULL)
        , m_dwTickCount(0)
    {
		m_szError[0] = '\0';
	}

	KScriptMgr::~KScriptMgr()
	{
	}

	bool KScriptMgr::ReInit()
	{
		Exit();
		return Init();
	}

	bool KScriptMgr::Init()
	{
		BOOL bRetCode = FALSE;

		m_LuaState = lua_open();
		KGLOG_PROCESS_ERROR(m_LuaState);

		lua_pushstring(m_LuaState, LUA_SCRIPT_THIS);
		lua_pushlightuserdata(m_LuaState, this);
		lua_settable(m_LuaState, LUA_GLOBALSINDEX);

		luaL_openlibs(m_LuaState);

		lua_settop(m_LuaState, 0);

		CreateEnv();

        bRetCode = KConstMgr::Load(m_LuaState);
		KGLOG_PROCESS_ERROR(bRetCode);

		return true;
Exit0:
		return false;
	}


	void KScriptMgr::Exit()
	{
		if (m_LuaState)
		{
			lua_close(m_LuaState);
			m_LuaState = NULL;
		}
		m_Events.clear();
	}

    void KScriptMgr::Activate()
    {
#if 0
        static DWORD const LUA_GC_COLLECT_TICK_COUNT = 16;

        KGLOG_PROCESS_ERROR(m_LuaState);

        if (m_dwTickCount++ == LUA_GC_COLLECT_TICK_COUNT)
        {
            m_dwTickCount = 0;
            lua_gc(m_LuaState, LUA_GCCOLLECT, 0);
        }

Exit0:
        return;
#endif
    }

	bool KScriptMgr::LoadScriptFile(LPCSTR szFilePath, LPCSTR pcwszEnv)
	{
#if ENABLE_CHECK_LUA_STACK 
		KCheckLuaStack c(GetLuaState(), 0);
#endif // ENABLE_CHECK_LUA_STACK 

		BOOL bRetCode = FALSE;
		lua_State* L = m_LuaState;
        int nTopIndex = 0;

		ASSERT(L);

		nTopIndex = lua_gettop(L);

		KGLOG_PROCESS_ERROR(szFilePath && szFilePath[0] != '\0');

		bRetCode = KScriptLoader::LoadFile(L, szFilePath, pcwszEnv);
		KGLOG_PROCESS_ERROR(bRetCode);

		KGLOG_PROCESS_ERROR(nTopIndex == lua_gettop(L));

		return true;
Exit0:
		lua_settop(L, nTopIndex);
		KGLogPrintf(KGLOG_ERR, "KGUI KScriptMgr::LoadScriptFile(%s) failed\n", szFilePath);
		return false;
	}


	bool KScriptMgr::ExecuteScriptString(LPCSTR szScriptString, LPCSTR pcwszEnv)
	{
		ASSERT(szScriptString);

		Lua_State * L = m_LuaState;

		ASSERT(L);

		int nTopIndex = lua_gettop(L);

		if (luaL_loadstring(L, szScriptString) == 0)
		{
			if (pcwszEnv)
			{
				int nTop = lua_gettop(L);
				lua_pushstring(L, "_G");
				lua_rawget(L, LUA_GLOBALSINDEX);
				lua_pushstring(L, pcwszEnv);
				lua_rawget(L, -2);
				if (lua_istable(L, -1))
					lua_setfenv(L, -3);
				lua_settop(L, nTop);
			}
			if (lua_pcall(L, 0, 0, 0) != 0)
			{
				_snprintf(m_szError, _countof(m_szError), "KGUI execute string failed [%s]\n", lua_tostring(L, -1));
				KGLogPrintf(KGLOG_ERR, m_szError);
				FireEvent("CALL_LUA_ERROR", 0);
			}
		}
		else
		{
			_snprintf(m_szError, _countof(m_szError), "KGUI load string[%s] failed [%s]\n", szScriptString, lua_tostring(L, -1));
			KGLogPrintf(KGLOG_ERR, m_szError);
			FireEvent("CALL_LUA_ERROR", 0);
		}

		lua_settop(L, nTopIndex);
		return true;
	}

	bool KScriptMgr::ExecuteScript(int nRef)
	{
		ASSERT(m_LuaState);
		bool bResult = FALSE;
		int nTop = lua_gettop(m_LuaState);

		KG_PROCESS_ERROR(nRef != LUA_NOREF);
		lua_rawgeti(m_LuaState, LUA_REGISTRYINDEX, nRef);
		KG_PROCESS_ERROR(lua_isfunction(m_LuaState, -1));
		KG_PROCESS_ERROR(Call(0, 0));
Exit0:
		lua_settop(m_LuaState, nTop);
		return bResult;
	}


	lua_State* KScriptMgr::GetLuaState()
	{
		return m_LuaState;
	}


	void KScriptMgr::LogStack(LPCSTR szMsg)
	{
		lua_State* L = m_LuaState;

		ASSERT(L);

		int nTop = lua_gettop(L);

		KGLogPrintf(KGLOG_ERR, "KGUI KScriptMgr::LogStack(%s) %d\n", szMsg, nTop);
		for (int nIndex = 0; nIndex < nTop; ++nIndex)
		{
			KGLogPrintf(KGLOG_ERR, "\t%s\n", lua_tostring(L, nIndex + 1));
		}
	}

	bool KScriptMgr::LoadLoginScriptLib()
	{
		bool bResult = false;
		IIniFile *pIni = NULL;
		LPCSTR pcszIniFile = NULL;
        
        pcszIniFile = g_pUI->m_FilePathMgr.GetFilePath("LoginScripLib");
		KGLOG_PROCESS_ERROR(pcszIniFile);

#ifdef KG_PUBLISH
		pIni = g_OpenIniFileInPak(pcszIniFile);
		if (!pIni)
			pIni = g_OpenIniFile(pcszIniFile);
		KGLOG_PROCESS_ERROR(pIni);
#else
		pIni = g_OpenIniFile(pcszIniFile);
		KGLOG_PROCESS_ERROR(pIni);
#endif
		LoadScriptLib(pIni);

		bResult = true;
Exit0:
		KG_COM_RELEASE(pIni);
		return bResult;
	}

	bool KScriptMgr::LoadDefaultScriptLib()
	{
		bool bResult = false;
		IIniFile *pIni = NULL;
		LPCSTR pcszIniFile = NULL;
        
        pcszIniFile = g_pUI->m_FilePathMgr.GetFilePath("DefaultScripLib");
		KGLOG_PROCESS_ERROR(pcszIniFile);

#ifdef KG_PUBLISH
		pIni = g_OpenIniFileInPak(pcszIniFile);
		if (!pIni)
			pIni = g_OpenIniFile(pcszIniFile);
		KGLOG_PROCESS_ERROR(pIni);
#else
		pIni = g_OpenIniFile(pcszIniFile);
		KGLOG_PROCESS_ERROR(pIni);
#endif

		LoadScriptLib(pIni);

		bResult = true;
Exit0:
		KG_COM_RELEASE(pIni);
		return bResult;
	}

	bool KScriptMgr::LoadScriptLib(LPCSTR pcszIni)
	{
		bool bResult = false;
		IIniFile *pIni = NULL;
        
		KGLOG_PROCESS_ERROR(pcszIni);

		pIni = g_OpenIniFile(pcszIni);
		KGLOG_PROCESS_ERROR(pIni);

		LoadScriptLib(pIni);

		bResult = true;
Exit0:
		KG_COM_RELEASE(pIni);
		return bResult;
	}

	int KScriptMgr::RefBuffer(const char * pcszBuffer)
	{
		ASSERT(m_LuaState);
		int nResult = 0;
		int nRef = LUA_NOREF;
		int nTop = lua_gettop(m_LuaState);
		
		KG_PROCESS_ERROR(pcszBuffer);

		nResult = luaL_loadstring(m_LuaState, pcszBuffer);
		if (nResult)
		{
			_snprintf(m_szError, _countof(m_szError), "KGUI load string[%s] failed [%s]\n", pcszBuffer, lua_tostring(m_LuaState, -1));
			KGLogPrintf(KGLOG_ERR, m_szError);
			FireEvent("CALL_LUA_ERROR", 0);
		}
		KG_PROCESS_ERROR(!nResult);

		nRef = luaL_ref(m_LuaState, LUA_REGISTRYINDEX);
Exit0:
		lua_settop(m_LuaState, nTop);
		return nRef;
	}

	int KScriptMgr::UnRef(int nRef)
	{
		if (m_LuaState && nRef != LUA_NOREF)
			luaL_unref(m_LuaState, LUA_REGISTRYINDEX, nRef);
		return true;
	}

	int KScriptMgr::UnRefUIObject(int nRef)
	{
		lua_State* L = m_LuaState;
		if (L && nRef != LUA_NOREF)
		{
			int nTop = lua_gettop(L);
			lua_rawgeti(L, LUA_REGISTRYINDEX, nRef);
			lua_pushstring(L, STR_OBJ_KEY_NAME);
			lua_pushnil(L);
			lua_settable(L, nTop + 1);
			luaL_unref(L, LUA_REGISTRYINDEX, nRef);
			lua_settop(L, nTop);
		}
		return true;
	}

	bool KScriptMgr::LoadScriptLib(IIniFile *pIni)
	{
		bool bResult = false;
		char szSection[MAX_PATH];
		char szNextSection[MAX_PATH];
		char szPath[MAX_PATH];

		szSection[0] = '\0';
		szNextSection[0] = '\0';
		szPath[0] = '\0';

		KG_PROCESS_ERROR(pIni);

		while (pIni->GetNextSection(szSection, szNextSection))
		{
			strcpy(szSection, szNextSection);
			int nIndex = 0;
			while (true)
			{
				char szKey[32];

				ASSERT(nIndex < 4096);

				sprintf(szKey, "lua_%d", nIndex++);

				if (!pIni->GetString(szSection, szKey, "", szPath, _countof(szPath)))
					break;

				LoadScriptFile(szPath);
			}
		}

		bResult = true;
Exit0:
		return bResult;
	}

	int KScriptMgr::PrepareCall(KWndWindow* pSelf, LPCSTR szEvent)
	{
		ASSERT(szEvent);
		ASSERT(m_LuaState);
		ASSERT(pSelf);

		lua_State *L = m_LuaState;

		int nRef = pSelf->GetLuaRef();
		if (nRef != LUA_NOREF)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, nRef);
			if (lua_istable(L, -1))
			{
				lua_getfield(L, -1, szEvent);
				if (lua_isfunction(L, -1))
				{
					return true;
				}
			}
		}

		KWndWindow *pRoot = pSelf;
		while (pRoot->GetWndType() != "WndFrame")
        {
			pRoot = pRoot->GetParent();
            if (!pRoot)
			{
                return false;
			}
        }

		ASSERT(pRoot);
		KWndFrame* pFrame = (KWndFrame*)CastWndWindow(pRoot, "WndFrame");

		ASSERT(pFrame);
		nRef = pFrame->GetDefaultTableRef();
		if (nRef == LUA_NOREF)
			return false;

		lua_rawgeti(L, LUA_REGISTRYINDEX, nRef);
		if (lua_istable(L, -1))
		{
			lua_getfield(L, -1, szEvent);
			if (lua_isfunction(L, -1))
			{
				return true;
			}
		}
		return false;
	}

	int KScriptMgr::PrepareCall(KItemNull *pSelf, LPCSTR szEvent)
	{
		ASSERT(szEvent);
		ASSERT(m_LuaState);
		ASSERT(pSelf);

		lua_State *L = m_LuaState;

		int nRef = pSelf->GetLuaRef();
		if (nRef != LUA_NOREF)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, nRef);
			if (lua_istable(L, -1))
			{
				lua_getfield(L, -1, szEvent);
				if (lua_isfunction(L, -1))
					return true;
			}
		}

		KWndWindow *pRoot = pSelf->GetWndOwner();
		ASSERT(pRoot);
		while (pRoot->GetWndType() != "WndFrame")
		{
			pRoot = pRoot->GetParent();
			if (!pRoot)
				return false;
		}
		ASSERT(pRoot);
		KWndFrame* pFrame = (KWndFrame*)CastWndWindow(pRoot, "WndFrame");
		ASSERT(pFrame);
		nRef = pFrame->GetDefaultTableRef();
		if (nRef == LUA_NOREF)
			return false;

		lua_rawgeti(L, LUA_REGISTRYINDEX, nRef);
		if(lua_istable(L, -1))
		{
			lua_getfield(L, -1, szEvent);
			if (lua_isfunction(L, -1))
				return true;
		}
		return false;
	}

	int KScriptMgr::PrepareCall(LPCSTR szEvent)
	{
		ASSERT(szEvent);
		ASSERT(m_LuaState);

		Lua_GetGlobal(m_LuaState, szEvent);
		if (lua_isfunction(m_LuaState, -1))
			return true;
		return false;
	}

	int KScriptMgr::Call(int nParam, int nResult)
	{
		if (lua_pcall(m_LuaState, nParam, nResult, 0))
		{
			_snprintf(m_szError, _countof(m_szError), "KGUI Call function failed [%s]\n", lua_tostring(m_LuaState, -1));
			KGLogPrintf(KGLOG_ERR, m_szError);
			FireEvent("CALL_LUA_ERROR", 0);
			return false;
		}
		return true;
	}

	int KScriptMgr::FireWndEvent(KWndWindow *pWnd, LPCSTR szEvent, int *pnRet)
	{
		ASSERT(pWnd);
		ASSERT(szEvent);
		ASSERT(m_LuaState);

		int nResult = false;
		int nRetCode = false;
		int nTopIndex = lua_gettop(m_LuaState);
		if (pnRet)
			*pnRet = false;

		nRetCode = PrepareCall(pWnd, szEvent);
		KG_PROCESS_ERROR(nRetCode);

		{
			KLuaThisGuard thisGard(m_LuaState, pWnd);
			if (pnRet)
			{
				nRetCode = Call(0, 1);
				KG_PROCESS_ERROR(nRetCode);
				if (lua_isnil(m_LuaState, -1))
					*pnRet = false;
				else if (lua_isboolean(m_LuaState, -1))
					*pnRet = lua_toboolean(m_LuaState, -1);
				else
					*pnRet = (int)lua_tonumber(m_LuaState, -1);			
				goto Exit1;
			}
			nRetCode = Call(0, 0);
			KG_PROCESS_ERROR(nRetCode);
		}

Exit1:
		nResult = true;
Exit0:
		lua_settop(m_LuaState, nTopIndex);
		return nResult;
	}

	int KScriptMgr::FireWindowEvent(KWndWindow *pWnd, LPCSTR pcszEvent)
	{
		ASSERT(m_LuaState);
		ASSERT(pcszEvent);

		int nResult = false;
		int nRetCode = false;
		int nTopIndex = lua_gettop(m_LuaState);

		nRetCode = PrepareCall(pWnd, "OnEvent");
		KG_PROCESS_ERROR(nRetCode);

		{
			KLuaThisGuard thisGard(m_LuaState, pWnd);
			Lua_PushString(m_LuaState, pcszEvent);
			nRetCode = Call(1, 0);
			KG_PROCESS_ERROR(nRetCode);
		}

		nResult = true;
Exit0:
		lua_settop(m_LuaState, nTopIndex);
		return nResult;
	}

	int KScriptMgr::FireFunctionEvent(int nFunctionRef, LPCSTR pcszEvent)
	{
		ASSERT(m_LuaState);
		ASSERT(pcszEvent);

		int nResult = false;
		int nRetCode = false;
		int nTopIndex = lua_gettop(m_LuaState);

		lua_rawgeti(m_LuaState, LUA_REGISTRYINDEX, nFunctionRef);
		KG_PROCESS_ERROR(lua_isfunction(m_LuaState, -1));

		Lua_PushString(m_LuaState, pcszEvent);
		nRetCode = Call(1, 0);
		KG_PROCESS_ERROR(nRetCode);

		nResult = true;
Exit0:
		lua_settop(m_LuaState, nTopIndex);
		return nResult;
	}

	int KScriptMgr::FireItemEvent(KItemNull *pItem, LPCSTR szEvent, int *pnRet)
	{
		ASSERT(m_LuaState);

		int nResult = false;
		int nRetCode = false;
		int nTopIndex = lua_gettop(m_LuaState);
		if (pnRet)
			*pnRet = false;

		nRetCode = PrepareCall(pItem, szEvent);
		KG_PROCESS_ERROR(nRetCode);

		{
			KLuaThisGuard thisGard(m_LuaState, pItem);
			if (pnRet)
			{
				nRetCode = Call(0, 1);
				KG_PROCESS_ERROR(nRetCode);

				if (lua_isnil(m_LuaState, -1))
					*pnRet = false;
				else if (Lua_IsBoolean(m_LuaState, -1))
					*pnRet = Lua_ValueToBoolean(m_LuaState, -1);
				else
					*pnRet = (int)Lua_ValueToNumber(m_LuaState, -1);			
				goto Exit1;
			}
			nRetCode = Call(0, 0);
			KG_PROCESS_ERROR(nRetCode);
		}

Exit1:
		nResult = true;
Exit0:
		lua_settop(m_LuaState, nTopIndex);
		return nResult;
	}

	int KScriptMgr::OnItemInit(KItemNull *pItem, LPCSTR pszScript)
	{
		ASSERT(m_LuaState);
		ASSERT(pszScript);

		KLuaThisGuard thisGuard(m_LuaState, pItem);
		
		return ExecuteScriptString(pszScript, STR_ADD_ON_ENV);
	}

	int KScriptMgr::FireGlobalEvent(LPCSTR szEvent, int *pnRet)
	{
		ASSERT(m_LuaState);

		int nResult = false;
		int nRetCode = false;
		int nTopIndex = lua_gettop(m_LuaState);
		if (pnRet)
			*pnRet = false;

		nRetCode = PrepareCall(szEvent);
		KG_PROCESS_ERROR(nRetCode);

		if (pnRet)
		{
			nRetCode = Call(0, 1);
			KG_PROCESS_ERROR(nRetCode);
			if (lua_isnil(m_LuaState, -1))
				*pnRet = false;
			else if (Lua_IsBoolean(m_LuaState, -1))
				*pnRet = Lua_ValueToBoolean(m_LuaState, -1);
			else
				*pnRet = (int)Lua_ValueToNumber(m_LuaState, -1);			
			goto Exit1;
		}

		nRetCode = Call(0, 0);
		KG_PROCESS_ERROR(nRetCode);

Exit1:
		nResult = true;
Exit0:
		lua_settop(m_LuaState, nTopIndex);
		return nResult;
	}


	void KScriptMgr::UnregisterAllEvent()
	{
		m_Events.clear();
	}

	
	bool KScriptMgr::BindScript(KWndFrame* pFrame)
	{
		bool bResult = false;
		bool bRetCode = false;
		LPCSTR szName = NULL;
		int nTop = -1;

		KGLOG_PROCESS_ERROR(m_LuaState);
		KGLOG_PROCESS_ERROR(pFrame);

		nTop = lua_gettop(m_LuaState);

		szName = pFrame->GetName();
		ASSERT(szName);
		KG_PROCESS_ERROR(szName[0] != '\0');

		lua_getglobal(m_LuaState, szName);
		if (lua_istable(m_LuaState, -1))
		{
			pFrame->SetDefaultTableRef(luaL_ref(m_LuaState, LUA_REGISTRYINDEX));
		}
		else
		{
            LPCSTR pcszClass = NULL;

			pcszClass = pFrame->GetLuaClass();
			KG_PROCESS_ERROR(pcszClass[0] != '\0');

			lua_getglobal(m_LuaState, pcszClass);
			lua_getfield(m_LuaState, -1, "new");
			KG_PROCESS_ERROR(lua_isfunction(m_LuaState, -1));

			bRetCode = lua_pcall(m_LuaState, 0, 1, 0) == 0;
			KG_PROCESS_ERROR(bRetCode);
			KG_PROCESS_ERROR(lua_istable(m_LuaState, -1));

			pFrame->SetDefaultTableRef(luaL_ref(m_LuaState, LUA_REGISTRYINDEX));
			lua_rawgeti(m_LuaState, LUA_REGISTRYINDEX, pFrame->GetDefaultTableRef());
			lua_setglobal(m_LuaState, szName);
			pFrame->SetStatus(UI_WND_FRAME_DESTROY_LUA_TABLE, true);
		}

		bResult = true;
Exit0:
		if (m_LuaState)
			lua_settop(m_LuaState, nTop);
		return bResult;
	}


	bool KScriptMgr::UnbindScript(KWndFrame* pFrame, LPCSTR szName, int nRef, int nDestroy)
	{
        KGLOG_PROCESS_ERROR(m_LuaState);
		KGLOG_PROCESS_ERROR(pFrame);

		if (nRef != LUA_NOREF)
		{
			luaL_unref(m_LuaState, LUA_REGISTRYINDEX, nRef);
			if (nDestroy && szName && szName[0] != '\0')
			{
				lua_pushnil(m_LuaState);
				lua_setglobal(m_LuaState, szName);
			}
		}

		return true;
Exit0:
		return false;
	}


	void KScriptMgr::SubscribeEvent(KWndFrame* pFrame, LPCSTR szEvent, DWORD dwCookie)
	{
		ASSERT(pFrame);
		ASSERT(szEvent);

		EVENT_INFO Info;
		Info.pFrame = pFrame;
		Info.nFunctionRef = LUA_NOREF;
		Info.dwCookie = dwCookie;

		m_Events.insert(std::make_pair(g_StringHash(szEvent), Info));
	}


	void KScriptMgr::UnsubscribeEvent(KWndFrame* pFrame)
	{
		// TODO: optimize
		assert(pFrame);

		KEventMap::iterator it = m_Events.begin();
		KEventMap::iterator ie = m_Events.end();

		while (it != ie)
		{
			if (it->second.pFrame == pFrame)
			{
				it = m_Events.erase(it);
				continue;
			}
			++it;
		}
	}

	void KScriptMgr::SubscribeEvent(int nFunctionRef, LPCSTR szEvent, DWORD dwCookie)
	{
		ASSERT(nFunctionRef != LUA_NOREF);
		ASSERT(szEvent);

		EVENT_INFO Info;
		Info.pFrame = NULL;
		Info.nFunctionRef = nFunctionRef;
		Info.dwCookie = dwCookie;

		m_Events.insert(std::make_pair(g_StringHash(szEvent), Info));

	}

	void KScriptMgr::UnsubscribeEvent(int nFunctionRef)
	{
		assert(m_LuaState);
		assert(nFunctionRef != LUA_NOREF);

		lua_rawgeti(m_LuaState, LUA_REGISTRYINDEX, nFunctionRef);

		KEventMap::iterator it = m_Events.begin();
		KEventMap::iterator ie = m_Events.end();

		while (it != ie)
		{
			if (it->second.nFunctionRef != LUA_NOREF)
			{
				lua_rawgeti(m_LuaState, LUA_REGISTRYINDEX, it->second.nFunctionRef);
				if (lua_equal(m_LuaState, -1, -2))
				{
					lua_pop(m_LuaState, 1);
					luaL_unref(m_LuaState, LUA_REGISTRYINDEX, it->second.nFunctionRef);
					it = m_Events.erase(it);
					continue;
				}
				lua_pop(m_LuaState, 1);
			}
			++it;
		}
		lua_pop(m_LuaState, 1);
	}


	void KScriptMgr::FireEvent(LPCSTR szEvent, DWORD dwCookie)
	{
		// NOTE: 兼顾性能跟简单性，事件名字使用hash，会有多个不同名事件触发相同事件的可能，所以要求脚本OnEvent要检查event是否为所要求事件。

		ASSERT(szEvent);

		DWORD dwEvent = g_StringHash(szEvent);

		KEventMap::iterator it = m_Events.lower_bound(dwEvent);
		KEventMap::iterator ie = m_Events.end();
		while (it != ie && it->first == dwEvent)
		{
			const EVENT_INFO crInfo = it->second;
			if (crInfo.pFrame)
			{
				ASSERT(crInfo.nFunctionRef == LUA_NOREF);
				if (dwCookie == 0 || crInfo.dwCookie == 0 || dwCookie == crInfo.dwCookie)
				{
					FireWindowEvent(crInfo.pFrame, szEvent);
				}
			}
			else
			{
				ASSERT(crInfo.nFunctionRef != LUA_NOREF);
				if (dwCookie == 0 || crInfo.dwCookie == 0 || dwCookie == crInfo.dwCookie)
				{
					FireFunctionEvent(crInfo.nFunctionRef, szEvent);
				}
			}
			++it;
		}
	}

	void KScriptMgr::ClearShieldTable()
	{
		Lua_State * L = m_LuaState;
		if (L)
		{
			lua_newtable(L);
			lua_setglobal(L, "_g_ShildOnceKey");
		}
	}

	int KScriptMgr::CreateEnv()
	{
		Lua_State * L = m_LuaState;
		KGLOG_PROCESS_ERROR(L);

		lua_pushstring(L, "_G");
		lua_rawget(L, LUA_GLOBALSINDEX);
		//将engine的metatable，指向AddOn的table
		lua_newtable(L);

		lua_pushstring(L, "__index");
		lua_pushcfunction(L, IndexOfEngine);
		lua_settable(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcfunction(L, NewIndexOfEngine);
		lua_settable(L, -3);

		lua_setmetatable(L, -2);

		lua_pushstring(L, STR_ADD_ON_ENV);
		lua_newtable(L);

		//将_AddOn环境的_G设置为环境本身
		
		lua_pushstring(L, "_G");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);

		//将addon的metatable，指向engine的table
		lua_newtable(L);

		lua_pushstring(L, "__index");
		lua_pushcfunction(L, IndexOfAddOn);
		lua_settable(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcfunction(L, NewIndexOfAddOn);
		lua_settable(L, -3);

		lua_setmetatable(L, -2);

		lua_rawset(L, -3);
		lua_pop(L, 1);

		return true;
Exit0:
		return false;
	}

	int KScriptMgr::IndexOfEngine(lua_State* L)
	{
		lua_pushstring(L, STR_ADD_ON_ENV);
		lua_rawget(L, LUA_GLOBALSINDEX);

		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		lua_remove(L, -2);
		return 1;
	}

	int KScriptMgr::NewIndexOfEngine(lua_State* L)
	{
		lua_pushstring(L, STR_ADD_ON_ENV);
		lua_rawget(L, LUA_GLOBALSINDEX);

		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		if (lua_isnil(L, -1))
		{
			lua_settop(L, 3);
			lua_rawset(L, -3);
			lua_settop(L, 3);
			return 0;
		}

		lua_pop(L, 1);
		lua_pushvalue(L, 2);
		lua_pushvalue(L, 3);
		lua_rawset(L, -3);
		lua_settop(L, 3);
		return 0;
	}

	int KScriptMgr::IndexOfAddOn(lua_State* L)
	{
		lua_pushstring(L, "_G");
		lua_rawget(L, LUA_GLOBALSINDEX);

		lua_pushstring(L, "_ShieldList");
		lua_rawget(L, -2);

		if (!(lua_istable(L, -1)))
		{
			lua_pop(L, 1);

			lua_pushvalue(L, 2);
			lua_rawget(L, -2);

			lua_remove(L, -2);
			return 1;
		}

		lua_pushvalue(L, 2);
		lua_gettable(L, -2);

		if (lua_isnil(L, -1))
		{
			lua_pop(L, 2);

			lua_pushvalue(L, 2);
			lua_rawget(L, -2);

			lua_remove(L, -2);
			return 1;
		}

		if (lua_isfunction(L, -1))
		{
			lua_pushvalue(L, 3);
			if (lua_pcall(L, 1, 1, 0) != 0)
			{
				KGLogPrintf(KGLOG_ERR, "KGUI KScriptMgr::IndexOfAddOn Call function error: %s\n", lua_tostring(L, -1));
				lua_pop(L, 3);
				lua_pushnil(L);
				return 1;
			}
			lua_remove(L, -2);
			lua_remove(L, -2);
			return 1;
		}

		KGLogPrintf(KGLOG_ERR, "AddOn Index a Forbidden Variable:%s\n", lua_tostring(L, 2));

		lua_pop(L, 3);
		lua_pushnil(L);
		return 1;
	}

	int KScriptMgr::NewIndexOfAddOn(lua_State* L)
	{
		lua_pushstring(L, "_G");
		lua_rawget(L, LUA_GLOBALSINDEX);

		lua_pushstring(L, "_ShieldList");
		lua_rawget(L, -2);

		if (!(lua_istable(L, -1)))
		{
			lua_pop(L, 1);
			goto Exit1;
		}

		lua_pushvalue(L, 2);
		lua_gettable(L, -2);

		if (lua_isnil(L, -1))
		{
			lua_pop(L, 2);
			goto Exit1;
		}

		KGLogPrintf(KGLOG_ERR, "AddOn NewIndex a Forbidden Variable:%s\n", lua_tostring(L, 2));

		lua_pop(L, 3);
		return 0;
Exit1:
		lua_pushvalue(L, 2);
		lua_rawget(L, -2);
		if (lua_isnil(L, -1))
		{
			lua_settop(L, 3);
			lua_rawset(L, -3);
			lua_settop(L, 3);
			return 0;
		}

		lua_pop(L, 1);

		lua_pushvalue(L, 2);
		lua_pushvalue(L, 3);
		lua_rawset(L, -3);

		lua_settop(L, 3);
		return 0;
	}



#ifdef KGUI_LUA_LOADER
	int g_LuaGetTableField(Lua_State *pLua, int nTableIndex, KLuaTableField *pConfigVar)
    {
    	int nResult  = false;
    	int nRetCode = false;
    
        int     nTopIndex   = 0;
        int     nVarNilFlag = false;
        LPCVOID pcvVar      = NULL;

        KGLOG_PROCESS_ERROR(pLua);
        KGLOG_PROCESS_ERROR(nTableIndex != 0);
        
        KGLOG_PROCESS_ERROR(pConfigVar);
        KGLOG_PROCESS_ERROR(pConfigVar->pcszKey);
        KGLOG_PROCESS_ERROR(pConfigVar->pcszKey[0]);
        KGLOG_PROCESS_ERROR(pConfigVar->emVarType > emVAR_TYPE_BEGIN);
        KGLOG_PROCESS_ERROR(pConfigVar->emVarType < emVAR_TYPE_END);

        nRetCode = Lua_IsTable(pLua, nTableIndex);
        KGLOG_PROCESS_ERROR(nRetCode);

        nTopIndex = Lua_GetTopIndex(pLua);  // 保存下栈顶位置，用来恢复
        lua_getfield(pLua, nTableIndex, pConfigVar->pcszKey);

        nVarNilFlag = lua_isnil(pLua, -1); // 栈顶是不是nil，是：将使用默认值 否：将获取栈顶值

        switch(pConfigVar->emVarType)
        {
        case emVAR_TYPE_FLOAT:  // 处理浮点数类型参数
            if (nVarNilFlag)
            {
                *(float *)pConfigVar->pvVar = (float)pConfigVar->nDefault;
            }
            else
            {
                *(float *)pConfigVar->pvVar = (float)Lua_ValueToNumber(pLua, -1);
            }
        	break;

        case emVAR_TYPE_INT:   // 处理整数类型参数 
            if (nVarNilFlag)
            {
                *(int *)pConfigVar->pvVar = pConfigVar->nDefault;
            }
            else
            {
                *(int *)pConfigVar->pvVar = (int)Lua_ValueToNumber(pLua, -1);
            }

            break;

        case emVAR_TYPE_STRING: // 处理字符串类型参数
            KGLOG_PROCESS_ERROR(pConfigVar->pvDefault);
            KGLOG_PROCESS_ERROR(pConfigVar->uSize > 0);
            if (nVarNilFlag)
            {
                pcvVar = pConfigVar->pvDefault;
            }
            else
            {
                pcvVar = Lua_ValueToString(pLua, -1);
                KGLOG_PROCESS_ERROR(pcvVar); // 空串合法
            }
            strncpy((LPSTR)pConfigVar->pvVar, (LPSTR)pcvVar, pConfigVar->uSize);
            ((LPSTR)pConfigVar->pvVar)[pConfigVar->uSize - 1] = '\0';   
            break;

        default:
            KGLOG_PROCESS_ERROR(false && "类型错误");
            break;
        }

    // Exit1:
    	nResult = true;
    Exit0:
        Lua_SetTopIndex(pLua, nTopIndex);   // 恢复堆栈
        return nResult;
    }
#endif   
}

