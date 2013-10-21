#include "stdafx.h"

#ifdef _UNIT_TEST
#include "KItemNull_Test.h"
#include "../ui/elem/wndstation.h"

CPPUNIT_TEST_SUITE_REGISTRATION(KItemNull_Test);

void KItemNull_Test::setUp()
{
	int nRetCode = false;
	m_item = NULL;


	m_item = UI::KItemNull::Create(m_itemdata);
	CPPUNIT_ASSERT(m_item);	

	nRetCode = UI::KWndStation::Init();
	CPPUNIT_ASSERT(nRetCode);
	
}

void KItemNull_Test::tearDown()
{
	if (m_item)
	{
		KG_COM_RELEASE(m_item);
	}

	UI::KItemEventMgr::Exit();
}

//////////////////////////////////////////////////////////////////////////
//@用例编号 :1
//@用例目的 :验证int KItemNull::Init(KItemNullData & ItemNullData) 的正确性
//@前置场景 :
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :传入空对象
//@OP2      :未初始化内存的KItemNullData
//@OP3      :KItemNullData第一个字符为0
//@OP4      :内存皆初始化为1
//@OP5      :赋值正常的KItemNullData对象
//@OP6      :Init两次
//@后置场景 :
//@CP1      :Init失败
//@CP2      :Init失败
//@CP3      :Init失败
//@CP4      :Init失败
//@CP5      :Init成功
//@CP6      :Init成功
//////////////////////////////////////////////////////////////////////////
void KItemNull_Test::InitItemTest_1()
{
	int nRetCode = false;
	UI::KItemNullData *pTempItemData = NULL;

	nRetCode = m_item->Init(*pTempItemData);
	CPPUNIT_ASSERT(!nRetCode);
}

void KItemNull_Test::InitItemTest_2()
{
	int nRetCode = false;
	UI::KItemNullData *pTempItemData = NULL;

	pTempItemData = (UI::KItemNullData *)malloc(sizeof(UI::KItemNullData));
	nRetCode  = m_item->Init(*pTempItemData);
	if (pTempItemData)
	{
		free(pTempItemData);
		pTempItemData = NULL;
	}
	CPPUNIT_ASSERT(!nRetCode);
}

void KItemNull_Test::InitItemTest_3()
{
	int nRetCode = false;
	UI::KItemNullData *pTempItemData = NULL;
	char *pstr = NULL;


	pTempItemData = (UI::KItemNullData *)malloc(sizeof(UI::KItemNullData));
	pstr = (char *)pTempItemData;
	pstr[0] = '\0';
	nRetCode = m_item->Init(*pTempItemData);
	if (pTempItemData)
	{
		free(pTempItemData);
		pTempItemData = NULL;
	}
	CPPUNIT_ASSERT(!nRetCode);

}

void KItemNull_Test::InitItemTest_4()
{
	int nRetCode = false;
	UI::KItemNullData *pTempItemData = NULL;
	char *pstr = NULL;

	pTempItemData = (UI::KItemNullData *)malloc(sizeof(UI::KItemNullData));
	memset(pTempItemData, 1, sizeof(UI::KItemNullData));
	nRetCode = m_item->Init(*pTempItemData);
	if (pTempItemData)
	{
		free(pTempItemData);
		pTempItemData = NULL;
	}
	CPPUNIT_ASSERT(!nRetCode);
}

void KItemNull_Test::InitItemTest_5()
{
	int nRetCode = false;
	UI::KItemNullData *pTempItemData = NULL;
	char *pstr = NULL;

	strcpy(m_itemdata.szName, "testitem1");
	m_itemdata.nAutoSize = 0;
	m_itemdata.nPosType = 1;
	m_itemdata.fHeight = 0.1f;
	m_itemdata.fWidth = 0.2f;
	pTempItemData = &m_itemdata;
	nRetCode = m_item->Init(*pTempItemData);
	CPPUNIT_ASSERT(nRetCode);
}

void KItemNull_Test::InitItemTest_6()
{
	int nRetCode = false;
	UI::KItemNullData *pTempItemData = NULL;
	char *pstr = NULL;

	strcpy(m_itemdata.szName, "testitem2");
	m_itemdata.nAutoSize = 2;
	m_itemdata.nPosType = 2;
	m_itemdata.fHeight = 0.5f;
	m_itemdata.fWidth = 0.2f;
	pTempItemData = &m_itemdata;
	nRetCode = m_item->Init(*pTempItemData);
	CPPUNIT_ASSERT(nRetCode);
	nRetCode = m_item->Init(*pTempItemData);
	CPPUNIT_ASSERT(nRetCode);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :2
//@用例目的 :验证virtual void Release(); 的正确性
//@前置场景 :
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1      :连续两次Release
//@后置场景 :
//@CP1      :Release成功
//////////////////////////////////////////////////////////////////////////
void KItemNull_Test::DeleteItemTest()
{
	int nRetCode = false;

	m_item->Release();
	m_item->Release();
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :3
//@用例目的 :验证virtual int IsItemCanChangeTo(DWORD dwType); 的正确性
//@前置场景 :
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1       :参数为ITEM_BEGINE
//@OP2       :参数为ITEM_NULL
//@OP3       :参数为ITEM_ANIAMTE
//@OP4       :参数为ITEM_COUNT
//@OP5       :参数为100
//@OP6       :参数为-1 
//@后置场景 :
//@CP1      :返回成功
//@CP2      :返回成功
//@CP3      :返回失败
//@CP4      :返回失败
//@CP5      :返回失败
//@CP6      :返回失败
//////////////////////////////////////////////////////////////////////////
void KItemNull_Test::ChgItemTypeTest_1()
{
	int nRetCode = false;
	DWORD dwItemType = UI::ITEM_BEGINE;

	nRetCode = m_item->IsItemCanChangeTo(dwItemType);
	CPPUNIT_ASSERT(nRetCode);
}

void KItemNull_Test::ChgItemTypeTest_2()
{
	int nRetCode = false;
	DWORD DWItemType = UI::ITEM_NULL;

	nRetCode = m_item->IsItemCanChangeTo(DWItemType);
	CPPUNIT_ASSERT(nRetCode);
}

void KItemNull_Test::ChgItemTypeTest_3()
{
	int nRetCode = false;
	DWORD DWItemType = UI::ITEM_ANIAMTE;

	nRetCode = m_item->IsItemCanChangeTo(DWItemType);
	CPPUNIT_ASSERT(nRetCode);
}

void KItemNull_Test::ChgItemTypeTest_4()
{
	int nRetCode = false;
	DWORD DWItemType = UI::ITEM_COUNT;

	nRetCode = m_item->IsItemCanChangeTo(DWItemType);
	CPPUNIT_ASSERT(nRetCode);
}

void KItemNull_Test::ChgItemTypeTest_5()
{
	int nRetCode = false;
	DWORD DWItemType = 100;

	nRetCode = m_item->IsItemCanChangeTo(DWItemType);
	CPPUNIT_ASSERT(nRetCode);
}

void KItemNull_Test::ChgItemTypeTest_6()
{
	int nRetCode = false;
	DWORD DWItemType = -1;

	nRetCode = m_item->IsItemCanChangeTo(DWItemType);
	CPPUNIT_ASSERT(nRetCode);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :4
//@用例目的 :验证int KItemNull::PtInItem(float fAbsX, float fAbsY)的正确性
//@前置场景 :
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1       :X<m_fAbsX,Y<m_fAbsY
//@OP2       :X<m_fAbsX,Y=m_fAbsY
//@OP3       :X=m_fAbsX,Y<m_fAbsY
//@OP4       :X=m_fAbsX,Y=m_fAbsY
//@OP5       :X=m_fAbsX + m_fWidth,y=m_fAbsY +m_fHeight
//@OP6       :X>m_fAbsX + m_fWidth,y=m_fAbsY +m_fHeight
//@OP7       :X=m_fAbsX + m_fWidth,y>m_fAbsY +m_fHeight
//@OP8       :X>m_fAbsX + m_fWidth,y>m_fAbsY +m_fHeight
//@OP9       :Item属性为显示
//@OP10      :Item属性为隐藏
//@后置场景 :
//@CP1      :不在Item中
//@CP2      :不在Item中
//@CP3      :不在Item中
//@CP4      :在Item中
//@CP5      :在Item中
//@CP6      :不在Item中
//@CP7      :不在Item中
//@CP8      :不在Item中
//@CP9      :坐标x=m_fAbsX+1,y=m_fAbsY+1,坐标在Item中
//@CP10     :坐标x=m_fAbsX+1,y=m_fAbsY+1,坐标不在Item中
//////////////////////////////////////////////////////////////////////////
void KItemNull_Test::PtINItemTest_1()
{
	int nResult = false;
	int nRetCode = false;
	float fx = 0;
	float fy = 0;

	nRetCode = m_item->SetAbsPos(1, 1);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->SetSize(10, 10);
	KG_PROCESS_ERROR(nRetCode);
	fx = 0;
	fy = 0;
	nRetCode = m_item->PtInItem(fx,fy);
	KG_PROCESS_ERROR(!nRetCode);
	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
	
}

void KItemNull_Test::PtINItemTest_2()
{
	int nResult = false;
	int nRetCode = false;
	float fx = 0;
	float fy = 0;

	nRetCode = m_item->SetAbsPos(1, 1);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->SetSize(10, 10);
	KG_PROCESS_ERROR(nRetCode);
	fx = 0;
	fy = 1;
	nRetCode = m_item->PtInItem(fx,fy);
	KG_PROCESS_ERROR(!nRetCode);
	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KItemNull_Test::PtINItemTest_3()
{
	int nResult = false;
	int nRetCode = false;
	float fx = 0;
	float fy = 0;

	nRetCode = m_item->SetAbsPos(1, 1);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->SetSize(10, 10);
	KG_PROCESS_ERROR(nRetCode);
	fx = 1;
	fy = 0;
	nRetCode = m_item->PtInItem(fx,fy);
	KG_PROCESS_ERROR(!nRetCode);
	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KItemNull_Test::PtINItemTest_4()
{
	int nResult = false;
	int nRetCode = false;
	float fx = 0;
	float fy = 0;

	nRetCode = m_item->SetAbsPos(1, 1);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->SetSize(10, 10);
	KG_PROCESS_ERROR(nRetCode);
	fx = 1;
	fy = 1;
	nRetCode = m_item->PtInItem(fx,fy);
	KG_PROCESS_ERROR(nRetCode);
	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KItemNull_Test::PtINItemTest_5()
{
	int nResult = false;
	int nRetCode = false;
	float fx = 0;
	float fy = 0;

	nRetCode = m_item->SetAbsPos(1, 1);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->SetSize(10, 10);
	KG_PROCESS_ERROR(nRetCode);
	fx = 11;
	fy = 11;
	nRetCode = m_item->PtInItem(fx,fy);
	KG_PROCESS_ERROR(nRetCode);
	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KItemNull_Test::PtINItemTest_6()
{
	int nResult = false;
	int nRetCode = false;
	float fx = 0;
	float fy = 0;

	nRetCode = m_item->SetAbsPos(1, 1);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->SetSize(10, 10);
	KG_PROCESS_ERROR(nRetCode);
	fx = 15;
	fy = 11;
	nRetCode = m_item->PtInItem(fx,fy);
	KG_PROCESS_ERROR(!nRetCode);
	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KItemNull_Test::PtINItemTest_7()
{
	int nResult = false;
	int nRetCode = false;
	float fx = 0;
	float fy = 0;

	nRetCode = m_item->SetAbsPos(1, 1);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->SetSize(10, 10);
	KG_PROCESS_ERROR(nRetCode);
	fx = 11;
	fy = 12;
	nRetCode = m_item->PtInItem(fx,fy);
	KG_PROCESS_ERROR(!nRetCode);
	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KItemNull_Test::PtINItemTest_8()
{
	int nResult = false;
	int nRetCode = false;
	float fx = 0;
	float fy = 0;

	nRetCode = m_item->SetAbsPos(1, 1);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->SetSize(10, 10);
	KG_PROCESS_ERROR(nRetCode);
	fx = 15;
	fy = 12;
	nRetCode = m_item->PtInItem(fx,fy);
	KG_PROCESS_ERROR(!nRetCode);
	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KItemNull_Test::PtINItemTest_9()
{
	int nResult = false;
	int nRetCode = false;
	float fx = 0;
	float fy = 0;

	nRetCode = m_item->SetAbsPos(1, 1);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->SetSize(10, 10);
	KG_PROCESS_ERROR(nRetCode);
	fx = 7;
	fy = 7;
	nRetCode = m_item->Show();
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->PtInItem(fx,fy);
	KG_PROCESS_ERROR(nRetCode);
	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KItemNull_Test::PtINItemTest_10()
{
	int nResult = false;
	int nRetCode = false;
	float fx = 0;
	float fy = 0;

	nRetCode = m_item->SetAbsPos(1, 1);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->SetSize(10, 10);
	KG_PROCESS_ERROR(nRetCode);
	fx = 7;
	fy = 7;
	nRetCode = m_item->Hide();
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = m_item->PtInItem(fx,fy);
	KG_PROCESS_ERROR(!nRetCode);
	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :5
//@用例目的 :验证int KItemNull::SetPosType(DWORD dwPosType) 的正确性
//@前置场景 :
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1       :设置为POSITION_BEGINE
//@OP2       :设置为POSITION_END
//@OP3       :设置为POSITION_TOP_CENTER
//@OP4       :设置为-1
//@OP5       :设置为100
//@后置场景 :
//@CP1      :设置成功
//@CP2      :设置失败
//@CP3      :设置成功
//@CP4      :设置失败
//@CP5      :设置失败
//////////////////////////////////////////////////////////////////////////
void KItemNull_Test::SetPosTypeTest_1()
{
	int nRetCode = false;

	nRetCode = m_item->SetPosType(UI::POSITION_BEGINE);
	CPPUNIT_ASSERT(nRetCode);
}

void KItemNull_Test::SetPosTypeTest_2()
{
	int nRetCode = false;

	nRetCode = m_item->SetPosType(UI::POSITION_END);
	CPPUNIT_ASSERT(!nRetCode);
}

void KItemNull_Test::SetPosTypeTest_3()
{
	int nRetCode = false;

	nRetCode = m_item->SetPosType(UI::POSITION_TOP_CENTER);
	CPPUNIT_ASSERT(nRetCode);
}

void KItemNull_Test::SetPosTypeTest_4()
{
	int nRetCode = false;

	nRetCode = m_item->SetPosType(-1);
	CPPUNIT_ASSERT(!nRetCode);
}

void KItemNull_Test::SetPosTypeTest_5()
{
	int nRetCode = false;

	nRetCode = m_item->SetPosType(100);
	CPPUNIT_ASSERT(!nRetCode);
}
//////////////////////////////////////////////////////////////////////////
//@用例编号 :6
//@用例目的 :验证int KItemNull::SetName(LPCTSTR pcszItemName) 的正确性
//@前置场景 :
//@C1       :
//@操作源   :
//@操作对象 :
//@操作过程 :
//@OP1       :参数为空指针
//@OP2       :参数为空字符串
//@OP3       :参数为传入字符串长度超出范围ITEM_NAME_MAX_SIZE
//@OP4       :参数为正常字符串
//@后置场景 :
//@CP1      :设置失败
//@CP2      :设置失败
//@CP3      :设置失败
//@CP4      :设置成功
//////////////////////////////////////////////////////////////////////////
void KItemNull_Test::SetNameTest_1()
{
	int nRetCode = false;
	LPCTSTR pcszItemName = NULL;

	nRetCode = m_item->SetName(pcszItemName);
	CPPUNIT_ASSERT(!nRetCode);
}

void KItemNull_Test::SetNameTest_2()
{
	int nRetCode = false;
	LPCTSTR pcszItemName = "";

	//pcszItemName[0] = '\0';
	nRetCode = m_item->SetName(pcszItemName);
	CPPUNIT_ASSERT(!nRetCode);
}
void KItemNull_Test::SetNameTest_3()
{
	int nRetCode = false;
	LPCTSTR pcszItemName = "a123456789b123456789c123456789d12";

	nRetCode = m_item->SetName(pcszItemName);
	CPPUNIT_ASSERT(!nRetCode);
}
void KItemNull_Test::SetNameTest_4()
{
	int nRetCode = false;
	LPCTSTR pcszItemName = "test001";

	nRetCode = m_item->SetName(pcszItemName);
	CPPUNIT_ASSERT(nRetCode);
}
#endif  //#ifdef UNIT_TEST