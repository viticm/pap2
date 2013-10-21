////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGDesignPatternWidgets.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-4 15:42:25
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGDESIGNPATTERNWIDGETS_H_
#define _INCLUDE_KGDESIGNPATTERNWIDGETS_H_

////////////////////////////////////////////////////////////////////////////////
struct KGCommand 
{
	virtual HRESULT Execute() = 0;
	virtual ~KGCommand() = 0{}
};

struct KGCommandDummy 
{
	virtual HRESULT Execute(){}
	virtual ~KGCommandDummy(){}
};

#endif //_INCLUDE_KGDESIGNPATTERNWIDGETS_H_
