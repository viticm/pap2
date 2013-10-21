////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGMemDateList.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-7-13 19:46:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGMEMDATELIST_H_
#define _INCLUDE_KGMEMDATELIST_H_

#include "KGListCtrl.h"
#include "KGTreeCtrl.h"
#include "resource.h"

enum KGMEMDATETYPE
{
    MEM_TYPE_STRING,    // char*
    MEM_TYPE_PATH,      // char* (file path)
    MEM_TYPE_FLOAT,     // float
    MEM_TYPE_INT,       // int
    MEM_TYPE_BOOL,      // int (bool)
    MEM_TYPE_COLOR,     // DWORD (color)
    MEM_TYPE_DWORD,     // DWORD

    MEM_TYPE_TITLE,     // title use to edit

    MEM_TYPE_UNKNOW     
};

struct KGMEMDATEELEMENT
{
    INT     nOffset;        // date offset
    INT     nSize;          // date size (byte)
    DWORD   dwType;         // date type (emun)
    LPCSTR  strPlate;       // display name   
};

#define ELEMENT_TITLE(str) { 0,  0, MEM_TYPE_TITLE,  str}
#define ELEMENT_END()      {-1, -1, MEM_TYPE_UNKNOW, _T("")}
#define ELEMENT_MAX_NUM    100
#define ELEMENT_AUTO       -10

class KGMemDateList : public KGListCtrl
{
public :
    int BandDate(BYTE* pDate, KGMEMDATEELEMENT element[]);
    int RetrievesItemData(LPKGLISTITEM pItem);
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    int UpdateCtrl(int nFlag);
	int UpdateToBandDate();
 
public :
    KGMemDateList();
    virtual ~KGMemDateList();

public :
    struct _ItemDate
    {
        char    szPlate[128];
        char    szDate[1000];

        INT     nOffset;
        INT     nSize;
        DWORD   dwType;

        _ItemDate()
        {
            *szPlate = '\0';
            *szDate  = '\0';
        }
    };

    LPKGLISTITEM        m_lpCurItem;
    BYTE*               m_pDate;

    CButton			    m_bnEditShow;
    CEdit			    m_editEdit;
    CSpinButtonCtrl     m_spinCtrl;
    CComboBox		    m_comboxCtrl;

    CString			    m_pszState[3];

public :
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point); 
    afx_msg void OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClick();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    DECLARE_MESSAGE_MAP()

};

#endif //_INCLUDE_KGMEMDATELIST_H_
