////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : kaddonmgr.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-11-9 9:28:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "kaddonmgr.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"


////////////////////////////////////////////////////////////////////////////////

namespace UI
{

	KAddOnMgr g_AddOnMgr;

	int KAddOnMgr::Init()
	{
		int nResult = false;
        LPCSTR pcszPath = NULL;
		std::string strPath = "";
		std::string strFile = "";
		char szRootPath[MAX_PATH];
		int nLen = 0;
        g_GetPriorRootPath(szRootPath);
        if (szRootPath[0] == '\0')
		{
            g_GetRootPath(szRootPath);
		}

		m_nEnableOverdueAddOn = 0;
		m_aAddOn.clear();
		
		szRootPath[_countof(szRootPath) - 1] = '\0';
		nLen = (int)strlen(szRootPath);
		KGLOG_PROCESS_ERROR(nLen > 0 && nLen < MAX_PATH - 2);

		if (szRootPath[nLen - 1] != '\\' && szRootPath[nLen - 1] != '/')
		{
			szRootPath[nLen] = '\\';
			szRootPath[nLen + 1] = '\0';
		}

		strPath += szRootPath;

		pcszPath = g_pUI->m_FilePathMgr.GetFilePath("AddOnScripLibDir");
		KGLOG_PROCESS_ERROR(pcszPath);

		strPath += pcszPath;
		strPath += "\\*";

		WIN32_FIND_DATA FindFileData;
		HANDLE hFile = ::FindFirstFile(strPath.c_str(), &FindFileData);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
				strcmp(FindFileData.cFileName, ".") != 0 &&
				strcmp(FindFileData.cFileName, "..") != 0 )
			{
				strFile = "";
				strFile += pcszPath;
				strFile += "\\";
				strFile += FindFileData.cFileName;
				strFile += "\\";
				strFile += "info.ini";
				Append(strFile.c_str(), FindFileData.cFileName);
			}
			while (::FindNextFile(hFile, &FindFileData)) 
			{
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
					strcmp(FindFileData.cFileName, ".") != 0 &&
					strcmp(FindFileData.cFileName, "..") != 0 )
				{
					strFile = "";
					strFile += pcszPath;
					strFile += "\\";
					strFile += FindFileData.cFileName;
					strFile += "\\";
					strFile += "info.ini";
					Append(strFile.c_str(), FindFileData.cFileName);
				}
			}
			::FindClose(hFile);
			hFile = NULL;
		}

		nResult = true;
Exit0:
		return nResult;
	}

	int KAddOnMgr::Append(LPCSTR pcszFile, LPCSTR pcszFolder)
	{
		int nResult = false;
		IIniFile *pIni = NULL;
		char szVaule[512];

		KG_PROCESS_ERROR(pcszFile);
		KG_PROCESS_ERROR(pcszFolder);

		KG_PROCESS_ERROR(g_IsFileExist(pcszFile));

		pIni = g_OpenIniFile(pcszFile);
		KG_PROCESS_ERROR(pIni);

		if (pIni->IsSectionExist(pcszFolder))
		{
			int nSize = (int)m_aAddOn.size();
			m_aAddOn.resize(nSize + 1);
			KADDONINFO & rAddOnInfo = m_aAddOn[nSize];

			rAddOnInfo.strID = pcszFolder;
			pIni->GetString(pcszFolder, "name", "", szVaule, _countof(szVaule));
			rAddOnInfo.strName = szVaule;
			pIni->GetString(pcszFolder, "desc", "", szVaule, _countof(szVaule));
			rAddOnInfo.strDesc = szVaule;
			pIni->GetString(pcszFolder, "dependence", "", szVaule, _countof(szVaule));
			rAddOnInfo.strDependence = szVaule;
			pIni->GetString(pcszFolder, "weak_dependence", "", szVaule, _countof(szVaule));
			rAddOnInfo.strWeakDependence = szVaule;
			pIni->GetString(pcszFolder, "version", "", szVaule, _countof(szVaule));
			rAddOnInfo.strVersion = szVaule;

			pIni->GetInteger(pcszFolder, "default", 0, &rAddOnInfo.nDefault);
			rAddOnInfo.nDisable = false;
			rAddOnInfo.nLoad = false;
			rAddOnInfo.nLoaded = false;

			char szKey[32];
			int nIndex = 0;
			while (true)
			{
				sprintf(szKey, "lua_%d", nIndex);
				++nIndex;
				if (!pIni->GetString(pcszFolder, szKey, "", szVaule, _countof(szVaule)))
					break;
				rAddOnInfo.aStrFile.push_back(szVaule);
			}
		}

		nResult = true;
Exit0:
		KG_COM_RELEASE(pIni);
		return nResult;
	}

	int KAddOnMgr::GetCount()
	{
		return (int)(m_aAddOn.size());
	}

	int KAddOnMgr::Enable(int nIndex, int nEnable)
	{
		int nResult = false;

		KG_PROCESS_ERROR(nIndex >= 0 && nIndex < (int)(m_aAddOn.size()));

		m_aAddOn[nIndex].nLoad = nEnable;

		nResult = true;
Exit0:
		return nResult;
	}

	int KAddOnMgr::Disable(int nIndex, int nDisable)
	{
		int nResult = false;

		KG_PROCESS_ERROR(nIndex >= 0 && nIndex < (int)(m_aAddOn.size()));

		m_aAddOn[nIndex].nDisable = nDisable;

		nResult = true;
Exit0:
		return nResult;
	}

	KAddOnMgr::KADDONINFO * KAddOnMgr::GetAddOnInfo(int nIndex)
	{

		if (nIndex >= 0 && nIndex < (int)(m_aAddOn.size()))
			return &(m_aAddOn[nIndex]);
		return NULL;
	}

	int KAddOnMgr::LoadAddOn()
	{
		int nCount = (int)m_aAddOn.size();
		for (int i = 0; i < nCount; ++i)
		{
			LoadAddOn(i, FALSE);
		}

		return true;
	}

	int KAddOnMgr::EnableOverdueAddOn(int nEnable)
	{
		m_nEnableOverdueAddOn = nEnable;
		return true;
	}

	int KAddOnMgr::IsEnableOverdueAddOn()
	{
		return m_nEnableOverdueAddOn;
	}

	int KAddOnMgr::SetCurrentAddOnVersion(const char * pcszVersion)
	{
		if (!pcszVersion)
		{
			pcszVersion = '\0';
		}
		m_strCurrentAddOnVersion = pcszVersion;
		return TRUE;
	}

	int KAddOnMgr::LoadAddOn(const char * pcszID, BOOL bForce)
	{
		int nResult = false;
		int nIndex = -1;

		KG_PROCESS_ERROR(pcszID);
		KG_PROCESS_ERROR(*pcszID != L'\0');

		nIndex = GetIndexByID(pcszID);
		KG_PROCESS_ERROR(nIndex >= 0 && nIndex < (int)m_aAddOn.size());
		nResult = LoadAddOn(nIndex, bForce);
Exit0:
		return nResult;
	}

	int KAddOnMgr::LoadAddOn(int nIndex, BOOL bForce)
	{
		int nResult = false;
		KADDONINFO * pAddOnInfo = NULL;

		KG_PROCESS_ERROR(nIndex >= 0 && nIndex < (int)m_aAddOn.size());

		pAddOnInfo = &m_aAddOn[nIndex];

		KG_PROCESS_SUCCESS(pAddOnInfo->nLoaded);
		if (!bForce)
		{
			KG_PROCESS_ERROR(pAddOnInfo->nLoad || (pAddOnInfo->nDefault && !pAddOnInfo->nDisable));
			if (!m_nEnableOverdueAddOn)
			{
				KG_PROCESS_ERROR(pAddOnInfo->strVersion == m_strCurrentAddOnVersion);
			}
		}
		KG_PROCESS_ERROR(bForce || pAddOnInfo->nLoad || (pAddOnInfo->nDefault && !pAddOnInfo->nDisable));

		if (pAddOnInfo->strDependence.c_str())
		{
			std::vector<int> aIndex;
			ATL::CA2W wszDependence(pAddOnInfo->strDependence.c_str(), GetCodePage());
			WCHAR * pwszDependence = wszDependence;
			WCHAR * pcwszClip = wcschr(pwszDependence, L';');
			while (pcwszClip)
			{
				*pcwszClip = L'\0';

				if (*pwszDependence)
				{
					int nDependenceIndex = GetIndexByID(ATL::CW2A(pwszDependence, GetCodePage()));
					KG_PROCESS_ERROR(nDependenceIndex >= 0 && nDependenceIndex < (int)m_aAddOn.size());
					aIndex.push_back(nDependenceIndex);
				}

				pwszDependence = pcwszClip + 1;
				pcwszClip = wcschr(pwszDependence, L';');
			}
			if (*pwszDependence)
			{
				int nDependenceIndex = GetIndexByID(ATL::CW2A(pwszDependence, GetCodePage()));
				KG_PROCESS_ERROR(nDependenceIndex >= 0 && nDependenceIndex < (int)m_aAddOn.size());
				aIndex.push_back(nDependenceIndex);
			}

			for (std::vector<int>::iterator it = aIndex.begin(); it != aIndex.end(); ++it)
			{
				int nRetCode = LoadAddOn(*it, TRUE);
				KG_PROCESS_ERROR(nRetCode);
			}
		}

		if (pAddOnInfo->strWeakDependence.c_str())
		{
			ATL::CA2W wszDependence(pAddOnInfo->strWeakDependence.c_str(), GetCodePage());
			WCHAR * pwszDependence = wszDependence;
			WCHAR * pcwszClip = wcschr(pwszDependence, L';');
			while (pcwszClip)
			{
				*pcwszClip = L'\0';

				LoadAddOn(ATL::CW2A(pwszDependence, GetCodePage()), TRUE);

				pwszDependence = pcwszClip + 1;
				pcwszClip = wcschr(pwszDependence, L';');
			}
			LoadAddOn(ATL::CW2A(pwszDependence, GetCodePage()), TRUE);
		}


		for (std::vector<std::string>::iterator itFile = pAddOnInfo->aStrFile.begin(); 
			itFile != pAddOnInfo->aStrFile.end(); ++itFile)
		{
			g_pUI->m_Script.LoadScriptFile((*itFile).c_str(), STR_ADD_ON_ENV);
		}

		pAddOnInfo->nLoaded = true;

Exit1:
		nResult = true;
Exit0:
		return nResult;
	}

	int KAddOnMgr::GetIndexByID(const char * pcwszID)
	{
		int nSize = 0;
		KG_PROCESS_ERROR(pcwszID);
		
		nSize = (int)m_aAddOn.size();
		for (int i = 0; i < nSize; ++i)
		{
			KADDONINFO & rAddOnInfo = m_aAddOn[i];
			if (strcmp(rAddOnInfo.strID.c_str(), pcwszID) == 0)
				return i;
		}

Exit0:
		return -1;
	}

}