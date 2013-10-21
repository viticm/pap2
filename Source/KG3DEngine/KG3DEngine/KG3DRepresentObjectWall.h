#pragma once
#include "kg3drepresentobject.h"
#include "KG3DTypes.h"
#include "IEEditor.h"

class KG3DWallDefine : public IEKG3DWallDefine
{
public:
	ULONG m_ulRefCount;

	struct WallElement
	{
		KG3DWALLELEMENTTYPE eType;
		AABBOX          bbox;
		D3DXVECTOR3     vConnectionPointA;
		D3DXVECTOR3     vConnectionPointB;
		float           fLength;
		string          szModelFile;
		int             nIndex;

		HRESULT GetDataFromFile();
	};

	vector<WallElement>m_vecWallElement;
	string m_szName;

	KG3DWallDefine();
	~KG3DWallDefine();

	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	HRESULT LoadFromFile(LPSTR pFileName);
	HRESULT SaveToFile(LPSTR pFileName);

	virtual int GetNumElement();
	virtual HRESULT GetElement(int nIndex,KG3DWALLELEMENTTYPE* pType,D3DXVECTOR3* pA,D3DXVECTOR3* pB,LPSTR pFileName);
	virtual HRESULT SetElement(int nIndex,KG3DWALLELEMENTTYPE eType,D3DXVECTOR3 vA,D3DXVECTOR3 vB,LPSTR pFileName,BOOL bComputeFromMesh);
	virtual HRESULT NewOneElement(int* pIndex);
	virtual HRESULT DeleteElement(int nIndex);


	HRESULT FindElement(vector<WallElement*>*pvecElement,KG3DWALLELEMENTTYPE eType);

};

class KG3DWallDefineTable
{
	friend class KG3DWallDefine;
public:
	map<string,KG3DWallDefine*>m_mapWallDefines;
	list<KG3DWallDefine*>m_listWallDefines;

	HRESULT LoadWallDefineFromFile(LPSTR pFileName,KG3DWallDefine** ppDefine);

	HRESULT Init();
	HRESULT UnInit();

	KG3DWallDefineTable();
	~KG3DWallDefineTable();

protected:
	HRESULT DeleteWallDefine(KG3DWallDefine* pDefine);
};

class KG3DTerrainBlockGrassShareVB_High;

class KG3DRepresentObjectWall :
	public KG3DRepresentObject,public IEKG3DWall
{
	struct Wall;
	struct KeyPoint
	{
		D3DXVECTOR3 m_vPosition;
		KG3DModel*  m_lpModel;
		KeyPoint()
		{
			m_vPosition = D3DXVECTOR3(0,0,0);
			m_lpModel = NULL;
		}
		~KeyPoint()
		{
			SAFE_RELEASE(m_lpModel);
		}

		HRESULT ComputeModel(KG3DRepresentObjectWall* pWall,Wall* pWallSection);

	};

	struct LineSection
	{
		D3DXVECTOR3 vA;
		D3DXVECTOR3 vB;
		KG3DModel*  pModel;
		D3DXMATRIX  mat;
		int nElementIndex;

		LineSection();
		~LineSection();

		HRESULT ComputeModel(KG3DRepresentObjectWall* pWall,Wall* pWallSection);
	};


	struct Line
	{
		int m_nIndexKeyPointA;
		int m_nIndexKeyPointB;
		float m_fLength;
		float m_fLengthXZ;

		BOOL  m_bStartEnd;
		BOOL  m_bDoor;

		vector<LineSection*>vecLineSection;

		HRESULT ComputeLineSection(KG3DRepresentObjectWall* pWall,Wall* pWallSection);

		~Line();
	};

	struct Wall
	{
		vector<KeyPoint*>m_vecKeyPoints;
		vector<Line*>m_vecLines;

		HRESULT ClearLines();
		HRESULT RenderLine();

		HRESULT RefreshLines(KG3DRepresentObjectWall* pWall);

		HRESULT UpdateLine(int nIndex,KG3DRepresentObjectWall* pWall);
		HRESULT GetModels(DWORD& dwFaceCount,vector<KG3DModel*>*pvecModel);

		Wall();
		~Wall();
	};

	BOOL m_bOrthogonality;//正交模式，墙体间相互垂直
	BOOL m_bLockXZ;//统一水平高度
	BOOL m_bAutoPitch;//自动倾斜墙体

protected:
	string m_szWallDefineFile;
	
	KG3DWallDefine* m_lpWallDefine;
	DWORD m_dwWallModelTotalFaces;

	vector<Wall*>m_vecWall;

	KG3DTerrainBlockGrassShareVB_High* m_lpShareVB;
	BOOL m_bNeedRefreshVB;

	HRESULT CreateToShareVB();

	HRESULT GetShareVB();

	LPDIRECT3DVERTEXBUFFER9 GetShareVBuffer(DWORD& dwFaces);
	LPDIRECT3DVERTEXDECLARATION9 m_lpDeclaration;

public:
	virtual HRESULT Copy(KG3DRepresentObject* pSrcObject);

	virtual HRESULT ComputeBBox();
	virtual HRESULT Render(DWORD dwOption);

	virtual HRESULT Load(LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam);//这个用于加载附属的模型或者别的东西

	HRESULT Init();
	HRESULT UnInit();

	KG3DRepresentObjectWall(void);
	~KG3DRepresentObjectWall(void);

	//////////////////////////////////////////////////////////////////////////
	virtual IEKG3DWallDefine* GetWallDefine()
	{
		return m_lpWallDefine;
	}
	virtual HRESULT SetOption(BOOL bOrthogonality,BOOL bLockXZ,BOOL bAutoPitch);
	virtual HRESULT GetOption(BOOL& bOrthogonality,BOOL& bLockXZ,BOOL& bAutoPitch);

	virtual int GetNumWall();
	virtual HRESULT GetWall(int nID,int& nPoint,int& nLine);
	virtual HRESULT GetWallLine(int nID,int nIndex,BOOL& bStartEnd, BOOL& bDoor);
	virtual HRESULT SetWallLine(int nID,int nIndex,BOOL  bStartEnd, BOOL  bDoor);
	virtual HRESULT NewOneWall(int& nID);
	virtual HRESULT DeleteOneWall(int nID);

	virtual HRESULT Fix();

	HRESULT ModifyWallKeyPoint(KG3DRepresentObjectWall* pObjectWall,int nWallID,int nIndex,D3DXVECTOR3 vPosition);
};
extern KG3DWallDefineTable g_cWallDefineTable;