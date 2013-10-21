#include "stdafx.h"
#include <stdarg.h>
#include <string.h>
#include "KSkillDebug.h"
#include "KPlayerServer.h"
#include "KCharacter.h"
#include "KSO3World.h"
#include "KItemList.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KDoodad.h"

KSkillDebug::KSkillDebug()
{
    m_pOwner = NULL;
    m_nDebugLevel = 0;
}

KSkillDebug::~KSkillDebug()
{
    m_pOwner = NULL;
}

BOOL KSkillDebug::Init(KCharacter* pOwner)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(pOwner);

    m_pOwner = pOwner;
    m_nDebugLevel = 0;
    m_OutprotVector.clear();
    m_OnAttachVector.clear();

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillDebug::UnInit()
{
    BOOL bResult = false;

    DetachAll();
    m_OutprotVector.clear();
    m_OnAttachVector.clear();
    m_pOwner = NULL;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillDebug::Attach(DWORD dwCharacter, int nDebugLevel)
{
    BOOL            bResult     = false;
    BOOL            bRetCode    = false;
    KCharacter*     pDstAttach  = NULL;

    assert(m_pOwner);

    bRetCode = IS_PLAYER(dwCharacter);

    if (bRetCode)
    {
        pDstAttach = g_pSO3World->m_PlayerSet.GetObj(dwCharacter);
    }
    else
    {
        pDstAttach = g_pSO3World->m_NpcSet.GetObj(dwCharacter);
    }
    KGLOG_PROCESS_ERROR(pDstAttach);

    bRetCode = pDstAttach->m_SkillDebug.OnAttach(m_pOwner->m_dwID);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_OutprotVector.push_back(dwCharacter);

    m_nDebugLevel = nDebugLevel;

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillDebug::OnAttach(DWORD dwCharacter)
{
    BOOL bResult = false;

    m_OnAttachVector.push_back(dwCharacter);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillDebug::Detach(DWORD dwCharacter)
{
    BOOL                            bResult     = false;
    BOOL                            bRetCode    = false;
    KCharacter*                     pDstDetach  = NULL;
    std::vector<DWORD>::iterator    it;

    assert(m_pOwner);

    bRetCode = IS_PLAYER(dwCharacter);

    if (bRetCode)
    {
        pDstDetach = g_pSO3World->m_PlayerSet.GetObj(dwCharacter);
    }
    else
    {
        pDstDetach = g_pSO3World->m_NpcSet.GetObj(dwCharacter);
    }
    KG_PROCESS_ERROR(pDstDetach);

    it = find(m_OutprotVector.begin(), m_OutprotVector.end(), dwCharacter);
    KG_PROCESS_ERROR(it != m_OutprotVector.end());

    m_OutprotVector.erase(it);

    bRetCode = pDstDetach->m_SkillDebug.OnDetach(m_pOwner->m_dwID);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillDebug::OnDetach(DWORD dwCharacter)
{
    BOOL                            bResult = false;
    std::vector<DWORD>::iterator    it;

    it = find(m_OnAttachVector.begin(), m_OnAttachVector.end(), dwCharacter);
    KGLOG_PROCESS_ERROR(it != m_OnAttachVector.end());

    m_OnAttachVector.erase(it);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillDebug::DetachAll()
{
    BOOL        bResult     = false;
    BOOL        bRetCode    = false;
    DWORD       dwCharacter = 0;
    KCharacter* pCharacter  = NULL;

    while (!m_OnAttachVector.empty())
    {
        dwCharacter = m_OnAttachVector.back();

        bRetCode = IS_PLAYER(dwCharacter);

        if (bRetCode)
        {
            pCharacter = g_pSO3World->m_PlayerSet.GetObj(dwCharacter);
        }
        else
        {
            pCharacter = g_pSO3World->m_NpcSet.GetObj(dwCharacter);
        }
        KGLOG_PROCESS_ERROR(pCharacter);

        bRetCode = pCharacter->m_SkillDebug.Detach(m_pOwner->m_dwID);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillDebug::Trace(int nDebugLevel, const char* pszFormat, ...)
{
    BOOL                            bResult                             = false;
    BOOL                            bRetCode                            = false;
    va_list                         ArgPtr                              = NULL;
    size_t                          uDataLen                            = 0;
    char                            szOutPutMessage[MAX_CHAT_TEXT_LEN];
    std::vector<DWORD>::iterator    it;
    std::vector<DWORD>::iterator    itEnd;

    KG_PROCESS_SUCCESS(nDebugLevel > m_nDebugLevel);
    KG_PROCESS_ERROR(pszFormat);

    szOutPutMessage[0] = tdbText;

    va_start(ArgPtr, pszFormat);
    vsnprintf(&szOutPutMessage[1], sizeof(szOutPutMessage) - sizeof(char), pszFormat, ArgPtr);
    va_end(ArgPtr);

    uDataLen = strlen(szOutPutMessage) + 1;

#ifdef _SERVER
    for (it = m_OutprotVector.begin(), itEnd = m_OutprotVector.end(); it != itEnd; ++it)
    {
        DWORD dwID = *it;

        g_PlayerServer.DoTalkMessage(
            trLocalSys, ERROR_ID, NULL, dwID, NULL, uDataLen, (BYTE*)szOutPutMessage
        );
    }
#endif

Exit1:
    bResult = true;
Exit0:
    return bResult;
}

BOOL KSkillDebug::SetOwner(KCharacter* pOwner)
{
    BOOL bResult = false;

    KGLOG_PROCESS_ERROR(pOwner);

    m_pOwner = pOwner;

    bResult = true;
Exit0:
    return bResult;
}
