////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderInfrastructure.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-6-10 15:08:10
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KG3DShaderInfrastructure.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

D3DXMACRO_ARRAY KG3DShaderMacroDef::ToD3DXMACRO_ARRAY( std::vector<D3DXMACRO>& bufferTemp )const
{
	if(macroDefs.empty())
	{
		return D3DXMACRO_ARRAY::GetEmptyMacroArray();
	}
	KG_PROCESS_ERROR(D3DXMACRO_ARRAY::em_max_macro >= macroDefs.size() + 1);

	{
		bufferTemp.clear();
		bufferTemp.reserve(macroDefs.size() + 1);	//最后要用NULL , NULL结尾，这是D3D的规范

		for (size_t i = 0, uEnd = macroDefs.size(); i < uEnd; ++i)
		{
			const TypeSingleDef& refDef = macroDefs[i];
			D3DXMACRO macroDef = {
				refDef.first.c_str(),
				refDef.second.c_str(),
			};
			bufferTemp.push_back(macroDef);
		}

		D3DXMACRO macroNULL = {NULL, NULL};

		bufferTemp.push_back(macroNULL);
		_ASSERTE(macroDefs.size() + 1 == bufferTemp.size());

		D3DXMACRO_ARRAY ret = {
			&bufferTemp[0],
			bufferTemp.size(),
		};
		_ASSERTE(D3DXMACRO_ARRAY::IsValid(ret));

#if defined(DEBUG) | defined(_DEBUG)
		if (! macroDefs.empty())
		{
			_ASSERTE(macroDefs.size() + 1 == ret.uMacroCount);
		}
#endif
		return ret;
	}
Exit0:
	return D3DXMACRO_ARRAY::GetEmptyMacroArray();
}

BOOL KG3DShaderMacroDef::FromD3DXMACRO_ARRAY( const D3DXMACRO_ARRAY& macroArray )
{
	macroDefs.clear();
	if (NULL == macroArray.pMacroDefs)
	{
		return TRUE;
	}

	KG_PROCESS_ERROR(D3DXMACRO_ARRAY::IsValid(macroArray));

	macroDefs.reserve(macroArray.uMacroCount);

	for (size_t i = 0; i < macroArray.uMacroCount - 1; ++i)	//最后是有结尾的,不用管
	{
		TypeSingleDef singleDef;
		const D3DXMACRO& curMacro = macroArray.pMacroDefs[i];
		KG_PROCESS_ERROR(NULL != curMacro.Name && NULL != curMacro.Definition);
		singleDef.first = curMacro.Name;
		singleDef.second = curMacro.Definition;

		macroDefs.push_back(singleDef);	//这里很多底层的分配和复制行为，效率的确不是很高，不过牺牲一点效率换取易读的结构是值得的，而且调用频率很低
	}

	
#if defined(DEBUG) | defined(_DEBUG)
	const D3DXMACRO& lastMacro = macroArray.pMacroDefs[macroArray.uMacroCount - 1];
	_ASSERTE(NULL == lastMacro.Name && NULL == lastMacro.Definition);

	_ASSERTE(macroDefs.size() + 1 == macroArray.uMacroCount);
#endif

	
	return TRUE;
Exit0:
	return FALSE;
}

enum
{
	em_KG3DShaderBuildStruct_binary_size = offsetof(KG3DShaderBuildStruct, pEventHandler),
};

KG3DShaderBuildStruct& KG3DShaderBuildStruct::operator=(const KG3DShaderBuildStruct& Other)
{
	memcpy_s(this, em_KG3DShaderBuildStruct_binary_size, &Other, em_KG3DShaderBuildStruct_binary_size);

	pEventHandler = ((NULL == Other.pEventHandler) ? NULL : Other.pEventHandler->Clone());

	strFilePathOrResourceID = Other.strFilePathOrResourceID;
	strIncludeFilePath = Other.strIncludeFilePath;
	strErrorString = Other.strErrorString;
	vecEffectParamHandleBuffer = Other.vecEffectParamHandleBuffer;

	macroDef = Other.macroDef;

	return *this;
}
bool operator==(KG3DShaderMacroDef& a, KG3DShaderMacroDef& b)
{
	_ASSERTE(TDD::IsTDDWorking() && _T("只在TDD测试中使用") );
	return a.macroDefs == b.macroDefs;//这个很慢，不过这个函数是用于测试用的
}
bool operator==(KG3DShaderBuildStruct& a, KG3DShaderBuildStruct& b)
{
	_ASSERTE(TDD::IsTDDWorking() && _T("只在TDD测试中使用"));
	bool bRet1 = (0 == memcmp(&a, &b, em_KG3DShaderBuildStruct_binary_size));
	if(! bRet1)
		return false;

	bRet1 = bRet1 && (0 == _tcsicmp(a.strFilePathOrResourceID.c_str(), b.strFilePathOrResourceID.c_str()));
	if(! bRet1)
		return false;

	bRet1 = bRet1 && (0 == _tcsicmp(a.strIncludeFilePath.c_str(), b.strIncludeFilePath.c_str()));
	if(! bRet1)
		return false;

	bRet1 = bRet1 && (a.macroDef == b.macroDef);
	if(! bRet1)
		return false;

	//下面有取[0]，一定要判空
	if (a.vecEffectParamHandleBuffer.empty())
	{
		if(! b.vecEffectParamHandleBuffer.empty())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else 
	{
		if(b.vecEffectParamHandleBuffer.empty())
		{
			return false;
		}
	}

	_ASSERTE(! a.vecEffectParamHandleBuffer.empty() && ! b.vecEffectParamHandleBuffer.empty());
	bool bRet2 = (a.vecEffectParamHandleBuffer.size() == b.vecEffectParamHandleBuffer.size()
		&& 0 == memcmp(&a.vecEffectParamHandleBuffer[0], &b.vecEffectParamHandleBuffer[0], a.vecEffectParamHandleBuffer.size()));
	return bRet2;
}

#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DShaderMacroDef)
{
#if 0

	_ASSERTE(NULL && _T("这个测试在平时可以禁止掉"));
	std::vector<D3DXMACRO> bufferMacro;
	BOOL bRet = FALSE;
	D3DXMACRO_ARRAY macroArray;

	//ToD3DXMACRO_ARRAY
	{
		//空会返回空
		{
			KG3DShaderMacroDef md;
			macroArray = md.ToD3DXMACRO_ARRAY(bufferMacro);
			_ASSERTE(NULL == macroArray.pMacroDefs && 0 == macroArray.uMacroCount);			
		}

		//在边界上会成功，超出会失败
		{
			{
				KG3DShaderMacroDef md;
				md.macroDefs.resize(D3DXMACRO_ARRAY::em_max_macro - 1);
				macroArray = md.ToD3DXMACRO_ARRAY(bufferMacro);
				_ASSERTE(D3DXMACRO_ARRAY::em_max_macro == macroArray.uMacroCount);
			}

			{
				KG3DShaderMacroDef md;
				md.macroDefs.resize(D3DXMACRO_ARRAY::em_max_macro);
				macroArray = md.ToD3DXMACRO_ARRAY(bufferMacro);
				_ASSERTE(0 == macroArray.uMacroCount);
			}
		}

		//成功的例子
		{
			D3DXMACRO tempMacros[] = 
			{
				{_T("A"), _T("aDef")},
				{_T("B"), _T("bDef")},
			};

			KG3DShaderMacroDef md;
			md.macroDefs.push_back(std::make_pair(std::tstring(tempMacros[0].Name), std::tstring(tempMacros[0].Definition)));
			md.macroDefs.push_back(std::make_pair(std::tstring(tempMacros[1].Name), std::tstring(tempMacros[1].Definition)));

			macroArray = md.ToD3DXMACRO_ARRAY(bufferMacro);
			_ASSERTE(3 == macroArray.uMacroCount && NULL != macroArray.pMacroDefs);
		}
	}
	//FromD3DXMACRO_ARRAY
	{
		//空的，得到也是空的
		{
			KG3DShaderMacroDef md;
			bRet = md.FromD3DXMACRO_ARRAY(D3DXMACRO_ARRAY::GetEmptyMacroArray());
			_ASSERTE(bRet && 0 == md.macroDefs.size());
		}
		//空指针会失败
		{
			D3DXMACRO_ARRAY emptyMacroArray = {
				NULL,
				1,
			};

			KG3DShaderMacroDef md;
			bRet = md.FromD3DXMACRO_ARRAY(emptyMacroArray);
			_ASSERTE(bRet && 0 == md.macroDefs.size());
		}
		//数目太大也会失败
		{
			D3DXMACRO	tempMacro = {_T("a"), _T("a")};
			std::vector<D3DXMACRO>	macroArrayBinary(D3DXMACRO_ARRAY::em_max_macro, tempMacro);
			macroArrayBinary.push_back(D3DXMACRO_ARRAY::GetEmptyMacro());
			D3DXMACRO_ARRAY macroArrayTemp = {
				&macroArrayBinary[0]
				, macroArrayBinary.size()
			};

			KG3DShaderMacroDef md;
			bRet = md.FromD3DXMACRO_ARRAY(macroArrayTemp);
			_ASSERTE(! bRet && 0 == md.macroDefs.size());
		}
		//成功的例子
		{
			D3DXMACRO	tempMacro = {_T("a"), _T("b")};
			std::vector<D3DXMACRO>	macroArrayBinary(D3DXMACRO_ARRAY::em_max_macro-1, tempMacro);
			macroArrayBinary.push_back(D3DXMACRO_ARRAY::GetEmptyMacro());
			D3DXMACRO_ARRAY macroArrayTemp = {
				&macroArrayBinary[0]
				, macroArrayBinary.size()
			};

			KG3DShaderMacroDef md;
			bRet = md.FromD3DXMACRO_ARRAY(macroArrayTemp);
			_ASSERTE(bRet && D3DXMACRO_ARRAY::em_max_macro - 1 == md.macroDefs.size());
			_ASSERTE(0 == _tcscmp(md.macroDefs[0].first.c_str(), _T("a")));
			_ASSERTE(0 == _tcscmp(md.macroDefs[0].second.c_str(), _T("b")));
		}
		//随便有个NULL都会失败
		{
			{
				D3DXMACRO	tempMacro = {_T("a"), _T("b")};
				std::vector<D3DXMACRO>	macroArrayBinary(1, tempMacro);
				macroArrayBinary.push_back(D3DXMACRO_ARRAY::GetEmptyMacro());
				macroArrayBinary[0].Name = NULL;
				D3DXMACRO_ARRAY macroArrayTemp = {
					&macroArrayBinary[0]
					, macroArrayBinary.size()
				};

				KG3DShaderMacroDef md;
				bRet = md.FromD3DXMACRO_ARRAY(macroArrayTemp);
				_ASSERTE(! bRet);
			}
			{
				D3DXMACRO	tempMacro = {_T("a"), _T("b")};
				std::vector<D3DXMACRO>	macroArrayBinary(1, tempMacro);
				macroArrayBinary.push_back(D3DXMACRO_ARRAY::GetEmptyMacro());
				macroArrayBinary[0].Definition = NULL;
				D3DXMACRO_ARRAY macroArrayTemp = {
					&macroArrayBinary[0]
					, macroArrayBinary.size()
				};

				KG3DShaderMacroDef md;
				bRet = md.FromD3DXMACRO_ARRAY(macroArrayTemp);
				_ASSERTE(! bRet);
			}
		}
		//没有加结尾，失败
		{
			D3DXMACRO	tempMacro = {_T("a"), _T("b")};
			std::vector<D3DXMACRO>	macroArrayBinary(D3DXMACRO_ARRAY::em_max_macro-1, tempMacro);
			D3DXMACRO_ARRAY macroArrayTemp = {
				&macroArrayBinary[0]
				, macroArrayBinary.size()
			};

			KG3DShaderMacroDef md;
			bRet = md.FromD3DXMACRO_ARRAY(macroArrayTemp);
			_ASSERTE(! bRet);
		}
	}
#endif
}
KG_TEST_END(KG3DShaderMacroDef)
#endif 
////////////////////////////////////////////////////////////////////////////////

