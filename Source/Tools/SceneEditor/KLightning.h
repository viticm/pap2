// File: KLightning.h
// Desc:

#pragma once

#ifndef KLIGHTNING_H
#define KLIGHTNING_H

const unsigned int LIGHTNING_VERTEX_FVF = D3DFVF_XYZ | D3DFVF_TEX1;
struct KLightningVertex
{
	float x, y, z;
	float u, v;
};

class KLightningNode
{
public:
	unsigned int m_VertexOffset;  // The first vertex index into
	unsigned int m_NumOfVertices;  // The num of vertices

public:
	KLightningNode(): m_VertexOffset(0), m_NumOfVertices(0) {}
	~KLightningNode() {}

	unsigned int GetVertexOffset() const { return m_VertexOffset; }
	unsigned int GetNumOfVertices() const { return m_NumOfVertices; }

	void SetVertexOffset(unsigned int VertexOffset) { m_VertexOffset = VertexOffset; }
	void SetNumOfVertices(unsigned int NumOfVertices) { m_NumOfVertices = NumOfVertices; }

	KLightningVertex* GetVertex(KLightningVertex* pLVBuffer, unsigned int index) const;
};

class KLightning: public KModel
{
protected:
	friend class KDlg_Lightning;

	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DTexture9* m_pTexture;
	string m_TexName;

	KLightningNode* m_pNodes;
	unsigned int m_MaxNumOfNodes;
	unsigned int m_CurrentNumOfNodes;

	unsigned char m_Alpha;  // 不透明程度

	D3DXVECTOR3 m_StartPoint, m_EndPoint;
	float m_LengthOfNode, m_WidthOfNode;

	unsigned int m_LastTime;  // ms
	unsigned int m_FrameInterval;  // ms
	
	bool m_bCreate;

	string m_Path, m_Name;

public:
	KLightning(): KModel(), m_pVB(0), m_pTexture(0), m_pNodes(0), m_MaxNumOfNodes(0), m_CurrentNumOfNodes(0), m_Alpha(0), m_StartPoint(), m_EndPoint(), m_LengthOfNode(50), m_WidthOfNode(5), m_LastTime(0), m_FrameInterval(0), m_bCreate(false) {}
	virtual ~KLightning() { Destroy(); }

	virtual HRESULT LoadMesh(LPSTR pFileName);
	virtual HRESULT SaveMesh(FILE* pFile);
	virtual HRESULT FrameMove();
	virtual HRESULT Render();

	bool Init(unsigned int MaxNumOfNodes, IDirect3DTexture9* pTexture, unsigned int FrameInterval, const string& sPath, const string& sName);
	bool Create(const D3DXVECTOR3& StartPoint, const D3DXVECTOR3& EndPoint, float LengthOfNode, float WidthOfNode);
	void Destroy();

	HRESULT Acquire();//暴露属性
};

#endif