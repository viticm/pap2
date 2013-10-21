////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderInclude.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-1 11:50:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DShaderInclude.h"
#include "DummyInterfaces/KGDummyIFile.h"
#include "KGStringFunctionsBase.h"

////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DShaderInclude::KG3DShaderInclude( LPCTSTR pFileIncludePath )
{
	if(NULL != pFileIncludePath)
	{
		_ASSERTE(NULL == _tcschr(pFileIncludePath, '.') 
			&& _T("包含目录应该是目录而不能是文件名，同时也不支持..格式"));

		size_t SizeOfPathString = _tcslen(pFileIncludePath);
		if(SizeOfPathString < 1)
		{
			m_FileIncludePath = _T("");
			return;
		}

		TCHAR LastChar = pFileIncludePath[SizeOfPathString - 1];
		if (LastChar == '\\' || LastChar == '/')
		{
			m_FileIncludePath = pFileIncludePath;
		}
		else
		{
			m_FileIncludePath = pFileIncludePath;
			m_FileIncludePath += '/';
		}
		for (size_t i = 0; i < m_FileIncludePath.size(); ++i)
		{
			TCHAR& Ref = m_FileIncludePath[i];
			if(Ref == '\\')
				Ref = '/';
		}
	}
	else
		m_FileIncludePath = _T("");
}

HRESULT KG3DShaderInclude::Open(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName
				, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	//IncludeType == D3DXINC_LOCAL || D3DXINC_SYSTEM;
	_ASSERTE(NULL != pFileName && NULL != ppData && NULL != pBytes);
	BYTE* pData = NULL;
	std::tstring FullPath = this->GetFullFilePath(pFileName);
	IFile* pFile = this->GetFilePtr(FullPath.c_str());
	KG_PROCESS_ERROR(NULL != pFile);
	{
		unsigned long Size = pFile->Size();
		pData = new BYTE[Size];
		KG_PROCESS_ERROR(NULL != pData);
		
		unsigned long Ret = pFile->Read(pData, Size);
		KG_PROCESS_ERROR(Ret == Size);
		
		*ppData = pData;
		*pBytes = Size;
	}
	SAFE_RELEASE(pFile);
	return S_OK;
Exit0:
	SAFE_DELETE_ARRAY(pData);
	SAFE_RELEASE(pFile);
	return E_FAIL;
}
HRESULT KG3DShaderInclude::Close(THIS_ LPCVOID pData)
{
	BYTE* pData2 = (BYTE*)pData;
	SAFE_DELETE_ARRAY( pData2 );
	return S_OK;
}

IFile* KG3DShaderInclude::GetFilePtr( LPCTSTR FileName )
{
	_ASSERTE(NULL != FileName);
	KG_CUSTOM_HELPERS::UnicodeConvertor Cvt;
	return g_OpenFile(Cvt.ToA(FileName));
}

std::tstring KG3DShaderInclude::GetFullFilePath( LPCTSTR FileName )
{
	_ASSERTE(NULL != FileName);
	return m_FileIncludePath + FileName;
}
#if defined(DEBUG) | defined(_DEBUG)
KG_TEST_CLASS_TEST_BEGIN(KG3DShaderInclude)
{
	static TCHAR stBuffer[] = _T("This is a nonsence Shader include file");
	struct DummyFile : public KGDummyIFile
	{
		virtual unsigned long Read(void* Buffer, unsigned long ReadBytes)
		{
			return 0 == memcpy_s(Buffer, ReadBytes, stBuffer, sizeof(stBuffer)) ? ReadBytes : 0;
		}
		virtual unsigned long Size(){return sizeof(stBuffer);}
		virtual void	Release(){delete this;}
	};
	struct TestInclude : public KG3DShaderInclude
	{
		TestInclude(LPCTSTR pFileIncludePath):KG3DShaderInclude(pFileIncludePath){}
		virtual IFile* GetFilePtr(LPCTSTR FileName)
		{
			return new DummyFile();
		}
	};

	//测试包含目录的自动字符串链接
	{
		std::tstring IncludePath = _T("Data/public/");
		std::tstring FileName = _T("Shader.fx");
		std::tstring FullPath = _T("Data/public/Shader.fx");
		{
			TestInclude aInclude(IncludePath.c_str());
			_ASSERTE(aInclude.GetFullFilePath(FileName.c_str()) == FullPath);
		}
		{
			IncludePath.at(IncludePath.length()-1) = '\\';
			TestInclude aInclude(IncludePath.c_str());
			_ASSERTE(aInclude.GetFullFilePath(FileName.c_str()) == FullPath);
		}
		{
			IncludePath.at(IncludePath.length()-1) = '\0';
			TestInclude aInclude(IncludePath.c_str());
			_ASSERTE(aInclude.GetFullFilePath(FileName.c_str()) == FullPath);
		}
		{
			TestInclude aInclude(_T(""));
			_ASSERTE(aInclude.GetFullFilePath(FileName.c_str()) == FileName);
		}
		{
			TestInclude aInclude(NULL);
			_ASSERTE(aInclude.GetFullFilePath(FileName.c_str()) == FileName);
		}
	}
	//测试Open和Close
	{
		TestInclude aInclude(NULL);
		LPCTSTR	pNewBuffer;
		UINT	BufferSize;
		HRESULT hr = aInclude.Open(D3DXINC_FORCE_DWORD, _T("Shader.fx"), NULL, (LPCVOID*)&pNewBuffer, &BufferSize);
		_ASSERTE(SUCCEEDED(hr) 
			&& BufferSize == sizeof(stBuffer)
			&& _tcscmp(pNewBuffer, stBuffer) == 0);
		hr = aInclude.Close(pNewBuffer);
		_ASSERTE(SUCCEEDED(hr));
	}
	
}
KG_TEST_CLASS_TEST_END(KG3DShaderInclude)
#endif