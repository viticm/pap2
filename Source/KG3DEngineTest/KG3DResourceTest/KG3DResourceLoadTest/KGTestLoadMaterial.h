//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGTestLoadMaterial.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-15 10:00
//  Comment     :   加载材质数据并分析加载后的日志中记录的错误信息，输出分析结果
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KGTestLoadMaterial_H__
#define _KGTestLoadMaterial_H__

#include "KGTestCommon.h"
#include "KG3DInterface.h"
#include "KG3DAutoTestInterface.h"

using namespace std;

class KGTestLoadMaterial
{
public:
	KGTestLoadMaterial();
	~KGTestLoadMaterial();
	
public:
	int  Init(IKG3DAutoTestInterface* pTestInterface,
			  const char cszResultPath[]);
	void TestLoadOneMtlFile(const char cszFileName[]);	
	int  AnalyseLog();
	int  OutputResult();
	void UnInit();

private:
	/*检查材质文件是否需要过滤
		return 
		true:不需要过滤
		false:需要过滤
	*/
	int  CheckFilter(const char cszMtlFile[], const char cszTexFile[]);



	//抽取子串
	//例：GetSubStr("abcdefg", "a", "g", pszSub, cuSize); -> pszSub = "bcdef"
	int  GetSubStr(const char cszSource[],
				   const char cszStart[],
				   const char cszEnd[],
				   char* pszSub,
				   const size_t cuSize);

private:
	enum						//加载贴图的附加信息
	{						
		NO_EXIST,				//找不到该贴图文件
		LOADING_ERROR,			//加载贴图过程中出错
		NO_ANY_TEX,				//没有任何贴图
		//在此之前添加新的附加信息类型
		COUNT					//附加信息数量
	};
	typedef map<string, int> TextureInfoMap;			//string:贴图文件名 int:加载贴图的附加信息
	typedef map<string, TextureInfoMap> MtlFileInfoMap; //材质文件和对应的贴图文件信息

	MtlFileInfoMap m_mapErrorMtlFileInfo;				//记录加载材质文件相关的贴图文件错误信息
	vector<string> m_vecLoadFailedMtl;					//记录加载失败的材质信息
	vector<string> m_vecFilterMtl;						//记录过滤的材质文件
	IKG3DAutoTestInterface* m_pTestInterface;
	KG3DMaterialTable*		m_pMtlTable;
	char m_szApplicationPath[MAX_PATH];
	char m_szLogFileName[MAX_PATH];
	char m_szResultPath[MAX_PATH];


};
#endif