// FilterTextLib.cpp: implementation of the CFilterTextLib class.
//
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "FilterTextLib.h"
#include "../ErrorCode.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFilterTextLib::CFilterTextLib()
	//: m_hFilterText(NULL), m_lpfnCreateTextFilter(NULL)
{

}

CFilterTextLib::~CFilterTextLib()
{

}


BOOL CFilterTextLib::Initialize()
{
/*	Error_SetErrorCode("CFilterTextLib::Initialize()");
	if (m_hFilterText)
	{
		return TRUE;
	}

	if (!(m_hFilterText = ::LoadLibrary("FilterText.dll")))
	{
		Error_SetErrorCode(ERR_T_LOAD_MODULE_FAILED);
		Error_SetErrorString("FilterText.dll");
		return FALSE;
	}

	if (!(m_lpfnCreateTextFilter = (LPFNCREATETEXTFILTER)GetProcAddress(m_hFilterText, "CreateTextFilter")))
	{
		Error_SetErrorCode(ERR_T_MODULE_UNCORRECT);
		Error_SetErrorString("FilterText.dll");
		return FALSE;
	}
*/
	return TRUE;
}

BOOL CFilterTextLib::Uninitialize()
{
/*
	m_lpfnCreateTextFilter = NULL;

	if (m_hFilterText)
	{
		::FreeLibrary(m_hFilterText);
		m_hFilterText = NULL;
	}
*/
	return TRUE;
}


HRESULT CFilterTextLib::CreateTextFilter(ITextFilter** ppTextFilter)
{
/*
	if (!m_lpfnCreateTextFilter)
		return -1;

	return m_lpfnCreateTextFilter(ppTextFilter);
*/
	return ::CreateTextFilter(ppTextFilter);
}
