#pragma once

#include <conio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <list>
#include <set>
#include <float.h>

#define KG_PROCESS_ERROR(C) \
    do                      \
    {                       \
        if (!(C))           \
            goto Exit0;     \
    } while (false)

#define KG_COM_RELEASE(p)   \
    do                      \
    {                       \
        if (p)              \
        {                   \
            (p)->Release(); \
            (p) = NULL;     \
        }                   \
    } while (false)


#define KG_DELETE(p)        \
    do                      \
    {                       \
        if (p)              \
        {                   \
            delete (p);     \
            (p) = NULL;     \
        }                   \
    } while (false)


#define KG_DELETE_ARRAY(p)  \
    do                      \
    {                       \
        if (p)              \
        {                   \
            delete[] (p);   \
            (p) = NULL;     \
        }                   \
    } while (false)

#define KEEP_RANGE(v, MinV, MaxV)  \
    do                      \
    {                       \
        if ((v) < (MinV))   \
            (v) = (MinV);   \
        if ((v) > (MaxV))   \
            (v) = (MaxV);   \
    } while (false)

#define GAME_FPS        12

struct KTriangleEx;

struct KVertex 
{
    KVertex() : vPos(0.0f, 0.0f, 0.0f), vNormal(0.0f, 0.0f, 0.0f) {};

    D3DXVECTOR3             vPos;
    D3DXVECTOR3             vNormal;
};

struct KVertexEx : KVertex 
{
    KVertexEx() : fCandiCost(FLT_MAX), pCandiCollapse(NULL), bFix(false) {};

    std::set<KVertexEx*>    Neighbors;
    std::set<KTriangleEx*>  Faces;

    float                   fCandiCost;
    KVertexEx*              pCandiCollapse;
    BOOL                    bFix;
};

struct KTriangle
{
    int         nA;
    int         nB;
    int         nC;
};

struct KTriangleEx 
{
    KTriangleEx() : pVertexA(NULL), pVertexB(NULL), pVertexC(NULL), vNormal(0.0f, 0.0f, 0.0f) {};

    KVertexEx*  pVertexA;
    KVertexEx*  pVertexB;
    KVertexEx*  pVertexC;
    D3DXVECTOR3 vNormal;

    void UpdateNormal()
    {
        D3DXVECTOR3 vB2A = pVertexB->vPos - pVertexA->vPos;
        D3DXVECTOR3 vC2B = pVertexC->vPos - pVertexB->vPos;

        D3DXVec3Cross(&vNormal, &vC2B, &vB2A);
        D3DXVec3Normalize(&vNormal, &vNormal);
    };
};

struct KDrawInfo 
{
    D3DXVECTOR3 vOffset;
    int         nVertexCount;
    int         nVertexSize;
    DWORD       dwVertexFVF;
    void*       pvVetexData;
    int         nTriangleCount;
    void*       pvTriangleData;
};

typedef std::list<KDrawInfo> KDrawList;

class KApplication
{
public:
    KApplication(void);
    ~KApplication(void);

    BOOL Setup(HINSTANCE hInstance, int nCmdShow);
    void Clear();

    void Run();

    BOOL SetupWindow(int nCmdShow);

    BOOL ResetDevice();

    void OnMouseMove(WPARAM wParam, LPARAM lParam);
    void ProcessMessage();
    void FrameUpdate();

private:
    BOOL Draw(const KDrawInfo& crDrawInfo);
    BOOL SetLight();
private:
    HINSTANCE           m_hInstance;
    HWND                m_hWnd;
    BOOL                m_bRunFlag;
    DWORD               m_dwStartTime;
    DWORD               m_dwFrame;
    IDirect3DDevice9*   m_piDevice;
    int                 m_nPreMouseX;
    int                 m_nPreMouseY;
};

extern KApplication g_App;