#ifndef KG3D_UI_MINI_SCENE_H
#define KG3D_UI_MINI_SCENE_H

#include "../../../Include/KG3DEngine/KG3DInterface.h"
#include "./KG3DCamera.h"
#include "KG3DTerrainBlock.h"

class KG3DPhysiscScene;
class KG3DUIMiniScene : public IKG3DScene
{
public:
	KG3DUIMiniScene();
	virtual ~KG3DUIMiniScene();

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(/* [in] */ REFIID riid, /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WOption, DWORD_PTR LOption);///<CTH>
	virtual HRESULT RefreshOption(unsigned uOption);
	virtual HRESULT SaveToFile(const char cszFileName[], unsigned uOption);
	virtual unsigned GetType();
	virtual DWORD    GetID();
	virtual HRESULT PrepareRender(unsigned uOption); 
	virtual HRESULT PostRender(); 
	virtual HRESULT GetDateInformation(int *pnMemory, int *pnVideoMemory, int *pnCompressMem, int *pnCompressVideoMem);
	virtual unsigned GetLastUseTime();
	virtual unsigned GetUseFrequency();
	virtual HRESULT Render(unsigned uOption);
    virtual HRESULT EnablePostRenderEffect(int nOutputWindowID, BOOL bEnable);
    virtual HRESULT EnablePostRenderEffect(int nOutputWindowID, PostRenderEffectType nType, BOOL bEnable);
	virtual HRESULT GetMinimapLayer(IN const D3DVECTOR &vPos, OUT int &nLayer);
	virtual HRESULT GetCameraMovement(IKG3DSceneCameraMovement** ppiCameraMovement);
	virtual HRESULT InitGameTimeClock(KG3D_GAMETIME_INITSTRUCT* pInitStruct);
	virtual HRESULT GetFloor(const D3DXVECTOR3* pvInput, FLOAT* pfRet);
	virtual HRESULT GetWaterHeight(D3DXVECTOR3* pOut, D3DXVECTOR3* pIn);
	virtual HRESULT GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR3* pIn);
	virtual HRESULT LogicalToScene(D3DXVECTOR3* pPos, float fX, float fY, float fZ, BOOL bOnFloor);
	virtual HRESULT SceneToLogical(int* pX, int* pY, int* pZ, D3DXVECTOR3 Pos);
    virtual HRESULT ScenePosToScreenPos(D3DXVECTOR3 const& vScenePos, float& fScreenX, float& fScreenY);
	//virtual HRESULT GetSelectModelList(const D3DXVECTOR2 &vScale,std::vector<IKG3DModel*> &pList);
	//virtual HRESULT GetSelectModelListForClient(const D3DXVECTOR2 &vScale,std::vector<IKG3DModel*> &pList) { return E_NOTIMPL; }
	virtual HRESULT GetSelectModelList(const D3DXVECTOR2& vPosScreen, std::vector<IKG3DModel*>& pList, BOOL bGetNearestOrAll);
	//virtual HRESULT GetSelectModelList(const D3DXVECTOR2 &vScaleLeftUp,const D3DXVECTOR2 &vScale1RightBottom, std::vector<IKG3DModel*> &pList);
	virtual HRESULT GetSelectPosition(D3DXVECTOR2 const& vScale, D3DXVECTOR3& vPosition);
	virtual HRESULT AddOutputWindow(LPCTSTR szName, HWND hWnd, DWORD dwType, DWORD_PTR dwOption, int* pOutputWindowID);
	virtual HRESULT RemoveOutputWindow(int nOutputWindowID);
	virtual HRESULT ClearAllHelpLine();
	virtual HRESULT SetHelpLineSize(size_t nLine, size_t nSize);
	virtual HRESULT SetHelpLineStart(size_t nLine, size_t nStart);
	virtual HRESULT SetHelpLine(size_t nLine, int nSetID, size_t nBegin, size_t nCount, D3DCOLOR color, D3DXVECTOR3* pPos);
	virtual HRESULT GetHelpLine(size_t nLine, size_t* pPoint, D3DXVECTOR3** ppPos);

    virtual HANDLE  AddSceneSigns(const D3DXVECTOR3& vPos) { return 0; }
    virtual void    DelSceneSings(HANDLE Handle)           {}
	virtual HRESULT FrameMove();
	virtual HRESULT RenderToScreen(D3DXVECTOR2 const* pA, D3DXVECTOR2 const* pC, D3DXVECTOR2 const* pUVA, D3DXVECTOR2 const* pUVC, float fZ, DWORD dwFlags);

	virtual HRESULT AddRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable,BOOL bLight);
	virtual HRESULT RemoveRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable);

	virtual IKG3DCamera* GetCurrentCamera();
	virtual HRESULT SetCurrentCamera(IKG3DCamera *pCamera);

    virtual HRESULT GetTerrainCross(D3DXVECTOR3* vInter, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vDst);

	virtual HRESULT OnResizeWindow(PRECT const pRect);
	virtual DWORD   GetGroundType(const D3DXVECTOR3 &vec3Pos) { return INVALID_GROUNDTYPE; }

	virtual float GetLoadingProcess();
	virtual HRESULT RemoveSelectableModel(IKG3DResourceBase *pEntity) { _ASSERTE(0); return S_OK; }
	virtual HRESULT SetFocus(D3DXVECTOR3& vecPos);

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

private:
	HRESULT Render(DWORD dwFlags);

private:
    typedef std::vector<KG3DModel *> KRenderEntityList;
	KRenderEntityList m_RenderEntities;
	KG3DCamera m_3DCamera;

    KG3DPhysiscScene* m_pPhysicsScene;
    DWORD m_LastFrameMoveTime;

	//KG3DTerrainConverMap m_converMap;//改成用全局的KG3DTerrainConverMap::GetUniqueDefault()
};


#endif // KG3D_UI_MINI_SCENE_H
