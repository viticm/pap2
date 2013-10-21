#include "StdAfx.h"
#include "KG3DDataFlowArc.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


KG3DDataFlowArc::KG3DDataFlowArc(void)
{
	m_uDataType = 0;
	m_dwContainerType = 0;
	m_bDataReady = FALSE;
}

KG3DDataFlowArc::~KG3DDataFlowArc(void)
{
}

HRESULT KG3DDataFlowArc::SetName(LPCTSTR pName)
{
	m_szName = pName;
	return S_OK;
}

HRESULT KG3DDataFlowArc::GetName(LPCTSTR pName)
{
	wsprintf((LPTSTR)pName,"%s",m_szName.c_str());
	return S_OK;
}

HRESULT KG3DDataFlowArc::SetType(UINT uDataType,DWORD dwContainerType)
{
	m_uDataType = uDataType;
	m_dwContainerType = dwContainerType;

	return S_OK;
}

HRESULT KG3DDataFlowArc::GetType(UINT* puDataType,DWORD* pdwContainerType)
{
	(*puDataType) = m_uDataType;
	(*pdwContainerType) = m_dwContainerType;
	return S_OK;
}

HRESULT KG3DDataFlowArc::SetNumInputConnection(DWORD dwNum)
{
	m_vecInputConnection.resize(dwNum);
	return S_OK;
}

HRESULT KG3DDataFlowArc::GetNumInputConnection(DWORD* pdwNum)
{
	(*pdwNum) = (DWORD)m_vecInputConnection.size();
	return S_OK;
}

HRESULT KG3DDataFlowArc::SetNumOutputConnection(DWORD dwNum)
{
	m_vecOutputConnection.resize(dwNum);
	return S_OK;
}

HRESULT KG3DDataFlowArc::GetNumOutputConnection(DWORD* pdwNum)
{
	(*pdwNum) = (DWORD)m_vecOutputConnection.size();
	return S_OK;
}

HRESULT KG3DDataFlowArc::SetInputConnection(DWORD dwIndex,UINT uProcessorHandle,KG3DDataFlowProcessor* pProcessor,DWORD dwProcessorConnectionIndex)
{
	KGLOG_PROCESS_ERROR(dwIndex < (DWORD)m_vecInputConnection.size());

	ConnectionInfo& vInfo = m_vecInputConnection[dwIndex];

	vInfo.uHandleProcessor = uProcessorHandle;
	vInfo.lpProcessor = pProcessor;
	vInfo.dwIndex = dwProcessorConnectionIndex;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowArc::GetInputConnection(DWORD dwIndex,UINT* puProcessorHandle,KG3DDataFlowProcessor** ppProcessor,DWORD* pdwProcessorConnectionIndex)
{
	KGLOG_PROCESS_ERROR(dwIndex < (DWORD)m_vecInputConnection.size());

	ConnectionInfo& vInfo = m_vecInputConnection[dwIndex];

	(*puProcessorHandle) = vInfo.uHandleProcessor;
	(*ppProcessor) = vInfo.lpProcessor;
	(*pdwProcessorConnectionIndex) = vInfo.dwIndex;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowArc::SetOutputConnection(DWORD dwIndex,UINT uProcessorHandle,KG3DDataFlowProcessor* pProcessor,DWORD dwProcessorConnectionIndex)
{
	KGLOG_PROCESS_ERROR(dwIndex < (DWORD)m_vecOutputConnection.size());

	ConnectionInfo& vInfo = m_vecOutputConnection[dwIndex];

	vInfo.uHandleProcessor = uProcessorHandle;
	vInfo.lpProcessor = pProcessor;
	vInfo.dwIndex = dwProcessorConnectionIndex;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DDataFlowArc::GetOutputConnection(DWORD dwIndex,UINT* puProcessorHandle,KG3DDataFlowProcessor** ppProcessor,DWORD* pdwProcessorConnectionIndex)
{
	KGLOG_PROCESS_ERROR(dwIndex < (DWORD)m_vecOutputConnection.size());

	ConnectionInfo& vInfo = m_vecOutputConnection[dwIndex];

	(*puProcessorHandle) = vInfo.uHandleProcessor;
	(*ppProcessor) = vInfo.lpProcessor;
	(*pdwProcessorConnectionIndex) = vInfo.dwIndex;

	return S_OK;
Exit0:
	return E_FAIL;
}

BOOL KG3DDataFlowArc::IsDataReady()
{
	return m_bDataReady;
}

HRESULT KG3DDataFlowArc::InputDataList(KG3DDataFlowDataObject* pObject)
{
	m_listDataObject.push_back(pObject);
	m_bDataReady = TRUE;
	return S_OK;
}
