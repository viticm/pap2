////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DModelST.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-20 9:16:39
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DMODELST_H_
#define _INCLUDE_IEKG3DMODELST_H_

////////////////////////////////////////////////////////////////////////////////

interface IEKG3DModelSet;
class IKG3DModel;
class IEKG3DBip;
interface IEKG3DModelST
{
    virtual HRESULT LoadAnimationFromFile(LPCTSTR strFileName, DWORD dwOption) = 0;
    virtual HRESULT IEChangeModel(DWORD dwIndex, IKG3DModel* pModel) = 0;
    virtual ULONG STDMETHODCALLTYPE Release() = 0;
    virtual HRESULT GetIKG3DModel(IKG3DModel **ppiModel) = 0;
	virtual int GetBindToSocketModel(const char cszSocketName[],vector<IEKG3DModel*> &vecpModel)=0;
	virtual HRESULT GetAllChilds(vector<IEKG3DModel*> &vecpChildsModel)=0;
	virtual HRESULT GetIEBip(IEKG3DBip** pBip) = 0;
	virtual void SetbDrawBigBox(BOOL val) =0;
	virtual BOOL GetbDrawBigBox() = 0;
	virtual HRESULT ComputeMeshBigBox() = 0;
};
#endif //_INCLUDE_IEKG3DMODELST_H_
