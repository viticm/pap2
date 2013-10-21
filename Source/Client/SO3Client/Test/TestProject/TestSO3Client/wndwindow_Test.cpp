

#include "stdafx.h"

#ifdef _UNIT_TEST

#include <vector>
#include <string>
#include "wndwindow_Test.h"
#include "../ui/elem/wndwindow.h"

using namespace std;
CPPUNIT_TEST_SUITE_REGISTRATION(KWndWindow_Test);


KWndWindow_Test::KWndWindow_Test()
{

}

KWndWindow_Test::~KWndWindow_Test()
{

}


void KWndWindow_Test::setUp()
{


}

void KWndWindow_Test::tearDown()
{

}


//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :增加子窗口

//@前置场景 :m_KWndWindow与任何窗口都无关联, 4个指针都为空指针	
//@C1       : 
//@C2       :
//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :增加子窗口, 双向链表
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      :传入空指针
//@CP2      :依次增加100个与任何窗口无关联的子窗口
//////////////////////////////////////////////////////////////////////////

void KWndWindow_Test::AddChild_Test(void)
{
	//传入NULL
	KWndWindow m_KWndWindow; //
	m_KWndWindow.AddChild(NULL);
	//SWndWindow_Data *s_data1 = reinterpret_cast<SWndWindow_Data *>
				//(CopyClassData(reinterpret_cast<void *>(&m_KWndWindow), sizeof(KWndWindow)));
	//没有子窗口 返回值是NULL
	CPPUNIT_ASSERT(m_KWndWindow.GetFirstChild() == NULL);

	//增加100个子窗口
	//传入100个有效的KWndWindow指针
	KWndWindow window[100];
	for (int i=0; i<100; i++)
	{
		m_KWndWindow.AddChild(&window[i]);	
	}

	//SWndWindow_Data *s_data2 = reinterpret_cast<SWndWindow_Data *>
				//(CopyClassData(reinterpret_cast<void *>(&m_KWndWindow), sizeof(KWndWindow)));

	CPPUNIT_ASSERT(m_KWndWindow.GetFirstChild() == &window[0]);
	KWndWindow *ChildWindow = m_KWndWindow.GetFirstChild();
	
	//for (int i=1; i<100; i++)
	//{
	//	CPPUNIT_ASSERT(ChildWindow->m_pNextWnd == &window[i]);
	//	ChildWindow = (SWndWindow_Data *)ChildWindow->m_pNextWnd;
	//}
	//CPPUNIT_ASSERT(ChildWindow->m_pNextWnd == NULL);

	int j = 1;
	ChildWindow = m_KWndWindow.GetFirstChild();
	while (ChildWindow->GetNextWnd() != NULL)
	{
		//检查增加的100个子窗口顺序是否正确
		//每个新增加的子窗口在双向链表 表尾
		CPPUNIT_ASSERT(ChildWindow->GetNextWnd() == &window[j]);  
		//检查每个新增加的子窗口的父窗口
		CPPUNIT_ASSERT(ChildWindow->GetParent() == &m_KWndWindow); 
		ChildWindow = ChildWindow->GetNextWnd();
		j++;
	}

	//检查是否100个子窗口
	CPPUNIT_ASSERT(j == 100);  
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :增加兄弟窗口

//@前置场景 :m_KWndWindow与任何窗口都无关联, 4个指针都为空指针	
//@C1       : 
//@C2       :
//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :增加兄弟窗口, 双向链表
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      :传入空指针
//@CP2      :无父窗口 依次增加100个与任何窗口无关联的兄弟窗口
//@CP3      :有父窗口 依次增加100个兄弟窗口(与其他窗口有联系)
//////////////////////////////////////////////////////////////////////////
void KWndWindow_Test::AddBrother_Test(void)
{
	//传入NULL
	KWndWindow m_KWndWindow;
	m_KWndWindow.AddBrother(NULL);
	CPPUNIT_ASSERT(m_KWndWindow.GetNextWnd() == NULL);


	//无父窗口 传入100个有效的KWndWindow指针
	KWndWindow window[100];
	for (int i=0; i<100; i++)
	{
		m_KWndWindow.AddBrother(&window[i]);
	}

	//s_data1 = reinterpret_cast<SWndWindow_Data *>
				//(CopyClassData(reinterpret_cast<void *>(&m_KWndWindow), sizeof(KWndWindow)));
	int j = 0;
	KWndWindow *BroWindow = &m_KWndWindow;
	while (BroWindow->GetNextWnd() != NULL)
	{
		//每个新增加的兄弟窗口在双向链表 表尾
		//检查增加的100个兄弟窗口顺序是否正确 新增加的窗口在表尾吗
		CPPUNIT_ASSERT(BroWindow->GetNextWnd() == &window[j]);  
		BroWindow = BroWindow->GetNextWnd();
		j++;
	}
	CPPUNIT_ASSERT(j == 100);  //是否为100个兄弟窗口
	

	//有父窗口 依次增加100个兄弟窗口(这些窗口都有兄弟窗口)
	KWndWindow MyParent;
	KWndWindow MyChild;
	MyParent.AddChild(&MyChild);
	for (int i=0; i<100; i++)
	{
		MyChild.AddBrother(&window[i]);
	}	
	//s_data1 = reinterpret_cast<SWndWindow_Data *>
				//(CopyClassData(reinterpret_cast<void *>(&MyChild), sizeof(KWndWindow)));	
	j = 0;
	BroWindow = &MyChild;
	//第一个子窗口的父窗口是正确的吗
	CPPUNIT_ASSERT(BroWindow->GetParent() == &MyParent);  
	while (BroWindow->GetNextWnd() != NULL)
	{
		//每个新增加的兄弟窗口在双向链表 表尾
		//检查增加的100个兄弟窗口顺序是否正确 新增加的窗口在表尾吗
		CPPUNIT_ASSERT(BroWindow->GetNextWnd() == &window[j]);  
		CPPUNIT_ASSERT(BroWindow->GetParent() == &MyParent);  
		BroWindow = BroWindow->GetNextWnd();
		j++;
	}
	CPPUNIT_ASSERT(j == 100);  //是否为100个兄弟窗口

}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :把窗口移动到最前面
//@前置场景 : 
//@C1        :与其他窗口无任何联系
//@C2        :给m_KWndWindow增加100个兄弟窗口,
//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      :无任何兄弟窗口 把m_KWndWindow窗口移动到最前面
//@CP2      :100个兄弟窗口 把m_KWndWindow窗口移动到最前面
//@CP3      :100个兄弟窗口 把Window[1]窗口移动到最前面
//@CP4      :100个兄弟窗口 把Window[50]窗口移动到最前面
//@CP5      :100个兄弟窗口 把Window[99]窗口移动到最前面
//////////////////////////////////////////////////////////////////////////

void KWndWindow_Test::BringToTop_Test()
{
	//无任何兄弟窗口 把m_KWndWindow把窗口移动到最前面
	KWndWindow m_KWndWindow;
	m_KWndWindow.BringToTop();
	KWndWindow *BroWindow = &m_KWndWindow;
	while (BroWindow->GetNextWnd() != NULL)
	{
		BroWindow = BroWindow->GetNextWnd();
	}
	//把m_KWndWindow把窗口移动到最前面
	CPPUNIT_ASSERT(BroWindow == &m_KWndWindow);  


	//增加100个兄弟窗口
	KWndWindow window[100];
	for (int i=0; i<100; i++)
	{
		m_KWndWindow.AddBrother(&window[i]);
	}
	
	//SWndWindow_Data *s_data1 = reinterpret_cast<SWndWindow_Data *>
				//(CopyClassData(reinterpret_cast<void *>(&m_KWndWindow), sizeof(KWndWindow)));
	//把m_KWndWindow把窗口移动到最前面
	m_KWndWindow.BringToTop();
	BroWindow = &m_KWndWindow;
	while (BroWindow->GetNextWnd() != NULL)
	{
		BroWindow = BroWindow->GetNextWnd();
	}
	//最前面的窗口是m_KWndWindow吗
	CPPUNIT_ASSERT(BroWindow == &m_KWndWindow);  

	//s_data1 = reinterpret_cast<SWndWindow_Data *>
				//(CopyClassData(reinterpret_cast<void *>(&window[1]), sizeof(KWndWindow)));
	//把window[1]把窗口移动到最前面
	window[1].BringToTop();   
	BroWindow = &window[1];
	while (BroWindow->GetNextWnd() != NULL)
	{
		BroWindow = BroWindow->GetNextWnd();
	}
	//最前面的窗口是window[1]吗
	CPPUNIT_ASSERT(BroWindow == &window[1]);   


	//s_data1 = reinterpret_cast<SWndWindow_Data *>
			//(CopyClassData(reinterpret_cast<void *>(&window[50]), sizeof(KWndWindow)));
	//把window[50]把窗口移动到最前面
	window[50].BringToTop();  
	BroWindow = &window[50];
	while (BroWindow->GetNextWnd() != NULL)
	{
		BroWindow = BroWindow->GetNextWnd();
	}
	//最前面的窗口是window[50]吗
	CPPUNIT_ASSERT(BroWindow == &window[50]);  

	//s_data1 = reinterpret_cast<SWndWindow_Data *>
			//(CopyClassData(reinterpret_cast<void *>(&window[99]), sizeof(KWndWindow)));

	//把window[99]把窗口移动到最前面
	window[99].BringToTop();  
	BroWindow = &window[99];
	while (BroWindow->GetNextWnd()!= NULL)
	{
		BroWindow = BroWindow->GetNextWnd();
	}
	//最前面的窗口是window[99]吗
	CPPUNIT_ASSERT(BroWindow == &window[99]);  
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :设置窗口位置（左上角相对于父窗口左上角位置的位移）
//@前置场景 :
//@C1       : 
//@C2       :
//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      :设置窗口位置(对于父窗口0, 0)
//@CP2      :设置窗口位置(对于父窗口10, 10)
//@CP3      :设置窗口位置(对于父窗口1024, 1024)
//////////////////////////////////////////////////////////////////////////
void KWndWindow_Test::SetPosition_Test(void)
{
	KWndWindow m_KWndWindow[3];
	
	//SWndWindow_Data *s_data1 = reinterpret_cast<SWndWindow_Data *>
				//(CopyClassData(reinterpret_cast<void *>(&m_KWndWindow[0]), sizeof(KWndWindow)));
	float left = -1, top = -1;
	m_KWndWindow[0].SetPosition(0, 0);
	m_KWndWindow[0].GetPosition(&left, &top);
	CPPUNIT_ASSERT(left == 0.0); 
	CPPUNIT_ASSERT(top == 0.0); 

	m_KWndWindow[1].SetPosition(10, 10);
	//s_data1 = reinterpret_cast<SWndWindow_Data *>
				//(CopyClassData(reinterpret_cast<void *>(&m_KWndWindow[1]), sizeof(KWndWindow)));
	m_KWndWindow[1].GetPosition(&left, &top);
	CPPUNIT_ASSERT(left == 10.0); 
	CPPUNIT_ASSERT(top == 10.0); 

	
	//s_data1 = reinterpret_cast<SWndWindow_Data *>
				//(CopyClassData(reinterpret_cast<void *>(&m_KWndWindow[2]), sizeof(KWndWindow)));
	m_KWndWindow[2].SetPosition(1024, 1024);
	m_KWndWindow[2].GetPosition(&left, &top);
	CPPUNIT_ASSERT(left == 1024.0); 
	CPPUNIT_ASSERT(top == 1024.0); 
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :获取窗口位置 相对坐标
//@前置场景 :
//@C1       : 设置窗口位置(对于父窗口-10, -10)
//@C2       : 设置窗口位置(对于父窗口0, 0)
//@C3       : 设置窗口位置(对于父窗口10, 10)
//@C4       : 设置窗口位置(对于父窗口1024, 1024)
//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      : 通过输出参数 检查窗口位置(对于父窗口-10, -10)
//@CP2      : 通过输出参数 检查设置窗口位置(对于父窗口0, 0)
//@CP3      : 通过输出参数 检查设置窗口位置(对于父窗口10, 10)
//@CP4      : 通过输出参数 检查设置窗口位置(对于父窗口1024, 1024)
//////////////////////////////////////////////////////////////////////////
void KWndWindow_Test::GetPosition_Test(void)
{
	//如果GetPosition中有一个参数是NULL  则只有一个值是正确的 应该用
	//返回值来判断
	KWndWindow m_KWndWindow[4];
	float  point[4] = {-10, 0, 10, 1024};

	for (int i=0; i<4; i++)
	{
		m_KWndWindow[i].SetPosition(point[i], point[i]);	
	}

	float left, top;
	for (int i=0; i<4; i++)
	{
		m_KWndWindow[i].GetPosition(&left, &top);
		CPPUNIT_ASSERT(left == point[i]); 
		CPPUNIT_ASSERT(top  == point[i]); 	
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :设置窗口大小
//@前置场景 :有子窗口和无子窗口
//@C1       : 有子窗口 设置窗口大小(10, 0)
//@C2       : 有子窗口 设置窗口大小(1024, 1024)
//@C3       : 无子窗口 设置窗口大小(10, 0)
//@C4       : 无子窗口 设置窗口大小(1024, 1024)
//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      : 检查父窗口和子窗口大小(0, 0)
//@CP2      : 检查父窗口和子窗口大小(1024, 1024)
//////////////////////////////////////////////////////////////////////////
void KWndWindow_Test::SetSize_Test(void)
{
	float size[2] = {10.0, 1024.0};
	KWndWindow NoChild;
	KWndWindow window[6];
	
	window[0].SetStyle(0xFFFFFFFF);
	//增加子窗口
	for (int i=1; i<6; i++)
	{
		window[0].AddChild(&window[i]);
		window[i].SetStyle(0xFFFFFFFF);   //设置窗口类型 
	}

	//有子窗口 设置窗口大小(10, 0)
	//有子窗口 设置窗口大小(1024, 1024)
	for (int j=0; j<2; j++)
	{
		window[0].SetSize(size[j], size[j]);

		{
			//检查父窗口的大小
			float Width = -1, Height = -1;
			window[0].GetSize(&Width, &Height);
			CPPUNIT_ASSERT(Width  == size[j]);      //父窗口的高度和宽度都改变了
			CPPUNIT_ASSERT(Height  == size[j]);   
		}
		//检查所有子窗口的大小
		for (int i=1; i<6; i++)
		{
			float Width = -1, Height = -1;
			window[i].GetSize(&Width, &Height);
			CPPUNIT_ASSERT(Width  == 0.0);       //只改变了子窗口的高度, 而父窗口的高度和宽度都改变了
			CPPUNIT_ASSERT(Height  == size[j]);   //与窗口的类型有关 窗口类型WND_S_SIZE_WITH_B_EDGE
		}
	}

	//无子窗口
	for (int j=0; j<2; j++)
	{
		NoChild.SetSize(size[j], size[j]);
		//检查窗口的大小		
		float Width = -1, Height = -1;
		NoChild.GetSize(&Width, &Height);
		CPPUNIT_ASSERT(Width ==size[j]); 
		CPPUNIT_ASSERT(Height  == size[j]); 	
	}

}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :获取窗口大小
//@前置场景 :无子窗口
//@C1       : 无子窗口 设置窗口大小(0, 0)
//@C2       : 无子窗口 设置窗口大小(10, 10)
//@C3       : 无子窗口 设置窗口大小(1024, 1024)
//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      : 检查窗口大小(0, 0)
//@CP2      : 检查窗口大小(1024, 1024)

//////////////////////////////////////////////////////////////////////////
void KWndWindow_Test::GetSize_Test(void)
{
	//若一个参数为NULL
	KWndWindow m_KWndWindow[4];
	float  point[3] = {0.0, 10.0, 1024.0};
	for (int i=0; i<3; i++)
	{
		float left = -1, top = -1;
		m_KWndWindow[i].SetSize(point[i], point[i]);	
		m_KWndWindow[i].GetSize(&left, &top);
		CPPUNIT_ASSERT(left == point[i]); 
		CPPUNIT_ASSERT(top  == point[i]); 	
	}
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :绝对坐标的调整
//@前置场景 :有子窗口和无子窗口
//@C1       : 有子窗口 设置窗口绝对坐标(0, 0)
//@C2       : 有子窗口 设置窗口绝对坐标(1024, 1024)
//@C3       : 无子窗口 设置窗口绝对坐标(0, 0)
//@C4       : 无子窗口 设置窗口绝对坐标(1024, 1024)
//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      : 检查窗口绝对坐标(0, 0)
//@CP2      : 检查窗口绝对坐标(1024, 1024)

//////////////////////////////////////////////////////////////////////////
void KWndWindow_Test::AbsoluteMove_Test(void)
{
	
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :获得绝对坐标
//@前置场景 :初始化绝对坐标为(0, 0)
//@C1       : 无子窗口 设置窗口绝对坐标(-10, -10)
//@C2       : 无子窗口 设置窗口绝对坐标(20, 20)
//@C3       : 无子窗口 设置窗口绝对坐标(50, 50)
//@C4       : 无子窗口 设置窗口绝对坐标(1024, 1024)
//@C5       : 有子窗口 设置窗口绝对坐标(-10, -10)
//@C6       : 有子窗口 设置窗口绝对坐标(20, 30)
//@C7       : 有子窗口 设置窗口绝对坐标(50, 50)
//@C8       : 有子窗口 设置窗口绝对坐标(1024, 1024)
//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      : 通过输出参数 检查窗口绝对坐标(-10, -10)
//@CP2      : 通过输出参数 检查窗口绝对坐标(20, 20)
//@CP3      : 通过输出参数 检查窗口绝对坐标(50, 50)
//@CP4      : 通过输出参数 检查窗口绝对坐标(1024, 1024)
//////////////////////////////////////////////////////////////////////////
void KWndWindow_Test::GetAbsolutePos_Test(void)
{
	float AbsolutePos = 0.0f;
	float Pos[4] = {-10, 20, 50, 1024};
	KWndWindow NoChild;
	KWndWindow window[10];
	for (int i=1; i<10; i++)
	{
		window[0].AddChild(&window[i]);    //增加子窗口
	}
	
    //-----------------------------------如果绝对坐标小于0 也可以吗?
	for (int j=0; j<4; j++)
	{
		//正确的绝对坐标
		AbsolutePos += Pos[j];
		window[0].AbsoluteMove(Pos[j], Pos[j]);
		//所有窗口的绝对坐标是正确的吗(包括子窗口)
		for (int i=0; i<10; i++)
		{
			float ax = -1, ay = -1;
			window[i].GetAbsolutePos(&ax, &ay);
			CPPUNIT_ASSERT(ax  == AbsolutePos); 
			CPPUNIT_ASSERT(ay  == AbsolutePos); 
		}
	}	

	AbsolutePos = 0.0f;
	for (int j=0; j<4; j++)
	{
		//正确的绝对坐标
		AbsolutePos += Pos[j];
		NoChild.AbsoluteMove(Pos[j], Pos[j]);
		//窗口的绝对坐标是正确的吗
		float ax = -1, ay = -1;
		NoChild.GetAbsolutePos(&ax, &ay);
		CPPUNIT_ASSERT(ax  == AbsolutePos); 
		CPPUNIT_ASSERT(ay  == AbsolutePos); 	
	}
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :获得绝对坐标
//@前置场景 :初始化绝对坐标为(0, 0)  窗口可视
//@C1       : 无子窗口 设置窗口绝对坐标(0, 0) 大小(0, 0)
//@C2       : 无子窗口 设置窗口绝对坐标(50, 50) 大小(20, 20)
//@C3       : 无子窗口 设置窗口绝对坐标(50, 50) 大小(1024, 1024)

//@C4       : 有子窗口 设置窗口绝对坐标(0, 0) 大小(0, 0) 子窗口 坐标(0, 0) 大小(0, 0)
//@C5       : 有子窗口 设置窗口绝对坐标(50, 50) 大小(20, 20) 子窗口坐标(50, 50) 大小(100, 100)
//@C6       : 有子窗口 设置窗口绝对坐标(50, 50) 大小(1024, 1024) 子窗口坐标(10, 10) 大小(10, 10) 
//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      : 通过输出参数 检查坐标是否在窗口内
//////////////////////////////////////////////////////////////////////////

void KWndWindow_Test::PtInWindow_Test(void)
{
	//PtInWindow 的参数仍然是int而不是float
	typedef struct 
	{
		int x;
		int y;
	}POINT, *LPPOINT;

	POINT point[3] = { {10, 10}, {60, 60}, {100, 100} };
	float size[3] = {0, 20, 1024};
	float Pos[3] = {0, 50, 50};

	
	//先检查无子窗口的坐标是否在窗口内
	KWndWindow Parent[3];
	for (int i=0; i<3; i++)
	{
		//设置窗口可视
		Parent[i].SetStyle(WND_S_VISIBLE);  
		Parent[i].AbsoluteMove(Pos[i], Pos[i]);
		Parent[i].SetSize(size[i], size[i]);
	}
	
	for (int i=0; i<3; i++)
	{
		int res = Parent[0].PtInWindow(point[i].x, point[i].y);
		CPPUNIT_ASSERT(res  == 0);  //不在窗口内
	}

	int res = -1;
	res = Parent[0].PtInWindow(point[0].x, point[0].y);
	CPPUNIT_ASSERT(res  == 0);  //不在窗口内
	res = Parent[0].PtInWindow(point[1].x, point[1].y);
	CPPUNIT_ASSERT(res  == 0);  //不在窗口内
	res = Parent[0].PtInWindow(point[2].x, point[2].y);
	CPPUNIT_ASSERT(res  == 0);  //不在窗口内

	res = Parent[1].PtInWindow(point[0].x, point[0].y);
	CPPUNIT_ASSERT(res  == 0);  //不在窗口内
	res = Parent[1].PtInWindow(point[1].x, point[1].y);
	CPPUNIT_ASSERT(res  == 1);  //在窗口内
	res = Parent[1].PtInWindow(point[2].x, point[2].y);
	CPPUNIT_ASSERT(res  == 0);  //不在窗口内

	res = Parent[2].PtInWindow(point[0].x, point[0].y);
	CPPUNIT_ASSERT(res  == 0);  //不在窗口内
	res = Parent[2].PtInWindow(point[1].x, point[1].y);
	CPPUNIT_ASSERT(res  == 1);  //在窗口内
	res = Parent[2].PtInWindow(point[2].x, point[2].y);
	CPPUNIT_ASSERT(res  == 1);  //不在窗口内

	//有子窗口的坐标是否在窗口内
	KWndWindow Child[3];
	//添加子窗口
	for (int i=0; i<3; i++)
	{
		//添加子窗口时会修改子窗口的绝对坐标
		Parent[i].SetStyle(0);
		Parent[i].AddChild(&Child[i]);
	}
	Child[0].SetSize(0, 0);
	res = Parent[0].PtInWindow(point[0].x, point[0].y);
	CPPUNIT_ASSERT(res  == 0);  //不在窗口内
	res = Parent[0].PtInWindow(point[1].x, point[1].y);
	CPPUNIT_ASSERT(res  == 0);  //不在窗口内
	res = Parent[0].PtInWindow(point[2].x, point[2].y);
	CPPUNIT_ASSERT(res  == 0);  //不在窗口内
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :是否为最上层窗口
//@前置场景 :初始化绝对坐标为(0, 0)  窗口可视
//@C1       : 无子窗口 设置窗口绝对坐标(0, 0) 大小(0, 0)
//@C2       : 无子窗口 设置窗口绝对坐标(50, 50) 大小(20, 20)
//@C3       : 无子窗口 设置窗口绝对坐标(50, 50) 大小(1024, 1024)

//@操作源   :
//@操作对象 :m_KWndWindow
//@操作过程 :
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      : 通过输出参数 检查输出是否为最上层窗口
//////////////////////////////////////////////////////////////////////////

void KWndWindow_Test::TopChildFromPoint_Test(void)
{
	//PtInWindow 的参数仍然是int而不是float
	typedef struct 
	{
		int x;
		int y;
	}POINT, *LPPOINT;

	POINT point[3] = { {10, 10}, {60, 60}, {100, 100} };
	float size[3] = {0, 20, 1024};
	float Pos[3] = {0, 50, 50};

	
	//先检查无子窗口的坐标是否在窗口内
	KWndWindow Parent[3];
	for (int i=0; i<3; i++)
	{
		//设置窗口可视
		Parent[i].SetStyle(WND_S_VISIBLE);  
		Parent[i].AbsoluteMove(Pos[i], Pos[i]);
		Parent[i].SetSize(size[i], size[i]);
	}	
	KWndWindow *TopChild = Parent[0].TopChildFromPoint(point[0].x, point[0].y);
	//不在任何窗口内
	CPPUNIT_ASSERT(TopChild  == NULL);  
	TopChild = Parent[0].TopChildFromPoint(point[1].x, point[1].y);
	//不在任何窗口内
	CPPUNIT_ASSERT(TopChild  == NULL); 
	TopChild = Parent[0].TopChildFromPoint(point[2].x, point[2].y);
	//不在任何窗口内
	CPPUNIT_ASSERT(TopChild  == NULL); 

	//检查窗口1
	TopChild = Parent[1].TopChildFromPoint(point[0].x, point[0].y);
	//无上层窗口
	CPPUNIT_ASSERT(TopChild  == NULL);
	TopChild = Parent[1].TopChildFromPoint(point[1].x, point[1].y);
	//最上层窗口是Parent[1]
	CPPUNIT_ASSERT(TopChild  == &Parent[1]);
	TopChild = Parent[1].TopChildFromPoint(point[2].x, point[2].y);
	//无上层窗口
	CPPUNIT_ASSERT(TopChild  == NULL);

	//检查窗口2
	TopChild = Parent[2].TopChildFromPoint(point[0].x, point[0].y);
	//无上层窗口
	CPPUNIT_ASSERT(TopChild  == NULL);
	TopChild = Parent[2].TopChildFromPoint(point[1].x, point[1].y);
	//最上层窗口是Parent[2]
	CPPUNIT_ASSERT(TopChild  == &Parent[2]);
	TopChild = Parent[2].TopChildFromPoint(point[2].x, point[2].y);
	//最上层窗口是Parent[2]
	CPPUNIT_ASSERT(TopChild  == &Parent[2]);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :自己（及子窗口）从窗口树里面里面分离出来

//@前置场景 :
//@C1       :无父窗口
//@C2       :有父窗口 
//@C3       :有父窗口 , 有一个next兄弟窗口
//@C3       :有父窗口 , 有一个next兄弟窗口, 有一个Previous窗口
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :无父窗口 SplitSmaleFamily
//@OP2      :子窗口 SplitSmaleFamily
//@OP3      :兄弟窗口 SplitSmaleFamily
//@后置场景 :
//@CP1      :
//@CP2      :检查窗口树是否正确
//////////////////////////////////////////////////////////////////////////

void KWndWindow_Test::SplitSmaleFamily_Test(void)
{
	//无父窗口
	KWndWindow wnd;
	wnd.SplitSmaleFamily();
	CPPUNIT_ASSERT(wnd.GetNextWnd() == NULL);
	CPPUNIT_ASSERT(wnd.GetPrevWnd() == NULL);
	CPPUNIT_ASSERT(wnd.GetParent() == NULL);
	
	//有父窗口 
	KWndWindow Child;
	wnd.AddChild(&Child);
	Child.SplitSmaleFamily();
	CPPUNIT_ASSERT(Child.GetNextWnd() == NULL);
	CPPUNIT_ASSERT(Child.GetPrevWnd() == NULL);
	//父窗口指针为NULL
	CPPUNIT_ASSERT(Child.GetParent() == NULL);
	
	//有父窗口 , 有一个next兄弟窗口
	KWndWindow parent1, child1, brothor1;
	parent1.AddChild(&child1);
	child1.AddBrother(&brothor1);
	child1.SplitSmaleFamily();
	//子窗口的指针为NULL
	CPPUNIT_ASSERT(Child.GetNextWnd() == NULL);
	CPPUNIT_ASSERT(Child.GetPrevWnd() == NULL);
	CPPUNIT_ASSERT(Child.GetParent() == NULL);
	//父窗口的pChild指针指向brothor1;
	CPPUNIT_ASSERT(parent1.GetFirstChild() == &brothor1);

	//有父窗口 , 有一个next兄弟窗口, 有一个Previous窗口
	KWndWindow parent2, child2, brothor2, brothor3;
	parent2.AddChild(&child2);
	parent2.AddChild(&brothor2);
	parent2.AddChild(&brothor3);
	brothor2.SplitSmaleFamily();
	//子窗口的指针为NULL
	CPPUNIT_ASSERT(brothor2.GetNextWnd() == NULL);
	CPPUNIT_ASSERT(brothor2.GetPrevWnd() == NULL);
	CPPUNIT_ASSERT(brothor2.GetParent() == NULL);
	//父窗口的pChild指针指向child2;
	CPPUNIT_ASSERT(parent2.GetFirstChild() == &child2);
	//brothor3的父窗口指针
	CPPUNIT_ASSERT(brothor3.GetParent() == &parent2);
	//brothor3的兄弟窗口指针
	CPPUNIT_ASSERT(brothor3.GetPrevWnd() == &child2);

	//把第一个子窗口分离出来
	child2.SplitSmaleFamily();
	//父窗口的pChild指针指向borthor3;
	CPPUNIT_ASSERT(parent2.GetFirstChild() == &brothor3);
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :取得包含所有子窗口分布区域的最小区域

//@前置场景 :
//@C1       :
//@C2       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :
//@OP2      :
//@后置场景 :
//@CP1      :
//@CP2      :
//////////////////////////////////////////////////////////////////////////

void KWndWindow_Test::GetAllChildLayoutRect_Test()
{




}
#endif
