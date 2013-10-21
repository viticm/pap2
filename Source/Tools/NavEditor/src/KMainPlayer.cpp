#define _WIN32_WINNT 0x0500 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <Windows.h>
#include <Windowsx.h>
#include <zmouse.h>
#include <WinUser.h>
#include "KApplication.h"
#include "KScene.h"
#include "XScene.h"
#include "KMainPlayer.h"
#include "KCamera.h"
#include "MathTools.h"

KMainPlayer g_Player;

#define PLAYER_MODEL_SIZE   0.5f

KMainPlayer::KMainPlayer(void)
{
    m_vPosition     = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    m_fSpeed        = 5.0f;
    m_fRotateAngel  = 0.0f;
}

KMainPlayer::~KMainPlayer(void)
{
}

BOOL KMainPlayer::Setup()
{
    // 上面的顶点
    m_Vertex[0].vPos = D3DXVECTOR3(0.0f, PLAYER_MODEL_SIZE, 0.0f);
    
    // 中间的多边形
    for (int i = 0; i < PLAYER_MODEL_LINE_COUNT; i++)
    {
        float fAngel = D3DX_PI * 2 / PLAYER_MODEL_LINE_COUNT * i;

        m_Vertex[1 + i].vPos.x = PLAYER_MODEL_SIZE * cosf(fAngel);
        m_Vertex[1 + i].vPos.z = PLAYER_MODEL_SIZE * sinf(fAngel);
        m_Vertex[1 + i].vPos.y = 0.0f;
    }

    // 下面的顶点
    m_Vertex[PLAYER_MODEL_LINE_COUNT + 1].vPos = D3DXVECTOR3(0, -PLAYER_MODEL_SIZE, 0);

    // 上面的三角形
    for (int i = 0; i < PLAYER_MODEL_LINE_COUNT; i++)
    {
        m_Triangle[i].pVertexA = &m_Vertex[1 + i];
        m_Triangle[i].pVertexB = &m_Vertex[1 + (i + 1) % PLAYER_MODEL_LINE_COUNT];
        m_Triangle[i].pVertexC = &m_Vertex[0];
    }

    // 下面的三角形
    for (int i = 0; i < PLAYER_MODEL_LINE_COUNT; i++)
    {
        m_Triangle[PLAYER_MODEL_LINE_COUNT + i].pVertexA = &m_Vertex[1 + i];
        m_Triangle[PLAYER_MODEL_LINE_COUNT + i].pVertexB = &m_Vertex[PLAYER_MODEL_LINE_COUNT + 1];
        m_Triangle[PLAYER_MODEL_LINE_COUNT + i].pVertexC = &m_Vertex[1 + (i + 1) % PLAYER_MODEL_LINE_COUNT];
    }

    UpdateNormal(
        m_Vertex, PLAYER_MODEL_VERTEX_COUNT,
        m_Triangle, PLAYER_MODEL_TRIANGLE_COUNT
    );

    return true;
}

void KMainPlayer::FrameUpdate()
{
    BOOL        bRetCode        = false;
    int         nMoveX          = 0;
    int         nMoveZ          = 0;
    float       fHeight         = 0.0f;
    D3DXVECTOR3 vNewPostion     = m_vPosition;
    D3DXVECTOR3 vOldPostion     = m_vPosition;
    D3DXVECTOR3 vMoveVector;
    D3DXVECTOR3 vMax;
    D3DXVECTOR3 vTop;

    if (GetAsyncKeyState('A'))
    {
        nMoveX -= 1;
    }

    if (GetAsyncKeyState('D'))
    {
        nMoveX += 1;
    }

    if (GetAsyncKeyState('W'))
    {
        nMoveZ += 1;
    }

    if (GetAsyncKeyState('S'))
    {
        nMoveZ -= 1;
    }

    if (nMoveX)
    {
        vNewPostion += (float)nMoveX * g_Camera.m_vRight * m_fSpeed / GAME_FPS * 3;
    }

    if (nMoveZ)
    {
        D3DXVECTOR3 vForward(g_Camera.m_vLook.x, 0.0f, g_Camera.m_vLook.z);

        D3DXVec3Normalize(&vForward, &vForward);

        vNewPostion += (float)nMoveZ * vForward * m_fSpeed / GAME_FPS * 3;
    }

    g_Scene.GetMaxCoordinate(&vMax);

    bRetCode = g_Scene.GetHeight(vNewPostion, &fHeight);
    KG_PROCESS_ERROR(bRetCode);

    vNewPostion.y = fHeight;

    vMoveVector = vOldPostion - vNewPostion;

    m_vPosition = vNewPostion;

    m_fRotateAngel += D3DX_PI * 2 / (GAME_FPS * 4);
    if (m_fRotateAngel > D3DX_PI * 2)
    {
        m_fRotateAngel -= D3DX_PI * 2;
    }

Exit0:
    KEEP_RANGE(m_vPosition.x, 0.0f, vMax.x);
    KEEP_RANGE(m_vPosition.z, 0.0f, vMax.z);
    g_Scene.UpdateViewArea(m_vPosition.x, m_vPosition.z);
}

void KMainPlayer::SpeedUp()
{
    m_fSpeed *= 1.1f;
}

void KMainPlayer::SpeedDown()
{
    m_fSpeed *= 0.9f;

    if (m_fSpeed < 0.1f)
    {
        m_fSpeed = 0.1f;
    }
}

void KMainPlayer::Draw(IDirect3DDevice9* piDevice)
{
    BOOL                    bRetCode        = false;
    IDirect3DVertexBuffer9* piVertexBuffer  = NULL;
    IDirect3DIndexBuffer9*  piIndexBuffer   = NULL;
    void*                   pvBuffer        = NULL;
    D3DXMATRIX              Position;
    D3DXMATRIX              Rotation;
    D3DXMATRIX              Matrix;

    bRetCode = piDevice->CreateVertexBuffer(
        sizeof(KVertex) * PLAYER_MODEL_VERTEX_COUNT, D3DUSAGE_DYNAMIC,
        VERTEX_FVF, D3DPOOL_DEFAULT, &piVertexBuffer, 0
    );
    KG_PROCESS_ERROR(!FAILED(bRetCode));

    bRetCode = piVertexBuffer->Lock(0, 0, &pvBuffer, D3DLOCK_DISCARD);
    KG_PROCESS_ERROR(!FAILED(bRetCode));

    for (int i = 0; i < PLAYER_MODEL_VERTEX_COUNT; i++)
    {
        KVertex* pV = (KVertex*)pvBuffer + i;
        pV->vPos    = m_Vertex[i].vPos;
        pV->vNormal = m_Vertex[i].vNormal;
    }
    piVertexBuffer->Unlock();

    bRetCode = piDevice->CreateIndexBuffer(
        sizeof(KTriangle) * PLAYER_MODEL_TRIANGLE_COUNT, D3DUSAGE_DYNAMIC,
        D3DFMT_INDEX32, D3DPOOL_DEFAULT, &piIndexBuffer, 0
    );
    KG_PROCESS_ERROR(!FAILED(bRetCode));

    bRetCode = piIndexBuffer->Lock(0, 0, &pvBuffer, D3DLOCK_DISCARD);
    KG_PROCESS_ERROR(!FAILED(bRetCode));

    for (int i = 0; i < PLAYER_MODEL_TRIANGLE_COUNT; i++)
    {
        KTriangle* pT = (KTriangle*)pvBuffer + i;
        pT->nA = (int)(m_Triangle[i].pVertexA - m_Vertex);
        pT->nB = (int)(m_Triangle[i].pVertexB - m_Vertex);
        pT->nC = (int)(m_Triangle[i].pVertexC - m_Vertex);
    }
    piIndexBuffer->Unlock();

    D3DXMatrixTranslation(&Position, m_vPosition.x, m_vPosition.y + PLAYER_MODEL_SIZE, m_vPosition.z);
    D3DXMatrixRotationAxis(&Rotation, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), m_fRotateAngel);
    D3DXMatrixMultiply(&Matrix, &Rotation, &Position);

    piDevice->SetTransform(D3DTS_WORLD, &Matrix);

    piDevice->SetStreamSource(0, piVertexBuffer, 0, sizeof(KVertex));
    piDevice->SetIndices(piIndexBuffer);
    piDevice->SetFVF(VERTEX_FVF);

    piDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(0, 60, 200));

    piDevice->SetRenderState(D3DRS_LIGHTING, true);
    piDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    piDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, PLAYER_MODEL_VERTEX_COUNT, 0, PLAYER_MODEL_TRIANGLE_COUNT);

    piDevice->SetRenderState(D3DRS_LIGHTING, false);
    piDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    piDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, PLAYER_MODEL_VERTEX_COUNT, 0, PLAYER_MODEL_TRIANGLE_COUNT);

Exit0:
    KG_COM_RELEASE(piVertexBuffer);
    KG_COM_RELEASE(piIndexBuffer);
    return;
}
