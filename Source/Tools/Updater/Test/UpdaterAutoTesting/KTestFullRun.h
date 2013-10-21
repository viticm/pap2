//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestFullRun.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-3-4 10:00
//  Comment     :   验证完整运行更新程序进行更新后的数据
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestFullRun_H__
#define _KTestFullRun_H__

#include "cppunit\extensions\HelperMacros.h"
#include "KTestCommon.h"

class KTestFullRun : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestFullRun);
	CPPUNIT_TEST(TestUpdateOneVersionAndCombinePack);
	CPPUNIT_TEST(TestUpdateOneVersionAndNewPack);
 	CPPUNIT_TEST(TestUpdateSeveralVersionAndCombinePack);
	CPPUNIT_TEST(TestUpdateSeveralVersionAndNewPack);
	
	//CPPUNIT_TEST(TestUpdateSeveralVersionAndNewPack_OneDay);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown(); 

	//更新一个升级包的验证，同时验证合包和增量更新
	void TestUpdateOneVersionAndCombinePack();
	//更新一个升级包的验证，同时验证分包和覆盖更新
	void TestUpdateOneVersionAndNewPack();
	//更新多个升级包的验证，同时验证合包和增量更新
	void TestUpdateSeveralVersionAndCombinePack();
	//更新多个升级包的验证，同时验证分包和覆盖更新
	void TestUpdateSeveralVersionAndNewPack();

	////临时测试运行一天。更新多个升级包的验证，同时验证分包和覆盖更新
	//int  SetUp_OneDay();
	//int  TearDown_OneDay();
	//void TestUpdateSeveralVersionAndNewPack_OneDay();

private:
	//准备更新后的文件，用于验证覆盖更新
	int PrepareExistFiles();

	//准备完整更新的测试环境
	int PrepareTestSpace(const TCHAR cszTestVersion[]);

	//验证数据更新后的文件
	int ValidateFileData();

	//验证更新后的注册表
	int ValidateRegData(const TCHAR cszVersion[]);

	int StartProc();
	int WaitAndCloseProc(const TCHAR szLatestVersion[], UINT uMaxTimeOut);

	//创建达到默认分包条件的目标合并包
	int CreatePakForNewPack();

private:
	STARTUPINFO			m_ServerStartupinfo;		//http服务器启动信息
	STARTUPINFO			m_GameUpdaterStartupinfo;	//GameUpdater进程启动信息
	PROCESS_INFORMATION m_ServerProcessInfo;		//http服务器进程信息结构体
	PROCESS_INFORMATION m_GameUpdaterProcessInfo;	//GameUpdater进程信息结构体
	BOOL				m_bRegSaved;				//记录原始的注册表的标志
};

#endif ///KTestFullRun