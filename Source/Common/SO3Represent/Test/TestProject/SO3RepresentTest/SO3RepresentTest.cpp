// SO3RepresentTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "case/gameworld/kgameworldhandler.h"
#include "case/actionobject/kactionactivenpcnormal.h"
#include "base/object/kactiveobjectmgr.h"
#include "base/file/kfilepath.h"
#include "base/action/kactionmgr.h"
#include "../../KG3DEngine/Include/KG3DInterface.h"
#include "../../KG3DEngine/KG3DEngine/KG3DCamera.h"

// global flag 
bool bDone = false; 
int	 g_nOutputWindowID = 0;
HWND g_hWnd = NULL;
IKG3DEngineManager* g_pEngineManager = NULL;
DWORD				g_dwScene = 0;

static float const fCameraSpeed = 100.0;
static int s_nLastX = 0;
static int s_nLastY = 0;


inline IKSO3RepresentHandler& GetRepresent()
{
	return g_GetRepresent();
}


int InitAction()
{
	int nRetCode = 0;
	HRESULT hr = E_FAIL;

	KGameWorldHandler* pGameWorldHandler = NULL;

	KActionActiveNPCNormal* pNPC0 = NULL;
	KActionActiveNPCNormal* pNPC1 = NULL;
	KActionActiveNPCNormal* pNPC2 = NULL;

	IKSO3RepresentHandler& pRepresent = GetRepresent();

	nRetCode = KActiveObjectMgr::GetSelf().Lookup(&pGameWorldHandler, _T("GameWorldHandler"));
	KG_PROCESS_ERROR(nRetCode);

	g_pEngineManager = g_GetEngineManager();
	g_pEngineManager->Init(0, g_hWnd, g_hWnd);

	hr = pRepresent.InitScene(_T("Main"), 0, g_hWnd);
	KG_COM_PROCESS_ERROR(hr);

#if 0
	hr = pRepresent.OnNewScene(_T("Main"), g_dwScene, 0, g_hWnd);
	KG_COM_PROCESS_ERROR(hr);

	hr = pRepresent.NPC_New(0);
	KG_COM_PROCESS_ERROR(hr);

	hr = pRepresent.NPC_New(1);
	KG_COM_PROCESS_ERROR(hr);

	hr = pRepresent.NPC_New(2);
	KG_COM_PROCESS_ERROR(hr);

	hr = pRepresent.NPC_Walk(0);
	hr = pRepresent.NPC_MoveTo(0);

	pNPC0 = pRepresent.GetNPC(0);
	KG_PROCESS_ERROR(pNPC0);

	pNPC1 = pRepresent.GetNPC(1);
	KG_PROCESS_ERROR(pNPC1);

	pNPC2 = pRepresent.GetNPC(2);
	KG_PROCESS_ERROR(pNPC2);

	hr = pNPC0->DoMoveTo(D3DXVECTOR3(200.0f, 0.0f, 0.0f));
	KG_COM_PROCESS_ERROR(hr);

	hr = pNPC1->DoMoveTo(D3DXVECTOR3(3000.0f, 0.0f, 0.0f));
	KG_COM_PROCESS_ERROR(hr);

	hr = pRepresent.NPC_Talk(0);
	KG_COM_PROCESS_ERROR(hr);
#endif // 0
	return 1;
Exit0:
	return 0;
}

void FreeActionRelativeTable()
{
	if (g_pEngineManager)
	{
		g_pEngineManager->UnInit();
		g_pEngineManager = NULL;
	}
}

int TestAction()
{
	int nRetCode = 0;
	KActionMgr* pActionMgr = NULL;

	nRetCode = KActiveObjectMgr::GetSelf().Lookup(&pActionMgr, _T("ActionMgr"));
	KG_PROCESS_ERROR(nRetCode);

Exit0:
	return 1;
}

static LRESULT OnZoom(float fScale)
{
	float fStep = 100.0f;

	D3DXVECTOR3 vEye;
	D3DXVECTOR3 vLookAt;

	GetRepresent().GetCameraPosition(g_dwScene, vEye);
	GetRepresent().GetCameraLookAtPosition(g_dwScene, vLookAt);

	D3DXVECTOR3 N = vEye - vLookAt;
	float R = D3DXVec3Length(&N);
	D3DXVec3Normalize(&N, &N);

	vEye += fCameraSpeed * N * fScale;
	vLookAt += fCameraSpeed * N * fScale;

	GetRepresent().SetCameraPosition(g_dwScene, vEye);
	GetRepresent().SetCameraLookAtPosition(g_dwScene, vLookAt);

	return 1;
}

static LRESULT OnXZPlaneRotate(int nX, int nY)
{
#if 1
	D3DXVECTOR3 vEye;
	D3DXVECTOR3 vLookAt;
	D3DXVECTOR3 vUp;

	GetRepresent().GetCameraPosition(g_dwScene, vEye);
	GetRepresent().GetCameraLookAtPosition(g_dwScene, vLookAt);
	GetRepresent().GetCameraUpDirection(g_dwScene, vUp);

	D3DXMATRIX mtxView;
	D3DXMatrixLookAtLH(&mtxView, &vEye, &vLookAt, &vUp);

	float fX = (nX - s_nLastX) * fCameraSpeed * 0.03f;
	float fY = (nY - s_nLastY) * fCameraSpeed * 0.03f;

	D3DXVECTOR3 L(mtxView._11, mtxView._21, mtxView._31);
	D3DXVECTOR3 U(mtxView._12, mtxView._22, mtxView._32);

	D3DXVECTOR3 N = vEye - vLookAt;
	float const R = D3DXVec3Length(&N);

	N += -fX * L + fY * U;
	D3DXVec3Normalize(&N, &N);

	vEye = vLookAt + N * R;

	GetRepresent().SetCameraPosition(g_dwScene, vEye);
#else

	D3DXVECTOR3 vEye;
	D3DXVECTOR3 vLookAt;
	D3DXVECTOR3 vUp;

	GetRepresent().GetCameraPosition(g_dwScene, vEye);
	GetRepresent().GetCameraLookAtPosition(g_dwScene, vLookAt);
	GetRepresent().GetCameraUpDirection(g_dwScene, vUp);

	D3DXMATRIX mtxView;
	D3DXMatrixLookAtLH(&mtxView, &vEye, &vLookAt, &vUp);

	float fX = (nX - s_nLastX) * fCameraSpeed * 0.03f;
	float fY = (nY - s_nLastY) * fCameraSpeed * 0.03f;

	fX = fX < 0.16f ? fX : 0.16f;
	fY = fY < 0.16f ? fY : 0.16f;

	fX = fX > -0.16f ? fX : -0.16f;
	fY = fY > -0.16f ? fY : -0.16f;

	D3DXQUATERNION qView;
	D3DXQuaternionRotationYawPitchRoll(&qView, fX, fY, 0.0f);

	D3DXMatrixRotationQuaternion(&mtxView, &qView);

	D3DXVECTOR4 v4;

	D3DXVec3Transform(&v4, &vEye, &mtxView);
	vEye.x = v4.x;
	vEye.y = v4.y;
	vEye.z = v4.z;

	D3DXVec3Transform(&v4, &vLookAt, &mtxView);
	vLookAt.x = v4.x;
	vLookAt.y = v4.y;
	vLookAt.z = v4.z;

	GetRepresent().SetCameraPosition(g_dwScene, vEye);
	GetRepresent().SetCameraLookAtPosition(g_dwScene, vLookAt);

#endif
	return 1;
}

static LRESULT OnXZPlaneShift(int nX, int nY)
{
	D3DXVECTOR3 vEye;
	D3DXVECTOR3 vLookAt;
	D3DXVECTOR3 vUp;

	GetRepresent().GetCameraPosition(g_dwScene, vEye);
	GetRepresent().GetCameraLookAtPosition(g_dwScene, vLookAt);
	GetRepresent().GetCameraUpDirection(g_dwScene, vUp);

	D3DXMATRIX mtxView;
	D3DXMatrixLookAtLH(&mtxView, &vEye, &vLookAt, &vUp);

	float const fX = (nX - s_nLastX) * fCameraSpeed * 0.03f;
	float const fY = (nY - s_nLastY) * fCameraSpeed * 0.03f;

	D3DXVECTOR3 L(mtxView._11, 0, mtxView._31);
	D3DXVECTOR3 F(mtxView._13, 0, mtxView._33);

	D3DXVec3Normalize(&L, &L);
	D3DXVec3Normalize(&F, &F);

	D3DXVECTOR3 const vOffset = -fX * L + fY * F;

	vEye += vOffset;
	vLookAt += vOffset;

	GetRepresent().SetCameraPosition(g_dwScene, vEye);
	GetRepresent().SetCameraLookAtPosition(g_dwScene, vLookAt);

	return 1;
}

static LRESULT OnZAxisShift(int nY)
{
	D3DXVECTOR3 vEye;
	D3DXVECTOR3 vLookAt;
	D3DXVECTOR3 vUp;

	GetRepresent().GetCameraPosition(g_dwScene, vEye);
	GetRepresent().GetCameraLookAtPosition(g_dwScene, vLookAt);
	GetRepresent().GetCameraUpDirection(g_dwScene, vUp);

	D3DXMATRIX mtxView;
	D3DXMatrixLookAtLH(&mtxView, &vEye, &vLookAt, &vUp);

	float const fY = (nY - s_nLastY) * fCameraSpeed * 0.03f;

	D3DXVECTOR3 U(0, mtxView._22, mtxView._32);

	D3DXVec3Normalize(&U, &U);

	D3DXVECTOR3 const vOffset = -fY * U;

	vEye += vOffset;
	vLookAt += vOffset;

	GetRepresent().SetCameraPosition(g_dwScene, vEye);
	GetRepresent().SetCameraLookAtPosition(g_dwScene, vLookAt);

	return 1;
}


void InputThreadProc(void*) 
{ 
    g_hWnd = ::CreateDialog(NULL, MAKEINTRESOURCE(IDD_DIALOG1), NULL, NULL); 
    if (g_hWnd != NULL) 
    { 
        ::ShowWindow(g_hWnd, SW_SHOW); 
    } 
    else 
    { 
        printf("Failed to create dialog\n"); 
        bDone = true; 
        return; 
    } 

	InitAction();

    MSG msg; 
    while (true) 
    { 
        if (::PeekMessage(&msg, g_hWnd, 0, 0, PM_REMOVE)) 
        { 
            switch (msg.message)
			{
			case WM_KEYUP:
				{
					switch (msg.wParam)
					{
					case VK_ESCAPE:
						::DestroyWindow(g_hWnd); 
						bDone = true; 
						break; 
					case VK_LEFT:
						break;
					case VK_RIGHT:
						break;
					case VK_UP:
						break;
					case VK_DOWN:
						break;
					default:
						break;
					} 
				}
				break;
			case WM_MOUSEMOVE:
				{
					int const nX = LOWORD(msg.lParam); 
					int const nY = HIWORD(msg.lParam);

					if (msg.wParam & MK_MBUTTON)
					{
						if (::GetAsyncKeyState(VK_CONTROL))
							OnZAxisShift(nY);
						else
							OnXZPlaneShift(nX, nY);
					}
					else if (msg.wParam & MK_RBUTTON)
					{
						OnXZPlaneRotate(nX, nY);
					}

					s_nLastX = nX;
					s_nLastY = nY;
				}
				break;
			case 0x020A:// WM_MOUSEWHEEL:
				{
					float const fScale = ((short)HIWORD(msg.wParam)) > 0 ? -0.9f : 1.1f;
					OnZoom(fScale);
				}
				break;
			default:
				break;
            } 

			::TranslateMessage(&msg); 
            ::DispatchMessage(&msg); 
        } 
        else 
        { 
			TestAction();

			GetRepresent().Activate();
			g_pEngineManager->Render();

            ::Sleep(1000 / 16); 
        } 
    } 

	FreeActionRelativeTable();
} 

int _tmain(int argc, _TCHAR* argv[])
{
	int nRetCode = 0;

	g_InitRepresent();

    if (_beginthread(InputThreadProc, 0, 0) == -1) 
    { 
        printf("Failed to create thread"); 
        KG_PROCESS_ERROR(0); 
    } 

    // wait for the new thread to finish 
    while (!bDone) 
    { 
        // sleep 3 seonds 
        ::Sleep(3000); 
        printf("main thread running\n"); 
    } 

Exit0:
	g_ExitRepresent();
	return 0;
}

