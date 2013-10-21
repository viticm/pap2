////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : HeaderAnalysor.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-6-2 14:30:43
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_HEADERANALYSOR_H_
#define _INCLUDE_HEADERANALYSOR_H_

////////////////////////////////////////////////////////////////////////////////
struct HeaderAnalysor
{
	virtual BOOL GetHeaderFileNameList(LPCTSTR strFileFullPath, std::vector<CString>* pFileNames, CString* pError) = 0;
	virtual ~HeaderAnalysor() = 0{}
};
HeaderAnalysor* CreateDefHeaderAnalysor();
#endif //_INCLUDE_HEADERANALYSOR_H_
