////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSTLWrappers.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-19 17:22:25
/*  Comment     : 
用来包装STL那些麻烦的语法，会损失一点用虚函数的性能，但是
程序慢基本不可能因为你不用虚函数就改善的，基本都是算法问题，所以没有关系。
用了Wrapper可以统一STL的访问方式，不会因为老改容器类型而需要改动代码

最重要的，使用Proxy的理由是——封装，绝对不能暴露底层容器

注意：
1.如果作为成员使用，Proxy应该在其代理的容器后面声明，不然它析构的时候因为原容器已经析构，会引发错误。
这点和Iteractor做容器是一致的

2.Proxy的循环过程中对容器做改变操作要十分注意。就是不应该在循环内
insert或者delete。

这点对于std容器，类似容器的类，普通数组都是一样的。一时方便，错误的几率
就隐形的增大了。

使用方式:
1.假设写了这样一个函数
void PrintCt(const KGCH::FrontAccessProxy& Proxy)
{
	for (Proxy.Begin(); ! Proxy.IsEnd(); Proxy.StepForward())
	{
		INT* p = (INT*)Proxy.Cur();
		std::cout << *p << " ";
	}
}

调用方式如下：
std::set<INT> TempSet;

PrintCt(KGCH::CreateFrontAccessProxy(TempSet));

如果变成了map
std::map<INT> TempSet;
调用方式和函数本身没有任何变化

2.假如类ClassA有成员std::set<INT> m_set;
需要让ClassA的使用者访问里面的元素，又不想暴露里面的东西
那么

这样子

ClassA
{
	...
public:
	typedef KGCH::TFrontAccessProxy<INT>	TypeAccessProxy;	
	//也可以是typedef KGCH::FrontAccessProxy TypeAccessProxy。这样子GetCur不可用，但可以用Cur得到LPVOID，没有模板，更简洁一些

	TypeAccessProxy& GetProxy(){return m_Proxy;}

private:
	typedef std::set<INT> TypeCt;

	TypeCt	m_set;

	KGCH::TFrontAccessProxySolid<TypeCt>	m_Proxy;

public:
	//在构造函数里面把Proxy和容器本身连接起来
	ClassA():m_Proxy(m_set){}
};

那样无论怎么改容器，外部代码都不需要做任何改动。而且即使以后变成了非std容器
，那么自定义Proxy，实现KGCH::TFrontAccessProxy<INT>的接口，外部代码还是一行也不用改
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSTLWRAPPER_H_
#define _INCLUDE_KGSTLWRAPPER_H_
#include <map>
////////////////////////////////////////////////////////////////////////////////
namespace KG_CUSTOM_HELPERS
{
	enum{
		em_frontAccessProxy_cap_forward = 1 << 0,
		em_frontAccessProxy_cap_bidirection = 1 << 1,
	};

	namespace Private
	{
		inline BOOL IsForwardIteratorTag(...){return FALSE;}
		inline BOOL IsForwardIteratorTag(std::forward_iterator_tag*){return TRUE;}
		inline BOOL IsBidirectionalIteratorTag(...){return FALSE;}
		inline BOOL IsBidirectionalIteratorTag(std::bidirectional_iterator_tag*){return TRUE;}

		template<typename iteratorType>
		ULONG GetIteratorCapMask()
		{
			iteratorType::iterator_category* p = NULL; 
			ULONG ret = 0;
			if(IsForwardIteratorTag(p))
				ret |= em_frontAccessProxy_cap_forward;
			if(IsBidirectionalIteratorTag(p))
				ret |= em_frontAccessProxy_cap_bidirection;
			return ret;
		}
	};

	struct FrontAccessProxy 
	{
		virtual VOID Begin() = 0;
		virtual BOOL IsEnd()const = 0;
		virtual VOID StepForward() = 0;
		virtual VOID StepBackward() = 0;	//不一定是双向的Proxy不一定能StepBackward()，请看能力标志，能后向的一定能前向，所以要查bidirection，关于这个类目，请参考STL标准库那边书
		virtual VOID GoToEnd() = 0;	//End指最后一个元素之后的位置，和stl的容器意义应该一致
		virtual	LPVOID	Cur() = 0;

		virtual BOOL IsContentThisType(const type_info& info)const  = 0;
		virtual BOOL IsEmpty()const = 0;
		virtual ULONG	GetCapMask()const = 0;	//得到迭代器的性质

		virtual ~FrontAccessProxy() = 0{}

		LPVOID GetBackElemPointer();//这个会移动iterator。使用之前要查IsEmpty
	};

	template<class Type>
	struct TFrontAccessProxy : public FrontAccessProxy	//这个类是用来加强类型检查的，要保证能和FrontAccessProxy强转，不能加虚函数，不然用到强制的地方可能出错
	{
		Type& GetCur()		{return *static_cast<Type*>(this->Cur());}
		Type& GetFront()	{this->Begin();return GetCur();}	//这个会移动iterator的，小心
		Type& GetBack()		{return *static_cast<Type*>(this->GetBackElemPointer());}	//这个会移动iterator。使用之前要查IsEmpty
	};

	template<class Type>
	struct TFrontAccessProxySolid;

	template<class Type>
	struct TFrontAccessProxySolid : public TFrontAccessProxy<typename Type::value_type>
	{
		typedef typename Type::reference reference;
		typedef typename Type::value_type value_type;
		typedef typename Type::iterator iterator;
	public:
		explicit TFrontAccessProxySolid(Type& Ct):m_Ct(Ct){m_it = Ct.end();}
		TFrontAccessProxySolid(const TFrontAccessProxySolid& Other):m_Ct(Other.m_Ct), m_it(Other.m_it){}

		virtual VOID Begin(){m_it = m_Ct.begin();}
		virtual BOOL IsEnd()const{return m_it == m_Ct.end();}
		virtual VOID StepForward(){++m_it;}
		virtual VOID StepBackward(){--m_it;}
		virtual	LPVOID	Cur(){return &(*m_it);}
		virtual VOID GoToEnd(){m_it = m_Ct.end();}
		
		virtual BOOL IsContentThisType(const type_info& info)const{return typeid(value_type) == info;}
		virtual BOOL IsEmpty()const{return m_Ct.empty();}

		virtual ULONG	GetCapMask()const{return Private::GetIteratorCapMask<iterator>();}
	protected:
		mutable Type&	m_Ct;
		mutable iterator m_it; 
	};

	template<class TypeKey, class TypeValue>
	struct TFrontAccessProxySolid<std::map<TypeKey, TypeValue> > : public TFrontAccessProxy<TypeValue>
	{
		typedef std::map<TypeKey, TypeValue> Type;
		typedef typename Type::reference reference;
		typedef typename Type::iterator iterator;
		typedef TypeValue value_type;
	public:
		explicit TFrontAccessProxySolid(Type& Ct):m_Ct(Ct){m_it = Ct.begin();}
		TFrontAccessProxySolid(const TFrontAccessProxySolid& Other):m_Ct(Other.m_Ct), m_it(Other.m_it){}

		virtual VOID Begin(){m_it = m_Ct.begin();}
		virtual BOOL IsEnd()const{return m_it == m_Ct.end();}
		virtual VOID StepForward(){++m_it;}
		virtual VOID StepBackward(){--m_it;}
		virtual	LPVOID	Cur(){return &(m_it->second);}
		virtual VOID GoToEnd(){m_it = m_Ct.end();}

		virtual BOOL IsContentThisType(const type_info& info)const{return typeid(TypeValue) == info;}
		virtual BOOL IsEmpty()const{return m_Ct.empty();}

		virtual ULONG	GetCapMask()const{return Private::GetIteratorCapMask<iterator>();}
	protected:
		mutable Type&	m_Ct;
		mutable iterator m_it; 
	};

	template<class TypeElem>
	struct TFrontAccessProxyArray : public TFrontAccessProxy<TypeElem> 
	{
	public:
		template<size_t ArraySize>
		TFrontAccessProxyArray(TypeElem (&Ct)[ArraySize])
			:m_Ct(Ct)
			,m_uArraySize(ArraySize)
		{
			m_it = m_Ct;
		}
		TFrontAccessProxyArray(const TFrontAccessProxyArray& Other):m_Ct(Other.m_Ct), m_it(Other.m_it){}

		virtual VOID Begin(){m_it = m_Ct;}
		virtual BOOL IsEnd()const{return m_it == m_Ct + m_uArraySize;}
		virtual VOID StepForward(){++m_it;}
		virtual VOID StepBackward(){--m_it;}
		virtual	LPVOID	Cur(){return &(*m_it);}
		virtual VOID GoToEnd(){m_it = m_Ct + m_uArraySize;}

		virtual BOOL IsContentThisType(const type_info& info)const{return typeid(TypeElem) == info;}
		virtual BOOL IsEmpty()const{return false;}

		virtual ULONG	GetCapMask()const{return em_frontAccessProxy_cap_bidirection;}
	protected:
		mutable TypeElem* m_Ct;
		mutable TypeElem* m_it;
		const size_t m_uArraySize;
	};

	template<class Type>
	TFrontAccessProxySolid<Type> CreateFrontAccessProxy(Type& Ct){return TFrontAccessProxySolid<Type>(Ct);}
};
namespace KGCH = KG_CUSTOM_HELPERS;
#endif //_INCLUDE_KGSTLWRAPPER_H_
