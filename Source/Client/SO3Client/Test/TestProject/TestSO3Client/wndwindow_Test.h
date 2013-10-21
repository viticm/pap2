/***************************************************
Filename:Test_UI_WndButton.cpp
CreatedBy: zhaoyunhua
CreatedOn: 2005Äê7ÔÂ29ÈÕ 10:21:29.6486898
Test Requirement Reference by Wiki path:
http://192.168.22.107/pmwiki/pmwiki.php?n=J3Test.J3-QC-REQ-SO3Client_UI_button
Comment:

**************************************************/
#ifndef _WNDWINDOW_TEST_H
#define _WNDWINDOW_TEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"

#include "../ui/driver/igraphic.h"
#include "KSUnitTest/KingsoftUnitTest.H"
#include "KSUnitTest/ClassDataCopyBuffer.H"

using namespace UI;
using namespace KingsoftUnitTest;




class KWndWindow_Test : public CppUnit::TestFixture , public IClassUnitTest
{
	CPPUNIT_TEST_SUITE(KWndWindow_Test);
	CPPUNIT_TEST(AddBrother_Test);
	CPPUNIT_TEST(AddChild_Test);
	CPPUNIT_TEST(BringToTop_Test);
	CPPUNIT_TEST(SetPosition_Test);
	CPPUNIT_TEST(GetPosition_Test);
	CPPUNIT_TEST(AbsoluteMove_Test);
	CPPUNIT_TEST(SetSize_Test);
	CPPUNIT_TEST(GetSize_Test);
	CPPUNIT_TEST(GetSize_Test);
	CPPUNIT_TEST(GetAbsolutePos_Test);
	CPPUNIT_TEST(PtInWindow_Test);
	CPPUNIT_TEST(TopChildFromPoint_Test);
	CPPUNIT_TEST(SplitSmaleFamily_Test);
	CPPUNIT_TEST(GetAllChildLayoutRect_Test);
	CPPUNIT_TEST_SUITE_END();

public:
	KWndWindow_Test();
	~KWndWindow_Test();

	virtual void CompareCopy(bool bAutoReleaseCopy) {  }
	virtual void DummyClassToFile( LPCSTR szFilename) {  }
	virtual void DummyClassForSecondCopy(void* pvClassBeingTest) { }
	virtual void DummyClassForFirstCopy(void* pvClassBeingTest)  { }
	
	void *CopyClassData(const void *CopyData, int size)
	{
		static ClassDataCopyBuffer copy;
		copy.SetFirstCopy(CopyData, size);
		return copy.GetFirstCopy();
	}

	void AddChild_Test(void);
	void AddBrother_Test(void);
	void BringToTop_Test(void);
	void SetPosition_Test(void);
	void GetPosition_Test(void);
	void AbsoluteMove_Test(void);
	void SetSize_Test(void);
	void GetSize_Test(void);
	void GetAbsolutePos_Test(void);
	void PtInWindow_Test(void);
	void TopChildFromPoint_Test(void);
	void SplitSmaleFamily_Test(void);
	void GetAllChildLayoutRect_Test();
	void setUp();
	void tearDown();
	
private:
	
};

#endif //_TEST_UI_RICHTEXT_H