////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : DirState.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-8-6 11:45:06
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_DIRSTATE_H_
#define _INCLUDE_DIRSTATE_H_

////////////////////////////////////////////////////////////////////////////////

namespace DirState
{
	BOOL	SaveDirState(LPCTSTR strDirStateFile, const TypeShaderFileCt& shaderFileCt);
	BOOL	LoadDirStateCheckUnchanged(LPCTSTR strDirStateFile, const TypeShaderFileCt& shaderFileCt);//返回True表示没有更改
};


#endif //_INCLUDE_DIRSTATE_H_
