//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGSceneViewer.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-7-28 10:00
//  Comment     :   场景浏览器，实现了场景的加载和渲染，响应键盘鼠标控制摄像机在场景中游走，和对场景设置屏幕特效的功能
//
//////////////////////////////////////////////////////////////////////////////////////

#include "KGTestCommon.h"
#include "KG3DInterface.h"
#include "KG3DSceneDataInterface.h"
#include "IEEditor.h"

const TCHAR TEST_PROJECT_NAME[] = "KGSceneViewer";

class KGSceneViewer
{
public:
	KGSceneViewer(void);
	~KGSceneViewer(void);

	int  Init(HWND hWindow);
	int  LoadScene(const TCHAR cszMapPathName[]);
	int  Run();
	void UnInit();
	int  KeyListener(UINT_PTR uKeyValue);
	int  OnMouseMove(WPARAM wParam, LPARAM lParam);
	int  SetMouseState(UINT uMsg, LPARAM lParam);

private:
	IKG3DEngineManager*	m_pEngineMgr;
	HMODULE m_hEngineDLL;
	HWND m_hWindow;
	IKG3DScene* m_pScene;
	IEKG3DSceneOutputWnd* m_pCurOutputWnd;

	bool m_bEnableEffect;			//标记特效状态
	bool m_bRButton;				//鼠标右键状态(true:down false:up)
	POINT m_ptLastMousePos;
	POINT m_ptCurrentMousePos;
	TCHAR m_szSceneInfo[1024];		//场景信息(目前只收集摄像机的位置和朝向信息)
	TCHAR m_szResultPath[MAX_PATH];	//截图存放目录
	int m_nPictureIndex;			//截图索引，用于生成不重复的图片文件名
	bool m_bKMemoryInited;			//内存管理初始化标记
};
