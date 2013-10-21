// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PSDREAD_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PSDREAD_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PSDREAD_EXPORTS
#define PSDREAD_API __declspec(dllexport)
#else
#define PSDREAD_API __declspec(dllimport)
#endif

EXTERN_C PSDREAD_API BOOL ReadPsdInfo(LPCTSTR fileName, int* layerCount, int* layersPosBuf);
