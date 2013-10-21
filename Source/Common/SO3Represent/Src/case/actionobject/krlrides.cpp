#include "stdafx.h"
#include "./krlrides.h"
#include "./krlcharacter.h"
#include "../render/krlresource.h"
#include "../render/kmodel.h"
#include "../gameworld/krlanimationtable.h"
#include "../gameworld/ktabletransform.h"
#include "../../common/ksmallobjectpool.h"
#include "../../SO3Represent.h"
#include "../gameworld/krlcharacterframedata.h"

#pragma warning(push)
#pragma warning(disable : 4355)

namespace 
{
    int const s_nRidesSkillBuffSFXs[SKILL_BUFF_PART_COUNT][2] =
    {   
        // Left                              // Right
        { RIDES_SFX_SKILL_BUFF_TOP_LH,       RIDES_SFX_SKILL_BUFF_TOP_RH },       // SKILL_BUFF_PART_TOP
        { RIDES_SFX_SKILL_BUFF_MIDDLE_LH,    RIDES_SFX_SKILL_BUFF_MIDDLE_RH },    // SKILL_BUFF_PART_MIDDLE
        { RIDES_SFX_SKILL_BUFF_BOTTOM_LH,    RIDES_SFX_SKILL_BUFF_BOTTOM_RH }     // SKILL_BUFF_PART_BOTTOM
    };

	KSmallObjectPool	g_Allocator(_T("KRLRides"), sizeof(KRLRides));
}

void* KRLRides::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KRLRides::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KRLRides::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KRLRides::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

KRLRides::KRLRides()
: m_dwRepresentID(0)
, m_dwAnimationID(0)
, m_dwIdleAnimationID(0)
, m_dwSlopeType(0)
, m_pRLCharacter(NULL)
{
    m_RenderData.Init(this, MODEL_RIDES_EQUIP_COUNT, MODEL_RIDES_SOCKET_COUNT, RIDES_SFX_COUNT);
}

KRLRides::~KRLRides()
{
    Reset();
}

int KRLRides::Init(KRLScene* pRLScene, BOOL bCurrentPlayer)
{
    int nResult = false;
    HRESULT hr = E_FAIL;

    hr = m_RenderData.SetMDLSelectable(!bCurrentPlayer);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.AttachScene(pRLScene);
    KGLOG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    return nResult;
}

void KRLRides::Reset()
{
    m_dwRepresentID = 0;
    m_dwIdleAnimationID = 0;
    m_dwSlopeType = 0;
    m_RenderData.Reset();
    m_RenderData.DetachScene();
}

HRESULT KRLRides::DisplayDataChanged()
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    int nBindCharacter = false;

    if (m_RenderData.HasLoadedMDL())
    {
        hr = m_RenderData.Reset();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hr = LoadMDL();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = LoadEquipModel();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = LoadDefaultSocket();
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = BindCharacter();
    KGLOG_COM_PROCESS_ERROR(hr);

    nBindCharacter = true;

    nRetCode = InitPlayAnimation();
    KGLOG_PROCESS_ERROR(nRetCode);
    
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nBindCharacter)
        {
            hr = UnbindCharacter();
            KGLOG_COM_CHECK_ERROR(hr);
			nBindCharacter = false;
        }

        hr = m_RenderData.Reset();
        KGLOG_COM_CHECK_ERROR(hr);
    }
    return hrResult;
}

HRESULT KRLRides::PlayRideAnimation(DWORD dwAnimationID, int nOffsetTime, enuAnimationControllerPriority nTrackType)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLSFXParam SFXParam;
    KRLMonoAnimationParam MonoAnimationParam;
    KRidesAnimationModel const* pAnimationModel = NULL;

    KG_PROCESS_SUCCESS(!dwAnimationID);
    KG_PROCESS_SUCCESS(!m_RenderData.HasLoadedMDL());

    pAnimationModel = g_pRL->m_TableList.GetRidesAnimationModel(m_dwRepresentID, dwAnimationID);
    KG_PROCESS_SUCCESS(pAnimationModel == NULL);

    MonoAnimationParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pAnimationModel->dwAnimationType);
    MonoAnimationParam.nTrackType = nTrackType;
    MonoAnimationParam.fSpeed = pAnimationModel->fAnimationSpeed;
    MonoAnimationParam.nOffsetTime = nOffsetTime;
    MonoAnimationParam.dwTweenTime = g_pRL->m_TableList.GetRidesAnimationFusion(m_dwRepresentID, m_dwAnimationID, dwAnimationID);
    MonoAnimationParam.szFilePath = pAnimationModel->szAnimationFile;
    MonoAnimationParam.fStartTime = g_pRL->m_fTime;

    hr = m_RenderData.PlayMonoAnimation(MonoAnimationParam);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_dwAnimationID = dwAnimationID;

    if (pAnimationModel->szSFXFile[0] != _T('\0'))
    {
        SFXParam.dwPlayType = AnimationPlayTypeRLTo3D((RL_ANIMATION_PLAY_TYPE)pAnimationModel->dwSFXType);
        SFXParam.fSpeed = pAnimationModel->fSFXSpeed;
        SFXParam.fScale = pAnimationModel->fSFXScale;
        SFXParam.szFilePath = pAnimationModel->szSFXFile;
        SFXParam.szBoneName = pAnimationModel->szBoneName;

        hr = m_RenderData.LoadMDLSFX(RIDES_SFX_MAIN, SFXParam);
        KGLOG_COM_CHECK_ERROR(hr);
    }

Exit1:
    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLRides::UpdateAnimationSpeed(float fSpeed)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_RenderData.SetMonoAnimationSpeed(fSpeed);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

int KRLRides::InitPlayAnimation()
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    int nOffsetTime = 0;
    DWORD dwAnimationID = 0;
    KRLAdjustAnimationParam Param;

    ASSERT(m_pRLCharacter);

    nRetCode = m_pRLCharacter->FillRLAdjustAnimationParam(&Param, m_pRLCharacter->m_bSheath);
    KGLOG_PROCESS_ERROR(nRetCode);

    dwAnimationID = GetAdjustAnimationIndexID(m_pRLCharacter->m_pDownBodyCurrentAnimationTrack->dwAnimationID, &Param);

    nOffsetTime = GetAnimationPlayTime(m_pRLCharacter->m_AnimationStatus.SplitStatus.DownLowTrackParam.fStartTime);

    hr = PlayRideAnimation(dwAnimationID, nOffsetTime, ANICTL_PRIMARY);
    KGLOG_COM_PROCESS_ERROR(hr);

    nResult = true;
Exit0:
    return nResult;
}

HRESULT KRLRides::LoadMDL()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRidesModel const* pRidesModel = NULL;
    float fCharacterScale = 1.0f;
    int nLoadMDL = false;

    pRidesModel = g_pRL->m_TableList.GetRidesModel(m_dwRepresentID);
    KGLOG_PROCESS_ERROR(pRidesModel);
    KGLOG_PROCESS_ERROR(pRidesModel->szMDLFilePath[0] != _T('\0'));

	KGLOG_PROCESS_ERROR(m_pRLCharacter);
	
    fCharacterScale = m_pRLCharacter->m_EquipResource.MDL.fMDLScale;

    hr = m_RenderData.SetMDLVisible(TRUE);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_RenderData.LoadMDL(pRidesModel->szMDLFilePath, pRidesModel->fModelScale * fCharacterScale, 
        pRidesModel->nColorChannelTable, pRidesModel->nColorChannel); // TODO: 马还没装备偏色
    KGLOG_COM_PROCESS_ERROR(hr);

    nLoadMDL = true;

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        hr = m_RenderData.UnloadMDL();
        KGLOG_COM_CHECK_ERROR(hr);
        
        nLoadMDL = false;
    }
    return hrResult;
}

HRESULT KRLRides::LoadEquipModel()
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLRidesEquipResource EquipParam[MODEL_RIDES_EQUIP_COUNT];

    KG_ASSERT_EXIT(m_pRLCharacter);
    KG_ASSERT_EXIT(m_pRLCharacter->m_pFrameData);
    KGLOG_PROCESS_ERROR(m_RenderData.HasLoadedMDL());

    nRetCode = GetRidesEquipModel(m_dwRepresentID, m_pRLCharacter->m_EquipResource, EquipParam);
    KGLOG_PROCESS_ERROR(nRetCode);

    for (int nIndex = 0; nIndex != MODEL_RIDES_EQUIP_COUNT; ++nIndex)
    {
        KRLRidesEquipResource& part = EquipParam[nIndex];

        if (m_RenderData.HasLoadedModel(nIndex))
        {
            hr = m_RenderData.UnloadModel(nIndex);
            KGLOG_COM_CHECK_ERROR(hr);
        }

        if (m_pRLCharacter->m_pFrameData->m_Current.bOnRide && part.szMeshFilePath[0] != _T('\0'))
        {
            hr = m_RenderData.LoadModel(nIndex, part.szMeshFilePath, part.szMtlFilePath, part.fMeshScale, part.nGlobalColorChannel, part.nColorChannel);
            KGLOG_COM_PROCESS_ERROR(hr);
        }
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLRides::LoadDefaultSocket()
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    KRLSocketResource SocketParam[MODEL_RIDES_SOCKET_COUNT];

    KG_PROCESS_ERROR(m_RenderData.HasLoadedMDL());

    nRetCode = GetRidesSocketModel(m_pRLCharacter->m_EquipResource, SocketParam);
    KG_PROCESS_ERROR(nRetCode);

    for (int nIndex = 0; nIndex != MODEL_RIDES_SOCKET_COUNT; ++nIndex)
    {
        hr = LoadSocket(nIndex, SocketParam[nIndex]);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLRides::LoadSocket(int nIndex, KRLSocketResource const SocketParam)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KG_ASSERT_EXIT(m_pRLCharacter);
    KG_ASSERT_EXIT(m_pRLCharacter->m_pFrameData);

    if (m_pRLCharacter->m_pFrameData->m_Current.bOnRide && SocketParam.szMeshFilePath[0] != _T('\0'))
    {   
        hr = m_RenderData.LoadSocket(nIndex, SocketParam.szSocketName, SocketParam.szMeshFilePath, SocketParam.szMtlFilePath, SocketParam.fMeshScale, SocketParam.nGlobalColorChannel, SocketParam.nColorChannel);
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLRides::BindCharacter()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    
    KG_ASSERT_EXIT(m_pRLCharacter);

    hr = m_pRLCharacter->m_RenderData.BindTo(&m_RenderData, _T("S_Hs"));
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLRides::UnbindCharacter()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    
    KG_ASSERT_EXIT(m_pRLCharacter);

    hr = m_pRLCharacter->m_RenderData.UnbindFrom(&m_RenderData);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLRides::OnPositionChange(D3DXVECTOR3 const& vPosition)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = m_RenderData.SetPosition(vPosition);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KRLRides::OnRotationLHChange(D3DXQUATERNION const& qRotationLH)
{
    if (m_RenderData.HasLoadedMDL())
        m_RenderData.SetRotation(qRotationLH);
	return S_OK;
}

HRESULT KRLRides::OnAlphaChange(float fAlpha)
{
    if (m_RenderData.HasLoadedMDL())
        m_RenderData.SetAlpha(fAlpha);
    return S_OK;
}

HRESULT KRLRides::OnGetBBox(D3DXVECTOR3& vMin, D3DXVECTOR3& vMax) const
{
    if (m_RenderData.HasLoadedMDL())
        m_RenderData.GetBBox(vMin, vMax);
    return S_OK;
}

void KRLRides::UpdateRemote(double fTime, double fTimeLast, DWORD dwGameLoop, BOOL bFrame)
{
	if (bFrame)
    {
		UpdateFade();
    }
}

HRESULT KRLRides::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
	return S_OK;
}

HRESULT KRLRides::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
    return S_OK;
}

HRESULT KRLRides::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
    return S_OK;
}

void KRLRides::LoadSkillBuff(size_t nIndex, KSkillBuffParam SkillBuff)
{
    HRESULT hr = E_FAIL;

    if (SkillBuff.szBoneNameLeft[0] == _T('\0') && SkillBuff.szBoneNameRight[0] == _T('\0'))
    {  
        // 绑到原点
        hr = m_RenderData.LoadMDLSFX(s_nRidesSkillBuffSFXs[nIndex][0], SkillBuff.SFXParam);
        KGLOG_COM_CHECK_ERROR(hr);
    }
    else
    {
        if (SkillBuff.szBoneNameLeft[0] != _T('\0'))
        { 
            // Left
            SkillBuff.SFXParam.szBoneName = SkillBuff.szBoneNameLeft;

            hr = m_RenderData.LoadMDLSFX(s_nRidesSkillBuffSFXs[nIndex][0], SkillBuff.SFXParam);
            KGLOG_COM_CHECK_ERROR(hr);
        }

        if (SkillBuff.szBoneNameRight[0] != _T('\0'))
        {
            // Right
            SkillBuff.SFXParam.szBoneName = SkillBuff.szBoneNameRight;

            hr = m_RenderData.LoadMDLSFX(s_nRidesSkillBuffSFXs[nIndex][1], SkillBuff.SFXParam);
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
}

void KRLRides::UnloadSkillBuff(size_t nIndex)
{
    HRESULT hr = E_FAIL;

    if (m_RenderData.HasLoadedMDLSFX(s_nRidesSkillBuffSFXs[nIndex][0]))
    {
        hr = m_RenderData.UnloadMDLSFX(s_nRidesSkillBuffSFXs[nIndex][0]);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    if (m_RenderData.HasLoadedMDLSFX(s_nRidesSkillBuffSFXs[nIndex][1]))
    {
        hr = m_RenderData.UnloadMDLSFX(s_nRidesSkillBuffSFXs[nIndex][1]);
        KGLOG_COM_CHECK_ERROR(hr);
    }
}

#pragma warning(pop)
