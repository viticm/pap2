////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DCollocator.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-12-3 17:07:35
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DCOLLOCATOR_H_
#define _INCLUDE_IEKG3DCOLLOCATOR_H_

////////////////////////////////////////////////////////////////////////////////

namespace KG_CUSTOM_HELPERS
{
	struct StrPathSplited;
};

interface IEKG3DCollocator 
{
	virtual HRESULT GetSavePath(DWORD dwType, LPCTSTR scenePath	//dwType是EM_SAVE_OP_ENVIRONMENT等，ScenePath是场景保存的路径
		, KG_CUSTOM_HELPERS::StrPathSplited* pRetPathSplited	//可以为NULL。返回分裂的路径，得到盘符和路径都很方便
		, LPTSTR pathRet, size_t uPathBufferSize) = 0;	//pathRet可以为NULL，返回完整的路径
};

#endif //_INCLUDE_IEKG3DCOLLOCATOR_H_
