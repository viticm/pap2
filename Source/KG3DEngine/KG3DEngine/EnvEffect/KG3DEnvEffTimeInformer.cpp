////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffTimeInformer.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-12-30 17:51:14
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

///编译宏选择
#include "KG3DEnvEffCommonDef.h"

#include "KG3DEnvEffTimeInformer.h"
#include "IEKG3DEnvEffDynamicWeather.h"
#include "KG3DGraphicsTool.h"
#include "KGINIFileHelpers.h"
#include "MathTool.h"
#include "KG3DTime.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


#define UNIFORM_NUM(NumToBeUniform, nMax)	\
	{	\
		_ASSERTE(NumToBeUniform >= 0);	\
		if (NumToBeUniform > nMax)	\
			NumToBeUniform = NumToBeUniform % nMax;	\
		if (NumToBeUniform == 0)	\
			NumToBeUniform = nMax;	\
	}NULL

////////////////////////////////////////////////////////////////////////////////
namespace KG3D_ENVIRONMENT_EFFECT
{
const float KG3DEnvEffTimeInformer::TIME_INFORMER_MIN_BOOST_FACTOR = 0.1f;
KG3DEnvEffTI_DescStruct::KG3DEnvEffTI_DescStruct()	
{
	m_fBoostFactor = em_default_boost_factor;	
	m_dwCurHour = 0;		
}
KG3DEnvEffTI_DescStruct::KG3DEnvEffTI_DescStruct(const KG3DEnvEffTI_DescStruct& OtherStruct)
{
	memcpy_s(this, sizeof(KG3DENVEFFTI_DESCSTRUCT), &OtherStruct, sizeof(KG3DENVEFFTI_DESCSTRUCT));
}
KG3DEnvEffTI_DescStruct::KG3DEnvEffTI_DescStruct(const KG3DENVEFFTI_DESCSTRUCT& OtherStruct)
{
	memcpy_s(this, sizeof(KG3DENVEFFTI_DESCSTRUCT), &OtherStruct, sizeof(KG3DENVEFFTI_DESCSTRUCT));
}
void KG3DEnvEffTI_DescStruct::operator=(const KG3DEnvEffTI_DescStruct& OtherStruct)
{
	memcpy_s(this, sizeof(KG3DENVEFFTI_DESCSTRUCT), &OtherStruct, sizeof(KG3DENVEFFTI_DESCSTRUCT));
}

bool KG3DEnvEffTI_DescStruct::SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
{
	_ASSERTE (pIniFile&&pSection);
	if (m_fBoostFactor != em_default_boost_factor)
	{
		_KG3D_DEBUG_SELF_REPORT("不正确的BoostFactor\n");
		m_fBoostFactor = em_default_boost_factor;
	}
	if (m_dwCurHour != 0)
	{
		_KG3D_DEBUG_SELF_REPORT("不正确的Hour\n");
		m_dwCurHour = 0;
	}
	pIniFile->WriteFloat(pSection, _STRINGER(m_fBoostFactor), m_fBoostFactor);
	pIniFile->WriteInteger(pSection, _STRINGER(m_dwCurHour), (int)m_dwCurHour);
	return true;
}

bool KG3DEnvEffTI_DescStruct::LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection )
{
	_ASSERTE (pIniFile&&pSection);
	//pIniFile->GetFloat(pSection, _STRINGER(m_fBoostFactor), -0.1f, &m_fBoostFactor);
	KG_CUSTOM_HELPERS::LoadINIFileData<FLOAT>(pIniFile, pSection, _STRINGER(m_fBoostFactor), -0.1f, &m_fBoostFactor);
	//pIniFile->GetInteger(pSection, _STRINGER(m_dwCurHour), 50, (int*)&m_dwCurHour);
	KG_CUSTOM_HELPERS::LoadINIFileData(pIniFile, pSection, _STRINGER(m_dwCurHour), 50, (int*)&m_dwCurHour);
	MT_LIMIT_TO_RANGE(m_fBoostFactor, KG3DENVEFFTI_MAX_BOOST_FACTOR
		, KG3DEnvEffTimeInformer::TIME_INFORMER_MIN_BOOST_FACTOR);
	if (m_dwCurHour != 0)///0的话，遵循系统时间
	{
		MT_LIMIT_TO_RANGE(m_dwCurHour, KG3DENVEFFTI_HOUR_MAX, KG3DENVEFFTI_HOUR_MIN);
	}
	
	return true;
}


/************************************************************************/
/* KG3DEnvEffTimeInformer定义开始
*/
/************************************************************************/
KG3DEnvEffTimeInformer::KG3DEnvEffTimeInformer(KG3DTimeClock& timeClock, KG3DMessageReceiverBase& receiverToBeInformed)
:m_pClock(&timeClock)
,m_dwGetTimeGap(KG3DENVEFFTI_GET_TIME_GAP)///是秒
,m_dwFrameLastGetTimeTime(0)
,m_dwState(0)
{
	timeClock.AddRef();
	bool bRet = this->RegisterReceiver(&receiverToBeInformed);
	KG_USE_ARGUMENT(bRet);
	_ASSERTE(bRet);
}
KG3DEnvEffTimeInformer::~KG3DEnvEffTimeInformer()
{
	SAFE_RELEASE(m_pClock);
}
HRESULT KG3DEnvEffTimeInformer::FrameMove()
{
	///GetTime延时逻辑
	DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
	KG_PROCESS_SUCCESS(dwNowTime - m_dwFrameLastGetTimeTime < m_dwGetTimeGap);
	m_dwFrameLastGetTimeTime = dwNowTime;
	KG_PROCESS_SUCCESS(this->IsTimeCheckingStopped());
	_ASSERTE(NULL != m_pClock);
	{
		DWORD dwNowHour = this->GetHour(this->GetSecond());
		if (dwNowHour != m_DescStruct.m_dwCurHour)
		{
			m_DescStruct.m_dwCurHour = dwNowHour;
			this->PostMessage(KG3DMessage(KM_TIME_ON_REACH_HOUR, m_DescStruct.m_dwCurHour, NULL));
		}
	}
	
Exit1:
	return S_OK;
#if 0
Exit0:
	_KG3D_DEBUG_SELF_ASSERTE(NULL&&_STRINGER(KG3DEnvEffTimeInformer)":"_STRINGER(FrameMove)"\n");
	return S_FALSE;///为了使用正常不管怎样都是S，但是如果到这里的话是不正常的
#endif
}
HRESULT KG3DEnvEffTimeInformer::RequestModification(const KG3DEnvEffTI_DescStruct* pDescStruct, DWORD dwModificationOption)
{
	KG_PROCESS_ERROR(pDescStruct);
	if(dwModificationOption&KG3DENVEFFTI_MDO_BOOST_FACTOR)
	{
		float fTempFactor = pDescStruct->m_fBoostFactor;
		KG_PROCESS_ERROR(fTempFactor >= TIME_INFORMER_MIN_BOOST_FACTOR
			&&fTempFactor <= KG3DENVEFFTI_MAX_BOOST_FACTOR);
		{
			m_DescStruct.m_fBoostFactor = fTempFactor;

			//重新设定GetTimeGap
			m_dwGetTimeGap = (DWORD)((float)KG3DENVEFFTI_GET_TIME_GAP / fTempFactor);
			m_dwGetTimeGap = max(m_dwGetTimeGap, 1);///不能太少，单位是秒
		}
	}
	if(dwModificationOption&KG3DENVEFFTI_MDO_CUR_TIME)
	{
		KG_PROCESS_ERROR(pDescStruct->m_dwCurHour >= KG3DENVEFFTI_HOUR_MIN && pDescStruct->m_dwCurHour <= KG3DENVEFFTI_HOUR_MAX);
		
		DWORD dwTime = 0;
		KG_PROCESS_ERROR(m_pClock);
		dwTime = m_pClock->GetNowTimeSecondInADay();
		
		m_DescStruct.m_dwCurHour = pDescStruct->m_dwCurHour;

		m_StandardSecond = m_DescStruct.m_dwCurHour * KG3DENVEFFTI_SECOND_IN_AN_HOUR;
		m_LastGettedSecond = static_cast<DWORD>(static_cast<float>(dwTime) * m_DescStruct.m_fBoostFactor);			
	}	
	//*pdwModificationOption = dwReturnMDO;
	return S_OK;
Exit0:
	return E_FAIL;		
}

inline const KG3DEnvEffTI_DescStruct* KG3DEnvEffTimeInformer::GetStruct()
{
	return &m_DescStruct;	
}

KG3DEnvEffTimeInformer* KG3DEnvEffTimeInformer::Build( LPCTSTR, DWORD_PTR ptimeClock, DWORD_PTR pReceiver)
{
	KG3DEnvEffTimeInformer* pRet = NULL;
	KG3DTimeClock* pTimeClock = reinterpret_cast<KG3DTimeClock*>(ptimeClock);
	KG3DMessageReceiver* pReceiverObj = reinterpret_cast<KG3DMessageReceiver*>(pReceiver);
	KG_PROCESS_ERROR(NULL != pTimeClock && NULL != pReceiverObj);
	pRet = new KG3DEnvEffTimeInformer(*pTimeClock, *pReceiverObj);
	KG_PROCESS_ERROR(NULL != pRet);
	return pRet;
Exit0:
	SAFE_DELETE(pRet);
	return NULL;
}

HRESULT KG3DEnvEffTimeInformer::SaveMapSetting( IIniFile* pIniFile )
{
	bool bResult = m_DescStruct.SaveMapSetting(pIniFile, _STRINGER(KG3DEnvEffTI_DescStruct));
	return bResult = true ? S_OK : E_FAIL;
}

HRESULT KG3DEnvEffTimeInformer::LoadMapSetting( IIniFile* pIniFile )
{
	
	KG_COM_PROCESS_ERROR(InitTimmer());
	
	if (pIniFile)
	{
		KG3DEnvEffTI_DescStruct TempStruct;
		if (TempStruct.LoadMapSetting(pIniFile, _STRINGER(KG3DEnvEffTI_DescStruct)))
		{
			DWORD dwMod = KG3DENVEFFTI_MDO_ALL & (~KG3DENVEFFTI_MDO_CUR_TIME);
			//dwMod &= (~KG3DENVEFFTI_MDO_BOOST_FACTOR);//不能改Boost
			return RequestModification(&TempStruct, dwMod);
		}
	}
	

Exit0:
	return E_FAIL;
}

DWORD KG3DEnvEffTimeInformer::GetSecond()
{
	if (NULL != m_pClock)
	{
		DWORD dwTemp = m_pClock->GetNowTimeSecondInADay();
		return  m_StandardSecond + static_cast<DWORD>(static_cast<float>(dwTemp) * m_DescStruct.m_fBoostFactor) - m_LastGettedSecond;
	}
	return m_StandardSecond + m_LastGettedSecond;
}
DWORD KG3DEnvEffTimeInformer::GetHour(DWORD dwSecond)
{
	DWORD dwHour = dwSecond / KG3DENVEFFTI_SECOND_IN_AN_HOUR;
	UNIFORM_NUM(dwHour, KG3DENVEFFTI_HOUR_IN_A_DAY);
	return dwHour;
}
/************************************************************************/
/* 接口定义
*/
/************************************************************************/

HRESULT KG3DEnvEffTimeInformer::RequestModification( const KG3DENVEFFTI_DESCSTRUCT* pDescStruct, DWORD dwModificationOption )
{
	if (!pDescStruct)
	{
		return E_FAIL;
	}
	KG3DEnvEffTI_DescStruct TempStruct(*pDescStruct);
	return RequestModification(&TempStruct, dwModificationOption);	
}
HRESULT KG3DEnvEffTimeInformer::GetStruct( KG3DENVEFFTI_DESCSTRUCT* pDescStruct )
{
	if (!pDescStruct)
	{
		return E_FAIL;
	}
	*pDescStruct = m_DescStruct;
	return S_OK;
}

INT KG3DEnvEffTimeInformer::GetCurHour()
{
	return m_DescStruct.m_dwCurHour;
}

HRESULT KG3DEnvEffTimeInformer::InitTimmer()
{
	m_StandardSecond = 0;
	DWORD dwTemp = 0;
	_ASSERTE(NULL != m_pClock);
	dwTemp = m_pClock->GetNowTimeSecondInADay();	

	m_LastGettedSecond = static_cast<DWORD>(static_cast<float>(dwTemp) * m_DescStruct.m_fBoostFactor);
	m_StandardSecond = dwTemp;
	m_DescStruct.m_dwCurHour = GetHour(dwTemp);
	return S_OK;
}

VOID KG3DEnvEffTimeInformer::StopTimeChecking( bool bStop )
{
	bStop ? m_dwState |= emIsStopped : m_dwState &= ~emIsStopped;
	if (! IsTimeCheckingStopped())
	{
		InitTimmer();//从Stop状态恢复的时候要重新初始化内部的数据
	}
}


#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DEnvEffTimeInformer)
{
	struct KG3DMessageSenderBaseTest : public IKG3DMessageSenderBase 
	{
		virtual bool __stdcall RegisterReceiver( IKG3DMessageReceiverBase* pReceiver ){return true;}
		virtual bool __stdcall UnRegisterReceiver( IKG3DMessageReceiverBase* pReceiver ){return true;}
	};

	class KG3DEnvEffTimeInformerTester : public KG3DEnvEffTimeInformer
	{
	public:
		KG3DEnvEffTimeInformerTester(KG3DTimeClock& timeClock, KG3DMessageReceiverBase& receiverToBeInformed)
			:KG3DEnvEffTimeInformer(timeClock, receiverToBeInformed)
		{

		}
		using KG3DEnvEffTimeInformer::GetHour;
	};

	KG3DTimeClockDummy dummyClock;
	KG3DMessageReceiver testReceiver;
	KG3DEnvEffTimeInformerTester	TI(dummyClock, testReceiver);
	//测试上面的宏 
	{
		int nHour = KG3DENVEFFTI_HOUR_MAX+2;
		UNIFORM_NUM(nHour, KG3DENVEFFTI_HOUR_IN_A_DAY);
		_ASSERTE(nHour == 2);
		nHour = KG3DENVEFFTI_HOUR_MAX * 2;
		UNIFORM_NUM(nHour, KG3DENVEFFTI_HOUR_IN_A_DAY);
		_ASSERTE(nHour == KG3DENVEFFTI_HOUR_MAX);
		nHour = 0;
		UNIFORM_NUM(nHour, KG3DENVEFFTI_HOUR_IN_A_DAY);
		_ASSERTE(nHour == KG3DENVEFFTI_HOUR_MAX);
	}
	//测试GetHour
	_ASSERTE(TI.GetHour(0) == 24);
	_ASSERTE(TI.GetHour(KG3DENVEFFTI_SECOND_IN_AN_HOUR - 1) == 24);
	_ASSERTE(TI.GetHour(KG3DENVEFFTI_SECOND_IN_AN_HOUR) == 1);
	_ASSERTE(TI.GetHour(KG3DENVEFFTI_SECOND_IN_AN_HOUR * 25 - 1) == 24);
	_ASSERTE(TI.GetHour(KG3DENVEFFTI_SECOND_IN_AN_HOUR * 25) == 1);

	_ASSERTE(! TI.IsTimeCheckingStopped());
	TI.StopTimeChecking(true);
	_ASSERTE(TI.IsTimeCheckingStopped());
	TI.StopTimeChecking(false);
	_ASSERTE(! TI.IsTimeCheckingStopped());

}
KG_TEST_END(KG3DEnvEffTimeInformer)
#endif


};///namespace