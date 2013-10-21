////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGHelperClasses.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-3-20 19:18:47
/*  Comment     : 帮助类，里面绝对不能有任何需要依赖工程任何文件的东西
//			这个东西也提供给SceneEditor使用

1.TTypeCopy
用于自动的基类转换，和自动太小
如Class A; Class B : public A;

A a;
B b;
TTypeCopy<A>(&b, &a);

2.TTypeCmp
同上，用于基类Binary比较



3.ForAll
class AClass
{
public:
	HRESULT Render();
}
std::map<xx,xx> AMap;
KG_CUSTOM_HELPERS::ForAll(AMap, std::mem_func_ref(&AClass::Render));

对于ForAll要注意的是，微软的std::map对for_each兼容不是很好，所以要有下面的ForAll对map的特化版本

4.TContainerDelete
TContainerDelete(AMap);	//里面是指针才行，会自动调用delete

5.TContainerRelease 同上，但是是用Release清除里面东西
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGHELPERCLASSES_H_
#define _INCLUDE_KGHELPERCLASSES_H_
#include <map>
#include <vector>
#include <set>
#include <algorithm>
//////////////////////////////////////////////////////////////////////////

namespace KG_CUSTOM_HELPERS
{
	/************************************************************************/
	/*函数模板不能就非class类型偏特化，只能重载，下面就是实现 非class类型到
	class的帮助类
	*/
	/************************************************************************/
	template<int num>
	struct TNumToType 
	{
		enum{result = num,};
	};
	/************************************************************************/
	/*用于指定一个空类 
	*/
	/************************************************************************/
	struct NullType 
	{
	};
	/************************************************************************/
	/* 用于选择一个类
	*/
	/************************************************************************/
	template<bool _Bool, typename T1, typename T2> struct TTypeSelector{typedef T1 Result;};
	template<typename T1, typename T2> struct TTypeSelector<false, T1, T2>{typedef T2 Result;};
	//////////////////////////////////////////////////////////////////////////
	///需要显式指定模板参数
	template<class _DescStructBase>
	inline bool TTypeCopy(_DescStructBase* pDesc, const _DescStructBase* pSrc)
	{	
		_ASSERTE(pDesc && pSrc);
		return 0 == memcpy_s(pDesc, sizeof(_DescStructBase), pSrc, sizeof(_DescStructBase));
	}
	template<class _DescStructBase>
	inline bool TTypeCmp(_DescStructBase* pDesc, const _DescStructBase* pSrc)
	{
		return 0 == memcmp(pDesc, pSrc, sizeof(_DescStructBase));
	}
	//for_each加强版

	template<class _Container,class _Fn1> 
	inline
	_Fn1 ForAll(_Container& Container, _Fn1 _Func)
	{
		return std::for_each(Container.begin(), Container.end(), _Func);
	}

	template<class _ContainedType,size_t _Size, class _Fn1> 
	inline
		_Fn1 ForAll(_ContainedType (&Container)[_Size], _Fn1 _Func)
	{
		for (size_t i = 0; i < _Size; ++i)
		{
			_Func(Container[i]);
		}
		return _Func;
	}

	//用于std::map的for_each加强版，这样用KG_CUSTOM_HELPERS::ForAll(map, std::mem_func(&ClassA::DoSomething));
	template<class _Key, class _Type, class _Fn1>
	inline _Fn1 ForAll(std::map<_Key, _Type>& Container, _Fn1 _Func)
	{
		typedef std::map<_Key, _Type> TypeMap;
		struct PairSeperator 
		{
			_Fn1& m_Func;
			PairSeperator(_Fn1& Func):m_Func(Func){}
			void operator()(TypeMap::value_type& Value)
			{
				m_Func(Value.second);
			}
			operator _Fn1(){return m_Func;}
		};

		return std::for_each(Container.begin(), Container.end(), PairSeperator(_Func));
	};

	namespace Private
	{
		template<class _Fn1, class _Cmp> 
		class DoIf
		{
			_Fn1 m_Func;
			_Cmp m_CmpFunc;
		public:
			DoIf(_Fn1 _Func, _Cmp _CmpFunc):m_Func(_Func),m_CmpFunc(_CmpFunc){}
			template<class _Type>
			void operator()(_Type& A)
			{
				if(m_CmpFunc(A))
					m_Func(A);
			}
			template<class _Type>
			void operator()(const _Type& A)
			{
				if(m_CmpFunc(A))
					m_Func(A);
			}		
			operator _Fn1(){return m_Func;}
		};

	};

	struct IsNotNull
	{
		bool operator()(LPVOID APointer){return APointer != NULL;}
	};

	template<class _Container, 	class _Fn1, class _Cmp> 
	inline _Fn1 ForAllIf(_Container& Container, _Fn1 _Func, _Cmp _CmpFunc)
	{
		return ForAll(Container, Private::DoIf<_Fn1, _Cmp>(_Func, _CmpFunc));
	}
	
	template<class _Container, 	class _Fn1> 
	inline _Fn1 ForAllIfNotNull(_Container& Container, _Fn1 _Func)
	{
		return ForAllIf(Container, _Func, IsNotNull());
	}
	/************************************************************************/
	/*下面的用于清空标准容器
	*/
	/************************************************************************/
	struct SafeDelete 
	{
		template<class _Type>
		void operator()(_Type*& pType){SAFE_DELETE(pType);}
	};

	struct SafeRelease 
	{
		template<class _Type>
		void operator()(_Type*& pType){SAFE_RELEASE(pType);}
	};	

	template<class _Type>
	BOOL TContainerDelete(_Type& Container)
	{
		ForAll(Container, SafeDelete());		
		Container.clear();
		return TRUE;
	}

	template<class _Type, size_t _Size>
	BOOL TContainerDelete(_Type (&Container)[_Size])
	{
		ForAll(Container, SafeDelete());
		return TRUE;
	}

	template<class _Type>
	BOOL TContainerRelease(_Type& Container)
	{
		ForAll(Container, SafeRelease());		
		Container.clear();
		return TRUE;
	}
	
	template<class _Type, size_t _Size>
	BOOL TContainerRelease(_Type (&Container)[_Size])
	{
		ForAll(Container, SafeRelease());
		return TRUE;
	}

	/*
	IteratorHolder的原理很简单。基于容器实现分离的原则，我们不能暴露容器作为接口，也就不能暴露iterator作为接口。
	COM的解决方法是IEnum，但是IEnum需要频繁创建，释放，如果要频繁访问的话，对堆栈不是好事。
	如果在类内部实现IEnum的Reset，Next，那么需要保存一个iterator，这样子很好，不用生命周期管理，也很简单明了。
	但这样子如果嵌套遍历一定会出错。
	下面是一个很简单的解决方案。返回一个Buffer作为Handle，实现GetFirst，GetNext型接口。那么这个Handle可以是任何东西。
	如果在Buffer里面直接放iterator的话，那么实现就更方便了。同时这也不违反OpenClose原则，这个Handle根据需要可以
	是任何东西的，只要你的GetFirst和GetNext能够解释它。
	*/
	class IteratorHolder 
	{
		enum
		{
			em_map_iterator_size = sizeof(std::map<int, int>::iterator),
			em_vector_iterator_size = sizeof(std::vector<int>::iterator),
			em_list_iterator_size = sizeof(std::list<int>::iterator),
			em_buffer_size_temp = em_vector_iterator_size > em_map_iterator_size ? em_vector_iterator_size : em_map_iterator_size,
			em_buffer_size = em_list_iterator_size > em_buffer_size_temp ? em_list_iterator_size : em_buffer_size_temp,
		};
	private:
		BYTE	m_Buffer[em_buffer_size];
	};

	template<class Type>
	inline IteratorHolder CreateIteratorHolder(Type& container)
	{
		typedef typename Type::iterator iterator;
		IteratorHolder holder;
		_ASSERTE(! container.empty());
		iterator it = container.begin();
		memcpy_s(&holder, sizeof(IteratorHolder), &it, sizeof(iterator));
		return holder;
	}

};
namespace KGCH = KG_CUSTOM_HELPERS;
#endif //_INCLUDE_KGHELPERCLASSES_H_
