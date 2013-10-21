//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   IKGSO3WorldClient.h
//  Version     :   1.0
//  Creater     :   Chen Jie
//  Date        :   2007-11-15 15:55:19
//  Comment     :   游戏逻辑客户端(编辑器)接口
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _IKGSO3WORLDCLIENT_H_ 
#define _IKGSO3WORLDCLIENT_H_

#ifdef _CLIENT

#include "Global.h"
#include "INpcFile.h"
#include "IDoodadFile.h"

class KSceneObject;
class KCharacter;
class KPlayer;
class KDoodad;
class KScene;
class KTargetData;
class KSkill;
class KNpc;
struct KMapParams;

typedef BOOL (*KPlayerTraverser)(DWORD dwID, KPlayer* pPlayer);
typedef BOOL (*KNpcTraverser)(DWORD dwID, KNpc* pNpc);
typedef BOOL (*KDoodadTraverser)(DWORD dwID, KDoodad* pDoodad);

struct KCHARACTER_MAGIC_REPRESENT 
{
    DWORD dwHead;
    DWORD dwWaist;
    DWORD dwFoot;
};

enum KTITLE_MARK_TYPE
{
    tmtInvalid,

    tmtNoMark,
    tmtQuestMark,
    tmtShopMark,
    tmtPartyMark,

    tmtTotal
};

struct KCHARACTER_TITLE_MARK
{
    KTITLE_MARK_TYPE eMarkType;
    int              nPartyMarkType;
};

struct IKGSO3WorldClient
{
    virtual BOOL MoveCharacter(KCharacter* pCharacter, int nX, int nY, int nZ) = 0;
    virtual BOOL MoveCharacter(KPlayer* pPlayer, int nX, int nY, int nZ) = 0;
    virtual BOOL MoveCharacter(KNpc* pNpc, int nX, int nY, int nZ) = 0;
    virtual BOOL MoveCharacter(DWORD dwCharacterID, int nX, int nY, int nZ) = 0;
    
    virtual BOOL AddNpc(KScene* pScene, const KNPC_DATA& crNpcData) = 0;
    virtual BOOL DelNpc(KNpc* pNpc) = 0;
    virtual KNpc* GetNpc(DWORD dwNpcID) = 0;

    virtual BOOL SetDoodadScript(KDoodad* pDoodad, DWORD dwScriptID) = 0;
    virtual BOOL GetDoodadScript(KDoodad* pDoodad, DWORD* pdwRetScriptID) = 0;
    virtual KDoodad* GetDoodad(DWORD dwDoodadID) = 0;

    // Get object position
    virtual BOOL GetAbsoluteCoordinate(KSceneObject* pSceneObject, int* pnX, int* pnY, int* pnZ) = 0;
    virtual BOOL GetAbsoluteCoordinate(KCharacter* pCharacter, int* pnX, int* pnY, int* pnZ) = 0;
    virtual BOOL GetAbsoluteCoordinate(KPlayer* pPlayer, int* pnX, int* pnY, int* pnZ) = 0;
    virtual BOOL GetAbsoluteCoordinate(KNpc* pNpc, int* pnX, int* pnY, int* pnZ) = 0;
    virtual BOOL GetAbsoluteCoordinate(KDoodad* pDoodad, int* pnX, int* pnY, int* pnZ) = 0;
    virtual BOOL GetAbsoluteCoordinateByCharacterID(DWORD dwID, int* pnX, int* pnY, int* pnZ) = 0;
    virtual BOOL GetAbsoluteCoordinateByDoodadID(DWORD dwID, int* pnX, int* pnY, int* pnZ) = 0;

    virtual KPlayer* GetPlayer(DWORD dwPlayerID) = 0;


    virtual void MoveCtrl(BOOL bMove, BOOL bWalk, int nDirection, int nTurn) = 0;

    virtual void Stop(KCharacter* pCharacter) = 0;
    virtual void Stand(KCharacter* pCharacter) = 0;
    virtual void RunTo(KCharacter* pCharacter, int nDestX, int nDestY) = 0;
    virtual void WalkTo(KCharacter* pCharacter, int nDestX, int nDestY) = 0;
    virtual void SwimTo(KCharacter* pCharacter, int nDestX, int nDestY, int nDestZ) = 0;
    virtual void Jump(KCharacter* pCharacter, BOOL bStandJump, int nDirectionXY) = 0;
    virtual void TurnTo(KCharacter* pCharacter, int nNewDirection) = 0;
    virtual void PitchTo(KCharacter* pCharacter, int nNewDirection) = 0;

    virtual BOOL SetTarget(KTargetData* pTarget, TARGET_TYPE eType, DWORD dwID) = 0;
    virtual BOOL SetTarget(KTargetData* pTarget, KCharacter* pCharacter) = 0;
    virtual BOOL SetTarget(KTargetData* pTarget, KPlayer* pPlayer) = 0;
    virtual BOOL SetTarget(KTargetData* pTarget, KNpc* pNpc) = 0;

    virtual BOOL GetTarget(KTargetData* pTarget, KSceneObject** ppSceneObject) = 0;
    virtual BOOL GetTarget(KTargetData* pTarget, KCharacter** ppCharacter) = 0;
    virtual BOOL GetTarget(KTargetData* pTarget, int* pnX, int* pnY, int* pnZ) = 0;

    virtual DWORD GetPlayerEquipRepresent(KPlayer* pPlayer, int nEquipRepresentIndex) = 0;
    virtual BOOL GetCharacterMagicRepresent(KCharacter* pCharacter, KCHARACTER_MAGIC_REPRESENT* pMagicRepresent) = 0;
    
    virtual BOOL IsClientLootListVisible(KDoodad* pDoodad) = 0;
    virtual BOOL IsClientQuestEffectVisible(KDoodad* pDoodad) = 0;
    virtual void StopFollow(KCharacter* pCharacter) = 0;
    virtual KScene* GetScene(DWORD dwSceneID) = 0;

    virtual KSkill* GetSkill(DWORD dwSkillID, DWORD dwSkillLevel) = 0;
    virtual KMapParams* GetMapParams(DWORD dwMapID) = 0;
    virtual KScene* GetClientScene() = 0;
    virtual int GetLogicDirection(int nX, int nY) = 0;

    virtual int GetRelation(KCharacter* pSrcCharacter, KCharacter* pDstCharacter) = 0;
    virtual int GetSubjectiveDirection(KCharacter* pCharacter) = 0;
    virtual int GetSubjectiveVelocity(KCharacter* pCharacter) = 0;
    virtual BOOL GetRepresentState(KCharacter* pCharacter, KBUFF_REPRESENT_POS nPos, DWORD& dwRepresentID) = 0;

    virtual BOOL TraversePlayerSet(KPlayerTraverser PlayerTraverser) = 0;
    virtual BOOL TraverseNpcSet(KNpcTraverser NpcTraverser) = 0;
    virtual BOOL TraverseDoodadSet(KDoodadTraverser DoodadTraverser) = 0;

    virtual int GetWaterline(KCharacter* pCharacter) = 0;

    virtual KCHARACTER_TITLE_MARK GetCharacterTitleMark(KCharacter* pCharacter) = 0;

    virtual time_t GetWorldTime() = 0;

    virtual int BeginForwardCharacterPosition(KCharacter* pCharacter, KPOSITION_RECORD* pPositionRecord) = 0;
    virtual int EndForwardCharacterPosition(KCharacter* pCharacter, KPOSITION_RECORD* pPositionRecord) = 0;
	virtual int ForwardCharacterPosition(KCharacter* pCharacter, int nFrame) = 0;
};

IKGSO3WorldClient* QuerySO3WorldClientInterface();

#endif // #ifdef _CLIENT

#endif // #ifndef _IKGSO3WORLDCLIENT_H_ 
