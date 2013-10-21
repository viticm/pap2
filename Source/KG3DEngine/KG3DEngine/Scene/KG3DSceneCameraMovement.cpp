//*******************************************************************************
/*关于暴露到interface的IKG3DSceneCameraKeyFrame，IKG3DSceneCameraAnimation，IKG3DSceneCameraMovement
后者分别是前者的容器，用Movement控制当前的Animation（SetCurCamAnimation），然后得到这个Animation
的指针，用Animation的StartPlay开始播放，StopPlay停止播放，在Start之后，Stop之前，可以用PausePlay
暂停播放。第一次PausePlay的话，可以得到一个KeyFrame，这个KeyFrame是暂停时的位置，第二次PausePlay
，播放从暂停的位置开始。所以要流畅的接上整个播放的话，就要用暂停时得到的位置做文章了。
关键帧需要在编辑器中编辑，编辑好之后可以存成CamAni结尾的文件，这个文件中包含了复数的CameraAnimation
使用Movement的LoadFromFile可以多次载入CamAni文件（每次载入都会Clear掉以前的CamAni
），从而选择不同的CamAni进行播放。编辑器中添加了一个连接到第一帧的功能，可以连成循环无缝的动画
*/
//*******************************************************************************
#include "stdafx.h"
#include "KG3DSceneCameraMovement.h"
#include "KG3DCamera.h"
#include "kg3dgraphicstool.h"
#include ".\kg3dsceneentity.h"
#include "Helpers\KGStringFunctionsBase.h"
#include "IEKG3DScene.h"
#include "KG3DRepresentObjectTable.h"
#include "KG3DRepresentObjectDummy.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
//KG3DSceneEntityDummyExecutor<KG3DSceneEntityDummy> KG3DSceneCameraAnimation::m_stKG3DSceneEntityDummyExecutor;

static FLOAT stfTangentScale  = 10.0f;//用于控制切线的长度比例。切线不缩短点很难操作

struct KG3DSceneEntityExecuterCamAniKeyFrame : public KG3DSceneEntityExecutor
{
	typedef KG3DSceneCameraAnimation::CameraKeyFrameForSelection TypeKeyForSel;

	virtual DWORD GetType() const		{return SCENEENTITY_CAM_ANI_KEY;}
	virtual KG3DSceneEntityExecutor* Clone() const	{return new KG3DSceneEntityExecuterCamAniKeyFrame;}
	virtual HRESULT GetBBox(KG3DSceneEntity& Entity, AABBOX* pOutBox)
	{
		TypeKeyForSel* pKey = reinterpret_cast<TypeKeyForSel*>(Entity.m_pPointer);
		_ASSERTE(NULL != pKey);
		D3DXVECTOR3 vSize(5,5,5);
		*pOutBox = AABBOX(pKey->m_vVecPos-vSize, pKey->m_vVecPos+vSize);
		return S_OK;
	}
	virtual HRESULT RenderSelectMark(KG3DSceneEntity& Entity, DWORD dwColor)
	{
		TypeKeyForSel* pKey = reinterpret_cast<TypeKeyForSel*>(Entity.m_pPointer);	
		return g_cGraphicsTool.DrawBoxAtPos(pKey->m_vVecPos, 15, 0xFF1122FF, FALSE);
	}
	virtual HRESULT OnSelect(KG3DSceneEntity& Entity)
	{
		TypeKeyForSel* pKey = reinterpret_cast<TypeKeyForSel*>(Entity.m_pPointer);	
		_ASSERTE(NULL != pKey);
		pKey->m_pAnimation->OnKeyFrameSelected(pKey);
		return S_OK;
	}
	virtual HRESULT SetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue)
	{
		if(NULL == pValue)
			return E_FAIL;
		TypeKeyForSel* pKey = reinterpret_cast<TypeKeyForSel*>(Entity.m_pPointer);	
		_ASSERTE(NULL != pKey);
		pKey->m_pAnimation->OnKeyFrameTranslate(pKey, pValue);
		return S_OK;
	}
	/*virtual HRESULT SetRotation(KG3DSceneEntity& Entity, D3DXQUATERNION* pValue, D3DXVECTOR3* pValueXYZ)
	{
		if(NULL == pValue)
			return E_FAIL;
		TypeKeyForSel* pKey = reinterpret_cast<TypeKeyForSel*>(Entity.m_pPointer);	
		_ASSERTE(NULL != pKey);
		pKey->m_pAnimation->OnKeyFrameRotation(pKey, pValue);
		return S_OK;
	}*/
	virtual HRESULT GetTranslation(KG3DSceneEntity& Entity, D3DXVECTOR3* pValue)
	{
		if(NULL == pValue)
			return E_FAIL;
		TypeKeyForSel* pKey = reinterpret_cast<TypeKeyForSel*>(Entity.m_pPointer);
		_ASSERTE(NULL != pKey);
		*pValue = pKey->m_vVecPos;
		return S_OK;
	}
};
extern KG3DSceneEntityExecutorFactory g_KG3DSceneEntityExecutorFactory;

KG3DSceneCameraMovement::KG3DSceneCameraMovement(const IEKG3DScene& Scene)
	:m_SceneRef(const_cast<IEKG3DScene&>(Scene))
{
	m_pCurCameraAnimation = NULL;
	m_nNewCamAniTimes = 0;
	NewOneCamAnimation();
	ASSERT( 1 == m_vecCameraAnimationTable.size() );

	//注册Executer，重复注册没有所谓
	g_KG3DSceneEntityExecutorFactory.RegisterExecutor(KG3DSceneEntityExecuterCamAniKeyFrame());
}

KG3DSceneCameraMovement::~KG3DSceneCameraMovement(void)
{
	m_pCurCameraAnimation->StopPlay();
#if defined(_DEBUG) | defined(DEBUG)
	HRESULT hr =
#endif
		this->Clear();
	ASSERT( SUCCEEDED( hr ) );
}
///严禁使用拷贝运算符
void KG3DSceneCameraMovement::operator=( KG3DSceneCameraMovement &source) 
{
	NULL;
}
	

///返回当前摄像机动画的指针，如果是无效指针的话，会保证至少返回一个新的Animation。因为这个方法可以保证不会出现无效指针，所以除了速度要求高的帧循环函数之外，绝对不要直接把这个函数的返回值复制使用。
HRESULT KG3DSceneCameraMovement::GetCurCamAnimation(
    IEKG3DSceneCameraAnimation **ppiRetCameraAnimation
)
{
    HRESULT hResult = E_FAIL;
    KG_PROCESS_ERROR(ppiRetCameraAnimation);
    *ppiRetCameraAnimation = this->GetCurCamAnimation();
	return S_OK;
Exit0:
    return hResult;
}
KG3DSceneCameraAnimation* KG3DSceneCameraMovement::GetCurCamAnimation()
{
	if (! m_pCurCameraAnimation )
	{
		if (m_vecCameraAnimationTable.empty())
		{
			NewOneCamAnimation();	///新建一个CamAni的话，当前动画指针也会相应的指向新的
		}
		else
		{
			m_pCurCameraAnimation = m_vecCameraAnimationTable[0];
		}
	}	
	ASSERT( NULL != m_pCurCameraAnimation );
	return m_pCurCameraAnimation;
}

HRESULT KG3DSceneCameraMovement::GetCurCamAnimation( IKG3DSceneCameraAnimation **ppiRetCameraAnimation )
{
	HRESULT hResult = E_FAIL;
	KG_PROCESS_ERROR(ppiRetCameraAnimation);
	*ppiRetCameraAnimation = this->GetCurCamAnimation();
	return S_OK;
Exit0:
	return hResult;
}
///设定当前摄像机动画
HRESULT	KG3DSceneCameraMovement::SetCurCamAnimation( int nIndex )
{
	ASSERT( m_pCurCameraAnimation );

    BOOL bIsPlaying = FALSE;
    m_pCurCameraAnimation->GetPlayState(&bIsPlaying);
	KG_PROCESS_ERROR(! bIsPlaying);

	
	{
		m_pCurCameraAnimation->EnableEditCamAniKeyFrame( FALSE );//先Remove掉关键帧编辑。
		
		KG_PROCESS_ERROR( nIndex>= 0 && nIndex< (int)m_vecCameraAnimationTable.size() );
		m_pCurCameraAnimation = m_vecCameraAnimationTable.at( nIndex );
	}
	return S_OK;
Exit0:
	return E_FAIL;

}

///新建一动画
HRESULT	KG3DSceneCameraMovement::NewOneCamAnimation()
{
	KG3DSceneCameraAnimation* pCamAni = NULL;
	
	if (m_pCurCameraAnimation )
	{
        BOOL bIsPlaying = FALSE;
        m_pCurCameraAnimation->GetPlayState(&bIsPlaying);
		KG_PROCESS_ERROR(! bIsPlaying);

		m_pCurCameraAnimation->EnableEditCamAniKeyFrame( FALSE );//先Remove掉关键帧编辑。
	}
	//_KG3D_DEBUG_FAIL_REPORT( _CrtCheckMemory() );
	pCamAni = new KG3DSceneCameraAnimation(m_SceneRef);
	//_KG3D_DEBUG_FAIL_REPORT( _CrtCheckMemory() );
	
	if (pCamAni)
	{
		///命名
		char cTempName[50];	
		sprintf_s( cTempName, _T("CameraAnimation_%d"), m_nNewCamAniTimes );
        pCamAni->SetName(cTempName);
		///End命名

		m_vecCameraAnimationTable.push_back( pCamAni );
		m_pCurCameraAnimation = pCamAni;

		m_nNewCamAniTimes++;	///成功增加动画了，加1
		return S_OK;
	}
Exit0:
	SAFE_DELETE(pCamAni);
	return E_FAIL;
}
///移除index动画，如果不加上参数的话，就是最后一个，如果碰到当前的动画指针就是要删除的那个的话，移动。
HRESULT	KG3DSceneCameraMovement::RemoveOneCamAnimation( int nIndex )
{
	KG_PROCESS_ERROR(m_pCurCameraAnimation);
	
	{
		BOOL bIsPlaying = FALSE;
		m_pCurCameraAnimation->GetPlayState(&bIsPlaying);
		if (bIsPlaying)
		{
			return E_FAIL;
		}

		m_pCurCameraAnimation->EnableEditCamAniKeyFrame( FALSE );//先Remove掉关键帧编辑。

		if ( -1 == nIndex )
		{
			nIndex = ((int)m_vecCameraAnimationTable.size())-1;	///如果不加参数的话，就删除最后一个
		}
		KG_PROCESS_ERROR( nIndex > -1 && nIndex < (int)m_vecCameraAnimationTable.size() );
		if ( m_pCurCameraAnimation == m_vecCameraAnimationTable[ nIndex ] )
		{
			if ( nIndex >= 1 )
			{
				m_pCurCameraAnimation = m_vecCameraAnimationTable[ nIndex-1 ];///前面还有的话，位置前移
			}
			else if( 0 == nIndex )
			{
				if ( m_vecCameraAnimationTable.size() > 1 )
				{
					///位置后移
					m_pCurCameraAnimation = m_vecCameraAnimationTable[ 1 ];
				}
				else
				{
					NewOneCamAnimation();	///会删除光的话，重新加一个新的。
				}			 
			}		
		}
		///在动画列表中删除指定的Index动画
		vector<KG3DSceneCameraAnimation*>::iterator it = m_vecCameraAnimationTable.begin();
		delete m_vecCameraAnimationTable[ nIndex ];
		m_vecCameraAnimationTable.erase( it+nIndex );
		ASSERT( NULL != m_pCurCameraAnimation );
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DSceneCameraMovement::RemoveCurCamAnimation()
{
	if ( NULL == m_pCurCameraAnimation )
	{
		return E_FAIL;
	}
    BOOL bIsPlaying = FALSE;
    m_pCurCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		return E_FAIL;
	}
	m_pCurCameraAnimation->EnableEditCamAniKeyFrame( FALSE );//先Remove掉关键帧编辑。
    DWORD dwCurCamAniIndex = 0;
    GetCurCamAniIndex(&dwCurCamAniIndex);
	HRESULT hr = RemoveOneCamAnimation(dwCurCamAniIndex);
	return hr;
}
///用名字找到vector中索引,nFinded表示第几个找到的，默认是1
int KG3DSceneCameraMovement::FindCamAniByName( LPCTSTR pstrCamAniName, int nFinded )
{
	ASSERT( pstrCamAniName );
	KG3DSceneCameraAnimation* pCamAni;
	int nIndex = -1;
	int nFindCount = 0;
    int nRetCode = 0;
	vector<KG3DSceneCameraAnimation*>::iterator it;
	for ( it = m_vecCameraAnimationTable.begin(), nIndex = 0, nFindCount = 0
		; it != m_vecCameraAnimationTable.end()
		; it++, nIndex++ 
		)
	{
        pCamAni = (*it);
		const std::string& szName = pCamAni->GetName();
        nRetCode = strcmp(szName.c_str(), pstrCamAniName);
		if (nRetCode == 0)
		{
			nFindCount += 1;
			if ( nFindCount == nFinded )
				{
					return nIndex;
				}												
		}
	}
	return -1;
}
HRESULT KG3DSceneCameraMovement::FindCamAniByName(LPCTSTR pstrCamAniName, int nFinded, int* nRetIndex)
{
	KG_PROCESS_ERROR(pstrCamAniName&&nRetIndex);
	{
		int nTempResult = FindCamAniByName(pstrCamAniName, nFinded);
		KG_PROCESS_ERROR(nTempResult != -1);
		*nRetIndex = nTempResult;
		return S_OK;
	}
Exit0:
	return E_FAIL;
}
///移除所有的CameraAnimation
HRESULT KG3DSceneCameraMovement::Clear()
{
	ASSERT( m_pCurCameraAnimation );

    BOOL bIsPlaying = FALSE;
    m_pCurCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		return E_FAIL;
	}
	KG3DSceneCameraAnimation* pCamAni;
	vector<KG3DSceneCameraAnimation*>::iterator it;
	for ( it = m_vecCameraAnimationTable.begin(); it != m_vecCameraAnimationTable.end(); it++ )
	{
		pCamAni = *it;
		ASSERT( NULL != pCamAni );
       	///其实不是很需要，析构的时候会自动调用
		pCamAni->EnableEditCamAniKeyFrame( FALSE );//先Remove掉关键帧编辑。
		delete pCamAni;
	}
	m_vecCameraAnimationTable.clear();
	m_pCurCameraAnimation = NULL;
	if (m_vecCameraAnimationTable.empty())
	{
		return S_OK;
	}
	return E_FAIL;
}
///得到内部动画个数，如果返回0的话就知道内部为空
HRESULT KG3DSceneCameraMovement::GetAnimationCount(DWORD *pdwRetAnimationCount)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwRetAnimationCount);

    *pdwRetAnimationCount = (DWORD)m_vecCameraAnimationTable.size();

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

///把当前动画在表中上移
HRESULT	KG3DSceneCameraMovement::LetCurCamAniPosMoveUp()
{
    HRESULT hRetCode = E_FAIL;
    DWORD dwCurCamAniIndex = 0;
    hRetCode = GetCurCamAniIndex(&dwCurCamAniIndex);
	if (FAILED(hRetCode))
	{
		return E_FAIL;		///没有Index，不用移动了
	}
	if ( 0 == dwCurCamAniIndex )	///最前面无需前移
	{
		return S_OK;
	}
	
	KG3DSceneCameraAnimation* pCamAni = m_vecCameraAnimationTable[ dwCurCamAniIndex - 1 ];
	m_vecCameraAnimationTable[ dwCurCamAniIndex -1 ] = m_vecCameraAnimationTable[ dwCurCamAniIndex ];
	m_vecCameraAnimationTable[ dwCurCamAniIndex ] = pCamAni;
	ASSERT( m_pCurCameraAnimation == m_vecCameraAnimationTable[ dwCurCamAniIndex -1 ] );
	return S_OK;
}

///把当前动画在表中下移
HRESULT KG3DSceneCameraMovement::LetCurCamAniPosMoveDown()
{
    HRESULT hRetCode = E_FAIL;
    DWORD dwCurCamAniIndex = 0;
    hRetCode = GetCurCamAniIndex(&dwCurCamAniIndex);
    if (FAILED(hRetCode))
	{
		return E_FAIL;		///没有Index，不用移动了
	}
	if ( (m_vecCameraAnimationTable.size() - 1 ) == dwCurCamAniIndex )	///最后面面无需后移
	{
		return S_OK;
	}

	KG3DSceneCameraAnimation* pCamAni = m_vecCameraAnimationTable[ dwCurCamAniIndex + 1 ];
	m_vecCameraAnimationTable[ dwCurCamAniIndex + 1 ] = m_vecCameraAnimationTable[ dwCurCamAniIndex ];
	m_vecCameraAnimationTable[ dwCurCamAniIndex ] = pCamAni;
	ASSERT( m_pCurCameraAnimation == m_vecCameraAnimationTable[ dwCurCamAniIndex + 1 ] );
	return S_OK;
}
HRESULT	KG3DSceneCameraMovement::SetCurCamAniName( LPCTSTR pCamAniName )
{
	if ( NULL == m_pCurCameraAnimation )
	{
		return E_FAIL;
	}

	if ( NULL == pCamAniName )
	{
		return E_FAIL;
	}
	const std::string& szOldName = m_pCurCameraAnimation->GetName();
	if ( 0 == _tcscmp(szOldName.c_str(), pCamAniName ) )
	{
		return S_OK;
	}
	if ( -1 != this->FindCamAniByName( pCamAniName ) )	///有重名的
	{
		return E_FAIL;
	}
	else
	{
        m_pCurCameraAnimation->SetName(pCamAniName);
		return S_OK;
	}
	return E_FAIL;
			
}
HRESULT KG3DSceneCameraMovement::GetCurCamAniIndex(DWORD *pdwRetAnimationIndex)
{
    HRESULT hResult  = E_FAIL;
    vector<KG3DSceneCameraAnimation*>::iterator it;

    KGLOG_PROCESS_ERROR(pdwRetAnimationIndex);
    
    it = find( 
        m_vecCameraAnimationTable.begin()
        , m_vecCameraAnimationTable.end()
        , m_pCurCameraAnimation 
        );
    if( m_vecCameraAnimationTable.end() == it )
    {
        return E_FAIL;
    }

    *pdwRetAnimationIndex = (DWORD)(it - m_vecCameraAnimationTable.begin());
    ASSERT((*pdwRetAnimationIndex) < m_vecCameraAnimationTable.size() );

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneCameraMovement::SaveToFile(LPSTR pFileName)
{
	HRESULT hr;
	FILE* pFile = fopen(pFileName,"wb");
	if(!pFile)
		return E_FAIL;
	
	DWORD dwType = 0xABFF0056;
	DWORD dwVersion = 1;
	DWORD dwNumAni = (DWORD)m_vecCameraAnimationTable.size();		///有可能在保存中改掉CamAni的数目所以要小心
	fwrite(&dwType,sizeof(DWORD),1,pFile);
	fwrite(&dwVersion,sizeof(DWORD),1,pFile);
	fwrite(&dwNumAni,sizeof(DWORD),1,pFile);
	//////////////////////////////////////////////////////////////////////////
	vector<KG3DSceneCameraAnimation*>::iterator it;
	KG3DSceneCameraAnimation* pCamAni;
	for ( it = m_vecCameraAnimationTable.begin(); it != m_vecCameraAnimationTable.end(); it++ )
	{
		pCamAni = *it;
		hr = pCamAni->SaveToFile(pFile);
		if ( FAILED( hr ) )
		{
			break;
		}
	}	
	//////////////////////////////////////////////////////////////////////////

	fclose(pFile);
	return S_OK;
}

HRESULT KG3DSceneCameraMovement::LoadFromFile(LPSTR pFileName)
{
	HRESULT hr = E_FAIL;
    BOOL bIsPlaying = FALSE;
    IFile* pFile = NULL;

    m_pCurCameraAnimation->GetPlayState(&bIsPlaying);
    if (bIsPlaying)
	{
		return E_FAIL;
	}
	
	KG_COM_PROCESS_ERROR(this->Clear());
	{
        unsigned long uSize = 0;

		pFile = g_OpenFile(pFileName);
		if (!pFile)
			return E_FAIL;

		DWORD dwType = 0xABFF0056;
		DWORD dwVersion = 1;
		DWORD dwNumAni = 1;
		
        uSize = pFile->Read(&dwType, sizeof(DWORD));
        KGLOG_PROCESS_ERROR(uSize == sizeof(DWORD));

		uSize = pFile->Read(&dwVersion, sizeof(DWORD));
        KGLOG_PROCESS_ERROR(uSize == sizeof(DWORD));

        uSize = pFile->Read(&dwNumAni, sizeof(DWORD));
        KGLOG_PROCESS_ERROR(uSize == sizeof(DWORD));

        //////////////////////////////////////////////////////////////////////////
		//
		for ( int i = 0; i < (int)dwNumAni; i++ )
		{
			if ( 0 == i )	///读入第一个的话，读入到当前CamAni
			{
				hr = GetCurCamAnimation()->LoadFromFile(pFile);
			}
			else
			{
				hr = NewOneCamAnimation();
				ASSERT( SUCCEEDED(hr) );
				hr = GetCurCamAnimation()->LoadFromFile(pFile);
			}
			if ( FAILED( hr ) )
			{
				break;
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}
	hr = S_OK;
Exit0:
    KG_COM_RELEASE(pFile);
	return hr;
}

HRESULT KG3DSceneCameraMovement::SetCurCamAnimationByName( LPCTSTR pstrCamAniName, int nFinded, IKG3DSceneCameraAnimation **ppiRetCameraAnimation )
{
	int nFindedIndex = -1;
	KG_COM_PROCESS_ERROR(FindCamAniByName(pstrCamAniName, nFinded, &nFindedIndex));
	{
		KG_PROCESS_ERROR(nFindedIndex >=0 && nFindedIndex < (int)m_vecCameraAnimationTable.size());
		KG_COM_PROCESS_ERROR(SetCurCamAnimation(nFindedIndex));
		{
			if (ppiRetCameraAnimation)
			{
				*ppiRetCameraAnimation = GetCurCamAnimation();
			}
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
/************************************************************************/
/*开始KG3DSceneCameraAnimation的函数定义
/************************************************************************/

KG3DSceneCameraAnimation::KG3DSceneCameraAnimation(IEKG3DScene& Scene)
:m_vecPosCenter(0, 0, 0)
,m_CallbackKeySelChange(NULL)
,m_SceneRef(Scene)
{
	m_fAngle = 0.0F;
	m_fPlaySpeed = 1.0F;
	m_dwPlayMethod = 0;
	m_dwLastKeyFrameTime = 0;
	m_dwAnimationStartTime = 0;	
	m_nRenderCount = 0;	
	m_bIsPlaying = false;
	m_szName = "KG3DSceneCameraAnimation";
	m_bIsTangentComputed = false;	///改变了关键帧的话，要重新算Tangent
	m_bIsKeyEditable = false;
	/*m_plistRender = NULL;	
	m_plistSelectable = NULL;
	m_plistSelected = NULL;*/
	m_bIsPaused = false; 			///表示动画是否在暂停状态
	m_dwAnimationPauseTime = 0;
	m_dwAnimationPauseStartTime = 0;
	m_bLoopWhilePlaying = false;
	_ASSERTE(m_szName.length() < max_name_length-1);
	//m_uSelectedKeyIndex = 0;
	m_dwSelectedKeyTime = 0;
}

KG3DSceneCameraAnimation::~KG3DSceneCameraAnimation()
{
	this->StopPlay();
	this->EnableEditCamAniKeyFrame( FALSE );
	
	/*//Container中不再用指针，而是直接放 Last modify by Chen Tianhong: 2007-8-14 15:03:06

	TypeKeyContainer::iterator i =
			m_mapCameraKeyFrame.begin();
		while(i!=m_mapCameraKeyFrame.end())
		{
			CameraKeyFrame* pKeyFrame = i->second;
			SAFE_DELETE(pKeyFrame);
			i++;
		}*/
	
	m_mapCameraKeyFrame.clear();

	KG_CUSTOM_HELPERS::TContainerRelease(m_vecObjectsInTheScene);
}
void KG3DSceneCameraAnimation::operator=( KG3DSceneCameraAnimation &source)
{
	NULL;	
}
DWORD   KG3DSceneCameraAnimation::GetAnimationTotalTime()
{
	// 这里的速度是距离每时间，而不是骨骼动画的播放倍速;
	return (DWORD)(m_fDistanceTotal / m_fPlaySpeed);
}

///以fKeyFrameTime为时间加入已存在的关键帧，如果fKeyFrameTime传默认值的化会自动增加时间
HRESULT KG3DSceneCameraAnimation::AddKeyFrame( CameraKeyFrame* pKey, DWORD dwKeyFrameTime )
{
	KG_PROCESS_ERROR(! m_bIsPlaying && ! m_bIsKeyEditable);///可以编辑关键帧的话不能添加和删除帧
	if ( dwKeyFrameTime > 0 )
	{
		KG_PROCESS_ERROR(dwKeyFrameTime > m_dwLastKeyFrameTime);//只能增加
		m_dwLastKeyFrameTime = dwKeyFrameTime;
	}
	else
	{
		if (m_mapCameraKeyFrame.empty())
			m_dwLastKeyFrameTime = 0;	///第一帧
		else
		{ 
			m_dwLastKeyFrameTime += 3000;	///3秒
		}
	}
	RemoveKeyFrame( m_dwLastKeyFrameTime );
	//pKey->fTime = m_dwLastKeyFrameTime;
	m_mapCameraKeyFrame[ m_dwLastKeyFrameTime ] = *pKey;
	
	m_bIsTangentComputed = false;	///增加了关键帧的话，要重新算Tangent
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneCameraAnimation::AddKeyFrameToHeadFramePosition()
{
	KG_PROCESS_ERROR (! m_mapCameraKeyFrame.empty());
	{
		TypeKeyContainer::iterator it = m_mapCameraKeyFrame.begin();
		return AddKeyFrame(&it->second);	//注意时间还是应该是自动的
	}	
Exit0:
	return E_FAIL;
}

///以dwKeyFrameTime为时间，获取pCamera的位置和焦点位置创建关键帧，并把结果的指针输出到pKeyOut，如果dwKeyFrameTime传默认值的化会自动增加时间

HRESULT KG3DSceneCameraAnimation::AddKeyFrame(
    KG3DCamera* pCamera, DWORD dwKeyFrameTime /* = 0 */, CameraKeyFrame* pKeyOut /* = NULL */
)
{
	CameraKeyFrame Key;
	pCamera->GetPosition(&Key.vPosition);
	pCamera->GetLookAtPosition(&Key.vLookAt);
	Key.vPositionTangent = Key.vPosition + D3DXVECTOR3(10,0,10);
	Key.vLookAtTangent = Key.vLookAt + D3DXVECTOR3(10,0,10);
	
	KG_COM_PROCESS_ERROR(this->AddKeyFrame(&Key, dwKeyFrameTime));

	if (pKeyOut )
		*pKeyOut = Key;	///返回已经添加的指针。
	
	return S_OK;
Exit0:
	return E_FAIL;
}

///用当前Camera和当前时间添加KeyFrame
HRESULT KG3DSceneCameraAnimation::AddKeyFrame()
{
	KG_PROCESS_ERROR(! m_bIsPlaying && ! m_bIsKeyEditable);///可以编辑关键帧的话不能添加和删除帧
	{
		KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
		KG_PROCESS_ERROR(pCamera);
		KG_COM_PROCESS_ERROR(AddKeyFrame(pCamera));	
		m_bIsTangentComputed = false;	///增加了关键帧的话，要重新算Tangent
		return S_OK;
	}
Exit0:
	return E_FAIL;
}
///移除nKey时间的关键帧，如果取0值的话，删除最近的一帧
HRESULT KG3DSceneCameraAnimation::RemoveKeyFrame(DWORD dwKeyFrameTime)
{
	KG_PROCESS_ERROR(!m_bIsPlaying&& ! m_bIsKeyEditable);///可以编辑关键帧的话不能添加和删除帧
	KG_PROCESS_SUCCESS(m_mapCameraKeyFrame.empty());
	if ( dwKeyFrameTime == 0 )	///取默认值的情况，删除最后一帧
	{
		TypeKeyContainer::iterator it = m_mapCameraKeyFrame.end();
		it--;
		TypeKeyContainer::iterator itPrev = it;
		if (it != m_mapCameraKeyFrame.begin())
		{
			--itPrev;
			m_dwLastKeyFrameTime = itPrev->first;
		}
		else
		{
			m_dwLastKeyFrameTime = 0;			
		}
		m_mapCameraKeyFrame.erase( it ) ;
	}
	else
	{
		TypeKeyContainer::iterator it = m_mapCameraKeyFrame.find( dwKeyFrameTime );
		if(it!=m_mapCameraKeyFrame.end())
		{
			m_mapCameraKeyFrame.erase( it );
		}

	}
	m_bIsTangentComputed = false;	///改变了关键帧的话，要重新算Tangent
Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}
///渲染关键帧和曲线
HRESULT KG3DSceneCameraAnimation::RenderCameraKeyFrame(DWORD dwOption)
{
	D3DXVECTOR3 vPos(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vEye(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vPosTan(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vEyeTan(0.0f, 0.0f, 0.0f);
	
	if(1)
	{
		TypeKeyContainer::iterator it = m_mapCameraKeyFrame.begin();
		while(it!=m_mapCameraKeyFrame.end())
		{
			CameraKeyFrame* pKeyFrame = &(it->second);

			/*DWORD dwPosColor = 0xFFFFFF00, dwLookAtColor = 0xFF00FF00;
			if (it->first == m_dwSelectedKeyTime)
			{
				dwPosColor = 0xFFFF0000;
				dwLookAtColor = 0xFF33FF00;
			}
			
			AABBOX BoxPos;
			BoxPos.AddPosition(pKeyFrame->vPosition-D3DXVECTOR3(8,8,10));
			BoxPos.AddPosition(pKeyFrame->vPosition+D3DXVECTOR3(8,8,10));
			g_cGraphicsTool.DrawAABBox(&BoxPos,dwPosColor);*/
			g_cGraphicsTool.DrawLine(&pKeyFrame->vPosition,
				&pKeyFrame->vPositionTangent,
				0xFFFFFF00,0xFFFFFF00);

			/*BoxPos.Clear();
			BoxPos.AddPosition(pKeyFrame->vLookAt -D3DXVECTOR3(8,8,10));
			BoxPos.AddPosition(pKeyFrame->vLookAt+D3DXVECTOR3(8,8,10));
			g_cGraphicsTool.DrawAABBox(&BoxPos,dwLookAtColor);*/
			g_cGraphicsTool.DrawLine(&pKeyFrame->vLookAt,
				&pKeyFrame->vLookAtTangent,
				0xFF00FF00,0xFF00FF00);

			if(it!=m_mapCameraKeyFrame.begin() && m_bIsTangentComputed)
			{
				g_cGraphicsTool.DrawHermiteSpline(&vPos,&((vPosTan - vPos)*stfTangentScale),
					&pKeyFrame->vPosition,&((pKeyFrame->vPositionTangent-pKeyFrame->vPosition)*stfTangentScale),
					0xFFFFFF00,0xFFFFFFFF,16);
				g_cGraphicsTool.DrawHermiteSpline(&vEye,&((vEyeTan-vEye)*stfTangentScale),
					&pKeyFrame->vLookAt,&((pKeyFrame->vLookAtTangent-pKeyFrame->vLookAt)*stfTangentScale),
					0xFF00FF00,0xFFFFFFFF,16);
			}

			vPos = pKeyFrame->vPosition;
			vEye = pKeyFrame->vLookAt;
			vPosTan = pKeyFrame->vPositionTangent;
			vEyeTan = pKeyFrame->vLookAtTangent;
			it++;
		}
	}
	else
	{
		for (size_t i=0;i<m_vecTineLine.size();i++)
		{
			TinyLine Line = m_vecTineLine[i];
			g_cGraphicsTool.DrawLine(&Line.vLookAtA,&Line.vLookAtB,0xFF00FF00,0xFFFFFFFF);
			g_cGraphicsTool.DrawLine(&Line.vPositionA,&Line.vPositionB,0xFFFFFF00,0xFFFFFFFF);
		}
	}
	
	return S_OK;
}

HRESULT KG3DSceneCameraAnimation::TinyLine::GetDataByTinyLine(CameraKeyFrame* pValue,float fDistance)
{
	float S = (fDistance - fDistanceStart) / this->fDistance;
	pValue->vLookAt = vLookAtA + (vLookAtB - vLookAtA) * S;
	pValue->vPosition = vPositionA + (vPositionB - vPositionA) * S;
	return S_OK;
}

HRESULT KG3DSceneCameraAnimation::GetDataByTinyLine(CameraKeyFrame* pValue, DWORD dwTime)
{
	if(m_vecTineLine.size() == 0)
		return E_FAIL;

	size_t nIndex = 0;
	float fDistance = m_fPlaySpeed * dwTime;
	
	if ((fDistance <= 0) || (fDistance > m_fDistanceTotal))
		return E_FAIL;

	nIndex = (size_t)(fDistance / m_fDistanceTotal * m_vecTineLine.size());
	if(nIndex>= m_vecTineLine.size())
		nIndex = m_vecTineLine.size() -1;

	TinyLine vTinyLine;
	vTinyLine = m_vecTineLine[nIndex];

	if (fDistance < vTinyLine.fDistanceStart)
	{
		for (size_t i=nIndex;i>=0;i--)
		{
			vTinyLine = m_vecTineLine[i];
			if ((fDistance >= vTinyLine.fDistanceStart)&&(fDistance <= vTinyLine.fDistanceEnd))
			{
				return vTinyLine.GetDataByTinyLine(pValue, fDistance);
			}
		}
	}
	else if (fDistance > vTinyLine.fDistanceEnd)
	{
		for (size_t i=nIndex;i<m_vecTineLine.size();i++)
		{
			vTinyLine = m_vecTineLine[i];
			if ((fDistance >= vTinyLine.fDistanceStart)&&(fDistance <= vTinyLine.fDistanceEnd))
			{
				return vTinyLine.GetDataByTinyLine(pValue, fDistance);
			}
		}
	}
	else
	{
		return vTinyLine.GetDataByTinyLine(pValue,fDistance);
	}
	return E_FAIL;
}

///以dwTime为时间的到一个关键帧结构体，实际上是两个关键帧之间的插值。
HRESULT KG3DSceneCameraAnimation::GetData(CameraKeyFrame* pValue, DWORD dwTime)
{
	TypeKeyContainer::iterator i_Down;
	TypeKeyContainer::iterator i_Up;
	CameraKeyFrame* A,*B;
	DWORD dwTimeUp,dwTimeDown;

	if(m_mapCameraKeyFrame.empty())
		return E_FAIL;
	if(m_mapCameraKeyFrame.size()==1)
	{
		i_Up = m_mapCameraKeyFrame.begin();
		(*pValue) = i_Up->second;
		return S_OK;
	}
	else
	{
		i_Down = m_mapCameraKeyFrame.upper_bound(dwTime);
		if(i_Down==m_mapCameraKeyFrame.end())
		{
			i_Up = i_Down;
			i_Up--;

			(*pValue) = i_Up->second;
			return S_OK;
		}
		else if(i_Down==m_mapCameraKeyFrame.begin())
		{
			(*pValue) = i_Down->second;
			return S_OK;
		}
		else
		{
			i_Up = i_Down;
			i_Up--;

			A = &i_Up->second;
			B = &i_Down->second;
			dwTimeUp = i_Up->first;
			dwTimeDown = i_Down->first;
			if (dwTimeUp >= dwTimeDown)	///时间距离不能过小
			{
				return E_FAIL;
			}

			float S = (float)(dwTime - dwTimeUp) / (float)(dwTimeDown - dwTimeUp);
			D3DXVec3Hermite(&pValue->vLookAt,
				&A->vLookAt,
				&((A->vLookAtTangent-A->vLookAt)*stfTangentScale),
				&B->vLookAt,
				&((B->vLookAtTangent-B->vLookAt)*stfTangentScale),
				S);
			D3DXVec3Hermite(&pValue->vPosition,
				&A->vPosition,
				&((A->vPositionTangent-A->vPosition)*stfTangentScale),
				&B->vPosition,
				&((B->vPositionTangent-B->vPosition)*stfTangentScale),
				S);
			return S_OK;
		}
	}
	return S_OK;
}

BOOL KG3DSceneCameraAnimation::IsFinished(DWORD dwNowTime)
{
    BOOL bResult = FALSE;
    if (!m_bIsPaused)
    {
        DWORD dwPassedTime = dwNowTime - m_dwAnimationPauseTime - m_dwAnimationStartTime;
        if (dwPassedTime >= m_dwLastKeyFrameTime)
            bResult = TRUE;
    }
    return bResult;
}

HRESULT KG3DSceneCameraAnimation::UpdateCameraByTime(KG3DCamera* pCamera)
{
    HRESULT hResult = E_FAIL;
    CameraKeyFrame KeyFrame;
    DWORD dwTimePassed = 0;
    DWORD dwNowTime = g_cGraphicsTool.GetNowTime();

    KG_PROCESS_ERROR(NULL != pCamera);
    KG_PROCESS_ERROR(m_bIsPlaying);
    KG_PROCESS_SUCCESS(m_bIsPaused);
    ///先把暂停的时间砍掉

    ASSERT(dwNowTime >= (m_dwAnimationPauseTime + m_dwAnimationStartTime));

    dwTimePassed = dwNowTime - m_dwAnimationPauseTime - m_dwAnimationStartTime;

    {
        if(SUCCEEDED(GetData(&KeyFrame, dwTimePassed)))
        {
            D3DXVECTOR3 vecUpDir,vecLookatDir,vecLookAtPos,vecLeftDir,vecRealUpDir;
            D3DXMATRIX matRotion;
            vecUpDir = D3DXVECTOR3(0,1,0);
            vecLookatDir = KeyFrame.vLookAt - KeyFrame.vPosition;
            D3DXVec3Normalize(&vecLookatDir,&vecLookatDir);
            D3DXVec3Normalize(&vecUpDir,&vecUpDir);
            D3DXVec3Cross(&vecLeftDir,&vecUpDir,&vecLookatDir);
            D3DXVec3Normalize(&vecLeftDir,&vecLeftDir);
            D3DXVec3Cross(&vecRealUpDir,&vecLookatDir,&vecLeftDir);
            D3DXVec3Normalize(&vecRealUpDir,&vecRealUpDir);
            D3DXMatrixRotationAxis(
                &matRotion,
                &vecRealUpDir,
                m_fAngle
                );

            D3DXVec3TransformNormal(
                &vecLookatDir,
                &vecLookatDir,
                &matRotion
                );
            vecLookAtPos =  KeyFrame.vPosition + 100 * vecLookatDir;
            pCamera->SetPositionLookAtUp(&KeyFrame.vPosition, &vecLookAtPos, &vecRealUpDir);
        }
    }

Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

///刷新Camera的位置。使用之前必须保证使用了StartPlay方法，否则无效，然后播放结束的时候必须调用StopPlay方法。
HRESULT KG3DSceneCameraAnimation::UpdateCamera(KG3DCamera* pCamera)
{
	KG_PROCESS_ERROR(NULL != pCamera);
	{
		if (!m_bIsPlaying )
		{
			return E_FAIL;
		}
		if (m_bIsPaused)
		{
			return S_OK;
		}
		CameraKeyFrame KeyFrame;
		DWORD dwTimePassed = 0;
        DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
		///先把暂停的时间砍掉

        ASSERT(dwNowTime >= (m_dwAnimationPauseTime + m_dwAnimationStartTime));

		dwTimePassed = dwNowTime - m_dwAnimationPauseTime - m_dwAnimationStartTime;

		/*if(0)
		{
			if( SUCCEEDED( GetData( &KeyFrame, dwTimePassed ) ) )
			{
				pCamera->SetPosition( KeyFrame.vPosition );
				pCamera->SetLookAtPosition( KeyFrame.vLookAt );
			}
		}
		else*/
		{
			if(SUCCEEDED(GetDataByTinyLine(&KeyFrame, dwTimePassed)))
			{
				D3DXVECTOR3 vecUpDir,vecLookatDir,vecLookAtPos,vecLeftDir,vecRealUpDir;
				D3DXMATRIX matRotion;
				/*pCamera->SetPosition( KeyFrame.vPosition );
				pCamera->SetLookAtPosition( KeyFrame.vLookAt );*/
				//pCamera->GetUpDirection(&vecUpDir);
				vecUpDir = D3DXVECTOR3(0,1,0);
				vecLookatDir = KeyFrame.vLookAt - KeyFrame.vPosition;
				D3DXVec3Normalize(&vecLookatDir,&vecLookatDir);
				D3DXVec3Normalize(&vecUpDir,&vecUpDir);
				D3DXVec3Cross(&vecLeftDir,&vecUpDir,&vecLookatDir);
				D3DXVec3Normalize(&vecLeftDir,&vecLeftDir);
				D3DXVec3Cross(&vecRealUpDir,&vecLookatDir,&vecLeftDir);
				D3DXVec3Normalize(&vecRealUpDir,&vecRealUpDir);
				D3DXMatrixRotationAxis(
					&matRotion,
					&vecRealUpDir,
					m_fAngle
					);

				D3DXVec3TransformNormal(
					&vecLookatDir,
					&vecLookatDir,
					&matRotion
					);
				vecLookAtPos =  KeyFrame.vPosition + 100 * vecLookatDir;
				//pCamera->SetLookAtPosition(vecLookAtPos);
				pCamera->SetPositionLookAtUp(&KeyFrame.vPosition, &vecLookAtPos, &vecRealUpDir);
			}
		}


		///强制同步
		this->Sync(dwNowTime);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DSceneCameraAnimation::PausePlay( IKG3DSceneCameraKeyFrame* pPauseKey, BOOL bBenchMark)
{
    DWORD dwNowTime = g_cGraphicsTool.GetNowTime();

	KG_PROCESS_ERROR(m_bIsPlaying);///没有开始播放就暂停，挂

	if (!m_bIsPaused)
	{
        this->Sync(dwNowTime, bBenchMark);
        DWORD dwTimePassed = dwNowTime - m_dwAnimationStartTime;
		{///记录现场
			m_dwAnimationPauseStartTime = dwNowTime;
			//m_dwAnimationPauseTime = 0;//不清0，因为在一个循环中，可能多次Pause。只有经过了一个循环的时候，Syn才自动清0
		}
		m_bIsPaused = true;
		if (pPauseKey)
		{
			CameraKeyFrame KeyFrame;
			KG_COM_PROCESS_ERROR(GetData(&KeyFrame, dwTimePassed));
			//memcpy_s(pPauseKey, sizeof(IKG3DSceneCameraKeyFrame), &KeyFrame, sizeof(IKG3DSceneCameraKeyFrame));
			*pPauseKey = KeyFrame;
		}	
	}
	else
	{
		m_dwAnimationPauseTime += dwNowTime - m_dwAnimationPauseStartTime;
		m_bIsPaused = false;
	}	
	return S_OK;
Exit0:
	return E_FAIL;
}


///保存CameraAni文件
HRESULT KG3DSceneCameraAnimation::SaveToFile(FILE* pFile)
{
	if (!pFile)
	{
		return E_FAIL;
	}
	if (!m_bIsTangentComputed)
	{
		StopPlay();
		ComputeTangent();
	}
	
	char czFileBegin[20] = "<CamAni>";
	char czFileEnd[20] = "</CamAni>";
	int	nEmptyBuffer[8];///用于设置保留位
	memset(nEmptyBuffer, 0, sizeof(nEmptyBuffer));
	fwrite(czFileBegin,sizeof(char),20,pFile);
	{
		char cBuffer[256];
		ZeroMemory(cBuffer, sizeof(cBuffer));
		_ASSERTE(m_szName.length() < max_name_length);
		strcpy_s(cBuffer, m_szName.c_str());
		cBuffer[max_name_length-1] = '\0';//把字符串断下来，下面的空间要放别的东西

		///写入版本
		*(DWORD*)(cBuffer + max_name_length) = cur_version_main;
		*(DWORD*)(cBuffer + max_name_length + sizeof(DWORD)) = cur_version_sub;

		fwrite(cBuffer,sizeof(char),256,pFile);	///空隙
	}
	fwrite(&m_fPlaySpeed,sizeof(float),1,pFile);
	fwrite(&m_dwPlayMethod,sizeof(DWORD),1,pFile);
	DWORD dwAniVersion = 0;
	DWORD dwAniSize = (DWORD)m_mapCameraKeyFrame.size();
	fwrite(&dwAniVersion,sizeof(DWORD),1,pFile);
	fwrite(&dwAniSize,sizeof(DWORD),1,pFile);

	_ASSERTE(sizeof(CameraKeyFrame) == sizeof(D3DXVECTOR3)*4);
	TypeKeyContainer::iterator it = m_mapCameraKeyFrame.begin();
	while(it!=m_mapCameraKeyFrame.end())
	{
		float fTime = (float)it->first; // 文件格式不好改DWORD了:(
		CameraKeyFrame* pKeyFrame = &it->second;
		fwrite(&fTime,sizeof(float),1,pFile);
		fwrite(pKeyFrame,sizeof(CameraKeyFrame),1,pFile);
		
		it++;
	}
	fwrite(nEmptyBuffer,sizeof(int),_countof(nEmptyBuffer),pFile);///保留位
	fwrite(czFileEnd,sizeof(char),20,pFile);
	return S_OK;
}
///读取CameraAni文件
HRESULT KG3DSceneCameraAnimation::LoadFromFile(IFile* pFile)
{
    // TODO: 性能低

	HRESULT hr;
    unsigned long uSize = 0;
	char czTemp[22];
    char cBuffer[256];
	DWORD nMainVersion = 0;
    DWORD nSubVersion = 0;
	DWORD dwAniVersion = 0;
	DWORD dwAniSize = 0;
	D3DXVECTOR3 vec3ZeroVec(0.0f, 0.0f, 0.0f);
	int	nEmptyBuffer[8];///用于设置保留位

    if (m_bIsPlaying || !pFile )
	{
		return E_FAIL;
	}
	
    uSize = pFile->Read(czTemp, sizeof(char) * 20);
    KGLOG_PROCESS_ERROR(uSize == sizeof(char) * 20);

	if ( 0 != strcmp( czTemp, "<CamAni>" ) )
	{
		return E_FAIL;
	}

    ZeroMemory(cBuffer, sizeof(cBuffer));

    uSize = pFile->Read(cBuffer, sizeof(char) * 256);///空隙
    KGLOG_PROCESS_ERROR(uSize == sizeof(char) * 256);

    cBuffer[max_name_length-1] = '\0';//把字符串断下来，下面的空间要放别的东西
    m_szName = cBuffer;
    _ASSERTE(m_szName.length() + 1 < max_name_length);

    //读入版本，根据版本采取不同策略
    nMainVersion = *(DWORD*)(cBuffer+max_name_length);
    if(nMainVersion > cur_version_main || nMainVersion < 1)
        nMainVersion = 1;//乱码的情况，取最原始版本
    nSubVersion  = *(DWORD*)(cBuffer+max_name_length+sizeof(DWORD));
    if(nSubVersion > cur_version_sub || nSubVersion < 0)
        nSubVersion = 0;
	
    uSize = pFile->Read(&m_fPlaySpeed, sizeof(float));
    KGLOG_PROCESS_ERROR(uSize == sizeof(float));

	uSize = pFile->Read(&m_dwPlayMethod, sizeof(DWORD));
    KGLOG_PROCESS_ERROR(uSize == sizeof(DWORD));

	uSize = pFile->Read(&dwAniVersion, sizeof(DWORD));
    KGLOG_PROCESS_ERROR(uSize == sizeof(DWORD));

	uSize = pFile->Read(&dwAniSize, sizeof(DWORD));	
    KGLOG_PROCESS_ERROR(uSize == sizeof(DWORD));

	_ASSERTE(sizeof(CameraKeyFrame) == sizeof(D3DXVECTOR3)*4);
	if (nMainVersion == 1 && nSubVersion  == 0)
	{
		for(DWORD i =0;i<dwAniSize;i++)
		{
			float fTime = -0.1f;
			CameraKeyFrame KeyFrame;

			uSize = pFile->Read(&fTime, sizeof(float));
            KGLOG_PROCESS_ERROR(uSize == sizeof(float));

			uSize = pFile->Read(&KeyFrame, sizeof(IKG3DSceneCameraKeyFrame));
            KGLOG_PROCESS_ERROR(uSize == sizeof(IKG3DSceneCameraKeyFrame));

			hr = AddKeyFrame( &KeyFrame, (DWORD)(fTime / 100 * 3000));///新的时间用毫秒做单位，需要换算一下
			KG_COM_PROCESS_ERROR(hr);		
		}
	}
	else if (nMainVersion >= 1 && nSubVersion >= 10000)///在这个版本之后用新的读取
	{
		for(DWORD i =0;i<dwAniSize;i++)
		{
			/*
			CameraKeyFrame KeyFrame;
						fread(&KeyFrame,sizeof(CameraKeyFrame),1,pFile);*/
			float fTime = 0.0f;
			CameraKeyFrame KeyFrame;

			uSize = pFile->Read(&fTime, sizeof(float));
            KGLOG_PROCESS_ERROR(uSize == sizeof(float));

			uSize = pFile->Read(&KeyFrame, sizeof(IKG3DSceneCameraKeyFrame));
            KGLOG_PROCESS_ERROR(uSize == sizeof(IKG3DSceneCameraKeyFrame));
			
			AddKeyFrame(&KeyFrame, (DWORD)fTime);///新的就直接读取就行了
			//KG_COM_PROCESS_ERROR(hr);		
		}
	}
	KG_PROCESS_ERROR(!m_mapCameraKeyFrame.empty());

	memset(nEmptyBuffer, 0, sizeof(nEmptyBuffer));

	uSize = pFile->Read(nEmptyBuffer, sizeof(int));
    KGLOG_PROCESS_ERROR(uSize == sizeof(int));

	if (nEmptyBuffer[0] == 0)///空白保留区存在
	{
		uSize = pFile->Read(nEmptyBuffer + 1, sizeof(int) * 7);
        KGLOG_PROCESS_ERROR(uSize == sizeof(int) * 7);

		uSize = pFile->Read(czTemp, sizeof(char) * 20);
        KGLOG_PROCESS_ERROR(uSize == sizeof(char) * 20);
	}
	else
	{
		memcpy_s(czTemp, sizeof(int), nEmptyBuffer, sizeof(int));
		
        uSize = pFile->Read(czTemp + sizeof(int), sizeof(char) * (20 - sizeof(int)));
        KGLOG_PROCESS_ERROR(uSize == sizeof(char) * (20 - sizeof(int)));
	}	
	if ( 0 != strcmp( czTemp, "</CamAni>" ) )
	{
		return E_FAIL;
	}
	m_bIsTangentComputed = false;
	ComputeTinyLine();
	return S_OK;
Exit0:
	return E_FAIL;
}
///计算切线，存储到关键帧中
HRESULT KG3DSceneCameraAnimation::ComputeTangent()
{

	if(m_bIsPlaying || m_bIsKeyEditable)
	{
		ComputeTinyLine();
		return E_FAIL;
	}
	TypeKeyContainer::iterator it = m_mapCameraKeyFrame.begin();
	while(it!=m_mapCameraKeyFrame.end())
	{
		CameraKeyFrame* pKeyFrame = &it->second;
		if(it==m_mapCameraKeyFrame.begin())
		{
			pKeyFrame->vLookAtTangent = pKeyFrame->vLookAt + D3DXVECTOR3(0,0,0);//把Tangent矢量清0
			pKeyFrame->vPositionTangent = pKeyFrame->vPosition + D3DXVECTOR3(0,0,0);

		}
		TypeKeyContainer::iterator j = it;
		j++;
		if(j!=m_mapCameraKeyFrame.end())
		{
			CameraKeyFrame* pKeyFrameNext = &j->second;
			D3DXVECTOR3 T_LookAt = pKeyFrame->vLookAtTangent - pKeyFrame->vLookAt;
			D3DXVECTOR3 T_Position = pKeyFrame->vPositionTangent - pKeyFrame->vPosition;
			D3DXVECTOR3 T_LookAtNext = (pKeyFrameNext->vLookAt - pKeyFrame->vLookAt)*0.06F;
			D3DXVECTOR3 T_PositionNext = (pKeyFrameNext->vPosition - pKeyFrame->vPosition)*0.06F;

			pKeyFrameNext->vLookAtTangent = T_LookAtNext  + pKeyFrameNext->vLookAt;
			pKeyFrameNext->vPositionTangent = T_PositionNext + pKeyFrameNext->vPosition;
			
			pKeyFrame->vLookAtTangent = (T_LookAtNext+T_LookAt)*0.5F + pKeyFrame->vLookAt;
			pKeyFrame->vPositionTangent = (T_PositionNext+T_Position)*0.5F + pKeyFrame->vPosition;
		}

		it++;
	}
	m_bIsTangentComputed = true;	///改变了关键帧的话，要重新算Tangent
	ComputeTinyLine();

	return S_OK;
}
///把关键帧加入到场景的KG3DRenderEntity列表中
HRESULT KG3DSceneCameraAnimation::AddDataToSceneEditing()
{
	KG_PROCESS_ERROR(! m_bIsPlaying);
	
	{
		RemoveDataFromSceneEditing();
		_ASSERTE(m_vecObjectsInTheScene.empty());
		size_t uKeyFrameSize = m_mapCameraKeyFrame.size();
		KG_PROCESS_SUCCESS(uKeyFrameSize == 0);
		_ASSERTE(m_vecKeyForSelection.empty());
		m_vecKeyForSelection.reserve(uKeyFrameSize*4);
		for(TypeKeyContainer::iterator it = m_mapCameraKeyFrame.begin(); it != m_mapCameraKeyFrame.end(); it++)
		{
            DWORD dwKeyFrameTime = it->first;
			CameraKeyFrame& CamKey = it->second;

			for (size_t j = 0; j < CamKey.GetVecCount(); j++)
			{
				CameraKeyFrameForSelection KeyForSelection;
				KeyForSelection.m_dwType = CamKey.GetVecDesc(j);
				KeyForSelection.m_dwTime = dwKeyFrameTime;
				KeyForSelection.m_vVecPos = CamKey.GetVec(j);
				KeyForSelection.m_pAnimation = this;
#if 0
				m_vecKeyForSelection.push_back(KeyForSelection);


				//KG3DSceneEntity EntityTemp(m_vecKeyForSelection.back(), NULL);
				/*listRender.push_back(EntityTemp);
				listSelectable.push_back(EntityTemp);
				listSelected.push_back(EntityTemp);*/

				//旧格式已经没有用了，如果是旧格式就这么干

				m_SceneRef.AddRenderEntity(SCENEENTITY_CAM_ANI_KEY
					, reinterpret_cast<LPVOID>(&m_vecKeyForSelection.back()));
#else
				KG3DRepresentObjectDummy* pObjectDummy = NULL;
				_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DRepresentObject, KG3DRepresentObjectDummy);
				HRESULT hr = g_GetObjectTable().Get1NewResource((KG3DRepresentObject**)&pObjectDummy, REPRESENTOBJECT_DUMMY, 0);
				if (SUCCEEDED(hr))
				{
					D3DXVECTOR3& vec = CamKey.GetVec(j);
					pObjectDummy->SetTranslation(&vec);
					pObjectDummy->SetSize(40);
					bool bRet = pObjectDummy->RegisterReceiver(this);
					hr = pObjectDummy->SetBuffer(&KeyForSelection, sizeof(CameraKeyFrameForSelection));
					if (SUCCEEDED(hr) && bRet)
					{
						_ASSERTE(NULL != pObjectDummy);
						//hr = m_SceneRef.AddRepresentObject(pObjectDummy, TRUE, FALSE);
						hr = m_SceneRef.AddRepresentObject(pObjectDummy, EM_SCENE_ADD_OPTION_SELECTABLE, NULL);
						if (SUCCEEDED(hr))
						{
							pObjectDummy->AddRef();//自己要保存一份，所以要AddRef
							m_vecObjectsInTheScene.push_back(pObjectDummy);
						}
						else
							SAFE_RELEASE(pObjectDummy);
					}
					else
						SAFE_RELEASE(pObjectDummy);				
				}
#endif
			}//for (size_t j
		}
	}
Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}
///移除场景中加入的KG3DRenderEntity关键帧
HRESULT KG3DSceneCameraAnimation::RemoveDataFromSceneEditing()
{
	KG_PROCESS_ERROR(! m_bIsPlaying);
	KG_PROCESS_SUCCESS(m_mapCameraKeyFrame.empty());
	{
#if 0
		for (size_t i = 0; i < m_vecKeyForSelection.size(); i++)
		{
			//KG3DSceneEntity	EntityTemp(m_vecKeyForSelection[i], NULL);
			/*if(! listRender.empty())
				listRender.remove(EntityTemp);
			if(! listSelectable.empty())
				listSelectable.remove(EntityTemp);
			if (! listSelected.empty())
				listSelected.remove(EntityTemp);*/

			m_SceneRef.RemoveRenderEntity(SCENEENTITY_CAM_ANI_KEY
				, reinterpret_cast<LPVOID>(&m_vecKeyForSelection[i]));

		}
#else
		for (size_t i = 0; i < m_vecObjectsInTheScene.size(); ++i)
		{
			m_SceneRef.RemoveRepresentObject(m_vecObjectsInTheScene[i]);
		}
		KG_CUSTOM_HELPERS::TContainerRelease(m_vecObjectsInTheScene);
#endif
		m_vecKeyForSelection.clear();
	}
Exit1:
	return S_OK;
Exit0:
	return E_FAIL;
}
///清除map中的所有关键帧
HRESULT KG3DSceneCameraAnimation::Clear()
{
	if (m_bIsPlaying || m_bIsKeyEditable )
	{
		return E_FAIL;
	}
	m_mapCameraKeyFrame.clear();
	m_dwLastKeyFrameTime = 0;
	m_bIsTangentComputed = false;	///改变了关键帧的话，要重新算Tangent
	EnableEditCamAniKeyFrame(FALSE);
	return S_OK;
}

///设定开始播放的标记，并记录一个开始时间，和RenderCount
HRESULT KG3DSceneCameraAnimation::StartPlay(BOOL bLoop )
{
	if ( (int)m_mapCameraKeyFrame.size() < 2 )	///帧数小于两帧，不能开始播放，不然同步会有问题
	{
		return E_FAIL;
	}
    m_dwAnimationStartTime = g_cGraphicsTool.GetNowTime();

	if ( false == m_bIsTangentComputed )	///没有计算曲线的话，先计算曲线才开始播放动画
	{
		this->ComputeTangent();
	}
	if (m_bIsPlaying)
	{
		StopPlay();
	}
	m_bLoopWhilePlaying = bLoop ? true : false;

	m_bIsPlaying = true;
	m_nRenderCount = 0;
	m_bIsPaused = false;
	m_dwAnimationPauseStartTime = 0;
	m_dwAnimationPauseTime = 0;
	return S_OK;
}

///设定开始播放的标记
HRESULT KG3DSceneCameraAnimation::StopPlay()
{
	m_dwAnimationStartTime = 0;
	m_bIsPlaying = false;
	m_nRenderCount = 0;
	m_bIsPaused = false;
	m_dwAnimationPauseStartTime = 0;
	m_dwAnimationPauseTime = 0;
	return S_OK;
}


HRESULT KG3DSceneCameraAnimation::PausePlay( IKG3DSceneCameraKeyFrame* pPauseKey, BOOL* bIsPaused )
{
	HRESULT hr = PausePlay(pPauseKey);
	if (bIsPaused)
	{
		*bIsPaused = m_bIsPaused;
	}
	return hr;
}
///同步，fNowTime，不让UpdateCamera的时间控制脱离控制
HRESULT KG3DSceneCameraAnimation::Sync(DWORD dwNowTime, BOOL bBenchMark)
{
    ASSERT(dwNowTime >= (m_dwAnimationPauseTime + m_dwAnimationStartTime));

    DWORD dwTimePassed = dwNowTime - m_dwAnimationPauseTime - m_dwAnimationStartTime;
    DWORD dwCurAniLength = this->GetAnimationTotalTime();
    if (bBenchMark)
        dwCurAniLength = m_dwLastKeyFrameTime;
	
	m_nRenderCount ++;

	if( dwTimePassed > dwCurAniLength )
	{
#if 0
		float FPS = m_nRenderCount * /*500.0F*/1000.0f / ( fCurTime - m_fAnimationStartTime );
		TCHAR str[256];
		sprintf( str
			,"Frames: %d Time: %f FPS:%f"
			,this->m_nRenderCount
			,fCurAniLength
			,FPS
			);
#endif


		///重新开始计算时间
		m_dwAnimationStartTime = dwNowTime;
		m_nRenderCount = 0;
		m_dwAnimationPauseStartTime = 0;
		m_dwAnimationPauseTime = 0;

		if (!m_bLoopWhilePlaying)
		{
			this->StopPlay();
		}
	}
	
	return S_OK;
}
HRESULT KG3DSceneCameraAnimation::GetName(LPTSTR ppcszRetName, size_t uBufferSize)
{
    KG_PROCESS_ERROR(ppcszRetName);
	{
		KG_CUSTOM_HELPERS::UnicodeConvertor Cvt;
		return Cvt.ToT(m_szName.c_str(), m_szName.length()+1, ppcszRetName, uBufferSize)?S_OK:E_FAIL;
	}
Exit0:
    return E_FAIL;
}

const std::string& KG3DSceneCameraAnimation::GetName()
{
	return m_szName;
}

HRESULT KG3DSceneCameraAnimation::GetName( LPCTSTR *ppcszRetName )
{
	 static TCHAR stName[max_name_length];
	 KG_CUSTOM_HELPERS::UnicodeConvertor Cvt;
	 TCHAR* pT = Cvt.ToT(m_szName.c_str(), m_szName.length()+1, stName, _countof(stName));
	 KG_PROCESS_ERROR(pT && ppcszRetName);
	 *ppcszRetName = pT;
	 return S_OK;
Exit0:
	 return E_FAIL;
}
HRESULT KG3DSceneCameraAnimation::SetName(LPCTSTR pcszName)
{
    HRESULT hResult  = E_FAIL;

    KG_PROCESS_ERROR(pcszName && _tcslen(pcszName) < max_name_length);

    m_szName = pcszName;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

///检查是否在播放过程中
HRESULT KG3DSceneCameraAnimation::GetPlayState(BOOL *pbRetIsPlaying)
{
    HRESULT hResult  = E_FAIL;
    
    KGLOG_PROCESS_ERROR(pbRetIsPlaying);

    *pbRetIsPlaying = m_bIsPlaying;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

//是否允许编辑关键帧，允许的话，关键帧数据加入RenderEntity列表，同时修改m_bIsKeyEditable的状态，只要使用一次就会自动记录下两个列表的指针
HRESULT KG3DSceneCameraAnimation::EnableEditCamAniKeyFrame(BOOL bEnable)
{
	/*///即使在播放中，也应该允许这个函数作用
	///第一次使用的话，记录下两个列表的指针
	if ( ! m_plistRender )
		m_plistRender = plistRender;
	if ( ! m_plistSelectable )
		m_plistSelectable = plistSelectable;
	if (! m_plistSelected )
		m_plistSelected = plistSelected;*/
	///第二次以上使用这个函数都会使用成员指针
	//KG_PROCESS_ERROR(m_plistRender && m_plistSelectable && m_plistSelected);
	
	if (bEnable && ! m_bIsKeyEditable)
	{
		HRESULT hr =  AddDataToSceneEditing();
		KG_COM_PROCESS_ERROR(hr);
		m_bIsKeyEditable = true;
	}
	else if(! bEnable && m_bIsKeyEditable)
	{
		HRESULT hr = RemoveDataFromSceneEditing();
		KG_COM_PROCESS_ERROR(hr);
		m_bIsKeyEditable = false;
	}
	return S_OK;
Exit0:
	return E_FAIL;

}
///返回m_bIsEditable
HRESULT KG3DSceneCameraAnimation::IsKeyFrameEditable(BOOL *pbRetIsKeyFreeEditable)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pbRetIsKeyFreeEditable);

    *pbRetIsKeyFreeEditable = m_bIsKeyEditable;

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

///得到关键帧数量
HRESULT KG3DSceneCameraAnimation::GetKeyFrameCount(DWORD *pdwKeyFrameCount)
{
    HRESULT hResult  = E_FAIL;

    KGLOG_PROCESS_ERROR(pdwKeyFrameCount);

    *pdwKeyFrameCount = (DWORD)m_mapCameraKeyFrame.size(); 

//Exit1:
    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DSceneCameraAnimation::GetCurSelectedKey( IKG3DSceneCameraKeyFrameEx* pKey )
{
	TypeKeyContainer::iterator it = m_mapCameraKeyFrame.find(m_dwSelectedKeyTime);
	KG_PROCESS_ERROR(pKey && it != m_mapCameraKeyFrame.end());
	CopyKeyFrame(it->second, it->first, *pKey);	
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneCameraAnimation::SetCurSelectedKey( const IKG3DSceneCameraKeyFrameEx* pKey )
{
	KG_PROCESS_ERROR(pKey);
	{
		TypeKeyContainer::iterator it = m_mapCameraKeyFrame.find(m_dwSelectedKeyTime);
		KG_PROCESS_ERROR(it != m_mapCameraKeyFrame.end());

		std::vector<CameraKeyFrameForSelection*> vecKeyForSelTemp;
		if (m_bIsKeyEditable)
		{
			for (size_t i = 0; i != m_vecKeyForSelection.size(); i++)
			{
				if (m_vecKeyForSelection[i].m_dwTime == it->first)//同一时间可以有个KeyFrame，因为有Pos，PosTran等东西
				{
					vecKeyForSelTemp.push_back(&m_vecKeyForSelection[i]);
				}
			}
		}
		

		if (MT_FLOAT_UNEQUAL(pKey->dwTime, it->first))//即要求修改时间
		{
			//上下查找
			TypeKeyContainer::iterator itUp = it;
			TypeKeyContainer::iterator itDown = it;
			if (it != m_mapCameraKeyFrame.begin())
				--itUp;
			if (it != m_mapCameraKeyFrame.end())
				++itDown;
			//改时间不能超越上下界
			KG_PROCESS_ERROR((itUp == it || pKey->dwTime > itUp->first) && (itDown == m_mapCameraKeyFrame.end() || itDown == it || pKey->dwTime < itDown->first));

			CameraKeyFrame KeyTemp = it->second;
			m_mapCameraKeyFrame.erase(it);
			m_mapCameraKeyFrame[pKey->dwTime] = KeyTemp;

			if (m_bIsKeyEditable)
			{
				if (! vecKeyForSelTemp.empty())
				{
					for (size_t i = 0; i < vecKeyForSelTemp.size(); i++)
					{
						CameraKeyFrameForSelection* pKeyForSel = vecKeyForSelTemp[i];
						pKeyForSel->m_dwTime = pKey->dwTime;
					}
					
				}
			}
			m_dwSelectedKeyTime = pKey->dwTime;
			if (itDown == m_mapCameraKeyFrame.end())
			{
				this->m_dwLastKeyFrameTime =  pKey->dwTime;
			}
			it = m_mapCameraKeyFrame.find(pKey->dwTime);//改过时间之后要重新查找
		}
		
		
		KG_PROCESS_ERROR(it != m_mapCameraKeyFrame.end());
		{
			CameraKeyFrame& Key = it->second; 
			IKG3DSceneCameraKeyFrame&	KeyBase1 = Key;///去掉时间的部分
			IKG3DSceneCameraKeyFrame	KeyBase2 = *pKey;
			if (0 != memcmp(&KeyBase1, &KeyBase2, sizeof(IKG3DSceneCameraKeyFrame)))	//位置不等
			{
				KeyBase1 = KeyBase2;
				if (m_bIsKeyEditable)	//如果当前正在编辑关键帧,要同步一下
				{
					for (size_t i = 0; i < Key.GetVecCount(); i++)
					{
						DWORD dwType = Key.GetVecDesc(i);
						for (size_t j = 0; j < vecKeyForSelTemp.size(); j++)
						{
							CameraKeyFrameForSelection* pKey = vecKeyForSelTemp[j];
							if (pKey->m_dwType == dwType)
							{
								pKey->m_vVecPos = Key.GetVec(i);
							}
						}	
					}
						
				}
			}
		}

	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneCameraAnimation::GetCurSelectedKeyTime(
    DWORD *pdwRetSelectedKeyTime, DWORD *pdwRetPreKeyTime, DWORD *pdwRetNextKeyTime
)
{
	if (pdwRetSelectedKeyTime)
	{
		const TypeKeyContainer::iterator it = m_mapCameraKeyFrame.find(m_dwSelectedKeyTime);
		KG_PROCESS_ERROR(it != m_mapCameraKeyFrame.end());
		*pdwRetSelectedKeyTime = m_dwSelectedKeyTime;
		if (pdwRetPreKeyTime)
		{
			TypeKeyContainer::iterator itPrev = it;
			if (it != m_mapCameraKeyFrame.begin())
			{
				--itPrev;
				*pdwRetPreKeyTime = itPrev->first;
			}
			else
			{
				*pdwRetPreKeyTime = UINT_MAX;
			}
		}
		if (pdwRetNextKeyTime)
		{
			TypeKeyContainer::iterator itNext = it;
			_ASSERTE(it != m_mapCameraKeyFrame.end());
			{
				++itNext;
				if (itNext != m_mapCameraKeyFrame.end())
				{
					*pdwRetNextKeyTime = itNext->first;
				}
				else
				{
					*pdwRetNextKeyTime = UINT_MAX;
				}
			}
			/*
			else
						{
							*pdwRetNextKeyTime = -1;
						}*/
			
		}
		return S_OK;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneCameraAnimation::SetKeySelChangeCallback( KG3DSCAniSelKeyChangeCallback pNewCallback, KG3DSCAniSelKeyChangeCallback* ppOldCallback )
{
	if (ppOldCallback)
	{
		*ppOldCallback = m_CallbackKeySelChange;
	}
	m_CallbackKeySelChange = pNewCallback;//可以为NULL
	return S_OK;
}

HRESULT KG3DSceneCameraAnimation::CreateSpecialTrack( DWORD dwType, DWORD_PTR dwParam )
{
	KG_PROCESS_ERROR(!m_bIsKeyEditable && !m_bIsPlaying);
	if (dwType == emKG3DSceneCameraAnimationSpecialKeyTrack_Circle)
	{

		KG3DSceneCameraAnimationSPTrack_Struct Param;
		if (dwParam)
		{
			Param = *reinterpret_cast<KG3DSceneCameraAnimationSPTrack_Struct*>(dwParam);
		}
		KG_PROCESS_ERROR(Param.m_fRadius > 0);

		BOOL bEnableEditKeyFrame = m_bIsKeyEditable;

		this->Clear();
		D3DXVECTOR3 vRightBios(Param.m_fRadius, 0, 0);
		D3DXVECTOR3 vUpBios(0, 0, Param.m_fRadius);
		D3DXVECTOR3 vLookAtTan(0, 0, 0);

		static float stfCircleTanScale = 1.657f;//这个比例是用Hermite曲线和圆的曲线拟合算出来的，反正切线长用这个比例就可以做出圆。本来算出来是0.657和1.657之间的，后者好
		
		D3DXVECTOR3 vTanUp(0, 0, Param.m_fRadius * (stfCircleTanScale) / stfTangentScale );
		D3DXVECTOR3 vTanLeft(-Param.m_fRadius * stfCircleTanScale / stfTangentScale, 0, 0);

		if (Param.m_bClockWiseOrCounter)
		{
			vTanUp = -vTanUp;
			vTanLeft = -vTanLeft;
		}
		
		//四个点分别位于+-X和+-Z，然后切线垂直
		CameraKeyFrame CamKeyArray[] = 
		{
			CameraKeyFrame(Param.m_vPos + vRightBios, Param.m_vPos, Param.m_vPos + vRightBios + vTanUp, vLookAtTan),
			CameraKeyFrame(Param.m_vPos + vUpBios, Param.m_vPos, Param.m_vPos + vUpBios + vTanLeft, vLookAtTan),
			CameraKeyFrame(Param.m_vPos - vRightBios, Param.m_vPos, Param.m_vPos - vRightBios - vTanUp, vLookAtTan),
			CameraKeyFrame(Param.m_vPos - vUpBios, Param.m_vPos, Param.m_vPos - vUpBios - vTanLeft, vLookAtTan),
		};
		if (! Param.m_bClockWiseOrCounter)//逆时针
		{
			for (size_t u = 0; u < _countof(CamKeyArray); u++)
			{
				this->AddKeyFrame(&CamKeyArray[u]);
			}
			this->AddKeyFrame(&CamKeyArray[0]);//在最后一帧重复第一帧，形成封闭
		}
		else
		{
			this->AddKeyFrame(&CamKeyArray[0]);
			for (int u = _countof(CamKeyArray)-1; u >= 0; u--)
			{
				this->AddKeyFrame(&CamKeyArray[u]);
			}
		}
		ComputeTinyLine();
		m_bIsTangentComputed = true;

		EnableEditCamAniKeyFrame(bEnableEditKeyFrame);

	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneCameraAnimation::ComputeTinyLine()
{
	m_fDistanceTotal = 0;
	m_vecTineLine.clear();

	if(m_bIsPlaying || m_bIsKeyEditable)
	{
		return E_FAIL;
	}

	TypeKeyContainer::iterator it = m_mapCameraKeyFrame.begin();
	while(it!=m_mapCameraKeyFrame.end())
	{
		CameraKeyFrame* pKeyFrame = &it->second;

		TypeKeyContainer::iterator j = it;
		j++;
		if(j!=m_mapCameraKeyFrame.end())
		{
			CameraKeyFrame* pKeyFrameNext = &j->second;

			D3DXVECTOR3 VLookAt = pKeyFrameNext->vLookAt - pKeyFrame->vLookAt;
			float fLenLookAt = D3DXVec3Length(&VLookAt);
			D3DXVECTOR3 VPos= pKeyFrameNext->vPosition - pKeyFrame->vPosition;
			float fLenPos = D3DXVec3Length(&VPos);
			float flen = fLenPos + fLenLookAt;
			int n = int(flen / 500.0F);
			if(n<=0)
				n = 1;
			for (int i=0;i<n;i++)
			{
				TinyLine vTineLine;

				float S_L = 1.0F / n * i;
				float S_H = 1.0F / n * (i+1);

				D3DXVec3Hermite(&vTineLine.vLookAtA,
					&pKeyFrame->vLookAt,
					&((pKeyFrame->vLookAtTangent-pKeyFrame->vLookAt)*stfTangentScale),
					&pKeyFrameNext->vLookAt,
					&((pKeyFrameNext->vLookAtTangent-pKeyFrameNext->vLookAt)*stfTangentScale),
					S_L);
				D3DXVec3Hermite(&vTineLine.vPositionA,
					&pKeyFrame->vPosition,
					&((pKeyFrame->vPositionTangent-pKeyFrame->vPosition)*stfTangentScale),
					&pKeyFrameNext->vPosition,
					&((pKeyFrameNext->vPositionTangent-pKeyFrameNext->vPosition)*stfTangentScale),
					S_L);
				D3DXVec3Hermite(&vTineLine.vLookAtB,
					&pKeyFrame->vLookAt,
					&((pKeyFrame->vLookAtTangent-pKeyFrame->vLookAt)*stfTangentScale),
					&pKeyFrameNext->vLookAt,
					&((pKeyFrameNext->vLookAtTangent-pKeyFrameNext->vLookAt)*stfTangentScale),
					S_H);
				D3DXVec3Hermite(&vTineLine.vPositionB,
					&pKeyFrame->vPosition,
					&((pKeyFrame->vPositionTangent-pKeyFrame->vPosition)*stfTangentScale),
					&pKeyFrameNext->vPosition,
					&((pKeyFrameNext->vPositionTangent-pKeyFrameNext->vPosition)*stfTangentScale),
					S_H);

				D3DXVECTOR3 VLookAt = vTineLine.vLookAtB - vTineLine.vLookAtA;
				float fLenLookAt = D3DXVec3Length(&VLookAt);
				D3DXVECTOR3 VPos= vTineLine.vPositionB - vTineLine.vPositionA;
				float fLenPos = D3DXVec3Length(&VPos) + fLenLookAt;
				vTineLine.fDistance = fLenPos;
				vTineLine.fDistanceStart = m_fDistanceTotal;
				vTineLine.fDistanceEnd = vTineLine.fDistanceStart + vTineLine.fDistance;

				m_fDistanceTotal += vTineLine.fDistance;
				m_vecTineLine.push_back(vTineLine);
			}

		}

		it++;
	}
	return S_OK;
}

void __stdcall KG3DSceneCameraAnimation::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	switch(Message.m_uMessage)
	{
	case KM_SELECT:
	case KM_TRANSLATION:
	case KM_ROTATION:
		{
			KG3DRepresentObjectDummy* pObjDummy = dynamic_cast<KG3DRepresentObjectDummy*>(pSender);
			if (NULL == pObjDummy)
				break;

			CameraKeyFrameForSelection KeyForSel;
			HRESULT hr = pObjDummy->GetBuffer(&KeyForSel, sizeof(KeyForSel));
			if (FAILED(hr))
				break;

			switch(Message.m_uMessage)
			{
			case KM_SELECT:
				if(Message.m_dwLParam == EM_EVENT_STATE_END)
					this->OnKeyFrameSelected(&KeyForSel);
				break;
			case KM_TRANSLATION:
				this->OnKeyFrameTranslate(&KeyForSel, reinterpret_cast<D3DXVECTOR3*>(Message.m_dwWParam));
			    break;
			/*case KM_ROTATION:
				this->OnKeyFrameRotation(&KeyForSel, reinterpret_cast<D3DXQUATERNION*>(Message.m_dwWParam));
				break;*/
			default:
			    break;
			}
		}
	    break;
	default:
	    break;
	}
}
