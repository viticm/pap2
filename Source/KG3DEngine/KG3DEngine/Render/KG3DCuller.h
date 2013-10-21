////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCuller.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-14 10:05:19
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DCULLER_H_
#define _INCLUDE_KG3DCULLER_H_

class KG3DRepresentObject;
////////////////////////////////////////////////////////////////////////////////
struct KG3DCuller 
{
	virtual BOOL Cull(KG3DRepresentObject& Obj) = 0;
	virtual ~KG3DCuller() = 0{}
};

#endif //_INCLUDE_KG3DCULLER_H_