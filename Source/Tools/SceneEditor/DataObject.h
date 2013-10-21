// DataObject.h: interface for the KComponentObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAOBJECT_H__E19A5558_C467_4853_B824_8150FA780A2D__INCLUDED_)
#define AFX_DATAOBJECT_H__E19A5558_C467_4853_B824_8150FA780A2D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KRegion.h"

#define OBJECT_DEFAULT 0
#define OBJECT_SKYBOX  1
#define OBJECTTYPE_GIRLNO1   100
#define OBJECTTYPE_NPC       200
#define OBJECTTYPE_PLAYER    300

#define TERRIANCELLWIDTH 50.0F
#define TERRIANCELLTEXTUREWIDTH 100.0F
//#define TERRIANREGIONCELL 32
#define TERRIANREGIONWIDTH 1600.0F

#define BARRIER_NONE          0x00000000
#define BARRIER_CAN_WALK      0x00000001
#define BARRIER_CAN_JUMP      0X00000002
#define BARRIER_BULLET_PASS   0X00000004
#define BARRIER_OBJECT_PLACE  0X00000008

#define BARRIER_OPERATOR_ADD     0
#define BARRIER_OPERATOR_REMOVE  1
#define BARRIER_OPERATOR_SET     2

#define HEIGHTDOWN_UP            10
#define HEIGHTDOWN_DOWN          11
#define HEIGHTDOWN_SET           12

#define SCRABBLE_SANP2GRID       1
#define SCRABBLE_VERTICAL        (1<<1)
#define SCRABBLE_PITCHONLY       (1<<2)
#define SCRABBLE_MODIFYTERRAIN   (1<<3)
#define SCRABBLE_MODIFYSELF      (1<<4)
#define SCRABBLE_NOONBARRIER     (1<<5)
#define SCRABBLE_RUNTIME         (1<<6)
#define SCRABBLE_WATER           (1<<7)

#define HEIGHTMAPDATATYPE_FLOATHEIGHTONLY   1
#define HEIGHTMAPDATATYPE_FLOATHEIGHTNORMAL 2

typedef class KHeightMapBase
{
public:
	AABBOX      m_BBox;//高度图的包围盒子

	int         nWidth; //长度 x 方向
	int         nHeight;//宽度 z 方向
	int         m_nDataType;//数据类型, 

	float       CellWidth;//格子的宽度
	void*       pData;    //高度数据
	void*       pDataBack;//高度数据的倍份,用来实现UNDO/REDO
	int         nDataSize;//数据的宽度
	float       fEffect;  //该高度层的混合系数
	D3DXVECTOR2 m_LeftUp; //左上角坐标

	KHeightMapBase();
	virtual ~KHeightMapBase();

	virtual HRESULT CleanUp();//清除数据,恢复默认参数设置

	HRESULT CheckBorder_Left (KHeightMapBase* DestMap);//将对方高度图的边界高度等于本高度图
	HRESULT CheckBorder_Right(KHeightMapBase* DestMap);
	HRESULT CheckBorder_Up   (KHeightMapBase* DestMap);
	HRESULT CheckBorder_Down (KHeightMapBase* DestMap);

	HRESULT ModifyHeight(D3DXVECTOR2 Center,float Size,int Shape,int Operator,float Value);//修改高度信息
	HRESULT ModifyHeight(ModelUI::FRECT Rect,int Shape,int Operator,float Value,float Force);

	float   GetVertexHeight(int nX,int nZ);
	float   GetVertexHeight(const int Index);
	D3DXVECTOR3 GetVertexPosion(int nX,int nZ);//获取数据顶点位置
	D3DXVECTOR3 GetVertexPosionBack(int nX,int nZ);//获取备份的数据顶点位置
	D3DXVECTOR3 GetVertexNormal(int nX,int nZ);

	void    SetVertexNormal(D3DXVECTOR3 Normal,int nX,int nZ);//设置数据顶点的法向量
	void    SetVertexPosition(D3DXVECTOR3 Pos,int nX,int nZ);//设置数据顶点位置
	HRESULT SetLeftUp(D3DXVECTOR2 InXZ);//设置左上角位置

	virtual BOOL GetPosition(D3DXVECTOR3* pOutPos,D3DXVECTOR2 InXZ);//查询位置
	virtual BOOL GetNormal(D3DXVECTOR3* pOutNormal,D3DXVECTOR2 InXZ);//查询法向量

	void    CreateTestMap();

	HRESULT Create(D3DXVECTOR2 Left_Up,int Width,int Height,int DataType,
		           float Cell_Width,float BaseHeight);//创建高度图

	HRESULT FromTexture(LPDIRECT3DTEXTURE9 pTexture,float HeightBase,float HeightScale);//从图象创建
	HRESULT Save2File(FILE* pFile);//保存到文件
	HRESULT LoadFromFile(FILE* pFile);//从文件读取

	void    BackUpData();//将数据保存到备分中
	void    RestoreData();//从备份数据中恢复数据
}*LPHEIGHTMAP;


typedef class KComponentObject  
{
public:
	struct _ObjectFileHeaderBase
	{
		DWORD FileMark;
		DWORD Type;
		DWORD SubType;
		DWORD BlockLength;

		_ObjectFileHeaderBase()
		{
			FileMark = 0x4F6C7A2B;
			Type     = 0;
			SubType  = 0;
			BlockLength = 0;
		}
	};

	struct _BuildingObjectHeader : _ObjectFileHeaderBase
	{
		int  nStart_X;
		int  nStart_Z;
		int  nWidth;
		int  nHeight;

		DWORD  BlockHeightMapDown;
		DWORD  BlockHeightMapUp;
		DWORD  BlockBarrier;
		DWORD  BlockMeshes;
		DWORD  BlockSetting;
		DWORD  BlockScrabble;
		DWORD  BlockTime;
		DWORD  BlockView;
		DWORD  BlockRender;
		DWORD  BlockScript;
		DWORD  BlockEnvent;
		DWORD  BlockPreperty;
		DWORD  BlockMethod;

		DWORD  Extend[20];

		_BuildingObjectHeader()
		{
			Type = OBJECT_DEFAULT;
			SubType = 0;

			nStart_X  = 0;
			nStart_Z  = 0;
			nWidth    = 0;
			nHeight   = 0;

			BlockHeightMapDown = 0;
			BlockHeightMapUp   = 0;
			BlockBarrier       = 0;
			BlockMeshes        = 0;
			BlockSetting       = 0;
			BlockScrabble      = 0;
			BlockTime          = 0;
			BlockView          = 0;
			BlockRender        = 0;
			BlockScript        = 0;
			BlockEnvent        = 0;
			BlockPreperty      = 0;
			BlockMethod        = 0;

			ZeroMemory(Extend,sizeof(DWORD)*20);
		}
	};
	//struct to keep xmesh
	typedef struct _MeshKeeper
	{
		DWORD      m_dMeshID;
		D3DXMATRIX m_Matrix;
		_MeshKeeper()
		{
			m_dMeshID = 0;
			D3DXMatrixIdentity(&m_Matrix);
		}
	}*LPMESHKEEPER;

	typedef struct _CellInfo
	{
		DWORD Barrier;
		DWORD Keep;
		_CellInfo()
		{
			Barrier     = BARRIER_CAN_WALK | BARRIER_CAN_JUMP | BARRIER_BULLET_PASS | BARRIER_OBJECT_PLACE;
			Keep = 0;
		}
	}*LPCELLINFO;

	typedef struct _GridInfo
	{
		float Height_Down;
		float Height_Up;
		_GridInfo()
		{
			Height_Down = 0;
			Height_Up   = 0;
		}
	}*LPGRIDINFO;

public:

	int        m_nStart_X;
	int        m_nStart_Z;
	int        m_nWidth;
	int        m_nHeight;
	LPCELLINFO m_lpCellInformation;
	//LPGRIDINFO M_lpGridInformation;
	LPD3DXMESH m_lpMeshGridInfo;

	LPHEIGHTMAP m_lpHeightDown;

	DWORD ID;
	DWORD m_dwType;

	vector<_MeshKeeper>m_vecMeshKeeper;

	//LPMESHKEEPER m_lpMeshKeeper;
	//DWORD m_dNumMesh;
	BOOL         m_bHeightDownNeedUpdate;

	D3DXMATRIX   m_Matrix;

	BBox         m_BBox;


	D3DXVECTOR3  m_Position;
	D3DXVECTOR3  m_Rotation;
	D3DXVECTOR3  m_Scaling;

	D3DXVECTOR3  m_PositionBack;
	D3DXVECTOR3  m_RotationBack;
	D3DXVECTOR3  m_ScalingBack;


	D3DXVECTOR3  m_PickLine;
	float        m_fBrushSize;

	DWORD        m_dwScrabbleOption;//拼接信息
	string       m_FileName;
	DWORD        m_dwGroupIndex;//物件对应的物件组中的索引
	DWORD        m_dwIndexInGroup;
protected:
	//D3DXVECTOR3  m_PositionReal;//实际的位置信息

public:
	virtual HRESULT RenderWithEffect(DWORD Option,PVOID pData);

	virtual HRESULT LoadFromFile(LPSTR pFileName);
	virtual HRESULT Save2File(LPSTR pFileName);


	virtual BOOL    SelectByPickLine(float* Distance,D3DXVECTOR3* pOrg,D3DXVECTOR3* pDir);
	HRESULT SetGridHeightDown(DWORD Operator,float Value);
	HRESULT SetCellBarrier(DWORD Operator,DWORD dwBarrier);
	HRESULT SetPickLine(D3DXVECTOR3* pOrg,D3DXVECTOR3* pDir);

	HRESULT UpdataCellInfoHeightDown();
	HRESULT BuildCellInfoHeightDown();

	virtual HRESULT RenderCellInfoBarrier(DWORD dwBarrier);
	virtual HRESULT RenderCellInfoHeightUp();
	virtual HRESULT RenderCellInfoHeightDown();
	virtual HRESULT CreateCellInformation();
	
	HRESULT DeleteMesh(DWORD Index);

	virtual	PropertyBase::KPropertyBase* Acquire();
	virtual HRESULT Copy(KComponentObject** pNewObject);

	virtual HRESULT LoadByIni(IIniFile* pIniFile,LPSTR pFileName);//根据 ini 文件来创建一个组件

	virtual ModelUI::FRECT GetXZRect();

	virtual HRESULT RenderShadow(DWORD ShadowType);
	virtual HRESULT BackupData();

	HRESULT Edit_Scaling(D3DXVECTOR3 * pScal);
	HRESULT Edit_Rotate(D3DXVECTOR3* pRot);
	HRESULT Edit_Move(D3DXVECTOR3* pPos);
	HRESULT DrawBBox();
	HRESULT ComputeBBox();
	virtual HRESULT CleanUp();
	HRESULT GetMesh(LPMODEL* ppXMesh,DWORD ID);
	HRESULT LoadMaterial(LPSTR pFileName);
	HRESULT LoadMesh(LPSTR pFileName);
	HRESULT AddMesh(DWORD MeshID);
	HRESULT GetMeshKeeper(LPMESHKEEPER * pMeshKeeper,DWORD ID);
	HRESULT Get1NewXMeshKeeper(LPMESHKEEPER * pMeshKeeper);
	HRESULT LoadFromX(LPSTR pFileName);

	virtual HRESULT Render();
	virtual HRESULT FrameMove();
	virtual void Initialize(PVOID pObjectTable);

	KComponentObject();
	virtual ~KComponentObject();

}* LPOBJECT;

#endif // !defined(AFX_DATAOBJECT_H__E19A5558_C467_4853_B824_8150FA780A2D__INCLUDED_)
