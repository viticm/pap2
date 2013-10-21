#include "stdafx.h"
#include "KGPublic.h"
#include "KPrivilege.h"

KPrivilege::KPrivilege()
{
    m_hToken = NULL;
}

KPrivilege::~KPrivilege()
{
    if (m_hToken)
    {
        CloseHandle(m_hToken);
        m_hToken = NULL;
    }
}

int KPrivilege::EnablePrivilege(TCHAR szPrivilege[])
{
    int nResult  = FALSE;
    int nRetCode = FALSE;
    DWORD dwLength = 0;
    TOKEN_PRIVILEGES NewState;

    nRetCode = OpenThreadToken(GetCurrentThread(), 
                            TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, TRUE, 
                            &m_hToken);
    if (!nRetCode)
    {
        if (GetLastError() == ERROR_NO_TOKEN)
        {
            // No impersonation token for the current thread is available. 
            // Let's go for the process token instead.
            nRetCode = OpenProcessToken(GetCurrentProcess(),
                                    TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
                                    &m_hToken);
            KG_PROCESS_ERROR(nRetCode);
        }
    }

    NewState.PrivilegeCount = 1;
    NewState.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    nRetCode = LookupPrivilegeValue(0, szPrivilege, &NewState.Privileges[0].Luid);
    KG_PROCESS_ERROR(nRetCode);
    
    nRetCode = AdjustTokenPrivileges(m_hToken, FALSE, &NewState, 
        sizeof(m_PreviousState), &m_PreviousState, &dwLength);
    KG_PROCESS_ERROR(nRetCode);

    nResult = TRUE;
Exit0:
    if (!nResult)
    {
        if (m_hToken)
        {
            CloseHandle(m_hToken);
            m_hToken = NULL;
        }
    }
    return nResult;
}

int KPrivilege::RestorePrivilege()
{
    if (m_hToken)
        return AdjustTokenPrivileges(m_hToken, FALSE, &m_PreviousState, 0, NULL, NULL);
    return TRUE;
}