#pragma once
#include "kg3dsceneeditorbase.h"
#include "kg3dsceneSelectBase.h"
#include "kg3dmodel.h"
#include "kg3dmaterialex.h"
#include "KG3DSpeedTreeLeafGroup.h"
#include "IEEditor.h"
#include "KG3DToolsImpl.h"
#include "KG3DTerrainBlock.h"
class KG3DAnimationTagContainer;
class KG3DBillboardCloudCreater;
class KG3DBillboardCloudCreater2;
class KSceneModelEditorMeshBindDialog;
class KG3DSceneModelEditorPlayerTools;
interface IEKG3DModelSpeedTree;
struct KG3DSelectionBBoxGetter;

class KG3DSceneModelEditor : public CKG3DConnectionPoint<IEKG3DSceneModelEditor>, public KG3DSceneEditorBase
{
	friend class KSceneModelEditorMeshBindDialog;
protected:
	KG3DSceneEntityList  m_listPostRenderEntity;

	HRESULT AddPostRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable);

	HRESULT RenderMainWindow(KG3DSceneOutputWnd& wndCur);
	HRESULT RenderPreviewWindow(KG3DSceneOutputWnd* pWnd);
	HRESULT RenderPreviewWindowNpcPortrait(KG3DSceneOutputWnd* pWnd);
	
	virtual void HandleEngineMessages( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );	//继承至KG3DSceneSelectBase
public:
	//KG3DTerrainConverMap m_converMap;//改成用全局的KG3DTerrainConverMap::GetUniqueDefault()
	virtual void SetfBoneWeight(float& fWeight);
	virtual void SetbOnlyShowPitchOn(BOOL val);
	virtual BOOL GetbOnlyShowPitchOn();
	//by huangjinshou 2007 8.9
	virtual HRESULT LoadSFXMeteor(LPCSTR pFileName,LPCSTR pSocketName,IEKG3DModel* piModel);
	virtual void    ReleaseSFXMeteor(); 
//(by dengzhihui Refactor 2006年11月23日
	//virtual HRESULT SetAmbient(DWORD dwAmbient);
	//virtual HRESULT CloneSunlight(IEKG3DScene *piScene);
	virtual HRESULT GetScene(IEKG3DScene** ppScene);
	virtual HRESULT GetSceneEditorBase(IEKG3DSceneEditorBase** ppSceneBase);
	virtual HRESULT GetSceneSelector(IEKG3DSceneSelectBase** ppSelector);
	virtual HRESULT GetSelectionToolInterface(IEKG3DSceneSelectionTool** ppTool);

	virtual HRESULT LoadModel(LPCSTR strFileName, 
		IKG3DModel **ppModel);

	virtual HRESULT PlayerNormalTool_LoadStandModel(LPCSTR pFileName);
	virtual HRESULT PlayerNormalTool_AddMDL(LPCSTR pFileName);
	virtual HRESULT PlayerNormalTool_AddModelFromIni(LPCSTR pFileName);
	virtual HRESULT PlayerNormalTool_ProcessAllModel();
	virtual HRESULT PlayerNormalTool_SetCurrentModelPart(int nPart,int nIndex);
	virtual HRESULT PlayerNormalTool_GetModelPartVector(int nIndex,vector<string>* pvec);

	virtual HRESULT GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR3* pIn);
	virtual HRESULT GetSocketCount(INT* nCount);
	virtual HRESULT GetSocketName(INT nIndex, LPTSTR lpName, INT nLen);
	virtual HRESULT GetModelForSockets(INT nIndex, IEKG3DModel** ppModel);
	virtual HRESULT ProcessEntity(BOOL bAdd);
	virtual HRESULT MoveSelectEntityToBack();
	virtual HRESULT ClearSelection();
	virtual HRESULT PushEntityToSelected(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer);
	virtual HRESULT CopyBackToSelectedEntity();
//)
    virtual HRESULT RemoveModel(IEKG3DModel* pModel);
	//virtual HRESULT GetSelectedEntityBBox(AABBOX& SelectedEntityBox);	//用GetSelectionBBoxGetter实现
	//virtual HRESULT GetSelectedEntityCenter(D3DXVECTOR3* pPos);
	virtual HRESULT GetCurModel(IEKG3DModel **ppiRetModel);
	KG3DSceneModelEditorPlayerTools* m_lpPlayerTools;

	void MsgOnKeyDown(WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>
	void MsgOnLButtonUp(WPARAM wParam, LPARAM lParam);

	void ClearModel();
	HRESULT AddModel(IEKG3DModel* pModel);
	
	HRESULT OpenModel(IEKG3DModel* pModel, BOOL bAddMode);

	HRESULT OpenModel(LPCSTR strMeshFile,
		LPCSTR strMaterialFile,
		LPCSTR strAniamtionFile,
                BOOL bAddMode,
		IEKG3DModel** ppModel);

	virtual HRESULT AddRenderEntity(IKG3DResourceBase *pEntity,    unsigned uClass, BOOL bSelectable = TRUE ); ///覆盖KG3DScene的同名方法，以达到可以选取的目的。chentianhong 2006-08-14，
    virtual HRESULT AddRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer); ///为了兼容，覆盖KG3DScene的同名方法。chentianhong 2006-08-21
    virtual HRESULT AddRenderEntity(const KG3DSceneEntity& Entity, BOOL bIsSelectable = TRUE, BOOL bIsSelected = FALSE);	//不知道为什么不能自动访问Scene的同名函数，这里加上一个转移调用
	
	HRESULT Init();
	HRESULT UnInit();

	virtual HRESULT RenderForEachWindow(KG3DSceneOutputWnd& wndCur);
	//HRESULT GetCurMesh(KG3DModel** ppModel);
	KG3DSceneModelEditor(void);
	virtual ~KG3DSceneModelEditor(void);
	
	//HRESULT NewOneLight(KG3DLight** ppLight);
	HRESULT LoadModelForSockets(const TCHAR* strFileName);
	
	void SetEditLeafMode(BOOL bMode);
	void OnCommandLeafEditMode(enumCommandType eCommandType);
	void OnCommand(enumCommandType eCommandType);
	virtual void MsgOnMouseWheel(WPARAM wParam, LPARAM lParam);
	virtual HRESULT SetSpeedTreeUIFunction(pFunction pCallBack,
		void *pDlg);
	HRESULT FrameMove();

	/*virtual HRESULT SelectEnd(D3DVIEWPORT9 &Viewport,
		D3DXMATRIX &matProjection,
		D3DXMATRIX &matView);*/
	//virtual HRESULT OnToggleStateScaling();//<OnToggleStateScaling Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态

	virtual void __stdcall OnReceiveMessage(const KG3DMessage& Message , IKG3DMessageSenderBase* pSender);
	virtual BOOL IsDrawGound();
	virtual void SetDrawGound(BOOL bDraw);
    HRESULT SetFloorSlipSpeed(float fSpeed);
	HRESULT SetFloorTexture(LPCTSTR szFilePath);
        void IsolateCurrentSelection();
	BOOL GetIsolateMode();

    IDirect3DSurface9*  GetBillBoardTexture(DWORD dwHeight, DWORD dwWidth);

	virtual HRESULT GetAllModels(vector<IEKG3DModel*>* pvecModels);
	HRESULT GetAllModelsForModel(vector<IEKG3DModel*>* pvecModels,KG3DModel* pModel);
	HRESULT ZoomToObj();
	virtual HRESULT SetEditState(DWORD dwState);

	virtual HRESULT GenerateBspForMesh(LPSTR pFileName);
	virtual HRESULT GenerateLodForMesh(LPSTR pFileName,int& nMediumFace,int& nLowFace,float fLowRation,float fMediumRation);
	
	virtual void EnableLOD(BOOL bEnable);
public:
	class KG3DSpeedTreeModifier : public IEKG3DSpeedTreeModifier
	{
		friend class KG3DSceneModelEditor;
		KG3DSpeedTreeLeafGroup Leaves;
		void* pEditorDlg;
		pFunction pfUpdateUI;
		KG3DSceneModelEditor* pModelEditor;
		KG3DModelSpeedTree* pSpeedTree;
	public:	
		enum 
		{
			ENU_SCALE_UNIFORM,
			ENU_SCALE_LEAF,
			ENU_SCALE_COUNT,
		};

		void SetSpeedTree(KG3DModelSpeedTree* pSpeedTree);
		KG3DSpeedTreeModifier();
		void SetModelEditor(KG3DSceneModelEditor *pModelEditor) { this->pModelEditor = pModelEditor; }
		virtual BOOL SelectLeaf(IELeaf* pLeaf);
		virtual BOOL UnSelectLeaf(IELeaf* pLeaf);
		virtual void DeleteLeaf(IELeaf* pLeaf);
		virtual void AddLeaf(int nBoneIndex);
		void DeleteSelectedLeaf();
		BOOL IsActive();
		KG3DSceneEntity ConvertToEntity();
        //KG3DSceneEntityList ConvertLeavesToEntityList();
		HRESULT SetAllLeavesSelectable(KG3DSceneEditorBase& editBase);
		DWORD GetScaleMode();
		void SetScaleMode(DWORD dwScaleMode);
		void UnSelectAll();
		void SetAllLeafRenderFlag(BOOL bFlag);
		void SetAllLeafSelectableFlag(BOOL bFlag);
		void SetAllLeafSelectedFlag(BOOL bFlag);
	};

	KG3DSpeedTreeModifier* GetSpeedTreeModifier();
	
	virtual HRESULT GetSpeedTreeModifier(IEKG3DSpeedTreeModifier** pModifier);
        void RestoreIsolated();
	virtual IEKG3DBillBoardCloudCreater* GetBillBoardCloudCreater();//<BillboardCloudV1>
	virtual IEKG3DBillboardCloudCreater2* GetBillboardCloudCreater2();//<BillboardCloud>
	virtual HRESULT AddOutputWindow(LPCTSTR szName, HWND hWnd, DWORD dwType, DWORD_PTR dwOption, int* pOutputWindowID);
	virtual HRESULT ShockWaveRender();
	virtual HRESULT RemoveRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable);
	virtual HRESULT RemoveRenderEntity(KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList = TRUE);
	virtual IEKG3DAnimationTagContainer* GetTagContainer();
	vector<KG3DModel*> m_Models;
private:
	static LPCTSTR s_strIsoSelGroup;	//选择由选择工具管理，外界不能直接取得内部内容，将由选择组来管理这种情况
	static LPCTSTR s_strSeletableBack;	
	//KG3DSceneEntityList m_lstIsolated;//2009-1-8 选择组不能明确暴露，现在改成用一个字符串来访问
	//KG3DSceneEntityList m_listSelectableEntityBack;

	static LPCTSTR s_strSelBackUp;
	//KG3DSceneEntityList m_listSelectedEntityBack;//选中的

	std::vector<KG3DModel*> m_vecModelsForSockets;
	BOOL m_bEditLeaves;
	KG3DSpeedTreeModifier m_SpeedTreeModifier;
	
	KG3DBillboardCloudCreater* m_lpBillboardCloudCreater;//<BillboardCloudV1>
	KG3DBillboardCloudCreater2* m_lpBillboardCloudCreater2;//<BillboardCloud>
	
	BOOL m_bIsolated;
	BOOL m_bEnableSelection;

    //
    DWORD        m_dwPrevTime;
    KG3DTexture* m_pFloorTexture;
    int          m_nShowFloor;
    float        m_fFloorMoveSpeed;

    D3DXVECTOR2  m_vUVLT;
    D3DXVECTOR2  m_vUVRB;
	KG3DAnimationTagContainer* m_pTagContainer;
	vector<KG3DSFX*>   m_VecpSFXMeteor;//用于测试刀光绑定在人物上的效果，
	std::vector<IKG3DModel*> m_vecModelLoaded;
	//KG3DSFXSuede m_SFXSuede;   //Use to test Meteor
	//KG3DModel*  m_pItemModel; //Use to test Meteor
	BOOL m_bDrawGroundGrid;
private:
	KG3DSelectionBBoxGetter* BuildCustomBBoxGetter(KG3DSceneModelEditor& editor, KG3DSelectionBBoxGetter* pDefaultBBoxGetter);
protected:
	VOID ZoomToSelection();
};

extern float g_fModelRenderTwiceCullDistance;
