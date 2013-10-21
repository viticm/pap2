#include "StdAfx.h"
#include "./KSO3Client.h"
#include "./KJX3Sdoa.h"
#include "SO3UI/ISO3UI.h"
#include "SO3Represent/SO3RepresentInterface.h"

//#define SO3CLIENT_PROFILE_MSG_PROC

LRESULT	KSO3Client::OnMsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int nRetCode = false;
    LRESULT lResult = 0;
    IKSO3RepresentHandler* pRepresentHandler = NULL;

#ifdef SO3CLIENT_PROFILE_MSG_PROC
    static int s_nCount = 0;
    static DWORD s_dwTotalTickCount = 0;

    DWORD dwTickCount = KG_GetTickCount();
#endif
    
    if (m_JX3Sdoa.pSDOADx9)
    {
        nRetCode = HandleJX3SdoaMsgProc(&m_JX3Sdoa, GetHWND(), uMsg, wParam, lParam, &lResult);
        KG_PROCESS_SUCCESS(nRetCode);
    }

    if (m_pRepresent)
        pRepresentHandler = m_pRepresent->GetRepresentHandelr();
    
    if (pRepresentHandler)
        pRepresentHandler->MsgProc(uMsg, wParam, lParam);

    if (m_pUI)
        lResult = m_pUI->WndProc(uMsg, wParam, lParam);

#ifdef SO3CLIENT_PROFILE_MSG_PROC
    s_dwTotalTickCount += KG_GetTickCount() - dwTickCount;

    ++s_nCount;

    if ((s_nCount & 0xF) == 0)
    {
        printf("[Client] MsgProc %d %d %f\n", s_dwTotalTickCount, s_nCount, (double)s_dwTotalTickCount / s_nCount);
    }
#endif

Exit1:
    return lResult;
}

