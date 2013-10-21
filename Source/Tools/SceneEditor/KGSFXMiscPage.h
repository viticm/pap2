#pragma once
#include "KSceneSFXEditorTrackPage.h"
#include "KSceneSFXEditorMeteor.h"
#include "KSceneSFXEditorMagicParpet.h"
#include "KSceneSFXEditChannel.h"
#include "KGLilisPanel.h"
#include "KGSFXSoundPanel.h"

class KGSFXMiscPage : public CPropertyPage
{
	DECLARE_DYNAMIC(KGSFXMiscPage)

public:
	KGSFXMiscPage();
	virtual ~KGSFXMiscPage();
	enum { IDD = IDD_DIALOG_MISC };

public :
    KSceneSFXEditorTrackPage        m_pageTrack;
    KSceneSFXEditorMeteor           m_pageMeteor;
    KSceneSFXEditorMagicParpet      m_pageMagicParpet;
    KSceneSFXEditChannel            m_pageChannel;
    KGSFXSoundPanel                 m_pageSound; 
    KGLilisPanel                    m_panel;


    virtual BOOL OnSetActive()
    {
        if (::IsWindow(m_pageTrack.m_hWnd))
            m_pageTrack.OnSetActive();
        if (::IsWindow(m_pageMeteor.m_hWnd))
            m_pageMeteor.OnSetActive();
        if (::IsWindow(m_pageMagicParpet.m_hWnd))
            m_pageMagicParpet.OnSetActive();
        if (::IsWindow(m_pageChannel.m_hWnd))
            m_pageChannel.OnSetActive();
        if (::IsWindow(m_pageSound.m_hWnd))
            m_pageSound.OnSetActive();
        return TRUE;
    }
    int UpdateUI()
    {
        if (::IsWindow(m_pageTrack.m_hWnd))
            m_pageTrack.UpdateUI();
        if (::IsWindow(m_pageMeteor.m_hWnd))
            m_pageMeteor.UpdateUI();
        if (::IsWindow(m_pageMagicParpet.m_hWnd))
            m_pageMagicParpet.UpdateUI();
        if (::IsWindow(m_pageChannel.m_hWnd))
            m_pageChannel.UpdateUI();
        if (::IsWindow(m_pageSound.m_hWnd))
            m_pageSound.UpdateUI();
        return TRUE;
    };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnSize(UINT nType, int cx, int cy);
    BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
