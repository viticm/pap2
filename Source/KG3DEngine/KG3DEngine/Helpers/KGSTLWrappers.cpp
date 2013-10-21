////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSTLWrappers.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-28 14:10:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KGSTLWrappers.h"


////////////////////////////////////////////////////////////////////////////////
LPVOID KG_CUSTOM_HELPERS::FrontAccessProxy::GetBackElemPointer()
{
	_ASSERTE(! this->IsEmpty() && _T("使用GetBack之前需要检查非空才行，空容器没法取Back原素")); 
	this->GoToEnd();
	this->StepBackward();//如果容器是单向迭代器，这里可能出错，可以用GetGepMask来查能力标志
	return Cur();
}

#if defined(KG_ENABLE_TDD)
namespace KGCH = KG_CUSTOM_HELPERS;
template<class Type>
struct ProxyTester : public KGCH::TFrontAccessProxySolid<Type>
{
	typedef KGCH::TFrontAccessProxySolid<Type> TypeBase;

	explicit ProxyTester(Type& Ct):TypeBase(Ct){}

	typename TypeBase::iterator& GetIt(){return m_it;}
};
KG_TEST_BEGIN(KGSTLWrappers)
{
	using namespace KGCH;
	//Test Vector
	{
		std::vector<int> testVec;
		ProxyTester<std::vector<int> > tester(testVec);

		enum{em_vec_count = 10,};
		for (int i = 0; i < em_vec_count; ++i)
		{
			testVec.push_back(i);
		}

		_ASSERTE(tester.IsContentThisType(typeid(int)));

		std::vector<int>::iterator it = testVec.begin();
		tester.Begin();
		for (size_t i = 0
			; i < em_vec_count
			; ++i, tester.StepForward(), ++it)
		{
			_ASSERTE(! tester.IsEnd());
			_ASSERTE(it == tester.GetIt());
			_ASSERTE(tester.Cur() == (LPVOID)&(testVec[i]));
		}
	}

	//Test Array
	{
		INT intArray[] = {0,1,2,3,4};
		TFrontAccessProxyArray<INT> tempProxy(intArray);

		size_t i = 0;
		for (tempProxy.Begin(); ! tempProxy.IsEnd(); tempProxy.StepForward(), ++i)
		{
			INT& curInt = tempProxy.GetCur();
			_ASSERTE(curInt == intArray[i]);
			
			curInt = 10;
		}

		for (i = 0; i < _countof(intArray); ++i)
		{
			_ASSERTE(10 == intArray[i]);
		}


	}

	//GetCapMask
	{
		{
			std::vector<int> testVec;
			ProxyTester<std::vector<int> > tester(testVec);
			ULONG cap = tester.GetCapMask();
			_ASSERTE((cap & em_frontAccessProxy_cap_forward) && (cap & em_frontAccessProxy_cap_bidirection));
		}

		typedef std::vector<int> TypeVec;
		typedef std::vector<int>::iterator TypeVecIt;
		{
			struct CustomVector : public TypeVec
			{
				struct CustomIterator : public  TypeVecIt
				{
					CustomIterator(){}
					CustomIterator(const TypeVecIt& it):TypeVecIt(it){}
					typedef forward_iterator_tag iterator_category;
				};

				typedef CustomIterator iterator;
			};
			CustomVector testVec;
			ProxyTester<CustomVector > tester(testVec);
			ULONG cap = tester.GetCapMask();

			_ASSERTE((cap & em_frontAccessProxy_cap_forward) && !(cap & em_frontAccessProxy_cap_bidirection));
		}

		{
			struct CustomVector : public TypeVec
			{
				struct CustomIterator : public  TypeVecIt
				{
					CustomIterator(){}
					CustomIterator(const TypeVecIt& it):TypeVecIt(it){}
					typedef input_iterator_tag iterator_category;
				};

				typedef CustomIterator iterator;
			};
			CustomVector testVec;
			ProxyTester<CustomVector > tester(testVec);
			ULONG cap = tester.GetCapMask();

			_ASSERTE(!(cap & em_frontAccessProxy_cap_forward) && !(cap & em_frontAccessProxy_cap_bidirection));
		}
	}

}
KG_TEST_END(KGSTLWrappers)

#endif

