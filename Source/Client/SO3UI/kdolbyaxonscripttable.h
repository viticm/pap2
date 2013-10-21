#ifndef UI_SCRIPT_KDOLBYAXONSCRIPTTABLE_H
#define UI_SCRIPT_KDOLBYAXONSCRIPTTABLE_H

class KDolbyAxonScriptTable
{
public:
    static int Init();
    static void UnInit();

    static void Load();

private:
    static int LuaICEClient_SetPremium(lua_State* L);
    static int LuaICEClient_CreateIceClient(lua_State* L);
    static int LuaICEClient_GetDeviceList(lua_State* L);
    static int LuaICEClient_SetDefaultDevice(lua_State* L);
    static int LuaICEClient_GetDefaultEngineType(lua_State* L);
    static int LuaICEClient_Version(lua_State* L);
    static int LuaICEClient_InterfaceVersion(lua_State* L);

    static int LuaICEClient_DestroyIceClient(lua_State* L);
    static int LuaICEClient_Init(lua_State* L);
    static int LuaICEClient_GetCodecs(lua_State* L);
    static int LuaICEClient_SetCodec(lua_State* L);
    static int LuaICEClient_AutoTick(lua_State* L);
    static int LuaICEClient_Pause(lua_State* L);
    static int LuaICEClient_GetStats(lua_State* L);
    static int LuaICEClient_SetVolume(lua_State* L);
    static int LuaICEClient_SetServer(lua_State* L);
    static int LuaICEClient_SetName(lua_State* L);
    static int LuaICEClient_SetPrivateID(lua_State* L);
    static int LuaICEClient_SetRotation(lua_State* L);
    static int LuaICEClient_SetRoomMode(lua_State* L);
    static int LuaICEClient_SetVoiceFont(lua_State* L);
    static int LuaICEClient_SetCustomVoiceFont(lua_State* L);
    static int LuaICEClient_TalkInto(lua_State* L);
    static int LuaICEClient_OnlyTalkInto(lua_State* L);
    static int LuaICEClient_MuteMic(lua_State* L);
    static int LuaICEClient_Mute(lua_State* L);
    static int LuaICEClient_SetDevice(lua_State* L);
    static int LuaICEClient_GetOutputDevice(lua_State* L);
    static int LuaICEClient_GetCaptureDevice(lua_State* L);
    static int LuaICEClient_StartSpkTest(lua_State* L);
    static int LuaICEClient_StopSpkTest(lua_State* L);
    static int LuaICEClient_StartMicTestPhase1(lua_State* L);
    static int LuaICEClient_StartMicTestPhase2(lua_State* L);
    static int LuaICEClient_StopMicTest(lua_State* L);
    static int LuaICEClient_SetEchoSupression(lua_State* L);
    static int LuaICEClient_SetAudioLogging(lua_State* L);
    static int LuaICEClient_DumpState(lua_State* L);
    static int LuaICEClient_CreateStateDump(lua_State* L);
    static int LuaICEClient_GetStateDumpFile(lua_State* L);
    static int LuaICEClient_ClearStateDump(lua_State* L);
};


#endif // UI_SCRIPT_KDOLBYAXONSCRIPTTABLE_H
