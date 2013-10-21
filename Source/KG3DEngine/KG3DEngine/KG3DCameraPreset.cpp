#include "stdafx.h"
#include "kg3dcamerapreset.h"
#include "kg3dmodel.h"
#include "KG3DGraphicsTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

D3DXMATRIX KG3DCameraPreset::m_matTransform[CPT_COUNT];

KG3DCameraPreset::KG3DCameraPreset(void)
{
	static bool s_bMatrixInited = false;
	if (!s_bMatrixInited)
	{
		float fHalfPI = D3DX_PI / 2.0f;
		D3DXMatrixIdentity(&m_matTransform[CPT_FRONT]);
		D3DXMatrixRotationY(&m_matTransform[CPT_BACK], D3DX_PI);
		D3DXMatrixRotationY(&m_matTransform[CPT_RIGHT], fHalfPI);
		D3DXMatrixRotationY(&m_matTransform[CPT_LEFT], -fHalfPI);
		D3DXMatrixRotationX(&m_matTransform[CPT_TOP], fHalfPI);
		D3DXMatrixRotationX(&m_matTransform[CPT_BOTTOM], fHalfPI * 3.0f);
		s_bMatrixInited = true;
	}
}

KG3DCameraPreset::~KG3DCameraPreset(void)
{
}


void KG3DCameraPreset::GetPresetCamera(KG3DModel *pModel, 
									   enuCameraPresetType Type,
									   float fAspect,
									   bool bUsedForMeshMdlThumbnail/*  = false */ )
{
	D3DXVECTOR3 vec3Position, vec3LookAt;
	float fWidth, fHeight, fDepth, fZFar;
	fZFar = 0.0f;
	D3DXMATRIX matTranlation;
	float offset_position = 0.0f;
	KG_PROCESS_ERROR(pModel);

	fWidth = abs(pModel->m_BBox.A.x - pModel->m_BBox.B.x);
	fHeight = abs(pModel->m_BBox.A.y - pModel->m_BBox.B.y);
	fDepth = abs(pModel->m_BBox.A.z - pModel->m_BBox.B.z);

	vec3LookAt.z = (pModel->m_BBox.A.z + pModel->m_BBox.B.z) / 2.0f;
	vec3LookAt.x = pModel->m_BBox.A.x + fWidth / 2.0f;
	vec3LookAt.y = (pModel->m_BBox.A.y + pModel->m_BBox.B.y) / 2;

	if (bUsedForMeshMdlThumbnail)
	{
		TCHAR strFile[MAX_PATH]; 
		TCHAR strDrive[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strExt[MAX_PATH];
		_splitpath_s(pModel->m_scName.c_str(), 
			strDrive, 
			MAX_PATH,
			strPath, 
			MAX_PATH, 
			strFile, 
			MAX_PATH, 
			strExt,
			MAX_PATH);


		offset_position = max(max(fWidth, fHeight), fDepth) / 2.0f / tan(D3DX_PI / 6.0f);

		if (strcmpi(strExt, ".mesh") == 0)
		{

			vec3Position.z = vec3LookAt.z - offset_position;
			vec3Position.x = vec3LookAt.x + offset_position;
			vec3Position.y = vec3LookAt.y + offset_position;
		}
		else
		{
			vec3LookAt.y = (pModel->m_BBox.A.y + pModel->m_BBox.B.y) / 2.0f + fHeight / 5.0f;
			vec3Position.z = vec3LookAt.z - offset_position * 0.65f;
			vec3Position.x = vec3LookAt.x + offset_position * 0.65f;
			vec3Position.y = vec3LookAt.y + offset_position * 0.65f;
		}		
	}
	else
	{
		vec3Position.z = pModel->m_BBox.A.z - max(fWidth, fHeight) / 2.0f / tan(D3DX_PI / 8);
		vec3Position.x = vec3LookAt.x;
		vec3Position.y = vec3LookAt.y;
	}

	fZFar = vec3Position.z - pModel->m_BBox.B.z;

	/*SetUpDirection(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	SetPosition(vec3Position);
	SetLookAtPosition(vec3LookAt);*/
	SetPositionLookAtUp(&vec3Position, &vec3LookAt, &D3DXVECTOR3(0,1,0));
	SetPerspective(D3DX_PI / 4, fAspect, 10.0f, fZFar);
	ComputeMatrix(Type);
	D3DXMatrixTranslation(&matTranlation, 0.0f, 0.0f, -(pModel->m_BBox.A.y + pModel->m_BBox.B.y) / 2.0f);
	m_CameraInfo[Type].matView = /*matTranlation * */m_matTransform[Type] * m_CameraInfo[Type].matView;
Exit0:
	;
}

void KG3DCameraPreset::ComputeMatrix(enuCameraPresetType Type)
{
	D3DXMatrixLookAtLH(&m_CameraInfo[Type].matView, 
		&m_Position, 
		&m_LookAt, 
		&m_DefaultUp);

	if(m_bPerspective)
	{
		D3DXMatrixPerspectiveFovLH(&m_CameraInfo[Type].matProj,
			m_PerspectiveData.fFovy,
			m_PerspectiveData.fAspect,
			m_PerspectiveData.fzNear,
			m_PerspectiveData.fzFar);
	}
	else
	{
		D3DXMatrixOrthoLH(&m_CameraInfo[Type].matProj,
			m_OrthogonalData.fWidth,
			m_OrthogonalData.fHeight,
			m_OrthogonalData.fzNear,
			m_OrthogonalData.fzFar);
	}
}

HRESULT KG3DCameraPreset::SetCamera(enuCameraPresetType Type)
{
	g_cGraphicsTool.SetCamera(this);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &m_CameraInfo[Type].matView);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_CameraInfo[Type].matProj);
	return S_OK;	
}

