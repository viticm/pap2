// File: TerrainTree.cpp
// Desc:

#include"TerrainTree.h"
#include"..\Render\Render.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Terrain
{

TerrainTree::TerrainTree(): Render::Renderable(), m_TTDesc(), m_HeightMap(), m_Root(0)
{
	m_CenterX = 0.f;
	m_CenterZ = 0.f;

	m_BTs = new BTNode[BTPOOL_SIZE];
	m_CurrentBTPos = 0;

	m_Stream.m_dwFVF = TerrainVertexFVF;
	m_Stream.m_SizeOfVertex = sizeof(TerrainVertex);
	m_CurrentNumOfVertices = 0;

	m_bDrawOutlineOnly = false;
}

TerrainTree::TerrainTree(const Common::String& sName): Render::Renderable(), m_TTDesc(sName), m_HeightMap(sName), m_Root(0)
{
	if(sName.IfContain('.'))
	{
		throw Common::Exception(Common::EL_BAD, "TerrainTree::TerrainTree", "Filename has an extension! Please remove the extension.");
	}

	m_TTDesc.SetName(sName + ".ttd");
	m_HeightMap.SetName(sName + ".hmv");

	m_TTDesc.ReadFile();
	m_HeightMap.ReadFile();

	m_CenterX = 0.f;
	m_CenterZ = 0.f;

	m_BTs = new BTNode[BTPOOL_SIZE];
	m_CurrentBTPos = 0;

	m_Stream.m_dwFVF = TerrainVertexFVF;
	m_Stream.m_pVertexBuffer = g_GetRenderCore()->CreateVertexBuffer(MAXNUM_VERTICES, sizeof(TerrainVertex), m_Stream.m_dwFVF);
	m_Stream.m_SizeOfVertex = sizeof(TerrainVertex);
	m_CurrentNumOfVertices = 0;

	m_TLOD.ReadConfig(sName + ".ttd", m_TTDesc.m_MaxLOD);

	m_bDrawOutlineOnly = false;
}

TerrainTree::~TerrainTree()
{
	if(GetCount() == 1)
	{
		DestroyTree();
		Destroy();

		SafeDeleteArray(m_BTs);
	}
}

void TerrainTree::CreateD3DResources()
{
	m_Stream.m_pVertexBuffer = g_GetRenderCore()->CreateVertexBuffer(MAXNUM_VERTICES, sizeof(TerrainVertex), m_Stream.m_dwFVF);
	m_TerrainGroup.Create(MAXNUM_INDICES);
}

void TerrainTree::SetName(const Common::String& sName)
{
	if(sName.IfContain('.'))
	{
		throw Common::Exception(Common::EL_BAD, "TerrainTree::SetName", "Filename has an extension! Please remove the extension.");
	}

	m_TTDesc.SetName(sName + ".ttd");
	m_HeightMap.SetName(sName + ".hmv");

	m_TTDesc.ReadFile();
	m_HeightMap.ReadFile();

	m_TLOD.ReadConfig(sName + ".ttd", m_TTDesc.m_MaxLOD);

	DestroyTree();
}

void TerrainTree::SetTerrainTreeDesc(const TerrainTreeDesc& TTDesc)
{
	m_TTDesc = TTDesc;

	//Common::String sTerrainName = m_TTDesc.m_TTDFilename;
	//sTerrainName = sTerrainName.substr(0, sTerrainName.GetSize() - 4);
	//m_HeightMap.SetName(sTerrainName);
	//m_HeightMap.ReadFile();

	m_TLOD.ReadConfig(m_TTDesc.m_TTDFilename, m_TTDesc.m_MaxLOD);

	DestroyTree();
}

void TerrainTree::CreateQuadTree(float cx, float cy, float cz)
{
	m_CenterX = cx;
	m_CenterZ = cz;

	m_Root = new QuadNode(Maths::Vector3F(cx, GetHeightValue(cx, cz), cz), GetTerrainWidth(), GetTerrainHeight(), 0);
	CreateQuadTreeSublevels(m_Root, 1, Maths::CalNumOfPower(m_TTDesc.m_FNs), Maths::CalNumOfPower(m_TTDesc.m_QLs) - 1);

	for(unsigned int i = 0; i < m_AllQuadLeaves.size(); ++i)
	{
		QuadLeaf* pQL = m_AllQuadLeaves[i];

		BTNode* pBT = pQL->GetBTNodeRoot(QuadNode::BTP_LEFT);
		pBT->SetNeighbours(BTNode::CP_LEFT);
		pBT = pQL->GetBTNodeRoot(QuadNode::BTP_RIGHT);
		pBT->SetNeighbours(BTNode::CP_RIGHT);

		SetQuadLeafTLUV(pQL);
	}
}

void TerrainTree::CreateQuadTreeSublevels(QuadNode* pNode, const unsigned int Sublevel, const unsigned int NumOfFieldNodeLevel, const unsigned int NumOfBlockNodeLevel)
{
	if(!pNode)
	{
		return;
	}

	float NodeWidth = pNode->GetXLength();
	float NodeHeight = pNode->GetZLength();

	float dx = NodeWidth * 0.25f;
	float dz = NodeHeight * 0.25f;
	float fHalfWidth = NodeWidth * 0.5f;
	float fHalfHeight = NodeHeight * 0.5f;

	Maths::Vector3F v[QuadNode::MAX_CHILDS];  // The four center points for the four childs

	v[QuadNode::CP_TL].x = pNode->m_Center.x - dx;
	v[QuadNode::CP_TL].z = pNode->m_Center.z + dz;
	v[QuadNode::CP_TL].y = GetHeightValue(v[QuadNode::CP_TL].x, v[QuadNode::CP_TL].z);

	v[QuadNode::CP_TR].x = pNode->m_Center.x + dx;
	v[QuadNode::CP_TR].z = pNode->m_Center.z + dz;
	v[QuadNode::CP_TR].y = GetHeightValue(v[QuadNode::CP_TR].x, v[QuadNode::CP_TR].z);

	v[QuadNode::CP_BR].x = pNode->m_Center.x + dx;
	v[QuadNode::CP_BR].z = pNode->m_Center.z - dz;
	v[QuadNode::CP_BR].y = GetHeightValue(v[QuadNode::CP_BR].x, v[QuadNode::CP_BR].z);

	v[QuadNode::CP_BL].x = pNode->m_Center.x - dx;
	v[QuadNode::CP_BL].z = pNode->m_Center.z - dz;
	v[QuadNode::CP_BL].y = GetHeightValue(v[QuadNode::CP_BL].x, v[QuadNode::CP_BL].z);

	if(Sublevel == NumOfFieldNodeLevel + NumOfBlockNodeLevel - 1)  // The childs to be created are QuadLeaves; This is the last sublevel we will create. After creating it, the recursive function returns
	{
		pNode->m_qnChilds = new QuadLeaf[QuadNode::MAX_CHILDS];
		for(unsigned int ci = 0; ci < QuadNode::MAX_CHILDS; ++ci)
		{
			QuadLeaf* pQL = static_cast<QuadLeaf*>(pNode->GetChild(ci));
			m_AllQuadLeaves.push_back(pQL);
		}
	}
	else if(Sublevel == NumOfFieldNodeLevel - 1)  // The childs to be created are FieldNodes
	{
		FieldNode* pFN = new FieldNode[QuadNode::MAX_CHILDS];
		pNode->m_qnChilds = pFN;
	}
	else  // The childs to be created are QuadNodes
	{
		pNode->m_qnChilds = new QuadNode[QuadNode::MAX_CHILDS];
	}

	unsigned int i = 0;
	for(; i < QuadNode::MAX_CHILDS; ++i)
	{
		QuadNode* pChild = pNode->GetChild(i);
		if(Sublevel == NumOfFieldNodeLevel + NumOfBlockNodeLevel - 1)
		{
			QuadLeaf* pQL = static_cast<QuadLeaf*>(pChild);
			pQL->Init(v[i], fHalfWidth, fHalfHeight, pNode);
		}
		else if(Sublevel == NumOfFieldNodeLevel - 1)  // The childs to be created are FieldNodes
		{
			FieldNode* pFN = static_cast<FieldNode*>(pChild);
			pFN->Init(v[i], fHalfWidth, fHalfHeight, pNode);

			m_AllFieldNodes.push_back(pFN);
		}
		else
		{
			QuadNode* pQN = pChild;
			pQN->Init(v[i], fHalfWidth, fHalfHeight, pNode);
		}

		if(Sublevel != NumOfFieldNodeLevel + NumOfBlockNodeLevel - 1)  // The childs we just created are not QuadLeaves so we should create their childs recursively.
		{
			CreateQuadTreeSublevels(pChild, Sublevel + 1, NumOfFieldNodeLevel, NumOfBlockNodeLevel);
		}
	}
}

void TerrainTree::SetQuadLeafTLUV(QuadLeaf* pQL)
{
	unsigned int row = 0, col = 0;  // pQL在哪一行哪一列，从0开始计数

	QuadNode* pTemp = pQL->GetNeighbour(QuadNode::NP_T);
	while(pTemp)
	{
		++row;
		pTemp = pTemp->GetNeighbour(QuadNode::NP_T);
	}
	pTemp = pQL->GetNeighbour(QuadNode::NP_L);
	while(pTemp)
	{
		++col;
		pTemp = pTemp->GetNeighbour(QuadNode::NP_L);
	}

	row %= 4;
	col %= 2;

	switch(row)
	{
	case 0:
		switch(col)
		{
		case 0:
			pQL->m_TL_U = 0.f;
			pQL->m_TL_V = 0.f;
			break;

		case 1:
			pQL->m_TL_U = 1.f;
			pQL->m_TL_V = 0.f;
			break;
		}
		break;

	case 1:
		switch(col)
		{
		case 0:
			pQL->m_TL_U = 0.f;
			pQL->m_TL_V = 1.f;
			break;

		case 1:
			pQL->m_TL_U = 1.f;
			pQL->m_TL_V = 1.f;
			break;
		}
		break;

	case 2:
		switch(col)
		{
		case 0:
			pQL->m_TL_U = 1.f;
			pQL->m_TL_V = 0.f;
			break;

		case 1:
			pQL->m_TL_U = 0.f;
			pQL->m_TL_V = 0.f;
			break;
		}
		break;

	case 3:
		switch(col)
		{
		case 0:
			pQL->m_TL_U = 1.f;
			pQL->m_TL_V = 1.f;
			break;

		case 1:
			pQL->m_TL_U = 0.f;
			pQL->m_TL_V = 1.f;
			break;
		}
		break;

	default:
		pQL->m_TL_U = 0.f;
		pQL->m_TL_V = 0.f;
		break;
	}
}

void TerrainTree::DestroyTree()
{
	SafeDelete(m_Root);

	m_AllFieldNodes.clear();
	m_VisibleFieldNodes.clear();

	m_CurrentBTPos = 0;

	m_VisibleQuadLeaves.clear();
	m_TouchedQuadLeaves.clear();

	m_CurrentNumOfVertices = 0;
	m_TerrainGroup.Reset();
}

bool TerrainTree::GenerateOutline(const Maths::Camera& camera)  // Just find out visible QuadLeaves and create vertices and indices
{
	m_NumOfVisibleFieldNodes = 0;
	m_NumOfVisibleQuadLeaves = 0;
	m_NumOfVertices = 0;
	m_NumOfPrimitives = 0;


	m_VisibleFieldNodes.clear();  // Note here
	m_VisibleQuadLeaves.clear();
	m_CurrentNumOfVertices = 0;
	m_TerrainGroup.Reset();

	m_TimeWhileFindVisibleFieldNodes = g_GetDateTimeSystem()->GetTimeValueMS();
	if(m_Root->FindVisibleFieldNodes(camera, m_VisibleFieldNodes))  // First, find out all visible FieldNodes
	{
		m_TimeWhileFindVisibleFieldNodes = g_GetDateTimeSystem()->GetTimeValueMS() - m_TimeWhileFindVisibleFieldNodes;

		unsigned int i = 0;

		// Second, for each FieldNode, find out all visible QuadLeaves
		m_TimeWhileFindVisibleQuadLeaves = g_GetDateTimeSystem()->GetTimeValueMS();
		unsigned int NumOfVisibleFieldNodes = m_VisibleFieldNodes.size();
		for(i = 0; i < NumOfVisibleFieldNodes; ++i)
		{
			FieldNode* pFN = m_VisibleFieldNodes[i];
			pFN->FindVisibleQuadLeaves(camera, m_VisibleQuadLeaves);
		}
		m_TimeWhileFindVisibleQuadLeaves = g_GetDateTimeSystem()->GetTimeValueMS() - m_TimeWhileFindVisibleQuadLeaves;

		// Third for each visible QuadLeaves, set its outline vertex(s) and indices
		m_TimeWhileCreateOutlineVertices = 0;  // g_GetDateTimeSystem()->GetTimeValueMS();
		m_vDecoration.clear();
		void* pVB = LockVertexBuffer();
		unsigned int NumOfVisibleQuadLeaves = m_VisibleQuadLeaves.size();
		for(i = 0; i < NumOfVisibleQuadLeaves; ++i)
		{
			QuadLeaf* pQL = m_VisibleQuadLeaves[i];
			if(!pQL)
			{
				// Push error-code into error-pipeline
				return false;
			}

			TerrainVertex tv;
			pQL->GetCorner(QuadNode::CP_TL, tv);  // Top-left
			tv.Diffuse = m_TTDesc.m_TerrainBaseColor;
			tv.u = pQL->m_TL_U;
			tv.v = pQL->m_TL_V;
			PushVertex(tv, pVB);  // This will internally increment the value of m_CurrentNumOfVertices by 1

			pQL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_RIGHT] = m_CurrentNumOfVertices - 1;
			pQL->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_LEFT] = m_CurrentNumOfVertices - 1;

			unsigned int TimeWhileGetNeighbours = g_GetDateTimeSystem()->GetTimeValueMS();
			QuadLeaf* pQL_L = 0;
			QuadLeaf* pQL_T = 0;
			QuadLeaf* pQL_R = 0;
			QuadLeaf* pQL_B = 0;
			QuadLeaf* pQL_TL = 0;
			QuadLeaf* pQL_TR = 0;
			QuadLeaf* pQL_BR = 0;
			QuadLeaf* pQL_BL = 0;
			pQL_L = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_L));
			pQL_T = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_T));
			pQL_R = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_R));
			pQL_B = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_B));

			if(pQL_T && pQL_L)
			{
				pQL_TL = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_TL));
			}
			if(pQL_T && pQL_R)
			{
				pQL_TR = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_TR));
			}
			if(pQL_B && pQL_R)
			{
				pQL_BR = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_BR));
			}
			if(pQL_B && pQL_L)
			{
				pQL_BL = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_BL));
			}

			TimeWhileGetNeighbours = g_GetDateTimeSystem()->GetTimeValueMS() - TimeWhileGetNeighbours;
			m_TimeWhileCreateOutlineVertices += TimeWhileGetNeighbours;

			if(pQL_L)
			{
				pQL_L->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_TOP] = m_CurrentNumOfVertices - 1;

				if(!pQL_L->m_bVisible)
				{
					pQL_L->m_bVisible = true;
					m_vDecoration.push_back(pQL_L);
				}
			}
			if(pQL_TL)
			{
				pQL_TL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_LEFT] = m_CurrentNumOfVertices - 1;
				pQL_TL->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_RIGHT] = m_CurrentNumOfVertices - 1;

				if(!pQL_TL->m_bVisible)
				{
					pQL_TL->m_bVisible = true;
					m_vDecoration.push_back(pQL_TL);
				}
			}
			if(pQL_T)
			{
				pQL_T->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_TOP] = m_CurrentNumOfVertices - 1;

				if(!pQL_T->m_bVisible)
				{
					pQL_T->m_bVisible = true;
					m_vDecoration.push_back(pQL_T);
				}
			}

			bool bRightEdge = false;
			bool bBottomEdge = false;
			if(!pQL_R)
			{
				bRightEdge = true;

				pQL->GetCorner(QuadNode::CP_TR, tv);
				tv.Diffuse = m_TTDesc.m_TerrainBaseColor;
				if(Maths::FEqual(pQL->m_TL_U, 0.f) && Maths::FEqual(pQL->m_TL_V, 0.f))
				{
					tv.u = 1.f;
					tv.v = 0.f;
				}
				else if(Maths::FEqual(pQL->m_TL_U, 0.f) && Maths::FEqual(pQL->m_TL_V, 1.f))
				{
					tv.u = 1.f;
					tv.v = 1.f;
				}
				else if(Maths::FEqual(pQL->m_TL_U, 1.f) && Maths::FEqual(pQL->m_TL_V, 0.f))
				{
					tv.u = 0.f;
					tv.v = 0.f;
				}
				else
				{
					tv.u = 0.f;
					tv.v = 1.f;
				}
				PushVertex(tv, pVB);

				pQL->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_TOP] = m_CurrentNumOfVertices - 1;

				if(pQL_T)
				{
					pQL_T->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_LEFT] = m_CurrentNumOfVertices - 1;
					pQL_T->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_RIGHT] = m_CurrentNumOfVertices - 1;
				}
			}
			else if(!pQL_R->m_bVisible)
			{
				pQL_R->m_bVisible = true;
				m_vDecoration.push_back(pQL_R);
			}

			if(!pQL_B)
			{
				bBottomEdge = true;

				pQL->GetCorner(QuadNode::CP_BL, tv);
				tv.Diffuse = m_TTDesc.m_TerrainBaseColor;
				if(Maths::FEqual(pQL->m_TL_U, 0.f) && Maths::FEqual(pQL->m_TL_V, 0.f))  // 0
				{
					tv.u = 0.f;
					tv.v = 1.f;
				}
				else if(Maths::FEqual(pQL->m_TL_U, 0.f) && Maths::FEqual(pQL->m_TL_V, 1.f))  //2
				{
					tv.u = 1.f;
					tv.v = 0.f;
				}
				else if(Maths::FEqual(pQL->m_TL_U, 1.f) && Maths::FEqual(pQL->m_TL_V, 0.f))  // 1
				{
					tv.u = 1.f;
					tv.v = 1.f;
				}
				else  // 3
				{
					tv.u = 0.f;
					tv.v = 0.f;
				}
				PushVertex(tv, pVB);

				pQL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_TOP] = m_CurrentNumOfVertices - 1;

				if(pQL_L)
				{
					pQL_L->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_LEFT] = m_CurrentNumOfVertices - 1;
					pQL_L->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_RIGHT] = m_CurrentNumOfVertices - 1;
				}
			}
			else if(!pQL_B->m_bVisible)
			{
				pQL_B->m_bVisible = true;
				m_vDecoration.push_back(pQL_B);
			}

			if(bRightEdge && bBottomEdge)
			{
				pQL->GetCorner(QuadNode::CP_BR, tv);
				tv.Diffuse = m_TTDesc.m_TerrainBaseColor;
				TerrainVertex* temp = 0;
				GetTerrainVertex(pVB, pQL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_TOP], temp);
				if(Maths::FEqual(temp->u, 0.f) && Maths::FEqual(temp->v, 0.f))
				{
					tv.u = 1.f;
					tv.v = 0.f;
				}
				else if(Maths::FEqual(temp->u, 0.f) && Maths::FEqual(temp->v, 1.f))
				{
					tv.u = 1.f;
					tv.v = 1.f;
				}
				else if(Maths::FEqual(temp->u, 1.f) && Maths::FEqual(temp->v, 0.f))
				{
					tv.u = 0.f;
					tv.v = 0.f;
				}
				else
				{
					tv.u = 0.f;
					tv.v = 1.f;
				}
				PushVertex(tv, pVB);

				pQL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_LEFT] = m_CurrentNumOfVertices - 1;
				pQL->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_RIGHT] = m_CurrentNumOfVertices - 1;
			}

			if(pQL_TR && !pQL_TR->m_bVisible)
			{
				pQL_TR->m_bVisible = true;
				m_vDecoration.push_back(pQL_TR);
			}
			if(pQL_BR && !pQL_BR->m_bVisible)
			{
				pQL_BR->m_bVisible = true;
				m_vDecoration.push_back(pQL_BR);
			}
			if(pQL_BL && !pQL_BL->m_bVisible)
			{
				pQL_BL->m_bVisible = true;
				m_vDecoration.push_back(pQL_BL);
			}

			//SetQuadLeafTexCoords(pVB, pQL, bRightEdge, bBottomEdge);
		}
		/**********/
		unsigned int NumOfDecorationQLs = m_vDecoration.size();
		for(i = 0; i < NumOfDecorationQLs; ++i)
		{
			QuadLeaf* pQL = m_vDecoration[i];
			if(!pQL)
			{
				// Push error-code into error-pipeline
				return false;
			}

			TerrainVertex tv;
			pQL->GetCorner(QuadNode::CP_TL, tv);  // Top-left
			tv.Diffuse = m_TTDesc.m_TerrainBaseColor;
			tv.u = pQL->m_TL_U;
			tv.v = pQL->m_TL_V;
			PushVertex(tv, pVB);  // This will internally increment the value of m_CurrentNumOfVertices by 1

			pQL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_RIGHT] = m_CurrentNumOfVertices - 1;
			pQL->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_LEFT] = m_CurrentNumOfVertices - 1;

			unsigned int TimeWhileGetNeighbours = g_GetDateTimeSystem()->GetTimeValueMS();
			QuadLeaf* pQL_L = 0;
			QuadLeaf* pQL_T = 0;
			QuadLeaf* pQL_R = 0;
			QuadLeaf* pQL_B = 0;
			QuadLeaf* pQL_TL = 0;
			QuadLeaf* pQL_TR = 0;
			QuadLeaf* pQL_BR = 0;
			QuadLeaf* pQL_BL = 0;
			pQL_L = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_L));
			pQL_T = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_T));
			pQL_R = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_R));
			pQL_B = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_B));

			if(pQL_T && pQL_L)
			{
				pQL_TL = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_TL));
			}
			if(pQL_T && pQL_R)
			{
				pQL_TR = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_TR));
			}
			if(pQL_B && pQL_R)
			{
				pQL_BR = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_BR));
			}
			if(pQL_B && pQL_L)
			{
				pQL_BL = static_cast<QuadLeaf*>(pQL->GetNeighbour(QuadNode::NP_BL));
			}

			TimeWhileGetNeighbours = g_GetDateTimeSystem()->GetTimeValueMS() - TimeWhileGetNeighbours;
			m_TimeWhileCreateOutlineVertices += TimeWhileGetNeighbours;

			if(pQL_L)
			{
				pQL_L->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_TOP] = m_CurrentNumOfVertices - 1;
			}
			if(pQL_TL)
			{
				pQL_TL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_LEFT] = m_CurrentNumOfVertices - 1;
				pQL_TL->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_RIGHT] = m_CurrentNumOfVertices - 1;
			}
			if(pQL_T)
			{
				pQL_T->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_TOP] = m_CurrentNumOfVertices - 1;
			}

			bool bRightEdge = false;
			bool bBottomEdge = false;
			if(!pQL_R || !pQL_R->m_bVisible)
			{
				bRightEdge = true;

				pQL->GetCorner(QuadNode::CP_TR, tv);
				tv.Diffuse = m_TTDesc.m_TerrainBaseColor;
				if(Maths::FEqual(pQL->m_TL_U, 0.f) && Maths::FEqual(pQL->m_TL_V, 0.f))
				{
					tv.u = 1.f;
					tv.v = 0.f;
				}
				else if(Maths::FEqual(pQL->m_TL_U, 0.f) && Maths::FEqual(pQL->m_TL_V, 1.f))
				{
					tv.u = 1.f;
					tv.v = 1.f;
				}
				else if(Maths::FEqual(pQL->m_TL_U, 1.f) && Maths::FEqual(pQL->m_TL_V, 0.f))
				{
					tv.u = 0.f;
					tv.v = 0.f;
				}
				else
				{
					tv.u = 0.f;
					tv.v = 1.f;
				}
				PushVertex(tv, pVB);

				pQL->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_TOP] = m_CurrentNumOfVertices - 1;

				if(pQL_T)
				{
					pQL_T->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_LEFT] = m_CurrentNumOfVertices - 1;
					pQL_T->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_RIGHT] = m_CurrentNumOfVertices - 1;
				}
			}

			if(!pQL_B || !pQL_B->m_bVisible)
			{
				bBottomEdge = true;

				pQL->GetCorner(QuadNode::CP_BL, tv);
				tv.Diffuse = m_TTDesc.m_TerrainBaseColor;
				if(Maths::FEqual(pQL->m_TL_U, 0.f) && Maths::FEqual(pQL->m_TL_V, 0.f))  // 0
				{
					tv.u = 0.f;
					tv.v = 1.f;
				}
				else if(Maths::FEqual(pQL->m_TL_U, 0.f) && Maths::FEqual(pQL->m_TL_V, 1.f))  //2
				{
					tv.u = 1.f;
					tv.v = 0.f;
				}
				else if(Maths::FEqual(pQL->m_TL_U, 1.f) && Maths::FEqual(pQL->m_TL_V, 0.f))  // 1
				{
					tv.u = 1.f;
					tv.v = 1.f;
				}
				else  // 3
				{
					tv.u = 0.f;
					tv.v = 0.f;
				}
				PushVertex(tv, pVB);

				pQL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_TOP] = m_CurrentNumOfVertices - 1;

				if(pQL_L)
				{
					pQL_L->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_LEFT] = m_CurrentNumOfVertices - 1;
					pQL_L->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_RIGHT] = m_CurrentNumOfVertices - 1;
				}
			}

			if(bRightEdge && bBottomEdge)
			{
				pQL->GetCorner(QuadNode::CP_BR, tv);
				tv.Diffuse = m_TTDesc.m_TerrainBaseColor;
				TerrainVertex* temp = 0;
				GetTerrainVertex(pVB, pQL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_TOP], temp);
				if(Maths::FEqual(temp->u, 0.f) && Maths::FEqual(temp->v, 0.f))
				{
					tv.u = 1.f;
					tv.v = 0.f;
				}
				else if(Maths::FEqual(temp->u, 0.f) && Maths::FEqual(temp->v, 1.f))
				{
					tv.u = 1.f;
					tv.v = 1.f;
				}
				else if(Maths::FEqual(temp->u, 1.f) && Maths::FEqual(temp->v, 0.f))
				{
					tv.u = 0.f;
					tv.v = 0.f;
				}
				else
				{
					tv.u = 0.f;
					tv.v = 1.f;
				}
				PushVertex(tv, pVB);

				pQL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_uiAngleVertexIndices[BTNode::VP_LEFT] = m_CurrentNumOfVertices - 1;
				pQL->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_uiAngleVertexIndices[BTNode::VP_RIGHT] = m_CurrentNumOfVertices - 1;
			}

			//SetQuadLeafTexCoords(pVB, pQL, bRightEdge, bBottomEdge);
		}
		/**********/
		UnlockVertexBuffer();
		//m_TimeWhileCreateOutlineVertices = g_GetDateTimeSystem()->GetTimeValueMS() - m_TimeWhileCreateOutlineVertices;


		m_NumOfVisibleFieldNodes = m_VisibleFieldNodes.size();
		m_NumOfVisibleQuadLeaves = m_VisibleQuadLeaves.size();
		m_NumOfVertices = m_CurrentNumOfVertices;


		return true;
	}

	return false;  // No visible FieldNode
}

bool TerrainTree::CreateBTTree(const Maths::Camera& camera)  // WARN: this method is called very frequently
{
	m_NumOfSubBTNodes = 0;


	// Create outline
	if(!GenerateOutline(camera))
	{
		return false;
	}
	unsigned int NumOfVisibleQuadLeaves = m_VisibleQuadLeaves.size();


	unsigned int i = 0;

	// Reset touched QuadLeaves
	m_TimeWhileHandleTouchedQuadLeaves = g_GetDateTimeSystem()->GetTimeValueMS();
	if(!m_TouchedQuadLeaves.empty())
	{
		std::map<QuadLeaf*, QuadLeaf*>::iterator it, ibegin, iend;
		ibegin = m_TouchedQuadLeaves.begin();
		iend = m_TouchedQuadLeaves.end();
		for(it = ibegin; it != iend; ++it)
		{
			QuadLeaf* pQL = it->second;

			if(pQL)
			{
				pQL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_btnChilds[BTNode::CP_LEFT] = 0;
				pQL->GetBTNodeRoot(QuadNode::BTP_LEFT)->m_btnChilds[BTNode::CP_RIGHT] = 0;
				pQL->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_btnChilds[BTNode::CP_LEFT] = 0;
				pQL->GetBTNodeRoot(QuadNode::BTP_RIGHT)->m_btnChilds[BTNode::CP_RIGHT] = 0;
			}
		}
		m_TouchedQuadLeaves.clear();
	}
	// Insert touched QuadLeaves
	for(i = 0; i < NumOfVisibleQuadLeaves; ++i)
	{
		m_TouchedQuadLeaves.insert(std::map<QuadLeaf*, QuadLeaf*>::value_type(m_VisibleQuadLeaves[i], m_VisibleQuadLeaves[i]));
	}
	unsigned int NumOfDecorationQLs = m_vDecoration.size();
	for(i = 0; i < NumOfDecorationQLs; ++i)
	{
		m_TouchedQuadLeaves.insert(std::map<QuadLeaf*, QuadLeaf*>::value_type(m_vDecoration[i], m_vDecoration[i]));
	}
	m_TimeWhileHandleTouchedQuadLeaves = g_GetDateTimeSystem()->GetTimeValueMS() - m_TimeWhileHandleTouchedQuadLeaves;

	// Set texture coordinates for each visible QuadLeaves
	for(i = 0; i < NumOfVisibleQuadLeaves; ++i)
	{
	}

	m_TimeWhileCreateSubBTNodes = g_GetDateTimeSystem()->GetTimeValueMS();
	// Create BT-Tree
	m_CurrentBTPos = 0;
	if(!m_bDrawOutlineOnly)
	{
		void* pVB = LockVertexBuffer();
		for(i = 0; i < NumOfVisibleQuadLeaves; ++i)
		{
			QuadLeaf* pQL = m_VisibleQuadLeaves[i];
			if(!pQL)
			{
				// Push error-code into error-pipeline
				return false;
			}

			pQL->CreateBTTreeSublevels(CalculateLevelOfDetail(camera, pQL->m_Center), m_TTDesc, pVB, m_TouchedQuadLeaves);
		}
		for(i = 0; i < NumOfDecorationQLs; ++i)
		{
			QuadLeaf* pQL = m_vDecoration[i];
			if(!pQL)
			{
				// Push error-code into error-pipeline
				return false;
			}

			pQL->CreateBTTreeSublevels(CalculateLevelOfDetail(camera, pQL->m_Center), m_TTDesc, pVB, m_TouchedQuadLeaves);
		}
		UnlockVertexBuffer();
	}
	m_TimeWhileCreateSubBTNodes = g_GetDateTimeSystem()->GetTimeValueMS() - m_TimeWhileCreateSubBTNodes;

	m_TimeWhileExtractIndices = g_GetDateTimeSystem()->GetTimeValueMS();
	// Extract out the indices from QuadLeaves and BTNodes
	void* pIB = m_TerrainGroup.LockIndexBuffer();
	for(i = 0; i < NumOfVisibleQuadLeaves; ++i)
	{
		QuadLeaf* pQL = m_VisibleQuadLeaves[i];
		pQL->GetValidIndices(m_TerrainGroup, pIB);
		pQL->m_bVisible = false;  // reset this flag for next loop
	}
	for(i = 0; i < NumOfDecorationQLs; ++i)
	{
		QuadLeaf* pQL = m_vDecoration[i];
		pQL->GetValidIndices(m_TerrainGroup, pIB);
		pQL->m_bVisible = false;  // reset this flag for next loop
	}
	m_TerrainGroup.UnlockIndexBuffer();
	m_TerrainGroup.SetNumOfVertices(m_CurrentNumOfVertices);
	m_TimeWhileExtractIndices = g_GetDateTimeSystem()->GetTimeValueMS() - m_TimeWhileExtractIndices;

	m_NumOfSubBTNodes = m_CurrentBTPos;
	m_NumOfPrimitives = m_TerrainGroup.GetNumOfPrimitives();

	return true;
}

BTNode* TerrainTree::GetBTNodeFromPool(BTNode* btnParent)
{
	if(IsBTPoolFull())
	{
		throw Common::Exception(Common::EL_BAD, "TerrainTree::GetBTNodeFromPool", "The BTPool is full!");
		return 0;  // This indicates that the recursive will return from now
	}

	BTNode* pBT = m_BTs + m_CurrentBTPos;

	// Initialize the BTNode
	pBT->m_qlParent = 0;
	pBT->m_btnParent = btnParent;  // The two towers :)

	unsigned int i = 0;
	for(; i < BTNode::MAX_NEIGHBOURS; ++i)  // Neighbours
	{
		pBT->m_btnNeighbours[i] = 0;
	}
	for(i = 0; i < BTNode::MAX_CHILDS; ++i)  // Childs
	{
		pBT->m_btnChilds[i] = 0;
	}
	for(i = 0; i < BTNode::MAX_VERTICES; ++i)  // Indices
	{
		pBT->m_uiAngleVertexIndices[i] = 0xFFFFFFFF;
	}

	++m_CurrentBTPos;
	return pBT;
}

QuadLeaf* TerrainTree::GetLocatedQuadLeaf(const Maths::Vector3F& point) const
{
	bool bFound = false;
	QuadNode* pRet = m_Root;

	if(pRet->IfContain(point))
	{
		while(!pRet->IsLeaf())
		{
			for(unsigned int i = 0; i < QuadNode::MAX_CHILDS; ++i)
			{
				QuadNode* pChild = pRet->GetChild(i);
				if(pChild && pChild->IfContain(point))
				{
					pRet = pChild;

					if(pRet->IsLeaf())
					{
						return static_cast<QuadLeaf*>(pRet);
					}

					break;
				}
			}
		}
	}

	return 0;
}

float TerrainTree::GetHeightValue(float& x, float& z)
{
	unsigned int HalfWidthOfHM = m_HeightMap.GetWidth() / 2;
	unsigned int HalfHeightOfHM = m_HeightMap.GetHeight() / 2;
	float XRatio = GetTerrainWidth() / m_HeightMap.GetWidth();
	float ZRatio = GetTerrainHeight() / m_HeightMap.GetHeight();

	if(!m_HeightMap.IsLoaded())
	{
		m_HeightMap.ReadFile();
	}

	unsigned int Row = Maths::Round(HalfHeightOfHM + (m_CenterZ - z)/ZRatio);
	unsigned int Col = Maths::Round(HalfWidthOfHM + (x - m_CenterX)/XRatio);
	return m_HeightMap.GetHeightValue(Row, Col) * m_TTDesc.m_Factor;
}

bool TerrainTree::GetTerrainVertex(void* pVB, unsigned int VertexIndex, TerrainVertex& tv) const
{
	if(!pVB)
	{
		// Push error-code into error-pipeline
		return false;
	}

	if(VertexIndex >= m_CurrentNumOfVertices)  // Vertex index beyond the mark
	{
		return false;
	}

	tv = *((TerrainVertex*)pVB + VertexIndex);

	return true;
}

bool TerrainTree::GetTerrainVertex(void* pVB, unsigned int VertexIndex, TerrainVertex*& ptv) const
{
	if(!pVB)
	{
		// Push error-code into error-pipeline
		return false;
	}

	if(VertexIndex >= m_CurrentNumOfVertices)  // Vertex index beyond the mark
	{
		return false;
	}

	ptv = (TerrainVertex*)pVB + VertexIndex;

	return true;
}

void* TerrainTree::LockVertexBuffer()
{
	if(!m_Stream.IsValid())
	{
		// Push error-code into error-pipeline
		return 0;
	}

	void* pRet = 0;
	IDirect3DVertexBuffer9* pVertexBuffer = m_Stream.m_pVertexBuffer;  //m_VertexBuffers[0];
	if(FAILED(pVertexBuffer->Lock(0, 0, &pRet, 0)))
	{
		// Push error-code into error-pipeline
		return 0;
	}

	return pRet;
}

void TerrainTree::PushVertex(const TerrainVertex& tv, void* pVB)
{
	if(!pVB)
	{
		// Push error-code into error-pipeline
		return;
	}
	if(IsVertexBufferFull())
	{
		// Push error-code into error-pipeline
		return;
	}

	memcpy((unsigned char*)pVB + m_CurrentNumOfVertices * sizeof(tv), (void*)&tv, sizeof(tv));
	++m_CurrentNumOfVertices;
}

void TerrainTree::UnlockVertexBuffer()
{
	if(!m_Stream.IsValid())
	{
		// Push error-code into error-pipeline
		return;
	}
	IDirect3DVertexBuffer9* pVertexBuffer = m_Stream.m_pVertexBuffer;  //m_VertexBuffers[0];
	if(FAILED(pVertexBuffer->Unlock()))
	{
		// Push error-code into error-pipeline
	}
}

void TerrainTree::Destroy()
{
	SafeRelease(m_Stream.m_pVertexBuffer);
}

void TerrainTree::Render()
{
	m_Stream.Apply();
	m_TerrainGroup.Render();
}

}  // namespace