//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestGenerateList.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-2-23 15:00
//  Comment     :   验证产生下载列表的正确性
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestGenerateList_H__
#define _KTestGenerateList_H__

#include "cppunit\extensions\HelperMacros.h"
#include "KTestCommon.h"
#include "KVersionParser.h"

class KTestGenerateList : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestGenerateList);
	CPPUNIT_TEST(TestGenerateSuccess);
	CPPUNIT_TEST(TestLostRecord);
	CPPUNIT_TEST(TestSeveralVersionInOnePack);
	CPPUNIT_TEST(TestIntersectionVersionInside);
	CPPUNIT_TEST(TestIntersectionVersionOutside);
	CPPUNIT_TEST(TestSubsetVersion);
	CPPUNIT_TEST(TestGenerateFail);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	//更新列表中的更新包记录正确，进行生成下载列表正确性的验证
	void TestGenerateSuccess();
	//更新列表中的更新包记录缺失的处理 
	void TestLostRecord();
	//更新列表中的更新包记录有跨多版本的处理  
	void TestSeveralVersionInOnePack();
	//更新列表中的更新包记录有交集版本的处理，输入交集版本"内"的测试版本号
	void TestIntersectionVersionInside();
	//更新列表中的更新包记录有交集版本的处理，输入交集版本"外"的测试版本号
	void TestIntersectionVersionOutside();
	//更新列表中的更新包记录有子集版本的处理 
	void TestSubsetVersion();
	//无法生成针对某版本号的下载列表的处理
	void TestGenerateFail();

private:
	//验证数据
	int ValidateData(
		KVersionParser* pParser,
		const TCHAR** cppZipList,
		const int cnListCount,
		const TCHAR cszEntryFile[],
		const TCHAR cszCurVersion[]
	);
};
#endif ///_KTestGenerateList_H__