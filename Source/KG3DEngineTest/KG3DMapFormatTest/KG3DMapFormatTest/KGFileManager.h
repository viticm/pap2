//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGFileManager.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-9-30 10:00
//  Comment     :   文件操作类，用于搜索目录下的文件和文件比较
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGFileManager_H__
#define _KGFileManager_H__

#include "KGTestCommon.h"

class KGFileManager
{
public:
	static KGFileManager* Instance();
private:
	KGFileManager(void);
	KGFileManager(const KGFileManager& object);
	KGFileManager& operator =(const KGFileManager& object);
	~KGFileManager(void);

public:
	//搜索指定目录下的所有指定类型的文件名称，多种类型可以以|隔开，如.mesh|.pvs
	int  SearchFiles(const char cszPath[], vector<string>* pvecFileName, const char cszExt[] = NULL);
	//比较指定两个目录下的所有指定类型的文件，二进制比较方式，多种类型可以以|隔开，如.mesh|.pvs
	int  ComparePathFiles(const char cszPath_1[], const char cszPath_2[], const char cszExt[] = NULL);
	//比较指定两个文件，二进制比较方式
	int  CompareFileByDataStream(const char cszFileName_1[], const char cszFileName_2[]);
	//复制指定目录下的所有文件和文件夹到某个目录下，采用XCopy命令
	int  CopyPathFiles(const char cszSourcePath[], const char cszDestPath[]);

private:
	int  SearchFilesFromPath(const char cszPath[]);

private:
	vector<string> m_vecFileExt;
	vector<string> m_vecFoundFilesList;
	static KGFileManager m_Instance;
};

#endif