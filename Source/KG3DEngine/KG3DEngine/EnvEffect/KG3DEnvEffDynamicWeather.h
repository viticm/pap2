////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDynamicWeather.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-12-5 15:27:58
//  Comment     : 这个类属于环境效果（Environment Effect），是天空系统，灯光等的控制器
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFDYNAMICWEATHER_H_
#define _INCLUDE_KG3DENVEFFDYNAMICWEATHER_H_

#include "IEKG3DEnvironment.h"
#include "IEKG3DEnvEffDynamicWeather.h"
#include "KG3DEnvEffect.h"
#include "KG3DMessenger.h"
#include "KG3DEnvEffDW_KeyTrack.h"
#include "KG3DEnvEffTimeInformer.h"
#include "KG3DEnvEffDWController.h"

class KG3DRegionInformer;

namespace KG3D_ENVIRONMENT_EFFECT
{
	class KG3DEnvEffDynamicWeather : 
        public KG3DMessageReceiver, 
        public KG3DEnvEffect, 
        public IEKG3DEnvEffDynamicWeather
	{
	public:
		KG3DEnvEffDynamicWeather(KG3DEnvEffListBase* pEnvEffList);
		virtual ~KG3DEnvEffDynamicWeather();

		virtual HRESULT Render(const KG3DEnvEffRenderContext& renderContext);
		virtual HRESULT	FrameMove();
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		virtual HRESULT ToSolidInterface(LPVOID* pEffectInterface);

    public:
		virtual void __stdcall OnReceiveMessage(const KG3DMessage& Message, IKG3DMessageSenderBase* pSender);

	private:	
		KG3DEnvEffDynamicWeather();//不能这样用

	public:///接口声明
		virtual HRESULT	RequestModification( const KG3DENVEFF_DYNWEATHER_DESCSTRUCT* pStruct );///可以用pStruct中的ModificationOption设定改什么东西
		virtual HRESULT	GetStruct( KG3DENVEFF_DYNWEATHER_DESCSTRUCT* pResultStruct );
		
		///Getter
		virtual HRESULT GetTimeInformer(IEKG3DEnvEffTimeInformer** ppTI);
		virtual HRESULT GetRegionInformer(IEKG3DRegionInformer** ppRI);

		///预览效果
		virtual HRESULT StartQuickPlay(INT nRegion, INT nStartHour, DWORD_PTR dwExtraParam);
		virtual HRESULT StopQuickPlay(DWORD_PTR dwExtraParam);

		virtual HRESULT EnablePlay(BOOL bEnable);

		virtual ULONG	GetCurHour();
		virtual HRESULT SetCurHour(INT Hour, BOOL bSetToOrBlendTo);

		virtual ULONG	GetCurRegion();
		virtual HRESULT SetCurRegion(INT Region, BOOL bSetToOrBlendTo);
		
		//<新的时间线接口>
		//Region Index的操作部分
		virtual HRESULT TL_GetKeyTable(KG3DENVEFF_HANDLE Handle, IEKG3DEnvEffDWGC_KeyTable** ppKeyTable);
		virtual HRESULT TL_AddRegionIndex(INT RegionIndex);
		virtual HRESULT TL_DelRegionIndex(INT RegionIndex);
		virtual HRESULT TL_ClearRegionIndices();
		virtual ULONG	TL_GetRegionIndicesCount();
		virtual HRESULT TL_ModifyRegionIndex(INT RegionIndex, INT NewIndex);
		virtual HRESULT TL_GetFirstRegionIndex(INT* pRegionIndex);
		virtual HRESULT TL_GetNextRegionIndex(INT PrevRegionIndex, INT* pNextRegionIndex);

		//Key的操作部分
		virtual HRESULT	TL_DelKey(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);
		virtual ULONG	TL_GetKeyCount(KG3DENVEFF_HANDLE Handle);
		virtual HRESULT	TL_GetFirstKeyHandle(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo);
		virtual HRESULT	TL_GetNextKeyHandle(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo);
		virtual HRESULT	TL_ClearKey(KG3DENVEFF_HANDLE Handle);
		virtual HRESULT	TL_GetFirstKeyHandleOfRegion(KG3DENVEFF_HANDLE Handle, INT Region, KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo);
		virtual HRESULT	TL_GetNextKeyHandleOfRegion(KG3DENVEFF_HANDLE Handle, INT Region, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo);
		virtual HRESULT TL_GetPrevKeyHandle(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DENVEFF_DWGCKEY_HANDLE* pPrevHandle);

		virtual HRESULT TL_CopyKeyTo(KG3DENVEFF_HANDLE SrcHandle, KG3DENVEFF_DWGCKEY_HANDLE KeySrc, KG3DENVEFF_HANDLE DescHandle, KG3DENVEFF_DWGCKEY_HANDLE KeyDesc, BOOL bByValueOrRef);

		//SetKey
		virtual HRESULT StateToKey(KG3DENVEFF_HANDLE EffHandle,  KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);
		virtual HRESULT KeyToState(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DENVEFF_HANDLE EffHandle);
		
		//辅助函数
		virtual ULONG	IsEffectDWGCAvailable(KG3DENVEFF_HANDLE EffHandle);
		//</新的时间线接口>

		//用于给Control控制的分发Key的机制

		VOID AllEnvEff_RequestPopFrontState(float fCurBlendPercent = 0);
		VOID AllEnvEff_RequestSubStateChange(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);
		
		VOID AllEnvEff_EnableSubState(BOOL bEnable);
		VOID AllEnvEff_SetPercent(float fPercent);
		VOID AllEnvEff_RequestModification(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);

		VOID AllEnvEff_CopyMainStateToSubState();

	private:
		void OnEnterHour(DW_HOUR nHour);
		void OnEnterRegion(DW_REGION_TAB nRegion);
		bool DistributeInterpKeys(DW_REGION_TAB nRegion, DW_HOUR nHour, DWORD dwMDO, DWORD_PTR dwExtraData = NULL);

		HRESULT InitRegionAndHourState();

	private:
		KG3DDWGC_KeyTrack	m_KeyTrack;
		KG3DEnvEffTimeInformer* m_pTimeInformer;		//用于通知时间的消息
		KG3DEnvEffDWController	m_DWController;		//用于统一控制各个效果的渐变
		KG3DEnvEffListBase* m_pEnvEffList;			//各个类的指针表，与KG3DEnvironment一起用
		KG3DRegionInformer* m_pRegionInformer;		//用于通知地域改变的消息

		///各种指示性数据
		enum
		{
			EmOnEnterHour = 1,
			EmOnEnterRegion = 2,
			EmEnableDistributeKeys = 4,
			EmEnalbePlay = 8,
		};

		DWORD m_dwState;
		
		bool IsEnablePlay(){return (m_dwState & EmEnalbePlay) ? true : false;}
		void IsEnablePlay(bool bRet){bRet ? m_dwState |= EmEnalbePlay : m_dwState &= (~EmEnalbePlay);}		
		
		DW_HOUR m_nCurHour;
		DW_REGION_TAB m_nCurRegion;
	};
};


#endif //_INCLUDE_KG3DENVEFFDYNAMICWEATHER_H_
