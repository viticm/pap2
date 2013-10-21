#include "stdafx.h"
#include <algorithm>
#include "KSO3World.h"
#include "KRandGenerator_Test.h"
using namespace std;
#ifndef TEST_DEBUG
CPPUNIT_TEST_SUITE_REGISTRATION(KRandGenerator_Test);
#endif

const int KRandGenerator_Test::RAND_BUF_SIZE;

void KRandGenerator_Test::setUp()
{
	m_pKRandGenerator1 = new KRandGenerator;
	m_pKRandGenerator2 = new KRandGenerator;
	m_pKRandGenerator3 = new KRandGenerator;
	m_pKRandGenerator4 = new KRandGenerator;
	m_pKRandGenerator5 = new KRandGenerator;

	g_SO3World.m_CurrentTime = time(0);
	startTime = g_SO3World.m_CurrentTime;

	initRandBuf1(m_RandBuf1);
	initRandBuf2(m_RandBuf2);
	initRandBuf3(m_RandBuf3);
	initRandBuf4(m_RandBuf4);
}

void KRandGenerator_Test::tearDown()
{
	delete m_pKRandGenerator1;
	delete m_pKRandGenerator2;
	delete m_pKRandGenerator3;
	delete m_pKRandGenerator4;
	delete m_pKRandGenerator5;
}

void KRandGenerator_Test::testLoad_TC1()
{
	KS_CPPUNIT_DESCRIPTION("输入缓冲为NULL指针");
	char *ret = m_pKRandGenerator1->Load(0);
	CPPUNIT_ASSERT(ret == NULL);
}

void KRandGenerator_Test::testLoad_TC2()
{
	KS_CPPUNIT_DESCRIPTION("Rand个数为0，GlobalRP为0");
	char outBuf[RAND_BUF_SIZE];
	char *ret = m_pKRandGenerator2->Load(m_RandBuf1);
	char *retExpected = getRandBufEnd(m_RandBuf1);
	size_t size = retExpected - m_RandBuf1;
	CPPUNIT_ASSERT(ret == retExpected);

	ret = m_pKRandGenerator2->Save(outBuf);
	retExpected = getRandBufEnd(outBuf);
	CPPUNIT_ASSERT(ret == retExpected);
	CPPUNIT_ASSERT(!memcmp(m_RandBuf1, outBuf, size));
}

void KRandGenerator_Test::testLoad_TC3()
{
	KS_CPPUNIT_DESCRIPTION("Rand个数为MAX_RAND_COUNT,Global RP为SHRT_MAX");
	char outBuf[RAND_BUF_SIZE];
	RAND_TIMER_VECTOR outVec, expectVec;
	char *ret, *retExpected;
	char *pInRPData, *pOutRPData;	//points to where GLOBAL rp is
	
	//return value of Load()
	retExpected = getRandBufEnd(m_RandBuf2);
	ret = m_pKRandGenerator3->Load(m_RandBuf2);
	CPPUNIT_ASSERT(ret == retExpected);

	//save返回值
	ret = m_pKRandGenerator3->Save(outBuf);
	retExpected = getRandBufEnd(outBuf);
	CPPUNIT_ASSERT(ret == retExpected);
	//<index, time_t>部分
	loadRandTimer(m_RandBuf2, expectVec);
	loadRandTimer(outBuf, outVec);
	CPPUNIT_ASSERT(outVec == expectVec);

	//RP部分
	size_t rpSize = getRPSize();
	pInRPData = findGlobalRPPos(m_RandBuf2);
	pOutRPData = findGlobalRPPos(outBuf);
	CPPUNIT_ASSERT(!memcmp(pInRPData, pOutRPData, rpSize));
}

void KRandGenerator_Test::testLoad_TC4()
{
	KS_CPPUNIT_DESCRIPTION("Rand个数为1， Global RP为1");
	char outBuf[RAND_BUF_SIZE];
	char *retExpected, *ret;
	size_t size;

	//return value of Load();
	ret = m_pKRandGenerator4->Load(m_RandBuf3);
	retExpected = getRandBufEnd(m_RandBuf3);
	size = retExpected - m_RandBuf3;
	CPPUNIT_ASSERT(ret == retExpected);

	ret = m_pKRandGenerator4->Save(outBuf);
	retExpected = getRandBufEnd(outBuf);
	CPPUNIT_ASSERT(ret == retExpected);
	CPPUNIT_ASSERT(!memcmp(outBuf, m_RandBuf3, size));
}

void KRandGenerator_Test::testSave_TC1()
{
	KS_CPPUNIT_DESCRIPTION("输出缓冲指针为NULL");
	char *ret;

	ret = m_pKRandGenerator4->Save(0);
	CPPUNIT_ASSERT(ret == NULL);
}

void KRandGenerator_Test::testSave_TC2()
{
	KS_CPPUNIT_DESCRIPTION("改变Global RP后保存");
	char outBuf[RAND_BUF_SIZE];
	char expectBuf[RAND_BUF_SIZE];
	char *pInRPData, *pOutRPData;

	m_pKRandGenerator5->Load(m_RandBuf2);
	m_pKRandGenerator5->m_wGlobalRPValue = 100;
	m_pKRandGenerator5->Save(outBuf);

	memcpy(expectBuf, m_RandBuf2, RAND_BUF_SIZE);
	writeGlobbalRP(expectBuf, MAX_RAND_COUNT, 100);
	pInRPData = findGlobalRPPos(expectBuf);
	pOutRPData = findGlobalRPPos(outBuf);
	size_t rpSize = getRPSize();
	CPPUNIT_ASSERT(!memcmp(pInRPData, pOutRPData, rpSize));
}
void KRandGenerator_Test::initRandBuf1(char *buf)
{
	writeRandCount(buf, 0);
	writeGlobbalRP(buf, 0, 0);

	for (int i = 1; i <= MAX_RP_GROUP_COUNT; i++)
	{
		writeRP(buf, 0, i, i - 1);
	}
}

void KRandGenerator_Test::initRandBuf2(char *buf)
{
	writeRandCount(buf, MAX_RAND_COUNT);
	for (int i = 1; i <= MAX_RAND_COUNT; i++)
	{
		writeRandIndex(buf, i, i - 1);
		writeRandTime(buf, i, startTime + i);
	}
	writeGlobbalRP(buf, MAX_RAND_COUNT, SHRT_MAX);
	for (int i = 1; i <= MAX_RP_GROUP_COUNT; i++)
	{
		writeRP(buf, MAX_RAND_COUNT, i, i);
	}
	//some modifications
	writeRandIndex(buf, 5, 1001);
	writeRandTime(buf, 5, startTime - 10);

}

void KRandGenerator_Test::initRandBuf3(char *buf)
{
	writeRandCount(buf, MAX_RAND_COUNT);
	for (int i = 1; i <= MAX_RAND_COUNT; i++)
	{
		writeRandIndex(buf, i, MAX_RAND_COUNT + i);
		writeRandTime(buf, i, startTime - i);
	}
	writeGlobbalRP(buf, MAX_RAND_COUNT, SHRT_MAX);
	for (int i = 1; i <= MAX_RP_GROUP_COUNT; i++)
	{
		writeRP(buf, MAX_RAND_COUNT, i, 0);
	}
}

void KRandGenerator_Test::initRandBuf4(char *buf)
{
	int adjust;
	writeRandCount(buf, 1);
	writeRandIndex(buf, 1, 65535);
	writeRandTime(buf, 1, startTime + 172800); //172800是48小时的秒数
	writeGlobbalRP(buf, 1, 1);
	for (int i = 1; i <= MAX_RP_GROUP_COUNT; i++)
	{
		adjust = (i % 2 == 0) ? -1 : 1;
		adjust *= i;
		writeRP(buf, 1, i, 99 + adjust);	//围绕99上下波动
	}
}

void KRandGenerator_Test::writeRandCount(char *buf, BYTE randCount)
{
	BYTE *pCount;
	pCount = reinterpret_cast<BYTE*>(buf);
	*pCount = randCount;
}

void KRandGenerator_Test::writeRandIndex(char *buf, int randNO, WORD index)
{
	WORD *pWord;
	buf += sizeof(BYTE) + (sizeof(WORD) + sizeof(time_t)) * (randNO - 1);
	pWord = reinterpret_cast<WORD*>(buf);
	*pWord = index;
}

void KRandGenerator_Test::writeRandTime(char *buf, int randNO, time_t t)
{
	time_t *pTime;
	buf += sizeof(BYTE) + (sizeof(WORD) + sizeof(time_t)) * (randNO - 1) +
		sizeof(WORD);
	pTime = reinterpret_cast<time_t*>(buf);
	*pTime = t;
}

void KRandGenerator_Test::writeGlobbalRP(char *buf, int randCount, 
										 WORD globalRP)
{
	WORD *pWord;
	buf += sizeof(BYTE) + (sizeof(WORD) + sizeof(time_t)) * randCount;
	pWord = reinterpret_cast<WORD*>(buf);
	*pWord = globalRP;
}

void KRandGenerator_Test::writeRP(char *buf, int randCount, int rpNO, 
								  WORD rp)
{
	WORD *pWord;
	buf += sizeof(BYTE) + (sizeof(WORD) + sizeof(time_t)) * randCount;
	buf += sizeof(WORD);	//skip global RP
	buf += sizeof(WORD) * (rpNO - 1);
	pWord = reinterpret_cast<WORD*>(buf);
	*pWord = rp;
}

char *KRandGenerator_Test::getRandBufEnd(char *buf)
{
	BYTE *pRandCount;
	int offset;
	pRandCount = reinterpret_cast<BYTE*>(buf);
	offset = sizeof(BYTE) + 
		(sizeof(WORD) + sizeof(time_t)) * (*pRandCount) + 
		sizeof(WORD) +
		sizeof(WORD) * MAX_RP_GROUP_COUNT;
	return buf + offset;
}

void KRandGenerator_Test::
loadRandTimer(char *buf, RAND_TIMER_VECTOR &vec)
{
	BYTE *pCount;
	WORD *pRandIndex;
	time_t *pRandTime;

	vec.clear();
	pCount = reinterpret_cast<BYTE*>(buf);
	buf += sizeof(BYTE);
	for (int i = 0; i < *pCount; i++);
	{
		pRandIndex = reinterpret_cast<WORD*>(buf);
		buf += sizeof(WORD);
		pRandTime = reinterpret_cast<time_t*>(buf);
		buf += sizeof(time_t);
		vec.push_back(make_pair(*pRandIndex, *pRandTime));
	}
	sort(vec.begin(), vec.end());
}
char *KRandGenerator_Test::
findGlobalRPPos(char *buf)
{
	BYTE *pCount;

	pCount = reinterpret_cast<BYTE*>(buf);
	return buf + sizeof(BYTE) + 
		(sizeof(WORD) + sizeof(time_t)) * (*pCount);
}

size_t KRandGenerator_Test::
getRPSize()
{
	return sizeof(WORD) + sizeof(WORD) * MAX_RP_GROUP_COUNT;
}