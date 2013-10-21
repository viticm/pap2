#pragma once

#include "KG3DInterface.h"
class IKG3DResourceManager;
class D3DXQUATERNION;
class D3DXVECTOR3;

interface IEKG3DRepresentObject : public IKG3DResourceBase
{
    // for npc 
    virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption) PURE;
    virtual HRESULT SetModelFace(LPCTSTR pMeshFile,LPCTSTR pMtlFile,LPCTSTR pAniFile,LPCTSTR pSocketName) PURE;
    virtual HRESULT PlayAnimation(LPCSTR szAniPath) PURE;

    // for group
    virtual HRESULT ResetGroupPose() = 0;

	virtual HRESULT LoadFromIniFile(LPCTSTR pFileName,DWORD dwOption) = 0;
	virtual HRESULT SaveToIniFile(LPCTSTR pFileName,DWORD dwOption) = 0;
	virtual HRESULT GetModelCount(INT* pValue) = 0;

	virtual HRESULT GetModelFileName(int nIndex,LPSTR pMeshFile,LPSTR pMtlFile,LPSTR pAniName) = 0;
	virtual HRESULT GetName(LPTSTR pName, size_t BufferSize) = 0;

    virtual HRESULT GetModel(IKG3DModel*& pModel, DWORD dwIndex) = 0;
    virtual HRESULT SetModel(DWORD dwIndex,
        const char pcszMeshFileName[],
        const char pcszMtlFileName[],
        const char pcszAniFileName[],BOOL bForceLoad) = 0;
   
	virtual HRESULT SetTranslation(const D3DXVECTOR3* pTrans) = 0;
	virtual HRESULT SetRotation(const D3DXQUATERNION* pRotaion) = 0;
	virtual HRESULT SetScaling(const D3DXVECTOR3* pScale) = 0;

	virtual HRESULT GetAnotherIEInterface(DWORD dwType, VOID** ppAnotherInterface) = 0;//如把IERepresentObject变成IERepresentObjectSet, Type可用来判断

	virtual HRESULT SetDynamicObstacleGroupIndex(int nIndex) = 0;//动态障碍
};

interface IEKG3DRepresentObjectSet
{
    virtual HRESULT IELoadFromIniFile(LPCTSTR pFileName,DWORD dwOption) = 0;
	virtual HRESULT SetCurrentModel(INT nIndex,BOOL bForceLoad) = 0;
	virtual INT GetNumModelofSet() = 0;
	virtual HRESULT GetModelInfo(int nIndex,LPSTR pMeshName,LPSTR pMtlName,LPSTR pAniName) = 0;
	virtual HRESULT AddModelInfo(int& nIndex,LPSTR pMeshName,LPSTR pMtlName,LPSTR pAniName) = 0;
	virtual HRESULT DeleteModelInfo(int nIndex) =0;
	virtual HRESULT GetIERepresentObject(IEKG3DRepresentObject** pObj) =0;
};

interface IEKG3DRepresentObjectSoundBall
{
	virtual HRESULT LoadFromSoundBallDataFile(LPCTSTR pFileName) = 0;
	virtual void Stop() = 0;
	virtual void Start() = 0;
};

interface IEKG3DRepresentObjectVisibleEntitySet
{
	virtual HRESULT LoadEdgeModel(LPSTR pModelName) = 0;
	virtual DWORD   GetInsideLayerID() = 0;
	virtual DWORD   GetOutsideLayerID() =0;
	virtual HRESULT SetInsideLayerID(DWORD dwID) = 0;
	virtual HRESULT SetOutsideLayerID(DWORD dwID) = 0;
	virtual DWORD   GetSetDepth() = 0;
	virtual HRESULT SetSetDepth(DWORD dwDepth) = 0;
};

//HRESULT GetRepresentObjectTable(IKG3DResourceManager** ppObjectTable);//转移到EngineManager接口中