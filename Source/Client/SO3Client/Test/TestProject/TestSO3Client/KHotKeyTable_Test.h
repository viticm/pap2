/***************************************************
Filename:testKHotKeyTable.h
CreatedBy: McWell
CreatedOn: 2005Äê7ÔÂ21ÈÕ 10:26:18.9245384
Test Requirement Reference by Wiki path:

Comment:Test all public functions of KHotkeyTable class by cppunit framework

**************************************************/
#ifndef _CTEST_UI_KHOTKEYTALBE_H
#define _CTEST_UI_KHOTKEYTALBE_H

#ifdef _UNIT_TEST

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "../ui/driver/khotkeytable.h"
#include "KSUnitTest/ClassDataCopyBuffer.H"
#include <string>

using namespace KingsoftUnitTest;

class KHotKeyTable_Test : public CppUnit::TestFixture, public ClassDataCopyBuffer
{
	CPPUNIT_TEST_SUITE(KHotKeyTable_Test);

	CPPUNIT_TEST(testTranslateTextToHotkey);
	CPPUNIT_TEST(testTranslateHotkeyToText);

	CPPUNIT_TEST_SUITE_END();
public:
	void setUp();           
	void tearDown();

protected:
	void testTranslateTextToHotkey();
	void testTranslateHotkeyToText();

private:
	UI::KHotkeyTable m_KHotKeyTable;
};
#endif // #ifdef _UNIT_TEST
#endif