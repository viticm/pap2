#ifndef _CTEST_UI_KWNDLIST2_H
#define _CTEST_UI_KWNDLIST2_H

#ifdef _UNIT_TEST

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "KSUnitTest/ClassDataCopyBuffer.H"

class KWndList2_Test : public CppUnit::TestFixture, KingsoftUnitTest::ClassDataCopyBuffer
{
	CPPUNIT_TEST_SUITE(KWndList2_Test);

	CPPUNIT_TEST(test);

	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();           
	void tearDown();

protected:
	void test();

};
#endif // #ifdef _UNIT_TEST

#endif