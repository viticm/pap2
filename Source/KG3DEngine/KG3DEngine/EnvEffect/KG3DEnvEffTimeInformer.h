////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffTimeInformer.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-12-30 17:50:37
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#pragma once
#ifndef _INCLUDE_KG3DENVEFFTIMEINFORMER_H_
#define _INCLUDE_KG3DENVEFFTIMEINFORMER_H_

#include "IEKG3DEnvEffDynamicWeather.h"
#include "KG3DMessenger.h"
//#include "FileType.h"
class KG3DTimeClock;
////////////////////////////////////////////////////////////////////////////////
namespace KG3D_ENVIRONMENT_EFFECT
{
	////////////////////////////////////////////////////////////////////////////////
	/************************************************************************/
	/*使用方法：派生这个类，然后把指针给DynamicWeather中的Informer。保证在DynamicWeather生命周期内存在。
	然后DynamicWeather中的TimeInformer会利用GetNowTime方法来取得一个时间                                                                      */
	/************************************************************************/
	
	struct KG3DEnvEffTI_DescStruct : public KG3DENVEFFTI_DESCSTRUCT 
	{
		enum{
			em_default_boost_factor = 1,
		};
		KG3DEnvEffTI_DescStruct();
		KG3DEnvEffTI_DescStruct(const KG3DEnvEffTI_DescStruct& OtherStruct);
		KG3DEnvEffTI_DescStruct(const KG3DENVEFFTI_DESCSTRUCT& OtherStruct);
		void operator=(const KG3DEnvEffTI_DescStruct& OtherStruct);
		bool SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		bool LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		//operator KG3DENVEFFTI_DESCSTRUCT();
	};
	class KG3DEnvEffTimeInformer : public KG3DMessageSingleReceiverSender, public IEKG3DEnvEffTimeInformer
	{
	private:
		KG3DTimeClock* m_pClock;
		KG3DEnvEffTI_DescStruct m_DescStruct;
		DWORD m_dwGetTimeGap;
		DWORD m_dwFrameLastGetTimeTime;
		DWORD m_dwState;
		enum
		{
			emIsStopped = 0x01,
		};		

	public:
		static const float TIME_INFORMER_MIN_BOOST_FACTOR; ///放大10倍
		
		HRESULT FrameMove();	///用于实现类似多线程的机制
		HRESULT RequestModification(const KG3DEnvEffTI_DescStruct* pDescStruct, DWORD dwModificationOption);
		inline const KG3DEnvEffTI_DescStruct* GetStruct();
		
		///<接口>		
		virtual HRESULT RequestModification(const KG3DENVEFFTI_DESCSTRUCT* pDescStruct, DWORD dwModificationOption);
		virtual HRESULT GetStruct(KG3DENVEFFTI_DESCSTRUCT* pDescStruct);
		HRESULT SaveMapSetting(IIniFile* pIniFile);
		HRESULT LoadMapSetting(IIniFile* pIniFile);
		INT	GetCurHour();	///主要用于初始化
		
		VOID StopTimeChecking(bool bStop);
		bool IsTimeCheckingStopped(){return m_dwState & emIsStopped;}
		///</接口>

		static KG3DEnvEffTimeInformer* Build(LPCTSTR, DWORD_PTR ptimeClock, DWORD_PTR pKG3DMessageReceiver);

		~KG3DEnvEffTimeInformer();
	private:
		DWORD m_StandardSecond;
		DWORD m_LastGettedSecond;
		
		DWORD GetSecond();
		HRESULT InitTimmer();

#if defined(DEBUG) | defined(_DEBUG)
	protected:
#else
	private:
#endif
		KG3DEnvEffTimeInformer(KG3DTimeClock& timeClock, KG3DMessageReceiverBase& receiverToBeInformed);
		

		DWORD GetHour(DWORD dwSecond);
	};

};///namespace
#endif //_INCLUDE_KG3DENVEFFTIMEINFORMER_H_
