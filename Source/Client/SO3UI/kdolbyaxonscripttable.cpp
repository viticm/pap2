#include "stdafx.h"
#include "SO3UI/IKGUI.h"
#include "KG3DEngine/KG3DInterface.h"
#include "./kdolbyaxonscripttable.h"
#include "./kscript.h"
#include "./SO3UI.h"
#include "iceclientlib.h"


namespace
{
    typedef void (*FnICEClient_SetPremium)(int prem);
    typedef ICECLIENT* (*FnICEClient_CreateIceClient)();
    typedef void (*FnICEClient_DestroyIceClient)(ICECLIENT* ic);
    typedef ICECLIENT_ERROR (*FnICEClient_Init)(ICECLIENT* ic, ICECLIENT_ENGINE_TYPE engine_type);
    typedef ICECLIENT_ERROR (*FnICEClient_InitDolby)(ICECLIENT* ic);
    typedef const unsigned char* (*FnICEClient_GetCodecs)(ICECLIENT* ic);
    typedef ICECLIENT_ERROR (*FnICEClient_SetCodec)(ICECLIENT* ic, unsigned char codec_id);
    typedef void (*FnICEClient_SetLogging)(ICECLIENT* ic, void (*logfunc)(unsigned int level, const char* msg, void* rock), void* rock, unsigned int level);
    typedef void (*FnICEClient_Tick)(ICECLIENT* ic);
    typedef ICECLIENT_ERROR (*FnICEClient_AutoTick)(ICECLIENT* ic);
    typedef ICECLIENT_ERROR (*FnICEClient_Pause)(ICECLIENT* ic, int on);
    typedef ICEClient_stat (*FnICEClient_GetStats)(ICECLIENT* ic);
    typedef void (*FnICEClient_SetVolume)(ICECLIENT* ic, const float vol);
    typedef ICECLIENT_ERROR (*FnICEClient_SetServer)(ICECLIENT* ic, const char* server, const unsigned short port);
    typedef void (*FnICEClient_SetName)(ICECLIENT* ic, const char* player_name);
    typedef void (*FnICEClient_SetPrivateID)(ICECLIENT* ic, const unsigned int privID);
    typedef ICECLIENT_ERROR (*FnICEClient_SetRotation)(ICECLIENT* ic, const int rot);
    typedef ICECLIENT_ERROR (*FnICEClient_SetRotation_npr)(ICECLIENT* ic, const int yaw, const int pitch, const int roll);
    typedef ICECLIENT_ERROR (*FnICEClient_SetRoomMode)(ICECLIENT* ic, ICECLIENT_ROOM_MODE mode); 
    typedef ICECLIENT_ERROR (*FnICEClient_SetVoiceFont)(ICECLIENT* ic, ICECLIENT_VOICEFONT_PRESET preset);
    typedef ICECLIENT_ERROR (*FnICEClient_SetCustomVoiceFont)(ICECLIENT* ic, ICEClient_voicefont vfont);
    typedef void (*FnICEClient_TalkInto)(ICECLIENT* ic, const int on, const unsigned char channel_mapping);
    typedef void (*FnICEClient_OnlyTalkInto)(ICECLIENT* ic, const unsigned char* channel_mappings, const unsigned short len);
    typedef void (*FnICEClient_MuteMic)(ICECLIENT* ic, int on);
    typedef void (*FnICEClient_Mute)(ICECLIENT* ic, int on);
    typedef ICECLIENT_ERROR (*FnICEClient_GetDeviceList)(ICEClient_device* devices, unsigned int *len);
    typedef ICECLIENT_ERROR (*FnICEClient_SetDefaultDevice)(const ICEClient_device* spkdev, const ICEClient_device* micdev);
    typedef ICECLIENT_ENGINE_TYPE (*FnICEClient_GetDefaultEngineType)(const ICECLIENT_DEVICE_TYPE devtype);
    typedef ICECLIENT_ERROR (*FnICEClient_SetDevice)(ICECLIENT* ic, const ICEClient_device* spkdev, const ICEClient_device* micdev); 
    typedef ICECLIENT_ERROR (*FnICEClient_GetOutputDevice)(ICECLIENT* ic, ICEClient_device* spkdev); 
    typedef ICECLIENT_ERROR (*FnICEClient_GetCaptureDevice)(ICECLIENT* ic, ICEClient_device* micdev); 
    typedef ICECLIENT_ERROR (*FnICEClient_StartSpkTest)(ICECLIENT* ic, ICECLIENT_SPKTEST_MODE mode, void (*callback)(void*), void *rock);
    typedef ICECLIENT_ERROR (*FnICEClient_StopSpkTest)(ICECLIENT* ic);
    typedef ICECLIENT_ERROR (*FnICEClient_StartMicTestPhase1)(ICECLIENT* ic);
    typedef ICECLIENT_ERROR (*FnICEClient_StartMicTestPhase2)(ICECLIENT* ic, ICECLIENT_MICTEST_MODE mode, void (*callback)(void*), void *rock);
    typedef ICECLIENT_ERROR (*FnICEClient_StopMicTest)(ICECLIENT* ic);
    typedef ICECLIENT_ERROR (*FnICEClient_SetGameEncapsulation)(ICECLIENT* ic, void (*encapsulatefunc)(const char* msg, unsigned int len, void* rock), void* rock);
    typedef ICECLIENT_ERROR (*FnICEClient_ReceiveCapsule)(ICECLIENT* ic, const char* msg, unsigned int len);
    typedef ICECLIENT_ERROR (*FnICEClient_SetGameIcon)(ICECLIENT* ic, const char* data, unsigned int len);
    typedef ICECLIENT_ERROR (*FnICEClient_SetEchoSupression)(ICECLIENT* ic, int on);
    typedef ICECLIENT_ERROR (*FnICEClient_SetAudioLogging)(ICECLIENT* ic, int on);
    typedef ICECLIENT_ERROR (*FnICEClient_DumpState)(ICECLIENT* ic);
    typedef unsigned int (*FnICEClient_CreateStateDump)(ICECLIENT* ic, const int include_audio);
    typedef ICECLIENT_ERROR (*FnICEClient_GetStateDumpFile)(ICECLIENT* ic, const unsigned int index, const char** filename, const char** data, unsigned int *len);
    typedef void (*FnICEClient_ClearStateDump)(ICECLIENT* ic);
    typedef ICECLIENT_ERROR (*FnICEClient_PlayWav)(ICECLIENT* ic, const char* data, const unsigned int len, const ICEClient_wavspec spec, const unsigned int repeat, const float volume, unsigned int *wav_id, void (*callback)(void*), void* rock);
    typedef void (*FnICEClient_CancelWav)(ICECLIENT* ic, const unsigned int wav_id);
    typedef const char* (*FnICEClient_Version)();
    typedef const char* (*FnICEClient_InterfaceVersion)();
    typedef const char* (*FnICEClient_ErrorToString)(const ICECLIENT_ERROR e);

    FnICEClient_SetPremium pfnICEClient_SetPremium = NULL;
    FnICEClient_CreateIceClient pfnICEClient_CreateIceClient = NULL;
    FnICEClient_DestroyIceClient pfnICEClient_DestroyIceClient = NULL;
    FnICEClient_Init pfnICEClient_Init = NULL;
    FnICEClient_InitDolby pfnICEClient_InitDolby = NULL;
    FnICEClient_GetCodecs pfnICEClient_GetCodecs = NULL;
    FnICEClient_SetCodec pfnICEClient_SetCodec = NULL;
    FnICEClient_SetLogging pfnICEClient_SetLogging = NULL;
    FnICEClient_Tick pfnICEClient_Tick = NULL;
    FnICEClient_AutoTick pfnICEClient_AutoTick = NULL;
    FnICEClient_Pause pfnICEClient_Pause = NULL;
    FnICEClient_GetStats pfnICEClient_GetStats = NULL;
    FnICEClient_SetVolume pfnICEClient_SetVolume = NULL;
    FnICEClient_SetServer pfnICEClient_SetServer = NULL;
    FnICEClient_SetName pfnICEClient_SetName = NULL;
    FnICEClient_SetPrivateID pfnICEClient_SetPrivateID = NULL;
    FnICEClient_SetRotation pfnICEClient_SetRotation = NULL;
    FnICEClient_SetRotation_npr pfnICEClient_SetRotation_npr = NULL;
    FnICEClient_SetRoomMode pfnICEClient_SetRoomMode = NULL; 
    FnICEClient_SetVoiceFont pfnICEClient_SetVoiceFont = NULL;
    FnICEClient_SetCustomVoiceFont pfnICEClient_SetCustomVoiceFont = NULL;
    FnICEClient_TalkInto pfnICEClient_TalkInto = NULL;
    FnICEClient_OnlyTalkInto pfnICEClient_OnlyTalkInto = NULL;
    FnICEClient_MuteMic pfnICEClient_MuteMic = NULL;
    FnICEClient_Mute pfnICEClient_Mute = NULL;
    FnICEClient_GetDeviceList pfnICEClient_GetDeviceList = NULL;
    FnICEClient_SetDefaultDevice pfnICEClient_SetDefaultDevice = NULL;
    FnICEClient_GetDefaultEngineType pfnICEClient_GetDefaultEngineType = NULL;
    FnICEClient_SetDevice pfnICEClient_SetDevice = NULL; 
    FnICEClient_GetOutputDevice pfnICEClient_GetOutputDevice = NULL;
    FnICEClient_GetCaptureDevice pfnICEClient_GetCaptureDevice = NULL; 
    FnICEClient_StartSpkTest pfnICEClient_StartSpkTest = NULL;
    FnICEClient_StopSpkTest pfnICEClient_StopSpkTest = NULL;
    FnICEClient_StartMicTestPhase1 pfnICEClient_StartMicTestPhase1 = NULL;
    FnICEClient_StartMicTestPhase2 pfnICEClient_StartMicTestPhase2 = NULL;
    FnICEClient_StopMicTest pfnICEClient_StopMicTest = NULL;
    FnICEClient_SetGameEncapsulation pfnICEClient_SetGameEncapsulation = NULL;
    FnICEClient_ReceiveCapsule pfnICEClient_ReceiveCapsule = NULL;
    FnICEClient_SetGameIcon pfnICEClient_SetGameIcon = NULL;
    FnICEClient_SetEchoSupression pfnICEClient_SetEchoSupression = NULL;
    FnICEClient_SetAudioLogging pfnICEClient_SetAudioLogging = NULL;
    FnICEClient_DumpState pfnICEClient_DumpState = NULL;
    FnICEClient_CreateStateDump pfnICEClient_CreateStateDump = NULL;
    FnICEClient_GetStateDumpFile pfnICEClient_GetStateDumpFile = NULL;
    FnICEClient_ClearStateDump pfnICEClient_ClearStateDump = NULL;
    FnICEClient_PlayWav pfnICEClient_PlayWav = NULL;
    FnICEClient_CancelWav pfnICEClient_CancelWav = NULL;
    FnICEClient_Version pfnICEClient_Version = NULL;
    FnICEClient_InterfaceVersion pfnICEClient_InterfaceVersion = NULL;
    FnICEClient_ErrorToString pfnICEClient_ErrorToString = NULL;

	HMODULE g_hIceClientDll = NULL;

    void UnloadIceClientDll()
    {
        pfnICEClient_SetPremium = NULL;
        pfnICEClient_CreateIceClient = NULL;
        pfnICEClient_DestroyIceClient = NULL;
        pfnICEClient_Init = NULL;
        pfnICEClient_InitDolby = NULL;
        pfnICEClient_GetCodecs = NULL;
        pfnICEClient_SetCodec = NULL;
        pfnICEClient_SetLogging = NULL;
        pfnICEClient_Tick = NULL;
        pfnICEClient_AutoTick = NULL;
        pfnICEClient_Pause = NULL;
        pfnICEClient_GetStats = NULL;
        pfnICEClient_SetVolume = NULL;
        pfnICEClient_SetServer = NULL;
        pfnICEClient_SetName = NULL;
        pfnICEClient_SetPrivateID = NULL;
        pfnICEClient_SetRotation = NULL;
        pfnICEClient_SetRotation_npr = NULL;
        pfnICEClient_SetRoomMode = NULL; 
        pfnICEClient_SetVoiceFont = NULL;
        pfnICEClient_SetCustomVoiceFont = NULL;
        pfnICEClient_TalkInto = NULL;
        pfnICEClient_OnlyTalkInto = NULL;
        pfnICEClient_MuteMic = NULL;
        pfnICEClient_Mute = NULL;
        pfnICEClient_GetDeviceList = NULL;
        pfnICEClient_SetDefaultDevice = NULL;
        pfnICEClient_GetDefaultEngineType = NULL;
        pfnICEClient_SetDevice = NULL; 
        pfnICEClient_GetOutputDevice = NULL;
        pfnICEClient_GetCaptureDevice = NULL; 
        pfnICEClient_StartSpkTest = NULL;
        pfnICEClient_StopSpkTest = NULL;
        pfnICEClient_StartMicTestPhase1 = NULL;
        pfnICEClient_StartMicTestPhase2 = NULL;
        pfnICEClient_StopMicTest = NULL;
        pfnICEClient_SetGameEncapsulation = NULL;
        pfnICEClient_ReceiveCapsule = NULL;
        pfnICEClient_SetGameIcon = NULL;
        pfnICEClient_SetEchoSupression = NULL;
        pfnICEClient_SetAudioLogging = NULL;
        pfnICEClient_DumpState = NULL;
        pfnICEClient_CreateStateDump = NULL;
        pfnICEClient_GetStateDumpFile = NULL;
        pfnICEClient_ClearStateDump = NULL;
        pfnICEClient_PlayWav = NULL;
        pfnICEClient_CancelWav = NULL;
        pfnICEClient_Version = NULL;
        pfnICEClient_InterfaceVersion = NULL;
        pfnICEClient_ErrorToString = NULL;

        if (g_hIceClientDll)
        {
            ::FreeLibrary(g_hIceClientDll);
            g_hIceClientDll = NULL;
        }
    }

    int LoadIceClientDll()
    {
        int nRetCode = false;
        int nResult = false;

        g_hIceClientDll = ::LoadLibraryA(g_pSO3UI->m_szIceClientLibDllPath);
        KG_PROCESS_SUCCESS(g_hIceClientDll == NULL);

        pfnICEClient_SetPremium = (FnICEClient_SetPremium)::GetProcAddress(g_hIceClientDll, "ICEClient_SetPremium");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetPremium);

        pfnICEClient_CreateIceClient = (FnICEClient_CreateIceClient)::GetProcAddress(g_hIceClientDll, "ICEClient_CreateIceClient");
        KGLOG_PROCESS_ERROR(pfnICEClient_CreateIceClient);

        pfnICEClient_DestroyIceClient = (FnICEClient_DestroyIceClient)::GetProcAddress(g_hIceClientDll, "ICEClient_DestroyIceClient");
        KGLOG_PROCESS_ERROR(pfnICEClient_DestroyIceClient);

        pfnICEClient_Init = (FnICEClient_Init)::GetProcAddress(g_hIceClientDll, "ICEClient_Init");
        KGLOG_PROCESS_ERROR(pfnICEClient_Init);

        pfnICEClient_InitDolby = (FnICEClient_InitDolby)::GetProcAddress(g_hIceClientDll, "ICEClient_InitDolby");
        KGLOG_PROCESS_ERROR(pfnICEClient_InitDolby);

        pfnICEClient_GetCodecs = (FnICEClient_GetCodecs)::GetProcAddress(g_hIceClientDll, "ICEClient_GetCodecs");
        KGLOG_PROCESS_ERROR(pfnICEClient_GetCodecs);

        pfnICEClient_SetCodec = (FnICEClient_SetCodec)::GetProcAddress(g_hIceClientDll, "ICEClient_SetCodec");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetCodec);

        pfnICEClient_SetLogging = (FnICEClient_SetLogging)::GetProcAddress(g_hIceClientDll, "ICEClient_SetLogging");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetLogging);

        pfnICEClient_Tick = (FnICEClient_Tick)::GetProcAddress(g_hIceClientDll, "ICEClient_Tick");
        KGLOG_PROCESS_ERROR(pfnICEClient_Tick);

        pfnICEClient_AutoTick = (FnICEClient_AutoTick)::GetProcAddress(g_hIceClientDll, "ICEClient_AutoTick");
        KGLOG_PROCESS_ERROR(pfnICEClient_AutoTick);

        pfnICEClient_Pause = (FnICEClient_Pause)::GetProcAddress(g_hIceClientDll, "ICEClient_Pause");
        KGLOG_PROCESS_ERROR(pfnICEClient_Pause);

        pfnICEClient_GetStats = (FnICEClient_GetStats)::GetProcAddress(g_hIceClientDll, "ICEClient_GetStats");
        KGLOG_PROCESS_ERROR(pfnICEClient_GetStats);

        pfnICEClient_SetVolume = (FnICEClient_SetVolume)::GetProcAddress(g_hIceClientDll, "ICEClient_SetVolume");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetVolume);

        pfnICEClient_SetServer = (FnICEClient_SetServer)::GetProcAddress(g_hIceClientDll, "ICEClient_SetServer");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetServer);

        pfnICEClient_SetName = (FnICEClient_SetName)::GetProcAddress(g_hIceClientDll, "ICEClient_SetName");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetName);

        pfnICEClient_SetPrivateID = (FnICEClient_SetPrivateID)::GetProcAddress(g_hIceClientDll, "ICEClient_SetPrivateID");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetPrivateID);

        pfnICEClient_SetRotation = (FnICEClient_SetRotation)::GetProcAddress(g_hIceClientDll, "ICEClient_SetRotation");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetRotation);

        pfnICEClient_SetRotation_npr = (FnICEClient_SetRotation_npr)::GetProcAddress(g_hIceClientDll, "ICEClient_SetRotation_npr");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetRotation_npr);

        pfnICEClient_SetRoomMode = (FnICEClient_SetRoomMode)::GetProcAddress(g_hIceClientDll, "ICEClient_SetRoomMode");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetRoomMode);

        pfnICEClient_SetVoiceFont = (FnICEClient_SetVoiceFont)::GetProcAddress(g_hIceClientDll, "ICEClient_SetVoiceFont");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetVoiceFont);

        pfnICEClient_SetCustomVoiceFont = (FnICEClient_SetCustomVoiceFont)::GetProcAddress(g_hIceClientDll, "ICEClient_SetCustomVoiceFont");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetCustomVoiceFont);

        pfnICEClient_TalkInto = (FnICEClient_TalkInto)::GetProcAddress(g_hIceClientDll, "ICEClient_TalkInto");
        KGLOG_PROCESS_ERROR(pfnICEClient_TalkInto);

        pfnICEClient_OnlyTalkInto = (FnICEClient_OnlyTalkInto)::GetProcAddress(g_hIceClientDll, "ICEClient_OnlyTalkInto");
        KGLOG_PROCESS_ERROR(pfnICEClient_OnlyTalkInto);

        pfnICEClient_MuteMic = (FnICEClient_MuteMic)::GetProcAddress(g_hIceClientDll, "ICEClient_MuteMic");
        KGLOG_PROCESS_ERROR(pfnICEClient_MuteMic);

        pfnICEClient_Mute = (FnICEClient_Mute)::GetProcAddress(g_hIceClientDll, "ICEClient_Mute");
        KGLOG_PROCESS_ERROR(pfnICEClient_Mute);

        pfnICEClient_GetDeviceList = (FnICEClient_GetDeviceList)::GetProcAddress(g_hIceClientDll, "ICEClient_GetDeviceList");
        KGLOG_PROCESS_ERROR(pfnICEClient_GetDeviceList);

        pfnICEClient_SetDefaultDevice = (FnICEClient_SetDefaultDevice)::GetProcAddress(g_hIceClientDll, "ICEClient_SetDefaultDevice");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetDefaultDevice);

        pfnICEClient_GetDefaultEngineType = (FnICEClient_GetDefaultEngineType)::GetProcAddress(g_hIceClientDll, "ICEClient_GetDefaultEngineType");
        KGLOG_PROCESS_ERROR(pfnICEClient_GetDefaultEngineType);

        pfnICEClient_SetDevice = (FnICEClient_SetDevice)::GetProcAddress(g_hIceClientDll, "ICEClient_SetDevice");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetDevice);

        pfnICEClient_GetOutputDevice = (FnICEClient_GetOutputDevice)::GetProcAddress(g_hIceClientDll, "ICEClient_GetOutputDevice");
        KGLOG_PROCESS_ERROR(pfnICEClient_GetOutputDevice);

        pfnICEClient_GetCaptureDevice = (FnICEClient_GetCaptureDevice)::GetProcAddress(g_hIceClientDll, "ICEClient_GetCaptureDevice");
        KGLOG_PROCESS_ERROR(pfnICEClient_GetCaptureDevice);

        pfnICEClient_StartSpkTest = (FnICEClient_StartSpkTest)::GetProcAddress(g_hIceClientDll, "ICEClient_StartSpkTest");
        KGLOG_PROCESS_ERROR(pfnICEClient_StartSpkTest);

        pfnICEClient_StopSpkTest = (FnICEClient_StopSpkTest)::GetProcAddress(g_hIceClientDll, "ICEClient_StopSpkTest");
        KGLOG_PROCESS_ERROR(pfnICEClient_StopSpkTest);

        pfnICEClient_StartMicTestPhase1 = (FnICEClient_StartMicTestPhase1)::GetProcAddress(g_hIceClientDll, "ICEClient_StartMicTestPhase1");
        KGLOG_PROCESS_ERROR(pfnICEClient_StartMicTestPhase1);

        pfnICEClient_StartMicTestPhase2 = (FnICEClient_StartMicTestPhase2)::GetProcAddress(g_hIceClientDll, "ICEClient_StartMicTestPhase2");
        KGLOG_PROCESS_ERROR(pfnICEClient_StartMicTestPhase2);

        pfnICEClient_StopMicTest = (FnICEClient_StopMicTest)::GetProcAddress(g_hIceClientDll, "ICEClient_StopMicTest");
        KGLOG_PROCESS_ERROR(pfnICEClient_StopMicTest);

        pfnICEClient_SetGameEncapsulation = (FnICEClient_SetGameEncapsulation)::GetProcAddress(g_hIceClientDll, "ICEClient_SetGameEncapsulation");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetGameEncapsulation);

        pfnICEClient_ReceiveCapsule = (FnICEClient_ReceiveCapsule)::GetProcAddress(g_hIceClientDll, "ICEClient_ReceiveCapsule");
        KGLOG_PROCESS_ERROR(pfnICEClient_ReceiveCapsule);

        pfnICEClient_SetGameIcon = (FnICEClient_SetGameIcon)::GetProcAddress(g_hIceClientDll, "ICEClient_SetGameIcon");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetGameIcon);

        pfnICEClient_SetEchoSupression = (FnICEClient_SetEchoSupression)::GetProcAddress(g_hIceClientDll, "ICEClient_SetEchoSupression");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetEchoSupression);

        pfnICEClient_SetAudioLogging = (FnICEClient_SetAudioLogging)::GetProcAddress(g_hIceClientDll, "ICEClient_SetAudioLogging");
        KGLOG_PROCESS_ERROR(pfnICEClient_SetAudioLogging);

        pfnICEClient_DumpState = (FnICEClient_DumpState)::GetProcAddress(g_hIceClientDll, "ICEClient_DumpState");
        KGLOG_PROCESS_ERROR(pfnICEClient_DumpState);

        pfnICEClient_CreateStateDump = (FnICEClient_CreateStateDump)::GetProcAddress(g_hIceClientDll, "ICEClient_CreateStateDump");
        KGLOG_PROCESS_ERROR(pfnICEClient_CreateStateDump);

        pfnICEClient_GetStateDumpFile = (FnICEClient_GetStateDumpFile)::GetProcAddress(g_hIceClientDll, "ICEClient_GetStateDumpFile");
        KGLOG_PROCESS_ERROR(pfnICEClient_GetStateDumpFile);

        pfnICEClient_ClearStateDump = (FnICEClient_ClearStateDump)::GetProcAddress(g_hIceClientDll, "ICEClient_ClearStateDump");
        KGLOG_PROCESS_ERROR(pfnICEClient_ClearStateDump);

        pfnICEClient_PlayWav = (FnICEClient_PlayWav)::GetProcAddress(g_hIceClientDll, "ICEClient_PlayWav");
        KGLOG_PROCESS_ERROR(pfnICEClient_PlayWav);

        pfnICEClient_CancelWav = (FnICEClient_CancelWav)::GetProcAddress(g_hIceClientDll, "ICEClient_CancelWav");
        KGLOG_PROCESS_ERROR(pfnICEClient_CancelWav);

        pfnICEClient_CancelWav = (FnICEClient_CancelWav)::GetProcAddress(g_hIceClientDll, "ICEClient_CancelWav");
        KGLOG_PROCESS_ERROR(pfnICEClient_CancelWav);

        pfnICEClient_Version = (FnICEClient_Version)::GetProcAddress(g_hIceClientDll, "ICEClient_Version");
        KGLOG_PROCESS_ERROR(pfnICEClient_Version);

        pfnICEClient_InterfaceVersion = (FnICEClient_InterfaceVersion)::GetProcAddress(g_hIceClientDll, "ICEClient_InterfaceVersion");
        KGLOG_PROCESS_ERROR(pfnICEClient_InterfaceVersion);

        pfnICEClient_ErrorToString = (FnICEClient_ErrorToString)::GetProcAddress(g_hIceClientDll, "ICEClient_ErrorToString");
        KGLOG_PROCESS_ERROR(pfnICEClient_ErrorToString);

Exit1:
        nResult = true;

Exit0:
        if (!nResult)
        {
            UnloadIceClientDll();
            KGLogPrintf(KGLOG_ERR, "KGUI LoadLibrary(%s) failed.\n", g_pSO3UI->m_szIceClientLibDllPath);
        }
        return nResult;
    }
}

int KDolbyAxonScriptTable::Init()
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = LoadIceClientDll();
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

void KDolbyAxonScriptTable::UnInit()
{
    UnloadIceClientDll();
}

void KDolbyAxonScriptTable::Load()
{
	static luaL_reg const s_ICEClientLibTable[] =
	{
		{ "SetPremium", LuaICEClient_SetPremium },
		{ "CreateIceClient", LuaICEClient_CreateIceClient },
        { "GetDeviceList", LuaICEClient_GetDeviceList },
        { "SetDefaultDevice", LuaICEClient_SetDefaultDevice },
        { "GetDefaultEngineType", LuaICEClient_GetDefaultEngineType },
        { "Version", LuaICEClient_Version },
        { "InterfaceVersion", LuaICEClient_InterfaceVersion },

		{ NULL, NULL },
	};

	static luaL_reg const s_ICEClientMetaTable[] =
	{
		{ "Destroy", LuaICEClient_DestroyIceClient },
		{ "Init", LuaICEClient_Init },
        { "GetCodecs", LuaICEClient_GetCodecs },
        { "SetCodec", LuaICEClient_SetCodec },
        { "AutoTick", LuaICEClient_AutoTick },
        { "Pause", LuaICEClient_Pause },
        { "GetStats", LuaICEClient_GetStats },
        { "SetVolume", LuaICEClient_SetVolume },
        { "SetServer", LuaICEClient_SetServer },
        { "SetName", LuaICEClient_SetName },
        { "SetPrivateID", LuaICEClient_SetPrivateID },
        { "SetRotation", LuaICEClient_SetRotation },
        { "SetRoomMode", LuaICEClient_SetRoomMode },
        { "SetVoiceFont", LuaICEClient_SetVoiceFont },
        { "SetCustomVoiceFont", LuaICEClient_SetCustomVoiceFont },
        { "TalkInto", LuaICEClient_TalkInto },
        { "OnlyTalkInto", LuaICEClient_OnlyTalkInto },
        { "MuteMic", LuaICEClient_MuteMic },
        { "Mute", LuaICEClient_Mute },
        { "SetDevice", LuaICEClient_SetDevice },
        { "GetOutputDevice", LuaICEClient_GetOutputDevice },
        { "GetCaptureDevice", LuaICEClient_GetCaptureDevice },
        { "StartSpkTest", LuaICEClient_StartSpkTest },
        { "StopSpkTest", LuaICEClient_StopSpkTest },
        { "StartMicTestPhase1", LuaICEClient_StartMicTestPhase1 },
        { "StartMicTestPhase2", LuaICEClient_StartMicTestPhase2 },
        { "StopMicTest", LuaICEClient_StopMicTest },
        { "SetEchoSupression", LuaICEClient_SetEchoSupression },
        { "SetAudioLogging", LuaICEClient_SetAudioLogging },
        { "DumpState", LuaICEClient_DumpState },
        { "CreateStateDump", LuaICEClient_CreateStateDump },
        { "GetStateDumpFile", LuaICEClient_GetStateDumpFile },
        { "ClearStateDump", LuaICEClient_ClearStateDump },

		{ NULL,	NULL },
	};

	g_pSO3UI->m_pUI->RegisterLibTable("ICEClient", s_ICEClientLibTable);

	g_pSO3UI->m_pUI->RegisterMetaTable("ICECLIENT", s_ICEClientMetaTable);
}

static bool NewUserData(lua_State* L, ICECLIENT* pUserData)
{
	return TNewUserData(L, pUserData, "ICECLIENT");
}

static ICECLIENT* ToICECLIENT(lua_State* L, int nIndex)
{
	ICECLIENT** ppUserData = (ICECLIENT**)lua_touserdata(L, nIndex);
	return ppUserData ? *ppUserData : NULL;
}

int KDolbyAxonScriptTable::LuaICEClient_SetPremium(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nPremium = false;

    KG_PROCESS_SUCCESS(pfnICEClient_SetPremium == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    nPremium = lua_toboolean(L, 1);

    pfnICEClient_SetPremium(nPremium);

Exit1:
    nResult = true;
Exit0:
    return 0;
}

static int PushError(lua_State* L, ICECLIENT_ERROR nError)
{
    if (nError != ICECLIENT_ERROR_NONE)
    {
        if (pfnICEClient_ErrorToString)
        {
            lua_pushstring(L, pfnICEClient_ErrorToString(nError));
        }
        else
        {
            lua_pushstring(L, "IceClient not initialized");
        }
    }
    else
    {
        lua_pushstring(L, "[ui] argument error");
    }

    return 1;
}

static void IceClientLogCallback(unsigned int uLevel, const char* pcszMsg, void* pRock)
{
    //       0 : CRITICAL
    //       1 : SERIOUS
    //       2 : WARNING
    //       3 : DEBUG

    KGLogPrintf(KGLOG_ERR, "[IceClient][%d] %s\n", pcszMsg);
}

int KDolbyAxonScriptTable::LuaICEClient_CreateIceClient(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    unsigned int uLogPriority = 0;
    ICECLIENT* pIceClient = NULL;
    
    KG_PROCESS_SUCCESS(pfnICEClient_CreateIceClient == NULL);
    KG_PROCESS_SUCCESS(pfnICEClient_SetLogging == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    uLogPriority = (unsigned int)lua_tonumber(L, 1);

    pIceClient = pfnICEClient_CreateIceClient();
    KGLOG_PROCESS_ERROR(pIceClient);

    nRetCode = NewUserData(L, pIceClient);
    KGLOG_PROCESS_ERROR(nRetCode);

    pfnICEClient_SetLogging(pIceClient, IceClientLogCallback, NULL, uLogPriority);

    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

struct KIceClientDeviceItem
{
    LPCSTR szName;
    ICECLIENT_DEVICE_TYPE nType;
};

static KIceClientDeviceItem s_IceClientDeviceItems[] = 
{
    { "unknown", ICECLIENT_DEVICE_UNKNOWN },
    { "capture", ICECLIENT_DEVICE_CAPTURE },
    { "headphones", ICECLIENT_DEVICE_HEADPHONES },
    { "stereospeakers", ICECLIENT_DEVICE_STEREOSPEAKERS },
    { "surroundspeakers", ICECLIENT_DEVICE_SURROUNDSPEAKERS },
    { NULL, ICECLIENT_DEVICE_UNKNOWN },
};

static LPCSTR IceClientDeviceName(ICECLIENT_DEVICE_TYPE nType)
{
    for (KIceClientDeviceItem* pItem = s_IceClientDeviceItems; pItem->szName; ++pItem)
    {
        if (pItem->nType == nType)
        {
            return pItem->szName;
        }
    }

    return s_IceClientDeviceItems[0].szName;
}

static ICECLIENT_DEVICE_TYPE IceClientDeviceType(LPCSTR szName)
{
    ICECLIENT_DEVICE_TYPE nType = ICECLIENT_DEVICE_UNKNOWN;

    KGLOG_PROCESS_ERROR(szName);

    for (KIceClientDeviceItem* pItem = s_IceClientDeviceItems; pItem->szName; ++pItem)
    {
        if (strcmp(pItem->szName, szName) == 0)
        {
            nType = pItem->nType;
            break;
        }
    }

Exit0:
    return nType;
}

struct KIceClientEngineTypeItem
{
    LPCSTR szName;
    ICECLIENT_ENGINE_TYPE nType;
};

static KIceClientEngineTypeItem s_IceClientEngineTypeItems[] =
{
    { "default", ICECLIENT_ENGINE_DEFAULT },
    { "mono", ICECLIENT_ENGINE_MONO },
    { "directx", ICECLIENT_ENGINE_DIRECTX },
    { "dolbyheadphone", ICECLIENT_ENGINE_DOLBYHEADPHONE },
    { "dolbyvirtualspeaker", ICECLIENT_ENGINE_DOLBYVIRTUALSPEAKER },
};

static LPCSTR GetIceClientEngineTypeName(ICECLIENT_ENGINE_TYPE nType)
{
    for (KIceClientEngineTypeItem* pItem = s_IceClientEngineTypeItems; pItem->szName; ++pItem)
    {
        if (pItem->nType == nType)
        {
            return pItem->szName;
        }
    }

    return s_IceClientEngineTypeItems[0].szName;
}

static ICECLIENT_ENGINE_TYPE IceClientEngineType(LPCSTR szName)
{
    ICECLIENT_ENGINE_TYPE nType = ICECLIENT_ENGINE_DEFAULT;

    KGLOG_PROCESS_ERROR(szName);

    for (KIceClientEngineTypeItem* pItem = s_IceClientEngineTypeItems; pItem->szName; ++pItem)
    {
        if (strcmp(pItem->szName, szName) == 0)
        {
            nType = pItem->nType;
            break;
        }
    }

Exit0:
    return nType;
}

static int SetIceClientDevice(lua_State* L, int nIndex, ICEClient_device const* pDevice)
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = lua_istable(L, nIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLOG_PROCESS_ERROR(pDevice);

    lua_pushnumber(L, pDevice->id);
    lua_setfield(L, nIndex, "id");

    lua_pushstring(L, pDevice->name);
    lua_setfield(L, nIndex, "name");

    lua_pushstring(L, IceClientDeviceName(pDevice->devtype));
    lua_setfield(L, nIndex, "devtype");

    lua_pushboolean(L, pDevice->systemdefault);
    lua_setfield(L, nIndex, "systemdefault");

    lua_pushboolean(L, pDevice->icedefault);
    lua_setfield(L, nIndex, "icedefault");

    lua_pushstring(L, GetIceClientEngineTypeName(pDevice->enginetype));
    lua_setfield(L, nIndex, "enginetype");

    nResult = true;
Exit0:
    return nResult;
}

static int GetIceClientDevice(lua_State* L, int nIndex, ICEClient_device* pDevice)
{
    int nRetCode = false;
    int nResult = false;
    int nTop = 0;

    nTop = lua_gettop(L);

    nRetCode = lua_istable(L, nIndex);
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLOG_PROCESS_ERROR(pDevice);

    lua_getfield(L, nIndex, "id");
    if (lua_isnumber(L, -1))
        pDevice->id = (unsigned int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "name");
    if (lua_isstring(L, -1))
    {
        nRetCode = strncpy_s(pDevice->name, _countof(pDevice->name), lua_tostring(L, -1), _TRUNCATE);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "devtype");
    if (lua_isstring(L, -1))
        pDevice->devtype = IceClientDeviceType(lua_tostring(L, -1));
    else if (lua_isnumber(L, -1))
        pDevice->devtype = (ICECLIENT_DEVICE_TYPE)(int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "systemdefault");
    pDevice->systemdefault = lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "icedefault");
    pDevice->icedefault = lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, nIndex, "enginetype");
    if (lua_isstring(L, -1))
        pDevice->enginetype = IceClientEngineType(lua_tostring(L, -1));
    else if (lua_isnumber(L, -1))
        pDevice->enginetype = (ICECLIENT_ENGINE_TYPE)(int)lua_tonumber(L, -1);
    lua_pop(L, 1);

    nResult = true;
Exit0:
    if (!nResult)
    {
        lua_settop(L, nTop);
    }
    return nResult;
}

int KDolbyAxonScriptTable::LuaICEClient_GetDeviceList(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICEClient_device* pDevices = NULL;
    unsigned int nDevices = 0;

    KG_PROCESS_SUCCESS(pfnICEClient_GetDeviceList == NULL);

    nRetCode = lua_gettop(L) == 0;
    KGLOG_PROCESS_ERROR(nRetCode);

    nError = pfnICEClient_GetDeviceList(NULL, &nDevices);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

    pDevices = (ICEClient_device*)malloc(sizeof(ICEClient_device) * nDevices);
    KGLOG_PROCESS_ERROR(pDevices);

    nError = pfnICEClient_GetDeviceList(pDevices, &nDevices);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

    lua_newtable(L);

    for (unsigned int nIndex = 0; nIndex < nDevices; ++nIndex)
    {
        ICEClient_device& d = pDevices[nIndex];

        lua_pushnumber(L, nIndex + 1);
        lua_newtable(L);

        nRetCode = SetIceClientDevice(L, -2, pDevices + nIndex);
        KGLOG_PROCESS_ERROR(nRetCode);

        lua_settable(L, -3);
    }

    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    SAFE_FREE(pDevices);
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetDefaultDevice(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICEClient_device SpkDev = { 0 };
    ICEClient_device MicDev = { 0 };

    KG_PROCESS_SUCCESS(pfnICEClient_SetDefaultDevice == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetIceClientDevice(L, 1, &SpkDev);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetIceClientDevice(L, 2, &MicDev);
    KGLOG_PROCESS_ERROR(nRetCode);

    nError = pfnICEClient_SetDefaultDevice(&SpkDev, &MicDev);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_GetDefaultEngineType(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICECLIENT_DEVICE_TYPE nDeviceType = ICECLIENT_DEVICE_UNKNOWN;
    ICECLIENT_ENGINE_TYPE nEngineType = ICECLIENT_ENGINE_DEFAULT;
    LPCSTR szEngineName = NULL;

    KG_PROCESS_SUCCESS(pfnICEClient_GetDefaultEngineType == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    if (lua_isstring(L, 1))
        nDeviceType = IceClientDeviceType(lua_tostring(L, 1));
    else if (lua_isnumber(L, 1))
        nDeviceType = (ICECLIENT_DEVICE_TYPE)(int)lua_tonumber(L, 1);

    nEngineType = pfnICEClient_GetDefaultEngineType(nDeviceType);

    szEngineName = GetIceClientEngineTypeName(nEngineType);

    lua_pushstring(L, szEngineName);
    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_Version(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    LPCSTR szVersion = NULL;

    KG_PROCESS_SUCCESS(pfnICEClient_Version == NULL);

    nRetCode = lua_gettop(L) == 0;
    KGLOG_PROCESS_ERROR(nRetCode);

    szVersion = pfnICEClient_Version();
    KGLOG_PROCESS_ERROR(szVersion);

    lua_pushstring(L, szVersion);
    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_InterfaceVersion(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    LPCSTR szVersion = NULL;

    KG_PROCESS_SUCCESS(pfnICEClient_InterfaceVersion == NULL);

    nRetCode = lua_gettop(L) == 0;
    KGLOG_PROCESS_ERROR(nRetCode);

    szVersion = pfnICEClient_InterfaceVersion();
    KGLOG_PROCESS_ERROR(szVersion);

    lua_pushstring(L, szVersion);
    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_DestroyIceClient(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    ICECLIENT* pIceClient = NULL;

    KG_PROCESS_SUCCESS(pfnICEClient_DestroyIceClient == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    pfnICEClient_DestroyIceClient(pIceClient);

Exit1:
    nResult = true;
Exit0:
    return 0;
}

int KDolbyAxonScriptTable::LuaICEClient_Init(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ENGINE_TYPE nEngineType = ICECLIENT_ENGINE_DEFAULT;
    LPCSTR pszEngineType = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;

    KG_PROCESS_SUCCESS(pfnICEClient_Init == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    if (lua_isnumber(L, 2))
    {
        nEngineType = (ICECLIENT_ENGINE_TYPE)(int)lua_tonumber(L, 2);
    }
    else
    {
        pszEngineType = lua_tostring(L, 2);
        KGLOG_PROCESS_ERROR(pszEngineType);

        nEngineType = IceClientEngineType(pszEngineType);
    }

    nError = pfnICEClient_Init(pIceClient, nEngineType);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_GetCodecs(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    int nTableIndex = 1;
    unsigned char const* pucCodecs = NULL;

    KG_PROCESS_SUCCESS(pfnICEClient_GetCodecs == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    pucCodecs = pfnICEClient_GetCodecs(pIceClient);
    KGLOG_PROCESS_ERROR(pucCodecs);

    lua_newtable(L);

    for (; *pucCodecs != 0; ++pucCodecs)
    {
        lua_pushnumber(L, nTableIndex++);
        lua_pushnumber(L, *pucCodecs);
        lua_settable(L, -3);
    }

    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetCodec(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    unsigned char ucCodecs = 0;

    KG_PROCESS_SUCCESS(pfnICEClient_SetCodec == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    ucCodecs = (unsigned char)lua_tonumber(L, 2);

    nError = pfnICEClient_SetCodec(pIceClient, ucCodecs);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_AutoTick(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;

    KG_PROCESS_SUCCESS(pfnICEClient_AutoTick == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nError = pfnICEClient_AutoTick(pIceClient);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_Pause(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    int nOn = false;

    KG_PROCESS_SUCCESS(pfnICEClient_Pause == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nOn = lua_toboolean(L, 2);

    nError = pfnICEClient_Pause(pIceClient, nOn);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_GetStats(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICEClient_stat Stats = { 0 };

    KG_PROCESS_SUCCESS(pfnICEClient_GetStats == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    Stats = pfnICEClient_GetStats(pIceClient);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

    lua_newtable(L);

    lua_pushnumber(L, Stats.ping);
    lua_setfield(L, -2, "ping");

    lua_pushnumber(L, Stats.netin);
    lua_setfield(L, -2, "netin");

    lua_pushnumber(L, Stats.netout);
    lua_setfield(L, -2, "netout");

    lua_pushstring(L, Stats.gsname);
    lua_setfield(L, -2, "gsname");

    lua_pushstring(L, Stats.asname);
    lua_setfield(L, -2, "asname");

    lua_pushnumber(L, Stats.talking);
    lua_setfield(L, -2, "talking");

    lua_pushboolean(L, Stats.running);
    lua_setfield(L, -2, "running");

    lua_pushboolean(L, Stats.premium);
    lua_setfield(L, -2, "premium");

    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetVolume(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    float fVolume = 0.0f;

    KG_PROCESS_SUCCESS(pfnICEClient_SetVolume == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    fVolume = (float)lua_tonumber(L, 2);

    pfnICEClient_SetVolume(pIceClient, fVolume);

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetServer(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    LPCSTR szIp = NULL;
    unsigned short nPort = 0;

    KG_PROCESS_SUCCESS(pfnICEClient_SetServer == NULL);

    nRetCode = lua_gettop(L) == 3;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    szIp = lua_tostring(L, 2);
    nPort = (unsigned short)lua_tonumber(L, 3);

    nError = pfnICEClient_SetServer(pIceClient, szIp, nPort);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetName(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    LPCSTR szName = NULL;

    KG_PROCESS_SUCCESS(pfnICEClient_SetName == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    szName = lua_tostring(L, 2);

    pfnICEClient_SetName(pIceClient, szName);

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetPrivateID(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    unsigned int nPrivateID = 0;

    KG_PROCESS_SUCCESS(pfnICEClient_SetPrivateID == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nPrivateID = (unsigned int)lua_tonumber(L, 2);

    pfnICEClient_SetPrivateID(pIceClient, nPrivateID);

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetRotation(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nParamCount = 0;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    int nYaw = 0;
    int nPitch = 0;
    int nRoll = 0;

    KG_PROCESS_SUCCESS(pfnICEClient_SetRotation_npr == NULL);
    KG_PROCESS_SUCCESS(pfnICEClient_SetRotation == NULL);

    nParamCount = lua_gettop(L);
    KGLOG_PROCESS_ERROR(nParamCount == 2 || nParamCount == 4);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    if (nParamCount == 2)
    {
        nYaw = (int)lua_tonumber(L, 2);

        nError = pfnICEClient_SetRotation(pIceClient, nYaw);
        KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);
    }
    else
    {
        nYaw = (int)lua_tonumber(L, 2);
        nPitch = (int)lua_tonumber(L, 3);
        nRoll = (int)lua_tonumber(L, 4);

        nError = pfnICEClient_SetRotation_npr(pIceClient, nYaw, nPitch, nRoll);
        KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);
    }

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

static int GetIceClientRoomMode(ICECLIENT_ROOM_MODE* pMode, LPCSTR pszMode)
{
    int nResult = false;
    ICECLIENT_ROOM_MODE nMode = ICECLIENT_ROOM_NORMAL;

    KGLOG_PROCESS_ERROR(pMode);

    if (stricmp("normal", pszMode) == 0)
        nMode = ICECLIENT_ROOM_NORMAL;
    else if (stricmp("cavern", pszMode) == 0)
        nMode = ICECLIENT_ROOM_CAVERN;
    else if (stricmp("small", pszMode) == 0)
		nMode = ICECLIENT_ROOM_SMALL;

    *pMode = nMode;

    nResult = true;
Exit0:
    return nResult;
}

int KDolbyAxonScriptTable::LuaICEClient_SetRoomMode(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICECLIENT_ROOM_MODE nMode = ICECLIENT_ROOM_NORMAL;
    LPCSTR szMode = NULL;

    KG_PROCESS_SUCCESS(pfnICEClient_SetRoomMode == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    if (lua_isnumber(L, 2))
    {
        nMode = (ICECLIENT_ROOM_MODE)(int)lua_tonumber(L, 2);
    }
    else
    {
        szMode = lua_tostring(L, 2);
        KGLOG_PROCESS_ERROR(szMode);

        nRetCode = GetIceClientRoomMode(&nMode, szMode);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nError = pfnICEClient_SetRoomMode(pIceClient, nMode);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

static int GetIceClientVoiceFontPreset(ICECLIENT_VOICEFONT_PRESET* pPreset, LPCSTR pszPreset)
{
    int nResult = false;
    ICECLIENT_VOICEFONT_PRESET nPreset = ICECLIENT_VOICEFONT_NONE;

    KGLOG_PROCESS_ERROR(pPreset);

    if (stricmp("none", pszPreset) == 0)
        nPreset = ICECLIENT_VOICEFONT_NONE;
    else if (stricmp("w2m", pszPreset) == 0)
        nPreset = ICECLIENT_VOICEFONT_PRESET_W2M;
    else if (stricmp("m2w", pszPreset) == 0)
		nPreset = ICECLIENT_VOICEFONT_PRESET_M2W;
    else if (stricmp("elf", pszPreset) == 0)
		nPreset = ICECLIENT_VOICEFONT_PRESET_ELF;

    *pPreset = nPreset;

    nResult = true;
Exit0:
    return nResult;
}

int KDolbyAxonScriptTable::LuaICEClient_SetVoiceFont(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICECLIENT_VOICEFONT_PRESET nPreset = ICECLIENT_VOICEFONT_NONE;
    LPCSTR szPreset = NULL;

    KG_PROCESS_SUCCESS(pfnICEClient_SetVoiceFont == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    if (lua_isnumber(L, 2))
    {
        nPreset = (ICECLIENT_VOICEFONT_PRESET)(int)lua_tonumber(L, 2);
    }
    else
    {
        szPreset = lua_tostring(L, 2);
        KGLOG_PROCESS_ERROR(szPreset);

        nRetCode = GetIceClientVoiceFontPreset(&nPreset, szPreset);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nError = pfnICEClient_SetVoiceFont(pIceClient, nPreset);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetCustomVoiceFont(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICEClient_voicefont VoiceFont = { 0 };

    KG_PROCESS_SUCCESS(pfnICEClient_SetCustomVoiceFont == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    KGLOG_PROCESS_ERROR(lua_istable(L, -1));

    lua_getfield(L, -1, "pitchchange");
    if (!lua_isnil(L, -1))
        VoiceFont.pitchchange = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "timbrechange");
    if (!lua_isnil(L, -1))
        VoiceFont.timbrechange = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    nError = pfnICEClient_SetCustomVoiceFont(pIceClient, VoiceFont);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_TalkInto(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    int nOn = false;
    unsigned char nChannelMapping = 0;

    KG_PROCESS_SUCCESS(pfnICEClient_TalkInto == NULL);

    nRetCode = lua_gettop(L) == 3;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nOn = lua_toboolean(L, 2);
    nChannelMapping = (unsigned int)lua_tonumber(L, 3);

    pfnICEClient_TalkInto(pIceClient, nOn, nChannelMapping);

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_OnlyTalkInto(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    unsigned char* pnChannelMapping = NULL;
    unsigned int nChannelMappingCount = 0;

    KG_PROCESS_SUCCESS(pfnICEClient_OnlyTalkInto == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nRetCode = lua_istable(L, -1);
    KGLOG_PROCESS_ERROR(nRetCode);

    nChannelMappingCount = (unsigned int)lua_objlen(L, -1);
    KGLOG_PROCESS_ERROR(nChannelMappingCount > 0);

    pnChannelMapping = (unsigned char*)malloc(sizeof(unsigned char) * nChannelMappingCount);
    KGLOG_PROCESS_ERROR(pnChannelMapping);

    for (unsigned int nIndex = 0; nIndex < nChannelMappingCount; ++nIndex)
    {
        int nChannelMapping = 0;

        lua_pushnumber(L, nIndex + 1);
        lua_gettable(L, -2);

        KGLOG_PROCESS_ERROR(lua_isnumber(L, -1));

        nChannelMapping = (int)lua_tonumber(L, -1);
        KGLOG_PROCESS_ERROR(nChannelMapping >= 0);
        KGLOG_PROCESS_ERROR(nChannelMapping < 0xFF);

        pnChannelMapping[nIndex] = (unsigned char)nChannelMapping;
    }

    pfnICEClient_OnlyTalkInto(pIceClient, pnChannelMapping, (unsigned short)nChannelMappingCount);

Exit1:
    nResult = true;
Exit0:
    SAFE_FREE(pnChannelMapping);
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_MuteMic(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    int nOn = false;

    KG_PROCESS_SUCCESS(pfnICEClient_MuteMic == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nOn = lua_toboolean(L, 2);

    pfnICEClient_MuteMic(pIceClient, nOn);

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_Mute(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    int nOn = false;

    KG_PROCESS_SUCCESS(pfnICEClient_Mute == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nOn = lua_toboolean(L, 2);

    pfnICEClient_Mute(pIceClient, nOn);

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetDevice(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICEClient_device SpkDev = { 0 };
    ICEClient_device MicDev = { 0 };

    KG_PROCESS_SUCCESS(pfnICEClient_SetDevice == NULL);

    nRetCode = lua_gettop(L) == 3;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nRetCode = GetIceClientDevice(L, 2, &SpkDev);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = GetIceClientDevice(L, 3, &MicDev);
    KGLOG_PROCESS_ERROR(nRetCode);

    nError = pfnICEClient_SetDevice(pIceClient, &SpkDev, &MicDev);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_GetOutputDevice(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICEClient_device SpkDev = { 0 };

    KG_PROCESS_SUCCESS(pfnICEClient_GetOutputDevice == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nError = pfnICEClient_GetOutputDevice(pIceClient, &SpkDev);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

    lua_newtable(L);

    nRetCode = SetIceClientDevice(L, -1, &SpkDev);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_GetCaptureDevice(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICEClient_device MicDev = { 0 };

    KG_PROCESS_SUCCESS(pfnICEClient_GetCaptureDevice == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nError = pfnICEClient_GetCaptureDevice(pIceClient, &MicDev);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

    lua_newtable(L);

    nRetCode = SetIceClientDevice(L, -1, &MicDev);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

struct KIceClientSpkTestModeItem
{
    LPCSTR szName;
    ICECLIENT_SPKTEST_MODE nMode;
};

static KIceClientSpkTestModeItem s_IceClientSpkTestModeItems[] =
{
    { "voice_5_1", ICECLIENT_SPKTEST_VOICE_5_1 },
    { "noise_5_1", ICECLIENT_SPKTEST_NOISE_5_1 },
    { "noise_sweep", ICECLIENT_SPKTEST_NOISE_SWEEP },
    { NULL, ICECLIENT_SPKTEST_VOICE_5_1 },
};

static ICECLIENT_SPKTEST_MODE GetIceClientSpkTestMode(LPCSTR szName)
{
    ICECLIENT_SPKTEST_MODE nMode = ICECLIENT_SPKTEST_VOICE_5_1;

    KGLOG_PROCESS_ERROR(szName);

    for (KIceClientSpkTestModeItem* pItem = s_IceClientSpkTestModeItems; pItem->szName; ++pItem)
    {
        if (strcmp(pItem->szName, szName) == 0)
        {
            nMode = pItem->nMode;
            break;
        }
    }

Exit0:
    return nMode;
}

int KDolbyAxonScriptTable::LuaICEClient_StartSpkTest(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICECLIENT_SPKTEST_MODE nSpkTestMode = ICECLIENT_SPKTEST_VOICE_5_1;

    KG_PROCESS_SUCCESS(pfnICEClient_StartSpkTest == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    if (lua_isstring(L, 2))
        nSpkTestMode = GetIceClientSpkTestMode(lua_tostring(L, 2));
    else if (lua_isnumber(L, 2))
        nSpkTestMode = (ICECLIENT_SPKTEST_MODE)(int)lua_tonumber(L, 2);

    nError = pfnICEClient_StartSpkTest(pIceClient, nSpkTestMode, NULL, NULL);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_StopSpkTest(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;

    KG_PROCESS_SUCCESS(pfnICEClient_StopSpkTest == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nError = pfnICEClient_StopSpkTest(pIceClient);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_StartMicTestPhase1(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;

    KG_PROCESS_SUCCESS(pfnICEClient_StartMicTestPhase1 == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nError = pfnICEClient_StartMicTestPhase1(pIceClient);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

struct KIceClientMicTestModeItem
{
    LPCSTR szName;
    ICECLIENT_MICTEST_MODE nMode;
};

static KIceClientMicTestModeItem s_IceClientMicTestModeItems[] =
{
    { "loopback", ICECLIENT_MICTEST_LOOPBACK },
    { "to_server", ICECLIENT_MICTEST_TO_SERVER },
    { NULL, ICECLIENT_MICTEST_LOOPBACK },
};

static ICECLIENT_MICTEST_MODE GetIceClientMicTestMode(LPCSTR szName)
{
    ICECLIENT_MICTEST_MODE nMode = ICECLIENT_MICTEST_LOOPBACK;

    KGLOG_PROCESS_ERROR(szName);

    for (KIceClientMicTestModeItem* pItem = s_IceClientMicTestModeItems; pItem->szName; ++pItem)
    {
        if (strcmp(pItem->szName, szName) == 0)
        {
            nMode = pItem->nMode;
            break;
        }
    }

Exit0:
    return nMode;
}

int KDolbyAxonScriptTable::LuaICEClient_StartMicTestPhase2(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    ICECLIENT_MICTEST_MODE nMicTestMode = ICECLIENT_MICTEST_LOOPBACK;

    KG_PROCESS_SUCCESS(pfnICEClient_StartMicTestPhase2 == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    if (lua_isstring(L, 2))
        nMicTestMode = GetIceClientMicTestMode(lua_tostring(L, 2));
    else if (lua_isnumber(L, 2))
        nMicTestMode = (ICECLIENT_MICTEST_MODE)(int)lua_tonumber(L, 2);

    nError = pfnICEClient_StartMicTestPhase2(pIceClient, nMicTestMode, NULL, NULL);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_StopMicTest(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;

    KG_PROCESS_SUCCESS(pfnICEClient_StopMicTest == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nError = pfnICEClient_StopMicTest(pIceClient);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetEchoSupression(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    int nOn = false;

    KG_PROCESS_SUCCESS(pfnICEClient_SetEchoSupression == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nOn = lua_toboolean(L, 2);

    nError = pfnICEClient_SetEchoSupression(pIceClient, nOn);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_SetAudioLogging(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    int nOn = false;

    KG_PROCESS_SUCCESS(pfnICEClient_SetAudioLogging == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nOn = lua_toboolean(L, 2);

    nError = pfnICEClient_SetAudioLogging(pIceClient, nOn);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_DumpState(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;

    KG_PROCESS_SUCCESS(pfnICEClient_DumpState == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nError = pfnICEClient_DumpState(pIceClient);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_CreateStateDump(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    unsigned int nIndex = 0;
    int nIncludeAudio = false;

    KG_PROCESS_SUCCESS(pfnICEClient_CreateStateDump);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nIncludeAudio = lua_toboolean(L, 2);

    nIndex = pfnICEClient_CreateStateDump(pIceClient, nIncludeAudio);

    lua_pushnumber(L, nIndex);
    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_GetStateDumpFile(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;
    ICECLIENT_ERROR nError = ICECLIENT_ERROR_NONE;
    unsigned int nIndex = 0;
    char const* pszFilePath = NULL;

    KG_PROCESS_SUCCESS(pfnICEClient_GetStateDumpFile == NULL);

    nRetCode = lua_gettop(L) == 2;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    nIndex = (unsigned int)lua_tonumber(L, 2);

    nError = pfnICEClient_GetStateDumpFile(pIceClient, nIndex, &pszFilePath, NULL, NULL);
    KGLOG_PROCESS_ERROR(nError == ICECLIENT_ERROR_NONE);

    lua_pushstring(L, pszFilePath);
    nRetCount = 1;

Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        nRetCount += PushError(L, nError);
    }
    return nRetCount;
}

int KDolbyAxonScriptTable::LuaICEClient_ClearStateDump(lua_State* L)
{
    int nRetCode = false;
    int nResult = false;
    int nRetCount = 0;
    ICECLIENT* pIceClient = NULL;

    KG_PROCESS_SUCCESS(pfnICEClient_ClearStateDump == NULL);

    nRetCode = lua_gettop(L) == 1;
    KGLOG_PROCESS_ERROR(nRetCode);

    pIceClient = ToICECLIENT(L, 1);
    KGLOG_PROCESS_ERROR(pIceClient);

    pfnICEClient_ClearStateDump(pIceClient);

Exit1:
    nResult = true;
Exit0:
    return nRetCount;
}
