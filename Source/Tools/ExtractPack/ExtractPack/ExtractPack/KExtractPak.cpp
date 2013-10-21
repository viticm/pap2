#include "StdAfx.h"
#include "KExtractPak.h"
#include "KGLog.h"
#include "updatepublic.h"
#include "KPackFileManager.h"



KExtractPak::KExtractPak(void)
{
}


KExtractPak::~KExtractPak(void)
{
}

bool KExtractPak::Init(const char cszPakFile[], const char cszExtracPath[])
{

	assert(cszPakFile);
	assert(cszExtracPath);

	memcpy(m_szPakFile, cszPakFile, sizeof(m_szPakFile));
	memcpy(m_szExtractPath, cszExtracPath, sizeof(m_szExtractPath));

	return true;
}

bool KExtractPak::Run()
{
	int           nResult     = false;
	//t_pack_header stdHeader;
	//DWORD         offset      = 0;
	//t_index_info* stdIndex    = NULL;
	////打开资源源文件
	//FILE*         stdFile     = NULL;
	//char*         temp_buffer = NULL;
	//LPSTR         DataBuffer  = NULL;
	//XPackIndexInfo *pPakIndex = NULL;

	//temp_buffer = new CHAR[MAX_FILE * sizeof(t_index_info)];	
	//KGLOG_PROCESS_ERROR(temp_buffer);
	//ZeroMemory(temp_buffer, MAX_FILE * sizeof(t_index_info));

	//stdFile = fopen(m_szPakFile, "r+b");
	//KGLOG_PROCESS_ERROR(stdFile);
	//
	//GetHeader(&stdHeader, stdFile); //得到目标PAK的文件个数

	//pPakIndex = new XPackIndexInfo[stdHeader.count];

	//memset(pPakIndex, 0, stdHeader.count * sizeof(XPackIndexInfo));
	
	//fseek();

	nResult = ExtractPak();


Exit0:
	/*if (stdFile)
	{
		fclose(stdFile);
		stdFile = NULL;
	}*/
	return true;
}

bool KExtractPak::ExtractPak()
{
	int nResult = false;
	int nRetCode = false;
	int nPackIndex = 0;
	int nUnpackCount = 0;
	IPackFileShell *pPackFileShell = NULL;

	pPackFileShell = CreatePackFileShell();
	KGLOG_PROCESS_ERROR(pPackFileShell);

	nPackIndex = pPackFileShell->CreatePack(m_szPakFile, true, false);
	KGLOG_PROCESS_ERROR(nPackIndex);

	nResult = pPackFileShell->UnpackAll(nPackIndex, nUnpackCount, m_szExtractPath);
	KGLOG_PROCESS_ERROR(nResult);


	nRetCode = false;
Exit0:
	if (pPackFileShell && nPackIndex > 0)
	{
		pPackFileShell->ClosePack(nPackIndex);
		pPackFileShell->Release();
	}
	return nRetCode;
}