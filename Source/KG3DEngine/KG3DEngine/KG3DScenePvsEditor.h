
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DScenePvsEditor.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2008-10-25 17:27:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DSCENEPVSEDITOR_H_
#define _KG3DSCENEPVSEDITOR_H_

#include "KG3DScenePvsEditor.h"
#include "KG3DSceneEditorBase.h"
#include "IEKG3DSceneSFXEditor.h"
#include "KG3DRepresentNPC.h"

#define MAX_PLACE_OBJ_HISTORY_SIZE 128

class KG3DRepresentObjectPVS;
class KG3DScenePointLightRender;
class KG3DLightMapBaker;

class KG3DScenePvsEditor : public KG3DSceneEditorBase, public IEKG3DScenePvsEditor
{
public :
    friend class KG3DLightMapBaker;
    /* render */
	virtual HRESULT RenderForEachWindow(KG3DSceneOutputWnd& wndCur);
	HRESULT RenderForMainWindow(KG3DSceneOutputWnd& wndCur);

    HRESULT RenderObjViewWindow(KG3DSceneOutputWnd* pWnd);
    HRESULT FrameMove();

    
    /* editor pvs */
    HRESULT OpenPvs(LPCTSTR fileName);
    HRESULT NewPvs();
    HRESULT SavePvs(LPCTSTR fileName);


    /* get other scene, for editor */
    virtual IEKG3DScenePvsEditor* GetScenePvsEditor() { return this;          }
    virtual IEKG3DScene* GetScene()                   { return this;          }
    virtual IEKG3DSceneEditorBase* GetSceneEditBase() { return this;          }
    virtual IEKG3DSceneSelectBase* GetSelectBase()    { return this;          }

    
    /* get current edit pvs object*/
    virtual IEKG3DRepresentPVS* GetPvs()              { return m_pPvs;        }       
	virtual ULONG GetEditState()                      { return KG3DSceneEditorBase::GetEditState(); }	


    /* */
    virtual void MsgOnMouseWheel(WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>
    virtual HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);//<PROCESS_MESSAGE>
    virtual int DeleteSelectedObject();
    virtual HRESULT SetHandObject(LPCTSTR fileName);
    virtual HRESULT SetEditState(DWORD dwState);
    virtual void SetCoordMode(KG3DCOORD coord);
    virtual void SetPlaceProtalMode(DWORD dwMode)    { m_dwPlacePortalMode = dwMode; }
    virtual void RegistEventCallBack(IPvsEditorEventCallback* pEvent);
    virtual void UnRegistEventCallBack(IPvsEditorEventCallback* pEvent);
    virtual void SelectObject(IEKG3DRepresentObject* pObj);
    virtual void DeleteObject(IEKG3DRepresentObject* pObj);
	virtual IEKG3DRepresentObject* GetFirstSelectedObj();
	//virtual HRESULT SelectEnd(D3DVIEWPORT9& Viewport,D3DXMATRIX& Projection,D3DXMATRIX& View);//2009-1-22´úÂëÒÆ¶¯µ½OnReceiveMessage
    virtual HRESULT PasteSelection();
    //virtual HRESULT GetStandHeight(DWORD& dwCount,D3DXVECTOR3 *pInter,D3DXVECTOR3& vIn,DWORD dwMaxCount);
    virtual void EnablePMode(BOOL bEnable)           { m_bRunMode = !!bEnable; }
    virtual BOOL IsPMode()                           { return m_bRunMode;      }
    virtual void EnableCull(BOOL bCull);
    virtual void OnCommand(enumCommandType eCommandType);

    virtual HRESULT Init();
    virtual HRESULT UnInit();

    virtual HRESULT GetFloor(const D3DXVECTOR3* pvInput, FLOAT* pfRet);
	virtual void __stdcall OnReceiveMessage(const KG3DMessage& Message , IKG3DMessageSenderBase* pSender);


	virtual HRESULT BakeLightMap(BOOL bAllModel,BOOL bComputeDirectLight,BOOL bComputeIndirectLight);
	virtual HRESULT ComputePhotons();
    KG3DScenePvsEditor();
    virtual ~KG3DScenePvsEditor();


	virtual BOOL GetEnableShowLightSign();
	virtual void SetEnableShowLightSign(BOOL bEnable);

	virtual BOOL GetEnableShowChessBoradLightmap();
	virtual void SetEnableShowChessBoradLightmap(BOOL bEnable);
protected :

    /* draw accessorial information */
    void DrawGroundGrid();
    void DrawInfo();

    /* editor */
    int DeleteSeleteObject();
    DWORD ObjTypeFromName(LPCTSTR fileName);
    bool GetPlaceObjectPos(D3DXVECTOR3& vPos, KG3DSceneOutputWnd* pWnd);
    bool GetPlacePortalPos(D3DXVECTOR3& vPos, KG3DSceneOutputWnd* pWnd, KG3DPvsPortal* ptl);
    void Place2Pvs();
    void ScalHandObj(float fScal);

    INT ExecAction(INT nActionID, INT nExecState, WPARAM wParam, LPARAM lParam);
    void TrackCamareFrameMove();
    void SetCamareState(WORD wState, BOOL bSet);

protected :
	KG3DLightMapBaker*       m_lpLightMapBaker;

    KG3DRepresentObjectPVS*  m_pPvs;
    KG3DRepresentObject*     m_pHandObj;
    KG3DPvsPortal*           m_pHandPtl;
    DWORD                    m_dwPlacePortalMode;
    list<string>             m_listPlaceObj;
    list<string>::iterator   m_itorListObj;
    bool                     m_bSwitchObj;
    BOOL                     m_bCull;
    vector<IPvsEditorEventCallback*> m_vpEventCallBack;

    KG3DSceneEntityList    m_listSelTabEntity;

    KG3DSceneEntityList::iterator m_itorTabSel;

    KG3DRepresentNPC*        m_pRunder;
    bool                     m_bRunMode;
    FLOAT                    m_CameraSpeed;
    BOOL                     m_bMouseTargetMoving;
    D3DXVECTOR3              m_vMouseTargetPosition;
    DWORD	                 m_CamareState;
    D3DXVECTOR3              m_vCamera;

    LPDIRECT3DVERTEXBUFFER9  m_pGroundGridVertexs;

    /* fps */
 
    DWORD		m_dwPrevTime;
    DWORD		m_dwCurrTime;
    FLOAT		m_fCounter;
    DWORD       m_dwTimer;
    FLOAT       m_fFPS;

    KG3DScenePointLightRender *m_lpPointLightRender;
};

#endif // _KG3DSCENEPVSEDITOR_H_
