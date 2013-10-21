#ifndef UI_SCRIPT_K_SCHEME_SCRIPT_TABLE_H
#define UI_SCRIPT_K_SCHEME_SCRIPT_TABLE_H

class KSchemeScriptTable
{
public:
	static void Load();

private:
	static int LuaGWTextEncoder_Encode(Lua_State *L);
	static int LuaGWTextEncoder_EncodeTalkData(Lua_State *L);

	static int LuaInteraction_Clear(Lua_State *L);
	static int LuaInteraction_AddParam(Lua_State *L);
	static int LuaInteraction_Send(Lua_State *L);
	static int LuaInteraction_Request(Lua_State *L);

	static void Interaction_SendResult(char *pszName, int nResult);
	static void Interaction_RequestResult(char *pszName, int nResult, BYTE *pbyBuffer, DWORD dwBufferSize);

    static int LuaAttributeStringToID(lua_State *pLua);
    static int LuaRequireStringToID(lua_State *pLua);

};

#endif // UI_SCRIPT_K_SCHEME_SCRIPT_TABLE_H

