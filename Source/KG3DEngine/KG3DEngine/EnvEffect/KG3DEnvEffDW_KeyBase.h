////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffDW_KeyBase.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-28 11:10:37
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFDW_KEYBASE_H_
#define _INCLUDE_KG3DENVEFFDW_KEYBASE_H_
#include "IEKG3DEnvironment.h"
#include "IEKG3DEnvEffDynamicWeather.h"
//#include "FileType.h"
#include <utility>
////////////////////////////////////////////////////////////////////////////////
namespace KG3D_ENVIRONMENT_EFFECT
{
	///KG3D_ENVIRONMENT_EFFECT全局的Enum
	///DW代表DynamicWeather动态天气，GC代表GraduallyChange渐变
	typedef int DW_HOUR;
	typedef int DW_REGION_TAB;

	enum
	{		
		///INVALID_INDEX表示空索引，无效索引，无效索引依然能够索引到表
		///ERROR_INDEX表示错误，错误索引，这种索引无法索引到表
		DW_KEYINDEX_INVALID  = 0,
		DW_KEYINDEX_MUST_VALID = 1,				///至少有一个的，必定有效的KeyIndex
		DW_KEYTRAIT_ERROR = 0x80000000,			///用于标志无效的Key
		DW_KEYTRAIT_INTERP = 0x40000000,		///次最高位为1，表示这是个插值出来的Key
		DW_KEYINDEX_VALID_MIN = DW_KEYINDEX_MUST_VALID,
		DW_KEYINDEX_INTERP_START = 200,
		DW_KEYINDEX_VALID_MAX = 239,///注意这里插值索引只定义了40个，所以注意特效不能加40个以上
		DW_KEYINDEX_INTERP_END = DW_KEYINDEX_VALID_MAX,
	};
	enum
	{
		KG3DENVEFF_MAX_SUBEFFECT = 20,
	};
	//////////////////////////////////////////////////////////////////////////
	// HIGH_TODO : 创建一个公用的指针表
	class KG3DEnvEffect;
	class KG3DEnvEffectVisitor;
	struct KG3DEnvEffListBase
	{
		//virtual KG3DEnvEffect* GetFirstTopEnvEffect(UINT& Handle) = 0;
		//virtual KG3DEnvEffect* GetNextTopEnvEffect(UINT Handle, UINT& NextHandle) = 0;
		virtual KG3DEnvEffect* GetEnvEffect(DWORD dwType, int nFinded = 0) = 0;///可以串联得到所有的EnvEffect
		virtual INT GetEnvEffectCount(DWORD dwType) = 0;
		virtual VOID GetExistEnvEffectTypes(std::vector<DWORD>& dwTyps) = 0;

		//virtual KG3DEnvEffect* ResetIterator() = 0;
		//virtual KG3DEnvEffect* NextIterator() = 0;//注意不能同时改变容器内容
		virtual KG3DEnvEffect* GetFirstTopEffect(KGCH::IteratorHolder& holderFirst) = 0;
		virtual KG3DEnvEffect* GetNextTopEffect(KGCH::IteratorHolder& holderPrev) = 0;

		virtual HRESULT Traverse(KG3DEnvEffectVisitor& Visitor) = 0;
	};
	struct KG3DEnvEffListDummy : public KG3DEnvEffListBase
	{
		virtual KG3DEnvEffect* GetEnvEffect(DWORD dwType, int nFinded = 0){return NULL;}
		virtual INT GetEnvEffectCount(DWORD dwType){return 0;}
		virtual VOID GetExistEnvEffectTypes(std::vector<DWORD>& dwTyps){}
		/*virtual KG3DEnvEffect* ResetIterator(){return NULL;}
		virtual KG3DEnvEffect* NextIterator(){return NULL;}*/
		virtual KG3DEnvEffect* GetFirstTopEffect(KGCH::IteratorHolder& holderFirst){return NULL;}
		virtual KG3DEnvEffect* GetNextTopEffect(KGCH::IteratorHolder& holderPrev){return NULL;}
		virtual HRESULT Traverse(KG3DEnvEffectVisitor& Visitor){return S_OK;}
	};
	//////////////////////////////////////////////////////////////////////////
	//class KG3DDWGC_KeyTableManagerBase1;
	//typedef KG3DDWGC_KeyTableManagerBase1 KG3DDWGC_KeyTableManagerBase;

	///注意应该可以强转成正确的类型,所以注意派生时，要注意public顺序
	///所有DWGC类的基类，DWGC代表DynamicWeather Gradually Change动态天空渐变
	///Key是会拷贝的，不要把Key搞太复杂了
	struct KG3DDWGC_KeyBase
	{
		/*
		开头必须有DWORD m_dwModificationOption
		最高位用于标志是否一个无效的Key
		*/

		virtual bool CalculateInterp(const KG3DDWGC_KeyBase& BaseKeyBefore, const KG3DDWGC_KeyBase& BaseKeyAfter, float fPercent)= 0;	///所有的InterpStruct头尾两个参数都必须是实在的Key，不能是插值插出来的,结果放在调用的那个类上面，即this。因为有的类是不需要TableManager也没有办法得到TableManager的，所以最后一个参数要可选，只能用指针
		//virtual KG3DDWGC_KeyBase* GetContainedKey(KG3DDWGC_KeyTableManagerBase& RefTableMgr, DWORD dwType, int nFinded)const	{return NULL;}
		//virtual INT GetContainedKeyIndex(KG3DDWGC_KeyTableManagerBase& RefTableMgr, DWORD dwType, int nFinded)const	{return DW_KEYINDEX_INVALID;}
		//virtual bool SetContainedKeyIndex(KG3DDWGC_KeyTableManagerBase& RefTableMgr, DWORD dwType, int nFinded, INT nNewIndex)	{return false;}	///不管释放，nNewIndex = DW_KEYINDEX_INVALID的话等于删除Key
		//virtual int GetContainedKeyCount()const		{return 0;}				///用于和下面的组合来遍历，所以要保证就是内部KeyIndex的多少
		//virtual std::pair<DWORD, int> GetContainedKeyIndexByOrder(int nOrder)const{	return std::pair<DWORD, int>(KG3DTYPE_ENVEFF_INVALID_EFFECT, -1);}
		//virtual bool SetContainedKeyIndexByOrder(KG3DDWGC_KeyTableManagerBase& RefTableMgr, int nOrder, INT nNewIndex){return false;}
		virtual DWORD GetType()const = 0;
		//virtual bool IsLegal(KG3DDWGC_KeyTableManagerBase* pTableManager)const {return true;}//用于检查加载是否成功和能否用于渲染，不是必须
		//virtual void CopyLegalPartsOfAnotherKey(const KG3DDWGC_KeyBase& OtherKeyBase, KG3DDWGC_KeyTableManagerBase& TableManager ){}
		//virtual void ReleaseContainedKey(KG3DDWGC_KeyTableManagerBase* pTableManager, bool OnlyInterp){};///卸载下属的Key，没有的话可以写空函数	
		///记得最后把这些东东去掉,现在为了强制所有Key都必须有这两个方法，所以就暂时加上
#if defined(DEBUG) | defined(_DEBUG)
		virtual bool SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection ) = 0;
		virtual bool LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection ) = 0;
		virtual void DebugPrint(std::ostringstream& OSS) = 0;//注意这个不支持UNICODE
#endif		
	};
	//////////////////////////////////////////////////////////////////////////
//	struct KG3DDWGC_KeyTableBase0 : public IEKG3DEnvEffDWGC_KeyTable0///注意其派生类应该可以强转成正确的类型
//	{
//		enum
//		{
//			DWGC_TABLE_MAX_KEYNAME_SIZE = 64,
//			DWGC_TABLE_MAX_KEYVALUE_SIZE = 64,			
//		};
//		virtual ~KG3DDWGC_KeyTableBase0() = 0{}
//		///<接口>
//		virtual ULONG GetSize() = 0;
//		virtual HRESULT GetType(DWORD* pdwType) = 0;
//		virtual HRESULT RemoveKey(INT nIndex) = 0;
//		virtual HRESULT GetElementIndices(INT* pnIndex, INT nArraySize, INT* pnRealWritedIndexCount) = 0;///得到全部的Index
//		///关键帧界面
//		virtual HRESULT GetDescription(INT nIndex, LPTSTR pDesc, INT nBufferSize) = 0;
//		virtual HRESULT KeyElementCount(INT* pSize) = 0;
//		virtual HRESULT ConvertValueString(INT nWhichKey, INT nWhichPart, LPTSTR tczString, INT nBufferSize, BOOL bGetOrSet) = 0;
//		///</接口>
//		virtual void Init(KG3DDWGC_KeyTableManagerBase* pTableManager) = 0;
//		virtual KG3DDWGC_KeyBase& GetInterpKey(int& _Keyval) = 0;	//用CheckIsKeyInterp或者CheckIsKeyValue来判断成功，注意这些方法的返回应该可以强转成正确类型		
//		virtual KG3DDWGC_KeyBase* GetKey(const int& _Keyval) = 0;		///在多态下，还是指针好用0
//		virtual KG3DDWGC_KeyBase* GetOneEmptyKey(int& _Keyval) = 0;
//		virtual KG3DDWGC_KeyBase* GetSpecialKey(DWORD dwTrait, int& _Keyval) = 0;
//		virtual KG3DDWGC_KeyBase* Find(const int& _Keyval) = 0;
//		virtual bool ReleaseKeyAndContainedKey(const int _Keyval, bool bOnlyInterp) = 0;
//		virtual bool Empty() = 0;
//		virtual int PushBack(KG3DDWGC_KeyBase* pBaseKey) = 0;
//		virtual bool Clear() = 0;
//		///最后要不考虑利用这些虚函数，要不就删除掉
//		virtual bool SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection ) = 0;
//		virtual bool LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection ) = 0;
//		virtual void DoNothing() = 0;
//#if defined(DEBUG) | defined(_DEBUG)
//		virtual void DebugPrintTable() = 0;
//#endif
//
//	};
	struct KG3DDWGC_KeyTableBase1 : public KG3D_CUSTOM_HELPERS::KG3DLifeTimeDependedClassBase<KG3DENVEFF_MAX_SUBEFFECT>,
									public IEKG3DEnvEffDWGC_KeyTable1
	{
		virtual ~KG3DDWGC_KeyTableBase1() = 0{}//虚析构是必须的

		//基本管理
		virtual KG3DDWGC_KeyBase* New(KG3DENVEFF_DWGCKEY_HANDLE) = 0;
		virtual HRESULT Del(KG3DENVEFF_DWGCKEY_HANDLE) = 0;
		virtual KG3DDWGC_KeyBase* Get(KG3DENVEFF_DWGCKEY_HANDLE) = 0;
		virtual HRESULT Clear() = 0;
		virtual ULONG	Size()const = 0;
		virtual DWORD	GetType()const = 0;
		
		//对Region的管理
		virtual HRESULT DelRegion(INT nRegion) = 0;
		virtual HRESULT MoveAllKeysInRegionToNewRegion(INT nRegion, INT nNewRegion) = 0;
		virtual ULONG	GetKeyCountOfRegion(INT Region) = 0;
		virtual ULONG	IsAnyKeyInRegion(INT Region) = 0;	
		
		//<接口>
		virtual ULONG	GetSize(){return Size();}
		virtual HRESULT GetType(DWORD* pdwType){if(pdwType){*pdwType = this->GetType(); return S_OK;}return E_FAIL;}
		virtual HRESULT GetFirstKeyHandle(KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo) = 0;
		virtual HRESULT GetNextKeyHandle(KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo) = 0;
		virtual HRESULT GetFirstKeyHandleOfRegion(INT Region, KG3DENVEFF_DWGCKEY_HANDLE* pHandle, LPVOID pExtraInfo) = 0;
		virtual HRESULT GetNextKeyHandleOfRegion(INT Region, KG3DENVEFF_DWGCKEY_HANDLE PrevHandle, KG3DENVEFF_DWGCKEY_HANDLE* pNextHandle, LPVOID pExtraInfo) = 0;
		virtual HRESULT GetPrevKeyHandle(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DENVEFF_DWGCKEY_HANDLE* pPrevHandle) = 0;
		//</接口>
		
		//得到插值
		virtual	HRESULT GetInterpKey(KG3DENVEFF_DWGCKEY_HANDLE KeyHandle, KG3DDWGC_KeyBase& RetKey) = 0;
		virtual std::auto_ptr<KG3DDWGC_KeyBase> NewKey() = 0;	//注意auto_ptr的用法

		//拷贝,不允许覆盖
		virtual HRESULT CopyTo(KG3DENVEFF_DWGCKEY_HANDLE Src, KG3DDWGC_KeyTableBase1& DescTable, KG3DENVEFF_DWGCKEY_HANDLE Desc, BOOL bByValueOrByRef) = 0;

		//读取
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection) = 0;
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection) = 0;
	};
	typedef KG3DDWGC_KeyTableBase1 KG3DDWGC_KeyTableBase;
	///KG3DDWGC_KeyBase的Dummy，所有都会返回失败
	struct KG3DDWGC_KeyDummy : public KG3DDWGC_KeyBase
	{
		virtual bool CalculateInterp(const KG3DDWGC_KeyBase& BaseKeyBefore, const KG3DDWGC_KeyBase& BaseKeyAfter, float fPercent){return false;}	
		//virtual KG3DDWGC_KeyBase* GetContainedKey(KG3DDWGC_KeyTableManagerBase& RefTableMgr, DWORD dwType, int nFinded)const	{return NULL;}
		//virtual INT GetContainedKeyIndex(KG3DDWGC_KeyTableManagerBase& RefTableMgr, DWORD dwType, int nFinded)const	{return DW_KEYINDEX_INVALID;}
		//virtual bool SetContainedKeyIndex(KG3DDWGC_KeyTableManagerBase& RefTableMgr, DWORD dwType, int nFinded, INT nNewIndex)	{return false;}	///不管释放，nNewIndex = DW_KEYINDEX_INVALID的话等于删除Key
		//virtual int GetContainedKeyCount()const		{return 0;}				
		//virtual std::pair<DWORD, int> GetContainedKeyIndexByOrder(int nOrder)const{	return std::pair<DWORD, int>(KG3DTYPE_ENVEFF_INVALID_EFFECT, -1);}
		//virtual bool SetContainedKeyIndexByOrder(KG3DDWGC_KeyTableManagerBase& RefTableMgr, int nOrder, INT nNewIndex){return false;}
		virtual DWORD GetType()const {return KG3DTYPE_ENVEFF_INVALID_EFFECT;}
		//virtual bool IsLegal(KG3DDWGC_KeyTableManagerBase* pTableManager) const{return false;}
		//virtual void CopyLegalPartsOfAnotherKey(const KG3DDWGC_KeyBase& OtherKeyBase, KG3DDWGC_KeyTableManagerBase& TableManager ){}
		
#if defined(DEBUG) | defined(_DEBUG)
		virtual bool SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection ) {return false;}
		virtual bool LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection ) {return false;}
		//virtual void ReleaseContainedKey(KG3DDWGC_KeyTableManagerBase* pTableManager, bool OnlyInterp){}
		virtual void DebugPrint(std::ostringstream& OSS){}
#endif		
	};
	///KeyTable的Dummy，所有方法都会失败，用于引用形式的返回
	/*struct KG3DDWGC_KeyTableDummy : public KG3DDWGC_KeyTableBase0
	{
		virtual ULONG GetSize(){return 0;}
		virtual HRESULT GetType(DWORD* pdwType){return E_FAIL;}
		virtual HRESULT RemoveKey(INT nIndex){return E_FAIL;}
		virtual HRESULT GetElementIndices(INT* pnIndex, INT nArraySize, INT* pnRealWritedIndexCount){return E_FAIL;}///得到全部的Index
		///关键帧界面
		virtual HRESULT GetDescription(INT nIndex, LPTSTR pDesc, INT nBufferSize){return E_FAIL;}
		virtual HRESULT KeyElementCount(INT* pSize){return E_FAIL;}
		virtual HRESULT ConvertValueString(INT nWhichKey, INT nWhichPart, LPTSTR tczString, INT nBufferSize, BOOL bGetOrSet){return E_FAIL;}
		///</接口>
		virtual void Init(KG3DDWGC_KeyTableManagerBase* pTableManager){}
		virtual KG3DDWGC_KeyBase& GetInterpKey(int& _Keyval){static KG3DDWGC_KeyDummy KeyDummy; return KeyDummy;}	
		virtual KG3DDWGC_KeyBase* GetKey(const int& _Keyval){return NULL;}		
		virtual KG3DDWGC_KeyBase* GetOneEmptyKey(int& _Keyval){return NULL;}
		virtual KG3DDWGC_KeyBase* GetSpecialKey(DWORD dwTrait, int& _Keyval){return NULL;}
		virtual KG3DDWGC_KeyBase* Find(const int& _Keyval){return NULL;}
		virtual bool ReleaseKeyAndContainedKey(const int _Keyval, bool bOnlyInterp){return false;}
		virtual bool Empty(){return true;}
		virtual int PushBack(KG3DDWGC_KeyBase* pBaseKey){return DW_KEYINDEX_INVALID;}
		virtual bool Clear(){return true;}
		///最后要不考虑利用这些虚函数，要不就删除掉
		virtual bool SaveMapSetting( IIniFile* pIniFile, LPCTSTR pSection ){return true;}
		virtual bool LoadMapSetting( IIniFile* pIniFile, LPCTSTR pSection ){return true;}
		virtual void DoNothing(){}
#if defined(DEBUG) | defined(_DEBUG)
		virtual void DebugPrintTable(){}
#endif
	};*/
	//////////////////////////////////////////////////////////////////////////
	/*inline DWORD GetTableType(KG3DDWGC_KeyTableBase0& Table)
	{
		DWORD dwType = KG3DTYPE_ENVEFF_INVALID_EFFECT;
		KG_COM_PROCESS_ERROR(Table.GetType(&dwType));
		return dwType;
Exit0:
		return KG3DTYPE_ENVEFF_INVALID_EFFECT;
	}*/
	//////////////////////////////////////////////////////////////////////////
	inline bool CheckIsIndexInterp(int nIndex){return  (nIndex >= DW_KEYINDEX_INTERP_START && nIndex <= DW_KEYINDEX_INTERP_END);}
	inline bool CheckIsIndexValidAndNotInterp(int nIndex){return (nIndex >= DW_KEYINDEX_VALID_MIN && nIndex < DW_KEYINDEX_INTERP_START);}
	inline bool CheckIsIndexValid(int nIndex){return (nIndex >= DW_KEYINDEX_VALID_MIN && nIndex <= DW_KEYINDEX_VALID_MAX);}

	inline bool CheckIsHourIndexValid(DW_HOUR nHour){return (nHour >= KG3DENVEFFTI_HOUR_MIN && nHour <= KG3DENVEFFTI_HOUR_MAX);}
	inline bool CheckIsRegionIndexValid(DW_REGION_TAB nRegion){return (nRegion >= KG3DENVEFFRI_REGION_MIN && nRegion <= KG3DENVEFFRI_REGION_MAX);}
	inline bool CheckIsKeyHandleValid(KG3DENVEFF_DWGCKEY_HANDLE Handle){return CheckIsRegionIndexValid(Handle.m_Region) && CheckIsHourIndexValid(Handle.m_Hour);}
	
	template<typename _TypeKey>
	inline bool CheckIsKeyAnErrorKey(const _TypeKey& Key)
	{
		_KG3D_DEBUG_STATIC_CHECK_T(_KG3D_SUPERSUBCLASS(KG3DDWGC_KeyBase, _TypeKey), KeyIsNotPublicFromKeyBase);
		return (Key.m_dwModificationOption & DW_KEYTRAIT_ERROR) ? true : false;
	}
	template<typename _TypeToCheck>
	inline bool CheckIsObjectTraitError(const _TypeToCheck& Obj)
	{
		return Obj.GetTrait() == DW_KEYTRAIT_ERROR;
	}
	inline INT DWGCGetNextHour(INT nHour)
	{
		++nHour;
		if (nHour > KG3DENVEFFTI_HOUR_IN_A_DAY)
		{
			nHour = nHour % KG3DENVEFFTI_HOUR_IN_A_DAY;
			if(nHour == 0)
				nHour = KG3DENVEFFTI_HOUR_IN_A_DAY;
		}
		return nHour;
	}
	template<class _Type>
	void BalanceTwoVectors(std::vector<_Type>& ArrayBefore
		, std::vector<_Type>& ArrayAfter
		, size_t MaxSize)//根据两个Array对其自身Array的策略
	{
		typedef std::vector<_Type> TypeArray;

		size_t nBeforeSize = ArrayBefore.size();
		size_t nAfterSize = ArrayAfter.size();
		if (nBeforeSize != nAfterSize)
		{
			size_t nLimit = 0;///防止无限循环
			while (nBeforeSize < nAfterSize && nLimit++ < MaxSize)
			{
				ArrayBefore.push_back(ArrayAfter[nBeforeSize]);
				nBeforeSize = ArrayBefore.size();
			}
			nLimit = 0;
			while (nBeforeSize > nAfterSize && nLimit++ < MaxSize)
			{
				ArrayAfter.push_back(ArrayBefore[nAfterSize]);
				nAfterSize = ArrayAfter.size();
			}
			_ASSERTE(ArrayBefore.size() == ArrayAfter.size());
		}


		return;
	}
	//////////////////////////////////////////////////////////////////////////
//	struct KG3DDWGC_KeyTableManagerBase0
//	{
//		//virtual bool IsTableInUse(DWORD dwType) = 0;
//		virtual KG3DDWGC_KeyTableBase0& GetTableByType(DWORD dwType) = 0;
//		virtual ~KG3DDWGC_KeyTableManagerBase0() = 0{}
//		virtual INT IsTypeLegal(DWORD dwType) = 0;	///illegal的话返回-数，否则返回索引，
//#if defined(_DEBUG) | defined(DEBUG)
//		KG3DDWGC_KeyTableManagerBase0(){}
//	private:
//		KG3DDWGC_KeyTableManagerBase0(const KG3DDWGC_KeyTableManagerBase0& Other){}
//#endif
//	};

//	struct KG3DDWGC_KeyTableManagerBase1
//	{
//		//virtual bool IsTableInUse(DWORD dwType) = 0;
//		virtual KG3DDWGC_KeyTableBase0& GetTableByType(DWORD dwType) = 0;
//		virtual ~KG3DDWGC_KeyTableManagerBase1() = 0{}
//		virtual INT IsTypeLegal(DWORD dwType) = 0;	///illegal的话返回-数，否则返回索引
//#if defined(_DEBUG) | defined(DEBUG)
//		KG3DDWGC_KeyTableManagerBase1(){}
//	private:
//		KG3DDWGC_KeyTableManagerBase1(const KG3DDWGC_KeyTableManagerBase1& Other){}
//#endif
//	};
//
//	typedef KG3DDWGC_KeyTableManagerBase1 KG3DDWGC_KeyTableManagerBase;

	struct KG3DDWGC_KeyTableGeneratorBase : public KG3D_CUSTOM_HELPERS::KG3DLifeTimeDependedClassBase<KG3DENVEFF_MAX_SUBEFFECT> //用生命相关化机制 
	{
		virtual ~KG3DDWGC_KeyTableGeneratorBase() = 0{}
		virtual bool ReleaseTable(KG3DDWGC_KeyTableBase* pTable) = 0;
		virtual KG3DDWGC_KeyTableBase* RequestANewTable() = 0;
		//用于遍历
		virtual size_t Size() = 0;
		virtual KG3DDWGC_KeyTableBase& GetTable(size_t i) = 0;
		virtual DWORD  GetType() = 0;
		//读取
		virtual HRESULT SaveMapSetting(IIniFile* pIniFile, LPCTSTR pSection) = 0;
		virtual HRESULT LoadMapSetting(IIniFile* pIniFile, LPCTSTR pSection) = 0;
		virtual VOID	SortTables(const std::vector<KG3DDWGC_KeyTableBase*>& VecOfAllTablesInEffects) = 0;
	};

};///KG3D_ENVIRONMENT_EFFECT

#endif //_INCLUDE_KG3DENVEFFDW_KEYBASE_H_
