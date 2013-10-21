///////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffLightSet.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-11-22 10:53:32
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFLIGHTSET_H_
#define _INCLUDE_KG3DENVEFFLIGHTSET_H_
#include "KG3DEnvEffect.h"
#include "IEKG3DEnvEffLightSet.h"
//#include "TemplateLibs/LOKI_APP/KG3DLokiStructInterface.h"
////////////////////////////////////////////////////////////////////////////////


namespace KG3D_ENVIRONMENT_EFFECT
{
	/************************************************************************/
	/*DescStruct,DescStruct的作用包括有读写和保存
	*/
	/************************************************************************/
	class KG3DEnvEffLight;
	//struct KG3DEnvEffLightDescStruct0;
	//struct KG3DEnvEffLightSetDescStruct0 
	//{
	//	//在旧的文件中，虽然用了Scene中的Vec循环，但是只有一个被读取了，这个读取的被用作SunLight
	//	KG_CUSTOM_HELPERS::TCommonParamBuffer<KG3DEnvEffLightDescStruct0, 172> m_pSunLightDesc;
	//	//PlayerLight读的是MapSetting
	//	KG_CUSTOM_HELPERS::TCommonParamBuffer<KG3DEnvEffLightDescStruct0, 172> m_pPlayerLightDesc;
	//	
	//	bool m_bSunLightReadSucceed;
	//	bool m_bPlayerLightReadSucceed;

	//	KG3DEnvEffLightSetDescStruct0():m_bSunLightReadSucceed(false), m_bPlayerLightReadSucceed(false){}
	//	HRESULT LoadFromFile(LPCTSTR tcsFileName, IIniFile* pFile);
	//	HRESULT SaveToFile(LPCTSTR tcsFileName);
	//};
	class KG3DEnvEffLightSet;
	struct KG3DDWGC_Key_LightSet;
	struct KG3DEnvEffLightSetDescStruct1 : public KG3DENVEFFLIGHTSET_DESCSTRUCT 
	{
		KG3DEnvEffLightSetDescStruct1();
		//KG3DEnvEffLightSetDescStruct1(KG3DEnvEffLightSet* pLightSet);
		void operator=(const KG3DENVEFFLIGHTSET_DESCSTRUCT& Other){KG_CUSTOM_HELPERS::TTypeCopy<KG3DENVEFFLIGHTSET_DESCSTRUCT>(this, &Other);}
		void ToBaseStruct(KG3DENVEFFLIGHTSET_DESCSTRUCT& Other){KG_CUSTOM_HELPERS::TTypeCopy<KG3DENVEFFLIGHTSET_DESCSTRUCT>(&Other, this);}
		HRESULT LoadMapSetting(IIniFile* pFile, LPCTSTR pLightSet);
		HRESULT SaveMapSetting(IIniFile* pFile, LPCTSTR pLightSet);
		HRESULT SynWithKey(const KG3DDWGC_Key_LightSet& Key);
		HRESULT ToKey(KG3DDWGC_Key_LightSet& Key);
		VOID	SetDefault(){m_dwSceneAmbient = 0xff808080;}
	};

	typedef struct KG3DEnvEffLightSetDescStruct1 KG3DEnvEffLightSetDescStruct;
	typedef KG3DEnvEffLightSetDescStruct KG3DEnvEffLightSetLoader;


	/************************************************************************/
	/*Key 结构 
	*/
	/************************************************************************/
	struct KG3DDWGC_Key_LightSet : public KG3DDWGC_KeyBase
	{
		enum{ DWGCKEY_TYPE = KG3DTYPE_ENVEFF_LIGHTSET,};
		enum{ DWGCSUBKEY_TYPE = KG3DTYPE_ENVEFF_LIGHT,};
		virtual DWORD GetType()const{return DWGCKEY_TYPE;}
		//////////////////////////////////////////////////////////////////////////
		DWORD	m_dwModificationOption;
		D3DCOLOR	m_Ambient;
		//LIGHT_INDEX_ARRAY	m_IndexArray;

		/*
		typedef LOKI_APP::StructInterface::TElementConvertor<
					TYPELIST_1(D3DCOLOR)> ConverterType;
				static ConverterType& GetConverter()
				{
					static MAKE_DESC_ARRAY_1(pDescArray, m_Ambient);
					static ConverterType Convertor(pDescArray, _countof(pDescArray));
					return Convertor;
				}*/
		

		//////////////////////////////////////////////////////////////////////////
		enum{max_light = 10,};
		virtual bool CalculateInterp(const KG3DDWGC_KeyBase& BaseKeyBefore
			, const KG3DDWGC_KeyBase& BaseKeyAfter
			, float fPercent);

		bool SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		bool LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);

#if defined(DEBUG) | defined(_DEBUG)
		void DebugPrint(std::ostringstream& OSS);
#endif
	};

	/************************************************************************/
	/*LightSet本体 
	*/
	/************************************************************************/
	class KG3DEnvEffLightSet : public KG3DEnvEffect, public IEKG3DEnvEffLightSet
	{
	public:
		enum{max_light = KG3DDWGC_Key_LightSet::max_light,};
		KG3DEnvEffLightSet();
		~KG3DEnvEffLightSet();
	private:
		//为了保证有一个Light用于Sun一个Light用于Player，用下面的Buffer来创建两个栈中的Light
		KG_CUSTOM_HELPERS::TCommonParamBuffer<KG3DEnvEffLight, 356> m_pSunLightBuffer;
		KG_CUSTOM_HELPERS::TCommonParamBuffer<KG3DEnvEffLight, 356> m_pPlayerLightBuffer;
		
		KG3DEnvEffLightSetDescStruct	m_DescStruct;
		KG3DDWGC_Key_LightSet			m_SubKey;
		KG3DDWGC_Key_LightSet			m_KeyResult;
		FLOAT							m_fPercent;

		enum{emSubStateEnableBit = 0x00080000,};
		BOOL	IsSubStateEnable(){return m_DescStruct.m_dwModificationOption & emSubStateEnableBit;}
		void	SetSubStateEnable(BOOL bEnable){bEnable ? m_DescStruct.m_dwModificationOption |= emSubStateEnableBit : m_DescStruct.m_dwModificationOption &= (~emSubStateEnableBit);}
	public:
		//HRESULT RequestModification(KG3DEnvEffLightSetDescStruct0* pDesc);
		KG3DEnvEffLight* Get(INT nIndex);
		INT				 Size();
		KG3DEnvEffLight* Add(INT* pIndex);//参数都可以为NULL
		KG3DEnvEffLight& GetSunLight();
		KG3DEnvEffLight& GetPlayerLight();
		HRESULT Apply();
		D3DCOLOR	GetSceneAmbient();
		bool		SetSceneAmbient(D3DCOLOR Color);

		//<接口>
		virtual HRESULT Add(IEKG3DEnvEffLight** pRetLight, INT* pIndex);
		virtual HRESULT Del(INT nIndex);
		virtual HRESULT Clear();
		virtual HRESULT Size(INT* pSize);
		virtual HRESULT Get(INT nIndex, IEKG3DEnvEffLight** pRetLight);
		virtual HRESULT RequestModification(KG3DENVEFFLIGHTSET_DESCSTRUCT* pStruct);
		virtual HRESULT GetStruct(KG3DENVEFFLIGHTSET_DESCSTRUCT* pStruct);
		//</接口>
		virtual HRESULT Render(const KG3DEnvEffRenderContext& renderContext);
		virtual HRESULT	FrameMove();
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection);
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection);

		virtual HRESULT ToSolidInterface(LPVOID* pEffectInterface);

		//用上面的版本实现
		virtual KG3DEnvEffect* GetSubEffect(DWORD dwType, int nFinded);
		virtual KG3DEnvEffect* GetSubEffect(INT nIndex, KG3DENVEFF_HANDLE* pHandle = NULL);
		virtual INT GetSubEffectCount();
		virtual INT GetSubEffectCount(DWORD dwSubType);
		virtual VOID GetSubEffectTypeList(std::vector<DWORD>& Types){Types.push_back(KG3DTYPE_ENVEFF_LIGHTSET);}
		virtual VOID GetExistSubEffectTypeList( std::vector<DWORD>& Types );
		
	public://动态天气
		typedef KG3DDWGC_Key_LightSet	TypeKey;
		enum{DWGCSUBKEY_TYPE = TypeKey::DWGCSUBKEY_TYPE,};
		//<DW_Controller>
		HRESULT DWGCRequestPopFrontState(float fCurBlendPercent = 0.0f);
		HRESULT DWGCRequestSubStateChange(const KG3DDWGC_KeyBase* pRetKey);
		HRESULT DWGCGetCurStateToKey(KG3DDWGC_KeyBase* pRetKey);		
		HRESULT DWGCEnableSubState(BOOL bEnable);
		HRESULT DWGCSetPercent(float fPercent);
		HRESULT DWGCRequestModification(const KG3DDWGC_KeyBase* pKey);
		//</DW_Controller>
		
	private:
		void AddTwoDefaultLights();
		const std::tstring	GetLTFileFileNameFromSceneLoadingPath(LPCTSTR);
		
		//For Each
		typedef std::vector<KG3DEnvEffLight*> TypeLightContainer;
		TypeLightContainer m_vecLight;

		template<class T>
		T ForEach(T& Operator)
		{
			TypeLightContainer::iterator it = m_vecLight.begin();
			TypeLightContainer::iterator itEnd = m_vecLight.end();
			while (it != itEnd)
			{
				KG3DEnvEffLight* pLight = *it;
				_ASSERTE(pLight);
				Operator.Accept(*pLight);
				++it;
			}
						
		}
		

	};
};
typedef KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffLightSet KG3DLightSet;

#endif //_INCLUDE_KG3DENVEFFLIGHTSET_H_
