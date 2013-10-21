#pragma once

#include "SO3Represent/SO3RepresentInterface.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "SO3UI/ISO3UI.h"
#include "SO3World/IKGSO3WorldClient.h"

interface IEKG3DSceneEditorBase;
class CInputProxy;

extern CDllHolder g_ModuleRepresent;
extern CDllHolder g_ModuleUI;
//extern IKSO3RepresentInterface* g_pRepresent;
extern IKSO3RepresentHandler* g_pRepresentHandler;
extern ISO3UI* g_pUI;
//extern BOOL g_bUseSO3Logical;
extern BOOL g_bSO3LogicInited;
extern BOOL g_bKG3DEngineInited;


BOOL InitUI();
void UnInitUI();
BOOL InitRepresent();
void UnInitRepresent();
void InitKeyState(IEKG3DSceneEditorBase* pBase, HWND hWnd, CInputProxy* pInputProxy);
int ShowProgress(LPCTSTR pName, float fPencent);
void BeforeChange(KSceneEditorDocLogical* m_lpLogicalData);
void ExchangeLogicalData(LPCTSTR pFileName, KScene* lpLogicalScene, KSceneEditorDocLogical* m_lpLogicalData);
void ExchangeNpc(KScene *pLogicalScene, KSceneEditorDocLogical* m_lpLogicalData);
void ExchangeDoodad(KScene *pLogicalScene, KSceneEditorDocLogical* m_lpLogicalData);
void ExchaneNpcRefreshPointSet(LPCTSTR pFileName,LPCTSTR pMapName, KSceneEditorDocLogical* m_lpLogicalData);
void ExchaneDoodadRefreshPointSet(LPCTSTR pFileName,LPCTSTR pMapName, KSceneEditorDocLogical* m_lpLogicalData);
void ExchangeAIGroup(LPCTSTR pFileName,LPCTSTR pMapName, KSceneEditorDocLogical* m_lpLogicalData);
void ExchangeWayPointSet(LPCTSTR pFileName,LPCTSTR pMapName, KSceneEditorDocLogical* m_lpLogicalData);
void ExchangeTrafficPointSet(LPCTSTR pFileName, KSceneEditorDocLogical* m_lpLogicalData);

