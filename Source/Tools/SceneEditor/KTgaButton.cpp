#include "StdAfx.h"
#include "KTgaButton.h"
#include "resource.h"

#include "KSceneSceneEditorDialogGroundTextureUV.h"
#include "KStaticBitmapSectionSelector.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern bool				g_IsLoadDialogInfo;
KTgaButton				*g_pTgaButton = NULL;
extern TCHAR g_szDefWorkDirectory[256];

KTgaButton::KTgaButton(void)
{
	m_nLayer = -1;
	m_bIsHaveTexture = false;
	m_nTextureIndex = -1;
	m_bIsSelected = false;
    m_byTexCoordIndex = 0;
	m_pDialogTextureUV = NULL;
	m_lpStaticSection = NULL;
}

KTgaButton::~KTgaButton(void)
{
	g_pTgaButton = NULL;
	SAFE_DELETE(m_pDialogTextureUV);
}

BYTE *KTgaButton::SacleData(const RECT &rect,const byte *SrcData,const int &BytePerPixel)
{
	int height = abs(rect.bottom - rect.top);
	int width = abs(rect.left - rect.right);

	const float sx = (float)m_TgaInfo.Header.width / width;
	const float sy = (float)m_TgaInfo.Header.height / height;

	const int ix = (int)sx;
	const int iy = (int)sy;
	const float spx = sx - ix;
	const float spy = sy - iy;
	int i,j;
	DWORD sumx;//,sumz;
	float sumspx = 0.0f,sumspz = 0.0f;
	byte bv0,bv1,bv2,bv3;
	//	float fv;
	BYTE* pSData = (BYTE*)SrcData;
	BYTE *NewData = new BYTE[width * height * BytePerPixel];
	BYTE *pDData = NewData;
	DWORD dwDstPitch = m_TgaInfo.Header.width * BytePerPixel;
	DWORD dwDstPitch2 = width * BytePerPixel;

	for(i = 0; i < height; ++i)
	{
		sumx = 0;
		for(j = 0;j < width; ++j)
		{
			assert(pSData <=  SrcData + m_TgaInfo.Header.width * m_TgaInfo.Header.height * BytePerPixel);
			assert(pDData <= width * height * BytePerPixel + NewData);
			if(sumspx >= 1.0f)
			{
				sumx += (ix + 1) * BytePerPixel;
				sumspx -= 1.0f;
			}
			else
				sumx += ix * BytePerPixel;

			bv0 = pSData[sumx + 0];
			bv1 = pSData[sumx + 1];
			bv2 = pSData[sumx + 2];
			bv3 = pSData[sumx + 3];

			pDData[j * BytePerPixel + 0] = bv0;
			pDData[j * BytePerPixel + 1] = bv1;
			pDData[j * BytePerPixel + 2] = bv2;
			pDData[j * BytePerPixel + 3] = bv3;

			sumspx += spx;
		}

		if(sumspz >= 1.0f)
		{
			pSData += dwDstPitch * (iy + 1);
			sumspz -= 1.0f;
		}
		else
			pSData += dwDstPitch * iy;

		sumspz += spy;

		pDData += dwDstPitch2;
	}


	return NewData;
}

HRESULT KTgaButton::LoadTgaFile(const char* strFileName,BOOL bAddToTerrian)
{
	if (!strFileName)
		return E_FAIL;
	
	bool isload = false;
	std::string dir = strFileName;
	std::list<std::string>::iterator its,listend;
	std::string *psting = NULL;

	//FILE* fpFile = fopen(strFileName, "rb");
	 IFile* fpFile = g_OpenFile(strFileName);
	if (!fpFile)//打开默认的贴图文件
	{
		TCHAR NameDefault[MAX_PATH];
		wsprintf(NameDefault,"%s\\Data\\Editor\\Texture\\replace.tga",g_szDefWorkDirectory);

		fpFile = g_OpenFile(NameDefault);//fpFile = fopen(NameDefault, "rb");//
		if(!fpFile)
			return E_FAIL;
	}
	

	fpFile->Read(&m_TgaInfo.Header, sizeof(TGAHeader));//fread(&m_TgaInfo.Header, sizeof(TGAHeader), 1, fpFile);//
	const int byteperpixel = m_TgaInfo.Header.bitsperpixel / 8;
	if(byteperpixel != 4)
	{
		MessageBox("Tga格式不正确，必须为32位");
		SAFE_RELEASE(fpFile);//fclose(fpFile);//
		return E_FAIL;
	}

	m_TextureDir = dir;

	//TCHAR drive[256];
	//TCHAR dirc[256];
	//TCHAR fname[256];
	//TCHAR ext[256];
	std::string filename;

	if(m_pTerrainEx && m_nButtonType == GROUNDTEXTURE)
	{
		int nCoord =0;
		if(SUCCEEDED(m_pTerrainEx->GetGroundTextureCoordIndex(m_nLayer,m_nTextureIndex,&nCoord)))
		{
			SetTexCoordIndex(nCoord);
		}
	}

	size_t Size = m_TgaInfo.Header.height * m_TgaInfo.Header.width * byteperpixel;
	SAFE_DELETE_ARRAY(m_TgaInfo.pBuffer);
	m_TgaInfo.pBuffer = new BYTE[Size];
	fpFile->Seek(m_TgaInfo.Header.Id + sizeof(TGAHeader), SEEK_SET);//fseek(fpFile, m_TgaInfo.Header.Id + sizeof(TGAHeader), SEEK_SET);//
	fpFile->Read(m_TgaInfo.pBuffer, (unsigned long)Size);//fread(m_TgaInfo.pBuffer, Size, 1, fpFile);//

	RECT rect;
	GetClientRect(&rect);
	int height = abs(rect.bottom - rect.top);
	int width = abs(rect.left - rect.right);

	BYTE *NewData = SacleData(rect,m_TgaInfo.pBuffer,byteperpixel);
	CDC* pDc = this->GetDC();
	m_Bitmap.CreateCompatibleBitmap(pDc, width, height);
	m_Bitmap.SetBitmapBits(width * height * byteperpixel,NewData);

	SetBitmap(m_Bitmap);
	ReleaseDC(pDc);
	SAFE_DELETE_ARRAY(NewData);

	//fclose(fpFile);
	SAFE_RELEASE(fpFile);
	m_bIsHaveTexture = true;

	return S_OK;
}

bool KTgaButton::InitButton(const int &Layer,const int &ButtonType)
{
	m_MenuTga.LoadMenu(IDR_MENUTGA);
	m_nLayer = Layer;
	m_nButtonType = ButtonType;

    m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XZ,MF_CHECKED);

	return true;
}

BEGIN_MESSAGE_MAP(KTgaButton, CButton)
	ON_WM_PAINT()
	ON_WM_RBUTTONUP()
    ON_COMMAND(ID_ASSGRASS, OnAssociateGrass)
	ON_COMMAND(ID_LoadTexture, &KTgaButton::OnLoadtexture)
	ON_COMMAND(ID_DeleteTexture, &KTgaButton::OnDeletetexture)
	ON_COMMAND(ID_ChangeTexture, &KTgaButton::OnChangetexture)

    ON_COMMAND(ID_SELECTTEXCOORD_XZ,&KTgaButton::OnSelectTexCoordXZ)
    ON_COMMAND(ID_SELECTTEXCOORD_XY, &KTgaButton::OnSelectTexCoordXY)
    ON_COMMAND(ID_SELECTTEXCOORD_YZ, &KTgaButton::OnSelectTexCoordYZ)

	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_SetTexCoord, &KTgaButton::OnSettexcoord)
END_MESSAGE_MAP()



void KTgaButton::OnAssociateGrass()
{
	if(m_pOutDoorMgr)
		return ;
    /*static KGAssociateGrassDlg DlgAssGrass;
    DlgAssGrass.AttachScene(m_lpSceneEditor, m_pTerrainEx, m_nLayer, m_nTextureIndex);
    
    if (!IsWindow(DlgAssGrass.m_hWnd))
        DlgAssGrass.Create(IDD_ASSOCIATE_GRASS, this);
    
    if (!m_Bitmap.m_hObject)
        DlgAssGrass.ShowWindow(SW_HIDE);
    else
        DlgAssGrass.ShowWindow(SW_SHOW);

    CString strText;
    strText.Format(TEXT("Layer : %d TextureIndex : %d :)"), m_nLayer, m_nTextureIndex);
    DlgAssGrass.SetWindowText(strText);*/
}


void KTgaButton::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(m_pOutDoorMgr && m_nButtonType == GROUNDTEXTURE)
		return ;
	CButton::OnRButtonUp(nFlags, point);

	POINT point1;
	GetCursorPos(&point1);
	CMenu* pMenu = m_MenuTga.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point1.x, point1.y, this);
	CButton::OnRButtonUp(nFlags, point);


}

void KTgaButton::OnLoadtexture()
{
	// TODO: Add your command handler code here

	if(m_bIsHaveTexture)
		return;

	char bigBuff[800] = "";
	char szFilter[] ="Texture (*.tga)|*.tga|";
	TCHAR FileDir[256];
	std::string DiablogPath;

	CFileDialog dlg(TRUE, NULL, NULL,
		OFN_HIDEREADONLY , szFilter);

	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);

	if(m_nButtonType == GROUNDTEXTURE)
	{
		DiablogPath = g_szDefWorkDirectory;
		DiablogPath += "Data\\source\\maps_source\\Texture\\地表贴图";
		memcpy(FileDir,DiablogPath.c_str(),DiablogPath.length());
		FileDir[DiablogPath.size()] = '\0';
		dlg.m_ofn.lpstrInitialDir = FileDir;
	}
	else
	{
		DiablogPath = g_szDefWorkDirectory;
		DiablogPath += "Data\\editor\\brush\\笔刷";
		memcpy(FileDir,DiablogPath.c_str(),DiablogPath.length());
		FileDir[DiablogPath.size()] = '\0';
		dlg.m_ofn.lpstrInitialDir = FileDir;
	}

	if (	dlg.DoModal() == IDOK)
	{
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		CString Vs = dlg.GetPathName();

		wsprintf(path_buffer,"%s",Vs);
		_splitpath( path_buffer, drive, dir, fname, ext );


		wsprintf(FileDir,"%s%s%s.tga",drive,dir,fname);

		if(m_nButtonType == GROUNDTEXTURE)
		{
			if(m_pTerrainEx)
				m_pTerrainEx->SetGroundTexture(m_nLayer,m_nTextureIndex,FileDir);
			if (m_pOutDoorMgr)
			{
				m_pOutDoorMgr->ChangeDetailTexture(0,FileDir);//改变最底层地表贴图
			}

		}

		LoadTgaFile(FileDir,TRUE);

		UpdateData(false);
	}
}

void KTgaButton::OnDeletetexture()
{
	// TODO: Add your command handler code here

    if(MessageBox("Click Yes to delete texture",NULL,
        MB_YESNO | MB_ICONQUESTION) == IDNO)
        return;

	if(/*m_nLayer == 0 || */!m_bIsHaveTexture)
		return;

	std::list<std::string>::iterator its,listend ;//= m_staticGroundTextureLoadTextureList.begin();
	std::string *psting = NULL;
	//size_t s1,s2;
	//s1 = m_TextureDir.size();
	bool isremove = false;

	if(m_pTerrainEx)
	{
		if(m_Bitmap.DeleteObject())
		{
			m_bIsHaveTexture = false;

			if(m_nButtonType == GROUNDTEXTURE)
			{
				m_pTerrainEx->SetTgaButtonTextureDic(true,false,m_TextureDir.c_str(),m_nLayer);
				m_pTerrainEx->DeleteGroundTexture(m_nLayer,m_nTextureIndex);
				m_pTerrainEx->ClearGroundTextureByPath(m_TextureDir.c_str(),m_nLayer);
				m_pTerrainEx->SetCurrentPaintTextureId(-1);
			}
			else
				m_pTerrainEx->SetTgaButtonTextureDic(false,false,m_TextureDir.c_str(),m_nLayer);

			m_TextureDir = " ";
		}

	}
	if (m_pOutDoorMgr)
	{
		if (m_nButtonType == BRUSHMAP)
		{
			if(m_Bitmap.DeleteObject())
			{
				m_bIsHaveTexture = false; 
				m_TextureDir = "";
			}
		}
		
	}
	Invalidate();

	return ;
}

HRESULT KTgaButton::ChangeTagFile(const char *strFileName)
{
	if (!strFileName)
		return E_FAIL;

	/*if(m_nButtonType != GROUNDTEXTURE)
		return S_FALSE;*/

	bool isload = false;
	std::string dir = strFileName;
	std::list<std::string>::iterator its,listend ;//= m_staticGroundTextureLoadTextureList.begin();
	std::string *psting = NULL;
	//size_t s1,s2;
	//s1 = dir.size();

	FILE* fpFile = fopen(strFileName, "rb");
	if (!fpFile)//打开默认的贴图文件
	{
		TCHAR NameDefault[MAX_PATH];
		wsprintf(NameDefault,"%s\\Data\\Editor\\Texture\\replace.tga",g_szDefWorkDirectory);

		fpFile = fopen(NameDefault, "rb");
		if(!fpFile)
			return E_FAIL;
	}

	fread(&m_TgaInfo.Header, sizeof(TGAHeader), 1, fpFile);
	const int byteperpixel = m_TgaInfo.Header.bitsperpixel / 8 ;
	if(byteperpixel != 4)
	{
		MessageBox("Tga格式不正确，必须为32位");
		fclose(fpFile);
		return S_FALSE;
	}

	if(m_pTerrainEx)
	{
		m_pTerrainEx->SetPaintGroundState();
		//m_lpSceneEditor->SetToEditorTerrainState();
		m_lpSceneEditor->SetEditState(SCENESTATE_TERRAIN);

		m_TextureDir = dir;

		if(m_nButtonType == GROUNDTEXTURE)
		{
			m_pTerrainEx->SetTextureOpertaorLayter(m_nLayer);
			m_pTerrainEx->SetBrushState(0);

			//m_pTerrainEx->SetCurrentBrushMap(dir.c_str());
			m_pTerrainEx->SetTextureOpertaorLayter(m_nLayer);
			m_pTerrainEx->SetCurrentPaintTextureId(m_nTextureIndex);

		}
		else
		{
			m_pTerrainEx->SetTgaButtonTextureDic(false,true,dir.c_str(),m_nLayer);
			m_pTerrainEx->SetTgaButtonTextureDic(false,false,m_TextureDir.c_str(),m_nLayer);

			m_pTerrainEx->SetCurrentBrushMap(m_TextureDir.c_str());// m_Brush.strCurrentBrushMap = m_TextureDir;
			m_pTerrainEx->ChangeBrushMap(m_nTextureIndex,m_TextureDir.c_str());
		}

	}
	if(m_pOutDoorMgr && m_lpSceneEditor)//add by huangjinshou 2008 -6-5,新地形格式用
	{
		m_lpSceneEditor->SetEditState(SCENESTATE_TERRAIN);
		m_lpSceneEditor->SetTBModifierState(MAP_PAINT_GROUND);
		m_TextureDir = dir;
		if(m_nButtonType == GROUNDTEXTURE)
		{
			m_pOutDoorMgr->ChangeDetailTexture(0,m_TextureDir.c_str());//改变最底层地表贴图
			m_lpSceneEditor->SetPaintTexIndex(0);
			if(m_lpStaticSection)
			{
				m_lpStaticSection->LoadTga(m_TextureDir.c_str(),FALSE);
				m_lpStaticSection->UpdateControl();
			}
		}		
	}
	std::string NewPath = dir;

	//TCHAR drive[256];
	//TCHAR dirc[256];
	//TCHAR fname[256];
	//TCHAR ext[256];
	std::string filename;
	int newid = 0;

	/*if(m_nButtonType == GROUNDTEXTURE)
	{
		m_pTerrainEx->ChangGroundTextureLayer(m_TextureDir,NewPath,newid,m_nLayer);
		m_TextureDir = NewPath;

		m_pTerrainEx->m_lpGroundTextureManager->FindTextureInLayer(m_nLayer,m_nTextureIndex,m_TextureDir.c_str());
		m_pTerrainEx->SetCurrentPaintTextureId(m_nTextureIndex);
	}*/


	/*FILE* fpFile = fopen(strFileName, "rb");
	if (!fpFile)
		return E_FAIL;

	fread(&m_TgaInfo.Header, sizeof(TGAHeader), 1, fpFile);

	const int byteperpixel = m_TgaInfo.Header.bitsperpixel / 8 ;*/
	size_t Size = m_TgaInfo.Header.width * m_TgaInfo.Header.height * byteperpixel;
	SAFE_DELETE_ARRAY(m_TgaInfo.pBuffer);
	m_TgaInfo.pBuffer = new BYTE[Size];
	fseek(fpFile, m_TgaInfo.Header.Id + sizeof(TGAHeader), SEEK_SET);
	fread(m_TgaInfo.pBuffer, Size, 1, fpFile);

	RECT rect;
	GetClientRect(&rect);
	int height = abs(rect.bottom - rect.top);
	int width = abs(rect.left - rect.right);

	m_Bitmap.DeleteObject();

	BYTE *NewData = SacleData(rect,m_TgaInfo.pBuffer,byteperpixel);
	CDC* pDc = this->GetDC();
	m_Bitmap.CreateCompatibleBitmap(pDc, width, height);
	m_Bitmap.SetBitmapBits(width * height * byteperpixel,NewData);

	SetBitmap(m_Bitmap);
	ReleaseDC(pDc);
	SAFE_DELETE_ARRAY(NewData);

	fclose(fpFile);

	m_bIsHaveTexture = true;

	return S_OK;
}

void KTgaButton::OnChangetexture()
{
	// TODO: Add your command handler code here

	if(!m_bIsHaveTexture)
		return;

	char bigBuff[800] = "";
	char szFilter[] ="Texture (*.tga)|*.tga|";
	TCHAR FileDir[256];
	std::string DiablogPath;

	CFileDialog dlg(TRUE, ".tga", "temp.tga",
		OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT , szFilter);


	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);

	if(m_nButtonType == GROUNDTEXTURE)
	{
		DiablogPath = g_szDefWorkDirectory;
		DiablogPath += "Data\\source\\maps_source\\Texture\\地表贴图";
		memcpy(FileDir,DiablogPath.c_str(),DiablogPath.length());
		FileDir[DiablogPath.size()] = '\0';
		dlg.m_ofn.lpstrInitialDir = FileDir;
	}
	else
	{
		DiablogPath = g_szDefWorkDirectory;
		DiablogPath += "Data\\editor\\brush\\笔刷";
		memcpy(FileDir,DiablogPath.c_str(),DiablogPath.length());
		FileDir[DiablogPath.size()] = '\0';
		dlg.m_ofn.lpstrInitialDir = FileDir;
	}


	if (	dlg.DoModal() == IDOK)
	{
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		CString Vs = dlg.GetPathName();

		wsprintf(path_buffer,"%s",Vs);
		_splitpath( path_buffer, drive, dir, fname, ext );


		wsprintf(FileDir,"%s%s%s.tga",drive,dir,fname);

		if(m_pTerrainEx)
		{
			if(m_nButtonType == GROUNDTEXTURE)
			{
				m_pTerrainEx->SetGroundTexture(m_nLayer,m_nTextureIndex,FileDir);

				if(m_lpStaticSection)
				{
					m_lpStaticSection->LoadTga(m_TextureDir.c_str(),FALSE);
					m_lpStaticSection->UpdateControl();
				}
			}
		}


		ChangeTagFile(FileDir);

		UpdateData(false);
	}

}

void KTgaButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CButton::OnLButtonUp(nFlags, point);

	if(!m_bIsHaveTexture)
		return;
	if(m_nButtonType == GROUNDTEXTURE)
	{
		if(g_pTgaButton)
		{
			g_pTgaButton->SetIsSelected(false);
			g_pTgaButton->Invalidate();
			m_bIsSelected = true;
			g_pTgaButton = this;
		}
		else
		{
			m_bIsSelected = true;
			g_pTgaButton = this;
		}
	}
	if (m_pOutDoorMgr && m_lpSceneEditor)
	{
		m_lpSceneEditor->SetEditState(SCENESTATE_TERRAIN);
		m_lpSceneEditor->SetTBModifierState(MAP_PAINT_GROUND);
		if (m_nButtonType == GROUNDTEXTURE)
		{
			m_lpSceneEditor->SetPaintTexIndex(0);
			if(m_lpStaticSection)
			{
				m_lpStaticSection->LoadTga(m_TextureDir.c_str(),FALSE);
				m_lpStaticSection->UpdateControl();
			}
		}
		else
		{
			m_lpSceneEditor->SetCurrentBrushMap(m_TextureDir.c_str());
		}
	}
	if(m_pTerrainEx)
	{
		m_pTerrainEx->SetPaintGroundState();
		//m_lpSceneEditor->SetToEditorTerrainState();
		m_lpSceneEditor->SetEditState(SCENESTATE_TERRAIN);

		if(m_nButtonType == GROUNDTEXTURE)
		{
			m_pTerrainEx->SetTextureOpertaorLayter(m_nLayer);
			m_pTerrainEx->SetBrushState(0);

			m_pTerrainEx->SetPaintCurrentTextureId(m_nLayer,m_nTextureIndex);
			m_pTerrainEx->SetCurrentGroundTexture(m_TextureDir.c_str());

			if(m_lpStaticSection)
			{
				m_lpStaticSection->LoadTga(m_TextureDir.c_str(),FALSE);
				m_lpStaticSection->UpdateControl();
			}
		}
		else
		{
			m_pTerrainEx->SetCurrentBrushMap(m_TextureDir.c_str());
			//m_pTerrainEx->setcu

			//m_pTerrainEx->m_Brush.strCurrentBrushMap = m_TextureDir;
			//m_pTerrainEx->m_Brush.ChangeBrushMap(m_TextureDir.c_str(),*m_pTerrainEx);
		}
	}
	
	Invalidate();
	UpdateData(false);
}

void KTgaButton::OnPaint()
{
	CButton::OnPaint();

	if (m_bIsSelected)
	{
		CRect rect;
		GetClientRect(&rect);
		rect.left  += 2;
		rect.right -= 2;
		rect.top += 2;
		rect.bottom -= 2;
		CClientDC dc(this);
		//dc.FillRect(&rect, &CBrush(RGB(200,0,0)));

		CPen pen;

        if(m_byTexCoordIndex == 0)
		    pen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
        else if(m_byTexCoordIndex == 1)
            pen.CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
        else
            pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 255));

		dc.SelectObject(&pen);

		dc.MoveTo(rect.left - 1, rect.top);
		dc.LineTo(rect.right, rect.top);
		dc.LineTo(rect.right, rect.bottom);
		dc.LineTo(rect.left, rect.bottom);
		dc.LineTo(rect.left, rect.top);

		UpdateData(false);
	}
}

void KTgaButton::SetTexCoordIndex(int &Index)
{
    if(m_byTexCoordIndex != Index)
    {
        if(m_byTexCoordIndex == 0)
            m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XZ, MF_UNCHECKED);
        else if(m_byTexCoordIndex == 1)
            m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XY, MF_UNCHECKED);
        else
            m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_YZ, MF_UNCHECKED);

        if(Index == 0)
            m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XZ, MF_CHECKED);
        else if(Index == 1)
            m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XY, MF_CHECKED);
        else
            m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_YZ, MF_CHECKED);

        m_byTexCoordIndex = (byte)Index;
    }
}

void KTgaButton::OnSelectTexCoordXZ()
{
    if(!m_bIsHaveTexture)
        return;

    m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XZ, MF_CHECKED);

    if(m_byTexCoordIndex == 1)
        m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XY, MF_UNCHECKED);
    else if(m_byTexCoordIndex == 2)
        m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_YZ, MF_UNCHECKED);

    m_byTexCoordIndex = 0;
	if(m_pTerrainEx)
		 m_pTerrainEx->SetGroundTextureCoordIndex(m_nLayer,m_nTextureIndex,m_byTexCoordIndex);
	if(m_pOutDoorMgr)
		m_pOutDoorMgr->SetDetailTextureCoordIndex(0,m_byTexCoordIndex);
}

void KTgaButton::OnSelectTexCoordXY()
{
    if(!m_bIsHaveTexture)
        return;

    m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XY, MF_CHECKED);

    if(m_byTexCoordIndex == 0)
        m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XZ, MF_UNCHECKED);
    else if(m_byTexCoordIndex == 2)
        m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_YZ, MF_UNCHECKED);

    m_byTexCoordIndex = 1;
    //m_pTerrainEx->SetGroundTextureTexCoordIndex(m_TextureDir,m_byTexCoordIndex,m_nLayer);
	if(m_pTerrainEx)
		m_pTerrainEx->SetGroundTextureCoordIndex(m_nLayer,m_nTextureIndex,m_byTexCoordIndex);
	if(m_pOutDoorMgr)
		m_pOutDoorMgr->SetDetailTextureCoordIndex(0,m_byTexCoordIndex);
}

void KTgaButton::OnSelectTexCoordYZ()
{
    if(!m_bIsHaveTexture)
        return;

    m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_YZ, MF_CHECKED);

    if(m_byTexCoordIndex == 0)
        m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XZ, MF_UNCHECKED);
    else if(m_byTexCoordIndex == 1)
        m_MenuTga.CheckMenuItem(ID_SELECTTEXCOORD_XY, MF_UNCHECKED);

    m_byTexCoordIndex = 2;
    //m_pTerrainEx->SetGroundTextureTexCoordIndex(m_TextureDir,m_byTexCoordIndex,m_nLayer);
	if(m_pTerrainEx)
		m_pTerrainEx->SetGroundTextureCoordIndex(m_nLayer,m_nTextureIndex,m_byTexCoordIndex);
	if(m_pOutDoorMgr)
		m_pOutDoorMgr->SetDetailTextureCoordIndex(0,m_byTexCoordIndex);
	

}

void KTgaButton::OnSettexcoord()
{
	//if(m_pOutDoorMgr)
	//	return ;
	// TODO: Add your command handler code here
	if(!m_pDialogTextureUV)
	{
		m_pDialogTextureUV = new KSceneSceneEditorDialogGroundTextureUV;
		m_pDialogTextureUV->Create(KSceneSceneEditorDialogGroundTextureUV::IDD,NULL);
	}

	m_pDialogTextureUV->SetGroundTexture(m_pTerrainEx,m_nLayer,m_nTextureIndex);
	m_pDialogTextureUV->ShowWindow(TRUE);
	m_pDialogTextureUV->CenterWindow();
}
