////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DSceneEntity.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2006-11-02 15:25:12
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DSCENEENTITY_H_
#define _INCLUDE_IEKG3DSCENEENTITY_H_

////////////////////////////////////////////////////////////////////////////////

interface IEKG3DSceneEntity
{
    virtual HRESULT GetType(SCENEENTITYTYPE *peRetType) = 0;
    virtual HRESULT GetPointer(VOID **ppvRetPointer) = 0;
    virtual HRESULT GetRefData(VOID **ppvRetData) = 0;

    virtual HRESULT GetName(LPTSTR pName, size_t BufferSize) = 0;

    virtual HRESULT GetTranslation(D3DXVECTOR3 *pValue) = 0;
    virtual HRESULT SetTranslation(D3DXVECTOR3 *pValue) = 0;

    virtual HRESULT GetRotation(
        D3DXQUATERNION *pValue, D3DXVECTOR3 *pValueXYZ
    ) = 0;
    virtual HRESULT SetRotation(
        D3DXQUATERNION *pValue, D3DXVECTOR3 *pValueXYZ
    ) = 0;

    virtual HRESULT GetScaling(D3DXVECTOR3* pValue) = 0;
    virtual HRESULT SetScaling(D3DXVECTOR3 *pValue) = 0;   
    virtual void SetScalingRotation(D3DXQUATERNION* pValue) = 0;

    virtual HRESULT UpdateTransformation() = 0;

    virtual HRESULT Reload() = 0;

    virtual HRESULT Compare(IEKG3DSceneEntity *piEntity, BOOL *pbRetEqualFlag) = 0;

    virtual HRESULT RotationY(FLOAT fAngelY) = 0;
    virtual HRESULT RotationX(FLOAT fAngelX) = 0;
    virtual HRESULT RotationZ(FLOAT fAngelZ) = 0;

};
 
#endif //_INCLUDE_IEKG3DSCENEENTITY_H_
