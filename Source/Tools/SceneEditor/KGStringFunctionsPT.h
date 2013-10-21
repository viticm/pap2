////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGStringFunctionsPT.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-25 11:10:07
//  Comment     : 模板需要静态类型，但如果要使用数组，每个数组类型不同的话，
//	除了TypeList就很难解决了。而通过DWORD标识类型再访问却可以获得更大的灵活性，
//	原理是用静态数组通过类型转换为函数指针
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSTRINGFUNCTIONSPT_H_
#define _INCLUDE_KGSTRINGFUNCTIONSPT_H_

////////////////////////////////////////////////////////////////////////////////
namespace KSCENEEDITOR_HELPERCLASSES
{
	enum///不能随便改值，因为和数组关联了
	{
		KSCENE_COMMON_PARAM_D3DCOLOR = 0,
		KSCENE_COMMON_PARAM_INT = 1,
		KSCENE_COMMON_PARAM_FLOAT = 2,
		KSCENE_COMMON_PARAM_VEC_INT = 3,
		KSCENE_COMMON_PARAM_STRING = 4,
		KSCENE_COMMON_PARAM_UINT = 5,
		KSCENE_COMMON_PARAM_D3DXVECTOR3 = 6,
		KSCENE_COMMON_PARAM_INVALUE_TYPE = DWORD(-1),
	};
	template<typename _Type>
	struct TConvertorDWORDType;
#define  ADD_TCONVERTOR_DWORD_TYPE(_Type, dWord) template<>struct TConvertorDWORDType<_Type>{enum{result = dWord};}
	ADD_TCONVERTOR_DWORD_TYPE(INT, KSCENE_COMMON_PARAM_INT);
	ADD_TCONVERTOR_DWORD_TYPE(D3DCOLOR, KSCENE_COMMON_PARAM_D3DCOLOR);
	ADD_TCONVERTOR_DWORD_TYPE(FLOAT, KSCENE_COMMON_PARAM_FLOAT);
	ADD_TCONVERTOR_DWORD_TYPE(std::vector<INT>, KSCENE_COMMON_PARAM_VEC_INT);
	ADD_TCONVERTOR_DWORD_TYPE(std::tstring, KSCENE_COMMON_PARAM_STRING);
	ADD_TCONVERTOR_DWORD_TYPE(UINT, KSCENE_COMMON_PARAM_UINT);
	ADD_TCONVERTOR_DWORD_TYPE(D3DXVECTOR3, KSCENE_COMMON_PARAM_D3DXVECTOR3);
	INT	GetTypeSize(DWORD dwType);
	BOOL MakeDefaultValue(DWORD dwType, VOID* pBuffer, INT nBufferSize);
	BOOL ConvertValueToString(DWORD dwType, const VOID* pValue, INT nValueSize, std::tstring& csResult);
	BOOL ConvertStringToValue(DWORD dwType, const tstring& csSrc, VOID* pRetValue, INT nValueSize);
	BOOL ConvertorIsAValidType(DWORD dwType);
	BOOL ConvertorTypeCopy(DWORD dwType, const VOID* pFrom, VOID* pTo, INT nSize);
	BOOL ConvertorMakeObjectInBuffer(DWORD dwType, VOID* pBuffer, INT nSize);
	DWORD ConverKG3DTYPEToParamType(DWORD dwType);//返回值KSCENE_COMMON_PARAM_INVALUE_TYPE失败
};
namespace KSH = KSCENEEDITOR_HELPERCLASSES;

#endif //_INCLUDE_KGSTRINGFUNCTIONSPT_H_
