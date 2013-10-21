////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGHelperClasses.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-2 10:28:22
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "KGHelperClasses.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace KG_CUSTOM_HELPERS
{

#if defined(KG_ENABLE_TDD)
	KG_TEST_BEGIN(KGHelperClasses)
	{
		//²âÊÔForAll
		{
			struct AClass
			{
				INT m_A;
				void DoSomething(){m_A = 10000;}
			};
			AClass StandardInstance;
			StandardInstance.m_A = 1;

			{
				std::vector<AClass> Vec(2, StandardInstance);
				KG_TDD_CHECK(Vec[0].m_A == 1);
				ForAll(Vec, std::mem_fun_ref(&AClass::DoSomething));
				KG_TDD_CHECK(Vec[1].m_A == 10000);
			}

			{
				std::map<INT, AClass> AMap;
				AMap[0] = StandardInstance;
				AMap[1] = StandardInstance;
				KG_TDD_CHECK(AMap[0].m_A == 1);
				ForAll(AMap, std::mem_fun_ref(&AClass::DoSomething));
				KG_TDD_CHECK(AMap[1].m_A == 10000);
			}		

			{
				AClass aclassArray[10];
				ForAll(aclassArray, std::mem_fun_ref(&AClass::DoSomething));
				KG_TDD_CHECK(aclassArray[0].m_A == 10000);
			}


		}
		//²âÊÔSafeRelease
		{
			static size_t s_uReleased = 0;
			struct AClass 
			{
				static void Release(){++s_uReleased;}	//Ð´³öÐéÄâµÄµ÷ÓÃ
			};
			AClass tempClass;
            AClass* aclassArray[10];
			for (size_t i = 0; i < _countof(aclassArray); ++i)
			{
				aclassArray[i] = &tempClass;
			}
           
			TContainerRelease(aclassArray);
			KG_TDD_CHECK(s_uReleased == _countof(aclassArray));
		}
		//²âÊÔIteratorHolder
		{
			{
				typedef std::map<INT, INT> TypeCt;
				TypeCt ctTemp;
				ctTemp[0] = 0;
				ctTemp[1] = 1;
				IteratorHolder holder = CreateIteratorHolder(ctTemp);
				TypeCt::iterator it = *reinterpret_cast<TypeCt::iterator*>(&holder);
				_ASSERTE(0 == it->second);
				++it;
				_ASSERTE(1 == it->second);
			}
			{
				typedef std::set<INT> TypeCt;
				TypeCt ctTemp;
				ctTemp.insert(0);
				ctTemp.insert(1);
				IteratorHolder holder = CreateIteratorHolder(ctTemp);
				TypeCt::iterator it = *reinterpret_cast<TypeCt::iterator*>(&holder);
				_ASSERTE(0 == *it);
				++it;
				_ASSERTE(1 == *it);
			}
			{
				typedef std::list<INT> TypeCt;
				TypeCt ctTemp;
				ctTemp.push_back(0);
				ctTemp.push_back(1);
				IteratorHolder holder = CreateIteratorHolder(ctTemp);
				TypeCt::iterator it = *reinterpret_cast<TypeCt::iterator*>(&holder);
				_ASSERTE(0 == *it);
				++it;
				_ASSERTE(1 == *it);
			}
			{
				typedef std::vector<INT> TypeCt;
				TypeCt ctTemp;
				ctTemp.push_back(0);
				ctTemp.push_back(1);
				IteratorHolder holder = CreateIteratorHolder(ctTemp);
				TypeCt::iterator it = *reinterpret_cast<TypeCt::iterator*>(&holder);
				_ASSERTE(0 == *it);
				++it;
				_ASSERTE(1 == *it);
			}
		}
	}
	KG_TEST_END(KGHelperClasses)
#endif
};



////////////////////////////////////////////////////////////////////////////////

