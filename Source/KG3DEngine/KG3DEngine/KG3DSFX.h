////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFX_H_
#define _INCLUDE_KG3DSFX_H_

#include "KG3DSFXParticleContainer.h"
#include "KG3DSFXParticleTextureTool.h"
#include "KG3DSFXParticleLifeTimeLine.h"
#include "KG3DSFXParticleLauncher.h"
#include "KG3DSFXParticleForceField.h"
#include "KG3DTexture.h"
#include "KG3DSFXFileDefine.h"
#include "KG3DModel.h"
#include "KG3DSFXBillboard.h"
#include "KG3DSFXTrack.h"
#include "KG3DSFXMeteor.h"
#include "KG3DSFXMagicParpet.h"
#include "KG3DSFXChannel.h"
#include "KG3DSFXAnimationController.h"
#include "IEKG3DSFX.h"
#include "IEKG3DMaterial.h"
#include "KG3DSfxExp.h"
#include "KG3DSfxBoxProjection.h"

#define Aoe_Tex_Size 1024

//  global tool function 

extern float		GetRandom(float fMin, float fMax);
extern int			GetRandomSgn();
extern D3DXVECTOR2  GetRandVec2D();
extern D3DXVECTOR3  GetRandVec3D();
 
//namespace {

    inline BOOL SetForceBit(DWORD& dwBits, DWORD dwIndex)
    {
        if (dwIndex >= 32)
            return FALSE;

        dwBits |= 0x00000001 << dwIndex;

        return TRUE;
    }

    inline BOOL HasForceBit(DWORD& dwBits, DWORD dwIndex)
    {
        if (dwIndex >= 32)
            return FALSE;

        if (dwBits & (0x00000001 << dwIndex))
            return TRUE;

        return FALSE;
    }

    inline BOOL DelForceBit(DWORD& dwBits, DWORD dwIndex)
    {
        if (dwIndex >= 32)
            return FALSE;

        dwBits &= ~(0x00000001 << dwIndex);

        return TRUE;
    }
//}


interface IEKG3DSFXChannel;
class KG3DSFXSelectMark;
struct TagSourceInfo;

class KG3DSFX : public KG3DModel, public IEKG3DSFX
{
public :
	
	//  load and save 
	
    HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
    HRESULT SaveToFile(const char cszFileName[], unsigned uOption);
	HRESULT _SaveToFile(const char cszFileName[], unsigned uOption);
    HRESULT Reload();
    HRESULT Init();      
    void    ClearUp(BOOL bUnBind);
    void    ReSet();
    BOOL    IsLoaded();

	virtual BOOL IsSortAsSFX();
	
	BOOL    IsModelLoad();
    BOOL    IsAniLoad();
	virtual BOOL IsResourceReady(BOOL bTestText);


	HRESULT OnReferenceZero();
	//	render and update

	HRESULT Render(unsigned uOption, void* pExtInfo);
    HRESULT FrameMove();
    HRESULT UpdateParticleSystem();
    HRESULT RenderSelectMark(DWORD color = 0xFFFFFFFF);
	void	TransformationFrameMove();

	
	//	animation contral
    IKG3DAnimationController* GetAnimationController(enuAnimationControllerPriority nPriority);
	IKG3DAnimationController* GetSplitAnimationContorller(DWORD dwPartIndex, DWORD dwControllerIndex);
    HRESULT SetCurrentFrame(float fFrame);
    float   GetCurrentFrame() const;
    HRESULT PlayAnimation(SFX_ANIMATION_PLAY sfxAnimationPlay, float fSpeed = 1.f, int nOffsetTime = 0.f);
    HRESULT PlayAnimation(
        DWORD  dwPlayType, 
        LPCSTR cszAnimationName, 
        FLOAT  fSpeed, 
        int    nOffsetTime, 
        PVOID  pReserved,
		PVOID  pUserdata,
		enuAnimationControllerPriority Priority
    );
    SFX_ANIMATION_PLAY GetPlayAnimation() const;
    HRESULT SetFrameInterval(DWORD dwInterval);
    DWORD   GetFrameInterval() const;
    float   GetCurrInterval() const;
    HRESULT SetTotalFrameNum(DWORD dwTotalFrame);
    DWORD   GetTotalFrameNum() const;
    DWORD   GetParticleNum() const;
    DWORD   GetMaxParticleNum() const;
    void    SetMaxParticleNum(DWORD dwNum);
    int    GetPlayTime() const;
    virtual BOOL OnProcessCommands(KG3DModelCommandBase* pCmd);


	// bind and unbind

    virtual HRESULT _BindToSocket(KG3DModel *pModel, const char cszSocketName[]);
    virtual HRESULT _BindToBone(KG3DModel* pModel, const char* cszSocketName);
    virtual HRESULT UnBindFromOther();
	virtual void _SetPhysicsScene(KG3DPhysiscScene *pScene);

	HRESULT BindToModel(KG3DModel *pModel);
	HRESULT UnBind();


	//	set and get attribute

	D3DXMATRIX*  GetCurrentWorldMatrix();
    virtual BOOL IsBBoxChanged();
    virtual void SetBBoxChanged(BOOL bBBoxChanged);
	HRESULT		 ComputeBBox();
	BOOL		 IsRayIntersect(D3DXVECTOR3 &Inter, D3DXVECTOR3 Origin, D3DXVECTOR3 Direction);
	HRESULT		 SetAlpha(float fAlpha, BOOL bUseSpecial);

	void		SetScaleSize(float f);
	float		GetScaleSize();

	int			GetIsCloseFloor();
	int			GetIsShockWave();
    DWORD       GetShockMode() { return m_dwShockMode; }
    void        SetShockMode(DWORD mode) { m_dwShockMode = mode; }
	HRESULT		SetIsShockWave(int nValue);
	void        SetTagUsingInfo();
    DWORD       IsFixedSize() { return m_dwFixedSize; }
    void        EnableEixedSize(DWORD dwEnable) { m_dwFixedSize = dwEnable; }

    DWORD       IsLoacAxisY() { return m_dwLockYAlex; };
    void        EnableLockY(DWORD dwEnable) { m_dwLockYAlex = dwEnable; };
    LPCTSTR     GetName()   { return m_scName.c_str(); }
    void        SetShockWavePower(float power)      { KG3DModel::SetShockWavePower(power);  }
    float       GetShockWaePower()                  { return KG3DModel::GetShockWaePower(); }

    virtual HRESULT SetPointLightInfo(KG3D_PointLight_Info& vInfo) { return S_OK; }


	// texture

    HRESULT AddTexture(const char szTextureName[]);
    HRESULT RemoveTexture(UINT uTextureIndex);
    DWORD   GetTextureNum() const;
	const UINT GetRandTexFrame(UINT uTexIndex) const;

    HRESULT GetTexture(UINT uTextureIndex, KG3DTexture** ppTexture) const;
    HRESULT GetIKG3DTexture(UINT uTextureIndex, IKG3DTexture** ppTexture) const;
    HRESULT SetTexture(UINT uTextureIndex, const char szTextureName[]);
    HRESULT SetTextureCutNum(UINT  uTextureIndex, UINT  uCuttingNum);
    HRESULT GetTextureCutNum(UINT  uTextureIndex, UINT* pCutNum) const;
    HRESULT GetTextureCoord(FRECT* pTextureCoord, UINT  uTextureIndex, UINT uFrameIndex);

	HRESULT GetRenderTargetTextureCount(OUT DWORD* pTexCount);
	HRESULT GetRenderTargetTexture(OUT DWORD* pResourceId, IN DWORD dwTexIndex = 0);
	HRESULT SetCallbackRender(CallbackRender fpCallbackRender, void* pUserData);

	HRESULT ReflushTexture();


	//	particel lift time line

    HRESULT AddParticleLifeTimeLine(IEKG3DSFXParticleLifeTimeLine** ppLifeTimeLine);
    HRESULT GetParticleLifeTimeLine(DWORD dwIndex, IEKG3DSFXParticleLifeTimeLine **ppLifeTimeLine);
    HRESULT RemoveParticleLifeTimeLine(DWORD dwIndex);
    DWORD   GetParticleLifeTimeLineNum() const;


	//	particle launcher

    HRESULT AddParticleLauncher(
        SFX_PARTICLE_TYPE		  eParticleType,
        SFX_LAUNCHER_SHAPE		  eLauncherShape,
        SFX_PARTICLE_UPDATE_MODE  eParticleUpdateMode,
        KG3DSFXParticleLauncher** ppParticleLauncher
    );
    HRESULT GetParticleLauncher(
        DWORD					  dwIndex,
        KG3DSFXParticleLauncher** ppParticleLauncher
    );
	HRESULT GetParticleLauncher(
		DWORD					    dwIndex,
		IEKG3DSFXParticleLauncher** ppParticleLauncher);
    HRESULT RemoveParticleLauncher(DWORD dwIndex);
    DWORD	GetParticleLauncherNum() const;
    BOOL    IsLauncherBeing(KG3DSFXParticleLauncher* pLauncer);


	//	force field
    
    HRESULT AddForceField(
        SFX_FORCE_FIELD_TYPE		  eForceFieldType,
        IEKG3DSFXParticleForceField** ppForceField
    );
    HRESULT GetForceField(
		DWORD						  dwIndex,
        IEKG3DSFXParticleForceField** ppForceField
    );
    HRESULT RemoveForceField(DWORD dwIndex);
    DWORD GetForceFieldNum() const;


	//  track

    HRESULT AddTrack(const char cszFileName[], KG3DSFXTrack **ppTrack);
    HRESULT GetTrack(DWORD dwIndex, KG3DSFXTrack**   ppTrack);
	HRESULT GetTrack(DWORD dwIndex, IEKG3DSFXTrack** ppTrack);
    HRESULT RemoveTrack(DWORD dwIndex);
    HRESULT ChangeTrack(DWORD dwIndex, const char cszNewFileName[]);
    DWORD   GetTrackNum() const;


	//	meteor

    HRESULT AddMeteor(KG3DSFXMeteor **ppMeteor);
    HRESULT GetMeteor(DWORD dwIndex, IEKG3DSFXMeteor **ppMeteor);
    DWORD   GetMeteorNum() const;
    HRESULT RemoveMeteor(DWORD dwIndex);
    HRESULT SetMeteorInterpolate(float fInterpolate);
    float   GetMeteorInterpolate();
    DWORD   GetMeteorBindTrackIndex(DWORD dwIndex);
	void    ClearMeteorListNode();//by huangjinshou 2007 8.1
	void    SetMeteorSampling(BOOL val);
	void	SetMeteorRunState(BOOL bSampling, KG3DAnimationController *pController);


	//	billboard

    HRESULT AddBillboard(KG3DSFXBillboard** ppBillboard);
    HRESULT GetBillboard(DWORD dwIndex, KG3DSFXBillboard **ppBillboard);
	HRESULT GetBillboard(DWORD dwIndex, IEKG3DSFXBillboard** ppBillboard);
    DWORD   GetBillboardNum() const;
    HRESULT RemoveBillboard(DWORD dwIndex);
    HRESULT EnableBillboardSort(BOOL bEnable);


	//	magic parpet

	HRESULT AddMagicParpet(IEKG3DSFXMagicParpet **ppMagicParpet, DWORD dwType);
	HRESULT RemoveMagicParpet(int index);
    HRESULT RemoveMagicParpet(IEKG3DSFXMagicParpet* aoe);
    size_t GetMagicParpetNum() { return m_vecMagicParpet.size(); }
	IEKG3DSFXMagicParpet* GetMagicParpet(int index);
	IEKG3DSFXSelectMark* GetSelectMark();


    // box projection

    HRESULT AddBoxProjection(IKG3DSfxBoxProjection** ppBoxProjection);
    HRESULT RemoveBoxProjection(IKG3DSfxBoxProjection* pBoxProjection);
    HRESULT RemoveBoxProjection(int index);
    IKG3DSfxBoxProjection* GetBoxProjection(int index);
    size_t GetBoxProjectionNum()    { return m_vecBoxProj.size(); }

	//	channel

	HRESULT AddChannel(IEKG3DSFXChannel** ppChannel);
	HRESULT RemoveChannel();
	HRESULT GetChannel(IEKG3DSFXChannel ** pChannel);
	KG3DSFXChannel *GetChannel();
	STDMETHOD(UpdateChannelEffectPos)(D3DXVECTOR3 v1, D3DXVECTOR3 v2);


	// models of sfx

    KG3DModel* GetModel(size_t index);
    HRESULT	SetModel(const char cszFileName[], KG3DModel** ppModel, size_t index);
    HRESULT AddModel(const char cszFileName[], KG3DModel** ppModel);
    HRESULT RemoveModel(size_t index);
    HRESULT GetModel(size_t index, IEKG3DModel **ppModel);

    HRESULT GetModelBindTrackIndex(size_t index, int* pTrackIndex);
    HRESULT GetModelAnimationSpeed(float *pValue, size_t index);
    HRESULT GetModelAnimationStart(float *pValue, size_t index);
    HRESULT GetModelAnimationMode(DWORD *pValue, size_t index);
	HRESULT GetModelAnimationFrameCount(DWORD* pValue, size_t index);
	HRESULT GetModelAnimationFrameTime(float* pValue, size_t index);

	void	SetModelAnimationFrameTime(float Value, size_t index);
	void	SetModelAnimationSpeed(float  Value, size_t index);
    void	SetModelAnimationStart(float Value, size_t index);
    void	SetModelAnimationMode(DWORD Value, size_t index);
	void	SetModelAnimationFrameCount(DWORD Value, size_t index);


    HRESULT GetMeshFileName(LPSTR pFileName);
    HRESULT GetMaterialFileName(LPSTR pFileName);
    HRESULT GetAnimationFileName(LPSTR pFileName);

	DWORD	GetNumModels() const;

	void	SetModelStartFrame(float fStartFrame);
	float	GetModelStartFrame();
	DWORD	GetMdlNumFrame(size_t index);
	FLOAT	GetNdlFrameTime(size_t index);

	HRESULT EnagleMdlAniConnect(int nEnagle, size_t index);
    HRESULT UpdateModelAniInfo(size_t index);
	HRESULT UpdateModelAniInfo();
    HRESULT SeekModelAniBySFXFrame(float fFrame, float* pSeek, size_t index);
	HRESULT SeekModelAniBySFXFrame(float fFrame, float* pfSeek);
	int		IsMdlAniConnect(size_t index);
    DWORD   GetModelPlayMode();

    HRESULT EnagleMdlBillBoard(int nEnagle,  size_t index);
    BOOL    IsMdlBillBoard(size_t index);


	//	model subset color time line

	HRESULT AddSubsetColorDiffuseLine(DWORD dwIndex, D3DCOLORVALUE initColor);
	HRESULT DeleteSubsetColorDiffuseLine(DWORD dwIndex);
    HRESULT AddMutiSubsetColorDiffuseLine(DWORD dwIndex, IEKG3DMaterial *piMat);
	HRESULT AddTimeLineModelAniBound();
	TKG3DTimeLinebase* GetSubsetColorLine(DWORD dwSubset,DWORD dwIndex);
	int GetSubsetColorLineSize(DWORD dwSubset);

	
	// model animation time line

	int GetTimeLineModelAniBoundSize();
	TKG3DTimeLinebase* GetModelAniBoundTimeLine(DWORD dwIndex);

    
    // sound 

    virtual HRESULT InsertSoundKey(LPCTSTR szFileName,     
        DWORD  dwMode,
        FLOAT  fVolume,
        FLOAT  fSFXKey,
        FLOAT  fMinDis,
        FLOAT  fMaxDis,
        BOOL   bLoop,
        BOOL   bContinue,
        int    nProbability,
        int    nSubIndex
        );
    virtual HRESULT RemoveSoundKey(FLOAT fSFXKey, int nSubIndex);

    virtual HRESULT ModifySoundKey(LPCTSTR szFileName,
        DWORD  dwMode,
        FLOAT  fVolume,
        FLOAT  fSFXKey,
        FLOAT  fMinDis,
        FLOAT  fMaxDis,
        BOOL   bLoop,
        BOOL   bContinue,
        int    nProbability,
        int    nSubIndex
        );
    virtual HRESULT QuerySoundKey(LPTSTR  szFileName,
        DWORD&  dwMode,
        FLOAT&  fVolume,
        FLOAT   fSFXKey,
        FLOAT&  fMinDis,
        FLOAT&  fMaxDis,
        BOOL&   bLoop,
        BOOL&   bContinue,
        int&    nProbability,
        int     nSubIndex
        );
    virtual int GetSoundKeyNum(FLOAT fSFXKey);
    
    HRESULT PlaySound(_SoundInfo& soundInfo, size_t index);
    //HRESULT PlaySound(SFX_SOUND_BLOCK& Sound);

    virtual TKG3DTimeLinebase* GetSoundTimeLine()
    {
        return &m_tlSoundKey;
    }

	virtual HRESULT ChangeResource(UINT uFrame, UINT uIndex, LPCSTR strNew);
	virtual HRESULT ChangePlayType(UINT uFrame, UINT uIndex, void *pDefault);
	virtual HRESULT DeleteResource(UINT uFrame, UINT uIndex);
	virtual HRESULT AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault);
	virtual HRESULT DeleteAllSound();
	virtual DWORD  GetSourceReference(TagSourceInfo *pInfo, DWORD dwSize);

    virtual IEKG3DSfxExp* GetExpEffect() { return m_pExpEffect; }

    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice();
    virtual void ScanlAllTimeLine(float fScanl);

	HRESULT _LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);

	virtual HRESULT LoadFromFileMultiThread();
    virtual HRESULT LoadPostProcess();
public :
    KG3DSFX();
    virtual ~KG3DSFX();

public :
        typedef HRESULT (KG3DSFX::*ProcessBlockFunction)(BYTE *pBuffer, DWORD dwOffset);
        static ProcessBlockFunction ms_pfnProcessBlock[SFX_FEID_SIZE];

		DWORD GetUserData()
		{
			return m_dwUserData;
		}
		void SetUserData(DWORD dwData)
		{
			m_dwUserData = dwData;
		}
private :

	// resource load and save	

    HRESULT ReadGeneralInformationBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadParticleKindBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadLauncherBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadForceFieldBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadTextureBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadBillboardBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadBoxProjBlock(BYTE* pBuffer, DWORD dwOffset);
    HRESULT ReadTrackBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadModelBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadMeteorBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadMagicParpetBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadChannelBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadSoundBlock(BYTE* pBuffer, DWORD dwOffset);
	HRESULT ReadSelectMarkBlock(BYTE *pBuffer, DWORD dwOffset);
    HRESULT ReadExpBlock(BYTE* pBuffer, DWORD dwOffset);

    DWORD   WriteGeneralInformationBlock(FILE *pFile);
    DWORD   WriteParticleKindBlock(FILE *pFile, DWORD dwIndex);
    DWORD   WriteLauncherBlock(FILE *pFile, DWORD dwIndex);
    DWORD   WriteForceFieldBlock(FILE *pFile, DWORD dwIndex);
    DWORD   WriteTextureBlock(FILE *pFile, DWORD dwIndex);
    DWORD   WriteBillboardBlock(FILE *pFile, DWORD dwIndex);
    DWORD   WriteBoxProjBlock(FILE* pFile, DWORD dwIndex);
    DWORD   WriteTrackBlock(FILE *pFile, DWORD dwIndex);
    DWORD   WriteModelBlock(FILE *pFile, DWORD dwIndex);
    DWORD   WriteMeteorBlock(FILE *pFile, DWORD dwIndex);
    DWORD   WriteMagicParpetBlock(FILE *pFile, DWORD dwIndex);
    DWORD   WriteChannelBlock(FILE *pFile);
    DWORD   WriteSoundBlock(FILE* pFile, DWORD dwIndex);
	DWORD   WriteSelectMarkBlock(FILE* pFile);
    DWORD   WriteExpBlock(FILE* pFile);
    void	SortBillboard();
    BOOL    ExpEffectValid();
    BOOL    HaveTimeScalingEffect();

    KG3DSFXAnimationController* m_pSFXAnimationController;

    HRESULT CalculateAABB();
    HRESULT UpdateMatrix();

    HRESULT PlayAnimationImpl(SFX_ANIMATION_PLAY sfxAnimationPlay, float fSpeed, int nOffsetTime);
	
public:	
	//Footprint/////////////  add by huangjinshou 2008-03-14
	HRESULT RenderFootprint();
	HRESULT AddSelectMark(IEKG3DSFXSelectMark ** ppSelectMark);
	HRESULT RemoveSelectMark();
	virtual	HRESULT Playing(); 
	DWORD m_dwLastTimeUpdateFootprint;
	///////////////////////
public :
    vector<KG3DSFXParticleLifeTimeLine*>        m_vecParticleLifeTimeLine;
    vector<KG3DSFXParticleForceField*>          m_vecSFXParticleForceField;
    vector<KG3DSFXParticleLauncher*>            m_vecSFXParticleLauncher;
    KG3DSFXParticleContainer                    m_ParticleContainer;

    typedef TKG3DTimeLine<D3DXCOLOR>			COLOR_LINE_T;
    typedef vector<COLOR_LINE_T*>				MDL_COLOR_LINE_T;
    // faint!
    vector<MDL_COLOR_LINE_T*>					m_vecSubsetColor;

	typedef TKG3DTimeLine<INT>					MDL_ANI_BOUND_T;
    vector<MDL_ANI_BOUND_T*>					m_timelineModelAniBound;

public :
    vector<KG3DTexture*>                        m_vecTexture;
    vector<UINT>                                m_vecCuttingNum;

    float               m_fCurrentFrame;
	float				m_fPrevFrame;
    DWORD               m_dwTotalFrameNum;
    DWORD               m_dwFrameInterval;
    DWORD               m_dwLastTime;
	float				m_fDelayTime;
    DWORD               m_dwStartTime;
	BOOL                m_bModelAnimationStarted;
    SFX_ANIMATION_PLAY  m_eSFXAnimationPlay;        // the curr paly mode
    SFX_ANIMATION_PLAY  m_eSFXAnimationPlaySave;    // the perv paly mode
    DWORD               m_dwModelPlayMode;
    float               m_fShockWavePower;
    float               m_fLastParticlFrame;
    float               m_fFixedFrame;
    float               m_fSpeed;

    vector<KG3DSFXBillboard*>   m_vecBillboard;
    vector<KG3DSFXTrack*>       m_vecTrack;
    vector<KG3DSFXMeteor*>      m_vecMeteor;
    vector<KG3DSFXMagicParpet*> m_vecMagicParpet;
    KG3DSFXChannel*             m_pChannel;
	KG3DSFXSelectMark*			m_pSelectMark;  // Add by huangjinshou 2008 1-15
    KG3DSfxExp*                 m_pExpEffect;       
    vector<KG3DSfxBoxProjection*> m_vecBoxProj;

    BOOL        m_bSortBillboard;
    float       m_fSizeScale;
    AABBOX      m_staticAABB;
	float	    m_fModelAniStartFrame;
    int         m_nPlayFinishFlag;
    int         m_nBillBoardMdlNum;

    CallbackRender  m_fpCallBackRender;
    void*           m_pUserData;
    DWORD           m_dwRenderTarIndex;
    DWORD           m_dwFixedSize;           // 0 : not 1 : yes
    DWORD           m_dwLockYAlex;           // 0 : not 1 : yes

    int             m_nVerson;
    int             m_nEditor;
	DWORD           m_dwLoadOption;

    int             m_nTexLoadIndex;
    int             m_nMdlLoadIndex;

    DWORD           m_dwMaxParticleNum;

    struct _SRT {

        D3DXVECTOR3 vScanl;
        D3DXVECTOR3 vTrans;
        D3DXMATRIX  matRot;

        inline D3DXMATRIX* ScanlMat(D3DXMATRIX* mat) {
            return D3DXMatrixScaling(mat, vScanl.x, vScanl.y, vScanl.z);
        }

        inline D3DXMATRIX* TransMat(D3DXMATRIX* mat) {
            return D3DXMatrixTranslation(mat, vTrans.x, vTrans.y, vTrans.z);
        }

    };

    _SRT            m_Srt;

    // models
   
	struct _SfxModelInfo
    {
        KG3DModel* pSFXModel;
        
        FLOAT      fPlayStart;
        FLOAT      fPlaySpeed;
        DWORD      dwPlayMode;
        DWORD      dwNumFrame;
        FLOAT      fFrameTime;
        INT        nLeftFrame;
        INT        nRighFrame;
        INT        nCycleFlag;
        INT        nValidFlag;
        INT        nTrackIndex;
		DWORD	   dwIsBillBoard;

        _SfxModelInfo()
        {
            pSFXModel  = NULL;
            fPlaySpeed = 1.0f;
            fPlayStart = 0.0f;
            dwPlayMode = 0;
            dwNumFrame = 0;
            fFrameTime = 0.0f;
            nLeftFrame = 0;
            nRighFrame = 0;
            nCycleFlag = FALSE;
            nValidFlag = FALSE;
            nTrackIndex = -1;
			dwIsBillBoard = FALSE;
        }
    };
    vector<_SfxModelInfo> m_vecModelsInfo;

   
    TKG3DTimeLine<_SoundInfo> m_tlSoundKey;
    //TKG3DTimeLine<SFX_SOUND_BLOCK> m_vecSoundKey;
    
	DWORD m_dwUserData;
};


class KG3DSFXSuede : public KG3DModel, public IEKG3DSFXSuede
{
public :

    enum { BIND_TYPE_BONE = 0, BIND_TYPE_ORG = 1, BIND_TYPE_SOCKET = 2 };

        // load and save 

    HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
    HRESULT LoadFromFile(IFile* pFile);
    HRESULT SaveToFile(const char cszFileName[]);
    HRESULT SaveToMemory(void** ppData, DWORD *pdwSize);
    BOOL    IsLoaded();
    
        // bind and unbind

    HRESULT BindToSocket(IKG3DModel* pModel, const char* cszSocketName);
    HRESULT BindToBone(IKG3DModel* pModel, const char* strBoneName);
    HRESULT UnBindFromOther();
	HRESULT UnBindFromOther(UINT uIndex);
    HRESULT Bind(IKG3DModel* pModel);
	HRESULT ApplyBind(IKG3DModel* pParent, UINT uIndex, BOOL bWorldBind = FALSE);
        // animation 

    IKG3DAnimationController* GetAnimationController(enuAnimationControllerPriority nPriority);
    IKG3DAnimationController* GetSplitAnimationContorller(DWORD dwPartIndex, DWORD dwControllerIndex);
    HRESULT PlayAnimation(DWORD dwType = SFX_AP_LOOP);
    HRESULT PlayAnimation(
        DWORD dwPlayType, 
        FLOAT fSpeed, 
        int   nOffsetTime, 
        PVOID pReserved,
		PVOID pUserdata,
		enuAnimationControllerPriority Priority
        );
    HRESULT PlayAnimation(
        DWORD  dwPlayType, 
        LPCSTR strAnimationName,
        FLOAT  fSpeed,
        int    nOffsetTime, 
        PVOID  pReserved,
		PVOID  pUserdata,
		enuAnimationControllerPriority Priority
        );

        // attributes

    HRESULT     SetAlpha(float fAlpha, BOOL bUseSpecial);
    HRESULT     SetCallbackRender(CallbackRender fpCallbackRender, void* pUserData);
    HRESULT     GetRenderTargetTextureCount(OUT DWORD* pTexCount);
    HRESULT     GetRenderTargetTexture(OUT DWORD* pResourceId, IN DWORD dwTexIndex = 0);

    DWORD       GetModelCount();
    IKG3DModel* GetModel(DWORD dwIndex);
	LPCSTR      GetModelName(DWORD dwIndex);
    KG3DModel*  GetBindModel();
	KG3DModel*  GetBindModel(UINT uIndex);

    HRESULT      AddModel(KG3DModel* pModel);
    HRESULT      DelModel(DWORD dwIndex);
	HRESULT      ChangeModel(UINT uIndex, KG3DModel *pModel);
   // KG3DModel*   UpdateModel(KG3DModel* pModel, DWORD dwIndex);
    HRESULT      Trim();
    HRESULT      CopyBindInfo(int nSrcIndex, int nDstIndex = -1);   
    HRESULT      ModifyInfo(int nIndex, KG3DModel* pModel, DWORD dwBindType, string TarName);

    HRESULT UnInit();
        // render and update

    HRESULT FrameMove();
    HRESULT Render();

	HRESULT Clone(KG3DSFXSuede* pSrc);

	void Reset();
public :
    KG3DSFXSuede();
    virtual ~KG3DSFXSuede();
private :
    KG3DSFXSuedeAnimationController* m_pSuedeAnimationController;
    struct _BindInfo {
        KG3DModel* pModel;
        DWORD      dwBindType;    
        string     strTarName;
		string     FileName;
		D3DXVECTOR3    vTransforation;
		D3DXVECTOR3    vScaling;
		D3DXQUATERNION qRotation;
		float SizeScale;
        _BindInfo()
        {
            vTransforation = D3DXVECTOR3(0.f, 0.f, 0.f);
            vScaling = D3DXVECTOR3(1.f, 1.f, 1.f);
            D3DXQuaternionIdentity(&qRotation);
        }
    };
	std::vector<KG3DModel*> m_ParentModels;

public :
    vector<_BindInfo>  m_vecBindInfo;
};

#endif //_INCLUDE_KG3DSFX_H_

