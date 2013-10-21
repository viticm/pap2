#include "stdafx.h"
#include <climits>
#include <map>
#include <iostream>
#include <memory.h>
#include <ctime>
#include "KCDTimerList_Test.h"
#include "KSO3World.h"

using namespace std;
const int KCDTimerList_Test::TIMER_BUF_SIZE;

#ifndef TEST_DEBUG
CPPUNIT_TEST_SUITE_REGISTRATION(KCDTimerList_Test);
#endif

void KCDTimerList_Test::setUp()
{
	g_SO3World = KSO3World();
	g_SO3World.m_CurrentTime = time(0);
	m_StartTime = g_SO3World.m_CurrentTime;
	initTimerBuf1();
	initTimerBuf2();
	initTimerBuf3();
	initTimerBuf4();
	m_TimerList1 = new KCDTimerList;
	m_TimerList2 = new KCDTimerList;
	m_TimerList3 = new KCDTimerList;
	m_TimerList4 = new KCDTimerList;
	m_TimerList5 = new KCDTimerList;
	m_NullTimerList = new KCDTimerList;
}

void KCDTimerList_Test::tearDown()
{
	KG_DELETE(m_TimerList1);	
	KG_DELETE(m_TimerList2);
}
/*
*/
void KCDTimerList_Test::testLoad_TC1()
{
	KS_CPPUNIT_DESCRIPTION("基本计数器缓冲读取。");

	char outBuf[TIMER_BUF_SIZE];
	TIMER_VECTOR_T outVector;
	TIMER_VECTOR_T timerVec1;
	char *pBufEndExpect, *pBufEndReal;

	g_SO3World.m_CurrentTime = m_StartTime;
	pBufEndExpect = getTimerBufEnd(m_TimerBuf1);
	pBufEndReal = m_TimerList1->Load(m_TimerBuf1);
	m_TimerList1->Save(outBuf);
	readTimerBuf(outBuf, outVector);
	readTimerBuf(m_TimerBuf1, timerVec1);
	CPPUNIT_ASSERT(timerVec1.size() == outVector.size());
	CPPUNIT_ASSERT(timerVec1 == outVector);
	CPPUNIT_ASSERT(pBufEndExpect == pBufEndReal);	//检查函数返回值
}

void KCDTimerList_Test::testLoad_TC2()
{
	KS_CPPUNIT_DESCRIPTION("计数器缓冲有重复ID");
	char *ret;
	
	ret = m_TimerList5->Load(m_TimerBuf4);
	CPPUNIT_ASSERT(ret == NULL);
}


//All timers will be saved.
void KCDTimerList_Test::testSave_TC1()
{
	KS_CPPUNIT_DESCRIPTION("基本计时器保存");

	char outBuf[TIMER_BUF_SIZE];
	TIMER_VECTOR_T outVector;
	TIMER_VECTOR_T timerVec1;
	char *pBufEndExpect, *pBufEndReal;

	g_SO3World.m_CurrentTime = m_StartTime;
	m_TimerList1->Load(m_TimerBuf1);
	pBufEndReal = m_TimerList1->Save(outBuf);
	pBufEndExpect = getTimerBufEnd(outBuf);
	readTimerBuf(outBuf, outVector);
	readTimerBuf(m_TimerBuf1, timerVec1);
	CPPUNIT_ASSERT(timerVec1.size() == outVector.size());
	CPPUNIT_ASSERT(timerVec1 == outVector);
	CPPUNIT_ASSERT(pBufEndExpect == pBufEndReal);	//检查函数返回值
}

//Only timer 1, 3, 4, 9 will be saved, others will be filtered when saving.
void  KCDTimerList_Test::testSave_TC2()
{
	KS_CPPUNIT_DESCRIPTION("只有部分计时器会保存");
	char outBuf[TIMER_BUF_SIZE];
	char expectBuf[TIMER_BUF_SIZE];
	char *pOutBufEnd, *pExpectBufEnd;
	TIMER_VECTOR_T outVector, expectVector;

	g_SO3World.m_CurrentTime = m_StartTime;
	//generate expected saved buffer
	writeTimerCount(expectBuf, 4);
	writeTimer(expectBuf, 1, 0, LOGOUT_WAITING_TIME + 1);
	writeTimer(expectBuf, 2, 103, LOGOUT_WAITING_TIME + 3);
	writeTimer(expectBuf, 3, 1004, LOGOUT_WAITING_TIME + 14);
	writeTimer(expectBuf, 4, 100000009, 1000);
	
	m_TimerList1->Load(m_TimerBuf2);
	
#if 0
	KCDTimerList::TIMER_LIST tl = m_TimerList1->m_TimerList; 
	printf("%s %d\n", __FILE__, __LINE__);
	for (KCDTimerList::TIMER_LIST::iterator iter = tl.begin(); 
		iter != tl.end(); iter++)
	{
		cout << iter->first << " : " << iter->second <<endl;
	}
#endif
	pOutBufEnd = m_TimerList1->Save(outBuf);
	pExpectBufEnd = getTimerBufEnd(outBuf);
	readTimerBuf(outBuf, outVector);
	readTimerBuf(expectBuf, expectVector);
#if 0
	printf("%s %d\n", __FILE__, __LINE__);
	for (TIMER_VECTOR_T::iterator iter = outVector.begin(); 
		iter != outVector.end(); iter++)
	{
		cout << iter->first << " : " << iter->second <<endl;
	}
#endif
	CPPUNIT_ASSERT(outVector.size() == expectVector.size());
	CPPUNIT_ASSERT(outVector == expectVector);
	CPPUNIT_ASSERT(pOutBufEnd == pExpectBufEnd);	
}

void KCDTimerList_Test::testCheckTimer()
{
	KS_CPPUNIT_DESCRIPTION("CheckTimer基本检查");
	m_TimerList3->Load(m_TimerBuf2);
	g_SO3World.m_CurrentTime += LOGOUT_WAITING_TIME;
	CPPUNIT_ASSERT(m_TimerList3->CheckTimer(123));	//not exist
	CPPUNIT_ASSERT(m_TimerList3->CheckTimer(100006)); //计时刚结束
	CPPUNIT_ASSERT(!m_TimerList3->CheckTimer(0));	//计时不应结束
	CPPUNIT_ASSERT(m_TimerList3->CheckTimer(10005)); //计时结束
}

void KCDTimerList_Test::testClearTimer()
{
	char outBuf[TIMER_BUF_SIZE], expectBuf[TIMER_BUF_SIZE];
	TIMER_VECTOR_T outVector, expectVector;

	KS_CPPUNIT_DESCRIPTION("ClearTimer基本检查");
	g_SO3World.m_CurrentTime = m_StartTime;

	m_TimerList4->Load(m_TimerBuf1);
#if 0	/* diagonistic print */
	KCDTimerList::TIMER_LIST tl = m_TimerList4->m_TimerList;
	for (KCDTimerList::TIMER_LIST::iterator iter = tl.begin(); 
		iter != tl.end(); iter++)
	{
		cout << iter->first << " : " << iter->second <<endl;
	}
#endif
	m_TimerList4->ClearTimer(323); //timer not exist
	m_TimerList4->ClearTimer(3000); //timer exist
	m_TimerList4->Save(outBuf);
	//construct expected out buf
	writeTimerCount(expectBuf, 4);
	writeTimer(expectBuf, 1, 1, LOGOUT_WAITING_TIME + 3);
	writeTimer(expectBuf, 2, ULONG_MAX, 10000);
	writeTimer(expectBuf, 3, ULONG_MAX - 1, LOGOUT_WAITING_TIME + 1000);
	writeTimer(expectBuf, 4, 0, LOGOUT_WAITING_TIME + 1);
	
	readTimerBuf(outBuf, outVector);
	readTimerBuf(expectBuf, expectVector);
#if 0
	printf("%s, %d\n", __FILE__, __LINE__);
	for (TIMER_VECTOR_T::iterator iter = outVector.begin();
		iter != outVector.end(); iter++)
	{
		cout << iter->first << " : " << iter->second << endl;
	}
#endif
	CPPUNIT_ASSERT(outVector.size() == expectVector.size());
	CPPUNIT_ASSERT(outVector == expectVector);
	//如果TimerList里没有计时器
	m_NullTimerList->ClearTimer(123);
	m_NullTimerList->Save(outBuf);
	writeTimerCount(expectBuf, 0);
	outVector.clear();
	expectVector.clear();
	readTimerBuf(outBuf, outVector);
	readTimerBuf(expectBuf, expectVector);
	CPPUNIT_ASSERT(outVector.size() == expectVector.size());
	CPPUNIT_ASSERT(outVector == expectVector);
}
void KCDTimerList_Test::initTimerBuf1()
{
	memset(m_TimerBuf1, 0, TIMER_BUF_SIZE);
	writeTimerCount(m_TimerBuf1, 5);	//timer count
	//timer 1
	//buf, timer N.O., timerID, interval
	writeTimer(m_TimerBuf1, 1, 1, LOGOUT_WAITING_TIME + 3);
	//timer 2
	writeTimer(m_TimerBuf1, 2, 3000, LOGOUT_WAITING_TIME + 99);
	//timer 3
	writeTimer(m_TimerBuf1, 3, ULONG_MAX, 10000);
	//timer 4
	writeTimer(m_TimerBuf1, 4, ULONG_MAX - 1, LOGOUT_WAITING_TIME + 1000);
	//timer 5
	writeTimer(m_TimerBuf1, 5, 0, LOGOUT_WAITING_TIME + 1);
}

void KCDTimerList_Test::testResetTimer()
{
	KS_CPPUNIT_DESCRIPTION("ResetTimer基本检查");
	g_SO3World.m_CurrentTime = m_StartTime;
	char outBuf[TIMER_BUF_SIZE], expectBuf[TIMER_BUF_SIZE];
	TIMER_VECTOR_T outVector, expectVector;

	m_TimerList2->Load(m_TimerBuf3);
	//Make timer2002 be saved in output
	m_TimerList2->ResetTimer(2002, LOGOUT_WAITING_TIME + 10);
	//make timer101 not be saved in output
	m_TimerList2->ResetTimer(101, LOGOUT_WAITING_TIME - 1);
	//this timer doesn't exist, and will be added
	m_TimerList2->ResetTimer(40004, LOGOUT_WAITING_TIME + 3);
	m_TimerList2->ResetTimer(5005, LOGOUT_WAITING_TIME);

	//construct expected outoput
	writeTimerCount(expectBuf,2);
	writeTimer(expectBuf, 1, 2002, LOGOUT_WAITING_TIME + 10);
	writeTimer(expectBuf, 2, 40004, LOGOUT_WAITING_TIME + 3);
	readTimerBuf(expectBuf, expectVector);

	m_TimerList2->Save(outBuf);
	readTimerBuf(outBuf, outVector);

	CPPUNIT_ASSERT(outVector.size() == expectVector.size());
	CPPUNIT_ASSERT(outVector == expectVector);
}

void KCDTimerList_Test::readTimerBuf(char *timerBuf, TIMER_VECTOR_T &timerVec)
{
	WORD *pCount = reinterpret_cast<WORD*>(timerBuf);
	timerBuf += sizeof(WORD);
	DWORD *pID, timerID;
	int *pInterval, interval;

	timerVec.clear();
	for (int i = 0; i < *pCount; i++)
	{
		pID = reinterpret_cast<DWORD*>(timerBuf);
		timerID = *pID;
		timerBuf += sizeof(DWORD);
		pInterval = reinterpret_cast<int*>(timerBuf);
		interval = *pInterval;
		timerBuf += sizeof(int);
		timerVec.push_back(make_pair(timerID, interval));
	}
	sort(timerVec.begin(), timerVec.end());
}

void KCDTimerList_Test::writeTimerCount(char *timerBuf, WORD timerCount)
{
	WORD *pCount = reinterpret_cast<WORD*>(timerBuf);
	*pCount = timerCount;
}

void KCDTimerList_Test::writeTimer(char *timerBuf, int timerNO, DWORD timerID, 
									  int interval)
{
	DWORD *pID;
	int *pInterval;
	size_t offset;

	offset = sizeof(WORD) + (sizeof(DWORD) + sizeof(int)) * (timerNO - 1);
	timerBuf += offset;
	pID = reinterpret_cast<DWORD*>(timerBuf);
	timerBuf += sizeof(DWORD);
	pInterval = reinterpret_cast<int*>(timerBuf);

	*pID = timerID;
	*pInterval = interval;
}

void KCDTimerList_Test::initTimerBuf2()
{
	writeTimerCount(m_TimerBuf2, 10);
	writeTimer(m_TimerBuf2, 1, 0, LOGOUT_WAITING_TIME + 1);
	writeTimer(m_TimerBuf2, 2, 12, 0);
	writeTimer(m_TimerBuf2, 3, 103, LOGOUT_WAITING_TIME + 3);
	writeTimer(m_TimerBuf2, 4, 1004, LOGOUT_WAITING_TIME + 14);
	writeTimer(m_TimerBuf2, 5, 10005, LOGOUT_WAITING_TIME - 1);
	writeTimer(m_TimerBuf2, 6, 100006, LOGOUT_WAITING_TIME);
	writeTimer(m_TimerBuf2, 7, 1000007, 
		min(max(LOGOUT_WAITING_TIME - 100, 100), 5));
	writeTimer(m_TimerBuf2, 8, 10000008, 0);
	writeTimer(m_TimerBuf2, 9, 100000009, 1000);
	writeTimer(m_TimerBuf2, 10, 1000000010, 6);
}

char* KCDTimerList_Test::getTimerBufEnd(char *timerBuf)
{
	WORD *pCount = reinterpret_cast<WORD*>(timerBuf);
	size_t offset = sizeof(WORD) + 
		(sizeof(DWORD) + sizeof(int)) * (*pCount);
	return timerBuf + offset;
}

void KCDTimerList_Test::initTimerBuf3()
{
	writeTimerCount(m_TimerBuf3, 3);
	writeTimer(m_TimerBuf3, 1, 101, LOGOUT_WAITING_TIME + 1);
	writeTimer(m_TimerBuf3, 2, 2002, 1);
	writeTimer(m_TimerBuf3, 3, 3003, LOGOUT_WAITING_TIME - 1);
}

//计数器2、4的ID相同
void KCDTimerList_Test::initTimerBuf4()
{
	writeTimerCount(m_TimerBuf4, 4);
	writeTimer(m_TimerBuf4, 1, 901, 1);
	writeTimer(m_TimerBuf4, 2, 902, 2);
	writeTimer(m_TimerBuf4, 3, 903, 3);
	writeTimer(m_TimerBuf4, 4, 902, 4);
}