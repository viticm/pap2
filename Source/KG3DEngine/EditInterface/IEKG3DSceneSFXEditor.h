////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DSceneSFXEditor.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2006-11-21 15:16:11
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DSCENESFXEDITOR_H_
#define _INCLUDE_IEKG3DSCENESFXEDITOR_H_

#include "IEEditor.h"

////////////////////////////////////////////////////////////////////////////////
interface IEKG3DSceneOutputWnd;
interface RenderCallBack
{
    virtual int Render(IEKG3DSceneOutputWnd* pOWnd) = 0;
    int wndID;
};

struct SfxImportElement {
    DWORD dwType;
    DWORD dwOffset;
    DWORD dwSelfIndex;
};

typedef std::vector<SfxImportElement> SfxImportInfo;
struct SFX_ENTITY_DATA
{
    float fRotationX;
    float fRotationY;
    float fRotationZ;

    TCHAR szEntityName[32];
};

enum
{
    SFX_SCENE_MODE_EDIT,
    SFX_SCENE_MODE_BIND
};


/*
 * Sfx Editor Geo Type
 */

enum 
{
    SFX_GEO_TYPE_LAUNCHER,
    SFX_GEO_TYPE_BILLBOARD,
    SFX_GEO_TYPE_NUM
};

/*
 * Sfx Editor Geo Sub Type
 */

enum 
{
    SFX_GEO__LAUNCHER_SUB_TYPE_RECT,
    SFX_GEO__LAUNCHER_SUB_TYPE_CIRC,
    SFX_GEO__LAUNCHER_SUB_TYPE_SHER,
    SFX_GEO__LAUNCHER_SUB_TYPE_CYLR
};

/*
 * Sfx Editor Geo Step
 */

enum 
{
    SFX_GEO_STEP_P,
    SFX_GEO_STEP_1,
    SFX_GEO_STEP_2,
    SFX_GEO_STEP_F
};

interface IKG3DModel;
interface IEKG3DSFX;
interface IEKG3DModel;
interface IEKG3DScene;
enum SFX_ANIMATION_PLAY;
class KG3DSceneEditorBase;
//interface IEKG3DTexture;
interface IEKG3DSceneEditorBase;//by dengzhihui Refactor 2006Äê12ÔÂ6ÈÕ
interface IEKG3DSceneSFXEditor
{
	virtual BOOL    IsShowBGTexture() = 0;
	virtual void    EnableShowBGTexture(BOOL val) = 0;
	virtual HRESULT LoadBGTexture(const char filename[]) = 0 ;// by huangjinshou
    virtual ULONG   GetEditState() = 0;
    virtual HRESULT SetEditState(DWORD dwState) = 0;
    virtual float   GetFps() = 0;
	virtual void    SyncBindInfo() = 0;

    // edit
    virtual void    SetGeoType(const DWORD dwType, const DWORD dsSubType) = 0;
    virtual void    GetGeoType(DWORD& dwType, DWORD& dsSubType) = 0;

    virtual void    SetGeoStep(const DWORD dwStep) = 0;
    virtual DWORD   GetGeoStep() = 0;

    virtual void    CreateGeoElement() = 0;
    virtual void    AppendGeoElementToSfx() = 0;
    virtual void    ReleaseGeoElement() = 0;
    virtual void    ModifyGeoElement(const D3DXVECTOR3& x, const D3DXVECTOR3& z, const D3DXVECTOR3& y) = 0;

    virtual D3DXVECTOR3 GetGeoRayPointHor() = 0;
    virtual D3DXVECTOR3 GetGeoRayPointVel(const D3DXVECTOR3& x) = 0;

    virtual HRESULT GetScene(IEKG3DScene **ppiScene) = 0;
    virtual HRESULT GetSceneEditBase(IEKG3DSceneEditorBase **ppiRetSceneEditorBase) = 0;
    virtual HRESULT GetSFX(IEKG3DSFX **ppiRetSFX) = 0;
    virtual HRESULT GetTrackBindInfoLauncherCount(DWORD dwIndex, DWORD *pdwRetCount) = 0;
    virtual HRESULT GetTrackBindInfoBillboardCount(DWORD dwIndex, DWORD *pdwRetCount) = 0;
    virtual HRESULT GetTrackBindInfoModelCount(DWORD dwIndex, DWORD *pdwRetCount) = 0;
    virtual HRESULT GetTrackBindInfoMeteorCount(DWORD dwIndex, DWORD *pdwRetCount) = 0;

    virtual HRESULT UpdateTrackBindInfo() = 0;
    virtual HRESULT LauncherBindToTrack(DWORD dwLauncherIndex, DWORD dwTrackIndex) = 0;
    virtual HRESULT LauncherUnbindFromTrack(DWORD dwLauncherIndex) = 0;
    virtual HRESULT BillboardBindToTrack(DWORD dwBillboardIndex, DWORD dwTrackIndex) = 0;
    virtual HRESULT BillboardUnbindFromTrack(DWORD dwBillboardIndex) = 0;
    virtual HRESULT ModelBindToTrack(DWORD dwBillboardIndex, DWORD dwTrackIndex) = 0;
    virtual HRESULT ModeldUnbindFromTrack(DWORD dwBillboardIndex) = 0;
    virtual HRESULT MeteorBindToTrack(DWORD dwBillboardIndex, DWORD dwTrackIndex) = 0;
    virtual HRESULT MeteorUnbindFromTrack(DWORD dwBillboardIndex) = 0;
    
    virtual HRESULT SetCurBindModel(IEKG3DModel *piModel) = 0;
    virtual HRESULT GetBindModel(IEKG3DModel **ppiRetBindModel) = 0;
    virtual HRESULT GetRefrenceModel(int index, IEKG3DModel **ppiRetReferenceModel) = 0;
    virtual HRESULT RefModelsSeekAnimation(DWORD dwSeekType, FLOAT fOffset) = 0;
    virtual HRESULT RefModelsPlaySFX(
        SFX_ANIMATION_PLAY SFXModelAnimationMode,
        BOOL bAnotherModelPauseAnimation
    ) = 0;
    virtual HRESULT GetCurBindModel(IEKG3DModel **ppiRetCurBindModel) = 0;
    virtual HRESULT GetCurBeBindModel(IEKG3DModel **ppiRetCurBeBindModel) = 0;
    virtual HRESULT SetBindTargerInfo(INT nType, LPCTSTR pcszName) = 0;
    virtual HRESULT GetBindTargerInfoName(LPCTSTR *ppcszRetName) = 0;

	virtual HRESULT EnableSfxCloseFloor(int nEnble) = 0;
    virtual HRESULT GetModelNameBySocketName(
        LPCTSTR pcszSocketName, LPCTSTR *ppcszRetModelName
    ) = 0;

    virtual HRESULT NewSFX() = 0;
    virtual HRESULT OpenSFX(LPCTSTR pFileName) = 0;

    virtual HRESULT LoadBindFile(const char cszBindFile[]) = 0;
    virtual HRESULT SaveBindFile(const char cszFileName[]) = 0;

    virtual HRESULT LoadBindModel(LPSTR pszMesh) = 0;
    virtual HRESULT LoadBindModelAnimation(LPSTR pszName) = 0;

    virtual HRESULT ExportSelModelSuede(const TCHAR szFileName[]) = 0;
    virtual HRESULT ImportSuedeToSelMdl(const TCHAR szFileName[]) = 0;

    virtual HRESULT BindSelMdlToTargetMdl() = 0;
    virtual HRESULT BindSelMdlToTargetMdl(const TCHAR szName[], DWORD dwType) = 0;
    virtual HRESULT UnBindSelModel() = 0;

    virtual HRESULT AddHandMdlToRefMdl() = 0;

    virtual HRESULT AddLauncher(float fX, float fY, float fZ) = 0;
    virtual HRESULT RemoveLauncher(DWORD dwIndex) = 0;

    virtual HRESULT AddBillboard(float fX, float fY, float fZ) = 0;
    virtual HRESULT RemoveBillboard(DWORD dwIndex) = 0;

    virtual HRESULT AddBoxProjection() = 0;
    virtual HRESULT RemoveBoxProjection(DWORD dwIndex) = 0;

    virtual HRESULT AddMeteor() = 0;
    virtual HRESULT RemoveMeteor(DWORD dwIndex) = 0;

    virtual HRESULT AddTrack(const char cszFileName[], float fX, float fY, float fZ) = 0;
    virtual HRESULT RemoveTrack(DWORD dwIndex) = 0;
    virtual HRESULT ChangeTrack(DWORD dwIndex, const char cszNewFileName[]) = 0;

    virtual HRESULT AddModel(const char cszFileName[], float fX, float fY, float fZ) = 0;
    virtual HRESULT RemoveModel(size_t index) = 0;
    virtual HRESULT ChangeModel(const char cszNewFileName[], size_t index) = 0;

    virtual HRESULT SetFocus(BOOL bFocus) = 0;
    virtual HRESULT GetFocus(BOOL *pbRetFocus) = 0;

    virtual HRESULT SaveToFile(const char lpszPathName[]) = 0;

    virtual HRESULT GetBillboardEntityDataCount(DWORD *pdwBillboardCount) = 0;
    virtual HRESULT GetBillboardEntityData(DWORD dwIndex, SFX_ENTITY_DATA **ppRetBillboardEntityData) = 0;
    virtual HRESULT SetBillboardEntityDataNameToDefault(SFX_ENTITY_DATA *pBillboardEntityData) = 0;

    virtual HRESULT GetLauncherEntityDataCount(DWORD *pdwLauncherCount) = 0;
    virtual HRESULT GetLauncherEntityData(DWORD dwIndex, SFX_ENTITY_DATA **ppRetLauncherEntityData) = 0;
    virtual HRESULT AddLauncherEntityData() = 0;
    virtual HRESULT SetLauncherEntityDataNameToDefault(SFX_ENTITY_DATA *pLauncherEntityData) = 0;


    virtual HRESULT RemoveSFXEntity(DWORD dwIndex) = 0;

    virtual HRESULT UpdateSceneMode(DWORD dwMode) = 0;
    virtual HRESULT UpdateBindOrRefrenceModel(LPSTR szPath) = 0;
    virtual HRESULT UpdateBindOrRefrenceAni(LPSTR szPath) = 0;

    virtual HRESULT GetSFXSceneMode(DWORD *pdwRetSFXSceneMode) = 0;

    virtual HRESULT UpdateSFXBindIndex(int nIndex) = 0;

    virtual HRESULT UnbindItemToSocket(LPSTR pszSocketName) = 0;
    virtual HRESULT BindItemToSocket(LPSTR pszSocketName, LPSTR pszItemPath) = 0;

    virtual HRESULT EnableSoceWave(int nEnagle) = 0;
    virtual HRESULT SetShockMode(DWORD mode) = 0;

    virtual HRESULT EnableShowGround(int nEnable) = 0;

    virtual HRESULT RegistCalkBackRender(RenderCallBack* pRender) = 0;

    virtual HRESULT ShowBindBone() = 0;
    virtual HRESULT UpdateBoneSel() = 0;


    virtual HRESULT GetD3DFillModel(DWORD *pdwD3DFillModel) = 0;
    virtual HRESULT SetD3DFillModel(DWORD dwD3DFillModel) = 0;

    virtual FLOAT GetRotationCoordAngelX() = 0;
    virtual FLOAT GetRotationCoordAngelY() = 0;
    virtual FLOAT GetRotationCoordAngelZ() = 0;

	virtual HRESULT AdhereToModelEx(
		IEKG3DModel*  pSourceModel,
		LPCTSTR       strSurEFName,
		DWORD       dwSurType,
		IEKG3DModel*  pTargetModel,
		LPCTSTR      strTarEFName,
		DWORD       dwTarType
		) = 0;
	virtual BOOL HaveModel(IEKG3DModel* pModel) = 0;
    virtual HRESULT ShowVideoInfo(int nEnable) = 0;
    virtual BOOL    IsVideoInfoShow() = 0;
    virtual PDWORD  GetBkColorPtr() = 0;
    virtual  void   SetBkColor(DWORD dwColor) = 0;
    virtual bool    ImportSfx(SfxImportInfo& info, DWORD dwImpFrameNum, DWORD dwImpInterval, DWORD dwPrecept, BYTE* pBuffer) = 0;
    
    virtual BOOL    OpenModel2Bind(LPCTSTR szModelName, LPCTSTR szBindName) = 0;
    virtual HRESULT ModifyBindInfo(IKG3DModel* pModel, IKG3DModel* pBind, int nModifyIndex) = 0;
    virtual BOOL    IsModifyMode() = 0;


    virtual void    SetCoordMode(DWORD dwMode) = 0;
    virtual void    OnEntityTransformation(IEKG3DSceneEntity* pEntity, int nFrame) = 0;
    virtual void    GetScanlModelScanl(D3DXVECTOR3* pvScanl) = 0;
    virtual void    InsertExpScanlKeyFrame(int nFrame) = 0;
    virtual void    ShowExpScanlModel(BOOL bShow) = 0;
    virtual void    UpdateSfxExp() = 0;

	virtual HRESULT GetSelectionToolInterface(IEKG3DSceneSelectionTool** ppTool) = 0;
};

interface IEKG3DRepresentPVS;

enum {
    PLACE_PTL_BILLBOARD,
    PLACE_PTL_LOCK_Y,
    PLACE_PTL_HOR
};

interface IPvsEditorEventCallback
{
    virtual void OnAddObject(IEKG3DRepresentObject* obj) = 0;
    virtual void OnDelObject(IEKG3DRepresentObject* obj) = 0;
    virtual void OnObjModify(IEKG3DRepresentObject* obj) = 0;
	virtual void OnSelectedChanged() = 0;
};

enum KG3DLightType
{
	LIGHTTYPE_POINT,
	LIGHTTYPE_DIRECTION,
	LIGHTTYPE_PROJECTION,
};

interface IEKG3DScenePvsEditor
{
    virtual HRESULT OpenPvs(LPCTSTR fileName) = 0;
    virtual HRESULT NewPvs() = 0;
    virtual HRESULT SavePvs(LPCTSTR fileName) = 0;
    virtual IEKG3DScene* GetScene() = 0;
    virtual IEKG3DSceneEditorBase* GetSceneEditBase() = 0;
    virtual ULONG GetEditState() = 0;
    virtual int DeleteSelectedObject() = 0;
	virtual IEKG3DRepresentObject* GetFirstSelectedObj() = 0;
    virtual HRESULT SetHandObject(LPCTSTR fileName) = 0;
    virtual void ScalHandObj(float fScal) = 0;
    virtual IEKG3DRepresentPVS* GetPvs() = 0;
    virtual void SetCoordMode(KG3DCOORD coord) = 0;
    virtual void SetPlaceProtalMode(DWORD dwMode) = 0;
    virtual void RegistEventCallBack(IPvsEditorEventCallback* pEvent)= 0;
    virtual void UnRegistEventCallBack(IPvsEditorEventCallback* pEvent) = 0;
    virtual void SelectObject(IEKG3DRepresentObject* obj) = 0;
    virtual void DeleteObject(IEKG3DRepresentObject* obj) = 0;
    virtual void EnablePMode(BOOL bEnable) = 0;
    virtual BOOL IsPMode() = 0;
    virtual void EnableCull(BOOL bCull) = 0;
    virtual void OnCommand(enumCommandType eCommandType) = 0;

	virtual HRESULT BakeLightMap(BOOL bAllModel,BOOL bComputeDirectLight,BOOL bComputeIndirectLight) = 0;
	virtual HRESULT ComputePhotons() = 0;

	virtual BOOL GetEnableShowLightSign() = 0;
	virtual void SetEnableShowLightSign(BOOL bEnable) = 0;

	virtual BOOL GetEnableShowChessBoradLightmap() = 0;
	virtual void SetEnableShowChessBoradLightmap(BOOL bEnable) = 0;
};

#endif //_INCLUDE_IEKG3DSCENESFXEDITOR_H_
