////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DTime.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-2-26 9:50:24
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DTIME_H_
#define _INCLUDE_KG3DTIME_H_

#include "KG3DCommonObject.h"
#include "KG3DInterface.h"
struct KG3D_GAMETIME_INITSTRUCT;
////////////////////////////////////////////////////////////////////////////////

struct KG3DTimeClock : public KG3DCommonObjectSimpleRef, public IKG3DTimeClock	//使用引用计数
{
	STDMETHOD_(ULONG, GetType)() = 0;
	STDMETHOD_(LPVOID, ToAnotherInterface)(DWORD_PTR Param) = 0;	

	virtual HRESULT AdjustClock(DWORD dwMilliSeconds) = 0;
	virtual ULONG GetNowTimeMillisecond()const  = 0;
	virtual ULONG GetNowTimeSecondInADay()const = 0;	//得到当前时间，会放缩到一天之内的秒数
	virtual FLOAT GetBoostFactor()const = 0;	//得到的是服务器时间是实际时间的多少倍

	static KG3DCommonObject* Build(LPCTSTR, DWORD_PTR pKG3D_GAMETIME_INITSTRUCT, DWORD_PTR);
};

struct KG3DTimeClockDummy : public KG3DTimeClock 
{
	STDMETHOD_(ULONG, GetType)();
	STDMETHOD_(LPVOID, ToAnotherInterface)(DWORD_PTR Param){return NULL;}

	virtual HRESULT AdjustClock(DWORD dwMilliSeconds){return E_FAIL;}
	virtual ULONG GetNowTimeMillisecond()const;
	virtual ULONG GetNowTimeSecondInADay()const;
	virtual FLOAT GetBoostFactor()const;
};

#endif //_INCLUDE_KG3DTIME_H_
