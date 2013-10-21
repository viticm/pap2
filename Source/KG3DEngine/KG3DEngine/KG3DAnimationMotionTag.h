#pragma once
#include "KG3DAnimationTag.h"

class KG3DSFX;

class KG3DAnimationMotionTag: public IKG3DAnimationTag, 
	public IEKG3DAnimationMotionTag
{
	struct MotionTagDataInfoNew
	{
		TCHAR strTagName[TAG_NAME_MAX_LENGTH];
		DWORD dwFrame;
		DWORD dwNumMotionExtension;
		DWORD dwBlockLength[MOTIONTAG_MAX_BLOCK];
	};
	const static DWORD s_dwCurrentVersion = 1;
	
	struct MotionTagKeyFrameSave
	{
		UINT uFrame;
		MotionTagKeyFrame FrameData;
	};

public:
	KG3DAnimationMotionTag();
	~KG3DAnimationMotionTag();

	//IEKG3DAnimationTag Interfaces Start
	virtual UINT ModifyTag(UINT uID, void* Data);
	virtual UINT RemoveTag(UINT uID);
	virtual UINT MoveTag(UINT uID, UINT uFrameTo);
	virtual UINT CopyTag(UINT uID, UINT uFrameDest);
	virtual UINT GetNumKeyFrame(UINT uFrame);
	virtual UINT GetBindedSocketName(vector<string> &vecSocketName);
	virtual HRESULT SaveToMemory(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame);
	virtual void ClearUnused();
	//virtual UINT GetKeyFrameData(UINT uFrame, MotionTagDataInfo& Data);
	virtual UINT GetKeyFrame(UINT uFrame, MotionTagDataRuntime* &pData);
	virtual DWORD GetBlockType(DWORD dwFrame, DWORD dwIndex);
	
	virtual void  SetMotionStart(UINT uFrame);
	virtual void  SetMotionEnd(UINT uFrame);
	virtual UINT GetMotionStart();
	virtual UINT GetMotionEnd();
	virtual DWORD GetSourceReference(TagSourceInfo *pBuffer, DWORD dwSize);
	virtual HRESULT ChangeResource(UINT uFrame, UINT uIndex, LPCSTR strNew);
	virtual HRESULT DeleteResource(UINT uFrame, UINT uIndex);
	virtual HRESULT AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault);
	virtual HRESULT ChangePlayType(UINT uFrame, UINT uIndex, void *pDefault);
	virtual HRESULT DeleteAllSound();
	//virtual UINT GetKeyFrameData(UINT uFrame, UINT uIndex, void* pData);
	//IEKG3DAnimationTag Interfaces end

	//IKG3DAnimationTag interfaces start
	virtual enuTagRunState GetState();
	virtual HRESULT Clear();
	virtual HRESULT LoadFromFile(IFile* pFile, DWORD dwVersion, DWORD dwNumFrames);
	virtual HRESULT Helper_Apply(KG3DAnimationController* pController, 
		UINT uStartFrame, 
		UINT uEndFrame, 
		D3DXVECTOR3 *pvPos);
	virtual BOOL CheckValidation();
	virtual void SetAnimationSpeed(float fSpeed);
	virtual void OnMotionChange();
	virtual void SetParentModel(IKG3DModel* pModel);
	virtual void Reset();
	virtual HRESULT Clone(IKG3DAnimationTag* pSrc);

	//IKG3DAnimationTag interfaces end

	//IKG3DAnimationMotionTag start
	//virtual UINT GetNumTag();
	//IKG3DAnimationMotionTag end

	//IEKG3DAnimationMotionTag start
	virtual DWORD GetMotionTagType(UINT uFrame);
	//IEKG3DAnimationMotionTag end
	void UpdateMeteor(TagMotionCallBackExtraInfo &ExtraInfo, KG3DAnimationController *pController);

protected:

	UINT CreateNewTag(UINT uFrame, 
		TCHAR* strTagName);

	void OnApplyDamagePercentage(void *pData, 
		KG3DAnimationController *pController, 
		D3DXVECTOR3 *pvPos);

	void OnApplyFaceMotion(void *pData,
		KG3DAnimationController *pController, 
		D3DXVECTOR3 *pvPos);


	void OnApplyMeteor(void *pData, 
		KG3DAnimationController *pController, 
		D3DXVECTOR3 *pvPos);

	void OnApplyMissle(void *pData,
		KG3DAnimationController *pController,
		D3DXVECTOR3 *pvPos);

	void OnApplyEquipSfx(void *pData,
		KG3DAnimationController *pContoller,
		D3DXVECTOR3 *pvPos);

	void OnApplyGroundEffect(void *pData,
		KG3DAnimationController *pController,
		D3DXVECTOR3 *pvPos);



	HRESULT ConvertToVersion1(MotionTagKeyFrameSave& Data);

	
	DWORD GetBlockLength(DWORD dwType);
	void* GetNewBlock(DWORD dwType);


	typedef void (KG3DAnimationMotionTag::*pfOnApply)(void*, KG3DAnimationController *pController, D3DXVECTOR3 *pvPos);
	static pfOnApply s_fpApplyList[MTBT_COUNT];
    HRESULT ProcessUnderWater(
        D3DXVECTOR3 *pvPos);
    HRESULT PlayGoundEffectSfx(LPCSTR pSfxName,D3DXVECTOR3 *pvPos);
private:
	typedef std::map<UINT, MotionTagDataRuntime> NewDataStructure;
	std::vector<UINT> m_KeyedFrame;
	NewDataStructure m_NewData;
	UINT m_uStartFrame;
	UINT m_uEndFrame;
	IKG3DModel* m_pParentModel;
	float m_fParentAnimationSpeed;
	std::set<string> m_setSockets;
    D3DXVECTOR3 m_vLastPos;
};

//在普通播放情况下的表情动画的播放完成回调
//在每次播放完成之后会根据播放几率决定下一次播放的动画
class KG3DFaceMotionLifeRangeHandler: public IKG3DAnimationEventHandler
{
public:
	virtual ~KG3DFaceMotionLifeRangeHandler();
	KG3DFaceMotionLifeRangeHandler();
	void SetUser(KG3DAnimationMotionTag* pUser) { m_pTag = pUser; }
	virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);
	
	virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, 
		IKG3DModel* pModelSFX, 
		TagSFXCallBackExtraInfo* pExtraInfo);
	
	virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, 
		DWORD dwMotionInfo, 
		TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);
private:
	KG3DAnimationMotionTag* m_pTag;
};

//默认表情的回调函数
//在播放完成一次表情动画之后, 根据默认表情的设置参数选择播放的下一个表情动画
class KG3DFaceMotionDefaultHandler: public IKG3DAnimationEventHandler
{
public:
	virtual ~KG3DFaceMotionDefaultHandler();
	KG3DFaceMotionDefaultHandler();
	void SetUser(KG3DAnimationMotionTag* pUser) { m_pTag = pUser; }
	virtual HRESULT OnPlayAnimationFinish(IKG3DAnimationController* pController);

	virtual HRESULT OnTagPlayingSFX(IKG3DAnimationController* pController, 
		IKG3DModel* pModelSFX, 
		TagSFXCallBackExtraInfo* pExtraInfo);

	virtual HRESULT OnMotionNotify(IKG3DAnimationController* pController, 
		DWORD dwMotionInfo, 
		TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo);
private:
	KG3DAnimationMotionTag* m_pTag;
};

extern KG3DFaceMotionDefaultHandler* g_pFaceMotionDefaultHandler;
extern KG3DFaceMotionLifeRangeHandler* g_pLifeRangeHandler;