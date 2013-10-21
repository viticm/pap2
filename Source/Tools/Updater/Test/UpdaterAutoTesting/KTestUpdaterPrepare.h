//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestUpdaterPrepare.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-2-18 10:00
//  Comment     :   更新前的数据准备的测试，主要验证更新前获取的数据的正确性
//					由于LoadConfig直接读取运行目录下的配置文件，所以此功能接口只能在运行目录下测试，无法在测试空间进行测试
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestUpdaterPrepare_H_
#define _KTestUpdaterPrepare_H_

#include "cppunit\extensions\HelperMacros.h"
#include "KTestCommon.h"

class KTestUpdaterPrepare : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestUpdaterPrepare);
	CPPUNIT_TEST(TestPrepareSuccess);
	CPPUNIT_TEST(TestLostVersionFile);
 	CPPUNIT_TEST(TestLostConfigFile);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	//获取的数据正确性验证
	void TestPrepareSuccess();
	//版本信息文件缺失的处理
	void TestLostVersionFile();
	//配置文件缺失的处理
	void TestLostConfigFile();

private:
	//准备测试用的版本信息文件
	int PrepareVersionFile(const TCHAR cszVersion[]);
	//准备测试用的配置文件
	int PrepareGUconfigFile(const TCHAR cszRemotePath[]);
	
private:
	TCHAR m_szGUconfig[MAX_PATH];					//配置文件
	TCHAR m_szVersionFile[MAX_PATH];				//版本信息文件
	TCHAR m_szUpdateTempDir[MAX_PATH];				//临时更新目录
	TCHAR m_szAppDir[MAX_PATH];						//测试用程序运行目录
	BOOL m_bUpdateDirCreated;						//临时更新文件夹创建标记
};

#endif ///_KTestUpdaterPrepare_H_