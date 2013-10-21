////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDWController.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-7-4 14:35:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
///编译宏选择
#include "KG3DEnvEffCommonDef.h"
#include "KG3DEnvEffDWController.h"
#include "KG3DEnvEffDynamicWeather.h"
#include "KG3DLoopQueue.h"
#include "KG3DGraphicsTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
////////////////////////////////////////////////////////////////////////////////


namespace KG3D_ENVIRONMENT_EFFECT
{
	KG3DEnvEffDWController::KG3DEnvEffDWController()
		:m_pObj(NULL)
		,m_bIsBlending(FALSE)
		,m_fBlendPercent(0)
		,m_dwBlendStartMiliSecond(0)
	{
		_ASSERTE(m_DescStruct.m_fBlendGapH2H_ms == DWGC_BLEND_GAP_HOUR2HOUR_SECOND * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND);///确认Struct构造函数有执行
		m_fNextBlendGap = m_DescStruct.m_fBlendGapH2H_ms;
	}

	KG3DEnvEffDWController::~KG3DEnvEffDWController()
	{
		m_pObj = NULL;
	}

	void KG3DEnvEffDWController::UnInitBlendData()
	{
		m_fBlendPercent = 0;
		m_bIsBlending = FALSE;
		if (m_pObj)
		{
			//注意顺序
			m_pObj->AllEnvEff_SetPercent(0);
			m_pObj->AllEnvEff_EnableSubState(FALSE);
		}
		m_dwBlendStartMiliSecond = 0;
	}
	bool KG3DEnvEffDWController::InitBlendData()
	{
		_ASSERTE(m_pObj);
		
		KG_PROCESS_ERROR(! m_BlendQueue.empty());
		{
			BlendStruct& RefBlendStruct = m_BlendQueue.front();
			m_fNextBlendGap = (RefBlendStruct.m_dwMDO & KG3DEnvEffDWController_MDO_GAP_H2H)
				? m_DescStruct.m_fBlendGapH2H_ms : m_DescStruct.m_fBlendGapR2R_ms;
			_ASSERTE(m_fNextBlendGap > 0);

			m_pObj->AllEnvEff_RequestSubStateChange(RefBlendStruct.m_KeyHandle);

			m_fBlendPercent = 0;
			m_bIsBlending = TRUE;

			//记录开始时间
			m_dwBlendStartMiliSecond = g_cGraphicsTool.GetNowTime();

			//注意顺序
			_ASSERTE(m_pObj);
			m_pObj->AllEnvEff_EnableSubState(TRUE);
			m_pObj->AllEnvEff_SetPercent(0);

			return true;
		}
Exit0:
		_KG3D_DEBUG_REPORT("很不正常的情况:KG3DEnvEffDWController初始化渐变状态失败\n");
		return false;
	}

	void KG3DEnvEffDWController::SetBindedControlObjectAndInitControlState( TypeControlObj* pObj ) ///NULL的话，解除控制
	{
		if (m_pObj&&pObj)
		{
			_KG3D_DEBUG_SELF_REPORT(NULL&&"DWCG_Controller的控制对象重复绑定，错误");
			return;
		}
		UnInitBlendData();	
		m_pObj = pObj;

		if (m_pObj)///接入渐变状态
		{
			while (m_BlendQueue.size() > 1)
			{
				m_BlendQueue.pop();
			}

			if (m_BlendQueue.size() > 0)
			{
				InitBlendData();
			}
			m_pObj->AllEnvEff_CopyMainStateToSubState();//开始同步之前，需要统一SubState和主State，以免GetKey失败的情况下面得到错误的结果
							
		}
	}
	
	HRESULT KG3DEnvEffDWController::SetNextBlendStruct(const BlendStruct& TempBlendStruct )
	{	
		//先检查是否和最后一个相等
		{
			
			if (m_BlendQueue.size() > 0)
			{
				BlendStruct& RefStruct = m_BlendQueue.back();
				if (0 == memcmp(&RefStruct.m_KeyHandle, &TempBlendStruct.m_KeyHandle, sizeof(KG3DENVEFF_DWGCKEY_HANDLE)))
				{
					goto Exit1;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		_ASSERTE(m_BlendQueue.size() <= 1);

		if (m_BlendQueue.size() == 1)
		{
			if(m_pObj)
			{
				if (m_fBlendPercent > 0 && m_fBlendPercent < 1)
				{
					m_pObj->AllEnvEff_RequestPopFrontState(m_fBlendPercent);	//把两个渐变中的状态合并，准备下一个渐变
				}
				else
				{
					m_pObj->AllEnvEff_RequestPopFrontState();
				}
			}
			
			m_BlendQueue.back() = TempBlendStruct;
		}
		else
		{
			m_BlendQueue.push(TempBlendStruct);
		}
		
		if(! m_pObj)//即使没有绑定控制对象，也继续做上面的BlendQueue的操作，一旦有控制对象就可以立刻起作用了
		{
			goto Exit1;//如果没有控制对象就什么也不做
		}
		
		InitBlendData();
Exit1:
		return S_OK;
	}

	HRESULT KG3DEnvEffDWController::FrameMove()
	{
		KG_PROCESS_SUCCESS(NULL == m_pObj);
		if (m_bIsBlending)	///正在渐变
		{			
			_ASSERTE(m_fBlendPercent > 0- FLT_EPSILON);
			DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
			_ASSERTE(dwNowTime > 0.0f);

			m_fBlendPercent = (dwNowTime - m_dwBlendStartMiliSecond) / (m_fNextBlendGap);
			_ASSERTE(m_fBlendPercent >= 0.0f);
			
			if (m_fBlendPercent < 1.0f)
			{
				m_pObj->AllEnvEff_SetPercent(m_fBlendPercent);
				goto Exit1;
			}
			
			m_fBlendPercent = 0;///归0

			_ASSERTE(m_BlendQueue.size() == 1);
			m_BlendQueue.pop();
			
			m_pObj->AllEnvEff_RequestPopFrontState();
			this->UnInitBlendData();
		}///if (m_bIsBlending)	///正在渐变
Exit1:
		return S_OK;
	}

	HRESULT KG3DEnvEffDWController::RequestModification( KG3DEnvEffDWControllerDescStruct& DescStruct )
	{
		KG3DEnvEffDWControllerDescStruct TempStruct = m_DescStruct;
		if (DescStruct.m_dwModificationOption & KG3DEnvEffDWController_MDO_GAP_H2H)
		{
			KG_PROCESS_ERROR(DescStruct.m_fBlendGapH2H_ms > 0.0f);
			TempStruct.m_fBlendGapH2H_ms = DescStruct.m_fBlendGapH2H_ms;
		}
		if (DescStruct.m_dwModificationOption & KG3DEnvEffDWController_MDO_GAP_R2R)
		{
			KG_PROCESS_ERROR(DescStruct.m_fBlendGapR2R_ms > 0.0f);
			TempStruct.m_fBlendGapR2R_ms = DescStruct.m_fBlendGapR2R_ms;
		}
		if (DescStruct.m_dwModificationOption & KG3DEnvEffDWController_MDO_GAP_BOOST)
		{
			KG_PROCESS_ERROR(DescStruct.m_fBlendBoost > 0.0f);
			TempStruct.m_fBlendBoost = DescStruct.m_fBlendBoost;
			TempStruct.m_fBlendGapH2H_ms = DWGC_BLEND_GAP_HOUR2HOUR_SECOND * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND / DescStruct.m_fBlendBoost;
			TempStruct.m_fBlendGapR2R_ms = DWGC_BLEND_GAP_REGION2REGION_SECOND * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND / DescStruct.m_fBlendBoost;
		}
		m_DescStruct = TempStruct;
		return S_OK;
Exit0:
		return E_FAIL;
	}
};

