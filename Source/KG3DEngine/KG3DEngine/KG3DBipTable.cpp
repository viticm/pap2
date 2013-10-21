#include "StdAfx.h"
#include "KG3DBipTable.h"
#include "kg3dbip.h"

KG3DBipTable g_cBipTable;

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DBipTable::KG3DBipTable(void)
{
}

KG3DBipTable::~KG3DBipTable(void)
{
	std::map<DWORD, KG3DBip*>::iterator it = m_BipTable.begin();
	std::map<DWORD, KG3DBip*>::iterator itEnd = m_BipTable.end();
	while (it != itEnd)
	{
		SAFE_DELETE(it->second);
		it++;
	}
	m_BipTable.clear();
}

KG3DBip* KG3DBipTable::GetBip(LPCSTR strFileName)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    KG3DBip* pBip = NULL;
    DWORD dwID = static_cast<DWORD>(INVALIDATE_ID);
    BipTableIterator It;

    m_Lock.Lock();

    KG_PROCESS_ERROR(strFileName);

    dwID = static_cast<DWORD>(g_FileNameHash(strFileName));
    KG_PROCESS_ERROR(dwID != INVALIDATE_ID);

    It = m_BipTable.find(dwID);
    if (It != m_BipTable.end())
    {
        pBip = It->second;
        pBip->AddRef();
    }
    else
    {
        pBip = new KG3DBip;
        KGLOG_PROCESS_ERROR(pBip);

        pBip->m_dwID = dwID;
        hrRetCode = pBip->LoadFromTextFile(strFileName);
        KG_COM_PROCESS_ERROR(hrRetCode);

        m_BipTable[dwID] = pBip;
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        SAFE_DELETE(pBip);
    }
    m_Lock.Unlock();
    return pBip;
}

void KG3DBipTable::ReleaseBip(DWORD dwID)
{
	m_Lock.Lock();

	int nRef = 0;
	BipTableIterator it = m_BipTable.find(dwID);
	KG_PROCESS_ERROR(it != m_BipTable.end());

	KG_PROCESS_ERROR(it->second);
	
	nRef = it->second->Release();
	if (nRef <= 0)
	{
		m_BipTable.erase(it);
	}
Exit0:
	m_Lock.Unlock();
	;
}