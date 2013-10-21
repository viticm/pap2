#include "StdAfx.h"
#include <dsound.h>
#include "KG3DAnimationSoundTag.h"
#include "KG3DEngineManager.h"
#include "..\..\..\Include\IKG3DSoundShell.h"
#include "KG3DResourcePacker.h"
#include "KG3DAnimationSoundTagInfo.h"
#define SOUND_3D_MIN_DISTANCE 1.0f
#define SOUND_3D_MAX_DISTANCE 50.0f
#define NO_FRAME_ACTIVE 0xffffffff
static const FLOAT fComBy = 100.0;
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define GETVERSION(ID) (ID & 0x0000ffff)
#define GETFRAME(ID) ((ID & 0xffff0000) >> 16)
const float KG3DAnimationSoundTag::s_fDefaultMaxDistance = 2500.0f;
const float KG3DAnimationSoundTag::s_fDefaultMinDistance = 1000.0f;
KG3DAnimationSoundTag::KG3DAnimationSoundTag()
{

}

KG3DAnimationSoundTag::~KG3DAnimationSoundTag()
{

}

KG3DAnimationSoundTag::RunTimeInfo::RunTimeInfo()
{
    RunState = SRS_NONE;
    dwHandle = INVALIDATE_SOUND_HANDLE;
}

UINT KG3DAnimationSoundTag::ModifyTag(UINT uID, void* Data)
{
	UINT uReturn = uID;
	UINT uFrame = GETFRAME(uID);
	UINT uVersion = GETVERSION(uID);
	SoundDataInfoVersion2 data2;
	SoundDataInfoVersion3 data3;
	std::vector<UINT>::iterator it;
	KG_PROCESS_ERROR(Data);
	it = find(m_KeyedFrame.begin(), m_KeyedFrame.end(), uFrame);
	if(it == m_KeyedFrame.end())
	{
		m_KeyedFrame.push_back(uFrame);
		sort(m_KeyedFrame.begin(),m_KeyedFrame.end());
		m_Tags[uFrame].Data = data2;
		m_TagsVersion3[uFrame].Data = data3;
	}
	switch(uVersion)
	{
	case 3:
		data3 = *static_cast<SoundDataInfoVersion3*>(Data);
		m_TagsVersion3[uFrame].Data = data3;
		data2 = m_Tags[uFrame].Data;
		data2.bLoop = data3.bLoop;
		data2.InterruptMode = data3.InterruptMode;
		data2.MotionChangeState = data3.MotionChangeState;
		m_Tags[uFrame].Data = data2;
		break;
	default:
		data2 = *static_cast<SoundDataInfoVersion2*>(Data);
		m_Tags[uFrame].Data = data2;
		data3 = m_TagsVersion3[uFrame].Data;
		data3.bLoop = data2.bLoop;
		data3.InterruptMode = data2.InterruptMode;
		data3.MotionChangeState = data2.MotionChangeState;
		m_TagsVersion3[uFrame].Data = data3;
	}

	uReturn = static_cast<UINT>(IEIR_SUCCESS);
    
Exit0:
    return uReturn;
}
HRESULT KG3DAnimationSoundTag::ClearTag(UINT uId)
{
	UINT uFrame = GETFRAME(uId);
	UINT uVersion = GETVERSION(uId);
	switch(uVersion)
	{
	case 3:
		{
			m_TagsVersion3[uFrame].Data.soundType = ST_NONE;
		}
		break;
	default:
		{
			m_Tags[uFrame].Data.dwValidateCandidate = 0;
		}
		break;
	}
	return S_OK;
}
UINT KG3DAnimationSoundTag::RemoveTag(UINT uFrame)
{
	UINT uReturn = uFrame;
	KeyFrameInfo::iterator itFind;
	std::vector<UINT>::iterator itKeyedFind;
	KeyFrameInfoVersion3::iterator itFindVersion3;
	
	itFindVersion3 = m_TagsVersion3.find(uFrame); 
	KG_PROCESS_ERROR(itFindVersion3 != m_TagsVersion3.end());
	itFind = m_Tags.find(uFrame); 
	KG_PROCESS_ERROR(itFind != m_Tags.end());
	itKeyedFind = find(m_KeyedFrame.begin(), m_KeyedFrame.end(), uFrame);
	KG_PROCESS_ERROR(itKeyedFind != m_KeyedFrame.end());
	m_TagsVersion3.erase(itFindVersion3);
	m_Tags.erase(itFind);
	m_KeyedFrame.erase(itKeyedFind);

	uReturn = static_cast<UINT>(IEIR_SUCCESS);
Exit0:
	return uReturn;
}

void KG3DAnimationSoundTag::Reset()
{
	KeyFrameInfo::iterator it = m_Tags.begin();
	KeyFrameInfoVersion3::iterator itVersion3 = m_TagsVersion3.begin();
	IKG3DSoundShell *pSound = g_cEngineManager.GetSound();
	KG_PROCESS_SUCCESS(!pSound);

	while (it != m_Tags.end())
	{
		if (it->second.RunState.RunState == SRS_ACTIVE)
		{
			if (it->second.RunState.dwHandle != INVALIDATE_SOUND_HANDLE)
			{
				pSound->Stop3DSound(it->second.RunState.dwHandle);
				it->second.RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
			}
		}
		it->second.RunState.RunState = SRS_NONE;
		it++;
	}

	while (itVersion3 != m_TagsVersion3.end())
	{
		if (itVersion3->second.RunState.RunState == SRS_ACTIVE)
		{
			if (itVersion3->second.RunState.dwHandle != INVALIDATE_SOUND_HANDLE)
			{
				pSound->Stop3DSound(itVersion3->second.RunState.dwHandle);
				itVersion3->second.RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
			}
		}
		itVersion3->second.RunState.RunState = SRS_NONE;
		itVersion3++;
	}
Exit1:
	//Exit0:
	;
}

UINT KG3DAnimationSoundTag::MoveTag(UINT uID, UINT uFrameTo)
{
    UINT uReturn = uFrameTo;
    KeyFrameInfo::iterator itFind;

    uReturn = CopyTag(uID, uFrameTo);
    KG_PROCESS_ERROR(uReturn != uFrameTo);
    
    uReturn = RemoveTag(uID);
Exit0:
    return uReturn;
}

UINT KG3DAnimationSoundTag::CopyTag(UINT uID, UINT uDest)
{
	UINT uReturn = uDest;
	KeyFrameInfo::iterator itFindSrc, itFindDest;
	KeyFrameInfoVersion3::iterator itFindSrcVersion3,itFindDestVersion3;
	UINT uDestTemp = 0;
	itFindSrcVersion3 = m_TagsVersion3.find(uID);
	KG_PROCESS_ERROR(itFindSrcVersion3 != m_TagsVersion3.end());

	itFindDestVersion3 = m_TagsVersion3.find(uDest);
	KG_PROCESS_ERROR(itFindDestVersion3 != m_TagsVersion3.end());
	uDestTemp = GETID(uDest,3);
	uReturn = ModifyTag(uDestTemp, static_cast<void*>(&(itFindSrcVersion3->second)));
	itFindSrc = m_Tags.find(uID);
	KG_PROCESS_ERROR(itFindSrc != m_Tags.end());

	itFindDest = m_Tags.find(uDest);
	KG_PROCESS_ERROR(itFindDest != m_Tags.end());

	uDestTemp = GETID(uDest,2);
	uReturn = ModifyTag(uDestTemp, static_cast<void*>(&(itFindSrc->second)));
	KG_PROCESS_ERROR(uReturn == IEIR_SUCCESS);

Exit0:
	return uReturn;
}

UINT KG3DAnimationSoundTag::GetNumKeyFrame(UINT uFrame)
{
    return static_cast<UINT>(m_Tags.count(uFrame));
}

HRESULT KG3DAnimationSoundTag::Clear()
{
    OnMotionChange();
    return S_OK;
}

void KG3DAnimationSoundTag::OnLoop()
{
	//当动作循环时候的处理
	KeyFrameInfo::iterator it = m_Tags.begin();
	KeyFrameInfoVersion3::iterator itVersion3 = m_TagsVersion3.begin();
	IKG3DSoundShell *pSound = g_cEngineManager.GetSound();
	KG_PROCESS_SUCCESS(!pSound);

	while (it != m_Tags.end())
	{
		KeyFrame &Frame = it->second;
        if ((Frame.RunState.RunState == SRS_ACTIVE))
		{
            if (Frame.Data.MotionChangeState == SMCS_DELETE)
			{
				if (Frame.RunState.dwHandle != INVALIDATE_SOUND_HANDLE)
				{
					pSound->Stop3DSound(Frame.RunState.dwHandle);
					Frame.RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
				}
			}
			else
			{
				if (Frame.RunState.dwHandle != INVALIDATE_SOUND_HANDLE)
				{
					if (Frame.Data.bLoop)
					{
						pSound->Stop3DSoundAfterThisTime(Frame.RunState.dwHandle);
					}
					Frame.RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
				}
			}
		}
		_ASSERTE(Frame.RunState.dwHandle == INVALIDATE_SOUND_HANDLE);
		Frame.RunState.RunState = SRS_NONE;
		it++;
	}
	while (itVersion3 != m_TagsVersion3.end())
	{
		KeyFrameVersion3 &Frame = itVersion3->second;
		if ((Frame.RunState.RunState == SRS_ACTIVE))
		{
			if (Frame.Data.MotionChangeState == SMCS_DELETE)
			{
				if (Frame.RunState.dwHandle != INVALIDATE_SOUND_HANDLE)
				{
					pSound->Stop3DSound(Frame.RunState.dwHandle);
					Frame.RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
				}
			}
			else
			{
				if (Frame.RunState.dwHandle != INVALIDATE_SOUND_HANDLE)
				{
					if (Frame.Data.bLoop)
					{
						pSound->Stop3DSoundAfterThisTime(Frame.RunState.dwHandle);
					}
					Frame.RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
				}
			}
		}
		_ASSERTE(Frame.RunState.dwHandle == INVALIDATE_SOUND_HANDLE);
		Frame.RunState.RunState = SRS_NONE;
		itVersion3++;
	}
Exit1:
//Exit0:
	;
}


HRESULT KG3DAnimationSoundTag::Helper_Apply(KG3DAnimationController* pController, 
											UINT uStartFrame, 
											UINT uEndFrame, 
											D3DXVECTOR3 *pvPos)
{
    HRESULT hRetCode = E_FAIL;
	HRESULT hr = E_FAIL;
    size_t uNumKeyedFrame = m_KeyedFrame.size();
    UINT uStartIndex, uEndIndex;
	DWORD dwActiveFrame = NO_FRAME_ACTIVE;
	IKG3DSoundShell *pSound = g_cEngineManager.GetSound();
	KG_PROCESS_SUCCESS(!uNumKeyedFrame);
	KG_PROCESS_SUCCESS(!pSound);

    if (uStartFrame > uEndFrame)
    {
        //在动作循环播放的时候会出现这种情况
        Helper_Apply(pController, uStartFrame, max(m_KeyedFrame[uNumKeyedFrame - 1], uStartFrame), pvPos);
        OnLoop();
		Helper_Apply(pController, min(uEndFrame, m_KeyedFrame[0]), uEndFrame, pvPos);
    }
    else
    {
		hr = FindKeyFrame(uStartFrame, FFT_GREATEREQUAL, m_KeyedFrame, uStartIndex);
        //if failed(hr), no key frame found, exit
        KG_PROCESS_SUCCESS(hr);
        hr = FindKeyFrame(uEndFrame, FFT_LESSEQUAL, m_KeyedFrame, uEndIndex);
        KG_PROCESS_SUCCESS(hr);
        for (UINT uIndex = uStartIndex; uIndex <= uEndIndex; uIndex++)
        {
            DWORD dwHandle = 0;
			BOOL bIsVersion3 = FALSE;

			if(m_TagsVersion3[m_KeyedFrame[uIndex]].Data.soundType != ST_NONE)
				bIsVersion3 = TRUE;
			if(!bIsVersion3)
			{
                KeyFrame& Frame = m_Tags[m_KeyedFrame[uIndex]];
                if (Frame.RunState.RunState == SRS_FINISHED)
			    {
				    continue;
			    }
                if (Frame.RunState.RunState == SRS_ACTIVE)
                {
                    //Sound is playing
				    continue;
                }
                DWORD dwCandidate = GetCandidate(Frame);
			    KG_PROCESS_ERROR(dwCandidate != INVALIDATE_CANDIDATE);

			    dwActiveFrame = m_KeyedFrame[uIndex];
			    SoundTagSoundFileCandidate1 &Candidate = Frame.Data.SoundCandidates[dwCandidate];
                if (Candidate.strSoundFileName.strFileName[0] == '\0')
			    {
				    Frame.RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
				    Frame.RunState.RunState = SRS_FINISHED;
			    }
			    else
			    {
				    D3DXVECTOR3 vec3ListenerPos;
				    pSound->GetListenerPosition(&vec3ListenerPos, NULL);
				    float fDis = D3DXVec3Length(&(vec3ListenerPos - *pvPos));
				    if (pvPos && (fDis <= Candidate.fMaxDistance / 100.f))
				    {
					    D3DXVECTOR3 vVel(0.0f, 0.0f, 0.0f);
					    hr = pSound->Play3DSound(CHARACTER_SOUND,
						    Candidate.strSoundFileName.strFileName,
                                Candidate.fVolume/100.0,
						    //1.0f, //Candidate.fVolume / 100.0f,
						    Frame.Data.bLoop,
						    pvPos,
						    &vVel,
						    g_cEngineOption.fMinSoundRange, 
						    DS3D_DEFAULTMAXDISTANCE, 
						    &dwHandle);
					    KG_COM_PROCESS_ERROR(hr);
				    }

				    Frame.RunState.dwHandle = dwHandle;
				    Frame.RunState.RunState = SRS_ACTIVE;
				 }
			}
			else
			{
				KeyFrameVersion3 & Frame = m_TagsVersion3[m_KeyedFrame[uIndex]];
				if (Frame.RunState.RunState == SRS_FINISHED)
				{
					continue;
				}

				if (Frame.RunState.RunState == SRS_ACTIVE)
				{
					//Sound is playing
					continue;
				}
				AnimationSoundTagInfo SoundTagInfo;
				hr = g_GetAnimationSoundTagInfo().GetSound(Frame.Data.soundType,&SoundTagInfo);
				if(FAILED(hr))
					continue;

            
				D3DXVECTOR3 vVel(0.0f, 0.0f, 0.0f);
				dwActiveFrame = m_KeyedFrame[uIndex];
                if(SoundTagInfo.strFileName[0] == '\0')
                {
                    Frame.RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
                    Frame.RunState.RunState = SRS_FINISHED;
                }
                else
                {
                    D3DXVECTOR3 vec3ListenerPos;
                    pSound->GetListenerPosition(&vec3ListenerPos, NULL);
                    float fDis = D3DXVec3Length(&(vec3ListenerPos - *pvPos));
                    if (pvPos)
                    {
				        hr = pSound->Play3DSound(CHARACTER_SOUND,
					        SoundTagInfo.strFileName,
                            Frame.Data.fVolume/100.0,
					        //1.0f, //Candidate.fVolume / 100.0f,
					        Frame.Data.bLoop,
					        pvPos,
					        &vVel,
					        g_cEngineOption.fMinSoundRange, 
					        DS3D_DEFAULTMAXDISTANCE, 
					        &dwHandle);
				        KG_COM_PROCESS_ERROR(hr);
                    }
                    Frame.RunState.dwHandle = dwHandle;
                    Frame.RunState.RunState = SRS_ACTIVE;
                }
			}
		}
	}
Exit1:
    hRetCode = S_OK;
Exit0:
	ClearHandle(dwActiveFrame);
	UpdatePosition(pvPos);
    return hRetCode;
}

BOOL KG3DAnimationSoundTag::CheckValidation()
{
    return TRUE;
}

void KG3DAnimationSoundTag::SetAnimationSpeed(float fSpeed)
{

}

void KG3DAnimationSoundTag::OnMotionChange()
{
	IKG3DSoundShell *pSound = g_cEngineManager.GetSound();
	KG_PROCESS_SUCCESS(!pSound);

	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		if (m_Tags[m_KeyedFrame[i]].Data.MotionChangeState == SMCS_DELETE)
		{
            if (m_Tags[m_KeyedFrame[i]].RunState.dwHandle != INVALIDATE_SOUND_HANDLE)
            {
			    pSound->Stop3DSound(m_Tags[m_KeyedFrame[i]].RunState.dwHandle);
                m_Tags[m_KeyedFrame[i]].RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
            }
			m_Tags[m_KeyedFrame[i]].RunState.RunState = SRS_FINISHED;
		}
		//需要处理持续播放的问题。之前的处理是把循环的转化为单次。目前没有这个声音的接口。
		else
		{
			if (m_Tags[m_KeyedFrame[i]].RunState.dwHandle != INVALIDATE_SOUND_HANDLE)
			{
				pSound->Stop3DSoundAfterThisTime(m_Tags[m_KeyedFrame[i]].RunState.dwHandle);
				m_Tags[m_KeyedFrame[i]].RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
			}
			m_Tags[m_KeyedFrame[i]].RunState.RunState = SRS_FINISHED;
		}
	}

	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		if (m_TagsVersion3[m_KeyedFrame[i]].Data.MotionChangeState == SMCS_DELETE)
		{
			if (m_TagsVersion3[m_KeyedFrame[i]].RunState.dwHandle != INVALIDATE_SOUND_HANDLE)
			{
				pSound->Stop3DSound(m_TagsVersion3[m_KeyedFrame[i]].RunState.dwHandle);
				m_TagsVersion3[m_KeyedFrame[i]].RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
			}
			m_TagsVersion3[m_KeyedFrame[i]].RunState.RunState = SRS_FINISHED;
		}
		//需要处理持续播放的问题。之前的处理是把循环的转化为单次。目前没有这个声音的接口。
		else
		{
			if (m_TagsVersion3[m_KeyedFrame[i]].RunState.dwHandle != INVALIDATE_SOUND_HANDLE)
			{
				pSound->Stop3DSoundAfterThisTime(m_TagsVersion3[m_KeyedFrame[i]].RunState.dwHandle);
				m_TagsVersion3[m_KeyedFrame[i]].RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
			}
			m_TagsVersion3[m_KeyedFrame[i]].RunState.RunState = SRS_FINISHED;
		}
	}
Exit1:
//Exit0:
	;
}

void KG3DAnimationSoundTag::SetParentModel(IKG3DModel* pModel)
{
    ;
}

void KG3DAnimationSoundTag::UpdatePosition(D3DXVECTOR3 *pPos)
{
	D3DXVECTOR3 vec3Vel(0.0f, 0.0f, 0.0f);
	IKG3DSoundShell *pSound = g_cEngineManager.GetSound();
	KG_PROCESS_SUCCESS(!pSound);
	KG_PROCESS_ERROR(pPos);

	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		DWORD& dwHandle = m_Tags[m_KeyedFrame[i]].RunState.dwHandle;
		if (dwHandle != INVALIDATE_SOUND_HANDLE)
		{
			pSound->Set3DSoundPosition(dwHandle, pPos, &vec3Vel);
		}
	}

	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		DWORD& dwHandle = m_TagsVersion3[m_KeyedFrame[i]].RunState.dwHandle;
		if (dwHandle != INVALIDATE_SOUND_HANDLE)
		{
			pSound->Set3DSoundPosition(dwHandle, pPos, &vec3Vel);
		}
	}
Exit1:
Exit0:
	;
}

void KG3DAnimationSoundTag::ClearHandle(DWORD dwActiveFrame)
{
	IKG3DSoundShell *pSound = g_cEngineManager.GetSound();
	KG_PROCESS_SUCCESS(!pSound);

	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
    {
        DWORD& dwHandle = m_Tags[m_KeyedFrame[i]].RunState.dwHandle;
        if (dwHandle != INVALIDATE_SOUND_HANDLE)
		{
			if (!pSound->IsPlaying3DSound(dwHandle))
			{
				//当声音播放完成
				m_Tags[m_KeyedFrame[i]].RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
				m_Tags[m_KeyedFrame[i]].RunState.RunState = SRS_FINISHED;
			}
			else
			{
				//当声音正在播放时候
				if ((dwActiveFrame == NO_FRAME_ACTIVE) ||//如果没有frame被激活
					(dwActiveFrame == m_KeyedFrame[i]))//或者当前检查的正好是激活帧都直接跳过
					continue;
				if (m_Tags[m_KeyedFrame[i]].Data.InterruptMode == SPI_ENABLE)
				{
					RunTimeInfo& Runtime = m_Tags[m_KeyedFrame[i]].RunState;
                    if (Runtime.dwHandle != INVALIDATE_SOUND_HANDLE)
                    {
                        pSound->Stop3DSound(Runtime.dwHandle);
                        Runtime.dwHandle = INVALIDATE_SOUND_HANDLE;
                    }
					Runtime.RunState = SRS_FINISHED;
				}
			}
		}
	}

	for (size_t i = 0; i < m_KeyedFrame.size(); i++)
	{
		DWORD& dwHandle = m_TagsVersion3[m_KeyedFrame[i]].RunState.dwHandle;
		if (dwHandle != INVALIDATE_SOUND_HANDLE)
		{
			if (!pSound->IsPlaying3DSound(dwHandle))
			{
				//当声音播放完成
				m_TagsVersion3[m_KeyedFrame[i]].RunState.dwHandle = INVALIDATE_SOUND_HANDLE;
				m_TagsVersion3[m_KeyedFrame[i]].RunState.RunState = SRS_FINISHED;
			}
			else
			{
				//当声音正在播放时候
				if ((dwActiveFrame == NO_FRAME_ACTIVE) ||//如果没有frame被激活
					(dwActiveFrame == m_KeyedFrame[i]))//或者当前检查的正好是激活帧都直接跳过
					continue;
				if (m_TagsVersion3[m_KeyedFrame[i]].Data.InterruptMode == SPI_ENABLE)
				{
					RunTimeInfo& Runtime = m_TagsVersion3[m_KeyedFrame[i]].RunState;
					if (Runtime.dwHandle != INVALIDATE_SOUND_HANDLE)
					{
						pSound->Stop3DSound(Runtime.dwHandle);
						Runtime.dwHandle = INVALIDATE_SOUND_HANDLE;
					}
					Runtime.RunState = SRS_FINISHED;
				}
			}
		}
	}
Exit1:
	;
}

HRESULT KG3DAnimationSoundTag::GetKeyFrameData(UINT uFrame, const SoundDataInfoVersion2** pKeyFrameData)
{
    HRESULT hr = E_FAIL;
    KeyFrameInfo::iterator itFind = m_Tags.find(uFrame);
    KG_PROCESS_ERROR(itFind != m_Tags.end());

    *pKeyFrameData = &(itFind->second.Data);
    hr = S_OK;
Exit0:
    return hr;
}
HRESULT KG3DAnimationSoundTag::GetKeyFrameData(UINT uFrame, const SoundDataInfoVersion3 **pKeyFrameData)
{
	HRESULT hResult = E_FAIL;
	KeyFrameInfoVersion3::iterator itFind = m_TagsVersion3.find(uFrame);
	KG_PROCESS_ERROR(itFind != m_TagsVersion3.end());
	*pKeyFrameData = &(itFind->second.Data);
	hResult = S_OK;
Exit0:
	return hResult;
}
static void ConvertSoundPathSave(TCHAR* strFullPath)
{
    TCHAR strPath[MAX_PATH];
	if (strFullPath[0] != '\0')
	{
		g_pFileFinder->GetRelativePath(g_Def_WorkDirectory,
			strFullPath,
			strPath);
		strcpy_s(strFullPath, MAX_PATH, strPath);
	}
}
BOOL KG3DAnimationSoundTag::IsOnlyVersion2Data()
{
    BOOL bResult = FALSE;
    KeyFrameInfoVersion3::iterator it = m_TagsVersion3.begin();
    while (it != m_TagsVersion3.end())
    {
        KG_PROCESS_ERROR((it->second).Data.soundType == ST_NONE);
        it ++;
    }
    bResult = TRUE;
Exit0:
    return bResult;
}
HRESULT KG3DAnimationSoundTag::SaveToMemory2(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame)
{
    HRESULT hResult = E_FAIL;
    AnimationTagBlockHeader *pHeader = NULL;
    SoundDataSaveVersion2 *pFrameData = NULL;
    SoundDataInfoVersion2 FrameDataDefault;
    DWORD dwNumFrameUsed = 0;
    KG_PROCESS_ERROR(ppData);
    KG_PROCESS_ERROR(pdwSize);
    for (size_t i = 0 ; i < m_KeyedFrame.size() ; i ++)
    {
        if(m_KeyedFrame[i] <= dwMaxFrame)
        {
            if(m_Tags.find(m_KeyedFrame[i]) == m_Tags.end())
                ASSERT(FALSE&&"声音标签版本2存档出错");
            if(m_Tags[m_KeyedFrame[i]].Data.dwValidateCandidate != 0)
                dwNumFrameUsed ++;
        }
    }
    *pdwSize = (DWORD)(dwNumFrameUsed * sizeof(SoundDataSaveVersion2)) + sizeof(AnimationTagBlockHeader);
    *ppData = new BYTE[*pdwSize];
    KG_PROCESS_ERROR(*ppData);
    pHeader = reinterpret_cast<AnimationTagBlockHeader*>(*ppData);
    ASSERT(pHeader);

    pHeader->dwNumKeyFrames = dwNumFrameUsed;
    pHeader->dwType = TAGTYPE_SOUND;
    pHeader->dwVersion = 2;
    
    pFrameData = reinterpret_cast<SoundDataSaveVersion2*>(pHeader  + 1);
    ASSERT(pFrameData);
    for (size_t i = 0; i < m_KeyedFrame.size(); i++)
    {
        UINT uFrame = m_KeyedFrame[i];
        if(uFrame <= dwMaxFrame)
        {
            if(m_Tags.find(uFrame) != m_Tags.end())
            {
                if(m_Tags[uFrame].Data.dwValidateCandidate == 0)
                    continue;
                pFrameData->dwFrame = uFrame;
                memcpy(&(pFrameData->DataInfo), &(m_Tags[m_KeyedFrame[i]].Data), sizeof(SoundDataInfoVersion2));
                for (DWORD j = 0; j < pFrameData->DataInfo.dwValidateCandidate; j++)
                {
                    ConvertSoundPathSave(pFrameData->DataInfo.SoundCandidates[j].strSoundFileName.strFileName);
                }
            }
            else
            {
                ASSERT(FALSE&&"声音标签版本2存档出错");
            }
            pFrameData++;
        }
    }
    hResult = S_OK;
Exit0:
    return hResult;
}
HRESULT KG3DAnimationSoundTag::SaveToMemory3(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame)
{
    HRESULT hr = E_FAIL;
    AnimationTagBlockHeader *pHeader = NULL;
    SoundDataSaveVersion2 *pFrameData = NULL;
    SoundDataSaveVersion3 *pFrameDataVersion3 = NULL;
    SoundDataInfoVersion2 FrameDataDefault;
    SoundDataInfoVersion3 FrameDataDefaultVersion3;
    DWORD dwNumFrameUsed = 0;
    std::vector<UINT>::iterator itSet;
    KG_PROCESS_ERROR(ppData);
    KG_PROCESS_ERROR(pdwSize);

    dwNumFrameUsed = 0;

    for (size_t i = 0 ; i < m_KeyedFrame.size() ; i ++)
    {
        if(m_KeyedFrame[i] <= dwMaxFrame)
            dwNumFrameUsed ++;
    }
    *pdwSize = (DWORD)(dwNumFrameUsed * sizeof(SoundDataSaveVersion2)) + (DWORD)(dwNumFrameUsed * sizeof(SoundDataSaveVersion3))
        + sizeof(AnimationTagBlockHeader);
    *ppData = new BYTE[*pdwSize];
    KG_PROCESS_ERROR(*ppData);

    pHeader = reinterpret_cast<AnimationTagBlockHeader*>(*ppData);
    ASSERT(pHeader);

    pHeader->dwNumKeyFrames = dwNumFrameUsed;
    pHeader->dwType = TAGTYPE_SOUND;
    pHeader->dwVersion = CURRENT_VERSION;
    pFrameDataVersion3 = reinterpret_cast<SoundDataSaveVersion3*>(pHeader  + 1);
    ASSERT(pFrameDataVersion3);
    itSet = m_KeyedFrame.begin();
    while (itSet != m_KeyedFrame.end())
    {
        UINT uFrame = *itSet;
        if(uFrame <= dwMaxFrame)
        {
            pFrameDataVersion3->dwFrame = uFrame;

            if (m_TagsVersion3.find(uFrame) != m_TagsVersion3.end())
            {
                memcpy_s(&(pFrameDataVersion3->DataInfo),sizeof(SoundDataInfoVersion3),&(m_TagsVersion3[uFrame].Data),sizeof(SoundDataInfoVersion3));
            }
            else
            {
                ASSERT(FALSE&&"声音标签版本3存档出错");
                //memcpy_s(&(pFrameDataVersion3->DataInfo),sizeof(SoundDataInfoVersion3),&FrameDataDefaultVersion3,sizeof(SoundDataInfoVersion3));
            }
            pFrameDataVersion3 ++;
        }
        itSet++;
    }
    pFrameData =  reinterpret_cast<SoundDataSaveVersion2*>(pFrameDataVersion3);
    ASSERT(pFrameData);
    for (size_t i = 0; i < m_KeyedFrame.size(); i++)
    {
        UINT uFrame = m_KeyedFrame[i];
        if(uFrame <= dwMaxFrame)
        {
            if(m_Tags.find(uFrame) != m_Tags.end())
            {
                pFrameData->dwFrame = uFrame;
                memcpy(&(pFrameData->DataInfo), &(m_Tags[m_KeyedFrame[i]].Data), sizeof(SoundDataInfoVersion2));
                for (DWORD j = 0; j < pFrameData->DataInfo.dwValidateCandidate; j++)
                {
                    ConvertSoundPathSave(pFrameData->DataInfo.SoundCandidates[j].strSoundFileName.strFileName);
                }
            }
            else
            {
                ASSERT(FALSE&&"声音标签版本2存档出错");
            }
            pFrameData++;
        }
    }


    hr = S_OK;
Exit0:
    return hr;
}
HRESULT KG3DAnimationSoundTag::SaveToMemory(BYTE** ppData, DWORD* pdwSize,DWORD dwMaxFrame)
{
	HRESULT hr = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    BOOL bOnlyVersion2Data = FALSE;
    int nSaveVersion3Data = 0;
	KG_PROCESS_ERROR(ppData);
	KG_PROCESS_ERROR(pdwSize);
    bOnlyVersion2Data = IsOnlyVersion2Data();
    nSaveVersion3Data = g_GetAnimationSoundTagInfo().IsSaveVersionData();
	ClearDumpedTag();
	
    if(bOnlyVersion2Data||nSaveVersion3Data == 0)
    {
        hRetCode = SaveToMemory2(ppData,pdwSize,dwMaxFrame);
        KG_COM_PROCESS_ERROR(hRetCode);
    }
    else
    {
        hRetCode = SaveToMemory3(ppData,pdwSize,dwMaxFrame);
        KG_COM_PROCESS_ERROR(hRetCode);
    }
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DAnimationSoundTag::LoadFromFile(IFile* pFile, DWORD dwVersion, DWORD dwNumFrames)
{
    HRESULT hr = E_FAIL;
    unsigned long uRetCode = 0;

    KG_PROCESS_ERROR(pFile);

    switch(dwVersion)
    {
    case 0:
        {
            for (DWORD i = 0; i < dwNumFrames; i++)
            {
				SoundDataSave DataIn;
				SoundDataSaveVersion2 NewData;
				SoundDataInfoVersion3 SoundDataVersion3;
				UINT uId = 0;
				uRetCode = pFile->Read(&DataIn, sizeof(SoundDataSave));
				KG_PROCESS_ERROR(uRetCode == sizeof(SoundDataSave));
				uId = GETID(DataIn.dwFrame,2);
				NewData = DataIn;
				//UpdateVersion021(DataIn.DataInfo, NewData);
				//NewData.PlayType = SPT_ONCE;
				uRetCode = ModifyTag(uId, (void*)(&NewData.DataInfo));
				KG_PROCESS_ERROR(uRetCode == IEIR_SUCCESS);
				uId = GETID(DataIn.dwFrame,3);
				SoundDataVersion3.bLoop = NewData.DataInfo.bLoop;
				SoundDataVersion3.InterruptMode = NewData.DataInfo.InterruptMode;
				SoundDataVersion3.MotionChangeState = NewData.DataInfo.MotionChangeState;
				uRetCode = ModifyTag(uId, (void*)(&SoundDataVersion3));
				KG_PROCESS_ERROR(uRetCode == IEIR_SUCCESS);
            }
        }
		break;
	case 1:
		{
			for (DWORD i = 0; i < dwNumFrames; i++)
			{
				SoundDataSaveVersion1 DataIn;
				SoundDataSaveVersion2 NewData;
				SoundDataInfoVersion3 SoundDataVersion3;
				UINT uId = 0;
				uRetCode = pFile->Read(&DataIn, sizeof(SoundDataSaveVersion1));
				KG_PROCESS_ERROR(uRetCode == sizeof(SoundDataSaveVersion1));
				uId = GETID(DataIn.dwFrame,2);
				DataIn.DataInfo.bLoop = FALSE;
				NewData = DataIn;
				uRetCode = ModifyTag(uId, (void*)(&NewData.DataInfo));
				KG_PROCESS_ERROR(uRetCode == IEIR_SUCCESS);
				uId = GETID(DataIn.dwFrame,3);
				SoundDataVersion3.bLoop = NewData.DataInfo.bLoop;
				SoundDataVersion3.InterruptMode = NewData.DataInfo.InterruptMode;
				SoundDataVersion3.MotionChangeState = NewData.DataInfo.MotionChangeState;
				uRetCode = ModifyTag(uId, (void*)(&SoundDataVersion3));
				KG_PROCESS_ERROR(uRetCode == IEIR_SUCCESS);
			}
		}
        break;
	case 2:
		{
			for (DWORD i = 0; i < dwNumFrames; i++)
			{
				SoundDataSaveVersion2 DataIn;
				SoundDataInfoVersion3 SoundDataVersion3;
				UINT uId = 0;
				uRetCode = pFile->Read(&DataIn, sizeof(SoundDataSaveVersion2));
				KG_PROCESS_ERROR(uRetCode == sizeof(SoundDataSaveVersion2));
				uId = GETID(DataIn.dwFrame ,2);
				float fSum = 0.0f;
				for (DWORD j = 0; j < DataIn.DataInfo.dwValidateCandidate; j++)
				{
					fSum += DataIn.DataInfo.SoundCandidates[j].fHitRate;
				}

				if (abs(fSum) < FLT_EPSILON)
				{
					fSum = 1.0f;
				}

				fSum /= 100.0f;

				for (DWORD j = 0; j < DataIn.DataInfo.dwValidateCandidate; j++)
				{
					DataIn.DataInfo.SoundCandidates[i].fHitRate /= fSum;
				}
				//DataIn.DataInfo.bLoop = FALSE;
				uRetCode = ModifyTag(uId, (void*)(&DataIn.DataInfo));
				KG_PROCESS_ERROR(uRetCode == IEIR_SUCCESS);
				uId = GETID(DataIn.dwFrame,3);
				SoundDataVersion3.bLoop = DataIn.DataInfo.bLoop;
				SoundDataVersion3.InterruptMode = DataIn.DataInfo.InterruptMode;
				SoundDataVersion3.MotionChangeState = DataIn.DataInfo.MotionChangeState;
				uRetCode = ModifyTag(uId, (void*)(&SoundDataVersion3));
				KG_PROCESS_ERROR(uRetCode == IEIR_SUCCESS);
			}
		}
		break;
	case 3:
		// 在 版本3中，仍然会有版本2的数据，可以自由选择打开哪种版本的界面
		for(DWORD i = 0 ; i < dwNumFrames ; i ++)
		{// 读取版本3 数据
			SoundDataSaveVersion3 DataIn3;
			UINT uID = 0;
			uRetCode = pFile->Read(&DataIn3,sizeof(SoundDataSaveVersion3));
			KG_PROCESS_ERROR(uRetCode == sizeof(SoundDataSaveVersion3));

			uID = GETID(DataIn3.dwFrame,3);
			uRetCode = ModifyTag(uID,(void*)(&DataIn3.DataInfo));
			KG_PROCESS_ERROR(uRetCode == IEIR_SUCCESS);
		}

		for (DWORD i = 0 ; i < dwNumFrames ; i ++)
		{// 读取版本2数据
			SoundDataSaveVersion2 DataIn2;
			UINT uID = 0; 

			uRetCode = pFile->Read(&DataIn2,sizeof(SoundDataSaveVersion2));
			KG_PROCESS_ERROR(uRetCode == sizeof(SoundDataSaveVersion2));

			float fSum = 0.0f;
			for (DWORD j = 0; j < DataIn2.DataInfo.dwValidateCandidate; j++)
			{
				fSum += DataIn2.DataInfo.SoundCandidates[j].fHitRate;
			}

			if (abs(fSum) < FLT_EPSILON)
			{
				fSum = 1.0f;
			}

			fSum /= 100.0f;

			for (DWORD j = 0; j < DataIn2.DataInfo.dwValidateCandidate; j++)
			{
				DataIn2.DataInfo.SoundCandidates[i].fHitRate /= fSum;
			}

			uID = GETID(DataIn2.dwFrame,2);
			uRetCode = ModifyTag(uID,(void*)(&DataIn2.DataInfo));
			KG_PROCESS_ERROR(uRetCode == IEIR_SUCCESS);
		}
		break;
    default:
        assert(0);
    }
    hr = S_OK;
Exit0:
    return hr;
}

KG3DAnimationSoundTag::SoundDataSaveVersion2& KG3DAnimationSoundTag::SoundDataSaveVersion2::operator =(const SoundDataSave &In)
{
	dwFrame = In.dwFrame;
	strcpy_s(DataInfo.strTagName, TAG_NAME_MAX_LENGTH, In.DataInfo.strTagName);
	DataInfo.dwValidateCandidate = 1;
	memcpy_s(&DataInfo.SoundCandidates[0].strSoundFileName, 
		sizeof(SafeFileName), 
		&In.DataInfo.strSoundFileName, 
		sizeof(SafeFileName));
	DataInfo.SoundCandidates[0].fHitRate = 100.0f;
	DataInfo.SoundCandidates[0].fVolume = 100.0f;
	DataInfo.SoundCandidates[0].fMaxDistance = s_fDefaultMaxDistance;
	DataInfo.SoundCandidates[0].fMinDistance = s_fDefaultMinDistance;
	DataInfo.bLoop = In.DataInfo.bLoop;
	DataInfo.MotionChangeState = In.DataInfo.MotionChangeState;
	DataInfo.InterruptMode = In.DataInfo.InterruptMode;
	return *this;
}

KG3DAnimationSoundTag::SoundDataSaveVersion2& KG3DAnimationSoundTag::SoundDataSaveVersion2::operator =(const SoundDataSaveVersion1 &In)
{
	dwFrame = In.dwFrame;
	strcpy_s(DataInfo.strTagName, TAG_NAME_MAX_LENGTH, In.DataInfo.strTagName);
	DataInfo.dwValidateCandidate = In.DataInfo.dwValidateCandidate;
	for (DWORD i = 0; i < In.DataInfo.dwValidateCandidate; i++)
	{
		memcpy_s(&DataInfo.SoundCandidates[i].strSoundFileName, 
		sizeof(SafeFileName), 
		&In.DataInfo.SoundCandidates[i].strSoundFileName, 
		sizeof(SafeFileName));
		DataInfo.SoundCandidates[i].fHitRate = In.DataInfo.SoundCandidates[i].fHitRate;
		DataInfo.SoundCandidates[i].fVolume = In.DataInfo.SoundCandidates[i].fVolume;
		DataInfo.SoundCandidates[i].fMinDistance = s_fDefaultMinDistance;
		DataInfo.SoundCandidates[i].fMaxDistance = s_fDefaultMaxDistance;
	}
	
	DataInfo.bLoop = In.DataInfo.bLoop;
	DataInfo.MotionChangeState = In.DataInfo.MotionChangeState;
	DataInfo.InterruptMode = In.DataInfo.InterruptMode;
	return *this;
}

inline DWORD KG3DAnimationSoundTag::GetCandidate(const KeyFrame& Frame)
{
	float fRandom = static_cast<float>(rand() % 100);
	float fCurrentFloor = 0.0f;
	for (DWORD i = 0; i < Frame.Data.dwValidateCandidate; i++)
	{
		if (fRandom >= fCurrentFloor && fRandom <= fCurrentFloor + Frame.Data.SoundCandidates[i].fHitRate)
			return i;
		fCurrentFloor += Frame.Data.SoundCandidates[i].fHitRate;
	}
	return INVALIDATE_CANDIDATE;
}

HRESULT KG3DAnimationSoundTag::DeleteSoundCandidate(UINT uFrame, UINT uIndex)
{
	HRESULT hr = E_FAIL;
	KeyFrameInfo::iterator itFind = m_Tags.find(uFrame);
	SoundDataInfoVersion2 *pData = NULL;
	KG_PROCESS_ERROR(itFind != m_Tags.end());

	pData = &(itFind->second.Data);
	KG_PROCESS_ERROR(uIndex < pData->dwValidateCandidate);
	
	for (DWORD dwIndex = uIndex; dwIndex < pData->dwValidateCandidate - 1; dwIndex++)
	{
		pData->SoundCandidates[dwIndex] = pData->SoundCandidates[dwIndex + 1];
	}
	pData->SoundCandidates[pData->dwValidateCandidate - 1] = SoundTagSoundFileCandidate1();
	pData->dwValidateCandidate--;
	hr = S_OK;
Exit0:
	return hr;
}

enuTagRunState KG3DAnimationSoundTag::GetState()
{
	enuTagRunState TagState = TAG_STATE_FINISHED;
	return TagState;
}

HRESULT KG3DAnimationSoundTag::Clone(IKG3DAnimationTag* pSrc)
{
	HRESULT hr = E_FAIL;
	KG3DAnimationSoundTag* pSrcTag = dynamic_cast<KG3DAnimationSoundTag*>(pSrc);
	KGLOG_PROCESS_ERROR(pSrcTag);

	m_Tags = pSrcTag->m_Tags;
	m_KeyedFrame = pSrcTag->m_KeyedFrame;
	m_fParentAnimationSpeed = pSrcTag->m_fParentAnimationSpeed;

	m_TagsVersion3 = pSrcTag->m_TagsVersion3;
	hr = S_OK;
Exit0:
	return hr;
}

void KG3DAnimationSoundTagBatchTool::SetAllSoundTagKeep(KG3DAnimationSoundTag *pTag)
{
	KG_PROCESS_ERROR(pTag);

	{
		KG3DAnimationSoundTag::KeyFrameInfo::iterator it = pTag->m_Tags.begin();
		KG3DAnimationSoundTag::KeyFrameInfo::iterator itEnd = pTag->m_Tags.end();

		while (it != itEnd)
		{
			KG3DAnimationSoundTag::KeyFrame &KeyFrame = it->second;
			KeyFrame.Data.MotionChangeState = SMCS_KEEP;
			it++;
		}
	}
	{
		KG3DAnimationSoundTag::KeyFrameInfoVersion3::iterator it = pTag->m_TagsVersion3.begin();
		KG3DAnimationSoundTag::KeyFrameInfoVersion3::iterator itEnd = pTag->m_TagsVersion3.end();
		while (it != itEnd)
		{
			KG3DAnimationSoundTag::KeyFrameVersion3 &KeyFrame = it->second;
			KeyFrame.Data.MotionChangeState = SMCS_KEEP;
			it ++;
		}
	}
Exit0:
	;
}

DWORD KG3DAnimationSoundTag::GetSourceReference(TagSourceInfo *pBuffer, DWORD dwSize)
{
	DWORD dwCount = 0;
	DWORD dwReturn = INVALIDATESOURCEREFCOUNT;
	KeyFrameInfo::iterator it, itEnd;
	KG_PROCESS_ERROR(pBuffer);
	
	it = m_Tags.begin();
	itEnd = m_Tags.end();

	while (it != itEnd)
	{
		KG3DAnimationSoundTag::KeyFrame &KeyFrame = it->second;
		for (DWORD i = 0; i < KeyFrame.Data.dwValidateCandidate; i++)
		{
			TCHAR strFileName[MAX_PATH];
			TCHAR strFileExt[MAX_PATH];

			(pBuffer + dwCount)->dwFrame = it->first;
			(pBuffer + dwCount)->dwIndex = i;
			(pBuffer + dwCount)->dwType = TAGTYPE_SOUND;
			(pBuffer + dwCount)->strFilePath = KeyFrame.Data.SoundCandidates[i].strSoundFileName.strFileName;
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
			dwCount++;
			KG_PROCESS_ERROR(dwCount < dwSize);
		}
		it++;
	}
	dwReturn = dwCount;
Exit0:
	return dwReturn;
}


HRESULT KG3DAnimationSoundTag::ChangeResource(UINT uFrame, 
											  UINT uIndex,
											  LPCSTR strNew)
{
	HRESULT hResult = E_FAIL;
	KeyFrameInfo::iterator itFind = m_Tags.find(uFrame);
	
	KG_PROCESS_ERROR(itFind != m_Tags.end());

	SoundDataInfoVersion2 &Info = itFind->second.Data;
	KG_PROCESS_ERROR(uIndex < Info.dwValidateCandidate);

	SoundTagSoundFileCandidate1 &Candidate = itFind->second.Data.SoundCandidates[uIndex];
	strcpy(Candidate.strSoundFileName.strFileName, strNew);
	
	hResult = S_OK;
Exit0:
	return hResult;
}	

HRESULT KG3DAnimationSoundTag::DeleteResource(UINT uFrame, 
											  UINT uIndex)
{
	HRESULT hResult = E_FAIL;
	KeyFrameInfo::iterator itFind = m_Tags.find(uFrame);

	KG_PROCESS_ERROR(itFind != m_Tags.end());

	SoundDataInfoVersion2 &Info = itFind->second.Data;
	KG_PROCESS_ERROR(uIndex < Info.dwValidateCandidate);

	SoundTagSoundFileCandidate1 &Candidate = itFind->second.Data.SoundCandidates[uIndex];
	memset(Candidate.strSoundFileName.strFileName, 0, sizeof(TCHAR) * MAX_PATH);
	Candidate.fHitRate = 0.0f;
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DAnimationSoundTag::AddResource(UINT uFrame, LPCSTR strAdd, void *pDefault)
{
	HRESULT hResult = E_FAIL;
	SoundDataInfoVersion2 *pNew = NULL;
	KeyFrameInfo::iterator itFind = m_Tags.find(uFrame);

	if (itFind != m_Tags.end())
	{
		SoundDataInfoVersion2 &Info = itFind->second.Data;
		KG_PROCESS_ERROR(Info.dwValidateCandidate < SOUND_TAG_MAX_CANDIDATES - 1);

		SoundTagSoundFileCandidate1 &Candidate = itFind->second.Data.SoundCandidates[Info.dwValidateCandidate];
		strcpy_s(Candidate.strSoundFileName.strFileName, MAX_PATH, strAdd);
		Info.dwValidateCandidate++;
		
		if (pDefault)
		{
			pNew = static_cast<SoundDataInfoVersion2*>(pDefault);
			SoundTagSoundFileCandidate1 &CandidateNew = pNew->SoundCandidates[0];
			Candidate.fHitRate = CandidateNew.fHitRate;
			Candidate.fVolume = CandidateNew.fVolume;
			Info.bLoop = pNew->bLoop;
			Info.InterruptMode = pNew->InterruptMode;
			Info.MotionChangeState = pNew->MotionChangeState;
			strcpy_s(Info.strTagName, TAG_NAME_MAX_LENGTH, pNew->strTagName);	
		}
	}
	else
	{
		SoundDataInfoVersion2 NewFrame;
		SoundDataInfoVersion2 *pNew = &NewFrame;
		UINT uId = GETID(uFrame,2);
		if (!pDefault)
		{
			NewFrame.dwValidateCandidate = 1;
			NewFrame.SoundCandidates[0].fHitRate = 100.0f;
		}
		else
		{
			pNew = static_cast<SoundDataInfoVersion2*>(pDefault);
			pNew->dwValidateCandidate = 1;
		}
		strcpy_s(pNew->SoundCandidates[0].strSoundFileName.strFileName, MAX_PATH, strAdd);
		ModifyTag(uId, pNew);
	}
	hResult = S_OK;
Exit0:
	return hResult;	
}

HRESULT KG3DAnimationSoundTag::ChangePlayType(UINT uFrame, UINT uIndex, void *pDefault)
{
	HRESULT hResult = E_FAIL;
	KeyFrameInfo::iterator itFind = m_Tags.find(uFrame);
	SoundDataInfoVersion2 *pNew = NULL;

	KG_PROCESS_ERROR(pDefault);
	KG_PROCESS_ERROR(itFind != m_Tags.end());

	SoundDataInfoVersion2 &Info = itFind->second.Data;
	KG_PROCESS_ERROR(uIndex < Info.dwValidateCandidate);

	SoundTagSoundFileCandidate1 &Candidate = itFind->second.Data.SoundCandidates[uIndex];
	pNew = static_cast<SoundDataInfoVersion2*>(pDefault);

	Info.bLoop = pNew->bLoop;
	Info.InterruptMode = pNew->InterruptMode;
	Info.MotionChangeState = pNew->MotionChangeState;
	strcpy_s(Info.strTagName, TAG_NAME_MAX_LENGTH, pNew->strTagName);

	Candidate.fHitRate = pNew->SoundCandidates[0].fHitRate;
	Candidate.fVolume = pNew->SoundCandidates[0].fVolume;

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DAnimationSoundTag::DeleteAllSound()
{
	m_Tags.clear();
	m_KeyedFrame.clear();
	m_TagsVersion3.clear();
	return S_OK;
}

void KG3DAnimationSoundTag::ClearDumpedTag()
{
	KeyFrameInfo::iterator it = m_Tags.begin();
	std::set<UINT> ZeroKeys;
	std::set<UINT>::iterator itSet;
	KeyFrameInfoVersion3::iterator itVersion3 = m_TagsVersion3.begin();
	while (it != m_Tags.end())
	{
		SoundDataInfoVersion2 &Data = it->second.Data;
		DWORD dwNumCandidate = Data.dwValidateCandidate; 
		for (DWORD i = 0; i < dwNumCandidate; i++)
		{
			if (Data.SoundCandidates[i].fHitRate == 0.0f)
			{
				if (i < Data.dwValidateCandidate - 1) //如果不是最后一个， 用最后一个覆盖掉这个需要删除的
				{
					memcpy(&Data.SoundCandidates[i], 
						&Data.SoundCandidates[Data.dwValidateCandidate - 1], 
						sizeof(SoundTagSoundFileCandidate1));
				}
				Data.dwValidateCandidate--;
			}
		}
		if (Data.dwValidateCandidate == 0)
		{
			//如果没有Candidate了就要去掉Key
			ZeroKeys.insert(it->first);
		}
		it++;
	}

	for (int i = static_cast<int>(m_KeyedFrame.size()) - 1; i >= 0; i--)
	{
		if (ZeroKeys.count(m_KeyedFrame[i]))
		{
		    if(m_TagsVersion3.find(m_KeyedFrame[i]) == m_TagsVersion3.end() ||
			    m_TagsVersion3[m_KeyedFrame[i]].Data.soundType == ST_NONE)
		    {
			    m_Tags.erase(m_KeyedFrame[i]);
			    m_TagsVersion3.erase(m_KeyedFrame[i]);
			    std::vector<UINT>::iterator itFind = find(m_KeyedFrame.begin(), m_KeyedFrame.end(), m_KeyedFrame[i]);
			    _ASSERTE(itFind != m_KeyedFrame.end());
			    m_KeyedFrame.erase(itFind);
            }   
		}
	}
}

HRESULT KG3DAnimationSoundTag::GetSound(SoundType soundType,AnimationSoundTagInfo * pInfo)
{
	return g_GetAnimationSoundTagInfo().GetSound(soundType,pInfo);
}


BOOL KG3DAnimationSoundTag::IsModify()
{
    
    SoundDataInfoVersion2 SoundData;
    SoundDataInfoVersion3 SoundDataVersion3;
    for (size_t i = 0 ; i < m_KeyedFrame.size() ; i ++)
    {
        SoundData = m_Tags[m_KeyedFrame[i]].Data;
        SoundDataVersion3 = m_TagsVersion3[m_KeyedFrame[i]].Data;
        for (int j = 0 ; j < SoundData.dwValidateCandidate ; j ++)
        {
            if( fabs(SoundData.SoundCandidates[j].fVolume - fComBy) > 1e-5)
                return TRUE;
        }
        if( SoundDataVersion3.soundType != ST_NONE && fabs(SoundDataVersion3.fVolume - fComBy) > 1e-5)
            return TRUE;
    }
    return FALSE;
}

HRESULT KG3DAnimationSoundTag::GetSoundTagInfo(void *pDefault)
{
    HRESULT hResult = E_FAIL;
    SoundDataInfoVersion2 SoundData;
    SoundDataInfoVersion3 SoundDataVersion3;
    std::vector<SoundTagInfo> *pvSoundTagInfo = NULL;
    KG_PROCESS_ERROR(pDefault);
    pvSoundTagInfo = static_cast<std::vector<SoundTagInfo> *>(pDefault);
    KG_PROCESS_ERROR(pvSoundTagInfo);
    for (size_t i = 0 ; i < m_KeyedFrame.size() ; i ++)
    {
        SoundData = m_Tags[m_KeyedFrame[i]].Data;
        SoundDataVersion3 = m_TagsVersion3[m_KeyedFrame[i]].Data;
        SoundTagInfo TagInfo;
        for (int j = 0 ; j < SoundData.dwValidateCandidate ; j ++)
        {
            TagInfo.dwFrame = m_KeyedFrame[i];
            TagInfo.dwIndex = j;
            TagInfo.nVersion = 2;
            sprintf_s(TagInfo.strFileName,MAX_PATH,"%s",SoundData.SoundCandidates[j].strSoundFileName.strFileName);
            TagInfo.fVolume = SoundData.SoundCandidates[j].fVolume;
            pvSoundTagInfo->push_back(TagInfo);
        }
        if(SoundDataVersion3.soundType != ST_NONE)
        {
            TagInfo.dwFrame = m_KeyedFrame[i];
            TagInfo.fVolume = SoundDataVersion3.fVolume;
            TagInfo.nVersion = 3;
            TagInfo.soundType = SoundDataVersion3.soundType;
            pvSoundTagInfo->push_back(TagInfo);
        }
    }
    hResult = S_OK;
Exit0:
    return hResult; 
}

HRESULT KG3DAnimationSoundTag::Play3DSound(LPSTR pFileName,FLOAT fVolume)
{
    HRESULT hResult = E_FAIL;
    DWORD dwDummyHandle = 0;
    D3DXVECTOR3 vec3Vel(.0f, .0f, .0f);
    D3DXVECTOR3 pos(.0f, .0f, .0f);
    IKG3DSoundShell *pSound = g_cEngineManager.GetSound();
    KG_PROCESS_ERROR(pSound);
    if (pFileName[0] != '\0' && pSound)
    {
        pSound->Play3DSound(CHARACTER_SOUND,
            pFileName,
            fVolume/100.0,
            FALSE,
            &pos,
            &vec3Vel,
            g_cEngineOption.fMinSoundRange, 
            DS3D_DEFAULTMAXDISTANCE, 
            &dwDummyHandle);
    }
    hResult = S_OK;
Exit0:
    return hResult;
}

