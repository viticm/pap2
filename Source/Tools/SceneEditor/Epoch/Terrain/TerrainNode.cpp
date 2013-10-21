// File: TerrainNode.cpp
// Desc:

#include"..\Common\Common.h"
#include"TerrainNode.h"
#include"TerrainDLLFuncs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

namespace Terrain
{

static unsigned int g_NumOfQNs = 0;

// QuadNode implementation

QuadNode::QuadNode(const Maths::Vector3F& vCenter, float fWidth, float fHeight, QuadNode* pParent): AxisAlignedBoundingBox(vCenter, vCenter), m_Center(vCenter), m_qnParent(pParent)
{
	// Initialize the bounding box
	float HalfWidth = fWidth / 2;
	float HalfHeight = fHeight / 2;
	m_vMin.x -= HalfWidth;
	m_vMin.z -= HalfHeight;
	m_vMax.x += HalfWidth - 1;
	m_vMax.z += HalfHeight - 1;

	Maths::Vector3F vCorners[MAX_CHILDS];

	unsigned int i = 0;
	for(; i < MAX_CHILDS; ++i)
	{
		GetCorner(TranslateIntToCP(i), vCorners[i]);
	}

	for(i = 0; i < MAX_CHILDS; ++i)
	{
		UpdateAABB(vCorners[i]);
	}

	// Initialize the data structure of the node
	m_qnChilds = 0;
}

QuadNode& QuadNode::operator = (const QuadNode& qn)
{
	if(&qn != this)
	{
		AxisAlignedBoundingBox::operator = (qn);

		m_Center = qn.m_Center;

		m_qnParent = qn.m_qnParent;

		m_qnChilds = qn.m_qnChilds;
	}

	return *this;
}

QuadNode::~QuadNode()
{
	SafeDeleteArray(m_qnChilds);
}

void QuadNode::Init(const Maths::Vector3F& vCenter, float fWidth, float fHeight, QuadNode* pParent)
{
	m_vMin = vCenter;
	m_vMax = vCenter;
	m_Center = vCenter;
	m_qnParent = pParent;

	// Initialize the bounding box
	float HalfWidth = fWidth / 2;
	float HalfHeight = fHeight / 2;
	m_vMin.x -= HalfWidth;
	m_vMin.z -= HalfHeight;
	m_vMax.x += HalfWidth - 1;
	m_vMax.z += HalfHeight - 1;

	Maths::Vector3F vCorners[MAX_CHILDS];

	unsigned int i = 0;
	for(; i < MAX_CHILDS; ++i)
	{
		GetCorner(TranslateIntToCP(i), vCorners[i]);
	}

	for(i = 0; i < MAX_CHILDS; ++i)
	{
		UpdateAABB(vCorners[i]);
	}

	// Initialize the data structure of the node
	m_qnChilds = 0;
}

QuadNode* QuadNode::HandleGetNeighbour(QuadNode* pChild, NeighbourPos np) const
{
	ChildPos cp;

	unsigned int i = 0;
	bool bChild = false;
	for(i = 0; i < MAX_CHILDS; ++i)
	{
		QuadNode* pQN = GetChild(i);
		if(pQN == pChild)
		{
			bChild = true;
			cp = TranslateIntToCP(i);
			break;
		}
	}

	if(!bChild)
	{
		throw Common::Exception(Common::EL_BAD, "QuadNode::HandleGetNeighbour", "The parameter pChild is not child of this node!");
	}

	switch(cp)
	{
	case CP_TL:
		switch(np)
		{
		case NP_L:
			{
				QuadNode* pQN_L_SELF = GetNeighbour(NP_L);
				if(pQN_L_SELF)
				{
					return pQN_L_SELF->GetChild(CP_TR);
				}
				return 0;
			}
			break;

		case NP_T:
			{
				QuadNode* pQN_T_SELF = GetNeighbour(NP_T);
				if(pQN_T_SELF)
				{
					return pQN_T_SELF->GetChild(CP_BL);
				}
				return 0;
			}
			break;

		case NP_R:
			return GetChild(CP_TR);

		case NP_B:
			return GetChild(CP_BL);

		case NP_TL:
			{
				QuadNode* pQN_TL_SELF = GetNeighbour(NP_TL);
				if(pQN_TL_SELF)
				{
					return pQN_TL_SELF->GetChild(CP_BR);
				}
				return 0;
			}
			break;

		case NP_TR:
			{
				QuadNode* pQN_T_SELF = GetNeighbour(NP_T);
				if(pQN_T_SELF)
				{
					return pQN_T_SELF->GetChild(CP_BR);
				}
				return 0;
			}
			break;

		case NP_BR:
			return GetChild(CP_BR);

		case NP_BL:
			{
				QuadNode* pQN_L_SELF = GetNeighbour(NP_L);
				if(pQN_L_SELF)
				{
					return pQN_L_SELF->GetChild(CP_BR);
				}
				return 0;
			}
			break;
		}
		break;

	case CP_TR:
		switch(np)
		{
		case NP_L:
			return GetChild(CP_TL);

		case NP_T:
			{
				QuadNode* pQN_T_SELF = GetNeighbour(NP_T);
				if(pQN_T_SELF)
				{
					return pQN_T_SELF->GetChild(CP_BR);
				}
				return 0;
			}
			break;

		case NP_R:
			{
				QuadNode* pQN_R_SELF = GetNeighbour(NP_R);
				if(pQN_R_SELF)
				{
					return pQN_R_SELF->GetChild(CP_TL);
				}
				return 0;
			}
			break;

		case NP_B:
			return GetChild(CP_BR);

		case NP_TL:
			{
				QuadNode* pQN_T_SELF = GetNeighbour(NP_T);
				if(pQN_T_SELF)
				{
					return pQN_T_SELF->GetChild(CP_BL);
				}
				return 0;
			}
			break;

		case NP_TR:
			{
				QuadNode* pQN_TR_SELF = GetNeighbour(NP_TR);
				if(pQN_TR_SELF)
				{
					return pQN_TR_SELF->GetChild(CP_BL);
				}
				return 0;
			}
			break;

		case NP_BR:
			{
				QuadNode* pQN_R_SELF = GetNeighbour(NP_R);
				if(pQN_R_SELF)
				{
					return pQN_R_SELF->GetChild(CP_BL);
				}
				return 0;
			}
			break;

		case NP_BL:
			return GetChild(CP_BL);
		}
		break;

	case CP_BR:
		switch(np)
		{
		case NP_L:
			return GetChild(CP_BL);

		case NP_T:
			return GetChild(CP_TR);

		case NP_R:
			{
				QuadNode* pQN_R_SELF = GetNeighbour(NP_R);
				if(pQN_R_SELF)
				{
					return pQN_R_SELF->GetChild(CP_BL);
				}
				return 0;
			}
			break;

		case NP_B:
			{
				QuadNode* pQN_B_SELF = GetNeighbour(NP_B);
				if(pQN_B_SELF)
				{
					return pQN_B_SELF->GetChild(CP_TR);
				}
				return 0;
			}
			break;

		case NP_TL:
			return GetChild(CP_TL);

		case NP_TR:
			{
				QuadNode* pQN_R_SELF = GetNeighbour(NP_R);
				if(pQN_R_SELF)
				{
					return pQN_R_SELF->GetChild(CP_TL);
				}
				return 0;
			}
			break;

		case NP_BR:
			{
				QuadNode* pQN_BR_SELF = GetNeighbour(NP_BR);
				if(pQN_BR_SELF)
				{
					return pQN_BR_SELF->GetChild(CP_TL);
				}
				return 0;
			}
			break;

		case NP_BL:
			{
				QuadNode* pQN_B_SELF = GetNeighbour(NP_B);
				if(pQN_B_SELF)
				{
					return pQN_B_SELF->GetChild(CP_TL);
				}
				return 0;
			}
			break;
		}
		break;

	case CP_BL:
		switch(np)
		{
		case NP_L:
			{
				QuadNode* pQN_L_SELF = GetNeighbour(NP_L);
				if(pQN_L_SELF)
				{
					return pQN_L_SELF->GetChild(CP_BR);
				}
				return 0;
			}
			break;

		case NP_T:
			return GetChild(CP_TL);

		case NP_R:
			return GetChild(CP_BR);

		case NP_B:
			{
				QuadNode* pQN_B_SELF = GetNeighbour(NP_B);
				if(pQN_B_SELF)
				{
					return pQN_B_SELF->GetChild(CP_TL);
				}
				return 0;
			}
			break;

		case NP_TL:
			{
				QuadNode* pQN_L_SELF = GetNeighbour(NP_L);
				if(pQN_L_SELF)
				{
					return pQN_L_SELF->GetChild(CP_TR);
				}
				return 0;
			}
			break;

		case NP_TR:
			return GetChild(CP_TR);

		case NP_BR:
			{
				QuadNode* pQN_B_SELF = GetNeighbour(NP_B);
				if(pQN_B_SELF)
				{
					return pQN_B_SELF->GetChild(CP_TR);
				}
				return 0;
			}
			break;

		case NP_BL:
			{
				QuadNode* pQN_BL_SELF = GetNeighbour(NP_BL);
				if(pQN_BL_SELF)
				{
					return pQN_BL_SELF->GetChild(CP_TR);
				}
				return 0;
			}
			break;
		}
		break;

	default:
		return 0;
	}

	return 0;
}

bool QuadNode::GetCorner(ChildPos CornerID, Maths::Vector3F& Corner) const
{
	float HalfWidth = GetXLength() / 2;
	float HalfHeight = GetZLength() / 2;

	switch(CornerID)
	{
	case CP_TL:
		Corner.x = m_Center.x - HalfWidth;
		Corner.z = m_Center.z + HalfHeight;
		Corner.y = g_GetTerrainTree()->GetHeightValue(Corner.x, Corner.z);

		break;

	case CP_TR:
		Corner.x = m_Center.x + HalfWidth;
		Corner.z = m_Center.z + HalfHeight;
		Corner.y = g_GetTerrainTree()->GetHeightValue(Corner.x, Corner.z);

		break;

	case CP_BR:
		Corner.x = m_Center.x + HalfWidth;
		Corner.z = m_Center.z - HalfHeight;
		Corner.y = g_GetTerrainTree()->GetHeightValue(Corner.x, Corner.z);

		break;

	case CP_BL:
		Corner.x = m_Center.x - HalfWidth;
		Corner.z = m_Center.z - HalfHeight;
		Corner.y = g_GetTerrainTree()->GetHeightValue(Corner.x, Corner.z);

		break;

	default:
		return false;
	}

	return true;
}

bool QuadNode::GetCorner(ChildPos CornerID, TerrainVertex& tv) const
{
	float HalfWidth = GetXLength() / 2;
	float HalfHeight = GetZLength() / 2;

	switch(CornerID)
	{
	case CP_TL:
		tv.x = m_Center.x - HalfWidth;
		tv.z = m_Center.z + HalfHeight;
		tv.y = g_GetTerrainTree()->GetHeightValue(tv.x, tv.z);

		break;

	case CP_TR:
		tv.x = m_Center.x + HalfWidth;
		tv.z = m_Center.z + HalfHeight;
		tv.y = g_GetTerrainTree()->GetHeightValue(tv.x, tv.z);

		break;

	case CP_BR:
		tv.x = m_Center.x + HalfWidth;
		tv.z = m_Center.z - HalfHeight;
		tv.y = g_GetTerrainTree()->GetHeightValue(tv.x, tv.z);

		break;

	case CP_BL:
		tv.x = m_Center.x - HalfWidth;
		tv.z = m_Center.z - HalfHeight;
		tv.y = g_GetTerrainTree()->GetHeightValue(tv.x, tv.z);

		break;

	default:
		return false;
	}

	return true;
}

void QuadNode::UpdateAABB(const Maths::Vector3F& aabb)
{
	bool bUpdated = false;

	if(m_vMin.x > aabb.x)
	{
		m_vMin.x = aabb.x;
		bUpdated = true;
	}
	if(m_vMin.y > aabb.y)
	{
		m_vMin.y = aabb.y;
		bUpdated = true;
	}
	if(m_vMin.z > aabb.z)
	{
		m_vMin.z = aabb.z;
		bUpdated = true;
	}

	if(m_vMax.x < aabb.x)
	{
		m_vMax.x = aabb.x;
		bUpdated = true;
	}
	if(m_vMax.y < aabb.y)
	{
		m_vMax.y = aabb.y;
		bUpdated = true;
	}
	if(m_vMax.z < aabb.z)
	{
		m_vMax.z = aabb.z;
		bUpdated = true;
	}

	if(m_qnParent && bUpdated)
	{
		m_qnParent->UpdateAABB(aabb);
	}
}

void QuadNode::UpdateAABB_Y(float y)
{
	bool bUpdated = false;

	if(m_vMin.y > y)
	{
		m_vMin.y = y;
		bUpdated = true;
	}
	else if(m_vMax.y < y)
	{
		m_vMax.y = y;
		bUpdated = true;
	}

	if(m_qnParent && bUpdated)
	{
		m_qnParent->UpdateAABB_Y(y);
	}
}

//unsigned int QuadNode::GetNumOfQuadLeaves()
//{
//	return g_NumOfQNs;
//}

bool QuadNode::GetAllFieldNodes(vector<FieldNode*>& vVisibleFieldNodes) const
{
	bool bRet = false;

	if(IsFieldNode())  // We are in a FieldNode
	{
		vVisibleFieldNodes.push_back((FieldNode*)this);
		bRet = true;
	}
	else
	{
		for(unsigned int i = 0; i < MAX_CHILDS; ++i)
		{
			QuadNode* pQN = GetChild(i);
			if(pQN)
			{
				if(pQN->GetAllFieldNodes(vVisibleFieldNodes))
				{
					bRet = true;
				}
			}
		}
	}

	return bRet;
}

bool QuadNode::FindVisibleFieldNodes(const Maths::Camera& camera, vector<FieldNode*>& vVisibleFieldNodes) const
{
	bool bRet = false;

	Maths::RelativeLocation rl = TestQuadNodeVSCamera(camera);
	switch(rl)
	{
	case Maths::RL_BACK:  // 结点完全位于照相机的ViewFrustum以外，所以完全看不见
		break;

	case Maths::RL_FRONT:  // 结点完全位于照相机的ViewFrustum以内，所以完全看得见
		GetAllFieldNodes(vVisibleFieldNodes);
		bRet = true;
		break;

	case Maths::RL_SPLIT:  // 结点部分位于照相机的ViewFrustum以内
		if(IsFieldNode())
		{
			vVisibleFieldNodes.push_back((FieldNode*)this);
			bRet = true;
		}
		else
		{
			for(unsigned int i = 0; i < MAX_CHILDS; ++i)
			{
				QuadNode* pQN = GetChild(i);
				if(pQN)
				{
					if(pQN->FindVisibleFieldNodes(camera, vVisibleFieldNodes))
					{
						bRet = true;
					}
				}
			}
		}
		break;

	default:
		break;
	}

	return bRet;
}

bool QuadNode::GetAllQuadLeaves(vector<QuadLeaf*>& vQuadLeaves, bool bForVisible) const
{
	bool bRet = false;

	if(IsLeaf())  // We are in a QuadLeaf
	{
		QuadLeaf* pQL = (QuadLeaf*)this;

		if(bForVisible)
		{
			pQL->m_bVisible = true;
		}
		vQuadLeaves.push_back(pQL);

		bRet = true;
	}
	else
	{
		for(unsigned int i = 0; i < MAX_CHILDS; ++i)
		{
			QuadNode* pQN = GetChild(i);
			if(pQN)
			{
				if(pQN->GetAllQuadLeaves(vQuadLeaves, bForVisible))
				{
					bRet = true;
				}
			}
		}
	}

	return bRet;
}

bool QuadNode::FindVisibleQuadLeaves(const Maths::Camera& camera, vector<QuadLeaf*>& vVisibleQuadLeaves) const
{
	bool bRet = false;

	Maths::RelativeLocation rl = TestQuadNodeVSCamera(camera);
	switch(rl)
	{
	case Maths::RL_BACK:  // 结点完全位于照相机的ViewFrustum以外，所以完全看不见
		break;

	case Maths::RL_FRONT:  // 结点完全位于照相机的ViewFrustum以内，所以完全看得见
		GetAllQuadLeaves(vVisibleQuadLeaves);
		bRet = true;
		break;

	case Maths::RL_SPLIT:  // 结点部分位于照相机的ViewFrustum以内
		if(IsLeaf())
		{
			QuadLeaf* pQL = (QuadLeaf*)this;
			pQL->m_bVisible = true;
			vVisibleQuadLeaves.push_back(pQL);
			bRet = true;
		}
		else
		{
			for(unsigned int i = 0; i < MAX_CHILDS; ++i)
			{
				QuadNode* pQN = GetChild(i);
				if(pQN)
				{
					if(pQN->FindVisibleQuadLeaves(camera, vVisibleQuadLeaves))
					{
						bRet = true;
					}
				}
			}
		}
		break;

	default:
		break;
	}

	return bRet;
}

Maths::RelativeLocation QuadNode::TestQuadNodeVSCamera(const Maths::Camera& camera) const
{
	Maths::Sphere NodeSphere;
	NodeSphere.m_Location = (m_vMin + m_vMax) * 0.5f;
	float RadiusSquare = ((m_vMax - m_vMin) * 0.5f).Square();

	Maths::RelativeLocation rlSphereVSSphere = camera.SphereTestSphere(NodeSphere.m_Location, RadiusSquare);  // 使用照相机的球测试结点的球
	switch(rlSphereVSSphere)
	{
	case Maths::RL_BACK:  // 结点的球完全位于照相机的球以外，所以完全看不见
		return Maths::RL_BACK;

	case Maths::RL_FRONT:  // 结点的球完全位于照相机的球以内 (但照相机的球比照相机的视域大得多)
	case Maths::RL_SPLIT:  // 结点的球部分位于照相机的球以内
		{
			// 首先测试结点的球是否位于照相机的视锥内
			NodeSphere.m_Radius = sqrtf(RadiusSquare);
			if(!camera.m_ViewCone.ConeIntersectSphere(NodeSphere))
			{
				return Maths::RL_BACK;  // 结点的球位于照相机的视锥以外
			}

			Maths::RelativeLocation rlVFVSSphere = camera.ViewFrustumTestSphere(NodeSphere);  // 使用照相机的ViewFrustum测试结点的球
			switch(rlVFVSSphere)
			{
			case Maths::RL_BACK:  // 结点的球完全位于照相机的ViewFrustum以外，所以完全看不见
				return Maths::RL_BACK;

			case Maths::RL_FRONT:  // 结点的球完全位于照相机的ViewFrustum以内，所以完全看得见
				return Maths::RL_FRONT;

			case Maths::RL_SPLIT:  // 结点的球部分位于照相机的ViewFrustum以内
				{
					Maths::RelativeLocation rlVFVSAABB = camera.TestAxisAlignedBoundingBox(*this);  // 使用照相机的ViewFrustum测试结点的AABB
					switch(rlVFVSAABB)
					{
					case Maths::RL_BACK:  // 结点的AABB完全位于照相机的ViewFrustum以外，所以完全看不见
						return Maths::RL_BACK;

					case Maths::RL_FRONT:  // 结点的AABB完全位于照相机的ViewFrustum以内，所以完全看得见
						return Maths::RL_FRONT;

					case Maths::RL_SPLIT:  // 结点的AABB部分位于照相机的ViewFrustum以内
						return Maths::RL_SPLIT;
					}
				}
				break;
			}
		}
		break;
	}

	return Maths::RL_BACK;
}

// FieldNode implementation

FieldNode& FieldNode::operator = (const FieldNode& fn)
{
	if(&fn != this)
	{
		QuadNode::operator = (fn);
	}

	return *this;
}

// QuadLeaf implementation

QuadLeaf::QuadLeaf(const Maths::Vector3F& vCenter, float fWidth, float fHeight, QuadNode* pParent): QuadNode(vCenter, fWidth, fHeight, pParent)
{
	m_btnRoots = new BTNode[MAX_BTNODEROOTS];

	for(unsigned int i = 0; i < MAX_BTNODEROOTS; ++i)
	{
		BTNode* pBT = GetBTNodeRoot(i);
		pBT->m_qlParent = this;
	}

	m_bVisible = false;

	m_TL_U = 0;
	m_TL_V = 0;
}

QuadLeaf& QuadLeaf::operator = (const QuadLeaf& ql)
{
	if(&ql != this)
	{
		QuadNode::operator = (ql);

		m_btnRoots = ql.m_btnRoots;
		m_bVisible = ql.m_bVisible;

		m_TL_U = ql.m_TL_U;
		m_TL_V = ql.m_TL_V;
	}

	return *this;
}

QuadLeaf::~QuadLeaf()
{
	SafeDeleteArray(m_btnRoots);
}

void QuadLeaf::Init(const Maths::Vector3F& vCenter, float fWidth, float fHeight, QuadNode* pParent)
{
	QuadNode::Init(vCenter, fWidth, fHeight, pParent);

	m_btnRoots = new BTNode[MAX_BTNODEROOTS];

	for(unsigned int i = 0; i < MAX_BTNODEROOTS; ++i)
	{
		BTNode* pBT = GetBTNodeRoot(i);
		pBT->m_qlParent = this;
	}

	m_bVisible = false;

	m_TL_U = 0;
	m_TL_V = 0;
}

BTNode* QuadLeaf::GetBTNodeRoot(unsigned int i) const
{
	if(i >= MAX_BTNODEROOTS)
	{
		throw Common::Exception(Common::EL_BAD, "BTNode::GetBTNodeRoot", "Index is out of range!");
	}

	return m_btnRoots + i;
}

void QuadLeaf::CreateBTTreeSublevels(const unsigned int LOD, const TerrainTreeDesc& ttdesc, void* pVB, map<QuadLeaf*, QuadLeaf*>& TouchedQuadLeaves)
{
	if(!pVB)
	{
		// Push error-code into error-pipeline
		return;
	}
	if(ttdesc.m_Variant < 0)
	{
		// Push error-code into error-pipeline
		return;
	}

	for(unsigned int i = 0; i < MAX_BTNODEROOTS; ++i)
	{
		BTNode* pBT = GetBTNodeRoot(i);
		pBT->CreateChilds(LOD, ttdesc, 0, pVB, TouchedQuadLeaves);
	}
}

void QuadLeaf::GetValidIndices(TerrainGroup& tg, void* pIB) const
{
	for(unsigned int i = 0; i < MAX_BTNODEROOTS; ++i)
	{
		BTNode* pBT = GetBTNodeRoot(i);
		if(pBT)
		{
			if(tg.IsIndexBufferFull())
			{
				return;
			}
			pBT->GetValidIndices(tg, pIB);
		}
	}
}

void QuadLeaf::InvalidIndices()
{
	for(unsigned int i = 0; i < MAX_BTNODEROOTS; ++i)
	{
		BTNode* pBTN = GetBTNodeRoot(i);
		if(pBTN)
		{
			memset(pBTN->m_uiAngleVertexIndices, 0xFFFFFFFF, sizeof(pBTN->m_uiAngleVertexIndices));
		}
	}
}

// BTNode implementation

BTNode::BTNode(): m_qlParent(0), m_btnParent(0)
{
	unsigned int i = 0;
	for(; i < MAX_NEIGHBOURS; ++i)
	{
		m_btnNeighbours[i] = 0;
	}
	for(i = 0; i < MAX_CHILDS; ++i)
	{
		m_btnChilds[i] = 0;
	}
	for(i = 0; i < MAX_VERTICES; ++i)
	{
		m_uiAngleVertexIndices[i] = 0xFFFFFFFF;  // We use the most biggest unsigned integer to indicate that the index is not valid yet.
	}
}

BTNode::BTNode(QuadLeaf* bnParent): m_qlParent(bnParent), m_btnParent(0)
{
	unsigned int i = 0;
	for(; i < MAX_NEIGHBOURS; ++i)
	{
		m_btnNeighbours[i] = 0;
	}
	for(i = 0; i < MAX_CHILDS; ++i)
	{
		m_btnChilds[i] = 0;
	}
	for(i = 0; i < MAX_VERTICES; ++i)
	{
		m_uiAngleVertexIndices[i] = 0xFFFFFFFF;  // We use the most biggest unsigned integer to indicate that the index is not valid yet.
	}
}

BTNode::BTNode(BTNode* btnParent): m_qlParent(0), m_btnParent(btnParent)
{
	unsigned int i = 0;
	for(; i < MAX_NEIGHBOURS; ++i)
	{
		m_btnNeighbours[i] = 0;
	}
	for(i = 0; i < MAX_CHILDS; ++i)
	{
		m_btnChilds[i] = 0;
	}
	for(i = 0; i < MAX_VERTICES; ++i)
	{
		m_uiAngleVertexIndices[i] = 0xFFFFFFFF;  // We use the most biggest unsigned integer to indicate that the index is not valid yet.
	}
}

BTNode::BTNode(const BTNode& btn): m_qlParent(btn.m_qlParent), m_btnParent(btn.m_btnParent)
{
	unsigned int i = 0;
	for(; i < MAX_NEIGHBOURS; ++i)
	{
		m_btnNeighbours[i] = btn.m_btnNeighbours[i];
	}
	for(i = 0; i < MAX_CHILDS; ++i)
	{
		m_btnChilds[i] = btn.m_btnChilds[i];
	}
	for(i = 0; i < MAX_VERTICES; ++i)
	{
		m_uiAngleVertexIndices[i] = btn.m_uiAngleVertexIndices[i];
	}
}

BTNode& BTNode::operator = (const BTNode& btn)
{
	if(&btn != this)
	{
		m_qlParent = btn.m_qlParent;

		m_btnParent = btn.m_btnParent;

		unsigned int i = 0;
		for(; i < MAX_NEIGHBOURS; ++i)
		{
			m_btnNeighbours[i] = btn.m_btnNeighbours[i];
		}
		for(i = 0; i < MAX_CHILDS; ++i)
		{
			m_btnChilds[i] = btn.m_btnChilds[i];
		}
		for(i = 0; i < MAX_VERTICES; ++i)
		{
			m_uiAngleVertexIndices[i] = btn.m_uiAngleVertexIndices[i];
		}
	}

	return *this;
}

bool BTNode::CreateChilds(const unsigned int LOD, const TerrainTreeDesc& ttdesc, const unsigned int Level, void* pVB, map<QuadLeaf*, QuadLeaf*>& TouchedQuadLeaves, bool bCalledForDiamond)
{
	if(!pVB)
	{
		// Push error-code into error-pipeline
		return false;
	}

	bool bSplited = false;
	unsigned int i = 0;
	for(; i < MAX_CHILDS; ++i)
	{
		if(m_btnChilds[i])
		{
			bSplited = true;

			if(!m_btnChilds[i]->CreateChilds(LOD, ttdesc, Level + 1, pVB, TouchedQuadLeaves))
			{
				return false;  // This indicates that the recursive function will return from now
			}
		}
	}
	if(bSplited)
	{
		return true;
	}

	if(Level >= LOD)
	{
		return true;
	}

	unsigned int IndexOfTVC = 0xFFFFFFFF;

	if(m_btnNeighbours[NP_BOTTOM])  // The bottom neighbour MUST be present. If not, it must be caused by the expire of the BTNodes' pool. In that case, it's no way to continue the recursive and must return at once.
	{
		if(m_btnNeighbours[NP_BOTTOM]->m_btnNeighbours[NP_BOTTOM] != this)  // We are not located in a diamond. Make it to be a diamond.
		{
			m_btnNeighbours[NP_BOTTOM]->CreateChilds(LOD, ttdesc, Level - 1, pVB, TouchedQuadLeaves, true);
		}

		for(i = 0; i < MAX_CHILDS; ++i)
		{
			if(m_btnNeighbours[NP_BOTTOM]->m_btnChilds[i])  // TVC已经存在
			{
				IndexOfTVC = m_btnNeighbours[NP_BOTTOM]->m_btnChilds[i]->m_uiAngleVertexIndices[VP_TOP];
				break;
			}
		}

		if(IndexOfTVC == 0xFFFFFFFF)
		{
			TerrainVertex tv_l, tv_r, tv_c;
			g_GetTerrainTree()->GetTerrainVertex(pVB, m_uiAngleVertexIndices[VP_LEFT], tv_l);
			g_GetTerrainTree()->GetTerrainVertex(pVB, m_uiAngleVertexIndices[VP_RIGHT], tv_r);
			tv_c.x = (tv_l.x + tv_r.x) / 2;
			tv_c.y = (tv_l.y + tv_r.y) / 2;
			tv_c.z = (tv_l.z + tv_r.z) / 2;
			float RealHeight = g_GetTerrainTree()->GetHeightValue(tv_c.x, tv_c.z);
			if(!bCalledForDiamond)
			{
				if(Maths::FastFabs(tv_c.y - RealHeight) < ttdesc.m_Variant)  // 地形高度已经足够精确，不必划分
				{
					return true;
				}
			}

			tv_c.y = RealHeight;  // 修正高度
			if(m_qlParent)
			{
				m_qlParent->UpdateAABB_Y(RealHeight);
			}

#ifdef EPOCH_DEBUG
			switch(Level)
			{
			case 0:
				tv_c.Diffuse = D3DCOLOR_XRGB(255, 0, 0);
				break;
			case 1:
				tv_c.Diffuse = D3DCOLOR_XRGB(0, 255, 0);
				break;
			case 2:
				tv_c.Diffuse = D3DCOLOR_XRGB(0, 0, 255);
				break;
			case 3:
				tv_c.Diffuse = D3DCOLOR_XRGB(255, 255, 0);
				break;
			case 4:
				tv_c.Diffuse = D3DCOLOR_XRGB(255, 0, 255);
				break;
			case 5:
				tv_c.Diffuse = D3DCOLOR_XRGB(0, 255, 255);
				break;
			case 6:
				tv_c.Diffuse = 0x3A6EA5;  // Win2K桌面蓝
				break;
			case 7:
				tv_c.Diffuse = 0x685060;  //  魔兽世界远山灰
				break;
			default:
				tv_c.Diffuse = 0x685060;
			}
#else
			tv_c.Diffuse = ttdesc.m_TerrainBaseColor;
#endif

			/*
			tv_c.u = (tv_l.u + tv_r.u) * 0.5;
			tv_c.v = (tv_l.v + tv_r.v) * 0.5;  // although this is the fastest method to  calculate the texture coordinates, it can result in an uncomfortable stretch.
			*/
			float dl = (Maths::Vector3F(tv_c.x, tv_c.y, tv_c.z) - Maths::Vector3F(tv_l.x, tv_l.y, tv_l.z)).Length();
			float dr = (Maths::Vector3F(tv_c.x, tv_c.y, tv_c.z) - Maths::Vector3F(tv_r.x, tv_r.y, tv_r.z)).Length();
			float ratio_r = dl / (dl + dr);  // Note here
			float ratio_l = 1.f - ratio_r;
			tv_c.u = tv_l.u * ratio_l + tv_r.u * ratio_r;
			tv_c.v = tv_l.v * ratio_l + tv_r.v * ratio_r;

			g_GetTerrainTree()->PushVertex(tv_c, pVB);  // Push the new generated vertex into the vertex buffer

			IndexOfTVC = g_GetTerrainTree()->GetCurrentNumOfVertices() - 1;  // Get the index of the new generated vertex
		}
	}
	else
	{
		return false;
	}

	// OK now, we have got a valid index for the vertex of the center point on the bias

	if(!g_GetTerrainTree()->IsBTPoolFull())
	{
		m_btnChilds[CP_LEFT] = g_GetTerrainTree()->GetBTNodeFromPool(this);  // The left child
		// Set the qlParent
		m_btnChilds[CP_LEFT]->m_qlParent = m_qlParent;
		// Set the indices
		m_btnChilds[CP_LEFT]->m_uiAngleVertexIndices[VP_TOP] = IndexOfTVC;
		m_btnChilds[CP_LEFT]->m_uiAngleVertexIndices[VP_LEFT] = m_uiAngleVertexIndices[VP_TOP];
		m_btnChilds[CP_LEFT]->m_uiAngleVertexIndices[VP_RIGHT] = m_uiAngleVertexIndices[VP_LEFT];
	}
	if(!g_GetTerrainTree()->IsBTPoolFull())
	{
		m_btnChilds[CP_RIGHT] = g_GetTerrainTree()->GetBTNodeFromPool(this);  // The right child
		// Set the qlParent
		m_btnChilds[CP_RIGHT]->m_qlParent = m_qlParent;
		// Set the indices
		m_btnChilds[CP_RIGHT]->m_uiAngleVertexIndices[VP_TOP] = IndexOfTVC;
		m_btnChilds[CP_RIGHT]->m_uiAngleVertexIndices[VP_LEFT] = m_uiAngleVertexIndices[VP_RIGHT];
		m_btnChilds[CP_RIGHT]->m_uiAngleVertexIndices[VP_RIGHT] = m_uiAngleVertexIndices[VP_TOP];
	}
	if(!g_GetTerrainTree()->IsBTPoolFull())
	{
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_LEFT] = g_GetTerrainTree()->GetBTNodeFromPool(m_btnNeighbours[NP_BOTTOM]);  // The left child of the bottom neighbour
		// Set the qlParent
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_LEFT]->m_qlParent = m_btnNeighbours[NP_BOTTOM]->m_qlParent;
		// Set the indices
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_LEFT]->m_uiAngleVertexIndices[VP_TOP] = IndexOfTVC;
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_LEFT]->m_uiAngleVertexIndices[VP_LEFT] = m_btnNeighbours[NP_BOTTOM]->m_uiAngleVertexIndices[VP_TOP];
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_LEFT]->m_uiAngleVertexIndices[VP_RIGHT] = m_btnNeighbours[NP_BOTTOM]->m_uiAngleVertexIndices[VP_LEFT];
	}
	if(!g_GetTerrainTree()->IsBTPoolFull())
	{
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_RIGHT] = g_GetTerrainTree()->GetBTNodeFromPool(m_btnNeighbours[NP_BOTTOM]);  // The right child
		// Set the qlParent
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_RIGHT]->m_qlParent = m_btnNeighbours[NP_BOTTOM]->m_qlParent;
		// Set the indices
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_RIGHT]->m_uiAngleVertexIndices[VP_TOP] = IndexOfTVC;
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_RIGHT]->m_uiAngleVertexIndices[VP_LEFT] = m_btnNeighbours[NP_BOTTOM]->m_uiAngleVertexIndices[VP_RIGHT];
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_RIGHT]->m_uiAngleVertexIndices[VP_RIGHT] = m_btnNeighbours[NP_BOTTOM]->m_uiAngleVertexIndices[VP_TOP];
	}

	if(m_btnChilds[CP_LEFT])
	{
		m_btnChilds[CP_LEFT]->SetNeighbours(CP_LEFT);
	}
	if(m_btnChilds[CP_RIGHT])
	{
		m_btnChilds[CP_RIGHT]->SetNeighbours(CP_RIGHT);
	}
	if(m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_LEFT])
	{
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_LEFT]->SetNeighbours(CP_LEFT);
	}
	if(m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_RIGHT])
	{
		m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_RIGHT]->SetNeighbours(CP_RIGHT);
	}

	TouchedQuadLeaves.insert(std::map<QuadLeaf*, QuadLeaf*>::value_type(m_qlParent, m_qlParent));
	if(m_btnNeighbours[NP_BOTTOM])
	{
		QuadLeaf* pQL = m_btnNeighbours[NP_BOTTOM]->m_qlParent;
		TouchedQuadLeaves.insert(std::map<QuadLeaf*, QuadLeaf*>::value_type(pQL, pQL));
	}

	if(!bCalledForDiamond)
	{
		for(i = 0; i < MAX_CHILDS; ++i)
		{
			if(m_btnChilds[i] && !g_GetTerrainTree()->IsBTPoolFull())
			{
				if(!m_btnChilds[i]->CreateChilds(LOD, ttdesc, Level + 1, pVB, TouchedQuadLeaves))
				{
					return false;
				}
			}
			/*
			if(m_btnNeighbours[NP_BOTTOM]->m_btnChilds[i] && !g_GetTerrainTree()->IsBTPoolFull())
			{
				if(!m_btnNeighbours[NP_BOTTOM]->m_btnChilds[i]->CreateChilds(LOD, Variant, Level + 1, pTT, pVB))
				{
					return false;
				}
			}
			*/
		}

		if(g_GetTerrainTree()->IsBTPoolFull())
		{
			return false;  // This indicates that the recursive function will return from now
		}
	}

	return true;
}

void BTNode::SetNeighbours(ChildPos cp)
{
	if(!m_qlParent && !m_btnParent)
	{
		return;
	}

	switch(cp)
	{
	case CP_LEFT:
		if(m_btnParent)
		{
			m_btnNeighbours[NP_LEFT] = m_btnParent->m_btnChilds[CP_RIGHT];
			if(m_btnNeighbours[NP_LEFT])
			{
				m_btnNeighbours[NP_LEFT]->m_btnNeighbours[NP_RIGHT] = this;
			}

			if(m_btnParent->m_btnNeighbours[NP_BOTTOM])
			{
				m_btnNeighbours[NP_RIGHT] = m_btnParent->m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_RIGHT];
				if(m_btnNeighbours[NP_RIGHT])
				{
					m_btnNeighbours[NP_RIGHT]->m_btnNeighbours[NP_LEFT] = this;
				}
			}

			if(m_btnParent->m_btnNeighbours[NP_LEFT])
			{
				BTNode* pParNei = m_btnParent->m_btnNeighbours[NP_LEFT];
				if(m_btnParent == pParNei->m_btnNeighbours[NP_LEFT])
				{
					if(pParNei->m_btnChilds[CP_LEFT])
					{
						m_btnNeighbours[NP_BOTTOM] = pParNei->m_btnChilds[CP_LEFT];
						m_btnNeighbours[NP_BOTTOM]->m_btnNeighbours[NP_BOTTOM] = this;  // Diamond
					}
					else
					{
						m_btnNeighbours[NP_BOTTOM] = pParNei;
					}
				}
				else
				{
					if(pParNei->m_btnChilds[CP_RIGHT])
					{
						m_btnNeighbours[NP_BOTTOM] = pParNei->m_btnChilds[CP_RIGHT];
						m_btnNeighbours[NP_BOTTOM]->m_btnNeighbours[NP_BOTTOM] = this;  // Diamond
					}
					else
					{
						m_btnNeighbours[NP_BOTTOM] = pParNei;
					}
				}
			}
		}
		else if(m_qlParent)
		{
			QuadNode* pQNP_B = m_qlParent->GetNeighbour(QuadNode::NP_B);
			if(pQNP_B)
			{
				m_btnNeighbours[NP_LEFT] = static_cast<QuadLeaf*>(pQNP_B)->GetBTNodeRoot(QuadNode::BTP_RIGHT);
			}

			QuadNode* pQNP_L = m_qlParent->GetNeighbour(QuadNode::NP_L);
			if(pQNP_L)
			{
				m_btnNeighbours[NP_RIGHT] = static_cast<QuadLeaf*>(pQNP_L)->GetBTNodeRoot(QuadNode::BTP_RIGHT);
			}

			m_btnNeighbours[NP_BOTTOM] = m_qlParent->GetBTNodeRoot(QuadNode::BTP_RIGHT);
		}

		break;

	case CP_RIGHT:
		if(m_btnParent)
		{
			if(m_btnParent->m_btnNeighbours[NP_BOTTOM])
			{
				m_btnNeighbours[NP_LEFT] = m_btnParent->m_btnNeighbours[NP_BOTTOM]->m_btnChilds[CP_LEFT];
				if(m_btnNeighbours[NP_LEFT])
				{
					m_btnNeighbours[NP_LEFT]->m_btnNeighbours[NP_RIGHT] = this;
				}
			}

			m_btnNeighbours[NP_RIGHT] = m_btnParent->m_btnChilds[CP_LEFT];
			if(m_btnNeighbours[NP_RIGHT])
			{
				m_btnNeighbours[NP_RIGHT]->m_btnNeighbours[NP_LEFT] = this;
			}

			if(m_btnParent->m_btnNeighbours[NP_RIGHT])
			{
				BTNode* pParNei = m_btnParent->m_btnNeighbours[NP_RIGHT];
				if(m_btnParent == pParNei->m_btnNeighbours[NP_LEFT])
				{
					if(pParNei->m_btnChilds[CP_LEFT])
					{
						m_btnNeighbours[NP_BOTTOM] = pParNei->m_btnChilds[CP_LEFT];
						m_btnNeighbours[NP_BOTTOM]->m_btnNeighbours[NP_BOTTOM] = this;  // Diamond
					}
					else
					{
						m_btnNeighbours[NP_BOTTOM] = pParNei;
					}
				}
				else
				{
					if(pParNei->m_btnChilds[CP_RIGHT])
					{
						m_btnNeighbours[NP_BOTTOM] = pParNei->m_btnChilds[CP_RIGHT];
						m_btnNeighbours[NP_BOTTOM]->m_btnNeighbours[NP_BOTTOM] = this;  // Diamond
					}
					else
					{
						m_btnNeighbours[NP_BOTTOM] = pParNei;
					}
				}
			}
		}
		else if(m_qlParent)
		{
			QuadNode* pQNP_T = m_qlParent->GetNeighbour(QuadNode::NP_T);
			if(pQNP_T)
			{
				m_btnNeighbours[NP_LEFT] = static_cast<QuadLeaf*>(pQNP_T)->GetBTNodeRoot(QuadNode::BTP_LEFT);
			}

			QuadNode* pQNP_R = m_qlParent->GetNeighbour(QuadNode::NP_R);
			if(pQNP_R)
			{
				m_btnNeighbours[NP_RIGHT] = static_cast<QuadLeaf*>(pQNP_R)->GetBTNodeRoot(QuadNode::BTP_LEFT);
			}

			m_btnNeighbours[NP_BOTTOM] = m_qlParent->GetBTNodeRoot(QuadNode::BTP_LEFT);
		}

		break;

	default:
		throw Common::Exception(Common::EL_BAD, "BTNode::SetNeighbours", "Illegal parameter!");
	}
}

bool BTNode::IsVisible(const Maths::ViewFrustum& vf) const
{
	Maths::Vector3F v[MAX_VERTICES];
	// Use the three vertex indices stored in array m_uiAngleVertexIndices to index out the real vertices, then translate them into Vector3F defined in our Maths module.
	// ...

	for(unsigned int i = 0; i < MAX_VERTICES; ++i)
	{
		if(vf.IsPointInside(v[i]))
		{
			return true;
		}
	}

	return false;
}

void BTNode::GetValidIndices(TerrainGroup& tg, void* pIB) const
{
	if(IsLeaf())
	{
		if(!tg.IsIndexBufferFull())
		{
			tg.PushIndex(m_uiAngleVertexIndices[VP_TOP], pIB);
			tg.PushIndex(m_uiAngleVertexIndices[VP_RIGHT], pIB);
			tg.PushIndex(m_uiAngleVertexIndices[VP_LEFT], pIB);
			tg.AddNumOfPrimitives();
		}
	}
	else
	{
		for(unsigned int i = 0; i < MAX_CHILDS; ++i)
		{
			if(m_btnChilds[i])
			{
				m_btnChilds[i]->GetValidIndices(tg, pIB);
			}
		}
	}
}

}  // namespace