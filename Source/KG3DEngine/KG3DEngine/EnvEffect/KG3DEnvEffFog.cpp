
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffFog.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-1-15 10:55:39
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

///编译宏选择
#include "KG3DEnvEffCommonDef.h"
//#include "KG3DGraphicsTool.h"
#include "KG3DEnvEffFog.h"
#include "KGINIFileHelpers.h"
#include "MathTool.h"
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG3D_ENVIRONMENT_EFFECT
{
	
	KG3DEnvEffFog::KG3DEnvEffFog()
		:KG3DEnvEffect(0, KG3DTYPE_ENVEFF_FOG)
		,m_bEnableBlending(FALSE)
		,m_fBlendPercent(0)
		,m_fDistancePercent(1)
		,m_bIsUnderFullControl(false)
	{
	}

	KG3DEnvEffFog::~KG3DEnvEffFog()
	{
		if(NULL != g_pd3dDevice)	//这个判断是需要的，因为在TDD里面有用
			g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);///不关掉的话，再次新建的时候会错误的显示Fog
	}
	HRESULT KG3DEnvEffFog::Render(const KG3DEnvEffRenderContext& renderContext)
	{
		//_ASSERTE(NULL && _T("不应该被调用"));
		return E_NOTIMPL;
	}

	HRESULT KG3DEnvEffFog::FrameMove()
	{
		return S_OK;
	}

	HRESULT KG3DEnvEffFog::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		_ASSERTE(pIniFile);
		m_DescStruct.SaveMapSetting(pIniFile, "SceneFog");
		return S_OK;
	}

	HRESULT KG3DEnvEffFog::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		this->GetCaps(m_CapsStruct);
		if (! pIniFile)
		{
			this->Apply();
			return S_OK;
		}		
		KG3DEnvEffFogDescStruct tempDescStruct;
		HRESULT hr = E_FAIL;
		if (tempDescStruct.LoadMapSetting(pIniFile, "SceneFog"))
		{
			hr = RequestModification(tempDescStruct, KG3DENVEFFFOG_MDO_ALL);
			if (SUCCEEDED(hr))
			{
				this->Apply();
			}
		}
		_KG3D_DEBUG_FAIL_REPORT_MSG(SUCCEEDED(hr), _STRINGER(KG3DEnvEffFog)_T(":")_STRINGER(LoadMapSetting));		
		return hr;
	}
	HRESULT KG3DEnvEffFog::Apply()
	{
		if (!m_bEnableBlending)
		{
			m_DescStruct.ToKey(&m_curKey);
		}
		else
		{
			///<DW_Frame>

			KG3DDWGC_Key_Fog BeforeKey;
			m_DescStruct.ToKey(&BeforeKey);
			m_curKey.CalculateInterp(BeforeKey, m_SubKey, m_fBlendPercent);
		}

		//外部请求全权控制Fog，内部就不做事
		KG_PROCESS_SUCCESS(m_bIsUnderFullControl);
		g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, m_DescStruct.m_bFogEnable);		

		FLOAT fFogEndUse = CalFogEnd(m_curKey.m_fFogEndValue, m_fDistancePercent);

		g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, m_curKey.m_FogColor);
		g_pd3dDevice->SetRenderState(D3DRS_FOGSTART,   FtoDW(m_curKey.m_fFogStartValue));
		g_pd3dDevice->SetRenderState(D3DRS_FOGEND,     FtoDW(fFogEndUse));

		g_pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, FtoDW(m_DescStruct.m_fFogDensity));
		
		if (m_DescStruct.m_bUsingTableFog)
		{
			g_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE,  D3DFOG_NONE);
			g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE,   m_DescStruct.m_nFogMode);
			///W_FOG是自动的
		}
		else
		{
			g_pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE,   D3DFOG_NONE);
			g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE,  m_DescStruct.m_nFogMode);
			g_pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, m_DescStruct.m_bRangeBasedFog);
		}

Exit1:
		return S_OK;
	}
	
	HRESULT KG3DEnvEffFog::RequestModification( const KG3DEnvEffFogDescStruct& DescStruct, DWORD dwModificationOption)
	{
		if (dwModificationOption & KG3DENVEFFFOG_MDO_FOG_BASE)
		{
			m_DescStruct.m_bFogEnable = DescStruct.m_bFogEnable;
			if (!m_DescStruct.m_bFogEnable)
			{
				NULL;
			}
			m_DescStruct.m_nFogMode = DescStruct.m_nFogMode;
			m_DescStruct.m_bUsingTableFog = DescStruct.m_bUsingTableFog;
		}
		if (dwModificationOption & KG3DENVEFFFOG_MDO_FOG_COLOR)
		{
			//SetFogColor(DescStruct.m_FogColor);
			m_DescStruct.m_FogColor = DescStruct.m_FogColor;
		}
		if ((dwModificationOption & KG3DENVEFFFOG_MDO_TABLE_FOG)&&m_CapsStruct.m_bCanDoTableFog)///即PixelFog
		{
			if (m_CapsStruct.m_bCanDoWFog)
			{
				m_DescStruct.m_bDeviceUsesWFog = DescStruct.m_bDeviceUsesWFog;
			}		
		}
		if (dwModificationOption & KG3DENVEFFFOG_MDO_VERTEX_FOG)///VertexFog是一定可以的，不检查了
		{
			if (m_CapsStruct.m_bCanDoRangeBasedFog)
			{
				m_DescStruct.m_bRangeBasedFog = DescStruct.m_bRangeBasedFog;
			}		
		}
		if(dwModificationOption & KG3DENVEFFFOG_MDO_MODE_PARAM)
		{
			KG_PROCESS_ERROR(DescStruct.m_fFogEndValue > DescStruct.m_fFogStartValue);
			m_DescStruct.m_fFogStartValue = DescStruct.m_fFogStartValue;
			m_DescStruct.m_fFogEndValue = DescStruct.m_fFogEndValue;
			m_DescStruct.m_fFogDensity = DescStruct.m_fFogDensity;
		}
		this->Apply();
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffFog::RequestModification( const KG3DENVEFFFOG_DESCSTRUCT* pDescStruct, DWORD dwModificationOption )
	{
		return RequestModification(*pDescStruct, dwModificationOption);
	}

	const KG3DEnvEffFogDescStruct* KG3DEnvEffFog::GetStruct()
	{
		return &m_DescStruct;
	}

	HRESULT KG3DEnvEffFog::GetStruct( KG3DENVEFFFOG_DESCSTRUCT* pDescStruct )
	{
		KG_PROCESS_ERROR(pDescStruct);
		m_DescStruct.ToBaseStruct(*pDescStruct);
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffFog::GetCaps( KG3DENVEFFFOG_CAPS_STRUCT& CapsStruct )
	{
		D3DCAPS9 TempCaps;
		KG_COM_PROCESS_ERROR(g_pd3dDevice->GetDeviceCaps(&TempCaps));
		DWORD dwRasterCaps = TempCaps.RasterCaps;
		if (dwRasterCaps & D3DPRASTERCAPS_FOGTABLE)
		{
			CapsStruct.m_bCanDoTableFog = TRUE;
		}
		if (dwRasterCaps & D3DPRASTERCAPS_FOGVERTEX)
		{
			CapsStruct.m_bCanDoVertexFog =TRUE;
		}
		if (dwRasterCaps & D3DPRASTERCAPS_WFOG)
		{
			CapsStruct.m_bCanDoWFog = TRUE;
		}
		if (dwRasterCaps & D3DPRASTERCAPS_FOGRANGE)
		{
			CapsStruct.m_bCanDoRangeBasedFog = TRUE;
		}
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffFog::GetCaps( KG3DENVEFFFOG_CAPS_STRUCT* pRetCaps )
	{
		KG_PROCESS_ERROR(pRetCaps);
		*pRetCaps = m_CapsStruct;
		return S_OK;
Exit0:
		return E_FAIL;
	}
	HRESULT KG3DEnvEffFog::GetFogColor( D3DCOLOR* pRetColor )
	{
		KGLOG_PROCESS_ERROR(pRetColor);
		*pRetColor = m_DescStruct.m_FogColor;
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffFog::SetFogColor( D3DCOLOR Color )
	{
		m_DescStruct.m_FogColor = Color;
		this->Apply();
		//PostMessage(KG3DMessage(KM_DATA_SYNCHRONIZE, (DWORD)(Color)));
		//g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR,  m_DescStruct.m_FogColor);
		return S_OK;
	}

	HRESULT KG3DEnvEffFog::ToSolidInterface( LPVOID* pEffectInterface )
	{
		if (!pEffectInterface)
		{
			return E_FAIL;
		}
		IEKG3DEnvEffFog* pInterface = this;
		*pEffectInterface = (LPVOID)pInterface;
		return S_OK;
	}
	///动态天气部分


	HRESULT KG3DEnvEffFog::DWGCRequestPopFrontState(float fCurBlendPercent/* = 0.0f*/)
	{
		if (fCurBlendPercent > 0 && fCurBlendPercent < 1.0f)
		{
			KG3DDWGC_Key_Fog RetKey, BeforeKey;
			m_DescStruct.ToKey(&BeforeKey);
			RetKey.CalculateInterp(BeforeKey, m_SubKey, fCurBlendPercent);

			m_DescStruct.SynWithKey(RetKey);
		}
		else
		{
			KG3DEnvEffFogDescStruct TempStruct = m_DescStruct;
			m_DescStruct.SynWithKey(m_SubKey);
		}
		_KG3D_DEBUG_SELF_OUTPUT0(_STRINGER(KG3DEnvEffFog)":"_STRINGER(DWGCRequestPopFrontState)"\n");
		return S_OK;
	}

	HRESULT KG3DEnvEffFog::DWGCRequestSubStateChange( const KG3DDWGC_KeyBase* pRetKey)
	{
		_KG3D_DEBUG_SELF_OUTPUT0(_STRINGER(KG3DEnvEffFog)":"_STRINGER(DWGCRequestSubStateChange)"\n");
		const TypeKey* pSolidKey = dynamic_cast<const TypeKey*>(pRetKey);
		KG_PROCESS_ERROR(pSolidKey);
		m_SubKey = *pSolidKey;
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffFog::DWGCGetCurStateToKey( KG3DDWGC_KeyBase* pRetKey)
	{
		TypeKey* pSolidKey = dynamic_cast<TypeKey*>(pRetKey);
		KG_PROCESS_ERROR(pSolidKey);
		{
			m_DescStruct.ToKey(pSolidKey);
			return S_OK;
		}
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffFog::DWGCEnableSubState( BOOL bEnable )
	{
		m_bEnableBlending = bEnable;
		return S_OK;
	}

	HRESULT KG3DEnvEffFog::DWGCSetPercent( float fPercent )
	{
		m_fBlendPercent = fPercent;
		return S_OK;
	}

	HRESULT KG3DEnvEffFog::DWGCRequestModification( const KG3DDWGC_KeyBase* pKey)
	{
		const KG3DDWGC_Key_Fog* pKeyFog = dynamic_cast<const KG3DDWGC_Key_Fog*>(pKey);
		if(!pKeyFog)
			return E_FAIL;
		KG3DEnvEffFogDescStruct TempStruct = m_DescStruct;
		TempStruct.SynWithKey(*pKeyFog);
		return RequestModification(TempStruct, KG3DENVEFFFOG_MDO_ALL);
	}

	HRESULT KG3DEnvEffFog::SetFogParam( const KG3DFogParam& Param )
	{
		KG3DENVEFFFOG_DESCSTRUCT Struct;

		Struct.m_bFogEnable = Param.bEnable;
		Struct.m_bUsingTableFog = Param.bUseTableFog;
		Struct.m_nFogMode = D3DFOG_LINEAR;

		Struct.m_fFogStartValue = Param.fStart;
		Struct.m_fFogEndValue = Param.fEnd;
		_ASSERTE(Param.fEnd > Param.fStart);

		Struct.m_fFogDensity = Param.fDensityDevidedBy_endMinusStart * (Param.fEnd - Param.fStart);
		Struct.m_FogColor = Param.vColor;

		DWORD dwMDO = KG3DENVEFFFOG_MDO_FOG_BASE | KG3DENVEFFFOG_MDO_FOG_COLOR | KG3DENVEFFFOG_MDO_MODE_PARAM;

		HRESULT hr = RequestModification(&Struct, dwMDO);
		return hr;
	}

	KG3DFogParam KG3DEnvEffFog::GetFogParam()	//这个还满复杂的
	{
		const KG3DEnvEffFogDescStruct* p = this->GetStruct();
		_ASSERTE(NULL != p);

		KG3DFogParam fogParam;
		fogParam.bEnable = p->m_bFogEnable;
		fogParam.bUseTableFog = p->m_bUsingTableFog;
		fogParam.nFogMode = p->m_nFogMode;

		FLOAT fEnd = CalFogEnd(m_curKey.m_fFogEndValue, m_fDistancePercent);
		//fogParam.vStartEndDensityNull = D3DXVECTOR4(m_curKey.m_fFogStartValue, fEnd, p->m_fFogDensity, 0);	//Density没有关键帧

		fogParam.vColor = m_curKey.m_FogColor;
		fogParam.fStart = m_curKey.m_fFogStartValue;
		_ASSERTE(fEnd > m_curKey.m_fFogStartValue);

		if(fEnd <= m_curKey.m_fFogStartValue)
			fogParam.fDensityDevidedBy_endMinusStart = 0;
		else
			fogParam.fDensityDevidedBy_endMinusStart = p->m_fFogDensity / (fEnd - m_curKey.m_fFogStartValue);
		
		fogParam.fEnd = fEnd;
		return fogParam;
	}

	HRESULT KG3DEnvEffFog::SetDistancePercent( FLOAT fPercent )
	{
		if(fPercent < FLT_EPSILON)
			return E_FAIL;
		m_fDistancePercent = fPercent;
		return S_OK;
	}

	HRESULT KG3DEnvEffFog::DemandFullControl()
	{
		m_bIsUnderFullControl = true;
		
		return S_OK;
	}

	ULONG KG3DEnvEffFog::IsInFullControl()
	{
		return m_bIsUnderFullControl;
	}

	HRESULT KG3DEnvEffFog::ReleaseFullControl()
	{
		m_bIsUnderFullControl = false;
		return S_OK;
	}

	FLOAT KG3DEnvEffFog::CalFogEnd(FLOAT fEnd, FLOAT fDistancePercent)
	{
		return fDistancePercent > 1 ? fEnd * fDistancePercent 
			: fEnd;	//只有远景比较远才放缩，如果小于1就维持默认
	}

	KG3DFogParam KG3DEnvEffFog::GetDefaultFogParam()
	{
		static bool s_bIsFogParamInited = false;
		static KG3DFogParam s_fogParamDefault;
		if(! s_bIsFogParamInited)
		{
			s_bIsFogParamInited = true;

			KG3DEnvEffFogDescStruct defaultDesc;
			s_fogParamDefault.bEnable = defaultDesc.m_bFogEnable;
			s_fogParamDefault.nFogMode = defaultDesc.m_nFogMode;
			s_fogParamDefault.bUseTableFog = defaultDesc.m_bUsingTableFog;
			
			s_fogParamDefault.vColor = Color2Value(defaultDesc.m_FogColor);

			//s_fogParamDefault.vStartEndDensityNull = D3DXVECTOR4(defaultDesc.m_fFogStartValue, defaultDesc.m_fFogEndValue, defaultDesc.m_fFogDensity, 0);

			s_fogParamDefault.fStart = defaultDesc.m_fFogStartValue;
			s_fogParamDefault.fEnd = defaultDesc.m_fFogEndValue;
			_ASSERTE(defaultDesc.m_fFogEndValue > defaultDesc.m_fFogStartValue);
			s_fogParamDefault.fDensityDevidedBy_endMinusStart = defaultDesc.m_fFogDensity / (defaultDesc.m_fFogEndValue - defaultDesc.m_fFogStartValue);
		}
		return s_fogParamDefault;
	}
	/************************************************************************/
	// KG3DEnvEffFogDescStruct定义
	/************************************************************************/
	KG3DEnvEffFogDescStruct::KG3DEnvEffFogDescStruct()
	{
		///下面是默认的Fog参数
		m_bFogEnable = FALSE;

		m_FogColor             = 0xffffffff/*0x00FCDFFD*/;
		m_nFogMode             = D3DFOG_LINEAR;

		m_bDeviceUsesWFog      = FALSE;
		m_bRangeBasedFog       = FALSE;
		m_bUsingTableFog       = FALSE;

		m_fFogStartValue       = 1000.0f;
		m_fFogEndValue         = 8000.0f;
		m_fFogDensity          = 1.f;
	}

	KG3DEnvEffFogDescStruct::KG3DEnvEffFogDescStruct( const KG3DEnvEffFogDescStruct& OtherStruct )
	{
		KG3DENVEFFFOG_DESCSTRUCT* pMyBase = this;
		const KG3DENVEFFFOG_DESCSTRUCT* pOther = &OtherStruct;
		memcpy_s(pMyBase, sizeof(KG3DENVEFFFOG_DESCSTRUCT)
			, pOther, sizeof(KG3DENVEFFFOG_DESCSTRUCT));
	}

	KG3DEnvEffFogDescStruct::KG3DEnvEffFogDescStruct( const KG3DENVEFFFOG_DESCSTRUCT& OtherStruct )
	{
		KG3DENVEFFFOG_DESCSTRUCT* pMyBase = this;
		const KG3DENVEFFFOG_DESCSTRUCT* pOther = &OtherStruct;
		memcpy_s(pMyBase, sizeof(KG3DENVEFFFOG_DESCSTRUCT)
			, pOther, sizeof(KG3DENVEFFFOG_DESCSTRUCT));
	}

	void KG3DEnvEffFogDescStruct::operator=( const KG3DEnvEffFogDescStruct& OtherStruct )
	{
		KG3DENVEFFFOG_DESCSTRUCT* pMyBase = this;
		const KG3DENVEFFFOG_DESCSTRUCT* pOther = &OtherStruct;
		memcpy_s(pMyBase, sizeof(KG3DENVEFFFOG_DESCSTRUCT)
			, pOther, sizeof(KG3DENVEFFFOG_DESCSTRUCT));
	}

	void KG3DEnvEffFogDescStruct::operator=( const KG3DENVEFFFOG_DESCSTRUCT& OtherStruct )
	{
		KG3DENVEFFFOG_DESCSTRUCT* pMyBase = this;
		const KG3DENVEFFFOG_DESCSTRUCT* pOther = &OtherStruct;
		memcpy_s(pMyBase, sizeof(KG3DENVEFFFOG_DESCSTRUCT)
			, pOther, sizeof(KG3DENVEFFFOG_DESCSTRUCT));
	}

	void KG3DEnvEffFogDescStruct::ToBaseStruct( KG3DENVEFFFOG_DESCSTRUCT& OtherStruct )
	{
		KG3DENVEFFFOG_DESCSTRUCT* pMyBase = this;
		KG3DENVEFFFOG_DESCSTRUCT* pOther = &OtherStruct;
		memcpy_s(pOther, sizeof(KG3DENVEFFFOG_DESCSTRUCT)
			, pMyBase, sizeof(KG3DENVEFFFOG_DESCSTRUCT));
	}

	bool KG3DEnvEffFogDescStruct::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		_ASSERTE(pIniFile&&pSection);
		pIniFile->WriteInteger(
			pSection, "bFogEnable", m_bFogEnable ? 1 : 0
			);

		KG_CUSTOM_HELPERS::SaveINIFileData<D3DCOLOR>(pIniFile, pSection, "dwFogColo", m_FogColor);
		
		pIniFile->WriteInteger(pSection, "bDeviceUsesWFog", m_bDeviceUsesWFog ? 1 : 0);
		pIniFile->WriteInteger(pSection, "bRangeBasedFog", m_bRangeBasedFog ? 1 : 0);
		pIniFile->WriteInteger(	pSection, "bUsingTableFog", m_bUsingTableFog ? 1 : 0);
		switch(m_nFogMode)
		{
		case D3DFOG_LINEAR : 
			pIniFile->WriteString(pSection, "nFogMode", "D3DFOG_LINEAR");
			break;
		case D3DFOG_NONE :
			pIniFile->WriteString(pSection, "nFogMode", "D3DFOG_NONE");
			break;
		case D3DFOG_EXP :
			pIniFile->WriteString(pSection, "nFogMode", "D3DFOG_EXP");
			break;
		case D3DFOG_EXP2 :  
			pIniFile->WriteString(pSection, "nFogMode", "D3DFOG_EXP2");
			break;	
		case D3DFOG_FORCE_DWORD :    
			pIniFile->WriteString(pSection, "nFogMode", "D3DFOG_FORCE_DWORD");
			break;
		default:;
		}
		pIniFile->WriteFloat(pSection, "fFogStartValue", m_fFogStartValue);
		pIniFile->WriteFloat(pSection, "fFogEndValue", m_fFogEndValue);
		//pIniFile->WriteFloat(pSection, "fFogDensity", m_fFogDensity);
		return true;
	}

	bool KG3DEnvEffFogDescStruct::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		_ASSERTE(pIniFile&&pSection);
		char	szTexture[256];

		KG_CUSTOM_HELPERS::LoadINIFileData<BOOL>(pIniFile, pSection, "bFogEnable", TRUE, &m_bFogEnable);
		KG_CUSTOM_HELPERS::LoadINIFileData<D3DCOLOR>(pIniFile, pSection, "dwFogColo", 0x00000000, &m_FogColor);		
		KG_CUSTOM_HELPERS::LoadINIFileData<BOOL>(pIniFile, pSection, "bDeviceUsesWFog", TRUE, &m_bDeviceUsesWFog);
		KG_CUSTOM_HELPERS::LoadINIFileData<BOOL>(pIniFile, pSection, "bRangeBasedFog", TRUE, &m_bRangeBasedFog);
		KG_CUSTOM_HELPERS::LoadINIFileData<BOOL>(pIniFile, pSection, "bUsingTableFog", TRUE, &m_bUsingTableFog);
		
		pIniFile->GetString(pSection, "nFogMode", "D3DFOG_LINEAR", szTexture, sizeof(szTexture));
		if (!_stricmp("D3DFOG_LINEAR", szTexture))
		{
			m_nFogMode = D3DFOG_LINEAR;
		}
		else if (!_stricmp("D3DFOG_NONE", szTexture))
		{
			m_nFogMode = D3DFOG_NONE;
		}
		else if (!_stricmp("D3DFOG_EXP", szTexture))
		{
			m_nFogMode = D3DFOG_EXP;
		}
		else if (!_stricmp("D3DFOG_EXP2", szTexture))
		{
			m_nFogMode = D3DFOG_EXP2;
		}		
		else if (!_stricmp("D3DFOG_FORCE_DWORD", szTexture))
		{
			m_nFogMode = D3DFOG_FORCE_DWORD;
		}
		pIniFile->GetFloat(pSection, "fFogStartValue",1000.0f, &m_fFogStartValue);
		pIniFile->GetFloat(pSection, "fFogEndValue", 8000.0f, &m_fFogEndValue);
		//pIniFile->GetFloat(pSection, "fFogDensity", 0.5f, &m_fFogDensity);
		return true;
	}

	void KG3DEnvEffFogDescStruct::ToKey( KG3DDWGC_Key_Fog* pKey )
	{
		_ASSERTE(pKey);
		pKey->m_FogColor = m_FogColor;	
		pKey->m_fFogStartValue = m_fFogStartValue;
		pKey->m_fFogEndValue = m_fFogEndValue;
		//pKey->m_fFogDensity = m_fFogDensity;
	}

	void KG3DEnvEffFogDescStruct::SynWithKey(const KG3DDWGC_Key_Fog& Key )
	{
		m_FogColor = Key.m_FogColor;
		m_fFogStartValue = Key.m_fFogStartValue;
		m_fFogEndValue = Key.m_fFogEndValue;
	}
	/************************************************************************/
	//KG3DDWGC_Key_Fog                                               
	/************************************************************************/
	KG3DDWGC_Key_Fog::KG3DDWGC_Key_Fog()
	{
		m_FogColor = 0xffffffff;
		m_fFogStartValue       = 1000.0f;
		m_fFogEndValue         = 8000.0f;
		//m_fFogDensity          = 0.5f;
	}

	bool KG3DDWGC_Key_Fog::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		_ASSERTE(pIniFile&&pSection);	//下面dwFogColo天生就少写了一个r
		KG_CUSTOM_HELPERS::SaveINIFileData<D3DCOLOR>(pIniFile, pSection, _T("dwFogColo"), m_FogColor);
		pIniFile->WriteFloat(pSection, "fFogStartValue", m_fFogStartValue);
		pIniFile->WriteFloat(pSection, "fFogEndValue", m_fFogEndValue);
		//pIniFile->WriteFloat(pSection, "fFogDensity", m_fFogDensity);
		return true;
	}

	bool KG3DDWGC_Key_Fog::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
	{
		_ASSERTE(pIniFile&&pSection);
		KG_CUSTOM_HELPERS::LoadINIFileData<D3DCOLOR>(pIniFile, pSection, _T("dwFogColo"), 0x00000000, &m_FogColor);
		pIniFile->GetFloat(pSection, "fFogStartValue",1000.0f, &m_fFogStartValue);
		pIniFile->GetFloat(pSection, "fFogEndValue", 8000.0f, &m_fFogEndValue);
		//pIniFile->GetFloat(pSection, "fFogDensity", 0.5f, &m_fFogDensity);
		return true;
	}
	bool KG3DDWGC_Key_Fog::CalculateInterp( const KG3DDWGC_KeyBase& BaseKeyBefore
				, const KG3DDWGC_KeyBase& BaseKeyAfter
				, float fPercent) 
	{
		//留意Key结构，下面的这种强转不一定可以的
		KG3DDWGC_Key_Fog& FogBefore = (KG3DDWGC_Key_Fog&)BaseKeyBefore;
		KG3DDWGC_Key_Fog& FogAfter = (KG3DDWGC_Key_Fog&)BaseKeyAfter;
		
		TwoColorBlend(FogBefore.m_FogColor, FogAfter.m_FogColor, m_FogColor, fPercent);
		
		MT_FLOAT_BLEND(FogBefore.m_fFogStartValue, FogAfter.m_fFogStartValue, m_fFogStartValue, fPercent);
		MT_FLOAT_BLEND(FogBefore.m_fFogEndValue, FogAfter.m_fFogEndValue, m_fFogEndValue, fPercent);
		
		return true;
	}
	
#if defined(DEBUG) | defined(_DEBUG)
	void KG3DDWGC_Key_Fog::DebugPrint(std::ostringstream& OSS )
	{
		/*
		_ASSERTE(pDebugStream);
				char czTemp[32];
				sprintf_s(czTemp, 32, "FogColor:%x; ", m_FogColor);
				_tcscat_s(pDebugStream, nSize, czTemp);
				nSize-=(int)strlen(czTemp);		*/
		OSS << "C:" << m_FogColor;
	}
#endif

#ifdef KG_ENABLE_TDD
	KG_TEST_BEGIN(KG3DEnvEffFog)
	{
		KG3DEnvEffFog fogTemp;
		_ASSERTE(! fogTemp.IsInFullControl());
		KG3DFogParam	fogParam;
		fogTemp.DemandFullControl();
		_ASSERTE(fogTemp.IsInFullControl());
		fogTemp.ReleaseFullControl();
		_ASSERTE(! fogTemp.IsInFullControl());
	}
	KG_TEST_END(KG3DEnvEffFog)
#endif
};

