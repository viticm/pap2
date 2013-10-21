////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGValueEditNpcNumDataDlg.cpp
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-5-13 20:18:44
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KGValueEditNpcNumDataDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KGValueEditNpcNumDataDlg, CDialog)

KGValueEditNpcNumDataDlg::KGValueEditNpcNumDataDlg(
	ITabFile* pTabFile, IIniFile* pIniFile,
	LPKGLISTITEM pListItem, CWnd* pParent
): CDialog(KGValueEditNpcNumDataDlg::IDD, pParent)
{
	m_pTabFile   = pTabFile;
	m_pIniFile	 = pIniFile;
	m_pListItem  = pListItem;
	m_pParant	 = pParent;

	//m_vectorPropertyData.resize(INT_DATA_SIZE);
}

KGValueEditNpcNumDataDlg::~KGValueEditNpcNumDataDlg()
{
}

void KGValueEditNpcNumDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, Level_e,					 m_nLevel);
	DDX_Text(pDX, Intensity,				 m_nIntensity);
	DDX_Text(pDX, MaxLife_e,				 m_nMaxLife);
	DDX_Text(pDX, LifeReplenish_e,			 m_nLifeReplenish);
	DDX_Text(pDX, LifeReplenishPercent_e,	 m_nLifeReplenishPercent);
	DDX_Text(pDX, MaxMana_e,				 m_nMaxMana);
	DDX_Text(pDX, ManaReplenish_e,			 m_nManaReplenish);
	DDX_Text(pDX, ManaReplenishPercent_e,	 m_nManaReplenishPercent);
	DDX_Text(pDX, MaxRage_e,				 m_nMaxRage);
	DDX_Text(pDX, Hit_e,					 m_nHit);
	DDX_Text(pDX, Dodge_e,					 m_nDodge);
	DDX_Text(pDX, CriticalStrike_e,			 m_nCriticalStrike);
	DDX_Text(pDX, PhysicsShieldBase_e,		 m_nPhysicsShieldBase);
	DDX_Text(pDX, SolarMagicDefence_e,		 m_nSolarMagicDefence);
	DDX_Text(pDX, NeutralMagicDefence_e,	 m_nNeutralMagicDefence);
	DDX_Text(pDX, LunarMagicDefence_e,		 m_nLunarMagicDefence);
	DDX_Text(pDX, PoisonMagicDefence_e,		 m_nPoisonMagicDefence);

}

BOOL KGValueEditNpcNumDataDlg::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;

	USER_ITEM_DATA itemData;

	nRetCode = CDialog::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	KG_PROCESS_ERROR(m_pListItem);
	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pIniFile);

	((CSpinButtonCtrl*)(GetDlgItem(Level_s)))->SetBuddy(GetDlgItem(Level_e));
	((CSpinButtonCtrl*)(GetDlgItem(MaxLife_s)))->SetBuddy(GetDlgItem(MaxLife_e));
	((CSpinButtonCtrl*)(GetDlgItem(LifeReplenish_s)))->SetBuddy(GetDlgItem(LifeReplenish_e));
	((CSpinButtonCtrl*)(GetDlgItem(LifeReplenishPercent_s)))->SetBuddy(GetDlgItem(LifeReplenishPercent_e));
	((CSpinButtonCtrl*)(GetDlgItem(MaxMana_s)))->SetBuddy(GetDlgItem(MaxMana_e));
	((CSpinButtonCtrl*)(GetDlgItem(ManaReplenish_s)))->SetBuddy(GetDlgItem(ManaReplenish_e));
	((CSpinButtonCtrl*)(GetDlgItem(ManaReplenishPercent_s)))->SetBuddy(GetDlgItem(ManaReplenishPercent_e));
	((CSpinButtonCtrl*)(GetDlgItem(MaxRage_s)))->SetBuddy(GetDlgItem(MaxRage_e));
	((CSpinButtonCtrl*)(GetDlgItem(Hit_s)))->SetBuddy(GetDlgItem(Hit_e));
	((CSpinButtonCtrl*)(GetDlgItem(Dodge_s)))->SetBuddy(GetDlgItem(Dodge_e));
	((CSpinButtonCtrl*)(GetDlgItem(CriticalStrike_s)))->SetBuddy(GetDlgItem(CriticalStrike_e));
	((CSpinButtonCtrl*)(GetDlgItem(PhysicsShieldBase_s)))->SetBuddy(GetDlgItem(PhysicsShieldBase_e));
	((CSpinButtonCtrl*)(GetDlgItem(SolarMagicDefence_s)))->SetBuddy(GetDlgItem(SolarMagicDefence_e));
	((CSpinButtonCtrl*)(GetDlgItem(NeutralMagicDefence_s)))->SetBuddy(GetDlgItem(NeutralMagicDefence_e));
	((CSpinButtonCtrl*)(GetDlgItem(LunarMagicDefence_s)))->SetBuddy(GetDlgItem(LunarMagicDefence_e));
	((CSpinButtonCtrl*)(GetDlgItem(PoisonMagicDefence_s)))->SetBuddy(GetDlgItem(PoisonMagicDefence_e));

	((CSpinButtonCtrl*)(GetDlgItem(Level_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(MaxLife_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(MaxLife_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(LifeReplenish_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(LifeReplenishPercent_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(MaxMana_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(ManaReplenish_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(ManaReplenishPercent_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(MaxRage_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(Hit_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(Dodge_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(CriticalStrike_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(PhysicsShieldBase_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(SolarMagicDefence_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(NeutralMagicDefence_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(LunarMagicDefence_s)))->SetRange32(0, INT_MAX);
	((CSpinButtonCtrl*)(GetDlgItem(PoisonMagicDefence_s)))->SetRange32(0, INT_MAX);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("Level"), 0, &m_nLevel);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("Intensity"), 0, &m_nIntensity);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("MaxLife"), 0, &m_nMaxLife);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("LifeReplenish"), 0, &m_nLifeReplenish);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("LifeReplenishPercent"), 0, &m_nLifeReplenishPercent);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("MaxMana"), 0, &m_nMaxMana);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("ManaReplenish"), 0, &m_nManaReplenish);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("ManaReplenishPercent"), 0, &m_nManaReplenishPercent);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("MaxRage"), 0, &m_nMaxRage);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("Hit"), 0, &m_nHit);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("Dodge"), 0, &m_nDodge);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("CriticalStrike"), 0, &m_nCriticalStrike);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("PhysicsShieldBase"), 0, &m_nPhysicsShieldBase);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("SolarMagicDefence"), 0, &m_nSolarMagicDefence);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("NeutralMagicDefence"), 0, &m_nNeutralMagicDefence);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("LunarMagicDefence"), 0, &m_nLunarMagicDefence);
	m_pTabFile->GetInteger(itemData.nRowsIndexInFile, _T("PoisonMagicDefence"), 0, &m_nPoisonMagicDefence);

	UpdateData(false);

	nResult = true;
Exit0:
	return nResult;
}

BEGIN_MESSAGE_MAP(KGValueEditNpcNumDataDlg, CDialog)
	ON_BN_CLICKED(IDOK, &KGValueEditNpcNumDataDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// KGValueEditNpcNumDataDlg message handlers

void KGValueEditNpcNumDataDlg::OnBnClickedOk()
{
	CString stringText = _T("");

	GetDlgItem(Level_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(Level_e)->SetWindowText(stringText);

	GetDlgItem(MaxLife_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(MaxLife_e)->SetWindowText(stringText);

	GetDlgItem(LifeReplenish_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(LifeReplenish_e)->SetWindowText(stringText);

	GetDlgItem(LifeReplenishPercent_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(LifeReplenishPercent_e)->SetWindowText(stringText);

	GetDlgItem(MaxMana_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(MaxMana_e)->SetWindowText(stringText);

	GetDlgItem(ManaReplenish_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(ManaReplenish_e)->SetWindowText(stringText);

	GetDlgItem(ManaReplenishPercent_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(ManaReplenishPercent_e)->SetWindowText(stringText);

	GetDlgItem(MaxRage_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(MaxRage_e)->SetWindowText(stringText);

	GetDlgItem(Hit_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(Hit_e)->SetWindowText(stringText);

	GetDlgItem(Dodge_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(Dodge_e)->SetWindowText(stringText);

	GetDlgItem(CriticalStrike_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(CriticalStrike_e)->SetWindowText(stringText);

	GetDlgItem(PhysicsShieldBase_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(PhysicsShieldBase_e)->SetWindowText(stringText);

	GetDlgItem(SolarMagicDefence_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(SolarMagicDefence_e)->SetWindowText(stringText);

	GetDlgItem(NeutralMagicDefence_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(NeutralMagicDefence_e)->SetWindowText(stringText);

	GetDlgItem(LunarMagicDefence_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(LunarMagicDefence_e)->SetWindowText(stringText);

	GetDlgItem(PoisonMagicDefence_e)->GetWindowText(stringText);
	stringText.Replace(_T(","), _T(""));
	GetDlgItem(PoisonMagicDefence_e)->SetWindowText(stringText);


	UpdateData();

	USER_ITEM_DATA itemData;

	KG_PROCESS_ERROR(m_pListItem);
	KG_PROCESS_ERROR(m_pTabFile);
	KG_PROCESS_ERROR(m_pIniFile);

	m_pListItem->GetStructData(&itemData, sizeof(itemData));

	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("Level"), m_nLevel);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("Intensity"), m_nIntensity);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("MaxLife"), m_nMaxLife);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("LifeReplenish"), m_nLifeReplenish);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("LifeReplenishPercent"), m_nLifeReplenishPercent);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("MaxMana"), m_nMaxMana);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("ManaReplenish"), m_nManaReplenish);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("ManaReplenishPercent"), m_nManaReplenishPercent);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("MaxRage"), m_nMaxRage);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("Hit"), m_nHit);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("Dodge"), m_nDodge);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("CriticalStrike"), m_nCriticalStrike);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("PhysicsShieldBase"), m_nPhysicsShieldBase);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("SolarMagicDefence"), m_nSolarMagicDefence);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("NeutralMagicDefence"), m_nNeutralMagicDefence);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("LunarMagicDefence"), m_nLunarMagicDefence);
	m_pTabFile->WriteInteger(itemData.nRowsIndexInFile, _T("PoisonMagicDefence"), m_nPoisonMagicDefence);

Exit0:
	OnOK();
}
