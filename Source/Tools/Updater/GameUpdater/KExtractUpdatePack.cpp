//////////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    :   KExtractUpdatePack.cpp
//  Creater     :   ZhongGuangZe
//  Date        :   2008-06-10
//  Comment     :   MargePack
//
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <io.h>
#include "UpdateDefine.h"
#include "KExtractUpdatePack.h"
#include "updatepublic.h"
#include "UpdateDefine.h"
#include "KLastError.h"

const TCHAR* UNEXTRACT_FILE[] =
{
	GAMEUPDATER_EXE_NAME, //gameupdater.exe
	SWF_FILE_NAME,        //update.swf
	_T("update.dll"),
	PACKAGE_INI_NAME      //package.ini
};

KExtractUpdatePack::KExtractUpdatePack()
{
	m_nZipTotalFiles   = 0;
	m_nZipItemIndex	   = 0;
	m_nVersionCfgIndex = -1;
	m_pZip             = NULL;
	m_nPackageCount    = 0;
	m_uPackSize        = (unsigned)1024 * 1024 * 1024 * 2;

	memset(m_pszPackageList, 0, sizeof(m_pszPackageList));
}

int KExtractUpdatePack::Init(const TCHAR szPackFileName[], const TCHAR szExtractPath[])
{
	int      nRetCode = false;
	int      nResult  = false;
	ZIPENTRY ze;

	KGLOG_PROCESS_ERROR(szPackFileName);
	KGLOG_PROCESS_ERROR(szExtractPath);

	_tcsncpy(m_szPackFileName, szPackFileName, MAX_PATH);
	_tcsncpy(m_szExtractPath, szExtractPath, MAX_PATH);

	m_pZip = OpenZip(m_szPackFileName, NULL);
	KGLOG_PROCESS_ERROR(m_pZip);

	nRetCode = GetZipItem(m_pZip, -1, &ze);
	KGLOG_PROCESS_ERROR(nRetCode == ZR_OK);

	m_nZipTotalFiles = ze.index;

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (m_pZip)
		{
			CloseZip(m_pZip);
			m_pZip = NULL;
		}
	}

	return nResult;
}

void KExtractUpdatePack::UnInit()
{
	if(m_pZip)
	{
		CloseZip(m_pZip);
		m_pZip = NULL;
	}

	for (int j = 0; j < m_nPackageCount; j++)
	{
		if (m_pszPackageList[j])
		{
			delete[] m_pszPackageList[j];
			m_pszPackageList[j] = NULL;
		}
	}
	m_nZipItemIndex    = 0;
	m_nZipTotalFiles   = 0;
	m_nPackageCount    = 0;
	m_nVersionCfgIndex = -1;
}

EXTRACT_PACK_RESULT KExtractUpdatePack::ExtractNext()
{
	int                  nRetCode           = false;
	EXTRACT_PACK_RESULT  eResult            = EXTRACT_FAILD;
	TCHAR                szDirectoryName[6] = "X:\\";
	ULARGE_INTEGER       FreeDiskSize       = {0};
	ZIPENTRY             ZipEntry;
    tstring strItemName;
    tstring strTargetFileName;

	KGLOG_PROCESS_ERROR(m_pZip);
	KGLOG_PROCESS_ERROR(m_nZipItemIndex >= 0);

	szDirectoryName[0] = m_szExtractPath[0];

    //如果当前索引已经超过更新包内文件的大小，那么就返回解压完成。
    if (m_nZipItemIndex > m_nZipTotalFiles)
        return  EXTRACT_FINISH;

	if (m_nZipItemIndex == m_nZipTotalFiles)
	{   //最后解压版本文件 version.cfg
        KGLOG_PROCESS_ERROR(m_nVersionCfgIndex >= 0 && "version.cfg not found.");

        strTargetFileName = tstring(m_szExtractPath) + _T("version.cfg");
        nRetCode = _ExtractFileToPathFile(m_nVersionCfgIndex, strTargetFileName.c_str());
        KGLOG_PROCESS_ERROR(nRetCode && "Failed to extract version.cfg.");

        KG_PROCESS_SUCCESS(true);
	}

	//取得当前解压文件的信息
	nRetCode = GetZipItem(m_pZip, m_nZipItemIndex, &ZipEntry);
	KGLOG_PROCESS_ERROR(nRetCode == ZR_OK);

    strItemName = ZipEntry.name;
    nRetCode = _tcsnicmp(strItemName.c_str(), _T("updatepack/"), _tcslen(_T("updatepack/")));
    KG_PROCESS_SUCCESS(nRetCode);   // 包外的文件不被解压
    
    strItemName.erase(0, _tcslen(_T("updatepack/")));
    nRetCode = strItemName.empty();
    KG_PROCESS_SUCCESS(nRetCode);   // updatepak这层目录 跳过。

    //如果是目录就跳过，解压文件时来创建目录
    nRetCode = ZipEntry.attr & FILE_ATTRIBUTE_DIRECTORY;
    KG_PROCESS_SUCCESS(nRetCode);

    //如果当前欲解压的文件为version.cfg，那么把这个文件放到最后去解压
    nRetCode = _tcsicmp(strItemName.c_str(), _T("version.cfg"));
    if (nRetCode == 0)
    {
        KGLOG_PROCESS_ERROR(m_nVersionCfgIndex < 0 && "There is not only one version.cfg.");
        m_nVersionCfgIndex = m_nZipItemIndex;
        KG_PROCESS_SUCCESS(true);
    }

    nRetCode = _ExceptFile(strItemName.c_str()); //看看是否为需要排除的文件
    KG_PROCESS_SUCCESS(nRetCode);

	//检查磁盘空间
	nRetCode = GetDiskFreeSpaceEx(szDirectoryName, NULL, NULL, &FreeDiskSize);
	KGLOG_PROCESS_ERROR(nRetCode != S_OK);

	if (ZipEntry.unc_size >= FreeDiskSize.QuadPart)
	{
		KLastError::SetErrorCode(UPDATE_ERR_NO_ENOUGH_DISK_SPACE);
		KGLOG_PROCESS_ERROR(false && "disk is full!");
	}

	//设置当前解压的文件名字
	_tcsncpy(m_szCurrentFile, strItemName.c_str(), sizeof(m_szCurrentFile) / sizeof(m_szCurrentFile[0]));
	m_szCurrentFile[sizeof(m_szCurrentFile) / sizeof(m_szCurrentFile[0]) -1 ] = _T('\0');

	//区分欲解压的文件是以PAK为后缀名的还是普通文件
	nRetCode = _IsPackFileSuffix(strItemName.c_str());
	if (!nRetCode)
	{//普通文件如DLL,EXE等文件
        strTargetFileName = tstring(m_szExtractPath) + strItemName;
		nRetCode = _ExtractFileToPathFile(m_nZipItemIndex, strTargetFileName.c_str());
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	else
	{//以PAK为后缀名的文件
		nRetCode = _DisposePackFile(m_nZipItemIndex);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

Exit1:
	++m_nZipItemIndex;
	eResult = EXTRACT_OK;
Exit0:
	return eResult;
}

int KExtractUpdatePack::ExtractSingleFile(const TCHAR szZipName[], const TCHAR szFileName[], const TCHAR szOutPath[])
{
	int      nResult  = false;
	int      nRetCode = false;
	int      nIndex   = 0;
	ZIPENTRY ze;

	KGLOG_PROCESS_ERROR(szZipName);
	KGLOG_PROCESS_ERROR(szFileName);
	KGLOG_PROCESS_ERROR(szOutPath);

	m_pZip = OpenZip(szZipName, NULL);
	KGLOG_PROCESS_ERROR(m_pZip);

	nRetCode = FindZipItem(m_pZip, szFileName, false, &nIndex, &ze);
	KGLOG_PROCESS_ERROR(-1 != nIndex);

	nRetCode = UnzipItem(m_pZip, nIndex, szOutPath);
	KGLOG_PROCESS_ERROR(ZR_OK == nRetCode);

	nResult = true;
Exit0:
	if (m_pZip)
	{
		CloseZip(m_pZip);
		m_pZip = NULL;
	}

	return nResult;
}

int KExtractUpdatePack::_ExtractFileToPathFile(int nItemIndex, const TCHAR cszFullFileName[])
{
	int nRetCode = false;
	int nResult  = false;
	ZIPENTRY ZipEntry;

	KGLOG_PROCESS_ERROR(nItemIndex >= 0);
	KGLOG_PROCESS_ERROR(nItemIndex < m_nZipTotalFiles);
    ASSERT(cszFullFileName);
    ASSERT(cszFullFileName[0]);

    nRetCode = GetZipItem(m_pZip, nItemIndex, &ZipEntry);
    KGLOG_PROCESS_ERROR(nRetCode == ZR_OK && "GetZipItem()");
    
    if (ZipEntry.unc_size == 0)
    {   // The UnzipItem() has the BUG, it return error if zip item is empty file.
        nRetCode = _CreateEmptyFile(cszFullFileName);
        KGLOG_PROCESS_ERROR(nRetCode && "_CreateEmptyFile()");
    }
    else
    {
        nRetCode = UnzipItem(m_pZip, nItemIndex, cszFullFileName);
        KGLOG_PROCESS_ERROR(nRetCode == ZR_OK && "UnzipItem()");
    }

Exit1:
	nResult = true;
Exit0:    
    if (!nResult)
        KGLogPrintf(KGLOG_DEBUG, "Fail to extract normal item : %s.", cszFullFileName);
	return nResult;
}

int KExtractUpdatePack::_DisposePackFile(int nItemIndex)
{
	int      nRetCode                  = false;
	int      nResult                   = false;
	int      nPathLen                  = 0;
	ZIPENTRY ZipEntry;
	TCHAR    szDesPackFile[MAX_PATH];
	TCHAR    szSrcPackFile[MAX_PATH];

	KGLOG_PROCESS_ERROR(nItemIndex >= 0);
	KGLOG_PROCESS_ERROR(nItemIndex <= m_nZipTotalFiles);

	//读取package.ini内的PAK列表
	nRetCode = _ReadPackageini();
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = GetZipItem(m_pZip, nItemIndex, &ZipEntry);
	KGLOG_PROCESS_ERROR(nRetCode == ZR_OK);

	nRetCode = _GetFileNameLoaction(ZipEntry.name, &nPathLen);
	KGLOG_PROCESS_ERROR(nRetCode);

	_sntprintf(szSrcPackFile, MAX_PATH, "%s%s\\%s", 
		m_szExtractPath, EXTRACT_PATH, ZipEntry.name + nPathLen);
	
	//构造pak/update_N.pak
	_sntprintf(szDesPackFile, MAX_PATH, "%s\\%s", m_szPakPath, m_pszPackageList[0]);

	nRetCode = _FileIsExist(szDesPackFile);  //判断package.ini列表中第0个是否存在
	if (nRetCode == 0)
	{//不存在那么直接从zip中解压，更新结束
		nRetCode = UnzipItem(m_pZip, nItemIndex, szDesPackFile);
		KGLOG_PROCESS_ERROR(nRetCode == ZR_OK);
		goto Exit1;
	}
	nRetCode = UnzipItem(m_pZip, nItemIndex, szSrcPackFile);
	KGLOG_PROCESS_ERROR(nRetCode == ZR_OK);

	nRetCode = _CheckPack(szDesPackFile, szSrcPackFile);	
	if (nRetCode == -1)
	{
		_tremove(szSrcPackFile);
		KGLOG_PROCESS_ERROR(false && "No disk space!");
	} 
	else if (!nRetCode)
	{
		nRetCode = _AddUpdatePack(); //增加一个update pak
		KGLOG_PROCESS_ERROR(nRetCode);

		_sntprintf(szDesPackFile, MAX_PATH, "%s\\%s", m_szPakPath, m_pszPackageList[0]);
		rename(szSrcPackFile, szDesPackFile);

		nRetCode = _SavePackageList();
		KGLOG_PROCESS_ERROR(nRetCode);

		goto Exit1;
	}

	//开始合并PAK包
	nRetCode = _MargePackFile(szSrcPackFile, szDesPackFile);
	_tremove(szSrcPackFile);
	KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KExtractUpdatePack::_IsPackFileSuffix(const TCHAR szFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    unsigned uFileNameLength = 0;
    const unsigned SUFFIX_LENGTH = sizeof(PACK_SUFFIX) / sizeof(PACK_SUFFIX[0]) - 1;

    ASSERT(szFileName);
    ASSERT(szFileName[0]);

    // Check the suffix of file name is ".pak"
    uFileNameLength = (unsigned)_tcslen(szFileName);
    KG_PROCESS_ERROR(uFileNameLength > SUFFIX_LENGTH);

    nRetCode = _tcsicmp(szFileName + uFileNameLength - SUFFIX_LENGTH, PACK_SUFFIX);
    KG_PROCESS_ERROR(nRetCode == 0);

    nResult = true;
Exit0:
    return nResult;
}

int KExtractUpdatePack::_ReadPackageini()
{
	int   nRetCode                = false;
	int   nResult                 = false;
	int   nIndex                  = 0;
	TCHAR temp[10]                = {0};
	TCHAR pszCurrPakName[MAX_PATH];
	TCHAR szPackIniPath[MAX_PATH];

	_sntprintf(szPackIniPath, MAX_PATH, "%s%s", m_szExtractPath, PACKAGE_INI_NAME);	

	//得到pak包所在的目录
	nRetCode = GetPrivateProfileString("SO3Client", SECTION_PACK_PATH, "", m_szPakPath, MAX_PATH, szPackIniPath);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	//获取pak的列表
	while (true)
	{
		int nStrLen = 0;

		_itot(nIndex, temp, 10);
		nRetCode = GetPrivateProfileString("SO3Client", temp, "", pszCurrPakName, MAX_PATH, szPackIniPath);
		if (nRetCode == 0)
			break;
		
		nStrLen = (int)_tcslen(pszCurrPakName);

		if (!m_pszPackageList[nIndex])
		{
			m_pszPackageList[nIndex] = new TCHAR[nStrLen + 1];
			KGLOG_PROCESS_ERROR(m_pszPackageList[nIndex]);	
		}

		_tcsncpy(m_pszPackageList[nIndex], pszCurrPakName, nStrLen);
		m_pszPackageList[nIndex][nStrLen] = '\0';

		nIndex++;
	}	
	
	if (nIndex <= 0)
	{
		m_nPackageCount = 0;
		goto Exit0;
	}

	m_nPackageCount = nIndex;

	nResult = true;
Exit0:
	if (!nResult)
	{
		for (int j = 0; j < nIndex; j++)
		{
			if (m_pszPackageList[j])
			{
				delete[] m_pszPackageList[j];
				m_pszPackageList[j] = NULL;
			}
		}
	}
	return nResult;
	
}

int KExtractUpdatePack::_SavePackageList()
{
	int   nRetCode                  = false;
	int   nResult                   = false;
	TCHAR temp[10]                  = {0};
	TCHAR szPackIniPath[MAX_PATH];

	KGLOG_PROCESS_ERROR(m_nPackageCount > 0);

	_sntprintf(szPackIniPath, MAX_PATH, "%spackage.ini", m_szExtractPath);	

	for (int nIndex = 0; nIndex < m_nPackageCount; ++nIndex)
	{
		_itot(nIndex, temp, 10);
		nRetCode = WritePrivateProfileString("SO3Client", temp, m_pszPackageList[nIndex], szPackIniPath);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KExtractUpdatePack::_AddUpdatePack()
{
	int     nResult                      = false;
	TCHAR*  pszNum                       = NULL;
	TCHAR*  pszNewUpdatePack             = NULL;
	int	    nStrLen                      = 0;
	TCHAR*	pszTempList[PAK_LIST_LMIT];

	memset(pszTempList, 0, PAK_LIST_LMIT * sizeof(TCHAR*));

	KGLOG_PROCESS_ERROR(m_nPackageCount > 0);
	KGLOG_PROCESS_ERROR((m_nPackageCount + 1) < PAK_LIST_LMIT);
	
	nStrLen = (int)_tcslen(m_pszPackageList[0]);
	pszNewUpdatePack = new TCHAR[nStrLen + 1];
	KGLOG_PROCESS_ERROR(pszNewUpdatePack);

	memcpy(pszTempList, m_pszPackageList, m_nPackageCount * sizeof(TCHAR*));

	_tcsncpy(pszNewUpdatePack, m_pszPackageList[0], nStrLen);
	pszNewUpdatePack[nStrLen] = '\0';

	pszNum = _tcsstr(pszNewUpdatePack, ".");
	KGLOG_PROCESS_ERROR(pszNum);
	*(pszNum - 1) = ((*(pszNum - 1) - '0') + 1) + '0';

	m_pszPackageList[0] = pszNewUpdatePack;
	memcpy(m_pszPackageList + 1, pszTempList, m_nPackageCount * sizeof(TCHAR*));
	m_nPackageCount++;

	memset(pszTempList, 0, PAK_LIST_LMIT * sizeof(TCHAR*));
	nResult = true;
Exit0:
	if (!nResult)
	{
		delete[] pszNewUpdatePack;
		pszNewUpdatePack = NULL;
	}
	return nResult;
}

int KExtractUpdatePack::_CheckPack(const TCHAR szDesPack[], const TCHAR szAddPack[])
{
	int              nRetCode           = false;
	int              nResult            = false;
	FILE*            stdFile            = NULL;
	FILE*            addFile            = NULL;
	DWORD            dwDesFileSize      = 0;
	DWORD            dwAddFileSize      = 0;
	ULARGE_INTEGER   FreeDiskSize       = {0};
	TCHAR            szDirectoryName[6] = "X:\\";
	t_pack_header    stdHeader;
	t_pack_header    addHeader;

	KGLOG_PROCESS_ERROR(szDesPack);
	KGLOG_PROCESS_ERROR(szAddPack);

	//2个要合并的PAK体积加起来必须不能大于1.8G
	nRetCode = _GetFileSize(szDesPack, &dwDesFileSize);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _GetFileSize(szAddPack, &dwAddFileSize);
	KGLOG_PROCESS_ERROR(nRetCode);

	KGLOG_PROCESS_ERROR((dwDesFileSize + dwAddFileSize) < (m_uPackSize * 0.9));

	//2个要合并的PAK必须有足够的空间，如果空间不够，那么返回 -1
	szDirectoryName[0] = szAddPack[0];
	nRetCode = GetDiskFreeSpaceEx(szDirectoryName, NULL, NULL, &FreeDiskSize);
	KGLOG_PROCESS_ERROR(nRetCode != S_OK);
	
	if (dwAddFileSize >= FreeDiskSize.QuadPart)
	{
		nResult = -1;
		KLastError::SetErrorCode(UPDATE_ERR_NO_ENOUGH_DISK_SPACE);
		goto Exit0;
	}

	//2个要合并的PAK包内文件个数必须不能超过20W
	stdFile = _tfopen(szDesPack, "rb");
	KGLOG_PROCESS_ERROR(stdFile);

	addFile = _tfopen(szAddPack, "rb");
	KGLOG_PROCESS_ERROR(addFile);
	
	GetHeader(&stdHeader, stdFile);
	GetHeader(&addHeader, addFile);

	//检查目标文件文件是否为PAK文件，如果不是，那么就生成一个update_N+1.pak
	nRetCode = _tcsnicmp(PACK_FILE_HEADER, (char*)stdHeader.signature, strlen("PACK"));
	KGLOG_PROCESS_ERROR(nRetCode == 0 && "target not pak file~");

	KGLOG_PROCESS_ERROR(stdHeader.count < MAX_FILE);
	KGLOG_PROCESS_ERROR((stdHeader.count + addHeader.count) < MAX_FILE);

	nResult = true;
Exit0:
	if (stdFile)
	{
		fclose(stdFile);
		stdFile = NULL;
	}

	if (addFile)
	{
		fclose(addFile);
		addFile = NULL;
	}

	return nResult;
}

int KExtractUpdatePack::_ExceptFile(const TCHAR szFileName[])
{
    int nRetCode = false;
    tstring strFileName;

    ASSERT(szFileName);
    ASSERT(szFileName[0]);

    strFileName = szFileName;
    nRetCode = (int)strFileName.find_last_of(_T('/'));
    if (nRetCode != tstring::npos)
        strFileName.erase(0, nRetCode + 1);

    for (int i = 0; i < sizeof(UNEXTRACT_FILE) / sizeof(UNEXTRACT_FILE[0]); ++i)
	{
        nRetCode = _tcsicmp(strFileName.c_str(), UNEXTRACT_FILE[i]);
        if (nRetCode == 0)
            return true;
	}
    return false;
}

int KExtractUpdatePack::_MargePackFile(const TCHAR szSrcPack[], const TCHAR szDesPack[])
{
	int   nRetCode        = false;
	int   nResult         = false;
	DWORD dwFileAttribute = 0;

	KGLOG_PROCESS_ERROR(szSrcPack);
	KGLOG_PROCESS_ERROR(szDesPack);	

	dwFileAttribute = GetFileAttributes(szSrcPack);
	dwFileAttribute &= ~dwFileAttribute;
	SetFileAttributes(szSrcPack, dwFileAttribute);
	dwFileAttribute = GetFileAttributes(szDesPack);
	dwFileAttribute &= ~dwFileAttribute;
	SetFileAttributes(szDesPack, dwFileAttribute);

	nRetCode = PackComb(szDesPack, szSrcPack);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}


int KExtractUpdatePack::_GetFileNameLoaction(const TCHAR szFile[], int* nLocation)
{
	int nResult    = false;
	int nStrLength = 0;

	KGLOG_PROCESS_ERROR(szFile);

	nStrLength = (int)strlen(szFile);

	for (; nStrLength > 0; --nStrLength)
	{
		if (
			szFile[nStrLength] == '/' ||
			szFile[nStrLength] == '\\'
			)
		{
			*nLocation = nStrLength + 1;
			goto Exit1;
		}
	}

	nLocation = 0;

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KExtractUpdatePack::_FileIsExist(const TCHAR szFilePath[])
{
	int nRetCode = false;
	int nResult  = false;

	KGLOG_PROCESS_ERROR(szFilePath);

	nRetCode = _taccess(szFilePath, 0);

	if (-1 == nRetCode)
	{
		if (errno == ENOENT)
			goto Exit0;
	}

	nResult = true;
Exit0:
	return nResult;
}

int KExtractUpdatePack::_GetFileSize(const TCHAR szFileName[], DWORD* dwRetSize)
{
	WIN32_FIND_DATA   FindFileData;
	int               nResult      = false;
	HANDLE            hFile        = INVALID_HANDLE_VALUE;

	KGLOG_PROCESS_ERROR(szFileName);

	hFile = ::FindFirstFile(szFileName, &FindFileData);  
	KGLOG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);
	*dwRetSize = FindFileData.nFileSizeLow;

	nResult = true;
Exit0:
	if (hFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFile);
		hFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}

int KExtractUpdatePack::_CreateEmptyFile(const TCHAR cszFullFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    tstring strParentDirectory;
    FILE *fpFile = NULL;

    ASSERT(cszFullFileName);
    ASSERT(cszFullFileName[0]);

    nRetCode = _GetParentDirectory(cszFullFileName, &strParentDirectory);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = _EnsureDirectory(strParentDirectory.c_str());
    KGLOG_PROCESS_ERROR(nRetCode);

    fpFile = _tfopen(cszFullFileName, _T("wb"));
    KGLOG_PROCESS_ERROR(fpFile);        

    nResult = true;
Exit0:
    if (!nResult)
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to create empty file `%s`, Error code : %d.",
            cszFullFileName, GetLastError()
        );
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    return nResult;
}

int KExtractUpdatePack::_GetParentDirectory(const TCHAR cszFullName[], tstring *pstrRetParentDirectory)
{
    int nResult  = false;
    int nRetCode = false;
    tstring strFullName;
    int nLastSlashIndex = -1;

    ASSERT(cszFullName);
    ASSERT(cszFullName[0]);
    ASSERT(pstrRetParentDirectory);

    strFullName = cszFullName;
    for (int i = (int)strFullName.length(); i >= 0 ; i--)
    {
        if ((strFullName[i] == '/') || (strFullName[i] == '\\'))
        {
            nLastSlashIndex = i;
            break;
        }
    }    
    KG_PROCESS_ERROR(nLastSlashIndex >= 0);

    *pstrRetParentDirectory = strFullName.substr(0, nLastSlashIndex);

    nResult = true;
Exit0:
    return nResult;
}

int KExtractUpdatePack::_EnsureDirectory(const TCHAR cszDirectoryName[])
{
    int nResult  = false;
    int nRetCode = false;
    tstring strParentDirectory;
    int nLastSlashIndex = -1;

    ASSERT(cszDirectoryName);
    ASSERT(cszDirectoryName[0]);

    nRetCode = _GetParentDirectory(cszDirectoryName, &strParentDirectory);
    if (nRetCode)
    {
        nRetCode = _EnsureDirectory(strParentDirectory.c_str());
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = CreateDirectory(cszDirectoryName, NULL);
    if (nRetCode == 0)
    {
        nRetCode = GetLastError();
        KGLOG_PROCESS_ERROR(nRetCode == ERROR_ALREADY_EXISTS);
    }

    nResult = true;
Exit0:
    return nResult;
}