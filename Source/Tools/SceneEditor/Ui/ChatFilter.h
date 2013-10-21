// ChatFilter.h: interface for the CChatFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHATFILTER_H__4D8459D3_39B0_454E_95B2_7B1D91C10B36__INCLUDED_)
#define AFX_CHATFILTER_H__4D8459D3_39B0_454E_95B2_7B1D91C10B36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilterTextLib.h"

class CChatFilter  
{
public:
	CChatFilter();
	virtual ~CChatFilter();

public:
	BOOL Initialize();
	BOOL Uninitialize();

	BOOL IsTextPass(LPCTSTR text);

private:
	ITextFilter* m_pTextFilter;
};

#endif // !defined(AFX_CHATFILTER_H__4D8459D3_39B0_454E_95B2_7B1D91C10B36__INCLUDED_)
