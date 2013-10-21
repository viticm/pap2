#pragma once
#include "kg3dinterface.h"
#include "IEEditor.h"//by dengzhihui Refactor 2006年11月8日
#include "kg3deffect.h"
#include "IEKG3DMaterial.h"


#define TEXTURE_TYPE_DIFFUSE "*MAP_DIFFUSE"
#define TEXTURE_TYPE_AMBIENT "*MAP_AMBIENT"

#define MAX_SUBSET_COUNT 16

////////////////////////////////////////////////////////////////////////////////
class KG3DModel;
class KG3DTexture;
class KG3DBufferReader;

struct KG3DMaterialDetail
{
    int m_nIndex;

	D3DCOLORVALUE m_cMainColor;
	D3DCOLORVALUE m_cMainColor1;
	float         m_fMainColorMin;
	float         m_fMainColorMax;
	float         m_fMainColorScale;
	float         m_fEnv;
	float         m_fSpecular;
	string        m_szChannelFileName;

	string m_szDetail0FileName;
	string m_szDetail1FileName;

	KG3DTexture* m_lpChannelTexture;
	KG3DTexture* m_lpDetailTexture0;
	KG3DTexture* m_lpDetailTexture1;

	KG3DTexture* m_lpFlowTexture;//流光效果贴图

	float  m_fDetail0UVScale;
	float  m_fDetail1UVScale;
	float  m_fDetail0ColorScale;
	float  m_fDetail1ColorScale;
	float  m_fDetail0Env;
	float  m_fDetail0Specular;
	float  m_fDetail1Env;
	float  m_fDetail1Specular;

	D3DCOLORVALUE m_cDetial0Color;
	D3DCOLORVALUE m_cDetial1Color;
	string m_szName;
    string m_szTextrueFileName;

	D3DCOLORVALUE m_Flow_Color;
	float  m_fFlow_ColorScale;//流光效果参数
	float  m_fFlow_UVScale;
	D3DXVECTOR2  m_vFlow_Speed;
	string m_szFlow_Texture;

	KG3DMaterialDetail();
	~KG3DMaterialDetail();

	HRESULT SaveDefineToIni(IIniFile* pFile,LPSTR SectionName);
	HRESULT LoadFormIni(IIniFile* pFile,LPSTR SectionName);

	HRESULT LoadTextures();

	BOOL IsResourceReady();

    KG3DMaterialDetail* GetSubSetDetail(BOOL bWholeColorCast, int nID);
    void UnInit();

    KG3DMaterialDetail* m_SubSetDetail[MAX_SUBSET_COUNT];
};

class KG3DMaterial : public IKG3DResourceBase, public IEKG3DMaterial
{
public:
	virtual HRESULT OnLostDevice(){return S_OK;};
	virtual HRESULT OnResetDevice(){return S_OK;};

    virtual HRESULT GetName(LPCTSTR *ppszName);
    virtual HRESULT GetMaterialCount(DWORD *pdwCount);
    virtual HRESULT GetSubMaterialItemAlphaOp(DWORD dwIndex, _MtlAlphaOperation **ppAlphaOp);

    virtual HRESULT SetSubMaterialColorRef2Value(
        DWORD dwIndex, 
        DWORD dwAmbientColor,
        DWORD dwDiffuseColor,
        DWORD dwEmissive,
        DWORD dwSpecular
    );
    virtual HRESULT GetSubMaterialOption(DWORD dwIndex, DWORD *pdwOption);
    virtual HRESULT SetSubMaterialOption(DWORD dwIndex, DWORD dwOption);

    virtual HRESULT GetSubMaterial(DWORD dwIndex, PVOID *ppSubMat);
    virtual HRESULT GetSubMaterialTextureID(DWORD dwIndex, DWORD *pdwTextureIDs);

	virtual HRESULT RemoveTexture(DWORD dwSubsetIndex, 
		DWORD dwIndex);

	virtual HRESULT ChangeTexture(DWORD dwSubsetIndex, 
		DWORD dwIndex, 
		LPCSTR strNewTextureFileName);

	virtual HRESULT GetTexturePathName(DWORD dwSubsetIndex,
		DWORD dwIndex,
		LPSTR strBuffer, 
		DWORD dwSize,
        TEXTURE_LOAD_STATE *peLoadState
    );
public:
    enum 
    {
        MAX_NUM_TEXTURESTAGE = 8,

    };

    enum enuShaderType
    {
        MAT_SHADER_TYPE_NONE, 
        MAT_SHADER_TYPE_SKIN,
        MAT_SHADER_TYPE_NORMAL_NOSKIN,
        MAT_SHADER_FORCE_DWORD = 0xffffffff,
    };

	struct _MtlLogical
	{
		DWORD dwOption;
	};

	struct _MtlCull 
	{
		DWORD CullMode;
		DWORD BakCullMode;
	};

	struct _MtlBlend
	{
		DWORD SrcBlend;
		DWORD DestBlend;
		DWORD BakSrcBlend;
		DWORD BakDestBlend;
		_MtlBlend();
	};
	
	struct _MtlTextureMap
	{
		DWORD TextureMap;
		DWORD BakTextureMap;
		_MtlTextureMap();
		
	};

	struct _MtlTextureOp
	{
		DWORD ColorOp;
		DWORD ColorArg1;
		DWORD ColorArg2;
		DWORD AlphaOp;
		DWORD AlphaArg1;
		DWORD AlphaArg2;
		DWORD TFactor;

		DWORD BakColorOp;
		DWORD BakColorArg1;
		DWORD BakColorArg2;
		DWORD BakAlphaOp;
		DWORD BakAlphaArg1;
		DWORD BakAlphaArg2;
		DWORD BakTFactor;

		_MtlTextureOp();
	};

	//////////////////////////////////////////////////////////////////////////
	//高级的图层操作
	struct _MtlTextureOpEx
	{
		DWORD ColorOp;
		DWORD ColorArg1;
		DWORD ColorArg2;
		DWORD ColorArg0;
		DWORD Result;
		DWORD AlphaOp;
		DWORD AlphaArg1;
		DWORD AlphaArg2;
		DWORD AlphaArg0;
		DWORD AlphaResult;
		DWORD TFactor;
		DWORD Constant;

		DWORD BakColorOp;
		DWORD BakColorArg1;
		DWORD BakColorArg2;
		DWORD BakColorArg0;
		DWORD BakResult;
		DWORD BakAlphaOp;
		DWORD BakAlphaArg1;
		DWORD BakAlphaArg2;
		DWORD BakAlphaArg0;
		DWORD BakTFactor;
		DWORD BakConstant;

		_MtlTextureOpEx();
		
	};	

	struct _MtlTextureTf
	{
		D3DXVECTOR3 Scaling;
		D3DXVECTOR3 Rotation;
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Speed_Scaling;
		D3DXVECTOR3 Speed_Rotation;
		D3DXVECTOR3 Speed_Position;
	};

	struct _MtlTextureData
	{
		std::string FileName;
		DWORD  TextureID;

		_MtlTextureData();
		
		~_MtlTextureData();
		
	};
	struct _MatCull 
	{
		DWORD CullMode;
		DWORD BakCullMode;
	};

	struct _TextureMap
	{
		DWORD TextureMap;
		DWORD BakTextureMap;
		_TextureMap();
	};

	struct _TextureTf
	{
		D3DXVECTOR3 Scaling;
		D3DXVECTOR3 Rotation;
		D3DXVECTOR3 Position;
		D3DXVECTOR3 Speed_Scaling;
		D3DXVECTOR3 Speed_Rotation;
		D3DXVECTOR3 Speed_Position;
	};

	struct _TextureOp
	{
		DWORD ColorOp;
		DWORD ColorArg1;
		DWORD ColorArg2;
		DWORD AlphaOp;
		DWORD AlphaArg1;
		DWORD AlphaArg2;
		DWORD TFactor;
		DWORD BakColorOp;
		DWORD BakColorArg1;
		DWORD BakColorArg2;
		DWORD BakAlphaOp;
		DWORD BakAlphaArg1;
		DWORD BakAlphaArg2;
		DWORD BakTFactor;
		_TextureOp();
	};

	struct _TextureOpEx
	{
		DWORD ColorOp;
		DWORD ColorArg1;
		DWORD ColorArg2;
		DWORD ColorArg0;
		DWORD Result;
		DWORD AlphaOp;
		DWORD AlphaArg1;
		DWORD AlphaArg2;
		DWORD AlphaArg0;
		DWORD AlphaResult;
		DWORD TFactor;
		DWORD Constant;

		DWORD BakColorOp;
		DWORD BakColorArg1;
		DWORD BakColorArg2;
		DWORD BakColorArg0;
		DWORD BakResult;
		DWORD BakAlphaOp;
		DWORD BakAlphaArg1;
		DWORD BakAlphaArg2;
		DWORD BakAlphaArg0;
		DWORD BakTFactor;
		DWORD BakConstant;
		_TextureOpEx();
		
	};	

	struct _MatBlend
	{
		DWORD SrcBlend;
		DWORD DestBlend;
		DWORD BakSrcBlend;
		DWORD BakDestBlend;
		_MatBlend();	
	};

	struct _MtlOption
	{
		DWORD Type;
		PVOID pData;
		_MtlOption();
	
		~_MtlOption();
		
	};

	struct KG3DMaterialSubset
	{
		const static int cNumID = 8;
		const static int cNumColorCast = 8;
		D3DMATERIAL9      m_sMaterial9;
		DWORD            m_dwNumFaces;
		//DWORD            m_dwTextureIDs[cNumID];
		KG3DTexture*     m_lpTextures[cNumID];
		DWORD            m_pTextureNumOption[cNumID];
		DWORD            m_dwOption;
		DWORD            m_dwNumOptions;
		DWORD            m_dwHashValue;//
		DWORD            m_dwNumUsedTexture;
		KG3DTexture*     m_lpNormalMap;

        D3DCOLORVALUE    m_ColorCast[cNumColorCast];
        float            m_fSpecPower;
        float            m_fEmssPower;

		std::string      m_scTextureName[cNumID];
		std::string      m_scTextureType[cNumID];

		std::vector<_MtlOption*> m_pTextureOptions[cNumID];
		std::vector<_MtlOption*> m_pMatOptions;
		
		DWORD m_dwMaterialID;//用来做材质排序用的id
		DWORD m_dwPassID;//使用的Shader的Pass
		KG3DMaterialSubset();
		~KG3DMaterialSubset();
		
        HRESULT LoadFromBuffer(
            const char cszFileName[], KG3DBufferReader *pBufferReader, 
            int *pnRetHasDetail, int *pnRetIsSortAsSFX
        );
        HRESULT SaveToFile(FILE *fpFile, int nHasDetail, int nIsSortAsSFX);
        HRESULT _SaveOption(FILE *fpFile, _MtlOption* pOption);

		DWORD GetAlphaType();
		_MtlAlphaOperation* GetAlphaOperation();
		_MtlRenderTwiceAlphaOption* GetAlphaRenderTwice();
		
		_MtlTextureOpEx* GetTextureStageOperation(DWORD TextIndex);
	
		KG3DTexture* GetSubMeshTexture();
		KG3DTexture* GetSubMeshTextureNormal();
	
		HRESULT SetMaterialEffect(float fAlpha,DWORD dwOption,int nShader,LPD3DXEFFECT pEffect);

        HRESULT SetBillboardCloudMaterial();
        HRESULT RestoreBillboardCloudMaterial();

		HRESULT SetMaterial(float fAlpha,DWORD dwOption);
		HRESULT RestoreMaterial();
		HRESULT SetOption(DWORD dwTextureStage, _MtlOption* pOption);

		HRESULT _LoadTexturOption(KG3DBufferReader *pBufferReader, DWORD dwTextureStageIndex);
		HRESULT _LoadMaterialOption(KG3DBufferReader *pBufferReader);

		void  ClearTextureStage(DWORD dwStageIndex);
		void* GetMtlOption(DWORD dwType, DWORD dwExtInfo);
		void* FindOption(DWORD dwType, DWORD dwExtInfo);

		DWORD ComputeHashValue();

		void CleanMtlOptions();
		void CleanTextureStageOptions();
		void CleanTextureStageOptions(DWORD dwStage);

		_MtlAlphaOperation* m_lpAlphaOperation;
	};

	typedef KG3DMaterialSubset* LPMATERIAL;

	DWORD      m_dwType;//类型
	DWORD      m_dwID;
	DWORD      m_dwNumMaterials;

	int        m_nLastEffect;
	BOOL       m_bSortAsSFX;
	BOOL       m_bHasDetail;
	BOOL       m_bInMultiThreadLoadList;
	UINT       m_uOption;
	ULONG      m_ulRefCount;//物品使用计数
	std::string m_scName;
	KG3DMaterial(void);
	virtual ~KG3DMaterial(void);
	//////////////////////////////////////////////////////////////////////////
	//引用计数
	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		/* [in] */ REFIID riid,
		/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

	//////////////////////////////////////////////////////////////////////////
	//初始化,清除数据
	virtual HRESULT Init();
	virtual HRESULT UnInit();
	//////////////////////////////////////////////////////////////////////////
	//加载,保存
	virtual HRESULT LoadFromFile(const char cszFileName[], UINT uFileNameHash, UINT uOption);
	virtual HRESULT SaveToFile(const char cszFileName[],   UINT uOption);
	virtual HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam)
	{
		return LoadFromFile(cszFileName, uFileNameHash, uOption);
	}
	virtual HRESULT RefreshOption(unsigned uOption);
	//////////////////////////////////////////////////////////////////////////
	//ID,type
	virtual DWORD GetID();
	void SetID(DWORD dwID) { m_dwID = dwID;}
	void SetType(DWORD dwType){m_dwType = dwType;}
	virtual unsigned GetType();
	//////////////////////////////////////////////////////////////////////////
	//内存管理
	virtual HRESULT PrepareRender(unsigned uOption) ;//压缩数据
	virtual HRESULT PostRender();//恢复数据，对象可以立即被使用
	virtual HRESULT GetDateInformation(int* pnMem,int* pnVedioMem,int* pnConMem,int* pnConVMem);//得到数据信息，参数依次为占用内存大小，现存大小，压缩后内存大小，压缩后内存大小
	virtual unsigned   GetLastUseTime();//查询上次使用的时间
	virtual unsigned   GetUseFrequency();//查询使用频率，一段时间里的使用次数

	//////////////////////////////////////////////////////////////////////////
	HRESULT CreateSubset(DWORD dwNumSubset);
	HRESULT LoadFromFileMultiThread();

	//shader type
	virtual void SetShaderType(DWORD dwType);
	virtual DWORD GetShaderType();
	
	BOOL IsLoaded()
	{
		return m_eLoadingState == STATE_LOADCOMPLETE;
	}

	enum MtlLoadState
	{
		STATE_NONE,
		STATE_MULTITHREADFINISH,
		STATE_LOADCOMPLETE,
	}m_eLoadingState;

	vector<KG3DMaterialDetail*>m_vecDetailDefine;
	virtual int GetMaterialDetailDefineCount();
	virtual HRESULT AddMaterialDetailDefine(int nIndex);
	virtual HRESULT DeleteMaterialDetailDefine(int nIndex);

	HRESULT SaveMaterialDetails(LPSTR pFileName);
	HRESULT LoadMaterialDetails(LPSTR pFileName);

protected:
	HRESULT _LoadFromFileDirect(const char cszFileName[], unsigned uOption);
	virtual HRESULT OnReferenceZero();
private:
	DWORD m_ShaderType;

	LPMATERIAL m_pMaterials;
public:
	BOOL m_bTextureLoaded;

	virtual DWORD GetNumMaterial( ) { return m_dwNumMaterials; }

	KG3DMaterialSubset*  GetSubMaterial(DWORD dwIndex);
    LPDIRECT3DTEXTURE9  GetSpeedTreeLeafTexture(DWORD MaterialID);
	void CopyOption(DWORD dwType, DWORD dwExtInfo, unsigned int nSrc, unsigned int nDest);

	HRESULT SetSubsetTexture(unsigned int uSubsetID, 
		unsigned int uStageID,
		KG3DTexture* pTexture);

	virtual BOOL IsResourceReady(BOOL bTestTexture);

	HRESULT _LoadTexture(DWORD dwOption);
	
	void _ModifyLoadOption(DWORD &dwTextureLoadOption, 
		DWORD dwSubsetID);

	virtual BOOL IsSortAsSFX(){return m_bSortAsSFX;};
	virtual HRESULT SetSortAsSFX(BOOL bSort){m_bSortAsSFX = bSort;return S_OK;};

	virtual HRESULT GetMaterilDetail(int nIndex,KG3DMaterialDetail* pDest);
    HRESULT SetMaterialDetail(KG3DMaterialDetail* pDest, KG3DMaterialDetail* pSource);

	virtual HRESULT GetMaterilDetail(int nDetailType, int nSubSetID, LPSTR pszName, LPSTR pszTextureFileName, float* pfEnv,float* pfSpecular,
		float* pBodyMax,float* pBodyMin,float* pBodyScale,D3DCOLORVALUE* pBodyColor,D3DCOLORVALUE* pBodyColor1,LPSTR pChannelFile,
		float* pDetail0_ColorScale,float* pDetail0_UVScale,float* pfDetail0_Env,float* pfDetail0_Specular,D3DCOLORVALUE* pDetail0_Color,LPSTR pDetail0_File,
		float* pDetail1_ColorScale,float* pDetail1_UVScale,float* pfDetail1_Env,float* pfDetail1_Specular,D3DCOLORVALUE* pDetail1_Color,LPSTR pDetail1_File,
		D3DCOLORVALUE* pFlow_Color,float* pfFlow_ColorScale,float* pfFlow_UVScale,D3DXVECTOR2* pvSpeed,LPSTR pFlowTexture);

	virtual HRESULT SetMaterilDetail(int nDetailType, int nSubSetID, LPSTR pszName, LPSTR pszTextureFileName, float fEnv,float fSpecular,
		float fBodyMax,float fBodyMin,float fBodyScale,D3DCOLORVALUE vBodyColor,D3DCOLORVALUE vBodyColor1,LPSTR pChannelFile,
		float fDetail0_ColorScale,float fDetail0_UVScale,float fDetail0_Env,float fDetail0_Specular,D3DCOLORVALUE vDetail0_Color,LPSTR pDetail0_File,
		float fDetail1_ColorScale,float fDetail1_UVScale,float fDetail1_Env,float fDetail1_Specular,D3DCOLORVALUE vDetail1_Color,LPSTR pDetail1_File,
		D3DCOLORVALUE Flow_Color,float fFlow_ColorScale,float fFlow_UVScale,D3DXVECTOR2 vSpeed,LPSTR pFlowTexture);

    virtual void SetDetailWholeColorCast(BOOL bWholeColorCast) { m_bDetailWholeColorCast = bWholeColorCast; return; }
    virtual BOOL IsSubSetDetailExist(int nIndex);
    BOOL GetDetailWholeColorCast() {return m_bDetailWholeColorCast;}

    virtual int GetMaxMtlIndex()
    {
        return m_nMaxMtlIndex;
    }

    virtual int FindIndex(int nIndex);
    virtual HRESULT CopyTextureToFile(LPCSTR pszSourceFile, LPCSTR pszDestFile);
    virtual HRESULT MoveMaterialDetails(int nIndex);
    virtual string GetOrgTextureFileName();
    virtual HRESULT SetOrgTextureFileName(LPCSTR pszNewFileName);
private:
	LPMATERIAL m_pMaterialsMulti;
	DWORD      m_dwNumMaterialsMulti;
	vector<KG3DMaterialDetail>m_vecDetailDefineMulti;

	BOOL bTest;
    int  m_nMaxMtlIndex;
    string* m_pstrTextureName;

    BOOL m_bDetailWholeColorCast; // 整体偏色
    string m_strOrgTextureFilePath;
};


extern float g_fHightLight;

