#ifndef _CTEST_UI_KWNDLABLEDBUTTON_H
#define _CTEST_UI_KWNDLABLEDBUTTON_H

#ifdef _UNIT_TEST

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "KSUnitTest/ClassDataCopyBuffer.H"

class KWndLabeledButton_Test : public CppUnit::TestFixture, KingsoftUnitTest::ClassDataCopyBuffer
{
	CPPUNIT_TEST_SUITE(KWndLabeledButton_Test);

	CPPUNIT_TEST(testClone);

	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();           
	void tearDown();

protected:
	void testClone();
};
#endif // #ifdef _UNIT_TEST

#endif