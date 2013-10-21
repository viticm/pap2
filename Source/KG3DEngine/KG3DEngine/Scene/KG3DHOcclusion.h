#pragma  once

struct OBBNode;

class KG3DHOcclusion
{
public:

	KG3DHOcclusion()
	{
		m_nWidth = m_nHeight = 0;
		m_pfHeightBuffer = NULL;
	}
	KG3DHOcclusion(const int W,const int H);
	~KG3DHOcclusion();

	HRESULT			Init(const int &W,const int &H);
	bool			IsBelow(float &aSX,float &aSY,float &bSX,float &bSY);
	bool			ClipOBBLine(D3DXVECTOR4 &a,D3DXVECTOR4 &b);

	bool			Clip_Test_OBBLine(D3DXVECTOR4 a,D3DXVECTOR4 b,D3DXVECTOR2 &outa,D3DXVECTOR2 &outb);
	
	void			Insert(float &aSX,float &aSY,float &bSX,float &bSY);
	void			Insert(D3DXVECTOR2 &aS,D3DXVECTOR2 &bS);
	void			Insert(D3DXVECTOR4 a,D3DXVECTOR4 b,const OBBNode &refNode);		
	void			DrawHerizon();

public:

	inline	int GetWidth()
	{
		return m_nWidth;
	}

	inline	int GetHeight()
	{
		return m_nHeight;
	}
	

public:


	void			ReSet(const int &W,const int &H);

	inline void		Clear()
	{
		memset(m_pfHeightBuffer,0,sizeof(float) * m_nHeight * m_nWidth);
	}



private:

	int							m_nWidth,m_nHeight;
	float						*m_pfHeightBuffer;
	LPDIRECT3DVERTEXBUFFER9		m_lpHerizonVB;
};