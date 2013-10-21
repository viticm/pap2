///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KUpdateEntryParser.cpp
// Creator  : chengzhaohui
// Date     : 2007-10-18
// Comment  :
//
///////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "KVersionParser.h"
#include "KVersionNumber.h"

namespace
{
    tstring::size_type FindAtCount(const tstring& strSrc, const tstring& strPattern, int nFindCount)
    {        
        int nCount = 0;
        tstring::size_type pos = strSrc.find(strPattern);
        while(++nCount < nFindCount)
        {
            pos += strPattern.length();
            pos = strSrc.find(strPattern, pos);
        }
        return pos;
    }
}

KVersionParser::KVersionParser(void)
{  
    m_nNeedUpdateFlag = FALSE;
    m_nCanDownloadPackFlag = FALSE;
	m_nBestPath = false;
	m_nCountList = 0;

	memset(m_pZipFileList, 0 ,sizeof(KZipFileInfo*) * MAX_PATH);
}

KVersionParser::~KVersionParser(void)
{
}

int KVersionParser::Parse(const tstring &strEntryFile, const tstring &strCurrentVersion)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

	m_strCurrentVersion = strCurrentVersion;
    nRetCode = ParseLatestVersion(strEntryFile, strCurrentVersion);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = ParseUpdatePack(strEntryFile, strCurrentVersion);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    _tremove(strEntryFile.c_str());
    return nResult;
}

int KVersionParser::ParseLatestVersion(
    const tstring &strEntryFile, 
    const tstring &strCurrentVersion
)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    TCHAR szVersion[MAX_VERSION_LENGTH];

    nRetCode = GetPrivateProfileString(
        SECTION_ENTRY_VERSION, KEY_ENTRY_VERSION, _T(""), 
        szVersion, sizeof(szVersion) / sizeof(TCHAR),
        strEntryFile.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    m_strLatestVersion = szVersion;
	{
		KVersionNumber Ver1(m_strLatestVersion.c_str());
		KVersionNumber Ver2(strCurrentVersion.c_str());
		m_nNeedUpdateFlag = (Ver1 > Ver2);
	}	

	nResult = TRUE;
Exit0:
    return nResult;
}

int KVersionParser::ParseUpdatePack(
    const tstring &strEntryFile, 
    const tstring &strCurrentVersion
)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;
    TCHAR szPackName[MAX_PACK_NAME_LENGTH];
    TCHAR szPackMD5[MD5_LENGTH + 1];
    TCHAR szPackKey[MAX_PACK_NAME_LENGTH];
    tstring strBestPack;
    tstring strBestPackVersion;

    KG_PROCESS_SUCCESS(!m_nNeedUpdateFlag);

    for (int i = 1; TRUE; ++i)
    {
        tstring strTmpPackVersion;
		tstring strFileLastVersion;
        _sntprintf(szPackKey, sizeof(szPackKey) / sizeof(TCHAR),
            _T("%s%d"), KEY_ENTRY_PACK_PREFIX, i);
        szPackKey[sizeof(szPackKey) /  sizeof(TCHAR) - 1] = _T('\0');

        nRetCode = GetPrivateProfileString(
            SECTION_ENTRY_PACK, szPackKey, _T(""), 
            szPackName, sizeof(szPackName) / sizeof(TCHAR),
            strEntryFile.c_str()
        );
        if (nRetCode <= 0)
            break;

        _tcslwr(szPackName);

        nRetCode = ParsePackName(tstring(szPackName), strTmpPackVersion, strFileLastVersion);
		KGLOG_PROCESS_ERROR(nRetCode);

		_sntprintf(szPackKey, sizeof(szPackKey) / sizeof(TCHAR),
			_T("%s%d"), KEY_ENTRY_PACKMD5_PREFIX, i);
		szPackKey[sizeof(szPackKey) /  sizeof(TCHAR) - 1] = _T('\0');

		nRetCode = GetPrivateProfileString(
			SECTION_ENTRY_PACK, szPackKey, _T(""), 
			szPackMD5, sizeof(szPackMD5) / sizeof(TCHAR),
			strEntryFile.c_str()
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
		_tcslwr(szPackMD5);

		nRetCode = SaveUpdatePath(szPackName, strTmpPackVersion, strFileLastVersion, szPackMD5);
		KGLOG_PROCESS_ERROR(nRetCode);
    }
	nRetCode = AnalysisPath(strCurrentVersion);
	KGLOG_PROCESS_ERROR(nRetCode);
	if (m_nCountList == 0)
	{
		m_nCanDownloadPackFlag = FALSE;
		goto Exit1;
	}

    m_nCanDownloadPackFlag  = TRUE;
    
Exit1:
    nResult = TRUE;
Exit0:
	if (!nResult)
		CleanUp();
    return nResult;
}

int KVersionParser::ParsePackName(const tstring &strPackName, tstring &strFilePrevVer, tstring &strFileLastVer)
{
    int nRetCode = FALSE;
    int nResult  = FALSE;

    TCHAR szLatestVersion[MAX_VERSION_LENGTH];
    TCHAR szPackVersion[MAX_VERSION_LENGTH];

    tstring strTmpPackName = strPackName;
    tstring::size_type uPosition = 0;
    tstring::size_type uPosition1 = 0;

    uPosition = strTmpPackName.find(UPDATE_PACK_NAME_PREFIX);
    KGLOG_PROCESS_ERROR(uPosition != tstring::npos);

    nRetCode = (int)_tcslen(UPDATE_PACK_NAME_PREFIX);
    strTmpPackName.erase(0, uPosition + nRetCode);

    uPosition = strTmpPackName.find(_T("-"));
    KGLOG_PROCESS_ERROR(uPosition != tstring::npos);
    KGLOG_PROCESS_ERROR(uPosition < sizeof(szPackVersion) / sizeof(TCHAR));
    strTmpPackName.copy(szPackVersion, uPosition);
    szPackVersion[uPosition] = _T('\0');

    uPosition = FindAtCount(strTmpPackName, _T("-"), 2);
    KGLOG_PROCESS_ERROR(uPosition != tstring::npos);
    uPosition1 = FindAtCount(strTmpPackName, _T("_"), 1);
    KGLOG_PROCESS_ERROR(uPosition != tstring::npos);
    KGLOG_PROCESS_ERROR(uPosition1 - uPosition - 1 < sizeof(szLatestVersion) / sizeof(TCHAR));
    strTmpPackName.copy(szLatestVersion, uPosition1 - uPosition - 1, uPosition + 1);
    szLatestVersion[uPosition1 - uPosition - 1] = _T('\0');
	
	strFilePrevVer = szPackVersion;
    strFileLastVer = szLatestVersion;

    nResult = TRUE;
Exit0:
    return nResult;
}

int KVersionParser::SaveUpdatePath(const tstring &strPackName, const tstring &strPrevVersion,
								   const tstring &strLastVersion, const tstring &strCurrentMD5)
{
	int nRetCode = false;
	int nResult  = false;	
	PKFileInfo pkFileInfo = NULL;
	 
	pkFileInfo = m_UpdatePath[strPrevVersion];

	if (pkFileInfo)
	{
 		KVersionNumber Ver1(strLastVersion.c_str());
 		KVersionNumber Ver2(pkFileInfo->strVersion.c_str());
 		nRetCode = (Ver1 > Ver2);
		if (nRetCode)
		{
			pkFileInfo->strFileName = strPackName;
			pkFileInfo->strVersion  = strLastVersion;
			pkFileInfo->strFileMd5  = strCurrentMD5;
		}
	}
	else
	{
		pkFileInfo = new KFileInfo;
		KGLOG_PROCESS_ERROR(pkFileInfo);

		pkFileInfo->strFileMd5        = strCurrentMD5;
		pkFileInfo->strFileName       = strPackName;
		pkFileInfo->strVersion        = strLastVersion;
		m_UpdatePath[strPrevVersion] = pkFileInfo;
	}

	nResult = true;
Exit0:
	return nResult;
}	

int KVersionParser::GetPackFileInfo(int nIndex, KZipFileInfo &pKZipFileInfo)
{
	int nResult = false;
	int nLength = 0;
	KZipFileInfo* pkFileInfo = NULL;
	tstring strTempInfo;

	KGLOG_PROCESS_ERROR(nIndex < m_nCountList);
	KGLOG_PROCESS_ERROR(nIndex >= 0);
	
	pkFileInfo = m_pZipFileList[nIndex];

	nLength = sizeof(pKZipFileInfo.szZipFileName) / sizeof(pKZipFileInfo.szZipFileName[0]);
	_tcsncpy(pKZipFileInfo.szZipFileName, pkFileInfo->szZipFileName, nLength);
	pKZipFileInfo.szZipFileName[nLength - 1] = _T('\0');

	nLength = sizeof(pKZipFileInfo.szZipFileMD5) / sizeof(pKZipFileInfo.szZipFileMD5[0]);
	_tcsncpy(pKZipFileInfo.szZipFileMD5, pkFileInfo->szZipFileMD5, nLength);
	pKZipFileInfo.szZipFileMD5[nLength - 1] = _T('\0');

	nResult = true;
Exit0:
	return nResult;
}

void KVersionParser::CleanUp()
{
	map<tstring, PKFileInfo>::iterator itor;

	if (m_UpdatePath.empty())
		goto Exit0;
	
	for (
		itor = m_UpdatePath.begin();
		itor != m_UpdatePath.end();
		++itor
	)
	{
		if (itor->second)
		{
			delete itor->second;
			itor->second = NULL;
		}
	}
	for (int i = 0; i < m_nCountList; i++)
	{
		if (m_pZipFileList[i])
		{
			delete m_pZipFileList[i];
			m_pZipFileList[i] = NULL;
		}
	}
	m_nCountList = 0;
	m_UpdatePath.clear();
Exit0:
	return;
}

int KVersionParser::AnalysisPath(const tstring &strBeginVersion)
{
	int nRetCode = false;
	int nResult  = false;
	int nLength = 0;
	tstring strTempBeginVer;
	tstring strTempRetVer;
	strTempBeginVer = strBeginVersion;

	for (size_t i = 0; i < m_UpdatePath.size(); i++)
	{
		tstring strTmpVersion;
		nRetCode = SearchClosePath(strTempBeginVer, strTempRetVer);
		KGLOG_PROCESS_ERROR(nRetCode);

		KVersionNumber Ver1(strTempBeginVer.c_str());
		strTmpVersion = m_UpdatePath[strTempRetVer]->strVersion;		
		KVersionNumber Ver2(strTmpVersion.c_str());
		if (Ver1 == Ver2)
			break;
		
		m_pZipFileList[i] = new KZipFileInfo;
		KGLOG_PROCESS_ERROR(m_pZipFileList[i]);
		nLength = sizeof(m_pZipFileList[i]->szZipFileName) / sizeof(m_pZipFileList[i]->szZipFileName[0]);
		_tcsncpy(m_pZipFileList[i]->szZipFileName, m_UpdatePath[strTempRetVer]->strFileName.c_str(), nLength);
		m_pZipFileList[i]->szZipFileName[nLength - 1] = _T('\0');

		nLength = sizeof(m_pZipFileList[i]->szZipFileMD5) / sizeof(m_pZipFileList[i]->szZipFileMD5[0]);
		_tcsncpy(m_pZipFileList[i]->szZipFileMD5, m_UpdatePath[strTempRetVer]->strFileMd5.c_str(), nLength);
		m_pZipFileList[i]->szZipFileMD5[nLength - 1] = _T('\0');
		m_nCountList++;
		if (m_nBestPath)
			break;
		strTempBeginVer = m_UpdatePath[strTempRetVer]->strVersion;
	}

	nResult = true;
Exit0:
	if (!nResult)
	{
		for (int i = 0; i < m_nCountList; i++)
		{
			if (m_pZipFileList[i])
			{
				delete m_pZipFileList[i];
				m_pZipFileList[i] = NULL;
			}
		}
		m_nCountList = 0;
	}
	return nResult;
}

int KVersionParser::SearchClosePath(const tstring &strVerion, /*out*/tstring &strRetVersion)
{
	int  nResult  = false;
	bool IsMatch  = false;
	PKFileInfo pkFileInfoe = NULL;
	tstring    strTempVersron;
	map<tstring, PKFileInfo>::iterator itor;
	map<tstring, PKFileInfo>::const_iterator cFindItor;
	KVersionNumber LastVer(m_strLatestVersion.c_str());
	KVersionNumber StartVer(strVerion.c_str());

	if (StartVer == LastVer)
	{
		IsMatch = true;
		m_nBestPath = true;
		goto Exit1;		
	}

	if (m_UpdatePath.empty())
		goto Exit0;

	cFindItor = m_UpdatePath.find(strVerion);
	if (cFindItor != m_UpdatePath.end())
	{
		strTempVersron = strVerion;
	}

	for (
		itor = m_UpdatePath.begin();
		itor != m_UpdatePath.end();
		++itor
	)
	{
		pkFileInfoe = itor->second; 
		KVersionNumber NowVer(itor->first.c_str());						      //当前循环的版本
		KVersionNumber NowVerCanToVer(pkFileInfoe->strVersion.c_str());       //当前循环版本的可到达版本

		if (strTempVersron.empty())
		{
			if (StartVer > NowVer)
			{
				IsMatch = true;
				strTempVersron = itor->first;
				continue;
			}
			else
				continue;
		}
		KVersionNumber CurrentVer(strTempVersron.c_str());                    //待比较的版本
		KVersionNumber CurVerCanToVer(m_UpdatePath[strTempVersron]->strVersion.c_str());

		if (StartVer == NowVer)
		{
			if (NowVerCanToVer == LastVer)
			{
				IsMatch        = true;
				m_nBestPath    = true;
				strTempVersron = itor->first;
				break;
			}
		}

		if (CurrentVer == NowVer)
		{			
			if (NowVerCanToVer == LastVer)
			{
				IsMatch = true;
				m_nBestPath = true;
				break;
			}
			
			if (NowVerCanToVer > CurVerCanToVer || 
				NowVerCanToVer == CurVerCanToVer)
			{
				IsMatch = true;
				continue;
			}
		}

		if (StartVer > NowVer)
		{
			if (NowVer > CurrentVer)
			{
				if (NowVerCanToVer > CurVerCanToVer ||
					NowVerCanToVer == CurVerCanToVer)
				{
					strTempVersron = itor->first;
					IsMatch = true;
					continue;
				}
			}
		}

		if (CurrentVer > NowVer)
		{
			if (NowVerCanToVer == LastVer)
			{
				strTempVersron = itor->first;
				IsMatch = true;
				m_nBestPath = true;
				continue;
			}
			if (NowVerCanToVer > CurVerCanToVer)
			{
				strTempVersron = itor->first;
				IsMatch = true;
				break;
			}
		}		
	}

Exit1:
	if (IsMatch)
	{
		strRetVersion = strTempVersron;
		KVersionNumber RetVer(m_UpdatePath[strTempVersron]->strVersion.c_str());
		if (LastVer == RetVer)
		{
			m_nBestPath = true;
		}
		nResult = true;
 	}
Exit0:
	return nResult;
}
//返回 -1为版本不匹配。 0为最新版本 true符合升级规则
int KVersionParser::CanUpdate(const TCHAR* szPackName, const TCHAR* szCurrentVer)
{
	int nRetCode = false;
	int nResult  = false;

	tstring strPrevVer;
	tstring strLastVer;
	KVersionNumber PrevVer;
	KVersionNumber LastVer;
	KVersionNumber CurrVer(szCurrentVer);
	
	KGLOG_PROCESS_ERROR(szCurrentVer);
	nRetCode = ParsePackName(tstring(szPackName), strPrevVer, strLastVer);
	KGLOG_PROCESS_ERROR(nRetCode);

	PrevVer = strPrevVer.c_str();
	LastVer = strLastVer.c_str(); 

	if (PrevVer > CurrVer || CurrVer > LastVer)
	{
		nResult = -1;
		goto Exit0;
	} 

	if (CurrVer == LastVer)
		goto Exit0;
	
	nResult = true;
Exit0:
	return nResult;
}
