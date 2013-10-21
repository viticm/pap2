////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DDefaultShaderGroup.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-5 9:40:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DDefaultShaderGroup.h"
#include "KG3DShaderHolder.h"
#include "KG3DShaderManager.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	struct CustomShaderEventHandler : public KG3DShaderEventHandler
	{
		virtual	HRESULT OnResetDevice(LPD3DXEFFECT p)
		{
			_ASSERTE(NULL != p);
			//p->SetTexture("tConver", NULL);//<ConverMap Shared>

			INT uTexCount = 0;
			HRESULT hr = p->GetInt(_T("texCount"), &uTexCount);
			if (SUCCEEDED(hr))
			{
				for(INT i = 0; i < uTexCount; ++i)
				{
					TCHAR strTexName[MAX_PATH];
					_stprintf_s(strTexName, _T("tex%d"), i);
					strTexName[_countof(strTexName) - 1] = _T('\0');
					p->SetTexture(strTexName, NULL);
				}
			}

			return S_OK;
		}
		virtual ULONG STDMETHODCALLTYPE Release(){return 0;}
		virtual KG3DShaderEventHandler* Clone()const;
	};

	static CustomShaderEventHandler s_ShaderEventHandler;

	KG3DShaderEventHandler* CustomShaderEventHandler::Clone() const{return &s_ShaderEventHandler;}
};

////////////////////////////////////////////////////////////////////////////////
class KG3DDefaultShaderGroupSolid : public KG3DDefaultShaderGroup
{
	KG_TEST_DECLARE_CLASS_TEST(KG3DDefaultShaderGroupSolid);
	enum{em_max_shader_count = 100};
public:
	virtual KG3DShaderHolder& GetDefaultShader(DWORD dwType);
	virtual LPD3DXEFFECT	GetDefaultEffect(DWORD dwType);
	virtual HRESULT ReloadShaders();
	virtual HRESULT	Init(KG3DDefaultShaderRegistrationArg* pArgs, size_t Count);//类型要在一定范围内
	virtual HRESULT ReplaceDefaultShader(DWORD dwType, const KG3DDefaultShaderGroupElem& newShader, KG3DDefaultShaderGroupElem& oldShader);
	virtual HRESULT ModifyGlobalMacro(D3DXMACRO_ARRAY macroModificationArray);
	virtual HRESULT CheckShaderModel();
	virtual ULONG GetShaderModelToUse();
	virtual HRESULT LoadAllShaders();
	KG3DDefaultShaderGroupSolid();
	~KG3DDefaultShaderGroupSolid(){}
private:
	
	static UINT GetShaderModelToUsePrivate(const D3DCAPS9& capsIn, INT nForceSM);//如果ForceSM不为0，则使用ForceSM的值

	struct DefaultShaderRecord;
	static VOID CreateShader(DefaultShaderRecord& ds, UINT uShaderModel);

	struct DefaultShaderRecord 
	{
		KG3DDefaultShaderGroupElem elem;
		KG3DDefaultShaderRegistrationArg arg;
		std::vector<D3DXMACRO>	macroBuffer;
	};
	typedef std::vector<DefaultShaderRecord> TypeCt;

	DWORD m_dwMinType;
	TypeCt m_vecShaders;
	bool   m_bIsInit;
	UINT	m_uShaderModelToBeUsed;

	KG3DDefaultShaderGroupSolid(const KG3DDefaultShaderGroupSolid&);
	KG3DDefaultShaderGroupSolid& operator=(const KG3DDefaultShaderGroupSolid&);
	DefaultShaderRecord& GetRecord(DWORD dwType);
};

extern KG3DEngineOption g_cEngineOption;

KG3DDefaultShaderGroupSolid::KG3DDefaultShaderGroupSolid()
	:m_bIsInit(false)
	,m_dwMinType(INT_MAX)
	,m_uShaderModelToBeUsed(2)
{

}

HRESULT KG3DDefaultShaderGroupSolid::CheckShaderModel()
{
	m_uShaderModelToBeUsed = GetShaderModelToUsePrivate(g_cGraphicsEngine.GetD3DCaps(), g_cEngineOption.nForceShaderModel);
	return S_OK;
}
HRESULT KG3DDefaultShaderGroupSolid::Init( KG3DDefaultShaderRegistrationArg* pArgs, size_t Count )
{
	_ASSERTE(NULL != pArgs && Count <= em_max_shader_count);

	_ASSERTE(! m_bIsInit);
	_ASSERTE(m_vecShaders.empty());

	KG_PROCESS_SUCCESS(0 == Count);

	m_vecShaders.reserve(Count);

	m_dwMinType = pArgs[0].dwType;

	DWORD dwLastType = m_dwMinType;
	for (size_t i = 0; i < Count; ++i)
	{
		m_vecShaders.push_back(DefaultShaderRecord());//注意push_back触发的拷贝构造函数
		DefaultShaderRecord& ds = m_vecShaders.back();

		bool bMacroValid = D3DXMACRO_ARRAY::IsValid(pArgs->macroArray);
		_ASSERTE(bMacroValid && _T("必须填对，不然自己找事"));
		if (! bMacroValid)
		{
			KGLogPrintf(KGLOG_ERR, "以下shader的预设Macro错误，预计会造成编译失败: %u", ds.arg.dwType);
		}
		
		ds.arg = pArgs[i];
		ds.arg.macroArray = D3DXMACRO_ARRAY::GetEmptyMacroArray();
		ds.arg.macroArray.MergeDefinition(ds.macroBuffer, pArgs[i].macroArray);

		

#if defined(DEBUG) | defined(_DEBUG)

		if (0 != i)
		{
			_ASSERTE(pArgs[i].dwType == dwLastType + 1 
				&& _T("类型必须是一个比一个大1的状态连续放置，这样下面GetRecord才能高效的线性查找到对应记录"));
		}

		{
			DefaultShaderRecord& dsRef = m_vecShaders.back();

			for (size_t i = 0; i < _countof(dsRef.elem.m_ShaderBottles); ++i)
			{
				_ASSERTE(! Private::IsShaderHolderInited(dsRef.elem.m_ShaderBottles[i]));//这个时候ShaderHolder应该是空的
			}
		}
#endif
		dwLastType = pArgs[i].dwType;
	}
	_ASSERTE(m_dwMinType >= 0 && m_vecShaders.size() == (size_t)Count);
Exit1:
	m_bIsInit = true;
	return S_OK;	
}
KG3DDefaultShaderGroupSolid::DefaultShaderRecord& KG3DDefaultShaderGroupSolid::GetRecord(DWORD dwType)
{
	_ASSERTE(NULL != g_pd3dDevice);

	size_t Pos = dwType - m_dwMinType;
	_ASSERTE(Pos < m_vecShaders.size());

	return m_vecShaders[Pos];
}


KG3DShaderHolder& KG3DDefaultShaderGroupSolid::GetDefaultShader(DWORD dwType)
{
	DefaultShaderRecord& ds = GetRecord(dwType);
	_ASSERTE(dwType == ds.arg.dwType);

	_ASSERTE(m_uShaderModelToBeUsed <= EM_MAX_SHADER_MODEL_SUPPORTED);
	KG3DShaderHolder& shaderHolderBottleRef = ds.elem.m_ShaderBottles[m_uShaderModelToBeUsed-1];
	
	if (Private::IsShaderHolderInited(shaderHolderBottleRef))
	{
		return shaderHolderBottleRef;
	}
	
	CreateShader(ds, m_uShaderModelToBeUsed);

	return shaderHolderBottleRef;
}
VOID KG3DDefaultShaderGroupSolid::CreateShader( DefaultShaderRecord& ds, UINT uShaderModel )
{
	_ASSERTE(uShaderModel <= EM_MAX_SHADER_MODEL_SUPPORTED);
	KG3DDefaultShaderRegistrationArg& curArg = ds.arg;

	std::tstring* pShaderPath = NULL;
	if(uShaderModel == 1)
	{
		pShaderPath  = &curArg.tcsPathVersion1_1;
	}
	else if (uShaderModel == 2)
	{
		pShaderPath = &curArg.tcsPathVersion2_0;
	}
	else	//包括0的情况，即默认
	{
		pShaderPath = &curArg.tcsPathVersion3_0;
	}
	
	_ASSERTE(NULL != pShaderPath && uShaderModel - 1 < _countof(ds.elem.m_ShaderBottles));
	ds.elem.m_ShaderBottles[uShaderModel-1] = g_GetShaderManager().Create(g_pd3dDevice
		, pShaderPath->c_str()
		, curArg.macroArray
		, curArg.IncludeFilePath
		, curArg.Flags
		, curArg.uShaderParamHandleSize
		, TRUE	//<Shared Pool> 由这个启动
		, NULL
		, &CustomShaderEventHandler());
	_ASSERTE(Private::IsShaderHolderInited(ds.elem.m_ShaderBottles[uShaderModel-1]));
}
LPD3DXEFFECT KG3DDefaultShaderGroupSolid::GetDefaultEffect( DWORD dwType )
{
	return GetDefaultShader(dwType).GetEffect();
}

HRESULT KG3DDefaultShaderGroupSolid::ReloadShaders()
{
	g_GetShaderManager().ReloadAll();
	return S_OK;
}

ULONG KG3DDefaultShaderGroupSolid::GetShaderModelToUse()
{
	return m_uShaderModelToBeUsed;
}
UINT KG3DDefaultShaderGroupSolid::GetShaderModelToUsePrivate( const D3DCAPS9& capsIn, INT nForceSM )
{
	UINT uSM = 3;

	if (0 != nForceSM && nForceSM >= 1 && nForceSM <= EM_MAX_SHADER_MODEL_SUPPORTED)
	{
		uSM = nForceSM;
	}
	else
	{
		DWORD dwVersion = capsIn.PixelShaderVersion;

		if(dwVersion < D3DPS_VERSION(2, 0))
		{
			uSM = 1;
		}
		else if(dwVersion < D3DPS_VERSION(3, 0))
		{
			uSM = 2;
		}
	}

	_ASSERTE(uSM >= EM_MIN_SHADER_MODEL_SUPPORTED && uSM <= EM_MAX_SHADER_MODEL_SUPPORTED);

	return uSM;
}
HRESULT KG3DDefaultShaderGroupSolid::ReplaceDefaultShader( DWORD dwType, const KG3DDefaultShaderGroupElem& newShader, KG3DDefaultShaderGroupElem& oldShader)
{
	DefaultShaderRecord& DS =GetRecord(dwType);
	oldShader = DS.elem;
	DS.elem = newShader;
	return S_OK;
}
HRESULT KG3DDefaultShaderGroupSolid::ModifyGlobalMacro(D3DXMACRO_ARRAY macroModificationArray)
{
	KG_PROCESS_SUCCESS(NULL == macroModificationArray.pMacroDefs);
	KG_PROCESS_ERROR(D3DXMACRO_ARRAY::IsValid(macroModificationArray));
	for (size_t i = 0, uEnd = m_vecShaders.size(); i < uEnd; ++i)
	{
		DefaultShaderRecord& ds = m_vecShaders[i];

		_ASSERTE(D3DXMACRO_ARRAY::IsValid(ds.arg.macroArray));

		BOOL bRet = ds.arg.macroArray.MergeDefinition(ds.macroBuffer, macroModificationArray);
		if(! bRet)
		{
			KGLogPrintf(KGLOG_ERR, _STRINGER(KG3DDefaultShaderGroupSolid::ModifyGlobalMacro)_T(",%u"), ds.arg.dwType);
			continue;
		}

		_ASSERTE(m_uShaderModelToBeUsed - 1 < _countof(ds.elem.m_ShaderBottles));
		for (size_t x = 0; x < _countof(ds.elem.m_ShaderBottles); ++x)
		{
			if (x != (m_uShaderModelToBeUsed - 1))
			{
				ds.elem.m_ShaderBottles[x] = KG3DShaderHolder();	//清空其它不用的shader				
			}
			else if(Private::IsShaderHolderInited(ds.elem.m_ShaderBottles[x]))
			{
				KG3DShaderPtrHolder* ptrHolder= ds.elem.m_ShaderBottles[x].GetShaderPtrHolder();
				_ASSERTE(NULL != ptrHolder);
				g_GetShaderManager().ReloadWithMacroModification(*ptrHolder, ds.arg.macroArray);
			}
		}

		
	}
Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}
extern KG3DDefaultShaderGroup& g_GetDefaultShaderGroup()
{
	static KG3DDefaultShaderGroupSolid stInstance;
	return stInstance;
}
HRESULT KG3DDefaultShaderGroupSolid::LoadAllShaders()
{
	//把所有shader加载一遍
	for(size_t i = 0; i < m_vecShaders.size(); ++i)
	{
		KG3DDefaultShaderRegistrationArg& argTmp = m_vecShaders[i].arg;
		DWORD dwType = argTmp.dwType;
		KG3DShaderHolder shTmp = this->GetDefaultShader(dwType);
		_ASSERTE(Private::IsShaderHolderInited(shTmp));
	}
	return S_OK;
}
#ifdef KG_ENABLE_TDD
KG_TEST_CLASS_TEST_BEGIN(KG3DDefaultShaderGroupSolid)
{
	_ASSERTE(D3DPS_VERSION(1,1) < D3DPS_VERSION(2, 0));
	_ASSERTE(D3DPS_VERSION(2,0) < D3DPS_VERSION(3, 0));

	_ASSERTE(EM_MAX_SHADER_MODEL_SUPPORTED == KG3DEngineCaps::EM_MAX_SHADER_MODEL_SUPPORTED && _T("要改就一起改"));
	{
		//KG3DDefaultShaderGroupSolid testInstance;

		struct TestStruct 
		{
			DWORD dwInputVSVersion;
			UINT	  uShaderModel;
		};

		TestStruct testCases[] = 
		{
			{D3DPS_VERSION(0,0), 1},
			{D3DPS_VERSION(1,0), 1},
			{D3DPS_VERSION(1,1), 1},
			{D3DPS_VERSION(2,0), 2},
			{D3DPS_VERSION(2,1), 2},
			{D3DPS_VERSION(3,0), 3},
			{D3DPS_VERSION(4,0), 3},//暂时我们不支持4_0
		};

		D3DCAPS9 capsTemp;
		for (size_t i = 0; i < _countof(testCases); ++i)
		{
			capsTemp.PixelShaderVersion = testCases[i].dwInputVSVersion;
			UINT uSM = KG3DDefaultShaderGroupSolid::GetShaderModelToUsePrivate(capsTemp, 0);
			_ASSERTE(uSM == testCases[i].uShaderModel);
		}
		
		{
			capsTemp.PixelShaderVersion = D3DPS_VERSION(2,0);
			_ASSERTE(1 == KG3DDefaultShaderGroupSolid::GetShaderModelToUsePrivate(capsTemp, 1));
			_ASSERTE(3 == KG3DDefaultShaderGroupSolid::GetShaderModelToUsePrivate(capsTemp, 3));
			_ASSERTE(2 == KG3DDefaultShaderGroupSolid::GetShaderModelToUsePrivate(capsTemp, -1));
			_ASSERTE(2 == KG3DDefaultShaderGroupSolid::GetShaderModelToUsePrivate(capsTemp, EM_MAX_SHADER_MODEL_SUPPORTED+1));
		}
	}

}
KG_TEST_CLASS_TEST_END(KG3DDefaultShaderGroupSolid)
#endif