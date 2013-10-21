#pragma once
//#include "scene.h"
//#include "KDlg_PropertyDialog.h"
//#include "Operation.h"
//#include "MeshMoveGrid.h"
//#include "MeshRotateGrid.h"


/////////////////////////////////////////////////////



//class KSceneEditBase :
//	public K3DScene , public KEditOperationHistory , public PropertyBase::KPropertyBase
//{
//public:
//	HRESULT Acquire();
//
//	class _SceneOperatorBase : public KEditOperation
//	{
//	public:
//		DWORD m_dwOperator;
//		DWORD m_dwOption;
//		ENTITYLIST m_EntityList;
//		PVOID      m_pData;
//		KSceneEditBase* m_lpSceneEditBase;
//	public:
//		_SceneOperatorBase()
//		{
//			m_dwOperator = 0;
//			m_dwOption   = 0;
//			m_pData     = NULL;
//			m_lpSceneEditBase = NULL;
//		}
//
//		_SceneOperatorBase(DWORD Operator)
//		{
//			m_dwOperator = Operator;
//			m_pData = NULL;
//		}
//
//		virtual ~_SceneOperatorBase()
//		{
//			m_dwOperator = 0;
//			m_dwOption   = 0;
//			m_EntityList.clear();
//			SAFE_DELETE_ARRAY(m_pData);
//		}
//
//		virtual HRESULT BackupData(KSceneEditBase* pScene);		
//		virtual HRESULT Undo(KSceneEditBase* pScene);
//
//		virtual HRESULT Undo()
//		{
//			if(m_lpSceneEditBase)
//			{
//				return Undo(m_lpSceneEditBase);
//			}
//			return S_OK;
//		}
//	protected:
//
//	};
//	PropertyBase::KDlg_PropertyDialog*  m_lpDlgPropertySet;//属性设置工具条
//
//	DWORD              m_dwSelectState;//0,1,2 普通，加选，减选
//
//	BOOL               m_bEditing;//是否处于编辑状态，是则处理对应的鼠标键盘消息
//
//	KModelGridMove*    m_lpMoveGrid;
//	KModelGridRotate*  m_lpRotateGrid;
//
//	DWORD   m_dwState;
//	DWORD   m_dwSubState;
//
//	POINT   m_PointSelectStart;
//	POINT   m_PointSelectEnd;
//	BOOL    m_bShowSelectionRect;
//
//	DWORD   m_GridTextureID;
//	BOOL    m_bMeshEdit;
//	DWORD   m_LastOperation;
//
//	POINT   m_LastMousePos;
//	D3DXVECTOR3 m_SelectedCenter;
//
//
//	K3DScene::ENTITYLIST m_SelectedList;
//	K3DScene::ENTITYLIST m_SelectedListBack;
//	K3DScene::ENTITYLIST m_SelectableList;
//	K3DScene::ENTITYLIST m_ClipBoardList;
//	K3DScene::ENTITYLIST m_LastOperationList;
//
//public:
//	virtual HRESULT ShowProperty(BOOL bShow);
//	virtual HRESULT OnSelectChanged();
//	virtual HRESULT OnEditMoving();
//	virtual HRESULT OnEditRotating();
//	virtual HRESULT OnEditScaling();
//
//	HRESULT GetFirstSelectedEntity(KSceneEntity& Entity);
//
//	HRESULT RemoveSelectedObject(LPOBJECT pObject);
//
//	HRESULT CleanUp();
//
//	virtual HRESULT Edit_Undo();
//	virtual HRESULT Edit_Copy();
//	virtual HRESULT Edit_Paste();
//	virtual HRESULT Edit_Cut();
//	virtual HRESULT Edit_Select();
//
//	HRESULT RemoveSelectedEntity(SCENEENTITYTYPE EntityType,PVOID pEntity);
//	HRESULT RemoveSelectableEntity(SCENEENTITYTYPE EntityType,PVOID pEntity);
//
//	HRESULT AddSelectedEntity(SCENEENTITYTYPE EntityType,PVOID pEntity);
//	HRESULT AddSelectableEntity(SCENEENTITYTYPE EntityType,PVOID pEntity);
//
//	HRESULT AddSelectableMesh(LPMODEL pMesh);
//	HRESULT DrawSelectObjectBox(LPSCENEENTITY pObj);
//
//	HRESULT RestoreLastOperation_Delete();
//	HRESULT AddLastOperation_Delete(LPOBJECT pObject);
//
//	HRESULT RestoreLastOperation_Cut();
//	HRESULT RecordLastOperation_Cut();
//	HRESULT AddLastOperation_Cut(LPOBJECT pObject);
//
//	HRESULT RestoreLastOperation_Select();
//	HRESULT RecordLastOperation_Delete();
//	HRESULT RemoveSelectableObject(LPOBJECT pObject);
//	HRESULT GetSelectCenter(D3DXVECTOR3* pCenter);
//	HRESULT Edit_DeleteSelectedObject();
//
//	HRESULT Initialize(PVOID pSceneTable);
//
//	virtual void    ONKEYDOWN(WPARAM wParam, LPARAM lParam);
//	virtual void    ONLBUTTONDOWN(WPARAM wParam, LPARAM lParam);
//	virtual void    ONLBUTTONUP(WPARAM wParam, LPARAM lParam);
//	virtual void    ONMOUSEMOVE(WPARAM wParam, LPARAM lParam);
//	virtual void    ONMOUSEWHEEL(WPARAM wParam, LPARAM lParam);
//	virtual void    ONRBUTTONDOWN(WPARAM wParam, LPARAM lParam);
//
//	HRESULT ClearLastOperationList();
//	HRESULT AddLastOperation_Select(LPOBJECT pObject);
//	HRESULT RecordLastOperation_Select();
//
//	HRESULT AddSelectableObject(LPOBJECT pObject);
//	HRESULT AddSelectedObject(LPOBJECT pObject);
//	HRESULT AddSelectedMesh(KModel* pMesh);
//	HRESULT CleanSelection();
//	HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
//
//	KSceneEditBase(void);
//	virtual ~KSceneEditBase(void);
//
//	/*virtual HRESULT Edit_Undo();
//	virtual HRESULT Edit_DeleteSelectedObject();
//	virtual HRESULT Edit_Copy();
//	virtual HRESULT Edit_Paste();
//	virtual HRESULT Edit_CutSelectedObject();
//	virtual HRESULT Edit_DropSelectedObject();
//	virtual HRESULT Edit_Select();*/
//	HRESULT Render(void);
//
//	HRESULT RemoveSelectedEntity(KSceneEntity Entity);
//	HRESULT RemoveSelectableEntity(KSceneEntity Entity);
//
//	protected:
//		HRESULT DrawSelectObjectInfo(LPSCENEENTITY pObj);
//};
