////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSceneTreeCtrl.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-2-21 16:11:35
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESCENETREECTRL_H_
#define _INCLUDE_KSCENESCENETREECTRL_H_

////////////////////////////////////////////////////////////////////////////////
class KSceneCustomScrollBar;

struct TreeItemVisitor 
{
	virtual HRESULT Accept(HTREEITEM hItem) = 0;//返回E_XX会中断流程
	virtual ~TreeItemVisitor() = 0{}
};

class KSceneSceneTreeCtrl;

struct KSceneSceneTreeCtrlEventHandler 
{
	virtual BOOL OnItemExpanded(KSceneSceneTreeCtrl& tree, HTREEITEM hItemExpander){return TRUE;}//返回值暂时没有用
	virtual BOOL OnItemDelete(KSceneSceneTreeCtrl& tree, HTREEITEM hItemDelete){return TRUE;}//返回值没有用
	virtual BOOL OnItemInserted(KSceneSceneTreeCtrl& tree, HTREEITEM hItemInserted){return TRUE;}//返回值没有用
	virtual BOOL OnItemSelectChange(KSceneSceneTreeCtrl& tree, HTREEITEM hItemInserted, HTREEITEM hItemLastSelect){return TRUE;}//返回值没有用
	virtual BOOL OnItemExpanding(KSceneSceneTreeCtrl& tree, HTREEITEM hItemExpanding){return TRUE;}//返回值没有用
	virtual ~KSceneSceneTreeCtrlEventHandler() = 0{}
};

class KSceneSceneTreeCtrl : public CTreeCtrl
	//, public KSUIHELPER::CWndCallbackFrameWork<KSceneSceneTreeCtrl, HTREEITEM, 7>
{
public:
	/*struct TreeEnumCallBackStruct 
	{
	INT m_nLevel;
	INT m_nEnumCounted;
	};*/
	INT GetChildItemCount(HTREEITEM hItem);
	INT GetItemExpandedPos(HTREEITEM hItem);///找到Item是展开的Item中从上排下来的第几个Item。从1开始
	BOOL CopyAllChildItems(HTREEITEM hSrc, HTREEITEM hDes);
	BOOL IsItemExpanded(HTREEITEM hItem);
	HTREEITEM GetRootItemOfItem(HTREEITEM hItem);
	BOOL IsItemHandleValid(HTREEITEM hItem);//负数hItem有可能无来由的在MFC的内部实现中当掉，所以不允许,

	HRESULT TraverseItems(HTREEITEM hItem, TreeItemVisitor& visitor);//传TVI_ROOT就遍历整个树
	//BOOL TraverseItemsOnce(TypeFuncCallback Func, DWORD_PTR dwParam, HTREEITEM hItem = TVI_ROOT);
	HRESULT TraverseItemsExpanded(HTREEITEM hItem, TreeItemVisitor& visitor);
	//BOOL TraverseItemsExpandedOnce(HTREEITEM hItem, TreeItemVisitor& visitor);
	INT GetExpandedTreeItemCount(HTREEITEM hItem = TVI_ROOT);
	BOOL BindVScrollBar(KSceneCustomScrollBar* pScrollBarOutter);//绑定了VScrollBar之后，滚动的时候会通知外部的VScrollBar
	virtual HTREEITEM InsertItemEx(CString Text
		, HTREEITEM hParent = TVI_ROOT
		, HTREEITEM hInsertAfter = TVI_LAST
		, LPARAM* plParam = NULL
		, INT nImage = -1
		, INT nSelectedImage = -1
		, UINT* puState = NULL
		, UINT  uStateMask = 0
		);
	HTREEITEM GetNextSiblingOrUpperSiblingItem(HTREEITEM hItem);///找到下一个同级或者更高级的Item
	HRESULT SetEventHandler(KSceneSceneTreeCtrlEventHandler* pEventHandler);	//注意生命周期，内部只是简单保存指针

	afx_msg void OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnTvnItemexpanded(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
protected:
	CScrollBar*	m_pScrollBarVert;
	VOID SynScrollBar();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	HRESULT EnumChildItems(HTREEITEM hItem, TreeItemVisitor& visitor);
	HRESULT EnumChildExpandedItems(HTREEITEM hItem, TreeItemVisitor& visitor);
	/*static DWORD_PTR CALLBACK EnumExpandedCountCallback(KSceneSceneTreeCtrl* pTreeCtrl
	, HTREEITEM hItem
	, DWORD_PTR pTreeEnumCallBackStruct
	, DWORD_PTR pExtraParam);*/
	/*static DWORD_PTR CALLBACK EnumExpandedItemPosCallback(KSceneSceneTreeCtrl* pTreeCtrl
	, HTREEITEM hItem
	, DWORD_PTR pTreeEnumCallBackStruct
	, DWORD_PTR pExtraParam);*/
private:
	KSceneSceneTreeCtrlEventHandler* m_pEventHandler;
public:
	KSceneSceneTreeCtrl();
	virtual ~KSceneSceneTreeCtrl();
	enum{ENUM_LIMIT_MAX_LEVEL = 6,};
	//enum
	//{
	//	//CALLBACK_ITEM_EXPAND = 0,
	//	//CALLBACK_ITEM_DELETING = 1,
	//	//CALLBACK_ITEM_INSERT = 2,
	//	//CALLBACK_ITEM_ENUM = 3,
	//	//CALLBACK_ITEM_SELCHANGED = 4,
	//	//CALLBACK_ITEM_EXPANDING = 5,
	//	//CALLBACK_ITEM_END,
	//};
	DECLARE_DYNAMIC(KSceneSceneTreeCtrl)
	//typedef KSUIHELPER::CWndCallbackFrameWork<KSceneSceneTreeCtrl, HTREEITEM, 7> TypeCallbackFrameWork;
	//typedef TypeCallbackFrameWork::TypeFuncCallback TypeFuncCallback;
protected:
	DECLARE_MESSAGE_MAP()
};
#endif //_INCLUDE_KSCENESCENETREECTRL_H_
