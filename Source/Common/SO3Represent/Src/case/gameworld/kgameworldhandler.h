#ifndef CASE_GAME_WORLD_K_GAME_WORLD_HANDLER_H
#define CASE_GAME_WORLD_K_GAME_WORLD_HANDLER_H

#include "SO3Represent/SO3RepresentInterface.h"
#include "../../common/tblock.h"

class KRLCursor;
class KRLTarget;
class KRLCamera;
class KActionObjectTable;
class KActionHandlerTable;
class KActionEventTable;
class KModelMgr;
class KSO3World;
struct ISO3UI;
struct IKG3DSoundShell;
struct IKGSO3WorldClient;

class KGameWorldHandler : public IKSO3RepresentHandler
{
public:
	virtual~KGameWorldHandler();

    virtual HRESULT Activate();
	virtual HRESULT ResetActiveObject();

	virtual HRESULT Attach3DEngine(IKG3DEngineManager* p3DEngineManager, IKG3DResourceManager* p3DModelManager, IKG3DUI* p3DUI);
	virtual HRESULT AttachGameWorld(KSO3World* pSO3World, IKGSO3WorldClient* pKGSO3WorldClient);
	virtual HRESULT AttachUI(ISO3UI *pSO3UI, IKSO3GameWorldUIHandler *pGameWorldUIHandler);

	virtual HRESULT InitHWND(HWND hWnd);

	virtual HRESULT NewScene(DWORD dwSceneID, DWORD dwOption, int nX, int nY, int nZ, LPCTSTR szFilePath);
	virtual HRESULT LoadScene(DWORD dwSceneID, DWORD dwOption, int nX, int nY, int nZ, LPCTSTR szFilePath);
	virtual HRESULT DeleteScene(DWORD dwSceneID);

	virtual HRESULT AddSceneOutputWindow(DWORD dwSceneID, HWND hWnd);
	virtual HRESULT RemoveSceneOutputWindow(DWORD dwSceneID);

	virtual size_t SelectPtInScene(DWORD dwSceneID, D3DXVECTOR2 v2Scale, KRepresentSceneObject* pObject, size_t nObjectCount);
	virtual HRESULT SelectPtInGround(DWORD dwSceneID, D3DXVECTOR2 v2Scale, int& nX, int& nY, int& nZ);
    virtual HRESULT GetCharacterTop(DWORD dwCharacterID, D3DXVECTOR3& vPosition);

    virtual HRESULT ScenePointToScreenPoint(DWORD dwSceneID, D3DXVECTOR3 const& vScenePos, float& fScreenX, float& fScreenY);
	virtual HRESULT GameWorldPositionToScenePosition(D3DXVECTOR3& vPosition, DWORD dwScene, float fX, float fY, float fZ, BOOL bOnFloor);
	virtual HRESULT ScenePositionToGameWorldPosition(DWORD dwScene, float& fX, float& fY, float& fZ, D3DXVECTOR3 const& vPosition);

	virtual HRESULT GetCurrentSceneID(DWORD& dwScene) const;

	virtual HRESULT	GetScene(DWORD dwSceneID, IKG3DScene** pp3DScene) const;

	virtual HRESULT ZoomCamera(DWORD dwScene, float fScale);

	virtual HRESULT BeginDragCamera(DWORD dwScene, int& nX, int& nY);
	virtual HRESULT EndDragCamera(DWORD dwScene, int nX, int nY);
	virtual HRESULT HasCameraDragged(
        DWORD dwScene,
        BOOL& bCamera,
        BOOL& bObject,
        float& fMaxMouseX,
        float& fMaxMouseY,
        float& fFinalMouseX,
        float& fFinalMouseY
    );

    virtual HRESULT SetCameraParams(DWORD dwScene, KRepresentCameraParams Param);
    virtual HRESULT GetCameraParams(DWORD dwScene, KRepresentCameraParams& Param);

    virtual HRESULT SetCameraRTParams(DWORD dwScene, KRepresentCameraRTParams Param);
    virtual HRESULT GetCameraRTParams(DWORD dwScene, KRepresentCameraRTParams& Param);

	virtual HRESULT EnableControlCamera(DWORD dwScene, size_t nIndex, BOOL bEnable);
	virtual HRESULT ToggleControlCamera(DWORD dwScene, size_t nIndex);

	virtual HRESULT SetView(DWORD dwScene, float fAngle);

    virtual HRESULT TurnToFaceDirection(DWORD dwScene);

    virtual HRESULT GetCameraYawPitchRoll(DWORD dwScene, float& fPitch, float& fYaw, float& fRoll);

    virtual HRESULT SwitchCameraMode(DWORD dwScene, int nMode);

    virtual HRESULT IsCharacterMoving(DWORD dwScene, int nMoveStyle, int *pnIsMoving);

    virtual HRESULT MouseControlMoveEnable(DWORD dwScene, BOOL bEnable);

    virtual HRESULT LockControl(DWORD dwScene, DWORD dwFrame);

    virtual HRESULT StartFollow(DWORD dwScene, DWORD dwTargetType, DWORD dwTargetID);
    virtual HRESULT StopFollow(DWORD dwScene);
    virtual HRESULT IsFollowing(DWORD dwScene, BOOL *pbIsFollowing, DWORD *pdwTargetType, DWORD *pdwTargetID);

    virtual HRESULT AutoMoveToPoint(DWORD dwScene, D3DXVECTOR3 vPoint);
    virtual HRESULT AutoMoveToTarget(DWORD dwScene, DWORD dwTargetType, DWORD dwTargetID);
    virtual HRESULT IsAutoMoving(DWORD dwScene, BOOL *pbIsAutoMoving);

    virtual HRESULT ShowCursorEffect(DWORD dwScene, DWORD dwCursorEffectID);

	virtual BOOL OnNewScene(KScene* pScene, int nX, int nY, int nZ, BOOL bEditor);
	virtual void OnDeleteScene(DWORD dwSceneID);

    virtual void OnResetFrameData(int nGameLoop, DWORD dwStartTime);

    //Player和Npc共用事件接口
    virtual void OnAddCharacter(KCharacter* pCharacter);
    virtual void OnRemoveCharacter(KCharacter* pCharacter, BOOL bKilled);

    virtual void OnUpdateCharacterFrameData(KCharacter* pCharacter);
    virtual void OnCharacterDisplayData(KCharacter* pCharacter);
    virtual void OnCharacterMountHorse(KCharacter* pCharacter, BOOL bMount);
    virtual void OnCharacterTurnTo(KCharacter* pCharacter, BOOL bTurnImmediately);

    virtual void OnCharacterPrepareCastSkill(KCharacter* pCharacter, DWORD dwSkillID, DWORD dwSkillLevel);
    virtual void OnCharacterCastSkill(KCharacter* pCharacter, KTargetData* pTarget, KSkill* pSkill, DWORD dwBulletID, BOOL bPlayAni);
    virtual void OnCharacterSkillBuffData(KCharacter* pCharacter);
    virtual void OnCharacterSkillEffectResult(KCharacter* pTarget, KRepresentSkillEffectResult* pResult);
    virtual void OnCharacterSkillChainDisplay(int nCount, KChainDisplayInfo* pDisplayList);
    
    virtual void OnCharacterPlayAnimation(KCharacter* pCharacter, DWORD dwAnimationID);
    virtual void OnCharacterPlayAnimationEnd(KCharacter* pCharacter, DWORD dwAnimationID);

    virtual void OnCharacterLevelUp(KCharacter* pCharacter, REPRESENT_SFX nSFX);
    virtual void OnCharacterSound(KCharacter* pCharacter, DWORD dwSoundID);

    virtual void OnCharacterOpenDoodad(KCharacter* pCharacter, KDoodad* pDoodad);
    virtual void OnCharacterBeginPickDoodad(KCharacter* pCharacter, KDoodad* pDoodad);
    virtual void OnCharacterEndPickDoodad(KCharacter* pCharacter, KDoodad* pDoodad);
    virtual void OnCharacterBeginCraftAnimation(KCharacter* pCharacter, int nCraftID);
    virtual void OnCharacterEndCraftAnimation(KCharacter* pCharacter, int nCraftID);

    virtual void OnCharacterUpdateTitleMark(KCharacter* pCharacter);
    virtual void OnCharacterFollowTarget(KCharacter* pCharacter, KCharacter* pTarget);
    
    virtual void OnCharacterUpdateRelation(KCharacter* pCharacter);
    virtual void OnCharacterUpdateAllRelation();

    virtual HRESULT SetSceneObjectTitleEffect(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, DWORD dwEffectID);

    //Doodad相关事件接口
	virtual void OnAddDoodad(KDoodad* pDoodad);
	virtual void OnRemoveDoodad(KDoodad* pDoodad);

    virtual void OnDoodadIdle(KDoodad* pDoodad);
    virtual void OnDoodadAfterHit(KDoodad* pDoodad);
    virtual void OnDoodadBeHit(KDoodad* pDoodad);
    virtual void OnDoodadLootList(KDoodad* pDoodad);
    virtual void OnUpdateDoodadQuestEffect();

    virtual void OnFinishQuest(KPlayer* pPlayer, DWORD dwQuestID, BOOL bAssist);
    virtual void OnReputationLevelUp(KPlayer* pPlayer, BOOL bLevelUp);

	virtual void OnDisconnect();

	virtual HRESULT SetSceneObjectAlpha(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, float fScale);
	virtual HRESULT SetSceneObjectBrightness(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, float fScale);

	virtual HRESULT SetSceneObjectSelectable(DWORD dwScene, DWORD dwObjectType, DWORD dwObject, BOOL bSelectable);

	virtual int SceneObjectTypeToGameWorldObjectType(DWORD dwSceneObjectType);
	virtual DWORD GameWorldObjectTypeToSceneObjectType(int nGameWorldObjectType);

    virtual HRESULT GetPlayerAnimationResource(ROLE_TYPE nRoleType, DWORD dwAnimationID, KRepresentPlayerAnimationResource* pResource) const;
	virtual HRESULT GetPlayerEquipResource(ROLE_TYPE nRoleType, DWORD dwRepresentIDs[perRepresentCount], KRepresentPlayerEquipResource* pResource) const;

    virtual HRESULT GetRidesAnimationResource(DWORD dwRidesID, DWORD dwAnimationID, KRepresentRidesAnimationResource* pResource) const;
    virtual HRESULT GetRidesEquipResource(DWORD dwRidesID, DWORD dwRepresentIDs[MODEL_RIDES_EQUIP_AND_SOCKET_COUNT], KRepresentRidesEquipResource* pResource) const;

	virtual HRESULT GetNPCAnimationResource(DWORD dwRepresentID, DWORD dwAnimationID, KRepresentNPCAnimationResource* pResource) const;
	virtual HRESULT GetNPCEquipResource(DWORD dwRepresentID, KRepresentNPCEquipResource* pResource) const;

	virtual HRESULT GetDoodadResource(DWORD dwRepresentID, KRepresentDoodadResource* pResource) const;

	virtual HRESULT ShowSelectionSFX(DWORD dwCursor, DWORD dwSkillID, DWORD dwSkillLevel);
	virtual HRESULT HideSelectionSFX(DWORD dwCursor);

	virtual HRESULT AttachTargetSceneObject(DWORD dwTarget, DWORD dwObject, DWORD dwObjectType);
	virtual HRESULT DetachTargetSceneObject(DWORD dwTarget, DWORD dwObject, DWORD dwObjectType);
	virtual HRESULT ShowTargetSelectionSFX(DWORD dwTarget, int nForceRelationType);
	virtual HRESULT HideTargetSelectionSFX(DWORD dwTarget);

    virtual void EnableHeadSFX(int nCharacterType, int nIndex, BOOL bEnable);
    virtual void UpdateHeadTopPosition();

	virtual int GetMinimapLayer(DWORD dwScene, D3DVECTOR const& vPos);
	virtual DWORD GetRegionInfo(DWORD dwScene, D3DVECTOR const& vPos);
	virtual DWORD GetRegionInfoByMapID(DWORD dwMapID, D3DVECTOR const& vPos);

	virtual HRESULT ExecuteCommand(LPCTSTR szCommand, void* pResult);

    virtual HRESULT PlayAnimation(DWORD dwScene, DWORD dwActor, DWORD dwTarget, BOOL bTurnToTarget, DWORD dwAnimationID);
    virtual HRESULT StopAnimation(DWORD dwScene, DWORD dwActor);

    virtual HRESULT PlaySpeech(DWORD dwScene, DWORD dwActor, DWORD dwTarget, BOOL bTurnToTarget, LPCTSTR pszFilePath);
    virtual HRESULT StopSpeech(DWORD dwScene, DWORD dwActor);

    virtual HRESULT MsgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

    virtual int GetCameraInitParams(
        int *pnCameraResetMode, float *pfCameraDragSpeed, float *pfCameraMaxDistance, float *pfCameraResetSpeed);
};

#endif // CASE_GAME_WORLD_K_GAME_WORLD_HANDLER_H