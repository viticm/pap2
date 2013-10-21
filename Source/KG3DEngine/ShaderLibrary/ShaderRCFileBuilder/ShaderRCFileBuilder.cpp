// ShaderRCFileBuilder.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "resource.h"
#include "DirState.h"
#include "CmdLineParser.h"

#if defined(UNICODE)	|| defined(_UNICODE)
#error This project can not be compiled as unicode
#endif


//typedef std::list<TypeShaderFile> TypeShaderFileCt;
TypeShaderFileCt g_listShaderFiles;

enum
{
	em_min_id = 113,
};
HGLOBAL	GetFileTemplate(UINT uIDofTemplate, LPCVOID* ppData, size_t* puSize, std::vector<size_t>& vecInsertPoints);
BOOL BuildRCFile(LPCTSTR strResourceH, LPCTSTR strShadersRC, LPCTSTR strShaderID_H);
HGLOBAL MyLoadResourceA( HMODULE hModule, LPCSTR strName, LPCSTR strType,LPCVOID* ppData, DWORD* pSize );
BOOL IsFileExit(LPCTSTR strFilePath)
{
	WIN32_FIND_DATA fd;
	HANDLE hHandle = ::FindFirstFile(strFilePath, &fd);
	if (INVALID_HANDLE_VALUE != hHandle)
	{
		::FindClose(hHandle);
		return TRUE;
	}
	return FALSE;
}
int MainFunc(int argc, _TCHAR* argv[])
{
	LPCTSTR strCommandLine = ::GetCommandLine();
	g_listShaderFiles.clear();

	_ASSERTE(NULL != strCommandLine);

	CCmdLineParser cmdlineParser(strCommandLine);

	enum
	{
		em_dir = 0,
		em_h = 1,
		em_rc = 2,
		em_id = 3,
		em_out = 4,
	};
	CString cmdLineParams[5];
	LPCTSTR cmdKeys[] = {
		_T("Dir"),
		_T("H"),
		_T("RC"),
		_T("ID"),
		_T("Out"),
	};

	for(size_t i = 0; i < _countof(cmdLineParams); ++i)
	{
		bool bValue = cmdlineParser.GetVal(cmdKeys[i], cmdLineParams[i]);
		if(! bValue)
		{
			std::tcout << _T("error arg, exit\r\n");
			return 1;
		}
	}
		
	//分析出查找的路径
	{
		CString& strDir = cmdLineParams[em_dir];
		TCHAR lastChar = strDir.GetAt(strDir.GetLength());
		if ((lastChar != _T('/') && lastChar != _T('\\')))
		{
			strDir += _T("/");
		}
	}
	
	
	//遍历查找的所有路径放到一个容器
	WIN32_FIND_DATA	findData;
	HANDLE hFind = NULL;
	
	{
		CString strFirstSearchFile = cmdLineParams[em_dir] + _T("*.*");
		hFind = ::FindFirstFile(strFirstSearchFile, &findData);
		if(INVALID_HANDLE_VALUE == hFind)
		{
			std::tcout << _T("dir error, exit.\r\n");
			return 1;
		}
	}

	size_t uFindCount = 0;
	while(::FindNextFile(hFind, &findData) && uFindCount < 1000)
	{
		++uFindCount;

		CString strFileName = findData.cFileName;
		strFileName.MakeLower();
		{
			LPCTSTR pExt = _tcschr(findData.cFileName, _T('.'));
			if (NULL == pExt)
				continue;

			if (NULL != _tcsicmp(pExt, _T(".fx")) && NULL != _tcsicmp(pExt, _T(".h")))
			{
				continue;
			}
		}

		//进行一些简单的处理，处理出宏
		TypeShaderFile shaderFile;
		shaderFile.strFilePath = cmdLineParams[em_dir];
		shaderFile.strFilePath += strFileName;

		shaderFile.strFileName = strFileName;

		shaderFile.strFileMacro.Format(_T("SHADER_RC_%s"), strFileName);
		shaderFile.strFileMacro.Truncate(shaderFile.strFileMacro.Find(_T('.'), 0));	//去掉后缀
		shaderFile.strFileMacro.MakeUpper();
		shaderFile.strFileMacro.Replace(_T(' '), _T('_'));	//不能有空格
		if (shaderFile.strFileMacro.GetLength() < 1)
		{
			continue;
		}

		g_listShaderFiles.push_back(shaderFile);
	}

	::FindClose(hFind);

	if (0 == g_listShaderFiles.size())
	{
		std::tcout << _T("no file, exit.\r\n");
		return 1;
	}

	BOOL IsDirStateUnChanged = DirState::LoadDirStateCheckUnchanged(cmdLineParams[em_out], g_listShaderFiles);

	if(IsDirStateUnChanged)
	{
		if (IsFileExit(cmdLineParams[em_h]) && IsFileExit(cmdLineParams[em_rc]))
		{
			std::tcout << _T("dir structure remain the same, skipped resource file build\r\n");
			return 0;
		}
	}
	
	DirState::SaveDirState(cmdLineParams[em_out], g_listShaderFiles);

	BOOL bRet = ::BuildRCFile(cmdLineParams[em_h], cmdLineParams[em_rc], cmdLineParams[em_id]);
	if (! bRet)
	{
		std::tcout << _T("create rc file failed, exit.\r\n");
		return 1;
	}
	return 0;
}
int _tmain(int argc, _TCHAR* argv[])
{
	__try
	{
		return MainFunc(argc, argv);
	}	
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		
	}
	
	std::tcerr << _T("exception caught, app abort\r\n");
	return 1;
}
BOOL MyWriteFile(LPCVOID pData, size_t uSize, FILE* pFile, LPCTSTR strFilePath)
{
	size_t uWrited = fwrite(pData, uSize, 1, pFile);
	if (uWrited != 1)
	{
		std::tcout << _T("file write error:") << strFilePath << _T("\r\n");
		return FALSE;
	}
	return TRUE;
}

BOOL BuildRCFile(LPCTSTR strResourceH, LPCTSTR strShadersRC, LPCTSTR strShaderID_H)
{
	_ASSERTE(NULL != strResourceH && NULL != strShadersRC && NULL != strShaderID_H);
	
	FILE* pResourceHFile = NULL;
	FILE* pRCFile = NULL;
	FILE* pShaderIDFile = NULL;
	HGLOBAL hglTmp = NULL;
	BOOL bReturn = FALSE;
	//1.生成resource.h
	do
	{
		errno_t errt = _tfopen_s(&pResourceHFile, strResourceH, _T("wb"));
		if(0 != errt)
		{
			std::tcout << _T("file open incorrect:") << strResourceH << ("\r\n");
			goto Exit0;
		}

		TCHAR ResourceHeader[] = _T("//{{NO_DEPENDENCIES}}\r\n\
// Microsoft Visual C++ generated include file.\r\n\
// Used by Shaders.rc\r\n\
//\r\n\r\n\r\n");

		//- sizeof(TCHAR)是因为最后有个\0，不要写到文件里面。下同
		if(! MyWriteFile(ResourceHeader, sizeof(ResourceHeader) - sizeof(TCHAR), pResourceHFile, strResourceH))
			goto Exit0;

		size_t uMinID = em_min_id;

		for (TypeShaderFileCt::iterator it = g_listShaderFiles.begin()
			; it != g_listShaderFiles.end(); ++it, ++uMinID)
		{
			TypeShaderFile& shaderFile = *it;
			CString strDefine;

			strDefine.Format(_T("#define %s\t%u\r\n"), (LPCTSTR)shaderFile.strFileMacro, uMinID);

			if(! MyWriteFile((LPCTSTR)strDefine, strDefine.GetLength() * sizeof(TCHAR), pResourceHFile, strResourceH))
				goto Exit0;

		}

		fclose(pResourceHFile);
	}while(false);

	//2.利用模板生成rc文件
	{
		std::vector<size_t> vecInsertPoints;
		LPCVOID pData = NULL;
		size_t uSize = 0;
		hglTmp = GetFileTemplate(IDR_SHADR_RC, &pData, &uSize, vecInsertPoints);
		if(NULL == hglTmp)
			goto Exit0;

		_ASSERTE(3 == vecInsertPoints.size());
		if (3 != vecInsertPoints.size())
		{
			std::tcout << _T("file template incorrect: ") << strShadersRC << _T("\r\n");
			goto Exit0;
		}
		errno_t errt = _tfopen_s(&pRCFile, strShadersRC, _T("wb"));
		if(0 != errt)
		{
			std::tcout << _T("file open incorrect:") << strShadersRC << ("\r\n");
			return FALSE;
		}
		
		if(! MyWriteFile(pData, vecInsertPoints[0], pRCFile, strShadersRC))
			goto Exit0;

		//写入include
		{
			CString csTmp;
			csTmp.Format(_T("\r\n#include\"%s\"\r\n"), strResourceH);
			if(! MyWriteFile((LPCTSTR)csTmp, csTmp.GetLength() * sizeof(TCHAR), pRCFile, strShadersRC))
				goto Exit0;
		}

		_ASSERTE(vecInsertPoints[1] > vecInsertPoints[0]);
		if(! MyWriteFile((BYTE*)pData + vecInsertPoints[0], vecInsertPoints[1] - vecInsertPoints[0], pRCFile, strShadersRC))
			goto Exit0;
		
		//写入rc定义
		size_t uMacroWriteCount = 0;
		for (TypeShaderFileCt::iterator it = g_listShaderFiles.begin()
			; it != g_listShaderFiles.end(); ++it)
		{
			TypeShaderFile& shaderFile = *it;
			CString strDefine;

			strDefine.Format(_T("%s\tSHADER\t\"%s\"\r\n"), (LPCTSTR)shaderFile.strFileMacro, (LPCTSTR)shaderFile.strFilePath);

			if(! MyWriteFile((LPCTSTR)strDefine, strDefine.GetLength() * sizeof(TCHAR), pRCFile, strShadersRC))
				goto Exit0;

			++uMacroWriteCount;
		}
		
		if(! MyWriteFile((BYTE*)pData + vecInsertPoints[1], vecInsertPoints[2] - vecInsertPoints[1], pRCFile, strShadersRC))
			goto Exit0;

		//再次写入include
		{
			CString csTmp;
			csTmp.Format(_T("\t\"%s\\0\""), strResourceH);
			if(! MyWriteFile((LPCTSTR)csTmp, csTmp.GetLength() * sizeof(TCHAR), pRCFile, strShadersRC))
				goto Exit0;
		}

		if(! MyWriteFile((BYTE*)pData + vecInsertPoints[2], uSize - vecInsertPoints[2], pRCFile, strShadersRC))
			goto Exit0;

		fclose(pRCFile);
		::FreeResource(hglTmp);//注意，这个系统有进行管理的，Free还是不Free都不是大问题
	}

	//利用模板生成ShaderID.h
	{
		std::vector<size_t> vecInsertPoints;
		LPCVOID pData = NULL;
		size_t uSize = 0;
		hglTmp = GetFileTemplate(IDR_SHADR_ID_H, &pData, &uSize, vecInsertPoints);
		if(NULL == hglTmp)
			goto Exit0;

		_ASSERTE(2 == vecInsertPoints.size());
		if (2 != vecInsertPoints.size())
		{
			std::tcout << _T("file template incorrect: ") << strShaderID_H << _T("\r\n");
			goto Exit0;
		}
		errno_t errt = _tfopen_s(&pShaderIDFile, strShaderID_H, _T("wb"));
		if(0 != errt)
		{
			std::tcout << _T("file open incorrect:") << strShaderID_H << ("\r\n");
			return FALSE;
		}

		if(! MyWriteFile(pData, vecInsertPoints[0], pShaderIDFile, strShaderID_H))
			goto Exit0;

		//再次写入文件ID声明，这个不会和resource.h的冲突
		{
			size_t uMinID = em_min_id;

			CString csTmp;
			csTmp.Format("\r\n#define SHADER_RC_MIN_VALUE %u\r\n\r\n\r\n", em_min_id);
			MyWriteFile((LPCTSTR)csTmp, csTmp.GetLength() * sizeof(TCHAR), pShaderIDFile, strShaderID_H);

			for (TypeShaderFileCt::iterator it = g_listShaderFiles.begin()
				; it != g_listShaderFiles.end(); ++it, ++uMinID)
			{
				TypeShaderFile& shaderFile = *it;
				CString strDefine;

				strDefine.Format(_T("#define %s\t%u\r\n"), (LPCTSTR)shaderFile.strFileMacro, uMinID);

				if(! MyWriteFile((LPCTSTR)strDefine, strDefine.GetLength() * sizeof(TCHAR), pShaderIDFile, strShaderID_H))
					goto Exit0;

			}
		}

		_ASSERTE(vecInsertPoints[1] > vecInsertPoints[0]);
		if(! MyWriteFile((BYTE*)pData + vecInsertPoints[0], vecInsertPoints[1] - vecInsertPoints[0], pShaderIDFile, strShaderID_H))
			goto Exit0;

		size_t uMacroWriteCount = 0;
		for (TypeShaderFileCt::iterator it = g_listShaderFiles.begin()
			; it != g_listShaderFiles.end(); ++it)
		{
			TypeShaderFile& shaderFile = *it;
			CString strDefine;

			strDefine.Format(_T("{%s\t,\t\"%s\"},\r\n"), (LPCTSTR)shaderFile.strFileMacro, (LPCTSTR)shaderFile.strFilePath);

			if(! MyWriteFile((LPCTSTR)strDefine, strDefine.GetLength() * sizeof(TCHAR), pShaderIDFile, strShaderID_H))
				goto Exit0;

			++uMacroWriteCount;
		}

		if(! MyWriteFile((BYTE*)pData + vecInsertPoints[1], uSize - vecInsertPoints[1], pShaderIDFile, strShaderID_H))
			goto Exit0;

		fclose(pShaderIDFile);
		::FreeResource(hglTmp);//注意，这个系统有进行管理的，Free还是不Free都不是大问题
	}
	
	bReturn = TRUE;
Exit0:
	if(NULL != pResourceHFile)
		::fclose(pResourceHFile);
	if(NULL != pRCFile)
		::fclose(pRCFile);
	if(NULL != pShaderIDFile)
		::fclose(pShaderIDFile);

	if(NULL != hglTmp)
		::FreeResource(hglTmp);
	if (! bReturn)
	{
		std::tcout << _T("error create files: \r\n ");
	}
	return bReturn;
	
}

HGLOBAL MyLoadResourceA( HMODULE hModule, LPCSTR strName, LPCSTR strType,LPCVOID* ppData, DWORD* pSize )
{
	HGLOBAL hglobalTmp = NULL;
	__try						 
	{
		HRSRC hrsrcTmp = ::FindResourceA(hModule, strName, strType);
		if(NULL == hrsrcTmp)
			return NULL;

		DWORD dwSize = ::SizeofResource(hModule, hrsrcTmp);
		if(0 == dwSize)
			return NULL;

		hglobalTmp = ::LoadResource(hModule, hrsrcTmp);
		if(NULL == hglobalTmp)
			return NULL;

		LPVOID lpMemory = ::LockResource(hglobalTmp);
		if(NULL == lpMemory)
		{
			::FreeResource(hglobalTmp);
			hglobalTmp = NULL;
			return NULL;
		}

		if(NULL != ppData)
			*ppData = lpMemory;
		if(NULL != pSize)
			*pSize = dwSize;

		return hglobalTmp;
	}									 
	__except(EXCEPTION_EXECUTE_HANDLER)	 
	{									 

	}

	if(NULL != hglobalTmp)
		FreeResource(hglobalTmp);
	return NULL;
}

HGLOBAL	GetFileTemplate(UINT uIDofTemplate, LPCVOID* ppData, size_t* puSize, std::vector<size_t>& vecInsertPoints)
{
	_ASSERTE(NULL != ppData && NULL != puSize);
	BOOL bAllSucceeded = FALSE;
	BYTE* pMemoryResource  = NULL;
	
	vecInsertPoints.clear();

	LPCVOID pData = NULL;
	DWORD	uSize = 0;
	HGLOBAL hgTmp = MyLoadResourceA(NULL, MAKEINTRESOURCE(uIDofTemplate), _T("TEXT_TMP"), &pData, &uSize);
	_ASSERTE(NULL != hgTmp);
	if (NULL == hgTmp)
	{
		std::tcout << _T("resource not correct:") << uIDofTemplate << _T("\r\n");
		goto Exit0;
	}

	if(uSize > 1024 *1024)
	{
		std::tcout << _T("resource too large") << _T("\r\n");
		goto Exit0;
	}

	//复制到可以修改的内存中做字符串分析
	pMemoryResource = (BYTE*)::malloc(uSize);
	if (NULL == pMemoryResource)
	{
		std::tcout << _T("melloc resource memory failed\r\n"); 
		goto Exit0;
	}
	
	errno_t errt = memcpy_s(pMemoryResource, uSize, pData, uSize);
	if (0 != errt)
	{
		std::tcout << _T("copy resource failed\r\n");
		goto Exit0;
	}
	
	TCHAR strInsertPoint[] = _T("//<InsertPoint>");
	size_t uSizeOfStrInsertPoint = (_countof(strInsertPoint) - 1) * sizeof(TCHAR);

	enum
	{
		em_search_point_movement = sizeof(TCHAR) * MAX_PATH,
	};

	//因为可能上一个循环找不到某字符串，而字符串又有部分在某次循环检索的字符串后面，所以要卷着查找
	_ASSERTE(em_search_point_movement > uSizeOfStrInsertPoint);
	size_t uSearchLengthPerTime = em_search_point_movement + uSizeOfStrInsertPoint;
	BYTE* pMemoryEnd = pMemoryResource + uSize;

	for (size_t i = 0; i < uSize / em_search_point_movement; ++i)		//这里假定了InsertPoint不在最头也不在最尾，所以可以忽略边界,同时假定每行只有一个InsertPoint
	{
		BYTE* pSearch = pMemoryResource + i * em_search_point_movement;
		
		if(i != 0)
			pSearch -= uSizeOfStrInsertPoint;//每次向回滚一点
		_ASSERTE(pSearch <= pMemoryEnd);

		BYTE* pSearchEnd = pMemoryResource + (i+1) * em_search_point_movement;
		_ASSERTE(pSearchEnd <= pMemoryEnd);

		TCHAR uOldChar = *(TCHAR*)pSearchEnd;	//把字符替换成字符串结尾，以便查找
		*(TCHAR*)pSearchEnd = _T('\0');

		LPCTSTR pFound = _tcsstr((TCHAR*)pSearch, strInsertPoint);
		while(pFound)
		{
			size_t uPosition = (BYTE*)pFound - pMemoryResource;
			vecInsertPoints.push_back(uPosition);
			pSearch = (BYTE*)pFound + uSizeOfStrInsertPoint;
			pFound = _tcsstr((TCHAR*)pSearch, strInsertPoint);
		}

		*(TCHAR*)pSearchEnd = uOldChar;
	}

	*ppData = pData;
	*puSize = uSize;
	bAllSucceeded = TRUE;
Exit0:
	if(! bAllSucceeded)
	{
		std::tcout << _T("GetResourceFailed") << uIDofTemplate << _T("\r\n");
		if(NULL != hgTmp)
		{
			::FreeResource(hgTmp);
			hgTmp = NULL;
		}
	}
	if(NULL != pMemoryResource)
	{
		::free(pMemoryResource);
		pMemoryResource = NULL;
	}
	return hgTmp;
}
