/***************************************************
Filename:wndscrollbar_Test.h
CreatedBy: Yan Chunwa
CreatedOn: 2005Äê8ÔÂ30ÈÕ 10:13:17.6947495
Test Requirement Reference by Wiki path:
http://sword-test/qcweb/pmwiki.php?n=XSJQC-DEV.JX3-QC-REQ-SO3Client_UI_Scrollbar
Comment:

**************************************************/


#ifndef WNDSCROLLBAR_TEST_H
#define WNDSCROLLBAR_TEST_H

#ifdef _UNIT_TEST

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
//#include "../ui/elem/wndbutton.h"
//#include "../ui/elem/wndscrollbar.h"
//#include "KSUnitTest/KingsoftUnitTest.H"
//#include "KSUnitTest/ClassDataCopyBuffer.H"

const int MinPos = 0;
const int MaxPos = 200;


class KWndScrollBar_Test :
	public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(KWndScrollBar_Test);
	CPPUNIT_TEST(SetScrollPostest);
	CPPUNIT_TEST(ScrollLinetest);
	CPPUNIT_TEST(ScrollPagetest);
	CPPUNIT_TEST(SetValueRangetest);
	CPPUNIT_TEST(SetPageSizetest);
	CPPUNIT_TEST(SetTotalSizetest);
	CPPUNIT_TEST(SetSizetest);
	CPPUNIT_TEST_SUITE_END();

public:
	KWndScrollBar_Test(){}
	~KWndScrollBar_Test(){}

public:
	void setUp();
	void tearDown();

	void SetScrollPostest();
	void ScrollLinetest();
	void ScrollPagetest();
	void SetValueRangetest();
	void SetPageSizetest();
	void SetTotalSizetest();
	void SetSizetest();

//private:
//	UI::KWndButton* btnNext;
//	UI::KWndButton* btnPage;
//	UI::KWndButton* btnSlide;
//	UI::KWndScrollBar* scrollbar;

};

#endif // #ifdef _UNIT_TEST

#endif //WNDSCROLLBAR_TEST_H