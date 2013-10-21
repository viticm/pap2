// KSceneSceneTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "KSceneCustomTreeCtrl.h"
#include "KSceneSceneHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// KSceneSceneTreeCtrl

namespace KSCENEEDITOR_HELPERCLASSES
{
	namespace COMMON_CTRL
	{
			
	};//COMMON_CTRL
};

KSceneCustomTreeCtrlTester::KSceneCustomTreeCtrlTester()
{
	//CTreeCtrl
	{
		CTreeCtrl TempTree;
		CWnd* pDeskTop = CWnd::FromHandle(::GetDesktopWindow());
		TempTree.Create(WS_CHILD | WS_VISIBLE, CRect(100,100,100,100), pDeskTop, 1234567);

		HTREEITEM hItem = TempTree.InsertItem("0", TVI_ROOT, TVI_LAST);
		HTREEITEM hItem2 = TempTree.InsertItem("1", TVI_ROOT, TVI_LAST);

		_ASSERTE(KSH::CTreeCtrlIsHandleValid(TempTree, hItem));

		TempTree.DeleteItem(hItem2);
		_ASSERTE(! KSH::CTreeCtrlIsHandleValid(TempTree, hItem2));

		_ASSERTE(! KSH::CTreeCtrlIsHandleValid(TempTree, (HTREEITEM)INT_MAX));
		_ASSERTE(! KSH::CTreeCtrlIsHandleValid(TempTree, NULL));

		_ASSERTE(! KSH::CTreeCtrlIsHandleValid(TempTree, (HTREEITEM)-1));

		TempTree.DestroyWindow();
	}

	//KSceneCustomTreeCtrl
	{
		static INT stLTCount = 0;
		struct TypeElem 
		{
			INT nA;
			TypeElem():nA(0){++stLTCount;}
			TypeElem(const TypeElem& Other):nA(Other.nA){++stLTCount;}
			~TypeElem(){--stLTCount;}
		};
		KSceneCustomTreeCtrl<TypeElem> TempTree;
		CWnd* pDeskTop = CWnd::FromHandle(::GetDesktopWindow());
		TempTree.Create(WS_CHILD | WS_VISIBLE, CRect(100,100,100,100), pDeskTop, 1234567);

		//Insert
		{
			TypeElem TempElems[3];
			TempElems[0].nA = 012,
				TempElems[1].nA = 123,
				TempElems[2].nA = 234;
			TempTree.InsertItemEx2("", TVI_ROOT, TVI_LAST, &TempElems[0]);
			HTREEITEM h = TempTree.InsertItemEx2("", TVI_ROOT, TVI_LAST, &TempElems[1]);
			_ASSERTE(h);
			TempTree.InsertItemEx2("", h, TVI_LAST, &TempElems[2]);

			_ASSERTE(TempTree.GetCount() == 3);
		}
		_ASSERTE(stLTCount == 3);
		_ASSERTE(TempTree.GetBigDataCount() == 3);
		//Is GetItemData And SetItemData Sealed
		{
			HTREEITEM h = TempTree.GetChildItem(TVI_ROOT);
			_ASSERTE(h);
			TempTree.SetItemData(h, 0x12345678);
			LPARAM LP = TempTree.GetItemData(h);
			_ASSERTE(! LP);
		}
		//GetData
		{
			HTREEITEM h = TempTree.GetChildItem(TVI_ROOT);
			_ASSERTE(h);
			TypeElem* pElem0 = TempTree.GetItemBigData(h);
			_ASSERTE(pElem0);
			_ASSERTE(pElem0->nA == 012);

			{
				h = TempTree.GetNextSiblingItem(h);
			}
			TypeElem* pElem1 = TempTree.GetItemBigData(h);
			_ASSERTE(pElem1);
			_ASSERTE(pElem1->nA == 123);
			{
				h = TempTree.GetChildItem(h);
			}
			TypeElem* pElem2 = TempTree.GetItemBigData(h);
			_ASSERTE(pElem2);
			_ASSERTE(pElem2->nA == 234);
		}
		//SetData
		{
			TypeElem TempElem;
			TempElem.nA = 12345;
			HTREEITEM h = TempTree.GetChildItem(TVI_ROOT);
			_ASSERTE(h);
			TempTree.SetItemBigData(h, TempElem);
			TypeElem* p = TempTree.GetItemBigData(h);
			_ASSERTE(p);
			_ASSERTE(p->nA == 12345);

			TempTree.SetItemBigData(HTREEITEM(0x3232), TempElem);
			_ASSERTE(TempTree.GetItemBigData(NULL) == NULL);					
		}
		_ASSERTE(stLTCount == 3);
		_ASSERTE(TempTree.GetBigDataCount() == 3);
		//Del	//IsItemValid
		{
			HTREEITEM h = TempTree.GetChildItem(TVI_ROOT);
			h = TempTree.GetNextSiblingItem(h);
			_ASSERTE(h);
			TempTree.DeleteItem(h);

			_ASSERTE(! TempTree.IsItemHandleValid(h));
		}
		_ASSERTE(stLTCount == 1);
		_ASSERTE(TempTree.GetBigDataCount() == 1);

		//DelAll
		{
			TempTree.DeleteAllItems();
		}
		_ASSERTE(stLTCount == 0);
		_ASSERTE(TempTree.GetBigDataCount() == 0);


		TempTree.DestroyWindow();

	}
	_KG3D_DEBUG_REPORT("KSceneCustomTreeCtrl RunTime Test All Succeeded, Congratulations!");
}