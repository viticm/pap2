#ifndef UPDATE_EXTRACTOR_H
#define UPDATE_EXTRACTOR_H

#include <vector>
using std::vector;

#ifdef UPDATEEXTRACTOR_EXPORTS
	#define DLLDECL __declspec(dllexport)
#else
	#define DLLDECL __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void* HExtract;

//初始化,返回解压缩的一个HANDLE
//packagePath:	压缩包路径
//outputPath:	解压缩路径
DLLDECL HExtract UpdateExtractor_InitW(wchar_t const *packagePath, wchar_t const *outputPath);
DLLDECL HExtract UpdateExtractor_InitA(char const *packagePath, char const *outputPath);

#if defined(UNICODE) || defined(_UNICODE)
#define UpdateExtractor_Init		UpdateExtractor_InitW
#else
#define UpdateExtractor_Init		UpdateExtractor_InitA
#endif

//解压缩下一个文件
DLLDECL int UpdateExtractor_ExtractNext(HExtract he);

//解压单独一个文件
DLLDECL int UpdateExtractor_ExtractSingleFile(HExtract he, const TCHAR* szFileName);
//释放资源
//如果InitExtractInfo失败，不必调用UninitExtractInfo
DLLDECL void UpdateExtractor_Uninit(HExtract he);
//获取压缩包内文件总数，是一个大概值。实际应以ExtractNext的返回值判断是否解压完成。
DLLDECL int UpdateExtractor_GetFileCount(HExtract he);
//获取当前正在解压缩的文件名
DLLDECL char const *UpdateExtractor_GetCurrentFileA(HExtract he);
DLLDECL wchar_t const *UpdateExtractor_GetCurrentFileW(HExtract he);

#if defined(UNICODE) || defined(_UNICODE)
#define UpdateExtractor_GetCurrentFile	UpdateExtractor_GetCurrentFileW
#else
#define UpdateExtractor_GetCurrentFile	UpdateExtractor_GetCurrentFileA
#endif

#ifdef __cplusplus
}
#endif

#endif