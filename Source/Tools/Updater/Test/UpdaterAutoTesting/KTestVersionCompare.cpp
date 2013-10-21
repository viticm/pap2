#include "stdafx.h"
#include "KTestVersionCompare.h"
#include "KVersionNumber.h"

//测试数据
const TCHAR DOT_HIGH_VERSION[]		= _T("1.0.8.1341");		//点分隔类型
const TCHAR DOT_LOW_VERSION[]		= _T("1.0.8.1300");
const TCHAR COMMA_HIGH_VERSION[]	= _T("1,0,8,1341");		//逗号分隔类型
const TCHAR COMMA_LOW_VERSION[]		= _T("1,0,8,1300");
const TCHAR HYPHEN_HIGH_VERSION[]	= _T("1-0-8-1341");		//横杠分隔类型
const TCHAR HYPHEN_LOW_VERSION[]	= _T("1-0-8-1300");

void KTestVersionCompare::setUp()
{
}
void KTestVersionCompare::tearDown()
{
}

void KTestVersionCompare::TestVersionEqual_01()
{
	int nResult  = false;
	int nRetCode = false;

	KVersionNumber DotVersion1(DOT_HIGH_VERSION);		//1.0.8.1341
	KVersionNumber DotVersion2(DOT_LOW_VERSION);		//1.0.8.1300
	KVersionNumber COMMAVersion1(COMMA_HIGH_VERSION);	//1,0,8,1341
	KVersionNumber COMMAVersion2(COMMA_LOW_VERSION);	//1,0,8,1300
	KVersionNumber HyphenVersion1(HYPHEN_HIGH_VERSION);	//1-0-8-1341
	KVersionNumber HyphenVersion2(HYPHEN_LOW_VERSION);	//1-0-8-1300

	nRetCode = (DotVersion1 == DotVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nRetCode = (COMMAVersion1 == COMMAVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nRetCode = (HyphenVersion1 == HyphenVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestVersionCompare::TestVersionEqual_02()
{
	int nResult  = false;
	int nRetCode = false;

	KVersionNumber DotVersion1(DOT_LOW_VERSION);		//1.0.8.1300
	KVersionNumber DotVersion2(DOT_HIGH_VERSION);		//1.0.8.1341
	KVersionNumber COMMAVersion1(COMMA_LOW_VERSION);	//1,0,8,1300
	KVersionNumber COMMAVersion2(COMMA_HIGH_VERSION);	//1,0,8,1341
	KVersionNumber HyphenVersion1(HYPHEN_LOW_VERSION);	//1-0-8-1300
	KVersionNumber HyphenVersion2(HYPHEN_HIGH_VERSION);	//1-0-8-1341

	nRetCode = (DotVersion1 == DotVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nRetCode = (COMMAVersion1 == COMMAVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nRetCode = (HyphenVersion1 == HyphenVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestVersionCompare::TestVersionEqual_03()
{
	int nResult  = false;
	int nRetCode = false;

	KVersionNumber DotVersion1(DOT_HIGH_VERSION);		//1.0.8.1341
	KVersionNumber DotVersion2(DOT_HIGH_VERSION);		//1.0.8.1341
	KVersionNumber COMMAVersion1(COMMA_HIGH_VERSION);	//1,0,8,1341
	KVersionNumber COMMAVersion2(COMMA_HIGH_VERSION);	//1,0,8,1341
	KVersionNumber HyphenVersion1(HYPHEN_HIGH_VERSION);	//1-0-8-1341
	KVersionNumber HyphenVersion2(HYPHEN_HIGH_VERSION);	//1-0-8-1341

	nRetCode = (DotVersion1 == DotVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == TRUE);

	nRetCode = (COMMAVersion1 == COMMAVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == TRUE);

	nRetCode = (HyphenVersion1 == HyphenVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == TRUE);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestVersionCompare::TestVersionGreater_01()
{
	int nResult  = false;
	int nRetCode = false;

	KVersionNumber DotVersion1(DOT_HIGH_VERSION);		//1.0.8.1341
	KVersionNumber DotVersion2(DOT_LOW_VERSION);		//1.0.8.1300
	KVersionNumber COMMAVersion1(COMMA_HIGH_VERSION);	//1,0,8,1341
	KVersionNumber COMMAVersion2(COMMA_LOW_VERSION);	//1,0,8,1300
	KVersionNumber HyphenVersion1(HYPHEN_HIGH_VERSION);	//1-0-8-1341
	KVersionNumber HyphenVersion2(HYPHEN_LOW_VERSION);	//1-0-8-1300

	nRetCode = (DotVersion1 > DotVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == TRUE);

	nRetCode = (COMMAVersion1 > COMMAVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == TRUE);

	nRetCode = (HyphenVersion1 > HyphenVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == TRUE);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestVersionCompare::TestVersionGreater_02()
{
	int nResult  = false;
	int nRetCode = false;

	KVersionNumber DotVersion1(DOT_LOW_VERSION);		//1.0.8.1300
	KVersionNumber DotVersion2(DOT_HIGH_VERSION);		//1.0.8.1341
	KVersionNumber COMMAVersion1(COMMA_LOW_VERSION);	//1,0,8,1300
	KVersionNumber COMMAVersion2(COMMA_HIGH_VERSION);	//1,0,8,1341
	KVersionNumber HyphenVersion1(HYPHEN_LOW_VERSION);	//1-0-8-1300
	KVersionNumber HyphenVersion2(HYPHEN_HIGH_VERSION);	//1-0-8-1341

	nRetCode = (DotVersion1 > DotVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nRetCode = (COMMAVersion1 > COMMAVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nRetCode = (HyphenVersion1 > HyphenVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}

void KTestVersionCompare::TestVersionGreater_03()
{
	int nResult  = false;
	int nRetCode = false;

	KVersionNumber DotVersion1(DOT_HIGH_VERSION);		//1.0.8.1341
	KVersionNumber DotVersion2(DOT_HIGH_VERSION);		//1.0.8.1341
	KVersionNumber COMMAVersion1(COMMA_HIGH_VERSION);	//1,0,8,1341
	KVersionNumber COMMAVersion2(COMMA_HIGH_VERSION);	//1,0,8,1341
	KVersionNumber HyphenVersion1(HYPHEN_HIGH_VERSION);	//1-0-8-1341
	KVersionNumber HyphenVersion2(HYPHEN_HIGH_VERSION);	//1-0-8-1341

	nRetCode = (DotVersion1 > DotVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);
	
	nRetCode = (COMMAVersion1 > COMMAVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nRetCode = (HyphenVersion1 > HyphenVersion2);
	KGLOG_PROCESS_ERROR(nRetCode == false);

	nResult = true;
Exit0:
	CPPUNIT_ASSERT(nResult);
}