////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_Benchmark.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-11-25 10:59:51
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG_BENCHMARK_H_
#define _INCLUDE_KG_BENCHMARK_H_

////////////////////////////////////////////////////////////////////////////////
struct KGBenchmarkOption
{
    BOOL bDisablePreload;
    BOOL bDisablePreloadShader;
    BOOL bPlayAll;
    TCHAR strCudaRecFile[MAX_PATH];
	TCHAR strNormalRecFile[MAX_PATH];
	BOOL bPlayCudaTest;
	BOOL bPlayNoramlTest;

    VOID SetDefault();
    HRESULT LoadFromFile(LPCTSTR strFileName);
};

extern KGBenchmarkOption g_benchMarkOption;
extern HINSTANCE g_hInstance;
extern BOOL g_bNormalMode;

#endif //_INCLUDE_KG_BENCHMARK_H_
