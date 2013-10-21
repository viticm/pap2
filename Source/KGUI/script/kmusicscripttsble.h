#ifndef UI_SCRIPT_K_MUSIC_SCRIPT_TABLE_H
#define UI_SCRIPT_K_MUSIC_SCRIPT_TABLE_H


namespace UI
{

	class KMusicScriptTable
	{
	public:
		static void Load(lua_State* L);

	private:
		static int LuaPlayBgMusic(Lua_State* L);
		static int LuaStopBgMusic(Lua_State* L);
        static int LuaPlaySound(Lua_State* L);
	};
}


#endif