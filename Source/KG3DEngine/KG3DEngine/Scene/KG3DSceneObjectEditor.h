
#ifndef _KG3DSCENEOBJECTEDITOR_H_
#define _KG3DSCENEOBJECTEDITOR_H_

#include "KG3DSceneEditorBase.h"
//#include "FileType.h"
#include "File.h"
#include "KG3DSceneSceneEditor.h"

#include "IEEditor.h"//by dengzhihui Refactor 2006年11月15日

class KG3DRepresentObject;

class KG3DSceneObjectEditor : public KG3DSceneEditorBase, public IEKG3DSceneObjectEditor
{
	

public:
	//(by dengzhihui Refactor 2006年11月15日
	HRESULT GetRepresentObject(IEKG3DRepresentObject** pObj);
	HRESULT GetCurAniFrameTime(float* fCurAniFrameTime);
	HRESULT SetCurAniFrameTime(float fCurAniFrameTime);
	HRESULT GetBaseScene(IKG3DScene** ppScene);
	//)
	HRESULT OpenObject(int dwLogicID);
	HRESULT OpenObject(LPSTR pFileName);
	HRESULT SetCurModel(const char pcszModelField[]);

	HRESULT SetCurAnimt(const char pcszAnimtField[]);
	HRESULT PlayAnimation(const char pcszAnimtFile[]);
	HRESULT PlayAnimation(DWORD dwType, float fSpeed);
	HRESULT PauseAnimation(BOOL bPause);
	HRESULT SetAnimationType(DWORD dwType);
	HRESULT SetAnimationSpeed(float fSpeed);
	
	HRESULT SetAlpha(float fAlpha);
	HRESULT SetScalingg(D3DXVECTOR3* pScalVector);
	HRESULT BandObjectTemplate(ITabFile* pObjectTemplate, ITabFile* pLogicTable);
    HRESULT Init();
	HRESULT UnInit();

	HRESULT SetCurAniFrameTime(int nTime);
	INT		GetCurAniFrameTime();
	HRESULT SaveCurAniFrameTime();

	KG3DSceneObjectEditor(void);
	virtual ~KG3DSceneObjectEditor(void);

protected:
	HRESULT RenderForMainWnd(KG3DSceneOutputWnd* pWnd);
	virtual HRESULT RenderForEachWindow(KG3DSceneOutputWnd& wndCur);
	HRESULT FrameMove();

public:
	// Base Data
	KG3DRepresentObject* m_lpRepresentObject;
	LPD3DXFONT			 m_lpd3dFont;
	ITabFile*			 m_pObjectTemplate;
	ITabFile*			 m_pLogicTable;
	int					 m_nLogicID;

	// Animation Data
	DWORD				 m_dwAinPlayType;
	float				 m_fAniPlaySpeed;

	// frame move
	D3DXVECTOR3			 m_vNpcPosition;
	D3DXVECTOR3			 m_vNpcDirection;
	POINT				 m_pointPrevMoust;

	DWORD				 m_dwPrevTime;
	DWORD				 m_dwCurrTime;
	float				 m_fMoveSpeed;
	float			     m_fRotinRate;
	float				 m_fRotationY;
	float				 m_fDelayCount;
	float			     m_fFPS;
	
	KG3DTexture*		 m_pTexture;

	float				 m_fCurAniFrameTime;
protected:
	struct BindInfo
	{
		enum enu
		{
			FACE,
			LEFTHAND,
			RIGHTHAND,
			LEFTPELVIS,
			RIGHTPELVIS,
			SPINE0,
			SPINE1,
			COUNT
		};
		
		KG3DModel* m_Models[COUNT];
		~BindInfo();
		BindInfo();
	};

public:
	HRESULT Bind(DWORD dwPartIndex, 
			LPCSTR strFileName,
			BOOL bUnBindOrg = FALSE);

	HRESULT UnBind(DWORD dwPartIndex);
	
	BindInfo m_BindInfo;
};

#endif //_KG3DSCENEOBJECTEDITOR_H_