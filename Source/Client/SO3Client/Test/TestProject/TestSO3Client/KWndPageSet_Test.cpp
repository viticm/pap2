/***************************************************
Filename:KWndPageSet_Test.cpp
CreatedBy: WilsonWei
CreatedOn: 2005年7月27日 14:32:24.8338619
Test Requirement Reference by Wiki path:
http://192.168.22.107/pmwiki/pmwiki.php?n=J3Test.J3-QC-REQ-SO3Client_UI_KWndPageSet

Comment:




**************************************************/



#include "StdAfx.h"

#ifdef  _UNIT_TEST
#include "KWndPageSet_Test.h"
#include "../ui/elem/wndpageset.h"
#include "../ui/elem/wndbutton.h"
#include "../ui/elem/wndpage.h"

CPPUNIT_TEST_SUITE_REGISTRATION( KWndPageSet_Test );


KWndPageSet_Test::KWndPageSet_Test()
{
}

KWndPageSet_Test::~KWndPageSet_Test()
{
}

void KWndPageSet_Test::setUp()
{
}

void KWndPageSet_Test::tearDown()
{

}

void KWndPageSet_Test::Test_Init()
{
}
void KWndPageSet_Test::Test_Bind()
{

}
//////////////////////////////////////////////////////////////////////////
//@用例编号 : KWndPageSet_ActivePage_1
//@用例目的 : 
//在没有任何页面的情况下尝试激活
//
//@前置场景 :
//@C1		: KWndPageSet中没有任何Page
//@C2		:
//@操作源   : 测试代码
//@操作对象 : KWndPageSet类
//@操作过程 :
//@OP1		: 新建一个KWndPageSet实例
//@OP2		: 调用KWndPageSet.ActivePage(0)
//@后置场景 :
//@CP1		: ActivePage()返回false
//@CP2		:
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_ActivePage_1()
{
	//确定不存在任何页面
	UI::KWndPageSet pageSet;
	//激活不存在的页面
	bool nRetCode = pageSet.ActivePage(0);
	CPPUNIT_ASSERT_EQUAL(false, nRetCode );
}


//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_ActivePage_2
//@用例目的 : 
//在只有一个页面的情况下,尝试激活存在的页面
//
//@前置场景 :
//@C1		:KWndPageSet中存在两个Page
//@C2		:
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1		:新建一个KWndPageSet实例
//@OP2		:增加两个Page
//@OP3		:调用KWndPageSet.ActivePage(0)
//@后置场景 :
//@CP1		:ActivePage()返回false
//@CP2		:第一页被激活
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_ActivePage_2()
{
	bool nRetCode = false;
	UI::KWndPageSet pageSet;
	UI::KWndPage page;
	UI::KWndPage page2;
	UI::KWndButton btn;
	UI::KWndButton btn2;
	nRetCode = pageSet.AddPage( &page, &btn);
	nRetCode = pageSet.AddPage( &page2, &btn2);
	nRetCode = pageSet.ActivePage(0);
	CPPUNIT_ASSERT( nRetCode == false);
	CPPUNIT_ASSERT_EQUAL( 0,pageSet.m_nAcitvePage);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_ActivePage_3
//@用例目的 :
//在有两个页面的情况下,激活第二个页面
//
//@前置场景 :
//@C1       :KWndPageSet中存在两个Page
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :新建一个KWndPageSet实例
//@OP2      :增加两个Page
//@OP3		:调用KWndPageSet.ActivePage(1)
//@后置场景 :
//@CP1      :ActivePage()返回true
//@CP2      :第二页被激活
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_ActivePage_3()
{
	UI::KWndPageSet pageSet;
	UI::KWndPage page;
	UI::KWndPage page2;
	UI::KWndButton btn;
	UI::KWndButton btn2;
	pageSet.AddPage( &page, &btn);
	pageSet.AddPage( &page2, &btn2);
	bool nRetCode = pageSet.ActivePage(1);
	CPPUNIT_ASSERT_EQUAL( true, nRetCode);
	CPPUNIT_ASSERT_EQUAL( 1, pageSet.m_nAcitvePage);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_ActivePage_4
//@用例目的 : 
//在有两个页面的情况下,激活不存在的页面(负数)
//
//@前置场景 :
//@C1       :KWndPageSet中存在两个Page
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :新建一个KWndPageSet实例
//@OP2      :增加两个Page
//@OP3		:调用KWndPageSet.ActivePage(-1)
//@后置场景 :
//@CP1      :ActivePage()返回false
//@CP2      :m_nAcitvePage = 0
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_ActivePage_4()
{
	UI::KWndPageSet pageSet;
	UI::KWndPage page;
	UI::KWndPage page2;
	UI::KWndButton btn;
	UI::KWndButton btn2;
	pageSet.AddPage( &page, &btn);
	pageSet.AddPage( &page2, &btn2);
	bool nRetCode = pageSet.ActivePage( -1 );
	CPPUNIT_ASSERT_EQUAL( false, nRetCode );
	CPPUNIT_ASSERT_EQUAL( 0, pageSet.m_nAcitvePage);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_ActivePage_5
//@用例目的 :
//在有两个页面的情况下,激活不存在的页面(正大数)
//
//@前置场景 :
//@C1       :KWndPageSet中存在两个Page
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :新建一个KWndPageSet实例
//@OP2      :增加两个Page
//@OP3		:调用KWndPageSet.ActivePage(1000)
//@后置场景 :
//@CP1      :ActivePage()返回false
//@CP2      :m_nAcitvePage=0
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_ActivePage_5()
{
	UI::KWndPageSet pageSet;
	UI::KWndPage page;
	UI::KWndPage page2;
	UI::KWndButton btn;
	UI::KWndButton btn2;
	pageSet.AddPage( &page, &btn);
	pageSet.AddPage( &page2, &btn2);
	bool nRetCode = pageSet.ActivePage( 10000 );
	CPPUNIT_ASSERT_EQUAL( false, nRetCode );
	CPPUNIT_ASSERT_EQUAL( 0, pageSet.m_nAcitvePage);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_GetActivePageIndex_1
//@用例目的 : 
//不存在任何页,获取当前激活索引号
//
//@前置场景 : 
//@C1		: KWndPageSet中不存在任何页
//@C2		:
//@操作源   : 测试代码
//@操作对象 : KWndPageSet
//@操作过程 :
//@OP1		:创建一个KWndPageSet实例
//@OP2		:调用GetActivePageIndex()方法
//@后置场景 :
//@CP1		: GetActivePageIndex返回 -1
//@CP2		:
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_GetActivePageIndex_1()
{
    UI::KWndPageSet pageSet;
    int nPageIndex = pageSet.GetActivePageIndex();
    CPPUNIT_ASSERT_EQUAL( -1, nPageIndex);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_GetActivePageIndex_2
//@用例目的 : 
//测试在只有一个页的情况下,获取激活的页索引
//
//@前置场景 :
//@C1       :KWndPageSet中存在一个页
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet
//@操作过程 :
//@OP1      :创建一个KWndPageSet实例
//@OP2      :添加一个页
//@OP3      :获取当前被激活的页索引号
//@后置场景 :
//@CP1      :GetActivePageIndex返回0
//@CP2      :
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_GetActivePageIndex_2()
{
    UI::KWndPageSet pageSet;
    UI::KWndPage page;
    UI::KWndButton btn;
    pageSet.AddPage( &page, &btn);
    int nPageIndex = pageSet.GetActivePageIndex();
    CPPUNIT_ASSERT_EQUAL( 0, nPageIndex);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_GetActivePageIndex_3
//@用例目的 :
//不存在任何页,获取当前激活索引号
//@前置场景 :
//@C1       :KWndPageSet中不存在任何页
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :新建一个KWndPageSet实例
//@OP2      :调用GetActivePageIndex()
//@后置场景 :
//@CP1      :GetActivePageIndex()返回-1
//@CP2      :
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_GetActivePageIndex_3()
{
	UI::KWndPageSet pageSet;
	UI::KWndPage page;
	int nPageIndex = pageSet.GetActivePageIndex();
	CPPUNIT_ASSERT_EQUAL( -1, nPageIndex);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_AddPage_1
//@用例目的 :不存在任何页,增加一页,检查功能是否正常
//
//@前置场景 :
//@C1       :KWndPageSet中不存在任何页
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :创建一个KWndPageSet实例
//@OP2      :增加一页
//@OP3		:获取页数目
//@后置场景 :
//@CP1      :m_nPageNum = 1
//@CP2      :
//////////////////////////////////////////////////////////////////////////
void KWndPageSet_Test::Test_AddPage_1()
{
    UI::KWndPageSet pageSet;
    UI::KWndPage page;
	UI::KWndButton btn;
	pageSet.AddPage( &page, &btn);
	int nPageNum = pageSet.m_nNumPage;
	CPPUNIT_ASSERT_EQUAL( 1, nPageNum);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_AddPage_2
//@用例目的 :不存在任何页,连续增加100页,检查功能是否正常

//@前置场景 :
//@C1       :KWndPageSet不存在任何页
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :创建一个KWndPageSet实例
//@OP2      :连续增加100页
//@OP3		:获取页数目
//@后置场景 :
//@CP1      :m_nPageNum = 100
//@CP2      :
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_AddPage_2()
{
	UI::KWndPageSet pageSet;
	for( int i=0; i<100 ; i++)
	{
		UI::KWndPage page;
		UI::KWndButton btn;
		pageSet.AddPage( &page, &btn);
	}
	int nPageNum = pageSet.m_nNumPage;
	CPPUNIT_ASSERT_EQUAL( 100, nPageNum);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_GetActivePage_1
//@用例目的 :不存在任何页,获取当前激活页

//@前置场景 :
//@C1       :KWndPageSet中不存在任何页
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :创建一个KWndPageSet实例
//@OP2      :调用GetActivePage
//@后置场景 :
//@CP1      :GetActivePage()返回 NULL
//@CP2      :
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_GetActivePage_1()
{
	UI::KWndPageSet pageSet;
	UI::KWndPage* pRetPage = pageSet.GetActivePage();
	CPPUNIT_ASSERT_MESSAGE("只要返回NULL,则正确!", NULL==pRetPage);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_GetActivePage_2
//@用例目的 :不存在任何页,增加一个新页后,获取当前激活页

//@前置场景 :
//@C1       :KWndPageSet中不存在任何页
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :创建一个KWndPageSet实例
//@OP2      :增加一个新页
//@OP3		:调用GetActivePage()
//@后置场景 :
//@CP1      :GetActivePage()返回新增加的页对象的指针
//@CP2      :
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_GetActivePage_2()
{
	UI::KWndPageSet pageSet;
	UI::KWndPage page;
	UI::KWndButton btn;
	pageSet.AddPage( &page, &btn);
	UI::KWndPage* pRetPage = pageSet.GetActivePage();
	CPPUNIT_ASSERT( pRetPage);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_GetActivePage_3
//@用例目的 :不存在任何页,新增加100页,获取当前激活页

//@前置场景 :
//@C1       :KWndPageSet中不存在任何页
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :创建一个KWndPageSet实例
//@OP2      :新增加100页
//@OP3		:调用GetActivePage()
//@后置场景 :
//@CP1      :GetActivePage()返回第100页的指针
//@CP2      :
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_GetActivePage_3()
{
	UI::KWndPageSet pageSet;
	for(int i = 0; i<100; i++)
	{
		UI::KWndPage page;
		UI::KWndButton btn;
		pageSet.AddPage( &page, &btn);
	}
	UI::KWndPage* pRetPage = pageSet.GetActivePage();
	CPPUNIT_ASSERT( pRetPage );
	CPPUNIT_ASSERT( !(pRetPage->GetNextWnd()));
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_GetActivePage_4
//@用例目的 :不存在任何页,新增加10页,获取当前激活页

//@前置场景 :
//@C1       :KWndPageSet中不存在任何页
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :创建一个KWndPageSet实例
//@OP2      :新增加10页
//@OP3		:将第8页激活
//@OP4		:调用GetActivePage()
//@后置场景 :
//@CP1      :GetActivePage()返回第8页的指针
//@CP2      :
//////////////////////////////////////////////////////////////////////////
void KWndPageSet_Test::Test_GetActivePage_4()
{
	UI::KWndPageSet pageSet;
	for(int i=0; i<10; i++)
	{
		UI::KWndPage page;
		UI::KWndButton btn;
		pageSet.AddPage( &page, &btn);
	}
	bool nRetCode = pageSet.ActivePage(7);
	CPPUNIT_ASSERT_EQUAL( true, nRetCode);
	UI::KWndPage* pActivePage = pageSet.GetActivePage();
	CPPUNIT_ASSERT( pActivePage);
	CPPUNIT_ASSERT_EQUAL( 7, pageSet.m_nAcitvePage);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :KWndPageSet_GetTypeInfo_1
//@用例目的 :不存在任何页,获取类型信息

//@前置场景 :
//@C1       :KWndPageSet中不存在任何页
//@C2       :
//@操作源   :测试代码
//@操作对象 :KWndPageSet类
//@操作过程 :
//@OP1      :创建一个KWndPageSet实例
//@OP2      :调用GetTypeInfo
//@后置场景 :
//@CP1      :GetTypeInfo返回常量指针(不为NULL)
//@CP2      :
//////////////////////////////////////////////////////////////////////////

void KWndPageSet_Test::Test_GetTypeInfo_1()
{
	UI::KWndPageSet pageSet;
	UI::KWndTypeInfo const* pTypeInfo = pageSet.GetTypeInfo();
	CPPUNIT_ASSERT( pTypeInfo );
}
void KWndPageSet_Test::Test_StaticMember()
{
}
void KWndPageSet_Test::DummyClassForFirstCopy(void* pvClassBeingTest)
{
    _testdata.m_nAcitvePage = ((UI::KWndPageSet*) pvClassBeingTest)->m_nAcitvePage;
    _testdata.m_nNumPage    = ((UI::KWndPageSet*) pvClassBeingTest)->m_nNumPage;
    //_testdata.ms_sTypeInfo = ((UI::KWndPageSet*) pvTestData)->ms_sTypeInfo;
    SetFirstCopy( &_testdata, sizeof( _TestData ));
}
void KWndPageSet_Test::DummyClassForSecondCopy(void* pvClassBeingTest)
{
    _testdata.m_nAcitvePage = ((UI::KWndPageSet*) pvClassBeingTest)->m_nAcitvePage;
    _testdata.m_nNumPage    = ((UI::KWndPageSet*) pvClassBeingTest)->m_nNumPage;
    //_testdata.ms_sTypeInfo = ((UI::KWndPageSet*) pvClass)->ms_sTypeInfo;
    SetSecondCopy( &_testdata, sizeof( _TestData ));
}
void KWndPageSet_Test::DummyClassToFile( LPCSTR szFilename)
{
}
void KWndPageSet_Test::CompareCopy(bool bAutoReleaseCopy)
{
    if( NULL != _FirstCopy && NULL != _SecondCopy)
    {
        _TestData* pFirstTestdata = (_TestData*) _FirstCopy;
        _TestData* pSecondTestData = (_TestData*) _SecondCopy;
        CPPUNIT_ASSERT( pFirstTestdata->m_nAcitvePage == pSecondTestData->m_nAcitvePage);
        CPPUNIT_ASSERT( pFirstTestdata->m_nNumPage == pSecondTestData->m_nNumPage);
        if( bAutoReleaseCopy)
            ReleaseAllCopy();
    }
}

#endif //End of #ifdef _UNIT_TEST