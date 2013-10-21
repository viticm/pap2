#pragma once
#include "IEEditor.h" //by dengzhihui Refactor 2006年11月16日

//#include "KG3DStructs.h"
class KScene;
class KRegion;
class KCell;
interface IEKG3DTerrain;
class CInputProxy;

#define MAXCELLSRENDEREACHTIME 512 //每次最多画多少个格子

class KG3DSceneLogicalEditorBase : public IEKG3DSceneLogicalEditorBase
{

private:
	LPDIRECT3DVERTEXBUFFER9 m_pCellVB;
	LPDIRECT3DINDEXBUFFER9  m_pCellIB;
	int m_nCurRegionX,m_nCurRegionZ;//当前编辑中心REGION
protected:
	struct CellInfo
	{
		D3DXVECTOR3 vPosition;//左下角的坐标
		float      fTop;
		KCell*     pCell;
		BOOL       bSelected;
		AABBOX     vBBox;
		KRegion*   pRegion;
		BYTE	   byX, byY;
		float      H[4];//保存顶点高度，适应地图表面斜度，方便观察
		CellInfo();
		CellInfo(bool abEditGroundCells, IEKG3DSceneSceneEditor* pEditor,int nX,int nZ,KCell* pCellInput, KRegion* argpRegion, BYTE argbyX, BYTE argbyY,float fLogicalStartX,float fLogicalStartZ);
		BOOL IsRayIntersect(float& fDistance,D3DXVECTOR3& CameraPos,D3DXVECTOR3& CameraDirct);

		//(by dengzhihui 2006年7月26日 16:22:36
		BOOL operator==(const CellInfo& comp) const
		{
			//return vPosition == comp.vPosition; //只判断左下角是否相等
			return pCell == comp.pCell;
		}
		CellInfo(const CellInfo& src);
			
		//)
	};

	//BYTE m_byVBBuf[64*64*9*32];//64*64*9*24
	//(by dengzhihui 2006年7月26日 15:51:51
	typedef list<CellInfo*> defShowListCellInfo;
	defShowListCellInfo m_listLogicalCellCurrMarked;	//处于当前编辑状态的属性集合的逻辑格子
	defShowListCellInfo m_listLogicalCellDiffMarked;	//非编辑状态的属性集合的逻辑格子
	//)

	list<CellInfo>m_listLogicalCellSelectableGround;//可以选择/编辑的逻辑格子
	list<CellInfo>m_listLogicalCellSelectableOverGround;//可以选择/编辑的逻辑格子
	defShowListCellInfo m_listLogicalCellSelected;//选中的逻辑格子
	list<KRegion*>m_listLogicalRegionEditing;//正在编辑的逻辑区域；
	KScene* m_pLogicalScene;//正在编辑的逻辑场景

	
	//By Li Kan
	//对m_listLogicalCellSelectableGround优化的优化，里面存储的是真正捕获到的地形cell，并是不是一个region上所有的地形cell
	list<CellInfo>m_listLogicalCellGroundSelected;



	BOOL m_bShowRest;
	BOOL m_bEditGroundCells;
	BOOL m_bEditOverGroundCells;
	CInputProxy* m_pProxy;
public:
	/*enum eCellModifyState
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
	};*///by dengzhihui Refactor 2006年11月16日
	DWORD m_dwLogicalCellModifyState;//编辑状态
	DWORD m_dwLogicalCellShowState;  //显示状态
	DWORD m_dwCurrentScriptID;		 //当前选中的脚本ID,是由外面传入的文件名计算得出,0表示没有选择脚本

	BOOL m_bAddCellInfo;//填加还是删除障碍上的信息
// 	enum eCellBrushShape
// 	{
// 		cbCircle = 0, cbRectangle = 1,
// 	};//by dengzhihui Refactor 2006年11月16日

private:
	HRESULT GetShowCellList(defShowListCellInfo* pListCurrShow,defShowListCellInfo* pListDiffShow, const list<CellInfo>& listInput);

	HRESULT RenderLogicalCellList(defShowListCellInfo* pList,DWORD dwColor);

	CellInfo* GetFirstLogicalCellIntersectRay(D3DXVECTOR3& CameraPos,D3DXVECTOR3& CameraDirct);

	HRESULT CreateBuffersForRenderCell();
	//(by dengzhihui 2006年7月27日 9:08:31
	//inline void ModifyLogicCellInfo(KCell* pCell);
	DWORD m_d3dclrCurrColor; //当前正在编辑的CELL应该显示的颜色
	void GetCurrentModifyMask(DWORD dwCurrentState, DWORD* pDes);
	int m_nBrushWidth, m_nDisplayWidth, m_nCellHorzHeight;
	int m_nXStart, m_nXEnd, m_nZStart, m_nZEnd; //当前正显示的逻辑区域范围
	inline void GetMapSize(int* pMapWidth, int* pMapHeight);
	eCellBrushShape m_eBrushType;
	//)

	list<D3DXVECTOR3>m_listInterPos;
protected:
	virtual HRESULT PaintSelectedLogicalCell(BOOL bAddCellInfo, BOOL bForceDel);
	virtual HRESULT RenderLogicalCell();
	virtual HRESULT SelectLogicalCells(int nXStart,int nZStart,IEKG3DSceneSceneEditor* pEditor,D3DXVECTOR3* pCameraPos,D3DXVECTOR3* pCameraDirct,D3DXVECTOR3 vGroundInter);
	virtual HRESULT UpdateEditingCells(int nXStart,int nZStart,IEKG3DSceneSceneEditor* pEditor,D3DXVECTOR3& InterPos);
public:
	virtual HRESULT Init();
	virtual HRESULT UnInit();

	HRESULT SetModifyState(eCellModifyState eState,BOOL bModify);
	BOOL GetModifyState(eCellModifyState eState);
	HRESULT SetShowState(eCellModifyState eState,BOOL bModify);
	BOOL GetShowState(eCellModifyState eState);

	HRESULT SetLogicalScene(KScene* pLogicalScene);
	KScene* GetLogicalScene();

	void SetInputProxy(CInputProxy* pProxy);

	KG3DSceneLogicalEditorBase(void);
	virtual ~KG3DSceneLogicalEditorBase(void);
	//(by dengzhihui 2006年8月3日 10:47:49
	void RecalcShowCell();
	void SetCurrentEditColor(COLORREF clr);
	void ModifyCurrentCellInfo(long lOldInfo, long lNewInfo, LPCTSTR OldScriptFileName, LPCTSTR NewScriptFileName);
	void DeleteCurrentCellInfo();//long lLogicInfo, LPCTSTR lpstrScriptFileName);
	void SetCurrentScriptFileName(LPCTSTR lpstrFileName);
	void SetBrushInfo(int nWidth, eCellBrushShape eBrushType);
	void SetDisplayParam(int nDisplayWidth, int nCellHorzHeight);
	void SetShowRest(BOOL bShowRest);
	//)
	//(by dengzhihui Refactor 2006年11月16日
	virtual HRESULT SetShowLogicalCell(BOOL bValue);
	virtual HRESULT SetShowLogicalTerrenCell(BOOL bValue);
	virtual HRESULT SetShowLogicalItemCell(BOOL bValue);
	virtual HRESULT SetShowLogicalSelectedCell(BOOL bValue);
	virtual HRESULT GetLogicScene(LPVOID* ppLogicScene);
	virtual HRESULT SetLogicScene(LPVOID pLogicScene);
protected:
	BOOL m_bShowLogicalCell;
	BOOL m_bShowLogicalTerrenCell;
	BOOL m_bShowLogicalItemCell;
	BOOL m_bShowLogicalSelectedCell;

	virtual BOOL GetShowLogicalCell(){return m_bShowLogicalCell;}
	virtual BOOL GetShowLogicalTerrenCell(){return m_bShowLogicalTerrenCell;}
	virtual BOOL GetShowLogicalItemCell(){return m_bShowLogicalItemCell;}
	virtual BOOL GetShowLogicalSelectedCell(){return m_bShowLogicalSelectedCell;}
	//)
	virtual HRESULT GetCellModifyState(DWORD* eState);
};
