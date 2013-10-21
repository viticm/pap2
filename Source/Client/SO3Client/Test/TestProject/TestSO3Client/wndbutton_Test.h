/***************************************************
Filename:Test_UI_WndButton.cpp
CreatedBy: zhaoyunhua
CreatedOn: 2005Äê7ÔÂ29ÈÕ 10:21:29.6486898
Test Requirement Reference by Wiki path:
http://192.168.22.107/pmwiki/pmwiki.php?n=J3Test.J3-QC-REQ-SO3Client_UI_button
Comment:

**************************************************/




#ifndef _WNDBUTTON_TEST_H
#define _WNDBUTTON_TEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "../ui/driver/igraphic.h"
#include "KSUnitTest/KingsoftUnitTest.H"
#include "KSUnitTest/ClassDataCopyBuffer.H"

using namespace UI;
using namespace KingsoftUnitTest;


class KWndButton_Test : public CppUnit::TestFixture , public IClassUnitTest
{
	CPPUNIT_TEST_SUITE(KWndButton_Test);
	CPPUNIT_TEST(SetToolTipInfo_test);
	CPPUNIT_TEST(GetToolTipInfo_Test);
	CPPUNIT_TEST_SUITE_END();

public:
	KWndButton_Test();
	~KWndButton_Test();

	virtual void setUp();
	virtual void tearDown();

	virtual void CompareCopy(bool bAutoReleaseCopy) {  }
	virtual void DummyClassToFile( LPCSTR szFilename) {  }
	virtual void DummyClassForSecondCopy(void* pvClassBeingTest) 
	{ 
		
	}
	virtual void DummyClassForFirstCopy(void* pvClassBeingTest)  
	{
		
	}

	void *CopyClassData(const void *CopyData, int size)
	{
		static ClassDataCopyBuffer copy;
		copy.SetFirstCopy(CopyData, size);
		return copy.GetFirstCopy();
	}

private:
	void GetToolTipInfo_Test(void);

	void SetToolTipInfo_test(void);

	ClassDataCopyBuffer copy;
	
};

#endif //_TEST_UI_RICHTEXT_H