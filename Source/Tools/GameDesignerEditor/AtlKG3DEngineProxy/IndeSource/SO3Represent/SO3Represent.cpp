#include "stdafx.h"
#include "IKG3DSoundShell.h"
#include "IKGSO3WorldClient.h"
#include "./SO3Represent.h"

SO3Represent* g_pRL = NULL;

HANDLE SO3Represent::ms_hInstance = NULL;

SO3Represent::SO3Represent()
: m_fTime(0.0)
, m_fFrameTime(0.0)
, m_fFrameTimeLast(0.0)
, m_fFrameTimeStart(0.0)
, m_dwGameLoop(0)
, m_nStartLoop(0)
, m_p3DEngineManager(NULL)
, m_p3DModelManager(NULL)
, m_p3DUI(NULL)
, m_pSound(NULL)
, m_pSO3UI(NULL)
, m_pGameWorldUIHandler(NULL)
, m_pSO3World(NULL)
, m_pSO3WorldClient(NULL)
, m_hWnd(NULL)
{
    memset(&m_Option, 0, sizeof(m_Option));

#ifdef RL_TRACE_OPTION
    memset(&m_TraceOption, 0, sizeof(m_TraceOption));
    memset(&m_CurrentTraceOption, 0, sizeof(m_CurrentTraceOption));
#endif // RL_TRACE_OPTION

    memset(&m_OptimizationOption, 0, sizeof(m_OptimizationOption));
    memset(&m_CurrentOptimizationOption, 0, sizeof(m_CurrentOptimizationOption));

    memset(&m_TerrainLeanOption, 0, sizeof(m_TerrainLeanOption));
    memset(&m_CurrentTerrainLeanOption, 0, sizeof(m_CurrentTerrainLeanOption));

    InitOptimization(&m_Optimization);
}

SO3Represent::~SO3Represent()
{
	Exit();
}

void SO3Represent::Release()
{
	SAFE_DELETE(g_pRL);
}

BOOL SO3Represent::Init()
{
	int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    KG3D_GAME_INIT_CONTEXT InitContext;
    time_t time = 0;
    struct tm* tmWorld = NULL;

	m_pSound = Create3DSoundShell();
    KGLOG_CHECK_ERROR(m_pSound);

    nRetCode = m_FilePath.Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_TableList.Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = m_SceneMgr.Init();
    KGLOG_PROCESS_ERROR(nRetCode);

    time = m_pSO3WorldClient->GetWorldTime();

    tmWorld = localtime(&time);
    KGLOG_PROCESS_ERROR(tmWorld);

	ZeroMemory(&InitContext, sizeof(InitContext));
	InitContext.gameTime.dwInitTimeMS = ((tmWorld->tm_hour * 60 + tmWorld->tm_min) * 60 + tmWorld->tm_sec) * 1000;
	InitContext.gameTime.fBoostFactor = 1;

	InitContext.logicalConstance.logicalCellCmLength = LOGICAL_CELL_CM_LENGTH;
	InitContext.logicalConstance.regionGridHeight = REGION_GRID_HEIGHT;
	InitContext.logicalConstance.regionGridWidth = REGION_GRID_WIDTH;
	InitContext.logicalConstance.terrainMinHeight = TERRAIN_MIN_HEIGHT;
	InitContext.logicalConstance.terrainMaxHeight = TERRAIN_MAX_HEIGHT;
	InitContext.logicalConstance.cellLength = CELL_LENGTH;
	InitContext.logicalConstance.pointPerAltitude = POINT_PER_ALTITUDE;
	InitContext.logicalConstance.altitudeUnit = ALTITUDE_UNIT;
	InitContext.logicalConstance.invalidObstableGroup = INVALID_OBSTACLE_GROUP;

	hr = m_p3DEngineManager->InitGameContext(&InitContext);
	KGLOG_COM_PROCESS_ERROR(hr);

	nResult = true;
Exit0:
    if (!nResult)
	    Exit();
	return nResult;
}

void SO3Represent::Exit()
{
	m_p3DEngineManager = NULL;
	m_p3DModelManager = NULL;
	m_p3DUI = NULL;
	m_pSO3UI = NULL;
	m_pGameWorldUIHandler = NULL;

    m_SceneMgr.Exit();

    m_TableList.Exit();
    m_FilePath.Exit();

	SAFE_RELEASE(m_pSound);
}

IKSO3GameWorldRepresentHandler* SO3Represent::GetGameWorldRepresentHandler()
{
	return &m_GameWorldHandler;
}

IKSO3RepresentHandler* SO3Represent::GetRepresentHandelr()
{
	return &m_GameWorldHandler;
}


//------------------------------------------------------------------------------------------------------------------------
//BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
//{
//	SO3Represent::ms_hInstance = hModule;
//	return TRUE;
//}

extern "C" __declspec(dllexport) 
IKSO3RepresentInterface* CreateSO3Represent()
{
    g_pRL  = new(std::nothrow) SO3Represent;
    return g_pRL;
}
