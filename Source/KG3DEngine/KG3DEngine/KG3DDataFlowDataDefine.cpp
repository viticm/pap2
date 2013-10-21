#include "StdAfx.h"
#include "KG3DDataFlowDataDefine.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD GetVariableLengthByType(KG3DDATAFLOWVARIABLETYPE eType)
{
	DWORD dwLength = 0;

	switch(eType)
	{
	case VARIABLETYPE_INT:
		dwLength = sizeof(int);
		break;
	case VARIABLETYPE_DCOLOR:
		dwLength = sizeof(DWORD);
		break;

	case VARIABLETYPE_FLOAT:
		dwLength = sizeof(float);
		break;

	case VARIABLETYPE_FLOAT2:
		dwLength = sizeof(float)*2;
		break;

	case VARIABLETYPE_FLOAT3:
		dwLength = sizeof(float)*3;
		break;

	case VARIABLETYPE_FLOAT4:
		dwLength = sizeof(float)*4;
		break;

	case VARIABLETYPE_CHAR32:
		dwLength = sizeof(CHAR)*32;
		break;

	case VARIABLETYPE_CHAR128:
		dwLength = sizeof(CHAR)*128;
		break;

	case VARIABLETYPE_CHAR256:
		dwLength = sizeof(CHAR)*256;

		break;

	case VARIABLETYPE_WCHAR32:
		dwLength = sizeof(WCHAR)*32;
		break;

	case VARIABLETYPE_WCHAR128:
		dwLength = sizeof(WCHAR)*128;
		break;

	case VARIABLETYPE_WCHAR256:
		dwLength = sizeof(WCHAR)*256;
		break;
	default:
		dwLength = 0;
	    break;
	}
	return dwLength;
}

KG3DDataFlowDataDefine::KG3DDataFlowDataDefine(void)
{
}

KG3DDataFlowDataDefine::~KG3DDataFlowDataDefine(void)
{
}

HRESULT KG3DDataFlowDataDefine::ComputeDataLength()
{
	HRESULT hr = E_FAIL;
	m_dwDataLength = 0;
	for (size_t i=0;i<m_vecVariableDefine.size();i++)
	{
		VariableDefine& Define = m_vecVariableDefine[i];

		DWORD dwLength = GetVariableLengthByType(Define.eType);
		KGLOG_PROCESS_ERROR(dwLength);

		Define.dwLength = dwLength;
		Define.dwPostionStart = m_dwDataLength;

		m_dwDataLength += dwLength;
	}

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DDataFlowDataDefine::FindVariableByName(int& nIndex,LPCTSTR pName)
{
	for (size_t i=0;i<m_vecVariableDefine.size();i++)
	{
		VariableDefine& Define = m_vecVariableDefine[i];

		if (_strcmpi(Define.szName.c_str(),pName) == 0)
		{
			nIndex = (int)i;
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT KG3DDataFlowDataDefine::LoadFromFile(LPCTSTR pFileName)
{
	return S_OK;
}

HRESULT KG3DDataFlowDataDefine::SaveToFile(LPCTSTR pFileName)
{
	return S_OK;
}

HRESULT KG3DDataFlowDataDefine::SetName(LPCTSTR pName)
{
	m_szName = pName;
	return S_OK;
}

HRESULT KG3DDataFlowDataDefine::GetName(LPCTSTR pName)
{
	wsprintf((LPTSTR)pName,"%s",m_szName.c_str());
	return S_OK;
}

HRESULT KG3DDataFlowDataDefine::SetNumVariable(DWORD dwNum)
{
	m_vecVariableDefine.resize((size_t)dwNum);
	return S_OK;
}

DWORD   KG3DDataFlowDataDefine::GetNumVariable()
{
	return (DWORD)m_vecVariableDefine.size();
}

HRESULT KG3DDataFlowDataDefine::SetVariable(DWORD dwIndex,KG3DDATAFLOWVARIABLETYPE eType,LPCTSTR pName)
{
	if (dwIndex < m_vecVariableDefine.size())
	{
		m_vecVariableDefine[dwIndex].eType = eType;
		m_vecVariableDefine[dwIndex].szName = pName;

		ComputeDataLength();
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

HRESULT KG3DDataFlowDataDefine::GetVariable(DWORD dwIndex,KG3DDATAFLOWVARIABLETYPE* peType,LPCTSTR pName)
{
	if (dwIndex < m_vecVariableDefine.size())
	{
		(*peType) = m_vecVariableDefine[dwIndex].eType;
		wsprintf((LPTSTR)pName,"%s",m_vecVariableDefine[dwIndex].szName.c_str());
		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}
