#include "StdAfx.h"
#include "kg3dfilefinder.h"
#include "KG3DEngineManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DFileFinder::KG3DFileFinder(void)
{
}

KG3DFileFinder::~KG3DFileFinder(void)
{
}


//HRESULT KG3DFileFinder::AddPath(LPSTR Path,BOOL IncludeSub)
//{
//	TCHAR Path_abs[256];
//	TCHAR path_buffer[256];
//
//	WIN32_FIND_DATA Find;
//	TCHAR str[256];
//
//	wsprintf(str,"%s%s\\*.*",g_Work_Directory,Path);
//	wsprintf(Path_abs,"%s",Path);
//
//	HANDLE hFind = FindFirstFile(str,&Find);
//	if (hFind != INVALID_HANDLE_VALUE)
//	{
//		wsprintf(path_buffer,"%s",Find.cFileName);
//
//		if((_strcmpi(path_buffer,".")!=0)&&(_strcmpi(path_buffer,"..")!=0))
//		{
//			wsprintf(str,"%s%s%s",g_Work_Directory,Path_abs,path_buffer);
//			DWORD Attrib = GetFileAttributes(str);
//			if((Attrib!=0xFFFFFFFF)&&(Attrib&FILE_ATTRIBUTE_DIRECTORY))
//			{
//				TCHAR Dir[256];
//				wsprintf(Dir,"%s%s",Path_abs,path_buffer);
//				string name = Dir;
//				m_vecPath.push_back(name);
//			}
//		}
//		while(FindNextFile(hFind,&Find))
//		{
//			wsprintf(path_buffer,"%s",Find.cFileName);
//			if((_strcmpi(path_buffer,".")!=0)&&(_strcmpi(path_buffer,"..")!=0))
//			{
//				wsprintf(str,"%s%s\\%s",g_Work_Directory,Path_abs,path_buffer);
//				DWORD Attrib = GetFileAttributes(str);
//				if((Attrib!=0xFFFFFFFF)&&(Attrib&FILE_ATTRIBUTE_DIRECTORY))
//				{
//					TCHAR Dir[256];
//					wsprintf(Dir,"%s\\%s",Path_abs,path_buffer);
//					string name = Dir;
//					m_vecPath.push_back(name);
//					if(IncludeSub)
//					{
//						AddPath(Dir,IncludeSub);
//					}
//				}
//			}
//		}
//		FindClose(hFind);
//	}
//	return S_OK;
//}

//HRESULT KG3DFileFinder::FindFile(LPSTR pOutAbsName,LPSTR pOutRetName,LPSTR pInName)
//{
//	DWORD Attrib = GetFileAttributes(pInName);
//
//	TCHAR path_buffer[256];
//	TCHAR drive[256];
//	TCHAR dir[256];
//	TCHAR fname[256];
//	TCHAR ext[256];
//
//	wsprintf(path_buffer,"%s",pInName);
//	_splitpath( path_buffer, drive, dir, fname, ext );
//
//	if(_strcmpi(ext,"")==0)
//	{
//		wsprintf(pOutAbsName,"");
//		if(pOutRetName)
//			wsprintf(pOutRetName,"");
//		return E_FAIL;
//	}
//
//	for(DWORD i=0;i<m_vecPath.size();i++)
//	{
//		wsprintf(pOutAbsName,"%s%s\\%s%s",g_Work_Directory,m_vecPath[i].c_str(),fname,ext);
//		DWORD Attrib = GetFileAttributes(pOutAbsName);
//		if(Attrib!=0xFFFFFFFF)
//		{
//			if(pOutRetName)
//			{
//				wsprintf(pOutRetName,"%s\\%s%s",m_vecPath[i].c_str(),fname,ext);
//				if(pOutRetName[0]=='\\')
//				{
//					LPSTR pSec = &pOutRetName[1];
//					TCHAR str[256];
//					wsprintf(str,"%s",pSec);
//					wsprintf(pOutRetName,"%s",str);
//				}
//			}
//			return S_OK;
//		}
//	}
//	wsprintf(pOutAbsName,"");
//	if(pOutRetName)
//		wsprintf(pOutRetName,"");
//	return E_FAIL;
//}

HRESULT KG3DFileFinder::Init()
{
	strcpy_s(m_strLowerDefDir, MAX_PATH, g_Def_WorkDirectory);
	strlwr(m_strLowerDefDir);
	return S_OK;
}

HRESULT KG3DFileFinder::GetRelativePath(const TCHAR* strAbsPath11, const TCHAR* strAbsPath21, TCHAR* strRelativePath)
{
    ASSERT(strAbsPath11);
    ASSERT(strAbsPath21);
    ASSERT(strRelativePath);

	HRESULT hr = E_FAIL;
	size_t uLength = 0;
	TCHAR strDrive1[MAX_PATH];
	TCHAR strPath1[MAX_PATH];
	TCHAR strFileName1[MAX_PATH];
	TCHAR strExt1[MAX_PATH];

	TCHAR strDrive2[MAX_PATH];
	TCHAR strPath2[MAX_PATH];
	TCHAR strFileName2[MAX_PATH];
	TCHAR strExt2[MAX_PATH];

	TCHAR strAbsPath1[MAX_PATH];
	TCHAR strAbsPath2[MAX_PATH];

	TCHAR* strFind = NULL;

	int nIndex = 0;
	int nDepthsOfDirectory = 0;
	int nPath1Length = 0;
	int nPath2Length = 0;
	strRelativePath[0] = '\0'; 

    if (strAbsPath21[0] == '\0')
        return S_OK;
	
	strcpy(strAbsPath1, strAbsPath11);
	strcpy(strAbsPath2, strAbsPath21);

	_strlwr(strAbsPath1);
	_strlwr(strAbsPath2);

	NormalizePath(strAbsPath1);
	NormalizePath(strAbsPath2);

	if (!strstr(strAbsPath2, ":"))
	{
		strcpy(strRelativePath, strAbsPath2);
		hr = S_OK;
		goto Exit0;
	}
	_splitpath(strAbsPath1, strDrive1, strPath1, strFileName1, strExt1);
	_splitpath(strAbsPath2, strDrive2, strPath2, strFileName2, strExt2);

	
	if(strcmp(strDrive1, strDrive2))
	{
		//If the driver's name is not the same, no way to got the relative path
		strcpy(strRelativePath, strAbsPath2);
		goto Exit0;
	}
	
	strFind = strstr(strPath2, strPath1);
	if(strFind)//strPath1 is in strPath2, means strPath2 is a sub directory of strPath1
	{
		strFind += strlen(strPath1);
		strcpy(strRelativePath, strFind);
		strcat(strRelativePath, strFileName2);
		strcat(strRelativePath, strExt2);
		hr = S_OK;
	}
	else
	{
		//If not check if strPath1 is a sub directory of strPath2
		strFind = strstr(strPath1, strPath2);
		if(strFind)
		{
			//strPath1 is a sub directory of strPath2
			nIndex = (int) strlen(strPath1) - 1;
			nDepthsOfDirectory = 0;
			uLength = strlen(strPath2);
			while(nIndex > 0 && (strPath1 + nIndex) != strFind + uLength)
			{
				if(strPath1[nIndex] == '\\')
					nDepthsOfDirectory++;
				nIndex--;
			}
			for(int i = 0; i < nDepthsOfDirectory; i++)
			{
				strcat(strRelativePath, "..\\");
			}
			//if(*(strFind + uLength) == '\\')
			//	strcat(strRelativePath, strFind + uLength + 1);
			//else
			//	strcat(strRelativePath, strFind + uLength);
			strcat(strRelativePath, strFileName2);
			strcat(strRelativePath, strExt2);
			hr = S_OK;
		}
		else//strPath1 and strPath2 has no sub directory relationship, but they are in the same disk
		{
			nPath1Length = static_cast<int>(strlen(strPath1));
			nPath2Length = static_cast<int>(strlen(strPath2));
			int nShorterPath = (nPath1Length > nPath2Length) ? nPath2Length : nPath1Length;
			int nSamePathIndex = (strPath1[0] == '\\' ? 1 : 0);
			int nSameDirIndex = 0;
			int nSameDir = 0;
			while(nSamePathIndex < nShorterPath)
			{
				if(strPath1[nSamePathIndex] != strPath2[nSamePathIndex])
					break;
				if(strPath1[nSamePathIndex] == '\\')
				{
					nSameDir++;
					nSameDirIndex = nSamePathIndex;
				}
				nSamePathIndex++;
			}
			nDepthsOfDirectory = 0;
			nIndex = (int)strlen(strPath1) - 1;
			while(strPath1[nIndex] == '\\')
				nIndex--;
			while(nIndex >= 0)
			{
				if(strPath1[nIndex] == '\\')
				{
					nDepthsOfDirectory++;
				}
				nIndex--;
			}
			nDepthsOfDirectory -= nSameDir;
			for(int i = 0; i < nDepthsOfDirectory; i++)
			{
				strcat(strRelativePath, "..\\");
			}

			if(nSameDir)
			{
				if(strPath2[nSameDirIndex] == '\\')
					strcat(strRelativePath, strPath2 + nSameDirIndex + 1);
				else
					strcat(strRelativePath, strPath2 + nSameDirIndex);
			}
			else
			{
				if(strPath2[0] == '\\')
					strcat(strRelativePath, strPath2 + 1);
				else
					strcat(strRelativePath, strPath2);
			}
			strcat(strRelativePath, strFileName2);
			strcat(strRelativePath, strExt2);
			hr = S_OK;
		}
	}
Exit0:
	return hr;
}

HRESULT KG3DFileFinder::GetAbsPath(const TCHAR* strFileName, TCHAR* strOutPath)
{
	HRESULT hr = E_FAIL;
	/*TCHAR strDrive1[MAX_PATH];
	TCHAR strPath1[MAX_PATH];
	TCHAR strFileName1[MAX_PATH];
	TCHAR strExt1[MAX_PATH];*/
	ASSERT(strFileName);
	ASSERT(strOutPath);
	
	//_splitpath(strAbsPath1, strDrive1, strPath1, strFileName1, strExt1);
	strcpy(strOutPath, strFileName);
	size_t strLen = strlen(strOutPath);
	while(strLen > 0 && strOutPath[strLen - 1] != '\\')
	{
		strLen--;	
	}
	if(strLen > 0)
	{
		strOutPath[strLen] = '\0';
		hr = S_OK;
	}
	return hr;
}

HRESULT KG3DFileFinder::GetAbsPathFromRelativePath(const TCHAR* strAbsPath, const TCHAR* strRelativePath, TCHAR* strOutPath)
{
	HRESULT hr = E_FAIL;
    
	ASSERT(strAbsPath);
	ASSERT(strRelativePath);
	ASSERT(strOutPath);
    size_t uAbsPathLen = strlen(strAbsPath);
	strOutPath[0] = '\0';
    const TCHAR* strDir = strRelativePath;
	const TCHAR* strFinalRelativePath = strDir;
    int nDirUp = 0;
	KG_PROCESS_ERROR(uAbsPathLen != 0);
    int nIndex = static_cast<int>(uAbsPathLen) - 1;
	do
	{	
		strDir = strstr(strDir, "..\\");
		if(strDir)
		{
			nDirUp++;
			strDir += 3;
			strFinalRelativePath = strDir;
		}
	}while(strDir);
	
	//除掉路径最后的那些"\"
	while(strAbsPath[nIndex] == '\\')
		nIndex--;
	while(nIndex > 0 && nDirUp)
	{
		if(strAbsPath[nIndex] == '\\') 
		{
			nDirUp--;			
		}
		nIndex--;
	}
	if(nDirUp == 0)
	{	
		strcpy(strOutPath, strAbsPath);
		strOutPath[nIndex + 1] = '\0';
		if(*strFinalRelativePath != '\\')
		{
			strcat(strOutPath, "\\");
		}
		strcat(strOutPath, strFinalRelativePath);
	}
    hr = S_OK;
Exit0:
	return hr;
}


void KG3DFileFinder::NormalizePath(TCHAR* strPath)
{
	int i = 0;
	int nPathLength = 0;
	
	if (!strPath)
		return;

	nPathLength = (int)strlen(strPath);
	for(i = 1; i < nPathLength; i++)
	{
		if (strPath[i - 1] == '\\' &&
			strPath[i] == '\\')
			strmov(strPath, i, 1);
	}
}



void KG3DFileFinder::strmov(TCHAR* strPath, int nStartPos, int nOffset)
{
	TCHAR* strBuffer = NULL;
	int nStrLen = 0; 
		
	if (!strPath)
		return;

	nStrLen = (int)strlen(strPath) + 1;
	if (nStartPos >= nStrLen)
		return;

	strBuffer = new TCHAR[nStrLen];
	assert(strBuffer);

	nOffset = max(0, nStartPos - nOffset);
	strcpy(strBuffer, strPath);
	strcpy(strPath + nOffset, strBuffer + nStartPos);

	SAFE_DELETE_ARRAY(strBuffer);
}

void KG3DFileFinder::strShiftLeft(TCHAR* strPath, 
								  size_t uDestPos, 
								  size_t uSrcPos)
{
	TCHAR* strBuffer = NULL;
	KG_PROCESS_ERROR(strPath);

	size_t uSize = strlen(strPath) + 1;

	strBuffer = new TCHAR[uSize];
	KGLOG_PROCESS_ERROR(strBuffer);

	strcpy(strBuffer, strPath);

	strcpy(strPath + uDestPos, strBuffer + uSrcPos);
Exit0:
	SAFE_DELETE_ARRAY(strBuffer);

	
	
}

const char* GetRetPath(const char filePath[])
{

	TCHAR rootPath[MAX_PATH];
	TCHAR szfilePath[MAX_PATH];
	g_GetRootPath(rootPath);
	strncpy(szfilePath,filePath,MAX_PATH);
	_strlwr_s(rootPath,MAX_PATH);
	_strlwr_s(szfilePath,MAX_PATH);
	char* pStr = strstr(const_cast<char*>(szfilePath), rootPath);
	size_t offset = 0;

	if (pStr == szfilePath) 
	{
		offset = strlen(rootPath);            
		if (filePath[offset] == '\\')
			++offset;
	}

	return filePath + offset;
}

HRESULT KG3DFileFinder::GetRelativePath(LPCSTR strFileName, 
										TCHAR *strOutput, 
										size_t uSize)
{
	HRESULT hRetCode = E_FAIL;
	TCHAR strInput[MAX_PATH];
	TCHAR *strFind = NULL;
	KG_PROCESS_ERROR(strOutput);
	strcpy_s(strInput, MAX_PATH, strFileName);
	_strlwr_s(strInput, MAX_PATH);
	strFind = strstr(strInput, m_strLowerDefDir);
	if (strFind)
	{
		strcpy_s(strOutput, uSize, strFind + strlen(m_strLowerDefDir));
		hRetCode = S_OK;
	}
	else
	{
		strcpy_s(strOutput, uSize, strFileName);
	}
Exit0:
	return hRetCode;
}