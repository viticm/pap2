// File: TerrainNode.h
// Desc: 定义渲染地形需要结点的3种结点：FieldNode、BlockNode、BTNode

#ifndef TERRAINNODE_H
#define TERRAINNODE_H

#include"..\Maths\Maths.h"
#include"..\Render\Render.h"
#include"TerrainGroup.h"
#include"TerrainTreeDesc.h"

#include<vector>
using std::vector;
#include<map>
using std::map;

namespace Terrain
{

struct DLLAPI TerrainVertex;
class DLLAPI TerrainTree;
class DLLAPI TerrainGroup;

class DLLAPI FieldNode;
class DLLAPI QuadLeaf;
class DLLAPI BTNode;

const unsigned char NM_L = 0x01;
const unsigned char NM_T = 0x02;
const unsigned char NM_R = 0x04;
const unsigned char NM_B = 0x08;
const unsigned char NM_TL = 0x10;
const unsigned char NM_TR = 0x20;
const unsigned char NM_BR = 0x40;
const unsigned char NM_BL = 0x80;

/***********************************
QuadNode: Base class
***********************************/
class DLLAPI QuadNode: public Maths::AxisAlignedBoundingBox
{
public:
	enum
	{
		MAX_CHILDS = 4,
		MAX_NEIGHBOURS = 8,
		MAX_BTNODEROOTS = 2
	};

	enum NeighbourPos
	{
		// 4个边相邻的结点
		NP_L = 0,
		NP_T = 1,
		NP_R = 2,
		NP_B = 3,

		// 4个顶点相邻的结点；仅用于可见域，与二元三角树无关
		NP_TL = 4,
		NP_TR = 5,
		NP_BR = 6,
		NP_BL = 7
	};

	enum ChildPos
	{
		CP_TL = 0,
		CP_TR = 1,
		CP_BR = 2,
		CP_BL = 3
	};
	
	enum BTNodeRootPos
	{
		BTP_LEFT = 0,
		BTP_RIGHT = 1
	};

public:
	Maths::Vector3F m_Center;  // The center point
	QuadNode* m_qnParent;  // 指向父结点的指针
	QuadNode* m_qnChilds;  // 指向连续存储MAX_CHILDS个子结点的第一个结点的指针

public:
	QuadNode(): AxisAlignedBoundingBox(), m_Center(), m_qnParent(0), m_qnChilds(0) {}
	QuadNode(const Maths::Vector3F& vCenter, float fWidth, float fHeight, QuadNode* pParent);
	QuadNode(const QuadNode& qn): Maths::AxisAlignedBoundingBox(qn), m_Center(qn.m_Center), m_qnParent(qn.m_qnParent), m_qnChilds(qn.m_qnChilds) {}
	QuadNode& operator = (const QuadNode& qn);
	virtual ~QuadNode();

	virtual void Init(const Maths::Vector3F& vCenter, float fWidth, float fHeight, QuadNode* pParent);

	QuadNode* GetChild(ChildPos cp) const;
	QuadNode* GetChild(unsigned int i) const;
	QuadNode* GetNeighbour(NeighbourPos np) const { return m_qnParent? m_qnParent->HandleGetNeighbour((QuadNode*)this, np): 0; }
	QuadNode* HandleGetNeighbour(QuadNode* pChild, NeighbourPos np) const;

	bool IsPointInside(const Maths::Vector3F& vPoint) const { return IfContain(vPoint); }

	bool GetCorner(ChildPos CornerID, Maths::Vector3F& Corner) const;
	bool GetCorner(ChildPos CornerID, TerrainVertex& tv) const;
	void UpdateAABB(const Maths::Vector3F& aabb);
	void UpdateAABB_Y(float y);
	
	virtual BTNode* GetBTNodeRoot(BTNodeRootPos pos) const { return 0; }
	virtual bool IsLeaf() const { return false; }
	virtual bool IsFieldNode() const { return false; }

	static ChildPos TranslateIntToCP(unsigned int i);

	bool GetAllFieldNodes(vector<FieldNode*>& vFieldNodes) const;
	bool FindVisibleFieldNodes(const Maths::Camera& camera, vector<FieldNode*>& vVisibleFieldNodes) const;
	bool GetAllQuadLeaves(vector<QuadLeaf*>& vQuadLeaves, bool bForVisible = true) const;
	bool FindVisibleQuadLeaves(const Maths::Camera& camera, vector<QuadLeaf*>& vVisibleQuadLeaves) const;
	Maths::RelativeLocation TestQuadNodeVSCamera(const Maths::Camera& camera) const;
	
	// For debug
	//unsigned int GetID() const { return 0; }
	//static unsigned int GetNumOfQuadLeaves();
};

inline QuadNode::ChildPos QuadNode::TranslateIntToCP(unsigned int i)
{
	switch(i)
	{
	case 0:
		return CP_TL;

	case 1:
		return CP_TR;

	case 2:
		return CP_BR;

	case 3:
		return CP_BL;

	default:
		throw Common::Exception(Common::EL_BAD, "QuadNode::TranslateIntToCP", "Illegal value of parameter!");
	}
}

/***********************************
FieldNode: 
***********************************/
class DLLAPI FieldNode: public QuadNode
{
public:
	FieldNode(): QuadNode() {}
	FieldNode(const Maths::Vector3F& vCenter, float fWidth, float fHeight, QuadNode* pParent): QuadNode(vCenter, fWidth, fHeight, pParent) {}
	FieldNode(const FieldNode& fn): QuadNode(fn) {}
	FieldNode& operator = (const FieldNode& fn);
	virtual ~FieldNode() {}

	virtual bool IsFieldNode() const { return true; }
};

/***********************************
QuadLeaf: QuadNode的叶结点，二元三角树从这些结点上衍生
***********************************/
class DLLAPI QuadLeaf: public QuadNode
{
public:
	BTNode* m_btnRoots;  // 二元三角树
	unsigned short m_TextureID;  // Texture ID; this indicates which texture should be used for this QuadLeaf(BNL)
	// MaterialPtr: The material handle; indicates the info about material and texture layer, etc.

	bool m_bVisible;
	unsigned char m_TL_U, m_TL_V;  // The texture coordinates of top-left corner

public:
	QuadLeaf(): QuadNode(), m_btnRoots(0), m_bVisible(false), m_TL_U(0), m_TL_V(0) {}
	QuadLeaf(const Maths::Vector3F& vCenter, float fWidth, float fHeight, QuadNode* parent);
	QuadLeaf(const QuadLeaf& ql): QuadNode(ql), m_btnRoots(ql.m_btnRoots), m_bVisible(ql.m_bVisible) {}
	QuadLeaf& operator = (const QuadLeaf& ql);
	virtual ~QuadLeaf();

	virtual void Init(const Maths::Vector3F& vCenter, float fWidth, float fHeight, QuadNode* pParent);

	virtual BTNode* GetBTNodeRoot(BTNodeRootPos pos) const;
	BTNode* GetBTNodeRoot(unsigned int i) const;
	virtual bool IsLeaf() const { return true; }

	void CreateBTTreeSublevels(const unsigned int LOD, const TerrainTreeDesc& ttdesc, void* pVB, map<QuadLeaf*, QuadLeaf*>& TouchedQuadLeaves);
	void GetValidIndices(TerrainGroup& tg, void* pIB) const;
	void InvalidIndices();
};

inline QuadNode* QuadNode::GetChild(ChildPos cp) const
{
	if(!m_qnChilds)
	{
		return 0;
	}

	if(m_qnChilds->IsFieldNode())
	{
		FieldNode* pFN = static_cast<FieldNode*>(m_qnChilds);
		return pFN + cp;
	}
	else if(m_qnChilds->IsLeaf())
	{
		QuadLeaf* pQL = static_cast<QuadLeaf*>(m_qnChilds);
		return pQL + cp;
	}
	else  // Is simple QuadNode
	{
		return m_qnChilds + cp;
	}
}

inline QuadNode* QuadNode::GetChild(unsigned int i) const
{
	if(i >= MAX_CHILDS)
	{
		throw Common::Exception(Common::EL_BAD, "QuadNode::GetChild", "The index is out of range!");
	}

	if(!m_qnChilds)
	{
		return 0;
	}

	if(m_qnChilds->IsFieldNode())
	{
		FieldNode* pFN = static_cast<FieldNode*>(m_qnChilds);
		return pFN + i;
	}
	else if(m_qnChilds->IsLeaf())
	{
		QuadLeaf* pQL = static_cast<QuadLeaf*>(m_qnChilds);
		return pQL + i;
	}
	else  // Is simple QuadNode
	{
		return m_qnChilds + i;
	}
}

/***********************************
BTNode: 二元三角树的结点，真正与地形渲染相关的结点
***********************************/
class DLLAPI BTNode
{
public:
	enum
	{
		MAX_CHILDS = 2,
		MAX_NEIGHBOURS = 3,
		MAX_VERTICES = 3
	};

	enum NeighbourPos
	{
		NP_LEFT = 0,
		NP_RIGHT = 1,
		NP_BOTTOM = 2
	};

	enum ChildPos
	{
		CP_LEFT = 0,
		CP_RIGHT = 1
	};

	enum VertexPos
	{
		VP_TOP = 0,
		VP_LEFT = 1,
		VP_RIGHT = 2
	};

public:
	QuadLeaf* m_qlParent;
	BTNode* m_btnParent;
	BTNode* m_btnNeighbours[MAX_NEIGHBOURS];
	BTNode* m_btnChilds[MAX_CHILDS];

	// The following members are always valid. It's a kinda different from BlockNode.
	// However, only in leaves the three vertex indices are used for rendering Terrain, while in non-leaf nodes the three can be used for visibility detection (even this is not necessary in most situations).
	unsigned int m_uiAngleVertexIndices[MAX_VERTICES];

public:
	BTNode();
	BTNode(QuadLeaf* bnlParent);  // Only BNLs may call this constructor
	BTNode(BTNode* btnParent);
	BTNode(const BTNode& btn);
	BTNode& operator = (const BTNode& btn);

	bool CreateChilds(const unsigned int LOD, const TerrainTreeDesc& ttdesc, const unsigned int Level, void* pVB, map<QuadLeaf*, QuadLeaf*>& TouchedQuadLeaves, bool bCalledForDiamond = false);
	void SetNeighbours(ChildPos cp);
	bool IsVisible(const Maths::ViewFrustum& vf) const;
	bool IsLeaf() const{ return !m_btnChilds[CP_LEFT] && !m_btnChilds[CP_RIGHT]; }
	void GetValidIndices(TerrainGroup& tg, void* pIB) const;
};

inline BTNode* QuadLeaf::GetBTNodeRoot(BTNodeRootPos pos) const
{
	return m_btnRoots + pos;
}

}  // namespace

#endif