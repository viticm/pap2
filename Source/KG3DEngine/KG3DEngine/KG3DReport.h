#pragma once

class KG3DReport
{
public:	
	vector<string>m_vecReport;

	HRESULT AddReport(LPCTSTR pReport);
	int GetNumReport();
	HRESULT GetReport(int nIndex,LPTSTR pReport);

	KG3DReport(void);
public:
	virtual ~KG3DReport(void);
};
extern KG3DReport g_cReport;