#include "KG3DTestLoadMesh.h"
#include "KG3DMesh.h"
#include "KG3DMeshTable.h"

const char RESULT_LOAD_MESH[]    = "Result_Load_Mesh.txt";	//Mesh文件加载失败的记录输出文件
const char RESULT_BSP_MESH[]	 = "Result_Bsp_Mesh.txt";	//Mesh文件加载对应的BSP文件失败的记录

KG3DTestLoadMesh::KG3DTestLoadMesh(void)
{
	m_pTestInterface = NULL;
	m_pMeshTable	 = NULL;

	m_szApplicationPath[0] = '\0';
	m_szLogFileName[0]	   = '\0';
	m_szResultPath[0]	   = '\0';
	m_vecLoadFailedMesh.clear();
}

KG3DTestLoadMesh::~KG3DTestLoadMesh(void)
{
	UnInit();
}

int KG3DTestLoadMesh::Init(IKG3DAutoTestInterface* pTestInterface,
						   const char cszResultPath[])
{
	int nResult  = false;
	int nRetCode = false;
	char* pszResult = NULL;

	KG_ASSERT_EXIT(pTestInterface);
	_ASSERTE(cszResultPath);

	m_pTestInterface = pTestInterface;
	m_pTestInterface->AddRef();
	m_pMeshTable = m_pTestInterface->GetMeshTable();

	nRetCode = strncpy_s(m_szResultPath, sizeof(m_szResultPath), cszResultPath, strlen(cszResultPath));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	KGLogPrintf(KGLOG_INFO, "Ready to Test Load Mesh");

	nResult = true;
Exit0:
	if (!nResult)
	{
		UnInit();
	}
	return nResult;
}

void KG3DTestLoadMesh::TestLoadOneMeshFile(const char cszFileName[])
{
	HRESULT hrRetCode = E_FAIL;
	KG3DMesh* pMesh = NULL;

	_ASSERTE(cszFileName);
	hrRetCode = m_pMeshTable->LoadResourceFromFile(cszFileName, 0, 0, (IKG3DResourceBase**)&pMesh);
	if (FAILED(hrRetCode) || pMesh == NULL)
	{
		m_vecLoadFailedMesh.push_back(cszFileName);
	}
	if (pMesh)
	{
		//这里要注意加参数RELEASE_ATONCE，以便立即释放资源，默认是放入释放队列（RELEASE_INQUEUE）
		//否则内存中会一直占用释一定数量的资源，数量等于释放资源队列的大小
		hrRetCode = m_pMeshTable->ReleaseResource((KG3DMesh*)pMesh, RELEASE_ATONCE);
		if (FAILED(hrRetCode))
		{
			KGLogPrintf(KGLOG_ERR, "Release Mtl Failed!");
		}
		else
		{
			pMesh = NULL;
		}
	}
}

int KG3DTestLoadMesh::OutputResult()
{
	int nResult  = false;
	int nRetCode = false;
	char szResultFile[MAX_PATH] = {0};
	char* pPath = NULL;
	FILE* pResultFile = NULL;
	string strWrite;

	{//输出加载失败的Mesh记录到文件
		nRetCode = _snprintf_s(
			szResultFile,
			sizeof(szResultFile) - 1,        
			"%s%s",
			m_szResultPath,
			RESULT_LOAD_MESH
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);

		pResultFile = fopen(szResultFile, "w");
		KGLOG_PROCESS_ERROR(pResultFile);

		strWrite = "【Load Failed Mesh File Name】\n\n";
		fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
		for (size_t i = 0; i < m_vecLoadFailedMesh.size(); i++)
		{
			strWrite = "\t" + m_vecLoadFailedMesh[i] + "\n";
			fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
		}
		strWrite = string("\n【Total Num】\t") +
			KGTestCommon::IntToString((int)m_vecLoadFailedMesh.size());
		fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);

		fclose(pResultFile);
		pResultFile = NULL;
		szResultFile[0] = '\0';
	}
	{//输出加载BSP失败的Mesh记录到文件
		nRetCode = _snprintf_s(
			szResultFile,
			sizeof(szResultFile) - 1,        
			"%s%s",
			m_szResultPath,
			RESULT_BSP_MESH
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);

		pResultFile = fopen(szResultFile, "w");
		KGLOG_PROCESS_ERROR(pResultFile);

		strWrite = "【BSP unmatch Mesh File Name】\n\n";
		fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
		for (size_t i = 0; i < m_vecLoadFailedMesh.size(); i++)
		{
			strWrite = "\t" + m_vecLoadFailedMesh[i] + "\n";
			fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
		}
		strWrite = string("\n【Total Num】\t") +
			KGTestCommon::IntToString((int)m_vecLoadFailedMesh.size());
		fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);

		fclose(pResultFile);
		pResultFile = NULL;
		szResultFile[0] = '\0';
	}

	nResult = true;
Exit0:
	if (pResultFile)
	{
		fclose(pResultFile);
		pResultFile = NULL;
	}
	return nResult;
}

void KG3DTestLoadMesh::UnInit()
{
	if (m_pTestInterface)
	{
		m_pTestInterface->Release();
		m_pTestInterface = NULL;
	}
	m_pMeshTable = NULL;
	m_vecLoadFailedMesh.clear();
}

int KG3DTestLoadMesh::AnalyseLog()
{
	int nResult  = false;
	int nRetCode = false;
	FILE* pLogFile = NULL;
	char* pResult = NULL;
	char szLine[MAX_LOG_LINE]	  = {0};
	char szNewLogFile[MAX_PATH]	  = {0};
	char szMeshFileName[MAX_PATH] = {0};

	nRetCode = KGTestCommon::FindLogFile(g_szLogPath, m_szLogFileName, sizeof(m_szLogFileName));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _snprintf_s(
		szNewLogFile,
		sizeof(szNewLogFile),
		sizeof(szNewLogFile) - 1,
		"%s.log",
		m_szLogFileName
		);	//*****.log -> *****.log.log
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	nRetCode = ::CopyFile(m_szLogFileName, szNewLogFile, TRUE);
	KGLOG_PROCESS_ERROR(nRetCode);

	pLogFile = fopen(szNewLogFile, "r");
	KGLOG_PROCESS_ERROR(pLogFile);

	while (true)
	{
		pResult = fgets(szLine, sizeof(szLine), pLogFile);
		if (!pResult)
		{
			nRetCode = feof(pLogFile);
			KG_PROCESS_SUCCESS(nRetCode);
			KGLOG_PROCESS_ERROR(nRetCode);
		}
		//D:\JZ_土匪房子002sn.Mesh, BSP unmatch Mesh :(
		nRetCode = KGTestCommon::GetSubStr(szLine, ": ", ", BSP unmatch Mesh", szMeshFileName, sizeof(szMeshFileName));
		if (!nRetCode)
		{
			continue;
		}
		m_vecBSPFailedMesh.push_back(szMeshFileName);
	}

Exit1:
	nResult = true;
Exit0:
	if (pLogFile)
	{
		fclose(pLogFile);
		pLogFile = NULL;
	}
	nRetCode = remove(szNewLogFile);
	if (nRetCode == -1 && szNewLogFile)
	{
		KGLogPrintf(KGLOG_ERR, "remove file faild: %s", szNewLogFile);
	}
	return nResult;
}