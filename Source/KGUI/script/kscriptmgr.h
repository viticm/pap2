#ifndef UI_SCRIPT_K_SCRIPT_MGR_H
#define UI_SCRIPT_K_SCRIPT_MGR_H

namespace UI
{

#define STR_ADD_ON_ENV "_AddOn"

    template <typename T>
    bool TNewUserData(lua_State* L, T* pUserData, LPCSTR szMetatable)
    {
        ASSERT(szMetatable);

        T** ppUserData = reinterpret_cast<T**>(lua_newuserdata(L, sizeof(T*)));
        KGLOG_PROCESS_ERROR(ppUserData);

        *ppUserData = pUserData;

        luaL_getmetatable(L, szMetatable);
        lua_setmetatable(L, -2);

        return true;
Exit0:
        return false;
    }

    class KItemNull;
    class KWndFrame;
    class KWndWindow;

    class KScriptMgr
    {
    private:
		struct EVENT_INFO
		{
			DWORD dwCookie;
			KWndFrame* pFrame;
			int nFunctionRef;
		};
        typedef std::multimap<unsigned int, EVENT_INFO> KEventMap;

    public:
        KScriptMgr();
        ~KScriptMgr();

        bool Init();
        void Exit();
        bool ReInit();
        void Activate();

        bool LoadScriptFile(LPCSTR szFilePath, LPCSTR pcwszEnv = NULL);

        bool ExecuteScriptString(LPCSTR szScriptString, LPCSTR pcwszEnv = NULL);
		bool ExecuteScript(int nRef);

        lua_State* GetLuaState();

        bool LoadLoginScriptLib();
        bool LoadDefaultScriptLib();
        bool LoadScriptLib(LPCSTR pcszIni);

		int RefBuffer(const char * pcszBuffer);

		int UnRef(int nRef);
        int UnRefUIObject(int nRef);

        //----------------c调用lua事件接口-----------------------
        int PrepareCall(KWndWindow* pSelf, LPCSTR szEvent);
        int PrepareCall(KItemNull *pSelf, LPCSTR szEvent);
        int PrepareCall(LPCSTR szEvent);
        int Call(int nParam, int nResult);

        int FireWndEvent(KWndWindow *pWnd, LPCSTR szEvent, int *pnRet = NULL);

        int FireItemEvent(KItemNull *pItem, LPCSTR szEvent, int *pnRet = NULL);

        int OnItemInit(KItemNull *pItem, LPCSTR szScript);

        int FireGlobalEvent(LPCSTR szEvent, int *pnRet = NULL);

        void UnregisterAllEvent();
        void SubscribeEvent(KWndFrame* pFrame, LPCSTR szEvent, DWORD dwCookie);
        void UnsubscribeEvent(KWndFrame* pFrame);

		void SubscribeEvent(int nFunctionRef, LPCSTR szEvent, DWORD dwCookie);
		void UnsubscribeEvent(int nFunctionRef);

        bool BindScript(KWndFrame* pFrame);
        bool UnbindScript(KWndFrame* pFrame, LPCSTR szName, int nRef, int nDestroy);

        void FireEvent(LPCSTR szEvent, DWORD dwCookie);

		void ClearShieldTable();

		const char * GetLastError() { return m_szError; }
    private:
        int FireWindowEvent(KWndWindow* pWnd, LPCSTR szEvent);
		int FireFunctionEvent(int nFunctionRef, LPCSTR szEvent);

        //注意:使用的时候，可能会造成堆栈里面的number数据类型转换为string类型
        void LogStack(LPCSTR szMsg);

        bool LoadScriptLib(IIniFile *pIni);

		int CreateEnv();

		static int IndexOfEngine(lua_State* L);
		static int NewIndexOfEngine(lua_State* L);
		static int IndexOfAddOn(lua_State* L);
		static int NewIndexOfAddOn(lua_State* L);
    private:
        lua_State* m_LuaState;
        DWORD m_dwTickCount;
        KEventMap m_Events;
		char m_szError[1024];
    };


#ifdef KGUI_LUA_LOADER
    enum KEM_VAR_TYPE
    {
        emVAR_TYPE_BEGIN,

        emVAR_TYPE_INT,
        emVAR_TYPE_FLOAT,
        emVAR_TYPE_STRING,

        emVAR_TYPE_END,
    };

    struct KLuaTableField
    {
        LPCSTR       pcszKey;
        KEM_VAR_TYPE emVarType;
        union
        {
            LPCVOID pvDefault;
            int     nDefault;
        };
        LPVOID      pvVar;
        size_t      uSize;
    };

    int g_LuaGetTableField(Lua_State *pLua, int nTableIndex, KLuaTableField *pConfigVar);

#define KLUA_TABLE_FIELD(pcszKey, emVarType, pvDefault, pvVar, uSize)    \
    { pcszKey, emVarType, (LPCVOID)pvDefault, pvVar, uSize }

    // 设置位标志
    // nSet:        是否设置
    // dwBit:       标志位
    // dwRefFlag:   标志变量（引用）
    inline DWORD g_SetBitFlag(int nSet, DWORD dwBit, DWORD &dwRefFlag)
    {
        if (nSet)
            dwRefFlag |= dwBit;
        else
            dwRefFlag &= ~dwBit;

        return dwRefFlag;
    }
#endif
}

#endif // UI_SCRIPT_K_SCRIPT_MGR_H