#pragma once
#include "KG3DScene.h"
#include ".\kg3dsceneselectbase.h"
#include "IEEditor.h"

interface IEKG3DSceneLayer;
class     KG3DSceneLayer;
class	  KG3DMessageReceiver;
class	KG3DTransformTool;
struct KG3DSelectionBBoxGetter;

class KG3DSceneEditorBase :
	public KG3DScene, public KG3DSceneSelectBase, public IEKG3DSceneEditorBase
{
public:
	virtual HRESULT SetEditState(DWORD dwState); 
	virtual HRESULT GetMouseXY(INT* pX, INT* pY);
	virtual HRESULT SetMouseXY(INT pX, INT pY);
	virtual ULONG GetEditState(){return m_dwEditState;}

	virtual void    SetRtsActive(BOOL active) { m_RtsActive = active; }
    virtual BOOL    GetRtsActive() { return m_RtsActive; }
    BOOL m_RtsActive;
	BOOL    m_bEditing;         //是否处于编辑状态

	float   m_fWheel;

    int     m_bEnableMoveCam;
	
	KG3DSceneLayer         *m_pRootLayer;
	
	//vector<KG3DSceneLayer *>m_vecSceneLayer;
	virtual KG3DSceneSelectionTool&	GetSelectionTool();
	virtual KG3DTransformTool& GetTransformTool(DWORD dwType);//Type是KG3DTYPE_TRANSLATION_TOOL等

	virtual HRESULT GetSelectionToolInterface(IEKG3DSceneSelectionTool** ppTool);

	virtual void __stdcall OnReceiveMessage(const KG3DMessage& Message , IKG3DMessageSenderBase* pSender);
public:
	void MsgOnMouseMove(int fwKeys,int xPos,int yPos,KG3DSceneOutputWnd *pWnd);

	virtual HRESULT Init();
	virtual HRESULT UnInit();

	//virtual HRESULT EnableEditCameraKeyFrame(BOOL bEdit);
	virtual HRESULT GetFirstSelectedModel(IEKG3DModel*& pModel);

	virtual HRESULT FrameMove();
	virtual HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>
public:
	KG3DSceneEditorBase(void);
	virtual ~KG3DSceneEditorBase(void);
	//(by dengzhihui 2006年9月13日 15:38:32 //增加快捷键的特殊处理
	virtual INT ExecAction(INT nActionID, INT nExecState, WPARAM wParam, LPARAM lParam);
	virtual INT ExecActionSelect(INT nActionID, INT nExecState, INT nXOff, INT nYOff);
	virtual void SyncPickRay();	//由外部的鼠标消息调用

	//void ZoomView(short nDelta, POINT pt);
	virtual HRESULT SetEditing(DWORD dwEditing);
	virtual DWORD GetEditing();

	/*enum enumCommandType{ctUp, ctDown, ctLeft, ctRight, ctPageDown, ctPageUp, ctHome, ctEnd, ctInc, ctDec, 
	ctHideObject, ctHideTerrain, ctHideGrass, ctAddKeyFrame, ctDropObject};*/
	virtual void OnCommand(enumCommandType eCommandType);
	//virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>//2009-1-14 注掉，没有检查到任何引用
	virtual HRESULT GetRootLayer(IEKG3DSceneLayer** pLayer);
	//virtual INT GetSceneLayerSize();
	//virtual HRESULT GetSceneLayer(INT nIndex , IEKG3DSceneLayer** pLayer);
	//virtual HRESULT ClearSelectedEntity();
	virtual HRESULT GetSceneSelector(IEKG3DSceneSelectBase** ppSelector);
	virtual HRESULT GetEditState(DWORD* pdwState);

	virtual HRESULT PasteSelection();

	virtual HRESULT AddRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer);
	virtual HRESULT RemoveRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer);
	virtual HRESULT AddRenderEntity(KG3DSceneEntity Entity, BOOL bIsSelectable = TRUE, BOOL bIsSelected = FALSE);
	virtual HRESULT RemoveRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable = TRUE);
	virtual HRESULT RemoveRenderEntity(KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList = TRUE);
	/*
	对于PVS等编辑器，是不需要复杂的地形管理的，只需要放几个物件就够了，原来是用listSelectable来管理的。
	但是这是物件管理，这并不符合Selectable的意义，和SceneSceneEditor的理念很冲突。
	所以这里暂时重构成特殊化的管理器，把除SceneSceneEditor之外的物件原来的SetEntitySelectable放这里

	在重构完成前，暂时会利用旧的选择器
	*/
	
	HRESULT InsertEntitySelectable(KG3DSceneEntity& entity, BOOL bInsertAndSetSelectable);//<Selectable Refactor>//当BOOL为FALSE的时候，同时清理出去
	virtual HRESULT IEInsertEntitySelectable(SCENEENTITYTYPE dwType, LPVOID pPoint, BOOL bInsertAndSetSelectable);//<Selectable Refactor>
private:
	DWORD   m_dwEditState;//用接口操作这个。EditState的变化是非常敏感的，必须用方法操作，不然不能截获的话，很多东西会有问题
	KG3DMessageReceiver*		m_pCustomReceiver;	//接收SelectionTool等的消息

	KG3DSelectionBBoxGetter*	m_pSelectionBoxGetter;	//Stratege模式，选择器的功能应该最小化，计算其总Box的算法是高层算法，应该脱离底层实现
protected:
	KG3DSceneEntityList	m_CtEntities;
protected:
	virtual void MsgOnLButtonDown(WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>
	virtual void MsgOnLButtonUp(WPARAM wParam, LPARAM lParam);
	virtual void MsgOnRButtonDown(WPARAM wParam, LPARAM lParam);
	virtual void MsgOnRButtonUp(WPARAM wParam, LPARAM lParam);
	virtual void MsgOnMButtonDown(WPARAM wParam, LPARAM lParam);
	virtual void MsgOnMButtonUp(WPARAM wParam, LPARAM lParam);
	virtual void MsgOnMouseWheel(WPARAM wParam, LPARAM lParam);
	virtual void MsgOnMouseMove(WPARAM wParam, LPARAM lParam);//新工具接手之后，同名函数可以删除
	virtual void MsgOnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual void MsgOnKeyUp(WPARAM wParam, LPARAM lParam);
	

	virtual HRESULT DeleteSelectedEntity();

	KG3DSelectionBBoxGetter& GetSelectionBBoxGetter();
	HRESULT	SetSelectionBBoxGetter(KG3DSelectionBBoxGetter& newGetter, KG3DSelectionBBoxGetter** ppOld);//如果Set进来的话，生命周期会被接管，旧的会在结果中传出来，按需要保存或者Delete

private:
	static VOID TDDCheck(LPVOID pExtraData);
	static VOID TDDCheckAfterInitialized(LPVOID pExtraData);
protected:
	virtual VOID	ZoomToSelection();
};
