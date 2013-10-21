////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneCustomGridCtrl.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-2-20 10:27:38
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KSceneCustomGridCtrl.h"



#if defined(_DEBUG) | defined(DEBUG)
KSceneCustomGridCtrlTest::KSceneCustomGridCtrlTest()
{


	static unsigned stDataObjectCount = 0;
	struct  TypeDataObject
	{
		INT nA;
		TypeDataObject(){++stDataObjectCount;}
		~TypeDataObject(){--stDataObjectCount;}
		TypeDataObject(const TypeDataObject& Other):nA(Other.nA){++stDataObjectCount;}
	};

	KSceneCustomGridCtrl<TypeDataObject>	TempGridCtrl;

	CWnd* pDesktop = CWnd::FromHandle(::GetDesktopWindow());

	TempGridCtrl.Create(CRect(0,0,100,100), pDesktop, 12345678, WS_BORDER | WS_TABSTOP | WS_VISIBLE);

	KSceneCustomGridCtrl<TypeDataObject>::InitAppearanceStruct InitStruct;
	InitStruct.m_bEditEnable = true;
	InitStruct.m_bInitBasicTitles = true;
	InitStruct.m_bEnableHightLighFixed = false;
	InitStruct.m_csCornerDescription = _T("类型/时间");
	InitStruct.m_nColumnCount = 10;
	InitStruct.m_nDefRowHeight = 10;
	InitStruct.m_nDefColumnWidth = 25;
	InitStruct.m_nFixedColumnCount = 1;
	InitStruct.m_nFixedColumnWidth = 100;
	InitStruct.m_nFixedRowCount = 1;
	InitStruct.m_nRowCount = 10;
	TempGridCtrl.InitAppearance(&InitStruct);

	_ASSERTE(stDataObjectCount == 0);

	{
		TypeDataObject TempObject, TempObject2;
		TempObject.nA = 12345;
		TempObject2.nA = 23456;
		TempGridCtrl.SetItemBigData(1,1, TempObject);
		TempGridCtrl.SetItemBigData(9,9, TempObject2);
	}
	_ASSERTE(stDataObjectCount == 2);
	{
		TypeDataObject* p = TempGridCtrl.GetItemBigData(1,1);
		_ASSERTE(p);
		_ASSERTE(p->nA == 12345);
		p = TempGridCtrl.GetItemBigData(9,9);
		_ASSERTE(p);
		_ASSERTE(p->nA == 23456);
	}
	_ASSERTE(stDataObjectCount == 2);

	{
		TempGridCtrl.SetItemBigData(5,5,TypeDataObject());
	}
	_ASSERTE(stDataObjectCount == 3);

	{
		TempGridCtrl.SetRowCount(1);
	}
	_ASSERTE(stDataObjectCount == 0);//Grid被删除了，应该Data被释放

	TempGridCtrl.DestroyWindow();
}

#endif

////////////////////////////////////////////////////////////////////////////////

