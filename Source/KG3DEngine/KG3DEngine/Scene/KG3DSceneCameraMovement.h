#pragma once
//#include "kg3dkeyframedatabase.h"

#include "IEKG3DSceneCamera.h"
#include "KG3DMessenger.h"

class KG3DCamera;
class KG3DSceneEntity;
class IEKG3DScene;
class KG3DRepresentObjectDummy;
/************************************************************************/
/* @brief 摄像机动画类，用于记录和控制摄像机播放动画
/************************************************************************/
class KG3DSceneCameraAnimation : public IEKG3DSceneCameraAnimation, public KG3DMessageReceiver
{
public:
/************************************************************************/
/* @brief 结构体，用于记录关键帧。一个关键帧包括1.摄像机位置切线；2.摄像机焦点切线；3.摄像机位置；4.摄像机焦点位置。切线由ComputeTangent方法计算。
/************************************************************************/
	enum
	{
		emCameraKeyFrameForSelection_PositionTan = 0,
		emCameraKeyFrameForSelection_LookAtTan = 1,
		emCameraKeyFrameForSelection_Position = 2,
		emCameraKeyFrameForSelection_LookAt = 3,
		emCameraKeyFrameForSelection_Center = 4,
	};
	struct CameraKeyFrame : public IKG3DSceneCameraKeyFrame
	{
		CameraKeyFrame()
		{
			memset(this, 0, sizeof(IKG3DSceneCameraKeyFrame));
		}
		CameraKeyFrame(const D3DXVECTOR3& vPosTemp, const D3DXVECTOR3& vLookAtTemp, const D3DXVECTOR3& vPosTanTemp, const D3DXVECTOR3& vLookAtTangentTemp)
		{
			vPosition = vPosTemp;
			vLookAt = vLookAtTemp;
			vPositionTangent = vPosTanTemp;
			vLookAtTangent = vLookAtTangentTemp;
		}
		enum{vec_count = 4,};
		size_t GetVecCount(){return vec_count;}
		DWORD  GetVecDesc(size_t uSize)
		{
			static DWORD DescArray[] = 
			{
				emCameraKeyFrameForSelection_PositionTan,
				emCameraKeyFrameForSelection_LookAtTan,
				emCameraKeyFrameForSelection_Position,
				emCameraKeyFrameForSelection_LookAt,
			};
			_ASSERTE(uSize < vec_count);
			return DescArray[uSize];
		}
		D3DXVECTOR3&	GetVec(size_t uSize)
		{
			switch(uSize)
			{
			case 0:
				return vPositionTangent;
				break;
			case 1:
				return vLookAtTangent;
				break;
			case 2:
				return vPosition;
			    break;
			case 3:
				return vLookAt;
			    break;
			default:
				_ASSERTE(NULL);
			    break;
			}
			static D3DXVECTOR3 vecError;
			return vecError;
		}
	};
	typedef map<DWORD, CameraKeyFrame> TypeKeyContainer;
	enum{max_name_length = 64,};
	enum{cur_version_main = 1, cur_version_sub = 10000,};
    static VOID CopyKeyFrame(IKG3DSceneCameraKeyFrame& From, DWORD dwTime, IKG3DSceneCameraKeyFrameEx& To)
	{
		memcpy_s(&To, sizeof(IKG3DSceneCameraKeyFrame), &From, sizeof(IKG3DSceneCameraKeyFrame));
		To.dwTime = dwTime;
	}
private:
	struct TinyLine//运动曲线上的一小段
	{
		D3DXVECTOR3 vLookAtA,vLookAtB,vPositionA,vPositionB;
		float fDistance;
		float fDistanceStart;
		float fDistanceEnd;

		HRESULT GetDataByTinyLine(CameraKeyFrame* pValue, float fDistance);
	};
	vector<TinyLine>m_vecTineLine;
    float   m_fDistanceTotal;

	HRESULT GetDataByTinyLine(CameraKeyFrame* pValue, DWORD dwTime);

	HRESULT ComputeTinyLine();

	std::string	m_szName;		///动画的名字，
	float	m_fPlaySpeed;	///播放的随机种子，没有什么用	
	DWORD	m_dwPlayMethod;	///播放的方法，没有什么用

	float m_fAngle; //镜头旋转角度录像用 

    DWORD m_dwLastKeyFrameTime;	///用于记录动画长度和加入帧的时间和帧的Key关键字，实际上没有真正记录长度，只是每次加个100而已
    
	int		m_nRenderCount;			///记录动画渲染的次数
	bool	m_bIsPlaying;			///表示动画开始了
	bool	m_bIsPaused;			///表示动画是否在暂停状态
	bool	m_bLoopWhilePlaying;	///是否循环播放
    DWORD	m_dwAnimationStartTime;	///记录动画开始的时间
    DWORD	m_dwAnimationPauseTime;
    DWORD	m_dwAnimationPauseStartTime;
	TypeKeyContainer m_mapCameraKeyFrame;	///用于放置关键帧的map

	bool	m_bIsTangentComputed;	///表示是否要再计算一次曲线

	bool	m_bIsKeyEditable;		///表示是否可以编辑关键帧，如果可以的话，会加入RenderEntity列表中
	
	/*KG3DSceneEntityList* m_plistRender;	///外部的Entity列表的指针
	KG3DSceneEntityList* m_plistSelectable;	///外部的Entity选择列表的指针
	KG3DSceneEntityList* m_plistSelected;*/
	//<关键帧选择>
public:
	struct CameraKeyFrameForSelection;
public:
	//typedef KG3DSceneEntityDummyExecutor<KG3DSceneEntityDummy>	TypeCamKeyForSelBase;	///无聊的事件由Dummy处理了
	struct CameraKeyFrameForSelection// : public TypeCamKeyForSelBase	//覆盖掉基类中的某些方法
	{
		DWORD m_dwType;
		//const size_t m_uIndex;	//vec中的索引，用于选择
		DWORD m_dwTime;
		D3DXVECTOR3 m_vVecPos;
		KG3DSceneCameraAnimation* m_pAnimation;

		//////////////////////////////////////////////////////////////////////////
		
		/*CameraKeyFrameForSelection(const CameraKeyFrameForSelection& Other)
			:m_dwType(Other.m_dwType)
		{
			memcpy_s(this, sizeof(CameraKeyFrameForSelection), &Other, sizeof(CameraKeyFrameForSelection));			
		}*/
		/*CameraKeyFrameForSelection& operator=(const CameraKeyFrameForSelection& Other)
		{
			_ASSERTE(m_dwType == Other.m_dwType);
			memcpy_s(this, sizeof(CameraKeyFrameForSelection), &Other, sizeof(CameraKeyFrameForSelection));	
			return *this;
		};*/
		/*HRESULT GetCenter(LPVOID, D3DXVECTOR3* pvCenter)
		{
			if (pvCenter)
			{
				*pvCenter = m_vVecPos;
				return S_OK;
			}
			return E_FAIL;
		}
		HRESULT GetBBox(LPVOID, AABBOX* pOutBox)
		{
			if (pOutBox)
			{
				static D3DXVECTOR3	stSize(5, 5, 5);
				*pOutBox = AABBOX(m_vVecPos+stSize, m_vVecPos-stSize);
				return S_OK;
			}
			return E_FAIL;
		}
		HRESULT RenderOption(LPVOID, DWORD_PTR dwOption){return E_FAIL;}
		HRESULT RenderSelectMark(LPVOID, DWORD dwMask);
		HRESULT OnSelect(LPVOID){_ASSERTE(m_pAnimation); m_pAnimation->OnKeyFrameSelected(this); return S_OK;}
		HRESULT SetTranslation(LPVOID, D3DXVECTOR3* pVecPos){_ASSERTE(m_pAnimation); m_pAnimation->OnKeyFrameTranslate(this, pVecPos);return S_OK;}
		HRESULT SetRotation(LPVOID, D3DXQUATERNION* pqRot){_ASSERTE(m_pAnimation); if(m_dwType == emCameraKeyFrameForSelection_Center)m_pAnimation->OnKeyFrameRotation(this, pqRot);return S_OK;}
		HRESULT GetTranslation(LPVOID lpObj, D3DXVECTOR3* pvPos){_ASSERTE(pvPos); *pvPos = m_vVecPos;return S_OK;}*/
	};
private:
	//static KG3DSceneEntityDummyExecutor<KG3DSceneEntityDummy> m_stKG3DSceneEntityDummyExecutor;
	friend CameraKeyFrameForSelection;
	//size_t m_uSelectedKeyIndex;
	DWORD m_dwSelectedKeyTime;
	typedef vector<CameraKeyFrameForSelection> TypeVecKeyForSelection;
	TypeVecKeyForSelection m_vecKeyForSelection;
	KG3DSCAniSelKeyChangeCallback	m_CallbackKeySelChange;

	D3DXVECTOR3 m_vecPosCenter;

	std::vector<KG3DRepresentObjectDummy*>	m_vecObjectsInTheScene;
public:
	VOID OnKeyFrameSelected(CameraKeyFrameForSelection* pKeyForSelection)
	{
		_ASSERTE(pKeyForSelection);
		//m_uSelectedKeyIndex = pKeyForSelection->m_uIndex;
		m_dwSelectedKeyTime = pKeyForSelection->m_dwTime;
		if (m_CallbackKeySelChange)
		{
			(*m_CallbackKeySelChange)(m_dwSelectedKeyTime);
		}
	}
	VOID OnKeyFrameTranslate(CameraKeyFrameForSelection* pKeyForSelection, const D3DXVECTOR3* pVecPos)
	{
		_ASSERTE(pKeyForSelection && pVecPos);
		if (pKeyForSelection->m_dwType == emCameraKeyFrameForSelection_Center)///集体移动
		{
			D3DXVECTOR3 vBios = *pVecPos - m_vecPosCenter;
			for (TypeKeyContainer::iterator it = m_mapCameraKeyFrame.begin(); it != m_mapCameraKeyFrame.end(); it++)
			{
				CameraKeyFrame& Key = it->second;
				Key.vLookAt += vBios;
				Key.vLookAtTangent += vBios;
				Key.vPosition += vBios;
				Key.vPositionTangent += vBios;
			}
			for (size_t i = 0; i < m_vecKeyForSelection.size(); i++)
			{
				CameraKeyFrameForSelection& Key = m_vecKeyForSelection[i];
				Key.m_vVecPos += vBios;
			}
			m_vecPosCenter = *pVecPos;
			
		}
		else
		{
			pKeyForSelection->m_vVecPos = *pVecPos;
			TypeKeyContainer::iterator it = m_mapCameraKeyFrame.find(pKeyForSelection->m_dwTime);
			if (it != m_mapCameraKeyFrame.end())
			{
				CameraKeyFrame& Key = it->second;
				pKeyForSelection->m_vVecPos = *pVecPos;
				switch(pKeyForSelection->m_dwType)
				{
				case emCameraKeyFrameForSelection_PositionTan:
					Key.vPositionTangent = *pVecPos;
					break;
				case emCameraKeyFrameForSelection_LookAtTan:
					Key.vLookAtTangent = *pVecPos;
					break;
				case emCameraKeyFrameForSelection_Position:
					Key.vPosition = *pVecPos;
					break;
				case emCameraKeyFrameForSelection_LookAt:
					Key.vLookAt = *pVecPos;
					break;
				default:
					_ASSERTE(NULL);
					break;
				}
			}
			if (m_CallbackKeySelChange)
			{
				(*m_CallbackKeySelChange)(m_dwSelectedKeyTime);
			}
		}
		return;
	}
	virtual void __stdcall OnReceiveMessage(const KG3DMessage& Message, IKG3DMessageSenderBase* pSender );
	//</关键帧选择>
	

public:
	int GetRenderCount() const { return m_nRenderCount; }
	void SetRenderCount(int val) { m_nRenderCount = val; }
    DWORD GetAnimationStartTime() const { return m_dwAnimationStartTime; }
    void SetAnimationStartTime(DWORD dwStartTime) { m_dwAnimationStartTime = dwStartTime; }	
    DWORD   GetAnimationTotalTime();
    HRESULT PausePlay(IKG3DSceneCameraKeyFrame* pPauseKey, BOOL bBenchMark = FALSE);
protected:	///封闭这两个方法，而用一个EnableEditCamAniKeyFrame代替
    HRESULT Sync(DWORD dwNowTime, BOOL bBenchMark = FALSE);		///同步，fTimePassed不让UpdateCamera的时间控制脱离控制
	HRESULT AddDataToSceneEditing();	///把关键帧加入到场景的KG3DRenderEntity列表中。
	HRESULT RemoveDataFromSceneEditing();	///移除场景中加入的KG3DRenderEntity关键帧。
public:
	virtual float GetPlaySpeed()
	{
		return m_fPlaySpeed;
	}
	virtual HRESULT SetPlaySpeed(float fSpeed)
	{
		m_fPlaySpeed = fSpeed;
		return S_OK;
	}

	virtual float GetPlayAngle()
	{
		return m_fAngle;
	}
	virtual HRESULT SetPlayAngle(float fAngle)
	{
		m_fAngle = fAngle;
		return S_OK;
	}

	const std::string& GetName();
	virtual HRESULT ComputeTangent();		///计算切线，存储到关键帧中

	HRESULT AddKeyFrame( CameraKeyFrame* pKey, DWORD dwKeyFrameTime = 0 );	///以dwKeyFrameTime为时间加入已存在的关键帧，如果fKeyFrameTime传默认值的化会自动增加时间
	
    HRESULT AddKeyFrame( KG3DCamera* pCamera,DWORD dwKeyFrameTime = 0, CameraKeyFrame* pKeyOut = NULL );		///以dwKeyFrameTime为时间，获取pCamera的位置和焦点位置创建关键帧，并把结果的输出到pKeyOut，如果fKeyFrameTime传NULL值的化会自动增加时间
	
	virtual HRESULT AddKeyFrame();	///自动用当前时间和Camera添加KeyFrame	

	virtual HRESULT AddKeyFrameToHeadFramePosition(); ///加一个关键帧可以和第一个关键帧接上
	
	virtual HRESULT RemoveKeyFrame( DWORD dwKeyFrameTime);	///移除dwKeyFrameTime时间的关键帧，如果取0的话，删除最近的一帧

	HRESULT RenderCameraKeyFrame(DWORD dwOption);	///渲染关键帧和曲线
	
    HRESULT UpdateCamera( KG3DCamera* pCamera);	//刷新Camera的位置。使用之前必须保证使用了StartPlay方法，否则无效，然后播放结束的时候必须调用StopPlay方法。
	HRESULT GetData(CameraKeyFrame* pValue, DWORD dwTime);	///以dwTime为时间的到一个关键帧结构体，实际上是两个关键帧之间的插值。
    BOOL    IsFinished(DWORD dwNowTime);
    HRESULT UpdateCameraByTime(KG3DCamera* pCamera);

	HRESULT SaveToFile(FILE* pFile);	///保存CameraAni文件
	HRESULT LoadFromFile(IFile* pFile);	///读取CameraAni文件

	virtual HRESULT Clear();	///清除map中的所有关键帧

	virtual HRESULT StartPlay(BOOL bLoop);
	
	virtual HRESULT StopPlay();		///设定开始播放的标记
	virtual HRESULT PausePlay(IKG3DSceneCameraKeyFrame* pPauseKey, BOOL* bIsPaused);
	

    virtual HRESULT GetName(LPCTSTR *ppcszRetName);	///得到名字
	virtual HRESULT GetName(LPTSTR ppcszRetName, size_t uBufferSize);//上面那个旧的没法改了
    virtual HRESULT SetName(LPCTSTR pcszName);

	virtual HRESULT GetPlayState(BOOL *pbRetIsPlaying);	///检查是否在播放过程中

	
	virtual HRESULT IsKeyFrameEditable(BOOL *pbRetIsKeyFreeEditable);	///返回m_bIsEditable

	virtual HRESULT GetKeyFrameCount(DWORD *pdwKeyFrameCount);		///得到关键帧数量

	virtual HRESULT GetCurSelectedKeyTime(DWORD *pdwRetSelectedKeyTime, DWORD *pdwRetPreKeyTime, DWORD *pdwRetNextKeyTime);
	virtual HRESULT GetCurSelectedKey(IKG3DSceneCameraKeyFrameEx* pKey);
	virtual HRESULT SetCurSelectedKey(const IKG3DSceneCameraKeyFrameEx* pKey);
	virtual HRESULT SetKeySelChangeCallback(KG3DSCAniSelKeyChangeCallback pNewCallback, KG3DSCAniSelKeyChangeCallback* ppOldCallback);
	virtual HRESULT CreateSpecialTrack(DWORD dwType, DWORD_PTR dwParam);

	virtual HRESULT EnableEditCamAniKeyFrame(BOOL bEnable);

	KG3DSceneCameraAnimation(IEKG3DScene& Scene);
	virtual ~KG3DSceneCameraAnimation();	///清除map中所有的关键帧。

private:
	void operator=( KG3DSceneCameraAnimation &source);	///严禁使用拷贝运算符

private:
	IEKG3DScene& m_SceneRef;

};
/************************************************************************/
/* KG3DSceneCameraMovement声明开始
/************************************************************************/

class KG3DSceneCameraMovement : public IEKG3DSceneCameraMovement
{	
public:
	KG3DSceneCameraMovement(const IEKG3DScene& Scene);
	virtual ~KG3DSceneCameraMovement(void);
private:
	void operator=( KG3DSceneCameraMovement &source);	///严禁使用拷贝运算符	
public:
	inline KG3DSceneCameraAnimation* GetCurCamAnimation();
	virtual HRESULT GetCurCamAnimation(IKG3DSceneCameraAnimation **ppiRetCameraAnimation);
	virtual HRESULT GetCurCamAnimation(IEKG3DSceneCameraAnimation **ppiRetCameraAnimation);	///返回当前摄像机动画的指针，如果是无效指针的话，会保证至少返回一个新的Animation。因为这个方法可以保证不会出现无效指针，所以除了速度要求高的帧循环函数之外，绝对不要直接把这个函数的返回值复制使用。指针值的传递返回不会太影响速度
	virtual HRESULT	SetCurCamAnimation( int nIndex );	///设定当前摄像机动画
	virtual HRESULT	NewOneCamAnimation();		///新建一动画
	HRESULT	RemoveOneCamAnimation( int nIndex = -1 );	///移除index动画，如果不加上参数的话，就是最后一个，如果碰到当前的动画指针就是要删除的那个的话，指针自动向前移动。因为在删除之前，当前动画指针就先移动了位置，所以不会引发实时绘制得到的指针失效的情况
	virtual HRESULT RemoveCurCamAnimation();		///移除当前摄像机动画，当前动画指针会自动向前移动。因为在删除之前，当前动画指针就先移动了位置，所以不会引发实时绘制得到的指针失效的情况
	int FindCamAniByName( LPCTSTR pstrCamAniName, int nFinded = 1 );	//用名字找到vector中索引,nFinded表示第几个找到的，默认是1
	virtual HRESULT FindCamAniByName(LPCTSTR pstrCamAniName, int nFinded, int* nRetIndex);///nFinded用1表示要第一个找到的，一般传1
	virtual HRESULT SetCurCamAnimationByName(LPCTSTR pstrCamAniName, int nFinded, IKG3DSceneCameraAnimation **ppiRetCameraAnimation);///传空指针就不返回CamAni
	virtual HRESULT Clear();		///移除所有的CameraAnimation
	virtual HRESULT GetAnimationCount(DWORD *pdwRetAnimationCount);		///得到内部动画个数，如果返回0的话就知道内部为空
	virtual HRESULT	LetCurCamAniPosMoveUp();	///把当前动画在表中上移
	virtual HRESULT LetCurCamAniPosMoveDown();	///把当前动画在表中下移
	virtual HRESULT	SetCurCamAniName( LPCTSTR pCamAniName );		///不允许重名
	virtual HRESULT GetCurCamAniIndex(DWORD *pdwRetAnimationIndex);		///得到当前动画在列表中的位置
	HRESULT SaveToFile(LPSTR pFileName);
	virtual HRESULT LoadFromFile(LPSTR pFileName);

private:
	KG3DSceneCameraAnimation* m_pCurCameraAnimation;
	vector<KG3DSceneCameraAnimation*> m_vecCameraAnimationTable;
	int m_nNewCamAniTimes;	///用于给Animation命名
	IEKG3DScene&	m_SceneRef;
};
