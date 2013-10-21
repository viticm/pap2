////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneCustomGridCtrl.h
//  Version     : 1.0
//  Creator     : Chen tianhong
//  Create Date : 2009-2-20 10:01:48
//  Comment     : 这个是模板来的，不要放在头文件那
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENECUSTOMGRIDCTRL_H_
#define _INCLUDE_KSCENECUSTOMGRIDCTRL_H_
#include "KSceneSceneGridCtrl.h"
////////////////////////////////////////////////////////////////////////////////
/*
用SetBigData和GetBigData系列，可以放入任意大小的元素
*/

template<typename _Type>
class KSceneCustomGridCell : public CGridCell
{
public:
	KSceneCustomGridCell(){}
	~KSceneCustomGridCell()
	{
		_Type* p = this->GetBigData();
		if(p)
			delete p;
	};
	KSceneCustomGridCell(const KSceneCustomGridCell& Other)
	{
		_Type* p = Other.GetBigData();
		if(p)
			this->SetBigData(*p);
	}
private:
	virtual VOID SetData(LPARAM){return;}
	virtual LPARAM GetData(){return NULL;}
public:
	VOID SetBigData(const _Type& Data)
	{
		LPARAM lp = CGridCell::GetData();
		lp ? *(reinterpret_cast<_Type*>(lp)) = Data : CGridCell::SetData(reinterpret_cast<LPARAM>(new _Type(Data)));
	}
	_Type* GetBigData()const{return reinterpret_cast<_Type*>(CGridCell::GetData());}

	//<DECALRE_DYNAMIC 宏等价声明>
protected:
	static CRuntimeClass* PASCAL _GetBaseClass(){return RUNTIME_CLASS(CGridCell); } 
public:
	static CObject* PASCAL CreateObject(){ return new KSceneCustomGridCell<_Type>(); }
	static CRuntimeClass* PASCAL GetThisClass()
	{
		static CRuntimeClass class_name = {
			"KSceneCustomGridCell", sizeof(class KSceneCustomGridCell<_Type>), 0xFFFF, &CreateObject, 
			&_GetBaseClass, NULL, NULL 
		};
		return &class_name;
	}	
	//</DECALRE_DYNAMIC 宏等价声明>
};

//////////////////////////////////////////////////////////////////////////
template<typename _Type>
class KSceneCustomGridCtrl : public KSceneSceneGridCtrl
{
public:
	KSceneCustomGridCtrl(){this->SetDefaultCellType(RUNTIME_CLASS(KSceneCustomGridCell<_Type>));}
	~KSceneCustomGridCtrl(){}
public:
	BOOL SetItemBigData(int nRow, int nCol, const _Type& Data)
	{
		if (GetVirtualMode())
			return FALSE;

		KSceneCustomGridCell<_Type>* pCell = dynamic_cast<KSceneCustomGridCell<_Type>* >(this->GetCell(nRow, nCol));
		if (!pCell)
			return FALSE;

		pCell->SetBigData(Data);
		SetModified(TRUE, nRow, nCol);
		return TRUE;
	}
	_Type* GetItemBigData(int nRow, int nCol) const
	{
		KSceneCustomGridCell<_Type>* pCell = dynamic_cast<KSceneCustomGridCell<_Type>* >(this->GetCell(nRow, nCol));
		if (! pCell)
			return NULL;

		return pCell->GetBigData();
	}

	//<DECALRE_DYNAMIC 宏等价声明>
protected:
	static CRuntimeClass* PASCAL _GetBaseClass(){return RUNTIME_CLASS(KSceneSceneGridCtrl); } 
public:
	static CObject* PASCAL CreateObject(){ return new KSceneCustomGridCtrl<_Type>(); }
	static CRuntimeClass* PASCAL GetThisClass()
	{
		static CRuntimeClass class_name = {
			"KSceneCustomGridCtrl", sizeof(class KSceneCustomGridCtrl<_Type>), 0xFFFF, &CreateObject, 
			&_GetBaseClass, NULL, NULL 
		};
		return &class_name;
	}
	//</DECALRE_DYNAMIC 宏等价声明>

};
#if defined(_DEBUG) | defined(DEBUG)
struct KSceneCustomGridCtrlTest
{
	KSceneCustomGridCtrlTest();
};
#endif

namespace KSH = KSCENEEDITOR_HELPERCLASSES;
#endif //_INCLUDE_KSCENECUSTOMGRIDCTRL_H_
