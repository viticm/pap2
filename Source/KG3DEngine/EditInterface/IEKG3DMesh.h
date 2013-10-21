////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DMesh.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-14 11:47:33
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DMESH_H_
#define _INCLUDE_IEKG3DMESH_H_

////////////////////////////////////////////////////////////////////////////////
interface IEKG3DMesh
{
    virtual HRESULT GetNumBones(INT *pnRetNum) = 0;

    virtual HRESULT GetBaseBoneIDVectorSize(DWORD *pdwSize) = 0;
    virtual HRESULT GetBaseBoneIDVectorItem(DWORD dwPosition, INT *pBaseBoneID) = 0;

    virtual HRESULT GetBoneInfoName(INT nBaseBoneID, LPCTSTR *ppszName) = 0;

    virtual HRESULT GetBoneInfoChildVectorSize(DWORD dwBoneInfoPosition, DWORD *pdwSize) = 0;
    virtual HRESULT GetBoneInfoChildVectorItem(DWORD dwBoneInfoPosition, DWORD dwChildPosition, INT *pnID) = 0;

    virtual HRESULT SetBoneInfoScale(DWORD dwBoneInfoPosition, FLOAT x, FLOAT y, FLOAT z) = 0;

    virtual HRESULT GetSocketName(DWORD dwPosition, LPCTSTR *ppszName) = 0;
    virtual HRESULT SetSocketName(DWORD dwPosition, LPCTSTR pszName) = 0;
    virtual HRESULT FindSocket(LPCTSTR strSocketName, INT *pnIndex) = 0;
    virtual HRESULT AddSocket(LPCTSTR strParentBoneName, LPCTSTR strSocketName) = 0;
    virtual HRESULT RemoveSocket(LPCTSTR strSocketName) = 0;

    virtual HRESULT GetSocketParentBoneName(DWORD dwPosition, LPCTSTR *ppszParentBoneName) = 0;
    
    virtual HRESULT CreateProgressMesh() = 0;

    virtual HRESULT GetOption(DWORD *pdwOption) = 0;
    virtual HRESULT SetOption(DWORD dwOption) = 0;  

    virtual HRESULT GetSocketCount(DWORD *pdwSocketCount) = 0;

    virtual HRESULT IsMeshSysNotNull(BOOL *pbState) = 0;
    virtual HRESULT GetMeshSysFVF(DWORD *pdwFVF) = 0;
    virtual HRESULT GetMeshSysNumVertices(DWORD *pdwNumVertex) = 0;
    virtual HRESULT LockVertexBufferOfMesfSys(DWORD dwFlag, PVOID *ppBuffer) = 0;
    virtual HRESULT UnLockVertexBufferOfMesfSys() = 0;

    virtual HRESULT HalfSphereNormal(DWORD dwOption) = 0;
    virtual HRESULT SphereNormal() = 0;

    virtual HRESULT GetProgressMeshMinFaces(INT *pnMin) = 0;
    virtual HRESULT GetProgressMeshMaxFaces(INT *pnMax) = 0;

    virtual HRESULT SetNumFaceLOD(INT nNumFace) = 0;
	
	virtual HRESULT ComputeBoneBBoxByIndex(DWORD dwIndex) = 0;
   
	virtual void SetCurSelected(DWORD Index) = 0 ;
	
	virtual void SetBoneBBoxScale(DWORD Index,float& X,float& Y,float& Z) = 0;
	virtual void GetBoneBBoxScale(DWORD Index,float& x,float& y,float& z) = 0;
	virtual HRESULT CreateBBoxOfBone() = 0;

	virtual HRESULT GetName(LPCTSTR *ppszName) = 0;
	virtual HRESULT SavePropertyToIni(LPCTSTR cszFileName) = 0;
	virtual ULONG STDMETHODCALLTYPE Release() = 0;
	virtual HRESULT GetType(DWORD *pdwType) = 0;
	virtual HRESULT GetObjProperty(BYTE **ppbObj) = 0;
	
	virtual HRESULT SavePrograssMeshAsLowMesh(LPSTR pName) = 0;
	virtual HRESULT RefreshByBase() = 0;//根据SYS刷新其他对应的模型
};

#endif //_INCLUDE_IEKG3DMESH_H_
