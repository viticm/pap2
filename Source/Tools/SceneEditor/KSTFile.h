// File: KSTFile.h
// Desc: 定义了一些结构，用来读取stf文件
#pragma once

#ifndef KSTFILE_H
#define KSTFILE_H

const float DEF_MAX_BRANCHWIDTH = 200;
const float DEF_MAX_BRANCHHEIGHT = 200;
const float DEF_MIN_BRANCHWIDTH = 50;
const float DEF_MIN_BRANCHHEIGHT = 50;

const unsigned int BranchVertexFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
struct BranchVertex
{
	float x, y, z;
	D3DCOLOR diffuse;  // 只其缩放光照强度的作用
	float u, v;
};

const unsigned int NumOfVerticesPerBranch = 4;

struct KSTBranch
{
	unsigned int m_Texture;  // The texture index, not used yet

	unsigned int m_Bone;  // Which bone this branch is situated
	D3DXMATRIX m_CurrMatrix;  // World matrix 

	D3DXVECTOR3 m_Center;

	float m_Angle;  // Current angle that is offset from the original orientation
	float m_Velocity;

	unsigned int m_VertexOffset;
	float m_Width, m_Height;

public:
	KSTBranch();
	~KSTBranch();

	void Clear();

	unsigned int GetVertexOffset() const { return m_VertexOffset; }
	void SetVertexOffset(unsigned int vo) { m_VertexOffset = vo; }

	void SetDiffuse(D3DCOLOR diffuse, BranchVertex* pVB);
	void SetDimensions(float Width, float Height, BranchVertex* pVB);
};

const unsigned int FileBranchSize = sizeof(KSTBranch) - sizeof(IDirect3DVertexBuffer9*) + NumOfVerticesPerBranch * sizeof(BranchVertex);

#endif