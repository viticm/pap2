#pragma once

#define PLAYER_MODEL_LINE_COUNT     6
#define PLAYER_MODEL_VERTEX_COUNT   (PLAYER_MODEL_LINE_COUNT + 2)
#define PLAYER_MODEL_TRIANGLE_COUNT (PLAYER_MODEL_LINE_COUNT * 2)

struct KMainPlayer
{
    KMainPlayer(void);
    ~KMainPlayer(void);

    BOOL Setup();

    void FrameUpdate();
    void SpeedUp();
    void SpeedDown();
    void UpdatePlayer();
    void Draw(IDirect3DDevice9* piDevice);
    
    D3DXVECTOR3 m_vPosition;
    float       m_fSpeed;
    KVertexEx   m_Vertex[PLAYER_MODEL_VERTEX_COUNT];
    KTriangleEx m_Triangle[PLAYER_MODEL_TRIANGLE_COUNT];
    float       m_fRotateAngel;
};

extern KMainPlayer g_Player;
