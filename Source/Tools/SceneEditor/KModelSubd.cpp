// KModelSubd.cpp: implementation of the KModelSubd class.
//
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include "sceneeditor.h"
#include "KModelSubd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction

//////////////////////////////////////////////////////////////////////

/**
* @brief 构造函数，对实例变量进行赋值
**/
KModelSubd::KModelSubd()
{
	m_iNumVers = 0;
	m_iNumFaces = 0;
	m_nHeight = 50;
	m_fVertexHeightDif = 0.1f;
}

KModelSubd::~KModelSubd()
{

}

HRESULT KModelSubd::FrameMove()
{
	return S_OK;
}

/**
* @param nFaceIndex 面的索引值
* @brief 设置第nFaceIndex面的高度差
* @return 面各顶点的高度差（以y轴为高度）
*/
float KModelSubd::SetHighDif(const int nFaceIndex)
{
	float fHigh, fMaxHeight;
	//fMaxHeight= ( nFaceIndex * 13 ) % 10 * 0.01;
	
	int nVertex0, nVertex1, nVertex2; 
	nVertex0 = m_VecFace3D[nFaceIndex].Vers[0];
	nVertex1 = m_VecFace3D[nFaceIndex].Vers[1];
	nVertex2 = m_VecFace3D[nFaceIndex].Vers[2];
	fMaxHeight = abs(m_VecVers3D[nVertex0].position.y - m_VecVers3D[nVertex1].position.y);
	fHigh = abs(m_VecVers3D[nVertex0].position.y - m_VecVers3D[nVertex2].position.y);
	if ( fHigh > fMaxHeight)
		fMaxHeight = fHigh;
	fHigh = abs(m_VecVers3D[nVertex1].position.y - m_VecVers3D[nVertex2].position.y);
	if ( fHigh > fMaxHeight)
		fMaxHeight = fHigh;
	
	return fMaxHeight;
}
/**
* @param nIndex 面的索引值
* @brief设置第nIndex面Refine参数 
* @return  面的细化参数值
*/
float KModelSubd::SetRefineParam(int nIndex)
{
	//RefineParam = 1 * ;
	float fParam;
	float fArea;
	fArea = GetFaceArea(nIndex);
	//fParam = ( int ( fArea * 100 ) )% 10 * 0.1f;	
	//fParam = (nIndex *7  )% 10 * 0.1f + ((int) ( fArea * 13)) % 10 * 0.1;
	fParam = fArea * (m_nSubdivisionN +1);
	return fParam;
}

/**
* @param *pSubd 目前网格的指针
* @param nIndex 要处理的面的索引值
* @param nVertex 点的索引值
* @brief 判断第nIndex面中是否包含索引值为nVertex的点
* @return false 表示失败，true 表示成功
*/
int KModelSubd::Face3D::IsIncludeVertex(KModelSubd *pSubd, int nIndex, int nVertex)
{
	int nResult = false;
	for (int i = 0 ; i < 3 ; i++)
		if ( pSubd->m_VecFace3D[nIndex].Vers[i] == nVertex)
		{
			nResult = true;
			goto Exit0;
		}
Exit0:
		return nResult;
}

/**
* @param nIndex 面的索引值
* @brief 获取第nIndex面的面积
* @return 面的面积
*/
float KModelSubd::GetFaceArea(int nIndex)
{
	///求三角形各边的边长
	int A,B,C;
	D3DXVECTOR3 vector_A,vector_B,vector_C;
	D3DXVECTOR3 t_vector;
	float fA,fB,fC,fS;
	float fArea;

	A = m_VecFace3D[nIndex].Vers[0];
	B = m_VecFace3D[nIndex].Vers[1];
	C = m_VecFace3D[nIndex].Vers[2];
	vector_A = m_VecVers3D[A].position;
	vector_B = m_VecVers3D[B].position;
	vector_C = m_VecVers3D[C].position;
	t_vector = vector_A - vector_B ;	
	fA = D3DXVec3Length(&t_vector);
	t_vector = vector_B - vector_C ;	
	fB = D3DXVec3Length(&t_vector);
	t_vector = vector_C - vector_A ;	
	fC = D3DXVec3Length(&t_vector);
	fS = ( fA + fB + fC ) / 2 ;

	fArea = sqrt(fS * ( fS - fA ) * ( fS - fB ) * ( fS - fC ));
	return fArea;
}
/*
* @param void
* @brief 设置当前网格每个点的度数(相邻点的个数),并记下这些点的index值
*        计算每个点的pFaceN,pVertexN,degree,iFaceN的值
* @return false 表示失败，true 表示成功
*/

int KModelSubd::CalcVertexNeighborValue()
{
	int nResult = false;
	int nVertexIndex;
	int nOtherVertex;
	int nDegree = 0;
	int nFace ;
	int nVertex;
	
	for ( nFace = 0 ; nFace < m_iNumFaces ; nFace++ )
	{
		for ( nVertex = 0 ; nVertex < 3 ; nVertex++ )
		{			
			///对当前的点判断同一个三角形中另外两个点
			nVertexIndex = m_VecFace3D[nFace].Vers[nVertex];
			nOtherVertex = m_VecFace3D[nFace].Vers[(nVertex+1)%3];
			nDegree = m_VecVers3D[nVertexIndex].pVertexN.size();///目前已经存放的关联点的个数
			for (int i = 0; i < nDegree ; i++ )
			{///遍历查找是否有nOtherVertex点,如果已经有了,则立刻退出循环
				if ( m_VecVers3D[nVertexIndex].pVertexN[i] == nOtherVertex )
					break;
			}
			if ( i >= nDegree )
			{///如果遍历循环都没有发现等于nOtherVertex的点,则放入pVertexN中
				m_VecVers3D[nVertexIndex].pVertexN.push_back(nOtherVertex);
			}
			nOtherVertex = m_VecFace3D[nFace].Vers[(nVertex+2)%3];			
			for (int i = 0; i < nDegree ; i++ )
			{///遍历查找是否有nOtherVertex点,如果已经有了,则立刻退出循环
				if ( m_VecVers3D[nVertexIndex].pVertexN[i] == nOtherVertex )
					break;
			}
			if ( i >= nDegree )
			{///如果遍历循环都没有发现等于nOtherVertex的点,则放入pVertexN中
				m_VecVers3D[nVertexIndex].pVertexN.push_back(nOtherVertex);
			}

			m_VecVers3D[nVertexIndex].pFaceN.push_back(nFace);
		}
	}
	///给点的iFaceN和degree两个属性赋值
	for ( nVertex = 0 ; nVertex < m_iNumVers ; nVertex++ )
	{
		m_VecVers3D[nVertex].iFaceN = m_VecVers3D[nVertex].pFaceN.size();
		m_VecVers3D[nVertex].degree = m_VecVers3D[nVertex].pVertexN.size();
	}

	nResult = true;
	return nResult;
}
/**
* @param pFileName 导入的文件名
* @brief  load mesh,并做初始化的工作
* @return S_FALSE 表示失败，S_OK 表示成功
*/
HRESULT KModelSubd::LoadMesh(LPSTR pFileName)
{
	HRESULT hrResult;
	hrResult = S_FALSE;
	KModel::LoadMesh(pFileName);
	m_pMeshSubdSys = KModel::m_pMeshSys;

	///得到网格的vertex和face的Num值
	m_iNumVers = m_pMeshSubdSys->GetNumVertices();
	m_iNumFaces  = m_pMeshSubdSys->GetNumFaces();
		
	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	WORD* pIndex = NULL;
	DWORD * pAttrib = NULL;
	if (FAILED(m_pMeshSubdSys->LockVertexBuffer(0,(void**)&pVers)))
		goto Exit0;
	if (FAILED(m_pMeshSubdSys->LockIndexBuffer (0,(void**)&pIndex)))
		goto Exit0;
	if (FAILED(m_pMeshSubdSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
		goto Exit0;	

	///用于计算临接点
	LPD3DXBUFFER m_pAdjacencyBuffer = NULL;   // Contains the adjaceny info loaded with the mesh
	if ( FAILED( D3DXCreateBuffer(m_pMeshSys->GetNumFaces()*3*sizeof(DWORD),
		&m_pAdjacencyBuffer)))
		goto Exit0;
	if ( FAILED(  m_pMeshSys->GenerateAdjacency(0,
		(DWORD*)m_pAdjacencyBuffer->GetBufferPointer())))
		goto Exit0;

	DWORD * pAdjacen = (DWORD*)m_pAdjacencyBuffer->GetBufferPointer();
	

	///访问每个face，导入信息到KModelSubd类的数组m_VecFace3D中
	for(int i=0;i<m_iNumFaces;i++)
	{
		
		WORD A,B,C;
		DWORD SubsetID;
		A = pIndex[i*3  ];
		B = pIndex[i*3+1];
		C = pIndex[i*3+2];
		SubsetID = pAttrib[i];

		D3DXVECTOR3 Pos_A = pVers[A].p;
		D3DXVECTOR3 Pos_B = pVers[B].p;
		D3DXVECTOR3 Pos_C = pVers[C].p;
		///存储面
		Face3D myFace ;
		myFace.index = i;
		
		myFace.iFaceNeighborN = 0;
		for (int j = 0 ; j < 3 ; j++ )
		{
			myFace.Vers[j] = pIndex[i*3+j];
			myFace.neighborFace[j] = pAdjacen[3*i + j];
			if (myFace.neighborFace[j] != -1) 
				myFace.iFaceNeighborN = myFace.iFaceNeighborN + 1;
			myFace.EdgeVertex[j] = -1;
		}
		
		m_VecFace3D.push_back(myFace);
	}
	///访问每个点，导入信息到KModelSubd类的数组m_VecVers3D中
	for (int nVer = 0 ; nVer < m_iNumVers ; nVer++ )
	{
		Vertex3D myVertex;
		myVertex.index = nVer;
		myVertex.position = pVers[nVer].p;
		myVertex.normal = pVers[nVer].Normal;
		myVertex.tu = pVers[nVer].tu1;
		myVertex.tv = pVers[nVer].tv1;	
		myVertex.degree = 0;
		myVertex.iFaceN = 0;
		myVertex.pFaceN.clear();
		myVertex.pVertexN.clear();
		m_VecVers3D.push_back(myVertex);
	}

	m_nSubdivisionN = 0;
	

	if (FAILED(m_pMeshSubdSys->UnlockVertexBuffer()))
		goto Exit0;
	if (FAILED(m_pMeshSubdSys->UnlockIndexBuffer()))
		goto Exit0;
	if (FAILED(m_pMeshSubdSys->UnlockAttributeBuffer()))
		goto Exit0;


	hrResult = S_OK;
Exit0:


	return hrResult;
}

/**
* @param & midVec ，输出中点的向量
* @param vector1 , 顶点1
* @param vector2 , 顶点2
* @brief 求得向量vector1,vector2连线的中点，返回该中点值得向量
* @return void
*/
void KModelSubd::MidVector(D3DXVECTOR3 & midVec, const D3DXVECTOR3 vector1, const D3DXVECTOR3 vector2)
{
	midVec.x = ( vector1.x + vector2.x ) /2 ;
	midVec.y = ( vector1.y + vector2.y ) /2 ;
	midVec.z = ( vector1.z + vector2.z ) /2 ;
}

/**
* @param vector <parent.>
* @brief 建立面的拓扑结构
* @return void
*/
void KModelSubd::BuildAdjacency(vector <ParentFace> & vecFace)
{
	///建立各个面的临接面
	for (int iFace = 0 ; iFace < m_iNumFaces ; iFace++)
	{
		if (vecFace[iFace].bSubd1To4)///如果该面有四个子面
		{
			int iNeighborFace0 = vecFace[iFace].NeighborFace[0];
			int iNeighborFace1 = vecFace[iFace].NeighborFace[1];
			int iNeighborFace2 = vecFace[iFace].NeighborFace[2];
			
			//////////////////////////////////////////////////////////////////////////	
			///set the first faces
			int iChildFace = vecFace[iFace].ChildFace[0];
			int iPoint = vecFace[iFace].Vers[0];
			
			if ( iNeighborFace0 != -1 )
			{///设置第一个neighbor face
					for ( int j = 0 ; j < 3 ; j++ )
					{
						int iNeighborChild = vecFace[iNeighborFace0].ChildFace[j];
						if (m_VecFace3D[iNeighborChild].IsIncludeVertex(this,iNeighborChild,iPoint))
						{
							m_VecFace3D[iChildFace].neighborFace[0] = iNeighborChild;
							break;
						}
					}		
			}
			m_VecFace3D[iChildFace].neighborFace[1] = vecFace[iFace].ChildFace[3];/// the second neighbor face
			if ( iNeighborFace2 != -1 )
			{///the third neighbor face
				for ( int j = 0 ; j < 3 ; j++ )
				{
					int iNeighborChild = vecFace[iNeighborFace2].ChildFace[j];
					if (m_VecFace3D[iNeighborChild].IsIncludeVertex(this,iNeighborChild,iPoint))
					{
						m_VecFace3D[iChildFace].neighborFace[2] = iNeighborChild;
						break;
					}
				}
			}
			for ( int k = 0 ; k < 3 ; k++ )
			{
				if (m_VecFace3D[iChildFace].neighborFace[k] != -1)
					m_VecFace3D[iChildFace].iFaceNeighborN = m_VecFace3D[iChildFace].iFaceNeighborN + 1 ;
			}
			//////////////////////////////////////////////////////////////////////////
			///set the second faces 
			iChildFace = vecFace[iFace].ChildFace[1];
			iPoint = vecFace[iFace].Vers[1];
			if ( iNeighborFace0 != -1 )
			{///设置第一个neighbor face
				for ( int j = 0 ; j < 3 ; j++ )
				{
					int iNeighborChild = vecFace[iNeighborFace0].ChildFace[j];
					if (m_VecFace3D[iNeighborChild].IsIncludeVertex(this,iNeighborChild,iPoint))
					{
						m_VecFace3D[iChildFace].neighborFace[0] = iNeighborChild;
						break;
					}
				}		
			}
			if ( iNeighborFace1 != -1 )
			{///the second neighbor face
				for ( int j = 0 ; j < 3 ; j++ )
				{
					int iNeighborChild = vecFace[iNeighborFace1].ChildFace[j];
					if (m_VecFace3D[iNeighborChild].IsIncludeVertex(this,iNeighborChild,iPoint))
					{
						m_VecFace3D[iChildFace].neighborFace[1] = iNeighborChild;
						break;
					}
				}
			}
			m_VecFace3D[iChildFace].neighborFace[2] = vecFace[iFace].ChildFace[3];/// the third neighbor face
			for ( int k = 0 ; k < 3 ; k++ )
			{
				if (m_VecFace3D[iChildFace].neighborFace[k] != -1)
					m_VecFace3D[iChildFace].iFaceNeighborN = m_VecFace3D[iChildFace].iFaceNeighborN + 1 ;
			}
			//////////////////////////////////////////////////////////////////////////
			///set the third face
			iChildFace = vecFace[iFace].ChildFace[2];
			iPoint = vecFace[iFace].Vers[2];
			m_VecFace3D[iChildFace].neighborFace[0] = vecFace[iFace].ChildFace[3];/// the first neighbor face
			if ( iNeighborFace1 != -1 )
			{///设置第一个second face
				for ( int j = 0 ; j < 3 ; j++ )
				{
					int iNeighborChild = vecFace[iNeighborFace1].ChildFace[j];
					if (m_VecFace3D[iNeighborChild].IsIncludeVertex(this,iNeighborChild,iPoint))
					{
						m_VecFace3D[iChildFace].neighborFace[1] = iNeighborChild;
						break;
					}
				}		
			}
			if ( iNeighborFace2 != -1 )
			{///the second neighbor face
				for ( int j = 0 ; j < 3 ; j++ )
				{
					int iNeighborChild = vecFace[iNeighborFace2].ChildFace[j];
					if (m_VecFace3D[iNeighborChild].IsIncludeVertex(this,iNeighborChild,iPoint))
					{
						m_VecFace3D[iChildFace].neighborFace[2] = iNeighborChild;
						break;
					}
				}
			}
			for ( int k = 0 ; k < 3 ; k++ )
			{
				if (m_VecFace3D[iChildFace].neighborFace[k] != -1)
					m_VecFace3D[iChildFace].iFaceNeighborN = m_VecFace3D[iChildFace].iFaceNeighborN + 1 ;
			}
			//////////////////////////////////////////////////////////////////////////
			///set the fourth face
			iChildFace = vecFace[iFace].ChildFace[3];
			m_VecFace3D[iChildFace].neighborFace[0] = vecFace[iFace].ChildFace[1];
			m_VecFace3D[iChildFace].neighborFace[1] = vecFace[iFace].ChildFace[2];
			m_VecFace3D[iChildFace].neighborFace[2] = vecFace[iFace].ChildFace[0];
			m_VecFace3D[iChildFace].iFaceNeighborN = 3;	
		}///(vecFace[iFace].bSubd1To4)///如果该面有四个子面
		else if (vecFace[iFace].bSubd1To2)
		{
			int iChildFace = vecFace[iFace].ChildFace[0];
			int nPoint1,nPoint2;///与临界面关联的两个顶点
			int iParentNeighborFace;///邻接面
			int nNeighborChildFace;///临界面的子面
			for (int j = 0 ; j < 3 ; j++ )
			{
				if ( vecFace[iFace].EdgeVertex[j] != -1 )
				{
					break;
				}
			}
			//////////////////////////////////////////////////////////////////////////			
			///set the first face			
			///the first neighbor face(如果没有临接面就是-1)
			iParentNeighborFace = vecFace[iFace].NeighborFace[(j+2)%3];
			nPoint1 = vecFace[iFace].Vers[j];
			nPoint2 = vecFace[iFace].Vers[(j+2)%3];
			
			if (vecFace[iParentNeighborFace].bSubd1To2 == true)
			{///如果改邻接面是1-2细分了的话,则其neighbor face是是它的childFace
				for (int n = 0 ; n < 2 ; n++ )
				{
					nNeighborChildFace = vecFace[iParentNeighborFace].ChildFace[n];
					if ( (m_VecFace3D[nNeighborChildFace].IsIncludeVertex(this,nNeighborChildFace,nPoint1))
						&& ( m_VecFace3D[nNeighborChildFace].IsIncludeVertex(this,nNeighborChildFace,nPoint2) ) )
						break;
				}
				m_VecFace3D[iChildFace].neighborFace[0] = nNeighborChildFace;
			}
			else
			{///直接赋值,如果为-1也直接赋值
				if (iParentNeighborFace == -1)
					m_VecFace3D[iChildFace].neighborFace[0] = -1 ;
				else
					m_VecFace3D[iChildFace].neighborFace[0] = vecFace[iParentNeighborFace].ChildFace[0];
			}			
			///the second neighbor face
			int iPoint ;
			iPoint = vecFace[iFace].Vers[j];			
			iParentNeighborFace = vecFace[iFace].NeighborFace[j];
			if ( iParentNeighborFace != -1 )
			{
				for ( int k = 0 ; k < 3 ; k++)
				{
					int iNeighborChild = vecFace[iParentNeighborFace].ChildFace[k];
					if (m_VecFace3D[iNeighborChild].IsIncludeVertex(this,iNeighborChild,iPoint))
					{
						m_VecFace3D[iChildFace].neighborFace[1] = iNeighborChild;
						break;
					}
				}
			}
			///the third neighbor face
			m_VecFace3D[iChildFace].neighborFace[2] = vecFace[iFace].ChildFace[1];
			for ( int k = 0 ; k < 3 ; k++ )
			{
				if (m_VecFace3D[iChildFace].neighborFace[k] != -1)
					m_VecFace3D[iChildFace].iFaceNeighborN = m_VecFace3D[iChildFace].iFaceNeighborN + 1 ;
			}		
			//////////////////////////////////////////////////////////////////////////
			///set the second face
			iChildFace = vecFace[iFace].ChildFace[1];
			nPoint1 = vecFace[iFace].Vers[(j+1)%3];
			nPoint2 = vecFace[iFace].Vers[(j+2)%3];

			///the first neighbor face
			m_VecFace3D[iChildFace].neighborFace[0] = vecFace[iFace].ChildFace[0];
			///the second neighbor face
			iPoint = vecFace[iFace].Vers[(j+1)%3];
			iParentNeighborFace = vecFace[iFace].NeighborFace[j];
			if ( iParentNeighborFace != -1 )
			{
				for ( int k = 0 ; k < 3 ; k++)
				{
					int iNeighborChild = vecFace[iParentNeighborFace].ChildFace[k];
					if (m_VecFace3D[iNeighborChild].IsIncludeVertex(this,iNeighborChild,iPoint))
					{
						m_VecFace3D[iChildFace].neighborFace[1] = iNeighborChild;
						break;
					}
				}
			}
			///the third neighbor face			
			iParentNeighborFace = vecFace[iFace].NeighborFace[(j+1)%3];
			if (vecFace[iParentNeighborFace].bSubd1To2 == true)
			{
				for (int n = 0 ; n < 2 ; n++ )
				{
					nNeighborChildFace = vecFace[iParentNeighborFace].ChildFace[n];
					if ( (m_VecFace3D[nNeighborChildFace].IsIncludeVertex(this,nNeighborChildFace,nPoint1))
						&& ( m_VecFace3D[nNeighborChildFace].IsIncludeVertex(this,nNeighborChildFace,nPoint2) ) )
						break;

				}
				m_VecFace3D[iChildFace].neighborFace[2] = nNeighborChildFace;
			}
			else
			{
				if (iParentNeighborFace == -1)
					m_VecFace3D[iChildFace].neighborFace[2] = -1 ;
				else
					m_VecFace3D[iChildFace].neighborFace[2] = vecFace[iParentNeighborFace].ChildFace[0];
			}			
			///计数////
			for ( int k = 0 ; k < 3 ; k++ )
			{
				if (m_VecFace3D[iChildFace].neighborFace[k] != -1)
					m_VecFace3D[iChildFace].iFaceNeighborN = m_VecFace3D[iChildFace].iFaceNeighborN + 1 ;
			}
		}
		else
		{///该面没有被细化过
			int iChildFace = vecFace[iFace].ChildFace[0];
			int nNeighborChildFace;
			int nPoint1,nPoint2;
			int iNeighborFace;
			for ( int k = 0 ; k < 3 ; k++ )
			{
				iNeighborFace = vecFace[iFace].NeighborFace[k];
				nPoint1 = vecFace[iFace].Vers[k];
				nPoint2 = vecFace[iFace].Vers[(k+1)%3];
				if (vecFace[iNeighborFace].bSubd1To2 == true)
				{
					for (int n = 0 ; n < 2 ; n++ )
					{
						nNeighborChildFace = vecFace[iNeighborFace].ChildFace[n];
						if ( (m_VecFace3D[nNeighborChildFace].IsIncludeVertex(this,nNeighborChildFace,nPoint1))
							&& ( m_VecFace3D[nNeighborChildFace].IsIncludeVertex(this,nNeighborChildFace,nPoint2) ) )
							break;
					}
					m_VecFace3D[iChildFace].neighborFace[k] = nNeighborChildFace;
				}
				else
				{
					if (iNeighborFace == -1 )
						m_VecFace3D[iChildFace].neighborFace[k] = -1 ;
					else
						m_VecFace3D[iChildFace].neighborFace[k] = vecFace[iNeighborFace].ChildFace[0];	
				}				
				if ( iNeighborFace != -1 )
					m_VecFace3D[iChildFace].iFaceNeighborN = m_VecFace3D[iChildFace].iFaceNeighborN +1;
			}

		}

	}///for (int iFace = 0 ; iFace < m_iNumFaces ; iFace++)

	///设置点的iFace和pFaceN
}

/**
* @param &faceNormal ，输出，该面的法向量
* @param parentNormal 要计算面的父面
* @brief 在网格初始化后计算面的法向量，
* @return false 表示失败，true表示成功
*/
int KModelSubd::CalcFaceNormal2(D3DXVECTOR3 & faceNormal,const D3DXVECTOR3 parentNormal)
{
	faceNormal.x = parentNormal.x;
	faceNormal.y = parentNormal.y;
	faceNormal.z = parentNormal.z;
	return true;
}
/*
* @param & faceNormal ,输出，该面的法向量
* @param parentNormal 要计算面的父面
* @brief 在网格初始化时计算面的法向量
* @return false 表示失败，true表示成功
*/
int KModelSubd::CalcFaceNormal(D3DXVECTOR3 & faceNormal,int faceIndex)
{
	int iResult =false;
	int iVertex1,iVertex2,iVertex0;
	D3DXVECTOR3 p0,p1,p2;
	iVertex0 = m_VecFace3D[faceIndex].Vers[0];
	iVertex1 = m_VecFace3D[faceIndex].Vers[1];
	iVertex2 = m_VecFace3D[faceIndex].Vers[2];
	p0 = m_VecVers3D[iVertex0].position;
	p1 = m_VecVers3D[iVertex1].position;
	p2 = m_VecVers3D[iVertex2].position;
	
	D3DXPLANE placeFace;
	D3DXPlaneFromPoints(&placeFace,&p0,&p2,&p1);
	faceNormal.x = placeFace.a;
	faceNormal.y = placeFace.b;
	faceNormal.z = placeFace.c;
	iResult =true;

	return iResult;
}

/**
* @param & vertexNormal ，输出，点的法向量
* @param vertexIndex, 点的索引值
* @return false 表示失败，true表示成功
**/
int KModelSubd::CalcVertexNormal(D3DXVECTOR3 & vertexNormal, int vertexIndex)
{
	int iResult = false;
	float x,y,z;
	x = 0; y = 0; z = 0 ;
	int iFaceN ;
	iFaceN = m_VecVers3D[vertexIndex].iFaceN ;
	for (int i = 0; i < iFaceN ; i++)
	{
		int iNeighborFace = m_VecVers3D[vertexIndex].pFaceN[i];
		x += m_VecFace3D[iNeighborFace].Normal.x;
		y += m_VecFace3D[iNeighborFace].Normal.y;
		z += m_VecFace3D[iNeighborFace].Normal.z;
	}
	///取周围各临接面的平均值
	x /= iFaceN;
	y /= iFaceN;
	z /= iFaceN;
	vertexNormal.x = x ;
	vertexNormal.y = y ;
	vertexNormal.z = z ;

	return iResult;
}

/**
* @param faceIndex 面的索引值
* @brief 对面进行线性插值
* @return false 表示失败，true表示成功
**/
int KModelSubd::LineInterpolate(int faceIndex)
{
	int iResult = false;

	Vertex3D mid;

	for (int iEdge = 0 ; iEdge < 3 ; iEdge++)
	{
		if (m_VecFace3D[faceIndex].EdgeVertex[iEdge] == -1)///如果该面上得边点还没有生成
		{
			//LineInterpolateMid(*midVertex,faceIndex,iEdge);
			LineInterpolateMid(mid,faceIndex,iEdge);
			//m_VecVers3D.push_back(*midVertex);
			mid.index = m_VecVers3D.size();
			m_VecVers3D.push_back(mid);
			m_VecFace3D[faceIndex].EdgeVertex[iEdge] = mid.index;
		}
	}
	m_VecFace3D[faceIndex].bSubd1To4 = true;
	
	///调整周围的面
	for (int iEdge = 0 ; iEdge < 3 ; iEdge++)
		ModifyNeighborFace( faceIndex, iEdge );


	iResult = true;
	return iResult;
}
/*
* @param parentFace 新生成面的父面的索引值
* @brief 将面parentFaceIndex分裂成四个子面,并插入到m_VecFace3D的末尾
* @return false 表示失败，true表示成功
*/
void KModelSubd::CreateFourNewFaces( const ParentFace parentFace)
{
	Face3D newFace;
	int nIndex;
	//////////////////////////////////////////////////////////////////////////	
	nIndex = m_VecFace3D.size();
	newFace.index = nIndex;
	newFace.bSubd1To4 = false;
	newFace.bSubd1To2 = false;
	for ( int j = 0 ;j < 3 ; j++)
	{
		newFace.EdgeVertex[j] = -1;
		newFace.neighborFace[j] = -1;
	}
	newFace.parentFace = parentFace.index;
	newFace.iFaceNeighborN = 0;
	newFace.Vers[0] = parentFace.Vers[0];
	newFace.Vers[1] = parentFace.EdgeVertex[0];
	newFace.Vers[2] = parentFace.EdgeVertex[2];
	m_VecFace3D.push_back(newFace);
//////////////////////////////////////////////////////////////////////////
	nIndex = m_VecFace3D.size();
	newFace.index = nIndex;
	newFace.bSubd1To4 = false;
	newFace.bSubd1To2 = false;
	for ( int j = 0 ;j < 3 ; j++)
	{
		newFace.EdgeVertex[j] = -1;
		newFace.neighborFace[j] = -1;
	}
	newFace.parentFace = parentFace.index;
	newFace.iFaceNeighborN = 0;
	newFace.Vers[0] = parentFace.EdgeVertex[0];
	newFace.Vers[1] = parentFace.Vers[1];
	newFace.Vers[2] = parentFace.EdgeVertex[1];

	m_VecFace3D.push_back(newFace);
	//////////////////////////////////////////////////////////////////////////
	nIndex = m_VecFace3D.size();
	newFace.index = nIndex;
	newFace.bSubd1To4 = false;
	newFace.bSubd1To2 = false;
	for ( int j = 0 ;j < 3 ; j++)
	{
		newFace.EdgeVertex[j] = -1;
		newFace.neighborFace[j] = -1;
	}
	newFace.parentFace = parentFace.index;
	newFace.iFaceNeighborN = 0;
	newFace.Vers[0] = parentFace.EdgeVertex[2];
	newFace.Vers[1] = parentFace.EdgeVertex[1];
	newFace.Vers[2] = parentFace.Vers[2];

	m_VecFace3D.push_back(newFace);
	//////////////////////////////////////////////////////////////////////////
	nIndex = m_VecFace3D.size();
	newFace.index = nIndex;
	newFace.bSubd1To4 = false;
	newFace.bSubd1To2 = false;
	for ( int j = 0 ;j < 3 ; j++)
	{
		newFace.EdgeVertex[j] = -1;
		newFace.neighborFace[j] = -1;
	}
	newFace.parentFace = parentFace.index;
	newFace.iFaceNeighborN = 0;
	newFace.Vers[0] = parentFace.EdgeVertex[0];
	newFace.Vers[1] = parentFace.EdgeVertex[1];
	newFace.Vers[2] = parentFace.EdgeVertex[2];

	m_VecFace3D.push_back(newFace);
}

/*
* @param parentFace 新生成面的父面的索引值
* @brief 将面parentFaceIndex分裂成两个子面,插入到m_VecFace3D的末尾
* @return false 表示失败，true表示成功
*/
void KModelSubd::CreateTwoNewFaces(const ParentFace parentFace)
{
	Face3D newFace;
	int nIndex;
	int nEdge;
	for ( int j = 0 ; j < 3 ; j++ )
	{
		if ( parentFace.EdgeVertex[j] != -1 )
		{
			nEdge = j;
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	nIndex = m_VecFace3D.size();
	newFace.index = nIndex;
	newFace.bSubd1To4 = false;
	newFace.bSubd1To2 = false;
	for ( int j = 0 ;j < 3 ; j++)
	{
		newFace.EdgeVertex[j] = -1;
		newFace.neighborFace[j] = -1;
	}
	newFace.parentFace = parentFace.index;
	newFace.iFaceNeighborN = 0;
	newFace.Vers[0] = parentFace.Vers[(nEdge+2)%3];
	newFace.Vers[1] = parentFace.Vers[nEdge];
	newFace.Vers[2] = parentFace.EdgeVertex[nEdge];

	m_VecFace3D.push_back(newFace);
	//////////////////////////////////////////////////////////////////////////
	nIndex = m_VecFace3D.size();
	newFace.index = nIndex;
	newFace.bSubd1To4 = false;
	newFace.bSubd1To2 = false;
	for ( int j = 0 ;j < 3 ; j++)
	{
		newFace.EdgeVertex[j] = -1;
		newFace.neighborFace[j] = -1;
	}
	newFace.parentFace = parentFace.index;
	newFace.iFaceNeighborN = 0;
	newFace.Vers[0] = parentFace.Vers[(nEdge+2)%3];
	newFace.Vers[1] = parentFace.EdgeVertex[nEdge];
	newFace.Vers[2] = parentFace.Vers[(nEdge+1)%3];

	m_VecFace3D.push_back(newFace);
}

/*
* @param parentFace 新生成面的父面的索引值
* @brief 从父面Clone到新的子面中去
* @return false 表示失败，true表示成功
*/
void KModelSubd::CloneFromParentFace(const ParentFace parentFace)
{
	Face3D newFace;
	int nIndex;

	nIndex = m_VecFace3D.size();
	newFace.index = nIndex;
	newFace.bSubd1To4 = false;
	newFace.bSubd1To2 = false;
	for ( int j = 0 ;j < 3 ; j++)
	{
		newFace.EdgeVertex[j] = -1;
		newFace.neighborFace[j] = -1;
	}
	newFace.parentFace = parentFace.index;
	newFace.iFaceNeighborN = 0;
	newFace.Vers[0] = parentFace.Vers[0];
	newFace.Vers[1] = parentFace.Vers[1];
	newFace.Vers[2] = parentFace.Vers[2];

	m_VecFace3D.push_back(newFace);
}

/*
* @param void
* @brief 对KModelSubd的全局变量的初始化赋值
* @return false 表示失败，true表示成功
*/
int KModelSubd::InitializeSubd()
{
	int nResult = false;
	///计算Mesh中各个点的度数,邻接面的个数和值
	if (m_nSubdivisionN == 0)
	{
		for (int i = 0 ; i < m_iNumFaces ; i++ )
		{
			CalcFaceNormal(m_VecFace3D[i].Normal,i);

		}

		CalcVertexNeighborValue();
		
		if (m_lpTexture)
		{
			CalcTextureHighValue(m_lpTexture);
			ChangeMeshHigh();

		}
		SetMaxMinArea();
	}

	for (int i = 0 ; i < m_iNumFaces ; i++ )
	{
		
		
		m_VecFace3D[i].parentFace = -1;
		m_VecFace3D[i].bSubd1To2 = false;
		m_VecFace3D[i].bSubd1To4 = false;
		
		float fRefine = 0.0;
		fRefine = SetRefineParam(i);
		
		m_VecFace3D[i].RefineParam = fRefine;
		float fHigh;
		fHigh = SetHighDif(i);
		m_VecFace3D[i].HighDif =fHigh;
		
		float fArea;
		fArea = GetFaceArea(i);

	}

	nResult = true;
		return nResult;
}

/*
* @param void
* @brief 网格细化的主程序
* @return false 表示失败，true表示成功
*/
int KModelSubd::Refinement()
{
	int iResult = false;
	int iNumFace = m_iNumFaces;
	int iNumVers = m_iNumVers;
	int nNumNewFaces = 0;
	
	vector <ParentFace> vec_ParentFace;
	ParentFace tempParent;

	InitializeSubd();
	for (int i = 0 ; i < iNumFace ; i++ )
	{
		if ( m_VecFace3D[i].HighDif > HIGH_DIFFERENT_VALUE ) 
			if ( !IsSameHeightVertexofFace( i ) )
				LoopSubdivision(i);
		//else if ( ( m_VecFace3D[i].RefineParam < m_fMaxArea ) && ( m_VecFace3D[i].RefineParam > m_fMinArea ))
			//	LineInterpolate(i);
	} 
	m_iNumVers = m_VecVers3D.size();//修改网格中点的个数

	///将面信息先存储到parentFace中,以便下面的生成新面,和拓扑调整
	for (int i = 0 ; i < iNumFace ; i++ )
	{
		//Face3D * p3D ;
		//p3D = &m_VecFace3D[i];
		tempParent.bSubd1To2 = m_VecFace3D[i].bSubd1To2;
		tempParent.bSubd1To4 = m_VecFace3D[i].bSubd1To4;
		tempParent.index = m_VecFace3D[i].index;
		for (int j = 0 ; j < 3 ; j++)
		{
			tempParent.EdgeVertex[j]= m_VecFace3D[i].EdgeVertex[j];
			tempParent.NeighborFace[j] = m_VecFace3D[i].neighborFace[j];
			tempParent.Vers[j] = m_VecFace3D[i].Vers[j];
		}
		tempParent.Normal = m_VecFace3D[i].Normal;
		vec_ParentFace.push_back(tempParent);
	}

	///create new Faces
	m_VecFace3D.clear();
	
	ParentFace *pParentFace;
	for (int iFace = 0 ; iFace < iNumFace ; iFace++ )
	{
		pParentFace = &vec_ParentFace[iFace];
		if ( vec_ParentFace[iFace].bSubd1To4 == true ) 
		{
			//CreateFourNewFaces(vec_ParentFace[iFace],m_iNewFaces);			
			CreateFourNewFaces(vec_ParentFace[iFace]);			
			vec_ParentFace[iFace].ChildFace[0] = nNumNewFaces++;
			vec_ParentFace[iFace].ChildFace[1] = nNumNewFaces++;
			vec_ParentFace[iFace].ChildFace[2] = nNumNewFaces++;
			vec_ParentFace[iFace].ChildFace[3] = nNumNewFaces++;
		}
		else if (vec_ParentFace[iFace].bSubd1To2 == true)			
		{
			//CreateTwoNewFaces(vec_ParentFace[iFace],m_iNewFaces);
			CreateTwoNewFaces(vec_ParentFace[iFace]);
			vec_ParentFace[iFace].ChildFace[0] = nNumNewFaces++;
			vec_ParentFace[iFace].ChildFace[1] = nNumNewFaces++;
		}
		else
		{
			//CloneFromParentFace(vec_ParentFace[iFace],m_iNewFaces);
			CloneFromParentFace(vec_ParentFace[iFace]);
			vec_ParentFace[iFace].ChildFace[0] = nNumNewFaces++;
			//m_iNewFaces++;
		}

	}

	///build topology
	BuildAdjacency(vec_ParentFace);

	m_iNumFaces = nNumNewFaces;

	///给新产生的点和面建立度,iFaceN,等其他相关的属性
	CalcVertexNeighborValue();

	for (int iFace = 0 ; iFace < m_iNumFaces ; iFace++ )
	{	
		//CalcFaceNormal( m_VecFace3D[iFace].Normal,iFace);
		
		int nParent;
		nParent = m_VecFace3D[iFace].parentFace;

		CalcFaceNormal2( m_VecFace3D[iFace].Normal,vec_ParentFace[nParent].Normal);		
		
	}
	for ( int nVer = 0 ; nVer < m_iNumVers ; nVer++ )
	{
		CalcVertexNormal(m_VecVers3D[nVer].normal,nVer);
	}

	///根据红色通道修改点的高度值,
	if (m_lpTexture)
	{
		CalcTextureHighValue(m_lpTexture);
		ChangeMeshHigh();
	}
	m_nSubdivisionN++;
	iResult = true;


	return iResult;
}


/**
* @param & oddVertex 输出奇点
* @param faceIndex 面的索引值
* @param edgeIndex 边的索引值
* @brief 面faceIndex上的第edgeIndex条边计算奇点的几何位置,使用loop方法
* @return false 表示失败，true表示成功
*/
int KModelSubd::CalcOddVertex(Vertex3D & oddVertex,int faceIndex, int edgeIndex)
{
	int iResult =false;

	float x,y,z;
	float tu,tv;

	int iCurVertex,iNextVertex;///与oddVertex关联的两个点
	iCurVertex = m_VecFace3D[faceIndex].Vers[edgeIndex];
	iNextVertex = m_VecFace3D[faceIndex].Vers[( edgeIndex + 1 ) % 3 ];
	if (m_VecFace3D[faceIndex].neighborFace[edgeIndex] != -1)
	{//interior
		int ineighborFaceIndex = m_VecFace3D[faceIndex].neighborFace[edgeIndex];
		int iOtherVertex = m_VecFace3D[faceIndex].Vers[(edgeIndex + 2) %3];
		int iNeighborVertex;
		for (int i = 0 ; i < 3 ; i++ )		
			if ( (m_VecFace3D[ineighborFaceIndex].Vers[i] != iCurVertex) && ( m_VecFace3D[ineighborFaceIndex].Vers[i] != iNextVertex ) )
			{
				iNeighborVertex = m_VecFace3D[ineighborFaceIndex].Vers[i];
				break;
			}
		/*
		* The weight of iCurVertex,iNextVertex are  3/8 , and those of iOtherVertex and  iNeighborVertex are 1/8
		*/
		x = (float)( 3 * ( m_VecVers3D[iCurVertex].position.x + m_VecVers3D[iNextVertex].position.x )
			+ 1 * ( m_VecVers3D[iOtherVertex].position.x + m_VecVers3D[iNeighborVertex].position.x )) /8;
		y = (float)( 3 * ( m_VecVers3D[iCurVertex].position.y + m_VecVers3D[iNextVertex].position.y )  
			+ 1 * ( m_VecVers3D[iOtherVertex].position.y + m_VecVers3D[iNeighborVertex].position.y )) /8;
		z = (float)( 3 * ( m_VecVers3D[iCurVertex].position.z + m_VecVers3D[iNextVertex].position.z )  
			+ 1 * ( m_VecVers3D[iOtherVertex].position.z + m_VecVers3D[iNeighborVertex].position.z )) / 8;
		///给tu,tv赋值
		tu = (float)( 3 * ( m_VecVers3D[iCurVertex].tu + m_VecVers3D[iNextVertex].tu )
			+ 1 * ( m_VecVers3D[iOtherVertex].tu + m_VecVers3D[iNeighborVertex].tu )) /8;
		tv = (float)( 3 * ( m_VecVers3D[iCurVertex].tv + m_VecVers3D[iNextVertex].tv )
			+ 1 * ( m_VecVers3D[iOtherVertex].tv + m_VecVers3D[iNeighborVertex].tv )) /8;
	}
	else 
	{///boundary 
		x = (float) ( m_VecVers3D[iCurVertex].position.x + m_VecVers3D[iNextVertex].position.x ) /2 ;
		y = (float) ( m_VecVers3D[iCurVertex].position.y + m_VecVers3D[iNextVertex].position.y ) /2 ;
		z = (float) ( m_VecVers3D[iCurVertex].position.z + m_VecVers3D[iNextVertex].position.z ) /2 ;
		tu = (float)( m_VecVers3D[iCurVertex].tu + m_VecVers3D[iNextVertex].tu )/2;
		tv = (float)( m_VecVers3D[iCurVertex].tv + m_VecVers3D[iNextVertex].tv )/2;
	}

	oddVertex.position.x = x;
	oddVertex.position.y = y;
	oddVertex.position.z = z;
	oddVertex.tu = tu;
	oddVertex.tv = tv;
	//oddVertex.index = m_iNumVers++;
/**
	tu,tv值估计很有问题
*/
	iResult= true;

	return iResult;
}

/**
* @param faceIndex 面的索引值
* @param vertexIndex 点的索引值
* @brief修改faceIndex上的第vertexIndex 个even 点
* @return void
*/
void KModelSubd::CalcEvenVertex(int faceIndex, int vertexIndex)
{
	float x,y,z;
	x=y=z=0;
	int k ;
	float fBeta;
	float fAlfa;
	int iEvenVertex;
	iEvenVertex = m_VecFace3D[faceIndex].Vers[vertexIndex];///得到该even点的索引值
	k = m_VecVers3D[iEvenVertex].degree;
	//Vertex3D * p3D ;
	//p3D = & m_VecVers3D[iEvenVertex];

	fAlfa = 3/8 + 1/4 * cos((2*PI)/k);
	fBeta = (float) ( 5/8 - fAlfa * fAlfa )  /k;

	for (int iAdajance = 0 ; iAdajance < k ; iAdajance++)
	{
		int iVertex = m_VecVers3D[iEvenVertex].pVertexN[iAdajance];
		x +=(float) fBeta * m_VecVers3D[iVertex].position.x ;
		y +=(float) fBeta * m_VecVers3D[iVertex].position.y ;
		z +=(float) fBeta * m_VecVers3D[iVertex].position.z ;
	}
	x += (1 - k * fBeta ) * m_VecVers3D[iEvenVertex].position.x;
	y += (1 - k * fBeta ) * m_VecVers3D[iEvenVertex].position.y;
	z += (1 - k * fBeta ) * m_VecVers3D[iEvenVertex].position.z;
	
	m_VecVers3D[iEvenVertex].position.x = x ;
	m_VecVers3D[iEvenVertex].position.y = y ;
	m_VecVers3D[iEvenVertex].position.z = z ;
	/**
	*没有修改tu,tv值,
	*/

}
/**
* @param & vertex 输出的顶点向量
* @param faceIndex 面的索引值
* @param edgeIndex 边的索引值
* @brief在面faceIndex的第edgeIndex面上线性插入一个点vertex
* @remark 其中存储了该点的position, index, tu, tv 值
* @return false 表示失败，true表示成功
*/
int KModelSubd::LineInterpolateMid(Vertex3D &vertex,int faceIndex, int edgeIndex)
{
	int iResult = false;
	Vertex3D  vertexCur;

	Vertex3D  vertexNext;

	int iCurIndex,iNextIndex;
	iCurIndex = m_VecFace3D[faceIndex].Vers[edgeIndex];
	iNextIndex = m_VecFace3D[faceIndex].Vers[(edgeIndex+1)%3];
	vertexCur =  m_VecVers3D[iCurIndex] ;
	vertexNext = m_VecVers3D[iNextIndex] ;
	MidVector(vertex.position,vertexCur.position,vertexNext.position);
	
	vertex.tu = ( vertexCur.tu + vertexNext.tu ) / 2 ;
	vertex.tv = ( vertexCur.tv + vertexNext.tv ) / 2 ;
	
	iResult = true;

	return iResult;
}

/**
 * @brief 修改faceIndex上的第edgeIndex 个邻接面上得odd vertex,
 * @param faceIndex 面的索引值
 * @param edgeIndex 面的边索引值
 * @return false 表示失败，true表示成功
 * @remark 这是一个递归,直接修改m_VecFace3D中得数据
 * 
**/
int KModelSubd::ModifyNeighborFace(int faceIndex, int edgeIndex)
{
	int iResult = false;
	int iNeighborFace = m_VecFace3D[faceIndex].neighborFace[edgeIndex];	///faceIndex面的第edgeIndex边上的邻接面
	
	Vertex3D * newVertex = NULL;
	newVertex = new Vertex3D();
	if (!newVertex)
		goto Exit0;
	if (iNeighborFace != -1)
	{	
		if (m_VecFace3D[iNeighborFace].bSubd1To4 != true)
		{
			for (int i = 0 ; i < 3 ; i++ )
			{	
				///set the edge vertex of the neighbor face
				if (m_VecFace3D[iNeighborFace].neighborFace[i] == faceIndex)
				{
					m_VecFace3D[iNeighborFace].EdgeVertex[i] = m_VecFace3D[faceIndex].EdgeVertex[edgeIndex];
					break;
				}
			}
			if (m_VecFace3D[iNeighborFace].bSubd1To2 == true)
			{
				////查找otherEdge
				int iOtherEdge;
				for (int i = 0 ; i < 3 ; i++ )
				{	
					///set the edge vertex of the neighbor face
					if  ( m_VecFace3D[iNeighborFace].EdgeVertex[i] == -1 ) 
					{
						iOtherEdge = i;					
						break;
					}					
				}
				
				if ( ( m_VecFace3D[iNeighborFace].HighDif <= HIGH_DIFFERENT_VALUE) ||
					( IsSameHeightVertexofFace( iNeighborFace ) ) )
				{///该邻接面不满足再次细化的条件,需要进行2-4细分调整
					LineInterpolateMid( *newVertex, iNeighborFace, iOtherEdge);
					newVertex->index = m_VecVers3D.size();
					m_VecVers3D.push_back(*newVertex);
					m_VecFace3D[iNeighborFace].EdgeVertex[iOtherEdge] =  newVertex->index;
					m_VecFace3D[iNeighborFace].bSubd1To4 = true;
					ModifyNeighborFace(iNeighborFace, iOtherEdge);
				}

			}
			else 
			{
				m_VecFace3D[iNeighborFace].bSubd1To2 = true;
			}
		}
		iResult = true;
	}
	else 
	{
		goto Exit0;
	}	
	
Exit0:
	if ( newVertex )
		delete newVertex;

	return iResult;
}

/**
* @param faceIndex 面的索引值
* @brief 对面faceIndex进行loop细分
* @return false 表示失败，true表示成功
* @remark 在该函数中,只是生成边点(odd vertex),并修改 even vertex
*         至于生成新得面等到所有得面都loop完了,再生成.以便进行临界面得判断
**/

int KModelSubd::LoopSubdivision(int faceIndex)
{
	int iResult;
	int iNumVers = m_iNumVers;
	int iNumFaces = m_iNumFaces;

	//Face3D face3d;
	Vertex3D oddVertex3D;
	for (int iEdge = 0 ; iEdge < 3 ; iEdge++)
	{
		if (m_VecFace3D[faceIndex].EdgeVertex[iEdge] == -1)///如果该面上得边点还没有生成
			//if ( CalcOddVertex( oddVertex3D, faceIndex,iEdge ) != false)
			{
				//oddVertex->index = m_iNumVers++;///将新产生的点插入队列的后面
				CalcOddVertex( oddVertex3D, faceIndex,iEdge );
				oddVertex3D.degree = 0;
				oddVertex3D.iFaceN = 0;
				oddVertex3D.pVertexN.clear();
				oddVertex3D.pFaceN.clear();
				oddVertex3D.index = m_VecVers3D.size();
				m_VecVers3D.push_back(oddVertex3D);
				m_VecFace3D[faceIndex].EdgeVertex[iEdge] = oddVertex3D.index;
				///设置其他属性
			}
	}

	for (int iVertex = 0 ; iVertex < 3 ; iVertex++)
		 CalcEvenVertex(faceIndex,iVertex); 

	///置该面已经1-4细分了
	m_VecFace3D[faceIndex].bSubd1To4 = true;
	
	// face3d = m_VecFace3D[faceIndex];

	for (int iEdge = 0 ; iEdge < 3 ; iEdge++)
		if (m_VecFace3D[faceIndex].neighborFace[iEdge] != -1)
			 ModifyNeighborFace( faceIndex, iEdge );
	
	iResult = true;

	return iResult;

}
/**
* @param void
* @brief render该网格
* @return false 表示失败，true表示成功
**/
HRESULT KModelSubd::Render()
{
	DrawMesh(m_pMeshSubdSys);
	//g_pd3dDevice->SetTexture(0,NULL);
	//DrawMeshSubset(m_pMeshSubdSys,0);
	//m_pWaterUp->DrawSubset(0);

	return S_OK;
}
/**
* @param void
* @brief 保存数据到m_pMeshSubdSys
* @return false 表示失败，true表示成功
*/
int KModelSubd::SaveSubdMesh()
{
	int iResult = false;
	
	VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
	WORD* pIndex = NULL;
	DWORD * pAttrib = NULL;	

	if (FAILED(D3DXCreateMeshFVF(m_iNumFaces,m_iNumVers,D3DXMESH_MANAGED,
		(D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1),g_pd3dDevice,&m_pMeshSubdSys )))
	{
		goto Exit0;
	}
	if (FAILED(m_pMeshSubdSys->LockVertexBuffer(0,(void**)&pVers)))
		goto Exit0;
	if (FAILED(m_pMeshSubdSys->LockIndexBuffer (0,(void**)&pIndex)))
		goto Exit0;
	if (FAILED(m_pMeshSubdSys->LockAttributeBuffer(0,(DWORD**)&pAttrib)))
		goto Exit0;	

	///赋值操作
	for ( int iVers = 0 ; iVers < m_iNumVers ; iVers++ )
	{
		pVers[iVers].Normal = m_VecVers3D[iVers].normal;
		pVers[iVers].p = m_VecVers3D[iVers].position;
		pVers[iVers].tu1 = m_VecVers3D[iVers].tu;
		pVers[iVers].tv1 = m_VecVers3D[iVers].tv;		
	}
	for (int iFace = 0 ; iFace < m_iNumFaces ; iFace++ )
	{
		pIndex[3*iFace] = m_VecFace3D[iFace].Vers[0];
		pIndex[3*iFace+1] = m_VecFace3D[iFace].Vers[1];
		pIndex[3*iFace+2] = m_VecFace3D[iFace].Vers[2];
		pAttrib[iFace] = 0;
	}
	if (FAILED(m_pMeshSubdSys->UnlockVertexBuffer()))
		goto Exit0;
	if (FAILED(m_pMeshSubdSys->UnlockIndexBuffer()))
		goto Exit0;
	if (FAILED(m_pMeshSubdSys->UnlockAttributeBuffer()))
		goto Exit0;

	iResult = true;
Exit0:

	return iResult;
}
/**
* @param pTexture 网格的贴图
* @brief 根据导入的Texture中的各点的RGB值,计算其向外凸出的高度
* @return false 表示失败，true表示成功
*/
int KModelSubd::CalcTextureHighValue( LPDIRECT3DTEXTURE9 pTexture)
{
	int nResult = false;
	if (!pTexture)
		goto Exit0;
	D3DSURFACE_DESC Desc;
	Desc.Width =0;/// width of texture 
	Desc.Height =0;/// height of texture

	if (FAILED(pTexture->GetLevelDesc(0,&Desc)))///得到第0层图的desc信息
		goto Exit0;
	if ((Desc.Format!=D3DFMT_A8R8G8B8)&&(Desc.Format!=D3DFMT_X8R8G8B8))
		goto Exit0;	

	D3DLOCKED_RECT lockedRect;
	lockedRect.pBits = NULL;

	if (FAILED(pTexture->LockRect(0, &lockedRect, NULL, D3DLOCK_READONLY )))
		goto Exit0;

	DWORD* pTextData = (DWORD*) lockedRect.pBits;
	for (int nVertex = 0 ; nVertex < m_iNumVers; nVertex++ )
	{
		float fRow,fColumn;///行和列
		fColumn= m_VecVers3D[nVertex].tu *Desc.Width;
		fRow = m_VecVers3D[nVertex].tv * Desc.Height;

		DWORD * pdwTemp = (DWORD *)pTextData +(DWORD) fRow * lockedRect.Pitch/4 + (DWORD)fColumn;
		unsigned char uncRed;
		uncRed = (BYTE)((*pdwTemp & 0x00ff0000)>>16);
		float fRed;
		fRed = (float)uncRed/255;
		m_VecVers3D[nVertex].fHeight = fRed;
	}
	pTexture->UnlockRect(0);

	nResult = true;
Exit0:
	return nResult;
}
/**
* @param pTexture 网格的贴图
* @brief 由外界导入texture
* @return false 表示失败，true表示成功
**/
int KModelSubd::LoadTexture(LPDIRECT3DTEXTURE9 pTexture)
{
	if ( pTexture)
	{
		m_lpTexture = pTexture;
		return true;
	}
	else
		return false;
}

/**
* @param void
* @brief 对初始网格,根据每个点的fHeight属性,改变他的position的值,使每个点由高度值而在几何上改变位置
* @return false 表示失败，true表示成功
*/
int KModelSubd::ChangeMeshHigh()
{
	D3DXVECTOR3 vectorHeight ;
	D3DXVECTOR3 vectorPosition ;

	for (int i = 0; i < m_iNumVers ; i++ )
	{

		//m_VecVers3D[i].position.y =   m_nHeight * abs(m_VecVers3D[i].fHeight);
		float fPositiony;

		if ( m_VecVers3D[i].fHeight > 0.05 )
		{ 

			fPositiony = m_VecVers3D[i].position.y + m_nHeight * m_VecVers3D[i].fHeight 
				- m_VecVers3D[i].position.y * m_VecVers3D[i].normal.y;
			if (fPositiony > m_VecVers3D[i].position.y)
				m_VecVers3D[i].position.y = fPositiony;
		}
		else 

		{
			m_VecVers3D[i].position.y = m_VecVers3D[i].position.y;
		}

		//m_VecVers3D[i].position.z = m_VecVers3D[i].position.z + m_nHeight * m_VecVers3D[i].Height.z 
		//	- m_VecVers3D[i].position.z * m_VecVers3D[i].normal.z;	

	}   

	return true;
}

/**
* @param void
* @brief 设置网格中的最大和最小面积
* @return false 表示失败，true表示成功
**/
int KModelSubd::SetMaxMinArea()
{
	float fMax,fMin,fCur;
	fMax = fMin = GetFaceArea(0);
	for ( int i = 1 ; i < m_iNumFaces ; i++ )
	{
		fCur = GetFaceArea(i);
		if ( fCur > fMax )
			fMax = fCur;
		if ( fCur < fMin )
			fMin = fCur;
	}
	m_fMinArea = fMin;
	m_fMaxArea = fMax;

	return true;
}

/**
* @param faceIndex 表示面的索引值
* @brief 判断一个面的通过通道确定的顶点高度值是否都相等
* @return false 表示失败，true 表示成功
**/
int KModelSubd::IsSameHeightVertexofFace( int  faceIndex)
{
	int nResult = false;
	float  fVertexHeight;
	int nVertexIndex;
	nVertexIndex = m_VecFace3D[faceIndex].Vers[0];
	fVertexHeight = m_VecVers3D[nVertexIndex].fHeight;
	for ( int nVertex = 0; nVertex< 3 ; nVertex++ )
	{
		nVertexIndex = m_VecFace3D[faceIndex].Vers[nVertex];
		fVertexHeight = m_VecVers3D[nVertexIndex].fHeight;
		if ( fVertexHeight > m_fVertexHeightDif ) ///如果超出了该高度值则认为该面的顶点通道高度值不相同
		{			
			return nResult;
		}
	}
	nResult = true;
	return nResult;
}

HRESULT KModelSubd::Acquire()
{
	//	if (!m_bNeedReAcquire)
	//		return S_OK;

	PropertyBase::_PropertyPage* pPage = NULL;
	int PageIndex = -1;

	PageIndex = this->AddPage("Information");

	pPage = this->FindPage(PageIndex);
	if(pPage)
	{
		PropertyBase::_PropertyGroup Group(0,"网格细化",FALSE);
		int index_Group = pPage->AddGroup(Group);
		PropertyBase::_PropertyGroup* pGroup = pPage->FindGroup(index_Group);

		pGroup->AddPropertyFloat("照相机远近",&m_fCameroLength,0.0f,100,1);

	}
	//	m_bNeedReAcquire = FALSE;
	return S_OK;
}
