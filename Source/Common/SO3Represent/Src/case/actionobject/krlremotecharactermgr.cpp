#include "stdafx.h"
#include "./krlremotecharacter.h"
#include "./krlremotecharactermgr.h"

KRLRemoteCharacterMgr::KRLRemoteCharacterMgr()
{
}

KRLRemoteCharacterMgr::~KRLRemoteCharacterMgr()
{
}

HRESULT KRLRemoteCharacterMgr::Append(DWORD dwCharacter, KRLRemoteCharacter* pRLRemoteCharacter)
{
    ASSERT(pRLRemoteCharacter);

    KPtrMap::iterator it = m_aCharacter.find(dwCharacter);
    KGLOG_PROCESS_ERROR(it == m_aCharacter.end());

    m_aCharacter.insert(std::make_pair(dwCharacter, pRLRemoteCharacter));
    return S_OK;
Exit0:
    return E_FAIL;
}

HRESULT KRLRemoteCharacterMgr::Remove(DWORD dwCharacter)
{
    KPtrMap::iterator it = m_aCharacter.find(dwCharacter);
    KGLOG_PROCESS_ERROR(it != m_aCharacter.end());

    m_aCharacter.erase(it);

    return S_OK;
Exit0:
    return E_FAIL;
}

KRLRemoteCharacter* KRLRemoteCharacterMgr::Lookup(DWORD dwCharacter)
{
    KPtrMap::iterator it = m_aCharacter.find(dwCharacter);
    KG_PROCESS_ERROR(it != m_aCharacter.end());

    return (*it).second;
Exit0:
    return NULL;
}

BOOL KRLRemoteCharacterMgr::Init()
{
    return TRUE;
}

void KRLRemoteCharacterMgr::Exit()
{
    for (KPtrMap::iterator it = m_aCharacter.begin(); it != m_aCharacter.end();)
    {
        KRLRemoteCharacter* pRLRemoteCharacter = (*it).second;

        KPtrMap::iterator itRemove = it++;

        SAFE_DELETE(pRLRemoteCharacter);

        m_aCharacter.erase(itRemove);
    }
}

BOOL KRLRemoteCharacterMgr::Reset()
{
	int nRetCode = false;

	Exit();

	nRetCode = Init();
	KGLOG_PROCESS_ERROR(nRetCode);

	return TRUE;
Exit0:
	Exit();
	return FALSE;
}

BOOL KRLRemoteCharacterMgr::Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
    BOOL bRetResult = TRUE;
    BOOL bHasFinished = FALSE;

    for (KPtrMap::iterator it = m_aCharacter.begin(); it != m_aCharacter.end(); ++it)
    {
        KRLRemoteCharacter* pRLRemoteCharacter = (*it).second;

        pRLRemoteCharacter->Update(fTime, fTimeLast, dwGameLoop, bFrame);

        if (!bHasFinished && pRLRemoteCharacter->IsFinished())
            bHasFinished = TRUE;
    }

    if (bHasFinished)
    {
        for (KPtrMap::iterator it = m_aCharacter.begin(); it != m_aCharacter.end();)
        {
            KRLRemoteCharacter* pRLRemoteCharacter = (*it).second;
            if (pRLRemoteCharacter->IsFinished())
            {
                KPtrMap::iterator itRemove = it++;

                SAFE_DELETE(pRLRemoteCharacter);

                m_aCharacter.erase(itRemove);
            }
            else
            {
                ++it;
            }
        }
    }

    return bRetResult;
}
