///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : KCrashReporter.H
// Creator  : alex
// Date     : 
// Comment  :
//
///////////////////////////////////////////////////////////////
#ifndef CRASH_RPT_H
#define CRASH_RPT_H

#include <vector>
#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>

class KCrashReporter
{
public:	
    static BOOL CALLBACK 
        DumpRptCallback(PVOID pCrashRpt,
        CONST PMINIDUMP_CALLBACK_INPUT CallbackInput,
        PMINIDUMP_CALLBACK_OUTPUT CallbackOutput);
private:
    struct VideoMemInfo
    {
        int totalMem;
        int freeMem;
    };
    PEXCEPTION_POINTERS m_excpInfo;
    std::vector<MINIDUMP_MODULE_CALLBACK> m_modules;
    TCHAR m_rptFile[MAX_PATH];

    enum  D3DCAPS9_FILED_TYPE
    {
        d3d_dword,
        d3d_float,
        d3d_uint
    };
    
public:
    KCrashReporter(PEXCEPTION_POINTERS pExcp = NULL);
    bool MakeReport(const TCHAR *pVersion, const TCHAR* const rptFile, const TCHAR* const feedback=_T(""));
private:
    void AddModule(MINIDUMP_MODULE_CALLBACK module);
    void WriteExceptionRecord(FILE *f, EXCEPTION_RECORD* pExceptionRecord);
    void WriteOSRecord(FILE *f);
    void WriteModuleRecord(FILE *f);
    void WriteMemoryRecord(FILE *f);
    void WriteCPURecord(FILE *f);
    void WriteVideoRecord(FILE *f);
	void WriteSenderInfo(FILE *f);
	void WriteJx3ClientVersion(FILE *f, const TCHAR* const version);
	void WriteFeedback(FILE *f, const TCHAR* const feedback );
    void WriteD3DCaps9(FILE *f);
    KCrashReporter::VideoMemInfo GetVideoMemInfo();
	tstring GetUILang();
    void WriteD3DCaps9OneNode(tstring node, tstring name, void* val, D3DCAPS9_FILED_TYPE valType, FILE *f);
};

#endif