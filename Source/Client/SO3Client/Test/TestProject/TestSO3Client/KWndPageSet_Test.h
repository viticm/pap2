#pragma once

/***************************************************
Filename:Test_UI_wndpageset.h
CreatedBy: WilsonWei
CreatedOn: 2005年7月27日 10:51:16.7778130
Test Requirement Reference by Wiki path:
http://192.168.22.107/pmwiki/pmwiki.php?n=J3Test.J3-QC-REQ-SO3Client_UI_KWndPageSet
Comment:




**************************************************/



#include "cppunit/extensions/HelperMacros.h"
#include "cppunit/TestFixture.h"
#include "../ui/elem/wndshowanimate.h"
#include "KSUnitTest/KingsoftUnitTest.h"
#include "KSUnitTest/ClassDataCopyBuffer.H"

class KWndPageSet_Test : public CppUnit::TestFixture, public KingsoftUnitTest::ClassDataCopyBuffer, public KingsoftUnitTest::IClassUnitTest
{
public:
	KWndPageSet_Test();
	~KWndPageSet_Test();
private:
    struct _TestData
    {
        _TestData(){};
        ~_TestData(){};
        int				m_nNumPage;					//页面的数目
        int				m_nAcitvePage;				//当前激活的页面
    };
    _TestData _testdata;
public: 
    void setUp();
    void tearDown();
public:
    CPPUNIT_TEST_SUITE( KWndPageSet_Test );
    CPPUNIT_TEST( Test_Init);
    CPPUNIT_TEST( Test_Bind);
    CPPUNIT_TEST( Test_ActivePage_1);
    CPPUNIT_TEST( Test_ActivePage_2);
	CPPUNIT_TEST( Test_ActivePage_3);
	CPPUNIT_TEST( Test_ActivePage_4);
	CPPUNIT_TEST( Test_ActivePage_5);
    CPPUNIT_TEST( Test_GetActivePageIndex_1);
    CPPUNIT_TEST( Test_GetActivePageIndex_2);
	CPPUNIT_TEST( Test_GetActivePageIndex_3);
    CPPUNIT_TEST( Test_AddPage_1);
    CPPUNIT_TEST( Test_AddPage_2);
    CPPUNIT_TEST( Test_GetActivePage_1);
    CPPUNIT_TEST( Test_GetActivePage_2);
	CPPUNIT_TEST( Test_GetActivePage_3);
	CPPUNIT_TEST( Test_GetActivePage_4);
    CPPUNIT_TEST( Test_GetTypeInfo_1);
    CPPUNIT_TEST( Test_StaticMember);
    CPPUNIT_TEST_SUITE_END();

private:
    void Test_Init();
    void Test_Bind();
    void Test_ActivePage_1();
    void Test_ActivePage_2();
	void Test_ActivePage_3();
	void Test_ActivePage_4();
	void Test_ActivePage_5();
    void Test_GetActivePageIndex_1();
    void Test_GetActivePageIndex_2();
	void Test_GetActivePageIndex_3();
    void Test_GetTypeInfo_1();
    void Test_AddPage_1();
    void Test_AddPage_2();
    void Test_StaticMember();
    void Test_GetActivePage_1();
    void Test_GetActivePage_2();
	void Test_GetActivePage_3();
	void Test_GetActivePage_4();

public:
    /*将类的内存变量复制到第一份拷贝*/
    void DummyClassForFirstCopy(void* pvClassBeingTest);
    /*将类的内存变量复制到第二份拷贝*/
    void DummyClassForSecondCopy(void* pvClassBeingTest);
    /*将类的内存变量保存到指定的文件*/
    void DummyClassToFile( LPCSTR szFilename);
    /*比较两份拷贝*/
    void CompareCopy(bool bAutoReleaseCopy);
};
