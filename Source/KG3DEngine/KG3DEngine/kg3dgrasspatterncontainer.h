#pragma once

struct HighDefGrassModelVertex
{
	D3DXVECTOR3 vec3Pos;
	D3DXVECTOR3 vec3Normal;
	D3DXVECTOR2 vec2UV;
	const static DWORD dwFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};


struct PatternModel
{
	HighDefGrassModelVertex* pVertexBuffer;
	DWORD               dwVertexCnt;
	short*              pIndicesBuffer;
	DWORD               dwIndicesCnt;
	BBox                PatternBBox;
	TCHAR               strModelFileName[MAX_PATH];
	~PatternModel();
	PatternModel();
	PatternModel(const PatternModel& src);
	PatternModel& operator =(const PatternModel& src);
};

class KG3DGrassPatternContainer
{
public:
	HRESULT LoadPattern(LPCSTR strFileName, int& nIndex);
	HRESULT FindPattern(LPCSTR strFileName, INT *pnRetIndex);
	PatternModel* GetPattern(unsigned int nIndex);
	KG3DModel* GetModel(unsigned int nIndex);
	KG3DGrassPatternContainer();
	~KG3DGrassPatternContainer();

    HRESULT Init();
	HRESULT UnInit();

private:
	std::vector<PatternModel> m_Patterns;
	std::vector<KG3DModel*> m_p3DModel;
};

extern KG3DGrassPatternContainer g_GrassPatternContainer;