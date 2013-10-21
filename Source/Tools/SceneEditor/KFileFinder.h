// KFileFinder.h: interface for the KFileFinder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KFILEFINDER_H__771D9024_20F2_4B13_A258_EBC0C850ADF8__INCLUDED_)
#define AFX_KFILEFINDER_H__771D9024_20F2_4B13_A258_EBC0C850ADF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class KFileFinder 
{
public:
	vector<string>m_vecPath;

public:
	virtual HRESULT AddPath(LPSTR Path,BOOL IncludeSub);
	virtual HRESULT FindFile(LPSTR pOutName,LPSTR pInName);

	KFileFinder();
	virtual ~KFileFinder();
};

#endif // !defined(AFX_KFILEFINDER_H__771D9024_20F2_4B13_A258_EBC0C850ADF8__INCLUDED_)
