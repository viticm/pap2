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

KRLRemoteCharacter* KRLRemoteCharacterMgr::Lookup(int nX, int nY, int nZ, int nFaceDirection, DWORD dwModelID)
{
    KPtrMap::iterator it = m_aCharacter.begin();
    KPtrMap::iterator ie = m_aCharacter.end();
    for (; it != ie; ++it)
    {
        KRLRemoteCharacter* pRLRemoteCharacter = (*it).second;

        KRLCharacterFrameData& FrameData = pRLRemoteCharacter->m_RLCharacter.m_FrameData;
        
        if (FrameData.m_Current.nX != nX)
            continue;
        if (FrameData.m_Current.nY != nY)
            continue;
        if (FrameData.m_Current.nZ != nZ)
            continue;
        if (FrameData.m_Current.nFaceDirection != nFaceDirection)
            continue;
        if (pRLRemoteCharacter->m_RLCharacter.m_EquipResource.dwModelID != dwModelID)
            continue;

        return pRLRemoteCharacter;
    }

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
