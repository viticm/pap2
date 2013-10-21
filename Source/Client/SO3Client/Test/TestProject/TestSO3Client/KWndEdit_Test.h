/***************************************************
Filename:KWndEdit_Test.h
CreatedBy: McWell
CreatedOn: 2005Äê7ÔÂ21ÈÕ 10:26:18.9245384
Test Requirement Reference by Wiki path:

Comment:Test all public functions of KHotkeyTable class by cppunit framework

**************************************************/
#ifndef _CTEST_UI_KWNDEDIT_H
#define _CTEST_UI_KWNDEDIT_H

#ifdef _UNIT_TEST

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "KSUnitTest/ClassDataCopyBuffer.H"

class KWndEdit_Test : public CppUnit::TestFixture, KingsoftUnitTest::ClassDataCopyBuffer
{
	CPPUNIT_TEST_SUITE(KWndEdit_Test);

	CPPUNIT_TEST(testClone);
	CPPUNIT_TEST(testSetText);
	CPPUNIT_TEST(testSetIntText);
	CPPUNIT_TEST(testEnable);
	CPPUNIT_TEST(testClearText);
	CPPUNIT_TEST(testInsertString);
	CPPUNIT_TEST(testSetFocusBkColor);
	CPPUNIT_TEST(testSetLimitLen);
	CPPUNIT_TEST(testSetFlag);
	CPPUNIT_TEST(testSetType);
	CPPUNIT_TEST(testGetTextLength);
	CPPUNIT_TEST(testGetText);
	CPPUNIT_TEST(testGetIntNumber);
	CPPUNIT_TEST(testGetCaretPosition);
	CPPUNIT_TEST(testGetFocusBgColor);
	CPPUNIT_TEST(testGetFlag);
	CPPUNIT_TEST(testGetFontSize);
	CPPUNIT_TEST(testGetFontColor);
	CPPUNIT_TEST(testGetFontBorderColor);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();           
	void tearDown();

protected:
	void testClone();
	void testSetText();
	void testSetIntText();
	void testEnable();
	void testClearText();
	void testInsertString();
	void testSetFocusBkColor();
	void testSetLimitLen();
	void testSetFlag();
	void testSetType();
	void testGetTextLength();
	void testGetText();
	void testGetIntNumber();
	void testGetCaretPosition();
	void testGetFocusBgColor();
	void testGetFlag();
	void testGetFontSize();
	void testGetFontColor();
	void testGetFontBorderColor();
};
#endif // #ifdef _UNIT_TEST

#endif