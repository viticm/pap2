
#pragma once
#include "../../../Include/kg3dengine/kg3dinterface.h"

using namespace std;

class KG3DFileFinder
{
private:
	//vector<string>m_vecPath;

public:
	HRESULT Init();
	void strmov(TCHAR* strPath, int nStartPos, int nOffset);
	//HRESULT AddPath(LPSTR Path,BOOL IncludeSub);
	//HRESULT FindFile(LPSTR pOutAbsName,LPSTR pOutRetName,LPSTR pInName);

	void strShiftLeft(TCHAR* strPath, 
		size_t uDestPos, 
		size_t uSrcPos);
	TCHAR m_strLowerDefDir[MAX_PATH];
public:
	KG3DFileFinder(void);
	virtual ~KG3DFileFinder(void);
	
	//Get the strPath2's relative path to strPath1.
	HRESULT GetRelativePath(const TCHAR* strAbsPath1, const TCHAR* strAbsPath2, TCHAR* strRelativePath);
	HRESULT GetAbsPathFromRelativePath(const TCHAR* strAbsPath, const TCHAR* strRelativePath, TCHAR* strOutPath);
	HRESULT GetAbsPath(const TCHAR* strFileName, TCHAR* strOutPath);
	void NormalizePath(TCHAR* strPath);
	
	HRESULT GetRelativePath(LPCSTR strFileName, 
		TCHAR *strOutput, 
		size_t uSize);
};
const char* GetRetPath(const char filePath[]);//add by huangjinshou 2008-7-8 
	