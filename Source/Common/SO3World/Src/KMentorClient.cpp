#include "stdafx.h"
#include "KSO3World.h"
#include "KMentorClient.h"
#include "KMentorDef.h"

#ifdef _CLIENT

BOOL KMentorClient::Update(DWORD dwMentor, DWORD dwApprentice, const KMentorRecordBase& crInfo, int* npMentorValueChange)
{
    BOOL  bResult          = false;
    DWORD dwClientPlayerID = g_pSO3World->m_dwClientPlayerID;
    int   nValueChange     = 0;
    
    KGLOG_PROCESS_ERROR(dwClientPlayerID == dwMentor || dwClientPlayerID == dwApprentice);

    if (dwMentor == dwClientPlayerID)
    {
        KApprenticeTable::iterator it = m_ApprenticeTable.find(dwApprentice);
        
        if (it != m_ApprenticeTable.end())
        {
            nValueChange = crInfo.nMentorValue - m_ApprenticeTable[dwApprentice].nMentorValue;
        }

        m_ApprenticeTable[dwApprentice] = crInfo;

    }
    else
    {
        m_MentorTable[dwMentor] = crInfo;
    }

    *npMentorValueChange = nValueChange;
    
    bResult = true;
Exit0:
    return bResult;
}

BOOL KMentorClient::DeleteRecord(DWORD dwMentor, DWORD dwApprentice)
{
    BOOL  bResult          = false;
    DWORD dwClientPlayerID = g_pSO3World->m_dwClientPlayerID;

    KGLOG_PROCESS_ERROR(dwClientPlayerID == dwMentor || dwClientPlayerID == dwApprentice);

    if (dwMentor == dwClientPlayerID)
    {
        m_ApprenticeTable.erase(dwApprentice);
    }
    else
    {
        m_MentorTable.erase(dwMentor);
    }

    bResult = true;
Exit0:
    return bResult;
}

void KMentorClient::Clear()
{
    m_MentorTable.clear();
    m_ApprenticeTable.clear();
}

BOOL KMentorClient::IsMyMentor(DWORD dwPlayerID)
{
    BOOL                    bResult = false;
    KMentorTable::iterator  itM     = m_MentorTable.find(dwPlayerID);
    BYTE                    byState = emrsInvalid;
    
    KG_PROCESS_ERROR(itM != m_MentorTable.end());

    byState = itM->second.byState;

    KG_PROCESS_ERROR(byState == emrsNormal || byState == emrsMBreak || byState == emrsABreak);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMentorClient::IsMyApprentice(DWORD dwPlayerID)
{
    BOOL                    bResult = false;
    KMentorTable::iterator  itA     = m_ApprenticeTable.find(dwPlayerID);
    BYTE                    byState = emrsInvalid;

    KG_PROCESS_ERROR(itA != m_ApprenticeTable.end());

    byState = itA->second.byState;

    KG_PROCESS_ERROR(byState == emrsNormal || byState == emrsMBreak || byState == emrsABreak);

    bResult = true;
Exit0:
    return bResult;
}

BOOL KMentorClient::IsMentor()
{
    for (KApprenticeTable::iterator it = m_ApprenticeTable.begin(); it != m_ApprenticeTable.end(); ++it)
    {
        BYTE byState = it->second.byState;
        
        if (byState == emrsNormal || byState == emrsMBreak || byState == emrsABreak)        
            return true;
    }

    return false;
}

BOOL KMentorClient::IsApprentice()
{
    for (KMentorTable::iterator it = m_MentorTable.begin(); it != m_MentorTable.end(); ++it)
    {
        BYTE byState = it->second.byState;

        if (byState == emrsNormal || byState == emrsMBreak || byState == emrsABreak)        
            return true;
    }

    return false;
}

#endif // _CLIENT

