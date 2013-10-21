////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DTime.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-12-10 14:28:48
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DTime.h"
#include "IEKG3DEnvEffDynamicWeather.h"
#include "KG3DInterface.h"


class KG3DTimeClockSolid : public KG3DTimeClock///模拟一个KG3DTimeClock
{

public:
	KG3DTimeClockSolid();
	STDMETHOD_(ULONG, GetType)(){return KG3DTYPE_TIME_CLOCK;}
	STDMETHOD(FrameMove)();
public:
	STDMETHOD_(LPVOID, ToAnotherInterface)(DWORD_PTR Param)
	{
		if(Param != KG3DTYPE_I_INTERFACE)
			return NULL;

		IKG3DTimeClock* p = this;
		return p;
	}
	virtual HRESULT AdjustClock(DWORD dwTime);
	virtual ULONG GetNowTimeMillisecond()const;	///用于返回标准时间,其中DWORD的单位是秒
	virtual ULONG GetNowTimeSecondInADay()const;
	virtual FLOAT GetBoostFactor()const;

	HRESULT Init(KG3D_GAMETIME_INITSTRUCT* pInitStruct); 

	KG_TEST_DECLARE_CLASS_TEST(KG3DTimeClockSolid);
private:
	DWORD m_dwLastGainServerTimeMS;
	DWORD m_dwMachineTimeGainWhenAdjustMS;

	FLOAT m_fTimeBoostFactor;	
	DWORD m_dwNowTimeMS;

	enum
	{
		em_get_time_gap = 300,	//毫秒
	};
private:
	virtual DWORD GetSystemTimeMS();
	DWORD GetNowTime();
	void InterpNowTime();
	
};

#define UNIFORM_NUM(NumToBeUniform, nMax)	\
{	\
	_ASSERTE(NumToBeUniform >= 0);	\
	if (NumToBeUniform > nMax)	\
	NumToBeUniform = NumToBeUniform % nMax;	\
	if (NumToBeUniform == 0)	\
	NumToBeUniform = nMax;	\
}NULL

/************************************************************************/
/* KG3DTimeClockSolid定义开始
*/
/************************************************************************/
KG3DTimeClockSolid::KG3DTimeClockSolid()
:m_dwLastGainServerTimeMS(0)
,m_fTimeBoostFactor(1.0f)
,m_dwMachineTimeGainWhenAdjustMS(0)
,m_dwNowTimeMS(0)

{
	
}

ULONG KG3DTimeClockSolid::GetNowTimeMillisecond()const
{
	return m_dwNowTimeMS;
}
void KG3DTimeClockSolid::InterpNowTime()
{
	DWORD timeGap = this->GetSystemTimeMS() - m_dwMachineTimeGainWhenAdjustMS;
	enum{millisecond_in_a_year = 24 * 10 * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND * KG3DENVEFFTI_SECOND_IN_AN_HOUR};
	if(timeGap <= 0 || timeGap > millisecond_in_a_year)
		return;

	float fScaledTimeGap = static_cast<FLOAT>(timeGap) * m_fTimeBoostFactor;
	//m_fTimeBoostFactor不是加速的意思，而是
	//当游戏时间和实际时间不一样的时候，在这个插值函数里面，需要用实际时间插出游戏时间，
	//这个Factor就是 现实一天秒数/游戏一天秒数

	_ASSERTE(fScaledTimeGap > 0);

	DWORD dwNowTime = m_dwLastGainServerTimeMS + static_cast<DWORD>(fScaledTimeGap);
	
	m_dwNowTimeMS = dwNowTime;
}
HRESULT STDMETHODCALLTYPE KG3DTimeClockSolid::FrameMove()
{
	InterpNowTime();
	return S_OK;
}

DWORD KG3DTimeClockSolid::GetSystemTimeMS()
{
	return ::timeGetTime();
}

HRESULT KG3DTimeClockSolid::Init(KG3D_GAMETIME_INITSTRUCT* pInitStruct)
{
	DWORD dwCurSystemTime = this->GetSystemTimeMS();

	KG3D_GAMETIME_INITSTRUCT tempStruct;
	tempStruct.dwInitTimeMS = EM_GAME_INIT_TIME_DEFAULT * KG3DENVEFFTI_SECOND_IN_AN_HOUR * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND;
	tempStruct.fBoostFactor = EM_GAME_BOOST_DEFAULT;

	KG3D_GAMETIME_INITSTRUCT* pInitStructUsing = &tempStruct;
	if(NULL != pInitStruct)
	{
		KG_PROCESS_ERROR(pInitStruct->dwInitTimeMS >= 0
				&& pInitStruct->dwInitTimeMS <= KG3DENVEFFTI_TIME_MAX * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND);
		KG_PROCESS_ERROR(pInitStruct->fBoostFactor > 0 
				&& pInitStruct->fBoostFactor - static_cast<FLOAT>(KG3DENVEFFTI_MAX_BOOST_FACTOR) < FLT_EPSILON);
		pInitStructUsing = pInitStruct;
	}		

	m_dwLastGainServerTimeMS = pInitStructUsing->dwInitTimeMS;
	m_dwNowTimeMS = m_dwLastGainServerTimeMS;
	m_dwMachineTimeGainWhenAdjustMS = dwCurSystemTime;
	m_fTimeBoostFactor = pInitStructUsing->fBoostFactor;
	
	return S_OK;
Exit0:
	return E_INVALIDARG;
}

FLOAT KG3DTimeClockSolid::GetBoostFactor() const
{
	return m_fTimeBoostFactor;
}

HRESULT KG3DTimeClockSolid::AdjustClock(DWORD dwMilliSeconds)
{
#if defined(DEBUG) | defined(_DEBUG)
	DWORD dwCalculatedNowTime = this->GetNowTimeMillisecond();
	DWORD nGap = dwCalculatedNowTime - dwMilliSeconds;
	if(nGap > KG3DENVEFFTI_TIME_TOLERANCE)
	{
		_KG3D_DEBUG_SELF_OUTPUT0(_STRINGER(KG3DTimeClockSolid)":对时错误\n");
	}
#endif

	m_dwLastGainServerTimeMS = dwMilliSeconds;
	m_dwNowTimeMS = m_dwLastGainServerTimeMS;
	m_dwMachineTimeGainWhenAdjustMS = this->GetSystemTimeMS();

	return S_OK;
}

ULONG KG3DTimeClockSolid::GetNowTimeSecondInADay()const
{
	ULONG nowTimeSecond = this->GetNowTimeMillisecond() / KG3DENVEFFTI_MILLISECOND_IN_A_SECOND;
	UNIFORM_NUM(nowTimeSecond, KG3DENVEFFTI_TIME_RANGE);

	_ASSERTE(nowTimeSecond >= KG3DENVEFFTI_TIME_MIN && nowTimeSecond <= KG3DENVEFFTI_TIME_MAX);
	return nowTimeSecond;
}
////////////////////////////////////////////////////////////////////////////////


KG3DCommonObject* KG3DTimeClock::Build( LPCTSTR, DWORD_PTR pKG3D_GAMETIME_INITSTRUCT, DWORD_PTR )
{
	KG3DTimeClockSolid* p = new KG3DTimeClockSolid;

	KG_PROCESS_ERROR(NULL != p);
	{
		HRESULT hr = p->Init(reinterpret_cast<KG3D_GAMETIME_INITSTRUCT*>(pKG3D_GAMETIME_INITSTRUCT));
		KG_COM_PROCESS_ERROR(hr);
	}
	return p;
Exit0:
	SAFE_RELEASE(p);
	return NULL;
}

#if defined(KG_ENABLE_TDD)
KG_TEST_CLASS_TEST_BEGIN(KG3DTimeClockSolid)
{
#if 1
	HRESULT hr = E_FAIL;

	static DWORD curSystemTime = 0;

	struct KG3DTimeClockTest : public KG3DTimeClockSolid
	{
		virtual DWORD GetSystemTimeMS(){return curSystemTime;}
	};

	{
		KG3DTimeClockTest tempClock;

		KG3D_GAMETIME_INITSTRUCT initStruct;
		initStruct.dwInitTimeMS = 1024;
		initStruct.fBoostFactor = 1;

		curSystemTime = 1111;
		hr = tempClock.Init(&initStruct);
		_ASSERTE(SUCCEEDED(hr));
		_ASSERTE(tempClock.m_dwNowTimeMS == initStruct.dwInitTimeMS);
		_ASSERTE(tempClock.m_fTimeBoostFactor == initStruct.fBoostFactor);
		_ASSERTE(tempClock.m_dwMachineTimeGainWhenAdjustMS == curSystemTime);
	}

	{
		KG3DTimeClockTest tempClock;

		KG3D_GAMETIME_INITSTRUCT initStruct;
		initStruct.dwInitTimeMS = 0;
		initStruct.fBoostFactor = 1;

		hr = tempClock.Init(&initStruct);
		_ASSERTE(SUCCEEDED(hr));
		_ASSERTE(tempClock.m_dwNowTimeMS == 0);
	}

	{
		KG3DTimeClockTest tempClock;

		KG3D_GAMETIME_INITSTRUCT initStruct;
		initStruct.dwInitTimeMS = 1;
		initStruct.fBoostFactor = 1;

		hr = tempClock.Init(&initStruct);
		_ASSERTE(SUCCEEDED(hr));
		_ASSERTE(tempClock.m_dwNowTimeMS == 1);
	}

	{
		KG3DTimeClockTest tempClock;

		KG3D_GAMETIME_INITSTRUCT initStruct;
		initStruct.dwInitTimeMS = 1024;
		initStruct.fBoostFactor = KG3DENVEFFTI_MAX_BOOST_FACTOR + 1;

		hr = tempClock.Init(&initStruct);
		_ASSERTE(FAILED(hr));
	}

	{
		KG3DTimeClockTest tempClock;
		
		KG3D_GAMETIME_INITSTRUCT initStruct;
		initStruct.dwInitTimeMS = KG3DENVEFFTI_TIME_MAX * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND + 1;
		initStruct.fBoostFactor = 1;

		hr = tempClock.Init(&initStruct);
		_ASSERTE(FAILED(hr));
	}

	{
		KG3DTimeClockTest tempClock;

		KG3D_GAMETIME_INITSTRUCT initStruct;
		initStruct.dwInitTimeMS = KG3DENVEFFTI_TIME_MIN * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND;
		initStruct.fBoostFactor = 1;

		curSystemTime = 0;
		_ASSERTE(curSystemTime == 0);
		hr = tempClock.Init(&initStruct);
		_ASSERTE(SUCCEEDED(hr));
		
		
		curSystemTime = 100;
		tempClock.FrameMove();

		_ASSERTE(tempClock.m_dwLastGainServerTimeMS == KG3DENVEFFTI_TIME_MIN * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND);
		_ASSERTE(tempClock.m_dwMachineTimeGainWhenAdjustMS == 0);
		_ASSERTE(tempClock.m_dwNowTimeMS == tempClock.m_dwLastGainServerTimeMS + curSystemTime);

		curSystemTime = 200;
		tempClock.FrameMove();
		_ASSERTE(tempClock.m_dwMachineTimeGainWhenAdjustMS == 0);
		_ASSERTE(tempClock.m_dwNowTimeMS == tempClock.m_dwLastGainServerTimeMS + 200 - 0);

		curSystemTime = 100;
		tempClock.FrameMove();
		_ASSERTE(tempClock.m_dwNowTimeMS == tempClock.m_dwLastGainServerTimeMS + 100);

	}

	{
		KG3DTimeClockTest tempClock;

		KG3D_GAMETIME_INITSTRUCT initStruct;
		initStruct.dwInitTimeMS = 50 * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND;
		initStruct.fBoostFactor = KG3DENVEFFTI_MAX_BOOST_FACTOR;

		curSystemTime = 100;
		tempClock.Init(&initStruct);
		_ASSERTE(tempClock.m_fTimeBoostFactor == initStruct.fBoostFactor);

		curSystemTime = 200;
		tempClock.FrameMove();
		_ASSERTE(tempClock.m_dwMachineTimeGainWhenAdjustMS == 100);
		_ASSERTE(tempClock.m_dwNowTimeMS == 50 * KG3DENVEFFTI_MILLISECOND_IN_A_SECOND + (200- 100) * initStruct.fBoostFactor);

	}
#endif
}
KG_TEST_CLASS_TEST_END(KG3DTimeClockSolid)
#endif


ULONG KG3DTimeClockDummy::GetNowTimeMillisecond() const
{
	return KG3DENVEFFTI_TIME_MIN;
}

ULONG KG3DTimeClockDummy::GetNowTimeSecondInADay() const
{
	return KG3DENVEFFTI_TIME_MIN;
}

ULONG STDMETHODCALLTYPE KG3DTimeClockDummy::GetType()
{
	return KG3DTYPE_TIME_CLOCK;
}

FLOAT KG3DTimeClockDummy::GetBoostFactor() const
{
	return EM_GAME_BOOST_DEFAULT;
}