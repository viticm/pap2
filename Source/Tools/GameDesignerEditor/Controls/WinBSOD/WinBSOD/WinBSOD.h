// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the WINBSOD_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// WINBSOD_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WINBSOD_EXPORTS
#define WINBSOD_API __declspec(dllexport)
#else
#define WINBSOD_API __declspec(dllimport)
#endif


extern "C" WINBSOD_API int WinBSOD(int _type); // _type 是错误类型(0:内存访问错误, 1:除零),  返回值总是 0x00DEAD00;
