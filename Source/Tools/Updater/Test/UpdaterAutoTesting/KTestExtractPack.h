//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestExtractPack.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-2-25 11:00
//  Comment     :   验证分pak包的正确性
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestExtractPack_H__
#define _KTestExtractPack_H__

#include "cppunit\extensions\HelperMacros.h"
#include "KTestCommon.h"

class KTestExtractPack : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestExtractPack);
	CPPUNIT_TEST(TestExtractPackSuccess);
	CPPUNIT_TEST(TestExtractPack_NoVersionFile);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	//解压的目标文件夹不存在
	//解压的第一个文件为0kb大小
	//更新包内在不同的目录下有两个version.cfg
	//更新包updatepack目录之外相关文件不能被解压出来
	//更新包updatepack目录之下存在多层目录
	//黑名单下的文件不解压
	//updatepack文件夹之外的文件不需要解压
	void TestExtractPackSuccess();

	//更新包内没有version.cfg
	void TestExtractPack_NoVersionFile();
};

#endif ///_KTestNewPack_H__