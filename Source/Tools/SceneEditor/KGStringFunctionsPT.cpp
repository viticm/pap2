////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGStringFunctionsPT.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-25 11:12:53
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KGStringFunctionsPT.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////
template<typename _Type>
BOOL TKGConvertTypeToString(const VOID* pValue, INT nValueSize, std::tstring& csResult );

template<typename _Type>
BOOL TKGConvertStringToType(const std::tstring& csResult, VOID* pRetValue, INT nValueSize);

template<typename _Type>
BOOL TKGConvertTypeToString(const VOID* pValue, INT nValueSize, std::tstring& csResult )
{
	_ASSERTE(pValue && nValueSize >= sizeof(_Type));
	const _Type* pSolidValue = reinterpret_cast<const _Type*>(pValue);
	std::tstring TempString(_T(""));
	return KG_CUSTOM_HELPERS::TValueToString(*pSolidValue, csResult);	
}
template<typename _Type>
BOOL TKGConvertStringToType(const std::tstring& csResult, VOID* pRetValue, INT nValueSize)
{
	_ASSERTE(pRetValue && nValueSize >= sizeof(_Type));
	_Type* pSolidValue = reinterpret_cast<_Type*>(pRetValue);
	return KG_CUSTOM_HELPERS::TStringToValue(csResult, *pSolidValue);	
}

template<>
BOOL TKGConvertTypeToString<std::tstring>(const VOID* pValue, INT nValueSize, std::tstring& csResult)
{
	_ASSERTE(pValue != (VOID*)&csResult &&  nValueSize>= sizeof(std::tstring));
	csResult = *reinterpret_cast<const std::tstring*>(pValue);
	return TRUE;
}

template<>
BOOL TKGConvertStringToType<std::tstring>(const std::tstring& csResult, VOID* pRetValue, INT nValueSize)
{
	_ASSERTE((VOID*)&csResult != pRetValue && nValueSize >= sizeof(std::tstring));
	*reinterpret_cast<std::tstring*>(pRetValue) = csResult;
	return TRUE;
}

typedef BOOL (*Func_ValueToString)(const VOID*, INT, std::tstring&);
typedef BOOL (*Func_StringToValue)(const std::tstring&, VOID*, INT);

typedef struct _Convertor_Func_Struct
{
	Func_ValueToString m_pFuncValueToString;
	Func_StringToValue m_pFuncStringToValue;
	_Convertor_Func_Struct(Func_ValueToString A, Func_StringToValue B)
		:m_pFuncValueToString(A)
		,m_pFuncStringToValue(B)
	{

	}
}Convertor_Func_Struct;
///注意一个一个对应
static Convertor_Func_Struct ConvertorFuncArray[] = 
{
	Convertor_Func_Struct(&(TKGConvertTypeToString<D3DCOLOR>), &(TKGConvertStringToType<D3DCOLOR>)),
	Convertor_Func_Struct(&(TKGConvertTypeToString<INT>), &(TKGConvertStringToType<INT>)),
	Convertor_Func_Struct(&(TKGConvertTypeToString<FLOAT>), &(TKGConvertStringToType<FLOAT>)),
	Convertor_Func_Struct(&(TKGConvertTypeToString<vector<INT> >), &(TKGConvertStringToType<vector<INT> >)),
	Convertor_Func_Struct(&(TKGConvertTypeToString<std::tstring>), &(TKGConvertStringToType<std::tstring>)),
	Convertor_Func_Struct(&(TKGConvertTypeToString<UINT>), &(TKGConvertStringToType<UINT>)),
	Convertor_Func_Struct(&(TKGConvertTypeToString<D3DXVECTOR3>), &(TKGConvertStringToType<D3DXVECTOR3>))
};
//////////////////////////////////////////////////////////////////////////
namespace KSCENEEDITOR_HELPERCLASSES
{
	INT GetTypeSize( DWORD dwType )
	{
		switch(dwType)
		{
		case KSCENE_COMMON_PARAM_D3DCOLOR:
			return sizeof(D3DCOLOR);
			break;
		case KSCENE_COMMON_PARAM_INT:
			return sizeof(INT);
			break;
		case KSCENE_COMMON_PARAM_FLOAT:
			return sizeof(FLOAT);
			break;
		case KSCENE_COMMON_PARAM_VEC_INT:
			return sizeof(std::vector<INT>);
			break;
		case KSCENE_COMMON_PARAM_STRING:
			return sizeof(std::tstring);
			break;
		case KSCENE_COMMON_PARAM_UINT:
			return sizeof(UINT);
			break;
		case KSCENE_COMMON_PARAM_D3DXVECTOR3:
			return sizeof(D3DXVECTOR3);
			break;
		default:
			goto Exit0;
			break;
		}
Exit0:
		return INT_MAX;
	}
	BOOL MakeDefaultValue( DWORD dwType, VOID* pBuffer, INT nBufferSize )
	{
		int nTypeSize = GetTypeSize(dwType);
		KG_PROCESS_ERROR(pBuffer && nBufferSize >= GetTypeSize(dwType));
		switch(dwType)
		{
		case KSCENE_COMMON_PARAM_D3DCOLOR:
		case KSCENE_COMMON_PARAM_INT:
		case KSCENE_COMMON_PARAM_FLOAT:
		case KSCENE_COMMON_PARAM_UINT:
		case KSCENE_COMMON_PARAM_D3DXVECTOR3:
			{
				ZeroMemory(pBuffer, nTypeSize);
			}
			break;
		case KSCENE_COMMON_PARAM_VEC_INT:
#ifdef new
#undef new
#define _NEW_RETURN_TO_ORIGIONAL_FORM
#endif
			{
				pBuffer = new(pBuffer) vector<INT>;
			}
			break;
		case KSCENE_COMMON_PARAM_STRING:
			{
				pBuffer = new(pBuffer) std::tstring(_T(""));
			}
#ifdef _NEW_RETURN_TO_ORIGIONAL_FORM
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
			break;
		default:
			goto Exit0;
			break;
		}
		return TRUE;
Exit0:
		return FALSE;
	}
	BOOL ConvertValueToString( DWORD dwType, const VOID* pValue, INT nValueSize, std::tstring& csResult )
	{
		_ASSERTE(dwType <= _countof(ConvertorFuncArray));
		Func_ValueToString FuncP= ConvertorFuncArray[dwType].m_pFuncValueToString;
		return (*FuncP)(pValue, nValueSize, csResult);
	}

	BOOL ConvertStringToValue(DWORD dwType, const std::tstring& csSrc, VOID* pRetValue, INT nValueSize )
	{
		_ASSERTE(dwType <= _countof(ConvertorFuncArray));
		Func_StringToValue FuncP= ConvertorFuncArray[dwType].m_pFuncStringToValue;
		return (*FuncP)(csSrc, pRetValue, nValueSize);
	}
	BOOL ConvertorIsAValidType(DWORD dwType)
	{
		if (dwType < _countof(ConvertorFuncArray))
		{
			return TRUE;
		}
		return FALSE;
	}
	BOOL ConvertorTypeCopy( DWORD dwType, const VOID* pFrom, VOID* pTo, INT nSize )
	{
		_ASSERTE(GetTypeSize(dwType) == nSize && pFrom != pTo);
		switch(dwType)
		{
		case KSCENE_COMMON_PARAM_D3DCOLOR:
		case KSCENE_COMMON_PARAM_INT:
		case KSCENE_COMMON_PARAM_FLOAT:
		case KSCENE_COMMON_PARAM_UINT:
		case KSCENE_COMMON_PARAM_D3DXVECTOR3:
			{
				int nRet = memcpy_s(pTo, nSize, pFrom, nSize);
				if (0 == nRet)
					return TRUE;
			}
			break;
		case KSCENE_COMMON_PARAM_VEC_INT:
			{
				*reinterpret_cast<vector<INT>* >(pTo) = *reinterpret_cast<const vector<INT>* >(pFrom);
				return TRUE;
			}
			break;
		case KSCENE_COMMON_PARAM_STRING:
			{
				*reinterpret_cast<std::tstring*>(pTo) = *reinterpret_cast<const std::tstring*>(pFrom);
				return TRUE;
			}
			break;
		default:
			break;
		}
		return FALSE;
	}
	BOOL ConvertorMakeObjectInBuffer( DWORD dwType, VOID* pBuffer, INT nSize )
	{
		_ASSERTE(pBuffer && nSize >= GetTypeSize(dwType));
		switch(dwType)
		{
		case KSCENE_COMMON_PARAM_D3DCOLOR:
		case KSCENE_COMMON_PARAM_INT:
		case KSCENE_COMMON_PARAM_FLOAT:
		case KSCENE_COMMON_PARAM_UINT:
		case KSCENE_COMMON_PARAM_D3DXVECTOR3:
			{
				ZeroMemory(pBuffer, nSize);
			}
			break;
		case KSCENE_COMMON_PARAM_VEC_INT:
#ifdef new
#undef new
#define _NEW_RETURN_TO_ORIGIONAL_FORM
#endif
			{
				pBuffer = new(pBuffer) vector<INT>;
			}
			break;
		case KSCENE_COMMON_PARAM_STRING:
			{
				pBuffer = new(pBuffer) std::tstring(_T(""));
			}
#ifdef _NEW_RETURN_TO_ORIGIONAL_FORM
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
			break;
		default:
			goto Exit0;
			break;
		}
		return TRUE;
Exit0:
		return FALSE;
	}

	DWORD ConverKG3DTYPEToParamType( DWORD dwType )
	{
		DWORD RetType = (DWORD)KSCENE_COMMON_PARAM_INVALUE_TYPE;
		switch(dwType)
		{
		case KG3DTYPE_DWORD:
			RetType = KSCENE_COMMON_PARAM_D3DCOLOR;
			break;
		case KG3DTYPE_UINT:
			RetType = KSCENE_COMMON_PARAM_UINT;
			break;
		case KG3DTYPE_FLOAT:
			RetType = KSCENE_COMMON_PARAM_FLOAT;
		    break;
		default:
		    break;
		}
		return RetType;
	}
};

////////////////////////////////////////////////////////////////////////////////

