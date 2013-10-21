#ifndef SO3_REPRESENT_HANDLER_H
#define SO3_REPRESENT_HANDLER_H


#include "KG3DEngine/KG3DTypes.h"
#include "SO3GameWorldRepresentHandler.h"
#include "SO3GlobalDef.h"

#pragma warning(push)
#pragma warning(disable : 4819)
#include <d3dx9math.h>
#pragma warning(pop)


class IKG3DEngineManager;
class IKG3DResourceManager;
class IKG3DUI;
class IKG3DModel;
class IKG3DScene;
class KSO3World;
class KSceneObject;
class KPluginCentre;

struct ISO3UI;
struct IKSO3GameWorldUIHandler;
struct IKGSO3WorldClient;

enum
{
	MODEL_PLAYER_EQUIP_SFX_BEGIN,

	MODEL_PLAYER_EQUIP_SFX_PHYSICS			= MODEL_PLAYER_EQUIP_SFX_BEGIN,
	MODEL_PLAYER_EQUIP_SFX_STEAL_LIFE,
	MODEL_PLAYER_EQUIP_SFX_STEAL_MANA,
	MODEL_PLAYER_EQUIP_SFX_LUNAR_MAGIC,
	MODEL_PLAYER_EQUIP_SFX_NEUTRAL_MAGIC,
	MODEL_PLAYER_EQUIP_SFX_SOLOR_MAGIC,
	MODEL_PLAYER_EQUIP_SFX_POISON,

	MODEL_PLAYER_EQUIP_SFX_END,
};

enum
{
	MODEL_PLAYER_EQUIP_BEGIN,

	MODEL_PLAYER_EQUIP_CHEST = MODEL_PLAYER_EQUIP_BEGIN,
	MODEL_PLAYER_EQUIP_HEADFORM,
	MODEL_PLAYER_EQUIP_WAIST,
	MODEL_PLAYER_EQUIP_PANTS,
	MODEL_PLAYER_EQUIP_BANGLE,
    MODEL_PLAYER_EQUIP_BANG,
    MODEL_PLAYER_EQUIP_PLAIT,

	MODEL_PLAYER_EQUIP_END,
};

enum
{
	MODEL_PLAYER_SOCKET_BEGIN				= MODEL_PLAYER_EQUIP_END,

    MODEL_PLAYER_SOCKET_WEAPON_BEGIN        = MODEL_PLAYER_SOCKET_BEGIN, 
	MODEL_PLAYER_SOCKET_WEAPON_LH		    = MODEL_PLAYER_SOCKET_WEAPON_BEGIN,
	MODEL_PLAYER_SOCKET_WEAPON_RH,
    MODEL_PLAYER_SOCKET_WEAPON_END,

    MODEL_PLAYER_SOCKET_OTHERS_BEGIN        = MODEL_PLAYER_SOCKET_WEAPON_END,
	MODEL_PLAYER_SOCKET_BACK_EXTEND         = MODEL_PLAYER_SOCKET_OTHERS_BEGIN,
    MODEL_PLAYER_SOCKET_WAIST_EXTEND,
	MODEL_PLAYER_SOCKET_FACE,
    MODEL_PLAYER_SOCKET_OTHERS_END,

    MODEL_PLAYER_SOCKET_HAT                 = MODEL_PLAYER_SOCKET_OTHERS_END,

	MODEL_PLAYER_SOCKET_END,

    MODEL_PLAYER_SOCKET_ADD_ONE_BEGIN       = MODEL_PLAYER_SOCKET_END,
    MODEL_PLAYER_SOCKET_LH_SHEATH           = MODEL_PLAYER_SOCKET_ADD_ONE_BEGIN,
    MODEL_PLAYER_SOCKET_RH_SHEATH,
    MODEL_PLAYER_SOCKET_ADD_ONE_END,
};

enum
{
	MODEL_PLAYER_EQUIP_COUNT				= MODEL_PLAYER_EQUIP_END - MODEL_PLAYER_EQUIP_BEGIN,
	MODEL_PLAYER_SOCKET_COUNT				= MODEL_PLAYER_SOCKET_END - MODEL_PLAYER_SOCKET_BEGIN,
    MODEL_PLAYER_ALL_SOCKET_COUNT		    = MODEL_PLAYER_SOCKET_ADD_ONE_END - MODEL_PLAYER_SOCKET_BEGIN,
    MODEL_PLAYER_SOCKET_ADD_ONE_COUNT       = MODEL_PLAYER_SOCKET_ADD_ONE_END - MODEL_PLAYER_SOCKET_ADD_ONE_BEGIN,
    MODEL_PLAYER_EQUIP_AND_SOCKET_COUNT		= MODEL_PLAYER_EQUIP_COUNT + MODEL_PLAYER_SOCKET_COUNT,
    MODEL_PLAYER_EQUIP_AND_ALL_SOCKET_COUNT	= MODEL_PLAYER_EQUIP_COUNT + MODEL_PLAYER_ALL_SOCKET_COUNT,

    MODEL_PLAYER_ONLY_SOCKET_COUNT          = MODEL_PLAYER_SOCKET_OTHERS_END - MODEL_PLAYER_SOCKET_OTHERS_BEGIN,
};

enum
{
	RL_NPC_SOCKET_FACE,
	RL_NPC_SOCKET_HAT,
	RL_NPC_SOCKET_LH,
	RL_NPC_SOCKET_LP,
    RL_NPC_SOCKET_LC,
	RL_NPC_SOCKET_RH,
	RL_NPC_SOCKET_RP,
    RL_NPC_SOCKET_RC,
    RL_NPC_SOCKET_LONG,
    RL_NPC_SOCKET_SPINE,
	RL_NPC_SOCKET_SPINE2,

	RL_NPC_SOCKET_COUNT,
};

enum
{
    MODEL_RIDES_EQUIP_BEGIN,

    MODEL_RIDES_EQUIP_MAIN = MODEL_RIDES_EQUIP_BEGIN,

    MODEL_RIDES_EQUIP_END,
};


enum
{
    MODEL_RIDES_SOCKET_BEGIN,

    MODEL_RIDES_SOCKET_MAIN = MODEL_RIDES_SOCKET_BEGIN, 
    MODEL_RIDES_SOCKET_1,
    MODEL_RIDES_SOCKET_2,
    MODEL_RIDES_SOCKET_3,
    MODEL_RIDES_SOCKET_4,

    MODEL_RIDES_SOCKET_END,
};

enum
{
    MODEL_RIDES_EQUIP_COUNT				= MODEL_RIDES_EQUIP_END - MODEL_RIDES_EQUIP_BEGIN,
    MODEL_RIDES_SOCKET_COUNT			= MODEL_RIDES_SOCKET_END - MODEL_RIDES_SOCKET_BEGIN,
    MODEL_RIDES_EQUIP_AND_SOCKET_COUNT	= MODEL_RIDES_EQUIP_COUNT + MODEL_RIDES_SOCKET_COUNT,
};

enum
{
    MODEL_DOODAD_EQUIP_BEGIN,

    MODEL_DOODAD_EQUIP_MAIN	= MODEL_DOODAD_EQUIP_BEGIN,

    MODEL_DOODAD_EQUIP_END,
};


enum
{
    MODEL_DOODAD_SOCKET_BEGIN,

    MODEL_DOODAD_SOCKET_FACE = MODEL_DOODAD_SOCKET_BEGIN,
    MODEL_DOODAD_SOCKET_HAT,
    MODEL_DOODAD_SOCKET_LH,
    MODEL_DOODAD_SOCKET_LP,
    MODEL_DOODAD_SOCKET_LC,
    MODEL_DOODAD_SOCEKT_RH,
    MODEL_DOODAD_SOCKET_RP,
    MODEL_DOODAD_SOCKET_RC,
    MODEL_DOODAD_SOCKET_LONG,
    MODEL_DOODAD_SOCKET_SPINE,
    MODEL_DOODAD_SOCKET_SPINE2,

    MODEL_DOODAD_SOCKET_END,
};

enum RL_DOODAD_STATE
{
    RL_DOODAD_STATE_NONE,

    RL_DOODAD_STATE_ENTER_SCENE,
    RL_DOODAD_STATE_LEAVE_SCENE,
	RL_DOODAD_STATE_IDLE,
    RL_DOODAD_STATE_OPERATION,
	RL_DOODAD_STATE_AFTER_OPERATION,

	RL_DOODAD_STATE_COUNT,
};

enum RL_DOODAD_ANIMATION
{
    RL_DOODAD_ANIMATION_IDLE,
    RL_DOODAD_ANIMATION_ENTER_SCENE,
    RL_DOODAD_ANIMATION_LEAVE_SCENE,
    RL_DOODAD_ANIMATION_OPERATION,
    RL_DOODAD_ANIMATION_AFTER_OPERATION,
    RL_DOODAD_ANIMATION_IDLE_TO_OPERATION,
    RL_DOODAD_ANIMATION_IDLE_TO_AFTER_OPERATION,
    RL_DOODAD_ANIMATION_OPERATION_TO_IDLE,
    RL_DOODAD_ANIMATION_OPERATION_TO_AFTER_OPERATION,
    RL_DOODAD_ANIMATION_AFTER_OPERATION_TO_IDLE,
    RL_DOODAD_ANIMATION_AFTER_OPERATION_TO_OPERATION,

    RL_DOODAD_ANIMATION_COUNT,
};

enum
{
    MODEL_DOODAD_EQUIP_COUNT            = MODEL_DOODAD_EQUIP_END - MODEL_DOODAD_EQUIP_BEGIN,
    MODEL_DOODAD_SOCKET_COUNT           = MODEL_DOODAD_SOCKET_END - MODEL_DOODAD_SOCKET_BEGIN,
    MODEL_DOODAD_EQUIP_AND_SOCKET_COUNT = MODEL_DOODAD_EQUIP_COUNT + MODEL_DOODAD_SOCKET_COUNT,
};

enum RL_OJBECT
{
	RL_OBJECT_UNKNOWN,
	RL_OBJECT_PLAYER,
	RL_OBJECT_NPC,
	RL_OBJECT_DOODAD,
    RL_OBJECT_RIDES,

	RL_OBJECT_COUNT,
};

enum RL_ANIMATION_PLAY_TYPE
{
	RL_ANIMATION_PLAY_TYPE_ONCE,
	RL_ANIMATION_PLAY_TYPE_LOOP,

	RL_ANIMATION_PLAY_TYPE_COUNT,
};


//-----------------------------------------------------------------------------------------------------------------------------

struct KRepresentSceneObject
{
	DWORD	dwSceneObjectType;
	DWORD	dwSceneObjectID;
};

//-----------------------------------------------------------------------------------------------
struct KRepresentPlayerAnimationResource
{
	LPCTSTR		szAniFilePath;
	LPCTSTR		szAniSoundFilePath; 
	DWORD		dwAniPlayType;
	float		fAniPlaySpeed;
};

struct KRepresentPlayerEquipPartResource
{
	LPCTSTR		szSocketName;

	LPCTSTR		szMeshFilePath;
	LPCTSTR		szMtlFilePath;

    LPCTSTR		szSheathSocketName;
	LPCTSTR		szSheathMeshFilePath;
	LPCTSTR		szSheathMtlFilePath;

	float		fMeshScale;

	LPCTSTR		szSFX1FilePath;
    LPCTSTR		szSFX2FilePath;

    int         nColorChannel;
};

struct KRepresentPlayerEquipResource
{
	LPCTSTR		szMDLFilePath;
	
	KRepresentPlayerEquipPartResource Parts[MODEL_PLAYER_EQUIP_AND_SOCKET_COUNT];
};

//-----------------------------------------------------------------------------------------------

struct KRepresentRidesAnimationResource
{
    LPCTSTR		szAniFilePath;
    LPCTSTR		szAniSoundFilePath; 
    DWORD		dwAniPlayType;
    float		fAniPlaySpeed;
    float		fAniSoundRange;

    LPCTSTR		szSFXFilePath;
    LPCTSTR		szSFXBoneName;
    DWORD		dwSFXPlayType;
    float		fSFXPlaySpeed;
    float		fSFXScale;
};

struct KRepresentRidesEquipPartResource
{
    LPCTSTR		szMeshFilePath;
    LPCTSTR		szMtlFilePath;
    LPCTSTR		szSocketName;
    float		fMeshScale;
    int         nColorChannel;
};

struct KRepresentRidesEquipResource
{
    LPCTSTR		szMDLFilePath;

    KRepresentRidesEquipPartResource Parts[MODEL_RIDES_EQUIP_AND_SOCKET_COUNT];
};

//-----------------------------------------------------------------------------------------------
struct KRepresentNPCAnimationResource
{
	LPCTSTR		szAniFilePath;
	LPCTSTR		szAniSoundFilePath; 
	DWORD		dwAniPlayType;
	float		fAniPlaySpeed;
	float		fAniSoundRange;

	LPCTSTR		szSFXFilePath;
	LPCTSTR		szSFXBoneName;
	DWORD		dwSFXPlayType;
	float		fSFXPlaySpeed;
	float		fSFXScale;
};

struct KRepresentNPCEquipPartResource
{
	LPCTSTR		szMeshFilePath;
	LPCTSTR		szMtlFilePath;
};

struct KRepresentNPCEquipResource
{
	LPCTSTR		szMDLFilePath;

	int			nSoundDistance;
	int			nColorChannelTable;
	int			nColorChannel;
	float		fModelScale;
	float		fSocketScale;

	KRepresentNPCEquipPartResource Parts[RL_NPC_SOCKET_COUNT];
};

//-----------------------------------------------------------------------------------------------
struct KRepresentDoodadAnimationResource
{
	LPCTSTR		szMeshFilePath;
	LPCTSTR		szMtlFilePath;
	LPCTSTR		szSFXFilePath;
	LPCTSTR		szAniFilePath;
    DWORD		dwAniPlayType;
};

struct KRepresentDoodadResource
{
	int			nFogVisible;
	int			nColorChannelTable;
	int			nColorChannel;
	int			nVisiblePriority;
	float		fModelScale;
	DWORD		dwPlayerOpenAnimationID;
	DWORD		dwPlayerBeginPickAnimationID;
	DWORD		dwPlayerEndPickAnimationID;

	KRepresentDoodadAnimationResource	aAnimation[RL_DOODAD_ANIMATION_COUNT];
};

//------------------------------------------------------------------------------------------
struct KRepresentCameraParams
{
    float fDragSpeed;
    float fMaxCameraDistance;
    float fSpringResetSpeed;
    float fCameraResetSpeed;

	int nResetMode;
};

// RT : Run time
struct KRepresentCameraRTParams
{
    float fCameraToObjectEyeScale;
    float fYaw;
    float fPitch;
};

struct KRepresentSceneObjectData
{
};

//------------------------------------------------------------------------------------------
class IKSO3RepresentHandler : public IKSO3GameWorldRepresentHandler
{
public:
	virtual~IKSO3RepresentHandler();

	virtual HRESULT Activate(DWORD dwTickCount, DWORD dwGameLoop, BOOL bFrame) = 0;
	virtual HRESULT ResetActiveObject() = 0;

	virtual HRESULT Attach3DEngine(IKG3DEngineManager* p3DEngineManager, IKG3DResourceManager* p3DModelManager, IKG3DUI* p3DUI) = 0;
	virtual HRESULT AttachGameWorld(KSO3World* pSO3World, IKGSO3WorldClient* m_pSO3WorldClient) = 0;
	virtual HRESULT AttachUI(ISO3UI *pSO3UI, IKSO3GameWorldUIHandler *pGameWorldUIHandler) = 0;

	virtual HRESULT InitHWND(HWND hWnd) = 0;
	virtual HRESULT NewScene(DWORD dwSceneID, DWORD dwOption, int nX, int nY, int nZ, LPCTSTR szFilePath) = 0;
	virtual HRESULT LoadScene(DWORD dwSceneID, DWORD dwOption, int nX, int nY, int nZ, LPCTSTR szFilePath) = 0;
	virtual HRESULT DeleteScene(DWORD dwSceneID) = 0;

	virtual HRESULT AddSceneOutputWindow(DWORD dwSceneID, HWND hWnd) = 0;
	virtual HRESULT RemoveSceneOutputWindow(DWORD dwSceneID) = 0;

	virtual size_t SelectPtInScene(DWORD dwSceneID, D3DXVECTOR2 v2Scale, KRepresentSceneObject* pObject, size_t nObjectCount) = 0;
	virtual HRESULT SelectPtInGround(DWORD dwSceneID, D3DXVECTOR2 v2Scale, int& nX, int& nY, int& nZ) = 0;
	virtual HRESULT GetCharacterTop(DWORD dwCharacterID, D3DXVECTOR3& vPosition) = 0;

	virtual HRESULT ScenePointToScreenPoint(DWORD dwSceneID, D3DXVECTOR3 const& vScenePos, float& fScreenX, float& fScreenY) = 0;
	virtual HRESULT GameWorldPositionToScenePosition(D3DXVECTOR3& vPosition, DWORD dwScene, float fX, float fY, float fZ, BOOL bOnFloor) = 0;
	virtual HRESULT ScenePositionToGameWorldPosition(DWORD dwScene, float& fX, float& fY, float& fZ, D3DXVECTOR3 const& vPosition) = 0;

	virtual HRESULT GetCurrentSceneID(DWORD& dwScene) const = 0; // TODO: remove

	virtual HRESULT	GetScene(DWORD dwScene, IKG3DScene** pp3DScene) const = 0;

	virtual HRESULT ZoomCamera(DWORD dwScene, float fScale) = 0;

	virtual HRESULT BeginDragCamera(DWORD dwScene, int& nX, int& nY) = 0;
	virtual HRESULT EndDragCamera(DWORD dwScene, int nX, int nY) = 0;
	virtual HRESULT HasCameraDragged(
        DWORD dwScene,
        BOOL& bCamera,
        BOOL& bObject,
        float& fMaxMouseX,
        float& fMaxMouseY,
        float& fFinalMouseX,
        float& fFinalMouseY
    ) = 0;

	virtual HRESULT SetCameraParams(DWORD dwScene, KRepresentCameraParams Param) = 0;
	virtual HRESULT GetCameraParams(DWORD dwScene, KRepresentCameraParams& Param) = 0;

    virtual HRESULT SetCameraRTParams(DWORD dwScene, KRepresentCameraRTParams Param) = 0;
    virtual HRESULT GetCameraRTParams(DWORD dwScene, KRepresentCameraRTParams& Param) = 0;

	virtual HRESULT EnableControlCamera(DWORD dwScene, size_t nIndex, BOOL bEnable) = 0;
	virtual HRESULT ToggleControlCamera(DWORD dwScene, size_t nIndex) = 0;

	virtual HRESULT SetView(DWORD dwScene, float fAngle) = 0;

    virtual HRESULT TurnToFaceDirection(DWORD dwScene) = 0;

	virtual HRESULT GetCameraYawPitchRoll(DWORD dwScene, float& fYaw, float& fPitch, float& fRoll) = 0;

    virtual HRESULT SwitchCameraMode(DWORD dwScene, int nMode) = 0;

    virtual HRESULT IsCharacterMoving(DWORD dwScene, int nMoveStyle, int *pnIsMoving) = 0;

    virtual HRESULT MouseControlMoveEnable(DWORD dwScene, BOOL bEnable) = 0;

    virtual HRESULT LockControl(DWORD dwScene, DWORD dwFrame) = 0;

    virtual HRESULT StartFollow(DWORD dwScene, DWORD dwTargetType, DWORD dwTargetID) = 0;
    virtual HRESULT StopFollow(DWORD dwScene) = 0;
    virtual HRESULT IsFollowing(DWORD dwScene, BOOL *pbIsFollowing, DWORD *pdwTargetType, DWORD *pdwTargetID) = 0;
	
    virtual HRESULT AutoMoveToPoint(DWORD dwScene, D3DXVECTOR3 vPoint) = 0;
    virtual HRESULT AutoMoveToTarget(DWORD dwScene, DWORD dwTargetType, DWORD dwTargetID) = 0;
    virtual HRESULT IsAutoMoving(DWORD dwScene, BOOL *pbIsAutoMoving) = 0;

    virtual HRESULT ShowCursorEffect(DWORD dwScene, DWORD dwCursorEffectID) = 0;

	virtual HRESULT SetSceneObjectAlpha(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, float fScale) = 0;
	virtual HRESULT SetSceneObjectBrightness(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, float fScale) = 0;

	virtual HRESULT SetSceneObjectSelectable(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, BOOL bSelectable) = 0;

    virtual HRESULT SetSceneObjectTitleEffect(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, DWORD dwEffectID) = 0;

    virtual int SceneObjectTypeToGameWorldObjectType(DWORD dwSceneObjectType) = 0;
	virtual DWORD GameWorldObjectTypeToSceneObjectType(int nGameWorldObjectType) = 0;

    virtual HRESULT GetSceneObjectData(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, KRepresentSceneObjectData* pData) = 0;

    virtual HRESULT GetPlayerAnimationResource(ROLE_TYPE nRoleType, DWORD dwAnimationID, KRepresentPlayerAnimationResource* pResource) const = 0;
	virtual HRESULT GetPlayerEquipResource(ROLE_TYPE nRoleType, DWORD dwRepresentIDs[perRepresentCount], KRepresentPlayerEquipResource* pResource) const = 0;

    virtual HRESULT GetRidesAnimationResource(DWORD dwRidesID, DWORD dwAnimationID, KRepresentRidesAnimationResource* pResource) const = 0;
    virtual HRESULT GetRidesEquipResource(DWORD dwRidesID, DWORD dwRepresentIDs[MODEL_RIDES_EQUIP_AND_SOCKET_COUNT], KRepresentRidesEquipResource* pResource) const = 0;

	virtual HRESULT GetNPCAnimationResource(DWORD dwRepresentID, DWORD dwAnimationID, KRepresentNPCAnimationResource* pResource) const = 0;
	virtual HRESULT GetNPCEquipResource(DWORD dwRepresentID, KRepresentNPCEquipResource* pResource) const = 0;

	virtual HRESULT GetDoodadResource(DWORD dwRepresentID, KRepresentDoodadResource* pResource) const = 0;

	virtual HRESULT ShowSelectionSFX(DWORD dwCursor, DWORD dwSkillID, DWORD dwSkillLevel) = 0;
	virtual HRESULT HideSelectionSFX(DWORD dwCursor) = 0;

	virtual HRESULT AttachTargetSceneObject(DWORD dwTarget, DWORD dwObject, DWORD dwObjectType) = 0;
	virtual HRESULT DetachTargetSceneObject(DWORD dwTarget, DWORD dwObject, DWORD dwObjectType) = 0;
	virtual HRESULT ShowTargetSelectionSFX(DWORD dwTarget, int nForceRelationType) = 0;
	virtual HRESULT HideTargetSelectionSFX(DWORD dwTarget) = 0;

	virtual void EnableHeadSFX(int nCharacterType, int nIndex, BOOL bEnable) = 0;
	virtual void UpdateHeadTopPosition() = 0;

	virtual int GetMinimapLayer(DWORD dwScene, D3DVECTOR const& vPos) = 0;
	virtual DWORD GetRegionInfo(DWORD dwScene, D3DVECTOR const& vPos) = 0;
	virtual DWORD GetRegionInfoByMapID(DWORD dwMapID, D3DVECTOR const& vPos) = 0;

	virtual HRESULT ExecuteCommand(LPCTSTR szCommand) = 0;

    virtual HRESULT PlayAnimation(DWORD dwScene, DWORD dwActor, DWORD dwTarget, BOOL bTurnToTarget, DWORD dwAnimationID) = 0;
    virtual HRESULT StopAnimation(DWORD dwScene, DWORD dwActor) = 0;

    virtual HRESULT PlaySpeech(DWORD dwScene, DWORD dwActor, DWORD dwTarget, BOOL bTurnToTarget, LPCTSTR pszFilePath) = 0;
    virtual HRESULT StopSpeech(DWORD dwScene, DWORD dwActor) = 0;
};


inline IKSO3RepresentHandler::~IKSO3RepresentHandler()
{
}


#endif // SO3_REPRESENT_HANDLER_H

