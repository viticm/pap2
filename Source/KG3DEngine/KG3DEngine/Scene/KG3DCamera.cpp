#include "StdAfx.h"
#include ".\kg3dcamera.h"
#include "KG3DRecorder.h"
#include "KG3DIntersection.h"
#include "KG3DMesh.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const float QrtTwo = 1.41421356f;

KG3DCamera::KG3DCamera(void)
:m_lpCurScene(NULL)
,m_bPerspectiveSave(TRUE)
{
	m_dwRecordID = 0;
	m_Position = D3DXVECTOR3(0,500,-1000);
	m_LookAt   = D3DXVECTOR3(0,0,0);
	m_DefaultUp= D3DXVECTOR3(0,1,0);
    m_vOffset  = D3DXVECTOR3(0.f, 0.f, 0.f);

    m_vRight   = D3DXVECTOR3(1.f, 0.f, 0.f);
    m_vFront   = D3DXVECTOR3(0.f, 0.f, 1.f);
    m_vUp      = D3DXVECTOR3(0.f, 1.f, 0.f);

	m_bPerspective = TRUE;
    m_bGlobalPerspective             = TRUE;

    m_GlobalPerspectiveData.fAspect  = 1.3333f;
    m_GlobalPerspectiveData.fFovy    = D3DXToRadian(55.f);
    m_GlobalPerspectiveData.fzNear   = 10.f;
    m_GlobalPerspectiveData.fzFar    = 100000.f;

    m_GlobalOrthogonalData.fHeight   = 30.f;
    m_GlobalOrthogonalData.fWidth    = 30.f;
    m_GlobalOrthogonalData.fzNear    = 10.f;
    m_GlobalOrthogonalData.fzFar     = 100000.f;

	m_uSceneSpaceNode = 0;//0为非法值
	m_fHeightDistance = 0.0f;

    ComputeViewMatrix();
    RestoreGlobalProject();
}

KG3DCamera::~KG3DCamera(void)
{
}
IKG3DCamera* KG3DCamera::GetNew()
{
	return new KG3DCamera;
}

HRESULT KG3DCamera::Copy(IKG3DCamera* pCamera)
{
	KG3DCamera* p3DCamera = static_cast<KG3DCamera*>(pCamera);
	if (!p3DCamera)
		return E_FAIL;
	*p3DCamera = *this;
	return S_OK;	
}

HRESULT KG3DCamera::UpdateCamera(D3DXVECTOR3 vecPosition, D3DXVECTOR3 vecTargetPos, float fLookAtOffset, float fTargetSpeed)
{
    return SetPositionLookAtUp(&vecPosition, &vecTargetPos, NULL);
}

HRESULT KG3DCamera::GetPosition(D3DXVECTOR3 *pPos)
{
	(*pPos) = m_Position;
	return S_OK;
}
HRESULT KG3DCamera::SetPosition(D3DXVECTOR3 Pos,BOOL bOutCall)
{
	if (!bOutCall && g_cRecorder.IsRecord() &&  m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(D3DXVECTOR3); 			
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(D3DXVECTOR3*)(pData) = Pos;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DCAMERA_SETPOSITION,GetCurrentTime(),MyParameters);
	}   
	SetPositionPrivate(Pos);
	ComputeViewMatrix();
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_MatrixView);
	return S_OK;
}

HRESULT KG3DCamera::SetLookAtPosition(D3DXVECTOR3 Pos,BOOL bOutCall)
{
	if (!bOutCall && g_cRecorder.IsRecord() &&  m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(D3DXVECTOR3); 			
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(D3DXVECTOR3*)(pData) = Pos;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DCAMERA_SETLOOKATPOSITION,GetCurrentTime(),MyParameters);
	}   
	SetLookAtPostionPrivate(Pos);
	ComputeViewMatrix();
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_MatrixView);
	return S_OK;
}

HRESULT KG3DCamera::GetLookAtPosition(D3DXVECTOR3 *pPos)
{
	(*pPos) = m_LookAt;
	return S_OK;
}


BOOL KG3DCamera::IsPerspective()
{
	return m_bPerspective;
}

HRESULT KG3DCamera::GetPerspective(
							   float *pfFovy,
							   float *pfAspect,
							   float *pfzNear,
							   float *pfzFar
							   )
{
	if (pfFovy)
		(*pfFovy) = m_PerspectiveData.fFovy;
	if (pfAspect)
		(*pfAspect) = m_PerspectiveData.fAspect;
	if (pfzNear)
		(*pfzNear) = m_PerspectiveData.fzNear;
	if (pfzFar)
		(*pfzFar) = m_PerspectiveData.fzFar;

	return S_OK;
}
HRESULT KG3DCamera::SetPerspective(
							   float fFovy,
							   float fAspect,
							   float fzNear,
							   float fzFar
							   )
{
	//if (g_bIsRecord &&  m_dwRecordID != 0)
	//{
	//	KG3DRecorder::Parameters MyParameters;
	//	MyParameters.nNum = 4;
	//	MyParameters.size_tDataSize = sizeof(float) * 4; 			
	//	char* pData = new char[MyParameters.size_tDataSize];
	//	MyParameters.pData = pData;
	//	MyParameters.nFrom[0] = 0;		
	//	*(float*)(pData) = fFovy;
	//	MyParameters.nFrom[1] = sizeof(float);		
	//	*(float*)(&pData[MyParameters.nFrom[1]]) = fAspect;
	//	MyParameters.nFrom[2] = sizeof(float) * 2;		
	//	*(float*)(&pData[MyParameters.nFrom[2]]) = fzNear;
	//	MyParameters.nFrom[3] = sizeof(float) * 3;		
	//	*(float*)(&pData[MyParameters.nFrom[3]]) = fzFar;

	//	g_cRecorder.RecordEvent(m_dwRecordID,
	//		"SetPerspective(float fFovy,float fAspect,float fzNear,float fzFar)",
	//		GetCurrentTime(),MyParameters);
	//} 
	m_PerspectiveData.fFovy = fFovy;
	m_PerspectiveData.fAspect = fAspect;
	m_PerspectiveData.fzNear = fzNear;
	m_PerspectiveData.fzFar = fzFar;
    m_bPerspective = TRUE;
	//SetCamera();
	ComputePerspectiveMatrix();
	if(NULL != g_pd3dDevice)
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_MatrixProject);
	return S_OK;
}

HRESULT KG3DCamera::GetOrthogonal(
							  float *pfWidth,//获取正交投影参数
							  float *pfHeight,
							  float *pfzNear,
							  float *pfzFar )
{
	if (pfWidth)
		(*pfWidth) = m_OrthogonalData.fWidth;
	if (pfHeight)
		(*pfHeight) = m_OrthogonalData.fHeight;
	if (pfzNear)
		(*pfzNear) = m_OrthogonalData.fzNear;
	if (pfzFar)
		(*pfzFar) = m_OrthogonalData.fzFar;
	return S_OK;
}
HRESULT KG3DCamera::SetOrthogonal(
							  float fWidth,
							  float fHeight,
							  float fzNear,
							  float fzFar )
{
	//if (g_bIsRecord &&  m_dwRecordID != 0)
	//{
	//	KG3DRecorder::Parameters MyParameters;
	//	MyParameters.nNum = 4;
	//	MyParameters.size_tDataSize = sizeof(float) * 4; 			
	//	char* pData = new char[MyParameters.size_tDataSize];
	//	MyParameters.pData = pData;
	//	MyParameters.nFrom[0] = 0;		
	//	*(float*)(pData) = fWidth;
	//	MyParameters.nFrom[1] = sizeof(float);		
	//	*(float*)(&pData[MyParameters.nFrom[1]]) = fHeight;
	//	MyParameters.nFrom[2] = sizeof(float) * 2;		
	//	*(float*)(&pData[MyParameters.nFrom[2]]) = fzNear;
	//	MyParameters.nFrom[3] = sizeof(float) * 3;		
	//	*(float*)(&pData[MyParameters.nFrom[3]]) = fzFar;

	//	g_cRecorder.RecordEvent(m_dwRecordID,
	//		"SetOrthogonal(float fWidth,float fHeight,float fzNear,float fzFar )",
	//		GetCurrentTime(),MyParameters);
	//} 

	m_OrthogonalData.fWidth = fWidth;
	m_OrthogonalData.fHeight = fHeight;
	m_OrthogonalData.fzNear = fzNear;
	m_OrthogonalData.fzFar = fzFar;
	m_bPerspective = FALSE;
	//SetCamera();
	ComputePerspectiveMatrix();
	if(NULL != g_pd3dDevice)
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_MatrixProject);
	return S_OK;
}

HRESULT KG3DCamera::GetUpDirection(D3DXVECTOR3 *pUp)
{
	(*pUp) = m_DefaultUp;
	return S_OK;
}

HRESULT KG3DCamera::SetUpDirection(D3DXVECTOR3 Up,BOOL bOutCall)
{
	if (!bOutCall && g_cRecorder.IsRecord() &&  m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 1;
		MyParameters.size_tDataSize = sizeof(D3DXVECTOR3); 			
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(D3DXVECTOR3*)(pData) = Up;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DCAMERA_SETUPDIRECTION,GetCurrentTime(),MyParameters);
	} 
	SetUpDirectionPrivate(Up);
	//SetCamera();
	ComputeViewMatrix();
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_MatrixView);
	return S_OK;
}

HRESULT KG3DCamera::ComputeMatrix()
{	
	ComputeViewMatrix();
	ComputePerspectiveMatrix();
	return S_OK;
}

int KG3DCamera::GetQuadrant()
{
	D3DXVECTOR3 dirc =m_LookAt - m_Position;
	dirc.y = 0.0f;

	D3DXVec3Normalize(&dirc,&dirc);

	if(dirc.x > 0.00001)
	{
		if(abs(dirc.z) <= QrtTwo / 2.0f)
			return 2;
		else if(dirc.z > 0.00001)
			return 1;
		else
			return 3;
	}
	else if(dirc.x < -0.00001)
	{
		if(abs(dirc.z) <= QrtTwo / 2.0f)
			return 4;
		else if(dirc.z > 0.00001)
			return 1;
		else
			return 3;
	}

	if(dirc.z > 0.0001)
		return 1;
	else
		return 3;
}

HRESULT KG3DCamera::SetCamera()
{
	//ComputeMatrix();//不用Compute，每个地方都会自动Compute
	if(NULL != g_pd3dDevice)
	{
		g_pd3dDevice->SetTransform(D3DTS_VIEW,&m_MatrixView);
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&m_MatrixProject);
	}
	return S_OK;
}

HRESULT KG3DCamera::GetNearPlaneRect(D3DXVECTOR3& A, D3DXVECTOR3& B, D3DXVECTOR3& C, D3DXVECTOR3& D, D3DXVECTOR3& Center)
{
    D3DXVECTOR3 vRight = GetCameraRight();
    D3DXVECTOR3 vUp    = GetCameraUp();
    D3DXVECTOR3 vFront = GetCameraFront();
    D3DXVECTOR3 vPos;

    FLOAT fNear  = m_PerspectiveData.fzNear; 
    FLOAT fVel   = tanf(m_PerspectiveData.fFovy / 2) * fNear * 2;
    FLOAT fHov   = m_PerspectiveData.fAspect * fVel;


    GetPosition(&vPos);

    D3DXVec3Normalize(&vRight, &vRight);
    D3DXVec3Normalize(&vUp, &vUp);
    D3DXVec3Normalize(&vFront, &vFront);

    Center = vPos + vFront * fNear;
    A = Center + vUp * fVel / 2 - vRight * fHov / 2;
    B = A + vRight * fHov;
    C = B - vUp * fVel;
    D = C - vRight * fHov;

    return S_OK;
}




float KG3DCamera::GetHeightDistance()
{
	return m_fHeightDistance;
}

void KG3DCamera::SetHeightDistance(float& fVal)
{
	m_fHeightDistance = fVal;
}

HRESULT KG3DCamera::GetMatrix( D3DXMATRIX* pMatrixView, D3DXMATRIX* pMatrixProjection )
{
	if (NULL != pMatrixView)
	{
		*pMatrixView = m_MatrixView;
	}
	if (NULL != pMatrixProjection)
	{
		*pMatrixProjection = m_MatrixProject;
	}

	return S_OK;
}

HRESULT KG3DCamera::Capture()
{
	m_bPerspectiveSave = m_bPerspective;

	if (m_bPerspectiveSave)
		m_PerspectiveDataSave = m_PerspectiveData;
	else
		m_OrthogonalDataSave  = m_OrthogonalData;

	return S_OK;
}

HRESULT KG3DCamera::RestoreCapture()
{
	m_bPerspective = m_bPerspectiveSave;
	if (m_bPerspective)
		m_PerspectiveData = m_PerspectiveDataSave;
	else
		m_OrthogonalData  = m_OrthogonalDataSave;
	//return SetCamera();
	ComputePerspectiveMatrix();
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_MatrixProject);
	return S_OK;
}

HRESULT KG3DCamera::RestoreGlobalProject()
{
	if (m_bGlobalPerspective)
	{
		return SetPerspective(
			m_GlobalPerspectiveData.fFovy,  m_GlobalPerspectiveData.fAspect, 
			m_GlobalPerspectiveData.fzNear, m_GlobalPerspectiveData.fzFar
			);
	}
	else
	{
		return SetOrthogonal(
			m_GlobalOrthogonalData.fWidth, m_GlobalOrthogonalData.fHeight,
			m_GlobalOrthogonalData.fzNear, m_GlobalOrthogonalData.fzFar
			);
	}
}

HRESULT KG3DCamera::GetGlobalPerspective( float* pfFovy, float* pfAspect, float* pfzNear, float* pfzFar )
{
	(*pfFovy)   = m_GlobalPerspectiveData.fFovy;
	(*pfAspect) = m_GlobalPerspectiveData.fAspect;
	(*pfzNear)  = m_GlobalPerspectiveData.fzNear;
	(*pfzFar)   = m_GlobalPerspectiveData.fzFar;

	return S_OK;
}

HRESULT KG3DCamera::SetGlobalPerspective( float fFovy, float fAspect, float fzNear, float fzFar )
{
	if (g_cRecorder.IsRecord() &&  m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 4;
		MyParameters.size_tDataSize = sizeof(float) * 4; 			
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(float*)(pData) = fFovy;
		MyParameters.nFrom[1] = sizeof(float);
		*(float*)(&pData[MyParameters.nFrom[1]]) = fAspect;
		MyParameters.nFrom[2] = sizeof(float) * 2;
		*(float*)(&pData[MyParameters.nFrom[2]]) = fzNear;
		MyParameters.nFrom[3] = sizeof(float) * 3;
		*(float*)(&pData[MyParameters.nFrom[3]]) = fzFar;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DCAMERA_SETGLOBALPERSPECTIVE,GetCurrentTime(),MyParameters);
	}
	
	_ASSERTE(fFovy > 0 && fFovy < D3DX_PI);
	_ASSERTE(fAspect > 0 && fAspect < 1000.0);//长宽比不可能超过1000。这个值不对会引起很多问题
	_ASSERTE(fzNear > 0);
	_ASSERTE(fzFar > fzNear);//其实是有可能ZFar<ZNear的，不过这里不考虑奇异矩阵的情况，要用奇异矩阵在函数内部自己修改View，不要和Camera搅合

	m_GlobalPerspectiveData.fFovy   = fFovy;
	m_GlobalPerspectiveData.fAspect = fAspect;
	m_GlobalPerspectiveData.fzNear  = fzNear;
	m_GlobalPerspectiveData.fzFar   = fzFar;
	m_bGlobalPerspective            = TRUE;

	return S_OK;
}

HRESULT KG3DCamera::GetGlobalOrthogonal( float* pfWidth, float* pfHeight, float* pfzNear, float* pfzFar )
{
	(*pfWidth)  = m_GlobalOrthogonalData.fWidth;
	(*pfHeight) = m_GlobalOrthogonalData.fHeight;
	(*pfzNear)  = m_GlobalOrthogonalData.fzNear;
	(*pfzFar)   = m_GlobalOrthogonalData.fzFar;

	return S_OK;
}

HRESULT KG3DCamera::SetGlobalOrthogonal( float fWidth, float fHeight, float fzNear, float fzFar )
{
	if (g_cRecorder.IsRecord() &&  m_dwRecordID != 0 && !g_cRecorder.IsLock())
	{
		KG3DRecorder::Parameters MyParameters;
		MyParameters.nNum = 4;
		MyParameters.size_tDataSize = sizeof(float) * 4; 			
		char* pData = new char[MyParameters.size_tDataSize];
		MyParameters.pData = pData;
		MyParameters.nFrom[0] = 0;		
		*(float*)(pData) = fWidth;
		MyParameters.nFrom[1] = sizeof(float);
		*(float*)(&pData[MyParameters.nFrom[1]]) = fHeight;
		MyParameters.nFrom[2] = sizeof(float) * 2;
		*(float*)(&pData[MyParameters.nFrom[2]]) = fzNear;
		MyParameters.nFrom[3] = sizeof(float) * 3;
		*(float*)(&pData[MyParameters.nFrom[3]]) = fzFar;
		g_cRecorder.RecordEvent(m_dwRecordID,FT_KG3DCAMERA_SETGLOBALORTHOGONAL,GetCurrentTime(),MyParameters);
	} 
	m_GlobalOrthogonalData.fWidth  = fWidth;
	m_GlobalOrthogonalData.fHeight = fHeight;
	m_GlobalOrthogonalData.fzNear  = fzNear;
	m_GlobalOrthogonalData.fzFar   = fzFar;
	m_bGlobalPerspective           = FALSE;

	return S_OK;
}

BOOL KG3DCamera::GetCrossPosition( const D3DXVECTOR3& VS, const D3DXVECTOR3& VD, D3DXVECTOR3* pVecCrossPos )
{
	FLOAT fRet = FLT_MAX;

	D3DXVECTOR3 vDir = VD - VS;
	FLOAT fTestRange = D3DXVec3Length(&vDir);

	_ASSERTE(fTestRange > 0 && _T("两点距离为0，不能形成发射线"));
	if(fTestRange < FLT_EPSILON)
		return FALSE;

	D3DXVec3Normalize(&vDir, &vDir);

	BOOL bRet = RayIntersectionCamera(VS, vDir, fTestRange, &fRet);
	if(bRet && NULL != pVecCrossPos)
	{
		*pVecCrossPos = VS + vDir * fRet;
	}
	return bRet;
}

BOOL KG3DCamera::RayIntersectionCamera( const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDir, FLOAT fTestRange, FLOAT* pfRet )
{
	struct KG3DCameraIntersectionFilter : public KG3DIntersectionFilter
	{
		virtual HRESULT Accept(KG3DModelST& ModelST){return E_FAIL;}//绝对不和ModelST碰撞
		virtual HRESULT Accept(KG3DMesh& Mesh)						//不和不阻塞Camera的物件碰撞
		{
			return Mesh.GetObjProperty().bObscatleCamera ? S_OK : E_FAIL;
		}
	};

	if (NULL == m_lpCurScene)
		return FALSE;

	const FLOAT fRetDistBias = 1.01f;
	FLOAT fRangeUse = min(fTestRange * fRetDistBias, (FLOAT)EM_RAY_INTERSECTION_MAX_RANGE);	//需要有个最小的情况

	static KG3DCameraIntersectionFilter stFilter;

	FLOAT fTempRet = FLT_MAX;
	BOOL bRet = ::RayIntersection(*m_lpCurScene, vSrc, vDir
		, &fTempRet, NULL, NULL, fRangeUse, &stFilter);

	_ASSERTE(!bRet || (fTempRet > -FLT_EPSILON && fTempRet < fRangeUse));
	if (!(fTempRet < fTestRange))
		return FALSE;

	if (bRet && NULL != pfRet)
	{
		*pfRet = fTempRet;
	}
	return bRet;
}

HRESULT KG3DCamera::SetPositionLookAtUp(const D3DXVECTOR3* pPos, const D3DXVECTOR3* pLookAt, const D3DXVECTOR3* pUp )
{
	if(NULL != pPos)		
		SetPositionPrivate(*pPos);
	if(NULL != pLookAt)		
		SetLookAtPostionPrivate(*pLookAt);
	if(NULL != pUp)
		SetUpDirectionPrivate(*pUp);

	ComputeViewMatrix();
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_MatrixView);

	return S_OK;
}


void KG3DCamera::ComputeViewMatrix()
{
	D3DXVECTOR3 vOffset = D3DXVECTOR3(0.f, 0.f, 0.f);
	if (m_bPerspective)
	{
		vOffset = m_vOffset;
	}
	if (vOffset != D3DXVECTOR3(0.f, 0.f, 0.f))
	{
		D3DXVECTOR3 front;
		D3DXVECTOR3 right;
		D3DXVECTOR3 up;

		D3DXVec3Normalize(&front, &D3DXVECTOR3(m_MatrixView._13, m_MatrixView._23, m_MatrixView._33));
		D3DXVec3Normalize(&right, &D3DXVECTOR3(m_MatrixView._11, m_MatrixView._21, m_MatrixView._31));
		D3DXVec3Normalize(&up,    &D3DXVECTOR3(m_MatrixView._12, m_MatrixView._22, m_MatrixView._32));

		vOffset = front * vOffset.z + right * vOffset.x + up * vOffset.y;
	}
	
	D3DXMatrixLookAtLH(&m_MatrixView, &(m_Position + vOffset), &(m_LookAt + vOffset), &m_DefaultUp);

	D3DXVec3Normalize(&m_vRight, &D3DXVECTOR3(m_MatrixView._11, m_MatrixView._21, m_MatrixView._31));
	D3DXVec3Normalize(&m_vFront, &D3DXVECTOR3(m_MatrixView._13, m_MatrixView._23, m_MatrixView._33));
	D3DXVec3Normalize(&m_vUp,    &D3DXVECTOR3(m_MatrixView._12, m_MatrixView._22, m_MatrixView._32));


}

void KG3DCamera::ComputePerspectiveMatrix()
{
	if(m_bPerspective)
	{
		D3DXMatrixPerspectiveFovLH(&m_MatrixProject,
			m_PerspectiveData.fFovy,
			m_PerspectiveData.fAspect,
			m_PerspectiveData.fzNear,
			m_PerspectiveData.fzFar);
	}
	else
	{
		D3DXMatrixOrthoLH(&m_MatrixProject,
			m_OrthogonalData.fWidth,
			m_OrthogonalData.fHeight,
			m_OrthogonalData.fzNear,
			m_OrthogonalData.fzFar);
	}
}

HRESULT KG3DCamera::BindScene( KG3DScene* lpCurScene )
{
	if (NULL != lpCurScene)
	{
		m_lpCurScene = lpCurScene;
		return S_OK;
	}
	return E_FAIL;
		
}

HRESULT KG3DCamera::GetFront( D3DXVECTOR3* pFront )
{
	if(NULL == pFront)
		return E_FAIL;
	*pFront = GetCameraFront();
	return S_OK;
}

HRESULT KG3DCamera::GetRight( D3DXVECTOR3* pRight )
{
	if(NULL == pRight)
		return E_FAIL;
	*pRight = GetCameraRight();
	return S_OK;
}

HRESULT KG3DCamera::PushMatrixToDevice()
{
	if (NULL != g_pd3dDevice)
	{
		g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_MatrixView);
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_MatrixProject);
		return S_OK;
	}
	return E_FAIL;
}

void KG3DCamera::SetPositionPrivate( const D3DXVECTOR3& Pos )
{
	m_Position = Pos;
}

void KG3DCamera::SetLookAtPostionPrivate( const D3DXVECTOR3& Pos )
{
	m_LookAt = Pos;
}

void KG3DCamera::SetUpDirectionPrivate( const D3DXVECTOR3& Up )
{  
	m_DefaultUp = Up;
}

HRESULT KG3DCamera::UnBindScene()
{
	m_lpCurScene = NULL;
	return S_OK;
}