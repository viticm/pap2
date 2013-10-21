////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFlexibleBody.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008/12/24 15:12:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DFLEXIBLEBODY_H_
#define _INCLUDE_KG3DFLEXIBLEBODY_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DPhysiscScene;

struct IKG3DFlexibleBody : public IUnknown
{
    virtual HRESULT AddToPhysicsScene(KG3DPhysiscScene *pPhysicsScene) = 0;
    virtual HRESULT RemoveFromPhysicsScene() = 0;

    virtual HRESULT Update() = 0;
};

////////////////////////////////////////////////////////////////////////////////
class KG3DModel;
class KG3DMesh;
// maybe return S_OK, E_FAIL, KG3D_E_NO_FLEXIBLE_BODY_DATA
HRESULT KG3DCreateFlexibleBodyFromModel(KG3DModel *pModel, IKG3DFlexibleBody **ppiRetFlexibleBody);
HRESULT KG3DCreateFlexibleBodyFromMesh(KG3DMesh *pMesh, IKG3DFlexibleBody **ppiRetFlexibleBody);

HRESULT KG3DIsFlexibleBodyBone(const char cszBoneName[], int *pnRetFlexibleBoneFlag);

#endif //_INCLUDE_KG3DFLEXIBLEBODY_H_
