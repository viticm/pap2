/***************************************************
Filename:wndRichText_Test.h
CreatedBy: YangWeidong
CreatedOn: 2005Äê8ÔÂ15ÈÕ 14:42:53.9916240
Test Requirement Reference by Wiki path:
http://192.168.22.107/qcweb/pmwiki.php?n=XSJQC-DEV.J3-QC-CASE-SO3Client_UI_RichText-Original
Comment:
**************************************************/

#ifndef _WNDRichText_TEST_H
#define _WNDRichText_TEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "KSUnitTest/KingsoftUnitTest.H"
#include "KSUnitTest/ClassDataCopyBuffer.H"
#include "../ui/driver/igraphic.h"
#include "../ui/text/ktext.h"

using namespace UI;
using namespace KingsoftUnitTest;

class KWndRichText_Test : public CppUnit::TestFixture , public IClassUnitTest
{
	CPPUNIT_TEST_SUITE(KWndRichText_Test);
	CPPUNIT_TEST(SetFontSize_Test);
	CPPUNIT_TEST(SetTextColor_Test);
	CPPUNIT_TEST(SetTextBorderColor_Test);
	CPPUNIT_TEST(SetRowSpacing_Test);
	CPPUNIT_TEST(SetTopLine_Test);
	//CPPUNIT_TEST(SetText_Test);

	CPPUNIT_TEST(Encode_Test_FontSize);
	CPPUNIT_TEST(Encode_Test_FontType);
	CPPUNIT_TEST(Encode_Test_FontColor);
	CPPUNIT_TEST(Encode_Test_FontBorderSize);
	CPPUNIT_TEST(Encode_Test_FontBorderColor);
	CPPUNIT_TEST(Encode_Test_Event);
	CPPUNIT_TEST(Encode_Test_Pic);
	CPPUNIT_TEST(Encode_Test_Newline);
	CPPUNIT_TEST_SUITE_END();

public:
	KWndRichText_Test();
	~KWndRichText_Test();

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

	void SetFontSize_Test();
	void SetTextColor_Test();
	void SetTextBorderColor_Test();
	void SetRowSpacing_Test();
	void SetTopLine_Test();

	void Encode_Test_FontSize();
	void Encode_Test_FontType();
	void Encode_Test_FontColor();
	void Encode_Test_FontBorderSize();
	void Encode_Test_FontBorderColor();
	void Encode_Test_Event();
	void Encode_Test_Pic();
	void Encode_Test_Newline();

	void WriteTestString(LPTSTR ToString, int& iWritePos, LPTSTR FromString, int iWriteLength = 0);
	void WriteTestCtrl(LPTSTR ToString, int& iWritePos, UI::KText::KTEXT_CTRL_CODE nCtrl);
	template<class T>
	void WriteTestOther(LPTSTR ToString, int& iWritePos, T iCtrl)
	{
		byte * pWrite = reinterpret_cast<byte*>(&iCtrl);
		int iWriteLength = sizeof(T);
		for (int i=0; i<iWriteLength; i++)
		{
			ToString[iWritePos++] = pWrite[i];
		}
		ToString[iWritePos] = '\0';
	};

	void CompareTestString(LPTSTR TestEncodeString, LPTSTR ProgramEncodeString, int iCmpLength = 0);

	void setUp();
	void tearDown();

private:

};

#endif