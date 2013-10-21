#include "stdafx.h"
#include "./krlrides.h"
#include "./krlridesmgr.h"

KRLRidesMgr::KRLRidesMgr()
{
}

KRLRidesMgr::~KRLRidesMgr()
{
}

HRESULT KRLRidesMgr::Append(DWORD dwRides, KRLRides* pRLRides)
{
	ASSERT(pRLRides);

	KPtrMap::iterator it = m_aRides.find(dwRides);
	KGLOG_PROCESS_ERROR(it == m_aRides.end());

	m_aRides.insert(std::make_pair(dwRides, pRLRides));
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KRLRidesMgr::Remove(DWORD dwRides)
{
	KPtrMap::iterator it = m_aRides.find(dwRides);
	KGLOG_PROCESS_ERROR(it != m_aRides.end());

	m_aRides.erase(it);

	return S_OK;
Exit0:
	return E_FAIL;
}

KRLRides* KRLRidesMgr::Lookup(DWORD dwRides)
{
	KPtrMap::iterator it = m_aRides.find(dwRides);
	KG_PROCESS_ERROR(it != m_aRides.end());

	return (*it).second;
Exit0:
	return NULL;
}

BOOL KRLRidesMgr::Init()
{
	return TRUE;
}

void KRLRidesMgr::Exit()
{
	for (KPtrMap::iterator it = m_aRides.begin(); it != m_aRides.end();)
	{
		KRLRides* pRides = (*it).second;
		ASSERT(pRides);

		KPtrMap::iterator itRemove = it++;

		SAFE_DELETE(pRides);

		m_aRides.erase(itRemove);
	}
}

BOOL KRLRidesMgr::Reset()
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

BOOL KRLRidesMgr::Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
	BOOL bRetResult = TRUE;
	BOOL bHasFinished = FALSE;

	for (KPtrMap::iterator it = m_aRides.begin(); it != m_aRides.end(); ++it)
	{
		KRLRides* pRides = (*it).second;

        pRides->UpdateRemote(fTime, fTimeLast, dwGameLoop, bFrame);

        if (!bHasFinished && pRides->IsFinished())
        {
			bHasFinished = TRUE;
        }
	}

	if (bHasFinished)
	{
		for (KPtrMap::iterator it = m_aRides.begin(); it != m_aRides.end();)
		{
			KRLRides* pRides = (*it).second;
			if (pRides->IsFinished())
			{
				KPtrMap::iterator itRemove = it++;

				SAFE_DELETE(pRides);

				m_aRides.erase(itRemove);
			}
			else
			{
				++it;
			}
		}
	}

	return bRetResult;
}
