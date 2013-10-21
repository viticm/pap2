/***************************************************
Filename:WndDropDownList_Test.h
CreatedBy: HouRui
CreatedOn: 2005年7月31日 21:00:00
Test Requirement Reference by Wiki path:
http://192.168.22.107/pmwiki/pmwiki.php?n=J3Test.J3-QC-REQ-SO3Client_UI_dropdownlist
Comment:

**************************************************/

#ifndef WNDDROPDOWNLIST_TEST_H
#define WNDDROPDOWNLIST_TEST_H

#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "../ui/script/kwndscripttable.h"
#include "KSUnitTest/KingsoftUnitTest.h"
#include "KSUnitTest/ClassDataCopyBuffer.H"
//#include "../ui/elem/wnddropdownlist.h"

#include "../ui/elem/wndedit.h"
#include "../ui/elem/wndimage.h"
#include "../ui/elem/wndlist2.h"

class WndDropDownList_Test : 
        public CppUnit::TestFixture,
        public KingsoftUnitTest::ClassDataCopyBuffer
        //public KingsoftUnitTest::IClassUnitTest
{
    CPPUNIT_TEST_SUITE(WndDropDownList_Test);

    CPPUNIT_TEST(Test_InsertItem_1);
    CPPUNIT_TEST(Test_InsertItem_2);
    CPPUNIT_TEST(Test_InsertItem_3);
    CPPUNIT_TEST(Test_InsertItem_4);
    CPPUNIT_TEST(Test_InsertItem_5);
    CPPUNIT_TEST(Test_InsertItem_6);
    CPPUNIT_TEST(Test_InsertItem_7);
    CPPUNIT_TEST(Test_InsertItem_8);
    CPPUNIT_TEST(Test_InsertItem_9);
    CPPUNIT_TEST(Test_InsertItem_10);
    CPPUNIT_TEST(Test_InsertItem_11);
    CPPUNIT_TEST(Test_InsertItem_12);
	CPPUNIT_TEST(Test_InsertItem_13);
    CPPUNIT_TEST(Test_InsertItem_14);
	CPPUNIT_TEST(Test_InsertItem_15);
	CPPUNIT_TEST(Test_InsertItem_16);
	CPPUNIT_TEST(Test_InsertItem_17);
	CPPUNIT_TEST(Test_InsertItem_18);
	CPPUNIT_TEST(Test_InsertItem_19);
	CPPUNIT_TEST(Test_InsertItem_20);
	CPPUNIT_TEST(Test_InsertItem_21);

    CPPUNIT_TEST(Test_DeleteItem_1);
    CPPUNIT_TEST(Test_DeleteItem_2);
    CPPUNIT_TEST(Test_DeleteItem_3);
    CPPUNIT_TEST(Test_DeleteItem_4);
    CPPUNIT_TEST(Test_DeleteItem_5);

    CPPUNIT_TEST(Test_SelectItem_1);
    CPPUNIT_TEST(Test_SelectItem_2);
    CPPUNIT_TEST(Test_SelectItem_3);
	CPPUNIT_TEST(Test_SelectItem_4);

    CPPUNIT_TEST(Test_SearchItem_1);
    CPPUNIT_TEST(Test_SearchItem_2);
    CPPUNIT_TEST(Test_SearchItem_3);
    CPPUNIT_TEST(Test_SearchItem_4);
    CPPUNIT_TEST(Test_SearchItem_5);

    CPPUNIT_TEST(Test_SetFontSize_1); 
    CPPUNIT_TEST(Test_SetFontSize_2);

    CPPUNIT_TEST(Test_SetTipColor_1);
    CPPUNIT_TEST(Test_SetTipColor_2);

    CPPUNIT_TEST(Test_SetListBGColor_1); 
    CPPUNIT_TEST(Test_SetListBGColor_2); 

    CPPUNIT_TEST(Test_SetListSLColor_1);
    CPPUNIT_TEST(Test_SetListSLColor_2);

    CPPUNIT_TEST(Test_SetListFLColor_1);
    CPPUNIT_TEST(Test_SetListFLColor_2);

    CPPUNIT_TEST_SUITE_END();

//public:
//    WndDropDownList_Test(void);
//    ~WndDropDownList_Test(void);

//private:
//    struct _TestData
//    {
//        _TestData(){};
//        ~_TestData(){};
//        BOOL			m_bEditable;		// 设置能否被编辑
//        BOOL			m_bEnableTip;		// 禁止或允许显示Tip
//        int		    m_nFontSize;		// 字体大小
//        int			m_nMaxItemNum;		// 允许记录的最大记录条数目
//        char			m_szTipInfo[128];	// Tip提示
//        DWORD			m_dwTipColor;		// Tip文字颜色
//        DWORD			m_dwListBGColor;	// 下拉列表的背景颜色
//        DWORD			m_dwListSLColor;	// 下拉列表的分隔线颜色
//        DWORD			m_dwListFLColor;	// 下拉列表的边框颜色
//        BOOL			m_bListExtend;		// 是否处于展开状态
//    };
//    _TestData _testdata;

public:
    void setUp();           
    void tearDown();

public:
    //插入新项测试（包含2个被测函数：指定位置插入和在列表末尾插入）
    void Test_InsertItem_1();
    void Test_InsertItem_2();
    void Test_InsertItem_3();
    void Test_InsertItem_4();
    void Test_InsertItem_5();
    void Test_InsertItem_6();
    void Test_InsertItem_7();
    void Test_InsertItem_8();
    void Test_InsertItem_9();
    void Test_InsertItem_10();
    void Test_InsertItem_11();
    void Test_InsertItem_12();
	void Test_InsertItem_13();
	void Test_InsertItem_14();
	void Test_InsertItem_15();
	void Test_InsertItem_16();
	void Test_InsertItem_17();
	void Test_InsertItem_18();
	void Test_InsertItem_19();
	void Test_InsertItem_20();
	void Test_InsertItem_21();

    //删除项测试（包含2个被测函数：指定位置删除和在列表末尾删除）
    void Test_DeleteItem_1();
    void Test_DeleteItem_2();
    void Test_DeleteItem_3();
    void Test_DeleteItem_4();
    void Test_DeleteItem_5();

    //设置当前选择项测试
    void Test_SelectItem_1();
    void Test_SelectItem_2();
    void Test_SelectItem_3();
    void Test_SelectItem_4();

    //搜索指定项的测试（包含2个被测函数：搜索指定字符串值的项和搜索指定数据值的项）
    void Test_SearchItem_1();
    void Test_SearchItem_2();
    void Test_SearchItem_3();
    void Test_SearchItem_4();
    void Test_SearchItem_5();

    //设置字体大小测试
    void Test_SetFontSize_1();
    void Test_SetFontSize_2();

    //设置Tip文字颜色测试
    void Test_SetTipColor_1();
    void Test_SetTipColor_2();

    //设置下拉列表的背景颜色测试
    void Test_SetListBGColor_1();
    void Test_SetListBGColor_2();

    //设置下拉列表的分隔线颜色测试
    void Test_SetListSLColor_1();
    void Test_SetListSLColor_2();

    //设置下拉列表的边框颜色测试
    void Test_SetListFLColor_1();
    void Test_SetListFLColor_2();

private:
    UI::KWndEdit          *m_pEditTextBox1;
    UI::KWndImage		  *m_pButtonImage1;
    UI::KWndList2		  *m_pItemList1;
    //UI::KWndDropDownList  *m_dropDownList;
//public:
//    /*将类的内存变量复制到第一份拷贝*/
//    void DummyClassForFirstCopy(void* pvClassBeingTest);
//    /*将类的内存变量复制到第二份拷贝*/
//    void DummyClassForSecondCopy(void* pvClassBeingTest);
//    /*将类的内存变量保存到指定的文件*/
//    void DummyClassToFile( LPCSTR szFilename);
//    /*比较两份拷贝*/
//    void CompareCopy(bool bAutoReleaseCopy);
};

#endif //WNDDROPDOWNLIST_TEST_H