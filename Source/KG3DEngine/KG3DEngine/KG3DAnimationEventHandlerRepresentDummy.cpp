#include "StdAfx.h"
#include "KG3DAnimationEventHandlerRepresentDummy.h"
#include "kg3dcliptable.h"
#include "KG3DAnimationController.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern TCHAR g_szSceneRunnerAniName[MAX_PATH];
KG3DAnimationEventHandlerRepresentDummy* g_pRepresentEventHandlerDummy;

KG3DAnimationEventHandlerRepresentDummy::KG3DAnimationEventHandlerRepresentDummy(void)
{
    m_pModel = NULL;
}

KG3DAnimationEventHandlerRepresentDummy::~KG3DAnimationEventHandlerRepresentDummy(void)
{
	g_cClipTable.GetEventCenter()->RemoveEventHandler(this);
}


HRESULT KG3DAnimationEventHandlerRepresentDummy::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
    HRESULT hr = E_FAIL;
    //KG_PROCESS_ERROR(m_pModel);
    switch (pController->GetMotionExtraInfo())
    {
    case IKG3DAnimationController::TYPE_PART0://Top part
        {
          /* hr = m_pModel->PlaySplitAnimation("E:\\Sword\\sword3-products\\trunk\\client\\data\\public\\PlayerCheckTool\\Male\\Normal\\M2b01aq¹¥»÷21.ani", 
                ANIST_TYPE0,
                TRUE,
                ANI_JOINT_TOP,
                ENU_ANIMATIONPLAY_ONCE,
                1.0f,
                0,
                NULL);*/
//			int n = 0;
        }
        break;
    case IKG3DAnimationController::TYPE_PART1://bottom part
        {
//			int n = 0;
        }
        break;
    case IKG3DAnimationController::TYPE_ALLPART:
        {
//			int n = 0;
        }
        break;
    }
	IKG3DModel *pModel = pController->GetModel();
	KG_PROCESS_ERROR(pModel);
	pModel->PlayAnimation(ENU_ANIMATIONPLAY_ONCE, g_szSceneRunnerAniName, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
    hr = S_OK;
Exit0:
    return hr;
};

HRESULT KG3DAnimationEventHandlerRepresentDummy::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModel, TagSFXCallBackExtraInfo* pCallBackExtraInfo)
{
    return S_OK;
};

HRESULT KG3DAnimationEventHandlerRepresentDummy::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
    HRESULT hRetCode = E_FAIL;
    KG_PROCESS_ERROR(pMotionCallBackExtraInfo);

	switch(pMotionCallBackExtraInfo->Type)
	{
	case MTBT_DAMAGEPERCENTAGE:
		//Test Motion Tag Damage percentage
		ATLTRACE("Part %d, Action caused damage %f\n", 
			dwMotionInfo, 
			pMotionCallBackExtraInfo->fData[0]);
		break;
	case MTBT_MISSILE:
		//Test Motion Tag Missile
		ATLTRACE("Part %d, Missile ID %d, missile caused damage %f\n", 
			dwMotionInfo, 
			pMotionCallBackExtraInfo->dwData[0],
			pMotionCallBackExtraInfo->fData[0]);
		break;
	}
    hRetCode = S_OK;
Exit0:
    return hRetCode;
}