// SceneEditorDoc.h : interface of the KSceneEditorDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEEDITORDOC_H__5A6A5499_B841_4F9B_9394_9B416D5AF0AB__INCLUDED_)
#define AFX_SCENEEDITORDOC_H__5A6A5499_B841_4F9B_9394_9B416D5AF0AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KEditorDocumentBase.h"
#include "KDlgLogicSaveProgress.h"
#include "KSceneSceneEditorSaveOption.h"
#include "KGPhysicsParamEditorDlg.h"
#include "kscenesceneeditordialoglogical.h"

#include "IETools.h"   //by dengzhihui Refactor 2006年11月2日
#include "IEEditor.h"	 //by dengzhihui Refactor 2006年11月2日

class KScene;
class IEKG3DSceneSceneEditor;
class KSceneEditorDocLogical;
//class CS3LogicForm;
class KSceneSceneEditorDialogLogical;

class KLogicalSceneConnection : public IELogicalSceneConnection
{
public:
	KScene* m_lpLogicalScene;
	_KG3DLogicalRegionInfo* m_lpLogicalRegionInfo;
	virtual HRESULT BindLogicalScene(KScene* pScene)
	{
		m_lpLogicalScene = pScene;
		return S_OK;
	}
	virtual BOOL CheckObstacle(int nXCell, int nYCell, int nLayer);
	virtual KRegion* GetRegion(int nRegionX, int nRegionY);
	virtual HRESULT FillLogicalCellsInfo(
		D3DXVECTOR3 A,int X,int Z,int& nCellCount,KCell* pCell,IELogicalVertex* pLLine, IELogicalVertex* pLFace, 
		BOOL bTerrian,BOOL bItem, BOOL bSelected, int nMaxCount); 
	virtual KCell* GetCellOfRegion(KRegion* pRegion, INT nIndex);
	virtual FLOAT GetLogicalHeight(const D3DXVECTOR3& vLogicalCoord, size_t* pOrderOfCell);
	virtual KCell* GetCurOrLowerObstacle(const KRegion& regionCur, int nXCell, int nYCell, int nZ, size_t* pOrderOfCell);
	virtual KCell* GetNextCell(const KCell* pCell, BOOL bIgnoreDynamicSwitch);
	virtual KCell* GetLowestObstacle(const KRegion& regionCur, int nXCell, int nYCell);
	virtual POINT LogicalToLogicalRegionIndex( FLOAT fLogicalX, FLOAT fLogicalY );

	KLogicalSceneConnection()
	{
		m_lpLogicalScene = NULL;
		m_lpLogicalRegionInfo = NULL;
	}

	~KLogicalSceneConnection()
	{
		m_lpLogicalScene = NULL;
		SAFE_DELETE_ARRAY(m_lpLogicalRegionInfo);
	}
	HRESULT CreateLogicalRegionInfo();
	
};

class KSceneEditorDoc : public CDocument , public KEditorDocumentBase
{
protected: // create from serialization only
	KSceneEditorDoc();
	DECLARE_DYNCREATE(KSceneEditorDoc)

	// Attributes
public:

	KGPhysicsParamEditorDlg* m_lpPhysiscSetting;

	KSceneEditorDocLogical* m_lpLogicalData;

	KLogicalSceneConnection m_LogicalConnection;
	KSceneSceneEditorDialogLogical* m_lpLogicalform;
// Operations
public:
	void ImportOldWayPointSet();

	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);
	HRESULT TransformatCell();
	BOOL OnSaveDocument(LPCTSTR lpszPathName);
	HRESULT SaveAllCellRest();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual int  OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	void ExchangeLogicalData(LPCTSTR pFileName);

	void LoadRepresentNPC();
	void LoadRepresentDoodad();
	void LoadRepresentWayPoint();
	void LoadRepresentTrafficPoint();
	void LoadRepresentRefreshPoint();
	//HRESULT SetScene(KSceneSceneEdit* pScene);
	virtual ~KSceneEditorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
    HRESULT TransformAllHeightToCellHeight(IEKG3DSceneSceneEditor* pEditorA,KScene* pLogicalScene);

protected:

	void BeforeChange();

	HRESULT ExchangeNpcReliveSetting(LPTSTR pFileName);
	HRESULT ExchangeDoodadReliveSetting(LPTSTR pFileName);
	HRESULT ExchangeNpcRandomSetting(LPTSTR pFileName,LPTSTR pDir);

	void ExchangeAIGroup(LPCTSTR pFileName,LPCTSTR pMapName);
	void ExchangeWayPointSet(LPCTSTR pFileName,LPCTSTR pMapName);
	void ExchangeDoodad(KScene *pLogicalScene);
	void CalcDoodadObstacleGroupIdIntoRepresentObject(); // 计算doodad的动态障碍的group id
	void ExchangeNpc(KScene *pLogicalScene);
	void ExchaneDoodadRefreshPointSet(LPCTSTR pFileName,LPCTSTR pMapName);
	void ExchaneNpcRefreshPointSet(LPCTSTR pFileName,LPCTSTR pMapName);
	void ExchangeTrafficPointSet(LPCTSTR pFileName);
	//void ExchangeTrafficPoint(LPCTSTR pFileName,LPCTSTR pMapName);


private:
	HRESULT NewLogicalScene();
	HRESULT AttachLogicalSceneTo3DScene(KScene* pLogicalScene);
	IEKG3DSceneLogicalEditorBase* m_lpLogicEditor;
	
	KSceneSceneEditorSaveOption m_dlgSaveOption;

//	KScene *m_lpLogicalScene;

// Generated message map functions
protected:
	//{{AFX_MSG(KSceneEditorDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPhysiscsetting();

	HRESULT ClearAllTrap();
	afx_msg void OnClearalltrap();

	IEKG3DSceneLogicalEditorBase* GetLogicalScene() {return m_lpLogicEditor;}
};

int ShowProgress(LPCTSTR pName,float fPencent);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENEEDITORDOC_H__5A6A5499_B841_4F9B_9394_9B416D5AF0AB__INCLUDED_)
