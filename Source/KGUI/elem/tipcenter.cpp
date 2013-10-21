////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : tipcenter.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-11 9:52:29
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "tipcenter.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"

////////////////////////////////////////////////////////////////////////////////
namespace UI
{

KTipCenter* KTipCenter::ms_pSelf = NULL;

KTipCenter::KTipCenter()
{
	m_nCurrentShowTipType = -1;
	Reserve(TIP_TABLE_ITEM_COUNT);
}

KTipCenter::~KTipCenter()
{

}

bool KTipCenter::Init()
{
	ASSERT(!ms_pSelf);
	ms_pSelf = new(std::nothrow) KTipCenter;
	return (ms_pSelf != NULL);
}

void KTipCenter::Exit()
{
	delete ms_pSelf;
}

KTipCenter &KTipCenter::GetSelf()
{
	ASSERT(ms_pSelf);
	return *ms_pSelf;
}

void KTipCenter::Reserve(int nCount)
{
	m_nReserveCount = nCount;
	m_aTip.Reserve(nCount);	
}

int KTipCenter::Append(int nTipIndex, int nShowTipType, int nRichText)
{
	int nResult = INVALID_TIP_INDEX;
	KUITIPDATA Tip;
	
	KG_PROCESS_ERROR(nTipIndex >= 0);
	Tip.nMessageIndex = nTipIndex;
	Tip.nShowTipType = nShowTipType;
	Tip.nRichText = nRichText;

	nResult = m_aTip.Append(Tip);
Exit0:
	return nResult;
}

void KTipCenter::Remove(int nIndex)
{
	m_aTip.Remove(nIndex);
}

LPCSTR KTipCenter::GetTip(int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nReserveCount);
	const KUITIPDATA *pTip = m_aTip.GetAt(nIndex);
	if (!pTip)
		return NULL;
	return g_pUI->m_GlobalStrValuableMgr.GetAt(pTip->nMessageIndex);
}

int KTipCenter::GetShowTipType(int nIndex) const
{
	ASSERT(nIndex >= 0 && nIndex < m_nReserveCount);
	const KUITIPDATA *pTip = m_aTip.GetAt(nIndex);
	if (!pTip)
		return SHOW_TIP_IN_A_CHANGELESS_POS;
	return pTip->nShowTipType;
}

int KTipCenter::AddRef(int nIndex)
{
	return m_aTip.AddRefCount(nIndex);
}

int KTipCenter::GetUsedCount() const
{
	return m_aTip.GetUsedCount();
}

int KTipCenter::GetTotalCount() const
{
	return m_aTip.GetTotalCount();
}

int KTipCenter::ShowTip(int nIndex, float fX, float fY, float fW, float fH)
{
	int nResult = false;
	int nRetCode = false;
	const KUITIPDATA *pTip = NULL;
	LPCSTR pcszTip = NULL;

	KG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_nReserveCount);

	pTip = m_aTip.GetAt(nIndex);
	KG_PROCESS_ERROR(pTip);

	pcszTip = g_pUI->m_GlobalStrValuableMgr.GetAt(pTip->nMessageIndex);
	KG_PROCESS_ERROR(pcszTip);

	float fScale = g_pUI->m_WndStation.GetUIScale();

	m_Info.pcszTip = pcszTip;
	m_Info.nShowTipType = pTip->nShowTipType;
	m_Info.nRichText = pTip->nRichText;
	m_Info.fX = fX / fScale;
	m_Info.fY = fY / fScale;
	m_Info.fW = fW / fScale;
	m_Info.fH = fH / fScale;
	g_pUI->m_Script.FireGlobalEvent("OutputAutoTipInfo");

	SetCurrentShowTipType(pTip->nShowTipType);

	nResult = true;
Exit0:
	return nResult;
}

int KTipCenter::HideTip()
{
	g_pUI->m_Script.FireGlobalEvent("HideTip"); 
	SetCurrentShowTipType(-1);
	return true;
}

int KTipCenter::SetCurrentShowTipType(int nType)
{
	m_nCurrentShowTipType = nType;
	return true;
}

int KTipCenter::OnMouseMove()
{
	if (m_nCurrentShowTipType != SHOW_TIP_BY_MOUSE_SIDE_AND_FOLLOW_MOUSE_MOVE)
		return true;
	g_pUI->m_Script.FireGlobalEvent("TipOnMouseMove"); 
	return true;
}

KTipCenter::KTIPINFO & KTipCenter::GetTipInfo()
{
	return m_Info;
}

bool operator==(KUITIPDATA const& lhs, KUITIPDATA const& rhs)
{
	return (
		lhs.nShowTipType == rhs.nShowTipType && 
		lhs.nMessageIndex == rhs.nMessageIndex && 
		lhs.nRichText == rhs.nRichText
	);
}

} //namespace UI
