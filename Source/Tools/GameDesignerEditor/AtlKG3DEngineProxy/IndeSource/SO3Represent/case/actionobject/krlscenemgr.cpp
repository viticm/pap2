#include "stdafx.h"
#include "KScene.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "./krlscene.h"
#include "../../SO3Represent.h"

KRLSceneMgr::KRLSceneMgr()
: m_dwClientPlayerRegionInfo(0)
, m_vClientPlayerRegionPosition(0.0f, 0.0f, 0.0f)
{
}

KRLSceneMgr::~KRLSceneMgr()
{
}

HRESULT KRLSceneMgr::Append(DWORD dwScene, KRLScene* pRLScene)
{
	ASSERT(pRLScene);
	KGLOG_PROCESS_ERROR(Lookup(dwScene) == NULL);

	pRLScene->m_dwScene = dwScene;
	m_apScene.push_back(pRLScene);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KRLSceneMgr::Remove(DWORD dwScene)
{
	KRLScenePtrArray::iterator it = m_apScene.begin();
	KRLScenePtrArray::iterator ie = m_apScene.end();

	KGLOG_PROCESS_ERROR(Lookup(dwScene) != NULL);

	for (; it != ie; ++it)
	{
		KRLScene* pRLScene = *it;
		ASSERT(pRLScene);

		if (pRLScene->m_dwScene == dwScene)
		{
			break;
		}
	}

	KGLOG_PROCESS_ERROR(it != m_apScene.end());

	m_apScene.erase(it);

	return S_OK;
Exit0:
	return E_FAIL;
}

KRLScene* KRLSceneMgr::Lookup(DWORD dwScene)
{
	KRLScenePtrArray::iterator it = m_apScene.begin();
	KRLScenePtrArray::iterator ie = m_apScene.end();
	for (; it != ie; ++it)
	{
		KRLScene* pRLScene = *it;
		ASSERT(pRLScene);

		if (pRLScene->m_dwScene == dwScene)
			return pRLScene;
	}
	return NULL;
}

BOOL KRLSceneMgr::Init()
{
	BOOL nRetCode = TRUE;

	KRLScenePtrArray::iterator it = m_apScene.begin();
	KRLScenePtrArray::iterator ie = m_apScene.end();
	for (; it != ie; ++it)
	{
		KRLScene* pRLScene = *it;
		ASSERT(pRLScene);

		if (!pRLScene->Init())
			nRetCode = FALSE;
	}
	return nRetCode;
}

void KRLSceneMgr::Exit()
{
    for (K3DRegionMgrMap::iterator it = m_a3DRegionMgr.begin(); it != m_a3DRegionMgr.end(); ++it)
    {
        IKG3DRegionInfoManager* p3DRegionInfoMgr = it->second;

        SAFE_RELEASE(p3DRegionInfoMgr);
    }

    for (KRLScenePtrArray::iterator it = m_apScene.begin(); it != m_apScene.end(); ++it)
    {
        KRLScene* pRLScene = *it;
        ASSERT(pRLScene);

        pRLScene->Exit();
    }
}

BOOL KRLSceneMgr::Reset()
{
	BOOL nRetCode = TRUE;

	for (KRLScenePtrArray::iterator it = m_apScene.begin(); it != m_apScene.end(); ++it)
	{
		KRLScene* pRLScene = *it;
		ASSERT(pRLScene);

		if (!pRLScene->Reset())
        {
			nRetCode = FALSE;
        }
	}
	return nRetCode;
}

BOOL KRLSceneMgr::Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
	BOOL nRetCode = TRUE;

	for (KRLScenePtrArray::iterator it = m_apScene.begin(); it != m_apScene.end(); ++it)
	{
		KRLScene* pRLScene = *it;
		ASSERT(pRLScene);

		if (!pRLScene->Activate(fTime, fTimeLast, dwGameLoop, bFrame))
        {
			nRetCode = FALSE;
        }
	}

	return nRetCode;
}

DWORD KRLSceneMgr::GetRegionData(DWORD dwScene, D3DXVECTOR3 vPosition)
{
	KScene* pScene = NULL;
	IKGSO3WorldClient* pSO3WorldClient = NULL;
	
	pSO3WorldClient = g_pRL->m_pSO3WorldClient;
    KGLOG_PROCESS_ERROR(pSO3WorldClient);

	pScene = pSO3WorldClient->GetScene(dwScene);
	KGLOG_PROCESS_ERROR(pScene);

    m_vClientPlayerRegionPosition = vPosition;

	return GetRegionDataByMapID(pScene->m_dwMapID, vPosition);
Exit0:
	return 0;
}

DWORD KRLSceneMgr::GetRegionDataByMapID(DWORD dwMapID, D3DXVECTOR3 vPosition)
{
	int nRetCode = false;
	HRESULT hr = E_FAIL;
	DWORD dwData = 0;
	IKG3DRegionInfoManager* p3DRegionInfoMgr = NULL;
	K3DRegionMgrMap::iterator it;
	IKGSO3WorldClient* pSO3WorldClient = NULL;

	pSO3WorldClient = g_pRL->m_pSO3WorldClient;
	KGLOG_PROCESS_ERROR(pSO3WorldClient);
	
	it = m_a3DRegionMgr.find(dwMapID);
	if (it == m_a3DRegionMgr.end())
	{
		TCHAR szFilePath[MAX_PATH];
		KMapParams* pParams = NULL;
		size_t nFilePathLength = 0;
		LPTSTR pszLastPoint = NULL;

		pParams = pSO3WorldClient->GetMapParams(dwMapID);
		KGLOG_PROCESS_ERROR(pParams);

		_tcscpy(szFilePath, pParams->szResourceFilePath);
		pszLastPoint = _tcsrchr(szFilePath, '.');
		if (pszLastPoint)
		{
			*pszLastPoint = _T('\0');
			_tcscat_s(szFilePath, _T("RegionInfo\\RLSplit.bmp"));
			if (g_IsFileExist(szFilePath))
			{
                if (g_pRL->m_p3DEngineManager)
                {
                    g_pRL->m_p3DEngineManager->GetRegionInfoManager(szFilePath, 0, 0, &p3DRegionInfoMgr);
                }
			}

            m_a3DRegionMgr.insert(std::make_pair(dwMapID, p3DRegionInfoMgr));
		}
	}
	else
	{
		p3DRegionInfoMgr = it->second;
	}

	if (p3DRegionInfoMgr)
	{
		hr = p3DRegionInfoMgr->GetRegionInfoData(vPosition, &dwData);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	return dwData;
Exit0:
	return 0;
}
