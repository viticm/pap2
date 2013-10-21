#include "stdafx.h"
#include <atlpath.h>
#include "KG_TabFile.h"

static const int CELL_SIZE = 4;
static const char CACHE_PATH[] = "\\Cache";
static const char FILE_MAP_PREFIX[] = "kg_jx3_tab";


static int ErrorCodePrintf(DWORD dwError)
{
    LPSTR pszErrorString = NULL;

    if (dwError != 0)
    {
        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            dwError,
            0,
            (LPTSTR)&pszErrorString,
            0,
            NULL
        );

        KGLogPrintf(
            KGLOG_ERR,
            "Errorcode::%s!\n",
            pszErrorString
        );
    }

    if (pszErrorString)
    {
        LocalFree(pszErrorString);
    }
    return true;
}

static int FormatBinPath(const char *pcszFilePath, char szRetBinFile[], unsigned uBinFileLength)
{
    int nResult = false;
    int nRetCode = false;

    char szBinPath[MAX_PATH];
    char szFilePath[MAX_PATH];
    size_t ulength = 0;

    KGLOG_PROCESS_ERROR(pcszFilePath);
    KGLOG_PROCESS_ERROR(pcszFilePath[0]);
    KGLOG_PROCESS_ERROR(szRetBinFile);

    ulength = strlen(pcszFilePath);
    KGLOG_PROCESS_ERROR(ulength > 2);

    g_GetFullPath(szFilePath, pcszFilePath);
    szFilePath[sizeof(szFilePath) - 1] = '\0';
    g_GetFilePathFromFullPath(szBinPath, szFilePath);
    szBinPath[sizeof(szBinPath) - 1] = '\0';

    nRetCode = snprintf(
        szFilePath,
        sizeof(szFilePath),
        "%s\\%s",
        CACHE_PATH,
        szBinPath
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    g_GetFullPath(szRetBinFile, szFilePath);
    szRetBinFile[uBinFileLength - 1] = '\0';
    g_ChangeFileExt(szRetBinFile, "ktb");
    szRetBinFile[uBinFileLength - 1] = '\0';
    g_ExtractFilePath(szBinPath, szRetBinFile);
    szBinPath[sizeof(szBinPath) - 1] = '\0';

    nRetCode = PathFileExists(szBinPath);
    KG_PROCESS_SUCCESS(nRetCode);

    nRetCode = g_CreatePath(szBinPath);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

static int FormatFileMapName(const char *pcszFilePath, char szFileMapName[], unsigned uLength)
{
    int nResult = false;
    int nRetCode = false;

    size_t i = 0;
    size_t uNameLength = 0;

    char szFileName[MAX_PATH];

    KGLOG_PROCESS_ERROR(pcszFilePath);
    KGLOG_PROCESS_ERROR(pcszFilePath[0]);
    KGLOG_PROCESS_ERROR(szFileMapName);

    g_GetFullPath(szFileName, pcszFilePath);
    szFileName[sizeof(szFileName - 1)] = '\0';

    uNameLength = snprintf(
        szFileMapName,
        uLength,
        "%s%s",
        FILE_MAP_PREFIX,
        szFileName
    );
    KGLOG_PROCESS_ERROR(uNameLength > 0);
    szFileMapName[uLength - 1] = '\0';

    for (i = 0; i < uNameLength; i++)
    {
        if (szFileMapName[i] == '\\' || szFileMapName[i] == '/')
        {
            szFileMapName[i] = '_';
        }
    }

    nResult = true;
Exit0:
    return nResult;
}

KG_TabFile::KG_TabFile()
{
    m_emFileOpenMode = KG_FILE_OPEN_NORMAL;
    m_uRowCount = 0;
    m_uColumnCount = 0;
    m_pszFormat = NULL;

    m_hFileMap = NULL;
    m_pszText = NULL;
    m_pszMapViewText = NULL;

    m_pBinTab = NULL;
    m_pbyData = NULL;

    m_dwLastError = 0;
}

KG_TabFile::~KG_TabFile()
{
}

IKG_TabFile* OpenTabFile(
    const char *pcszFilePath,
    const char *pcszFormat,
    KG_FILE_OPEN_MODE emFileOpenMode
)
{
    int nResult = false;
    int nRetCode = false;

    HANDLE hFile = INVALID_HANDLE_VALUE;
    KG_TabFile *pTabFile = NULL;

    __int64 nTextOffset = 0;
    __int64 nFileOffset = 0;
    char szBinFile[MAX_PATH];
    char szFilePath[MAX_PATH];
    char szFileMapName[MAX_PATH];

    size_t uFormatLength = 0;

    KGLOG_PROCESS_ERROR(pcszFilePath);
    KGLOG_PROCESS_ERROR(pcszFilePath[0]);
    KGLOG_PROCESS_ERROR(pcszFormat);
    KGLOG_PROCESS_ERROR(pcszFormat[0]);
    KGLOG_PROCESS_ERROR(emFileOpenMode >= KG_FILE_OPEN_MODE_BEGIN);
    KGLOG_PROCESS_ERROR(emFileOpenMode < KG_FILE_OPEN_MODE_COUNT);

    pTabFile = new(nothrow) KG_TabFile();
    KGLOG_PROCESS_ERROR(pTabFile);

    pTabFile->m_emFileOpenMode = emFileOpenMode;
    
    uFormatLength = strlen(pcszFormat);
    pTabFile->m_pszFormat = new(nothrow) char[uFormatLength + 1];
    KGLOG_PROCESS_ERROR(pTabFile->m_pszFormat);

    strncpy(pTabFile->m_pszFormat, pcszFormat, uFormatLength);
    pTabFile->m_pszFormat[uFormatLength] = '\0';

    pTabFile->m_uColumnCount = uFormatLength;

    strncpy(szFilePath, pcszFilePath, sizeof(szFilePath));
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    nRetCode = FormatFileMapName(szFilePath, szFileMapName, sizeof(szFileMapName));
    KGLOG_PROCESS_ERROR(nRetCode);
     
    
    pTabFile->m_pBinTab = new(nothrow) KBinTab();
    KGLOG_PROCESS_ERROR(pTabFile->m_pBinTab);

    nRetCode = pTabFile->m_pBinTab->Load(szFilePath, pcszFormat);
    KGLOG_PROCESS_ERROR(nRetCode);

    pTabFile->m_pbyData = (BYTE *)pTabFile->m_pBinTab->Data(pTabFile->m_uRowCount);
    KGLOG_PROCESS_ERROR(pTabFile->m_pbyData);

    KG_PROCESS_SUCCESS(emFileOpenMode == KG_FILE_OPEN_NORMAL);

    pTabFile->m_hFileMap = OpenFileMapping(FILE_MAP_READ, FALSE, szFileMapName);
    if (pTabFile->m_hFileMap == NULL)
    {
        nRetCode = FormatBinPath(szFilePath, szBinFile, sizeof(szBinFile));
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = pTabFile->m_pBinTab->Save(szBinFile);
        KGLOG_PROCESS_ERROR(nRetCode);

        hFile = CreateFile(
            szBinFile, 
            GENERIC_READ,
            0, 
            NULL, 
            OPEN_EXISTING, 
            FILE_ATTRIBUTE_NORMAL, 
            NULL
        );
        KGLOG_PROCESS_ERROR(hFile != INVALID_HANDLE_VALUE);

        pTabFile->m_hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, szFileMapName);
        KGLOG_PROCESS_ERROR(pTabFile->m_hFileMap);
    }

    GetSystemInfo(&pTabFile->m_SysInfo);
    
    nTextOffset = pTabFile->m_uRowCount * pTabFile->m_uColumnCount * CELL_SIZE;
    nFileOffset = nTextOffset / pTabFile->m_SysInfo.dwAllocationGranularity * pTabFile->m_SysInfo.dwAllocationGranularity;
    pTabFile->m_pszMapViewText = (char *)MapViewOfFile(
        pTabFile->m_hFileMap, 
        FILE_MAP_READ,
        (DWORD)(nFileOffset  >> 32),
        (DWORD)(nFileOffset & 0xFFFFFFF),
        0
    );
    KGLOG_PROCESS_ERROR(pTabFile->m_pszMapViewText);
    pTabFile->m_pszText = pTabFile->m_pszMapViewText + nTextOffset - nFileOffset;
   
Exit1:
    nResult = true;
Exit0:
    if (!nResult && emFileOpenMode == KG_FILE_OPEN_FILE_MAPPING)
    {
        DWORD dwError = GetLastError();
        ErrorCodePrintf(dwError);
    }

    if (hFile != INVALID_HANDLE_VALUE)
    {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
    if (pTabFile && (!nResult || emFileOpenMode == KG_FILE_OPEN_FILE_MAPPING))
    {
        SAFE_DELETE(pTabFile->m_pBinTab);
        pTabFile->m_pbyData = NULL;
    }
    if (!nResult)
    {
        if (pTabFile)
        {
            delete [] pTabFile->m_pszFormat;
            pTabFile->m_pszFormat = NULL;

            if (pTabFile->m_pszMapViewText)
            {
                UnmapViewOfFile(pTabFile->m_pszMapViewText);
                pTabFile->m_pszMapViewText = NULL;
            }
            pTabFile->m_pszText = NULL;

            CloseHandle(pTabFile->m_hFileMap);
            pTabFile->m_hFileMap = NULL;
        }

        SAFE_DELETE(pTabFile);

        KGLogPrintf(
            KGLOG_ERR,
            "OpenTabFile(%s) Failed!\n",
            pcszFilePath
        );
    }

    return pTabFile;
}

ULONG KG_TabFile::Release()
{
    delete [] m_pszFormat;
    m_pszFormat = NULL;

    if (m_pszMapViewText)
    {
        UnmapViewOfFile(m_pszMapViewText);
        m_pszMapViewText = NULL;
    }
    m_pszText = NULL;

    CloseHandle(m_hFileMap);
    m_hFileMap = NULL;

    m_pbyData = NULL;
    SAFE_DELETE(m_pBinTab);

    delete this;
    return 0;
}

size_t KG_TabFile::GetRowCount()
{
    return m_uRowCount;
}

int KG_TabFile::Search(
    void *pvKey,
    FUNC_COMPARE fnCompare,
    void *pvRetResul, 
    unsigned uLength
)
{
    int nResult = false;
    int nRetCode = false;

    BYTE *pbyRow = NULL;
    size_t uMapFileLength = NULL;

    KGLOG_PROCESS_ERROR(pvKey);
    KGLOG_PROCESS_ERROR(fnCompare);
    KGLOG_PROCESS_ERROR(pvRetResul);
    KGLOG_PROCESS_ERROR(uLength >= m_uColumnCount * CELL_SIZE);

    if (m_emFileOpenMode == KG_FILE_OPEN_FILE_MAPPING)
    {
        uMapFileLength = m_uColumnCount * m_uRowCount * CELL_SIZE;
        m_pbyData = (BYTE *)MapViewOfFile(m_hFileMap, FILE_MAP_READ, 0, 0, uMapFileLength);
        KGLOG_PROCESS_ERROR(m_pbyData);
    }
  
    pbyRow = (BYTE *)bsearch(
        pvKey,
        m_pbyData, 
        m_uRowCount, 
        m_uColumnCount * CELL_SIZE, 
        fnCompare
    );
    KG_PROCESS_ERROR(pbyRow);

    memcpy(pvRetResul, pbyRow, m_uColumnCount * CELL_SIZE);

    if (m_emFileOpenMode == KG_FILE_OPEN_FILE_MAPPING)
    {
        nRetCode = OffsetToPointer(pvRetResul);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
   
    nResult = true;
Exit0:
    if (m_emFileOpenMode == KG_FILE_OPEN_FILE_MAPPING && !m_pbyData)
    {
        DWORD dwError = GetLastError();
        if (dwError != m_dwLastError)
        {
            m_dwLastError = dwError;
            ErrorCodePrintf(dwError);
        }
    }

    if (m_emFileOpenMode == KG_FILE_OPEN_FILE_MAPPING && m_pbyData)
    {
        UnmapViewOfFile(m_pbyData);
        m_pbyData = NULL;
		
    }
	
    return nResult;
}

int KG_TabFile::OffsetToPointer(void *pvRow)
{
    int nResult = false;
    int nRetCode = false;

    BYTE *pbyRow = NULL;
    BYTE *pbyCell = NULL;
    int i = 0;
    int nOffset = 0;
    char **ppchCell = NULL;

    KGLOG_PROCESS_ERROR(pvRow);

    pbyRow = (BYTE *)pvRow;
    for (i = 0; i < (int)m_uColumnCount; i++)
    {
        if (m_pszFormat[i] != 's' && m_pszFormat[i] != 'S')
            continue;

        pbyCell = pbyRow + CELL_SIZE * i;
        nOffset = *((int *)pbyCell);
        ASSERT(sizeof(char *) == sizeof(int));
        ppchCell = (char **)pbyCell;
        *ppchCell = (char *)(m_pszText + nOffset);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KG_TabFile::GetRow(unsigned uIndex, void *pvRetRow, unsigned uLength)
{
    int nResult = false;
    int nRetCode = false;

    BYTE *pbyMapViewRowData = NULL;
    BYTE *pbyRowData = NULL;
    unsigned uRowLength = 0;
    __int64 nRowOffset = 0;
    __int64 nFileOffset = 0;

    KGLOG_PROCESS_ERROR(uIndex >= 0);
    KGLOG_PROCESS_ERROR(uIndex < m_uRowCount);
    KGLOG_PROCESS_ERROR(pvRetRow);
    KGLOG_PROCESS_ERROR(uLength >= m_uColumnCount * CELL_SIZE);

    if (m_emFileOpenMode == KG_FILE_OPEN_NORMAL)
    {
        pbyRowData = m_pbyData + uIndex * m_uColumnCount * CELL_SIZE;
        memcpy(pvRetRow, pbyRowData, m_uColumnCount * CELL_SIZE);

        KG_PROCESS_SUCCESS(true);
    }

    nRowOffset = uIndex * m_uColumnCount * CELL_SIZE;
    nFileOffset = nRowOffset / m_SysInfo.dwAllocationGranularity * m_SysInfo.dwAllocationGranularity;
    uRowLength = (unsigned)(m_uColumnCount * CELL_SIZE + (nRowOffset - nFileOffset) * CELL_SIZE);
    pbyMapViewRowData = (BYTE *)MapViewOfFile(
        m_hFileMap, 
        FILE_MAP_READ, 
        (DWORD)(nFileOffset >> 32), 
        (DWORD)(nFileOffset & 0xFFFFFFF), 
        uRowLength
    );

    KGLOG_PROCESS_ERROR(pbyMapViewRowData);
    pbyRowData = pbyMapViewRowData + nRowOffset - nFileOffset;

    memcpy(pvRetRow, pbyRowData, m_uColumnCount * CELL_SIZE);
    
    nRetCode = OffsetToPointer(pvRetRow);
    KGLOG_PROCESS_ERROR(nRetCode);
 
Exit1:
    nResult = true;
Exit0:
    if (pbyMapViewRowData)
    {
        UnmapViewOfFile(pbyMapViewRowData);
        pbyMapViewRowData = NULL;
    }
    if (!nResult)
    {
        DWORD dwError = GetLastError();
        ErrorCodePrintf(dwError);
    }
    return nResult;
}



