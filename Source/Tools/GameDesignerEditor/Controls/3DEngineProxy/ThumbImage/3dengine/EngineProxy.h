#pragma once
#include "stdafx.h"
#include "IEOther.h"
#include "IEEditor.h"

class EngineProxy
{
public:
	EngineProxy(void);
	~EngineProxy(void);

public:
	//初始化日至系统
	BOOL InitLog();	

	//初始化3d引擎
	BOOL Init3DEngine(unsigned uMode, unsigned int uFlag, HWND hBaseWindow, HWND hRenderWindow,const char szEngineRootPath[]);

	//反初始化3d引擎
	void UnInit3DEngine();

	//初始化运行时上下文
	BOOL InitContext();

	//初始化场景
	BOOL InitScene(HWND hwndOutput, LONG iOption);	

	//初始化模型编辑器
	BOOL InitModeEditor();

	//根据当前文件路径更新引擎根路径
	BOOL UpdateEngineRootPath(LPCSTR lpFileName);


public:
	
	//开始引擎
	BOOL Start(HWND hwd);

	//反初始化
	BOOL UnInit();

	//停止引擎
	BOOL Stop();

	//创建文件的缩略图，并返回是否成功
	BOOL CreateThumbnail(const SIZE bmSize, LPCTSTR lpFileName);

	//BOOL CreateGifFile(const SIZE bmSize, LPCTSTR lpFileName);

	BOOL CreateAnimationFile(const SIZE bmSize, LPCTSTR lpFileName, LPCTSTR lpAniFileName);
//
//private:
//	//判断文件是否是mesh| mdl 文件类型
//	BOOL	IsMeshMdlFile(LPCTSTR lpFileName);


	//变量们
	IEKG3DSceneModelEditor* m_pModelEditor ;
	HMODULE m_hEngineMod;
	IEKG3DEngineManager* m_pEngineManager;
	IEKG3DScene* m_pScene;
	IKG3DCamera* m_pCamera ;
	D3DXVECTOR3 m_CameraPos;
	INT m_nOutputWindowID;
	IEKG3DModel* m_pLastModel;
	HANDLE m_hThreadEngine;
	HANDLE m_hEngineInitFinishEvent;


	
};
