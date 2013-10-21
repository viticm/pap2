
#ifndef _KG3DREPRESENTOBJECTPVS_H_
#define _KG3DREPRESENTOBJECTPVS_H_

#include "KG3DRepresentObject.h"
#include "KG3DFrustum.h"
#include "KG3DSceneOutDoorSpaceMgr.h"


#define PVS_FILE_VERSION_1      100 
#define PVS_FILE_VERSION_2      200 
#define PVS_FILE_VERSION_3      300 

#define PVS_PORTAL_NAME_MAX_LEN 32
#define PVS_SUOBJ_MAX_MDL_NUM   32

enum {
    PORTAL_LEFT_TOP = 0,
    PORTAL_RIGHT_TOP,  
    PORTAL_RIGHT_B0TTOM,
    PORTAL_LEFT_B0TTOM  
};

enum {
    OUTSIDE_SHELL = 0,
    INSIDE_SHELL = 1,
    SHELL_COUNT 
};

class KG3DRepresentPoly;
class KG3DRepresentObjectPVS;
class KG3DCamera;


/*
 * the portal for scene pvs
 */

class KG3DPvsPortal : public IKG3DPvsPortal, public KG3DTransformation
{
    friend class KG3DRepresentObjectPVS;
public :

    KG3DPvsPortal::KG3DPvsPortal(KG3DRepresentObjectPVS* pPvs);

    /* check whether the portal can be see or not*/
    BOOL IsVisiable(KG3DCamera* pCamera);

    /* update the frustum by camera and portal*/
    HRESULT UpdateFrustum(KG3DCamera* pCamera);

    /* calculate the aabbox */
    HRESULT GetBBox(AABBOX* pAABBox);

    /* update world matrix and position*/
    HRESULT FrameMove();

    /* render selected mark */
    HRESULT RenderSelectMark();

    /* render, use for editor*/
    HRESULT Render(DWORD dwColor = 0x55000000);

    /* get the world matrix */
    D3DXMATRIX GetWorldMatrix();

    /* get the local matrix */
    D3DXMATRIX GetMatrix();

    KG3DPvsPortal* Clone(KG3DRepresentObjectPVS* pvs);

    virtual LPCTSTR GetName();

    virtual void SetName(LPCTSTR szName) { m_strName = szName; }
    D3DXVECTOR3 GetFront() { return D3DXVECTOR3(m_matWorld._31, m_matWorld._32, m_matWorld._33); }

    KG3DRepresentObjectPVS* GetParentPvs() { return m_pDependPvs; }

public :
    D3DXVECTOR3 m_vLocalVector[4];
    D3DXVECTOR3 m_vWorldVector[4];

private :

    KG3DFrustum m_Frustum;
    D3DXMATRIX  m_matWorld;

    KG3DRepresentObjectPVS* m_pDependPvs;
    int m_nSelected;

    string m_strName;
};


/*
 * the pvs object
 */

class KG3DRepresentObjectPVS : public KG3DRepresentObject, public IEKG3DRepresentPVS, public KG3DSceneSpaceNode,
	public IKG3DSceneEntityContainer
{
public :
	struct ModelExtData
	{
		int nLightmapID;
		int nShaderType;
		int nExtened[8];

		ModelExtData()
		{
			ZeroMemory(this,sizeof(ModelExtData));
			nShaderType = -1;
			nLightmapID = -1;
		}
	};

    /* get border model */
    virtual IEKG3DModel* GetOutsideBorderModel() { return (m_vecModel.size() < 1) ?  NULL : m_vecModel[OUTSIDE_SHELL]; }
    virtual IEKG3DModel* GetInsideBorderModel()  { return (m_vecModel.size() < 2) ?  NULL : m_vecModel[INSIDE_SHELL]; }


    /* set border model */
    virtual HRESULT SetOutsideBorderModel(LPCTSTR fileName);
    virtual HRESULT SetInsideBorderModel(LPCTSTR fileName);


    /* add / delete static object (by load pvs file)*/
    virtual HRESULT AddObject(KG3DRepresentObject* obj, KG3DScene* pScene);
    virtual HRESULT DelObject(KG3DRepresentObject* obj, KG3DScene* pScene);
    

    /*input / remove model (dynimic by client) */
    virtual HRESULT InputModel(KG3DModel* mdl);
    virtual HRESULT RemoveModel(KG3DModel* mdl);

    virtual HRESULT InputObject(KG3DRepresentObject* obj, KG3DScene* pScene);
    virtual HRESULT RemoveObject(KG3DRepresentObject* obj, KG3DScene* pScene);


    /* check whether the positions in the pvs */
	virtual BOOL IsPositionsInSpace(D3DXVECTOR3* pVers, int nCount);
    virtual BOOL IsPositionInSpace(D3DXVECTOR3& vPos);


    /* scene node type */
    virtual DWORD  GetSceneNodeType() { return SceneNodeType_Pvs; }


    /* load / save object shell */
    virtual HRESULT SaveToInformationBuffer(KG3DMemoryFile* pFile);
    virtual HRESULT LoadFromInformationBuffer(const KG3DMemoryFile* pFile,DWORD dwOption);


    /* load / save pvs file */
    virtual HRESULT SavePvsFile(LPCTSTR szFileName);
    virtual HRESULT LoadPvsFile(LPCTSTR szFileName, DWORD dwOption);

    virtual HRESULT Load(LPCTSTR strName, DWORD_PTR wParam, DWORD_PTR lParam) { return LoadPvsFile(strName, 0); }
    virtual HRESULT Copy(KG3DRepresentObject* pSrcObject);


    /* init / uninit */
    virtual HRESULT Init();
    virtual HRESULT UnInit();
    virtual HRESULT Clear(BOOL bClearEntity);

    
    /* render / framemove */
    virtual HRESULT RenderHierarchy(BOOL bCull = TRUE, KG3DScenePointLightRender* pRender = NULL);
    virtual HRESULT Render(DWORD dwOption);
    virtual HRESULT FrameMove();


    /* culling */
    virtual void UpdateVisiableSubset(KG3DCullData* pCull);
    virtual void OnRemoveFromEntityMgr(KG3DCullData* pCull);
    virtual HRESULT CalcFrustum(const TypeFrustums& vInputFrustum, DWORD dwLookMode);//继承自KG3DSceneSpaceNode
    virtual int GetInsideObjectCount();
    virtual IEKG3DRepresentObject* GetInsideObject(DWORD dwIndex);

    BOOL    IsPositionInSetDirection(const D3DXVECTOR3& Position, const D3DXVECTOR3& Direction);
    BOOL    IsPositionInSet(D3DXVECTOR3& Position);
    HRESULT UpdateVisiableByPortal(KG3DPvsPortal* pProtal);
    HRESULT UpdateVisiable();


    /* render option */
    virtual BOOL    GetRenderPortal()               { return m_bRenderPortal;                      }
    virtual HRESULT SetRenderPortal(BOOL bEnable)   { m_bRenderPortal = bEnable; return S_OK;      }
    virtual BOOL    GetRenderBorder()               { return m_bRenderBorderModel;                 }
    virtual HRESULT SetRenderBorder(BOOL bEnable)   { m_bRenderBorderModel = bEnable; return S_OK; }

    virtual void    ShowOutsideShell(BOOL bShow)    { m_bShelllShowFlag[OUTSIDE_SHELL] = bShow;    }
    virtual void    ShowInsdieShell(BOOL bShow)     { m_bShelllShowFlag[INSIDE_SHELL] = bShow;     }

    virtual void NextSubOctree();


    /* portal edit */
    virtual int     GetPortalCount();
    virtual IKG3DPvsPortal* GetPortal(DWORD dwIndex);
    virtual HRESULT DeletePortal(DWORD dwIndex);

    HRESULT AddPortal(KG3DPvsPortal*& pPort, const D3DXMATRIX& matPose); 
    HRESULT AddPortal(KG3DPvsPortal*& pPort,D3DXVECTOR3 vPos,D3DXVECTOR3 vDirection,float fHeight,float fWidth);
    HRESULT DeletePortal(KG3DPvsPortal* pPort);
    HRESULT DeleteAllPortal();


    /* other */
    virtual HRESULT ComputeBBox();
    virtual KG3DRepresentObject* CheckLoadIndoorWall();
    virtual KG3DSceneSpaceNode* Conver2SpaceNode()  { return this; }
    virtual size_t GetSubModels(vector<KG3DModel*>& vModels);
    virtual BOOL GetCameraNearRayIntersect(IN CONST D3DXVECTOR3 vSrc[], IN CONST D3DXVECTOR3 vDst[], IN CONST INT nNum, OUT FLOAT* pDis);
    virtual BOOL GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction, BOOL bFiltrate);
	virtual BOOL RayIntersection(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir, FLOAT* pfRet, BOOL bIntersectBoxOnly, KG3DIntersectionFilter* pFilter = NULL);
	
	HRESULT GetPolyIntersectRect(vector<KG3DRepresentPoly*>& vPoly,D3DXVECTOR2& A,D3DXVECTOR2& C);//只在m_setEntityShellInside里查找
	HRESULT GetObjectIntersectRect(vector<KG3DRepresentObject*>& vObject,D3DXVECTOR2& A,D3DXVECTOR2& C);//只在m_setEntityShellInside里查找
    /* name */
    virtual LPCTSTR GetName()                                { return m_scName.c_str(); }
    virtual void SetName(LPCTSTR strName)                    { m_scName = strName;      }
    virtual HRESULT GetName(LPTSTR pName, size_t BufferSize) { strncpy(pName, m_scName.c_str(), BufferSize); return S_OK; }
    virtual LPCTSTR GetNodeName()                            { return m_scName.c_str(); }


    KG3DRepresentObjectPVS();
    virtual ~KG3DRepresentObjectPVS();

	virtual HRESULT RemoveEntityObjectShell(KG3DEntityObjectShell* pShell);
	virtual HRESULT InputObjectShell(KG3DEntityObjectShell* pShell, BOOL bCheck2Pvs);
public :
    list<KG3DPvsPortal*>        m_listPortal;	
	//////////////////////////////////////////////////////////////////////////
    set<KG3DRepresentObject*>   m_setObjectInside;      
    set<KG3DModel*>             m_setModelInside;      
	set<KG3DEntityObjectShell*> m_setEntityShellInside;
	//////////////////////////////////////////////////////////////////////////
    set<KG3DRepresentObject*>   m_setObjectVisiable;    
    DWORD                       m_dwVersion;
    BOOL                        m_bRenderBorderModel;
    BOOL                        m_bRenderPortal;
    BOOL                        m_bShelllShowFlag[SHELL_COUNT];
    //KG3DPvsOCTree               m_PvsOCTree;
    static BOOL                 m_sCulling;

	HRESULT GetModels(vector<KG3DModel*>* pvecModels,vector<KG3DModelPointLight*>* pvecPointLight,BOOL bIncludeOutsiderBorder);
protected:

	HRESULT ProcessLightmapKeyForModels(vector<KG3DModel*>* pvecModelWithLightmap);

	HRESULT ProcessModel(KG3DModel* pModel,ModelExtData& vData);

	HRESULT _LoadModelExtData(ModelExtData& vData,IFile* pFile);
	HRESULT _SaveModelExtData(KG3DModel* pModel,IFile* pFile);

	HRESULT _LoadPvsFileVersion2(IFile* pFile,DWORD dwOption);
		
	HRESULT _LoadPvsFileVersion3(IFile* pFile,DWORD dwOption);

	HRESULT _ReadObjectsInside(IFile* pFile,DWORD dwOption);

	DWORD m_dwLightmapInUse;
};

#define ANTIPORTAL_SINGLEPLANE 0
#define ANTIPORTAL_DOUBLEPLANE 1
#define ANTIPORTAL_CYLINKDER   2
#define ANTIPORTAL_CYLINKDEREX 3

class KG3DRepresentObjectAntiPortal ://阻挡片
	public KG3DRepresentObject,
	public IEKG3DRepresentAntiPortal
{
public:
	DWORD m_dwAntiPortalType;

	float m_fHeight;
	float m_fWidthBottom;
	float m_fWidthTop;

	D3DXVECTOR3 m_vWorldPostion;//世界坐标系里的位置
	D3DXVECTOR3 m_vWorldDirection;//世界坐标系里的方向

	KG3DFrustum m_Frustum;//用来做裁剪的视锥

	D3DXVECTOR3 m_vPortalPos[4];
	D3DXVECTOR3 m_vPortalPosWorld[4];

	//////////////////////////////////////////////////////////////////////////
	virtual DWORD GetPortalType();
	virtual HRESULT SetPortalType(DWORD dwType);

	virtual HRESULT GetData(float* pWidthBottom,float* pWidthTop,float* pHeight);
	virtual HRESULT SetData(float  fWdithBottom,float fWdithTop,float fHeight);

	//////////////////////////////////////////////////////////////////////////
	HRESULT UpdateToWorld(KG3DCamera* pCamera);
	BOOL    IsVisiable(KG3DCamera* pCamera);
	HRESULT UpdateFrustum(KG3DCamera* pCamera);

	virtual HRESULT Render(DWORD dwOption);

	KG3DRepresentObjectAntiPortal();
	virtual ~KG3DRepresentObjectAntiPortal();

protected:

private:

};

#endif