//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestCombinePack.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-2-25 11:00
//  Comment     :   验证合并pak包的正确性
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestCombinePack_H__
#define _KTestCombinePack_H__

#include "cppunit\extensions\HelperMacros.h"
#include "KTestCommon.h"

class KTestCombinePack : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestCombinePack);
	CPPUNIT_TEST(TestCombineOnce);
	CPPUNIT_TEST(TestCombineSeveralTimes);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	//合并1次升级包，并进行结果验证
	void TestCombineOnce();
	//合并多次升级包，并进行结果验证，这里测试合10次包
	void TestCombineSeveralTimes();

private:
	TCHAR m_szTestPack1[MAX_PATH];		//合并目录包
	TCHAR m_szTestPack2[MAX_PATH];		//测试合并包
};

#endif ///_KTestCombinePack_H__