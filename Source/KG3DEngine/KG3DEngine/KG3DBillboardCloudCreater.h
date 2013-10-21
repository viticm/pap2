#pragma once
#include "IEKG3DBillBoardCloudCreater.h"


class KG3DMesh;
class IEKG3DSceneModelEditor;
class KG3DModel;
class KG3DModelSpeedTree;
class KG3DTexture;
class KG3DRenderTargetTool;
class KG3DBillboardCloud;

class KG3DBillboardCloudCreater : public IEKG3DBillBoardCloudCreater
{
public:
	struct Triangle
	{
		int nIndex;
		D3DXVECTOR3 vVertexPos[3];
		D3DXVECTOR2 vVertexUV1[3];
		D3DXVECTOR2 vVertexUV2[3];
		D3DXVECTOR3 vVertexNormal[3];
		DWORD       m_dwSubSet;

		D3DXVECTOR3 vTriangleNormal;
		float fArea;
	};
	struct BillboardPlane
	{
		D3DXPLANE vPlane;
		D3DXVECTOR3 vPlaneNormal;
		D3DXVECTOR3 vCenter;
		vector<Triangle>vecReferenceTriangle;	//代表的Triangle
		float fArea;
		D3DXVECTOR3 vAxisX,vAxisY;
		AABBOX vBBox;
		FRECT vRectUV;

		HRESULT ComputeAxis();
		HRESULT ComputeBBox();
	};
private:
	struct _VextorOffset {
		_VextorOffset(float fSize, float ua, float ub, float uc, float ud,
			float va, float vb, float vc, float vd);
		enum {count = 6};
		D3DXVECTOR3 vOffsets[count][3];
		D3DXVECTOR2 vUVs[count][3];
		D3DXVECTOR3 vNormal[count];
	};

	struct _Point 
	{
		float _x; 
		float _y;

		_Point() : _x(0.f), _y(0.f)
		{
		} 

		_Point(float x, float y) : _x(x), _y(y)
		{
		}

		bool operator < (const _Point& p)
		{
			if (_x < p._x)
				return true;
			else if (_x == p._x)
				return _y < p._y;
		}

		bool operator == (const _Point& p)
		{
			return (_x == p._x) && (_y == p._y);
		}
	};
	struct _Rect 
	{
		float left;
		float top;
		float right;
		float bottom;
	};
private:
	KG3DBillboardCloud* m_pBillboardCloud;	//预览用

	KG3DModel* m_lpModel;
	D3DXVECTOR3 m_vModelOrigin;
	vector<Triangle>	m_vecTriangles;			//Input模型或者别的东西的时候，把模型解开到这个Vector里面
	vector<BillboardPlane>m_vecBillboardPlanes;	//计算算好的Billboard
	IEKG3DSceneModelEditor* m_lpModelEdtior;	//只为模型编辑器输入模型使用
	float m_fPrecision;//精度
	float m_fPrecisionTemp;
	D3DXVECTOR3 m_vTransWhenRenderBillboards;
	float m_fTextureWidth;
	list<BillboardPlane*>m_listBillboardToTry;
	vector<BillboardPlane*>m_vecBillboardTried;

	KG3DRenderTargetTool* m_lpRenderTargetTool;

	LPDIRECT3DTEXTURE9 m_lpTextureColor;
	LPDIRECT3DTEXTURE9 m_lpTextureNormal;

	DWORD m_dwTextureWidth;	//决定Billboard的贴图的精度
	BOOL  m_bTwitterPlane;
	D3DXVECTOR3  m_vFronts[6];
	KG3DTexture* m_apTexArray[16];
	DWORD        m_dwTexNumber;
	std::list<_Point> m_listMovePos;
	vector<Triangle>  m_vecTrianglesCopy;
	DWORD m_dwState;
	float m_fTexInteval;
public:

	virtual int GetTextureWidth();
	virtual void SetTextureWidth(int nWidth);

	virtual HRESULT ComputeUV();
	virtual HRESULT ComputeTexture();
	virtual float GetPrecision()	{return m_fPrecision;}
	virtual void  SetPrecision(float Precision){	m_fPrecision = Precision;}
	virtual DWORD GetBillBoardCount()	{return (DWORD)m_vecBillboardPlanes.size();}
	virtual DWORD GetState()                        { return m_dwState; }
	virtual void StartBillBoardCreating();  
	virtual void StopBillBoardCreating(); 
	virtual BOOL IsTwitterPlane()                   { return m_bTwitterPlane; }
	virtual void EnableTwitterPlane(BOOL bEnable)   { m_bTwitterPlane = bEnable; }
	virtual HRESULT SaveBillboardCloud();

	VOID SetModelEditor(IEKG3DSceneModelEditor* pEditor){m_lpModelEdtior = pEditor;}
	HRESULT Render();//RanderBillBoardPlanes
	HRESULT InputModel(KG3DModel* pModel);//入口，分解出Mesh和贴图

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

private:
	void PipelineBillBoardCreating();
	
	HRESULT InputMesh(KG3DMesh* pMesh);
	HRESULT InputSpeedTree(KG3DModelSpeedTree* pSTree);

	float TriangleArea(Triangle* pTriangle);
	HRESULT FindBillboardPlane(BillboardPlane& newPlane,vector<Triangle>* pvecTrianlges,float e);	//一次拾取一个BillboardPlane
	bool CalcBillBoardUV();

	HRESULT ProcessBillBoardUV();
	HRESULT RenderTextureForBillBoard(BillboardPlane* pPlane,BOOL bClear,LPD3DXEFFECT pShader,LPDIRECT3DTEXTURE9 pOutputTexture);

	
public:
	KG3DBillboardCloudCreater(void);
	virtual ~KG3DBillboardCloudCreater(void);
private:
	static float KG3DBillboardCloudCreater::CalcRange(const vector<BillboardPlane*>& vBillBoards);
	HRESULT CreateTextures();//创建临时用的那两张贴图
};

//////////////////////////////////////////////////////////////////////////


struct KG3DBillboardCloudCreater2 : public IEKG3DBillboardCloudCreater2
{
	virtual HRESULT BuildForModel(IEKG3DModel* pModel) = 0;	//入口

	virtual HRESULT FrameMove() = 0;
	virtual HRESULT Render() = 0;

	virtual VOID	SetLastBCTransformation(const D3DXMATRIX& mat) = 0;//预览的时候才有用

	virtual ~KG3DBillboardCloudCreater2() = 0{}


	static KG3DBillboardCloudCreater2* Build(LPCTSTR, DWORD_PTR, DWORD_PTR);
};

