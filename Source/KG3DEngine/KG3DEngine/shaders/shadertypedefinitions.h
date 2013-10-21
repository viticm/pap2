////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : ShaderTypeDefinitions.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-8-15 15:52:14
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_SHADERTYPEDEFINITIONS_H_
#define _INCLUDE_SHADERTYPEDEFINITIONS_H_

////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
typedef D3DXVECTOR4 float4;
typedef D3DXVECTOR3 float3;
typedef D3DXVECTOR2 float2;

typedef D3DXVECTOR3	(float3x3)[3];;	//this is a float 2D array typedef gramma
typedef D3DXVECTOR2	(float2x2)[2];
typedef D3DXMATRIX float4x4;
#endif

#endif //_INCLUDE_SHADERTYPEDEFINITIONS_H_
