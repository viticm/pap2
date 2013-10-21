#ifndef CASE_ACTION_OBJECT_KRLSCENE_H
#define CASE_ACTION_OBJECT_KRLSCENE_H

#include "./krlremotecharactermgr.h"
#include "./krldoodadmgr.h"
#include "./krlmissilemgr.h"
#include "./krlridesmgr.h"
#include "./krlskilleffectresult.h"
#include "../render/kmodelgcmgr.h"
#include "../render/kmodelmgr.h"

class IKG3DScene;
class KRLCamera;
class KRLCursor;
class KRLTarget;
class KRLCharacter;

class KRLScene
{
public:
	KRLScene();
	~KRLScene();

	int Init();
	void Exit();
	BOOL Reset();
	BOOL Activate(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame);

	BOOL InitCamera();
	void ExitCamera();

	BOOL InitCursor();
	void ExitCursor();

	BOOL InitTarget();
	void ExitTarget();

public:
	KRLCamera* m_pRLCamera;
	KRLCursor* m_pRLCursor;
	KRLTarget* m_pRLTarget;

    // 运行中队列
    KRLMissileMgr m_MissileMgr;
	KRLRemoteCharacterMgr m_CharacterMgr;
	KRLDoodadMgr m_DoodadMgr;
    KRLRidesMgr m_RidesMgr;

    // 删除队列
    KRLRemoteCharacterMgr m_CharacterGCMgr;
	KRLDoodadMgr m_DoodadGCMgr;
    KRLRidesMgr m_RidesGCMgr;

    KRLSkillEffectResult m_SkillEffectResult;

    KModelGCMgr m_ModelGCMgr;
    KModelMgr m_ModelMgr;

	DWORD m_dwScene;
	IKG3DScene* m_p3DScene;
	int	m_nOutputWindowID;
};

KRLDoodad* GetRLDoodad(DWORD dwDoodad);
KRLCharacter* GetRLCharacter(DWORD dwCharacter);

#endif // CASE_ACTION_OBJECT_KRLSCENE_H