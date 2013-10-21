////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : HeaderAnalysor.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-6-2 14:30:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "HeaderAnalysor.h"
#include "Core.h"
#include <tchar.h>
#include <string.h>
#include <wchar.h>

#ifndef PROCESS_ERROR_EX
#define  PROCESS_ERROR_EX(condition, errorNotice)	{if(!(condition)) {strError = errorNotice; goto Exit0;}}NULL
#endif

struct HeaderAnalysorSolid : public HeaderAnalysor 
{
	virtual BOOL GetHeaderFileNameList(LPCTSTR strFileFullPath, std::vector<CString>* pFileNames, CString* pError)
	{
		enum
		{
			em_file_byte_per_time = MAX_PATH*2,
			em_prev_line_bytes = MAX_PATH,
			em_string_include_length = 8,//字符串#include的长度
		};
		_ASSERTE(NULL != strFileFullPath && NULL != pFileNames);

		char bufferTemp[em_prev_line_bytes + em_file_byte_per_time + 4 * sizeof(DWORD)];	//那个20是折行放字符串"include"用的，后面的是保护位
		memset(bufferTemp, 1, em_prev_line_bytes);
		memset(bufferTemp + em_prev_line_bytes, 0, sizeof(bufferTemp) - em_prev_line_bytes);

		HANDLE hHandle = ::CreateFile(strFileFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING
			, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

		CString strError;
		PROCESS_ERROR_EX(hHandle != INVALID_HANDLE_VALUE, _T("Can not open file"));
		{
			DWORD dwFileSize = ::GetFileSize(hHandle, NULL);
			PROCESS_ERROR_EX(dwFileSize != INVALID_FILE_SIZE && dwFileSize < Core::em_file_max_size, _T("Can not get file size, maybe too large"));

			DWORD dwRemainSize = (dwFileSize % em_file_byte_per_time);
			DWORD dwGetTimes = dwFileSize / em_file_byte_per_time;
			if (dwRemainSize != 0)
			{
				dwGetTimes += 1;
			}

			BOOL bUniCode = FALSE;
			for (DWORD i = 0; i < dwGetTimes; ++i)
			{
				DWORD dwReadBytes = em_file_byte_per_time;
				if (i == dwGetTimes - 1)
				{
					dwReadBytes = dwRemainSize;
				}

				char* pBufferToRead = bufferTemp + em_prev_line_bytes;	//最前面的是上一次读取的
				{
					DWORD dwNumBytesRead = 0;
					BOOL bRet = ::ReadFile(hHandle, pBufferToRead, dwReadBytes, &dwNumBytesRead, NULL);
					PROCESS_ERROR_EX(bRet && dwNumBytesRead == dwReadBytes, _T("Read file error"));
				}

				*(DWORD*)(pBufferToRead + dwReadBytes) = 0;	//最后填上0当字符串读取
				
				if (0 == i)
				{
					bUniCode = ::IsTextUnicode(pBufferToRead, dwReadBytes, NULL);
					PROCESS_ERROR_EX(!bUniCode, _T("Do not accept unicode file"));
				}
				
				CHAR* p = bufferTemp;
				while(p < pBufferToRead + dwReadBytes)
				{
					CString csIncludeFinded;
					p = strstr(p, "#include");
					if (NULL == p)
						break;

					p+=em_string_include_length;

					p = strchr(p, ('\"'));
					if (NULL == p)
						break;

					++p;
					csIncludeFinded = p;
					int nFinded = csIncludeFinded.Find(('\"'));
					if (nFinded<=0)
						break;

					csIncludeFinded.Truncate(nFinded);
					if (0 != csIncludeFinded.Compare(""))
					{
						pFileNames->push_back(csIncludeFinded);

						p += nFinded;
						ZeroMemory(bufferTemp, p - bufferTemp);
					}
				}
				
				//复制这次读取的最后到前面，和下次读取的组成一行
				memcpy_s(bufferTemp, em_prev_line_bytes, bufferTemp + em_file_byte_per_time, em_prev_line_bytes);
			}
		}
		::CloseHandle(hHandle);
		return TRUE;
Exit0:
		if (NULL != pError)
		{
			*pError = strError;
		}
		if(INVALID_HANDLE_VALUE != hHandle)
			::CloseHandle(hHandle);
		return FALSE;
	}
};

HeaderAnalysor* CreateDefHeaderAnalysor()
{
	return new HeaderAnalysorSolid;
}


////////////////////////////////////////////////////////////////////////////////

