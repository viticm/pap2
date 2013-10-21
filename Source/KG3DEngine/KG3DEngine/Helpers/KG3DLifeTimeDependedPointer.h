////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : 
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-16 20:11:59
//  Comment     : 用于在头文件中Include的类的帮助类

/*
这个类主要解决这样的问题：
A类和B类之间没有任何空间上的直接关系，而B类需要经常读A类以获得某些信息。
而如果B类每次要获得A类的话，就要从类的层次上跋涉很长的路径索取A类的话，
这样不单会造成代码很难读，依赖性很强，而且也会造成一些性能消耗。

简单的，如果A类的生命周期一定比B类长，那么B类直接拥有A类的指针就好了
如A类间接拥有B类的情况

而如果它们之间不能保证生命周期的话，即使用了引用计数管理，也是不对的，
因为不能因为他们之间有逻辑关系就妨碍一方的释放。

本来用MessageSender那一套也是可以的，但是这里并不需要相互通知事件，所以
用那套还是不很方便。

这个TKG3DLifeTimeDependedPointer的意义就是把一方的指针赋予这个Pointer
，一方释放之后，这个Pointer会自动失效。
实际上内部实现的方法只是简单的相互通知而已。就是为了能像普通指针那样用这个类
而使用了一些特殊的手法。这些手法可以参考《c++设计新思维》智能指针的部分，本人拾人牙慧而已。

用法如下：
要使用这种指针，类必须由KG3DLifeTimeDependedClassBase派生，派生的时候，用模板参数指定有多少个依赖
如class B : public KG3DLifeTimeDependedClassBase<1>{};	//有一个依赖

1.另外一个类中，声明一个TKG3DLifeTimeDependedPointer<B>	m_pB;
这是一个类实例，但是作为指针使用。

2.第一次使用之前，和指针一样 m_pB = &aB;//aB是一个class B的实例，
也可以像成员初始化那样在成员初始化列表中使用。
只能赋值一次，第二次赋值就会触发ASSERT

赋值之后，m_pB不一定为有效值，因为多个依赖的话，可能会超出注册允许的大小，从而无法注册

3. if(m_pB)
	m_pB->DoSomething();	//重载了指针操作符

用的时候，可以用任何形式的if来检查其有效性：if(m_pB);if(!m_pB),if(NULL == m_pB)，之类之类的
但必须检查有效性，不然B释放了，调用的就是无效指针：


*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DTEMPLATECLASSFUNDATIONS_H_
#define _INCLUDE_KG3DTEMPLATECLASSFUNDATIONS_H_
//#include "./TemplateLibs/LOKI_APP/KG3DLokiBase.h"	//需要用到其中的SUPERSUBCLASS宏
#include <vector>
////////////////////////////////////////////////////////////////////////////////
namespace KG3D_CUSTOM_HELPERS
{
	/************************************************************************/
	/*因为这些类是帮助类，为免和派生类的成员和方法发生名字冲突，方法后面都加上_后缀 
	*/
	/************************************************************************/
	struct KG3DLifeTimeDependedPointerBase
	{
		virtual bool OnDependedClassDestroy_() = 0;
		virtual bool CanBeRegister_() const = 0;
		virtual bool CanBeUnRegister_() const = 0;
		virtual ~KG3DLifeTimeDependedPointerBase() = 0{};
	};
	//////////////////////////////////////////////////////////////////////////
	/************************************************************************/
	/*派生类获得“可被依赖指针依赖“的能力。本身的任何方法都不需要被客户调用 
	*/
	/************************************************************************/
	///用于辨别是否同一类
	namespace Private
	{
		struct KG3DLifeTimeDependedClassBaseBase
		{

		};
	};
	template<size_t _nDependOnCount>
	class KG3DLifeTimeDependedClassBase : public Private::KG3DLifeTimeDependedClassBaseBase
	{
	private:
		enum{max_depended_ptr_count = _nDependOnCount,};
		//KG3DLifeTimeDependedPointerBase* m_DependArray[PTR_COUNT];
		typedef std::vector<KG3DLifeTimeDependedPointerBase*> TypePtrArray;
		TypePtrArray m_DependArray;
		//INT m_nRegisteredCount;
	private:
		KG3DLifeTimeDependedClassBase(const KG3DLifeTimeDependedClassBase&);
		void operator=(const KG3DLifeTimeDependedClassBase&);
	public:
		KG3DLifeTimeDependedClassBase(){}
		~KG3DLifeTimeDependedClassBase()
		{
			_ASSERT(m_DependArray.size() <= max_depended_ptr_count);
			for (size_t i = 0; i < m_DependArray.size(); i++)
			{
				_ASSERTE(m_DependArray[i]);
#if defined(DEBUG) | defined(_DEBUG)
				bool bRet = 
#endif
					m_DependArray[i]->OnDependedClassDestroy_();
				_ASSERTE(bRet);
			}
		}
		bool RegisterDependence_(const KG3DLifeTimeDependedPointerBase& ADependPointer)
		{
			//INT nRegisteredCount = 0;
			//保证外部的调用总是失败，这样就不用声明为private加友员了
			if (!(ADependPointer.CanBeRegister_()))
			{
				_KG3D_DEBUG_REPORT(_STRINGER(KG3DLifeTimeDependedClassBase)_T(":")_STRINGER(RegisterDependence_)_T("错误\n"));
				return false;
			}
			_ASSERTE(m_DependArray.size()<= max_depended_ptr_count);
			if(m_DependArray.size() == max_depended_ptr_count)
				return false;
			//不能重复
			for (size_t i = 0; i < m_DependArray.size(); i++)
			{
				_ASSERTE(m_DependArray[i]);
				
				if (m_DependArray[i] == &ADependPointer)
				{
					_KG3D_DEBUG_REPORT(_STRINGER(KG3DLifeTimeDependedClassBase)_T(":")_STRINGER(RegisterDependence_)_T("错误\n"));
					return false;
				}
			}
			//_ASSERTE(nRegisteredCount < _nDependOnCount);//多次注册
			m_DependArray.push_back(const_cast<KG3DLifeTimeDependedPointerBase*>(&ADependPointer));
			return true;
		}
		bool UnRegisterDependence_(const KG3DLifeTimeDependedPointerBase& ADependPointer)
		{
			if (ADependPointer.CanBeUnRegister_())
			{
				TypePtrArray::iterator it = std::find(m_DependArray.begin(), m_DependArray.end(), &ADependPointer);
				if (it != m_DependArray.end())
				{
					m_DependArray.erase(it);
					return true;
				}
			}
			_KG3D_DEBUG_REPORT(_STRINGER(KG3DLifeTimeDependedClassBase)_T(":")_STRINGER(UnRegisterDependence_)_T("错误\n"));
			return false;
		}
		
	};///KG3DLifeTimeDependedClassBase
	/************************************************************************/
	/*分析出指针类型的帮助类 
	*/
	/************************************************************************/
	namespace Private
	{
		//////////////////////////////////////////////////////////////////////////
		template<typename _Type>
		struct IsConstType{ enum{Result = false,};};
		template<typename _Type>
		struct IsConstType<const _Type>{ enum{Result = true,};};		
	};
	/************************************************************************/
	/*依赖指针。和KG3DLifeTimeDependedClassBase一起用。当KG3DLifeTimeDependedClassBase
	析构的时候，自己也失效。每次使用之前记得比较。
	*/
	/************************************************************************/
	template<class _Type>
	class TKG3DLifeTimeDependedPointer;

	template<class _Type>
	class TKG3DLifeTimeDependedPointer<_Type*> : public KG3DLifeTimeDependedPointerBase
	{
	private:
		typedef _Type* _PtrType;
		typedef const _Type* _const_Ptr_Type;///这和const _PtrType是完全两回事
		//////////////////////////////////////////////////////////////////////////
	private:
		_PtrType m_pObj;
	public:
		TKG3DLifeTimeDependedPointer()
			:m_pObj(NULL)
		{	
			_KG3D_DEBUG_STATIC_CHECK_T(_KG3D_SUPERSUBCLASS(Private::KG3DLifeTimeDependedClassBaseBase, _Type), InValidObj);
			_KG3D_DEBUG_STATIC_CHECK_T(Private::IsConstType<_Type>::Result == false, InValidPtrType);
		}
		explicit TKG3DLifeTimeDependedPointer(const _PtrType& pObj)
			:m_pObj(NULL)
		{	
			_ASSERTE(NULL != pObj);
			if (!m_pObj)
			{
				bool bRet = pObj->RegisterDependence_(*this);
				if (bRet)
				{
					m_pObj = pObj;
					return;
				}
			}
			return;	
		}
		bool operator=(const _PtrType& pObj)///如果使用了非_PtrType类型来=就会因为下面被禁止的拷贝构造函数引发歧义错误
		{
			_ASSERTE(NULL != pObj);
			if (!m_pObj)
			{
				bool bRet = pObj->RegisterDependence_(*this);
				if (bRet)
				{
					m_pObj = pObj;
					return true;
				}
			}
			return false;			
		}
		
		~TKG3DLifeTimeDependedPointer()
		{
			if (!m_pObj)
			{
				return;				
			}
#if defined(DEBUG) | defined(_DEBUG)
			bool bRet = m_pObj->UnRegisterDependence_(*this);
			_ASSERTE(bRet);
#else
			m_pObj->UnRegisterDependence_(*this);
#endif
			m_pObj = NULL;
		}
	private:///禁止复制
		explicit TKG3DLifeTimeDependedPointer(const TKG3DLifeTimeDependedPointer&);
		void operator=(const TKG3DLifeTimeDependedPointer&);
	public:
		virtual bool CanBeRegister_() const 
		{
			return m_pObj == NULL;
		}
		virtual bool CanBeUnRegister_() const 
		{
			return m_pObj != NULL;
		}
		virtual bool OnDependedClassDestroy_()
		{
			m_pObj = NULL;
			return true;
		}
		inline _PtrType operator->()
		{
			_ASSERTE(NULL != m_pObj);
			return m_pObj;
		}
		///比较函数集合

		inline bool operator!()const
		{
			return m_pObj == NULL;
		}
		inline friend bool operator==(const TKG3DLifeTimeDependedPointer& pTP,
			const _PtrType pObj)
		{ return pTP.m_pObj == pObj; }

		inline friend bool operator==(const _PtrType pObj,
			const TKG3DLifeTimeDependedPointer& pTP)
		{ return pObj == pTP.m_pObj; }

		inline friend bool operator!=(const TKG3DLifeTimeDependedPointer& pTP,
			const _PtrType pObj)
		{ return !(pTP.m_pObj == pObj); }

		inline friend bool operator!=(const _PtrType pObj,
			const TKG3DLifeTimeDependedPointer& pTP)
		{ return !(pObj == pTP.m_pObj); }
		
		inline friend _PtrType GetPtr(TKG3DLifeTimeDependedPointer& pTP){return pTP.m_pObj;}
	private:
		// 用于'if (sp)'形式的比较。除了这种自动转型之外，其他都是不安全的，
		// 指针一旦有了自动转型就没有所有智能指针的保护了
		 struct DummyClass
		 {
			 DummyClass() {}
			 private:
				 void operator delete(void*);//这是为了防止写出delete p来
		 };

	public:
		operator DummyClass*() const
		{
			if (!*this) return 0;
			static DummyClass t;
			return &t;
		}


		
	};///TKG3DLifeTimeDependedPointer
};///KG3D_CUSTOM_HELPERS



#endif //_INCLUDE_KG3DTEMPLATECLASSFUNDATIONS_H_
