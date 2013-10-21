#pragma once
#ifndef _CTEST_KBASEOBJECTSET_TEST_H_
#define _CTEST_KBASEOBJECTSET_TEST_H_

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "KBaseObjectSet.h"

struct testStruct
{
	DWORD m_dwID;
	char  m_szName[32];
	int   m_nAge;
}myStruct;
class KBaseObjectSet_Test : public CppUnit::TestFixture
{
#ifndef TEST_DEBUG
	CPPUNIT_TEST_SUITE(KBaseObjectSet_Test);
	CPPUNIT_TEST(testInit);
	CPPUNIT_TEST(testNew);
	CPPUNIT_TEST(testDelete);
	CPPUNIT_TEST(testGetObj);
	CPPUNIT_TEST(testGetSize);
	CPPUNIT_TEST(testGetUsedSize);
	CPPUNIT_TEST(testTraverse);
	CPPUNIT_TEST(testOperator);
	CPPUNIT_TEST_SUITE_END();
#endif

public:
	void setUp();           
	void tearDown();
protected:
	void testInit();
	void testNew();
	void testDelete();
	void testGetObj();
	void testGetSize();
	void testGetUsedSize();
	void testOperator();
	void testTraverse();
//private:
	//KBaseObjectSet<testStruct> myObjectSet;
};
#endif