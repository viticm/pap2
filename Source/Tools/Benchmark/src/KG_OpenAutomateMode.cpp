////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_OpenAutomateMode.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008-8-12 10:33:12
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <conio.h>
#include "KG_OpenAutomateMode.h"
#include "KG_NormalMode.h"
#include "OpenAutomate.h"
#include "Engine.h"

static TCHAR *s_strKG3DEngine = "KG3DENGINE";
class KG_OpenAutomateModeNotify : public IKG_RecPlayerNotify
{
public:
    KG_OpenAutomateModeNotify(void);
    ~KG_OpenAutomateModeNotify(void);

    virtual int OnPlayBegin(IKG3DEngineManager *piEngineManager, const char cszRecFileName[]);
    virtual int OnPlayEnd(IKG3DEngineManager *piEngineManager);
    virtual int OnFrameMove(IKG3DEngineManager *piEngineManager);
private:
    DWORD m_dwRenderFrameCount;
    DWORD m_dwStartTime;
    DWORD m_dwRunTime;
};

////////////////////////////////////////////////////////////////////////////////
KG_OpenAutomateModeNotify::KG_OpenAutomateModeNotify(void)
{
    m_dwRenderFrameCount = 0;
    m_dwStartTime = 0;
    m_dwRunTime = 0;
}

KG_OpenAutomateModeNotify::~KG_OpenAutomateModeNotify(void)
{
}
////////////////////////////////////////////////////////////////////////////////
int KG_OpenAutomateModeNotify::OnPlayBegin(IKG3DEngineManager *piEngineManager, const char cszRecFileName[])
{
    int nResult  = false;
    //int nRetCode = false;

    KGLOG_PROCESS_ERROR(piEngineManager);
    KG_USE_ARGUMENT(cszRecFileName);

    piEngineManager->SetOnPresentCallBackFunc(oaDisplayFrame);
    
    oaStartBenchmark();

    m_dwRenderFrameCount = 0;
    m_dwStartTime = ::GetTickCount();

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_OpenAutomateModeNotify::OnPlayEnd(IKG3DEngineManager *piEngineManager)
{
    int nResult  = false;
    //int nRetCode = false;
    oaValue Val;

    KGLOG_PROCESS_ERROR(piEngineManager);

    m_dwRunTime = ::GetTickCount() - m_dwStartTime;
    
    Val.Float =  m_dwRunTime * 100.0f / m_dwRenderFrameCount;
    oaAddResultValue("Score", OA_TYPE_FLOAT, &Val);    
    oaEndBenchmark();

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int KG_OpenAutomateModeNotify::OnFrameMove(IKG3DEngineManager *piEngineManager)
{
    int nResult  = false;
    //int nRetCode = false;

    KGLOG_PROCESS_ERROR(piEngineManager);
    
    ++m_dwRenderFrameCount;
    
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
struct CurrentOption
{
	DWORD dwResolution;
	//DWORD dwTextureQuality;
	//BOOL  bEnableGrass;
	//BOOL  bEnableMSAA;
	//BOOL  bEnableReflection;
	BOOL  bEnablePostRenderEffect;
	DWORD dwShadowType;
	CurrentOption()
	{
		memset(this, 0, sizeof(CurrentOption));
	}
};

static CurrentOption gs_CurrentOption;

enum enuOptionName
{
	OPTION_NAME_RESOLUTION,
	//OPTION_NAME_TEXTURE_QUALITY,
	//OPTION_NAME_ENABLE_GRASS,
	//OPTION_NAME_WATER_REFLECTION,
	//OPTION_NAME_MSAA,
	OPTION_NAME_POST_RENDER_EFFECT,
	OPTION_NAME_CSM_SHADOW_MAP,
	OPTION_NAME_COUNT,
	OPTION_NAME_FORCE_DWORD,
};

struct RESOLUTION
{
    char *pczName;
    unsigned uWidth;
    unsigned uHeight;
};

static RESOLUTION gs_Resolution[] =
{
 //   {"1024x768",  1024, 768 },
 //   {"1280x1024", 1280, 1024},
	//{"1600*1200", 1600, 1200},
	//{"1920x1200", 1920, 1200},
	//{"2560x1600", 2560, 1600},

	{"1280 x 1024", 1280, 1024},
	{"1440 x 900",  1440, 900 },
	{"1600 x 1200", 1600, 1200},
	{"1680 x 1050", 1680, 1050},
	{"1600 x 1024", 1600, 1024},
	{"1920 x 1200", 1920, 1200},
	{"2560 x 1600", 2560, 1600}
};

static DWORD gs_dwTextureScale[] = 
{
	1, 2, 4, 8,
};

//s_SetOptionName和s_SetOptionFunctionList需要一一对应
static TCHAR* gs_SetOptionName[] = 
{
	"Resolution",
	//"Texture Quality",
	//"Enable Grass",
	//"Enable Water Reflection",
	//"Enable MSAA",
	"Enable Post Render Effect",
	"Enable CSM shadow map"
};

void SetResolution(KG_RecPlayer *pPlayer, oaNamedOption *pOption);
//void SetTextureQuality(KG_RecPlayer *pPlayer, oaNamedOption *pOption);
//void SetEnableGrass(KG_RecPlayer *pPlayer, oaNamedOption *pOption);
//void SetEnableWaterReflection(KG_RecPlayer *pPlayer, oaNamedOption *pOption);
void SetEnablePostRenderEffect(KG_RecPlayer *pPlayer, oaNamedOption *pOption);
void SetEnableCSM(KG_RecPlayer *pPlayer, oaNamedOption *pOption);
//void SetEnableMSAA(KG_RecPlayer *pPlayer, oaNamedOption *pOption);

typedef void(*FP_SET_OPTION)(KG_RecPlayer *pPlayer, oaNamedOption *pOption);
static FP_SET_OPTION gs_SetOptionFunctionList[] = 
{
	SetResolution,
	//SetTextureQuality,
	//SetEnableGrass,
	//SetEnableWaterReflection,
	//SetEnableMSAA,
	SetEnablePostRenderEffect,
	SetEnableCSM
};

void SetResolution(KG_RecPlayer *pPlayer, oaNamedOption *pOption)
{
    for (size_t i = 0; i < sizeof(gs_Resolution) / sizeof(gs_Resolution[0]); i++)
    {
        if (strcmp(pOption->Value.String, gs_Resolution[i].pczName) == 0)
        {
            pPlayer->SetResolution(gs_Resolution[i].uWidth, gs_Resolution[i].uHeight);
            gs_CurrentOption.dwResolution = static_cast<DWORD>(i);
            break;
        }
    }
}

//void SetEnableMSAA(KG_RecPlayer *pPlayer, oaNamedOption *pOption)
//{
//	KG3DEngineOption Option;
//	pPlayer->GetEngineOption(&Option);
//	BOOL bEnableMSAA = static_cast<BOOL>(pOption->Value.Bool);
//}
//void SetTextureQuality(KG_RecPlayer *pPlayer, oaNamedOption *pOption)
//{
//    KG3DEngineOption Option;
//    pPlayer->GetEngineOption(&Option);
//    Option.nTextureScale = gs_dwTextureScale[pOption->Value.Int];
//    pPlayer->SetEngineOption(&Option);
//    gs_CurrentOption.dwTextureQuality = pOption->Value.Int;
//}
//
//void SetEnableGrass(KG_RecPlayer *pPlayer, oaNamedOption *pOption)
//{
//    KG3DEngineOption Option;
//    pPlayer->GetEngineOption(&Option);
//    Option.bRenderGrass = static_cast<BOOL>(pOption->Value.Bool);
//    pPlayer->SetEngineOption(&Option);
//    gs_CurrentOption.bEnableGrass = Option.bRenderGrass;
//}
//
//void SetEnableWaterReflection(KG_RecPlayer *pPlayer, oaNamedOption *pOption)
//{
//    KG3DEngineOption Option;
//    pPlayer->GetEngineOption(&Option);
//    Option.bWaterReflection = static_cast<BOOL>(pOption->Value.Bool);
//    pPlayer->SetEngineOption(&Option);
//    gs_CurrentOption.bEnableReflection = Option.bWaterReflection;
//}

void SetEnablePostRenderEffect(KG_RecPlayer *pPlayer, oaNamedOption *pOption)
{
    KG3DEngineOption Option;
    pPlayer->GetEngineOption(&Option);
    Option.bPostEffectEnable = static_cast<BOOL>(pOption->Value.Bool);
	if (Option.bPostEffectEnable)
	{
		Option.bDOF = TRUE;
		Option.bHDR = TRUE;
		Option.bGodRay = TRUE;
		Option.bShockWaveEnable = TRUE;
	}
	else
	{
		Option.bDOF = FALSE;
		Option.bHDR = FALSE;
		Option.bGodRay = FALSE;
		Option.bShockWaveEnable = FALSE;
	}
    pPlayer->SetEngineOption(&Option);
    gs_CurrentOption.bEnablePostRenderEffect = Option.bPostEffectEnable;
}

void SetEnableCSM(KG_RecPlayer *pPlayer, oaNamedOption *pOption)
{
    KG3DEngineOption Option;
    pPlayer->GetEngineOption(&Option);
	Option.ModelShadowType = pOption->Value.Int;
	pPlayer->SetEngineOption(&Option);
    gs_CurrentOption.dwShadowType= Option.ModelShadowType;
}

void On_SetOptions(KG_RecPlayer *pPlayer)
{
    oaNamedOption *Option;
    while((Option = oaGetNextOption()) != NULL)
    {
        for (DWORD i = 0; i < OPTION_NAME_COUNT; i++)
        {
            if (!strcmp(gs_SetOptionName[i], Option->Name))
            {
                (*gs_SetOptionFunctionList[i])(pPlayer, Option);
            }
        }
    }
}


void On_GetAllOptions(void)
{
    /* for each in-game setting available in the current environment */
    {
        oaNamedOptionStruct Option; 

        oaInitOption(&Option);
        Option.Name = gs_SetOptionName[OPTION_NAME_RESOLUTION];
        Option.DataType = OA_TYPE_ENUM;
        for (size_t i = 0; i < sizeof(gs_Resolution) / sizeof(gs_Resolution[0]); i++)
        {
            Option.Value.Enum = gs_Resolution[i].pczName;
            oaAddOption(&Option);
        }

        //oaInitOption(&Option);
        //Option.Name = gs_SetOptionName[OPTION_NAME_TEXTURE_QUALITY];
        //Option.DataType = OA_TYPE_INT;
        //Option.MinValue.Int = 0;
        //Option.MaxValue.Int = 3;
        //Option.NumSteps = 3;
        //oaAddOption(&Option);

        //oaInitOption(&Option);
        //Option.Name = gs_SetOptionName[OPTION_NAME_ENABLE_GRASS];
        //Option.DataType = OA_TYPE_BOOL;
        //oaAddOption(&Option);

        //oaInitOption(&Option);
        //Option.Name = gs_SetOptionName[OPTION_NAME_WATER_REFLECTION];
        //Option.DataType = OA_TYPE_BOOL;
        //oaAddOption(&Option);

		//oaInitOption(&Option)
		//Option.Name = gs_SetOptionName[OPTION_NAME_MSAA];
		//Option.DataType = OA_TYPE_BOOL;
		//oaAddOption(&Option);


        oaInitOption(&Option);
        Option.Name = gs_SetOptionName[OPTION_NAME_POST_RENDER_EFFECT];
        Option.DataType = OA_TYPE_BOOL;
        oaAddOption(&Option);

        oaInitOption(&Option);
        Option.Name = gs_SetOptionName[OPTION_NAME_CSM_SHADOW_MAP];
        Option.DataType = OA_TYPE_INT;
		Option.MinValue.Int = 1;
		Option.MaxValue.Int = 3;
		Option.NumSteps = 1;
        oaAddOption(&Option);
    }
}

void On_GetCurrentOptions(void)
{
    printf("GetCurrentOptions\n");
    oaValue Val;

    Val.Enum = gs_Resolution[gs_CurrentOption.dwResolution].pczName;
    oaAddOptionValue(gs_SetOptionName[OPTION_NAME_RESOLUTION], OA_TYPE_ENUM,  &Val);

    //Val.Int = gs_CurrentOption.dwTextureQuality;
    //oaAddOptionValue(gs_SetOptionName[OPTION_NAME_TEXTURE_QUALITY], OA_TYPE_INT, &Val);

    //Val.Bool = gs_CurrentOption.bEnableGrass ? OA_TRUE : OA_FALSE;
    //oaAddOptionValue(gs_SetOptionName[OPTION_NAME_ENABLE_GRASS], OA_TYPE_BOOL, &Val);
    //
    //Val.Bool = gs_CurrentOption.bEnableReflection ? OA_TRUE : OA_FALSE;
    //oaAddOptionValue(gs_SetOptionName[OPTION_NAME_WATER_REFLECTION], OA_TYPE_BOOL, &Val);
	//Val.Bool = gs_CurrentOption.bEnableMSAA ? OA_TRUE : OA_FALSE;
	//oaAddOptionValue(gs_SetOptionName[OPTION_NAME_MSAA], OA_TYPE_BOOL, &Val);
 //   
    Val.Bool = gs_CurrentOption.bEnablePostRenderEffect ? OA_TRUE : OA_FALSE;
    oaAddOptionValue(gs_SetOptionName[OPTION_NAME_POST_RENDER_EFFECT], OA_TYPE_BOOL, &Val);
    
    Val.Int = gs_CurrentOption.dwShadowType;
    oaAddOptionValue(gs_SetOptionName[OPTION_NAME_CSM_SHADOW_MAP], OA_TYPE_INT, &Val);
}

////////////////////////////////////////////////////////////////////////////////
void On_GetBenchmarks(void)
{
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FindData;


	const std::string strDir = "DebugFiles/BenchMark/";
	LPCTSTR strExt = "rec";

	TCHAR strRecordFilePath[MAX_PATH];
	{
		TCHAR strForFind[MAX_PATH] = ("");
		_stprintf_s(strForFind, _T("%s*.%s"), strDir.c_str(), strExt);
		hFindFile = ::FindFirstFile(strForFind, &FindData);
	}
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL, "Can't find any record file (*.rec)!", "Error", MB_OK | MB_ICONSTOP);
		KG_PROCESS_ERROR(false);
	}
	_stprintf_s(strRecordFilePath,"%s%s",strDir.c_str(), FindData.cFileName);
	oaAddBenchmark(strRecordFilePath);
	while (FindNextFile(hFindFile, &FindData) != 0)
	{
		_stprintf_s(strRecordFilePath,_T("%s%s"),strDir.c_str(),FindData.cFileName);		
		oaAddBenchmark(strRecordFilePath);
	}

    //hFindFile = ::FindFirstFile("*.rec", &FindData);
    //if (hFindFile == INVALID_HANDLE_VALUE)
    //{
    //    printf("Can't find any record file (*.rec)!\n");
    //    KG_PROCESS_ERROR(false);
    //}
    //oaAddBenchmark(FindData.cFileName);

    //while (FindNextFile(hFindFile, &FindData) != 0)
    //{
    //    oaAddBenchmark(FindData.cFileName);
    //}

Exit0:
    if (hFindFile)
    {
        ::FindClose(hFindFile);
        hFindFile = INVALID_HANDLE_VALUE;
    }
    return;
}

void InitCurrentOption(KG_RecPlayer *pPlayer)
{
 //   KG3DEngineOption Option;
 //   
 //   ASSERT(pPlayer);

 //   pPlayer->GetEngineOption(&Option);
 //   gs_CurrentOption.dwResolution = 0;
 //   gs_CurrentOption.dwTextureQuality = 0;
 //   for (DWORD  i = 0; i < sizeof(gs_dwTextureScale) / sizeof(gs_dwTextureScale[0]); i++)
 //   {
 //       if ((int)gs_dwTextureScale[i] == Option.nTextureScale)
 //       {
 //           gs_CurrentOption.dwTextureQuality = i;
 //       }
 //   }
	//gs_CurrentOption.dwShadowType = Option.ModelShadowType;
 //   gs_CurrentOption.bEnableGrass = Option.bRenderGrass;
 //   gs_CurrentOption.bEnableReflection = Option.bWaterReflection;
 //   gs_CurrentOption.bEnablePostRenderEffect = Option.bPostEffectEnable;
}
////////////////////////////////////////////////////////////////////////////////
int _NewConsole()
{
    int nResult  = false;
    int nRetCode = false;
    int fdConsoleHandle = -1;
    HANDLE hStdHandle = INVALID_HANDLE_VALUE;
    FILE *fpConsole = NULL;

    nRetCode = ::AllocConsole();
    KGLOG_PROCESS_ERROR(nRetCode);

    // redirect unbuffered STDOUT to the console
    hStdHandle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    KGLOG_PROCESS_ERROR(hStdHandle != INVALID_HANDLE_VALUE);

    fdConsoleHandle = _open_osfhandle((intptr_t)hStdHandle, O_TEXT);
    KGLOG_PROCESS_ERROR(fdConsoleHandle != -1);

    fpConsole = _fdopen(fdConsoleHandle, "w");
    KGLOG_PROCESS_ERROR(fpConsole);

    *stdout = *fpConsole;
    setvbuf( stdout, NULL, _IONBF, 0 );

    // redirect unbuffered STDIN to the console
    hStdHandle = ::GetStdHandle(STD_INPUT_HANDLE);
    KGLOG_PROCESS_ERROR(hStdHandle != INVALID_HANDLE_VALUE);

    fdConsoleHandle = _open_osfhandle((intptr_t)hStdHandle, O_TEXT);
    KGLOG_PROCESS_ERROR(fdConsoleHandle != -1);

    fpConsole = _fdopen(fdConsoleHandle, "r");
    KGLOG_PROCESS_ERROR(fpConsole);

    *stdin = *fpConsole;
    setvbuf(stdin, NULL, _IONBF, 0);

    // redirect unbuffered STDERR to the console
    hStdHandle = ::GetStdHandle(STD_ERROR_HANDLE);
    KGLOG_PROCESS_ERROR(hStdHandle != INVALID_HANDLE_VALUE);

    fdConsoleHandle = _open_osfhandle((intptr_t)hStdHandle, O_TEXT);
    KGLOG_PROCESS_ERROR(fdConsoleHandle != -1);

    fpConsole = _fdopen(fdConsoleHandle, "w");
    KGLOG_PROCESS_ERROR(fpConsole);

    *stderr = *fpConsole;
    setvbuf(stderr, NULL, _IONBF, 0);

    nResult = true;
Exit0:
    return nResult;
}

void InitCofigFile()
{
	HRESULT hResult = E_FAIL;
	KSmartIniFile pFile ;
	const char CONFIG_FILE_NAME[] = "config.ini";
	pFile = g_OpenIniFile(CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(pFile);
	pFile->WriteInteger("KG3DENGINE", "MaxAnisotropy", 16);
	pFile->WriteFloat("KG3DENGINE", "CammeraDistance", 80000.000000);
	pFile->WriteInteger("KG3DENGINE", "RenderGrass", 1);
	pFile->WriteInteger("KG3DENGINE", "WaterRefraction", 1);
	pFile->WriteInteger("KG3DENGINE", "WaterReflection",         1);
	pFile->WriteInteger("KG3DENGINE", "WaterReflectEnvironment", 1);
	pFile->WriteInteger("KG3DENGINE", "WaterReflectGround",      1);
	pFile->WriteInteger("KG3DENGINE", "WaterReflectNPC",         1);
	pFile->WriteInteger("KG3DENGINE", "WaterReflectEntity",      1);
	pFile->WriteInteger("KG3DENGINE", "TextureScale",      1);

	pFile->WriteInteger("KG3DENGINE", "GrassAnimation",      1);
	pFile->WriteInteger("KG3DENGINE", "VegetationDensity",      1);
	pFile->WriteInteger("KG3DENGINE", "VSync",      0);
	pFile->WriteInteger("KG3DENGINE", "WaterDetail",      4);
	pFile->WriteInteger("KG3DENGINE", "EnablePhysics",      1);
	pFile->WriteInteger("ENGINEOPTION", "b3DVision",      1);
	pFile->Save(CONFIG_FILE_NAME);
Exit0:
	return;
}

////////////////////////////////////////////////////////////////////////////////
int RunAsOpenAutomateMode(const char cszOption[])
{
    int nResult  = false;
    int nRetCode = false;
    int nLoopFlag = false;
    oaCommand Command;
    oaVersion Version;
    KG_OpenAutomateModeNotify NotifyProc;
    KG_RecPlayer Player;
    int nPlayerInitFlag = false;
	IIniFile * pConfig = NULL;
	int nResolution = 0;

    KGLOG_PROCESS_ERROR(cszOption);

    //nRetCode = _NewConsole();
    //KGLOG_PROCESS_ERROR(nRetCode);
    InitCofigFile();
    nRetCode = oaInit(cszOption, &Version);
    if (!nRetCode)
    {
        fprintf(stderr, "ERROR: OA did not initialize properly.\n");
        KG_PROCESS_ERROR(false);
    }

	pConfig = g_OpenIniFile("config.ini");
	KG_PROCESS_ERROR(pConfig);

	pConfig->GetInteger("kg3dengine", "resolution", 0, &nResolution);

	SAFE_RELEASE(pConfig);

	gs_CurrentOption.dwResolution = static_cast<DWORD>(nResolution);
	
	nRetCode = Player.Init(0, 
		0, 
		gs_Resolution[gs_CurrentOption.dwResolution].uWidth, 
		gs_Resolution[gs_CurrentOption.dwResolution].uHeight, 
		0);
    KGLOG_PROCESS_ERROR(nRetCode);
    nPlayerInitFlag = true;

    nLoopFlag = true;
    while(nLoopFlag)
    {
        switch(oaGetNextCommand(&Command))
        {
            /* No more commands, exit program */
        case OA_CMD_EXIT: 
			nLoopFlag = false;
            break;

            /* Run as normal */
        case OA_CMD_RUN:
            if (nPlayerInitFlag)
            {
                Player.UnInit();
                nPlayerInitFlag = false;
            }
            RunAsNormalMode();
            nLoopFlag = false;
            break;

            /* Enumerate all in-game options */
        case OA_CMD_GET_ALL_OPTIONS: 
            On_GetAllOptions();
            break;

            /* Return the option values currently set */
        case OA_CMD_GET_CURRENT_OPTIONS:
            On_GetCurrentOptions();
            break;

            /* Set all in-game options */
        case OA_CMD_SET_OPTIONS: 
            On_SetOptions(&Player);
            break;

            /* Enumerate all known benchmarks */
        case OA_CMD_GET_BENCHMARKS: 
            On_GetBenchmarks();
            break;

            /* Run benchmark */
        case OA_CMD_RUN_BENCHMARK: 
            nRetCode = Player.Play(Command.BenchmarkName, &NotifyProc);
            if (nRetCode)
                printf("Run benchmark %s\n", Command.BenchmarkName);
            else
                printf("Run benchmark %s failed.\n", Command.BenchmarkName);

            break;
        }
    }

	pConfig = g_OpenIniFile("config.ini", true, true);
	KG_PROCESS_ERROR(pConfig);

	pConfig->WriteInteger("kg3dengine", "resolution", static_cast<int>(gs_CurrentOption.dwResolution));

	pConfig->Save("config.ini");

    nResult = true;
Exit0:
    if (nPlayerInitFlag)
    {
        Player.UnInit();
        nPlayerInitFlag = false;
    }

	SAFE_RELEASE(pConfig);
    return nResult;
}
