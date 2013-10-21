////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDW_KeyTrack.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-28 14:22:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFDW_KEYTRACK_H_
#define _INCLUDE_KG3DENVEFFDW_KEYTRACK_H_

////////////////////////////////////////////////////////////////////////////////
#include "IEKG3DEnvironment.h"
#include "IEKG3DEnvEffDynamicWeather.h"
#include "KG3DMessenger.h"
#include "KG3DEnvEffDW_BaseClasses.h"

#include "KG3DEnvEffDW_HelperClasses.h"

////////////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*
各个需要改变的类都要是KG3DEnvEffect的派生类，由于SkyBox，FarMountain，
Cloud等已经完成了，所以用他们的管理类SkySystem来统一作为一个EnvEffect处理。

每个类都有一个相关的 DWGC_Key
DW代表DynamicWeather动态天气，GC代表GraduallyChange渐变

这个Key用于渐变，渐变结构体在类内部会与Desc_Struct描述结构体比较从而得到
渐变数据。其实可以直接使用描述结构体的，但是多一个渐变结构体的话，
可以减少储存空间，而且可以限制可以渐变的参数。像远山那种，很难想像它
的高度会渐变，所以渐变参数是应该限制的。

每一个渐变结构体中的DWORD都作为一个保留字，作为参数，或者以后放其它东
西。有的Struct中指向其它Struct的Index都可以是Invalid的，如果为Invalid，则表示没
有关键帧。
*/
/************************************************************************/

/************************************************************************/
//时间轴的正式定义 
/************************************************************************/

///将来要统一到这个namespace
namespace KG3D_ENVIRONMENT_EFFECT
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	class KG3DDWGC_KeyTrackTester;
	class KG3DDWGC_KeyTrack
	{
		friend class KG3DDWGC_KeyTrackTester;
	public:
		typedef std::vector<KG3DDWGC_KeyTableGeneratorBase*> TypeTableGens;
		typedef std::set<INT>	TypeRegionIndexSet;
		enum{all_enveff_count = KG3DTYPE_ENVEFF_END - KG3DTYPE_ENVEFF_BEGIN,};
		static size_t GetVecPos(DWORD dwType){return dwType - KG3DTYPE_ENVEFF_BEGIN;}
	private:
		TypeTableGens	m_vecTableGens;	//里面放Table生成器，生成器本身包含N个Table
		TypeRegionIndexSet	m_setRegionIndex;
		KG3DEnvEffListBase&	m_EnvEffList;
	protected:
		VOID AddKeyTables();
		KG3DDWGC_KeyTrack();//EnvEffList主要用来得到KeyTable，如果是RunTimeTest用的话，在内部生成Table
		KG3DDWGC_KeyTableBase* GetTable(KG3DENVEFF_HANDLE Handle);
		KG3DDWGC_KeyTableGeneratorBase* GetTableGen(DWORD dwType);
		std::tstring&	GetSaveSectionName();
		size_t GetAllTableCount();
	public:
		KG3DDWGC_KeyTrack(KG3DEnvEffListBase& List);
		~KG3DDWGC_KeyTrack();
		
		VOID	GetTypesOfAllExistTables(std::vector<DWORD>& vecResult);
		IEKG3DEnvEffDWGC_KeyTable* GetTableInterface(KG3DENVEFF_HANDLE Handle){return GetTable(Handle);}//IEKG3DEnvEffDWGC_KeyTable是一个退化界面，只读
		bool	IsRegionAllowed(INT Region){return m_setRegionIndex.end() != m_setRegionIndex.find(Region);}
		//Region管理
		HRESULT AddRegionIndex(INT RegionIndex);
		HRESULT DelRegionIndex(INT RegionIndex);
		HRESULT ClearRegionIndices();
		ULONG	GetRegionIndicesCount(){return static_cast<ULONG>(m_setRegionIndex.size());}
		HRESULT ModifyRegionIndex(INT RegionIndex, INT NewIndex);

		HRESULT GetFirstRegionIndex(INT* pRegionIndex);
		HRESULT GetNextRegionIndex(INT PrevRegionIndex, INT* pNextRegionIndex);
		

		//Key管理
		KG3DDWGC_KeyBase* NewKey(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);
		KG3DDWGC_KeyBase* GetKey(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);
		VOID			DelKey(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);
		ULONG			GetKeyCount(KG3DENVEFF_HANDLE Handle);
		HRESULT			GetFirstKeyHandle(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo);
		HRESULT			GetNextKeyHandle(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo);
		HRESULT			GetFirstKeyHandleOfRegion(KG3DENVEFF_HANDLE Handle, INT Region, KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo);
		HRESULT			GetNextKeyHandleOfRegion(KG3DENVEFF_HANDLE Handle, INT Region, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo);
		HRESULT			GetPrevKeyHandle(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DENVEFF_DWGCKEY_HANDLE* pPrevHandle);

		VOID			ClearKey(KG3DENVEFF_HANDLE Handle);

		//Key插值
		std::auto_ptr<KG3DDWGC_KeyBase>		GetInterpKey(KG3DENVEFF_HANDLE Handle, KG3DENVEFF_DWGCKEY_HANDLE KeyHandle);//注意AutoPtr的用法
		std::auto_ptr<KG3DDWGC_KeyBase>		NewAKey(KG3DENVEFF_HANDLE Handle);//工厂方法

		//Copy
		HRESULT			CopyTo(KG3DENVEFF_HANDLE SrcHandle, KG3DENVEFF_DWGCKEY_HANDLE KeySrc, KG3DENVEFF_HANDLE DescHandle, KG3DENVEFF_DWGCKEY_HANDLE KeyDesc, BOOL bByValueOrRef);

		//Load Save
		HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR);
		HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR);
	};
	
	
};


#endif //_INCLUDE_KG3DENVEFFDW_KEYTRACK_H_
