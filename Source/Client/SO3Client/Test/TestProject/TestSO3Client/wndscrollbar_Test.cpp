/***************************************************
Filename:wndscrollbar_Test.cpp
CreatedBy: Yan Chunwa
CreatedOn: 2005年8月30日 10:20:03.0083581
Test Requirement Reference by Wiki path:
http://sword-test/qcweb/pmwiki.php?n=XSJQC-DEV.JX3-QC-REQ-SO3Client_UI_Scrollbar
Comment:
**************************************************/
#include "stdafx.h"

#ifdef _UNIT_TEST

#include "wndscrollbar_Test.h"
#include "../ui/elem/wndscrollbar.h"
#include "../ui/elem/wndbutton.h"
#include "../ui/elem/wndimage.h"

using namespace std;
#include "../../DevEnv/Include/Lua/llimits.h"

CPPUNIT_TEST_SUITE_REGISTRATION(KWndScrollBar_Test);

UI::KWndButton* btnNext;
UI::KWndButton* btnPage;
UI::KWndButton* btnSlide;
UI::KWndScrollBar* scrollbar;

void KWndScrollBar_Test::setUp()
{  
	scrollbar = new UI::KWndScrollBar;
	btnNext = new UI::KWndButton;
	btnPage = new UI::KWndButton;
	btnSlide = new UI::KWndButton;
	scrollbar->BindNextPageButton(btnNext);
	scrollbar->BindPrePageButton(btnPage);
	scrollbar->BindSlideButton(btnSlide);
}

void KWndScrollBar_Test::tearDown()
{
	delete scrollbar;
	delete btnNext;
	delete btnPage;
	delete btnSlide;
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :验证SetScrollPos,GetScrollPos的正确性
//@前置场景 :要先设置好位置取值范围SetValueRange
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :分别传入-1，0，1，200，MAX_INT
//@后置场景 :
//@CP1      :检查获得的当前位置值是否与预置相同
//@CP2      :
//////////////////////////////////////////////////////////////////////////

void KWndScrollBar_Test::SetScrollPostest()
{
	int nRetCode = false;
	int i = 0;
	int j = 0;
	const pos[5] = {-1, 0, 1, 200, MAX_INT};
	
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == 0);

	//未经初始化位置值范围
	for (i = 0; i < 5; i++)
	{
		scrollbar->SetScrollPos(pos[i]);
		nRetCode = scrollbar->GetScrollPos();
		CPPUNIT_ASSERT(nRetCode == MinPos);
	}
	//已经初始化位置值范围
	scrollbar->SetValueRange(MinPos, MaxPos);
	nRetCode = scrollbar->GetMaxValue();
	CPPUNIT_ASSERT(nRetCode == MaxPos);
	nRetCode = scrollbar->GetMinValue();
	CPPUNIT_ASSERT(nRetCode == MinPos);
		

	for (i = 0; i < 5; i++)
	{
		scrollbar->SetScrollPos(pos[i]);
		nRetCode = scrollbar->GetScrollPos();
		switch(i) 
		{
		case 0:
			CPPUNIT_ASSERT(nRetCode == MinPos);
			break;
		case 3:
			CPPUNIT_ASSERT(nRetCode == MaxPos - 1);
			break;
		case 4:
			CPPUNIT_ASSERT(nRetCode == MaxPos - 1);
			break;
		default:
			CPPUNIT_ASSERT(nRetCode == pos[i]);
		}//switch
		
	}//for i
	
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :2
//@用例目的 :验证ScrollLine的正确性
//@前置场景 :要先设置好位置取值范围SetValueRange
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :传入0，1，NULL并检查结果
//@OP2      :在特殊位置首端/末端，向后/向前滚动
//@后置场景 :
//@CP1      :检查获得的当前位置值是否与预计值相同
//@CP2      :滚动不成功
//////////////////////////////////////////////////////////////////////////
void KWndScrollBar_Test::ScrollLinetest()
{
	int nRetCode = false;
	int i = 0;
	int curPos = 10;
	bool bpre[3] = {0, 1, NULL};

	scrollbar->SetValueRange(MinPos, MaxPos);
	nRetCode = scrollbar->GetMaxValue();
	CPPUNIT_ASSERT(nRetCode == MaxPos);
	nRetCode = scrollbar->GetMinValue();
	CPPUNIT_ASSERT(nRetCode == MinPos);
	scrollbar->SetScrollPos(curPos);
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == curPos);
    //向后滚
    nRetCode = scrollbar->ScrollLine(bpre[0]);
	CPPUNIT_ASSERT(nRetCode == curPos + 1);
	curPos = nRetCode;
	//向前滚
	nRetCode = scrollbar->ScrollLine(bpre[1]);
	CPPUNIT_ASSERT(nRetCode == curPos - 1);
	curPos = nRetCode;
	//错误参数
	nRetCode = scrollbar->ScrollLine(bpre[2]);
	//CPPUNIT_ASSERT(!nRetCode);

	//当前位置为第一个，不能向前滚
	scrollbar->SetScrollPos(MinPos);
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == MinPos);
	nRetCode = scrollbar->ScrollLine(bpre[1]);
	CPPUNIT_ASSERT(nRetCode == MinPos);
	//当前位置为最后一个，不能向后滚
	scrollbar->SetScrollPos(MaxPos);
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == MaxPos - 1);
	nRetCode = scrollbar->ScrollLine(bpre[0]);
	CPPUNIT_ASSERT(nRetCode == MaxPos - 1);
	
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :3
//@用例目的 :验证ScrollPage的正确性
//@前置场景 :要先设置好位置取值范围SetValueRange,设置好页面大小
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :传入0，1，NULL并检查结果  
//@OP2      :在特殊位置首端(0,2,10)/末端(190,198,200)，向后/向前翻页
//@后置场景 :
//@CP1      :检查获得的当前位置值是否与预计值相同
//@CP2      :翻页不成功
//////////////////////////////////////////////////////////////////////////
void KWndScrollBar_Test::ScrollPagetest()
{
	int nRetCode = false;
	int curPos = 25;
	int PageSize = 10;
	bool bpre[3] = {0, 1, NULL};

	scrollbar->SetValueRange(MinPos, MaxPos);
	nRetCode = scrollbar->GetMaxValue();
	CPPUNIT_ASSERT(nRetCode == MaxPos);
	nRetCode = scrollbar->GetMinValue();
	CPPUNIT_ASSERT(nRetCode == MinPos);

	scrollbar->SetPageSize(PageSize);
	nRetCode = scrollbar->GetPageSize();
	CPPUNIT_ASSERT(nRetCode == PageSize);

	scrollbar->SetScrollPos(curPos);
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == curPos);

	//向后翻页
    nRetCode = scrollbar->ScrollPage(bpre[0]);
	CPPUNIT_ASSERT(nRetCode == curPos + PageSize);
	curPos = nRetCode;
	//向前翻页
	nRetCode = scrollbar->ScrollPage(bpre[1]);
	CPPUNIT_ASSERT(nRetCode == curPos - PageSize);
	curPos = nRetCode;
	//错误参数
	nRetCode = scrollbar->ScrollPage(bpre[2]);
	//CPPUNIT_ASSERT(!nRetCode);

	//当前页为第一页，不可向前翻页
	scrollbar->SetScrollPos(MinPos);
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == MinPos);
	nRetCode = scrollbar->ScrollPage(bpre[1]);
	CPPUNIT_ASSERT(nRetCode == MinPos);

	scrollbar->SetScrollPos(2);
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == 2);
	nRetCode = scrollbar->ScrollPage(bpre[1]);
	CPPUNIT_ASSERT(nRetCode == MinPos);

	scrollbar->SetScrollPos(10);
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == 10);
	nRetCode = scrollbar->ScrollPage(bpre[1]);
	CPPUNIT_ASSERT(nRetCode == MinPos);

	//当前页为最后一页，不可向后翻页
	scrollbar->SetScrollPos(MaxPos - 1);
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == MaxPos - 1);
	nRetCode = scrollbar->ScrollPage(bpre[0]);
	CPPUNIT_ASSERT(nRetCode == MaxPos - 1);

	scrollbar->SetScrollPos(198);
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == 198);
	nRetCode = scrollbar->ScrollPage(bpre[0]);
	CPPUNIT_ASSERT(nRetCode == MaxPos - 1);

	scrollbar->SetScrollPos(190);
	nRetCode = scrollbar->GetScrollPos();
	CPPUNIT_ASSERT(nRetCode == 190);
	nRetCode = scrollbar->ScrollPage(bpre[0]);
	CPPUNIT_ASSERT(nRetCode == MaxPos - 1);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :4
//@用例目的 :验证SetValueRange,GetMinValue,GetMaxValue的正确性
//@前置场景 :
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :传入（3，2），（1，1），（-4，-1），(-4, 1)（1，100）并检查结果 
//@OP2      :
//@后置场景 :
//@CP1      :检查位置最大值，最小值
//@CP2      :
//////////////////////////////////////////////////////////////////////////
void KWndScrollBar_Test::SetValueRangetest()
{
	int nRetCode = false;
	int minVal = 0;
	int maxVal = 0;
    
	minVal = scrollbar->m_nMinValue;
	maxVal = scrollbar->m_nMaxValue;
	//最小值 > 最大值 
	scrollbar->SetValueRange(3, 2);
	nRetCode = scrollbar->GetMinValue();
	CPPUNIT_ASSERT(nRetCode == minVal);
	nRetCode = scrollbar->GetMaxValue();
	CPPUNIT_ASSERT(nRetCode == maxVal);
	//最小值 ＝ 最大值
	scrollbar->SetValueRange(1, 1);
	nRetCode = scrollbar->GetMinValue();
	CPPUNIT_ASSERT(nRetCode == 1);
	minVal = nRetCode;
	nRetCode = scrollbar->GetMaxValue();
	CPPUNIT_ASSERT(nRetCode == 1);
	maxVal = nRetCode;
	//最小值 < 最大值，不过皆为负数
	scrollbar->SetValueRange(-4, -1);
	nRetCode = scrollbar->GetMinValue();
	CPPUNIT_ASSERT(nRetCode == -4);
	minVal = nRetCode;
	nRetCode = scrollbar->GetMaxValue();
	CPPUNIT_ASSERT(nRetCode == -1);
	maxVal = nRetCode;
	//最小值 < 最大值，最大值为负数
	scrollbar->SetValueRange(-4, 1);
	nRetCode = scrollbar->GetMinValue();
	CPPUNIT_ASSERT(nRetCode == -4);
	minVal = nRetCode;
	nRetCode = scrollbar->GetMaxValue();
	CPPUNIT_ASSERT(nRetCode == 1);
	maxVal = nRetCode;
	//正常情况
	scrollbar->SetValueRange(1, 100);
	nRetCode = scrollbar->GetMinValue();
	CPPUNIT_ASSERT(nRetCode == 1);
	nRetCode = scrollbar->GetMaxValue();
	CPPUNIT_ASSERT(nRetCode == 100);

}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :5
//@用例目的 :验证SetPageSize,GetPageSize的正确性
//@前置场景 :
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :传入-1，0，1，50，MAX_INT 
//@OP2      :
//@后置场景 :
//@CP1      :检查获得的当前页面大小是否与预计值相同
//@CP2      :
//////////////////////////////////////////////////////////////////////////
void KWndScrollBar_Test::SetPageSizetest()
{
	int nRetCode = false;
	int i = 0;
	const int nSize[5] = {-1, 0, 1, 50, MAX_INT};

	for(i = 0; i < 5; i++)
	{
		nRetCode = scrollbar->m_nPageSize;
		scrollbar->SetPageSize(nSize[i]);		
		if(i < 2)
			CPPUNIT_ASSERT(nRetCode == scrollbar->GetPageSize());
		else
		{
		CPPUNIT_ASSERT(nRetCode != scrollbar->GetPageSize() && scrollbar->GetPageSize() == nSize[i]);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :6
//@用例目的 :验证SetTotalSize的正确性
//@前置场景 :
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :传入-1，0，1，50，MAX_INT 
//@OP2      :
//@后置场景 :
//@CP1      :检查获得的当前位置值是否与设置值相同
//@CP2      :
//////////////////////////////////////////////////////////////////////////
void KWndScrollBar_Test::SetTotalSizetest()
{
	int nRetCode = false;
	int i = 0;
	const int nSize[5] = {-1, 0, 1, 50, MAX_INT};

	for(i = 0; i < 5; i++)
	{
		nRetCode = scrollbar->m_nTotalSize;
		scrollbar->SetTotalSize(nSize[i]);
		
		if(i < 2)
			CPPUNIT_ASSERT(nRetCode == scrollbar->m_nTotalSize);
		else
			CPPUNIT_ASSERT(nRetCode != scrollbar->m_nTotalSize && scrollbar->m_nTotalSize == nSize[i]);
	}
	
	
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :7
//@用例目的 :验证SetSize的正确性
//@前置场景 :滚动条为水平型的
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :传入（-1，-8),（0，0),(0, 50),(50, 0)（50，100),(MAX_INT，MAX_INT）
//@OP2      :
//@后置场景 :
//@CP1      :检查获得的当前窗口大小是否与设置值相同
//@CP2      :
//////////////////////////////////////////////////////////////////////////
void KWndScrollBar_Test::SetSizetest()
{
	int nRetCode = false;
	float pWidth1 = 0;
	float pHeight1 = 0;
	float pWidth2 = 0;
	float pHeight2 = 0;

    
	//(-1, -8)	
	scrollbar->GetSize(&pWidth1, &pHeight1);
	scrollbar->SetSize(-1, -8);  
	scrollbar->GetSize(&pWidth2, &pHeight2);
	CPPUNIT_ASSERT(pWidth2 == 0 && pHeight2 == 5);
	//(0, 0)
	scrollbar->GetSize(&pWidth1, &pHeight1);
	scrollbar->SetSize(0, 0);  
	scrollbar->GetSize(&pWidth2, &pHeight2);
	CPPUNIT_ASSERT(pWidth2 == pWidth1 && pHeight2 == pHeight1);
	//(0, 50)
	scrollbar->GetSize(&pWidth1, &pHeight1);
	scrollbar->SetSize(0, 50);  
	scrollbar->GetSize(&pWidth2, &pHeight2);
	CPPUNIT_ASSERT(pWidth2 == pWidth1 && pHeight2 == 50);
	//(50, 0)
	scrollbar->GetSize(&pWidth1, &pHeight1);
	scrollbar->SetSize(50, 0);  
	scrollbar->GetSize(&pWidth2, &pHeight2);
	CPPUNIT_ASSERT(pWidth2 == 50 && pHeight2 == 5);
	//(50, 100)
	//scrollbar->GetSize(pWidth1, pHeight1);
	scrollbar->SetSize(50, 100);  
	scrollbar->GetSize(&pWidth2, &pHeight2);
	CPPUNIT_ASSERT(pWidth2 == 50 && pHeight2 == 100);
	//（MAX_INT，MAX_INT）
	scrollbar->SetSize(MAX_INT, MAX_INT);  
	scrollbar->GetSize(&pWidth2, &pHeight2);
	CPPUNIT_ASSERT(pWidth2 == MAX_INT && pHeight2 == MAX_INT);
}

#endif


