////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DModelSpeedTree.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2006-11-21 14:06:03
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DMODELSPEEDTREE_H_
#define _INCLUDE_IEKG3DMODELSPEEDTREE_H_

////////////////////////////////////////////////////////////////////////////////
enum 
{
    ENU_SAVE,
    ENU_SAVE_AS,
};
////////////////////////////////////////////////////////////////////////////////

interface IELeaf
{
    virtual HRESULT SetTextureUV(INT nTextureUV) = 0;
    
    virtual HRESULT SetSize(FLOAT fSize) = 0;
    
    virtual HRESULT SetWave(FLOAT fWave) = 0;
    
    virtual HRESULT SetAmbient(D3DCOLORVALUE *pColor) = 0;
    
    virtual HRESULT SetDiffuse(D3DCOLORVALUE *pColor) = 0;
    
    virtual HRESULT GetBoneID(INT *pnBoneID) = 0;
    
    virtual HRESULT GetSelectState(BOOL *pbSelected) = 0;

    virtual HRESULT UpdateLeafOriginPosition() = 0;

};
////////////////////////////////////////////////////////////////////////////////
class KG3DSceneEditorBase;
interface IEKG3DModel;
interface IEKG3DSceneEditorBase;
interface IEKG3DSceneModelEditor;
interface IEKG3DModelSpeedTree
{
    virtual HRESULT GetLeafVectorSize(DWORD *pdwVectorSzie) = 0;
    virtual HRESULT GetLeafVectorItem(DWORD dwIndex, IELeaf **ppiLeaf) = 0;

    virtual HRESULT ChangeLeafTexture(LPCTSTR strFileName) = 0;

    virtual HRESULT GetModelTree(IEKG3DModel **ppiModel) = 0;

    virtual HRESULT GetNeedUpdatePositionState(BOOL *pbNeed) = 0;
    virtual HRESULT SetNeedUpdatePositionState(BOOL bNeed) = 0;
    
    virtual HRESULT FillLeafVerticesBuffer() = 0;

    virtual HRESULT SetLocalBBoxDirty() = 0;

    virtual HRESULT ComputeBBox() = 0;

    //virtual HRESULT DirtyUVBuffer() = 0;

    virtual HRESULT SetNeedDrawBone(BOOL bNeed) = 0;

    virtual HRESULT SetEditMode(BOOL bEditMode, IEKG3DSceneModelEditor* pScene) = 0;

    virtual HRESULT CreateAuto(int nLeaf, D3DXVECTOR3 *pScale) = 0;

    virtual HRESULT LoadLeafFromFile(LPCTSTR strFileName) = 0;

    virtual HRESULT SetPitchLimit(FLOAT fPitch) = 0;
    virtual HRESULT GetPitchLimit(FLOAT *pfPitch) = 0;

    virtual HRESULT SetRollLeft(FLOAT fRollLeft) = 0;

    virtual HRESULT SetRollRight(FLOAT fRollRight) = 0;
    virtual HRESULT GetRollRight(FLOAT *pfRollRight) = 0;

    virtual HRESULT SetMaxRollSpeed(FLOAT fMaxRollSpeed) = 0;
    virtual HRESULT GetMaxRollSpeed(FLOAT *pfMaxRollSpeed) = 0;

    virtual HRESULT SetMaxPitchSpeed(FLOAT fMaxPitchSpeed) = 0;

    virtual HRESULT SetRollAccelerate(FLOAT fRollAccelerate) = 0;
    virtual HRESULT GetRollAccelerate(FLOAT *pfRollAccelerate) = 0;

    virtual HRESULT SetPitchAccelerate(FLOAT fPitchAccelerate) = 0;

    virtual HRESULT SaveToFile(const TCHAR cszFileName[], UINT uOption) = 0;

    //virtual HRESULT GetFullName(LPCTSTR *ppszFullName) = 0;

    virtual HRESULT Init(IEKG3DModel* pModel) = 0;

    virtual HRESULT GetObjProperty(BYTE **ppbObj) = 0;

    virtual HRESULT SavePropertyToIni(const char cszFileName[]) = 0;
};

#endif //_INCLUDE_IEKG3DMODELSPEEDTREE_H_
