#include "StdAfx.h"
#include "KG3DReport.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DReport g_cReport;

KG3DReport::KG3DReport(void)
{
}

KG3DReport::~KG3DReport(void)
{
}

HRESULT KG3DReport::AddReport(LPCTSTR pReport)
{
	m_vecReport.push_back(pReport);
	return S_OK;
}

int KG3DReport::GetNumReport()
{
	return (int)m_vecReport.size();
}

HRESULT KG3DReport::GetReport(int nIndex,LPTSTR pReport)
{
	wsprintf(pReport,"%s",m_vecReport[nIndex].c_str());
	return S_OK;
}

