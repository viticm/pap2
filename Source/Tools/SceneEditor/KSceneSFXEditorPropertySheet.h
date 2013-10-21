////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorPropertySheet.h
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:16:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESFXEDITORPROPERTYSHEET_H_
#define _INCLUDE_KSCENESFXEDITORPROPERTYSHEET_H_

////////////////////////////////////////////////////////////////////////////////
#include "afxdlgs.h"
#include "KSceneSFXEditorParticlePage.h"
#include "KSceneSFXEditorBillboardPage.h"
#include "KSceneSFXEditorTrackPage.h"
#include "KSceneSFXEditorModel.h"
#include "KSceneSFXEditorMeteor.h"
#include "KSceneSFXEditorMagicParpet.h"
#include "KGSFXBillbordPage.h"
#include "KGSFXModelPage.h"
#include "KGSFXMiscPage.h"
#include "KGSFXBingPage.h"
#include "KGPropertySheet.h"
#include "KSceneSFXEditChannel.h"
#include "KGSfxEditorEditGeoPane.h"
#include "KGSfxEditModifyPane.h"
#include "KSceneSFXEditorExpPage.h"


class KSceneSFXEditorPropertySheet : public KGPropertySheet
{
	DECLARE_DYNAMIC(KSceneSFXEditorPropertySheet)

	KSceneSFXEditorParticlePageAUT  m_pageParticle;
    KGSFXBillbordPage               m_pageBillboard;
    //KSceneSFXEditorTrackPage        m_pageTrack;
    KGSFXModelPage                  m_pageModel;
    //KSceneSFXEditorMeteor           m_pageMeteor;
    //KSceneSFXEditorMagicParpet      m_pageMagicParpet;
    KGSFXMiscPage                   m_pageMisc;
    KGSFXBindPage                   m_pageBind;
    KGSfxEditorEditGeoPane          m_GeoPane;
    KGSfxEditModifyPane             m_MfyPane;
    KSceneSFXEditorExpPage          m_pageExp;



    KSceneSFXEditorDoc   *m_pSceneSFXEditorDoc;

public:
    BOOL PreTranslateMessage(MSG* pMsg);
    void SetSceneSFXEditorDoc( KSceneSFXEditorDoc *pDoc )
    {
        ASSERT( pDoc );
        m_pSceneSFXEditorDoc = pDoc;
        m_pageMisc.m_pageTrack.SetSFXEditorDoc(pDoc);
        m_pageModel.SetSFXEditorDoc(pDoc);
    }

    void UpdateUI()
    {
        if (GetActiveIndex() == 0)
            m_pageParticle.UpdateUI();
        else if (GetActiveIndex() == 1)
            m_pageBillboard.UpdateUI();
        else if (GetActiveIndex() == 2)
            m_pageModel.UpdateUI();
        else if (GetActiveIndex() == 3)
            m_pageMisc.UpdateUI();
        /*
        else if (GetActiveIndex() == 5)
            m_GeoPane.UpdateUI();
            */
       /* else if (GetActiveIndex() == 4)
            null;*/
        /*else if (GetActiveIndex() == 2)
            m_pageTrack.UpdateUI();
        else if (GetActiveIndex() == 3)
            m_pageModel.UpdateUI();
        else if (GetActiveIndex() == 4)
            m_pageMeteor.UpdateUI();*/
    }

    KSceneSFXEditorParticlePage* GetParticlePage()
    {
        return &m_pageParticle.m_particlePage;
    }

    KSceneSFXEditorBillboardPage* GetBillboardPage()
    {
        return &m_pageBillboard.m_page;
    }

    KSceneSFXEditorTrackPage* GetTrackPage()
    {
        return &m_pageMisc.m_pageTrack;
    }

    KSceneSFXEditorModel* GetModelPage()
    {
        return &m_pageModel.m_page;
    }

    KSceneSFXEditorMeteor* GetMeteorPage()
    {
        return &m_pageMisc.m_pageMeteor;
    }

    KGSFXSoundPanel* GetSoundPage()
    {
        return &m_pageMisc.m_pageSound;
    }

    KGSFXBindPage*  GetBindPage()
    {
        return &m_pageBind;
    }

    KSceneSFXEditChannel* GetChannelPage()
    {
        return &m_pageMisc.m_pageChannel;
    }


public:
	KSceneSFXEditorPropertySheet(CWnd* pParentWnd);
	virtual ~KSceneSFXEditorPropertySheet(void);

    DECLARE_MESSAGE_MAP()
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};


#endif //_INCLUDE_KSCENESFXEDITORPROPERTYSHEET_H_
