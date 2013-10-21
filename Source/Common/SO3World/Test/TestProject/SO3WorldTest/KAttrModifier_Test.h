#pragma once
#ifndef _CTEST_KATTRMODIFIER_H_
#define _CTEST_KATTRMODIFIER_H_

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "KAttrModifier.h"

class KAttrModifier_Test : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(KAttrModifier_Test);
	CPPUNIT_TEST(testInit);
	CPPUNIT_TEST(testProcessStrength);
	CPPUNIT_TEST(testProcessStrengthPercent);
	CPPUNIT_TEST(testProcessMaxLifePercent);
	CPPUNIT_TEST(testProcessCurrentLife);
	
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();           
	void tearDown();
protected:
	void testInit();
	void testProcessStrength();
	void testProcessStrengthPercent();
	void testProcessMaxLifePercent();
	void testProcessCurrentLife();
};

#endif
