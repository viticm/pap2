//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestRegOperation.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-2-23 15:00
//  Comment     :   验证读写注册表接口的正确性
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestRegOperation_H__
#define _KTestRegOperation_H__

#include "cppunit\extensions\HelperMacros.h"
#include "KTestCommon.h"

class KTestRegOperation : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestRegOperation);
	CPPUNIT_TEST(TestInstallDirectory);
	CPPUNIT_TEST(TestVersionInfo);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	//验证读写注册表安装信息
	void TestInstallDirectory();
	//验证读写注册表版本信息
	void TestVersionInfo();

private:
	BOOL m_bRegSaved;								//记录原始的注册表的标志
};

#endif ///_KTestRegOperation_H__