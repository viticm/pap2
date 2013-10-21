// KFileFinder.cpp: implementation of the KFileFinder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "KFileFinder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KFileFinder::KFileFinder()
{

}

KFileFinder::~KFileFinder()
{

}

HRESULT KFileFinder::AddPath(LPSTR Path,BOOL IncludeSub)
{
	TCHAR Path_abs[256];
	TCHAR path_buffer[256];
	//TCHAR drive[256];
	//TCHAR dir[256];
	//TCHAR fname[256];
	//TCHAR ext[256];

	WIN32_FIND_DATA Find;
	TCHAR str[256];
	wsprintf(str,"%s\\*.*",Path);

	wsprintf(Path_abs,"%s",Path);
	int Length = (int)strlen(Path_abs);
	
	if(Path_abs[Length-1]!='\\')
	{
		Path_abs[Length] = '\\';
		Path_abs[Length+1] = '\0';
	}

	HANDLE hFind = FindFirstFile(str,&Find);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		wsprintf(path_buffer,"%s",Find.cFileName);
		
		if((_strcmpi(path_buffer,".")!=0)&&(_strcmpi(path_buffer,"..")!=0))
		{
			wsprintf(str,"%s%s",Path_abs,path_buffer);
			DWORD Attrib = GetFileAttributes(str);
			if((Attrib!=0xFFFFFFFF)&&(Attrib&FILE_ATTRIBUTE_DIRECTORY))
			{
				wsprintf(path_buffer,"%s\\",str);
				string name = path_buffer;
				m_vecPath.push_back(name);
			}
		}
		while(FindNextFile(hFind,&Find))
		{
			wsprintf(path_buffer,"%s",Find.cFileName);
			if((_strcmpi(path_buffer,".")!=0)&&(_strcmpi(path_buffer,"..")!=0))
			{
				wsprintf(str,"%s%s",Path_abs,path_buffer);
				DWORD Attrib = GetFileAttributes(str);
				if((Attrib!=0xFFFFFFFF)&&(Attrib&FILE_ATTRIBUTE_DIRECTORY))
				{
					wsprintf(path_buffer,"%s\\",str);
					string name = path_buffer;
					m_vecPath.push_back(name);
					
					if(IncludeSub)
					{
						AddPath(str,IncludeSub);
					}
				}
			}
		}
		FindClose(hFind);
	}
	return S_OK;
}

HRESULT KFileFinder::FindFile(LPSTR pOutName,LPSTR pInName)
{
	DWORD Attrib = GetFileAttributes(pInName);
	if(Attrib!=0xFFFFFFFF)
	{
		wsprintf(pOutName,"%s",pInName);
		return S_OK;
	}
	else
	{
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		wsprintf(path_buffer,"%s",pInName);
		_splitpath( path_buffer, drive, dir, fname, ext );

		if(_strcmpi(ext,"")==0)
		{
			return E_FAIL;
		}

		for(DWORD i=0;i<m_vecPath.size();i++)
		{
			wsprintf(pOutName,"%s%s%s",m_vecPath[i].c_str(),fname,ext);
			DWORD Attrib = GetFileAttributes(pOutName);
			if(Attrib!=0xFFFFFFFF)
			{
				return S_OK;
			}
		}
		wsprintf(pOutName,"");
		return E_FAIL;
	}

}
