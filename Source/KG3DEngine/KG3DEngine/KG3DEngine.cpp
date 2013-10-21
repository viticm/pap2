// KG3DEngine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "KG3DEngine.h"
#include "KG3DSceneSceneEditor.h"
#include "IEKG3DSceneCamera.h"
#include ".\kg3denginemanager.h"
#include "KG3DFileLoader.h"
#include "KG3DMaterialTable.h"
#include "KG3DModelTable.h"
#include "KG3DMeshTable.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KMemory.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern KG3DEngineManager g_cEngineManager;         // 引擎管理器

TCHAR g_szModuleFilePath[MAX_PATH];
TCHAR g_szConfigFilePath[MAX_PATH];

////////////////////////////////////////////////////////////////////////////////

#ifdef _MANAGED
#pragma managed(push, off)
#endif


static int GetDllFilePath(HMODULE hModule)
{
    int nRetCode = false;
    int nResult = false;
    LPTSTR pszDelimiter = NULL;

    /*nRetCode = GetModuleFileName(hModule, g_szModuleFilePath, _countof(g_szModuleFilePath));
    KGLOG_PROCESS_ERROR(nRetCode);*/

	GetCurrentDirectoryA(MAX_PATH, g_szModuleFilePath);
	strcat(g_szModuleFilePath, "\\");

    pszDelimiter = _tcsrchr(g_szModuleFilePath, _T('\\'));
    KGLOG_PROCESS_ERROR(pszDelimiter);

    KGLOG_PROCESS_ERROR(pszDelimiter + 1 != g_szModuleFilePath + _countof(g_szModuleFilePath));

    *(pszDelimiter + 1) = _T('\0');

    nResult = true;
Exit0:
    return nResult;
}

static int InitConfigFilePath(LPTSTR szConfigFilePath, size_t nConfigFilePathBufferSize, LPCTSTR szModuleFilePath)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(szConfigFilePath);
    KGLOG_PROCESS_ERROR(szModuleFilePath);

    nRetCode = _sntprintf_s(szConfigFilePath, nConfigFilePathBufferSize, _TRUNCATE, _T("%sconfig.ini"), szModuleFilePath) > 0;
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

//BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
//{
//    int nRetCode = false;
//    int nResult = false;
//
//	switch (ul_reason_for_call)
//	{
//	case DLL_PROCESS_ATTACH:
//        nRetCode = GetDllFilePath(hModule);
//        KGLOG_PROCESS_ERROR(nRetCode);
//
//        nRetCode = InitConfigFilePath(g_szConfigFilePath, _countof(g_szConfigFilePath), g_szModuleFilePath);
//        KGLOG_PROCESS_ERROR(nRetCode);
//
//        break;
//	case DLL_THREAD_ATTACH:
//	case DLL_THREAD_DETACH:
//	case DLL_PROCESS_DETACH:
//		break;
//	}
//
//	nResult = true;
//Exit0:
//    return nResult;
//}

#ifdef _MANAGED
#pragma managed(pop)
#endif

//DLLKG3DENGINE_API HRESULT Get3DEngineInterface(void** ppEngineManager)
//{
//	*ppEngineManager = &g_cEngineManager;
//	return S_OK;
//}

////////////////////////////////////////////////////////////////////////////////
//             Test Code for Engine Performance Analyze
//                              ------ By Wu Caizhong
////////////////////////////////////////////////////////////////////////////////
#ifdef KG3D_DEV_TEST

#include <Psapi.h>
#include "KG3DStateBlock.h"

HWND g_hWnd = NULL;

#define MODEL_COUNT_PER_LINE 10
#define LINE_COUNT 5
const float X_DELTA = 100.0f;
const float Z_DELTA = 1000.0f;
#define MODEL_COUNT_PER_GROUP (MODEL_COUNT_PER_LINE * LINE_COUNT)

enum MODEL_GROUP
{
    GROUP_FRONT = 0,
    GROUP_BACK,
    GROUP_TOTAL
};

const char *g_cszModelFile[] = 
{
    "data\\source\\player\\F2\\部件\\Mdl\\F2.mdl",
    "data\\source\\player\\F2\\部件\\Mdl\\NPC_F2_chunyang2601.mdl",
    "data\\source\\player\\F2\\部件\\Mdl\\NPC_F2_粉衣3003.mdl",
    "data\\source\\player\\F2\\部件\\Mdl\\NPC_F2_红衣3008.mdl",
    "data\\source\\player\\F2\\部件\\Mdl\\NPC_F2_混搭.mdl",
    "data\\source\\player\\F2\\部件\\Mdl\\NPC_F2_绿衣3007.mdl",
    "data\\source\\player\\F2\\部件\\Mdl\\NPC_qixiu_2501.mdl",
    "data\\source\\player\\F2\\部件\\Mdl\\NPC_yangzhou_红兰.mdl"
};
const int MODEL_FILE_COUNT = sizeof(g_cszModelFile) / sizeof(g_cszModelFile[0]);

const char *g_cszAniFile[] = 
{
    "data\\source\\player\\F2\\动作\\F2H普通待机01.ani",
    "data\\source\\player\\F2\\动作\\F2b01aq攻击21.ani",
    "data\\source\\player\\F2\\动作\\F2b01aq攻击22.ani",
    "data\\source\\player\\F2\\动作\\F2b02yd奔跑.ani",
    "data\\source\\player\\F2\\动作\\F2b02yd二段跳a.ani"
};
const int ANI_FILE_COUNT = sizeof(g_cszAniFile) / sizeof(g_cszAniFile[0]);

typedef std::vector<KG3DModel *> MODEL_ARRAY;
MODEL_ARRAY g_ModelArray;


// 以下两变量作为Model资源回收的标志位
int g_nAddModelFront = false;
int g_nAddModelBack = false;
int g_nAddModelAniFront = false;
int g_nAddModelAniBack = false;
int g_nCurSceneID = 0;

// 测试
const D3DXVECTOR3 g_cCameraInitPos(6251.19f, -200.15f, 4459.35f);
const D3DXVECTOR3 g_cLookUpInitPos(5757.33f, -100.25f, 5351.45f);
const char SCENE_FILE_NAME[]="data\\source\\maps\\测试\\测试1.Map";

// 稻香村
//const D3DXVECTOR3 g_cCameraInitPos(25834.0f, 1366.0f, 46305.0f);
//const D3DXVECTOR3 g_cLookUpInitPos(26063.0f, 1366.0f, 49398.0f);
//const char SCENE_FILE_NAME[]="data\\source\\maps\\稻香村\\稻香村.map";

// 扬州
//const D3DXVECTOR3 g_cCameraInitPos(94998.0f, 396.0f, 33139.0f);
//const D3DXVECTOR3 g_cLookUpInitPos(93699.0f, 496.0f, 30323.0f);
//const char SCENE_FILE_NAME[]="data\\source\\maps\\扬州\\扬州.map";

//////////////////////////////////////////////////////////////////////////
int InitModelArray();
void UnintModelArray();
int AddModel(
     IKG3DScene *piScene, 
     const D3DXVECTOR3& CameraPos, 
     const D3DXVECTOR3& LookAtPos, 
     int nGroupIndex
);
int DeleteModel(IKG3DScene *piScene, int nGroupIndex);
int AddModelAni(int nGroupIndex);
int DeleteModelAni(int nGroupIndex);
int ProcessKeyboardMessage(UINT_PTR uKeyValue);
////////////////////////////////////////////////////////////////////////////////

struct PROCESS_INTO
{
    DWORD dwTickCount;   // In millisecond
    DWORD dwKernelTime; // In millisecond
    DWORD dwUserTime;   // In millisecond
    DWORD dwPageFileSize;
    DWORD dwReadOperationCount;
    DWORD dwOtherOperationCount;
};

////////////////////////////////////////////////////////////////////////////////
INT GetProcessInfo(PROCESS_INTO *pProcessInfo)
{
    INT nResult  = E_FAIL;
    INT nRetCode  = FALSE;
    FILETIME CreateTime;
    FILETIME ExitTime;
    FILETIME KernelTime;
    FILETIME UserTime;
    ULARGE_INTEGER ullValue;
    PROCESS_MEMORY_COUNTERS ProcessMemCounter;
    IO_COUNTERS IOCounter;
    HANDLE hProcess = INVALID_HANDLE_VALUE;


    KGLOG_PROCESS_ERROR(pProcessInfo);

    hProcess = ::GetCurrentProcess();

    pProcessInfo->dwTickCount = ::GetTickCount();

    nRetCode = ::GetProcessTimes(hProcess, &CreateTime, &ExitTime, &KernelTime, &UserTime);
    KGLOG_PROCESS_ERROR(nRetCode);

    ullValue.HighPart = KernelTime.dwHighDateTime;
    ullValue.LowPart  = KernelTime.dwLowDateTime;
    pProcessInfo->dwKernelTime = (DWORD)(ullValue.QuadPart / 10000);

    ullValue.HighPart = UserTime.dwHighDateTime;
    ullValue.LowPart  = UserTime.dwLowDateTime;
    pProcessInfo->dwUserTime   = (DWORD)(ullValue.QuadPart / 10000);

    nRetCode = ::GetProcessMemoryInfo(hProcess, &ProcessMemCounter, sizeof(ProcessMemCounter));
    KGLOG_PROCESS_ERROR(nRetCode);

    pProcessInfo->dwPageFileSize = (DWORD)ProcessMemCounter.PagefileUsage;

    nRetCode = ::GetProcessIoCounters(hProcess, &IOCounter);
    KGLOG_PROCESS_ERROR(nRetCode);

    pProcessInfo->dwReadOperationCount = (DWORD)(IOCounter.ReadOperationCount);
    pProcessInfo->dwOtherOperationCount = (DWORD)(IOCounter.OtherOperationCount);

    nResult = true;
Exit0:
    return nResult;
}

INT ShowProcessInfoDiff(
    const char cszOperation[],
    DWORD dwROperationCount,
    const PROCESS_INTO &crPro1,
    const PROCESS_INTO &crPro2
)
{
    INT nResult  = false;
    INT nRetCode = false;
    char szMsg[256];

    KGLOG_PROCESS_ERROR(cszOperation);

    nRetCode = snprintf(
        szMsg, sizeof(szMsg) - 1,
        "\r\n"
        "%s(%u)所用资源(增量)：\r\n"
        "平均速度\t运行时间(s)\t用户CPU(s)\t内核CPU(s)\t总CPU/运行时间\t内存使用(MB)\t读IO\t其它IO\r\n"
        "%.1f\t\t%.3f\t\t%.3f\t\t%.3f\t\t%02.1f%%\t\t%u\t\t%u\t\t%u\t%u\r\n",
        cszOperation,
        dwROperationCount,
        dwROperationCount * 1000.0 / (crPro2.dwTickCount  - crPro1.dwTickCount),
        (crPro2.dwTickCount  - crPro1.dwTickCount) / 1000.0,
        (crPro2.dwUserTime   - crPro1.dwUserTime) / 1000.0,
        (crPro2.dwKernelTime - crPro1.dwKernelTime) / 1000.0,
        (
            ((crPro2.dwKernelTime + crPro2.dwUserTime) - (crPro1.dwKernelTime + crPro1.dwUserTime)) * 100.0f /
            (crPro2.dwTickCount - crPro1.dwTickCount)
        ),
        ((crPro2.dwPageFileSize - crPro1.dwPageFileSize) / 1024 / 1024),
        (crPro2.dwReadOperationCount  - crPro1.dwReadOperationCount),
        (crPro2.dwOtherOperationCount - crPro1.dwOtherOperationCount)
    );
    ASSERT(nRetCode);
    szMsg[sizeof(szMsg) - 1] = '\0';

    KGLogPrintf(KGLOG_INFO, "%s", szMsg);    
    //::MessageBox(g_hWnd, szMsg, "按确定键继续", MB_OK);

    nResult = true;
Exit0:
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int GetFileList(const char szPath[], const char szExtName[], vector<string> *pvecFileList)
{
    int nResult  = false;
    int nRetCode = false;
    FILE *fpFile = NULL;
    char szListFileName[MAX_PATH];
    char szFileName[MAX_PATH];
    char szCMD[MAX_PATH];
    int nNeedCreateListFile = true;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    ASSERT(szPath);
    ASSERT(szExtName);
    ASSERT(pvecFileList);
    
    nRetCode = snprintf(szListFileName, _countof(szListFileName), "temp_FileList.%s", szExtName);
    ASSERT(nRetCode > 0);
    ASSERT(nRetCode < _countof(szListFileName));

    nRetCode = g_IsFileExist(szListFileName);
    if (nRetCode)
    {
        nRetCode = ::MessageBox(NULL, "文件列表已存在，点“是”直接使用上次结果，点“否”重新扫描", "文件列表已存在", MB_YESNO);
        if (nRetCode == IDYES)
            nNeedCreateListFile = false;
    }

    if (nNeedCreateListFile)
    {
        char szMessage[256];
        snprintf(
            szMessage, sizeof(szMessage), 
            "搜索 %s\\*.%s,可能要很久（二十分钟左右），请耐心等待……", 
            szPath, szExtName
        );
        szMessage[sizeof(szMessage) - 1] = '\0';

        ::MessageBox(NULL, szMessage, "提示", MB_OK);

        nRetCode = snprintf(szCMD, sizeof(szCMD), "dir \"%s\\*.%s\" /s /b > %s", szPath, szExtName, szListFileName);
        ASSERT(nRetCode > 0);
        ASSERT(nRetCode < sizeof(szCMD));

        nRetCode = system(szCMD);
        KGLOG_PROCESS_ERROR(nRetCode == 0);
    }
    
    pvecFileList->reserve(10000);
    fpFile = fopen(szListFileName, "r");
    KGLOG_PROCESS_ERROR(fpFile);

    while (true)
    {
        char *pRetCode = fgets(szFileName, sizeof(szFileName) - 1, fpFile);
        if (!pRetCode)
            break;
        szFileName[sizeof(szFileName) - 1] = '\0';

        size_t uLen = strlen(szFileName);
        ASSERT(uLen > 1);
        size_t i = uLen - 1;
        while (szFileName[i] == '\r' || szFileName[i] == '\n')
        {
            ASSERT(i > 0);
            --i;
        }
        szFileName[i + 1] = '\0';

        pvecFileList->push_back(szFileName);
    }

    nResult = true;
Exit0:
    ASSERT(hFile == INVALID_HANDLE_VALUE);
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    //remove(LIST_FILE_NAME);
    return nResult;
}
typedef int (*PROCESS_FUNCTION)(const char cszFileName[]);
////////////////////////////////////////////////////////////////////////////////
int TestLoadModel(const char szFileName[])
{
    int nResult  = false;
    HRESULT hrRetCode = E_FAIL;
    IKG3DModel *piModel = NULL;

    hrRetCode = g_cModelTable.LoadResourceFromFile(szFileName, 0, 0, (IKG3DResourceBase**)&piModel);
    KG_COM_PROCESS_ERROR(hrRetCode);
    
    g_LoadPostProcessor.FrameMove();
    
    nResult = true;
Exit0:
    KG_COM_RELEASE(piModel);
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int TestLoadMesh(const char szFileName[])
{
	int nResult  = false;
	HRESULT hrRetCode = E_FAIL;
	KG3DMesh *pMesh = NULL;

	hrRetCode = g_cMeshTable.LoadResourceFromFile(szFileName, 0, 0, (IKG3DResourceBase**)&pMesh);
	KG_COM_PROCESS_ERROR(hrRetCode);

	nResult = true;
Exit0:
	KG_COM_RELEASE(pMesh);
	return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int TestLoadMaterial(const char cszFileName[])
{
    int nResult  = false;
    HRESULT hrRetCode = E_FAIL;
    IKG3DModel *piMaterial = NULL;

    ASSERT(cszFileName);

    hrRetCode = g_cMaterialTable.LoadResourceFromFile(cszFileName, 0, 0, (IKG3DResourceBase**)&piMaterial);
    KG_COM_PROCESS_ERROR(hrRetCode);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piMaterial);
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int ProcessForAllFile(
    const char cszProcessName[],
    const char szPath[], 
    const char cszExtName[], 
    PROCESS_FUNCTION pfFunction
)
{
    int nResult  = false;
    int nRetCode = false;
    vector<string> vecFileList;
    char szMessage[256];
    unsigned uTotalCount = 0;
    unsigned uFailedCount = 0;
    unsigned uExceptionCount = 0;
    PROCESS_INTO Begin, End;
    MSG msg;

    ASSERT(cszProcessName);
    ASSERT(szPath);
    ASSERT(cszExtName);
    ASSERT(pfFunction);
    
    ::SetWindowLong(g_hWnd, GWL_STYLE, WS_CAPTION | WS_SYSMENU | WS_VISIBLE);
    ::SetWindowPos(g_hWnd, NULL, 300, 300, 500, 200, SWP_FRAMECHANGED);

    nRetCode = GetFileList(szPath, cszExtName, &vecFileList);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    GetProcessInfo(&Begin);
    uTotalCount = (unsigned)vecFileList.size();
    for (unsigned i = 0; i < uTotalCount; ++i)
    {
        try
        {
            nRetCode = pfFunction(vecFileList[i].c_str());
            if (!nRetCode)
            {
                KGLogPrintf(KGLOG_ERR, "[%s] failed:%s\r\n\r\n", cszProcessName, vecFileList[i].c_str());
                ++uFailedCount;
            }
        }
        catch (...)
        {
            KGLogPrintf(KGLOG_ERR, "[%s] Exception:%s\r\n\r\n", cszProcessName, vecFileList[i].c_str());
            ++uExceptionCount;
            ++uFailedCount;
        }

        if ((i + 1) % 10 == 0)
        {
            snprintf(
                szMessage, sizeof(szMessage), 
                "%s：%3u/%3u 发现错误：%u (其中抛出异常 %u)", 
                cszProcessName, i + 1, uTotalCount, uFailedCount, uExceptionCount
            );
            szMessage[sizeof(szMessage) - 1] = '\0';
            SetWindowText(g_hWnd, szMessage);
        }

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            switch (msg.message)
            {
            case WM_QUIT:
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessage(&msg);     
            }   
        }
        if (msg.message == WM_QUIT)
            break;
    }
    GetProcessInfo(&End);
    ShowProcessInfoDiff(cszProcessName, uTotalCount, Begin, End);

     snprintf(
        szMessage, sizeof(szMessage), 
        "%s：共处理 %u 个 发现错误 %u 个(其中抛出异常 %u 个)，请检查日志获得出错的资源文件名", 
        cszProcessName, uTotalCount, uFailedCount, uExceptionCount
    );
    szMessage[sizeof(szMessage) - 1] = '\0';
    ::MessageBox(g_hWnd, szMessage, "报告", MB_OK);
   
    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int TestTexture(const char cszPath[])
{
    int nResult  = false;
    int nRetCode = false;
    vector<string> vecFileList;
    unsigned uTotalCount = 0;
    IKG_Buffer *piFileBuf = NULL;
    IDirect3DTexture9 *piTexture = NULL;
    LARGE_INTEGER ullBeginTime = {0};
    LARGE_INTEGER ullEndTime = {0};
    LARGE_INTEGER ullLoadTime = {0};
    LARGE_INTEGER ullCheckTime = {0};
    LARGE_INTEGER ullCreateTime = {0};
    LARGE_INTEGER ullFrequence = {0};
    PROCESS_INTO ProInfoBegin;
    PROCESS_INTO ProInfoEnd;

    QueryPerformanceFrequency(&ullFrequence);
    
    nRetCode = GetFileList(cszPath, "*.*", &vecFileList);
    KGLOG_PROCESS_ERROR(nRetCode);

    GetProcessInfo(&ProInfoBegin);
    uTotalCount = (unsigned)vecFileList.size();
    for (size_t i = 0; i < uTotalCount; ++i)
    {
        unsigned uFileSize = 0;
        void    *pvFileData = NULL;
        HRESULT hrRetCode = E_FAIL;

        QueryPerformanceCounter(&ullBeginTime);
        nRetCode = g_IsFileExist(vecFileList[i].c_str());
        QueryPerformanceCounter(&ullEndTime);
        ullCheckTime.QuadPart += ullEndTime.QuadPart - ullBeginTime.QuadPart;

        QueryPerformanceCounter(&ullBeginTime);
        nRetCode = g_FileLoader.LoadImmediately(vecFileList[i].c_str(), &piFileBuf);
        KGLOG_PROCESS_ERROR(nRetCode);
        ASSERT(piFileBuf);
        QueryPerformanceCounter(&ullEndTime);
        ullLoadTime.QuadPart += ullEndTime.QuadPart - ullBeginTime.QuadPart;

        uFileSize  = piFileBuf->GetSize();
        pvFileData = piFileBuf->GetData();

        QueryPerformanceCounter(&ullBeginTime);
        hrRetCode = D3DXCreateTextureFromFileInMemoryEx(
            g_pd3dDevice, pvFileData, uFileSize,
            0, 0, 0,
            0, D3DFMT_FROM_FILE, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
            0, NULL,NULL,
            &piTexture
        );
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
        QueryPerformanceCounter(&ullEndTime);
        ullCreateTime.QuadPart += ullEndTime.QuadPart - ullBeginTime.QuadPart;
       
        KG_COM_RELEASE(piTexture);

        KG_COM_RELEASE(piFileBuf);
    }
    GetProcessInfo(&ProInfoEnd);
    ShowProcessInfoDiff(cszPath, uTotalCount, ProInfoBegin, ProInfoEnd);

    KGLogPrintf(
        KGLOG_INFO,
        "%s,CheckTime=%.3fs, LoadTime=%.3fs, CreateTime=%.3fs", 
        cszPath,
        ullCheckTime.QuadPart  / (double)ullFrequence.QuadPart,
        ullLoadTime.QuadPart   / (double)ullFrequence.QuadPart,
        ullCreateTime.QuadPart / (double)ullFrequence.QuadPart
    );
    nResult = true;
Exit0:
    KG_COM_RELEASE(piTexture);
    KG_COM_RELEASE(piFileBuf);
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////
int LoadScene(IKG3DScene **ppiRetScene)
{
    int nResult = false;
    int nRetCode = false;
    HRESULT hRetCode = E_FAIL;
    PROCESS_INTO ProInfoLast;
    PROCESS_INTO ProInfoCur;
    int nOutputWindowID= 0;
    IKG3DScene *piScene = NULL;
    IKG3DCamera *piCamera = NULL;

    hRetCode = g_cEngineManager.NewOneScene(SCENETYPE_SCENEEDITOR, &piScene);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    // ------------------------------ load ------------------------------
    nRetCode = GetProcessInfo(&ProInfoCur);
    KGLOG_PROCESS_ERROR(nRetCode);
    ProInfoLast = ProInfoCur;

    hRetCode = piScene->LoadFromFile(SCENE_FILE_NAME, 0, 0);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = ((KG3DSceneSceneEditor *)piScene)->ForceLoadAllEntity();
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    nRetCode = GetProcessInfo(&ProInfoCur);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ShowProcessInfoDiff("加载Scene", 0, ProInfoLast, ProInfoCur);
    KGLOG_PROCESS_ERROR(nRetCode);
    ProInfoLast = ProInfoCur;

    // ------------------------------  ------------------------------
    hRetCode = piScene->AddOutputWindow("Login Scene", g_hWnd, 0, 0, &nOutputWindowID);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    piCamera = piScene->GetCurrentCamera();
    KGLOG_PROCESS_ERROR(piCamera);

    hRetCode = piCamera->SetPosition(g_cCameraInitPos, false);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piCamera->SetLookAtPosition(g_cLookUpInitPos, false);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    //-- set default test scene
    ::PostMessage(g_hWnd, WM_CHAR, '6', 0);

    *ppiRetScene = piScene;
    nResult = true;
Exit0:
    if (!nResult && piScene)
    {
        g_cEngineManager.RemoveScene(piScene);
        piScene = NULL; //     KG_COM_RELEASE(piScene);
    }
    return nResult;    
}

////////////////////////////////////////////////////////////////////////////////
int InitModelArray()
{
    int nResult = false;
    HRESULT hRetCode = E_FAIL;
    IKG3DResourceManager *piModelTable = NULL;
    KG3DModel *pModel = NULL;
    
    g_ModelArray.clear();

    piModelTable = g_cEngineManager.Get3DModelTable();
    KGLOG_PROCESS_ERROR(piModelTable);

    for (int i = 0; i < GROUP_TOTAL * MODEL_COUNT_PER_GROUP; ++i)
    {
        int nFileIndex = min((i % MODEL_COUNT_PER_LINE), (i % MODEL_FILE_COUNT));
        hRetCode = piModelTable->LoadResourceFromFile(g_cszModelFile[nFileIndex], 0, 0, (IKG3DResourceBase**)&pModel);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        g_ModelArray.push_back(pModel);

        pModel = NULL;
    }

    nResult = true;
Exit0:
    if (!nResult && piModelTable)
    {
        for (size_t i = 0; i < g_ModelArray.size(); ++i)
        {
            g_ModelArray[i]->Release();
            g_ModelArray[i] = NULL;
        }
        g_ModelArray.clear();
    }

    return nResult;
}
//////////////////////////////////////////////////////////////////////////
void UnintModelArray()
{
    int nRetCode = false;
    KG3DScene *pScene = NULL;

    pScene = g_cEngineManager.GetFocusScene();
    KGLOG_PROCESS_ERROR(pScene);

    if (g_nAddModelFront)
    {
        nRetCode = DeleteModel(pScene, GROUP_FRONT);
        KGLOG_CHECK_ERROR(nRetCode);

        g_nAddModelFront = false;
    }

    if (g_nAddModelBack)
    {
        nRetCode = DeleteModel(pScene, GROUP_BACK);
        KGLOG_CHECK_ERROR(nRetCode);

        g_nAddModelBack = false;
    }

    for (size_t i = 0; i < g_ModelArray.size(); ++i)
    {
        g_ModelArray[i]->Release();
        g_ModelArray[i] = NULL;
    }
    g_ModelArray.clear();

Exit0:

    return;
}
//////////////////////////////////////////////////////////////////////////
int AddModel(
    IKG3DScene *piScene, 
    const D3DXVECTOR3& CameraPos, 
    const D3DXVECTOR3& LookAtPos, 
    int nGroupIndex
)
{
    int nResult = false;
    HRESULT hRetCode = E_FAIL;
    float fXDelta = 0.0f;
    float fZDelta = 0.0f;
    D3DXVECTOR3 ModelPos(0, 0, 0);
    int nIndex = 0;
    int nIndexEnd = 0;
    MODEL_ARRAY TempModelArray;
    D3DXVECTOR3 vCameraDirect;
    float fAngle = 0.0f;
    D3DXMATRIX matRotation;

    ASSERT(piScene);
    ASSERT(nGroupIndex < GROUP_TOTAL);

    vCameraDirect = D3DXVECTOR3(LookAtPos.x, 0, LookAtPos.z) -  D3DXVECTOR3(CameraPos.x, 0, CameraPos.z);
    D3DXVec3Normalize(&vCameraDirect, &vCameraDirect);
    fAngle = acos(D3DXVec3Dot(&D3DXVECTOR3(0, 0, 1), &vCameraDirect));
    D3DXQUATERNION RotQuat;
    D3DXQuaternionRotationAxis(&RotQuat, &D3DXVECTOR3(0, 1, 0), -fAngle);
    D3DXMatrixAffineTransformation(&matRotation, 1, &CameraPos, &RotQuat, NULL);

    nIndex = MODEL_COUNT_PER_GROUP * nGroupIndex;
    nIndexEnd = nIndex + MODEL_COUNT_PER_GROUP;

    TempModelArray.clear();

    for (NULL; nIndex < nIndexEnd; ++nIndex)
    {
        fXDelta = (X_DELTA / 2) + X_DELTA * (-(MODEL_COUNT_PER_LINE / 2) + (nIndex % MODEL_COUNT_PER_LINE));
        if (nGroupIndex == GROUP_FRONT)
            fZDelta =  600 + Z_DELTA * (nIndex / MODEL_COUNT_PER_LINE);
        else // ASSERT(nGroupIndex == GROUP_BACK)
            fZDelta = -600.0f - Z_DELTA * ((nIndex - MODEL_COUNT_PER_GROUP) / MODEL_COUNT_PER_LINE);

        ModelPos.x = CameraPos.x + fXDelta;
        ModelPos.y = CameraPos.y;
        ModelPos.z = CameraPos.z + fZDelta;
        
        D3DXVec3TransformCoord(&ModelPos, &ModelPos, &matRotation);

        hRetCode = piScene->GetHeight(&ModelPos, &ModelPos);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        g_ModelArray[nIndex]->SetTranslation(&ModelPos);

        hRetCode = piScene->AddRenderEntity(g_ModelArray[nIndex], SCENEENTITY_MODEL, true, true);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        TempModelArray.push_back(g_ModelArray[nIndex]);
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        for (size_t i = 0; i < TempModelArray.size(); ++i)
        {
            hRetCode = piScene->RemoveRenderEntity((TempModelArray[i]), SCENEENTITY_MODEL, true);
            if (FAILED(hRetCode))
            {
                KGLOG_CHECK_ERROR(false);
                break;
            }
        }
        TempModelArray.clear();
    }
    
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int DeleteModel(IKG3DScene *piScene, int nGroupIndex)
{
    int nResult = false;    
    HRESULT hRetCode = E_FAIL;
    int nIndex = 0;
    int nIndexEnd = 0;

    ASSERT(nGroupIndex < GROUP_TOTAL);

    nIndex = MODEL_COUNT_PER_GROUP * nGroupIndex;
    nIndexEnd = nIndex + MODEL_COUNT_PER_GROUP;

    for (NULL; nIndex < nIndexEnd; ++nIndex)
    {
        hRetCode = piScene->RemoveRenderEntity((IKG3DResourceBase *)(g_ModelArray[nIndex]), SCENEENTITY_MODEL, true);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
    }
    
    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int AddModelAni(int nGroupIndex)
{
    int nResult = false;    
    HRESULT hRetCode = E_FAIL;
    int nIndex = 0;
    int nIndexEnd = 0;

    ASSERT(nGroupIndex < GROUP_TOTAL);

    nIndex = MODEL_COUNT_PER_GROUP * nGroupIndex;
    nIndexEnd = nIndex + MODEL_COUNT_PER_GROUP;

    for (NULL; nIndex < nIndexEnd; ++nIndex)
    {
        int nFileIndex = min(((nIndex / MODEL_COUNT_PER_LINE) % LINE_COUNT), (nIndex / MODEL_COUNT_PER_LINE) % ANI_FILE_COUNT);
        hRetCode = g_ModelArray[nIndex]->PlayAnimation(
            ENU_ANIMATIONPLAY_CIRCLE, 
            g_cszAniFile[nFileIndex],
            1.0f, 0, NULL, NULL, ANICTL_PRIMARY
        );
        KGLOG_COM_PROCESS_ERROR(hRetCode);
    }

    
    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int DeleteModelAni(int nGroupIndex)
{
    int nResult = false;    
    HRESULT hRetCode = E_FAIL;
    int nIndex = 0;
    int nIndexEnd = 0;

    ASSERT(nGroupIndex < GROUP_TOTAL);

    nIndex = MODEL_COUNT_PER_GROUP * nGroupIndex;
    nIndexEnd = nIndex + MODEL_COUNT_PER_GROUP;

    for (NULL; nIndex < nIndexEnd; ++nIndex)
    {
        hRetCode = g_ModelArray[nIndex]->PlayAnimation(
            ENU_ANIMATIONPLAY_ONCE, 
            g_cszAniFile[(nIndex / MODEL_COUNT_PER_LINE) % LINE_COUNT], 
            1.0f, 0, NULL, NULL, ANICTL_PRIMARY
        );
        KGLOG_COM_PROCESS_ERROR(hRetCode);
    }
    
    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int ChangeModelGroup(
    IKG3DScene *piScene, 
    const D3DXVECTOR3 &CameraPos, 
    const D3DXVECTOR3 &LocokAt,
    BOOL bAddGroupFront,
    BOOL bAddGroupBack
)
{
    int nResult  = false;
    int nRetCode = false;

    if (bAddGroupFront)
    {
        if (!g_nAddModelFront)
        {
            nRetCode = AddModel(piScene, CameraPos, LocokAt, GROUP_FRONT);
            KGLOG_PROCESS_ERROR(nRetCode);

            g_nAddModelFront = true;
        }
    }
    else
    {
        if (g_nAddModelFront)
        {
            nRetCode = DeleteModel(piScene, GROUP_FRONT);
            KGLOG_PROCESS_ERROR(nRetCode);

            g_nAddModelFront = false;
        }
    }

    if (bAddGroupBack)
    {
        if (!g_nAddModelBack)
        {
            nRetCode = AddModel(piScene, CameraPos, LocokAt, GROUP_BACK);
            KGLOG_PROCESS_ERROR(nRetCode);

            g_nAddModelBack = true;
        }
    }
    else
    {
        if (g_nAddModelBack)
        {
            nRetCode = DeleteModel(piScene, GROUP_BACK);
            KGLOG_PROCESS_ERROR(nRetCode);

            g_nAddModelBack = false;
        }
    }

    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int ChangeModelAniGroup(BOOL bAddGroupFront, BOOL bAddGroupBack)
{
    int nResult  = false;
    int nRetCode = false;

    if (bAddGroupFront)
    {
        if (!g_nAddModelAniFront)
        {
            nRetCode = AddModelAni(GROUP_FRONT);
            KGLOG_PROCESS_ERROR(nRetCode);

            g_nAddModelAniFront = true;
        }
    }
    else
    {
        if (g_nAddModelAniFront)
        {
            nRetCode = DeleteModelAni(GROUP_FRONT);
            KGLOG_PROCESS_ERROR(nRetCode);

            g_nAddModelAniFront = false;
        }
    }

    if (bAddGroupBack)
    {
        if (!g_nAddModelAniBack)
        {
            nRetCode = AddModelAni(GROUP_BACK);
            KGLOG_PROCESS_ERROR(nRetCode);

            g_nAddModelAniBack = true;
        }
    }
    else
    {
        if (g_nAddModelAniBack)
        {
            nRetCode = DeleteModelAni(GROUP_BACK);
            KGLOG_PROCESS_ERROR(nRetCode);

            g_nAddModelAniBack = false;
        }
    }

    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int ProcessKeyboardMessage(UINT_PTR uKeyValue)
{
    int nResult  = false;
    int nRetCode = false;
    HRESULT hRetCode = E_FAIL;
    D3DXVECTOR3 CameraPos;
    D3DXVECTOR3 LookAt;
    KG3DScene *pScene = NULL;
    IKG3DCamera *piCamera = NULL;
    static float s_fCarameSpeedIncDelta = 200.0f;
    static float s_fCarameHeigh = -1.0f;
    D3DXMATRIX Matrix;
    D3DXVECTOR3 PosDeta(0, 0, 0);
    float fFixY = 0.0f;
    D3DXVECTOR3 Front(0, 0, 0);
    float fTerrainHeight = 0.0f;

    pScene = g_cEngineManager.GetFocusScene();
    KGLOG_PROCESS_ERROR(pScene);
    
    piCamera = pScene->GetCurrentCamera();
    KGLOG_PROCESS_ERROR(piCamera);

    hRetCode = piCamera->GetPosition(&CameraPos);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piCamera->GetLookAtPosition(&LookAt);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    if (s_fCarameHeigh < 0.0f)
    {
        fTerrainHeight = pScene->GetHeight(CameraPos.x, CameraPos.z);
        s_fCarameHeigh = CameraPos.y - fTerrainHeight;
    }

    switch (uKeyValue)
    {
    case 'a':
        D3DXMatrixRotationY(&Matrix, -0.05f);
        LookAt = D3DXVECTOR3(LookAt.x - CameraPos.x, LookAt.y, LookAt.z - CameraPos.z);
        D3DXVec3TransformCoord(&LookAt, &LookAt, &Matrix);
        LookAt = D3DXVECTOR3(LookAt.x + CameraPos.x, LookAt.y, LookAt.z + CameraPos.z);
        break;

    case 'd':
        D3DXMatrixRotationY(&Matrix, 0.05f);
        LookAt = D3DXVECTOR3(LookAt.x - CameraPos.x, LookAt.y, LookAt.z - CameraPos.z);
        D3DXVec3TransformCoord(&LookAt, &LookAt, &Matrix);
        LookAt = D3DXVECTOR3(LookAt.x + CameraPos.x, LookAt.y, LookAt.z + CameraPos.z);
        break;

    case 'w':
        hRetCode = piCamera->GetFront(&Front);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        D3DXVec3Normalize(&Front, &Front);
    
        PosDeta = Front * s_fCarameSpeedIncDelta;
        CameraPos += PosDeta;
        LookAt    += PosDeta;
        break;

    case 's':
        hRetCode = piCamera->GetFront(&Front);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        D3DXVec3Normalize(&Front, &Front);
    
        PosDeta = Front * s_fCarameSpeedIncDelta;
        CameraPos -= PosDeta;
        LookAt    -= PosDeta;
        break;

    case ' ':
        s_fCarameHeigh += s_fCarameSpeedIncDelta;
        CameraPos.y += s_fCarameSpeedIncDelta;
        LookAt.y    += s_fCarameSpeedIncDelta;
        break;

    case 'b':
        s_fCarameHeigh -= s_fCarameSpeedIncDelta;
        CameraPos.y -= s_fCarameSpeedIncDelta;
        LookAt.y    -= s_fCarameSpeedIncDelta;
        break;

    case '+':
    case '=':
        s_fCarameSpeedIncDelta += 100;
        s_fCarameSpeedIncDelta = s_fCarameSpeedIncDelta > 300 ? 300 : s_fCarameSpeedIncDelta;

        break;
    case '-':
        s_fCarameSpeedIncDelta -= 100;
        s_fCarameSpeedIncDelta = s_fCarameSpeedIncDelta > 0 ? s_fCarameSpeedIncDelta : 5;
        break;

    case '`':
        g_nCurSceneID = 0;
        CameraPos = g_cCameraInitPos;
        LookAt = g_cLookUpInitPos;
        fTerrainHeight = pScene->GetHeight(CameraPos.x, CameraPos.z);
        s_fCarameHeigh = CameraPos.y - fTerrainHeight;
        
        nRetCode = ChangeModelGroup(pScene, CameraPos, LookAt, false, false);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = ChangeModelAniGroup(false, false);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;

    case '1':
        g_nCurSceneID = 1;
        CameraPos = g_cCameraInitPos;
        LookAt = g_cLookUpInitPos;
        fTerrainHeight = pScene->GetHeight(CameraPos.x, CameraPos.z);
        s_fCarameHeigh = CameraPos.y - fTerrainHeight;
        
        nRetCode = ChangeModelGroup(pScene, CameraPos, LookAt, false, true);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = ChangeModelAniGroup(false, false);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;

    case '2':
        g_nCurSceneID = 2;
        CameraPos = g_cCameraInitPos;
        LookAt = g_cLookUpInitPos;
        fTerrainHeight = pScene->GetHeight(CameraPos.x, CameraPos.z);
        s_fCarameHeigh = CameraPos.y - fTerrainHeight;
        
        nRetCode = ChangeModelGroup(pScene, CameraPos, LookAt, false, true);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = ChangeModelAniGroup(false, true);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;

    case '3':
        g_nCurSceneID = 3;
        CameraPos = g_cCameraInitPos;
        LookAt = g_cLookUpInitPos;
        fTerrainHeight = pScene->GetHeight(CameraPos.x, CameraPos.z);
        s_fCarameHeigh = CameraPos.y - fTerrainHeight;
        
        nRetCode = ChangeModelGroup(pScene, CameraPos, LookAt, true, false);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = ChangeModelAniGroup(false, false);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;

    case '4':
        g_nCurSceneID = 4;
        CameraPos = g_cCameraInitPos;
        LookAt = g_cLookUpInitPos;
        fTerrainHeight = pScene->GetHeight(CameraPos.x, CameraPos.z);
        s_fCarameHeigh = CameraPos.y - fTerrainHeight;
        
        nRetCode = ChangeModelGroup(pScene, CameraPos, LookAt, true, false);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = ChangeModelAniGroup(true, false);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;

    case '5':
        g_nCurSceneID = 5;
        CameraPos = g_cCameraInitPos;
        LookAt = g_cLookUpInitPos;
        fTerrainHeight = pScene->GetHeight(CameraPos.x, CameraPos.z);
        s_fCarameHeigh = CameraPos.y - fTerrainHeight;
        
        nRetCode = ChangeModelGroup(pScene, CameraPos, LookAt, true, true);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = ChangeModelAniGroup(false, false);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;

    case '6':
        g_nCurSceneID = 6;
        CameraPos = g_cCameraInitPos;
        LookAt = g_cLookUpInitPos;
        fTerrainHeight = pScene->GetHeight(CameraPos.x, CameraPos.z);
        s_fCarameHeigh = CameraPos.y - fTerrainHeight;
        
        nRetCode = ChangeModelGroup(pScene, CameraPos, LookAt, true, true);
        KGLOG_PROCESS_ERROR(nRetCode);
        nRetCode = ChangeModelAniGroup(true, true);
        KGLOG_PROCESS_ERROR(nRetCode);
        break;

    default:
        goto Exit1;
    }

    // Y轴修正
    if (s_fCarameHeigh < 10.0f)
        s_fCarameHeigh = 10.0f;
    fTerrainHeight = pScene->GetHeight(CameraPos.x, CameraPos.z);
    fFixY = (fTerrainHeight + s_fCarameHeigh) - CameraPos.y;
    LookAt.y    += fFixY;
    CameraPos.y += fFixY;

    hRetCode = piCamera->SetPosition(CameraPos, false);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    hRetCode = piCamera->SetLookAtPosition(LookAt, false);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int TestScene()
{
    int nResult  = false;
    int nRetCode = false;
    HRESULT hRetCode = E_FAIL;
    IKG3DScene *piScene = NULL;
    unsigned uFrameCount = 0;
    PROCESS_INTO ProInfoLast;
    PROCESS_INTO ProInfoCur;
    MSG msg = {0};
    DWORD dwLastTime = 0;
    int nModelArrayInit = false;

    nRetCode = LoadScene(&piScene);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = InitModelArray();
    KGLOG_PROCESS_ERROR(nRetCode);

    nModelArrayInit = true;

    // ------------------------------ render ------------------------------

    nRetCode = ::GetProcessInfo(&ProInfoLast);
    KGLOG_PROCESS_ERROR(nRetCode);
    
    dwLastTime = GetTickCount();    
    while (true)
    {
        ++uFrameCount;
        if ((uFrameCount % 10) == 0)
        {
            char szTitle[1024];
            DWORD dwNowTime = GetTickCount();
            snprintf(
                szTitle, 
                sizeof(szTitle),
                "3D引擎Test程序 (%d, AVG FPS=%d) 当前场景:%d(FM=%d FA=%d BM=%d BA=%d), 场景切换:`123456, 转镜头:ADWS, 镜头升降:空格和B, 镜头升降速度:-+", 
                uFrameCount, 10 * 1000 / (dwNowTime - dwLastTime),
                g_nCurSceneID,
                g_nAddModelFront, g_nAddModelAniFront,
                g_nAddModelBack,  g_nAddModelAniBack
            );
            szTitle[sizeof(szTitle) - 1] = '\0';
            SetWindowText(g_hWnd, szTitle);
            dwLastTime = dwNowTime;
        }

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            switch (msg.message)
            {
            case WM_QUIT:
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessage(&msg);     
            }   
        }
        if (msg.message == WM_QUIT)
            break;

        hRetCode = g_cEngineManager.FrameMove();
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        hRetCode = g_cEngineManager.Render();
        KGLOG_COM_PROCESS_ERROR(hRetCode);
    }

    nRetCode = GetProcessInfo(&ProInfoCur);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ShowProcessInfoDiff("Render Scene", uFrameCount, ProInfoLast, ProInfoCur);
    KGLOG_PROCESS_ERROR(nRetCode);
    ProInfoLast = ProInfoCur;

    nResult = true;
Exit0:
    if (nModelArrayInit)
    {
        UnintModelArray();
        nModelArrayInit = false;
    }

    if (piScene)
    {
        g_cEngineManager.RemoveScene(piScene);
        piScene = NULL; //     KG_COM_RELEASE(piScene);
    }

    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CHAR:
        ProcessKeyboardMessage(wParam);
        //KGLOG_PROCESS_ERROR(nRetCode);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
int SetLowerConfig()
{
    int nResult = false;
    IIniFile *piFile = NULL;

    piFile = g_OpenIniFile(g_szConfigFilePath);
    KGLOG_PROCESS_ERROR(piFile);

    piFile->WriteInteger("KG3DENGINE", "MultiSampleType", D3DMULTISAMPLE_NONE);
    piFile->WriteInteger("KG3DENGINE", "SampMipFilter",   D3DTEXF_POINT);
    piFile->WriteInteger("KG3DENGINE", "SampMinFilter",   D3DTEXF_POINT);
    piFile->WriteInteger("KG3DENGINE", "SampMagFilter",   D3DTEXF_POINT);

    piFile->WriteFloat("KG3DENGINE", "CammeraDistance", 20000.0f);

    piFile->WriteInteger("KG3DENGINE", "RenderGrass",     false);
    piFile->WriteInteger("KG3DENGINE", "WaterRefraction", false);

    piFile->WriteInteger("KG3DENGINE", "WaterReflection",         false);
    piFile->WriteInteger("KG3DENGINE", "WaterReflectEnvironment", false);
    piFile->WriteInteger("KG3DENGINE", "WaterReflectGround",      false);
    piFile->WriteInteger("KG3DENGINE", "WaterReflectNPC",         false);
    piFile->WriteInteger("KG3DENGINE", "WaterReflectEntity",      false);

    piFile->WriteInteger("KG3DENGINE", "PostRenderEffect", false);
    piFile->WriteInteger("KG3DENGINE", "bGodRay",          false);
    piFile->WriteInteger("KG3DENGINE", "ShockWaveEnable",  false);
    piFile->WriteInteger("KG3DENGINE", "BloomEnable",      false);

    piFile->WriteInteger("KG3DENGINE", "ModelShadowType", 0);

    piFile->Save(g_szConfigFilePath);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piFile);
    return nResult;
}
////////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    HRESULT hRetCode = E_FAIL;
    int nKGLogInitFlag = false;
    int nKG3DEngineInitFlag = false;
    int nRetCode = false;
    KGLOG_PARAM LogParam = {"logs", "KG3DEngine", KGLOG_OPTION_FILE, 65536};
    WNDCLASSEX wcex = {0};
    MSG msg = {0};
    IIniFile* piFile = NULL;
    int nX = 0;
    int nY = 0;
    int nWidth  = 0;
    int nHeight = 0;
    int nFullScreen = 0;

    KMemory::Initialize("engine.memory");

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    nRetCode = GetDllFilePath(NULL);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = InitConfigFilePath(g_szConfigFilePath, _countof(g_szConfigFilePath), g_szModuleFilePath);
    KGLOG_PROCESS_ERROR(nRetCode);

    g_SetRootPath(NULL);
    SetLowerConfig();

	wcex.cbSize         = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.hInstance		= hInstance;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= TEXT("KG3DEnginePerformanceAnalyzer");
	RegisterClassEx(&wcex);

    piFile = g_OpenIniFile(g_szConfigFilePath);
    KGLOG_PROCESS_ERROR(piFile);

    piFile->GetInteger("Main", "X", 0, &nX);
    piFile->GetInteger("Main", "Y", 0, &nY);
    piFile->GetInteger("Main", "CanvasWidth",  1024, &nWidth);
    piFile->GetInteger("Main", "CanvasHeight", 768,  &nHeight);
    piFile->GetInteger("Main", "FullScreen",   0,    &nFullScreen);

    g_hWnd = ::CreateWindow( 
        wcex.lpszClassName,  TEXT("3D引擎性能分析程序"),  WS_CAPTION | WS_SYSMENU,
        nX, nY, nWidth, nHeight,
        NULL, NULL, hInstance, NULL 
    );
    KGLOG_PROCESS_ERROR(g_hWnd);

    if (nFullScreen)
    {
        nRetCode = ::SetWindowLong(g_hWnd, GWL_STYLE, WS_OVERLAPPED | WS_SYSMENU);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = ::SetWindowPos(
            g_hWnd, NULL, nX, nY, nWidth, nHeight, SWP_FRAMECHANGED
        );
        KGLOG_PROCESS_ERROR(nRetCode);
    }


    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    //--------------------------------
    hRetCode = g_cEngineManager.Init(0, KG3DENG_CLIENT, g_hWnd, g_hWnd);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    nKG3DEngineInitFlag = true;

    TestScene();
    //ProcessForAllFile("测试加载Mesh", "data", "mesh", TestLoadMesh);
    //ProcessForAllFile("测试加载Model", "data", "mesh", TestLoadModel);
    //ProcessForAllFile("测试加载MDL",  "data", "mdl", TestLoadModel);
    //ProcessForAllFile("测试加载MTL",  "data", "mtl", TestLoadMaterial);

    //TestTexture("d:\\UserData\\Documents\\Temp\\TextureTest\\bmp32");
    //TestTexture("d:\\UserData\\Documents\\Temp\\TextureTest\\dxt5");
    //TestTexture("d:\\UserData\\Documents\\Temp\\TextureTest\\jpg");
    //TestTexture("d:\\UserData\\Documents\\Temp\\TextureTest\\tga");

Exit0:
    if (nKG3DEngineInitFlag)
    {
        g_cEngineManager.UnInit();
        nKG3DEngineInitFlag = false;
    }
    KG_COM_RELEASE(piFile);
    if (nKGLogInitFlag)
    {
        KGLogUnInit(NULL);
        nKGLogInitFlag = false;
    } 
    KMemory::Finalize();
	return (int) msg.wParam;
}

#endif//KG3D_DEV_TEST

