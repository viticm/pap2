// DllMergeTga.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "DllMergeTga.h"
#include "KWndNameList.h"
#include "KTgaBlockLoader.h"
#include "KTgaManager.h"
#include "KPickBox.h"
#include "KShow.h"
#include "KAnimateManager.h"
#include "KSaveManager.h"
#include "KWndAnimateList.h"
#include "KWndFrameList.h"
#include "KIconManager.h"

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

// This is an example of an exported variable
DLLMERGETGA_API int g_nInited=0;

// This is an example of an exported function.
DLLMERGETGA_API int TgaToUitex(TCHAR* strFileTga, TCHAR* szPath)
{
	int nRetCode = false;
	if (!g_nInited)
	{
		g_nInited = 1;
		nRetCode = KTgaBlockLoader::Init();
		KG_PROCESS_ERROR(nRetCode);

		nRetCode = KTgaManager::Init();
		KG_PROCESS_ERROR(nRetCode);

		nRetCode = KAnimateManager::Init();
		KG_PROCESS_ERROR(nRetCode);

		nRetCode = KIconManager::Init();
		KG_PROCESS_ERROR(nRetCode);

		nRetCode = KPickBox::Init();
		KG_PROCESS_ERROR(nRetCode);

		nRetCode = KShow::Init();
		KG_PROCESS_ERROR(nRetCode);

		nRetCode = KSaveManager::Init();
		KG_PROCESS_ERROR(nRetCode);
	}

	
	int nIndex = 0;
	
	//KTgaManager::GetSelf().UnMakeAll();
	KSaveManager::GetSelf().SetLastOpenOrSaveDir(strFileTga);
	KTgaBlockLoader::GetSelf().LoadTga(strFileTga);

	KSaveManager::GetSelf().SetLastOpenOrSaveDir(szPath);
	KTgaManager::GetSelf().UnMakeAll();
	while(true)
	{
		TCHAR szName[MAX_PATH];
		szName[0] = _T('\0');
		TCHAR szFile[MAX_PATH];
		LPTSTR pcszClip = NULL;
		LPTSTR pcszTemp = NULL;
		KTgaBlock *pBlock = KTgaManager::GetSelf().Get(0);
		if (!pBlock)
			break;
		pBlock->SetTop(0);
		pBlock->SetLeft(0);
		nRetCode = KTgaManager::GetSelf().TryDropBlock(0);
		KG_PROCESS_ERROR(nRetCode);

		pBlock->GetName(szName, sizeof(szName) / sizeof(TCHAR));
		pcszClip = _tcsrchr(szName, _T('\\'));
		if (!pcszClip)
			pcszClip = szName;
		pcszTemp = _tcsrchr(pcszClip, _T('/'));
		if (pcszTemp)
			pcszClip = pcszTemp;
		pcszTemp = _tcsrchr(pcszClip, _T('.'));
		if (pcszTemp)
			*pcszTemp = _T('\0');
		if(pcszClip[0] == _T('\0'))
		{
			_sntprintf(szFile, MAX_PATH, _T("%s\\%d%s"), szPath, nIndex, _T(".UITex"));
			nIndex++;
		}
		else
		{
			_sntprintf(szFile, MAX_PATH, _T("%s%s%s"), szPath, pcszClip, _T(".UITex"));
		}
		nRetCode = KSaveManager::GetSelf().SetCurrentEditFile(szFile);
		KG_PROCESS_ERROR(nRetCode);
		nRetCode = KSaveManager::GetSelf().Save();
		KG_PROCESS_ERROR(nRetCode);

		KTgaManager::GetSelf().RemoveThePosedBlock();

		KTgaManager::GetSelf().UpDataListView();
		KTgaManager::GetSelf().UpDataShow();
	}
Exit0:
	return nRetCode;
}

// This is the constructor of a class that has been exported.
// see DllMergeTga.h for the class definition
CDllMergeTga::CDllMergeTga()
{
	return;
}
