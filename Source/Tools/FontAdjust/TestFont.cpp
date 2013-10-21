#include "stdafx.h"
#include "Resource.h"
#include "./font.h"
#include "./fontrenderer.h"
#include "./FontAdjust.h"


//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Our rendering device

LPDIRECT3DVERTEXBUFFER9 pVS = NULL;

FontRenderer*		g_pFontRenderer = NULL;

HWND g_hControlWnd = NULL;

int g_nFontColorRGB[3] = {255, 255, 255};
int g_nBackColorRGB[3] = {0, 0, 0};
int g_nUseBackTexture = false;

LPDIRECT3DTEXTURE9 g_pTextureBack = NULL;

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D(HWND hWnd)
{
    // Create the D3D object.
    if(NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return E_FAIL;

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    // Create the D3DDevice
    if(FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice)))
    {
        return E_FAIL;
    }

    // Turn off culling, so we see the front and back of the triangle
    g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    // Turn off D3D lighting, since we are providing our own vertex colors
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry()
// Desc: Creates the scene geometry
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	ATLTRACE("sizeof(Font)\t%d\n", sizeof(Font));
	ATLTRACE("sizeof(Font)\t%d\n", sizeof(FontRenderer));

	g_pFontRenderer = new FontRenderer;
	g_pFontRenderer->Initialize();

	return S_OK;
}


VOID Cleanup()
{
	g_pFontRenderer->CleanUp();
	delete g_pFontRenderer;


    if(g_pd3dDevice != NULL)
        g_pd3dDevice->Release();

    if(g_pD3D != NULL)
        g_pD3D->Release();
}



//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    // For our world matrix, we will just rotate the object about the y-axis.
    D3DXMATRIXA16 matWorld;

    // Set up the rotation matrix to generate 1 full rotation (2*PI radians) 
    // every 1000 ms. To avoid the loss of precision inherent in very high 
    // floating point numbers, the system time is modulated by the rotation 
    // period before conversion to a radian angle.
    UINT  iTime  = timeGetTime() % 1000;
    FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
    D3DXMatrixRotationY(&matWorld, fAngle);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXVECTOR3 vEyePt(0.0f, 3.0f,-5.0f);
    D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
    g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f);
    g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}


DWORD g_dwFPS = 0;

void UpdateFPS()
{
	static DWORD s_dwFPS = 0;
	static DWORD s_dwLastTime = 0;

	DWORD dwCurrentTime = ::timeGetTime();

	++s_dwFPS;

	if (dwCurrentTime - s_dwLastTime > 1000)
	{
		s_dwLastTime = dwCurrentTime;
		g_dwFPS = s_dwFPS;
		s_dwFPS = 0;
	}
}


VOID Render(HWND hWnd)
{
	UpdateFPS();

	static TCHAR szLast[256];
	TCHAR szText[256];
	_sntprintf(szText, _countof(szText), _T("文件:%s,字体号:%u, DPI: %u, value1:%.4f, value2:%.4f, Use:%d."), 
		g_FontAjust.GetFontFile(), g_FontAjust.GetFontSize(), g_FontAjust.GetDpi(), 
		g_FontAjust.GetControl1(), g_FontAjust.GetControl2(), g_FontAjust.IsSharp());
	if (_tcscmp(szLast, szText) != 0)
	{
		::SetWindowText(hWnd, szText);
		_tcscpy(szLast, szText);
	}
	

    g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(g_nBackColorRGB[0],g_nBackColorRGB[1],g_nBackColorRGB[2]), 1.0f, 0);

    if (SUCCEEDED(g_pd3dDevice->BeginScene()))
    {

		if (g_nUseBackTexture && g_pTextureBack)
		{
			struct _VS
			{
				D3DXVECTOR4 p;
				float u;
				float v;
			};

			RECT rc;
			GetClientRect(hWnd, &rc);

			if (!pVS)
			{
				g_pd3dDevice->CreateVertexBuffer(
					6 * sizeof(_VS),
					0,
					D3DFVF_XYZRHW | D3DFVF_TEX1,
					D3DPOOL_MANAGED,
					&pVS,
					NULL
					);
				_VS* data = NULL;
				pVS->Lock(0, 0, (void**)&data, 0);

				data[0].p = D3DXVECTOR4(0.f, 0.f, 0.f, 1.f);
				data[0].u = 0.f;
				data[0].v = 0.f;
				data[1].p = D3DXVECTOR4((float)(rc.right), 0.f, 0.f, 1.f);
				data[1].u = 1.f;
				data[1].v = 0.f;
				data[2].p = D3DXVECTOR4((float)(rc.right), (float)(rc.bottom), 0.f, 1.f);
				data[2].u = 1.f;
				data[2].v = 1.f;

				data[3].p = D3DXVECTOR4(0.f, 0.f, 0.f, 1.f);
				data[3].u = 0.f;
				data[3].v = 0.f;
				data[4].p = D3DXVECTOR4((float)(rc.right), (float)(rc.bottom), 0.f, 1.f);
				data[4].u = 1.f;
				data[4].v = 1.f;
				data[5].p = D3DXVECTOR4(0.f, (float)(rc.bottom), 0.f, 1.f);
				data[5].u = 0.f;
				data[5].v = 1.f;

				pVS->Unlock();
			}

			g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

			g_pd3dDevice->SetStreamSource(0, pVS, 0, sizeof(_VS));
			g_pd3dDevice->SetTexture(0, g_pTextureBack);
			g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			g_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		}

		SetupMatrices();

		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		if (g_pFontRenderer->Prepare())
		{

			Font *pFont = g_FontAjust.GetFont();
			if (pFont)
			{

				pFont->SetTextAlpha(255);
				pFont->SetTextColor(D3DCOLOR_RGBA(g_nFontColorRGB[0], g_nFontColorRGB[1], g_nFontColorRGB[2], 255));
				pFont->SetTextScaling(1.0f, 1.0f);

				float fY = 0.0f;
				float fDelta = pFont->GetCharHeight() + 2;
				pFont->DrawText(0.0f, fY, "==================================================================", -1);
				fY += fDelta;				 
				pFont->DrawText(0.0f, fY, "【卜算子】咏梅", -1);
				fY += fDelta;				 
				pFont->DrawText(0.0f, fY, "驿外断桥边，寂寞开无主。", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "已是黄昏独自愁，更著风和雨。", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "无意苦争春，一任群芳妒。", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "零落成泥辗作尘，只有香如故。", -1);
				fY += fDelta;

				pFont->DrawText(0.0f, fY, "==================================================================", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "A White Rose", -1);
				fY += fDelta;				 
				pFont->DrawText(0.0f, fY, "The red rose whispers of passion,", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "And the white rose breathes of love;", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "O the red rose is a falcon, ", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "And the white rose is a dove.", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "But I send you a cream-white rosebud", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "With a flush on its petal tips;", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "For the love that is purest and sweetest", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "Has a kiss of desire on the lips.", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "==================================================================", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, ",<.>;:'\"/?\\|=+-_(){}[]*&^%$#@!...0123456789876543210...", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "==================================================================", -1);
				fY += fDelta;

				pFont->DrawText(0.0f, fY, "　　黄昏来时翠翠坐在家中屋后白塔下，看天空为夕阳烘成桃花色的薄云。", -1);
				fY += fDelta;				 
				pFont->DrawText(0.0f, fY, "十四中寨逢场，城中生意人过中寨收买山货的很多，过渡人也特别多，祖父", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "在渡船上忙个不息。天快夜了，别的雀子似乎都在休息了，只杜鹃叫个不息。", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "石头泥土为白日晒了一整天，草木为白日晒了一整天，到这时节皆放散一种", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "热气。空气中有泥土气味，有草木气味，且有甲虫类气味。翠翠看着天上的", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "红云，听着渡口飘乡生意人的杂乱声音，心中有些儿薄薄的凄凉。", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "　　黄昏照样的温柔，美丽，平静。但一个人若体念到这个当前一切时，也", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "就照样的在这黄昏中会有点儿薄薄的凄凉。于是，这日子成为痛苦的东西了。", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "翠翠觉得好象缺少了什么。好象眼见到这个日子过去了，想在一件新的人事", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "上攀住它，但不成。好象生活太平凡了，忍受不住。", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "　　“我要坐船下桃源县过洞庭湖，让爷爷满城打锣去叫我，点了灯笼火把", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "去找我。”", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "　　她便同祖父故意生气似的，很放肆的去想到这样一件事，她且想象她出", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "走后，祖父用各种方法寻觅全无结果，到后如何无可奈何躺在渡船上。", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "　　人家喊，“过渡，过渡，老伯伯，你怎么的，不管事！”“怎么的！翠", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "翠走了，下桃源县了！”“那你怎么办？”“怎么办吗？拿把刀，放在包袱", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "里，搭下水船去杀了她！”…… ", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "　　翠翠仿佛当真听着这种对话，吓怕起来了，一面锐声喊着她的祖父，一", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "面从坎上跑向溪一面从坎上跑向溪边渡口去。见到了祖父正把船拉在溪中心，", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "船上人喁喁说着话，小小心子还依然跳跃不已。", -1);
				fY += fDelta;
				pFont->DrawText(0.0f, fY, "==================================================================", -1);
				fY += fDelta;				
			}
			g_pFontRenderer->Flush();

		}

		struct _av
		{
			D3DXVECTOR4 v;
			DWORD c;
		};
		_av av[256];
		const unsigned char *puch = g_FontAjust.GetUch();
		for (int i = 0; i < 256; ++i)
		{
			av[i].v.x = 700.0f + i / 2.0f;
			av[i].v.y = 10 + (255.0f - puch[i]) / 2.0f;
			av[i].v.z = 0.0f;
			av[i].v.w = 1.0f;
			av[i].c = D3DCOLOR_RGBA(g_nFontColorRGB[0], g_nFontColorRGB[1], g_nFontColorRGB[2], 255);
		}


		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		g_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
		g_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		g_pd3dDevice->SetTexture(0, NULL);
		HRESULT hr = g_pd3dDevice->DrawPrimitiveUP(
			D3DPT_LINESTRIP,
			255,
			av,
			sizeof(_av)
			);

        g_pd3dDevice->EndScene();
    }

    g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

LRESULT CALLBACK GetUserFontSize(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			::SetDlgItemInt(hDlg, IDC_EDIT_SIZE, g_FontAjust.GetFontSize(), false);
		}
		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				int nSize = ::GetDlgItemInt(hDlg, IDC_EDIT_SIZE, NULL, false);
				if (nSize < 2)
					nSize = 2;
				if (nSize > 100)
					nSize = 100;
				g_FontAjust.SetFontSize(nSize);
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
			break;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK GetUserFontDPI(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			::SetDlgItemInt(hDlg, IDC_EDIT_DPI, g_FontAjust.GetDpi(), false);
		}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				int nSize = ::GetDlgItemInt(hDlg, IDC_EDIT_DPI, NULL, false);
				if (nSize < 10)
					nSize = 10;
				if (nSize > 200)
					nSize = 200;
				g_FontAjust.SetDpi(nSize);
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
			break;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK AboutBoxProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
			break;
		}
		break;
	}
	return FALSE;
}

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
    switch(msg)
    {
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	case WM_COMMAND:
		{
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
			case ID_OPEN:
				{
					LPTSTR pszClip = NULL;
					LPTSTR pszTemp = NULL;
					OPENFILENAME	ofn;
					TCHAR	szFile[MAX_PATH];
					szFile[0] = _T('\0');
					TCHAR szDirLast[MAX_PATH];
					szDirLast[0] = _T('\0');

					_tcscpy(szDirLast, g_FontAjust.GetFontFile()); 
					LPTSTR pszLast = _tcsrchr(szDirLast, _T('\\'));
					if (pszLast)
					{
						LPTSTR pcszNext = _tcsrchr(szDirLast, _T('/'));
						if (pcszNext)
							pszLast = pcszNext;
						pszLast++;
						*pszLast = _T('\0');
					}

					memset(&ofn, 0, sizeof(OPENFILENAME));
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = hWnd;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
						OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
					ofn.lpstrTitle = _T("请选择字体文件");
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFile = szFile;
					ofn.lpstrFilter = _T("字体文件\0*.ttf\0\0");
					ofn.lpstrInitialDir = szDirLast;
					if (::GetOpenFileName(&ofn))
					{
						g_FontAjust.Init(szFile, g_FontAjust.GetFontSize(), g_FontAjust.GetDpi());
						if (g_FontAjust.IsSharp())
						{
							if (GetMenuState(GetMenu(hWnd), ID_USE, MF_BYCOMMAND) != MF_CHECKED) 
							{
								CheckMenuItem(GetMenu(hWnd), ID_USE, MF_CHECKED);
							}
						}
						else
						{
							if (GetMenuState(GetMenu(hWnd), ID_USE, MF_BYCOMMAND) == MF_CHECKED) 
							{
								CheckMenuItem(GetMenu(hWnd), ID_USE, MF_UNCHECKED);
							}
						}

						if (g_hControlWnd)
						{
							::SendMessage(g_hControlWnd, WM_INITDIALOG, 0, 0);
						}
					}
				}
				break;
			case ID_SAVE:
				g_FontAjust.Save();
				break;
			case ID_SAVETEXT:
				g_FontAjust.SaveText();
				break;
			case IDM_EXIT:
				::DestroyWindow(hWnd);
				break;
			case IDM_ABOUT:
				DialogBox(NULL, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)AboutBoxProc);
				break;
			case ID_FONT_SIZE:
				DialogBox(NULL, (LPCTSTR)IDD_INPUT_FONT_SIZE, hWnd, (DLGPROC)GetUserFontSize);
				{
					if (g_FontAjust.IsSharp())
					{
						if (GetMenuState(GetMenu(hWnd), ID_USE, MF_BYCOMMAND) != MF_CHECKED) 
						{
							CheckMenuItem(GetMenu(hWnd), ID_USE, MF_CHECKED);
						}
					}
					else
					{
						if (GetMenuState(GetMenu(hWnd), ID_USE, MF_BYCOMMAND) == MF_CHECKED) 
						{
							CheckMenuItem(GetMenu(hWnd), ID_USE, MF_UNCHECKED);
						}
					}

					if (g_hControlWnd)
					{
						::SendMessage(g_hControlWnd, WM_INITDIALOG, 0, 0);
					}
				}
				break;
			case ID_DPI:
				DialogBox(NULL, (LPCTSTR)IDD_INSERT_DPI, hWnd, (DLGPROC)GetUserFontDPI);
				{
					if (g_FontAjust.IsSharp())
					{
						if (GetMenuState(GetMenu(hWnd), ID_USE, MF_BYCOMMAND) != MF_CHECKED) 
						{
							CheckMenuItem(GetMenu(hWnd), ID_USE, MF_CHECKED);
						}
					}
					else
					{
						if (GetMenuState(GetMenu(hWnd), ID_USE, MF_BYCOMMAND) == MF_CHECKED) 
						{
							CheckMenuItem(GetMenu(hWnd), ID_USE, MF_UNCHECKED);
						}
					}

					if (g_hControlWnd)
					{
						::SendMessage(g_hControlWnd, WM_INITDIALOG, 0, 0);
					}

				}
				break;
			case ID_USE:
				{
					if (GetMenuState(GetMenu(hWnd), ID_USE, MF_BYCOMMAND) == MF_CHECKED) 
					{
						CheckMenuItem(GetMenu(hWnd), ID_USE, MF_UNCHECKED);
						g_FontAjust.NoSharp();
					}
					else
					{
						CheckMenuItem(GetMenu(hWnd), ID_USE, MF_CHECKED);
						g_FontAjust.Sharp();
					}

					if (g_hControlWnd)
					{
						::SendMessage(g_hControlWnd, WM_INITDIALOG, 0, 0);
					}

				}
				break;
			case ID_FONT_COLOR:
				{

					CHOOSECOLOR		color = { 0 };
					COLORREF		CustClr[16] = { 0 };

					color.lStructSize = sizeof(CHOOSECOLOR);
					color.hwndOwner   = hWnd;
					color.Flags = CC_RGBINIT | CC_FULLOPEN;
					color.lpCustColors = CustClr;
					color.rgbResult = (g_nFontColorRGB[0] | (g_nFontColorRGB[1] << 8) | (g_nFontColorRGB[2] << 16));
					if (::ChooseColor(&color))
					{
						g_nFontColorRGB[0] =  (color.rgbResult & 0xff);
						g_nFontColorRGB[1] =  ((color.rgbResult >> 8) & 0xff);
						g_nFontColorRGB[2] =  ((color.rgbResult >> 16) & 0xff);
					}
				}
				break;
			case ID_BACK_COLOR:
				{

					CHOOSECOLOR		color = { 0 };
					COLORREF		CustClr[16] = { 0 };

					color.lStructSize = sizeof(CHOOSECOLOR);
					color.hwndOwner   = hWnd;
					color.Flags = CC_RGBINIT | CC_FULLOPEN;
					color.lpCustColors = CustClr;
					color.rgbResult = (g_nBackColorRGB[0] | (g_nBackColorRGB[1] << 8) | (g_nBackColorRGB[2] << 16));
					if (::ChooseColor(&color))
					{
						g_nBackColorRGB[0] =  (color.rgbResult & 0xff);
						g_nBackColorRGB[1] =  ((color.rgbResult >> 8) & 0xff);
						g_nBackColorRGB[2] =  ((color.rgbResult >> 16) & 0xff);
					}
				}
				break;
			case ID_USE_BACK:
				{
					if (GetMenuState(GetMenu(hWnd), ID_USE_BACK, MF_BYCOMMAND) == MF_CHECKED) 
					{
						CheckMenuItem(GetMenu(hWnd), ID_USE_BACK, MF_UNCHECKED);
						g_nUseBackTexture = false;
					}
					else
					{
						CheckMenuItem(GetMenu(hWnd), ID_USE_BACK, MF_CHECKED);
						g_nUseBackTexture = true;
					}
				}
				break;
			case ID_BACK_PIC:
				{
					LPTSTR pszClip = NULL;
					LPTSTR pszTemp = NULL;
					OPENFILENAME	ofn;
					TCHAR	szFile[MAX_PATH];
					szFile[0] = _T('\0');
					static TCHAR szDirLast[MAX_PATH] = {0};

					memset(&ofn, 0, sizeof(OPENFILENAME));
					ofn.lStructSize = sizeof(OPENFILENAME);
					ofn.hwndOwner = hWnd;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST |
						OFN_HIDEREADONLY | OFN_EXPLORER | OFN_EXTENSIONDIFFERENT;
					ofn.lpstrTitle = _T("请选择背景贴图文件");
					ofn.nMaxFile = sizeof(szFile);
					ofn.lpstrFile = szFile;
					ofn.lpstrFilter = _T("贴图文件\0*.jpg;*.jpeg;*.tga;*.dds;*.bmp\0\0");
					ofn.lpstrInitialDir = szDirLast;
					if (::GetOpenFileName(&ofn))
					{
						LPDIRECT3DTEXTURE9 pTexture = NULL;
						if (SUCCEEDED(D3DXCreateTextureFromFile(g_pd3dDevice, szFile, &pTexture) && pTexture))
						{
							if (g_pTextureBack)
							{
								g_pTextureBack->Release();
								g_pTextureBack = NULL;
							}
							g_pTextureBack = pTexture;
							CheckMenuItem(GetMenu(hWnd), ID_USE_BACK, MF_CHECKED);
							g_nUseBackTexture = true;
						}

						_tcscpy(szDirLast, szFile); 
						LPTSTR pszLast = _tcsrchr(szDirLast, _T('\\'));
						if (pszLast)
						{
							LPTSTR pcszNext = _tcsrchr(szDirLast, _T('/'));
							if (pcszNext)
								pszLast = pcszNext;
							pszLast++;
							*pszLast = _T('\0');
						}

					}
				}
				break;
			}
		}
		break;
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case VK_UP:
				break;
			case VK_DOWN:
				break;
			case VK_LEFT:
				break;
			case VK_RIGHT:
				break;
			}
		}
		break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK ParamControl(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		{
			HWND hSlider = ::GetDlgItem(hDlg, IDC_SLIDER1);
			if (hSlider)
			{
				SendMessage(hSlider, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 4000));
				SendMessage(hSlider, TBM_SETPAGESIZE, 0, (LPARAM)0);
				SendMessage(hSlider, TBM_SETSEL, (WPARAM)FALSE, (LPARAM)MAKELONG(0, 4000));     
				SendMessage(hSlider, TBM_SETPOS, (WPARAM)TRUE,  (LPARAM)((int)(g_FontAjust.GetControl1() * 1000)));     
				TCHAR szValue[32];
				_stprintf(szValue, _T("%.4f"), g_FontAjust.GetControl1());
				::SetDlgItemText(hDlg, IDC_VALUE1, szValue);
			}
			hSlider = ::GetDlgItem(hDlg, IDC_SLIDER2);
			if (hSlider)
			{
				SendMessage(hSlider, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 4000));
				SendMessage(hSlider, TBM_SETPAGESIZE, 0, (LPARAM)0);
				SendMessage(hSlider, TBM_SETSEL, (WPARAM)FALSE, (LPARAM)MAKELONG(0, 4000));     
				SendMessage(hSlider, TBM_SETPOS, (WPARAM)TRUE,  (LPARAM)((int)(g_FontAjust.GetControl2() * 1000)));     
				TCHAR szValue[32];
				_stprintf(szValue, _T("%.4f"), g_FontAjust.GetControl2());
				::SetDlgItemText(hDlg, IDC_VALUE2, szValue);
			}
		}
		return TRUE;
	case WM_COMMAND:
		break;
	case WM_HSCROLL:
		if ((wParam & 0x0000FFFF) == SB_THUMBPOSITION || (wParam & 0x0000FFFF) == SB_THUMBTRACK)
		{
			int nValue = (int)((wParam & 0xFFFF0000) >> 16);
			HWND hSlider = ::GetDlgItem(hDlg, IDC_SLIDER1);
			if (hSlider == (HWND)lParam)
			{
				g_FontAjust.SetControl1(nValue / 1000.0f);
				TCHAR szValue[32];
				_stprintf(szValue, _T("%.4f"), nValue / 1000.0f);
				::SetDlgItemText(hDlg, IDC_VALUE1, szValue);
			}

			hSlider = ::GetDlgItem(hDlg, IDC_SLIDER2);
			if (hSlider == (HWND)lParam)
			{
				g_FontAjust.SetControl2(nValue / 1000.0f);
				TCHAR szValue[32];
				_stprintf(szValue, _T("%.4f"), nValue / 1000.0f);
				::SetDlgItemText(hDlg, IDC_VALUE2, szValue);
			}	
		}
		break;

	}
	return FALSE;
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      hInst, NULL, NULL, NULL, NULL,
                      _T("D3D Tutorial"), NULL };
	wc.lpszMenuName = (LPCTSTR)IDC_TESTFONT;
    RegisterClassEx(&wc);

    // Create the application's window
    HWND hWnd = CreateWindow(_T("D3D Tutorial"), _T("D3D Tutorial 03: Matrices"),
                              WS_OVERLAPPED | WS_SYSMENU , 10, 10, 900, 610,
                              NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if(SUCCEEDED(InitD3D(hWnd)))
    {
        // Create the scene geometry
        if(SUCCEEDED(InitGeometry()))
        {
            // Show the window
            ShowWindow(hWnd, SW_SHOWDEFAULT);
            UpdateWindow(hWnd);

			g_hControlWnd = ::CreateDialog((HINSTANCE)wc.hInstance, MAKEINTRESOURCE(IDD_PARAM_CONTROL), hWnd, (DLGPROC)ParamControl);
			if (g_hControlWnd)
			{
				RECT rc;
				::GetWindowRect(g_hControlWnd, &rc);
				::SetWindowPos(g_hControlWnd, HWND_TOP, 600, 300, rc.right - rc.left, rc.bottom - rc.top,SWP_SHOWWINDOW);
				::ShowWindow(g_hControlWnd, SW_SHOW);
			}

            // Enter the message loop
            MSG msg;
            ZeroMemory(&msg, sizeof(msg));
            while(msg.message!=WM_QUIT)
            {
                if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                else
				{
                    Render(hWnd);
				}
            }

			if (g_hControlWnd)
			{
				::EndDialog(g_hControlWnd, 0);
			}

			if (pVS)
			{
				pVS->Release();
				pVS = NULL;
			}

			if (g_pTextureBack)
			{
				g_pTextureBack->Release();
				g_pTextureBack = NULL;
			}
        }
    }

    UnregisterClass(_T("D3D Tutorial"), wc.hInstance);
    return 0;
}



