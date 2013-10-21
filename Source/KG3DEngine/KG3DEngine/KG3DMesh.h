#pragma once
#include "kg3dinterface.h"
#include "KG3DTypes.h"
#include "KG3DStructs.h"
#include "KG3DMaterial.h"
#include "IEKG3DMesh.h"
#include "KG3DToolsImpl.h"
#include "KG3DDefaultShaderGroup.h"

#define RENDERTWICE_CULL_LODLEVEL 0.8f

class KG3DScene;
class KG3DSFXBladeTest;
class KG3DAni;
class KG3DBoneWeightTableEx;
class KG3DModel;
class KG3DClipTools;
class KG3DBip;
class KG3DModelSpeedTree;
class Leaf;
struct KG3DIntersectionFilter;

class  KG3DBsp;
struct IKG3DFlexibleBody;

struct KG3DCpuProcessVertexFormat
{
	D3DXVECTOR3 vPosition;
	D3DXVECTOR3 vNormal;
	float       tu,tv,tIndex;
};

struct KG3DMeshCpuProcessData
{
	WORD*                       m_lpIndex;
	KG3DCpuProcessVertexFormat* m_lpVertex;
	BYTE*                       m_lpVertexInfo;
	DWORD         m_dwNumVertex;
	DWORD         m_dwNumFace;
	LPDIRECT3DINDEXBUFFER9      m_lpIB;
	LPDIRECT3DVERTEXBUFFER9     m_lpVB;
	LPDIRECT3DVERTEXBUFFER9     m_lpVBTangent;

	KG3DMeshCpuProcessData()
	{
		m_lpIndex  = NULL;
		m_lpVertex = NULL;
		m_lpVertexInfo = NULL;
		m_dwNumVertex = 0;
		m_dwNumFace   = 0;

		m_lpIB = NULL;
		m_lpVB = NULL;
		m_lpVBTangent = NULL;
	}
	~KG3DMeshCpuProcessData()
	{
		SAFE_DELETE_ARRAY(m_lpIndex);
		SAFE_DELETE_ARRAY(m_lpVertex);
		SAFE_DELETE_ARRAY(m_lpVertexInfo);
		m_dwNumVertex = 0;
		m_dwNumFace   = 0;
		SAFE_RELEASE(m_lpIB);
		SAFE_RELEASE(m_lpVB);
		SAFE_RELEASE(m_lpVBTangent);
	}

	HRESULT Create(DWORD dwFace,DWORD dwVertex);
	HRESULT ComputeTangent(D3DXVECTOR3* pTangent);
	HRESULT WedingTangent(D3DXVECTOR3* pTangent);
	HRESULT PrepareRender();
	D3DXVECTOR3 calculateTangentSpaceVector(
		const D3DXVECTOR3& position1, const D3DXVECTOR3& position2, const D3DXVECTOR3& position3,
		float u1, float v1, float u2, float v2, float u3, float v3);
};

class KG3DMesh : public IKG3DModelMesh, 
	public CKG3DConnectionPoint<IEKG3DMesh>
{
	friend class KG3DAni;
	friend class KG3DBoneWeightTableEx;
	friend class KG3DModel;
	friend class KG3DClipTools;
	friend class KG3DModelSpeedTree;
	friend class Leaf;
	friend class KG3DSkinCPURender;
    friend class KG3DMeshTable;
	friend class KG3DMeshLodGenerater;
	friend class KG3DMeshSkin;
	
public:
	enum SaveMeshBlockType
	{
		SMBT_NORMAL,
		SMBT_OPTIMIZED,
		SMBT_BLENDED,
		SMBT_ACTIVE,
		SMBT_COUNT,			//新的类型加在这个值前面
		SMBT_FORCE_DWORD = 0xffffffff,
	};
	typedef SaveMeshBlockType MeshType;

    const static DWORD s_dwMaxSocketName = 32;
	const static DWORD s_dwMaxSocket = 32;
	const static DWORD s_dwMaxBoneName = 32;
	const static DWORD s_dwMaxChildBone = 32;
	const static DWORD s_dwInvalidateIndex = 0xffffffff;
	const static DWORD s_dwMaxBone = 255;
	const static DWORD s_dwMaxNumVertices = 65535;

	//原始数据复制到Mesh时的拷贝参数信息
	struct VertexFromatOffsetItem
	{
		const static DWORD s_dwMaxVertexElement = 8;
		DWORD dwFVF;//目标FVF
		DWORD dwNumElement;//有几个参数
		DWORD dwSrcOffset[s_dwMaxVertexElement];//源顶点数据偏移量
		DWORD dwSrcStride[s_dwMaxVertexElement];//源顶点数据长度
		DWORD dwDestOffset[s_dwMaxVertexElement];//目的顶点数据偏移量
		DWORD dwDestStride[s_dwMaxVertexElement];//目的顶点数据长度
	};
protected:
	KG3DBsp*            m_lpBsp;
	enum DynamicState
	{
		STATE_NONE,//没有加载
		STATE_VIDEOMEM,//加载到显存中了
		STATE_LOADFAILED,
        STATE_MULTITHREAD_PREPEAR
	};


	//新插槽格式
#pragma pack(push,1) 
	struct Socket1
	{
		TCHAR strSocketName[s_dwMaxSocketName];
		DWORD dwParentBoneIndex;
		D3DXMATRIX matMatrixOffset;//偏移矩阵
		D3DXMATRIX matCurMatrix;
		Socket1()
		{
			dwParentBoneIndex = s_dwInvalidateIndex;
		}
	};

	//新骨骼格式
	struct BoneInfo1
	{
		TCHAR strBoneName[s_dwMaxBoneName];
		DWORD dwNumChild;
		DWORD dwChildIndex[s_dwMaxChildBone];
		DWORD dwParentIndex;

		D3DXMATRIX  CurMatrix;
		D3DXMATRIX  LocalWorldCurMatrix_NotNeeded; //骨骼在本身世界中的位置矩阵
		D3DXMATRIX  ParentReletiveMatrix;

		// New Flexible Bone
		BOOL        IsFlexibleBone;
		int         nFlexBoneLevel_NotNeeded;
		//Bone's BigBox
		BBox        bBox;
		//BigBox's Scale Matrix
		D3DXMATRIX  BBoxScaleMatrix;
		//暂时添加
		DWORD       NumVerticesRef;
		DWORD*      VerticesIndexRef;
		float*      VerticesWeightRef;
		BoneInfo1();
		~BoneInfo1();
	};

	struct MeshDataBlocks
	{
		DWORD  PositionBlock;
		DWORD  NormalBlock;
		DWORD  DiffuseBlock;
		DWORD  TextureUVW1Block;
		DWORD  TextureUVW2Block;
		DWORD  TextureUVW3Block;
		DWORD  FacesIndexBlock;
		DWORD  SubsetIndexBlock;
		DWORD  SkinInfoBlock;
		DWORD  LODInfoBlock;
		DWORD  FlexibleBodyBlock; // not need the data now
		DWORD  BBoxBlock;
		DWORD  BlendMeshBlock;
		DWORD  ExtendBlock[17];
	};

	struct _MeshHead
	{
		DWORD  dwNumVertices;
		DWORD  dwNumFaces;
		DWORD  dwNumSubset;
		union
		{
			MeshDataBlocks Blocks;
			DWORD dwBlocks[30];
		};
		_MeshHead();
	};

    struct _MeshFileHead
    {
        TFileHeader KSFileHeader_NotUse;
        DWORD       dwFileMask;
        DWORD       dwBlockLength;
        DWORD       dwVersion;
        DWORD       AnimationBlock_NotUse;
        DWORD       ExtendBlock_NotUse[10];
        DWORD       MeshCount_NotUse;
        _MeshFileHead();
        const static DWORD s_dwFileMask = 0x4d455348;//"mesh"
        const static DWORD s_dwCurrentVersion = 0;
    };
#pragma pack(pop)

	struct ReadInMeshData
	{
		D3DXVECTOR3* pPos;
		D3DXVECTOR3* pNormals;
		D3DCOLOR*    pDiffuse;
		D3DXVECTOR3* pUV1;
		D3DXVECTOR3* pUV2;
		D3DXVECTOR3* pUV3;
		DWORD*       pFaceIndices;
		DWORD*       pSubsetIndices;
		DWORD        dwMeshFVF;
		DWORD        dwNumVertices;
		DWORD        dwNumFaces;
		DWORD        dwNumSubset;
		ReadInMeshData();
	};

public:
	KG3DMeshCpuProcessData* m_lpCpuProcessData;
	IKG3DFlexibleBody*   m_lpFlexibleBody;

    HRESULT CreateDataForCpuProcess();
	HRESULT CreateDataForCpuProcess(ReadInMeshData* pMeshData);

	KG3DMesh();
	virtual ~KG3DMesh();
		
	virtual DWORD GetID();
	void SetID(DWORD dwID) { m_dwID = dwID; }
	
	virtual unsigned GetType();
	void SetType(DWORD dwType){ m_dwType = dwType; }
	
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	
	virtual HRESULT Init();
	virtual HRESULT UnInit();

	//for IE interface
	virtual HRESULT GetNumBones(INT *pnRetNum);
    virtual HRESULT GetBaseBoneIDVectorSize(DWORD *pdwSize);
    virtual HRESULT GetBaseBoneIDVectorItem(DWORD dwPosition, INT *pBaseBoneID);
    virtual HRESULT GetBoneInfoName(INT nBaseBoneID, LPCTSTR *ppszName);
    virtual HRESULT GetBoneInfoChildVectorSize(DWORD dwBoneInfoPosition, DWORD *pdwSize);
    virtual HRESULT GetBoneInfoChildVectorItem(DWORD dwBoneInfoPosition, DWORD dwChildPosition, INT *pnID);
    virtual HRESULT SetBoneInfoScale(DWORD dwBoneInfoPosition, FLOAT x, FLOAT y, FLOAT z);
    virtual HRESULT GetSocketName(DWORD dwPosition, LPCTSTR *ppszName);
    virtual HRESULT SetSocketName(DWORD dwPosition, LPCTSTR pszName);
    virtual HRESULT GetSocketParentBoneName(DWORD dwPosition, LPCTSTR *ppszParentBoneName);
    virtual HRESULT GetOption(DWORD *pdwOption);
    virtual HRESULT SetOption(DWORD dwOption);  
    virtual HRESULT GetSocketCount(DWORD *pdwSocketCount);
    virtual HRESULT IsMeshSysNotNull(BOOL *pbState);
    virtual HRESULT GetMeshSysFVF(DWORD *pdwFVF);
    virtual HRESULT GetMeshSysNumVertices(DWORD *pdwNumVertex);
    virtual HRESULT LockVertexBufferOfMesfSys(DWORD dwFlag, PVOID *ppBuffer);
    virtual HRESULT UnLockVertexBufferOfMesfSys();
    virtual HRESULT FindSocket(LPCTSTR strSocketName, INT *pnIndex);
    virtual HRESULT AddSocket(LPCTSTR strParentBoneName, LPCTSTR strSocketName);
    virtual HRESULT RemoveSocket(LPCTSTR strSocketName);
    virtual HRESULT GetProgressMeshMinFaces(INT *pnMin);
    virtual HRESULT GetProgressMeshMaxFaces(INT *pnMax);
    virtual HRESULT SphereNormal();
    virtual HRESULT SetNumFaceLOD(INT nNumFace);
	virtual HRESULT RefreshByBase();

    KG3DBsp* GetBsp() { return m_lpBsp; }
	//////////////////////////////////////////////////////////////////////////
	//获取信息
	void GetProfile(DWORD& dwNumFacesTotal,DWORD& dwNumVertices);
	
	virtual LPCSTR GetBoneName(int nIndex);
    virtual int GetNumBones();

	virtual DWORD GetNumSockets();
	LPCSTR GetSocketName(int nIndex);

	virtual BOOL IsSkin();
	DWORD GetNumSubset() { return m_dwNumSubset; }

	D3DXMATRIX* GetSocketMatrix(int nIndex);
	D3DXMATRIX* GetBoneMatrix(int nBoneIndex);

	virtual LPD3DXMESH GetMesh(MeshType Type);//不加引用计数
	
	//////////////////////////////////////////////////////////////////////////
	//加载,保存
	HRESULT LoadFromFileEx(const char cszFileName[], 
		unsigned uFileNameHash, 
		unsigned uOption, 
		DWORD_PTR WParam, 
		DWORD_PTR LParam)
	{
		return LoadFromFile(cszFileName, uFileNameHash, uOption);
	}
	virtual HRESULT LoadFromFile(const TCHAR* strFileName, 
		unsigned uFileNameHash, 
		unsigned uOption);
	virtual HRESULT RefreshOption(unsigned uOption);
	virtual HRESULT SaveToFile(const TCHAR* strFileName,
		unsigned uOption);

    //Object Property
    virtual HRESULT SavePropertyToIni(LPCTSTR cszFileName);

	HRESULT CheckLoadingState();
	
	//////////////////////////////////////////////////////////////////////////
	//内存管理
	virtual HRESULT PrepareRender(unsigned uOption);//压缩数据
	virtual HRESULT PostRender();  //恢复数据，对象可以立即被使用
	virtual HRESULT GetDateInformation(int* pnMem,
		int* pnVedioMem,
		int* pnConMem,
		int* pnConVMem);//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
	virtual unsigned   GetLastUseTime(); //查询上次使用的时间
	virtual unsigned   GetUseFrequency();//查询使用频率，一段时间里的使用次数
	
	
	//////////////////////////////////////////////////////////////////////////
	//设备
	virtual HRESULT OnLostDevice();
	virtual HRESULT OnResetDevice();

	//////////////////////////////////////////////////////////////////////////
	//骨骼相关
	virtual HRESULT	SetBoneIndex(WORD* pIndex);
	virtual int     FindBone(LPCSTR strBoneName);

	virtual BOOL GetAllBoneBox(vector<BBox>& BoneBoxInfo);

	virtual HRESULT ComputeBoneBBoxByIndex(DWORD dwIndex);
	virtual void SetCurSelected(DWORD Index);
	virtual void SetBoneBBoxScale(DWORD Index,float& X,float& Y,float& Z);
	virtual void GetBoneBBoxScale(DWORD Index,float& x,float& y,float& z);
	virtual HRESULT CreateBBoxOfBone();

//	BBox GetBigBox();
//	HRESULT UpdateSelBoneBBox(KG3DScene* pScene,D3DXMATRIX* matWorldInv);
	
	//////////////////////////////////////////////////////////////////////////
	//绘制
	void SetModel(KG3DModel* pModel);
	
	inline LPD3DXMESH GetRenderSkinMesh();

	HRESULT DrawBones(void* pExtInfo);
	
	virtual HRESULT RenderWithoutStateChange(float fAlpha);
	
	virtual HRESULT Render(KG3DMaterial* pMaterial, 
		float fAlpha, 
		DWORD dwOption, 
		void* pExtInfo,BOOL bUseDetail);

	virtual HRESULT RenderShadowMap(LPD3DXEFFECT &lpEffect,
		KG3DMaterial* pMaterial,
		const D3DXMATRIX &matVP,
		const D3DXMATRIX &matWVP);

	virtual HRESULT RenderSelfShadow(LPD3DXEFFECT &lpEffect,
		const D3DXMATRIX &matVP,
		const D3DXMATRIX &matWVP,
		LPDIRECT3DTEXTURE9 lpDepthMap);

	HRESULT DrawNormal();
	HRESULT DrawBinormal();
	HRESULT DrawTangent();

	HRESULT GetRenderMatrix(LPD3DXMESH& pMeshToRender,
		D3DXMATRIX*& pRenderMatrices,
		void* pExtInfo);

	HRESULT Shader_DrawMeshSubset_Begin(int subsetid,
		KG3DMaterial::KG3DMaterialSubset* pSubMat, 
		float fAlpha,
		DWORD dwOption,
		LPD3DXEFFECT pShader,BOOL bUseDetail);

	HRESULT Shader_DrawMeshSubset_NoChange(LPD3DXMESH pMesh,
		int subsetid,
		KG3DMaterial::KG3DMaterialSubset* pSubMat, 
		float fAlpha,
		DWORD dwOption,
		LPD3DXEFFECT pShader,
        BOOL bUseDetail);

	HRESULT Shader_DrawMeshSubset_DepthOnly(LPD3DXMESH pMesh,
		int subsetid,
		KG3DMaterial::KG3DMaterialSubset* pSubMat);

	HRESULT Shader_DrawMeshSubset_End(KG3DMaterial::KG3DMaterialSubset* pSubMat, 
		LPD3DXEFFECT pShader);

	HRESULT Shader_DrawMesh_Begin(LPD3DXMESH pMesh,
		float fAlpha,
		DWORD dwOption,BOOL bUseDetail,LPD3DXEFFECT pShader);

	HRESULT Shader_DrawMesh_NoChange(LPD3DXMESH pMesh,
		KG3DMaterial* pMaterial, 
		float fAlpha,
		DWORD dwOption,BOOL bUseDetail,
		LPD3DXEFFECT pShader);

	HRESULT Shader_DrawMesh_End(LPD3DXMESH pMesh,
		float fAlpha,
		DWORD dwOption,BOOL bUseDetail,LPD3DXEFFECT pShader);

	HRESULT Shader_RenderSkinMeshDepth_Begin();
	HRESULT Shader_RenderSkinMeshDepth_NoChange(D3DXMATRIX* pmatBones, 
        KG3DMaterial* pMaterial, 
		DWORD dwOption);
	HRESULT Shader_RenderSkinMeshDepth_End();
	HRESULT Shader_RenderSkinMesh_Begin(BOOL bUseConverTexture,BOOL bUseDetail);

	HRESULT Shader_RenderSkinMesh_NoChange(D3DXMATRIX* pmatBones, 
        KG3DMaterial* pMaterial, 
		float fAlpha,
		DWORD dwOption,BOOL bDetail);

	HRESULT Shader_RenderSkinMesh_End();

    HRESULT Shader_RenderSkinMesh_SingleSubset(
        D3DXMATRIX *pmatBones,
        KG3DMaterial *pMaterial,
        DWORD dwOption,
        BOOL bDetail
    );

	/*HRESULT Shader_DrawMeshAmbientMap_Begin(LPD3DXMESH pRenderMesh,
		float fAlpha,
		DWORD dwOption,BOOL bUseDetail);*/

	/*HRESULT Shader_DrawMeshAmbientMap_NoChange(LPD3DXMESH pRenderMesh,
		KG3DMaterial* pMaterial,
		float fAlpha,
		DWORD dwOption);
	HRESULT Shader_DrawMeshAmbientMap_End(LPD3DXMESH pRenderMesh,
		float fAlpha,
		DWORD dwOption);*/

	HRESULT Shader_DrawMeshNormalNoSkin(LPD3DXMESH pRenderMesh,
		KG3DMaterial* pMaterial,
		float fAlpha,
		DWORD dwOption);

	static HRESULT Shader_DrawMeshNormalNoSkin_Begin();

	HRESULT Shader_DrawMeshNormalNoSkin_NoChange(LPD3DXMESH pRenderMesh,
        KG3DMaterial* pMaterial,
		float fAlpha,
		DWORD dwOption);

	static HRESULT Shader_DrawMeshNormalNoSkin_End();

	
	//////////////////////////////////////////////////////////////////////////
	//模型处理
	HRESULT OptimizeUnSkinedMesh();
	HRESULT CreateSkinMesh();
	HRESULT GenerateEnhancedMesh(UINT dwNewNumSegs,LPD3DXMESH pMesh);
	virtual HRESULT CreateProgressMesh();
	HRESULT GetProgressMesh(LPD3DXPMESH* pMesh);
	virtual HRESULT PostLoadProcess(void* pData);
	virtual HRESULT AttachBip(KG3DBip* pBip, std::vector<int>& RemapTable);
	
	void Helper_UpdateToObj(int nIndex, 
		const D3DXMATRIX& matBase, 
		D3DXMATRIX* pmatLocal, 
		D3DXMATRIX* pmatObj);
	void UpdateToObj(D3DXMATRIX* pmatObj,
		D3DXMATRIX* pmatLocal);

	//////////////////////////////////////////////////////////////////////////
	//相交检测
	virtual BOOL IsRayIntersect(D3DXVECTOR3&Inter, 
		D3DXVECTOR3 Origin, 
		D3DXVECTOR3 Direction, 
		D3DXMATRIX* pMatrices = NULL, 
		BBox* pBoxUpdate = NULL);

	virtual BOOL GetRayIntersect(DWORD& dwCount,
		D3DXVECTOR3* pInterPoint,
		DWORD dwMaxNum,
		D3DXVECTOR3 Origin,
		D3DXVECTOR3 Direction);
        
    virtual BOOL GetCameraNearRayIntersect(
        IN  CONST D3DXVECTOR3 vSrc[],
        IN  CONST D3DXVECTOR3 vDst[],
        IN  CONST INT         nNum,
		IN INT *pIgnoreList,
		IN INT nNumIgnoreList,
        OUT D3DXVECTOR3*      pCross,
        OUT       INT*        pIndex
        );	//只对摄像机用，意义是到vDst的最近点
	virtual BOOL RayIntersection(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir
		, FLOAT* pfRet, D3DXVECTOR3* pFaceNormal,KG3DIntersectionFilter* pFilter = NULL);	//只需要和线碰撞的最近点，其它忽略，由vSrc + vNormalizedDir * fRet就可以得到交点坐标。这个函数精度不是很高

	//GetNearRayIntersect的功能由RayIntersection负责
	//virtual BOOL GetNearRayIntersect(const D3DXVECTOR3& vSrc, const D3DXVECTOR3& vNormalizedDir
	//	, FLOAT* pfRet);//只需要和线碰撞的最近点，其它忽略，由vSrc + vNormalizedDir * fRet就可以得到交点坐标;
 
private:	//只在内部转调
	BOOL GetNearRayIntersect_SSE(
		IN  CONST D3DXVECTOR3 vSrc[],
		IN  CONST D3DXVECTOR3 vDst[],
		IN  CONST INT         nNum,
		IN INT *pIgnoreList,
		IN INT nNumIgnoreList,
		OUT D3DXVECTOR3*      pCross,
		OUT         INT*      pIndex
		);
public:
	virtual BOOL IsVertexInside(D3DXVECTOR3& vVertex, 
		D3DXVECTOR3& vDir, 
		KG3DModel* pModel);

	virtual BOOL GetRayIntersectPointAndDir(const D3DXVECTOR3& vec3Origin, 
		const D3DXVECTOR3& vec3Direction,
		D3DXVECTOR3* pIntersectionPoints,
		float* pAngles,
		DWORD& nNumIntersectionPoints,
		const int nBufferSize);	//得到碰转点，求碰撞点Normal，相当耗时间的函数，不要乱用
    //<Billboard Cloud>
public :
    //LPDIRECT3DVERTEXBUFFER9 m_pBillBoardCloundVertexBuffer;
    //DWORD                   m_dwBillBoardCount;
	//</Billboard Cloud>
    
	
	virtual HRESULT SetLOD(float fLod);
	BOOL IsSecondUV();
	
	//////////////////////////////////////////////////////////////////////////
	//基本属性
	DWORD            m_dwOptionEx;//保存MESH数据的扩展选项
	DWORD            m_dwOption;//选项
	unsigned         m_uOption;//加载的参数
	DWORD            m_dwNumSubset;//模型上子物体的数目
	//////////////////////////////////////////////////////////////////////////
	//骨骼和插槽
	std::vector<int> m_nBaseBoneID;//起点骨骼ID
	DWORD            m_dwNumBone;//骨骼数目
	DWORD            m_dwNumSocket;//插槽数目
    DWORD            m_dwMaxFaceInfl;//对于蒙皮模型，每个Attribute涉及到骨骼的最大值
	DWORD            m_dwNumAttributeGroups;//蒙皮模型切分后的自部分数目
	LPD3DXBUFFER     m_pBoneCombinationTable;//骨骼蒙皮信息
	LPD3DXSKININFO   m_pSkinInfo;//骨骼蒙皮信息
	LPD3DXBUFFER     m_pVertexRemap;//顶点重新定位表，用于顶点动画
	//////////////////////////////////////////////////////////////////////////
	//网格
	LPD3DXPMESH      m_pMeshProgress; //优化显示后网格，可能是静止的也可能是骨骼蒙皮的;
	LPD3DXMESH       m_pMeshEnhanced;
	LPD3DXMESH       m_pMeshBeforeProgress;//Mesh before LOD
	LPD3DXMESH       m_ppMeshes[SMBT_COUNT];

	float*           m_pVerticesLodWeight;
	int              m_nNumCurFacesLOD;
	DWORD            m_dwVertexStride;
	DWORD m_dwCurSel;
	BoneInfo1 *m_pBoneInfo; // size is m_NumBone
	Socket1   *m_pSockets;  // size is m_dwNumSocket
	std::string m_scName;
	AABBOX  m_BBox;
protected:
	DynamicState m_eDynamicLoadState;
	KG3DOBJProperty m_objPropertys;
	DWORD m_dwType;//类型
	DWORD m_dwID;
	D3DXMATRIX *m_pmatReverseMatrix; // size is m_NumBone
	ULONG m_ulRefCount;//物品使用计数
	BOOL m_bBBoxChanged;

	KG3DModel* m_pModel;
	LPDIRECT3DVERTEXBUFFER9 m_lpNormal;
	LPDIRECT3DVERTEXBUFFER9 m_lpBinormal;
	LPDIRECT3DVERTEXBUFFER9 m_lpTangent;

	HRESULT OnReferenceZero();
	HRESULT SkinMeshPostProcess();
    const VertexFromatOffsetItem* GetVertexFormat(DWORD dwFVF);
	
private:
    HRESULT _ProcessDelayLoad();
	D3DXVECTOR3 SphereNormalCompute(D3DXVECTOR3& Pos);
	virtual HRESULT HalfSphereNormal(DWORD dwOption);

	//Helper functions for loading mesh
	HRESULT Helper_CreateD3DXMesh(const ReadInMeshData& Data);

	//Load socket info
	HRESULT Helper_LoadSocketFromBuffer(KG3DBufferReader *pBufferReader);

	//Load skin info
	HRESULT _Helper_LoadSkinInfoFromBuffer(KG3DBufferReader *pBufferReader);

	//Load mesh data into memory
    HRESULT Helper_LoadMeshFromBuffer(
        KG3DBufferReader *pBufferReader, ReadInMeshData *pRetMeshData
    );

	//Fill mesh data from read in
	HRESULT Helper_FillVertexData(const ReadInMeshData& Data);
	
	HRESULT DrawMesh(LPD3DXMESH pMesh, 
        KG3DMaterial* pMaterial, 
		float fAlpha, 
		DWORD dwOption,BOOL bUseDetail);

	HRESULT Shader_RenderSkinMesh(D3DXMATRIX* pmatBones, 
		KG3DMaterial* pMaterial, 
		float fAlpha,
		DWORD dwOption);
	/*HRESULT Shader_DrawMeshAmbientMap(LPD3DXMESH pRenderMesh,
		KG3DMaterial* pMaterial,
		float fAlpha,
		DWORD dwOption,BOOL bUseDetail);*/
	HRESULT Helper_CreateSkinMesh();
	HRESULT Helper_CreateSkinInfo();
	DWORD GetVertexStride(DWORD dwFVF);

	

public:
	virtual BOOL IsBBoxChanged();
	
	HRESULT GetName(LPCTSTR *ppszName);
	HRESULT GetType(DWORD *pdwType);
	HRESULT GetObjProperty(BYTE **ppbObj);
	const KG3DOBJProperty& GetObjProperty();

	virtual HRESULT SavePrograssMeshAsLowMesh(LPSTR pName);
    HRESULT CreateBspFile();

private:
	HRESULT _LoadBSPFileFromMemory(IKG_Buffer *piBSPFile);
    HRESULT _LoadPropertyFromIni(IIniFile *piPropertyFile);
    HRESULT _LoadFromMemory(
        IKG_Buffer *piMeshFile, IKG_Buffer *piBSPFile, IIniFile *piPropertyFile, unsigned uOption
    );
	HRESULT _LoadMeshFileFromMemory(KG3DBufferReader *pBufferReader, unsigned uOption);

	static HRESULT Helper_ShaderSetConverMap(LPD3DXEFFECT pShader, BOOL bUseConverTexture,BOOL bSkin,BOOL bUseDetail);
public:
	HRESULT GetBoneFamily(
		DWORD dwBoneIndex,
		DWORD *pdwRetParentIndex, 
		DWORD *pdwRetChildBoneNum,
		const DWORD **ppdwRetChildBoneIndexs
		);

	HRESULT RenderForSelector(KG3DMaterial* pMaterial, 
		void* pExtInfo,
		DWORD dwOption);
	HRESULT Shader_RenderSkinMeshForSelector(D3DXMATRIX *pMatrices, KG3DMaterial *pMaterial, DWORD dwOption);
	HRESULT Shader_RenderSkinMeshForSelector_Begin(D3DXMATRIX* pmatBones);
	HRESULT Shader_RenderSkinMeshForSelector_End(D3DXMATRIX* pmatBones);
	HRESULT Shader_RenderSkinMeshForSelector_NoChange(D3DXMATRIX* pmatBones, 
		DWORD dwOption,
		KG3DMaterial* pMaterial);

	HRESULT Shader_RenderMeshForSelector(KG3DMaterial *pMaterial);
	HRESULT Shader_RenderMeshForSelector_NoChange(KG3DMaterial *pMaterial);
	HRESULT Shader_RenderMeshForSelector_Begin();

	HRESULT PrepareMatrixForSkinMeshRender(int& nNumMatrix,const D3DXMATRIX* matTemp[],DWORD dwOption,int i,D3DXMATRIX* pmatBones,D3DXMATRIX* pIdentify);

private:
    HANDLE m_hMeshFileTask;
    HANDLE m_hBSPFileTask;
    HANDLE m_hPropertyFileTask;  
};

struct RenderTwiceInfo
{
	KG3DMesh *pMesh;
	KG3DMaterial *pMaterial;
	D3DXMATRIX *pmatBones;
	float fAlpha;
	DWORD dwOption;
	KG3DModel *pModel;
};

void g_RenderRenderTwiceInfo(bool bUnderWater, bool bClear);
void g_RenderRenderTwiceInfo();
void g_ClearRenderTwiceInfo();
//void g_RenderSpecialAlphaInfo();
//void PushBackRenderSpecialAlphaInfo(DWORD dwShaderType, 
//									const RenderTwiceInfo &Input);
void RemoveBackRenderSpecialAlphaModel(KG3DModel *pModel);
