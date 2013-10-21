#include "StdAfx.h"
#include "KG3DResourcePacker.h"
#include "KG3DEngineManager.h"
#include "engine/KMutex.h"

static int _PrepareDirectory(const char szPathFile[])
{
    int nResult = false;
    int nRetCoe = false;
	char szDriver[MAX_PATH];
	char szDir[MAX_PATH];
    vector<string> Dirs;
    char *pchCurPos = NULL;
    char szTemp[MAX_PATH];
   
	nRetCoe = _splitpath_s(szPathFile,
		szDriver,   MAX_PATH,
		szDir,      MAX_PATH,
		NULL,       0,
		NULL,       0
    );
    KGLOG_PROCESS_ERROR(nRetCoe == 0);

	pchCurPos = szDir; // szDir end with '\'
	while (*pchCurPos)
	{
		if ((pchCurPos != szDir) && (*pchCurPos == '\\'))
		{
			*pchCurPos = '\0';
			
            nRetCoe = snprintf(szTemp, countof(szTemp), "%s%s", szDriver, szDir);
            KGLOG_PROCESS_ERROR(nRetCoe > 0);
            KGLOG_PROCESS_ERROR(nRetCoe < countof(szTemp));

			Dirs.push_back(szTemp);
			*pchCurPos = '\\';
		}
		++pchCurPos;
	}

	for (size_t i = 0; i < Dirs.size(); i++)
	{
		mkdir(Dirs[i].c_str());
	}
    nResult = true;
Exit0:
    return nResult;
}

static int _NormalizePathString(
    const char cszPath[], char *pszRetNormalizedPath, unsigned uCharCountOfRetNormalizedPath
)
{
    int nResult = true;
    unsigned uPathLength = 0;
    static char s_szLowerDefWorkDirectory[MAX_PATH] = { 0 };
    char *pchCurPos = NULL;

    ASSERT(cszPath);
    ASSERT(pszRetNormalizedPath);
    uPathLength = (unsigned)strlen(cszPath);
    KGLOG_PROCESS_ERROR(uPathLength < uCharCountOfRetNormalizedPath);

    if (s_szLowerDefWorkDirectory[0] == '\0')
    {
        strncpy(s_szLowerDefWorkDirectory, g_Def_WorkDirectory, countof(s_szLowerDefWorkDirectory));
        s_szLowerDefWorkDirectory[countof(s_szLowerDefWorkDirectory) - 1] = '\0';
        strlwr(s_szLowerDefWorkDirectory);
    }

    strncpy(pszRetNormalizedPath, cszPath, uCharCountOfRetNormalizedPath);
    pszRetNormalizedPath[uCharCountOfRetNormalizedPath - 1] = '\0';
    strlwr(pszRetNormalizedPath);

    pchCurPos = pszRetNormalizedPath + uPathLength - 1;
    // remove \ or / at tail
    if (
        (pchCurPos >= pszRetNormalizedPath) && 
        ((*pchCurPos == '\\') || (*pchCurPos == '/'))
    )
    {
        *pchCurPos = '\0';
        pchCurPos--;
    }

    while (pchCurPos >= pszRetNormalizedPath)
    {
        if (*pchCurPos == '/')  // replace / to \ 
        {
            *pchCurPos = '\\';
        }

        if (
            (*pchCurPos == '\\') && 
            (
                (*(pchCurPos + 1)== '\\') ||         //remove double \ like "path1\\path2"
                (pchCurPos == pszRetNormalizedPath)  // remove \ at head like "\data\a.txt"
            )
        )
        {
            strcpy(pchCurPos, pchCurPos + 1);
        }
        --pchCurPos;
    }

    // remove resource with physic drivers path like "d:\kingsoft\...."
	pchCurPos = strstr(pszRetNormalizedPath, s_szLowerDefWorkDirectory);
	if (pchCurPos)
	{
        static unsigned s_uWorkDirectoryLen = (unsigned)strlen(s_szLowerDefWorkDirectory);
		strcpy(pszRetNormalizedPath, pchCurPos + s_uWorkDirectoryLen);
	}

    nResult = true;
Exit0:
    return nResult;
}


////////////////////////////////////////////////////////////////////////////////

class KG3DResourcePacker
{
public:
    KG3DResourcePacker(void);
    ~KG3DResourcePacker(void);
    int AddResource(const char cszResoureName[]);
private:
    int _SaveResourceList();
    int _SaveResource(const char cszDestDirectory[]);
private:
    KMutex m_ResourceLock;
    std::vector<std::string> m_vecResourceList;
    std::set<DWORD> m_setResourceID;
};
////////////////////////////////////////////////////////////////////////////////
KG3DResourcePacker g_ResourceParker;

KG3DResourcePacker::KG3DResourcePacker(void)
{
}

KG3DResourcePacker::~KG3DResourcePacker(void)
{
    if (g_cEngineOption.bOutputUsedResource)
    {
        _SaveResourceList();
        _SaveResource("d:\\Kingsoft\\JX3Benchmark");
    }
}

int KG3DResourcePacker::AddResource(const char cszResoureName[])
{
    int nResult  = false;
    int nRetCode = false;
	TCHAR szNormalizedResoureName[MAX_PATH];
    DWORD dwFileNameHashID = 0;

    KG_ASSERT_EXIT(cszResoureName);

	nRetCode = _NormalizePathString(cszResoureName, szNormalizedResoureName, countof(szNormalizedResoureName));
    KGLOG_PROCESS_ERROR(nRetCode);

	dwFileNameHashID = g_FileNameHash(szNormalizedResoureName);
    
    m_ResourceLock.Lock();
	if (!m_setResourceID.count(dwFileNameHashID))
	{
		m_setResourceID.insert(dwFileNameHashID);
		m_vecResourceList.push_back(szNormalizedResoureName);
	}
    m_ResourceLock.Unlock();
    
    nResult = true;
Exit0:
     return nResult;
}

int KG3DResourcePacker::_SaveResourceList()
{
	int nResult  = false;
	FILE *fpFile = fopen("usedresource.txt", "w+");
	KG_PROCESS_ERROR(fpFile);

    m_ResourceLock.Lock();
	for (size_t i = 0; i < m_vecResourceList.size(); i++)
	{
		fprintf(fpFile, "%s\n", m_vecResourceList[i].c_str());
	}
    m_ResourceLock.Unlock();

	nResult = true;
Exit0:
	if (fpFile)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	return nResult;
}

int KG3DResourcePacker::_SaveResource(const char cszDestDirectory[])
{
    m_ResourceLock.Lock();
    int nResult  = false;
    int nRetCode = false;
    char szNormalizedDestDirectory[MAX_PATH];
    char szDestFilePath[MAX_PATH];
    
    nRetCode = _NormalizePathString(cszDestDirectory, szNormalizedDestDirectory, countof(szNormalizedDestDirectory));
    KGLOG_PROCESS_ERROR(nRetCode);

    for (size_t i = 0; i < m_vecResourceList.size(); i++)
	{

		nRetCode = snprintf(
            szDestFilePath, countof(szDestFilePath),				
			"%s\\%s",
			cszDestDirectory,
			m_vecResourceList[i].c_str()
        );
        KGLOG_PROCESS_ERROR(nRetCode < countof(szDestFilePath));
        KGLOG_PROCESS_ERROR(nRetCode > 0);

		nRetCode = _PrepareDirectory(szDestFilePath);
        KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = CopyFile(m_vecResourceList[i].c_str(), szDestFilePath, TRUE);
		if (!nRetCode)
		{
			KGLogPrintf(KGLOG_ERR, "Copy file %s failed.\n", m_vecResourceList[i].c_str());
		}
	}

    nResult = S_OK;
Exit0:
    m_ResourceLock.Unlock();
	return nResult;
}


////////////////////////////////////////////////////////////////////////////////
// redirect file API in engine.dll
////////////////////////////////////////////////////////////////////////////////
#undef g_OpenIniFile
#undef g_OpenTabFile
#undef g_OpenFile
IIniFile *g_OpenIniFileShell(const char* FileName, int ForceUnpakFile/* = false*/, int ForWrite/* = false*/)
{
    IIniFile *piIniFile = g_OpenIniFile(FileName, ForceUnpakFile, ForWrite); 
    if (g_cEngineOption.bOutputUsedResource && piIniFile)
    {
        g_ResourceParker.AddResource(FileName);
    }

    return piIniFile;
}
ITabFile *g_OpenTabFileShell(const char* FileName, int ForceUnpakFile/* = false*/, int ForWrite/* = false*/)
{
    ITabFile *piTableFile = g_OpenTabFile(FileName, ForceUnpakFile, ForWrite);
    if (g_cEngineOption.bOutputUsedResource && piTableFile)
    {
        g_ResourceParker.AddResource(FileName);
    }

    return piTableFile;
}
IFile *g_OpenFileShell(const char* FileName, int ForceUnpakFile/* = false*/, int ForWrite/* = false*/)
{
    IFile *piFile = g_OpenFile(FileName, ForceUnpakFile, ForWrite);
    if (g_cEngineOption.bOutputUsedResource && piFile)
    {
        g_ResourceParker.AddResource(FileName);
    }
     return piFile;
}
