////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DModelSpeedTree.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-10-19 11:56:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DMODELSPEEDTREE_H_
#define _KG3DMODELSPEEDTREE_H_

#include "IEKG3DModelSpeedTree.h"
#include "KG3DModel.h"
#include "KG3DGraphicsTool.h"
#include "Engine/KMutex.h"
#include "KG3DShaderHolder.h"

class KG3DSceneEditorBase;
class KG3DModelSpeedTree;
class KG3DSpeedTreeLeafGroup;
class KG3DSceneEntity;
class Leaf;
typedef Leaf* LPLEAF;
struct KG3DTerrainConverMap;
struct KG3DSpeedTreeLeafsData;

struct KG3DSpeedTreeLeafsTable
{
	KMutex m_Lock;
	map<UINT,KG3DSpeedTreeLeafsData*>s_mapLeafsData;

	KG3DSpeedTreeLeafsData* RequestLeafsData(BOOL& bReUsed,UINT uFileHashID);
	HRESULT FreeLeafsData(KG3DSpeedTreeLeafsData* pData);

	KG3DSpeedTreeLeafsTable();
	~KG3DSpeedTreeLeafsTable();
};

struct LeafData
{
	D3DXVECTOR3     OriginPosition;//for old speedtree version, this is the offset from the parent bone, 
								   //for new spdtree version, this is offset from the root
	D3DXVECTOR3     Position;
	D3DXVECTOR3     Normal;
	D3DXVECTOR3     NormalOffset;
	D3DCOLORVALUE   Ambient;
	D3DCOLORVALUE   Diffuse;
	D3DXVECTOR3     YawPitchRoll;
	D3DXVECTOR3     YawPitchRollSpeed;
	D3DXVECTOR3     YawPitchRollAccelerate;
	float           Wave;
	float           Size;
	int             BoneID;
	int             TextureUV;
	LeafData()
	{
		OriginPosition           = D3DXVECTOR3(0,0,0);
		Position                 = D3DXVECTOR3(0,0,0);
		Normal                   = D3DXVECTOR3(0,0,0);
		NormalOffset             = D3DXVECTOR3(0,1,0);
		Ambient.a                = 1.0f;
		Ambient.r                = 0.6f;
		Ambient.g                = 0.6f;
		Ambient.b                = 0.6f;
		Diffuse                  = Ambient;
		YawPitchRoll             = D3DXVECTOR3(0,0,0);
		YawPitchRollSpeed        = D3DXVECTOR3(0,0,0);
		YawPitchRollAccelerate   = D3DXVECTOR3(0,0,0);
		Wave                     = 1;
		Size                     = 50;
		BoneID                   = 0;
		TextureUV                = 0;
	};
};

class Leaf : public LeafData, public IELeaf
{
    friend class KG3DModelSpeedTree;
    friend class KG3DSpeedTreeLeafGroup;
    friend class KG3DSceneEntity;
    friend bool Small(LPLEAF A, LPLEAF B);
public://IE interface
    virtual HRESULT SetTextureUV(INT nTextureUV);
    virtual HRESULT SetSize(FLOAT fSize);
    virtual HRESULT SetWave(FLOAT fWave);
    virtual HRESULT SetAmbient(D3DCOLORVALUE *pColor);
    virtual HRESULT SetDiffuse(D3DCOLORVALUE *pColor);
    virtual HRESULT GetBoneID(INT *pnBoneID);
    virtual HRESULT GetSelectState(BOOL *pbSelected);

public:

    KG3DModelSpeedTree* m_pSpeedTree;
    
    Leaf(KG3DModelSpeedTree* pTree)
    {
        m_pSpeedTree = pTree;
        bSelected = FALSE;
    }
    void ConvertToWorldCoordinate();


    HRESULT Render();
    HRESULT RenderSelectMark(DWORD dwColor = 0xFFFF0000);
    HRESULT OnDelete();
    HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);	//<PROCESS_MESSAGE>

    virtual HRESULT UpdateLeafOriginPosition();
    void SetDirtyFlag();
    
    BOOL bSelected;
};

struct _LEAF_VERTEX
{
	float x, y, z;
	float nx, ny, nz;
	float ox, oy;
	float tu, tv;
};
////////////////////////////////////////////////////////////////////////////////
struct KG3DSpeedTreeLeafsData
{
//private:
	std::vector<LPLEAF> m_ListLeaf;
public:
	//string      m_strPropertyFileName;
	//KG3DOBJProperty m_objPropertys;


	float m_fRollLeft;
	float m_fRollRight;
	float m_fMaxRollSpeed;
	float m_fRollAccelerate;

	float m_fPitchLimit;
	float m_fMaxPitchSpeed;
	float m_fPitchAccelerate;

	LPDIRECT3DVERTEXBUFFER9 m_lpLeafVertexBuffer;
	BOOL m_bNeedRecomputePostionByBone;

	UINT  m_uFileHashID;

	HRESULT ClearLeafs();
	KG3DSpeedTreeLeafsData(UINT uHashID);
	~KG3DSpeedTreeLeafsData();

	ULONG m_ulRefCount;
	BOOL  m_bLoaded;
	ULONG STDMETHODCALLTYPE AddRef();
	ULONG STDMETHODCALLTYPE Release();

	DWORD GetLeafCount()
	{
		return (DWORD)m_ListLeaf.size();
	}

	HRESULT FillLeafVerticesBuffer();
	HRESULT CreateVerticesBuffer();
	HRESULT LoadFromFile(IFile* pFile,PVOID pHead,KG3DModelSpeedTree* pTree);
	HRESULT ComputeLeafNormal(D3DXVECTOR3 vCenter);
	HRESULT ConvertToWorldCoordinate(KG3DModelSpeedTree* pSpeedTree);
	HRESULT AddLeaf(LPLEAF pLeaf);
};

//////////////////////////////////////////////////////////////////////////
typedef vector<LPLEAF>::iterator LEAFIter_t;

class KG3DModelSpeedTree : public KG3DModel, public IEKG3DModelSpeedTree
{
    friend class Leaf;
	friend class KG3DSpeedTreeLeafsData;
public:
	virtual BOOL IsLoaded();

	virtual HRESULT UnInit();

	virtual HRESULT OnLostDevice();
	virtual HRESULT OnResetDevice();

    HRESULT RenderLeafs();

	virtual BOOL IsNeedFrameMove();

    virtual HRESULT SetPointLightInfo(KG3D_PointLight_Info& vInfo);
	HRESULT RenderLeafsBegin(int nPass,KG3DLight& rLight,KG3DTerrainConverMap* pConverMap,BOOL bUpdateTime,float fTrans);
	HRESULT RenderLeafsEnd();
	HRESULT RenderLeafsWithoutEffectAndMtlChange();
    HRESULT ApplyLeafsMaterial();

    virtual HRESULT GetLeafVectorSize(DWORD *pdwVectorSzie);
    virtual HRESULT GetLeafVectorItem(DWORD dwIndex, IELeaf **ppiLeaf);
    virtual HRESULT GetModelTree(IEKG3DModel **ppiModel);
    virtual HRESULT GetNeedUpdatePositionState(BOOL *pbNeed);
    virtual HRESULT SetNeedUpdatePositionState(BOOL bNeed);
    virtual HRESULT SetNeedDrawBone(BOOL bNeed);
    //virtual HRESULT GetFullName(LPCTSTR *ppszFullName);

    virtual HRESULT SetPitchLimit(FLOAT fPitch);
    virtual HRESULT GetPitchLimit(FLOAT *pfPitch);
    virtual HRESULT SetRollLeft(FLOAT fRollLeft);
    virtual HRESULT SetRollRight(FLOAT fRollRight);
    virtual HRESULT GetRollRight(FLOAT *pfRollRight);
    virtual HRESULT SetMaxRollSpeed(FLOAT fMaxRollSpeed);
    virtual HRESULT GetMaxRollSpeed(FLOAT *pfMaxRollSpeed);
    virtual HRESULT SetMaxPitchSpeed(FLOAT fMaxPitchSpeed);
    virtual HRESULT SetRollAccelerate(FLOAT fRollAccelerate);
    virtual HRESULT GetRollAccelerate(FLOAT *pfRollAccelerate);
    virtual HRESULT SetPitchAccelerate(FLOAT fPitchAccelerate);

    virtual DWORD GetVertexNum();
    virtual DWORD GetFaceNum();

	//virtual BOOL IsLoaded();

public:
    struct SpeedTreeFileHeader
    {
        DWORD dwMark;
        char  szTreeMeshName[256];
        char  szTreeMtlName [256];
        char  szTreeAniName [256];

        DWORD dwNumLeaf;

        float fRollLeft;
        float fRollRight;
        float fMaxRollSpeed;
        float fRollAccelerate;

        float fPitchLimit;
        float fMaxPitchSpeed;
        float fPitchAccelerate;

        DWORD Extend[16];

        SpeedTreeFileHeader()
        {
            ZeroMemory(this, sizeof(SpeedTreeFileHeader));
			dwMark = dwVersion1;
        }

        void Input(KG3DModelSpeedTree* pSpeedTree)
        {
            dwMark = dwVersion1;
            wsprintf(szTreeMeshName,"%s", pSpeedTree->m_szTreeMeshName.c_str());
            wsprintf(szTreeMtlName ,"%s", pSpeedTree->m_szTreeMtlName .c_str());
            wsprintf(szTreeAniName ,"%s", pSpeedTree->m_szTreeAniName .c_str());

            dwNumLeaf        = (DWORD)pSpeedTree->m_lpLeafsData->GetLeafCount();
            fRollLeft        = pSpeedTree->m_lpLeafsData->m_fRollLeft;
            fRollRight       = pSpeedTree->m_lpLeafsData->m_fRollRight;
            fMaxRollSpeed    = pSpeedTree->m_lpLeafsData->m_fMaxRollSpeed;
            fRollAccelerate  = pSpeedTree->m_lpLeafsData->m_fRollAccelerate;
            fPitchLimit      = pSpeedTree->m_lpLeafsData->m_fPitchLimit;
            fMaxPitchSpeed   = pSpeedTree->m_lpLeafsData->m_fMaxPitchSpeed;
            fPitchAccelerate = pSpeedTree->m_lpLeafsData->m_fPitchAccelerate;
        }

        void OutPut(KG3DModelSpeedTree* pSpeedTree)
        {
            pSpeedTree->m_szTreeMeshName   = szTreeMeshName;
            pSpeedTree->m_szTreeMtlName    = szTreeMtlName;
            pSpeedTree->m_szTreeAniName    = szTreeAniName;

            pSpeedTree->m_lpLeafsData->m_fRollLeft        = fRollLeft;
            pSpeedTree->m_lpLeafsData->m_fRollRight       = fRollRight;
            pSpeedTree->m_lpLeafsData->m_fMaxRollSpeed    = fMaxRollSpeed;
            pSpeedTree->m_lpLeafsData->m_fRollAccelerate  = fRollAccelerate;
            pSpeedTree->m_lpLeafsData->m_fPitchLimit      = fPitchLimit;
            pSpeedTree->m_lpLeafsData->m_fMaxPitchSpeed   = fMaxPitchSpeed;
            pSpeedTree->m_lpLeafsData->m_fPitchAccelerate = fPitchAccelerate;
        }
    };

	

	float m_fLeafScale;
public:
	virtual HRESULT FrameMove();
	virtual void SetParentObjMatrix(D3DXMATRIX* pMatrix);

	virtual HRESULT SetAlpha(float fAlpha, BOOL bUseSpecail);

	virtual HRESULT ChangeLeafTexture(LPCTSTR strFileName);

	virtual HRESULT SetEditMode(BOOL bEditMode, IEKG3DSceneModelEditor* pSceneA);

	virtual HRESULT RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);

	virtual void GetProfile(DWORD& dwNumFacesTotal,DWORD& dwNumVertices,DWORD& dwNumTextures,DWORD& dwNumID);

	virtual HRESULT FillLeafVerticesBuffer();

	virtual HRESULT CreateAuto(int nLeaf, D3DXVECTOR3 *pScale);
    HRESULT CreateVerticesBuffer();
    HRESULT UpdateVerticesBuffer(UINT uNumLeaf,unsigned int uOption);
    
    HRESULT LoadTreeMesh(
        const char cszFileName[], unsigned uFileNameHash, unsigned uOption,BOOL MultiThread
    );
    HRESULT LoadTreeMtl(
        const char cszFileName[], unsigned uFileNameHash, unsigned uOption
    );
    HRESULT LoadTreeAni(
        const char cszFileName[], unsigned uFileNameHash, unsigned uOption
    );
    
    HRESULT LoadFromFile(
        const char cszFileName[], unsigned uFileNameHash, unsigned uOption
    );

    LPLEAF Add1Leaf(int BoneIndex);
    HRESULT DeleteLeaf(LPLEAF pLeaf);


    HRESULT Render(unsigned int uOption, void* pExtInfo);
    //HRESULT RenderShadowMap(LPD3DXEFFECT &lpEffect,const D3DXMATRIX &matVP,const D3DXMATRIX &matWVP);
    virtual HRESULT SaveLeaf(LPCSTR pFileName);
	virtual HRESULT SaveToFile(const TCHAR cszFileName[], UINT uOption);
    

    //HRESULT UpdateLeafPosition(UINT uNumLeaf);

    HRESULT PlaneShadowRender();

    virtual HRESULT Copy(KG3DModel** ppNewModel);

	virtual HRESULT GetMeshFileName(LPSTR pFileName);
	virtual HRESULT GetMaterialFileName(LPSTR pFileName);
	virtual HRESULT GetAnimationFileName(LPSTR pFileName);

    virtual BOOL IsBBoxChanged();
    virtual void SetBBoxChanged(BOOL bBBoxChanged);
    virtual HRESULT ComputeBBox();

	virtual HRESULT LoadLeafFromFile(LPCTSTR strFileName);

	virtual HRESULT Init(IEKG3DModel* pModel);

	HRESULT RenderTreeModel();
public:

	void ReclusterLeaf();
    //void  UpdatePositionBufferShadowMap( UINT leafNums);

public :

	KG3DSpeedTreeLeafsData *m_lpLeafsData;

    // Object Propertys
    virtual HRESULT LoadPropertyFromIni(const char cszFileName[]);
    virtual HRESULT SavePropertyToIni(const char cszFileName[]);


public:
	static LPDIRECT3DVERTEXDECLARATION9    m_lpVertexDeclaration;
	
	string m_szSpeedTreeName;
	string m_szSpeedTreeMtlName;

    string m_szTreeMeshName;
    string m_szTreeMtlName;
    string m_szTreeAniName;

	UINT        m_uFileHashID;//该speedtree文件所对应的ID,主要用来做优化处理

    KG3DModel*  m_lpModelTree;

    BOOL m_bNeedUpdataPosition;
    BOOL m_bNeeDrawBone;

    /**
     * speed tree billboard texture
     */

    enum {
        TREE_FRONT,
        TREE_BACK,
        TREE_LEFT,
        TREE_RIGHT,
        TREE_UP,

        TREE_DIR_NUM
    };

    struct _TexArray {
        IDirect3DTexture9* TexArray[TREE_DIR_NUM];
        _TexArray() {
            memset(TexArray, 0, sizeof(TexArray));
        }
        IDirect3DTexture9*& operator[] (int index) {
            return TexArray[index];
        } 
        std::vector<KG3DModelSpeedTree*> vTreeRefs[TREE_DIR_NUM];
    };

    struct _TreeTexInfo {
        DWORD dwHash;
        D3DXQUATERNION q;
    };

   
    static std::map<_TreeTexInfo, _TexArray> m_mapBillBoardTexs;
    static LPDIRECT3DVERTEXBUFFER9 m_pBillBoardBuf;

    enum {BILLBOARD_BUF_NUM = 50};

	int    m_nUpdata;


public:

    DWORD QueryCameraLookAtFace(); 
    IDirect3DTexture9* GetBillboareTex(DWORD dwFace);
    bool IsFaraway();
    HRESULT UpdateBillboardBuf( VFormat::FACES_DIFFUSE_TEXTURE1* pVer, size_t index, DWORD dwFace);
    HRESULT RenderBillboard();

    static HRESULT InitBillBoardTree();
    static HRESULT AddBillBoardTree(KG3DModelSpeedTree* pTree);
    static HRESULT RenderBillboardTree();
    static HRESULT ClearOldTreeTex();

    static HRESULT OnLostDeviceStatic();
    static HRESULT OnResetDeviceStatic();


	static HRESULT InitStaticResource();
	static HRESULT UnInitStaticResource();

    void SetLeafScaling(const D3DXVECTOR3* pValue);
    KG3DModelSpeedTree(void);
    ~KG3DModelSpeedTree(void);

	virtual HRESULT SetLocalBBoxDirty();
	HRESULT GetBoneMatrix(unsigned int uIndex, 
		D3DXMATRIX& Mat, 
		D3DXVECTOR3 *pvec3LocalOffset = NULL);
	
	HRESULT GetSocketMatrix(unsigned int uIndex, 
		D3DXMATRIX& mat,
		D3DXVECTOR3* pvec3LocalOffset  = NULL);
	
	void UpdateLeafOriginPosition();

	void GetBoneWorldMatrix(D3DXMATRIX& matBone, unsigned int uIndex);

    virtual HRESULT GetObjProperty(BYTE **ppbObj);
	void SetTranslation(D3DXVECTOR3 *pValue);
	void SetRotation(D3DXQUATERNION *pValue);
	void SetScaling(D3DXVECTOR3 *pValue);
protected:
	float m_fLastLeafScale;
	////////////////////////////////////////////////
	UINT m_nOption;
	BOOL m_bEditMode;

   //////////////////////////////////////////////////


    //////////////////////////////////////////////////
	const static DWORD dwVersion0 = 0xFFAAEEDD;
	const static DWORD dwVersion1 = 0xFFAAEEDE;
private:


	HRESULT LoadFromFileVersion0(FILE *fpFile, 
		SpeedTreeFileHeader *pHeader,
		unsigned uFileNameHash, 
		unsigned uOption);

	HRESULT LoadFromFileVersion1(FILE *fpFile, 
		SpeedTreeFileHeader *pHeader,
		unsigned uFileNameHash, 
		unsigned uOption);

	HRESULT FillLeafVerticesBufferVersion1();
	void Helper_ReclusterLeaf(Leaf& L);
	void ExcludeLeaf();
	void CheckLeafSpan(const Leaf *pLeaf, size_t uEndIndex, float fThreshold);
	BOOL m_bNeedUpdateVB;

	HRESULT TryUpdataLeafVertexBuffer();


	HRESULT _LoadFromFile( 
		const char cszFileName[], 
		unsigned uFileNameHash, 
		unsigned uOption 
		);

	virtual HRESULT LoadFromFileMultiThread();

    virtual HRESULT LoadPostProcess();
};

extern BOOL g_bRenderTree;
extern float g_fSpeedTreeLeafScale;
#endif // _KG3DMODELSPEEDTREE_H_