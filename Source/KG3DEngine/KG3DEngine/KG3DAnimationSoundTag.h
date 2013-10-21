#pragma once
#include "kg3danimationtag.h"

class KG3DAnimationSoundTagBatchTool;
class KG3DAnimationSoundTag : public IKG3DAnimationTag, public IEKG3DAnimationSoundTag
{
    const static DWORD INVALIDATE_SOUND_HANDLE = 0xffffffff;
	const static DWORD CURRENT_VERSION = 3;
	const static DWORD INVALIDATE_CANDIDATE = 0xffffffff;
	const static float s_fDefaultMaxDistance;
	const static float s_fDefaultMinDistance;
	friend class KG3DAnimationSoundTagBatchTool;
public:
    
    enum enuRunState
    {
        SRS_NONE,
        SRS_ACTIVE,
        SRS_FINISHED,
    };

    KG3DAnimationSoundTag();
    virtual ~KG3DAnimationSoundTag();
    
    //IEKG3DAnimationTag Interface
	virtual UINT ModifyTag(UINT uID, void* Data);
    virtual UINT RemoveTag(UINT uID);
    virtual UINT MoveTag(UINT uID, UINT uFrameTo);
    virtual UINT CopyTag(UINT uID, UINT uFrameDest);
    virtual UINT GetNumKeyFrame(UINT uFrame);
    virtual HRESULT GetKeyFrameData(UINT uFrame, const SoundDataInfoVersion2** pKeyFrameData);
	virtual HRESULT GetKeyFrameData(UINT uFrame, const SoundDataInfoVersion3** pKeyFrameData);
	virtual HRESULT SaveToMemory(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame);
	virtual HRESULT DeleteSoundCandidate(UINT uFrame, UINT uIndex);
	virtual DWORD GetSourceReference(TagSourceInfo *pBuffer, DWORD dwSize);
	virtual HRESULT GetSound(SoundType soundType,AnimationSoundTagInfo * pInfo);
	virtual HRESULT ClearTag(UINT uID);
	//IEKG3DAnimationTag Interface end;

    //IKG3DAnimationTag
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
	virtual HRESULT ChangeResource(UINT uFrame, UINT uIndex, LPCSTR strNew);
	virtual HRESULT ChangePlayType(UINT uFrame, UINT uIndex, void *pDefault);
	virtual HRESULT DeleteResource(UINT uFrame, UINT uIndex);
	virtual HRESULT AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault);
	virtual HRESULT DeleteAllSound();
    virtual BOOL IsModify();
    virtual HRESULT GetSoundTagInfo(void *pDefault);
    virtual HRESULT Play3DSound(LPSTR pFileName,FLOAT fVolume);
	//IKG3DAnimationTag end


private:
    void UpdatePosition(D3DXVECTOR3 *pPos);
	void ClearHandle(DWORD dwActiveHandle);
	void OnLoop();
    void ClearDumpedTag();
    BOOL IsOnlyVersion2Data();
    HRESULT SaveToMemory3(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame);
    HRESULT SaveToMemory2(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame);
    struct RunTimeInfo
    {
        enuRunState RunState;
        DWORD       dwHandle;//保存KeyFrame打开的声音句柄
        RunTimeInfo();
    };

    struct KeyFrame
    {
        SoundDataInfoVersion2 Data;
        RunTimeInfo RunState;
    };
	struct KeyFrameVersion3 
	{
		SoundDataInfoVersion3 Data;
		RunTimeInfo RunState;
	};
    struct SoundDataSave
    {
        DWORD dwFrame;
        SoundDataInfo DataInfo;
    };
	
	struct SoundDataSaveVersion1
	{
		DWORD dwFrame;
		SoundDataInfoVersion1 DataInfo;
	};

	struct SoundDataSaveVersion2
	{
		DWORD dwFrame;
		SoundDataInfoVersion2 DataInfo;
		SoundDataSaveVersion2& operator =(const SoundDataSave &In);
		SoundDataSaveVersion2& operator =(const SoundDataSaveVersion1 &In);
	};
	struct SoundDataSaveVersion3 
	{
		DWORD dwFrame;
		SoundDataInfoVersion3 DataInfo;
	};
	inline DWORD GetCandidate(const KeyFrame& Frame);
	typedef std::map<UINT, KeyFrame> KeyFrameInfo;
    KeyFrameInfo m_Tags;
    std::vector<UINT> m_KeyedFrame;
	typedef std::map<UINT, KeyFrameVersion3> KeyFrameInfoVersion3;// 版本3的数据
	KeyFrameInfoVersion3 m_TagsVersion3;

    float m_fParentAnimationSpeed;
};


class KG3DAnimationSoundTagBatchTool
{
public:
	static void SetAllSoundTagKeep(KG3DAnimationSoundTag *pTag);//设置所有的SoundTag为Keep属性
};