//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestWinInetDownloader.h    
//  Version     :   1.0
//  Creater     :   linfuyong
//  Date        :   2009-5-22 11:00
//  Comment     :   验证下载结果的正确性
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestWinInetDownloader_H__
#define _KTestWinInetDownloader_H__

#include "cppunit\extensions\HelperMacros.h"
#include "KTestCommon.h"
#include "KGU_Public.h"
#include <string>

#if defined _UNICODE || defined UNICODE
typedef std::wstring _tstring;
#else
typedef std::string _tstring;
#endif

class KTestWinInetDownloader : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestWinInetDownloader);
    CPPUNIT_TEST(TestDownloadNormal);
    CPPUNIT_TEST(TestDownloadConver);
    CPPUNIT_TEST(TestDownloadNoSavePath);
    CPPUNIT_TEST(TestDownloadErrorURL);
    CPPUNIT_TEST(TestDownloadServerClosed);
	CPPUNIT_TEST(TestDownloadBadNetwork);
	CPPUNIT_TEST(TestDownloadCloseServer);

	//CPPUNIT_TEST(TestDownload);//手动测试
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();

	//正常下载一个文件
	void TestDownloadNormal();

	//覆盖下载（已经存在相同名字的文件）
	void TestDownloadConver();

	//测试保存路径不存在的情况
	void TestDownloadNoSavePath();

	//测试下载地址异常,URL不可达
	void TestDownloadErrorURL();

	//测试服务器没有启动的情况
	void TestDownloadServerClosed();

	//测试网络不流畅、多路由的情况（下载国外网址的文件）
	void TestDownloadBadNetwork();
	
	//没有下载完就关闭服务器
	void TestDownloadCloseServer();

	//正常下载一个文件
	void TestDownload();

private:
	int _OpenServer();
	int _CloseServer();

private:
	STARTUPINFO m_ServerStartupInfo;					//http服务器启动信息
	STARTUPINFO m_WinInetDownloaderStartupInfo;			//TestWinInetDownloader进程启动信息
	PROCESS_INFORMATION m_ServerProcessInfo;			//http服务器进程信息结构体
	PROCESS_INFORMATION m_WinInetDownloaderProcessInfo;	//GameUpdater进程信息结构体
};
#endif ///_KTestWinInetDownloader_H__