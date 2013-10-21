#include "StdAfx.h"
#include "kg3dmodel.h"
#include "KG3DRTSInfoCollector.h"
#include "KG3DBip.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DRTSInfoCollector::KG3DRTSInfoCollector(void)
{
	m_dwSize = 0;
	m_RTSData = NULL;
}

KG3DRTSInfoCollector::~KG3DRTSInfoCollector(void)
{
	m_dwSize = 0;
	SAFE_DELETE_ARRAY(m_RTSData);
}

void KG3DRTSInfoCollector::SaveData(DWORD dwIndex,
									void *pData)
{
	KGLOG_PROCESS_ERROR(dwIndex < m_dwSize);
	m_RTSData[dwIndex] = *(static_cast<RTS*>(pData));
Exit0:
	return;
}

void* KG3DRTSInfoCollector::GetData()
{
	return static_cast<void*>(m_RTSData);
}

DWORD KG3DRTSInfoCollector::GetSize()
{
	return m_dwSize;
}

void KG3DRTSInfoCollector::SetSize(DWORD dwSize)
{
	/*ASSERT(dwSize);
	m_dwSize = dwSize;
		
	RTS **pNewBuffer = new RTS*[dwSize];
	KG_PROCESS_ERROR(pNewBuffer);
	m_dwSize = dwSize;
	SAFE_DELETE_ARRAY(m_RTSData);
	m_RTSData = pNewBuffer;
	memset(m_RTSData, 0, sizeof(RTS*) * dwSize);
Exit0:
	return;*/
}

void* KG3DRTSInfoCollector::GetData(DWORD dwIndex)
{
	return static_cast<void*>(&m_RTSData[dwIndex]);
}

void KG3DRTSInfoCollector::Clear()
{
	memset(m_RTSData, 0, sizeof(RTS) * m_dwSize);
}

HRESULT KG3DRTSInfoCollector::CreateStore(KG3DModel *pModel)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    int nNumBones = 0;
  
	KG_PROCESS_ERROR(pModel);

	hrRetCode = pModel->GetNumBones(&nNumBones);
    KG_COM_PROCESS_ERROR(hrRetCode);
	KG_PROCESS_ERROR(nNumBones > 0);

    ASSERT(!m_RTSData);
	m_RTSData = new RTS[nNumBones];
	KGLOG_PROCESS_ERROR(m_RTSData);

	m_dwSize = static_cast<INT>(nNumBones);

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(m_RTSData);
    }
	return hrResult;
}

HRESULT KG3DRTSInfoCollector::CreateStore(KG3DBip *pBip)
{
    HRESULT hrResult = E_FAIL;
    int nNumBones = 0;

    KG_PROCESS_ERROR(pBip);

	nNumBones = pBip->GetNumBones();
	KG_PROCESS_ERROR(nNumBones > 0);

    ASSERT(!m_RTSData);
	m_RTSData = new RTS[nNumBones];
	KGLOG_PROCESS_ERROR(m_RTSData);

	m_dwSize = static_cast<INT>(nNumBones);

	hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        KG_DELETE_ARRAY(m_RTSData);
    }
	return hrResult;
}

void KG3DRTSInfoCollector::SetData(void* pData, size_t NumData)
{
	KG_PROCESS_ERROR(m_dwSize >= NumData);
	KG_PROCESS_ERROR(pData);

	memcpy(m_RTSData, pData, NumData * sizeof(RTS));

Exit0:
	return;
}