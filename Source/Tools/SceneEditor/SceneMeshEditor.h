#pragma once
//#include "scene.h"
#include "sceneeditbase.h"

#include ".\meshmeshmaterialtool.h"
#include ".\meshcolorpick.h"
#include ".\meshmeshanimation.h"
#include "MeshMeshTool.h"
#include "MeshCombobox.h"
#include "MeshMeshSelect.h"
//#include "kdlg_ps_main.h"
//#include "KDlg_SpeedTree.h"
#include "MeshPropertySet.h"

class KSceneModelEditorSFXBillBoardDialog;
class KSceneModelEditorSFXBladeDialog;
class KSceneModelEditorSFXSpeedTreeDialog;
class KSceneModelEditorSFXSceneSFXDialog;

class KSceneModelEditor :
	public KSceneEditBase
{
public:
	BOOL                   m_bShowGrid;
	BOOL                   m_bShowBone;
	BOOL                   m_bShowSocket;
	BOOL                   m_bMoveMesh;

	LPMODEL                m_lpCurMesh;
	POINT                  m_LastMousePos;

	/*ModelTools::KModelToolMaterial*     m_lpMaterialTool;
	ModelTools::KModelToolColorPick*    m_lpColorPick;
	ModelTools::KModelToolAnimation*    m_lpAnimationTool;
	ModelTools::KModelToolMesh*         m_lpMeshTool;*/
	//ModelTools::KModelToolSelect*       m_lpMeshSelect;
	
	//ModelTools::KModelToolSFX*      m_lpMeshPropertySet;
	//ModelUI::KModelUIBase*          m_lpWinBase;


public:
	virtual HRESULT ShowProperty(BOOL bShow);
	virtual HRESULT OnSelectChanged();
	virtual HRESULT Edit_Select();
	HRESULT Acquire();
	void    OpenSubdvMesh();

	HRESULT FrameMove();
	HRESULT SetCurMesh(LPMODEL pMesh);
    LPMODEL GetCurMesh();
	HRESULT Initialize(PVOID pSceneTable);
	HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>
	//void    ONMOUSEMOVE(WPARAM wParam, LPARAM lParam);

	KSceneModelEditor(void);
	virtual ~KSceneModelEditor(void);
	HRESULT Render(void);
	HRESULT OnChangeMaterialColor(void);
private:
	//PropertyBase::KDlg_PS_Main* m_pPS;
	KSceneModelEditorSFXBillBoardDialog* m_lpDialogSFXBillBoard;
	KSceneModelEditorSFXBladeDialog*     m_lpDialogSFXBlade;
	KSceneModelEditorSFXSpeedTreeDialog* m_lpDialogSFXSpeedTree;
	KSceneModelEditorSFXSceneSFXDialog*  m_lpDialogSFXSceneSFX;
};
