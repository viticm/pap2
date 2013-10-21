////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_StaticText.h
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-9-6  17:06:19
//  Comment     : The static text with transparent background and could be set text color.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG_STATIC_TEXT_H_
#define _KG_STATIC_TEXT_H_

class KG_StaticText : public CStatic
{
public:
    KG_StaticText();
    ~KG_StaticText();
public:
    void     SetWindowText(LPCTSTR lpszString);
    COLORREF SetTextColor(COLORREF Color);
private:
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
private:
    CString  m_strText;
    COLORREF m_TextColor;
};

#endif // _KG_STATIC_TEXT_H_
