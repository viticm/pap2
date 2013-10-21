/**@file 
 * @brief Test class KCDTimerList
 * @author Li Qing
 * @date 2005.12.28
 */
#ifndef _TEST_KCDTIMERLIST_H_
#define _TEST_KCDTIMERLIST_H_
#include <vector>
#include <algorithm>

#include "KSUnitTest/KSCppUnit_Helper.h"
#include "KSO3World.h"
#include "KCDTimerList.h"

/**
 * KCDTimerList_Test
 */
class KCDTimerList_Test : public CppUnit::TestFixture
{
#ifndef TEST_DEBUG
	CPPUNIT_TEST_SUITE(KCDTimerList_Test);
	CPPUNIT_TEST(testLoad_TC1);		/*基本计时器读*/
	CPPUNIT_TEST(testLoad_TC2);		/*计时器缓冲有重复ID*/
	CPPUNIT_TEST(testSave_TC1);		/*基本计时器写*/
	CPPUNIT_TEST(testSave_TC2);		/*只有部分计时器会保存*/
	CPPUNIT_TEST(testResetTimer);
	CPPUNIT_TEST(testCheckTimer);
	CPPUNIT_TEST(testClearTimer);
	CPPUNIT_TEST_SUITE_END();
#endif
public:
	static const int TIMER_BUF_SIZE = 256;
	void setUp();
	void tearDown();
	
	void testLoad_TC1();
	void testLoad_TC2();
	void testSave_TC1();	
	void testSave_TC2();
	void testResetTimer();
	void testCheckTimer();
	void testClearTimer();
protected:
	KCDTimerList *m_TimerList1;
	KCDTimerList *m_TimerList2;
	KCDTimerList *m_TimerList3;
	KCDTimerList *m_TimerList4;
	KCDTimerList *m_TimerList5;
	KCDTimerList *m_NullTimerList;	//空KCDTimerList，里面不存放任何数据

	time_t m_StartTime;
	/**
	 * 原始timer char数组的友好保存方式，即按照<DWORD, int>格式将
	 * 计时器信息保存在vector中。
	 */
	typedef vector<pair<DWORD, int> > TIMER_VECTOR_T;
	char m_TimerBuf1[TIMER_BUF_SIZE];		//测试数据1的原始形式
	char m_TimerBuf2[TIMER_BUF_SIZE];		//测试数据2的原始形式
	char m_TimerBuf3[TIMER_BUF_SIZE];		//测试数据3的原始形式
	char m_TimerBuf4[TIMER_BUF_SIZE];		//测试数据4，有重复ID



	void initTimerBuf1();	
	void initTimerBuf2();
	void initTimerBuf3();
	void initTimerBuf4();
	/** 
	 * 从char*中读timer信息，便于测试中使用。
	 * @param timerBuf	要读取的char数组
	 * @return timerVec	将timerBuf中的计时器保存到timerVec中。
	 */
	void readTimerBuf(char *timerBuf, TIMER_VECTOR_T &timerVec);
	/** 
	 * 把一个计数器写到char数组中
	 * @param timerBuf	缓冲
	 * @param timerNO	第几个计时器？
	 * @param timerID	timer ID
	 * @param interval	timer interval
	 */
	void writeTimer(char *timerBuf, int timerNO, DWORD timerID, 
		int interval);
	/**
	 * 把计时器个数写到缓冲
	 */
	void writeTimerCount(char *timerBuf, WORD timerCount);
	/** 
	 * 获取缓冲区最后一个计时器信息的下一个位置，比如最后一个计时器结束
	 * 与第20个字节，那么就返回指向第21字节的指针。
	 */
	char *getTimerBufEnd(char *timerBuf);
};

#endif //_TEST_KCDTIMERLIST_H_

