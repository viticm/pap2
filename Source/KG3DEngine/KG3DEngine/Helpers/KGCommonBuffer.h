////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGCommonBuffer.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-25 9:35:17
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGCOMMONBUFFER_H_
#define _INCLUDE_KGCOMMONBUFFER_H_

#include "KGCommonWidgets.h"
#include "KGHelperClasses.h"
////////////////////////////////////////////////////////////////////////////////
namespace KG_CUSTOM_HELPERS
{
	namespace Private
	{
		//用于创建的类，可以覆盖。有空的话，应该改成符合std命名规范的样式
		template<typename _Type>
		struct TTypePlacementNewer 
		{
			template<size_t uSize>
			inline _Type* MakeType(char (&uBuffer)[uSize])
			{
				_KG3D_DEBUG_STATIC_CHECK_T(uSize >= sizeof(_Type), SizeTooSmall);
				return new(uBuffer) _Type();
			}
			template<size_t uSize>
			inline _Type* MakeType(char (&uBuffer)[uSize], const _Type& Other)
			{
				_KG3D_DEBUG_STATIC_CHECK_T(uSize >= sizeof(_Type), SizeTooSmall);
				return new(uBuffer) _Type(Other);
			}
			template<size_t uSize>
			inline void CopyType(char (&uBuffer)[uSize], const _Type& Other)
			{
				_KG3D_DEBUG_STATIC_CHECK_T(uSize >= sizeof(_Type), SizeTooSmall);
				_Type* pType = (_Type*)uBuffer;
				*pType = Other;
			}
			template<size_t uSize, typename _InitParamType>
			inline _Type* MakeType(char (&uBuffer)[uSize], const _InitParamType& Other)
			{
				_KG3D_DEBUG_STATIC_CHECK_T(uSize >= sizeof(_Type), SizeTooSmall);
				return new(uBuffer) _Type(Other);
			};

			/*//Placement Delete?究竟要不要？
			template<size_t uSize>
			inline void DelType(char (&uBuffer)[uSize])
			{
			_KG3D_DEBUG_STATIC_CHECK_T(uSize >= sizeof(_Type), SizeTooSmall);
			_Type* pType = (Type*)uBuffer;
			pType->~_Type();
			};*/

		};

		template<unsigned _Size> struct TSize;
	};
	/*
	C++很大的一个问题是所有成员都定义在头文件里面，连私有成员都在，目的只是能计算出大小而已。
	会造成耦合。而在一个头文件中把成员声明为别的东东就必须把别的东东的头文件包含在用户文件的
	头文件里面，于是这个耦合性会一直传递出去。用指针+构造函数New会解决一部分问题，但是每加一个
	成员都要分别在头文件，析构函数，构造函数，拷贝构造函数，复制函数分别操作一次，简直......
	一不小心就......现在尝试用模板，预先填一个大小来解决这个问题，因为是PlacementNew，本身是
	绝对不会内存泄漏的。这个模板只适合用于简单类，因为默认就构造了一次。以后可能会针对一些复杂类
	写另外一种类似的东西。

	使用方式是
	1.在头文件中声明
	class ClassA;
	KG_CUSTOM_HELPER::TCommonParamBuffer<ClassA, XX（字节为单位的类大小）>	m_pXXX;
	头文件中不需要包含类ClassA的头文件，这个是最大的好处，因为模板只有在Instanize的时候才需要定义，
	所以只需要在头文件中估计一个大小，让C++预先分配一个空间就好了。

	这个大小是需要自己预估的，不能直接写sizeof，不然就需要在头文件中包含ClassA的头文件了。当然，你
	如果估计不出来的话，可以用调试的方式在某些地方得到这个大小。

	如果这个大小，在模板参数中填错了，那么SizeError_PleaseGetTheCorrectSizeByTheTemplateParam
	的静态检测宏会在编译的时候就报错。留意其模板参数，可以直接得到正确的大小
	如写了KG_CUSTOM_HELPER::TCommonParamBuffer<ClassA, 12>	m_pXXX;
	报C2079 SizeError_PleaseGetTheCorrectSizeByTheTemplateParam uses undefined struct 'KG_CUSTOM_HELPERS::Private::TSize<_Size>'
	with
	[
	_Size=432
	]
	就表示模板参数中的声明的空间太小。432就是正确的大小
	另外声明太大也会报错，同样用上面的方法检查正确的大小

	其实也不是必须要求填大小的，如果这个东西当成SmartPtr的话，可以考虑用new的方式来生成类实例。但是
	这样就需要多浪费一点内存了，而且这和通常的类成员预先分配空间的方式也不一致，如果用在容器中的话
	平白无故多了复杂度。

	2.在cpp中包含那个_Type的定义。
	因为在cpp中才会对模板实例化，所以类定义在cpp中才需要，这样就免除了头文件耦合了。
	这样就可以在类里面直接用m_pXXX->foo();之类的方法了，当指针吧。

	特别注意：使用这种智能指针的类的构造，析构，复制构造函数等必须是在cpp中定义的，因为编译器会在编译这些函数的
	时候需要子类的析构和构造定义。另外默认的析构，构造函数是动态生成的，也不行
	如在AClass.h中这么声明
	class BClass
	class AClass
	{
	KG_CUSTOM_HELPERS::TCommonParamBuffer<BClass, 4>	m_pBClass;
	};
	那么在C.cpp中如果这么写 AClass a;的话，编译不过，因为这时候，编译器在这里才需要动态生成AClass的默认构造函数
	和析构函数，这时候找不到BClass的定义。当然，在C.cpp中包含BClass.h也是可行的，不过这样就违反了用这个智能指针
	来阻止类定义扩散的用意了。

	一般要这样
	class BClass
	class AClass
	{
	KG_CUSTOM_HELPERS::TCommonParamBuffer<BClass, 4>	m_pBClass;
	public:
	AClass();
	~AClass();
	};
	然后在cpp中加入#include "BClass.h"
	当然，如果C.cpp要直接用这个智能指针的话也是要#include "BClass.h"，反正和这样子是一个道理
	class BClass
	class AClass
	{
	BClass*	m_pBClass;
	public:
	AClass();
	~AClass();
	};

	要说这个智能指针比上面的这种方式有啥不同
	就是不需要AClass中到处new，不需要检查分配失败的情况，没有内存泄漏

	按照C++设计新思维里面反思的：非常tricky —— c++对于用c很习惯的人也很tricky，所以能用就用吧。其实原理说清楚
	了也就那回事而已。


	3.如果需要特殊的初始化，需要覆盖后面的_Newer模板参数

	4.当智能指针那样使用，已经重载了->操作符了，用if(! p)等语法也是可以的

	5.需要提取内部的指针的话，可以这样写：
	ClassA* pXX = KGCommonBufferGetPtr(m_pXXX);	
	注意不需要在KGCommonBufferGetPtr前面写namespace,写了反而不行。这是利用C++标准中的参数名字查找策略的奇怪手法。
	SmartPtr的同类函数也是这样实现的。

	6.delete它的话绝对出错，因为其本来的目的是做“不用在头文件中包含HAS-A的类的头文件的成员”，你本来也不会
	delete一个类成员（你只会delete成员的指针）。如果非要删除，那么用KGCommonBufferRecreate(m_pXX);
	这样的话会在Buffer中清除原来的实例，并重新在原位置创建出一个空白的实例。

	*/
	///目的是为了解决头文件耦合，这个东东是不需要_Type的定义的，目的是像成员一样用
	template<typename _Type, unsigned _uBufferSize, class _Newer = Private::TTypePlacementNewer<_Type> >
	class TCommonParamBuffer
	{
	private:
		enum{buffer_size = _uBufferSize, max_wastable_size = (buffer_size / 10) > 12 ? 12 : (buffer_size / 10)};
		char m_Buffer[buffer_size];
		_Type* m_pType;
	private:
		inline void Clear()
		{
			if(!m_pType)
			{
				ZeroMemory(m_Buffer, sizeof(m_Buffer));
				return;
			}	
			m_pType->~_Type();
			ZeroMemory(m_Buffer, sizeof(m_Buffer));
			m_pType = NULL;
		}
		inline _Type* Recreate()//删除原来的类，重新创建
		{
			Clear();
			m_pType = _Newer().MakeType(m_Buffer);
			_ASSERTE((void*)m_pType == (void*)m_Buffer);
			return m_pType;
		}

		inline void CheckSize()
		{
			/*
			如果这里出错，显示

			1>D:\Kingsoft\Game\DataStores\Sword3\Source\KG3DEngine\KG3DEngine\Helpers\KGCommonBuffer.h(194) : error C2079: 'SizeError_PleaseGetTheCorrectSizeByTheTemplateParam' uses undefined struct 'KG_CUSTOM_HELPERS::Private::TSize<_Size>'
			1>        with
			1>        [
			1>            _Size=812
			1>        ]

			这样的错误，那么把TCommonParamBuffer的大小改成 _Size的值就好了。如上面这个Bug，改成812就可以了
			*/
#if defined(_DEBUG) | defined(DEBUG)//检查大小
			/*
			下面这个模板TypeSeletor这样作用：
			它有3个参数：bool,Type1,Type2
			如果作为bool参数的条件满足，那么Result是NullType，那么下面声明一个临时的NullType不会出什么事
			如果不满足，那么Result是Private::TSize，这个类根本没有声明，于是下面声明的临时变量不能通过
			编译，系统会把这个变量名显示出来，而且其参数正好就是TCommonParamBuffer要缓冲的那个类的大小
			*/
			typedef typename TTypeSelector<sizeof(_Type) <= buffer_size, NullType, Private::TSize<sizeof(_Type)> >::Result TypeSizeChecker;//如果这个类构造失败就会报错
			TypeSizeChecker SizeError_PleaseGetTheCorrectSizeByTheTemplateParam;
			(void)SizeError_PleaseGetTheCorrectSizeByTheTemplateParam;
			//下面这个Check用于避免用了太大的Buffer，浪费空间
			typedef typename TTypeSelector<buffer_size - sizeof(_Type) <= max_wastable_size, NullType, Private::TSize<sizeof(_Type)> >::Result TypeSizeChecker2;
			TypeSizeChecker2 SizeError_WastingBufferSize_SeeTheTemplateParam;
			(void)SizeError_WastingBufferSize_SeeTheTemplateParam;
#endif
		}
	public:
		TCommonParamBuffer()
			:m_pType(NULL)
		{
			CheckSize();			
			Clear();
			_ASSERTE(!m_pType);
			m_pType = _Newer().MakeType(m_Buffer);
			_ASSERTE((void*)m_pType == (void*)m_Buffer);
		}
		explicit TCommonParamBuffer(const TCommonParamBuffer& Other)
			:m_pType(NULL)
		{
			CheckSize();
			_ASSERTE(this != &Other);
			_ASSERTE(Other.m_pType);
			Clear();
			_ASSERTE(!m_pType);
			m_pType = _Newer().MakeType(m_Buffer, *Other.m_pType);
			_ASSERTE((void*)m_pType == (void*)m_Buffer);	
		}
		template<typename _InitParamType>
		explicit TCommonParamBuffer(const _InitParamType& Other)
			:m_pType(NULL)
		{
			CheckSize();			
			Clear();
			_ASSERTE(!m_pType);
			m_pType = _Newer().MakeType(m_Buffer, Other);
			_ASSERTE((void*)m_pType == (void*)m_Buffer);
		}
		TCommonParamBuffer& operator=(const TCommonParamBuffer& Other)
		{
			_ASSERTE(this != &Other);
			_ASSERTE(Other.m_pType);
			_Newer().CopyType(m_Buffer, *Other.m_pType);
			return *this;
		}

		~TCommonParamBuffer()
		{
			Clear();
		}

		inline _Type* operator->(){_ASSERTE(m_pType); return m_pType;}
		inline const _Type* operator->()const	{_ASSERTE(m_pType); return m_pType;}
		inline _Type& operator*(){_ASSERTE(m_pType); return *m_pType;}///内部一直有一个对像，所以不用担心出去的是无用的
		inline const _Type& operator*()const{_ASSERTE(m_pType); return *m_pType;}

		//<比较>
	private:
		// 用于'if (sp)'形式的比较。除了这种自动转型之外，其他都是不安全的，
		// 指针一旦有了自动转型就没有所有智能指针的保护了
		struct DummyClass
		{
			DummyClass() {}
		private:
			void operator delete(void*);
		};

	public:
		operator DummyClass*() const
		{
			if (! m_pType) return 0;
			static DummyClass t;
			return &t;
		}
		inline bool operator!()const
		{
			return m_pType == NULL;
		}
		inline friend bool operator==(const TCommonParamBuffer& pTP,
			const _Type pObj)
		{ return pTP.m_pType == pObj; }

		inline friend bool operator==(const _Type pObj,
			const TCommonParamBuffer& pTP)
		{ return pObj == pTP.m_pType; }

		inline friend bool operator!=(const TCommonParamBuffer& pTP,
			const _Type pObj)
		{ return !(pTP.m_pType == pObj); }

		inline friend bool operator!=(const _Type pObj,
			const TCommonParamBuffer& pTP)
		{ return !(pObj == pTP.m_pType); }
		//</比较>

		/************************************************************************/
		/*注意使用的时候不要加namespace限定，不然会触发C3767。这是一种诡异的
		手法——friend后面直接加定义。但SmartPtr的实现是需要这种手法的。
		使用的时候这样就好了
		KGCommonBufferGetPtr(xx);
		编译器会根据C++标准中的参数名字查找策略找到这个函数的
		*/
		/************************************************************************/
		friend inline _Type* KGCommonBufferGetPtr(TCommonParamBuffer& Buffer){return Buffer.m_pType;}
		friend inline const _Type* KGCommonBufferGetPtr(const TCommonParamBuffer& Buffer){return Buffer.m_pType;}
		friend inline _Type* KGCommonBufferRecreate(TCommonParamBuffer& Buffer){return Buffer.Recreate();}
	};
};
namespace KGCH = KG_CUSTOM_HELPERS;

#endif //_INCLUDE_KGCOMMONBUFFER_H_
