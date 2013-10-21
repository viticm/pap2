// DriveCombo.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DriveCombo.h"
#include "BrowseForFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDriveCombo

CDriveCombo::CDriveCombo()
{
}

CDriveCombo::~CDriveCombo()
{
}


BEGIN_MESSAGE_MAP(CDriveCombo, CComboBox)
	//{{AFX_MSG_MAP(CDriveCombo)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendok)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDriveCombo message handlers

void CDriveCombo::LoadDrives()
{
	TCHAR DriveIndex;
	TCHAR DriveLetter[]=_T("x:\\");
	DWORD AvailableDrives=GetLogicalDrives();
	SHFILEINFO sfi;
	UINT uFlags = SHGFI_SYSICONINDEX | SHGFI_DISPLAYNAME | SHGFI_ICON;

	for(DriveIndex='A';DriveIndex<='Z';DriveIndex++,AvailableDrives>>=1)
	{
		if(!(AvailableDrives&1))
			continue;
		DriveLetter[0]=DriveIndex;
        if (SHGetFileInfo(DriveLetter,0,&sfi,sizeof(SHFILEINFO),uFlags))
			SetItemData(AddString(sfi.szDisplayName),DriveIndex);
		else
			SetItemData(AddString(DriveLetter),DriveIndex);
	}
	SetItemData(AddString("Browse"),1000);
}

void CDriveCombo::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	
}

void CDriveCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{

}

void CDriveCombo::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	TCHAR DriveLetter[]=_T("x:\\");
	DriveLetter[0]=(char)lpDrawItemStruct->itemData;
	TCHAR *cBrowse={"Browse"};
	int nRet;
	if(DriveLetter!=NULL)
	{
		SHFILEINFO sfi;
		UINT uFlags = SHGFI_DISPLAYNAME | SHGFI_SMALLICON | SHGFI_ICON;

		if(lpDrawItemStruct->itemData==1000||lpDrawItemStruct->itemData==1001)
		{
			CString strItem;
			GetLBText(lpDrawItemStruct->itemID,strItem);
			strcpy(sfi.szDisplayName,strItem);
			if(lpDrawItemStruct->itemData==1000)
				sfi.hIcon=LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_BLANK));
			else
				sfi.hIcon=LoadIcon(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDI_FOLDER));
			nRet=1;
		}
		else
			nRet=SHGetFileInfo(DriveLetter,0,&sfi,sizeof(SHFILEINFO),uFlags);

        if (nRet)
		{
			CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

			lpDrawItemStruct->rcItem.left+=2;
			lpDrawItemStruct->rcItem.top+=2;
			switch (lpDrawItemStruct->itemAction)
			{
			case ODA_DRAWENTIRE: 
				DrawIconEx(lpDrawItemStruct->hDC,lpDrawItemStruct->rcItem.left,lpDrawItemStruct->rcItem.top,sfi.hIcon,16,16,0,0,DI_NORMAL);
			case ODA_SELECT:
				lpDrawItemStruct->rcItem.left+=18;
				
				if(lpDrawItemStruct->itemState & ODS_SELECTED)
				{
					
					CSize sizeItem=pDC->GetTextExtent(sfi.szDisplayName,strlen(sfi.szDisplayName));
					CRect rcItem;
					rcItem=lpDrawItemStruct->rcItem;
					rcItem.right=rcItem.left+sizeItem.cx;

					pDC->FillSolidRect(&rcItem,::GetSysColor(COLOR_HIGHLIGHT));

					COLORREF OldBkColor=pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
					COLORREF OldTextColor=pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
					pDC->DrawText(sfi.szDisplayName,-1,&lpDrawItemStruct->rcItem,DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
					pDC->DrawFocusRect(&rcItem);
					pDC->SetBkColor(OldBkColor);
					pDC->SetTextColor(OldTextColor);
					
					
				}
				else
				{
					pDC->FillSolidRect(&lpDrawItemStruct->rcItem,RGB(255,255,255));
					pDC->DrawText(sfi.szDisplayName,-1,&lpDrawItemStruct->rcItem,DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
				}
				break;
			}
			
		}
		
	}
	
	CComboBox::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CDriveCombo::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight=18; 
	
	CComboBox::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}



void CDriveCombo::OnSelendok() 
{
	CString strItem;
	int nItem=GetCurSel();
	GetLBText(nItem,strItem);
	if(strItem=="Browse")
	{
		CBrowseForFolder clsBrowse; 
		
		CString strDirectory=clsBrowse.BrowseForFolder(GetSafeHwnd(),"Select a folder",BIF_RETURNONLYFSDIRS);
		if(!strDirectory.IsEmpty())
		{
			if(FindStringExact(0,strDirectory)==LB_ERR)
			{
				SetItemData(SetCurSel(InsertString(nItem+1,strDirectory)),1001);
				nItem=FindStringExact(0,"Browse");
				while(GetCount()-nItem>6)
					DeleteString(GetCount()-1);
			}
			else
				SetCurSel(FindString(0,strDirectory));
		}
		else
			SetCurSel(1);
	}
}

CString CDriveCombo::GetSelectedDirectory()
{
	int nItemData=GetItemData(GetCurSel());
	TCHAR cDriveLetter[]={"x:\\"};
	CString strRet;
	cDriveLetter[0]=nItemData;
	if(nItemData==1001)
		GetLBText(GetCurSel(),strRet);
	else
		if(nItemData>0)
			strRet=cDriveLetter;
		else
			GetWindowText(strRet);
	return strRet;
}
