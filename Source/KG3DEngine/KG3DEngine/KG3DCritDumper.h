#ifndef MINIDUMPER_H
#define MINIDUMPER_H

#include <windows.h>

class KG3DCritDumper
{
public:
	// constructors / destructors
    KG3DCritDumper( bool bPromptUserForMiniDump );
    ~KG3DCritDumper( );


private:
    static LONG WINAPI unhandledExceptionHandler(struct _EXCEPTION_POINTERS *pExceptionInfo);
    void setMiniDumpFileName(void);
    bool getImpersonationToken(HANDLE* phToken);
    BOOL enablePrivilege(LPCTSTR pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
    BOOL restorePrivilege(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
    LONG writeMiniDump(_EXCEPTION_POINTERS *pExceptionInfo );

    _EXCEPTION_POINTERS *m_pExceptionInfo;
    TCHAR m_szMiniDumpPath[MAX_PATH];
    TCHAR m_szAppPath[MAX_PATH];
    TCHAR m_szAppBaseName[MAX_PATH];
    bool m_bPromptUserForMiniDump;

    static KG3DCritDumper* s_pMiniDumper;
    static LPCRITICAL_SECTION s_pCriticalSection;
};


#endif // MINIDUMPER_H
