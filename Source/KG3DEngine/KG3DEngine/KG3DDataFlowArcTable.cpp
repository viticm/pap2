#include "StdAfx.h"
#include "KG3DDataFlowArcTable.h"
#include "KG3DDataFlowArc.h"

KG3DDataFlowArcTable::KG3DDataFlowArcTable(void)
{
	m_uCurrentHandle = 1;
}

KG3DDataFlowArcTable::~KG3DDataFlowArcTable(void)
{
	UnInit();
}

HRESULT KG3DDataFlowArcTable::UnInit()
{
	map<UINT,KG3DDataFlowArc*>::iterator i = m_mapArc.begin();
	map<UINT,KG3DDataFlowArc*>::iterator iend = m_mapArc.end();
	while (i!=iend)
	{
		KG3DDataFlowArc* pArc = i->second;
		SAFE_DELETE(pArc);
		i++;
	}
	m_mapArc.clear();

	return S_OK;
}

HRESULT KG3DDataFlowArcTable::FindArc(KG3DDataFlowArc** ppArc,UINT uHandle)
{
	map<UINT,KG3DDataFlowArc*>::iterator i = m_mapArc.find(uHandle);
	if (i!=m_mapArc.end())
	{
		(*ppArc) = i->second;
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT KG3DDataFlowArcTable::ReleaseArc(UINT uHandle)
{
	map<UINT,KG3DDataFlowArc*>::iterator i = m_mapArc.find(uHandle);
	if (i!=m_mapArc.end())
	{
		KG3DDataFlowArc* pArc = i->second;
		SAFE_DELETE(pArc);

		m_mapArc.erase(i);
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT KG3DDataFlowArcTable::NewOneArc(UINT* puHandle)
{
	KG3DDataFlowArc* pArc = new KG3DDataFlowArc;
	KGLOG_PROCESS_ERROR(pArc);

	UINT uHandle = GetCurrentHandle();
	m_mapArc[uHandle] = pArc;

	(*puHandle) = uHandle;
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowArcTable::GetAllArc(vector<UINT>* pvecHandle)
{
	KGLOG_PROCESS_ERROR(pvecHandle);

	{
		map<UINT,KG3DDataFlowArc*>::iterator i = m_mapArc.begin();
		map<UINT,KG3DDataFlowArc*>::iterator iend = m_mapArc.end();
		while (i!=iend)
		{
			pvecHandle->push_back(i->first);
			i++;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}