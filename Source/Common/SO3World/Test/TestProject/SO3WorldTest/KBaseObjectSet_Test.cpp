#include "StdAfx.h"
#include ".\kbaseobjectset_test.h"

#ifndef TEST_DEBUG
CPPUNIT_TEST_SUITE_REGISTRATION(KBaseObjectSet_Test);
#endif

void KBaseObjectSet_Test::setUp()
{
}
void KBaseObjectSet_Test::tearDown()
{
}
void KBaseObjectSet_Test::testInit(void)
{
	int nRetCode     = FALSE;
	DWORD dwRet      = 0;
	//DWORD dwSize[]   = {0, 1, 0x0FFFFFFF, 0x10000000, 0x10000001, UINT_MAX};//前3个是正确数据，后3个是错误数据
	DWORD dwSize[]   = {0, 1, 100000};
	DWORD dwPrefix[] = {0, 1, UINT_MAX};
	KBaseObjectSet<testStruct> myObjectSet;

	for(int nIndex = 0; nIndex < 3; nIndex++)
	{
		for (int nNum = 0; nNum < 3; nNum++)
		{
			nRetCode = myObjectSet.Init(dwSize[nIndex], dwPrefix[nNum]);

			if(nIndex > 2)
			{
				CPPUNIT_ASSERT(nRetCode == FALSE);
				CPPUNIT_ASSERT(myObjectSet.m_dwSize == 0);
				CPPUNIT_ASSERT(myObjectSet.m_mapId2Obj.size() == 0);
				CPPUNIT_ASSERT(myObjectSet.m_FreeList.size() == 0);
				CPPUNIT_ASSERT(myObjectSet.m_dwIdGenerator == ERROR_ID);
				CPPUNIT_ASSERT(myObjectSet.m_ObjectSet == NULL);
			}
			else
			{
				CPPUNIT_ASSERT(nRetCode == TRUE);
				CPPUNIT_ASSERT(myObjectSet.m_dwSize == dwSize[nIndex]);
				CPPUNIT_ASSERT(myObjectSet.m_ObjectSet != NULL);
				CPPUNIT_ASSERT(myObjectSet.m_mapId2Obj.size() == 0);
				dwRet = (DWORD)(dwPrefix[nNum] << (sizeof(myObjectSet.m_dwIdGenerator) * 8 - PREFIX_BIT_NUM)) + 1;
				CPPUNIT_ASSERT(myObjectSet.m_dwIdGenerator == dwRet);
				CPPUNIT_ASSERT(myObjectSet.m_FreeList.size() == dwSize[nIndex]);
			}
		}
	}
}

void KBaseObjectSet_Test::testNew(void)
{
	DWORD dwID[]   = {0, 1, 0x0FFFFFFF, 0x10000000, 0x10000001, UINT_MAX};
	DWORD dwSize   = 100;
	DWORD dwPrefix = 6;
	DWORD dwRet    = 0;
	KBaseObjectSet<testStruct> myObjectSet;

	myObjectSet.Init(dwSize, dwPrefix);
	
	int nListSize = myObjectSet.m_FreeList.size();
	int nMapId2ObjSize = myObjectSet.m_mapId2Obj.size();
	int nGeneratorId = myObjectSet.m_dwIdGenerator;

	for(int nLoop = 0; nLoop < 6; nLoop++)
	{
		if(nLoop < 3)
		{
            --nListSize;
			++nMapId2ObjSize;

			CPPUNIT_ASSERT(myObjectSet.New(dwID[nLoop]));
			CPPUNIT_ASSERT(nListSize ==  myObjectSet.m_FreeList.size());
			CPPUNIT_ASSERT(nMapId2ObjSize ==  myObjectSet.m_mapId2Obj.size());
			if(!nLoop)
			{
				++nGeneratorId;
				CPPUNIT_ASSERT(nGeneratorId == myObjectSet.m_dwIdGenerator);

				dwRet = myObjectSet.m_mapId2Obj.begin()->second->m_dwID;
				CPPUNIT_ASSERT(dwRet == nGeneratorId);
			}
			else
			{
				dwRet = myObjectSet.m_mapId2Obj[dwID[nLoop]]->m_dwID;
				CPPUNIT_ASSERT(dwRet == dwID[nLoop]);
			}
		}
		else
		{
			//如果传入的dwId大于ID的最大值 ？程序代码没有判断！
		}
	}
}
void KBaseObjectSet_Test::testDelete(void)
{
	int nRetCode   = FALSE;
	DWORD dwID[]   = {1, UINT_MAX};
	DWORD dwSize   = 100;
	DWORD dwPrefix = 6;
	KBaseObjectSet<testStruct> myObjectSet;
	testStruct StructObj[] = {{1, "Well", 26}, {UINT_MAX, "soWell", 28}};

	myObjectSet.Init(dwSize, dwPrefix);
	myObjectSet.New(dwID[0]);
	myObjectSet.New(dwID[1]);

	int nMapSize = myObjectSet.m_mapId2Obj.size();
	int nListSize = myObjectSet.m_FreeList.size();

	for(int nIndex = 0; nIndex < 2; nIndex++)
	{
		--nMapSize;
		++nListSize;
		nRetCode = myObjectSet.Delete(&StructObj[nIndex]);
		CPPUNIT_ASSERT(nRetCode == TRUE);
		CPPUNIT_ASSERT(myObjectSet.m_mapId2Obj.size() == nMapSize);
		CPPUNIT_ASSERT(myObjectSet.m_FreeList.size() == nListSize);
		CPPUNIT_ASSERT(StructObj[nIndex].m_dwID == ERROR_ID);
	}	
}
void KBaseObjectSet_Test::testGetObj(void)
{
	int nRetCode   = FALSE;
	DWORD dwID[]   = {1, UINT_MAX};
	DWORD dwSize   = 100;
	DWORD dwPrefix = 6;
	KBaseObjectSet<testStruct> myObjectSet;
	testStruct StructObj[] = {{1, "Well", 26}, {UINT_MAX, "soWell", 28}};

	myObjectSet.Init(dwSize, dwPrefix);
	myObjectSet.New(dwID[0]);
	myObjectSet.New(dwID[1]);

	int nMapSize = myObjectSet.m_mapId2Obj.size();
	int nListSize = myObjectSet.m_FreeList.size();

	for(int nIndex = 0; nIndex < 2; nIndex++)
	{
		--nMapSize;
		++nListSize;
		nRetCode = myObjectSet.Delete(&StructObj[nIndex]);
		CPPUNIT_ASSERT(nRetCode == TRUE);
		CPPUNIT_ASSERT(myObjectSet.m_mapId2Obj.size() == nMapSize);
		CPPUNIT_ASSERT(myObjectSet.m_FreeList.size() == nListSize);
		CPPUNIT_ASSERT(StructObj[nIndex].m_dwID == ERROR_ID);
	}	
}

void KBaseObjectSet_Test::testGetSize(void)
{
	DWORD dwCode   = 0;
	//DWORD dwSize[] = {0, 1, 0x0FFFFFFF, 0x10000000, 0x10000001, UINT_MAX};
	DWORD dwPrefix = 6;
	DWORD dwSize[] = {0, 1,1000000};
	KBaseObjectSet<testStruct> myObjectSet;

	for(int nIndex = 0; nIndex < 3; nIndex++)
	{
		if(nIndex < 3)
		{
			myObjectSet.Init(dwSize[nIndex], dwPrefix);
			dwCode = myObjectSet.GetSize();
			CPPUNIT_ASSERT(dwCode == dwSize[nIndex]);
		}
		else
		{
			myObjectSet.Init(dwSize[nIndex], dwPrefix);
			dwCode = myObjectSet.GetSize();
			CPPUNIT_ASSERT(dwCode == 0);
		}
	}
}
void KBaseObjectSet_Test::testGetUsedSize()
{
	DWORD dwCode   = 0;
	DWORD dwID[]   = {0, 1, 0x0FFFFFFF, 0x10000000, 0x10000001, UINT_MAX};
	DWORD dwSize   = 100;
	DWORD dwPrefix = 6;
	DWORD dwCount  = 0;
	KBaseObjectSet<testStruct> myObjectSet;

	myObjectSet.Init(dwSize, dwPrefix);
	
	for(int nIndex = 0; nIndex < 6; nIndex++)
	{
		++dwCount;
		myObjectSet.New(dwID[nIndex]);
		dwCode = myObjectSet.GetUsedSize();
		CPPUNIT_ASSERT(dwCode == dwCount);
	}
}

void KBaseObjectSet_Test::testTraverse(void)
{
}

void KBaseObjectSet_Test::testOperator()
{
	DWORD dwCode    = 0;
	DWORD dwIndex[] = {0, 1, 0x0FFFFFFF, 0x10000000, 0x10000001, UINT_MAX};
	DWORD dwSize    = 100;
	DWORD dwPrefix  = 6;
	DWORD dwCount   = 0;
	KBaseObjectSet<testStruct> myObjectSet;
	testStruct* pStructObj = NULL;

	for(int nLoop = 0; nLoop < 6; nLoop++)
	{
		pStructObj = &myObjectSet[dwIndex[nLoop]];
		//if(nLoop < 2)
			//CPPUNIT_ASSERT_EQUAL(pStructObj, myObjectSet.m_ObjectSet[dwIndex[nLoop]]);
		//else
			//CPPUNIT_ASSERT_EQUAL(pStructObj, myObjectSet.m_ObjectSet[myObjectSet.m_dwSize]);
	}
	
}