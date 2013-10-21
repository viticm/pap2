////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffFog.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-1-15 10:55:25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFFOG_H_
#define _INCLUDE_KG3DENVEFFFOG_H_
#include "IEKG3DEnvEffFog.h"
#include "KG3DEnvEffect.h"
//#include "KG3DMessenger.h"
//#include "TemplateLibs/LOKI_APP/KG3DLokiStructInterface.h"
////////////////////////////////////////////////////////////////////////////////
namespace KG3D_ENVIRONMENT_EFFECT
{
	struct KG3DDWGC_Key_Fog : public KG3DDWGC_KeyBase 
	{
		enum{DWGCKEY_TYPE = KG3DTYPE_ENVEFF_FOG,};
		virtual DWORD GetType()const{return DWGCKEY_TYPE;}

		DWORD m_dwModificationOption;
		///KG3DENVEFFFOG_MDO_FOG_BASE
		D3DCOLOR   m_FogColor;
		///KG3DENVEFFFOG_MDO_MODE_PARAM
		FLOAT      m_fFogStartValue;
		FLOAT      m_fFogEndValue;

		KG3DDWGC_Key_Fog();

		virtual bool CalculateInterp(
			const KG3DDWGC_KeyBase& BaseKeyBefore, 
			const KG3DDWGC_KeyBase& BaseKeyAfter,
			float fPercent);

		bool SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		bool LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);

#if defined(DEBUG) | defined(_DEBUG)
		void DebugPrint(std::ostringstream& OSS);
#endif	
	};
	//////////////////////////////////////////////////////////////////////////
	struct KG3DEnvEffFogDescStruct: public KG3DENVEFFFOG_DESCSTRUCT  
	{
		KG3DEnvEffFogDescStruct();
		KG3DEnvEffFogDescStruct(const KG3DEnvEffFogDescStruct& OtherStruct);
		KG3DEnvEffFogDescStruct(const KG3DENVEFFFOG_DESCSTRUCT& OtherStruct);
		void SynWithKey(const KG3DDWGC_Key_Fog& Key);///<DW_Frame>
		void operator=(const KG3DEnvEffFogDescStruct& OtherStruct); 
		void operator=(const KG3DENVEFFFOG_DESCSTRUCT& OtherStruct);
		void ToBaseStruct(KG3DENVEFFFOG_DESCSTRUCT& OtherStruct);
		void ToKey(KG3DDWGC_Key_Fog* pKey);///<DW_Frame>
		bool SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		bool LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
	};

    class KG3DEnvEffFog : 
        public KG3DEnvEffect, 
        public IEKG3DEnvEffFog
	{		
	public:
		virtual HRESULT Render(const KG3DEnvEffRenderContext& renderContext);
		virtual HRESULT	FrameMove();
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		virtual HRESULT ToSolidInterface(LPVOID* pEffectInterface);
		
		///IEKG3DEnvEffFog的接口
		virtual HRESULT RequestModification(const KG3DENVEFFFOG_DESCSTRUCT* pDescStruct, DWORD dwModificationOption);
		virtual HRESULT GetStruct(KG3DENVEFFFOG_DESCSTRUCT* pDescStruct);
		virtual HRESULT GetCaps(KG3DENVEFFFOG_CAPS_STRUCT* pRetCaps);
		virtual HRESULT GetFogColor(D3DCOLOR* pRetColor);
		virtual HRESULT SetFogColor(D3DCOLOR Color);
		
		///非接口
		HRESULT SetFogParam(const KG3DFogParam& Param);
		KG3DFogParam GetFogParam();
		HRESULT SetDistancePercent(FLOAT fPercent);//用于在可视距离变远的时候改变雾的远端
		HRESULT RequestModification(const KG3DEnvEffFogDescStruct& DescStruct, DWORD dwModificationOption);
		const KG3DEnvEffFogDescStruct* GetStruct();
		HRESULT Apply();	//应用内部的参数设置全局的Fog，一般不需要特别的专门调用，Fog会自己处理Apply。

		static KG3DFogParam GetDefaultFogParam();

		//用于在水下的时候，被外部控制Fog的情况。控制了之后记得在适当的时候释放
		HRESULT DemandFullControl();
		ULONG	IsInFullControl();
		HRESULT ReleaseFullControl();

		KG3DEnvEffFog();
		~KG3DEnvEffFog();
		
		///<动态天气>
	private:///用于动态天气的数据
		KG3DDWGC_Key_Fog m_SubKey;
		KG3DDWGC_Key_Fog m_curKey;
		BOOL	m_bEnableBlending;
		float	m_fBlendPercent;
	public:
		typedef		KG3DDWGC_Key_Fog	TypeKey;		
	public:
		HRESULT DWGCRequestPopFrontState(float fCurBlendPercent = 0.0f);
		HRESULT DWGCRequestSubStateChange(const KG3DDWGC_KeyBase* pRetKey);
		HRESULT DWGCGetCurStateToKey(KG3DDWGC_KeyBase* pRetKey);		
		HRESULT DWGCEnableSubState(BOOL bEnable);
		HRESULT DWGCSetPercent(float fPercent);
		HRESULT DWGCRequestModification(const KG3DDWGC_KeyBase* pKey);		
		///</动态天气>
	private:
		HRESULT GetCaps(KG3DENVEFFFOG_CAPS_STRUCT& CapsStruct);
		static FLOAT CalFogEnd(FLOAT fEnd, FLOAT fDisPercent);
	private:
		KG3DEnvEffFogDescStruct     m_DescStruct;
		KG3DENVEFFFOG_CAPS_STRUCT   m_CapsStruct;
		FLOAT						m_fDistancePercent;//用于根据ZFar调整
		bool						m_bIsUnderFullControl;
	};
};

typedef KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffFog KG3DFog;//占住这个名字，谁也别用，不然两个功能类似，名字类似的类存在同一工程里面，那就是往死里整了。要用Fog名字的，请想个其它名字用

#endif //_INCLUDE_KG3DENVEFFFOG_H_
