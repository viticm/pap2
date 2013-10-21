/*
FileName    :  Richtext_Test.h
Version     :  
Creater     :  YangWeidong
Date        :  2005-7-29 17:03:05
Description :  
*/

#ifndef _RICHTEXT_TEST_H_
#define _RICHTEXT_TEST_H_

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "../ui/elem/wndrichtext.h"
#include "../ui/driver/igraphic.h"
#include "../ui/text/ktextlayout.h"

using namespace UI;

class Richtext_Test : public CppUnit::TestFixture, KingsoftUnitTest::ClassDataCopyBuffer
{
	LPCTSTR              m_RichtextContent;
	int                  m_RichtextSize;
	UI::IGraphic::KColor m_RichtextColor;
	UI::IGraphic::KColor m_RichtextBorderColor;
	int                  m_Topline;
	int                  m_RowSpaceing;

	KWndRichText &m_RichtextObj;

	CPPUNIT_TEST_SUITE(cText_UI_Richtext);

	CPPUNIT_TEST(SetText_test);
	CPPUNIT_TEST(SetTextSize_test);
	CPPUNIT_TEST(SetTextColor_test);
	CPPUNIT_TEST(SetTextBorderColor_test);
	CPPUNIT_TEST(SetTopLine_test );
	CPPUNIT_TEST(SetRowSpacing_test);

	CPPUNIT_TEST_SUITE_END();

public:
	Richtext_Test();
	~Richtext_Test();

	virtual void setUp();
	virtual void tearDown();

	/*
	对richtext设置字符串的功能进行测试, 内容包括以下几个方面: 
	1. 初始化richtext的字符串(无格式字符)
	2. 增加richtext字符串(无格式字符)
	3. 增加richtext字符串(有格式字符)
	4. 传递给richtext不同规格的字符: 单字节、宽字符、unicode字符
	5. 测试richtext所能接受的最大字符长限制
	*/
	void SetText_test();

	/*
	对richtext设置字体大小的功能进行测试, 内容包括以下几个方面: 
	1. 正常大小
	2. 设置字体大小为0
	3. 设置字体大小为负
	4. 设置字体大小为100
	*/
	void SetTextSize_test(int nSize);

	/*
	对richtext设置字体颜色的功能进行测试, 内容包括以下几个方面: 
	1. 正常值 0x334455
	2. 非正常值 0x33445566
	*/
	void SetTextColor_test(UI::IGraphic::KColor sColor);

	/*
	对richtext设置字体边框颜色的功能进行测试, 内容包括以下几个方面: 
	1. 正常值 0x334455
	2. 非正常值 0x33445566
	*/
	void SetTextBorderColor_test(IGraphic::KColor sColor);

	/*
	对richtext设置开始显示行数的进行测试, 内容包括以下几个方面: 
	1. 正常值 0, 1
	2. 非正常值 -1, 99999
	*/
	void SetTopLine_test(int nValue);

	/*
	对richtext设置行间距的功能进行测试, 内容包括以下几个方面: 
	1. 正常值 0, 1
	2. 非正常值 -1, 99999
	*/
	void SetRowSpacing_test(int nValue);

};

#endif //_RICHTEXT_TEST_H_