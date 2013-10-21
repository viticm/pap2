// File: TerrainTree.h
// Desc: 四叉树与二元三角树相结合的TerrainTree，可用于可视判断、碰撞检测、LOD地形渲染

#ifndef TERRAINTREE_H
#define TERRAINTREE_H

#include"TerrainTreeDesc.h"
#include"HeightMap.h"
#include"TerrainNode.h"
#include"TerrainLOD.h"

#include<vector>
using std::vector;
#include<map>
using std::map;

namespace Terrain
{

const unsigned int TerrainVertexFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
struct DLLAPI TerrainVertex  // 地形顶点的格式
{
	float x, y, z;  // Position
	DWORD Diffuse;  // Diffuse color
	float u, v;  // Base texture coordinates
	//float LightMapU, LightMapV;  // Light map coordinates
	//float MagicU, MagicV;  // Magic coordinates
};

class DLLAPI TerrainTree: public Render::Renderable
{
public:
	// Some constants
	enum
	{
		BTPOOL_SIZE = 3000,  // For the pool of BTNode. This pool is used for generating BTNodes dynamically
		MAXNUM_VERTICES = 3000,  // 渲染地形所用顶点的最大数目
		MAXNUM_INDICES =  9000  // 渲染地形所用顶点索引的最大树目
	};

protected:
	/*****************************************
	* Terrain-Tree related; Used for dynamic terrain generating
	*****************************************/

	TerrainTreeDesc m_TTDesc;
	HeightMap<unsigned char> m_HeightMap;

	QuadNode* m_Root;  // root of the tree
	float m_CenterX;
	float m_CenterZ;

	vector<FieldNode*> m_AllFieldNodes;
	vector<QuadLeaf*> m_AllQuadLeaves;
	vector<FieldNode*> m_VisibleFieldNodes;

	BTNode* m_BTs;  // Note: this array doesn't contain the roots of BTNode-Trees!!
	unsigned int m_CurrentBTPos;  // 这是一个类似内存池的东西，它可以使我们在动态创建二元三角树时不用动态分配内存

	vector<QuadLeaf*> m_VisibleQuadLeaves;  // 存储当前帧需要渲染的所有的QuadLeaf
	vector<QuadLeaf*> m_vDecoration;  // 用于修正因不可视结点造成的地面呼吸、空三角现象
	map<QuadLeaf*, QuadLeaf*> m_TouchedQuadLeaves;  // 存储上一帧渲染时，所有被访问过的QuadLeaf：包括全部可见的QuadLeaf和部分由菱形需要而访问的QuadLeaf


	/*****************************************
	* Terrain-Data related; Used for rendering
	*****************************************/

	Render::RSStream m_Stream;  // VertexStream
	unsigned int m_CurrentNumOfVertices;  // statistic
	TerrainGroup m_TerrainGroup;  // In fact, this should be something like std::map<RSMaterial*, TerrainGroup> because generally there are more than 1 TerrainGroup in a real game. However, we haven't yet involved with material, so just 1 TerrainGroup at the moment. This will be changed in future...

	TerrainLOD m_TLOD;

	bool m_bDrawOutlineOnly;

public:  // For debug
	unsigned int m_NumOfVisibleFieldNodes;
	unsigned int GetNumOfVisibleFieldNodes() const { return m_NumOfVisibleFieldNodes; }
	unsigned int m_NumOfVisibleQuadLeaves;
	unsigned int GetNumOfVisibleQuadLeaves() const { return m_NumOfVisibleQuadLeaves; }
	unsigned int m_NumOfVertices;
	unsigned int GetNumOfVertices() const { return m_NumOfVertices; }
	unsigned int m_NumOfPrimitives;
	unsigned int GetNumOfPrimitives() const { return m_NumOfPrimitives; }
	unsigned int m_NumOfSubBTNodes;
	unsigned int GetNumOfSubBTNodes() const { return m_NumOfSubBTNodes; }

	unsigned int m_TimeWhileFindVisibleFieldNodes;
	unsigned int GetTimeWhileFindVisibleFieldNodes() const { return m_TimeWhileFindVisibleFieldNodes; }
	unsigned int m_TimeWhileFindVisibleQuadLeaves;
	unsigned int GetTimeWhileFindVisibleQuadLeaves() const { return m_TimeWhileFindVisibleQuadLeaves; }
	unsigned int m_TimeWhileCreateOutlineVertices;
	unsigned int GetTimeWhileCreateOutlineVertices() const { return m_TimeWhileCreateOutlineVertices; }
	unsigned int m_TimeWhileHandleTouchedQuadLeaves;
	unsigned int GetTimeWhileHandleTouchedQuadLeaves() const { return m_TimeWhileHandleTouchedQuadLeaves; }
	unsigned int m_TimeWhileCreateSubBTNodes;
	unsigned int GetTimeWhileCreateSubBTNodes() const { return m_TimeWhileCreateSubBTNodes; }
	unsigned int m_TimeWhileExtractIndices;
	unsigned int GetTimeWhileExtractIndices() const { return m_TimeWhileExtractIndices; }

public:
	TerrainTree();
	TerrainTree(const Common::String& sName);  // BE WARE: sName must have NO extension
	virtual ~TerrainTree();

	virtual void CreateD3DResources();

	void SetName(const Common::String& sName);
	void SetTerrainTreeDesc(const TerrainTreeDesc& TTDesc);

	void CreateQuadTree(float cx, float cy, float cz);
	void CreateQuadTreeSublevels(QuadNode* pNode, const unsigned int Sublevel, const unsigned int NumOfFieldNodeLevels, const unsigned int NumOfBlockNodeLevels);
	void SetQuadLeafTLUV(QuadLeaf* pQL);
	void DestroyTree();

	bool GenerateOutline(const Maths::Camera& camera);
	bool CreateBTTree(const Maths::Camera& camera);
	bool IsBTPoolFull() const { return m_CurrentBTPos >= BTPOOL_SIZE; }
	BTNode* GetBTNodeFromPool(BTNode* btnParent);

	float GetTerrainWidth() const { return m_TTDesc.m_FNs * m_TTDesc.m_QLs * m_TTDesc.m_XSpan; }
	float GetTerrainHeight() const { return m_TTDesc.m_FNs * m_TTDesc.m_QLs * m_TTDesc.m_ZSpan; }
	
	QuadLeaf* GetLocatedQuadLeaf(const Maths::Vector3F& point) const;
	virtual float GetHeightValue(float& x, float& z);

	bool GetDrawOutlineOnly() const { return m_bDrawOutlineOnly; }
	void SetDrawOutlineOnly(bool bVal) { m_bDrawOutlineOnly = bVal; }

	unsigned int CalculateLevelOfDetail(const Maths::Camera& camera, const Maths::Vector3F& vPoint) const { return m_TLOD.CalculateLevelOfDetail(camera, vPoint); }
	const TerrainTreeDesc& GetTTDesc() const { return m_TTDesc; }

	unsigned int GetCurrentNumOfVertices() const { return m_CurrentNumOfVertices; }
	bool IsVertexBufferFull() const { return m_CurrentNumOfVertices >= MAXNUM_VERTICES; }
	void* LockVertexBuffer();
	void PushVertex(const TerrainVertex& tv, void* pVB);
	bool GetTerrainVertex(void* pVB, unsigned int VertexIndex, TerrainVertex& tv) const;
	bool GetTerrainVertex(void* pVB, unsigned int VertexIndex, TerrainVertex*& ptv) const;
	void UnlockVertexBuffer();
	bool IsIndexBufferFull() const { return m_TerrainGroup.IsIndexBufferFull(); }
	void* LockIndexBuffer() { return m_TerrainGroup.LockIndexBuffer(); }
	void PushIndex(unsigned int Index, void* pIB) { m_TerrainGroup.PushIndex(Index, pIB); }
	void UnlockIndexBuffer() { m_TerrainGroup.UnlockIndexBuffer(); }

	// Functions derived from Render::Renderable
	virtual void Destroy();
	virtual void Render();
};

}  // namespace

#endif