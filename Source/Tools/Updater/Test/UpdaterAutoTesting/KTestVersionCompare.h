//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestVersionCompare.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-2-23 10:00
//  Comment     :   验证比较版本号的接口正确性
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestVersionCompare_H__
#define _KTestVersionCompare_H__

#include "cppunit\extensions\HelperMacros.h"
#include "KTestCommon.h"

class KTestVersionCompare : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestVersionCompare);
	CPPUNIT_TEST(TestVersionEqual_01);
	CPPUNIT_TEST(TestVersionEqual_02);
	CPPUNIT_TEST(TestVersionEqual_03);
	CPPUNIT_TEST(TestVersionGreater_01);
	CPPUNIT_TEST(TestVersionGreater_02);
	CPPUNIT_TEST(TestVersionGreater_03);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	//比较版本号相等的功能接口验证
	//测试A版本号大于、小于、等于B版本号的情况
	//分别对三种版本号格式进行验证，如：1.0.8.1341或1,0,8,1341或1-0-8-1341
	void TestVersionEqual_01();
	void TestVersionEqual_02();
	void TestVersionEqual_03();

	//比较版本号大于的功能接口验证
	//测试A版本号大于、小于、等于B版本号的情况
	//分别对三种版本号格式进行验证，如：1.0.8.1341或1,0,8,1341或1-0-8-1341
	void TestVersionGreater_01();
	void TestVersionGreater_02();
	void TestVersionGreater_03();
};
#endif ///KTestVersionCompare