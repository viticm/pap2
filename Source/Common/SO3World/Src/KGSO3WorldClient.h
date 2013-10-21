//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KGSO3WorldClient.h
//  Version     :   1.0
//  Creater     :   Chen Jie
//  Date        :   2007-11-15 15:55:19
//  Comment     :   游戏逻辑客户端(编辑器)接口实现
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGSO3WORLDCLIENT_H_ 
#define _KGSO3WORLDCLIENT_H_

#ifdef _CLIENT

#include "SO3World/IKGSO3WorldClient.h"

struct KGSO3WorldClientInterface : public IKGSO3WorldClient
{
    virtual BOOL MoveCharacter(KCharacter* pCharacter, int nX, int nY, int nZ);
    virtual BOOL MoveCharacter(KPlayer* pPlayer, int nX, int nY, int nZ);
    virtual BOOL MoveCharacter(KNpc* pNpc, int nX, int nY, int nZ);
    virtual BOOL MoveCharacter(DWORD dwCharacterID, int nX, int nY, int nZ);

    virtual BOOL AddNpc(KScene* pScene, const KNPC_DATA& crNpcData);
    virtual BOOL DelNpc(KNpc* pNpc);
    virtual KNpc* GetNpc(DWORD dwNpcID);

    virtual BOOL SetDoodadScript(KDoodad* pDoodad, DWORD dwScriptID);
    virtual BOOL GetDoodadScript(KDoodad* pDoodad, DWORD* pdwRetScriptID);
    virtual KDoodad* GetDoodad(DWORD dwDoodadID);

    virtual BOOL GetAbsoluteCoordinate(KSceneObject* pSceneObject, int* pnX, int* pnY, int* pnZ);
    virtual BOOL GetAbsoluteCoordinate(KCharacter* pCharacter, int* pnX, int* pnY, int* pnZ);
    virtual BOOL GetAbsoluteCoordinate(KPlayer* pPlayer, int* pnX, int* pnY, int* pnZ);
    virtual BOOL GetAbsoluteCoordinate(KNpc* pNpc, int* pnX, int* pnY, int* pnZ);
    virtual BOOL GetAbsoluteCoordinate(KDoodad* pDoodad, int* pnX, int* pnY, int* pnZ);

    virtual BOOL GetAbsoluteCoordinateByCharacterID(DWORD dwID, int* pnX, int* pnY, int* pnZ);
    virtual BOOL GetAbsoluteCoordinateByDoodadID(DWORD dwID, int* pnX, int* pnY, int* pnZ);
    
    virtual KPlayer* GetPlayer(DWORD dwPlayerID);

    virtual void MoveCtrl(BOOL bMove, BOOL bWalk, int nDirection, int nTurn);

    virtual void Stop(KCharacter* pCharacter);
    virtual void Stand(KCharacter* pCharacter);
    virtual void RunTo(KCharacter* pCharacter, int nDestX, int nDestY);
    virtual void WalkTo(KCharacter* pCharacter, int nDestX, int nDestY);
    virtual void SwimTo(KCharacter* pCharacter, int nDestX, int nDestY, int nDestZ);
    virtual void Jump(KCharacter* pCharacter, BOOL bStandJump, int nDirectionXY);
    virtual void TurnTo(KCharacter* pCharacter, int nNewDirection);
    virtual void PitchTo(KCharacter* pCharacter, int nNewDirection);

    virtual BOOL SetTarget(KTargetData* pTarget, TARGET_TYPE eType, DWORD dwID);
    virtual BOOL SetTarget(KTargetData* pTarget, KCharacter* pCharacter);
    virtual BOOL SetTarget(KTargetData* pTarget, KPlayer* pPlayer);
    virtual BOOL SetTarget(KTargetData* pTarget, KNpc* pNpc);

    virtual BOOL GetTarget(KTargetData* pTarget, KSceneObject** ppSceneObject);
    virtual BOOL GetTarget(KTargetData* pTarget, KCharacter** ppCharacter);
    virtual BOOL GetTarget(KTargetData* pTarget, int* pnX, int* pnY, int* pnZ);

    virtual DWORD GetPlayerEquipRepresent(KPlayer* pPlayer, int nEquipRepresentIndex);
    virtual BOOL GetCharacterMagicRepresent(KCharacter* pCharacter, KCHARACTER_MAGIC_REPRESENT* pMagicRepresent);

    virtual BOOL IsClientLootListVisible(KDoodad* pDoodad);
    virtual BOOL IsClientQuestEffectVisible(KDoodad* pDoodad);
    virtual void StopFollow(KCharacter* pCharacter);
    virtual KScene* GetScene(DWORD dwSceneID);
    virtual KSkill* GetSkill(DWORD dwSkillID, DWORD dwSkillLevel);
    virtual KMapParams* GetMapParams(DWORD dwMapID);
    virtual KScene* GetClientScene();

    virtual int GetLogicDirection(int nX, int nY);

    virtual int GetRelation(KCharacter* pSrcCharacter, KCharacter* pDstCharacter);
    virtual int GetSubjectiveDirection(KCharacter* pCharacter);
    virtual int GetSubjectiveVelocity(KCharacter* pCharacter);
    virtual BOOL GetRepresentState(KCharacter* pCharacter, KBUFF_REPRESENT_POS nPos, DWORD& dwRepresentID);

    virtual BOOL TraversePlayerSet(KPlayerTraverser PlayerTraverser);
    virtual BOOL TraverseNpcSet(KNpcTraverser NpcTraverser);
    virtual BOOL TraverseDoodadSet(KDoodadTraverser DoodadTraverser);

    virtual int GetWaterline(KCharacter* pCharacter);

    virtual KCHARACTER_TITLE_MARK GetCharacterTitleMark(KCharacter* pCharacter);

    virtual time_t GetWorldTime();

    virtual int BeginForwardCharacterPosition(KCharacter* pCharacter, KPOSITION_RECORD* pPositionRecord);
    virtual int EndForwardCharacterPosition(KCharacter* pCharacter, KPOSITION_RECORD* pPositionRecord);
	virtual int ForwardCharacterPosition(KCharacter* pCharacter, int nFrame);

    virtual void MoveViewPoint(int nX, int nY, int nZ);

    virtual int IsPartyMember(DWORD dwLeader, DWORD dwMemberID, BOOL* pbIsMember);
};


#endif // #ifdef _CLIENT

#endif // #ifndef _KGSO3WORLDCLIENT_H_ 
