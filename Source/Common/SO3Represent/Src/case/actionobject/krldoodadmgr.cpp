#include "stdafx.h"
#include "./krldoodad.h"
#include "./krldoodadmgr.h"

KRLDoodadMgr::KRLDoodadMgr()
{
}

KRLDoodadMgr::~KRLDoodadMgr()
{
}

HRESULT KRLDoodadMgr::Append(DWORD dwDoodad, KRLDoodad* pRLDoodad)
{
	ASSERT(pRLDoodad);

	KPtrMap::iterator it = m_aDoodad.find(dwDoodad);
	KGLOG_PROCESS_ERROR(it == m_aDoodad.end());

	m_aDoodad.insert(std::make_pair(dwDoodad, pRLDoodad));
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KRLDoodadMgr::Remove(DWORD dwDoodad)
{
	KPtrMap::iterator it = m_aDoodad.find(dwDoodad);
	KGLOG_PROCESS_ERROR(it != m_aDoodad.end());

	m_aDoodad.erase(it);

	return S_OK;
Exit0:
	return E_FAIL;
}

KRLDoodad* KRLDoodadMgr::Lookup(DWORD dwDoodad)
{
	KPtrMap::iterator it = m_aDoodad.find(dwDoodad);
	KG_PROCESS_ERROR(it != m_aDoodad.end());

	return (*it).second;
Exit0:
	return NULL;
}

BOOL KRLDoodadMgr::Init()
{
	return TRUE;
}

void KRLDoodadMgr::Exit()
{
	for (KPtrMap::iterator it = m_aDoodad.begin(); it != m_aDoodad.end();)
	{
		KRLDoodad* pDoodad = (*it).second;

		KPtrMap::iterator itRemove = it++;

		SAFE_DELETE(pDoodad);

		m_aDoodad.erase(itRemove);
	}
}

BOOL KRLDoodadMgr::Reset()
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

BOOL KRLDoodadMgr::Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
	BOOL bRetResult = TRUE;
	BOOL bHasFinished = FALSE;

	for (KPtrMap::iterator it = m_aDoodad.begin(); it != m_aDoodad.end(); ++it)
	{
		KRLDoodad* pDoodad = (*it).second;

		pDoodad->Update(fTime, fTimeLast, dwGameLoop, bFrame);

        if (!bHasFinished && pDoodad->IsFinished())
        {
			bHasFinished = TRUE;
        }
	}

	if (bHasFinished)
	{
		for (KPtrMap::iterator it = m_aDoodad.begin(); it != m_aDoodad.end();)
		{
			KRLDoodad* pDoodad = (*it).second;
			if (pDoodad->IsFinished())
			{
				KPtrMap::iterator itRemove = it++;
                
				SAFE_DELETE(pDoodad);

                m_aDoodad.erase(itRemove);
			}
			else
			{
				++it;
			}
		}
	}

	return bRetResult;
}
