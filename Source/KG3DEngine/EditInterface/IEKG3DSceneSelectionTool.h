////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DSceneSelectionTool.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-16 17:49:45
//  Comment     : 以后，选择通过这个进行
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DSCENESELECTIONTOOL_H_
#define _INCLUDE_IEKG3DSCENESELECTIONTOOL_H_

////////////////////////////////////////////////////////////////////////////////
class IEKG3DSceneEntity;
class KG3DRepresentObjectVisitor;
class KG3DSceneEntityVisitor;
class IEKG3DRepresentObject;

interface IEKG3DSceneSelectionToolAccessProxy
{
	/*virtual VOID Begin() = 0;
	virtual BOOL IsEnd() = 0;
	virtual VOID StepForward() = 0;
	virtual IEKG3DSceneEntity* GetCur() = 0;*/

	virtual IEKG3DSceneEntity* Reset() = 0;	//和IEnum一致，迭代器到最前面,如果有元素就返回第一个值
	virtual IEKG3DSceneEntity* Next() = 0;

	virtual ~IEKG3DSceneSelectionToolAccessProxy() = 0{}
};

interface IEKG3DSceneSelectionTool
{
	virtual HRESULT SetSelectable(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelectable) = 0;	//临时方法
	virtual HRESULT SetSelectable(const IEKG3DSceneEntity& Entity, BOOL bSelectable) = 0;

	virtual HRESULT SetSelected(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelected) = 0;	//临时方法
	virtual HRESULT SetSelected(const IEKG3DSceneEntity& Entity, BOOL bSelected) = 0;

	virtual HRESULT SetSelected(IEKG3DRepresentObject& obj, BOOL bSelected) = 0;

	virtual HRESULT ClearSelection() = 0;
	virtual ULONG	GetSelectionCount() = 0;

	virtual HRESULT SelectAll() = 0;
	virtual HRESULT SelectInvert() = 0;

	virtual HRESULT GetAccessProxy(IEKG3DSceneSelectionToolAccessProxy** ppAccessProxy) = 0;	//访问用这个函数,请别嫌用起来麻烦，COM标准里面枚举用的IEnum和这个是形式相近的
};

interface IEKG3DSceneSelectBase
{
	//virtual HRESULT OnToggleStateRotation() = 0;//<OnToggleStateRotation Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
	//virtual HRESULT OnToggleStateMove() = 0;//<OnToggleStateMove Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
	//virtual HRESULT OnToggleStateScaling() = 0;

	//virtual HRESULT AddSelectableEntity(KG3DSceneEntity* Entity) = 0;
	//virtual HRESULT AddSelectableEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer) = 0;
	//virtual HRESULT AddSelectedEntity(IEKG3DSceneEntity* Entity) = 0;
	//virtual HRESULT AddSelectedEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer) = 0;

	//virtual HRESULT RemoveSelectableEntity(KG3DSceneEntity* Entity) = 0;
	//virtual HRESULT RemoveSelectedEntity(IEKG3DSceneEntity* Entity) = 0;
	//virtual HRESULT RemoveSelectableEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer, BOOL bEitherRemoveInSelectedList) = 0;
	//virtual HRESULT RemoveSelectedEntity(SCENEENTITYTYPE EntityType,PVOID EntityPointer) = 0;

	//virtual HRESULT CopySelectedEntity() = 0;//<Copy Refactor>
	//virtual HRESULT ParseCopiedEntity() = 0;//2009-2-11 这个函数改动到SceneEditBase的PasteSelection
	//virtual INT GetCopiedEntityCount() = 0;//2009-1-12 <Copy Refactor>

	//virtual INT GetSelectedEntitySize() = 0;//用IEKG3DSceneSelectionTool的GetSelectionCount
	//virtual HRESULT GetSelectedEntity(INT nIndex, IEKG3DSceneEntity** ppEntity) = 0;

	virtual INT GetSelectableEntitySize() = 0;
	virtual HRESULT GetSelectableEntity(INT nIndex, IEKG3DSceneEntity** ppEntity) = 0;

	//virtual HRESULT ClearSelectedEntity() = 0;
	virtual HRESULT CopySelectable2SelectedSet(LPVOID pBillboard) = 0;

	//svirtual HRESULT SelectNone(DWORD dwClass) = 0;	//改成用IEKG3DSceneSelectionTool::ClearSelection
	//virtual HRESULT SelectAll(DWORD dwClass) = 0;		////改成用IEKG3DSceneSelectionTool
	//virtual HRESULT SelectInvert(DWORD dwClass) = 0;	////改成用IEKG3DSceneSelectionTool
};

//选择的过滤器
//interface ISelectFilter {
//	virtual bool operator()(IEKG3DSceneEntity* pEntity) = 0;
//};//<ISelectFilter Refactor>

#endif //_INCLUDE_IEKG3DSCENESELECTIONTOOL_H_
