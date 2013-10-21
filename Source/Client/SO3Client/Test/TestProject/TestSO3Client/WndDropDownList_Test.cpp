/***************************************************
Filename:  testDropdownlist.cpp
CreatedBy: HouRui 
CreatedOn: 2005年7月31日 21:00:00
Test Requirement Reference by Wiki path:
http://192.168.22.107/pmwiki/pmwiki.php?n=J3Test.J3-QC-REQ-SO3Client_UI_dropdownlist
Comment:

**************************************************/

#include "stdafx.h"
#include "WndDropDownList_Test.h"
#include "../ui/elem/wnddropdownlist.h"


#ifdef _UNIT_TEST

#include <iostream>
using namespace std;
UI::KWndDropDownList  *m_dropDownList;
CPPUNIT_TEST_SUITE_REGISTRATION(WndDropDownList_Test);



void WndDropDownList_Test::setUp()
{
	m_pEditTextBox1 = new UI::KWndEdit;
    m_pButtonImage1 = new UI::KWndButton;
    m_pItemList1    = new UI::KWndList2;
    m_dropDownList  = new UI::KWndDropDownList;
    m_dropDownList->BindEdit(m_pEditTextBox1);
    m_dropDownList->BindImage(m_pButtonImage1);
    m_dropDownList->BindList2(m_pItemList1);
}

void WndDropDownList_Test::tearDown()
{
    if (m_pEditTextBox1)
    {
        delete m_pEditTextBox1;
        m_pEditTextBox1 = NULL;
    }
    if (m_pButtonImage1)
    {
        delete m_pButtonImage1;
        m_pButtonImage1 = NULL;
    }
    if (m_pItemList1)
    {
        delete m_pItemList1;
        m_pItemList1 = NULL;
    }
    if (m_dropDownList)
    {
        delete m_dropDownList;
        m_dropDownList = NULL;
    }   
}

//WndDropDownList_Test::WndDropDownList_Test()
//{
//}
//
//WndDropDownList_Test::~WndDropDownList_Test()
//{
//}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.1 
//@用例目的 :测试InsertItem是否能在列表为空的时候插入新项
//@前置场景 :
//@C1       :列表为空，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入新项
//@后置场景 :
//@CP1      :可以插入新项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_1()
{
    int nRetCode		    = false;
    const char cszStrName[] = "item1";
    char szStr[10];

	nRetCode = m_dropDownList->InsertItem(0, cszStrName);
    CPPUNIT_ASSERT(nRetCode == 0);
	nRetCode = m_dropDownList->GetItemCount();
    CPPUNIT_ASSERT(nRetCode == 1);
	nRetCode = m_dropDownList->GetItemText(0, szStr, sizeof(szStr));
	CPPUNIT_ASSERT(nRetCode);
	CPPUNIT_ASSERT(strcmp(szStr, cszStrName) == 0);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.2
//@用例目的 :验证能够在列表正常情况下在表头，表中，表尾插入新项
//@前置场景 :
//@C1       :列表正常，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :在表头，表中，表尾分别插入新项
//@后置场景 :
//@CP1      :可以在表头，表中，表尾插入新项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_2()
{
    int nRetCode				= false;
    int i						= 0;
    const char *cszStrName[]    = {"item2", "item4", "item1", "item3", "item5"};
    const char *cszStrNameNew[] = {"item1", "item2", "item3", "item4", "item5"};
    char szStr[10]				= {0};  

    for (i = 0; i < 2; i++)
	{
		nRetCode = m_dropDownList->InsertItem(i, cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode == i);
	}
	//在表头插入
	nRetCode = m_dropDownList->InsertItem(0, cszStrName[2]);
	CPPUNIT_ASSERT(nRetCode == 0);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);
	
	//在表中插入
	nRetCode = m_dropDownList->InsertItem(2, cszStrName[3]);
	CPPUNIT_ASSERT(nRetCode == 2);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 4);

	//在表尾插入
	nRetCode = m_dropDownList->InsertItem(4, cszStrName[4]);
	CPPUNIT_ASSERT(nRetCode == 4);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 5);

    for (i = 0; i < 5; i++)
    {
        nRetCode = m_dropDownList->GetItemText(i, szStr, sizeof(szStr));
        CPPUNIT_ASSERT(nRetCode > 0);
        CPPUNIT_ASSERT(strcmp(szStr, cszStrNameNew[i]) == 0);    
    }   
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.3
//@用例目的 :验证能够在列表的末尾插入包含特殊字符的新项
//@前置场景 :
//@C1       :列表正常，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入特殊字符
//@后置场景 :
//@CP1      :可以插入包含特殊字符的新项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_3()
{
    int nRetCode			 = false;
    int i					 = 0;
    const char *cszStrName[] = {"item1", "item2", "item3", "＋∨∑∏∪∥∞∮"};
    char szStr[20]			 = {0};  

    for (i = 0; i < 4; i++)
    {
        nRetCode = m_dropDownList->InsertItem(i, cszStrName[i]);
        CPPUNIT_ASSERT(nRetCode == i);
		if (i == 3)
		{
			nRetCode = m_dropDownList->GetItemCount();
			CPPUNIT_ASSERT(nRetCode == 4);
			nRetCode = m_dropDownList->GetItemText(3, szStr, sizeof(szStr));
			CPPUNIT_ASSERT(strcmp(szStr, cszStrName[3]) == 0);
		}
    }
    
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.4
//@用例目的 :验证字符串指针为空的时候不能插入
//@前置场景 :
//@C1       :列表正常，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入时传入空指针
//@后置场景 :不能插入新项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_4()
{
    int nRetCode	   = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};
    char *szStr		   = NULL;

	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->InsertItem(i, cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode == i);
	}

	nRetCode = m_dropDownList->InsertItem(3, szStr);
	CPPUNIT_ASSERT(nRetCode == -1);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);

}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.5
//@用例目的 :验证不能插入空字符串
//@前置场景 :
//@C1       :列表正常，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :在列表的末尾插入插入空字符串
//@后置场景 :
//@CP1      :不能插入空字符串
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_5()
{
	int nRetCode	   = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};
	char *szStr		   = "";

	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->InsertItem(i, cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode == i);
	}

	nRetCode = m_dropDownList->InsertItem(3, szStr);
	CPPUNIT_ASSERT(nRetCode == -1);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.6
//@用例目的 :验证不能插入空格
//@前置场景 :
//@C1       :列表正常，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入空格
//@后置场景 :
//@CP1      :不能插入空格
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_6()
{
	int nRetCode	   = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};
	char *szStr		   = " ";

	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->InsertItem(i, cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode == i);
	}

	nRetCode = m_dropDownList->InsertItem(3, szStr);
	CPPUNIT_ASSERT(nRetCode == -1);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.7
//@用例目的 :验证不能插入超长字符串260个a
//@前置场景 :
//@C1       :列表正常，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入超长字符串
//@后置场景 :
//@CP1      :可以插入超长字符串
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_7()
{
	int nRetCode	   = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};
	char szStr[]	   = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	char szOutStr[270];
	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->InsertItem(i, cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode == i);
	}

	nRetCode = m_dropDownList->InsertItem(3, szStr);
	CPPUNIT_ASSERT(nRetCode == 3);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 4);
	nRetCode = m_dropDownList->GetItemTextLength(3);
	CPPUNIT_ASSERT(nRetCode == 260);
	nRetCode = m_dropDownList->GetItemText(3, szOutStr, sizeof(szOutStr));
	CPPUNIT_ASSERT(nRetCode);
	CPPUNIT_ASSERT(strcmp(szOutStr, szStr) == 0);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.8
//@用例目的 :验证不能插入到不存在位置
//@前置场景 :
//@C1       :列表正常，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :要插入的位置不存在
//@后置场景 :
//@CP1      :不能插入到不存在位置
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_8()
{
	int nRetCode	   = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};
	char *szStr		   = "item";
	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->InsertItem(i, cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode == i);
	}

	nRetCode = m_dropDownList->InsertItem(5, szStr);
	CPPUNIT_ASSERT(nRetCode == -1);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.9
//@用例目的 :验证不能插入重复的项
//@前置场景 :
//@C1       :列表正常，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入重复的项
//@后置场景 :
//@CP1      :不能插入重复的项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_9()
{
	int nRetCode		    = false;
	const char cszStrName[] = "item1";

	nRetCode = m_dropDownList->InsertItem(0, cszStrName);
	CPPUNIT_ASSERT(nRetCode == 0);
	nRetCode = m_dropDownList->InsertItem(1, cszStrName);
	CPPUNIT_ASSERT(nRetCode == -1);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 1);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.10
//@用例目的 :验证不能插入没有结束符的字符串
//@前置场景 :
//@C1       :列表正常，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入没有结束符的字符串
//@后置场景 :
//@CP1      :不能插入没有结束符的字符串
//////////////////////////////////////////////////////////////////////////

void WndDropDownList_Test::Test_InsertItem_10()
{
	int nRetCode		   = false;
	int i				   = 0;
	char szStr[20]		   = {0};
	const char cszCmpStr[] = "aa";

	char szName[2] = "";
	szName[0]	   = 'a';
	szName[1]	   = 'a';

	nRetCode = m_dropDownList->InsertItem(0, szName);
	CPPUNIT_ASSERT(nRetCode == 0);
	nRetCode = m_dropDownList->GetItemTextLength(0);
	CPPUNIT_ASSERT(nRetCode == 2);
	nRetCode = m_dropDownList->GetItemText(0, szStr, sizeof(szStr));
	CPPUNIT_ASSERT(nRetCode == 2);
	CPPUNIT_ASSERT(strcmp(szStr, cszCmpStr) == 0);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.11
//@用例目的 :验证在列表为空的时候可以插入新项
//@前置场景 :
//@C1       :列表为空，用AddItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入新项
//@后置场景 :
//@CP1      :可以插入新项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_11()
{
	int nRetCode		    = false;
	const char cszStrName[] = "item1";
	char szStr[10];

	nRetCode = m_dropDownList->AddItem(cszStrName);
	CPPUNIT_ASSERT(nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 1);
	nRetCode = m_dropDownList->GetItemText(0, szStr, sizeof(szStr));
	CPPUNIT_ASSERT(nRetCode);
	CPPUNIT_ASSERT(strcmp(szStr, cszStrName) == 0);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.12
//@用例目的 :验证列表正常情况下可以插入新项
//@前置场景 :
//@C1       :列表正常，用AddItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入新项
//@后置场景 :
//@CP1      :可以插入新项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_12()
{
	int nRetCode		    = false;
	const char *cszStrName[] = {"item1", "item2"};
	char szStr[10];
    
	nRetCode = m_dropDownList->AddItem(cszStrName[0]);
	CPPUNIT_ASSERT(nRetCode);
	nRetCode = m_dropDownList->AddItem(cszStrName[1]);
	CPPUNIT_ASSERT(nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 2);
	nRetCode = m_dropDownList->GetItemText(1, szStr, sizeof(szStr));
	CPPUNIT_ASSERT(nRetCode);
	CPPUNIT_ASSERT(strcmp(szStr, cszStrName[1]) == 0);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.13
//@用例目的 :验证可以插入特殊字符
//@前置场景 :
//@C1       :列表正常，用AddItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入特殊字符
//@后置场景 :
//@CP1      :可以插入特殊字符
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_13()
{
	int nRetCode			 = false;
	int i					 = 0;
	const char *cszStrName[] = {"item1", "item2", "item3", "＋∨∑∏∪∥∞∮"};
	char szStr[20]			 = {0};  

	for (i = 0; i < 4; i++)
	{
		nRetCode = m_dropDownList->AddItem(cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode);
		if (i == 3)
		{
			nRetCode = m_dropDownList->GetItemCount();
			CPPUNIT_ASSERT(nRetCode == 4);
			nRetCode = m_dropDownList->GetItemText(3, szStr, sizeof(szStr));
			CPPUNIT_ASSERT(strcmp(szStr, cszStrName[3]) == 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.14
//@用例目的 :验证插入时传入空指针不能插入成功
//@前置场景 :
//@C1       :列表正常，用AddItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入时传入空指针
//@后置场景 :
//@CP1      :插入时传入空指针不能插入成功
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_14()
{
	int nRetCode	   = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};
	char *szStr		   = NULL;

	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->AddItem(cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode);
	}

	nRetCode = m_dropDownList->AddItem(szStr);
	CPPUNIT_ASSERT(!nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.15
//@用例目的 :验证不可以插入空字符串
//@前置场景 :
//@C1       :列表正常，用AddItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入空字符串
//@后置场景 :
//@CP1      :不可以插入空字符串
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_15()
{
	int nRetCode	   = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};
	char *szStr		   = "";

	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->AddItem(cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode);
	}

	nRetCode = m_dropDownList->AddItem(szStr);
	CPPUNIT_ASSERT(!nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.16
//@用例目的 :验证不可以插入空格
//@前置场景 :
//@C1       :列表正常，用AddItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入空格
//@后置场景 :
//@CP1      :不可以插入空格
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_16()
{
	int nRetCode	   = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};
	char *szStr		   = " ";

	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->AddItem(cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode);
	}

	nRetCode = m_dropDownList->AddItem(szStr);
	CPPUNIT_ASSERT(!nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.17
//@用例目的 :验证不可以插入超常字符串
//@前置场景 :
//@C1       :列表正常，用AddItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入超长字符串
//@后置场景 :
//@CP1      :不可以插入超长字符串
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_17()
{
	int nRetCode	   = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};
	char szStr[]	   = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	char szOutStr[270];
	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->AddItem(cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode);
	}

	nRetCode = m_dropDownList->AddItem(szStr);
	CPPUNIT_ASSERT(nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 4);
	nRetCode = m_dropDownList->GetItemTextLength(3);
	CPPUNIT_ASSERT(nRetCode == 260);
	nRetCode = m_dropDownList->GetItemText(3, szOutStr, sizeof(szOutStr));
	CPPUNIT_ASSERT(nRetCode);
	CPPUNIT_ASSERT(strcmp(szOutStr, szStr) == 0);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.18
//@用例目的 :验证不可以插入重复的项
//@前置场景 :
//@C1       :列表正常，用AddItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入新项
//@后置场景 :
//@CP1      :可以插入新项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_18()
{
	int nRetCode		    = false;
	const char cszStrName[] = "item1";

	nRetCode = m_dropDownList->AddItem(cszStrName);
	CPPUNIT_ASSERT(nRetCode);
	nRetCode = m_dropDownList->AddItem(cszStrName);
	CPPUNIT_ASSERT(!nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 1);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.19
//@用例目的 :验证不可以插入没有结束符的字符串
//@前置场景 :
//@C1       :列表正常，用AddItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入没有结束符的字符串
//@后置场景 :
//@CP1      :不可以插入没有结束符的字符串
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_19()
{
	int nRetCode		   = false;
	int i				   = 0;
	char szStr[20]		   = {0};
	const char cszCmpStr[] = "aa";

	char szName[2] = {0};
	szName[0]	   = 'a';
	szName[1]	   = 'a';

	nRetCode = m_dropDownList->AddItem(szName);
	CPPUNIT_ASSERT(nRetCode);
	nRetCode = m_dropDownList->GetItemTextLength(0);
	CPPUNIT_ASSERT(nRetCode == 2);
	nRetCode = m_dropDownList->GetItemText(0, szStr, sizeof(szStr));
	CPPUNIT_ASSERT(nRetCode == 2);
	CPPUNIT_ASSERT(strcmp(szStr, cszCmpStr) == 0);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.20
//@用例目的 :验证插入开头，中间或结尾有空格的字符串的时候，实际插入字符串是否一致
//@前置场景 :
//@C1       :列表正常，用InsertItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入开头，中间或结尾有空格的字符串
//@后置场景 :
//@CP1      :检查实际插入字符串是否一致
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_20()
{
	int nRetCode = false;
	int i = 0;
	char *cszStrName[] = {"item1", "item2", "item3", "  item4", "item    5", "item6    "};
	char szStr[20];

    for (i = 0; i < 6; i++)
	{
		nRetCode = m_dropDownList->InsertItem(i, cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode == i);
	}
    nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 6);
	for (i = 3; i < 6; i++)
	{
		nRetCode = m_dropDownList->GetItemText(i, szStr, sizeof(szStr));
		CPPUNIT_ASSERT(nRetCode);
		nRetCode = strcmp(szStr,cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode == 0);
	}
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1.21
//@用例目的 :验证插入开头，中间或结尾有空格的字符串的时候，实际插入字符串是否一致
//@前置场景 :
//@C1       :列表正常，用AddItem插入
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :插入开头，中间或结尾有空格的字符串
//@后置场景 :
//@CP1      :检查实际插入字符串是否一致
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_InsertItem_21()
{
	int nRetCode = false;
	int i = 0;
	char *cszStrName[] = {"item1", "item2", "item3", "  item4", "item    5", "item6    "};
	char szStr[20];

	for (i = 0; i < 6; i++)
	{
		nRetCode = m_dropDownList->AddItem(cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode);
	}
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 6);
	for (i = 3; i < 6; i++)
	{
		nRetCode = m_dropDownList->GetItemText(i, szStr, sizeof(szStr));
		CPPUNIT_ASSERT(nRetCode);
		nRetCode = strcmp(szStr,cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode == 0);
	}
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :2.1
//@用例目的 :验证空列表不能删除项
//@前置场景 :
//@C1       :列表为空，用DeleteItem删除
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :删除列表项
//@后置场景 :
//@CP1      :不可以删除列表项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_DeleteItem_1()
{
 int nRetCode = true;
 
 nRetCode = m_dropDownList->DeleteItem(2);
 CPPUNIT_ASSERT(!nRetCode);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :2.2
//@用例目的 :验证可以删除表头，表中，表尾的项
//@前置场景 :
//@C1       :列表正常，用DeleteItem删除
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :删除表头，表中，表尾的项
//@后置场景 :
//@CP1      :可以删除表头，表中，表尾的项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_DeleteItem_2()
{
	int nRetCode				 = false;
	int i						 = 0;
	char *cszStrName[]			 = {"item1", "item2", "item3", "item4", "item5", "item6"};
	const char *cszStrNameRest[] = {"item2", "item3", "item5"};
	char szstr[10];

	for (i = 0; i < 6; i++)
	{
		nRetCode = m_dropDownList->AddItem(cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode);
	}
	//删除表头项
	nRetCode = m_dropDownList->DeleteItem(0);
	CPPUNIT_ASSERT(nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 5);
	//删除表中项
    nRetCode = m_dropDownList->DeleteItem(2);
	CPPUNIT_ASSERT(nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 4);
	//删除表尾项
	nRetCode = m_dropDownList->DeleteItem(3);
	CPPUNIT_ASSERT(nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);

	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->GetItemText(i, szstr, sizeof(szstr));
		CPPUNIT_ASSERT(nRetCode);
		CPPUNIT_ASSERT(strcmp(szstr, cszStrNameRest[i]) == 0);
	}
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :2.3
//@用例目的 :验证不可以删除不存在的项
//@前置场景 :
//@C1       :列表正常，用DeleteItem删除
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :删除不存在的项
//@后置场景 :
//@CP1      :不可以删除不存在的项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_DeleteItem_3()
{
	int nRetCode	   = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};
    
	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->AddItem(cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode);
	}
	nRetCode = m_dropDownList->DeleteItem(5);
	CPPUNIT_ASSERT(!nRetCode);
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :2.4
//@用例目的 :验证当前列表为空的情况下不能删除
//@前置场景 :
//@C1       :列表为空，用DeleteAllItem删除
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :删除列表所有项
//@后置场景 :
//@CP1      :不能删除
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_DeleteItem_4()
{
	int nRetCode = true;

	m_dropDownList->DeleteAllItem();
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 0);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :2.5
//@用例目的 :验证列表正常情况下可以删除所有项
//@前置场景 :
//@C1       :列表正常，用DeleteAllItem删除
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :删除列表所有项
//@后置场景 :
//@CP1      :可以删除列表的项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_DeleteItem_5()
{
	int nRetCode       = false;
	int i			   = 0;
	char *cszStrName[] = {"item1", "item2", "item3"};

	for (i = 0; i < 3; i++)
	{
		nRetCode = m_dropDownList->AddItem(cszStrName[i]);
		CPPUNIT_ASSERT(nRetCode);
	}
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 3);
	m_dropDownList->DeleteAllItem();
	nRetCode = m_dropDownList->GetItemCount();
	CPPUNIT_ASSERT(nRetCode == 0);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :3.1
//@用例目的 :验证不能列表为空的情况下设置当前选择项
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置当前选择项
//@后置场景 :
//@CP1      :不能设置当前选择项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SelectItem_1()
{
	int nRetCode = false;

	nRetCode = m_dropDownList->SelectItem(1);
	CPPUNIT_ASSERT(nRetCode == -1);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :3.2 
//@用例目的 :验证能够在正常的情况下设置当前选择项
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置当前选择项
//@后置场景 :
//@CP1      :可以设置当前选择项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SelectItem_2()
{
    int i					  = 0;
    int nRetCode			  = false;
    const char *cszitemname[] = {"item1", "item2", "item3"};

    for (i = 0; i < 3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszitemname[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
    nRetCode = m_dropDownList->SelectItem(1);
    CPPUNIT_ASSERT(nRetCode == 1);
    nRetCode = m_dropDownList->GetSelectedItemIndex();
    CPPUNIT_ASSERT(nRetCode == 1);
}



//////////////////////////////////////////////////////////////////////////
//@用例编号 :3.3
//@用例目的 :验证不能设置当前选择项为不存在列表项
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置当前选择项为不存在列表项
//@后置场景 :
//@CP1      :不能设置当前选择项为不存在列表项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SelectItem_3()
{
    int i					  = 0;
    int nRetCode			  = false;
    const char *cszitemname[] = {"item1", "item2", "item3"};

    for (i = 0; i< 3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszitemname[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
    nRetCode = m_dropDownList->SelectItem(5);
    CPPUNIT_ASSERT(nRetCode == -1);
	nRetCode = m_dropDownList->SelectItem(-1);
	CPPUNIT_ASSERT(nRetCode == -1);
	nRetCode = m_dropDownList->SelectItem(-10);
	CPPUNIT_ASSERT(nRetCode == -1);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :3.4
//@用例目的 :验证不能设置已经设置过的项
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置位置和上次的重复
//@后置场景 :
//@CP1      :可以设置已经设置过的项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SelectItem_4()
{
    int i					  = 0;
    int nRetCode			  = false;
    const char *cszItemName[] = {"item1", "item2", "item3"};

    for (i = 0; i < 3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszItemName[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
    nRetCode = m_dropDownList->SelectItem(1);
    CPPUNIT_ASSERT(nRetCode == 1);
    nRetCode = m_dropDownList->SelectItem(1);
    CPPUNIT_ASSERT(nRetCode == 1);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :4.1 
//@用例目的 :验证不能在列表为空时搜索指定字符串的项
//@前置场景 :
//@C1       :列表为空的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :搜索指定字符串的项
//@后置场景 :
//@CP1      :不能搜索指定字符串的项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SearchItem_1()
{
	int nRetCode                    = false;
	const char cszsearch[]			= "item2";	

	nRetCode = m_dropDownList->SearchItem(cszsearch, 0);
	CPPUNIT_ASSERT(nRetCode == -1);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :4.2 
//@用例目的 :验证能够在正常的情况下搜索指定字符串的项
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :搜索指定字符串的项
//@后置场景 :
//@CP1      :可以搜索指定字符串的项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SearchItem_2()
{
    int i                     = 0;
    int nRetCode              = false;
    char cszout[10]           = {0};
    const char *cszitemname[] = {"item1", "item2", "item3"};	

    for (i = 0; i < 3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszitemname[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
    nRetCode = m_dropDownList->SearchItem(cszitemname[1], 0);
    CPPUNIT_ASSERT(nRetCode == 1);
    nRetCode = m_dropDownList->GetItemText(nRetCode, cszout, sizeof(cszout));
    CPPUNIT_ASSERT(nRetCode);
    CPPUNIT_ASSERT(strcmp(cszout, cszitemname[1]) == 0);
}


//////////////////////////////////////////////////////////////////////////
//@用例编号 :4.3
//@用例目的 :测试不能搜索没有匹配的字符串的项（位置不对，没有该字符串）
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :搜索指定字符串的项
//@后置场景 :
//@CP1      :不能搜索指定字符串的项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SearchItem_3()
{
    int i					   = false;
    int nRetCode               = false;
    char cszout[10]			   = {0};
    const char *cszsearch[]    ={"item1", "item4"};
    const char *cszitemname[ ] ={"item1", "item2", "item3"};	

    for (i = 0; i < 3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszitemname[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
    //位置不对
    nRetCode = m_dropDownList->SearchItem(cszsearch[0], 1);
    CPPUNIT_ASSERT(nRetCode == -1);
    //没有对应的项
    nRetCode = m_dropDownList->SearchItem(cszsearch[1], 0);
    CPPUNIT_ASSERT(nRetCode == -1);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :4.4
//@用例目的 :测试搜索的项不能传入异常字符串
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :搜索异常字符串的项
//@后置场景 :
//@CP1      :不能搜索异常字符串的项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SearchItem_4()
{
    int i                     = 0;
    int nRetCode              = false;
    char cszout[10]           = {0};
    const char *cszsearch[]   = {
                                    NULL, 
                                    "",
                                    " ",
                                    "＋∨∑∏∪∥∞∮",
                                    "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
                                    };
    const char *cszitemname[] = {"item1", "item2", "item3"};	

    for (i = 0; i < 3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszitemname[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
    for (i = 0; i < 5; i++)
    {
        nRetCode = m_dropDownList->SearchItem(cszsearch[i], 0);
        CPPUNIT_ASSERT(nRetCode == -1);
    }
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :4.5
//@用例目的 :测试列表中有字符串前缀重复的项的情况下的搜索
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :搜索指定字符串的项
//@后置场景 :
//@CP1      :可以搜索到唯一匹配的下拉列表项
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SearchItem_5()
{
    int i                     = 0;
    int nRetCode              = false;
    char cszout[300]          = {0};
    const char *cszitemname[] = {"item1", "item2", "item113"};	
   
    for (i = 0; i < 3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszitemname[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
	nRetCode = m_dropDownList->SearchItem(cszitemname[0], 0);
	CPPUNIT_ASSERT(nRetCode == 0);
	nRetCode = m_dropDownList->GetItemText(nRetCode, cszout, sizeof(cszout));
	CPPUNIT_ASSERT(nRetCode);
	CPPUNIT_ASSERT(strcmp(cszout, cszitemname[0]) == 0);

    nRetCode = m_dropDownList->SearchItem(cszitemname[2], 0);
    CPPUNIT_ASSERT(nRetCode == 2);
    nRetCode = m_dropDownList->GetItemText(nRetCode, cszout, sizeof(cszout));
    CPPUNIT_ASSERT(nRetCode);
    CPPUNIT_ASSERT(strcmp(cszout, cszitemname[2]) == 0);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :5.1 
//@用例目的 :验证能够在列表为空的情况下设置字体大小
//@前置场景 :
//@C1       :列表为空的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置字体大小
//@后置场景 :
//@CP1      :可以设置字体大小
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SetFontSize_1()
{
	int nRetCode			  = false;
	int dwSize				  = 0;
	const int cszsize		  = 14;

	m_dropDownList->SetFontSize(cszsize);
	dwSize = m_dropDownList->GetFontSize();
	CPPUNIT_ASSERT_EQUAL(dwSize, cszsize);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :5.2 
//@用例目的 :验证能够在正常的情况下设置字体大小
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置字体大小
//@后置场景 :
//@CP1      :可以设置字体大小
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SetFontSize_2()
{
    int i                     = 0;
    int KWndDropDownLi        = 0;
    int nRetCode              = false;
    int dwSize                = 0;
    const int cszsize         = 12;
    const char *cszitemname[] = {"item1", "item2", "item3"};

    for (i = 0; i<3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszitemname[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
    m_dropDownList->SetFontSize(cszsize);
    dwSize = m_dropDownList->GetFontSize();
    CPPUNIT_ASSERT_EQUAL(dwSize, cszsize);
}


//////////////////////////////////////////////////////////////////////////
//@用例编号 :6.1 
//@用例目的 :验证能够在列表为空情况下设置提示文字颜色
//@前置场景 :
//@C1       :列表为空的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置提示文字颜色
//@后置场景 :
//@CP1      :可以设置提示文字颜色
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SetTipColor_1()
{
	int nRetCode         = false;
	DWORD dwColor        = 0;
	const DWORD cszcolor = 155;   

	m_dropDownList->SetTipColor(cszcolor);
	dwColor = m_dropDownList->GetTipColor();
	CPPUNIT_ASSERT_EQUAL(dwColor, cszcolor);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :6.2 
//@用例目的 :验证能够在正常情况下设置提示文字颜色（不能添加提示）
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置提示文字颜色
//@后置场景 :
//@CP1      :可以设置提示文字颜色
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SetTipColor_2()
{
    int i                     = 0;
    int nRetCode              = false;
    DWORD dwColor             = 0;
    const DWORD cszcolor      = 255;
    const char *cszitemname[] = {"item1", "item2", "item3"};

    for (i = 0; i<3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszitemname[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
    m_dropDownList->SetTipColor(cszcolor);
    dwColor = m_dropDownList->GetTipColor();
    CPPUNIT_ASSERT_EQUAL(dwColor, cszcolor);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :7.1
//@用例目的 :验证能够空列表情况下设置下拉列表背景颜色
//@前置场景 :
//@C1       :列表为空的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置下拉列表背景颜色
//@后置场景 :
//@CP1      :可以设置下拉列表背景颜色
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SetListBGColor_1()
{
	int nRetCode	    	= false;
	DWORD dwColor	    	= 0;
	const DWORD cszcolor    = 123;

	m_dropDownList->SetListBGColor(cszcolor);
	dwColor = m_dropDownList->GetListBGColor();
	CPPUNIT_ASSERT_EQUAL(dwColor, cszcolor);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :7.2 
//@用例目的 :验证能够正常情况下设置下拉列表背景颜色
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置下拉列表背景颜色
//@后置场景 :
//@CP1      :可以设置下拉列表背景颜色
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SetListBGColor_2()
{
    int i					  = 0;
    int nRetCode			  = false;
    DWORD dwColor			  = 0;
    const DWORD cszcolor	  = 451;
    const char *cszitemname[] = {"item1", "item2", "item3"};

    for (i = 0; i<3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszitemname[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
    m_dropDownList->SetListBGColor(cszcolor);
    dwColor = m_dropDownList->GetListBGColor();
    CPPUNIT_ASSERT_EQUAL(dwColor, cszcolor);
}


//////////////////////////////////////////////////////////////////////////
//@用例编号 :8.1
//@用例目的 :验证能够在列表为空的情况下设置分隔线颜色
//@前置场景 :
//@C1       :列表为空的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置分隔线颜色
//@后置场景 :
//@CP1      :可以设置分隔线颜色
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SetListSLColor_1()
{
	int nRetCode		 = false;
	DWORD dwColor		 = 0;
	const DWORD cszcolor = 233;

	m_dropDownList->SetListSLColor(cszcolor);
	dwColor = m_dropDownList->GetListSLColor();
	CPPUNIT_ASSERT(dwColor == cszcolor);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :8.2 
//@用例目的 :验证能够在有列表项和分隔线的情况下设置分隔线颜色
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置分隔线颜色
//@后置场景 :
//@CP1      :可以设置分隔线颜色
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SetListSLColor_2()
{	
	int nRetCode			  = false;
	int i                     = 0;
	DWORD dwColor		      = 0;
	const DWORD cszcolor	  = 233;
	const char *cszitemname[] = {"item1", "item2", "item3"};

	for (i = 0; i<3; i++)
	{
		nRetCode = m_dropDownList->AddItem(cszitemname[i]);
		CPPUNIT_ASSERT(nRetCode);
	}
	m_dropDownList->SetListSLColor(cszcolor);
	dwColor = m_dropDownList->GetListSLColor();
	CPPUNIT_ASSERT(dwColor == cszcolor);
}


//////////////////////////////////////////////////////////////////////////
//@用例编号 :9.1
//@用例目的 :验证能够在列表为空情况下设置下拉列表边框颜色
//@前置场景 :
//@C1       :列表为空的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置下拉列表边框颜色
//@后置场景 :
//@CP1      :可以设置下拉列表边框颜色
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SetListFLColor_1()
{
	int nRetCode         = false;
	DWORD dwColor        = 0;
	const DWORD cszcolor =125;

	m_dropDownList->SetListFLColor(cszcolor);
	dwColor = m_dropDownList->GetListFLColor();
	CPPUNIT_ASSERT(dwColor == cszcolor);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :9.2 
//@用例目的 :验证能够在列表正常情况下设置下拉列表边框颜色
//@前置场景 :
//@C1       :列表正常的Dropdownlist
//@操作源   :Dropdownlist
//@操作对象 :
//@操作过程 :
//@OP1      :设置下拉列表边框颜色
//@后置场景 :
//@CP1      :可以设置下拉列表边框颜色
//////////////////////////////////////////////////////////////////////////
void WndDropDownList_Test::Test_SetListFLColor_2()
{
    int i                     = 0;
    int nRetCode              = false;
    DWORD dwColor             = 0;
    const DWORD cszcolor      = 255;
    const char *cszitemname[] = {"item1", "item2", "item3"};

    for(i = 0; i<3; i++)
    {
        nRetCode = m_dropDownList->AddItem(cszitemname[i]);
        CPPUNIT_ASSERT(nRetCode);
    }
    m_dropDownList->SetListFLColor(cszcolor);
    dwColor = m_dropDownList->GetListFLColor();
    CPPUNIT_ASSERT(dwColor == cszcolor);
}





//void WndDropDownList_Test::DummyClassForFirstCopy(void* pvClassBeingTest)
//{
//    _testdata.m_bEditable     = ((UI::KWndDropDownList*)pvClassBeingTest)->m_bEditable;
//    _testdata.m_bEnableTip    = ((UI::KWndDropDownList*)pvClassBeingTest)->m_bEnableTip;
//    _testdata.m_bListExtend   = ((UI::KWndDropDownList*)pvClassBeingTest)->m_bListExtend;
//    _testdata.m_dwListBGColor = ((UI::KWndDropDownList*)pvClassBeingTest)->m_dwListBGColor;
//    _testdata.m_dwListFLColor = ((UI::KWndDropDownList*)pvClassBeingTest)->m_dwListFLColor;
//    _testdata.m_dwListSLColor = ((UI::KWndDropDownList*)pvClassBeingTest)->m_dwListSLColor;
//    _testdata.m_dwTipColor    = ((UI::KWndDropDownList*)pvClassBeingTest)->m_dwTipColor;
//    _testdata.m_nFontSize     = ((UI::KWndDropDownList*)pvClassBeingTest)->m_nFontSize;
//    _testdata.m_nMaxItemNum   = ((UI::KWndDropDownList*)pvClassBeingTest)->m_nMaxItemNum;
//    //_testdata.m_szTipInfo     = ((UI::KWndDropDownList*)pvClassBeingTest)->m_szTipInfo;
//    SetFirstCopy( &_testdata, sizeof( _TestData ));
//}
//void WndDropDownList_Test::DummyClassForSecondCopy(void* pvClassBeingTest)
//{
//    _testdata.m_bEditable     = ((UI::KWndDropDownList*)pvClassBeingTest)->m_bEditable;
//    _testdata.m_bEnableTip    = ((UI::KWndDropDownList*)pvClassBeingTest)->m_bEnableTip;
//    _testdata.m_bListExtend   = ((UI::KWndDropDownList*)pvClassBeingTest)->m_bListExtend;
//    _testdata.m_dwListBGColor = ((UI::KWndDropDownList*)pvClassBeingTest)->m_dwListBGColor;
//    _testdata.m_dwListFLColor = ((UI::KWndDropDownList*)pvClassBeingTest)->m_dwListFLColor;
//    _testdata.m_dwListSLColor = ((UI::KWndDropDownList*)pvClassBeingTest)->m_dwListSLColor;
//    _testdata.m_dwTipColor    = ((UI::KWndDropDownList*)pvClassBeingTest)->m_dwTipColor;
//    _testdata.m_nFontSize     = ((UI::KWndDropDownList*)pvClassBeingTest)->m_nFontSize;
//    _testdata.m_nMaxItemNum   = ((UI::KWndDropDownList*)pvClassBeingTest)->m_nMaxItemNum;
//    //_testdata.m_szTipInfo     = ((UI::KWndDropDownList*)pvClassBeingTest)->m_szTipInfo;
//    SetSecondCopy( &_testdata, sizeof( _TestData ));
//}
//void WndDropDownList_Test::DummyClassToFile( LPCSTR szFilename)
//{
//}
//void WndDropDownList_Test::CompareCopy(bool bAutoReleaseCopy)
//{
//    if( NULL != _FirstCopy && NULL != _SecondCopy)
//    {
//        _TestData* pFirstTestdata  = (_TestData*) _FirstCopy;
//        _TestData* pSecondTestData = (_TestData*) _SecondCopy;
//
//        CPPUNIT_ASSERT(pFirstTestdata->m_bEditable     == pSecondTestData->m_bEditable);
//        CPPUNIT_ASSERT(pFirstTestdata->m_bEnableTip    == pSecondTestData->m_bEnableTip);
//        CPPUNIT_ASSERT(pFirstTestdata->m_bListExtend   == pSecondTestData->m_bListExtend);
//        CPPUNIT_ASSERT(pFirstTestdata->m_dwListBGColor == pSecondTestData->m_dwListBGColor);
//        CPPUNIT_ASSERT(pFirstTestdata->m_dwListFLColor == pSecondTestData->m_dwListFLColor);
//        CPPUNIT_ASSERT(pFirstTestdata->m_dwListSLColor == pSecondTestData->m_dwListSLColor);
//        CPPUNIT_ASSERT(pFirstTestdata->m_dwTipColor    == pSecondTestData->m_dwTipColor);
//        CPPUNIT_ASSERT(pFirstTestdata->m_nFontSize     == pSecondTestData->m_nFontSize);
//        CPPUNIT_ASSERT(pFirstTestdata->m_nMaxItemNum   == pSecondTestData->m_nMaxItemNum);
//        CPPUNIT_ASSERT(pFirstTestdata->m_szTipInfo     == pSecondTestData->m_szTipInfo);
//        if( bAutoReleaseCopy)
//            ReleaseAllCopy();
//    }
//}
#endif //End of #ifdef _UNIT_TEST