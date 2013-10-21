////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSFXFileDefine.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-15 12:03:49
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSFXFILEDEFINE_H_
#define _INCLUDE_KG3DSFXFILEDEFINE_H_

////////////////////////////////////////////////////////////////////////////////
enum SFX_FILE_ELEMENT_ID
{
    SFX_FEID_GENERAL_INFORMATION,
    SFX_FEID_PARTICLE_KIND,
    SFX_FEID_LAUNCHER,
    SFX_FEID_FORCE_FIELD,
    SFX_FEID_TEXTURE,
    SFX_FEID_BILLBOARD,
    SFX_FEID_TRACK,
    SFX_FEID_MODEL,
    SFX_FEID_METEOR,
    SFX_FEID_MAGIC_PARPET,
    SFX_FEID_CHANNEL,
    SFX_FEID_SOUND,
	SFX_FEID_SELECTMARK,
    SFX_FEID_EXP,
    SFX_FEID_BOX_PROJ,

    SFX_FEID_PARICLE_KIND_EXT,
    SFX_FEID_SIZE
};

const DWORD SFX_ID = MAKEFOURCC('S', 'F', 'X', '\0');
const DWORD SFX_VERSION_31 = MAKELONG(3, 1);//MAKELONG(2, 1);
const DWORD SFX_VERSION_51 = MAKELONG(5, 1);
const DWORD SFX_VERSION    = MAKELONG(6, 1);

// DO NOT CHANGE THESE LENGTH
const DWORD SFX_TEXTURE_FILE_NAME_LENGTH = 128;
const DWORD SFX_NAME_LENGTH = 32;
const DWORD SFX_TRACK_FILE_NAME_LENGTH = 128;
const DWORD SFX_MODEL_FILE_NAME_LENGTH = 128;




struct FLOAT5 
{
public:
    FLOAT5() { x = y = z = u = v = 0.f; };

    FLOAT5( FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->u = u;
        this->v = v;
    }

    FLOAT5& operator += ( CONST FLOAT5& f5) {
        this->x += f5.x;
        this->y += f5.y;
        this->z += f5.z;
        this->u += f5.u;
        this->v += f5.v;
        return *this;
    }

    FLOAT5& operator -= ( CONST FLOAT5& f5) {
        this->x -= f5.x;
        this->y -= f5.y;
        this->z -= f5.z;
        this->u -= f5.u;
        this->v -= f5.v;
        return *this;
    }

    FLOAT5& operator *= ( FLOAT f) {
        this->x *= f;
        this->y *= f;
        this->z *= f;
        this->u *= f;
        this->v *= f;
        return *this;
    }

    FLOAT5& operator /= ( FLOAT f) {
        this->x /= f;
        this->y /= f;
        this->z /= f;
        this->u /= f;
        this->v /= f;
        return *this;
    }

    FLOAT5 operator + ( CONST FLOAT5& f5) const {
        return FLOAT5(
            this->x + f5.x,
            this->y + f5.y,
            this->z + f5.z,
            this->u + f5.u,
            this->v + f5.v
            );
    }

    FLOAT5 operator - ( CONST FLOAT5& f5) const {
        return FLOAT5(
            this->x - f5.x,
            this->y - f5.y,
            this->z - f5.z,
            this->u - f5.u,
            this->v - f5.v
            );
    }

    FLOAT5 operator * ( FLOAT f) const {
        return FLOAT5(
            this->x * f,
            this->y * f,
            this->z * f,
            this->u * f,
            this->v * f
            );
    }

    FLOAT5 operator / ( FLOAT f) const {
        return FLOAT5(
            this->x / f,
            this->y / f,
            this->z / f,
            this->u / f,
            this->v / f
            );
    }

    BOOL operator == ( CONST FLOAT5& f5) const {
        return (
            this->x == f5.x && 
            this->y == f5.y && 
            this->z == f5.z &&
            this->u == f5.u &&
            this->v == f5.v
            );
    }

    BOOL operator != ( CONST FLOAT5& f5) const {
        return (
            this->x != f5.x ||
            this->y != f5.y || 
            this->z != f5.z ||
            this->u != f5.u ||
            this->v != f5.v
            );
    }

    FLOAT x, y, z, u, v;

};

#pragma pack(push,1)

struct SFX_ELEMENT
{
    DWORD   dwElementID;
    DWORD   dwElementOffset;
};

struct SFX_KEY_FRAME
{
    DWORD   dwFrame;
    DWORD   dwValue;
};

struct SFX_KEY_FRAME_TRANSFORMATION
{
    DWORD dwFrame;

    D3DXVECTOR3         vScalingCenter;
    D3DXQUATERNION      qScalingRotation;
    D3DXVECTOR3         vScaling;
    D3DXVECTOR3         vRotationCenter;
    D3DXQUATERNION      qRotation;
    D3DXVECTOR3         vTranslation;
};

struct SFX_KEY_FRAME_VECTOR5
{
    DWORD dwFrame;
    FLOAT5 v;
};


struct SFX_KEY_FRAME_VECTOR4
{
    DWORD dwFrame;
    D3DXQUATERNION q;
};

struct SFX_KEY_FRAME_VECTOR3
{
    DWORD dwFrame;
    D3DXVECTOR3 Vector3;
};

struct SFX_KEY_FRAME_VECTOR2
{
    DWORD dwFrame;
    D3DXVECTOR2 Vector2;
};

struct SFX_FILE_HEADER
{
    DWORD   dwID;               // MAKEFOURCC = ('S', 'F', 'X', '\0');
    DWORD   dwVersion;          // HIWORD(Version) = Major version,
                                // LOWORD(Version) = Minor version
    DWORD   dwFileLength;
    DWORD   dwElementNum;
};

enum {

    SFX_SOUND_MODE_APPEND = 0,
    SFX_SOUND_MODE_INT,
    SFX_SOUND_MODE_WAIT,
    SFX_SOUND_MODE_INT_ALL,

    SFX_SOUND_MODE_NUM

};

struct SFX_SOUND_BLOCK 
{
    DWORD  dwSound;         
    DWORD  dwMode;    
    FLOAT  fVolume;
    FLOAT  fUnUse;
    FLOAT  fMinDis;
    FLOAT  fMaxDis;
    TCHAR  szFileName[128];
    DWORD  bLoop;
    DWORD  bContinue;
    int    nProbability;
    DWORD  dwExpand[2];

    SFX_SOUND_BLOCK operator- (const SFX_SOUND_BLOCK& r) const
    {
        SFX_SOUND_BLOCK t;

        t.fVolume = fVolume - r.fVolume;
        t.fMaxDis = fMaxDis - r.fMaxDis;
        t.fMinDis = fMinDis - r.fMinDis;

        return t;
    };

    SFX_SOUND_BLOCK operator/ (float f) const 
    {
        SFX_SOUND_BLOCK t;

        t.fVolume = fVolume / f;
        t.fMaxDis = fMaxDis / f;
        t.fMinDis = fMinDis / f;

        return t;
    };

    SFX_SOUND_BLOCK operator* (float f) const 
    {
        SFX_SOUND_BLOCK t;

        t.fVolume = fVolume * f;
        t.fMaxDis = fMaxDis * f;
        t.fMinDis = fMinDis * f;

        return t;
    };

    SFX_SOUND_BLOCK operator+ (const SFX_SOUND_BLOCK& r) const
    {
        SFX_SOUND_BLOCK t;

        t.fVolume = fVolume + r.fVolume;
        t.fMaxDis = fMaxDis + r.fMaxDis;
        t.fMinDis = fMinDis + r.fMinDis;

        return t;
    };

};

struct SFX_SOUND_BLOCK_EX
{
    DWORD dwSoundNum;
    /*
     dwSoundNum of SFX_SOUND_BLOCK data
     */

};

struct _SoundInfo
{
    std::vector<SFX_SOUND_BLOCK> vSubSound;
    int nProbabilitySum; 

    _SoundInfo()
        : nProbabilitySum(1)
    {
    }

    _SoundInfo(const _SoundInfo& oth)
        : vSubSound(oth.vSubSound),
          nProbabilitySum(oth.nProbabilitySum)
    {
    }

    _SoundInfo operator- (const _SoundInfo& r) const
    {
        return *this;
    };

    _SoundInfo operator/ (float f) const 
    {
        return *this;
    };

    _SoundInfo operator* (float f) const 
    {
        return *this;
    };

    _SoundInfo operator+ (const _SoundInfo& r) const
    {
        return *this;
    };
};


struct SFX_EXP_BLOCK 
{
    DWORD dwTimeScalFrameNum;
    DWORD dwSwingFrameNum;
    DWORD dwFrequencyFrameNum;
    DWORD dwModelScanlFrameNum;

    DWORD dwExpend[12];
};


struct SFX_GENERAL_INFORMATION_BLOCK
{
    DWORD   dwTotalFrameNum;
    DWORD   dwFrameInterval;
    FLOAT   fShockWavePower;
    DWORD   dwSoveWaveFlag;
	DWORD	dwCloseFloorFlag;
    DWORD   dwScanlByParentFlag;
    DWORD   dwLockYAlex;
    DWORD   dwShockMode;
    DWORD   dwMaxParticle;
};

// update by zhaoxinyu
struct SFX_PARTICLE_KIND_BLOCK
{
    DWORD   dwColorFrameNumL;
    DWORD   dwWidthFrameNum;
    DWORD   dwHeightFrameNum;
    DWORD   dwTextureFrameNum;
    DWORD   dwTextureIndex;
    DWORD   dwRotationFactor;

    DWORD   dwIsRandomTextureFrame;

	DWORD   dwSelfRotationNumMin;
    DWORD   dwColorFrameNumR;
	DWORD   dwSelfRotationNumMax;
    WORD    wType;                      /* 0 : normal praticle  1 : meteor praticle */
    WORD    wFlag;

    DWORD   dwSmooth; 
};

struct SFX_PARTICLE_LAUNCHER_BLOCK
{
    DWORD   dwParticleNumFrameNum;
    DWORD   dwLifeMaxFrameNum;
    DWORD   dwLifeMinFrameNum;
    DWORD   dwSpeedMaxFrameNum;
    DWORD   dwSpeedMinFrameNum;
    DWORD   dwScatterMinFrameNum;
    DWORD   dwScatterMaxFrameNum;

    DWORD   dwTranslationFrameNum;
    DWORD   dwRotationFrameNum;
    DWORD   dwScalingFrameNum;

   /* D3DXVECTOR3 vRotationCenter;
    D3DXVECTOR3 vScalingCenter;*/
    DWORD   dwColorDelay;
    DWORD   dwRoatnDelay;
    DWORD   dwAlphaDelay;
    DWORD   dwFcRotDelay;
    DWORD   dwExpend[2];

    DWORD   dwIsSpeedInherit;
    DWORD   dwParticleType;
    DWORD   dwLauncherShape;
    DWORD   dwParticleUpdateMode;

    DWORD   dwBlendMode;

    float   fLauncherShapeParam1;
    float   fLauncherShapeParam2;

    DWORD   dwParticleKindIndex;

    BYTE    byName[SFX_NAME_LENGTH];
    
    DWORD   dwBindTrackIndex;
    DWORD   dwBindTrackMode;

	DWORD	dwRandTexDelay;
    DWORD   dwEffForceBit;
    DWORD   dwMotionType;
    DWORD   dwForceOption;

    DWORD   dwElementNum;
};

struct SFX_FORCE_FIELD_BLOCK
{
    DWORD   dwForceType;
    DWORD   dwForceFrameNum;
    DWORD   dwRandom;
    DWORD   dwRoataionFrameNum;
    DWORD   dwGFrameNum;
    DWORD   dwReserved[2];

    DWORD   dwElementNum;
};

struct SFX_TEXTURE_BLOCK
{
    BYTE    byTextureName[SFX_TEXTURE_FILE_NAME_LENGTH];
    DWORD   dwCutNum;
    DWORD   dwReserved[5];

    DWORD   dwElementNum;
};

struct SFX_BILLBOARD_BLOCK
{
    DWORD   dwPositionFrameNum;
    DWORD   dwWidthFrameNum;
    DWORD   dwHeightFrameNum;
    DWORD   dwDiffuseFrameNum;
    DWORD   dwTextureFrameNum;
    DWORD   dwTexture2FrameNum;
    DWORD   dwUVFrameNum;
    DWORD   dwRotationFrameNum;

    BYTE    byTextureName[SFX_TEXTURE_FILE_NAME_LENGTH];
    DWORD   dwCutNum;

    BYTE    byTexture2Name[SFX_TEXTURE_FILE_NAME_LENGTH];
    DWORD   dwCutNum2;

    DWORD   dwBillBoardType;//dwIsLockAxis;
    DWORD   dwIsRotation;

    BYTE    byBoneName[SFX_NAME_LENGTH];
    
    DWORD   dwBindTrackIndex;

    D3DXVECTOR3 vRotation;

    DWORD   dwBlendMode;
    //DWORD   dwReserved[1];
    DWORD   dwRenterTarget;
    DWORD   dwElementNum;
};

struct SFX_CHANNEL_BLOCK
{
    BYTE            byTextureFileName[SFX_TEXTURE_FILE_NAME_LENGTH];
    int             nPieceCount;      // piece count
    int             nVCount;          // the V piece count
    float           fWidth;           // piece width
    float           fLength;          // piece length 
    float           fUSpeed;          // the uv moving speed in direction U
    float           fVUpdateFcy;      // the uv update frequency in direction V   
    int             nBlendMode;
    DWORD           dwReserved[1];  
};

struct SFX_BOX_PROJ
{
    BYTE            byTextureFileName[SFX_TEXTURE_FILE_NAME_LENGTH];
    D3DXVECTOR3     vScalingCenter;
    D3DXQUATERNION  qScalingRotation;
    D3DXVECTOR3     vScaling;
    D3DXVECTOR3     vRotationCenter;
    D3DXQUATERNION  qRotation;
    D3DXVECTOR3     vTranslation;
    int             nClipU;
    int             nClipV;
    float           fFrameSpeed;
    DWORD           dwProjMode;
    DWORD           dwBlend;
    DWORD           dwColorFrameNum;
    DWORD           dwUVFrameNum;

    DWORD           dwReserved[25];
};

struct SFX_TRACK_BLOCK
{
    BYTE    byTrackFileName[SFX_TRACK_FILE_NAME_LENGTH];

    D3DXVECTOR3         vScalingCenter;
    D3DXQUATERNION      qScalingRotation;
    D3DXVECTOR3         vScaling;
    D3DXVECTOR3         vRotationCenter;
    D3DXQUATERNION      qRotation;
    D3DXVECTOR3         vTranslation;

    DWORD   dwInterval;
    DWORD   dwReserved[5];

    DWORD   dwElementNum;
};

struct SFX_MODEL_BLOCK
{
    BYTE    byMeshFileName[SFX_MODEL_FILE_NAME_LENGTH];
    BYTE    byMaterialFileName[SFX_MODEL_FILE_NAME_LENGTH];
    BYTE    byAnimationFileName[SFX_MODEL_FILE_NAME_LENGTH];

    DWORD   dwPlayMode;
    DWORD   dwPlaySpeed;
    DWORD   dwPlayStart;

    DWORD   dwOption;

    D3DXVECTOR3         vScalingCenter;
    D3DXQUATERNION      qScalingRotation;
    D3DXVECTOR3         vScaling;
    D3DXVECTOR3         vRotationCenter;
    D3DXQUATERNION      qRotation;
    D3DXVECTOR3         vTranslation;

    DWORD   dwSubsetNum;
	DWORD	dwPlayConnect;
	DWORD	dwStartFrame;
	DWORD	dwFinisFrame;

    int     dwBindTrackIndex;
    int		dwIsBillBoard;

    DWORD   dwElementNum;
};

struct SFX_MODEL_SUBSET
{
    DWORD   dwDiffuseColorFrame;
    DWORD   dwReserved[5];

    DWORD   dwElementNum;
};

struct SFX_METEOR_BLOCK
{
    //D3DXVECTOR3         vScalingCenter;
    float   fTranslationFrameNum;
    float   fRotationFrameNum;
    float   fScalingFrameNum;

    D3DXQUATERNION      qScalingRotation;
    D3DXVECTOR3         vScaling;
    D3DXVECTOR3         vRotationCenter;
    D3DXQUATERNION      qRotation;
    D3DXVECTOR3         vTranslation;

    float fEmitInterval;
    float fLife;
    float fWidth;

    DWORD   dwTextureIdx;
    int     nTrackIdx;

    D3DXVECTOR3 vOffset;
    BOOL        bEnableRandomOffset;
    DWORD       BlendMode;
    float       fRallyingScale;
    float       fInterpolateLength;
    float       fDuration;
	DWORD       dwSampling;
    DWORD		dwReserved[1];
};

struct SFX_MAGIC_PARPET_BLOCK
{
    DWORD   dwFaceCameraFlag;
    DWORD   dwRadius;
    DWORD   dwOffset;
    DWORD   dwHeightLimit;

    BYTE    byTextureFileName[SFX_TEXTURE_FILE_NAME_LENGTH];
	DWORD   dwIsFootprint;
	DWORD   dwLife;
    DWORD   dwType;
    DWORD   dwFaceType;
    DWORD   dwExpFlag;
};

struct SFX_MAGIC_PARPET_EXP_BLOCK
{
    DWORD dwRadiusNum;
    DWORD dwColorNum;
    int   nTexCullX;
    int   nTexCullY;
    float fTexSpeed;
    DWORD dwReserved[5];
};

struct SFX_SELECTMARK_BLOCK
{
	DWORD	dwWidth;
	DWORD	dwHeight;
	BYTE    byTextureFileName[SFX_TEXTURE_FILE_NAME_LENGTH];
	DWORD   dwReserved[8];
};
#pragma pack(pop)


#endif //_INCLUDE_KG3DSFXFILEDEFINE_H_
