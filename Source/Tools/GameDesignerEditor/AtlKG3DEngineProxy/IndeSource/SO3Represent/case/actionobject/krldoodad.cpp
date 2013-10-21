#include "stdafx.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "./krlscene.h"
#include "./krldoodad.h"
#include "./krlremotecharacter.h"
#include "../gameworld/ktabletransform.h"
#include "../../common/kmathtools.h"
#include "../../common/ksmallobjectpool.h"
#include "../../so3represent.h"

#pragma warning(push)
#pragma warning(disable : 4355)

namespace
{
	KSmallObjectPool	g_Allocator(_T("KRLDoodad"), sizeof(KRLDoodad));
}

void* KRLDoodad::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KRLDoodad::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KRLDoodad::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KRLDoodad::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

KRLDoodad::KRLDoodad()
: m_nState(RL_DOODAD_STATE_NONE)
, m_bCorpse(FALSE)
, m_bOnFloor(FALSE)
, m_bSelectable(FALSE)
, m_dwRepresentID(0)
{
    m_RenderData.Init(this, MODEL_DOODAD_EQUIP_COUNT, MODEL_DOODAD_SOCKET_COUNT, DOODAD_SFX_COUNT);
}

KRLDoodad::~KRLDoodad()
{
    Reset();
}

int KRLDoodad::Init(KDoodad* pDoodad, KRLScene* pRLScene)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;

	ASSERT(pDoodad);
	ASSERT(pRLScene);
    
    m_bCorpse = pDoodad->m_eKind == dkCorpse;
    m_bOnFloor = TRUE;

    pDoodad->m_pRepresentObject = reinterpret_cast<void*>(this);

    m_RenderData.AttachScene(pRLScene);
    
	m_FrameData.m_pDoodad = pDoodad;
	m_FrameData.UpdateData();

    nRetCode = UpdateDisplayData(TRUE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = SetState(RL_DOODAD_STATE_ENTER_SCENE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        hr = m_RenderData.Reset();
        KGLOG_COM_CHECK_ERROR(hr);

        if (pDoodad)
        {
            pDoodad->m_pRepresentObject = NULL;
        }
    }
    return nResult;
}

int KRLDoodad::InitCorpse(KDoodad* pDoodad, KRLScene* pRLScene, KRLRemoteCharacter* pRLRemoteCharacter)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;

	ASSERT(pDoodad);
    ASSERT(pDoodad->m_eKind == dkCorpse);
	ASSERT(pRLRemoteCharacter);
    
    m_bCorpse = TRUE;
    m_bOnFloor = TRUE;

    pDoodad->m_pRepresentObject = reinterpret_cast<void*>(this);

    m_RenderData.AttachScene(pRLScene);
    m_RenderData.TransferOwnershipFrom(pRLRemoteCharacter->m_RLCharacter.m_RenderData);
    
    nRetCode = m_RenderData.RemoveCaption();
    KGLOG_PROCESS_ERROR(nRetCode);

    hr = m_RenderData.UnloadAllMDLSFX();
    KGLOG_COM_PROCESS_ERROR(hr);

	m_FrameData.m_pDoodad = pDoodad;
	m_FrameData.UpdateData();

    nRetCode = SetState(RL_DOODAD_STATE_ENTER_SCENE);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        hr = m_RenderData.Reset();
        KGLOG_COM_CHECK_ERROR(hr);

        if (pDoodad)
        {
            pDoodad->m_pRepresentObject = NULL;
        }
    }
    return nResult;
}

int KRLDoodad::Reset()
{
    int nResult = false;
	HRESULT hr = E_FAIL;

    hr = m_RenderData.Reset();
    KGLOG_COM_CHECK_ERROR(hr);

    hr = m_RenderData.DetachScene();
    KGLOG_COM_CHECK_ERROR(hr);

	m_FrameData.m_pDoodad = NULL;

    m_dwRepresentID = 0;

    nResult = true;
    return nResult;
}

static DWORD GetRepresentID(KDoodad const* pDoodad)
{
    KRLDoodadUniformModel const* pRLDoodadUniformModel = NULL;
    DWORD dwRepresentID = 0;

    KGLOG_PROCESS_ERROR(pDoodad);

    if (pDoodad->m_eKind == dkCorpse)
    {
        if (g_pRL->m_CurrentOptimizationOption.bHideCorpse)
        {
            pRLDoodadUniformModel = g_pRL->m_TableList.GetDoodadUniformModel(RL_DOODAD_UNIFORM_NONE);
            goto Exit1;
        }

        if (g_pRL->m_CurrentOptimizationOption.bCorpseUniqueUniform)
        {
            pRLDoodadUniformModel = g_pRL->m_TableList.GetDoodadUniformModel(RL_DOODAD_UNIFORM_CORPSE);
            goto Exit1;
        }
    }
    else
    {
        if (g_pRL->m_CurrentOptimizationOption.bHideDoodad)
        {
            pRLDoodadUniformModel = g_pRL->m_TableList.GetDoodadUniformModel(RL_DOODAD_UNIFORM_NONE);
            goto Exit1;
        }

        if (g_pRL->m_CurrentOptimizationOption.bDoodadUniform)
        {
            pRLDoodadUniformModel = g_pRL->m_TableList.GetDoodadUniformModel(RL_DOODAD_UNIFORM_DOODAD);
            goto Exit1;
        }
    }

Exit1:
    if (pRLDoodadUniformModel)
        dwRepresentID = pRLDoodadUniformModel->dwRepresentID;
    else
        dwRepresentID = pDoodad->m_dwRepresentID;

Exit0:
    return dwRepresentID;
}

int KRLDoodad::UpdateDisplayData(BOOL bForceUpdate)
{
    int nResult = false;
	HRESULT hr = E_FAIL;
	D3DXVECTOR3 vPosition;
	BOOL bFadeIn = FALSE;

    m_dwRepresentID = GetRepresentID(m_FrameData.m_pDoodad);

    if (m_FrameData.m_pDoodad->m_eKind == dkCorpse)
    {
        if (m_RenderData.HasLoadedMDL())
        {
            hr = m_RenderData.Reset();
            KGLOG_COM_CHECK_ERROR(hr);
        }
         
        hr = LoadCorpseMDL(bForceUpdate);
        KG_COM_PROCESS_ERROR(hr);

        hr = LoadCorpseSocket();
        KGLOG_COM_CHECK_ERROR(hr);
    }
	else
	{
        KRLDoodadModel const* pDoodadModel = NULL;

        pDoodadModel = g_pRL->m_TableList.GetDoodadModel(m_dwRepresentID);
        if (pDoodadModel)
        {
            m_bOnFloor = pDoodadModel->bOnFloor;
            bFadeIn = pDoodadModel->bFadeIn;
        }
	}

    hr = GameWorldPositionToScenePosition(m_RenderData.GetRLScene()->m_p3DScene, vPosition, 
        (float)m_FrameData.m_Current.nX, (float)m_FrameData.m_Current.nY, (float)m_FrameData.m_Current.nZ, m_bOnFloor, false, false);
    KGLOG_COM_PROCESS_ERROR(hr);

    SetPosition(vPosition);

	if (bFadeIn)
		SetFadeAlpha(0.0f);
	FadeIn();

    nResult = true;
Exit0:
    return nResult;
}

int KRLDoodad::UpdateState(BOOL bForceUpdate)
{
    int nRetCode = false;
    int nResult = false;

    nRetCode = UpdateState(m_nState, bForceUpdate);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLDoodad::SetState(RL_DOODAD_STATE nState)
{
    int nRetCode = false;
    int nResult = false;
    RL_DOODAD_ANIMATION nAnimation = RL_DOODAD_ANIMATION_IDLE;

    KGLOG_PROCESS_ERROR(m_FrameData.m_pDoodad);

    if (m_FrameData.m_pDoodad->m_eKind == dkCorpse)
    {
        if (m_nState == RL_DOODAD_STATE_NONE)
        {
            nRetCode = UpdateState(nState, TRUE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }
    else
    {
        if (m_nState != nState)
        {
            nRetCode = UpdateState(nState, TRUE);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

    nResult = true;
Exit0:
    m_nState = nState;

    return nResult;
}

static RL_DOODAD_ANIMATION GetDoodadAnimationID(RL_DOODAD_STATE nPreviousState, RL_DOODAD_STATE nCurrentState)
{
    RL_DOODAD_ANIMATION nDoodadAnimation = RL_DOODAD_ANIMATION_IDLE;

    switch (nPreviousState)
    {
    case RL_DOODAD_STATE_NONE:
        switch (nCurrentState)
        {
        case RL_DOODAD_STATE_NONE:
            break;
        case RL_DOODAD_STATE_ENTER_SCENE:
            nDoodadAnimation = RL_DOODAD_ANIMATION_ENTER_SCENE;
            break;
        case RL_DOODAD_STATE_LEAVE_SCENE:
            nDoodadAnimation = RL_DOODAD_ANIMATION_LEAVE_SCENE;
            break;
        case RL_DOODAD_STATE_IDLE:
            break;
        case RL_DOODAD_STATE_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_OPERATION;
            break;
        case RL_DOODAD_STATE_AFTER_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_AFTER_OPERATION;
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    case RL_DOODAD_STATE_ENTER_SCENE:
        switch (nCurrentState)
        {
        case RL_DOODAD_STATE_NONE:
        case RL_DOODAD_STATE_ENTER_SCENE:
        case RL_DOODAD_STATE_LEAVE_SCENE:
        case RL_DOODAD_STATE_IDLE:
            break;
        case RL_DOODAD_STATE_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_OPERATION;
            break;
        case RL_DOODAD_STATE_AFTER_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_AFTER_OPERATION;
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    case RL_DOODAD_STATE_LEAVE_SCENE:
        switch (nCurrentState)
        {
        case RL_DOODAD_STATE_NONE:
        case RL_DOODAD_STATE_ENTER_SCENE:
        case RL_DOODAD_STATE_LEAVE_SCENE:
        case RL_DOODAD_STATE_IDLE:
            break;
        case RL_DOODAD_STATE_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_OPERATION;
            break;
        case RL_DOODAD_STATE_AFTER_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_AFTER_OPERATION;
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    case RL_DOODAD_STATE_IDLE:
        switch (nCurrentState)
        {
        case RL_DOODAD_STATE_NONE:
        case RL_DOODAD_STATE_ENTER_SCENE:
        case RL_DOODAD_STATE_LEAVE_SCENE:
        case RL_DOODAD_STATE_IDLE:
            break;
        case RL_DOODAD_STATE_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_IDLE_TO_OPERATION;
            break;
        case RL_DOODAD_STATE_AFTER_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_IDLE_TO_AFTER_OPERATION;
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    case RL_DOODAD_STATE_OPERATION:
        switch (nCurrentState)
        {
        case RL_DOODAD_STATE_NONE:
        case RL_DOODAD_STATE_ENTER_SCENE:
        case RL_DOODAD_STATE_LEAVE_SCENE:
        case RL_DOODAD_STATE_IDLE:
            break;
        case RL_DOODAD_STATE_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_OPERATION;
            break;
        case RL_DOODAD_STATE_AFTER_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_OPERATION_TO_AFTER_OPERATION;
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    case RL_DOODAD_STATE_AFTER_OPERATION:
        switch (nCurrentState)
        {
        case RL_DOODAD_STATE_NONE:
        case RL_DOODAD_STATE_ENTER_SCENE:
        case RL_DOODAD_STATE_LEAVE_SCENE:
        case RL_DOODAD_STATE_IDLE:
            break;
        case RL_DOODAD_STATE_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_AFTER_OPERATION_TO_OPERATION;
            break;
        case RL_DOODAD_STATE_AFTER_OPERATION:
            nDoodadAnimation = RL_DOODAD_ANIMATION_AFTER_OPERATION;
            break;
        default:
            ASSERT(0);
            break;
        }
        break;
    default:
        ASSERT(0);
        break;
    }

    return nDoodadAnimation;
}

int KRLDoodad::UpdateState(RL_DOODAD_STATE nState, BOOL bForceUpdate)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    RL_DOODAD_ANIMATION nAnimation = RL_DOODAD_ANIMATION_IDLE;

	ASSERT(nState != RL_DOODAD_STATE_NONE);

    KG_PROCESS_ERROR(m_FrameData.m_pDoodad);

    if (m_FrameData.m_pDoodad->m_eKind == dkCorpse)
    {
        nRetCode = PlayAnimation(RL_DOODAD_ANIMATION_IDLE);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        if (bForceUpdate)
        {
            nAnimation = GetDoodadAnimationID(m_nState, nState);

            nRetCode = PlayAnimation(nAnimation);
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

    hr = UpdateLootList();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = UpdateQuestEffect();
    KGLOG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    return nResult;
}

int KRLDoodad::SetSelectable(BOOL bSelectable)
{
    int nResult = false;
    HRESULT hr = E_FAIL;

    KGLOG_PROCESS_ERROR(m_FrameData.m_pDoodad);

    switch (m_FrameData.m_pDoodad->m_eKind)
    {
    case dkInvalid:
    case dkNormal:					// 普通的Doodad,有Tip,不能操作
        bSelectable = TRUE;
        break;
    case dkCorpse:					// 尸体
        bSelectable = TRUE;
        break;
    case dkQuest:					// 任务相关的Doodad
        break;
    case dkRead:					// 可以看的Doodad
    case dkDialog:					// 可以对话的Doodad
    case dkAcceptQuest:				// 可以接任务的Doodad,本质上上面3个类型是一样的,只是图标不同而已
    case dkTreasure:				// 宝箱
    case dkOrnament:				// 装饰物,不能操作
    case dkCraftTarget:				// 生活技能的采集物
        bSelectable = TRUE;
        break;
	case dkClientOnly:				// 客户端用
    case dkNull:					// 摆摊Doodad(已删除，位置可给新的类型使用)
    case dkChair:					// 可以坐的Doodad
    case dkGuide:                   // 路标
	case dkDoor:					// 门之类有动态障碍的
        bSelectable = TRUE;
        break;
    default:
        ASSERT(0);
        break;
    }

    hr = m_RenderData.SetMDLSelectable(bSelectable);;
    KGLOG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLDoodad::UpdateLootList()
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bSelectable = FALSE;

    KG_PROCESS_SUCCESS(!m_RenderData.HasLoadedMDL());

    KG_PROCESS_ERROR(m_FrameData.m_pDoodad);

    if (m_FrameData.m_pDoodad->m_eKind == dkCorpse)
    {
	    if (g_pRL->m_pSO3WorldClient->IsClientLootListVisible(m_FrameData.m_pDoodad))
        {
            hr = LoadDoodadEffect(DOODAD_SFX_LOOT_LIST);
            KGLOG_COM_PROCESS_ERROR(hr);

            bSelectable = TRUE;
        }
        else
        {
            bSelectable = FALSE;

            hr = UnloadDoodadEffect(DOODAD_SFX_LOOT_LIST);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    nRetCode = SetSelectable(bSelectable);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLDoodad::UpdateQuestEffect()
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bSelectable = FALSE;

    KG_PROCESS_SUCCESS(!m_RenderData.HasLoadedMDL());

    KG_PROCESS_ERROR(m_FrameData.m_pDoodad);

    if (m_FrameData.m_pDoodad->m_eKind != dkCorpse)
    {
        if (g_pRL->m_pSO3WorldClient->IsClientQuestEffectVisible(m_FrameData.m_pDoodad))
        {
            bSelectable = TRUE;

            hr = LoadDoodadEffect(DOODAD_SFX_QUEST);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
        else
        {
            bSelectable = FALSE;

            hr = UnloadDoodadEffect(DOODAD_SFX_QUEST);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    nRetCode = SetSelectable(bSelectable);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLDoodad::PlayAnimation(RL_DOODAD_ANIMATION nAni)
{
    int nRetCode = false;
    int nResult = false;
	HRESULT hr = E_FAIL;

	ASSERT(nAni < RL_DOODAD_ANIMATION_COUNT);

    KG_PROCESS_ERROR(m_FrameData.m_pDoodad);

    if (m_FrameData.m_pDoodad->m_eKind == dkCorpse)
    {
        KRLNpcModel const* pRLNpcModel = NULL;
        DWORD dwAnimationID = 0;

        pRLNpcModel = g_pRL->m_TableList.GetNpcModel(m_dwRepresentID);
        KGLOG_PROCESS_ERROR(pRLNpcModel);

        dwAnimationID = KRLNpcModel::GetAnimationID(pRLNpcModel, RL_NPC_ANIMATION_TABLE_DEATH);

        nRetCode = PlayCorpseAnimation(dwAnimationID);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else 
    {
        KRLDoodadModel const* pRLDoodadModel = NULL;

		pRLDoodadModel = g_pRL->m_TableList.GetDoodadModel(m_dwRepresentID);
        KGLOG_PROCESS_ERROR(pRLDoodadModel);

        hr = LoadDoodadMDL(nAni, *pRLDoodadModel);
        KGLOG_COM_PROCESS_ERROR(hr);

        nRetCode = PlayDoodadAnimation(nAni, *pRLDoodadModel);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
	
    nResult = true;
Exit0:
	return nResult;
}

HRESULT KRLDoodad::LoadDoodadMDL(RL_DOODAD_ANIMATION nAni, KRLDoodadModel const& Model)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    D3DXVECTOR3 vPosition;
	KRLDoodadModel::KRLAnimationParam const& Animation = Model.Animations[nAni];

    ASSERT(nAni < RL_DOODAD_ANIMATION_COUNT);

    KG_PROCESS_SUCCESS(Animation.szMeshFilePath[0] == _T('\0'));

    if (m_RenderData.HasLoadedMDL())
    {
        hr = m_RenderData.UnloadMDL();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    m_bSelectable = Model.bSelectable;

    hr = m_RenderData.SetMDLSelectable(m_bSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.SetMDLVisible(TRUE);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.LoadMDL(Animation.szMeshFilePath, Model.fModelScale, Model.nColorChannelTable, Model.nColorChannel);
    KGLOG_COM_PROCESS_ERROR(hr);

    GetPosition(vPosition);
    SetPosition(vPosition);

    KRLSceneObject::SetDirectionLH(m_FrameData.m_Current.fFaceDirection);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLDoodad::PlayDoodadAnimation(RL_DOODAD_ANIMATION nAni, KRLDoodadModel const& RLDoodadModel)
{
    int nResult = false;
    HRESULT hr = E_FAIL;

    ASSERT(nAni < RL_DOODAD_ANIMATION_COUNT);
    
    if (RLDoodadModel.Animations[nAni].szAniFilePath[0] != _T('\0'))
    {
        KRLMonoAnimationParam MonoAnimationParam;

        MonoAnimationParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)RLDoodadModel.Animations[nAni].dwAniPlayType);
        MonoAnimationParam.nTrackType = ANICTL_PRIMARY;
        MonoAnimationParam.fSpeed = 1.0f;
        MonoAnimationParam.nOffsetTime = 0;
        MonoAnimationParam.dwTweenTime = 0;
        MonoAnimationParam.pUserdata = NULL;
        MonoAnimationParam.szFilePath = RLDoodadModel.Animations[nAni].szAniFilePath;
        MonoAnimationParam.fStartTime = g_pRL->m_fTime;

        hr = m_RenderData.PlayMonoAnimation(MonoAnimationParam);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (RLDoodadModel.Animations[nAni].szSFXFilePath[0] != _T('\0'))
    {
        KRLSFXParam SFXParam;

        SFXParam.dwPlayType = RLDoodadModel.Animations[nAni].dwAniPlayType;
        SFXParam.szFilePath = RLDoodadModel.Animations[nAni].szSFXFilePath;
        SFXParam.fScale = 1.0f;
        SFXParam.fSpeed = 1.0f;
        SFXParam.szBoneName = _T("");

        hr = m_RenderData.LoadMDLSFX(DOODAD_SFX_MAIN, SFXParam);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KRLDoodad::PlayCorpseAnimation(DWORD dwAnimationID)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    KNpcAnimationModel const* pAnimationModel = NULL;
    KRLMonoAnimationParam MonoAnimationParam;
    KRLSFXParam SFXParam;
    DWORD dwTrackType = 0;

    KG_PROCESS_SUCCESS(!m_RenderData.HasLoadedMDL());

    pAnimationModel = g_pRL->m_TableList.GetNpcAnimationModel(m_dwRepresentID, dwAnimationID);
	KG_PROCESS_SUCCESS(pAnimationModel == NULL);

	MonoAnimationParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pAnimationModel->dwAnimationType);
	MonoAnimationParam.fSpeed = pAnimationModel->fAnimationSpeed;
    MonoAnimationParam.nOffsetTime = ANIMATION_SEEK_OFFSET_END;
    MonoAnimationParam.dwTweenTime = 0;
    MonoAnimationParam.pUserdata = NULL;
	MonoAnimationParam.szFilePath = pAnimationModel->szAnimationFile;
    MonoAnimationParam.fStartTime = g_pRL->m_fTime;

    dwTrackType = g_pRL->m_TableList.GetNPCAnimationTrackType(pAnimationModel->dwKindID);
    if (dwTrackType > 0)
        MonoAnimationParam.nTrackType = ANICTL_SECONDARY;
    else
        MonoAnimationParam.nTrackType = ANICTL_PRIMARY;

	SFXParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pAnimationModel->dwSFXType);
	SFXParam.fSpeed = pAnimationModel->fSFXSpeed;
	SFXParam.fScale = pAnimationModel->fSFXScale;
	SFXParam.szFilePath = pAnimationModel->szSFXFile;
	SFXParam.szBoneName = pAnimationModel->szSFXBoneName;

    hr = m_RenderData.UnloadAllSocket();
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = LoadCorpseSocket(pAnimationModel);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.PlayMonoAnimation(MonoAnimationParam);
	KGLOG_COM_PROCESS_ERROR(hr);

    if (SFXParam.szFilePath[0] != _T('\0'))
    {
        hr = m_RenderData.LoadMDLSFX(DOODAD_SFX_MAIN, SFXParam);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLDoodad::LoadCorpseMDL(BOOL bForceUpdate)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLNpcModel const* pRLNpcModel = NULL;
    
    pRLNpcModel = g_pRL->m_TableList.GetNpcModel(m_dwRepresentID);
    KGLOG_PROCESS_ERROR(pRLNpcModel);

    if (bForceUpdate)
        m_bSelectable = pRLNpcModel->bSelectable;
    else
        m_bSelectable = m_RenderData.IsMDLSelectable();

    hr = m_RenderData.SetMDLSelectable(m_bSelectable);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.SetMDLVisible(TRUE);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.LoadMDL(
        pRLNpcModel->szMainModelFile, 
        pRLNpcModel->fModelScale, 
        pRLNpcModel->nColorChannelTable, 
        pRLNpcModel->nColorChannel);
    KG_COM_PROCESS_ERROR(hr);

    KRLSceneObject::SetDirectionLH(m_FrameData.m_Current.fFaceDirection);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLDoodad::LoadCorpseSocket()
{
    HRESULT hr = E_FAIL;
    KRLNpcModel const* pRLNpcModel = NULL;

    pRLNpcModel = g_pRL->m_TableList.GetNpcModel(m_FrameData.m_pDoodad->m_dwRepresentID);
    KGLOG_PROCESS_ERROR(pRLNpcModel);

    for (int nIndex = 0; nIndex != RL_NPC_SOCKET_COUNT; ++nIndex)
    {
        KRLNpcModel::KSocket const& part = pRLNpcModel->Sockets[nIndex];

        hr = LoadCorpseSocket(nIndex, part.szMeshFilePath, part.szMtlFilePath, pRLNpcModel->fSocketScale,
            pRLNpcModel->nColorChannelTable, pRLNpcModel->nColorChannel);
        KGLOG_COM_CHECK_ERROR(hr);
    }

Exit0:
    return S_OK;
}

HRESULT KRLDoodad::LoadCorpseSocket(KNpcAnimationModel const* pAnimationModel)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    BOOL bLoadDefult = FALSE;
    KRLNpcModel const* pNpcModel = NULL;
    
    ASSERT(pAnimationModel);

    bLoadDefult = pAnimationModel->bLoadDefaultSocket;
    
    pNpcModel = g_pRL->m_TableList.GetNpcModel(m_FrameData.m_pDoodad->m_dwRepresentID);
    KGLOG_PROCESS_ERROR(pNpcModel);

    for (int nIndex = 0; nIndex != RL_NPC_SOCKET_COUNT; ++ nIndex)
    {
        if (bLoadDefult && pAnimationModel->SocketInfo[nIndex].szMeshFilePath[0] == _T('\0'))
        {
            KRLNpcModel::KSocket const& part = pNpcModel->Sockets[nIndex];

            hr = LoadCorpseSocket(nIndex, part.szMeshFilePath, part.szMtlFilePath, pNpcModel->fSocketScale, 
                pNpcModel->nColorChannelTable, pNpcModel->nColorChannel);
            KGLOG_COM_CHECK_ERROR(hr);
        }
        else
        {
            KNpcAnimationModel::KSocketAnimation const& part = pAnimationModel->SocketInfo[nIndex];

            hr = LoadCorpseSocket(nIndex, part.szMeshFilePath, part.szMaterialFile, pNpcModel->fSocketScale, 
                pNpcModel->nColorChannelTable, pNpcModel->nColorChannel);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLDoodad::LoadCorpseSocket(size_t nSocketIndex, LPCTSTR szMeshFilePath, LPCTSTR szMtlFilePath, float fSocketScale, int nColorChannelTable, int nColorChannel)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
	LPCTSTR szSocket = GetNpcSocketName(nSocketIndex);

    KG_PROCESS_SUCCESS(szSocket[0] == _T('\0'));
    KG_PROCESS_SUCCESS(szMeshFilePath[0] == _T('\0'));

    if (nSocketIndex == RL_NPC_SOCKET_FACE)
        fSocketScale = 1.0f;

    hr = m_RenderData.LoadSocket(nSocketIndex, szSocket, szMeshFilePath, szMtlFilePath, fSocketScale, nColorChannelTable, nColorChannel);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLDoodad::LoadDoodadEffect(DWORD dwEffectID)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    float fScale = 1.0f;
    KDoodadEffectModel const* pDoodadEffectModel = NULL;
    KRLSFXParam SFXParam;

    KG_PROCESS_SUCCESS(m_RenderData.HasLoadedMDLSFX(dwEffectID));

    KGLOG_PROCESS_ERROR(m_FrameData.m_pDoodad);

    if (m_FrameData.m_pDoodad->m_eKind == dkCorpse)
    {
        KRLNpcModel const* pNpcModel = NULL;

        KG_PROCESS_SUCCESS(g_pRL->m_CurrentOptimizationOption.bHideCorpseEffect);

        pNpcModel = g_pRL->m_TableList.GetNpcModel(m_FrameData.m_pDoodad->m_dwRepresentID);
        if (pNpcModel)
        {
            fScale = pNpcModel->fEffectScale;
        }
    }
    else
    {
        KRLDoodadModel const* pDoodadModel = NULL;

        KG_PROCESS_SUCCESS(g_pRL->m_CurrentOptimizationOption.bHideDoodadEffect);

        pDoodadModel = g_pRL->m_TableList.GetDoodadModel(m_FrameData.m_pDoodad->m_dwRepresentID);
        if (pDoodadModel)
        {
            fScale = pDoodadModel->fEffectScale;
        }
    }

    pDoodadEffectModel = g_pRL->m_TableList.GetEffectModel(dwEffectID);
    KG_PROCESS_SUCCESS(pDoodadEffectModel == NULL);

    SFXParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pDoodadEffectModel->dwPlayType);
    SFXParam.szFilePath = pDoodadEffectModel->szSFXFilePath;
    SFXParam.szBoneName = pDoodadEffectModel->szBoneName;
    SFXParam.fSpeed = pDoodadEffectModel->fSpeed;
    SFXParam.fScale = fScale;

    KG_PROCESS_SUCCESS(SFXParam.szFilePath[0] == _T('\0'));

    hr = m_RenderData.LoadMDLSFX(dwEffectID, SFXParam);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLDoodad::UnloadDoodadEffect(DWORD dwEffectID)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_SUCCESS(!m_RenderData.HasLoadedMDLSFX(dwEffectID));

    hr = m_RenderData.UnloadMDLSFX(dwEffectID);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLDoodad::GetTerrainSlopeRotationLH(D3DXQUATERNION& qRotationLH)
{
    int nRetCode = false;
    int nResult = false;
    RL_TERRAIN_SLOPE nSlopeType = RL_TERRAIN_SLOPE_NONE;
    IKG3DScene* p3DScene = NULL;
    KRLNpcModel const* pModel = NULL;
    D3DXVECTOR3 vPosition;

    KG_PROCESS_ERROR(m_FrameData.m_pDoodad);

    ASSERT(m_FrameData.m_pDoodad->m_eKind == dkCorpse);

    pModel = g_pRL->m_TableList.GetNpcModel(m_dwRepresentID);
    KGLOG_PROCESS_ERROR(pModel);

    nSlopeType = (RL_TERRAIN_SLOPE)pModel->nTerrainSlope;

    p3DScene = m_RenderData.GetRLScene()->m_p3DScene;
    KGLOG_PROCESS_ERROR(p3DScene);

    GetPosition(vPosition);

    nRetCode = GetTerrainSlopeRotation(m_RenderData.GetRLScene()->m_p3DScene, vPosition, m_FrameData.m_Current.fFaceDirection, nSlopeType, qRotationLH);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

int KRLDoodad::Update(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;

    KG_USE_ARGUMENT(fTime);
	KG_USE_ARGUMENT(fTimeLast);
	KG_USE_ARGUMENT(dwGameLoop);

	if (bFrame)
    {
		m_FrameData.UpdateData();
		UpdateFade();

        hr = m_RenderData.Update();
        KGLOG_COM_PROCESS_ERROR(hr);

        KG_PROCESS_ERROR(m_FrameData.m_pDoodad);

        // 错开更新
        if (m_bOnFloor && (dwGameLoop % 0x1F) == (m_FrameData.m_pDoodad->m_dwID & 0x1F))
        {
            nRetCode = UpdatePosition();
            KGLOG_PROCESS_ERROR(nRetCode);
        }
    }

	nResult = true;
Exit0:
    return nResult;
}

int KRLDoodad::UpdatePosition()
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    KRLScene* pRLScene = NULL;
    IKG3DScene* p3DScene = NULL;
	D3DXVECTOR3 vPosition;

    pRLScene = m_RenderData.GetRLScene();
    KGLOG_PROCESS_ERROR(pRLScene);

    p3DScene = pRLScene->m_p3DScene;
    KGLOG_PROCESS_ERROR(p3DScene);

    hr = GameWorldPositionToScenePosition(p3DScene, vPosition, 
        (float)m_FrameData.m_Current.nX, (float)m_FrameData.m_Current.nY, (float)m_FrameData.m_Current.nZ, 
        m_bOnFloor && m_RenderData.IsVisible(), FALSE, FALSE);
    KGLOG_COM_PROCESS_ERROR(hr);

    SetPosition(vPosition);

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLDoodad::OnRotationLHChange(D3DXQUATERNION const& qRotationLH)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    D3DXQUATERNION qSlopeLH(qRotationLH);

    KG_PROCESS_SUCCESS(!m_RenderData.HasLoadedMDL());

    KG_PROCESS_ERROR(m_FrameData.m_pDoodad);

    if (m_FrameData.m_pDoodad->m_eKind == dkCorpse)
    {
        nRetCode = GetTerrainSlopeRotationLH(qSlopeLH);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    hr = m_RenderData.SetRotation(qSlopeLH);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLDoodad::OnPositionChange(D3DXVECTOR3 const& vPosition)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_RenderData.SetPosition(vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLDoodad::OnAlphaChange(float fAlpha)
{
	if (m_RenderData.HasLoadedMDL())
		m_RenderData.SetAlpha(fAlpha);
	return S_OK;
}

HRESULT KRLDoodad::OnGetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const
{
    if (m_RenderData.HasLoadedMDL())
        m_RenderData.GetBBox(vMin, vMax);
    return S_OK;
}

HRESULT KRLDoodad::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
    int nRetCode = false;
    HRESULT hrResult = E_FAIL;

	KG_PROCESS_SUCCESS(IsFadeOut());

    nRetCode = UpdateState(m_nState, TRUE);
    KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KRLDoodad::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
	KG_PROCESS_SUCCESS(IsFadeOut());

Exit1:
	return E_NOTIMPL;
}

HRESULT KRLDoodad::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
	KG_PROCESS_SUCCESS(IsFadeOut());

Exit1:
    return S_OK;
}
