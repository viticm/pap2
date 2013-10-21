////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneSFXEditor.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-8 14:40:20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENESFXEDITOR_H_
#define _INCLUDE_KG3DSCENESFXEDITOR_H_

////////////////////////////////////////////////////////////////////////////////
#include "KG3DSceneSelectBase.h"
#include "KG3DSceneEditorBase.h"
#include "KG3DModel.h"
#include "KG3DSFXTrack.h"
#include "KG3DMeshBone.h"
#include "KG3DSFXChannel.h"
#include "IEKG3DSceneSFXEditor.h"


class KG3DSFX;
interface IEKG3DModel;

struct SFX_BIND_INFORMATION
{
    int nLauncher1;
    int nLauncher2;
    vector<int>     vecBillboard;
};

struct SFX_TRACK_BIND_INFORMATION
{
    vector<int>     vecBillboard;
    vector<int>     vecLauncher;
    vector<int>     vecModel;
    vector<int>     vecMeteor;
};


// 简直哦哟了~~
#define SCENESTATE_SUB_NONE						0
#define SCENESTATE_SUB_BILLBOARD_ROTATION		1
#define SCENESTATE_SUB_BILLBOARD_AXIS_ROTATION	2

#pragma pack(push,1)


struct SFX_BIND_HEADER
{
    DWORD  dwSFXCnt;
};

struct SFX_BIND_BLOCK
{
    char cszSFXFileName[128];
    D3DXMATRIX  Transformation;

    float SizeScale;
    char cszBoneName[32];
    DWORD PlayMode;

    DWORD  dwTargetType;        // 0 : bone 1 : orgin 2 :socket
    float  fInterpolateLength;  // 插值长度：后改为加密关键帧精度

    D3DXVECTOR3    vTransforation;
    D3DXVECTOR3    vScaling;
    D3DXQUATERNION qRotation;

    DWORD  dwReserved[6];
};

#pragma pack(pop)

class KG3DSceneSFXEditor : public KG3DSceneEditorBase, public IEKG3DSceneSFXEditor
{
public://add by xiayong
    virtual HRESULT SetCurBindModel(IEKG3DModel *piModel);
public:
	// 这下哦哟了三
	DWORD	m_dwSubEditState;

    static int ms_BillboardIndex;
    static int ms_LauncherIndex;

	KG3DSFX     *m_lpSFX;

    KG3DModel   *m_pModel;    // deprecate

	KG3DSceneEntityList  m_listPostRenderEntity;

    std::vector<SFX_BIND_INFORMATION>    m_vecBindInformation;

    std::vector<SFX_TRACK_BIND_INFORMATION> m_vecTrackBindInformation;

    std::vector<SFX_ENTITY_DATA*>       m_vecBillboardEntityData;
    std::vector<SFX_ENTITY_DATA*>       m_vecLauncherEntityData;
    std::vector<SFX_ENTITY_DATA*>       m_vecTrackEntityData;

    SFX_ENTITY_DATA*    m_pModelEntityData;

    KG3DMeshBone        m_meshBone;

    DWORD               m_dwBkColor;
	
	KG3DTexture*        m_pBGTexture;// huangjinshou
	BOOL                m_bShowBGTexture;
public:
	virtual BOOL    IsShowBGTexture();
	virtual void    EnableShowBGTexture(BOOL val);
	virtual HRESULT LoadBGTexture(const char filename[]);// by huangjinshou
    virtual ULONG   GetEditState() { return KG3DSceneEditorBase::GetEditState(); }
    virtual HRESULT SetEditState(DWORD dwState);
    virtual float   GetFps() { return m_fFPS; }
	virtual void    SyncBindInfo();

    // edit
    virtual void    SetGeoType(const DWORD dwType, const DWORD dwSubType) { m_dwGeoType = dwType; m_dwGeoSubType = dwSubType; };
    virtual void    GetGeoType(DWORD& dwType, DWORD& dwSubType)           { dwType = m_dwGeoType; dwSubType = m_dwGeoSubType; };

    virtual void    SetGeoStep(const DWORD dwStep) { m_dwGeoStep = dwStep; }
    virtual DWORD   GetGeoStep()                   { return m_dwGeoStep;   }

    virtual void    CreateGeoElement();
    virtual void    AppendGeoElementToSfx();
    virtual void    ReleaseGeoElement();
    virtual void    ModifyGeoElement(const D3DXVECTOR3& x, const D3DXVECTOR3& z, const D3DXVECTOR3& y);

    virtual D3DXVECTOR3 GetGeoRayPointHor();
    virtual D3DXVECTOR3 GetGeoRayPointVel(const D3DXVECTOR3& vHor);

    virtual int GetModelOverlapBox(KG3DModel* pModels[], int maxNum, D3DXVECTOR3 box[8]);
	HRESULT AddPostRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable);

public:
    virtual HRESULT GetScene(IEKG3DScene **ppiScene);
    virtual HRESULT GetSceneEditBase(IEKG3DSceneEditorBase **ppiRetSceneEditorBase);
    virtual HRESULT GetSFX(IEKG3DSFX **ppiRetSFX);
    virtual HRESULT GetTrackBindInfoLauncherCount(DWORD dwIndex, DWORD *pdwRetCount);
    virtual HRESULT GetTrackBindInfoBillboardCount(DWORD dwIndex, DWORD *pdwRetCount);
    virtual HRESULT GetTrackBindInfoModelCount(DWORD dwIndex, DWORD *pdwRetCount);
    virtual HRESULT GetTrackBindInfoMeteorCount(DWORD dwIndex, DWORD *pdwRetCount);

    virtual HRESULT UpdateTrackBindInfo();
    virtual HRESULT LauncherBindToTrack(DWORD dwLauncherIndex, DWORD dwTrackIndex);
    virtual HRESULT LauncherUnbindFromTrack(DWORD dwLauncherIndex);
    virtual HRESULT BillboardBindToTrack(DWORD dwBillboardIndex, DWORD dwTrackIndex);
    virtual HRESULT BillboardUnbindFromTrack(DWORD dwBillboardIndex);
    virtual HRESULT ModelBindToTrack(DWORD dwBillboardIndex, DWORD dwTrackIndex);
    virtual HRESULT ModeldUnbindFromTrack(DWORD dwBillboardIndex);
    virtual HRESULT MeteorBindToTrack(DWORD dwBillboardIndex, DWORD dwTrackIndex);
    virtual HRESULT MeteorUnbindFromTrack(DWORD dwBillboardIndex);

    virtual HRESULT GetBindModel(IEKG3DModel **ppiRetBindModel);
    virtual HRESULT GetRefrenceModel(int index, IEKG3DModel **ppiRetReferenceModel);
    virtual HRESULT RefModelsSeekAnimation(DWORD dwSeekType, FLOAT fOffset);
    virtual HRESULT RefModelsPlaySFX(
        SFX_ANIMATION_PLAY SFXModelAnimationMode, 
        BOOL bAnotherModelPauseAnimation
    );

    virtual HRESULT GetCurBindModel(IEKG3DModel **ppiRetCurBindModel);
    virtual HRESULT GetCurBeBindModel(IEKG3DModel **ppiRetCurBeBindModel);
    virtual HRESULT SetBindTargerInfo(INT nType, LPCTSTR pcszName);
    virtual HRESULT GetBindTargerInfoName(LPCTSTR *ppcszRetName);

    virtual HRESULT GetModelNameBySocketName(
        LPCTSTR pcszSocketName, LPCTSTR *ppcszRetModelName
    );

    virtual HRESULT GetD3DFillModel(DWORD *pdwD3DFillModel);
    virtual HRESULT SetD3DFillModel(DWORD dwD3DFillModel);
public:
    virtual HRESULT NewSFX();
	virtual HRESULT OpenSFX(LPCTSTR pFileName);

    HRESULT LoadMesh(LPSTR pszMeshFile);        // deprecate
    HRESULT LoadMaterial(LPSTR pszMeshFile);    // deprecate
    HRESULT LoadAnimation(LPSTR pszAniFile);    // deprecate

    HRESULT GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR3* pIn);

    // Bind things
    virtual HRESULT LoadBindFile(const char cszBindFile[]);
    virtual HRESULT SaveBindFile(const char cszFileName[]);

    virtual HRESULT LoadBindModel(LPSTR pszMesh);
    virtual HRESULT LoadBindModelAnimation(LPSTR pszName);
    //HRESULT LoadBindSFXModel(LPSTR pszSFX);

    //HRESULT AddBindSFXModel();
    //HRESULT RemoveBindSFXModel(DWORD dwIndex);

    KG3DModel* m_pBindModel;      // use to bind
   // KG3DModel* m_pRefrenceModel;  // use to look

    vector<KG3DModel*>   m_vecRefModels;
    KG3DModel*           m_pHandModel;
    KG3DModel*           m_pCurBindModel;   // 
    KG3DModel*           m_pCurBeBindMdl;

    KG3DModel*           m_pScanlModel;


    //LPD3DXMESH           m_pBindFlagMesh;

    DWORD                m_dwGeoType;
    DWORD                m_dwGeoSubType;
    DWORD                m_dwGeoStep;

    LPD3DXEFFECT         m_pEffect;

    KG3DSFXParticleLauncher* m_pGeoLauncher;

    enum _BIND_STATE 
    {
        BIND_STATE_FREE,
        BIND_STATE_LINKING
    };
    _BIND_STATE m_nLinkingFlag;

    struct _BindTarget
    {
        int   nType; // -1 : none 0 : orgin 1 : bone 2 : socket 
        TCHAR szName[128];
        
        _BindTarget()
        {
            nType = 0;
            ZeroMemory(szName, sizeof(szName));
        }
        
        int ReSet()
        {
            nType = 0;
            ZeroMemory(szName, sizeof(szName));
            return true;
        }
    };
    _BindTarget m_bindTargerInfo;

    virtual HRESULT ExportSelModelSuede(const TCHAR szFileName[]);
    virtual HRESULT ImportSuedeToSelMdl(const TCHAR szFileName[]);

    virtual HRESULT BindSelMdlToTargetMdl();
    virtual HRESULT BindSelMdlToTargetMdl(const TCHAR szName[], DWORD dwType);
    virtual HRESULT UnBindSelModel();

    HRESULT SetCurBindModel();
    HRESULT DrawBindFlag();

    virtual HRESULT AddHandMdlToRefMdl();
    HRESULT LoadHandModel(LPSTR pPath);
    HRESULT UpdateSelRefMdl();

	virtual HRESULT RenderForEachWindow(KG3DSceneOutputWnd& wndCur);
	HRESULT RenderForMainWindow(KG3DSceneOutputWnd& wndCur);

    virtual HRESULT ShockWaveRender();
    HRESULT FrameMove();

    void MsgOnLButtonUp(WPARAM wParam, LPARAM lParam);
    void MsgOnLButtonDown(WPARAM wParam, LPARAM lParam);
    void MsgOnKeyDown(WPARAM wParam, LPARAM lParam);
    void MsgOnRButtonUp(WPARAM wParam, LPARAM lParam);

    // move	//2009-1-14 <Move Refactor>基类会处理Move，这里就不用处理了
    //virtual HRESULT MoveBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction);
    //virtual HRESULT MoveEnd();
    //virtual HRESULT MoveFrameMove(/*KG3DCamera*  pCamera, D3DXVECTOR3& Origin, D3DXVECTOR3& Direction, float XX, float YY*/);//2009-1-22去掉
    //virtual HRESULT OnToggleStateMove();//<OnToggleStateMove Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态

    //HRESULT GetSelectedEntityCenter(D3DXVECTOR3* vPos);
    //HRESULT RenderSelectedEntityMark(DWORD dwEditState, D3DVIEWPORT9& Viewport, D3DXMATRIX& Projection, D3DXMATRIX& View, BOOL bShowScreenMask);//2009-1-15，和SelectBase里面的一样，移动掉了

    // rotation
    //HRESULT RotateBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction);
    //HRESULT RotateEnd();

   /* HRESULT RotateFrameMove(
        D3DXVECTOR3& Origin,
        D3DXVECTOR3& Direction,
        float XX,
        float YY,
        float ZZ
        );*/	//没有覆盖行为，去掉
    //HRESULT OnToggleStateRotation();

    // scal
    //HRESULT ScaleBegin(D3DXVECTOR3& Origin,D3DXVECTOR3& Direction);
    /*HRESULT ScaleFrameMove(
        KG3DCamera*  pCamera,
        D3DXVECTOR3& Origin,
        D3DXVECTOR3& Direction,
        float XX,
        float YY,
        float ZZ
        );*///没有覆盖行为，去掉
    //HRESULT ScaleEnd();
    //HRESULT OnToggleStateScaling();//<OnToggleStateScaling Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态

    void    SetCoordMode(DWORD dwMode);


    virtual HRESULT AddLauncher(float fX, float fY, float fZ);
    virtual HRESULT RemoveLauncher(DWORD dwIndex);

    virtual HRESULT AddBillboard(float fX, float fY, float fZ);
    virtual HRESULT RemoveBillboard(DWORD dwIndex);

    virtual HRESULT AddBoxProjection();
    virtual HRESULT RemoveBoxProjection(DWORD dwIndex);

    virtual HRESULT AddMeteor();
    virtual HRESULT RemoveMeteor(DWORD dwIndex);

    virtual HRESULT AddTrack(const char cszFileName[], float fX, float fY, float fZ);
    virtual HRESULT RemoveTrack(DWORD dwIndex);
    virtual HRESULT ChangeTrack(DWORD dwIndex, const char cszNewFileName[]);

    virtual HRESULT AddModel(const char cszFileName[], float fX, float fY, float fZ);
    virtual HRESULT RemoveModel(size_t index);
    virtual HRESULT ChangeModel(const char cszNewFileName[], size_t index);

    //HRESULT NewOneLight(KG3DLight** ppLight);//CTH,2007-12-7 14:41:54，Light由GetEnvironment().GetLightSet()创建

    virtual HRESULT SetFocus(BOOL bFocus);
    virtual HRESULT GetFocus(BOOL *pbRetFocus);

    HRESULT Init();
	KG3DSceneSFXEditor(void);
	virtual ~KG3DSceneSFXEditor(void);

    virtual HRESULT SaveToFile(const char lpszPathName[]);

//    void CopySelectedObject(float fX, float fY, float fZ);

    virtual HRESULT GetBillboardEntityDataCount(DWORD *pdwBillboardCount);
    virtual HRESULT GetBillboardEntityData(DWORD dwIndex, SFX_ENTITY_DATA **ppRetBillboardEntityData);
    virtual HRESULT SetBillboardEntityDataNameToDefault(SFX_ENTITY_DATA *pBillboardEntityData);

    virtual HRESULT GetLauncherEntityDataCount(DWORD *pdwLauncherCount);
    virtual HRESULT GetLauncherEntityData(DWORD dwIndex, SFX_ENTITY_DATA **ppRetLauncherEntityData);
    virtual HRESULT AddLauncherEntityData();
    virtual HRESULT SetLauncherEntityDataNameToDefault(SFX_ENTITY_DATA *pLauncherEntityData);

    virtual HRESULT RemoveSFXEntity(DWORD dwIndex);
    HRESULT RemoveSFXEntity(KG3DSceneEntity &Entity);

    // Editer 
	HRESULT EnableSfxCloseFloor(int nEnable);
    HRESULT EnableRenterLauncher(int nBeRender);
	HRESULT GetFloor(const D3DXVECTOR3* pvInput, FLOAT* pfRet);

    // scene mode
    virtual HRESULT UpdateSceneMode(DWORD dwMode);
    virtual HRESULT UpdateBindOrRefrenceModel(LPSTR szPath);
    virtual HRESULT UpdateBindOrRefrenceAni(LPSTR szPath);
    HRESULT LoadRefrenceModel(LPSTR pszMesh);
    //HRESULT LoadRefrenceModelAnimation(LPSTR szPath);

    // bind
    virtual HRESULT GetSFXSceneMode(DWORD *pdwRetSFXSceneMode);
    //KG3DSFXSuede* GetSFXSuede();
    
    virtual HRESULT  UpdateSFXBindIndex(int nIndex);
    int      GetCurSfxBindIndex();

    HRESULT  BindSuedeSFX();
    HRESULT  BindSuedeSFX(LPSTR pszBoneName);
    HRESULT  UnBindSuedeSFX();

    HRESULT  UpdateSFXSuedeSFX(LPSTR pszPath);
    HRESULT  UpdateSceneEntity();  

    virtual HRESULT  UnbindItemToSocket(LPSTR pszSocketName);
    virtual HRESULT  BindItemToSocket(LPSTR pszSocketName, LPSTR pszItemPath);

    // sock wave
    virtual HRESULT EnableSoceWave(int nEnagle);
    virtual HRESULT SetShockMode(DWORD mode);

    virtual HRESULT EnableShowGround(int nEnable);


    // calk back
    virtual HRESULT RegistCalkBackRender(RenderCallBack* pRender);

    //
    virtual HRESULT ShowBindBone();
    virtual HRESULT UpdateBoneSel();

    virtual FLOAT GetRotationCoordAngelX();
    virtual FLOAT GetRotationCoordAngelY();
    virtual FLOAT GetRotationCoordAngelZ();

    virtual BOOL    OpenModel2Bind(LPCTSTR szModelName, LPCTSTR szBindName);
    virtual HRESULT ModifyBindInfo(IKG3DModel* pModel, IKG3DModel* pBind, int nModifyIndex);
    virtual BOOL    IsModifyMode(){ return m_bIsModify; };

    virtual bool    ImportSfx(
        SfxImportInfo& info, DWORD dwImpFrameNum, DWORD dwImpInterval, DWORD dwPrecept, BYTE* pBuffer);

    virtual void    OnEntityTransformation(IEKG3DSceneEntity* pEntity, int nFrame);
    virtual void    GetScanlModelScanl(D3DXVECTOR3* pvScanl);
    virtual void    InsertExpScanlKeyFrame(int nFrame);
    virtual void    ShowExpScanlModel(BOOL bShow);
    virtual void    UpdateSfxExp();

	virtual HRESULT GetSelectionToolInterface(IEKG3DSceneSelectionTool** ppTool);
   /* struct _RayLine
    {
        D3DXVECTOR3 vOrg;
        D3DXVECTOR3 vDir;
    };*/

   /* _RayLine m_linkLineOrgRay;
    HRESULT GetLinkEntity(
        _RayLine         originRayLine,
        _RayLine         targetRayLine,
        KG3DSceneEntity& originEntity,
        KG3DSceneEntity& targetEntity
    );*/

    PDWORD  GetBkColorPtr(){ return &m_dwBkColor; };
    void SetBkColor(DWORD dwColor){ m_dwBkColor = dwColor; }

    void DrawGroundGrid(KG3DCamera* pCamera);

    HRESULT AdhereToModel(
        KG3DModel*  pSourceModel,
		std::string      strSurEFName,
        DWORD       dwSurType,
        KG3DModel*  pTargetModel,
		std::string      strTarEFName,
        DWORD       dwTarType
    );
    struct _AdhereInfo
    {
        KG3DModel*  pModel;              // adhere model
		std::string      strEffectPointName;   // the name of the bone or socket
        DWORD       dwType;               // 0 : origin 1 : bone 2 : socket
        HRESULT     Update(D3DXVECTOR3* pPosition);

        _AdhereInfo()
        {
            pModel = NULL;
        }
    };
    _AdhereInfo     m_adereSource;       
    _AdhereInfo     m_adereTarget;


private:
    LPD3DXFONT  m_lpD3DFont;
    BOOL        m_bFocus;
    BOOL        m_bRenderLauncherFlag;

	// SPS
	DWORD		m_dwPrevTime;
	DWORD		m_dwCurrTime;
	TCHAR		m_szInfo[128];
	FLOAT		m_fCounter;
    DWORD       m_dwTimer;
    FLOAT       m_fFPS;

    LPDIRECT3DVERTEXBUFFER9 m_pGroundGridVertexs;

    // scene mode

    KG3DSceneEntityList   m_listSceneEntityEdit;
    KG3DSceneEntityList   m_listSceneEntityBind;
    KG3DSceneEntityList   m_listPostRenderEdit;
    KG3DSceneEntityList   m_listPostRenderBind;

    DWORD       m_dwSceneMode;
    BOOL        m_bIsModify;

    KG3DTexture*          m_pGroundTexture;

    // bind 
    
    // modify for tag

    KG3DSFXSuede*           m_pModifySuede;
    int                     m_nModifyIndex;


    //KG3DModel*              m_pMfyRdfModel;


    int                     m_nCurBindSfxIndex;
    
    int                     m_nShowVideoInfo;

    HRESULT ShowVideoInfo(int nEnable){m_nShowVideoInfo = nEnable; return S_OK;}
    BOOL    IsVideoInfoShow(){return m_nShowVideoInfo;}

public :
/*    KG3DLocalMoveCoord      m_localMoveCoord;
    KG3DLocalRotationCoord  m_localRotationCoord;
    KG3DLocalScalCoord      m_localScalCoord;  */  

    // Item Bind
    struct _BINDITEM
    {
		std::string      strSocketName;
        KG3DModel*  pItemModel;

        _BINDITEM()
        {
            strSocketName = TEXT("");
            pItemModel    = NULL;
        }
    };
    vector<_BINDITEM>  m_vecITEM;
    

    // calk back 
    vector<RenderCallBack*> m_lpCalkBackRender; 


    DWORD   m_dwD3DFillModel;

	HRESULT AdhereToModelEx(
		IEKG3DModel*  pSourceModel,
		LPCTSTR       strSurEFName,
		DWORD       dwSurType,
		IEKG3DModel*  pTargetModel,
		LPCTSTR      strTarEFName,
		DWORD       dwTarType
		);
	BOOL HaveModel(IEKG3DModel* pModel);
};

#endif //_INCLUDE_KG3DSCENESFXEDITOR_H_
