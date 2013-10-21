#define _WIN32_WINNT 0x0500 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <Windowsx.h>
#include <zmouse.h>
#include <WinUser.h>
#include "KApplication.h"
#include "WinProc.h"
#include "KCamera.h"
#include "KMainPlayer.h"
#include "KScene.h"
#include "MathTools.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

KApplication g_App;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, int nShowCmd)
{
    BOOL bRetCode = false;

    AllocConsole();

    bRetCode = g_App.Setup(hInstance, nShowCmd);
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_Scene.Setup();
    KG_PROCESS_ERROR(bRetCode);

    bRetCode = g_Player.Setup();
    KG_PROCESS_ERROR(bRetCode);

    g_App.Run();

Exit0:
    FreeConsole();
    g_Scene.Clear();
    g_App.Clear();
    return 0;
}
