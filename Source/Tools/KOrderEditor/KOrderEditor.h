#pragma once
#include <list>

// 队列结点的极坐标,半径单位为点,角度单位为弧度(-PI--PI)
struct KORDER_NODE
{
    float fRadius;
    float fAngel;
};

typedef std::list<KORDER_NODE> KORDER_LIST;


class KOrderEditor
{
    KOrderEditor();
public:
    BOOL Init(HINSTANCE hInstance, int nCmdShow);
    void UnInit();

    static KOrderEditor* GetInstance();

    void Run();

private:
    BOOL InitWindow(int nCmdShow);

    static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

    LRESULT OnCommond(UINT uMessage, WPARAM wParam, LPARAM lParam);
    LRESULT OnPaint(HDC hdc);
    LRESULT OnLMouseUp(WPARAM wParam, LPARAM lParam);
    LRESULT OnRMouseUp(WPARAM wParam, LPARAM lParam);
    LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);

    void DrawItems(HDC hdc);
    BOOL Open();
    BOOL Save();
    void SetFileDlg(OPENFILENAME& ofn);

    HINSTANCE m_hInstance;
    HWND      m_hWnd;
    int       m_nMaxWidth;
    int       m_nMaxHeight;
    int       m_nGridXCount;
    int       m_nGridYCount;
    int       m_nGridStartX;
    int       m_nGridStartY;
    int       m_nOrgX;
    int       m_nOrgY;
    int       m_nMouseX;
    int       m_nMouseY;

    BOOL      m_bViewMode;
    
    KORDER_LIST m_OrderList;
    KORDER_LIST m_ViewList;

    TCHAR m_szFileName[MAX_PATH];
};

#define WIN_MIN_W       1024
#define WIN_MIN_H       768

#define TITLE_HEIGHT    40
#define COMMENT_WIDTH   200
#define PROMPT_HEIGHT   200

#define GRID_WIDTH      32
#define GRID_HEIGHT     32

// "鼠标位置"显示的地方, 不是绝对位置
#define MOUSE_POS_X     400
#define MOUSE_POS_Y     120
