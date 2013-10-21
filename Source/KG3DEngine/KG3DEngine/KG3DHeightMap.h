#pragma once


class KG3DHeightMap
{
	friend class KG3DSceneOutDoorSpaceMgr;
private:
	struct PNG_FILE_INFO
	{
		unsigned width;
		unsigned height;
		int bit_depth;
		int color_type;
		int interlace_type;
		int compression_type;
		int filter_type;
	};  
	PNG_FILE_INFO m_PngFileInfo;

	BITMAPINFO    m_Bmpinfo;

protected:

	int m_nWidth;
	int	m_nHeight;//顶点个数

	float* m_lpData;
	D3DXVECTOR3* m_lpNormal;

	float m_fHeightLowest;
	float m_fHeightTotal;

	BYTE *LoadBmpFile(int&nSize,const char FileName[],BITMAPINFO *info);
	BYTE *LoadPngFile(const char file_name[]);

	HRESULT SmoothMapHeight();//平滑整个地形
	HRESULT SmoothNormal();//平滑地形所有顶点法向量
	HRESULT SmoothNormal(int xStart,int xEnd,int zStart,int zEnd); //平滑指定的矩形范围内的地形顶点法向量
	int GetPlaneBufferIndex(int nX,int nZ);
public:
	HRESULT SaveToR32File(int nWidth,int nHeight,LPCTSTR pFileName);

	int GetMapHeight();
	int GetMapWidth();
	float*  GetVertexPointer(int nX,int nZ);
	D3DXVECTOR3* GetVertexNormalPointer(int nX,int nZ);
	HRESULT SetHeight(int xStart,int xEnd,int zStart,int zEnd,float fHeight);
	HRESULT SmoothMapHeight(int xStart,int xEnd,int zStart,int zEnd,float K);//平滑指定的矩形范围内的地形高度
	HRESULT ComputeNormal();//计算地形所有顶点法向量
	HRESULT ComputeNormal(int xStart,int xEnd,int zStart,int zEnd);//计算指定的矩形范围内的地形顶点法向量,m_lpData里存的都是绝对高度值时用的

	float GetHeight(float fX,float fY);
	D3DXVECTOR3 GetNormal(float fX,float fY);

	HRESULT SetHeightScale(float fLowest,float fTotal);

	HRESULT Init(int nWidth,int nHeight);
	HRESULT UnInit();

	HRESULT CreateFromPngFile(LPSTR pFileName);
	HRESULT CreateFromBmpFile(LPCTSTR FileDir);

	HRESULT SaveToHeightMapFile(const char cszFileName[]);
	HRESULT LoadFromHeightMapFile(const char cszFileName[]);

	HRESULT CreateFromR32File(int nWidth,int nHeight,LPCTSTR pFileName);
	HRESULT TranslateAbsoluteData();

	KG3DHeightMap(void);
	~KG3DHeightMap(void);
};
