#pragma once
//这个文件现在被SceneEditor当接口头文件使用了，小心不要在这里include别的引擎的文件

enum enuClipType
{
	ANIMATION_NONE                = 0,
	ANIMATION_VERTICES            = 1,
	ANIMATION_VERTICES_RELATIVE   = 2,
	ANIMATION_BONE                = 10,
	ANIMATION_BONE_RELATIVE       = 11,
	ANIMATION_BONE_16             = 12,
	ANIMATION_BONE_RTS            = 13,
	ANIMATION_COUNT,
	ANIMATION_FORCE_DWORD = 0xffffffff,
};

enum enuClipOption
{
	ENUM_CLIPOP_UVANI = 0x1,
	ENUM_CLIPOP_FORCE_DWORD = 0xffffffff,
};

enum enuClipLoadOption
{
	ENUM_CLIPLOADOPT_NORMAL = 0,
	ENUM_CLIPLOADOPT_MASKLOADED = 1,
};

enum enuRTSFlag
{
	Bone_Rotation = 1,
	Bone_Scale = 1 << 1,
	Bone_Translation = 1 << 2,
	Bone_Rotation_Reverse = 1 << 3,
	Bone_Scale_Reverse = 1 << 4,
	Bone_Translation_Reverse = 1 << 5,
	Bone_Rotation_None = 1 << 6,
	Bone_Scale_None = 1 << 7,
	Bone_Translation_None = 1 << 8,
	Bone_Translation_Mask = Bone_Translation | Bone_Translation_None | Bone_Translation_Reverse,
	Bone_Scale_Mask = Bone_Scale | Bone_Scale_None | Bone_Scale_Reverse,
	Bone_Rotation_Mask = Bone_Rotation | Bone_Rotation_None | Bone_Rotation_Reverse,
	NORMAL_RTS = Bone_Rotation | Bone_Scale | Bone_Translation,
};


struct RTS
{
	D3DXVECTOR3 Translation;
	D3DXVECTOR3 Scale;
	D3DXQUATERNION Rotation;
	float Sign;
	D3DXQUATERNION SRotation;
};

class KG3DAnimationUpdateExtraInfoCollector
{
public:
	virtual void SaveData(DWORD dwIndex, void *pData) = 0;
	virtual void* GetData() = 0;
	virtual DWORD GetSize() = 0;
	virtual void SetSize(DWORD dwSize) = 0;
	virtual void* GetData(DWORD dwIndex) = 0;
	virtual HRESULT CreateStore(KG3DModel* pModel) = 0;
	virtual void Clear() = 0;
	virtual void SetData(void* pData, size_t NumData) = 0;
	virtual ~KG3DAnimationUpdateExtraInfoCollector(){};
};
