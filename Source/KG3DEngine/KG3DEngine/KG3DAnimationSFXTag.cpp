#include "StdAfx.h"
#include "KG3DAnimationSFXTag.h"
#include "kg3dSfx.h"
#include "KG3DTagSFXAnimationController.h"
#include "kg3dmodeltable.h"
#include "kg3denginemanager.h"
#include "KG3DScene.h"
#include "kg3dcliptable.h"
#include "KG3DAnimationController.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool s_bCopyMode = false;
extern BOOL g_bEnableAddEntity;
static std::set<KG3DModel*> s_SelfDestructSfxList;

void ClearSelfDestructSfxList()
{
	std::set<KG3DModel*>::iterator it = s_SelfDestructSfxList.begin();
	std::set<KG3DModel*>::iterator itEnd = s_SelfDestructSfxList.end();

	while (it != itEnd)
	{
		_ASSERTE(*it);
		(*it)->Release();
		it++;
	}
}

static void RemoveSelfDestruct(KG3DModel *pSfx)
{
	std::set<KG3DModel*>::iterator itErase = s_SelfDestructSfxList.find(pSfx);
	if (itErase != s_SelfDestructSfxList.end())
	{
		s_SelfDestructSfxList.erase(itErase);
	}
}

static void AddSelfDestruct(KG3DModel *pSfx)
{
	if (pSfx)
	{
		pSfx->m_bSelfDestructFramemoveFlag = TRUE;
		s_SelfDestructSfxList.insert(pSfx);
	}
}

void SelfDestructedFrameMove()
{

	std::set<KG3DModel*> TempSelfDestructSfxList = s_SelfDestructSfxList;
	std::set<KG3DModel*>::iterator it = TempSelfDestructSfxList.begin();
	std::set<KG3DModel*>::iterator itEnd = TempSelfDestructSfxList.end();

	while (it != itEnd)
	{
		_ASSERTE(*it);
		if ((*it)->m_bSelfDestructFramemoveFlag)
		{
			(*it)->FrameMove();
		}
		it++;
	}

	//这里需要这样写， 因为Framemove会有可能导致sfx的释放(*it)可以能在Framemove之后就失效了
	it = s_SelfDestructSfxList.begin();
	itEnd = s_SelfDestructSfxList.end();
	while (it != itEnd)
	{
		_ASSERTE(*it);
		(*it)->m_bSelfDestructFramemoveFlag = TRUE;
		it++;
	}
}


KG3DAnimationSFXTag::SetRunTimeFunc KG3DAnimationSFXTag::m_SetRunTimeFunc[] = 
{
    KG3DAnimationSFXTag::Helper_SetRunState,
    KG3DAnimationSFXTag::Helper_SetModel,
    KG3DAnimationSFXTag::Helper_SetFinishState,
    KG3DAnimationSFXTag::Helper_SetMotionFinishState
};

DWORD KG3DAnimationSFXTag::m_SetRunTimeFuncIndex[] = 
{
    KG3DAnimationSFXTag::RTIM_RUNSTATE,
    KG3DAnimationSFXTag::RTIM_MODEL,
    KG3DAnimationSFXTag::RTIM_FINISHSTATE,
    KG3DAnimationSFXTag::RTIM_MOTIONFINISHSTATE,
};

KG3DAnimationSFXTag::KG3DAnimationSFXTag()
{
    m_fParentAnimationSpeed = 1.0f;
    m_EventHandler.SetUser(this);
    m_pParentModel = NULL; 
    m_pBindInfo = new KG3DSFXSuede;
}

KG3DAnimationSFXTag::~KG3DAnimationSFXTag()
{
	m_KeyFrames.clear();
	m_KeyedFrame.clear();
	SAFE_DELETE(m_pBindInfo);
}

UINT KG3DAnimationSFXTag::GetNumKeyFrame(UINT uFrame)
{
    UINT uReturn = 0;
    std::map<UINT, std::vector<Data>>::iterator itFind = m_KeyFrames.find(uFrame);
    KG_PROCESS_ERROR(itFind != m_KeyFrames.end());
    uReturn = static_cast<UINT>(itFind->second.size());
Exit0:
    return uReturn;
}

UINT KG3DAnimationSFXTag::ModifyTag(UINT uFrame, void* Data)
{
    UINT uReturn = KEYFRAME_PEND;
    SFXKeyFrameInfoVersion1* pInput = static_cast<SFXKeyFrameInfoVersion1*>(Data);
    KG_PROCESS_ERROR(pInput);
    UINT uFrameIndex = 0;
    UINT uKey = 0;
	UINT uID = 0;
    HRESULT hr = m_IDMgr.GetFrame(pInput->uID, uFrameIndex, uKey);
    if (SUCCEEDED(hr))
    {
        //本帧上已经有了关键帧, 并且传入的关键帧的index是有效的, 处理为修改
        memcpy(&(m_KeyFrames[uFrameIndex][uKey].KeyFrame), pInput, sizeof(SFXKeyFrameInfoVersion1));
		uID = pInput->uID;
    }
    else
    {
        //增加一个关键帧
        UINT uIndex = static_cast<UINT>(m_KeyFrames[uFrame].size());
        m_KeyFrames[uFrame].resize(uIndex + 1);
        uID = m_IDMgr.InsertKey(uFrame, uIndex);
		SFXKeyFrameInfoVersion1 &KeyDest = m_KeyFrames[uFrame][uIndex].KeyFrame;
        memcpy(&KeyDest, pInput, sizeof(SFXKeyFrameInfoVersion1));
		KeyDest.uID = uID;
		pInput->uID = uID;
		
		if (find(m_KeyedFrame.begin(), m_KeyedFrame.end(), uFrame) == m_KeyedFrame.end())
        {
            m_KeyedFrame.push_back(uFrame);
            sort(m_KeyedFrame.begin(), m_KeyedFrame.end());
        }
        if (m_KeyFrames[uFrame][uIndex].KeyFrame.uBindFileIndex == SFXKeyFrameInfo::TAG_INVALIDATE_VALUE)
        {
            //处理没有新加入还没有绑定模型的信息时候
			m_KeyFrames[uFrame][uIndex].KeyFrame.uBindFileIndex = m_pBindInfo->GetModelCount();
			IKG3DModel* pPatternModel = KG3DAnimationSFXTag::GetPatternModel();
			if (pPatternModel)
			{
				pPatternModel->BindToBone(m_pParentModel, "");
				pPatternModel->AddRef();
				m_pBindInfo->AddModel(static_cast<KG3DModel*>(pPatternModel));
			}           
        }
		else if (s_bCopyMode)
		{
			//如果本来是有uBindFileIndex的需要复制一个
			m_KeyFrames[uFrame][uIndex].KeyFrame.uBindFileIndex = m_pBindInfo->GetModelCount();
			m_pBindInfo->CopyBindInfo(pInput->uBindFileIndex);
		}
    }
	uReturn = uID;
Exit0:
    return uReturn;
}

UINT KG3DAnimationSFXTag::RemoveTag(UINT uFrameIndex, UINT uIndex)
{
    UINT uReturn = uFrameIndex;
    std::vector<Data>::iterator itErase;
    KG_PROCESS_ERROR(m_KeyFrames.count(uFrameIndex));
    KG_PROCESS_ERROR(uIndex < m_KeyFrames[uFrameIndex].size());
    itErase = m_KeyFrames[uFrameIndex].begin();
    advance(itErase, uIndex);
    m_IDMgr.RemoveKey(itErase->KeyFrame.uID);
    m_KeyFrames[uFrameIndex].erase(itErase);
    if (!m_KeyFrames[uFrameIndex].size())
    {
        std::map<UINT, std::vector<Data>>::iterator itKeyErase = m_KeyFrames.find(uFrameIndex);
        m_KeyFrames.erase(itKeyErase);
        std::vector<UINT>::iterator itKeyedErase = find(m_KeyedFrame.begin(), m_KeyedFrame.end(), uFrameIndex);
        ASSERT(itKeyedErase != m_KeyedFrame.end());
        m_KeyedFrame.erase(itKeyedErase);
    }
    uReturn = static_cast<UINT>(IEIR_SUCCESS);
Exit0:
    return uReturn;
}

UINT KG3DAnimationSFXTag::MoveTag(UINT uFrameFrom, 
                                  UINT uKeyIndex, 
                                  UINT uFrameTo)
{
    UINT uReturn = static_cast<UINT>(KEYFRAME_PEND);
    uReturn = CopyTag(uFrameFrom, uKeyIndex, uFrameTo);
    RemoveTag(uFrameFrom, uKeyIndex);
    return uReturn;
}

UINT KG3DAnimationSFXTag::CopyTag(UINT uFrameSrc, 
                                  UINT uKeyIndex, 
                                  UINT uFrameDest)
{
    SFXKeyFrameInfoVersion1 FrameSrc;
	UINT uReturn = KEYFRAME_PEND;
	KG_PROCESS_ERROR(m_KeyFrames.count(uFrameSrc));
    KG_PROCESS_ERROR(m_KeyFrames[uFrameSrc].size() > uKeyIndex);
	s_bCopyMode = true;
    memcpy(&FrameSrc, &m_KeyFrames[uFrameSrc][uKeyIndex].KeyFrame, sizeof(SFXKeyFrameInfoVersion1));
    FrameSrc.uID = KEYFRAME_PEND;
    uReturn = ModifyTag(uFrameDest, &FrameSrc);
	s_bCopyMode = false;
Exit0:
    return uReturn;
}

std::vector<KG3DAnimationSFXTag::Data>* KG3DAnimationSFXTag::GetKeyFrameInfo(UINT uFrame)
{
    std::vector<Data>* pReturn = NULL;
    KeyFrameData::iterator itFind = m_KeyFrames.find(uFrame);
    KG_PROCESS_ERROR(itFind != m_KeyFrames.end());
    pReturn = &(itFind->second);
Exit0:
    return pReturn;
}


//检查当前激活关键帧对其他帧的影响
void KG3DAnimationSFXTag::OnSFXActived(UINT uActiveIndex)
{
	if (uActiveIndex == KEYFRAME_PEND)
		return;
	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
    {
        if (uActiveIndex == m_KeyedFrame[i])
            continue;
        std::vector<Data>* pCurKeyFrame = GetKeyFrameInfo(m_KeyedFrame[i]);
        ASSERT(pCurKeyFrame);

		for (size_t j = 0; j < pCurKeyFrame->size(); j++)
        {
            Data& CurrentSubKey = (*pCurKeyFrame)[j];
			if (CurrentSubKey.KeyFrame.bInterruptable &&
                CurrentSubKey.RunTime.RunState == SFX_RS_ACTIVE)
            {
                //如果其他的正在运行状态的特效是可以中断的， 中断掉这些特效
				if (CurrentSubKey.RunTime.pModel)
				{
					CurrentSubKey.RunTime.pModel->UnBindFromOther();
					CurrentSubKey.RunTime.pModel->PlayAnimation(SFX_AP_ONE_TIME, 1.0f, 0, NULL, NULL, ANICTL_PRIMARY);
					RemoveSelfDestruct(CurrentSubKey.RunTime.pModel);
					SAFE_RELEASE(CurrentSubKey.RunTime.pModel);
				}
				CurrentSubKey.RunTime.RunState = SFX_RS_FININSH;
				CurrentSubKey.RunTime.LifeRangeType = SFX_MFS_DELETE;
            }
        }
    }
}

HRESULT KG3DAnimationSFXTag::Helper_Apply(KG3DAnimationController* pController, 
										  UINT uStartFrame, 
										  UINT uEndFrame, 
										  D3DXVECTOR3 *pvPos)
{
	HRESULT hr = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    std::map<UINT, std::vector<Data>>::iterator i;
    std::map<UINT, std::vector<Data>>::iterator iEnd;
	KG_PROCESS_ERROR(m_pBindInfo);
    KG_PROCESS_ERROR(m_pParentModel);
    KG_PROCESS_SUCCESS(!m_KeyedFrame.size());
    
    if (uStartFrame > uEndFrame)
    {
        //在动作循环播放的时候会出现这种情况
        Helper_Apply(pController, uStartFrame, max(m_KeyedFrame[m_KeyedFrame.size() - 1], uStartFrame), pvPos);
        Helper_Apply(pController, min(uEndFrame, m_KeyedFrame[0]), uEndFrame, pvPos);
    }
    else
    {
        KG_PROCESS_SUCCESS(uStartFrame > m_KeyedFrame[m_KeyedFrame.size() - 1] ||
            uEndFrame < m_KeyedFrame[0]);
        UINT uStartIndex = 0;
        UINT uEndIndex = 0;
        hr = FindKeyFrame(uStartFrame, FFT_GREATEREQUAL, m_KeyedFrame, uStartIndex);
        //if failed(hr), no key frame found, exit
        KG_PROCESS_SUCCESS(hr);
        hr = FindKeyFrame(uEndFrame, FFT_LESSEQUAL, m_KeyedFrame, uEndIndex);
        KG_PROCESS_SUCCESS(hr);
        for (UINT uIndex = uStartIndex; uIndex <= uEndIndex; uIndex++)
        {
            std::vector<Data>* pCurKeyFrame = GetKeyFrameInfo(m_KeyedFrame[uIndex]);
            ASSERT(pCurKeyFrame);
            if (!pCurKeyFrame)
                continue;
            std::vector<Data>& CurKeyFrame = *pCurKeyFrame;
            UINT uActiveSFX = KEYFRAME_PEND;
            for (size_t i = 0; i < CurKeyFrame.size(); i++)
            {
                switch(CurKeyFrame[i].RunTime.RunState)
                {
                case SFX_RS_NONE:
                    {
                        SFXKeyFrameInfoVersion1& CurKeyFrameInfo = CurKeyFrame[i].KeyFrame;
						RunTimeInfo& CurRunTimeInfo = CurKeyFrame[i].RunTime;
                        CurRunTimeInfo.FinishEventType = CurKeyFrameInfo.FinishState;
                        KG3DSFX* pModel = dynamic_cast<KG3DSFX*>(m_pBindInfo->GetModel(CurKeyFrameInfo.uBindFileIndex));
                        KG_PROCESS_ERROR(pModel);
                        if (CurKeyFrameInfo.bNotify)
                        {
							IKG3DAnimationController* pController = m_pParentModel->GetAnimationController(ANICTL_PRIMARY);
							ASSERT(pController);
                            IKG3DAnimationEventHandler* pEventHandler = pController->GetEventHandler();
                            if (pEventHandler)
                            {
                                pEventHandler->OnTagPlayingSFX(pController, pModel, &(CurKeyFrameInfo.CallBackExtraInfo));
                            }
                        }
						pModel->SetTagUsingInfo();
                        CurRunTimeInfo.pModel = pModel;
                        pModel->AddRef();
						float fPlaySpeed = CurKeyFrameInfo.fSpeed;
                        if (CurKeyFrameInfo.bAdaptSpeedToMotion)
                        {
                            fPlaySpeed *= m_fParentAnimationSpeed;
                        }
                        hr = pModel->PlayAnimation(ENU_ANIMATIONPLAY_ONCE, NULL, fPlaySpeed, 0, NULL, NULL, ANICTL_PRIMARY);
                        KGLOG_COM_PROCESS_ERROR(hr);
                        
						KG3DTagSFXAnimationController* pController = static_cast<KG3DTagSFXAnimationController*>(pModel->GetAnimationController(ANICTL_PRIMARY));
                        if (pController)
                        {
                            if (CurKeyFrameInfo.bSetWorldPosition)
							{
								//pController->RegistereEventHandler(GetSFXTagSceneWorldPosEventHandeler());
							}
							else
							{
								pController->RegistereEventHandler(&m_EventHandler);
							}
							pController->m_dwSFXID = CurKeyFrameInfo.uID;
                        }
						//BOOL bSave = pModel->m_bInMultiThreadLoadList;
						//pModel->m_bInMultiThreadLoadList = FALSE;
						
						
						m_pBindInfo->ApplyBind(m_pParentModel, 
							CurKeyFrameInfo.uBindFileIndex, 
							CurKeyFrameInfo.bSetWorldPosition);
						if (CurKeyFrameInfo.bSetWorldPosition)
						{
							/*KG3DScene *pScene = g_cGraphicsTool.GetCurScene();
							KG_PROCESS_ERROR(pScene);
							CurRunTimeInfo.pModel = NULL;
							BOOL bSave = g_bEnableAddEntity;
							g_bEnableAddEntity = TRUE;
							pScene->AddRenderEntity(pModel, 0, FALSE, FALSE);
							g_bEnableAddEntity = bSave;*/
						}
						//pModel->m_bInMultiThreadLoadList = bSave;
						CurRunTimeInfo.RunState = SFX_RS_ACTIVE;
                        //这里如果有多个特效被激活， 那么只有最后一个才会被保持一定会播放，其他的则取决于他们的可否中断属性
                        uActiveSFX = m_KeyedFrame[uIndex];
						/////////////////////////////////////////add by huangjinshou 2008-03-6  生成脚印/////////////////////////////////
						pModel->Playing();
						/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    }
                    break;
                case SFX_RS_ACTIVE:	
					{
						SFXKeyFrameInfoVersion1& CurKeyFrameInfo = CurKeyFrame[i].KeyFrame;
						RunTimeInfo& CurRunTimeInfo = CurKeyFrame[i].RunTime;
						CurRunTimeInfo.FinishEventType = CurKeyFrameInfo.FinishState;
						KG3DModel* pModel = static_cast<KG3DModel*>(m_pBindInfo->GetModel(CurKeyFrameInfo.uBindFileIndex));
						if(pModel)
							pModel->Playing();//add by huangjinshou 2008-03-6  生成脚印
					}
						break;
                case SFX_RS_FININSH:
                    break;
                }
            }
			OnSFXActived(uActiveSFX);
        }
    }
Exit1:
    hRetCode = S_OK;
Exit0:
    return hRetCode;
}

HRESULT KG3DAnimationSFXTag::CheckValidate()
{
    HRESULT hr = E_FAIL;

    hr = S_OK;
    return hr;
}

void KG3DAnimationSFXTag::OnMotionChange()
{
    for (size_t i = 0; i < m_KeyedFrame.size(); i++)
    {
        std::vector<Data>* pKeyFrame = GetKeyFrameInfo(m_KeyedFrame[i]);
        if (!pKeyFrame)
            continue;
        for (size_t j = 0; j < pKeyFrame->size(); j++)
        {
            Data& CurrentKeyFrame = (*pKeyFrame)[j];
            if (SFX_RS_ACTIVE == CurrentKeyFrame.RunTime.RunState)
            {
                //对于标示为动作结束时候结束的SFX
                switch(CurrentKeyFrame.KeyFrame.MotionFinishEvent)
                {
                case SFX_MFS_DELETE:
                    {
                        //如果是清除标志,直接清除掉
                        CurrentKeyFrame.RunTime.RunState = SFX_RS_FININSH;
                        if (CurrentKeyFrame.RunTime.pModel)
						{
							CurrentKeyFrame.RunTime.pModel->UnBindFromOther();
						}
						SAFE_RELEASE(CurrentKeyFrame.RunTime.pModel);
                        CurrentKeyFrame.RunTime.LifeRangeType = SFX_MFS_DELETE;
                    }
                    break;
                case SFX_MFS_KEEP:
                    {
                        //如果是保持,强制把所有的特效类型转化为单次,并且在完成之后释放
                        CurrentKeyFrame.RunTime.FinishEventType = SFX_FS_DELETE;
                        if (!CurrentKeyFrame.RunTime.pModel)
                            continue;
						IKG3DAnimationController* pController = CurrentKeyFrame.RunTime.pModel->GetAnimationController(ANICTL_PRIMARY);
                        if (!pController)
                            continue;
						pController->RegistereEventHandler(GetSFXTagMotionSwitchEventHandeler());
						AddSelfDestruct(CurrentKeyFrame.RunTime.pModel);
						CurrentKeyFrame.RunTime.pModel = NULL;
						CurrentKeyFrame.RunTime.RunState = SFX_RS_ACTIVE;
						
                    }
                    break;
                default:
                    assert(0);
                }
            }
        }
    }
}

void KG3DAnimationSFXTag::SetAnimationSpeed(float fSpeed)
{
    m_fParentAnimationSpeed = fSpeed;
}

//重置所有的标签运行状态
void KG3DAnimationSFXTag::Reset()
{
	KeyFrameData::iterator it = m_KeyFrames.begin();
    while (it != m_KeyFrames.end())
    {
        for (size_t i = 0; i < it->second.size(); i++)
        {
            Data& SubKeyData = (it->second)[i];
			SubKeyData.RunTime.RunState = SFX_RS_NONE;
			if (SubKeyData.RunTime.pModel)
			{
				IKG3DAnimationController *pController = SubKeyData.RunTime.pModel->GetAnimationController(ANICTL_PRIMARY);
				pController->UnregisterEventHandler(&m_EventHandler);
			}
			if (SubKeyData.RunTime.pModel)
			{
				SubKeyData.RunTime.pModel->UnBindFromOther();
				if(SubKeyData.KeyFrame.MotionFinishEvent == SFX_MFS_KEEP)
				{
					RemoveSelfDestruct(SubKeyData.RunTime.pModel);
				}
			}
			SAFE_RELEASE(SubKeyData.RunTime.pModel);
        }
        it++;
    }
    for (DWORD i = 0; i < m_pBindInfo->GetModelCount(); i++)
    {
        m_pBindInfo->UnBindFromOther(i);
    }
	m_pBindInfo->Reset();
}

HRESULT KG3DAnimationSFXTag::Clear()
{
	return E_NOTIMPL;
}

const KG3DAnimationSFXTag::RunTimeInfo* KG3DAnimationSFXTag::GetSFXRuntimeState(UINT uFrameIndex, UINT uKeyIndex)
{
    const RunTimeInfo* pReturn = NULL;
    std::vector<Data>* pKeyFrame = GetKeyFrameInfo(uFrameIndex);
    KG_PROCESS_ERROR(pKeyFrame);
    KG_PROCESS_ERROR(pKeyFrame->size() > uKeyIndex);
    pReturn = &((*pKeyFrame)[uKeyIndex].RunTime);
Exit0:
    return pReturn;
}

void KG3DAnimationSFXTag::SetSFXRuntimeState(UINT uID,  DWORD dwMask, const RunTimeInfo& Src)
{
    UINT uFrameIndex = 0;
    UINT uKeyIndex = 0;
    HRESULT hr = m_IDMgr.GetFrame(uID, uFrameIndex, uKeyIndex);
    KG_COM_PROCESS_ERROR(hr);

    std::vector<Data>* pKeyFrame = GetKeyFrameInfo(uFrameIndex);
    KG_PROCESS_ERROR(pKeyFrame);
    KG_PROCESS_ERROR(pKeyFrame->size() > uKeyIndex);
    for (UINT i = 0; i < RTIM_COUNT; i++)
    {
        if (dwMask & m_SetRunTimeFuncIndex[i])
        {
            m_SetRunTimeFunc[i]((*pKeyFrame)[uKeyIndex].RunTime, Src);
        }
    }
Exit0:
    ;
}

void KG3DAnimationSFXTag::Helper_SetRunState(RunTimeInfo& Dest, const RunTimeInfo& Src)
{
    Dest.RunState = Src.RunState;
}

void KG3DAnimationSFXTag::Helper_SetModel(RunTimeInfo& Dest, const RunTimeInfo& Src)
{
    Dest.pModel = Src.pModel;
}
void KG3DAnimationSFXTag::Helper_SetFinishState(RunTimeInfo& Dest, const RunTimeInfo& Src)
{
    Dest.FinishEventType = Src.FinishEventType;
}

void KG3DAnimationSFXTag::Helper_SetMotionFinishState(RunTimeInfo& Dest, const RunTimeInfo& Src)
{
    Dest.LifeRangeType = Src.LifeRangeType;
}

KG3DAnimationSFXTag::RunTimeInfo::RunTimeInfo()
{
    RunState        = SFX_RS_NONE;
    pModel          = NULL;
    FinishEventType = SFX_FS_LOOP;
    LifeRangeType   = SFX_MFS_KEEP;
}
void KG3DAnimationSFXTag::SetParentModel(IKG3DModel* pModel)
{
    m_pParentModel = static_cast<KG3DModel*>(pModel);
}

BOOL KG3DAnimationSFXTag::CheckValidation()
{
    return TRUE;
}

HRESULT KG3DAnimationSFXTag::GetTagIDByFrameIndex(UINT uFrame, UINT uIndex, UINT *pID)
{
    return m_IDMgr.GetID(uFrame, uIndex, *pID);
}

HRESULT KG3DAnimationSFXTag::GetTagFrameIndexByID(UINT uId, UINT *pFrame, UINT *pIndex)
{
    return m_IDMgr.GetFrame(uId, *pFrame, *pIndex);
}

HRESULT KG3DAnimationSFXTag::GetKeyFrameData(UINT uID, const SFXKeyFrameInfoVersion1** ppKeyFrameData)
{
	HRESULT hr = E_FAIL;
	UINT uFrame, uIndex;
	hr = m_IDMgr.GetFrame(uID, uFrame, uIndex);
	KG_COM_PROCESS_ERROR(hr);

	*ppKeyFrameData = &(m_KeyFrames[uFrame][uIndex].KeyFrame);
	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DAnimationSFXTag::GetSFXFileName(UINT uIndex, LPCSTR* strFileName)
{
    HRESULT hr = E_FAIL;
    KG_PROCESS_ERROR(uIndex < m_pBindInfo->GetModelCount());

    *strFileName = m_pBindInfo->GetModelName(uIndex);
    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DAnimationSFXTag::GetKeyFrameName(UINT uID, LPCSTR* strTagName)
{
    HRESULT hr = E_FAIL;   
    UINT uFrame, uIndex;
    hr = m_IDMgr.GetFrame(uID, uFrame, uIndex);
    KG_COM_PROCESS_ERROR(hr);
    
    ASSERT(m_KeyFrames.count(uFrame));

    *strTagName = m_KeyFrames[uFrame][uIndex].KeyFrame.strTagName;
    hr = S_OK;
Exit0:
    return hr;
}

KG3DAnimationTagSFXMotionSwithEventHandeler::KG3DAnimationTagSFXMotionSwithEventHandeler()
{

}

KG3DAnimationTagSFXMotionSwithEventHandeler::~KG3DAnimationTagSFXMotionSwithEventHandeler()
{
	g_cClipTable.GetEventCenter()->RemoveEventHandler(this);
}

HRESULT KG3DAnimationTagSFXMotionSwithEventHandeler::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
    HRESULT hr = E_FAIL;
    KG3DSFXAnimationController* pSFXController = static_cast<KG3DSFXAnimationController*>(pController);
    IKG3DModel* pModel = pSFXController->GetModel();
    KG_PROCESS_ERROR(pModel);
	RemoveSelfDestruct(static_cast<KG3DModel*>(pModel));
    pModel->UnBindFromOther();
    pModel->Release();
    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DAnimationTagSFXMotionSwithEventHandeler::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
    ASSERT(0);
    return S_OK;
}

IKG3DAnimationEventHandler* GetSFXTagMotionSwitchEventHandeler()
{
    return g_cClipTable.m_lpKG3DAnimationTagSFXMotionSwithEventHandeler;
}

IKG3DAnimationEventHandler* GetSFXTagSceneWorldPosEventHandeler()
{
	return g_cClipTable.m_lpKG3DSFXTagSceneWorldPosEventHandeler;
}

HRESULT KG3DAnimationSFXTag::GetBindInfoInstance(IKG3DModel** ppBindInfo)
{
    HRESULT hr = E_FAIL;
    KG_PROCESS_ERROR(m_pBindInfo);
    KG_PROCESS_ERROR(ppBindInfo);
    *ppBindInfo = m_pBindInfo;
    hr = S_OK;
Exit0:
    return hr;
}


void KG3DAnimationSFXTag::ClearUnusedSFX(DWORD dwMaxFrame)
{
	std::vector<UINT> KeyToRemove;
	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		if( m_KeyedFrame[i] > dwMaxFrame)
			continue;
		for (size_t j = 0; j < m_KeyFrames[m_KeyedFrame[i]].size(); j++)
		{
			const SFXKeyFrameInfoVersion1& Data = m_KeyFrames[m_KeyedFrame[i]][j].KeyFrame;
			if (Data.uBindFileIndex == SFXKeyFrameInfo::TAG_INVALIDATE_VALUE)
			{
				//表示这个标签被批量修改工具删除了
				KeyToRemove.push_back(Data.uID);
			}
		}
	}

	for (size_t i = 0; i < KeyToRemove.size(); i++)
	{
		RemoveTag(KeyToRemove[i]);
	}

	std::set<unsigned int> setUsedBindID;
	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		if( m_KeyedFrame[i] > dwMaxFrame)
			continue;
		for (size_t j = 0; j < m_KeyFrames[m_KeyedFrame[i]].size(); j++)
		{
			const SFXKeyFrameInfoVersion1& Data = m_KeyFrames[m_KeyedFrame[i]][j].KeyFrame;
			setUsedBindID.insert(Data.uBindFileIndex);
		}
	}
	int nModelCount = static_cast<int>(m_pBindInfo->GetModelCount());
	for (int i = nModelCount - 1; i >= 0; i--)
	{
		if (!setUsedBindID.count(i))
		{
			m_pBindInfo->DelModel(i);
			//修正比删除模型ID大的所有关键帧的bindID
			for (size_t j = 0; j < m_KeyedFrame.size(); j++)
			{
				for (size_t k = 0; k < m_KeyFrames[m_KeyedFrame[j]].size(); k++)
				{
					SFXKeyFrameInfoVersion1& Data = m_KeyFrames[m_KeyedFrame[j]][k].KeyFrame;
					if (Data.uBindFileIndex > static_cast<unsigned int>(i))
					{
						Data.uBindFileIndex--;
					}
				}
			}
		}
	}
}

HRESULT KG3DAnimationSFXTag::SaveToMemory(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame)
{
	HRESULT hrReturn = E_FAIL;
	HRESULT hr = E_FAIL;
	DWORD dwKeyFrameCount = 0;
	AnimationTagBlockHeader *pHeader = NULL;
	SFXKeyFrameInfoVersion1 *pFrameData = NULL;
	BYTE* pBindData = NULL;
	DWORD dwBindDataSize = 0;
	BYTE* pBindDataInPack = NULL;
	ClearUnusedSFX(dwMaxFrame);

	KG_PROCESS_ERROR(ppData);
	KG_PROCESS_ERROR(pdwSize);
	KG_PROCESS_ERROR(m_pBindInfo);

	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		if(m_KeyedFrame[i] <= dwMaxFrame)
			dwKeyFrameCount += (DWORD)m_KeyFrames[m_KeyedFrame[i]].size();
	}
    if(dwKeyFrameCount > 0)// add by chenganlai
    {
        hr = m_pBindInfo->SaveToMemory((void**)(&pBindData), &dwBindDataSize);
        KG_COM_PROCESS_ERROR(hr);
    }
	*pdwSize = dwKeyFrameCount * sizeof(SFXKeyFrameInfoVersion1) + sizeof(AnimationTagBlockHeader) + sizeof(BYTE) * dwBindDataSize;
	*ppData = new BYTE[*pdwSize];
	KG_PROCESS_ERROR(*ppData);
    


	pHeader = reinterpret_cast<AnimationTagBlockHeader*>(*ppData);
	ASSERT(pHeader);
	pHeader->dwNumKeyFrames = dwKeyFrameCount;
	pHeader->dwVersion = 0x1;
	pHeader->dwType = TAGTYPE_SFX;

	pBindDataInPack = reinterpret_cast<BYTE*>(pHeader + 1);
	ASSERT(pBindDataInPack);

	memcpy(pBindDataInPack, pBindData, sizeof(BYTE) * dwBindDataSize);
	pFrameData = reinterpret_cast<SFXKeyFrameInfoVersion1*>(pBindDataInPack + sizeof(BYTE) * dwBindDataSize);
	ASSERT(pFrameData);

	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		if(m_KeyedFrame[i] <= dwMaxFrame)
		{
			for (size_t j = 0; j < m_KeyFrames[m_KeyedFrame[i]].size(); j++)
			{
				SFXKeyFrameInfoVersion1 Data;
				memcpy(&Data, &(m_KeyFrames[m_KeyedFrame[i]][j].KeyFrame), sizeof(SFXKeyFrameInfoVersion1));
				Data.uID = m_KeyedFrame[i];
				memcpy(pFrameData, &Data, sizeof(SFXKeyFrameInfoVersion1));
				pFrameData++;
			}
		}	
	}
	hrReturn = S_OK;
Exit0:
	SAFE_DELETE_ARRAY(pBindData);
	return hrReturn;
}



HRESULT KG3DAnimationSFXTag::LoadFromFile(IFile* pFile, DWORD dwVersion, DWORD dwNumFrames)
{
    bool bDuplicated = false;
	HRESULT hr = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    unsigned long uRetCode = 0;
	std::set<UINT> UsedBindFileID;
	SAFE_DELETE(m_pBindInfo);
    m_pBindInfo = new KG3DSFXSuede;
    KG_PROCESS_ERROR(m_pBindInfo);

    KG_PROCESS_ERROR(pFile);
    KG_PROCESS_SUCCESS(!dwNumFrames);
    
    switch(dwVersion)
    {
    case 0:
        {
            hr = m_pBindInfo->LoadFromFile(pFile);
            KG_COM_PROCESS_ERROR(hr);

            for (DWORD i = 0; i < dwNumFrames; i++)
            {
                SFXKeyFrameInfo DataLoad;
				SFXKeyFrameInfoVersion1 DataInput;
                UINT uFrame = 0;

                uRetCode = pFile->Read(&DataLoad, sizeof(SFXKeyFrameInfo));
                KGLOG_PROCESS_ERROR(uRetCode == sizeof(SFXKeyFrameInfo));
                
                uFrame = DataLoad.uID;
                DataLoad.uID = KEYFRAME_PEND;

				DataInput = DataLoad;
                uRetCode = ModifyTag(uFrame, static_cast<void*>(&DataInput));
				                
				if (UsedBindFileID.count(DataInput.uBindFileIndex))
				{
					bDuplicated = TRUE;
				}
				UsedBindFileID.insert(DataInput.uBindFileIndex);
            }
        }
        break;
	case 1:
		{
			hr = m_pBindInfo->LoadFromFile(pFile);
			KG_COM_PROCESS_ERROR(hr);

			for (DWORD i = 0; i < dwNumFrames; i++)
			{
				SFXKeyFrameInfoVersion1 DataLoad;
				UINT uFrame = 0;

				uRetCode = pFile->Read(&DataLoad, sizeof(SFXKeyFrameInfoVersion1));
				KGLOG_PROCESS_ERROR(uRetCode == sizeof(SFXKeyFrameInfoVersion1));

				uFrame = DataLoad.uID;
				DataLoad.uID = KEYFRAME_PEND;
				uRetCode = ModifyTag(uFrame, static_cast<void*>(&DataLoad));

				if (UsedBindFileID.count(DataLoad.uBindFileIndex))
				{
					bDuplicated = TRUE;
				}
				UsedBindFileID.insert(DataLoad.uBindFileIndex);
			}
		}
		break;
    default:
        assert(0);
        break;
    }
	if (bDuplicated)
	{
		//处理一下重复的资源的问题
		std::set<UINT> UsedBindFileID1;
		KeyFrameData::iterator it = m_KeyFrames.begin();
		KeyFrameData::iterator itEnd = m_KeyFrames.end();
		while (it != itEnd)
		{
			std::vector<Data>& KeyFrames = it->second;
			for (size_t i = 0; i < KeyFrames.size(); i++)
			{
				UINT uBindID = KeyFrames[i].KeyFrame.uBindFileIndex;
				if (UsedBindFileID1.count(uBindID))
				{
					//这个是重复的
					m_pBindInfo->CopyBindInfo(uBindID);
					KeyFrames[i].KeyFrame.uBindFileIndex = m_pBindInfo->GetModelCount() - 1;
				}
				UsedBindFileID1.insert(KeyFrames[i].KeyFrame.uBindFileIndex);
			}
			it++;
		}
	}
Exit1:
    hRetCode = S_OK;
Exit0:
    return hRetCode;
}

HRESULT KG3DAnimationSFXTagLifeRangeEventHandeler::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
    return S_OK;
}

HRESULT KG3DAnimationTagSFXMotionSwithEventHandeler::OnMotionNotify(IKG3DAnimationController* pController, DWORD dwMotionInfo, TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
    return S_OK;
}

UINT KG3DAnimationSFXTag::RemoveTag(UINT uID)
{
    UINT uFrame, uIndex, uReturn = IEIR_FAILED;
	HRESULT hr = E_FAIL;

	hr = m_IDMgr.GetFrame(uID, uFrame, uIndex);
	KG_COM_PROCESS_ERROR(hr);

    uReturn = RemoveTag(uFrame, uIndex);
Exit0:
	return uReturn;        
}

UINT KG3DAnimationSFXTag::MoveTag(UINT uID, UINT uFrameTo)
{
    while (GetNumKeyFrame(uID))
	{
		MoveTag(uID, 0, uFrameTo);
	}
    return (UINT)IEIR_SUCCESS;
}

UINT KG3DAnimationSFXTag::CopyTag(UINT uID, UINT uFrameDest)
{
    UINT uNumFrames = GetNumKeyFrame(uID);
	for (UINT uIndex = 0; uIndex < uNumFrames; uIndex++)
	{
		CopyTag(uID, uIndex, uFrameDest);
	}
    return (UINT)IEIR_SUCCESS;        
}


//获得标签的运行状态，只要有任何一个标签还是激活状态，都返回TAG_STATE_RUNNING
//否则返回TAG_STATE_FINISHED
enuTagRunState KG3DAnimationSFXTag::GetState()
{
	enuTagRunState TagRunState = TAG_STATE_FINISHED;
	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		std::vector<Data>* pKeyFrame = GetKeyFrameInfo(m_KeyedFrame[i]);
		if (!pKeyFrame)
			continue;
		for (size_t j = 0; j < pKeyFrame->size(); j++)
		{
			if (SFX_RS_ACTIVE == (*pKeyFrame)[j].RunTime.RunState)
			{
				TagRunState = TAG_STATE_RUNNING;
				goto Exit0;
			}
		}
	}
Exit0:
	return TagRunState;	
}

IKG3DModel* KG3DAnimationSFXTag::GetPatternModel()
{
	static IKG3DModel* pstModel = NULL;
	static bool	stbCanCreate = true;//用于避免在创建失败之后，不断创建
	if (! pstModel)
	{
		if(! stbCanCreate)
		{
			_ASSERTE(! pstModel);
			return NULL;
		}
		TCHAR strPatternModelFilePath[MAX_PATH];
		sprintf_s(strPatternModelFilePath, 
			MAX_PATH,
			"%s",
			"data\\public\\TagEditor\\Pattern.Mesh");
		IKG3DModel* pTemp = NULL;
		HRESULT hr = g_cModelTable.LoadResourceFromFile(strPatternModelFilePath, 0, MLO_MULTI, (IKG3DResourceBase**)&pTemp);
		if (SUCCEEDED(hr))
		{
			pstModel = pTemp;
			ASSERT(pstModel);
		}
		else
		{
			stbCanCreate = false;	//不再尝试创建
			const char cErr[] = "KG3DAnimationSFXTag::GetPatternModel,Pattern.Mesh资源不存在";
			KGLogPrintf(KGLOG_ERR, cErr);
			_KG3D_DEBUG_OUTPUT0(cErr);
		}
	}
	//不用管析构的问题，因为无论Ref是否为0，g_cModelTable会删除掉所有资源
	//理论上，Ref永远是1，只创建了一次
	return pstModel;
}

HRESULT KG3DAnimationSFXTag::Clone(IKG3DAnimationTag* pSrc)
{
	HRESULT hr = E_FAIL;
	KG3DAnimationSFXTag* pSrcTag = dynamic_cast<KG3DAnimationSFXTag*>(pSrc);
	KGLOG_PROCESS_ERROR(pSrcTag);

	m_pBindInfo->Clone(pSrcTag->m_pBindInfo);

	//m_EventHandler = pSrcTag->m_EventHandler;
	m_EventHandler.SetUser(this);

	m_fParentAnimationSpeed = pSrcTag->m_fParentAnimationSpeed;
	m_KeyFrames = pSrcTag->m_KeyFrames;
	m_IDMgr = pSrcTag->m_IDMgr;

	m_KeyedFrame = pSrcTag->m_KeyedFrame;
	m_pParentModel = pSrcTag->m_pParentModel;
	/*{
		KeyFrameData::iterator i = m_KeyFrames.begin();
		KeyFrameData::iterator iend = m_KeyFrames.end();
		while (i!=iend)
		{
			UINT nFrame = i->first;
			std::vector<Data>& vecData = i->second;
			for (size_t t=0;t<vecData.size();t++)
			{
				if(vecData[t].KeyFrame.uBindFileIndex == SFXKeyFrameInfo::TAG_INVALIDATE_VALUE)
				{
					vecData[t].KeyFrame.uBindFileIndex = m_pBindInfo->GetModelCount();
					IKG3DModel* pPatternModel = KG3DAnimationSFXTag::GetPatternModel();
					if (pPatternModel)
					{
						pPatternModel->BindToBone(m_pParentModel, "");
						pPatternModel->AddRef();
						m_pBindInfo->AddModel(static_cast<KG3DModel*>(pPatternModel));
					}  
				}
			}
			i++;
		}
	}*/

	hr = S_OK;
Exit0:
	return hr;
}

DWORD KG3DAnimationSFXTag::GetSourceReference(TagSourceInfo *pBuffer, DWORD dwSize)
{
	DWORD dwCount = 0;
	DWORD dwReturn = INVALIDATESOURCEREFCOUNT;
	KeyFrameData::iterator it, itEnd;
	KG_PROCESS_ERROR(pBuffer);

	it = m_KeyFrames.begin();
	itEnd = m_KeyFrames.end();

	while (it != itEnd)
	{
		std::vector<Data>& KeyFrame = it->second;
		for (size_t i = 0; i < KeyFrame.size(); i++)
		{
			TCHAR strFileName[MAX_PATH];
			TCHAR strFileExt[MAX_PATH];
			(pBuffer + dwCount)->strFilePath = m_pBindInfo->GetModelName(KeyFrame[i].KeyFrame.uBindFileIndex);
			_splitpath_s((pBuffer + dwCount)->strFilePath.c_str(),
				NULL,
				0,
				NULL,
				0,
				strFileName,
				MAX_PATH,
				strFileExt,
				MAX_PATH);
			sprintf_s((pBuffer + dwCount)->strFileName, MAX_PATH, "%s%s", strFileName, strFileExt);
			(pBuffer + dwCount)->dwHash = g_FileNameHash((pBuffer + dwCount)->strFilePath.c_str());
			(pBuffer + dwCount)->dwFrame = it->first;
			(pBuffer + dwCount)->dwIndex = static_cast<DWORD>(i);
			(pBuffer + dwCount)->dwType = TAGTYPE_SFX;
			dwCount++;
			KG_PROCESS_ERROR(dwCount < dwSize);
		}
		it++;
	}
	dwReturn = dwCount;
Exit0:
	return dwReturn;
}

HRESULT KG3DAnimationSFXTag::ChangeResource(UINT uFrame, UINT uIndex, LPCSTR strNew)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	KG3DModel *pNewModel = NULL;
	KeyFrameData::iterator itFind = m_KeyFrames.find(uFrame);
	KG_PROCESS_ERROR(itFind != m_KeyFrames.end());

	std::vector<Data> &KeyFrames = itFind->second;
	KG_PROCESS_ERROR(uIndex < KeyFrames.size());

	hRetCode = g_cModelTable.LoadResourceFromFile(strNew, 0, 0, (IKG3DResourceBase**)&pNewModel);
	KG_COM_PROCESS_ERROR(hRetCode);

	hRetCode = m_pBindInfo->ChangeModel(KeyFrames[uIndex].KeyFrame.uBindFileIndex, pNewModel);
	KG_COM_PROCESS_ERROR(hRetCode);

	hResult = S_OK;
Exit0:
	SAFE_RELEASE(pNewModel);
	return hResult;
}	

HRESULT KG3DAnimationSFXTag::DeleteResource(UINT uFrame, UINT uIndex)
{
	//不能直接删除， 需要在存档时候删除， 因为删除了Index会变
	HRESULT hResult = E_FAIL;
	KeyFrameData::iterator itFind = m_KeyFrames.find(uFrame);
	KG_PROCESS_ERROR(itFind != m_KeyFrames.end());

	std::vector<Data> &KeyFrames = itFind->second;
	KG_PROCESS_ERROR(uIndex < KeyFrames.size());

	KeyFrames[uIndex].KeyFrame.uBindFileIndex = SFXKeyFrameInfo::TAG_INVALIDATE_VALUE;
	
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DAnimationSFXTag::DeleteAllSound()
{
	return S_OK;
}

HRESULT KG3DAnimationSFXTag::ChangePlayType(UINT uFrame, UINT uIndex, void *pDefault)
{
	HRESULT hResult = E_FAIL;
	SFXKeyFrameInfoVersion1 *pNew = NULL;
	KeyFrameData::iterator itFind = m_KeyFrames.find(uFrame);
	KG_PROCESS_ERROR(itFind != m_KeyFrames.end());
	KG_PROCESS_ERROR(pDefault);

	std::vector<Data> &KeyFrames = itFind->second;
	KG_PROCESS_ERROR(uIndex < KeyFrames.size());

	SFXKeyFrameInfoVersion1 &KeyFrame = KeyFrames[uIndex].KeyFrame;
	pNew = static_cast<SFXKeyFrameInfoVersion1*>(pDefault);

	strcpy_s(KeyFrame.strTagName, TAG_NAME_MAX_LENGTH, pNew->strTagName);
	KeyFrame.bAdaptSpeedToMotion = pNew->bAdaptSpeedToMotion;
	KeyFrame.bInterruptable = pNew->bInterruptable;
	KeyFrame.bNotify = pNew->bNotify;
	KeyFrame.bSetWorldPosition = pNew->bSetWorldPosition;
	KeyFrame.FinishState = pNew->FinishState;
	KeyFrame.fSpeed = pNew->fSpeed;
	KeyFrame.MotionFinishEvent = pNew->MotionFinishEvent;
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DAnimationSFXTag::AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	UINT uFrame0, uFrameIndex;
	SFXKeyFrameInfoVersion1 NewFrame;
	if (!pDefault)
	{
		strcpy_s(NewFrame.strTagName, TAG_NAME_MAX_LENGTH, "Batch tool added");
		pDefault = static_cast<void*>(&NewFrame);
	}
	
	UINT uID = ModifyTag(uFrame, static_cast<void*>(pDefault));

	KG_PROCESS_ERROR(uID == NewFrame.uID);
	
	hRetCode = m_IDMgr.GetFrame(uID, uFrame0, uFrameIndex);
	KG_COM_PROCESS_ERROR(hRetCode);

	_ASSERTE(uFrame0 == uFrame0);
	hRetCode = ChangeResource(uFrame, uFrameIndex, strAdd);
	KG_COM_PROCESS_ERROR(hRetCode);

	hResult = S_OK;
Exit0:
	return hResult;
}

KG3DAnimationSFXTagLifeRangeEventHandeler::KG3DAnimationSFXTagLifeRangeEventHandeler()
{
	m_pTag = NULL;
}

KG3DAnimationSFXTagLifeRangeEventHandeler::~KG3DAnimationSFXTagLifeRangeEventHandeler()
{
	g_cClipTable.GetEventCenter()->RemoveEventHandler(this);
}

HRESULT KG3DAnimationSFXTagLifeRangeEventHandeler::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
	return E_NOTIMPL;
}

HRESULT KG3DAnimationSFXTagLifeRangeEventHandeler::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
	HRESULT hr = E_FAIL;
	UINT uFrameIndex = 0;
	UINT uKeyIndex   = 0;
	KG3DAnimationSFXTag::RunTimeInfo RunTime;
	KG_PROCESS_ERROR(pController);
	KG3DTagSFXAnimationController* pTagController = static_cast<KG3DTagSFXAnimationController*>(pController);
	KG_PROCESS_ERROR(pTagController);
	hr = m_pTag->m_IDMgr.GetFrame(pTagController->m_dwSFXID, uFrameIndex, uKeyIndex);

	KG_COM_PROCESS_ERROR(hr);
	DWORD dwMask = KG3DAnimationSFXTag::RTIM_RUNSTATE;
	RunTime.RunState = KG3DAnimationSFXTag::SFX_RS_FININSH;
	const KG3DAnimationSFXTag::RunTimeInfo* pCurRunTime = m_pTag->GetSFXRuntimeState(uFrameIndex, uKeyIndex);
	KG_PROCESS_ERROR(pCurRunTime);
	switch (pCurRunTime->FinishEventType)
	{
	case SFX_FS_DELETE:
		if (pCurRunTime->pModel)
		{
			pCurRunTime->pModel->UnBindFromOther();
			pCurRunTime->pModel->ReSet();
			pCurRunTime->pModel->Release();
			RunTime.pModel = NULL;
			dwMask |= KG3DAnimationSFXTag::RTIM_MODEL;
		}
		RunTime.RunState = KG3DAnimationSFXTag::SFX_RS_FININSH;
		RunTime.LifeRangeType = SFX_MFS_KEEP;
		dwMask |= KG3DAnimationSFXTag::RTIM_MOTIONFINISHSTATE;
		break;
	case SFX_FS_KEEP:
		RunTime.RunState = KG3DAnimationSFXTag::SFX_RS_ACTIVE;
		break;
	case SFX_FS_LOOP:
		RunTime.RunState = KG3DAnimationSFXTag::SFX_RS_ACTIVE;
		static_cast<KG3DSFX*>(pTagController->GetModel())->PlayAnimation(ENU_ANIMATIONPLAY_ONCE, NULL, pController->GetSpeed(), 0, NULL, NULL, ANICTL_PRIMARY);
		break;
	default:
		ASSERT(0);
	}
	m_pTag->SetSFXRuntimeState(pTagController->m_dwSFXID, dwMask, RunTime);
	hr = S_OK;
Exit0:
	return hr;
}


KG3DAnimationSFXTagSceneLifeRangeEventHandeler::KG3DAnimationSFXTagSceneLifeRangeEventHandeler()
{
	m_pScene = NULL;
}

KG3DAnimationSFXTagSceneLifeRangeEventHandeler::~KG3DAnimationSFXTagSceneLifeRangeEventHandeler()
{
	g_cClipTable.GetEventCenter()->RemoveEventHandler(this);
}

HRESULT KG3DAnimationSFXTagSceneLifeRangeEventHandeler::OnTagPlayingSFX(IKG3DAnimationController* pController, IKG3DModel* pModelSFX, TagSFXCallBackExtraInfo* pExtraInfo)
{
	return E_NOTIMPL;
}

HRESULT KG3DAnimationSFXTagSceneLifeRangeEventHandeler::OnPlayAnimationFinish(IKG3DAnimationController* pController)
{
	HRESULT hResult = E_FAIL;
	KG3DScene *pScene = NULL;
	KG3DSFXAnimationController* pSFXController = static_cast<KG3DSFXAnimationController*>(pController);
	IKG3DModel* pModel = pSFXController->GetModel();
	KG_PROCESS_ERROR(pModel);

	pScene = g_cGraphicsTool.GetCurScene();
	KG_PROCESS_ERROR(pScene);

	pScene->RemoveRenderEntity(pModel, 0);
	RemoveSelfDestruct(static_cast<KG3DModel*>(pModel));
	SAFE_RELEASE(pModel);
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DAnimationSFXTagSceneLifeRangeEventHandeler::OnMotionNotify(IKG3DAnimationController* pController, 
																	   DWORD dwMotionInfo, 
																	   TagMotionCallBackExtraInfo *pMotionCallBackExtraInfo)
{
	return E_NOTIMPL;
}