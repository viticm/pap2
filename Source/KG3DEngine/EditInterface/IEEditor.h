#ifndef _INCLUDE_IEEDITOR_H_
#define _INCLUDE_IEEDITOR_H_

////////////////////////////////////////////////////////////////////////////////


#include "kg3dinterface.h"
#include "IEKG3DRepresentObject.h"
#include "FileType.h"
//#include "IEKG3DMeshBase.h"
#include "IEKG3DModelSpeedTree.h"
#include "IEKG3DSceneEntity.h"
#include "IETools.h"
//#include "KCell.h"
interface IEKG3DSceneOutDoorSpaceMgr;
#include "IEKG3DSceneSelectionTool.h"
#include "IEEditorTypes.h"

interface IEKG3DTerrain;
interface IEKG3DMaterial;
class KG3DSceneEntity;
interface IEKG3DSceneOutputWnd;
interface IEKG3DScene;
interface IEKG3DSceneEntity;
interface IEKG3DModel;
interface IEKG3DSceneLayer;
interface IEKG3DSFXBillboard;
interface IEKG3DMeshLensFlare;

struct _KG3DLogicalRegionInfo
{
	BOOL bLoaded;
	BOOL bModified;
};
enum ObjNodeType
{
	NODETYPE_NONE         = 0,
	NODETYPE_ROAD         = 1,
	NODETYPE_RIVER        = 2,
	NODETYPE_WALLKEYPOINT = 3,
	NODETYPE_RIGIDBODY     = 4
};

// 策划场景编辑器
#ifndef DESIGN_EXTRA_STRUCT_DEF
typedef struct _AtlVector3 {
	FLOAT x;
	FLOAT y;
	FLOAT z;
} AtlVector3;
#define DE_MAX_FIELDS 32
typedef struct _AtlObjInfo
{
	BSTR strType;				// "NPC", "Doodad" 等
	LONG iIndex;                // 序号
	LONG iLogicObjPtr;          // 逻辑对象结构体指针，例如NpcInfo*
	LONG bChanged;              // editor用，表示是否需要更新到逻辑数据
	LONG iFieldsCount;          // 值个数
	BSTR strFields[DE_MAX_FIELDS]; // 列
	BSTR strValues[DE_MAX_FIELDS]; // 值
} AtlObjInfo;
#endif

interface IEKG3DSceneCameraMovement;

interface IEKG3DWaterEffect
{
    virtual LPCTSTR GetWaterSurfaceTextureName()= 0;
    virtual LPCTSTR GetWaterBumpTextureName() = 0;

    virtual HRESULT SetWaterSurfaceTexture(LPCTSTR szFileName) = 0;
    virtual HRESULT SetWaterBumpTexture(LPCTSTR szFileName) = 0;

    virtual D3DXCOLOR* GetWaterColorPtr(int nDetail) = 0;
    virtual D3DXCOLOR* GetSpecColorPtr() = 0;
    virtual D3DXCOLOR* GetFogColorPtr() = 0;

    virtual D3DXVECTOR4* GetTextParamPtr() = 0;
    virtual D3DXVECTOR4* GetWaveParamPtr() = 0;
    virtual D3DXVECTOR4* GetWaveParam2Ptr() = 0;

    virtual HRESULT SetFogInfo(float fStart, float fEnd) = 0;
    virtual HRESULT GetFogInfo(float& fStart, float& fEnd) = 0;

    virtual HRESULT ReloadEffect() = 0;
};

interface IEKG3DSceneEditorBase
{
    virtual void    SetRtsActive(BOOL active) = 0;
    virtual BOOL    GetRtsActive() = 0;

	virtual HRESULT SetEditing(DWORD dwEditing) = 0;
	virtual DWORD GetEditing() = 0;
	virtual HRESULT GetEditState(DWORD* pdwState) = 0;
	virtual HRESULT SetEditState(DWORD dwState) = 0;
	//virtual HRESULT EnableEditCameraKeyFrame(BOOL bEdit) = 0;
	//virtual HRESULT GetCameraMovement(IEKG3DSceneCameraMovement** pCameraMovement) = 0;
	virtual HRESULT GetFirstSelectedModel(IEKG3DModel*& pModel) = 0;
	virtual INT ExecAction(INT nActionID, INT nExecState, WPARAM wParam, LPARAM lParam) = 0;	//返回非0标识处理消息完毕
	//virtual INT OnSelect(INT nActionID, INT nExecState, INT nXOff, INT nYOff) = 0;
	//virtual void SyncPickRay() = 0;	//SyncPickRay是内部的一种避免重复取交点的加速工作，不应该暴露在接口
	//virtual void OnMouseMove(WPARAM wParam, LPARAM lParam) = 0;
	//virtual void ZoomView(short nDelta, POINT pt) = 0;
	//virtual HRESULT AddSceneLayer(IEKG3DSceneLayer *piParentLayer, LPCTSTR pcszNewLayerName, IEKG3DSceneLayer **ppiRetNewLayer) = 0;
	//virtual HRESULT DeleteSceneLayer(IEKG3DSceneLayer *piLayer) = 0;
	//virtual HRESULT GetRootLayer(IEKG3DSceneLayer** pLayer) = 0;
	//virtual HRESULT PutSelectedEntityToLayer(IEKG3DSceneLayer *piLayer) = 0;
	//virtual INT GetSceneLayerSize() = 0;
	//virtual HRESULT GetSceneLayer(INT nIndex , IEKG3DSceneLayer** pLayer) = 0;
	//virtual HRESULT ClearSelectedEntity() = 0;
	virtual HRESULT GetSceneSelector(IEKG3DSceneSelectBase** ppSelector) = 0;
	virtual HRESULT GetMouseXY(INT* pX, INT* pY) = 0;
	virtual HRESULT SetMouseXY(INT pX, INT pY) = 0;

	virtual HRESULT GetSelectionToolInterface(IEKG3DSceneSelectionTool** ppTool) = 0;
	virtual HRESULT PasteSelection() = 0;
	virtual HRESULT IEInsertEntitySelectable(SCENEENTITYTYPE dwType, LPVOID pPoint, BOOL bInsertAndSetSelectable) = 0;//<Selectable Refactor>
};

enum enumCommandType{ctUp, ctDown, ctLeft, ctRight, ctPageDown, ctPageUp, ctHome, ctEnd, ctInc, ctDec,
ctHideObject, ctHideTerrain, ctHideGrass,ctHideTree, ctAddKeyFrame, ctDropObject, ctHideNPCAttackRange};

enum eCellModifyState
{
	MODIFY_NONE          = 0,
	MODIFY_BLOCKCHARACTER = 1,
	MODIFY_PUTOBJECT     = 2,
	MODIFY_SAFE         = 4,
	MODIFY_STALL        = 8,
	MODIFY_CELLTYPE      = 16,
	MODIFY_PASSWIDTH     = 32,
	MODIFY_SLIPDIRECTION = 64,
	MODIFY_SLIPVELOCITY  =128,
	//(by dengzhihui 2006年8月3日 10:35:48
	MODIFY_INDOOR		 =256,	//室内区域
	MODIFY_REST			 =512,	//休息区域
	MODIFY_RIDEHORSE	 =1024,	//下马区域
	MODIFY_SCRIPTFILE	 =2048,	//索引文件ID
	//)
	DWORD_FORCE = 0xFFFFFFFF,
};
enum eCellBrushShape
{
	cbCircle = 0, cbRectangle = 1,
};
interface IEKG3DSceneSceneEditor;
interface IEKG3DSceneLogicalEditorBase
{
	virtual HRESULT SetShowLogicalCell(BOOL bValue) = 0;
	virtual HRESULT SetShowLogicalTerrenCell(BOOL bValue) = 0;
	virtual HRESULT SetShowLogicalItemCell(BOOL bValue) = 0;
	virtual HRESULT SetShowLogicalSelectedCell(BOOL bValue) = 0;

	virtual HRESULT SetModifyState(eCellModifyState eState,BOOL bModify) = 0;
	virtual HRESULT SetShowState(eCellModifyState eState,BOOL bModify) = 0;
	//virtual HRESULT SetLogicalScene(KScene* pLogicalScene) = 0;
	virtual HRESULT GetLogicScene(LPVOID* ppLogicScene) = 0;
	virtual HRESULT SetLogicScene(LPVOID pLogicScene) = 0;
	virtual void RecalcShowCell() = 0;
	virtual void SetCurrentEditColor(COLORREF clr) = 0;
	virtual void ModifyCurrentCellInfo(long lOldInfo, long lNewInfo, LPCTSTR OldScriptFileName, LPCTSTR NewScriptFileName) = 0;
	virtual void DeleteCurrentCellInfo() = 0;
	virtual void SetCurrentScriptFileName(LPCTSTR lpstrFileName) = 0;
	virtual void SetBrushInfo(int nWidth, eCellBrushShape eBrushType) = 0;
	virtual void SetDisplayParam(int nDisplayWidth, int nCellHorzHeight) = 0;
	virtual void SetShowRest(BOOL bShowRest) = 0;
	//About Logic Editor
	virtual HRESULT PaintSelectedLogicalCell(BOOL bAddCellInfo, BOOL bForceDel) = 0;
	virtual HRESULT RenderLogicalCell() = 0;
	virtual HRESULT SelectLogicalCells(int nXStart,int nZStart,IEKG3DSceneSceneEditor* pEditor,D3DXVECTOR3* pCameraPos,D3DXVECTOR3* pCameraDirct,D3DXVECTOR3 vGroundInter) = 0;
	virtual HRESULT UpdateEditingCells(int nXStart,int nZStart,IEKG3DSceneSceneEditor* pEditor,D3DXVECTOR3& InterPos) = 0;

	virtual HRESULT Init() = 0;
	virtual HRESULT UnInit() = 0;

	virtual BOOL GetShowLogicalCell() = 0;
	virtual BOOL GetShowLogicalTerrenCell() = 0;
	virtual BOOL GetShowLogicalItemCell() = 0;
	virtual BOOL GetShowLogicalSelectedCell() = 0;


	virtual HRESULT GetCellModifyState(DWORD* eState) = 0;
};

enum ENTITY_LIST_TYPE
{
	RENDER_ENTITY_LIST   = 0x00,
	SELECTED_ENTITY_LIST = 0x01
};

//创建一个完整的NPC需要这个，使用Engine的BuildKG3DRepresentObject可以在WParam传入这个结构体
#ifndef tstring
#if defined(UNICODE) | defined(_UNICODE)
#define tstring wstring
#else
#define tstring string
#endif
#endif
struct IEKG3DRepresentNPC_BuildStruct 
{
	INT	nSize;
	DWORD dwNPCTempleteID;
	DWORD dwRepresentID;
	LPCTSTR strNPCName;
	LPCTSTR strMdlName;
	LPCTSTR strAniName;
	LPCTSTR strFaceMesh;
	LPCTSTR strFaceMtl;
	INT nLevel;
	INT nType;
	//INT nAIType;
	FLOAT fScale;	
	FLOAT fAlertRange;
	FLOAT fAttackRange;
	DWORD dwReliveID;
	DWORD dwRandomID;
	DWORD dwAISet;
	IEKG3DRepresentNPC_BuildStruct()
	{
		ZeroMemory(this, sizeof(IEKG3DRepresentNPC_BuildStruct));
		nSize = sizeof(IEKG3DRepresentNPC_BuildStruct);
	}
};

interface IEKG3DRepresentNPC
{
    virtual float GetMoveSpeed() = 0;
	virtual HRESULT SetMoveSpeed(float fMoveSpeed) = 0;

    virtual HRESULT GetName(LPTSTR pName, size_t BufferSize) = 0;
    virtual HRESULT GetNPCTranslation(D3DXVECTOR3* pValue) = 0;
    virtual HRESULT GetNPCRotation(D3DXQUATERNION* pValue) = 0;
    virtual HRESULT GetKind(INT *pnRetKind) = 0;
    //virtual HRESULT GetAIKind(INT *pnRetAIKind) = 0;
    virtual HRESULT GetLevel(INT *pnRetLevel) = 0;
    virtual HRESULT GetRepresentID(DWORD *pdwRetID) PURE; 

    virtual HRESULT GetTemplateID(DWORD *pdwRetID) = 0; 

    virtual HRESULT SetNPCTranslation(D3DXVECTOR3* pValue) = 0;
    virtual HRESULT SetNPCRotation(D3DXQUATERNION* pValue) = 0;

	virtual HRESULT GetAlertRange(float *pnAlertRange) = 0;
	virtual HRESULT GetAttackRange(float *pnAttackRange) = 0;

	virtual void HideAttackRange() = 0;
	virtual void SetbAttackRange(BOOL val) = 0;
    //virtual HRESULT GetIsDisableAutoTurn(int* val) = 0;
    //virtual HRESULT SetIsDisableAutoTurn(int val) = 0;

	virtual DWORD GetReliveID() = 0;
	virtual DWORD GetRandomID() = 0;
	virtual HRESULT SetReliveID(DWORD dwReliveID) = 0;
	virtual HRESULT SetRandomID(DWORD dwRandomID) = 0;

	virtual DWORD   GetAISetID() = 0;
	virtual HRESULT SetAISetID(DWORD dwAI) = 0;
};

struct IEKG3DRepresentDoodad_BuildStruct 
{
	INT		nSize;
	DWORD	dwDoodadTempleteID;
	LPCTSTR pDoodadName;
	LPCTSTR pMeshName;
	LPCTSTR pMtlName;
	LPCTSTR pAniName;
	DWORD   dwReliveID;
	BOOL	bUseDefaultObstacle;
	BOOL    bGenerateObstacle;
	IEKG3DRepresentDoodad_BuildStruct()
	{
		ZeroMemory(this, sizeof(IEKG3DRepresentDoodad_BuildStruct));
		nSize = sizeof(IEKG3DRepresentDoodad_BuildStruct);
	}
};
interface IEKG3DRepresentDoodad
{
    virtual HRESULT GetName(LPTSTR pName, size_t BufferSize) = 0;
    virtual HRESULT GetTranslation(D3DXVECTOR3* pValue) = 0;
    virtual HRESULT GetRotation(D3DXQUATERNION* pValue) = 0;
    virtual HRESULT SetDoodadTranslation(D3DXVECTOR3* pValue) = 0;
    virtual HRESULT SetRotation(D3DXQUATERNION* pValue) = 0;
    virtual HRESULT GetTemplateID(DWORD *pdwRetID) = 0; 

	virtual DWORD GetReliveID() = 0;
	virtual HRESULT SetReliveID(DWORD dwReliveID) = 0;

	virtual BOOL GetUseDefaultObstacle() = 0;
	virtual HRESULT SetUseDefaultObstacle(BOOL bUseDefaultObstacle) = 0;
	virtual BOOL GetGenerateObstacle() = 0;
	virtual HRESULT SetGenerateObstacle(BOOL bGenerateObstacle) = 0;
};

interface IEKG3DRepresentTerrainBlock
{
	 //virtual HRESULT GetName(LPSTR pszName) = 0;
	//virtual HRESULT PasteTerrain(DWORD dwOption) = 0;

};
struct IEKG3DRepresentWayPoint_BuildStruct
{
	INT nSize;
	DWORD dwSetID;
	LPCTSTR tcsName;
	IEKG3DRepresentWayPoint_BuildStruct()
	{
		ZeroMemory(this, sizeof(IEKG3DRepresentWayPoint_BuildStruct));
		nSize = sizeof(IEKG3DRepresentWayPoint_BuildStruct);
	}
};
interface IEKG3DRepresentWayPoint
{
	virtual HRESULT SetWayPointType(int nType) = 0;
	//virtual HRESULT LoadFromFile(LPCTSTR cszFileName, unsigned uFileNameHash, unsigned uOption) = 0;//没有用过
	//virtual	HRESULT Copy(IEKG3DRepresentObject* pSrcObject) = 0;
	virtual	HRESULT SetWayPointName(LPCTSTR lpstrName) = 0;
	virtual DWORD GetSetID() = 0;
	virtual	HRESULT SetSetID(DWORD val) = 0;

	virtual HRESULT GetName(LPTSTR pName, size_t BufferSize) = 0;
	virtual HRESULT GetTranslation(D3DXVECTOR3* pValue) = 0;
	virtual HRESULT GetRotation(D3DXQUATERNION* pValue) = 0;
};

struct IEKG3DRepresentTrafficPoint_BuildStruct
{
	INT nSize;
	DWORD dwSetID;
	DWORD dwType;
	LPCTSTR pTrafficSetName;
	IEKG3DRepresentTrafficPoint_BuildStruct()
	{
		ZeroMemory(this, sizeof(IEKG3DRepresentTrafficPoint_BuildStruct));
		nSize = sizeof(IEKG3DRepresentTrafficPoint_BuildStruct);
	}
};
interface IEKG3DRepresentTrafficPoint
{
	virtual HRESULT SetSetID(DWORD val) = 0;
	virtual	DWORD GetSetID() = 0;
	virtual HRESULT GetTranslation(D3DXVECTOR3* pValue) = 0;
	virtual HRESULT SetKind(DWORD dwType) = 0; 
	virtual ULONG GetKind() = 0;//不能用GetType，否则和RepresentObject的重复
};
struct IEKG3DRepresentRefreshPoint_BuildStruct
{
	INT	nSize;
	DWORD dwType;
	DWORD dwID;
	LPCTSTR tcsName;
	IEKG3DRepresentRefreshPoint_BuildStruct()
	{
		ZeroMemory(this, sizeof(IEKG3DRepresentRefreshPoint_BuildStruct));
		nSize = sizeof(IEKG3DRepresentRefreshPoint_BuildStruct);
	}
};
interface IEKG3DRepresentRefreshPoint
{
	//virtual HRESULT LoadFromFile(LPCTSTR cszFileName, unsigned uFileNameHash, unsigned uOption) = 0;
	virtual	HRESULT SetRefreshPointName(LPCTSTR name) = 0;
	virtual	DWORD GetRefreshPointType() = 0;
	virtual	HRESULT SetRefreshPointType(DWORD val) = 0;

	virtual DWORD GetSetID() = 0;
	virtual HRESULT SetSetID(DWORD val) = 0;

	virtual HRESULT GetName(LPTSTR pName, size_t BufferSize) = 0;
	virtual HRESULT GetTranslation(D3DXVECTOR3* pValue) = 0;
	virtual HRESULT GetRotation(D3DXQUATERNION* pValue) = 0;
};

enum POLYUSEAGE
{
	LOGICAL_CELL_PAINTER,
	ENTITY_PLACER,
};

interface IEKG3DRepresentPoly
{
	virtual FLOAT GetVertexHeight() = 0;
	virtual INT   GetVertexCount() = 0;
	virtual D3DXVECTOR3 GetVertex(int nIndex) = 0;
	virtual HRESULT SetVertexHeight(FLOAT fHeight) = 0;
	virtual HRESULT SetVertexCount(INT nCount) = 0;
	virtual HRESULT SetVertex(int nIndex,D3DXVECTOR3 vPos) = 0;

	virtual DWORD GetColor() = 0;
	virtual INT GetLogicalSetID() = 0;

	virtual HRESULT SetColor(DWORD dwColor) = 0;
	virtual HRESULT SetLogicalSetID(INT nID) = 0;

	virtual BOOL IsPointIn(D3DXVECTOR3 vPos,BOOL bTestHeight) = 0;

	virtual HRESULT SetModifyState(DWORD dwState) = 0;
	virtual DWORD GetModifyState() = 0;

	virtual LPCTSTR GetScriptFileName() = 0;
	virtual HRESULT SetScriptFileName(LPCTSTR pFileName) = 0;

	virtual LPCTSTR GetPolyName() = 0;
	virtual HRESULT SetPolyName(LPCTSTR pName) = 0;

	virtual POLYUSEAGE GetPolyUseage() = 0;
	virtual HRESULT SetPolyUseage(POLYUSEAGE e) = 0;

	virtual LPCTSTR GetLogicalSetName() = 0;
	virtual HRESULT SetLogicalSetName(string strName) = 0;

	virtual PVOID GetObjectPointer() = 0;
};

interface IKG3DPvsPortal {
    virtual HRESULT GetBBox(AABBOX* pAABBox) = 0;
    virtual LPCTSTR GetName() = 0;
    virtual void SetName(LPCTSTR szName) = 0;
};

interface IEKG3DRepresentPVS
{
    virtual HRESULT SavePvsFile(LPCTSTR szFileName) = 0;
    virtual HRESULT LoadPvsFile(LPCTSTR szFileName, DWORD dwOption) = 0;

    virtual HRESULT Clear(BOOL bClearEntity) = 0;

    virtual HRESULT SetOutsideBorderModel(LPCTSTR fileName) = 0;
    virtual HRESULT SetInsideBorderModel(LPCTSTR fileName)  = 0;

    virtual IEKG3DModel* GetOutsideBorderModel() = 0;
    virtual IEKG3DModel* GetInsideBorderModel()  = 0;;

    virtual LPCTSTR GetName()                       = 0;
    virtual void SetName(LPCTSTR strName)           = 0;


	virtual int GetPortalCount() = 0;
	virtual IKG3DPvsPortal* GetPortal(DWORD dwIndex) = 0;
	virtual HRESULT DeletePortal(DWORD dwIndex) = 0;

    virtual void NextSubOctree() = 0;

	//virtual HRESULT SetBorderBox(float fLength,float fWidth,float fHeight) = 0;
	//virtual HRESULT GetBorderBox(float* pfLength,float* pfWidth,float* pfHeight) = 0;
	//virtual HRESULT AutoComputeBorderBox() = 0;
	virtual BOOL    GetRenderPortal() = 0;
	virtual HRESULT SetRenderPortal(BOOL nEnable) = 0;
	virtual BOOL    GetRenderBorder() = 0;
	virtual HRESULT SetRenderBorder(BOOL nEnable) = 0;

	virtual int GetInsideObjectCount() = 0;
	virtual IEKG3DRepresentObject* GetInsideObject(DWORD dwIndex) = 0;

    virtual void ShowOutsideShell(BOOL bShow) = 0;
    virtual void ShowInsdieShell(BOOL bShow) = 0;

};

interface IEKG3DRepresentAntiPortal
{
	virtual DWORD GetPortalType() = 0;
	virtual HRESULT SetPortalType(DWORD dwType) = 0;

	virtual HRESULT GetData(float* pWidthBottom,float* pWidthTop,float* pHeight) = 0;
	virtual HRESULT SetData(float  fWdithBottom,float fWdithTop,float fHeight) = 0;
};

class KScene;
class KRegion;
class KCell;
struct IELogicalVertex 
{
	D3DXVECTOR3 posi;
	D3DCOLOR    color;
};

interface IELogicalSceneConnection //提供游戏逻辑数据转换的回调接口
{
	virtual HRESULT BindLogicalScene(KScene* pScene) = 0;
	virtual BOOL CheckObstacle(int nXCell, int nYCell, int nLayer) = 0;
	virtual KRegion* GetRegion(int nRegionX, int nRegionY) = 0;
	virtual HRESULT FillLogicalCellsInfo(
		D3DXVECTOR3 A,int X,int Z,int& nCellCount,KCell* pCell,IELogicalVertex* pLLine, IELogicalVertex* pLFace, 
		BOOL, BOOL, BOOL, int) = 0;
	virtual KCell* GetCellOfRegion(KRegion* pRegion, INT nIndex) = 0;//nIndex在Region中开始数
	virtual FLOAT GetLogicalHeight(const D3DXVECTOR3& vLogicalCoord, size_t* pOrderOfCell) = 0;	//得到脚下的逻辑高度，注意算法
	virtual KCell* GetCurOrLowerObstacle(const KRegion& regionCur, int nXCell, int nYCell, int nZ, size_t* pOrderOfCell) = 0;//返回点所在的Cell，或者它下面的那个Cell
	virtual KCell* GetNextCell(const KCell* pCell, BOOL bIgnoreDynamicSwitch) = 0;
	virtual KCell* GetLowestObstacle(const KRegion& regionCur, int nXCell, int nYCell) = 0;
	virtual POINT LogicalToLogicalRegionIndex( FLOAT fLogicalX, FLOAT fLogicalY ) = 0;
};




typedef struct _KG3DTERRAIN_LOAD_PARAM
{
	DWORD dwModificationOption;
	int nStart_X;	///允许负值
	int nStart_Y;
	int nWidth;		///>0
	int nHeight;
	int nSection;      // 是否创建nSection
	int nRegion;	   // 是否创建nRegion
	int nZoneTerrainBlock;   // 是否创建Zone的TerrainBlock 
	int nSectionTerrainBlock;// 是否创建Section的TerrainBlock
	int nRegionTerrainBlock; // 是否创建Region的TerrainBlock
	string strHeightMapName;
	int nHeightMapWidth;
	int nHeightLowest;
	int nHeightTotal;
#ifdef __cplusplus
	_KG3DTERRAIN_LOAD_PARAM()
		:dwModificationOption(0)
		,nStart_X(0)
		,nStart_Y(0)
		,nWidth(0)
		,nHeight(0)
		,nSection(1)
		,nRegion(1)
		,nZoneTerrainBlock(1)   
		,nSectionTerrainBlock(1)
		,nRegionTerrainBlock(1) 
		,nHeightMapWidth(0)
		,nHeightLowest(-200)
		,nHeightTotal(40000)
	{
		
	}
#endif
}KG3DTERRAIN_LOAD_PARAM;

interface IEKG3DDetailMaterialMgr
{
	virtual HRESULT GetMaterial(int nIndex,LPSTR pName,int& nTextureCount) = 0;
	virtual HRESULT GetTextureForMaterial(int nMtlIndex,int nIndex,int& nTextID) = 0;

	virtual HRESULT SetCurrentPaintMtl(int nIndex) = 0;
	virtual HRESULT SetCurrentPaintLevel(int nLevel) = 0;
};

interface IEKG3DEnvEffLight;

interface IEKG3DWallDefine
{
	virtual ULONG STDMETHODCALLTYPE AddRef() = 0;
	virtual ULONG STDMETHODCALLTYPE Release() = 0;
	virtual int GetNumElement() = 0;
	virtual HRESULT GetElement(int nIndex,KG3DWALLELEMENTTYPE* pType,D3DXVECTOR3* pA,D3DXVECTOR3* pB,LPSTR pFileName) = 0;
	virtual HRESULT SetElement(int nIndex,KG3DWALLELEMENTTYPE eType,D3DXVECTOR3 vA,D3DXVECTOR3 vB,LPSTR pFileName,BOOL bComputeFromMesh) = 0;
	virtual HRESULT NewOneElement(int* pIndex) = 0;
	virtual HRESULT DeleteElement(int nIndex) = 0;
	virtual HRESULT LoadFromFile(LPSTR pFileName) = 0;
	virtual HRESULT SaveToFile(LPSTR pFileName) = 0;
};

interface IEKG3DWall
{
	virtual IEKG3DWallDefine* GetWallDefine() = 0;
	virtual HRESULT SetOption(BOOL bOrthogonality,BOOL bLockXZ,BOOL bAutoPitch) = 0;
	virtual HRESULT GetOption(BOOL& bOrthogonality,BOOL& bLockXZ,BOOL& bAutoPitch) = 0;

	virtual int GetNumWall() = 0;
	virtual HRESULT GetWall(int nID,int& nPoint,int& nLine) = 0;
	virtual HRESULT GetWallLine(int nID,int nIndex,BOOL& bStartEnd, BOOL& bDoor) = 0;
	virtual HRESULT SetWallLine(int nID,int nIndex,BOOL  bStartEnd, BOOL  bDoor) = 0;
	virtual HRESULT NewOneWall(int& nID) = 0;
	virtual HRESULT DeleteOneWall(int nID) = 0;

	virtual HRESULT Fix() = 0;
};
class IKG3DMessageReceiverBase;

enum
{
	EM_SCENE_ADD_OPTION_SELECTABLE = 1,
	EM_SCENE_ADD_OPTION_SELECTED = 1 << 1,
	EM_SCENE_ADD_OPTION_TO_FILE = 1 << 2,
	EM_SCENE_ADD_OPTION_TO_PHYSICS = 1 << 3,
};

interface IEKG3DSceneSceneEditor
{
	//virtual void GetfModelLodLevel(float* fLodLevel) = 0 ;
	//virtual void SetfModelLodLevel(float fLodLevel) = 0 ;//把这个选项改到KG3DEngineOption.fModelLODSwitchGates[0]

	virtual void SetUseBoneBox(BOOL val)=0;
	virtual BOOL GetUseBoneBox()=0;
	//(by huangjinshow 2007年7月12日
	virtual HRESULT EnableRenderAlertRange(BOOL bEnable) = 0;
	virtual HRESULT EnableRenderAttackRange(BOOL bEnable) = 0;
	//)

    virtual void ZoomCoordinate(float fZoom) = 0;
    virtual void    SetCoordMode(DWORD dwMode) = 0;

    virtual HRESULT UpdateEditSpaceNodeInformation() = 0;
    virtual HRESULT NotifyObjInfomationChanged(IEKG3DRepresentObject* pObj) = 0;

    virtual IEKG3DWaterEffect* GetWaterEffect() = 0;

    virtual void EnablePvsCulling(BOOL bEnable) = 0;
    virtual BOOL IsSelectedPvs() = 0;

    virtual void EnableShowSelectedBsp(BOOL bShow) = 0;


	//<Obj操作>
	virtual IEKG3DRepresentNPC* GetIEKG3DRepresentNPC(PVOID pObject) = 0;
	virtual IEKG3DRepresentDoodad* GetIEKG3DRepresentDoodad(PVOID pObject) = 0;
	virtual IEKG3DRepresentWayPoint* GetIEKG3DRepresentWayPoint(PVOID pObject) = 0;
	virtual IEKG3DRepresentTrafficPoint* GetIEKG3DRepresentTrafficPoint(PVOID pObject) = 0;
	virtual IEKG3DRepresentRefreshPoint* GetIEKG3DRepresentRefreshPoint(PVOID pObject) = 0;
	virtual HRESULT GetRepresentNPC(IEKG3DRepresentNPC** ppNPC) = 0;


	virtual HRESULT ReplaceSelectedObjectWithInHand() = 0;
	virtual HRESULT GetSelectedObjectInHand() = 0;
	virtual HRESULT GetObjectInHandle(IEKG3DRepresentObject** pObj) = 0;
	virtual HRESULT SetObjectInHandle(IEKG3DRepresentObject* pObject, BOOL bReleaseHandleWhenAnotherIsSetted) = 0;

	STDMETHOD(AddRepresentObject)(IEKG3DRepresentObject* pObject, ULONG uOption, IEKG3DRepresentObject** ppCreatedObject) PURE;//用这个函数的话，Scene会接管生命周期，如果不想这样的话，用AddRenderEntity。uOption是	EM_SCENE_ADD_OPTION_SELECTABLE 的组合，注意如果ToFile，会创建出新的物件。新的物件可用ppCreateObject取得
	STDMETHOD(RemoveRepresentObject)(IEKG3DRepresentObject* pObject) PURE;
	STDMETHOD(DelRepresentObject)(IEKG3DRepresentObject* pObject) PURE;

	//</Obj操作>

	//<Add，Remove Entity，Object>//改用下面的新的选择器接口，
	virtual HRESULT RemoveRenderEntity(SCENEENTITYTYPE type,PVOID pPointer) = 0;
	virtual HRESULT RemoveRenderEntityAndNotifyLogic(SCENEENTITYTYPE type,PVOID pPointer) = 0;
	virtual HRESULT AddRenderEntity(SCENEENTITYTYPE type,PVOID pPointer) = 0;


	//<Select>新的选择器接口IEKG3DSceneSelectionTool中的方法
	//virtual HRESULT SetSelectable(const IEKG3DSceneEntity& Entity, BOOL bSelectable) = 0;//<Selectable Refactor>
	virtual HRESULT SetSelected(const IEKG3DSceneEntity& Entity, BOOL bSelected) = 0;
	//virtual HRESULT SetSelectable(SCENEENTITYTYPE dwType, LPVOID Pointer, BOOL bSelectable) = 0;//<Selectable Refactor>
	virtual HRESULT SetSelected(SCENEENTITYTYPE dwType, LPVOID Pointer, BOOL bSelected) = 0;
	virtual HRESULT SetSelected(IEKG3DRepresentObject* pObject, BOOL bAdd) = 0;
	//</Select>

	virtual HRESULT SetLogicalSceneConnection(IELogicalSceneConnection* pLogicalSceneConnection) = 0;

    virtual HRESULT Advise(IEKG3DSimpleSink* pNewSink) = 0; //增加一个观察者
    virtual HRESULT Unadvise(IEKG3DSimpleSink* const pNewSink) = 0;   //删除一个观察者

   
	virtual HRESULT GetSceneDetailMaterialMgr(IEKG3DDetailMaterialMgr** ppMtl) = 0;
    //virtual HRESULT SetToEditorTerrainState() = 0;
	virtual HRESULT SetNPCPosition(BOOL UseRayIntersectionPosOrInputPos, BOOL bTopViewSet, const D3DXVECTOR3* pInputPos) = 0;
	//virtual HRESULT GetIEKG3DMeshLensFlare(IEKG3DMeshLensFlare **ppLensFlare) = 0;
	virtual HRESULT GetIELogicScene(IEKG3DSceneLogicalEditorBase** pLogicScene) = 0;
	virtual HRESULT SetEditState(DWORD dwEditState) = 0;
	virtual HRESULT GetCurOutputWnd(IEKG3DSceneOutputWnd **ppiRetCurOutputWnd) = 0;
	
	virtual HRESULT AddOutputWindow(LPCTSTR szName, HWND hWnd, DWORD dwType, DWORD_PTR dwOption, int* pOutputWindowID) = 0;
	virtual HRESULT RemoveOutputWindow(int nOutputWindowID) = 0;
	virtual HRESULT GetEditState(DWORD* pdwState) = 0;
	virtual HRESULT SetSampleMip(D3DTEXTUREFILTERTYPE eType) = 0;
	virtual HRESULT SetSampleMag(D3DTEXTUREFILTERTYPE eType) = 0;
	virtual HRESULT SetSampleMin(D3DTEXTUREFILTERTYPE eType) = 0;
	virtual HRESULT SetMipmapLod(float fValue) = 0;

	virtual HRESULT GetScene(IEKG3DScene** ppScene) = 0;
	virtual HRESULT GetSceneEditorBase(IEKG3DSceneEditorBase** ppSceneBase) = 0;

	//virtual HRESULT GetSelectedEntityCount(INT* nCount) = 0;
	//virtual HRESULT GetSelectedEntity(INT nIndex, IEKG3DSceneEntity** ppEntity) = 0;
	virtual void OnCommand(enumCommandType eCommandType) = 0;

	//virtual HRESULT BegainComputeEntityLightmap(ENTITY_LIST_TYPE eEntityList) = 0;
	//virtual HRESULT BegainComputeTerrainRegionLightmap(BOOL bVisible) = 0;
	//virtual HRESULT BegainComputeTerrainRegionSmallMap(BOOL bVisible) = 0;

	//virtual BOOL  HasRenderOption(UINT flag) = 0;
	//virtual void  DisableRenderOpion(UINT flag) = 0;
	//virtual void  EnableRenderOption(UINT flag) = 0;

	virtual BOOL GetWaterReflect() = 0;
	virtual HRESULT SetWaterReflect(BOOL bValue) = 0;

    virtual BOOL GetWaterRefract() = 0;
    virtual HRESULT SetWaterRefract(BOOL bValue) = 0;

	virtual float GetZFar() = 0;
	virtual HRESULT SetZFar(float fZFar) = 0;
	virtual HRESULT ImportOldFile(LPSTR pFileName) = 0;

	//virtual INT GetSizeOflistEntityRenderLast() = 0;
	virtual INT GetSizeOflistRenderEntity() = 0;
	virtual HRESULT	GetSelecteNPC(IEKG3DModel*& pModel) = 0;
	virtual HRESULT LogicalToScene(D3DXVECTOR3* pPos, float fX, float fY, float fZ, BOOL bOnFloor) = 0;
	virtual HRESULT SceneToLogical(int* pX, int* pY, int* pZ, D3DXVECTOR3 Pos) = 0;
	virtual HRESULT GetSceneSelector(IEKG3DSceneSelectBase** ppSelector) = 0;
	virtual HRESULT GetSelectionToolInterface(IEKG3DSceneSelectionTool** ppTool) = 0;
	virtual HRESULT UnitSelectEntity() = 0;
	virtual HRESULT UnUnitSelectEntity() = 0;
	virtual HRESULT AddOneMeasureTool() = 0;
	virtual HRESULT ShockwaveEntity() = 0;
	virtual HRESULT GetFirstSelectedObject(IEKG3DRepresentObject** ppObj) = 0;

	
	virtual HRESULT GetPrimaryWindow(IEKG3DSceneOutputWnd **ppiRetPrimaryWindow)=0;
	
	virtual HRESULT GroupSameNameAndTypeObjects() = 0;
    virtual HRESULT RenderSceneSnap(LPSTR pFileName) = 0;
	virtual HRESULT ExprotTerrainTexture(LPSTR pFileName) = 0;
	//virtual HRESULT EndComputeTerrainRegionLightmap() = 0;
	//virtual HRESULT ClearLightMapEntityQuene() = 0;
	//virtual HRESULT ClearSmallMapRegionQuene() = 0;

	virtual D3DXVECTOR3 GetCameraTopPos() = 0;
	virtual HRESULT SetCameraTopPos(float x, float z) = 0;
	virtual D3DXVECTOR3 GetCameraMainPos() = 0;
	virtual float GetCameraTopScale() = 0;
	virtual HRESULT SetCameraTopScale(float fScale) = 0;
	virtual HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0;//<PROCESS_MESSAGE>
	virtual HRESULT AddSelectedEntityByIndex(INT Index) = 0;
	virtual INT GetObjectGroupSize() = 0;
	virtual HRESULT GetGroupInfo(INT nIndex, INT& nGroupSize, LPTSTR szName, INT nBufLen) = 0;
	virtual HRESULT DeleteSelectedEntity() = 0;

	

	virtual HRESULT ClearAllHelpLine() = 0;
	virtual DWORD GetHelpLineSize() = 0;
	virtual HRESULT ClearSetHelpLine(int nSetID) = 0;
	
	virtual HRESULT SetHelpLine(size_t nLine,int nSetID, size_t nBegin, size_t nCount, D3DCOLOR color, D3DXVECTOR3* pPos) = 0;
	virtual HRESULT SetHelpLineStart(size_t nLine, size_t nStart) = 0;
	
	virtual HRESULT ClearSelectedEntityList() = 0;
    virtual HRESULT IESetLogicalScene(IEKG3DSceneLogicalEditorBase* pLogicalScene) = 0;
   
    virtual HRESULT IEOnResizeWindow(PRECT const pRect) = 0;
  
    virtual HRESULT SetSnapToolScale(FLOAT fScale) = 0;
    virtual HRESULT AddSelectedEntityToAIGroup(DWORD dwSetID) = 0;
    virtual HRESULT RemoveSelectedEntityFromAIGroup(DWORD dwSetID) = 0;

    virtual HRESULT AddSelectedEntityToDoodadRefreshSet(DWORD dwSetID) = 0;
    virtual HRESULT RemoveSelectedEntityToDoodadRefreshSet(DWORD dwSetID) = 0;

    virtual HRESULT RemoveSelectedEntityToNPCRefreshSet(DWORD dwSetID) = 0;
    virtual HRESULT AddSelectedEntityToNPCRefreshSet(DWORD dwSetID) = 0;


    virtual INT IsPMode() = 0;

	//<属性>
	virtual HRESULT SetUseCrossPlayMode(BOOL bEnable) = 0;
	virtual ULONG IsUseCrossPlayMode() = 0;
	//</属性>

   
    STDMETHOD(BeginMovieREC)(
        SIZE                 Size, 
        FLOAT                fInterval, 
        D3DTEXTUREFILTERTYPE FiterType  = D3DTEXF_LINEAR,
        D3DXIMAGE_FILEFORMAT ImageType = D3DXIFF_JPG,
        INT                  nAutoScale = TRUE
    ) PURE; 
    STDMETHOD(EndMovieREC)()   PURE; 
    STDMETHOD(PlayEntityAnimation)(int nPlay/*true to play false to pause*/) PURE;

	virtual HRESULT ForceLoadAllEntity() = 0;
	virtual HRESULT ForceLoadEntity(D3DXVECTOR2 A, D3DXVECTOR2 C) = 0;
	virtual HRESULT ComputeAllEntityBBox() = 0;
	virtual HRESULT ComputeTerrainNormal() = 0;
	virtual HRESULT CompositorTerrainTexture() = 0;
	virtual HRESULT RefreshTerrain() =0;
	virtual HRESULT EnableLoadTexture(BOOL bEnable) = 0;
	virtual HRESULT PauseMultiThreadLoad(BOOL bEnable) = 0;
	//////////////////////////////////////////////////////////////////////////
	//区域选择与编辑
	virtual HRESULT BeginSelectSceneBlock(BOOL bZone,BOOL bSection,BOOL bRegion,BOOL bOnlyExist) = 0;
	virtual HRESULT EndSelectSceneBlock(int nAddFlag, int nFinishFlag) = 0;
	virtual HRESULT AddSelectSceneBlockPoint(int nIndex,D3DXVECTOR3 vPos) = 0;
	virtual HRESULT EnableRenderSelectedSceneBlock(BOOL bEnable) = 0;
    virtual std::list<POINT>* GetCurSelectedSceneBlock(int nLevel) = 0;

	virtual HRESULT CreateSelectedSceneBlock(BOOL bCreateTerrain) = 0;
	virtual HRESULT DeleteSelectedSceneBlock(BOOL bOnlyDeleteTerrain) = 0;
	virtual HRESULT ApplyHeightMapToSelectedSceneBlock(LPTSTR pHeightMap,int nWidth,float fLowest,float fHeightTotal) = 0;
	virtual HRESULT ApplyBaseMapToSelectedSceneBlock(LPTSTR pBaseMap) = 0;

	//////////////////////////////////////////////////////////////////////////
	//多边形编辑
	virtual HRESULT IELoadPoly(LPCTSTR pScriptFileName,LPCTSTR pPolyName,DWORD dwColor,LPCTSTR szLogicalSetName,int nLogicalSetIndex,DWORD dwModifyState,float fVertexHeight,int nNumVertex,D3DXVECTOR3 vPos[]) = 0;
	virtual HRESULT BeginAddPolyVertex() = 0;
	virtual HRESULT EndAddPolyVertex(DWORD dwType) = 0;
	virtual FLOAT GetPolyHeight() = 0;
	virtual HRESULT SetPolyHeight(FLOAT fH) = 0;

	virtual IEKG3DRepresentPoly* GetSelectedPoly() = 0;

	virtual INT GetLogicalPolyIntersectRect(IEKG3DRepresentPoly* pPolyArray[],INT nMax,D3DXVECTOR2 A,D3DXVECTOR2 C) = 0;

	virtual HRESULT EnableRenderPoly(BOOL bEnable) = 0;
	virtual BOOL IsRenderPoly() = 0;

	virtual HRESULT MoveScene(int nXStart,int nZStart) = 0;
	
	virtual HRESULT AddPolyVetexInTopView(D3DXVECTOR3& vInter)= 0;

	virtual HRESULT BuildPolyAndSetInHand(int nVergeNum,float fRadii,float fHeight,float fDegree) = 0;
	//地形，多地形////////////////////////////////////////////////////////////////////////
	virtual HRESULT AddOneMap(LPCTSTR pFileName) = 0;
	virtual HRESULT LoadDefaultTerrain(KG3DTERRAIN_LOAD_PARAM* pLoadParam) = 0;

	//////////////////////////////////////////////////////////////////////////
	//场景音效相关
	virtual BOOL IsSceneSoundEnable() = 0;
	virtual HRESULT EnableSceneSound(BOOL bEnable) = 0;
	virtual BOOL IsSceneSoundVisiable() = 0;
	virtual HRESULT VisiableSceneSound(BOOL bVisiable) = 0;
	virtual BOOL IsSoundRangeVisible() = 0;
	virtual void VisibleSoundRange(BOOL bVisible) = 0;

	//PVS
	virtual void EnablePVS(BOOL bEnable) = 0;
	virtual BOOL GetPVSState() = 0;	

	//得到原始的I接口
	virtual IKG3DScene* GetIInterface() = 0;
	///////by huangjinshou 2007.11.15//////////////////////////////////////////////////////
	virtual HRESULT SetOpenAddTexture(BOOL bFlag) = 0;
	virtual BOOL GetOpenAddTexture() const = 0;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//pvs
	virtual HRESULT	GetCurrentSelectedObjectPVS(IEKG3DRepresentPVS** ppPVS) = 0;
	virtual HRESULT ConvertCurrentSelectedObjectToPVS(IEKG3DRepresentPVS** ppPVS) = 0;
	virtual HRESULT MoveSceneObjectIntoPVS(IEKG3DRepresentPVS* pPVS) = 0;
	virtual HRESULT MoveObjectFromPVSToScene(IEKG3DRepresentPVS* pPVS) = 0;

	//////////////////////////////////////////////////////////////////////////
	//antiportal
	virtual HRESULT	GetCurrentSelectedObjectAntiPortal(IEKG3DRepresentAntiPortal** ppAntiPortal) = 0;
	virtual HRESULT ConvertCurrentSelectedObjectToAntiPortal(IEKG3DRepresentAntiPortal** ppAntiPortal) = 0;

	//////////////////////////////////////////////////////////////////////////
	//Road && River
	virtual HRESULT AddNodeToScene(DWORD dwNodeType) = 0;
	virtual HRESULT DeleteNodeFromScene(DWORD dwNodeType) = 0;
	virtual void SetRenderNode(BOOL bRender) = 0 ;
	virtual BOOL GetRenderNode() = 0;
	///////////////////////////////////////////////////////////////////////////
	//地形区域选择复制信息处理
	virtual HRESULT ResetSelectTerrain() = 0;
	virtual HRESULT SelectTerrainFinished() = 0;
	virtual HRESULT PasteSeclctTerrain(DWORD dwMixType,bool bIsPasteBlend,bool bIsPasteObject,bool bIsMax,float fWeight,float fWidth) = 0;
	virtual HRESULT SaveTerrainBlock(const char cszFileName[]) = 0;
	virtual HRESULT LoadTerrainBlock(const char cszFileName[]) = 0;
	virtual HRESULT TerrainCopyWindowOnShow() = 0;
	virtual HRESULT TerrainCopyWindowOnHide() = 0;


    // pvs
    virtual HRESULT AddPvsPortal(IEKG3DRepresentPVS* pPvs, const D3DXVECTOR3& vPos,
        const D3DXVECTOR3& vDir, float fWidth, float fHeight) = 0;

    virtual HRESULT AddPvsPortal() = 0;

    // for new map version
    virtual HRESULT LockSelSpaceNode() = 0;
    virtual HRESULT UnLockSpaceNode() = 0;

    // not user in new map version
    //virtual ISelectFilter* GetSelFilter() = 0;//<ISelectFilter Refactor>
    //virtual void   SetSelFilter(ISelectFilter* pSelectFilter) = 0;//<ISelectFilter Refactor>
    ///////////////////////////////////////////////////////////////////////////
	//交通点相关
    virtual HRESULT AddTrafficPoint(IEKG3DRepresentObject** ppPoint,D3DXVECTOR3& vecPos,DWORD dwType,DWORD dwSetID) = 0;

	//万能接口
	virtual HRESULT GetEngineIEInterface(DWORD dwType, DWORD_PTR wParam, DWORD_PTR lPram, VOID** ppiInterface) = 0;//得到的接口，如果有Release方法的，用完必须调用Release
	virtual HRESULT GetEngineIEAbstructInterface(DWORD dwAbstructType, DWORD dwType,  DWORD_PTR wParam, DWORD_PTR lParam, VOID** ppiInterface) = 0;//得到的接口，如果有Release方法的，用完必须调用Release
	//添加草，石头，灌木相关
	virtual HRESULT GetTBModifierTextureStride(int* pX,int* pY,DWORD dwType) = 0;
	virtual HRESULT SetTBModifierTextureStride(int  nX,int  nY,DWORD dwType) = 0;
	virtual HRESULT GetTBModifierWidthMax(FLOAT* pValue) = 0;
	virtual HRESULT GetTBModifierWidthMin(FLOAT* pValue) = 0;
	virtual HRESULT SetTBModifierWidthMax(FLOAT fValue) = 0;
	virtual HRESULT SetTBModifierWidthMin(FLOAT fValue) = 0;
	virtual HRESULT GetTBModifierHeightMax(FLOAT* pValue) = 0;
	virtual HRESULT GetTBModifierHeightMin(FLOAT* pValue) = 0;
	virtual HRESULT SetTBModifierHeightMax(FLOAT fValue) = 0;
	virtual HRESULT SetTBModifierHeightMin(FLOAT fValue) = 0;
	virtual HRESULT SetTBModifierNumPerM2(FLOAT fValue) = 0;
	virtual HRESULT SetTBModifierTextureIndexInfo(INT* pPointer, INT* pNumIndex) = 0;
	virtual HRESULT SetTBModifierState(DWORD dwState) = 0;
	virtual DWORD   GetTBModifierState() = 0;
	virtual HRESULT UpdatePatternList(INT* pList,INT nCount)=0;
	virtual HRESULT SetTBModifierTextureIndexMax(INT nMax) =0;
	virtual HRESULT SetTBModifierTextureIndexMin(INT nMin) = 0;
	virtual IEKG3DSceneOutDoorSpaceMgr* GetIEKG3DSceneOutDoorSpaceMgr() = 0;
   	virtual HRESULT GSFFitGround() = 0;
	virtual int GetTBModifierGrassDensity() =0;
	virtual HRESULT SetTBModifierGrassDensity(int nDensity) =0;
	virtual HRESULT DeleteAutoGrass(BYTE byDetailTexIndex) =0;
	virtual HRESULT BuildAutoGrass(BYTE byDetailTexIndex) = 0;
	virtual HRESULT SetAddAutoGrass(BOOL bVal) = 0;
	virtual HRESULT ClearAllTerrainObject() = 0;
	//跟地形相关
	virtual HRESULT GetTerrainModifyHeight(float& fHeight) = 0;
	virtual HRESULT SetTerrainModifyHeight(float fHeight) = 0;
	virtual HRESULT SetOppositeHeightValue(FLOAT fOppValue) = 0;
	virtual HRESULT SetIsGetAbsoluteHeight(BOOL bIsAbs) = 0;
	virtual float   GetTerrainModifyAlpha() = 0;
	virtual HRESULT SetTerrainModifyAlpha(float fAlpha) = 0;
	virtual DWORD   GetInnerHeightBrushSize() = 0;
	virtual DWORD   GetOuterHeightBrushSize() = 0;
	virtual HRESULT SetInnerHeightBrushSize(DWORD dwSize) = 0;
	virtual HRESULT SetOuterHeightBrushSize(DWORD dwSize) = 0;
	virtual HRESULT SetDiShu(FLOAT fPower) = 0;
	virtual HRESULT ReSizeHeightData(const WORD BrushSize) = 0;

	virtual HRESULT GetGroundHeight(float& fHeight,float x,float y) = 0;
	virtual HRESULT SetPaintTexIndex(BYTE byIndex) = 0;
	virtual BYTE GetPaintTexIndex() = 0;
	virtual HRESULT SetCurrentBrushMap(LPCTSTR pFileName) = 0;
	virtual HRESULT SetRenderBlockEdge(BOOL bVal) = 0;
	virtual HRESULT SetRenderWaterEdge(BOOL bVal) = 0;
	virtual HRESULT PaintTerrainTextureFromFilterMap(LPCSTR pFileName) = 0;
	virtual HRESULT ExprotFilterMap(LPCSTR pFileName) = 0;
	virtual HRESULT ClearTextureFromTerrain(BYTE byTexIndex) = 0;
	virtual HRESULT ExprotNormalMap(LPCSTR pFileName) = 0;
	//////////////////////////////////////////////////////////////////////////
	//墙体工具相关
	virtual HRESULT GetDefaultWallDefine(IEKG3DWallDefine** ppWallDefine) = 0;
	virtual HRESULT SetWallConnectionPoint(BOOL bShow,D3DXVECTOR3 vA,D3DXVECTOR3 vB) = 0;
	virtual IEKG3DWall* GetFirstSelectedObjectWall() = 0;
	virtual HRESULT AddWallNodeToScene(IEKG3DWall* pWall,int nWall,int nKeyPoint) = 0;

	virtual HRESULT GetLODProperties(FLOAT* pfLODHightToMiddleSwitchAngleGate, FLOAT* pfLODMiddleToLowSwitchAngleGate) = 0;
	virtual HRESULT SetLODProperties(FLOAT fLODHightToMiddleSwitchAngleGate, FLOAT fLODMiddleToLowSwitchAngleGate) = 0;

	//////////////////////////////////////////////////////////////////////////
	//地形高度图导出导入相关
	virtual HRESULT ExprotTerrainHeightMap(LPCSTR pFileName,DWORD dwOption) = 0;
	virtual HRESULT ImportTerrainHeightMap(LPCSTR pFileName,DWORD dwOption) = 0;
	virtual HRESULT GetSceneHeightInfo(int* pLowest,int* pHeightTotal) = 0;
	virtual HRESULT SetSceneHeightInfo(int  nLowest,int  nHeightTotal) = 0;
	//水面
	virtual HRESULT RefreshWaterEffect() = 0 ;
	virtual HRESULT SetTerrainWaterDefaultHeight(float fHeight) = 0;
	virtual HRESULT SetAlwayRenderWater(BOOL bVal) = 0;
	virtual BOOL    GetAlwayRenderWater() = 0;
	//////////////////////////////////////////////////////////////////////////
	//游戏逻辑区域的显示
	virtual HRESULT EnableShowLogicalSize(BOOL bEnable) = 0;
	virtual HRESULT EnableShowAppointLogicalRegion(BOOL bEnable) = 0;
	//是否线框模式渲染
	virtual HRESULT SetEnableWireFrameMode(BOOL bEnable) = 0;
	////////////////////////////////////////////////////////////////////////////
	virtual DWORD GetEntityNum() = 0;
	//////////////////////////////////////////////////////////////////////
	virtual HRESULT ReloadTerrainRenderShader()=0;


	// 策划场景编辑器
	virtual HRESULT SetCameraSpeed(FLOAT fSpeed) = 0;
	virtual HRESULT GetCameraPosLookat(AtlVector3* pos, AtlVector3* lookat) = 0;
	virtual HRESULT SetCameraPosLookat(AtlVector3* pos, AtlVector3* lookat) = 0;
	virtual HRESULT UpdateSelectableEntityList(LONG iCount) = 0;
	virtual HRESULT NpcPlayNextAction() = 0;
	virtual HRESULT NpcPlayPreviousAction() = 0;
	virtual HRESULT SetCameraLocation(FLOAT fPositionX, FLOAT fPositionY, FLOAT fPositionZ, FLOAT fDirectionX, FLOAT fDirectionY, FLOAT fDirectionZ) = 0;
	virtual HRESULT RotateSelectedObject(LPTSTR newRotationStr) = 0;
	virtual HRESULT GetSceneRect(LONG* width, LONG* height) = 0;
	virtual HRESULT SetObjectSelectMaskDesign(DWORD mask) = 0;
	virtual HRESULT SetCurrentEditBrushIndex(INT nIndex) = 0;

	//
	virtual void SetSelectOption(DWORD dwOption) = 0;
	virtual DWORD GetSelectOption() = 0;
	virtual void SetLimitValue(DWORD dwLimitValue) = 0;
	virtual DWORD GetLimitValue() = 0;

	virtual HRESULT SetHelpLineName(size_t nLine,LPSTR pName) = 0;

	virtual void ReplaceSelectedEntity(bool all) = 0;  // by likan
	virtual void FindLoadedNpc(vector<IEKG3DRepresentObject *>&npcs, DWORD templateid) = 0; // by likan
	virtual void FindLoadedDoodad(vector<IEKG3DRepresentObject *>&doodads, DWORD templateid) = 0; //by likan

    virtual HRESULT UpdateSettings(DWORD dwSettingType) = 0;

	virtual void EnableBlockBox(BOOL bEnable) = 0;

};//End of IEKG3DSceneSceneEditor

interface IEKG3DSpeedTreeModifier
{
	virtual BOOL SelectLeaf(IELeaf* pLeaf) = 0;
	virtual BOOL UnSelectLeaf(IELeaf* pLeaf) = 0;
	virtual void DeleteLeaf(IELeaf* pLeaf) = 0;
	virtual void AddLeaf(int nBoneIndex) = 0;
	virtual void DeleteSelectedLeaf() = 0;
	virtual void UnSelectAll() = 0;
    virtual void SetAllLeafRenderFlag(BOOL bFlag) = 0;
    virtual void SetAllLeafSelectableFlag(BOOL bFlag) = 0;
};

interface IEKG3DModel;
class IEKG3DSFX;
typedef void (*pFunction)(IEKG3DModelSpeedTree*, void*);


#define  ST_PART_BODY  0
#define  ST_PART_HAND  1
#define  ST_PART_LEG   2
#define  ST_PART_BELT  3
#define  ST_PART_HEAD  4
#define  ST_PART_FACE  5

interface IEKG3DBillBoardCloudCreater;
interface IEKG3DBillboardCloudCreater2;
class IEKG3DAnimationTagContainer;
interface IEKG3DSceneModelEditor : public IEKG3DConnectionPoint
{
    //virtual HRESULT SetAmbient(DWORD dwAmbient) = 0;
	//virtual HRESULT CloneSunlight(IEKG3DScene *piScene) = 0;
	//KG3DSceneModelEditorPlayerTools* m_lpPlayerTools;
	virtual void ClearModel() = 0;
	virtual HRESULT AddModel(IEKG3DModel* pModel) = 0;
    virtual HRESULT RemoveModel(IEKG3DModel* pModel) = 0;
	virtual HRESULT OpenModel(IEKG3DModel* pModel, BOOL bAddMode) = 0;
	virtual HRESULT OpenModel(LPCSTR strMeshFile,LPCSTR strMaterialFile,LPCSTR strAniamtionFile,BOOL bAddMode,IEKG3DModel** ppModel) = 0;
	virtual HRESULT AddRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable/* = TRUE*/) = 0;
	virtual HRESULT AddRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer) = 0;  ///为了兼容，覆盖KG3DScene的同名方法。chentianhong 2006-08-21
	//virtual HRESULT AddRenderEntity( KG3DSceneEntity Entity );  ///为了兼容，覆盖KG3DScene的同名方法。chentianhong 2006-08-21
	virtual HRESULT LoadModelForSockets(const TCHAR* strFileName) = 0;
	virtual HRESULT LoadModel(LPCSTR strFileName, 
		IKG3DModel **ppModel) = 0;
	virtual void SetEditLeafMode(BOOL bMode) = 0;
	virtual void OnCommand(enumCommandType eCommandType) = 0;
    virtual HRESULT SetSpeedTreeUIFunction(pFunction pCallBack, void *pDlg) = 0;
	
	//virtual HRESULT OnToggleStateScaling() = 0;
	virtual HRESULT GetSpeedTreeModifier(IEKG3DSpeedTreeModifier** pModifier) = 0;
	virtual HRESULT GetScene(IEKG3DScene** ppScene) = 0;
	virtual HRESULT GetSceneEditorBase(IEKG3DSceneEditorBase** ppSceneBase) = 0;
	virtual HRESULT GetSceneSelector(IEKG3DSceneSelectBase** ppSelector) = 0;

	//////////////////////////////////////////////////////////////////////////
	// player normal tools
	virtual HRESULT PlayerNormalTool_LoadStandModel(LPCSTR pFileName) = 0;
	virtual HRESULT PlayerNormalTool_AddMDL(LPCSTR pFileName) = 0;
	virtual HRESULT PlayerNormalTool_AddModelFromIni(LPCSTR pFileName) = 0;
	virtual HRESULT PlayerNormalTool_ProcessAllModel() = 0;
	virtual HRESULT PlayerNormalTool_SetCurrentModelPart(int nPart,int nIndex) = 0;
	virtual HRESULT PlayerNormalTool_GetModelPartVector(int nIndex,vector<string>* pvec) = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT GetSocketCount(INT* nCount) = 0;
	virtual HRESULT GetSocketName(INT nIndex, LPTSTR lpName, INT nLen) = 0;
	virtual HRESULT GetModelForSockets(INT nIndex, IEKG3DModel** ppModel) = 0;
    virtual HRESULT GetCurModel(IEKG3DModel **ppiRetModel) = 0;
	virtual HRESULT GetAllModels(vector<IEKG3DModel*>* pvecModels) = 0;

	virtual HRESULT SetFloorSlipSpeed(float fSpeed) = 0;
	virtual HRESULT SetFloorTexture(LPCTSTR szFilePath) = 0;

	virtual HRESULT ProcessEntity(BOOL bAdd) = 0;
	virtual HRESULT MoveSelectEntityToBack() = 0;
	//virtual HRESULT PushEntityToSelected(KG3DSceneEntity* pEntity) = 0;
	virtual HRESULT PushEntityToSelected(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer) = 0;
	virtual HRESULT CopyBackToSelectedEntity() = 0;

    virtual BOOL GetIsolateMode() = 0;
    virtual void RestoreIsolated() = 0;

    virtual void IsolateCurrentSelection() = 0;
	//by huangjinshou 8.9
	virtual HRESULT LoadSFXMeteor(LPCSTR pFileName,LPCSTR pSocketName,IEKG3DModel* piModel)=0;
	virtual void    ReleaseSFXMeteor() = 0;

	virtual void SetfBoneWeight(float& fWeight) = 0;
	virtual void SetbOnlyShowPitchOn(BOOL val) = 0;
	virtual BOOL GetbOnlyShowPitchOn() = 0;
    virtual IDirect3DSurface9*  GetBillBoardTexture(DWORD dwHeight, DWORD dwWidth) = 0;
	virtual IEKG3DAnimationTagContainer* GetTagContainer() = 0;

	//////////////////////////////////////////////////////////////////////////
	//公告板云生成器
	virtual IEKG3DBillBoardCloudCreater* GetBillBoardCloudCreater() = 0;//<BillboardCloudV1>
	virtual IEKG3DBillboardCloudCreater2* GetBillboardCloudCreater2() = 0;//<BillboardCloud>

	virtual HRESULT GetSelectionToolInterface(IEKG3DSceneSelectionTool** ppTool) = 0;
	virtual HRESULT ZoomToObj() = 0;

	virtual HRESULT GenerateBspForMesh(LPSTR pFileName) = 0;
	virtual HRESULT GenerateLodForMesh(LPSTR pFileName,int& nMediumFace,int& nLowFace,float fLowRation,float fMediumRation) = 0;
	virtual void EnableLOD(BOOL bEnable) = 0;

	virtual BOOL IsDrawGound() = 0;
	virtual void SetDrawGound(BOOL bDraw) = 0;
};

interface IEKG3DSceneObjectEditor
{
	virtual HRESULT OpenObject(int dwLogicID) = 0;
	virtual HRESULT OpenObject(LPSTR pFileName) = 0;
	virtual HRESULT SetCurModel(const char* pcszModelField) = 0;
	virtual HRESULT SetCurAnimt(const char* pcszAnimtField) = 0;
	virtual HRESULT PlayAnimation(const char* pcszAnimtFile) = 0;
	virtual HRESULT PlayAnimation(DWORD dwType, float fSpeed) = 0;
	virtual HRESULT PauseAnimation(BOOL bPause) = 0;
	virtual HRESULT SetAnimationSpeed(float fSpeed) = 0;
	virtual HRESULT BandObjectTemplate(ITabFile* pObjectTemplate, ITabFile* pLogicTable) = 0;
	virtual HRESULT GetRepresentObject(IEKG3DRepresentObject** pObj) = 0;
	virtual HRESULT GetCurAniFrameTime(float* fCurAniFrameTime) = 0;
	virtual HRESULT SetCurAniFrameTime(float fCurAniFrameTime) = 0;
	virtual HRESULT Bind(DWORD dwPartIndex, LPCSTR strFileName,BOOL bUnBindOrg = FALSE) = 0;
	virtual HRESULT UnBind(DWORD dwPartIndex) = 0;
	virtual HRESULT GetBaseScene(IKG3DScene** ppScene) = 0;
	virtual HRESULT SaveCurAniFrameTime() = 0;
};


enum KG3DDATAFLOWVARIABLETYPE
{
	VARIABLETYPE_INT,
	VARIABLETYPE_DCOLOR,

	VARIABLETYPE_FLOAT,
	VARIABLETYPE_FLOAT2,
	VARIABLETYPE_FLOAT3,
	VARIABLETYPE_FLOAT4,

	VARIABLETYPE_CHAR32,
	VARIABLETYPE_CHAR128,
	VARIABLETYPE_CHAR256,
	VARIABLETYPE_WCHAR32,
	VARIABLETYPE_WCHAR128,
	VARIABLETYPE_WCHAR256,

	VARIABLETYPE_END,//放到最后一个
	VARIABLETYPE_FORCEDWORD =0xFFFFFFFF,
};

interface IEKG3DSceneDataFlowEditor
{
	virtual HRESULT GetSceneEditorBase(IEKG3DSceneEditorBase** ppSceneBase) = 0;

	//////////////////////////////////////////////////////////////////////////
	//针对数据定义的操作
	virtual HRESULT NewOneDataDefine(UINT* pHandle) = 0;
	virtual HRESULT DeleteDataDefine(UINT uHandle) = 0;
	virtual HRESULT SetDataDefineName(LPCTSTR pName,UINT uHandle) = 0;
	virtual HRESULT GetDataDefineName(LPCTSTR pName,UINT uHandle) = 0;
	virtual HRESULT SetNumVariableofDataDefine(DWORD dwNumVariable,UINT uHandle) = 0;
	virtual HRESULT GetNumVariableofDataDefine(DWORD* pdwNumVariable,UINT uHandle) = 0;
	virtual HRESULT SetVariableofDataDefine(DWORD dwIndex,KG3DDATAFLOWVARIABLETYPE eType,LPCTSTR pName,UINT uHandle) = 0;
	virtual HRESULT GetVariableofDataDefine(DWORD dwIndex,KG3DDATAFLOWVARIABLETYPE* peType,LPCTSTR pName,UINT uHandle) = 0;
	virtual HRESULT GetAllDataDefine(vector<UINT>* pvecHandle) = 0;

	virtual HRESULT GetNumVariableType(DWORD* pdwNumType) = 0;
	virtual HRESULT GetVariableType(KG3DDATAFLOWVARIABLETYPE* peType,LPCTSTR pTypeName,DWORD dwIndex) = 0;
	//////////////////////////////////////////////////////////////////////////
	//针对处理单元
	virtual HRESULT GetAllDataProcessorType(vector<DWORD>*pvecProcessorType) = 0;
	virtual HRESULT GetDataProcessorDescriptionByType(LPCTSTR pDesc,DWORD dwType) = 0;

	virtual HRESULT NewOneDataProcessor(UINT* pHandle,DWORD dwType) = 0;
	virtual HRESULT DeleteDataProcessor(UINT pHandle) = 0;
	virtual HRESULT GetAllDataProcessor(vector<UINT>* pvecHandle) = 0;

	//////////////////////////////////////////////////////////////////////////
	virtual HRESULT RunDataFlow(BOOL bRun) = 0;
	virtual HRESULT ProcessRunMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0;
	virtual HRESULT ToggleEditCurrentSelectedProcessor(BOOL bEdit) = 0;

};

interface IEKG3DAnimationController;
interface IEKG3DAnimation;
interface IEKG3DMeshBone;
interface IEKG3DAnimationWarper;
interface IEKG3DModelST;
interface IEKG3DClip;
class IEKG3DAnimationTagContainer;
class IKG3DAnimationController;
class IEKG3DSFXSuede;
struct BindExtraInfo;
class IKG3DModel;
enum enuModelBindType;
class IEKG3DMesh;

interface IEKG3DModel : public IKG3DModel
{
	virtual HRESULT UpdateBoneBBoxInfo() = 0;
	virtual BOOL GetbDrawBigBox() = 0;
	virtual void SetbDrawBigBox(BOOL val) = 0;
	virtual HRESULT ComputeMeshBigBox() = 0;
	virtual HRESULT GetBoneScale(DWORD dwIndex, D3DXVECTOR3 *pScale) = 0;
	virtual HRESULT SetBoneScale(DWORD dwIndex, D3DXVECTOR3 *pScale) = 0;
    virtual HRESULT RefurbishTexts() = 0;
	virtual HRESULT RelaseMaterial() = 0;

	virtual HRESULT ModifySubsetID(DWORD* pSubsetID) = 0;
	virtual HRESULT GetNumMaterial(INT* pnCount) = 0;

    virtual HRESULT GetMeshIKG3DResourceBase(IKG3DResourceBase **ppiMesh) = 0;//add by xiayong
    virtual HRESULT GetIEKG3DMesh(IEKG3DMesh **ppiMesh) = 0;
    virtual HRESULT GetIEKG3DMaterial(IEKG3DMaterial **ppiMat) = 0;
    virtual HRESULT GetIEKG3DModelSpeedTree(IEKG3DModelSpeedTree **ppiSpeedTree) = 0;
    virtual HRESULT GetIEKG3DModelST(IEKG3DModelST **ppiModelST) = 0;

	virtual HRESULT RenderToPic(LPCSTR strOutputFile,
		DWORD dwWidth,
		DWORD dwHeight,
		DWORD dwBackColor,
		BOOL  bUsedForMeshMdlThumbnail = FALSE) = 0;
	virtual HRESULT RenderAnimationToPicSerial(LPCSTR strAnimationName, 
		LPCSTR strSerialName,
		DWORD dwWidth,
		DWORD dwHeight,
		DWORD dwBackColor) = 0;
	
//以下为 Material 相关的接口
	virtual HRESULT SaveMaterialToFile(LPCTSTR lpFileName, unsigned uOption) = 0; //m_lpCurModel->m_lpMaterial->SaveToFile
	virtual HRESULT GetMaterialName(LPTSTR lpName, INT nBufLen) = 0;	//lMat->m_scName.c_str()

	virtual HRESULT GetMaterialSubSetMaterial9(INT nSubIndex, D3DMATERIAL9** ppMaterial9) = 0;
	virtual HRESULT GetMaterialSubsetOption(INT nSubIndex, DWORD* dwOption) = 0;	//pSubMat->m_dwOption
	virtual HRESULT SetMaterialSubsetOption(INT nSubIndex, DWORD dwOption) = 0;		//pSubMat->m_dwOption

    virtual D3DCOLORVALUE* GetMaterialSubsetColorCast(int nSubIndex,int nColorCastIndex) = 0;
    virtual float*         GetMaterialSubsetSpecPower(int nSubIndex) = 0;
    virtual float*         GetMaterialSubsetEmssPower(int nSubIndex) = 0;

	//<BillboardCloudV1>
    virtual DWORD GetBillBoardCloudPlaneNum() = 0;
    virtual BOOL  HasBillBoardCloud() = 0;
    virtual void  IEReleaseBillboardCloud() = 0;
	//</BillboardCloudV1>

	virtual HRESULT GetMaterialSubsetMtlOptionData(INT nSubIndex, DWORD dwType, DWORD dwExtInfo, INT* pnData) = 0; //MtlOption->pData

	virtual HRESULT RemoveTexture(DWORD dwSubsetIndex, 
		DWORD dwIndex) = 0;

	virtual HRESULT ChangeTexture(DWORD dwSubsetIndex, 
		DWORD dwIndex, 
		LPCSTR strNewTextureFileName) = 0;

	virtual HRESULT SetMaterialSubSetAlphaOption(INT nSubIndex, DWORD dwSrcBlend, DWORD dwDestBlend, BOOL bEnableAlphaBlend) = 0;
	virtual HRESULT SetMaterialSubsetRenderTwiceAlphaOption(INT nSubsetIndex,
		BOOL bEnable,
		DWORD dwTopRef,
		DWORD dwBottomRef) = 0;

	virtual HRESULT GetMaterialSubsetRenderTwiceAlphaOption(INT nSubsetIndex,
		BOOL& bEnable,
		DWORD& dwTopRef,
		DWORD& dwBottomRef) = 0;
	virtual HRESULT CopyMaterialOption(DWORD dwType, DWORD dwExtInfo, unsigned int nSrc, unsigned int nDest) = 0;

//add by xiayong

    virtual HRESULT GetIEKG3DAnimation(IEKG3DClip **ppiClip) = 0;

    virtual HRESULT GetIEKG3DAnimationController(
        IEKG3DAnimationController **ppiAniController
    ) = 0;

    virtual HRESULT GetIEKG3DAnimationWarper(IEKG3DAnimationWarper **ppAniWarper) = 0;

    virtual HRESULT GetMatWorld(D3DXMATRIX **ppMatWorld) = 0;

    virtual HRESULT GetName(LPCTSTR *ppszName) = 0;

    //virtual HRESULT GetFullName(LPCTSTR *ppszFullName) = 0;

    virtual HRESULT SaveToFile(const TCHAR cszFileName[], UINT uOption) = 0;

    virtual HRESULT GetType(DWORD *pdwType) = 0;

    virtual HRESULT LoadMaterialFromFile(LPCTSTR strFileName,DWORD dwOption) = 0;

    virtual HRESULT Render(UINT uOption, PVOID pExtInfo) = 0;

    virtual HRESULT FrameMove() = 0;

    virtual VOID SetTranslation(D3DXVECTOR3 *pValue) = 0;
    virtual VOID SetRotation(D3DXQUATERNION *pValue) = 0;
    virtual VOID SetScaling(D3DXVECTOR3 *pValue) = 0;

    virtual VOID GetTranslation(D3DXVECTOR3 *pValue) = 0;

    virtual HRESULT GetMeshFileName(LPTSTR strFileName) = 0;
    virtual HRESULT GetMaterialFileName(LPTSTR strFileName) = 0;
    virtual HRESULT GetAnimationFileName(LPTSTR strFileName, size_t uBufferSize) = 0;

    virtual HRESULT ShowBones(BOOL bShow) = 0;

    virtual HRESULT UnbindChild(const TCHAR* strSocketName) = 0;
		
    virtual HRESULT EnableWarper(BOOL bEnable) = 0;

    virtual HRESULT LoadAnimationFromFile(LPCTSTR strFileName, DWORD Option) = 0;

    virtual HRESULT GetIEKG3DMeshBone(IEKG3DMeshBone **ppMeshBone) = 0;

    virtual HRESULT IEGetAnimationController(IKG3DAnimationController **ppiAniController) = 0;

    virtual HRESULT GetSFXSuede(IEKG3DSFXSuede **ppSFX) = 0;

    virtual HRESULT FindBone(const TCHAR* strBoneName, BindExtraInfo* pExtInfo, INT *pnRet) = 0;

    virtual HRESULT GetBoneMatrix(
        const BindExtraInfo* pInfo, D3DXMATRIX* mat, D3DXVECTOR3* pvec3Offset
    ) = 0;

    virtual HRESULT BindToBone(IKG3DModel* pModel, LPCTSTR strBoneName) = 0;

    virtual HRESULT UnBindFromOther() = 0;

    virtual HRESULT GetParentModel(IKG3DModel **ppModel) = 0;

    virtual HRESULT IEGetBindBoneName(LPCTSTR *ppszName) = 0;

    virtual HRESULT IEGetBindType(enuModelBindType *pEnu) = 0;

    virtual HRESULT GetSocketMatrix(
        const BindExtraInfo* pInfo,
        D3DXMATRIX* pmatResult,
        D3DXVECTOR3* pvec3Offset
    ) = 0;

    virtual HRESULT GetNumSocket(INT *pRetNumSocket) = 0;
    virtual HRESULT GetSocketName(int nIndex, LPCTSTR *ppRetName) = 0;

	virtual HRESULT GetNumBones(INT *pRetNumBones) = 0;

    virtual DWORD GetVertexNum() = 0;
    virtual DWORD GetFaceNum() = 0;

    virtual HRESULT GetFirstChildModel(IEKG3DModel **ppModel) = 0;
    virtual HRESULT GetNextChildModel(IEKG3DModel **ppModel) = 0;

    virtual HRESULT GetTransformationMatrix(D3DXMATRIX *pMatrix) = 0;
    virtual HRESULT SetTransformationMatrix(D3DXMATRIX *pMatrix) = 0;

    virtual ULONG STDMETHODCALLTYPE Release() = 0;

    virtual HRESULT IEReSet() = 0;
	virtual HRESULT GetBoneMatrix(
		UINT nBoneIndex, 
		D3DXMATRIX* pMat, 
		D3DXVECTOR3* pvec3Offset = NULL
		) = 0;

    virtual HRESULT ResizeSocketMatrix() = 0;

    virtual HRESULT GetAnimationTagContainer(IEKG3DAnimationTagContainer** pContainer) = 0;
	//在动画标签上用于取得绑定在插槽上的模型 by huangjinshou
	virtual int GetBindToSocketModel(const char cszSocketName[],vector<IEKG3DModel*> &vecpModel) = 0;
	virtual HRESULT GetAllChilds(vector<IEKG3DModel*> &vecpChildsModel)=0;
	virtual int GetNumModel() = 0;

	virtual HRESULT GetMaterilDetail(int nDetailType, int nSubSetID, LPSTR pszName, LPSTR pszTextureFileName, float* pfEnv,float* pfSpecular,
		float* pBodyMax,float* pBodyMin,float* pBodyScale,D3DCOLORVALUE* pBodyColor,D3DCOLORVALUE* pBodyColor1,LPSTR pChannelFile,
		float* pDetail0_ColorScale,float* pDetail0_UVScale,float* pfDetail0_Env,float* pfDetail0_Specular,D3DCOLORVALUE* pDetail0_Color,LPSTR pDetail0_File,
		float* pDetail1_ColorScale,float* pDetail1_UVScale,float* pfDetail1_Env,float* pfDetail1_Specular,D3DCOLORVALUE* pDetail1_Color,LPSTR pDetail1_File,
		D3DCOLORVALUE* pFlow_Color,float* pfFlow_ColorScale,float* pfFlow_UVScale,D3DXVECTOR2* pvSpeed,LPSTR pFlowTexture) = 0;

	virtual HRESULT SetMaterilDetail(int nDetailType, int nSubSetID, LPSTR pszName, LPSTR pszTextureFileName, float fEnv,float fSpecular,
		float fBodyMax,float fBodyMin,float fBodyScale,D3DCOLORVALUE vBodyColor,D3DCOLORVALUE vBodyColor1,LPSTR pChannelFile,
		float fDetail0_ColorScale,float fDetail0_UVScale,float fDetail0_Env,float fDetail0_Specular,D3DCOLORVALUE vDetail0_Color,LPSTR pDetail0_File,
		float fDetail1_ColorScale,float fDetail1_UVScale,float fDetail1_Env,float fDetail1_Specular,D3DCOLORVALUE vDetail1_Color,LPSTR pDetail1_File,
		D3DCOLORVALUE Flow_Color,float fFlow_ColorScale,float fFlow_UVScale,D3DXVECTOR2 vSpeed,LPSTR pFlowTexture) = 0;

	virtual HRESULT ReloadMesh() = 0;
	virtual HRESULT IESetLod(float fLod) = 0;
    virtual void SetClientAdd(BOOL bClient) = 0;
	virtual BOOL IsResourceReady(BOOL bTestTexture) = 0;
};


interface IEKG3DTexture;

interface IEKG3DSFXChannel
{
	virtual HRESULT Render(float fCurrentFrame) = 0;
	virtual HRESULT SetTexture(LPCTSTR lpstrFileName) = 0;
	virtual HRESULT GetTexture(IEKG3DTexture** pTexture) = 0;
	
	virtual HRESULT ReSet() = 0;
	virtual HRESULT UpdateEffectPos(D3DXVECTOR3 vSource, D3DXVECTOR3 vTarget) = 0;
	// sets
	virtual HRESULT SetPieceCount(int nCount) = 0;
	virtual HRESULT SetPieceWidth(float fWidte) = 0;
	virtual HRESULT SetPieceLength(float fLength) = 0;
	virtual HRESULT SetUSpeed(float fUSpeed) = 0;
	virtual HRESULT SetVUpdateFcy(float fVUpdateFcy) = 0;
	virtual HRESULT SetVPieceCount(int nCount) = 0;
    virtual HRESULT SetBlendMode(int nMode) = 0; /*0 : alpah 1 : add 2 : bright add*/
    virtual int    GetBlendMode() = 0; 

	// gets
	virtual int     GetPieceCount() = 0;
	virtual float   GetPieceWidth() = 0;
	virtual float   GetPieceLength() = 0;
	virtual float   GetUSpeed() = 0;
	virtual float   GetVUpdateFcy() = 0;
	virtual int     GetVPieceCount() = 0;
};
interface IEKG3DSerpentineMgr
{
	virtual HRESULT AddNewOne() = 0;
	virtual HRESULT DeleteOne(int nIndex) = 0;
	virtual DWORD   GetCount() = 0;
	virtual BOOL    FindSerpentineByName(const TCHAR* strName) = 0;
	virtual HRESULT SetCurSerpentine(int nIndex) = 0;
	virtual HRESULT GetAllName(vector<std::string>& vecName) = 0;
	virtual HRESULT RefreshCurSerpentine() = 0;
	virtual HRESULT RefreshHeight(BOOL bToTerrain) = 0;
	virtual float	GetCurSerWidth() = 0;
	virtual void	SetCurSerWidth(float fWidth) = 0;
	virtual void	SetCurSerDrawSelectMark(BOOL bVal) = 0;
	virtual BOOL	GetCurSerDrawSelectMark() = 0;
	virtual void	SetCurSerDrawExtendLine(BOOL bVal) = 0;
	virtual BOOL	GetCurSerDrawExtendLine() = 0;
	virtual HRESULT SetCurSerModulusK(float fK) = 0;
	virtual HRESULT GetCurSerModulusK(float& fK) = 0;
	virtual HRESULT SetCurSerName(const TCHAR* strName) = 0;
	virtual HRESULT GetCurSerName(std::string& strName) = 0;
	virtual HRESULT SetCurSerSegmentLength(int nLength) = 0;
	virtual HRESULT GetCurSerSegmentLength(int& nLength) = 0;
	virtual HRESULT MoveCurSerpentinePre()=0;
	virtual HRESULT MoveCurSerpentineNext() = 0;
};
interface IEKG3DTerrainRoadMgr : public IEKG3DSerpentineMgr
{
	virtual HRESULT SetCurrentRoadBlendLength(float fBlendLength) = 0;
	virtual HRESULT GetCurrentRoadBlendLength(float& fBlendLength) = 0;
	virtual HRESULT SetCurrentRoadTexDensity(float fTexDensity)=0;
	virtual HRESULT GetCurrentRoadTexDensity(float& fTexDensity)=0;
	virtual HRESULT SetCurrentRoadEdgeModulus(float fEdgeModulus) = 0;
	virtual HRESULT GetCurrentRoadEdgeModulus(float& fEdgeModulus) = 0;

	virtual HRESULT ChangeCurrentRoadTexture(const TCHAR* strName) = 0;
	virtual HRESULT GetCurrentRoadID(int& nID) =0;
	virtual HRESULT SetCurrentRoadID(int& nID) =0;

};

interface IEKG3DTerrainRiverMgr : public IEKG3DSerpentineMgr
{
	virtual HRESULT ComputeCurrentRiverAlpha() = 0;
	virtual float GetCurrentRiverDepth() = 0 ;
	virtual void SetCurrentRiverDepth(float fDepth)= 0;
};


#endif //_INCLUDE_IEEDITOR_H_
