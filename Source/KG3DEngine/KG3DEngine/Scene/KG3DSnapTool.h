#pragma once
#include "KG3DSceneEntityMgr.h"

class KG3DCamera;
class KG3DSceneSceneEditor;

class KG3DSnapTool//截图工具，整个场景
{
	enum 
	{
		SNAP_UNIT_SIZE = 128,
	};
private:
	LPDIRECT3DSURFACE9      m_lpTargetSave;
	LPDIRECT3DSURFACE9      m_lpDepthSave;
	D3DVIEWPORT9            m_viewPortSave;
	float                   m_fScale;           // 每个Cell（100）对应的像素个数
    KG3DSceneEntityMgr	    m_EntityMgrSketch;

	LPDIRECT3DSURFACE9      m_lpTargetSurface;
	LPDIRECT3DSURFACE9      m_lpDepthSurface;
	LPDIRECT3DSURFACE9      m_lpTargetData;
	LPDIRECT3DSURFACE9      m_lpTexSurface;
	
	int     m_nSnapWidth;
    int     m_nSnapHeight;
	float   m_ZNear;
    float   m_ZFar;

	//DWORD  *m_lpSnapBuffer;
	//string  m_szFileName;

    struct StepCameraInfo
    {
        int nX,nZ;
        D3DXVECTOR3 vEye;
        D3DXVECTOR3 vLookAt;
        float fWidth;
        float fHeight;
    };
    list<StepCameraInfo> m_listSetCameraInfo;
    KG3DSceneSceneEditor *m_pSceneEditor;
public:
    KG3DSnapTool(void);
    virtual ~KG3DSnapTool(void);

    HRESULT Init();
    HRESULT UnInit();
    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

    void  SetScale(float fScale);
    void  SetSceneEditor(KG3DSceneSceneEditor *pSceneEditor);

    HRESULT Render(const AABBOX& BoxScene,LPSTR pFileName);
	HRESULT ExportTerrainTexture(const AABBOX& BoxScene,KG3DSceneSceneEditor* pSceneEditor,LPSTR pFileName);
private:
    HRESULT ComputeSnapSize(const AABBOX& BoxScene);

    HRESULT BegainStep();
    HRESULT EndStep(int nX,int nZ);
    HRESULT GetCurrentCameraStep(KG3DCamera* pCamera,int& nX,int& nZ);
};
