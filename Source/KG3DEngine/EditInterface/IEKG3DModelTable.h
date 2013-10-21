////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DModelTable.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-12-5 10:08:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DMODELTABLE_H_
#define _INCLUDE_IEKG3DMODELTABLE_H_

interface IEKG3DModel;
class IKG3DResourceBase;
////////////////////////////////////////////////////////////////////////////////
interface IEKG3DModelTable
{
    virtual HRESULT IELoadResourceFromFile(
        const TCHAR cszFileName[],
        UINT uFileNameHash,
        UINT uOption,
        IKG3DResourceBase **ppRetResource
    ) = 0;
    
    virtual HRESULT IELoadModelFromFile(
        const TCHAR cszFileName[],
        UINT uFileNameHash,
        UINT uOption,
        IEKG3DModel **ppRetResource
    ) = 0;

    virtual HRESULT IEGet1NewResourse(IEKG3DModel **ppModel, DWORD dwType) = 0;


};
#endif //_INCLUDE_IEKG3DMODELTABLE_H_
