// KModelSubd.h: interface for the KModelSubd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KMODELSUBD_H__8F696C42_6FD2_449E_A832_1FE05ED81CFF__INCLUDED_)
#define AFX_KMODELSUBD_H__8F696C42_6FD2_449E_A832_1FE05ED81CFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "XMesh.h"

#define REFINE_VALUE 0.4f
#define HIGH_DIFFERENT_VALUE 1.0f

class KModelSubd : public KModel  
{
public:

	typedef struct ParentFace
	{	
		int index;
		int NeighborFace[3];
		int EdgeVertex[3];
		bool bSubd1To2;///是否已被1-2细分
		bool bSubd1To4;//是否已被1-4细分
		int Vers[3];
		int ChildFace[4];
		D3DXVECTOR3 Normal;
		ParentFace()
		{
			for (int i = 0 ; i < 3 ; i++)
			{
				NeighborFace[i] = -1;
				EdgeVertex[i] = -1;
				Vers[i] = -1;
				ChildFace[i] = -1;
			}
			ChildFace[3] = -1;
		}
		~ParentFace () 
		{
			//SAFE_DELETE_ARRAY(NeighborFace);
			//SAFE_DELETE_ARRAY(EdgeVertex);
		}
	
	}*LPParentFace;

	struct Face3D
	{
		int Vers[3];

		int index;
		int neighborFace[3];// the index of neighbour face
		int parentFace; //the index of parent face
		//bool bSubdivision; //whether been visited
		int EdgeVertex[3]; // Index of edge vertex
		
		float RefineParam;///进行细化的判断参数
		float HighDif;

		bool bSubd1To2;///是否已被1-2细分
		bool bSubd1To4;//是否已被1-4细分
		int iFaceNeighborN; ///	该点关联面的个数
		
		D3DXVECTOR3 Normal;///该面的法向量

		Face3D()
		{
			index = 0 ;
			parentFace =0;
			//bSubdivision =0;
			for ( int i = 0 ; i < 3 ; i++ )
			{
				neighborFace[i] = -1 ;
				Vers[i] = -1 ;
			}
				
		}
		~Face3D()
		{
			//SAFE_DELETE_ARRAY(Vers);
			//SAFE_DELETE_ARRAY(neighborFace);
			index = 0 ;
			parentFace =0;			
		}
	public:
		
		//float GetRefineParam();
		void SetRefineParam( KModelSubd *pSubd );
		//void SetRefineParam( KModelSubd *pSubd , int nIndex);
		//void SetHighDif();

		//float SetRefineParam();///返回该面的细化参数
		//float GetArea(KModelSubd *pSubd); ///返回该面的面积
		//float GetArea(KModelSubd *pSubd, int nIndex); ///返回该面的面积
		float CalcHighDif();///计算该面三点的高度差

		//Vertex3D GetOddVertex(int index);///得到该面上的ODD 点
		int IsIncludeVertex(int vertex)///判断一个面是否包含一个点
		{	
			int iResult = false;
			for (int i = 0 ; i < 3 ; i++)
				if ( Vers[i] == vertex)
				{
					iResult = true;
					goto Exit0;
				}
		Exit0:
				return iResult;
		}
		int IsIncludeVertex(KModelSubd *pSubd, int nIndex, int nVertex);///判断一个面是否包含一个点
	} *LPFace3D;

	typedef struct Vertex3D
	{
		D3DXVECTOR3 position ;///该点的三维坐标
		float tu;
		float tv;
		D3DXVECTOR3 normal;
		int index;
		int iFaceN;///关联面的个数
		int degree;///关联点的个数
		vector <int> pFaceN;
		//int pFaceN[20];///记录所关联面的index值
		vector <int> pVertexN;
		//int pVertexN[20];///记录所关联点的index值
		float fHeight;
		D3DXVECTOR3 Height; ///生长的高度有三个分量

		Vertex3D()
		{			
			tu = -1;
			tv = -1;			
			index = -1;
/*
			for (int i = 0 ; i < 20 ; i++ )
			{
				pFaceN[i] = -1 ;
				pVertexN[i] = -1 ;
			}
			*/
			//pFaceN = 0;
			//pVertexN = 0;
		}

		~Vertex3D()
		{
			//SAFE_DELETE_ARRAY(pFaceN);
			//SAFE_DELETE_ARRAY(pVertexN);
		}	
		int SetDegree();		
	} *LPVertex3D;

public:
	
	LPD3DXMESH        m_pMeshSubdSys; //mesh been subdivision

	vector <Face3D> m_VecFace3D; ///存储网格中的面信息
	vector <Vertex3D>  m_VecVers3D;///存储网格中的点信息
	int m_iNumVers;///网格中节点的个数
	int m_iNumFaces;///网格中面的个数

	float m_fCameroLength;///距离照相机的距离

	int m_nSubdivisionN;///网格细分的次数

	LPDIRECT3DTEXTURE9 m_lpTexture;///导入的贴图

	float m_fMaxArea;///网格中三角面的最大面积
	float m_fMinArea;///网格中三角面的最小面积

	int m_nHeight;///被拉高的高度
	float m_fVertexHeightDif;///判断值，如果面中的顶点的通道高度值得差有超过改值则要进行细化
public:

	KModelSubd();
	virtual ~KModelSubd();
	///重载KModel
	HRESULT FrameMove();
	HRESULT Render();

	HRESULT LoadMesh(LPSTR pFileName);//载入要细化的mesh
	HRESULT Acquire();//查询属性

	int LoadTexture(LPDIRECT3DTEXTURE9 pTexture); ///由外界导入texture
	int SaveSubdMesh();///将数据存储到m_pMeshSubdSys
	int InitializeSubd();
	int Refinement();	

	//int LineInterpolate();/// 直接的线性插值
	int LineInterpolateMid(Vertex3D &vertex,int faceIndex, int edgeIndex);///在面faceIndex的第edgeIndex条边中点位置插入新点vertex
	int LineInterpolate(int faceIndex);/// 对该面进行直接的线性插值
	
	int LoopSubdivision( int faceIndex );///对该面进行细分处理
	int ButterflySubdivsion( int faceIndex);
	void NPath();

	////////计算几何位置
	int CalcOddVertex(Vertex3D & oddVertex,int faceIndex, int edgeIndex);// 用loop方法计算faceindex上的第edgeIndex上的odd index
	
	void CalcEvenVertex(int faceIndex, int vertexIndex);// 找faceindex上的第vertexIndex上的Even index
	int CalcVertexNeighborValue();///计算每个点的度数和临接点
	//////////////////////////////////////////////////////////////////////////

	///计算拓扑位置
	//void BuildAdjacency(vector <Face3D> & vecFace);///建立topology	
	void BuildAdjacency(vector <ParentFace> & vecFace);///建立topology	
	int ModifyNeighborFace(int faceIndex, int edgeIndex);///调整面的邻接面上的边点
		
	void CreateFourNewFaces( const ParentFace parentFace);///将面parentFaceIndex分裂成四个子面
	void CreateTwoNewFaces(const ParentFace parentFace);///将面parentFaceIndex分裂成由两个子面
	void CloneFromParentFace(const ParentFace parentFace);///由面parentFace克隆到新的子面中去
	
	///关于面相关属性计算的全局函数
	void MidVector(D3DXVECTOR3 & midVec, const D3DXVECTOR3 vector1, const D3DXVECTOR3 vector2); ///计算两个向量的中点向量
	int CalcFaceNormal(D3DXVECTOR3 &faceNormal,int faceIndex);///计算面faceIndex的法向量
	int CalcFaceNormal2( D3DXVECTOR3 &faceNormal,const D3DXVECTOR3 parentNormal );///计算面faceIndex的法向量
		
	int CalcVertexNormal(D3DXVECTOR3 &vertexNormal,int vertexIndex);///计算点vertexIndex的法向量

	///计算细化参数
	float GetFaceArea(int nIndex);
	//float GetFaceArea(const Face3D myFace, int nIndex);
	float SetRefineParam(int nIndex);
	//float SetRefineParam(const Face3D myFace, int nIndex);
	float SetHighDif(const int nFaceIndex);

	//////////////////////////////////////////////////////////////////////////
	
	int CalcTextureHighValue( LPDIRECT3DTEXTURE9 pTexture);
	int ChangeMeshHigh();
	int ChangeMeshHigh2();
	
	int SetMaxMinArea();
	int IsSameHeightVertexofFace(int  faceIndex);
};

#endif // !defined(AFX_KMODELSUBD_H__8F696C42_6FD2_449E_A832_1FE05ED81CFF__INCLUDED_)
