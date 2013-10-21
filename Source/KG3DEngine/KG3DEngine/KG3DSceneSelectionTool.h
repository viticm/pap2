////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneSelectionTool.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-1-8 16:43:42
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENESELECTIONTOOL_H_
#define _INCLUDE_KG3DSCENESELECTIONTOOL_H_

#include "IEKG3DSceneSelectionTool.h"
#include "KG3DSceneEntity.h"
////////////////////////////////////////////////////////////////////////////////
class IEKG3DSceneEntity;
class KG3DRepresentObject;
class KG3DRepresentObjectVisitor;
class KG3DSceneEntityVisitor;
class IEKG3DRepresentObject;
class IKG3DMessageReceiverBase;
class KG3DMessageSender;



enum
{
	EM_SELECTION_INCREASE_MODE_DEFAULT = 0,
	EM_SELECTION_INCREASE_MODE_ADD = 1,
	EM_SELECTION_INCREASE_MODE_SUBTRACT = 2,

	EM_SELECTION_GROUP_MAX = 20,

	EM_SELECTION_SELECT_ALL_MAX = 200,
	EM_SELECTION_POINT_SELECT_ALL_MAX = 20,	//PointSelection在选择一串的时候最多处理这个数目
	EM_SELECTION_MIN_TIME_GAP = 30,//最少30毫秒才处理一次选择的消息
};

namespace KG_CUSTOM_HELPERS
{
	struct FrontAccessProxy;
};

typedef KGCH::TFrontAccessProxy<KG3DSceneEntity> TypeEntityAccessProxy;


struct KG3DSceneSelectionTool : public IEKG3DSceneSelectionTool
{
	virtual HRESULT SetSelectable(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelectable) = 0;	//临时方法
	virtual HRESULT SetSelectable(const IEKG3DSceneEntity& Entity, BOOL bSelectable) = 0;
	virtual HRESULT SetSelectable(IEKG3DRepresentObject& obj, BOOL bSelectable) = 0;

	virtual HRESULT SetSelected(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelected) = 0; 
	virtual HRESULT SetSelected(const IEKG3DSceneEntity& Entity, BOOL bSelected) = 0;
	virtual HRESULT SetSelected(IEKG3DRepresentObject& obj, BOOL bSelected) = 0;

	virtual HRESULT SelectAll() = 0;//全选，物件超过EM_SELECTION_SELECT_ALL_MAX的话，失败，并不会选择。不然对于大场景的话会出问题，大部分3D软件都无法避免Ctrl+A之后变很慢的问题。
	virtual HRESULT SelectInvert() = 0;//反选，有物件数限制，同SelectAll

	virtual HRESULT TraverseSelected(KG3DRepresentObjectVisitor& visitor) = 0;
	virtual HRESULT TraverseSelected(KG3DSceneEntityVisitor& visitor) = 0;
	virtual HRESULT TraverseSelectable(KG3DSceneEntityVisitor& Visitor) = 0;

	virtual HRESULT ClearSelectable() = 0;
	virtual HRESULT ClearSelection() = 0;

	virtual ULONG	GetSelectionCount() = 0;

	virtual ULONG	IsSelected(const KG3DRepresentObject& Obj) = 0;
	virtual ULONG	IsSelectable(const KG3DSceneEntity& entity) = 0;

	//保存一个额外的选择组的功能
	virtual HRESULT AllocSelGroup(LPCTSTR pGroupName) = 0;
	virtual HRESULT ClearSelGroup(LPCTSTR pGroupName) = 0;
	virtual ULONG	GetSelGroupSize(LPCTSTR pGroupName) = 0;

	virtual HRESULT CopySelGroupToCurSelection(LPCTSTR pGroupName) = 0;
	virtual HRESULT CopyCurSelectionToSelGroup(LPCTSTR pGroupName) = 0;
	virtual HRESULT CopySelGroupToSelectable(LPCTSTR pGroupName) = 0;
	virtual HRESULT CopySelectableToSelGroup(LPCSTR pGroupName) = 0;

	//virtual HRESULT GetFrontAccessProxy(LPCTSTR pGroupName, KG_CUSTOM_HELPERS::FrontAccessProxy** ppProxy) = 0;	//会创建一个访问代理器，用于逐步的前进防问。生命周期是SelectionTool内部维护的，只能在函数内使用这个方式访问SelectionTool，不能保存这个Proxy。虽然这种形式比Traverse更好用，但也更不安全。

	virtual TypeEntityAccessProxy& GetSelectionAccessProxy() = 0; //由内部维护的访问代理，获得和iteractor类似的功能。因为是内部维护的，所以只能一次使用一个使用者。另外使用的时候不应该使用任何方式改变SelectionTool内部的容器，否则结果可能会出错。如std容器，一边访问一边erase也是很容易出错的，应该尽量避免
	virtual HRESULT GetAccessProxy(IEKG3DSceneSelectionToolAccessProxy** ppAccessProxy) = 0;

	virtual HRESULT GetSelectableList(KG3DSceneEntityList& entities) = 0;	//这个方法是重构临时方法，以后可能要废弃掉，或者改成别的形式。用entity没有引用计数，不安全。传入的是KGSceneEntityList*。这里没法预定义这个指针，所以要用LPVOID

	virtual HRESULT RegisterEventListener(DWORD messageID, IKG3DMessageReceiverBase& receiver) = 0;	//可以侦听KM_SELECT

	virtual HRESULT PointSelection(ULONG uIncreaseMode, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, BOOL bNearsetOrAll) = 0;//PointSelection必须遵循以下原则：如果选择点找不到，那么找最近的物件的BBOX的碰撞点，不然有些物件会很难选择。bNearsetOrAll如果是TRUE则只选择最近的，否则选择一串。
	virtual HRESULT RectSelection(ULONG uIncreaseMode, const D3DXVECTOR4& vLeftTopRightBottom
			, const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXMATRIX& matViewport, FLOAT fSelRange) = 0;

	virtual ULONG	IsNewSelectionTool() = 0;	//选择工具重构完毕之后就可以去掉

#if defined(DEBUG) | defined(_DEBUG)
	virtual KG3DMessageSender* TestBegin() = 0;	//仅仅用于测试的时候用，用于给外部控制其行为,永远是最后一个方法
	virtual VOID TestEnd() = 0;
#endif
};

struct KG3DSceneSelectionToolDummy : public KG3DSceneSelectionTool
{
	virtual HRESULT SetSelectable(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelectable){return E_FAIL;}
	virtual HRESULT SetSelectable(const IEKG3DSceneEntity& Entity, BOOL bSelectable){return E_FAIL;}
	virtual HRESULT SetSelectable(IEKG3DRepresentObject& obj, BOOL bSelectable){return E_FAIL;}

	virtual HRESULT SetSelected(const IEKG3DSceneEntity& Entity, BOOL bSelected){return E_FAIL;}
	virtual HRESULT SetSelected(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer, BOOL bSelected){return E_FAIL;}
	virtual HRESULT SetSelected(IEKG3DRepresentObject& obj, BOOL bSelected){return E_FAIL;}

	virtual HRESULT SelectAll(){return E_FAIL;}//全选，物件超过EM_SELECTION_SELECT_ALL_MAX的话，失败，并不会选择。不然对于大场景的话会出问题，大部分3D软件都无法避免Ctrl+A之后变很慢的问题。
	virtual HRESULT SelectInvert(){return E_FAIL;}//反选，有物件数限制，同SelectAll

	virtual HRESULT TraverseSelected(KG3DRepresentObjectVisitor& visitor){return E_FAIL;}
	virtual HRESULT TraverseSelected(KG3DSceneEntityVisitor& visitor){return E_FAIL;}
	virtual HRESULT TraverseSelectable(KG3DSceneEntityVisitor& Visitor){return E_FAIL;}

	virtual HRESULT ClearSelectable(){return E_FAIL;}
	virtual HRESULT ClearSelection(){return E_FAIL;}

	virtual ULONG	GetSelectionCount(){return 0;}

	virtual ULONG	IsSelected(const KG3DRepresentObject& Obj){return FALSE;}
	virtual ULONG	IsSelectable(const KG3DSceneEntity& entity){return FALSE;}

	virtual HRESULT AllocSelGroup(LPCTSTR pGroupName){return E_FAIL;}
	virtual HRESULT ClearSelGroup(LPCTSTR pGroupName){return E_FAIL;}
	virtual ULONG	GetSelGroupSize(LPCTSTR pGroupName){return 0;}

	virtual HRESULT CopySelGroupToCurSelection(LPCTSTR pGroupName){return E_FAIL;}
	virtual HRESULT CopyCurSelectionToSelGroup(LPCTSTR pGroupName){return E_FAIL;}
	virtual HRESULT CopySelGroupToSelectable(LPCTSTR pGroupName){return E_FAIL;}
	virtual HRESULT CopySelectableToSelGroup(LPCSTR pGroupName){return E_FAIL;}

	//virtual HRESULT GetFrontAccessProxy(LPCTSTR pGroupName, KG_CUSTOM_HELPERS::FrontAccessProxy** pProxy){return E_FAIL;}

	virtual TypeEntityAccessProxy& GetSelectionAccessProxy();
	virtual HRESULT GetAccessProxy(IEKG3DSceneSelectionToolAccessProxy** ppAccessProxy){return E_FAIL;}

	virtual HRESULT GetSelectableList(KG3DSceneEntityList& entities){return E_FAIL;}

	virtual HRESULT RegisterEventListener(DWORD messageID, IKG3DMessageReceiverBase& receiver){return E_FAIL;}

	virtual HRESULT PointSelection(ULONG uIncreaseMode, const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, BOOL bNearestOrAll){return E_FAIL;}
	virtual HRESULT RectSelection(ULONG uIncreaseMode, const D3DXVECTOR4& vLeftTopRightBottom
		, const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXMATRIX& matViewport, FLOAT fSelRange){return E_FAIL;}

	virtual ULONG	IsNewSelectionTool(){return TRUE;}

#if defined(DEBUG) | defined(_DEBUG)
	virtual KG3DMessageSender* TestBegin(){return NULL;}
	virtual VOID TestEnd(){}
#endif
};

#endif //_INCLUDE_KG3DSCENESELECTIONTOOL_H_
