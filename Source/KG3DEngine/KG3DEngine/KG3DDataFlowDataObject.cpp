#include "StdAfx.h"
#include "KG3DDataFlowDataObject.h"
#include "KG3DDataFlowDataDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DDataFlowDataObject::KG3DDataFlowDataObject()
{
	m_uHandle = 0;
	m_lpData = NULL;
	m_dwSize = 0;
}

KG3DDataFlowDataObject::~KG3DDataFlowDataObject()
{
	m_uHandle = 0;
}

HRESULT KG3DDataFlowDataObject::Init(UINT uHandle,DWORD dwSize)
{
	m_uHandle = uHandle;
	if(m_dwSize != dwSize)
	{
		m_dwSize = dwSize;
		m_lpData = new BYTE[m_dwSize];
		KGLOG_PROCESS_ERROR(m_lpData);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowDataObject::UnInit()
{
	SAFE_DELETE_ARRAY(m_lpData);
	m_dwSize = 0;
	return S_OK;
}

BYTE* KG3DDataFlowDataObject::pGetVariablePointer(DWORD dwIndex,KG3DDataFlowDataDefine* pDefine)
{
	DWORD dwPos = pDefine->m_vecVariableDefine[dwIndex].dwPostionStart;

	return &m_lpData[dwPos];
}

HRESULT KG3DDataFlowDataObject::SetInt(int Value,DWORD dwIndex,KG3DDataFlowDataDefine* pDefine)
{
	KGLOG_PROCESS_ERROR(pDefine);
	KGLOG_PROCESS_ERROR(dwIndex < pDefine->m_vecVariableDefine.size());

	KGLOG_PROCESS_ERROR(pDefine->m_vecVariableDefine[dwIndex].eType==VARIABLETYPE_INT);

	BYTE* pPointer = pGetVariablePointer(dwIndex,pDefine);
	(*(int*)pPointer) = Value;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowDataObject::GetInt(int* pValue,DWORD dwIndex,KG3DDataFlowDataDefine* pDefine)
{
	KGLOG_PROCESS_ERROR(pDefine);
	KGLOG_PROCESS_ERROR(dwIndex < pDefine->m_vecVariableDefine.size());

	KGLOG_PROCESS_ERROR(pDefine->m_vecVariableDefine[dwIndex].eType==VARIABLETYPE_INT);

	BYTE* pPointer = pGetVariablePointer(dwIndex,pDefine);
	(*pValue) = (*(int*)pPointer) ;
Exit0:
	return E_FAIL;
}

