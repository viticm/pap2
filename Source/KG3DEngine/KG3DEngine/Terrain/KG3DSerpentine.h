#pragma once
#include "IEEditor.h"
class KG3DRepresentObjectNode;

class KG3DFrustum;
class KG3DMemoryFile;
class KG3DSceneOutDoorSpaceMgr;
class KG3DSerpentine;

struct _PassageHead
{
	DWORD  dwNumVertex;
	DWORD  dwNumFaces;
	DWORD  dwFVF;
	DWORD  dwVertexBlock;
	DWORD  dwVertexBlockSize;
	DWORD  dwIndexBlock;
	DWORD  dwIndexBlockSize;
	D3DXVECTOR3 vPosA;
	D3DXVECTOR3 vPosB;
	D3DXVECTOR3 vNormalA;
	D3DXVECTOR3 vNormalB;
	DWORD  dwExtendBlock[8];
	_PassageHead()
	{
		dwFVF = 0;
		dwNumVertex = 0;
		dwNumFaces = 0;
		dwVertexBlock =0;
		dwVertexBlockSize =0;
		dwIndexBlock=0;
		dwIndexBlockSize=0;
		vPosA = D3DXVECTOR3(0,0,0);
		vPosB = D3DXVECTOR3(0,0,0);
		vNormalA = D3DXVECTOR3(0,0,0);
		vNormalB = D3DXVECTOR3(0,0,0);
		ZeroMemory(dwExtendBlock,sizeof(DWORD)*8);
	}
	~_PassageHead(){}
};

struct _PassageData 
{
	DWORD  dwNumVertex;
	DWORD  dwNumFaces;
	DWORD  dwFVF;
	BYTE*  pVBData;
	BYTE*  pIBData;
	DWORD  dwVBSize;
	DWORD  dwIBSize;
	_PassageData()
	{
		dwNumVertex = 0;
		dwNumFaces = 0;
		dwFVF = 0;
		pVBData = NULL;
		pIBData = NULL;
		dwVBSize =0;
		dwIBSize=0;
	}
	~_PassageData()
	{
		SAFE_DELETE_ARRAY(pVBData);
		SAFE_DELETE_ARRAY(pIBData);
	}
};

struct KG3DPassage{		
	struct TinyLine
	{
		D3DXVECTOR3 vPositionA[3];
		D3DXVECTOR3 vPositionB[3];
		D3DXVECTOR3 vBANormal;
		D3DXVECTOR2 vBANormal2D;
		D3DXVECTOR3 vLeft;
		D3DXVECTOR2 vLeft2D;
		float fLength;
		float fLengthStart;
		float fLength2D;
		//float fDotMin,fDotMax;

		BBox bBox;
		BOOL IsPointInLine(float fX,float fZ);

		//Use To Terrain
		D3DXVECTOR3 vExtendPosA[2];
		D3DXVECTOR3 vExtendPosB[2];
		BBox ExtendBBox;
		BOOL IsPointInside(float fX,float fZ);
		//
	};
	vector<TinyLine>m_vecTinyLine;
	BOOL m_bIsVisable;   //是否可见
	int m_nFrameCount;
	KG3DRepresentObjectNode* m_pNodeA;
	KG3DRepresentObjectNode* m_pNodeB; 
	D3DXVECTOR3 m_vPositionA;
	D3DXVECTOR3 m_vPositionB;
	D3DXVECTOR3 m_vBorderPosA[2];//vRiverPositionA[2];
	D3DXVECTOR3 m_vBorderPosB[2];//vRiverPositionB[2];
	D3DXVECTOR3 m_vNormalA;
	D3DXVECTOR3 m_vNormalB;

	//////////////////////////////
	D3DXVECTOR3 m_vExtendPosA[2];
	D3DXVECTOR3 m_vExtendPosB[2];
	AABBOX m_ExtendBBox;
	list<POINT>m_listTerrainCell;
	vector<POINT>m_vecTerrainPoint;
	//////////////////////////////
	float m_fTotalLength; //所有TinyLine的长度总和
	float m_fABDistance;//A点到B点的距离。Length;
	//float fRiverLength[2];
	AABBOX m_Bbox;

	//	BOOL IsPointIn(float fX,float fZ,float fWidht);

	list<POINT>m_listCell;
	vector<POINT>m_vecPoint;

	UINT m_uNumVertex;
	UINT m_uNumFaces;
	LPDIRECT3DVERTEXBUFFER9 m_lpVB;
	LPDIRECT3DINDEXBUFFER9  m_lpIB;

	HRESULT CreateBuffers();
	HRESULT Render();
	HRESULT RenderTinyLines();
	HRESULT RenderSelectMark();
	HRESULT RenderLineEx();
	HRESULT CreateTinyLines(D3DXVECTOR3* pV,const int numVertex,float& fLengthStart,float fRoadWidth,float fExtendWidth);

	WORD	FindVertexPoint(int nX,int nZ);
	WORD	FindVertexTerrainPoint(int nX,int nZ);

	BOOL	GetVertexPoint(WORD& OutI,int nX,int nZ);
	BOOL    GetHeightAndUV(float& fHeight,float& fLeftDis,float& fLength,float fX,float fZ);
	BOOL	IsInside(TinyLine** pLine,float fX,float fZ);

	virtual HRESULT UpdateVertex(KG3DSerpentine* pSerpentine);
	virtual HRESULT UpdatAlpha(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);
	
	void UnInit();
	///读取与保存数据
	HRESULT WriteDataToFile(IFile* pFile);
	HRESULT ReadDataFromFile(IFile* pFile,_PassageData& ReadData);

	HRESULT CreateBuffersFromData(_PassageData& ReadData);
	HRESULT BulidPassageData(float& fLengthStart,float fRoadWidth,float fBendModulus,int nSegmentLength,float fExtendWidth);
public:
	KG3DPassage();
	~KG3DPassage();

};

class KG3DSerpentine
{
public:
	list<KG3DRepresentObjectNode*>m_listNode;
	list<KG3DPassage*>m_listPassage;

	UINT m_uHandle; //  跟DataInterface的TerrainInformation数据绑定，
	
	std::string  m_scName;
	float m_fNodeSize; //节点的大小
	float m_fWidth;//河流宽度
	AABBOX m_BBox;
	int m_nSegmentLength;//分段长度，每个Passage分为N（N>2）个Segment，用于计算路面统纹理。m_nSegmentLength（为正数）的值越小，精度越高。
	float m_fLength;//总长度
	float m_fBendModulus;     //Hermite中切线方程的参数
	float m_fExtendWidth;
	BOOL  m_bDrawSelectMark;//是否显示线框
	BOOL  m_bDrawExtendLine;//是否绘制外线框

//	float m_fDepth;//河流的深度

	/////////////////
	AABBOX m_ExtendBBox;
	/////////////////
	struct TerrainBlockInfo
	{
		int   nLevel;
		POINT Index;
		TerrainBlockInfo()
		{
			nLevel = 0;
			Index.x = 0;
			Index.y = 0;
		}
	};
	vector<TerrainBlockInfo> m_vecParentTerrainBlock;
public:
	HRESULT CheckVisablePassage(const KG3DFrustum* pFrustum);
	HRESULT Render();
	HRESULT RenderLines();
	HRESULT RenderSelectMark();

	BOOL    GetHeightAndUV(float& fHeight,float& fLeftDis,float& fLength,float fX,float fZ);
	////////////////////Terrain////////////////////////////////
	BOOL IsPositionInside(KG3DPassage** ppOutPassage,float fX,float fZ);
	//////////////////////////////////////////////////////
	BOOL IsPositionInsideExtend(KG3DPassage** ppOutPassage,float fX,float fZ);


	HRESULT InputTerrainCell();
	HRESULT ComputePassages();
	BOOL IsTerrainCellInside(int nX,int nZ,DWORD dwOption);

	HRESULT AddOneNode(KG3DRepresentObjectNode* ppNode,D3DXVECTOR3& vPos);

	HRESULT DeleteNode(KG3DRepresentObjectNode* pNode);
	HRESULT DeleteAllNode();

	virtual HRESULT AddOnePassage(KG3DPassage** ppPassage,int nPos);
	virtual HRESULT AddOnePassage(KG3DPassage** ppPassage);
	HRESULT DeletePassage(KG3DPassage* pPassage);
	HRESULT DeleteAllPassage();

	void SetSegmentLength(int nLength);
	int GetSegmentLength();
	void SetWidth(float fWidth);
	float GetWidth();
	void SetDrawSelectMark(BOOL bVal);
	BOOL GetDrawSelectMark();
	void SetDrawExtendLine(BOOL bVal);
	BOOL GetDrawExtendLine();
	float GetNodeSize();
	HRESULT SetNodeSize(float nSize);

	virtual HRESULT SaveToFile(const char cszFileName[]);
	virtual HRESULT LoadFromFile(const char cszFileName[]);
	virtual HRESULT LoadNode(DWORD dwNumOfNode,D3DXVECTOR3* pNodePos);
	
	virtual HRESULT RefreshHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BOOL bToTerrain);
	//HRESULT ComputeAlpha(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr);

	HRESULT Create();
	HRESULT CreatePassages(KG3DRepresentObjectNode* pNode);
	HRESULT ComputePassages(KG3DRepresentObjectNode* pCurNode);
	HRESULT ModifyData(set<KG3DPassage*>* psetPassage);

	HRESULT GetNeedModifyPassageAndNode(KG3DRepresentObjectNode* pInNode,set<KG3DPassage*>* psetPassageOut);
	BOOL IsInPassageSet(set<KG3DPassage*>* psetPassage,KG3DPassage* pPassage);
	BOOL FindPassage(KG3DPassage** ppOutPart,KG3DRepresentObjectNode* pNodeA,KG3DRepresentObjectNode* pNodeB);
	BOOL FindPassage(set<KG3DPassage*>* psetPart,KG3DRepresentObjectNode* pNode);

	virtual void UnInit();

	ULONG AddRef();
	ULONG Release();
	
protected:
	HRESULT OnReferenceZero(); 
	ULONG m_ulRefCount;
public:
	KG3DSerpentine(void);
	virtual ~KG3DSerpentine(void);
};

class KG3DSerpentineMgr //: public IEKG3DSerpentineMgr
{
public:
	UINT m_uHandle;
	KG3DSceneOutDoorSpaceMgr* m_lpOutDoorMgr;
	map<UINT,std::string> m_mapNames;
	std::string m_strFilePath;
	list<KG3DSerpentine*> m_listSerpentines;
	KG3DSerpentine* m_lpCurrentSerpentine;
public:
	KG3DSerpentineMgr();
	KG3DSerpentineMgr(KG3DSceneOutDoorSpaceMgr* pOutDoorMgr)
	{}
	~KG3DSerpentineMgr();
public:
	void UnInit();

	virtual HRESULT SaveToFile(LPCSTR strFileName); //保存河流信息
	virtual HRESULT AddNodeToCurSerpentine(KG3DRepresentObjectNode* pNode,D3DXVECTOR3& vPos);
	virtual HRESULT NewOne(KG3DSerpentine** ppSerpentine);

	HRESULT SaveToInformationBuffer(DWORD& dwLength,KG3DMemoryFile* pFile);
	HRESULT LoadFromInformationBuffer(DWORD& dwLength,KG3DMemoryFile* pFile);
	//HRESULT LoadByHandle(UINT uHandle,KG3DSerpentine** ppOutRiver);
	UINT GetHandle();
	KG3DSerpentine* GetSerpentineByHandle(UINT& uHandle);
	KG3DSerpentine* GetSerpentineByName(const TCHAR* strName);
	BOOL FindSerpentineByHandle(UINT& uHandle);  	
	HRESULT DeleteAll();

	HRESULT Render();

	int  GetSerpentineIndex(KG3DSerpentine* pSerpentine);
	KG3DSerpentine* GetCurSerpentine();
	///////////编辑器使用到的接口函数////////////////////////////
	virtual HRESULT DeleteOne(int nIndex);
	virtual DWORD GetCount();
	virtual HRESULT AddNewOne();
	virtual HRESULT SetCurSerpentine(int nIndex);
	virtual BOOL	FindSerpentineByName(const TCHAR* strName);
	virtual HRESULT GetAllName(vector<std::string>& vecName);
	virtual HRESULT RefreshCurSerpentine();
	virtual HRESULT RefreshHeight(BOOL bToTerrain);//UpdateTerrainHeight();
	virtual float GetCurSerWidth();
	virtual void SetCurSerWidth(float fWidth);
	virtual void SetCurSerDrawSelectMark(BOOL bVal);
	virtual BOOL GetCurSerDrawSelectMark();
	virtual void SetCurSerDrawExtendLine(BOOL bVal);
	virtual BOOL GetCurSerDrawExtendLine();
	virtual HRESULT SetCurSerModulusK(float fK);
	virtual HRESULT GetCurSerModulusK(float& fK);
	virtual HRESULT SetCurSerName(const TCHAR* strName);
	virtual HRESULT GetCurSerName(std::string& strName);
	virtual HRESULT SetCurSerSegmentLength(int nLength);
	virtual HRESULT GetCurSerSegmentLength(int& nLength);
	virtual HRESULT MoveCurSerpentinePre();
	virtual HRESULT MoveCurSerpentineNext();
};

extern BOOL  DeleteFileFromDirectory(char   *DirName,const char* ext);  
