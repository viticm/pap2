////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneCheckers.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-11-17 17:50:38
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DSceneCheckers.h"
#include "KG3DDefaultShaderGroup.h"
#include "KG3DShaderManager.h"
#include "KG3DShaderHolder.h"

////////////////////////////////////////////////////////////////////////////////

class KG3DSceneCheckersSolid : public KG3DSceneCheckers
{
public:
	STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_SCENE_CHECKERS;}
	STDMETHOD_(LPVOID, ToAnotherInterface)(DWORD_PTR);

	virtual HRESULT EnableCheckNormal(BOOL bEnable);
	virtual ULONG IsCheckNormalEnable();
	virtual HRESULT InitNormalCheckerShaders(KG3DNormalChekerShaderArg Args[], size_t ArgsCount);

	KG3DSceneCheckersSolid();
private:
	BOOL m_bEnableCheckNormal;
	struct NormalCheckerShaders 
	{
		KG3DNormalChekerShaderArg Arg;
		KG3DShaderHolder ShaderHolder;
		KG3DDefaultShaderGroupElem ShaderHolderSaved;
	};
	enum{em_max_normal_checker = 100,};
	std::vector<NormalCheckerShaders>	m_vecNormalCheckerShaders;	
};

KG3DSceneCheckers& g_GetSceneCheckers()
{
	static KG3DSceneCheckersSolid Instance;
	return Instance;
}

KG3DSceneCheckersSolid::KG3DSceneCheckersSolid()
:m_bEnableCheckNormal(FALSE)
{

}

LPVOID STDMETHODCALLTYPE KG3DSceneCheckersSolid::ToAnotherInterface( DWORD_PTR Param )
{
	IEKG3DSceneCheckers* p = this;
	return p;
}

HRESULT KG3DSceneCheckersSolid::EnableCheckNormal( BOOL bEnable )
{
	//下面采用新的方法，全局宏改变器
	if(! m_bEnableCheckNormal && bEnable)
	{
		D3DXMACRO macroNormal[] = {
			{_T("SHADER_MODEL"), _T("2")},	//NormalChecker只在这个shaderModel下起作用
			{_T("NORMAL_CHECK"), _T("1")},
			{NULL, NULL},
		};

		D3DXMACRO_ARRAY macroArray = {macroNormal, _countof(macroNormal)};
		
		g_GetDefaultShaderGroup().ModifyGlobalMacro(macroArray);

		m_bEnableCheckNormal = TRUE;
	}
	else if(m_bEnableCheckNormal && ! bEnable)
	{	
		UINT uSM = g_GetDefaultShaderGroup().GetShaderModelToUse();
		LPCTSTR strSM = _T("3");
		if (uSM == 1)
		{
			strSM = _T("1");
		}
		else if (uSM == 2)
		{
			strSM = _T("2");
		}
		D3DXMACRO macroNormal[] = {
			{_T("SHADER_MODEL"), strSM},	//恢复到正确的ShderModel
			{_T("NORMAL_CHECK"), _T("0")},
			{NULL, NULL},
		};

		D3DXMACRO_ARRAY macroArray = {macroNormal, _countof(macroNormal)};

		g_GetDefaultShaderGroup().ModifyGlobalMacro(macroArray);

		m_bEnableCheckNormal = FALSE;
	}
	return S_OK;
}

ULONG KG3DSceneCheckersSolid::IsCheckNormalEnable()
{
	return m_bEnableCheckNormal;
}

HRESULT KG3DSceneCheckersSolid::InitNormalCheckerShaders( KG3DNormalChekerShaderArg Args[], size_t ArgsCount )
{
	_ASSERTE(m_vecNormalCheckerShaders.size() == 0 && ArgsCount < em_max_normal_checker);
	m_vecNormalCheckerShaders.resize(ArgsCount);
	_ASSERTE(m_vecNormalCheckerShaders.size() == ArgsCount);
	for (size_t i = 0; i < ArgsCount; ++i)
	{
		m_vecNormalCheckerShaders[i].Arg = Args[i];
	}
	return S_OK;
}