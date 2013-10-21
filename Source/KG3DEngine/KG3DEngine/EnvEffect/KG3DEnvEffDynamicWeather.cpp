////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDynamicWeather.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-12-5 15:28:06
//  Comment     :  
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

///编译宏选择
#include "KG3DEnvEffCommonDef.h"
#include "KG3DEnvEffDynamicWeather.h"
#include "KG3DMessenger.h"
#include "KG3DEnvEffDW_KeyTrack.h"
#include "KG3DTime.h"
#include "KG3DGraphicsTool.h"
#include "KG3DRegionInformer.h"
#include "KGINIFileHelpers.h"
#include "KG3DEngineManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
namespace KG3D_ENVIRONMENT_EFFECT
{
    KG3DEnvEffDynamicWeather::KG3DEnvEffDynamicWeather( KG3DEnvEffListBase* pEnvEffList )
        : KG3DEnvEffect(0, KG3DTYPE_ENVEFF_DYNAMIC_WEATHER)
        , m_pRegionInformer(NULL)
        , m_pEnvEffList(pEnvEffList)
        , m_nCurHour(KG3DENVEFFTI_HOUR_MIN)
        , m_nCurRegion(KG3DENVEFFRI_REGION_MIN)
        , m_KeyTrack(*pEnvEffList)
        , m_dwState(0)
    {
        _ASSERTE(m_pEnvEffList);
    }

	KG3DEnvEffDynamicWeather::~KG3DEnvEffDynamicWeather()
	{
		SAFE_DELETE(m_pRegionInformer);
		SAFE_DELETE(m_pTimeInformer);
	}

	HRESULT KG3DEnvEffDynamicWeather::Render(const KG3DEnvEffRenderContext& renderContext)
	{
		if (!KG3DEnvEffect::IsEnable())
		{
			return E_FAIL;
		}
		return S_OK;
	}

	HRESULT	KG3DEnvEffDynamicWeather::FrameMove()
	{
		if (!KG3DEnvEffect::IsEnable())
		{
			return E_FAIL;
		}		
		
		_ASSERTE(NULL != m_pTimeInformer);
		m_pTimeInformer->FrameMove();

		if (m_pRegionInformer)
		{
			m_pRegionInformer->FrameMove();
		}

        m_DWController.FrameMove();
		return S_OK;
	}

	void KG3DEnvEffDynamicWeather::OnEnterHour(DW_HOUR nHour)
	{
		if (m_nCurHour == nHour)
		{
			_KG3D_DEBUG_SELF_OUTPUT1("DW:重复进入一个Hour:%d\n", nHour);
			return;
		}

		if (CheckIsHourIndexValid(nHour))
		{
			//m_KeyTrack0.OnEnterHour(nHour);
			_KG3D_DEBUG_SELF_OUTPUT1("DW:OnEnterHour:%d\n", nHour);
			m_nCurHour = nHour;
#if defined(_DEBUG) | defined(DEBUG)
			bool bRet = DistributeInterpKeys(m_nCurRegion, m_nCurHour, KG3DEnvEffDWController_MDO_GAP_H2H);
			_KG3D_DEBUG_FAIL_REPORT_MSG(bRet, _T("分发关键帧失败"));
#else
			DistributeInterpKeys(m_nCurRegion, m_nCurHour, KG3DEnvEffDWController_MDO_GAP_H2H);
#endif
		}
	}

	void KG3DEnvEffDynamicWeather::OnEnterRegion( DW_REGION_TAB nRegion )
	{	
		if (m_nCurRegion == nRegion)
		{
			_KG3D_DEBUG_SELF_OUTPUT1("DW:重复进入一个Region:%d\n", nRegion);
			return;
		}
		if (CheckIsRegionIndexValid(nRegion) && m_KeyTrack.IsRegionAllowed(nRegion))
		{
			_KG3D_DEBUG_SELF_OUTPUT1("DW:OnEnterRegion:%d\n", nRegion);
			m_nCurRegion = nRegion;
#if defined(_DEBUG) | defined(DEBUG)
			bool bRet = 
#endif
				DistributeInterpKeys(m_nCurRegion, m_nCurHour, KG3DEnvEffDWController_MDO_GAP_R2R);
			_KG3D_DEBUG_FAIL_REPORT_MSG(bRet, _T("分发关键帧失败"));
		}
	}
	HRESULT KG3DEnvEffDynamicWeather::SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection)
	{
		m_KeyTrack.SaveMapSetting(pIniFile, NULL);
		_ASSERTE(NULL != m_pTimeInformer);
		m_pTimeInformer->SaveMapSetting(pIniFile);

		if (m_pRegionInformer)
		{
			m_pRegionInformer->SaveMapSetting(pIniFile, _T("KG3DEnvEffRegionInformer"));
		}
		return S_OK;
	}
	HRESULT KG3DEnvEffDynamicWeather::LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection)
	{
		{
			//初始化TimeInformer
			KG3DTimeClock& timeClock = g_cEngineManager.GetTimeClock();

			KG3DMessageReceiver* pReceiver = this;
			m_pTimeInformer = KG3DEnvEffTimeInformer::Build(NULL, reinterpret_cast<DWORD_PTR>(&timeClock), reinterpret_cast<DWORD_PTR>(pReceiver));
			KG_PROCESS_ERROR(NULL != m_pTimeInformer);

			//初始化RegionInformer
			m_pRegionInformer = new KG3DRegionInformer;
			if (m_pRegionInformer)
			{
				m_pRegionInformer->RegisterReceiver(this);
				KG3DRegionInfoManager* pRM =  g_cGraphicsTool.GetSceneRegionDWInfoManager();
				if (pRM)
				{
					m_pRegionInformer->BindManager(*pRM);
				}
				KG3DGPSBase* pGPS = g_cGraphicsTool.GetSceneCurGPS();
				if (pGPS)
				{
					m_pRegionInformer->AcceptGPS(*pGPS);
				}	
			}		
		}

		if (! pIniFile)
		{
			if (m_pRegionInformer)
				m_pRegionInformer->LoadMapSetting(NULL, NULL);
			_ASSERTE(NULL != m_pTimeInformer);
			m_pTimeInformer->LoadMapSetting(NULL);
			this->InitRegionAndHourState();
			return S_OK;
		}
		KG_COM_PROCESS_ERROR(m_KeyTrack.LoadMapSetting(pIniFile, NULL));
		{
			_ASSERTE(NULL != m_pTimeInformer);
			HRESULT hr = m_pTimeInformer->LoadMapSetting(pIniFile);
			_KG3D_DEBUG_FAIL_REPORT_MSG(SUCCEEDED(hr), "动态天气TimeInformer加载失败");
			KGLOG_PROCESS_ERROR(SUCCEEDED(hr) && "动态天气TimeInformer加载失败");
		}
		if (m_pRegionInformer)
		{
			HRESULT hr = m_pRegionInformer->LoadMapSetting(pIniFile, "KG3DEnvEffRegionInformer");
			_KG3D_DEBUG_FAIL_REPORT_MSG(SUCCEEDED(hr), "动态天气RegionInformer加载失败");
			KGLOG_PROCESS_ERROR(SUCCEEDED(hr) && "动态天气RegionInformer加载失败");
		}

		//////////////////////////////////////////////////////////////////////////
		{
			INT nBlendGapHour2Hour = KG3DEnvEffDWController_MDO_GAP_H2H;
			INT nBlendGapRegion2Region = KG3DEnvEffDWController_MDO_GAP_R2R;
			if (KG_CUSTOM_HELPERS::LoadINIFileData<INT>(pIniFile, _T("DynamicWeather"), _T("BlendGapHour2Hour_s"), DWGC_BLEND_GAP_HOUR2HOUR_SECOND, &nBlendGapHour2Hour)
				&&KG_CUSTOM_HELPERS::LoadINIFileData<INT>(pIniFile, _T("DynamicWeather"), _T("BlendGapRegion2Region_s"), DWGC_BLEND_GAP_REGION2REGION_SECOND, &nBlendGapRegion2Region)
				)
			{
				if (nBlendGapHour2Hour > 1 && nBlendGapRegion2Region > 1)///1还是不正确的
				{
					KG3DEnvEffDWControllerDescStruct TempStruct;
					TempStruct.m_fBlendGapH2H_ms = static_cast<float>(nBlendGapHour2Hour * 1000);
					TempStruct.m_fBlendGapR2R_ms = static_cast<float>(nBlendGapRegion2Region * 1000);
					TempStruct.m_dwModificationOption = KG3DEnvEffDWController_MDO_GAP_H2H | KG3DEnvEffDWController_MDO_GAP_R2R;
					m_DWController.RequestModification(TempStruct);
				}
			}
		}

		InitRegionAndHourState();

		//TimeInformer的Boost和Controller的Boost应该一致
		{
			KG3DENVEFFTI_DESCSTRUCT Desc;
			_ASSERTE(NULL != m_pTimeInformer);
			HRESULT hr = m_pTimeInformer->GetStruct(&Desc);
			if (SUCCEEDED(hr))
			{
				KG3DEnvEffDWControllerDescStruct TempStruct;
				TempStruct.m_fBlendBoost = Desc.m_fBoostFactor;
				TempStruct.m_dwModificationOption = KG3DEnvEffDWController_MDO_GAP_BOOST;
				m_DWController.RequestModification(TempStruct);
			}
		}

		{
			BOOL bTemp = FALSE;
			pIniFile->GetInteger("DynamicWeather", "BoostOnStart", 0, &bTemp);
			if (bTemp)
			{
				//一开始就使用动态天气
				this->EnablePlay(TRUE);				

				this->StartQuickPlay(m_nCurRegion, m_nCurHour, NULL);
			}
		}
		return S_OK;
Exit0:
		_KG3D_DEBUG_SELF_REPORT(_STRINGER(KG3DEnvEffDynamicWeather::LoadMapSetting)"失败");
		InitRegionAndHourState();
		return E_FAIL;
	}
	HRESULT KG3DEnvEffDynamicWeather::InitRegionAndHourState()
	{
		//用RegionInformer中的信息出事化当前Region
		INT nIndex = -1;
		_KG3D_DEBUG_SELF_ASSERTE(m_pRegionInformer);
		if (m_pRegionInformer)
		{
			nIndex = m_pRegionInformer->GetRegionTraitByGPS();	///主动的查询当前Region位置
		}
		if (nIndex >= 0 && m_KeyTrack.IsRegionAllowed(nIndex))//m_KeyTrack0.IsRegionIndexExist(nIndex))
		{
			m_nCurRegion = nIndex;///如果能够从RegionInformer得到当前Region的Index，且关键帧表中存在，那么就用
		}
		else///退而求其次，用关键帧表中最小Region
		{
			_KG3D_DEBUG_SELF_REPORT("动态天气：无法得到正确的当前Region，尝试用关键帧表中的第一个Region序列代替\
				，检查关键帧表中Region的序号");
			INT FirstIndexInKeyTrack = DW_KEYINDEX_INVALID;
			HRESULT hr = m_KeyTrack.GetFirstRegionIndex(&FirstIndexInKeyTrack);
			//BOOL bRet = m_KeyTrack0.GetMinRegionIndex(nIndex);
			if(SUCCEEDED(hr) && CheckIsRegionIndexValid(FirstIndexInKeyTrack))
			{				
				m_nCurRegion = FirstIndexInKeyTrack;				
			}
			else
			{
				_KG3D_DEBUG_SELF_REPORT("动态天气初始化当前Region失败，当前Region被设为无效，动态天气被锁定");
				m_nCurRegion = DW_KEYINDEX_INVALID;///锁住动态天气
			}
		}
		_KG3D_DEBUG_SELF_OUTPUT1(_STRINGER(KG3DEnvEffDynamicWeather)_T("初始化的当前Region：%d\n"), m_nCurRegion);
		
		//用TimeInformer的信息初始化当前Hour
		_ASSERTE(NULL != m_pTimeInformer);
		INT nHour = m_pTimeInformer->GetCurHour();
		if (nHour >= KG3DENVEFFTI_HOUR_MIN && nHour <= KG3DENVEFFTI_HOUR_MAX)
		{
			m_nCurHour = nHour;
		}
		else
		{
			_KG3D_DEBUG_SELF_REPORT("无法初始化时间\n");
			m_nCurHour = 7;
		}
		///这里应该全局的Modification
		{
			m_DWController.SetBindedControlObjectAndInitControlState(this);
#if defined(_DEBUG) | defined(DEBUG)
			HRESULT hr =
#endif
				this->EnablePlay(TRUE);
#if defined(_DEBUG) | defined(DEBUG)
			if (FAILED(hr))
			{
				_KG3D_DEBUG_SELF_REPORT("无法EnblePlay\n");
			}
#endif
		}

		_KG3D_DEBUG_SELF_OUTPUT1(_STRINGER(KG3DEnvEffDynamicWeather)_T("初始化的当前Hour：%d\n"), m_nCurHour);
		return S_OK;
	}
	void __stdcall KG3DEnvEffDynamicWeather::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
	{
		switch(Message.m_uMessage)
		{				
		case KM_TIME_ON_REACH_HOUR:
			{
				INT HourToEnter = _kg_to_smaller_size_cast<INT>(Message.m_dwWParam);
				//_KG3D_DEBUG_SELF_ASSERTE(! OnEnterRegion());
				OnEnterHour(HourToEnter);
				break;
			}
		case KM_REGION_CHANGED:
			{
				INT RegionToEnter = _kg_to_smaller_size_cast<INT>(Message.m_dwWParam);
				//_KG3D_DEBUG_SELF_ASSERTE(! OnEnterHour());
				OnEnterRegion(RegionToEnter);
				break;
			}
		}
	}	

	/************************************************************************/
	//接口定义开始
	/************************************************************************/

	HRESULT KG3DEnvEffDynamicWeather::RequestModification( const KG3DENVEFF_DYNWEATHER_DESCSTRUCT* pStruct )
	{
		return S_OK;
	}
	HRESULT KG3DEnvEffDynamicWeather::GetStruct( KG3DENVEFF_DYNWEATHER_DESCSTRUCT* pResultStruct )
	{
		return S_OK;
	}

	bool KG3DEnvEffDynamicWeather::DistributeInterpKeys( DW_REGION_TAB nRegion, DW_HOUR nHour, DWORD dwMDO, DWORD_PTR dwExtraData /*= NULL*/ )
	{
		KG_PROCESS_ERROR(IsEnablePlay());//锁定，如果在编辑中，那么就不能分派Key，从而也就没有关键帧无效的问题了
		{
			KG3DEnvEffDWController::BlendStruct TempStruct(KG3DENVEFF_DWGCKEY_HANDLE(nRegion, nHour), dwMDO, dwExtraData);
			m_DWController.SetNextBlendStruct(TempStruct);
			return true;
		}
Exit0:
		return false;
	}

	HRESULT KG3DEnvEffDynamicWeather::ToSolidInterface( LPVOID* pEffectInterface )
	{
		if (!pEffectInterface)
		{
			return E_FAIL;
		}
		IEKG3DEnvEffDynamicWeather* pInterface = this;
		*pEffectInterface = (LPVOID)pInterface;
		return S_OK;
	}	

	HRESULT KG3DEnvEffDynamicWeather::GetTimeInformer( IEKG3DEnvEffTimeInformer** ppTI )
	{
		KG_PROCESS_ERROR(ppTI);
		*ppTI = m_pTimeInformer;
		_ASSERTE(*ppTI);
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffDynamicWeather::GetRegionInformer( IEKG3DRegionInformer** ppRI )
	{
		KG_PROCESS_ERROR(ppRI && m_pRegionInformer);
		*ppRI = m_pRegionInformer;
		return S_OK;
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffDynamicWeather::StartQuickPlay( INT nRegion, INT nStartHour, DWORD_PTR dwExtraParam ) /*= 0*/
	{
		FLOAT fBoostFactor = KG3DENVEFFTI_MAX_BOOST_FACTOR;
		FLOAT fSystemBoost = g_cEngineManager.GetTimeClock().GetBoostFactor();
		_ASSERTE(0 != fSystemBoost);;
		fBoostFactor /= fSystemBoost;

		KG_PROCESS_ERROR(m_KeyTrack.IsRegionAllowed(nRegion) && CheckIsHourIndexValid(nStartHour) && IsEnablePlay());
		{
			HRESULT hr = E_FAIL;

			//改RI
			
			if(m_pRegionInformer)
			{
				hr = m_pRegionInformer->StopRegionChecking(nRegion);
				KG_COM_PROCESS_ERROR(hr);
			}

			//改TI
			KG3DEnvEffTI_DescStruct TIStruct;
			TIStruct.m_dwCurHour = nStartHour;
			TIStruct.m_fBoostFactor = fBoostFactor;
			DWORD dwTIMDO = KG3DENVEFFTI_MDO_BOOST_FACTOR | KG3DENVEFFTI_MDO_CUR_TIME;
			_ASSERTE(NULL != m_pTimeInformer);
			hr = m_pTimeInformer->RequestModification(&TIStruct, dwTIMDO);
			KG_COM_PROCESS_ERROR(hr);

			//改Controller
			KG3DEnvEffDWControllerDescStruct DWCtrlStruct;	///生成默认参数
			DWCtrlStruct.m_dwModificationOption = KG3DEnvEffDWController_MDO_GAP_BOOST;
			_ASSERTE(DWCtrlStruct.m_fBlendGapH2H_ms == DWGC_BLEND_GAP_HOUR2HOUR_SECOND* 1000);
			DWCtrlStruct.m_fBlendBoost = KG3DENVEFFTI_MAX_BOOST_FACTOR;	///加速播放的时候也要加速插值

			hr = m_DWController.RequestModification(DWCtrlStruct);
			KG_COM_PROCESS_ERROR(hr);

			m_nCurRegion= nRegion;
			m_nCurHour = nStartHour;
			return S_OK;
		}
Exit0:
		this->StopQuickPlay(NULL);
		return E_FAIL;
	}

	HRESULT KG3DEnvEffDynamicWeather::StopQuickPlay( DWORD_PTR dwExtraParam ) /*= 0*/
	{
		HRESULT hr = E_FAIL;

		//RI
		/*
		KG3DRegionInformerDescStruct RIStruct;
				_ASSERTE(RIStruct.m_nCurRegion == 1);
				RIStruct.m_nCurRegion = 0;	//把CurRegion设为0可以重新归位
				DWORD dwRIMDO = KG3DRI_MDO_CURRENT_REGION;*/
		
		if(m_pRegionInformer)
		{
			//hr = m_pRegionInformer->RequestModification(RIStruct, dwRIMDO);
			hr = m_pRegionInformer->ReStartRegionChecking();
			_ASSERTE(SUCCEEDED(hr));
		}

		//TI
		KG3DEnvEffTI_DescStruct TIStruct;
		_ASSERTE(TIStruct.m_dwCurHour == 0);
		_ASSERTE(TIStruct.m_fBoostFactor == KG3DEnvEffTI_DescStruct::em_default_boost_factor);
		DWORD dwTIMDO = KG3DENVEFFTI_MDO_BOOST_FACTOR; //| KG3DENVEFFTI_MDO_CUR_TIME;	//不还原时间
		_ASSERTE(NULL != m_pTimeInformer);
		hr = m_pTimeInformer->RequestModification(&TIStruct, dwTIMDO);
		_ASSERTE(SUCCEEDED(hr));

		m_pTimeInformer->StopTimeChecking(false);

		//Controller
		KG3DEnvEffDWControllerDescStruct DWCtrlStruct;	///生成默认参数
		DWCtrlStruct.m_dwModificationOption = KG3DEnvEffDWController_MDO_GAP_BOOST;
		_ASSERTE(DWCtrlStruct.m_fBlendGapH2H_ms == DWGC_BLEND_GAP_HOUR2HOUR_SECOND* KG3DENVEFFTI_MILLISECOND_IN_A_SECOND);
		hr = m_DWController.RequestModification(DWCtrlStruct);
		_ASSERTE(SUCCEEDED(hr));

		return hr;
	}

	ULONG KG3DEnvEffDynamicWeather::GetCurHour() /*= 0*/
	{
		return static_cast<ULONG>(m_nCurHour);
	}

	HRESULT KG3DEnvEffDynamicWeather::SetCurHour( INT Hour, BOOL bSetToOrBlendTo) /*= 0*/
	{
		KG_PROCESS_SUCCESS(Hour == m_nCurHour);
		{
			if (IsEnablePlay())
			{
				_KG3D_DEBUG_OUTPUT0("动态天气播放中，不能改变当前时间");
				goto Exit0;
			}

			if (bSetToOrBlendTo)
			{
				//改状态
				this->AllEnvEff_RequestModification(KG3DENVEFF_DWGCKEY_HANDLE(m_nCurRegion, Hour));
				m_nCurHour = Hour;
			}
			else
			{
				KG3DEnvEffDWController::BlendStruct TempStruct(KG3DENVEFF_DWGCKEY_HANDLE(m_nCurRegion, Hour)
					, KG3DEnvEffDWController_MDO_GAP_H2H, NULL);
				m_DWController.SetNextBlendStruct(TempStruct);
				m_nCurHour = Hour;
			}
		}
Exit1:
		return S_OK;
Exit0:
		return E_FAIL;
	}

	ULONG KG3DEnvEffDynamicWeather::GetCurRegion() /*= 0*/
	{
		return static_cast<ULONG>(m_nCurRegion);
	}

	HRESULT KG3DEnvEffDynamicWeather::SetCurRegion( INT Region, BOOL bSetToOrBlendTo ) /*= 0*/
	{
		KG_PROCESS_SUCCESS(Region == m_nCurRegion);
		{
			if (IsEnablePlay())
			{
				_KG3D_DEBUG_OUTPUT0("动态天气播放中，不能改变当前时间");
				goto Exit0;
			}

			if (bSetToOrBlendTo)
			{
				//改状态
				this->AllEnvEff_RequestModification(KG3DENVEFF_DWGCKEY_HANDLE(Region, m_nCurHour));
				m_nCurRegion = Region;
			}
			else
			{
				KG3DEnvEffDWController::BlendStruct TempStruct(KG3DENVEFF_DWGCKEY_HANDLE(Region, m_nCurHour)
					, KG3DEnvEffDWController_MDO_GAP_R2R, NULL);
				m_DWController.SetNextBlendStruct(TempStruct);
				m_nCurRegion = Region;
			}
			
		}
Exit1:
		return S_OK;
Exit0:
		return E_FAIL;
	}
	
	HRESULT KG3DEnvEffDynamicWeather::EnablePlay( BOOL bEnable )
	{
		if (bEnable)
		{
			if (! IsEnablePlay())
			{
				AllEnvEff_RequestModification(KG3DENVEFF_DWGCKEY_HANDLE(m_nCurRegion, m_nCurHour));
				IsEnablePlay(true);
			}
		}
		else
		{
			if (IsEnablePlay())
			{
				IsEnablePlay(false);
			}
		}
		return S_OK;
	}
/*
	HRESULT KG3DEnvEffDynamicWeather::CaptureKeyToKeyTableHelper( KG3DEnvEffect* pEnvEffect, INT* pnRetIndex)
	{
		
		KG_PROCESS_ERROR(pEnvEffect);
				{
					KG3DDWGC_KeyTableBase0* pKeyTable = m_KeyTrack0.GetKeyTable(pEnvEffect->GetType());
					//_ASSERTE(pKeyTable);
					KG_PROCESS_ERROR(pKeyTable);
					
					INT nRetIndex = DW_KEYINDEX_INVALID;
					KG3DDWGC_KeyBase* pKeyBase = pKeyTable->GetOneEmptyKey(nRetIndex);
					KG_PROCESS_ERROR(pKeyBase);
					_ASSERTE(CheckIsIndexValidAndNotInterp(nRetIndex));
					
					KG_COM_PROCESS_ERROR(pEnvEffect->DWGCGetCurStateToKey(pKeyBase, &m_KeyTrack0.GetKeyTableManager()));
					if (pnRetIndex)
						*pnRetIndex = nRetIndex;							
					return S_OK;
				}
		Exit0:
				_KG3D_DEBUG_SELF_OUTPUT1("%u:无法捕捉关键帧\n", pEnvEffect->GetType());
				return E_FAIL;
		return E_FAIL;
	}*/
/*
	HRESULT KG3DEnvEffDynamicWeather::ModifyKeyInKeyTableHelper( KG3DEnvEffect* pEnvEffect, INT nIndex )
	{
		
		KG_PROCESS_ERROR(pEnvEffect);
				{
					KG3DDWGC_KeyTableBase0* pKeyTable = m_KeyTrack0.GetKeyTable(pEnvEffect->GetType());
					_ASSERTE(pKeyTable);
					KG_PROCESS_ERROR(pKeyTable);
				
					KG3DDWGC_KeyBase* pTempKey = pKeyTable->GetKey(nIndex);
					KG_PROCESS_ERROR(pTempKey);
					return pEnvEffect->DWGCGetCurStateToKey(pTempKey, &m_KeyTrack0.GetKeyTableManager());
				}
		Exit0:
				_KG3D_DEBUG_SELF_OUTPUT1("%u:无法改关键帧\n", pEnvEffect->GetType());
				return E_FAIL;
		return E_FAIL;
	}	*/
/*
	HRESULT KG3DEnvEffDynamicWeather::SynWithKeyInKeyTableHelper( KG3DEnvEffect* pEnvEffect, INT nIndex )
	{
		
		KG_PROCESS_ERROR(pEnvEffect);
				{
					KG3DDWGC_KeyTableBase0* pKeyTable = m_KeyTrack0.GetKeyTable(pEnvEffect->GetType());
					_ASSERTE(pKeyTable);
					KG_PROCESS_ERROR(pKeyTable);
				
					KG3DDWGC_KeyBase* pTempKey = pKeyTable->GetKey(nIndex);
					KG_PROCESS_ERROR(pTempKey);
					{
						pEnvEffect->DWGCSetPercent(0);///如果那个Obj正在进行插值，那么需要先暂时回到0的状态
						return pEnvEffect->DWGCRequestModification(pTempKey, &m_KeyTrack0.GetKeyTableManager());///用到了Key的版本
					}
				}
		Exit0:
				_KG3D_DEBUG_SELF_OUTPUT1("%u:无法同步关键帧\n", pEnvEffect->GetType());
				return E_FAIL;
		return E_FAIL;
	}*/

	HRESULT KG3DEnvEffDynamicWeather::TL_GetKeyTable( KG3DENVEFF_HANDLE Handle, IEKG3DEnvEffDWGC_KeyTable** ppKeyTable )
	{
		if(ppKeyTable)
		{
			IEKG3DEnvEffDWGC_KeyTable* p = m_KeyTrack.GetTableInterface(Handle);
			if(p)
			{
				*ppKeyTable = p;
				return S_OK;
			}
		}
		return E_INVALIDARG;
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_AddRegionIndex( INT RegionIndex )
	{
		return m_KeyTrack.AddRegionIndex(RegionIndex);
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_DelRegionIndex( INT RegionIndex )
	{
		return m_KeyTrack.DelRegionIndex(RegionIndex);
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_ClearRegionIndices()
	{
		return m_KeyTrack.ClearRegionIndices();
	}

	ULONG KG3DEnvEffDynamicWeather::TL_GetRegionIndicesCount()
	{
		return m_KeyTrack.GetRegionIndicesCount();
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_ModifyRegionIndex( INT RegionIndex, INT NewIndex )
	{
		return m_KeyTrack.ModifyRegionIndex(RegionIndex, NewIndex);
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_GetFirstRegionIndex( INT* pRegionIndex )
	{
		return m_KeyTrack.GetFirstRegionIndex(pRegionIndex);
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_GetNextRegionIndex( INT PrevRegionIndex, INT* pNextRegionIndex )
	{
		return m_KeyTrack.GetNextRegionIndex(PrevRegionIndex, pNextRegionIndex);
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_DelKey(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle )
	{
		m_KeyTrack.DelKey(Handle, KeyHandle);
		return S_OK;
	}

	ULONG KG3DEnvEffDynamicWeather::TL_GetKeyCount( KG3DENVEFF_HANDLE Handle )
	{
		return m_KeyTrack.GetKeyCount(Handle);
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_GetFirstKeyHandle( KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE* pKeyHandle, LPVOID pExtraInfo )
	{
		return m_KeyTrack.GetFirstKeyHandle(Handle, pKeyHandle, pExtraInfo);
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_GetNextKeyHandle( KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo )
	{
		return m_KeyTrack.GetNextKeyHandle(Handle, PrevHandle, pNextHandle, pExtraInfo);
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_ClearKey( KG3DENVEFF_HANDLE Handle)
	{
		m_KeyTrack.ClearKey(Handle);
		return S_OK;
	}
	HRESULT KG3DEnvEffDynamicWeather::TL_GetFirstKeyHandleOfRegion( KG3DENVEFF_HANDLE Handle, INT Region, KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo )
	{
		return m_KeyTrack.GetFirstKeyHandleOfRegion(Handle, Region, pHandle, pExtraInfo);
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_GetNextKeyHandleOfRegion(KG3DENVEFF_HANDLE Handle, INT Region, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo )
	{
		return m_KeyTrack.GetNextKeyHandleOfRegion(Handle, Region, PrevHandle, pNextHandle, pExtraInfo);
	}

	HRESULT KG3DEnvEffDynamicWeather::TL_GetPrevKeyHandle(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DENVEFF_DWGCKEY_HANDLE* pPrevHandle )
	{
		return m_KeyTrack.GetPrevKeyHandle(Handle, KeyHandle, pPrevHandle);
	}
	
	HRESULT KG3DEnvEffDynamicWeather::TL_CopyKeyTo( KG3DENVEFF_HANDLE SrcHandle, KG3DENVEFF_DWGCKEY_HANDLE KeySrc, KG3DENVEFF_HANDLE DescHandle, KG3DENVEFF_DWGCKEY_HANDLE KeyDesc, BOOL bByValueOrRef )
	{
		return m_KeyTrack.CopyTo(SrcHandle, KeySrc, DescHandle, KeyDesc, bByValueOrRef);
	}


	HRESULT KG3DEnvEffDynamicWeather::StateToKey( KG3DENVEFF_HANDLE EffHandle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle )
	{
		_ASSERTE(m_pEnvEffList);
		KG3DEnvEffect* pEffect = m_pEnvEffList->GetEnvEffect(EffHandle.m_dwType, EffHandle.m_nIndex);
		KG_PROCESS_ERROR(pEffect);
		KG_PROCESS_ERROR(pEffect->IsDWGCAvailable());
		{
			bool bKeyIsNew = false;
			KG3DDWGC_KeyBase* pKey = m_KeyTrack.GetKey(EffHandle, KeyHandle);
			if(! pKey)
			{
				pKey = m_KeyTrack.NewKey(EffHandle, KeyHandle);//没有就创建出来
				bKeyIsNew = true;
			}
			KG_PROCESS_ERROR(pKey);
			HRESULT hr = pEffect->DWGCGetCurStateToKey(pKey);
			if(FAILED(hr))
			{
				if(bKeyIsNew)
				{
					m_KeyTrack.DelKey(EffHandle, KeyHandle);
					_ASSERTE(! m_KeyTrack.GetKey(EffHandle, KeyHandle));
				}
				goto Exit0;
			}
			return S_OK;
		}
Exit0:
		return E_FAIL;
	}

	HRESULT KG3DEnvEffDynamicWeather::KeyToState( KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DENVEFF_HANDLE EffHandle )
	{
		_ASSERTE(m_pEnvEffList);
		KG3DEnvEffect* pEffect = m_pEnvEffList->GetEnvEffect(EffHandle.m_dwType, EffHandle.m_nIndex);
		KG_PROCESS_ERROR(pEffect);
		KG_PROCESS_ERROR(pEffect->IsDWGCAvailable());
		{
			if(IsEnablePlay())
			{
				_KG3D_DEBUG_OUTPUT0("在播放中不能用Key来改变State");
				goto Exit0;
			}
			KG3DDWGC_KeyBase* pKey = m_KeyTrack.GetKey(EffHandle, KeyHandle);
			KG_PROCESS_ERROR(pKey);
			HRESULT hr = pEffect->DWGCRequestModification(pKey);
			KG_COM_PROCESS_ERROR(hr);
			return S_OK;
		}
Exit0:
		return E_FAIL;
	}

	VOID KG3DEnvEffDynamicWeather::AllEnvEff_RequestPopFrontState(float fCurBlendPercent /*= 0*/)
	{
		struct TypePopFrontState : public KG3DEnvEffectVisitor
		{
			float m_fCurBlendPercent;
			virtual HRESULT Accept(KG3DEnvEffect& Eff/*, KG3DEnvEffDynamicWeather& DW*/)
			{
				if(Eff.GetType() == KG3DTYPE_ENVEFF_DYNAMIC_WEATHER)
					return S_OK;
				if(Eff.IsEnableControlByDynamicWeather())
					Eff.DWGCRequestPopFrontState(m_fCurBlendPercent);
				return S_OK;
			}
		};
		TypePopFrontState Visitor;
		Visitor.m_fCurBlendPercent = fCurBlendPercent;
		//this->TraverseAllEffectsWithoutDW(Visitor);
		if (NULL != m_pEnvEffList)
			m_pEnvEffList->Traverse(Visitor);
	}

	VOID KG3DEnvEffDynamicWeather::AllEnvEff_RequestSubStateChange( KG3DENVEFF_DWGCKEY_HANDLE KeyHandle )
	{
		struct TypeRequestSubStateChange : public KG3DEnvEffectVisitor
		{
			KG3DENVEFF_DWGCKEY_HANDLE m_KeyHandle;
			KG3DDWGC_KeyTrack* m_pKeyTrack;
			HRESULT Accept(KG3DEnvEffect& Eff, const KG3DENVEFF_HANDLE& Handle/*, KG3DEnvEffDynamicWeather& DW*/)
			{
				if(Handle.m_dwType == KG3DTYPE_ENVEFF_DYNAMIC_WEATHER)
					return S_OK;
				if(! Eff.IsEnableControlByDynamicWeather())
					return S_OK;
				
				std::auto_ptr<KG3DDWGC_KeyBase> auto_p= m_pKeyTrack->GetInterpKey(Handle, m_KeyHandle);
				
				if(auto_p.get())
					Eff.DWGCRequestSubStateChange(auto_p.get());
				
				return S_OK;
			}
		};
		TypeRequestSubStateChange Visitor;
		Visitor.m_KeyHandle = KeyHandle;
		Visitor.m_pKeyTrack = &m_KeyTrack;
		//this->TraverseAllEffects(Visitor);

		if(NULL != m_pEnvEffList)
			m_pEnvEffList->Traverse(Visitor);
	}

	VOID KG3DEnvEffDynamicWeather::AllEnvEff_EnableSubState( BOOL bEnable )
	{
		struct TypeEnableSubState : public KG3DEnvEffectVisitor
		{
			BOOL m_bEnableSubState;
			KG3DDWGC_KeyTrack* m_pKeyTrack;
			virtual HRESULT Accept(KG3DEnvEffect& Eff, const KG3DENVEFF_HANDLE& Handle/*, KG3DEnvEffDynamicWeather& DW*/)
			{
				if(Eff.GetType() == KG3DTYPE_ENVEFF_DYNAMIC_WEATHER)
					return S_OK;
				if(! Eff.IsEnableControlByDynamicWeather())
					return S_OK;
				
				Eff.DWGCEnableSubState(m_bEnableSubState);

				return S_OK;
			}
		};
		TypeEnableSubState Visitor;
		Visitor.m_bEnableSubState = bEnable;
		Visitor.m_pKeyTrack = &m_KeyTrack;
		//this->TraverseAllEffectsWithoutDW(Visitor);
		if(NULL != m_pEnvEffList)
			m_pEnvEffList->Traverse(Visitor);
	}

	VOID KG3DEnvEffDynamicWeather::AllEnvEff_SetPercent( float fPercent )
	{
		struct TypeSetPercent : public KG3DEnvEffectVisitor
		{
			float m_fPercent;
			virtual HRESULT Accept(KG3DEnvEffect& Eff/*, KG3DEnvEffDynamicWeather& DW*/)
			{
				if(Eff.GetType() == KG3DTYPE_ENVEFF_DYNAMIC_WEATHER)
					return S_OK;
				if(Eff.IsEnableControlByDynamicWeather())
					Eff.DWGCSetPercent(m_fPercent);
				return S_OK;
			}
		};
		TypeSetPercent Visitor;
		Visitor.m_fPercent = fPercent;
		//this->TraverseAllEffectsWithoutDW(Visitor);
		if(NULL != m_pEnvEffList)
			m_pEnvEffList->Traverse(Visitor);
	}

	VOID KG3DEnvEffDynamicWeather::AllEnvEff_RequestModification( KG3DENVEFF_DWGCKEY_HANDLE KeyHandle )
	{
		struct TypeRequestModification : public KG3DEnvEffectVisitor
		{
			KG3DENVEFF_DWGCKEY_HANDLE m_KeyHandle;
			KG3DDWGC_KeyTrack* m_pKeyTrack;
			HRESULT Accept(KG3DEnvEffect& Eff, const KG3DENVEFF_HANDLE& Handle/*,KG3DEnvEffDynamicWeather& DW*/)
			{
				if(Handle.m_dwType == KG3DTYPE_ENVEFF_DYNAMIC_WEATHER)
					return S_OK;
				//不用检查是否受动态天气控制
				std::auto_ptr<KG3DDWGC_KeyBase> auto_p= m_pKeyTrack->GetInterpKey(Handle, m_KeyHandle);
				if(auto_p.get())
					Eff.DWGCRequestModification(auto_p.get());
				return S_OK;
			}
		};
		TypeRequestModification Visitor;
		Visitor.m_KeyHandle = KeyHandle;
		Visitor.m_pKeyTrack = &m_KeyTrack;
		//this->TraverseAllEffects(Visitor);
		if(NULL != m_pEnvEffList)
			m_pEnvEffList->Traverse(Visitor);
	}

	ULONG KG3DEnvEffDynamicWeather::IsEffectDWGCAvailable( KG3DENVEFF_HANDLE EffHandle )
	{
		/*unsigned uHandle = KG3DEnvEffect::GetTypeHandle(EffHandle.m_dwType);
		if (KG3DEnvEffect::IsValidTypeHandle(uHandle))
		{
			if (KG3DEnvEffect::IsTypeDWGCAvailable(uHandle))
			{
				return TRUE;
			}
		}*/

		KG3DEnvEffectClassProperty* pProperty = g_GetEnvEffectClassLibrary().GetProperty(EffHandle.m_dwType);
		if (NULL != pProperty)
		{
			return pProperty->m_bIsDWGCAvailable;
		}

		return FALSE;
	}

	VOID KG3DEnvEffDynamicWeather::AllEnvEff_CopyMainStateToSubState()
	{
		struct TypeEnableSubState : public KG3DEnvEffectVisitor
		{
			KG3DDWGC_KeyTrack* m_pKeyTrack;
			virtual HRESULT Accept(KG3DEnvEffect& Eff, const KG3DENVEFF_HANDLE& Handle/*, KG3DEnvEffDynamicWeather& DW*/)
			{
				if(Eff.GetType() == KG3DTYPE_ENVEFF_DYNAMIC_WEATHER)
					return S_OK;
				if(! Eff.IsEnableControlByDynamicWeather())
					return S_OK;

				//开始同步之前，需要统一SubState和主State，以免GetKey失败的情况下面得到错误的结果

				std::auto_ptr<KG3DDWGC_KeyBase>	autoP =	m_pKeyTrack->NewAKey(Handle);
				if(NULL == autoP.get())
				return S_OK;

				HRESULT hr = Eff.DWGCGetCurStateToKey(autoP.get());
				_ASSERTE(SUCCEEDED(hr));

				hr = Eff.DWGCRequestSubStateChange(autoP.get());
				_ASSERTE(SUCCEEDED(hr));

				return S_OK;
			}
		};
		TypeEnableSubState Visitor;
		Visitor.m_pKeyTrack = &m_KeyTrack;
		//this->TraverseAllEffectsWithoutDW(Visitor);
		if(NULL != m_pEnvEffList)
			m_pEnvEffList->Traverse(Visitor);
	}
	

	
	
};
