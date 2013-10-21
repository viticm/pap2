// MeshPropertySet.h: interface for the KModelToolSFX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESHPROPERTYSET_H__C6B80F32_B961_42B2_A060_ED6DBAC148D2__INCLUDED_)
#define AFX_MESHPROPERTYSET_H__C6B80F32_B961_42B2_A060_ED6DBAC148D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "scene.h"
//#include "XMesh.h"
#include "MeshTreeCtrl.h"
namespace ModelTools
{
//class KModelToolSFX : public ModelUI::KModelUIBase 
//{
//public:
//	enum _ItemType
//	{
//		GROUP,
//		GROUP_PROPERTY,
//		GROUP_METHOD,
//		ITEM,
//		ITEM_PROPERTY,
//		ITEM_METHOD,
//	};
//	typedef struct  _ItemInfo
//	{
//		PVOID     Pointer;
//		_ItemType TYPE;
//		_ItemInfo()
//		{
//			Pointer = NULL;
//			TYPE = GROUP;
//		}
//		_ItemInfo(PVOID P,_ItemType T)
//		{
//			Pointer = P;
//			TYPE = T;
//		}
//	}ITEMINFO;
//
//	D3DXVECTOR3*       m_pLastPosition;//记录当前的针对VAR_POSITION的指针
//	ModelUI::KModelUITreeCtrl*  m_Tree;
//
//	PropertyBase::KPropertyBase*  m_lpSFXBase;
//	PVOID      m_lpScene;
//	
//	vector<ModelUI::KModelUIItem*>m_ItemVector; 
//	vector<ITEMINFO>  m_ItemInfo; 
//
//	ModelUI::KModelUIEditCtrl*    m_lpValueEdit;
//	ModelUI::KModelUISlideBar*    m_lpSlider;
//	BOOL                 m_bShowEdit;
//
//	int   m_nNumShowItem;
//	int   m_nMaxMumItem;
//	int   m_nItemStart;
//
//	//GraphicsStruct::_Command m_CmdBack;
//	D3DXVECTOR2  m_PositionGridA;
//	D3DXVECTOR2  m_PositionGridB;
//
//	float m_Group_Left;
//	float m_Group_Weigth;
//	float m_Group_Top;
//
//	float m_Item_Height;
//	float m_Item_Sep;
//
//	BOOL  m_bHide ;
//	
//	ModelUI::KModelUIListBox*   m_lpSFXListBox;
//
//	ModelUI::KModelUIButton*    m_pBSFXNew;
//
//	list<D3DXVECTOR3*>m_AddedPosition;
//
//	//CDialog* m_lpDailog;
//public:
//	HRESULT CleanUp();
//	HRESULT ComputeRect();
//
//	HRESULT BuildItemInformation();
//	HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
//
//	HRESULT BuildUI();
//	HRESULT Clear();
//	HRESULT SetMeshEditScene(PVOID pScene);
//	HRESULT Render();
//	HRESULT LoadMesh(LPSTR pFileName);
//
//	KModelToolSFX();
//	virtual ~KModelToolSFX();
//
//};
};
#endif // !defined(AFX_MESHPROPERTYSET_H__C6B80F32_B961_42B2_A060_ED6DBAC148D2__INCLUDED_)
