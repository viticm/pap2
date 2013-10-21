// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DLL1_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DLL1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef KG3DENGINE_DLL_EXPORTS
#define DLLKG3DENGINE_API extern "C" __declspec(dllexport)
#else
#define DLLKG3DENGINE_API extern "C" __declspec(dllimport)
#endif

DLLKG3DENGINE_API HRESULT Get3DEngineInterface(void** pEngineManager);
