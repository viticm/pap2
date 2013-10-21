////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRepresentNPC.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-9-27 12:55:29
//  Comment     : NPC Object Class
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DREPRESENTNPC_H_
#define _KG3DREPRESENTNPC_H_

#include "KG3DRepresentObject.h"
#include "KG3DRepresentNPCTemplate.h"
#include "KG3DEngineManager.h"
#include "KG3DSFX.h"
#include "IEEditor.h"
#include "MathToolEx.h"
#include "KG3DSceneOutDoorSpaceMgr.h"

class KG3DScene;

enum KG3D_NPC_TYPE
{
	NPC_TYPE_PROTAGONIST, 
	NPC_TYPE_COMPUTER,

	NPC_TYPE_COUNT
};

enum NPC_MOVEMENT_TYPE
{
    NPC_MOVEMENT_TYPE_STAND,
    NPC_MOVEMENT_TYPE_AHEAD,
    NPC_MOVEMENT_TYPE_BACK ,
    NPC_MOVEMENT_TYPE_LEFT ,
    NPC_MOVEMENT_TYPE_RIGHT,
    NPC_MOVEMENT_TYPE_JUMP ,
    NPC_MOVEMENT_TYPE_TURN_L,
    NPC_MOVEMENT_TYPE_TURN_R,

    NPC_MOVEMENT_TYPE_COUNT
};

class KG3DRepresentNPC : public KG3DRepresentObject, public IEKG3DRepresentNPC
{
public :
    int		FildTempleteIndex();

public :
	HRESULT LoadFromIniFileEx(LPSTR pFileName,LPSTR pSectionName,DWORD dwOption);

	HRESULT LoadFromIniSection(IIniFile* pFile,LPSTR pSectionName,DWORD dwOption);

    virtual HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
	virtual HRESULT Load(LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam);//完整的加载
	virtual HRESULT LoadByLogicId(DWORD dwNpcId);

public : 
    virtual HRESULT RenderHierarchy(BOOL bCull = TRUE);
	virtual HRESULT Render(DWORD dwOption);
	HRESULT Init();
	virtual HRESULT UnInit();

	HRESULT SetModelFace(LPCTSTR pMeshFile,LPCTSTR pMtlFile,LPCTSTR pAniFile,LPCTSTR pSocketName);
	HRESULT PauseAnimation(BOOL bPause);
	HRESULT PlayAnimation(LPCSTR szAniPath);
    HRESULT SetAlpha(float fAlpha);
	virtual HRESULT FrameMove();

	virtual	HRESULT Copy(KG3DRepresentObject* pSrcObject);

	void SetNPCName(LPCTSTR pName);
    
    HRESULT SetNPCTranslation(D3DXVECTOR3* pValue);
    HRESULT SetNPCRotation(D3DXQUATERNION* pValue);
	
public :

	KG3DRepresentNPC(void);
	virtual ~KG3DRepresentNPC(void);
	//virtual HRESULT NPCFrameMove();//2009-2-13功能移动到FrameMoveSceneRunner

	//////////////////////////////////////////////////////////////////////////
	// 2006-5-18
public :
	virtual DWORD GetReliveID()
	{
		return m_dwReliveID;
	}

	virtual DWORD GetRandomID()
	{
		return m_dwRandomID;
	}
	virtual HRESULT SetReliveID(DWORD dwReliveID)
	{
		m_dwReliveID = dwReliveID;
		return S_OK;
	}
	virtual HRESULT SetRandomID(DWORD dwRandomID)
	{
		m_dwRandomID = dwRandomID;
		return S_OK;
	}

	virtual DWORD GetAISetID()
	{
		return m_dwAISetID;
	}

	virtual HRESULT SetAISetID(DWORD dwAI)
	{
		m_dwAISetID = dwAI;
		return S_OK;
	}

	virtual HRESULT GetName(LPTSTR pName, size_t BufferSize);
    virtual HRESULT GetNPCTranslation(D3DXVECTOR3* pValue);
    virtual HRESULT GetNPCRotation(D3DXQUATERNION* pValue);

	HRESULT GetPrePosition(D3DXVECTOR3* pVecPosition);
	HRESULT GetCurPosition(D3DXVECTOR3* pVecPosition);
	HRESULT SetCurPosition(D3DXVECTOR3  vecPosition, KG3DScene& sceneCur, int nSnapToGround);

	HRESULT GetDirection(D3DXVECTOR3* pVecDirection);
	HRESULT SetDirection(D3DXVECTOR3  vecDirection);

	HRESULT GoAhead(float fSpeed, KG3DScene& sceneCur);
	HRESULT BackOff(float fSpeed, KG3DScene& sceneCur);
	HRESULT TurnLef(float fAngel, int nUpdateDirFlag = true);
	HRESULT TurnRig(float fAngel, int nUpdateDirFlag = true);
	HRESULT Jump(float fUSpeed);
	HRESULT MoveLef(float fSpeed, KG3DScene& sceneCur);
	HRESULT MoveRig(float fSpeed, KG3DScene& sceneCur);

	HRESULT SetNpcFrontDir(D3DXVECTOR3 vecFront);
	HRESULT	SetNpcType(DWORD dwNpcType);

    HRESULT ProcessRButtonDn(WPARAM wParam, LPARAM lParam);
    HRESULT ProcessRButtonUp(WPARAM wParam, LPARAM lParam);

    //HRESULT BandScene(IKG3DScene* lpScene);

	HRESULT DrawVisibleSphere(D3DCOLORVALUE color,float radius,float hight );//by huangjinshou 2007年7月13日
    //HRESULT LoadSFXFromBindFile(LPCSTR pstrFilePath);

	//(by dengzhihui 2006年9月15日 16:40:58
	//为将键盘判断去掉，用移动标志位替换对键盘的判断
	enum enumNpcMoveState{ 
		nmsNone = 0,
		nmsFoward = 1,
		nmsBack = 2, 
		nmsTurnLeft = 4, 
		nmsTurnBodyLeft = 8, //原地转，但镜头不转
		nmsTurnRight = 16, 
		nmsTurnBodyRight = 32, //原地转，但镜头不转
		nmsMoveLeft = 64,
		nmsMoveRight = 128,
		nmsJump = 256,
		nmsSpeedUp = 512,
		nmsSpeedDown = 1024,
		nmsCamareTurn = 2048,
	};
	void SetNpcMoveBits(enumNpcMoveState eState, BOOL bSet);
	//)
	void SaveTheFloorHeight(KG3DScene& sceneCur, IELogicalSceneConnection* pLogicalConnection);
public :
	int m_nKind;
	int m_nLevel;
	//int m_nAIKind;
	DWORD m_dwTemplateID;
	float m_nAlertRange;
	float m_nAttackRange;
	BOOL m_bAttackRange;
	DWORD m_dwRepresentID;
	DWORD m_dwReliveID;
	DWORD m_dwRandomID;
	DWORD m_dwAISetID;

	//int m_nDisableAutoTurn;

	D3DXVECTOR3		m_vecPrePosition;
	D3DXVECTOR3		m_vecCurPosition;
	D3DXVECTOR3		m_vecCurFloor;	//只用于SceneRunner
	D3DXVECTOR4		m_vLogicalFloor_CellOrder; //只用于SceneRunner，第四个值放Cell的位置
	D3DXVECTOR3		m_vecDirection;
	D3DXVECTOR3		m_vecPreDirecn;

	float			m_fAngelRateHor;
	float			m_fAngleHor;

	float			m_fUpSpeed;

	DWORD			m_dwNpcType;
	DWORD           m_dwMovementType;	//行动方式

	float			m_fMoveSpeed;
	float           m_fLegLen;

	D3DXVECTOR3		m_vNewDir;
	D3DXVECTOR3		m_vAxis;

	DWORD m_eNPCState;

	//KG3DScene*      m_lpScene;//<SetScene Refactor>

	KG3DSFXSuede	m_sfxSperture;

	virtual HRESULT GetKind(INT *pnRetKind);
	void SetKind(int val) { m_nKind = val; }
	

	virtual HRESULT GetLevel(INT *pnRetLevel);
	void SetLevel(int val) { m_nLevel = val; }
	

	//virtual HRESULT GetAIKind(INT *pnRetAIKind);
	//void SetAIKind(int val) { m_nAIKind = val; }

	
	virtual HRESULT GetTemplateID(DWORD *pdwRetID); 
	void SetTemplateID(DWORD val) { m_dwTemplateID = val; }

   
    virtual HRESULT GetRepresentID(DWORD *pdwRetID); 
    void SetRepresentID(DWORD val) { m_dwRepresentID = val; }
	
	virtual HRESULT GetAlertRange(float *pnAlertRange);
	void SetAlertRange(float val){ m_nAlertRange = val; }	
	
	virtual HRESULT GetAttackRange(float *pnAttackRange);
	void SetAttackRange(float val){ m_nAttackRange = val; }

	
	virtual void HideAttackRange(){m_bAttackRange = !m_bAttackRange;}
	virtual void SetbAttackRange(BOOL val){m_bAttackRange = val;}
    //HRESULT GetIsDisableAutoTurn(int* val);

    //HRESULT SetIsDisableAutoTurn(int val) {

    //    m_nDisableAutoTurn = val;
    //    return S_OK;
    //}

	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT SaveToInformationBuffer(KG3DMemoryFile* pFile);
	virtual HRESULT LoadFromInformationBuffer(const KG3DMemoryFile* pFile,DWORD dwOption);
private:
	void RenderFloorChecker( D3DXVECTOR3 &vScreen, LPD3DXFONT pFont);
private:
	KG3DModel* m_pModelFace;//脸的模型
	std::string m_scNPCName;
public:
	virtual float GetMoveSpeed();
	virtual HRESULT SetMoveSpeed(float fMoveSpeed);
};

class KG3DRepresentDoodad : 
	public KG3DRepresentObject, 
	public IEKG3DRepresentDoodad
{
	std::string m_scDoodadName;
public :
	virtual HRESULT LoadFromFile( 
		const char cszFileName[], unsigned uFileNameHash, unsigned uOption
		);

	virtual	HRESULT Copy(KG3DRepresentObject* pSrcObject);

	virtual HRESULT GetName(LPTSTR pName, size_t BufferSize);
    virtual HRESULT GetTranslation(D3DXVECTOR3* pValue);
    virtual HRESULT SetDoodadTranslation(D3DXVECTOR3* pValue);
    virtual HRESULT GetRotation(D3DXQUATERNION* pValue);
    virtual HRESULT SetRotation(D3DXQUATERNION* pValue);
    virtual HRESULT GetTemplateID(DWORD *pdwRetID); 


	virtual HRESULT Load(LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam);

	void SetDoodadName(const std::string &name);	
	
	DWORD m_dwTemplateID;
	DWORD m_dwReliveID;
	void SetTemplateID(DWORD val) { m_dwTemplateID = val; }


	virtual DWORD GetReliveID() { return m_dwReliveID; }
	virtual HRESULT SetReliveID(DWORD dwReliveID)
	{
		m_dwReliveID = dwReliveID;
		return S_OK;
	}
	// 动态障碍
	BOOL  m_bUseDefaultObstacle;
	BOOL  m_bGenerateObstacle;
	virtual BOOL GetUseDefaultObstacle() { return m_bUseDefaultObstacle; }
	virtual HRESULT SetUseDefaultObstacle(BOOL bUseDefaultObstacle)
	{
		m_bUseDefaultObstacle = bUseDefaultObstacle;
		return S_OK;
	}
	virtual BOOL GetGenerateObstacle() { return m_bGenerateObstacle; }
	virtual HRESULT SetGenerateObstacle(BOOL bGenerateObstacle)
	{
		m_bGenerateObstacle = bGenerateObstacle;
		return S_OK;
	}

	virtual HRESULT Render(DWORD dwOption);

	virtual HRESULT SaveToInformationBuffer(KG3DMemoryFile* pFile);
	virtual HRESULT LoadFromInformationBuffer(const KG3DMemoryFile* pFile,DWORD dwOption);
public:
	KG3DRepresentDoodad();
	virtual ~KG3DRepresentDoodad();
};

class KG3DRepresentWayPoint : 
	public KG3DRepresentObject, 
	public IEKG3DRepresentWayPoint
{	
	std::string m_scWayPointName;
	DWORD m_dwSetID;
	int m_nWayPointType;//0 起点 1 普通路点 2终点
public:

	virtual HRESULT SetWayPointType(int nType);

	/*
		virtual HRESULT LoadFromFile( 
				LPCTSTR cszFileName, unsigned uFileNameHash, unsigned uOption);*/
		
	virtual	HRESULT Copy(KG3DRepresentObject* pSrcObject);

	virtual	HRESULT SetWayPointName(LPCTSTR lpstrName)
	{
		m_scWayPointName = lpstrName;
		return S_OK;
	}	

	virtual	DWORD GetSetID() { return m_dwSetID; }
	virtual	HRESULT SetSetID(DWORD val) { m_dwSetID = val; return S_OK;}

	virtual HRESULT GetName(LPTSTR pName, size_t BufferSize);
    virtual HRESULT GetTranslation(D3DXVECTOR3* pValue);
    virtual HRESULT GetRotation(D3DXQUATERNION* pValue);

	virtual HRESULT Load(LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam);

	virtual HRESULT SaveToInformationBuffer(KG3DMemoryFile* pFile);
	virtual HRESULT LoadFromInformationBuffer(const KG3DMemoryFile* pFile,DWORD dwOption);

	KG3DRepresentWayPoint();
	virtual ~KG3DRepresentWayPoint();
};

class KG3DRepresentTrafficPoint : 
	public KG3DRepresentObject, 
	public IEKG3DRepresentTrafficPoint
{
private:
    DWORD m_dwSetID;
	DWORD m_TrafficPointKind/*m_dwType*/;//0 big，1 small	//如果用m_dwType的话会和基类的重复
	
public:	
	/*
		virtual HRESULT LoadFromFile( 
				LPCTSTR cszFileName, unsigned uFileNameHash, unsigned uOption);*/
		
	virtual HRESULT SetKind(DWORD dwType); 
	virtual DWORD GetKind() {return m_TrafficPointKind;}
	virtual	DWORD GetSetID() { return m_dwSetID; }
	virtual	HRESULT SetSetID(DWORD val) { m_dwSetID = val; return S_OK;}
	virtual	HRESULT Copy(KG3DRepresentObject* pSrcObject);
	//virtual BOOL IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);

	//virtual HRESULT GetName(LPTSTR pName, size_t BufferSize);
	//virtual HRESULT Render(DWORD dwOption);
	virtual HRESULT GetTranslation(D3DXVECTOR3* pValue);
	//virtual HRESULT ComputeDynamicBBox();
	//virtual HRESULT ComputeBBox();

	virtual HRESULT Load(LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam);
	virtual HRESULT SaveToInformationBuffer(KG3DMemoryFile* pFile);
	virtual HRESULT LoadFromInformationBuffer(const KG3DMemoryFile* pFile,DWORD dwOption);
};

class KG3DRepresentRefreshPoint : 
	public KG3DRepresentObject, 
	public IEKG3DRepresentRefreshPoint
{	
	std::string m_scRefreshPointName;
	DWORD m_dwSetID;
	DWORD m_dwRefreshPointType;//0 doodad ,1 npc
public:

	//virtual HRESULT LoadFromFile( LPCTSTR cszFileName, unsigned uFileNameHash, unsigned uOption);
	virtual	HRESULT Copy(KG3DRepresentObject* pSrcObject);

	virtual	HRESULT SetRefreshPointName(LPCTSTR name)
	{
		m_scRefreshPointName = name;
		return S_OK;
	}	
	virtual HRESULT GetName(LPTSTR pName, size_t BufferSize);
    virtual HRESULT GetTranslation(D3DXVECTOR3* pValue);
    virtual HRESULT GetRotation(D3DXQUATERNION* pValue);

	virtual	DWORD GetRefreshPointType(){ return m_dwRefreshPointType;}
	virtual	HRESULT SetRefreshPointType(DWORD val) { m_dwRefreshPointType = val; return S_OK;}
         
	virtual	DWORD GetSetID(){ return m_dwSetID; }
	virtual	HRESULT SetSetID(DWORD val) { m_dwSetID = val; return S_OK;}

	virtual HRESULT Load(LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam);
	virtual HRESULT SaveToInformationBuffer(KG3DMemoryFile* pFile);
	virtual HRESULT LoadFromInformationBuffer(const KG3DMemoryFile* pFile,DWORD dwOption);

	KG3DRepresentRefreshPoint();
	virtual ~KG3DRepresentRefreshPoint();
};


class KG3DRepresentPoly : 
	public KG3DRepresentObject,
	public IEKG3DRepresentPoly
{
private:
	string m_szScriptFileName;
public:
	int   m_nLogicalSetID;
	string m_strLogicalSetName;
	DWORD m_dwColor;
	KG3DPoly m_Poly;
	D3DXVECTOR3 m_PreviousTranslation;
	DWORD m_dwModifyState;
	

	POLYUSEAGE m_ePolyUseage;

	struct VERTEXPOLY
	{
		D3DXVECTOR3 pos;
		D3DCOLOR    color;
	};
	#define FVF_POLY (D3DFVF_XYZ|D3DFVF_DIFFUSE)
	LPDIRECT3DVERTEXBUFFER9  m_lpPolyMesh;// huangjinshou //用来绘制多边形面
	HRESULT DrawPoly();
public:
	virtual PVOID GetObjectPointer()
	{
		return (KG3DRepresentObject*)(this);
	}

	virtual BOOL KG3DRepresentPoly::RayIntersection( const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir 
		, FLOAT* pfRet, BOOL bIntersectBoxOnly, KG3DIntersectionFilter* pFilter /*= NULL*/ );
	virtual BOOL IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);
	virtual HRESULT ComputeDynamicBBox();

	virtual BOOL IsPointIn(D3DXVECTOR3 vPos,BOOL bTestHeight);

	HRESULT GetName(LPTSTR pName, size_t BufferSize);
	virtual HRESULT RenderSelectMark(DWORD dwColor /* = 0xFFFF0000 */);

	virtual HRESULT OnSetTranslation(D3DXVECTOR3* pValue);
	virtual HRESULT ComputeBBox();
	virtual HRESULT CreateFromScenePoly(KG3DPoly* pPoly);
	virtual HRESULT Render(DWORD dwOption);

	virtual FLOAT GetVertexHeight();
	virtual INT   GetVertexCount();
	virtual D3DXVECTOR3 GetVertex(int nIndex);
	virtual HRESULT SetVertexHeight(FLOAT fHeight);
	virtual HRESULT SetVertexCount(INT nCount);
	virtual HRESULT SetVertex(int nIndex,D3DXVECTOR3 vPos);

	virtual DWORD GetColor();
	virtual INT GetLogicalSetID();
	
	virtual HRESULT SetColor(DWORD dwColor);
	virtual HRESULT SetLogicalSetID(INT nID);

	virtual HRESULT SetModifyState(DWORD dwState);
	virtual DWORD GetModifyState();

	virtual LPCTSTR GetScriptFileName();
	virtual HRESULT SetScriptFileName(LPCTSTR pFileName);

	virtual LPCTSTR GetPolyName();
	virtual HRESULT SetPolyName(LPCTSTR pName);

    virtual POLYUSEAGE GetPolyUseage()
	{
		return m_ePolyUseage;
	}
	virtual HRESULT SetPolyUseage(POLYUSEAGE e){m_ePolyUseage = e;return S_OK;}

	virtual LPCTSTR GetLogicalSetName();
	virtual HRESULT SetLogicalSetName(string strName);
	
	virtual HRESULT Copy(KG3DRepresentObject* pSrcObject);

	KG3DRepresentPoly();
	~KG3DRepresentPoly();
};
////////////////////////////////////////////////////////////////////////////////
class KG3DRepresentTerrainBlock : 
	public KG3DRepresentObject, 
	public IEKG3DRepresentTerrainBlock
{
private:
	KG3DSceneOutDoorSpaceMgr* m_lpSceneOutDoorSpaceMgr;
	//vector<KG3DRepresentObject*>* m_pObjectList; 
	//vector<KG3DTransformation>* m_pvecPos;

	vector<KG3DRepresentObject*> m_vecCopyObjects;//用于保存临时的用于复制的对象
	vector<KG3DTransformation>	m_vecTrans;	//用于保存对象以前的坐标
public:
	KG3DRepresentTerrainBlock(void);
	~KG3DRepresentTerrainBlock();
	struct BlockFileHead
	{
		int m_nTerrainCopyWidth;
		int m_nTerrainCopyHeight;
		KG3DTerrainRegionTextureInfo_Old m_TerrainCopyTextureInfo; 
		float m_fTerrainCopyHeightMax;
		float m_fTerrainCopyHeightMin;
		float m_fTerrainCopyHeightAve;
		size_t m_nSize;
		int m_nDetailTextureNum;
		D3DXVECTOR3 m_vecCopyPos;
	};
	struct EntitySave_VisibleSetInfo 
	{
		char EdgeMeshName[MAX_PATH];
		DWORD dwInSideLayer;
		DWORD dwOutSideLayer;
		DWORD dwDepth;
		DWORD dwExtend[8];
	};
	struct EntitySave_ObjectSetInfo 
	{
		char  ModelSetName[MAX_PATH];
		int   nCurModel;
		INT32 nLightmapTextureIndex;
		DWORD dwModelProperty;
		DWORD dwExtend[8];
	};
	struct EntitySave_ModelTranslation
	{
		D3DXVECTOR3 vScaling;
		D3DXVECTOR3 vScalingCenter;
		D3DXQUATERNION vScalingRotation;
		D3DXQUATERNION vRotation;
		D3DXVECTOR3 vRotaionCenter;
		D3DXVECTOR3 vTranslation;
		DWORD dwExtend[4];
	};
	struct EntitySave_ModelInfoBase
	{
		char MeshName[MAX_PATH];
		char MtlName [MAX_PATH];
		char AniName [MAX_PATH];

		DWORD dwModelType;
		INT32 nLightmapTextureIndex;
		float fSFXScaling;
		DWORD dwModelProperty;
		DWORD dwExtend[8];
	};
	struct EntitySave_ObjectBaseInfo
	{
		DWORD dwType;
		DWORD dwModelCount;
		D3DXVECTOR3 vScaling;
		D3DXVECTOR3 vScalingCenter;
		D3DXQUATERNION vScalingRotation;
		D3DXQUATERNION vRotation;
		D3DXVECTOR3 vRotaionCenter;
		D3DXVECTOR3 vTranslation;
		DWORD dwObjectProperty;
		DWORD dwExtent[4];
	};

	virtual HRESULT Render(DWORD dwOption);
	virtual HRESULT SetTerrainPoint(KG3DSceneOutDoorSpaceMgr* pSceneOutDoorSpaceMgr);
	virtual HRESULT ComputeDynamicBBox();
	virtual HRESULT ComputeBBox();
	virtual void UpdateTransformation();
	//virtual void SetObjectList(vector<KG3DRepresentObject*>* pObjectList,vector<KG3DTransformation>* pPos);//现在这两个vector都自己保存，不用外部传进来了
	virtual HRESULT SaveBlockToFile(const char cszFileName[]);
	virtual HRESULT ReadBlockFromFile(const char cszFileName[]);
	virtual HRESULT SaveObject(FILE* fpFile, KG3DRepresentObject* pObject);
	virtual HRESULT ReadObject(IFile* pFile, KG3DRepresentObject** ppObject);
	virtual BOOL IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction);
	virtual HRESULT RenderSelectMark(DWORD dwColor);

	//对剪贴板的操作
	VOID	RemoveAllCopyObjectsInScene(KG3DSceneSceneEditor& Scene, BOOL bClearPasteBoard);//如果bClearPasteBoard，那么就把保存的引用也清除
	VOID	AddAllCopyObjectsToScene(KG3DSceneSceneEditor& Scene);

	KG3DRepresentObject*	GetCopyObject(size_t i)	{return m_vecCopyObjects[i];}
	size_t					GetCopyObjectCount()	{return m_vecCopyObjects.size();}
	HRESULT					AddCopyObject(KG3DRepresentObject* p);



	//virtual HRESULT PasteTerrain(DWORD dwOption);
};


extern BOOL g_bRenderRepresentPoly;
extern BOOL g_bRenderAlertRange;
extern BOOL g_bRenderAttackRange;
#endif //_KG3DREPRESENTNPC_H_