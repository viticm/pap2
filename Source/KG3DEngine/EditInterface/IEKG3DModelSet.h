////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DModelSet.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-6 16:22:10
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DMODELSET_H_
#define _INCLUDE_IEKG3DMODELSET_H_

////////////////////////////////////////////////////////////////////////////////
class IEKG3DBip;
class IEKG3DModel;
interface IEKG3DModelSet
{
    virtual HRESULT GetModelsCount(DWORD *pdwCount) = 0;
    virtual HRESULT GetModel(
        DWORD dwPosition, 
        IEKG3DModel **ppModel
    ) = 0;
    virtual HRESULT GetIEBip(IEKG3DBip **ppiBip) = 0;
	virtual int GetBindToSocketModel(const char cszSocketName[],vector<IEKG3DModel*> &vecpModel) = 0;
};

#endif //_INCLUDE_IEKG3DMODELSET_H_
