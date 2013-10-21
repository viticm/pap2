////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSceneTreeCtrl.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-6-25 15:47:27
//  Comment     : 用模板强化的Tree，能在Item上绑定任意数据类型，未免模板扩散，只能在cpp中使用
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENECUSTOMTREECTRL_H_
#define _INCLUDE_KSCENECUSTOMTREECTRL_H_
#include "KSceneSceneTreeCtrl.h"
////////////////////////////////////////////////////////////////////////////////
//#include "KSceneCallbackFramework.h"

class KSceneCustomScrollBar;

struct KSceneCustomTreeCtrlTester 
{
	KSceneCustomTreeCtrlTester();
};

template<typename _Type>
class KSceneCustomTreeCtrl :  public KSceneSceneTreeCtrl
{
	friend struct KSceneCustomTreeCtrlTester;
	//typedef DWORD_PTR (CALLBACK *TypeFuncCallback)(KSceneSceneTreeCtrl* Obj, HTREEITEM KeyParam, DWORD_PTR dwObjParam, DWORD_PTR dwExtraParam);
private:
	typedef std::map<HTREEITEM, _Type> TypeAlloc;
	TypeAlloc m_DataAlloc;
public:
	KSceneCustomTreeCtrl(){	}
	~KSceneCustomTreeCtrl(){}
	// ASSERT(::IsWindow(m_hWnd)); return SetItem(hItem, TVIF_PARAM, NULL, 0, 0, 0, 0, (LPARAM)dwData);
	VOID SetItemBigData(HTREEITEM hItem, const _Type& Another)
	{
		if(this->IsItemHandleValid(hItem))
			m_DataAlloc[hItem] = Another;
	}
	_Type* GetItemBigData(HTREEITEM hItem)
	{
		TypeAlloc::iterator it = m_DataAlloc.find(hItem);
		if(it == m_DataAlloc.end())
		{
			return NULL;
		}
		_ASSERTE(this->IsItemHandleValid(hItem));
		return &(it->second);
	}
	HTREEITEM GetHandleOfBigData(const _Type& Another)
	{
		for (TypeAlloc::iterator it = m_DataAlloc.begin(); it != m_DataAlloc.end(); ++it)
		{
			if (it->second == Another)
				return it->first;
		}
		return NULL;
	}
	HTREEITEM InsertItemEx2(CString Text
		, HTREEITEM hParent = TVI_ROOT
		, HTREEITEM hInsertAfter = TVI_LAST
		, const _Type* pOther = NULL
		, INT nImage = -1
		, INT nSelectedImage = -1
		, UINT* puState = NULL
		, UINT  uStateMask = 0
		)
	{
		HTREEITEM hItem = KSceneSceneTreeCtrl::InsertItemEx(Text, hParent, hInsertAfter, NULL, nImage, nSelectedImage, puState, uStateMask);
		if(hItem && pOther)
		{
			SetItemBigData(hItem, *pOther);
		}
		return hItem;
	}
	BOOL IsItemContainedBigData(HTREEITEM hItem){return m_DataAlloc.find(hItem) ? TRUE : FALSE;}
	size_t GetBigDataCount(){return m_DataAlloc.size();}
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch(message)
		{
		case TVM_SETITEM:	//封闭掉SETITEM方法
			{
				TVITEM* pItem = reinterpret_cast<TVITEM*>(lParam);
				_ASSERT(pItem);
				if(pItem->mask & TVIF_PARAM)
					return FALSE;
			}
			break;
		case TVM_GETITEM:	//封闭掉GETITEM方法
			{
				TVITEM* pItem = reinterpret_cast<TVITEM*>(lParam);
				_ASSERT(pItem);
				if(pItem->mask & TVIF_PARAM)
				{
					pItem->lParam = NULL;
					return TRUE;
				}				
			}
			break;
		case TVM_DELETEITEM:
			{
				LRESULT lpRet = KSceneSceneTreeCtrl::WindowProc(message, wParam, lParam);
				if(lpRet)
				{
					if(reinterpret_cast<HTREEITEM>(lParam) == TVI_ROOT)//DeleteAllItems
					{
						m_DataAlloc.clear();
					}
					else	//一个个的遍历Tree节点的Child节点再查map还不如反过来遍历map反查Tree
					{
						for (TypeAlloc::iterator it = m_DataAlloc.begin(); it != m_DataAlloc.end();)
						{
							if(! this->IsItemHandleValid(it->first))
								it = m_DataAlloc.erase(it);
							else
								++it;
						}
					}
				}
				return lpRet;
			}
			break;
		default:
			break;
		}
		return KSceneSceneTreeCtrl::WindowProc(message, wParam, lParam);
	}
	//MFC DECLARE_MESSAGE_MAP
protected:
	static const AFX_MSGMAP* PASCAL GetThisMessageMap()
	{ 
		typedef KSceneCustomTreeCtrl<_Type> ThisClass;						 
		typedef KSceneSceneTreeCtrl TheBaseClass;					  
		static const AFX_MSGMAP_ENTRY _messageEntries[] =  
		{
			{0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 }
		}; 
		static const AFX_MSGMAP messageMap = 
		{ &TheBaseClass::GetThisMessageMap, &_messageEntries[0] };
		return &messageMap;
	}						
	virtual const AFX_MSGMAP* GetMessageMap() const{ return KSceneCustomTreeCtrl<_Type>::GetThisMessageMap(); } 
	//MFC DECLARE_DYNAMIC
protected:
	static CRuntimeClass* PASCAL _GetBaseClass(){return RUNTIME_CLASS(KSceneSceneTreeCtrl); } 
public:
	static CRuntimeClass* PASCAL GetThisClass()
	{
		static CRuntimeClass class_name = {
			"KSceneCustomTreeCtrl", sizeof(class KSceneCustomTreeCtrl<_Type>), 0xFFFF, NULL, //注意没有CreateObject的指针
			&_GetBaseClass, NULL, NULL 
		};
		return &class_name;
	}	
};

#endif //_INCLUDE_KSCENECUSTOMTREECTRL_H_
