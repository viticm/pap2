////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffCommonDef.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-16 16:12:00
//  Comment     : 用于统一各种基本定义，否则出错
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFCOMMONDEF_H_
#define _INCLUDE_KG3DENVEFFCOMMONDEF_H_

////////////////////////////////////////////////////////////////////////////////
#if _KG3D_DEBUG_ENABLE_OUTPUT_DWGC == 1
#if defined(_KG3D_DEBUG_SELF_ENABLE_OUTPUT)
#undef _KG3D_DEBUG_SELF_ENABLE_OUTPUT
#endif
#define _KG3D_DEBUG_SELF_ENABLE_OUTPUT	1
#else

#if defined(_KG3D_DEBUG_SELF_ENABLE_OUTPUT)
#if defined(_KG3D_DEBUG_SELF_ENABLE_OUTPUT)
#undef _KG3D_DEBUG_SELF_ENABLE_OUTPUT
#endif
#endif

#endif

//可以定义的地方Begin////////////////////////////////////////////////////////////////////////

//可以定义的地方End////////////////////////////////////////////////////////////////////////
#else
#pragma message("警告：文件："__FILE__"；行号："__LINE__":重复包含KG3DEnvEffCommonDef.h")
#endif //_INCLUDE_KG3DENVEFFCOMMONDEF_H_
