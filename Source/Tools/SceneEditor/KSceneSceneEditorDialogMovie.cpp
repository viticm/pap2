////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSceneEditorDialogMovie.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-1-31 10:37:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSceneEditorDialogMovie.h"
#include "IEEditor.h"


// KSceneSceneEditorDialogMovie dialog

IMPLEMENT_DYNAMIC(KSceneSceneEditorDialogMovie, CDialog)

KSceneSceneEditorDialogMovie::KSceneSceneEditorDialogMovie(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneSceneEditorDialogMovie::IDD, pParent),
    m_lpSceneSceneEditor(NULL)
{

}

KSceneSceneEditorDialogMovie::~KSceneSceneEditorDialogMovie()
{
}

void KSceneSceneEditorDialogMovie::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_RECORD, m_bRecord);
    DDX_Check(pDX, IDC_PLAYANI, m_bPlay);
    DDX_Control(pDX, IDC_COMBO_IMAGE, m_ImageType);
    DDX_Control(pDX, IDC_COMBO_FITER, m_FiterType);
}

BOOL KSceneSceneEditorDialogMovie::OnInitDialog()
{
    int nResult = CDialog::OnInitDialog();

    GetDlgItem(IDC_EDIT_W)->SetWindowText(TEXT("1024"));
    GetDlgItem(IDC_EDIT_H)->SetWindowText(TEXT("768"));
    GetDlgItem(IDC_EDIT_I)->SetWindowText(TEXT("41"));

    m_ImageType.AddString(TEXT("jpg"));
    m_ImageType.AddString(TEXT("bmp"));
    m_ImageType.AddString(TEXT("png"));
    m_ImageType.AddString(TEXT("dds"));
    m_ImageType.AddString(TEXT("tga"));
    m_ImageType.SelectString(0, TEXT("jpg"));

    m_FiterType.AddString(TEXT("point"));
    m_FiterType.AddString(TEXT("linear"));
    m_FiterType.SelectString(0, TEXT("linear"));

    ((CButton*)GetDlgItem(IDC_CHECK_AUTOSIZE))->SetCheck(FALSE);

    
    return nResult;
}

BEGIN_MESSAGE_MAP(KSceneSceneEditorDialogMovie, CDialog)
    ON_BN_CLICKED(IDC_RECORD, &KSceneSceneEditorDialogMovie::OnBnClickedCheckRecord)
    ON_BN_CLICKED(IDC_PLAYANI, &KSceneSceneEditorDialogMovie::OnBnClickedPlayani)
END_MESSAGE_MAP()

void KSceneSceneEditorDialogMovie::OnBnClickedCheckRecord()
{
    UpdateData(); 

    if (m_bRecord)
    {
        CString strText;
        SIZE  Size;
        float fInterval;
        INT   nAutoScal;

        GetDlgItem(IDC_EDIT_W)->GetWindowText(strText);
        Size.cx = atoi(strText.GetBuffer());
        GetDlgItem(IDC_EDIT_H)->GetWindowText(strText);
        Size.cy = atoi(strText.GetBuffer());


        if (((CButton*)GetDlgItem(IDC_CHECK_AUTOSIZE))->GetCheck())
            nAutoScal = TRUE;
        else
            nAutoScal = FALSE;

        if (Size.cx < 10 || Size.cy < 10)
        {
            AfxMessageBox(TEXT("格式有错!!"));
            return;
        }

        GetDlgItem(IDC_EDIT_I)->GetWindowText(strText);
        fInterval = (float)atof(strText.GetBuffer());

        if (fInterval < 0)
        {
            AfxMessageBox(TEXT("帧间隔有错!!"));
            return;
        }

        D3DTEXTUREFILTERTYPE fiterType = D3DTEXF_LINEAR;
        switch (m_FiterType.GetCurSel())
        {
        case 0 :
            fiterType = D3DTEXF_POINT;
            break;
        case 1 :
            fiterType = D3DTEXF_LINEAR;
            break;
        case 2 :
            fiterType = D3DTEXF_ANISOTROPIC;
            break;
        case 3 :
            fiterType = D3DTEXF_GAUSSIANQUAD;
            break;
        default :
            fiterType = D3DTEXF_LINEAR;
            break;
        }

        D3DXIMAGE_FILEFORMAT imageType = D3DXIFF_JPG;
        switch (m_ImageType.GetCurSel())
        {
        case 0 :
            imageType = D3DXIFF_JPG;
            break;
        case 1 :
            imageType = D3DXIFF_BMP;
            break;
        case 2 :
            imageType = D3DXIFF_PNG;
            break;
        case 3 :
            imageType = D3DXIFF_DDS;
            break;
        case 4 :
            imageType = D3DXIFF_TGA;
            break;
        default :
            imageType = D3DXIFF_JPG;
            break;
        }	
		m_lpSceneSceneEditor->GetEditState(&m_dwEditorStateSave);
		m_lpSceneSceneEditor->SetEditState(SCENESTATE_CAMERAANIMATION);
	
        m_lpSceneSceneEditor->BeginMovieREC(Size, fInterval, fiterType, imageType, nAutoScal);
    }
    else
	{
        m_lpSceneSceneEditor->EndMovieREC();
	    m_lpSceneSceneEditor->SetEditState(m_dwEditorStateSave);
	}
	m_lpSceneSceneEditor->PlayEntityAnimation(m_bRecord);
}

void KSceneSceneEditorDialogMovie::OnBnClickedPlayani()
{
    UpdateData(); 

    if (m_bPlay)
    {
        m_lpSceneSceneEditor->GetEditState(&m_dwEditorStateSave);
        m_lpSceneSceneEditor->SetEditState(SCENESTATE_CAMERAANIMATION);
    }
    else
        m_lpSceneSceneEditor->SetEditState(m_dwEditorStateSave);

    m_lpSceneSceneEditor->PlayEntityAnimation(m_bPlay);
}
