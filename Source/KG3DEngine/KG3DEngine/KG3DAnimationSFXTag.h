#pragma once
#include "kg3danimationtag.h"
#include "kg3danimationtagid.h"

class KG3DAnimationSFXTag;
class KG3DAnimationSFXTagLifeRangeContainer;
class KG3DAnimationSFXTagLifeRangeEventHandeler: public IKG3DAnimationEventHandler
{
	//标签触发的SFX
public:
	virtual ~KG3DAnimationSFXTagLifeRangeEventHandeler();
    KG3DAnimationSFXTagLifeRangeEventHandeler();
    void SetUser(KG3DAnimationSFXTag* pUser) { m_pTag = pUser; }
    virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
    virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
    virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);
private:
    KG3DAnimationSFXTag* m_pTag;
};

class KG3DAnimationSFXTagSceneLifeRangeEventHandeler : public IKG3DAnimationEventHandler
{
	//用在直接放在了场景里面的SFX
public:
	virtual ~KG3DAnimationSFXTagSceneLifeRangeEventHandeler();
	KG3DAnimationSFXTagSceneLifeRangeEventHandeler();
	void SetScene(KG3DScene* pScene) { m_pScene = pScene; }
	virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
	virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
	virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);
private:
	KG3DScene *m_pScene;
};


class KG3DAnimationTagContainer;
class KG3DAnimationTagSFXMotionSwithContainer;
class KG3DAnimationSFXTag: public IKG3DAnimationTag, public IEKG3DAnimationSFXTag
{
    friend HRESULT KG3DAnimationSFXTagLifeRangeEventHandeler::OnPlayAnimationFinish(IKG3DAnimationController* pController);
    friend class KG3DAnimationTagSFXMotionSwithContainer;
    friend class KG3DAnimationTagContainer;
    struct RunTimeInfo;
public:
    const static UINT KEYFRAME_PEND = 0xffffffff;
    enum SFXRuntimeState
    {
        SFX_RS_NONE,
        SFX_RS_ACTIVE,
        SFX_RS_FININSH,
    };

    KG3DAnimationSFXTag();
    ~KG3DAnimationSFXTag();
    virtual HRESULT Clear();
    virtual UINT ModifyTag(UINT uFrame, void* Data);
    virtual UINT RemoveTag(UINT uID);
    virtual UINT MoveTag(UINT uID, UINT uFrameTo);
    virtual UINT CopyTag(UINT uID, UINT uFrameDest);

	virtual HRESULT Helper_Apply(KG3DAnimationController* pController, 
		UINT uStartFrame, 
		UINT uEndFrame, 
		D3DXVECTOR3 *pvPos);
	virtual HRESULT CheckValidate();
    virtual void SetAnimationSpeed(float fSpeed);
    virtual void OnMotionChange();
    virtual enuTagRunState GetState();
    virtual HRESULT LoadFromFile(IFile* pFile, DWORD dwVersion, DWORD dwNumFrames);
	virtual HRESULT SaveToMemory(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame);
    virtual void SetParentModel(IKG3DModel* pModel);
    virtual UINT GetNumKeyFrame(UINT uFrame);
    const RunTimeInfo* GetSFXRuntimeState(UINT uFrameIndex, UINT uKeyIndex);
    virtual BOOL CheckValidation();
    virtual void Reset();
	virtual HRESULT Clone(IKG3DAnimationTag* pSrc);

	virtual HRESULT GetKeyFrameData(UINT uID, const SFXKeyFrameInfoVersion1** ppKeyFrameData);
	virtual HRESULT GetKeyFrameName(UINT uID, LPCSTR* strTagName);
    virtual HRESULT GetTagIDByFrameIndex(UINT uFrame, UINT uIndex, UINT *pID);
    virtual HRESULT GetTagFrameIndexByID(UINT uId, UINT *pFrame, UINT *pIndex);
    virtual HRESULT GetSFXFileName(UINT uIndex, LPCSTR* strFileName);
    virtual HRESULT GetBindInfoInstance(IKG3DModel** ppBindInfo);
	virtual DWORD GetSourceReference(TagSourceInfo *pBuffer, DWORD dwSize);
	virtual HRESULT ChangeResource(UINT uFrame, UINT uIndex, LPCSTR strNew);
	virtual HRESULT DeleteResource(UINT uFrame, UINT uIndex);
	virtual HRESULT AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault);
	virtual HRESULT ChangePlayType(UINT uFrame, UINT uIndex, void *pDefault);
	virtual HRESULT DeleteAllSound();
private:
	void ClearUnusedSFX(DWORD dwMaxFrame);
    virtual UINT RemoveTag(UINT uFrameIndex, UINT uIndex);
    virtual UINT MoveTag(UINT uFrameFrom, UINT uKeyIndex, UINT uFrameTo);
    virtual UINT CopyTag(UINT uFrameSrc, UINT uKeyIndex, UINT uFrameDest);
    void SetSFXRuntimeState(UINT uID, DWORD dwMask, const RunTimeInfo& StateToSet);
  
    enum RunTimeInfoMask
    {
        RTIM_RUNSTATE = 0x1,
        RTIM_MODEL    = (0x1 << 1),
        RTIM_FINISHSTATE = (0x1 << 2),
        RTIM_MOTIONFINISHSTATE = (0x1 << 3),
        RTIM_COUNT = 4,
    };

    struct RunTimeInfo
    {
        SFXRuntimeState RunState;//当前的运行情况
        KG3DModel* pModel;//相关的特效
        SFXFinishState FinishEventType;//特效播放完成的处理方式, 是DataInfo中的拷贝,可能不一致
        SFXMotionFinshState LifeRangeType;//作为生命周期的标志
        RunTimeInfo();
    };

    typedef void(*SetRunTimeFunc)(RunTimeInfo&, const RunTimeInfo&);
    static void Helper_SetRunState(RunTimeInfo& Dest, const RunTimeInfo& Src);
    static void Helper_SetModel(RunTimeInfo& Dest, const RunTimeInfo& Src);
    static void Helper_SetFinishState(RunTimeInfo& Dest, const RunTimeInfo& Src);
    static void Helper_SetMotionFinishState(RunTimeInfo& Dest, const RunTimeInfo& Src);

    struct Data
    {
        SFXKeyFrameInfoVersion1 KeyFrame;
        RunTimeInfo  RunTime;
    };

    KG3DSFXSuede* m_pBindInfo;
    std::vector<Data>* GetKeyFrameInfo(UINT uFrame);
    void OnSFXActived(UINT uActiveIndex);
    KG3DAnimationSFXTagLifeRangeEventHandeler m_EventHandler;
    float m_fParentAnimationSpeed;
    typedef std::map<UINT, std::vector<Data>> KeyFrameData;
    KeyFrameData m_KeyFrames;
    KG3DAnimationTagID m_IDMgr;
    std::vector<UINT> m_KeyedFrame;
    static SetRunTimeFunc m_SetRunTimeFunc[RTIM_COUNT];
    static DWORD m_SetRunTimeFuncIndex[RTIM_COUNT];
    KG3DModel* m_pParentModel;
private:
	static IKG3DModel* GetPatternModel();//不需要Release，也不需要Delete// Last modify by Chen Tianhong: 2007-10-31 15:59:26

};


class KG3DAnimationTagSFXMotionSwithEventHandeler: public IKG3DAnimationEventHandler
{
	//动作完成之后继续播放的SFX， 需要自己删除自己
public:
   virtual ~KG3DAnimationTagSFXMotionSwithEventHandeler();
	KG3DAnimationTagSFXMotionSwithEventHandeler();
    virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
    virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo);
    virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);
private:
};

IKG3DAnimationEventHandler* GetSFXTagMotionSwitchEventHandeler();
IKG3DAnimationEventHandler* GetSFXTagSceneWorldPosEventHandeler();
extern void ClearSelfDestructSfxList();
extern void SelfDestructedFrameMove();