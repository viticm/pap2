//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KG3DTestLoadMesh.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-8-11 10:00
//  Comment     :   加载Mesh数据，记录加载失败的Mesh名称，输出结果
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KG3DTestLoadMesh_H__
#define _KG3DTestLoadMesh_H__

#include "KGTestCommon.h"
#include "KG3DInterface.h"
#include "KG3DAutoTestInterface.h"

class KG3DTestLoadMesh
{
public:
	KG3DTestLoadMesh(void);
	~KG3DTestLoadMesh(void);

public:
	int  Init(IKG3DAutoTestInterface* pTestInterface,
			  const char cszResultPath[]);	
	void TestLoadOneMeshFile(const char cszFileName[]);
	int  AnalyseLog();
	int  OutputResult();
	void UnInit();

private:
	vector<string> m_vecLoadFailedMesh;					//记录加载失败的MESH信息
	vector<string> m_vecBSPFailedMesh;					//日志中记录“BSP unmatch Mesh”的Mesh资源信息
	IKG3DAutoTestInterface* m_pTestInterface;
	KG3DMeshTable*		    m_pMeshTable;
	char m_szApplicationPath[MAX_PATH];
	char m_szLogFileName[MAX_PATH];
	char m_szResultPath[MAX_PATH];
};

#endif