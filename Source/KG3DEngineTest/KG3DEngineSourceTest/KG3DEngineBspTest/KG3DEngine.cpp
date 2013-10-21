// KG3DEngine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "KG3DEngine.h"
#include "KG3DSceneSceneEditor.h"
#include "IEKG3DSceneCamera.h"
#include ".\kg3denginemanager.h"
#include "KG3DAnimationTagContainer.h"
#include "KG3DAnimationSoundTag.h"
#include "KG3DTypes.h"
#include "kg3dmaterialtable.h"
#include "KG3DModelTable.h"
#include "KG3DSFXTrack.h"
#include "KG3DMeshTable.h"
#include "KG3DModelPointLight.h"
#include "KG3DMesh.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern KG3DEngineManager g_cEngineManager;         // 引擎管理器
extern BOOL g_bForceGenerateBspForMesh;

////////////////////////////////////////////////////////////////////////////////

#ifdef _MANAGED
#pragma managed(push, off)
#endif


BOOL APIENTRY DllMain( HMODULE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

DLLKG3DENGINE_API HRESULT Get3DEngineInterface(void** ppEngineManager)
{
	*ppEngineManager = &g_cEngineManager;
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
//             Test Code for Engine Performance Analyze
//                              ------ By Wu Caizhong
////////////////////////////////////////////////////////////////////////////////
#ifdef KG3D_DEV_TEST

#include <Psapi.h>
#include "KG3DStateBlock.h"

HWND g_hWnd = NULL;

int MAX_SIZE = 512;

//enum ERROR_TYPE
//{
//    TYPE_MDL = 0,
//    TYPE_SOUND,
//    TYPE_T0TAL
//};

enum TYPE_EXT
{
    EXT_MESH = 0,
    EXT_MDL,
    EXT_TANI,
    EXT_SFX,
    EXT_MTL,
    EXT_SPEEDTREE,
    EXT_T0TAL
};

const char g_cszErrorFileName[EXT_T0TAL][32] ={
    "ErrorMeshRecord.ini",
    "ErrorMDLRecord.ini",
    "ErrorSoundRecord.ini",
    "ErrorSFXRecord.ini",
    "ErrorMaterialRecord.ini",
    "ErrorSpeedtreeRecord.ini",
};

const char g_cszExtName[EXT_T0TAL][32] ={
    ".mesh",
    ".mdl",
    ".tani",
    ".sfx",
    ".mtl",
    ".stree"
};

//const char g_cszErrorFileName[32] = "ErrorRecord.ini"; 
vector<string> g_vecSourceList;

//////////////////////////////////////////////////////////////////////////
int PushValidFile(const char *pcszFileName, vector<string> *pvecFileList, TYPE_EXT ExtType)
{
    int nRetCode                = false;
    int nResult                 = false;
    int nLength                 = 0;
    static int nCount           = 0;
    char szMessage[MAX_PATH]    = "";  
    char szExt[MAX_PATH]        = "";
    _splitpath_s(pcszFileName, 
        NULL, 
        0,
        NULL, 
        0, 
        NULL, 
        0, 
        szExt,
        MAX_PATH);

    ++nCount;
    snprintf(
        szMessage, sizeof(szMessage), 
        "Now Try to Get Name: %d  ....", 
        nCount
        );
    szMessage[sizeof(szMessage) - 1] = '\0';
    SetWindowText(g_hWnd, szMessage);

    nRetCode = stricmp(szExt, g_cszExtName[ExtType]);
    KG_PROCESS_ERROR(!nRetCode);

    nLength = (int)strlen(g_cszExtName[ExtType]);
    if (!szExt[nLength])
    {
        pvecFileList->push_back(pcszFileName);
    }

    nResult = true;
Exit0:
    return nResult;
};


//////////////////////////////////////////////////////////////////////////
int GetFileList(const char *pcszDirPath, vector<string> *pvecFileList, TYPE_EXT ExtType)
{
    int nRetCode    = false;
    int nResult     = false;
    int nLength     = 0;
    DWORD dwError   = 0;
    DWORD dwAttriID = 0;
    HANDLE hFind    = INVALID_HANDLE_VALUE;
    char szDirPath[MAX_PATH]        = "";
    char szFileName[MAX_PATH]       = "";
    char szErrorInfo[MAX_PATH]      = "";
    WIN32_FIND_DATA FindFileData;

    hFind = FindFirstFile(pcszDirPath, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        dwError = GetLastError();
        sprintf_s (szErrorInfo, "Invalid file handle. Error is %u\n", dwError);
        KG_PROCESS_ERROR(false);
    }

    strncpy(szDirPath, pcszDirPath, MAX_PATH);
    nLength = (int)strlen(pcszDirPath);
    szDirPath[nLength - 3] = '\0';

    while (true)
    {
        if (strcmp(FindFileData.cFileName, ".") && strcmp(FindFileData.cFileName, ".."))
        {
            strncpy(szFileName, szDirPath, MAX_PATH);
            strcat(szFileName, FindFileData.cFileName);
            dwAttriID = GetFileAttributes(szFileName);
            if (dwAttriID == FILE_ATTRIBUTE_DIRECTORY)
            {
                if (strstr(FindFileData.cFileName, ".svn") == NULL)
                {
                    strcat(szFileName, "\\*.*");
                    GetFileList(szFileName, pvecFileList, ExtType);
                }
            }
            else
            {
                PushValidFile(szFileName, pvecFileList, ExtType);
            }
        }
        nRetCode = FindNextFile(hFind, &FindFileData);
        if (nRetCode)
        {
            continue;
        }

        dwError = GetLastError();
        if (dwError != ERROR_NO_MORE_FILES) 
        {
            sprintf_s (szErrorInfo, "FindNextFile error. Error is %u\n", dwError);
            KG_PROCESS_ERROR(false);
        }
        break;

    }

    nResult = true;
Exit0:
    if (hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        hFind = INVALID_HANDLE_VALUE;
    }

    return nResult;
};

//////////////////////////////////////////////////////////////////////////
int TestLoadAllMesh()
{
    int nResult                         = false;
    int nRetCode                        = false;
    HRESULT hrRetCode                   = E_FAIL;
    unsigned uTotalCount                = 0;
    unsigned uFailedCount               = 0;
    unsigned uFailedCountSave           = 0;    
    IIniFile *pFile                     = NULL;
    IKG3DModel *piModel                 = NULL;
    IKG3DResourceManager *piModelTable  = NULL;
    char szMessage[MAX_PATH]            = "";
    char szDirPath[MAX_PATH]            = "";
    //vector<string> vecModelList;
    //vector<string> vecMeshList;   
    MSG msg;
    
    nRetCode = GetCurrentDirectory(MAX_PATH, szDirPath);
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < MAX_PATH + 1);
    strcat(szDirPath, "\\data\\*.*");
    szDirPath[MAX_PATH - 1] = '\0';
    //strcpy(szDirPath, "D:\\Kingsoft\\Game\\sword3-products\\trunk\\client\\data\\source\\NPC_source\\*.*");

    piModelTable = g_cEngineManager.Get3DModelTable();
    KGLOG_PROCESS_ERROR(piModelTable);

    pFile = g_OpenIniFile(g_cszErrorFileName[EXT_MESH],false,true);
    KGLOG_PROCESS_ERROR(pFile);

    nRetCode = GetFileList(szDirPath, &g_vecSourceList/*vecMeshList*/, EXT_MESH);
    KGLOG_PROCESS_ERROR(nRetCode);
    uTotalCount = (unsigned)/*vecMeshList*/g_vecSourceList.size();
    /*uFailedCountSave = uFailedCount;*/
    for (unsigned i = 0; i < uTotalCount; ++i)
    {
        char szKeyName[32] = "";
        snprintf(
            szMessage, sizeof(szMessage), 
            "Mesh模型加载检查文件：%s", 
            /*vecMeshList*/g_vecSourceList[i].c_str()
            );
        szMessage[sizeof(szMessage) - 1] = '\0';
        SetWindowText(g_hWnd, szMessage);

        try
        {
            hrRetCode = piModelTable->LoadResourceFromFile(/*vecMeshList*/g_vecSourceList[i].c_str(), 0, 0, (IKG3DResourceBase**)&piModel);
            if (FAILED(hrRetCode))
            {
                itoa(uFailedCount, szKeyName, 10);
                pFile->WriteString("ErrorMesh",szKeyName, /*vecMeshList*/g_vecSourceList[i].c_str());
                ++uFailedCount;
            }
            KG_COM_RELEASE(piModel);
            g_cEngineManager.FrameMove();
        }
        catch (...)
        {
            itoa(uFailedCount, szKeyName, 10);
            pFile->WriteString("Mesh_Exception",szKeyName, /*vecMeshList*/g_vecSourceList[i].c_str());
            ++uFailedCount;
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
    pFile->WriteInteger("ErrorMesh", "ErrorNum", uFailedCount);
    pFile->Save(g_cszErrorFileName[EXT_MESH]);
  
    //uFailedCount += uFailedCountSave;
    //uTotalCount = (unsigned)(vecModelList.size() + vecMeshList.size());
    // snprintf(
    //    szMessage, sizeof(szMessage), 
    //    "模型加载检查完成：共检查模型 %u 个 已发现错误 %u 个，请检查文件%s", 
    //    uTotalCount, uFailedCount, g_cszErrorFileName[TYPE_MDL]
    //);
    //szMessage[sizeof(szMessage) - 1] = '\0';
    //::MessageBox(g_hWnd, szMessage, "检查报告", MB_OK);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piModel);
    KG_COM_RELEASE(pFile);
    g_vecSourceList.clear();
    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int TestLoadAllMDL()
{
    int nResult                         = false;
    int nRetCode                        = false;
    HRESULT hrRetCode                   = E_FAIL;
    unsigned uTotalCount                = 0;
    unsigned uFailedCount               = 0;
    unsigned uFailedCountSave           = 0;    
    IIniFile *pFile                     = NULL;
    IKG3DModel *piModel                 = NULL;
    IKG3DResourceManager *piModelTable  = NULL;
    char szMessage[MAX_PATH]            = "";
    char szDirPath[MAX_PATH]            = "";
    //vector<string> vecModelList;
    //vector<string> vecMeshList;   
    MSG msg;

    nRetCode = GetCurrentDirectory(MAX_PATH, szDirPath);
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < MAX_PATH + 1);
    strcat(szDirPath, "\\data\\*.*");
    szDirPath[MAX_PATH - 1] = '\0';


    pFile = g_OpenIniFile(g_cszErrorFileName[EXT_MDL],false,true);
    KGLOG_PROCESS_ERROR(pFile);

    piModelTable = g_cEngineManager.Get3DModelTable();
    KGLOG_PROCESS_ERROR(piModelTable);

    nRetCode = GetFileList(szDirPath, &g_vecSourceList/*vecModelList*/, EXT_MDL);
    KGLOG_PROCESS_ERROR(nRetCode);

    uFailedCount = 0;
    uTotalCount = (unsigned)/*vecModelList*/g_vecSourceList.size();
    for (unsigned i = 0; i < uTotalCount; ++i)
    {
        char szKeyName[32] = "";
        snprintf(
            szMessage, sizeof(szMessage), 
            "Mdl模型加载检查文件：%s", 
            /*vecModelList*/g_vecSourceList[i].c_str()
            );
        szMessage[sizeof(szMessage) - 1] = '\0';
        SetWindowText(g_hWnd, szMessage);
        try
        {
            hrRetCode = piModelTable->LoadResourceFromFile(/*vecModelList*/g_vecSourceList[i].c_str(), 0, 0, (IKG3DResourceBase**)&piModel);
            if (FAILED(hrRetCode))
            {
                itoa(uFailedCount, szKeyName, 10);
                pFile->WriteString("ErrorMdl",szKeyName, /*vecModelList*/g_vecSourceList[i].c_str());
                ++uFailedCount;
            }
            KG_COM_RELEASE(piModel);
            g_cEngineManager.FrameMove();
        }
        catch (...)
        {
            itoa(uFailedCount, szKeyName, 10);
            pFile->WriteString("Mdl_Exception",szKeyName, /*vecModelList*/g_vecSourceList[i].c_str());
            ++uFailedCount;
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
    pFile->WriteInteger("ErrorMdl", "ErrorNum", uFailedCount);
    pFile->Save(g_cszErrorFileName[EXT_MDL]);
    //uFailedCount += uFailedCountSave;
    //uTotalCount = (unsigned)(vecModelList.size() + vecMeshList.size());
    // snprintf(
    //    szMessage, sizeof(szMessage), 
    //    "模型加载检查完成：共检查模型 %u 个 已发现错误 %u 个，请检查文件%s", 
    //    uTotalCount, uFailedCount, g_cszErrorFileName[TYPE_MDL]
    //);
    //szMessage[sizeof(szMessage) - 1] = '\0';
    //::MessageBox(g_hWnd, szMessage, "检查报告", MB_OK);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piModel);
    KG_COM_RELEASE(pFile);
    g_vecSourceList.clear();
    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int TestLoadAllSound()
{
    int nResult                         = false;
    int nRetCode                        = false;
    HRESULT hrRetCode                   = E_FAIL;
    unsigned uTotalCount                = 0;
    unsigned uFailedCount               = 0;
    IIniFile *pFile                     = NULL;
    KG3DAnimationTagContainer *pContainer = NULL;
    char szMessage[MAX_PATH]            = "";
    char szDirPath[MAX_PATH]            = "";
    //vector<string> vecContainerList;
    vector<string> vecErrorSoundList; 
    MSG msg;
    BOOL bSave = g_eEngineOption.bUseMultiThreadAni;
    g_eEngineOption.bUseMultiThreadAni = FALSE;

    nRetCode = GetCurrentDirectory(MAX_PATH, szDirPath);
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < MAX_PATH + 1);
    strcat(szDirPath, "\\data\\*.*");
    szDirPath[MAX_PATH - 1] = '\0';

    pFile = g_OpenIniFile(g_cszErrorFileName[EXT_TANI], false,true);
    KGLOG_PROCESS_ERROR(pFile);

    g_vecSourceList.clear();
    nRetCode = GetFileList(szDirPath, &g_vecSourceList/*vecContainerList*/, EXT_TANI);
    KGLOG_PROCESS_ERROR(nRetCode);

    uTotalCount = (unsigned)/*vecContainerList*/g_vecSourceList.size();
    for (unsigned i = 0; i < uTotalCount; ++i)
    {
        char szKeyName[32] = "";
        snprintf(
            szMessage, sizeof(szMessage), 
            "加载检查标签文件：%s", 
            /*vecContainerList*/g_vecSourceList[i].c_str()
            );
        szMessage[sizeof(szMessage) - 1] = '\0';
        SetWindowText(g_hWnd, szMessage);
        try
        {
            pContainer = new KG3DAnimationTagContainer();
            hrRetCode = pContainer->_Load(/*vecContainerList*/g_vecSourceList[i].c_str());
            if (FAILED(hrRetCode))
            {
                itoa(uFailedCount, szKeyName, 10);
                pFile->WriteString("ErrorTani",szKeyName, /*vecContainerList*/g_vecSourceList[i].c_str());
                ++uFailedCount;
            }
           /* else
            {
                for (int i = 0; i < (int)(pContainer->m_vecTags.size()); ++i)
                {
                    KG3DAnimationTagContainer::Item Tag = (pContainer->m_vecTags)[i];
                    if (Tag.Type == 1)
                    {
                       KG3DAnimationSoundTag *pTag = (KG3DAnimationSoundTag *)Tag.pTag;
                       KG3DAnimationSoundTag::DataStructure::iterator itFind = pTag->m_Tags.begin();
                       while (itFind != pTag->m_Tags.end())
                       {
                           for (int j = 0; j < (itFind->second).Data.dwValidateCandidate; ++j)
                           {
                               char *pSoundName = (itFind->second).Data.SoundCandidates[j].strSoundFileName.strFileName;
                               nRetCode = g_IsFileExist(pSoundName);
                               if (!nRetCode && pSoundName[0])
                               {
                                   vecErrorSoundList.push_back(pSoundName);
                               }
                           }
                           ++itFind;
                       }
                    }
                    if (vecErrorSoundList.size())
                    {
                        ++uFailedCount;
                    }
                    for (int j =0; j < vecErrorSoundList.size(); ++j)
                    {
                        itoa(j, szKeyName, 10);
                        pFile->WriteString(vecContainerList[i].c_str(), szKeyName, vecErrorSoundList[i].c_str());
                    }
                }
            }*/
            SAFE_RELEASE(pContainer);
        }
        catch (...)
        {
            itoa(uFailedCount, szKeyName, 10);
            pFile->WriteString("Tani_Exception",szKeyName, /*vecContainerList*/g_vecSourceList[i].c_str());
            ++uFailedCount;
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
    pFile->WriteInteger("ErrorTani", "ErrorNum", uFailedCount);

    //snprintf(
    //    szMessage, sizeof(szMessage), 
    //    "模型加载检查完成：共检查标签 %u 个 已发现错误 %u 个，请检查文件%s", 
    //    uTotalCount, uFailedCount, g_cszErrorFileName[TYPE_SOUND]
    //);
    //szMessage[sizeof(szMessage) - 1] = '\0';
    //::MessageBox(g_hWnd, szMessage, "检查报告", MB_OK);

    pFile->Save(g_cszErrorFileName[EXT_TANI]);
    nResult = true;
Exit0:
    SAFE_DELETE(pContainer);
    KG_COM_RELEASE(pFile);
    g_eEngineOption.bUseMultiThreadAni = bSave;
    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int TestLoadAllSFX()
{
    int nResult                         = false;
    int nRetCode                        = false;
    HRESULT hrRetCode                   = E_FAIL;
    unsigned uTotalCount                = 0;
    unsigned uFailedCount               = 0;
    unsigned uFailedCountSave           = 0;    
    IIniFile *pFile                     = NULL;
    IKG3DModel *piSFX                   = NULL;
    IKG3DResourceManager *piModelTable  = NULL;
    char szMessage[MAX_PATH]            = "";
    char szDirPath[MAX_PATH]            = "";
    //vector<string> vecSFXList;   
    MSG msg;

    nRetCode = GetCurrentDirectory(MAX_PATH, szDirPath);
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < MAX_PATH + 1);
    strcat(szDirPath, "\\data\\*.*");
    szDirPath[MAX_PATH - 1] = '\0';


    pFile = g_OpenIniFile(g_cszErrorFileName[EXT_SFX],false,true);
    KGLOG_PROCESS_ERROR(pFile);

    piModelTable = g_cEngineManager.Get3DModelTable();
    KGLOG_PROCESS_ERROR(piModelTable);

    g_vecSourceList.clear();
    nRetCode = GetFileList(szDirPath, &g_vecSourceList/*vecSFXList*/, EXT_SFX);
    KGLOG_PROCESS_ERROR(nRetCode);

    uTotalCount = (unsigned)/*vecSFXList*/g_vecSourceList.size();
    for (unsigned i = 0; i < uTotalCount; ++i)
    {
        char szKeyName[32] = "";
        snprintf(
            szMessage, sizeof(szMessage), 
            "特效加载检查文件：%s", 
            /*vecSFXList*/g_vecSourceList[i].c_str()
            );
        szMessage[sizeof(szMessage) - 1] = '\0';
        SetWindowText(g_hWnd, szMessage);
        try
        {
            hrRetCode = piModelTable->LoadResourceFromFile(/*vecSFXList*/g_vecSourceList[i].c_str(), 0, 0, (IKG3DResourceBase**)&piSFX);
            if (FAILED(hrRetCode))
            {
                itoa(uFailedCount, szKeyName, 10);
                pFile->WriteString("ErrorSFX",szKeyName, /*vecSFXList*/g_vecSourceList[i].c_str());
                ++uFailedCount;
            }
            KG_COM_RELEASE(piSFX);
            g_cEngineManager.FrameMove();
        }
        catch (...)
        {
            itoa(uFailedCount, szKeyName, 10);
            pFile->WriteString("SFX_Exception",szKeyName, /*vecSFXList*/g_vecSourceList[i].c_str());
            ++uFailedCount;
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
    pFile->WriteInteger("ErrorSFX", "ErrorNum", uFailedCount);
  
    //uFailedCount += uFailedCountSave;
    //uTotalCount = (unsigned)(vecModelList.size() + vecMeshList.size());
    // snprintf(
    //    szMessage, sizeof(szMessage), 
    //    "模型加载检查完成：共检查模型 %u 个 已发现错误 %u 个，请检查文件%s", 
    //    uTotalCount, uFailedCount, g_cszErrorFileName[TYPE_MDL]
    //);
    //szMessage[sizeof(szMessage) - 1] = '\0';
    //::MessageBox(g_hWnd, szMessage, "检查报告", MB_OK);

    pFile->Save(g_cszErrorFileName[EXT_SFX]);
    nResult = true;
Exit0:
    KG_COM_RELEASE(piSFX);
    KG_COM_RELEASE(pFile);
    return nResult;
}

//////////////////////////////////////////////////////////////////////////
int TestLoadAllMaterial()
{
    int nResult                         = false;
    int nRetCode                        = false;
    HRESULT hrRetCode                   = E_FAIL;
    unsigned uTotalCount                = 0;
    unsigned uFailedCount               = 0;
    unsigned uFailedCountSave           = 0;    
    IIniFile *pFile                     = NULL;
    IEKG3DMaterial *piMTL               = NULL;
    char szMessage[MAX_PATH]            = "";
    char szDirPath[MAX_PATH]            = "";
    //vector<string> vecMTLList;   
    MSG msg;

    nRetCode = GetCurrentDirectory(MAX_PATH, szDirPath);
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < MAX_PATH + 1);
    strcat(szDirPath, "\\data\\*.*");
    szDirPath[MAX_PATH - 1] = '\0';


    pFile = g_OpenIniFile(g_cszErrorFileName[EXT_MTL],false,true);
    KGLOG_PROCESS_ERROR(pFile);

    g_vecSourceList.clear();
    nRetCode = GetFileList(szDirPath, &g_vecSourceList/*vecMTLList*/, EXT_MTL);
    KGLOG_PROCESS_ERROR(nRetCode);

    uTotalCount = (unsigned)/*vecMTLList*/g_vecSourceList.size();
    for (unsigned i = 0; i < uTotalCount; ++i)
    {
        char szKeyName[32] = "";
        snprintf(
            szMessage, sizeof(szMessage), 
            "材质加载检查文件：%s", 
            /*vecMTLList*/g_vecSourceList[i].c_str()
            );
        szMessage[sizeof(szMessage) - 1] = '\0';
        SetWindowText(g_hWnd, szMessage);
        try
        {
            hrRetCode = g_cMaterialTable.LoadResourceFromFile(/*vecMTLList*/g_vecSourceList[i].c_str(), 0, 0, (IKG3DResourceBase**)&piMTL);
            if (FAILED(hrRetCode))
            {
                itoa(uFailedCount, szKeyName, 10);
                pFile->WriteString("ErrorMTL",szKeyName, /*vecMTLList*/g_vecSourceList[i].c_str());
                ++uFailedCount;
            }
            KG_COM_RELEASE(piMTL);
        }
        catch (...)
        {
            itoa(uFailedCount, szKeyName, 10);
            pFile->WriteString("MTL_Exception",szKeyName, /*vecMTLList*/g_vecSourceList[i].c_str());
            ++uFailedCount;
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
    pFile->WriteInteger("ErrorMTL", "ErrorNum", uFailedCount);

    //uFailedCount += uFailedCountSave;
    //uTotalCount = (unsigned)(vecModelList.size() + vecMeshList.size());
    // snprintf(
    //    szMessage, sizeof(szMessage), 
    //    "模型加载检查完成：共检查模型 %u 个 已发现错误 %u 个，请检查文件%s", 
    //    uTotalCount, uFailedCount, g_cszErrorFileName[TYPE_MDL]
    //);
    //szMessage[sizeof(szMessage) - 1] = '\0';
    //::MessageBox(g_hWnd, szMessage, "检查报告", MB_OK);

    pFile->Save(g_cszErrorFileName[EXT_MTL]);
    nResult = true;
Exit0:
    KG_COM_RELEASE(piMTL);
    KG_COM_RELEASE(pFile);
    return nResult;
}

//////////////////////////////////////////////////////////////////////////

int TestLoadAllSPEEDTREE()
{
    int nResult                         = false;
    int nRetCode                        = false;
    HRESULT hrRetCode                   = E_FAIL;
    unsigned uTotalCount                = 0;
    unsigned uFailedCount               = 0;
    unsigned uFailedCountSave           = 0;    
    IIniFile *pFile                     = NULL;
    IKG3DModel *piSFX                   = NULL;
    IKG3DResourceManager *piModelTable  = NULL;
    char szMessage[MAX_PATH]            = "";
    char szDirPath[MAX_PATH]            = "";
    //vector<string> vecSFXList;   
    MSG msg;

    nRetCode = GetCurrentDirectory(MAX_PATH, szDirPath);
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < MAX_PATH + 1);
    strcat(szDirPath, "\\data\\*.*");
    szDirPath[MAX_PATH - 1] = '\0';


    pFile = g_OpenIniFile(g_cszErrorFileName[EXT_SPEEDTREE],false,true);
    KGLOG_PROCESS_ERROR(pFile);

    piModelTable = g_cEngineManager.Get3DModelTable();
    KGLOG_PROCESS_ERROR(piModelTable);

    g_vecSourceList.clear();
    nRetCode = GetFileList(szDirPath, &g_vecSourceList/*vecSFXList*/, EXT_SPEEDTREE);
    KGLOG_PROCESS_ERROR(nRetCode);

    uTotalCount = (unsigned)/*vecSFXList*/g_vecSourceList.size();
    for (unsigned i = 0; i < uTotalCount; ++i)
    {
        char szKeyName[32] = "";
        snprintf(
            szMessage, sizeof(szMessage), 
            "Speedtree加载检查文件：%s", 
            /*vecSFXList*/g_vecSourceList[i].c_str()
            );
        szMessage[sizeof(szMessage) - 1] = '\0';
        SetWindowText(g_hWnd, szMessage);
        try
        {
            hrRetCode = piModelTable->LoadResourceFromFile(/*vecSFXList*/g_vecSourceList[i].c_str(), 0, 0, (IKG3DResourceBase**)&piSFX);
            if (FAILED(hrRetCode))
            {
                itoa(uFailedCount, szKeyName, 10);
                pFile->WriteString("ErrorSpeedTree",szKeyName, /*vecSFXList*/g_vecSourceList[i].c_str());
                ++uFailedCount;
            }
            KG_COM_RELEASE(piSFX);
            g_cEngineManager.FrameMove();
        }
        catch (...)
        {
            itoa(uFailedCount, szKeyName, 10);
            pFile->WriteString("SpeedTree_Exception",szKeyName, /*vecSFXList*/g_vecSourceList[i].c_str());
            ++uFailedCount;
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
    pFile->WriteInteger("ErrorSpeedTree", "ErrorNum", uFailedCount);

    //uFailedCount += uFailedCountSave;
    //uTotalCount = (unsigned)(vecModelList.size() + vecMeshList.size());
    // snprintf(
    //    szMessage, sizeof(szMessage), 
    //    "模型加载检查完成：共检查模型 %u 个 已发现错误 %u 个，请检查文件%s", 
    //    uTotalCount, uFailedCount, g_cszErrorFileName[TYPE_MDL]
    //);
    //szMessage[sizeof(szMessage) - 1] = '\0';
    //::MessageBox(g_hWnd, szMessage, "检查报告", MB_OK);

    pFile->Save(g_cszErrorFileName[EXT_SPEEDTREE]);
    nResult = true;
Exit0:
    KG_COM_RELEASE(piSFX);
    KG_COM_RELEASE(pFile);
    return nResult;
}
//////////////////////////////////////////////////////////////////////////
int TestLoadAllDetail()
{
    
    int nResult                         = false;
    int nRetCode                        = false;
    HRESULT hrRetCode                   = E_FAIL;
    unsigned uTotalCount                = 0;
    unsigned uFailedCount               = 0;
    unsigned uFailedCountSave           = 0;    
    IIniFile *pFile                     = NULL;
    IKG3DModel *piModel                 = NULL;
    IKG3DResourceManager *piModelTable  = NULL;
    char szMessage[MAX_PATH]            = "";
    char szDirPath[MAX_PATH]            = "";
    //vector<string> vecModelList;
    //vector<string> vecMeshList;   
    MSG msg;

    nRetCode = GetCurrentDirectory(MAX_PATH, szDirPath);
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < MAX_PATH + 1);
    strcat(szDirPath, "\\data\\source\\NPC_source\\*.*");
    szDirPath[MAX_PATH - 1] = '\0';
    //strcpy(szDirPath, "D:\\Kingsoft\\Game\\sword3-products\\trunk\\client\\data\\source\\NPC_source\\*.*");

    piModelTable = g_cEngineManager.Get3DModelTable();
    KGLOG_PROCESS_ERROR(piModelTable);

    pFile = g_OpenIniFile(g_cszErrorFileName[EXT_MESH],false,true);
    KGLOG_PROCESS_ERROR(pFile);

    nRetCode = GetFileList(szDirPath, &g_vecSourceList/*vecMeshList*/, EXT_MESH);
    KGLOG_PROCESS_ERROR(nRetCode);
    uTotalCount = (unsigned)/*vecMeshList*/g_vecSourceList.size();
    /*uFailedCountSave = uFailedCount;*/
    for (unsigned i = 0; i < uTotalCount; ++i)
    {
        char szKeyName[32] = "";
        snprintf(
            szMessage, sizeof(szMessage), 
            "Mesh模型加载检查文件：%s", 
            /*vecMeshList*/g_vecSourceList[i].c_str()
            );
        szMessage[sizeof(szMessage) - 1] = '\0';
        SetWindowText(g_hWnd, szMessage);

        try
        {
            hrRetCode = piModelTable->LoadResourceFromFile(/*vecMeshList*/g_vecSourceList[i].c_str(), 0, 0, (IKG3DResourceBase**)&piModel);
            if (SUCCEEDED(hrRetCode))
            {
                KG3DModel *pModel = (KG3DModel*)piModel;
                if(pModel->GetDetail() == NULL)
                {
                    itoa(uFailedCount, szKeyName, 10);
                    pFile->WriteString("ErrorDetail",szKeyName, /*vecMeshList*/g_vecSourceList[i].c_str());
                    ++uFailedCount;
                    KGLogPrintf(KGLOG_ERR, "%s", g_vecSourceList[i].c_str());
                }
            }
            KG_COM_RELEASE(piModel);
            g_cEngineManager.FrameMove();
        }
        catch (...)
        {
            itoa(uFailedCount, szKeyName, 10);
            pFile->WriteString("Detail_Exception",szKeyName, /*vecMeshList*/g_vecSourceList[i].c_str());
            ++uFailedCount;
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
    pFile->WriteInteger("ErrorDetail", "ErrorNum", uFailedCount);
    pFile->Save(g_cszErrorFileName[EXT_MESH]);

    //uFailedCount += uFailedCountSave;
    //uTotalCount = (unsigned)(vecModelList.size() + vecMeshList.size());
    // snprintf(
    //    szMessage, sizeof(szMessage), 
    //    "模型加载检查完成：共检查模型 %u 个 已发现错误 %u 个，请检查文件%s", 
    //    uTotalCount, uFailedCount, g_cszErrorFileName[TYPE_MDL]
    //);
    //szMessage[sizeof(szMessage) - 1] = '\0';
    //::MessageBox(g_hWnd, szMessage, "检查报告", MB_OK);

    nResult = true;
Exit0:
    KG_COM_RELEASE(piModel);
    KG_COM_RELEASE(pFile);
    g_vecSourceList.clear();
    return nResult;
    
}

//////////////////////////////////////////////////////////////////////////
int TestLoadAllBSP()
{
    int nResult                         = false;
    int nRetCode                        = false;
    HRESULT hrRetCode                   = E_FAIL;
    unsigned uTotalCount                = 0;
    unsigned uFailedCount               = 0;
    unsigned uFailedCountSave           = 0;   
    BYTE *pbyTestFileData   = NULL;
    BYTE *pbySourceFileData = NULL;
    IIniFile *pFile                     = NULL;
    IFile *piTestFile                   = NULL;
    IKG3DModel *piModel                 = NULL;
    IKG3DResourceManager *piModelTable  = NULL;
    char szMessage[MAX_PATH]            = "";
    char szDirPath[MAX_PATH]            = "";
     TCHAR strBSPPathName[MAX_PATH]     = "";
    //vector<string> vecModelList;
    //vector<string> vecMeshList;   
    MSG msg;

    nRetCode = GetCurrentDirectory(MAX_PATH, szDirPath);
    KG_PROCESS_ERROR(nRetCode > 0 && nRetCode < MAX_PATH + 1);
    strcat(szDirPath, "\\data\\source\\*.*");
    szDirPath[MAX_PATH - 1] = '\0';
    //strcpy(szDirPath, "D:\\Kingsoft\\Game\\sword3-products\\trunk\\client\\data\\source\\NPC_source\\*.*");

    piModelTable = g_cEngineManager.Get3DModelTable();
    KGLOG_PROCESS_ERROR(piModelTable);

    pFile = g_OpenIniFile(g_cszErrorFileName[EXT_MESH],false,true);
    KGLOG_PROCESS_ERROR(pFile);

    nRetCode = GetFileList(szDirPath, &g_vecSourceList/*vecMeshList*/, EXT_MESH);
    KGLOG_PROCESS_ERROR(nRetCode);
    //g_vecSourceList.push_back("D:\\Kingsoft\\Game\\trunk\\data\\source\\maps_source\\亭塔\\T_erg风车塔001A.Mesh");
    uTotalCount = (unsigned)/*vecMeshList*/g_vecSourceList.size();

    for (unsigned i = 0; i < uTotalCount; ++i)
    {
        char szKeyName[32] = "";
        snprintf(
            szMessage, sizeof(szMessage), 
            "Mesh模型的BSP文件检查：%s", 
            /*vecMeshList*/g_vecSourceList[i].c_str()
            );
        szMessage[sizeof(szMessage) - 1] = '\0';
        SetWindowText(g_hWnd, szMessage);




        TCHAR strDriver[MAX_PATH];
        TCHAR strPath[MAX_PATH];
        TCHAR strFile[MAX_PATH];


        _splitpath_s(g_vecSourceList[i].c_str(), 
            strDriver,
            MAX_PATH,
            strPath,
            MAX_PATH,
            strFile,
            MAX_PATH,
            NULL,
            0);

        //sprintf_s(strBSPPathName,
        //    MAX_PATH,
        //    "%s%s%s.bsp",
        //    strDriver,
        //    strPath,
        //    strFile);

        //nRetCode = g_IsFileExist(strBSPPathName);

        //if (!nRetCode)
        //{
        //    /*itoa(uFailedCount, szKeyName, 10);
        //    pFile->WriteString("BSP_NotExited",szKeyName, g_vecSourceList[i].c_str());
        //    ++uFailedCount;*/
        //    continue;
        //}

        try
        {
            g_bForceGenerateBspForMesh = /*FALSE*/TRUE;
            hrRetCode = piModelTable->LoadResourceFromFile(/*vecMeshList*/g_vecSourceList[i].c_str(), 0, 0, (IKG3DResourceBase**)&piModel);
            if (FAILED(hrRetCode))
            {
                itoa(uFailedCount, szKeyName, 10);
                pFile->WriteString("ErrorMesh",szKeyName, /*vecMeshList*/g_vecSourceList[i].c_str());
                ++uFailedCount;
            }
            KG_COM_RELEASE(piModel);
            g_cEngineManager.FrameMove();
            g_bForceGenerateBspForMesh = FALSE;
            if (FAILED(hrRetCode))
            {
                continue;
            }

            //cmp
            unsigned uTestFileDataSize      = 0;
            unsigned uSourceFileDataSize    = 0;
            unsigned uSizeRead              = 0;
            

            sprintf_s(strBSPPathName,
                MAX_PATH,
                "%s%s%s.bsp",
                strDriver,
                strPath,
                strFile);

            nRetCode = g_IsFileExist(strBSPPathName);
            if (!nRetCode)
            {
                sprintf_s(strBSPPathName,
                    MAX_PATH,
                    "%s%s%s_test.bsp",
                    strDriver,
                    strPath,
                    strFile);
                nRetCode = g_IsFileExist(strBSPPathName);
                if (nRetCode)
                {
                    itoa(uFailedCount, szKeyName, 10);
                    pFile->WriteString("BSP_NotGenerate",szKeyName, g_vecSourceList[i].c_str());
                    ++uFailedCount;
                    DeleteFile(strBSPPathName);
                }
                continue;
            }

            piTestFile = g_OpenFile(strBSPPathName);
            KGLOG_PROCESS_ERROR(piTestFile);
            uSourceFileDataSize = piTestFile->Size();
            KGLOG_PROCESS_ERROR(uSourceFileDataSize > sizeof(DWORD));
            pbySourceFileData = new BYTE[uSourceFileDataSize];
            KGLOG_PROCESS_ERROR(pbySourceFileData);
            uSizeRead = piTestFile->Read(pbySourceFileData, uSourceFileDataSize);
            KGLOG_PROCESS_ERROR(uSizeRead == uSourceFileDataSize);

            KG_COM_RELEASE(piTestFile);


            sprintf_s(strBSPPathName,
                MAX_PATH,
                "%s%s%s_test.bsp",
                strDriver,
                strPath,
                strFile);

            piTestFile = g_OpenFile(strBSPPathName);
            if (!piTestFile)
            {
                itoa(uFailedCount, szKeyName, 10);
                pFile->WriteString("BSP_Don't Exited",szKeyName, g_vecSourceList[i].c_str());
                ++uFailedCount;
                SAFE_DELETE_ARRAY(pbySourceFileData);
                continue;
            }

            uTestFileDataSize = piTestFile->Size();
            KGLOG_PROCESS_ERROR(uTestFileDataSize > sizeof(DWORD));
            pbyTestFileData = new BYTE[uTestFileDataSize];
            KGLOG_PROCESS_ERROR(pbyTestFileData);
            uSizeRead = piTestFile->Read(pbyTestFileData, uTestFileDataSize);
            KGLOG_PROCESS_ERROR(uSizeRead == uTestFileDataSize);

            KG_COM_RELEASE(piTestFile);

            if (uTestFileDataSize != uSourceFileDataSize)
            {
                itoa(uFailedCount, szKeyName, 10);
                pFile->WriteString("BSP_NotMatchedSize",szKeyName, g_vecSourceList[i].c_str());
                ++uFailedCount;
                DeleteFile(strBSPPathName);
                SAFE_DELETE_ARRAY(pbyTestFileData);
                SAFE_DELETE_ARRAY(pbySourceFileData);
                continue;
            }

            nRetCode = memcmp(pbyTestFileData, pbySourceFileData, uSourceFileDataSize);
            if (nRetCode)
            {
                itoa(uFailedCount, szKeyName, 10);
                pFile->WriteString("BSP_NotMatched",szKeyName, g_vecSourceList[i].c_str());
                ++uFailedCount;
                DeleteFile(strBSPPathName);
                SAFE_DELETE_ARRAY(pbyTestFileData);
                SAFE_DELETE_ARRAY(pbySourceFileData);
                continue;
            }

            DeleteFile(strBSPPathName);
            SAFE_DELETE_ARRAY(pbyTestFileData);
            SAFE_DELETE_ARRAY(pbySourceFileData);


        }
        catch (...)
        {
            itoa(uFailedCount, szKeyName, 10);
            pFile->WriteString("Mesh_Exception",szKeyName, /*vecMeshList*/g_vecSourceList[i].c_str());
            ++uFailedCount;
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

    //uFailedCount += uFailedCountSave;
    //uTotalCount = (unsigned)(vecModelList.size() + vecMeshList.size());
    // snprintf(
    //    szMessage, sizeof(szMessage), 
    //    "模型加载检查完成：共检查模型 %u 个 已发现错误 %u 个，请检查文件%s", 
    //    uTotalCount, uFailedCount, g_cszErrorFileName[TYPE_MDL]
    //);
    //szMessage[sizeof(szMessage) - 1] = '\0';
    //::MessageBox(g_hWnd, szMessage, "检查报告", MB_OK);

    nResult = true;
Exit0:
    if (pFile)
    {
        pFile->WriteInteger("ErrorMesh", "ErrorNum", uFailedCount);
        pFile->Save(g_cszErrorFileName[EXT_MESH]);
    }
    SAFE_DELETE_ARRAY(pbyTestFileData);
    SAFE_DELETE_ARRAY(pbySourceFileData);
    if (strstr(strBSPPathName, "_test.bsp"))
    {
        DeleteFile(strBSPPathName);
    }


    KG_COM_RELEASE(piModel);
    KG_COM_RELEASE(pFile);
    KG_COM_RELEASE(piTestFile);
    g_bForceGenerateBspForMesh = FALSE;
    g_vecSourceList.clear();
    return nResult;
}

//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    //case WM_CHAR:
    //    ProcessKeyboardMessage(wParam);
    //    //KGLOG_PROCESS_ERROR(nRetCode);
    //    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    HRESULT hRetCode        = E_FAIL;
    int nRetCode            = false;
    int nKGLogInitFlag      = false;
    int nKG3DEngineInitFlag = false;    
    int nX          = 0;
    int nY          = 0;
    int nWidth      = 0;
    int nHeight     = 0;
    char szBuffer[MAX_PATH] = "";
    KGLOG_PARAM LogParam = {"logs", "KG3DEngine", KGLOG_OPTION_FILE, 65536};
    WNDCLASSEX wcex = {0};
    MSG msg = {0};

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    nRetCode = KGLogInit(LogParam, NULL);
    KG_PROCESS_ERROR(nRetCode);
    nKGLogInitFlag = true;

    g_SetRootPath(NULL);
    //SetLowerConfig();

	wcex.cbSize         = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.hInstance		= hInstance;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= TEXT("KG3DEnginePerformanceAnalyzer");
	RegisterClassEx(&wcex);

    nX = 0;
    nY = 0;
    nWidth = 800;
    nHeight = 600;

    g_hWnd = ::CreateWindow( 
        wcex.lpszClassName,  TEXT("3D引擎性能分析程序"),  WS_OVERLAPPEDWINDOW | WS_VISIBLE/*WS_CAPTION | WS_SYSMENU*/,
        nX, nY, nWidth, nHeight,
        NULL, NULL, hInstance, NULL 
    );
    KGLOG_PROCESS_ERROR(g_hWnd);

    ShowWindow(g_hWnd, true/*nCmdShow*/);
    UpdateWindow(g_hWnd);

    //ASSERT(FALSE);
    //--------------------------------
    hRetCode = g_cEngineManager.Init(0, KG3DENG_CLIENT, g_hWnd, g_hWnd);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    nKG3DEngineInitFlag = true;

    for (int nIndex = 0; nIndex < EXT_T0TAL; ++nIndex)
    {
        DeleteFile(g_cszErrorFileName[nIndex]);
    }    
    
    ::MessageBox(NULL, "搜索所有列表,进行Mesh,Mdl,Speedtree,Sfx,Mtl,Tani的检查，请耐心等待……", "提示", MB_OK);
    //TestScene();
    //TestLoadAllMesh();
    //KGLogPrintf(KGLOG_INFO, "Load Mesh Over\n");
    //TestLoadAllDetail();
    //KGLogPrintf(KGLOG_INFO, "Load Detail Over\n");
    //TestLoadAllMDL();
    //KGLogPrintf(KGLOG_INFO, "Load MDL Over\n");
    //TestLoadAllSPEEDTREE();
    //KGLogPrintf(KGLOG_INFO, "Load SPEEDTREE Over\n");
    //TestLoadAllSFX();
    //KGLogPrintf(KGLOG_INFO, "Load SFX Over\n");
    //TestLoadAllMaterial();
    //TestLoadAllSound();
    TestLoadAllBSP();
    KGLogPrintf(KGLOG_INFO, "Load Bsp Over\n");

    snprintf(
        szBuffer, sizeof(szBuffer), 
        "检查完毕，请查看文件%s", 
        g_cszErrorFileName
    );
    szBuffer[sizeof(szBuffer) - 1] = '\0';
    ::MessageBox(NULL, szBuffer, "提示", MB_OK);

Exit0:
    if (nKG3DEngineInitFlag)
    {
        g_cEngineManager.UnInit();
        nKG3DEngineInitFlag = false;
    }
    if (nKGLogInitFlag)
    {
        KGLogUnInit(NULL);
        nKGLogInitFlag = false;
    } 
	return (int) msg.wParam;
}

#endif//KG3DENGINE_PERFORMANCE_ANALYZE_TEST

