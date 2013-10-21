// File: TerrainApp.cpp
// Desc:

#include"TerrainApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


TerrainApp* g_pTApp = 0;
Common::WOFile gLog;

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nShowCmd)
{
	try
	{
	gLog.SetFilename("Log.txt");
	gLog.Open();
	gLog.WriteLine("Entered WinMain...");

	g_pTApp = new TerrainApp("Config.ini", hInst);
	if(!g_pTApp)
	{
		return 1;
	}

	if(g_pTApp->Init())
	{
		g_pTApp->Run();
	}

	}
	catch(Common::Exception e)
	{
		MessageBox(0, e.GetErrorMessage(), e.GetTitle(), MB_OK);
		if(e.GetLevel() == Common::EL_BAD)
		{
			return 2;
		}

	}

	gLog.WriteLine("Window is going to shut down...");

	return 0;
}

bool TerrainApp::Init()
{
	Common::INIFile inif;
	inif.SetFilename(m_sConfigFilename);
	inif.Open();

	Common::String sTemp;

	inif.GetINI("Window", "Title", m_sTitle);
	inif.GetINI("Window", "Width", sTemp);
	m_Width = sTemp.IntValue();
	inif.GetINI("Window", "Height", sTemp);
	m_Height = sTemp.IntValue();

	if(!InitWindow())  // Initialize the window
	{
		return false;
	}
	gLog.WriteLine("Window created OK!");

	// Inititalize the D3D

	Render::RenderCoreDesc rcdesc;
	memset(&rcdesc, 0, sizeof(rcdesc));

	rcdesc.m_hWnd = m_hWnd;  // HWND

	inif.GetINI("Render", "Adapter", sTemp);
	rcdesc.m_Adapter = sTemp.IntValue();  // Adapter

	inif.GetINI("HardwareRequirements", "DeviceType", sTemp);
	rcdesc.m_DeviceType = Render::IntToDevType((unsigned int)sTemp.IntValue());  // D3D device type

	inif.GetINI("Render", "ColorBits", sTemp);
	rcdesc.m_DisplayColorBits = sTemp.IntValue();  // Display color bits

	inif.GetINI("HardwareRequirements", "UsesMixedVertexProcessing", sTemp);
	rcdesc.HR_UsesMixedVP = (bool)sTemp.IntValue();

	inif.GetINI("HardwareRequirements", "MinimumStencilBits", sTemp);
	rcdesc.HR_MinStencilBits = sTemp.IntValue();

	rcdesc.m_PresentParameters.BackBufferWidth = m_Width;
	rcdesc.m_PresentParameters.BackBufferHeight = m_Height;

	rcdesc.m_PresentParameters.hDeviceWindow = m_hWnd;
	inif.GetINI("Render", "IsFullscreen", sTemp);
	rcdesc.m_PresentParameters.Windowed = !((bool)sTemp.IntValue());

	gLog.WriteLine("Try to initialize D3D...");
	if(!g_GetRenderCore()->Init(rcdesc))  //********** ==> Initializing D3D RenderCore
	{
		gLog.WriteLine("D3D initialization failed!");
		return false;
	}

	gLog.WriteLine("D3D initialized OK!");

	inif.GetINI("Terrain", "Name", sTemp);
	g_GetTerrainTree()->SetName(sTemp);

	inif.SetFilename(sTemp + ".ttd");
	inif.Open();
	inif.GetINI("TreeInfo", "CPX", sTemp);
	float cpx = sTemp.FloatValue();
	inif.GetINI("TreeInfo", "CPY", sTemp);
	float cpy = sTemp.FloatValue();
	inif.GetINI("TreeInfo", "CPZ", sTemp);
	float cpz = sTemp.FloatValue();
	g_GetTerrainTree()->CreateD3DResources();
	g_GetTerrainTree()->CreateQuadTree(cpx, cpy, cpz);

	inif.Close();


	m_Camera.Init(Maths::Vector3F(cpx, cpy + 256.f, cpz), Maths::Vector3F(cpx, cpy + 256.f, cpz + 100.f),
		Maths::Vector3F(0.f, 1.f, 0.f), 5.f, 30000.f, PI / 3, (float)m_Height / (float)m_Width);

	m_Camera.GetViewMatrix(m_mtxView);
	m_Camera.GetProjectionMatrix(m_mtxProj);

	m_pVB = g_GetRenderCore()->CreateVertexBuffer(3, sizeof(TEST_VERTEX), TEST_VERTEX_FVF);
	void* pVB = 0;
	m_pVB->Lock(0, 0, &pVB, 0);
	TEST_VERTEX varray[] =
	{
		0,   cpy + 256.f, 0, D3DCOLOR_XRGB(0, 255, 0),
		1024,  cpy + 256.f, -1024,  D3DCOLOR_XRGB(0, 255, 0),
		-1024, cpy + 256.f, -1024,  D3DCOLOR_XRGB(0, 0, 255)
	};

	memcpy(pVB, varray, 3 * sizeof(TEST_VERTEX));
	m_pVB->Unlock();

	return true;
}

void TerrainApp::Destroy()
{
	g_GetTerrainTree()->DestroyTree();
}

void TerrainApp::Run()
{
	gLog.WriteLine("Enter main loop...");

	g_GetRenderCore()->GetD3DDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);  // CCW
	g_GetRenderCore()->GetD3DDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	g_GetRenderCore()->GetD3DDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	g_GetRenderCore()->GetD3DDevice()->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_GetRenderCore()->GetD3DDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);  // WIREFRAME
	m_bSolid = false;

	Maths::Matrix mtx;
	g_GetRenderCore()->SetWorldMatrix(mtx);
	g_GetRenderCore()->SetProjectionMatrix(m_mtxProj);

	D3DXCreateTextureFromFile(g_GetRenderCore()->GetD3DDevice(), "tex.jpg", &m_pTexture);

	//m_mtxView = GetViewMatrixFromVF();
	//m_mtxProj = GetProjectionMatrixFromVF();

	//for(unsigned int i = 0; i < 11; ++i)
	{
	//	m_Camera.TurnRight(PI/64);
	}
	//m_Camera.GetCachedViewMatrix(m_mtxView);
	SendMessage(m_hWnd, WM_KEYDOWN, 'O', 0);

	m_Font.Init("宋体", 13, true, false, false);
	unsigned int Loop = 0;
	while(true)
	{
		if(!PumpWindowMessages())
		{
			gLog.WriteLine("Window is going to shut down...");
			return;
		}

		if(m_bActive)
		{
			unsigned int FrameBegin = g_GetDateTimeSystem()->GetTimeValueMS();


			g_GetRenderCore()->Clear(m_BGColor);
			g_GetRenderCore()->Begin();

			g_GetRenderCore()->SetViewMatrix(m_mtxView);

			if(m_pTexture)
			{
				g_GetRenderCore()->SetTexture(0, m_pTexture);
			}



			unsigned int TimeWhileCreateBTTree = g_GetDateTimeSystem()->GetTimeValueMS();
			unsigned int TimeWhileRender = g_GetDateTimeSystem()->GetTimeValueMS();
			if(g_GetTerrainTree()->CreateBTTree(m_Camera))
			{
				TimeWhileCreateBTTree = g_GetDateTimeSystem()->GetTimeValueMS() - TimeWhileCreateBTTree;

				TimeWhileRender = g_GetDateTimeSystem()->GetTimeValueMS();
				g_GetTerrainTree()->Render();
				TimeWhileRender = g_GetDateTimeSystem()->GetTimeValueMS() - TimeWhileRender;
			}
			else
			{
				TimeWhileCreateBTTree = g_GetDateTimeSystem()->GetTimeValueMS() - TimeWhileCreateBTTree;
				TimeWhileRender = g_GetDateTimeSystem()->GetTimeValueMS() - TimeWhileRender;
			}

			unsigned int FrameTimeMS = g_GetDateTimeSystem()->GetTimeValueMS() - FrameBegin;
			if(FrameTimeMS == 0)
			{
				//Common::String s = Common::String((int)FrameTimeMS) + ", loop: " + Common::String((int)loop);
				//m_Font.DrawSingleText("Warning: The FrameTimeMS is drop near the zero!", 0, m_Height - 50, D3DCOLOR_XRGB(0, 255, 0));
				FrameTimeMS = 1;
			}
			if(TimeWhileCreateBTTree == 0)
			{
				TimeWhileCreateBTTree = 1;
			}

			Common::String sInfo;
			m_Font.Begin();

			m_Font.DrawText("统计信息：", 0, 0, D3DCOLOR_XRGB(0, 255, 0));

			sInfo = "Frame time: " + Common::String((int)FrameTimeMS) + " ms; FPS: " + Common::String((int)(1000 / FrameTimeMS)) + "; Loop: " + Common::String((int)Loop);
			m_Font.DrawText(sInfo, 0, 15, D3DCOLOR_XRGB(0, 255, 0));

			sInfo = "NumOfVisibleFieldNodes: " + Common::String((int)g_GetTerrainTree()->GetNumOfVisibleFieldNodes()) + "; NumOfVisibleQuadLeaves: " + Common::String((int)g_GetTerrainTree()->GetNumOfVisibleQuadLeaves()) + "; NumOfSubBTNodes: " + Common::String((int)g_GetTerrainTree()->GetNumOfSubBTNodes());
			m_Font.DrawText(sInfo, 0, 30, D3DCOLOR_XRGB(0, 255, 0));

			sInfo = "NumOfVertices: " + Common::String((int)g_GetTerrainTree()->GetNumOfVertices()) + "; NumOfPrimitives: " + Common::String((int)g_GetTerrainTree()->GetNumOfPrimitives());
			m_Font.DrawText(sInfo, 0, 45, D3DCOLOR_XRGB(0, 255, 0));

			sInfo = "TimeWhileCreateBTTree: " + Common::String((int)TimeWhileCreateBTTree) +
				"ms, " + Common::String((int)(TimeWhileCreateBTTree * 100 / FrameTimeMS)) + "% in total frame time";
			m_Font.DrawText(sInfo, 0, 60, D3DCOLOR_XRGB(0, 255, 0));

			sInfo = "TimeWhileRender: " + Common::String((int)TimeWhileRender) +
				"ms, " + Common::String((int)(TimeWhileRender * 100 / FrameTimeMS)) + "% in total frame time";
			m_Font.DrawText(sInfo, 0, 75, D3DCOLOR_XRGB(0, 255, 0));

			int TimeWhileFindVisibleFieldNodes = g_GetTerrainTree()->GetTimeWhileFindVisibleFieldNodes();
			sInfo = "TimeWhileFindVisibleFieldNodes: " + Common::String(TimeWhileFindVisibleFieldNodes) + " ms, " + Common::String((int)(TimeWhileFindVisibleFieldNodes * 100 / TimeWhileCreateBTTree)) + "% in TimeWhileCreateBTTree";
			m_Font.DrawText(sInfo, 0, 90, D3DCOLOR_XRGB(0, 255, 0));

			int TimeWhileFindVisibleQuadLeaves = g_GetTerrainTree()->GetTimeWhileFindVisibleQuadLeaves();
			sInfo = "TimeWhileFindVisibleQuadLeaves: " + Common::String(TimeWhileFindVisibleQuadLeaves) + " ms, " + Common::String((int)(TimeWhileFindVisibleQuadLeaves * 100 / TimeWhileCreateBTTree)) + "% in TimeWhileCreateBTTree";
			m_Font.DrawText(sInfo, 0, 105, D3DCOLOR_XRGB(0, 255, 0));

			int TimeWhileCreateOutlineVertices = g_GetTerrainTree()->GetTimeWhileCreateOutlineVertices();
			sInfo = "TimeWhileCreateOutlineVertices: " + Common::String(TimeWhileCreateOutlineVertices) + " ms, " + Common::String((int)(TimeWhileCreateOutlineVertices * 100 / TimeWhileCreateBTTree)) + "% in TimeWhileCreateBTTree";
			m_Font.DrawText(sInfo, 0, 120, D3DCOLOR_XRGB(0, 255, 0));

			int TimeWhileHandleTouchedQuadLeaves = g_GetTerrainTree()->GetTimeWhileHandleTouchedQuadLeaves();
			sInfo = "TimeWhileHandleTouchedQuadLeaves: " + Common::String(TimeWhileHandleTouchedQuadLeaves) + " ms, " + Common::String((int)(TimeWhileHandleTouchedQuadLeaves * 100 / TimeWhileCreateBTTree)) + "% in TimeWhileCreateBTTree";
			m_Font.DrawText(sInfo, 0, 135, D3DCOLOR_XRGB(0, 255, 0));

			int TimeWhileCreateSubBTNodes = g_GetTerrainTree()->GetTimeWhileCreateSubBTNodes();
			sInfo = "TimeWhileCreateSubBTNodes: " + Common::String(TimeWhileCreateSubBTNodes) + "ms, " + Common::String((int)(TimeWhileCreateSubBTNodes * 100 / TimeWhileCreateBTTree)) + "% in TimeWhileCreateBTTree";
			m_Font.DrawText(sInfo, 0, 150, D3DCOLOR_XRGB(0, 255, 0));

			int TimeWhileExtractIndices = g_GetTerrainTree()->GetTimeWhileExtractIndices();
			sInfo = "TimeWhileExtractIndices: " + Common::String(TimeWhileExtractIndices) + "ms, " + Common::String((int)(TimeWhileExtractIndices * 100 / TimeWhileCreateBTTree)) + "% in TimeWhileCreateBTTree";
			m_Font.DrawText(sInfo, 0, 165, D3DCOLOR_XRGB(0, 255, 0));

			m_Font.End();

			g_GetRenderCore()->End();
			g_GetRenderCore()->Present();
			++Loop;

			//if(Loop == 200)
			//{
				//SendMessage(m_hWnd, WM_CLOSE, 0, 0);
				//SendMessage(m_hWnd, WM_KEYDOWN, VK_UP, 0);
			//}
		}
	}
}

bool TerrainApp::PumpWindowMessages()
{
	static MSG msg;
	BOOL bGotMsg;

	if(m_bActive)
	{
		bGotMsg = PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE);
	}
	else
	{
		bGotMsg = GetMessage(&msg, NULL, 0U, 0U);
	}

	if(bGotMsg)
	{
		if(WM_QUIT == msg.message)
		{
			return false;  // 退出
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

bool TerrainApp::InitWindow()
{
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)TerrainApp::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInst;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(0,0,0));
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = m_sTitle.c_str();
	wcex.hIconSm = 0;

	if(0 == RegisterClassEx(&wcex))
	{
		return false;
	}

	m_dwWindowStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_VISIBLE;

	// Set the window's initial width
	RECT rc;
	SetRect( &rc, 0, 0, m_Width, m_Height);
	AdjustWindowRect(&rc, m_dwWindowStyle, TRUE);

	// Create the render window
	m_hWnd = CreateWindow(wcex.lpszClassName, m_sTitle, m_dwWindowStyle,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               rc.right - rc.left, rc.bottom - rc.top, 0L,
                               0,
                               m_hInst, 0L);

	if(m_hWnd == 0)
	{
		return false;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return true;
}

LRESULT TerrainApp::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bRunDefault = false;

	switch(uMsg)
	{
	case WM_CREATE:
		g_pTApp->SetActive(true);
		break;

	case WM_PAINT:
		ValidateRect(hWnd, 0);
		break;

	case WM_ACTIVATEAPP:
		{
			bool bActive = (bool)wParam;
			//if(!g_pTApp->IsActive() && bActive)
			//{
			//	g_GetRenderCore()->Reset();
			//}
			g_pTApp->SetActive(bActive);
		}
		break;

	case WM_SETFOCUS:
		// DInput Process...
		break;

	case WM_KILLFOCUS:
		// DInput Process...
		break;

	case WM_KEYDOWN:
		{
			bool bSetView = false;
			switch(wParam)
			{
			case VK_UP:  // 抬头
				g_pTApp->m_Camera.LookUp(PI/64);

				g_pTApp->m_BGColor = D3DCOLOR_XRGB(0, 0, 0);

				bSetView = true;

				break;

			case VK_DOWN:  // 低头
				g_pTApp->m_Camera.LookDown(PI/64);

				g_pTApp->m_BGColor = D3DCOLOR_XRGB(0, 0, 0);

				bSetView = true;

				break;

			case VK_LEFT:  // 左看
				g_pTApp->m_Camera.TurnLeft(PI/64);

				g_pTApp->m_BGColor = D3DCOLOR_XRGB(0, 0, 0);

				bSetView = true;

				break;

			case VK_RIGHT:  // 右看
				g_pTApp->m_Camera.TurnRight(PI/64);

				g_pTApp->m_BGColor = 0;

				bSetView = true;

				break;

			case 'W':  // 前进
				g_pTApp->m_Camera.MoveForward(100.f);

				bSetView = true;

				break;

			case 'S':  // 后退
				g_pTApp->m_Camera.MoveBackward(100.f);

				bSetView = true;

				break;

			case 'A':  // 左平移
				g_pTApp->m_Camera.StrafeLeft(100.f);

				bSetView = true;

				break;

			case 'D':  // 右平移
				g_pTApp->m_Camera.StrafeRight(100.f);

				bSetView = true;

				break;

			case 'L':
				if(g_pTApp->m_bSolid)
				{
					g_GetRenderCore()->GetD3DDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
					g_pTApp->m_bSolid = false;
				}
				else
				{
					g_GetRenderCore()->GetD3DDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
					g_pTApp->m_bSolid = true;
				}
				break;

			case 'O':
				if(g_GetTerrainTree()->GetDrawOutlineOnly())
				{
					g_GetTerrainTree()->SetDrawOutlineOnly(false);
					MessageBox(0, "LOD Enabled.", "'O' Pressed", MB_OK);
				}
				else
				{
					g_GetTerrainTree()->SetDrawOutlineOnly(true);
					MessageBox(0, "LOD Disabled.", "'O' Pressed", MB_OK);
				}
				break;
			}
			if(bSetView)
			{
				g_pTApp->m_Camera.GetCachedViewMatrix(g_pTApp->m_mtxView);
			}
		}

		break;

//	case WM_SYSCOMMAND:
//		// Prevent moving/sizing and power loss in fullscreen mode
//		switch( wParam )
//		{
//		case SC_MOVE:
//		case SC_SIZE:
//		case SC_MAXIMIZE:
//		case SC_MONITORPOWER:
//			return 1;
//		default:
//			return DefWindowProc(hWnd, uMsg, wParam, lParam);
//		}
	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	default:
		bRunDefault = true;
		break;
	}

	if(bRunDefault)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}