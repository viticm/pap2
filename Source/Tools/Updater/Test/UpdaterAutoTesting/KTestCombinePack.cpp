
#include "stdafx.h"
#include "KTestCombinePack.h"
#include "KFileMD5Maker.h"
#include "updatepublic.h"

const TCHAR COMBINE_ONCE_MD5[]			= _T("4131f941b10dcd32d3860af141ae50cb");	//将TestPack_2合并到TestPack_1所得到TestPack_1的MD5
const TCHAR COMBINE_SEVERAL_TIMES_MD5[] = _T("d1db4ce5a005aebe1e4722671d6b18f6");	//将TestPack_2循环10次合并到TestPack_1所得到TestPack_1的MD5
const int COMBINE_TIMES = 10;		//测试循环合并的次数

void KTestCombinePack::setUp()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = _sntprintf(m_szTestPack1, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, TEST_PACK_1);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	m_szTestPack1[MAX_PATH - 1] = _T('\0');
	nRetCode = KTestCommon::PrepareTestResoure(TEST_PACK_2);
	KGLOG_PROCESS_ERROR(nRetCode);
	nRetCode = _sntprintf(m_szTestPack2, MAX_PATH, _T("%s\\%s"), g_szTestingSpaceDir, TEST_PACK_2);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	m_szTestPack2[MAX_PATH - 1] = _T('\0');

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestCombinePack::tearDown()
{
	int nResult  = false;
	int nRetCode = false;

	nRetCode = KTestCommon::CleanPath(g_szTestingSpaceDir);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestCombinePack::TestCombineOnce()
{
	int nResult  = false;
	int nRetCode = false;

	//合并1个包，m_szTestPack2合并到m_szTestPack1
	nRetCode = PackComb(m_szTestPack1, m_szTestPack2);
	KGLOG_PROCESS_ERROR(nRetCode);
	//验证合包后的结果
	nRetCode = KTestCommon::ValidateFileMD5(m_szTestPack1, COMBINE_ONCE_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestCombinePack::TestCombineSeveralTimes()
{
	int nResult  = false;
	int nRetCode = false;

	//循环合并10个包，m_szTestPack2合并到m_szTestPack1
	for (int nCount = 0; nCount < COMBINE_TIMES; nCount++)
	{
		nRetCode = PackComb(m_szTestPack1, m_szTestPack2);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	//验证合包后的结果
	nRetCode = KTestCommon::ValidateFileMD5(m_szTestPack1, COMBINE_SEVERAL_TIMES_MD5);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}