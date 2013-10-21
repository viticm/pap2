#pragma once
#include "../../../Include/kg3dengine/kg3dinterface.h"
#include "../EditInterface/IEOther.h"	//by dengzhihui Refactor 2006年11月2日
#include "KG3DRecorder.h"
class KG3DScene;
class KG3DTrackCamera;
class KG3DCamera :
	public IEKG3DCamera //by dengzhihui Refactor 2006年11月2日
{
protected :
	D3DXVECTOR3 m_Position;
	D3DXVECTOR3 m_LookAt;
	BOOL        m_bPerspective;         //  当前的投影模式，同时用于验证释放，不要改成bool就好
    BOOL        m_bGlobalPerspective;   //  主投影模式
    BOOL        m_bPerspectiveSave;
	D3DXVECTOR3 m_DefaultUp;

	KG3DCameraPersepectiveData m_PerspectiveData;
	KG3DCameraPersepectiveData m_GlobalPerspectiveData;
	KG3DCameraPersepectiveData m_PerspectiveDataSave;

	KG3DCameraOrthogonalData m_OrthogonalData;
	KG3DCameraOrthogonalData m_GlobalOrthogonalData;
	KG3DCameraOrthogonalData m_OrthogonalDataSave;

	D3DXMATRIX m_MatrixView;
	D3DXMATRIX m_MatrixProject;

	KG3DScene*      m_lpCurScene;           // 当前场景，因为Scene肯定是比Camera后释放的，所以Camera有效，Scene就有效，不用担心生命周期

    D3DXVECTOR3     m_vOffset;

    D3DXVECTOR3     m_vRight;
    D3DXVECTOR3     m_vFront;
    D3DXVECTOR3     m_vUp;

public:
	UINT m_uSceneSpaceNode;
	DWORD m_dwRecordID;		

    virtual HRESULT ClearOffset() { m_vOffset = D3DXVECTOR3(0.f, 0.f, 0.f);  return S_OK; }
	virtual HRESULT ComputeMatrix();	
    virtual HRESULT FrameMove() {return E_NOTIMPL;}
	virtual KG3DTrackCamera* GetTrackCameraInterface(){return NULL;}
private:
	//碰撞用::RayIntersection，转调给Scene，这个是为了方步外部用的，内部不要用，所以放在private里面
	virtual BOOL GetCrossPosition(const D3DXVECTOR3& VS, const D3DXVECTOR3& VD, D3DXVECTOR3* pVecCrossPos);
	void ComputeViewMatrix();
	void ComputePerspectiveMatrix();
protected:
	virtual BOOL RayIntersectionCamera(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDir, FLOAT fTestRange, FLOAT* pfRet);

	//下面这些方法是TrackCamera用的，但Camera本身不实现这些方法
public:
	virtual HRESULT GetStation(IEKG3DCameraStation** pStation){return E_NOTIMPL;}
	virtual HRESULT SetTrackInfo(TrackCameraInfo  trackInfo){return E_NOTIMPL;}
	virtual HRESULT GetTrackInfo(TrackCameraInfo* pTrackInof){return E_NOTIMPL;}
	virtual DWORD GetCurrentState(){return 0;}
public:
	const KG3DCameraPersepectiveData&	GetPerspectiveData()	{return m_PerspectiveData;}
	const KG3DCameraOrthogonalData&		GetOrthogonalData()		{return m_OrthogonalData;}

	const D3DXMATRIX&					GetViewMatrix(){return m_MatrixView;}
	const D3DXMATRIX&					GetProjectionMatrix(){return m_MatrixProject;}

    HRESULT Capture();			//保存当前的Presepective数据
    HRESULT RestoreCapture();	//重新恢复当前的Presepective数据
    HRESULT RestoreGlobalProject();
    HRESULT GetGlobalPerspective(float* pfFovy, float* pfAspect, float* pfzNear, float* pfzFar);
    HRESULT SetGlobalPerspective(float fFovy, float fAspect, float fzNear, float fzFar);
    HRESULT GetGlobalOrthogonal(float* pfWidth, float* pfHeight, float* pfzNear, float* pfzFar);
    HRESULT SetGlobalOrthogonal(float fWidth, float fHeight, float fzNear, float fzFar);
    HRESULT GetNearPlaneRect(D3DXVECTOR3& A, D3DXVECTOR3& B, D3DXVECTOR3& C, D3DXVECTOR3& D, D3DXVECTOR3& Center);

	//Camera的Set在UpdateCamera中
    inline D3DXVECTOR3 GetCameraRight(){ return m_vRight; }
    inline D3DXVECTOR3 GetCameraFront(){ return m_vFront; }
    inline D3DXVECTOR3 GetCameraUp()   { return m_vUp; }

	inline D3DXVECTOR3 GetPosition(){return m_Position;}
    inline D3DXVECTOR3 GetLookAtPosition() { return m_LookAt; }
    inline void SetSwingOffset(const D3DXVECTOR3& vOffset) { m_vOffset = vOffset; }

	HRESULT SetCamera();//设定View和Projection矩阵
	HRESULT	PushMatrixToDevice();	//把Matrix设到Device里面，不调用的话，不会主动设

	virtual IKG3DCamera* GetNew();
	virtual HRESULT Copy(IKG3DCamera* pCamera);
    virtual HRESULT UpdateCamera(D3DXVECTOR3 vecPosition, D3DXVECTOR3 vecTargetPos
								, float fLookAtOffset, float fTargetSpeed);//如果不是TrackCamera的话，就完全由外部控制，没有遮挡检测和弹簧的功能
	
	virtual HRESULT BindScene(KG3DScene* lpCurScene);	//绑定一个Scene
	virtual HRESULT UnBindScene();	//以后要重新改进，不要Camera里面放Scene指针了
	/************************************************************************/
	/*
	为了要Matrix和各个Vector同步，每Set一下就要重新计算一次Matrix，很耗的
	用下面的函数一次设置多个Vector，避免重复计算Matrix，如果只要设其中的一个
	或几个Vector，其它参数设为NULL
	*/
	/************************************************************************/
	virtual HRESULT SetPositionLookAtUp(const D3DXVECTOR3* pPos, const D3DXVECTOR3* pLookAt, const D3DXVECTOR3* pUp);//一次设3个量可以加速
	virtual HRESULT GetPosition(D3DXVECTOR3 *pPos);
	virtual HRESULT SetPosition(D3DXVECTOR3 Pos ,BOOL bOutCall = FALSE);//尽量用SetPositionLookAtUp一次设3个值，分开设的话，每次都要计算一次矩阵
	virtual HRESULT SetLookAtPosition(D3DXVECTOR3 Pos,BOOL bOutCall = FALSE);//尽量用SetPositionLookAtUp一次设3个值，分开设的话，每次都要计算一次矩阵
	
	virtual HRESULT GetLookAtPosition(D3DXVECTOR3 *pPos);
	virtual HRESULT GetUpDirection(D3DXVECTOR3 *pUp);
	virtual HRESULT SetUpDirection(D3DXVECTOR3 Up, BOOL bOutCall = FALSE);//尽量用SetPositionLookAtUp一次设3个值，分开设的话，每次都要计算一次矩阵

	virtual HRESULT GetFront(D3DXVECTOR3* pFront);
	virtual HRESULT GetRight(D3DXVECTOR3* pRight);

	int				GetQuadrant();

	virtual BOOL    IsPerspective();

	virtual HRESULT GetPerspective(float *pfFovy, float *pfAspect, float *pfzNear, float *pfzFar);
	virtual HRESULT SetPerspective(float fFovy, float fAspect, float fzNear, float fzFar);

	virtual HRESULT GetOrthogonal(float *pfWidth, float *pfHeight, float *pfzNear, float *pfzFar);//获取正交投影参数
	virtual HRESULT SetOrthogonal(float fWidth, float fHeight, float fzNear, float fzFar);

	virtual HRESULT GetMatrix(D3DXMATRIX* pMatrixView, D3DXMATRIX* pMatrixProjection);
///////////ADD BY HAUNGJINSHOU 2008-1-30/////////////////////////
public:
	float GetHeightDistance();
	void  SetHeightDistance(float& fVal);
	float m_fHeightDistance; //当前摄像机位置与目标位置的高度差
//////////////////////////////////////////////////////////////////
public:
	KG3DCamera(void);
	virtual ~KG3DCamera(void);
private:
	//必须保证用下面这几个函数，不然摄像机动画不能运作
	void SetPositionPrivate(const D3DXVECTOR3& Pos);
	void SetLookAtPostionPrivate(const D3DXVECTOR3& Pos );
	void SetUpDirectionPrivate(const D3DXVECTOR3& Up );
};
