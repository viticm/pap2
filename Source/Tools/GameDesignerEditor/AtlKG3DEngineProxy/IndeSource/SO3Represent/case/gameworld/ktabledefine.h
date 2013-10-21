#ifndef RL_GAMEWORLD_TABLE_KTABLEDEFINE_H
#define RL_GAMEWORLD_TABLE_KTABLEDEFINE_H

#include "../render/krlresource.h"
#include "../../base/missile/krepresentmissiledefine.h"

enum RL_LOAD_OPTIMIZATION
{
	RL_MODEL_OPTIMIZATION_LOAD_NONE,
	RL_MODEL_OPTIMIZATION_LOAD_NORMAL,
	RL_MODEL_OPTIMIZATION_LOAD_UNIQUE_UNIFORM,
	RL_MODEL_OPTIMIZATION_LOAD_GENDER_UNIFORM,
	RL_MODEL_OPTIMIZATION_LOAD_ROLE_TYPE_UNIFORM,
	RL_MODEL_OPTIMIZATION_LOAD_FORCE_UNIFORM,
};

enum RL_PLAYER_UNIFORM
{
	RL_PLAYER_UNIFORM_UNIQUE,

	RL_PLAYER_UNIFORM_MALE,
	RL_PLAYER_UNIFORM_FEMALE,
};

enum RL_PLAYER_ROLETYPE_UNIFORM
{
	RL_PLAYER_ROLETYPE_UNIFORM_STANDARD_MALE,
	RL_PLAYER_ROLETYPE_UNIFORM_STANDARD_FEMALE,

	RL_PLAYER_ROLETYPE_UNIFORM_STRONG_MALE,
	RL_PLAYER_ROLETYPE_UNIFORM_SEXY_FEMALE,

	RL_PLAYER_ROLETYPE_UNIFORM_LITTLE_BOY,
	RL_PLAYER_ROLETYPE_UNIFORM_LITTLE_GIRL,
};

enum RL_NPC_UNIFORM
{
	RL_NPC_UNIFORM_NONE,
	RL_NPC_UNIFORM_TRANSPARENCY,
	RL_NPC_UNIFORM_TRANSPARENCY_CORPSE,
	RL_NPC_UNIFORM_TRANSPARENCY_UNSELECTABLE,
	RL_NPC_UNIFORM_UNIQUE,
};

enum RL_DOODAD_UNIFORM
{
	RL_DOODAD_UNIFORM_NONE,
	RL_DOODAD_UNIFORM_CORPSE,
	RL_DOODAD_UNIFORM_DOODAD,
};

enum RL_FORCE_RELATION
{ 
	RL_FORCE_RELATION_INVALID,
	RL_FORCE_RELATION_ENEMY,
	RL_FORCE_RELATION_NEUTRALITY,
	RL_FORCE_RELATION_PARTY,
	RL_FORCE_RELATION_ALLY,
	RL_FORCE_RELATION_SELF,
	RL_FORCE_RELATION_NONE,
	RL_FORCE_RELATION_ALL,

	RL_FORCE_RELATION_COUNT,
};

enum RL_ANIMATION_TRACK
{
	RL_ANIMATION_TRACK_DOWN,
	RL_ANIMATION_TRACK_UP,
	RL_ANIMATION_TRACK_COUNT,
};

enum
{
	HAIR_TYPE_NORMAL,
	HAIR_TYPE_BARE,
};

enum
{
	HAT_TYPE_DOULI,     // 斗笠（主要影响头顶部分）
	HAT_TYPE_CORONAL,   // 冠（束发的都包含比如方巾）
	HAT_TYPE_HAT,       // 帽子（完全改变发型比如头盔，蒙头）
	HAT_TYPE_FILLET,    // 头带（对发型完全没影响包含面罩，眼罩，围巾）

	HAT_TYPE_COUNT,
};

// Sound
struct KRLSoundModel
{
	DWORD   dwSoundID;
	float	fSoundMinRange;
	float   fSoundMaxRange;
	LPCTSTR szSoundFile;

	bool operator<(KRLSoundModel const& rhs) const
	{
		return dwSoundID < rhs.dwSoundID;
	}
};

// Relation
struct KForceRelationModel
{
	int nForeceRelationType;
	LPCTSTR szDesc;
	DWORD dwColor;
	float fSFXScale;
	LPCTSTR szSFXFile;
};

enum
{
	RL_NUMBER_NONE,

	RL_NUMBER_HEAD_Y_OFFSET_NONE,
	RL_NUMBER_HEAD_Y_OFFSET_LIFE,
	RL_NUMBER_HEAD_Y_OFFSET_NAME,
	RL_NUMBER_HEAD_Y_OFFSET_TITLE,
	RL_NUMBER_HEAD_Y_OFFSET_MARK,

	RL_NUMBER_CAST_SKILL_RESET_PERIOD,
	RL_NUMBER_SKILL_EFFECT_RESULT_TIMEOUT,

	RL_NUMBER_CHARACTER_WALK_SPEED,
	RL_NUMBER_CHARACTER_RUN_SPEED,
	RL_NUMBER_CHARACTER_SWIM_SPEED,
	RL_NUMBER_CHARACTER_YAW_TURN_SPEED,
	RL_NUMBER_CHARACTER_YAW_TURN_RESET_SPEED,

	RL_NUMBER_CHARACTER_RIDE_WALK_SPEED,
	RL_NUMBER_CHARACTER_RIDE_RUN_SPEED,
	RL_NUMBER_CHARACTER_RIDE_RUN_FASTER_SPEED,
	RL_NUMBER_CHARACTER_RIDE_YAW_TURN_SPEED,
	RL_NUMBER_CHARACTER_RIDE_YAW_TURN_RESET_SPEED,

	RL_NUMBER_CHARACTER_VEHICLE_YAW_TURN_SPEED,

	RL_NUMBER_CAMERA_SPEED_RATIO,
	RL_NUMBER_CAMERA_RIDE_YAW_OFFSET,

	RL_NUMBER_MISSILE_LIVE_TIME,

	RL_NUMBER_CAPTION_FLOOR_SPACE,

	RL_NUMBER_COUNT,
};

struct KRLNumberModel
{
	DWORD dwID;
	LPCTSTR szDesc;
	float fHeight;

	bool operator<(KRLNumberModel const& rhs) const
	{
		return dwID < rhs.dwID;
	}
};

#define RL_GLOBAL_EFFECT_NONE 0

struct KGlobalEffectModel
{
	DWORD dwEffectID;
	LPCTSTR szDesc;
	DWORD dwPlayType;
	float fSpeed;
	float fScale;
	LPCTSTR szFilePath;

	bool operator<(KGlobalEffectModel const& rhs) const
	{
		return dwEffectID < rhs.dwEffectID;
	}
};

struct KRLCursorEffectModel
{
	DWORD dwEffectID;
	LPCTSTR szDesc;
	DWORD dwPlayType;
	float fSpeed;
	float fScale;
	LPCTSTR szFilePath;

	bool operator<(KRLCursorEffectModel const& rhs) const
	{
		return dwEffectID < rhs.dwEffectID;
	}
};

// Skill
struct KSkillBuffModel
{
	DWORD    dwPart;
	DWORD    dwSkillBuffID;
	LPCTSTR  szPartName;
	LPCTSTR  szBoneNameLeft;
	LPCTSTR  szBoneNameRight;
	float    fScale_m1;
	float    fScale_m2;
	float    fScale_m3;
	float    fScale_f1;
	float    fScale_f2;
	float    fScale_f3;
	DWORD    dwPlayType;
	BOOL     bShockWave;
	LPCTSTR  szSFXFilePath;

	bool operator<(KSkillBuffModel const& rhs) const
	{
		if (dwPart == rhs.dwPart)
			return dwSkillBuffID < rhs.dwSkillBuffID;
		return dwPart < rhs.dwPart;
	}
};

struct KSkillEffectModel 
{
	DWORD   dwEffectID;
	LPCTSTR szNote;
	BOOL    bAdjustDirection;
	BOOL    bBind; 
	float   fSFXScale;
	LPCTSTR szBoneName;
	LPCTSTR szSFXFilePath;

	bool operator<(KSkillEffectModel const& rhs) const
	{
		return dwEffectID < rhs.dwEffectID;
	}
};

struct KSkillResultModel 
{
	DWORD dwEffectResultID;
	DWORD dwEffectType;
	DWORD dwEffectID;
	LPCTSTR szNote;

	bool operator<(KSkillResultModel const& rhs) const
	{
		if (dwEffectResultID == rhs.dwEffectResultID)
			return dwEffectType < rhs.dwEffectType;
		return dwEffectResultID < rhs.dwEffectResultID;
	}
};

enum { RL_SKILL_SEQUENCE_COUNT = 4 };


struct KSkillCasterModel
{
	DWORD   dwSkillID;
	DWORD   dwSkillLevel;
	DWORD   dwMissileID;
	DWORD   dwLockControlTime;
	DWORD   dwPrepareCastSkillAnimationID;
	DWORD   dwCastSkillAnimationIDs[RL_SKILL_SEQUENCE_COUNT];
	DWORD   dwSkillEffectResultID[REPRESENT_SKILL_EFFECT_RESULT_END];
	float   fAOESelectionSFXScale;
	LPCTSTR szAOESelectionSFXFile;
	BOOL	bPlayBehitAnimation;

	bool operator<(KSkillCasterModel const& rhs) const
	{
		if (dwSkillID == rhs.dwSkillID)
			return dwSkillLevel < rhs.dwSkillLevel;
		return dwSkillID < rhs.dwSkillID;
	}
};

struct KNpcSkillCasterModel
{
	DWORD dwRepresentID;
	KSkillCasterModel Model;

	bool operator<(KNpcSkillCasterModel const& rhs) const
	{
		if (dwRepresentID == rhs.dwRepresentID)
		{
			if (Model.dwSkillID == rhs.Model.dwSkillID)
				return Model.dwSkillLevel < rhs.Model.dwSkillLevel;
			return Model.dwSkillID < rhs.Model.dwSkillID;
		}
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KSkillChainModel
{
	DWORD   dwSkillID;
	DWORD   dwSkillLevel;
	LPCTSTR szSrcBone;
	LPCTSTR szDstBone;
	LPCTSTR szFilePath;

	bool operator<(KSkillChainModel const& rhs) const
	{
		if (dwSkillID == rhs.dwSkillID)
			return dwSkillLevel < rhs.dwSkillLevel;
		return dwSkillID < rhs.dwSkillID;
	}
};

// TODO : will be removed Missile
struct KMissilePhaseBaseModel
{
	DWORD   dwMissilePhaseID;
	LPCTSTR szDesc;
	float   fAnimationSpeed;
	float   fModelScaleBegin;
	float   fModelScaleEnd;
	int     nAnimationType;
	LPCTSTR szMeshFile;
	LPCTSTR szMaterialFile;
	LPCTSTR szAnimationFile;
	LPCTSTR szSFXFile;
	int     nCasterType;
	int     nMovementType;
	int     nSymmetricalType;
	int     nMissileDirection;
	int     nMissileCount;
	int     nReferenceType;
	int     nZLockType;
	LPCTSTR szBoneNameCaster;   // TODO: will be removed
	LPCTSTR szBoneNameTarget;
	BOOL    bShockWave;

	bool operator<(KMissilePhaseBaseModel const& rhs) const
	{
		return dwMissilePhaseID < rhs.dwMissilePhaseID;
	}
};

struct KMissilePhaseLineModel : KMissilePhaseBaseModel
{    
	int     nTerrainOffsetBegin;
	int     nTerrainOffsetEnd;
	int     nLineHeightBegin;
	int     nLineHeightEnd;
	int     nLineWidthBegin;
	int     nLineWidthEnd;
	int     nParabolaAngle;
	float   fPervasionTime;
	float   fRotationFrame;
	float   fSpeedScale;
	int     nMeteorRadius;
	int     nAcceleration;

	bool operator<(KMissilePhaseLineModel const& rhs) const
	{
		return dwMissilePhaseID < rhs.dwMissilePhaseID;
	}
};

struct KMissilePhaseCircleModel : KMissilePhaseBaseModel
{    
	int     nTerrainOffsetBegin;
	int     nTerrainOffsetEnd;
	int     nLineHeightBegin;
	int     nLineHeightEnd;
	int     nFOVBegin;
	int     nFOVEnd;
	int     nCircleRadiusBegin;
	int     nCircleRadiusEnd;
	int     nFOVAngleVelocityBegin;
	int     nFOVAngleVelocityEnd;
	int     nParabolaAngle;
	float   fPervasionTime;
	float   fSpeedScale;
	int     nMeteorRadius;
	int     nAcceleration;

	bool operator<(KMissilePhaseCircleModel const& rhs) const
	{
		return dwMissilePhaseID < rhs.dwMissilePhaseID;
	}
};

struct KMissilePhaseHomingModel : KMissilePhaseBaseModel
{    
	int     nParabolaAngle;
	float   fSpeedScale;
	int     nMeteorRadius;

	bool operator<(KMissilePhaseHomingModel const& rhs) const
	{
		return dwMissilePhaseID < rhs.dwMissilePhaseID;
	}
};

struct KMissilePhaseFixedTrackModel : KMissilePhaseBaseModel
{    
	int nFixedTrackID[MISSILE_PHASE_SFX_COUNT];

	bool operator<(KMissilePhaseFixedTrackModel const& rhs) const
	{
		return dwMissilePhaseID < rhs.dwMissilePhaseID;
	}
};

struct KMissilePhaseFulminationModel : KMissilePhaseBaseModel
{    
	float fPervasionTime;

	bool operator<(KMissilePhaseFulminationModel const& rhs) const
	{
		return dwMissilePhaseID < rhs.dwMissilePhaseID;
	}
};

struct KRLMissileModelParam
{
	DWORD dwMissileParamID;
	LPCTSTR szDesc;

	float fAnimationSpeed;
	float fModelScaleBegin;
	float fModelScaleEnd;
	int nAnimationType;
	BOOL bBindTarget;
	LPCTSTR szMeshFile;
	LPCTSTR szMaterialFile;
	LPCTSTR szAnimationFile;
	LPCTSTR szSFXFile;
	BOOL bShockWave;

	bool operator<(KRLMissileModelParam const& rhs) const
	{
		return dwMissileParamID < rhs.dwMissileParamID;
	}
};

struct KRLMissileHomingParam
{
	DWORD dwMissileParamID;
	LPCTSTR szDesc;

	LPCTSTR szCasterBoneName;
	LPCTSTR szTargetBoneName;

	float fInitialVelocity;
	float fAcceleration;
	float fTargetRadius;
	float fConvergenceFactor;

	bool operator<(KRLMissileHomingParam const& rhs) const
	{
		return dwMissileParamID < rhs.dwMissileParamID;
	}
};

struct KRLMissileTimelineParam
{
	DWORD dwMissileParamID;
	LPCTSTR szDesc;

	float fMillisecond;

	bool operator<(KRLMissileTimelineParam const& rhs) const
	{
		return dwMissileParamID < rhs.dwMissileParamID;
	}
};

struct KRLMissileYAxisParam
{
	DWORD dwMissileParamID;
	LPCTSTR szDesc;

	float fY;

	bool operator<(KRLMissileYAxisParam const& rhs) const
	{
		return dwMissileParamID < rhs.dwMissileParamID;
	}
};

struct KRLMissileProcessorParam
{
	DWORD dwModelParamID;
	DWORD dwHomingParamID;
	DWORD dwYAxisParamID;
	DWORD dwTimelineParamID;
};

struct KRLMissileModel
{
	DWORD dwMissileID;
	LPCTSTR szDesc;

	KRLMissileProcessorParam Params[RL_MISSILE_PHASE_COUNT];

	bool operator<(KRLMissileModel const& rhs) const
	{
		return dwMissileID < rhs.dwMissileID;
	}
};

// Doodad
struct KRLDoodadModel
{
	struct KRLAnimationParam
	{
		LPCTSTR szMeshFilePath;
		LPCTSTR szMtlFilePath;
		LPCTSTR szAniFilePath;
		DWORD dwAniPlayType;
		LPCTSTR szSFXFilePath;
	};

	DWORD dwRepresentID;
	BOOL bOnFloor;
	BOOL bFadeIn;
	int nFogVisible;
	int nColorChannelTable;
	int nColorChannel;
	BOOL bMinimapShow;
	int nMinimapShowType;
	BOOL bSelectable;
	int nVisiblePriority;
	float fModelScale;
	float fEffectScale;
	DWORD dwPlayerOpenAnimationID;
	DWORD dwPlayerBeginPickAnimationID;
	DWORD dwPlayerEndPickAnimationID;
	KRLAnimationParam Animations[RL_DOODAD_ANIMATION_COUNT];

	bool operator<(KRLDoodadModel const& rhs) const
	{
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KDoodadEffectModel
{
	DWORD   dwEffectID;
	LPCTSTR szSFXFilePath;
	float   fSpeed;
	DWORD   dwPlayType;
	float   fScale;
	LPCTSTR szBoneName;

	bool operator<(KDoodadEffectModel const& rhs) const
	{
		return dwEffectID < rhs.dwEffectID;
	}
};

// Rides
struct KRidesModel
{
	DWORD   dwRepresentID;
	float   fCameraAdjust;
	int     nSlopeType;
	LPCTSTR szMDLFilePath;
	float   fModelScale;
	float   fSocketScale;
	float   fSelectionScale;
	DWORD   dwIdleAnimationID;
	int nColorChannelTable;
	int nColorChannel;

	bool operator<(KRidesModel const& rhs) const
	{
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KRidesEquipModel
{
	DWORD   dwRidesID;
	DWORD   dwModelID;
	int     nColorChannel;
	float   fScale;
	LPCTSTR szMesh;
	LPCTSTR szMaterial;

	bool operator<(KRidesEquipModel const& rhs) const
	{
		if (dwRidesID == rhs.dwRidesID)
			return dwModelID < rhs.dwModelID;
		return dwRidesID < rhs.dwRidesID;
	}
};

struct KRidesAnimationModel
{
	DWORD dwRepresentID;
	DWORD dwAnimationID;
	DWORD dwAnimationType;
	float fAnimationSpeed;
	DWORD dwSFXType;
	float fSFXSpeed;
	float fSFXScale;
	LPCTSTR szBoneName;
	LPCTSTR szSFXFile;
	LPCTSTR szAnimationFile;

	bool operator<(KRidesAnimationModel const& rhs) const
	{
		if (dwRepresentID == rhs.dwRepresentID)
			return dwAnimationID < rhs.dwAnimationID;
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KRidesAnimationFusion
{
	DWORD   dwRidesRepresentID;
	DWORD   dwPreviousAnimationID;
	DWORD   dwCurrentAnimationID;
	LPCTSTR szDesc;
	DWORD   dwFusionTime;

	bool operator<(KRidesAnimationFusion const& rhs) const
	{
		if (dwRidesRepresentID == rhs.dwRidesRepresentID)
		{
			if (dwPreviousAnimationID == rhs.dwPreviousAnimationID)
				return dwCurrentAnimationID < rhs.dwCurrentAnimationID;
			return dwPreviousAnimationID < rhs.dwPreviousAnimationID;
		}
		return dwRidesRepresentID < rhs.dwRidesRepresentID;
	}
};

struct KRidesForTrack
{
	DWORD dwTrackID;
	DWORD dwRidesID;

	bool operator<(KRidesForTrack const& rhs) const
	{
		return dwTrackID < rhs.dwTrackID;
	}
};

// Npc
struct KRLNpcModel
{
	struct KSocket
	{
		LPCTSTR szMeshFilePath;
		LPCTSTR szMtlFilePath;
	};

	struct KIdleAnimation
	{
		DWORD dwAnimationID[RL_NPC_IDLE_ANIMATION_COUNT];
		DWORD dwRatio[RL_NPC_IDLE_ANIMATION_COUNT];
	};

	struct KNormalAnimation
	{
		DWORD dwAnimationID[RL_NPC_NORMAL_ANIMATION_COUNT];
		DWORD dwRatio[RL_NPC_NORMAL_ANIMATION_COUNT];
	};

	DWORD dwRepresentID;
	LPCTSTR szMap;
	LPCTSTR szModel;
	BOOL bSplitModel;
	float fHeight;
	float fTitleOffsetY;
	int nTerrainSlope;
	LPCTSTR szMainModelFile;
	KSocket Sockets[RL_NPC_SOCKET_COUNT];
	int nColorChannelTable;
	int nColorChannel;
	int nMinimapShowType;
	BOOL bSelectable;
	float fModelScale;
	float fSocketScale;
	float fSelectionScale;
	float fEffectScale;
	DWORD dwEffectType;
	KIdleAnimation IdleAnimation;
	KNormalAnimation NormalAnimations[RL_NPC_ANIMATION_TABLE_COUNT - RL_NPC_ANIMATION_TABLE_SIT_DOWN];

	bool operator<(KRLNpcModel const& rhs) const
	{
		return dwRepresentID < rhs.dwRepresentID;
	}

	static void  Init(KRLNpcModel* pData, size_t nCount);
	static DWORD GetAnimationID(KRLNpcModel const* pData, RL_NPC_ANIMATION_TABLE nAnimationTable);
};

struct KNpcAnimationModel
{
	struct KSocketAnimation
	{
		LPCTSTR szMeshFilePath;
		LPCTSTR szMaterialFile;
	};

	DWORD dwRepresentID;
	DWORD dwAnimationID;
	DWORD dwKindID;
	DWORD dwAnimationType;
	DWORD dwAttackMove;
	BOOL bSFXShockWave;
	BOOL bAnimationShockWave;
	float fAnimationSpeed;
	float fSFXSpeed;
	DWORD dwSFXType;
	float fSFXScale;
	LPCTSTR szSFXBoneName;
	LPCTSTR szSFXFile;
	BOOL bLoadDefaultSocket;
	LPCTSTR szAnimationFile;
	KSocketAnimation SocketInfo[RL_NPC_SOCKET_COUNT];
	LPCTSTR szMap;

	bool operator<(KNpcAnimationModel const& rhs) const
	{
		if (dwRepresentID == rhs.dwRepresentID)
			return dwAnimationID < rhs.dwAnimationID;
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KRLNpcAdjustAnimationIndex
{
	DWORD dwRepresentID;
	DWORD dwAnimationID;
	DWORD dwRidesAnimationID;
	DWORD dwSwimAnimationID;
	DWORD dwJumpAnimationID;
	DWORD dwSpeedupAnimationID;
	DWORD dwWaterFlyAnimationID;

	bool operator<(KRLNpcAdjustAnimationIndex const& rhs) const
	{
		if (dwRepresentID == rhs.dwRepresentID)
			return dwAnimationID <rhs.dwAnimationID;
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KNpcSerialAnimation
{
	DWORD   dwSerialID;
	LPCTSTR szDesc;
	DWORD   dwPhaseAnimationIDs[3];

	bool operator<(KNpcSerialAnimation const& rhs) const
	{
		return dwSerialID < rhs.dwSerialID;
	}
};

// Player
enum
{
	RL_EQUIP_HIDE_CHEST,
	RL_EQUIP_HIDE_PANTS,
	RL_EQUIP_HIDE_HAT,
	RL_EQUIP_HIDE_FACE,
	RL_EQUIP_HIDE_BANGLE,
	RL_EQUIP_HIDE_MELEE_WEAPON,
	RL_EQUIP_HIDE_WAIST,
	RL_EQUIP_HIDE_BACK_EXTEND,
	RL_EQUIP_HIDE_WAIST_EXTEND,

	RL_EQUIP_HIDE_COUNT,
};

struct KRLPlayerModel
{
	int nRoleType;
	LPCTSTR szDesc;
	float fEyeAltitude;
	LPCTSTR szMDLFilePath;
	LPCTSTR szBalloonFilePath;
	DWORD dwBalloonTextureIndex;
	float fBalloonScale;
	float fTitleOffsetY;
	float fSelectionScale;
	float fModelScale;

	struct
	{
		LPCTSTR szLevelUpFilePath;
		float fLevelUpScale;
	}
	SFXs[REPRESENT_SFX_COUNT];
};

struct KPlayerEquipModel
{
	DWORD   dwRepresentID;
	float   fScale;
	int     nColorChannel;
	BOOL    bHideFlag[RL_EQUIP_HIDE_COUNT];
	LPCTSTR szSocketName;
	LPCTSTR szMesh;
	LPCTSTR szMaterial;

	bool operator<(KPlayerEquipModel const& rhs) const
	{
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KPlayerChestEquipModel
{
	DWORD   dwRepresentID;
	float   fScale;
	int     nColorChannel;
	BOOL    bHideFlag[RL_EQUIP_HIDE_COUNT];
	LPCTSTR szSocketName;
	LPCTSTR szMesh;
	LPCTSTR szMaterial;
	DWORD   dwEffectType;

	bool operator<(KPlayerChestEquipModel const& rhs) const
	{
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KPlayerHatModel
{
	DWORD   dwRepresentID;
	float   fScale;
	int     nColorChannel;
	BOOL    bHideFlag[RL_EQUIP_HIDE_COUNT];
	LPCTSTR szSocketName;
	LPCTSTR szMesh;
	LPCTSTR szMaterial;
	int     nType;

	bool operator<(KPlayerHatModel const& rhs) const
	{
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KPlayerHairModel
{
	DWORD   dwRepresentID;
	float   fScale;
	int     nColorChannel;
	BOOL    bHideFlag[RL_EQUIP_HIDE_COUNT];
	LPCTSTR szSocketName;
	LPCTSTR szMesh;
	LPCTSTR szMaterial;
	int     nType;

	bool operator<(KPlayerHairModel const& rhs) const
	{
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KPlayerHeadIndex
{
	DWORD dwHeadID;
	DWORD dwHeadformID;
	DWORD dwBangID;
	DWORD dwPlaitID;

	bool operator<(KPlayerHeadIndex const& rhs) const
	{
		return dwHeadID < rhs.dwHeadID;
	}
};

struct KPlayerHairHatRelation
{
	int dwOriginalHairID;
	int dwFinallyHairIDs[HAT_TYPE_COUNT];    // 头发在不同帽子类型下的表现ID

	bool operator<(KPlayerHairHatRelation const& rhs) const
	{
		return dwOriginalHairID < rhs.dwOriginalHairID;
	}
};

struct KPlayerWeaponModel
{
	struct KModelBaseData
	{
		LPCTSTR szSocketName;
		LPCTSTR szMesh;
		LPCTSTR szMaterial;
		LPCTSTR szMeshSheath;
		LPCTSTR szMaterialSheath;
	};

	DWORD           dwRepresentID;
	int             nWeaponType;
	float           fScale;
	int             nColorChannel;
	BOOL            bHideFlag[RL_EQUIP_HIDE_COUNT];
	int             nSheathSocket;
	KModelBaseData  ModelBaseLH;
	KModelBaseData  ModelBaseRH;

	bool operator<(KPlayerWeaponModel const& rhs) const
	{
		return dwRepresentID < rhs.dwRepresentID;
	}
};

struct KPlayerEquipSFXModel
{
	DWORD dwEquipID;
	DWORD dwEnchantID;
	LPCTSTR szSFXFileLeft;
	LPCTSTR szSFXFileRight;

	bool operator<(KPlayerEquipSFXModel const& rhs) const
	{
		if (dwEquipID == rhs.dwEquipID)
			return dwEnchantID < rhs.dwEnchantID;
		return dwEquipID < rhs.dwEquipID;
	}
};

struct KRLPlayerEquipSFXSequenceIndex
{
	DWORD dwEquipType;
	LPCTSTR szDesc;
	LPCTSTR szSFXSequenceFilePaths[rtTotal];

	bool operator<(KRLPlayerEquipSFXSequenceIndex const& rhs) const
	{
		return dwEquipType < rhs.dwEquipType;
	}
};

enum { RL_PLAYER_EQUIP_SEQUENCE_SFX_COUNT = 8 };

struct KRLPlayerEquipSFXSequence
{
	DWORD dwEquipID;
	DWORD dwSequenceID;
	LPCTSTR szDesc;
	DWORD dwSFXIDs[RL_PLAYER_EQUIP_SEQUENCE_SFX_COUNT];

	bool operator<(KRLPlayerEquipSFXSequence const& rhs) const
	{
		if (dwEquipID == rhs.dwEquipID)
			return dwSequenceID < rhs.dwSequenceID;
		return dwEquipID < rhs.dwEquipID;
	}
};

struct KRLPlayerEquipSFXModelParam
{
	DWORD dwSFXID;
	LPCTSTR szDesc;
	LPCTSTR szBoneNameLeft;
	LPCTSTR szBoneNameRight;
	float fScale;
	LPCTSTR szSFXFilePath;

	bool operator<(KRLPlayerEquipSFXModelParam const& rhs) const
	{
		return dwSFXID < rhs.dwSFXID;
	}
};

enum RL_SHEATH_SOCKET
{
	RL_SHEATH_SOCKET_GAMEWORLD,
	RL_SHEATH_SOCKET_SHEATH,
	RL_SHEATH_SOCKET_UNSHEATH,
};

struct KRLPlayerAnimationModel
{
	DWORD dwAnimationID;
	DWORD dwKindID;
	int nSheathSocket;
	float fRatio;
	float fAnimationSpeed;
	DWORD dwAnimationType;
	LPCTSTR szAnimationFile;

	bool operator<(KRLPlayerAnimationModel const& rhs) const
	{
		return dwAnimationID < rhs.dwAnimationID;
	}
};

struct KRLPlayerAdjustAnimationIndex
{
	DWORD dwAnimationID;
	DWORD dwRidesAnimationID;
	DWORD dwSwimAnimationID;
	DWORD dwJumpAnimationID;
	DWORD dwSpeedupAnimationID;
	DWORD dwWaterFlyAnimationID;

	bool operator<(KRLPlayerAdjustAnimationIndex const& rhs) const
	{
		return dwAnimationID < rhs.dwAnimationID;
	}
};

struct KPlayerSerialAnimation
{
	DWORD   dwSerialID;
	LPCTSTR szDesc;
	DWORD   dwPhaseAnimationIDs[3];

	bool operator<(KPlayerSerialAnimation const& rhs) const
	{
		return dwSerialID < rhs.dwSerialID;
	}
};

struct KPlayerCraftAnimation
{
	int   nCraftID;
	DWORD dwAnimationID;

	bool operator<(KPlayerCraftAnimation const& rhs) const
	{
		return nCraftID < rhs.nCraftID;
	}
};

struct KPlayerBuffAnimation
{
	DWORD dwPart;
	DWORD dwBuffID;
	DWORD dwAnimationID;
	DWORD dwBuffAniID;

	bool operator<(KPlayerBuffAnimation const& rhs) const
	{
		if (dwPart == rhs.dwPart)
		{
			if (dwBuffID == rhs.dwBuffID)
				return dwAnimationID < rhs.dwAnimationID;
			return dwBuffID < rhs.dwBuffID;
		}
		return dwPart < rhs.dwPart;
	}
};

// common
struct KRLAnimationFusion
{
	DWORD   dwPreviousAnimationID;
	DWORD   dwCurrentAnimationID;
	LPCTSTR szDesc;
	DWORD   dwFusionTime;

	bool operator<(KRLAnimationFusion const& rhs) const
	{
		if (dwPreviousAnimationID == rhs.dwPreviousAnimationID)
			return dwCurrentAnimationID < rhs.dwCurrentAnimationID;
		return dwPreviousAnimationID < rhs.dwPreviousAnimationID;
	}
};


enum RL_ANIMATION_INDEX
{
	RL_ANIMATION_INDEX_NONE,
	RL_ANIMATION_INDEX_RANDOM,
	RL_ANIMATION_INDEX_SEQUENCE,
};

struct KRLAnimationIndex
{
	DWORD   dwAnimationIndexID;
	LPCTSTR szDesc;
	DWORD   dwAnimationIndexType;   // RL_ANIMATION_INDEX
	DWORD   dwID;

	bool operator<(KRLAnimationIndex const& rhs) const
	{
		return dwAnimationIndexID < rhs.dwAnimationIndexID;
	}
};


struct KRLRandomAnimationItem
{
	DWORD dwAnimationID;
	DWORD dwRatio;
};

enum { RL_RANDOM_ANIMATION_COUNT = 4 };

struct KRLRandomAnimation
{
	DWORD dwRandomAnimationID;
	LPCTSTR szDesc;

	KRLRandomAnimationItem Items[RL_RANDOM_ANIMATION_COUNT];

	bool operator<(KRLRandomAnimation const& rhs) const
	{
		return dwRandomAnimationID < rhs.dwRandomAnimationID;
	}
};


struct KRLSequenceAnimationItem
{
	DWORD dwAnimationID;
	DWORD dwType;
};

enum { RL_SEQUENCE_ANIMATION_COUNT = 4 };

struct KRLSequenceAnimation
{
	DWORD dwSequenceAnimationID;
	LPCTSTR szDesc;

	KRLSequenceAnimationItem Items[RL_SEQUENCE_ANIMATION_COUNT];

	bool operator<(KRLSequenceAnimation const& rhs) const
	{
		return dwSequenceAnimationID < rhs.dwSequenceAnimationID;
	}
};


struct KRLAdjustAnimationIndex
{
	DWORD dwRidesAnimationID;
	DWORD dwSwimAnimationID;
	DWORD dwJumpAnimationID;
	DWORD dwSpeedupAnimationID;
	DWORD dwWaterFlyAnimationID;
};


struct KRLTerrainAdjustAnimation
{
	DWORD dwAnimationID;
	DWORD dwTerrainID;
	LPCTSTR szDesc;
	DWORD dwAdjustAnimationID;

	bool operator<(KRLTerrainAdjustAnimation const& rhs) const
	{
		if (dwAnimationID == rhs.dwAnimationID)
			return dwTerrainID < rhs.dwTerrainID;
		return dwAnimationID < rhs.dwAnimationID;
	}
};

struct KRLCharacterParam
{
	BOOL bPlayer;

	ROLE_TYPE nRoleType;
	DWORD dwModelID;
};


struct KRLAdjustAnimationParam
{
	KRLCharacterParam CharacterParam;

	BOOL bRide;
	BOOL bSwim;
	BOOL bJump;
	BOOL bSpeedup;
	BOOL bAboveWater;

	RL_WEAPON nWeaponType;

	DWORD dwSkillBuffs[SKILL_BUFF_PART_COUNT];

	DWORD dwTerrainID;
};


enum RL_ADVANCE
{
	RL_ADVANCE_STRAFE_LEFT,
	RL_ADVANCE_STRAFE_RIGHT,
	RL_ADVANCE_FORWARD,
	RL_ADVANCE_BACKWARD,

	RL_ADVANCE_COUNT,
};

enum RL_FIGHT_FLAG
{
	RL_FIGHT_FLAG_NONE,
	RL_FIGHT_FLAG_FIGHT,

	RL_FIGHT_FLAG_COUNT,
};

enum RL_ANIMATION_DOWN
{
	RL_ANIMATION_DOWN_UNKNOWN = -1,

	RL_ANIMATION_DOWN_NONE = 0,
	RL_ANIMATION_DOWN_STAND,
	RL_ANIMATION_DOWN_TURN_LEFT,
	RL_ANIMATION_DOWN_TURN_RIGHT,
	RL_ANIMATION_DOWN_WALK,
	RL_ANIMATION_DOWN_RUN,
	RL_ANIMATION_DOWN_SWIM,
	RL_ANIMATION_DOWN_SWIM_JUMP,
	RL_ANIMATION_DOWN_SWIM_DOUBLE_JUMP,
	RL_ANIMATION_DOWN_FLOAT,
	RL_ANIMATION_DOWN_JUMP,
	RL_ANIMATION_DOWN_DOUBLE_JUMP,
	RL_ANIMATION_DOWN_SIT_DOWN,
	RL_ANIMATION_DOWN_DEATH,
	RL_ANIMATION_DOWN_DASH,
	RL_ANIMATION_DOWN_KNOCK_DOWN,
	RL_ANIMATION_DOWN_KNOCK_BACK,
	RL_ANIMATION_DOWN_KNOCK_OFF,
	RL_ANIMATION_DOWN_HALT,
	RL_ANIMATION_DOWN_FREEZE,
	RL_ANIMATION_DOWN_ENTRAP,
	RL_ANIMATION_DOWN_AUTOFLY,
	RL_ANIMATION_DOWN_PULL,
	RL_ANIMATION_DOWN_REPULSED,
	RL_ANIMATION_DOWN_RISE,
	RL_ANIMATION_DOWN_SKID,

	RL_ANIMATION_DOWN_OTACTION_SKILL_PREPARE,
	RL_ANIMATION_DOWN_OTACTION_SKILL_CHANNEL,
	RL_ANIMATION_DOWN_OTACTION_RECIPE_PREPARE,
	RL_ANIMATION_DOWN_OTACTION_PICK_PREPARE,
	RL_ANIMATION_DOWN_OTACTION_PICKING,
	RL_ANIMATION_DOWN_OTACTION_ITEM_SKILL,
	RL_ANIMATION_DOWN_OTACTION_CUSTOM_PREPARE,
	RL_ANIMATION_DOWN_OTACTION_CUSTOM_CHANNEL,

	RL_ANIMATION_DOWN_COUNT,
};

enum RL_ANIMATION_UP
{
	RL_ANIMATION_UP_UNKNOWN = -1,

	RL_ANIMATION_UP_NONE = 0,
	RL_ANIMATION_UP_SKILL_PREPARE,
	RL_ANIMATION_UP_SKILL_CHANNEL,
	RL_ANIMATION_UP_RECIPE_PREPARE,
	RL_ANIMATION_UP_PICK_PREPARE,
	RL_ANIMATION_UP_OPEN_DOODAD,
	RL_ANIMATION_UP_BEGIN_PICK_DOODAD,
	RL_ANIMATION_UP_PICKING_DOODAD,
	RL_ANIMATION_UP_END_PICK_DOODAD,
	RL_ANIMATION_UP_ITEM_SKILL,
	RL_ANIMATION_UP_CUSTOM_PREPARE,
	RL_ANIMATION_UP_CUSTOM_CHANNEL,
	RL_ANIMATION_UP_BEGIN_SHEATH,
	RL_ANIMATION_UP_END_SHEATH,
	RL_ANIMATION_UP_BEGIN_UNSHEATH,
	RL_ANIMATION_UP_END_UNSHEATH,
	RL_ANIMATION_UP_DODGE,
	RL_ANIMATION_UP_BEHIT,
	RL_ANIMATION_UP_SERIAL_BEGIN,           // Once without reset
	RL_ANIMATION_UP_SERIAL_PROCESSING,      // Loop until end
	RL_ANIMATION_UP_SERIAL_END,             // Once and reset

	RL_ANIMATION_UP_COUNT,
};

struct KRLAnimationTableParam
{
	RL_ANIMATION_DOWN nDownType;
	RL_ADVANCE nAdvance;
	RL_FIGHT_FLAG nFightFlag;
	BOOL bSheathFlag;
	BOOL bSlip;
};

struct KRLPlayerUniformModel
{
	DWORD dwID;
	LPCTSTR szDesc;

	DWORD dwSkillID;
	DWORD dwSkillLevel;
	DWORD dwRepresentIDs[perRepresentCount];

	bool operator<(KRLPlayerUniformModel const& rhs) const
	{
		return dwID < rhs.dwID;
	}
};

struct KRLNpcUniformModel
{
	DWORD dwID;
	LPCTSTR szDesc;

	DWORD dwRepresentID;

	bool operator<(KRLNpcUniformModel const& rhs) const
	{
		return dwID < rhs.dwID;
	}
};

struct KRLDoodadUniformModel
{
	DWORD dwID;
	LPCTSTR szDesc;

	DWORD dwRepresentID;

	bool operator<(KRLDoodadUniformModel const& rhs) const
	{
		return dwID < rhs.dwID;
	}
};

struct KRLOptimizationModel
{
	DWORD dwLevel;
	DWORD dwLowFPS;
	DWORD dwHighFPS;
	DWORD dwEvaluationTime;
	BOOL bDisableTerrainLean;
	BOOL bHideDoodadEffect;
	BOOL bHideCorpseEffect;
	BOOL bHideDoodad;
	BOOL bHideCorpse;
	BOOL bHideEnemyNpc;
	BOOL bHideNeutralityNpc;
	BOOL bHidePartyNpc;
	BOOL bHideAllyNpc;
	BOOL bHideEnemyPlayer;
	BOOL bHideNeutralityPlayer;
	BOOL bHidePartyPlayer;
	BOOL bHideAllyPlayer;
	BOOL bHideSelf;
	BOOL bSelfUniform;
	BOOL bDoodadUniform;
	BOOL bCorpseUniqueUniform;
	BOOL bNpcUniform;
	BOOL bPlayerUniqueUniform;
	BOOL bPlayerGenderUniform;
	BOOL bPlayerRoleTypeUniform;
	BOOL bPlayerForceUniform;
	BOOL bSkillAnimationUniform;
	BOOL bDisableAnimationBlend;

	bool operator<(KRLOptimizationModel const& rhs) const
	{
		return dwLevel < rhs.dwLevel;
	}
};

#endif // RL_GAMEWORLD_TABLE_KTABLEDEFINE_H