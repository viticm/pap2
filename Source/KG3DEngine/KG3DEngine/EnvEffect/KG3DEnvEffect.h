////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffect.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2006-12-8 9:43:59
//  Comment     : 
/************************************************************************/
/*派生出一个Effect的方法：
1.派生Effect（必须第一派生），有的地方会强转（这个要去掉）。在KG3DTypes中加一个Type，在构造函数中调用KG3DEnvEffect的构造函数初始化Type
2.在KG3DCollocator中注册这个类的类型属性
3.覆盖Render等一组的纯虚函数，如果有子Effect，填写GetSubEffect那些函数
4.在Environment的AddEnvEffect中加入switch
5.如果需要用Scene的统一Get接口，在IEKG3DTypes中加Type,并在Scene中加Switch,如果是EnvEffect的派生类可以什么也不写
6.如果要使用动态天气，
	1)创建出Key类型，实现EnvEffect的几个动态天气相关方法，声明TypeKey
	2)KG3DDWGC_KeyTrack::AddKeyTables
7.如果要使用自己的RTTI，在KG3DRTTIType.cpp中ADD_RTTI

简单的可以用<KG3DEnvEffect SubClass Register Point>来查找填写上面这些东西的目标
*/
/************************************************************************/
//
////////////////////////////////////////////////////////////////////////////////
#pragma  once
#ifndef _INCLUDE_KG3DENVEFFECT_H_
#define _INCLUDE_KG3DENVEFFECT_H_
//#include "FileType.h"
#include "IEKG3DEnvironment.h"
#include "KG3DEnvEffDW_KeyBase.h"

namespace KG3D_ENVIRONMENT_EFFECT
{
	class KG3DEnvEffect;
	//选择实现一种，小心参数，因为不是纯虚，所以没有办法编译期检查派生类有没有覆盖正确的虚函数
	struct KG3DEnvEffectVisitor
	{
		virtual HRESULT Accept(KG3DEnvEffect& Eff){return S_OK;}
		virtual HRESULT Accept(KG3DEnvEffect& Eff, const KG3DENVEFF_HANDLE& HandleOfTheEff){return S_OK;}
		virtual ~KG3DEnvEffectVisitor() = 0{}
	};//所有的Visitor都是Traverse方法的调用者析构的，所以是不需要虚析构函数的

	//对类属性进行检查，在没有类实例的情况下比较低效，除了编辑相关的代码之外，应该在类实例中得到属性。
	class KG3DEnvEffectClassLibrary 
	{
	public:
		ULONG GetTypeCount();
		KG3DEnvEffectClassProperty*  GetProperty(DWORD dwType);
		BOOL	IsTypeTop(DWORD dwType);
		KG3DEnvEffectClassProperty*  GetFirstTypeProperty();
		KG3DEnvEffectClassProperty*  GetNextTypeProperty(KG3DEnvEffectClassProperty* pPreProperty);
		BOOL	RegisterType(KG3DEnvEffectClassProperty& propertyInput);

		KG3DEnvEffectClassLibrary();
	private:
		size_t m_uCount;
		KG3DEnvEffectClassProperty	m_arrayProperties[KG3DENVEFF_MAX];
	};

	KG3DEnvEffectClassLibrary& g_GetEnvEffectClassLibrary();

	struct KG3DDWGC_KeyBase;
	struct KG3DEnvEffRenderContext;
	class KG3DEnvEffect : public IEKG3DEnvEffect///注意派生必须放在类的最前面，直接强转就可以得到内部的实类
	{
		/************************************************************************/
		/*TypeTreeNode存在的目的是类型注册，方便Factory方法的类别请求，另外用于
		加速查找（路由表）
		KG3DEnvEffect是高度为2的树（如果连上根就是3，不过根是KG3DEnvironment），
		作为Top的EnvEffect，可以连带一系列的子EnvEffect，通常这样做是为了
		方便做一些统一的设置
		*/
		/************************************************************************/
		//<TypeTreeNode>
	public:
		static std::tstring	GetHandleString(KG3DENVEFF_HANDLE Handle);//可以非静态化
		static KG3DENVEFF_HANDLE	GetHandleFromString(LPCTSTR Str);//错误返回KG3DENVEFF_TYPE(KG3DTYPE_ENVEFF_INVALID_EFFECT, 0)//可以非静态化
		//</TypeTreeNode>

		
	//////////////////////////////////////////////////////////////////////////
	private://封闭方法
		KG3DEnvEffect();
		KG3DEnvEffect(const KG3DEnvEffect&);//不允许拷贝
		KG3DEnvEffect& operator=(const KG3DEnvEffect&);
	protected:
		KG3DEnvEffect(int nID, KG3DENVEFF_TYPE emType);//派生类可以正常使用,不能直接新建,Type是派生类的类型

	public:
		virtual ~KG3DEnvEffect();

	private:
		int m_nID;
		DWORD	m_dwType;
		/************************************************************************/
		/*
		KeyTable和EnvEffect是1，1相关的，当EnvEffect析构的时候，KeyTable也应该对应的析构
		所以要这样绑定。
		而且这样子取关键帧Table也会很方便
		*/
		/************************************************************************/
		//<KeyTable>	
		KG3D_CUSTOM_HELPERS::TKG3DLifeTimeDependedPointer<KG3DDWGC_KeyTableGeneratorBase*>	m_pKeyTableGenerator;
		KG3D_CUSTOM_HELPERS::TKG3DLifeTimeDependedPointer<KG3DDWGC_KeyTableBase*>	m_pKeyTable;
		//</KeyTable>
	protected:
		enum
		{
			EM_ENABLE = 1,
			EM_ENABLE_CONTROL_BY_DW  = 2,
			EM_IS_ID_AND_TYPE_INITIALIZED = 4,
			EM_IS_DWGC_AVAILABLE = KG3DENVEFF_STATE_FILTER_DWGC_AVAILABLE,
		};
		KG_CUSTOM_HELPERS::KGBitSet<32>	m_BitSet;

	public:
		enum{ MAX_NAME = 32 };

	public:
		BOOL IsEnable(){return m_BitSet.Is<EM_ENABLE>();}
		BOOL SetEnable(BOOL bEnable);
		ULONG GetType(){return m_dwType;}
	
		void SetEnableControlByDynamicWeather(BOOL bEnable);
		inline BOOL IsEnableControlByDynamicWeather() { return m_BitSet.Is<EM_ENABLE_CONTROL_BY_DW>(); }
		inline BOOL IsDWGCAvailable() { return m_BitSet.Is<EM_IS_DWGC_AVAILABLE>(); }

	public:
		//virtual STDMETHOD(Release) = 0;
		virtual HRESULT Render(const KG3DEnvEffRenderContext& renderContext) = 0;
		virtual HRESULT	FrameMove() = 0;
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection) = 0;
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection) = 0;
		virtual HRESULT ToSolidInterface(LPVOID* pEffectInterface) = 0;///使用这个方法可以得到EnvEffect对应的类的IE接口

        virtual HRESULT OnLostDevice();
        virtual HRESULT OnResetDevice();		
		virtual HRESULT PostRender();			
		
		//有子Effect必须覆盖下面函数的管理类
		virtual KG3DEnvEffect* GetSubEffect(DWORD dwType, int nFindex){return NULL;}
		virtual KG3DEnvEffect* GetSubEffect(INT nIndex, KG3DENVEFF_HANDLE* pHandle = NULL){return NULL;}///最后一个参数可以为NULL
		virtual INT GetSubEffectCount(){return 0;}
		virtual INT GetSubEffectCount(DWORD dwSubType){return 0;}
		virtual VOID GetSubEffectTypeList(std::vector<DWORD>& Types){}
		virtual VOID GetExistSubEffectTypeList(std::vector<DWORD>& Types){}
		
		//要使用动态天气必须覆盖下面的函数
		/*
		动态天气由4个元素组成：CurState,SubState，一个Percent，一个EnableSubState组成
		两个State分别保存现在和需要变化到的状态。
		Percent定义在现在这个时间，Effect处于两个State之间什么位置。值域为0-1
		EnableSubState表示是否需要根据SubState渲染，在不进行变化的时间里，关闭SubState可以加速
		*/
		///<DW_Controller>
		virtual HRESULT DWGCRequestPopFrontState(float fCurBlendPercent = 0.0f){return E_FAIL;}//要求SubState交换到CurState，如果发CurBlendPercent在0，1之间，那么取出中间状态
		virtual HRESULT DWGCRequestSubStateChange(const KG3DDWGC_KeyBase* pRetKey){return E_FAIL;}
		virtual HRESULT DWGCGetCurStateToKey(KG3DDWGC_KeyBase* pRetKey){return E_FAIL;}		
		virtual HRESULT DWGCEnableSubState(BOOL bEnable){return E_FAIL;}
		virtual HRESULT DWGCSetPercent(float fPercent){return E_FAIL;}
		virtual HRESULT DWGCRequestModification(const KG3DDWGC_KeyBase* pKey){return E_FAIL;}
		///</DW_Controller>

	public:
		///<有默认实现>
		virtual HRESULT	Enable(BOOL bEnable);
		virtual HRESULT IsEnable(BOOL* pbIsEnable);		
		//virtual	HRESULT	GetType(DWORD* pdwType);
		virtual HRESULT GetEnvEffName(LPTSTR tszName, INT nBufferSize);
		virtual HRESULT	GetID(int*	nID);
		virtual	HRESULT	EnableControlByDynamicWeather(BOOL	bEnable);
		virtual	HRESULT IsEnableControlByDynamicWeather(BOOL* pbEnable);

		virtual ULONG	GetState(size_t WhichState);
		
		//<KeyTable>
		KG3DDWGC_KeyTableBase* GetKeyTable();//得到内部绑定的Table
		KG3DDWGC_KeyTableBase* SetKeyTable(KG3DDWGC_KeyTableGeneratorBase& Gen, KG3DDWGC_KeyTableBase& KeyTable);
		//</KeyTable>
		///</有默认实现>
		
		
		///<IE包装>//只要上面填写了GetSubEffect的非Com标准版本，下面的会用上面的版本完成任务
	public:
		virtual HRESULT GetSubEffect(INT nIndex, IEKG3DEnvEffect** ppEnvEffect, KG3DENVEFF_HANDLE* pHandle);///最后一个参数可以为NULL
		virtual HRESULT GetSubEffectCount(INT* pnCount);
		///</IE包装>
	};


	class KG3DEnvEffectDummy : public KG3DEnvEffect
	{
	public:
		virtual HRESULT Render(const KG3DEnvEffRenderContext& renderContext){return E_NOTIMPL;}
		virtual HRESULT	FrameMove() {return E_NOTIMPL;}
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection){return E_NOTIMPL;}
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection){return E_NOTIMPL;}
		virtual HRESULT ToSolidInterface(LPVOID* pEffectInterface){return E_NOTIMPL;}
		KG3DEnvEffectDummy();
	};

	struct KG3DEnvEffRenderContext
	{
		D3DXMATRIX	matViewProj;
		D3DXVECTOR3	vCamPos;
		D3DXCOLOR	vFogColor;
	};
};

#endif //_INCLUDE_KG3DENVEFFECT_H_
