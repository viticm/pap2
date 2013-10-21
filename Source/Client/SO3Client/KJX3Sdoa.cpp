#include "stdafx.h"
#include "./KJX3Sdoa.h"
#include "./KJX3ClientEvent.h"
#include "./ClientString.h"
#include "./SO3Client.h"
#include "IKG3DSoundShell.h"

static int InitJX3SdoaDll(KJX3Sdoa* pJX3Sdoa, HMODULE hmodule)
{
    int nRetCode = false;
    int nResult = false;

    memset(pJX3Sdoa, 0, sizeof(KJX3Sdoa));

    pJX3Sdoa->igwInitialize = (LPigwInitialize)GetProcAddress(hmodule, "igwInitialize");
    KGLOG_PROCESS_ERROR(pJX3Sdoa->igwInitialize);

    pJX3Sdoa->igwGetModule = (LPigwGetModule)GetProcAddress(hmodule, "igwGetModule");
    KGLOG_PROCESS_ERROR(pJX3Sdoa->igwGetModule);

    pJX3Sdoa->igwTerminal = (LPigwTerminal)GetProcAddress(hmodule, "igwTerminal");
    KGLOG_PROCESS_ERROR(pJX3Sdoa->igwTerminal);

    nResult = true;
Exit0:
    if (!nResult)
    {
        pJX3Sdoa->igwInitialize = NULL;
        pJX3Sdoa->igwGetModule = NULL;
        pJX3Sdoa->igwTerminal = NULL;
    }
    return nResult;
}

static void WINAPI SetAudioSoundVolume(DWORD dwVolume)
{
    HRESULT hr = E_FAIL;
    IKG3DSoundShell* pSoundShell = NULL;

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

	if (dwVolume >= 100)
		dwVolume = 100;

    hr = pSoundShell->SetVolume((float)dwVolume / 100.0f);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    return;
}

static DWORD WINAPI GetAudioSoundVolume()
{
    HRESULT hr = E_FAIL;
    IKG3DSoundShell* pSoundShell = NULL;
    float fVolume = 0.0f;

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    fVolume = pSoundShell->GetVolume();

Exit0:
    return (DWORD)(fVolume * 100.0f + 0.5f);
}

static int SetVolumeScale(IKG3DSoundShell* pSoundShell, int nType, float fScale)
{
    HRESULT hr = E_FAIL;
    int nResult = false;
    float fVolume = 0.0f;

    ASSERT(pSoundShell);

    fVolume = pSoundShell->GetVolume(nType);

    fVolume *= fScale;

    if (fVolume > 1.0f)
        fVolume = 1.0f;
    if (fVolume < 0.0f)
        fVolume = 0.0f;

    hr = pSoundShell->SetVolume(nType, fVolume);
    KGLOG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    return nResult;
}

static void WINAPI SetAudioEffectVolume(DWORD dwVolume)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    IKG3DSoundShell* pSoundShell = NULL;
    float fEffectVolume = 0.0f;
    float fMainVolume = 0.0f;
    float fScale = 0.0f;

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

	if (dwVolume >= 100)
		dwVolume = 100;

    fEffectVolume = (float)dwVolume / 100.0f;
    fMainVolume = pSoundShell->GetVolume();

    if (fMainVolume > 0.0f)
        fScale = fEffectVolume / fMainVolume;
    else
        fScale = 0.0f;

    nRetCode = SetVolumeScale(pSoundShell, UI_SOUND, fScale);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = SetVolumeScale(pSoundShell, UI_ERROR_SOUND, fScale);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = SetVolumeScale(pSoundShell, SCENE_SOUND, fScale);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = SetVolumeScale(pSoundShell, CHARACTER_SOUND, fScale);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = SetVolumeScale(pSoundShell, CHARACTER_SPEAK, fScale);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit0:
    return;
}

static DWORD WINAPI GetAudioEffectVolume()
{
    HRESULT hr = E_FAIL;
    IKG3DSoundShell* pSoundShell = NULL;
    float fVolume = 0.0f;

    pSoundShell = g_SO3Client.m_pSoundShell;
    KG_PROCESS_ERROR(pSoundShell);

    fVolume = pSoundShell->GetVolume(SCENE_SOUND);

Exit0:
    return (DWORD)(fVolume * 100.0f + 0.5f);
}

int InitJX3Sdoa(KJX3Sdoa* pJX3Sdoa, HMODULE hmodule, LPCWSTR pcwszVersion)
{
    int nRetCode = false;
    int nResult = false;
    AppInfo appinfo;

    KGLOG_PROCESS_ERROR(pJX3Sdoa);

    memset(pJX3Sdoa, 0, sizeof(KJX3Sdoa));

    nRetCode = InitJX3SdoaDll(pJX3Sdoa, hmodule);
    KGLOG_PROCESS_ERROR(nRetCode);

    appinfo.cbSize = sizeof(AppInfo);
	appinfo.nAppID = 615;				        // 盛大统一游戏ID
	appinfo.pwcsAppName = GAME_TITLE_NAME_W;	// 游戏名称
	appinfo.pwcsAppVer = pcwszVersion;		   	// 游戏版本
	appinfo.nRenderType = SDOA_RENDERTYPE_D3D9;	// 游戏图形引擎类型 (取值SDOA_RENDERTYPE_UNKNOWN..SDOA_RENDERTYPE_OPENGL)
	appinfo.nMaxUser = JX3_MAX_USER;			// 游戏可同时在一台机器上游戏的最大人数（通常为1，例如:KOF应该是2）
	appinfo.nAreaId = -1;			            // 游戏区ID	(注意：不可用时传-1)
	appinfo.nGroupId = -1;                      // 游戏组ID	(注意：不可用时传-1)

    nRetCode = pJX3Sdoa->igwInitialize(SDOA_SDK_VERSION, &appinfo) == SDOA_OK;
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pJX3Sdoa->igwGetModule(__uuidof(ISDOADx9), (void**)&pJX3Sdoa->pSDOADx9);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pJX3Sdoa->igwGetModule(__uuidof(ISDOAApp), (void**)&pJX3Sdoa->pSDOAApp);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pJX3Sdoa->igwGetModule(__uuidof(ISDOAAppUtils), (void**)&pJX3Sdoa->pSDOAAppUtils);
    KGLOG_PROCESS_ERROR(nRetCode);

    KG_ASSERT_EXIT(pJX3Sdoa->pSDOAAppUtils);

    pJX3Sdoa->pSDOAAppUtils->SetAudioSetting(GetAudioSoundVolume, SetAudioSoundVolume, GetAudioEffectVolume, SetAudioEffectVolume);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (pJX3Sdoa)
        {
            SAFE_RELEASE(pJX3Sdoa->pSDOAAppUtils);
            SAFE_RELEASE(pJX3Sdoa->pSDOAApp);
            SAFE_RELEASE(pJX3Sdoa->pSDOADx9);
        }
    }
    return nResult;
}

int UnInitJX3Sdoa(KJX3Sdoa* pJX3Sdoa)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pJX3Sdoa);

    SAFE_RELEASE(pJX3Sdoa->pSDOAAppUtils);
    SAFE_RELEASE(pJX3Sdoa->pSDOAApp);
    SAFE_RELEASE(pJX3Sdoa->pSDOADx9);

    pJX3Sdoa->igwGetModule = NULL;
    pJX3Sdoa->igwInitialize = NULL;

    if (pJX3Sdoa->igwTerminal)
    {
        pJX3Sdoa->igwTerminal();
        pJX3Sdoa->igwTerminal = NULL;
    }

    nResult = true;
Exit0:
    return nResult;
}

int HandleJX3SdoaMsgProc(KJX3Sdoa* pJX3Sdoa, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pJX3Sdoa);
    KG_ASSERT_EXIT(plResult);

    if (pJX3Sdoa->pSDOADx9)
    {
        nRetCode = pJX3Sdoa->pSDOADx9->OnWindowProc(hwnd, uMsg, wParam, lParam, plResult) == SDOA_OK;
        KG_PROCESS_SUCCESS(nRetCode);
    }

    if (uMsg == SDOA_WM_CLIENT_RUN)
    {
        nRetCode = FireJX3SdoaInitFinishedEvent();
        KGLOG_PROCESS_ERROR(nRetCode);

        goto Exit1;
    }

    goto Exit0;

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

static BOOL CALLBACK JX3SdoaLoginCallback(int nErrorCode, LoginResult const* pLoginResult, int nUserData, int nReserved) 
{ 
    int nRetCode = false;
    int nResult = false;
    KJX3Sdoa* pJX3Sdoa = NULL;
    BOOL bLogin = FALSE;

    pJX3Sdoa = (KJX3Sdoa*)(LONG_PTR)(__int3264)nUserData;
    KGLOG_PROCESS_ERROR(pJX3Sdoa);

    bLogin = SDOA_ERRORCODE_OK == nErrorCode;

    pJX3Sdoa->bLogin = bLogin;

	if (bLogin) 
	{ 
        KGLOG_PROCESS_ERROR(pLoginResult);

        pJX3Sdoa->Result = *pLoginResult;
	} 

    nRetCode = FireJX3SdoaEvent(nErrorCode, pLoginResult, nUserData, nReserved);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = bLogin;
Exit0:
	return nResult; // 常规逻辑，登录成功则关闭登录对话框
} 

int JX3SdoaLogin(KJX3Sdoa* pJX3Sdoa)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pJX3Sdoa);
    KGLOG_PROCESS_ERROR(pJX3Sdoa->pSDOAApp);

    nRetCode = pJX3Sdoa->pSDOAApp->ShowLoginDialog(JX3SdoaLoginCallback, (__int3264)(LONG_PTR)pJX3Sdoa, 0) == SDOA_OK;
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int JX3SdoaLogout(KJX3Sdoa* pJX3Sdoa)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pJX3Sdoa);

    KG_PROCESS_SUCCESS(!pJX3Sdoa->bLogin);

    pJX3Sdoa->bLogin = false;

    if (pJX3Sdoa->pSDOAApp)
	{
		pJX3Sdoa->pSDOAApp->Logout();
	}

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int SetJX3SodaRoleInfo(KJX3Sdoa* pJX3Sdoa, RoleInfo const* pRoleInfo)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pJX3Sdoa);
    KGLOG_PROCESS_ERROR(pRoleInfo);

    KGLOG_PROCESS_ERROR(pJX3Sdoa->bLogin);

    if (pJX3Sdoa->pSDOAApp)
    {
        pJX3Sdoa->pSDOAApp->SetRoleInfo(pRoleInfo);
    }

    nResult = true;
Exit0:
    return nResult;
}

int GetJX3SdoaWinProperty(KJX3Sdoa* pJX3Sdoa, LPCWSTR pwcsWinName, SDOAWinProperty* pProperty)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pJX3Sdoa);
    KGLOG_PROCESS_ERROR(pwcsWinName);
    KGLOG_PROCESS_ERROR(pProperty);

    if (pJX3Sdoa->pSDOAApp)
    {
        nRetCode = pJX3Sdoa->pSDOAApp->GetWinProperty(pwcsWinName, pProperty);
        KGLOG_PROCESS_ERROR(nRetCode == SDOA_OK);
    }

    nResult = true;
Exit0:
    return nResult;
}

int SetJX3SdoaWinProperty(KJX3Sdoa* pJX3Sdoa, LPCWSTR pwcsWinName, SDOAWinProperty* pProperty)
{
    int nRetCode = false;
    int nResult = false;

    KGLOG_PROCESS_ERROR(pJX3Sdoa);
    KGLOG_PROCESS_ERROR(pwcsWinName);
    KGLOG_PROCESS_ERROR(pProperty);

    if (pJX3Sdoa->pSDOAApp)
    {
        nRetCode = pJX3Sdoa->pSDOAApp->SetWinProperty(pwcsWinName, pProperty);
        KGLOG_PROCESS_ERROR(nRetCode == SDOA_OK);
    }

    nResult = true;
Exit0:
    return nResult;
}

int JX3SdoaWinExists(KJX3Sdoa* pJX3Sdoa, LPCWSTR pwcsWinName, PBOOL pbExists)
{
    int nRetCode = false;
    int nResult = false;
    SDOAWinProperty Property = { 0 };

    KGLOG_PROCESS_ERROR(pJX3Sdoa);
    KGLOG_PROCESS_ERROR(pwcsWinName);
    KGLOG_PROCESS_ERROR(pbExists);

    if (pJX3Sdoa->pSDOAApp)
    {
        nRetCode = pJX3Sdoa->pSDOAApp->GetWinProperty(pwcsWinName, &Property);
        *pbExists = nRetCode == SDOA_OK;
    }

    nResult = true;
Exit0:
    return nResult;
}

int SetJX3SdoaScreenStatus(KJX3Sdoa* pJX3Sdoa, int nStatus)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pJX3Sdoa);

    if (pJX3Sdoa->pSDOAApp)
    {
        pJX3Sdoa->pSDOAApp->SetScreenStatus(nStatus);
    }

    nResult = true;
Exit0:
    return nResult;
}
