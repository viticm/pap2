////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : DirState.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-8-6 11:45:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DirState.h"

BOOL DirState::LoadDirStateCheckUnchanged( LPCTSTR strDirStateFile, const TypeShaderFileCt& shaderFileCt)
{
	_ASSERTE(NULL != strDirStateFile);

	BOOL bReturn = FALSE;
	std::tifstream fs(strDirStateFile, std::ios_base::in);

	if(! fs)
	{
		std::tcout << _T("dir state file open failed : ") << strDirStateFile << _T("\r\n");
		goto Exit0;
	}

	//fs ;	//不跳过空格
	size_t uFileCount = 0;
	fs >> uFileCount;
	fs.ignore(2);

	if (uFileCount != shaderFileCt.size())
	{
		std::tcout << _T("dir file count changed, rebuilding resource files\r\n"); 
		goto Exit0;
	}

	BOOL bAllUnchanged = TRUE;
	for (TypeShaderFileCt::const_iterator it = shaderFileCt.begin(); it != shaderFileCt.end(); ++it)
	{
		std::tstring strBuffer;
		//fs >> std::noskipws >> strBuffer;
		std::getline(fs, strBuffer, _T('\r'));
		if (! fs)
		{
			goto Exit0;
		}
		
		const TypeShaderFile& shaderFile = *it;
		if (0 != _tcsicmp(strBuffer.c_str(), (LPCTSTR)shaderFile.strFilePath))
		{
			bAllUnchanged = FALSE;
			break;
		}

		fs.ignore(1);	//跳过\n换行符号
	}

	if (! bAllUnchanged)
	{
		goto Exit0;
	}

	bReturn = TRUE;
Exit0:
	return bReturn;
}


BOOL DirState::SaveDirState( LPCTSTR strDirStateFile, const TypeShaderFileCt& shaderFileCt )
{
	_ASSERTE(NULL != strDirStateFile);

	BOOL bReturn = FALSE;
	std::tofstream fs(strDirStateFile, std::ios_base::out | std::ios_base::trunc);

	if(! fs)
	{
		std::tcout << "dir state file open failed : " << strDirStateFile << _T("\r\n");
		goto Exit0;
	}

	fs << shaderFileCt.size() << _T("\r\n");
	for (TypeShaderFileCt::const_iterator it = shaderFileCt.begin(); it != shaderFileCt.end(); ++it)
	{
		const TypeShaderFile& shaderFile = *it;
		fs << (LPCTSTR)shaderFile.strFilePath << _T("\r\n");
	}

	bReturn = TRUE;
Exit0:
	return bReturn;
}