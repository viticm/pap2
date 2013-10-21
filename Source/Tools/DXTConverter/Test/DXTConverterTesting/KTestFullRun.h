//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KTestFullRun.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-4-23 16:00
//  Comment     :   验证完整运行TGA转DDS后的文件是否正确。
//					程序启动时读取指定目录下的所有文件的文件名并应用与工具相同的规则对文件进行分类，
//					并记录文件名的索引，用于工具转换完成后的验证。
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KTestFullRun_H__
#define _KTestFullRun_H__

#include "stdafx.h"
#include "cppunit\extensions\HelperMacros.h"

class KTestFullRun : public CppUnit::TestFixture
{
public:
	CPPUNIT_TEST_SUITE(KTestFullRun);
	CPPUNIT_TEST(TestConvertedFiles);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp();
	void tearDown();
	void TestConvertedFiles();

private:
	//加载配置文件
	int LoadConfigFile();
	//加载文件数据，用于分类筛选和结果验证
	int LoadFileData();
	//从文件中获取过滤或处理方式的要求
	int LoadMapNames();
	int LoadSpecialConvertorFiles();
	int LoadFilterFiles();

	//循环对配置文件中CheckPath里的目录进行遍历TGA和DDS贴图文件
	int FindCheckFiles();
	//遍历一个目录下的TGA和DDS贴图文件，并对每个文件做分析和过滤
	int FindTextureFile(const char cszFilePath[]);
	
	//分析文件
	int AnalyseTGAFile(const char cszFilePath[], size_t uLen);
	int AnalyseDDSFile(const char cszFilePath[], size_t uLen);
	//文件的筛选分类规则
	int IsSpecialConvertFile(const UINT uIndex);
	int IsFilterMapFile(const UINT uIndex);
	int IsFilterFile(const UINT uIndex);
	int IsTexture(const UINT uIndex);
	
	//启动转换工具进行转换
	int RunDXTConverter();

	//验证转换后的文件
	int ValidateFiles();
	int ValidateNoConvertedFiles();
	int ValidateConvertedFiles();
	int ValidateDDSFiles();

	/////////////////////////////////////////////////////////////////////////
	//辅助函数
	//查找子串
	int SearchSubStr(
		const char cszSource[], size_t uSourceLen,
		const char cszSearch[], size_t uSearchSLen
		);
	//格式化目录
	int FormatPath(char* pszPath, size_t uSize);
	//输出
	int Output();
	/////////////////////////////////////////////////////////////////////////
private:
	//特殊转换文件ConvertorFiles.tab中的配置内容
	typedef struct _SpecialConvertFile 
	{
		char szFileName[MAX_PATH];
		UINT nDxt;
	}SpecialConvertFile;

	//用于验证特殊转换后的转换结果
	typedef struct _SpecialConvertFileForValidate
	{
		UINT nFileIndex;
		UINT nDxt;
	}SpecialConvertFileForValidate;

	//文件路径
	char m_szMapListPath[MAX_PATH];
	char m_szFilterTableName[MAX_PATH];
	char m_szSpecialConvertTableName[MAX_PATH];
	char m_szApplicationPath[MAX_PATH];
	char m_szConfigFile[MAX_PATH];
	//所有文件
	vector<char*> m_vAllFileList;
	//源数据，组成筛选分类规则
	vector<char*> m_vCheckPathList;
	vector<char*> m_vMapName;
	vector<char*> m_vFilterFileAndPath;
	vector<SpecialConvertFile> m_vSpecialConvertFiles;
	//分析后的数据，用于验证转换结果
	vector<SpecialConvertFileForValidate> m_vSpecialConvertFilesForValidate;
	vector<UINT> m_vDefaultConvertFileList;
	vector<UINT> m_vFilterFileList;
	vector<UINT> m_vErrorFileList;
	vector<UINT> m_vTGAFileList;
	vector<UINT> m_vDDSFileList;
};

#endif ///KTestFullRun
