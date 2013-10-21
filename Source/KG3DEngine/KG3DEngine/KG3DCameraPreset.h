#pragma once
#include "scene\kg3dcamera.h"

class KG3DModel;
class KG3DCameraPreset : public KG3DCamera
{
public:
	enum enuCameraPresetType
	{
		CPT_FRONT,
		CPT_TOP,
		CPT_LEFT,
		CPT_RIGHT,
		CPT_BOTTOM,
		CPT_BACK,
		CPT_USER,
		CPT_COUNT,
		CPT_FORCE_DWORD = 0xfffffff,
	};
	
	KG3DCameraPreset(void);
	~KG3DCameraPreset(void);
	
	HRESULT SetCamera(enuCameraPresetType Type);
	void ComputeMatrix(enuCameraPresetType Type);
	void GetPresetCamera(KG3DModel *pModel, 
		enuCameraPresetType Type,
		float fAspect,
		bool bUsedForMeshMdlThumbnail = false);
private:
	KG3DModel *m_pCurrentModel;
	struct CameraInfo
	{
		D3DXMATRIX matView;
		D3DXMATRIX matProj;
		float fAspect;
		BOOL bInited;
		CameraInfo()
		{
			bInited = FALSE;
			fAspect = 0.0f;
		}
	};

	CameraInfo m_CameraInfo[CPT_COUNT];
	static D3DXMATRIX m_matTransform[CPT_COUNT];
};
