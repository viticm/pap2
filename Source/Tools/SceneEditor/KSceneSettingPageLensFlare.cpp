#include "stdafx.h"
//#include "SceneEditor.h"
#include "KSceneSettingPageLensFlare.h"
//#include "IEKG3DScene.h"
//#include "KG3DGraphicsTool.h"
//#include "IEkg3dmeshskybox.h"
//#include "KG3DEngineManager.h"
//#include "SceneEditorCommon.h"	//by dengzhihui Refactor 2006年11月6日
#include <algorithm>
//#include "KSceneInterfaceGetters.h"
//#include "KG3DSceneSceneEditor.h"
#include "IEKG3DMeshLensFlare.h"
//#include "IEEditor.h"
//#include "IEKG3DTextureTable.h"
#include "KSceneSceneHelpers.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(KSceneSettingPageLensFlare, CPropertyPage)
KSceneSettingPageLensFlare::KSceneSettingPageLensFlare()
    : KSceneSettingPageBase(KSceneSettingPageLensFlare::IDD)
	, m_csText(_T(""))
	, m_nTexCellNumX(0)
	, m_nTexCellNumY(0)
	, m_fSize(0)
	, m_fHeight(0)
	, m_nWhichLensFlare(KG3DTYPE_ENVEFF_LENSFLARE_SUN)
	, m_uTimmer(0)
	, m_fVisibility(0)
	//, m_bDrawDummy(FALSE)
	, m_fBoundAspect(0)
	, m_fDummySize(0)
	, m_uWhichItemShouldBeSelected(0)
	, m_bItemSelectionPosChanged(false)
{
	
}

KSceneSettingPageLensFlare::~KSceneSettingPageLensFlare()
{
}

void KSceneSettingPageLensFlare::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_tree);
	DDX_Text(pDX, IDC_TEX, m_csText);
	DDX_Text(pDX, IDC_X, m_nTexCellNumX);
	DDX_Text(pDX, IDC_Y, m_nTexCellNumY);
	DDX_Text(pDX, IDC_HEIGHT, m_fHeight);
	DDX_Text(pDX, IDC_LENS_SIZE, m_fSize);
	DDX_Text(pDX, IDC_SS_VISIBILITY, m_fVisibility);
	//DDX_Check(pDX, IDC_SS_STATE_1, m_bDrawDummy);
	DDX_Text(pDX, IDC_SS_BOUND_ASPECT, m_fBoundAspect);
	DDX_Text(pDX, IDC_SS_OBJ_SIZE, m_fDummySize);
}

IEKG3DMeshLensFlare* KSceneSettingPageLensFlare::GetLensFlareInterface()
{
	switch(m_nWhichLensFlare)
	{
	case KG3DTYPE_ENVEFF_LENSFLARE_SUN:
		{
			return KSH::GetEngineInterface<KG3DTYPE_ENVEFF_LENSFLARE_SUN>();
		}
		break;
	case KG3DTYPE_ENVEFF_LENSFLARE_MOON:
		{
			return KSH::GetEngineInterface<KG3DTYPE_ENVEFF_LENSFLARE_MOON>();
		}
	    break;
	default:
	    break;
	}
	return NULL;
}

BEGIN_MESSAGE_MAP(KSceneSettingPageLensFlare, CPropertyPage)
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &KSceneSettingPageLensFlare::OnTvnSelchangedTree)
    ON_WM_TIMER()
    ON_WM_LBUTTONUP()
    ON_BN_CLICKED(IDC_TEXTURE, &KSceneSettingPageLensFlare::OnBnClickedTexture)
    ON_BN_CLICKED(IDC_ADD, &KSceneSettingPageLensFlare::OnBnClickedAdd)
    ON_BN_CLICKED(IDC_DEL, &KSceneSettingPageLensFlare::OnBnClickedDel)
    ON_BN_CLICKED(IDC_DEFULT, &KSceneSettingPageLensFlare::OnBnClickedDefult)
	//ON_BN_CLICKED(IDC_SS_BN_APPLY, &KSceneSettingPageLensFlare::OnBnClickedSsBnUpdate)
	//ON_BN_CLICKED(IDC_SS_BN_CANCEL, &KSceneSettingPageLensFlare::OnBnClickedSsBnCancel)
	ON_BN_CLICKED(IDC_SS_SWITCH, &KSceneSettingPageLensFlare::OnBnClickedSsSwitch)
	ON_BN_CLICKED(IDC_SS_STATE_1, &KSceneSettingPageLensFlare::OnBnClickedSsState1)
	ON_BN_CLICKED(IDC_RELOAD, &KSceneSettingPageLensFlare::OnBnClickedReloadShader)
END_MESSAGE_MAP()

void KSceneSettingPageLensFlare::OnBnClickedTexture()
{
    CFileDialog dlg(
        TRUE, "Texture", "LENSTEXT", OFN_OVERWRITEPROMPT,
        "TGA|*.tga|BMP|*.bmp|JPG|*.jpg|DDS|*.dds|"
    );
    
	IEKG3DMeshLensFlare* pFlare = this->GetLensFlareInterface();
	KG_PROCESS_ERROR(pFlare);
    if (dlg.DoModal() == IDOK)
    {
        CString csText = dlg.GetPathName();
        TCHAR tcsRetPath[MAX_PATH];
        g_GetFilePathFromFullPath(tcsRetPath, csText);///这个函数记得检查返回

		///不是很明白这里。如果外部请求的路径是在执行目录下的，则使用之，否则使用相对目录？
        if (csText.Find(g_szDefWorkDirectory) != -1)
            m_csText = tcsRetPath;
        else
            m_csText = csText;
      
        IEKG3DMeshLensFlareDescStruct	TempStruct = *m_pDescStruct;

		_tcscpy_s(TempStruct.m_tczTexName, m_csText);

		TempStruct.m_dwModificationOption = KG3DMeshLensFlare_MDO_TexPath;

		if(SUCCEEDED(pFlare->RequestModification(&TempStruct)))
		{
			if (SUCCEEDED(pFlare->GetDescStruct(&TempStruct)))
			{
				*m_pDescStruct = TempStruct;
			}
		}
    }
    UpdateData(FALSE);
Exit0:
	return;
}

void KSceneSettingPageLensFlare::DrawTex()
{
	DWORD dwID = 0;
	IEKG3DMeshLensFlare* pFlare = this->GetLensFlareInterface();
	KG_PROCESS_ERROR(pFlare);
	if (SUCCEEDED(pFlare->GetTexID(&dwID)))
	{
		CWnd* pWnd = this->GetDlgItem(IDC_VIEW);
		if (pWnd)
		{		

			D3DVIEWPORT9	tempViewPort;

			g_pd3dDevice->GetViewport(&tempViewPort);
			g_pd3dDevice->SetViewport(&m_viewPort);

			g_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1, 0);
			if (SUCCEEDED(g_pd3dDevice->BeginScene()))
			{
				DWORD dwFog = 0;
				g_pd3dDevice->GetRenderState(D3DRS_FOGENABLE, &dwFog);
				g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
				
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				g_pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
				g_pGraphicsTool->DrawScreenRect(&D3DXVECTOR2(0, 0)
						, &D3DXVECTOR2((FLOAT)m_viewPort.Width, (FLOAT)m_viewPort.Height), 0, 1.0f, 1.0f, 0xffffffff, dwID);


				g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0x88ff5555);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				
				g_pGraphicsTool->DrawScreenRect(&D3DXVECTOR2((FLOAT)m_CurSelectRect.left, (FLOAT)m_CurSelectRect.top)
						, &D3DXVECTOR2((FLOAT)m_CurSelectRect.right, (FLOAT)m_CurSelectRect.bottom), 0, 0x88ff5555);
				g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, dwFog);

				g_pd3dDevice->EndScene();

               /* extern g_EngineManager;*/
				g_pd3dDevice->Present(0, 0, pWnd->GetSafeHwnd(), 0);
                /*    g_cEngineManager.CheckDevice();*/
			}

			g_pd3dDevice->SetViewport(&tempViewPort);
		}
	}
Exit0:
	return;

}
void KSceneSettingPageLensFlare::OnTimer( UINT_PTR uIDEvent )
{
	if (uIDEvent == even_id)
	{		
		this->DrawTex();
	}
	CPropertyPage::OnTimer(uIDEvent); 
}
BOOL KSceneSettingPageLensFlare::OnKillActive()
{
	// TODO: Add your specialized code here and/or call the base class
	this->KillTimer(m_uTimmer);
	return CPropertyPage::OnKillActive();
}
BOOL KSceneSettingPageLensFlare::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	LPDIRECT3DSURFACE9 lpSurface = NULL;
	IEKG3DMeshLensFlare* pFlare = this->GetLensFlareInterface();

	KG_PROCESS_ERROR(pFlare);
	{
		g_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &lpSurface);
		KG_PROCESS_ERROR(lpSurface);

		D3DSURFACE_DESC	surfaceDesc;
		if (SUCCEEDED(lpSurface->GetDesc(&surfaceDesc)))
		{
			m_viewPort.Height = surfaceDesc.Height;
			m_viewPort.Width  = surfaceDesc.Width;	
		}
		else
		{
			m_viewPort.Height = 0;
			m_viewPort.Width = 0;
		}
		m_viewPort.X      = 0;
		m_viewPort.Y      = 0;
		m_viewPort.MinZ   = 0;
		m_viewPort.MaxZ   = 1;	
				
		///SetTimer(0, 1, NULL);// Last modify by Chen Tianhong: 2007-7-23 16:50:10
		///现在SceneSetting的整个Dialog都是无模式对话框，不用Timmer了。
		m_uTimmer = this->SetTimer(even_id, 100, NULL);///用于绘制

		/*CSliderCtrl* pSlide = (CSliderCtrl*)this->GetDlgItem(IDC_RATE);
		if (pSlide)
		{
			pSlide->SetPos(0);
		}*/

		SynFromInnerToInterface();
		UpdateTree();	///里面会调用SynFromInnerToInterface
		
	
		SAFE_RELEASE(lpSurface);
		return CPropertyPage::OnSetActive();
	}
Exit0:
	SAFE_RELEASE(lpSurface);
	return CPropertyPage::OnSetActive();
}

BOOL KSceneSettingPageLensFlare::OnInitDialog()
{
    return CDialog::OnInitDialog();
}

void KSceneSettingPageLensFlare::OnLButtonUp(UINT nFlags, CPoint point)
{
    CWnd* wndView = GetDlgItem(IDC_VIEW);
    HTREEITEM hItem = m_tree.GetSelectedItem();

	IEKG3DMeshLensFlare *pFlare = this->GetLensFlareInterface();;
	KG_PROCESS_ERROR(pFlare);

    if (wndView && hItem)
    {
        int n = (int)m_tree.GetItemData(hItem);
        POINT pt = point;
      
        ::ClientToScreen(m_hWnd, &pt);
        ::ScreenToClient(wndView->m_hWnd, &pt);
		CRect rect;
		wndView->GetClientRect(rect);
		if (rect.PtInRect(pt))
		{
			INT nX = pt.x / (rect.Width()/m_pDescStruct->m_nTexSplitNumX);
			INT nY = pt.y / (rect.Height()/m_pDescStruct->m_nTexSplitNumY);

			LensFlareInfo LensInfo;
			HRESULT hr = pFlare->GetLensFlareItem(n, &LensInfo);
			if(SUCCEEDED(hr))
			{
				LensInfo.nIndexX = nX;
				LensInfo.nIndexY = nY;
				if (SUCCEEDED(pFlare->SetLensFlareItem(n, &LensInfo, NULL)))
				{
					UpdateSelectionFrame(nX, nY);
				}
			}
		}
    }
Exit0:
	return;
}

void KSceneSettingPageLensFlare::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);   
    KG_PROCESS_ERROR(pNMTreeView->itemNew.hItem);
    
    this->SynFromInnerToInterface_LensFlareInfo();
Exit0:
    *pResult = 0;
}

BOOL KSceneSettingPageLensFlare::UpdateTree()
{
    HTREEITEM   hHead = NULL;
    HTREEITEM   hItem = NULL;

    TCHAR tszItem[64];

    m_tree.DeleteAllItems();

	size_t uCount = 0;
	IEKG3DMeshLensFlare *pFlare = this->GetLensFlareInterface();
	KG_PROCESS_ERROR(pFlare);
	uCount = (size_t)pFlare->GetLensFlareCount();

	if(m_uWhichItemShouldBeSelected > uCount)
		m_uWhichItemShouldBeSelected = 0;

	HTREEITEM hItemToSelect = NULL;
    for(size_t i  = 0; i < uCount; i++)
    {
        _stprintf_s(tszItem, "光晕_%d", i);
        hItem = m_tree.InsertItem(tszItem);
        m_tree.SetItemData(hItem, (DWORD_PTR)i);

		if(i == m_uWhichItemShouldBeSelected)
			hItemToSelect = hItem;
    }

    if (hItemToSelect && m_bItemSelectionPosChanged)
        m_tree.SelectItem(hItemToSelect);
	else
		m_tree.SelectItem(m_tree.GetFirstVisibleItem());

Exit0:
    return TRUE;
}

int KSceneSettingPageLensFlare::UpdateSelectionFrame(int nX, int nY)
{
    int nResult  = false;
    int nRetCode = false;
	
	{
		{
			KG_PROCESS_ERROR(m_pDescStruct->m_nTexSplitNumX >= 0 && m_pDescStruct->m_nTexSplitNumY >= 0);
			
			m_pLensInfo->nIndexX = nX;//(LONG)(i/nNumX);
			m_pLensInfo->nIndexY = nY;//i - m_CurSelectCell.cy * nNumX;//((i-m_CurSelectCell.cx)/nNumX);

			_ASSERTE(m_pLensInfo->nIndexX >= 0 && m_pLensInfo->nIndexX < m_pDescStruct->m_nTexSplitNumX
				&& m_pLensInfo->nIndexY >= 0 && m_pLensInfo->nIndexY < m_pDescStruct->m_nTexSplitNumY);
			
			INT nCellWidth = m_viewPort.Width / m_pDescStruct->m_nTexSplitNumX;
			INT nCellHeight = m_viewPort.Height / m_pDescStruct->m_nTexSplitNumY;
			
			m_CurSelectRect.left =  nCellWidth * m_pLensInfo->nIndexX;
			m_CurSelectRect.right = m_CurSelectRect.left + nCellWidth;
			m_CurSelectRect.top = nCellHeight * m_pLensInfo->nIndexY;
			m_CurSelectRect.bottom = m_CurSelectRect.top + nCellHeight;
		}	
	}	
    nResult = true;
Exit0:
    return nResult;
}



void KSceneSettingPageLensFlare::OnBnClickedAdd()
{
	IEKG3DMeshLensFlare *pFlare = this->GetLensFlareInterface();;
	KG_PROCESS_ERROR(pFlare);
    if ( pFlare)
    {
        LensFlareInfo LensInfo;
        //initialize this struct
        LensInfo.nIndexX = 0;
        LensInfo.nIndexY = 0;
        LensInfo.fHeight = 1.0f;
        LensInfo.fSize   = 0.05f;
        LensInfo.fRate   = 2;

        size_t uSize = (size_t)pFlare->GetLensFlareCount();
        if (uSize >= 1)
        {
			LensFlareInfo LensInfoTemp;
            pFlare->GetLensFlareItem(uSize - 1, &LensInfoTemp);
            LensInfo = LensInfoTemp;
            LensInfo.fHeight -= 0.1f;
        }
        pFlare->AddLensFlare(LensInfo);
        UpdateTree();
    }
Exit0:
	return;
}

void KSceneSettingPageLensFlare::OnBnClickedDel()
{
    HTREEITEM hTreeItem = m_tree.GetSelectedItem();

	IEKG3DMeshLensFlare *pFlare = this->GetLensFlareInterface();
	KG_PROCESS_ERROR(pFlare);
	if(pFlare->GetLensFlareCount() <= 1)
	{
		::MessageBox(NULL, _T("Lensflare剩1个了，不能删除，要删除整个Lensflare，请到环境面板中删除"), _T("注意"), MB_OK);
		goto Exit0;
	}
    if (hTreeItem)
    {
        size_t uItemDate = (size_t)m_tree.GetItemData(hTreeItem);
        pFlare->DelLensFlare(uItemDate);
        UpdateTree();
    }
Exit0:
	return;
}

void KSceneSettingPageLensFlare::OnBnClickedDefult()
{	
	IEKG3DMeshLensFlare *pFlare = this->GetLensFlareInterface();;
	KG_PROCESS_ERROR(pFlare);
    if (pFlare)
    {
        pFlare->DefaultBuild();
        UpdateTree();
    }
Exit0:
	return;
}

/*
void KSceneSettingPageLensFlare::OnBnClickedSsBnUpdate()
{
	// TODO: Add your control notification handler code here
	this->SynFromInterfaceToInner();
}

void KSceneSettingPageLensFlare::OnBnClickedSsBnCancel()
{
	// TODO: Add your control notification handler code here
}*/


HRESULT KSceneSettingPageLensFlare::SynFromInnerToInterface( DWORD_PTR dwOption /*= 0*/ )
{
	IEKG3DMeshLensFlare* pFlare = this->GetLensFlareInterface();
	KG_PROCESS_ERROR(pFlare);
	
	///处理其它参数
	{		
		HRESULT hr = pFlare->GetDescStruct(KGCommonBufferGetPtr(m_pDescStruct));
		KG_COM_PROCESS_ERROR(hr);

		m_nTexCellNumX = m_pDescStruct->m_nTexSplitNumX;
		m_nTexCellNumY = m_pDescStruct->m_nTexSplitNumY;
		
		m_csText = m_pDescStruct->m_tczTexName;
		m_fVisibility = m_pDescStruct->m_fVisibility;

		m_fBoundAspect = m_pDescStruct->m_fVisibleOutterBoundAspect;
		m_fDummySize = m_pDescStruct->m_fDummyLenSize;

		CWnd* pState= this->GetDlgItem(IDC_SS_STATE_1);
		if (pState)
		{
			KSH::SynACheckWndWithABool(pState, m_pDescStruct->m_dwState & KG3DMeshLensFlare_State_DrawDummyLen, TRUE);
		}
		UpdateData(FALSE);
	}
	///处理选中的LensFlareInfo
	if(! m_bItemSelectionPosChanged)//如果选择的Item没有更改的话就这样跟新的好了，否则，要操作Tree
	{
		SynFromInnerToInterface_LensFlareInfo();
	}
	else
	{
		KSH::CtreeCtrlSelectByOrder(m_tree, (UINT)m_uWhichItemShouldBeSelected);//会触发OnSelectChange，从而调用SynFromInnerToInterface_LensFlareInfo
		m_bItemSelectionPosChanged = false;
	}
	
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KSceneSettingPageLensFlare::SynFromInterfaceToInner( DWORD_PTR dwOption /*= 0*/ )
{
	IEKG3DMeshLensFlare* pFlare = this->GetLensFlareInterface();
	KG_PROCESS_ERROR(pFlare);
	{
		HRESULT hr = E_FAIL;
		CString csTemp = m_csText;
		UpdateData(TRUE);

		///处理选中的LensFlareInfo
		{
			LensFlareInfo LensInfoTemp = *m_pLensInfo;///nIndexX和nIndexY是不在这里改的
			LensInfoTemp.fHeight = m_fHeight;
			LensInfoTemp.fSize = m_fSize;
			{
				/*CSliderCtrl* pSlide = (CSliderCtrl*)this->GetDlgItem(IDC_RATE);
				if (pSlide)
				{
					LensInfoTemp.fRate = ((float)pSlide->GetPos())/10.0f;///默认是0-100的
				}*/
			}

			if (LensInfoTemp != *m_pLensInfo)
			{
				HTREEITEM hItem = m_tree.GetSelectedItem();
				if (hItem)
				{
					size_t uDateCur = (size_t)m_tree.GetItemData(hItem);

					hr = pFlare->SetLensFlareItem(uDateCur, &LensInfoTemp, &m_uWhichItemShouldBeSelected);
#if defined(_DEBUG) | defined(DEBUG)
					if (SUCCEEDED(hr) && SUCCEEDED(pFlare->GetLensFlareItem(m_uWhichItemShouldBeSelected, KGCommonBufferGetPtr(m_pLensInfo))))
					{
						_ASSERTE(LensInfoTemp == *m_pLensInfo);
					}
#endif		
					//因为SetLensFlareItem改了Item高之后，会重新排序的，所以要重新设定选择对应的Item
					if (m_uWhichItemShouldBeSelected != uDateCur)
					{
						m_bItemSelectionPosChanged = true;
					}
				}	
			}
		}

		///处理其它参数
	
		IEKG3DMeshLensFlareDescStruct TempStruct = *m_pDescStruct;
		TempStruct.m_dwModificationOption = 0;
		if (csTemp != m_csText)
		{
			_tcscpy_s(TempStruct.m_tczTexName, m_csText);
			TempStruct.m_dwModificationOption |= KG3DMeshLensFlare_MDO_TexPath;
		}
		if (m_nTexCellNumX != m_pDescStruct->m_nTexSplitNumX || m_nTexCellNumY != m_pDescStruct->m_nTexSplitNumY)
		{
			TempStruct.m_nTexSplitNumX = m_nTexCellNumX;
			TempStruct.m_nTexSplitNumY = m_nTexCellNumY;
			TempStruct.m_dwModificationOption |= KG3DMeshLensFlare_MDO_TexSplitNum;

			//检查LensFlare的各个片的位置有没有越界
			do
			{
				IEKG3DMeshLensFlareDescStruct InnerStruct;
				HRESULT hr = pFlare->GetDescStruct(&InnerStruct);
				_ASSERTE(SUCCEEDED(hr));

				//如果边界限制是变宽的，那么不用检查每个片的位置
				if (InnerStruct.m_nTexSplitNumX <= TempStruct.m_nTexSplitNumX 
					&& InnerStruct.m_nTexSplitNumY <= TempStruct.m_nTexSplitNumY)
				{
					break;
				}

				size_t Size = (size_t)pFlare->GetLensFlareCount();
				bool bIsAnyLensIllegal = false;
				for (size_t i = 0; i < Size; ++i)
				{
					LensFlareInfo TempInfo;
					hr = pFlare->GetLensFlareItem(i, &TempInfo);
					if (SUCCEEDED(hr))
					{
						if (TempInfo.nIndexX >= TempStruct.m_nTexSplitNumX || TempInfo.nIndexY >= TempStruct.m_nTexSplitNumY)
						{
							bIsAnyLensIllegal = true;
							break;
						}
					}
				}

				if(! bIsAnyLensIllegal)
					break;

				INT nRet = MessageBox(_T("你现在请求改小贴图的切分，有的光晕片会因此使用越界的位置，是否自动更正？")
					, NULL, MB_YESNO);

				if (nRet != IDYES)
				{
					MessageBox(_T("更改取消"));
					goto Exit0;
				}

				for (size_t i = 0; i < Size; ++i)
				{
					LensFlareInfo TempInfo;
					hr = pFlare->GetLensFlareItem(i, &TempInfo);
					if (SUCCEEDED(hr))
					{
						if (TempInfo.nIndexX >= m_nTexCellNumX || TempInfo.nIndexY >= m_nTexCellNumY)
						{
							TempInfo.nIndexX = m_nTexCellNumX - 1;
							TempInfo.nIndexY = m_nTexCellNumY - 1;
							pFlare->SetLensFlareItem(i, &TempInfo, NULL);
						}
					}
				}

			}while(false);

		}
		if (m_fVisibility != m_pDescStruct->m_fVisibility)
		{
			TempStruct.m_fVisibility = m_fVisibility;
			TempStruct.m_dwModificationOption |= KG3DMeshLensFlare_MDO_Visibility;
		}
		if (m_fBoundAspect != m_pDescStruct->m_fVisibleOutterBoundAspect)
		{
			TempStruct.m_fVisibleOutterBoundAspect = m_fBoundAspect;
			TempStruct.m_dwModificationOption |= KG3DMeshLensFlare_MDO_BoundAspect;
		}
		if (m_fDummySize != m_pDescStruct->m_fDummyLenSize)
		{
			TempStruct.m_fDummyLenSize = m_fDummySize;
			TempStruct.m_dwModificationOption |= KG3DMeshLensFlare_MDO_DummyLenSize;
		}
		KG_COM_PROCESS_ERROR(pFlare->RequestModification(&TempStruct));
		
	}
	return S_OK;
Exit0:
	
	return E_FAIL;
}

void KSceneSettingPageLensFlare::OnBnClickedSsSwitch()
{
	// TODO: Add your control notification handler code here
	static LPCTSTR TitleArray[] = {
		_T("太阳光晕")
		, _T("月亮光晕")
	};
	size_t uWitchTitle = 0;
	if (m_nWhichLensFlare == KG3DTYPE_ENVEFF_LENSFLARE_SUN)
	{
		m_nWhichLensFlare = KG3DTYPE_ENVEFF_LENSFLARE_MOON;
		if (this->GetLensFlareInterface())
		{
			uWitchTitle = 1;
		}
		else
		{
			m_nWhichLensFlare  = KG3DTYPE_ENVEFF_LENSFLARE_SUN;
			MessageBox("月亮光晕不存在");
			goto Exit0;
		}
	}
	else
	{
		m_nWhichLensFlare = KG3DTYPE_ENVEFF_LENSFLARE_SUN;
		if (this->GetLensFlareInterface())
		{
			uWitchTitle = 0;
		}
		else
		{
			MessageBox("太阳光晕不存在");
			m_nWhichLensFlare = KG3DTYPE_ENVEFF_LENSFLARE_SUN;
			goto Exit0;
		}
	}
	{
		CPropertySheet* pSheet = dynamic_cast<CPropertySheet*>(this->GetParent());
		_ASSERTE(pSheet);
		CTabCtrl* pTab = pSheet->GetTabControl();
		_ASSERTE(pTab);
		TC_ITEM ti;
		ti.mask = TCIF_TEXT;
		ti.pszText = const_cast<LPTSTR>(TitleArray[uWitchTitle]);
		pTab->SetItem (pSheet->GetPageIndex(this), &ti);
	}
	SynFromInnerToInterface();
	this->UpdateTree();
Exit0:
	return;
}

void KSceneSettingPageLensFlare::OnBnClickedSsState1()
{
	// TODO: Add your control notification handler code here
	IEKG3DMeshLensFlare* pFlare = this->GetLensFlareInterface();
	if(! pFlare)
		return;

	CWnd* pState = this->GetDlgItem(IDC_SS_STATE_1);
	if (!pState)
		return;
	
	BOOL bCheck = KSH::SynACheckWndWithABool(pState, TRUE, FALSE);
	IEKG3DMeshLensFlareDescStruct TempStruct;
	TempStruct.m_dwState = m_pDescStruct->m_dwState;
	if(bCheck)
		TempStruct.m_dwState |= KG3DMeshLensFlare_State_DrawDummyLen;
	else
		TempStruct.m_dwState &= (~KG3DMeshLensFlare_State_DrawDummyLen);
	TempStruct.m_dwModificationOption = KG3DMeshLensFlare_MDO_State;

	pFlare->RequestModification(&TempStruct);
	pFlare->GetDescStruct(&TempStruct);
	m_pDescStruct->m_dwState = TempStruct.m_dwState;
	KSH::SynACheckWndWithABool(pState, TempStruct.m_dwState & KG3DMeshLensFlare_State_DrawDummyLen, TRUE);
}

void KSceneSettingPageLensFlare::OnBnClickedReloadShader()
{
	IEKG3DMeshLensFlare* p = this->GetLensFlareInterface();
	if(NULL != p)
		p->ReloadShader();
}

void KSceneSettingPageLensFlare::SynFromInnerToInterface_LensFlareInfo()
{
	IEKG3DMeshLensFlare* pFlare = this->GetLensFlareInterface();
	KG_PROCESS_ERROR(pFlare);
	HTREEITEM hItem = m_tree.GetSelectedItem();

	if (hItem)
	{
		LensFlareInfo LensInfo;
		size_t uDateCur = (size_t)m_tree.GetItemData(hItem);
		if (SUCCEEDED(pFlare->GetLensFlareItem(uDateCur, &LensInfo)))
		{
			UpdateSelectionFrame(LensInfo.nIndexX, LensInfo.nIndexY);
			m_fHeight = LensInfo.fHeight;
			m_fSize   = LensInfo.fSize;
			/*int nRate     = (int)(LensInfo.fRate * 10);
			((CSliderCtrl*)(GetDlgItem(IDC_RATE)))->SetPos(nRate);*/
			*m_pLensInfo = LensInfo;
		}		
	}
	UpdateData(FALSE);
Exit0:
	return;
}