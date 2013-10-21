// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DLLMERGETGA_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DLLMERGETGA_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DLLMERGETGA_EXPORTS
#define DLLMERGETGA_API __declspec(dllexport)
#else
#define DLLMERGETGA_API __declspec(dllimport)
#endif

// This class is exported from the DllMergeTga.dll
class DLLMERGETGA_API CDllMergeTga {
public:
	CDllMergeTga(void);
	// TODO: add your methods here.
};

extern DLLMERGETGA_API int g_nInited;

extern "C" DLLMERGETGA_API int TgaToUitex(TCHAR* strFileTga, TCHAR* strOutputFolder);
