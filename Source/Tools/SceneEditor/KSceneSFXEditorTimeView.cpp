////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSFXEditorTimeView.cpp
//  Version     : 1.0
//  Creator     : Zeb
//  Create Date : 2005-10-18 17:17:10
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KSceneSFXEditorTimeView.h"


////////////////////////////////////////////////////////////////////////////////
#include "SceneEditor.h"
#include "KSceneSFXEditorDoc.h"
#include "IEKG3DSceneSFXEditor.h"
//#include "KG3DSceneEditorBase.h"
//#include "MainFrm.h" //(by dengzhihui 2006年7月6日 14:48:21
#include "KSceneSFXEditorTimeView.h"
#include "KSceneSFXEditorFrame.h"
#include "IEKG3DSFX.h"
#include ".\kscenesfxeditortimeview.h"
#include "IEKG3DMaterial.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

// KSceneSFXEditorTimeView
#define SFX_PLAY_TIMER 77987655

IMPLEMENT_DYNCREATE(KSceneSFXEditorTimeView, CFormView)

KSceneSFXEditorTimeView::KSceneSFXEditorTimeView()
	: CFormView(KSceneSFXEditorTimeView::IDD)
{
    m_pParticlePage = NULL;
    m_pBillboardPage = NULL;

    m_bAuto0 = FALSE;

    m_bAbs = FALSE;
    m_bAutoKey = FALSE;
}

KSceneSFXEditorTimeView::~KSceneSFXEditorTimeView()
{
}

void KSceneSFXEditorTimeView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_KEYFRAME_NUM, m_staticKeyFrameNum);
    DDX_Control(pDX, IDC_CURRENT_KEY_FRAME, m_staticCurrentKeyFrame);
    DDX_Control(pDX, IDC_SLIDER_TIME_LINE, m_sliderTimeLine);

    DDX_Control(pDX, IDC_CHECK_PLAY_SFX, m_checkPlaySFX);
    DDX_Control(pDX, IDC_BUTTON_MOVE_START, m_buttonMoveStart);
    DDX_Control(pDX, IDC_BUTTON_MOVE_END, m_buttonMoveEnd);
    DDX_Control(pDX, IDC_BUTTON_PRE_FRAME, m_buttonPreFrame);
    DDX_Control(pDX, IDC_BUTTON_NEXT_FRAME, m_buttonNextFrame);

    DDX_Control(pDX, IDC_STATIC_TRANS_STATUS, m_staticTransStatus);
    DDX_Control(pDX, IDC_EDIT_TRANS_X, m_editTransX);
    DDX_Control(pDX, IDC_EDIT_TRANS_Y, m_editTransY);
    DDX_Control(pDX, IDC_EDIT_TRANS_Z, m_editTransZ);
    DDX_Control(pDX, IDC_BUTTON_SET_KEYFRAME, m_buttonSetKeyFrame);

    DDX_Check(pDX, IDC_CHECK_AUTO_0, m_bAuto0);
    DDX_Check(pDX, IDC_CHECK_AUTOKEY, m_bAutoKey);
    DDX_Check(pDX, IDC_CHECK_ABS_REL, m_bAbs);
}

BEGIN_MESSAGE_MAP(KSceneSFXEditorTimeView, CFormView)
    ON_WM_HSCROLL()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON_DEL_KEY_FRAME, OnBnClickedButtonDelKeyFrame)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_KEY_FRAME_LEFT, OnBnClickedButtonMoveKeyFrameLeft)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_KEY_FRAME_RIGHT, OnBnClickedButtonMoveKeyFrameRight)
    ON_BN_CLICKED(IDC_CHECK_PLAY_SFX, OnBnClickedCheckPlaySfx)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_START, OnBnClickedButtonMoveStart)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_END, OnBnClickedButtonMoveEnd)
    ON_BN_CLICKED(IDC_CHECK_AUTO_0, OnBnClickedCheckAuto0)
    ON_BN_CLICKED(IDC_CHECK_AUTOKEY, OnBnClickedCheckAutokey)
    ON_BN_CLICKED(IDC_BUTTON_SET_KEYFRAME, OnBnClickedButtonSetKeyframe)
END_MESSAGE_MAP()


// KSceneSFXEditorTimeView diagnostics

#ifdef _DEBUG
void KSceneSFXEditorTimeView::AssertValid() const
{
	CFormView::AssertValid();
}

void KSceneSFXEditorTimeView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void KSceneSFXEditorTimeView::UpdateUIForParticlePage()
{
    CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pParticlePage = pFrame->GetPanelView()->GetParticlePage();

    if (m_pParticlePage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();

    IEKG3DSceneSFXEditor *pSceneSFXEditor = NULL;
    pDoc->GetSceneSFXEditor(&pSceneSFXEditor);

    IEKG3DSFX *pSFX = NULL;
    pSceneSFXEditor->GetSFX(&pSFX);

    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pSceneSFXEditor->GetSceneEditBase(&piSceneEditorBase);

    CString str;
    if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_PARTICLE_KIND))
    {
        int nSelectTimeLine = m_pParticlePage->m_comboParticleKind.GetCurSel();
        if (nSelectTimeLine != CB_ERR)
        {
            IEKG3DSFXParticleLifeTimeLine *pLifeTime = NULL;
            pSFX->GetParticleLifeTimeLine(
                nSelectTimeLine,
                &pLifeTime
                );

            ASSERT(pLifeTime);
            if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_COLOR))
            {
				TKG3DTimeLine<D3DXCOLOR>* pColorLine = static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLine());

                int nKeyFrameNum = (int)pColorLine->GetSize();
                str.Format( "颜色：%d", nKeyFrameNum );
                m_staticKeyFrameNum.SetWindowText( str );

                for( int i = 0; i < nKeyFrameNum; i++ )
                {
                    D3DXCOLOR Color;
                    int nKeyFrame;
                    pColorLine->GetKeyFrameByIndex( &nKeyFrame, &Color, i );
                    m_sliderTimeLine.SetTic( nKeyFrame );
                }
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_WIDTH))
            {
				TKG3DTimeLine<float>* pWidthLine = static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetWidthTimeLine());

                CString str;
                int nKeyFrameNum = (int)pWidthLine->GetSize();
                str.Format( "宽度：%d", nKeyFrameNum );
                m_staticKeyFrameNum.SetWindowText( str );

                for( int i = 0; i < nKeyFrameNum; i++ )
                {
                    float Width;
                    int nKeyFrame;
                    pWidthLine->GetKeyFrameByIndex( &nKeyFrame, &Width, i );
                    m_sliderTimeLine.SetTic( nKeyFrame );
                }
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_HEIGHT))
            {
				TKG3DTimeLine<float>* pHeightLine = static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetHeightTimeLine());

                int nKeyFrameNum = (int)pHeightLine->GetSize();
                str.Format( "高度：%d", nKeyFrameNum );
                m_staticKeyFrameNum.SetWindowText( str );

                for( int i = 0; i < nKeyFrameNum; i++ )
                {
                    float Height;
                    int nKeyFrame;
                    pHeightLine->GetKeyFrameByIndex( &nKeyFrame, &Height, i );
                    m_sliderTimeLine.SetTic( nKeyFrame );
                }
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_TEXTURE_INDEX))
            {
				TKG3DTimeLine<int>* pTextLine = static_cast<TKG3DTimeLine<int>*>(pLifeTime->GetTextureFrameTimeLine());

                int nKeyFrameNum = (int)pTextLine->GetSize();
                str.Format( "纹理坐标索引：%d", nKeyFrameNum );
                m_staticKeyFrameNum.SetWindowText( str );

                for( int i = 0; i < nKeyFrameNum; i++ )
                {
                    int nTextureIndex;
                    int nKeyFrame;
                    pTextLine->GetKeyFrameByIndex(
                        &nKeyFrame, &nTextureIndex, i
                        );
                    m_sliderTimeLine.SetTic( nKeyFrame );
                }
            }
        }
    }
    else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_LAUNCHER))
    {
        int nSelectLauncher = m_pParticlePage->m_comboLauncher.GetCurSel();
        if (nSelectLauncher != CB_ERR)
        {
            IEKG3DSFXParticleLauncher *pLauncher = NULL;
            pSFX->GetParticleLauncher(nSelectLauncher, &pLauncher);
            ASSERT(pLauncher);

            if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_PARTICLE_NUM))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleNumLine());

                int nKeyFrameNum = (int)pLine->GetSize();
                str.Format( "粒子数：%d", nKeyFrameNum );
                m_staticKeyFrameNum.SetWindowText( str );

                for( int i = 0; i < nKeyFrameNum; i++ )
                {
                    float fParticleNum;
                    int nKeyFrame;
                    pLine->GetKeyFrameByIndex(&nKeyFrame, &fParticleNum, i);
                    m_sliderTimeLine.SetTic( nKeyFrame );
                }
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MIN_LIFE))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMinLine());

                int nKeyFrameNum = (int)pLine->GetSize();
                str.Format( "生命最小：%d", nKeyFrameNum );
                m_staticKeyFrameNum.SetWindowText( str );

                for( int i = 0; i < nKeyFrameNum; i++ )
                {
                    float fLifeMin;
                    int nKeyFrame;
                    pLine->GetKeyFrameByIndex( &nKeyFrame, &fLifeMin, i );
                    m_sliderTimeLine.SetTic( nKeyFrame );
                }
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MAX_LIFE))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMaxLine());

                int nKeyFrameNum = (int)pLine->GetSize();
                str.Format( "生命最大：%d", nKeyFrameNum );
                m_staticKeyFrameNum.SetWindowText( str );

                for( int i = 0; i < nKeyFrameNum; i++ )
                {
                    float fLifeMax;
                    int nKeyFrame;
                    pLine->GetKeyFrameByIndex( &nKeyFrame, &fLifeMax, i );
                    m_sliderTimeLine.SetTic( nKeyFrame );
                }
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MIN_SPEED))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMinLine());

                int nKeyFrameNum = (int)pLine->GetSize();
                str.Format( "速率最小：%d", nKeyFrameNum );
                m_staticKeyFrameNum.SetWindowText( str );

                for( int i = 0; i < nKeyFrameNum; i++ )
                {
                    float fSpeedMin;
                    int nKeyFrame;
                    pLine->GetKeyFrameByIndex( &nKeyFrame, &fSpeedMin, i );
                    m_sliderTimeLine.SetTic( nKeyFrame );
                }
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MAX_SPEED))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMaxLine());

                int nKeyFrameNum = (int)pLine->GetSize();
                str.Format( "速率最大：%d", nKeyFrameNum );
                m_staticKeyFrameNum.SetWindowText( str );

                for( int i = 0; i < nKeyFrameNum; i++ )
                {
                    float fSpeedMax;
                    int nKeyFrame;
                    pLine->GetKeyFrameByIndex( &nKeyFrame, &fSpeedMax, i );
                    m_sliderTimeLine.SetTic( nKeyFrame );
                }
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_SCATTER_MIN))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMin());

                int nKeyFrameNum = (int)pLine->GetSize();
                str.Format("最小发散：%d", nKeyFrameNum);
                m_staticKeyFrameNum.SetWindowText( str );

                for (int i = 0; i < nKeyFrameNum; i++)
                {
                    float fScatter;
                    int nKeyFrame;
                    pLine->GetKeyFrameByIndex(&nKeyFrame, &fScatter, i );
                    m_sliderTimeLine.SetTic(nKeyFrame);
                }
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_SCATTER_MAX))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMax());

                int nKeyFrameNum = (int)pLine->GetSize();
                str.Format("最大发散：%d", nKeyFrameNum);
                m_staticKeyFrameNum.SetWindowText(str);

                for (int i = 0; i < nKeyFrameNum; i++)
                {
                    float fScatter;
                    int nKeyFrame;
                    pLine->GetKeyFrameByIndex(&nKeyFrame, &fScatter, i);
                    m_sliderTimeLine.SetTic(nKeyFrame);
                }
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_TRANSFORMATION))
            {
				TKG3DTimeLine<D3DXVECTOR3>* pLine =
					static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_TranslationTimeLine);

                int nKeyFrameNum = 0;
                CString str;
                if (GetEditState(piSceneEditorBase) == SCENESTATE_MOVE)
                {
                    nKeyFrameNum = (int)pLine->GetSize();
                    D3DXVECTOR3 vTrans;
                    for (int i = 0; i < nKeyFrameNum; ++i)
                    {
                        int nKeyFrame;
                        pLine->GetKeyFrameByIndex(&nKeyFrame, &vTrans, i);
                        m_sliderTimeLine.SetTic(nKeyFrame);
                    }
                }
                else if (GetEditState(piSceneEditorBase) == SCENESTATE_ROTATE)
                {
                    //ASSERT(false);
                    /*
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_RotationTimeLine);

                    nKeyFrameNum = (int)pLine->GetSize();
                    D3DXVECTOR3 vTrans;
                    for (int i = 0; i < nKeyFrameNum; ++i)
                    {
                        int nKeyFrame;
                        pLine->GetKeyFrameByIndex(&nKeyFrame, &vTrans, i);
                        m_sliderTimeLine.SetTic(nKeyFrame);
                    }*/
                }
                else if (GetEditState(piSceneEditorBase) == SCENESTATE_SCALE)
                {
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_ScalingTimeLine);

                    nKeyFrameNum = (int)pLine->GetSize();
                    D3DXVECTOR3 vTrans;
                    for (int i = 0; i < nKeyFrameNum; ++i)
                    {
                        int nKeyFrame;
                        pLine->GetKeyFrameByIndex(&nKeyFrame, &vTrans, i);
                        m_sliderTimeLine.SetTic(nKeyFrame);
                    }
                }
                str.Format("关键帧数：%d", nKeyFrameNum);
                m_staticKeyFrameNum.SetWindowText(str);
            }
        }
    }
    else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_FORCE_FIELD))
    {
        int nSelectForceField = m_pParticlePage->m_comboForceField.GetCurSel();
        if (nSelectForceField != CB_ERR)
        {
            IEKG3DSFXParticleForceField *pForceField = NULL;
            pSFX->GetForceField(nSelectForceField, &pForceField);
            ASSERT(pForceField);

			TKG3DTimeLine<FLOAT5>* pLine =
				static_cast<TKG3DTimeLine<FLOAT5>*>(pForceField->GetForceTimeLine());

			int nKeyFrameNum = (int)pLine->GetSize();
            str.Format("力场：%d", nKeyFrameNum);
            m_staticKeyFrameNum.SetWindowText(str);

            for (int i = 0; i < nKeyFrameNum; i++)
            {
                D3DXVECTOR3 Force;
                int nKeyFrame;
                pLine->GetKeyFrameByIndex(
                    &nKeyFrame, &Force, i
                    );
                m_sliderTimeLine.SetTic(nKeyFrame);
            }
        }
    }
}

void KSceneSFXEditorTimeView::UpdateUIForBillboardPage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pBillboardPage = pFrame->GetPanelView()->GetBillboardPage();

    if (m_pBillboardPage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;

    CString str;
    int nSelect = m_pBillboardPage->m_comboBillboard.GetCurSel();
    if (nSelect != CB_ERR)
    {
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);
        IEKG3DSFXBillboard *pBillboard = NULL;
        pSFX->GetBillboard(nSelect, &pBillboard);
        ASSERT(pBillboard);

        if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_COLOR))
        {
            int nKeyFrameNum = (int)pBillboard->DiffuseTimeLineGetSize();
            str.Format("颜色：%d", nKeyFrameNum);
            m_staticKeyFrameNum.SetWindowText(str);

            for (int i = 0; i < nKeyFrameNum; i++)
            {
                D3DXCOLOR Color;
                int nKeyFrame;
                pBillboard->DiffuseTimeLineGetKeyFrameByIndex(&nKeyFrame, &Color, i);
                m_sliderTimeLine.SetTic(nKeyFrame);
            }
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_WIDTH))
        {
            CString str;
            int nKeyFrameNum = (int)pBillboard->WidthTimeLineGetSize();
            str.Format("宽度：%d", nKeyFrameNum);
            m_staticKeyFrameNum.SetWindowText(str);

            for (int i = 0; i < nKeyFrameNum; i++)
            {
                float Width;
                int nKeyFrame;
                pBillboard->WidthTimeLineGetKeyFrameByIndex(&nKeyFrame, &Width, i);
                m_sliderTimeLine.SetTic(nKeyFrame);
            }
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_POSITION))
        {
            CString str;
            int nKeyFrameNum = (int)pBillboard->PositionTimeLineGetSize();
            str.Format("位置：%d", nKeyFrameNum);
            m_staticKeyFrameNum.SetWindowText(str);

            for (int i = 0; i < nKeyFrameNum; i++)
            {
                D3DXVECTOR3 vPos;
                int nKeyFrame;
                pBillboard->PositionTimeLineGetKeyFrameByIndex(&nKeyFrame, &vPos, i);
                m_sliderTimeLine.SetTic(nKeyFrame);
            }
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_HEIGHT))
        {
            int nKeyFrameNum = (int)pBillboard->HeightTimeLineGetSize();
            str.Format( "高度：%d", nKeyFrameNum );
            m_staticKeyFrameNum.SetWindowText(str);

            for (int i = 0; i < nKeyFrameNum; i++)
            {
                float Height;
                int nKeyFrame;
                pBillboard->HeightTimeLineGetKeyFrameByIndex(&nKeyFrame, &Height, i);
                m_sliderTimeLine.SetTic(nKeyFrame);
            }
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_TEXTURE_INDEX))
        {
            int nKeyFrameNum = (int)pBillboard->TextureFrameIndexGetSize();
            str.Format("纹理1坐标索引：%d", nKeyFrameNum);
            m_staticKeyFrameNum.SetWindowText(str);

            for (int i = 0; i < nKeyFrameNum; i++)
            {
                int nTextureIndex;
                int nKeyFrame;
                pBillboard->TextureFrameIndexGetKeyFrameByIndex(&nKeyFrame, &nTextureIndex, i);
                m_sliderTimeLine.SetTic(nKeyFrame);
            }
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_TEXTURE2_INDEX))
        {
            int nKeyFrameNum = (int)pBillboard->Texture2FrameIndexGetSize();
            str.Format("纹理2坐标索引：%d", nKeyFrameNum);
            m_staticKeyFrameNum.SetWindowText(str);

            for (int i = 0; i < nKeyFrameNum; i++)
            {
                int nTextureIndex;
                int nKeyFrame;
                pBillboard->Texture2FrameIndexGetKeyFrameByIndex(&nKeyFrame, &nTextureIndex, i);
                m_sliderTimeLine.SetTic(nKeyFrame);
            }
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_ROTATION_ANIMATION))
        {
            int nKeyFrameNum = (int)pBillboard->RotationTimeLineGetSize();
            str.Format("旋转：%d", nKeyFrameNum);
            m_staticKeyFrameNum.SetWindowText(str);

            for (int i = 0; i < nKeyFrameNum; i++)
            {
                float fRotation;
                int nKeyFrame;
                pBillboard->RotationTimeLineGetKeyFrameByIndex(&nKeyFrame, &fRotation, i);
                m_sliderTimeLine.SetTic(nKeyFrame);
            }
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_UV_OFFSET))
        {
            int nKeyFrameNum = (int)pBillboard->UVTimeLineGetSize();
            str.Format("ＵＶ动画：%d", nKeyFrameNum);
            m_staticKeyFrameNum.SetWindowText(str);

            for (int i = 0; i < nKeyFrameNum; i++)
            {
                D3DXVECTOR2 vUV;
                int nKeyFrame;
                pBillboard->UVTimeLineGetKeyFrameByIndex(&nKeyFrame, &vUV, i);
                m_sliderTimeLine.SetTic(nKeyFrame);
            }
        }
    }
 }

void KSceneSFXEditorTimeView::UpdateUIForModelPage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pModelPage = pFrame->GetPanelView()->GetModelPage();

    if (!m_pModelPage)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DModel *pModel = (IEKG3DModel*)pSFX->GetModel(m_pModelPage->m_dwCurIndex);
    if (!pModel)
        return;

    IEKG3DMaterial *pMtl = NULL;
    pModel->GetIEKG3DMaterial(&pMtl);
    ASSERT(pMtl);

    CString str;
    int nSelect = m_pModelPage->m_comboSubset.GetCurSel();
    int nMdlIdx = m_pModelPage->m_dwCurIndex;
    if (nSelect != CB_ERR)
    {
        DWORD dwNumMaterial = 0;
        pMtl->GetMaterialCount(&dwNumMaterial);
        ASSERT((DWORD)nSelect < dwNumMaterial);

		TKG3DTimeLine<D3DXCOLOR>* pLine =
			static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pSFX->GetSubsetColorLine(nMdlIdx,nSelect));


        int nKeyFrameNum = (int)
            pLine->GetSize();
        str.Format("偏色：%d", nKeyFrameNum);
        m_staticKeyFrameNum.SetWindowText(str);

        for (int i = 0; i < nKeyFrameNum; i++)
        {
            D3DXCOLOR Color;
            int nKeyFrame;
            pLine->GetKeyFrameByIndex(&nKeyFrame, &Color, i);
            m_sliderTimeLine.SetTic(nKeyFrame);
        }
    }
}

void KSceneSFXEditorTimeView::UpdateUI()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    m_staticKeyFrameNum.EnableWindow();
    m_staticCurrentKeyFrame.EnableWindow();
    m_sliderTimeLine.EnableWindow();
    m_sliderTimeLine.SetRange(0, pSFX->GetTotalFrameNum());
    m_sliderTimeLine.SetTicFreq(1);
    m_sliderTimeLine.ClearTics();

    CString str;
    str.Format("Frame：%d", m_sliderTimeLine.GetPos());
    m_staticCurrentKeyFrame.SetWindowText(str);

    if (pFrame->GetPanelView()->GetActivePageIndex() == 0)
        UpdateUIForParticlePage();
    else if (pFrame->GetPanelView()->GetActivePageIndex() == 2)
        UpdateUIForBillboardPage();
    else if (pFrame->GetPanelView()->GetActivePageIndex() == 4)
        UpdateUIForModelPage();

    UpdateEditStatusUI();
    UpdateSelectObjUI();
}

void KSceneSFXEditorTimeView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();

    // TODO: Add your specialized code here and/or call the base class
    HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_PLAY));
    m_checkPlaySFX.SetIcon(hIcon);

}

void KSceneSFXEditorTimeView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: Add your message handler code here and/or call default
    CFormView::OnHScroll(nSBCode, nPos, pScrollBar);

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    int nFrame = m_sliderTimeLine.GetPos();
    if (m_bAuto0)
    {
        if (nFrame >= (int)pSFX->GetTotalFrameNum())
            m_sliderTimeLine.SetPos(0);
    }

    pSFX->SetCurrentFrame((float)nFrame);
    pSFX->TransformationFrameMove();

    SFXUINeedUpdate();
}

void KSceneSFXEditorTimeView::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    if (nIDEvent == SFX_PLAY_TIMER)
    {
        KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
        if (!pDoc)
            return;
        IEKG3DSceneSFXEditor *pScene = NULL;
        pDoc->GetSceneSFXEditor(&pScene);
        if (!pScene)
            return;
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);
        float fCurrentFrame = pSFX->GetCurrentFrame();
        m_sliderTimeLine.SetPos((int)fCurrentFrame);

        CString str;
        str.Format("Frame：%f", pSFX->GetCurrentFrame());
        m_staticCurrentKeyFrame.SetWindowText(str);

        str.Format("ParticleNum: %d",
            pSFX->GetParticleContainerRectangleParticleSize());
        AfxGetMainWnd()->SetWindowText(str);

    }

    CFormView::OnTimer(nIDEvent);
}

void KSceneSFXEditorTimeView::DeleteKeyFrameParticlePage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pParticlePage = pFrame->GetPanelView()->GetParticlePage();

    if (m_pParticlePage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *piSceneSFXEditor = NULL;
    pDoc->GetSceneSFXEditor(&piSceneSFXEditor);
    if (!piSceneSFXEditor)
        return;
    IEKG3DSFX *pSFX = NULL;
    piSceneSFXEditor->GetSFX(&pSFX);
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    piSceneSFXEditor->GetSceneEditBase(&piSceneEditorBase);

    int nPos = m_sliderTimeLine.GetPos();

    if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_PARTICLE_KIND))
    {
        int nSelectTimeLine = m_pParticlePage->m_comboParticleKind.GetCurSel();
        if (nSelectTimeLine != CB_ERR)
        {
            IEKG3DSFXParticleLifeTimeLine *pLifeTime = NULL;
            pSFX->GetParticleLifeTimeLine(
                nSelectTimeLine,
                &pLifeTime
                );

            ASSERT(pLifeTime);

            if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_COLOR))
            {
				TKG3DTimeLine<D3DXCOLOR>* pColorLine = static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLine());
                pColorLine->RemoveKeyFrame(nPos);
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_WIDTH))
            {
				TKG3DTimeLine<float>* pWidthLine = static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetWidthTimeLine());
                pWidthLine->RemoveKeyFrame(nPos);
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_HEIGHT))
            {
				TKG3DTimeLine<float>* pHeightLine = static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetHeightTimeLine());
				pHeightLine->RemoveKeyFrame(nPos);
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_TEXTURE_INDEX))
            {
				TKG3DTimeLine<int>* pTextLine = static_cast<TKG3DTimeLine<int>*>(pLifeTime->GetTextureFrameTimeLine());
				pTextLine->RemoveKeyFrame(nPos);
            }
        }
    }
    else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_LAUNCHER))
    {
        int nSelectLauncher = m_pParticlePage->m_comboLauncher.GetCurSel();
        if (nSelectLauncher != CB_ERR)
        {
            IEKG3DSFXParticleLauncher *pLauncher = NULL;
            pSFX->GetParticleLauncher(nSelectLauncher, &pLauncher);
            ASSERT(pLauncher);

            if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_PARTICLE_NUM))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleNumLine());

                pLine->RemoveKeyFrame(nPos);
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MIN_LIFE))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMinLine());

                pLine->RemoveKeyFrame(nPos);
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MAX_LIFE))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMaxLine());

                pLine->RemoveKeyFrame(nPos);
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MIN_SPEED))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMinLine());

                pLine->RemoveKeyFrame(nPos);
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MAX_SPEED))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMaxLine());

                pLine->RemoveKeyFrame(nPos);
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_SCATTER_MIN))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMin());

                pLine->RemoveKeyFrame(nPos);
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_SCATTER_MAX))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMax());

                pLine->RemoveKeyFrame(nPos);
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_TRANSFORMATION))
            {
                if (GetEditState(piSceneEditorBase) == SCENESTATE_MOVE)
				{
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_TranslationTimeLine);

					pLine->RemoveKeyFrame(nPos);
				}
                else if (GetEditState(piSceneEditorBase) == SCENESTATE_ROTATE)
				{
                    /*
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pLauncher->GetRotationTimeLine());

                    pLine->RemoveKeyFrame(nPos);
                    */
				}
                else if (GetEditState(piSceneEditorBase) == SCENESTATE_SCALE)
				{
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_ScalingTimeLine);

					pLine->RemoveKeyFrame(nPos);
				}
            }
        }
    }
    else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_FORCE_FIELD))
    {
        int nSelectForceField = m_pParticlePage->m_comboForceField.GetCurSel();
        if (nSelectForceField != CB_ERR)
        {
            IEKG3DSFXParticleForceField *pForceField = NULL;
            pSFX->GetForceField(nSelectForceField, &pForceField);
            ASSERT(pForceField);

			TKG3DTimeLine<FLOAT5>* pLine =
				static_cast<TKG3DTimeLine<FLOAT5>*>(pForceField->GetForceTimeLine());

			pLine->RemoveKeyFrame(nPos);
        }
    }

    SFXUINeedUpdate();
}
void KSceneSFXEditorTimeView::DeleteKeyFrameBillboardPage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pBillboardPage = pFrame->GetPanelView()->GetBillboardPage();

    if (m_pBillboardPage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;
    int nPos = m_sliderTimeLine.GetPos();

    int nSelect = m_pBillboardPage->m_comboBillboard.GetCurSel();
    if (nSelect != CB_ERR)
    {
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);
        IEKG3DSFXBillboard *pBillboard = NULL;
        pSFX->GetBillboard(nSelect, &pBillboard);
        ASSERT(pBillboard);

        if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_COLOR))
        {
            pBillboard->DiffuseTimeLineRemoveKeyFrame(nPos);
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_POSITION))
        {
            pBillboard->PositionTimeLineRemoveKeyFrame(nPos);
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_WIDTH))
        {
            pBillboard->WidthTimeLineRemoveKeyFrame(nPos);
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_HEIGHT))
        {
            pBillboard->HeightTimeLineRemoveKeyFrame(nPos);
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_TEXTURE_INDEX))
        {
            pBillboard->TextureFrameIndexRemoveKeyFrame(nPos);
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_TEXTURE2_INDEX))
        {
            pBillboard->Texture2FrameIndexRemoveKeyFrame(nPos);
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_ROTATION_ANIMATION))
        {
            pBillboard->RotationTimeLineRemoveKeyFrame(nPos);
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_UV_OFFSET))
        {
            pBillboard->UVTimeLineRemoveKeyFrame(nPos);
        }
    }
}
void KSceneSFXEditorTimeView::DeleteKeyFrameModelPage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pModelPage = pFrame->GetPanelView()->GetModelPage();

    if (m_pModelPage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;
    int nPos = m_sliderTimeLine.GetPos();

    int nSelect = m_pModelPage->m_comboSubset.GetCurSel();
    int nMdlIdx = m_pModelPage->m_dwCurIndex;
    if (nSelect != CB_ERR)
    {
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);
		TKG3DTimeLine<D3DXCOLOR>* pLine =
			static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pSFX->GetSubsetColorLine(nMdlIdx,nSelect));

        pLine->RemoveKeyFrame(nPos);
    }
}

void KSceneSFXEditorTimeView::OnBnClickedButtonDelKeyFrame()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();

    if (pFrame->GetPanelView()->GetActivePageIndex() == 0)
        DeleteKeyFrameParticlePage();
    else if (pFrame->GetPanelView()->GetActivePageIndex() == 2)
        DeleteKeyFrameBillboardPage();
    else if (pFrame->GetPanelView()->GetActivePageIndex() == 4)
        DeleteKeyFrameModelPage();

    SFXUINeedUpdate();
}

void KSceneSFXEditorTimeView::OnBnClickedButtonMoveKeyFrameLeft()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();

    if (pFrame->GetPanelView()->GetActivePageIndex() == 0)
        MoveKeyFrameLeftParticlePage();
    else if (pFrame->GetPanelView()->GetActivePageIndex() == 2)
        MoveKeyFrameLeftBillboardPage();
    else if (pFrame->GetPanelView()->GetActivePageIndex() == 4)
        MoveKeyFrameLeftModelPage();

    SFXUINeedUpdate();
}

void KSceneSFXEditorTimeView::MoveKeyFrameLeftParticlePage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pParticlePage = pFrame->GetPanelView()->GetParticlePage();

    if (m_pParticlePage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;
    int nPos = m_sliderTimeLine.GetPos();
    if (nPos < 1)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

    if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_PARTICLE_KIND))
    {
        int nSelectTimeLine = m_pParticlePage->m_comboParticleKind.GetCurSel();
        if (nSelectTimeLine != CB_ERR)
        {
            IEKG3DSFXParticleLifeTimeLine *pLifeTime = NULL;
            pSFX->GetParticleLifeTimeLine(
                nSelectTimeLine,
                &pLifeTime
                );

            ASSERT(pLifeTime);

            if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_COLOR))
            {
				TKG3DTimeLine<D3DXCOLOR>* pLine = static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLine());

                D3DXCOLOR Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_WIDTH))
            {
                float Value;
				TKG3DTimeLine<float>* pLine = static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetWidthTimeLine());
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_HEIGHT))
            {
                float Value;
				TKG3DTimeLine<float>* pLine = static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetHeightTimeLine());
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_TEXTURE_INDEX))
            {
                int Value;
				TKG3DTimeLine<int>* pLine = static_cast<TKG3DTimeLine<int>*>(pLifeTime->GetTextureFrameTimeLine());
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
        }
    }
    else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_LAUNCHER))
    {
        int nSelectLauncher = m_pParticlePage->m_comboLauncher.GetCurSel();
        if (nSelectLauncher != CB_ERR)
        {
            IEKG3DSFXParticleLauncher *pLauncher = NULL;
            pSFX->GetParticleLauncher(nSelectLauncher, &pLauncher);
            ASSERT(pLauncher);

            if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_PARTICLE_NUM))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleNumLine());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MIN_LIFE))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMinLine());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MAX_LIFE))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMaxLine());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MIN_SPEED))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMinLine());

				float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MAX_SPEED))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMaxLine());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_SCATTER_MIN))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMin());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_SCATTER_MAX))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMax());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_TRANSFORMATION))
            {
                D3DXVECTOR3 Value;
                if (GetEditState(piSceneEditorBase) == SCENESTATE_MOVE)
                {
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_TranslationTimeLine);

                    KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                    KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                    KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                    KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
                }
                else if (GetEditState(piSceneEditorBase) == SCENESTATE_ROTATE)
                {
                    /*
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pLauncher->GetRotationTimeLine());

                    KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                    KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                    KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                    KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));*/
                }
                else if (GetEditState(piSceneEditorBase) == SCENESTATE_SCALE)
                {
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_ScalingTimeLine);

                    KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                    KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
                    KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                    KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
                }
            }
        }
    }
    else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_FORCE_FIELD))
    {
        int nSelectForceField = m_pParticlePage->m_comboForceField.GetCurSel();
        if (nSelectForceField != CB_ERR)
        {
            IEKG3DSFXParticleForceField *pForceField = NULL;
            pSFX->GetForceField(nSelectForceField, &pForceField);
            ASSERT(pForceField);

			TKG3DTimeLine<FLOAT5>* pLine =
				static_cast<TKG3DTimeLine<FLOAT5>*>(pForceField->GetForceTimeLine());

            D3DXVECTOR3 Value;
            KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
            KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
        }
    }

    m_sliderTimeLine.SetPos(nPos - 1);

Exit0:
    return;
}

void KSceneSFXEditorTimeView::MoveKeyFrameLeftBillboardPage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pBillboardPage = pFrame->GetPanelView()->GetBillboardPage();

    if (m_pBillboardPage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;
    int nPos = m_sliderTimeLine.GetPos();
    if (nPos < 1)
        return;

    int nSelect = m_pBillboardPage->m_comboBillboard.GetCurSel();
    if (nSelect != CB_ERR)
    {
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);
        IEKG3DSFXBillboard *pBillboard = NULL;
        pSFX->GetBillboard(nSelect, &pBillboard);
        ASSERT(pBillboard);

        if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_POSITION))
        {
            D3DXVECTOR3 Value;
            KG_COM_PROCESS_ERROR(pBillboard->PositionTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->PositionTimeLineIsKeyFrame(nPos - 1));
            KG_COM_PROCESS_ERROR(pBillboard->PositionTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->PositionTimeLineInsertKeyFrame(nPos - 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_WIDTH))
        {
            float Value;
            KG_COM_PROCESS_ERROR(pBillboard->WidthTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->WidthTimeLineIsKeyFrame(nPos - 1));
            KG_COM_PROCESS_ERROR(pBillboard->WidthTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->WidthTimeLineInsertKeyFrame(nPos - 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_HEIGHT))
        {
            float Value;
            KG_COM_PROCESS_ERROR(pBillboard->HeightTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->HeightTimeLineIsKeyFrame(nPos - 1));
            KG_COM_PROCESS_ERROR(pBillboard->HeightTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->HeightTimeLineInsertKeyFrame(nPos - 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_COLOR))
        {
            D3DXCOLOR Value;
            KG_COM_PROCESS_ERROR(pBillboard->DiffuseTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->DiffuseTimeLineIsKeyFrame(nPos - 1));
            KG_COM_PROCESS_ERROR(pBillboard->DiffuseTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->DiffuseTimeLineInsertKeyFrame(nPos - 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_TEXTURE_INDEX))
        {
            int Value;
            KG_COM_PROCESS_ERROR(pBillboard->TextureFrameIndexGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->TextureFrameIndexIsKeyFrame(nPos - 1));
            KG_COM_PROCESS_ERROR(pBillboard->TextureFrameIndexRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->TextureFrameIndexInsertKeyFrame(nPos - 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_TEXTURE2_INDEX))
        {
            int Value;
            KG_COM_PROCESS_ERROR(pBillboard->Texture2FrameIndexGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->Texture2FrameIndexIsKeyFrame(nPos - 1));
            KG_COM_PROCESS_ERROR(pBillboard->Texture2FrameIndexRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->Texture2FrameIndexInsertKeyFrame(nPos - 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_ROTATION_ANIMATION))
        {
            float Value;
            KG_COM_PROCESS_ERROR(pBillboard->RotationTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->RotationTimeLineIsKeyFrame(nPos - 1));
            KG_COM_PROCESS_ERROR(pBillboard->RotationTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->RotationTimeLineInsertKeyFrame(nPos - 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_UV_OFFSET))
        {
            D3DXVECTOR2 Value;
            KG_COM_PROCESS_ERROR(pBillboard->UVTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->UVTimeLineIsKeyFrame(nPos - 1));
            KG_COM_PROCESS_ERROR(pBillboard->UVTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->UVTimeLineInsertKeyFrame(nPos - 1, Value));
        }
    }

    m_sliderTimeLine.SetPos(nPos - 1);

Exit0:
    return;
}

void KSceneSFXEditorTimeView::MoveKeyFrameLeftModelPage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pModelPage = pFrame->GetPanelView()->GetModelPage();

    if (m_pModelPage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;
    int nPos = m_sliderTimeLine.GetPos();
    if (nPos < 1)
        return;

    int nSelect = m_pModelPage->m_comboSubset.GetCurSel();
    if (nSelect != CB_ERR)
    {
        IEKG3DSFX *pSFX = NULL;
        pScene->GetSFX(&pSFX);
        DWORD dwIndex = m_pModelPage->m_dwCurIndex;
		TKG3DTimeLine<D3DXCOLOR>* pLine =
			static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pSFX->GetSubsetColorLine(dwIndex,nSelect));

          /*  *(pSFX->m_vecSubsetColor[dwIndex]->begin() + nSelect);*/
        D3DXCOLOR Value;
        KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
        KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos - 1));
        KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
        KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos - 1, Value));
    }

    m_sliderTimeLine.SetPos(nPos - 1);

Exit0:
    return;

}

void KSceneSFXEditorTimeView::MoveKeyFrameRightParticlePage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pParticlePage = pFrame->GetPanelView()->GetParticlePage();

    if (m_pParticlePage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    IEKG3DSceneEditorBase *piSceneEditorBase = NULL;
    pScene->GetSceneEditBase(&piSceneEditorBase);

    int nPos = m_sliderTimeLine.GetPos();
    int nPosMax = pSFX->GetTotalFrameNum();
    if (nPos > nPosMax - 2)
        return;

    if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_PARTICLE_KIND))
    {
        int nSelectTimeLine = m_pParticlePage->m_comboParticleKind.GetCurSel();
        if (nSelectTimeLine != CB_ERR)
        {
            IEKG3DSFXParticleLifeTimeLine *pLifeTime = NULL;
            pSFX->GetParticleLifeTimeLine(
                nSelectTimeLine,
                &pLifeTime
                );

            ASSERT(pLifeTime);

            if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_COLOR))
            {
                D3DXCOLOR Value;
				TKG3DTimeLine<D3DXCOLOR>* pLine = static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pLifeTime->GetColorTimeLine());
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_WIDTH))
            {
                float Value;
				TKG3DTimeLine<float>* pLine = static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetWidthTimeLine());
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_HEIGHT))
            {
                float Value;
				TKG3DTimeLine<float>* pLine = static_cast<TKG3DTimeLine<float>*>(pLifeTime->GetHeightTimeLine());
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_TEXTURE_INDEX))
            {
                int Value;
				TKG3DTimeLine<int>* pLine = static_cast<TKG3DTimeLine<int>*>(pLifeTime->GetTextureFrameTimeLine());
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
        }
    }
    else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_LAUNCHER))
    {
        int nSelectLauncher = m_pParticlePage->m_comboLauncher.GetCurSel();
        if (nSelectLauncher != CB_ERR)
        {
            IEKG3DSFXParticleLauncher *pLauncher = NULL;
            pSFX->GetParticleLauncher(nSelectLauncher, &pLauncher);
            ASSERT(pLauncher);

            if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_PARTICLE_NUM))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleNumLine());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MIN_LIFE))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMinLine());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MAX_LIFE))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleLifeMaxLine());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MIN_SPEED))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMinLine());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_MAX_SPEED))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleSpeedMaxLine());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_SCATTER_MIN))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMin());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_SCATTER_MAX))
            {
				TKG3DTimeLine<float>* pLine =
					static_cast<TKG3DTimeLine<float>*>(pLauncher->GetParticleScatterMax());

                float Value;
                KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
            }
            else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_TRANSFORMATION))
            {
                D3DXVECTOR3 Value;
                if (GetEditState(piSceneEditorBase) == SCENESTATE_MOVE)
                {
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_TranslationTimeLine);

                    KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                    KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                    KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                    KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
                }
                else if (GetEditState(piSceneEditorBase) == SCENESTATE_ROTATE)
                {
                    /*
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pLauncher->GetRotationTimeLine());

                    KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                    KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                    KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                    KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));*/
                }
                else if (GetEditState(piSceneEditorBase) == SCENESTATE_SCALE)
                {
					TKG3DTimeLine<D3DXVECTOR3>* pLine =
						static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_ScalingTimeLine);

                    KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
                    KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
                    KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
                    KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
                }
            }
        }
    }
    else if (m_pParticlePage->IsDlgButtonChecked(IDC_RADIO_FORCE_FIELD))
    {
        int nSelectForceField = m_pParticlePage->m_comboForceField.GetCurSel();
        if (nSelectForceField != CB_ERR)
        {
            IEKG3DSFXParticleForceField *pForceField = NULL;
            pSFX->GetForceField(nSelectForceField, &pForceField);
            ASSERT(pForceField);

			TKG3DTimeLine<FLOAT5>* pLine =
				static_cast<TKG3DTimeLine<FLOAT5>*>(pForceField->GetForceTimeLine());

            D3DXVECTOR3 Value;
            KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
            KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
        }
    }

    m_sliderTimeLine.SetPos(nPos + 1);

Exit0:
    return;
}

void KSceneSFXEditorTimeView::MoveKeyFrameRightBillboardPage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pBillboardPage = pFrame->GetPanelView()->GetBillboardPage();

    if (m_pBillboardPage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    int nPos = m_sliderTimeLine.GetPos();
    int nPosMax = pSFX->GetTotalFrameNum();
    if (nPos > nPosMax - 2)
        return;

    int nSelect = m_pBillboardPage->m_comboBillboard.GetCurSel();
    if (nSelect != CB_ERR)
    {
        IEKG3DSFXBillboard *pBillboard = NULL;
        pSFX->GetBillboard(nSelect, &pBillboard);
        ASSERT(pBillboard);

        if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_POSITION))
        {
            D3DXVECTOR3 Value;
            KG_COM_PROCESS_ERROR(pBillboard->PositionTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->PositionTimeLineIsKeyFrame(nPos + 1));
            KG_COM_PROCESS_ERROR(pBillboard->PositionTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->PositionTimeLineInsertKeyFrame(nPos + 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_WIDTH))
        {
            float Value;
            KG_COM_PROCESS_ERROR(pBillboard->WidthTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->WidthTimeLineIsKeyFrame(nPos + 1));
            KG_COM_PROCESS_ERROR(pBillboard->WidthTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->WidthTimeLineInsertKeyFrame(nPos + 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_HEIGHT))
        {
            float Value;
            KG_COM_PROCESS_ERROR(pBillboard->HeightTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->HeightTimeLineIsKeyFrame(nPos + 1));
            KG_COM_PROCESS_ERROR(pBillboard->HeightTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->HeightTimeLineInsertKeyFrame(nPos + 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_COLOR))
        {
            D3DXCOLOR Value;
            KG_COM_PROCESS_ERROR(pBillboard->DiffuseTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->DiffuseTimeLineIsKeyFrame(nPos + 1));
            KG_COM_PROCESS_ERROR(pBillboard->DiffuseTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->DiffuseTimeLineInsertKeyFrame(nPos + 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_TEXTURE_INDEX))
        {
            int Value;
            KG_COM_PROCESS_ERROR(pBillboard->TextureFrameIndexGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->TextureFrameIndexIsKeyFrame(nPos + 1));
            KG_COM_PROCESS_ERROR(pBillboard->TextureFrameIndexRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->TextureFrameIndexInsertKeyFrame(nPos + 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_BILLBOARD_TEXTURE2_INDEX))
        {
            int Value;
            KG_COM_PROCESS_ERROR(pBillboard->Texture2FrameIndexGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->Texture2FrameIndexIsKeyFrame(nPos + 1));
            KG_COM_PROCESS_ERROR(pBillboard->Texture2FrameIndexRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->Texture2FrameIndexInsertKeyFrame(nPos + 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_ROTATION_ANIMATION))
        {
            float Value;
            KG_COM_PROCESS_ERROR(pBillboard->RotationTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->RotationTimeLineIsKeyFrame(nPos + 1));
            KG_COM_PROCESS_ERROR(pBillboard->RotationTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->RotationTimeLineInsertKeyFrame(nPos + 1, Value));
        }
        else if (m_pBillboardPage->IsDlgButtonChecked(IDC_RADIO_UV_OFFSET))
        {
            D3DXVECTOR2 Value;
            KG_COM_PROCESS_ERROR(pBillboard->UVTimeLineGetData(&Value, (float)nPos));
            KG_PROCESS_ERROR(S_OK != pBillboard->UVTimeLineIsKeyFrame(nPos + 1));
            KG_COM_PROCESS_ERROR(pBillboard->UVTimeLineRemoveKeyFrame(nPos));
            KG_COM_PROCESS_ERROR(pBillboard->UVTimeLineInsertKeyFrame(nPos + 1, Value));
        }
    }

    m_sliderTimeLine.SetPos(nPos + 1);

Exit0:
    return;
}

void KSceneSFXEditorTimeView::MoveKeyFrameRightModelPage()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();
    m_pModelPage = pFrame->GetPanelView()->GetModelPage();

    if (m_pModelPage == NULL)
        return;

    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;
    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    int nPos = m_sliderTimeLine.GetPos();
    int nPosMax = pSFX->GetTotalFrameNum();
    if (nPos > nPosMax - 2)
        return;

    int nSelect = m_pModelPage->m_comboSubset.GetCurSel();
    int nMdlIdx = m_pModelPage->m_dwCurIndex;
    if (nSelect != CB_ERR)
    {
		TKG3DTimeLine<D3DXCOLOR>* pLine =
			static_cast<TKG3DTimeLine<D3DXCOLOR>*>(pSFX->GetSubsetColorLine(nMdlIdx,nSelect));
        D3DXCOLOR Value;
        KG_COM_PROCESS_ERROR(pLine->GetData(&Value, (float)nPos));
        KG_PROCESS_ERROR(S_OK != pLine->IsKeyFrame(nPos + 1));
        KG_COM_PROCESS_ERROR(pLine->RemoveKeyFrame(nPos));
        KG_COM_PROCESS_ERROR(pLine->InsertKeyFrame(nPos + 1, Value));
    }

    m_sliderTimeLine.SetPos(nPos + 1);

Exit0:
    return;
}

void KSceneSFXEditorTimeView::OnBnClickedButtonMoveKeyFrameRight()
{
	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
    KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();

    if (pFrame->GetPanelView()->GetActivePageIndex() == 0)
        MoveKeyFrameRightParticlePage();
    else if (pFrame->GetPanelView()->GetActivePageIndex() == 2)
        MoveKeyFrameRightBillboardPage();
    else if (pFrame->GetPanelView()->GetActivePageIndex() == 4)
        MoveKeyFrameRightModelPage();

    SFXUINeedUpdate();
}

void KSceneSFXEditorTimeView::OnBnClickedCheckPlaySfx()
{
    // TODO: Add your control notification handler code here
    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);

    if (m_checkPlaySFX.GetCheck())
    {
        HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_STOP));
        m_checkPlaySFX.SetIcon(hIcon);

        SetTimer(SFX_PLAY_TIMER, 1, 0);

        pSFX->PlayAnimation(SFX_AP_LOOP);

        m_buttonMoveEnd.EnableWindow(FALSE);
        m_buttonMoveStart.EnableWindow(FALSE);
    }
    else
    {
        HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_PLAY));
        m_checkPlaySFX.SetIcon(hIcon);

        KillTimer(SFX_PLAY_TIMER);

        pSFX->PlayAnimation(SFX_AP_PAUSE);

        m_buttonMoveEnd.EnableWindow();
        m_buttonMoveStart.EnableWindow();
    }

    SFXUINeedUpdate();
}

void KSceneSFXEditorTimeView::OnBnClickedButtonMoveStart()
{
    // TODO: Add your control notification handler code here
    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    pSFX->SetCurrentFrame(0);
    m_sliderTimeLine.SetPos(0);

    SFXUINeedUpdate();
}

void KSceneSFXEditorTimeView::OnBnClickedButtonMoveEnd()
{
    // TODO: Add your control notification handler code here
    KSceneSFXEditorDoc *pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *pScene = NULL;
    pDoc->GetSceneSFXEditor(&pScene);
    if (!pScene)
        return;

    IEKG3DSFX *pSFX = NULL;
    pScene->GetSFX(&pSFX);
    DWORD dwTotal = pSFX->GetTotalFrameNum();
    pSFX->SetCurrentFrame((float)dwTotal);
    m_sliderTimeLine.SetPos(dwTotal);

    SFXUINeedUpdate();
}

void KSceneSFXEditorTimeView::OnBnClickedCheckAuto0()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
}

void KSceneSFXEditorTimeView::OnBnClickedCheckAutokey()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
}

void KSceneSFXEditorTimeView::UpdateEditStatusUI()
{
}

void KSceneSFXEditorTimeView::UpdateSelectObjUI()
{
    //KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)GetDocument();
    //IEKG3DSceneSFXEditor *pEditor = (IEKG3DSceneSFXEditor*)pDoc->GetScene();

    //MFCFramework::CMainFrame *pMainFrame = (MFCFramework::CMainFrame*)AfxGetMainWnd();
    //KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();

    //if (pEditor && pEditor->m_listSelectedEntity.size() == 1)
    //{
    //    KG3DSceneEntity *pEntity = &(*pEditor->m_listSelectedEntity.begin());
    //    CString str;
    //    if (pEditor->m_dwEditState == SCENESTATE_MOVE)
    //    {
    //        D3DXVECTOR3 vPos;
    //        pEntity->GetTranslation(&vPos);
    //        str.Format("%f", vPos.x);
    //        m_editTransX.SetWindowText(str);
    //        str.Format("%f", vPos.y);
    //        m_editTransY.SetWindowText(str);
    //        str.Format("%f", vPos.z);
    //        m_editTransZ.SetWindowText(str);
    //    }
    //    else if (pEditor->m_dwEditState == SCENESTATE_ROTATE)
    //    {
    //        if (pEntity->m_Type == SCENEENTITY_SFX_LAUNCHER ||
    //            pEntity->m_Type == SCENEENTITY_SFX_BILLBOARD ||
    //            pEntity->m_Type == SCENEENTITY_SFX_TRACK)
    //        {
    //            D3DXQUATERNION qRot;
    //            D3DXVECTOR3 vRot;
    //            pEntity->GetRotation(&qRot, &vRot);
    //            float fValue = vRot.x / D3DX_PI * 180;
    //            str.Format("%f", fValue);
    //            m_editTransX.SetWindowText(str);
    //            fValue = vRot.y / D3DX_PI * 180;
    //            str.Format("%f", fValue);
    //            m_editTransY.SetWindowText(str);
    //            fValue = vRot.z / D3DX_PI * 180;
    //            str.Format("%f", fValue);
    //            m_editTransZ.SetWindowText(str);
    //        }
    //    }
    //    else if (pEditor->m_dwEditState == SCENESTATE_SCALE)
    //    {
    //        D3DXVECTOR3 vScale;
    //        pEntity->GetScaling(&vScale);
    //        str.Format("%f", vScale.x);
    //        m_editTransX.SetWindowText(str);
    //        str.Format("%f", vScale.y);
    //        m_editTransY.SetWindowText(str);
    //        str.Format("%f", vScale.z);
    //        m_editTransZ.SetWindowText(str);
    //    }
    //    else
    //    {
    //        m_editTransX.SetWindowText("");
    //        m_editTransY.SetWindowText("");
    //        m_editTransZ.SetWindowText("");
    //    }
    //}
    //else
    //{
    //    m_editTransX.SetWindowText("");
    //    m_editTransY.SetWindowText("");
    //    m_editTransZ.SetWindowText("");
    //}

}


void KSceneSFXEditorTimeView::OnBnClickedButtonSetKeyframe()
{
    // TODO: Add your control notification handler code here
    SetTransformation(TRUE);
    UpdateUI();
}

void KSceneSFXEditorTimeView::SetTransformation(BOOL bForceSet)
{
    HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;

    KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *piSFXEditor = NULL;
    pDoc->GetSceneSFXEditor(&piSFXEditor);
    if (!piSFXEditor)
        return;
    IEKG3DSceneEditorBase *pEditor = NULL;
    piSFXEditor->GetSceneEditBase(&pEditor);

    if (!bForceSet && !m_bAutoKey)
        return;

	//KG3DSceneEditorBase::EntityList::iterator i = pEditor->m_listSelectedEntity.begin();
	//while (i != pEditor->m_listSelectedEntity.end())
    GETSELECT(pEditor);
	for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
	{
        IEKG3DSceneEntity *piEntity = 0;//&(*i);
		pSelector->GetSelectedEntity(i, &piEntity);

        hRetCode = piEntity->GetType(&EntityType);
        KGLOG_COM_PROCESS_ERROR(hRetCode);

        int nPos = m_sliderTimeLine.GetPos();
        if (EntityType == SCENEENTITY_SFX_LAUNCHER)
        {
            IEKG3DSFXParticleLauncher *pLauncher = NULL;
            hRetCode = piEntity->GetPointer((VOID **)&pLauncher);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            D3DXVECTOR3 Value;
            if (GetEditState(pEditor) == SCENESTATE_MOVE)
            {
				TKG3DTimeLine<D3DXVECTOR3>* pLine =
					static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_TranslationTimeLine);

                pLauncher->GetTranslation(&Value);
                pLine->InsertKeyFrame(nPos, Value);
            }
            else if (GetEditState(pEditor) == SCENESTATE_ROTATE)
            {
                ASSERT(false);
			/*	TKG3DTimeLine<D3DXVECTOR3>* pLine =
					static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(pLauncher->GetRotationTimeLine());

                pLauncher->GetRotation(&Value);
                pLine->InsertKeyFrame(nPos, Value);*/
            }
            else if (GetEditState(pEditor) == SCENESTATE_SCALE)
            {
				TKG3DTimeLine<D3DXVECTOR3>* pLine =
					static_cast<TKG3DTimeLine<D3DXVECTOR3>*>(&pLauncher->GetRtsTimeLine()->m_ScalingTimeLine);

                pLauncher->GetScaling(&Value);
                pLine->InsertKeyFrame(nPos, Value);
            }
        }
        else if (EntityType == SCENEENTITY_SFX_BILLBOARD)
        {
            IEKG3DSFXBillboard *pBillboard = NULL;
            D3DXVECTOR3 vPos;
            hRetCode = piEntity->GetPointer((VOID **)&pBillboard);
            KGLOG_COM_PROCESS_ERROR(hRetCode);

            pBillboard->GetTranslation(&vPos);
            pBillboard->PositionTimeLineInsertKeyFrame(nPos, vPos);
        }

		//i++;
    }
Exit0:
    return;
}

void KSceneSFXEditorTimeView::OnSetTranslation()
{
	KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *piSFXEditor = NULL;
    pDoc->GetSceneSFXEditor(&piSFXEditor);
    if (!piSFXEditor)
        return;
    IEKG3DSceneEditorBase *pEditor = NULL;
    piSFXEditor->GetSceneEditBase(&pEditor);

	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
	KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();

    //KG3DSceneEditorBase::EntityList::iterator i = pEditor->m_listSelectedEntity.begin();
	//while (i != pEditor->m_listSelectedEntity.end())
	GETSELECT(pEditor);
	for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
	{
        IEKG3DSceneEntity *piEntity = NULL;//&(*i);
		pSelector->GetSelectedEntity(i, &piEntity);

		D3DXVECTOR3 Value;
        piEntity->GetTranslation(&Value);
		CString str;
		m_editTransX.GetWindowText(str);
		Value.x = (float)atof(str);
		m_editTransY.GetWindowText(str);
		Value.y = (float)atof(str);
		m_editTransZ.GetWindowText(str);
		Value.z = (float)atof(str);
		piEntity->SetTranslation(&Value);
        piEntity->UpdateTransformation();
		//++i;
	}
}

void KSceneSFXEditorTimeView::OnSetRotation()
{
	KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *piSFXEditor = NULL;
    pDoc->GetSceneSFXEditor(&piSFXEditor);
    if (!piSFXEditor)
        return;
    IEKG3DSceneEditorBase *pEditor = NULL;
    piSFXEditor->GetSceneEditBase(&pEditor);

	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
	KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();

	//KG3DSceneEditorBase::EntityList::iterator i = pEditor->m_listSelectedEntity.begin();
	//while (i != pEditor->m_listSelectedEntity.end())
	GETSELECT(pEditor);
	for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i++)
	{
        IEKG3DSceneEntity *piEntity = NULL;//&(*i);
		pSelector->GetSelectedEntity(i, &piEntity);

		float x = 0, y = 0, z = 0;
		CString str;
		m_editTransX.GetWindowText(str);
		x = (float)atof(str) * D3DX_PI / 180;
		m_editTransY.GetWindowText(str);
		y = (float)atof(str) * D3DX_PI / 180;
		m_editTransZ.GetWindowText(str);
		z = (float)atof(str) * D3DX_PI / 180;

		D3DXQUATERNION q;
		D3DXQuaternionRotationYawPitchRoll(&q, y, x, z);
        piEntity->SetRotation(&q, &D3DXVECTOR3(x, y, z));
		//++i;
	}

}
void KSceneSFXEditorTimeView::OnSetScale()
{
	KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *piSFXEditor = NULL;
    pDoc->GetSceneSFXEditor(&piSFXEditor);
    if (!piSFXEditor)
        return;
    IEKG3DSceneEditorBase *pEditor = NULL;
    piSFXEditor->GetSceneEditBase(&pEditor);

	CFrameWnd *pMainFrame = (CFrameWnd*)AfxGetMainWnd(); //by dengzhihui 2006年7月6日 14:48:48
	KSceneSFXEditorFrame* pFrame = (KSceneSFXEditorFrame*)pMainFrame->GetActiveFrame();

    //KG3DSceneEditorBase::EntityList::iterator i = pEditor->m_listSelectedEntity.begin();
	//while (i != pEditor->m_listSelectedEntity.end())
	GETSELECT(pEditor);
	for (INT i = 0; i < pSelector->GetSelectedEntitySize(); i ++)
	{
        IEKG3DSceneEntity *piEntity = NULL;//&(*i);
		pSelector->GetSelectedEntity(i, &piEntity);

		float x = 0, y = 0, z = 0;
		CString str;
		m_editTransX.GetWindowText(str);
		x = (float)atof(str);
		m_editTransY.GetWindowText(str);
		y = (float)atof(str);
		m_editTransZ.GetWindowText(str);
		z = (float)atof(str);
        piEntity->SetScaling(&D3DXVECTOR3(x, y, z));
		//++i;
	}
}

void KSceneSFXEditorTimeView::OnSetTransformationX()
{
	KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *piSFXEditor = NULL;
    pDoc->GetSceneSFXEditor(&piSFXEditor);
    if (!piSFXEditor)
        return;
    IEKG3DSceneEditorBase *pEditor = NULL;
    piSFXEditor->GetSceneEditBase(&pEditor);

	if (GetEditState(pEditor) == SCENESTATE_MOVE)
		OnSetTranslation();
	else if (GetEditState(pEditor) == SCENESTATE_ROTATE)
		OnSetRotation();
	else if (GetEditState(pEditor) == SCENESTATE_SCALE)
		OnSetScale();
}

void KSceneSFXEditorTimeView::OnSetTransformationY()
{
	KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *piSFXEditor = NULL;
    pDoc->GetSceneSFXEditor(&piSFXEditor);
    if (!piSFXEditor)
        return;
    IEKG3DSceneEditorBase *pEditor = NULL;
    piSFXEditor->GetSceneEditBase(&pEditor);

	if (GetEditState(pEditor) == SCENESTATE_MOVE)
		OnSetTranslation();
	else if (GetEditState(pEditor) == SCENESTATE_ROTATE)
		OnSetRotation();
	else if (GetEditState(pEditor) == SCENESTATE_SCALE)
		OnSetScale();
}

void KSceneSFXEditorTimeView::OnSetTransformationZ()
{
	KSceneSFXEditorDoc* pDoc = (KSceneSFXEditorDoc*)GetDocument();
    if (!pDoc)
        return;
    IEKG3DSceneSFXEditor *piSFXEditor = NULL;
    pDoc->GetSceneSFXEditor(&piSFXEditor);
    if (!piSFXEditor)
        return;
    IEKG3DSceneEditorBase *pEditor = NULL;
    piSFXEditor->GetSceneEditBase(&pEditor);

	if (GetEditState(pEditor) == SCENESTATE_MOVE)
		OnSetTranslation();
	else if (GetEditState(pEditor) == SCENESTATE_ROTATE)
		OnSetRotation();
	else if (GetEditState(pEditor) == SCENESTATE_SCALE)
		OnSetScale();
}

LRESULT KSceneSFXEditorTimeView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: Add your specialized code here and/or call the base class
    switch (message)
    {
    case WM_EDIT_RECEIVE_ENTER:
        {
            switch (wParam)
            {
            case IDC_EDIT_TRANS_X:
                OnSetTransformationX();
				SetTransformation(FALSE);
                break;
            case IDC_EDIT_TRANS_Y:
                OnSetTransformationY();
				SetTransformation(FALSE);
                break;
            case IDC_EDIT_TRANS_Z:
                OnSetTransformationZ();
				SetTransformation(FALSE);
                break;

            default:
                break;
            }
			UpdateUI();
        }
    }

    return CFormView::WindowProc(message, wParam, lParam);
}
