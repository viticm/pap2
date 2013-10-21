#pragma once
#include "KG3DInterface.h"
class IEKG3DAnimation;
class IEKG3DClip;
struct MotionTagDataRuntime;
enum enuIEInterfaceReturnValue
{
	IEIR_SUCCESS = -1,
	IEIR_FAILED = 0,
	IEIR_FRAMEOVERWRITEN = 2,
};

const static DWORD FRAMEINDEX_NEW = 0xfffe;

enum enuFaceMotionType
{
	FACEMOTION_SPECIFIED = 0xffffffff,
};

struct MotionTagBlockFaceMotion
{
	enuMotionTagBlockType Type;
	DWORD dwMotionType;
	TCHAR strPath[MAX_PATH];
	float fPlaySpeed;
	DWORD dwPlayType;
	float fPlayRation;
	BOOL  bKeepOnMotionFinish;
	DWORD dwReserved[32];
	MotionTagBlockFaceMotion()
	{
		Type = MTBT_FACEMOTION;
		dwMotionType = 0;
		strPath[0] = '\0';
		fPlaySpeed = 1.0f;
		fPlayRation = 100.0f;
		dwPlayType = ENU_ANIMATIONPLAY_ONCE;
		bKeepOnMotionFinish = FALSE;
	}
};

struct MotionTagBlockDamagePercentage
{
	enuMotionTagBlockType Type;
	float fDamagePercentage;
	MotionTagBlockDamagePercentage()
	{
		Type = MTBT_DAMAGEPERCENTAGE;
		fDamagePercentage = 1.0f;
	}
};

struct MotionTagBlockMissile
{
	enuMotionTagBlockType Type;
	float fDamagePercentage;
	DWORD dwMissileID;
	MotionTagBlockMissile()
	{
		Type = MTBT_MISSILE;
		fDamagePercentage = 1.0f;
		dwMissileID = 0;
	}
};

struct MotionTagBlockEquipSFX
{
	enuMotionTagBlockType Type;
	DWORD dwEquipType;
	DWORD dwEquipRepesentID;
	DWORD dwSfxID;
	MotionTagBlockEquipSFX()
	{
		Type = MTBT_EQUIPSFX;
		dwEquipType = 0;
		dwEquipRepesentID = 0;
		dwSfxID = 0;
	}
};

struct MotionTagBlockGroundEffect
{
	enuMotionTagBlockType Type;
	DWORD dwFootIndex;
    DWORD dwSFXIndex;
	MotionTagBlockGroundEffect()
	{
		Type = MTBT_GROUNDEFFECT;
		dwFootIndex = 0;
        dwSFXIndex = 0;
	}
};

const static DWORD MOTIONTAG_MAX_BLOCK = 32;
const static DWORD MOTIONTAG_MAX_FLAG  = 8;

struct MotionTagBlockMeteor
{
	enuMotionTagBlockType Type;
	DWORD dwNumFlag;
	DWORD dwFlags[MOTIONTAG_MAX_FLAG];
	TCHAR strIndicator[MOTIONTAG_MAX_FLAG][MAX_BONE_NAME_LENGTH];
	MotionTagBlockMeteor()
	{
		Type = MTBT_METEOR;
		dwNumFlag = 0;
		memset(dwFlags, 0, sizeof(DWORD) * MOTIONTAG_MAX_FLAG);
		memset(strIndicator, 0, sizeof(TCHAR) * MOTIONTAG_MAX_FLAG * MAX_BONE_NAME_LENGTH);
	}
};

struct MotionTagDataRuntime
{
	TCHAR strTagName[TAG_NAME_MAX_LENGTH];
	MotionTagRunState RunState;
	std::vector<void*> Blocks;
};

struct TagSourceInfo
{
	DWORD dwType;
	DWORD dwFrame;
	DWORD dwIndex;
	DWORD dwHash;
	TCHAR strFileName[MAX_PATH];
	std::string strFilePath;
};

struct SoundTagInfo 
{
    int nVersion;
    DWORD dwFrame;
    DWORD dwIndex;
    TCHAR strFileName[MAX_PATH];
    FLOAT fVolume;
    SoundType soundType;
};
#define GETID(a, b) ((a & 0x0000ffff) << 16 | (b &0x0000ffff))

class IEKG3DAnimationTag
{
public:
	const static DWORD INVALIDATESOURCEREFCOUNT = 0xffffffff;
	virtual UINT ModifyTag(UINT uFrame, void* Data) = 0;
    virtual UINT RemoveTag(UINT uID) = 0;
    virtual UINT MoveTag(UINT uID, UINT uFrameTo) = 0;
    virtual UINT CopyTag(UINT uID, UINT uFrameDest) = 0;
	virtual HRESULT SaveToMemory(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame) = 0;
    virtual UINT GetNumKeyFrame(UINT uFrame) = 0;
	virtual DWORD GetSourceReference(TagSourceInfo *pBuffer, DWORD dwSize) = 0;
	virtual HRESULT ChangeResource(UINT uFrame, UINT uIndex, LPCSTR strNew) = 0;
	virtual HRESULT DeleteResource(UINT uFrame, UINT uIndex) = 0;
	virtual HRESULT AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault) = 0;
	virtual HRESULT ChangePlayType(UINT uFrame, UINT uIndex, void *pDefault) = 0;
	virtual HRESULT DeleteAllSound() = 0;
};

class IEKG3DAnimationMotionTag : public IEKG3DAnimationTag
{
public:
	const static UINT INVALIDATE_FRAME = 0xffffffff;
	virtual DWORD GetMotionTagType(UINT uFrame) = 0;
    //virtual UINT GetKeyFrameData(UINT uFrame, MotionTagDataInfo& Data) = 0;
	//virtual UINT GetKeyFrameData(UINT uFrame, UINT uIndex, void* pData) = 0;
	virtual void* GetNewBlock(DWORD dwType) = 0;
	virtual DWORD GetBlockType(DWORD dwFrame, DWORD dwIndex) = 0;
	virtual UINT GetBindedSocketName(vector<string> &vecSocketName) = 0;
	virtual UINT GetKeyFrame(UINT uFrame, MotionTagDataRuntime* &pData) = 0;
	virtual void ClearUnused() = 0;
	
	virtual UINT GetMotionStart() = 0;
	virtual UINT GetMotionEnd() = 0;
	virtual void SetMotionStart(UINT uFrame) = 0;
	virtual void SetMotionEnd(UINT uFrame) = 0;
};

class IEKG3DAnimationSFXTag : public IEKG3DAnimationTag
{
public:
	virtual HRESULT GetTagFrameIndexByID(UINT uId, UINT *pFrame, UINT *pIndex) = 0;
    virtual HRESULT GetTagIDByFrameIndex(UINT uFrame, UINT uIndex, UINT *pID) = 0;
	virtual HRESULT GetKeyFrameData(UINT uID, const SFXKeyFrameInfoVersion1** pKeyFrameData) = 0;
	virtual HRESULT GetKeyFrameName(UINT uID, LPCSTR* strTagName) = 0;
    virtual HRESULT GetSFXFileName(UINT uIndex, LPCSTR* strFileName) = 0;
    virtual HRESULT GetBindInfoInstance(IKG3DModel** ppBindInfo) = 0;
};

class IEKG3DAnimationSoundTag : public IEKG3DAnimationTag
{
public:
    virtual HRESULT GetKeyFrameData(UINT uFrame, const SoundDataInfoVersion2** pKeyFrameData) = 0;
	virtual HRESULT GetKeyFrameData(UINT uFrame, const SoundDataInfoVersion3** pKeyFrameData) = 0;
	virtual HRESULT DeleteSoundCandidate(UINT uFrame, UINT uIndex) = 0;
	virtual HRESULT GetSound(SoundType soundType,AnimationSoundTagInfo * pInfo) = 0;
	virtual HRESULT ClearTag(UINT uID) = 0;
    virtual BOOL IsModify() = 0;
    virtual HRESULT GetSoundTagInfo(void *pDefault) = 0;
    virtual HRESULT Play3DSound(LPSTR pFileName,FLOAT fVolume) = 0;
};

class IEKG3DAnimationTagContainer
{
public:
	virtual BOOL IsLoaded() = 0;
	virtual HRESULT AttachToModel(IKG3DModel* pModel) = 0;
    virtual IEKG3DAnimationTag* GetTag(enuTagType Type) = 0;
    virtual IKG3DModel* GetAttachedModel() = 0;
    virtual HRESULT Start(float fSpeed, int nOffsetTime, DWORD dwType, enuAnimationControllerPriority Priority) = 0;
    virtual HRESULT SetAnimation(LPCSTR strFileName) = 0;
    virtual HRESULT Load(LPCSTR strFileName) = 0;
    virtual HRESULT Save(LPCSTR strFileName) = 0;
    virtual LPCSTR GetAnimationFilePath() = 0;
    virtual BOOL GetModifyState() = 0;
    virtual void SetModifyState() = 0;
    virtual void Clear() = 0;
    virtual HRESULT GetCurrenetAnimation(IEKG3DClip** ppClip) = 0;
	virtual void OnExit() = 0;
	virtual HRESULT ExportTani(LPCSTR strFileName,DWORD dwMaxFrames) = 0;
	virtual DWORD GetSourceReference(TagSourceInfo *pBuffer, DWORD dwSize) = 0;
};
