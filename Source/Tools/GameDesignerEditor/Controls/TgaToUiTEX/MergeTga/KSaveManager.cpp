////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSaveManager.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-23 10:08:01
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KSaveManager.h"
#include "UiTex.h"
#include "KAnimateManager.h"
#include "KIconManager.h"
#include "KTgaManager.h"

////////////////////////////////////////////////////////////////////////////////

KSaveManager *KSaveManager::ms_pSelf = NULL;

KSaveManager::KSaveManager()
{
    m_szUITexFile[0] = _T('\0');
    m_szTgaFile[0] = _T('\0');
    m_szSTexFile[0] = _T('\0');
    m_szLastOpenOrSaveDir[0] = _T('\0');
    m_szLastUITexOpenOrSaveDir[0] = _T('\0');
    m_szLastCurrentPath[0] = _T('\0');
    m_nSaveHideCode = false;
}

KSaveManager::~KSaveManager()
{
}

int KSaveManager::Init()
{
    int nResult = false;

    ASSERT(!ms_pSelf);
    ms_pSelf = new KSaveManager;
    KG_PROCESS_ERROR(ms_pSelf);
    ms_pSelf->InitLastOpenOrSaveDir();

    nResult = true;
Exit0:
    return nResult;
}

int KSaveManager::Exit()
{
    KG_DELETE(ms_pSelf);
    return true;
}

KSaveManager &KSaveManager::GetSelf()
{
    ASSERT(ms_pSelf);
    return *ms_pSelf;
}

int KSaveManager::SetCurrentEditFile(LPCTSTR pcszUITex, LPCTSTR pcszTga /* = NULL */, LPCTSTR pcszSTex /* = NULL */, LPCTSTR pcszTxt/* = NULL */)
{
    int nResult = false;

    KG_PROCESS_ERROR(pcszUITex);

    _tcsncpy(m_szUITexFile, pcszUITex, sizeof(m_szUITexFile) / sizeof(TCHAR));
    m_szUITexFile[sizeof(m_szUITexFile) / sizeof(TCHAR) - 1] = _T('\0');

    if (!pcszTga)
        pcszTga = "";
    _tcsncpy(m_szTgaFile, pcszTga, sizeof(m_szTgaFile) / sizeof(TCHAR));
    m_szTgaFile[sizeof(m_szTgaFile) / sizeof(TCHAR) - 1] = _T('\0');

    if (!pcszSTex)
        pcszSTex = "";
    _tcsncpy(m_szSTexFile, pcszSTex, sizeof(m_szSTexFile) / sizeof(TCHAR));
    m_szSTexFile[sizeof(m_szSTexFile) / sizeof(TCHAR) - 1] = _T('\0');

	if (!pcszTxt)
		pcszTxt = "";
	_tcsncpy(m_szTxtFile, pcszTxt, sizeof(m_szTxtFile) / sizeof(TCHAR));
	m_szTxtFile[sizeof(m_szTxtFile) / sizeof(TCHAR) - 1] = _T('\0');

    nResult = true;
Exit0:
    return nResult;
}

int KSaveManager::GetCurrentEditFile(LPTSTR pszName, int nNameBufferSize)
{
    int nResult = false;

    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(nNameBufferSize > 0);

    _tcsncpy(pszName, m_szUITexFile, nNameBufferSize);
    pszName[nNameBufferSize - 1] = _T('\0');

    nResult = true;
Exit0:
    return nResult;
}

int KSaveManager::Clear()
{
	m_szTxtFile[0] = _T('\0');
    m_szTgaFile[0] = _T('\0');
    m_szUITexFile[0] = _T('\0');
    m_szSTexFile[0] = _T('\0');

    return true;
}

int KSaveManager::Save()
{
    int nResult = false;
    int nRetCode = false;
    size_t uWrite = 0;
    FILE *fpFile = NULL;
    LPTSTR pszClip = NULL;
    LPTSTR pszTemp = NULL;
    UITEXFILEHEADER UiTexHeader;
    TCHAR szPureTgaFileName[MAX_PATH];
    TCHAR szAlphaName[MAX_PATH];
    TCHAR szAlphaCodeName[MAX_PATH];

    KG_PROCESS_ERROR(m_szUITexFile[0] != _T('\0'));
    
    nRetCode = AutoSetTgaFileAndSTexFile();
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = KTgaManager::GetSelf().FormateSaveData();
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = KAnimateManager::GetSelf().FormateSaveData();
    KG_PROCESS_ERROR(nRetCode);

	fpFile = fopen(m_szTxtFile ,"w");
	KG_PROCESS_ERROR(fpFile);
	KTgaManager::GetSelf().ExportNewText(fpFile);
	fclose(fpFile);
	fpFile = NULL;

    FormateUITexHeader(UiTexHeader);

    fpFile = fopen(m_szUITexFile ,"wb");
    KG_PROCESS_ERROR(fpFile);
    uWrite = fwrite(&UiTexHeader, sizeof(UiTexHeader), 1, fpFile);
    KG_PROCESS_ERROR(uWrite == 1);
    nRetCode = KTgaManager::GetSelf().Save(fpFile);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = KAnimateManager::GetSelf().Save(fpFile);
    KG_PROCESS_ERROR(nRetCode);
    fclose(fpFile);
    fpFile = NULL;

#ifdef ENABLE_STEX
    fpFile = fopen(m_szSTexFile, "w");
    KG_PROCESS_ERROR(fpFile);
    fprintf(fpFile, "%d\n", 1);
    fprintf(fpFile, "%d\n", 2000);
    GetTgaPureFileName(szPureTgaFileName, sizeof(szPureTgaFileName) / sizeof(TCHAR));
    fprintf(fpFile, "%s", szPureTgaFileName);
    fclose(fpFile);
    fpFile = NULL;
#endif

    nRetCode = KTgaManager::GetSelf().GetSaveTga().SaveDataBufferToTgaFile(m_szTgaFile);
    KG_PROCESS_ERROR(nRetCode);

    KG_PROCESS_SUCCESS(!m_nSaveHideCode);

    _tcsncpy(szPureTgaFileName, m_szTgaFile, sizeof(szPureTgaFileName) / sizeof(TCHAR));
    pszClip = _tcsrchr(szPureTgaFileName, _T('\\'));
    if (pszClip)
        ++pszClip;
    else
        pszClip = szPureTgaFileName;
    pszTemp = _tcsrchr(pszClip, _T('/'));
    if (pszTemp)
        pszClip = pszTemp + 1;
    pszTemp = _tcsrchr(pszClip, _T('.'));
    if (pszTemp)
        *pszTemp = _T('\0');
    _sntprintf(szAlphaName, sizeof(szAlphaName) / sizeof(TCHAR), _T("%s%s"), szPureTgaFileName, _T(".UIAlpha"));
    _sntprintf(szAlphaCodeName, sizeof(szAlphaCodeName) / sizeof(TCHAR), _T("%s%s"), szPureTgaFileName, _T(".UIAlphaC"));
    nRetCode = KTgaManager::GetSelf().GetSaveTga().ProduceAlphaCodeToFile(szAlphaName, szAlphaCodeName);

Exit1:
    nResult = true;
Exit0:
    //清空保存时产生的数据，以节省内存。
    KTgaManager::GetSelf().ClearSaveData();
    KAnimateManager::GetSelf().ClearSaveData();
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    return nResult;
}
int KSaveManager::AutoSetTgaFileAndSTexFile()
{
    int nResult = false;
    LPTSTR pszClip = NULL;

	
    if (m_szUITexFile[0] == _T('\0'))
    {
        m_szSTexFile[0] = _T('\0');
        m_szTgaFile[0] = _T('\0');
		m_szTxtFile[0] = _T('\0');
        KG_PROCESS_SUCCESS(true);
    }

	if (m_szTxtFile[0] == _T('\0'))
	{
		_tcsncpy(m_szTxtFile, m_szUITexFile, sizeof(m_szTxtFile) / sizeof(TCHAR));
		m_szTgaFile[sizeof(m_szTxtFile) / sizeof(TCHAR) - 1] = _T('\0');
		pszClip = _tcsrchr(m_szTxtFile, _T('.'));
		KG_PROCESS_ERROR(pszClip);
		_tcsncpy(pszClip, ".txt", sizeof(m_szTxtFile) / sizeof(TCHAR) - (pszClip - m_szTxtFile));
		m_szTxtFile[sizeof(m_szTxtFile) / sizeof(TCHAR) - 1] = _T('\0');
	}

    if (m_szTgaFile[0] == _T('\0'))
    {
        _tcsncpy(m_szTgaFile, m_szUITexFile, sizeof(m_szTgaFile) / sizeof(TCHAR));
        m_szTgaFile[sizeof(m_szTgaFile) / sizeof(TCHAR) - 1] = _T('\0');
        pszClip = _tcsrchr(m_szTgaFile, _T('.'));
        KG_PROCESS_ERROR(pszClip);
        _tcsncpy(pszClip, ".Tga", sizeof(m_szTgaFile) / sizeof(TCHAR) - (pszClip - m_szTgaFile));
        m_szTgaFile[sizeof(m_szTgaFile) / sizeof(TCHAR) - 1] = _T('\0');
    }

    if (m_szSTexFile[0] == _T('\0'))
    {
        _tcsncpy(m_szSTexFile, m_szUITexFile, sizeof(m_szSTexFile) / sizeof(TCHAR));
        m_szSTexFile[sizeof(m_szSTexFile) / sizeof(TCHAR) - 1] = _T('\0');
        pszClip = _tcsrchr(m_szSTexFile, _T('.'));
        KG_PROCESS_ERROR(pszClip);
        _tcsncpy(pszClip, ".STex", sizeof(m_szSTexFile) / sizeof(TCHAR) - (pszClip - m_szSTexFile));
        m_szSTexFile[sizeof(m_szSTexFile) / sizeof(TCHAR) - 1] = _T('\0');
    }


Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KSaveManager::GetTgaPureFileName(LPTSTR pszName, int nNameBufferSize)
{
    int nResult = false;
    LPCTSTR pcszClip = NULL;
    LPCTSTR pcszTemp = NULL;
    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(nNameBufferSize > 0);

    pcszClip = _tcsrchr(m_szTgaFile, _T('\\'));
    if (!pcszClip)
        pcszClip = m_szTgaFile;
    else
        ++pcszClip;
    pcszTemp = _tcsrchr(pcszClip, _T('/'));
    if (pcszTemp)
        pcszClip = pcszTemp + 1;

    _tcsncpy(pszName, pcszClip, nNameBufferSize);
    pszName[nNameBufferSize - 1] = _T('\0');

    nResult = true;
Exit0:
    return nResult;
}

int KSaveManager::GetSTexPureFileName(LPTSTR pszName, int nNameBufferSize)
{
    int nResult = false;
    LPCTSTR pcszClip = NULL;
    LPCTSTR pcszTemp = NULL;
    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(nNameBufferSize > 0);

    pcszClip = _tcsrchr(m_szSTexFile, _T('\\'));
    if (!pcszClip)
        pcszClip = m_szSTexFile;
    else
        ++pcszClip;
    pcszTemp = _tcsrchr(pcszClip, _T('/'));
    if (pcszTemp)
        pcszClip = pcszTemp + 1;

    _tcsncpy(pszName, pcszClip, nNameBufferSize);
    pszName[nNameBufferSize - 1] = _T('\0');

    nResult = true;
Exit0:
    return nResult;
}

int KSaveManager::IsSaveFileNameEmpty()
{
    return m_szUITexFile[0] == _T('\0');
}

int KSaveManager::FormateUITexHeader(UITEXFILEHEADER &UiTexHeader)
{
    int nResult = false;
    int nRetCode = false;

    UiTexHeader.dwUiTexFlag = 18773;
    UiTexHeader.nFrameCount = KTgaManager::GetSelf().GetSaveBlockCount();
    UiTexHeader.nAnimateCount = KAnimateManager::GetSelf().GetSaveAnimateCount();
    UiTexHeader.nWidth = KTgaManager::GetSelf().GetSaveBufferWidth();
    UiTexHeader.nHeight = KTgaManager::GetSelf().GetSaveBufferHeight();
    GetCorrectWidthAndHeight(UiTexHeader.nWidth, UiTexHeader.nHeight);

    UiTexHeader.dwFileLen = sizeof(UITEXFILEHEADER);
    UiTexHeader.dwFileLen += UiTexHeader.nFrameCount * sizeof(UITEXFRAMEDATASTRUCTURE);
    UiTexHeader.dwFileLen += KAnimateManager::GetSelf().GetSaveAnimateBufferSize();

#ifdef ENABLE_STEX
	nRetCode = GetSTexPureFileName(
		UiTexHeader.szSTexFileName, 
		sizeof(UiTexHeader.szSTexFileName) + sizeof(UiTexHeader.szBuffer)
		);
#else
	nRetCode = GetTgaPureFileName(
		UiTexHeader.szTgaFileName, 
		sizeof(UiTexHeader.szTgaFileName) + sizeof(UiTexHeader.szBuffer)
		);
#endif
    KG_PROCESS_ERROR(nRetCode);   

    nResult = true;
Exit0:
    return true;
}

int KSaveManager::GetCorrectWidthAndHeight(IN OUT int &nWidth, IN OUT int &nHeight)
{
	//如果要补齐2的n次方，去掉注释
/*
    for (int i = 2; i < 10; ++i)
    {
        if (nWidth <= (2 << i))
        {
            nWidth = (2 << i);
            break;
        }
    }

    for (int i = 2; i < 10; ++i)
    {
        if (nHeight <= (2 << i))
        {
            nHeight = (2 << i);
            break;
        }
    }

    if (nWidth < nHeight)
    {
        while (nWidth * 2 < nHeight)
            nWidth *= 2;
    }
    else
    {
        while (nHeight * 2 < nWidth)
            nHeight *= 2;
    }
*/
	//补齐4的整数倍
	if (nWidth % 4)
		nWidth = nWidth - nWidth % 4 + 4;
	if (nHeight % 4)
		nHeight = nHeight - nHeight % 4 + 4;	

    return true;
}

int KSaveManager::SetLastCurrentPath(LPCTSTR pcszCurrent)
{
    int nResult = false;
    KG_PROCESS_ERROR(pcszCurrent);

    _tcsncpy(m_szLastCurrentPath, pcszCurrent, sizeof(m_szLastCurrentPath) / sizeof(TCHAR));

    nResult = true;
Exit0:
    return nResult;
}

int KSaveManager::SetLastOpenOrSaveDir(LPCTSTR pcszPath)
{
    int nResult = false;
    KG_PROCESS_ERROR(pcszPath);

    _tcsncpy(m_szLastOpenOrSaveDir, pcszPath, sizeof(m_szLastOpenOrSaveDir) / sizeof(TCHAR));

    nResult = true;
Exit0:
    return nResult;
}

int KSaveManager::SetLastUITexOpenOrSaveDir(LPCTSTR pcszUITexPath)
{
    int nResult = false;
    KG_PROCESS_ERROR(pcszUITexPath);

    _tcsncpy(m_szLastUITexOpenOrSaveDir, pcszUITexPath, sizeof(m_szLastUITexOpenOrSaveDir) / sizeof(TCHAR));

    nResult = true;
Exit0:
    return nResult;
}

LPCTSTR KSaveManager::GetLastOpenOrSaveDir()
{
    return m_szLastOpenOrSaveDir;
}

LPCTSTR KSaveManager::GetLastCurrentPath()
{
    return m_szLastCurrentPath;
}

LPCTSTR KSaveManager::GetLastUITexOpenOrSaveDir()
{
    return m_szLastUITexOpenOrSaveDir;
}

int KSaveManager::InitLastOpenOrSaveDir()
{
    ::GetCurrentDirectory(sizeof(m_szLastOpenOrSaveDir), m_szLastOpenOrSaveDir);
    return true;
}

int KSaveManager::IsLastOpenDirExist()
{
    if (m_szLastOpenOrSaveDir[0] != _T('\0'))
        return true;
    return false;
}

int KSaveManager::IsLastCurrentPath()
{
    if (m_szLastCurrentPath[0] != _T('\0'))
        return true;
    return false;
}

int KSaveManager::IsLastUITexOpenDirExist()
{
    if (m_szLastUITexOpenOrSaveDir[0] != _T('\0'))
        return true;
    return false;
}

int KSaveManager::SetSaveHideCode()
{
    m_nSaveHideCode = true;
    return true;
}

int KSaveManager::ClearSaveHideCode()
{
    m_nSaveHideCode = false;
    return true;
}

int KSaveManager::ExportText(LPCTSTR pcszName)
{
    int nResult = false;
    int nRetCode = false;
    FILE *fpFile = NULL; 
    UITEXFILEHEADER UiTexHeader;

    KG_PROCESS_ERROR(pcszName);
    KG_PROCESS_ERROR(pcszName[0] != _T('\0'));

    AutoSetTgaFileAndSTexFile();

    nRetCode = KTgaManager::GetSelf().FormateSaveData();
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = KAnimateManager::GetSelf().FormateSaveData();
    KG_PROCESS_ERROR(nRetCode);

    FormateUITexHeader(UiTexHeader);

    // TODO: 输出各项统计数据
    fpFile = fopen(pcszName ,"w");
    KG_PROCESS_ERROR(fpFile);
    
    fprintf(fpFile, "%s\n", "Dtail Infomation:");
    fprintf(fpFile, "%s \t: %s\n", "UiTex file name", m_szUITexFile);
    fprintf(fpFile, "%s \t: %s\n", "STex  file name", m_szSTexFile);
    fprintf(fpFile, "%s \t: %s\n", "Tga   file name", m_szTgaFile);
    fprintf(
        fpFile, "%s \t: %d * %d\n", "Make up tga size", 
        KTgaManager::GetSelf().GetSaveBufferWidth(), 
        KTgaManager::GetSelf().GetSaveBufferHeight()
    );
    fprintf(
        fpFile, "%s \t: %d * %d\n", "Real save tga size", 
        UiTexHeader.nWidth, UiTexHeader.nHeight
    );
    fprintf(
        fpFile, "%s \t: %d%s\n", "Using rate", 
        (int)((float)(KTgaManager::GetSelf().GetSaveBufferWidth() *      \
            KTgaManager::GetSelf().GetSaveBufferHeight()) /        \
        (float)(UiTexHeader.nWidth * UiTexHeader.nHeight)  * 100), "%"
    );

    fprintf(fpFile, "\n\n");
    KTgaManager::GetSelf().ExportText(fpFile);
    fprintf(fpFile, "\n\n");
    KAnimateManager::GetSelf().ExportText(fpFile);

    fclose(fpFile);
    fpFile = NULL;

    nResult = true;
Exit0:
    //清空保存时产生的数据，以节省内存。
    KTgaManager::GetSelf().ClearSaveData();
    KAnimateManager::GetSelf().ClearSaveData();
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    return nResult;
}

int KSaveManager::ExportText1(LPCTSTR pcszName)
{
	int nResult = false;
	int nRetCode = false;
	FILE *fpFile = NULL;
	UITEXFILEHEADER UiTexHeader;

	KG_PROCESS_ERROR(pcszName);
	KG_PROCESS_ERROR(pcszName[0] != _T('\0'));

	AutoSetTgaFileAndSTexFile();

	nRetCode = KTgaManager::GetSelf().FormateSaveData();
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = KAnimateManager::GetSelf().FormateSaveData();
	KG_PROCESS_ERROR(nRetCode);

	FormateUITexHeader(UiTexHeader);

	fpFile = _tfopen(pcszName,"w");
	KG_PROCESS_ERROR(fpFile);

	fprintf(fpFile,"%s\t%s\t%s\t%s\t%s\t%s\n","Farme","Left","Top","Width","High","Road");

	KTgaManager::GetSelf().ExportNewText(fpFile);

	fclose(fpFile);
	fpFile = NULL;

	nResult = true;
Exit0:
	return nResult;
}