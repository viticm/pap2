// SFXBase.cpp: implementation of the KPropertyBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sceneeditor.h"
#include "SFXBase.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace PropertyBase
{

KPropertyBase::KPropertyBase()
{
	m_bNeedReAcquire = TRUE;
	m_bNeedUpdateUI = TRUE;
}

KPropertyBase::~KPropertyBase()
{
	Clear();
}

DWORD KPropertyBase::GetNumVarGroup()
{
	return (DWORD)m_PropertyGroupVector.size();
}

HRESULT KPropertyBase::Acquire()
{
	return S_OK;
}

int KPropertyBase::AddPage(LPSTR Name)
{
	_PropertyPage NewPage(Name);
	m_vecPage.push_back(NewPage);
	return ((int)m_vecPage.size()-1);
}

_PropertyPage* KPropertyBase::FindPage(int Index)
{
	_PropertyPage* pPage = NULL;
	if((Index>=0)&&(Index<(int)m_vecPage.size()))
	{
		pPage = &m_vecPage[Index];
	}
	return pPage;
}

_PropertyPage* KPropertyBase::FindPage(LPSTR Name)
{
	_PropertyPage* pPage = NULL;
	for(DWORD i=0;i<m_vecPage.size();i++)
	{
		pPage = &m_vecPage[i];
		if(_strcmpi(pPage->scName.c_str(),Name)==0)
		{
			return pPage;
		}
	}
	return NULL;
}

int KPropertyBase::AddVerGroup(LPSTR Name,DWORD Type)
{
	LPVARGROUP pGroup = new VARGROUP;
	pGroup->scGroupName = Name;
	pGroup->dwType = Type;

	m_PropertyGroupVector.push_back(pGroup);
	m_bNeedUpdateUI = TRUE;
	return (int)(m_PropertyGroupVector.size() -1);
}

int KPropertyBase::AddVarItem(int GroupIndex, LPSTR Name)
{
	if ((GroupIndex<0)||(GroupIndex>=(int)m_PropertyGroupVector.size()))
	{
		return E_FAIL;
	}
	LPVARGROUP pGroup = m_PropertyGroupVector[GroupIndex];
	
	LPVARITEM  pItem = new VARITEM;
	wsprintf(pItem->Name,Name);
	pGroup->VarItemVector.push_back(pItem);
	m_bNeedUpdateUI = TRUE;
	return (int)(pGroup->VarItemVector.size() - 1);
}

HRESULT KPropertyBase::AddVarProperty(int GroupIndex, LPSTR Name,_VarProperty::_VarType Type,PVOID Address)
{
	if ((GroupIndex<0)||(GroupIndex>=(int)m_PropertyGroupVector.size()))
	{
		return E_FAIL;
	}
	LPVARGROUP pGroup = m_PropertyGroupVector[GroupIndex];
	
	LPVARPROPERTY pPer = new VARPROPERTY;

	pPer->scName = Name;
	pPer->Type = Type;
	pPer->Address = Address;

	pGroup->PropertyVector.push_back(pPer);
	m_bNeedUpdateUI = TRUE;
	return S_OK;
}

HRESULT KPropertyBase::AddItemVarProperty(int GroupIndex,int ItemIndex,LPSTR Name,_VarProperty::_VarType Type,PVOID Address)
{
	if ((GroupIndex<0)||(GroupIndex>=(int)m_PropertyGroupVector.size()))
	{
		return E_FAIL;
	}
	LPVARGROUP pGroup = m_PropertyGroupVector[GroupIndex];
	
	if ((ItemIndex<0)||(ItemIndex>=(int)pGroup->VarItemVector.size()))
	{
		return E_FAIL;
	}
	LPVARITEM  pItem = pGroup->VarItemVector[ItemIndex];
	
	LPVARPROPERTY pPer = new VARPROPERTY;

	pPer->scName = Name;
	pPer->Type = Type;
	pPer->Address = Address;

	pItem->PropertyVector.push_back(pPer);
	m_bNeedUpdateUI = TRUE;
	return S_OK;
}

HRESULT KPropertyBase::OnRefresh()
{
	return S_OK;
}

HRESULT KPropertyBase::Clear(void)
{
	for(int i_Group=0;i_Group<(int)m_PropertyGroupVector.size();i_Group++)
	{
		LPVARGROUP pGroup = m_PropertyGroupVector[i_Group];
		for(int i_GP=0;i_GP<(int)pGroup->PropertyVector.size();i_GP++)
		{
			LPVARPROPERTY pPer = pGroup->PropertyVector[i_GP];
			SAFE_DELETE(pPer);
		}
		pGroup->PropertyVector.clear();
		
		for(int i_Item=0;i_Item<(int)pGroup->VarItemVector.size();i_Item++)
		{
			LPVARITEM  pItem = pGroup->VarItemVector[i_Item];
			SAFE_DELETE(pItem);
		}
		pGroup->VarItemVector.clear();

		SAFE_DELETE(pGroup);
	}
	m_PropertyGroupVector.clear();
	m_bNeedUpdateUI = TRUE;

	for(DWORD i=0;i<m_vecPage.size();i++)
	{
		_PropertyPage* pPage = &m_vecPage[i];
		pPage->Clear();
	}
	m_vecPage.clear();

	return E_NOTIMPL;
}
};
