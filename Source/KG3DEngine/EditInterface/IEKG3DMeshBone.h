////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DMeshBone.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-6 14:10:03
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DMESHBONE_H_
#define _INCLUDE_IEKG3DMESHBONE_H_

////////////////////////////////////////////////////////////////////////////////
interface IEKG3DMeshBone
{
    virtual HRESULT GetSelBoneName(TCHAR szName[], INT nLen, DWORD *pdwType) = 0;
    virtual HRESULT SetSelectedBone(const TCHAR cszName[]) = 0;
};

#endif //_INCLUDE_IEKG3DMESHBONE_H_
