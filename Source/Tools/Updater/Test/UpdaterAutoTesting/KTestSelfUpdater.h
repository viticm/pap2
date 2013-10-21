//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestSelfUpdater.h    
//  Version     :   1.0
//  Creater     :   linfuyong
//  Date        :   2009-7-15 11:00
//  Comment     :   验证自更新
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestSelfUpdater_H__
#define _KTestSelfUpdater_H__

#include "cppunit\extensions\HelperMacros.h"
#include "KTestCommon.h"

class KTestSelfUpdater : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestSelfUpdater);
	CPPUNIT_TEST(TestNoUpdatePath);
	CPPUNIT_TEST(TestNoIniFile);
	//CPPUNIT_TEST(TestReadOnlyFiles);
	CPPUNIT_TEST(TestGameUpdaterIniURL);
	CPPUNIT_TEST(TestSelfUpdater_Normal);
	CPPUNIT_TEST(TestSelfUpdater_NoIni);
	CPPUNIT_TEST(TestSelfUpdater_ErrParam);
	CPPUNIT_TEST(TestSelfUpdater_ErrServerURL);
	CPPUNIT_TEST(TestVersionHigher);
	CPPUNIT_TEST(TestVersionEqual);

	//CPPUNIT_TEST(TestStartSeveralUpdateProc);	//手动测试
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	// 更新程序的自我更新，update目录不存在的情况
	void TestNoUpdatePath();

	// 更新程序的自我更新，gameupdater.ini不存在的情况
	void TestNoIniFile();

	// 更新程序的自我更新，等待的进程出现异常
	void TestReadOnlyFiles();

	// 更新程序的自我更新，gameupdater.ini配置文件有URL项
	void TestGameUpdaterIniURL();

	// KGU_SelfUpdater单元测试, 正常运行（先执行Init()，再执行Run()）
	void TestSelfUpdater_Normal();

	// KGU_SelfUpdater单元测试, 没有gameupdater.ini文件
	void TestSelfUpdater_NoIni();

	// KGU_SelfUpdater单元测试, Init()传入无效的路径名参数
	void TestSelfUpdater_ErrParam();

	// KGU_SelfUpdater单元测试, gameupdater.ini文件的服务器ip不正确
	void TestSelfUpdater_ErrServerURL();

	// 本地版本号比服务器的版本号高
	void TestVersionHigher();

	// 本地版本号与服务器的版本号相等
	void TestVersionEqual();

	// 更新程序的自我更新，同时启动几个更新进程(手动测试)
	void TestStartSeveralUpdateProc();

private:
	int PrepareExistFiles();
	int PrepareTestSpace(const TCHAR cszTestVersion[]);
	int StartProc();
	int WaitAndCloseProc(const TCHAR szLatestVersion[], UINT uMaxTimeOut);
	int ValidateFileData();
	int SetServerVersion(const TCHAR* pszVersion);
	int GetServerVersion(OUT TCHAR* pszRetVersion);

private:
	STARTUPINFO			m_ServerStartupinfo;		//http服务器启动信息
	STARTUPINFO			m_GameUpdaterStartupinfo;	//GameUpdater进程启动信息
	PROCESS_INFORMATION m_ServerProcessInfo;		//http服务器进程信息结构体
	PROCESS_INFORMATION m_GameUpdaterProcessInfo;	//GameUpdater进程信息结构体
	BOOL				m_bRegSaved;				//记录原始的注册表的标志
	TCHAR				m_szServerVersion[32];		//版本号长度不超过32个字符,如："1-0-0-1505"
};
#endif ///_KTestSelfUpdater_H__