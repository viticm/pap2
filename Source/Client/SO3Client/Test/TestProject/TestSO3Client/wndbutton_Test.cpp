
#include "stdafx.h"

#ifdef _UNIT_TEST

#include <vector>
#include <string>
#include "wndbutton_test.h"
#include "../ui/elem/wndbutton.h"
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(KWndButton_Test);


KWndButton_Test::KWndButton_Test()
{

}

KWndButton_Test::~KWndButton_Test()
{

}


void KWndButton_Test::setUp()
{


}

void KWndButton_Test::tearDown()
{

}

//////////////////////////////////////////////////////////////////////////
//@用例编号 : 1
//@前置场景 : KWndButton没有任何信息
//@C1:
//@C2:
//@操作源   : 测试代码
//@操作对象 : KWndButton类
//@操作过程 :
//@OP1: 新建一个KWndButton实例
//@OP2: 调用KWndButton.SetToolTipInfo()
//@后置场景 :
//@CP1: 传入空指针
//@CP2: 传入一个字符'a'
//@CP3: 传入4个ANSI字符
//@CP4: 传入中文 2个字节
//@CP5: 传入中文 和 数字  20个字节
//@CP6: 传入长ANSI字符串 256个字符'a'
//////////////////////////////////////////////////////////////////////////

void KWndButton_Test::SetToolTipInfo_test(void)
{
	int nRetCode = 0;
	string str_ANSITest(256, 'a');   //256个字符'a'
	vector<string> cs(100);
	//1个ANSI字符
	cs[0] = " ";
	//4个ANSI字符
	cs[1] = "   1";    
	//一个中文字符
	cs[2] = "符";   
	//特殊符号 长度:20
	cs[3] = "中文▲●★【】123456";  
	//长ANSI字符串 长度:256 个字符'a'
	cs[4] = str_ANSITest;
	

	//未检查传入空指针
	//KWndButton m_KWndButton;
	//nRetCode = m_KWndButton.SetToolTipInfo(NULL, 20);
	//CPPUNIT_ASSERT(nRetCode);

	for (int i=0; i<5; i++)   //建议将SetToolTipInfo的第一个参数改为const char*
	{
		KWndButton m_KWndButton2;
	
		const char *data = cs[i].c_str();
		int nRetCode = m_KWndButton2.SetToolTipInfo(const_cast<char *>(data),
												(int)cs[i].size());	

		//ClassDataCopyBuffer copy;
		//copy.SetFirstCopy(reinterpret_cast<void *>(&m_KWndButton2), sizeof(m_KWndButton2));
		//SWndButton_Data *s_data = reinterpret_cast<SWndButton_Data *>(copy.GetFirstCopy());

		//SWndButton_Data *s_data = 
		//	reinterpret_cast<SWndButton_Data *>
		//	(CopyClassData(reinterpret_cast<void *>(&m_KWndButton2), sizeof(m_KWndButton2)));
		char *p = m_KWndButton2.m_szTip;
		CPPUNIT_ASSERT(strncmp(cs[i].c_str(), p, nRetCode) == 0);
	}
}


//////////////////////////////////////////////////////////////////////////
//@用例编号 : 1
//@前置场景 : 设置KWndButton的Tip
//@C1:
//@C2:
//@操作源   : 测试代码
//@操作对象 : KWndButton类
//@操作过程 :
//@OP1: 新建一个KWndButton实例
//@OP2: 调用KWndButton.GetToolTipInfo()
//@后置场景 :GetToolTipInfo将返回信息存储在缓冲区
//@CP1: 检查参数为NULL
//@CP2:传入1个字节的缓冲区
//@CP3:传入63个字节的缓冲区
//@CP4:传入256个字节的缓冲区
//////////////////////////////////////////////////////////////////////////

void KWndButton_Test::GetToolTipInfo_Test(void)
{
	KWndButton m_KWndButton;
	char *szBuffer = NULL;
	char szBuffer_1[1];
	char szBuffer_2[63];
	char szBuffer_3[256];
	
	m_KWndButton.SetToolTipInfo("l.l.l", 4);
	//ClassDataCopyBuffer copy;
	//copy.SetFirstCopy(reinterpret_cast<void *>(&m_KWndButton), sizeof(KWndButton));
	//SWndButton_Data *s_data = reinterpret_cast<SWndButton_Data *>(copy.GetFirstCopy());

	//SWndButton_Data *s_data = reinterpret_cast<SWndButton_Data *>
				//(CopyClassData(reinterpret_cast<void *>(&m_KWndButton), sizeof(m_KWndButton)));
	char *p = m_KWndButton.m_szTip;

	//未检查参数为NULL
	int retCode = -1;
	//retCode = m_KWndButton.GetToolTipInfo(szBuffer, 4);
    //CPPUNIT_ASSERT
	//传入1个字节的缓冲区 字符串最后未赋值为'\0'
	retCode = m_KWndButton.GetToolTipInfo(szBuffer_1, 1);
	CPPUNIT_ASSERT(strncmp(szBuffer_1, p, retCode) == 0);
	//传入63个字节的缓冲区 字符串最后未赋值为'\0'
	retCode = m_KWndButton.GetToolTipInfo(szBuffer_2, 63);
	CPPUNIT_ASSERT(strncmp(szBuffer_2, p, retCode) == 0);
	//传入256个字节的缓冲区
	//字符串最后未赋值为'\0'
	retCode = m_KWndButton.GetToolTipInfo(szBuffer_3, 256);
	CPPUNIT_ASSERT(strncmp(szBuffer_3, p, retCode) == 0);
}

#endif
