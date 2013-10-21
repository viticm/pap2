//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestDownloadPack.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-2-23 15:00
//  Comment     :   验证下载结果的正确性
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestDownloadPack_H__
#define _KTestDownloadPack_H__

#include "cppunit\extensions\HelperMacros.h"
#include "KTestCommon.h"
#include "KHttpDownloader.h"

class KTestDownloadPack : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestDownloadPack);
	CPPUNIT_TEST(TestDownloadOnePack);
	CPPUNIT_TEST(TestDownloadSeveralPack);
	CPPUNIT_TEST(TestDownloadBoundary);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	//设置同时下载1-10个文件，验证下载1个升级包后的结果正确
	void TestDownloadOnePack();
	//设置同时下载1-10个文件，验证下载多个升级包后的结果正确，这里测试下载11个包
	void TestDownloadSeveralPack();
	//设置同时下载9个文件，验证下载8个、9个、10个升级包后的结果正确
	void TestDownloadBoundary();

private:
	//执行下载升级包
	int Download(KHttpDownloader* pDownloader, const int cnPackNum);
	//检验下载后的所有包的MD5值，并每验证一个删除一个
	int CheckAndDelZip(const int cnPackNum);

private:
	PROCESS_INFORMATION m_ServerProcessInfo;	//服务器进程信息结构体
	STARTUPINFO m_ServerStartupInfo;			//服务器启动信息结构体
};
#endif ///_KTestDownloadPack_H__