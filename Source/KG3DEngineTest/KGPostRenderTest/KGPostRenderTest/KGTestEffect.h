//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGTestEffect.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-7-28 10:00
//  Comment     :   封装单个屏幕特效测试用例，根据用例的控制数据进行用例的执行，先加载测试场景，
//					收集渲染前和渲染后的测试数据，最后输出结果
//					
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KGTestEffect_H_
#define _KGTestEffect_H_

#include "KGTestCommon.h"
#include "KG3DInterface.h"
#include "IEKG3DSceneOutputWnd.h"

class KGTestEffect
{
public:
	KGTestEffect();
	~KGTestEffect();

	int  Init(IKG3DEngineManager*	cpEngineMgr,
			  const HWND			chRenderWindow,
			  const TCHAR			cszResultFile[],
			  const TCHAR			cszMapName[],
			  const DWORD			cdwEffectType,
			  const int				cnTotalFrame);
	int  Run();
	void UnInit();

	int  AddCaseData(int nTestFrame,
		D3DXVECTOR3 vCameraPos,
		D3DXVECTOR3 vLookAtPos);
	void SetDefaultCameraPos(D3DXVECTOR3 vCameraPos) {m_CaseData.vDefaultCameraPos = vCameraPos;}
	void SetDefaultLookAtPos(D3DXVECTOR3 vLookAtPos) {m_CaseData.vDefaultLookAtPos = vLookAtPos;}
 	bool HasEffect(const DWORD dwEffectType) {return !!(m_dwEffectType & dwEffectType);}
	string GetCaseName() {return m_strCaseName;}

	struct FrameData						//帧数据结构
	{
		int nFrameCount;					//当前帧数
		float nAverageFPS;					//平均帧数
		float nFPS;							//本帧FPS
		DWORD dwBeginTime;					//本帧开始时间
		DWORD dwLastTime;					//渲染延续时间
		DWORD dwEndTime;					//本帧结束时间
	};

	struct CaseData							//测试数据结构
	{
		int nDefaultTotalTestFrame;		//默认测试总帧数
		D3DXVECTOR3 vDefaultCameraPos;	//默认摄像机位置
		D3DXVECTOR3 vDefaultLookAtPos;	//默认摄像机朝向
		vector<int> vecTestFrame;			//需要收集帧数据的帧数
		vector<D3DXVECTOR3> vecCameraPos;	//摄像机位置
		vector<D3DXVECTOR3> vecLookAtPos;	//摄像机朝向
	};
private:
	//渲染帧处理
	int BeforeFrame();
	int OnFrameMove();
	int AfterFrame();
	//////////////////////////////////////////////////////////////////////////

	//检查设置屏幕特效的情况
	int TestSetEffectProcess();

	//设置屏幕特效
	int DisableAllEffect();
	int EnableEffect(bool bEnable);
	//////////////////////////////////////////////////////////////////////////

	//组合用例名称
	int MakeupCaseName();

	//分析帧数据
	int AnalyseFrameData();

	//输出每个测试帧的信息
	int OutputResult();

	//场景管理
	int LoadScene();
	int SetupScene();
	int RemoveScene();
	//////////////////////////////////////////////////////////////////////////

private:
	string m_strCaseName;							//测试用例名称
	TCHAR m_szTestSceneName[MAX_PATH];				//测试场景名称
	TCHAR m_szResultFile[MAX_PATH];					//数据结果输出文件
	TCHAR m_szResultPath[MAX_PATH];					//结果输出目录

	int m_nCurrentFrame;							//当前的帧数
	DWORD m_dwTestingBeginTime;						//测试的开始时间
	FrameData m_CurrentFrameData;					//当前帧数据
	CaseData m_CaseData;							//测试用例配置数据
	DWORD m_dwEffectType;							//测试的特效类型
	int m_nTestFrameIndex;							//测试的帧索引

	vector<string> m_vecTestProcessResult;			//设置特效流程验证结果

	IKG3DEngineManager* m_pEngineMgr;
	IKG3DScene* m_pScene;
	IEKG3DSceneOutputWnd* m_pCurOutputWnd;
	HWND m_hRenderWindow;
	ITabFile* m_pFile;								//输出结果TAB文件指针
	static int m_nResultLine;						//输出结果的行数

};

#endif